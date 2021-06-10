// bdlcc_skiplist.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlcc_skiplist.h>

#include <bdlf_bind.h>
#include <bdlt_currenttime.h>
#include <bdlt_intervalconversionutil.h>
#include <bdlt_datetime.h>

#include <bsla_fallthrough.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_assert.h>
#include <bslmt_lockguard.h>
#include <bslmt_barrier.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_once.h>
#include <bslmt_threadattributes.h>
#include <bslmt_testutil.h>
#include <bslmt_threadutil.h>
#include <bslmt_threadgroup.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>
#include <bsltf_streamutil.h>
#include <bsltf_templatetestfacility.h>

#include <bsl_algorithm.h>    // 'min'
#include <bsl_cstdlib.h>
#include <bsl_c_stdlib.h>     // 'rand_r'
#include <bsl_c_ctype.h>
#include <bsl_cmath.h>        // 'floor', 'ceil'
#include <bsl_functional.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

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

#define ASSERT       BSLMT_TESTUTIL_ASSERT
#define ASSERTV      BSLMT_TESTUTIL_ASSERTV

#define Q   BSLMT_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLMT_TESTUTIL_P   // Print identifier and value.
#define P_  BSLMT_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLMT_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLMT_TESTUTIL_L_  // current Line number

#define COUT BSLMT_TESTUTIL_COUT
#define CERR BSLMT_TESTUTIL_CERR

#define V(X)  { if (verbose) P(X); }                 // Print in verbose mode
#define V_(X) { if (verbose) P_(X); }                // Print in verbose mode
#define VV(X)  { if (veryVerbose) P(X); }            // Print in verbose mode
#define VV_(X) { if (veryVerbose) P_(X); }           // Print in verbose mode

#define T2_ (cout << "  ");

#define ASSERT_L(X, L) { \
   if (!(X)) { aSsErT(1, #X, L); }}

#define ASSERT_LL(X, Y, L1, L2) { \
   if ((X)!=(Y)) { \
            aSsErT(1, #X "==" #Y, L2); \
       cout << "(" << L1 << ", " \
            << #X << ": " << X << ", " \
            << #Y << ": " << Y << ") " << endl; \
   }}

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int verbose;
int veryVerbose;
int veryVeryVerbose;
int veryVeryVeryVerbose;

typedef bsltf::TemplateTestFacility TTF;
typedef bsls::Types::Int64          Int64;
typedef bsls::Types::IntPtr         IntPtr;

#define U_RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

    // This macro is 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE' with a
    // couple of problematic types removed.  The removed types were hard to
    // accomodate because 'bdlcc::SkipList' lacks a 'comparator' functor
    // template parameter to override the use of 'operator<'.
    //
    // MISSING:
    //: o 'const char *'
    //: o bsltf::TemplateTestFacility::MethodPtr

#define U_TEST_TYPES_PRIMITIVE                                                \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::FunctionPtr

    // The Windows compiler was complaining that this test driver was too big,
    // so define our own, smaller, 'U_WINDOWS_TEST_TYPES_USER_DEFINED':
    //
    // MISSING:
    //: o bsltf::SimpleTestType
    //: o bsltf::BitwiseCopyableTestType
    //: o bsltf::MovableTestType

#define U_WINDOWS_TEST_TYPES_USER_DEFINED                                     \
    bsltf::EnumeratedTestType::Enum,                                          \
    bsltf::UnionTestType,                                                     \
    bsltf::AllocTestType,                                                     \
    bsltf::BitwiseMoveableTestType,                                           \
    bsltf::AllocBitwiseMoveableTestType,                                      \
    bsltf::MovableAllocTestType,                                              \
    bsltf::NonTypicalOverloadsTestType

#if defined(BSLS_PLATFORM_OS_WINDOWS)
# define U_TEST_TYPES_REGULAR                                                 \
         U_TEST_TYPES_PRIMITIVE,                                              \
         U_WINDOWS_TEST_TYPES_USER_DEFINED
#else
# define U_TEST_TYPES_REGULAR                                                 \
         U_TEST_TYPES_PRIMITIVE,                                              \
         BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_USER_DEFINED
#endif

namespace {
namespace u {

bsls::AtomicInt masterThreadId(0);

enum AddMode {
    e_BEGIN = 0,

    // The following enumeration indicate which 'add*' method is used to add
    // nodes to the container.

    e_ADD = e_BEGIN,                                 // exists
    e_ADD_R,                                         // exists
    e_ADD_HANDLE,                                    // exists
    e_ADD_HANDLE_R,                                  // exists
    e_ADD_RAW,                                       // 0
    e_ADD_RAW_R,                                     // 0
    e_ADD_RAW_PAIR,                                  // exists
    e_ADD_RAW_R_PAIR,                                // exists
    e_ADD_AT_LEVEL_RAW,                              // 0
    e_ADD_AT_LEVEL_RAW_R,                            // 0
    e_ADD_AT_LEVEL_RAW_PAIR,                         // exists
    e_ADD_AT_LEVEL_RAW_R_PAIR,                       // exists
    e_ADD_AT_LEVEL_UNIQUE_RAW,                       // 0
    e_ADD_AT_LEVEL_UNIQUE_RAW_R,                     // 0
    e_ADD_AT_LEVEL_UNIQUE_RAW_PAIR,                  // exists
    e_ADD_AT_LEVEL_UNIQUE_RAW_R_PAIR,                // exists
    e_ADD_UNIQUE,                                    // exists
    e_ADD_UNIQUE_R,                                  // exists
    e_ADD_UNIQUE_HANDLE,                             // exists
    e_ADD_UNIQUE_R_HANDLE,                           // exists
    e_ADD_UNIQUE_RAW,                                // 0
    e_ADD_UNIQUE_RAW_R,                              // 0
    e_ADD_UNIQUE_RAW_PAIR,                           // exists
    e_ADD_UNIQUE_RAW_R_PAIR,                         // exists

    e_END,

    // The following enumeration is to be the last one used in the iteration,
    // it indicates that nodes are added with 'add', and in this case and only
    // in this case, 'removeAll' is called with a vector of pair handles.  This
    // overload will use the default allocator for an internal temp vector.  We
    // expect all previous iterations not to use the default allocator at all.

    e_ADD_WITH_REMOVEALL_HANDLES = e_END,           // no such function

    e_END2 };

BSLMF_ASSERT(e_ADD_WITH_REMOVEALL_HANDLES == e_END2 - 1);

bool isUniqueAdd(AddMode mode)
    // Return 'true' if the specified 'mode' is a unique add.  The behavior is
    // undefined if 'e_ADD_WITH_REMOVEALL_HANDLES' or an invalid value of the
    // enum is specified.
{
    switch (mode) {
      case e_ADD_AT_LEVEL_UNIQUE_RAW_PAIR:    BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_UNIQUE_RAW_R_PAIR:  BSLA_FALLTHROUGH;
      case e_ADD_UNIQUE:                      BSLA_FALLTHROUGH;
      case e_ADD_UNIQUE_R:                    BSLA_FALLTHROUGH;
      case e_ADD_UNIQUE_HANDLE:               BSLA_FALLTHROUGH;
      case e_ADD_UNIQUE_R_HANDLE:             BSLA_FALLTHROUGH;
      case e_ADD_UNIQUE_RAW_PAIR:             BSLA_FALLTHROUGH;
      case e_ADD_UNIQUE_RAW_R_PAIR:           BSLA_FALLTHROUGH;
      case e_ADD_UNIQUE_RAW:                  BSLA_FALLTHROUGH;
      case e_ADD_UNIQUE_RAW_R:                BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_UNIQUE_RAW:         BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_UNIQUE_RAW_R: {
        return true;                                                  // RETURN
      } break;
      case e_ADD:                             BSLA_FALLTHROUGH;
      case e_ADD_R:                           BSLA_FALLTHROUGH;
      case e_ADD_HANDLE:                      BSLA_FALLTHROUGH;
      case e_ADD_HANDLE_R:                    BSLA_FALLTHROUGH;
      case e_ADD_RAW_PAIR:                    BSLA_FALLTHROUGH;
      case e_ADD_RAW_R_PAIR:                  BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_RAW_PAIR:           BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_RAW_R_PAIR:         BSLA_FALLTHROUGH;
      case e_ADD_RAW:                         BSLA_FALLTHROUGH;
      case e_ADD_RAW_R:                       BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_RAW:                BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_RAW_R: {
        return false;                                                 // RETURN
      } break;
      case e_ADD_WITH_REMOVEALL_HANDLES:      BSLA_FALLTHROUGH;
      case e_END2:                            BSLA_FALLTHROUGH;
      default: {
        ASSERTV((int) mode, 0 && "invalid mode");
        BSLS_ASSERT_OPT(    0 && "invalid mode");

        return false;                                                 // RETURN
      } break;
    }
}

bool isLevelAddMode(AddMode mode)
{
    switch (mode) {
      case e_ADD_AT_LEVEL_RAW:                BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_RAW_R:              BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_RAW_PAIR:           BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_RAW_R_PAIR:         BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_UNIQUE_RAW:         BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_UNIQUE_RAW_R:       BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_UNIQUE_RAW_PAIR:    BSLA_FALLTHROUGH;
      case e_ADD_AT_LEVEL_UNIQUE_RAW_R_PAIR: {
        return true;                                                  // RETURN
      } break;
      default: {
        return false;                                                 // RETURN
      }
    }
}

double myAbs(double x)
    // Return the absolute value of the specified 'x'.
{
    return x < 0 ? -x : x;
}

                              // ===============
                              // U_LENGTH, U_LEN
                              // ===============

#define U_LENGTH(array)    static_cast<int>(sizeof(array) / sizeof(array[0]))
#define U_LEN(container)   static_cast<int>(container.size())

                                // ==========
                                // Data, DATA
                                // ==========

// Note that no entries in this table contain values of '5' or below.

const struct Data {
    int         d_line;
    const char *d_spec_p;
    bool        d_unique;
} DATA[] = {
    { L_, "", 1 },
    { L_, "7", 1 },
    { L_, "8", 1 },
    { L_, "78a", 1 },
    { L_, "788a", 0 },
    { L_, "78a78a", 0 },
    { L_, "78abcd", 1 }
};
enum { k_NUM_DATA = U_LENGTH(DATA) };

                                // =============
                                // CountedDelete
                                // =============

class CountedDelete {
    // DATA
    bool                   d_isTemp;
    static bsls::AtomicInt s_deleteCount;

  public:
    // CLASS METHOD
    static int getDeleteCount()
        // Yield the number of times this object has been destroyed.
    {
        return s_deleteCount;
    }

    // CREATORS
    CountedDelete() : d_isTemp(true)
        // Default c'tor
    {}

    CountedDelete(const CountedDelete& original) : d_isTemp(false)
        // Copy c'tor
    {
        (void) original;
    }

    ~CountedDelete()
        // d'tor
    {
        if (!d_isTemp) {
            ++s_deleteCount;
        }
    }

    // MANIPULATORS
    CountedDelete& operator=(const CountedDelete&)
        // Assignment.
    {
        d_isTemp = false;
        return *this;
    }
};

bsls::AtomicInt CountedDelete::s_deleteCount(0);

                                // ==========
                                // TestObject
                                // ==========

template <class TYPE>
class TestObject {
    // The intention of this 'class' is to allow any TTF test type to be able
    // to be easily created with an 'int' value.

    // DATA
    bsls::ObjectBuffer<TYPE>  d_ob;
    bslma_Allocator          *d_allocator_p;

  public:
    // CREATORS
    TestObject(int value, bslma::Allocator *alloc);
    ~TestObject();

    // MANIPULATORS
    TYPE *address();

    TYPE& object();

    // ACCESSORS
    bslma::Allocator *allocator();
};

// CREATORS
template <class TYPE>
TestObject<TYPE>::TestObject(int value, bslma::Allocator *alloc)
{
    BSLS_ASSERT(0 <= value);
    BSLS_ASSERT(value < 128);

    ASSERT(alloc);

    TTF::emplace(d_ob.address(), value, alloc);
    d_allocator_p = alloc;
}

template <class TYPE>
TestObject<TYPE>::~TestObject()
{
    d_ob.address()->~TYPE();
}

// MANIPULATORS
template <class TYPE>
TYPE *TestObject<TYPE>::address()
{
    return d_ob.address();
}

template <class TYPE>
TYPE& TestObject<TYPE>::object()
{
    return d_ob.object();
}

// ACCESSOR
template <class TYPE>
bslma::Allocator *TestObject<TYPE>::allocator()
{
    return d_allocator_p;
}

                                    // ====
                                    // Disp
                                    // ====

class Disp {
    // DATA
    int d_value;

  public:
    // CREATOR
    explicit
    Disp(int value) : d_value(value)
    {
        ASSERT(0 <= value);
        ASSERT(value < 128);
    }

    // ACCESSOR
    int value() const
    {
        return d_value;
    }
};

bsl::ostream& operator<<(bsl::ostream& stream, const Disp& disp)
{
    int value = disp.value();

    if (0 <= value && value < 128 && isprint(value)) {
        stream << '\'' << static_cast<char>(value) << "'==" << value;
    }
    else {
        stream << value;
    }

    return stream;
}

                                // =============
                                // class RandGen
                                // =============

class RandGen {
    // Random number generator using the high-order 32 bits of Donald Knuth's
    // MMIX algorithm.

    bsls::Types::Uint64    d_seed;
    static bsls::AtomicInt s_globalSeed;
  public:
    RandGen();
        // Initialize the generator with the global seed, and modify the global
        // seed.

    explicit
    RandGen(int startSeed);
        // Initialize the generator with the specified 'startSeed'.

    unsigned operator()();
        // Return the next random number in the series;
};

// DATA
bsls::AtomicInt RandGen::s_globalSeed(0);

// CREATOR
inline
RandGen::RandGen()
: d_seed(++s_globalSeed)
{
    (void) (*this)();
    (void) (*this)();
    (void) (*this)();
}

inline
RandGen::RandGen(int startSeed)
: d_seed(startSeed)
{
    (void) (*this)();
    (void) (*this)();
    (void) (*this)();
}
                                                                              \
// MANIPULATOR
inline
unsigned RandGen::operator()()
{
    d_seed = d_seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return static_cast<unsigned>(d_seed >> 32);
}

}  // close namespace u
}  // close unnamed namespace

bsl::ostream& operator<<(bsl::ostream& stream, u::AddMode mode)
{
    const char *pc;

#undef  CASE
#define CASE(id)    case u::id: pc = #id; break

    switch (mode) {
      CASE(e_ADD);
      CASE(e_ADD_R);
      CASE(e_ADD_HANDLE);
      CASE(e_ADD_HANDLE_R);
      CASE(e_ADD_RAW);
      CASE(e_ADD_RAW_R);
      CASE(e_ADD_RAW_PAIR);
      CASE(e_ADD_RAW_R_PAIR);
      CASE(e_ADD_AT_LEVEL_RAW);
      CASE(e_ADD_AT_LEVEL_RAW_R);
      CASE(e_ADD_AT_LEVEL_RAW_PAIR);
      CASE(e_ADD_AT_LEVEL_RAW_R_PAIR);
      CASE(e_ADD_AT_LEVEL_UNIQUE_RAW);
      CASE(e_ADD_AT_LEVEL_UNIQUE_RAW_R);
      CASE(e_ADD_AT_LEVEL_UNIQUE_RAW_PAIR);
      CASE(e_ADD_AT_LEVEL_UNIQUE_RAW_R_PAIR);
      CASE(e_ADD_UNIQUE);
      CASE(e_ADD_UNIQUE_R);
      CASE(e_ADD_UNIQUE_HANDLE);
      CASE(e_ADD_UNIQUE_R_HANDLE);
      CASE(e_ADD_UNIQUE_RAW);
      CASE(e_ADD_UNIQUE_RAW_R);
      CASE(e_ADD_UNIQUE_RAW_PAIR);
      CASE(e_ADD_UNIQUE_RAW_R_PAIR);
      CASE(e_ADD_WITH_REMOVEALL_HANDLES);
      CASE(e_END2);
      default: {
        stream << "Unknown mode: " << static_cast<int>(mode);
        return stream;                                                // RETURN
      }
    }

# undef CASE

    return stream << pc;
}

namespace ValueSemanticTest {

enum LMode { e_LEVELS_MATCH = 0,
             e_LEVELS_RANDOM,
             e_LEVELS_DIFFERENT,
             e_LEVELS_END };

}  // close namespace ValueSemanticTest

bsl::ostream& operator<<(bsl::ostream& stream, ValueSemanticTest::LMode mode)
{
# define CASE(id)    case ValueSemanticTest::e_ ## id: stream << #id;    break

    switch (mode) {
      CASE(LEVELS_MATCH);
      CASE(LEVELS_RANDOM);
      CASE(LEVELS_DIFFERENT);
      CASE(LEVELS_END);
      default: {
        stream << "Invalid 'LMode': " << int(mode);
        ASSERT(0 && "invalid mode");
      }
    }

# undef CASE

    return stream;
}

// ============================================================================
//                             TEST DRIVER CLASS
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bsltf {

// For some reason, Solaris CC wasn't able to find these '<' operators when
// they were in the global namespace.  Linux gcc and Aix xlC_r could handle it
// just fine.

                          // =========================
                          // 'operator<' for TTF types
                          // =========================

bool operator<(const bsltf::UnionTestType& lhs,
               const bsltf::UnionTestType& rhs)
{
    return TTF::getIdentifier(lhs) < TTF::getIdentifier(rhs);
}

bool operator<(const bsltf::SimpleTestType& lhs,
               const bsltf::SimpleTestType& rhs)
{
    return TTF::getIdentifier(lhs) < TTF::getIdentifier(rhs);
}

bool operator<(const bsltf::AllocTestType& lhs,
               const bsltf::AllocTestType& rhs)
{
    return TTF::getIdentifier(lhs) < TTF::getIdentifier(rhs);
}

bool operator<(const bsltf::BitwiseCopyableTestType& lhs,
               const bsltf::BitwiseCopyableTestType& rhs)
{
    return TTF::getIdentifier(lhs) < TTF::getIdentifier(rhs);
}

bool operator<(const bsltf::BitwiseMoveableTestType& lhs,
               const bsltf::BitwiseMoveableTestType& rhs)
{
    return TTF::getIdentifier(lhs) < TTF::getIdentifier(rhs);
}

bool operator<(const bsltf::AllocBitwiseMoveableTestType& lhs,
               const bsltf::AllocBitwiseMoveableTestType& rhs)
{
    return TTF::getIdentifier(lhs) < TTF::getIdentifier(rhs);
}

bool operator<(const bsltf::MovableTestType& lhs,
               const bsltf::MovableTestType& rhs)
{
    return TTF::getIdentifier(lhs) < TTF::getIdentifier(rhs);
}

bool operator<(const bsltf::MovableAllocTestType& lhs,
               const bsltf::MovableAllocTestType& rhs)
{
    return TTF::getIdentifier(lhs) < TTF::getIdentifier(rhs);
}

bool operator<(const bsltf::NonTypicalOverloadsTestType& lhs,
               const bsltf::NonTypicalOverloadsTestType& rhs)
{
    return TTF::getIdentifier(lhs) < TTF::getIdentifier(rhs);
}

// For some reason it was very difficult to get 'operator<' for
// 'TTF::MethodPtr' working, decided it wasn't worth it.

}  // close namespace bsltf
}  // close enterprise namespace

                            // ======================
                            // TestDriver_TableRecord
                            // ======================

struct TestDriver_TableRecord {
    // One record in a table to indicate the integral values of the 'KEY' and
    // 'DATA' components of an element in a 'SkipList'.  Usually these records
    // occur in an array that is passed to the 'populate' and 'verify*' methods
    // of 'TestDriver'.  An array of these records will indicate how a
    // 'SkipList' is to be populated, and then the contents of the 'SkipList'
    // can be checked against the array with the 'verify' and 'verifyR'
    // methods.

    // DATA
    int d_line;
    int d_key;
    int d_data;
    int d_level;

    // ACCESSOR
    bool operator<(const TestDriver_TableRecord& rhs) const
    {
        return d_key < rhs.d_key;
    }
};

                                // ==========
                                // TestDriver
                                // ==========

template <class KEY_TYPE, class DATA_TYPE = KEY_TYPE>
class TestDriver {
    // PRIVATE TYPES
    typedef bdlcc::SkipList<KEY_TYPE, DATA_TYPE> Obj;
    typedef typename Obj::PairHandle             PairHandle;
    typedef typename Obj::Pair                   Pair;
    typedef TestDriver_TableRecord               TableRecord;
    typedef TestDriver                           TD;

    enum { k_OBJ_IS_PRINTABLE = !bsl::is_same<KEY_TYPE,  const char *>::value
                             && !bsl::is_same<DATA_TYPE, const char *>::value,
           k_IS_ALLOCATING = bslma::UsesBslmaAllocator<KEY_TYPE>::value ||
                             bslma::UsesBslmaAllocator<DATA_TYPE>::value,
           k_ADD_BY_MODE_MAX_LEVEL = 8,
           k_CHECK_CONTAINER_VALUE_OFFSET = 10,
                // Difference between 'key' element values and 'data' element
                // values to verify that both aren't being assigned the same
                // value.
           k_KEY_VALUE_MOD = 128 - k_CHECK_CONTAINER_VALUE_OFFSET
    };

    // PRIVATE CLASS METHODS
    static
    int addByMode(Obj              *dst,
                  u::AddMode        addMode,
                  int               keyValue,
                  int               numPreExisting,
                  u::RandGen       *randGen,
                  bool              checkFront,
                  bslma::Allocator *alloc);
        // Call the 'add' method indicated by the specified 'addMode', and do
        // extensive checking that the 'add' succeeded.  The specified 'dst' is
        // the skip list to be added to.  The key object passed to the 'add'
        // function will be initialized to 'keyValue', the data' value will be
        // initialized to 'keyValue + 10'.  Pass the specified 'numPreExisting'
        // the number of items already in the skip list with the same value of
        // 'key'.  If the number of pre existing nodes is unknown (as is the
        // case in a multithreaded test), pass -1.  Pass 'randGen', the random
        // number generator to be used when generating levels to be passed to
        // the add method.  Pass 'alloc' as the allocator to the 'key' and
        // 'data' objects to be passed to the 'add' method.  The specified
        // 'randGen' is to be used for generating levels.  If the specified
        // 'checkFront' is true, if the 'add*' method called returns
        // 'newFrontFlag == true', then verify that the new node is on the
        // front of the list (this should not be done on multithreaded tests).
        // The behavior is undefined unless '!*ph_p' and '!*pr_p' at the
        // beginning of the call.

    static
    void checkContainer(
                  const Obj&  mX,
                  const int (&numNodesForKeyVal)[k_KEY_VALUE_MOD]);
    static
    void checkContainer(
                  const Obj&              mX,
                  const bsls::AtomicInt (&numNodesForKeyVal)[k_KEY_VALUE_MOD]);
        // The specified 'numNodesForKeyVal' has, for each index, the number of
        // nodes that should be in the container with that key value.  Each
        // corresponding data element should have a value of
        // 'keyValue + k_CHECK_CONTAINER_VALUE_OFFSET'.  There should be no
        // elements in the container with values outside the range
        // '[ 0 .. k_KEY_VALUE_MOD )'.  This function traverses the
        // skip list twice, once forward and once backward, verifyhing that
        // right number of nodes with each value are present.

    static
    Obj& hexGg(Obj *dst, const char *spec, int level);
        // Populate the specified 'dst' according to the specified 'spec',
        // which consists of hexadecimal digits with which the 'key' and
        // 'value' of the added nodes are to be set.  If the specified 'level'
        // is non-negative, it is used for the new nodes, if it is negative,
        // the levels for the new nodes are auto-generated.

    static
    bool hexVerify(const Obj & src, const char *spec);
        // Examine the specified 'src', and return 'true' if the values in it
        // match the specified 'spec', where the 'key' value of a node
        // corresponds to the hex value of a character of 'spec', and the
        // corresponding 'data' value has the key value plus one.  Note that
        // for 'true' to be returned, the nodes in 'src' must be in the same
        // order as their corresponding characters in 'spec.

    static
    void populate(Obj               *dst,
                  const TableRecord *array,
                  const bsl::size_t  length);
        // Populate the specified 'dst' with values indicated by the specified
        // 'array' of the specified length 'length'.
        //
        // Note: I attempted to pass 'array' as 'array[N]' where 'N' is an
        // 'int' template parameter, but the Sun CC compiler seems to have a
        // bug where it can't deal with it.  It worked fine on Linux and Aix.

    static
    void dump(const Obj&         list,
              const TableRecord *table = 0,
              bsl::size_t        tableSize = 0);
        // Print the specified 'list', as integer 'key, value' pairs, to
        // 'cout', followed by the contents of 'table' in the same format.  If
        // the specified 'table' is 0, omit printing the table.

    static
    int raiseMaximumLevel(Obj *dst);
        // Add nodes with key value 0 and data value
        // 'k_CHECK_CONTAINER_VALUE_OFFSET' to the specified '*dst' to
        // eventually raise the maximum node level accepted by '*dst' to
        // 'k_ADD_BY_MODE_MAX_LEVEL', and return the number of such nodes added
        // in the process.

    static
    void verify(const Obj&         list,
                const TableRecord *array,
                const bsl::size_t  length,
                const int          line);
        // Traverse the nodes in the specified 'list' from front to back and
        // simultaneously traverse the records in the specified 'array' of
        // specified length 'length' and check with asserts that the nodes in
        // 'list' match those in 'array'.  If an assert fails, print out the
        // specified 'line' as part of the report.

    static
    void verifyR(const Obj&         list,
                 const TableRecord *array,
                 const bsl::size_t  length,
                 const int          line);
        // Traverse the nodes in the specified 'list' from back to front and
        // simultaneously traverse the records in the specified 'array' of
        // specified length 'length' in reverse order and check with asserts
        // that the nodes in 'list' match those in 'array'.  If an assert
        // fails, print out the specified 'line' as part of the report.
        //
        // Note that for a given 'list' and a given 'array', both 'verify' and
        // 'verifyR' should have the same result.

  public:
    // CLASS METHODS

    static void testIndependentAddFunctions();
        // Iterate through all add modes supported by 'addByMode', and for each
        // one, create an 'Obj' and then add values from a table to it using
        // that one add mode and observe the result.

    static void testMultiAddFunctions();
        // Many times, create an 'Obj' and then add a sequence of values to it
        // from a table using a rotating choice of add modes.

                    // Many Threads Random Modes on One SkipList Test

                    // Run many threads all of which add to the same skip list
                    // at the same time.  Keep sums of how many nodes have been
                    // added for every key value in 'numNodesForKeyValue',
                    // which will be used by a call to 'checkContainer' at the
                    // end to verify that the state of the skip list is as we
                    // think it is.

    static void manyThreadsRandomModesOnOneSkipListMain();
        // 'manyThreadsRandomModesOnOneSkipListMain' creates an object of type
        // 'Obj' and spawns subthreads running
        // 'manyThreadsRandomModesOnOneSkipListThread'.

    static void manyThreadsRandomModesOnOneSkipListThread(
                                            Obj             *mX_p,
                                            bsls::AtomicInt *numNodesForKeyVal,
                                            bslmt::Barrier  *barrier);
        // Do many adds calling random 'add' methods to add nodes with random
        // keys to the specified '*mX_p', and keep a tally of how many nodes
        // were added in the specified array 'numNodesForKeyVal' of length
        // 'k_KEY_VALUE_MOD'.  Block on the specified 'barrier' at the
        // beginning.

    static void valueSemanticTest();
        // Test the equality and inequality comparitors, and verify that the
        // 'levels' of nodes are not a salient attribute of the container.

    static void skipTest();
        // Testing 'skipBackward*' and 'skipForward*'.

    static void updateRTest();

    static void addRTest();

    static void removeAllTest();

    static void findTest();

    static void findRTest();

    static void iterationTest();

    static void updateTest();

    static void newFrontTest();

    static void allocationTest();
};

template <class KEY_TYPE, class DATA_TYPE>
int
TestDriver<KEY_TYPE, DATA_TYPE>::addByMode(Obj              *dst,
                                           u::AddMode        addMode,
                                           int               keyValue,
                                           int               numPreExisting,
                                           u::RandGen       *randGen,
                                           bool              checkFront,
                                           bslma::Allocator *alloc)
{
    const bool isUniqueAdd = u::isUniqueAdd(addMode);
    int rc = 0;

    ASSERT(keyValue <= 127 - 10);

    PairHandle  ph;
    Pair       *pr = 0;

    u::TestObject<KEY_TYPE>  tko(keyValue,      alloc);
    u::TestObject<DATA_TYPE> tdo(keyValue + 10, alloc);
    const KEY_TYPE&  key  = tko.object();
    const DATA_TYPE& data = tdo.object();

    const int level = isLevelAddMode(addMode)
                    ? (*randGen)() % (k_ADD_BY_MODE_MAX_LEVEL + 1)
                    : -1;
    bool newFrontFlag = false;

    switch (addMode) {
      case u::e_ADD: {
        dst->add(key, data, &newFrontFlag);
      } break;
      case u::e_ADD_R: {
        dst->addR(key, data, &newFrontFlag);
      } break;
      case u::e_ADD_HANDLE: {
        dst->add(&ph, key, data, &newFrontFlag);
        ASSERT(ph);
      } break;
      case u::e_ADD_HANDLE_R: {
        dst->addR(&ph, key, data, &newFrontFlag);
        ASSERT(ph);
      } break;
      case u::e_ADD_RAW: {
        dst->addRaw(0, key, data, &newFrontFlag);
      } break;
      case u::e_ADD_RAW_R: {
        dst->addRawR(0, key, data, &newFrontFlag);
      } break;
      case u::e_ADD_RAW_PAIR: {
        dst->addRaw(&pr, key, data, &newFrontFlag);
        ASSERT(pr);
      } break;
      case u::e_ADD_RAW_R_PAIR: {
        dst->addRawR(&pr, key, data, &newFrontFlag);
        ASSERT(pr);
      } break;
      case u::e_ADD_AT_LEVEL_RAW: {
        dst->addAtLevelRaw(0, level, key, data, &newFrontFlag);
      } break;
      case u::e_ADD_AT_LEVEL_RAW_R: {
        dst->addAtLevelRawR(0, level, key, data, &newFrontFlag);
      } break;
      case u::e_ADD_AT_LEVEL_RAW_PAIR: {
        dst->addAtLevelRaw(&pr, level, key, data, &newFrontFlag);
        ASSERT(pr);
      } break;
      case u::e_ADD_AT_LEVEL_RAW_R_PAIR: {
        dst->addAtLevelRawR(&pr, level, key, data, &newFrontFlag);
        ASSERT(pr);
      } break;
      case u::e_ADD_AT_LEVEL_UNIQUE_RAW: {
        rc = dst->addAtLevelUniqueRaw(0, level, key, data, &newFrontFlag);
      } break;
      case u::e_ADD_AT_LEVEL_UNIQUE_RAW_R: {
        rc = dst->addAtLevelUniqueRawR(0, level, key, data, &newFrontFlag);
      } break;
      case u::e_ADD_AT_LEVEL_UNIQUE_RAW_PAIR: {
        rc = dst->addAtLevelUniqueRaw(&pr, level, key, data, &newFrontFlag);
        ASSERT(!pr == !!rc);
      } break;
      case u::e_ADD_AT_LEVEL_UNIQUE_RAW_R_PAIR: {
        rc = dst->addAtLevelUniqueRawR(&pr, level, key, data, &newFrontFlag);
        ASSERT(!pr == !!rc);
      } break;
      case u::e_ADD_UNIQUE: {
        rc = dst->addUnique(key, data, &newFrontFlag);
      } break;
      case u::e_ADD_UNIQUE_R: {
        rc = dst->addUniqueR(key, data, &newFrontFlag);
      } break;
      case u::e_ADD_UNIQUE_HANDLE: {
        rc = dst->addUnique(&ph, key, data, &newFrontFlag);
        ASSERT(!!ph == !rc);
      } break;
      case u::e_ADD_UNIQUE_R_HANDLE: {
        rc = dst->addUniqueR(&ph, key, data, &newFrontFlag);
        ASSERT(!!ph == !rc);
      } break;
      case u::e_ADD_UNIQUE_RAW: {
        rc = dst->addUniqueRaw(0, key, data, &newFrontFlag);
      } break;
      case u::e_ADD_UNIQUE_RAW_R: {
        rc = dst->addUniqueRawR(0, key, data, &newFrontFlag);
      } break;
      case u::e_ADD_UNIQUE_RAW_PAIR: {
        rc = dst->addUniqueRaw(&pr, key, data, &newFrontFlag);
        ASSERT(!pr == !!rc);
      } break;
      case u::e_ADD_UNIQUE_RAW_R_PAIR: {
        rc = dst->addUniqueRawR(&pr, key, data, &newFrontFlag);
        ASSERT(!pr == !!rc);
      } break;
      default: {
        P(addMode);
        BSLS_ASSERT(0);
      } break;
    }

    if (0 == rc) {
        ASSERT(!!ph + !!pr <= 1);
        ASSERT(!!ph == ph.isValid());

        if (0 <= level && (ph || pr)) {
            typedef bdlcc::SkipList_Node<KEY_TYPE, DATA_TYPE> Node;

            const Pair *pr2 = pr ? pr : static_cast<const Pair *>(ph);
            ASSERT(pr2);
            const Node *node = reinterpret_cast<const Node *>(pr2);

            ASSERTV(node->level(), level, node->level() == level);
        }

        if (ph) {
            ASSERT(ph.key()  == key);
            ASSERT(ph.data() == data);

            if (checkFront) {
                dst->skipBackward(&ph);
                ASSERTV(!ph, newFrontFlag, keyValue, !ph == newFrontFlag);
            }

            if (ph) {
                ph.release();
            }
        }

        if (pr) {
            ASSERT(pr->key()  == key);
            ASSERT(pr->data() == data);

            if (checkFront) {
                dst->skipBackwardRaw(&pr);
                ASSERTV(!pr, newFrontFlag, keyValue, !pr == newFrontFlag);
            }

            if (pr) {
                dst->releaseReferenceRaw(pr);
            }
        }
    }
    else {
        ASSERT(0 != numPreExisting && isUniqueAdd);
        ASSERT(!ph);
        ASSERT(!pr);
    }

    const int addThisTime = !isUniqueAdd || 0 == numPreExisting;
    const int expected    = numPreExisting + addThisTime;

    int rc2 = dst->findLowerBound(&ph, key);
    for (int ii = 0; ii < expected; ++ii) {
        ASSERT(0 == rc2);
        ASSERT(ph);
        ASSERTV(ii, expected, ph.key()  == key);
        ASSERTV(ii, expected, ph.data() == data);
        rc2 = dst->skipForward(&ph);
    }

    if (0 <= numPreExisting && ph.isValid()) {
        ASSERTV(ph.key(), key, expected, ph.key()  != key);
        ASSERT(ph.data() != data);
    }

    return rc;
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::checkContainer(
                   const Obj&  mX,
                   const int (&numNodesForKeyVal)[k_KEY_VALUE_MOD])
{
    PairHandle ph;

    bslma::TestAllocator va("va", veryVeryVeryVerbose);

    int rc = mX.front(&ph);

    for (int keyVal = 0; keyVal < k_KEY_VALUE_MOD; ++keyVal)  {
        u::TestObject<KEY_TYPE>  kto(keyVal,      &va);
        u::TestObject<DATA_TYPE> dto(keyVal + 10, &va);
        const KEY_TYPE&  key  = kto.object();
        const DATA_TYPE& data = dto.object();

        const int numNodes = numNodesForKeyVal[keyVal];

        for (int ii = 0; ii < numNodes; ++ii) {
            ASSERT(0 == rc);
            ASSERT(ph);
            ASSERTV(ii, numNodes, ph.key()  == key);
            ASSERTV(ii, numNodes, ph.data() == data);
            rc = mX.skipForward(&ph);
        }

        if (ph) {
            ASSERTV(numNodes, ph.key()  != key);
            ASSERTV(numNodes, ph.data() != data);
        }
    }

    ASSERT(!ph);

    rc = mX.back(&ph);

    for (int keyVal = k_KEY_VALUE_MOD; 0 < keyVal--; )  {
        u::TestObject<KEY_TYPE>  kto(keyVal,      &va);
        u::TestObject<DATA_TYPE> dto(keyVal + 10, &va);
        const KEY_TYPE&  key  = kto.object();
        const DATA_TYPE& data = dto.object();

        const int numNodes = numNodesForKeyVal[keyVal];

        for (int ii = 0; ii < numNodes; ++ii) {
            ASSERT(0 == rc);
            ASSERT(ph);
            ASSERTV(ii, numNodes, ph.key()  == key);
            ASSERTV(ii, numNodes, ph.data() == data);
            rc = mX.skipBackward(&ph);
        }

        if (ph) {
            ASSERTV(numNodes, ph.key()  != key);
            ASSERTV(numNodes, ph.data() != data);
        }
    }

    ASSERT(!ph);
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::checkContainer(
                   const Obj&              mX,
                   const bsls::AtomicInt (&numNodesForKeyVal)[k_KEY_VALUE_MOD])
{
    int intNumNodesForKeyVal[k_KEY_VALUE_MOD];
    int *ret = bsl::copy(numNodesForKeyVal + 0,
                         numNodesForKeyVal + k_KEY_VALUE_MOD,
                         intNumNodesForKeyVal + 0);
    ASSERT(intNumNodesForKeyVal + k_KEY_VALUE_MOD == ret);

    TD::checkContainer(mX, intNumNodesForKeyVal);
}

template <class KEY_TYPE, class DATA_TYPE>
bdlcc::SkipList<KEY_TYPE, DATA_TYPE>&
TestDriver<KEY_TYPE, DATA_TYPE>::hexGg(Obj        *dst,
                                       const char *spec,
                                       int         level)
{
    for (const char *pc = spec; *pc; ++pc) {
        int id = *pc;
        int keyId  = '0' <= id && id <= '9'
                   ? id - '0'
                   : 'a' <= id && id <= 'f'
                   ? 10 + (id - 'a')
                   : 'A' <= id && id <= 'F'
                   ? 10 + (id - 'A')
                   : 126;
        int dataId = keyId + 1;

        ASSERTV(dataId <= 15);

        u::TestObject<KEY_TYPE>  key( keyId,  dst->allocator());
        u::TestObject<DATA_TYPE> data(dataId, dst->allocator());

        Pair *pr = 0;

        if (-1 == level) {
            dst->addRaw(&pr, key.object(), data.object());
        }
        else {
            dst->addAtLevelRaw(&pr, level, key.object(), data.object());
        }
        ASSERTV(spec, pc - spec, pr);
        dst->releaseReferenceRaw(pr);
    }

    return *dst;
}

template <class KEY_TYPE, class DATA_TYPE>
bool TestDriver<KEY_TYPE, DATA_TYPE>::hexVerify(const Obj&  src,
                                                const char *spec)
{
    bsl::string specCopy(spec, src.allocator());

    if (static_cast<int>(specCopy.size()) != src.length()) {
        return false;                                                 // RETURN
    }

    char c = 0;
    for (char *pc = specCopy.empty() ? &c : &specCopy[0]; *pc; ++pc) {
        if ('a' <= *pc && *pc <= 'f') {
            *pc = static_cast<char>(*pc + 'A' - 'a');
        }
    }
    bsl::sort(specCopy.begin(), specCopy.end());

    PairHandle ph;
    src.front(&ph);
    const char *pc = specCopy.c_str();
    for (; ph.isValid() && *pc; src.skipForward(&ph), ++pc) {
        int id = *pc;
        int keyId  = '0' <= id && id <= '9'
                   ? id - '0'
                   : 'A' <= id && id <= 'F'
                   ? 10 + (id - 'A')
                   : 126;
        int dataId = keyId + 1;

        ASSERTV(dataId <= 15);

        u::TestObject<KEY_TYPE>  key( keyId,  src.allocator());
        u::TestObject<DATA_TYPE> data(dataId, src.allocator());

        if (ph.key() != key.object() || ph.data() != data.object()) {
            return false;                                             // RETURN
        }
    }

    return !ph.isValid() && !*pc;
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::populate(Obj               *dst,
                                               const TableRecord *array,
                                               const bsl::size_t  length)
{
    PairHandle ph;

    for (unsigned ii = 0; ii < length; ++ii) {
        u::TestObject<KEY_TYPE>  key( array[ii].d_key,  dst->allocator());
        u::TestObject<DATA_TYPE> data(array[ii].d_data, dst->allocator());

        dst->add(&ph, key.object(), data.object());
        ASSERTV(ii, ph);
    }
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::dump(const Obj&         list,
                                           const TableRecord *table,
                                           const bsl::size_t  tableSize)
{
    PairHandle ph;

    bool first = true;
    int rc = 0;
    for (list.front(&ph); 0 == rc && ph; rc = list.skipForward(&ph)) {
        bsl::cout << (first ? "list:  " : ", ") << '(' <<
                                        TTF::getIdentifier(ph.key()) << ", " <<
                                        TTF::getIdentifier(ph.data()) << ')';
        first = false;
    }
    ASSERT(0 == rc);
    ASSERT(!ph);

    bsl::cout << bsl::endl;

    if (!table) {
        return;                                                       // RETURN
    }

    first = true;
    for (unsigned ii = 0; ii < tableSize; ++ii) {
        bsl::cout << (first ? "table: " : ", ") << '(' << table[ii].d_key <<
                                               ", " << table[ii].d_data << ')';
        first = false;
    }

    bsl::cout << bsl::endl;
}

template <class KEY_TYPE, class DATA_TYPE>
int TestDriver<KEY_TYPE, DATA_TYPE>::raiseMaximumLevel(Obj *dst)
{
    // The skiplist maintains a maximum node level, which starts out at
    // zero and increments by one each time an attempt is made to add a
    // node at a higher level than the maximum node level.  'addByMode'
    // attempts to add at levels up to level 8, and when it does an
    // add at level, expects to get the level it added at.  So attempt a
    // bunch of add at levels at 8 to get the maxium node level up to the
    // point where 'addByMode' will get the level it asks for.

    typedef bdlcc::SkipList_Node<KEY_TYPE, DATA_TYPE> Node;

    bslma::TestAllocator va;

    u::TestObject<KEY_TYPE>  kto( 0, &va);
    u::TestObject<DATA_TYPE> dto(10, &va);

    int nodeLevel;
    const Node *node;
    int ret = 0;
    do {
        Pair *pr;

        dst->addAtLevelRaw(&pr,
                           k_ADD_BY_MODE_MAX_LEVEL,
                           kto.object(),
                           dto.object());
        ++ret;

        node = reinterpret_cast<const Node *>(pr);
        nodeLevel = node->level();
        dst->releaseReferenceRaw(pr);
    } while (nodeLevel < k_ADD_BY_MODE_MAX_LEVEL);

    return ret;
}


template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::verify(const Obj  &       list,
                                             const TableRecord *array,
                                             const bsl::size_t  uLength,
                                             const int          line)
{
    const int length = static_cast<int>(uLength);

    const int startStatus = testStatus;

    const char *kn = bsls::NameOf<KEY_TYPE>().name();
    const char *dn = bsls::NameOf<DATA_TYPE>().name();

    ASSERTV(line, length, list.length(), length == list.length());
    const int iterations = bsl::min(length, list.length());;

    Pair       *pr;
    PairHandle  skPh;
    PairHandle  nePh;

    list.frontRaw(&pr);
    list.front(&skPh);
    list.front(&nePh);

    int handleMisses = 0;

    for (int ii = 0; ii < iterations; ++ii) {
        const TableRecord&   record = array[ii];

        const int            LINE = record.d_line;
        const int            key  = record.d_key;
        const int            data = record.d_data;

        u::TestObject<KEY_TYPE>  KEY_O( key,  list.allocator());
        KEY_TYPE&                KEY  = KEY_O.object();
        u::TestObject<DATA_TYPE> DATA_O(data, list.allocator());
        DATA_TYPE&               DATA = DATA_O.object();

        typedef u::Disp D;

        ASSERTV(line, LINE, kn, D(key),  pr->key(),  KEY  == pr->key());
        ASSERTV(line, LINE, dn, D(data), pr->data(), DATA == pr->data());

        handleMisses += KEY  != skPh.key();
        handleMisses += DATA != skPh.data();

        handleMisses += KEY  != nePh.key();
        handleMisses += DATA != nePh.data();

        int rcPr = list.skipForwardRaw(&pr);
        int rcSk = list.skipForward(&skPh);
        int rcNe = list.next(&nePh, nePh);

        ASSERT(0 == rcPr);
        ASSERT(0 == rcSk);
        ASSERT(nePh);

        if (ii < iterations - 1) {
            ASSERTV(line, LINE, pr);
            ASSERTV(line, LINE, skPh);
            ASSERTV(line, LINE, rcNe, ii, list.length(), 0 == rcNe);
        }
        else if (iterations == list.length()) {
            ASSERTV(line, LINE, !pr);
            ASSERTV(line, LINE, !skPh);
            ASSERTV(line, LINE, rcNe, ii, list.length(), 0 != rcNe);
        }
    }

    if (startStatus == testStatus) {
        ASSERT(0 == handleMisses);
    }

    if (verbose && startStatus < testStatus) {
        bsl::cout << "verify failed:\n";
        dump(list, array, uLength);
    }
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::verifyR(const Obj&         list,
                                              const TableRecord *array,
                                              const bsl::size_t  uLength,
                                              const int          line)
{
    const int length = static_cast<int>(uLength);

    const int startStatus = testStatus;

    const char *kn = bsls::NameOf<KEY_TYPE>().name();
    const char *dn = bsls::NameOf<DATA_TYPE>().name();

    ASSERTV(line, length, list.length(), length == list.length());
    const int iterations = bsl::min(length, list.length());;
    const int end        = length - iterations;

    Pair       *pr;
    PairHandle  skPh, pvPh;

    list.backRaw(&pr);
    list.back(&skPh);
    list.back(&pvPh);

    int handleMisses = 0;

    for (int ii = length - 1; end <= ii; --ii) {
        const TableRecord&   record = array[ii];

        const int            LINE = record.d_line;
        const int            key  = record.d_key;
        const int            data = record.d_data;

        u::TestObject<KEY_TYPE>  KEY_O( key,  list.allocator());
        KEY_TYPE&                KEY  = KEY_O.object();
        u::TestObject<DATA_TYPE> DATA_O(data, list.allocator());
        DATA_TYPE&               DATA = DATA_O.object();

        typedef u::Disp D;

        ASSERTV(line, LINE, kn, D(key),  pr->key(),  KEY  == pr->key());
        ASSERTV(line, LINE, dn, D(data), pr->data(), DATA == pr->data());


        handleMisses += KEY  != skPh.key();
        handleMisses += DATA != skPh.data();

        handleMisses += KEY  != pvPh.key();
        handleMisses += DATA != pvPh.data();

        // If we are at the head of the list, 'skipBackward' will return 0 but
        // reset 'skPh'.  'previous' will set 'pvPh' to the head node, but
        // return non-zero.

        int rcPr = list.skipBackwardRaw(&pr);
        int rcSk = list.skipBackward(&skPh);
        int rcPv = list.previous(&pvPh, pvPh);

        ASSERTV(line, LINE, 0 == rcPr);
        ASSERTV(line, LINE, rcSk, ii, length, list.length(), 0 == rcSk);
        ASSERTV(line, LINE, pvPh);

        if (end < ii) {
            ASSERTV(line, LINE, pr);
            ASSERTV(line, LINE, skPh);
            ASSERTV(line, LINE, rcPv, ii, length, list.length(), 0 == rcPv);
        }
        else if (iterations == list.length()) {
            ASSERTV(line, LINE, !pr);
            ASSERTV(line, LINE, end, !skPh);
            ASSERTV(line, LINE, end, rcPv, ii, length, 0 != rcPv);
        }
    }

    if (startStatus == testStatus) {
        ASSERT(0 == handleMisses);
    }

    if (verbose && startStatus < testStatus) {
        bsl::cout << "verifyR failed:\n";
        dump(list, array, uLength);
    }
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::testIndependentAddFunctions()
{
    if (verbose) cout << "testIndependentAddFunctions<" <<
                                   bsls::NameOf<KEY_TYPE>() << ", " <<
                                   bsls::NameOf<DATA_TYPE>() << ">();" << endl;

    u::RandGen randGen;

    for (int ai = u::e_BEGIN; ai < u::e_END; ++ai) {
        u::AddMode addMode = static_cast<u::AddMode>(ai);

        if (veryVerbose) { T2_;    P(addMode); }

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
        bslma::TestAllocator va("va", veryVeryVeryVerbose);
        bslma::TestAllocator da("da", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        const bool isUnique = u::isUniqueAdd(addMode);

        int numNodesForKeyVal[k_KEY_VALUE_MOD] = { 0 };

        static
        int keys[] = { 8, 10, 20, 15, 14, 16, 4, 25, 20, 17, 25, 8, 20, 18,
                      23, 25, 21, 3, 11, 33, 20, 31, 4, 33, 16, 19, 17, 12,
                       8, 9, 10, 40, 16, 25, 26, 2, 40, 8, 9, 11, 10, 12 };
        enum { k_NUM_KEYS = sizeof keys / sizeof *keys };

        Obj mX(&ta);

        if (u::isLevelAddMode(addMode)) {
            const int numZeroesToRaise = raiseMaximumLevel(&mX);
            if (veryVeryVerbose) P(numZeroesToRaise);
            mX.removeAll();
        }

        for (int ti = 0; ti < k_NUM_KEYS; ++ti) {
            const int keyVal = keys[ti];

            if (veryVeryVerbose) P(keyVal);

            int& numNodes = numNodesForKeyVal[keyVal];
            int rc = TD::addByMode(&mX,
                                   addMode,
                                   keyVal,
                                   numNodes,
                                   &randGen,
                                   true,
                                   &va);
            ASSERT((0 != rc) == (isUnique && 0 < numNodes));
            numNodes += 0 == rc;

            ASSERT(0 == va.numBlocksInUse());
        }

        TD::checkContainer(mX, numNodesForKeyVal);
        ASSERT(0 == da.numBlocksTotal());
    }
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::testMultiAddFunctions()
{
    if (verbose) cout << "testMultiAddFunctions<" <<
                                   bsls::NameOf<KEY_TYPE>() << ", " <<
                                   bsls::NameOf<DATA_TYPE>() << ">();" << endl;

    u::RandGen randGen;

    BSLMF_ASSERT(u::e_BEGIN == 0);

    enum { k_MODE_MOD = u::e_END - u::e_BEGIN };

    for (int startIdx = u::e_BEGIN; startIdx < u::e_END; ++startIdx) {
        const u::AddMode startMode = static_cast<u::AddMode>(startIdx);

        if (veryVerbose) { T2_;    P(startMode); }

        bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
        bslma::TestAllocator va("va", veryVeryVeryVerbose);
        bslma::TestAllocator da("da", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        int numNodesForKeyVal[k_KEY_VALUE_MOD] = { 0 };

        static
        int keys[] = { 8, 10, 20, 15, 14, 16, 4, 25, 20, 17, 25, 8, 20, 18,
                      23, 25, 21, 3, 11, 33, 20, 31, 4, 33, 16, 19, 17, 12,
                       8, 9, 10, 40, 16, 25, 26, 2, 40, 8, 9, 11, 10, 12 };
        enum { k_NUM_KEYS = sizeof keys / sizeof *keys };

        Obj mX(&ta);

        const int numZeroesToRaise = raiseMaximumLevel(&mX);
        if (veryVeryVerbose) P(numZeroesToRaise);
        mX.removeAll();

        int ai = startIdx;
        for (int ti = 0; ti < k_NUM_KEYS; ++ti, ai = (ai + 1) % k_MODE_MOD) {
            const int        keyVal  = keys[ti];
            const u::AddMode addMode = static_cast<u::AddMode>(ai);

            const bool isUnique = u::isUniqueAdd(addMode);

            if (veryVeryVerbose) P(keyVal);

            int& numNodes = numNodesForKeyVal[keyVal];
            int rc = TD::addByMode(&mX,
                                   addMode,
                                   keyVal,
                                   numNodes,
                                   &randGen,
                                   true,
                                   &va);
            ASSERT((0 != rc) == (isUnique && 0 < numNodes));
            numNodes += 0 == rc;

            ASSERT(0 == va.numBlocksInUse());
        }

        TD::checkContainer(mX, numNodesForKeyVal);
        ASSERT(0 == da.numBlocksTotal());
    }
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::manyThreadsRandomModesOnOneSkipListMain()
{
    // ------------------------------------------------------------------------
    // THOROUGH MULTI-THREADED ADD TEST
    //
    // Concern:
    //: 1 Need to test doing adds and carefully examining the state of the
    //:   container after the add is completed, for all 'add' functions under
    //:   multithreaded conditions.
    //
    // Plan:
    //: 1 Use the function 'addByMode' which will call one of the 'add*'
    //:   functions to attempt to add a node to the container, and then
    //:   exhaustively check out the state of the container after the 'add'.
    //:
    //: 2 Spawn many threads with differently seeded random number generators
    //:   to driver 'addByMode'.
    //:
    //: 3 Have all the threads attempt to add nodes to the container, and keep
    //:   track of how many nodes with each key value were successfully added
    //:   in an array of 'bsls::AtomicInt's.
    //:
    //: 4 After the threads have been joined, call 'checkContainer' to verify
    //:   that exactly the number of nodes for each key value expected were
    //:   present.
    //..
    // Main Function:      manyThreadsRandomModesOnOneSkipListMain
    // Subthread Function: manyThreadsRandomModesOnOneSkipListThread
    ///..
    // ------------------------------------------------------------------------

    if (verbose) COUT << "manyThreadsRandomModesOnOneSkipListMain<" <<
                                   bsls::NameOf<KEY_TYPE>() <<
                           ", " << bsls::NameOf<DATA_TYPE>() << ">();" << endl;


    bslma::TestAllocator ta;

    Obj mX(&ta);
    bsls::AtomicInt numNodesForKeyVal[k_KEY_VALUE_MOD];

    const int numZeroesToRaise = raiseMaximumLevel(&mX);
    if (veryVerbose) P(numZeroesToRaise);
    mX.removeAll();

#if defined(BSLS_PLATFORM_OS_AIX)
    // Aix runs this test much slower than other platforms.

    const int numThreads = 80;
#else
    const int numThreads = 175;
#endif
    bslmt::ThreadGroup tg(&ta);
    bslmt::Barrier barrier(numThreads);

    tg.addThreads(bdlf::BindUtil::bind(
                                    &manyThreadsRandomModesOnOneSkipListThread,
                                    &mX,
                                    numNodesForKeyVal + 0,
                                    &barrier),
                  numThreads);
    tg.joinAll();

    TD::checkContainer(mX, numNodesForKeyVal);
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::
                             manyThreadsRandomModesOnOneSkipListThread(
                                           Obj              *mX_p,
                                           bsls::AtomicInt  *numNodesForKeyVal,
                                           bslmt::Barrier   *barrier)
{
    int threadId = u::masterThreadId++;

    if (veryVeryVerbose) COUT <<
                                "manyThreadsRandomModesOnOneSkipListThread<" <<
                                     bsls::NameOf<KEY_TYPE>()  << ", " <<
                                     bsls::NameOf<DATA_TYPE>() << ">(): id " <<
                                                             threadId << ";\n";

    bslma::TestAllocator va("va", veryVeryVerbose);

    u::RandGen randGen(threadId);

    barrier->wait();

    for (int ii = 0; ii < 100; ++ii) {
        const int keyVal = randGen() % k_KEY_VALUE_MOD;

        u::AddMode addMode = static_cast<u::AddMode>(randGen() % u::e_END);

        int rc = TD::addByMode(mX_p,
                               addMode,
                               keyVal,
                               -1,
                               &randGen,
                               false,
                               &va);
        ASSERT(0 == rc || u::isUniqueAdd(addMode));
        numNodesForKeyVal[keyVal] += 0 == rc;

        ASSERT(0 == va.numBlocksInUse());
    }
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::valueSemanticTest()
// ----------------------------------------------------------------------------
// Value-semantic tests
//
// Concerns:
//: 1 The levels of the nodes in a container have no influence on the semantic
//:   value of the container.
//:
//: 2 The equality and inequality operators reflect the semantic value of the
//:   container.
//
// Plan:
//: 1 Have an 'enum' type, 'LMode', specifying 3 modes of testing
//:   o levels match
//:
//:   o levels different
//:
//:   o levels random
//:
//: 2 Iterate two nested loops, each through all rows in the 'DATA' table and
//:   all values of 'LMode'.
//:
//: 3 Use the 'hexGg' function to create containers mX and mY, one for the
//:   specification and level arg of each loop.
//:
//: 4 Compare the two containers with '==' and '!=' and observe that they match
//:   when and only when they are both constructed from the same row of 'DATA'.
//:
//: 5 Copy construct container 'mZ' from 'X' and observe the two compare equal.
//:
//: 6 Assign 'Y' to 'mZ' and observe that now THOSE two containers compare
//:   equal.
//:
//: 7 Add a couple of nodes to 'mY' and observe that 'Y' and 'Z' no longer
//:   match.
//:
//: 8 Remove the new nodes from 'mY' and observe that 'Y' and 'Z' match again.
// ----------------------------------------------------------------------------
{
    if (verbose) cout << "valueSemanticTest<" << bsls::NameOf<KEY_TYPE>() <<
                           ", " << bsls::NameOf<DATA_TYPE>() << ">();" << endl;

    namespace TC = ValueSemanticTest;

    bslma::TestAllocator va("va", veryVeryVeryVerbose);
    bslma::TestAllocator ta("ta", veryVeryVeryVerbose);
    bslma::TestAllocator da("da", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    enum { k_NUM_I_ITERATIONS = u::k_NUM_DATA * TC::e_LEVELS_END };

    u::RandGen randGen;     // Constructed differently from global seed each
                            // time.

    enum { k_NUM_VALS = 10 };
    bsls::ObjectBuffer<KEY_TYPE>   obk[k_NUM_VALS];
    KEY_TYPE                      *kvs = obk[0].address();
    const KEY_TYPE                *KVS = kvs;
    bsls::ObjectBuffer<DATA_TYPE>  obd[k_NUM_VALS];
    DATA_TYPE                     *dvs = obd[0].address();
    const DATA_TYPE               *DVS = dvs;

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        TTF::emplace(obk[ii].address(), ii, &va);
        TTF::emplace(obd[ii].address(), ii, &va);
    }

    Int64 expDaAllocations = 0;
    for (int ti = 0; ti < k_NUM_I_ITERATIONS; ++ti) {
        const TC::LMode    LMODE  = static_cast<TC::LMode>(ti / u::k_NUM_DATA);
        const int          TI     =                        ti % u::k_NUM_DATA;
        const u::Data&     iData  = u::DATA[TI];
        const char        *ISPEC  = iData.d_spec_p;
        const IntPtr       ILEN   = bsl::strlen(ISPEC);
        const int          ILEVEL = TC::e_LEVELS_RANDOM == LMODE
                                  ? -1
                                  : (randGen() % 16) & ~1;

        ASSERT(LMODE < TC::e_LEVELS_END);
        ASSERT((TC::e_LEVELS_RANDOM == LMODE) == (ILEVEL < 0));

        Obj mX(&ta);    const Obj& X = TD::hexGg(&mX, ISPEC, ILEVEL);
        ASSERT(&mX == &X);

        ASSERT(ILEN == X.length());
        ASSERT((0 == ILEN) == X.isEmpty());

        for (int tj = 0; tj < u::k_NUM_DATA; ++tj) {
            const u::Data&     jData  = u::DATA[tj];
            const char        *JSPEC  = jData.d_spec_p;
            const IntPtr       JLEN   = bsl::strlen(JSPEC);
            const int          JLEVEL = TC::e_LEVELS_RANDOM == LMODE
                                      ? -1
                                      : TC::e_LEVELS_DIFFERENT == LMODE
                                      ? (randGen() % 16) | 1
                                      : ILEVEL;    // e_LEVELS_MATCH

            if ((veryVerbose && 0 == tj) || veryVeryVerbose) {
                P_(LMODE);    P_(ILEVEL);    P(JLEVEL);
            }

            ASSERT(TC::e_LEVELS_DIFFERENT != LMODE || ILEVEL != JLEVEL);
            ASSERT(TC::e_LEVELS_MATCH     != LMODE ||
                                            (ILEVEL == JLEVEL && 0 <= ILEVEL));
            ASSERTV((TC::e_LEVELS_RANDOM == LMODE) == (JLEVEL < 0));
            ASSERT((ILEVEL < 0) == (JLEVEL < 0));

            Obj mY(&ta);    const Obj& Y = mY;

            // Add a node to 'mY' and delete it so that if both containers are
            // randomizing levels, their respective random number generators
            // will be out of sync.

            {
                PairHandle ph3;
                mY.add(&ph3, KVS[3], DVS[4]);
                mY.remove(ph3);
            }

            ASSERT(&Y == &TD::hexGg(&mY, JSPEC, JLEVEL));

            ASSERT(JLEN == Y.length());
            ASSERT((0 == JLEN) == Y.isEmpty());

            ASSERTV(LMODE, X, Y, (TI == tj) == (X == Y));
            ASSERT((TI == tj) != (X != Y));
            ASSERT((TI == tj) == (Y == X));
            ASSERT((TI == tj) != (Y != X));

            Obj mZ(X, &ta);    const Obj& Z = mZ;
            expDaAllocations += !X.isEmpty();

            ASSERT(TD::hexVerify(X, ISPEC));
            ASSERT(TD::hexVerify(Z, ISPEC));

            ASSERT(Z == X);
            ASSERT(!(Z != X));
            ASSERT((TI == tj) == (Z == Y));
            ASSERT((TI == tj) != (Z != Y));

            mZ = Y;
            expDaAllocations += !Y.isEmpty();

            ASSERT(TD::hexVerify(Z, JSPEC));
            ASSERT(TD::hexVerify(Y, JSPEC));

            ASSERT(Z == Y);
            ASSERT(!(Z != Y));
            ASSERT(Y == Z);
            ASSERT(!(Y != Z));
            ASSERT((TI == tj) == (Z == X));
            ASSERT((TI == tj) != (Z != X));
            ASSERT((TI == tj) == (X == Z));
            ASSERT((TI == tj) != (X != Z));

            PairHandle ph5;
            mY.add(&ph5, KVS[5], DVS[6]);

            ASSERT(Z != Y);
            ASSERT(!(Z == Y));
            ASSERT(Y != Z);
            ASSERT(!(Y == Z));

            ASSERT(X != Y);
            ASSERT(!(X == Y));
            ASSERT(Y != X);
            ASSERT(!(Y == X));

            ASSERT(Y.length() == Z.length() + 1);

            PairHandle ph4;
            mY.add(&ph4, KVS[4], DVS[5]);

            ASSERT(Z != Y);
            ASSERT(!(Z == Y));
            ASSERT(Y != Z);
            ASSERT(!(Y == Z));

            ASSERT(X != Y);
            ASSERT(!(X == Y));
            ASSERT(Y != X);
            ASSERT(!(Y == X));

            ASSERT(Y.length() == Z.length() + 2);

            ASSERT(0 == mY.remove(ph5));
            ASSERT(0 == mY.remove(ph4));

            ASSERT(Z == Y);
            ASSERT(Y == Z);
            ASSERT(!(Z != Y));
            ASSERT(!(Y != Z));

            ASSERT((TI == tj) == (X == Y));
            ASSERT((TI == tj) != (X != Y));
            ASSERT((TI == tj) == (Y == X));
            ASSERT((TI == tj) != (Y != X));

            mZ.removeAll();

            ASSERT(Z.isEmpty());
            ASSERT(0 == Z.length());
            ASSERT(Y.isEmpty() == (Z == Y));
            ASSERT(TD::hexVerify(Z, ""));
        }
    }

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        obk[ii].address()->~KEY_TYPE();
        obd[ii].address()->~DATA_TYPE();
    }

    ASSERTV(da.numAllocations(), expDaAllocations,
                                      da.numAllocations() == expDaAllocations);
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::skipTest()
// ----------------------------------------------------------------------------
// Testing 'skipBackward*' and 'skipForward*'
//
// Test appropriate behavior of 'skipBackward' and 'skipForward' and
// their "raw" variants in a single-threaded environment.
//
// Concerns:
//: 1 skipping returns 0 if the item is in the container and 'e_NOT_FOUND'
//:   otherwise.
//:
//: 2 skipping off the front/end resets the handle (as appropriate) and returns
//:   0.
//:
//: 3 reference counting is correct
// ----------------------------------------------------------------------------
{
    if (verbose) cout << "skipTest<" << bsls::NameOf<KEY_TYPE>() <<
                           ", " << bsls::NameOf<DATA_TYPE>() << ">();" << endl;

    bslma::TestAllocator va("va", veryVeryVerbose);
    bslma::TestAllocator ta("ta", veryVeryVerbose);
    bslma::TestAllocator da("da", veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    Obj mX(&ta);    const Obj& X = mX;

    enum { k_NUM_VALS = 10 };
    bsls::ObjectBuffer<KEY_TYPE>   obk[k_NUM_VALS];
    KEY_TYPE                      *kvs = obk[0].address();
    const KEY_TYPE                *KVS = kvs;
    bsls::ObjectBuffer<DATA_TYPE>  obd[k_NUM_VALS];
    DATA_TYPE                     *dvs = obd[0].address();
    const DATA_TYPE               *DVS = dvs;

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        TTF::emplace(obk[ii].address(), ii,      &va);
        TTF::emplace(obd[ii].address(), ii + 10, &va);
    }

    PairHandle  ph[7];
    Pair       *pr[7] = { 0 };

    for (int ii = 1; ii <= 6; ++ii) {
        mX.add(&ph[ii], KVS[ii], DVS[ii]);
        ASSERT(0 == X.findRaw(&pr[ii], KVS[ii]));
        ASSERT(pr[ii] == ph[ii]);
    }

    ASSERT(0 == X.skipForward(&ph[2]));
    ASSERT(ph[2].key()  == KVS[3]);
    ASSERT(ph[2].data() == DVS[3]);

    ASSERT(0 == X.skipBackward(&ph[2]));
    ASSERT(ph[2].key()  == KVS[2]);
    ASSERT(ph[2].data() == DVS[2]);

    ASSERT(0 == X.skipForwardRaw(&pr[5]));
    ASSERT(pr[5]->key()  == KVS[6]);
    ASSERT(pr[5]->data() == DVS[6]);

    ASSERT(0 == X.skipBackwardRaw(&pr[5]));
    ASSERT(pr[5]->key()  == KVS[5]);
    ASSERT(pr[5]->data() == DVS[5]);

    // 4 cases of skipping off the end.

    ASSERT(0 == X.skipBackward(&ph[1]));
    ASSERT(!ph[1].isValid());

    ASSERT(0 == X.skipBackwardRaw(&pr[1]));
    ASSERT(0 == pr[1]);

    ASSERT(0 == X.skipForward(&ph[6]));
    ASSERT(!ph[6].isValid());

    ASSERT(0 == X.skipForwardRaw(&pr[6]));
    ASSERT(0 == pr[6]);

    // Delete a node in the middle, then try skipping backward and forward
    // from it.

    ASSERT(6 == X.length());
    mX.remove(ph[3]);
    ASSERT(5 == X.length());
    ASSERT(ph[3].isValid());

    ASSERT(!ph[0].isValid());
    ASSERT(0 != X.find(&ph[0], KVS[3]));
    ASSERT(!ph[0].isValid());

    for (int ii = 0; ii < 4; ++ii) {
        switch (ii) {
          case 0: {
            ASSERT(Obj::e_NOT_FOUND == X.skipBackward(&ph[3]));
          } break;
          case 1: {
            ASSERT(Obj::e_NOT_FOUND == X.skipForward( &ph[3]));
          } break;
          case 2: {
            ASSERT(Obj::e_NOT_FOUND == X.skipBackwardRaw(&pr[3]));
          } break;
          case 3: {
            ASSERT(Obj::e_NOT_FOUND == X.skipForwardRaw( &pr[3]));
          } break;
          default: {
            BSLS_ASSERT(0);
          }
        }

        // Always, nothing changed.

        ASSERT(pr[3]);
        ASSERT(ph[3].isValid());
        ASSERT(pr[3] == ph[3]);
        ASSERT(ph[3].key()  == KVS[3]);
        ASSERT(ph[3].data() == DVS[3]);
        ASSERT(pr[3]->key()  == KVS[3]);
        ASSERT(pr[3]->data() == DVS[3]);
    }

    const Int64 numBlocks = ta.numBlocksInUse();

    for (int ii = 1; ii <= 6; ++ii) {
        const bool exp = 1 != ii && 6 != ii;
        ASSERT(exp == !!pr[ii]);
        ASSERT(exp == ph[ii].isValid());
        if (exp) {
            mX.releaseReferenceRaw(pr[ii]);
            ph[ii].release();
        }
    }

    // The above loop should have released all references to node 3, which
    // is no longer in the list, which should have resulted in deallocations if
    // 'KEY_TYPE' or 'DATA_TYPE' do allocations.  Note that the node itself
    // will be reclaimed by the skip list's pool, which will not in itself
    // result in any deallocation.

    ASSERT(k_IS_ALLOCATING == (ta.numBlocksInUse() < numBlocks));

    ASSERT(5 == X.length());

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        obk[ii].address()->~KEY_TYPE();
        obd[ii].address()->~DATA_TYPE();
    }

    ASSERT(0 == da.numAllocations());
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::updateRTest()
// ----------------------------------------------------------------------------
// UPDATER TEST
//
// Concerns:
//: 1 Update on an item that's been removed returns e_NOT_FOUND.
//:
//: 2 Update to an existing position returns e_DUPLICATE if allowDuplicates is
//:   false.
//:
//: 3 Update updates the key value stored on the node.
//:
//: 4 After an update, the data can be looked up by its new value but not by
//:   its old value.
//:
//: 5 Update to an existing position succeeds if allowDuplicates is true.
//
// Plan:
//: 1 Update an item to a new location and verify C-2 through C-4
//:
//: 2 Check C-1 and C-5.
//
// Note that We want to ensure that we move both *to* and *from* the beginning,
// middle, and end of the test.
//
// Note that an important feature of the "parameters" set is that
// executing it returns the list to its original state.
// ---------------------------------------------------------------------------
{
    if (verbose) cout << "updateRTest<" << bsls::NameOf<KEY_TYPE>() <<
                           ", " << bsls::NameOf<DATA_TYPE>() << ">();" << endl;

    bslma::TestAllocator va(veryVeryVerbose);
    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    Obj mX(&ta);    const Obj& X = mX;

    PairHandle phs[4];

    int ret;

    enum { k_NUM_VALS = 20 };
    bsls::ObjectBuffer<KEY_TYPE>   obk[k_NUM_VALS];
    KEY_TYPE                      *kvs = obk[0].address();
    const KEY_TYPE                *KVS = kvs;
    bsls::ObjectBuffer<DATA_TYPE>  obd[k_NUM_VALS];
    DATA_TYPE                     *dvs = obd[0].address();
    const DATA_TYPE               *DVS = dvs;

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        TTF::emplace(obk[ii].address(), ii, &va);
        TTF::emplace(obd[ii].address(), ii, &va);
    }

    mX.addR(&phs[0], KVS[1], DVS[2]);
    mX.addR(&phs[1], KVS[2], DVS[3]);
    mX.addR(&phs[2], KVS[3], DVS[4]);
    mX.addR(&phs[3], KVS[4], DVS[5]);

    TableRecord check[] = {
        { L_, 1, 2, 0 },
        { L_, 2, 3, 0 },
        { L_, 3, 4, 0 },
        { L_, 4, 5, 0 },
    };

    verify(X, check, U_LENGTH(check), L_);

    ret = mX.updateR(phs[3], KVS[4], 0, false);    // no change
    ASSERT(0 == ret);

    verify(X, check, U_LENGTH(check), L_);

    ret = mX.updateR(phs[3], KVS[1], 0, false);    // collision
    ASSERT(Obj::e_DUPLICATE == ret);

    verify(X, check, U_LENGTH(check), L_);
    verifyR(X, check, U_LENGTH(check), L_);

    bool newFront;
    ASSERT(0 == mX.updateR(phs[1], KVS[12], &newFront));
    ASSERT(!newFront);
    ASSERT(0 == mX.updateR(phs[0], KVS[13], &newFront));
    ASSERT(!newFront);
    ASSERT(0 == mX.updateR(phs[2], KVS[11], &newFront));
    ASSERT(!newFront);
    ASSERT(0 == mX.updateR(phs[3], KVS[10], &newFront));
    ASSERT(newFront);

    TableRecord checkB[] = {
        { L_, 10, 5, 0 },
        { L_, 11, 4, 0 },
        { L_, 12, 3, 0 },
        { L_, 13, 2, 0 },
    };

    verify( X, checkB, U_LENGTH(checkB), L_);
    verifyR(X, checkB, U_LENGTH(checkB), L_);

    const Pair *pr = phs[0];
    ASSERT(0 == mX.remove(pr));
    ASSERT(Obj::e_NOT_FOUND == mX.updateR(phs[0], KVS[1]));
    ASSERT(0 == mX.updateR(phs[3], KVS[11]));
    ASSERT(Obj::e_DUPLICATE == mX.updateR(phs[1], KVS[11], &newFront, false));

    TableRecord checkC[] = {
        { L_, 11, 4, 0 },
        { L_, 11, 5, 0 },
        { L_, 12, 3, 0 },
    };

    verify( X, checkC, U_LENGTH(checkC), L_);
    verifyR(X, checkC, U_LENGTH(checkC), L_);

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        obk[ii].address()->~KEY_TYPE();
        obd[ii].address()->~DATA_TYPE();
    }

    ASSERT(0 == da.numAllocations());
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::addRTest()
// ----------------------------------------------------------------------------
// ADDR TEST
//
// Concern:
//: 1 'addR' adds a node to the container.
//:
//: 2 Nodes with redundant keys added via 'addR' add the new node after
//:   previously present nodes with the same key.
//:
//: 3 Nodes with redundant keys added via 'addR' add the new node before
//:   previously present nodes with the same key.
//
// Plan:
//: 1 Add numerous non-redundant nodes via 'addR' and confirm that the
//:   contents of the container are as expected.
//:
//: 2 Add a node with a redundant 'key' value to the node on the front of the
//:   list (but with a different 'data' value) using 'addR' and observe, via
//:   the 'newFront' flag and by skipping backward from the returned pair
//:   handle, that the new node was positioned immediately after the
//:   pre-existing node with the same key.
//:
//: 3 Add a node with a redundant 'key' value to the node on the front of the
//:   list (but with a different 'data' value) using 'add' (not 'addR') and
//:   observe, via the 'newFront' flag and by skipping forward from the
//:   returned pair handle, that the new node was positioned immediately before
//:   all pre-existing nodes with the same key.
//:
//: 4 Observe that the default allocator was never used.
// ----------------------------------------------------------------------------
{
    if (verbose) cout << "addRTest<" << bsls::NameOf<KEY_TYPE>() <<
                           ", " << bsls::NameOf<DATA_TYPE>() << ">();" << endl;

    bslma::TestAllocator va(veryVeryVerbose);
    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    enum { k_NUM_VALS = 30 };
    bsls::ObjectBuffer<KEY_TYPE>   obk[k_NUM_VALS];
    KEY_TYPE                      *kvs = obk[0].address();
    const KEY_TYPE                *KVS = kvs;
    bsls::ObjectBuffer<DATA_TYPE>  obd[k_NUM_VALS];
    DATA_TYPE                     *dvs = obd[0].address();
    const DATA_TYPE               *DVS = dvs;

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        TTF::emplace(obk[ii].address(), ii, &va);
        TTF::emplace(obd[ii].address(), ii, &va);
    }

    {
        Obj mX(&ta);    const Obj& X = mX;

        ASSERT(X.isEmpty());

        PairHandle ph, phB;
        mX.addR(&ph, KVS[2], DVS[10]);

        ASSERT(ph);
        ASSERT(KVS[2] == ph.key());
        ASSERT(DVS[10] == ph.data());
        ASSERT(1 == X.length());
        ASSERT(!X.isEmpty());

        ph.release();
        ASSERT(!ph);

        ASSERT(!phB);
        ASSERT(0 == mX.popFront(&phB));

        ASSERT(phB);
        ASSERT(KVS[2] == phB.key());
        ASSERT(DVS[10] == phB.data());
        ASSERT(0 == X.length());
        ASSERT(X.isEmpty());

        phB.release();
        ASSERT(!phB);
    }

    {
        TableRecord check[] = {
            { L_, 1, 2, 0 },
            { L_, 2, 3, 0 },
            { L_, 3, 4, 0 },
            { L_, 4, 5, 0 },
        };

        Obj mX(&ta);    const Obj& X = mX;

        ASSERT(X.isEmpty());

        mX.addR(KVS[3], DVS[4]);
        mX.addR(KVS[2], DVS[3]);
        mX.addR(KVS[1], DVS[2]);
        mX.addR(KVS[4], DVS[5]);

        ASSERT(!X.isEmpty());
        ASSERT(4 == X.length());

        verify(X, check, U_LENGTH(check), L_);

        PairHandle ph;
        int ret;
        ret = mX.popFront(&ph);
        ASSERT(ret == 0);
        ASSERT(ph);

        ASSERT(KVS[1] == ph.key());
        ASSERT(DVS[2] == ph.data());

        ret = mX.popFront(&ph);
        ASSERT(ret == 0);
        ASSERT(ph);

        ASSERT(KVS[2] == ph.key());
        ASSERT(DVS[3] == ph.data());

        ret = mX.popFront(&ph);
        ASSERT(ret == 0);
        ASSERT(ph);

        ASSERT(KVS[3] == ph.key());
        ASSERT(DVS[4] == ph.data());

        ASSERT(1 == X.length());

        ret = mX.popFront(&ph);
        ASSERT(ret == 0);
        ASSERT(ph);

        ASSERT(X.isEmpty());

        ASSERT(KVS[4] == ph.key());
        ASSERT(DVS[5] == ph.data());

        ret = mX.popFront(&ph);
        ASSERT(Obj::e_NOT_FOUND == ret);
    }

    TableRecord init[] = {
        { L_ , 5, 6, 0 },
        { L_ , 3, 4, 0 },
        { L_ , 6, 7, 0 },
        { L_ , 4, 5, 0 },
        { L_ , 8, 9, 0 },
    };
    const int LEN = U_LENGTH(init);

    bsl::vector<TableRecord> initCheckVec(init + 0, init + LEN, &va);
    bsl::sort(initCheckVec.begin(), initCheckVec.end());
    TableRecord *initCheck = initCheckVec.data();

    {
        Obj mX(&ta);    const Obj& X = mX;

        ASSERT(X.isEmpty());

        TD::populate(&mX, init, LEN);
        verify(X, initCheck, LEN, L_);

        bool newFront = false;

        mX.addR(KVS[20], DVS[21], &newFront);
        ASSERT(X.length() == LEN + 1);
        ASSERT(!newFront);

        mX.addR(KVS[1], DVS[2], &newFront);
        ASSERT(X.length() == LEN + 2);
        ASSERT(newFront);

        PairHandle ph;

        mX.addR(&ph, KVS[7], DVS[8], &newFront);
        ASSERT(X.length() == LEN + 3);
        ASSERT(!newFront);
        ASSERT(ph.key()  == KVS[7]);
        ASSERT(ph.data() == DVS[8]);

        mX.addR(&ph, KVS[0], DVS[1], &newFront);
        ASSERT(X.length() == LEN + 4);
        ASSERT(newFront);
        ASSERT(ph.key()  == KVS[0]);
        ASSERT(ph.data() == DVS[1]);

        // 'addR' of redundant node adds after existing nodes with the same
        // value.

        mX.addR(&ph, KVS[0], DVS[6], &newFront);
        ASSERT(X.length() == LEN + 5);
        ASSERT(!newFront);
        ASSERT(ph.key()  == KVS[0]);
        ASSERT(ph.data() == DVS[6]);

        ASSERT(0 == X.skipBackward(&ph));
        ASSERT(ph);
        ASSERT(ph.key()  == KVS[0]);
        ASSERT(ph.data() == DVS[1]);

        // 'add' (not 'addR') of redundant node adds before existing nodes with
        // the same value.

        mX.add(&ph, KVS[0], DVS[10], &newFront);
        ASSERT(X.length() == LEN + 6);
        ASSERT(newFront);
        ASSERT(ph.key()  == KVS[0]);
        ASSERT(ph.data() == DVS[10]);

        ASSERT(0 == X.skipForward(&ph));
        ASSERT(ph);
        ASSERT(ph.key()  == KVS[0]);
        ASSERT(ph.data() == DVS[1]);

        ASSERT(0 == X.skipForward(&ph));
        ASSERT(ph);
        ASSERT(ph.key()  == KVS[0]);
        ASSERT(ph.data() == DVS[6]);
    }

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        obk[ii].address()->~KEY_TYPE();
        obd[ii].address()->~DATA_TYPE();
    }

    ASSERT(0 == da.numAllocations());
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::removeAllTest()
// ----------------------------------------------------------------------------
// REMOVEALL TEST
//
// Concerns:
//: 1 That the function 'removeAll' empties the container.
//:
//: 2 If a vector is passed to 'removeAll', the container is emptied but the
//:   vector is populated with pair handles referring to all the deleted nodes
//:   from the container.
//
// Plan:
//: 1 Create a container and populate it from the non-sorted table 'VALUES1',
//:
//: 2 Use the function 'verify' to traverse the container and compare it with
//:   table 'VALUES2', which has the same data as 'VALUES1', only sorted.
//:
//: 3 Call 'removeAll' with no args, and verify that it empties the container.
//:
//: 4 Re-populate and check the conainter again as in steps '1' and '2'.
//:
//: 5 Create a vector of pair handles and put one pair handle in it.
//:
//: 6 Call 'removeAll' passing it the vector, and observe that the container
//:   is emptied.
//:
//: 7 Examine the contents of the vector and observe that it has the handle
//:   that was originally there, with the other handles from the container
//:   appended to that, in the correct order.
//:
//: 8 Don't check the default allocator -- 'removeAll' uses it.
// ---------------------------------------------------------------------------
{
    if (verbose) cout << "removeAllTest<" << bsls::NameOf<KEY_TYPE>() <<
                           ", " << bsls::NameOf<DATA_TYPE>() << ">();" << endl;

    TableRecord VALUES1[] = {
        { L_ , 1, 2, 0},
        { L_ , 3, 4, 0},
        { L_ , 0, 1, 0},
        { L_ , 2, 3, 0},
        { L_ , 4, 5, 0},
    };

    TableRecord VALUES2[] = {
        { L_ , 0, 1, 0},
        { L_ , 1, 2, 0},
        { L_ , 2, 3, 0},
        { L_ , 3, 4, 0},
        { L_ , 4, 5, 0},
    };

    const int LEN = U_LENGTH(VALUES1);
    ASSERT(U_LENGTH(VALUES2) == LEN);

    // Note that 'removeAll' to a vector first gathers a temporary vector of
    // 'Pair *'s which it then copies to a vector of 'PairHandle's.

    bslma::TestAllocator va(veryVeryVerbose);
    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    {
        Obj mX(&ta);    const Obj& X = mX;

        TD::populate(&mX, VALUES1, LEN);
        verify(X, VALUES2, LEN, L_);
        ASSERT(X.length() == LEN);

        int numRemoved = mX.removeAll();
        ASSERT(X.isEmpty());
        ASSERT(0 == X.length());
        ASSERT(LEN == numRemoved);

        TD::populate(&mX, VALUES1, LEN);
        verify(X, VALUES2, LEN, L_);
        ASSERT(X.length() == LEN);

        ASSERT(0 == da.numAllocations());

        bsl::vector<PairHandle> removed(&ta);

        PairHandle ph;
        ASSERT(0 == mX.front(&ph));
        ASSERT(ph);
        removed.push_back(ph);

        numRemoved = mX.removeAll(&removed);    // Appends handles of removed
                                                // elements to container.
        ASSERT(X.isEmpty());
        ASSERT(LEN == numRemoved);
        ASSERT(LEN + 1 == static_cast<int>(removed.size()));

        ASSERT(TTF::getIdentifier(removed[0].key())  == VALUES2[0].d_key);
        ASSERT(TTF::getIdentifier(removed[0].data()) == VALUES2[0].d_data);

        for (int ii = 0; ii < LEN; ++ii) {
            PairHandle ph = removed[ii + 1];

            ASSERT(TTF::getIdentifier(ph.key())  == VALUES2[ii].d_key);
            ASSERT(TTF::getIdentifier(ph.data()) == VALUES2[ii].d_data);
        }

        if (k_OBJ_IS_PRINTABLE) {
            if (veryVerbose) {
                X.print(cout);
            } else if (verbose) {
                X.print(cout, 0, -1) << endl;
            }
        }

        ASSERT(0 <  da.numAllocations());
    }

    ASSERT(0 <  ta.numAllocations());
    ASSERT(0 == ta.numBytesInUse());
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::findTest()
// ----------------------------------------------------------------------------
// FIND TEST
//
// Concerns:
//: 1 That the return value of 'find' correctly indicates whether an element
//:   with a given 'key' is in the list.
//:
//: 2 If duplicate matches for 'key' exist in the list, 'find' returns the
//:   first one.
//
// Plan:
//: 1 Create a container and populate it from the non-sorted table 'init',
//:
//: 2 Use the function 'verify' to traverse the container and compare it with
//:   table 'initCheck', which has the same data as 'init', only sorted.
//:
//: 3 Do several calls to 'find' on the containers, with keys that are present
//:   and some that are not, and observe that the return values are correct.
//:
//: 4 Remove the element found by one of the 'find's, using its pair handle.
//:
//: 5 Call 'verify' on the container with table 'deletedCheck' to verify that
//:   the deleted node is no longer in the container.
//:
//: 6 Examine the 'key' and 'data' values of the deleted node still held by the
//:   pair handle to show that the node is not released yet.
//:
//: 7 Release the pair handle.
//:
//: 8 Start over, create another list where the 'data' element increments with
//:   every node, but the 'key' element increases only every three nodes, so
//:   there are duplicates.
//:
//: 9 Call 'find' on every key value in the list, and observe that:
//:   o the 'data' value of the node found indicates that it's the first on the
//:     list with that 'key' value'.
//:
//:   o skip backward, and observe that the 'key' value of the new node does
//:     not match.
//:
//: 10 Observe that the default allocator was never used.
// ---------------------------------------------------------------------------
{
    const char *keyType  = bsls::NameOf<KEY_TYPE>();
    const char *dataType = bsls::NameOf<DATA_TYPE>();

    if (verbose) cout << "findTest<" << keyType <<
                                            ", " << dataType << ">();" << endl;

    bslma::TestAllocator va(veryVeryVerbose);
    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    enum { k_NUM_VALS = 20 };
    bsls::ObjectBuffer<KEY_TYPE>   obk[k_NUM_VALS];
    KEY_TYPE                      *kvs = obk[0].address();
    const KEY_TYPE                *KVS = kvs;
    bsls::ObjectBuffer<DATA_TYPE>  obd[k_NUM_VALS];
    DATA_TYPE                     *dvs = obd[0].address();
    const DATA_TYPE               *DVS = dvs;

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        TTF::emplace(obk[ii].address(), ii, &va);
        TTF::emplace(obd[ii].address(), ii, &va);
    }

    {
        TableRecord init[] = {
            { L_ , 1, 2, 0},
            { L_ , 5, 6, 0},
            { L_ , 3, 4, 0},
            { L_ , 2, 3, 0},
            { L_ , 4, 5, 0},
        };

        TableRecord initCheck[] = {
            { L_ , 1, 2, 0},
            { L_ , 2, 3, 0},
            { L_ , 3, 4, 0},
            { L_ , 4, 5, 0},
            { L_ , 5, 6, 0},
        };

        const int LEN = U_LENGTH(init);
        ASSERT(LEN == U_LENGTH(initCheck));

        TableRecord deletedCheck[] = {
            { L_ , 1, 2, 0},
            { L_ , 3, 4, 0},
            { L_ , 4, 5, 0},
            { L_ , 5, 6, 0},
        };

        {
            Obj mX(&ta);    const Obj& X = mX;

            TD::populate(&mX, init, LEN);
            verify(    X, initCheck, LEN, L_);

            PairHandle ph, phB;

            ASSERT(0 != X.find(&ph, KVS[0]));
            ASSERT(0 != X.find(&ph, KVS[10]));
            ASSERT(0 == X.find(&ph, KVS[2]));

            mX.remove(ph);

            verify(X, deletedCheck, U_LENGTH(deletedCheck), L_);

            ASSERT(0 != X.find(&phB, KVS[0]));
            ASSERT(0 != X.find(&phB, KVS[10]));
            ASSERT(0 != X.find(&phB, KVS[2]));

            ASSERT(ph.key()  == KVS[2]);
            ASSERT(ph.data() == DVS[3]);

            ph.release();

            verify(X, deletedCheck, U_LENGTH(deletedCheck), L_);

            if (k_OBJ_IS_PRINTABLE) {
                if (veryVerbose) {
                    if (veryVeryVerbose) {
                        X.print(cout);
                    } else {
                        X.print(cout, 0, -1) << endl;
                    }
                }
            }
        }
    }

    {
        u::RandGen randGen;

        for (int len = 0; len < 30; ++len) {
            int numNodesForKey[k_NUM_VALS] = { 0 };

            Obj mX(&ta);    const Obj& X = mX;

            for (int ii = 0; ii < len; ++ii) {
                int key  = randGen() % k_NUM_VALS;
                int data = (key + 1) % k_NUM_VALS;

                mX.addRaw(0, KVS[key], DVS[data]);
                ++numNodesForKey[key];
            }
            ASSERT(X.length() == len);

            for (int key = 0; key < k_NUM_VALS; ++key) {
                int data = (key + 1) % k_NUM_VALS;

                PairHandle ph;
                const bool found = (0 == X.find(&ph, KVS[key]));
                ASSERT(found == !!ph);
                ASSERT(found == !!numNodesForKey[key]);
                if (found) {
                    ASSERT(KVS[key]  == ph.key());
                    ASSERT(DVS[data] == ph.data());
                }
            }
        }
    }

    {
        bsl::vector<TableRecord> init(&ta);

        Obj mX(&ta);    const Obj& X = mX;

        for (int ii = 0; ii < 12; ++ii) {
            mX.addR(KVS[ii / 3], DVS[ii]);

            TableRecord tr = { L_, ii / 3, ii, 0 };
            init.push_back(tr);
        }

        TD::verify( X, &init[0], init.size(), L_);
        TD::verifyR(X, &init[0], init.size(), L_);

        if (verbose) TD::dump(X, &init[0], init.size());

        PairHandle ph;

        for (int ii = 0; ii < 4; ++ii) {
            ASSERT(0 == X.find(&ph, KVS[ii]));
            ASSERTV(keyType, dataType, TTF::getIdentifier(ph.data()), ii,
                                                     ph.data() == DVS[3 * ii]);
            ASSERT(0 == X.skipBackward(&ph));
            ASSERT(!ph == (0 == ii));
            if (ph) {
                ASSERT(ph.key() == KVS[ii - 1]);
                ASSERTV(keyType, dataType, TTF::getIdentifier(ph.data()), ii,
                                                 ph.data() == DVS[3 * ii - 1]);
            }
        }
    }

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        obk[ii].address()->~KEY_TYPE();
        obd[ii].address()->~DATA_TYPE();
    }

    ASSERT(0 == da.numAllocations());
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::findRTest()
// ----------------------------------------------------------------------------
// FINDR TEST
//
// Concerns:
//: 1 That the return value of 'findR' correctly indicates whether an element
//:   with a given 'key' is in the list.
//:
//: 2 If duplicate matches for 'key' exist in the list, 'find' returns the
//:   last one.
//
// Plan:
//: 1 Create a container and populate it from the non-sorted table 'init',
//:
//: 2 Use the function 'verify' to traverse the container and compare it with
//:   table 'initCheck', which has the same data as 'init', only sorted.
//:
//: 3 Do several calls to 'findR' on the containers, with keys that are present
//:   and some that are not, and observe that the return values are correct.
//:
//: 4 Remove the element found by one of the 'findR's, using its pair handle.
//:
//: 5 Erase the row in 'initCheck' corresponding to the removed element, and
//:   call 'verify' on the container with table 'initCheck' to verify that the
//:   deleted node is no longer in the container.
//:
//: 6 Examine the 'key' and 'data' values of the deleted node still held by the
//:   pair handle to show that the node is not released yet.
//:
//: 7 Observe that the default allocator was never used.
//:
//: 8 Start over, create another list where the 'data' element increments with
//:   every node, but the 'key' element increases only every three nodes, so
//:   there are duplicates.
//:
//: 9 Call 'findR' on every key value in the list, and observe that:
//:   o the 'data' value of the node found indicates that it's the last on the
//:     list with that 'key' value'.
//:
//:   o skip forward, and observe that the 'key' value of the new node does not
//:     match.
//:
//: 10 Observe that the default allocator was never used.
// ---------------------------------------------------------------------------
{
    const char *keyType  = bsls::NameOf<KEY_TYPE>();
    const char *dataType = bsls::NameOf<DATA_TYPE>();

    if (verbose) cout << "findRTest<" << keyType <<
                                            ", " << dataType << ">();" << endl;


    bslma::TestAllocator va(veryVeryVerbose);
    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    Obj mX(&ta);    const Obj& X = mX;

    enum { k_NUM_VALS = 12 };
    bsls::ObjectBuffer<KEY_TYPE>   obk[k_NUM_VALS];
    KEY_TYPE                      *kvs = obk[0].address();
    const KEY_TYPE                *KVS = kvs;
    bsls::ObjectBuffer<DATA_TYPE>  obd[k_NUM_VALS];
    DATA_TYPE                     *dvs = obd[0].address();
    const DATA_TYPE               *DVS = dvs;

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        TTF::emplace(obk[ii].address(), ii, &va);
        TTF::emplace(obd[ii].address(), ii, &va);
    }

    {
        static TableRecord init[] = {
            { L_, 2, 3, 0 },
            { L_, 5, 6, 0 },
            { L_, 4, 5, 0 },
            { L_, 1, 2, 0 },
            { L_, 3, 4, 0 },
            { L_, 0, 1, 0 }
        };
        bsl::vector<TableRecord> initCheck(init + 0,
                                           init + U_LENGTH(init),
                                           &ta);

        TD::populate(&mX, init, U_LENGTH(init));
        bsl::sort(&initCheck[0], &initCheck[0] + U_LEN(initCheck));
        verify(X, &initCheck[0], U_LEN(initCheck), L_);

        PairHandle ph;
        ASSERT(0 != X.findR(&ph, KVS[10]));
        ASSERT(!ph);

        ASSERT(0 == X.findR(&ph, KVS[0]));
        ASSERT(ph);

        mX.remove(ph);
        ASSERT(DVS[1] == ph.data());

        initCheck.erase(initCheck.begin());

        verify(X, &initCheck[0], U_LEN(initCheck), L_);

        ASSERT(0 != X.findR(&ph, KVS[0]));
        ASSERT(ph);
        ASSERT(KVS[0] == ph.key());
        ASSERT(DVS[1] == ph.data());

        ASSERT(0 == X.findR(&ph, KVS[2]));
        ASSERT(ph);
        ASSERT(KVS[2] == ph.key());
        ASSERT(DVS[3] == ph.data());

        mX.remove(ph);
        ASSERT(KVS[2] == ph.key());
        ASSERT(DVS[3] == ph.data());

        initCheck.erase(initCheck.begin() + 1);

        verify(X, &initCheck[0], U_LEN(initCheck), L_);

        ASSERT(0 == X.findR(&ph, KVS[1]));
        ASSERT(ph);
        ASSERT(KVS[1] == ph.key());
        ASSERT(DVS[2] == ph.data());

        ASSERT(0 == mX.update(ph, KVS[4], 0, true));
        ASSERT(ph);
        ASSERT(KVS[4] == ph.key());
        ASSERT(DVS[2] == ph.data());

        static TableRecord check2[] = {
            { L_, 3, 4, 0 },
            { L_, 4, 2, 0 },
            { L_, 4, 5, 0 },
            { L_, 5, 6, 0 }
        };

        verify(X, &check2[0], 4, L_);

        ASSERT(0 == X.findR(&ph, KVS[4]));
        ASSERT(ph);
        ASSERT(KVS[4] == ph.key());
        ASSERT(DVS[5] == ph.data());

        mX.removeAll();

        verify  (X,   init, 0, L_);

        ASSERT(0 != X.findR(&ph, KVS[3]));
    }

    {
        u::RandGen randGen;

        for (int len = 0; len < 30; ++len) {
            int numNodesForKey[k_NUM_VALS] = { 0 };

            Obj mX(&ta);    const Obj& X = mX;

            for (int ii = 0; ii < len; ++ii) {
                int key  = randGen() % k_NUM_VALS;
                int data = (key + 1) % k_NUM_VALS;

                mX.addRaw(0, KVS[key], DVS[data]);
                ++numNodesForKey[key];
            }
            ASSERT(X.length() == len);

            for (int key = 0; key < k_NUM_VALS; ++key) {
                int data = (key + 1) % k_NUM_VALS;

                PairHandle ph;
                const bool found = (0 == X.findR(&ph, KVS[key]));
                ASSERT(found == !!ph);
                ASSERT(found == !!numNodesForKey[key]);
                if (found) {
                    ASSERT(KVS[key]  == ph.key());
                    ASSERT(DVS[data] == ph.data());
                }
            }
        }
    }

    {
        bsl::vector<TableRecord> init(&ta);

        Obj mX(&ta);    const Obj& X = mX;

        for (int ii = 0; ii < 12; ++ii) {
            mX.addR(KVS[ii / 3], DVS[ii]);

            TableRecord tr = { L_, ii / 3, ii, 0 };
            init.push_back(tr);
        }

        TD::verify( X, &init[0], init.size(), L_);
        TD::verifyR(X, &init[0], init.size(), L_);

        if (verbose) TD::dump(X, &init[0], init.size());

        PairHandle ph;

        for (int ii = 0; ii < 4; ++ii) {
            ASSERT(0 == X.findR(&ph, KVS[ii]));
            ASSERTV(keyType, dataType, TTF::getIdentifier(ph.data()), ii,
                                                 ph.data() == DVS[3 * ii + 2]);
            ASSERT(0 == X.skipForward(&ph));
            ASSERT(!ph == (3 == ii));
            if (ph) {
                ASSERT(ph.key() == KVS[ii + 1]);
                ASSERT(ph.data() != DVS[4 * ii]);
            }
        }
    }

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        obk[ii].address()->~KEY_TYPE();
        obd[ii].address()->~DATA_TYPE();
    }

    ASSERT(0 == da.numAllocations());
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::iterationTest()
// ----------------------------------------------------------------------------
// ITERATION TEST
//
// Concerns:
//: 1 That iterators between elements of the container work.
//
// Plan:
//: 1 Create a container and populate it from the non-sorted table 'VALUES1',
//:
//: 2 Use the functions 'verify' and 'verifyR', which use iterators, to
//:   traverse the container forward and backward, and compare it with table
//:   'VALUES2', which has the same data as 'VALUES1', only sorted.
// ---------------------------------------------------------------------------
{
    if (verbose) cout << "iterationTest<" << bsls::NameOf<KEY_TYPE>() <<
                           ", " << bsls::NameOf<DATA_TYPE>() << ">();" << endl;

    TableRecord VALUES1[] = {
        { L_ , 1, 2, 0},
        { L_ , 3, 4, 0},
        { L_ , 0, 1, 0},
        { L_ , 2, 3, 0},
        { L_ , 4, 5, 0},
    };

    TableRecord VALUES2[] = {
        { L_ , 0, 1, 0},
        { L_ , 1, 2, 0},
        { L_ , 2, 3, 0},
        { L_ , 3, 4, 0},
        { L_ , 4, 5, 0},
    };

    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    {
        Obj mX(&ta);    const Obj& X = mX;
        TD::populate(&mX, VALUES1, U_LENGTH(VALUES1));
        verify( X, VALUES2, U_LENGTH(VALUES2), L_);
        verifyR(X, VALUES2, U_LENGTH(VALUES2), L_);

        if (k_OBJ_IS_PRINTABLE) {
            if (veryVerbose) {
                X.print(cout);
            } else if (verbose) {
                X.print(cout, 0, -1) << endl;
            }
        }
    }

    ASSERT(0 == da.numAllocations());
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::updateTest()
// ----------------------------------------------------------------------------
// UPDATE TEST
//
// Concerns:
//: 1 Update on an item that's been removed returns e_NOT_FOUND.
//:
//: 2 Update to an existing position returns e_DUPLICATE if allowDuplicates is
//:   false
//:
//: 3 Update updates the key value stored on the node.
//:
//: 4 After an update, the data can be looked up by its new value but not by
//:   its old value.
//:
//: 5 Update to an existing position succeeds if allowDuplicates is true
//:
//: 6 That when 'update' is called and the destination will create a duplicate,
//:   the moved node is after the other nodes with the same key value.
//:
//: 7 That when 'updateR' is called and the destination will create a
//:   duplicate, the moved node is after the other nodes with the same key
//:   value.
//
// Plan: each test step will update an item to a new location and
// verify 2 through 4 (we'll check (1) and (5) separately afterwards).
// We want to ensure that we move both *to* and *from* the beginning,
// middle, and end of the test.
//
// Note that an important feature of the "parameters" set is that
// executing it returns the list to its original state.
//
// Start with a table where all nodes are unique, and do 'update's and
// 'updateR's on the table to create duplicate keys, and observe that the
// ordering of the moved nodes is as expected.
// ---------------------------------------------------------------------------
{
    if (verbose) cout << "updateTest<" << bsls::NameOf<KEY_TYPE>() << ", " <<
                                         bsls::NameOf<DATA_TYPE>() << ">();\n";

    typedef int (Obj::*Updater)(const Pair*,
                                const KEY_TYPE &,
                                bool*,
                                bool);
    Updater updaters[] = { &Obj::update, &Obj::updateR };

    bslma::TestAllocator va("va", veryVeryVerbose);
    bslma::TestAllocator ta("ta", veryVeryVerbose);
    bslma::TestAllocator da("da", veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    enum { k_NUM_VALS  = 30 };

    bsls::ObjectBuffer<KEY_TYPE>   obk[k_NUM_VALS];
    KEY_TYPE                      *kvs = obk[0].address();
    const KEY_TYPE                *KVS = kvs;
    bsls::ObjectBuffer<DATA_TYPE>  obd[k_NUM_VALS];
    DATA_TYPE                     *dvs = obd[0].address();
    const DATA_TYPE               *DVS = dvs;

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        TTF::emplace(obk[ii].address(), ii, &va);
        TTF::emplace(obd[ii].address(), ii, &va);
    }

    {
        enum { k_NUM_ITEMS = 4 };

        const struct Parameters {
            int d_line;
            int d_from;
            int d_to;
            int d_value;
            bool d_isDuplicate;
        } parameters[] = {
            {L_,  0,  4,  0,  0},  //front-to-middle
            {L_,  3,  6,  1,  1},  //front dup
            {L_,  3, 10,  1,  0},  //front-to-back
            {L_, 10,  4,  1,  1},  //back dup
            {L_,  6, 12,  2,  0},  //middle-to-back
            {L_,  9,  3,  3,  0},  //middle-to-front
            {L_,  4, 10,  0,  1},  //middle dup
            {L_, 12,  2,  2,  0},  //back-to-front
            {L_,  4,  0,  0,  0},  //middle-to-back, again
            {L_, 10,  1,  1,  0},  //back-to-middle
            {L_,  3,  9,  3,  0},  //no move
            {L_,  2,  6,  2,  0},  //no move
            {L_,  1,  3,  1,  0},  //no move
        };

        for (int i = 0; i < 2; ++i) {
            Updater updater = updaters[i];

            Obj mX(&ta);

            for (int j = 0; j < k_NUM_ITEMS; ++j) {
                mX.add(KVS[j * 3], DVS[j]);
            }

            for (int j = 0; j < (int) (sizeof(parameters) /
                                                   sizeof (Parameters)); ++j) {
                const Parameters& p = parameters[j];

                PairHandle fromH;
                ASSERT(0 == mX.find(&fromH, KVS[p.d_from]));
                ASSERT(DVS[p.d_value] == fromH.data());

                int rc = (mX.*updater)(fromH, KVS[p.d_to], 0, false);
                if (p.d_isDuplicate) {
                    ASSERT(rc == Obj::e_DUPLICATE);
                }
                else {
                    ASSERTV(i, p.d_line, rc, 0 == rc);
                    ASSERT(KVS[p.d_to] == fromH.key());

                    PairHandle toH;
                    ASSERT(0 == mX.find(&toH, KVS[p.d_to]));
                    ASSERT(KVS[p.d_to] == toH.key());
                    ASSERT(DVS[p.d_value] == toH.data());
                    ASSERT(0 != mX.find(&toH, KVS[p.d_from]));
                }
            }

            ASSERT(k_NUM_ITEMS == mX.length());
            mX.removeAll();
            ASSERT(0 == mX.length());

            Obj mX2(&ta);

            PairHandle items[k_NUM_ITEMS];
            for (int j = 0; j < k_NUM_ITEMS; ++j) {
                mX2.add(items + j, KVS[j], DVS[j]);
            }

            ASSERT(k_NUM_ITEMS == mX2.length());

            //Now check concerns (1) and (5)
            ASSERT(0 == mX2.remove(items[0]));
            ASSERT(Obj::e_NOT_FOUND == (mX2.*updater)(items[0],
                                                      KVS[25],
                                                      0, false));
            ASSERT(Obj::e_NOT_FOUND == (mX2.*updater)(items[0],
                                                      KVS[25],
                                                      0, true));
            ASSERT(0 == mX2.remove(items[3]));
            ASSERT(Obj::e_NOT_FOUND == (mX2.*updater)(items[3],
                                                      KVS[25],
                                                      0, false));
            ASSERT(Obj::e_NOT_FOUND == (mX2.*updater)(items[3],
                                                      KVS[25],
                                                      0, true));

            ASSERT(0 == (mX2.*updater)(items[1], KVS[2], 0, true));
            ASSERT(KVS[2] == items[1].key());
            ASSERT(DVS[1] == items[1].data());
            ASSERT(KVS[2] == items[2].key());
        }
    }

    {
        enum { k_NUM_ITEMS      = 25,

               k_FORWARD_BEGIN  = 3,
               k_FORWARD_END    = 6,
               k_FORWARD_DEST   = 10,
               k_NUM_FORWARD    = k_FORWARD_END - k_FORWARD_BEGIN,

               k_FORWARD_DEST_OFFSET = k_FORWARD_DEST - k_NUM_FORWARD,

               k_BACKWARD_BEGIN = 18,
               k_BACKWARD_END   = 22,
               k_BACKWARD_DEST  = 15,
               k_NUM_BACKWARD   = k_BACKWARD_END - k_BACKWARD_BEGIN };

        bsl::vector<TableRecord> init(&ta);
        bsl::vector<TableRecord> moved(&ta);

        for (int ii = 0; ii < k_NUM_ITEMS; ++ii) {
            TableRecord tr = { L_, ii, ii, ii % 5 };
            init.push_back(tr);
        }

        Obj mX(&ta);    const Obj& X = mX;

        TD::populate(&mX, &init[0], init.size());
        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // move some nodes to somewhere in the middle

        for (int ii = k_FORWARD_BEGIN; ii < k_FORWARD_END; ++ii) {
            PairHandle ph;
            ASSERTV(ii, TTF::getIdentifier(KVS[ii]),
                                                    0 == X.find(&ph, KVS[ii]));
            ASSERT(ph);
            if (!ph) {
                TD::dump(X);
            }
            bool newFrontFlag;
            ASSERT(0 == mX.update(ph, KVS[k_FORWARD_DEST], &newFrontFlag, 1));
            ASSERT(! newFrontFlag);
            ASSERT(ph.key() == KVS[k_FORWARD_DEST]);
            ASSERT(0 == X.previous(&ph, ph));
            ASSERT(ph.key() != KVS[k_FORWARD_DEST]);

            TableRecord tr = init[ii];
            tr.d_key = k_FORWARD_DEST;
            moved.push_back(tr);
        }

        init.erase(init.begin() + k_FORWARD_BEGIN,
                   init.begin() + k_FORWARD_END);

        bsl::reverse(moved.begin(), moved.end());
        init.insert(init.begin() + k_FORWARD_DEST_OFFSET,
                    moved.begin(),
                    moved.end());

        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // start over

        init.clear();
        moved.clear();
        mX.removeAll();

        for (int ii = 0; ii < k_NUM_ITEMS; ++ii) {
            TableRecord tr = { L_, ii, ii, ii % 5 };
            init.push_back(tr);
        }

        TD::populate(&mX, &init[0], init.size());
        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // Move them to the front.

        for (int ii = k_FORWARD_BEGIN; ii < k_FORWARD_END; ++ii) {
            PairHandle ph;
            ASSERT(0 == X.find(&ph, KVS[ii]));
            bool newFrontFlag;
            ASSERT(0 == mX.update(ph, KVS[0], &newFrontFlag, true));
            ASSERT(newFrontFlag);
            TableRecord tr = init[ii];
            tr.d_key = 0;
            moved.push_back(tr);
        }

        init.erase(init.begin() + k_FORWARD_BEGIN,
                   init.begin() + k_FORWARD_END);

        bsl::reverse(moved.begin(), moved.end());
        init.insert(init.begin(), moved.begin(), moved.end());

        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // move the front node to the end

        {
            PairHandle ph;
            ASSERT(0 == X.find(&ph, KVS[0]));
            bool newFrontFlag;
            ASSERT(0 == mX.update(ph, KVS[k_NUM_ITEMS - 1], &newFrontFlag, 1));
            ASSERT(! newFrontFlag);

            TableRecord tr = init[0];
            tr.d_key = k_NUM_ITEMS - 1;

            init.erase(init.begin());
            init.insert(init.begin() + init.size() - 1, tr);
        }

        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // move the back node to the front

        {
            PairHandle ph;
            ASSERT(0 == X.findR(&ph, KVS[k_NUM_ITEMS - 1]));
            ASSERT(ph.data() == DVS[k_NUM_ITEMS - 1]); 
            bool newFrontFlag;
            ASSERT(0 == mX.update(ph, KVS[0], &newFrontFlag, 1));
            ASSERT(newFrontFlag);

            TableRecord tr = init[k_NUM_ITEMS - 1];
            tr.d_key = 0;

            init.erase(init.begin() + init.size() - 1);
            init.insert(init.begin(), tr);
        }

        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // start over

        init.clear();
        moved.clear();
        mX.removeAll();

        for (int ii = 0; ii < k_NUM_ITEMS; ++ii) {
            TableRecord tr = { L_, ii, ii, ii % 5 };
            init.push_back(tr);
        }

        TD::populate(&mX, &init[0], init.size());
        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // move several nodes from the middle to somewhere else in the middle

        for (int ii = k_BACKWARD_BEGIN; ii < k_BACKWARD_END; ++ii) {
            PairHandle ph;
            ASSERTV(ii, TTF::getIdentifier(KVS[ii]),
                                                   0 == X.findR(&ph, KVS[ii]));
            bool newFrontFlag;
            ASSERT(0 == mX.updateR(ph, KVS[k_BACKWARD_DEST], &newFrontFlag,1));
            ASSERT(! newFrontFlag);
            ASSERT(ph.key() == KVS[k_BACKWARD_DEST]);
            ASSERT(0 == X.next(&ph, ph));
            ASSERT(ph.key() != KVS[k_BACKWARD_DEST]);

            TableRecord tr = init[ii];
            tr.d_key = k_BACKWARD_DEST;
            moved.push_back(tr);
        }

        init.erase(init.begin() + k_BACKWARD_BEGIN,
                   init.begin() + k_BACKWARD_END);

        init.insert(init.begin() + k_BACKWARD_DEST + 1,
                    moved.begin(),
                    moved.end());

        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // start over

        init.clear();
        moved.clear();
        mX.removeAll();

        for (int ii = 0; ii < k_NUM_ITEMS; ++ii) {
            TableRecord tr = { L_, ii, ii, ii % 5 };
            init.push_back(tr);
        }

        TD::populate(&mX, &init[0], init.size());
        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // move them to the rear

        for (int ii = k_BACKWARD_BEGIN; ii < k_BACKWARD_END; ++ii) {
            PairHandle ph;
            ASSERTV(ii, TTF::getIdentifier(KVS[ii]),
                                                   0 == X.findR(&ph, KVS[ii]));
            bool newFrontFlag;
            ASSERT(0 == mX.updateR(ph, KVS[k_NUM_ITEMS - 1], &newFrontFlag,1));
            ASSERT(! newFrontFlag);
            ASSERT(ph.key() == KVS[k_NUM_ITEMS - 1]);
            ASSERT(0 == X.skipForward(&ph));
            ASSERTV(TTF::getIdentifier(ph.key()),
                                           TTF::getIdentifier(ph.data()), !ph);

            TableRecord tr = init[ii];
            tr.d_key = k_NUM_ITEMS - 1;
            moved.push_back(tr);
        }

        init.erase(init.begin() + k_BACKWARD_BEGIN,
                   init.begin() + k_BACKWARD_END);

        init.insert(init.end(), moved.begin(), moved.end());

        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);

        // move the back node to the front

        {
            PairHandle ph;
            ASSERT(0 == X.back(&ph));
            ASSERT(KVS[k_NUM_ITEMS    - 1] == ph.key());
            ASSERT(DVS[k_BACKWARD_END - 1] == ph.data());

            bool newFrontFlag;
            ASSERT(0 == mX.updateR(ph, KVS[0], &newFrontFlag, true));
            ASSERT(!newFrontFlag);

            TableRecord tr = init[init.size() - 1];
            tr.d_key = 0;

            init.erase(init.end() - 1);
            init.insert(init.begin() + 1, tr);

            X.skipForward(&ph);
            ASSERT(KVS[1] == ph.key());
        }            

        verify( X, &init[0], init.size(), L_);
        verifyR(X, &init[0], init.size(), L_);
    }

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        obk[ii].address()->~KEY_TYPE();
        obd[ii].address()->~DATA_TYPE();
    }

    V(da.numAllocations());
    ASSERT(0 == da.numBytesInUse());
    ASSERT(0 == da.numAllocations());
    V(ta.numBytesInUse());
    V(va.numBytesInUse());
    ASSERT(0 == ta.numBytesInUse());
    ASSERT(0 <  ta.numAllocations());
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::newFrontTest()
    // ------------------------------------------------------------------------
    // NEWFRONT TEST
    //
    // Concern:
    //: 1 'add' and 'update' correctly up the 'newFront' boolean arg if it's
    //:   passed to the list.
    //
    // Plan:
    //: 1 Add a few objects to the container.
    //:
    //: 2 Add another element that will not be at the front, observe that the
    //:   'isNewFront' boolean passed is 'false'.
    //:
    //: 3 Add another element that will nbe at the front, observe that the
    //:   'isNewFront' boolean passed is 'true'.
    //:
    //: 4 Update an element in such a way that it will not land at the front,
    //:   observe that the 'isNewFront' boolean passed is 'false'.
    //:
    //: 5 Update an element in such a way that it will land at the front,
    //:   observe that the 'isNewFront' boolean passed is 'true'.
    // ------------------------------------------------------------------------
{
    // -


    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator va(veryVeryVerbose);
    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    enum { k_NUM_VALS = 10 };
    bsls::ObjectBuffer<KEY_TYPE>   obk[k_NUM_VALS];
    KEY_TYPE                      *kvs = obk[0].address();
    const KEY_TYPE                *KVS = kvs;
    bsls::ObjectBuffer<DATA_TYPE>  obd[k_NUM_VALS];
    DATA_TYPE                     *dvs = obd[0].address();
    const DATA_TYPE               *DVS = dvs;

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        TTF::emplace(obk[ii].address(), ii, &va);
        TTF::emplace(obd[ii].address(), ii, &va);
    }

    Obj mX(&ta);

    if (verbose) cout << "newTop/newFront<" << bsls::NameOf<KEY_TYPE>() <<
                                 ", " << bsls::NameOf<DATA_TYPE>() << ">();\n";

    PairHandle h;
    bool isNewFront;

    mX.add(KVS[2], DVS[2]);
    ASSERT(1 == mX.length());

    mX.add(KVS[3], DVS[3], &isNewFront);
    ASSERT(!isNewFront);
    ASSERT(2 == mX.length());

    mX.add(KVS[1], DVS[1], &isNewFront);
    ASSERT(isNewFront);

    mX.add(&h, KVS[0], DVS[0], &isNewFront);
    ASSERT(isNewFront);

    ASSERT(4 == mX.length());

    Pair *pr;

    mX.frontRaw(&pr);
    ASSERT(DVS[0] == pr->data());
    ASSERT(KVS[0] == pr->key());

    ASSERT(!mX.update(pr, KVS[5], &isNewFront));
    ASSERT(!isNewFront);
    mX.releaseReferenceRaw(pr);

    mX.frontRaw(&pr);
    ASSERT(DVS[1] == pr->data());
    ASSERT(KVS[1] == pr->key());

    Pair *pr2;
    ASSERT(!mX.nextRaw(&pr2, pr));
    mX.releaseReferenceRaw(pr);
    ASSERT(KVS[2] == pr2->key());
    ASSERT(!mX.update(pr2, KVS[0], &isNewFront));
    ASSERT(isNewFront);
    mX.releaseReferenceRaw(pr2);

    mX.frontRaw(&pr);
    ASSERT(DVS[2] == pr->data());
    mX.releaseReferenceRaw(pr);

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        obk[ii].address()->~KEY_TYPE();
        obd[ii].address()->~DATA_TYPE();
    }
}

template <class KEY_TYPE, class DATA_TYPE>
void TestDriver<KEY_TYPE, DATA_TYPE>::allocationTest()
    // ------------------------------------------------------------------------
    // ALLLOCATION TEST
    //
    // Concern:
    //: 1 Memory comes from the correct allocator.
    //
    // Plan:
    //: 1 Add a few objects to the container, the pop them out.
    //:
    //: 2 Observe that no memory was allocated by the default allocator.
    //:
    //: 3 Observe that memory was allocated by the container's allocator.
    // ------------------------------------------------------------------------
{
    if (verbose) cout << "ALLOCATION TEST\n"
                         "===============\n";

    if (verbose) cout << "KEY_TYPE: " << bsls::NameOf<KEY_TYPE>() <<
                          ", DATA_TYPE: " << bsls::NameOf<DATA_TYPE>() << endl;

    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator va(veryVeryVerbose);
    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocGuard(&da);

    enum { k_NUM_VALS = 4 };

    bsls::ObjectBuffer<KEY_TYPE>   obk[k_NUM_VALS];
    KEY_TYPE                      *kvs = obk[0].address();
    const KEY_TYPE                *KVS = kvs;
    bsls::ObjectBuffer<DATA_TYPE>  obd[k_NUM_VALS];
    DATA_TYPE                     *dvs = obd[0].address();
    const DATA_TYPE               *DVS = dvs;

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        TTF::emplace(obk[ii].address(), ii, &va);
        TTF::emplace(obd[ii].address(), ii, &va);
    }

    {
        Obj mX(&ta);    const Obj& X = mX;

        ASSERT(X.isEmpty());
        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());

        mX.add(KVS[3], DVS[3]);
        mX.add(KVS[2], DVS[2]);
        mX.add(KVS[0], DVS[0]);
        mX.add(KVS[1], DVS[1]);

        ASSERT(!X.isEmpty());
        ASSERT(0 == da.numAllocations());

        PairHandle h;

        int ret = mX.popFront(&h);
        ASSERT(ret == 0);
        V_(h.key());    V(h.data());

        ASSERT(DVS[0] == h.data());

        ret = mX.popFront(&h);
        ASSERT(ret == 0);
        V_(h.key());    V(h.data());

        ASSERT(DVS[1] == h.data());

        ret = mX.popFront(&h);
        ASSERT(ret == 0);
        V_(h.key());    V(h.data());

        ASSERT(DVS[2] == h.data());

        ret = mX.popFront(&h);
        ASSERT(ret == 0);
        V_(h.key());    V(h.data());

        ASSERT(DVS[3] == h.data());
        V_(h.key());    V(h.data());

        ASSERT(X.isEmpty());

        ret = mX.popFront(&h);
        ASSERT(ret == Obj::e_NOT_FOUND);
    }

    for (int ii = 0; ii < k_NUM_VALS; ++ii) {
        obk[ii].address()->~KEY_TYPE();
        obd[ii].address()->~DATA_TYPE();
    }

    ASSERT(0 <  ta.numAllocations());
    ASSERT(0 == da.numAllocations());
}

// ============================================================================
//                            SOME HELPFUL EXTRAS
// ----------------------------------------------------------------------------

namespace RemoveAllSafetyTest {

typedef bdlcc::SkipList<int, bsltf::AllocTestType> Obj;
typedef Obj::Pair                                  Pair;
typedef Obj::PairHandle                            PairHandle;

bsls::AtomicInt   masterId(0);
bsls::AtomicInt   collisions(0);
bsls::AtomicInt   removed(0);
bslmt::Barrier   *barrier_p = 0;
bslma::Allocator *alloc_p;

void removeAllSafetyFunc(Obj        *list,
                         int         numIterations,
                         u::AddMode  mode)
{
    int id = ++masterId;

    barrier_p->wait();

    const bsltf::AllocTestType data(id, alloc_p);
    bsl::vector<Pair*>         removedPairs(alloc_p);
    bsl::vector<PairHandle>    removedHandles(alloc_p);

    for (int ii = 0; ii < numIterations; ++ii) {
        int         key = ii;
        Pair       *h   = 0;
        PairHandle  ph;

        switch (mode) {
          case u::e_ADD: {
            list->add(key, data);
          } break;
          case u::e_ADD_R: {
            list->addR(key, data);
          } break;
          case u::e_ADD_HANDLE: {
            list->add(&ph, key, data);
            ASSERT(ph);
          } break;
          case u::e_ADD_HANDLE_R: {
            list->addR(&ph, key, data);
            ASSERT(ph);
          } break;
          case u::e_ADD_RAW: {
            list->addRaw(0, key, data);
          } break;
          case u::e_ADD_RAW_R: {
            list->addRawR(0, key, data);
          } break;
          case u::e_ADD_RAW_PAIR: {
            list->addRaw(&h, key, data);
            ASSERT(h);
          } break;
          case u::e_ADD_RAW_R_PAIR: {
            list->addRawR(&h, key, data);
            ASSERT(h);
          } break;
          case u::e_ADD_AT_LEVEL_RAW: {
            list->addAtLevelRaw(0, 0, key, data);
          } break;
          case u::e_ADD_AT_LEVEL_RAW_R: {
            list->addAtLevelRawR(0, 0, key, data);
          } break;
          case u::e_ADD_AT_LEVEL_RAW_PAIR: {
            list->addAtLevelRaw(&h, 0, key, data);
            ASSERT(h);
          } break;
          case u::e_ADD_AT_LEVEL_RAW_R_PAIR: {
            list->addAtLevelRawR(&h, 0, key, data);
            ASSERT(h);
          } break;
          case u::e_ADD_AT_LEVEL_UNIQUE_RAW: {
            int rc = list->addAtLevelUniqueRaw(0, 0, key, data);
            collisions += !!rc;
          } break;
          case u::e_ADD_AT_LEVEL_UNIQUE_RAW_R: {
            int rc = list->addAtLevelUniqueRawR(0, 0, key, data);
            collisions += !!rc;
          } break;
          case u::e_ADD_AT_LEVEL_UNIQUE_RAW_PAIR: {
            int rc = list->addAtLevelUniqueRaw(&h, 0, key, data);
            ASSERT(!h == !!rc);
            collisions += !!rc;
          } break;
          case u::e_ADD_AT_LEVEL_UNIQUE_RAW_R_PAIR: {
            int rc = list->addAtLevelUniqueRawR(&h, 0, key, data);
            ASSERT(!h == !!rc);
            collisions += !!rc;
          } break;
          case u::e_ADD_UNIQUE: {
            int rc = list->addUnique(key, data);
            collisions += !!rc;
          } break;
          case u::e_ADD_UNIQUE_R: {
            int rc = list->addUniqueR(key, data);
            collisions += !!rc;
          } break;
          case u::e_ADD_UNIQUE_HANDLE: {
            int rc = list->addUnique(&ph, key, data);
            ASSERT(!!ph == !rc);
            collisions += !!rc;
          } break;
          case u::e_ADD_UNIQUE_R_HANDLE: {
            int rc = list->addUniqueR(&ph, key, data);
            ASSERT(!!ph == !rc);
            collisions += !!rc;
          } break;
          case u::e_ADD_UNIQUE_RAW: {
            int rc = list->addUniqueRaw(0, key, data);
            collisions += !!rc;
          } break;
          case u::e_ADD_UNIQUE_RAW_R: {
            int rc = list->addUniqueRawR(0, key, data);
            collisions += !!rc;
          } break;
          case u::e_ADD_UNIQUE_RAW_PAIR: {
            int rc = list->addUniqueRaw(&h, key, data);
            ASSERT(!h == !!rc);
            collisions += !!rc;
          } break;
          case u::e_ADD_UNIQUE_RAW_R_PAIR: {
            int rc = list->addUniqueRawR(&h, key, data);
            ASSERT(!h == !!rc);
            collisions += !!rc;
          } break;
          case u::e_ADD_WITH_REMOVEALL_HANDLES: {
            list->add(key, data);
          } break;
          default: {
            P(mode);
            BSLS_ASSERT(0);
          } break;
        }
        if (h) {
            list->releaseReferenceRaw(h);
        }

        if (u::e_ADD_WITH_REMOVEALL_HANDLES == mode) {
            // 'e_ADD_WITH_REMOVEALL_HANDLES' is the last mode tested.  This
            // overload of 'removeAll' uses the default allocator for an
            // internal temporary vector.  All previous modes should not use
            // the default allocator.

            unsigned rc = list->removeAll(&removedHandles);
            ASSERT(rc == removedHandles.size());
            removed += rc;

            removedHandles.clear();
        } else {
            switch (ii % 3) {
              case 0: {
                removed += list->removeAll();
              } break;
              case 1: {
                unsigned rc = list->removeAllRaw(&removedPairs);
                ASSERT(rc == removedPairs.size());
                removed += rc;

                for (unsigned jj = 0; jj < removedPairs.size(); ++jj) {
                    list->releaseReferenceRaw(removedPairs[jj]);
                }

                removedPairs.clear();
              } break;
              case 2: {
                removed += list->removeAllRaw(0);
              } break;
              default: {
                BSLS_ASSERT(0);
              } break;
            }
        }
    }
}

}  // close namespace RemoveAllSafetyTest

namespace ReferenceTest {

typedef bdlcc::SkipList<int, u::CountedDelete> Obj;

bsls::AtomicInt masterId(0);

void referenceTestFunc(Obj *list, int maxRefCount)
{
    u::RandGen randGen(++masterId);

    // Increment the ref count up to maxRefCount, then decrease.  While
    // incrementing, 10% of the time throw in a decrement.

    Obj::Pair *front;
    ASSERT(0 == list->frontRaw(&front));

    for (int numReferencesAdded = 1; numReferencesAdded < maxRefCount; )
    {
        list->addPairReferenceRaw(front);

        if (4 == randGen() % 10) {
            list->releaseReferenceRaw(front);
        }
        else {
            numReferencesAdded++;
        }
    }

    for (int i = 0; i < maxRefCount; ++i) {
        list->releaseReferenceRaw(front);
    }
}

}  // close namespace ReferenceTest

namespace USAGE {

//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Scheduler
///- - - - - - - - - - - - - - - -
// The "R" methods of 'bdlcc::SkipList' make it ideal for use in a scheduler,
// in which events are likely to be scheduled after existing events.  In such
// an implementation, events are stored in the list with their scheduled
// execution times as 'KEY' objects: Searching near the end of the list for the
// right location for new events, and removing events from the front of the
// list for execution, are very efficient operations.  Being thread- enabled
// also makes 'bdlcc::SkipList' well-suited to use in a scheduler - a
// "dispatcher" thread can safety use the list at the same time that events are
// being scheduled from other threads.  The following is an implementation of a
// simple scheduler class using 'bdlcc::SkipList'.  Note that the mutex in the
// scheduler is used only in connection with the scheduler's condition variable
// - thread-safe access to the 'bdlcc::SkipList' object does *not* require any
// synchronization.
//
//..
    class SimpleScheduler
    {
        // TYPES
        typedef bdlcc::SkipList<bdlt::Datetime, bsl::function<void()> > List;

        // DATA
        List                       d_list;
        bslmt::ThreadUtil::Handle  d_dispatcher;
        bslmt::Condition           d_notEmptyCond;
        bslmt::Condition           d_emptyCond;
        bslmt::Barrier             d_startBarrier;
        bslmt::Mutex               d_condMutex;
        volatile int               d_doneFlag;

      private:
        // NOT IMPLEMENTED
        SimpleScheduler(const SimpleScheduler&);

      private:
        // PRIVATE MANIPULATORS
        void dispatcherThread()
            // Run a thread that executes functions off 'd_list'.
        {
            d_startBarrier.wait();

            while (!d_doneFlag) {
                List::PairHandle firstItem;
                if (0 == d_list.front(&firstItem)) {
                    // The list is not empty.

                    bsls::TimeInterval when =
                        bdlt::IntervalConversionUtil::convertToTimeInterval(
                                   firstItem.key() - bdlt::CurrentTime::utc());
                    if (when.totalSecondsAsDouble() <= 0) {
                        // Execute now and remove from schedule, then iterate.

                        d_list.remove(firstItem);
                        firstItem.data()();

                        List::PairHandle tmpItem;

                        bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);

                        if (0 == d_list.length()) {
                            d_emptyCond.broadcast();
                        }
                    }
                    else {
                        // Wait until the first scheduled item is due.

                        bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);
                        List::PairHandle newFirst;
                        if (!d_doneFlag && (0 != d_list.front(&newFirst) ||
                                          newFirst.key() == firstItem.key())) {
                            d_notEmptyCond.timedWait(&d_condMutex,
                                              bdlt::CurrentTime::now() + when);
                        }
                    }
                }
                else {
                    // The list is empty; wait on the condition variable.

                    bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);
                    if (d_list.isEmpty() && !d_doneFlag) {
                        d_notEmptyCond.wait(&d_condMutex);
                    }
                }
            }
        }

      public:
        // CREATORS
        explicit
        SimpleScheduler(bslma::Allocator *basicAllocator = 0)
        : d_list(basicAllocator)
        , d_startBarrier(2)
        , d_doneFlag(false)
            // Creator.
        {
            int rc = bslmt::ThreadUtil::create(
                    &d_dispatcher,
                    bdlf::BindUtil::bind(&SimpleScheduler::dispatcherThread,
                                            this));
            BSLS_ASSERT(0 == rc);  (void)rc;
            d_startBarrier.wait();
        }

        ~SimpleScheduler()
            // d'tor
        {
            stop();
        }

        // MANIPULATORS
        void drain()
            // Block until the scheduler has no jobs.
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);

            while (!d_doneFlag && 0 != d_list.length()) {
                d_emptyCond.wait(&d_condMutex);
            }
        }

        void scheduleEvent(const bsl::function<void()>& event,
                           const bdlt::Datetime&        when)
            // Schedule the specified 'event' to occur at the specified 'when'.
        {
            // Use 'addR' since this event will probably be placed near the end
            // of the list.

            bool newFrontFlag;
            d_list.addR(when, event, &newFrontFlag);
            if (newFrontFlag) {
                // This event is scheduled before all other events.  Wake up
                // the dispatcher thread.

                d_notEmptyCond.signal();
            }
        }

        void stop()
            // Stop the scheduler.
        {
            bslmt::LockGuard<bslmt::Mutex> guard(&d_condMutex);

            d_list.removeAll();

            d_doneFlag = true;
            d_notEmptyCond.signal();
            d_emptyCond.broadcast();

            if (bslmt::ThreadUtil::invalidHandle() != d_dispatcher) {
                bslmt::ThreadUtil::Handle dispatcher = d_dispatcher;
                {
                    bslmt::LockGuardUnlock<bslmt::Mutex> g(&d_condMutex);
                    bslmt::ThreadUtil::join(dispatcher);
                }
                d_dispatcher = bslmt::ThreadUtil::invalidHandle();
            }
        }
    };
//..
// We can verify the correct behavior of 'SimpleScheduler'.  First, we need a
// wrapper around vector<int>::push_back, since this function is overloaded and
// cannot be bound directly:
//..
    void pushBackWrapper(bsl::vector<int> *vector, int item)
        // Push the specified 'item' onto the specified 'vector'.
    {
        vector->push_back(item);
    }

}  // close namespace USAGE

// ============================================================================
//             CASE 29 REPRODUCE BUG / VERIFY FIX OF DRQS 145745492
// ----------------------------------------------------------------------------

namespace SKIPLIST_TEST_CASE_NO_DEFAULT_CTOR_KEY_VALUE {

class KeyValue {
    // DATA
    int d_i;

  public:
    // CREATOR
    explicit
    KeyValue(int i)
    : d_i(i)
        // Create an object whose value is the specified 'i'.
    {}

    // ACCESSOR
    bool operator<(const KeyValue rhs) const
        // Return 'true' if this object is less than the specified 'rhs' and
        // 'false' otherwise.
    {
        return d_i < rhs.d_i;
    }
};

}  // close namespace SKIPLIST_TEST_CASE_NO_DEFAULT_CTOR_KEY_VALUE

// ============================================================================
//                         CASE 28 DRQS 144652915
// ----------------------------------------------------------------------------

namespace SKIPLIST_TEST_CASE_DRQS_144652915 {

struct Payload { char d_data[140]; };
typedef bdlcc::SkipList<int, Payload> Obj;
bsls::AtomicInt index(0);

Obj *g_skipList_p = 0;

int numThreads            =   16;
int numNodesPerThread     = 1000;
int barrierTimeoutSeconds =   60;

void addNodes(bslmt::Barrier *barrier)
    // Add 'numNodesPerThread' nodes to the skip list.  Ensure threads run
    // simultaneously by waiting on the specified 'barrier'.
{
    bsls::TimeInterval timeout(bsls::SystemTime::now(barrier->clockType()));
    timeout.addSeconds(barrierTimeoutSeconds);

    int barrierTimedWaitResult = barrier->timedWait(timeout);
    ASSERT(!barrierTimedWaitResult);

    Payload payload;
    for (unsigned uu = 0; uu < sizeof(Payload); ++uu) {
        payload.d_data[uu] = static_cast<char>(0xaf);
    }
    for (int i = 0; i < numNodesPerThread; ++i) {
        Obj::PairHandle handle;
        bool newtop;
        g_skipList_p->addR(&handle, ++index, payload, &newtop);
        if (newtop) {
            cout << "Newtop\n";
        }
    }
}

}  // close namespace SKIPLIST_TEST_CASE_DRQS_144652915

// ============================================================================
//                         CASE 101 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace SKIPLIST_OLD_TEST_CASES_NAMEPSACE {

namespace TC = SKIPLIST_OLD_TEST_CASES_NAMEPSACE;

struct DATA {
    int         l;
    int         key;
    const char *data;
    int         level;
};

struct IDATA {
    int l;
    int key;
    int data;
    int level;
};

void case16Produce (bdlcc::SkipList<int, int> *list, bsls::AtomicInt *done)
    // While the specified 'done' is 'false', produce nodes on the specified
    // 'list'.
{
    int count = 0;
    while (!(*done)) {
        if (0 > ++count) {
            count = 0;
        }
        bdlcc::SkipList<int, int>::Pair *h;
        list->addRaw(&h, count, count);
        bslmt::ThreadUtil::yield();
        list->releaseReferenceRaw(h);
    }
}

void case16Consume(bdlcc::SkipList<int, int> *list, bsls::AtomicInt *done)
    // While the specified 'done' is 'false', from nodes from the specified
    // 'list'.
{
    while (!(*done)) {
        bdlcc::SkipList<int, int>::Pair *h1;
        ASSERT(0 == list->frontRaw(&h1));
        bdlcc::SkipList<int, int>::Pair *h2;
        ASSERT(0 == list->findRRaw(&h2, h1->key()));
        ASSERT(0 != h2);
        ASSERT(h2->key() == h2->key());
        list->releaseReferenceRaw(h1);
        list->releaseReferenceRaw(h2);
        list->remove(h1);
    }
}

template<class SKIPLIST, class ARRAY>
void populate(SKIPLIST *list, const ARRAY& array, int length)
    // Add the specified 'length' items from the specified 'array' to the
    // specified 'list'.
{
    for (int i=0; i<length; i++) {
        list->add(array[i].key, array[i].data);
    }
}

template<class SKIPLIST, class ARRAY>
void populateEx(SKIPLIST *list, const ARRAY& array, int length)
    // Add the specified 'length' items from the specified 'array' to the
    // specified 'list', taking the 'level' field into account.
{
    for (int i=0; i<length; i++) {
        list->addAtLevelRaw(0, array[i].level, array[i].key, array[i].data);
    }
}

template<class SKIPLIST, class ARRAY>
void verify(SKIPLIST *list, const ARRAY& array, int length, int line)
    // Verify that the contents of the specified 'list' match those of the
    // specified 'array' of the specified 'length'.  If a mismatch occurs,
    // have the asserts show the specified 'line' in the trace.
{
    // scan forward using 'raw' and 'skip' methods; and also using 'front' and
    // 'next' (non-raw) methods, in parallel.
    typename SKIPLIST::Pair *p;
    typename SKIPLIST::PairHandle h;
    list->frontRaw(&p);
    list->front(&h);

    int i;

    for (i=0; i<length && p; i++) {
        ASSERT_LL(p->key(), array[i].key, array[i].l, line);
        ASSERT_LL(p->data(), array[i].data, array[i].l, line);

        ASSERT_LL(h.key(), array[i].key, array[i].l, line);
        ASSERT_LL(h.data(), array[i].data, array[i].l, line);

        int ret = list->skipForwardRaw(&p);
        ASSERT(ret==0);

        ret = list->next(&h, h);
        ASSERT(ret == 0 || i == length-1);
    }

    if (p) {
        list->releaseReferenceRaw(p);
    }

    ASSERT_L(i==length, line);
    ASSERT_L(p==0, line);
}

template<class SKIPLIST, class ARRAY>
void verifyReverse(const SKIPLIST& list,
                   const ARRAY&    array,
                   int             length,
                   int             line)
    // Verify that the contents of the specified 'list' match those of the
    // specified 'array' of the specified 'length' by doing a backward
    // traversal of both.  If a mismatch occurs, have the asserts show the
    // specified 'line' in the trace.
{
    // scan backward using 'skip' and also 'previous', in parallel

    typename SKIPLIST::PairHandle p, p2;
    list.back(&p);
    list.back(&p2);

    int i;

    for (i=length-1; i>=0 && p; i--) {
        ASSERT_LL(p.key(), array[i].key, array[i].l, line);
        ASSERT_LL(p.data(), array[i].data, array[i].l, line);

        ASSERT_LL(p2.key(), array[i].key, array[i].l, line);
        ASSERT_LL(p2.data(), array[i].data, array[i].l, line);

        int ret = list.skipBackward(&p);
        ASSERT(ret==0);

        ret = list.previous(&p2, p2);
        ASSERT(ret == 0 || i == 0);
    }

    ASSERT_L(i==-1, line);
    ASSERT_L(p==0, line);
}

template<class SKIPLIST, class ARRAY>
void verifyEx(SKIPLIST* list, const ARRAY& array, int length, int line)
    // Verify that the contents of the specified 'list' match those of the
    // specified 'array' of the specified 'length'.  If a mismatch occurs,
    // have the asserts show the specified 'line' in the trace.
{
    typename SKIPLIST::Pair *p;
    list->frontRaw(&p);

    int i;

    for (i=0; i<length && p; i++) {
        ASSERT_LL(p->key(), array[i].key, array[i].l, line);
        ASSERT_LL(p->data(), array[i].data, array[i].l, line);
        ASSERT_LL(list->level(p), array[i].level, array[i].l, line);

        int ret = list->skipForwardRaw(&p);
        ASSERT(ret==0);
    }

    if (p) {
        list->releaseReferenceRaw(p);
    }

    ASSERT_L(i==length, line);
    ASSERT_L(p==0, line);
}

#define POPULATE_LIST(LP, A)   { TC::populate(LP, A, sizeof(A)/sizeof(A[0])); }
#define POPULATE_LIST_EX(LP, A)  {                                            \
    TC::populateEx(LP, A, sizeof(A)/sizeof(A[0]));                            \
}

#define VERIFY_LIST(L, A) {                                                   \
    TC::verify(&(L), A, sizeof(A)/sizeof(A[0]), __LINE__);                    \
}

#define VERIFY_LIST_REVERSE(L, A) { \
                   TC::verifyReverse(L, A, sizeof(A)/sizeof(A[0]), __LINE__); }

#define VERIFY_LIST_EX(L, A) { \
                     TC::verifyEx(&(L), A, sizeof(A)/sizeof(A[0]), __LINE__); }

typedef bdlcc::SkipList<int,int> List;

enum {
    k_NUM_THREADS    = 12,
    k_NUM_ITERATIONS = 100
};

void threadFunc(List *list, int numIterations, int threadNum)
{
    for (int j=0; j<numIterations; j++) {
        for (int i=0; i<numIterations; i++) {
            list->add(1000*i + threadNum, -1000*i - threadNum);
        }

        for (int i=0; i<numIterations; i++) {
            int k = 1000*i + threadNum;
            int d = -k;
            List::Pair *h;
            ASSERT(0 == list->findRaw(&h, k));
            ASSERT(h);
            ASSERT(h->key()==k);
            ASSERT(h->data()==d);
            list->remove(h);
            list->releaseReferenceRaw(h);
        }
    }
}

void run()
{
    if (verbose) cout << endl
                      << "The thread-safety test" << endl
                      << "======================" << endl;

    IDATA VALUES1[] = {
        { L_ , 1, 1, 0},
        { L_ , 3, 3, 1},
        { L_ , 0, 0, 2},
        { L_ , 2, 2, 3},
        { L_ , 4, 4, 4},
    };

    IDATA VALUES2[] = {
        { L_ , 0, 0, 2},
        { L_ , 1, 1, 0},
        { L_ , 2, 2, 3},
        { L_ , 3, 3, 1},
        { L_ , 4, 4, 4},
    };

    List list;
    POPULATE_LIST_EX(&list, VALUES1);

    bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::create(&threads[i],
                                 bdlf::BindUtil::bind(&threadFunc,
                                                     &list,
                                                     (int)k_NUM_ITERATIONS,
                                                     i+1));
    }

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

    VERIFY_LIST_EX(list, VALUES2);
}

}  // close namespace SKIPLIST_OLD_TEST_CASES_NAMEPSACE

// ============================================================================
//                        CASE -100 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace SKIPLIST_TEST_CASE_MINUS_100 {

static bsls::AtomicInt currentTime(0);
typedef bdlcc::SkipList<bsls::TimeInterval,int> TimeQ;

enum {
    k_NUM_THREADS    = 4,
    k_NUM_ITERATIONS = 100,
    k_SEND_COUNT = 1000,
    k_RCV_COUNT = 900,
    k_DELAY = 500
};

void threadFunc(TimeQ *timeQueue,
                int    numIterations,
                int    sendCount,
                int    receiveCount,
                int    delay)
    // Thread function, operate on the specified 'timeQueue' for the specified
    // 'numIterations', each iteration adding 'sendCount' objects to
    // 'timeQueue' and removing 'releaseCount' objects.  Add the specified
    // 'delay' to the 'value' elements in the skiplist nodes.
{
    bsl::vector<TimeQ::Pair*> timers;
    timers.resize(sendCount);

    bsls::Stopwatch sw;

    for (int i=0; i<numIterations; i++) {
        if ( verbose ) {
            sw.start();
        }

        // "send" messages
        for (int snd=0; snd<sendCount; snd++) {
            currentTime++;
            bsls::TimeInterval t(currentTime + delay, 0);
            timeQueue->addRaw(&timers[snd], t, delay);
        }

        // "receive" replies
        for (int rcv=0; rcv<receiveCount; rcv++) {
            timeQueue->remove(timers[rcv]);
            timeQueue->releaseReferenceRaw(timers[rcv]);
        }

        // "resend" replies
        bsls::TimeInterval now(currentTime, 0);
        while (1) {
            TimeQ::Pair *resubmit;
            if (0 != timeQueue->frontRaw(&resubmit)) {
                break;
            }

            const bsls::TimeInterval& k = resubmit->key();
            if (k <= now) {
                timeQueue->remove(resubmit);
                int newdelay = resubmit->data() * 2;
                bsls::TimeInterval t(currentTime + newdelay, 0);
                timeQueue->add(t, newdelay);
                timeQueue->releaseReferenceRaw(resubmit);
            }
            else {
                timeQueue->releaseReferenceRaw(resubmit);
                break;
            }
        }

        // clean up remaining handles
        for (int cln=receiveCount; cln<sendCount; cln++) {
            timeQueue->releaseReferenceRaw(timers[cln]);
        }

        if ( verbose ) {
            sw.stop();

            int iteration = i;
            double elapsed = sw.elapsedTime();
            P(iteration);
            P(elapsed);
        }
    }
}

void run()
    // run
{
    if (verbose) cout << endl
                      << "The router simulation (kind of) test" << endl
                      << "====================================" << endl;

    TimeQ timeQueue;

    bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::create(&threads[i],
                                 bdlf::BindUtil::bind(&threadFunc,
                                                     &timeQueue,
                                                     (int)k_NUM_ITERATIONS,
                                                     (int)k_SEND_COUNT,
                                                     (int)k_RCV_COUNT,
                                                     (int)k_DELAY));
    }

    for (int i = 0; i < k_NUM_THREADS; ++i) {
        bslmt::ThreadUtil::join(threads[i]);
    }

}

}  // close namespace SKIPLIST_TEST_CASE_MINUS_100

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 27: {
        // --------------------------------------------------------------------
        // THOROUGH MULTI-THREADED ADD TEST
        //
        // Concern:
        //: 1 Need to test doing adds and carefully examining the state of the
        //:   container after the add is completed, for all 'add' functions
        //:   under multithreaded conditions.
        //
        // Plan:
        //: 1 Use the function 'addByMode' which will call one of the 'add*'
        //:   functions to attempt to add a node to the container, and then
        //:   exhaustively check out the state of the container after the
        //:   'add'.
        //:
        //: 2 Spawn many threads with differently seeded random number
        //:   generators to driver 'addByMode'.
        //:
        //: 3 Have all the threads attempt to add nodes to the container, and
        //:   keep track of how many nodes with each key value were
        //:   successfully added in an array of 'bsls::AtomicInt's.
        //:
        //: 4 After the threads have been joined, call 'checkContainer' to
        //:   verify that exactly the number of nodes for each key value
        //:   expected were present.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        manyThreadsRandomModesOnOneSkipListMain,
                        U_TEST_TYPES_REGULAR);

        // With the exceptio of 'TTF::MethodPtr', all these 'DATA' types are
        // already covered by 'U_TEST_TYPES_REGULAR', we just want to do
        // some tests where 'KEY' & 'DATA' are different types.

        TestDriver<int, TTF::MethodPtr>::
                                     manyThreadsRandomModesOnOneSkipListMain();
        TestDriver<int, bsltf::AllocTestType>::
                                     manyThreadsRandomModesOnOneSkipListMain();
        TestDriver<int, bsltf::MovableAllocTestType>::
                                     manyThreadsRandomModesOnOneSkipListMain();
        TestDriver<int, bsltf::SimpleTestType>::
                                     manyThreadsRandomModesOnOneSkipListMain();
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // THOROUGH SINGLE-THREADED ADD TEST
        //
        // Concern:
        //: 1 Need to test doing adds and carefully examining the state of the
        //:   container after the add is completed, for all 'add' functions.
        //
        // Plan:
        //: 1 In the first pass, for every 'add' function, create a skip list
        //:   and then add values from a table to it using that one add
        //:   function.
        //:
        //: 2 In the second pass, iterate, creating an add function, then
        //:   perform a sequence of adds to it, adding values from a table, and
        //:   rotating which 'add' method is used each time.
        //:
        //: 3 In both '1' and '2' above, the table contains many redundant
        //:   values to test the 'unique' functions.
        //:
        //: 4 In both '1' and '2' above, keep an array 'numNodesPerKeyVal'
        //:   which keeps a tally of how many nodes have been added for a given
        //:   key value.  At the end, 'checkContainer' is called which verifies
        //:   that these tallies are correct.
        // --------------------------------------------------------------------

        if (verbose) cout << "THOROUGH SINGLE-THREADED ADD TEST\n"
                             "=================================\n";

        if (verbose) cout << "\ntestIndependentAddFunctions\n"
                               "---------------------------\n";

        U_RUN_EACH_TYPE(TestDriver,
                        testIndependentAddFunctions,
                        U_TEST_TYPES_REGULAR);

        // With the exceptio of 'TTF::MethodPtr', all these 'DATA' types are
        // already covered by 'U_TEST_TYPES_REGULAR', we just want to do
        // some tests where 'KEY' & 'DATA' are different types.

        TestDriver<int, TTF::MethodPtr>::testIndependentAddFunctions();
        TestDriver<int, bsltf::AllocTestType>::testIndependentAddFunctions();
        TestDriver<int, bsltf::MovableAllocTestType>::
                                                 testIndependentAddFunctions();
        TestDriver<int, bsltf::SimpleTestType>::testIndependentAddFunctions();

        if (verbose) cout << "\ntestMultiAddFunctions\n"
                               "---------------------\n";

        U_RUN_EACH_TYPE(TestDriver,
                        testMultiAddFunctions,
                        U_TEST_TYPES_REGULAR);

        // With the exceptio of 'TTF::MethodPtr', all these 'DATA' types are
        // already covered by 'U_TEST_TYPES_REGULAR', we just want to do
        // some tests where 'KEY' & 'DATA' are different types.

        TestDriver<int, TTF::MethodPtr>::testMultiAddFunctions();
        TestDriver<int, bsltf::AllocTestType>::testMultiAddFunctions();
        TestDriver<int, bsltf::MovableAllocTestType>::testMultiAddFunctions();
        TestDriver<int, bsltf::SimpleTestType>::testMultiAddFunctions();
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // REPRODUCE BUG / VERIFY FIX OF DRQS 145745492
        //
        // Concern:
        //: 1 The DRQS complains that recent changes to skiplist made it
        //:   require default c'tors for the key and value types.  Reproduce
        //:   bug.
        //
        // Plan:
        //: 1 Declare a 'KeyValue' type with no default and use it to create
        //:   a 'SkipList'.
        //
        // Testing:
        //   REPRODUCE BUG / VERIFY FIX OF DRQS 145745492
        // --------------------------------------------------------------------

        if (verbose) cout << "REPRODUCE BUG / VERIFY FIX OF DRQS 145745492\n"
                             "============================================\n";

        namespace Test = SKIPLIST_TEST_CASE_NO_DEFAULT_CTOR_KEY_VALUE;

        bdlcc::SkipList<Test::KeyValue, Test::KeyValue> sl;    (void) sl;
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // REPRODUCE BUG FROM DRQS 144652915
        //
        // Concerns:
        //: 1 DRQS illustrated a problem when many threads were adding items
        //:   simultaneously
        //
        // Plan:
        //: 1 Have 16 threads each simultaneously add 1000 items to the skip
        //:   list, and observe whether any of the threads throws
        //:   'std::bad_alloc'.
        //
        // Testing:
        //   DRQS 144652915
        // --------------------------------------------------------------------

        if (verbose) cout << "REPRODUCE BUG FROM DRQS 144652915\n";

        using namespace SKIPLIST_TEST_CASE_DRQS_144652915;

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        Obj skipList(&ta);
        g_skipList_p = &skipList;

        P_(numThreads);    P(numNodesPerThread);

        bslmt::ThreadUtil::setThreadName("Main Thread");

        bsl::vector<bslmt::ThreadUtil::Handle> handles(&ta);

        bslmt::Barrier barrier(numThreads);

        for (int ii = 0; ii < numThreads; ++ii) {
            bslmt::ThreadAttributes attr(&ta);
            bsl::ostringstream oss(&ta);
            oss << "Thread " << ii;
            attr.setThreadName(oss.str());
            bslmt::ThreadUtil::Handle handle;

            int rc = bslmt::ThreadUtil::create(
                &handle,
                attr,
                bdlf::BindUtil::bind(&addNodes, &barrier));
            ASSERT(0 == rc);

            handles.push_back(handle);
        }

        for (int ii = 0; ii < numThreads; ++ii) {
            bslmt::ThreadUtil::join(handles[ii]);
        }

        P(skipList.length());
        P(ta.numBytesMax());
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING 'allocator' ACCESSOR
        //
        // Concern:
        //   That the 'allocator' accessor correctly returns the allocator
        //   suplied at construction.
        //
        // Plan:
        //   Create objects with different allocators, and verify that the
        //   value returned by the 'allocator' accessor is as expected.
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'allocator' ACCESSOR\n"
                             "============================\n";

        typedef bdlcc::SkipList<int, bsl::string> Obj;

        {
            Obj mX;  const Obj& X = mX;

            ASSERT(bslma::Default::defaultAllocator() == X.allocator());
        }
        {
            bslma::TestAllocator oa("supplied");

            Obj mX(&oa);  const Obj& X = mX;

            ASSERT(&oa == X.allocator());
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // add / addR stability test
        // --------------------------------------------------------------------

        using namespace SKIPLIST_OLD_TEST_CASES_NAMEPSACE;

        DATA VALUES1[] = {
            // line,  key,  data,  level
            { L_ , 1, "fwd 1", 0},
            { L_ , 1, "middle-fwd 1", 0},
            { L_ , 1, "middle-back  1 ", 0},
            { L_ , 1, "back 1", 0},
            { L_ , 3, "3", 1},
            { L_ , 0, "0", 2},
            { L_ , 2, "2", 3},
            { L_ , 4, "first 4", 4},
            { L_ , 4, "middle 4", 4},
            { L_ , 4, "last 4", 4},
            { L_ , 8, "first 8", 4},
            { L_ , 8, "last 8", 4},
        };
        if (verbose) cout << endl
                      << "addR - verify insertion order is stable" << endl
                      << "=======================================" << endl;

          bslma::TestAllocator ta(veryVeryVerbose);
          bslma::TestAllocator da(veryVeryVerbose);
          bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
          typedef bdlcc::SkipList<int, bsl::string> Obj;
          {
            Obj mX(&ta);
            unsigned int size = sizeof(VALUES1)/sizeof(VALUES1[0]);
            for (unsigned int i = 0; i < size; ++i) {
                mX.addR(VALUES1[i].key, VALUES1[i].data);
            }

            Obj::PairHandle h;
            mX.popFront(&h);
            ASSERT(h.key() == 0);
            ASSERT(h.data() == "0");

            mX.popFront(&h);
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "fwd 1");

            mX.popFront(&h);
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "middle-fwd 1");

            mX.popFront(&h);
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "middle-back  1 ");

            mX.popFront(&h);
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "back 1");

            if (veryVerbose) {
                mX.print(cout);
            } else if (verbose) mX.print(cout, 0, -1) << endl;
        }

        if (verbose) cout << endl
                      << "add - verify insertion order is not stable" << endl
                      << "=========================================" << endl;

        {
            Obj mX(&ta);
            unsigned int size = sizeof(VALUES1)/sizeof(VALUES1[0]);
            for (unsigned int i = 0; i < size; ++i) {
                mX.add(VALUES1[i].key, VALUES1[i].data);
            }

            Obj::PairHandle h;
            mX.popFront(&h);
            ASSERT(h.key() == 0);
            ASSERT(h.data() == "0");

            mX.popFront(&h);
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "back 1");

            mX.popFront(&h);
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "middle-back  1 ");

            mX.popFront(&h);
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "middle-fwd 1");

            mX.popFront(&h);
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "fwd 1");

            if (veryVerbose) {
                mX.print(cout);
            } else if (verbose) mX.print(cout, 0, -1) << endl;
        }

        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // findLowerBoundR / findUpperBoundR test
        // --------------------------------------------------------------------

        using namespace SKIPLIST_OLD_TEST_CASES_NAMEPSACE;

        DATA VALUES1[] = {
            // line,  key,  data,  level
            { L_ , 1, "fwd 1", 0},
            { L_ , 1, "middle-fwd 1", 0},
            { L_ , 1, "middle-back  1 ", 0},
            { L_ , 1, "back 1", 0},
            { L_ , 3, "3", 1},
            { L_ , 0, "0", 2},
            { L_ , 2, "2", 3},
            { L_ , 4, "first 4", 4},
            { L_ , 4, "middle 4", 4},
            { L_ , 4, "last 4", 4},
            { L_ , 8, "first 8", 4},
            { L_ , 8, "last 8", 4},
        };
        if (verbose) cout << endl
                          << "findLowerBoundR/findUpperBoundR test" << endl
                          << "====================================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bdlcc::SkipList<int, bsl::string> Obj;
        {
            Obj mX(&ta);
            unsigned int size = sizeof(VALUES1)/sizeof(VALUES1[0]);
            for (unsigned int i = 0; i < size; ++i) {
                mX.addR(VALUES1[i].key, VALUES1[i].data);
            }

            Obj::PairHandle h;
            //validate error when trying to find something greater than max key
            ASSERT(mX.findLowerBoundR(&h, 10));
            ASSERT(mX.findUpperBoundR(&h, 10));

            //validate success when trying to find something smaller
            //than min key
            ASSERT(!mX.findLowerBoundR(&h,-1));
            ASSERT(h.key() == 0);
            ASSERT(h.data() == "0");

            ASSERT(!mX.findUpperBoundR(&h,-1));
            ASSERT(h.key() == 0);
            ASSERT(h.data() == "0");

            //check front edge
            ASSERT(!mX.findUpperBoundR(&h, 0));
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "fwd 1");

            ASSERT(!mX.findLowerBoundR(&h, 0));
            ASSERT(h.key() == 0);
            ASSERT(h.data() == "0");

            //check somewhere inside of list
            ASSERT(!mX.findLowerBoundR(&h,6));
            ASSERT(h.key() == 8);
            ASSERT(h.data() == "first 8");

            ASSERT(!mX.findUpperBoundR(&h,6));
            ASSERT(h.key() == 8);
            ASSERT(h.data() == "first 8");

            //check back edge
            ASSERT(mX.findUpperBoundR(&h, 8));

            ASSERT(!mX.findLowerBoundR(&h, 8));
            ASSERT(h.key() == 8);
            ASSERT(h.data() == "first 8");

            //check middle
            ASSERT(!mX.findUpperBoundR(&h, 4));
            ASSERT(h.key() == 8);
            ASSERT(h.data() == "first 8");

            ASSERT(!mX.findLowerBoundR(&h, 4));
            ASSERT(h.key() == 4);
            ASSERT(h.data() == "first 4");

            if (veryVerbose) {
                mX.print(cout);
            } else if (verbose) mX.print(cout, 0, -1) << endl;
        }

        {
            u::RandGen randGen;
            const bsl::string s("woof", &ta);

            enum { k_NUM_VALS = 10 };

            for (int len = 0; len < 30; ++len) {
                int numNodesForKey[k_NUM_VALS] = { 0 };

                Obj mX(&ta);    const Obj& X = mX;

                for (int ii = 0; ii < len; ++ii) {
                    int key  = randGen() % k_NUM_VALS;

                    mX.addRaw(0, key, s);
                    ++numNodesForKey[key];
                }
                ASSERT(X.length() == len);

                for (int key = 0; key < k_NUM_VALS; ++key) {
                    {
                        Obj::PairHandle  ph, phF;
                        Obj::Pair       *pr = 0;

                        bool expected = false;
                        for (int jj = key + 1; jj < k_NUM_VALS; ++jj) {
                            if (numNodesForKey[jj]) {
                                expected = true;
                                break;
                            }
                        }

                        bool found    = (0 == X.findUpperBound(   &phF, key));
                        bool foundRaw = (0 == X.findUpperBoundRaw(&pr,  key));
                        ASSERT(found == foundRaw);
                        ASSERT(found == !!pr);
                        ASSERT(pr == phF);
                        ASSERT(found == expected);
                        if (found) {
                            ASSERT(key <  phF.key());
                            ASSERT(s   == phF.data());

                            Obj::PairHandle phSB = phF;
                            mX.skipBackward(&phSB);
                            if (phSB) {
                                ASSERT(phSB.key() <= key);
                            }
                            mX.releaseReferenceRaw(pr);
                            phSB.release();
                        }

                        bool foundR    = (0 == X.findUpperBoundR(   &ph, key));
                        bool foundRRaw = (0 == X.findUpperBoundRRaw(&pr, key));
                        ASSERT(found == foundR);
                        ASSERT(found == foundRRaw);
                        ASSERT(found == !!pr);
                        ASSERT(phF == ph);
                        ASSERT(pr == ph);
                        ASSERT(found == expected);
                        if (found) {
                            ASSERT(key <  ph.key());
                            ASSERT(s   == ph.data());
                            mX.releaseReferenceRaw(pr);
                            ph.release();
                        }
                    }

                    {
                        Obj::PairHandle  ph, phF;
                        Obj::Pair       *pr = 0;

                        bool expected = false;
                        for (int jj = key; jj < k_NUM_VALS; ++jj) {
                            if (numNodesForKey[jj]) {
                                expected = true;
                                break;
                            }
                        }

                        bool found    = (0 == X.findLowerBound(   &phF, key));
                        bool foundRaw = (0 == X.findLowerBoundRaw(&pr,  key));
                        ASSERT(found == foundRaw);
                        ASSERT(found == !!pr);
                        ASSERT(pr == phF);
                        ASSERT(found == expected);
                        if (found) {
                            ASSERT(key <= phF.key());
                            ASSERT(s   == phF.data());

                            Obj::PairHandle phSB = phF;
                            mX.skipBackward(&phSB);
                            if (phSB) {
                                ASSERT(phSB.key() < key);
                            }
                            mX.releaseReferenceRaw(pr);
                            phSB.release();
                        }

                        bool foundR    = (0 == X.findLowerBoundR(   &ph, key));
                        bool foundRRaw = (0 == X.findLowerBoundRRaw(&pr, key));
                        ASSERT(found == foundR);
                        ASSERT(found == foundRRaw);
                        ASSERT(found == !!pr);
                        ASSERT(phF == ph);
                        ASSERT(pr == ph);
                        ASSERT(found == expected);
                        if (found) {
                            ASSERT(key <= ph.key());
                            ASSERT(s   == ph.data());
                            mX.releaseReferenceRaw(pr);
                            ph.release();
                        }
                    }
                }
            }
        }

        ASSERT(0 == da.numAllocations());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // findLowerBound / findUpperBound test
        // --------------------------------------------------------------------

        using namespace SKIPLIST_OLD_TEST_CASES_NAMEPSACE;

        if (verbose) cout << endl
                          << "findLowerBound/findUpperBound test" << endl
                          << "==================================" << endl;

        DATA VALUES1[] = {
            // line,  key,  data,  level
            { L_ , 1, "fwd 1", 0},
            { L_ , 1, "middle-fwd 1", 0},
            { L_ , 1, "middle-back  1 ", 0},
            { L_ , 1, "back 1", 0},
            { L_ , 3, "3", 1},
            { L_ , 0, "0", 2},
            { L_ , 2, "2", 3},
            { L_ , 4, "first 4", 4},
            { L_ , 4, "middle 4", 4},
            { L_ , 4, "last 4", 4},
            { L_ , 8, "first 8", 4},
            { L_ , 8, "last 8", 4},
        };

        bslma::TestAllocator ta(veryVeryVerbose);
        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        typedef bdlcc::SkipList<int, bsl::string> Obj;
        {
            Obj mX(&ta);
            unsigned int size = sizeof(VALUES1)/sizeof(VALUES1[0]);
            for (unsigned int i = 0; i < size; ++i) {
                mX.addR(VALUES1[i].key, VALUES1[i].data);
            }

            Obj::PairHandle h;
            //validate error when trying to find something greater than max key
            ASSERT(mX.findLowerBound(&h, 10));
            ASSERT(mX.findUpperBound(&h, 10));

            //validate success when trying to find something smaller
            //than min key
            ASSERT(!mX.findLowerBound(&h,-1));
            ASSERT(h.key() == 0);
            ASSERT(h.data() == "0");

            ASSERT(!mX.findUpperBound(&h,-1));
            ASSERT(h.key() == 0);
            ASSERT(h.data() == "0");

            //check front edge
            ASSERT(!mX.findUpperBound(&h, 0));
            ASSERT(h.key() == 1);
            ASSERT(h.data() == "fwd 1");

            ASSERT(!mX.findLowerBound(&h, 0));
            ASSERT(h.key() == 0);
            ASSERT(h.data() == "0");

            //check somewhere inside of list
            ASSERT(!mX.findLowerBound(&h,6));
            ASSERT(h.key() == 8);
            ASSERT(h.data() == "first 8");

            ASSERT(!mX.findUpperBound(&h,6));
            ASSERT(h.key() == 8);
            ASSERT(h.data() == "first 8");

            //check back edge
            ASSERT(mX.findUpperBound(&h, 8));

            ASSERT(!mX.findLowerBound(&h, 8));
            ASSERT(h.key() == 8);
            ASSERT(h.data() == "first 8");

            //check middle
            ASSERT(!mX.findUpperBound(&h, 4));
            ASSERT(h.key() == 8);
            ASSERT(h.data() == "first 8");

            ASSERT(!mX.findLowerBound(&h, 4));
            ASSERT(h.key() == 4);
            ASSERT(h.data() == "first 4");

            if (veryVerbose) {
                mX.print(cout);
            } else if (verbose) mX.print(cout, 0, -1) << endl;
        }

        V(da.numBytesInUse());
        ASSERT(0 == da.numBytesInUse());
        V(ta.numBytesInUse());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // DISTRIBUTION TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nDISTRIBUTION TEST"
                               << "\n=================" << bsl::endl;

        enum { k_NUM_ITERATIONS = 600000 };

        typedef bdlcc::SkipList<int,int> Obj;
        typedef Obj::PairHandle H;

        Obj list;

        int n[32] = {0};

        for (int i=0; i<k_NUM_ITERATIONS; i++) {
            H h;
            list.add(&h, 0, 0);
            int l = list.level(h);
            ASSERT(l>=0);
            ASSERT(l<32);
            n[l]++;
            list.remove(h);
        }

        double exp = (k_NUM_ITERATIONS / 4) * 3;
        double tolPercent = 5;    // tolerance

        for (int i=0; i<32; i++, exp /= 4) {
            int miss = static_cast<int>(
                                       100 * u::myAbs(n[i] - exp) / exp + 0.5);
            miss *= n[i] > exp ? 1 : -1;

            if (verbose) {
                cout << "i: " << bsl::setw(2) << i << ", ";
                bsl::ios_base::fmtflags flags = cout.flags();
                cout.precision(4);
                cout.setf(bsl::ios_base::scientific,
                          bsl::ios_base::floatfield);
                cout << "exp: " << bsl::setw(6) << exp << ", " <<
                        "n[i]: " << bsl::setw(6) << n[i] << ", " <<
                        "tolerance: " << bsl::setw(6) << tolPercent << "%, ";
                cout.flags(flags);
                cout << "miss: " << bsl::setw(4) << miss << "%\n";
                (void) cout.precision();

            }

            const double tol = tolPercent / 100;
            ASSERTV(exp, n[i], tolPercent, n[i] <=
                                                bsl::ceil( exp * (1.0 + tol)));
            ASSERTV(exp, n[i], tolPercent, n[i] >=
                                                bsl::floor(exp * (1.0 - tol)));

            tolPercent = tol * 200;
        }

      }  break;
      case 18: {
        // --------------------------------------------------------------------
        // Value-semantic tests
        //
        // Doc box at beginning of defintion of function 'valueSemanticTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        valueSemanticTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::valueSemanticTest();
        TestDriver<int, bsltf::AllocTestType>::valueSemanticTest();
        TestDriver<int, bsltf::MovableAllocTestType>::valueSemanticTest();
        TestDriver<int, bsltf::SimpleTestType>::valueSemanticTest();
      }  break;
      case 17: {
        // --------------------------------------------------------------------
        // Testing 'skipBackward*' and 'skipForward*'
        //
        // Doc box at beginning of defintion of function 'skipTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        skipTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::skipTest();
        TestDriver<int, bsltf::AllocTestType>::skipTest();
        TestDriver<int, bsltf::MovableAllocTestType>::skipTest();
        TestDriver<int, bsltf::SimpleTestType>::skipTest();
      } break;
      case 16: {
        // ----------------------------------------------------
        // 'addPairReferenceRaw' thread safety test
        // ----------------------------------------------------
        if (verbose) cout << endl
                          << "addPairReferenceRaw Safety test" << endl
                          << "===============================" << endl;

        namespace TC = ReferenceTest;

        enum {
            k_NUM_THREADS = 5
        };

        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        bslma::TestAllocator ta;

        TC::Obj            mX(&ta);
        bslmt::ThreadGroup tg(&ta);

        mX.add(1, u::CountedDelete());

        int numThreads = k_NUM_THREADS;
        if (verbose) {
            numThreads = atoi(argv[2]);
            if (numThreads == 0) {
                numThreads = k_NUM_THREADS;
            }
            else {
                cout << "Running with " << numThreads << " threads."
                     << endl;
            }
        }
        int maxRefCount = 4000 / numThreads;

        tg.addThreads(bdlf::BindUtil::bind(&TC::referenceTestFunc,
                                           &mX,
                                           maxRefCount),
                      numThreads);
        tg.joinAll();

        ASSERT(k_NUM_THREADS == TC::masterId);

        ASSERTV(u::CountedDelete::getDeleteCount(),
                                      0 == u::CountedDelete::getDeleteCount());

        mX.popFront();

        ASSERTV(u::CountedDelete::getDeleteCount(),
                                      1 == u::CountedDelete::getDeleteCount());

        ASSERT(0 == da.numAllocations());
        ASSERT(0 <  ta.numAllocations());
      } break;
      case 15: {
        // ------------------------------------
        // 'removeAll' thread safety test
        // ------------------------------------

        namespace TC = RemoveAllSafetyTest;

        enum {
            k_NUM_THREADS = 20,
            k_NUM_ITERATIONS = 1000
        };

        int numThreads = k_NUM_THREADS;
        if (verbose) {
            numThreads = atoi(argv[2]);
            if (numThreads == 0) {
                numThreads = k_NUM_THREADS;
            }
            verbose = veryVerbose;
            veryVerbose = veryVeryVerbose;
            veryVeryVerbose = veryVeryVeryVerbose;
            veryVeryVeryVerbose = false;

            if (verbose) argv[2] = argv[3];
        }

        bslmt::Barrier barrier(numThreads);
        TC::barrier_p = &barrier;

        int numIterations = k_NUM_ITERATIONS;
        if (verbose) {
            numIterations = atoi(argv[2]);
            if (numIterations == 0) {
                numIterations = k_NUM_ITERATIONS;
            }

            verbose = veryVerbose;
            veryVerbose = veryVeryVerbose;
            veryVeryVerbose = false;
        }

        if (verbose) {
            cout << "RemoveAll Safety test: ";
            P_(numThreads);    P(numIterations);
            cout << "======================================================\n";
        }

        bsls::Stopwatch sw;
        if (verbose) sw.start();

        bslma::TestAllocator da(veryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultAllocGuard(&da);
        bslma::TestAllocator ta;
        TC::alloc_p = &ta;

        TC::Obj mX(&ta);
        bslmt::ThreadGroup tg(&ta);

        int totalCollisions = 0;
        for (int mi = u::e_BEGIN; mi < u::e_END2; ++mi) {
            u::AddMode mode = static_cast<u::AddMode>(mi);

            if (veryVerbose) cout << mode << endl;

            TC::masterId = 0;
            TC::collisions = 0;
            TC::removed = 0;
            tg.addThreads(bdlf::BindUtil::bind(&TC::removeAllSafetyFunc,
                                               &mX,
                                               numIterations,
                                               mode),
                          numThreads);
            tg.joinAll();
            ASSERT(mX.isEmpty());
            ASSERT(TC::masterId == numThreads);
            ASSERTV(mode,
                     TC::removed, TC::collisions, TC::removed + TC::collisions,
                                                    numThreads * numIterations,
                   TC::removed + TC::collisions == numThreads * numIterations);

            switch (mode) {
              case u::e_ADD_AT_LEVEL_UNIQUE_RAW:        BSLA_FALLTHROUGH;
              case u::e_ADD_AT_LEVEL_UNIQUE_RAW_R:      BSLA_FALLTHROUGH;
              case u::e_ADD_AT_LEVEL_UNIQUE_RAW_PAIR:   BSLA_FALLTHROUGH;
              case u::e_ADD_AT_LEVEL_UNIQUE_RAW_R_PAIR: BSLA_FALLTHROUGH;
              case u::e_ADD_UNIQUE_RAW:                 BSLA_FALLTHROUGH;
              case u::e_ADD_UNIQUE_RAW_R:               BSLA_FALLTHROUGH;
              case u::e_ADD_UNIQUE_RAW_PAIR:            BSLA_FALLTHROUGH;
              case u::e_ADD_UNIQUE_RAW_R_PAIR:          BSLA_FALLTHROUGH;
              case u::e_ADD_UNIQUE:                     BSLA_FALLTHROUGH;
              case u::e_ADD_UNIQUE_R:                   BSLA_FALLTHROUGH;
              case u::e_ADD_UNIQUE_HANDLE:              BSLA_FALLTHROUGH;
              case u::e_ADD_UNIQUE_R_HANDLE: {
                if (verbose || k_NUM_THREADS < numThreads) {
                    P_(TC::collisions);    P(mode);
                }
              } break;
              default: {
                ASSERT(0 == TC::collisions);
              } break;
            }

            totalCollisions += TC::collisions;

            // 'e_ADD_WITH_REMOVEALL_HANDLES' is the last mode tested.  This
            // overload of 'removeAll' uses the default allocator for an
            // internal temporary vector.  All previous modes should not use
            // the default allocator.

            ASSERT((u::e_ADD_WITH_REMOVEALL_HANDLES == mode) ==
                                                    (0 < da.numAllocations()));
        }

        ASSERT(0 <  ta.numAllocations());

        if (40 <= numThreads && 1000 <= numIterations) {
            ASSERTV(totalCollisions, numIterations / 4 <= totalCollisions);
        }
        if (verbose) {
            sw.stop();
            P_(totalCollisions);    P(sw.elapsedTime());
        }
      } break;
      case 14: {
        // ------------------------------------
        // 'remove' test
        // ------------------------------------

        if (verbose) cout << endl
                          << "Remove test" << endl
                          << "===========" << endl;

        typedef bdlcc::SkipList<int, u::CountedDelete> Obj;

        Obj list;
        {
            list.add(1000, u::CountedDelete());
        }
        Obj::PairHandle firstItem;
        ASSERT(0 == list.front(&firstItem));

        ASSERT(!list.isEmpty());

        list.remove(firstItem);
        ASSERT(list.isEmpty());
        ASSERT(0 == u::CountedDelete::getDeleteCount());
        firstItem.release();
        ASSERT(1 == u::CountedDelete::getDeleteCount());

        ASSERT(list.isEmpty());

        {
            list.add(900, u::CountedDelete());
        }
        ASSERT(0 == list.front(&firstItem));
        ASSERT(1 == list.length());

        firstItem.release();
        ASSERT(1 == u::CountedDelete::getDeleteCount());
        {
            list.add(1000, u::CountedDelete());
        }
        ASSERT(0 == list.front(&firstItem));
        list.remove(firstItem);
        ASSERT(1 == u::CountedDelete::getDeleteCount());

        ASSERT(0 == list.front(&firstItem));
        ASSERT(2 == u::CountedDelete::getDeleteCount());

        list.remove(firstItem);

        ASSERT(list.isEmpty());
        ASSERT(2 == u::CountedDelete::getDeleteCount());
        firstItem.release();
        ASSERT(3 == u::CountedDelete::getDeleteCount());

      } break;
      case 13: {
        // ------------------------------------------
        // Node deallocation thread safety test (TBD)
        // ------------------------------------------
        if (verbose) cout << endl
                          << "Node deallocation thread safety test" << endl
                          << "====================================" << endl;

        bslma::TestAllocator ta(veryVeryVerbose);
        {
            typedef bdlcc::SkipList<int, int> Obj;
            Obj list(&ta);

            bsls::AtomicInt done(0);

            // TBD
        }
        ASSERT(0 < ta.numAllocations());
        ASSERT(0 == ta.numBytesInUse());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // updateR
        //
        // Doc box at beginning of defintion of function 'updateRTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        updateRTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::updateRTest();
        TestDriver<int, bsltf::AllocTestType>::updateRTest();
        TestDriver<int, bsltf::MovableAllocTestType>::updateRTest();
        TestDriver<int, bsltf::SimpleTestType>::updateRTest();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // ADDR TEST
        //
        // Doc box at beginning of defintion of function 'addRTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        addRTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::addRTest();
        TestDriver<int, bsltf::AllocTestType>::addRTest();
        TestDriver<int, bsltf::MovableAllocTestType>::addRTest();
        TestDriver<int, bsltf::SimpleTestType>::addRTest();
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // REMOVEALL TEST
        //
        // Doc box at beginning of defintion of function 'removeAllTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        removeAllTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::removeAllTest();
        TestDriver<int, bsltf::AllocTestType>::removeAllTest();
        TestDriver<int, bsltf::MovableAllocTestType>::removeAllTest();
        TestDriver<int, bsltf::SimpleTestType>::removeAllTest();
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // findR test
        //
        // Doc box at beginning of defintion of function 'findRTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        findRTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::findRTest();
        TestDriver<int, bsltf::AllocTestType>::findRTest();
        TestDriver<int, bsltf::MovableAllocTestType>::findRTest();
        TestDriver<int, bsltf::SimpleTestType>::findRTest();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // FIND TEST
        //
        // Doc box at beginning of defintion of function 'findTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        findTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::findTest();
        TestDriver<int, bsltf::AllocTestType>::findTest();
        TestDriver<int, bsltf::MovableAllocTestType>::findTest();
        TestDriver<int, bsltf::SimpleTestType>::findTest();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TIMER TEST
        //
        // Apparently just test that the anticipated 'EventTimeQueue' type
        // based on a skip list calling the 'add' method can compile.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "timer test" << endl
                          << "==========" << endl;

        typedef bdlcc::SkipList<bsls::TimeInterval, bsl::function<void()> >
                                                               EventTimeQueue;
        typedef EventTimeQueue::PairHandle                     EventHandle;

        bsls::TimeInterval    timer;
        bsl::function<void()> callback;

        EventTimeQueue eventTimeQueue;
        EventHandle h;
        eventTimeQueue.add(&h, timer, callback);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // ITERATION TEST
        //
        // Doc box at beginning of defintion of function 'iterationTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        iterationTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::iterationTest();
        TestDriver<int, bsltf::AllocTestType>::iterationTest();
        TestDriver<int, bsltf::MovableAllocTestType>::iterationTest();
        TestDriver<int, bsltf::SimpleTestType>::iterationTest();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // UPDATE TEST
        //
        // Doc box at beginning of defintion of function 'updateTest'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "update test" << endl
                          << "=============" << endl;

        U_RUN_EACH_TYPE(TestDriver,
                        updateTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::updateTest();
        TestDriver<int, bsltf::AllocTestType>::updateTest();
        TestDriver<int, bsltf::MovableAllocTestType>::updateTest();
        TestDriver<int, bsltf::SimpleTestType>::updateTest();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // NEWFRONT TEST
        //
        // Doc box at beginning of defintion of function 'newFrontTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        newFrontTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::newFrontTest();
        TestDriver<int, bsltf::AllocTestType>::newFrontTest();
        TestDriver<int, bsltf::MovableAllocTestType>::newFrontTest();
        TestDriver<int, bsltf::SimpleTestType>::newFrontTest();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Allocation Test
        //
        // Doc box at beginning of defintion of function 'allocationTest'.
        // --------------------------------------------------------------------

        U_RUN_EACH_TYPE(TestDriver,
                        allocationTest,
                        U_TEST_TYPES_REGULAR);

        TestDriver<int, TTF::MethodPtr>::allocationTest();
        TestDriver<int, bsltf::AllocTestType>::allocationTest();
        TestDriver<int, bsltf::MovableAllocTestType>::allocationTest();
        TestDriver<int, bsltf::SimpleTestType>::allocationTest();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Usage Example" << endl
                          << "=============" << endl;


        using namespace USAGE;

        {
//..
// Now, in 'main', verify that the scheduler executes events when expected:
//..
    SimpleScheduler      scheduler;

    bsl::vector<int>     values;

    const bdlt::Datetime start = bdlt::CurrentTime::utc();
    bdlt::Datetime       scheduleTime;
//..
// Add events out of sequence and ensure they are executed in the proper order.
//..
if (veryVerbose) cout << "Start:      " << start << endl;

    scheduleTime = start;
    scheduleTime.addMilliseconds(2250);
if (veryVerbose) cout << "scheduling: " << scheduleTime << endl;
    scheduler.scheduleEvent(bdlf::BindUtil::bind(&pushBackWrapper, &values, 2),
                            scheduleTime);

    scheduleTime = start;
    scheduleTime.addMilliseconds(750);
if (veryVerbose) cout << "scheduling: " << scheduleTime << endl;
    scheduler.scheduleEvent(bdlf::BindUtil::bind(&pushBackWrapper, &values, 0),
                            scheduleTime);

    scheduleTime = start;
    scheduleTime.addMilliseconds(1500);
if (veryVerbose) cout << "scheduling: " << scheduleTime << endl;
    scheduler.scheduleEvent(bdlf::BindUtil::bind(&pushBackWrapper, &values, 1),
                            scheduleTime);

    ASSERT(values.empty());

    scheduler.drain();

    bdlt::Datetime finish = bdlt::CurrentTime::utc();
if (veryVerbose) cout << "Finish:     " << finish << endl;

    ASSERT(3 == values.size());
    ASSERT(0 == values[0]);
    ASSERT(1 == values[1]);
    ASSERT(2 == values[2]);

    const double elapsed = bdlt::IntervalConversionUtil::convertToTimeInterval(
                                        finish - start).totalSecondsAsDouble();

    ASSERT(2.25 <= elapsed);
    ASSERT(elapsed < 2.75);

if (veryVerbose) cout << "Elapsed: " << elapsed << " seconds\n";
//..
// Note that the destructor of 'scheduler' will call 'stop()'.
//..
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            typedef bdlcc::SkipList<int, const char *> Obj;

            Obj mX;    const Obj& X = mX;
            ASSERT(X.isEmpty());
            ASSERT(0 == X.length());
            mX.add(0, "Hi");
            ASSERT(!X.isEmpty());
            ASSERT(1 == X.length());
            Obj::Pair *h = 0;
            int ret = mX.popFrontRaw(&h);
            ASSERT(ret==0);
            ASSERT(X.isEmpty());
            ASSERTV(X.length(), 0 == X.length());
            bsl::string d = h->data();
            V(d);
            mX.releaseReferenceRaw(h);
            ASSERT(d=="Hi");

            typedef bdlcc::SkipList<int, bsl::string> Obj2;
            Obj2 mX2;        const Obj2& X2 = mX2;

            ASSERT(X2.isEmpty());
            mX2.add(3, "3");
            mX2.add(2, "2");
            mX2.add(0, "0");
            mX2.add(1, "1");
            ASSERT(4 == X2.length());
            ASSERT(!X2.isEmpty());

            Obj2::Pair *h2 = 0;
            ret = mX2.popFrontRaw(&h2);
            ASSERTV(X2.length(), 3 == X2.length());
            ASSERT(ret==0);

            V(h2->data());
            V(h2->key());

            ASSERT("0" == h2->data());
            mX2.releaseReferenceRaw(h2);

            ret = mX2.popFrontRaw(&h2);
            ASSERT(ret==0);
            V(h2->data());
            V(h2->key());
            ASSERT("1" == h2->data());
            mX2.releaseReferenceRaw(h2);

            ret = mX2.popFrontRaw(&h2);
            ASSERT(ret==0);
            ASSERT(1 == X2.length());
            V(h2->data());
            V(h2->key());
            ASSERT("2" == h2->data());
            mX2.releaseReferenceRaw(h2);

            Obj2::PairHandle h2h;
            ret = mX2.popFront(&h2h);
            ASSERT(ret==0);
            V(h2h.data());
            V(h2h.key());

            ASSERT("3" == h2h.data());

            ASSERT(0 == X2.length());
            ASSERT(X2.isEmpty());

            ret = mX2.popFront(&h2h);
            ASSERT(ret == Obj::e_NOT_FOUND);
        }
      } break;
      case -101: {
        // --------------------------------------------------------------------
        // The thread-safety test
        // --------------------------------------------------------------------
        SKIPLIST_OLD_TEST_CASES_NAMEPSACE::run();
      } break;
      case -100: {
        // --------------------------------------------------------------------
        // The router simulation (kind of) test
        // --------------------------------------------------------------------
        SKIPLIST_TEST_CASE_MINUS_100::run();
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
