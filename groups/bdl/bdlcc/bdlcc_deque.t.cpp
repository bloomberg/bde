// bdlcc_deque.t.cpp                                                  -*-C++-*-

#include <bdlcc_deque.h>

#include <bslmt_barrier.h>
#include <bslmt_configuration.h>
#include <bslmt_lockguard.h>
#include <bslmt_semaphore.h>
#include <bslmt_testutil.h>
#include <bslmt_threadutil.h>
#include <bslmt_threadgroup.h>
#include <bsls_systemtime.h>

#include <bslalg_typetraitbitwisecopyable.h>
#include <bslalg_typetraitbitwisemoveable.h>
#include <bslalg_typetraithasstliterators.h>

#include <bsltf_streamutil.h>

#include <bslalg_constructorproxy.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_newdeleteallocator.h>
#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_atomic.h>
#include <bsls_nameof.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_deque.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>

#include <bsl_c_ctype.h>

using namespace BloombergLP;
using bsls::NameOf;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

#if defined(BDE_BUILD_TARGET_EXC)
// The following enum is set to '1' when exceptions are enabled and to '0'
// otherwise.  It is here to avoid having preprocessor macros throughout.
enum { PLAT_EXC = 1 };
#else
enum { PLAT_EXC = 0 };
#endif

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides a thread-enabled proxy to the 'bsl::deque'
// container.  The purpose of this test driver is to assert that each operation
// is properly "hooked up" to its respective 'bsl::deque' operation, and that
// the locking mechanisms work as expected when the boundary conditions on
// length and high water mark are reached.  In addition, although all the
// memory allocated is allocated by the underlying 'bsl::deque', we want to
// make sure that the allocator is correctly passed to it.  The component is
// tested in a single thread by the breathing test.  In the rest of the test
// cases, we use multiple threads and test the locking and concurrency
// mechanisms:
//
// Single threaded tests are marked '- st', multi-threaded tests are marked
// '- mt'.
//
//-----------------------------------------------------------------------------
// TYPES
// [ 7] ConstProctor
// [ 7] Proctor
//
// CLASS METHODS
// [ 2] static size_t maxSizeT();
//
// CREATORS
// [ 2] Deque(bslma::Allocator *basicAllocator = 0);
// [ 5] Deque(int highWaterMark, Alloc *alloc = 0);
// [21] Deque(INPUT_ITER, INPUT_ITER, Alloc *);
// [16] Deque(INPUT_ITER, INPUT_ITER, size_t, Alloc *);
// [23] Deque();
// [23] Deque(alloc);
// [23] Deque(clock);
// [23] Deque(clock, alloc);
// [23] Deque(hwm);
// [23] Deque(hwm, alloc);
// [23] Deque(hwm, clock);
// [23] Deque(hwm, clock, alloc);
// [23] Deque(ITER, ITER);
// [23] Deque(ITER, ITER, alloc);
// [23] Deque(ITER, ITER, clock);
// [23] Deque(ITER, ITER, clock, alloc);
// [23] Deque(ITER, ITER, hwm);
// [23] Deque(ITER, ITER, hwm, alloc);
// [23] Deque(ITER, ITER, hwm, clock);
// [23] Deque(ITER, ITER, hwm, clock, alloc);
// [ 2] ~Deque();
//
// MANIPULATORS
// [16] void forcePushBack(INPUT_ITER, INPUT_ITER); - st
// [16] void forcePushFront(INPUT_ITER, INPUT_ITER); - st
// [17] void forcePushBack(const T&); - mt
// [17] void forcePushBack(INPUT_ITER, INPUT_ITER); - mt
// [17] void forcePushFront(const T&); - mt
// [17] void forcePushFront(INPUT_ITER, INPUT_ITER); - mt
// [17] void forcePushBack(T&&); - mt
// [17] void forcePushFront(T&&); - mt
// [19] void forcePushBack(const T&); - st
// [19] void forcePushFront(const T&); - st
// [19] void forcePushBack(T&&); - st
// [19] void forcePushFront(T&&); - st
// [19] void removeAll(vector<T> *); - st
// [ 2] void pushFront(const TYPE&); - st
// [ 2] void pushBack(const TYPE&); - st
// [ 2] TYPE popFront(); - st
// [ 2] void popFront(TYPE *); - st
// [ 2] TYPE popBack(); - st
// [ 2] void popBack(TYPE *); - st
// [ 3] T popBack(); - mt
// [ 3] void popBack(TYPE *); - mt
// [ 3] T popFront(); - mt
// [ 3] void popFront(TYPE *); - mt
// [ 3] void pushBack(const T&); - mt
// [ 3] void pushFront(const T&); - mt
// [15] T popBack(); - mt
// [15] void popBack(T *); - mt
// [15] T popFront(); - mt
// [15] void popFront(T *); - mt
// [15] void pushFront(const TYPE&); - mt
// [15] void pushBack(const TYPE&); - mt
// [15] void pushFront(TYPE&&); - mt
// [15] void pushBack(TYPE&&); - mt
// [11] void pushFront(const TYPE&); - mt
// [11] void pushBack(const TYPE&); - mt
// [11] void pushFront(TYPE&&); - mt
// [11] void pushBack(TYPE&&); - mt
// [11] int timedPopBack(TYPE *, const TimeInterval&); - mt
// [11] int timedPopFront(TYPE *, const TimeInterval&); - mt
// [11] int timedPushBack(TYPE&&, const TimeInterval&); - mt
// [11] int timedPushFront(TYPE&&, const TimeInterval&); - mt
// [ 4] int timedPopBack(TYPE *, const TimeInterval&); - mt
// [ 6] int timedPopBack(TYPE *, const bsls::TimeInterval&);
// [10] int timedPopBack(TYPE *, const bsls::TimeInterval&);
// [ 4] int timedPopFront(TYPE *, const TimeInterval&); - mt
// [ 6] int timedPopFront(TYPE *, const bsls::TimeInterval&);
// [10] int timedPopFront(TYPE *, const bsls::TimeInterval&);
// [ 6] int timedPushBack(const T&, const TimeInterval &);
// [ 6] int timedPushFront(const T&,  const TimeInterval &);
// [ 8] removeAll();
// [ 8] removeAll(bsl::vector<T> *buffer);
// [ 9] int tryPopFront(TYPE *); - st
// [ 9] void tryPopFront(size_t, vector<TYPE> *); - st
// [ 9] int tryPopBack(TYPE *); - st
// [ 9] void tryPopBack(size_t, vector<TYPE> *); - st
// [12] int tryPopFront(TYPE *); - mt
// [12] void tryPopFront(size_t, vector<TYPE> *); - mt
// [12] void tryPopFront(size_t, vector<TYPE> *, bool); - mt
// [12] int tryPopBack(TYPE *); - mt
// [12] void tryPopBack(size_t, vector<TYPE> *); - mt
// [12] void tryPopBack(size_t, vector<TYPE> *, bool); - mt
// [16] int tryPushBack(const T&); - st
// [16] int tryPushFront(const T&); - st
// [16] int tryPushBack(T&&); - st
// [16] int tryPushFront(T&&); - st
// [16] int tryPushBack(const T&); - mt
// [16] int tryPushFront(const T&); - mt
// [18] void tryPushBack(INPUT_ITER, INPUT_ITER); - st
// [18] void tryPushFront(INPUT_ITER, INPUT_ITER); - st
// [20] void tryPushBack(INPUT_ITER, INPUT_ITER); - mt
// [20] void tryPushFront(INPUT_ITER, INPUT_ITER); - mt
// [22] bslma::UsesBslmaAllocator
// [25] void pushFront(TYPE&&); - st
// [25] void pushBack(TYPE&&); - st
// [25] void tryPushFront(TYPE&&); - st
// [25] void tryPushBack(TYPE&&); - st
// [25] TYPE popFront(); - move semantics
// [25] void popFront(TYPE *); - move semantics
// [25] TYPE popBack(); - move semantics
// [25] void popBack(TYPE *); - move semantics
// [26] int timedPopBack(TYPE *, const TimeInterval&); - move semantics
// [26] int timedPopFront(TYPE *, const TimeInterval&); - move semantics
// [26] int timedPushBack(TYPE&&, const TimeInterval&);
// [26] int timedPushFront(TYPE&&, const TimeInterval&);
//
// ACCESSORS
// [23] bslma::Allocator *allocator() const;
// [23] bsls::SystemClockType::Enum clockType() const;
// [ 5] size_t highWaterMark() const;
// [ 2] size_t length() const; - st
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [24] PROCTOR LIFETIME
// [27] USAGE EXAMPLE 1
// [28] USAGE EXAMPLE 2
// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

bsls::AtomicInt testStatus(0);

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

#define ASSERT       BSLMT_TESTUTIL_ASSERT
#define ASSERTV      BSLMT_TESTUTIL_ASSERTV

#define Q            BSLMT_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLMT_TESTUTIL_P   // Print identifier and value.
#define P_           BSLMT_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLMT_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLMT_TESTUTIL_L_  // current Line number

#define TAB cout << '\t';

#define PP(X) (cout << #X " = " << (X) << endl, false) // Print identifier and
                                         // value, return false, as expression.

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                                    GLOBALS
//-----------------------------------------------------------------------------

int test;
int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

bslma::TestAllocator sa("scratch");
bslma::Allocator&    nda = bslma::NewDeleteAllocator::singleton();

//=============================================================================
//                GLOBAL TYPEDEFS/CONSTANTS/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsltf::TemplateTestFacility  TTF;
typedef double                       Element;
typedef bdlcc::Deque<Element>        Obj;
typedef bsltf::AllocTestType         AElement;
typedef bdlcc::Deque<AElement>       AObj;
typedef bsltf::MovableAllocTestType  MElement;
typedef bdlcc::Deque<MElement>       MObj;
typedef bsl::vector<AElement>        AVec;
typedef bsl::vector<MElement>        MVec;
typedef bsl::size_t                  size_t;
typedef Obj::Proctor                 Proctor;
typedef Obj::ConstProctor            ConstProctor;
typedef bsls::Types::Int64           Int64;
typedef bsls::Types::Uint64          Uint64;
typedef bsls::Types::IntPtr          IntPtr;
typedef bsltf::MoveState             MoveState;
typedef bslmf::MovableRefUtil        MUtil;

static const bsl::size_t      maxSizeT = ~static_cast<bsl::size_t>(0);

static const double DECI_SEC      = 0.1;
                                    // 1 deci second (i.e., 1/10th of a second)

static const int MICRO_100TH_SEC =    10000;
                                       // number of micro seconds in .1 seconds

static const bsltf::MoveState::Enum e_NOT_MOVED = MoveState::e_NOT_MOVED;
static const bsltf::MoveState::Enum e_MOVED     = MoveState::e_MOVED;
static const bsltf::MoveState::Enum e_UNKNOWN   = MoveState::e_UNKNOWN;

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
const bsltf::MoveState::Enum expAssignMoveState = e_MOVED;
const bool                   expAssignMove      = true;
#else
const bsltf::MoveState::Enum expAssignMoveState = e_NOT_MOVED;
const bool                   expAssignMove      = false;
#endif

//=============================================================================
//                  SUPPORT CLASSES AND FUNCTIONS USED FOR TESTING
//-----------------------------------------------------------------------------

namespace {
namespace u {

enum VecType { e_BEGIN,
               e_BSL = e_BEGIN,
               e_STD,
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
               e_PMR,
#endif
               e_END
};

template <class VECTOR>
typename VECTOR::value_type *vData(VECTOR *v)
    // Provide access similar to 'vector::data()'.  According to
    // cppreference.com, the 'vector::data()' manipulator is provided on all
    // versions of C++, but we find that it is supported on neither Solaris nor
    // AIX.
{
    return v->empty() ? 0 : &(*v)[0];
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bsltf {
                        // ===========================
                        // class MoveCopyAllocTestType
                        // ===========================

class MoveCopyAllocTestType {
    // This 'class' is to behave like 'bsltf::MovableAllocTestType', except
    // that, if moved in a context where source and destination allocators
    // don't match, it does a non-moving copy construct or copy assign.

    // DATA
    int                    *d_data_p;       // pointer to the data value
    bslma::Allocator       *d_allocator_p;  // allocator used to supply memory
                                            // (held, not owned)

    MoveCopyAllocTestType  *d_self_p;       // pointer to self (to verify this
                                            // object is not bit-wise moved
    MoveState::Enum         d_movedFrom;    // moved-from state
    MoveState::Enum         d_movedInto;    // moved-from state

  public:
    // CREATORS
    explicit MoveCopyAllocTestType(bslma::Allocator *basicAllocator = 0);
        // Create a 'MoveCopyAllocTestType' object having the (default)
        // attribute values:
        //..
        //  data() == -1
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit MoveCopyAllocTestType(int               data,
                                   bslma::Allocator *basicAllocator = 0);
        // Create a 'MoveCopyAllocTestType' object having the specified 'data'
        // attribute value.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    MoveCopyAllocTestType(
                     bslmf::MovableRef<MoveCopyAllocTestType>  original);
    MoveCopyAllocTestType(
                     bslmf::MovableRef<MoveCopyAllocTestType>  original,
                     bslma::Allocator                         *basicAllocator);
        // Move-construct this objectt from the specified 'original'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If no
        // allocator is specified or if 'basicAllocator' matches the allocator
        // of 'original', then 'original' is left in a valid but unspecified
        // state, if 'basicAllocator' is specified and does not match
        // 'original', the 'original' is copied without modification.

    MoveCopyAllocTestType(const MoveCopyAllocTestType&  original,
                          bslma::Allocator             *basicAllocator = 0);
        // Create a 'MoveCopyAllocTestType' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~MoveCopyAllocTestType();
        // Destroy this object.

    // MANIPULATORS
    MoveCopyAllocTestType& operator=(const MoveCopyAllocTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    MoveCopyAllocTestType& operator=(
                                 bslmf::MovableRef<MoveCopyAllocTestType> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  If
        // the allocators of '*this' and 'rhs' match, 'rhs' is left in a valid
        // but unspecified state, otherwise 'rhs' is copied without
        // modification.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    void setMovedInto(MoveState::Enum value);
        // Set the moved-into state of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

    MoveState::Enum movedInto() const;
        // Return the move state of this object as target of a move operation.

    MoveState::Enum movedFrom() const;
        // Return the move state of this object as source of a move operation.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
bool operator==(const MoveCopyAllocTestType& lhs,
                const MoveCopyAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MoveCopyAllocTestType' objects have
    // the same if their 'data' attributes are the same.
    // TBD: think about the behavior when specified on an object that was
    // moved-from on this as well as other functions/methods if appropriate.

bool operator!=(const MoveCopyAllocTestType& lhs,
                const MoveCopyAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MoveCopyAllocTestType' objects
    // do not have the same value if their 'data' attributes are not the same.

// FREE FUNCTIONS
MoveState::Enum getMovedFrom(const MoveCopyAllocTestType& object);
    // Return the move-from state of the specified 'object'.

MoveState::Enum getMovedInto(const MoveCopyAllocTestType& object);
    // Return the move-into state of the specified 'object'.

void setMovedInto(MoveCopyAllocTestType *object, MoveState::Enum value);
    // Set the moved-into state of the specified 'object' to the specified
    // 'value'.

// MANIPULATORS
inline
void MoveCopyAllocTestType::setMovedInto(MoveState::Enum value)
{
    d_movedInto = value;
}

// ACCESSORS
inline
int MoveCopyAllocTestType::data() const
{
    return d_data_p ? *d_data_p : 0;
}

inline
MoveState::Enum MoveCopyAllocTestType::movedFrom() const
{
    return d_movedFrom;
}

inline
MoveState::Enum MoveCopyAllocTestType::movedInto() const
{
    return d_movedInto;
}

                                  // Aspects

inline
bslma::Allocator *MoveCopyAllocTestType::allocator() const
{
    return d_allocator_p;
}

// FREE FUNCTION
inline
MoveState::Enum getMovedFrom(const MoveCopyAllocTestType& object)
{
    return object.movedFrom();
}

inline
MoveState::Enum getMovedInto(const MoveCopyAllocTestType& object)
{
    return object.movedInto();
}

inline
void setMovedInto(MoveCopyAllocTestType *object, MoveState::Enum value)
{
    object->setMovedInto(value);
}

                        // ---------------------------
                        // class MoveCopyAllocTestType
                        // ---------------------------

// CREATORS
MoveCopyAllocTestType::MoveCopyAllocTestType(bslma::Allocator *basicAllocator)
: d_data_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
    d_data_p = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = 0;
}

MoveCopyAllocTestType::MoveCopyAllocTestType(int               data,
                                             bslma::Allocator *basicAllocator)
: d_data_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
    d_data_p = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = data;
}

MoveCopyAllocTestType::MoveCopyAllocTestType(
                             bslmf::MovableRef<MoveCopyAllocTestType> original)
: d_data_p(0)
, d_allocator_p(bslmf::MovableRefUtil::access(original).d_allocator_p)
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_MOVED)
{
    MoveCopyAllocTestType& lvalue = original;

    if (lvalue.d_data_p) {
        d_data_p        = lvalue.d_data_p;
        lvalue.d_data_p = 0;
    }
    else {
        d_data_p  = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_data_p = 0;
    }
    lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
}

MoveCopyAllocTestType::MoveCopyAllocTestType(
                       bslmf::MovableRef<MoveCopyAllocTestType> original,
                       bslma::Allocator                        *basicAllocator)
: d_data_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
{
    MoveCopyAllocTestType& lvalue = original;

    if (d_allocator_p == lvalue.d_allocator_p) {
        if (lvalue.d_data_p) {
            d_data_p = lvalue.d_data_p;
            lvalue.d_data_p = 0;
        }
        else {
            d_data_p  =
                      static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
            *d_data_p = 0;
        }
        lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
        d_movedInto        = bsltf::MoveState::e_MOVED;
    }
    else {
        d_data_p    = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_data_p   = lvalue.data();
        d_movedInto = bsltf::MoveState::e_NOT_MOVED;
    }
}

MoveCopyAllocTestType::MoveCopyAllocTestType(
                                  const MoveCopyAllocTestType&  original,
                                  bslma::Allocator             *basicAllocator)
: d_data_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
    d_data_p = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = original.data();
}

MoveCopyAllocTestType::~MoveCopyAllocTestType()
{
    d_allocator_p->deallocate(d_data_p);

    BSLS_ASSERT_OPT(!!d_data_p ==
                               (bsltf::MoveState::e_NOT_MOVED == d_movedFrom));

    // Ensure that this objects has not been bitwise moved.

    BSLS_ASSERT_OPT(this == d_self_p);
}

// MANIPULATORS
MoveCopyAllocTestType&
MoveCopyAllocTestType::operator=(const MoveCopyAllocTestType& rhs)
{
    if (&rhs != this)
    {
        int *newData = static_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        d_allocator_p->deallocate(d_data_p);
        d_data_p = newData;
        *d_data_p = rhs.data();

        d_movedFrom = bsltf::MoveState::e_NOT_MOVED;
        d_movedInto = bsltf::MoveState::e_NOT_MOVED;
    }
    return *this;
}

MoveCopyAllocTestType&
MoveCopyAllocTestType::operator=(bslmf::MovableRef<MoveCopyAllocTestType> rhs)
{
    MoveCopyAllocTestType& lvalue = rhs;
    if (&lvalue != this)
    {
        if (d_allocator_p == lvalue.d_allocator_p) {
            if (lvalue.d_data_p) {
                if (d_data_p) {
                    d_allocator_p->deallocate(d_data_p);
                }
                d_data_p = lvalue.d_data_p;
                lvalue.d_data_p = 0;
            }
            else {
                int *newData = static_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
                if (d_data_p) {
                    d_allocator_p->deallocate(d_data_p);
                }

                d_data_p = newData;
                *d_data_p = 0;
            }

            d_movedInto        = bsltf::MoveState::e_MOVED;
            lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
        }
        else {
            int *newData = static_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
            if (d_data_p) {
                d_allocator_p->deallocate(d_data_p);
            }
            d_data_p = newData;
            *d_data_p = lvalue.data();

            d_movedInto        = bsltf::MoveState::e_MOVED;
        }
        d_movedFrom = bsltf::MoveState::e_NOT_MOVED;
    }
    return *this;
}

void MoveCopyAllocTestType::setData(int value)
{
    if (!d_data_p) {
        int *newData = static_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        d_data_p = newData;
    }
    *d_data_p = value;

    d_movedFrom = bsltf::MoveState::e_NOT_MOVED;
    d_movedInto = bsltf::MoveState::e_NOT_MOVED;
}

}  // close namespace bsltf

// FREE OPERATORS
inline
bool bsltf::operator==(const bsltf::MoveCopyAllocTestType& lhs,
                       const bsltf::MoveCopyAllocTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool bsltf::operator!=(const bsltf::MoveCopyAllocTestType& lhs,
                       const bsltf::MoveCopyAllocTestType& rhs)
{
    return lhs.data() != rhs.data();
}

// TRAITS
namespace bslma {
template <>
struct UsesBslmaAllocator<bsltf::MoveCopyAllocTestType>
    : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

typedef bsltf::MoveCopyAllocTestType MCElement;
typedef bsl::vector<MCElement>       MCVec;
typedef bdlcc::Deque<MCElement>      MCObj;

namespace {

bsl::ostream& operator<<(bsl::ostream& stream, const AElement& d)
{
    int c = d.data();

    if (0 == (c & ~0xff) && isalnum(c)) {
        char array[] = { "' '" };
        array[1] = static_cast<char>(c);

        stream << array;
    }
    else {
        stream << c;
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const MElement& d)
{
    int c = d.data();

    if (0 == (c & ~0xff) && isalnum(c)) {
        char array[] = { "' '" };
        array[1] = static_cast<char>(c);

        stream << array;
    }
    else {
        stream << c;
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const MCElement& d)
{
    int c = d.data();

    if (0 == (c & ~0xff) && isalnum(c)) {
        char array[] = { "' '" };
        array[1] = static_cast<char>(c);

        stream << array;
    }
    else {
        stream << c;
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const Obj& x)
    // Output the specified 'x' to the specified 'stream'.
{
    Obj::ConstProctor          cp(&x);
    const bsl::deque<Element>& d = *cp;

    stream << "[";
    for (unsigned ii = 0; ii < d.size(); ++ii) {
        stream << ' ' << d[ii];
    }
    stream << " ]";

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const AObj& x)
    // Output the specified 'x' to the specified 'stream'.
{
    AObj::ConstProctor          cp(&x);
    const bsl::deque<AElement>& d = *cp;

    stream << "[";
    for (unsigned ii = 0; ii < d.size(); ++ii) {
        stream << ' ' << d[ii];
    }
    stream << " ]";

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const MObj& x)
    // Output the specified 'x' to the specified 'stream'.
{
    MObj::ConstProctor          cp(&x);
    const bsl::deque<MElement>& d = *cp;

    stream << "[";
    for (unsigned ii = 0; ii < d.size(); ++ii) {
        stream << ' ' << d[ii];
    }
    stream << " ]";

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const MCObj& x)
    // Output the specified 'x' to the specified 'stream'.
{
    MCObj::ConstProctor          cp(&x);
    const bsl::deque<MCElement>& d = *cp;

    stream << "[";
    for (unsigned ii = 0; ii < d.size(); ++ii) {
        stream << ' ' << d[ii];
    }
    stream << " ]";

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const bsl::deque<Element>& d)
    // Output the specified 'd' to the specified 'stream'.
{
    stream << "[";
    for (unsigned ii = 0; ii < d.size(); ++ii) {
        stream << ' ' << d[ii];
    }
    stream << " ]";

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const bsl::deque<AElement>& d)
    // Output the specified 'd' to the specified 'stream'.
{
    stream << "[";
    for (unsigned ii = 0; ii < d.size(); ++ii) {
        stream << ' ' << d[ii];
    }
    stream << " ]";

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const bsl::deque<MElement>& d)
    // Output the specified 'd' to the specified 'stream'.
{
    stream << "[";
    for (unsigned ii = 0; ii < d.size(); ++ii) {
        stream << ' ' << d[ii];
    }
    stream << " ]";

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const bsl::deque<MCElement>& d)
    // Output the specified 'd' to the specified 'stream'.
{
    stream << "[";
    for (unsigned ii = 0; ii < d.size(); ++ii) {
        stream << ' ' << d[ii];
    }
    stream << " ]";

    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, bsltf::MoveState::Enum x)
{
    return stream << bsltf::MoveState::toAscii(x);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, u::VecType value)
{
    stream << (u::e_BSL == value ? "bsl"      :
               u::e_STD == value ? "std"      :
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
               u::e_PMR == value ? "std::pmr" :
#endif
               "unk");

    return stream;
}

namespace u {

bsls::TimeInterval now()
    // Return the current time, as a 'TimeInterval'.
{
    return bsls::SystemTime::nowRealtimeClock();
}

template <class TYPE>
struct HasWellBehavedMove : public bsl::is_trivially_copyable<TYPE>
{};

template <>
struct HasWellBehavedMove<bsltf::MovableAllocTestType> : public bsl::false_type
{};

template <>
struct HasWellBehavedMove<bsltf::AllocTestType> : public bsl::true_type
{};

template <>
struct HasWellBehavedMove<bsltf::MoveCopyAllocTestType> : public bsl::true_type
{};

template <class TYPE>
struct IsMoveAware : public bsl::false_type
{};

template <>
struct IsMoveAware<bsltf::MovableAllocTestType> : public bsl::true_type
{};

template <>
struct IsMoveAware<bsltf::MoveCopyAllocTestType> : public bsl::true_type
{};

template <class TYPE>
struct IsAllocating : public bsl::false_type
{};

template <>
struct IsAllocating<bsltf::AllocTestType> : public bsl::true_type
{};

template <>
struct IsAllocating<bsltf::MovableAllocTestType> : public bsl::true_type
{};

template <>
struct IsAllocating<bsltf::MoveCopyAllocTestType> : public bsl::true_type
{};

template <class TYPE>
void setData(TYPE *dst, int src)
{
    dst->setData(src);
}

template <>
void setData<Element>(Element *dst, int src)
{
    *dst = src;
}

template <class TYPE>
int getData(const TYPE& src)
{
    return TTF::getIdentifier(src);
}

template <>
int getData<Element>(const Element& src)
{
    ASSERTV(src, src <= INT_MAX);
    ASSERTV(src, src >= INT_MIN);

    return static_cast<int>(src);
}

template <>
int getData<MCElement>(const MCElement& src)
{
    return src.data();
}

const void *p(long unsigned u)
{
    return reinterpret_cast<const void *>(u);
}

struct BreakTestAllocator : public bslma::TestAllocator {
    // This 'struct' enables us to have an allocator with all the functionality
    // of a test allocator with a different place to put breakpoints on
    // 'allocate'.

    // CREATORS
    explicit
    BreakTestAllocator(bslma::Allocator *basicAllocator = 0)
    : TestAllocator("break_test", basicAllocator)
    {}
    explicit
    BreakTestAllocator(const char       *name,
                       bslma::Allocator *basicAllocator = 0)
    : TestAllocator(name,
                    basicAllocator)
    {}
    explicit
    BreakTestAllocator(bool              verboseFlag,
                       bslma::Allocator *basicAllocator = 0)
    : TestAllocator("break_test",
                    verboseFlag,
                    basicAllocator)
    {}
    explicit
    BreakTestAllocator(const char       *name,
                       bool              verboseFlag = false,
                       bslma::Allocator *basicAllocator = 0)
    : TestAllocator(name,
                    verboseFlag,
                    basicAllocator)
    {}
        // Create an instrumented "break test" allocator.  Optionally specify a
        // 'name' (associated with this object) to be included in diagnostic
        // messages written to 'stdout', thereby distinguishing this test
        // allocator from others that might be used in the same program.  If
        // 'name' is 0 (or not specified), no distinguishing name is
        // incorporated in diagnostics.  Optionally specify a 'verboseFlag'
        // indicating whether this test allocator should automatically report
        // all allocation/deallocation events to 'stdout' and print accumulated
        // statistics on destruction.  If 'verboseFlag' is 'false' (or not
        // specified), allocation/deallocation and summary messages will not be
        // written automatically.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the
        // 'MallocFreeAllocator' singleton is used.

    // MANIPULATOR
    void *allocate(size_type size);
        // Call the base class's 'allocate' function with the specified 'size'.
};

void *BreakTestAllocator::allocate(size_type size)
{
    return this->bslma::TestAllocator::allocate(size);
}

template <class TYPE>
const TYPE& myBack(const bdlcc::Deque<TYPE>& d)
    // Return, by value, the value of the back elament on the specified 'd'.
    // The behavior is undefined if 'd' is empty.
{
    typename bdlcc::Deque<TYPE>::ConstProctor proctor(&d);
    ASSERT(!proctor->empty());

    return proctor->back();
}

template <class TYPE>
const TYPE& myFront(const bdlcc::Deque<TYPE>& d)
    // Return, by value, the value of the front elament on the specified 'd'.
    // The behavior is undefined if 'd' is empty.
{
    typename bdlcc::Deque<TYPE>::ConstProctor proctor(&d);
    ASSERT(!proctor->empty());

    return proctor->front();
}

template <class TYPE>
size_t myLength(const bdlcc::Deque<TYPE>& d)
    // Check that the 'length' accessor returns the correct length of the
    // specified 'd', and return the length.  The behavior is undefined if
    // other threads are simulataneously modifying 'd.
{
    size_t ret = d.length();

    typename bdlcc::Deque<TYPE>::ConstProctor proctor(&d);

    ASSERT(ret == proctor->size());

    return ret;
}

template <int NUM>
void loadFromArray(Obj *dst, const Element (&src)[NUM])
    // Load all of the elements from the specified array 'src' into the
    // specified object 'dst'.
{
    Obj::Proctor proctor(dst);
    proctor->assign(src + 0, src + NUM);
}

class RandGen {
    // Random number generator using the high-order 32 bits of Donald Knuth's
    // MMIX algorithm.

    bsls::Types::Uint64 d_seed;

  public:
    explicit
    RandGen(int startSeed);
        // Initialize the generator with the specified 'startSeed'.

    unsigned operator()();
        // Return the next random number in the series;
};

// CREATOR
inline
RandGen::RandGen(int startSeed)
: d_seed(startSeed)
{
    (void) (*this)();
    (void) (*this)();
    (void) (*this)();
}

// MANIPULATOR
inline
unsigned RandGen::operator()()
{
    d_seed = d_seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<unsigned>(d_seed >> 32);
}

class RandChar {
    // Provide random, alpha numeric characters for values for test cases.  All
    // the types employed in numerous test cases can have their values set to
    // values analogous to ascii characters, this 'class' will provide random,
    // human-readable ascii values for these test cases.

    // DATA
    RandGen d_randGen;

  public:
    explicit
    RandChar(int startSeed = 12345)
    : d_randGen(startSeed)
        // Initialize the generator with the optionally specified 'startSeed'.
    {
        (void) d_randGen();
        (void) d_randGen();
        (void) d_randGen();
    }

    char operator()()
        // Return the next random number in the series;
    {
        int x = d_randGen() % 62;
        x = x < 26
          ? 'A' + x
          : x < 52
          ? 'a' + (x - 26)
          : '0' + (x - 52);

        return static_cast<char>(x);
    }
};

void populateRandCharVec(bsl::vector<char> *buffer, int numChars, int seed)
    // Populate the specified '*buffer' with the specified 'numChars' distinct
    // alphanumeric 'char' values, using the specified 'seed' as the random
    // number seed.
{
    BSLS_ASSERT(buffer->empty());
    BSLS_ASSERT(numChars <= 62);

    RandChar randChar(seed);

    for (int ii = 0; ii < numChars; ++ii) {
        char c = randChar();
        ASSERTV(c, isalnum(c));

        bool found = false;
        for (int jj = 0; !found && jj < ii; ++jj) {
            found = (*buffer)[jj] == c;
        }
        if (found) {
            --ii;
            continue;
        }

        buffer->push_back(c);
    }
}

class RandElement {
    // This 'class' is a stateful random number generator that will generate
    // random values of type 'Element'.

    // DATA
    u::RandGen d_randGen;

  public:
    // CREATOR
    explicit
    RandElement(unsigned seed)
    : d_randGen(seed)
        // Initialize the random number generator will the specified 'seed'.
    {
    }

    // MANIPULATOR
    Element operator()();
        // Return an 'Element' object with random state.
};

Element RandElement::operator()()
{
    const unsigned divDenom = static_cast<unsigned>(1) << 31;
    const unsigned divMask  = divDenom - 1;

    const unsigned num = d_randGen();
    const unsigned div = d_randGen();
    const Element  ret = num + static_cast<double>(div & divMask) / divDenom;

    return (div & divDenom) ? ret : -ret;
}

class Rand1To8 {
    // This 'class' is a stateful random number generator that will generate
    // random values in the range '[1 .. 8]'.

    // DATA
    RandGen   d_randGen;
    unsigned  d_rand;
    unsigned  d_bitsInDRand;

  public:
    // CREATOR
    explicit
    Rand1To8(unsigned seed)
    : d_randGen(seed)
    , d_rand(0)
    , d_bitsInDRand(0)
        // Create a random number generator starting with the specified 'seed'.
    {
    }

    // MANIPULATOR
    unsigned operator()()
        // Return a random number in the range '[1 .. 8]'.
    {
        // Leverage off 'randGen', that uses MMIX, but call it infrequently
        // because we only need 3 random bits per call.

        if (d_bitsInDRand < 3) {
            d_rand = d_randGen();
            d_bitsInDRand = 32;
        }

        unsigned ret = (d_rand & 7) + 1;
        d_rand >>= 3;
        d_bitsInDRand -= 3;

        return ret;
    }
};

template <class ELEMENT>
class ObjChecker {
    // This 'class' will, upon destruction, compare the two 'Deque<ELEMENT>'s
    // passed to it upon construction and assert that they are equal, unless
    // the 'release' method has been called, in which case the c'tor becomes a
    // no-op.  'ELEMENT' can be any streamable, equality comparable type.

    // TYPES
    typedef typename bdlcc::Deque<ELEMENT>::ConstProctor ConstProctor;

    // DATA
    const bdlcc::Deque<ELEMENT> *d_toBeModified_p;
    const bdlcc::Deque<ELEMENT> *d_toBeCompared_p;
    int                          d_line;

  public:
    // CREATORS
    ObjChecker(const bdlcc::Deque<ELEMENT>& toBeModified,
               const bdlcc::Deque<ELEMENT>& toBeCompared,
               int                          line)
        // Create a checker that will compare the values of the 'bsl::deque's
        // contained in the specified 'toBeModified' and 'toBeCompared'.  Store
        // 'line', the line number where the checker is created, to be part of
        // the warning message if the comparison fails.
    : d_toBeModified_p(&toBeModified)
    , d_toBeCompared_p(&toBeCompared)
    , d_line(line)
    {}

    ~ObjChecker()
        // Unless the 'release' method has been called in the lifetime of this
        // object, compare the two containers tracked by this object.
    {
        if (d_toBeModified_p) {
            ConstProctor               tbmProc(d_toBeModified_p);
            ConstProctor               tbcProc(d_toBeCompared_p);
            const bsl::deque<ELEMENT>& tbm = *tbmProc;
            const bsl::deque<ELEMENT>& tbc = *tbcProc;

            ASSERTV(d_line, tbm, tbc, "AObjChecker" && tbm == tbc);
        }
    }

    // MANIPULATORS
    void release()
        // Cancel the checking this object will do at destruction.
    {
        d_toBeModified_p = 0;
    }
};

template <class ELEMENT>
struct VecChecker {
    // DATA
    const bsl::vector<ELEMENT> *d_toBeModified_p;
    const bsl::vector<ELEMENT> *d_toBeCompared_p;
    int                         d_line;

    // CREATORS
    VecChecker(const bsl::vector<ELEMENT>& toBeModified,
               const bsl::vector<ELEMENT>& toBeCompared,
               int                         line)
    : d_toBeModified_p(&toBeModified)
    , d_toBeCompared_p(&toBeCompared)
    , d_line(line)
        // Create a checker that will compare the values of the specified
        // 'toBeModified' and 'toBeCompared'.  Store 'line', the line number
        // where the checker is created, to be part of the warning message if
        // the comparison fails.
    {}

    ~VecChecker()
        // Unless the 'release' method has been called in the lifetime of this
        // object, compare the two containers tracked by this object.
    {
        if (d_toBeModified_p) {
            if (*d_toBeModified_p != *d_toBeCompared_p) {
                const bsl::vector<ELEMENT>& M = *d_toBeModified_p;
                const bsl::vector<ELEMENT>& C = *d_toBeCompared_p;

                cout << "*d_toBeModified:";
                IntPtr s = M.size();
                for (int ii = 0; ii < s; ++ii) {
                    cout << "\tM[" << ii << "]: " << M[ii];
                }
                cout << "\n*d_toBeCompared:";
                s = C.size();
                for (int ii = 0; ii < s; ++ii) {
                    cout << "\tC[" << ii << "]: " << C[ii];
                }
                cout << '\n';
                ASSERTV(d_line, "VecChecker" && M == C);
            }
        }
    }

    // MANIPULATORS
    void release()
        // Cancel the checking this object will do at destruction.
    {
        d_toBeModified_p = 0;
    }
};

}  // close namespace u
}  // close unnamed namespace

//=============================================================================
//                               SUPPORT MACROS
//-----------------------------------------------------------------------------

// This macro pair injects exceptions based on the allocator belonging to the
// specified 'TEST_CONTAINER', which is of type 'bdlcc::Deque<ELEM_TYPE>'.  The
// macros verify that
//:
//: 1 The allocator used by the container under test is a test allocator.
//:
//: 2 When an exception is thrown, 'EXCEP_TEST_AOBJ_TBM' has been restored to
//:   its original value.
//:
//: 3 When the exception block is exited, if exceptions are enabled, at least
//:   one exception was thrown.
//
// Note that the macro will cause an 'ASSERT' failure if at least one
// allocation doesn't happen within the exception block.

#define BEGIN_EXCEP_TEST_OBJ(EXCEP_TEST_ELEM_TYPE, EXCEP_TEST_OBJ_OBJ)        \
    {                                                                         \
        bslma::TestAllocator *EXCEP_TEST_OBJ_ALLOC =                          \
                                dynamic_cast<bslma::TestAllocator *>(         \
                                           (EXCEP_TEST_OBJ_OBJ).allocator()); \
        ASSERT(EXCEP_TEST_OBJ_ALLOC);                                         \
                                                                              \
        const bdlcc::Deque<EXCEP_TEST_ELEM_TYPE> EXCEP_TEST_OBJ_COMPARE(      \
                                                 (EXCEP_TEST_OBJ_OBJ), &nda); \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*EXCEP_TEST_OBJ_ALLOC) {     \
            u::ObjChecker<EXCEP_TEST_ELEM_TYPE> EXCEP_TEST_OBJ_CHECKER(       \
                                                   (EXCEP_TEST_OBJ_OBJ),      \
                                                   EXCEP_TEST_OBJ_COMPARE,    \
                                                   __LINE__);

#define END_EXCEP_TEST_OBJ                                                    \
            EXCEP_TEST_OBJ_CHECKER.release();                                 \
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                              \
    }

// This macro pair injects exceptions based on the allocator belonging to the
// specified 'EXCEP_TEST_OBJ_OBJ', which must be an 'MObj' container.  The
// specified 'EXCEP_TEST_OBJ_VEC' is to be a vector, which is being populated
// by the operation performed in the block.  The macros very that
//:
//: 1 Both containers under test use the same allocator, and it's a test
//:   allocator.
//:
//: 2 When an exception is thrown, both containers have been restored to
//:   their original values.
//:
//: 3 When the exception block is exited, if exceptions are enabled, at least
//:   one exception was thrown.

#define BEGIN_EXCEP_TEST_OBJ_VEC(EXCEP_TEST_ELEM_TYPE,                        \
                                 EXCEP_TEST_OBJ_OBJ,                          \
                                 EXCEP_TEST_OBJ_VEC)                          \
    {                                                                         \
        bslma::TestAllocator *EXCEP_TEST_VEC_ALLOC =                          \
                    dynamic_cast<bslma::TestAllocator *>(                     \
                          (EXCEP_TEST_OBJ_VEC).get_allocator().mechanism());  \
        ASSERT(EXCEP_TEST_VEC_ALLOC);                                         \
                                                                              \
        const bdlcc::Deque<EXCEP_TEST_ELEM_TYPE>                              \
                   EXCEP_TEST_OBJ_COMPARE((EXCEP_TEST_OBJ_OBJ), &nda);        \
        const bsl::vector<EXCEP_TEST_ELEM_TYPE>                               \
                   EXCEP_TEST_OBJ_VEC_COMPARE((EXCEP_TEST_OBJ_VEC), &nda);    \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*EXCEP_TEST_VEC_ALLOC) {     \
            u::ObjChecker<EXCEP_TEST_ELEM_TYPE> EXCEP_TEST_OBJ_CHECKER(       \
                                                   (EXCEP_TEST_OBJ_OBJ),      \
                                                   EXCEP_TEST_OBJ_COMPARE,    \
                                                   __LINE__);                 \
            u::VecChecker<EXCEP_TEST_ELEM_TYPE> EXCEP_TEST_VEC_CHECKER(       \
                                                   (EXCEP_TEST_OBJ_VEC),      \
                                                   EXCEP_TEST_OBJ_VEC_COMPARE,\
                                                   __LINE__);

#define END_EXCEP_TEST_OBJ_VEC                                                \
            EXCEP_TEST_VEC_CHECKER.release();                                 \
            EXCEP_TEST_OBJ_CHECKER.release();                                 \
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                              \
    }

//=============================================================================
//          USAGE example 2 from header (with assert replaced with ASSERT)
//-----------------------------------------------------------------------------

namespace USAGE_EXAMPLE_2 {

//
///Example 2: A Queue of Events
/// - - - - - - - - - - - - - -
// First, we declare the 'Event' type, that will be contained in our
// 'bdlcc::Deque' object.

    struct Event {
        enum EventType {
            e_IN_PROGRESS   = 1,
            e_TASK_COMPLETE = 2 };

        EventType   d_type;
        int         d_workerId;
        int         d_eventNumber;
        const char *d_eventText_p;
    };

// Then, we define the number of events each thread will push:

    const int k_NUM_TO_PUSH = 5;

// Next, we declare our 'WorkerFunctor' type, that will push 'k_NUM_TO_PUSH'
// events into the deque.

    struct WorkerFunctor {
        int                  d_workerId;
        bdlcc::Deque<Event> *d_deque_p;
        bslmt::Barrier      *d_barrier_p;

        void operator()()
            // All the threads will block on the same barrier so they all start
            // at once to maximize concurrency.
        {
            d_barrier_p->wait();

            // Loop to push 'k_NUM_TO_PUSH - 1' events onto the deque.

            int evnum = 1;
            while (evnum < k_NUM_TO_PUSH) {
                // Yield every loop to maximize concurrency.

                bslmt::ThreadUtil::yield();

                // Create the event object.

                Event ev = {
                    Event::e_IN_PROGRESS,
                    d_workerId,
                    evnum++,
                    "In-Progress Event"
                };

                // Push the event object.

                d_deque_p->pushBack(ev);
            }

            // Create the completing event object.

            Event ev = {
                Event::e_TASK_COMPLETE,
                d_workerId,
                evnum,
                "Task Complete"
            };

            // Push the completing event object.

            d_deque_p->pushBack(ev);
        }
    };
//..

}  // close namespace USAGE_EXAMPLE_2

//=============================================================================
//          USAGE example 1 from header (with assert replaced with ASSERT)
//-----------------------------------------------------------------------------

namespace USAGE_EXAMPLE_1 {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Queue of Work Requests
///- - - - - - - - - - - - - - - - - -
// First, declarer the struct 'WordData'.  Imagine it contains some data one
// wants to process:
//..
    struct WorkData {
        // work data...
    };
//..
// Then, create the function that will produce a 'WorkData' object:
//..
    bool getWorkData(WorkData *)
        // Dummy implementation of 'getWorkData' function required by the usage
        // example.
    {
        static bsls::AtomicInt i(1);
        return ++i < 1000;
    }
//..
// Next, declare 'WorkRequest', the type of object that will be stored in
// the container:
//..
    struct WorkRequest {
        // PUBLIC TYPES
        enum RequestType {
            e_WORK = 1,
            e_STOP = 2
        };

        // PUBLIC DATA
        RequestType d_type;
        WorkData d_data;
    };
//..
// Then, create the function that will do work on a 'WorkRequest' object:
//..
    void doWork(WorkData *workData)
        // Function that pretends to do work on the specified 'workData'.
    {
        // do some stuff with '*workData' ...

        (void) workData;
    }
//..
// Next, create the functor that will be run in the consumer threads:
//..
    struct ConsumerFunctor {
        // DATA
        bdlcc::Deque<WorkRequest> *d_deque_p;

        // CREATORS
        explicit
        ConsumerFunctor(bdlcc::Deque<WorkRequest> *container)
            // Create a ''ConsumerFunctor' object that will consumer work
            // requests from the specified 'container'.
        : d_deque_p(container)
        {}

        // MANIPULATORS
        void operator()()
            // Pop work requests off the deque and process them until an
            // 'e_STOP' request is encountered.
        {
            WorkRequest item;

            do {
                item = d_deque_p->popFront();
                if (WorkRequest::e_WORK == item.d_type) {
                    doWork(&item.d_data);
                }
            } while (WorkRequest::e_STOP != item.d_type);
        }
    };
//..
// Then, create the functor that will be run in the producer threads:
//..
    struct ProducerFunctor {
        // DATA
        bdlcc::Deque<WorkRequest> *d_deque_p;

        // CREATORS
        explicit
        ProducerFunctor(bdlcc::Deque<WorkRequest> *container)
            // Create a 'ProducerFunctor' object that will enqueue work
            // requests into the specified 'container'.
        : d_deque_p(container)
        {}

        // MANIPULATORS
        void operator()()
            // Enqueue work requests to the container until 'getWorkData'
            // returns 'false', then enqueue an 'e_STOP' request.
        {
            WorkRequest item;
            WorkData    workData;

            while (!getWorkData(&workData)) {
                item.d_type = WorkRequest::e_WORK;
                item.d_data = workData;
                d_deque_p->pushBack(item);
            }

            item.d_type = WorkRequest::e_STOP;
            d_deque_p->pushBack(item);
        }
    };
//..

}  // close namespace USAGE_EXAMPLE_1

//=============================================================================
//                                  TEST CASE 26
//-----------------------------------------------------------------------------

namespace TEST_CASE_26 {

template <class ELEMENT>
class TimedPopRecordBack {
    // Functor to do a couple of 'timedPopBack' on the 'Deque<ELEMENT>' passed
    // at construction.  Sometimes the main thread will push an item to the
    // 'Deque' for us to pop, other times it won't so that we will time out.

    // DATA
    bdlcc::Deque<ELEMENT> *d_deque_p;
    bslmt::Barrier        *d_barrier_p;
    bsls::TimeInterval     d_timeout;
    const ELEMENT&         d_expected;

  public:
    // CREATORS
    TimedPopRecordBack(bdlcc::Deque<ELEMENT> *deque,
                       bslmt::Barrier        *barrier,
                       bsls::TimeInterval     timeout,
                       const ELEMENT&         value)
    : d_deque_p(deque)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_expected(value)
        // Create a test object that will access the specified '*deque' and
        // with the specified 'val' and the specified 'timeout' and block on
        // the specified '*barrier'.  After popping, confirm the value popped
        // equals the specified 'value'.
    {
    }

    // MANIPULATORS
    void operator()()
        // Do two pushes, the first of which will timeout, the second of which
        // will succeed without timing out.
    {
        bsl::vector<ELEMENT> resultFootprint(d_deque_p->allocator());
        resultFootprint.resize(1);
        ELEMENT& result = resultFootprint[0];
        u::setData(&result, 'Z');

        d_barrier_p->wait();

        // no one's pushing this time, pop will tame out

        bsls::TimeInterval start = u::now();
        int timeoutFlag = d_deque_p->timedPopBack(&result, start + d_timeout);
        bsls::TimeInterval end   = u::now();

        ASSERT(0 != timeoutFlag);
        ASSERT(end >= start + d_timeout);
        ASSERT('Z' == u::getData(result));    // not assigned to
        ASSERT(e_MOVED != bsltf::getMovedInto(result));

        d_barrier_p->wait();

        // other thread will push

        d_barrier_p->wait();

        int numThrows = -1;
        start = u::now();
        BEGIN_EXCEP_TEST_OBJ(ELEMENT, *d_deque_p) {
            ++numThrows;

            timeoutFlag = d_deque_p->timedPopBack(&result,
                                                  start + d_timeout);
        } END_EXCEP_TEST_OBJ
        end   = u::now();
        ASSERTV(numThrows, expAssignMove, !PLAT_EXC ||
                (!u::IsAllocating<ELEMENT>::value ||
                             (u::IsMoveAware<ELEMENT>::value && expAssignMove))
                                                          == (0 == numThrows));
        const bsltf::MoveState::Enum expMS = u::IsMoveAware<ELEMENT>::value
                                           ? expAssignMoveState
                                           : e_UNKNOWN;
        const bsltf::MoveState::Enum ms = bsltf::getMovedInto(result);
        ASSERTV(expMS, ms, expMS == ms);
        ASSERT(end <  start + d_timeout);
        ASSERTV(result, 'A' == u::getData(result));

        // pop should've succeeded

        ASSERT(0 == timeoutFlag);
        ASSERTV(result, d_expected, result == d_expected);
    }
};

template <class ELEMENT>
class TimedPopRecordFront {
    // Functor to do a couple of 'timedPopFront' on the 'Deque<ELEMENT>' passed
    // at construction.  Sometimes the main thread will push an item to the
    // 'Deque' for us to pop, other times it won't so that we will time out.

    // DATA
    bdlcc::Deque<ELEMENT> *d_deque_p;
    bslmt::Barrier        *d_barrier_p;
    bsls::TimeInterval     d_timeout;
    const ELEMENT&         d_expected;

  public:
    // CREATORS
    TimedPopRecordFront(bdlcc::Deque<ELEMENT> *deque,
                        bslmt::Barrier        *barrier,
                        bsls::TimeInterval     timeout,
                        const ELEMENT&         value)
    : d_deque_p(deque)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_expected(value)
        // Create a test object that will access the specified '*deque' and
        // with the specified 'val' and the specified 'timeout' and block on
        // the specified '*barrier'.  After popping, confirm the value popped
        // equals 'value'.
    {
    }

    // MANIPULATORS
    void operator()()
        // Do two pushes, the first of which will timeout, the second of which
        // will succeed without timing out.
    {
        bsl::vector<ELEMENT> resultFootprint(d_deque_p->allocator());
        resultFootprint.resize(1);
        ELEMENT& result = resultFootprint[0];
        u::setData(&result, 'Z');

        d_barrier_p->wait();

        // no one's pushing this time, pop will tame out

        bsls::TimeInterval start = u::now();
        int timeoutFlag = d_deque_p->timedPopFront(&result, start + d_timeout);
        bsls::TimeInterval end   = u::now();

        ASSERT(0 != timeoutFlag);
        ASSERT(end >= start + d_timeout);
        ASSERT('Z' == u::getData(result));    // not assigned to
        ASSERT(e_MOVED != bsltf::getMovedInto(result));

        d_barrier_p->wait();

        // other thread will push

        d_barrier_p->wait();

        int numThrows = -1;
        start = u::now();
        BEGIN_EXCEP_TEST_OBJ(ELEMENT, *d_deque_p) {
            ++numThrows;

            timeoutFlag = d_deque_p->timedPopFront(&result,
                                                   start + d_timeout);
        } END_EXCEP_TEST_OBJ
        end   = u::now();
        ASSERTV(numThrows, expAssignMove, !PLAT_EXC ||
                (!u::IsAllocating<ELEMENT>::value ||
                             (u::IsMoveAware<ELEMENT>::value && expAssignMove))
                                                          == (0 == numThrows));
        const bsltf::MoveState::Enum expMS = u::IsMoveAware<ELEMENT>::value
                                           ? expAssignMoveState
                                           : e_UNKNOWN;
        const bsltf::MoveState::Enum ms = bsltf::getMovedInto(result);
        ASSERTV(expMS, ms, expMS == ms);
        ASSERT(end <  start + d_timeout);
        ASSERTV(result, 'A' == u::getData(result));

        // pop should've succeeded

        ASSERT(0 == timeoutFlag);
        ASSERTV(result, d_expected, result == d_expected);
    }
};

template <class ELEMENT>
void testTimedPushPopMove()
{
    if (verbose) cout << "testTimedPushPopMove<" << NameOf<ELEMENT>() <<
                                                                       ">()\n";

    bslma::TestAllocator         da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);
    bslma::TestAllocator         ta(veryVeryVeryVerbose);

    bsl::vector<ELEMENT>         vFootprint(&ta);
    vFootprint.resize(4);
    u::setData(&vFootprint[0], 'A');
    u::setData(&vFootprint[1], 'B');
    u::setData(&vFootprint[2], 'A');
    u::setData(&vFootprint[3], 'B');
    ELEMENT&                     vA = vFootprint[0];
    ELEMENT&                     vB = vFootprint[1];
    const ELEMENT&               VA = vFootprint[2];
    const ELEMENT&               VB = vFootprint[3];

    // Expected Copyconstruct Movestate

    bsltf::MoveState::Enum ecm   = u::IsMoveAware<ELEMENT>::value
                                 ? e_MOVED
                                 : e_UNKNOWN;

    if (verbose) cout << "\tWith 'timedPopBack'" << endl;
    {
        bsls::TimeInterval T3(3 * DECI_SEC);          // 0.3s

        bslmt::Barrier barrier(2);
        bdlcc::Deque<ELEMENT> x(1, &ta);    const bdlcc::Deque<ELEMENT>& X = x;
        {
            typename bdlcc::Deque<ELEMENT>::Proctor p(&x);
            p->reserve(4);
        }
        ASSERT(1 == X.highWaterMark());

        TimedPopRecordBack<ELEMENT> testMObj(&x, &barrier, T3, VA);
        bslmt::ThreadUtil::Handle thread;
        bslmt::ThreadUtil::create(&thread, testMObj);

        barrier.wait();

        // thread will time out on a pop

        barrier.wait();

        ASSERT(0 == X.length());

        // First push, will succeed without timeout.

        Int64 tana = ta.numAllocations();
        bsls::TimeInterval start = u::now();
        int rc = x.timedPushBack(bslmf::MovableRefUtil::move(vA),
                                 start + T3);
        bsls::TimeInterval end = u::now();
        ASSERT(0 == rc);
        ASSERT(end < start + T3);
        ASSERTV(ta.numAllocations() - tana,
                            (u::IsAllocating<ELEMENT>::value &&
                                    !u::IsMoveAware<ELEMENT>::value) ==
                                             (0 < ta.numAllocations() - tana));
        ASSERTV(bsltf::getMovedFrom(vA), ecm, bsltf::getMovedFrom(vA) == ecm);
        vA = VA;

        // Second push, will timeout and fail.

        tana = ta.numAllocations();
        ASSERT(1 == X.length());
        start = u::now();
        rc = x.timedPushBack(bslmf::MovableRefUtil::move(vB),
                             start + T3);
        end   = u::now();
        ASSERT(0 != rc);
        ASSERT(end >= start + T3);
        ASSERTV(ta.numAllocations() - tana, 0 == ta.numAllocations() - tana);
        ASSERTV(vB, VB, VB == vB);
        ASSERTV(bsltf::getMovedFrom(vB), e_MOVED != bsltf::getMovedFrom(vB));

        ASSERT(1 == X.length());

        barrier.wait();

        bslmt::ThreadUtil::join(thread);

        ASSERT(0 == X.length());
    }
    ASSERT(0 == da.numAllocations());

    if (verbose) cout << "\tWith 'timedPopFront'" << endl;
    {
        bsls::TimeInterval T3(3 * DECI_SEC);          // 0.3s

        bslmt::Barrier barrier(2);
        bdlcc::Deque<ELEMENT> x(1, &ta);    const bdlcc::Deque<ELEMENT>& X = x;
        {
            typename bdlcc::Deque<ELEMENT>::Proctor p(&x);
            p->reserve(4);
        }
        ASSERT(1 == X.highWaterMark());

        TimedPopRecordFront<ELEMENT> testMObj(&x, &barrier, T3, VA);
        bslmt::ThreadUtil::Handle thread;
        bslmt::ThreadUtil::create(&thread, testMObj);

        barrier.wait();

        // thread will time out on a pop

        barrier.wait();

        ASSERT(0 == X.length());

        // First push, will succeed without timeout.

        Int64 tana = ta.numAllocations();
        bsls::TimeInterval start = u::now();
        int rc = x.timedPushFront(bslmf::MovableRefUtil::move(vA),
                                  start + T3);
        bsls::TimeInterval end = u::now();
        ASSERT(0 == rc);
        ASSERT(end < start + T3);
        ASSERTV(ta.numAllocations() - tana,
                            (u::IsAllocating<ELEMENT>::value &&
                                    !u::IsMoveAware<ELEMENT>::value) ==
                                             (0 < ta.numAllocations() - tana));
        ASSERTV(bsltf::getMovedFrom(vA), ecm, bsltf::getMovedFrom(vA) == ecm);
        vA = VA;

        // Second push, will timeout and fail.

        tana = ta.numAllocations();
        ASSERT(1 == X.length());
        start = u::now();
        rc = x.timedPushFront(bslmf::MovableRefUtil::move(vB),
                              start + T3);
        end   = u::now();
        ASSERT(0 != rc);
        ASSERT(end >= start + T3);
        ASSERTV(ta.numAllocations() - tana, 0 == ta.numAllocations() - tana);
        ASSERTV(vB, VB, VB == vB);
        ASSERTV(bsltf::getMovedFrom(vB), e_MOVED != bsltf::getMovedFrom(vB));

        ASSERT(1 == X.length());

        barrier.wait();

        bslmt::ThreadUtil::join(thread);

        ASSERT(0 == X.length());
    }
    ASSERT(0 == da.numAllocations());
}

}  // close namespace TEST_CASE_26

//=============================================================================
//                                  TEST CASE 25
//-----------------------------------------------------------------------------

namespace TEST_CASE_25 {

template <class ELEMENT>
void testSingleMovingPushesPops()
{
    bslma::TestAllocator         da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

    bslma::TestAllocator sa(veryVeryVeryVerbose);
    bslma::TestAllocator ta(veryVeryVeryVerbose);

    Int64 expDaNumBlocksTotal = da.numBlocksTotal();
    Int64 numPopValueAllocs;
        // The number of default allocations done by a return-by-value depends
        // heavily on whether the compiler does the return value optimizations,
        // which varies unpredictably with different compilers and optimization
        // levels, so we'll have to set this variable empirically on the first
        // pop returning by value.

    // Expected Assign Movestate

    bsltf::MoveState::Enum eam   = u::IsMoveAware<ELEMENT>::value
                                 ? expAssignMoveState
                                 : e_UNKNOWN;

    // Expected Copyconstruct Movestate

    bsltf::MoveState::Enum ecm   = u::IsMoveAware<ELEMENT>::value
                                 ? e_MOVED
                                 : e_UNKNOWN;

    // Expected Copyconstruct Movestate -- Non-Matching Allocators

    bsltf::MoveState::Enum ecmnma =
                             bslmf::IsSame<ELEMENT,
                                           bsltf::MoveCopyAllocTestType>::value
                                           ? e_NOT_MOVED
                                           : ecm;

    bool expAssignAlloc = u::IsAllocating<ELEMENT>::value && eam != e_MOVED;
    bool expCreateAlloc = u::IsAllocating<ELEMENT>::value && ecm != e_MOVED;

    bsl::vector<char> randCharVec(&sa);
    u::populateRandCharVec(&randCharVec, 20, test);

    Int64 taNumAllocs;

    if (verbose) cout << "testSingleMovingPushesPops<" << NameOf<ELEMENT>() <<
                                                                       ">()\n";

    if (verbose) cout << "\t1. Explicit Pushes and Pops\n";
    {
        enum { NUM_V = 4 };
        bsl::vector<ELEMENT> v(&ta);
        bsl::vector<ELEMENT> vConst(&ta);
        const bsl::vector<ELEMENT>& V = vConst;
        bsl::vector<ELEMENT> eFootprint(&ta);
        eFootprint.resize(1);
        ELEMENT& e = eFootprint.front();

        v.resize(NUM_V);
        vConst.resize(4);
        for (int ti = 0; ti < NUM_V; ++ti) {
            u::setData(&v[     ti], randCharVec[ti]);
            u::setData(&vConst[ti], randCharVec[ti]);
            if (veryVerbose) { T_ T_ P_(ti); P(V[ti]); }
        }

        bdlcc::Deque<ELEMENT> x(&ta);    const bdlcc::Deque<ELEMENT>& X = x;

        for (int tryPush = 0; tryPush < 2; ++tryPush) {
            if (verbose) cout << "\t\t'pushBack' && 'length'\n";
            {
                ASSERT(0 == u::myLength(X));

                if (tryPush) {
                    ASSERT(0 == x.tryPushBack(MUtil::move(v[0])));
                }
                else {
                    x.pushBack(MUtil::move(v[0]));
                }

                ASSERT(1 == u::myLength(X));
                const ELEMENT *back_p = &u::myBack(X);

                ASSERT(V[0] == *back_p);
                ASSERT(ecm == bsltf::getMovedInto(*back_p));

                if (tryPush) {
                    ASSERT(0 == x.tryPushBack(MUtil::move(v[1])));
                }
                else {
                    x.pushBack(MUtil::move(v[1]));
                }

                ASSERT(2 == u::myLength(X));
                const ELEMENT *front_p = &u::myFront(X);
                ASSERT(V[0] == *front_p);
                ASSERTV(ecm, bsltf::getMovedInto(*front_p),
                                         ecm == bsltf::getMovedInto(*front_p));
                back_p = &u::myBack(X);
                ASSERT(V[1] == *back_p);
                ASSERTV(ecm, bsltf::getMovedInto(*back_p),
                                          ecm == bsltf::getMovedInto(*back_p));

                ASSERTV(ecm, bsltf::getMovedFrom(v[0]),
                                             ecm == bsltf::getMovedFrom(v[0]));
                ASSERTV(ecm, bsltf::getMovedFrom(v[1]),
                                             ecm == bsltf::getMovedFrom(v[1]));
            }

            v[0] = V[0];
            v[1] = V[1];

            ASSERTV(da.numBlocksTotal() - expDaNumBlocksTotal,
                               0 == da.numBlocksTotal() - expDaNumBlocksTotal);

            if (verbose) cout << "\t\t'popBack' && 'length'\n";
            {
                ASSERT(2 == u::myLength(X));
                const ELEMENT& rv = x.popBack();
                ASSERT(V[1] == rv);
                numPopValueAllocs = da.numBlocksTotal() - expDaNumBlocksTotal;
                expDaNumBlocksTotal += numPopValueAllocs;
                ASSERTV(numPopValueAllocs, bsltf::getMovedInto(rv),
                     !!numPopValueAllocs == (u::IsAllocating<ELEMENT>::value &&
                                          e_MOVED != bsltf::getMovedInto(rv)));

                ASSERT(1 == u::myLength(X));

                x.pushBack(bslmf::MovableRefUtil::move(v[1]));
                ASSERT(2 == u::myLength(X));
                u::setData(&e, -1);
                taNumAllocs = ta.numAllocations();
                x.popBack(&e);
                ASSERT(V[1] == e);
                ASSERT(eam == bsltf::getMovedInto(e));
                ASSERTV(ta.numAllocations(), taNumAllocs, expAssignAlloc,
                          ta.numAllocations() == taNumAllocs + expAssignAlloc);

                ASSERT(V[0] == u::myFront(X));
                ASSERT(V[0] == u::myBack( X));

                ASSERT(V[0] == x.popBack());
                expDaNumBlocksTotal += numPopValueAllocs;

                ASSERT(0 == u::myLength(X));

            }

            v[1] = V[1];

            ASSERTV(numPopValueAllocs,
                                     da.numBlocksTotal() - expDaNumBlocksTotal,
                               0 == da.numBlocksTotal() - expDaNumBlocksTotal);

            if (verbose) cout << "\t\t'pushFront' && 'length'\n";
            {
                ASSERT(0 == u::myLength(X));

                if (tryPush) {
                    ASSERT(0 == x.tryPushFront(MUtil::move(v[2])));
                }
                else {
                    x.pushFront(MUtil::move(v[2]));
                }

                ASSERT(1 == u::myLength(X));
                const ELEMENT *front_p = &u::myFront(X);
                ASSERT(V[2] == *front_p);
                ASSERT(ecm == bsltf::getMovedInto(*front_p));
                ASSERT(ecm == bsltf::getMovedFrom(v[2]));

                if (tryPush) {
                    ASSERT(0 == x.tryPushFront(MUtil::move(v[3])));
                }
                else {
                    x.pushFront(bslmf::MovableRefUtil::move(v[3]));
                }
                ASSERT(ecm == bsltf::getMovedFrom(v[3]));

                ASSERT(2 == u::myLength(X));

                const ELEMENT *back_p = &u::myBack(X);
                front_p = &u::myFront(X);

                ASSERT(V[3] == *front_p);
                ASSERT(ecm == bsltf::getMovedInto(*front_p));

                ASSERT(V[2] == *back_p);
                ASSERT(ecm == bsltf::getMovedInto(*back_p));
            }

            v[2] = V[2];
            v[3] = V[3];

            ASSERTV(da.numBlocksTotal() - expDaNumBlocksTotal,
                               0 == da.numBlocksTotal() - expDaNumBlocksTotal);

            if (verbose) cout << "\t\t'popFront' && 'length'\n";
            {
                ASSERT(2 == u::myLength(X));
                const ELEMENT& rv = x.popFront();
                ASSERT(V[3] == rv);
                expDaNumBlocksTotal += numPopValueAllocs;
                ASSERTV(numPopValueAllocs, bsltf::getMovedInto(rv),
                     !!numPopValueAllocs == (u::IsAllocating<ELEMENT>::value &&
                                          e_MOVED != bsltf::getMovedInto(rv)));

                ASSERT(1 == u::myLength(X));
                x.pushFront(V[3]);    // not move, move already tested
                const ELEMENT *front_p = &u::myFront(X);
                ASSERT(e_MOVED != bsltf::getMovedInto(*front_p));
                ASSERT(2 == u::myLength(X));

                u::setData(&e, -1);
                taNumAllocs = ta.numAllocations();
                x.popFront(&e);
                ASSERT(V[3] == e);
                ASSERT(eam == bsltf::getMovedInto(e));
                ASSERTV(ta.numAllocations(), taNumAllocs, expAssignAlloc,
                          ta.numAllocations() == taNumAllocs + expAssignAlloc);

                front_p = &u::myFront(X);
                const ELEMENT *back_p  = &u::myBack(X);
                ASSERT(V[2] == *front_p);
                ASSERT(V[2] == *back_p);
                ASSERT(front_p == back_p);

                ASSERT(V[2] == x.popFront());
                expDaNumBlocksTotal += numPopValueAllocs;

                ASSERT(0 == u::myLength(X));
            }
        }

        ASSERTV(da.numBlocksTotal() - expDaNumBlocksTotal,
                               0 == da.numBlocksTotal() - expDaNumBlocksTotal);
    }

    ASSERT(0 == ta.numBytesInUse());

    if (verbose) cout << "\t2. Random pushes and pops\n";
    {
        // In this block, we have 3 parallel containers 'x', 'xB', and 'd', to
        // which we apply identical pushes and pops, and observe their behavior
        // is always the same.

        bdlcc::Deque<ELEMENT>        x(&ta);
        const bdlcc::Deque<ELEMENT>& X  = x;
        bdlcc::Deque<ELEMENT>        xB(&ta);
        const bdlcc::Deque<ELEMENT>& XB = xB;
        bsl::deque<ELEMENT>          d(&ta);
        const bsl::deque<ELEMENT>&   D = d;

        ASSERT(0 == D.size());
        ASSERT(0 == X.length());
        ASSERT(u::myLength(X) == X.length());

        unsigned int expectedLength = 0;
        const int    ITERATIONS = veryVeryVerbose ? 50 : 5000;
        u::RandGen   randGen(12345);
        u::RandChar  randChar(12345);

        for (int i = 0; i < ITERATIONS; ++i) {
            unsigned int ll;
            do {
                ll = randGen() % 8;
            } while (expectedLength == ll);
            const unsigned int LENGTH = ll;

            if (expectedLength < LENGTH) {
                while (expectedLength < LENGTH) {
                    ASSERT(expectedLength == D.size());
                    ASSERT(expectedLength == u::myLength(X));
                    ASSERT(expectedLength == u::myLength(XB));

                    // Generate a fairly random double using 'generate15'.

                    const char c = randChar();
                    bsl::vector<ELEMENT> vFootprint(&ta);
                    vFootprint.resize(4);
                    ELEMENT& v  = vFootprint[0];
                    ELEMENT& vB = vFootprint[1];
                    ELEMENT& vD = vFootprint[2];
                    u::setData(&v,  c);
                    u::setData(&vB, c);
                    u::setData(&vD, c);

                    if (randGen() & 0x80) {
                        d. push_back(bslmf::MovableRefUtil::move(vD));
                        x. pushBack( bslmf::MovableRefUtil::move(v));
                        xB.pushBack( bslmf::MovableRefUtil::move(vB));
                        if (veryVerbose) {
                            T_ T_ P_(i); P_(LENGTH); P_(expectedLength);
                            cout << "\tPUB: '" << c << "'\n";
                        }
                    }
                    else {
                        d. push_front(bslmf::MovableRefUtil::move(vD));
                        x. pushFront( bslmf::MovableRefUtil::move(v));
                        xB.pushFront( bslmf::MovableRefUtil::move(vB));
                        if (veryVerbose) {
                            T_ T_ P_(i); P_(LENGTH); P_(expectedLength);
                            cout << "\tPUF: '" << c << "'\n";
                        }
                    }

                    ++expectedLength;

                    ASSERT(expectedLength == D.size());
                    ASSERT(expectedLength == u::myLength(X));
                    ASSERT(expectedLength == u::myLength(XB));
                }
            }
            else {
                bsl::vector<ELEMENT> vFootprint(&ta);
                vFootprint.resize(2);
                ELEMENT& popped = vFootprint[0];
                ELEMENT& e      = vFootprint[1];

                while (expectedLength > LENGTH) {
                    ASSERT(expectedLength == D.size());
                    ASSERT(expectedLength == u::myLength(X));
                    ASSERT(expectedLength == u::myLength(XB));

                    if (randGen() & 0x80) {
                        popped = D.back();
                        d.pop_back();
                        ASSERT(popped == x.popBack());
                        expDaNumBlocksTotal += numPopValueAllocs;
                        taNumAllocs = ta.numAllocations();
                        u::setData(&e, -1);
                        xB.popBack(&e);
                        ASSERT(popped == e);
                        ASSERT(eam == bsltf::getMovedInto(e));
                        ASSERTV(ta.numAllocations(), taNumAllocs,
                                                                expAssignAlloc,
                                     ta.numAllocations() ==
                                                 taNumAllocs + expAssignAlloc);
                        if (veryVerbose) {
                            T_ T_ P_(i); P_(LENGTH); P_(expectedLength);
                            cout << "\tPOB: " << popped << endl;
                        }
                    }
                    else {
                        popped = D.front();
                        d.pop_front();
                        ASSERT(popped == x.popFront());
                        expDaNumBlocksTotal += numPopValueAllocs;
                        taNumAllocs = ta.numAllocations();
                        u::setData(&e, -1);
                        xB.popFront(&e);
                        ASSERT(popped == e);
                        ASSERT(eam == bsltf::getMovedInto(e));
                        ASSERTV(ta.numAllocations(), taNumAllocs,
                                                                expAssignAlloc,
                                     ta.numAllocations() ==
                                                 taNumAllocs + expAssignAlloc);
                        if (veryVerbose) {
                            T_ T_ P_(i); P_(LENGTH); P_(expectedLength);
                            cout << "\tPOF: " << popped << endl;
                        }
                    }

                    --expectedLength;

                    ASSERT(expectedLength == D.size());
                    ASSERT(expectedLength == u::myLength(X));
                    ASSERT(expectedLength == u::myLength(XB));

                    ASSERTV(da.numBlocksTotal() - expDaNumBlocksTotal,
                               0 == da.numBlocksTotal() - expDaNumBlocksTotal);
                }
            }

            ASSERT(LENGTH == expectedLength);
        }
    }

    if (verbose) cout << "\t3. Explicit Pushes and Pops, Alloc & Excep\n";
    {
        enum { NUM_V = 4 };
        bsl::vector<ELEMENT> vSa(&sa);
        vSa.resize(NUM_V);
        int val = 'A';
        for (int ti = 0; ti < NUM_V; ++ti, ++val) {
            u::setData(&vSa[ti], randCharVec[ti]);
        }
        const bsl::vector<ELEMENT> V(vSa, &sa);
        bsl::vector<ELEMENT>       vTa(V, &ta);
        bsl::vector<ELEMENT>       eTaFootprint(&ta);
        eTaFootprint.resize(1);
        ELEMENT&                   eTa = eTaFootprint.front();
        bsl::vector<ELEMENT>       eSaFootprint(&sa);
        eSaFootprint.resize(1);
        ELEMENT&                   eSa = eSaFootprint.front();

        bdlcc::Deque<ELEMENT> x(&ta);    const bdlcc::Deque<ELEMENT>& X = x;
        {
            typename bdlcc::Deque<ELEMENT>::Proctor p(&x);
            p->reserve(4);
        }

        int numThrows;
        const ELEMENT *back_p, *front_p;

        if (verbose) cout << "\t\t'pushBack' && 'length', matching\n";
        {
            ASSERT(0 == u::myLength(X));

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.pushBack(bslmf::MovableRefUtil::move(vTa[0]));
            } END_EXCEP_TEST_OBJ
            ASSERTV(numThrows, !PLAT_EXC || (expCreateAlloc ? 0 <  numThrows
                                                            : 0 == numThrows));

            ASSERT(1 == u::myLength(X));
            back_p = &u::myBack(X);
            ASSERT(V[0] == *back_p);
            ASSERT(ecm == bsltf::getMovedInto(*back_p));
            ASSERT(ecm == bsltf::getMovedFrom(vTa[0]));

            vTa[0] = V[0];

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.pushBack(bslmf::MovableRefUtil::move(vTa[1]));
            } END_EXCEP_TEST_OBJ
            ASSERTV(numThrows, !PLAT_EXC || (expCreateAlloc ? 0 <  numThrows
                                                            : 0 == numThrows));

            ASSERT(2 == u::myLength(X));
            back_p = &u::myBack(X);
            ASSERT(V[1] == *back_p);
            ASSERT(ecm == bsltf::getMovedInto(*back_p));
            ASSERT(ecm == bsltf::getMovedFrom(vTa[1]));

            front_p = &u::myFront(X);
            ASSERT(V[0] == *front_p);
            ASSERT(ecm == bsltf::getMovedInto(*front_p));

            vTa[1] = V[1];
        }

        if (verbose) cout << "\t\t'pushBack' && 'length', non-matching\n";
        {
            ASSERT(2 == u::myLength(X));

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.pushBack(bslmf::MovableRefUtil::move(vSa[2]));
            } END_EXCEP_TEST_OBJ
            ASSERTV(NameOf<ELEMENT>(), numThrows, !PLAT_EXC ||
                           (u::IsAllocating<ELEMENT>::value ? 0 <  numThrows
                                                            : 0 == numThrows));

            ASSERT(3 == u::myLength(X));
            back_p = &u::myBack(X);
            ASSERT(V[2] == *back_p);

            ASSERT(ecmnma == bsltf::getMovedInto(*back_p));
            ASSERT(ecmnma == bsltf::getMovedFrom(vSa[2]));

            vSa[2] = V[2];

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.pushBack(bslmf::MovableRefUtil::move(vSa[3]));
            } END_EXCEP_TEST_OBJ
            ASSERTV(NameOf<ELEMENT>(), numThrows, !PLAT_EXC ||
                           (u::IsAllocating<ELEMENT>::value ? 0 <  numThrows
                                                            : 0 == numThrows));

            ASSERT(4 == u::myLength(X));
            back_p = &u::myBack(X);
            ASSERT(V[3] == *back_p);
            ASSERT(ecmnma == bsltf::getMovedInto(*back_p));
            ASSERT(ecmnma == bsltf::getMovedFrom(vSa[3]));

            front_p = &u::myFront(X);
            ASSERT(V[0] == *front_p);
            ASSERT(ecm == bsltf::getMovedInto(*front_p));

            vSa[3] = V[3];
        }

        if (verbose) cout << "\t\t'popBack' && 'length' -- matching\n";
        {
            ASSERT(4 == u::myLength(X));
            {
                bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                ASSERT(V[3] == x.popBack());
                ASSERT(V[2] == x.popBack());
            }
            ASSERT(2 == u::myLength(X));

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.popBack(&eTa);
            } END_EXCEP_TEST_OBJ
            ASSERTV(numThrows, eam, NameOf<ELEMENT>(), !PLAT_EXC ||
                         (u::IsAllocating<ELEMENT>::value && e_MOVED != eam) ==
                                                              (0 < numThrows));
            ASSERT(V[1] == eTa);

            ASSERT(V[0] == u::myFront(X));
            ASSERT(V[0] == u::myBack( X));

            {
                bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                ASSERT(V[0] == x.popBack());
            }
            ASSERT(0 == u::myLength(X));
        }

        if (verbose) cout << "\t\t'popBack' && 'length' -- non-matching\n";
        {
            ASSERT(0 == u::myLength(X));
            x.pushBack(V[0]);
            x.pushBack(V[1]);
            x.pushBack(V[2]);
            x.pushBack(V[3]);
            ASSERT(4 == u::myLength(X));

            numThrows = -1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                ++numThrows;

                x.popBack(&eSa);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(NameOf<ELEMENT>(), !PLAT_EXC ||
                           u::IsAllocating<ELEMENT>::value == (0 < numThrows));

            ASSERT(V[3] == eSa);

            ASSERT(3 == u::myLength(X));

            ASSERT(V[0] == u::myFront(X));
            ASSERT(V[2] == u::myBack(X));

            {
                bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                ASSERT(V[0] == x.popFront());
                ASSERT(V[1] == x.popFront());
                ASSERT(V[2] == x.popFront());
            }

            ASSERT(0 == u::myLength(X));
        }

        if (verbose) cout << "\t\t'pushFront' && 'length', matching\n";
        {
            ASSERT(0 == u::myLength(X));

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.pushFront(bslmf::MovableRefUtil::move(vTa[0]));
            } END_EXCEP_TEST_OBJ
            ASSERTV(numThrows, !PLAT_EXC || (expCreateAlloc ? 0 <  numThrows
                                                            : 0 == numThrows));

            ASSERT(1 == u::myLength(X));
            back_p = &u::myFront(X);
            ASSERT(V[0] == *back_p);
            ASSERT(ecm == bsltf::getMovedInto(*back_p));
            ASSERT(ecm == bsltf::getMovedFrom(vTa[0]));

            vTa[0] = V[0];

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.pushFront(bslmf::MovableRefUtil::move(vTa[1]));
            } END_EXCEP_TEST_OBJ
            ASSERTV(numThrows, !PLAT_EXC || (expCreateAlloc ? 0 <  numThrows
                                                            : 0 == numThrows));

            ASSERT(2 == u::myLength(X));
            back_p = &u::myFront(X);
            ASSERT(V[1] == *back_p);
            ASSERT(ecm == bsltf::getMovedInto(*back_p));
            ASSERT(ecm == bsltf::getMovedFrom(vTa[1]));

            front_p = &u::myBack(X);
            ASSERT(V[0] == *front_p);
            ASSERT(ecm == bsltf::getMovedInto(*front_p));

            vTa[1] = V[1];
        }

        if (verbose) cout << "\t\t'pushFront' && 'length', non-matching\n";
        {
            ASSERT(2 == u::myLength(X));

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.pushFront(bslmf::MovableRefUtil::move(vSa[2]));
            } END_EXCEP_TEST_OBJ
            ASSERTV(NameOf<ELEMENT>(), numThrows, !PLAT_EXC ||
                           (u::IsAllocating<ELEMENT>::value ? 0 <  numThrows
                                                            : 0 == numThrows));

            ASSERT(3 == u::myLength(X));
            back_p = &u::myFront(X);
            ASSERT(V[2] == *back_p);

            ASSERT(ecmnma == bsltf::getMovedInto(*back_p));
            ASSERT(ecmnma == bsltf::getMovedFrom(vSa[2]));

            vSa[2] = V[2];

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.pushFront(bslmf::MovableRefUtil::move(vSa[3]));
            } END_EXCEP_TEST_OBJ
            ASSERTV(NameOf<ELEMENT>(), numThrows, !PLAT_EXC ||
                           (u::IsAllocating<ELEMENT>::value ? 0 <  numThrows
                                                            : 0 == numThrows));

            ASSERT(4 == u::myLength(X));
            back_p = &u::myFront(X);
            ASSERT(V[3] == *back_p);
            ASSERT(ecmnma == bsltf::getMovedInto(*back_p));
            ASSERT(ecmnma == bsltf::getMovedFrom(vSa[3]));

            front_p = &u::myBack(X);
            ASSERT(V[0] == *front_p);
            ASSERT(ecm == bsltf::getMovedInto(*front_p));

            vSa[3] = V[3];
        }

        if (verbose) cout << "\t\t'popFront' && 'length' -- matching\n";
        {
            ASSERT(4 == u::myLength(X));
            {
                bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                ASSERT(V[3] == x.popFront());
                ASSERT(V[2] == x.popFront());
            }
            ASSERT(2 == u::myLength(X));

            numThrows = -1;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, x) {
                ++numThrows;

                x.popFront(&eTa);
            } END_EXCEP_TEST_OBJ
            ASSERTV(numThrows, eam, NameOf<ELEMENT>(), !PLAT_EXC ||
                         (u::IsAllocating<ELEMENT>::value && e_MOVED != eam) ==
                                                              (0 < numThrows));
            ASSERT(V[1] == eTa);

            ASSERT(V[0] == u::myBack(X));
            ASSERT(V[0] == u::myFront( X));

            {
                bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                ASSERT(V[0] == x.popFront());
            }
            ASSERT(0 == u::myLength(X));
        }

        if (verbose) cout << "\t\t'popFront' && 'length' -- non-matching\n";
        {
            ASSERT(0 == u::myLength(X));
            x.pushFront(V[0]);
            x.pushFront(V[1]);
            x.pushFront(V[2]);
            x.pushFront(V[3]);
            ASSERT(4 == u::myLength(X));

            numThrows = -1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                ++numThrows;

                x.popFront(&eSa);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(NameOf<ELEMENT>(), !PLAT_EXC ||
                           u::IsAllocating<ELEMENT>::value == (0 < numThrows));

            ASSERT(V[3] == eSa);

            ASSERT(3 == u::myLength(X));

            ASSERT(V[0] == u::myBack(X));
            ASSERT(V[2] == u::myFront(X));

            {
                bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                ASSERT(V[0] == x.popBack());
                ASSERT(V[1] == x.popBack());
                ASSERT(V[2] == x.popBack());
            }

            ASSERT(0 == u::myLength(X));
        }
    }

    ASSERT(0 == ta.numBytesInUse());
    ASSERTV(da.numBlocksTotal() - expDaNumBlocksTotal,
                               0 == da.numBlocksTotal() - expDaNumBlocksTotal);
}

}  // close namespace TEST_CASE_25

//=============================================================================
//                                  TEST CASE 20
//-----------------------------------------------------------------------------

namespace MULTI_THREADED_TRY_PUSH {

const unsigned    nullItem     = ~0;
const unsigned    sequenceMask = 0xffff;

enum { HIGH_WATER_MARK   = 12,
       THREAD_ID_BITS    = 2,
       ID_SHIFT          = 32 - THREAD_ID_BITS,
       NUM_THREADS       = 1 << THREAD_ID_BITS,
       PUSHES_PER_THREAD = 40 * 1000,
       POPS_IN_MAIN      = NUM_THREADS * PUSHES_PER_THREAD };

bslmt::Barrier barrier(NUM_THREADS + 1);

BSLMF_ASSERT(PUSHES_PER_THREAD < sequenceMask);

class BufferedPopper {
    // DATA
    bdlcc::Deque<unsigned>& d_container;
    bsl::vector<unsigned>   d_buffer;
    u::Rand1To8             d_rand1To8;
    const bool              d_popFromFront;
    size_t                  d_currentIndex;

  private:
    // NOT IMPLEMENTED
    BufferedPopper(const BufferedPopper&, bslma::Allocator *);

  public:
    // CREATOR
    BufferedPopper(bdlcc::Deque<unsigned> *container,
                   bool                    popFromFront,
                   bslma::Allocator       *alloc)
    : d_container(*container)
    , d_buffer(alloc)
    , d_rand1To8(20)
    , d_popFromFront(popFromFront)
    , d_currentIndex(0)
        // Create a 'BufferedPopper' object that will pop from the specified
        // '*container', with the end to pop from specified by 'popFromFront',
        // using the specified allocator 'alloc' for allocating the buffer.
    {}

    // MANIPULATOR
    unsigned operator()()
        // Return a single value from the container, using 'd_buffer' to read
        // several at a time and keep them around.  From the caller's point of
        // view, we're just popping a single element from the container each
        // time.
    {
        bool firstTime = true;
        while (isEmpty()) {
            if (firstTime) {
                firstTime = false;

                d_buffer.clear();
                d_currentIndex = 0;
            }
            else {
                bslmt::ThreadUtil::yield();
            }

            const size_t limit = d_rand1To8() + 4;    // '[5 .. 12]'

            ASSERT(d_container.length() <= HIGH_WATER_MARK);
            if (d_popFromFront) {
                d_container.tryPopFront(limit, &d_buffer);
            }
            else {
                d_container.tryPopBack( limit, &d_buffer);
            }
        }

        return d_buffer[d_currentIndex++];
    }

    // ACCESSOR
    bool isEmpty() const
        // Return 'true' if this object contains no items to return without
        // replenishing itself from 'd_container'.
    {
        const size_t bs = d_buffer.size();
        ASSERT(d_currentIndex <= bs);

        return bs == d_currentIndex;
    }
};

// The objects pushed to the deque are 32 bit 'unsigned's.  The high order 2
// bits identify which of 4 pusher threads pushed the object.  The low order 16
// bits increase in sequence for any pusher, and the popper checks the
// sequence.

class MultiThreadedRangeTryPushTest {
    // DATA
    bdlcc::Deque<unsigned>& d_container;       // Container under test, will do
                                               // range-based 'tryPush*' on
                                               // both ends.
    bdlcc::Deque<unsigned>& d_endDeque;        // Not under test, always
                                               // 'pushBack' and 'popFront'.
    unsigned                d_hiId;            // high-order bits indicate
                                               // which of 4 pushers we are.
    unsigned                d_id;              // low-order bits are same as
                                               // high-order bits of 'd_hiId'.

  public:
    // CREATOR
    MultiThreadedRangeTryPushTest(bdlcc::Deque<unsigned>    *container,
                                  unsigned                   id,
                                  bdlcc::Deque<unsigned>    *endDeque)
    : d_container(*container)
    , d_endDeque(*endDeque)
    , d_hiId(id << ID_SHIFT)
    , d_id(id)
        // Create a 'MultiThreadedForcePushTest' object that will access the
        // specified 'container' and have the unique specified 'id'.  The
        // specified 'endDeque' is to be used to contain 'end' items.
    {
        BSLS_ASSERT(id < NUM_THREADS);
    }

    // MANIPULATOR
    void operator()()
        // Push 64K 'unsigned's to the back the of '*d_container_p', then push
        // 64K to the front.
    {
        u::Rand1To8 rand1To8(d_id);
        unsigned array[8];

        for (int isForward = 0; isForward < 2; ++isForward) {
            ASSERT(0 == d_container.length());
            ASSERT(0 == d_endDeque.length());

            barrier.wait();

            if (veryVerbose) bsl::printf("Producer %u %s pass start\n",
                                     d_id, isForward ? "forward" : "backward");

            long unsigned visitFlags = 0;   // Make sure we do pushes of all
                                            // possible lengths.

            for (unsigned u = 0; u < PUSHES_PER_THREAD; ) {
                ASSERTV(d_container.length(),
                                      d_container.length() <= HIGH_WATER_MARK);

                const unsigned left = PUSHES_PER_THREAD - u;
                if (1 == left) {
                    isForward ? d_container.pushBack( nullItem)
                              : d_container.pushFront(nullItem);

                    if (veryVerbose) bsl::printf(
                                              "Producer %u pushed ~0\n", d_id);
                    break;
                }

                const unsigned len = bsl::min(left - 1, rand1To8());
                for (unsigned ii = 0; ii < len; ++ii, ++u) {
                    array[ii] = d_hiId | u;
                }

                const size_t numPushed =
                            isForward
                            ? d_container.tryPushBack( array + 0, array + len)
                            : d_container.tryPushFront(array + 0, array + len);
                ASSERT(numPushed <= len);

                u -= static_cast<unsigned>(len - numPushed);
                if (0 < numPushed) {
                    d_endDeque.pushBack(u++);
                }

                visitFlags |= 1 << numPushed;
            }

            ASSERTV(reinterpret_cast<void *>(visitFlags), d_id,
                                                          0x1ff == visitFlags);
            if (veryVerbose) bsl::printf("Producer %u %s pass finish\n",
                                     d_id, isForward ? "forward" : "backward");

            visitFlags = 0;

            barrier.wait();
        }
    }
};

}  // close namespace MULTI_THREADED_TRY_PUSH

namespace BloombergLP {
namespace bslma {

// This is really unnecessary because the 'BufferedPopper' type is never
// contained in anything else.  We're just doing this to shut bde verify up.

template <>
struct UsesBslmaAllocator<MULTI_THREADED_TRY_PUSH::BufferedPopper> :
                                                                 bsl::true_type
{};

}  // close namespace bslma
}  // close enterprise namespace

//=============================================================================
//                                  TEST CASE 19
//                  SINGLE-THREADED SINGLE ITEM FORCE PUSH TEST
//-----------------------------------------------------------------------------

namespace TEST_CASE_19 {

template <class ELEMENT>
void testForcePushRemoveAllAux(const bool       match,
                               const bool       moveFlag,
                               const bool       pushFront,
                               const u::VecType vecType)
{
    enum { k_NUM_ELEMENTS  = 10,
           k_WELL_BEHAVED  = u::HasWellBehavedMove<ELEMENT>::value,
           k_IS_MOVE_AWARE = u::IsMoveAware<ELEMENT>::value,
           k_IS_MOVE_COPY  = bsl::is_same<ELEMENT,
                                          bsltf::MoveCopyAllocTestType>::value
    };

    // Expected Copyconstruct Movestate

    const char *name = NameOf<ELEMENT>();

    if (veryVeryVerbose) {
        P_(match);    P_(moveFlag);    P_(pushFront);    P(vecType);
    }

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator ta("test",    veryVeryVeryVerbose);
    bslma::TestAllocator oa("other",   veryVeryVeryVerbose);
    bslma::TestAllocator sa("source",  veryVeryVeryVerbose);

    bslma::TestAllocator& ca = !match || u::e_BSL == vecType ? ta : da;
    bslma::TestAllocator& va =  match ? (u::e_BSL == vecType ? ta : da)
                                      : oa;

    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

    bdlcc::Deque<ELEMENT> mX(2, &ca);  const bdlcc::Deque<ELEMENT>& X = mX;

    ASSERT(0 == X.length());

    bsltf::MoveState::Enum expMove = !u::IsMoveAware<ELEMENT>::value
                                   ? e_UNKNOWN
                                   : !moveFlag || (!match && k_IS_MOVE_COPY)
                                   ? e_NOT_MOVED
                                   : e_MOVED;

    bsl::vector<ELEMENT> footprintVec(&va);
    footprintVec.resize(1);
    ELEMENT& e = footprintVec.front();

    if (pushFront) {
        for (unsigned ii = k_NUM_ELEMENTS; 0 < ii--; ) {
            const int c = 'A' + ii;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, mX) {
                u::setData(&e, c);

                if (moveFlag) {
                    mX.forcePushFront(MUtil::move(e));
                }
                else {
                    mX.forcePushFront(e);
                }
            } END_EXCEP_TEST_OBJ
            ASSERTV(ii, X.length(), k_NUM_ELEMENTS - ii == X.length());
            ASSERTV(X, c == u::getData(u::myFront(X)));
            ASSERTV(expMove, bsltf::getMovedInto(u::myFront(X)),
                                expMove == bsltf::getMovedInto(u::myFront(X)));
            ASSERT(expMove == bsltf::getMovedFrom(e));
        }
    }
    else {
        for (unsigned ii = 0; ii < k_NUM_ELEMENTS; ++ii) {
            const int c = 'A' + ii;
            BEGIN_EXCEP_TEST_OBJ(ELEMENT, mX) {
                u::setData(&e, c);

                if (moveFlag) {
                    mX.forcePushBack(MUtil::move(e));
                }
                else {
                    mX.forcePushBack(e);
                }
            } END_EXCEP_TEST_OBJ
            ASSERTV(ii, X.length(), ii + 1 == X.length());
            ASSERTV(X, c == u::getData(u::myBack(X)));
            ASSERTV(expMove, bsltf::getMovedInto(u::myBack(X)),
                                 expMove == bsltf::getMovedInto(u::myBack(X)));
            ASSERT(expMove == bsltf::getMovedFrom(e));
        }
    }

    ELEMENT *pb = 0, *pe = 0;
    bsl::vector<ELEMENT> vBsl(&va);
    std::vector<ELEMENT> vStd;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    std::pmr::vector<ELEMENT> vPmr;
#endif

    switch (vecType) {
      case u::e_BSL: {
        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(va) {
            ++numThrows;

            mX.removeAll(&vBsl);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (PLAT_EXC) {
            ASSERTV(numThrows, 1 <= numThrows);
            ASSERTV(numThrows, !match || !moveFlag ||
                                           !k_IS_MOVE_AWARE || numThrows <= 1);
        }

        pb = u::vData(&vBsl);
        pe = pb + vBsl.size();
      } break;
      case u::e_STD: {
        mX.removeAll(&vStd);

        pb = u::vData(&vStd);
        pe = pb + vStd.size();
      } break;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
      case u::e_PMR: {
        mX.removeAll(&vPmr);

        pb = vPmr.data();
        pe = pb + vPmr.size();
      } break;
#endif
      default : {
        ASSERT(0 && "default");
      } break;
    }

    if (u::e_BSL == vecType) {
        // The behavior of non-bsl vectors with regard to passing allocator
        // args to move constructors varies from platform to platform.

        expMove = !u::IsMoveAware<ELEMENT>::value
                ? e_UNKNOWN
                : !match && k_IS_MOVE_COPY
                ? e_NOT_MOVED
                : e_MOVED;

        for (ELEMENT *p = pb; p < pe; ++p) {
            ASSERTV(name, vecType, expMove, bsltf::getMovedInto(*p),
                                           expMove == bsltf::getMovedInto(*p));
        }
    }

    if (k_WELL_BEHAVED || match) {
        ASSERTV(name, pe - pb, k_NUM_ELEMENTS == pe - pb);
        for (ELEMENT *p = pb; p < pe; ++p) {
            char exp = static_cast<char>('A' + (p - pb));
            ASSERTV(name, vecType, exp, u::getData(*p), exp == u::getData(*p));
        }
    }
    else {
        int prevData = 'A' - 1;
        int numValid = 0;
        for (ELEMENT *p = pb; p < pe; ++p) {
            int d = u::getData(*p);
            if (d) {
                ++numValid;
                ASSERTV(name, p-pb, prevData, *p, prevData < d);
                prevData = d;
            }
        }
        if (veryVerbose) {
            P_(name);    P_(numValid);    P(char(prevData));
        }
    }

    ASSERT(u::e_BSL != vecType || 0 == da.numAllocations());
}

template <class ELEMENT>
void testForcePushRemoveAll()
{
    const char *name = NameOf<ELEMENT>();

    if (verbose) cout << "Force Push Remove All: " << name << endl;

    for (int match = 0; match < 2; ++match) {
        for (int moveFlag = 0; moveFlag < 2; ++moveFlag) {
            for (int pushFront = 0; pushFront < 2; ++pushFront) {
                for (int vi = u::e_BEGIN; vi < u::e_END; ++vi) {
                    const u::VecType vecType = static_cast<u::VecType>(vi);

                    testForcePushRemoveAllAux<ELEMENT>(match,
                                                       moveFlag,
                                                       pushFront,
                                                       vecType);
                }
            }
        }
    }
}

}  // close namespace TEST_CASE_19

//=============================================================================
//                                  TEST CASE 17
//-----------------------------------------------------------------------------

namespace MULTI_THREADED_FORCE_PUSH {

typedef bdlcc::Deque<unsigned> Container;

bslmt::Barrier barrier(5);

const unsigned startEndMask = 1 << 29;
const unsigned sequenceMask = 0xffff;

// The objects pushed to the deque are 32 bit 'unsigned's.  The high order 2
// bits identify which of 4 pusher threads pushed the object.  The bit below
// that (bit 29) is the 'start-end' bit, if multiple objects are pushed by a
// single range push, the first and last objects pushed by that single action
// have this bit set, indicating that the popper is to ensure that all the
// object arrived into the queue as a unit without interruption by any other
// pushers.  The low order 16 bits increase in sequence for any pusher, and the
// popper checks the sequence.

class MultiThreadedForcePushTest {
    // DATA
    Container    *d_container_p;
    unsigned      d_id;
    u::Rand1To8   d_rand1To8;

  public:
    MultiThreadedForcePushTest(Container *container, unsigned id)
    : d_container_p(container)
    , d_id(id << 30)
    , d_rand1To8(id)
        // Create a 'MultiThreadedForcePushTest' object that will access the
        // specified 'container' and have the unique specified 'id'.
    {
        BSLS_ASSERT(id < 4);
    }

    void operator()()
        // Push 64K 'unsigned's to the back the of '*d_container_p', then push
        // 64K to the front.
    {
        unsigned array[8];

        barrier.wait();
        if (veryVerbose) bsl::printf(
                               "Producer %u forward pass start\n", d_id >> 30);

        // Do pushes of 'unsigned's to the back of the container.  3 Types of
        // pushes will be made:
        //: o A move-push of a single item3725
        //: o A copy-push of a single item
        //: o A range-copy-push of 2-7 items
        // The nature of the push is determined by 'len', then number of items,
        // and 'move', if it's a move.
        //
        // We push the loop counter as the value, bitwise-ored with a 2-bit
        // thread id.

        long unsigned visitFlags = 0;   // Make sure we do pushes of all
                                        // possible lengths.

        for (unsigned u = 0; u < 0x10000; ) {
            unsigned len = bsl::min(0x10000 - u, d_rand1To8());
            bool move = false;
            if (8 == len) {
                move = true;
                len = 1;
            }
            visitFlags |= 1 << len;

            if (1 == len) {
                unsigned uu = d_id | u++;
                move ? d_container_p->forcePushBack(MUtil::move(uu))
                     : d_container_p->forcePushBack(uu);
            }
            else {
                for (unsigned ii = 0; ii < len; ++ii, ++u) {
                    array[ii] = d_id | u;
                }
                array[0]       |= startEndMask;
                array[len - 1] |= startEndMask;

                d_container_p->forcePushBack(array + 0, array + len);
            }
        }

        ASSERTV(u::p(visitFlags), (d_id >> 30), 0xfe == visitFlags);
        if (veryVerbose) bsl::printf(
                              "Producer %u forward pass finish\n", d_id >> 30);
        visitFlags = 0;
        barrier.wait();
        ASSERT(0 == d_container_p->length());
        barrier.wait();
        if (veryVerbose) bsl::printf(
                              "Producer %u backward pass start\n", d_id >> 30);

        // Do pushes of 'unsigned's to the back of the container.  3 Types of
        // pushes will be made:
        //: o A move-push of a single item
        //: o A copy-push of a single item
        //: o A range-copy-push of 2-7 items
        // The nature of the push is determined by 'len', then number of items,
        // and 'move', if it's a move.
        //
        // We push the loop counter as the value, bitwise-ored with a 2-bit
        // thread id.

        for (unsigned u = 0; u < 0x10000; ) {
            unsigned len = bsl::min(0x10000 - u, d_rand1To8());
            bool move = false;
            if (8 == len) {
                move = true;
                len = 1;
            }
            visitFlags |= 1 << len;

            if (1 == len) {
                unsigned uu = d_id | u++;
                move ? d_container_p->forcePushFront(MUtil::move(uu))
                     : d_container_p->forcePushFront(uu);
            }
            else {
                for (unsigned ii = 0; ii < len; ++ii, ++u) {
                    array[ii] = d_id | u;
                }
                array[0]       |= startEndMask;
                array[len - 1] |= startEndMask;

                d_container_p->forcePushFront(array + 0, array + len);
            }
        }

        ASSERTV(u::p(visitFlags), (d_id >> 30), 0xfe == visitFlags);
        if (veryVerbose) bsl::printf(
                             "Producer %u backward pass finish\n", d_id >> 30);
    }
};

}  // close namespace MULTI_THREADED_FORCE_PUSH

//=============================================================================
//                                  TEST CASE 16
//-----------------------------------------------------------------------------

namespace FORCE_AND_TRY_HIGH_WATER_MARK_STRESS_TEST {

struct FATHWMStressTest {
    Obj            *d_container_p;
    bslmt::Barrier *d_barrier_p;

    // CREATOR
    FATHWMStressTest(Obj *container, bslmt::Barrier *barrier)
    : d_container_p(container)
    , d_barrier_p(barrier)
        // Create a test object that will access the specified '*container' and
        // wait on the specified '*barrier'.
    {}

    // ACCESSOR
    void operator()() const
        // Do 3 sequences of push's, using alternating 'tryPush*' and 'push*',
        // and make sure that all branches were visited.
    {
        const unsigned len = static_cast<unsigned>(d_container_p->length());
        ASSERT(len < 40);

        if (0 < len) {
            ConstProctor p(d_container_p);
            ASSERTV(len, p->back(), len - 1 == p->back());
        }

        d_barrier_p->wait();

        // First run: container was pre-populated by range c'tor

        bsls::Types::UintPtr visitedMask = 0;

        bool tryFlag = true;
        for (Element e = len; e < 40; ++e) {
            if (tryFlag) {
                bool success = 0 == d_container_p->tryPushBack(e);
                e       -= !success;
                tryFlag &=  success;
                visitedMask |= success ? 0x1 : 0x2;
            }
            else {
                d_container_p->pushBack(e);
                tryFlag = true;
                visitedMask |= 0x4;
            }
        }

        d_barrier_p->wait();

        // Second run: container was prepopulated by 'forcePushBack' and
        // 'tryPushBack's.

        tryFlag = true;
        for (Element e = len; e < 40; ++e) {
            if (tryFlag) {
                bool success = 0 == d_container_p->tryPushBack(e);
                e       -= !success;
                tryFlag &=  success;
                visitedMask |= success ? 0x8 : 0x10;
            }
            else {
                d_container_p->pushBack(e);
                tryFlag = true;
                visitedMask |= 0x20;
            }
        }

        d_barrier_p->wait();

        // Third run: container was prepopulated by 'forcePushFront' and
        // 'tryPushFront's.

        tryFlag = true;
        for (Element e = len; e < 40; ++e) {
            if (tryFlag) {
                bool success = 0 == d_container_p->tryPushFront(e);
                e       -= !success;
                tryFlag &=  success;
                visitedMask |= success ? 0x40 : 0x80;
            }
            else {
                d_container_p->pushFront(e);
                tryFlag = true;
                visitedMask |= 0x100;
            }
        }

        ASSERTV(reinterpret_cast<void *>(visitedMask), 0x1ff == visitedMask);
                                                       // all branches visited.
    }
};

}  // close namespace FORCE_AND_TRY_HIGH_WATER_MARK_STRESS_TEST

//=============================================================================
//                                  TEST CASE 15
//-----------------------------------------------------------------------------

namespace HIGH_WATER_MARK_STRESS_TEST {

struct HWMStressTest {
    Obj            *d_container_p;
    bslmt::Barrier *d_barrier_p;

    // CREATOR
    HWMStressTest(Obj *container, bslmt::Barrier *barrier)
    : d_container_p(container)
    , d_barrier_p(barrier)
        // Create an 'HWMStressTest' object referring to the specified
        // 'container' and the specified 'barrier'.
    {}

    // ACCESSOR
    void operator()() const
        // Push a fixed number of elements to the back, then the front of the
        // queue to evaluate whether the high water mark is respected.
    {
        for (int ii = 0; ii < 40; ++ii) {
            if (ii & 1) {
                d_container_p->pushBack(ii);
            }
            else {
                Element e(ii);
                d_container_p->pushBack(MUtil::move(e));
            }
        }

        d_barrier_p->wait();
        ASSERT(0 == d_container_p->length());

        for (int ii = 0; ii < 40; ++ii) {
            if (ii & 1) {
                d_container_p->pushFront(ii);
            }
            else {
                Element e(ii);
                d_container_p->pushFront(MUtil::move(e));
            }
        }
    }
};

}  // close namespace HIGH_WATER_MARK_STRESS_TEST

namespace RANDOM_PUSH_POP_TEST {

enum {
    NUM_TO_PUSH     = 50 * 1000,
    NUM_TO_POP      = NUM_TO_PUSH * 2,
    HIGH_WATER_MARK = 20,
    NUM_PUSHERS     = 8,
    NUM_POPPERS     = NUM_PUSHERS / 2
};

struct Item {
    int       d_pusherIdx;
    unsigned  d_sequenceNum;
};

typedef bdlcc::Deque<Item>::Proctor Proctor;

// 'pusherTotals' are the sums of the numbers pusher by each pusher, aggregated
// by the pushers themselves.  'popperTotalsByPusher' are the same sums, only
// aggregated by the poppers.  If any item gets lost, duplicated, or garbled in
// the deque, the two values will not be equal.  If the container performs as
// it should, the numbers will be equal.

bsls::AtomicInt64 popperTotalsByPusher[NUM_PUSHERS];    // default 0
Int64             pusherTotals[        NUM_PUSHERS];

bdlcc::Deque<Item>          deque(HIGH_WATER_MARK,
                                  &bslma::NewDeleteAllocator::singleton());

bsls::AtomicInt             seedMain(123456789);
bsls::AtomicInt             pusherIdxMain(0);

bslmt::Barrier              barrier(NUM_PUSHERS + NUM_POPPERS);

struct PusherThread {
    void operator()();
        // Push 'NUM_TO_PUSH' items randomly to the deque.
};

void PusherThread::operator()()
{
    u::RandGen   randGenerator(seedMain += 987654321);
    Item         item       = { pusherIdxMain++, 0 };
    Int64        localTotal = 0;

    barrier.wait();

    for (int ii = 0; ii < NUM_TO_PUSH; ++ii) {
        unsigned rand      =  randGenerator();
        item.d_sequenceNum =  rand;
        localTotal         += rand;

        switch (rand & 3) {
          case (0): {
            deque.pushFront(item);
          } break;
          case (1): {
            deque.pushBack(item);
          } break;
          case (2): {
            Proctor proctor(&deque);
            proctor->push_front(item);
          } break;
          case (3): {
            Proctor proctor(&deque);
            proctor->push_back(item);
          } break;
          default: {
            ASSERT(0);
          }
        }
    }

    pusherTotals[item.d_pusherIdx] = localTotal;
}

struct PopperThread {
    void operator()();
        // Pop 'NUM_TO_POP' items randomly from the deque.
};

void PopperThread::operator()()
{
    int        seed = seedMain += 987654321;
    u::RandGen randGen(seed);

    Int64 localTotalsByPusher[NUM_PUSHERS] = { 0 };

    barrier.wait();

    for (int ii = 0; ii < NUM_TO_POP; ++ii) {
        Item item;

        switch (randGen() & 7) {
          case (0):
          case (1): {
            item = deque.popFront();
          } break;
          case (2):
          case (3): {
            item = deque.popBack();
          } break;
          case (4): {
            deque.popFront(&item);
          } break;
          case (5): {
            deque.popBack(&item);
          } break;
          case (6): {
            Proctor proctor(&deque);
            if (proctor->empty()) {
                --ii;
                continue;
            }

            item = proctor->front();
            proctor->pop_front();
          } break;
          case (7): {
            Proctor proctor(&deque);
            if (proctor->empty()) {
                --ii;
                continue;
            }

            item = proctor->back();
            proctor->pop_back();
          } break;
          default: {
            ASSERT(0);
          }
        }

        localTotalsByPusher[item.d_pusherIdx] += item.d_sequenceNum;
    }

    for (int jj = 0; jj < NUM_PUSHERS; ++jj) {
        popperTotalsByPusher[jj] += localTotalsByPusher[jj];
    }
}

}  // close namespace RANDOM_PUSH_POP_TEST

//=============================================================================
//                          SEQUENCE CONSTRAINT TEST
//-----------------------------------------------------------------------------

namespace SEQUENCE_CONSTRAINT_TEST {

enum {
    NUM_ITERATIONS  = 2000,
    HIGH_WATER_MARK = 100,
    NUM_THREADS     = 9
};

struct Item {
    int  d_threadId;
    int  d_sequenceNum;
};

bslma::Allocator   *alloc;

bdlcc::Deque<Item> *container;

bool                backwards;       // If true, push front and pop back,
                                     // otherwise push back and pop front.
bool                popThroughPtr;   // If true, use 'pop...(&item)', otherwise
                                     // use item == 'pop...()'.

bslmt::Barrier     *barrier;

int                 totalToPop;
int                 numPushers;

bsls::AtomicInt     nextThreadId;
bsls::AtomicInt     numPopped;

struct PusherThread {
    void operator()() const;
        // Push 'NUM_ITERATIONS' items onto one end of the queue.
};

void PusherThread::operator()() const
{
    barrier->wait();

    int threadId = nextThreadId++;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        Item item;
        item.d_threadId = threadId;
        item.d_sequenceNum = i;
        if (0 == i % 100) {
            ASSERT(i + 10 <= NUM_ITERATIONS);

            // Note that when we push through a proctor, we may exceed the
            // high water mark.  This should stop this test from passing.

            bdlcc::Deque<Item>::Proctor proctor(container);
            for (int j = 0; j < 10; ++j, ++i) {
                item.d_sequenceNum = i;
                if (backwards) {
                    proctor->push_front(item);
                }
                else {
                    proctor->push_back( item);
                }
            }
            --i;
        }
        else {
            if (backwards) {
                container->pushFront(item);
            }
            else {
                container->pushBack( item);
            }
        }
    }
}

struct PopperThread {
    void operator()() const;
        // Pop items off of one end of the queue until 'numPopped >= numToPop'.
};

void PopperThread::operator()() const
{
    barrier->wait();

    bsl::vector<int> seq(numPushers, -1, alloc);

    int numTicksThisThread = 0;
    for (; numPopped++ < totalToPop; ++numTicksThisThread) {
        Item item;

        bool wasPopped = false;
        if (0 == numTicksThisThread % 50) {
            bdlcc::Deque<Item>::Proctor proctor(container);

            if (!proctor->empty()) {
                // Use the proctor to pop the element.

                if (backwards) {
                    item = proctor->back();
                    proctor->pop_back();
                }
                else {
                    item = proctor->front();
                    proctor->pop_front();
                }

                wasPopped = true;
            }
        }

        if (!wasPopped) {
            if (backwards) {
                if (popThroughPtr) {
                    container->popBack(&item);
                }
                else {
                    item = container->popBack();
                }
            }
            else {
                if (popThroughPtr) {
                    container->popFront(&item);
                }
                else {
                    item = container->popFront();
                }
            }
        }

        ASSERT(seq[item.d_threadId] < item.d_sequenceNum);
        seq[item.d_threadId] = item.d_sequenceNum;
    }
}

}  // close namespace SEQUENCE_CONSTRAINT_TEST

//=============================================================================
//                                  TEST CASE 12
//-----------------------------------------------------------------------------

namespace TEST_CASE_12 {

template <class VECTOR>
class TestPopFront {
    typedef typename VECTOR::value_type      ELEMENT;
    typedef bslalg::ConstructorProxy<VECTOR> ProxyVector;

    enum { k_IS_MOVE_AWARE = u::IsMoveAware<ELEMENT>::value };

    static const u::VecType s_vecType =
                         bsl::is_same<bsl::vector<ELEMENT>, VECTOR>::value
                       ? u::e_BSL
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                       : bsl::is_same<std::pmr::vector<ELEMENT>, VECTOR>::value
                       ? u::e_PMR
#endif
                       : u::e_STD;

    bdlcc::Deque<ELEMENT> *d_mX_p;
    bslma::TestAllocator  *d_alloc_p;

  public:
    TestPopFront(bdlcc::Deque<ELEMENT> *mX,
                 bslma::TestAllocator  *alloc)
        // Create a 'TestPopFront' object accessing the specified 'mX' and
        // using the specified 'alloc'.
    : d_mX_p(mX)
    , d_alloc_p(alloc)
    {
        ASSERT(d_mX_p->allocator() == d_alloc_p);
    }

    void operator()()
        // Pop 50 items off the front with 'tryPopFront' and verify their
        // values are as expected.
    {
        int                    expectedVal  = 0;
        ProxyVector            pv(d_alloc_p);
        VECTOR&                v = pv.object();
        IntPtr                 maxVecSize   = 0;
        int                    maxVecSizeAt = 0;
        bsltf::MoveState::Enum vms = !k_IS_MOVE_AWARE
                                   ? e_UNKNOWN
                                   : e_MOVED;
        bsltf::MoveState::Enum eam = !k_IS_MOVE_AWARE
                                   ? e_UNKNOWN
                                   : expAssignMoveState;
        bsl::vector<ELEMENT>   eFootprint(d_alloc_p);
        eFootprint.resize(1);
        ELEMENT&               e = eFootprint.front();

        v.reserve(20);

        while (expectedVal < 50) {
            u::setData(&e, -1);

            int sts = d_mX_p->tryPopFront(&e);
            if (!sts) {
                ASSERTV(expectedVal, u::getData(e),
                                 expectedVal++ == u::getData(e) && "popFront");
                ASSERTV(eam, eam == bsltf::getMovedInto(e));
            }

            v.clear();
            d_mX_p->tryPopFront(20, &v);
            IntPtr s = v.size();
            for (int ii = 0; ii < s; ++ii) {
                const int d = u::getData(v[ii]);
                ASSERTV(expectedVal, d, expectedVal++ == d && "popFront");
                ASSERTV(vms, bsltf::getMovedInto(v[ii]),
                   u::e_BSL != s_vecType || vms == bsltf::getMovedInto(v[ii]));
            }
            if (s > maxVecSize) {
                maxVecSize = v.size();
                maxVecSizeAt = expectedVal;
            }
        }

        if (veryVerbose) {
            cout << "    PopFront: max vecsize = " << maxVecSize <<
                    " at " << maxVecSizeAt << endl;
        }
    }
};

template <class VECTOR>
class TestPopBack {
    typedef typename VECTOR::value_type      ELEMENT;
    typedef bslalg::ConstructorProxy<VECTOR> ProxyVector;

    enum { k_IS_MOVE_AWARE = u::IsMoveAware<ELEMENT>::value };

    static const u::VecType s_vecType =
                         bsl::is_same<bsl::vector<ELEMENT>, VECTOR>::value
                       ? u::e_BSL
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                       : bsl::is_same<std::pmr::vector<ELEMENT>, VECTOR>::value
                       ? u::e_PMR
#endif
                       : u::e_STD;

    bdlcc::Deque<ELEMENT> *d_mX_p;
    bslma::TestAllocator  *d_alloc_p;

  public:
    TestPopBack(bdlcc::Deque<ELEMENT> *mX,
                bslma::TestAllocator  *alloc)
        // Create a 'TestPopBack' object accessing the specified 'mX' and using
        // the specified 'alloc'.
    : d_mX_p(mX)
    , d_alloc_p(alloc)
    {
        ASSERT(d_mX_p->allocator() == d_alloc_p);
    }

    void operator()()
        // Pop 50 items off the back with 'tryPopBack' and verify their values
        // are as expected.
    {
        int                    expectedVal  = 0;
        ProxyVector            pv(d_alloc_p);
        VECTOR&                v = pv.object();
        IntPtr                 maxVecSize   = 0;
        int                    maxVecSizeAt = 0;
        bsltf::MoveState::Enum vms = !k_IS_MOVE_AWARE
                                   ? e_UNKNOWN
                                   : e_MOVED;
        bsltf::MoveState::Enum eam = !k_IS_MOVE_AWARE
                                   ? e_UNKNOWN
                                   : expAssignMoveState;
        bsl::vector<ELEMENT>   eFootprint(d_alloc_p);
        eFootprint.resize(1);
        ELEMENT&               e = eFootprint.front();

        v.reserve(20);

        while (expectedVal < 50) {
            u::setData(&e, -1);

            int sts = d_mX_p->tryPopBack(&e);
            if (!sts) {
                ASSERTV(s_vecType, expectedVal, u::getData(e),
                                  expectedVal++ == u::getData(e) && "popBack");
                ASSERTV(eam, eam == bsltf::getMovedInto(e));
            }

            v.clear();

            d_mX_p->tryPopBack(20, &v);
            IntPtr s = v.size();
            for (int ii = 0; ii < s; ++ii) {
                const int d = u::getData(v[ii]);
                ASSERTV(s_vecType, expectedVal, d,
                                              expectedVal++ == d && "popBack");
                ASSERTV(vms, bsltf::getMovedInto(v[ii]),
                   u::e_BSL != s_vecType || vms == bsltf::getMovedInto(v[ii]));
            }
            if (s > maxVecSize) {
                maxVecSize = v.size();
                maxVecSizeAt = expectedVal;
            }
        }

        if (veryVerbose) {
            cout << "    PopBack: max vecsize = " << maxVecSize <<
                    " at " << maxVecSizeAt << endl;
        }
    }
};

template <class ELEMENT>
void testMultiThreadedTryPop()
{
    const char *name = NameOf<ELEMENT>();
    if (verbose) cout << "testMultiThreadedTryPop<" << name << ">()\n";

    bslma::TestAllocator         ta(veryVeryVeryVerbose);
    bslma::TestAllocator         da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

    {
        bdlcc::Deque<ELEMENT> mX(&ta);
        const bdlcc::Deque<ELEMENT>& X = mX;

        bslmt::ThreadUtil::Handle handle;

        bsl::vector<ELEMENT> footprintVec(&ta);
        footprintVec.resize(1);
        ELEMENT& ee = footprintVec[0];

        for (int run = u::e_BEGIN; run < u::e_END * 3; ++run) {
            const u::VecType vecType = static_cast<u::VecType>(run / 3);

            if (veryVerbose) cout << "  " << vecType << "::vector<" << name <<
                                                 ">, run: " << run % 3 << endl;

            switch (vecType) {
              case u::e_BSL: {
                TestPopFront<bsl::vector<ELEMENT> > frontFunc(&mX, &ta);
                bslmt::ThreadUtil::create(&handle, frontFunc);
              } break;
              case u::e_STD: {
                TestPopFront<std::vector<ELEMENT> > frontFunc(&mX, &ta);
                bslmt::ThreadUtil::create(&handle, frontFunc);
              } break;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
              case u::e_PMR: {
                TestPopFront<std::pmr::vector<ELEMENT> > frontFunc(&mX, &ta);
                bslmt::ThreadUtil::create(&handle, frontFunc);
              } break;
#endif
              default: {
                ASSERT(0 && "default");
              }
            }
            bslmt::ThreadUtil::microSleep(10 * 1000);
            for (int e = 0; e < 50; ++e) {
                if (3 == e % 7 || 1 == e % 9) {
                    if (veryVeryVerbose) {
                        cout << "      PopFront: wait at " << e << endl;
                    }
                    bslmt::ThreadUtil::yield();
                    bslmt::ThreadUtil::microSleep(10 * 1000);
                }
                u::setData(&ee, e);
                mX.pushBack(ee);
            }
            bslmt::ThreadUtil::join(handle);
            ASSERT(0 == X.length());

            switch (vecType) {
              case u::e_BSL: {
                TestPopBack<bsl::vector<ELEMENT> > backFunc(&mX, &ta);
                bslmt::ThreadUtil::create(&handle, backFunc);
              } break;
              case u::e_STD: {
                TestPopBack<std::vector<ELEMENT> > backFunc(&mX, &ta);
                bslmt::ThreadUtil::create(&handle, backFunc);
              } break;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
              case u::e_PMR: {
                TestPopBack<std::pmr::vector<ELEMENT> > backFunc(&mX, &ta);
                bslmt::ThreadUtil::create(&handle, backFunc);
              } break;
#endif
              default: {
                ASSERT(0 && "default");
              }
            }
            bslmt::ThreadUtil::microSleep(10 * 1000);
            for (int e = 0; e < 50; ++e) {
                if (3 == e % 6 || 5 == e % 11) {
                    if (veryVeryVerbose) {
                        cout << "      PopBack: wait at " << e << endl;
                    }
                    bslmt::ThreadUtil::yield();
                    bslmt::ThreadUtil::microSleep(10 * 1000);
                }
                u::setData(&ee, e);
                mX.pushFront(ee);
            }
            bslmt::ThreadUtil::join(handle);
            ASSERT(0 == X.length());

            ASSERT(u::e_BSL != vecType || 0 == da.numAllocations());
        }
    }

    ASSERT(0 == ta.numBlocksInUse());
}

}  // close namespace TEST_CASE_12

//=============================================================================
//                                  TEST CASE 11
//-----------------------------------------------------------------------------

namespace TEST_CASE_11 {

enum {
    THREAD_EXIT_SUCCESS = 57,
    THREAD_EXIT_FAIL    = 68,

    FRONT_VAL           = 45,
    BACK_VAL            = 46 };

static unsigned pushCount;

int threadResult = -1;

template <class ELEMENT>
class HighWaterMarkFunctor {
    bdlcc::Deque<ELEMENT> *d_deque_p;
    bsls::TimeInterval     d_timeout;

  public:
    explicit
    HighWaterMarkFunctor(bdlcc::Deque<ELEMENT> *deque)
        // Create a 'HighWaterMarkFunctor' object accessing the specified
        // 'deque'.
    : d_deque_p(deque)
    {
        // have everything time out 2 seconds after thread object creation

        d_timeout = u::now() + bsls::TimeInterval(4.0);
    }

    ~HighWaterMarkFunctor()
        // make sure we did not wait until timeout
    {
        ASSERT(u::now() < d_timeout);
    }

    void operator()()
        // thread function
    {
        const char *name = NameOf<ELEMENT>();

        // Push 32 objects to the front and back of the 'Deque', the objects
        // pushed to the front should have value 'FRONT_VAL', the objects
        // pushed to the back should the value 'BACK_VAL'.  We use 8 different
        // pushes, half of them moves.  The timed pushes should never time out.
        // The main thread will pop off the front and back of the 'Deque',
        // observing that values are as expected.

        // The popping thread will be sleeping periodically, and there is a
        // high water mark, so many of these pushes will block for awhile
        // before completing.

        bslma::Allocator *a_p = d_deque_p->allocator();
        bsl::vector<ELEMENT> footprintVec(a_p);
        footprintVec.resize(4);
        u::setData(&footprintVec[0], FRONT_VAL);
        const ELEMENT&  f = footprintVec[0];
        u::setData(&footprintVec[1], BACK_VAL);
        const ELEMENT&  b = footprintVec[1];
        ELEMENT&       mf = footprintVec[2];
        ELEMENT&       mb = footprintVec[3];

        for (int ii = 0; ii < 32; ++ii) {
            int jj = ii % 8;

            mf = f;
            ASSERTV(name, ii, bsltf::getMovedFrom(mf),
                                           e_MOVED != bsltf::getMovedFrom(mf));
            mb = b;
            ASSERTV(name, ii, bsltf::getMovedFrom(mb),
                                           e_MOVED != bsltf::getMovedFrom(mb));

            int sts = 0;
            switch (jj) {
              case 0: {
                d_deque_p->pushBack( b);
              } break;
              case 1: {
                d_deque_p->pushFront(f);
              } break;
              case 2: {
                sts = d_deque_p->timedPushBack( b, d_timeout);
              } break;
              case 3: {
                sts = d_deque_p->timedPushFront(f, d_timeout);
              } break;
              case 4: {
                d_deque_p->pushBack( MUtil::move(mb));
              } break;
              case 5: {
                d_deque_p->pushFront(MUtil::move(mf));
              } break;
              case 6: {
                sts = d_deque_p->timedPushBack( MUtil::move(mb), d_timeout);
              } break;
              case 7: {
                sts = d_deque_p->timedPushFront(MUtil::move(mf), d_timeout);
              } break;
            }
            if (sts) {
                ASSERTV(ii, jj, sts, 0 == sts);
                threadResult = THREAD_EXIT_FAIL;
                return;                                               // RETURN
            }

            if (4 <= jj && u::IsMoveAware<ELEMENT>::value) {
                ELEMENT& mRef = 0 == (jj & 1) ? mb : mf;
                ASSERTV(name, ii, bsltf::getMovedFrom(mRef),
                                         e_MOVED == bsltf::getMovedFrom(mRef));
            }

            ++pushCount;
        }

        threadResult = THREAD_EXIT_SUCCESS;
    }
};

template <class ELEMENT>
void highWaterMarkTest()
{
    bslma::TestAllocator         ta(veryVeryVeryVerbose);
    bslma::TestAllocator         da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

    const char *name = NameOf<ELEMENT>();

    if (verbose) cout << "highWaterMarkTest<" << name << ">()\n";

    {
        pushCount = 0;

        bdlcc::Deque<ELEMENT> mX(4, &ta);
        const bdlcc::Deque<ELEMENT>& X = mX;

        bsls::TimeInterval start = u::now();

        ASSERT(0 == X.length());

        HighWaterMarkFunctor<ELEMENT> functor(&mX);

        bslmt::ThreadUtil::Handle handle;
        bslmt::ThreadUtil::create(&handle, functor);

        for (unsigned u = 0; u <= 32 - 4; ++u) {
            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(50 * 1000);        // 50 mSec

            ASSERT(4 + u == pushCount);
            ASSERT(4     == X.length());

            if (veryVerbose) { P_(u);    P(ta.numBlocksInUse()); }

            if (u & 1) {
                const int val = u::getData(mX.popBack());
                ASSERTV(name, val, u, BACK_VAL  == val);
            }
            else {
                const int val = u::getData(mX.popFront());
                ASSERTV(name, val, u, FRONT_VAL == val);
            }
        }

        ASSERT(3 == X.length());

        bslmt::ThreadUtil::join(handle);
        ASSERTV(threadResult, THREAD_EXIT_SUCCESS == threadResult);

        if (veryVerbose) cout << "After join: " << ta.numBlocksInUse()
                                                << endl;

        ASSERT(3 == X.length());
        ASSERT(BACK_VAL  == u::getData(mX.popBack()));
        ASSERT(FRONT_VAL == u::getData(mX.popFront()));
        ASSERT(BACK_VAL  == u::getData(mX.popBack()));

        // On Solaris, each sleep could potentially take 2 seconds, but
        // usually it will be much, much less.

        ASSERT(u::now() < start + bsls::TimeInterval(10.1));
    }

    ASSERTV(ta.numBlocksInUse(), 0 == ta.numBlocksInUse());
    ASSERTV(da.numBlocksInUse(), 0 == da.numBytesInUse());
}

}  // close namespace TEST_CASE_11

//=============================================================================
//                                  TEST CASE 10
//-----------------------------------------------------------------------------

namespace TEST_CASE_10 {

enum {
    VALID_VAL = 45,
    TERMINATE = 46
};

class EmptyDequeFunctor {
    // DATA
    Obj                *d_deque_p;
    bslmt::Barrier     *d_barrier_p;
    int                *d_status_p;
    bsls::TimeInterval  d_timeout;

  public:
    EmptyDequeFunctor(Obj *deque, bslmt::Barrier *barrier, int *status)
        // Create a test object that will access the specified '*deque', block
        // on the specified '*barrier', and assign an exit status to the
        // specified 'status'.  The exit status is set to 0 when an element
        // having the 'TERMINATE' value is found on 'deque' and a non-zero
        // value if a blocking operation on 'deque' or 'barrier' times out.
    : d_deque_p(deque)
    , d_barrier_p(barrier)
    , d_status_p(status)
    {
        ASSERT(deque);
        ASSERT(barrier);
        ASSERT(status);

        // have everything time out 4 seconds after thread object creation

        d_timeout = u::now() + bsls::TimeInterval(4.0);
    }

    ~EmptyDequeFunctor()
        // make sure we did not wait until timeout
    {
        ASSERT(u::now() < d_timeout);
    }

    void operator()()
        // thread function
    {
        Element e;
        int     sts;

        for (bool back = false; true; back = !back) {
            ASSERT((sts = d_barrier_p->timedWait(d_timeout), !sts));
            if (sts) {
                ASSERT(bslmt::Barrier::e_TIMED_OUT == sts);
                *d_status_p = 2;
                break;
            }

            if (back) {
                ASSERT((sts = d_deque_p->timedPopBack( &e, d_timeout), !sts));
            }
            else {
                ASSERT((sts = d_deque_p->timedPopFront(&e, d_timeout), !sts));
            }
            if (sts) {
                ASSERT(bslmt::Barrier::e_TIMED_OUT == sts);
                *d_status_p = 1;
                break;
            }

            if (TERMINATE == e) {
                *d_status_p = 0;
                break;
            }
            ASSERT(VALID_VAL == e);

            sts = d_barrier_p->timedWait(d_timeout);
            ASSERT(!sts);
            if (sts) {
                ASSERT(bslmt::Barrier::e_TIMED_OUT == sts);
                *d_status_p = 3;
                break;
            }
        }
    }
};

}  // close namespace TEST_CASE_10

//=============================================================================
//                                  TEST CASE 9
//-----------------------------------------------------------------------------

namespace TEST_CASE_9 {

template <class VECTOR>
void testSingleThreadedTryPop()
{
    typedef typename VECTOR::value_type             ELEMENT;
    typedef typename bdlcc::Deque<ELEMENT>::Proctor Proctor;
    typedef bslalg::ConstructorProxy<VECTOR>        ProxyVector;

    const char *name = NameOf<VECTOR>();

    const int vecType = bsl::is_same<bsl::vector<ELEMENT>, VECTOR>::value
                      ? u::e_BSL
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                      : bsl::is_same<std::pmr::vector<ELEMENT>, VECTOR>::value
                      ? u::e_PMR
#endif
                      : u::e_STD;

    enum { k_NUM_ELEMENTS  = 10,
           k_WELL_BEHAVED  = u::HasWellBehavedMove<ELEMENT>::value,
           k_IS_MOVE_AWARE = u::IsMoveAware<ELEMENT>::value,
           k_ALLOC         = u::IsAllocating<ELEMENT>::value
        };

    bslma::TestAllocator         oa(veryVeryVeryVerbose);
    bslma::TestAllocator         ta(veryVeryVeryVerbose);
    bslma::TestAllocator         da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

    if (verbose) cout << "\n\nTry pop " << NameOf<ELEMENT>() << ' ' <<
                        (k_WELL_BEHAVED ? "" : "not ") << "well behaved, " <<
                        (k_IS_MOVE_AWARE ? "" : "not ") << "move aware, " <<
                        (k_ALLOC ? "allocates" : "doesn't allocate") << endl;

    for (int ti = 0; ti < 2; ++ti) {
        const bool match = ti & 1;

        bslma::TestAllocator& xa = u::e_BSL == vecType
                                 ? ta
                                 : (match ? da : ta);
        bslma::TestAllocator& va = u::e_BSL == vecType
                                 ? (match ? ta : oa)
                                 : da;

        bdlcc::Deque<ELEMENT> mX(&xa);    const bdlcc::Deque<ELEMENT>& X = mX;
        bsl::deque<ELEMENT>   mY(&xa);    const bsl::deque<ELEMENT>&   Y = mY;
        ProxyVector           pv(&va);
        VECTOR&               v = pv.object();
        bsl::vector<ELEMENT>  eFootprint(&va);
        eFootprint.resize(1);
        ELEMENT&              e = eFootprint.front();
        int                   sts;

        if (veryVerbose) cout << "\tPopFront: " <<
                                     (match ? "" : "no ") << "match, " << endl;

        ASSERT(!X.length());

        u::setData(&e, 'Z');
        sts = mX.tryPopFront(&e);
        ASSERT(0 != sts);
        ASSERT('Z' == u::getData(e));
        mX.tryPopFront(100, &v);
        ASSERT(v.empty());
        mX.tryPopFront(100);    // doesn't block
        v.reserve(10);

        ASSERT(!X.length());

        for (int i = 0; i < 10; ++i) {
            u::setData(&e, 'A' + i);
            mX.pushBack(e);
        }
        u::setData(&e, 'Z');

        mX.tryPopFront(1);
        ASSERT('Z' == u::getData(e));
        {
            Proctor pX(&mX);
            mY = *pX;
        }
        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(va) {
            ++numThrows;

            sts = mX.tryPopFront(&e);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERTV(numThrows, NameOf<ELEMENT>(), match, !PLAT_EXC ||
                 (k_ALLOC && (!match || !k_IS_MOVE_AWARE || !expAssignMove)) ==
                                                              (0 < numThrows));
        ASSERT(0 == sts);
        ASSERT('A' + 1 == u::getData(e));
        mY.pop_front();
        {
            Proctor pX(&mX);
            ASSERTV(Y, *pX, Y == *pX);
        }

        numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(va) {
            ++numThrows;

            mX.tryPopFront(4, &v);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        if (u::e_BSL == vecType) {
            // Note that we only do this check when we know that 'v' is a bsl
            // vector.  Whether the move c'tors of standard library vectors
            // pass allocator args varies by platform.  We similarly do this
            // check only for bsl vectors below.

            ASSERTV(numThrows, NameOf<ELEMENT>(), match, !PLAT_EXC ||
                                   (k_ALLOC && (!match || !k_IS_MOVE_AWARE)) ==
                                                              (0 < numThrows));
        }

        ASSERT(4 == v.size());
        if (k_WELL_BEHAVED || !k_IS_MOVE_AWARE) {
            ASSERT('A' + 2 == u::getData(v.front()));
            ASSERT('A' + 5 == u::getData(v.back()));
        }
        mY.erase(mY.begin(), mY.begin() + 4);
        {
            Proctor pX(&mX);
            ASSERTV(Y, *pX, Y == *pX);
        }

        ASSERTV(X.length(), 4 == X.length());
        ASSERTV('A' + 6 == u::getData(u::myFront(X)));
        ASSERTV('A' + 9 == u::getData(u::myBack(X)));
        v.clear();

        numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(va) {
            ++numThrows;

            mX.tryPopFront(10, &v);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        if (u::e_BSL == vecType) {
            ASSERTV(numThrows, !PLAT_EXC ||
                                   (k_ALLOC && (!match || !k_IS_MOVE_AWARE)) ==
                                                              (0 < numThrows));
        }
        ASSERT(0 == u::myLength(X));
        ASSERT(4 == v.size());
        if (k_WELL_BEHAVED || !k_IS_MOVE_AWARE) {
            ASSERT('A' + 6 == u::getData(v.front()));
            ASSERT('A' + 9 == u::getData(v.back()));
        }
        ASSERT(0 == X.length());
        v.clear();
        mX.tryPopFront(1, &v);    // doesn't block
        ASSERT(v.empty());
        ASSERT(0 == X.length());
        mY.clear();

        if (veryVerbose) cout << "\tPopBack:  " <<
                                     (match ? "" : "no ") << "match, " << endl;

        ASSERT(!X.length());

        u::setData(&e, 'Z');
        sts = mX.tryPopBack(&e);
        ASSERT(0 != sts);
        ASSERT('Z' == u::getData(e));
        mX.tryPopBack(100, &v);
        ASSERT(v.empty());
        mX.tryPopBack(100);    // doesn't block
        v.reserve(10);

        ASSERT(!X.length());

        for (int i = 0; i < 10; ++i) {
            u::setData(&e, 'A' + i);
            mX.pushFront(e);
        }
        u::setData(&e, 'Z');

        mX.tryPopBack(1);
        ASSERT('Z' == u::getData(e));
        {
            Proctor pX(&mX);
            mY = *pX;
        }
        numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(va) {
            ++numThrows;

            sts = mX.tryPopBack(&e);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERTV(numThrows, NameOf<ELEMENT>(), match, !PLAT_EXC ||
                 (k_ALLOC && (!match || !k_IS_MOVE_AWARE || !expAssignMove)) ==
                                                              (0 < numThrows));
        ASSERT(0 == sts);
        ASSERT('A' + 1 == u::getData(e));
        mY.pop_back();
        {
            Proctor pX(&mX);
            ASSERTV(Y, *pX, Y == *pX);
        }

        numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(va) {
            ++numThrows;

            mX.tryPopBack(4, &v);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        if (u::e_BSL == vecType) {
            ASSERTV(numThrows, NameOf<ELEMENT>(), match, !PLAT_EXC ||
                                   (k_ALLOC && (!match || !k_IS_MOVE_AWARE)) ==
                                                              (0 < numThrows));
        }
        ASSERT(4 == v.size());
        if (k_WELL_BEHAVED || !k_IS_MOVE_AWARE) {
            ASSERTV(v.front(), 'A' + 2 == u::getData(v.front()));
            ASSERTV(v.back(),  'A' + 5 == u::getData(v.back()));
        }
        mY.erase(mY.end() - 4, mY.end());
        {
            Proctor pX(&mX);
            ASSERTV(Y, *pX, Y == *pX);
        }

        ASSERTV(X.length(), 4 == X.length());
        ASSERTV(u::myBack(X),  'A' + 6 == u::getData(u::myBack(X)));
        ASSERTV(u::myFront(X), 'A' + 9 == u::getData(u::myFront(X)));
        v.clear();

        numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(va) {
            ++numThrows;

            mX.tryPopBack(10, &v);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        if (u::e_BSL == vecType) {
            ASSERTV(numThrows, name, match, !PLAT_EXC ||
                                   (k_ALLOC && (!match || !k_IS_MOVE_AWARE)) ==
                                                              (0 < numThrows));
        }
        ASSERT(0 == u::myLength(X));
        ASSERT(4 == v.size());
        if (k_WELL_BEHAVED || !k_IS_MOVE_AWARE) {
            ASSERTV(v.front(), 'A' + 6 == u::getData(v.front()));
            ASSERTV(v.back(),  'A' + 9 == u::getData(v.back()));
        }
        ASSERT(0 == X.length());
        v.clear();
        mX.tryPopBack(1, &v);    // doesn't block
        ASSERT(v.empty());
        ASSERT(0 == X.length());
        mY.clear();

        ASSERT(!X.length());
    }

    if (u::e_BSL == vecType) {
        ASSERT(0 == da.numAllocations());
    }
}

}  // close namespace TEST_CASE_9

//=============================================================================
//                                  TEST CASE 7
//-----------------------------------------------------------------------------

namespace TEST_CASE_7 {

enum WorkerType{ e_SRC_PUSHER,
                 e_DIRECT_PUSHER,
                 e_POPPER,
                 e_TRANSFERRER };

enum { k_DST_HIGH_WATER_MARK = 100,
       k_SRC_HIGH_WATER_MARK = 200 };

enum { k_NUM_TO_SRC_PUSH     = 2 * 1000,
       k_NUM_TO_DIRECT_PUSH  = k_NUM_TO_SRC_PUSH,

       k_NUM_SRC_PUSHERS     = 5,
       k_NUM_DIRECT_PUSHERS  = 5,
       k_NUM_POPPERS         = 5,

       k_TOTAL_TO_POP        = k_NUM_SRC_PUSHERS    * k_NUM_TO_SRC_PUSH +
                               k_NUM_DIRECT_PUSHERS * k_NUM_TO_DIRECT_PUSH,
       k_NUM_TO_POP          = k_TOTAL_TO_POP / k_NUM_POPPERS,

       k_NUM_TO_TRANSFER     = k_NUM_SRC_PUSHERS    * k_NUM_TO_SRC_PUSH,

       k_NUM_THREADS         = k_NUM_SRC_PUSHERS + k_NUM_DIRECT_PUSHERS +
                                                           k_NUM_POPPERS + 1 };

BSLMF_ASSERT(0 == k_TOTAL_TO_POP % k_NUM_POPPERS);

// The following 3 variables don't need to be atomic as they are only assigned
// to by the one transferrer thread, and then read by the main thread after
// all subthreads are joined.

bsl::size_t maxDstSize = 0, maxSrcSize = 0;

class ProctorTestFunctor {
    // This 'class' has is created in one of three types, as specified by the
    // 'workerType' arg to the c'tor.  When 'operator()' is called, it calls
    // one worker function according to its type.

    // DATA
    Obj                   *d_dst_p;
    Obj                   *d_src_p;
    const unsigned char    d_workerType;
    const bool             d_backWard;
    const int              d_workerIdx;

  public:
    // CREATORS
    ProctorTestFunctor(Obj        *dst_p,
                       Obj        *src_p,
                       WorkerType  workerType,
                       int         workerIdx,
                       bool        backWard);
        // Create a 'ProctorTestFunctor' object attached to the specified
        // '*dst_p' and the specified '*src_p' containers, of specified worker
        // type 'workerType', with specified index 'workerIdx' (that is
        // ignored unless 'e_PUSHER == workerType', If the specified 'backWard'
        // is 'false', items will be pushed to the back of deques and popped
        // from the front of them, if 'backWard' is 'true' the direction is
        // reverrsed.

    // ACCESSORS
    void operator()() const;
        // switch off the 'd_workType' field to dispatch to a specific worker
        // function.

    void directPusher() const;
        // Direct pusher worker function.  Push many elements in decreasing
        // order, ending with 'd_workerIdx + t', to the back or front of
        // '*d_dst_p', depending upon the polarity of 'd_backWard'.

    void popper() const;
        // Popper worker function.  Pop many elements from the front or back of
        // '*d_dst_p', depending on the polarity of 'd_backward'.

    void srcPusher() const;
        // Pusher worker function.  Push many elements in decreasing order,
        // ending with 'd_workerIdx + t', to the back or front of '*d_src_p',
        // depending upon the polarity of 'd_backWard'.

    void transferrer() const;
        // Transferrer worker function.  Transfer all elements from '*d_src_p'
        // to '*d_dst_p', taking care to remove elements from the opposite end
        // of '*d_src_p' from the end to which the pusher function pushed
        // elements, and add them to the opposite end of '*d_dst_p' from the
        // end from which the popper function pops elements.  Do block
        // transferrs by accessing both deques via proctors.
};

// CREATOR
ProctorTestFunctor::ProctorTestFunctor(Obj        *dst_p,
                                       Obj        *src_p,
                                       WorkerType  workerType,
                                       int         workerIdx,
                                       bool        backWard)
: d_dst_p(dst_p)
, d_src_p(src_p)
, d_workerType(static_cast<unsigned char>(workerType))
, d_backWard(backWard)
, d_workerIdx(workerIdx * 1000 * 1000)
{
    // Check for wraparound

    ASSERTV(d_workerIdx, workerIdx,
               (e_SRC_PUSHER != workerType && e_DIRECT_PUSHER != workerType) ||
                                                      d_workerIdx > workerIdx);
}

// ACCESSORS
void ProctorTestFunctor::operator()() const
    // Dispatch function -- switch on 'd_workerType' to call the appropriate
    // worker function.
{
    switch (d_workerType) {
      case e_SRC_PUSHER: {
        srcPusher();
      } break;
      case e_DIRECT_PUSHER: {
        directPusher();
      } break;
      case e_POPPER: {
        popper();
      } break;
      case e_TRANSFERRER: {
        transferrer();
      }
    }
}

void ProctorTestFunctor::directPusher() const
{
    for (int ti = k_NUM_TO_DIRECT_PUSH; ti > 0; --ti) {
        if (d_backWard) {
            d_dst_p->pushFront(d_workerIdx + ti);
        }
        else {
            d_dst_p->pushBack( d_workerIdx + ti);
        }
    }
}

void ProctorTestFunctor::popper() const
{
    Element val, lastVal = 0;

    for (int ti = k_NUM_TO_POP; ti > 0; --ti) {
        val = d_backWard ? d_dst_p->popBack() : d_dst_p->popFront();

        // check for duplicates

        ASSERT(val != lastVal);
        lastVal = val;
    }
}

void ProctorTestFunctor::srcPusher() const
{
    for (int ti = k_NUM_TO_SRC_PUSH; ti > 0; --ti) {
        if (d_backWard) {
            d_src_p->pushFront(d_workerIdx + ti);
        }
        else {
            d_src_p->pushBack( d_workerIdx + ti);
        }
    }
}

void ProctorTestFunctor::transferrer() const
{
    // Note that in these transfers, we deliberately overflow the dst deque
    // above the high water mark sometimes.

    // We lock both the src and dst deques at the same time by creating
    // proctors for them.  We avoid dining philosophers by always acquiring
    // them in the same order, and the other threads only lock one at a time,
    // and never in a greedy way.

    typedef bsl::deque<double>::const_iterator const_iterator;
    typedef bsl::deque<double>::iterator       iterator;

    Element           lastTransferred = 0;
    const bsl::size_t srcHwm  = d_src_p->highWaterMark();
    const bsl::size_t dstFull = d_dst_p->highWaterMark() + 20;

    bsl::size_t thisTransfer;

    Obj::Proctor dstProctor;

    for (bsl::size_t toTransfer = k_NUM_TO_TRANSFER;
                                  toTransfer > 0; toTransfer -= thisTransfer) {
        Obj::ConstProctor srcProctor(d_src_p);
        bsl::size_t       srcSize = srcProctor->size();

        ASSERT(srcSize <= k_SRC_HIGH_WATER_MARK);
        ASSERT(srcSize <= srcHwm);
        maxSrcSize = bsl::max(srcSize, maxSrcSize);

        thisTransfer = bsl::min<bsl::size_t>(srcSize, 100);
        if (thisTransfer < 10 && toTransfer > 10) {
            thisTransfer = 0;
        }
        else if (0 != thisTransfer) {
            dstProctor.load(d_dst_p);
            bsl::size_t  dstSize = dstProctor->size();
            maxDstSize = bsl::max(dstSize, maxDstSize);

            if (dstSize >= dstFull) {
                ASSERT(dstSize == dstFull);

                thisTransfer = 0;
            }
            else {
                thisTransfer = bsl::min(dstFull - dstSize,
                                        thisTransfer);

                // We can't delete the oldest 'thisTransferred' elements from
                // 'srcProctor' because 'srcProctor' is a 'ConstProctor'.

                if (d_backWard) {
                    const_iterator end = srcProctor->end();
                    const_iterator beg = end - thisTransfer;

                    dstProctor->insert(dstProctor->begin(), beg, end);

                    lastTransferred = dstProctor->front();
                }
                else {
                    const_iterator beg = srcProctor->begin();
                    const_iterator end = beg + thisTransfer;

                    // Note 'deque' has no 'append' modifier.

                    dstProctor->insert(dstProctor->end(), beg, end);

                    lastTransferred = dstProctor->back();
                }
            }
            dstProctor.release();
        }
        srcProctor.release();

        if (0 == thisTransfer) {
            // This means '*d_dst_p' is full or '*d_src_p' is empty, or we have
            // an insignificant number of elements to transfer.  Give the
            // pushers and poppers some time to run.

            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(10 * 1000);
        }
        else {
            Obj::Proctor trimProctor(d_src_p);

            // Now we can delete the oldest 'thisTransfer' elements from
            // '*d_src_p' because they were just transferred out.  Note that
            // some new elements might have been pushed into '*d_src_p' since
            // the 'ConstProctor' was destroyed, but they will be at the
            // opposite end and not affected by this 'erase'.

            ASSERT(trimProctor->size() >= thisTransfer);

            iterator begin = d_backWard ? trimProctor->end() - thisTransfer
                                        : trimProctor->begin();
            iterator end   = d_backWard ? trimProctor->end()
                                        : begin + thisTransfer;

            trimProctor->erase(begin, end);
        }
    }

    // The pushers arounting down from a large number down to 1, ending with
    // one (plus the workerIdx, that is a multiple of a million) so the last
    // value pushed by any worker, % a million, will be one, and the last value
    // transferred by the transferrer, must be the last value pushed by some
    // pusher.

    ASSERT(1 == (size_t) lastTransferred % (1000 * 1000));
}

}  // close namespace TEST_CASE_7

//=============================================================================
//                                  TEST CASE 6
//-----------------------------------------------------------------------------

namespace TEST_CASE_6 {

bsls::AtomicInt      waitingFlag(0);

class TimedHWMRecordBack {
    // DATA
    bdlcc::Deque<AElement> *d_deque_p;
    bsls::TimeInterval      d_timeout;
    const AElement&         d_toBeInserted;

  public:
    // CREATORS
    TimedHWMRecordBack(bdlcc::Deque<AElement> *deque,
                       bsls::TimeInterval      timeout,
                       const AElement&         value)
    : d_deque_p(deque)
    , d_timeout(timeout)
    , d_toBeInserted(value)
        // Create a test object accessing the specified '*deque', that will
        // push the specified 'value', with the specified 'timeout'.
    {
    }

    // MANIPULATORS
    void operator()()
        // Do the 'timedPushBack' with the values specified at construction.
    {
        waitingFlag = 1;
        ASSERT(0 == d_deque_p->timedPushBack(
                                        d_toBeInserted, u::now() + d_timeout));
    }
};

class TimedHWMRecordFront {
    // DATA
    bdlcc::Deque<AElement> *d_deque_p;
    bsls::TimeInterval      d_timeout;
    const AElement&         d_toBeInserted;

  public:
    // CREATORS
    TimedHWMRecordFront(bdlcc::Deque<AElement> *deque,
                        bsls::TimeInterval      timeout,
                        const AElement&         value)
    : d_deque_p(deque)
    , d_timeout(timeout)
    , d_toBeInserted(value)
        // Create a test object accessing the specified '*deque', that will
        // push the specified 'value', with the specified 'timeout'.
    {
    }

    // MANIPULATORS
    void operator()()
        // Do the 'timedPushFront' with the values specified at construction.
    {
        waitingFlag = 1;
        ASSERT(0 == d_deque_p->timedPushFront(d_toBeInserted,
                                              u::now() + d_timeout));
    }
};

extern "C" void *doTimedHWMRecordBack(void *arg)
    // Cast the specified 'arg' to a 'TimedHWMPushBack' and call it.
{
    (*static_cast<TimedHWMRecordBack *>(arg))();

    return 0;
}

extern "C" void *doTimedHWMRecordFront(void *arg)
    // Cast the specified 'arg' to a 'TimedHWMPushFront' and call it.
{
    (*static_cast<TimedHWMRecordFront *>(arg))();

    return 0;
}

}  // close namespace TEST_CASE_6

//=============================================================================
//                                  TEST CASE 5
//-----------------------------------------------------------------------------

namespace TEST_CASE_5 {

bsls::AtomicInt waitingFlag;

class StraightHWMFunctorBack {

    // DATA
    bdlcc::Deque<Element> *d_deque_p;
    Element                d_toBeInserted;

  public:
    // CREATORS
    StraightHWMFunctorBack(bdlcc::Deque<Element> *deque,
                           const Element&         value)
    : d_deque_p(deque)
    , d_toBeInserted(value)
        // Create a test object accessing the specified '*deque' with the
        // specified 'value'.
    {
        waitingFlag = 0;
    }

    // MANIPULATORS
    void operator()()
        // Do the 'pushBack'.
    {
        waitingFlag = 1;
        d_deque_p->pushBack(d_toBeInserted);
        waitingFlag = 0;
    }
};

class StraightHWMFunctorFront {

    // DATA
    bdlcc::Deque<Element> *d_deque_p;
    Element                d_toBeInserted;

  public:
    // CREATORS
    StraightHWMFunctorFront(bdlcc::Deque<Element> *deque,
                            const Element&         value)
    : d_deque_p(deque)
    , d_toBeInserted(value)
        // Create a test object accessing the specified '*deque' with the
        // specified 'value'.
    {
        waitingFlag = 0;
    }

    // MANIPULATORS
    void operator()()
        // Do the 'pushFront'.
    {
        waitingFlag = 1;
        d_deque_p->pushFront(d_toBeInserted);
        waitingFlag = 0;
    }
};

}  // close namespace TEST_CASE_5

//=============================================================================
//                                  TEST CASE 4
//-----------------------------------------------------------------------------

namespace TEST_CASE_4 {

class TimedPopRecordBack {
    // DATA
    bdlcc::Deque<AElement> *d_deque_p;
    bslmt::Barrier         *d_barrier_p;
    bsls::TimeInterval      d_timeout;
    bsls::AtomicInt         d_timeoutFlag;
    bsls::AtomicInt         d_waitingFlag;
    const AElement&         d_expected;

  public:
    // CREATORS
    TimedPopRecordBack(bdlcc::Deque<AElement> *deque,
                       bslmt::Barrier         *barrier,
                       bsls::TimeInterval      timeout,
                       const AElement&         val)
    : d_deque_p(deque)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_expected(val)
        // Create a test object that will access the specified '*deque' and
        // with the specified 'val' and the specified 'timeout' and block on
        // the specified '*barrier'.
    {
    }

    // MANIPULATORS
    void operator()()
        // Do two pushes, the first of which will timeout, the second of which
        // will succeed without timing out.
    {
        AElement result('Z', d_deque_p->allocator());

        d_barrier_p->wait();

        // no one's pushing this time, pop will tame out

        bsls::TimeInterval start = u::now();
        d_timeoutFlag = d_deque_p->timedPopBack(&result, start + d_timeout);
        bsls::TimeInterval end   = u::now();

        ASSERT(end >= start + d_timeout);
        ASSERT('Z' == result.data());    // not assigned to

        d_barrier_p->wait();

        // other thread will push

        d_barrier_p->wait();

        start = u::now();
        BEGIN_EXCEP_TEST_OBJ(AElement, *d_deque_p) {
            d_timeoutFlag = d_deque_p->timedPopBack(&result,
                                                    start + d_timeout);
        } END_EXCEP_TEST_OBJ
        end   = u::now();

        ASSERT(end <  start + d_timeout);

        // pop should've succeeded

        ASSERT(0 == d_timeoutFlag);
        ASSERT(result == d_expected);
    }

    // ACCESSORS
    int  timeOutFlag() const
        // Return the value of the time out flag.
    {
        return d_timeoutFlag;
    }

    int  waitingFlag() const
        // Return the value of the waiting flag.
    {
        return d_waitingFlag;
    }
};

class TimedPopRecordFront {

    // DATA
    bdlcc::Deque<AElement> *d_deque_p;
    bslmt::Barrier         *d_barrier_p;
    bsls::TimeInterval      d_timeout;
    bsls::AtomicInt         d_timeoutFlag;
    bsls::AtomicInt         d_waitingFlag;
    const AElement&         d_expected;

  public:
    // CREATORS
    TimedPopRecordFront(bdlcc::Deque<AElement> *deque,
                        bslmt::Barrier         *barrier,
                        bsls::TimeInterval      timeout,
                        const AElement&         value)
    : d_deque_p(deque)
    , d_barrier_p(barrier)
    , d_timeout(timeout)
    , d_timeoutFlag(0)
    , d_waitingFlag(1)
    , d_expected(value)
        // Create a test object that will access the specified '*deque' and
        // with the specified 'val' and the specified 'timeout' and block on
        // the specified '*barrier', and expect to pop the specified 'value'.
    {
    }

    // MANIPULATORS
    void operator()()
        // Do two pushes, the first of which will timeout, the second of which
        // will succeed without timing out.
    {
        AElement result('Z', d_deque_p->allocator());

        d_barrier_p->wait();

        // no one's pushing this time, pop will tame out

        bsls::TimeInterval start = u::now();
        d_timeoutFlag = d_deque_p->timedPopFront(&result, start + d_timeout);
        bsls::TimeInterval end   = u::now();

        ASSERT(end >= start + d_timeout);
        ASSERT('Z' == result.data());    // not assigned to

        d_barrier_p->wait();

        // other thread will push

        d_barrier_p->wait();

        start = u::now();
        BEGIN_EXCEP_TEST_OBJ(AElement, *d_deque_p) {
            d_timeoutFlag = d_deque_p->timedPopFront(&result,
                                                     start + d_timeout);
        } END_EXCEP_TEST_OBJ
        end   = u::now();

        ASSERT(end <  start + d_timeout);

        // pop should've succeeded

        ASSERT(0 == d_timeoutFlag);
        ASSERT(result == d_expected);
    }

    // ACCESSORS
    int timeOutFlag() const
        // Return the value of the timeout flag.
    {
        return d_timeoutFlag;
    }

    int waitingFlag() const
        // Return the value of the waiting flag.
    {
        return d_waitingFlag;
    }
};

// We could just feed 'TimedPopRecord{Front,Back}' objects into
// 'bslmt::ThreadUtil::create', but that would use the default allocator.

extern "C"
void *testClass4BackCaller(void *arg)
    // Cast the specified 'arg' to a test object and call it.
{
    TimedPopRecordBack *pt = (TimedPopRecordBack *) arg;
    (*pt)();
    return 0;
}

extern "C"
void *testClass4FrontCaller(void *arg)
    // Cast the specified 'arg' to a test object and call it.
{
    TimedPopRecordFront *pt = (TimedPopRecordFront *) arg;
    (*pt)();
    return 0;
}

}  // close namespace TEST_CASE_4

namespace TEST_CASE_3 {

enum PopMode {
    BY_VALUE,
    THROUGH_PTR
};

class PushPopRecordBack {

    // DATA
    bdlcc::Deque<Element> *d_deque_p;
    bsls::AtomicInt        d_waitingFlag;
    Element                d_expected;
    PopMode                d_popMode;

  public:
    // CREATORS
    PushPopRecordBack(bdlcc::Deque<Element> *deque,
                      const Element&         value,
                      PopMode                popMode)
    : d_deque_p(deque)
    , d_waitingFlag(0)
    , d_expected(value)
    , d_popMode(popMode)
        // Create a test object that will pop from the back of the specified
        // '*deque' with the pop style determined by the specified 'popMode'
        // and expect the specified 'value'.
    {
    }

    // MANIPULATORS
    void callback()
        // manipulate the flags and pop from the deque back
    {
        d_waitingFlag = 1;
        Element e;
        if (BY_VALUE == d_popMode) {
            e = d_deque_p->popBack();
        }
        else {
            ASSERT(THROUGH_PTR == d_popMode);
            d_deque_p->popBack(&e);
        }
        ASSERT(d_expected == e);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag()
        // reveal waiting flag
    {
        return d_waitingFlag;
    }
};

class PushPopRecordFront {
    // DATA
    bdlcc::Deque<Element> *d_deque_p;
    bsls::AtomicInt        d_waitingFlag;
    Element                d_expected;
    PopMode                d_popMode;

  public:
    // CREATORS
    PushPopRecordFront(bdlcc::Deque<Element> *deque,
                       const Element&         value,
                       PopMode                popMode)
    : d_deque_p(deque)
    , d_waitingFlag(0)
    , d_expected(value)
    , d_popMode(popMode)
        // Create a test object that will pop from the front of the specified
        // '*deque' with the pop style determined by the specified 'popMode'
        // and expect the specified 'value'.
    {
    }

    // MANIPULATORS
    void callback()
        // manipulate the flags and pop from the deque front
    {
        d_waitingFlag = 1;
        Element e;
        if (BY_VALUE == d_popMode) {
            e = d_deque_p->popFront();
        }
        else {
            ASSERT(THROUGH_PTR == d_popMode);
            d_deque_p->popFront(&e);
        }
        ASSERT(d_expected == e);
        d_waitingFlag = 0;
    }

    // ACCESSORS
    int waitingFlag()
        // reveal waiting flag
    {
        return d_waitingFlag;
    }
};

extern "C"
void *pushPopFunctionBack(void *arg)
    // The specified 'arg' refers to a test object, call its specified
    // 'callback'.
{
    PushPopRecordBack *x = (PushPopRecordBack*)arg;
    x->callback();
    return 0;
}

extern "C"
void *pushPopFunctionFront(void *arg)
    // The specified 'arg' refers to a test object, call its specified
    // 'callback'.
{
    PushPopRecordFront *x = (PushPopRecordFront*)arg;
    x->callback();
    return 0;
}

}  // close namespace TEST_CASE_3

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    bslma::TestAllocator         da(veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

    bslma::TestAllocator ta(veryVeryVeryVerbose);

    bslmt::Configuration::setDefaultThreadStackSize(
                    bslmt::Configuration::recommendedDefaultThreadStackSize());

    switch (test) { case 0:  // Zero is always the leading case.
      case 28: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //
        //: 1 The second usage example compiles and works as expected.
        //
        // Plan:
        //: 1 Create a multi-threaded deque of 'Event's.
        //: 2 Create a set of an arbitrary number of 'myWorker' threads, where
        //:   each 'myWorker' thread simulates a single task.
        //: 3 Each 'myWorker' thread generates and endeques multiple
        //:   'Event's.  Upon completion, each 'myWorker' thread endeques a
        //:   TASK_COMPLETE event.
        //: 4 Count the TASK_COMPLETE events until all threads are complete;
        //:   then "join" each thread.
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        using namespace USAGE_EXAMPLE_2;

        if (verbose) cout << "USAGE EXAMPLE 2\n"
                             "===============\n";

        {
// Next, in 'main', define the number of threads:
//..
    const int k_NUM_THREADS = 10;
//..
// Then, declare out 'bdlcc::Deque' object, the set of handles of the
// subthreads, and our barrier object:
//..
    bdlcc::Deque<Event>       myDeque;
    bslmt::ThreadUtil::Handle handles[k_NUM_THREADS];
    bslmt::Barrier            barrier(k_NUM_THREADS + 1);
//..
// Next, spawn the worker threads:
//..
    for (int ti = 0; ti < k_NUM_THREADS; ++ti) {
        WorkerFunctor functor = { ti, &myDeque, &barrier };

        bslmt::ThreadUtil::create(&handles[ti], functor);
    }
//..
// Then, wait on the barrier, that will set all the subthreads running:
//..
    barrier.wait();
//..
// Now, loop to pop the events off the deque, and keep track of how many
// 'e_COMPLETE' events have been popped.  When this equals the number of
// subthreads, we are done.
//..
    int numCompleted = 0, numEvents = 0;
    while (numCompleted < k_NUM_THREADS) {
        Event ev = myDeque.popFront();
        ++numEvents;
        if (verbose) {
            cout << "[" << ev.d_workerId << "] "
                 << ev.d_eventNumber << ". "
                 << ev.d_eventText_p << endl;
        }
        if (Event::e_TASK_COMPLETE == ev.d_type) {
            ++numCompleted;
            int rc = bslmt::ThreadUtil::join(handles[ev.d_workerId]);
            ASSERT(!rc);
        }
    }
//..
// Finally, perform some sanity checks:
//..
    ASSERT(k_NUM_THREADS * k_NUM_TO_PUSH == numEvents);
    ASSERT(0 == myDeque.length());
//..
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //
        // Concerns:
        //: 1 That the first usage example compiles and runs correctly.
        //
        // Plan:
        //: 1 Build the usage and run it.
        //
        // Testing:
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        using namespace USAGE_EXAMPLE_1;

        if (verbose) cout << "USAGE EXAMPLE 1\n"
                             "===============\n";

// Next, in 'main', define the number of consumer and producer threads (these
// numbers must be equal).
//..
    enum { k_NUM_CONSUMER_THREADS = 10,
           k_NUM_PRODUCER_THREADS = k_NUM_CONSUMER_THREADS };
//..
// Then, create our container:
//..
    bdlcc::Deque<WorkRequest> deque;
//..
// Next, create the array of thread handles for the threads we will spawn:
//..
    bslmt::ThreadUtil::Handle handles[k_NUM_CONSUMER_THREADS +
                                      k_NUM_PRODUCER_THREADS];
//..
// Now, spawn all the consumers and producers:
//..
    int ti = 0, rc;
    while (ti < k_NUM_CONSUMER_THREADS) {
        rc = bslmt::ThreadUtil::create(&handles[ti++],
                                       ConsumerFunctor(&deque));
        ASSERT(0 == rc);
    }
    while (ti < k_NUM_CONSUMER_THREADS + k_NUM_PRODUCER_THREADS) {
        rc = bslmt::ThreadUtil::create(&handles[ti++],
                                       ProducerFunctor(&deque));
        ASSERT(0 == rc);
    }
//..
// Finally, join all the threads after they finish and confirm the container is
// empty afterward:
//..
    while (ti > 0) {
        rc = bslmt::ThreadUtil::join(handles[--ti]);
        ASSERT(0 == rc);
    }
    ASSERT(0 == deque.length());
//..
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING TIMED POP & TIMED PUSH FUNCTIONS -- MOVE SEMANTICS
        //
        // Concerns:
        //: 1 Test the concerns of TC 4 with emphasis on move semantics.
        //
        // Plan:
        //: 1 Repeat TC 4 using 'MElement' instead of 'AElement', track the
        //:   memory allocation behavior very closely, and test 'timedPush*'
        //:   as well as 'timedPop*'.
        //
        // Testing:
        //   int timedPopBack(TYPE *, const TimeInterval&); - move semantics
        //   int timedPopFront(TYPE *, const TimeInterval&); - move semantics
        //   int timedPushBack(TYPE&&, const TimeInterval&);
        //   int timedPushFront(TYPE&&, const TimeInterval&);
        // --------------------------------------------------------------------

        using namespace TEST_CASE_26;

        if (verbose) cout <<
                "TESTING TIMED POP & TIMED PUSH FUNCTIONS -- MOVE SEMANTICS\n"
                "==========================================================\n";

        testTimedPushPopMove<Element>();
        testTimedPushPopMove<AElement>();
        testTimedPushPopMove<MElement>();
        testTimedPushPopMove<MCElement>();
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // SINGLE-THREADED TESTING SINGLE MOVING PUSHES, POPS
        //
        // Concerns:
        //: 1 Examine all the issues examined in TC 2, except that this time
        //:   we are testing move semantics.
        //
        // Plan:
        //: 1 Repeat everything tested in TC 2 with single pushes and pops,
        //:   but moving objects into the pushes, and observe whether objects
        //:   we moved or not.
        //
        // Testing:
        //   void pushFront(TYPE&&); - st
        //   void pushBack(TYPE&&); - st
        //   void tryPushFront(TYPE&&); - st
        //   void tryPushBack(TYPE&&); - st
        //   TYPE popFront(); - move semantics
        //   void popFront(TYPE *); - move semantics
        //   TYPE popBack(); - move semantics
        //   void popBack(TYPE *); - move semantics
        // --------------------------------------------------------------------

        if (verbose) cout <<
                        "SINGLE-THREADED TESTING SINGLE MOVING PUSHES, POPS\n"
                        "==================================================\n";

        namespace TC = TEST_CASE_25;

        TC::testSingleMovingPushesPops<Element>();
        TC::testSingleMovingPushesPops<AElement>();
        TC::testSingleMovingPushesPops<MElement>();
        TC::testSingleMovingPushesPops<MCElement>();
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING PROCTOR LIFETIME
        //
        // Concerns:
        //: 1 The C++ language definition specifies that a temporary created
        //:   within a statement will not outlive the statement unless bound to
        //:   a reference.  On some old compilers, notably Solaris, temporaries
        //:   could survive until the end of the block.  Ensure that on all
        //:   platforms we port to, proctors are being deleted at the end of
        //:   the statement.
        //
        // Plan:
        //: 1 Create several proctors, as temporaries, within the same block.
        //:   The fact that the mutex was able to be acquired multiple times
        //:   will establish that old proctors were being destroyed (C-1).
        //
        // Testing:
        //    PROCTOR LIFETIME
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING PROCTOR LIFETIME\n"
                             "========================\n";

        typedef Obj::MonoDeque::const_iterator CIt;

        Obj mX(&ta);
        mX.pushBack(3);
        mX.pushBack(4);
        ASSERT(2 == Obj::Proctor(&mX)->size());
        ASSERT(2 == Obj::ConstProctor(&mX)->size());

        mX.pushFront(2);
        mX.pushFront(1);
        ASSERT(4 == Obj::Proctor(&mX)->size());
        ASSERT(4 == Obj::ConstProctor(&mX)->size());

        Obj::Proctor(&mX)->push_front(0);

        {
            Obj::ConstProctor cp(&mX);
            double x = 0.0;
            for (CIt cit = cp->begin(); cp->end() != cit; ++cit, x += 1) {
                ASSERT(*cit == x);
            }
            ASSERT(5 == x);

            for (int y = 0; y < x; ++y) {
                ASSERT((*cp)[y] == y);
            }
        }

        mX.pushBack(5);

        ASSERT(6 == Obj::Proctor(&mX)->size());
        ASSERT(6 == Obj::ConstProctor(&mX)->size());

        int ii = 0, jj = 0;
        ASSERT(ii++ == Obj::ConstProctor(&mX)->front());
        ASSERT(ii++ == Obj::ConstProctor(&mX)[++jj]);
        ASSERT(ii++ == Obj::ConstProctor(&mX)[++jj]);
        ASSERT(ii++ == Obj::ConstProctor(&mX)[++jj]);
        ASSERT(ii++ == Obj::ConstProctor(&mX)[++jj]);
        ASSERT(ii++ == Obj::ConstProctor(&mX)->back());

        ASSERT(--ii == Obj::Proctor(&mX)->back());
        ASSERT(--ii == Obj::Proctor(&mX)[jj--]);
        ASSERT(--ii == Obj::Proctor(&mX)[jj--]);
        ASSERT(--ii == Obj::Proctor(&mX)[jj--]);
        ASSERT(--ii == Obj::Proctor(&mX)[jj--]);
        ASSERT(--ii == Obj::Proctor(&mX)->front());
        ASSERTV(ii, 0 == ii);
        ASSERTV(jj, 0 == jj);
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING ALL C'TORS
        //
        // Concerns:
        //: 1 That all c'tors work as specced.
        //
        // Plan:
        //: 1 Iterate through a loop, create 'bdlcc::Deque' objects with
        //:   varying c'tors and arguments.
        //:
        //: 2 After creation, set 'bool's to indicate which arguments were
        //:   passed to the c'tor.
        //:
        //: 3 Determine, through accessors and minor manipulation, that the
        //:   object was correctly created (C-1).
        //:
        //: 4 Run the c'tors with exceptions injected, to insure that no
        //:   memory is leaked.
        //
        // Testing:
        //   Deque();
        //   Deque(alloc);
        //   Deque(clock);
        //   Deque(clock, alloc);
        //   Deque(hwm);
        //   Deque(hwm, alloc);
        //   Deque(hwm, clock);
        //   Deque(hwm, clock, alloc);
        //   Deque(ITER, ITER);
        //   Deque(ITER, ITER, alloc);
        //   Deque(ITER, ITER, clock);
        //   Deque(ITER, ITER, clock, alloc);
        //   Deque(ITER, ITER, hwm);
        //   Deque(ITER, ITER, hwm, alloc);
        //   Deque(ITER, ITER, hwm, clock);
        //   Deque(ITER, ITER, hwm, clock, alloc);
        //   bslma::Allocator *allocator() const;
        //   bsls::SystemClockType::Enum clockType() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING ALL C'TORS\n"
                             "==================\n";

        bslma::TestAllocator fa;

        const int hwm = 4;
        const Element range[] = { 1.0, 2.0 };
        const Element * const b = range, * const e = range + 2;

        bool done = false;
        for (char ctor = 'a'; ctor <= 'p' ; ++ctor) {
            for (int sctInt = 0; sctInt < 2; ++sctInt) {
                const bsls::SystemClockType::Enum sct =
                                    sctInt ? bsls::SystemClockType::e_MONOTONIC
                                           : bsls::SystemClockType::e_REALTIME;

                Obj *pMX = 0;

                const Int64 daStart = da.numBlocksTotal();
                const Int64 taStart = ta.numBlocksTotal();

                const bool isRange =  ctor > 'h';
                const bool isHwm   = (ctor - 'a') % 8 >= 4;
                const bool isSct   = (ctor - 'a') % 4 >= 2;
                const bool isTa    = (ctor - 'a') % 2;

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN((isTa ? ta : da)) {
                    ++numPasses;

                    switch (ctor) {
                      case 'a': {
                        pMX = new (fa) Obj();
                      } break;
                      case 'b': {
                        pMX = new (fa) Obj(&ta);
                      } break;
                      case 'c': {
                        pMX = new (fa) Obj(sct);
                      } break;
                      case 'd': {
                        pMX = new (fa) Obj(sct, &ta);
                      } break;
                      case 'e': {
                        pMX = new (fa) Obj(hwm);
                      } break;
                      case 'f': {
                        pMX = new (fa) Obj(hwm, &ta);
                      } break;
                      case 'g': {
                        pMX = new (fa) Obj(hwm, sct);
                      } break;
                      case 'h': {
                        pMX = new (fa) Obj(hwm, sct, &ta);
                      } break;
                      case 'i': {
                        pMX = new (fa) Obj(b, e);
                      } break;
                      case 'j': {
                        pMX = new (fa) Obj(b, e, &ta);
                      } break;
                      case 'k': {
                        pMX = new (fa) Obj(b, e, sct);
                      } break;
                      case 'l': {
                        pMX = new (fa) Obj(b, e, sct, &ta);
                      } break;
                      case 'm': {
                        pMX = new (fa) Obj(b, e, hwm);
                      } break;
                      case 'n': {
                        pMX = new (fa) Obj(b, e, hwm, &ta);
                      } break;
                      case 'o': {
                        pMX = new (fa) Obj(b, e, hwm, sct);
                      } break;
                      case 'p': {
                        pMX = new (fa) Obj(b, e, hwm, sct, &ta);
                        done = true;
                      } break;
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(ctor, pMX && "unrecognized c'tor");

                Obj& mX = *pMX;    const Obj& X = mX;

                // At the time of this writing, even an empty 'bsl::deque' does
                // 2 allocations in the c'tor.

                ASSERTV(PLAT_EXC == (1 < numPasses));
                ASSERTV(ctor, X.length() == (isRange ? 2 : 0));
                ASSERTV(ctor, X.highWaterMark() == (isHwm ? hwm : maxSizeT));
                ASSERTV(ctor, X.clockType() ==
                            (isSct ? sct : bsls::SystemClockType::e_REALTIME));
                ASSERTV(ctor, X.allocator() == (isTa ? &ta : &da));

                if (!isRange) {
                    mX.pushBack(1.0);
                    mX.pushBack(2.0);
                }

                const bool daUsed = da.numBlocksTotal() > daStart;
                const bool taUsed = ta.numBlocksTotal() > taStart;

                ASSERTV(ctor, isTa ? !daUsed : !taUsed);
                ASSERTV(ctor, isTa ?  taUsed :  daUsed);

                mX.pushBack(3.0);
                mX.pushBack(4.0);

                ASSERTV(ctor, 4 == X.length());

                for (int ii = 0; ii < 4; ++ii) {
                    ASSERTV(ctor, Obj::ConstProctor(&X)->at(ii) == ii + 1);
                }

                ASSERTV(ctor, isHwm == (0 != mX.tryPushBack(5.0)));

                fa.deleteObjectRaw(pMX);
            }
        }
        ASSERT(done);
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //
        // Concern:
        //: 1 That the class has the 'UsesBslmaAllocator' type trait.
        //
        // Plan:
        //: 2 Evaluate the type traits.
        //
        // Testing
        //   bslma::UsesBslmaAllocator
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING TYPE TRAITS\n"
                             "===================\n";

        BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
        BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::VALUE);

        BSLMF_ASSERT(
                  !bslalg::TypeTraitHasStlIterators::Metafunction<Obj>::value);
        BSLMF_ASSERT(
                  !bslalg::TypeTraitBitwiseCopyable::Metafunction<Obj>::value);
        BSLMF_ASSERT(
                  !bslalg::TypeTraitBitwiseMoveable::Metafunction<Obj>::value);
        BSLMF_ASSERT(!bslmf::IsBitwiseMoveable<Obj>::value);
        BSLMF_ASSERT(!bsl::is_trivially_copyable<Obj>::value);
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TEST SIMPLE RANGE C'TOR
        //
        // Concerns:
        //: 1 That the range constructor initializes the container correctly.
        //
        // Plan:
        //: 1 Construct a container with that c'tor and verify its contents.
        //
        // Testing:
        //   Deque(INPUT_ITER, INPUT_ITER, Alloc *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST SIMPLE RANGE C'TOR\n"
                             "=======================\n";

        const Element elements[] = { -5, 3.2, 9.1, -4.1e21, 7.3, 9.7, -10.8 };
        enum { NUM_ELEMENTS = sizeof elements / sizeof *elements };

        Obj mX(elements + 0, elements + NUM_ELEMENTS, &ta); const Obj& X = mX;

        ASSERT(NUM_ELEMENTS == X.length());

        for (int ii = 0; ii < NUM_ELEMENTS; ++ii) {
            ASSERT(mX.popFront() == elements[ii]);
        }

        ASSERT(0 == X.length());
        ASSERT(0 == da.numAllocations());
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // MULTITHREADED RANGE TRY PUSH TEST
        //
        // Concerns:
        //: 1 The items pushed by a single 'tryPush*' appear contiguously in
        //:   the container.
        //:
        //: 2 The integral value returned by the 'tryPush*' correctly
        //:   represents the number of items pushed.
        //:
        //: 3 The items pushed are always the first items in the range
        //:   sequence.
        //:
        //: 4 That range try pushes never violate the high water mark.
        //:
        //: 5 Test over a range of lengths of ranges pushed, that is, don't
        //:   always just wind up pushing 1 or 2 items per call.
        //
        // Plan:
        //: 1 The idea here is to have multiple created pusher threads push to
        //:   one end of a 'bdlcc::deque', named 'container', while the main
        //:   thread pops off the other end.
        //:
        //: 2 The items pushed are of type 'unsigned'.  The high-order bits of
        //:   of an item identify that pusher thread pushed it, where pusher
        //:   threads have an 'id' in the range '[0 .. NUM_THREADS - 1]'.  The
        //:   low-order 16 bits are incremented between successive items.
        //:
        //: 3 There is a separate 'endDeque' for each thread.  Each thread
        //:   pushes to the back of its 'endDeque' and the main (popper) thread
        //:   pops from the front of it.  To ensure that all items pushed by a
        //:   single 'tryPush*' to 'containter' are adjacent to each other,
        //:   when a push is finished, if it succeeded in pushing at least one
        //:   item, the sequence number after the last number pushed to
        //:   'container' is pushed to 'endDeque'.
        //:
        //: 4 When the main (popper) thread pops an item off of 'container', it
        //:   looks at the high-order bits to determine which thread popped it.
        //:   It then pops the item 'end' off of that thread's 'endDeque'.  We
        //:   then expect the low-order 16-bit sequece values of the next
        //:   several values in the container to be the range
        //:   '[item .. end - 1]', and we verify this.  If this is not the
        //:   case, then the contiguous pushing of elements via 'tryPush*' was
        //:   violated, or the return value of 'tryPush*' did not accurately
        //:   represent the number of items pushed.  (C-1) (C-2) (C-3)
        //:
        //: 5 To make sure we vary the number of items to be pushed, we use the
        //:   random number generator 'class' 'Rand1To8' to generate random
        //:   numbers in the range '[1 .. 8]' that we are to try to push.  But
        //:   there is still a worry that the situation will devolve to
        //:   'container' being always nearly full, so that in practice only 1
        //:   or 2 items get successfully pushed per calls.  It would be nice
        //:   to pop a bunch of items at a time to a vector, leaving ample
        //:   clearance under the high water mark for many items to be pushed.
        //:   To avoid this greatly complicating the popper loop in 'main', we
        //:   also create 'class' 'BufferedPopper', that contains a
        //:   'bsl::vector' buffer to pop to, and a reference to 'container',
        //:   and an instance of 'Rand1To8'.  The 'Rand1To8' object is called
        //:   to return a value in the range '[5 .. 12]' of items to be popped
        //:   in a call, that are then attempted to be popped via 'tryPop*'.
        //:   This will leave plenty of room below the high water mark so that
        //:   pushes immediately following it will be able to push long ranges
        //:   successfully.  We also track how many items were pushed each call
        //:   in the mask 'visitFlags', setting the bit of 'visitFlags'
        //:   corresponding to each number of items pushed.  At the end of the
        //:   pass, we verify that ll bits of 'visitFlags' in the range
        //:   '[0 .. 8]' are set (a mask value of 0x1ff).
        //:
        //: 6 Normally, a minimum of 2 items must be pushed -- a value pushed
        //:   to 'container', and the subsequent value to the pusher's
        //:   'endDeque'.  This becomes problematic if we are near the end of
        //:   the sequence of values to be pushed, with only 1 value left to
        //:   go.  In this case we push a 'nullItem', that is a value that
        //:   will correspond to no other valid item, and terminate the pusher
        //:   pass.  The popper will check each popped item to see if it is a
        //:   'nullItem' and not expect a given sequence value or a
        //:   corresponding value in an 'endDeque' in that case.
        //
        // Testing:
        //   void tryPushBack(INPUT_ITER, INPUT_ITER); - mt
        //   void tryPushFront(INPUT_ITER, INPUT_ITER); - mt
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED RANGE TRY PUSH TEST\n"
                             "=================================\n";

        using namespace MULTI_THREADED_TRY_PUSH;

        bdlcc::Deque<unsigned>  container(HIGH_WATER_MARK, &ta);
        bdlcc::Deque<unsigned> *endDeques[NUM_THREADS];
        static unsigned         expecteds[NUM_THREADS];    // all initted to 0

        bslmt::ThreadUtil::Handle handles[NUM_THREADS];
        for (int ii = 0; ii < NUM_THREADS; ++ii) {
            endDeques[ii] = new (ta) bdlcc::Deque<unsigned>(&ta);

            int rc = bslmt::ThreadUtil::create(
                                &handles[ii],
                                MultiThreadedRangeTryPushTest(&container,
                                                              ii,
                                                              endDeques[ii]));
            ASSERT(0 == rc);
        }

        for (int isForward = 0; isForward < 2; ++isForward) {
            BufferedPopper bp(&container, isForward, &ta);
            bsl::memset(expecteds, 0, sizeof(expecteds));

            barrier.wait();
            if (veryVerbose) bsl::printf("Main thread, %s pass start\n",
                                           isForward ? "forward" : "backward");

            unsigned numNulls = 0;
            int      ii = 0;    // total popped from 'bp' and all 'endDeques'
            while (ii < POPS_IN_MAIN) {
                unsigned u = bp();
                ++ii;
                if (nullItem == u) {
                    ++numNulls;
                    continue;
                }
                const unsigned source = u >> ID_SHIFT;
                unsigned&      e = expecteds[source];
                const unsigned end = endDeques[source]->popFront();
                ++ii;
                unsigned s = u & sequenceMask;
                ASSERTV(source, end, s, e, s == e);
                while (++e < end) {
                    u = bp();
                    ++ii;
                    ASSERT(nullItem != u);
                    ASSERT(source == (u >> ID_SHIFT));
                    s = u & sequenceMask;
                    ASSERTV(source, end, s, e, s == e);
                }
                ASSERTV(end, e, end == e);
                ++e;
            }
            ASSERTV(ii, POPS_IN_MAIN == ii);

            ASSERT(bp.isEmpty());
            ASSERT(0 == container.length());
            unsigned sum = numNulls;
            for (int jj = 0; jj < NUM_THREADS; ++jj) {
                sum += expecteds[jj];
                ASSERT(PUSHES_PER_THREAD - 1 <= expecteds[jj]);
                ASSERT(0 == endDeques[jj]->length());
            }
            ASSERTV(POPS_IN_MAIN, sum, numNulls, POPS_IN_MAIN == sum);

            if (veryVerbose) bsl::printf(
                                 "Main thread, %s pass finish, numNulls: %u\n",
                                 isForward ? "forward" : "backward", numNulls);
            barrier.wait();
        }

        for (int ii = 0; ii < NUM_THREADS; ++ii) {
            int rc = bslmt::ThreadUtil::join(handles[ii]);
            ASSERT(0 == rc);

            ta.deleteObject(endDeques[ii]);
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // SINGLE-THREADED SINGLE ITEM FORCE PUSH TEST
        //
        // Concerns:
        //: 1 That single-item 'forcePushFront' and 'forcePushBack' satisfy
        //:   the strong exception guarantee.
        //
        // Plan:
        //: 1 Do some 'forcePushBack's and 'forcePushFront's on a container
        //:   with injected exceptions, testing that the strong guarantee is
        //:   satisfied.
        //:   o Do them on empty and non-empty containers.
        //
        // Testing:
        //   void forcePushBack(const T&); - st
        //   void forcePushFront(const T&); - st
        //   void forcePushBack(T&&); - st
        //   void forcePushFront(T&&); - st
        //   void removeAll(vector<T> *); - st
        // --------------------------------------------------------------------

        if (verbose) cout << "SINGLE-THREADED SINGLE ITEM FORCE PUSH TEST\n"
                             "===========================================\n";

        namespace TC = TEST_CASE_19;

        TC::testForcePushRemoveAll<Element>();
        TC::testForcePushRemoveAll<AElement>();
        TC::testForcePushRemoveAll<MElement>();
        TC::testForcePushRemoveAll<MCElement>();
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // SINGLE-THREADED RANGE TRY & FORCE PUSH TEST
        //
        // Concerns:
        //: 1 That range-based 'tryPush*' don't violate the high water mark.
        //:
        //: 2 If the number of elements in the range exceeds the number of
        //:   vacancies in the container before the high water mark is reached,
        //:   the high water mark will be reached exactly.
        //:
        //: 3 if the number of elements in the range is less than is necessary
        //:   to reach the high water mark, all elements are pushed.
        //:
        //: 4 That the elements in the container after the push have the
        //:   correct values.
        //
        // Plan:
        //: 1 Iterate, creating 'bdlcc::Deque' objects with high water marks
        //:   ranging from 1 to 9.
        //:
        //: 2 Within that, iterate 'numToPrePush' from 0 to 4.
        //:
        //: 3 Within that, iterate 'numToPush' from 'numToPrePush' to 20.
        //:
        //: 4 Do a forced range push of 'numToPrePush' elements.
        //:
        //: 5 Do a 'tryPush*' of the next 'numToPush - numToPrePush' elements
        //:   as a range.
        //:
        //: 6 Calculate the expected length of the container and verify it.
        //:   (C-1) (C-2) (C-3)
        //:
        //: 7 Pop the contents out of the container and verify they are as
        //:   expected.  (C-4)
        //:
        //: 8 Inject exceptions into the calls of each of the manipulators
        //:   under test to verify that they provide the strong guarantee.
        //
        // Testing:
        //   void tryPushBack(INPUT_ITER, INPUT_ITER); - st
        //   void tryPushFront(INPUT_ITER, INPUT_ITER); - st
        //   void forcePushBack(INPUT_ITER, INPUT_ITER); - st
        //   void forcePushFront(INPUT_ITER, INPUT_ITER); - st
        // --------------------------------------------------------------------

        if (verbose) cout << "SINGLE-THREADED RANGE TRY PUSH TEST\n"
                             "===================================\n";

        bsl::vector<AElement> elements(&ta);
        elements.resize(20, AElement('Z', &sa));
        for (int ii = 0; ii < 20; ++ii) {
            elements[ii].setData('A' + ii);
        }

        for (size_t hwm = 1; hwm < 10; ++hwm) {
            AObj mX(hwm, &ta);                const AObj& X = mX;

            for (size_t numToPrePush = 0; numToPrePush < 4; ++numToPrePush) {
                for (size_t numToPush = numToPrePush; numToPush < 20;
                                                                 ++numToPush) {
                    if (0 < numToPrePush) {
                        BEGIN_EXCEP_TEST_OBJ(AElement, mX) {
                            mX.forcePushBack(elements.begin() + 0,
                                             elements.begin() + numToPrePush);
                        } END_EXCEP_TEST_OBJ
                    }
                    else {
                        mX.forcePushBack(elements.begin() + 0,
                                         elements.begin() + numToPrePush);
                    }

                    if (numToPrePush < bsl::min(hwm, numToPush)) {
                        BEGIN_EXCEP_TEST_OBJ(AElement, mX) {
                            mX.tryPushBack(elements.begin() + numToPrePush,
                                           elements.begin() + numToPush);
                        } END_EXCEP_TEST_OBJ
                    }
                    else {
                        mX.tryPushBack(elements.begin() + numToPrePush,
                                       elements.begin() + numToPush);
                    }

                    const size_t expected = bsl::max(bsl::min(hwm, numToPush),
                                                     numToPrePush);
                    ASSERT(X.length() == expected);
                    for (size_t ii = 0; ii < expected; ++ii) {
                        AElement e(&ta);
                        mX.popFront(&e);
                        ASSERTV(ii, elements[ii], e, elements[ii] == e);
                    }

                    ASSERT(0 == X.length());
                }
            }

            for (size_t numToPrePush = 0; numToPrePush < 4; ++numToPrePush) {
                for (size_t numToPush = numToPrePush; numToPush < 20;
                                                                 ++numToPush) {
                    if (0 < numToPrePush) {
                        BEGIN_EXCEP_TEST_OBJ(AElement, mX) {
                            mX.forcePushFront(elements.begin() + 0,
                                              elements.begin() + numToPrePush);
                        } END_EXCEP_TEST_OBJ
                    }
                    else {
                        mX.forcePushFront(elements.begin() + 0,
                                          elements.begin() + numToPrePush);
                    }

                    if (numToPrePush < bsl::min(hwm, numToPush)) {
                        BEGIN_EXCEP_TEST_OBJ(AElement, mX) {
                            mX.tryPushFront(elements.begin() + numToPrePush,
                                            elements.begin() + numToPush);
                        } END_EXCEP_TEST_OBJ
                    }
                    else {
                        mX.tryPushFront(elements.begin() + numToPrePush,
                                        elements.begin() + numToPush);
                    }

                    const size_t expected = bsl::max(bsl::min(hwm, numToPush),
                                                     numToPrePush);
                    ASSERT(X.length() == expected);
                    for (size_t ii = 0; ii < expected; ++ii) {
                        AElement e(&ta);
                        mX.popBack(&e);
                        ASSERTV(ii, elements[ii], e, elements[ii] == e);
                    }

                    ASSERT(0 == X.length());
                }
            }
        }

        ASSERTV(da.numAllocations(), 0 == da.numAllocations());
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // MULTITHREADED FORCED PUSH TEST
        //
        // Concerns:
        //: 1 That forced pushes, both range pushes and single pushes, always
        //:   succeed.
        //:
        //: 2 That in the case of range pushes, all elements in the range
        //:   wind up being contiguous in the container.
        //:
        //: 3 That the values pushed are preserved until popped.
        //
        // Plan:
        //: 1 In this test, we shall have 4 pusher subthreads pushing onto one
        //:   container under test, and the main thread will pop items from
        //:   the container.
        //:
        //: 2 The items pushed and popped are of type 'unsigned', that is split
        //:   into 3 fields:
        //:   o The high-order 2 bits (bits 30 and 31), that indicate which of
        //:     4 pusher threads pushed the item.  These are the 'source' bits.
        //:   o The next lowest bit (bit 29), that indicates that the item is
        //:     the first or last of a sequence of at least 2 items pushed by a
        //:     single range push.  This is the 'start-end' bit.
        //:   o The low-order 16 bits, that are the 'sequence' bits that
        //:     represent a number.  For any pusher thread, the number
        //:     represented by the sequence bits is incremented with every
        //:     object that is pushed.
        //:   o The other bits are unused.
        //:
        //: 3 The pusher thread iterates, doing one push per loop.
        //:
        //: 4 At the beginning of the pusher loop the 'Rand1To8' random number
        //:   generator 'class' is used to generate a number, 'len', in the
        //:   range '[1 .. 8]' to determine how many items are to be pushed
        //:   this iteration.  All elements pushed have their 'source' bits
        //:   set to indicate the index of the pusher.
        //:   o If '1 == len', a single, non-range 'forcePush*' is used, and
        //:     the 'start-end' bit of the value pushed is not set.
        //:   o If 'len' has any other value, a range 'forcePush*' is used.
        //:     Only the first and last elements of the range pushed have their
        //:     'start-end' bits set.
        //:
        //: 5 In the main (popper) thread, there is an array of 4 'expecteds'
        //:   unsigned values, that start out at zero.  For a thread 'ii',
        //:   'expecteds[ii]' is the value expected for the sequence bits of
        //:   the next item received from that thread to have.
        //:
        //: 5 The popper loop iterates until it has received 4 times as many
        //:   items as one pusher thread pushes.
        //:
        //: 6 At the beginning of the popper loop, the first element is popped.
        //:   The source bits are separated out, and used to accessed the
        //:   member of 'expecteds' corresponding to the thread that pushed the
        //:   item.  The sequence bits are verified to match the
        //:   'expecteds[source]' value.
        //:
        //: 7 If the 'start-end' bit was set, then we know the item was the
        //:   first in a range.  We continue popping more elements, verifying
        //:   that the sequence bits increase by 1 each time, until we
        //:   encounter an item whose 'start-end' bit is set, and then we know
        //:   we have received the whole range pushed.  During this sequence we
        //:   verify that all items have the same source bits. (C-2)
        //:
        //: 8 Plan parts 6 and 7 together establish (C-1) (C-3)
        //
        // Testing:
        //   void forcePushBack(const T&); - mt
        //   void forcePushBack(INPUT_ITER, INPUT_ITER); - mt
        //   void forcePushFront(const T&); - mt
        //   void forcePushFront(INPUT_ITER, INPUT_ITER); - mt
        //   void forcePushBack(T&&); - mt
        //   void forcePushFront(T&&); - mt
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED FORCED PUSH TEST\n"
                             "==============================\n";

        using namespace MULTI_THREADED_FORCE_PUSH;

        Container container(10, &ta);

        bslmt::ThreadUtil::Handle handles[4];
        for (int ii = 0; ii < 4; ++ii) {
            int rc =bslmt::ThreadUtil::create(
                                   &handles[ii],
                                   MultiThreadedForcePushTest(&container, ii));
            ASSERT(0 == rc);
        }

        static unsigned expecteds[4];    // all initted to 0

        barrier.wait();
        if (veryVerbose) bsl::printf("Main thread, forward pass start\n");

        int len;
        int ii = 0;
        for (; ii < 0x40000; ii += len) {
            unsigned       u = container.popFront();
            const unsigned source = u >> 30;
            ASSERT(expecteds[source]++ == (u & sequenceMask));
            len = 1;
            if (u & startEndMask) {
                do {
                    u = container.popFront();
                    ASSERT(source == (u >> 30));
                    ASSERT(expecteds[source]++ == (u & sequenceMask));
                    ++len;
                } while (!(u & startEndMask));
            }
            ASSERT(len <= 8);
        }
        ASSERT(0x40000 == ii);

        ASSERT(0 == container.length());
        for (int jj = 0; jj < 4; ++jj) {
            ASSERT(0x10000 == expecteds[jj]);
        }
        if (veryVerbose) bsl::printf("Main thread, forward pass finish\n");
        bsl::memset(expecteds, 0, sizeof(expecteds));
        barrier.wait();
        barrier.wait();
        if (veryVerbose) bsl::printf("Main thread, backward pass start\n");

        for (ii = 0; ii < 0x40000; ii += len) {
            unsigned       u = container.popBack();
            const unsigned source = u >> 30;
            ASSERT(expecteds[source]++ == (u & sequenceMask));
            len = 1;
            if (u & startEndMask) {
                do {
                    u = container.popBack();
                    ASSERT(source == (u >> 30));
                    ASSERT(expecteds[source]++ == (u & sequenceMask));
                    ++len;
                } while (!(u & startEndMask));
            }
            ASSERT(len <= 8);
        }
        ASSERT(0x40000 == ii);

        if (veryVerbose) bsl::printf("Main thread, backward pass finish\n");

        for (int ii = 0; ii < 4; ++ii) {
            int rc = bslmt::ThreadUtil::join(handles[ii]);
            ASSERT(0 == rc);
        }

        ASSERT(0 == container.length());
        for (int jj = 0; jj < 4; ++jj) {
            ASSERT(0x10000 == expecteds[jj]);
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // STRESS-TEST HIGH WATER MARK WITH FORCING
        //
        // Concerns:
        //: 1 That normal pushes never result in the high water mark being
        //:   violated.
        //: 2 That range construction can violate the high water mark.
        //: 3 That single-element forced pushes can violate the high water
        //:   mark.
        //: 4 That range forced pushes can violate the high water mark.
        //: 5 That 'tryPush{Front,Back}' never violate the high water mark.
        //
        // Plan:
        //: 1 Have 3 passes, the first two are 'forward', pushing to the back
        //:   of the queue and reading from the front, and the third backwards.
        //:   o In the first pass, pre-populate the queue with some initial
        //:     values from a range construct, and then observe that the queue
        //:     is the desired length, that will sometimes be greater than the
        //:     high water mark.  (C-2) (C-3) (C-4)
        //:   o In the second two passes, pre-populate the queue with some
        //:     initial values via a combination of 'tryPush*', single-element
        //:     'forcePush*', and range 'forcePush*'.  If the queue is filled
        //:     to or above the high water mark, Attempt another 'tryPush*' and
        //:     verify that it fails.  (C-5)
        //: 2 Have a background thread push more elements to the queue.  The
        //:   elements in the queue are always ranging from 0 to 39, in
        //:   sequence.  The background thread is to alternate pushing with
        //:   'tryPush*' and 'push*'.
        //: 3 Have the main thread pop the 40 elements from the opposite end of
        //:   the queue from the end they were pushed to, and verify the values
        //:   increase in sequence.  Have the main thread pause periodically to
        //:   ensure that the background thread is able to fill the queue to
        //:   the high water mark.
        //: 4 Since none of the pushes following the initial pre-population of
        //:   the queue are forced pushes, then once the queue length decreases
        //:   to the high water mark, it should never increase past the high
        //:   water mark again.  Have the main thread verify this.  (C-1)
        //:   (C-5)
        //
        // Testing
        //   Deque(INPUT_ITER, INPUT_ITER, size_t, Alloc *);
        //   void forcePushBack(INPUT_ITER, INPUT_ITER); - st
        //   void forcePushFront(INPUT_ITER, INPUT_ITER); - st
        //   int tryPushBack(const T&); - st
        //   int tryPushFront(const T&); - st
        //   int tryPushBack(T&&); - st
        //   int tryPushFront(T&&); - st
        //   int tryPushBack(const T&); - mt
        //   int tryPushFront(const T&); - mt
        // --------------------------------------------------------------------

        if (verbose) cout << "STRESS-TEST HIGH WATER MARK WITH FORCING\n"
                             "========================================\n";

        using namespace FORCE_AND_TRY_HIGH_WATER_MARK_STRESS_TEST;

        const int START_LENGTHS[] = { 0, 1, 2, 5, 10, 15 };
        enum { NUM_START_LENGTHS = sizeof START_LENGTHS /
                                                       sizeof *START_LENGTHS };

        const size_t HIGH_WATER_MARK = 3;
        int          timesFull = 0;
        int          minTimesFull = 0;

        static const Element startArray[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                                          11, 12, 13, 14, 15 };

        for (int move = 0; move < 2; ++move) {
            for (int ti = 0; ti < NUM_START_LENGTHS; ++ti) {
                const unsigned startLength = START_LENGTHS[ti];

                if (veryVerbose) P(startLength);

                Obj mX(startArray + 0,
                       startArray + startLength,
                       HIGH_WATER_MARK,
                       &ta);
                const Obj& X = mX;
                bslmt::Barrier barrier(2);

                ASSERT(X.length() == startLength);

                bslmt::ThreadUtil::Handle handle;
                bslmt::ThreadUtil::create(&handle,
                                          FATHWMStressTest(&mX,
                                          &barrier));

                barrier.wait();

                size_t len;
                bool   caughtUp = X.length() <= HIGH_WATER_MARK;
                for (int expected = 0; expected < 40; ++expected) {
                    if (caughtUp && 0 == expected % 5) {
                        ++minTimesFull;
                        bslmt::ThreadUtil::microSleep(10 * 1000);
                    }
                    const size_t len = X.length();
                    if (len <= HIGH_WATER_MARK) {
                        caughtUp = true;
                    }
                    ASSERT(!caughtUp || len <= HIGH_WATER_MARK);
                    timesFull += caughtUp && HIGH_WATER_MARK == len;
                    Element e;
                    if (expected & 1) {
                        e = mX.popFront();
                    }
                    else {
                        mX.popFront(&e);
                    }
                    ASSERTV(e, expected, e == expected);
                }

                len = X.length();
                ASSERTV(X.length(), 0 == len);

                if (startLength > 0) {
                    if (move) {
                        Element e = startArray[0];
                        ASSERT(0 == mX.tryPushBack(MUtil::move(e)));
                    }
                    else {
                        ASSERT(0 == mX.tryPushBack(startArray[0]));
                    }
                    ASSERT(1 == X.length());
                    if (startLength >= 2) {
                        mX.forcePushBack(startArray + 1,
                                         startArray + startLength - 1);
                        mX.forcePushBack(startArray[startLength - 1]);
                    }
                }
                ASSERT(X.length() == startLength);
                if (HIGH_WATER_MARK <= startLength) {
                    if (move) {
                        Element e = startArray[0];
                        ASSERT(0 != mX.tryPushBack(MUtil::move(e)));
                    }
                    else {
                        ASSERT(0 != mX.tryPushBack(startArray[0]));
                    }
                }
                ASSERT(X.length() == startLength);
                barrier.wait();

                caughtUp = X.length() <= HIGH_WATER_MARK;
                for (int expected = 0; expected < 40; ++expected) {
                    if (caughtUp && 0 == expected % 5) {
                        ++minTimesFull;
                        bslmt::ThreadUtil::microSleep(10 * 1000);
                    }
                    const size_t len = X.length();
                    if (len <= HIGH_WATER_MARK) {
                        caughtUp = true;
                    }
                    ASSERT(!caughtUp || len <= HIGH_WATER_MARK);
                    timesFull += caughtUp && HIGH_WATER_MARK == len;
                    Element e;
                    if (expected & 1) {
                        e = mX.popFront();
                    }
                    else {
                        mX.popFront(&e);
                    }
                    ASSERTV(e, expected, e == expected);
                }

                len = X.length();
                ASSERTV(X.length(), 0 == len);

                if (startLength > 0) {
                    if (move) {
                        Element e = startArray[0];
                        ASSERT(0 == mX.tryPushFront(MUtil::move(e)));
                    }
                    else {
                        ASSERT(0 == mX.tryPushFront(startArray[0]));
                    }
                    ASSERT(1 == X.length());
                    if (startLength >= 2) {
                        mX.forcePushFront(startArray + 1,
                                          startArray + startLength - 1);
                        mX.forcePushFront(startArray[startLength - 1]);
                    }
                }
                ASSERT(X.length() == startLength);
                if (HIGH_WATER_MARK <= startLength) {
                    if (move) {
                        Element e = startArray[0];
                        ASSERT(0 != mX.tryPushFront(MUtil::move(e)));
                    }
                    else {
                        ASSERT(0 != mX.tryPushFront(startArray[0]));
                    }
                }
                ASSERT(X.length() == startLength);
                barrier.wait();

                while (X.length() < HIGH_WATER_MARK) {
                    bslmt::ThreadUtil::microSleep(10 * 1000);
                }

                caughtUp = X.length() <= HIGH_WATER_MARK;
                for (int expected = 0; expected < 40; ++expected) {
                    if (caughtUp && 0 == expected % 5) {
                        ++minTimesFull;
                        bslmt::ThreadUtil::microSleep(10 * 1000);
                    }
                    const size_t len = X.length();
                    if (len <= HIGH_WATER_MARK) {
                        caughtUp = true;
                    }
                    ASSERT(!caughtUp || len <= HIGH_WATER_MARK);
                    timesFull += caughtUp && HIGH_WATER_MARK == len;
                    Element e;
                    if (expected & 1) {
                        e = mX.popBack();
                    }
                    else {
                        mX.popBack(&e);
                    }
                    ASSERTV(e, expected, e == expected);
                }

                bslmt::ThreadUtil::join(handle);
            }

            ASSERTV(timesFull, minTimesFull, timesFull >= minTimesFull);

            if (veryVerbose) { P_(minTimesFull); P(timesFull); }
            ASSERT(0 == da.numAllocations());
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // STRESS-TEST HIGH WATER MARK
        //
        // Concerns:
        //: 1 That normal pushes never result in the high water mark being
        //:   violated.
        //
        // Plan:
        //: 1 Have one thread aggressively push to the queue, and another
        //:   thread read from the queue, occasionally pausing, verify that
        //:   the correct values are obtained from the queue, and that the
        //:   high water mark is never violated.  (C-1)
        //
        // Testing
        //   void pushFront(const TYPE&); - mt
        //   void pushBack(const TYPE&); - mt
        //   void pushFront(TYPE&&); - mt
        //   void pushBack(TYPE&&); - mt
        //   T popBack(); - mt
        //   void popBack(T *); - mt
        //   T popFront(); - mt
        //   void popFront(T *); - mt
        // --------------------------------------------------------------------

        if (verbose) cout << "STRESS-TEST HIGH WATER MARK\n"
                             "===========================\n";

        using namespace HIGH_WATER_MARK_STRESS_TEST;

        const size_t HIGH_WATER_MARKS[] = { 1, 2, 3, 5 };
        enum { NUM_HIGH_WATER_MARKS = sizeof HIGH_WATER_MARKS /
                                                    sizeof *HIGH_WATER_MARKS };

        int timesFull = 0;

        for (int ti = 0; ti < NUM_HIGH_WATER_MARKS; ++ti) {
            const size_t HIGH_WATER_MARK = HIGH_WATER_MARKS[ti];

            Obj           mX(HIGH_WATER_MARK, &ta);
            const Obj&    X = mX;
            bslmt::Barrier barrier(2);

            ASSERT(0 == X.length());

            bslmt::ThreadUtil::Handle handle;
            bslmt::ThreadUtil::create(&handle, HWMStressTest(&mX, &barrier));

            while (X.length() < HIGH_WATER_MARK) {
                bslmt::ThreadUtil::microSleep(10 * 1000);
            }

            for (int expected = 0; expected < 40; ++expected) {
                if (0 == expected % 5) {
                    bslmt::ThreadUtil::microSleep(10 * 1000);
                }
                const size_t len = X.length();
                ASSERT(len <= HIGH_WATER_MARK);
                timesFull += HIGH_WATER_MARK == len;
                if (expected & 1) {
                    ASSERT(mX.popFront() == expected);
                }
                else {
                    Element e;
                    mX.popFront(&e);
                    ASSERT(e == expected);
                }
            }

            ASSERT(0 == X.length());

            barrier.wait();

            while (X.length() < HIGH_WATER_MARK) {
                bslmt::ThreadUtil::microSleep(10 * 1000);
            }

            for (int expected = 0; expected < 40; ++expected) {
                if (0 == expected % 5) {
                    bslmt::ThreadUtil::microSleep(10 * 1000);
                }
                const size_t len = X.length();
                ASSERT(len <= HIGH_WATER_MARK);
                timesFull += HIGH_WATER_MARK == len;
                if (expected & 1) {
                    ASSERT(mX.popBack() == expected);
                }
                else {
                    Element e;
                    mX.popBack(&e);
                    ASSERT(e == expected);
                }
            }

            bslmt::ThreadUtil::join(handle);
        }

        const int minTimesFull = NUM_HIGH_WATER_MARKS * 2 * 40 / 5;
        ASSERTV(timesFull, minTimesFull, timesFull >= minTimesFull);

        if (veryVerbose) { P_(minTimesFull); P(timesFull); }
        ASSERTV(da.numAllocations(), 0 == da.numAllocations());
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // RANDOM_PUSH_POP_TEST
        //
        // Concerns:
        //: 1 That the container functions correctly with simultaneous
        //:   contention of pushes and pops from the same end.  The preceding
        //:   sequence constraint test tested the integrity of the container
        //:   well, but it did not test simultaneous push/pop contention from
        //:   the same end.
        //
        // Plan:
        //: 1 Have many threads that push to the container, and half as many
        //:   threads that pop from the container.  Each item pushed contains a
        //:   'threadIdx' to indicate which pusher thread pushed it, and a
        //:   random number.  Each pusher pthread sums all the random numbers
        //:   it pushes, and each popper thread keeps separate sums for the
        //:   numbers popped from each pushers.  At the end, the poppers
        //:   aggregate their sums for each pusher into the global
        //:   'popperTotalsByPusher' array, and after the threads are joined
        //:   this array is compared to the sums aggregated by the pushers
        //:   themselves (C-1).
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RANDOM_PUSH_POP_TEST\n"
                             "====================\n";

        using namespace RANDOM_PUSH_POP_TEST;

        for (int ii = 0; ii < NUM_PUSHERS; ++ii) {
            pusherTotals[ii] = 0;
        }

        bslmt::ThreadGroup tg(&ta);

        tg.addThreads(PusherThread(), NUM_PUSHERS);
        tg.addThreads(PopperThread(), NUM_POPPERS);

        tg.joinAll();

        ASSERT(0 == deque.length());

        for (int ii = 0; ii < NUM_PUSHERS; ++ii) {
            ASSERT(pusherTotals[ii] == popperTotalsByPusher[ii]);
            if (veryVerbose) { P_(ii); P(pusherTotals[ii]); }
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // SEQUENCE CONSTRAINT TEST
        //
        // Concerns:
        //: 1 Test the deque, pushing into one end and popping from the other,
        //:   under heavy contention (up to 16 threads accessing the container
        //:   simultaneously).
        //: 2 Test it forward and backward.
        //: 3 Test popping with return-by-value, and popping with
        //:   return-through-pointer.
        //: 4 Do proctor access while the container is under contention from
        //:   other threads, both for pushing and popping objects.
        //
        // Plan:
        //: 1 The whole test always tests the container under heavy contention.
        //:   (C-1).
        //: 2 Have a boolean 'backwards' to determine whether the container is
        //:   run forward or backward (C-2).
        //: 3 Have a boolean 'popThroughPtr' to determine whether pops through
        //:   a pointer occur, or pops that return by value (C-3)
        //: 4 Iterate through all 4 possible states of 'backwards' and
        //:   'popThroughPtr'.
        //: 5 Vary through all cominations of 1-8 pushers and 1-8 poppers.
        //: 6 Each pusher is to push thousands of elements into the container,
        //:   each popper is to pop from it until the expected number of
        //:   elements have been popped.
        //: 7 Both the pusher and popper, periodically, access the container
        //:   through proctors (C-4).
        // --------------------------------------------------------------------

        if (verbose) cout << "SEQUENCE CONSTRAINT TEST\n"
                             "========================\n";

        using namespace SEQUENCE_CONSTRAINT_TEST;

        alloc = &ta;
        bdlcc::Deque<Item> mX(HIGH_WATER_MARK, &ta);
        container = &mX;

        for (int config = 0; config < 4; ++config) {
            backwards     = config & 1;
            popThroughPtr = config & 2;

            for (numPushers = 1; numPushers <= NUM_THREADS; numPushers += 2) {
                totalToPop = NUM_ITERATIONS * numPushers;

                for (int numPoppers = 1; numPoppers <= NUM_THREADS;
                                                             numPoppers += 2) {
                    if (veryVerbose) {
                        cout <<
                            "----------------------------------------------\n";
                        P_(backwards); P_(popThroughPtr); P_(numPushers);
                        P(numPoppers);
                    }

                    bslmt::Barrier barrierObj(numPushers + numPoppers + 1);
                    barrier = &barrierObj;

                    nextThreadId = 0;
                    numPopped    = 0;

                    bslmt::ThreadGroup tg(&ta);

                    tg.addThreads(PusherThread(), numPushers);
                    tg.addThreads(PopperThread(), numPoppers);

                    barrierObj.wait();

                    tg.joinAll();

                    bdlcc::Deque<Item>::Proctor proctor(&mX);

                    ASSERTV(proctor->size(), 0 == proctor->size());
                    proctor->clear();
                }
            }
        }

        ASSERTV(da.numAllocations(), 0 == da.numAllocations());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // MULTITHREADED TEST OF TRYPOPFRONT, TRYPOPBACK
        //
        // Concern:
        //: 1 That tryPopFront, tryPopBack will work in a multithreaded
        //:   context.
        //:
        //: 2 That elements are moved or copied as expected.
        //
        // Plan:
        //: 1 Have two functors, TestPopFront and TestPopBack.  Each functor
        //:   will pop items off the deque and verify that the values are in
        //:   the expected sequence.  Simultaneously, the main thread will push
        //:   the sequence of values to the deque, frequently pausing.
        //:
        //: 2 Use 'MCElement' and query items popped for their move state.
        //
        // Testing:
        //   int tryPopFront(TYPE *); - mt
        //   void tryPopFront(size_t, vector<TYPE> *); - mt
        //   void tryPopFront(size_t, vector<TYPE> *, bool); - mt
        //   int tryPopBack(TYPE *); - mt
        //   void tryPopBack(size_t, vector<TYPE> *); - mt
        //   void tryPopBack(size_t, vector<TYPE> *, bool); - mt
        // --------------------------------------------------------------------

        if (verbose) cout << "MULTITHREADED TEST OF TRYPOPFRONT, TRYPOPBACK\n"
                             "=============================================\n";

        namespace TC = TEST_CASE_12;

        TC::testMultiThreadedTryPop<Element>();
        TC::testMultiThreadedTryPop<AElement>();
        TC::testMultiThreadedTryPop<MElement>();
        TC::testMultiThreadedTryPop<MCElement>();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TEST HIGH WATER MARK BLOCKING
        //
        // Concern:
        //: 1 That the deque blocks pushes properly when it is at the high
        //:   watermark.
        //
        // Plan:
        //: 1  Call both forms of push... with the deque in a variety of
        //:   states and observe the results.
        //
        // Testing:
        //   void pushFront(const TYPE&); - mt
        //   void pushBack(const TYPE&); - mt
        //   void pushFront(TYPE&&); - mt
        //   void pushBack(TYPE&&); - mt
        //   int timedPopBack(TYPE *, const TimeInterval&); - mt
        //   int timedPopFront(TYPE *, const TimeInterval&); - mt
        //   int timedPushBack(TYPE&&, const TimeInterval&); - mt
        //   int timedPushFront(TYPE&&, const TimeInterval&); - mt
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST HIGH WATER MARK BLOCKING\n"
                             "=============================\n";

        namespace TC = TEST_CASE_11;

        TC::highWaterMarkTest<Element>();
        TC::highWaterMarkTest<AElement>();
        TC::highWaterMarkTest<MElement>();
        TC::highWaterMarkTest<MCElement>();
      }  break;
      case 10: {
        // --------------------------------------------------------------------
        // TEST BLOCKING ON EMPTY DEQUE
        //
        // Concern:
        //: 1 Does the deques block pops properly when it is empty.
        //
        // Plan:
        //: 1 Call both forms of 'timedPop*' with the deque in a variety of
        //:   states and observe the results.
        //
        // Testing:
        //   int timedPopBack(TYPE *, const bsls::TimeInterval&);
        //   int timedPopFront(TYPE *, const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST BLOCKING ON EMPTY DEQUE\n"
                             "============================\n";

        namespace TC = TEST_CASE_10;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj                       mX(&ta);
        Element                   e = TC::VALID_VAL;
        bslmt::ThreadUtil::Handle handle;
        bslmt::Barrier            barrier(2);

        // Note microSleeps on Solaris can arbitrarily take as long as 2 sec,
        // so have a pessimistic timeout time -- normally this will take MUCH
        // less than 9 seconds.

        bsls::TimeInterval timeout = u::now() + bsls::TimeInterval(9.0);

        ASSERT(u::now() < timeout);

        ASSERT(0 == mX.length());

        int status = -1;

        TC::EmptyDequeFunctor functor(&mX, &barrier, &status);

        bslmt::ThreadUtil::create(&handle, functor);

        mX.pushFront(e);
        mX.pushBack(e);

        ASSERT(2 == mX.length());

        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));
        ASSERT(!barrier.timedWait(timeout));

        ASSERT(0 == mX.length());

        for (int i = 0; i < 4; ++i) {
            enum { SLEEP_TIME = 10 * 1000 };        // 10 mSec

            ASSERT(!barrier.timedWait(timeout));
            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(SLEEP_TIME);
            ASSERT(0 == mX.length());

            mX.pushBack(e);
            ASSERT(1 >= mX.length());

            ASSERT(!barrier.timedWait(timeout));
            ASSERT(0 == mX.length());
        }

        e = TC::TERMINATE;
        mX.pushFront(e);
        ASSERT(!barrier.timedWait(timeout));

        {
            void *sts;
            bslmt::ThreadUtil::join(handle, &sts);
            ASSERTV(sts,    !sts);
            ASSERTV(status, 0 == status);  // Expected after 'TERMINATE'.
        }

        ASSERT(u::now() < timeout);
      }  break;
      case 9: {
        // --------------------------------------------------------------------
        // TEST TRYPOPFRONT, TRYPOPBACK -- SINGLE THREAD
        //
        // Concern:
        //: 1 That tryPopFront and tryPopBack work as designed in a single -
        //:   threaded context.
        //
        // Plan:
        //: 1 Call both forms of tryPop... both with an empty deque and with
        //:   a deque containing items, verify that return values (if any)
        //:   are correct and that correct data is returned.
        //:
        //: 2 Inject exceptions into the calls of each of the manipulators
        //:   under test to verify that they provide the strong guarantee.
        //
        // Testing:
        //   int tryPopFront(TYPE *); - st
        //   void tryPopFront(size_t, vector<TYPE> *); - st
        //   int tryPopBack(TYPE *); - st
        //   void tryPopBack(size_t, vector<TYPE> *); - st
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST TRYPOPFRONT, TRYPOPBACK -- SINGLE THREAD\n"
                             "=============================================\n";

        namespace TC = TEST_CASE_9;

        TC::testSingleThreadedTryPop<bsl::vector<Element> >();
        TC::testSingleThreadedTryPop<bsl::vector<AElement> >();
        TC::testSingleThreadedTryPop<bsl::vector<MElement> >();
        TC::testSingleThreadedTryPop<bsl::vector<MCElement> >();

        TC::testSingleThreadedTryPop<std::vector<Element> >();
        TC::testSingleThreadedTryPop<std::vector<AElement> >();
        TC::testSingleThreadedTryPop<std::vector<MElement> >();
        TC::testSingleThreadedTryPop<std::vector<MCElement> >();

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        TC::testSingleThreadedTryPop<std::pmr::vector<Element> >();
        TC::testSingleThreadedTryPop<std::pmr::vector<AElement> >();
        TC::testSingleThreadedTryPop<std::pmr::vector<MElement> >();
        TC::testSingleThreadedTryPop<std::pmr::vector<MCElement> >();
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING REMOVEALL
        //
        // Concerns:
        //: 1 That 'removeAll' empties the deque, that it saves the elements in
        //:   proper order into the specified buffer if one is provided, and
        //:   destroys them properly otherwise.
        //: 2 That 'removeAll' properly signals threads waiting for the
        //:   'notFullCondition'.
        //
        // Plan:
        //: 1 Create a deque object with multiple elements endequed.
        //:
        //: 2 Invoke 'removeAll' and verify that the deque is empty and that
        //:   all elements have been copied to the optionally specified buffer
        //:   in the proper order.
        //:
        //: 3 Finally, create a deque with a high watermark, and fill it, then
        //:   create a thread to push into it, verify that thread is blocked.
        //:   In the main thread, invoke 'removeAll' and verifies that pushing
        //:   thread is unblocked.
        //:
        //: 4 Inject exceptions into the calls of 'removeAll' to 'vector' to
        //:   verify that it provides the strong guarantee.
        //
        // Testing:
        //   removeAll();
        //   removeAll(bsl::vector<T> *buffer);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING REMOVEALL\n"
                             "=================\n";

        AElement VA('A', &sa);
        AElement VB('B', &sa);
        AElement VC('C', &sa);
        AElement VD('D', &sa);
        AElement VE('E', &sa);

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            AObj mX(&ta);

            mX.pushBack(VA);
            mX.pushBack(VB);
            mX.pushBack(VC);

            mX.removeAll();
            ASSERT(0 == mX.length());
        }

        if (verbose) cout << "bsl::vector\n";
        {
            AObj                  mX(&ta);
            bsl::vector<AElement> buffer(&ta);

            mX.pushBack(VA);
            mX.pushBack(VB);
            mX.pushBack(VC);

            BEGIN_EXCEP_TEST_OBJ_VEC(AElement, mX, buffer) {
                mX.removeAll(&buffer);
            } END_EXCEP_TEST_OBJ_VEC
            ASSERT(0 == mX.length());
            ASSERT(3 == buffer.size());
            ASSERT(VA == buffer[0]);
            ASSERT(VB == buffer[1]);
            ASSERT(VC == buffer[2]);
        }
        {
            AObj                  mX(&ta);
            bsl::vector<AElement> buffer(&ta);

            buffer.push_back(VA);
            buffer.push_back(VB);

            mX.pushBack(VC);
            mX.pushBack(VD);
            mX.pushBack(VE);

            BEGIN_EXCEP_TEST_OBJ_VEC(AElement, mX, buffer) {
                mX.removeAll(&buffer);
            } END_EXCEP_TEST_OBJ_VEC
            ASSERT(0 == mX.length());
            ASSERT(5 == buffer.size());
            ASSERT(VA == buffer[0]);
            ASSERT(VB == buffer[1]);
            ASSERT(VC == buffer[2]);
            ASSERT(VD == buffer[3]);
            ASSERT(VE == buffer[4]);
        }

        if (verbose) cout << "std::vector\n";
        {
            AObj                  mX(&ta);
            std::vector<AElement> buffer;

            mX.pushBack(VA);
            mX.pushBack(VB);
            mX.pushBack(VC);

            mX.removeAll(&buffer);
            ASSERT(0 == mX.length());
            ASSERT(3 == buffer.size());
            ASSERT(VA == buffer[0]);
            ASSERT(VB == buffer[1]);
            ASSERT(VC == buffer[2]);
        }
        {
            AObj                  mX(&ta);
            std::vector<AElement> buffer;

            buffer.push_back(VA);
            buffer.push_back(VB);

            mX.pushBack(VC);
            mX.pushBack(VD);
            mX.pushBack(VE);

            mX.removeAll(&buffer);
            ASSERT(0 == mX.length());
            ASSERT(5 == buffer.size());
            ASSERT(VA == buffer[0]);
            ASSERT(VB == buffer[1]);
            ASSERT(VC == buffer[2]);
            ASSERT(VD == buffer[3]);
            ASSERT(VE == buffer[4]);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
        if (verbose) cout << "std::pmr::vector\n";
        {
            AObj                       mX(&ta);
            std::pmr::vector<AElement> buffer;

            mX.pushBack(VA);
            mX.pushBack(VB);
            mX.pushBack(VC);

            mX.removeAll(&buffer);
            ASSERT(0 == mX.length());
            ASSERT(3 == buffer.size());
            ASSERT(VA == buffer[0]);
            ASSERT(VB == buffer[1]);
            ASSERT(VC == buffer[2]);
        }
        {
            AObj                       mX(&ta);
            std::pmr::vector<AElement> buffer;

            buffer.push_back(VA);
            buffer.push_back(VB);

            mX.pushBack(VC);
            mX.pushBack(VD);
            mX.pushBack(VE);

            mX.removeAll(&buffer);
            ASSERT(0 == mX.length());
            ASSERT(5 == buffer.size());
            ASSERT(VA == buffer[0]);
            ASSERT(VB == buffer[1]);
            ASSERT(VC == buffer[2]);
            ASSERT(VD == buffer[3]);
            ASSERT(VE == buffer[4]);
        }
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING PROCTORS
        //
        // Concerns:
        //: 1 That proctors work as specified, and that they operate correctly
        //:   under heavy multi-threaded contention.
        //
        // Plan:
        //: 1 Create a 'bdlcc::Deque' object and populate it with a few values.
        //: 2 Single-Threaded 'ConstProctor'.
        //:   o Lock our 'bdlcc::Deque' with a 'ConstProctor' 'PR' and access
        //:     it through 'PR'.
        //: 3 Single-Threaded 'Proctor'.
        //:   o Lock our 'Bdlcc::Deque' with a 'Proctor' 'pr', access it, and
        //:     modify it thourhg 'pr'.
        //:   o Destroy the proctor.
        //:   o Pop the remaining elements out of the container and observe the
        //:     state is as expected, using thrad-safe modificrs and
        //:     accesssors.
        //: 4 Multi-threaded Proctor Testing
        //:   o Create 2 'bdlcc::Deque' objects, a source and a destination,
        //:     both with high water marks.
        //:   o Create 5 popper threads, that will pop elements from the dest
        //:     deque.
        //:   o Create 1 transferrer thread, that will use 3 proctors, one of
        //:     which is a const proctor, that will use proctor access to
        //:     transfer elements from the source deque to the dest deque, then
        //:     erase them from the source deque.  Note this transfer will
        //:     violate the high water mark, and we will observe this does not
        //:     create a problem (ie by signalling of condtions becoming
        //:     confused) and we will observe that no deadlock happens in spite
        //:     of heavy contention on the dest deque while the high water mark
        //:     is sometimes being violated.
        //:   o Create 5 direct pusher threads, that will push elements to the
        //:     dest deque, blocking when they reach the high water mark.
        //:   o Create 5 source pusher threads, that will push elements to the
        //:     dest deque, blocking when they reach the high water mark.
        //:   o Join all the threads (all of the threads are set up to process
        //:     a specefic number of elements, so after they have processed
        //:     their respective numbers of elements, they will just exit.
        //:   o Observe that both deques are empty after the threads are
        //:     joined.
        //
        // Testing:
        //   ConstProctor
        //   Proctor
        // --------------------------------------------------------------------

        namespace TC = TEST_CASE_7;

        if (verbose) cout << "TESTING PROCTORS\n"
                             "================\n";

        const Element VA =  1.2;
        const Element VB = -5.7;
        const Element VC = 43.2;
        const Element VD = -22.1;
        const Element VE = 15.3;
        const Element VF = -1e6;

        {
            TC::maxDstSize      = 0;
            TC::maxSrcSize      = 0;

            Obj        mX(&ta);
            const Obj& X = mX;

            mX.pushBack(VA);
            mX.pushBack(VB);
            mX.pushBack(VC);

            Obj        mY(&ta);
            const Obj& Y = mY;

            mY.pushBack(VD);
            mY.pushBack(VE);
            mY.pushBack(VF);

            const bsl::deque<Element> *pdx, *pdy;

            if (verbose) cout << "Single-Threaded ConstProctor Testing\n";
            {
                Obj::ConstProctor pr(&X);

                pdx = &*pr;

                ASSERT(!pr.isNull());
                ASSERT(3  == pr->size());
                ASSERT(VA == pr->front());
                ASSERT(VA == (*pr)[0]);
                ASSERT(VB == (*pr)[1]);
                ASSERT(VC == (*pr)[2]);
                ASSERT(VC == pr->back());

                ASSERT(3 == pr->end()  - pr->begin());
                ASSERT(3 == pr->rend() - pr->rbegin());

                pr.release();

                ASSERT(pr.isNull());

                ASSERT(VC == mX.popBack());
                mX.pushBack(VC);

                ConstProctor prB;    const Obj::ConstProctor& PRB = prB;
                prB.load(&X);
                ASSERT(&*prB == pdx);
                ASSERT(&*PRB == pdx);
                ASSERT(!prB.isNull());
                ASSERT(!PRB.isNull());

                ASSERT(3  == prB->size());
                ASSERT(VA == prB->front());
                ASSERT(VA == (*prB)[0]);
                ASSERT(VB == (*prB)[1]);
                ASSERT(VC == (*prB)[2]);
                ASSERT(VC == prB->back());

                ASSERT(3 == prB->end()  - prB->begin());
                ASSERT(3 == prB->rend() - prB->rbegin());

                ASSERT(3  == PRB->size());
                ASSERT(VA == PRB->front());
                ASSERT(VA == (*PRB)[0]);
                ASSERT(VB == (*PRB)[1]);
                ASSERT(VC == (*PRB)[2]);
                ASSERT(VC == PRB->back());

                ASSERT(3 == PRB->end()  - PRB->begin());
                ASSERT(3 == PRB->rend() - PRB->rbegin());

                // Negative testing

                bsls::AssertTestHandlerGuard hG;

                ASSERT_SAFE_PASS(prB[2]);
                ASSERT_SAFE_FAIL(prB[3]);
                ASSERT_SAFE_FAIL(prB[1000 * 1000]);
                ASSERT_SAFE_FAIL(prB[-1]);

                const Obj::ConstProctor PRC(&Y);

                pdy = &*PRC;

                ASSERTV(pdx, pdy, pdx != pdy);
                ASSERT(!PRC.isNull());

                ASSERT(3  == PRC->size());
                ASSERT(VD == PRC->front());
                ASSERT(VD == (*PRC)[0]);
                ASSERT(VE == (*PRC)[1]);
                ASSERT(VF == (*PRC)[2]);
                ASSERT(VF == PRC->back());

                ASSERT(3 == PRC->end()  - PRC->begin());
                ASSERT(3 == PRC->rend() - PRC->rbegin());

                // Negative testing

                ASSERT_SAFE_PASS(PRC[2]);
                ASSERT_SAFE_FAIL(PRC[3]);
                ASSERT_SAFE_FAIL(PRC[1000 * 1000]);
                ASSERT_SAFE_FAIL(PRC[-1]);
            }

            if (verbose) cout << "Single-Threaded Proctor Testing\n";
            {
                Obj::Proctor pr(&mX);    const Obj::Proctor& PR = pr;

                ASSERT(!pr.isNull());
                ASSERT(&*pr == pdx);

                ASSERT(3  == pr->size());
                ASSERT(VA == pr->front());
                ASSERT(VA == (*pr)[0]);
                ASSERT(VB == (*pr)[1]);
                ASSERT(VC == (*pr)[2]);
                ASSERT(VC == pr->back());

                ASSERT(3 == pr->end()  - pr->begin());
                ASSERT(3 == pr->rend() - pr->rbegin());

                ASSERT(3  == PR->size());
                ASSERT(VA == PR->front());
                ASSERT(VA == (*PR)[0]);
                ASSERT(VB == (*PR)[1]);
                ASSERT(VC == (*PR)[2]);
                ASSERT(VC == PR->back());

                ASSERT(3 == PR->end()  - PR->begin());
                ASSERT(3 == PR->rend() - PR->rbegin());

                pr->pop_front();
                ASSERT(VB == pr->front());
                ASSERT(VC == pr->back());
                ASSERT(2  == pr->size());

                ASSERT(2 == pr->end()  - pr->begin());
                ASSERT(2 == pr->rend() - pr->rbegin());

                PR->push_front(0.0);
                PR->pop_front();
                ASSERT(VB == PR->front());
                ASSERT(VC == PR->back());
                ASSERT(2  == PR->size());

                ASSERT(2 == PR->end()  - PR->begin());
                ASSERT(2 == PR->rend() - PR->rbegin());

                pr->push_back(VA);
                ASSERT(VB == pr->front());
                ASSERT(VB == (*pr)[0]);
                ASSERT(VC == (*pr)[1]);
                ASSERT(VA == (*pr)[2]);
                ASSERT(VA == pr->back());

                ASSERT(3  == pr->size());

                ASSERT(3 == pr->end()  - pr->begin());
                ASSERT(3 == pr->rend() - pr->rbegin());

                PR->pop_back();
                PR->push_back(VA);
                ASSERT(VB == PR->front());
                ASSERT(VB == (*PR)[0]);
                ASSERT(VC == (*PR)[1]);
                ASSERT(VA == (*PR)[2]);
                ASSERT(VA == PR->back());

                ASSERT(3  == PR->size());

                ASSERT(3 == PR->end()  - PR->begin());
                ASSERT(3 == PR->rend() - PR->rbegin());

                pr.load(&mY);
                ASSERT(&*pr != pdx);
                ASSERT(&*pr == pdy);
                ASSERT(&*PR != pdx);
                ASSERT(&*PR == pdy);

                ASSERT(!pr.isNull());
                ASSERT(!PR.isNull());

                // Modify 'mX' to show that it's now unlocked.

                ASSERT(VB == mX.popFront());
                mX.pushFront(VB);

                ASSERT(3  == pr->size());
                ASSERT(VD == pr->front());
                ASSERT(VD == (*pr)[0]);
                ASSERT(VE == (*pr)[1]);
                ASSERT(VF == (*pr)[2]);
                ASSERT(VF == pr->back());

                ASSERT(3 == pr->end()  - pr->begin());
                ASSERT(3 == pr->rend() - pr->rbegin());

                ASSERT(3  == PR->size());
                ASSERT(VD == PR->front());
                ASSERT(VD == (*PR)[0]);
                ASSERT(VE == (*PR)[1]);
                ASSERT(VF == (*PR)[2]);
                ASSERT(VF == PR->back());

                ASSERT(3 == PR->end()  - PR->begin());
                ASSERT(3 == PR->rend() - PR->rbegin());

                // Negative testing

                bsls::AssertTestHandlerGuard hG;

                ASSERT_SAFE_PASS(pr[2]);
                ASSERT_SAFE_FAIL(pr[3]);
                ASSERT_SAFE_FAIL(pr[1000 * 1000]);
                ASSERT_SAFE_FAIL(pr[-1]);

                ASSERT_SAFE_PASS(PR[2]);
                ASSERT_SAFE_FAIL(PR[3]);
                ASSERT_SAFE_FAIL(PR[1000 * 1000]);
                ASSERT_SAFE_FAIL(PR[-1]);

                Proctor prB;
                prB.load(&mX);
                ASSERT(&*prB == pdx);
                ASSERT(!prB.isNull());

                pr.release();
                ASSERT(pr.isNull());
            }

            ASSERT(3  == X.length());

            ASSERT(VB == mX.popFront());
            ASSERT(VA == mX.popBack());
            ASSERT(VC == mX.popFront());

            ASSERT(0  == X.length());
        }

        if (verbose) cout << "Multi-Threaded Proctor Testing\n";
        for (int backWard = 0; backWard < 2; ++backWard) {
            Obj dstDeque(TC::k_DST_HIGH_WATER_MARK, &ta);
            Obj srcDeque(TC::k_SRC_HIGH_WATER_MARK, &ta);

            const TC::ProctorTestFunctor popper(&dstDeque,
                                                &srcDeque,
                                                TC::e_POPPER,
                                                0,
                                                backWard);
            const TC::ProctorTestFunctor transferrer(&dstDeque,
                                                     &srcDeque,
                                                     TC::e_TRANSFERRER,
                                                     0,
                                                     backWard);

            bslmt::ThreadUtil::Handle threads[TC::k_NUM_THREADS];
            int                       threadIdx = 0;
            int                       rc;
            for (unsigned ti = 0; ti < TC::k_NUM_POPPERS; ++ti) {
                rc = bslmt::ThreadUtil::create(&threads[threadIdx++], popper);
                ASSERT(0 == rc);
            }

            rc = bslmt::ThreadUtil::create(&threads[threadIdx++], transferrer);
            ASSERT(0 == rc);

            unsigned ti;
            for (ti = 1; ti <= TC::k_NUM_DIRECT_PUSHERS; ++ti) {
                const TC::ProctorTestFunctor directPusher(&dstDeque,
                                                          &srcDeque,
                                                          TC::e_DIRECT_PUSHER,
                                                          ti,
                                                          backWard);
                rc = bslmt::ThreadUtil::create(&threads[threadIdx++],
                                               directPusher);
                ASSERT(0 == rc);
            }

            for (; ti <= TC::k_NUM_DIRECT_PUSHERS + TC::k_NUM_SRC_PUSHERS;
                                                                        ++ti) {
                const TC::ProctorTestFunctor pusher(&dstDeque,
                                                    &srcDeque,
                                                    TC::e_SRC_PUSHER,
                                                    ti,
                                                    backWard);
                rc = bslmt::ThreadUtil::create(&threads[threadIdx++], pusher);
                ASSERT(0 == rc);
            }

            while (threadIdx > 0) {
                rc = bslmt::ThreadUtil::join(threads[--threadIdx]);
                ASSERT(0 == rc);
            }

            ASSERT(0 == dstDeque.length());
            ASSERT(0 == srcDeque.length());

            if (verbose) { P_(TC::maxDstSize); P(TC::maxSrcSize); }
        }

        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == da.numAllocations());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING TIMED PUSH & POP FUNCTIONS WITH A HIGH WATER MARK
        //
        // Concerns:
        //: 1 That the 'timedPushBack' and 'timedPushFront' functions properly
        //:   time out unless the deque has fewer items than the high-water
        //:   mark
        //
        // Plan:
        //: 1 Create a deque object with a positive high-water mark, then push
        //:   that many items on the deque and verify that none of the push
        //:   operations block, then in a different thread add one more item
        //:   and verify that it now times out.  Then if the high-water mark is
        //:   positive, again add one more item in second thread, and in the
        //:   main thread, after half the timeout (or less, for margin), pop
        //:   one item and verify that the other thread is now unblocked and
        //:   that the element has been added to the deque.
        //:
        //: 2 Inject exceptions into the calls of each of the manipulators
        //:   under test to verify that they provide the strong guarantee.
        //:
        //: 3 Do a separate, single-threaded test of 'timedPop{Front,Back}'
        //:   with exceptions injected.  This can't be done in the main
        //:   multithreaded test because exceptions get injected into the
        //:   wrong threads where they are not caught.
        //
        // Testing:
        //   int timedPopBack( TYPE *, const bsls::TimeInterval&);
        //   int timedPopFront(TYPE *, const bsls::TimeInterval&);
        //   int timedPushBack( const TYPE&, const bsls::TimeInterval&);
        //   int timedPushFront(const TYPE&, const bsls::TimeInterval&);
        // --------------------------------------------------------------------

        namespace TC = TEST_CASE_6;

        if (verbose) cout <<
               "\nTESTING TIMED PUSH & POP FUNCTIONS WITH A HIGH WATER MARK\n"
                 "=========================================================\n";

        static  const struct {
            int          d_lineNum;
            unsigned int d_highWaterMark;
        } VALUES[] = {
            // line     high-water mark
            // ----     ---------------
            { L_ ,                    1 },
            { L_ ,                    3 },
            { L_ ,                   10 },
            { L_ ,                   15 },
        };

        const size_t NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        const int    T = 1 * MICRO_100TH_SEC; // in microseconds

        const bsls::TimeInterval T1(  1 * DECI_SEC);          //  .1s
        const bsls::TimeInterval T4(  4 * DECI_SEC);          //  .4s
        const bsls::TimeInterval T10(10 * DECI_SEC);          // 1.0s

        const AElement VA('A', &sa);    // deque originally filled with these
        const AElement VB('B', &sa);    // one pushed at end after high water
                                        // mark
        const AElement VC('C', &sa);    // never pushed to deque

        bsls::Stopwatch sw;

        if (verbose) cout << "\tWith 'timed{Pop,Push}Back'" << endl;

        for (size_t ti = 0; ti < NUM_VALUES; ++ti) {
            const unsigned int HIGH_WATER_MARK = VALUES[ti].d_highWaterMark;

            AObj                   x(HIGH_WATER_MARK, &ta);
            TC::TimedHWMRecordBack testObj(&x, T10, VB);

            // Verify timed pop on empty deque times out.

            sw.reset();
            sw.start(true);

            AElement ret(VC, &ta);
            ASSERT(0 != x.timedPopBack(&ret, u::now()+T1));
            ASSERT(VC == ret);

            sw.stop();

            // Verify we timed out after 0.1 sec.

            ASSERT(sw.accumulatedWallTime() > 0.09);

            sw.reset();
            sw.start(true);

            ASSERT(HIGH_WATER_MARK == x.highWaterMark());
            for (unsigned int j = 0; j < HIGH_WATER_MARK; ++j) {
                // should not block

                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    ASSERT(0 == x.timedPushBack(VA, u::now() + T10));
                } END_EXCEP_TEST_OBJ
            }

            sw.stop();

            // Verify none of the timed pushes timed out.

            ASSERT(sw.accumulatedUserTime() < 0.1);
            ASSERT(HIGH_WATER_MARK == x.length());

            // Additional timed push now times out.

            sw.reset();
            sw.start(true);

            ASSERT(0 != x.timedPushBack(VC, u::now() + T1));

            sw.stop();

            ASSERT(sw.accumulatedWallTime() > 0.09);
            ASSERT(HIGH_WATER_MARK == x.length());
            ASSERT(VA == u::myBack(x));

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread,
                                      &TC::doTimedHWMRecordBack,
                                      &testObj);

            while (0 == TC::waitingFlag) {
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }
            ASSERT(0 != TC::waitingFlag);

            // Wait a little to make certain subthread push is blocked on high
            // water mark.

            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(T);

            ASSERT(HIGH_WATER_MARK == x.length());

            sw.reset();
            sw.start(true);

            ret.setData(VC.data());
            ASSERT(0 == x.timedPopBack(&ret, u::now()+T10));
            ASSERT(VA == ret);

            sw.stop();

            // Verify the timed pop didn't time out.

            ASSERT(sw.accumulatedUserTime() < 0.1);

            while (x.length() < HIGH_WATER_MARK) {
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }
            ASSERT(HIGH_WATER_MARK == x.length());

            // The subthread has pushed

            sw.reset();
            sw.start(true);

            ret.setData(VC.data());
            ASSERT(0 == x.timedPopBack(&ret, u::now()+T10));
            ASSERT(VB == ret);

            sw.stop();

            // Verify the timed pop didn't time out.

            ASSERT(sw.accumulatedUserTime() < 0.1);

            ASSERT(HIGH_WATER_MARK - 1 == x.length());

            bslmt::ThreadUtil::join(thread);
            TC::waitingFlag = 0;
        }

        if (verbose) cout << "\tWith 'timed{Pop,Push}Front'" << endl;

        for (unsigned ti = 0; ti < NUM_VALUES; ++ti) {
            const unsigned int HIGH_WATER_MARK = VALUES[ti].d_highWaterMark;

            AObj                    x(HIGH_WATER_MARK, &ta);
            TC::TimedHWMRecordFront testObj(&x, T10, VB);

            // Verify timed pop on empty deque times out.

            sw.reset();
            sw.start(true);

            AElement ret(VC, &ta);
            ASSERT(0 != x.timedPopFront(&ret, u::now()+T1));
            ASSERT(VC == ret);

            sw.stop();

            // Verify we timed out after 0.1 sec.

            ASSERT(sw.accumulatedWallTime() > 0.09);

            sw.reset();
            sw.start(true);

            ASSERT(HIGH_WATER_MARK == x.highWaterMark());
            for (unsigned int j = 0; j < HIGH_WATER_MARK; ++j) {
                // should not block

                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    ASSERT(0 == x.timedPushFront(VA, u::now() + T10));
                } END_EXCEP_TEST_OBJ
            }

            sw.stop();

            // Verify none of the timed pushes timed out.

            ASSERT(sw.accumulatedUserTime() < 0.1);
            ASSERT(HIGH_WATER_MARK == x.length());

            // Additional timed push now times out.

            sw.reset();
            sw.start(true);

            ASSERT(0 != x.timedPushFront(VC, u::now() + T1));

            sw.stop();

            ASSERT(sw.accumulatedWallTime() > 0.09);
            ASSERT(HIGH_WATER_MARK == x.length());
            ASSERT(VA == u::myFront(x));

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread,
                                      &TC::doTimedHWMRecordFront,
                                      &testObj);

            while (0 == TC::waitingFlag) {
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }
            ASSERT(0 != TC::waitingFlag);

            // Wait a little to make certain subthread push is blocked on high
            // water mark.

            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(T);

            ASSERT(HIGH_WATER_MARK == x.length());

            sw.reset();
            sw.start(true);

            ret.setData(VC.data());
            ASSERT(0 == x.timedPopFront(&ret, u::now()+T10));
            ASSERT(VA == ret);

            sw.stop();

            // Verify the timed pop didn't time out.

            ASSERT(sw.accumulatedUserTime() < 0.1);

            while (x.length() < HIGH_WATER_MARK) {
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }
            ASSERT(HIGH_WATER_MARK == x.length());

            // The subthread has pushed

            sw.reset();
            sw.start(true);

            ret.setData(VC.data());
            ASSERT(0 == x.timedPopFront(&ret, u::now()+T10));
            ASSERT(VB == ret);

            sw.stop();

            // Verify the timed pop didn't time out.

            ASSERT(sw.accumulatedUserTime() < 0.1);

            ASSERT(HIGH_WATER_MARK - 1 == x.length());

            bslmt::ThreadUtil::join(thread);
            TC::waitingFlag = 0;
        }

        if (verbose) cout <<
                    "Single-threaded 'timedPop{Front,Back}' with exceptions\n";
        for (int ti = 0; ti < 2; ++ti) {
            bool doFront = ti;

            enum { k_NUM_ELEMENTS = 20 };

            AObj x(&ta);
            bsl::vector<AElement> v(&ta);
            v.insert(v.end(), k_NUM_ELEMENTS, VC);

            {
                AObj::Proctor p(&x);
                p->insert(p->end(), k_NUM_ELEMENTS, VA);
            }

            sw.reset();
            sw.start(true);
            double lastTime = sw.accumulatedWallTime();
            for (int ii = 20 - 1; 0 <= ii; --ii) {
                int pass = 0;
                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    ++pass;

                    for (int jj = 0; jj < k_NUM_ELEMENTS; ++jj) {
                        const AElement& EXP = jj <= ii ? VC : VA;
                        ASSERTV(VA, VC, jj, ii, v[jj], EXP, EXP == v[jj]);
                    }

                    ASSERTV(ii, x.length(), ii + 1 == (IntPtr) x.length());
                    for (int jj = 0; jj <= ii; ++jj) {
                        const AObj::Proctor p(&x);
                        const bsl::deque<AElement>& d = *p;
                        ASSERTV(VA, jj, d.size(), d[jj], VA == d[jj]);
                    }

                    ASSERT(sw.accumulatedWallTime() - lastTime < 0.05);
                    lastTime = sw.accumulatedWallTime();

                    if (doFront) {
                        bsl::deque<AElement>::iterator it;
                        {
                            AObj::Proctor p(&x);
                            it = p->end();
                        }

                        ASSERT(0 == x.timedPopFront(&v[ii], u::now()+T10));
                        {
                            AObj::Proctor p(&x);
                            ASSERT(p->end() == it);
                        }
                    }
                    else {
                        bsl::deque<AElement>::iterator it;
                        {
                            AObj::Proctor p(&x);
                            it = p->begin();
                        }

                        ASSERT(0 == x.timedPopBack(&v[ii], u::now()+T10));
                        {
                            AObj::Proctor p(&x);
                            ASSERT(p->begin() == it);
                        }
                    }

                    ASSERT(sw.accumulatedWallTime() - lastTime < 0.05);
                    ASSERTV(ii, pass, PLAT_EXC, (PLAT_EXC ? 1 : 0) < pass);

                    ASSERT(VA == v[ii]);

                    ASSERTV(ii, x.length(), ii == (IntPtr) x.length());
                    for (int jj = 0; jj < ii; ++jj) {
                        const AObj::Proctor p(&x);
                        const bsl::deque<AElement>& d = *p;
                        ASSERTV(VA, jj, d.size(), d[jj], VA == d[jj]);
                    }

                    for (int jj = 0; jj < k_NUM_ELEMENTS; ++jj) {
                        const AElement& EXP = jj < ii ? VC : VA;
                        ASSERTV(VA, VC, jj, ii, v[jj], EXP, EXP == v[jj]);
                    }

                    lastTime = sw.accumulatedWallTime();
                } END_EXCEP_TEST_OBJ
            }
            sw.stop();
        }

        ASSERT(0 == ta.numBytesInUse());
        ASSERTV(da.numAllocations(), 0 == da.numAllocations());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PUSH FUNCTIONS WITH HIGH WATER MARK
        //
        // Concerns:
        //: 1 That the 'pushBack' and 'pushFront' functions properly block
        //:   unless the deque has fewer items than the high-water mark.
        //
        // Plan:
        //: 1 Create a deque object with a positive high-water mark, then push
        //:   that many items on the deque and verify that none of the push
        //:   operations block, then in a different thread add one more item
        //:   and verify that it now blocks.  In the main thread, pop one item
        //:   and verify that the other thread is now unblocked and that the
        //:   element has been added to the deque.  When creating without high
        //:   water mark, many insertions should never block.
        //
        // Testing:
        //   Deque(int highWaterMark, Alloc *alloc = 0);
        //   size_t highWaterMark() const;
        // --------------------------------------------------------------------

        namespace TC = TEST_CASE_5;

        if (verbose) cout << "TESTING PUSH FUNCTIONS WITH HIGH WATER MARK\n"
                             "===========================================\n";

        static  const struct {
            int d_lineNum;
            int d_highWaterMark;
        } VALUES[] = {
            // line     high-water mark
            // ----     ---------------
            { L_ ,                    1 },
            { L_ ,                    2 },
            { L_ ,                   10 },
        };

        const size_t NUM_VALUES = sizeof VALUES / sizeof *VALUES;
        const int    T = 1 * MICRO_100TH_SEC; // 0.1 sec in microseconds

        const Element VA = 1.2;
        const Element VB = -5.7;

        if (verbose) cout << "\tWith 'pushBack'" << endl;
        for (size_t i = 0; i< NUM_VALUES; ++i)
        {
            {
                const int         LINE            = VALUES[i].d_lineNum;
                const bsl::size_t HIGH_WATER_MARK = VALUES[i].d_highWaterMark;

                if (veryVerbose) {
                    P_(LINE); P_(HIGH_WATER_MARK);
                }

                Obj                        x(HIGH_WATER_MARK, &ta);
                TC::StraightHWMFunctorBack testObj(&x, VB);

                ASSERTV(i, HIGH_WATER_MARK == x.highWaterMark() );
                for (unsigned j = 0; j < HIGH_WATER_MARK; ++j) {
                    x.pushBack(VA);                 // non-blocking
                }

                bslmt::ThreadUtil::Handle thread;
                bslmt::ThreadUtil::create(&thread, testObj);

                for (int j = 0; 0 == TC::waitingFlag && j < 50; ++j) {
                    bslmt::ThreadUtil::yield();
                    bslmt::ThreadUtil::microSleep(T);
                }

                ASSERT(HIGH_WATER_MARK == x.length());

                ASSERTV(i, HIGH_WATER_MARK, 0 != TC::waitingFlag);
                ASSERTV(i, VA == x.popBack());

                for (int j = 0; 0 != TC::waitingFlag && j < 50; ++j) {
                    bslmt::ThreadUtil::yield();
                    bslmt::ThreadUtil::microSleep(T);
                }

                ASSERT(HIGH_WATER_MARK == x.length());

                double ret;
                x.popBack(&ret);
                ASSERTV(VA, VB, ret, HIGH_WATER_MARK, VB == ret);

                bslmt::ThreadUtil::join(thread);
            }
            ASSERTV(i, 0 == ta.numBytesInUse());
        }

        if (verbose) cout << "\tWith 'push_front'" << endl;
        for (unsigned i = 0; i< NUM_VALUES; ++i)
        {
            {
                const int         LINE            = VALUES[i].d_lineNum;
                const bsl::size_t HIGH_WATER_MARK = VALUES[i].d_highWaterMark;

                if (veryVerbose) {
                    P_(LINE); P_(HIGH_WATER_MARK);
                }

                Obj                         x(HIGH_WATER_MARK, &ta);
                TC::StraightHWMFunctorFront testObj(&x, VB);

                ASSERTV(i, HIGH_WATER_MARK == x.highWaterMark() );
                for (unsigned j = 0; j < HIGH_WATER_MARK; ++j) {
                    x.pushBack(VA); // non-blocking
                }

                bslmt::ThreadUtil::Handle thread;
                bslmt::ThreadUtil::create(&thread, testObj);

                for (int j = 0; 0 == TC::waitingFlag && j < 50; ++j) {
                    bslmt::ThreadUtil::yield();
                    bslmt::ThreadUtil::microSleep(T);
                }

                ASSERT(HIGH_WATER_MARK == x.length());
                ASSERTV(i, HIGH_WATER_MARK, 0 != TC::waitingFlag);
                ASSERTV(i, VA == x.popFront() );

                for (int j = 0; 0 != TC::waitingFlag && j < 50; ++j) {
                    bslmt::ThreadUtil::yield();
                    bslmt::ThreadUtil::microSleep(T);
                }

                ASSERT(HIGH_WATER_MARK == x.length());

                double ret;
                x.popFront(&ret);
                ASSERTV(VA, VB, ret, HIGH_WATER_MARK, VB == ret);

                bslmt::ThreadUtil::join(thread);
            }

            ASSERTV(i, 0 == ta.numBytesInUse());
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING TIMED POP FUNCTIONS -- MT
        //
        // Concerns:
        //: 1 That the 'timedPopBack' and 'timedPopFront' functions block
        //:   properly when an item is not available.
        //: 2 That they time out properly when no item is available.
        //
        // Plan:
        //: 1 Create a deque object, pop within a different thread, verify that
        //:   thread is waiting and times out, then signal the other thread and
        //:   wait for half its timeout (or less, for margin) to push an
        //:   object, go to sleep and verify upon waking up that the other
        //:   thread has popped the object properly.
        //
        // Testing:
        //   int timedPopBack(TYPE *, const TimeInterval&); - mt
        //   int timedPopFront(TYPE *, const TimeInterval&); - mt
        // --------------------------------------------------------------------

        using namespace TEST_CASE_4;

        if (verbose) cout << "TESTING TIMED POP FUNCTIONS -- MT\n"
                             "=================================\n";

        if (verbose) cout << "\tWith 'timedPopBack'" << endl;
        bslma::TestAllocator ta(veryVeryVeryVerbose);
        {
            bsls::TimeInterval T10(10 * DECI_SEC);          // 1s

            bslmt::Barrier barrier(2);
            AObj           x(&ta);       const AObj& X = x;

            AElement           VA('A', &ta);
            TimedPopRecordBack testAObj(&x, &barrier, T10, VA);

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread,
                                      &testClass4BackCaller,
                                      &testAObj);

            barrier.wait();

            // thread will time out on a pop

            barrier.wait();

            ASSERT(0 == X.length());
            x.pushBack( VA );

            barrier.wait();

            bslmt::ThreadUtil::join(thread);

            ASSERT(0 == X.length());
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

        if (verbose) cout << "\tWith 'timedPopFront'" << endl;
        {
            bsls::TimeInterval T10(10 * DECI_SEC); // 1s

            bslmt::Barrier barrier(2);
            AObj           x(&ta);        const AObj& X = x;

            AElement            VA('A', &ta);
            TimedPopRecordFront testAObj(&x, &barrier, T10, VA);

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread,
                                      &testClass4FrontCaller,
                                      &testAObj);

            barrier.wait();

            // thread will time out on pop

            barrier.wait();

            ASSERT(0 == X.length());
            x.pushFront( VA );

            barrier.wait();

            bslmt::ThreadUtil::join(thread);

            ASSERT(0 == X.length());
        }

        if (verbose) cout << "\tEnsure wait time on timeout" << endl;
        {
            bsls::TimeInterval timeOut(1 * DECI_SEC);    // in seconds

            Obj x(&ta);

            ASSERT(0 == x.length());

            Element            result = -5;
            bsls::TimeInterval start = u::now();
            int                sts = x.timedPopFront(&result, start + timeOut);
            bsls::TimeInterval end = u::now();
            ASSERT(0 != sts);
            ASSERT(end >= start + timeOut);
            ASSERT(-5 == result);    // not assigned to

            ASSERT(0 == x.length());

            start = u::now();
            sts = x.timedPopBack(&result, start + timeOut);
            end = u::now();
            ASSERT(0 != sts);
            ASSERT(end >= start + timeOut);
            ASSERT(-5 == result);    // not assigned to

            ASSERT(0 == x.length());
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PUSH AND POP FUNCTIONS IN MT
        //
        // Concerns:
        //: 1 That the 'popBack' and 'popFront' functions properly block until
        //:   an item is available.
        //
        // Plan:
        //: 1 Create a deque object, pop within a different thread, verify that
        //:   second thread is waiting, then push an object and verify that
        //:   the second thread has correctly popped the object and unblocked.
        //
        // Testing:
        //   T popBack(); - mt
        //   T popFront(); - mt
        //   void popBack(TYPE *); - mt
        //   void popFront(TYPE *); - mt
        //   void pushBack(const T&); - mt
        //   void pushFront(const T&); - mt
        // --------------------------------------------------------------------

        using namespace TEST_CASE_3;

        if (verbose) cout << "TESTING PUSH AND POP FUNCTIONS IN MT\n"
                             "====================================\n";

        if (verbose) cout << "\tWith 'popBack'" << endl;
        bslma::TestAllocator ta(veryVeryVeryVerbose);
        for (int ti = 0; ti < 2; ++ti) {
            const int T = 1 * MICRO_100TH_SEC; // in microseconds
            Obj       x(&ta);

            Element           VA = 1.2;
            PushPopRecordBack testObj(&x, VA, (!ti ? BY_VALUE : THROUGH_PTR));

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread, pushPopFunctionBack, &testObj);

            // Yielding is not bullet-proof because it does not ENSURE that
            // 'testObj' is blocking on the 'popFront', so we make sure by
            // waiting as long as necessary -- to prevent failure in high loads
            // should the 'pushBack' below execute before 'popFront' in
            // 'testObj'.

            int iter = 100;
            while (0 == testObj.waitingFlag() && 0 < --iter) {
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }

            ASSERT(0 != testObj.waitingFlag());

            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(T);

            ASSERT(0 != testObj.waitingFlag());
            x.pushBack( VA );

            bslmt::ThreadUtil::join(thread);

            ASSERT(0 == testObj.waitingFlag());
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

        if (verbose) cout << "\tWith 'popFront'" << endl;
        for (int ti = 0; ti < 2; ++ti) {
            const int T = 1 * MICRO_100TH_SEC; // in microseconds
            Obj       x(&ta);

            Element            VA = 1.2;
            PushPopRecordFront testObj(&x, VA, (!ti ? BY_VALUE : THROUGH_PTR));

            bslmt::ThreadUtil::Handle thread;
            bslmt::ThreadUtil::create(&thread, pushPopFunctionFront, &testObj);

            // See note above.

            int iter = 100;
            while (0 == testObj.waitingFlag() && 0 < --iter) {
                bslmt::ThreadUtil::yield();
                bslmt::ThreadUtil::microSleep(T);
            }

            ASSERT(0 != testObj.waitingFlag());

            bslmt::ThreadUtil::yield();
            bslmt::ThreadUtil::microSleep(T);

            ASSERT(0 != testObj.waitingFlag());
            x.pushFront( VA );

            bslmt::ThreadUtil::join(thread);

            ASSERT(0 == testObj.waitingFlag());
        }
        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // SINGLE-THREADED TESTING SINGLE PUSHES, POPS, AND LENGTH
        //
        // Concerns:
        //: 1 That for a 'bdlcc::Dueue' 'd', 'd.length() == proctor->leNGTH()'
        //:   always.  This is verified by the 'myLength' function, that
        //:   measures the length both ways and asserts they match.  Since it
        //:   is usually impossible to test manipulators without calling
        //:   accessors, or accessors without calling manipulators, this
        //:   testing will take place while the manipulators are being tested
        //:   rather than in a separate section.  Also implement 'myFront' and
        //:   'myBack' that access the front and back of the 'bsl::deque' by
        //:   value via proctors.
        //:
        //: 2 That 'pushBack' has the same effect on the deque as calling
        //:   'push_back on the underlying 'bsl::deque'.
        //:
        //: 3 That 'popBack' calls 'pop_back' on the underlying 'bsl::deque',
        //:   except it also returns the popped value.
        //:
        //: 4 That 'pushFront' has the same effect on the deque as calling
        //:   'push_front on the underlying 'bsl::deque'.
        //:
        //: 5 That 'popFront' calls 'pop_front' on the underlying 'bsl::deque',
        //:   except it also returns the popped value.
        //
        // Plan:
        //: 1 Testing 'pushBack', 'popBack', 'pushFront', and 'popFront':
        //:
        //:   o Push a couple of different values into 'x', the deque, with
        //:     'pushBack', monitoring the length of the deque with 'myLength'
        //:     and monitoring the contents of the deque with 'myFront' and
        //:     'myBack'.  C-2, C-1.
        //:
        //:   o Pop the two values from the 'x', the deque, using 'popBack',
        //:     observing that the correct values are returned, and monitoring
        //:     the length of the deque with 'myLength' and monitoring the
        //:     contents of the deque with 'myBack()' and 'myFront()'.  C-3,
        //:     C-1.
        //:
        //:   o Push a couple of new, different values into 'x', the deque,
        //:     with 'pushFront', monitoring the length of the deque with
        //:     'myLength' and monitoring the contents of the deque with
        //:     'myBack()' and 'myFront()'.  C-4, C-1.
        //:
        //:   o Pop the two values from the 'x', the deque, using 'popFront',
        //:     observing that the correct values are returned, and monitoring
        //:     the length of the deque with 'myLength' and monitoring the
        //:     contents of the deque with 'myBack()' and 'myFront()'.  C-5,
        //:     C-1.
        //:
        //: 2 Iterate, randomly choosing a deque length in the range 0-7.  This
        //:   test tests C-1, C-2, C-3, C-4, and C-5, just more thoroughly.
        //:
        //:   o If the chosen length is longer than the existing deque length,
        //:     grow the deque to the desired deque length by random choosing
        //:     'pushFront' or 'pushBack', and pushing random doubles into the
        //:     deque.  Simultaneously push the same value onto the same end of
        //:     a 'bsl::deque' kept in parallel.
        //:
        //:   o If the chosen length is shorter than the existing deque, shrink
        //:     the deque to the designated deque length by randomly calling
        //:     'popFront' or 'popBack'.  Simultaneously do a similar pop from
        //:     the parallel 'bsl::deque', and observe the values popped are
        //:     identical.
        //:
        //:   o Each iteration, whether growing or shrinking, frequently check
        //:     the length is as expected, via 'myLength', but also verify it
        //:     matches the length of the 'bsl::deque'. check the length of the
        //:     deque, and call 'length()' on the deque and observe that all
        //:     three lengths match with the expected value.
        //:
        //: 3 Repeat step 2, only with the container containing a type that
        //:   allocates, and perform all actions that provide the strong
        //:   exception guarantee with exceptions, ensuring that they do in
        //:   fact provide that guarantee.
        //:
        //: 4 Inject exceptions into the calls of each of the manipulators
        //:   under test to verify that they provide the strong guarantee.
        //
        // Testing:
        //   Deque(bslma::Allocator *basicAllocator = 0);
        //   ~Deque();
        //   void pushFront(const TYPE&); - st
        //   void pushBack(const TYPE&); - st
        //   TYPE popFront(); - st
        //   void popFront(TYPE *); - st
        //   TYPE popBack(); - st
        //   void popBack(TYPE *); - st
        //   size_t length() const; - st
        //   static size_t maxSizeT();
        // --------------------------------------------------------------------

        if (verbose) cout <<
                   "SINGLE-THREADED TESTING SINGLE PUSHES, POPS, AND LENGTH\n"
                   "=======================================================\n";

        bsls::Stopwatch sw;
        sw.start();

        u::RandElement randElement(123456789);

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        if (verbose) cout << "maxSizeT\n";
        {
            size_t exp = 0;
            --exp;

            ASSERT(Obj::maxSizeT() == exp);
        }

        if (verbose) cout << "\t1. Explicit Pushes and Pops\n";
        {
            enum { NUM_V = 4 };
            Element        v[NUM_V];
            const Element *V = &v[0];
            for (int ti = 0; ti < NUM_V; ++ti) {
                v[ti] = randElement();
                for (int tj = 0; tj < ti; ++tj) {
                    ASSERTV(V[tj], V[ti], V[tj] != V[ti]);
                }
                if (veryVerbose) { T_ T_ P_(ti); P(V[ti]); }
            }

            Obj        x(&ta);
            const Obj& X = x;

            if (verbose) cout << "\t\t'pushBack' && 'length'\n";
            {
                ASSERT(0 == u::myLength(X));

                x.pushBack(V[0]);

                ASSERT(1 == u::myLength(X));
                ASSERT(V[0] == u::myBack(X));

                x.pushBack(V[1]);

                ASSERT(2 == u::myLength(X));
                ASSERT(V[0] == u::myFront(X));
                ASSERT(V[1] == u::myBack( X));
            }

            if (verbose) cout << "\t\t'popBack' && 'length'\n";
            {
                ASSERT(2 == u::myLength(X));
                ASSERT(V[1] == x.popBack());

                ASSERT(1 == u::myLength(X));

                x.pushBack(V[1]);
                ASSERT(2 == u::myLength(X));
                Element e;
                x.popBack(&e);
                ASSERT(V[1] == e);

                ASSERT(V[0] == u::myFront(X));
                ASSERT(V[0] == u::myBack( X));

                ASSERT(V[0] == x.popBack());

                ASSERT(0 == u::myLength(X));
            }

            if (verbose) cout << "\t\t'pushFront' && 'length'\n";
            {
                ASSERT(0 == u::myLength(X));

                x.pushFront(V[2]);

                ASSERT(1 == u::myLength(X));
                ASSERT(V[2] == u::myBack(X));

                x.pushFront(V[3]);

                ASSERT(2 == u::myLength(X));
                ASSERT(V[3] == u::myFront(X));
                ASSERT(V[2] == u::myBack( X));
            }

            if (verbose) cout << "\t\t'popFront' && 'length'\n";
            {
                ASSERT(2 == u::myLength(X));
                ASSERT(V[3] == x.popFront());

                ASSERT(1 == u::myLength(X));
                x.pushFront(V[3]);
                ASSERT(2 == u::myLength(X));
                Element e;
                x.popFront(&e);
                ASSERT(V[3] == e);

                ASSERT(V[2] == u::myFront(X));
                ASSERT(V[2] == u::myBack( X));

                ASSERT(V[2] == x.popFront());

                ASSERT(0 == u::myLength(X));
            }
        }

        ASSERT(0 == ta.numBytesInUse());

        if (verbose) cout << "\t2. Random pushes and pops\n";
        {
            // In this block, we have 3 parallel containers 'x', 'xB', and 'd',
            // to which we apply identical pushes and pops, and observe their
            // behavior is always the same.

            Obj                        x(&ta);
            const Obj&                 X  = x;
            Obj                        xB(&ta);
            const Obj&                 XB = xB;
            bsl::deque<Element>        d(&ta);
            const bsl::deque<Element>& D = d;

            ASSERT(0 == D.size());
            ASSERT(0 == X.length());
            ASSERT(u::myLength(X) == X.length());

            unsigned int expectedLength = 0;
            const int    ITERATIONS = veryVeryVerbose ? 50 : 5000;
            u::RandGen   randGen(12345);

            for (int i = 0; i < ITERATIONS; ++i) {
                unsigned int ll;
                do {
                    ll = randGen() % 8;
                } while (expectedLength == ll);
                const unsigned int LENGTH = ll;

                if (expectedLength < LENGTH) {
                    while (expectedLength < LENGTH) {
                        ASSERT(expectedLength == D.size());
                        ASSERT(expectedLength == u::myLength(X));
                        ASSERT(expectedLength == u::myLength(XB));

                        // Generate a fairly random double using 'generate15'.

                        const Element v = randElement();

                        if (randGen() & 0x80) {
                            d. push_back(v);
                            x. pushBack(v);
                            xB.pushBack(v);
                            if (veryVerbose) {
                                T_ T_ P_(i); P_(LENGTH); P_(expectedLength);
                                cout << "\tPUB: " << v << endl;
                            }
                        }
                        else {
                            d. push_front(v);
                            x. pushFront(v);
                            xB.pushFront(v);
                            if (veryVerbose) {
                                T_ T_ P_(i); P_(LENGTH); P_(expectedLength);
                                cout << "\tPUF: " << v << endl;
                            }
                        }

                        ++expectedLength;

                        ASSERT(expectedLength == D.size());
                        ASSERT(expectedLength == u::myLength(X));
                        ASSERT(expectedLength == u::myLength(XB));
                    }
                }
                else {
                    while (expectedLength > LENGTH) {
                        ASSERT(expectedLength == D.size());
                        ASSERT(expectedLength == u::myLength(X));
                        ASSERT(expectedLength == u::myLength(XB));

                        if (randGen() & 0x80) {
                            const Element popped = D.back();
                            d.pop_back();
                            ASSERT(popped == x.popBack());
                            Element e;
                            xB.popBack(&e);
                            ASSERT(popped == e);
                            if (veryVerbose) {
                                T_ T_ P_(i); P_(LENGTH); P_(expectedLength);
                                cout << "\tPOB: " << popped << endl;
                            }
                        }
                        else {
                            const Element popped = D.front();
                            d.pop_front();
                            ASSERT(popped == x.popFront());
                            Element e;
                            xB.popFront(&e);
                            ASSERT(popped == e);
                            if (veryVerbose) {
                                T_ T_ P_(i); P_(LENGTH); P_(expectedLength);
                                cout << "\tPOF: " << popped << endl;
                            }
                        }

                        --expectedLength;

                        ASSERT(expectedLength == D.size());
                        ASSERT(expectedLength == u::myLength(X));
                        ASSERT(expectedLength == u::myLength(XB));
                    }
                }

                ASSERT(LENGTH == expectedLength);
            }
        }

        ASSERT(0 == da.numBlocksTotal());

        if (verbose) cout << "\t3. Explicit Pushes and Pops, Alloc & Excep\n";
        {
            enum { NUM_V = 4 };
            bsl::vector<AElement> v(&ta);
            v.resize(NUM_V);
            const AElement *V = &v[0];
            int val = 'A';
            for (int ti = 0; ti < NUM_V; ++ti, ++val) {
                v[ti].setData(val);
                for (int tj = 0; tj < ti; ++tj) {
                    ASSERTV(V[tj], V[ti], V[tj] != V[ti]);
                }
                if (veryVerbose) { T_ T_ P_(ti); P(V[ti]); }
            }

            AObj        x(&ta);
            const AObj& X = x;

            if (verbose) cout << "\t\t'pushBack' && 'length'\n";
            {
                ASSERT(0 == u::myLength(X));

                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    x.pushBack(V[0]);
                } END_EXCEP_TEST_OBJ

                ASSERT(1 == u::myLength(X));
                ASSERT(V[0] == u::myBack(X));

                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    x.pushBack(V[1]);
                } END_EXCEP_TEST_OBJ

                ASSERT(2 == u::myLength(X));
                ASSERT(V[0] == u::myFront(X));
                ASSERT(V[1] == u::myBack( X));
            }

            if (verbose) cout << "\t\t'popBack' && 'length'\n";
            {
                ASSERT(2 == u::myLength(X));
                {
                    bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                    ASSERT(V[1] == x.popBack());
                }
                ASSERT(1 == u::myLength(X));

                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    x.pushBack(V[1]);
                } END_EXCEP_TEST_OBJ
                ASSERT(2 == u::myLength(X));
                AElement e(&ta);
                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    x.popBack(&e);
                } END_EXCEP_TEST_OBJ
                ASSERT(V[1] == e);

                ASSERT(V[0] == u::myFront(X));
                ASSERT(V[0] == u::myBack( X));

                {
                    bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                    ASSERT(V[0] == x.popBack());
                }
                ASSERT(0 == u::myLength(X));
            }

            if (verbose) cout << "\t\t'pushFront' && 'length'\n";
            {
                ASSERT(0 == u::myLength(X));

                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    x.pushFront(V[2]);
                } END_EXCEP_TEST_OBJ

                ASSERT(1 == u::myLength(X));
                ASSERT(V[2] == u::myBack(X));

                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    x.pushFront(V[3]);
                } END_EXCEP_TEST_OBJ

                ASSERT(2 == u::myLength(X));
                ASSERT(V[3] == u::myFront(X));
                ASSERT(V[2] == u::myBack( X));
            }

            if (verbose) cout << "\t\t'popFront' && 'length'\n";
            {
                ASSERT(2 == u::myLength(X));
                {
                    bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                    ASSERT(V[3] == x.popFront());
                }

                ASSERT(1 == u::myLength(X));
                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    x.pushFront(V[3]);
                } END_EXCEP_TEST_OBJ
                ASSERT(2 == u::myLength(X));
                AElement e(&ta);
                BEGIN_EXCEP_TEST_OBJ(AElement, x) {
                    x.popFront(&e);
                } END_EXCEP_TEST_OBJ
                ASSERT(V[3] == e);

                ASSERT(V[2] == u::myFront(X));
                ASSERT(V[2] == u::myBack( X));

                {
                    bslma::DefaultAllocatorGuard scratchAllocatorGuard(&sa);
                    ASSERT(V[2] == x.popFront());
                }

                ASSERT(0 == u::myLength(X));
            }
        }

        ASSERT(0 == ta.numBytesInUse());
        ASSERT(0 == da.numBlocksTotal());

        sw.stop();

        if (verbose) P(sw.accumulatedWallTime());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Exercise the basic functionality
        //
        // Concerns:
        //: 1 That basic essential functionality is operational for one thread
        //
        // Plan:
        //: 1 Create a deque object using the various constructors, 'pushBack'
        //:   three elements and verify that 'popFront'ing them results in same
        //:   order, then 'pushBack' three elements again and verify that
        //:   'popBack'ing them results in opposite order.  Verify that
        //:   'timedPopFront' will time out, then 'pushFront' another item,
        //:   verify that 'timedPopFront' pops the item, then 'pushFront'
        //:   another item and verify that 'timedPopFront' pops the item.
        //:   Finally, verify that one can gain modifiable access to the mutex,
        //:   condition variable, and deque, perform same operations and
        //:   release the mutex.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        const Element VA = 1.2;
        const Element VB = -5.7;
        const Element VC = 1234.99;
        const Element VD = 1.2e11;
        const Element VE = -4.7e-10;
        const Element VF = 4.321;
        const Element VG = 5.432;
        const Element VH = -6.54;
        const Element VI = -7.65e15;
        const Element VJ = 8.765e-4;

        const Element vArray[] = { VA, VB, VC, VD, VE, VF, VG, VH, VI, VJ };

        if (verbose) cout << "Exercising default c'tor and basic methods\n";
        {
            Obj x1(&ta);

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            Element tmp;

            ASSERT(VA == (x1.popFront(&tmp), tmp));
            ASSERT(VB == (x1.popFront(&tmp), tmp));
            ASSERT(VC == (x1.popFront(&tmp), tmp));

            x1.pushFront(VA);
            x1.pushFront(VB);
            x1.pushFront(VC);

            ASSERT(VA == (x1.popBack(&tmp), tmp));
            ASSERT(VB == (x1.popBack(&tmp), tmp));
            ASSERT(VC == (x1.popBack(&tmp), tmp));

            Element front;
            Element back;
            int     result;

            result = x1.timedPopFront( &front,
                                       u::now().addMilliseconds(250));
            ASSERT(0 != result);

            x1.pushBack(VA);
            result = x1.timedPopFront( &front,
                                       u::now().addMilliseconds(250));
            ASSERT(0 == result);
            ASSERT(VA == front);

            result = x1.timedPopBack( &back,
                                      u::now().addMilliseconds(250));
            ASSERT(0 != result);

            x1.pushBack(VB);
            result = x1.timedPopBack( &back,
                                      u::now().addMilliseconds(250));
            ASSERT(0 == result);
            ASSERT(VB == back);
        }

        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

        if (verbose) cout << "Exercising bsl::deque c'tor\n";
        {
            bsl::deque<Element> q1(&ta);  const bsl::deque<Element>& Q1 = q1;

            Obj x1(Q1.begin(), Q1.end(), &ta);

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            Element tmp;

            ASSERT(VA == (x1.popFront(&tmp), tmp));
            ASSERT(VB == (x1.popFront(&tmp), tmp));
            ASSERT(VC == (x1.popFront(&tmp), tmp));
        }

        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

        if (verbose) cout << "Exercising bdlcc::Deque with highWaterMark\n";
        {
            bsl::deque<Element>        q1(&ta);
            const bsl::deque<Element>& Q1 = q1;
            const unsigned int         HIGH_WATER_MARK = 4;

            Obj        x1(Q1.begin(),
                          Q1.end(),
                          HIGH_WATER_MARK,
                          &ta);
            const Obj& X1 = x1;

            Element tmp;

            ASSERT(0 != x1.tryPopFront(&tmp));
            ASSERT(0 != x1.tryPopBack( &tmp));

            ASSERT(0 == x1.tryPushBack(VB));
            ASSERT(0 == x1.tryPushBack(VC));
            ASSERT(0 == x1.tryPushFront(VA));
            x1.forcePushBack(VD);

            ASSERT(0 != x1.tryPushFront(VA));
            ASSERT(0 != x1.tryPushBack( VA));

            ASSERT(HIGH_WATER_MARK == X1.highWaterMark());
            ASSERT(HIGH_WATER_MARK == X1.length());

            x1.forcePushBack(VE);
            x1.forcePushFront(VF);

            ASSERT(6 == X1.length());

            ASSERT(0 != x1.tryPushFront(VA));
            ASSERT(0 != x1.tryPushBack( VA));

            ASSERT(6 == X1.length());

            ASSERT(VF == (x1.popFront(&tmp), tmp));
            ASSERT(VA == (x1.popFront(&tmp), tmp));

            ASSERT(0 == x1.tryPopFront(&tmp));
            ASSERT(VB == tmp);

            ASSERT(0 == x1.tryPopBack(&tmp));
            ASSERT(VE == tmp);

            ASSERT(2 == X1.length());
        }

        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

        if (verbose) cout << "Exercising proctor access\n";
        {
            bsl::deque<Element>        q1(&ta);
            const bsl::deque<Element>& Q1 = q1;
            q1.push_back(VA);
            q1.push_back(VB);
            q1.push_back(VC);

            Obj        x1(Q1.begin(), Q1.end(), &ta);
            const Obj& X1 = x1;

            ASSERT(3 == X1.length());

            {
                Obj::ConstProctor cpr(&X1);
                ASSERT(*cpr == Q1);
                ASSERT(3 == cpr->size());
                ASSERT(VA == (*cpr)[0]);
                ASSERT(VB == (*cpr)[1]);
                ASSERT(VC == (*cpr)[2]);
                ASSERT(VA == cpr[0]);
                ASSERT(VB == cpr[1]);
                ASSERT(VC == cpr[2]);
                ASSERT(VA == cpr->front());
                ASSERT(VC == cpr->back());
                ASSERT(VA == *cpr->begin());
                ASSERT(VC == *(cpr->end() - 1));
                ASSERT(VC == *cpr->rbegin());
                ASSERT(VA == *(cpr->rend() - 1));
            }

            {
                Obj::Proctor pr(&x1);
                ASSERT(*pr == Q1);
                ASSERT(3 == pr->size());
                ASSERT(VA == (*pr)[0]);
                ASSERT(VB == (*pr)[1]);
                ASSERT(VC == (*pr)[2]);
                ASSERT(VA == pr[0]);
                ASSERT(VB == pr[1]);
                ASSERT(VC == pr[2]);
                ASSERT(VA == pr->front());
                ASSERT(VC == pr->back());
                ASSERT(VA == *pr->begin());
                ASSERT(VC == *(pr->end() - 1));
                ASSERT(VC == *pr->rbegin());
                ASSERT(VA == *(pr->rend() - 1));

                using namespace bsl;
                swap((*pr)[0], (*pr)[2]);
                ASSERT(VC == pr->front());
                ASSERT(VB == (*pr)[1]);
                ASSERT(VA == pr->back());

                pr[1] = VA;
                ASSERT(VA == (*pr)[1]);

                pr->front() = VB;
                ASSERT(VB == (*pr)[0]);

                pr->back() = VC;
                ASSERT(VC == pr->back());

                pr->front() = VA;
                swap(pr->front(), pr->back());
            }

            ASSERT(3 == X1.length());

            Element tmp;

            ASSERT(VC == (x1.popFront(&tmp), tmp));
            ASSERT(VA == (x1.popFront(&tmp), tmp));
            ASSERT(VA == (x1.popFront(&tmp), tmp));

            {   // atomic push of three values
                Obj::Proctor pr(&x1);

                ASSERT(pr->empty());

                pr->push_back(VA);
                pr->push_back(VB);
                pr->push_back(VC);
                ASSERT(3 == pr->size());
                ASSERT(! pr->empty());
            }

            ASSERT(3 == X1.length());

            ASSERT(VA == (x1.popFront(&tmp), tmp));
            ASSERT(VB == (x1.popFront(&tmp), tmp));
            ASSERT(VC == (x1.popFront(&tmp), tmp));
        }

        if (verbose) cout << "Various vector pops\n";
        {
            Obj        x1(&ta);
            const Obj& X1 = x1;

            x1.pushBack(VD);
            x1.pushFront(VC);

            bsl::vector<Element> v(&ta);
            v.push_back(VF);

            v.clear();
            x1.tryPopFront(4, &v);

            ASSERT(0  == X1.length());
            ASSERT(2  == v.size());
            ASSERT(VC == v.front());
            ASSERT(VD == v.back());

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            v.clear();
            x1.tryPopFront(2, &v);

            ASSERT(1  == X1.length());
            ASSERT(2  == v.size());
            ASSERT(VA == v.front());
            ASSERT(VB == v.back());

            v.clear();
            x1.removeAll(&v);
            ASSERT(0  == X1.length());
            ASSERT(1  == v.size());
            ASSERT(VC == v.front());

            x1.pushBack(VA);
            x1.pushBack(VB);
            x1.pushBack(VC);

            v.clear();
            x1.tryPopBack(4, &v);

            ASSERT(0  == X1.length());
            ASSERT(3  == v.size());
            ASSERT(VC == v.front());
            ASSERT(VA == v.back());
            ASSERT(VB == v[1]);

            u::loadFromArray(&x1, vArray);

            v.clear();
            x1.tryPopBack(3, &v);

            ASSERT(7  == X1.length());
            ASSERT(3  == v.size());
            ASSERT(VJ == v.front());
            ASSERT(VH == v.back());
            ASSERT(VI == v[1]);

            {
                Obj::ConstProctor proctor(&X1);

                ASSERT(VA == proctor->front());
                ASSERT(VG == proctor->back());
            }

            x1.tryPopBack(1);

            ASSERT(6  == X1.length());

            {
                Obj::ConstProctor proctor(&X1);

                ASSERT(VA == proctor->front());
                ASSERT(VF == proctor->back());
            }

            v.clear();
            x1.tryPopFront(2, &v);

            ASSERT(4  == X1.length());
            ASSERT(2  == v.size());
            ASSERT(VA == v.front());
            ASSERT(VB == v.back());

            {
                Obj::ConstProctor proctor(&X1);

                ASSERT(VC == proctor->front());
                ASSERT(VF == proctor->back());
            }

            x1.tryPopFront(2);

            ASSERT(2  == X1.length());

            {
                Obj::ConstProctor proctor(&X1);

                ASSERT(VE == proctor->front());
                ASSERT(VF == proctor->back());
            }

            x1.removeAll();

            ASSERT(0  == X1.length());
        }

        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());

        if (verbose) cout << "Exercising 'highWaterMark'\n";
        {
            const bsl::size_t HIGH_WATER_MARK = 10;
            Obj               x(&ta);
            Obj               x2(HIGH_WATER_MARK, &ta);

            ASSERT(x.maxSizeT() == x.highWaterMark());
            ASSERT(bsl::numeric_limits<bsl::size_t>::max() ==
                                                            x.highWaterMark());
            ASSERT(HIGH_WATER_MARK == x2.highWaterMark());
        }

        ASSERT(0 == da.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << ".\n";
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
