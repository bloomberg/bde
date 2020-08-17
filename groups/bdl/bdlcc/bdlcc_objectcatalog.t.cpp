// bdlcc_objectcatalog.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlcc_objectcatalog.h>

#include <bdlf_bind.h>

#include <bdlt_currenttime.h>

#include <bsla_unused.h>
#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>
#include <bslma_testallocator.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_isfundamental.h>
#include <bslmf_issame.h>
#include <bslmt_barrier.h>
#include <bslmt_lockguard.h>
#include <bslmt_condition.h>
#include <bslmt_mutex.h>
#include <bslmt_testutil.h>
#include <bslmt_threadutil.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_nameof.h>
#include <bsls_review.h>
#include <bsls_types.h>
#include <bsltf_movestate.h>
#include <bsltf_streamutil.h>
#include <bsltf_templatetestfacility.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_queue.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Testing is divided into the following parts (apart from the breathing test):
// An alternate implementation (named PseudoObjectCatalog) for
// 'bdlcc::ObjectCatalog' is provided just for testing purpose.  It is tested
// in
// [ 2].  It is used in the later test cases to verify the
// 'bdlcc::ObjectCatalog'.
//
// We have chosen the primary manipulators for 'bdlcc::ObjectCatalog' as 'add'
// and 'remove'.  They are tested in [3].  Test cases [4], [5], [6] and [7]
// test the remaining manipulators of 'bdlcc::ObjectCatalog'.
//
// [9] tests all the accessors of 'bdlcc::ObjectCatalog' (including access
// through iterator).
//
// [8] tests the 'bdlcc::ObjectCatalogIter' class.
//
// All the test cases above are for a single thread.  [12] verifies that the
// catalog remain consistent in the presence of multiple threads accessing it
// (either directly or through iteration).
//
// [10] verifies that stale handles are rejected properly by catalog.
//
// [10] verifies that the objects are constructed and destroyed properly by
// the catalog.
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] bdlcc::ObjectCatalog(bslma::Allocator *allocator=0);
// [ 3] ~bdlcc::ObjectCatalog();
//
// MANIPULATORS
// [ 3] int add(TYPE const& object);
// [15] int add(TYPE&&);
// [15] void removeAll(bsl::vector<TYPE> *);
// [ 3] int remove(int handle, TYPE* valueBuffer=0);
// [ 5] int replace(int handle, TYPE const &newObject);
// [15] int replace(int, TYPE&&);
// [ 6] int remove(int handle);
// [ 7] int remove(int handle, TYPE* valueBuffer=0);
// [ 8] void removeAll();
//
// ACCESSORS
// [10] int find(int, TYPE *) const;
// [10] int length() const;
// [15] bslma::Allocator *allocator() const;
// [15] bool isMember(const TYPE&) const;
// [15] const TYPE& value(int) const;
// [15] int find(int) const;
//-----------------------------------------------------------------------------
// CREATORS
// [ 9] bdlcc::ObjectCatalogIter(const bdlcc::ObjectCatalog<TYPE>&);
// [ 9] ~bdlcc::ObjectCatalogIter();
//
// MANIPULATORS
// [ 9] void operator++();
//
// ACCESSORS
// [ 9] operator const void *() const;
// [ 9] pair<int, TYPE> operator()() const;
// [15] int Iter::handle() const;
// [15] const TYPE& Iter::value() const;
//-----------------------------------------------------------------------------
// [1 ] BREATHING TEST
// [2 ] TESTING ALTERNATE IMPLEMENTATION
// [11] TESTING OBJECT CONSTRUCTION/DESTRUCTION
// [12] TESTING OBJECT CONSTRUCTION/DESTRUCTION WITH ALLOCATORS
// [13] TESTING STALE HANDLE REJECTION
// [14] CONCURRENCY TEST
// [16] USAGE EXAMPLE

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

// ============================================================================
//         GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef bsltf::TemplateTestFacility         TTF;
typedef bsltf::MoveState                    MoveState;
typedef bdlcc::ObjectCatalog<int>           Obj;

namespace {
namespace u {

// From the header file
enum {
    k_INDEX_MASK      = 0x007fffff
  , k_BUSY_INDICATOR  = 0x00800000
  , k_GENERATION_INC  = 0x01000000
  , k_GENERATION_MASK = 0xff000000
  , k_GENERATION_SHIFT = 24 // static_log2(k_GENERATION_INC)
  , k_RECYCLE_COUNT    = 256
};

template<class TYPE>
class PseudoObjectCatalog
    // This class provides an alternative implementation for
    // 'bdlcc::ObjectCatalog'.
{
    enum { k_MAX = 100 };
    struct {
        bsls::ObjectBuffer<TYPE> d_obj;
        int   d_valid;
    } d_arr[k_MAX];

    int d_length;
    int d_topIndex;
  public:
    // CREATORS
    PseudoObjectCatalog() : d_length(0), d_topIndex(-1)
    {
        for (int i=0;i<k_MAX;i++) {
            d_arr[i].d_valid = 0;
        }
    }

    ~PseudoObjectCatalog()
    {
        removeAll();
    }

    // MANIPULATORS
    int add(TYPE const& object)
    {
        BSLS_ASSERT(d_topIndex != k_MAX-1);

        new (d_arr[++d_topIndex].d_obj.address()) TYPE(object);
        d_arr[d_topIndex].d_valid = 1;
        d_length++;
        return d_topIndex;
    }

    int remove(int h, TYPE* valueBuffer=0)
    {
        if (h<0 || h>d_topIndex || d_arr[h].d_valid==0) {
            return -1;                                                // RETURN
        }

        if (valueBuffer != 0) {
            *valueBuffer = *d_arr[h].d_obj.address();
        }

        d_length--;
        d_arr[h].d_valid = 0;
        d_arr[h].d_obj.address()->~TYPE();

        return 0;
    }

    void removeAll()
    {
        for (int i=0; i<=d_topIndex; i++) {
            d_arr[i].d_valid = 0;
            d_arr[i].d_obj.address()->~TYPE();
        }
        d_length = 0;
        d_topIndex = -1;
    }

    int replace(int h, TYPE const &newObject)
    {
        if (h<0 || h>d_topIndex || d_arr[h].d_valid==0) {
            return -1;                                                // RETURN
        }

        d_arr[h].d_obj.address()->~TYPE();

        new (d_arr[h].d_obj.address()) TYPE(newObject);
        return 0;
    }

    // ACCESSORS
    int find(int h, TYPE *p=0) const
    {
        if (h<0 || h>d_topIndex || d_arr[h].d_valid==0) {
            return -1; //non zero                                     // RETURN
        }
        if (p != 0) {
            *p = *d_arr[h].d_obj.address();
        }

        return 0;
    }

    bool isMember(TYPE val) const
    {
        TYPE v;
        for (int i=0; i<=d_topIndex; i++) {
            if (find(i, &v) == 0 && v == val) return 1;               // RETURN
        }
        return 0;
    }

    int length() const
    {
        return d_length;
    }

    void verifyState() const
    {
      ASSERT(d_topIndex+1 >= d_length);
      ASSERT(d_length >= 0);

      int nBusy = 0;
      for (int i=0;i<=d_topIndex;i++) {
          if (d_arr[i].d_valid == 1) {
              nBusy++;
          }
      }
      ASSERT(d_length == nBusy);
    }
};

typedef PseudoObjectCatalog<int> my_Obj;

// Each entry of this array specifies a state of 'bdlcc::ObjectCatalog' object
// (say 'catalog').  When the 'catalog' is in the state specified by 'spec'
// then the following are true:
//
// 'catalog.d_nodes.size() == strlen(spec)'
//
// 'catalog.d_nodes[i]->d_handle & k_BUSY_INDICATOR != 0'
//               FOR   0 <= i < 'catalog.d_nodes.size()'
//               AND   'spec[i]' = '1'
//
// 'catalog.d_nodes[i]->d_handle & k_BUSY_INDICATOR == 0'
//               FOR   0 <= i < 'catalog.d_nodes.size()'
//               AND   'spec[i]' = '0'
//
const char *SPECS[] = {
    "",
    "0",
    "1",
    "00",
    "01",
    "10",
    "11",
    "000",
    "001",
    "010",
    "011",
    "100",
    "101",
    "110",
    "111",
    "0000",
    "0001",
    "0010",
    "0011",
    "0100",
    "0101",
    "0110",
    "0111",
    "1000",
    "1001",
    "1010",
    "1011",
    "1100",
    "1101",
    "1110",
    "1111"
};
const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

void printSpec(const char *spec)
{
    int len = static_cast<int>(strlen(spec));
    cout << "[" ;
    for (int i=0; i<len; ++i) {
        cout << ((spec[i] == '0') ? "free" : "busy")
             << ((i == len-1) ? "" : "|") ;
    }
    cout << "]" ;
}

void verifyMatch(Obj          *o1,
                 vector<int>&  handles1,
                 my_Obj       *o2,
                 vector<int>&  handles2,
                 int           maxHandles)
    // Verify that the specified 'o1' is correct by comparing it with the
    // specified 'o2'.
{
    int v1, v2;
    ASSERT(o1->length() == o2->length());

    ASSERT(o1->find(-1) != 0);
    ASSERT(o2->find(-1) != 0);
    ASSERT(o1->find(maxHandles) != 0);
    ASSERT(o2->find(maxHandles) != 0);

    for (int i = 0; i < maxHandles; ++i) {
        int r1 = o1->find(handles1[i], &v1);
        int r2 = o2->find(handles2[i], &v2);

        if (r2 !=0) {
            ASSERTV(i, r1 != 0);
        }
        else {
            ASSERTV(i, r1 == 0);
            ASSERTV(i, v1, v2, v1 == v2);
        }
    }
}

template <class TYPE>
void ggInt(bdlcc::ObjectCatalog<TYPE> *o1,
           vector<int>&                handles1,
           PseudoObjectCatalog<TYPE>  *o2,
           vector<int>&                handles2,
           const char                 *spec,
           const int                   gens = 0)
    // Bring the specified object 'o1' into the state specified by the
    // specified 'spec' by using primary manipulators 'add' and 'remove' only.
    // Same sequence of method invocation is applied to the specified 'o2'.
    // Handles returned by 'o1->add' are put into the specified 'handles1' and
    // handles returned by 'o2->add' are put into the specified 'handles2'.
{
    // First invoke 'add' 'strlen(spec)' times, this will cause first
    // 'strlen(spec)' entries of 'o1->d_nodes' to be busy.  Then invoke
    // 'remove' for all the entries corresponding to char '0' of the 'spec',
    // this will cause those entries to be freed.  Optionally, add and remove
    // the entry 'gens' times to bring the generation numbers to 'gens' for the
    // entries still present, and to 'gens + 1' for freed entries.

    int v1, v2;
    int len = static_cast<int>(strlen(spec));
    for (int i=0 ;i < len; ++i) {
        if (veryVerbose) {
            cout << "\thandles1[" << i << "] = o1->add(" << i << "); // "
                 << gens << " generations\n";
        }

        handles1[i] = o1->add(i);
        for (int j=0; j < gens; ++j) {
            int r = o1->remove(handles1[i], &v1);
            ASSERT(r  == 0);
            ASSERT(v1 == i);
            int h = o1->add(i);
            ASSERT((h & k_INDEX_MASK) == (handles1[i] & k_INDEX_MASK));
            handles1[i] = h;
        }

        ASSERT((handles1[i] & k_INDEX_MASK) == (unsigned)i);
        ASSERT((handles1[i] & k_BUSY_INDICATOR) == k_BUSY_INDICATOR);
        ASSERT((((unsigned)handles1[i]) >> k_GENERATION_SHIFT) ==
                                                     (gens % k_RECYCLE_COUNT));

        handles2[i] = o2->add(i);
        ASSERT(handles2[i] == i);
    }

    for (int i=0; i < len; ++i) {
        if (spec[i] == '0') {
            if (veryVerbose) {
                cout << "\tr = o1->remove(handles1[" << i << "], &v1);\n";
            }
            int r = o1->remove(handles1[i], &v1);
            ASSERT(r == 0);
            ASSERT(v1 == i);

            r = o2->remove(handles2[i], &v2);
            ASSERT(r == 0);
            ASSERT(v2 == i);
        }
    }
}

template <class TYPE, bool isFundamental = bsl::is_fundamental<TYPE>::value>
struct AccessData;

template <class TYPE>
struct AccessData<TYPE, true> {
    static
    int get(const TYPE& value)
    {
        return static_cast<int>(value);
    }

    static
    void set(TYPE *target, int value)
    {
        *target = static_cast<TYPE>(value);
    }
};

template <class TYPE>
struct AccessData<TYPE, false> {
    static
    int get(const TYPE& value)
    {
        return value.data();
    }

    static
    void set(TYPE *target, int value)
    {
        target->setData(value);
    }
};

template <>
struct AccessData<bsltf::EnumeratedTestType::Enum, false> {
    static
    int get(const bsltf::EnumeratedTestType::Enum& value)
    {
        return static_cast<int>(value);
    }

    static
    void set(bsltf::EnumeratedTestType::Enum *target, int value)
    {
        *target = static_cast<bsltf::EnumeratedTestType::Enum>(value);
    }
};

template <>
struct AccessData<TTF::MethodPtr, false> {
    static
    int get(const TTF::MethodPtr value)
    {
        return TTF::getIdentifier(value);
    }

    static
    void set(TTF::MethodPtr *target, int value)
    {
        bslma::Allocator *alloc = 0;
        TTF::emplace(target, value, alloc);
    }
};

template <>
struct AccessData<TTF::FunctionPtr, false> {
    static
    int get(const TTF::FunctionPtr value)
    {
        return TTF::getIdentifier(value);
    }

    static
    void set(TTF::FunctionPtr *target, int value)
    {
        bslma::Allocator *alloc = 0;
        TTF::emplace(target, value, alloc);
    }
};

template <class TYPE>
struct AccessData<TYPE *, false> {
    static
    int get(TYPE * value)
    {
        return TTF::getIdentifier(value);
    }

    static
    void set(TYPE **target, int value)
    {
        bslma::Allocator *alloc = 0;

        TTF::emplace(target, value, alloc);
    }
};

template <class ELEMENT>
bool isMemberValue(const bdlcc::ObjectCatalog<ELEMENT>& container,
                   int                                  value)
{
    bdlcc::ObjectCatalogIter<ELEMENT> it(container);
    for (; it; ++it) {
        if (AccessData<ELEMENT>::get(it.value()) == value) {
            break;
        }
    }

    const void *ret = it;
    return 0 != ret;
}

static struct Data {
    int         d_line;
    const char *d_spec;
    int         d_length;
} DATA[] = {
    { L_, "", 0 },
    { L_, "A", 0 },
    { L_, "AAA", 0 },
    { L_, "AB", 0 },
    { L_, "B", 0 },
    { L_, "C", 0 },
    { L_, "CCC", 0 },
    { L_, "ABC", 0 },
    { L_, "ABCDEFG", 0 },
    { L_, "AAABCDEEEFG", 0 },
    { L_, "ZZYYXXTTUU", 0 },
    { L_, "ABCDEFGABCDEFG", 0 }
};

enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

struct DATA_init {
    // CREATOR
    DATA_init();
} data_init;

DATA_init::DATA_init()
{
    for (int ii = 0; ii < k_NUM_DATA; ++ii) {
        DATA[ii].d_length = static_cast<int>(bsl::strlen(DATA[ii].d_spec));
        for (const char *pc = DATA[ii].d_spec; *pc; ++pc) {
            BSLS_ASSERT('A' <= *pc);
            BSLS_ASSERT(*pc <= 'Z');
        }
    }
};

                    // ======================================
                    // class WellBehavedMoveOnlyAllocTestType
                    // ======================================

class WellBehavedMoveOnlyAllocTestType {
    // This unconstrained (value-semantic) attribute class that uses a
    // 'bslma::Allocator' to supply memory and defines the type trait
    // 'bslma::UsesBslmaAllocator'.  This class is primarily provided to
    // facilitate testing of templates by defining a simple type representative
    // of user-defined types having an allocator.  This class does not have the
    // 'bsl::is_copy_constructible' trait, and it does have the
    // 'bslma::UsesBslmaAllocator' trait.

    // DATA
    int                     *d_data_p;       // pointer to the data value

    bslma::Allocator        *d_allocator_p;  // allocator used to supply memory
                                             // (held, not owned)

    void                    *d_self_p;       // pointer to self (to verify not
                                             // bit-wise moved)

    bsltf::MoveState::Enum   d_movedFrom;    // moved-from state

    bsltf::MoveState::Enum   d_movedInto;    // moved-to state

  private:
    // NOT IMPLEMENTED
    WellBehavedMoveOnlyAllocTestType& operator=(
                                      const WellBehavedMoveOnlyAllocTestType&);
    WellBehavedMoveOnlyAllocTestType(const WellBehavedMoveOnlyAllocTestType&);

  public:
    // CREATORS
    explicit WellBehavedMoveOnlyAllocTestType(
                                          bslma::Allocator *basicAllocator = 0)
                                                                   BSLA_UNUSED;
        // Create a 'WellBehavedMoveOnlyAllocTestType' object having the
        // (default) attribute values:
        //..
        //  data() == 0
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit WellBehavedMoveOnlyAllocTestType(
                                         int               data,
                                         bslma::Allocator *basicAllocator = 0);
        // Create a 'WellBehavedMoveOnlyAllocTestType' object having the
        // specified 'data' attribute value.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    WellBehavedMoveOnlyAllocTestType(
                 bslmf::MovableRef<WellBehavedMoveOnlyAllocTestType> original);
    WellBehavedMoveOnlyAllocTestType(
          bslmf::MovableRef<WellBehavedMoveOnlyAllocTestType>  original,
          bslma::Allocator                                    *basicAllocator);
        // Create a 'WellBehavedMoveAllocTestType' object having the same value
        // as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  After
        // construction, this object will be in a 'movedInto' state, and
        // 'original' will be in a 'movedFrom' state.  No allocations shall
        // occur (so no exception will be thrown) unless 'basicAllocator' is
        // not the currently installed default allocator.

    ~WellBehavedMoveOnlyAllocTestType();
        // Destroy this object.

    // MANIPULATORS
    WellBehavedMoveOnlyAllocTestType& operator=(
                      bslmf::MovableRef<WellBehavedMoveOnlyAllocTestType> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  If
        // 'rhs' is a reference to this object, there are no other effects;
        // otherwise, the object referenced by 'rhs' will be reset to a default
        // constructed state, 'rhs' shall be in a 'movedFrom' state, and this
        // object will be in a 'movedTo' state.  No allocations shall occur
        // (so no exception will be thrown) unless this object and 'rhs' have
        // different allocators.  Note that the moved-from state is specified,
        // rather than "valid but unspecified", as this type is intended for
        // verifying test drivers that want to ensure that moves occur
        // correctly where expected.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    void setMovedInto(bsltf::MoveState::Enum value);
        // Set the moved-into state of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

    bsltf::MoveState::Enum movedInto() const;
        // Return the move state of this object as target of a move operation.

    bsltf::MoveState::Enum movedFrom() const;
        // Return the move state of this object as source of a move operation.
};

// FREE OPERATORS
bool operator==(const WellBehavedMoveOnlyAllocTestType& lhs,
                const WellBehavedMoveOnlyAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'WellBehavedMoveOnlyAllocTestType'
    // objects have the same if their 'data' attributes are the same.

bool operator!=(const WellBehavedMoveOnlyAllocTestType& lhs,
                const WellBehavedMoveOnlyAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two
    // 'WellBehavedMoveOnlyAllocTestType' objects do not have the same value if
    // their 'data' attributes are not the same.

// FREE FUNCTIONS
void swap(WellBehavedMoveOnlyAllocTestType& a,
          WellBehavedMoveOnlyAllocTestType& b) BSLA_UNUSED;
void swap(WellBehavedMoveOnlyAllocTestType& a,
          WellBehavedMoveOnlyAllocTestType& b);
    // Exchange the states of the specified 'a' and 'b'.  If the allocators
    // match, both 'a' and 'b' will be left in a moved-into state, otherwise,
    // both will not.  If 'a' and 'b' are the same object, this function will
    // have no effect.  No allocations shall occur (so no exceptions will be
    // thrown) unless 'a' and 'b' have different allocators.

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                    // --------------------------------------
                    // class WellBehavedMoveOnlyAllocTestType
                    // --------------------------------------

// CREATORS
WellBehavedMoveOnlyAllocTestType::WellBehavedMoveOnlyAllocTestType(
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
    d_data_p = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = 0;
}

WellBehavedMoveOnlyAllocTestType::WellBehavedMoveOnlyAllocTestType(
                                              int               data,
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
    d_data_p = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = data;
}

WellBehavedMoveOnlyAllocTestType::WellBehavedMoveOnlyAllocTestType(
                  bslmf::MovableRef<WellBehavedMoveOnlyAllocTestType> original)
: d_allocator_p(bslmf::MovableRefUtil::access(original).d_allocator_p)
, d_self_p(this)
, d_movedInto(bsltf::MoveState::e_MOVED)
{
    WellBehavedMoveOnlyAllocTestType& lvalue = original;

    if (lvalue.d_data_p) {
        d_data_p = lvalue.d_data_p;
        lvalue.d_data_p = 0;
        lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
        d_movedFrom        = bsltf::MoveState::e_NOT_MOVED;
    }
    else {
        d_data_p = 0;

        // lvalue.d_movedFrom -- unchanged

        d_movedFrom = bsltf::MoveState::e_MOVED;
    }
}

WellBehavedMoveOnlyAllocTestType::WellBehavedMoveOnlyAllocTestType(
            bslmf::MovableRef<WellBehavedMoveOnlyAllocTestType> original,
            bslma::Allocator                                   *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    WellBehavedMoveOnlyAllocTestType& lvalue = original;

    if (d_allocator_p == lvalue.d_allocator_p) {
        if (lvalue.d_data_p) {
            d_data_p = lvalue.d_data_p;
            lvalue.d_data_p = 0;
            lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
            d_movedFrom        = bsltf::MoveState::e_NOT_MOVED;
        }
        else {
            d_data_p = 0;

            // lvalue.d_movedFrom -- unchanged

            d_movedFrom = bsltf::MoveState::e_MOVED;
        }
        d_movedInto     = bsltf::MoveState::e_MOVED;
    }
    else {
        d_data_p =
                 reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_data_p = lvalue.data();

        // lvalue.d_movedFrom -- unchanged

        d_movedFrom = bsltf::MoveState::e_NOT_MOVED;
        d_movedInto = bsltf::MoveState::e_NOT_MOVED;
    }
}

WellBehavedMoveOnlyAllocTestType::~WellBehavedMoveOnlyAllocTestType()
{
    d_allocator_p->deallocate(d_data_p);

    BSLS_ASSERT_OPT(!!d_data_p ==
                               (bsltf::MoveState::e_NOT_MOVED == d_movedFrom));

    // Ensure that this objects has not been bitwise moved.

    BSLS_ASSERT_OPT(this == d_self_p);
}

// MANIPULATORS
WellBehavedMoveOnlyAllocTestType&
WellBehavedMoveOnlyAllocTestType::operator=(
                       bslmf::MovableRef<WellBehavedMoveOnlyAllocTestType> rhs)
{
    WellBehavedMoveOnlyAllocTestType& lvalue = rhs;

    if (&lvalue != this) {
        if (d_allocator_p == lvalue.d_allocator_p) {
            if (lvalue.d_data_p) {
                if (d_data_p) {
                    d_allocator_p->deallocate(d_data_p);
                }
                d_data_p = lvalue.d_data_p;
                lvalue.d_data_p = 0;

                lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
                d_movedFrom        = bsltf::MoveState::e_NOT_MOVED;
            }
            else {
                if (d_data_p) {
                    d_allocator_p->deallocate(d_data_p);
                    d_data_p = 0;
                }

                // lvalue.d_movedFrom -- unchanged

                d_movedFrom = bsltf::MoveState::e_MOVED;
            }

            d_movedInto = bsltf::MoveState::e_MOVED;
        }
        else {
            int *newData = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
            if (d_data_p) {
                d_allocator_p->deallocate(d_data_p);
            }
            d_data_p = newData;
            *d_data_p = lvalue.data();

            // lvalue.d_movedFrom -- unchanged

            d_movedFrom = bsltf::MoveState::e_NOT_MOVED;
            d_movedInto = bsltf::MoveState::e_NOT_MOVED;
        }
    }
    return *this;
}

void WellBehavedMoveOnlyAllocTestType::setData(int value)
{
    if (!d_data_p) {
        int *newData = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        d_data_p = newData;
    }
    *d_data_p = value;

    d_movedFrom = bsltf::MoveState::e_NOT_MOVED;
    d_movedInto = bsltf::MoveState::e_NOT_MOVED;
}

inline
void WellBehavedMoveOnlyAllocTestType::setMovedInto(
                                                  bsltf::MoveState::Enum value)
{
    d_movedInto = value;
}

// ACCESSORS
inline
bslma::Allocator *WellBehavedMoveOnlyAllocTestType::allocator() const
{
    return d_allocator_p;
}

inline
int WellBehavedMoveOnlyAllocTestType::data() const
{
    return d_data_p ? *d_data_p : 0;
}

inline
bsltf::MoveState::Enum WellBehavedMoveOnlyAllocTestType::movedFrom() const
{
    return d_movedFrom;
}

inline
bsltf::MoveState::Enum WellBehavedMoveOnlyAllocTestType::movedInto() const
{
    return d_movedInto;
}

// FREE OPERATORS
inline
bool operator==(const WellBehavedMoveOnlyAllocTestType& lhs,
                const WellBehavedMoveOnlyAllocTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const WellBehavedMoveOnlyAllocTestType& lhs,
                const WellBehavedMoveOnlyAllocTestType& rhs)
{
    return lhs.data() != rhs.data();
}

// FREE FUNCTIONS
void swap(WellBehavedMoveOnlyAllocTestType& a,
          WellBehavedMoveOnlyAllocTestType& b)
{
    typedef bslmf::MovableRefUtil MRU;

    if (&a == &b) {
        return;                                                       // RETURN
    }

    WellBehavedMoveOnlyAllocTestType intermediate(MRU::move(a));
    a = MRU::move(b);
    b = MRU::move(intermediate);
}

template <class ELEMENT, int moveAware>
struct GetMoved;

template <class ELEMENT>
struct GetMoved<ELEMENT, 0> {

    // CLASS METHODS
    static
    MoveState::Enum from(const ELEMENT&)
    {
        return MoveState::e_UNKNOWN;
    }

    static
    MoveState::Enum into(const ELEMENT&)
    {
        return MoveState::e_UNKNOWN;
    }
};

template <class ELEMENT>
struct GetMoved<ELEMENT, 1> {

    // CLASS METHODS
    static
    MoveState::Enum from(const ELEMENT& object)
    {
        return object.movedFrom();
    }

    static
    MoveState::Enum into(const ELEMENT& object)
    {
        return object.movedInto();
    }
};

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bslma {

// TRAITS
template <>
struct UsesBslmaAllocator<u::WellBehavedMoveOnlyAllocTestType> : bsl::true_type
{};

}  // close namespace bslma
}  // close enterprise namespace

namespace bsl {
template <>
struct is_copy_constructible<u::WellBehavedMoveOnlyAllocTestType>
        : bsl::false_type {};
}  // close namespace bsl

template <class ELEMENT>
class TestDriver {
    // TYPES
    typedef bdlcc::ObjectCatalog<ELEMENT>       Obj;
    typedef bdlcc::ObjectCatalogIter<ELEMENT>   Iter;
    typedef u::PseudoObjectCatalog<ELEMENT>     PObj;
    typedef bslmf::MovableRefUtil               MUtil;

    enum { k_IS_MOVE_AWARE =
               bsl::is_same<ELEMENT,
                                 u::WellBehavedMoveOnlyAllocTestType>::value ||
                  bsl::is_same<ELEMENT, bsltf::MoveOnlyAllocTestType>::value ||
                   bsl::is_same<ELEMENT, bsltf::MovableAllocTestType>::value ||
                        bsl::is_same<ELEMENT, bsltf::MovableTestType>::value,
           k_IS_MOVABLE =
               bsl::is_same<ELEMENT,
                                 u::WellBehavedMoveOnlyAllocTestType>::value ||
                  bsl::is_same<ELEMENT, bsltf::MoveOnlyAllocTestType>::value ||
                   bsl::is_same<ELEMENT, bsltf::MovableAllocTestType>::value ||
                        bsl::is_same<ELEMENT, bsltf::MovableTestType>::value,
           k_IS_ALLOCATING = bslma::UsesBslmaAllocator<ELEMENT>::value
    };

    static const MoveState::Enum s_expMoved = k_IS_MOVE_AWARE
                                            ? bsltf::MoveState::e_MOVED
                                            : bsltf::MoveState::e_UNKNOWN;
        // Compare this to a move state of an object that we expect to have
        // been moved.

    static const MoveState::Enum s_expNotMoved = k_IS_MOVE_AWARE
                                               ? bsltf::MoveState::e_NOT_MOVED
                                               : bsltf::MoveState::e_UNKNOWN;
        // Compare this to a move state of an object that we expect to have not
        // been moved.

    // PRIVATE CLASS METHODS
    static ELEMENT *ampersand(ELEMENT& expression)
        // Return a non-'const' ptr to the specified 'expression'.
    {
        return bsls::Util::addressOf(expression);
    }

    static void setData(ELEMENT *target, int value)
        // Set the value of the specified '*target' to the specified 'value'.
    {
        u::AccessData<ELEMENT>::set(target, value);
    }

    static int getData(const ELEMENT& value)
        // Get the value, as an 'int' of the specified 'value'.
    {
        return u::AccessData<ELEMENT>::get(value);
    }

    static bslma::Allocator *getAllocatorImpl(const ELEMENT& value,
                                              bsl::true_type)
        // Return the address of the allocator used by the specified value.
        // Note that 'bsl::true' type indicates that the 'ELEMENT' type uses
        // 'bslma' allocators.
    {
        return value.allocator();
    }

    static bslma::Allocator *getAllocatorImpl(const ELEMENT&,
                                              bsl::false_type)
        // Return 0.  Note that 'bsl::false_type' indicates that the 'ELEMENT'
        // type does not use 'bslma' allocators.
    {
        return 0;
    }

    static bool usesAllocatorOrNoAllocator(const ELEMENT&    value,
                                           bslma::Allocator *allocator)
        // Return 'true' if the specified 'value' is an allocating type and it
        // uses the specified 'allocator' for memory allocator, or if 'ELEMENT'
        // is not an allocating type, and 'false' otherwise.
    {
        bslma::Allocator *valueAllocator =
                 getAllocatorImpl(value, bslma::UsesBslmaAllocator<ELEMENT>());
        return !valueAllocator || valueAllocator == allocator;
    }

    static bool hasSpecElements(const Obj&  container,
                                const char *spec);
        // Return 'true' if the specified 'container' contains elements
        // corresponding to those described by the specified 'spec', but not
        // necessarily in the same order, and 'false' otherwise.

    static void gg(Obj              *object,
                   const char       *spec,
                   bsl::vector<int> *handles = 0);
        // Populate the specified 'object' with elements set to values in the
        // specified 'spec', the values being equal to the ASCII values of
        // the characters in the string, and set 'handles' to contain the
        // handles for the correspending characters of "spec'.

    static
    MoveState::Enum getMovedInto(const ELEMENT& object)
    {
        return u::GetMoved<ELEMENT, k_IS_MOVE_AWARE>::into(object);
    }

    static
    MoveState::Enum getMovedFrom(const ELEMENT& object)
    {
        return u::GetMoved<ELEMENT, k_IS_MOVE_AWARE>::from(object);
    }

  public:
    static void testCaseManipulatorsCopyable();
        // Test all the manipulators and accessors assuming that the test type
        // is copyable.

    static void testCaseManipulatorsCopyOrMovable();
        // Test all the manipulators and accessors assuming that the test type
        // is copyable, copyable and movable, or move-only.

    static void testCaseApparatus();
        // Test the 'gg' function and 'u::isMember'.

    static void testCaseBreathingCopyOrMovable();
        // Run test case 1 with types that are copyable, copyable and movable,
        // or move-only.

    static void testCaseBreathingCopyable();
        // Run test case 1 with types that are copyable.
};

template <class ELEMENT>
bool TestDriver<ELEMENT>::hasSpecElements(const Obj&  container,
                                          const char *spec)
{
    char c = 0;
    bsl::string tmpSpecBuffer(spec);
    const bsl::size_t LENGTH = tmpSpecBuffer.length();

    char *tmpSpec = tmpSpecBuffer.empty() ? &c : &tmpSpecBuffer[0];

    unsigned ii = 0;
    for (Iter it(container); it; ++it, ++ii) {
        const ELEMENT& el = it.value();
        const int rawValue = getData(el);
        ASSERT(0 < rawValue);
        ASSERT(rawValue < 128);
        const char value = static_cast<char>(rawValue);

        char *pc = bsl::find(tmpSpec, tmpSpec + LENGTH, value);
        if (tmpSpec + LENGTH == pc) {
            return false;                                             // RETURN
        }
        *pc = 0;
    }

    return LENGTH == ii;
}

template <class ELEMENT>
void TestDriver<ELEMENT>::gg(Obj              *object,
                             const char       *spec,
                             bsl::vector<int> *handles)
{
    bslma::Allocator *alloc = object->allocator();

    object->removeAll();
    if (handles) {
        handles->clear();
        handles->reserve(bsl::strlen(spec));
    }

    for (; *spec; ++spec) {
        bsls::ObjectBuffer<ELEMENT> OV;
        TTF::emplace(OV.address(), *spec, alloc);
        int handle = object->add(MUtil::move(OV.object()));
        if (handles) {
            handles->push_back(handle);
        }
        OV.address()->~ELEMENT();
    }

    object->verifyState();
}

template <class ELEMENT>
void TestDriver<ELEMENT>::testCaseManipulatorsCopyOrMovable()
{
    const char *tName = bsls::NameOf<ELEMENT>().name();

    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVerbose);

    if (verbose) P(tName);

    Obj mX(&ta);    const Obj& X = mX;
    bsl::vector<int> handles(&ta);

    ASSERT(&ta == X.allocator());

    const char *spec = "ABCDEF";

    gg(&mX, spec, &handles);
    ASSERT(6 == X.length());

    int HA = handles[0];    (void) HA;
    int HB = handles[1];    (void) HB;
    int HC = handles[2];    (void) HC;
    int HD = handles[3];    (void) HD;
    int HE = handles[4];    (void) HE;
    int HF = handles[5];    (void) HF;

    for (int ii = 0; ii < X.length(); ++ii) {
        ASSERT(spec[ii] == getData(X.value(handles[ii])));
        if (k_IS_MOVE_AWARE) {
            MoveState::Enum movedInto = getMovedInto(X.value(handles[ii]));
            ASSERTV(tName, s_expMoved, movedInto, s_expMoved == movedInto);
        }
    }

    enum { k_NUM_V = 4 };
    bsls::ObjectBuffer<ELEMENT> obs[k_NUM_V];
    ELEMENT *vs = obs[0].address();        const ELEMENT *VS = vs;

    for (int ii = 0; ii < k_NUM_V; ++ii) {
        TTF::emplace(obs[ii].address(), '0' + ii, &ta);
        ASSERT(getData(VS[ii]) == '0' + ii);
    }

    const int H0 = mX.add(MUtil::move(vs[0]));
    ASSERT(0 != H0);

    MoveState::Enum movedFrom = getMovedFrom(VS[0]);
    ASSERTV(tName, movedFrom, s_expMoved == movedFrom);
    MoveState::Enum movedInto = getMovedInto(X.value(H0));
    ASSERTV(tName, movedInto, s_expMoved == movedInto);
    if (k_IS_MOVABLE) {
        setData(ampersand(vs[0]), '0');
    }
    ASSERT(VS[0] == X.value(H0));
    ASSERT(7 == X.length());

    spec = "ABCDEF0";
    ASSERT(hasSpecElements(X, spec));

    mX.remove(HC, ampersand(vs[1]));
    ASSERTV(s_expMoved == getMovedInto(VS[1]));
    ASSERT(getData(VS[1]) == 'C');
    setData(ampersand(vs[1]), '1');

    spec = "ABDEF0";
    ASSERT(hasSpecElements(X, spec));

    int rc = mX.remove(HD);
    ASSERT(0 == rc);

    spec = "ABEF0";
    ASSERT(hasSpecElements(X, spec));

    rc = mX.remove(HD);
    ASSERT(0 != rc);

    rc = mX.remove(HF, ampersand(vs[3]));
    movedInto = getMovedInto(VS[3]);
    ASSERTV(tName, movedInto, s_expMoved, s_expMoved == movedInto);
    ASSERT(0 == rc);
    ASSERT('F' == getData(VS[3]));
    setData(ampersand(vs[3]), '3');

    spec = "ABE0";
    ASSERT(hasSpecElements(X, spec));

    rc = mX.replace(HE, MUtil::move(vs[1]));
    movedFrom = getMovedFrom(VS[1]);
    ASSERTV(tName, movedFrom, s_expMoved == movedFrom);
    movedInto = getMovedInto(X.value(HE));
    ASSERTV(tName, movedInto, s_expMoved == movedInto);

    ASSERT(0 == rc);
    setData(ampersand(vs[1]), '1');
    ASSERT(X.isMember(VS[1]));

    rc = mX.replace(HD, MUtil::move(vs[2]));
    ASSERT(0 != rc);
    movedFrom = getMovedFrom(VS[2]);
    ASSERTV(tName, s_expNotMoved, movedFrom, s_expNotMoved == movedFrom);

    ASSERT('2' == getData(VS[2]));
    ASSERT(!X.isMember(VS[2]));

    spec = "AB10";
    ASSERT(hasSpecElements(X, spec));

    mX.removeAll();
    ASSERT(0 == X.length());
    ASSERT(hasSpecElements(X, ""));

    spec = "ABCDEF";
    gg(&mX, spec, &handles);
    ASSERT(hasSpecElements(X, spec));
    ASSERT(6 == X.length());

    for (int ii = 0; ii < X.length(); ++ii) {
        const ELEMENT& E = X.value(handles[ii]);
        const int val = getData(E);
        movedInto = getMovedInto(E);
        ASSERTV(tName, s_expMoved, movedInto, s_expMoved == movedInto);
        ASSERT('A' <= val && val <= 'F');
        ASSERT(spec[ii] == val);
        ASSERT(usesAllocatorOrNoAllocator(E, &ta));
        ASSERT(0 == X.find(handles[ii]));
        ASSERT(X.isMember(E));
    }

    int numItems = 0;
    for (Iter it(X); it; ++it, ++numItems) {
        const int h = it.handle();
        const ELEMENT& E = it.value();
        movedInto = getMovedInto(E);
        ASSERTV(tName, s_expMoved, movedInto, s_expMoved == movedInto);
        const int val = getData(E);
        ASSERT('A' <= val && val <= 'F');
        ASSERT(usesAllocatorOrNoAllocator(E, &ta));
        ASSERT(0 == X.find(h));
        ASSERT(X.isMember(E));
    }
    ASSERT(6 == numItems);

    for (int ii = 0; ii < k_NUM_V; ++ii) {
        vs[ii].~ELEMENT();
    }
}

template <class ELEMENT>
void TestDriver<ELEMENT>::testCaseManipulatorsCopyable()
{
    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVerbose);
    bslma::Allocator&    td = *bslma::Default::allocator();

    if (verbose) P(bsls::NameOf<ELEMENT>());

    Obj mX(&ta);    const Obj& X = mX;
    bsl::vector<int> handles(&ta);

    ASSERT(&ta == X.allocator());

    const char *spec = "ABCDEF";

    gg(&mX, spec, &handles);
    ASSERT(6 == X.length());

    int HA = handles[0];    (void) HA;
    int HB = handles[1];    (void) HB;
    int HC = handles[2];    (void) HC;
    int HD = handles[3];    (void) HD;
    int HE = handles[4];    (void) HE;
    int HF = handles[5];    (void) HF;

    for (int ii = 0; ii < X.length(); ++ii) {
        ASSERT(spec[ii] == getData(X.value(handles[ii])));
    }

    enum { k_NUM_V = 4 };
    bsls::ObjectBuffer<ELEMENT> obs[k_NUM_V];
    ELEMENT *vs = obs[0].address();        const ELEMENT *VS = vs;

    for (int ii = 0; ii < k_NUM_V; ++ii) {
        TTF::emplace(obs[ii].address(), '0' + ii, &ta);
        ASSERT(getData(VS[ii]) == '0' + ii);
    }

    const int H0 = mX.add(VS[0]);
    ASSERT(0 != H0);
    ASSERT('0' == getData(VS[0]));    // not moved from
    ASSERT(VS[0] == X.value(H0));
    ASSERT(7 == X.length());

    spec = "ABCDEF0";
    ASSERT(hasSpecElements(X, spec));
    ASSERT(0 == X.find(HD, ampersand(vs[1])));
    ASSERT(getData(VS[1]) == 'D');
    ASSERT(hasSpecElements(X, spec));

    setData(ampersand(vs[1]), '1');
    mX.remove(HC, ampersand(vs[1]));
    ASSERT(getData(VS[1]) == 'C');
    setData(ampersand(vs[1]), '1');

    spec = "ABDEF0";
    ASSERT(hasSpecElements(X, spec));

    int rc = mX.remove(HD);
    ASSERT(0 == rc);

    spec = "ABEF0";
    ASSERT(hasSpecElements(X, spec));

    rc = mX.remove(HD);
    ASSERT(0 != rc);

    rc = mX.remove(HF, ampersand(vs[3]));
    ASSERT(0 == rc);
    ASSERT('F' == getData(VS[3]));
    setData(ampersand(vs[3]), '3');

    spec = "ABE0";
    ASSERT(hasSpecElements(X, spec));

    rc = mX.replace(HE, VS[1]);
    ASSERT(0 == rc);
    ASSERT(X.isMember(VS[1]));

    rc = mX.replace(HD, VS[2]);
    ASSERT(0 != rc);
    ASSERT(!X.isMember(VS[2]));

    spec = "AB10";
    ASSERT(hasSpecElements(X, spec));

    mX.removeAll();
    ASSERT(0 == X.length());
    ASSERT(hasSpecElements(X, ""));

    spec = "ABCDEF";
    gg(&mX, spec, &handles);
    ASSERT(hasSpecElements(X, spec));
    ASSERT(6 == X.length());

    for (int ii = 0; ii < X.length(); ++ii) {
        const ELEMENT& E = X.value(handles[ii]);
        const int val = getData(E);
        ASSERT('A' <= val && val <= 'F');
        ASSERT(spec[ii] == val);
        ASSERT(usesAllocatorOrNoAllocator(E, &ta));
        ASSERT(0 == X.find(handles[ii]));
        ASSERT(0 == X.find(handles[ii], ampersand(vs[0])));
        ASSERT(E == VS[0]);
        ASSERT(val == getData(VS[0]));
        ASSERT(X.isMember(VS[0]));
        setData(ampersand(vs[0]), '0');
    }

    int numItems = 0;
    for (Iter it(X); it; ++it, ++numItems) {
        const int h = it.handle();
        const ELEMENT& E = it.value();
        const int val = getData(E);
        ASSERT('A' <= val && val <= 'F');
        ASSERT(usesAllocatorOrNoAllocator(E, &ta));
        ASSERT(0 == X.find(h));
        ASSERT(0 == X.find(h, ampersand(vs[0])));
        ASSERT(E == VS[0]);
        ASSERT(val == getData(VS[0]));
        ASSERT(X.isMember(VS[0]));
        setData(ampersand(vs[0]), '0');

        const bsl::pair<int, ELEMENT>& pr = it();
        ASSERT(usesAllocatorOrNoAllocator(pr.second, &td));
    }
    ASSERT(6 == numItems);

    for (int ii = 0; ii < k_NUM_V; ++ii) {
        vs[ii].~ELEMENT();
    }
}

template <class ELEMENT>
void TestDriver<ELEMENT>::testCaseApparatus()
{
    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVerbose);

    if (verbose) P(bsls::NameOf<ELEMENT>());

    Obj mX(&ta);    const Obj& X = mX;

    bsls::ObjectBuffer<ELEMENT> oV;
    ELEMENT& v = oV.object();    const ELEMENT& V = v;
    TTF::emplace(oV.address(), '5', &ta);

    for (int ti = 0; ti < u::k_NUM_DATA; ++ti) {
        const u::Data&  data   = u::DATA[ti];
        const char     *SPEC   = data.d_spec;
        const int       LENGTH = data.d_length;

        if (veryVerbose) { T_; P(SPEC); }

        ASSERT(0 == ti || 0 < LENGTH);
        ASSERT(0 == SPEC[LENGTH]);

        // The logic here assumes '5' never occurs in the spec.  'DATA_init'
        // has ensured all the specs consist of nothing but upper case alphas.

        ASSERT(SPEC + LENGTH == bsl::find(SPEC, SPEC + LENGTH, '5'));
        ASSERT(SPEC + LENGTH == bsl::find(SPEC, SPEC + LENGTH, '7'));

        const int startLength = X.length();

        int vHandle = mX.add(MUtil::move(v));
        setData(ampersand(v), '5');
        ASSERT(0 != vHandle);
        ASSERT(startLength + 1 == X.length());
        ASSERT(v == X.value(vHandle));
        ASSERT('5' == getData(X.value(vHandle)));
        ASSERT(X.isMember(V));

        bsl::vector<int> handles(&tb);

        gg(&mX, SPEC, &handles);

        // Verify that 'gg' clears values previously in the catalog.

        ASSERT(!X.isMember(V));
        ASSERT(!u::isMemberValue(X, '5'));

        ASSERT(hasSpecElements(X, SPEC));

        {
            // Verify that 'hasSpecElements' doesn't care about the order of
            // the items in the catalog.

            bsl::string permuteSpec(SPEC, &tb);
            int numPermutations = 0;
            bsl::sort(permuteSpec.begin(), permuteSpec.end());
            do {
                ASSERT(hasSpecElements(X, permuteSpec.c_str()));
            } while (numPermutations++ < 10 &&
                                     bsl::next_permutation(permuteSpec.begin(),
                                                           permuteSpec.end()));
        }

        ASSERT(LENGTH == X.length());
        ASSERT(static_cast<unsigned>(LENGTH) == handles.size());
        for (int jj = 0; jj < LENGTH; ++jj) {
            const char value = SPEC[jj];
            ASSERT(u::isMemberValue(X, value));
            setData(ampersand(v), value);
            ASSERT(X.isMember(v));
            const int handle = handles[jj];
            ASSERT(0 == X.find(handle));

            ASSERT(0 == mX.remove(handle));
            ASSERT(0 != X.find(handle));
            const bool expThere =
                               bsl::find(SPEC + jj + 1, SPEC + LENGTH, value) <
                                                                 SPEC + LENGTH;
            ASSERT(expThere == u::isMemberValue(X, value));
            ASSERT(expThere == X.isMember(v));
            ASSERT(X.length() == LENGTH - jj - 1);
        }
        ASSERT(0 == X.length());
        for (int jj = 0; jj < LENGTH; ++jj) {
            int value = SPEC[jj];
            ASSERT(!u::isMemberValue(X, value));
            setData(ampersand(v), value);
            ASSERT(!X.isMember(v));
            ASSERT(0 != X.find(handles[jj]));
        }

        setData(ampersand(v), '5');
        vHandle = mX.add(MUtil::move(v));
        setData(ampersand(v), '5');
        ASSERT(0 != vHandle);
        ASSERT(1 == X.length());
        ASSERT(X.isMember(V));
        ASSERT(u::isMemberValue(X, '5'));

        // Now do it without the handles.

        gg(&mX, SPEC);

        // Verify that 'gg' clears values previously in the catalog.

        ASSERT(!X.isMember(V));
        ASSERT(!u::isMemberValue(X, '5'));

        ASSERT(hasSpecElements(X, SPEC));
        bsl::string newSpec(SPEC);
        newSpec += '7';
        ASSERT(!hasSpecElements(X, newSpec.c_str()));
        if (0 != LENGTH) {
            ASSERT(!hasSpecElements(X, ""));

            newSpec = SPEC;
            newSpec.resize(newSpec.size() - 1);
            ASSERT(!hasSpecElements(X, newSpec.c_str()));

            newSpec = SPEC;
            newSpec += SPEC[0];
            ASSERT(!hasSpecElements(X, newSpec.c_str()));
        }

        for (int jj = 0; jj < LENGTH; ++jj) {
            int value = SPEC[jj];
            ASSERT(u::isMemberValue(X, value));
            setData(ampersand(v), value);
            ASSERT(X.isMember(v));
        }

        ASSERT(LENGTH == X.length());
        char specCopy[100];
        ASSERT(LENGTH < 100);
        bsl::strcpy(specCopy, SPEC);
        int ii = 0;
        for (Iter it(X); it; ++it, ++ii) {
            const ELEMENT& vRef = it.value();
            const int value = getData(vRef);
            char *pc = bsl::find(specCopy + 0,
                                 specCopy + LENGTH,
                                 static_cast<char>(value));
            ASSERT(*pc == value);
            ASSERT(pc < specCopy + LENGTH);
            ASSERT(pc - specCopy == ii);
            setData(ampersand(v), value);

            ASSERT(0 == X.find(it.handle()));

            ASSERTV(SPEC, *pc, pc - specCopy, X.isMember(v));
            ASSERTV(SPEC, *pc, pc - specCopy, u::isMemberValue(X, value));

            *pc = 0;
        }
        ASSERT(LENGTH == ii);
        ASSERT(static_cast<unsigned>(LENGTH) ==
                               bsl::count(specCopy + 0, specCopy + LENGTH, 0));

        setData(ampersand(v), '5');
    }

    v.~ELEMENT();
}

template <class ELEMENT>
void TestDriver<ELEMENT>::testCaseBreathingCopyOrMovable()
{
    const char *name = bsls::NameOf<ELEMENT>();

    if (verbose) cout << "testCaseBreathingCopyOrMovable<" << name << ">()\n";

    bslma::TestAllocator ta(veryVeryVerbose);
    bslma::TestAllocator tb(veryVeryVerbose);

    typedef bsls::ObjectBuffer<ELEMENT> OElement;

    OElement OVA, OVB, OVC, OVD, OVE, OVF, OVG;
    ELEMENT& VA = OVA.object();
    ELEMENT& VB = OVB.object();
    ELEMENT& VC = OVC.object();
    ELEMENT& VD = OVD.object();
    ELEMENT& VE = OVE.object();
    ELEMENT& VF = OVF.object();
    ELEMENT& VG = OVG.object();

    TTF::emplace(OVA.address(), 'A', &ta);
    TTF::emplace(OVB.address(), 'B', &ta);
    TTF::emplace(OVC.address(), 'C', &ta);
    TTF::emplace(OVD.address(), 'D', &ta);
    TTF::emplace(OVE.address(), 'E', &ta);
    TTF::emplace(OVF.address(), 'F', &ta);
    TTF::emplace(OVG.address(), 'G', &ta);

    ASSERTV(name, getData(VA), 'A' == getData(VA));

    ASSERT( usesAllocatorOrNoAllocator(VA, &ta));
    ASSERT(!usesAllocatorOrNoAllocator(VA, &tb) || !k_IS_ALLOCATING);

    Obj x1(&ta);                const Obj &X1=x1;

    if (verbose) cout << "testing add(value), length(), and value(h)\n";

    int HA = x1.add(MUtil::move(VA));
    ASSERT(0 != HA);
    ASSERT(1 == X1.length());
    setData(OVA.address(), 'A');
    ASSERTV(name, getData(X1.value(HA)), VA == X1.value(HA));

    int HB = x1.add(MUtil::move(VB));
    ASSERT(0 != HB);
    ASSERT(2 == X1.length());
    setData(OVB.address(), 'B');
    ASSERT(VB == X1.value(HB));

    int HC = x1.add(MUtil::move(VC));
    ASSERT(0 != HC);
    ASSERT(3 == X1.length());
    setData(OVC.address(), 'C');
    ASSERT(VC == X1.value(HC));

    int HD = x1.add(MUtil::move(VD));
    ASSERT(0 != HD);
    ASSERT(4 == X1.length());
    setData(OVD.address(), 'D');
    ASSERT(VD == X1.value(HD));

    int HE = x1.add(MUtil::move(VE));
    ASSERT(0 != HE);
    ASSERT(5 == X1.length());
    setData(OVE.address(), 'E');
    ASSERT(VE == X1.value(HE));

    int HF = x1.add(MUtil::move(VF));
    ASSERT(0 != HF);
    ASSERT(6 == X1.length());
    setData(OVF.address(), 'F');
    ASSERT(VF == X1.value(HF));

    ELEMENT vbuffer;
    ASSERT(0 == x1.remove(HD, ampersand(vbuffer)));
    ASSERT(VD == vbuffer);
    ASSERT(5 == X1.length());
    ASSERT(0 != X1.find(HD));

    ASSERT(0 == x1.replace(HE, MUtil::move(VG)));
    setData(OVG.address(), 'G');
    ASSERT(VG == X1.value(HE));

    if (verbose) cout << "testing removeAll(buffer)" << endl;

    Obj x2(&ta);
    bsl::vector<ELEMENT> vec;
    x2.add(MUtil::move(VA));
    x2.add(MUtil::move(VB));
    x2.add(MUtil::move(VC));
    x2.removeAll(&vec);
    ASSERT(vec.size() == 3);

    setData(OVA.address(), 'A');
    setData(OVB.address(), 'B');
    setData(OVC.address(), 'C');

    ASSERT(vec[0] == VA);
    ASSERT(vec[1] == VB);
    ASSERT(vec[2] == VC);

    if (verbose) cout << "Testing iterator ++ and 'value'\n";

    int sum = 0;
    int iterations = 0;
    char intValues[] = { "ABCGF" };
    const bsls::Types::IntPtr k_NUM_INT_VALUES = bsl::strlen(intValues);
    for (Iter it(X1); it; ++it, ++iterations) {
        const int intVal = getData(it.value());
        ASSERT(usesAllocatorOrNoAllocator(it.value(), &ta));
        ASSERTV(intVal, 'A' <= intVal && intVal <= 'G');
        const char charVal = static_cast<char>(intVal);
        if (veryVerbose) cout << "Value: '" << charVal << "'\n";
        char *pc = bsl::find(intValues + 0,
                             intValues + k_NUM_INT_VALUES,
                             charVal);
        ASSERT(pc < intValues + k_NUM_INT_VALUES);
        sum += intVal;
        if (pc < intValues + k_NUM_INT_VALUES) *pc = 0;
    }
    const int expectedSum = 'A' + 'B' + 'C' + 'G' + 'F';
    ASSERTV(sum, expectedSum, sum == expectedSum);
    ASSERTV(k_NUM_INT_VALUES, iterations, k_NUM_INT_VALUES == iterations);
    ASSERT(k_NUM_INT_VALUES == bsl::count(intValues + 0,
                                          intValues + k_NUM_INT_VALUES,
                                          0));

    x1.removeAll();
    ASSERT(0 == x1.length());

    VA.~ELEMENT();
    VB.~ELEMENT();
    VC.~ELEMENT();
    VD.~ELEMENT();
    VE.~ELEMENT();
    VF.~ELEMENT();
    VG.~ELEMENT();
}

template <class ELEMENT>
void TestDriver<ELEMENT>::testCaseBreathingCopyable()
{
    const char *name = bsls::NameOf<ELEMENT>();

    if (verbose) cout << "testCaseBreathingCopyable<" << name << ">()\n";

    bslma::TestAllocator  ta(veryVeryVerbose);
    bslma::TestAllocator  tb(veryVeryVerbose);
    bslma::Allocator&     td = *bslma::Default::allocator();

    typedef bsls::ObjectBuffer<ELEMENT> OElement;

    OElement OVA, OVB, OVC, OVD, OVE, OVF, OVG;
    ELEMENT& VA = OVA.object();
    ELEMENT& VB = OVB.object();
    ELEMENT& VC = OVC.object();
    ELEMENT& VD = OVD.object();
    ELEMENT& VE = OVE.object();
    ELEMENT& VF = OVF.object();
    ELEMENT& VG = OVG.object();

    TTF::emplace(OVA.address(), 'A', &ta);
    TTF::emplace(OVB.address(), 'B', &ta);
    TTF::emplace(OVC.address(), 'C', &ta);
    TTF::emplace(OVD.address(), 'D', &ta);
    TTF::emplace(OVE.address(), 'E', &ta);
    TTF::emplace(OVF.address(), 'F', &ta);
    TTF::emplace(OVG.address(), 'G', &ta);

    ASSERTV(getData(VA), 'A' == getData(VA));

    ASSERT( usesAllocatorOrNoAllocator(VA, &ta));
    ASSERT(!usesAllocatorOrNoAllocator(VA, &tb) || !k_IS_ALLOCATING);

    Obj x1(&ta);                const Obj &X1=x1;
    ELEMENT vbuffer;

    if (verbose) cout << "testing add(value), length(), and find(h,buffer)\n";

    int HA = x1.add(VA);
    ASSERT(0 != HA);
    ASSERT(1 == X1.length());
    ASSERT(0 == X1.find(HA, ampersand(vbuffer)));
    ASSERT(VA == vbuffer);

    int HB = x1.add(VB);
    ASSERT(0 != HB);
    ASSERT(2 == X1.length());
    ASSERT(0 == X1.find(HB, ampersand(vbuffer)));
    ASSERT(VB == vbuffer);

    int HC = x1.add(VC);
    ASSERT(0 != HC);
    ASSERT(3 == X1.length());
    ASSERT(0 == X1.find(HC, ampersand(vbuffer)));
    ASSERT(VC == vbuffer);

    int HD = x1.add(VD);
    ASSERT(0 != HD);
    ASSERT(4 == X1.length());
    ASSERT(0 == X1.find(HD, ampersand(vbuffer)));
    ASSERT(VD == vbuffer);

    int HE = x1.add(VE);
    ASSERT(0 != HE);
    ASSERT(5 == X1.length());
    ASSERT(0 == X1.find(HE, ampersand(vbuffer)));
    ASSERT(VE == vbuffer);

    int HF = x1.add(VF);
    ASSERT(0 != HF);
    ASSERT(6 == X1.length());
    ASSERT(0 == X1.find(HF, ampersand(vbuffer)));
    ASSERT(VF == vbuffer);

    ASSERT(0 == x1.remove(HD, ampersand(vbuffer)));
    ASSERT(VD == vbuffer);
    ASSERT(5 == X1.length());
    ASSERT(0 != X1.find(HD));

    ASSERT(0 == x1.replace(HE, VG));
    ASSERT(0 == X1.find(HE, ampersand(vbuffer)));
    ASSERTV(name, VG == vbuffer);

    if (verbose) cout << "testing removeAll(buffer)" << endl;

    Obj x2(&ta);
    bsl::vector<ELEMENT> vec;
    x2.add(VA);
    x2.add(VB);
    x2.add(VC);
    x2.removeAll(&vec);
    ASSERT(vec.size() == 3);

    ASSERT(vec[0] == VA);
    ASSERT(vec[1] == VB);
    ASSERT(vec[2] == VC);

    if (verbose) cout << "Testing iterator ++ and 'operator()'\n";

    int sum = 0;
    int iterations = 0;
    char intValues[] = { "ABCGF" };
    const bsls::Types::IntPtr k_NUM_INT_VALUES = bsl::strlen(intValues);
    for (Iter it(X1); it; ++it, ++iterations) {
        const bsl::pair<int, ELEMENT>& pr = it();
        const int intVal = getData(pr.second);
        const int handle = pr.first;
        ASSERT(pr.second == X1.value(handle));
        ASSERTV(name, usesAllocatorOrNoAllocator(pr.second,        &td));
        ASSERTV(name, usesAllocatorOrNoAllocator(X1.value(handle), &ta));
        ASSERTV(intVal, 'A' <= intVal && intVal <= 'G');
        const char charVal = static_cast<char>(intVal);
        if (veryVerbose) cout << "Value: '" << charVal << "'\n";
        char *pc = bsl::find(intValues + 0,
                             intValues + k_NUM_INT_VALUES,
                             charVal);
        ASSERT(pc < intValues + k_NUM_INT_VALUES);
        sum += intVal;
        if (pc < intValues + k_NUM_INT_VALUES) *pc = 0;
    }
    const int expectedSum = 'A' + 'B' + 'C' + 'G' + 'F';
    ASSERTV(sum, expectedSum, sum == expectedSum);
    ASSERTV(k_NUM_INT_VALUES, iterations, k_NUM_INT_VALUES == iterations);
    ASSERT(k_NUM_INT_VALUES == bsl::count(intValues + 0,
                                          intValues + k_NUM_INT_VALUES,
                                          0));

    // Don't 'removeAll' -- leave it populated and make sure it destroys itself
    // properly.  We call 'removeAll' at the end of
    // 'testCaseBreathingCopyOrMovable'.

    VA.~ELEMENT();
    VB.~ELEMENT();
    VC.~ELEMENT();
    VD.~ELEMENT();
    VE.~ELEMENT();
    VF.~ELEMENT();
    VG.~ELEMENT();
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_USAGE_EXAMPLE

{

typedef bsl::queue<int> *RemoteAddress;
static bsl::queue<int>   server;
static bslmt::Mutex      serverMutex;
static bslmt::Condition  serverNotEmptyCondition;

const int NUM_QUERIES_TO_PROCESS   = 128; // for testing purposes
const int CALLBACK_PROCESSING_TIME = 10;  // in microseconds

class QueryResult;

void queryCallBack(const QueryResult& result)
    // For testing only, we simulate a callback that takes a given time to
    // process a query.
{
    (void) &result;

    bslmt::ThreadUtil::microSleep(CALLBACK_PROCESSING_TIME);
}

///Usage
///-----
//
/// Example 1: Catalog Usage
/// - - - - - - - - - - - -
// Consider a client sending queries to a server asynchronously.  When the
// response to a query arrives, the client needs to invoke the callback
// associated with that query.  For good performance, the callback should be
// invoked as quickly as possible.  One way to achieve this is as follows.  The
// client creates a catalog for the functors associated with queries.  It sends
// to the server the handle (obtained by passing the callback functor
// associated with the query to the 'add' method of catalog), along with the
// query.  The server does not interpret this handle in any way and sends it
// back to the client along with the computed query result.  The client, upon
// receiving the response, gets the functor (associated with the query) back by
// passing the handle (contained in the response message) to the 'find' method
// of catalog.
//
// Assume the following declarations (we leave the implementations as
// undefined, as the definitions are largely irrelevant to this example):
//..
    struct Query {
        // Class simulating the query.
    };

    class QueryResult {
        // Class simulating the result of a query.
    };

    class RequestMsg
        // Class encapsulating the request message.  It encapsulates the
        // actual query and the handle associated with the callback for the
        // query.
    {
        Query d_query;
        int   d_handle;

      public:
        RequestMsg(Query query, int handle)
            // Create a request message with the specified 'query' and
            // 'handle'.
        : d_query(query)
        , d_handle(handle)
        {
        }

        int handle() const
            // Return the handle contained in this response message.
        {
            return d_handle;
        }
    };

    class ResponseMsg
        // Class encapsulating the response message.  It encapsulates the query
        // result and the handle associated with the callback for the query.
    {
        int d_handle;

      public:
        void setHandle(int handle)
            // Set the "handle" contained in this response message to the
            // specified 'handle'.
        {
            d_handle = handle;
        }

        QueryResult queryResult() const
            // Return the query result contained in this response message.
        {
            return QueryResult();
        }

        int handle() const
            // Return the handle contained in this response message.
        {
            return d_handle;
        }
    };

    void sendMessage(RequestMsg msg, RemoteAddress peer)
        // Send the specified 'msg' to the specified 'peer'.
    {
        serverMutex.lock();
        peer->push(msg.handle());
        serverNotEmptyCondition.signal();
        serverMutex.unlock();
    }

    void recvMessage(ResponseMsg *msg, RemoteAddress peer)
        // Get the response from the specified 'peer' into the specified 'msg'.
    {
        serverMutex.lock();
        while (peer->empty()) {
            serverNotEmptyCondition.wait(&serverMutex);
        }
        msg->setHandle(peer->front());
        peer->pop();
        serverMutex.unlock();
    }

    void getQueryAndCallback(Query                            *query,
                             bsl::function<void(QueryResult)> *callBack)
        // Set the specified 'query' and 'callBack' to the next 'Query' and its
        // associated functor (the functor to be called when the response to
        // this 'Query' comes in).
    {
        (void)query;
        *callBack = &queryCallBack;
    }
//..
// Furthermore, let also the following variables be declared:
//..
    RemoteAddress serverAddress;  // address of remote server

    bdlcc::ObjectCatalog<bsl::function<void(QueryResult)> > catalog;
        // Catalog of query callbacks, used by the client internally to keep
        // track of callback functions across multiple queries.  The invariant
        // is that each element corresponds to a pending query (i.e., the
        // callback function has not yet been or is in the process of being
        // invoked).
//..
// Now we define functions that will be used in the thread entry functions:
//..
    void testClientProcessQueryCpp()
    {
        int queriesToBeProcessed = NUM_QUERIES_TO_PROCESS;
        while (queriesToBeProcessed--) {
            Query query;
            bsl::function<void(QueryResult)> callBack;

            // The following call blocks until a query becomes available.
            getQueryAndCallback(&query, &callBack);

            // Register 'callBack' in the object catalog.
            int handle = catalog.add(callBack);
            ASSERT(handle);

            // Send query to server in the form of a 'RequestMsg'.
            RequestMsg msg(query, handle);
            sendMessage(msg, serverAddress);
        }
    }

    void testClientProcessResponseCpp()
    {
        int queriesToBeProcessed = NUM_QUERIES_TO_PROCESS;
        while (queriesToBeProcessed--) {
            // The following call blocks until some response is available in
            // the form of a 'ResponseMsg'.

            ResponseMsg msg;
            recvMessage(&msg, serverAddress);
            int handle = msg.handle();
            QueryResult result = msg.queryResult();

            // Process query 'result' by applying registered 'callBack' to it.
            // The 'callBack' function is retrieved from the 'catalog' using
            // the given 'handle'.

            bsl::function<void(QueryResult)> callBack;
            ASSERT(0 == catalog.find(handle, &callBack));
            callBack(result);

            // Finally, remove the no-longer-needed 'callBack' from the
            // 'catalog'.  Assert so that 'catalog' may not grow unbounded if
            // remove fails.

            ASSERT(0 == catalog.remove(handle));
        }
    }
//..
//
///Example 2: Iterator Usage
///- - - - - - - - - - - - -
// The following code fragment shows how to use bdlcc::ObjectCatalogIter to
// iterate through all the objects of 'catalog' (a catalog of objects of type
// 'MyType').
//..
    void use(bsl::function<void(QueryResult)> object)
    {
        (void)object;
    }
//..

}  // close namespace OBJECTCATALOG_TEST_USAGE_EXAMPLE

// ============================================================================
//                         CASE 13 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_14

{

typedef bdlcc::ObjectCatalogIter<int> Iter;

enum {
    k_NUM_THREADS    = 10,
    k_NUM_ITERATIONS = 1000
};

bslma::TestAllocator ta(veryVeryVerbose);
bdlcc::ObjectCatalog<int> catalog(&ta);

bslmt::Barrier barrier(k_NUM_THREADS + 3);

int getObjectFromPair(Iter &it)
{
    return it().second;
}

void validateIter (int arr[], int len)
    // Verify the iteration.  This function is invoked from 'testIteration'
    // after it has iterated the 'catalog'.
{
    ASSERT(len <= k_NUM_THREADS);
    for (int i=0; i<len; i++) {
        // value must be valid
       int present = 0;
       for (int id=0; id<k_NUM_THREADS; id++) {
           if (id == arr[i] || -id-1 == arr[i]) {
               present=1; break;
           }
       }
       ASSERT(present == 1);

       // no duplicate should be there
       for (int j=i+1; j<len; j++) {
           ASSERT(arr[i] != arr[j]);
       }

    }
}

extern "C" {

void *testAddFindReplaceRemove(void *arg)
    // Invoke 'add', 'find', 'replace' and 'remove' in a loop.
{
    barrier.wait();
    int id = static_cast<int>(reinterpret_cast<bsls::Types::IntPtr>(arg));
    int v;
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        int h = catalog.add(id);
        ASSERTV(i, catalog.find(h) == 0);
        ASSERTV(i, catalog.find(h, &v) == 0);
        ASSERTV(i, v == id);
        ASSERTV(i, catalog.replace(h, -id-1) == 0);
        ASSERTV(i, catalog.find(h) == 0);
        ASSERTV(i, catalog.find(h, &v) == 0);
        ASSERTV(i, v == -id-1);
        v = -1; // reset
        ASSERTV(i, catalog.remove(h, &v) == 0);
        ASSERTV(i, v == -id-1);
        int findVal = catalog.find(h);
        ASSERTV(i, findVal == -1);
    }
    return NULL;
}

void *testLength(void *arg)
    // Invoke 'length' in a loop.
{
    (void)arg;
    barrier.wait();
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        int len = catalog.length();
        ASSERTV(i, len, len >= 0);
        ASSERTV(i, len, len <= k_NUM_THREADS);
    }
    return NULL;
}

void *testIteration(void *arg)
    // Iterate the 'catalog' in a loop.
{
    (void)arg;
    barrier.wait();
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {

        enum { k_MAX = 100 };
        int arr[k_MAX]; int size=0;
        for (Iter it(catalog); it; ++it) {
            arr[size++] = getObjectFromPair(it);
        }
        validateIter(arr, size);
    }
    return NULL;
}

void *verifyStateThread(void *arg)
    // Verify the 'catalog' in a loop.
{
    (void)arg;
    barrier.wait();
    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        catalog.verifyState();
    }
    return NULL;
}

} // extern "C"

}  // close namespace OBJECTCATALOG_TEST_CASE_14
// ============================================================================
//                         CASE 12 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_13

{

}  // close namespace OBJECTCATALOG_TEST_CASE_13

// ============================================================================
//                         CASE 12 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_12

{

class AllocPattern {
    // This class encapsulates an integer pattern.  It also has a static
    // variable 'objCount', that holds the number of objects created for this
    // class.  It uses memory allocation to store the pattern.

    bslma::Allocator *d_alloc_p;
    int              *d_pattern_p;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AllocPattern, bslma::UsesBslmaAllocator);

    static int objCount;

    // CREATORS
    explicit
    AllocPattern(bslma::Allocator *alloc = 0)
    : d_alloc_p(bslma::Default::allocator(alloc))
    , d_pattern_p((int*)d_alloc_p->allocate(sizeof *d_pattern_p))
    {
        ASSERT(0 == (bsls::Types::IntPtr)((char *)this) %
                                 bsls::AlignmentFromType<AllocPattern>::VALUE);
        *d_pattern_p = 0;
        objCount++;
    }

    AllocPattern(const AllocPattern& original, bslma::Allocator *alloc = 0)
    : d_alloc_p(bslma::Default::allocator(alloc))
    , d_pattern_p((int*)d_alloc_p->allocate(sizeof *d_pattern_p))
    {
        ASSERT(0 == (bsls::Types::IntPtr)((char *)this) %
                                 bsls::AlignmentFromType<AllocPattern>::VALUE);
        *d_pattern_p = *original.d_pattern_p;
        objCount++;
    }

    ~AllocPattern()
    {
        d_alloc_p->deallocate(d_pattern_p);
        objCount--;
    }

    // MANIPULATORS
    AllocPattern& operator=(const AllocPattern& rhs)
    {
        if (this != &rhs) {
            d_alloc_p->deallocate(d_pattern_p);
            d_pattern_p = new(*d_alloc_p) int(rhs.pattern());
        }
        return *this;
    }

    void setPattern(int pattern)
    {
        *d_pattern_p = pattern;
    }

    // ACCESSORS
    int pattern() const
    {
        return *d_pattern_p;
    }
};

int AllocPattern::objCount = 0;

}  // close namespace OBJECTCATALOG_TEST_CASE_12

// ============================================================================
//                         CASE 11 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_11

{

class Pattern {
    // This class encapsulates an integer pattern.  It also has a static
    // variable 'objCount', that holds the number of objects created for this
    // class.

    int d_pattern;

  public:
    static int objCount;

    // CREATORS
    Pattern()
    {
        ASSERT(0 == (bsls::Types::IntPtr)((char *)this)
                                    % bsls::AlignmentFromType<Pattern>::VALUE);
        d_pattern = 0;
        objCount++;
    }

    Pattern(const Pattern& original)
    {
        d_pattern = original.d_pattern;
        objCount++;
    }

    ~Pattern()
    {
        objCount--;
    }

    // MANIPULATORS
    void setPattern(int pattern)
    {
        d_pattern = pattern;
    }

    // ACCESSORS
    int pattern() const
    {
        return d_pattern;
    }
};

int Pattern::objCount = 0;

}  // close namespace OBJECTCATALOG_TEST_CASE_11

// ============================================================================
//                          CASE 10 RELATED ENTITIES
// ----------------------------------------------------------------------------

namespace OBJECTCATALOG_TEST_CASE_10

{

typedef bdlcc::ObjectCatalogIter<int> Iter;

void verifyAccessors(Obj          *o1,
                     vector<int>&  handles1,
                     u::my_Obj    *o2,
                     vector<int>&  handles2,
                     int           maxHandles)
    // Verify the catalog accessors (including iterator) by comparing with
    // alternate implementation.
{
    int v1, v2, v;

    if (veryVerbose) { cout << "\tverifying 'length'\n"; }
    ASSERT(o1->length() == o2->length());

    if (veryVerbose) { cout << "\tverifying 'find(i, &v)'\n"; }
    ASSERT(o1->find(-1, &v1) != 0);
    ASSERT(o2->find(-1, &v2) != 0);
    ASSERT(o1->find(maxHandles, &v1) != 0);
    ASSERT(o2->find(maxHandles, &v2) != 0);
    for (int i = 0; i < maxHandles; ++i) {
        int r1 = o1->find(handles1[i], &v1);
        int r2 = o2->find(handles2[i], &v2);

        if (r2 !=0) {
            ASSERTV(i, r1 != 0);
        }
        else {
            ASSERTV(i, r1 == 0);
            ASSERTV(i, v1, v2, v1 == v2);
        }
    }

    if (veryVerbose) { cout << "\tverifying 'find(i)'\n"; }
    ASSERT(o1->find(-1) != 0);
    ASSERT(o2->find(-1) != 0);
    ASSERT(o1->find(maxHandles) != 0);
    ASSERT(o2->find(maxHandles) != 0);
    for (int i = 0; i < maxHandles; ++i) {
        int r1 = o1->find(handles1[i]);
        int r2 = o2->find(handles2[i]);

        if (r2 !=0) {
            ASSERTV(i, r1 != 0);
        }
        else {
            ASSERTV(i, r1 == 0);
        }
    }

    if (veryVerbose) { cout << "\tverifying iteration\n"; }

    for (Iter it(*o1); it; ++it) {
        pair<int, int> p = it();
        o1->find(p.first, &v);
        ASSERT(v == p.second);
        ASSERT(o2->isMember(p.second));
    }
}

}  // close namespace OBJECTCATALOG_TEST_CASE_10

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n-------------" << bsl::endl;

        using namespace OBJECTCATALOG_TEST_USAGE_EXAMPLE;

        serverAddress = &server;

        {
            if (verbose) bsl::cout << "\n\tCatalog usage"
                                   << "\n\t-------------" << bsl::endl;

// In some thread, the client executes the following code.
//..
//  extern "C" void *testClientProcessQuery(void *)
//  {
//      testClientProcessQueryCpp();
//      return 0;
//  }
//..
// In some other thread, the client executes the following code.
//..
//  extern "C" void *testClientProcessResponse(void *)
//  {
//      testClientProcessResponseCpp();
//      return 0;
//  }
//..
        }

        {
            if (verbose) bsl::cout << "\n\tIterator usage"
                                   << "\n\t--------------" << bsl::endl;

// Now iterate through the 'catalog':
//..
//  for (bdlcc::ObjectCatalogIter<MyType> it(catalog); it; ++it) {
//      bsl::pair<int, MyType> p = it(); // p.first contains the handle and
//                                       // p.second contains the object
//      use(p.second);                   // the function 'use' uses the
//                                       // object in some way
//  }
//  // 'it' is now destroyed out of the scope, releasing the lock.
//..
// Note that the associated catalog is (read)locked when the iterator is
// constructed and is unlocked only when the iterator is destroyed.  This means
// that until the iterator is destroyed, all the threads trying to modify the
// catalog will remain blocked (even though multiple threads can concurrently
// read the object catalog).  So clients must make sure to destroy their
// iterators after they are done using them.  One easy way is to use the
// 'for (bdlcc::ObjectCatalogIter<MyType> it(catalog); ...' as above.

        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // MULTI-TYPE MANIPULATORS / ACCESSORS TEST
        //
        // Concerns:
        //: 1 Test the manipulators and accessors with the catalog containing a
        //:   wide variety of types.
        //:
        //: 2 For move-aware types, verify that they were moved when expected.
        //
        // Plan:
        //: 1 Use a templated test case configured with a wide variety of test
        //:   types.
        //:
        //: 2 Have two test case template functions, one which requires
        //:   'ELEMENT' to have a copy c'tor, and the other of which will work
        //:   where 'ELEMENT' is a move only type.
        //:
        //: 3 After operations, if the type is a move-aware type, use the
        //:   'value' accessors to access the object and see if it was moved
        //:   into.
        //
        // Testing:
        //   int add(TYPE&&);
        //   int replace(int, TYPE&&);
        //   void removeAll(bsl::vector<TYPE> *);
        //   bslma::Allocator *allocator() const;
        //   bool isMember(const TYPE&) const;
        //   const TYPE& value(int) const;
        //   int find(int) const;
        //   int Iter::handle() const;
        //   const TYPE& Iter::value() const;
        // --------------------------------------------------------------------

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                TestDriver,
                                testCaseManipulatorsCopyable,
                                BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                TestDriver,
                                testCaseManipulatorsCopyOrMovable,
                                BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                TestDriver,
                                testCaseManipulatorsCopyOrMovable,
                                u::WellBehavedMoveOnlyAllocTestType,
                                bsltf::MoveOnlyAllocTestType,
                                bsltf::MovableTestType,
                                bsltf::MovableAllocTestType);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST:
        //   Verify the concurrent access to catalog.
        //
        // Concerns:
        //   That the catalog remain consistent in presence of multiple
        //   threads accessing/modifying it (either directly or through
        //   iteration).
        //
        // Plan:
        //   Create a catalog.  Create 'k_NUM_THREADS' threads and let each
        //   thread invoke 'add', 'find', 'replace' and 'remove' in a loop.
        //   Create a thread and let it invoke 'length' in a loop.  Create a
        //   thread and let it iterate the catalog in a loop.
        //   Create a thread and let it invoke 'verifyState' in a loop.
        //   Let all above (k_NUM_THREADS + 3) threads run concurrently.
        //
        // Testing:
        //   CONCURRENCY TEST:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCURRENCY TEST" << endl
                          << "================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_14;

        bslmt::ThreadUtil::Handle threads[k_NUM_THREADS + 3];

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            bslmt::ThreadUtil::create(&threads[i],
                                      testAddFindReplaceRemove,
                                      (void*)(bsls::Types::IntPtr)i);
        }

        bslmt::ThreadUtil::create(&threads[k_NUM_THREADS + 0],
                                  testLength,
                                  NULL);
        bslmt::ThreadUtil::create(&threads[k_NUM_THREADS + 1],
                                  testIteration,
                                  NULL);
        bslmt::ThreadUtil::create(&threads[k_NUM_THREADS + 2],
                                  verifyStateThread,
                                  NULL);

        for (int i = 0; i < k_NUM_THREADS + 3; ++i) {
            bslmt::ThreadUtil::join(threads[i]);
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING STALE HANDLE REJECTION:
        //   Verify that catalog rejects the stale handles properly.
        //
        // Concerns:
        //   That stale handles must be rejected by catalog.
        //
        // Plan:
        //   Create a catalog, add an object and remove the added object thus
        //   making the handle (returned by 'add') stale.  Verify that the
        //   catalog rejects this handle correctly.
        //
        // Testing:
        //   TESTING STALE HANDLE REJECTION
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING STALE HANDLE REJECTION" << endl
                          << "==============================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_13;
        typedef bdlcc::ObjectCatalog<double> Obj;
        enum {
            k_NUM_ITERATIONS = 5
        };

        const double VA = 1.0;
        const double VB = 2.0;
        int HA = -1;
        int HB = -1;

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(&ta);

        HA = x.add(VA);
        x.remove(HA);

        for (bsl::size_t j=0; j < k_NUM_ITERATIONS; ++j) {
            for (bsl::size_t i = 1; i < u::k_RECYCLE_COUNT; ++i) {
                ASSERT(0 != x.find(HA)); // stale handle should be rejected
                                         // until the corresponding 'd_nodes'
                                         // entry is reused 'k_RECYCLE_COUNT'
                                         // times.

                HB = x.add(VB);
                x.remove(HB);
            }
            HB = x.add(VB);

            ASSERTV(j, HA == HB);
            ASSERTV(j, 0 == x.find(HA));

            double vbuf = 0.0;
            ASSERTV(j, 0 == x.find(HA, &vbuf));
            ASSERTV(j, vbuf, VB == vbuf);
            x.remove(HB);
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING OBJECT CONSTRUCTION/DESTRUCTION WITH ALLOCATION
        //   Verify that catalog properly constructs and destroys the objects
        //   with the catalog's allocator.
        //
        // Concerns:
        //   That 'bdlcc::ObjectCatalog<TYPE>' properly passes the allocator
        //   to its object.
        //
        // Plan:
        //   Create a catalog of 'AllocPattern' (a class that encapsulates an
        //   integer pattern with allocation) objects.  Create an allocated
        //   pattern object 'a', set its pattern to 'PATTERN1', add it to the
        //   catalog, invoke 'find' to get it back and verify that its pattern
        //   is 'PATTERN1'.
        //
        //   Create another pattern object 'b', set its pattern to be
        //   'PATTERN2', invoke 'replace' to replace 'a' with 'b', invoke
        //   'find' to get 'b' back and verify that its pattern is 'PATTERN2'.
        //   Invoke 'remove' to remove 'b' and verify that pattern of the
        //   removed object is 'PATTERN2'.
        //
        //   Finally invoke 'removeAll' and verify that the number of created
        //   objects (of class 'AllocPattern') is correct.
        //
        // Testing:
        //   TESTING OBJECT CONSTRUCTION/DESTRUCTION WITH ALLOCATION
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING OBJECT CONSTRUCTION/DESTRUCTION" << endl
                          << "=======================================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_12;

        typedef bdlcc::ObjectCatalog<AllocPattern> Obj;

        enum {
            k_PATTERN1 = 0x33333333,
            k_PATTERN2 = 0xaaaaaaaa,
            k_PATTERN3 = 0xbbbbbbbb
        };

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x(&ta);
        {
            AllocPattern a(&ta), b(&ta), vbuf(&ta);
            int HA;

            a.setPattern(k_PATTERN1);
            HA = x.add(a);

            x.find(HA, &vbuf);
            ASSERT(vbuf.pattern() == k_PATTERN1);

            b.setPattern(k_PATTERN2);
            x.replace(HA, b);
            x.find(HA, &vbuf);
            ASSERT((unsigned)vbuf.pattern() == k_PATTERN2);

            vbuf.setPattern(k_PATTERN3);
            x.remove(HA, &vbuf);
            ASSERT((unsigned)vbuf.pattern() == k_PATTERN2);

            x.removeAll();
        } // let 'a', 'b', and 'vbuf' be destroyed

        ASSERT(AllocPattern::objCount == 0);

      }break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING OBJECT CONSTRUCTION/DESTRUCTION:
        //   Verify that catalog properly constructs and destroys the objects.
        //
        // Concerns:
        //   That 'bdlcc::ObjectCatalog<TYPE>' properly constructs and destroys
        //   the objects (of type 'TYPE').
        //
        // Plan:
        //   Create a catalog of 'Pattern' (a class that encapsulates an
        //   integer pattern) objects.  Create a pattern object 'a', set its
        //   pattern to 'k_PATTERN1', add it to the catalog, invoke 'find' to
        //   get it back and verify that its pattern is 'k_PATTERN1'.
        //
        //   Create another pattern object 'b', set its pattern to be
        //   'k_PATTERN2', invoke 'replace' to replace 'a' with 'b', invoke
        //   'find' to get 'b' back and verify that its pattern is
        //   'k_PATTERN2'.  Invoke 'remove' to remove 'b' and verify that
        //   pattern of the removed object is 'k_PATTERN2'.
        //
        //   Finally invoke 'removeAll' and verify that the number of created
        //   objects (of class 'Pattern') is correct.
        //
        // Testing:
        //   TESTING OBJECT CONSTRUCTION/DESTRUCTION
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING OBJECT CONSTRUCTION/DESTRUCTION" << endl
                          << "=======================================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_11;

        typedef bdlcc::ObjectCatalog<Pattern> Obj;

        enum {
            k_PATTERN1 = 0x33333333,
            k_PATTERN2 = 0xaaaaaaaa,
            k_PATTERN3 = 0xbbbbbbbb
        };

        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        {
            Pattern a, b, vbuf;
            int HA;

            a.setPattern(k_PATTERN1);
            HA = x1.add(a);
            x1.find(HA, &vbuf);
            ASSERT(vbuf.pattern() == k_PATTERN1);

            b.setPattern(k_PATTERN2);
            x1.replace(HA, b);
            x1.find(HA, &vbuf);
            ASSERT((unsigned)vbuf.pattern() == k_PATTERN2);

            vbuf.setPattern(k_PATTERN3);
            x1.remove(HA, &vbuf);
            ASSERT((unsigned)vbuf.pattern() == k_PATTERN2);

            x1.removeAll();
        } // let 'a', 'b', and 'vbuf' be destroyed

        ASSERT(Pattern::objCount == 0);

      }break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ACCESSORS:
        //   Verify the accessors of catalog.
        //
        // Concerns:
        //   That accessors of 'bdlcc::ObjectCatalog' (including access through
        //   iterator) work correctly in presence of one thread.
        //
        // Plan:
        //   Bring the catalog into various states using primary manipulators.
        //   For each state, invoke various accessors and then verify the
        //   result.
        //
        // Testing:
        //   int find(int, TYPE *) const;
        //   int length() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ACCESSORS" << endl
                          << "=================" << endl;

        using namespace OBJECTCATALOG_TEST_CASE_10;

        for (int i=0; i < u::NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\ntesting 'length', 'find' and"
                      << "'iteration' with catalog-state \"";
                u::printSpec(u::SPECS[i]);
                cout << "\""<< endl;
            }
            int len = static_cast<int>(strlen(u::SPECS[i]));
            bslma::TestAllocator ta(veryVeryVerbose);

            for (bsl::size_t g = 0; g < 2 * u::k_RECYCLE_COUNT;
                                                               g = 2 * g + 1) {
                if (veryVerbose)
                    cout  << "\tUsing handles with " << g << " generations\n";

                if (veryVeryVerbose)
                    cout  << "\t\tbringing the catalog in the desired state\n";

                Obj o1(&ta);
                u::my_Obj o2;
                vector<int> handles1(len, -1);
                vector<int> handles2(len, -1);
                u::ggInt(&o1,
                         handles1,
                         &o2,
                         handles2,
                         u::SPECS[i],
                         static_cast<int>(g));

                if (veryVeryVerbose)
                    cout << "\t\tbrought the catalog into the desired state\n";

                if (veryVeryVerbose) { cout << "\t\tverifying o1\n"; }
                o1.verifyState();
                if (veryVeryVerbose) { cout << "\t\tverifying o2\n"; }
                o2.verifyState();

                if (veryVeryVerbose) { cout << "\t\tverifying accessors \n"; }
                verifyAccessors(&o1, handles1, &o2, handles2, len);
            }
        }
      }break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ITERATION:
        //   Verify the iteration.
        //
        // Concerns:
        //   That iteration works correctly in presence of one thread.
        //
        // Plan:
        //
        // Testing:
        //   bdlcc::ObjectCatalogIter(const bdlcc::ObjectCatalog<TYPE>&);
        //   ~bdlcc::ObjectCatalogIter();
        //   void operator++();
        //   operator const void *() const;
        //   pair<int, TYPE> operator()() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ITERATION" << endl
                          << "=================" << endl;

        typedef bdlcc::ObjectCatalog<double> Obj;
        typedef bdlcc::ObjectCatalogIter<double> Iter;
        int HA, HB, HC, HD, HE;
        double vbuffer;

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        x1.add(1); if (veryVerbose) {cout << "\tadd(1)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        x1.remove(HA); if (veryVerbose) { cout << "\tremove(1)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        x1.remove(HA); if (veryVerbose) { cout << "\tremove(1)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        x1.remove(HB); if (veryVerbose) { cout << "\tremove(2)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        x1.remove(HA); if (veryVerbose) { cout << "\tremove(1)\n";}
        x1.remove(HB); if (veryVerbose) { cout << "\tremove(2)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        HC = x1.add(3); if (veryVerbose) { cout << "\tadd(3)\n"; }
        HD = x1.add(4); if (veryVerbose) { cout << "\tadd(4)\n"; }
        HE = x1.add(5); if (veryVerbose) { cout << "\tadd(5)\n"; }

        x1.remove(HA); if (veryVerbose) { cout << "\tremove(1)\n";}
        x1.remove(HC); if (veryVerbose) { cout << "\tremove(3)\n";}
        x1.remove(HE); if (veryVerbose) { cout << "\tremove(5)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }

        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

        {
        bslma::TestAllocator ta(veryVeryVerbose);
        Obj x1(&ta);
        if (veryVerbose) { cout << "\ncreate new catalog\n"; }
        HA = x1.add(1); if (veryVerbose) { cout << "\tadd(1)\n"; }
        HB = x1.add(2); if (veryVerbose) { cout << "\tadd(2)\n"; }
        HC = x1.add(3); if (veryVerbose) { cout << "\tadd(3)\n"; }
        HD = x1.add(4); if (veryVerbose) { cout << "\tadd(4)\n"; }
        HE = x1.add(5); if (veryVerbose) { cout << "\tadd(5)\n"; }

        x1.remove(HB); if (veryVerbose) { cout << "\tremove(2)\n";}
        x1.remove(HD); if (veryVerbose) { cout << "\tremove(4)\n";}
        if (veryVerbose) { cout << "\tnow iterate\n"; }
        for (Iter it(x1); it; ++it) {
            pair<int, double> p = it();
            x1.find(p.first, &vbuffer);
            ASSERT(vbuffer == p.second);
            if (veryVerbose) { T_; T_; P(p.second); }
        }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVEALL':
        //   Verify the 'removeAll'.
        //
        // Concerns:
        //   That 'removeAll' works correctly in presence of one thread.
        //
        // Plan:
        //   Bring the catalog into various states.  For each state, invoke
        //   'removeAll' and then verify the result.
        //
        // Testing:
        //   void removeAll();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'REMOVEALL'" << endl
                          << "=================" << endl;

        for (int i=0; i < u::NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\nTesting 'removeAll' with catalog-state \"";
                u::printSpec(u::SPECS[i]);
                cout << "\"" << endl;
            }

            int len = static_cast<int>(strlen(u::SPECS[i]));
            if (veryVerbose) {
                cout  << "\tbringing the catalog in the desired state\n";
            }

            bslma::TestAllocator ta(veryVeryVerbose);
            Obj o1(&ta);
            u::my_Obj o2;
            vector<int> handles1(len, -1);
            vector<int> handles2(len, -1);
            u::ggInt(&o1, handles1, &o2, handles2, u::SPECS[i]);

            if (veryVerbose) {
                cout << "\tbrought the catalog into the desired state\n";
            }

            if (veryVerbose) { cout << "\t\tnow doing removeAll();\n"; }

            o1.removeAll ();
            o2.removeAll ();

            if (veryVerbose) { cout << "\tverifying o1\n"; }
            o1.verifyState();
            if (veryVerbose) { cout << "\tverifying o2\n"; }
            o2.verifyState();
            if (veryVerbose) { cout << "\tmatching o1 and o2\n\n"; }
            u::verifyMatch(&o1, handles1, &o2, handles2, len);

        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVE(handle, &valueBuf)':
        //   Verify the 'remove(handle, &valueBuf)'.
        //
        // Concerns:
        //   That 'remove(handle, valueBuffer)' works correctly in presence of
        //   one thread.
        //
        // Plan:
        //   Bring the catalog into various states using primary manipulators.
        //   For each state, invoke 'remove' (with various values of handle
        //   argument) and then verify the result.
        //
        // Testing:
        //   int remove(int handle, TYPE* valueBuffer=0);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'REMOVE(handle, &valueBuf)'\n"
                          << "===================================" << endl;

        for (int i=0; i < u::NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\ntesting 'remove' with catalog-state \"";
                u::printSpec(u::SPECS[i]);
                cout << "\""<< endl;
            }

            int len = static_cast<int>(strlen(u::SPECS[i]));
            for (int j=0; j<len; ++j) {
                if (veryVerbose) {
                    cout  << "\tbringing the catalog in the desired state\n";
                }
                bslma::TestAllocator ta(veryVeryVerbose);
                Obj o1(&ta);

                u::my_Obj o2;
                vector<int> handles1(len);
                vector<int> handles2(len);
                for (int k=0; k<len; ++k) {
                    handles1[k] = -1;
                    handles2[k] = -1;
                }
                u::ggInt(&o1, handles1, &o2, handles2, u::SPECS[i]);
                if (veryVerbose) {
                    cout << "\tbrought the catalog into the desired state\n";
                }

                if (veryVerbose) {
                    cout << "\tdoing remove(handles1[" << j  <<"], &v1);\n";
                }
                int v1, v2;
                int r1 = o1.remove (handles1[j], &v1);
                int r2 = o2.remove (handles2[j], &v2);
                if (veryVerbose) {
                    cout << "\tverifying the above remove operation\n";
                }
                if (r2 != 0) {
                    ASSERT(r1 != 0);
                }
                else {
                    ASSERT(r1 == 0);
                    ASSERT(v1 == v2);
                    r1 = o1.find(handles1[j]);
                    r2 = o2.find(handles2[j]);
                    ASSERT(r1 != 0);
                    ASSERT(r2 != 0);
                }
                if (veryVerbose) { cout << "\tverifying o1\n"; }
                o1.verifyState();
                if (veryVerbose) { cout << "\tverifying o2\n"; }
                o2.verifyState();
                if (veryVerbose) { cout << "\tmatching o1 and o2\n\n"; }
                u::verifyMatch(&o1, handles1, &o2, handles2, len);
            }
        }
      }break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'REMOVE(handle)':
        //   Verify the 'remove(handle)'.
        //
        // Concerns:
        //   That 'remove(handle)' works correctly in presence of one thread.
        //
        // Plan:
        //   Bring the catalog into various states using primary manipulators.
        //   For each state, invoke 'remove' (with various values of handle
        //   argument) and then verify the result.
        //
        // Testing:
        //   int remove(int handle);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'REMOVE(handle)'" << endl
                          << "========================" << endl;

        for (int i=0; i < u::NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\ntesting 'remove' with catalog-state \"";
                u::printSpec(u::SPECS[i]);
                cout << "\""<< endl;
            }

            int len = static_cast<int>(strlen(u::SPECS[i]));
            for (int j=0; j<len; ++j) {
                if (veryVerbose) {
                    cout  << "\tbringing the catalog in the desired state\n";
                }
                bslma::TestAllocator ta(veryVeryVerbose);
                Obj o1(&ta);

                u::my_Obj o2;
                vector<int> handles1(len);
                vector<int> handles2(len);
                for (int k=0; k<len; ++k) {
                    handles1[k] = -1;
                    handles2[k] = -1;
                }
                u::ggInt(&o1, handles1, &o2, handles2, u::SPECS[i]);
                if (veryVerbose) {
                    cout << "\tbrought the catalog into the desired state\n";
                }

                if (veryVerbose) {
                    cout << "\tnow doing remove(handles1[" << j  <<"]);\n";
                }
                int r1 = o1.remove (handles1[j]);
                int r2 = o2.remove (handles2[j]);
                if (veryVerbose) {
                    cout << "\tverifying the above remove operation\n";
                }
                if (r2 != 0) {
                    ASSERT(r1 != 0);
                }
                else {
                    int v1, v2;
                    ASSERT(r1 == 0);
                    r1 = o1.find(handles1[j], &v1);
                    r2 = o2.find(handles2[j], &v2);
                    ASSERT(r1 != 0);
                    ASSERT(r2 != 0);
                }
                if (veryVerbose) { cout << "\tverifying o1\n"; }
                o1.verifyState();
                if (veryVerbose) { cout << "\tverifying o2\n"; }
                o2.verifyState();
                if (veryVerbose) { cout << "\tmatching o1 and o2\n"; }
                u::verifyMatch(&o1, handles1, &o2, handles2, len);
            }
        }

      }break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'REPLACE':
        //   Verify the 'replace'.
        //
        // Concerns:
        //   That 'replace' works correctly in presence of one thread.
        //
        // Plan:
        //   Bring the catalog into various states using primary manipulators.
        //   For each state, invoke 'replace' (with various values of handle
        //   argument) and then verify the result.
        //
        // Testing:
        //   int replace(int handle, TYPE const &newObject);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'REPLACE'" << endl
                          << "=================" << endl;

        for (int i=0; i < u::NUM_SPECS; ++i) {
            if (veryVerbose) {
                cout  << "\n\ntesting 'replace' with catalog-state \"";
                u::printSpec(u::SPECS[i]);
                cout << "\""<< endl;
            }

            int len = static_cast<int>(strlen(u::SPECS[i]));
            for (int j=0; j<len; ++j) {
                if (veryVerbose) {
                  cout  << "\tbringing the catalog in the desired state\n";
                }
                bslma::TestAllocator ta(veryVeryVerbose);
                Obj o1(&ta);

                u::my_Obj o2;
                vector<int> handles1(len);
                vector<int> handles2(len);
                for (int k=0; k<len; ++k) {
                    handles1[k] = -1;
                    handles2[k] = -1;
                }
                u::ggInt(&o1, handles1, &o2, handles2, u::SPECS[i]);
                if (veryVerbose) {
                    cout << "\tbrought the catalog into the desired state\n";
                }
                const int V = 444;
                if (veryVerbose) {
                    cout << "doing replace(handles1[" << j  <<"], "
                         << V << ");\n";
                }
                int r1 = o1.replace (handles1[j], V);
                int r2 = o2.replace (handles2[j], V);
                if (veryVerbose) {
                    cout << "\tverifying the above replace operation\n";
                }
                if (r2 != 0) {
                    ASSERT(r1 != 0);
                }
                else {
                    int v1, v2;
                    ASSERT(r1 == 0);
                    r1 = o1.find(handles1[j], &v1);
                    r2 = o2.find(handles2[j], &v2);
                    ASSERT(r1 == 0);
                    ASSERT(r2 == 0);
                    ASSERT(v1 == V);
                    ASSERT(v2 == V);
                }
                if (veryVerbose) { cout << "\tverifying o1\n"; }
                o1.verifyState();
                if (veryVerbose) { cout << "\tverifying o2\n"; }
                o2.verifyState();
                if (veryVerbose) { cout << "\tmatching o1 and o2\n\n"; }
                u::verifyMatch(&o1, handles1, &o2, handles2, len);
            }
        }

      }break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS:
        //   Verify the primary manipulators of catalog.
        //
        // Concerns:
        //   That the chosen primary manipulators 'add' and 'remove' work
        //   correctly in presence of one thread.
        //
        // Plan:
        //   Using generator function 'gg', bring the catalog into various
        //   states (enumerated in 'SPECS' array, see the documentation for
        //   'SPECS') by invoking only 'add' and 'remove'.  Verify the result
        //   after each invocation.  Verification is done by the combination
        //   of following three.
        //     (1) Sanity checks using accessors.
        //
        //     (2) Comparison between the behavior of 'bdlcc::ObjectCatalog'
        //     implementation and the alternate (PseudoObjectCatalog)
        //     implementation.
        //
        //     (3) Invocation of 'catalog.verifyState'.
        //
        // Testing:
        //   int add(TYPE const& object);
        //   int remove(int handle, TYPE* valueBuffer=0);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING PRIMARY MANIPULATORS" << endl
                          << "============================" << endl;

        for (int i=0; i < u::NUM_SPECS; ++i) {
            bslma::TestAllocator ta(veryVeryVerbose);
            Obj o1(&ta);

            u::my_Obj o2;
            int len = static_cast<int>(strlen(u::SPECS[i]));
            vector<int> handles1(len, -1);
            vector<int> handles2(len, -1);

            if (veryVerbose) {
                cout << "\nbringing into state with spec = \""
                     << u::SPECS[i] << "\"\n";
                cout << "above spec corresponds to following state:\n" ;
                u::printSpec(u::SPECS[i]);
            }

            u::ggInt(&o1, handles1, &o2, handles2, u::SPECS[i]);
            if (veryVerbose) { cout << "brought into state\n"; }

            if (veryVerbose) { cout << "verifying o1\n"; }
            o1.verifyState();
            if (veryVerbose) { cout << "verifying o2\n"; }
            o2.verifyState();

            if (veryVerbose) {cout << "matching o1 and o2\n"; }
            u::verifyMatch(&o1, handles1, &o2, handles2, len);
        }
      }break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ALTERNATE IMPLEMENTATION:
        //   Verify the alternate implementation.
        //
        // Concerns:
        //   That the alternate implementation (named PseudoObjectCatalog)
        //   for 'bdlcc::ObjectCatalog' is operational for one thread.
        //
        // Plan:
        //   Create a catalog.  Add 6 objects to it, verifying state after
        //   each addition.  Remove one of the objects and verify the state.
        //   Replace one of the object and verify the state.  Invoke
        //   'removeAll' and verify the state.
        //
        //   Create a catalog, add five elements and verify that 'isMember'
        //   works correctly.
        //
        // Testing:
        //   PseudoObjectCatalog()
        //   ~PseudoObjectCatalog()
        //   int PseudoObjectCatalog::add(TYPE const& object)
        //   int PseudoObjectCatalog::remove(int h, TYPE* valueBuffer=0)
        //   void PseudoObjectCatalog::removeAll()
        //   int PseudoObjectCatalog::replace(int h, TYPE const &newObject)
        //   int PseudoObjectCatalog::find(int h, TYPE *p=0) const
        //   int PseudoObjectCatalog::isMember(TYPE val) const
        //   int PseudoObjectCatalog::length() const
        //   bdlcc::ObjectCatalog(bslma::Allocator *allocator=0);
        //   ~bdlcc::ObjectCatalog();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ALTERNATE IMPLEMENTATION" << endl
                          << "================================" << endl;

        typedef u::PseudoObjectCatalog<double> Obj;
        const double VA = 1.0;
        const double VB = 1.1;
        const double VC = 19.02;
        const double VD = 11.1902;
        const double VE = 10.20;
        const double VF = 111902.1020;
        const double VG = 121902.1020;

        Obj x1;
        const Obj &X1=x1;
        double vbuffer;

        int HA = x1.add(VA);
        ASSERT(1 == X1.length());
        ASSERT(0 == X1.find(HA));
        ASSERT(0 == X1.find(HA, &vbuffer));
        ASSERT(VA == vbuffer);

        int HB = x1.add(VB);
        ASSERT(0 != HB);
        ASSERT(2 == X1.length());
        ASSERT(0 == X1.find(HB, &vbuffer));
        ASSERT(VB == vbuffer);

        int HC = x1.add(VC);
        ASSERT(0 != HC);
        ASSERT(3 == X1.length());
        ASSERT(0 == X1.find(HC, &vbuffer));
        ASSERT(VC == vbuffer);

        int HD = x1.add(VD);
        ASSERT(0 != HD);
        ASSERT(4 == X1.length());
        ASSERT(0 == X1.find(HD, &vbuffer));
        ASSERT(VD == vbuffer);

        int HE = x1.add(VE);
        ASSERT(0 != HE);
        ASSERT(5 == X1.length());
        ASSERT(0 == X1.find(HE, &vbuffer));
        ASSERT(VE == vbuffer);

        int HF = x1.add(VF);
        ASSERT(0 != HF);
        ASSERT(6 == X1.length());
        ASSERT(0 == X1.find(HF, &vbuffer));
        ASSERT(VF == vbuffer);

        ASSERT(0 == x1.remove(HD, &vbuffer));
        ASSERT(VD == vbuffer);
        ASSERT(5 == X1.length());
        ASSERT(0 != x1.find(HD));

        ASSERT(0 == x1.replace(HE, VG));
        ASSERT(0 == x1.find(HE, &vbuffer));
        ASSERT(VG == vbuffer);

        x1.removeAll();
        ASSERT(0 == x1.length());

        // testing isMember
        {
            typedef u::PseudoObjectCatalog<int> Obj;
            Obj x;
            x.add(1); x.add(2); x.add(3); x.add(4); x.add(5);

            ASSERT(x.isMember(1) == 1);
            ASSERT(x.isMember(2) == 1);
            ASSERT(x.isMember(3) == 1);
            ASSERT(x.isMember(4) == 1);
            ASSERT(x.isMember(5) == 1);

            ASSERT(x.isMember(0) == 0);
            ASSERT(x.isMember(6) == 0);
        }

      }break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Test the 'gg' function.
        //
        // Concerns:
        //: 1 That the 'u::gg' function properly populates an object and the
        //:   handles vector, if any, passed to it.
        //:
        //: 2 That the 'u::isMember' and 'u::isMemberValue' correctly identiy
        //:   whether an item is in the object catalog.
        //
        // Testing:
        //   TD::gg(Obj *, const char *, bsl::vector<int> * = 0);
        //   TD::hasSpecElements(const Obj&, const char *);
        //   bool Obj::isMember(const TYPE&);
        //   bool u::isMemberValue(const Obj&, int);
        //   int Obj::find(int);
        //   int Obj::remove(int);
        //   Iter(const Obj&);
        //   Iter::operator const void();
        //   Iter Iter::operator++();
        //   const TYPE& Iter::value();
        //   int Iter::handle();
        //   TD::setData(ELEMENT *, int);
        //   TD::getData(const ELEMENT&);
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST APPARATUS\n"
                             "==============\n";

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                TestDriver,
                                testCaseApparatus,
                                BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                TestDriver,
                                testCaseApparatus,
                                u::WellBehavedMoveOnlyAllocTestType,
                                bsltf::MoveOnlyAllocTestType,
                                bsltf::MovableTestType,
                                bsltf::MovableAllocTestType);
      }break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise the basic functionality.
        //
        // Concerns:
        //   That basic essential functionality is operational
        //   for one thread.
        //
        // Plan:
        //   Create a catalog.  Add 6 objects to it, verifying state after
        //   each addition.  Remove one of the objects and verify the state.
        //   Replace one of the object and verify the state.  Invoke
        //   'removeAll' and verify the state.
        //
        //   Create a catalog.  Add 3 objects to it, invoke
        //   'removeAll(buffer)' and verify the state.
        //
        //   Create a catalog and an iterator for it.  Add 5 objects to the
        //   catalog and then iterate through it and finally verify the state.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                TestDriver,
                                testCaseBreathingCopyOrMovable,
                                BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                TestDriver,
                                testCaseBreathingCopyOrMovable,
                                BloombergLP::bsltf::MoveOnlyAllocTestType,
                                bsltf::MoveOnlyAllocTestType,
                                bsltf::MovableTestType,
                                bsltf::MovableAllocTestType);

        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                TestDriver,
                                testCaseBreathingCopyable,
                                BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
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
