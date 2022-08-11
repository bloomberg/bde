// bdlc_flathashset.t.cpp                                             -*-C++-*-

#include <bdlc_flathashset.h>
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
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_set.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a container implementing a flat hash set.
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
//: o [ 3] FlatHashSet& gg(FlatHashSet *object, const char *spec);
//: o [ 3] int ggg(FlatHashSet *object, const char *spec, int verboseFlag);
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
// [ 2] FlatHashSet();
// [ 2] FlatHashSet(Allocator *basicAllocator);
// [ 2] FlatHashSet(size_t capacity);
// [ 2] FlatHashSet(size_t capacity, Allocator *basicAllocator);
// [ 2] FlatHashSet(size_t capacity, const HASH&, Allocator *bA = 0);
// [ 2] FlatHashSet(size_t, const HASH&, const EQUAL&, Allocator * = 0);
// [20] FlatHashSet(INPUT_ITERATOR, INPUT_ITERATOR, Allocator *bA = 0);
// [20] FlatHashSet(INPUT_ITER, INPUT_ITER, size_t, Allocator * = 0);
// [20] FlatHashSet(II, II, size_t, const HASH&, Allocator * = 0);
// [20] FlatHashSet(II, II, size_t, const H&, const EQ&, Alloc * = 0);
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
// [21] FlatHashSet(bsl::initializer_list<KEY> values, Allocator * = 0);
// [21] FlatHashSet(bsl::initializer_list<KEY>, size_t, Allocator * = 0);
// [21] FlatHashSet(init_list<KEY>, size_t, const HASH&, Allocator * = 0);
// [21] FlatHashSet(init_list<K>, size_t, const H&, const EQ&, A * = 0);
// #endif
// [ 7] FlatHashSet(const FlatHashSet&, Allocator *bA = 0);
// [13] FlatHashSet(FlatHashSet&&);
// [14] FlatHashSet(FlatHashSet&&, Allocator *basicAllocator);
// [ 2] ~FlatHashSet();
//
// MANIPULATORS
// [ 9] FlatHashSet& operator=(const FlatHashSet&);
// [15] FlatHashSet& operator=(FlatHashSet&&);
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
// [22] FlatHashSet& operator=(bsl::initializer_list<KEY> values);
// #endif
// [ 2] void clear();
// [ 2] size_t erase(const KEY&);
// [17] const_iterator erase(const_iterator);
// [18] const_iterator erase(const_iterator, const_iterator);
// [ 2] bsl::pair<iterator, bool> insert(FORWARD_REF(ENTRY_TYPE) entry)
// [26] iterator insert(const_iterator, FORWARD_REF(ENTRY_TYPE) entry)
// [16] void insert(INPUT_ITERATOR, INPUT_ITERATOR);
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
// [23] void insert(bsl::initializer_list<KEY> values);
// #endif
// [19] void rehash(size_t);
// [19] void reserve(size_t);
// [ 2] void reset();
// [ 8] void swap(FlatHashSet&);
//
// ACCESSORS
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
// [ 6] bool operator==(const FlatHashSet&, const FlatHashSet&);
// [ 6] bool operator!=(const FlatHashSet&, const FlatHashSet&);
// [ 5] ostream& operator<<(ostream& stream, const FlatHashSet& set);
//
// FREE FUNCTIONS
// [ 8] void swap(FlatHashSet&, FlatHashSet&);
// ----------------------------------------------------------------------------
// [27] USAGE EXAMPLE
// [24] CONCERN: 'FlatHashMap' has the necessary type traits
// [25] DRQS 165258625: 'insert' could create reference to temporary
// [27] DRQS 169531176: 'bsl::inserter' usage on Sun
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
    // 'KEY' to hash.

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
        // Create a 'EqualAndState' object having the default state value.
    : d_state()
    {
    }

    explicit EqualAndState(const TYPE& state)
        // Create a 'EqualAndState' object having the specified 'state'.
    : d_state(state)
    {
    }

    EqualAndState(const EqualAndState& original)
        // Create a 'EqualAndState' object having the value of the specified
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
    typedef bdlc::FlatHashSet<KEY, TestValueIsHash<KEY> > Obj;

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

        bsls::ObjectBuffer<KEY> key1;
        bslma::ConstructionUtil::construct(key1.address(), &oa, 1);

        bslma::DestructorGuard<KEY> guard1(&key1.object());

        bsls::ObjectBuffer<KEY> key2;
        bslma::ConstructionUtil::construct(key2.address(), &oa, 2);

        bslma::DestructorGuard<KEY> guard2(&key2.object());

        Obj mX(&oa);  const Obj& X = mX;

        Obj mY(&oa);  const Obj& Y = mY;
        mY.insert(key1.object());

        Obj mYY(Y, &oa);  const Obj& YY = mYY;

        Obj mZ(&oa);  const Obj& Z = mZ;
        mZ.insert(key2.object());

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

        bsls::ObjectBuffer<KEY> key1;
        bslma::ConstructionUtil::construct(key1.address(), &oa, 1);

        bslma::DestructorGuard<KEY> guard1(&key1.object());

        bsls::ObjectBuffer<KEY> key2;
        bslma::ConstructionUtil::construct(key2.address(), &sa, 2);

        bslma::DestructorGuard<KEY> guard2(&key2.object());

        Obj mX(&oa);  const Obj& X = mX;

        Obj mY(&sa);  const Obj& Y = mY;
        mY.insert(key1.object());

        Obj mYY(Y, &sa);  const Obj& YY = mYY;

        Obj mZ(&sa);  const Obj& Z = mZ;
        mZ.insert(key2.object());

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
    typedef bdlc::FlatHashSet<KEY, TestValueIsHash<KEY> > Obj;

    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

    for (int i = 0; i < 2; ++i) {
        // Create control object 'W'.
        Obj mW(&oa);  const Obj& W = mW;

        bsls::ObjectBuffer<KEY> key;
        bslma::ConstructionUtil::construct(key.address(), &oa, i);

        bslma::DestructorGuard<KEY> guard(&key.object());

        mW.insert(key.object());

        LOOP_ASSERT(id, i == W.begin()->data());

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
    typedef bdlc::FlatHashSet<KEY, TestValueIsHash<KEY> > Obj;

    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

    for (int i = 0; i < 2; ++i) {
        // Create control object 'W'.
        Obj mW(&oa);  const Obj& W = mW;

        bsls::ObjectBuffer<KEY> key;
        bslma::ConstructionUtil::construct(key.address(), &oa, i);

        bslma::DestructorGuard<KEY> guard(&key.object());

        mW.insert(key.object());

        LOOP_ASSERT(id, i == W.begin()->data());

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
    // Address the 'insert' concerns of test cases 2 and 26 for the specified
    // 'id', that can be used to determine the 'KEY' in case of a test failure,
    // for a type that allocates if the specified 'allocates' is 'true', and
    // allocates on rehashes if the specified 'allocatesOnRehash' is 'true'.
    // Use 'insert' with a hint if the specified 'useHint' is 'true'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef bdlc::FlatHashSet<KEY, TestValueIsHash<KEY> > Obj;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    bsls::Types::Int64 expAllocations = oa.numAllocations();
    bsl::size_t        expCapacity    = 0;

    Obj mX(&oa);  const Obj& X = mX;

    for (int i = 0; i <= thirdAlloc; ++i) {
        bsls::ObjectBuffer<KEY> key;
        bslma::ConstructionUtil::construct(key.address(), &oa, i);

        bslma::DestructorGuard<KEY> guard(&key.object());

        if (!useHint) {
            bsl::pair<typename Obj::iterator, bool> rv;

            rv = mX.insert(key.object());
            LOOP2_ASSERT(id, i,    i == rv.first->data());
            LOOP2_ASSERT(id, i, true == rv.second);

            rv = mX.insert(key.object());
            LOOP2_ASSERT(id, i,     i == rv.first->data());
            LOOP2_ASSERT(id, i, false == rv.second);
        }
        else {
            typename Obj::iterator rv;

            rv = mX.insert(X.begin(), key.object());
            LOOP2_ASSERT(id, i, i == rv->data());

            rv = mX.insert(X.end(), key.object());
            LOOP2_ASSERT(id, i, i == rv->data());
        }

        typename Obj::const_iterator iter = X.find(key.object());
        LOOP2_ASSERT(id, i, i == iter->data());

        key.object().setData(i + 1);
        iter = X.find(key.object());
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
    // Address the move 'insert' concerns of test cases 2 and 26 for the
    // specified 'id' that can be used to determine 'KEY' in case of a test
    // failure, for a type that allocates if the specified 'allocates' is
    // 'true', and can be moved if the specified 'moveable' is 'true'.  Use
    // 'insert' with a hint if the specified 'useHint' is 'true'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef bdlc::FlatHashSet<KEY, TestValueIsHash<KEY> > Obj;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    Obj mX(&oa);  const Obj& X = mX;

    bsls::Types::Int64 expAllocations = oa.numAllocations();
    bsl::size_t        expCapacity    = 0;

    for (int i = 0; i <= thirdAlloc; ++i) {
        bsls::ObjectBuffer<KEY> key1;
        bslma::ConstructionUtil::construct(key1.address(), &oa, i);

        bslma::DestructorGuard<KEY> guard1(&key1.object());

        bsls::ObjectBuffer<KEY> key2;
        bslma::ConstructionUtil::construct(key2.address(), &oa, i);

        bslma::DestructorGuard<KEY> guard2(&key2.object());

        if (!useHint) {
            bsl::pair<typename Obj::iterator, bool> rv;

            rv = mX.insert(bslmf::MovableRefUtil::move(key1.object()));
            LOOP2_ASSERT(id, i,    i == rv.first->data());
            LOOP2_ASSERT(id, i, true == rv.second);

            rv = mX.insert(bslmf::MovableRefUtil::move(key2.object()));
            LOOP2_ASSERT(id, i,     i == rv.first->data());
            LOOP2_ASSERT(id, i, false == rv.second);
        }
        else {
            typename Obj::iterator rv;

            rv = mX.insert(X.begin(),
                           bslmf::MovableRefUtil::move(key1.object()));
            LOOP2_ASSERT(id, i, i == rv->data());

            rv = mX.insert(X.end(),
                           bslmf::MovableRefUtil::move(key2.object()));
            LOOP2_ASSERT(id, i, i == rv->data());
        }

        typename Obj::const_iterator iter = X.find(key2.object());
        LOOP2_ASSERT(id, i, i == iter->data());

        key2.object().setData(i + 1);
        iter = X.find(key2.object());
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

template <class SET>
bsls::TimeInterval performanceFindPresent(SET *set)
    // For the specified 'set', insert a large number of values and then invoke
    // 'find()' with values matching those inserted.  Return the duration of
    // the 'find()' invocations.
{
    const int NUM_TRIAL = 101;
    const int MAX       = 4096;
    const int NUM_ITER  = 4096 * 128 / MAX;

    for (int i = 0; i < MAX; ++i) {
        set->insert(i);
    }

    bsl::vector<bsls::TimeInterval> results;
    for (int trial = 0; trial < NUM_TRIAL; ++trial) {
        for (int iter = 0; iter < NUM_ITER; ++iter) {
            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            for (int i = 0; i < MAX; ++i) {
                s_antiOptimization += *set->find((i * 7) & (MAX - 1));
            }

            results.push_back(bsls::SystemTime::nowMonotonicClock() - start);
        }
    }

    bsl::sort(results.begin(), results.end());

    return results[NUM_TRIAL / 2];
}

template <class SET>
bsls::TimeInterval performanceFindNotPresent(SET *set)
    // For the specified 'set', insert a large number of values and then invoke
    // 'find()' with values not matching those inserted.  Return the duration
    // of the 'find()' invocations.
{
    const int NUM_TRIAL = 101;
    const int MAX       = 4096;
    const int NUM_ITER  = 4096 * 128 / MAX;

    for (int i = 0; i < MAX; ++i) {
        set->insert(i * 2);
    }

    bsl::vector<bsls::TimeInterval> results;
    for (int trial = 0; trial < NUM_TRIAL; ++trial) {
        for (int iter = 0; iter < NUM_ITER; ++iter) {
            bsls::TimeInterval start = bsls::SystemTime::nowMonotonicClock();

            for (int i = 0; i < MAX; ++i) {
                if (set->end() == set->find(((i * 7) & (MAX - 1)) * 2 + 1)) {
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
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Categorizing Data
/// - - - - - - - - - - - - - -
// Unordered sets are useful in situations when there is no meaningful way to
// order key values, when the order of the values is irrelevant to the problem
// domain, or (even if there is a meaningful ordering) the value of ordering
// the results is outweighed by the higher performance provided by unordered
// sets (compared to ordered sets).  On platforms that support relevant SIMD
// instructions (e.g., SSE2), 'bdlc::FlatHashSet' generally exhibits better
// performance than 'bsl::unordered_set'.
//
// Suppose one is analyzing data on a set of customers, and each customer is
// categorized by several attributes: customer type, geographic area, and
// (internal) project code; and that each attribute takes on one of a limited
// set of values.  This data can be handled by creating an enumeration for each
// of the attributes:
//..
    typedef enum {
        e_REPEAT
      , e_DISCOUNT
      , e_IMPULSE
      , e_NEED_BASED
      , e_BUSINESS
      , e_NON_PROFIT
      , e_INSTITUTE
      // ...
    } CustomerCode;

    typedef enum {
        e_USA_EAST
      , e_USA_WEST
      , e_CANADA
      , e_MEXICO
      , e_ENGLAND
      , e_SCOTLAND
      , e_FRANCE
      , e_GERMANY
      , e_RUSSIA
      // ...
    } LocationCode;

    typedef enum {
        e_TOAST
      , e_GREEN
      , e_FAST
      , e_TIDY
      , e_PEARL
      , e_SMITH
      // ...
    } ProjectCode;
//..
// The data set (randomly generated for this example) is provided in a
// statically initialized array:
//..
    static const struct CustomerProfile {
        CustomerCode d_customer;
        LocationCode d_location;
        ProjectCode  d_project;
    } customerProfiles[] = {
        { e_IMPULSE   , e_CANADA  , e_SMITH },
        { e_NON_PROFIT, e_USA_EAST, e_GREEN },
        { e_INSTITUTE , e_USA_EAST, e_TOAST },
        { e_NON_PROFIT, e_CANADA  , e_PEARL },
        { e_NEED_BASED, e_CANADA  , e_FAST  },
        { e_BUSINESS  , e_ENGLAND , e_PEARL },
        { e_REPEAT    , e_SCOTLAND, e_TIDY  },
        { e_INSTITUTE , e_MEXICO  , e_PEARL },
        { e_DISCOUNT  , e_USA_EAST, e_GREEN },
        { e_BUSINESS  , e_USA_EAST, e_GREEN },
        { e_IMPULSE   , e_MEXICO  , e_TOAST },
        { e_DISCOUNT  , e_GERMANY , e_FAST  },
        { e_INSTITUTE , e_FRANCE  , e_FAST  },
        { e_NON_PROFIT, e_ENGLAND , e_PEARL },
        { e_BUSINESS  , e_ENGLAND , e_TIDY  },
        { e_BUSINESS  , e_CANADA  , e_GREEN },
        { e_INSTITUTE , e_FRANCE  , e_FAST  },
        { e_IMPULSE   , e_RUSSIA  , e_TOAST },
        { e_REPEAT    , e_USA_WEST, e_TOAST },
        { e_IMPULSE   , e_CANADA  , e_TIDY  },
        { e_NON_PROFIT, e_GERMANY , e_GREEN },
        { e_INSTITUTE , e_USA_EAST, e_TOAST },
        { e_INSTITUTE , e_FRANCE  , e_FAST  },
        { e_IMPULSE   , e_SCOTLAND, e_SMITH },
        { e_INSTITUTE , e_USA_EAST, e_PEARL },
        { e_INSTITUTE , e_USA_EAST, e_TOAST },
        { e_NON_PROFIT, e_ENGLAND , e_PEARL },
        { e_IMPULSE   , e_GERMANY , e_FAST  },
        { e_REPEAT    , e_GERMANY , e_FAST  },
        { e_REPEAT    , e_MEXICO  , e_PEARL },
        { e_IMPULSE   , e_GERMANY , e_TIDY  },
        { e_IMPULSE   , e_MEXICO  , e_TOAST },
        { e_NON_PROFIT, e_SCOTLAND, e_SMITH },
        { e_NEED_BASED, e_MEXICO  , e_TOAST },
        { e_NON_PROFIT, e_FRANCE  , e_SMITH },
        { e_INSTITUTE , e_MEXICO  , e_TIDY  },
        { e_NON_PROFIT, e_FRANCE  , e_TIDY  },
        { e_IMPULSE   , e_FRANCE  , e_FAST  },
        { e_DISCOUNT  , e_RUSSIA  , e_TIDY  },
        { e_IMPULSE   , e_USA_EAST, e_TIDY  },
        { e_IMPULSE   , e_USA_WEST, e_FAST  },
        { e_NON_PROFIT, e_FRANCE  , e_TIDY  },
        { e_BUSINESS  , e_ENGLAND , e_GREEN },
        { e_REPEAT    , e_FRANCE  , e_TOAST },
        { e_REPEAT    , e_RUSSIA  , e_SMITH },
        { e_REPEAT    , e_RUSSIA  , e_GREEN },
        { e_IMPULSE   , e_CANADA  , e_FAST  },
        { e_NON_PROFIT, e_USA_EAST, e_FAST  },
        { e_NEED_BASED, e_USA_WEST, e_TOAST },
        { e_NON_PROFIT, e_GERMANY , e_TIDY  },
        { e_NON_PROFIT, e_ENGLAND , e_GREEN },
        { e_REPEAT    , e_GERMANY , e_PEARL },
        { e_NEED_BASED, e_USA_EAST, e_PEARL },
        { e_NON_PROFIT, e_RUSSIA  , e_PEARL },
        { e_NEED_BASED, e_ENGLAND , e_SMITH },
        { e_INSTITUTE , e_CANADA  , e_SMITH },
        { e_NEED_BASED, e_ENGLAND , e_TOAST },
        { e_NON_PROFIT, e_MEXICO  , e_TIDY  },
        { e_BUSINESS  , e_GERMANY , e_FAST  },
        { e_NEED_BASED, e_SCOTLAND, e_PEARL },
        { e_NON_PROFIT, e_USA_WEST, e_TIDY  },
        { e_NON_PROFIT, e_USA_WEST, e_TOAST },
        { e_IMPULSE   , e_FRANCE  , e_PEARL },
        { e_IMPULSE   , e_ENGLAND , e_FAST  },
        { e_IMPULSE   , e_USA_WEST, e_GREEN },
        { e_DISCOUNT  , e_MEXICO  , e_SMITH },
        { e_INSTITUTE , e_GERMANY , e_TOAST },
        { e_NEED_BASED, e_CANADA  , e_PEARL },
        { e_NON_PROFIT, e_USA_WEST, e_FAST  },
        { e_DISCOUNT  , e_RUSSIA  , e_SMITH },
        { e_INSTITUTE , e_USA_WEST, e_GREEN },
        { e_INSTITUTE , e_RUSSIA  , e_TOAST },
        { e_INSTITUTE , e_FRANCE  , e_SMITH },
        { e_INSTITUTE , e_SCOTLAND, e_SMITH },
        { e_NON_PROFIT, e_ENGLAND , e_PEARL },
        { e_NON_PROFIT, e_CANADA  , e_SMITH },
        { e_NON_PROFIT, e_USA_EAST, e_TOAST },
        { e_REPEAT    , e_FRANCE  , e_TOAST },
        { e_NEED_BASED, e_FRANCE  , e_FAST  },
        { e_DISCOUNT  , e_MEXICO  , e_TOAST },
        { e_DISCOUNT  , e_FRANCE  , e_GREEN },
        { e_IMPULSE   , e_USA_EAST, e_FAST  },
        { e_REPEAT    , e_USA_EAST, e_GREEN },
        { e_NON_PROFIT, e_GERMANY , e_GREEN },
        { e_INSTITUTE , e_CANADA  , e_SMITH },
        { e_NEED_BASED, e_SCOTLAND, e_TOAST },
        { e_NEED_BASED, e_GERMANY , e_FAST  },
        { e_NON_PROFIT, e_RUSSIA  , e_TOAST },
        { e_BUSINESS  , e_ENGLAND , e_PEARL },
        { e_NEED_BASED, e_USA_EAST, e_TOAST },
        { e_INSTITUTE , e_USA_EAST, e_SMITH },
        { e_DISCOUNT  , e_USA_EAST, e_PEARL },
        { e_REPEAT    , e_SCOTLAND, e_FAST  },
        { e_IMPULSE   , e_GERMANY , e_TIDY  },
        { e_DISCOUNT  , e_CANADA  , e_TIDY  },
        { e_IMPULSE   , e_USA_EAST, e_TIDY  },
        { e_IMPULSE   , e_GERMANY , e_TIDY  },
        { e_NON_PROFIT, e_ENGLAND , e_FAST  },
        { e_NON_PROFIT, e_USA_WEST, e_TIDY  },
        { e_REPEAT    , e_MEXICO  , e_TOAST },
    };
    const bsl::size_t numCustomerProfiles = sizeof  customerProfiles
                                          / sizeof *customerProfiles;
//..
// Suppose, as the first step in our analysis, we wish to determine the number
// of unique combinations of customer attributes that exist in our data set.
// We can do that by inserting each data item into a flat hash set: the first
// insert of a combination will succeed, the others will fail, but at the end
// of the process, the set will contain one entry for every unique combination
// in our data.
//
// First, as there are no standard methods for hashing or comparing our
// user-defined types, we define 'CustomerProfileHash' and
// 'CustomerProfileEqual' classes, each a stateless functor.  Note that there
// is no meaningful ordering of the attribute values, they are merely arbitrary
// code numbers; nothing is lost by using an unordered set instead of an
// ordered set:
//..
    class CustomerProfileHash {
      public:
        // CREATORS
        //! CustomerProfileHash() = default;
            // Create a 'CustomerProfileHash' object.

        //! CustomerProfileHash(const CustomerProfileHash& original) = default;
            // Create a 'CustomerProfileHash' object.  Note that as
            // 'CustomerProfileHash' is an empty (stateless) type, this
            // operation has no observable effect.

        //! ~CustomerProfileHash() = default;
            // Destroy this object.

        // ACCESSORS
        bsl::size_t operator()(const CustomerProfile& x) const;
            // Return a hash value for the specified 'x'.
    };
//..
// The hash function combines the several enumerated values from the class
// (each a small 'int' value) into a single, unique 'int' value, and then
// applies the default hash function for 'int'.
//..
    // ACCESSORS
    bsl::size_t CustomerProfileHash::operator()(const CustomerProfile& x) const
    {
        return bsl::hash<int>()(  x.d_location * 100 * 100
                                + x.d_customer * 100
                                + x.d_project);
    }

    class CustomerProfileEqual {
      public:
        // CREATORS
        //! CustomerProfileEqual() = default;
            // Create a 'CustomerProfileEqual' object.

        //! CustomerProfileEqual(const CustomerProfileEqual& original)
        //!                                                          = default;
            // Create a 'CustomerProfileEqual' object.  Note that as
            // 'CustomerProfileEqual' is an empty (stateless) type, this
            // operation has no observable effect.

        //! ~CustomerProfileEqual() = default;
            // Destroy this object.

        // ACCESSORS
        bool operator()(const CustomerProfile& lhs,
                        const CustomerProfile& rhs) const;
            // Return 'true' if the specified 'lhs' has the same value as the
            // specified 'rhs', and 'false' otherwise.
    };

    // ACCESSORS
    bool CustomerProfileEqual::operator()(const CustomerProfile& lhs,
                                          const CustomerProfile& rhs) const
    {
        return lhs.d_location == rhs.d_location
            && lhs.d_customer == rhs.d_customer
            && lhs.d_project  == rhs.d_project;
    }
//..
// Notice that many of the required methods of the hash and comparator types
// are compiler generated.  (The declarations of those methods are commented
// out and suffixed by an '= default' comment.)
//
// Then, we define the type of the flat hash set:
//..
    typedef bdlc::FlatHashSet<CustomerProfile,
                              CustomerProfileHash,
                              CustomerProfileEqual> ProfileCategories;
//..

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
      case 28: {
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

// Next, we create a flat hash set and insert each item of 'customerProfiles':
//..
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    ProfileCategories profileCategories(&oa);

    for (bsl::size_t idx = 0; idx < numCustomerProfiles; ++idx) {
       profileCategories.insert(customerProfiles[idx]);
    }

    ASSERT(numCustomerProfiles >= profileCategories.size());
//..
// Notice that we ignore the status returned by the 'insert' method.  We fully
// expect some operations to fail.
//
// Finally, the size of 'profileCategories' matches the number of unique
// customer profiles in this data set:
//..
    if (verbose) {
        bsl::cout << numCustomerProfiles << ' ' << profileCategories.size()
                  << bsl::endl;
    }
//..
// Standard output shows:
//..
//  100 84
//..
      } break;
      case 27: {
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
        //   DRQS 169531176: 'bsl::inserter' usage on Sun
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INSERTER" << endl
                          << "========" << endl;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        typedef bdlc::FlatHashSet<int> Obj;
        Obj mX(&oa); const Obj& X = mX;

        bsl::fill_n(bsl::inserter(mX, mX.begin()), 5, 2);
        ASSERTV(X.size(), 1 == X.size());

      } break;
      case 26: {
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
        //   iterator insert(const_iterator, FORWARD_REF(ENTRY_TYPE) entry)
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
      case 25: {
        // --------------------------------------------------------------------
        // DRQS 165258625: 'insert' could create reference to temporary
        //
        // Concerns:
        //: 1 The 'insert' method does not access a value after it goes out of
        //:   scope.  When the argument to 'insert' is convertible (but not
        //:   equivalent) to the type held in the container, a reference to a
        //:   temporary value was created.  This reference was accessed after
        //:   the temporary value went out of scope.
        //
        // Plan:
        //: 1 In ASAN builds, verify the following code does not produce a
        //    "stack-use-after-scope" error.  (C-1)
        //
        // Testing:
        //   DRQS 165258625: 'insert' could create reference to temporary
        // --------------------------------------------------------------------

        if (verbose) {
// ---------^
cout << endl
     << "DRQS 165258625: 'insert' could create reference to temporary" << endl
     << "============================================================" << endl;
// ---------v
        }

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        bdlc::FlatHashSet<bsl::string>        mX(&oa);
        const bdlc::FlatHashSet<bsl::string>& X = mX;

        mX.insert("-1");
        ASSERT(X.begin() == X.find(bsl::string("-1")));
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TYPE TRAITS
        //   Ensure 'bdlc::FlatHashSet' has the expected type traits.
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
        //   CONCERN: 'FlatHashSet' has the necessary type traits
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TYPE TRAITS" << endl
                          << "===========" << endl;

        typedef bdlc::FlatHashSet<int> Obj;

        BSLMF_ASSERT(1 == bslalg::HasStlIterators<Obj>::value);
        BSLMF_ASSERT(1 == bslma::UsesBslmaAllocator<Obj>::value);

        BSLMF_ASSERT(0 == bslmf::IsBitwiseMoveable<Obj>::value);
        BSLMF_ASSERT(0 == bsl::is_trivially_copyable<Obj>::value);
        BSLMF_ASSERT(0 == bslmf::IsBitwiseEqualityComparable<Obj>::value);
        BSLMF_ASSERT(0 == bslmf::HasPointerSemantics<Obj>::value);
        BSLMF_ASSERT(0 == bsl::is_trivially_default_constructible<Obj>::value);
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
        //   void insert(bsl::initializer_list<KEY> values);
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
            typedef int                    Key;
            typedef bdlc::FlatHashSet<Key> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert({ 1 });

            ASSERT(    1 == X.size());
            ASSERT( true == X.contains(1));
            ASSERT(false == X.contains(2));
            ASSERT(false == X.contains(3));

            mX.insert({ 2, 3 });

            ASSERT(   3 == X.size());
            ASSERT(true == X.contains(1));
            ASSERT(true == X.contains(2));
            ASSERT(true == X.contains(3));
        }
        {
            typedef bsl::string            Key;
            typedef bdlc::FlatHashSet<Key> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&oa);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert({ SA });

            ASSERT(    1 == X.size());
            ASSERT( true == X.contains(SA));
            ASSERT(false == X.contains(SB));
            ASSERT(false == X.contains(SC));

            mX.insert({ SB, SC });

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
        //   FlatHashSet& operator=(bsl::initializer_list<KEY> values);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INITIALIZER LIST ASSIGNMENT OPERATOR" << endl
                          << "====================================" << endl;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

        const bsl::string SA("abc",    &ta);
        const bsl::string SB("abcdef", &ta);
        const bsl::string SC("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU",
                             &ta);

        {
            typedef int                                 Key;
            typedef SeedIsHash<Key>                     Hash;
            typedef EqualAndState<Key>                  Equal;
            typedef bdlc::FlatHashSet<Key, Hash, Equal> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Hash  hasher(5);
            Equal equal(11);

            Obj mX(0, hasher, equal, &oa);  const Obj& X = mX;

            mX = { 1, 2, 3 };

            ASSERT(   3 == X.size());
            ASSERT(true == X.contains(1));
            ASSERT(true == X.contains(2));
            ASSERT(true == X.contains(3));
            ASSERT(   5 == X.hash_function()(2));
            ASSERT(  11 == X.key_eq().state());

            mX = { 1, 2 };

            ASSERT(    2 == X.size());
            ASSERT( true == X.contains(1));
            ASSERT( true == X.contains(2));
            ASSERT(false == X.contains(3));
            ASSERT(    5 == X.hash_function()(3));
            ASSERT(   11 == X.key_eq().state());

            mX = { 2 };

            ASSERT(    1 == X.size());
            ASSERT(false == X.contains(1));
            ASSERT( true == X.contains(2));
            ASSERT(false == X.contains(3));
            ASSERT(    5 == X.hash_function()(5));
            ASSERT(   11 == X.key_eq().state());
        }
        {
            typedef bsl::string                         Key;
            typedef SeedIsHash<Key>                     Hash;
            typedef EqualAndState<Key>                  Equal;
            typedef bdlc::FlatHashSet<Key, Hash, Equal> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&oa);

            Hash  hasher(7);
            Equal equal(SB);

            Obj mX(0, hasher, equal, &oa);  const Obj& X = mX;

            mX = { SA };

            ASSERT(    1 == X.size());
            ASSERT( true == X.contains(SA));
            ASSERT(false == X.contains(SB));
            ASSERT(false == X.contains(SC));
            ASSERT(    7 == X.hash_function()(SA));
            ASSERT(   SB == X.key_eq().state());

            mX = { SA, SB };

            ASSERT(    2 == X.size());
            ASSERT( true == X.contains(SA));
            ASSERT( true == X.contains(SB));
            ASSERT(false == X.contains(SC));
            ASSERT(    7 == X.hash_function()(SB));
            ASSERT(   SB == X.key_eq().state());

            mX = { SA, SB, SC };

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
        //   FlatHashSet(bsl::initializer_list<KEY> values, Allocator * = 0);
        //   FlatHashSet(bsl::initializer_list<KEY>, size_t, Allocator * = 0);
        //   FlatHashSet(init_list<KEY>, size_t, const HASH&, Allocator * = 0);
        //   FlatHashSet(init_list<K>, size_t, const H&, const EQ&, A * = 0);
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

        bsl::initializer_list<int>         IDATA = { 1 };
        bsl::initializer_list<bsl::string> SDATA = { SA };

        if (verbose) cout << "Testing constructors." << endl;
        {
            // FlatHashSet(init);

            {
                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
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

            // FlatHashSet(init, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
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

            // FlatHashSet(init, size_t capacity);

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
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

            // FlatHashSet(init, size_t capacity, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
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

            // FlatHashSet(init, size_t, const HASH&, Allocator *bA = 0);

            {
                typedef int                          Key;
                typedef SeedIsHash<Key>              Hash;
                typedef bdlc::FlatHashSet<Key, Hash> Obj;

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
                typedef bsl::string                  Key;
                typedef SeedIsHash<Key>              Hash;
                typedef bdlc::FlatHashSet<Key, Hash> Obj;

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

            // FlatHashSet(init, size_t, const HASH&, const EQUAL&, A * = 0);

            {
                typedef int                                 Key;
                typedef SeedIsHash<Key>                     Hash;
                typedef bsl::equal_to<Key>                  Equal;
                typedef bdlc::FlatHashSet<Key, Hash, Equal> Obj;

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
                typedef bsl::string                         Key;
                typedef SeedIsHash<Key>                     Hash;
                typedef bsl::less<Key>                      Equal;
                typedef bdlc::FlatHashSet<Key, Hash, Equal> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Hash hasher(9);

                Obj mX(SDATA, 64, hasher, Equal(), &oa);  const Obj& X = mX;

                ASSERT(   64 == X.capacity());
                ASSERT(    1 == X.size());
                // Due to comparison operator, cannot test with 'contains'.
                ASSERT(   SA == *X.begin());
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
        //   FlatHashSet(INPUT_ITERATOR, INPUT_ITERATOR, Allocator *bA = 0);
        //   FlatHashSet(INPUT_ITER, INPUT_ITER, size_t, Allocator * = 0);
        //   FlatHashSet(II, II, size_t, const HASH&, Allocator * = 0);
        //   FlatHashSet(II, II, size_t, const H&, const EQ&, Alloc * = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ITERATOR VALUE CONSTRUCTORS" << endl
                          << "===========================" << endl;

        bslma::TestAllocator ta("test allocator", veryVeryVeryVerbose);

        const bsl::string SA("abc",    &ta);
        const bsl::string SB("abcdef", &ta);
        const bsl::string SC("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU",
                             &ta);

        bsl::vector<int> IDATA(&ta);
        {
            IDATA.push_back(1);
            IDATA.push_back(2);
        }

        bsl::vector<bsl::string> SDATA(&ta);
        {
            SDATA.push_back(SA);
            SDATA.push_back(SB);
        }

        if (verbose) cout << "Testing constructors." << endl;
        {
            // FlatHashSet(II, II);

            {
                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                Obj        mX(IDATA.begin(), IDATA.begin() + 1);
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                Obj        mX(SDATA.begin(), SDATA.begin() + 1);
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

            // FlatHashSet(II, II, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mX(IDATA.begin(),
                              IDATA.begin() + 1,
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj        mX(SDATA.begin(), SDATA.begin() + 1, &oa);
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

            // FlatHashSet(II, II, size_t capacity);

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mX(IDATA.begin(), IDATA.begin() + 1, 32);
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mX(SDATA.begin(), SDATA.begin() + 1, 64);
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

            // FlatHashSet(II, II, size_t capacity, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mX(IDATA.begin(),
                              IDATA.begin() + 1,
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
                typedef bslh::FibonacciBadHashWrapper<
                                                 bsl::hash<Key> > ExpHash;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj        mX(SDATA.begin(), SDATA.begin() + 1, 64, &oa);
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

            // FlatHashSet(II, II, size_t, const HASH&, Allocator *bA = 0);

            {
                typedef int                          Key;
                typedef SeedIsHash<Key>              Hash;
                typedef bdlc::FlatHashSet<Key, Hash> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Hash hasher(7);

                Obj        mX(IDATA.begin(), IDATA.begin() + 1, 32, hasher);
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
                typedef bsl::string                  Key;
                typedef SeedIsHash<Key>              Hash;
                typedef bdlc::FlatHashSet<Key, Hash> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Hash hasher(9);

                Obj        mX(SDATA.begin(),
                              SDATA.begin() + 1,
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

            // FlatHashSet(II, II, size_t, const HASH&, const EQUAL&, A * = 0);

            {
                typedef int                                 Key;
                typedef SeedIsHash<Key>                     Hash;
                typedef bsl::equal_to<Key>                  Equal;
                typedef bdlc::FlatHashSet<Key, Hash, Equal> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Hash hasher(7);

                Obj        mX(IDATA.begin(),
                              IDATA.begin() + 1,
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
                typedef bsl::string                         Key;
                typedef SeedIsHash<Key>                     Hash;
                typedef bsl::less<Key>                      Equal;
                typedef bdlc::FlatHashSet<Key, Hash, Equal> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Hash hasher(9);

                Obj        mX(SDATA.begin(),
                              SDATA.begin() + 1,
                              64,
                              hasher,
                              Equal(),
                              &oa);
                const Obj& X = mX;

                ASSERT(   64 == X.capacity());
                ASSERT(    1 == X.size());
                // Due to comparison operator, cannot test with 'contains'.
                ASSERT(   SA == *X.begin());
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
        //
        // Ensure the 'rehash' and 'reserve' methods operate as expected.
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
            typedef bdlc::FlatHashSet<int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.rehash(32);

            ASSERT(32 == X.capacity());

            mX.rehash(64);

            ASSERT(64 == X.capacity());
        }

        if (verbose) cout << "Testing 'reserve'." << endl;
        {
            typedef bdlc::FlatHashSet<int> Obj;

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
        //   const_iterator erase(const_iterator, const_iterator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RANGE 'erase'" << endl
                          << "=============" << endl;

        if (verbose) cout << "Testing range 'erase'." << endl;
        {
            typedef bdlc::FlatHashSet<int, SeedIsHash<int> > Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(1);

            ASSERT(X.begin() == mX.erase(mX.begin(), mX.begin()));
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
        //   const_iterator erase(const_iterator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ITERATOR 'erase'" << endl
                          << "================" << endl;

        if (verbose) cout << "Testing iterator 'erase'." << endl;
        {
            typedef bdlc::FlatHashSet<int, SeedIsHash<int> > Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(1);
            mX.insert(2);

            ASSERT(2 == *mX.erase(X.find(1)));
            ASSERT(1 == X.size());

            ASSERT(X.end() == mX.erase(X.find(2)));
            ASSERT(      0 == X.size());
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            typedef bdlc::FlatHashSet<int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(1);

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
            typedef bdlc::FlatHashSet<int, SeedIsHash<int> > Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mZ(&oa);  const Obj& Z = mZ;

            mZ.insert(1);

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
        //:   any target object being set to that of any source object.
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
        //   FlatHashSet& operator=(FlatHashSet&&);
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
        //: 1 Specify a set 'S' of two distinct object values to be used
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
        //   FlatHashSet(FlatHashSet&&, Allocator *basicAllocator);
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
        //: 1 Specify a set 'S' of two distinct object values to be used
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
        //   FlatHashSet(FlatHashSet&&);
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
        //: 3 The accessors 'contains', 'count', and 'equal_range' correctly
        //:   forward to the implementation object and correctly forward the
        //:   return value.
        //
        // Plan:
        //: 1 Using the basic accessors, verify the 'begin' methods return an
        //:   iterator referencing the first populated entry and incrementing
        //:   the 'begin' iterator 'size()' times visits all contained elements
        //:   and results in the 'end' iterator.  (C-1,2)
        //:
        //: 2 Directly verify the result of 'contains', 'count', and
        //:   'equal_range' when applied to various object values.  (C-3)
        //
        // Testing:
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

        typedef bdlc::FlatHashSet<int, SeedIsHash<int> > Obj;

        if (verbose) cout << "Testing iterator methods." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            ASSERT(X.end() == X.begin());
            ASSERT(X.end() == X.cbegin());
            ASSERT(X.end() == X.cend());

            mX.insert(1);

            Obj::const_iterator iter1 = X.begin();
            Obj::const_iterator iter2 = X.cbegin();

            ASSERT(1 == *iter1);
            ASSERT(1 == *iter2);

            ++iter1;
            ++iter2;

            ASSERT(X.end() == iter1);
            ASSERT(X.end() == iter2);

            mX.insert(2);

            iter1 = X.begin();
            iter2 = X.cbegin();

            ASSERT(1 == *iter1);
            ASSERT(1 == *iter2);

            ++iter1;
            ++iter2;

            ASSERT(2 == *iter1);
            ASSERT(2 == *iter2);

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

            mX.insert(1);
            mX.insert(3);

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

            ASSERT(  1 == *(rv.first));
            ASSERT(exp ==  rv.second);

            rv = X.equal_range(2);

            ASSERT(X.end() == rv.first);
            ASSERT(X.end() == rv.second);

            rv = X.equal_range(3);

            exp = rv.first;
            ++exp;

            ASSERT(  3 == *(rv.first));
            ASSERT(exp ==  rv.second);
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'empty' AND 'load_factor'
        //
        // Ensure the non-basic accessors and iterators operate as expected.
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

        typedef bdlc::FlatHashSet<int> Obj;

        if (verbose) cout << "Testing 'empty' and 'load_factor'." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            bsls::Types::Int64 expNumAllocations = oa.numAllocations();

            ASSERT(X.empty());

            ASSERT(0.0f == X.load_factor());

            ASSERT(expNumAllocations == oa.numAllocations());

            for (int i = 0; i < 100; ++i) {
                mX.insert(i);

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
        //   FlatHashSet& operator=(const FlatHashSet&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        typedef bdlc::FlatHashSet<int> Obj;

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
            mY.insert(1);

            Obj mZ(&oa);  const Obj& Z = mZ;
            mZ.insert(2);

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
        //   void swap(FlatHashSet&);
        //   void swap(FlatHashSet&, FlatHashSet&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTION" << endl
                          << "=============================" << endl;

        typedef bdlc::FlatHashSet<int> Obj;

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

            mW.insert(1);
            mZ.insert(2);

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

            mW.insert(1);
            mZ.insert(2);

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

            mY.insert(1);

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
        //   FlatHashSet(const FlatHashSet&, Allocator *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "Testing the copy constructor." << endl;
        {
            typedef bdlc::FlatHashSet<int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mW(&oa);  const Obj& W = mW;
            Obj mZ(&oa);  const Obj& Z = mZ;

            mW.insert(1);
            mZ.insert(2);

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
        //   bool operator==(const FlatHashSet&, const FlatHashSet&);
        //   bool operator!=(const FlatHashSet&, const FlatHashSet&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY OPERATORS" << endl
                          << "==================" << endl;

        if (verbose) cout << "Verify the equality operators." << endl;
        {
            typedef bdlc::FlatHashSet<int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            Obj mY(&oa);  const Obj& Y = mY;

            mY.insert(1);

            Obj mZ(&oa);  const Obj& Z = mZ;

            mZ.insert(1);
            mZ.insert(2);

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
        //   ostream& operator<<(ostream& stream, const FlatHashSet& set);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        typedef bdlc::FlatHashSet<int, SeedIsHash<int> > Obj;

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
                                          "0"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   1,  {0},        "["                                NL
                                          " 0"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  1,  {0},        "["                                SP
                                          "0"                                SP
                                          "]"                                },
        { L_,   0,   -8,  1,  {0},        "["                                NL
                                          "    0"                            NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   0,   3,  {0,1,-1},   "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "-1"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   3,  {0,1,-1},   "["                                NL
                                          " 0"                               NL
                                          " 1"                               NL
                                          " -1"                              NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  3,  {0,1,-1},   "["                                SP
                                          "0"                                SP
                                          "1"                                SP
                                          "-1"                               SP
                                          "]"                                },
        { L_,   0,   -8,  3,  {0,1,-1},   "["                                NL
                                          "    0"                            NL
                                          "    1"                            NL
                                          "    -1"                           NL
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
                                          "0"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   1,  {0},        "      ["                          NL
                                          "        0"                        NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  1,  {0},        "      ["                          SP
                                          "0"                                SP
                                          "]"                                },
        { L_,   3,   -8,  1,  {0},        "            ["                    NL
                                          "                0"                NL
                                          "            ]"                    NL
                                                                             },
        { L_,   3,   0,   3,  {0,1,-1},   "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "-1"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   3,  {0,1,-1},   "      ["                          NL
                                          "        0"                        NL
                                          "        1"                        NL
                                          "        -1"                       NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  3,  {0,1,-1},   "      ["                          SP
                                          "0"                                SP
                                          "1"                                SP
                                          "-1"                               SP
                                          "]"                                },
        { L_,   3,   -8,  3,  {0,1,-1},   "            ["                    NL
                                          "                0"                NL
                                          "                1"                NL
                                          "                -1"               NL
                                          "            ]"                    NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 } x { 3 } --> 1 expected o/p
        // -----------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,  2,  3,   3,  {0,1,-1},     "      ["                          NL
                                          "         0"                       NL
                                          "         1"                       NL
                                          "         -1"                      NL
                                          "      ]"                          NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 } x { -8 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,  -8,  -8,  1,   {0},        "["                                NL
                                          "    0"                            NL
                                          "]"                                NL
                                                                             },
        { L_,  -8,  -8,  3,   {0,1,-1},   "["                                NL
                                          "    0"                            NL
                                          "    1"                            NL
                                          "    -1"                           NL
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

                    mX.insert(ARRAY[i]);
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
                typedef SeedIsHash<int>                     Hash;
                typedef bsl::equal_to<int>                  Equal;
                typedef bdlc::FlatHashSet<int, Hash, Equal> Obj;

                Obj mX(0, Hash(), Equal());  const Obj& X = mX;

                ASSERT(                0 == X.capacity());
                ASSERT(                0 == X.hash_function()(0));
                ASSERT(             true == X.key_eq()(0, 0));
                ASSERT(            false == X.key_eq()(0, 1));
                ASSERT(            0.875 == X.max_load_factor());
                ASSERT(&defaultAllocator == X.allocator());
            }
            {
                typedef SeedIsHash<int>                     Hash;
                typedef bsl::less<int>                      Equal;
                typedef bdlc::FlatHashSet<int, Hash, Equal> Obj;

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
            typedef bdlc::FlatHashSet<int> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            ASSERT(      0 == X.size());
            ASSERT(X.end() == X.find(1));
            ASSERT(X.end() == X.find(2));
            ASSERT(X.end() == X.find(3));

            mX.insert(1);

            ASSERT(      1 ==  X.size());
            ASSERT(      1 == *X.find(1));
            ASSERT(X.end() ==  X.find(2));
            ASSERT(X.end() ==  X.find(3));

            mX.insert(2);

            ASSERT(      2 ==  X.size());
            ASSERT(      1 == *X.find(1));
            ASSERT(      2 == *X.find(2));
            ASSERT(X.end() ==  X.find(3));
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
        //   FlatHashSet();
        //   FlatHashSet(Allocator *basicAllocator);
        //   FlatHashSet(size_t capacity);
        //   FlatHashSet(size_t capacity, Allocator *basicAllocator);
        //   FlatHashSet(size_t capacity, const HASH&, Allocator *bA = 0);
        //   FlatHashSet(size_t, const HASH&, const EQUAL&, Allocator * = 0);
        //   ~FlatHashSet();
        //   void clear();
        //   size_t erase(const KEY&);
        //   bsl::pair<iterator, bool> insert(FORWARD_REF(ENTRY_TYPE) entry)
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
            // FlatHashSet();

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
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

            // FlatHashSet(Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
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

            // FlatHashSet(size_t capacity);

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
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

            // FlatHashSet(size_t capacity, Allocator *basicAllocator);

            {
                typedef int                                       Key;
                typedef bdlc::FlatHashSet<Key>                    Obj;
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
                typedef bdlc::FlatHashSet<Key>                    Obj;
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

            // FlatHashSet(size_t capacity, const HASH&, Allocator *bA = 0);

            {
                typedef int                          Key;
                typedef SeedIsHash<Key>              Hash;
                typedef bdlc::FlatHashSet<Key, Hash> Obj;

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
                typedef bsl::string                  Key;
                typedef SeedIsHash<Key>              Hash;
                typedef bdlc::FlatHashSet<Key, Hash> Obj;

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

            // FlatHashSet(size_t, const HASH&, const EQUAL&, Allocator * = 0);

            {
                typedef int                                 Key;
                typedef SeedIsHash<Key>                     Hash;
                typedef bsl::equal_to<Key>                  Equal;
                typedef bdlc::FlatHashSet<Key, Hash, Equal> Obj;

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
                typedef bsl::string                         Key;
                typedef SeedIsHash<Key>                     Hash;
                typedef bsl::less<Key>                      Equal;
                typedef bdlc::FlatHashSet<Key, Hash, Equal> Obj;

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
            typedef int                                       Key;
            typedef bdlc::FlatHashSet<Key>                    Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;

            mX.insert(1);
            mX.insert(2);
            mX.insert(3);

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
            typedef int                                       Key;
            typedef bdlc::FlatHashSet<Key>                    Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(32, &oa);  const Obj& X = mX;

            mX.insert(1);
            mX.insert(2);
            mX.insert(3);

            mX.clear();

            ASSERT( 0 == X.size());
            ASSERT(32 == X.capacity());

            mX.insert(1);
            mX.insert(2);

            mX.clear();

            ASSERT( 0 == X.size());
            ASSERT(32 == X.capacity());
        }

        if (verbose) cout << "Testing 'reset'." << endl;
        {
            typedef int                                       Key;
            typedef bdlc::FlatHashSet<Key>                    Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(32, &oa);  const Obj& X = mX;

            mX.insert(1);
            mX.insert(2);
            mX.insert(3);

            mX.reset();

            ASSERT(0 == X.size());
            ASSERT(0 == X.capacity());

            mX.insert(1);
            mX.insert(2);

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

        typedef bdlc::FlatHashSet<int> Obj;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;

        ASSERT(       0 == X.size());
        ASSERT( X.end() == X.find(0));

        {
            bsl::pair<Obj::iterator, bool> rv = mX.insert(0);

            ASSERT(       1 ==  X.size());
            ASSERT(    true ==  rv.second);
            ASSERT( X.end() !=  rv.first);
            ASSERT(       0 == *rv.first);
            ASSERT(rv.first ==  X.find(0));
            ASSERT( X.end() ==  X.find(1));
        }

        Obj mY(X, &oa); const Obj& Y = mY;

        ASSERT(      1 ==  Y.size());
        ASSERT(      0 == *Y.find(0));
        ASSERT(Y.end() ==  Y.find(1));
        ASSERT(      X ==  Y);
        ASSERT(      Y ==  X);

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
            bsl::pair<Obj::iterator, bool> rv = mY.insert(1);

            ASSERT(       1 ==  Y.size());
            ASSERT(    true ==  rv.second);
            ASSERT( Y.end() !=  rv.first);
            ASSERT(       1 == *rv.first);
            ASSERT( Y.end() ==  Y.find(0));
            ASSERT(rv.first ==  Y.find(1));
            ASSERT(       X !=  Y);
            ASSERT(       Y !=  X);
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //    Verify 'bdlc::FlatHashSet' can outperform 'bsl::unordered_set'.
        //
        // Concerns:
        //: 1 'bdlc::FlatHashSet' can outperform 'bsl::unordered_set' when
        //:   'find' is used for values present in the set.
        //:
        //: 2 'bdlc::FlatHashSet' can outperform 'bsl::unordered_set' when
        //:   'find' is used for values not present in the set.
        //
        // Plan:
        //: 1 Perform a performance test for 'find' with values in the set
        //:   and verify 'bdlc::FlatHashSet' outperforms 'bsl::unordered_set'.
        //:   (C-1)
        //:
        //: 2 Perform a performance test for 'find' with values not in the set
        //:   and verify 'bdlc::FlatHashSet' outperforms 'bsl::unordered_set'.
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
            bdlc::FlatHashSet<int>  mX;
            bsl::unordered_set<int> mY;

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
            bdlc::FlatHashSet<int>  mX;
            bsl::unordered_set<int> mY;

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
