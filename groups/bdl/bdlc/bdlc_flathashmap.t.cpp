// bdlc_flathashmap.t.cpp                                             -*-C++-*-

#include <bdlc_flathashmap.h>
#include <bdlc_flathashtable.h>
#include <bdlc_flathashtable_groupcontrol.h>

#include <bslalg_hasstliterators.h>

#include <bslh_fibonaccibadhashwrapper.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_haspointersemantics.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_movableref.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_systemtime.h>
#include <bsls_types.h>

#include <bsltf_allocbitwisemoveabletesttype.h>
#include <bsltf_alloctesttype.h>
#include <bsltf_bitwisecopyabletesttype.h>
#include <bsltf_bitwisemoveabletesttype.h>
#include <bsltf_movablealloctesttype.h>
#include <bsltf_movabletesttype.h>
#include <bsltf_moveonlyalloctesttype.h>
#include <bsltf_nondefaultconstructibletesttype.h>
#include <bsltf_nonoptionalalloctesttype.h>

#include <bsl_algorithm.h>
#include <bsl_functional.h>
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <bsl_initializer_list.h>
#endif
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#if defined(BDE_BUILD_TARGET_EXC)
#include <stdexcept>
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a container implementing a flat hash map.
// The general concerns are correctness, exception safety, and proper
// dispatching.  For all of the object's functionality except 'print' and
// 'operator<<', this simplifies to verifying the forwarding to the
// implementation object 'bdlc::FlatHashTable'.
//
// Primary Manipulators:
//: o 'clear'
//: o 'erase(key)'
//: o 'insert'
//: o 'reset'
//
// Basic Accessors:
//: o 'allocator'
//: o 'capacity'
//: o 'find'
//: o 'hash_function'
//: o 'key_eq'
//: o 'max_load_factor'
//: o 'size'
//
// Certain standard value-semantic-type test cases are omitted:
//: o [ 3] FlatHashMap& gg(FlatHashMap *object, const char *spec);
//: o [ 3] int ggg(FlatHashMap *object, const char *spec, int verboseFlag);
//: o [10] BDEX
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] FlatHashMap();
// [ 2] FlatHashMap(Allocator *basicAllocator);
// [ 2] FlatHashMap(size_t capacity);
// [ 2] FlatHashMap(size_t capacity, Allocator *basicAllocator);
// [ 2] FlatHashMap(size_t capacity, const HASH&, Allocator *bA = 0);
// [ 2] FlatHashMap(size_t, const HASH&, const EQUAL&, Allocator * = 0);
// [20] FlatHashMap(INPUT_ITERATOR, INPUT_ITERATOR, Allocator *bA = 0);
// [20] FlatHashMap(INPUT_ITER, INPUT_ITER, size_t, Allocator * = 0);
// [20] FlatHashMap(II, II, size_t, const HASH&, Allocator * = 0);
// [20] FlatHashMap(II, II, size_t, const H&, const EQ&, Alloc * = 0);
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
// [21] FlatHashMap(bsl::initializer_list<v_t> values, Allocator * = 0);
// [21] FlatHashMap(bsl::initializer_list<v_t>, size_t, Allocator * = 0);
// [21] FlatHashMap(init_list<v_t>, size_t, const HASH&, Allocator * = 0);
// [21] FlatHashMap(init_list<v_t>, size_t, const H&, const EQ&, A * = 0);
// #endif
// [ 7] FlatHashMap(const FlatHashMap&, Allocator *bA = 0);
// [13] FlatHashMap(FlatHashMap&&);
// [14] FlatHashMap(FlatHashMap&&, Allocator *basicAllocator);
// [ 2] ~FlatHashMap();
//
// MANIPULATORS
// [ 9] FlatHashMap& operator=(const FlatHashMap&);
// [15] FlatHashMap& operator=(FlatHashMap&&);
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
// [22] FlatHashMap& operator=(bsl::initializer_list<v_t> values);
// #endif
// [25] VALUE& operator[](FORWARD_REF(KEY) key);
// [29] VALUE& at(const KEY& key);
// [ 2] void clear();
// [12] bsl::pair<iterator, iterator> equal_range(const KEY& key);
// [ 2] size_t erase(const KEY&);
// [17] iterator erase(const_iterator);
// [17] iterator erase(iterator);
// [18] iterator erase(const_iterator, const_iterator);
// [24] iterator find(const KEY& key);
// [ 2] bsl::pair<iterator, bool> insert(FORWARD_REF(VALUE_TYPE) entry)
// [28] iterator insert(const_iterator, FORWARD_REF(VALUE_TYPE) entry)
// [16] void insert(INPUT_ITERATOR, INPUT_ITERATOR);
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
// [23] void insert(bsl::initializer_list<value_type> values);
// #endif
// [19] void rehash(size_t);
// [19] void reserve(size_t);
// [ 2] void reset();
// [12] iterator begin();
// [12] iterator end();
// [ 8] void swap(FlatHashMap&);
//
// ACCESSORS
// [29] const VALUE& at(const KEY& key) const;
// [ 4] size_t capacity() const;
// [12] bool contains(const KEY&) const;
// [12] bsl::size_t count(const KEY& key) const;
// [11] bool empty() const;
// [12] bsl::pair<ci, ci> equal_range(const KEY&) const;
// [ 4] const_iterator find(const KEY&) const;
// [ 4] HASH hash_function() const;
// [ 4] EQUAL key_eq() const;
// [11] float load_factor() const;
// [ 4] float max_load_factor() const;
// [ 4] size_t size() const;
// [12] const_iterator begin() const;
// [12] const_iterator cbegin() const;
// [12] const_iterator cend() const;
// [12] const_iterator end() const;
// [ 4] Allocator *allocator() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const FlatHashMap&, const FlatHashMap&);
// [ 6] bool operator!=(const FlatHashMap&, const FlatHashMap&);
// [ 5] ostream& operator<<(ostream& stream, const FlatHashMap& map);
//
// FREE FUNCTIONS
// [ 8] void swap(FlatHashMap&, FlatHashMap&);
// ----------------------------------------------------------------------------
// [31] USAGE EXAMPLE
// [26] CONCERN: 'FlatHashMap' has the necessary type traits
// [27] DRQS 165583038: 'insert' with conversion can crash
// [30] DRQS 169531176: bsl::inserter compatibility on Sun
// [ 1] BREATHING TEST
// [-1] PERFORMANCE TEST
// ----------------------------------------------------------------------------

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

#define ASSERT  BSLIM_TESTUTIL_ASSERT
#define ASSERTV BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q  BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P  BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_ BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLIM_TESTUTIL_L_  // current Line number

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

const bsl::uint8_t k_SIZE = bdlc::FlatHashTable_GroupControl::k_SIZE;

// ============================================================================
//                     GLOBAL VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;

// ============================================================================
//                  GLOBAL CLASSES/STRUCTS FOR TESTING
// ----------------------------------------------------------------------------

                             // ================
                             // class SeedIsHash
                             // ================

template <class TYPE>
class SeedIsHash {
    // This class template provides a hash algorithm that returns the specified
    // seed value for all hash requests.

    bsl::size_t d_seed;  // value to return for all hash requests

  public:
    // CREATORS
    SeedIsHash()
        // Create a 'SeedIsHash' object having 0 as the seed value.
    : d_seed(0)
    {
    }

    explicit SeedIsHash(bsl::size_t seed)
        // Create a 'SeedIsHash' object having the specified 'seed'.
    : d_seed(seed)
    {
    }

    SeedIsHash(const SeedIsHash& original)
        // Create a 'SeedIsHash' object having the value of the specified
        // 'original' object.
    : d_seed(original.d_seed)
    {
    }

    // MANIPULATORS
    SeedIsHash& operator=(const SeedIsHash& rhs)
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.
    {
        d_seed = rhs.d_seed;
        return *this;
    }

    // ACCESSORS
    bsl::size_t operator()(const TYPE&) const
        // Return the provided-at-construction seed value.
    {
        return d_seed;
    }
};

                          // =====================
                          // class TestValueIsHash
                          // =====================

template <class KEY>
class TestValueIsHash {
    // This class provides a hash algorithm that provides a simple mapping from
    // 'KEY' to hash.  The (template parameter) type 'KEY' must have a 'data'
    // method that returns an integral value.

  public:
    // ACCESSORS
    bsl::size_t operator()(const KEY& key) const
        // Return 'key.data()' for the specified 'key'.
    {
        return static_cast<bsl::size_t>(key.data());
    }
};

                           // ===================
                           // class EqualAndState
                           // ===================

template <class TYPE>
class EqualAndState {
    // This class template provides an equality functor that also has a value
    // useful for testing when the equality functor should be copied.

    TYPE d_state;  // state value

  public:
    // CREATORS
    EqualAndState()
        // Create an 'EqualAndState' object having the default state value.
    : d_state()
    {
    }

    explicit EqualAndState(const TYPE& state)
        // Create an 'EqualAndState' object having the specified 'state'.
    : d_state(state)
    {
    }

    EqualAndState(const EqualAndState& original)
        // Create an 'EqualAndState' object having the value of the specified
        // 'original' object.
    : d_state(original.d_state)
    {
    }

    // MANIPULATORS
    EqualAndState& operator=(const EqualAndState& rhs)
        // Assign to this object the value of the specified 'rhs', and return a
        // reference providing modifiable access to this object.
    {
        d_state = rhs.d_state;
        return *this;
    }

    // ACCESSORS
    bool operator()(const TYPE& lhs, const TYPE& rhs) const
        // Return, for the specified 'lhs' and 'rhs', 'lhs == rhs'.
    {
        return lhs == rhs;
    }

    const TYPE& state() const
        // Return a non-modifiable reference to the state value.
    {
        return d_state;
    }
};

// ============================================================================
//                     GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

template <class KEY>
void testCase15MoveAssignment(int id, bool allocates)
    // Address the move assignment concerns of test case 15 for the specified
    // 'id', that can be used to determine the 'KEY' in case of a test failure,
    // for a type that allocates if the specified 'allocates' is 'true'.
{
    typedef bdlc::FlatHashMap<KEY, int, TestValueIsHash<KEY> > Obj;
    typedef bsl::pair<KEY, int>                                Entry;

    {
        typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj> Obj);

        // Verify that the signature and return type are standard.

        operatorPtr operatorMoveAssignment = &Obj::operator=;

        // Quash unused variable warning.

        Obj a;
        Obj b;
        (a.*operatorMoveAssignment)(bslmf::MovableRefUtil::move(b));
    }

    {
        // Moved-from object has same allocator as moved-to object.

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        bsls::ObjectBuffer<Entry> entry1;
        bslma::ConstructionUtil::construct(entry1.address(),
                                           &oa,
                                           bsl::make_pair(1, 1));

        bslma::DestructorGuard<Entry> guard1(&entry1.object());

        bsls::ObjectBuffer<Entry> entry2;
        bslma::ConstructionUtil::construct(entry2.address(),
                                           &oa,
                                           bsl::make_pair(2, 2));

        bslma::DestructorGuard<Entry> guard2(&entry2.object());

        Obj mX(&oa);  const Obj& X = mX;

        Obj mY(&oa);  const Obj& Y = mY;
        mY.insert(entry1.object());

        Obj mYY(Y, &oa);  const Obj& YY = mYY;

        Obj mZ(&oa);  const Obj& Z = mZ;
        mZ.insert(entry2.object());

        Obj mZZ(Z, &oa);  const Obj& ZZ = mZZ;

        bsls::Types::Int64 expAllocations = oa.numAllocations();

        {
            Obj& rv = (mX = bslmf::MovableRefUtil::move(mY));

            LOOP_ASSERT(id,   X == YY);
            LOOP_ASSERT(id, &rv == &X);
        }
        {
            Obj& rv = (mX = bslmf::MovableRefUtil::move(mZ));

            LOOP_ASSERT(id,   X == ZZ);
            LOOP_ASSERT(id, &rv == &X);
        }

        LOOP_ASSERT(id, expAllocations == oa.numAllocations());
    }

    {
        // Moved-from object has different allocator than moved-to object.

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

        bsls::ObjectBuffer<Entry> entry1;
        bslma::ConstructionUtil::construct(entry1.address(),
                                           &oa,
                                           bsl::make_pair(1, 1));

        bslma::DestructorGuard<Entry> guard1(&entry1.object());

        bsls::ObjectBuffer<Entry> entry2;
        bslma::ConstructionUtil::construct(entry2.address(),
                                           &sa,
                                           bsl::make_pair(2, 2));

        bslma::DestructorGuard<Entry> guard2(&entry2.object());

        Obj mX(&oa);  const Obj& X = mX;

        Obj mY(&sa);  const Obj& Y = mY;
        mY.insert(entry1.object());

        Obj mYY(Y, &sa);  const Obj& YY = mYY;

        Obj mZ(&sa);  const Obj& Z = mZ;
        mZ.insert(entry2.object());

        Obj mZZ(Z, &sa);  const Obj& ZZ = mZZ;

        bsls::Types::Int64 expAllocations = oa.numAllocations() + (  allocates
                                                                   ? 6
                                                                   : 4);

        {
            Obj& rv = (mX = bslmf::MovableRefUtil::move(mY));

            LOOP_ASSERT(id,   X == YY);
            LOOP_ASSERT(id, &rv == &X);
        }
        {
            Obj& rv = (mX = bslmf::MovableRefUtil::move(mZ));

            LOOP_ASSERT(id,   X == ZZ);
            LOOP_ASSERT(id, &rv == &X);
        }

        LOOP_ASSERT(id, expAllocations == oa.numAllocations());
    }
}

template <class KEY>
void testCase14MoveConstructorWithAllocator(int id)
    // Address the move constructor with allocator concerns of test case 14 for
    // the specified 'id' that can be used to determine the 'KEY' in case of a
    // test failure.
{
    typedef bdlc::FlatHashMap<KEY, int, TestValueIsHash<KEY> > Obj;
    typedef bsl::pair<KEY, int>                                Entry;

    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

    for (int i = 0; i < 2; ++i) {
        // Create control object 'W'.
        Obj mW(&oa);  const Obj& W = mW;

        bsls::ObjectBuffer<Entry> entry;
        bslma::ConstructionUtil::construct(entry.address(),
                                           &oa,
                                           bsl::make_pair(i, i));

        bslma::DestructorGuard<Entry> guard(&entry.object());

        mW.insert(entry.object());

        LOOP_ASSERT(id, i == W.begin()->first.data());

        for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            bslma::TestAllocator za("different", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // Create source object 'Y'.
            bsls::ObjectBuffer<Obj> bufferY;

            Obj *pY = bufferY.address();
            bslma::ConstructionUtil::construct(pY, &sa, W);

            Obj& mY = *pY;  const Obj& Y = mY;

            LOOP_ASSERT(id, Y == W);

            bslma::TestAllocatorMonitor oam(&da), sam(&sa);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;
            bslma::TestAllocator *othAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                oam.reset(&sa);
                objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mY));
                objAllocatorPtr = &sa;
                othAllocatorPtr = &da;
              } break;
              case 'b': {
                oam.reset(&da);
                objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mY),
                                      (bslma::Allocator *)0);
                objAllocatorPtr = &da;
                othAllocatorPtr = &za;
              } break;
              case 'c': {
                oam.reset(&sa);
                objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mY), &sa);
                objAllocatorPtr = &sa;
                othAllocatorPtr = &da;
              } break;
              case 'd': {
                oam.reset(&za);
                objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mY), &za);
                objAllocatorPtr = &za;
                othAllocatorPtr = &da;
              } break;
              default: {
                LOOP2_ASSERT(id, CONFIG, !"Bad allocator config.");
                return;                                               // RETURN
              }
            }

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = *othAllocatorPtr;

            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify the value of the object.
            LOOP_ASSERT(id, X == W);

            // Verify that 'X' and 'Y' have the correct allocator.
            LOOP_ASSERT(id, &oa == X.allocator());
            LOOP_ASSERT(id, &sa == Y.allocator());

            // Verify no allocation from the non-object allocator and that the
            // object allocator is hooked up.
            if ('a' != CONFIG) {
                LOOP_ASSERT(id, 0 == noa.numBlocksTotal());
            }
            if (0 != X.capacity()) {
                LOOP_ASSERT(id, 0 < oa.numBlocksTotal());
            }

            // Verify 'Y' is in a valid state.
            pY->~Obj();

            // Verify the value of the object after destruction of 'Y'.
            LOOP_ASSERT(id, X == W);

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.
            LOOP_ASSERT(id, 0 == fa.numBlocksInUse());
            LOOP_ASSERT(id, 0 == da.numBlocksInUse());
            LOOP_ASSERT(id, 0 == sa.numBlocksInUse());
            LOOP_ASSERT(id, 0 == za.numBlocksInUse());
        }
    }
}

template <class KEY>
void testCase13MoveConstructorWithoutAllocator(int id)
    // Address the move constructor without allocator concerns of test case 13
    // for the specified 'id' that can be used to determine the 'KEY' in case
    // of a test failure.
{
    typedef bdlc::FlatHashMap<KEY, int, TestValueIsHash<KEY> > Obj;
    typedef bsl::pair<KEY, int>                                Entry;

    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

    for (int i = 0; i < 2; ++i) {
        // Create control object 'W'.
        Obj mW(&oa);  const Obj& W = mW;

        bsls::ObjectBuffer<Entry> entry;
        bslma::ConstructionUtil::construct(entry.address(),
                                           &oa,
                                           bsl::make_pair(i, i));

        bslma::DestructorGuard<Entry> guard(&entry.object());

        mW.insert(entry.object());

        LOOP_ASSERT(id, i == W.begin()->first.data());

        // Create source object 'Y' in an object buffer so it may be deleted.
        bsls::ObjectBuffer<Obj> objY;
        bslma::ConstructionUtil::construct(objY.address(), &oa, W);
        Obj& mY = objY.object();  const Obj& Y = mY;

        LOOP_ASSERT(id, Y == W);

        bsls::Types::Int64 expNumAllocations = oa.numAllocations();

        // Move construct.
        Obj mX(bslmf::MovableRefUtil::move(mY));  const Obj& X = mX;

        // Verify no memory was ever allocated (default and global allocators
        // are checked in 'main').

        LOOP_ASSERT(id, expNumAllocations == oa.numAllocations());

        // Verify the value of the object.
        LOOP_ASSERT(id, X == W);

        // Verify 'Y' is valid.
        objY.object().~Obj();

        // Verify 'X' is valid after 'Y' is destroyed and still equal to 'W'.
        LOOP_ASSERT(id, X == W);
    }
}

template <class KEY>
void testCaseInsert(int  id,
                    bool allocates,
                    bool allocatesOnRehash,
                    bool useHint)
    // Address the 'insert' concerns of test cases 2 and 28 for the specified
    // 'id', that can be used to determine the 'KEY' in case of a test failure,
    // for a type that allocates if the specified 'allocates' is 'true', and
    // allocates on rehashes if the specified 'allocatesOnRehash' is 'true'.
    // Use 'insert' with a hint if the specified 'useHint' is 'true'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef bdlc::FlatHashMap<KEY, int, TestValueIsHash<KEY> > Obj;
    typedef bsl::pair<KEY, int>                                Entry;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    bsls::Types::Int64 expAllocations = oa.numAllocations();
    bsl::size_t        expCapacity    = 0;

    Obj mX(&oa);  const Obj& X = mX;

    for (int i = 0; i <= thirdAlloc; ++i) {
        bsls::ObjectBuffer<Entry> entry;
        bslma::ConstructionUtil::construct(entry.address(),
                                           &oa,
                                           bsl::make_pair(i, i));

        bslma::DestructorGuard<Entry> guard(&entry.object());

        if (!useHint) {
            bsl::pair<typename Obj::iterator, bool> rv;

            rv = mX.insert(entry.object());
            LOOP2_ASSERT(id, i,    i == rv.first->first.data());
            LOOP2_ASSERT(id, i, true == rv.second);

            rv = mX.insert(entry.object());
            LOOP2_ASSERT(id, i,     i == rv.first->first.data());
            LOOP2_ASSERT(id, i, false == rv.second);
        }
        else {
            typename Obj::iterator rv;

            rv = mX.insert(X.begin(), entry.object());
            LOOP2_ASSERT(id, i, i == rv->first.data());

            rv = mX.insert(X.end(), entry.object());
            LOOP2_ASSERT(id, i, i == rv->first.data());
        }

        typename Obj::const_iterator iter = X.find(entry.object().first);
        LOOP2_ASSERT(id, i, i == iter->first.data());

        entry.object().first.setData(i + 1);
        iter = X.find(entry.object().first);
        LOOP2_ASSERT(id, i, X.end() == iter);

        if (allocates) {
            expAllocations += 2;
        }

        if (allocatesOnRehash) {
            if (thirdAlloc == i) {
                expCapacity    *= 2;
                expAllocations += 2 + X.size() - 1; // insert counted above
            }
            else if (secondAlloc == i) {
                expCapacity    *= 2;
                expAllocations += 2 + X.size() - 1; // insert counted above
            }
            else if (0 == i) {
                expCapacity     = 2 * k_SIZE;
                expAllocations += 2;
            }
        }
        else {
            if (thirdAlloc == i) {
                expCapacity    *= 2;
                expAllocations += 2;
            }
            else if (secondAlloc == i) {
                expCapacity    *= 2;
                expAllocations += 2;
            }
            else if (0 == i) {
                expCapacity     = 2 * k_SIZE;
                expAllocations += 2;
            }
        }

        LOOP2_ASSERT(id, i,    expCapacity == X.capacity());
        LOOP2_ASSERT(id, i, expAllocations == oa.numAllocations());
        LOOP2_ASSERT(id, i,          i + 1 == static_cast<int>(X.size()));
    }
}

template <class KEY>
void testCaseInsertMove(int id, bool allocates, bool moveable, bool useHint)
    // Address the move 'insert' concerns of test cases 2 and 28 for the
    // specified 'id' that can be used to determine 'KEY' in case of a test
    // failure, for a type that allocates if the specified 'allocates' is
    // 'true', and can be moved if the specified 'moveable' is 'true'.  Use
    // 'insert' with a hint if the specified 'useHint' is 'true'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef bdlc::FlatHashMap<KEY, int, TestValueIsHash<KEY> > Obj;
    typedef bsl::pair<KEY, int>                                Entry;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    Obj mX(&oa);  const Obj& X = mX;

    bsls::Types::Int64 expAllocations = oa.numAllocations();
    bsl::size_t        expCapacity    = 0;

    for (int i = 0; i <= thirdAlloc; ++i) {
        bsls::ObjectBuffer<Entry> entry1;
        bslma::ConstructionUtil::construct(entry1.address(), &oa, i, i);

        bslma::DestructorGuard<Entry> guard1(&entry1.object());

        bsls::ObjectBuffer<Entry> entry2;
        bslma::ConstructionUtil::construct(entry2.address(), &oa, i, i);

        bslma::DestructorGuard<Entry> guard2(&entry2.object());

        if (!useHint) {
            bsl::pair<typename Obj::iterator, bool> rv;

            rv = mX.insert(bslmf::MovableRefUtil::move(entry1.object()));
            LOOP2_ASSERT(id, i,    i == rv.first->first.data());
            LOOP2_ASSERT(id, i, true == rv.second);

            rv = mX.insert(bslmf::MovableRefUtil::move(entry2.object()));
            LOOP2_ASSERT(id, i,     i == rv.first->first.data());
            LOOP2_ASSERT(id, i, false == rv.second);
        }
        else {
            typename Obj::iterator rv;

            rv = mX.insert(X.begin(),
                           bslmf::MovableRefUtil::move(entry1.object()));
            LOOP2_ASSERT(id, i, i == rv->first.data());

            rv = mX.insert(X.end(),
                           bslmf::MovableRefUtil::move(entry2.object()));
            LOOP2_ASSERT(id, i, i == rv->first.data());
        }

        typename Obj::const_iterator iter = X.find(entry2.object().first);
        LOOP2_ASSERT(id, i, i == iter->first.data());

        entry2.object().first.setData(i + 1);
        iter = X.find(entry2.object().first);
        LOOP2_ASSERT(id, i, X.end() == iter);

        if (allocates) {
            if (   moveable
                && (   !bslmf::IsBitwiseMoveable<KEY>::value
                    || bsl::is_trivially_copyable<KEY>::value)) {
                expAllocations += 2;
            }
            else {
                expAllocations += 3;
            }
        }

        if (allocates && !moveable) {
            if (thirdAlloc == i) {
                expCapacity    *= 2;
                expAllocations += 2 + X.size() - 1; // insert counted above
            }
            else if (secondAlloc == i) {
                expCapacity    *= 2;
                expAllocations += 2 + X.size() - 1; // insert counted above
            }
            else if (0 == i) {
                expCapacity     = 2 * k_SIZE;
                expAllocations += 2;
            }
        }
        else {
            if (thirdAlloc == i) {
                expCapacity    *= 2;
                expAllocations += 2;
            }
            else if (secondAlloc == i) {
                expCapacity    *= 2;
                expAllocations += 2;
            }
            else if (0 == i) {
                expCapacity     = 2 * k_SIZE;
                expAllocations += 2;
            }
        }

        LOOP2_ASSERT(id, i,    expCapacity == X.capacity());
        LOOP2_ASSERT(id, i, expAllocations == oa.numAllocations());
        LOOP2_ASSERT(id, i,          i + 1 == static_cast<int>(X.size()));
    }
}

static unsigned int s_antiOptimization = 0;

template <class MAP>
bsls::TimeInterval performanceFindPresent(MAP *map)
    // For the specified 'map', insert a large number of values and then invoke
    // 'find()' with values matching those inserted.  Return the duration of
    // the 'find()' invocations.
{
    const int NUM_TRIAL = 101;
    const int MAX       = 4096;
    const int NUM_ITER  = 4096 * 128 / MAX;

    for (short i = 0; i < MAX; ++i) {
        map->insert(bsl::make_pair(i, i));
    }

    bsl::vector<bsls::TimeInterval> results;
    for (int trial = 0; trial < NUM_TRIAL; ++trial) {
        for (int iter = 0; iter < NUM_ITER; ++iter) {
            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            for (short i = 0; i < MAX; ++i) {
                s_antiOptimization +=
                     map->find(static_cast<short>((i * 7) & (MAX - 1)))->first;
            }

            results.push_back(bsls::SystemTime::nowMonotonicClock() - start);
        }
    }

    bsl::sort(results.begin(), results.end());

    return results[NUM_TRIAL / 2];
}

template <class MAP>
bsls::TimeInterval performanceFindNotPresent(MAP *map)
    // For the specified 'map', insert a large number of values and then invoke
    // 'find()' with values not matching those inserted.  Return the duration
    // of the 'find()' invocations.
{
    const int NUM_TRIAL = 101;
    const int MAX       = 4096;
    const int NUM_ITER  = 4096 * 128 / MAX;

    for (short i = 0; i < MAX; ++i) {
        map->insert(bsl::make_pair(static_cast<short>(i * 2),
                                   static_cast<short>(i * 2)));
    }

    bsl::vector<bsls::TimeInterval> results;
    for (int trial = 0; trial < NUM_TRIAL; ++trial) {
        for (int iter = 0; iter < NUM_ITER; ++iter) {
            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            for (short i = 0; i < MAX; ++i) {
                if (map->end() == map->find(static_cast<short>(
                                             ((i * 7) & (MAX - 1)) * 2 + 1))) {
                    ++s_antiOptimization;
                }
            }

            results.push_back(bsls::SystemTime::nowMonotonicClock() - start);
        }
    }

    bsl::sort(results.begin(), results.end());

    return results[NUM_TRIAL / 2];
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 31: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&oa);

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Gathering Document Statistics
/// - - - - - - - - - - - - - - - - - - - -
// Suppose one wished to gather statistics on the words appearing in a large
// set of documents on disk or in a database.  Gathering those statistics is
// intrusive (as one is competing for access to the documents with the regular
// users) and must be done as quickly as possible.  Moreover, the set of unique
// words appearing in those documents may be high.  The English language has in
// excess of a million words (albeit many appear infrequently), and, if the
// documents contain serial numbers, or Social Security numbers, or chemical
// formulas, etc., then the 'O[log(n)]' insertion time of ordered maps may well
// be inadequate.  An unordered map, having an 'O[1]' typical insertion cost,
// is a viable alternative.
//
// This example illustrates the use of 'bdlc::FlatHashMap' to gather one simple
// statistic (counts of unique words) on a portion of a single document.  To
// avoid irrelevant details of acquiring the data, the data is stored in static
// arrays:
//..
    static char document[] =
    " IN CONGRESS, July 4, 1776.\n"
    "\n"
    " The unanimous Declaration of the thirteen united States of America,\n"
    "\n"
    " When in the Course of human events, it becomes necessary for one\n"
    " people to dissolve the political bands which have connected them with\n"
    " another, and to assume among the powers of the earth, the separate\n"
    " and equal station to which the Laws of Nature and of Nature's G-d\n"
    " entitle them, a decent respect to the opinions of mankind requires\n"
    " that they should declare the causes which impel them to the\n"
    " separation.  We hold these truths to be self-evident, that all men\n"
    " are created equal, that they are endowed by their Creator with\n"
    " certain unalienable Rights, that among these are Life, Liberty and\n"
    " the pursuit of Happiness.  That to secure these rights, Governments\n"
    " are instituted among Men, deriving their just powers from the consent\n"
    " of the governed, That whenever any Form of Government becomes\n";
//..
// First, we define an alias to make our code more comprehensible:
//..
    typedef bdlc::FlatHashMap<bsl::string, int> WordTally;
//..
// Next, we create an (empty) flat hash map to hold our word tallies:
//..
    WordTally wordTally;
//..
// Then, we define the set of characters that define word boundaries:
//..
    const char *delimiters = " \n\t,:;.()[]?!/";
//..
// Next, we extract the words from our document.  Note that 'strtok' modifies
// the document array (which was not made 'const').
//
// For each iteration of the loop, a map entry matching the key value parsed by
// 'strtok' is obtained.  On the first occurrence of a word, the map has no
// such entry, so one is created with a default value of the mapped value (0,
// just what we want in this case) and inserted into the map where it is found
// on any subsequent occurrences of the word.  The 'operator[]' method returns
// a reference providing modifiable access to the mapped value.  Here, we apply
// the '++' operator to that reference to maintain a tally for the word:
//..
    for (char *cur = strtok(document, delimiters);
               cur;
               cur = strtok(NULL,     delimiters)) {
        ++wordTally[bsl::string(cur)];
    }
//..
// Now that the data has been (quickly) gathered, we can indulge in analysis
// that is more time consuming.  For example, we can define a comparison
// function, copy the data to another container (e.g., 'bsl::vector'), sort the
// entries, and determine the 10 most commonly used words in the given
// documents:
//..
    typedef bsl::pair<bsl::string, int> WordTallyEntry;
        // Assignable equivalent to 'WordTally::value_type'.  Note that
        // 'bsl::vector' requires assignable types.

    struct WordTallyEntryCompare {
        static bool lessThan(const WordTallyEntry& a,
                             const WordTallyEntry& b) {
            return a.second < b.second;
        }
        static bool greaterThan(const WordTallyEntry& a,
                                const WordTallyEntry& b) {
            return a.second > b.second;
        }
    };

    bsl::vector<WordTallyEntry> array(wordTally.cbegin(), wordTally.cend());

    ASSERT(10 <= array.size());

    bsl::partial_sort(array.begin(),
                      array.begin() + 10,
                      array.end(),
                      WordTallyEntryCompare::greaterThan);
//..
// Notice that 'partial_sort' suffices here since we seek only the 10 most used
// words, not a complete distribution of word counts.
//
// Finally, we print the sorted portion of 'array':
//..
    for (bsl::vector<WordTallyEntry>::const_iterator cur  = array.begin(),
                                                     end  = cur + 10;
                                                     end != cur; ++cur) {
        printf("%-10s %4d\n", cur->first.c_str(), cur->second);
    }
//..
// and standard output shows:
//..
//  the          13
//  of           10
//  to            7
//  that          4
//  are           4
//  and           4
//  which         3
//  these         3
//  them          3
//  among         3
//..
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // INSERTER
        //   Interoperation with 'bsl::inserter' works as expected.
        //
        // Concerns:
        //: 1 A 'bsl::inserter' object can be constructed from the class.
        //:
        //: 2 Use of the 'bsl::inserter' object with a STL algorithm adds items
        //:   to the collection as expected.
        //
        // Plan:
        //: 1 Construct a 'bsl::inserter' from the class. (C-1)
        //:
        //: 2 Invoke 'bsl::fill_n' on the inserter. (C-2)
        //
        // Testing:
        //   DRQS 169531176: bsl::inserter compatibility on Sun
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INSERTER" << endl
                          << "========" << endl;

        typedef bdlc::FlatHashMap<int, int> Obj;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        bsl::fill_n(bsl::inserter(mX, mX.begin()), 5, Obj::value_type(1,2));
        ASSERTV(X.size(), 1 == X.size());
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING 'at' METHOD
        //   Ensure the 'at' method operates as expected.
        //
        // Concerns:
        //: 1 The 'at' method correctly forwards to the underlying
        //:   implementation and correctly forwards the return value.
        //:
        //: 2 The returned reference can be used to modify the container.
        //:
        //: 3 The methods throw 'std::out_of_range' exception if the key does
        //:   not exist.
        //:
        //: 4 Any memory allocations come from the object's allocator.
        //
        // Plan:
        //: 1 Create an object and fill it with some key-value pairs using a
        //    loop-based approach.
        //:
        //: 2 For each key-value pair in the object:
        //:
        //:   1 Verify that both versions of 'at' return the expected 'VALUE'.
        //:
        //:   2 Set the value to something different using non-const version
        //:     and verify the value is changed.  (C-1..2)
        //:
        //: 3 Invoke both versions of 'at' method with a key that does not
        //:   exist in the container and verify 'std::out_of_range' is thrown.
        //:   (C-3)
        //:
        //: 4 Verify no memory is allocated from the default allocator.  (C-4)
        //
        // Testing:
        //   VALUE& at(const KEY&);
        //   const VALUE& at(const KEY&) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'at' METHOD" << endl
                          << "===================" << endl;

        typedef bdlc::FlatHashMap<int, int> Obj;

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&defaultAllocator);

        if (verbose)
            cout << "\tVerify expected behavior for existing elements."
                 << endl;
        {
            const int NUM_ELEMENTS = 10;

            Obj        mX(&oa);
            const Obj& X = mX;

            // Fill container.

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                mX[i] = NUM_ELEMENTS - i;
            }

            // Check return values.

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                const int EXP = NUM_ELEMENTS - i;
                ASSERTV(i,  mX.at(i), EXP == mX.at(i));
                ASSERTV(i,   X.at(i), EXP ==  X.at(i));
            }

            // Check the possibility of container change.

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                mX.at(i) = i;
            }

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                ASSERTV(i, X.at(i), i ==  X.at(i));
            }
        }

#if defined(BDE_BUILD_TARGET_EXC)
        if (verbose)
            cout << "\tVerify expected behavior for non-existing elements."
                 << endl;
        {
            const int NUM_ELEMENTS     = 5;
            const int NON_EXISTING_KEY = NUM_ELEMENTS + 1;

            Obj        mX(&oa);
            const Obj& X = mX;

            // Fill container.

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                mX[i] = NUM_ELEMENTS - i;
            }

            ASSERT(X.end() == X.find(NON_EXISTING_KEY));

            // on 'const Obj'
            {
                bool exceptionCaught = false;
                try {
                    X.at(NON_EXISTING_KEY);
                }
                catch (const std::out_of_range&) {
                    exceptionCaught = true;
                }
                ASSERT(true == exceptionCaught);
            }

            // on 'Obj'
            {
                bool exceptionCaught = false;
                try {
                    mX.at(NON_EXISTING_KEY);
                }
                catch (const std::out_of_range&) {
                    exceptionCaught = true;
                }
                ASSERTV(true == exceptionCaught);
            }
        }
#endif

        ASSERT(dam.isTotalSame()); // default allocator unused
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // HINT INSERT
        //   The hint insert method operates as expected.
        //
        // Concerns:
        //: 1 The manipulator 'insert' with a hint correctly forwards to the
        //:   implementation class, correctly forwards the return value from
        //:   the implementation class, and honors
        //:   bitwise-copy/bitwise-move/move.
        //
        // Plan:
        //: 1 Create objects using the 'bslma::TestAllocator', use the 'insert'
        //:   with a hint method with various values, verify the return value,
        //:   and use the basic accessors to verify the value of the object.
        //:   Use 'bsltf' test types to verify bitwise-copy/bitwise-move/move
        //:   are honored.  (C-1)
        //
        // Testing:
        //   iterator insert(const_iterator, FORWARD_REF(VALUE_TYPE) entry)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "HINT INSERT" << endl
                          << "===========" << endl;

        if (verbose) cout << "Testing copy 'insert' with hint." << endl;
        {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            testCaseInsert<bsltf::AllocBitwiseMoveableTestType>(1,
                                                                true,
                                                                false,
                                                                true);
            testCaseInsert<bsltf::AllocTestType>(2, true, true, true);
            testCaseInsert<bsltf::BitwiseCopyableTestType>(3,
                                                           false,
                                                           false,
                                                           true);
            testCaseInsert<bsltf::BitwiseMoveableTestType>(4,
                                                           false,
                                                           false,
                                                           true);
            testCaseInsert<bsltf::MovableAllocTestType>(5, true, false, true);
            testCaseInsert<bsltf::MovableTestType>(6, false, false, true);
            testCaseInsert<bsltf::NonDefaultConstructibleTestType>(7,
                                                                   false,
                                                                   false,
                                                                   true);
            testCaseInsert<bsltf::NonOptionalAllocTestType>(8,
                                                            true,
                                                            true,
                                                            true);
#else
            testCaseInsert<bsltf::AllocBitwiseMoveableTestType>(1,
                                                                true,
                                                                false,
                                                                true);
            testCaseInsert<bsltf::AllocTestType>(2, true, true, true);
            testCaseInsert<bsltf::BitwiseCopyableTestType>(3,
                                                           false,
                                                           false,
                                                           true);
            testCaseInsert<bsltf::BitwiseMoveableTestType>(4,
                                                           false,
                                                           false,
                                                           true);
            testCaseInsert<bsltf::MovableAllocTestType>(5, true, true, true);
            testCaseInsert<bsltf::MovableTestType>(6, false, false, true);
            testCaseInsert<bsltf::NonDefaultConstructibleTestType>(7,
                                                                   false,
                                                                   false,
                                                                   true);
            testCaseInsert<bsltf::NonOptionalAllocTestType>(8,
                                                            true,
                                                            true,
                                                            true);
#endif
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) cout << "Testing move 'insert' with hint." << endl;
        {
            testCaseInsertMove<bsltf::AllocBitwiseMoveableTestType>(1,
                                                                    true,
                                                                    true,
                                                                    true);
            testCaseInsertMove<bsltf::AllocTestType>(2, true, false, true);
            testCaseInsertMove<bsltf::BitwiseCopyableTestType>(3,
                                                               false,
                                                               true,
                                                               true);
            testCaseInsertMove<bsltf::BitwiseMoveableTestType>(4,
                                                               false,
                                                               true,
                                                               true);
            testCaseInsertMove<bsltf::MovableAllocTestType>(5,
                                                            true,
                                                            true,
                                                            true);
            testCaseInsertMove<bsltf::MovableTestType>(6, false, true, true);

            testCaseInsertMove<bsltf::MoveOnlyAllocTestType>(7,
                                                             true,
                                                             true,
                                                             true);

            testCaseInsertMove<bsltf::NonDefaultConstructibleTestType>(8,
                                                                       false,
                                                                       false,
                                                                       true);
            testCaseInsertMove<bsltf::NonOptionalAllocTestType>(9,
                                                                true,
                                                                false,
                                                                true);
        }
#endif
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // DRQS 165583038: 'insert' with conversion can crash
        //
        // Concerns:
        //: 1 The 'insert' method does not crash when a conversion occurs.
        //
        // Plan:
        //: 1 Verify the following code does not crash.  (C-1)
        //
        // Testing:
        //   DRQS 165583038: 'insert' with conversion can crash
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "DRQS 165583038: 'insert' with conversion can crash"
                 << endl
                 << "=================================================="
                 << endl;
        }

        typedef bdlc::FlatHashMap<bsl::string, int> Obj;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(64, &oa);  const Obj& X = mX;

        mX.insert(bsl::pair<const char *, int>("abc", 1));

        ASSERT(1 == X.size());
        ASSERT(1 == X.find("abc")->second);

#if !defined(BSLS_PLATFORM_CMP_SUN)
        // The SUN compiler does not compile the reported 'bsl::make_pair'
        // example.  Also, the observed crash occured when trying to hash the
        // key, well before the actual insertion, so the following does
        // recreate the issue, and provides additional sanity checks.

        mX.insert(bsl::make_pair("abc", 2));

        ASSERT(1 == X.size());
        ASSERT(1 == X.find("abc")->second);
#endif
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TYPE TRAITS
        //   Ensure 'bdlc::FlatHashMap' has the expected type traits.
        //
        // Concerns:
        //: 1 The expected type traits are defined, and common type traits
        //:   that are not expected are undefined.
        //
        // Plan:
        //: 1 'BSLMF_ASSERT' the expected defined and expected undefined type
        //:   traits.  (C-1)
        //
        // Testing:
        //   CONCERN: 'FlatHashMap' has the necessary type traits
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TYPE TRAITS" << endl
                          << "===========" << endl;

        typedef bdlc::FlatHashMap<int, int> Obj;

        BSLMF_ASSERT(1 == bslalg::HasStlIterators<Obj>::value);
        BSLMF_ASSERT(1 == bslma::UsesBslmaAllocator<Obj>::value);

        BSLMF_ASSERT(0 == bslmf::IsBitwiseMoveable<Obj>::value);
        BSLMF_ASSERT(0 == bsl::is_trivially_copyable<Obj>::value);
        BSLMF_ASSERT(0 == bslmf::IsBitwiseEqualityComparable<Obj>::value);
        BSLMF_ASSERT(0 == bslmf::HasPointerSemantics<Obj>::value);
        BSLMF_ASSERT(0 == bsl::is_trivially_default_constructible<Obj>::value);
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // 'operator[]'
        //   Ensure the 'operator[]' method operates as expected.
        //
        // Concerns:
        //: 1 The 'operator[]' method correctly forwards to the underlying
        //:   implementation and correctly forwards the return value.
        //:
        //: 2 When the method inserts an element, the default constructed value
        //:   is used.
        //:
        //: 3 The returned reference can be used to modify the container.
        //:
        //: 4 A movable key may be supplied to the method.
        //
        // Plan:
        //: 1 Use 'operator[]' on an empty container and verify the inserted
        //:   element has the expected value.
        //:
        //: 2 Use 'operator[]' to modify an existing element.  (C-1..3)
        //:
        //: 3 Use 'operator[]' with a movable key and verify the number of
        //:   allocations done to insert an element.  (C-4)
        //
        // Testing:
        //   VALUE& operator[](FORWARD_REF(KEY) key);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'operator[]'" << endl
                          << "============" << endl;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        if (verbose) cout << "Verify expected value for an inserted element."
                          << endl;
        {
            {
                typedef bdlc::FlatHashMap<int, int> Obj;

                Obj mX(&oa);  const Obj& X = mX;

                mX[0];

                ASSERT(1 == X.size());
                ASSERT(0 == X.find(0)->second);

                mX[1];

                ASSERT(2 == X.size());
                ASSERT(0 == X.find(1)->second);
            }
            {
                typedef bdlc::FlatHashMap<int, bsl::string> Obj;

                Obj mX(&oa);  const Obj& X = mX;

                mX[0];

                ASSERT(            1 == X.size());
                ASSERT(bsl::string() == X.find(0)->second);

                mX[1];

                ASSERT(            2 == X.size());
                ASSERT(bsl::string() == X.find(1)->second);
            }
        }

        if (verbose) cout << "Verify expected behavior for existing elements."
                          << endl;
        {
            typedef bdlc::FlatHashMap<int, int> Obj;

            Obj mX(&oa);  const Obj& X = mX;

            mX[0] = 1;

            ASSERT(1 == X.size());
            ASSERT(1 == X.find(0)->second);

            mX[1] = 2;

            ASSERT(2 == X.size());
            ASSERT(1 == X.find(0)->second);
            ASSERT(2 == X.find(1)->second);

            mX[0] = 3;

            ASSERT(2 == X.size());
            ASSERT(3 == X.find(0)->second);
            ASSERT(2 == X.find(1)->second);
        }

        if (verbose) cout << "Verify expected behavior with movable key."
                          << endl;
        {
            const bsl::string SC(
                             "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU",
                             &oa);

            typedef bdlc::FlatHashMap<bsl::string, int> Obj;

            {
                // Verify number of allocations without move.

                Obj mX(&oa);

                bsls::Types::Int64 expAllocations = oa.numAllocations() + 3;

                mX[SC];

                ASSERT(expAllocations == oa.numAllocations());
            }
            {
                // Verify number of allocations with move.

                Obj mX(&oa);

                bsl::string s(SC, &oa);

                bsls::Types::Int64 expAllocations = oa.numAllocations() + 2;

                mX[bslmf::MovableRefUtil::move(s)];

                ASSERT(expAllocations == oa.numAllocations());
            }
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // 'find'
        //   Ensure the 'find' method operates as expected.
        //
        // Concerns:
        //: 1 The 'find' method correctly forwards to the underlying
        //:   implementation and correctly forwards the return value.
        //:
        //: 2 The returned iterator is not a 'const_iterator'.
        //
        // Plan:
        //: 1 Verify the results of 'find' on an object with one and two
        //:   contained elements.  (C-1)
        //:
        //: 2 Assign a value to the returned iterator's 'second' to ensure the
        //:   iterator is not a 'const_iterator'.  (C-2)
        //
        // Testing:
        //   iterator find(const KEY& key);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'find'" << endl
                          << "======" << endl;

        if (verbose) cout << "Testing 'find'." << endl;
        {
            typedef bdlc::FlatHashMap<int, int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);

            ASSERT(mX.end() == mX.find(1));
            ASSERT(mX.end() == mX.find(2));
            ASSERT(mX.end() == mX.find(3));

            mX.insert(bsl::make_pair(1, 5));

            ASSERT(       1 == mX.find(1)->first);
            ASSERT(       5 == mX.find(1)->second);
            ASSERT(mX.end() == mX.find(2));
            ASSERT(mX.end() == mX.find(3));

            mX.insert(bsl::make_pair(2, 7));

            ASSERT(       1 == mX.find(1)->first);
            ASSERT(       5 == mX.find(1)->second);
            ASSERT(       2 == mX.find(2)->first);
            ASSERT(       7 == mX.find(2)->second);
            ASSERT(mX.end() == mX.find(3));

            mX.find(1)->second = 6;

            ASSERT(       1 == mX.find(1)->first);
            ASSERT(       6 == mX.find(1)->second);
            ASSERT(       2 == mX.find(2)->first);
            ASSERT(       7 == mX.find(2)->second);
            ASSERT(mX.end() == mX.find(3));
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // INITIALIZER LIST INSERT
        //   The initializer list insert method operates as expected.
        //
        // Concerns:
        //: 1 The initializer list insert method correctly forwards to the
        //:   underlying implementation, and produces the expected value.
        //
        // Plan:
        //: 1 Create various objects and directly verify the results of the
        //:   initializer list insert method.  (C-1)
        //
        // Testing:
        //   void insert(bsl::initializer_list<value_type> values);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INITIALIZER LIST INSERT" << endl
                          << "=======================" << endl;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

        const bsl::string SA("abc",    &ta);
        const bsl::string SB("abcdef", &ta);
        const bsl::string SC("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU",
                             &ta);

        {
            typedef int                           Key;
            typedef int                           Value;
            typedef bdlc::FlatHashMap<Key, Value> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert({ { 1, 1 } });

            ASSERT(    1 == X.size());
            ASSERT( true == X.contains(1));
            ASSERT(false == X.contains(2));
            ASSERT(false == X.contains(3));

            mX.insert({ { 2, 2 }, { 3, 3 } });

            ASSERT(   3 == X.size());
            ASSERT(true == X.contains(1));
            ASSERT(true == X.contains(2));
            ASSERT(true == X.contains(3));
        }
        {
            typedef bsl::string                   Key;
            typedef double                        Value;
            typedef bdlc::FlatHashMap<Key, Value> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&oa);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert({ { SA, 3.0 } });

            ASSERT(    1 == X.size());
            ASSERT( true == X.contains(SA));
            ASSERT(false == X.contains(SB));
            ASSERT(false == X.contains(SC));

            mX.insert({ { SB, 5.0 }, { SC, 7.0 } });

            ASSERT(   3 == X.size());
            ASSERT(true == X.contains(SA));
            ASSERT(true == X.contains(SB));
            ASSERT(true == X.contains(SC));
        }
#endif
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // INITIALIZER LIST ASSIGNMENT OPERATOR
        //   The initializer list assignment operator operates as expected.
        //
        // Concerns:
        //: 1 The initializer list assignment operator correctly forwards to
        //:   the underlying implementation, and produces the expected value.
        //:
        //: 2 The resultant hash and equality functors are as expected.
        //
        // Plan:
        //: 1 Create various objects, including stateful hash and equality
        //:   functors, and directly verify the results of the initializer
        //:   list assignment operator.  (C-1)
        //
        // Testing:
        //   FlatHashMap& operator=(bsl::initializer_list<v_t> values);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INITIALIZER LIST ASSIGNMENT OPERATOR" << endl
                          << "====================================" << endl;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

        const bsl::pair<const int, int> V1(1, 1);
        const bsl::pair<const int, int> V2(2, 2);
        const bsl::pair<const int, int> V3(3, 3);

        const bsl::string SA("abc",    &ta);
        const bsl::string SB("abcdef", &ta);
        const bsl::string SC("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU",
                             &ta);

        const bsl::pair<const bsl::string, double> VA(SA, 3.0, &ta);
        const bsl::pair<const bsl::string, double> VB(SB, 5.0, &ta);
        const bsl::pair<const bsl::string, double> VC(SC, 7.0, &ta);

        {
            typedef int                                        Key;
            typedef int                                        Value;
            typedef SeedIsHash<Key>                            Hash;
            typedef EqualAndState<Key>                         Equal;
            typedef bdlc::FlatHashMap<Key, Value, Hash, Equal> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Hash  hasher(5);
            Equal equal(11);

            Obj mX(0, hasher, equal, &oa);  const Obj& X = mX;

            mX = { V1, V2, V3 };

            ASSERT(   3 == X.size());
            ASSERT(true == X.contains(1));
            ASSERT(true == X.contains(2));
            ASSERT(true == X.contains(3));
            ASSERT(   5 == X.hash_function()(2));
            ASSERT(  11 == X.key_eq().state());

            mX = { V1, V2 };

            ASSERT(    2 == X.size());
            ASSERT( true == X.contains(1));
            ASSERT( true == X.contains(2));
            ASSERT(false == X.contains(3));
            ASSERT(    5 == X.hash_function()(3));
            ASSERT(   11 == X.key_eq().state());

            mX = { V2 };

            ASSERT(    1 == X.size());
            ASSERT(false == X.contains(1));
            ASSERT( true == X.contains(2));
            ASSERT(false == X.contains(3));
            ASSERT(    5 == X.hash_function()(5));
            ASSERT(   11 == X.key_eq().state());
        }
        {
            typedef bsl::string                                Key;
            typedef double                                     Value;
            typedef SeedIsHash<Key>                            Hash;
            typedef EqualAndState<Key>                         Equal;
            typedef bdlc::FlatHashMap<Key, Value, Hash, Equal> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&oa);

            Hash  hasher(7);
            Equal equal(SB);

            Obj mX(0, hasher, equal, &oa);  const Obj& X = mX;

            mX = { VA };

            ASSERT(    1 == X.size());
            ASSERT( true == X.contains(SA));
            ASSERT(false == X.contains(SB));
            ASSERT(false == X.contains(SC));
            ASSERT(    7 == X.hash_function()(SA));
            ASSERT(   SB == X.key_eq().state());

            mX = { VA, VB };

            ASSERT(    2 == X.size());
            ASSERT( true == X.contains(SA));
            ASSERT( true == X.contains(SB));
            ASSERT(false == X.contains(SC));
            ASSERT(    7 == X.hash_function()(SB));
            ASSERT(   SB == X.key_eq().state());

            mX = { VA, VB, VC };

            ASSERT(   3 == X.size());
            ASSERT(true == X.contains(SA));
            ASSERT(true == X.contains(SB));
            ASSERT(true == X.contains(SC));
            ASSERT(   7 == X.hash_function()(SC));
            ASSERT(  SB == X.key_eq().state());
        }
#endif
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // INITIALIZER LIST VALUE CONSTRUCTORS
        //   The initializer list value constructors operate as expected.
        //
        // Concerns:
        //: 1 The constructors create the correct initial value and have the
        //:   hashing, equality, and internal memory management systems hooked
        //:   up properly.
        //
        // Plan:
        //: 1 Create various objects using the constructors and directly verify
        //:   the arguments were stored correctly using the accessors.  (C-1)
        //
        // Testing:
        //   FlatHashMap(bsl::initializer_list<v_t> values, Allocator * = 0);
        //   FlatHashMap(bsl::initializer_list<v_t>, size_t, Allocator * = 0);
        //   FlatHashMap(init_list<v_t>, size_t, const HASH&, Allocator * = 0);
        //   FlatHashMap(init_list<v_t>, size_t, const H&, const EQ&, A * = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INITIALIZER LIST VALUE CONSTRUCTORS" << endl
                          << "===================================" << endl;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

        const bsl::string SA("abc",    &ta);
        const bsl::string SB("abcdef", &ta);
        const bsl::string SC("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU",
                             &ta);

        bslma::DefaultAllocatorGuard dag(&ta);

        bsl::initializer_list<bsl::pair<const int, int> >            IDATA =
                                                                   { { 1, 1} };
        bsl::initializer_list<bsl::pair<const bsl::string, double> > SDATA =
                                                               { { SA, 3.0 } };

        if (verbose) cout << "Testing constructors." << endl;
        {
            // FlatHashMap(init);

            {
                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                Obj mX(IDATA);  const Obj& X = mX;

                ASSERT(           1 == X.size());
                ASSERT(        true == X.contains(1));
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());
            }
            {
                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                typedef bsl::string                               Key;
                typedef double                                    Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                Obj mX(SDATA);  const Obj& X = mX;

                ASSERT(            1 == X.size());
                ASSERT(         true == X.contains(SA));
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &da == X.allocator());
            }

            // FlatHashMap(init, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(IDATA, (bslma::Allocator *)0);  const Obj& X = mX;

                ASSERT(           1 == X.size());
                ASSERT(        true == X.contains(1));
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());
            }
            {
                typedef bsl::string                               Key;
                typedef double                                    Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(SDATA, &oa);  const Obj& X = mX;

                ASSERT(            1 == X.size());
                ASSERT(         true == X.contains(SA));
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &oa == X.allocator());
            }

            // FlatHashMap(init, size_t capacity);

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(IDATA, 32);  const Obj& X = mX;

                ASSERT(          32 == X.capacity());
                ASSERT(           1 == X.size());
                ASSERT(        true == X.contains(1));
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());
            }
            {
                typedef bsl::string                               Key;
                typedef double                                    Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(SDATA, 64);  const Obj& X = mX;

                ASSERT(           64 == X.capacity());
                ASSERT(            1 == X.size());
                ASSERT(         true == X.contains(SA));
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &da == X.allocator());
            }

            // FlatHashMap(init, size_t capacity, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(IDATA, 32, (bslma::Allocator *)0);  const Obj& X = mX;

                ASSERT(          32 == X.capacity());
                ASSERT(           1 == X.size());
                ASSERT(        true == X.contains(1));
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());
            }
            {
                typedef bsl::string                               Key;
                typedef double                                    Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(SDATA, 64, &oa);  const Obj& X = mX;

                ASSERT(           64 == X.capacity());
                ASSERT(            1 == X.size());
                ASSERT(         true == X.contains(SA));
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &oa == X.allocator());
            }

            // FlatHashMap(init, size_t, const HASH&, Allocator *bA = 0);

            {
                typedef int                                 Key;
                typedef int                                 Value;
                typedef SeedIsHash<Key>                     Hash;
                typedef bdlc::FlatHashMap<Key, Value, Hash> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Hash hasher(7);

                Obj mX(IDATA, 32, hasher);  const Obj& X = mX;

                ASSERT(   32 == X.capacity());
                ASSERT(    1 == X.size());
                ASSERT( true == X.contains(1));
                ASSERT(    7 == X.hash_function()(0));
                ASSERT(    7 == X.hash_function()(1));
                ASSERT(    7 == X.hash_function()(7));
                ASSERT( true == X.key_eq()(0, 0));
                ASSERT(false == X.key_eq()(0, 1));
                ASSERT(  &da == X.allocator());
            }
            {
                typedef bsl::string                         Key;
                typedef double                              Value;
                typedef SeedIsHash<Key>                     Hash;
                typedef bdlc::FlatHashMap<Key, Value, Hash> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Hash hasher(9);

                Obj mX(SDATA, 64, hasher, &oa);  const Obj& X = mX;

                ASSERT(   64 == X.capacity());
                ASSERT(    1 == X.size());
                ASSERT( true == X.contains(SA));
                ASSERT(    9 == X.hash_function()(SA));
                ASSERT(    9 == X.hash_function()(SB));
                ASSERT(    9 == X.hash_function()(SC));
                ASSERT( true == X.key_eq()(SA, SA));
                ASSERT(false == X.key_eq()(SA, SB));
                ASSERT(  &oa == X.allocator());
            }

            // FlatHashMap(init, size_t, const HASH&, const EQUAL&, A * = 0);

            {
                typedef int                                        Key;
                typedef int                                        Value;
                typedef SeedIsHash<Key>                            Hash;
                typedef bsl::equal_to<Key>                         Equal;
                typedef bdlc::FlatHashMap<Key, Value, Hash, Equal> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Hash hasher(7);

                Obj mX(IDATA, 32, hasher, Equal());  const Obj& X = mX;

                ASSERT(   32 == X.capacity());
                ASSERT(    1 == X.size());
                ASSERT( true == X.contains(1));
                ASSERT(    7 == X.hash_function()(0));
                ASSERT(    7 == X.hash_function()(1));
                ASSERT(    7 == X.hash_function()(7));
                ASSERT( true == X.key_eq()(0, 0));
                ASSERT(false == X.key_eq()(0, 1));
                ASSERT(  &da == X.allocator());
            }
            {
                typedef bsl::string                                Key;
                typedef double                                     Value;
                typedef SeedIsHash<Key>                            Hash;
                typedef bsl::less<Key>                             Equal;
                typedef bdlc::FlatHashMap<Key, Value, Hash, Equal> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Hash hasher(9);

                Obj mX(SDATA, 64, hasher, Equal(), &oa);  const Obj& X = mX;

                ASSERT(   64 == X.capacity());
                ASSERT(    1 == X.size());
                // Due to comparison operator, cannot test with 'contains'.
                ASSERT(   SA == X.begin()->first);
                ASSERT(    9 == X.hash_function()(SA));
                ASSERT(    9 == X.hash_function()(SB));
                ASSERT(    9 == X.hash_function()(SC));
                ASSERT(false == X.key_eq()(SA, SA));
                ASSERT( true == X.key_eq()(SA, SB));
                ASSERT(  &oa == X.allocator());

            }
        }
#endif
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // ITERATOR VALUE CONSTRUCTORS
        //   The iterator value constructors operate as expected.
        //
        // Concerns:
        //: 1 The constructors create the correct initial value and have the
        //:   hashing, equality, and internal memory management systems hooked
        //:   up properly.
        //
        // Plan:
        //: 1 Create various objects using the constructors and directly verify
        //:   the arguments were stored correctly using the accessors.  (C-1)
        //
        // Testing:
        //   FlatHashMap(INPUT_ITERATOR, INPUT_ITERATOR, Allocator *bA = 0);
        //   FlatHashMap(INPUT_ITER, INPUT_ITER, size_t, Allocator * = 0);
        //   FlatHashMap(II, II, size_t, const HASH&, Allocator * = 0);
        //   FlatHashMap(II, II, size_t, const H&, const EQ&, Alloc * = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ITERATOR VALUE CONSTRUCTORS" << endl
                          << "===========================" << endl;

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

        const bsl::string SA("abc",    &ta);
        const bsl::string SB("abcdef", &ta);
        const bsl::string SC("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU",
                             &ta);

        bsl::map<int, int> IDATA(&ta);
        {
            IDATA.insert(bsl::make_pair(1, 1));
            IDATA.insert(bsl::make_pair(2, 2));
        }

        bsl::map<bsl::string, double> SDATA(&ta);
        {
            SDATA.insert(bsl::make_pair(SA, 3.0));
            SDATA.insert(bsl::make_pair(SB, 4.0));
        }

        if (verbose) cout << "Testing constructors." << endl;
        {
            // FlatHashMap(II, II);

            {
                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                Obj        mX(IDATA.begin(), ++IDATA.begin());
                const Obj& X = mX;

                ASSERT(           1 == X.size());
                ASSERT(        true == X.contains(1));
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());
            }
            {
                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                typedef bsl::string                               Key;
                typedef double                                    Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                Obj        mX(SDATA.begin(), ++SDATA.begin());
                const Obj& X = mX;

                ASSERT(            1 == X.size());
                ASSERT(         true == X.contains(SA));
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &da == X.allocator());
            }

            // FlatHashMap(II, II, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mX(IDATA.begin(),
                              ++IDATA.begin(),
                              (bslma::Allocator *)0);
                const Obj& X = mX;

                ASSERT(           1 == X.size());
                ASSERT(        true == X.contains(1));
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());
            }
            {
                typedef bsl::string                               Key;
                typedef double                                    Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj        mX(SDATA.begin(), ++SDATA.begin(), &oa);
                const Obj& X = mX;

                ASSERT(            1 == X.size());
                ASSERT(         true == X.contains(SA));
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &oa == X.allocator());
            }

            // FlatHashMap(II, II, size_t capacity);

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mX(IDATA.begin(), ++IDATA.begin(), 32);
                const Obj& X = mX;

                ASSERT(          32 == X.capacity());
                ASSERT(           1 == X.size());
                ASSERT(        true == X.contains(1));
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());
            }
            {
                typedef bsl::string                               Key;
                typedef double                                    Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mX(SDATA.begin(), ++SDATA.begin(), 64);
                const Obj& X = mX;

                ASSERT(           64 == X.capacity());
                ASSERT(            1 == X.size());
                ASSERT(         true == X.contains(SA));
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &da == X.allocator());
            }

            // FlatHashMap(II, II, size_t capacity, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mX(IDATA.begin(),
                              ++IDATA.begin(),
                              32,
                              (bslma::Allocator *)0);
                const Obj& X = mX;

                ASSERT(          32 == X.capacity());
                ASSERT(           1 == X.size());
                ASSERT(        true == X.contains(1));
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());
            }
            {
                typedef bsl::string                               Key;
                typedef double                                    Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj        mX(SDATA.begin(), ++SDATA.begin(), 64, &oa);
                const Obj& X = mX;

                ASSERT(           64 == X.capacity());
                ASSERT(            1 == X.size());
                ASSERT(         true == X.contains(SA));
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &oa == X.allocator());
            }

            // FlatHashMap(II, II, size_t, const HASH&, Allocator *bA = 0);

            {
                typedef int                                 Key;
                typedef int                                 Value;
                typedef SeedIsHash<Key>                     Hash;
                typedef bdlc::FlatHashMap<Key, Value, Hash> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Hash hasher(7);

                Obj        mX(IDATA.begin(), ++IDATA.begin(), 32, hasher);
                const Obj& X = mX;

                ASSERT(   32 == X.capacity());
                ASSERT(    1 == X.size());
                ASSERT( true == X.contains(1));
                ASSERT(    7 == X.hash_function()(0));
                ASSERT(    7 == X.hash_function()(1));
                ASSERT(    7 == X.hash_function()(7));
                ASSERT( true == X.key_eq()(0, 0));
                ASSERT(false == X.key_eq()(0, 1));
                ASSERT(  &da == X.allocator());
            }
            {
                typedef bsl::string                         Key;
                typedef double                              Value;
                typedef SeedIsHash<Key>                     Hash;
                typedef bdlc::FlatHashMap<Key, Value, Hash> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Hash hasher(9);

                Obj        mX(SDATA.begin(),
                              ++SDATA.begin(),
                              64,
                              hasher,
                              &oa);
                const Obj& X = mX;

                ASSERT(   64 == X.capacity());
                ASSERT(    1 == X.size());
                ASSERT( true == X.contains(SA));
                ASSERT(    9 == X.hash_function()(SA));
                ASSERT(    9 == X.hash_function()(SB));
                ASSERT(    9 == X.hash_function()(SC));
                ASSERT( true == X.key_eq()(SA, SA));
                ASSERT(false == X.key_eq()(SA, SB));
                ASSERT(  &oa == X.allocator());
            }

            // FlatHashMap(II, II, size_t, const HASH&, const EQUAL&, A * = 0);

            {
                typedef int                                        Key;
                typedef int                                        Value;
                typedef SeedIsHash<Key>                            Hash;
                typedef bsl::equal_to<Key>                         Equal;
                typedef bdlc::FlatHashMap<Key, Value, Hash, Equal> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Hash hasher(7);

                Obj        mX(IDATA.begin(),
                              ++IDATA.begin(),
                              32,
                              hasher,
                              Equal());
                const Obj& X = mX;

                ASSERT(   32 == X.capacity());
                ASSERT(    1 == X.size());
                ASSERT( true == X.contains(1));
                ASSERT(    7 == X.hash_function()(0));
                ASSERT(    7 == X.hash_function()(1));
                ASSERT(    7 == X.hash_function()(7));
                ASSERT( true == X.key_eq()(0, 0));
                ASSERT(false == X.key_eq()(0, 1));
                ASSERT(  &da == X.allocator());
            }
            {
                typedef bsl::string                                Key;
                typedef double                                     Value;
                typedef SeedIsHash<Key>                            Hash;
                typedef bsl::less<Key>                             Equal;
                typedef bdlc::FlatHashMap<Key, Value, Hash, Equal> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Hash hasher(9);

                Obj        mX(SDATA.begin(),
                              ++SDATA.begin(),
                              64,
                              hasher,
                              Equal(),
                              &oa);
                const Obj& X = mX;

                ASSERT(   64 == X.capacity());
                ASSERT(    1 == X.size());
                // Due to comparison operator, cannot test with 'contains'.
                ASSERT(   SA == X.begin()->first);
                ASSERT(    9 == X.hash_function()(SA));
                ASSERT(    9 == X.hash_function()(SB));
                ASSERT(    9 == X.hash_function()(SC));
                ASSERT(false == X.key_eq()(SA, SA));
                ASSERT( true == X.key_eq()(SA, SB));
                ASSERT(  &oa == X.allocator());

            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // 'rehash' AND 'reserve'
        //   Ensure the 'rehash' and 'reserve' methods operate as expected.
        //
        // Concerns:
        //: 1 The 'rehash' and 'reserve'  methods correctly forward to the
        //:   underlying implementation.
        //
        // Plan:
        //: 1 Construct and object, use 'rehash' and 'reserve' to alter the
        //:   object's capacity, and verify the resultant capacity with the
        //:   'capacity' accessor.  (C-1)
        //
        // Testing:
        //   void rehash(size_t);
        //   void reserve(size_t);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'rehash' AND 'reserve'" << endl
                          << "======================" << endl;

        if (verbose) cout << "Testing 'rehash'." << endl;
        {
            typedef bdlc::FlatHashMap<int, int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.rehash(32);

            ASSERT(32 == X.capacity());

            mX.rehash(64);

            ASSERT(64 == X.capacity());
        }

        if (verbose) cout << "Testing 'reserve'." << endl;
        {
            typedef bdlc::FlatHashMap<int, int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.reserve(28);

            ASSERT(32 == X.capacity());

            mX.reserve(56);

            ASSERT(64 == X.capacity());
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // RANGE 'erase'
        //   Ensure the range 'erase' method operates as expected.
        //
        // Concerns:
        //: 1 The range 'erase' method correctly forwards to the underlying
        //:   implementation and correctly forwards the return value.
        //
        // Plan:
        //: 1 For an object 'X' constructed with one contained value, verify
        //:   the results of range 'erase' for 'X.erase(X.begin(), X.begin())',
        //:   'X.erase(X.end(), X.end())', and 'X.erase(X.begin(), X.end())'.
        //:   (C-1)
        //
        // Testing:
        //   iterator erase(const_iterator, const_iterator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RANGE 'erase'" << endl
                          << "=============" << endl;

        if (verbose) cout << "Testing range 'erase'." << endl;
        {
            typedef bdlc::FlatHashMap<int, int, SeedIsHash<int> > Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(bsl::make_pair(1, 1));

            Obj::iterator rv = mX.erase(mX.begin(), mX.begin());

            ASSERT(X.begin() == rv);
            ASSERT(1 == X.begin()->first);
            ASSERT(1 == X.begin()->second);

            rv->second = 2;

            ASSERT(1 == X.begin()->first);
            ASSERT(2 == X.begin()->second);

            ASSERT(  X.end() == mX.erase(mX.end(), mX.end()));

            ASSERT(1 == X.size());

            ASSERT(X.end() == mX.erase(mX.begin(), mX.end()));

            ASSERT(0 == X.size());
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // ITERATOR 'erase'
        //   Ensure the iterator 'erase' method operates as expected.
        //
        // Concerns:
        //: 1 The iterator 'erase' method correctly forwards to the underlying
        //:   implementation and correctly forwards the return value.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct an object with two contained values and use the
        //:   iterator 'erase' method to empty the object, verifying the
        //:   object's value and the return value after each 'erase'.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   iterator erase(const_iterator);
        //   iterator erase(iterator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ITERATOR 'erase'" << endl
                          << "================" << endl;

        if (verbose) cout << "Testing iterator 'erase(const_iterator)'."
                          << endl;
        {
            typedef bdlc::FlatHashMap<int, int, SeedIsHash<int> > Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(2, 2));

            Obj::iterator rv = mX.erase(X.find(1));

            ASSERT(2 == rv->first);
            ASSERT(2 == rv->second);
            ASSERT(1 == X.size());

            rv->second = 3;

            ASSERT(2 == X.begin()->first);
            ASSERT(3 == X.begin()->second);

            ASSERT(X.end() == mX.erase(X.find(2)));
            ASSERT(      0 == X.size());
        }

        if (verbose) cout << "Testing iterator 'erase(iterator)'." << endl;
        {
            typedef bdlc::FlatHashMap<int, int, SeedIsHash<int> > Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(2, 2));

            Obj::iterator rv = mX.erase(mX.begin());

            ASSERT(2 == rv->first);
            ASSERT(2 == rv->second);
            ASSERT(1 == X.size());

            rv->second = 3;

            ASSERT(2 == X.begin()->first);
            ASSERT(3 == X.begin()->second);

            ASSERT(X.end() == mX.erase(mX.begin()));
            ASSERT(      0 == X.size());
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            typedef bdlc::FlatHashMap<int, int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(bsl::make_pair(1, 1));

            ASSERT_SAFE_PASS(mX.erase(X.begin()));
            ASSERT_SAFE_FAIL(mX.erase(X.end()));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // RANGE 'insert'
        //   Ensure the range 'insert' method correctly forwards to the
        //   implementation component.
        //
        // Concerns:
        //: 1 The range 'insert' method correctly forwards to the underlying
        //:   implementation component, resulting in the specified elements
        //:   being inserted.
        //
        // Plan:
        //: 1 For an object Z with one inserted element, verify the results of
        //:   range 'insert' into an empty object 'X' for
        //:   'X.insert(Z.begin(), Z.begin())', 'X.insert(Z.end(), Z.end())',
        //:   and 'X.insert(X.begin(), X.end())'.  (C-1)
        //
        // Testing:
        //   void insert(INPUT_ITERATOR, INPUT_ITERATOR);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RANGE 'insert'" << endl
                          << "==============" << endl;

        if (verbose) cout << "Testing range 'insert'." << endl;
        {
            typedef bdlc::FlatHashMap<int, int, SeedIsHash<int> > Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mZ(&oa);  const Obj& Z = mZ;

            mZ.insert(bsl::make_pair(1, 1));

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(Z.begin(), Z.begin());
            ASSERT(X.empty());

            mX.insert(Z.end(), Z.end());
            ASSERT(X.empty());

            mX.insert(Z.begin(), Z.end());
            ASSERT(X == Z);
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // MOVE ASSIGNMENT OPERATOR
        //   Ensure the move assignment operator correctly forwards to the
        //   implementation component.
        //
        // Concerns:
        //: 1 The signature and return type are standard.
        //:
        //: 2 The move assignment operator correctly forwards to the
        //:   underlying implementation component, resulting in the value of
        //:   any target object being map to that of any source object.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 There is no memory allocations.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a function pointer
        //:   having the appropriate structure for the move assignment operator
        //:   defined in this component.  (C-1)
        //:
        //: 2 Verify the results of the move assignment operator and its return
        //:   value on two distinct objects of various types.  (C-2,3)
        //:
        //: 3 Use a test allocator and verify that no memory is allocated
        //:   during the move assignment operator invocation.  (C-4)
        //
        // Testing:
        //   FlatHashMap& operator=(FlatHashMap&&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout << "Testing move assignment operator." << endl;
        {
            testCase15MoveAssignment<bsltf::AllocBitwiseMoveableTestType>(
                                                                         1,
                                                                         true);
            testCase15MoveAssignment<bsltf::AllocTestType>(2, true);
            testCase15MoveAssignment<bsltf::BitwiseCopyableTestType>(3, false);
            testCase15MoveAssignment<bsltf::BitwiseMoveableTestType>(4, false);
            testCase15MoveAssignment<bsltf::MovableAllocTestType>(5, true);
            testCase15MoveAssignment<bsltf::MovableTestType>(6, false);
            testCase15MoveAssignment<bsltf::NonDefaultConstructibleTestType>(
                                                                        7,
                                                                        false);
            testCase15MoveAssignment<bsltf::NonOptionalAllocTestType>(8, true);
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR WITH ALLOCATOR
        //   Ensure the move constructor with allocator correctly forwards to
        //   the implementation component.
        //
        // Concerns:
        //: 1 The constructor correctly forwards to the underlying
        //:   implementation component, resulting in the newly created object
        //:   having the same value as that of the original object before the
        //:   call.
        //:
        //: 2 The allocator is propagated to the newly created object if (and
        //:   only if) no allocator is specified in the call to the move
        //:   constructor.
        //:
        //: 3 The original object is always left in a valid state and the
        //:   allocator address held by the original object is unchanged.
        //:
        //: 4 Subsequent changes to or destruction of the original object have
        //:   no effect on the move-constructed object and vice-versa.
        //:
        //: 5 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 6 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Specify a map 'S' of two distinct object values to be used
        //:   sequentially in the following tests; for each entry, create a
        //:   control object.
        //:
        //: 2 Call the move constructor to create the container in all
        //:   relevant use cases involving the allocator: 1) no allocator
        //:   passed in, 2) a '0' is explicitly passed in as the allocator
        //:   argument, 3) the same allocator as that of the original object
        //:   is explicitly passed in, and 4) a different allocator than that
        //:   of the original object is passed in.
        //:
        //: 3 For each of the object values (P-1) and for each configuration
        //:   (P-2), verify the following:
        //:
        //:   1 Verify the newly created object has the same value as that of
        //:     the original object before the call to the move constructor
        //:     (control value).  (C-1)
        //:
        //:   3 Ensure that the new original, and control object continue to
        //:     have the correct allocator and that all memory allocations come
        //:     from the appropriate allocator.  (C-2,5)
        //:
        //:   7 Verify the original object (after the move construction) to
        //:     is in a valid state, destroy it, and then verify newly created
        //:     is in a valid state.  (C-3,4)
        //:
        //:   8 Verify all memory is released when the object is destroyed.
        //:     (C-6)
        //
        // Testing:
        //   FlatHashMap(FlatHashMap&&, Allocator *basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE CONSTRUCTOR WITH ALLOCATOR" << endl
                          << "===============================" << endl;

        if (verbose) cout << "Testing move constructor with allocator."
                          << endl;
        {
            testCase14MoveConstructorWithAllocator<
                                       bsltf::AllocBitwiseMoveableTestType>(1);
            testCase14MoveConstructorWithAllocator<bsltf::AllocTestType>(2);
            testCase14MoveConstructorWithAllocator<
                                            bsltf::BitwiseCopyableTestType>(3);
            testCase14MoveConstructorWithAllocator<
                                            bsltf::BitwiseMoveableTestType>(4);
            testCase14MoveConstructorWithAllocator<
                                               bsltf::MovableAllocTestType>(5);
            testCase14MoveConstructorWithAllocator<
                                                    bsltf::MovableTestType>(6);
            testCase14MoveConstructorWithAllocator<
                                    bsltf::NonDefaultConstructibleTestType>(7);
            testCase14MoveConstructorWithAllocator<
                                           bsltf::NonOptionalAllocTestType>(8);
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR WITHOUT ALLOCATOR
        //   Ensure the move constructor without allocator correctly forwards
        //   to the implementation component.
        //
        // Concerns:
        //: 1 The constructor correctly forwards to the underlying
        //:   implementation component, resulting in the newly created object
        //:   having the same value as that of the original object before the
        //:   call.
        //:
        //: 2 The original object is left in a valid state.
        //:
        //: 3 Subsequent changes to or destruction of the original object have
        //:   no effect on the move-constructed object and vice-versa.
        //:
        //: 4 The constructor does not allocate memory.
        //
        // Plan:
        //: 1 Specify a map 'S' of two distinct object values to be used
        //:   sequentially in the following tests; for each entry, create a
        //:   control object.
        //:
        //: 2 For each of the object values (P-1), verify the following:
        //:
        //:   1 Verify the newly created object has the same value as that of
        //:     the original object before the call to the move constructor
        //:     (control value).
        //:
        //:   2 After the move construction, verify the newly created object
        //:     is equal to the control object, and ensure the original object
        //:     is in a valid state.  (C-1,2)
        //:
        //:   3 Destroy the original object, and then ensure the newly created
        //:     object is in a valid state.  (C-3)
        //:
        //: 3 The allocators used to create the objects will be verified to
        //:   ensure that no memory was allocated during use of the constructor
        //:   (note that this check may be done at the global level).  (C-4)
        //
        // Testing:
        //   FlatHashMap(FlatHashMap&&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE CONSTRUCTOR WITHOUT ALLOCATOR" << endl
                          << "==================================" << endl;

        if (verbose) cout << "Testing move constructor without allocator."
                          << endl;
        {
            testCase13MoveConstructorWithoutAllocator<
                                       bsltf::AllocBitwiseMoveableTestType>(1);
            testCase13MoveConstructorWithoutAllocator<bsltf::AllocTestType>(2);
            testCase13MoveConstructorWithoutAllocator<
                                            bsltf::BitwiseCopyableTestType>(3);
            testCase13MoveConstructorWithoutAllocator<
                                            bsltf::BitwiseMoveableTestType>(4);
            testCase13MoveConstructorWithoutAllocator<
                                               bsltf::MovableAllocTestType>(5);
            testCase13MoveConstructorWithoutAllocator<
                                                    bsltf::MovableTestType>(6);
            testCase13MoveConstructorWithoutAllocator<
                                    bsltf::NonDefaultConstructibleTestType>(7);
            testCase13MoveConstructorWithoutAllocator<
                                           bsltf::NonOptionalAllocTestType>(8);
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // SEARCH METHODS AND ITERATORS
        //   Ensure the search methods and iterators operate as expected.
        //
        // Concerns:
        //: 1 The 'begin' iterator methods correctly determine the first
        //:   contained value.
        //:
        //: 2 Incrementing the 'begin' iterator 'size()' times visits all
        //:   contained elements and results in the 'end' iterator.
        //:
        //: 3 The return value of 'begin' invoked on a modifiable object is
        //:   a modifiable iterator.
        //:
        //: 4 The accessors 'contains', 'count', and 'equal_range' correctly
        //:   forward to the implementation object and correctly forward the
        //:   return value.
        //
        // Plan:
        //: 1 Using the basic accessors, verify the 'begin' methods return an
        //:   iterator referencing the first populated entry and incrementing
        //:   the 'begin' iterator 'size()' times visits all contained elements
        //:   and results in the 'end' iterator.  (C-1,2)
        //:
        //: 2 Modify the referenced value of any returned modifiable iterator.
        //:   (C-3)
        //:
        //: 3 Directly verify the result of 'contains', 'count', and
        //:   'equal_range' when applied to various object values.  (C-4)
        //
        // Testing:
        //   iterator begin();
        //   iterator end();
        //   bsl::pair<iterator, iterator> equal_range(const KEY& key);
        //   bool contains(const KEY&) const;
        //   bsl::size_t count(const KEY& key) const;
        //   bsl::pair<ci, ci> equal_range(const KEY&) const;
        //   const_iterator begin() const;
        //   const_iterator cbegin() const;
        //   const_iterator cend() const;
        //   const_iterator end() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SEARCH METHODS AND ITERATORS" << endl
                          << "============================" << endl;

        typedef bdlc::FlatHashMap<int, int, SeedIsHash<int> > Obj;

        if (verbose) cout << "Testing 'iterator' methods." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);

            ASSERT(mX.end() == mX.begin());

            mX.insert(bsl::make_pair(1, 1));

            Obj::iterator iter = mX.begin();

            ASSERT(1 == iter->first);
            ASSERT(1 == iter->second);

            iter->second = 7;

            iter = mX.begin();

            ASSERT(1 == iter->first);
            ASSERT(7 == iter->second);

            ++iter;

            ASSERT(mX.end() == iter);

            mX.insert(bsl::make_pair(2, 2));

            iter = mX.begin();

            ASSERT(1 == iter->first);

            ++iter;

            ASSERT(2 == iter->first);

            ++iter;

            ASSERT(mX.end() == iter);
        }

        if (verbose) cout << "Testing 'const_iterator' methods." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            ASSERT(X.end() == X.begin());
            ASSERT(X.end() == X.cbegin());
            ASSERT(X.end() == X.cend());

            mX.insert(bsl::make_pair(1, 1));

            Obj::const_iterator iter1 = X.begin();
            Obj::const_iterator iter2 = X.cbegin();

            ASSERT(1 == iter1->first);
            ASSERT(1 == iter2->first);

            ++iter1;
            ++iter2;

            ASSERT(X.end() == iter1);
            ASSERT(X.end() == iter2);

            mX.insert(bsl::make_pair(2, 2));

            iter1 = X.begin();
            iter2 = X.cbegin();

            ASSERT(1 == iter1->first);
            ASSERT(1 == iter2->first);

            ++iter1;
            ++iter2;

            ASSERT(2 == iter1->first);
            ASSERT(2 == iter2->first);

            ++iter1;
            ++iter2;

            ASSERT(X.end() == iter1);
            ASSERT(X.end() == iter2);
        }

        if (verbose) {
            cout << "Testing 'contains', 'count', and 'equal_range'." << endl;
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(3, 3));

            ASSERT( true == X.contains(1));
            ASSERT(false == X.contains(2));
            ASSERT( true == X.contains(3));

            ASSERT(1 == X.count(1));
            ASSERT(0 == X.count(2));
            ASSERT(1 == X.count(3));

            bsl::pair<Obj::const_iterator, Obj::const_iterator> rv;
            Obj::const_iterator                                 exp;

            rv = X.equal_range(1);

            exp = rv.first;
            ++exp;

            ASSERT(  1 == (rv.first)->first);
            ASSERT(exp == rv.second);

            rv = X.equal_range(2);

            ASSERT(X.end() == rv.first);
            ASSERT(X.end() == rv.second);

            rv = X.equal_range(3);

            exp = rv.first;
            ++exp;

            ASSERT(  3 == (rv.first)->first);
            ASSERT(exp == rv.second);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);

            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(3, 3));

            bsl::pair<Obj::iterator, Obj::iterator> rv;
            Obj::iterator                           exp;

            rv = mX.equal_range(1);

            exp = rv.first;
            ++exp;

            ASSERT(  1 == (rv.first)->first);
            ASSERT(  1 == (rv.first)->second);
            ASSERT(  3 == (rv.second)->first);
            ASSERT(  3 == (rv.second)->second);
            ASSERT(exp == rv.second);

            rv.first->second = 2;
            rv.second->second = 4;

            rv = mX.equal_range(1);

            ASSERT(  1 == (rv.first)->first);
            ASSERT(  2 == (rv.first)->second);
            ASSERT(  3 == (rv.second)->first);
            ASSERT(  4 == (rv.second)->second);

            rv = mX.equal_range(2);

            ASSERT(mX.end() == rv.first);
            ASSERT(mX.end() == rv.second);

            rv = mX.equal_range(3);

            exp = rv.first;
            ++exp;

            ASSERT(  3 == (rv.first)->first);
            ASSERT(exp == rv.second);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'empty' AND 'load_factor'
        //   Ensure the non-basic accessors and iterators operate as expected.
        //
        // Concerns:
        //: 1 The accessors 'empty' and 'load_factor' are consistent with the
        //:   results of 'size' and 'capacity'.
        //:
        //: 2 The methods do not allocate memory.
        //
        // Plan:
        //: 1 Create an object and insert values into the object.  Verify the
        //:   results of 'empty' and 'load_factor' to the value expected given
        //:   'size' and 'capacity' for the object values.  (C-1)
        //:
        //: 2 The allocators used to create the objects will be verified to
        //:   ensure that no memory was allocated during use of the accessors
        //:   and iterators.  (C-2)
        //
        // Testing:
        //   bool empty() const;
        //   float load_factor() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'empty' AND 'load_factor'" << endl
                          << "=========================" << endl;

        typedef bdlc::FlatHashMap<int, int> Obj;

        if (verbose) cout << "Testing 'empty' and 'load_factor'." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            bsls::Types::Int64 expNumAllocations = oa.numAllocations();

            ASSERT(X.empty());

            ASSERT(0.0f == X.load_factor());

            ASSERT(expNumAllocations == oa.numAllocations());

            for (int i = 0; i < 100; ++i) {
                mX.insert(bsl::make_pair(i, i));

                expNumAllocations = oa.numAllocations();

                LOOP_ASSERT(i, false == X.empty());

                float loadFactor = static_cast<float>(X.size())
                                            / static_cast<float>(X.capacity());

                // Note that since 'capacity' is a power of two, a simple check
                // suffices.

                LOOP_ASSERT(i, loadFactor == X.load_factor());

                LOOP_ASSERT(i, expNumAllocations == oa.numAllocations());
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //
        // The functionality was omitted.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDEX STREAMING" << endl
                          << "==============" << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The signature and return type are standard.
        //:
        //: 2 The assignment operator correctly forwards to the implementation
        //:   component and returns the correct value.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a function pointer
        //:   having the appropriate structure for the copy assignment operator
        //:   defined in this component.  (C-1)
        //:
        //: 2 Construct distinct objects X, Y, and Z.  Assign X the value of Y
        //:   and verify the resultant value of X and the return value of the
        //:   assignment.  Assign X the value of X and verify the resultant
        //:   value of X and the return value of the assignment.
        //
        // Testing:
        //   FlatHashMap& operator=(const FlatHashMap&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        typedef bdlc::FlatHashMap<int, int> Obj;

        if (verbose) {
            cout << "Assign the address of the operator to a variable."
                 << endl;
        }
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            // Quash unused variable warning.

            Obj a;
            Obj b;
            (a.*operatorAssignment)(b);
        }

        if (verbose) cout << "Testing copy assignment operator." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            Obj mY(&oa);  const Obj& Y = mY;
            mY.insert(bsl::make_pair(1, 1));

            Obj mZ(&oa);  const Obj& Z = mZ;
            mZ.insert(bsl::make_pair(2, 2));

            {
                Obj& rv = (mX = Y);

                ASSERT(  X == Y);
                ASSERT(&rv == &X);
            }
            {
                Obj& rv = (mX = Z);

                ASSERT(  X == Z);
                ASSERT(&rv == &X);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTION
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects.
        //
        // Concerns:
        //: 1 Both functions have standard signatures and return types.
        //:
        //: 2 Both functions correctly forward to the underlying implementation
        //:   component and exchange the values of the (two) supplied objects.
        //:
        //: 3 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-1)
        //:
        //: 2 Create various objects and verify the result of the 'swap' member
        //:   and free functions.  (C-2)
        //:
        //: 3 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:   values of two distinct objects.  (C-3)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void swap(FlatHashMap&);
        //   void swap(FlatHashMap&, FlatHashMap&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTION" << endl
                          << "=============================" << endl;

        typedef bdlc::FlatHashMap<int, int> Obj;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            using namespace bdlc;

            typedef void (Obj::*funcPtr)(Obj&);

            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            // Quash unused variable warnings.

            Obj a;
            Obj b;
            (a.*memberSwap)(b);
            freeSwap(a, b);
        }

        if (verbose) cout << "Testing 'swap' methods." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mW(&oa);  const Obj& W = mW;
            Obj mZ(&oa);  const Obj& Z = mZ;

            mW.insert(bsl::make_pair(1, 1));
            mZ.insert(bsl::make_pair(2, 2));

            ASSERT(W != Z);

            Obj mX(W, &oa);  const Obj& X = mX;
            Obj mY(Z, &oa);  const Obj& Y = mY;

            mX.swap(mY);

            ASSERT(X == Z);
            ASSERT(Y == W);

            swap(mX, mY);

            ASSERT(X == W);
            ASSERT(Y == Z);
        }
        {
            bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
            bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

            Obj mW(&oa1);  const Obj& W = mW;
            Obj mZ(&oa1);  const Obj& Z = mZ;

            mW.insert(bsl::make_pair(1, 1));
            mZ.insert(bsl::make_pair(2, 2));

            ASSERT(W != Z);

            Obj mX(W, &oa1);  const Obj& X = mX;
            Obj mY(Z, &oa2);  const Obj& Y = mY;

            swap(mX, mY);

            ASSERT(   X == Z);
            ASSERT(   Y == W);
            ASSERT(&oa1 == X.allocator());
            ASSERT(&oa2 == Y.allocator());
        }

        if (verbose) cout << "Invoke free 'swap' function where ADL is used."
                          << endl;
        {
            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            const Obj XX(X, &scratch);

            Obj mY(&oa);  const Obj& Y = mY;

            mY.insert(bsl::make_pair(1, 1));

            const Obj YY(Y, &scratch);

            if (veryVeryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            LOOP2_ASSERT(YY, X, YY == X);
            LOOP2_ASSERT(XX, Y, XX == Y);
            ASSERT(oam.isTotalSame());

            if (veryVeryVerbose) { T_ P_(X) P(Y) }
        }

        if (verbose) cout << "Negative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
            bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

            Obj mA(&oa1);
            Obj mB(&oa1);
            Obj mC(&oa1);
            Obj mZ(&oa2);

            ASSERT_SAFE_PASS(mA.swap(mB));
            ASSERT_SAFE_FAIL(mC.swap(mZ));
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   correctly forwards to the implementation object and creates an
        //:   object having the same value as the supplied original object.
        //
        // Plan:
        //: 1 Create objects W and Z, having distinct value.  Create X as a
        //:   copy of W without supplying an allocator.  Create Y as a copy of
        //:   Z supplying an allocator.  Verify the values of X and Y using
        //:   'operator=='.  Verify the allocators using the 'allocator'
        //:   accessor.  (C-1)
        //
        // Testing:
        //   FlatHashMap(const FlatHashMap&, Allocator *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "Testing the copy constructor." << endl;
        {
            typedef bdlc::FlatHashMap<int, int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mW(&oa);  const Obj& W = mW;
            Obj mZ(&oa);  const Obj& Z = mZ;

            mW.insert(bsl::make_pair(1, 1));
            mZ.insert(bsl::make_pair(2, 2));

            ASSERT(W != Z);

            bslma::TestAllocator da("temporary default",
                                    veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(W);  const Obj& X = mX;

            Obj mY(Z, &oa);  const Obj& Y = mY;

            ASSERT(X == W);
            ASSERT(Y == Z);

            ASSERT(&da == X.allocator());
            ASSERT(&oa == Y.allocator());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 The equality operators correctly forward to the implementation
        //:   object and propagate the return value from the implementation
        //:   object.
        //
        // Plan:
        //: 1 Create three distinct object values and verify the results of
        //:   of all nine possible comparisons for each operator, using 'const'
        //:   items exclusively.  (C-1)
        //
        // Testing:
        //   bool operator==(const FlatHashMap&, const FlatHashMap&);
        //   bool operator!=(const FlatHashMap&, const FlatHashMap&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY OPERATORS" << endl
                          << "==================" << endl;

        if (verbose) cout << "Verify the equality operators." << endl;
        {
            typedef bdlc::FlatHashMap<int, int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            Obj mY(&oa);  const Obj& Y = mY;

            mY.insert(bsl::make_pair(1, 1));

            Obj mZ(&oa);  const Obj& Z = mZ;

            mZ.insert(bsl::make_pair(1, 1));
            mZ.insert(bsl::make_pair(2, 2));

            ASSERT( true == (X == X));
            ASSERT(false == (X == Y));
            ASSERT(false == (X == Z));
            ASSERT(false == (X != X));
            ASSERT( true == (X != Y));
            ASSERT( true == (X != Z));

            ASSERT(false == (Y == X));
            ASSERT( true == (Y == Y));
            ASSERT(false == (Y == Z));
            ASSERT( true == (Y != X));
            ASSERT(false == (Y != Y));
            ASSERT( true == (Y != Z));

            ASSERT(false == (Z == X));
            ASSERT(false == (Z == Y));
            ASSERT( true == (Z == Z));
            ASSERT( true == (Z != X));
            ASSERT( true == (Z != Y));
            ASSERT(false == (Z != Z));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4,7)
        //:
        //: 2 Using the table-driven technique: (C-1..3,5..6,8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the two
        //:     formatting parameters, along with the expected output ( 'value'
        //:     x 'level' x 'spacesPerLevel' ):
        //:     1 { A } x { 0 } x { 0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 8 expected o/ps
        //:     3 { B } x { 2 } x { 3 } --> 1 expected op
        //:     4 { A B } x { -8 } x { -8 } --> 2 expected o/ps
        //:     4 { A B } x { -9 } x { -9 } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1: (C-1..3,5,7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5,8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3,6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream& stream, const FlatHashMap& map);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        typedef bdlc::FlatHashMap<int, int, SeedIsHash<int> > Obj;

        if (verbose) cout << "Assign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace bdlc;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;

            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
               "Create a table of distinct value/format combinations." << endl;
        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            bsl::size_t d_numElems;
            const int   d_array[5];

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 } x { 0, 1, -1, -8 } --> 4 expected o/ps
        // ------------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,   0,   0,   0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  0,  {},         "["                                SP
                                          "]"                                },
        { L_,   0,   -8,  0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   0,   1,  {0},        "["                                NL
                                          "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "]"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   1,  {0},        "["                                NL
                                          " ["                               NL
                                          "  0"                              NL
                                          "  1"                              NL
                                          " ]"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  1,  {0},        "["                                SP
                                          "["                                SP
                                          "0"                                SP
                                          "1"                                SP
                                          "]"                                SP
                                          "]"                                },
        { L_,   0,   -8,  1,  {0},        "["                                NL
                                          "    ["                            NL
                                          "        0"                        NL
                                          "        1"                        NL
                                          "    ]"                            NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   0,   3,  {0,1,-1},   "["                                NL
                                          "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "]"                                NL
                                          "["                                NL
                                          "1"                                NL
                                          "2"                                NL
                                          "]"                                NL
                                          "["                                NL
                                          "-1"                               NL
                                          "0"                                NL
                                          "]"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   3,  {0,1,-1},   "["                                NL
                                          " ["                               NL
                                          "  0"                              NL
                                          "  1"                              NL
                                          " ]"                               NL
                                          " ["                               NL
                                          "  1"                              NL
                                          "  2"                              NL
                                          " ]"                               NL
                                          " ["                               NL
                                          "  -1"                             NL
                                          "  0"                              NL
                                          " ]"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  3,  {0,1,-1},   "["                                SP
                                          "["                                SP
                                          "0"                                SP
                                          "1"                                SP
                                          "]"                                SP
                                          "["                                SP
                                          "1"                                SP
                                          "2"                                SP
                                          "]"                                SP
                                          "["                                SP
                                          "-1"                               SP
                                          "0"                                SP
                                          "]"                                SP
                                          "]"                                },
        { L_,   0,   -8,  3,  {0,1,-1},   "["                                NL
                                          "    ["                            NL
                                          "        0"                        NL
                                          "        1"                        NL
                                          "    ]"                            NL
                                          "    ["                            NL
                                          "        1"                        NL
                                          "        2"                        NL
                                          "    ]"                            NL
                                          "    ["                            NL
                                          "        -1"                       NL
                                          "        0"                        NL
                                          "    ]"                            NL
                                          "]"                                NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 6 expected o/ps
        // ------------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,   3,   0,   0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   0,  {},         "      ["                          NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  0,  {},         "      ["                          SP
                                          "]"                                },
        { L_,   3,   -8,  0,  {},         "            ["                    NL
                                          "            ]"                    NL
                                                                             },
        { L_,   3,   0,   1,  {0},        "["                                NL
                                          "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "]"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   1,  {0},        "      ["                          NL
                                          "        ["                        NL
                                          "          0"                      NL
                                          "          1"                      NL
                                          "        ]"                        NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  1,  {0},        "      ["                          SP
                                          "["                                SP
                                          "0"                                SP
                                          "1"                                SP
                                          "]"                                SP
                                          "]"                                },
        { L_,   3,   -8,  1,  {0},        "            ["                    NL
                                          "                ["                NL
                                          "                    0"            NL
                                          "                    1"            NL
                                          "                ]"                NL
                                          "            ]"                    NL
                                                                             },
        { L_,   3,   0,   3,  {0,1,-1},   "["                                NL
                                          "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "]"                                NL
                                          "["                                NL
                                          "1"                                NL
                                          "2"                                NL
                                          "]"                                NL
                                          "["                                NL
                                          "-1"                               NL
                                          "0"                                NL
                                          "]"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   3,  {0,1,-1},   "      ["                          NL
                                          "        ["                        NL
                                          "          0"                      NL
                                          "          1"                      NL
                                          "        ]"                        NL
                                          "        ["                        NL
                                          "          1"                      NL
                                          "          2"                      NL
                                          "        ]"                        NL
                                          "        ["                        NL
                                          "          -1"                     NL
                                          "          0"                      NL
                                          "        ]"                        NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  3,  {0,1,-1},   "      ["                          SP
                                          "["                                SP
                                          "0"                                SP
                                          "1"                                SP
                                          "]"                                SP
                                          "["                                SP
                                          "1"                                SP
                                          "2"                                SP
                                          "]"                                SP
                                          "["                                SP
                                          "-1"                               SP
                                          "0"                                SP
                                          "]"                                SP
                                          "]"                                },
        { L_,   3,   -8,  3,  {0,1,-1},   "            ["                    NL
                                          "                ["                NL
                                          "                    0"            NL
                                          "                    1"            NL
                                          "                ]"                NL
                                          "                ["                NL
                                          "                    1"            NL
                                          "                    2"            NL
                                          "                ]"                NL
                                          "                ["                NL
                                          "                    -1"           NL
                                          "                    0"            NL
                                          "                ]"                NL
                                          "            ]"                    NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 } x { 3 } --> 1 expected o/p
        // -----------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,  2,  3,   3,  {0,1,-1},     "      ["                          NL
                                          "         ["                       NL
                                          "            0"                    NL
                                          "            1"                    NL
                                          "         ]"                       NL
                                          "         ["                       NL
                                          "            1"                    NL
                                          "            2"                    NL
                                          "         ]"                       NL
                                          "         ["                       NL
                                          "            -1"                   NL
                                          "            0"                    NL
                                          "         ]"                       NL
                                          "      ]"                          NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 } x { -8 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,  -8,  -8,  1,   {0},        "["                                NL
                                          "    ["                            NL
                                          "        0"                        NL
                                          "        1"                        NL
                                          "    ]"                            NL
                                          "]"                                NL
                                                                             },
        { L_,  -8,  -8,  3,   {0,1,-1},   "["                                NL
                                          "    ["                            NL
                                          "        0"                        NL
                                          "        1"                        NL
                                          "    ]"                            NL
                                          "    ["                            NL
                                          "        1"                        NL
                                          "        2"                        NL
                                          "    ]"                            NL
                                          "    ["                            NL
                                          "        -1"                       NL
                                          "        0"                        NL
                                          "    ]"                            NL
                                          "]"                                NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 } x { -9 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  OFF   FLAG  DESC  EXP
        //---- - ---  ---   ----  ----  ---

        // { L_,  -9,  -9,  1,   {0},        "[ 0 ]"                         },
        // { L_,  -9,  -9,  3,   {0,1,-1},   "[ 0 1 -1 ]"                    },

#undef NL
#undef SP

    };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "Testing with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const int         L         = DATA[ti].d_level;
                const int         SPL       = DATA[ti].d_spacesPerLevel;
                const bsl::size_t NUM_ELEMS = DATA[ti].d_numElems;
                const int  *const ARRAY    =  DATA[ti].d_array;
                const char *const EXP       = DATA[ti].d_expected_p;

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX;  const Obj& X = mX;
                for (bsl::size_t i = 0; i < NUM_ELEMS; ++i) {
                    if (veryVerbose) { P_(LINE) P(i) }

                    mX.insert(bsl::make_pair(ARRAY[i], ARRAY[i] + 1));
                }

                ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVerbose) {
                    cout << "\t\t" << LINE << endl;
                    cout << "PRINT STRING" << endl;
                    for ( bsl::size_t i = 0; i < os.str().size(); ++i) {
                        cout << static_cast<int>(os.str()[i]) << " ";
                    }
                    cout << "\nPRINT EXP" << endl;
                    for ( bsl::size_t i = 0; i < strlen(EXP); ++i) {
                        cout << static_cast<int>(EXP[i]) << " ";
                    }
                    cout << "\n----------------------------------------\n\n";
                }
                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute of
        //:   the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates memory.
        //
        // Plan:
        //: 1 Produce objects of arbitrary state and verify the accessors'
        //:   return values against expected values.  Since the accessors all
        //:   forward to an implementation class, verifying the forwarding is
        //:   correct is all that is required.  (C-1)
        //:
        //: 2 The accessors will only be accessed from a 'const' reference to
        //:   the created object.  (C-2)
        //:
        //: 3 The allocators used to create the objects will be verified to
        //:   ensure that no memory was allocated during use of the accessors
        //:   (note that this check may be done at the global level).  (C-3)
        //
        // Testing:
        //   size_t capacity() const;
        //   const_iterator find(const KEY&) const;
        //   HASH hash_function() const;
        //   EQUAL key_eq() const;
        //   float max_load_factor() const;
        //   size_t size() const;
        //   Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) {
            cout << "\nTesting 'capacity', 'hash_function', 'key_eq', "
                 << "'max_load_factor', and 'allocator'."
                 << endl;
        }
        {
            {
                typedef SeedIsHash<int>                          Hash;
                typedef bsl::equal_to<int>                       Equal;
                typedef bdlc::FlatHashMap<int, int, Hash, Equal> Obj;

                Obj mX(0, Hash(), Equal());  const Obj& X = mX;

                ASSERT(                0 == X.capacity());
                ASSERT(                0 == X.hash_function()(0));
                ASSERT(             true == X.key_eq()(0, 0));
                ASSERT(            false == X.key_eq()(0, 1));
                ASSERT(            0.875 == X.max_load_factor());
                ASSERT(&defaultAllocator == X.allocator());
            }
            {
                typedef SeedIsHash<int>                          Hash;
                typedef bsl::less<int>                           Equal;
                typedef bdlc::FlatHashMap<int, int, Hash, Equal> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(32, Hash(1), Equal(), &oa);  const Obj& X = mX;

                ASSERT(   32 == X.capacity());
                ASSERT(    1 == X.hash_function()(0));
                ASSERT(false == X.key_eq()(0, 0));
                ASSERT( true == X.key_eq()(0, 1));
                ASSERT(0.875 == X.max_load_factor());
                ASSERT(  &oa == X.allocator());
            }
        }

        if (verbose) cout << "Testing 'find' and 'size'." << endl;
        {
            typedef bdlc::FlatHashMap<int, int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            ASSERT(      0 == X.size());
            ASSERT(X.end() == X.find(1));
            ASSERT(X.end() == X.find(2));
            ASSERT(X.end() == X.find(3));

            mX.insert(bsl::make_pair(1, 5));

            ASSERT(      1 == X.size());
            ASSERT(      1 == X.find(1)->first);
            ASSERT(      5 == X.find(1)->second);
            ASSERT(X.end() == X.find(2));
            ASSERT(X.end() == X.find(3));

            mX.insert(bsl::make_pair(2, 7));

            ASSERT(      2 == X.size());
            ASSERT(      1 == X.find(1)->first);
            ASSERT(      5 == X.find(1)->second);
            ASSERT(      2 == X.find(2)->first);
            ASSERT(      7 == X.find(2)->second);
            ASSERT(X.end() == X.find(3));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMITIVE GENERATOR FUNCTIONS
        //
        // The functionality was omitted.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMITIVE GENERATOR FUNCTIONS" << endl
                          << "=============================" << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   The constructor, the destructor, and the primary manipulators:
        //      - 'clear'
        //      - 'erase(key)'
        //      - 'insert'
        //      - 'reset'
        //   operate as expected.
        //
        // Concerns:
        //: 1 The constructors create the correct initial value and have the
        //:   hashing, equality, and internal memory management systems hooked
        //:   up properly.
        //:
        //: 2 The primary manipulator 'insert' correctly forwards to the
        //:   implementation class, correctly forwards the return value from
        //:   the implementation class, and honors
        //:   bitwise-copy/bitwise-move/move.
        //:
        //: 3 The primary manipulator 'erase(key)' correctly forwards to the
        //:   implementation class and correctly forwards the return value from
        //:   the implementation class.
        //:
        //: 4 The primary manipulators 'clear' and 'reset' correctly forward
        //:   to the implementation class.
        //:
        //: 5 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //
        // Plan:
        //: 1 Create various objects using the constructors and directly verify
        //:   the arguments were stored correctly using the (untested) basic
        //:   accessors.  Verify all allocations are done from the allocator
        //:   in future tests.  (C-1)
        //:
        //: 2 Create objects using the 'bslma::TestAllocator', use the 'insert'
        //:   method with various values, verify the return value, and use the
        //:   (untested) basic accessors to verify the value of the object.
        //:   Use 'bsltf' test types to verify bitwise-copy/bitwise-move/move
        //:   are honored.  (C-2)
        //:
        //: 3 Create objects using the 'bslma::TestAllocator', use 'insert'
        //:   to obtain various states, use 'erase', verify the return value,
        //:   and use the (untested) basic accessors to verify the value of the
        //:   object.  (C-3)
        //:
        //: 4 Create objects using the 'bslma::TestAllocator', use 'insert' to
        //:   obtain various states, use the primary manipulators 'clear' and
        //:   'reset', and verify the object's value using the (untested) basic
        //:   accessors.  (C-4)
        //:
        //: 5 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-5)
        //
        // Testing:
        //   FlatHashMap();
        //   FlatHashMap(Allocator *basicAllocator);
        //   FlatHashMap(size_t capacity);
        //   FlatHashMap(size_t capacity, Allocator *basicAllocator);
        //   FlatHashMap(size_t capacity, const HASH&, Allocator *bA = 0);
        //   FlatHashMap(size_t, const HASH&, const EQUAL&, Allocator * = 0);
        //   ~FlatHashMap();
        //   void clear();
        //   size_t erase(const KEY&);
        //   bsl::pair<iterator, bool> insert(FORWARD_REF(VALUE_TYPE) entry)
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        bslma::TestAllocator sa("string", veryVeryVeryVerbose);

        const bsl::string SA("abc",    &sa);
        const bsl::string SB("abcdef", &sa);
        const bsl::string SC("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU",
                             &sa);

        if (verbose) cout << "Testing constructors." << endl;
        {
            // FlatHashMap();

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                Obj mX;  const Obj& X = mX;

                ASSERT(                0 == X.capacity());
                ASSERT(     ExpHash()(0) == X.hash_function()(0));
                ASSERT(     ExpHash()(1) == X.hash_function()(1));
                ASSERT(     ExpHash()(7) == X.hash_function()(7));
                ASSERT(             true == X.key_eq()(0, 0));
                ASSERT(            false == X.key_eq()(0, 1));
                ASSERT(&defaultAllocator == X.allocator());
            }
            {
                typedef bsl::string                               Key;
                typedef float                                     Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                Obj mX;  const Obj& X = mX;

                ASSERT(                0 == X.capacity());
                ASSERT(    ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(    ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(    ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(             true == X.key_eq()(SA, SA));
                ASSERT(            false == X.key_eq()(SA, SB));
                ASSERT(&defaultAllocator == X.allocator());

            }

            // FlatHashMap(Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                Obj mX((bslma::Allocator *)0);  const Obj& X = mX;

                ASSERT(                0 == X.capacity());
                ASSERT(     ExpHash()(0) == X.hash_function()(0));
                ASSERT(     ExpHash()(1) == X.hash_function()(1));
                ASSERT(     ExpHash()(7) == X.hash_function()(7));
                ASSERT(             true == X.key_eq()(0, 0));
                ASSERT(            false == X.key_eq()(0, 1));
                ASSERT(&defaultAllocator == X.allocator());
            }
            {
                typedef bsl::string                               Key;
                typedef float                                     Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = mX;

                ASSERT(            0 == X.capacity());
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &oa == X.allocator());

            }

            // FlatHashMap(size_t capacity);

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(32);  const Obj& X = mX;

                ASSERT(          32 == X.capacity());
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());

                ASSERT(2 == da.numAllocations());
            }
            {
                typedef bsl::string                               Key;
                typedef float                                     Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(64);  const Obj& X = mX;

                ASSERT(           64 == X.capacity());
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &da == X.allocator());

                ASSERT(2 == da.numAllocations());
            }

            // FlatHashMap(size_t capacity, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef int                                       Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(32, (bslma::Allocator *)0);  const Obj& X = mX;

                ASSERT(          32 == X.capacity());
                ASSERT(ExpHash()(0) == X.hash_function()(0));
                ASSERT(ExpHash()(1) == X.hash_function()(1));
                ASSERT(ExpHash()(7) == X.hash_function()(7));
                ASSERT(        true == X.key_eq()(0, 0));
                ASSERT(       false == X.key_eq()(0, 1));
                ASSERT(         &da == X.allocator());

                ASSERT(2 == da.numAllocations());
            }
            {
                typedef bsl::string                               Key;
                typedef float                                     Value;
                typedef bdlc::FlatHashMap<Key, Value>             Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(64, &oa);  const Obj& X = mX;

                ASSERT(           64 == X.capacity());
                ASSERT(ExpHash()(SA) == X.hash_function()(SA));
                ASSERT(ExpHash()(SB) == X.hash_function()(SB));
                ASSERT(ExpHash()(SC) == X.hash_function()(SC));
                ASSERT(         true == X.key_eq()(SA, SA));
                ASSERT(        false == X.key_eq()(SA, SB));
                ASSERT(          &oa == X.allocator());

                ASSERT(2 == oa.numAllocations());
            }

            // FlatHashMap(size_t capacity, const HASH&, Allocator *bA = 0);

            {
                typedef int                                 Key;
                typedef int                                 Value;
                typedef SeedIsHash<Key>                     Hash;
                typedef bdlc::FlatHashMap<Key, Value, Hash> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Hash hasher(7);

                Obj mX(32, hasher);  const Obj& X = mX;

                ASSERT(   32 == X.capacity());
                ASSERT(    7 == X.hash_function()(0));
                ASSERT(    7 == X.hash_function()(1));
                ASSERT(    7 == X.hash_function()(7));
                ASSERT( true == X.key_eq()(0, 0));
                ASSERT(false == X.key_eq()(0, 1));
                ASSERT(  &da == X.allocator());

                ASSERT(2 == da.numAllocations());
            }
            {
                typedef bsl::string                         Key;
                typedef float                               Value;
                typedef SeedIsHash<Key>                     Hash;
                typedef bdlc::FlatHashMap<Key, Value, Hash> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Hash hasher(9);

                Obj mX(64, hasher, &oa);  const Obj& X = mX;

                ASSERT(   64 == X.capacity());
                ASSERT(    9 == X.hash_function()(SA));
                ASSERT(    9 == X.hash_function()(SB));
                ASSERT(    9 == X.hash_function()(SC));
                ASSERT( true == X.key_eq()(SA, SA));
                ASSERT(false == X.key_eq()(SA, SB));
                ASSERT(  &oa == X.allocator());

                ASSERT(2 == oa.numAllocations());
            }

            // FlatHashMap(size_t, const HASH&, const EQUAL&, Allocator * = 0);

            {
                typedef int                                        Key;
                typedef int                                        Value;
                typedef SeedIsHash<Key>                            Hash;
                typedef bsl::equal_to<Key>                         Equal;
                typedef bdlc::FlatHashMap<Key, Value, Hash, Equal> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Hash hasher(7);

                Obj mX(32, hasher, Equal());  const Obj& X = mX;

                ASSERT(   32 == X.capacity());
                ASSERT(    7 == X.hash_function()(0));
                ASSERT(    7 == X.hash_function()(1));
                ASSERT(    7 == X.hash_function()(7));
                ASSERT( true == X.key_eq()(0, 0));
                ASSERT(false == X.key_eq()(0, 1));
                ASSERT(  &da == X.allocator());

                ASSERT(2 == da.numAllocations());
            }
            {
                typedef bsl::string                                Key;
                typedef float                                      Value;
                typedef SeedIsHash<Key>                            Hash;
                typedef bsl::less<Key>                             Equal;
                typedef bdlc::FlatHashMap<Key, Value, Hash, Equal> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Hash hasher(9);

                Obj mX(64, hasher, Equal(), &oa);  const Obj& X = mX;

                ASSERT(   64 == X.capacity());
                ASSERT(    9 == X.hash_function()(SA));
                ASSERT(    9 == X.hash_function()(SB));
                ASSERT(    9 == X.hash_function()(SC));
                ASSERT(false == X.key_eq()(SA, SA));
                ASSERT( true == X.key_eq()(SA, SB));
                ASSERT(  &oa == X.allocator());

                ASSERT(2 == oa.numAllocations());
            }
        }

        if (verbose) cout << "Testing copy 'insert'." << endl;
        {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            testCaseInsert<bsltf::AllocBitwiseMoveableTestType>(1,
                                                                true,
                                                                false,
                                                                false);
            testCaseInsert<bsltf::AllocTestType>(2, true, true, false);
            testCaseInsert<bsltf::BitwiseCopyableTestType>(3,
                                                           false,
                                                           false,
                                                           false);
            testCaseInsert<bsltf::BitwiseMoveableTestType>(4,
                                                           false,
                                                           false,
                                                           false);
            testCaseInsert<bsltf::MovableAllocTestType>(5, true, false, false);
            testCaseInsert<bsltf::MovableTestType>(6, false, false, false);
            testCaseInsert<bsltf::NonDefaultConstructibleTestType>(7,
                                                                   false,
                                                                   false,
                                                                   false);
            testCaseInsert<bsltf::NonOptionalAllocTestType>(8,
                                                            true,
                                                            true,
                                                            false);
#else
            testCaseInsert<bsltf::AllocBitwiseMoveableTestType>(1,
                                                                true,
                                                                false,
                                                                false);
            testCaseInsert<bsltf::AllocTestType>(2, true, true, false);
            testCaseInsert<bsltf::BitwiseCopyableTestType>(3,
                                                           false,
                                                           false,
                                                           false);
            testCaseInsert<bsltf::BitwiseMoveableTestType>(4,
                                                           false,
                                                           false,
                                                           false);
            testCaseInsert<bsltf::MovableAllocTestType>(5, true, true, false);
            testCaseInsert<bsltf::MovableTestType>(6, false, false, false);
            testCaseInsert<bsltf::NonDefaultConstructibleTestType>(7,
                                                                   false,
                                                                   false,
                                                                   false);
            testCaseInsert<bsltf::NonOptionalAllocTestType>(8,
                                                            true,
                                                            true,
                                                            false);
#endif
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) cout << "Testing move 'insert'." << endl;
        {
            testCaseInsertMove<bsltf::AllocBitwiseMoveableTestType>(1,
                                                                    true,
                                                                    true,
                                                                    false);
            testCaseInsertMove<bsltf::AllocTestType>(2, true, false, false);
            testCaseInsertMove<bsltf::BitwiseCopyableTestType>(3,
                                                               false,
                                                               true,
                                                               false);
            testCaseInsertMove<bsltf::BitwiseMoveableTestType>(4,
                                                               false,
                                                               true,
                                                               false);
            testCaseInsertMove<bsltf::MovableAllocTestType>(5,
                                                            true,
                                                            true,
                                                            false);
            testCaseInsertMove<bsltf::MovableTestType>(6, false, true, false);

            testCaseInsertMove<bsltf::MoveOnlyAllocTestType>(7,
                                                             true,
                                                             true,
                                                             false);

            testCaseInsertMove<bsltf::NonDefaultConstructibleTestType>(8,
                                                                       false,
                                                                       false,
                                                                       false);
            testCaseInsertMove<bsltf::NonOptionalAllocTestType>(9,
                                                                true,
                                                                false,
                                                                false);
        }
#endif

        if (verbose) cout << "Testing 'erase'." << endl;
        {
            typedef int                           Key;
            typedef int                           Value;
            typedef bdlc::FlatHashMap<Key, Value> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(2, 2));
            mX.insert(bsl::make_pair(3, 3));

            ASSERT(      1 == mX.erase(2));
            ASSERT(      2 == X.size());
            ASSERT(X.end() == X.find(2));

            ASSERT(      0 == mX.erase(2));
            ASSERT(      2 == X.size());
            ASSERT(X.end() == X.find(2));

            ASSERT(      1 == mX.erase(1));
            ASSERT(      1 == X.size());
            ASSERT(X.end() == X.find(1));
        }

        if (verbose) cout << "Testing 'clear'." << endl;
        {
            typedef int                           Key;
            typedef int                           Value;
            typedef bdlc::FlatHashMap<Key, Value> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(32, &oa);  const Obj& X = mX;

            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(2, 2));
            mX.insert(bsl::make_pair(3, 3));

            mX.clear();

            ASSERT( 0 == X.size());
            ASSERT(32 == X.capacity());

            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(2, 2));

            mX.clear();

            ASSERT( 0 == X.size());
            ASSERT(32 == X.capacity());
        }

        if (verbose) cout << "Testing 'reset'." << endl;
        {
            typedef int                           Key;
            typedef int                           Value;
            typedef bdlc::FlatHashMap<Key, Value> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(32, &oa);  const Obj& X = mX;

            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(2, 2));
            mX.insert(bsl::make_pair(3, 3));

            mX.reset();

            ASSERT(0 == X.size());
            ASSERT(0 == X.capacity());

            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(2, 2));

            mX.reset();

            ASSERT(0 == X.size());
            ASSERT(0 == X.capacity());
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
        //: 1 Instantiate an object and verify basic functionality.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        typedef bdlc::FlatHashMap<int, int> Obj;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;

        ASSERT(       0 == X.size());
        ASSERT( X.end() == X.find(0));

        {
            bsl::pair<Obj::iterator, bool> rv =
                                               mX.insert(bsl::make_pair(0, 7));

            ASSERT(       1 == X.size());
            ASSERT(    true == rv.second);
            ASSERT( X.end() != rv.first);
            ASSERT(       0 == rv.first->first);
            ASSERT(       7 == rv.first->second);
            ASSERT(rv.first == X.find(0));
            ASSERT( X.end() == X.find(1));
        }

        Obj mY(X, &oa); const Obj& Y = mY;

        ASSERT(      1 == Y.size());
        ASSERT(      0 == Y.find(0)->first);
        ASSERT(      7 == Y.find(0)->second);
        ASSERT(Y.end() == Y.find(1));
        ASSERT(      X == Y);
        ASSERT(      Y == X);

        {
            bsl::size_t rv = mY.erase(0);

            ASSERT(      1 == rv);
            ASSERT(      0 == Y.size());
            ASSERT(Y.end() == Y.find(0));
            ASSERT(Y.end() == Y.find(1));
            ASSERT(      X != Y);
            ASSERT(      Y != X);
        }

        {
            bsl::pair<Obj::iterator, bool> rv =
                                               mY.insert(bsl::make_pair(1, 5));

            ASSERT(       1 == Y.size());
            ASSERT(    true == rv.second);
            ASSERT( Y.end() != rv.first);
            ASSERT(       1 == rv.first->first);
            ASSERT(       5 == rv.first->second);
            ASSERT( Y.end() == Y.find(0));
            ASSERT(rv.first == Y.find(1));
            ASSERT(       X != Y);
            ASSERT(       Y != X);
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //    Verify 'bdlc::FlatHashMap' can outperform 'bsl::unordered_map'.
        //
        // Concerns:
        //: 1 'bdlc::FlatHashMap' can outperform 'bsl::unordered_map' when
        //:   'find' is used for values present in the map.
        //:
        //: 2 'bdlc::FlatHashMap' can outperform 'bsl::unordered_map' when
        //:   'find' is used for values not present in the map.
        //
        // Plan:
        //: 1 Perform a performance test for 'find' with values in the map
        //:   and verify 'bdlc::FlatHashMap' outperforms 'bsl::unordered_map'.
        //:   (C-1)
        //:
        //: 2 Perform a performance test for 'find' with values not in the map
        //:   and verify 'bdlc::FlatHashMap' outperforms 'bsl::unordered_map'.
        //:   (C-2)
        //
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;

        bslma::NewDeleteAllocator oa;

        bslma::DefaultAllocatorGuard dag(&oa);

        {
            bdlc::FlatHashMap<short, short>  mX;
            bsl::unordered_map<short, short> mY;

            mY.max_load_factor(mX.max_load_factor());

            double x = static_cast<double>(
                               performanceFindPresent(&mX).totalNanoseconds());
            double y = static_cast<double>(
                               performanceFindPresent(&mY).totalNanoseconds());

            ASSERT(x < y);

            cout << "flat is "
                 << 100.0 * (y - x) / x
                 << "% faster than unordered for find when present"
                 << endl;
        }
        {
            bdlc::FlatHashMap<short, short>  mX;
            bsl::unordered_map<short, short> mY;

            mY.max_load_factor(mX.max_load_factor());

            double x = static_cast<double>(
                            performanceFindNotPresent(&mX).totalNanoseconds());
            double y = static_cast<double>(
                            performanceFindNotPresent(&mY).totalNanoseconds());

            ASSERT(x < y);

            cout << "flat is "
                 << 100.0 * (y - x) / x
                 << "% faster than unordered for find when not present"
                 << endl;
        }

        if (veryVeryVeryVerbose) {
            cout << "anti-optimization: " << s_antiOptimization << endl;
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                0 == defaultAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
