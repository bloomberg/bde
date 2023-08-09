// bdlc_flathashtable.t.cpp                                           -*-C++-*-

#include <bdlc_flathashtable.h>
#include <bdlc_flathashtable_groupcontrol.h>

#include <bdlb_bitutil.h>

#include <bsla_fallthrough.h>

#include <bslh_hash.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
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

#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//                              Overview
//                              --------
// The object under test is a container implementing a flat hash table.  The
// general concerns are correctness, exception safety, and proper dispatching
// (for member function templates such as insert).  This container is
// implemented in the form of a class template, and thus its proper
// instantiation for several types is a concern.
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
//: o 'controls'
//: o 'entries'
//: o 'hash_function'
//: o 'key_eq'
//: o 'max_load_factor'
//: o 'size'
//
// Certain standard value-semantic-type test cases are omitted:
//: o [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//: o [ 5] ostream& operator<<(ostream& stream, const FlatHashTable& table);
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
// [ 2] FlatHashTable(size_t, const HASH&, const EQUAL&, Allocator * = 0);
// [ 7] FlatHashTable(const FlatHashTable&, Allocator *bA = 0);
// [13] FlatHashTable(FlatHashTable&&);
// [14] FlatHashTable(FlatHashTable&&, Allocator *basicAllocator);
// [ 2] ~FlatHashTable();
//
// MANIPULATORS
// [ 9] FlatHashTable& operator=(const FlatHashTable&);
// [15] FlatHashTable& operator=(FlatHashTable&&);
// [20] ENTRY& operator[](FORWARD_REF(KEY_TYPE) key);
// [ 2] void clear();
// [12] bsl::pair<iterator, iterator> equal_range(const KEY&);
// [ 2] size_t erase(const KEY&);
// [17] iterator erase(const_iterator);
// [17] iterator erase(iterator);
// [18] iterator erase(const_iterator, const_iterator);
// [12] iterator find(const KEY&);
// [ 2] bsl::pair<iterator, bool> insert(FORWARD_REF(ENTRY_TYPE) entry)
// [16] void insert(INPUT_IT, INPUT_IT);
// [19] void rehash(size_t);
// [19] void reserve(size_t);
// [ 2] void reset();
//
// [12] iterator begin();
// [12] iterator end();
//
// [ 8] void swap(FlatHashTable&);
//
// ACCESSORS
// [ 4] size_t capacity() const;
// [12] bool contains(const KEY&) const;
// [ 4] const bsl::uint8_t *controls() const;
// [12] bsl::size_t count(const KEY& key) const;
// [11] bool empty() const;
// [ 4] const ENTRY *entries() const;
// [12] bsl::pair<ci, ci> equal_range(const KEY&) const;
// [12] const_iterator find(const KEY&) const;
// [ 4] HASH hash_function() const;
// [ 4] EQUAL key_eq() const;
// [11] float load_factor() const;
// [ 4] float max_load_factor() const;
// [ 4] size_t size() const;
//
// [12] const_iterator begin() const;
// [12] const_iterator cbegin() const;
// [12] const_iterator cend() const;
// [12] const_iterator end() const;
//
// [ 4] Allocator *allocator() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const FlatHashTable&, const FlatHashTable&);
// [ 6] bool operator!=(const FlatHashTable&, const FlatHashTable&);
//
// FREE FUNCTIONS
// [ 8] void swap(FlatHashTable&, FlatHashTable&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] FlatHashTable& gg(FlatHashTable *object, const char *spec);
// [ 3] int ggg(FlatHashTable *object, const char *spec, int verboseFlag);
// [ 2] CONCERN: 'isValid' works as expected
// [12] CONCERN: 'FHTCI FHTCI::operator++()'
// [12] CONCERN: 'const ENTRY& FHTCI::operator*()'
// [12] CONCERN: 'bool operator==(FHTCI&, FHTCI&)'
// [12] CONCERN: 'ENTRY& FHTI::operator*()'
// [21] CONCERN: {DRQS 167125039} BASIC OPERATIONS OF MOVED-TO TABLES

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

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

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

typedef bdlc::FlatHashTable_GroupControl GroupControl;

const bsl::uint8_t k_SIZE   = GroupControl::k_SIZE;
const bsl::uint8_t k_EMPTY  = GroupControl::k_EMPTY;
const bsl::uint8_t k_ERASED = GroupControl::k_ERASED;

// ============================================================================
//                     GLOBAL VARIABLES FOR TESTING
// ----------------------------------------------------------------------------

bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define 'DEFAULT_DATA' used by test cases.

struct DefaultDataRow {
    int         d_lineNum;  // source line number

    const char *d_spec_p;   // specification string

    int         d_valueId;  // value identifier (two values are equal if and
                            // only if their 'd_valueId' are equal)
};

static const DefaultDataRow DEFAULT_DATA[] = {
// -^
//LN                              spec                                    VI
//--  ------------------------------------------------------------------  --
{ L_, ""                                                                ,  0 },

{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  0 },
{ L_, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"                                ,  0 },
{ L_, "Aeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  1 },
{ L_, "eeeeeeeeeeeeeeeeAeeeeeeeeeeeeeee"                                ,  2 },
{ L_, "xAeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  1 },
{ L_, "eeeeeeeeeeeeeeeexAeeeeeeeeeeeeee"                                ,  2 },
{ L_, "xxAeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  1 },
{ L_, "eeeeeeeeeeeeeeeexxAeeeeeeeeeeeee"                                ,  2 },
{ L_, "ABeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  3 },
{ L_, "xABeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  3 },
{ L_, "xxABeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  3 },
{ L_, "xxBAeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  3 },
{ L_, "eeeeeeeeeeeeeeeexxABeeeeeeeeeeee"                                ,  4 },
{ L_, "eeeeeeeeeeeeeeeexxBAeeeeeeeeeeee"                                ,  4 },
{ L_, "eeeeeeeeeeeeeeeexxxxxxxxCDxxxxxx"                                , 12 },
{ L_, "eeeeeeeeeeeeeeeexxxxxxxxxxxxxxCD"                                , 12 },
{ L_, "xxABCeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  9 },
{ L_, "xxBACeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  9 },
{ L_, "eeeeeeeeeeeeeeeexxABCeeeeeeeeeee"                                , 11 },
{ L_, "eeeeeeeeeeeeeeeexxBACeeeeeeeeeee"                                , 11 },
{ L_, "eeeeeeeeeeeeeeeexxxxxxxxCBDeeeee"                                , 13 },
{ L_, "eeeeeeeeeeeeeeeexxxxxxxxxxxxxCBD"                                , 13 },
{ L_, "xxxxxxxAxxxxxxxxxxxxxxxxBxxxxxxx"                                ,  5 },

{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  0 },
{ L_, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",  0 },
{ L_, "Aeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  1 },
{ L_, "eeeeeeeeeeeeeeeeAeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  6 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeAeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  2 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeAeeeeeeeeeeeeeee",  7 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexAeeeeeeeeeeeeee",  7 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexxAeeeeeeeeeeeee",  7 },
{ L_, "ABeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  3 },
{ L_, "xABeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  3 },
{ L_, "xxABeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  3 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeABeeeeeeeeeeeeee",  8 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexABeeeeeeeeeeeee",  8 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexxABeeeeeeeeeeee",  8 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexxxxxxxxCDxxxxxx", 14 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexxxxxxxxxxxxxxCD", 14 },
{ L_, "ABCeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  9 },
{ L_, "xABCeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  9 },
{ L_, "xxABCeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  9 },
{ L_, "BACeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  9 },
{ L_, "xBACeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  9 },
{ L_, "xxBACeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  9 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeABCeeeeeeeeeeeee", 10 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexABCeeeeeeeeeeee", 10 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexxABCeeeeeeeeeee", 10 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexxxxxxxxADCeeeee", 15 },
{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeexxxxxxxxxxxxxADC", 15 },
// -v
};

const bsl::size_t DEFAULT_NUM_DATA = sizeof  DEFAULT_DATA
                                   / sizeof *DEFAULT_DATA;

// ============================================================================
//                  GLOBAL CLASSES/STRUCTS FOR TESTING
// ----------------------------------------------------------------------------

                            // ==================
                            // struct IntZeroHash
                            // ==================

class IntZeroHash {
    // This class provides a hash algorithm that always returns zero, useful
    // for testing various methods of a flat hash table.

  public:
    // ACCESSORS
    bsl::size_t operator()(const int) const
        // Return zero.
    {
        return 0;
    }
};

                           // ====================
                           // class IntValueIsHash
                           // ====================

class IntValueIsHash {
    // This class provides a hash algorithm that provides a simple mapping from
    // key to hash, useful for testing various methods of a flat hash table.

  public:
    bsl::size_t operator()(const int key) const
        // Return a value having the same seven lowest-order bits as the
        // specified 'key' and the highest-order byte the same as the second
        // lowest-order byte as the 'key'.  All other bits in 'key' are
        // ignored.
    {
        bsl::size_t k = static_cast<bsl::size_t>(key);

        return ((k & 0xff00) << (sizeof(bsl::size_t) * 8 - 16)) | (k & 0x7f);
    }
};

                             // ================
                             // class SeedIsHash
                             // ================

class SeedIsHash {
    // This class provides a hash algorithm that returns the specified seed
    // value for all hash requests, useful for testing 'FlatHashTable_Hash'.

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
    bsl::size_t operator()(const int&) const
        // Return the provided-at-constuction seed value.
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
    // 'KEY' to hash, useful for testing various methods of a flat hash table.

  public:
    // ACCESSORS
    bsl::size_t operator()(const KEY& key) const
        // Return a value having the same seven lowest-order bits as the
        // specified 'key.data()' and the highest-order byte the same as the
        // second lowest-order byte as the 'key.data()'.  All other bits in
        // 'key' are ignored.
    {
        bsl::size_t k = static_cast<bsl::size_t>(key.data());

        return ((k & 0xff00) << (sizeof(bsl::size_t) * 8 - 16)) | (k & 0x7f);
    }
};

                           // ====================
                           // struct TestEntryUtil
                           // ====================

template <class ENTRY>
struct TestEntryUtil
    // This templated utility provides methods to constuct an 'ENTRY' having a
    // specified integer key, and a method to extract the key from an 'ENTRY'.
{
    // CLASS METHODS
    static void construct(ENTRY             *entry,
                          bslma::Allocator  *allocator,
                          int                key)
        // Load into the specified 'entry' the specified 'key' value, using the
        // specified 'allocator' to supply memory.
    {
        bslma::ConstructionUtil::construct(entry, allocator, key);
    }

    static int key(const ENTRY& entry)
        // Return the key of the specified 'entry'.
    {
        return entry.data();
    }
};

template <>
struct TestEntryUtil<int>
    // This utility provides methods to assign the integer value of an entry,
    // and a method to extract the value from an entry.
{
    // CLASS METHODS
    static void construct(int               *entry,
                          bslma::Allocator  *,
                          int                key)
        // Load into the specified 'entry' the specified 'key' value.
    {
        *entry = key;
    }

    static int key(const int& entry)
        // Return the key of the specified 'entry'.
    {
        return entry;
    }
};

template <>
struct TestEntryUtil<bsl::pair<const int, int> >
    // This utility provides methods to constuct a 'bsl::pair<const int, int>'
    // having the specified key as the 'first' value and zero as the 'second'
    // value, and a method to extract the key from a
    // 'bsl::pair<const int, int>'.
{
    // CLASS METHODS
    static void construct(bsl::pair<const int, int> *entry,
                          bslma::Allocator          *allocator,
                          int                        key)
        // Load into the specified 'entry' the pair value comprised of the
        // value of the specified 'key' and zero, using the specified
        // 'allocator' to supply memory.
    {
        bslma::ConstructionUtil::construct(entry, allocator, key, 0);
    }

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    template <class ENTRY_TYPE>
    static const int& key(const ENTRY_TYPE& entry)
#else
    template <class ENTRY_TYPE>
    static typename enable_if<is_convertible<ENTRY_TYPE,
                                             const bsl::pair<const int,
                                                             int> >::value,
                              const int&>::type key(const ENTRY_TYPE& entry)
#endif
        // Return the key of the specified 'entry'.
    {
        return entry.first;
    }
};

                         // ========================
                         // struct TestPairEntryUtil
                         // ========================

template <class KEY, class ENTRY>
struct TestPairEntryUtil
    // This templated utility provides methods to constuct an 'ENTRY' having a
    // specified integer key value, and a method to extract the key from an
    // 'ENTRY'.
{
    // CLASS METHODS
    template <class KEY_TYPE>
    static void construct(
                        ENTRY                                       *entry,
                        bslma::Allocator                            *allocator,
                        BSLS_COMPILERFEATURES_FORWARD_REF(KEY_TYPE)  key)
        // Load into the first sub-value of the specified 'entry' the specified
        // 'key' value, and zero into the second sub-value, using the specified
        // 'allocator' to supply memory.
    {
        bslma::ConstructionUtil::construct(
                                  entry,
                                  allocator,
                                  BSLS_COMPILERFEATURES_FORWARD(KEY_TYPE, key),
                                  0);
    }

    static const KEY& key(const ENTRY& entry)
        // Return the key of the specified 'entry'.
    {
        return entry.first;
    }
};

                           // ====================
                           // struct IsValidResult
                           // ====================

struct IsValidResult
    // Return values for the global 'isValid' method.
{
    enum Enum {
        e_SUCCESS,
        e_BAD_BLOCK,
        e_BAD_HASHLET,
        e_BAD_POSITION,
        e_BAD_SIZE
    };
};

                            // ==================
                            // class IsValidGuard
                            // ==================

template <class TABLE>
class IsValidGuard
    // This class implements a guard that automatically invokes 'isValid' on
    // the managed 'TABLE' upon destruction, and asserts that the return value
    // indicates success.
{
    // DATA
    const TABLE *d_table_p;  // managed table

  public:
    // CREATORS
    explicit IsValidGuard(const TABLE *table)
        // Create a guard that managed the specified 'table'.
    : d_table_p(table)
    {
    }

    ~IsValidGuard()
        // Destroy this object and invoke 'isValid' on the managed table, and
        // assert the success of the 'isValid' invocation.
    {
        bsl::size_t errorIndex;
        ASSERT(IsValidResult::e_SUCCESS == isValid(&errorIndex, *d_table_p));
    }
};

// ============================================================================
//               GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
// ----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//..
// <SPEC> ::= <EMPTY>   | <LIST>
//
// <EMPTY> ::=
//
// <LIST> ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM> ::= <ELEMENT> | <EMPTY_ELEMENT> | <ERASED_ELEMENT>
//
// <ELEMENT> ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
//
// <EMPTY_ELEMENT> ::= 'e'
//
// <ERASED_ELEMENT> ::= 'x'
//..

enum { e_SUCCESS_EMPTY = -1, e_SUCCESS_ERASED = -2, e_SUCCESS_VALUE = -3 };

int getValue(int *value, char specChar, int verboseFlag)
    // Place into the specified 'value' the value corresponding to the
    // specified 'specChar' and display errors to 'cerr' if the specified
    // 'verboseFlag' is set.  Return 'e_SUCCESS_EMPTY', 'e_SUCCESS_ERASED', or
    // 'e_SUCCESS_VALUE' if operation successful, otherwise return a positive
    // integer.  For 'e_SUCCESS_VAUE', 'value' is the expected hashlet of the
    // entry (to be combined with a position dependant component specifying
    // insertion location).
{
    if ('e' == specChar) {
        return e_SUCCESS_EMPTY;                                       // RETURN
    }

    if ('x' == specChar) {
        return e_SUCCESS_ERASED;                                      // RETURN
    }

    if ('A' <= specChar && 'Z' >= specChar) {
        *value = static_cast<int>(specChar - 'A');
        return e_SUCCESS_VALUE;                                       // RETURN
    }

    if (verboseFlag) {
        cerr << "\t\tERROR!" << endl;
        cerr << specChar << " not recognized." << endl;
    }

    return 1;
}

template <class OBJ>
int ggg(OBJ *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator functions.  Optionally specify a zero
    // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
    // index of the first invalid character, and a negative value otherwise.
    // Note that this function is used to implement 'gg' as well as allow for
    // verification of syntax error detection.
{
    typedef typename OBJ::entry_type ENTRY;

    enum { e_SUCCESS = -1 };

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    bsl::vector<int> toErase(&oa);

    const bsl::size_t capacity = bsl::strlen(spec);

    object->reset();

    if (0 == capacity) {
        return e_SUCCESS;                                             // RETURN
    }

    for (int i = 0; object->capacity() < capacity; ++i) {
        bsls::ObjectBuffer<ENTRY> entry;

        bslma::ConstructionUtil::construct(entry.address(), &oa, i);

        bslma::DestructorGuard<ENTRY> guard(entry.address());

        object->insert(entry.object());
    }

    object->clear();

    bsl::uint64_t     numGroup = capacity / k_SIZE;
    const bsl::size_t shift    = 16 - bdlb::BitUtil::log2(numGroup);

    int nextErase = 0x0100;

    for (int i = 0; spec[i]; ++i) {
        int hashlet;
        int rv = getValue(&hashlet, spec[i], verboseFlag);

        if (0 == i % k_SIZE) {
            // since erasing elements will not affect insertion position,
            // process any erasures to ensure the inserts do not cause a rehash

            for (bsl::size_t i = 0; i < toErase.size(); ++i) {
                object->erase(toErase[i]);
            }
        }

        if (e_SUCCESS_EMPTY == rv) {
            // nothing to do
        }
        else if (e_SUCCESS_ERASED == rv) {
            // insert an entry to this location and track the key for future
            // erasure

            int key = ((i / k_SIZE) << shift) | nextErase;

            ++nextErase;
            toErase.push_back(key);

            bsls::ObjectBuffer<ENTRY> entry;

            bslma::ConstructionUtil::construct(entry.address(), &oa, key);

            bslma::DestructorGuard<ENTRY> guard(entry.address());

            object->insert(entry.object());
        }
        else if (e_SUCCESS_VALUE == rv) {
            int key = ((i / k_SIZE) << shift) | hashlet;

            bsls::ObjectBuffer<ENTRY> entry;

            bslma::ConstructionUtil::construct(entry.address(), &oa, key);

            bslma::DestructorGuard<ENTRY> guard(entry.address());

            object->insert(entry.object());
        }
        else {
            return i;                                                 // RETURN
        }
    }

    for (bsl::size_t i = 0; i < toErase.size(); ++i) {
        object->erase(toErase[i]);
    }

    const OBJ& X = *object;

    for (int i = 0; spec[i]; ++i) {
        if ('e' == spec[i]) {
            if (k_EMPTY != X.controls()[i]) {
                return i;                                             // RETURN
            }
        }
        else if ('x' == spec[i]) {
            if (k_ERASED != X.controls()[i]) {
                return i;                                             // RETURN
            }
        }
        else {
            if (static_cast<int>(spec[i] - 'A') != X.controls()[i]) {
                return i;                                             // RETURN
            }
        }
    }

    return e_SUCCESS;
}

template <class OBJ>
OBJ& gg(OBJ *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

// ============================================================================
//                      GLOBAL FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

void printError(const bsl::uint8_t  *controls,
                const bsl::size_t    capacity,
                bsl::size_t          index,
                IsValidResult::Enum  result)
    // Print the specified 'controls' values having the specified 'capacity',
    // an indicator to the error at the specified 'index', and text describing
    // the specified 'result'.
{
    static const char *hex = "0123456789abcdef";

    for (bsl::size_t i = 0; i < capacity; ++i) {
        if (i) {
            cout << ' ';
        }
        bsl::uint8_t c = controls[i];
        cout << hex[c >> 4] << hex[c & 0xf];
    }
    cout << endl;

    if (index < capacity) {
        for (bsl::size_t i = 0; i < index; ++i) {
            cout << "   ";
        }
        cout << "^^";
        cout << endl;
    }

    switch (result) {
      case IsValidResult::e_SUCCESS: {
      } break;
      case IsValidResult::e_BAD_BLOCK: {
        cout << "ERROR: bad block" << endl;
      } break;
      case IsValidResult::e_BAD_HASHLET: {
        cout << "ERROR: bad hashlet" << endl;
      } break;
      case IsValidResult::e_BAD_POSITION: {
        cout << "ERROR: bad position" << endl;
      } break;
      case IsValidResult::e_BAD_SIZE: {
        cout << "ERROR: size is incorrect" << endl;
      } break;
    }
}

template <class ENTRY, class ENTRY_UTIL, class HASH>
IsValidResult::Enum isValid(bsl::size_t        *errorIndex,
                            const ENTRY        *entries,
                            const bsl::uint8_t *controls,
                            const bsl::size_t   size,
                            const bsl::size_t   blockSize,
                            const bsl::size_t   capacity,
                            HASH                hasher,
                            ENTRY_UTIL          )
    // Return 'IsValidResult::e_SUCCESS' if the specified 'entries',
    // 'controls', 'size', and 'capacity' represent a valid table assuming the
    // specified 'blockSize', 'hasher', and 'ENTRY_UTIL'; otherwise return an
    // 'IsValidResult::Enum' value indicating the found error and populate the
    // specified 'errorIndex' with the location of the error, or 'capacity' if
    // the error is not position specific.
{
    if (0 == entries && 0 == controls && 0 == capacity) {
        return IsValidResult::e_SUCCESS;                              // RETURN
    }

    // Confirm that each control block has non-empty entries followed by empty
    // entries.

    {
        for (bsl::size_t i = 0; i < capacity; i += blockSize) {
            bsl::size_t j = 0;
            while (j < blockSize && k_EMPTY != controls[i + j]) {
                ++j;
            }
            while (j < blockSize && k_EMPTY == controls[i + j]) {
                ++j;
            }
            if (blockSize != j) {
                *errorIndex = i;
                return IsValidResult::e_BAD_BLOCK;                    // RETURN
            }
        }
    }

    bsl::uint64_t numGroup = capacity / blockSize;

    int groupControlShift = static_cast<int>(  sizeof(bsl::size_t) * 8
                                             - bdlb::BitUtil::log2(numGroup));

    bsl::size_t inUse = 0;

    for (bsl::size_t i = 0; i < capacity; ++i) {
        if (controls[i] < k_EMPTY) {
            bsl::size_t hash = hasher(ENTRY_UTIL::key(entries[i]));

            if (controls[i] != (hash & 0x7f)) {
                *errorIndex = i;
                return IsValidResult::e_BAD_HASHLET;                  // RETURN
            }

            bsl::size_t index = (hash >> groupControlShift) * blockSize;

            while (index != i) {
                if (controls[index] == k_EMPTY) {
                    int initialIndex = static_cast<int>(
                                      (hash >> groupControlShift) * blockSize);

                    char s[128];
                    sprintf(s,
                            "ERROR: incorrect position for initial index %i",
                            initialIndex);
                    *errorIndex = i;
                    return IsValidResult::e_BAD_POSITION;             // RETURN
                }

                ++index;
                if (index >= capacity) {
                    index = 0;
                }
            }

            ++inUse;
        }
    }

    if (inUse != size) {
        *errorIndex = capacity;
        return IsValidResult::e_BAD_SIZE;                             // RETURN
    }

    return IsValidResult::e_SUCCESS;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
IsValidResult::Enum isValid(bsl::size_t                       *errorIndex,
                            const bdlc::FlatHashTable<KEY,
                                                      ENTRY,
                                                      ENTRY_UTIL,
                                                      HASH,
                                                      EQUAL>&  table)
    // Return 'IsValidResult::e_SUCCESS' if the specified 'table' is valid;
    // otherwise return an 'IsValidResult::Enum' value indicating the found
    // error and populate the specified 'errorIndex' with the location of the
    // error, or 'table.capacity()' if the error is not position specific.
{
    return isValid(errorIndex,
                   table.entries(),
                   table.controls(),
                   table.size(),
                   k_SIZE,
                   table.capacity(),
                   table.hash_function(),
                   ENTRY_UTIL());
}


template <class ENTRY>
void testCase21OperationsWhenMoved(int id)
    // Address the key-based accessor and basic manipulator concerns of
    // moved-to objects from test case 21 for the specified 'id' value.  Note
    // that, in case of a test failure, 'id' can be used to determine 'ENTRY'.
    // See the test plan of case 21 for a description of the concerns checked
    // by this function.
{
    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;
    typedef typename Obj::iterator                                  Iter;
    typedef typename Obj::const_iterator                            ConstIter;
    typedef bsl::pair<ConstIter, ConstIter>                         ConstRange;

    const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Iterate over 'DEFAULT_DATA', which is used to provide specifications for
    // placing flat hash tables in various interesting states, e.g., having
    // certain entries inserted, removed, and never-inserted.
    for (bsl::size_t ti = 0; ti != NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_lineNum;
        const char *const SPEC = DATA[ti].d_spec_p;

        // Iterate over different accessors and basic manipulators to verify
        // for correctness on a moved-to flat hash table.
        enum { LOOKUP, ERASE, INSERT_COPY, INSERT_MOVE, NUM_OPERATIONS };
        for (bsl::size_t oi = 0; oi != NUM_OPERATIONS; ++oi) {
            const bsl::size_t OPERATION = oi;

            // Iterate over 4 different ways to move a flat hash table.  These
            // are, in order: 1) move-constructing with the same allocator, 2)
            // move-constructing with a different allocator, 3) move-assigning
            // with the same allocator, and 4) move-assigning with a different
            // allocator.
            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg; // Designate how we move 'mX' to 'mZ'.

                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                // Generate an 'mX' to move from using 'gg'.  The call to 'gg'
                // places 'mX' in a non-trivial state, which we will move into
                // an object 'mZ'.
                Obj mX(0, Hash(), Equal(), &sa); const Obj& X = mX;
                gg(&mX, SPEC);

                // Copy the generated 'mX' to an 'mY', in order verify the
                // state of 'mZ' later.
                Obj mY(X, &sa);                  const Obj& Y = mY;

                // Create an 'mZ' into which to move 'mX'.
                bsls::ObjectBuffer<Obj>     bufferZ;
                bslma::DestructorGuard<Obj> guardZ(bufferZ.address());

                Obj *pZ = bufferZ.address();

                // Select the manner in which to move 'mX' to 'mZ'.
                switch (CONFIG) {
                  case 'a': {
                    // Move 'mX' to 'mZ' with the same allocator.
                    bslma::ConstructionUtil::construct(
                                              pZ,
                                              &sa,
                                              bslmf::MovableRefUtil::move(mX));

                    LOOP3_ASSERT(id, ti, LINE,   Y != X ||   Y.empty());
                    LOOP3_ASSERT(id, ti, LINE, *pZ != X || pZ->empty());
                    LOOP3_ASSERT(id, ti, LINE, *pZ == Y);
                  } break;
                  case 'b': {
                    // Move 'mX' to 'mZ' with a different allocator.
                    bslma::ConstructionUtil::construct(
                                              pZ,
                                              &za,
                                              bslmf::MovableRefUtil::move(mX));

                    LOOP3_ASSERT(id, ti, LINE,   Y == X);
                    LOOP3_ASSERT(id, ti, LINE, *pZ == X);
                    LOOP3_ASSERT(id, ti, LINE, *pZ == Y);
                  } break;
                  case 'c': {
                    // Move-assign 'mX' to 'mZ' with the same allocator.
                    bslma::ConstructionUtil::construct(
                        pZ, &sa, 0, Hash(), Equal());
                    *pZ = bslmf::MovableRefUtil::move(mX);

                    LOOP3_ASSERT(id, ti, LINE,   Y != X ||   Y.empty());
                    LOOP3_ASSERT(id, ti, LINE, *pZ != X || pZ->empty());
                    LOOP3_ASSERT(id, ti, LINE, *pZ == Y);
                  } break;
                  case 'd': {
                    // Move-assign 'mX' to 'mZ' with a different allocator.
                    bslma::ConstructionUtil::construct(
                        pZ, &za, 0, Hash(), Equal());
                    *pZ = bslmf::MovableRefUtil::move(mX);

                    LOOP3_ASSERT(id, ti, LINE,   Y == X);
                    LOOP3_ASSERT(id, ti, LINE, *pZ == X);
                    LOOP3_ASSERT(id, ti, LINE, *pZ == Y);
                  } break;
                }

                // Get convenience aliases to 'mZ'.
                Obj& mZ = *pZ; const Obj& Z = mZ;

                // Verify that the internal structure of 'mZ' is valid.
                bsl::size_t errorIndex = 0;
                LOOP3_ASSERT(id,
                             ti,
                             LINE,
                             IsValidResult::e_SUCCESS ==
                                 isValid(&errorIndex, Z));

                // Calculate the number of groups that a flat hash table will
                // have based on the 'SPEC'.  This is used to reconstruct the
                // keys of entries that were inserted, removed, or left empty
                // in the call to 'gg'.
                const bsl::uint64_t NUM_GROUPS = bsl::strlen(SPEC) / k_SIZE;

                // If 'mZ' is empty, then there are no contents to check.
                if (0 == NUM_GROUPS) {
                    LOOP3_ASSERT(id, ti, LINE, Z.empty());
                    continue;                                       // CONTINUE
                }

                // 'nextErase' is a variable used to calculate the key of an
                // erased entry.
                int nextErase = 0x100;
                for (int value = 0; SPEC[value]; ++value) {
                    const char SPEC_CHAR = SPEC[value];

                    // Determine if the 'value' has an entry in 'mZ', and if
                    // so, get its hashlet.  The hashlet is used to reconstruct
                    // the key for the value specified by 'SPEC[value]' (as
                    // used in 'gg').
                    int hashlet;
                    const int rc = getValue(&hashlet, SPEC_CHAR, veryVerbose);
                    const bool IS_KEY_IN_TABLE = (e_SUCCESS_VALUE == rc);

                    // 'SHIFT' is a also a constant used to reconstruct the
                    // key for 'value'.
                    const bsl::size_t SHIFT =
                        16 - bdlb::BitUtil::log2(NUM_GROUPS);

                    // Finally, calculate the key for 'value'.
                    int KEY = 0;
                    if (e_SUCCESS_VALUE == rc) {
                        KEY = ((value / k_SIZE) << SHIFT) | hashlet;
                    }
                    else if (e_SUCCESS_EMPTY == rc) {
                        KEY = value;
                    }
                    else if (e_SUCCESS_ERASED == rc) {
                        KEY = ((value / k_SIZE) << SHIFT) | nextErase;
                        nextErase++;
                    }

                    // Given a key and an object 'mZ' in a non-trivial,
                    // moved-to state, verify that the set of accessors, or the
                    // basic manipulator, specified by 'OPERATION' functions
                    // correctly.
                    switch (OPERATION) {
                      case LOOKUP: {
                        // Verify all accessors related to lookup by key.

                        if (IS_KEY_IN_TABLE) {
                            // Verify that they find an entry for the key if it
                            // is in the table.

                            LOOP4_ASSERT(id, ti, KEY, LINE, Z.contains(KEY));

                            LOOP4_ASSERT(id, ti, KEY, LINE, 1 == Z.count(KEY));

                            LOOP4_ASSERT(
                                id, ti, KEY, LINE, Z.end() != Z.find(KEY));
                            LOOP4_ASSERT(
                                id, ti, KEY, LINE, mZ.end() != mZ.find(KEY));

                            LOOP4_ASSERT(
                                id,
                                ti,
                                KEY,
                                LINE,
                                bsl::distance(mZ.begin(), mZ.find(KEY)) ==
                                    bsl::distance(mY.begin(), mY.find(KEY)));
                            LOOP4_ASSERT(
                                id,
                                ti,
                                KEY,
                                LINE,
                                bsl::distance(Z.begin(), Z.find(KEY)) ==
                                    bsl::distance(Y.begin(), Y.find(KEY)));

                            const ConstRange EQUAL_RANGE = Z.equal_range(KEY);
                            LOOP4_ASSERT(
                                id,
                                ti,
                                KEY,
                                LINE,
                                1 == bsl::distance(EQUAL_RANGE.first,
                                                   EQUAL_RANGE.second));
                        }
                        else {
                            // Verify that they do not find an entry for the
                            // key if it is not in the table.

                            LOOP4_ASSERT(id, ti, KEY, LINE, !Z.contains(KEY));

                            LOOP4_ASSERT(id, ti, KEY, LINE, 0 == Z.count(KEY));

                            LOOP4_ASSERT(
                                id, ti, KEY, LINE, Z.end() == Z.find(KEY));
                            LOOP4_ASSERT(
                                id, ti, KEY, LINE, mZ.end() == mZ.find(KEY));

                            const ConstRange EQUAL_RANGE = Z.equal_range(KEY);
                            LOOP4_ASSERT(
                                id,
                                ti,
                                KEY,
                                LINE,
                                0 == bsl::distance(EQUAL_RANGE.first,
                                                   EQUAL_RANGE.second));
                        }
                      } break;
                      case ERASE: {
                        // Verify the 'erase' basic manipulator.

                        if (IS_KEY_IN_TABLE) {
                            // Verify that it erases the entry for the key if
                            // such an entry exists in the table.
                            LOOP4_ASSERT(
                                id, ti, KEY, LINE, 1 == mZ.erase(KEY));
                        }
                        else {
                            // Verify that it does not erase an entry for the
                            // key if such an entry does not exist in the
                            // table.
                            LOOP4_ASSERT(
                                id, ti, KEY, LINE, 0 == mZ.erase(KEY));
                        }
                      } break;
                      case INSERT_COPY:                       BSLA_FALLTHROUGH;
                      case INSERT_MOVE: {
                        // Verify the 'insert' basic manipulator.

                        // First, construct an entry for the key.
                        bsls::ObjectBuffer<ENTRY> entry;
                        bslma::ConstructionUtil::construct(
                            entry.address(), &za, KEY);
                        bslma::DestructorGuard<ENTRY> guardEntry(
                            entry.address());

                        // Then, copy the entry in order to verify the value
                        // of the inserted entry.
                        bsls::ObjectBuffer<ENTRY> entryCopy;
                        bslma::ConstructionUtil::construct(
                            entryCopy.address(), &za, entry.object());
                        bslma::DestructorGuard<ENTRY> guardEntryCopy(
                            entryCopy.address());

                        // Then, insert it into 'mZ'.
                        bsl::pair<Iter, bool> INSERTION;
                        if (OPERATION == INSERT_COPY) {
                            INSERTION = mZ.insert(entry.object());
                        }
                        else if (OPERATION == INSERT_MOVE) {
                            INSERTION = mZ.insert(
                                bslmf::MovableRefUtil::move(entry.object()));
                        }

                        // Verify that the entry in 'mZ' associated with the
                        // returned insertion result is equal to the entry that
                        // was inserted.
                        LOOP4_ASSERT(id,
                                     ti,
                                     KEY,
                                     LINE,
                                     entryCopy.object() == *INSERTION.first);

                        if (IS_KEY_IN_TABLE) {
                            // Verify that an insertion did not occur if an
                            // entry for the key already existed in the table.
                            LOOP4_ASSERT(
                                id, ti, KEY, LINE, false == INSERTION.second);
                        }
                        else {
                            // Verify that an insertion did occur if an entry
                            // for the key did not exist in the table.
                            LOOP4_ASSERT(
                                id, ti, KEY, LINE, true == INSERTION.second);
                        }
                      } break;
                    }
                }
            }
        }
    }
}

template <class ENTRY>
void testCase20Bracket(int id, bool allocates, bool allocatesOnRehash)
    // Address the copy 'operator[]' concerns of test case 20 for the specified
    // 'id' value, for a type that allocates if the specified 'allocates' is
    // 'true', and allocates on rehash if the specified 'allocatesOnRehash' is
    // 'true'.  Note that, in case of a test failure, 'id' can be used to
    // determine 'ENTRY'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    bsl::pair<typename Obj::iterator, bool> rv;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
        bsls::Types::Int64 expAllocations = oa.numAllocations();
        bsl::size_t        expCapacity    = 0;

        Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

        IsValidGuard<Obj> guard(&X);

        LOOP_ASSERT(id, expAllocations == oa.numAllocations());

        for (int i = 0; i <= thirdAlloc; ++i) {
            // insert
            LOOP2_ASSERT(id, i, i == EntryUtil::key(mX[i]));

            // lookup
            LOOP2_ASSERT(id, i, i == EntryUtil::key(mX[i]));

            if (allocates) {
                expAllocations += 1;
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
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

    ASSERT(0 == oa.numBytesInUse());
}

template <class KEY>
void testCase20BracketMove(int id, bool allocates, bool moveable)
    // Address the move 'operator[]' concerns of test case 20 for the specified
    // 'id' value, for a type that allocates if the specified 'allocates' is
    // 'true', and is moveable if the specified 'moveable' is 'true'.  Note
    // that, in case of a test failure, 'id' can be used to determine 'ENTRY'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef bsl::pair<KEY, int>                                     Entry;
    typedef TestPairEntryUtil<KEY, Entry>                           EntryUtil;
    typedef TestValueIsHash<KEY>                                    Hash;
    typedef bsl::equal_to<KEY>                                      Equal;
    typedef bdlc::FlatHashTable<KEY, Entry, EntryUtil, Hash, Equal> Obj;

    bsl::pair<typename Obj::iterator, bool> rv;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
        bsls::Types::Int64 expAllocations = oa.numAllocations();
        bsl::size_t        expCapacity    = 0;

        Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

        IsValidGuard<Obj> guard(&X);

        LOOP_ASSERT(id, expAllocations == oa.numAllocations());

        for (int i = 0; i <= thirdAlloc; ++i) {
            bsls::ObjectBuffer<KEY> key1;
            bslma::ConstructionUtil::construct(key1.address(), &oa, i);

            bslma::DestructorGuard<KEY> guard1(&key1.object());

            bsls::ObjectBuffer<KEY> key2;
            bslma::ConstructionUtil::construct(key2.address(), &oa, i);

            bslma::DestructorGuard<KEY> guard2(&key2.object());

            // insert
            Entry& entry1 = mX[bslmf::MovableRefUtil::move(key1.object())];
            LOOP2_ASSERT(id, i, i == EntryUtil::key(entry1).data());
            LOOP2_ASSERT(id, i, 0 == entry1.second);

            // modify
            entry1.second = 3;

            // lookup
            Entry& entry2 = mX[bslmf::MovableRefUtil::move(key2.object())];
            LOOP2_ASSERT(id, i, &entry1 == &entry2);
            LOOP2_ASSERT(id, i,       i == EntryUtil::key(entry2).data());
            LOOP2_ASSERT(id, i,       3 == entry2.second);

            if (allocates) {
                if (moveable && (   !bslmf::IsBitwiseMoveable<KEY>::value
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
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class ENTRY>
void testCase19Rehash(int id)
    // Address the 'rehash' concerns of test case 19 for the specified 'id'
    // value.  Note that, in case of a test failure, 'id' can be used to
    // determine 'ENTRY'.
{
    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const bsl::size_t capacities[] = { 0, 1, 7, 32, 49, 64, 128 };
    bsl::size_t NUM_CAPACITIES = sizeof capacities / sizeof *capacities;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_lineNum;
        const char *const SPEC = DATA[ti].d_spec_p;

        bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

        Obj        mZ(0, Hash(), Equal(), &oa);
        const Obj& Z = gg(&mZ, SPEC);

        for (bsl::size_t tj = 0; tj < NUM_CAPACITIES; ++tj) {
            const bsl::size_t CAPACITY = capacities[tj];

            bsls::Types::Int64 expInUse = oa.numBytesInUse();

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC);

                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                mX.rehash(CAPACITY);

                LOOP3_ASSERT(id, LINE, CAPACITY, X == Z);

                LOOP3_ASSERT(id, LINE, CAPACITY, CAPACITY <= X.capacity());

                if (0 == X.capacity()) {
                    LOOP3_ASSERT(id, LINE, CAPACITY, 0 == X.entries());
                    LOOP3_ASSERT(id, LINE, CAPACITY, 0 == X.controls());
                }
                else {
                    LOOP3_ASSERT(id,
                                 LINE,
                                 CAPACITY,
                                 X.capacity() >= Obj::k_MIN_CAPACITY);

                    LOOP3_ASSERT(
                        id,
                        LINE,
                        CAPACITY,
                        X.capacity() ==
                            bdlb::BitUtil::roundUpToBinaryPower(
                                    static_cast<bsl::uint64_t>(X.capacity())));

                    for (bsl::size_t i = 0; i < X.capacity(); ++i) {
                        LOOP3_ASSERT(id,
                                     LINE,
                                     CAPACITY,
                                     k_ERASED != X.controls()[i]);
                    }
                }

                LOOP3_ASSERT(id,
                             LINE,
                             CAPACITY,
                             X.load_factor() <= X.max_load_factor());

                bsl::size_t errorIndex;
                LOOP3_ASSERT(
                          id,
                          LINE,
                          CAPACITY,
                          IsValidResult::e_SUCCESS == isValid(&errorIndex, X));

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            LOOP3_ASSERT(id, LINE, CAPACITY, expInUse == oa.numBytesInUse());
        }

        {
            bsls::Types::Int64 expInUse = oa.numBytesInUse();

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC);

                mX.clear();

                mX.rehash(0);

                LOOP2_ASSERT(id, LINE, 0 == X.capacity());
                LOOP2_ASSERT(id, LINE, 0 == X.entries());
                LOOP2_ASSERT(id, LINE, 0 == X.controls());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            LOOP2_ASSERT(id, LINE, expInUse == oa.numBytesInUse());
        }
    }
}

template <class ENTRY>
void testCase17IteratorErase(int id)
    // Address the 'erase' concerns of test case 17 for the specified 'id'
    // value.  Note that, in case of a test failure, 'id' can be used to
    // determine 'ENTRY'.
{
    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_lineNum;
        const char *const SPEC = DATA[ti].d_spec_p;

        bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

        Obj        mZ(0, Hash(), Equal(), &oa);
        const Obj& Z = gg(&mZ, SPEC);

        // Test with an 'iterator'.

        {
            typename Obj::const_iterator keyIter = Z.begin();
            while (keyIter != Z.end()) {
                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC);

                Obj        mEXP(0, Hash(), Equal(), &oa);
                const Obj& EXP = gg(&mEXP, SPEC);
                mEXP.erase(EntryUtil::key(*keyIter));

                typename Obj::iterator iter     = mX.find(
                                                     EntryUtil::key(*keyIter));
                typename Obj::iterator nextIter = iter;
                ++nextIter;

                typename Obj::iterator returnIter = mX.erase(iter);

                LOOP2_ASSERT(id, LINE,        X == EXP);
                LOOP2_ASSERT(id, LINE, nextIter == returnIter);
                LOOP2_ASSERT(id, LINE, k_ERASED == X.controls()[&*keyIter
                                                               - Z.entries()]);

                if (X.empty()) {
                    LOOP2_ASSERT(id, LINE, X.end() == returnIter);
                }

                ++keyIter;
            }
        }

        // Test with an 'const_iterator'.

        {
            typename Obj::const_iterator keyIter = Z.begin();
            while (keyIter != Z.end()) {
                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC);

                Obj        mEXP(0, Hash(), Equal(), &oa);
                const Obj& EXP = gg(&mEXP, SPEC);
                mEXP.erase(EntryUtil::key(*keyIter));

                typename Obj::const_iterator iter     = X.find(
                                                     EntryUtil::key(*keyIter));
                typename Obj::const_iterator nextIter = iter;
                ++nextIter;

                typename Obj::iterator returnIter = mX.erase(iter);

                LOOP2_ASSERT(id, LINE,        X == EXP);
                LOOP2_ASSERT(id, LINE, nextIter == returnIter);
                LOOP2_ASSERT(id, LINE, k_ERASED == X.controls()[&*keyIter
                                                               - Z.entries()]);

                if (X.empty()) {
                    LOOP2_ASSERT(id, LINE, X.end() == returnIter);
                }

                ++keyIter;
            }
        }
    }
}

template <class ENTRY>
void testCase15MoveAssignment(int id)
    // Address the move assignment concerns of test case 15 for the specified
    // 'id' value.  Note that, in case of a test failure, 'id' can be used to
    // determine 'ENTRY'.
{
    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    {
        typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj> Obj);

        // Verify that the signature and return type are standard.

        operatorPtr operatorMoveAssignment = &Obj::operator=;

        // Quash unused variable warning.

        Obj a(0, Hash(), Equal());
        Obj b(0, Hash(), Equal());
        (a.*operatorMoveAssignment)(bslmf::MovableRefUtil::move(b));
    }

    // Create a test allocator and install it as the default.

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1 = DATA[ti].d_lineNum;
        const char *const SPEC1 = DATA[ti].d_spec_p;
        const int         ID1   = DATA[ti].d_valueId;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj        mZZ(0, Hash(), Equal(), &scratch);
        const Obj& ZZ = gg(&mZZ, SPEC1);

        // Ensure the first row of the table contains the default-constructed
        // value.

        static bool firstFlag = true;
        if (firstFlag) {
            LOOP2_ASSERT(id, LINE1, ZZ == Obj(0, Hash(), Equal(), &scratch));
            firstFlag = false;
        }

        // Test with same allocator.

        for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2 = DATA[tj].d_lineNum;
            const char *const SPEC2 = DATA[tj].d_spec_p;
            const int         ID2   = DATA[tj].d_valueId;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                const bsls::Types::Int64 AL = oa.allocationLimit();

                oa.setAllocationLimit(-1);

                Obj        mZ(0, Hash(), Equal(), &oa);
                const Obj& Z = gg(&mZ, SPEC1);

                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC2);

                LOOP3_ASSERT(id, LINE1, LINE2,(X == Z) == (ID1 == ID2));

                oa.setAllocationLimit(AL);

                bsls::Types::Int64 oaExpNumAllocations = oa.numAllocations();
                bsls::Types::Int64 sExpNumAllocations  =
                                                      scratch.numAllocations();

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));

                // Verify the address of the return value.

                LOOP3_ASSERT(id, LINE1, LINE2, mR == &mX);

                LOOP3_ASSERT(id, LINE1, LINE2,  X == ZZ);

                bsl::size_t errorIndex;
                LOOP3_ASSERT(id,
                             LINE1,
                             LINE2,
                             IsValidResult::e_SUCCESS == isValid(&errorIndex,
                                                                 X));
                LOOP3_ASSERT(id,
                             LINE1,
                             LINE2,
                             IsValidResult::e_SUCCESS == isValid(&errorIndex,
                                                                 Z));

                LOOP3_ASSERT(id, LINE1, LINE2, &oa == X.allocator());
                LOOP3_ASSERT(id, LINE1, LINE2, &oa == Z.allocator());

                LOOP3_ASSERT(id,
                             LINE1,
                             LINE2,
                             oaExpNumAllocations == oa.numAllocations());
                LOOP3_ASSERT(id,
                             LINE1,
                             LINE2,
                             sExpNumAllocations  == scratch.numAllocations());

                LOOP3_ASSERT(id, LINE1, LINE2, !da.numBlocksTotal());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            // Verify all memory is released on object destruction.

            LOOP3_ASSERT(id, LINE1, LINE2, 0 == da.numBlocksInUse());
            LOOP3_ASSERT(id, LINE1, LINE2, 0 == oa.numBlocksInUse());
        }

        // Test with different allocators.

        for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2 = DATA[tj].d_lineNum;
            const char *const SPEC2 = DATA[tj].d_spec_p;
            const int         ID2   = DATA[tj].d_valueId;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                const bsls::Types::Int64 AL = oa.allocationLimit();

                oa.setAllocationLimit(-1);

                Obj        mZ(0, Hash(), Equal(), &scratch);
                const Obj& Z = gg(&mZ, SPEC1);

                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC2);

                LOOP3_ASSERT(id, LINE1, LINE2,(X == Z) == (ID1 == ID2));

                oa.setAllocationLimit(AL);

                bsls::Types::Int64 sExpNumAllocations =
                                                      scratch.numAllocations();

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));

                // Verify the address of the return value.

                LOOP3_ASSERT(id, LINE1, LINE2, mR == &mX);

                LOOP3_ASSERT(id, LINE1, LINE2,  X == ZZ);

                bsl::size_t errorIndex;
                LOOP3_ASSERT(id,
                             LINE1,
                             LINE2,
                             IsValidResult::e_SUCCESS == isValid(&errorIndex,
                                                                 X));
                LOOP3_ASSERT(id,
                             LINE1,
                             LINE2,
                             IsValidResult::e_SUCCESS == isValid(&errorIndex,
                                                                 Z));

                LOOP3_ASSERT(id, LINE1, LINE2,      &oa == X.allocator());
                LOOP3_ASSERT(id, LINE1, LINE2, &scratch == Z.allocator());

                LOOP3_ASSERT(id,
                             LINE1,
                             LINE2,
                             sExpNumAllocations == scratch.numAllocations());

                LOOP3_ASSERT(id, LINE1, LINE2, !da.numBlocksTotal());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            // Verify all memory is released on object destruction.

            LOOP3_ASSERT(id, LINE1, LINE2, 0 == da.numBlocksInUse());
            LOOP3_ASSERT(id, LINE1, LINE2, 0 == oa.numBlocksInUse());
        }

        // self-assignment

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX(0, Hash(), Equal(), &oa);   const Obj& X  = gg(&mX, SPEC1);
            Obj mZZ(0, Hash(), Equal(), &oa);  const Obj& ZZ = gg(&mZZ, SPEC1);

            const Obj& Z = mX;

            LOOP2_ASSERT(id, LINE1, ZZ == Z);

            bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

            Obj *mR = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                mR = &(mX = bslmf::MovableRefUtil::move(Z));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            // Verify the address of the return value.

            LOOP2_ASSERT(id, LINE1, mR == &mX);
            LOOP2_ASSERT(id, LINE1, mR == &X);
            LOOP2_ASSERT(id, LINE1, ZZ == Z);

            LOOP2_ASSERT(id, LINE1, &oa == Z.allocator());

            LOOP2_ASSERT(id, LINE1, oam.isInUseSame());

            LOOP2_ASSERT(id, LINE1, sam.isInUseSame());

            LOOP2_ASSERT(id, LINE1, !da.numBlocksTotal());
        }

        // Verify all memory is released on object destruction.

        LOOP2_ASSERT(id, LINE1, 0 == oa.numBlocksInUse());
        LOOP2_ASSERT(id, LINE1, 0 == da.numBlocksInUse());
    }
}

template <class ENTRY>
void testCase14MoveConstructorWithAllocator(int id)
    // Address the move constructor with allocator concerns of test case 14 for
    // the specified 'id' value.  Note that, in case of a test failure, 'id'
    // can be used to determine 'ENTRY'.
{
    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bsl::size_t errorIndex;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_lineNum;
        const char *const SPEC = DATA[ti].d_spec_p;

        bslma::TestAllocator scratchAllocator("scratch", veryVeryVeryVerbose);

        // Create control object 'W'.
        Obj        mW(0, Hash(), Equal(), &scratchAllocator);
        const Obj& W = gg(&mW, SPEC);

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

            LOOP2_ASSERT(id, LINE, Y == W);

            Obj                  *objPtr;

            bslma::TestAllocator *objAllocatorPtr;  // object allocator

            bslma::TestAllocator *othAllocatorPtr;  // arbitrary "other"
                                                    // allocator

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mY));
                objAllocatorPtr = &sa;
                othAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mY),
                                      static_cast<bslma::Allocator *>(0));
                objAllocatorPtr = &da;
                othAllocatorPtr = &za;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mY), &sa);
                objAllocatorPtr = &sa;
                othAllocatorPtr = &da;
              } break;
              case 'd': {
                objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mY), &za);
                objAllocatorPtr = &za;
                othAllocatorPtr = &da;
              } break;
              default: {
                LOOP3_ASSERT(id, LINE, CONFIG, !"Bad allocator config.");
                return;                                               // RETURN
              }
            }

            // Verify 'Y' is in a valid state.
            LOOP2_ASSERT(id,
                         LINE,
                         IsValidResult::e_SUCCESS == isValid(&errorIndex, Y));

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = *othAllocatorPtr;

            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify the validity and value of the object.
            LOOP2_ASSERT(id,
                         LINE,
                         IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
            LOOP2_ASSERT(id, LINE, X == W);

            // Verify that 'X' and 'Y' have the correct allocator.
            LOOP2_ASSERT(id, LINE, &oa == X.allocator());
            LOOP2_ASSERT(id, LINE, &sa == Y.allocator());

            // Verify no allocation from the non-object allocator and that the
            // object allocator is hooked up.
            if ('a' != CONFIG) {
                LOOP2_ASSERT(id, LINE, 0 == noa.numBlocksTotal());
            }
            if (0 != X.capacity()) {
                LOOP2_ASSERT(id, LINE, 0 < oa.numBlocksTotal());
            }

            // Verify the validity and value of the object after modifying 'Y'.
            if (W.empty()) {
                LOOP2_ASSERT(id, LINE, pY->empty());
                LOOP2_ASSERT(id, LINE, *pY == W);
            }
            else if (Y.allocator() == objPtr->allocator()) {
                LOOP2_ASSERT(id, LINE, pY->empty());
                LOOP2_ASSERT(id, LINE, *pY != W);
            }
            else if (Y.allocator() != objPtr->allocator()) {
                LOOP2_ASSERT(id, LINE, !pY->empty());
                LOOP2_ASSERT(id, LINE, *pY == W);
            }

            mY = bslmf::MovableRefUtil::move(W);

            LOOP2_ASSERT(id,
                         LINE,
                         IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
            LOOP2_ASSERT(id, LINE, X == W);

            // Verify the validity and value of the object after destruction of
            // 'Y'.

            pY->~Obj();

            LOOP2_ASSERT(id,
                         LINE,
                         IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
            LOOP2_ASSERT(id, LINE, X == W);

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.
            ASSERTV(SPEC, 0 == fa.numBlocksInUse());
            ASSERTV(SPEC, 0 == da.numBlocksInUse());
            ASSERTV(SPEC, 0 == sa.numBlocksInUse());
            ASSERTV(SPEC, 0 == za.numBlocksInUse());
        }
    }

    // Verify exception neutrality.

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_lineNum;
        const char *const SPEC = DATA[ti].d_spec_p;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj        mZ(0, Hash(), Equal(), &scratch);
        const Obj& Z = gg(&mZ, SPEC);

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

            const bsls::Types::Int64 AL = oa.allocationLimit();

            oa.setAllocationLimit(-1);

            Obj mY(Z, &oa);

            oa.setAllocationLimit(AL);

            Obj obj(bslmf::MovableRefUtil::move(mY), &oa);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        LOOP2_ASSERT(id, LINE, !da.numBlocksInUse());
        LOOP2_ASSERT(id, LINE, !oa.numBlocksInUse());
    }
}

template <class ENTRY>
void testCase13MoveConstructorWithoutAllocator(int id)
    // Address the move constructor without allocator concerns of test case 13
    // for the specified 'id' value.  Note that, in case of a test failure,
    // 'id' can be used to determine 'ENTRY'.
{
    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bsl::size_t errorIndex;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_lineNum;
        const char *const SPEC = DATA[ti].d_spec_p;

        bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

        // Create control object 'W'.
        Obj        mW(0, Hash(), Equal(), &oa);
        const Obj& W = gg(&mW, SPEC);

        // Create source object 'Y' in an object buffer so it may be deleted.
        bsls::ObjectBuffer<Obj> objY;
        bslma::ConstructionUtil::construct(objY.address(), &oa, W);
        Obj& mY = objY.object();  const Obj& Y = mY;

        LOOP2_ASSERT(id, LINE, Y == W);

        bsls::Types::Int64 expNumAllocations = oa.numAllocations();

        // Move construct.
        Obj mX(bslmf::MovableRefUtil::move(mY));  const Obj& X = mX;

        // Verify original object's allocator has not changed.
        LOOP2_ASSERT(id, LINE, &oa == Y.allocator());

        // Verify no memory was ever allocated (default and global allocators
        // are checked in 'main').

        LOOP2_ASSERT(id, LINE, expNumAllocations == oa.numAllocations());

        // Verify the validity and value of the object.
        LOOP2_ASSERT(id, LINE, IsValidResult::e_SUCCESS == isValid(&errorIndex,
                                                                   X));

        LOOP2_ASSERT(id, LINE,                    X == W);
        LOOP2_ASSERT(id, LINE,        X.allocator() == W.allocator());
        LOOP2_ASSERT(id, LINE, X.hash_function()(0) == W.hash_function()(0));
        LOOP2_ASSERT(id, LINE, X.hash_function()(1) == W.hash_function()(1));
        LOOP2_ASSERT(id, LINE,     X.key_eq()(0, 0) == W.key_eq()(0, 0));
        LOOP2_ASSERT(id, LINE,     X.key_eq()(0, 1) == W.key_eq()(0, 1));

        // Verify 'Y' is valid.
        LOOP2_ASSERT(id, LINE, IsValidResult::e_SUCCESS == isValid(&errorIndex,
                                                                   Y));

        // Verify 'X' is valid after 'Y' is modified and still equal to 'W'.

        if (0 != W.size()) {
            LOOP2_ASSERT(id, LINE, Y != W);
        }

        mY = bslmf::MovableRefUtil::move(W);

        LOOP2_ASSERT(id, LINE, IsValidResult::e_SUCCESS == isValid(&errorIndex,
                                                                   X));

        LOOP2_ASSERT(id, LINE,                    X == W);
        LOOP2_ASSERT(id, LINE,        X.allocator() == W.allocator());
        LOOP2_ASSERT(id, LINE, X.hash_function()(0) == W.hash_function()(0));
        LOOP2_ASSERT(id, LINE, X.hash_function()(1) == W.hash_function()(1));
        LOOP2_ASSERT(id, LINE,     X.key_eq()(0, 0) == W.key_eq()(0, 0));
        LOOP2_ASSERT(id, LINE,     X.key_eq()(0, 1) == W.key_eq()(0, 1));

        // Verify 'X' is valid after 'Y' is destroyed and still equal to 'W'.

        mY.~Obj();

        LOOP2_ASSERT(id, LINE, IsValidResult::e_SUCCESS == isValid(&errorIndex,
                                                                   X));

        LOOP2_ASSERT(id, LINE,                    X == W);
        LOOP2_ASSERT(id, LINE,        X.allocator() == W.allocator());
        LOOP2_ASSERT(id, LINE, X.hash_function()(0) == W.hash_function()(0));
        LOOP2_ASSERT(id, LINE, X.hash_function()(1) == W.hash_function()(1));
        LOOP2_ASSERT(id, LINE,     X.key_eq()(0, 0) == W.key_eq()(0, 0));
        LOOP2_ASSERT(id, LINE,     X.key_eq()(0, 1) == W.key_eq()(0, 1));
    }
}

template <class ENTRY>
void testCase9CopyAssignment(int id)
    // Address the copy assignment concerns of test case 9 for the specified
    // 'id' value.  Note that, in case of a test failure, 'id' can be used to
    // determine 'ENTRY'.
{
    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    {
        typedef Obj& (Obj::*operatorPtr)(const Obj&);

        // Verify that the signature and return type are standard.

        operatorPtr operatorAssignment = &Obj::operator=;

        // Quash unused variable warning.

        Obj a(0, Hash(), Equal());
        Obj b(0, Hash(), Equal());
        (a.*operatorAssignment)(b);
    }

    // Create a test allocator and install it as the default.

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1 = DATA[ti].d_lineNum;
        const char *const SPEC1 = DATA[ti].d_spec_p;
        const int         ID1   = DATA[ti].d_valueId;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj        mZ(0, Hash(), Equal(), &scratch);
        const Obj& Z = gg(&mZ, SPEC1);

        Obj        mZZ(0, Hash(), Equal(), &scratch);
        const Obj& ZZ = gg(&mZZ, SPEC1);

        // Ensure the first row of the table contains the default-constructed
        // value.

        static bool firstFlag = true;
        if (firstFlag) {
            LOOP2_ASSERT(id, LINE1, Z == Obj(0, Hash(), Equal(), &scratch));
            firstFlag = false;
        }

        for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2 = DATA[tj].d_lineNum;
            const char *const SPEC2 = DATA[tj].d_spec_p;
            const int         ID2   = DATA[tj].d_valueId;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC2);

                LOOP3_ASSERT(id, LINE1, LINE2,(X == Z) == (ID1 == ID2));

                bslma::TestAllocatorMonitor sam(&scratch);

                Obj *mR = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mR = &(mX = Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // Verify the address of the return value.

                LOOP3_ASSERT(id, LINE1, LINE2, mR == &mX);

                LOOP3_ASSERT(id, LINE1, LINE2,  X == Z);
                LOOP3_ASSERT(id, LINE1, LINE2, ZZ == Z);

                LOOP3_ASSERT(id, LINE1, LINE2,      &oa == X.allocator());
                LOOP3_ASSERT(id, LINE1, LINE2, &scratch == Z.allocator());

                LOOP3_ASSERT(id, LINE1, LINE2, sam.isInUseSame());
                LOOP3_ASSERT(id, LINE1, LINE2, !da.numBlocksTotal());
            }

            // Verify all memory is released on object destruction.

            LOOP3_ASSERT(id, LINE1, LINE2, 0 == da.numBlocksInUse());
            LOOP3_ASSERT(id, LINE1, LINE2, 0 == oa.numBlocksInUse());
        }

        // self-assignment

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX(0, Hash(), Equal(), &oa);   const Obj& X  = gg(&mX, SPEC1);
            Obj mZZ(0, Hash(), Equal(), &oa);  const Obj& ZZ = gg(&mZZ, SPEC1);

            const Obj& Z = mX;

            LOOP2_ASSERT(id, LINE1, ZZ == Z);

            bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

            Obj *mR = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                mR = &(mX = Z);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            // Verify the address of the return value.

            LOOP2_ASSERT(id, LINE1, mR == &mX);
            LOOP2_ASSERT(id, LINE1, mR == &X);
            LOOP2_ASSERT(id, LINE1, ZZ == Z);

            LOOP2_ASSERT(id, LINE1, &oa == Z.allocator());

            if (0 == ti) {  // Empty, no allocation.
                LOOP2_ASSERT(id, LINE1, oam.isInUseSame());
            }

            LOOP2_ASSERT(id, LINE1, sam.isInUseSame());

            LOOP2_ASSERT(id, LINE1, !da.numBlocksTotal());
        }

        // Verify all memory is released on object destruction.

        LOOP2_ASSERT(id, LINE1, 0 == oa.numBlocksInUse());
        LOOP2_ASSERT(id, LINE1, 0 == da.numBlocksInUse());
    }
}

template <class ENTRY>
void testCase8Swap(int id)
    // Address the 'swap' concerns of test case 8 for the specified 'id' value.
    // Note that, in case of a test failure, 'id' can be used to determine
    // 'ENTRY'.
{
    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    // Assign the address of each function to a variable.

    {
        using namespace bdlc;

        typedef void (Obj::*funcPtr)(Obj&);

        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = swap;

        // Quash unused variable warnings.

        Obj a(0, Hash(), Equal());
        Obj b(0, Hash(), Equal());
        (a.*memberSwap)(b);
        freeSwap(a, b);
    }

    // Create a test allocator and install it as the default.

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_lineNum;
        const char *const SPEC1   = DATA[ti].d_spec_p;

        bslma::TestAllocator oa("object",       veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj        mW(0, Hash(), Equal(), &oa);
        const Obj& W = gg(&mW, SPEC1);
        const Obj  XX(W, &scratch);

        // Ensure the first row of the table contains the default-constructed
        // value.

        static bool firstFlag = true;
        if (firstFlag) {
            LOOP2_ASSERT(id, LINE1, W == Obj(0, Hash(), Equal(), &scratch));
            firstFlag = false;
        }

        // member 'swap'

        {
            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            LOOP2_ASSERT(id, LINE1, W == XX);
            LOOP2_ASSERT(id, LINE1, &oa == W.allocator());
            LOOP2_ASSERT(id, LINE1, oam.isTotalSame());
        }

        // free function 'swap'

        {
            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            LOOP2_ASSERT(id, LINE1, W == XX);
            LOOP2_ASSERT(id, LINE1, &oa == W.allocator());
            LOOP2_ASSERT(id, LINE1, oam.isTotalSame());
        }

        for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2   = DATA[tj].d_lineNum;
            const char *const SPEC2   = DATA[tj].d_spec_p;

            Obj        mX(XX, &oa);
            const Obj& X = mX;

            Obj        mY(0, Hash(), Equal(), &oa);
            const Obj& Y = gg(&mY, SPEC2);
            const Obj  YY(Y, &scratch);

            // member 'swap'

            {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                LOOP3_ASSERT(id, LINE1, LINE2,   X == YY);
                LOOP3_ASSERT(id, LINE1, LINE2,   Y == XX);
                LOOP3_ASSERT(id, LINE1, LINE2, &oa == X.allocator());
                LOOP3_ASSERT(id, LINE1, LINE2, &oa == Y.allocator());
                LOOP3_ASSERT(id, LINE1, LINE2, oam.isTotalSame());
            }

            // free function 'swap', same allocator

            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                LOOP3_ASSERT(id, LINE1, LINE2,   X == XX);
                LOOP3_ASSERT(id, LINE1, LINE2,   Y == YY);
                LOOP3_ASSERT(id, LINE1, LINE2, &oa == X.allocator());
                LOOP3_ASSERT(id, LINE1, LINE2, &oa == Y.allocator());
                LOOP3_ASSERT(id, LINE1, LINE2, oam.isTotalSame());
            }
        }
    }

    // Invoke free 'swap' function in a context where ADL is used.

    {
        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj        mX(0, Hash(), Equal(), &oa);
        const Obj& X = mX;
        const Obj  XX(X, &scratch);

        Obj        mY(0, Hash(), Equal(), &oa);
        const Obj& Y = gg(&mY, "EDCBAeeeeeeeeeeeeeeeeeeeeeeeeeee");
        const Obj  YY(Y, &scratch);

        bslma::TestAllocatorMonitor oam(&oa);

        bslalg::SwapUtil::swap(&mX, &mY);

        LOOP_ASSERT(id, YY == X);
        LOOP_ASSERT(id, XX == Y);
        LOOP_ASSERT(id, oam.isTotalSame());
    }

    // Verify no memory is allocated from the default allocator.

    LOOP2_ASSERT(id, da.numBlocksTotal(), !da.numBlocksTotal());

    // Free 'swap' function with different allocators.

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1 = DATA[ti].d_lineNum;
        const char *const SPEC1 = DATA[ti].d_spec_p;

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator     oa2("object2", veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj   mXX(0, Hash(), Equal(), &scratch);
        const Obj& XX = gg(&mXX, SPEC1);

        for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2 = DATA[tj].d_lineNum;
            const char *const SPEC2 = DATA[tj].d_spec_p;

            Obj mX(XX, &oa);  const Obj& X = mX;

            Obj        mY(0, Hash(), Equal(), &oa2);
            const Obj& Y = gg(&mY, SPEC2);
            const Obj  YY(Y, &scratch);

            // free function 'swap', different allocators
            {
                swap(mX, mY);

                ASSERTV(id, LINE1, LINE2, YY == X);
                ASSERTV(id, LINE1, LINE2, XX == Y);
                ASSERTV(id, LINE1, LINE2, &oa  == X.allocator());
                ASSERTV(id, LINE1, LINE2, &oa2 == Y.allocator());
            }
        }
    }

    // Negative Testing.

    {
        bsls::AssertTestHandlerGuard hG;

        bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
        bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

        Obj mA(0, Hash(), Equal(), &oa1);
        Obj mB(0, Hash(), Equal(), &oa1);
        Obj mC(0, Hash(), Equal(), &oa1);
        Obj mZ(0, Hash(), Equal(), &oa2);

        ASSERT_SAFE_PASS(mA.swap(mB));
        ASSERT_SAFE_FAIL(mC.swap(mZ));
    }
}

template <class ENTRY>
void testCase7CopyConstructor(int id)
    // Address the copy constructor concerns of test case 7 for the specified
    // 'id' value.  Note that, in case of a test failure, 'id' can be used to
    // determine 'ENTRY'.
{
    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bool anyObjectMemoryAllocatedFlag = false;  // We later check that this
                                                // test allocates some object
                                                // memory.

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_lineNum;
        const char *const SPEC = DATA[ti].d_spec_p;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj        mZ(0, Hash(), Equal(), &scratch);
        const Obj& Z = gg(&mZ, SPEC);

        Obj        mZZ(0, Hash(), Equal(), &scratch);
        const Obj& ZZ = gg(&mZZ, SPEC);

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr          = new (fa) Obj(Z);
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr          = new (fa) Obj(Z, 0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr          = new (fa) Obj(Z, &sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
                continue;
              } break;
            }
            LOOP3_ASSERT(id, LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

            Obj& mX = *objPtr;  const Obj& X = mX;

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(id,
                             LINE,
                             CONFIG,
                             Obj(0, Hash(), Equal()) == *objPtr);
                firstFlag = false;
            }

            // Verify the value of the object.

            LOOP3_ASSERT(id, LINE, CONFIG, X == Z);

            // Verify that the value of 'Z' has not changed.

            LOOP3_ASSERT(id, LINE, CONFIG, ZZ == Z);

            // Also apply the object's 'allocator' accessor, as well as that of
            // 'Z'.

            LOOP3_ASSERT(id, LINE, CONFIG, &oa == X.allocator());

            LOOP3_ASSERT(id, LINE, CONFIG, &scratch == Z.allocator());

            // Verify no allocation from the non-object allocator.

            LOOP3_ASSERT(id, LINE, CONFIG, 0 == noa.numBlocksTotal());

            // Verify memory is always allocated except when default
            // constructed (SPEC == "").

            if (0 == ti) {
                LOOP3_ASSERT(id, LINE, CONFIG, 0 == oa.numBlocksInUse());
            }
            else {
                LOOP3_ASSERT(id, LINE, CONFIG, 0 != oa.numBlocksInUse());
            }

            // Record if some object memory was allocated.

            anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP3_ASSERT(id, LINE, CONFIG, 0 == fa.numBlocksInUse());
            LOOP3_ASSERT(id, LINE, CONFIG, 0 == da.numBlocksInUse());
            LOOP3_ASSERT(id, LINE, CONFIG, 0 == sa.numBlocksInUse());
        }  // end foreach configuration
    }  // end foreach row

    // Double check that at least some object memory got allocated.

    ASSERT(anyObjectMemoryAllocatedFlag);

    // Note that memory should be independently allocated for each attribute
    // capable of allocating memory.

    for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE = DATA[ti].d_lineNum;
        const char *const SPEC = DATA[ti].d_spec_p;

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj        mZ(0, Hash(), Equal(), &scratch);
        const Obj& Z = gg(&mZ, SPEC);

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

            Obj obj(Z, &oa);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        LOOP2_ASSERT(id, LINE, !da.numBlocksInUse());
        LOOP2_ASSERT(id, LINE, !oa.numBlocksInUse());
    }
}

template <class HASH>
void testCase2InsertHash(int id)
    // Address the 'insert' hash concerns of test case 2 for the specified 'id'
    // value.  Note that, in case of a test failure, 'id' can be used to
    // determine 'ENTRY'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef TestEntryUtil<int>                                    EntryUtil;
    typedef bsl::equal_to<int>                                    Equal;
    typedef bdlc::FlatHashTable<int, int, EntryUtil, HASH, Equal> Obj;

    bsl::pair<typename Obj::iterator, bool> rv;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    bsls::Types::Int64 expAllocations = 0;
    bsl::size_t        expCapacity    = 0;

    Obj mX(0, HASH(), Equal(), &oa);  const Obj& X = mX;

    LOOP_ASSERT(id, expAllocations == oa.numAllocations());

    for (int i = 0; i <= thirdAlloc; ++i) {
        rv = mX.insert(i);
        LOOP2_ASSERT(id, i,    i == EntryUtil::key(*rv.first));
        LOOP2_ASSERT(id, i, true == rv.second);

        rv = mX.insert(i);
        LOOP2_ASSERT(id, i,     i == EntryUtil::key(*rv.first));
        LOOP2_ASSERT(id, i, false == rv.second);

        if (thirdAlloc == i) {
            expCapacity *= 2;
        }
        else if (secondAlloc == i) {
            expCapacity *= 2;
        }
        else if (0 == i) {
            expCapacity = 2 * k_SIZE;
        }

        LOOP2_ASSERT(id, i, expCapacity == X.capacity());
        LOOP2_ASSERT(id, i,       i + 1 == static_cast<int>(X.size()));

        bsl::size_t errorIndex;
        LOOP2_ASSERT(id,
                     i,
                     IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
    }
}

template <class ENTRY>
void testCase2CopyInsert(int id, bool allocates, bool allocatesOnRehash)
    // Address the copy 'insert' concerns of test case 2 for the specified 'id'
    // value, for a type that allocates if the specified 'allocates' is 'true',
    // and allocates on rehashes if the specified 'allocatesOnRehash' is
    // 'true'.  Note that, in case of a test failure, 'id' can be used to
    // determine 'ENTRY'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    bsl::pair<typename Obj::iterator, bool> rv;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
        bsls::Types::Int64 expAllocations = oa.numAllocations();
        bsl::size_t        expCapacity    = 0;

        Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

        IsValidGuard<Obj> guard(&X);

        for (int i = 0; i <= thirdAlloc; ++i) {
            bsls::ObjectBuffer<ENTRY> entry;
            bslma::ConstructionUtil::construct(entry.address(), &oa, i);

            bslma::DestructorGuard<ENTRY> guard(&entry.object());

            rv = mX.insert(entry.object());
            LOOP2_ASSERT(id, i,    i == EntryUtil::key(*rv.first));
            LOOP2_ASSERT(id, i, true == rv.second);

            rv = mX.insert(entry.object());
            LOOP2_ASSERT(id, i,     i == EntryUtil::key(*rv.first));
            LOOP2_ASSERT(id, i, false == rv.second);

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
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class ENTRY>
void testCase2MoveInsert(int id, bool allocates, bool moveable)
    // Address the move 'insert' concerns of test case 2 for the specified 'id'
    // value, for a type that allocates if the specified 'allocates' is 'true',
    // and can be moved if the specified 'moveable' is 'true'.  Note that, in
    // case of a test failure, 'id' can be used to determine 'ENTRY'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    bsl::pair<typename Obj::iterator, bool> rv;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
        Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

        IsValidGuard<Obj> guard(&X);

        bsls::Types::Int64 expAllocations = oa.numAllocations();
        bsl::size_t        expCapacity    = 0;

        for (int i = 0; i <= thirdAlloc; ++i) {
            bsls::ObjectBuffer<ENTRY> entry1;
            bslma::ConstructionUtil::construct(entry1.address(), &oa, i);

            bslma::DestructorGuard<ENTRY> guard1(&entry1.object());

            bsls::ObjectBuffer<ENTRY> entry2;
            bslma::ConstructionUtil::construct(entry2.address(), &oa, i);

            bslma::DestructorGuard<ENTRY> guard2(&entry2.object());

            rv = mX.insert(bslmf::MovableRefUtil::move(entry1.object()));
            LOOP2_ASSERT(id, i,    i == EntryUtil::key(*rv.first));
            LOOP2_ASSERT(id, i, true == rv.second);

            rv = mX.insert(bslmf::MovableRefUtil::move(entry2.object()));
            LOOP2_ASSERT(id, i,     i == EntryUtil::key(*rv.first));
            LOOP2_ASSERT(id, i, false == rv.second);

            if (allocates) {
                if (   moveable
                    && (   !bslmf::IsBitwiseMoveable<ENTRY>::value
                        || bsl::is_trivially_copyable<ENTRY>::value)) {
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
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class ENTRY>
void testCase2ClearAndReset(int id)
    // Address the 'clear' and 'reset' concerns of test case 2 for the
    // specified 'id' value.  Note that, in case of a test failure, 'id' can be
    // used to determine 'ENTRY'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    bsl::pair<typename Obj::iterator, bool> rv;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    for (int i = 0; i <= thirdAlloc; ++i) {
        Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

        for (int j = 0; j <= i; ++j) {
            bsls::ObjectBuffer<ENTRY> entry;
            bslma::ConstructionUtil::construct(entry.address(), &oa, j);

            rv = mX.insert(entry.object());
            LOOP2_ASSERT(id, i,    j == EntryUtil::key(*rv.first));
            LOOP2_ASSERT(id, i, true == rv.second);

            entry.object().~ENTRY();
        }

        mX.clear();

        LOOP2_ASSERT(id, i, 0 == X.size());

        for (bsl::size_t j = 0; j < X.capacity(); ++j) {
            LOOP2_ASSERT(id, i, k_EMPTY == X.controls()[j]);
        }

        if (i >= thirdAlloc) {
            LOOP2_ASSERT(id, i, 8 * k_SIZE == X.capacity());
        }
        else if (i >= secondAlloc) {
            LOOP2_ASSERT(id, i, 4 * k_SIZE == X.capacity());
        }
        else {
            LOOP2_ASSERT(id, i, 2 * k_SIZE == X.capacity());
        }

        bsl::size_t errorIndex;
        LOOP2_ASSERT(id,
                     i,
                     IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
    }
    for (int i = 0; i <= thirdAlloc; ++i) {
        Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

        for (int j = 0; j <= i; ++j) {
            bsls::ObjectBuffer<ENTRY> entry;
            bslma::ConstructionUtil::construct(entry.address(), &oa, j);

            rv = mX.insert(entry.object());
            LOOP2_ASSERT(id, i,    j == EntryUtil::key(*rv.first));
            LOOP2_ASSERT(id, i, true == rv.second);

            entry.object().~ENTRY();
        }

        mX.reset();

        LOOP2_ASSERT(id, i, 0 == X.size());
        LOOP2_ASSERT(id, i, 0 == X.capacity());
        LOOP2_ASSERT(id, i, 0 == X.controls());
        LOOP2_ASSERT(id, i, 0 == X.entries());
    }
}

template <class ENTRY>
void testCase2EraseAndReInsert(int id)
    // Address the 'erase' and 'insert' after 'erase' concerns of test case 2
    // for the specified 'id' value.  Note that, in case of a test failure,
    // 'id' can be used to determine 'ENTRY'.
{
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    typedef TestEntryUtil<ENTRY>                                    EntryUtil;
    typedef IntValueIsHash                                          Hash;
    typedef bsl::equal_to<int>                                      Equal;
    typedef bdlc::FlatHashTable<int, ENTRY, EntryUtil, Hash, Equal> Obj;

    bsl::pair<typename Obj::iterator, bool> rv;

    const int secondAlloc = 2 * 7 * (k_SIZE / 8);
    const int thirdAlloc  = 4 * 7 * (k_SIZE / 8);

    for (int i = 0; i <= thirdAlloc; ++i) {
        for (int key = 0; key <= i; ++key) {
            // construct the object
            Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;
            for (int j = 0; j <= i; ++j) {
                bsls::ObjectBuffer<ENTRY> entry;
                bslma::ConstructionUtil::construct(entry.address(), &oa, j);

                rv = mX.insert(entry.object());
                LOOP2_ASSERT(id, i,    j == EntryUtil::key(*rv.first));
                LOOP2_ASSERT(id, i, true == rv.second);

                entry.object().~ENTRY();
            }

            // store 'controls' for later comparison
            bsl::uint8_t originalControls[128];
            bsl::memcpy(originalControls, X.controls(), X.capacity());

            // erase the 'key'
            bsl::size_t numErased = mX.erase(key);

            LOOP2_ASSERT(id, i, 1 == numErased);
            LOOP2_ASSERT(id, i, i == static_cast<int>(X.size()));

            // verify 'controls' changes as expected
            for (bsl::size_t j = 0; j < X.capacity(); ++j) {
                if (originalControls[j] != X.controls()[j]) {
                    LOOP2_ASSERT(id,
                                 i,
                                 (Hash()(key) & 0x7f) == originalControls[j]);
                    LOOP2_ASSERT(id, i,      k_ERASED == X.controls()[j]);
                }
            }

            // ensure table is valid
            bsl::size_t errorIndex;
            LOOP2_ASSERT(id,
                         i,
                         IsValidResult::e_SUCCESS == isValid(&errorIndex, X));

            // attempt to 'erase' the 'key' again
            numErased = mX.erase(key);

            LOOP2_ASSERT(id, i, 0 == numErased);
            LOOP2_ASSERT(id, i, i == static_cast<int>(X.size()));

            // re-insert the erase element, should occupy erased entry and not
            // cause a rehash
            {
                bsls::ObjectBuffer<ENTRY> entry;
                bslma::ConstructionUtil::construct(entry.address(), &oa, key);

                rv = mX.insert(entry.object());
                LOOP2_ASSERT(id, i,  key == EntryUtil::key(*rv.first));
                LOOP2_ASSERT(id, i, true == rv.second);

                entry.object().~ENTRY();
            }

            LOOP2_ASSERT(id, i, i + 1 == static_cast<int>(X.size()));

            if (i >= thirdAlloc) {
                LOOP2_ASSERT(id, i, 8 * k_SIZE == X.capacity());
            }
            else if (i >= secondAlloc) {
                LOOP2_ASSERT(id, i, 4 * k_SIZE == X.capacity());
            }
            else {
                LOOP2_ASSERT(id, i, 2 * k_SIZE == X.capacity());
            }

            LOOP2_ASSERT(id,
                         i,
                         IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
        }
    }
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

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
      case 21: {
        // --------------------------------------------------------------------
        // {DRQS 167125039} BASIC OPERATIONS OF MOVED-TO TABLES
        //
        // Ensure that the key-based accessors and basic manipulators operate
        // as expected on moved-to objects.  This test case was created in
        // order to verify the fix for the regression identified in
        // {DRQS 167125039}.  A change made to 'bdlc::FlatHashTable' introduced
        // a regression in which lookup, insert, and erase operations applied
        // to 'bdlc::FlatHashTable' objects that were move-constructed or
        // move-assigned from a hash table having a different allocator could
        // return incorrect results or trigger segmentation faults.
        //
        // In the concerns below, 'X' is a placeholder used to refer to a
        // 'bdlc::FlatHashTable' object that has been move-constructed or
        // move-assigned from another 'bdlc::FlatHashTable', 'Y'.  Each concern
        // holds for the case when the allocators of 'X' and 'Y' are the same,
        // and when they differ.
        //
        // Concerns:
        //: 1 'X' is in a valid state.
        //:
        //: 2 The key-based accessors of 'X': 'contains', 'count',
        //:   'equal_range', and 'find', produce the expected return value and
        //:   leave 'X' in a valid state.  In no event do these accessors
        //:   return invalid results or lead to undefined behavior when used in
        //:   contract.
        //:
        //: 3 The copy and move 'insert' methods on 'X' produce the expected
        //:   return value and leaves 'X' in a valid state.  Inserting an entry
        //:   already present in 'X' results in a no-op.
        //:
        //: 4 The 'erase(key)' method on 'X' produces the expected return value
        //:   and leaves 'X' in a valid state.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..5)
        //:
        //:   1 Create a new object 'Y' moved from 'V' in one of 4 ways: via
        //:     move construction with the same allocator, move construction
        //:     with a different allocator, move assignment with the same
        //:     allocator, and move assignment with a different allocator.
        //:     With a fresh 'Y' in this state, perform each of the following:
        //:
        //:     1 Verify that the internal state of the 'Y' is valid (C-1)
        //:
        //:     2 Verify that each of the key-based accessors of 'Y':
        //:      'contains', 'count', 'equal_range', and 'find', produce the
        //:      expected results. (C-2)
        //:
        //:     3 Verify that the copy and move 'insert' methods leave 'Y' in a
        //:       valid state and produce the expected results (C-3).
        //:
        //:     5 Verify that the 'erase(key)' method leave 'Y' in a valid
        //:       state and produce the expected results (C-4).
        //
        // Testing
        //   {DRQS 167125039} BASIC OPERATIONS OF MOVED-TO TABLES
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "{DRQS 167125039} BASIC OPERATIONS OF MOVED-TO TABALES"
                 << endl
                 << "====================================================="
                 << endl;

        if (verbose)
            cout << "Testing key-based accessors and basic"
                 << " manipulators of moved-to tables." << endl;

        testCase21OperationsWhenMoved<int>(0);
        testCase21OperationsWhenMoved<bsltf::AllocBitwiseMoveableTestType>(1);
        testCase21OperationsWhenMoved<bsltf::AllocTestType>(2);
        testCase21OperationsWhenMoved<bsltf::BitwiseCopyableTestType>(3);
        testCase21OperationsWhenMoved<bsltf::BitwiseMoveableTestType>(4);
        testCase21OperationsWhenMoved<bsltf::MovableAllocTestType>(5);
        testCase21OperationsWhenMoved<bsltf::MovableTestType>(6);
        testCase21OperationsWhenMoved<bsltf::NonDefaultConstructibleTestType>(
                                                                            7);
        testCase21OperationsWhenMoved<bsltf::NonOptionalAllocTestType>(8);

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // 'operator[]'
        //
        // Ensure the 'operator[]' method operates as expected.
        //
        // Concerns:
        //: 1 The copy 'operator[]' produces the expected return value,
        //:   results in a valid state, increases capacity as needed, honors
        //:   bitwise-copy/bitwise-move/move, is exception neutral with respect
        //:   to memory allocation, and uses the supplied allocator.
        //:
        //: 2 The move 'operator[]' produces the expected return value,
        //:   results in a valid state, increases capacity as needed, honors
        //:   bitwise-copy/bitwise-move/move, is exception neutral with respect
        //:   to memory allocation, and uses the supplied allocator.
        //:
        //: 3 The returned reference refers to the correct entry and is
        //:   modifiable.
        //:
        //: 4 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //
        // Plan:
        //: 1 Create objects 'X' of varying value using the
        //:   'bslma::TestAllocator'.  Modify 'X' with the copy 'operator[]'
        //:   and verify the return referred to object has the correct key
        //:   value.  By using various 'bsltf' test types and verifying the
        //:   number of allocations, confirm bitwise-copy/bitwise-move/move are
        //:   honored.  Use the 'isValid' global method to ensure 'X' is a
        //:   valid table.  Also vary the test allocator's allocation limit to
        //:   verify behavior in the presence of exceptions.  (C-1)
        //:
        //: 2 Create objects 'X' of varying value using the
        //:   'bslma::TestAllocator'.  Modify 'X' with the move 'operator[]'
        //:   and verify the return referred to object has the correct key
        //:   value.  By using various 'bsltf' test types and verifying the
        //:   number of allocations, confirm bitwise-copy/bitwise-move/move are
        //:   honored.  Use the 'isValid' global method to ensure 'X' is a
        //:   valid table.  Also vary the test allocator's allocation limit to
        //:   verify behavior in the presence of exceptions.  (C-2)
        //:
        //: 3 Create a table with an 'int' key and a
        //:   'bsl::pair<const int, int>' entry.  Verify the returned
        //:   modifiable reference can modify the second value in the pair
        //:   without otherwise affecting the table.  (C-3)
        //:
        //: 4 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-4)
        //
        // Testing:
        //   ENTRY& operator[](FORWARD_REF(KEY_TYPE) key);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'operator[]'" << endl
                          << "============" << endl;

        if (verbose) cout << "Testing copy 'operator[]'." << endl;
        {
            testCase20Bracket<int>(0, false, false);
            testCase20Bracket<bsltf::AllocBitwiseMoveableTestType>(1,
                                                                   true,
                                                                   false);
            testCase20Bracket<bsltf::AllocTestType>(2, true, true);
            testCase20Bracket<bsltf::BitwiseCopyableTestType>(3, false, false);
            testCase20Bracket<bsltf::BitwiseMoveableTestType>(4, false, false);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            testCase20Bracket<bsltf::MovableAllocTestType>(5, true, false);
#else
            testCase20Bracket<bsltf::MovableAllocTestType>(5, true, true);
#endif

            testCase20Bracket<bsltf::MovableTestType>(6, false, false);
            testCase20Bracket<bsltf::NonDefaultConstructibleTestType>(7,
                                                                      false,
                                                                      false);
            testCase20Bracket<bsltf::NonOptionalAllocTestType>(8, true, true);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) cout << "Testing move 'operator[]'." << endl;
        {
            testCase20BracketMove<bsltf::AllocBitwiseMoveableTestType>(0,
                                                                       true,
                                                                       true);
            testCase20BracketMove<bsltf::AllocTestType>(1, true, false);
            testCase20BracketMove<bsltf::BitwiseCopyableTestType>(2,
                                                                  false,
                                                                  true);
            testCase20BracketMove<bsltf::BitwiseMoveableTestType>(3,
                                                                  false,
                                                                  true);
            testCase20BracketMove<bsltf::MovableAllocTestType>(4, true, true);
            testCase20BracketMove<bsltf::MovableTestType>(5, false, true);

            testCase20BracketMove<bsltf::MoveOnlyAllocTestType>(6, true, true);

            testCase20BracketMove<bsltf::NonDefaultConstructibleTestType>(
                                                                        7,
                                                                        false,
                                                                        false);
            testCase20BracketMove<bsltf::NonOptionalAllocTestType>(8,
                                                                 true,
                                                                 false);
        }
#endif

        if (verbose) cout << "Testing returned reference." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            typedef bdlc::FlatHashTable<
                                     int,
                                     bsl::pair<const int, int>,
                                     TestEntryUtil<bsl::pair<const int, int> >,
                                     IntValueIsHash,
                                     bsl::equal_to<int> > IntPairObj;

            IntPairObj mX(0, IntValueIsHash(), bsl::equal_to<int>(), &oa);
            const IntPairObj& X = mX;

            IntPairObj::entry_type& rv1 = mX[0];

            ASSERT(0 == rv1.first);
            ASSERT(0 == rv1.second);

            rv1.second = 1;

            ASSERT(0 == rv1.first);
            ASSERT(1 == rv1.second);
            ASSERT(0 == X.begin()->first);
            ASSERT(1 == X.begin()->second);

            ASSERT(&rv1 == &*X.find(0));

            IntPairObj::entry_type& rv2 = mX[1];

            ASSERT(1 == rv2.first);
            ASSERT(0 == rv2.second);

            rv2.second = 2;

            ASSERT(1 == rv2.first);
            ASSERT(2 == rv2.second);
            ASSERT(1 == (++X.begin())->first);
            ASSERT(2 == (++X.begin())->second);

            ASSERT(&rv2 == &*X.find(1));

            bsl::size_t errorIndex;
            ASSERT(IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // 'rehash' AND 'reserve'
        //
        // Ensure the 'rehash' and 'reserve' methods operate as expected.
        //
        // Concerns:
        //: 1 A 'rehash' does not change the value of the table.
        //:
        //: 2 After a 'rehash', the capacity is zero or a power of two greater
        //:   than or equal to the minimum capacity and the specified capacity.
        //:   If the capacity is zero, the table is in the zero-capacity state.
        //:
        //: 3 After a 'rehash', the 'load_factor' is less than or equal to
        //:   'max_load_factor'.
        //:
        //: 4 After a 'rehash', there are no erased entries.
        //:
        //: 5 Any memory allocation is exception neutral.
        //:
        //: 6 The 'reserve' method correctly forward to the 'rehash' method.
        //
        // Plan:
        //: 1 Using the table-driven technique, create objects 'X' and 'Z'
        //:   having the same generated value for a variety of object values.
        //:   For a set of capacities, invoke 'rehash' on 'X', verify the
        //:   capacity meets expectations, and verify the object invariants.
        //:   Verify 'X == Z'.  Use 'controls' to verify there are no erased
        //:   values in 'X'.  Vary the test allocator's allocation limit to
        //:   verify behavior in the presence of exceptions.  Finally, invoke
        //:   'X.clear()' and 'X.rehash(0) and verify the table is in the
        //:   zero-capacity state.  (C-1..5)
        //:
        //: 2 Invoke 'reserve' on an empty object with a variety of values and
        //:   directly verify the 'capacity' after each invocation.  (C-1..6)
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
            testCase19Rehash<int>(0);
            testCase19Rehash<bsltf::AllocBitwiseMoveableTestType>(1);
            testCase19Rehash<bsltf::AllocTestType>(2);
            testCase19Rehash<bsltf::BitwiseCopyableTestType>(3);
            testCase19Rehash<bsltf::BitwiseMoveableTestType>(4);
            testCase19Rehash<bsltf::MovableAllocTestType>(5);
            testCase19Rehash<bsltf::MovableTestType>(6);
            testCase19Rehash<bsltf::NonDefaultConstructibleTestType>(7);
            testCase19Rehash<bsltf::NonOptionalAllocTestType>(8);
        }

        if (verbose) cout << "Testing 'reserve'." << endl;
        {
            typedef TestEntryUtil<int>                               Util;
            typedef IntValueIsHash                                   Hash;
            typedef bsl::equal_to<int>                               Equal;
            typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

            mX.reserve(0);

            ASSERT(0 == X.capacity());

            mX.reserve(16);

            ASSERT(32 == X.capacity());

            mX.insert(0);

            mX.reserve(0);

            ASSERT(2 * k_SIZE == X.capacity());
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // RANGE 'erase'
        //
        // Ensure the range 'erase' method operates as expected.
        //
        // Concerns:
        //: 1 The range 'erase' method correctly forwards to 'erase'.
        //:
        //: 2 The range 'erase' return value is always the second iterator
        //:   value.
        //:
        //: 3 The returned iterator is modifiable.
        //
        // Plan:
        //: 1 Using the table-driven technique, for a set of identically
        //:   constructed object values 'X' and 'Z', verify the results of
        //:   range 'erase' for 'X.erase(X.begin(), X.begin())',
        //:   'X.erase(X.end(), X.end())',
        //:   'mX.erase(mX.begin(), ++mX.begin())', and
        //:   'X.erase(X.begin(), X.end())' using 'Z' initially as a control
        //:   value and later as an oracle for the table value resulting from
        //:   'Z.erase(Z.begin())'.  (C-1,2)
        //:
        //: 2 Create a table with an 'int' key and a
        //:   'bsl::pair<const int, int>' entry.  Verify the returned
        //:   modifiable reference can modify the second value in the pair
        //:   without otherwise affecting the table.  (C-3)
        //
        // Testing:
        //   iterator erase(const_iterator, const_iterator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RANGE 'erase'" << endl
                          << "=============" << endl;

        const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        typedef TestEntryUtil<int>                               Util;
        typedef IntValueIsHash                                   Hash;
        typedef bsl::equal_to<int>                               Equal;
        typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

        if (verbose) cout << "Testing range 'erase'." << endl;
        {
            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE = DATA[ti].d_lineNum;
                const char *const    SPEC = DATA[ti].d_spec_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj        mZ(0, Hash(), Equal(), &oa);
                const Obj& Z = gg(&mZ, SPEC);

                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC);

                Obj::iterator rv;

                rv = mX.erase(X.begin(), X.begin());
                LOOP_ASSERT(LINE,  X == Z);
                LOOP_ASSERT(LINE, rv == X.begin());

                rv = mX.erase(X.end(), X.end());
                LOOP_ASSERT(LINE, X == Z);
                LOOP_ASSERT(LINE, rv == X.end());

                if (!X.empty()) {
                    Obj::const_iterator endIter;
                    endIter = X.begin();
                    ++endIter;

                    rv = mX.erase(X.begin(), endIter);
                    mZ.erase(Z.begin());
                    LOOP_ASSERT(LINE, X == Z);
                    LOOP_ASSERT(LINE, rv == endIter);
                }

                rv = mX.erase(X.begin(), X.end());
                LOOP_ASSERT(LINE, X.empty());
                LOOP_ASSERT(LINE, rv == X.end());
            }
        }

        if (verbose) cout << "Testing returned iterator." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            typedef bdlc::FlatHashTable<
                                     int,
                                     bsl::pair<const int, int>,
                                     TestEntryUtil<bsl::pair<const int, int> >,
                                     IntValueIsHash,
                                     bsl::equal_to<int> > IntPairObj;

            IntPairObj mX(0, IntValueIsHash(), bsl::equal_to<int>(), &oa);
            const IntPairObj& X = mX;

            mX.insert(bsl::make_pair(0, 0));
            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(2, 2));

            IntPairObj::iterator rv1 = mX.erase(X.begin(), ++X.begin());

            ASSERT(rv1 == X.find(1));

            rv1->second = 0;

            ASSERT(1 == X.begin()->first);
            ASSERT(0 == X.begin()->second);

            IntPairObj::iterator rv2 = mX.erase(X.begin(), ++X.begin());

            ASSERT(rv2 == X.find(2));

            rv2->second = 1;

            ASSERT(2 == X.begin()->first);
            ASSERT(1 == X.begin()->second);

            bsl::size_t errorIndex;
            ASSERT(IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // ITERATOR 'erase'
        //
        // Ensure the iterator 'erase' methods operates as expected.
        //
        // Concerns:
        //: 1 The iterator 'erase' methods return the next available entry, or
        //:   'end()' if the erased entry was the only entry.
        //:
        //: 2 The iterator 'erase' methods set the 'controls' value at the
        //:   position to 'k_ERASED'.
        //:
        //: 3 The resultant object has the correct value.
        //:
        //: 4 The erased object is correctly destroyed.
        //:
        //: 5 The returned iterator is modifiable.
        //
        // Plan:
        //: 1 Using the table-driven technique, for a set of object values 'Z'
        //:   verify the results of the iterator 'erase' methods on an
        //:   identically constructed object 'X', by using the key 'erase'
        //:   method on an identically constructed object 'EXP' for all
        //:   entries in 'Z'.  Directly verify the return value and 'controls'
        //:   position.  (C-1..3)
        //:
        //: 2 Use a variety of testing types and verify all allocated memory
        //:   is returned, by allowing the used test allocator to go out of
        //:   scope.  (C-4)
        //:
        //: 3 Create a table with an 'int' key and a
        //:   'bsl::pair<const int, int>' entry.  Verify the returned
        //:   modifiable reference can modify the second value in the pair
        //:   without otherwise affecting the table.  (C-5)
        //
        // Testing:
        //   iterator erase(const_iterator);
        //   iterator erase(iterator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ITERATOR 'erase'" << endl
                          << "================" << endl;

        if (verbose) cout << "Testing iterator 'erase'." << endl;
        {
            testCase17IteratorErase<int>(0);
            testCase17IteratorErase<bsltf::AllocBitwiseMoveableTestType>(1);
            testCase17IteratorErase<bsltf::AllocTestType>(2);
            testCase17IteratorErase<bsltf::BitwiseCopyableTestType>(3);
            testCase17IteratorErase<bsltf::BitwiseMoveableTestType>(4);
            testCase17IteratorErase<bsltf::MovableAllocTestType>(5);
            testCase17IteratorErase<bsltf::MovableTestType>(6);
            testCase17IteratorErase<bsltf::NonDefaultConstructibleTestType>(7);
            testCase17IteratorErase<bsltf::NonOptionalAllocTestType>(8);
        }

        if (verbose) cout << "Testing returned iterator." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            typedef bdlc::FlatHashTable<
                                     int,
                                     bsl::pair<const int, int>,
                                     TestEntryUtil<bsl::pair<const int, int> >,
                                     IntValueIsHash,
                                     bsl::equal_to<int> > IntPairObj;

            IntPairObj mX(0, IntValueIsHash(), bsl::equal_to<int>(), &oa);
            const IntPairObj& X = mX;

            mX.insert(bsl::make_pair(0, 0));
            mX.insert(bsl::make_pair(1, 1));
            mX.insert(bsl::make_pair(2, 2));

            IntPairObj::iterator rv1 = mX.erase(X.begin());

            ASSERT(rv1 == X.find(1));

            rv1->second = 0;

            ASSERT(1 == X.begin()->first);
            ASSERT(0 == X.begin()->second);

            IntPairObj::iterator rv2 = mX.erase(X.begin());

            ASSERT(rv2 == X.find(2));

            rv2->second = 1;

            ASSERT(2 == X.begin()->first);
            ASSERT(1 == X.begin()->second);

            bsl::size_t errorIndex;
            ASSERT(IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // RANGE 'insert'
        //
        // Ensure the range 'insert' method operates as expected.
        //
        // Concerns:
        //: 1 The range 'insert' method correctly forwards to 'insert'.
        //:
        //: 2 The range 'insert' method can accept iterators from a differnt
        //:   container type.
        //
        // Plan:
        //: 1 Using the table-driven technique, for a set of object values 'Z'
        //:   verify the results of range 'insert' into an empty object 'X' for
        //:   'X.insert(Z.begin(), Z.begin())', 'X.insert(Z.end(), Z.end())',
        //:   and 'X.insert(X.begin(), X.end())'.  Note that the 'DATA' table
        //:   includes entries of various lengths.  (C-1)
        //:
        //: 2 Create a 'bsl::vector' with three entries, 'Z', and verify the
        //:   results of range 'insert' into an empty object 'X' for
        //:   'X.insert(Z.begin(), Z.begin())', 'X.insert(Z.end(), Z.end())',
        //:   and 'X.insert(X.begin(), X.end())'.  Note that the 'DATA' table
        //:   includes entries of various lengths.  (C-1)
        //
        // Testing:
        //   void insert(INPUT_IT, INPUT_IT);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'RANGE 'insert'" << endl
                          << "===============" << endl;

        const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        typedef TestEntryUtil<int>                               Util;
        typedef IntValueIsHash                                   Hash;
        typedef bsl::equal_to<int>                               Equal;
        typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

        if (verbose) cout << "Testing range 'insert'." << endl;
        {
            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE = DATA[ti].d_lineNum;
                const char *const    SPEC = DATA[ti].d_spec_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj        mZ(0, Hash(), Equal(), &oa);
                const Obj& Z = gg(&mZ, SPEC);

                Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

                mX.insert(Z.begin(), Z.begin());
                LOOP_ASSERT(LINE, X.empty());

                mX.insert(Z.end(), Z.end());
                LOOP_ASSERT(LINE, X.empty());

                mX.insert(Z.begin(), Z.end());
                LOOP_ASSERT(LINE, X == Z);

                bsl::size_t errorIndex;
                ASSERT(IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
            }
        }

        if (verbose) cout << "Testing inserting from a 'bsl::vector'." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            bsl::vector<int> mZ(&oa);  const bsl::vector<int>& Z = mZ;

            mZ.push_back(1);
            mZ.push_back(2);
            mZ.push_back(3);

            Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

            mX.insert(Z.begin(), Z.begin());
            ASSERT(X.empty());

            mX.insert(Z.end(), Z.end());
            ASSERT(X.empty());

            mX.insert(Z.begin(), Z.end());
            ASSERT(3 == X.size());
            ASSERT(1 == *X.find(1));
            ASSERT(2 == *X.find(2));
            ASSERT(3 == *X.find(3));

            bsl::size_t errorIndex;
            ASSERT(IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // MOVE ASSIGNMENT OPERATOR
        //   Ensure that we can move assign the value of any object of the
        //   class to any object of the class.
        //
        // Concerns:
        //: 1 The move assignment operator can change the value of any target
        //:   object to that of any source object.
        //:
        //: 2 The address of the target object's allocator is unchanged.
        //:
        //: 3 There is no memory allocations when the source and destination
        //:   allocators are identical.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The source object is valid after the move assignment.
        //:
        //: 7 The allocator of the source object is not modified.
        //:
        //: 8 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 The resultant value of the target object is as expected.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a function pointer
        //:   having the appropriate structure for the move assignment operator
        //:   defined in this component.  (C-4)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of valid object values (one per row) in terms of
        //:     their individual attributes, including (a) first, the default
        //:     value, (b) boundary values corresponding to every range of
        //:     values that each individual attribute can independently attain,
        //:     and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3: (C-1..3, 5..9)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):
        //:
        //:     1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       using 'oa' and having the value 'W'.
        //:
        //:     3 Use the equality comparison operators to verify that the two
        //:       objects, 'mX' and 'Z', are initially equal if and only if the
        //:       table indices from P-4 and P-4.2 are the same.
        //:
        //:     4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality comparison operators to verify that: (C-1,
        //:       6)
        //:
        //:       1 The target object, 'mX', now has the same value as 'ZZ'.
        //:
        //:       2 'Z' has a valid object value.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that both object allocators are unchanged.  (C-2, 7)
        //:
        //:     8 Use appropriate test allocators to verify that: (C-3, 8)
        //:
        //:       1 No additional memory is allocated by the source object.
        //:
        //:       2 All object memory is released when the object is destroyed.
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3: (C-1..3, 5..10)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj' 'mX'
        //:     using 'oa' and a 'const' 'Obj' 'ZZ' (using a distinct "scratch"
        //:     allocator).
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Use the equality comparison operators to verify that the target
        //:     object, 'mX', has the same value as 'ZZ'.
        //:
        //:   5 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   6 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-5)
        //:
        //:   7 Use the equality comparison operators to verify that the target
        //:     object, 'mX', still has the same value as 'ZZ'.  (C-10)
        //:
        //:   8 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.  (C-2)
        //:
        //:   9 Use appropriate test allocators to verify that: (C-3, 8)
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   FlatHashTable& operator=(FlatHashTable&&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout << "Testing move assignment operator." << endl;
        {
            testCase15MoveAssignment<int>(0);
            testCase15MoveAssignment<bsltf::AllocBitwiseMoveableTestType>(1);
            testCase15MoveAssignment<bsltf::AllocTestType>(2);
            testCase15MoveAssignment<bsltf::BitwiseCopyableTestType>(3);
            testCase15MoveAssignment<bsltf::BitwiseMoveableTestType>(4);
            testCase15MoveAssignment<bsltf::MovableAllocTestType>(5);
            testCase15MoveAssignment<bsltf::MovableTestType>(6);
            testCase15MoveAssignment<bsltf::NonDefaultConstructibleTestType>(
                                                                            7);
            testCase15MoveAssignment<bsltf::NonOptionalAllocTestType>(8);
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR WITH ALLOCATOR
        //
        // Concerns:
        //: 1 The newly created object has the same value as that of the
        //:   original object before the call.
        //:
        //: 2 Any internal representation of a given value can be used to
        //:   create a new object of equivalent value.
        //:
        //: 3 The allocator is propagated to the newly created object if (and
        //:   only if) no allocator is specified in the call to the move
        //:   constructor.
        //:
        //: 4 The original object is always left in a valid state and the
        //:   allocator address held by the original object is unchanged.
        //:
        //: 5 Subsequent changes to or destruction of the original object have
        //:   no effect on the move-constructed object and vice-versa.
        //:
        //: 6 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 7 Every object releases any allocated memory at destruction.
        //
        //: 8 Any memory allocation is exception neutral (we have tested each
        //:   of the code paths of this constructor).
        //
        // Plan:
        //: 1 Specify a set 'S' of object values with substantial and varied
        //:   differences, ordered by increasing length, to be used
        //:   sequentially in the following tests; for each entry, create a
        //:   control object.   (C-2)
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
        //:   2 Ensure that the new original, and control object continue to
        //:     have the correct allocator and that all memory allocations come
        //:     from the appropriate allocator.  (C-3,6)
        //:
        //:   3 Verify the original object (after the move construction) to
        //:     is in a valid state, destroy it, and then verify newly created
        //:     is in a valid state.  (C-4,5)
        //:
        //:   4 Verify all memory is released when the object is destroyed.
        //:     (C-7)
        //:
        //: 4 Perform tests in the presence of exceptions during memory
        //:   allocations using a 'bslma::TestAllocator' and varying its
        //:   *allocation* *limit*.  (C-8)
        //
        // Testing:
        //   FlatHashTable(FlatHashTable&&, Allocator *basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE CONSTRUCTOR WITH ALLOCATOR" << endl
                          << "===============================" << endl;

        if (verbose) cout << "Testing move constructor with allocator."
                          << endl;
        {
            testCase14MoveConstructorWithAllocator<int>(0);
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
        //
        // Concerns:
        //: 1 The newly created object has the same value (using the equality
        //:   operator) as that of the original object before the call.
        //:
        //: 2 Any internal representation of a given value can be used to
        //:   create a new object of equivalent value, including the
        //:   zero-capacity state.
        //:
        //: 3 The original object is left in a valid state and its allocator is
        //:   unchanged.
        //:
        //: 4 Subsequent changes to or destruction of the original object have
        //:   no effect on the move-constructed object and vice-versa.
        //:
        //: 5 The constructor does not allocate memory.
        //:
        //: 6 The allocator is propagated to the newly created object.
        //
        // Plan:
        //: 1 Specify a set 'S' of object values with substantial and varied
        //:   differences, ordered by increasing length, to be used
        //:   sequentially in the following tests; for each entry, create a
        //:   control object.  (C-2)
        //:
        //: 2 For each of the object values (P-1), verify the following:
        //:
        //:   1 Verify the newly created object has the same value as that of
        //:     the original object before the call to the move constructor
        //:     (control value).
        //:
        //:   2 After the move construction, verify the newly created object
        //:     is equal to the control object, and ensure the original object
        //:     is in a valid state and its allocator is unchanged.  (C-1,3)
        //:
        //:   3 Destroy the original object, and then ensure the newly created
        //:     object is in a valid state and that its value is unchanged.
        //:     (C-4)
        //:
        //: 3 The allocators used to create the objects will be verified to
        //:   ensure that no memory was allocated during use of the constructor
        //:   (note that this check may be done at the global level).  (C-5)
        //:
        //: 4 Verify the allocator in the resultant object is as expected.
        //:   (C-6)
        //
        // Testing:
        //   FlatHashTable(FlatHashTable&&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE CONSTRUCTOR WITHOUT ALLOCATOR" << endl
                          << "==================================" << endl;

        if (verbose) cout << "Testing move constructor without allocator."
                          << endl;
        {
            testCase13MoveConstructorWithoutAllocator<int>(0);
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
        //
        // Ensure the search methods and iterators operate as expected.  Since
        // the iterators are implemented using 'bslstl::ForwardIterator',
        // verification that the iterators interpret object state correctly is
        // all that is required.
        //
        // Concerns:
        //: 1 The 'begin' iterator methods correctly determine the first
        //:   contained value.
        //:
        //: 2 Incrementing the 'begin' iterator 'size()' times visits all
        //:   contained elements and results in the 'end' iterator.
        //:
        //: 3 The dereference operator for iterators works as expected.
        //:
        //: 4 The comparison 'operator==' for iterators works as expected.
        //:
        //: 5 The accessors 'contains', 'count', 'equal_range', and 'find'
        //:   correctly reflect the presence of a key.
        //:
        //: 6 None of these accessor or iterator methods allocate memory.
        //
        // Plan:
        //: 1 Using the basic accessors, verify the 'begin' methods return an
        //:   iterator referencing the first populated entry and incrementing
        //:   the 'begin' iterator 'size()' times visits all contained elements
        //:   and results in the 'end' iterator.  For each visited element,
        //:   directly verify the result of the dereference operator.  (C-1,2)
        //:
        //: 2 Using the table-driven technique, create objects 'X' and 'Y'
        //:   having the same generated value.  For every iterator value
        //:   obtained by iterating over the objects, ensure 'operator==' is
        //:   'true' only when the object and iterator position are the same.
        //:   (C-4)
        //:
        //: 3 Create a table with an 'int' key and a
        //:   'bsl::pair<const int, int>' entry.  Verify the modifiable
        //:   iterator can modify the second value in the pair without
        //:   otherwise affecting the table.  (C-3)
        //:
        //: 4 Using the table-driven technique, compare the results of
        //:   'contains', 'count', 'equal_range', and 'find' to a direct search
        //:   for one of a set of key value using the basic accessors.  Use the
        //:   iterator methods to verify the results of 'equal_range' and
        //:   'find'.  (C-5)
        //:
        //: 5 The allocators used to create the objects will be verified to
        //:   ensure that no memory was allocated during use of the accessors
        //:   and iterators.  (C-6)
        //
        // Testing:
        //   bsl::pair<iterator, iterator> equal_range(const KEY&);
        //   iterator find(const KEY&);
        //   bool contains(const KEY&) const;
        //   bsl::size_t count(const KEY& key) const;
        //   bsl::pair<ci, ci> equal_range(const KEY&) const;
        //   const_iterator find(const KEY&) const;
        //   iterator begin();
        //   iterator end();
        //   const_iterator begin() const;
        //   const_iterator cbegin() const;
        //   const_iterator cend() const;
        //   const_iterator end() const;
        //   CONCERN: 'FHTCI FHTCI::operator++()'
        //   CONCERN: 'const ENTRY& FHTCI::operator*()'
        //   CONCERN: 'bool operator==(FHTCI&, FHTCI&)'
        //   CONCERN: 'ENTRY& FHTI::operator*()'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SEARCH METHODS AND ITERATORS" << endl
                          << "============================" << endl;

        const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        typedef TestEntryUtil<int>                               Util;
        typedef IntValueIsHash                                   Hash;
        typedef bsl::equal_to<int>                               Equal;
        typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

        if (verbose) cout << "Testing 'begin' and iterator methods." << endl;
        {
            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE = DATA[ti].d_lineNum;
                const char *const    SPEC = DATA[ti].d_spec_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC);

                Obj mY(X, &oa);  const Obj& Y = mY;

                bsls::Types::Int64 expNumAllocations = oa.numAllocations();

                if (0 == X.size()) {
                    LOOP_ASSERT(LINE, X.end() == mX.begin());
                    LOOP_ASSERT(LINE, X.end() == mX.end());
                    LOOP_ASSERT(LINE, X.end() == X.begin());
                    LOOP_ASSERT(LINE, X.end() == X.cbegin());
                    LOOP_ASSERT(LINE, X.end() == X.cend());
                }
                else {
                    Obj::const_iterator iter = X.begin();

                    LOOP_ASSERT(LINE, *iter == *mX.begin());
                    LOOP_ASSERT(LINE,  iter ==  mX.begin());

                    LOOP_ASSERT(LINE, *iter == *X.cbegin());
                    LOOP_ASSERT(LINE,  iter ==  X.cbegin());

                    bsl::size_t at = 0;
                    while (    k_EMPTY == X.controls()[at]
                           || k_ERASED == X.controls()[at]) {
                        ++at;
                    }

                    LOOP_ASSERT(LINE, *iter == X.entries()[at]);

                    ++at;
                    ++iter;
                    for (bsl::size_t i = 1; i < X.size(); ++i) {
                        while (    k_EMPTY == X.controls()[at]
                               || k_ERASED == X.controls()[at]) {
                            ++at;
                        }

                        LOOP2_ASSERT(LINE, at, *iter == X.entries()[at]);

                        ++at;
                        ++iter;
                    }

                    LOOP_ASSERT(LINE, iter == X.end());

                    // testing 'operator=='

                    bsl::size_t xAt = 0;

                    Obj::const_iterator xIter = X.begin();
                    Obj::const_iterator yIter = Y.begin();

                    for (bsl::size_t i = 0; i < X.size(); ++i) {
                        LOOP_ASSERT(LINE,  true == (xIter == xIter));
                        LOOP_ASSERT(LINE,  true == (yIter == yIter));
                        LOOP_ASSERT(LINE, false == (xIter == yIter));

                        bsl::size_t xxAt = 0;

                        Obj::const_iterator xxIter = X.begin();

                        while (xxIter != X.end()) {
                            LOOP3_ASSERT(LINE,
                                         xAt,
                                         xxAt,
                                         (xAt == xxAt) == (xIter == xxIter));
                            ++xxAt;
                            ++xxIter;
                        }
                        LOOP_ASSERT(LINE, xxIter == X.end());

                        ++xAt;
                        ++xIter;
                        ++yIter;
                    }
                    LOOP_ASSERT(LINE, xIter == X.end());
                }

                LOOP_ASSERT(LINE, expNumAllocations == oa.numAllocations());
            }

            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                typedef bdlc::FlatHashTable<
                                     int,
                                     bsl::pair<const int, int>,
                                     TestEntryUtil<bsl::pair<const int, int> >,
                                     bsl::hash<int>,
                                     bsl::equal_to<int> > IntPairObj;

                IntPairObj mX(0, bsl::hash<int>(), bsl::equal_to<int>(), &oa);
                const IntPairObj& X = mX;

                mX.insert(bsl::make_pair(0, 0));

                ASSERT(0 == X.begin()->first);
                ASSERT(0 == X.begin()->second);

                mX.begin()->second = 1;

                ASSERT(0 == X.begin()->first);
                ASSERT(1 == X.begin()->second);

                mX.begin()->second = 2;

                ASSERT(0 == X.begin()->first);
                ASSERT(2 == X.begin()->second);

                bsl::size_t errorIndex;
                ASSERT(IsValidResult::e_SUCCESS == isValid(&errorIndex, X));
            }
        }

        if (verbose) {
            cout << "Testing 'contains', 'count', 'equal_range', and 'find'."
                 << endl;
        }
        {
            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE = DATA[ti].d_lineNum;
                const char *const    SPEC = DATA[ti].d_spec_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC);

                bsls::Types::Int64 expNumAllocations = oa.numAllocations();

                for (int key = 0; key < 20; ++key) {
                    bool present = false;
                    for (bsl::size_t i = 0; i < X.capacity(); ++i) {
                        if (    k_EMPTY != X.controls()[i]
                            && k_ERASED != X.controls()[i]) {
                            if (key == X.entries()[i]) {
                                present = true;
                            }
                        }
                    }

                    LOOP2_ASSERT(LINE, key, present == X.contains(key));

                    LOOP2_ASSERT(LINE, key, (present ? 1 : 0) == X.count(key));

                    {
                        Obj::iterator rv = mX.find(key);
                        if (present) {
                            LOOP2_ASSERT(LINE, key,  rv != X.end());
                            LOOP2_ASSERT(LINE, key, *rv == key);
                        }
                        else {
                            LOOP2_ASSERT(LINE, key, rv == X.end());
                        }
                    }
                    {
                        bsl::pair<Obj::iterator,
                                  Obj::iterator> rv = mX.equal_range(key);
                        if (present) {
                            Obj::iterator exp = rv.first;
                            ++exp;

                            LOOP2_ASSERT(LINE, key,    rv.first != X.end());
                            LOOP2_ASSERT(LINE, key, *(rv.first) == key);
                            LOOP2_ASSERT(LINE, key,   rv.second == exp);
                        }
                        else {
                            LOOP2_ASSERT(LINE, key,  rv.first == X.end());
                            LOOP2_ASSERT(LINE, key, rv.second == X.end());
                        }
                    }
                    {
                        Obj::const_iterator rv = X.find(key);
                        if (present) {
                            LOOP2_ASSERT(LINE, key,  rv != X.end());
                            LOOP2_ASSERT(LINE, key, *rv == key);
                        }
                        else {
                            LOOP2_ASSERT(LINE, key, rv == X.end());
                        }
                    }
                    {
                        bsl::pair<Obj::const_iterator,
                                  Obj::const_iterator> rv = X.equal_range(key);
                        if (present) {
                            Obj::const_iterator exp = rv.first;
                            ++exp;

                            LOOP2_ASSERT(LINE, key,    rv.first != X.end());
                            LOOP2_ASSERT(LINE, key, *(rv.first) == key);
                            LOOP2_ASSERT(LINE, key,   rv.second == exp);
                        }
                        else {
                            LOOP2_ASSERT(LINE, key,  rv.first == X.end());
                            LOOP2_ASSERT(LINE, key, rv.second == X.end());
                        }
                    }
                }

                LOOP_ASSERT(LINE, expNumAllocations == oa.numAllocations());
            }
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
        //: 1 Using the table-driven technique, compare the results of 'empty'
        //:   and 'load_factor' to the value expected given 'size' and
        //:   'capacity' for a set of object values.  (C-1)
        //:
        //: 2 The allocators used to create the objects will be verified to
        //:   ensure that no memory was allocated during use of the accessors
        //:   and iterators.  (C-22)
        //
        // Testing:
        //   bool empty() const;
        //   float load_factor() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'empty' AND 'load_factor'" << endl
                          << "=========================" << endl;

        const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        typedef TestEntryUtil<int>                               Util;
        typedef IntValueIsHash                                   Hash;
        typedef bsl::equal_to<int>                               Equal;
        typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

        {
            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE = DATA[ti].d_lineNum;
                const char *const    SPEC = DATA[ti].d_spec_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj        mX(0, Hash(), Equal(), &oa);
                const Obj& X = gg(&mX, SPEC);

                bsls::Types::Int64 expNumAllocations = oa.numAllocations();

                LOOP_ASSERT(LINE, X.empty() == (0 == X.size()));

                float loadFactor = X.capacity() > 0
                                 ? static_cast<float>(X.size())
                                             / static_cast<float>(X.capacity())
                                 : 0.0f;

                // Note that since 'capacity' is a power of two, a simple check
                // suffices.

                LOOP_ASSERT(LINE, loadFactor == X.load_factor());

                LOOP_ASSERT(LINE, expNumAllocations == oa.numAllocations());
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

        if (verbose) cout << "Not implemented." << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any target object
        //:   to that of any source object.
        //:
        //: 2 The address of the target object's allocator is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator of the source object is not modified.
        //:
        //: 8 Any memory allocation is exception neutral.
        //:
        //: 9 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a function pointer
        //:   having the appropriate structure for the copy assignment operator
        //:   defined in this component.  (C-4)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of valid object values (one per row) in terms of
        //:     their individual attributes, including (a) first, the default
        //:     value, (b) boundary values corresponding to every range of
        //:     values that each individual attribute can independently attain,
        //:     and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3: (C-1..3, 5..9)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):
        //:
        //:     1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       using 'oa' and having the value 'W'.
        //:
        //:     3 Use the equality comparison operators to verify that the two
        //:       objects, 'mX' and 'Z', are initially equal if and only if the
        //:       table indices from P-4 and P-4.2 are the same.
        //:
        //:     4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality comparison operators to verify that: (C-1,
        //:       6)
        //:
        //:       1 The target object, 'mX', now has the same value as 'Z'.
        //:
        //:       2 'Z' still has the same value as 'ZZ'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that both object allocators are unchanged.  (C-2, 7)
        //:
        //:     8 Use appropriate test allocators to verify that: (C-3, 8)
        //:
        //:       1 No additional memory is allocated by the source object.
        //:
        //:       2 All object memory is released when the object is destroyed.
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3: (C-1..3, 5..10)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj' 'mX'
        //:     using 'oa' and a 'const' 'Obj' 'ZZ' (using a distinct "scratch"
        //:     allocator).
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Use the equality comparison operators to verify that the target
        //:     object, 'mX', has the same value as 'ZZ'.
        //:
        //:   5 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   6 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-5)
        //:
        //:   7 Use the equality comparison operators to verify that the target
        //:     object, 'mX', still has the same value as 'ZZ'.  (C-10)
        //:
        //:   8 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.  (C-2)
        //:
        //:   9 Use appropriate test allocators to verify that: (C-3, 8)
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   FlatHashTable& operator=(const FlatHashTable&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout << "Testing copy assignment operator." << endl;
        {
            testCase9CopyAssignment<int>(0);
            testCase9CopyAssignment<bsltf::AllocBitwiseMoveableTestType>(1);
            testCase9CopyAssignment<bsltf::AllocTestType>(2);
            testCase9CopyAssignment<bsltf::BitwiseCopyableTestType>(3);
            testCase9CopyAssignment<bsltf::BitwiseMoveableTestType>(4);
            testCase9CopyAssignment<bsltf::MovableAllocTestType>(5);
            testCase9CopyAssignment<bsltf::MovableTestType>(6);
            testCase9CopyAssignment<bsltf::NonDefaultConstructibleTestType>(7);
            testCase9CopyAssignment<bsltf::NonOptionalAllocTestType>(8);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTION
        //   Ensure that we can exchange the values of any two objects with the
        //   free function, and we can exchange the values of any two objects
        //   that use the same allocator with the member function.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator address held by both objects is
        //:   unchanged.
        //:
        //: 3 The member function does not allocate memory from any allocator;
        //:   nor does the free function when the two objects being swapped use
        //:   the same allocator.
        //:
        //: 4 The free function can be called with two objects that use
        //:   different allocators.
        //:
        //: 5 Both functions have standard signatures and return types.
        //:
        //: 6 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 7 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //:
        //: 9 The above concerns are addressed for a variety of types.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-5)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 6)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-6)
        //:
        //:     1 The value is unchanged.  (C-6)
        //:
        //:     2 The allocator address held by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mY', and having the value described by 'R2'; also use
        //:       the copy constructor to create, using a "scratch" allocator,
        //:       a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator address held by 'mX' and 'mY'
        //:         is unchanged in both objects.  (C-2)
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-7)
        //:
        //:   1 Create a set of attribute values, 'A', distinct from the values
        //:     corresponding to the default-constructed object, choosing
        //:     values that allocate memory if possible.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX' (having default attribute values); also use the copy
        //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
        //:     'XX' from 'mX'.
        //:
        //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mY' having the value described by the 'Ai' attributes; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-7)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-7)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory was
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that free 'swap' exchanges the values of any two objects
        //:   that use different allocators.  (C-4)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when, using the member 'swap' function, an attempt is
        //:   made to swap objects that do not refer to the same allocator, but
        //:   not when the allocators are the same (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-8)
        //:
        //: 9 Perform the above for a variety of test types.  (C-9)
        //
        // Testing:
        //   void swap(FlatHashTable&);
        //   void swap(FlatHashTable&, FlatHashTable&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTION" << endl
                          << "=============================" << endl;

        if (verbose) cout << "Testing 'swap' methods." << endl;
        {
            testCase8Swap<int>(0);
            testCase8Swap<bsltf::AllocBitwiseMoveableTestType>(1);
            testCase8Swap<bsltf::AllocTestType>(2);
            testCase8Swap<bsltf::BitwiseCopyableTestType>(3);
            testCase8Swap<bsltf::BitwiseMoveableTestType>(4);
            testCase8Swap<bsltf::MovableAllocTestType>(5);
            testCase8Swap<bsltf::MovableTestType>(6);
            testCase8Swap<bsltf::NonDefaultConstructibleTestType>(7);
            testCase8Swap<bsltf::NonOptionalAllocTestType>(8);
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
        //:   creates an object having the same value as the supplied original
        //:   object.
        //:
        //: 2 If an allocator is *not* supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator is supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect on
        //:   subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator, that is
        //:   returned by the 'allocator' accessor method.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a reference providing
        //:   non-modifiable access to the object.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address of the original object is unchanged.
        //:
        //:12 Any memory allocation is exception neutral.
        //:
        //:13 The above concerns are addressed for a variety of types.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..12)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator, (b)
        //:     passing a null allocator address explicitly, and (c) passing
        //:     the address of a test allocator distinct from the default.
        //:
        //:   3 For each of these three iterations (P-2.2): (C-1..12)
        //:
        //:     1 Construct three 'bslma::TestAllocator' objects and install
        //:       one as the current default allocator (note that a ubiquitous
        //:       test allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       supplying it the 'const' object 'Z' (see P-2.1), configured
        //:       appropriately (see P-2.2) using a distinct test allocator for
        //:       the object's footprint.  (C-9)
        //:
        //:     3 Use the equality comparison operators to verify that:
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also apply the object's
        //:       'allocator' accessor, as well as that of 'Z'.  (C-6, 11)
        //:
        //:     5 Use appropriate test allocators to verify that: (C-2..4, 7,
        //:       8, 12)
        //:
        //:       1 Memory is always allocated, and comes from the object
        //:         allocator only (irrespective of the specific number of
        //:         allocations or the total amount of memory allocated).
        //:         (C-2, 4)
        //:
        //:       2 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       3 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Create an object as an automatic variable in the presence of
        //:   injected exceptions (using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros) and verify that no
        //:   memory is leaked.  (C-12)
        //:
        //: 4 Perform the above for a variety of test types.  (C-13)
        //
        // Testing:
        //   FlatHashTable(const FlatHashTable&, Allocator *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "Testing copy constructor." << endl;
        {
            testCase7CopyConstructor<int>(0);
            testCase7CopyConstructor<bsltf::AllocBitwiseMoveableTestType>(1);
            testCase7CopyConstructor<bsltf::AllocTestType>(2);
            testCase7CopyConstructor<bsltf::BitwiseCopyableTestType>(3);
            testCase7CopyConstructor<bsltf::BitwiseMoveableTestType>(4);
            testCase7CopyConstructor<bsltf::MovableAllocTestType>(5);
            testCase7CopyConstructor<bsltf::MovableTestType>(6);
            testCase7CopyConstructor<bsltf::NonDefaultConstructibleTestType>(
                                                                            7);
            testCase7CopyConstructor<bsltf::NonOptionalAllocTestType>(8);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes (size and the set of
        //:   values contained) respectively compare equal.  If 'X' and 'Y' do
        //:   not compare equal, they must compare not-equal.
        //:
        //: 2 Non-modifiable objects can be compared (i.e., objects providing
        //:   only non-modifiable access).
        //:
        //: 3 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //: 4 Both functions have standard signatures and return types.
        //
        // Plan:
        //: 1 Use the addresses of the free equality operators defined in this
        //:   component to initialize, respectively, pointers having the
        //:   appropriate signatures and return types.  (C-4)
        //:
        //: 2 Use an allocator for all object creation and verify the
        //:   comparison operators do not allocate memory.  (C-3)
        //:
        //: 3 Specify a set of specifications for distinct object values.  For
        //:   every item in the cross-product of this set with itself, verify
        //:   the result of the operators on the item to itself and the item to
        //:   every other item, using 'const' items exclusively.  Vary the
        //:   allocator to ensure allocator is not part of the comparison.
        //:   (C-1,2)
        //
        // Testing:
        //   bool operator==(const FlatHashTable&, const FlatHashTable&);
        //   bool operator!=(const FlatHashTable&, const FlatHashTable&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                         << "EQUALITY OPERATORS" << endl
                         << "==================" << endl;

        if (verbose) cout << "Verify signatures." << endl;
        {
            using namespace bdlc;

            typedef TestEntryUtil<int>                               Util;
            typedef IntValueIsHash                                   Hash;
            typedef bsl::equal_to<int>                               Equal;
            typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

            typedef bool (*OP)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            OP op;

            // Due to the internal compiler bug the following two lines of code
            // fail to be compiled by the MSVC (version 19.30) with the
            // following errors:
            //..
            //  error C3861: '==': identifier not found
            //  error C3861: '!=': identifier not found
            //..
            // The issue is reproduced with C++20 flag. This bug has been fixed
            // in compiler version 19.31.  See {DRQS 172604250}.

            op = operator==;
            op = operator!=;

            // Quash unused variable warnings.

            Obj a(0, Hash(), Equal());

            op(a, a);
        }

        if (verbose) cout << "Verify the equality operators." << endl;
        {
            const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            typedef TestEntryUtil<int>                               Util;
            typedef IntValueIsHash                                   Hash;
            typedef bsl::equal_to<int>                               Equal;
            typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

            bslma::TestAllocator oa1("object", veryVeryVeryVerbose);
            bslma::TestAllocator oa2("object", veryVeryVeryVerbose);

            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE1 = DATA[ti].d_lineNum;
                const char *const SPEC1 = DATA[ti].d_spec_p;
                const int         ID1   = DATA[ti].d_valueId;

                Obj        mX(0, Hash(), Equal(), &oa1);
                const Obj& X = gg(&mX, SPEC1);

                LOOP_ASSERT(LINE1,  true == (X == X));
                LOOP_ASSERT(LINE1, false == (X != X));

                for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
                    const int         LINE2 = DATA[tj].d_lineNum;
                    const char *const SPEC2 = DATA[tj].d_spec_p;
                    const int         ID2   = DATA[tj].d_valueId;

                    Obj        mY(0, Hash(), Equal(), &oa1);
                    const Obj& Y = gg(&mY, SPEC2);

                    bsls::Types::Int64 expNumAllocations1 =
                                                         oa1.numAllocations();

                    LOOP2_ASSERT(LINE1, LINE2, (ID1 == ID2) == (X == Y));
                    LOOP2_ASSERT(LINE1, LINE2, (ID1 != ID2) == (X != Y));

                    LOOP2_ASSERT(LINE1,
                                 LINE2,
                                 expNumAllocations1 == oa1.numAllocations());

                    Obj        mZ(0, Hash(), Equal(), &oa2);
                    const Obj& Z = gg(&mZ, SPEC2);

                    bsls::Types::Int64 expNumAllocations2 =
                                                         oa2.numAllocations();

                    LOOP2_ASSERT(LINE1, LINE2, (ID1 == ID2) == (X == Z));
                    LOOP2_ASSERT(LINE1, LINE2, (ID1 != ID2) == (X != Z));

                    LOOP2_ASSERT(LINE1,
                                 LINE2,
                                 expNumAllocations2 == oa2.numAllocations());
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //
        // The functions were omitted.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "Not implemented." << endl;
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
        //:   return values against expected values.  (C-1)
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
        //   const bsl::uint8_t *controls() const;
        //   const ENTRY *entries() const;
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
                typedef TestEntryUtil<int>                               Util;
                typedef IntValueIsHash                                   Hash;
                typedef bsl::equal_to<int>                               Equal;
                typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

                Obj mX(0, Hash(), Equal());  const Obj& X = mX;

                ASSERT(                0 == X.capacity());
                ASSERT(                0 == X.hash_function()(0));
                ASSERT(                1 == X.hash_function()(1));
                ASSERT(             true == X.key_eq()(0, 0));
                ASSERT(            false == X.key_eq()(0, 1));
                ASSERT(            0.875 == X.max_load_factor());
                ASSERT(&defaultAllocator == X.allocator());
            }
            {
                typedef TestEntryUtil<int>                               Util;
                typedef IntZeroHash                                      Hash;
                typedef bsl::less<int>                                   Equal;
                typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

                Obj        mX(0, Hash(), Equal(), &globalAllocator);
                const Obj& X = mX;

                ASSERT(               0 == X.capacity());
                ASSERT(               0 == X.hash_function()(0));
                ASSERT(               0 == X.hash_function()(1));
                ASSERT(           false == X.key_eq()(0, 0));
                ASSERT(            true == X.key_eq()(0, 1));
                ASSERT(           0.875 == X.max_load_factor());
                ASSERT(&globalAllocator == X.allocator());
            }
            {
                typedef TestEntryUtil<int>                               Util;
                typedef IntValueIsHash                                   Hash;
                typedef bsl::equal_to<int>                               Equal;
                typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                {
                    Obj mX(32, Hash(), Equal());  const Obj& X = mX;

                    ASSERT(   32 == X.capacity());
                    ASSERT(0.875 == X.max_load_factor());
                    ASSERT(  &da == X.allocator());
                }
                ASSERT(2 == da.numAllocations());
                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 == oa.numAllocations());
                ASSERT(0 == oa.numBytesInUse());

                {
                    Obj mX(64, Hash(), Equal(), 0);  const Obj& X = mX;

                    ASSERT(   64 == X.capacity());
                    ASSERT(0.875 == X.max_load_factor());
                    ASSERT(  &da == X.allocator());
                }
                ASSERT(4 == da.numAllocations());
                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 == oa.numAllocations());
                ASSERT(0 == oa.numBytesInUse());

                {
                    Obj mX(32, Hash(), Equal(), &oa);  const Obj& X = mX;

                    ASSERT(   32 == X.capacity());
                    ASSERT(0.875 == X.max_load_factor());
                    ASSERT(  &oa == X.allocator());
                }
                ASSERT(4 == da.numAllocations());
                ASSERT(0 == da.numBytesInUse());
                ASSERT(2 == oa.numAllocations());
                ASSERT(0 == oa.numBytesInUse());
            }
        }

        if (verbose) {
            cout << "Testing 'controls', 'entries', and 'size'." << endl;
        }
        {
            {
                typedef TestEntryUtil<int>                               Util;
                typedef IntValueIsHash                                   Hash;
                typedef bsl::equal_to<int>                               Equal;
                typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(0, Hash(), Equal(), &oa);  const Obj& X = mX;

                ASSERT(0 == X.controls());
                ASSERT(0 == X.entries());
                ASSERT(0 == X.size());
            }
            {
                typedef TestEntryUtil<int>                               Util;
                typedef IntValueIsHash                                   Hash;
                typedef bsl::equal_to<int>                               Equal;
                typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(32, Hash(), Equal(), &oa);  const Obj& X = mX;

                ASSERT(0 == X.size());

                ASSERT(k_EMPTY == X.controls()[ 0]);
                ASSERT(k_EMPTY == X.controls()[ 8]);
                ASSERT(k_EMPTY == X.controls()[16]);
                ASSERT(k_EMPTY == X.controls()[24]);

                if (16 == k_SIZE) {
                    mX.insert(0x0021);
                    ASSERT(     1 == X.size());
                    ASSERT(  0x21 == X.controls()[ 0]);
                    ASSERT(0x0021 == X.entries()[ 0]);

                    mX.insert(0x4022);
                    ASSERT(     2 == X.size());
                    ASSERT(  0x22 == X.controls()[ 1]);
                    ASSERT(0x4022 == X.entries()[ 1]);

                    mX.insert(0x8023);
                    ASSERT(     3 == X.size());
                    ASSERT(  0x23 == X.controls()[16]);
                    ASSERT(0x8023 == X.entries()[16]);

                    mX.insert(0xC024);
                    ASSERT(     4 == X.size());
                    ASSERT(  0x24 == X.controls()[17]);
                    ASSERT(0xC024 == X.entries()[17]);

                    mX.erase(0x4022);
                    ASSERT(       3 == X.size());
                    ASSERT(k_ERASED == X.controls()[ 1]);
                }
                else {
                    mX.insert(0x0021);
                    ASSERT(     1 == X.size());
                    ASSERT(  0x21 == X.controls()[ 0]);
                    ASSERT(0x0021 == X.entries()[ 0]);

                    mX.insert(0x4022);
                    ASSERT(     2 == X.size());
                    ASSERT(  0x22 == X.controls()[ 8]);
                    ASSERT(0x4022 == X.entries()[ 8]);

                    mX.insert(0x8023);
                    ASSERT(     3 == X.size());
                    ASSERT(  0x23 == X.controls()[16]);
                    ASSERT(0x8023 == X.entries()[16]);

                    mX.insert(0xC024);
                    ASSERT(     4 == X.size());
                    ASSERT(  0x24 == X.controls()[24]);
                    ASSERT(0xC024 == X.entries()[24]);

                    mX.erase(0x4022);
                    ASSERT(       3 == X.size());
                    ASSERT(k_ERASED == X.controls()[ 8]);
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMITIVE GENERATOR FUNCTIONS
        //   Ensure that the generator functions are able to create an object
        //   in any state.
        //
        // Concerns:
        //: 1 Valid syntax produces the expected results.
        //:
        //: 2 Any memory allocation is exception neutral.
        //:
        //: 3 Invalid syntax is detected and reported.
        //
        // Plan:
        //: 1 Evaluate a series of test strings of increasing complexity to
        //:   set the state of a newly created object and verify the returned
        //:   object using the basic accessors.  Vary the test allocator's
        //:   allocation limit to verify behavior in the presence of
        //:   exceptions.  (C-1,2)
        //:
        //: 2 Evaluate the 'ggg' function with a series of test strings of
        //:   increasing complexity and verify its return value.  (C-3)
        //
        // Testing:
        //   FlatHashTable& gg(FlatHashTable *object, const char *spec);
        //   int ggg(FlatHashTable *object, const char *spec, int verboseFlag);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMITIVE GENERATOR FUNCTIONS" << endl
                          << "=============================" << endl;

        typedef bsltf::AllocTestType                               Entry;
        typedef TestEntryUtil<Entry>                               Util;
        typedef IntValueIsHash                                     Hash;
        typedef bsl::equal_to<int>                                 Equal;
        typedef bdlc::FlatHashTable<int, Entry, Util, Hash, Equal> Obj;

        if (verbose) cout << "Testing generator on valid specs." << endl;
        {
            const bsl::size_t            NUM_DATA  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE = DATA[ti].d_lineNum;
                const char *const    SPEC = DATA[ti].d_spec_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    Obj        mX(0, Hash(), Equal(), &oa);
                    const Obj& X = gg(&mX, SPEC);

                    for (bsl::size_t i = 0; SPEC[i]; ++i) {
                        if ('e' == SPEC[i]) {
                            LOOP_ASSERT(LINE, k_EMPTY == X.controls()[i]);
                        }
                        else if ('x' == SPEC[i]) {
                            LOOP_ASSERT(LINE, k_ERASED == X.controls()[i]);
                        }
                        else {
                            LOOP_ASSERT(LINE,
                                    static_cast<int>(SPEC[i] - 'A') ==
                                                              X.controls()[i]);
                        }
                    }

                    bsl::size_t errorIndex;
                    LOOP_ASSERT(LINE,
                                IsValidResult::e_SUCCESS ==
                                                      isValid(&errorIndex, X));

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }

        if (verbose) cout << "Testing generator on invalid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_index;    // offending character index
            } DATA[] = {
// -------------^
//LN                              spec                                    IN
//--  ------------------------------------------------------------------  --
{ L_, ""                                                                , -1 },

{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                , -1 },
{ L_, "exeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  0 },
{ L_, "eAeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  0 },
{ L_, "x.eeeeeeeeeeeeeeeeeeeeeeeeeeeeee"                                ,  1 },
{ L_, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx."                                , 31 },

{ L_, "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee", -1 },
{ L_, "exeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  0 },
{ L_, "eAeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  0 },
{ L_, "x.eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",  1 },
{ L_, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.", 63 },
// -------------v
            };
            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const int         INDEX = DATA[ti].d_index;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(0, Hash(), Equal(), &oa);

                int result = ggg(&mX, SPEC, veryVerbose);
                LOOP2_ASSERT(LINE, result, INDEX == result);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   The constructor, the destructor, and the primary manipulators:
        //:     o 'clear'
        //:     o 'erase(key)'
        //:     o 'insert'
        //:     o 'reset'
        //   operate as expected.
        //
        // Concerns:
        //: 1 The global method 'isValid' checks the consistency of control
        //:   and entry blocks, and returns the expected enumerated value.  The
        //:   position (index) of the error, if any, is delivered by the output
        //:   argument.
        //:
        //: 2 The constructor creates the correct initial value and has
        //:   the hashing, equality, and internal memory management systems
        //:   hooked up properly.  The resultant capacity is as expected.
        //:
        //: 3 The method 'insert' uses the hash function to determine placement
        //:   of entries and stores the hashlet at the expected position in the
        //:   control block.
        //:
        //: 4 The copy and move 'insert' methods produce the expected return
        //:   value, results in a valid state, increases capacity as needed,
        //:   honors bitwise-copy/bitwise-move/move, is exception neutral with
        //:   respect to memory allocation, and uses the allocator.  Inserting
        //:   an entry whose key is already present is a no-op.
        //:
        //: 5 The method 'clear' produces the expected value (empty) and does
        //:   not affect capacity.  The method 'reset' returns the table to the
        //:   zero-capacity state.
        //:
        //: 6 The method 'erase(key)' returns the number of removed entries,
        //:   produces the expected state, and does not affect allocated
        //:   memory.
        //:
        //: 7 Erased positions are repopulated by subsequent inserts.
        //:
        //: 8 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //
        // Plan:
        //: 1 Using the table-driven technique, verify the 'isValid' method
        //:   works as expect.  (C-1)
        //:
        //: 2 Create various objects using the constructor with and without
        //:   passing in an allocator, verify the stored items using the
        //:   (untested) basic accessors, and verifying all allocations
        //:   are done from the allocator and the hash function is used in
        //:   future tests.  (C-2)
        //:
        //: 3 Create objects of various capacities and verify 'insert' uses the
        //:   hash value correctly by using the 'ValueIsHash' hasher and
        //:   examining the return value of the (untested) 'controls' basic
        //:   accessor.  (C-3)
        //:
        //: 4 Create objects using the 'bslma::TestAllocator', use the 'insert'
        //:   method with various values, verify the return value, and use the
        //:   (untested) basic accessors in conjunction with the 'isValid'
        //:   global method to verify the state of the object.  Use 'bsltf'
        //:   test types to verify bitwise-copy/bitwise-move/move are honored.
        //:   Also vary the test allocator's allocation limit to verify
        //:   behavior in the presence of exceptions.  (C-4)
        //:
        //: 5 Create objects using the 'bslma::TestAllocator', use 'insert'
        //:   to obtain various states, use 'clear' or 'reset', and verify the
        //:   resultant state.  (C-5)
        //:
        //: 6 Create objects using the 'bslma::TestAllocator', use 'insert'
        //:   to obtain various states, use 'erase', verify the return value,
        //:   and use the (untested) basic accessors in conjunction with the
        //:   'isValid' global method to verify the state of the object.  (C-6)
        //:
        //: 7 Create objects using the 'bslma::TestAllocator', use 'insert' and
        //:   'erase' to obtain various states, use 'insert', and use the
        //:   (untested) basic accessors in conjunction with the 'isValid'
        //:   global method to verify the state of the object.  (C-7)
        //:
        //: 8 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-8)
        //
        // Testing:
        //   FlatHashTable(size_t, const HASH&, const EQUAL&, Allocator * = 0);
        //   ~FlatHashTable();
        //   void clear();
        //   size_t erase(const KEY&);
        //   bsl::pair<iterator, bool> insert(FORWARD_REF(ENTRY_TYPE) entry)
        //   void reset();
        //   CONCERN: 'isValid' works as expected
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        if (verbose) cout << "Testing 'isValid'." << endl;
        {
            const int k_CAPACITY = 16;

            const bsl::uint8_t EE = k_EMPTY;
            const bsl::uint8_t XX = k_ERASED;

            typedef IsValidResult IVR;

            static const struct {
                int          d_lineNum;
                int          d_entries[k_CAPACITY];
                bsl::uint8_t d_controls[k_CAPACITY];
                bsl::size_t  d_size;
                bsl::size_t  d_blockSize;
                IVR::Enum    d_expResult;
                bsl::size_t  d_expErrorIndex;
            } DATA[] = {
// -------------^
//LN  entries          controls            size  BS        result         EI
//--  -------  --------------------------  ----  --  -------------------  --
{ L_, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      { EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE },
                                              0,  8,      IVR::e_SUCCESS,  0 },

{ L_, {  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      {  1, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE },
                                              1,  8,      IVR::e_SUCCESS,  0 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      { XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX, XX },
                                              0,  8,      IVR::e_SUCCESS, 16 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0, 11,  0,  0,  0,  0,  0,  0,  0 },
      { EE, EE, EE, EE, EE, EE, EE, EE,  1, EE, EE, EE, EE, EE, EE, EE },
                                              1,  8,      IVR::e_SUCCESS,  0 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0, 11,  0,  0,  0,  0,  0,  0,  0 },
      { EE, EE, EE, EE, EE, EE, EE, EE,  2, EE, EE, EE, EE, EE, EE, EE },
                                              1,  8,  IVR::e_BAD_HASHLET,  8 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0 },
      { EE, EE, EE, EE, EE, EE, EE, EE,  1, EE, EE, EE, EE, EE, EE, EE },
                                              1,  8, IVR::e_BAD_POSITION,  8 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0, 11,  0,  0,  0,  0,  0,  0,  0 },
      { EE, EE, EE, EE, EE, EE, EE, EE,  1, EE, EE, EE, EE, EE, EE, EE },
                                              2,  8,     IVR::e_BAD_SIZE, 16 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      { EE, XX, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE },
                                              0,  8,    IVR::e_BAD_BLOCK,  0 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      { EE, EE,  1, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE },
                                              0,  8,    IVR::e_BAD_BLOCK,  0 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      { XX, XX, EE, EE, EE, EE, EE, XX, EE, EE, EE, EE, EE, EE, EE, EE },
                                              0,  8,    IVR::e_BAD_BLOCK,  0 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      { EE, EE, EE, EE, EE, EE, EE, EE, EE, XX, EE, EE, EE, EE, EE, EE },
                                              0,  8,    IVR::e_BAD_BLOCK,  8 },

{ L_, {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      { EE, EE, EE, EE, EE, EE, EE, EE, EE, EE,  1, EE, EE, EE, EE, EE },
                                              0,  8,    IVR::e_BAD_BLOCK,  8 },

{ L_, { 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      {  1, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, EE, XX, XX, XX, XX },
                                              1,  4,      IVR::e_SUCCESS,  0 },

{ L_, {  0,  0,  0,  0,  0, 31,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
      { XX, XX, XX, XX, XX,  1, EE, EE, EE, EE, EE, EE, XX, XX, XX, XX },
                                              1,  4,      IVR::e_SUCCESS,  0 },
// -------------v
            };
            const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                const int           LINE            = DATA[i].d_lineNum;
                const int          *ENTRIES_RAW     = DATA[i].d_entries;
                const bsl::uint8_t *CONTROLS        = DATA[i].d_controls;
                const bsl::size_t   SIZE            = DATA[i].d_size;
                const bsl::size_t   BLOCK_SIZE      = DATA[i].d_blockSize;
                const IVR::Enum     EXP_RESULT      = DATA[i].d_expResult;
                const bsl::size_t   EXP_ERROR_INDEX = DATA[i].d_expErrorIndex;

                // decode 'ENTRIES_RAW' into values matching 'IntValueIsHash'
                int ENTRIES[k_CAPACITY];
                {
                    bsl::uint64_t numGroup = k_CAPACITY / BLOCK_SIZE;

                    bsl::size_t shift = 16 - bdlb::BitUtil::log2(numGroup);

                    for (bsl::size_t j = 0; j < k_CAPACITY; ++j) {
                        int v = ENTRIES_RAW[j];
                        v = ((v / 10) << shift) + (v % 10);
                        ENTRIES[j] = v;
                    }
                }

                bsl::size_t errorIndex;
                IVR::Enum result = isValid(&errorIndex,
                                           ENTRIES,
                                           CONTROLS,
                                           SIZE,
                                           BLOCK_SIZE,
                                           k_CAPACITY,
                                           IntValueIsHash(),
                                           TestEntryUtil<int>());

                LOOP2_ASSERT(LINE, result, EXP_RESULT == result);

                if (IVR::e_SUCCESS != result) {
                    LOOP2_ASSERT(LINE,
                                 errorIndex,
                                 EXP_ERROR_INDEX == errorIndex);
                }

                if (verbose && EXP_RESULT != result) {
                    printError(CONTROLS, k_CAPACITY, errorIndex, result);
                }
            }
        }

        if (verbose) cout << "Testing constructor." << endl;
        {
            {
                typedef TestEntryUtil<int>                               Util;
                typedef IntValueIsHash                                   Hash;
                typedef bsl::equal_to<int>                               Equal;
                typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

                Obj mX(0, Hash(), Equal());  const Obj& X = mX;

                ASSERT(                0 == X.capacity());
                ASSERT(                0 == X.hash_function()(0));
                ASSERT(                1 == X.hash_function()(1));
                ASSERT(             true == X.key_eq()(0, 0));
                ASSERT(            false == X.key_eq()(0, 1));
                ASSERT(&defaultAllocator == X.allocator());
            }
            {
                typedef TestEntryUtil<int>                               Util;
                typedef IntZeroHash                                      Hash;
                typedef bsl::less<int>                                   Equal;
                typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

                Obj        mX(0, Hash(), Equal(), &globalAllocator);
                const Obj& X = mX;

                ASSERT(               0 == X.capacity());
                ASSERT(               0 == X.hash_function()(0));
                ASSERT(               0 == X.hash_function()(1));
                ASSERT(           false == X.key_eq()(0, 0));
                ASSERT(            true == X.key_eq()(0, 1));
                ASSERT(&globalAllocator == X.allocator());
            }
            {
                typedef TestEntryUtil<int>                               Util;
                typedef IntValueIsHash                                   Hash;
                typedef bsl::equal_to<int>                               Equal;
                typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

                bslma::TestAllocator da("temporary default",
                                        veryVeryVeryVerbose);
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                {
                    Obj mX(31, Hash(), Equal());  const Obj& X = mX;
                    ASSERT( 32 == X.capacity());
                    ASSERT(&da == X.allocator());
                }
                ASSERT(2 == da.numAllocations());
                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 == oa.numAllocations());
                ASSERT(0 == oa.numBytesInUse());

                {
                    Obj mX(32, Hash(), Equal(), 0);  const Obj& X = mX;
                    ASSERT( 32 == X.capacity());
                    ASSERT(&da == X.allocator());
                }
                ASSERT(4 == da.numAllocations());
                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 == oa.numAllocations());
                ASSERT(0 == oa.numBytesInUse());

                {
                    Obj mX(33, Hash(), Equal(), &oa);  const Obj& X = mX;
                    ASSERT( 64 == X.capacity());
                    ASSERT(&oa == X.allocator());
                }
                ASSERT(4 == da.numAllocations());
                ASSERT(0 == da.numBytesInUse());
                ASSERT(2 == oa.numAllocations());
                ASSERT(0 == oa.numBytesInUse());
            }
        }

        if (verbose) cout << "Testing 'insert' uses 'HASH'." << endl;
        {
            typedef TestEntryUtil<int>                               Util;
            typedef IntValueIsHash                                   Hash;
            typedef bsl::equal_to<int>                               Equal;
            typedef bdlc::FlatHashTable<int, int, Util, Hash, Equal> Obj;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(32, Hash(), Equal(), &oa);  const Obj& X = mX;

            ASSERT(     32 == X.capacity());
            ASSERT(k_EMPTY == X.controls()[ 0]);
            ASSERT(k_EMPTY == X.controls()[ 8]);
            ASSERT(k_EMPTY == X.controls()[16]);
            ASSERT(k_EMPTY == X.controls()[24]);

            if (16 == k_SIZE) {
                mX.insert(0x0021);
                ASSERT(0x21 == X.controls()[ 0]);

                mX.insert(0x4022);
                ASSERT(0x22 == X.controls()[ 1]);

                mX.insert(0x8023);
                ASSERT(0x23 == X.controls()[16]);

                mX.insert(0xC024);
                ASSERT(0x24 == X.controls()[17]);
            }
            else {
                mX.insert(0x0021);
                ASSERT(0x21 == X.controls()[ 0]);

                mX.insert(0x4022);
                ASSERT(0x22 == X.controls()[ 8]);

                mX.insert(0x8023);
                ASSERT(0x23 == X.controls()[16]);

                mX.insert(0xC024);
                ASSERT(0x24 == X.controls()[24]);
            }

            testCase2InsertHash<bsl::hash<int> >(0);
            testCase2InsertHash<IntZeroHash>(1);
            testCase2InsertHash<IntValueIsHash>(2);
        }

        if (verbose) cout << "Testing copy 'insert'." << endl;
        {
            testCase2CopyInsert<int>(0, false, false);
            testCase2CopyInsert<bsltf::AllocBitwiseMoveableTestType>(1,
                                                                     true,
                                                                     false);
            testCase2CopyInsert<bsltf::AllocTestType>(2, true, true);
            testCase2CopyInsert<bsltf::BitwiseCopyableTestType>(3,
                                                                false,
                                                                false);
            testCase2CopyInsert<bsltf::BitwiseMoveableTestType>(4,
                                                                false,
                                                                false);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
            testCase2CopyInsert<bsltf::MovableAllocTestType>(5, true, false);
#else
            testCase2CopyInsert<bsltf::MovableAllocTestType>(5, true, true);
#endif

            testCase2CopyInsert<bsltf::MovableTestType>(6, false, false);
            testCase2CopyInsert<bsltf::NonDefaultConstructibleTestType>(7,
                                                                        false,
                                                                        false);
            testCase2CopyInsert<bsltf::NonOptionalAllocTestType>(8,
                                                                 true,
                                                                 true);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        if (verbose) cout << "Testing move 'insert'." << endl;
        {
            testCase2MoveInsert<int>(0, false, true);
            testCase2MoveInsert<bsltf::AllocBitwiseMoveableTestType>(1,
                                                                     true,
                                                                     true);
            testCase2MoveInsert<bsltf::AllocTestType>(2, true, false);
            testCase2MoveInsert<bsltf::BitwiseCopyableTestType>(3,
                                                                false,
                                                                true);
            testCase2MoveInsert<bsltf::BitwiseMoveableTestType>(4,
                                                                false,
                                                                true);
            testCase2MoveInsert<bsltf::MovableAllocTestType>(5, true, true);
            testCase2MoveInsert<bsltf::MovableTestType>(6, false, true);

            testCase2MoveInsert<bsltf::MoveOnlyAllocTestType>(7, true, true);

            testCase2MoveInsert<bsltf::NonDefaultConstructibleTestType>(8,
                                                                        false,
                                                                        false);
            testCase2MoveInsert<bsltf::NonOptionalAllocTestType>(9,
                                                                 true,
                                                                 false);
        }
#endif

        if (verbose) cout << "Testing 'clear'." << endl;
        {
            testCase2ClearAndReset<int>(0);
            testCase2ClearAndReset<bsltf::AllocTestType>(1);
            testCase2ClearAndReset<bsltf::BitwiseCopyableTestType>(2);
            testCase2ClearAndReset<bsltf::BitwiseMoveableTestType>(3);
        }

        if (verbose) cout << "Testing 'erase' and re-'insert'." << endl;
        {
            testCase2EraseAndReInsert<int>(0);
            testCase2EraseAndReInsert<bsltf::AllocTestType>(1);
            testCase2EraseAndReInsert<bsltf::BitwiseCopyableTestType>(2);
            testCase2EraseAndReInsert<bsltf::BitwiseMoveableTestType>(3);
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

        typedef bdlc::FlatHashTable<int,
                                    int,
                                    TestEntryUtil<int>,
                                    bsl::hash<int>,
                                    bsl::equal_to<int> > Obj;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj        mX(0, bsl::hash<int>(), bsl::equal_to<int>(), &oa);
        const Obj& X = mX;

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

        typedef bdlc::FlatHashTable<int,
                                    bsl::pair<const int, int>,
                                    TestEntryUtil<bsl::pair<const int, int> >,
                                    bsl::hash<int>,
                                    bsl::equal_to<int> > IntPairObj;

        IntPairObj mZ(0, bsl::hash<int>(), bsl::equal_to<int>(), &oa);
        const IntPairObj& Z = mZ;

        mZ.insert(bsl::make_pair(0, 0));

        ASSERT(1 == Z.size());
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
