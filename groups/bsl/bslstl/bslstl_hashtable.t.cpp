// bslstl_hashtable.t.cpp                                             -*-C++-*-
#include <bslstl_hashtable.h>

#include <bslstl_equalto.h>
#include <bslstl_hash.h>

#include <bslalg_bidirectionallink.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <stdio.h>
#include <stdlib.h>

// To resolve gcc warnings, while printing 'size_t' arguments portably on
// Windows, we use a macro and string literal concatenation to produce the
// correct 'printf' format flag.
#ifdef ZU
#undef ZU
#endif

#if defined BSLS_PLATFORM_CMP_MSVC
#  define ZU "%Iu"
#else
#  define ZU "%zu"
#endif

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior
    // is undefined unless 'a' and 'b' were created with the same allocator.
{
    BSLS_ASSERT_OPT(a.allocator() == b.allocator());

    using namespace bsl;
    swap(a, b);
}

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' (above).

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
//
// (informal plan to be written up later)
// HashTable is most similar to the 'unordered_multimap' container, although
// the 'key' is implicitly also part of the 'value_type', rather than being
// storing as distinct fields in a pair.
//
// There are a wide variety of potential types to independently instantiate
// each of the template parameters of HashTable with.  To ease testing, we
// will have a primary test driver having a single type parameter that can be
// easily driven by the template testing facility, delegated to by multiple
// test configuration classes, which act as a C++03 template alias to an
// appropriate configuration of the test driver.  The main 'switch' will
// therefore run multiple instantiations of each test driver function to
// establish the necessary properties.
//
// First we will validate that HashTable is a valid value-semantic type.  This
// is difficult in the case that the stored elements are not themselves value-
// semantic, so this early testing will be limited to only those types that
// provide a full range of required behavior; testing of non-value semantic
// elements, or awkward hash and compare functors, will be deferred past the
// initial 10 cases.
// To establish value semantics, we will test the following class members, and
// a couple of specific test-support functions that simplify the test space:
//     default constructor
//     copy constructor
//     destructor
//     copy assignment operator
//     operator ==/!=
//
//   Accessors and manipulators
//     'HashTable::allocator'
//     'HashTable::elementListRoot'
//     insertValue        - a test function using 'insert' restricted
//                          to ValueType
//     verifyListContents - key accessor to validate the list root points to a
//                          list having the right set of values, and arranged
//                          so that elements with equivalent keys, determined
//                          by a supplied comparator, are stored contiguously.
//
// Therefore, 'hasher' and 'comparator' are not salient attributes, even though
// value ultimately depends on 'comparator' to define key-equivalent groups.
// Likewise, no 'insert*' operation forms the primary manipulator, nor is
// 'maxLoadFactor' a concern in establishing value - insert operations must
// satisfy constraints implied by all these additional moving parts, and will
// all be established in test cases following the value-semantic test sequence.
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//  TBD....
//
//
//           ( A '*' IN THE TABLE BELOW INDICATES THE TEST CASE HAS )
//           (   BEEN IDENTIFIED BUT HAS NOT YET BEEN COMPLETED.    )
//
//           ( NOTE THAT ALL TESTING AT THE MOMENT ASSUMES A 'set'- )
//           ( LIKE KEY_CONFIG AND THERE IS NO 'map'-LIKE EQUIVALENT)
//           ( NOR MACHINERY FOR CREATING SUITABLE TEST SEQUENCES.  )
//
//           ( WE ARE STILL LOOKING FOR A MINIMAL-BUT-COMPLETE SET  )
//           ( OF FUNCTORS AND NON-BDE ALLOCATORS TO COMPRISE TEST  )
//           (          KITS TO INVOKE FOR EACH TEST CASE.          )
//
// TYPES
//*[23] typedef ALLOCATOR                              AllocatorType;
//*[23] typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
//*[23] typedef typename KEY_CONFIG::KeyType           KeyType;
//*[23] typedef typename KEY_CONFIG::ValueType         ValueType;
//*[23] typedef bslalg::BidirectionalNode<ValueType>   NodeType;
//*[23] typedef typename AllocatorTraits::size_type    SizeType;
//
// CREATORS
//*[11] HashTable(const ALLOCATOR&  allocator = ALLOCATOR());
//*[ 2] HashTable(const HASHER&, const COMPARATOR&, SizeType, const ALLOCATOR&)
//*[ 7] HashTable(const HashTable& original);
//*[ 7] HashTable(const HashTable& original, const ALLOCATOR& allocator);
//*[ 2] ~HashTable();
//
// MANIPULATORS
//*[ 9] operator=(const HashTable& rhs);
//*[16] insert(const SOURCE_TYPE& obj);
//*[16] insert(const ValueType& obj, const bslalg::BidirectionalLink *hint);
//*[17] insertIfMissing(bool *isInsertedFlag, const SOURCE_TYPE& obj);
//*[17] insertIfMissing(bool *isInsertedFlag, const ValueType& obj);
//*[18] insertIfMissing(const KeyType& key);
//*[12] remove(bslalg::BidirectionalLink *node);
//*[ 2] removeAll();
//*[13] rehashForNumBuckets(SizeType newNumBuckets);
//*[13] rehashForNumElements(SizeType numElements);
//*[15] setMaxLoadFactor(float loadFactor);
//*[ 8] swap(HashTable& other);
//
//      ACCESSORS
//*[ 4] allocator() const;
//*[ 4] comparator() const;
//*[ 4] hasher() const;
//*[ 4] size() const;
//*[22] maxSize() const;
//*[ 4] numBuckets() const;
//*[22] maxNumBuckets() const;
//*[14] loadFactor() const;
//*[ 4] maxLoadFactor() const;
//*[ 4] elementListRoot() const;
//*[19] find(const KeyType& key) const;
//*[20] findRange(BLink **first, BLink **last, const KeyType& k) const;
//*[ 6] findEndOfRange(bslalg::BidirectionalLink *first) const;
//*[ 4] bucketAtIndex(SizeType index) const;
//*[ 4] bucketIndexForKey(const KeyType& key) const;
//*[21] countElementsInBucket(SizeType index) const;
//
//*[ 6] bool operator==(const HashTable& lhs, const HashTable& rhs);
//*[ 6] bool operator!=(const HashTable& lhs, const HashTable& rhs);
//
//// specialized algorithms:
//*[ 8] void swap(HashTable& a, HashTable& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
//
// Class HashTable_ImpDetails
// [  ] size_t nextPrime(size_t n);
// [  ] bslalg::HashTableBucket *defaultBucketAddress();
//
// Class HashTable_Util<ALLOCATOR>
// [  ] initAnchor(bslalg::HashTableAnchor *, SizeType, const ALLOC&);
// [  ] destroyBucketArray(bslalg::HashTableBucket *, SizeType, const ALLOC&)
//
// Class HashTable_ListProctor
// [  ] TBD...
//
// Class HashTable_ArrayProctor
// [  ] TBD...
//
// TEST TEST APPARATUS AND GENERATOR FUNCTIONS
//*[ 3] int ggg(HashTable *object, const char *spec, int verbose = 1);
//*[ 3] HashTable& gg(HashTable *object, const char *spec);
//*[ 2] insertElement(HashTable<K, H, E, A> *, const K::ValueType&)
//*[ 3] verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
//
// [  ] CONCERN: The type employs the expected size optimizations.
// [  ] CONCERN: The type has the necessary type traits.

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

#define BSL_TF_EQ      BSLTF_TEMPLATETESTFACILITY_COMPARE_EQUAL 
#define BSL_TF_NOT_EQ  BSLTF_TEMPLATETESTFACILITY_COMPARE_NOT_EQUAL

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  --------             -------
    { L_,    0, "",                  "" },
    { L_,    1, "A",                 "A" },
    { L_,    2, "AA",                "AA" },
    { L_,   20, "B",                 "B" },
    { L_,    5, "AB",                "AB" },
    { L_,    5, "BA",                "AB" },
    { L_,   19, "AC",                "AC" },
    { L_,   23, "CD",                "CD" },
    { L_,    7, "ABC",               "ABC" },
    { L_,    7, "ACB",               "ABC" },
    { L_,    7, "BAC",               "ABC" },
    { L_,    7, "BCA",               "ABC" },
    { L_,    7, "CAB",               "ABC" },
    { L_,    7, "CBA",               "ABC" },
    { L_,   17, "BAD",               "ABD" },
    { L_,    4, "ABCA",              "AABC" },
    { L_,    6, "ABCB",              "ABBC" },
    { L_,    8, "ABCC",              "ABCC" },
    { L_,    3, "ABCABC",            "AABBCC" },
    { L_,    3, "AABBCC",            "AABBCC" },
    { L_,    9, "ABCD",              "ABCD" },
    { L_,    9, "ACBD",              "ABCD" },
    { L_,   18, "BEAD",              "ABDE" },
    { L_,   10, "ABCDE",             "ABCDE" },
    { L_,   22, "FEDCB",             "BCDEF" },
    { L_,   11, "FEDCBA",            "ABCDEF" },
    { L_,   14, "ABCDEFGHI",         "ABCDEFGHI" },
    { L_,   16, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,   16, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" }
};

static const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

static
const float DEFAULT_MAX_LOAD_FACTOR[] = {
     0.125f,
     0.66f,
     1.0f,
     1.5f,
     8.0f,
};
static const int DEFAULT_MAX_LOAD_FACTOR_SIZE =
              sizeof DEFAULT_MAX_LOAD_FACTOR / sizeof *DEFAULT_MAX_LOAD_FACTOR;

typedef bslalg::HashTableImpUtil     ImpUtil;
typedef bslalg::BidirectionalLink    Link;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace bsl {
template <>
struct equal_to< ::BloombergLP::bsltf::NonEqualComparableTestType> {
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType
                                                          first_argument_type;
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType
                                                          second_argument_type;
    typedef bool                                          result_type;

    bool operator()(const ::BloombergLP::bsltf::NonEqualComparableTestType& a,
                    const ::BloombergLP::bsltf::NonEqualComparableTestType& b)
                                                                          const
    {
        return BSL_TF_EQ(a, b);
    }
};

template <>
struct hash< ::BloombergLP::bsltf::NonEqualComparableTestType> {
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType argument_type;
    typedef size_t                                           result_type;

    size_t operator()(const ::BloombergLP::bsltf::NonEqualComparableTestType&
                                                                   value) const
    {
        static const bsl::hash<int> EVALUATE_HASH = bsl::hash<int>();
        return EVALUATE_HASH(value.data());
    }
};

}

namespace BloombergLP {
namespace bslstl {
// HashTable-specific print function.
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void debugprint(
         const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& t)
{
    if (0 == t.size()) {
        printf("<empty>");
    }
    else {
        for (Link *it = t.elementListRoot(); it; it = it->nextLink()) {
            const typename KEY_CONFIG::KeyType& key =
                                           ImpUtil::extractKey<KEY_CONFIG>(it);
            bsls::BslTestUtil::callDebugprint(static_cast<char>(
                             bsltf::TemplateTestFacility::getIdentifier(key)));
        }
    }
    fflush(stdout);
}

}  // close namespace BloombergLP::bslstl
}  // close namespace BloombergLP


namespace TestTypes {

class AwkwardMaplikeElement {
    // This class provides an awkward value-semantic type, designed to be used
    // with a KEY_CONFIG policy for a HashTable that supplies a non-equality
    // comparable key-type, using 'data' for the 'extractKey' method, while
    // the class itself *is* equality-comparable (as required of a value
    // semantic type) so that a HashTable of these objects should have a well-
    // defined 'operator=='.  Note that this class is a specific example for a
    // specific problem, rather than a template providing the general test typee
    // for keys distinct from values, as the template test facility requires an
    // explicit specialization of a function template,
    // 'TemplateTestFacility::getIdentifier<T>', which would require a partial
    // template specialization if this class were a template, and that is not
    // supported by the C++ language.

  private:
    bsltf::NonEqualComparableTestType d_data;

  public:
    AwkwardMaplikeElement()
    : d_data()
    {
    }

    explicit
    AwkwardMaplikeElement(int value)
    : d_data(value)
    {
    }

    explicit
    AwkwardMaplikeElement(const bsltf::NonEqualComparableTestType& value)
    : d_data(value)
    {
    }

    void setData(int value) { d_data.setData(value); }
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const { return d_data.data(); }
        // Return the value of the 'data' attribute of this object.

    const bsltf::NonEqualComparableTestType& key() const { return d_data; }
};

inline
bool operator==(const AwkwardMaplikeElement& lhs,
                const AwkwardMaplikeElement& rhs) {
    return BSL_TF_EQ(lhs.data(), rhs.data());
}

inline
bool operator!=(const AwkwardMaplikeElement& lhs,
                const AwkwardMaplikeElement& rhs) {
    return !(lhs == rhs);
}

inline
void debugprint(const AwkwardMaplikeElement& value)
{
    bsls::debugprint(value.data());
}

} // close namespace TestTypes


namespace BloombergLP {
namespace bsltf {
    
template <>
inline
int TemplateTestFacility::getIdentifier<TestTypes::AwkwardMaplikeElement>(
                               const TestTypes::AwkwardMaplikeElement& object)
{
    return object.data();
}

}  // close namespace BloombergLP::bsltf
}  // close namespace BloombergLP


namespace {

                            // ====================
                            // class ExceptionGuard
                            // ====================

template <class OBJECT>
struct ExceptionGuard {
    // This class provide a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores the
    // a copy of an object of the parameterized type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // create on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int           d_line;      // the line number at construction
    OBJECT        d_copy;      // copy of the object being tested
    const OBJECT *d_object_p;  // address of the original object

  public:
    // CREATORS
    ExceptionGuard(const OBJECT    *object,
                   int              line,
                   bslma::Allocator *basicAllocator = 0)
    : d_line(line)
    , d_copy(*object, basicAllocator)
    , d_object_p(object)
        // Create the exception guard for the specified 'object' at the
        // specified 'line' number.  Optionally, specify 'basicAllocator' used
        // to supply memory.
    {}

    ~ExceptionGuard()
        // Destroy the exception guard.  If the guard was not released, verify
        // that the state of the object supplied at construction has not
        // change.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_copy == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release the guard from verifying the state of the object.
    {
        d_object_p = 0;
    }
};

bool g_enableTestEqualityComparator = true;
bool g_enableTestHashFunctor = true;

template <class TYPE, int GROUP_SIZE>
class GroupedEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE', but in a
    // way that multiple distinct value will compare equal with each other.
    // The function-call operator compares the integer returned by the class
    // method 'TemplateTestFacility::getIdentifier' divided by the (template
    // parameter) GROUP_SIZE.

  public:
    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Return 'true' if the integer representation of the specified 'lhs'
        // divided by 'GROUP_SIZE' (rounded down) is equal to than integer
        // representation of the specified 'rhs' divided by 'GROUP_SIZE'.
    {
        int leftValue = bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
                    / GROUP_SIZE;
        int rightValue = bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs)
                        / GROUP_SIZE;
        return leftValue == rightValue;
    }
};

template <class TYPE, class HASHER, int GROUP_SIZE>
class GroupedHasher : private HASHER {
    // This test class provides a mechanism that defines a function-call
    // operator that returns the same hash for multiple values.  The
    // function-call operator invoke the (template parameter) type 'HASHER' on
    // the integer returned by the class method
    // 'TemplateTestFacility::getIdentifier' divided by the (template
    // parameter) GROUP_SIZE.  'HASHER' shall be a class that can be invoked as
    // if it has the following method:
    //..
    //  int operator()(int value);
    //..

  public:
    // ACCESSORS
    size_t operator() (const TYPE& value)
        // Return the hash value of the integer representation of the specified
        // 'value' divided by 'GROUP_SIZE' (rounded down) is equal to than
        // integer representation of the specified 'rhs' divided by
        // 'GROUP_SIZE'.
    {
        int groupNum = bsltf::TemplateTestFacility::getIdentifier<TYPE>(value)
                     / GROUP_SIZE;

        return HASHER::operator()(groupNum);
    }
};


                       // ====================
                       // class TestComparator
                       // ====================

template <class TYPE>
class TestEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked
    {
        g_enableTestEqualityComparator = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableTestEqualityComparator = true;
    }

    // CREATORS
    //! TestEqualityComparator(const TestEqualityComparator& original) =
    //                                                                 default;
        // Create a copy of the specified 'original'.

    explicit TestEqualityComparator(int id = 0)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_count(0)
    {
    }

    // MANIPULATORS
    void setId(int value)
    {
        d_id = value;
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableTestEqualityComparator) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            == bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
    }

    bool operator== (const TestEqualityComparator& rhs) const
    {
        return (id() == rhs.id());// && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};

template <class TYPE>
class TestHashFunctor {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked
    {
        g_enableTestHashFunctor = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableTestHashFunctor = true;
    }

    // CREATORS
    //! TestHashFunctor(const TestHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    explicit TestHashFunctor(int id = 0)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_count(0)
    {
    }

    // ACCESSORS
    native_std::size_t operator() (const TYPE& obj) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableTestHashFunctor) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj);
    }

    bool operator== (const TestHashFunctor& rhs) const
    {
        return (id() == rhs.id());// && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};

template <class KEY>
class StatefulHash : bsl::hash<KEY> {
    // This value-semantic class adapts a class meeting the C++11 'Hash'
    // requirements (C++11 [hash.requirements], 17.6.3.4) with an additional
    // 'mixer' attribute, that constitutes the value of this class, and is
    // used to mutate the value returned from the adapted hasher when calling
    // 'operator()'.
    typedef bsl::hash<KEY> Base;

    template <class OTHER_KEY>
    friend
    bool operator==(const StatefulHash<OTHER_KEY>&,
                    const StatefulHash<OTHER_KEY>&);

  private:
    // DATA
    native_std::size_t d_mixer;

  public:
    explicit StatefulHash(native_std::size_t mixer = 0xffff)
    : d_mixer(mixer)
    {
    }

    void setMixer(int value)
    {
        d_mixer = value;
    }

    native_std::size_t operator()(const KEY& k) const
    {
        return Base::operator()(k) ^ d_mixer;
    }
};

template <class KEY>
bool operator==(const StatefulHash<KEY>& lhs, const StatefulHash<KEY>& rhs)
{
    return lhs.d_mixer == rhs.d_mixer;
}

template <class KEY>
bool operator!=(const StatefulHash<KEY>& lhs, const StatefulHash<KEY>& rhs)
{
    return lhs.d_mixer != rhs.d_mixer;
}

template <class KEY, class HASHER = ::bsl::hash<int> >
class TestFacilityHasher : public HASHER { // exploit empty base
    // This test class provides a mechanism that defines a function-call
    // operator that provides a hash code for objects of the parameterized
    // 'KEY'.  The function-call operator is implemented by calling the wrapper
    // functor, 'HASHER', with integers converted from objects of 'KEY' by the
    // class method 'TemplateTestFacility::getIdentifier'.

  public:
    TestFacilityHasher(const HASHER& hash = HASHER())               // IMPLICIT
    : HASHER(hash)
    {
    }

    // ACCESSORS
    native_std::size_t operator() (const KEY& k) const
        // Return a hash code for the specified 'k' using the wrapped 'HASHER'.
    {
        return HASHER::operator()(
                           bsltf::TemplateTestFacility::getIdentifier<KEY>(k));
    }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class FUNCTOR, bool ENABLE_SWAP = true>
class DegenerateClass : public FUNCTOR {
    // This test class template adapts a DefaultConstructible class to offer
    // a minimal or outright obstructive interface for testing generic code.
    // We expect to use this to supply Hasher and Comparator classes to test
    // 'HashTable', which must be CopyConstructible, Swappable, and nothrow
    // Destructible, and offer the (inherited) function call operator as their
    // public interface.  No other operation should be usable.  We take
    // advantage of the fact that defining a copy constructor inhibits the
    // generation of a default constructor, and that constructors are not
    // inherited by a derived class.

  private:
    explicit DegenerateClass(const FUNCTOR& base);
        // Create a 'DegenerateClass' wrapping a copy of the specified
        // 'base'.

    void operator&();  // = delete;
        // not implemented

    template<class T>
    void operator,(const T&); // = delete;
        // not implemented

    template<class T>
    void operator,(T&); // = delete;
        // not implemented


    DegenerateClass& operator=(const DegenerateClass&);
    // TBD. Do we require functors be CopyAssigable, Swappable, or customize
    // availability for test scenario?

  public:
    static DegenerateClass cloneBaseObject(const FUNCTOR& base);

    DegenerateClass(const DegenerateClass& original);
        // Create a 'DegenerateClass' having the same value the specified
        // 'original'.

    void swap(DegenerateClass& other);
        // Swap the wrapped 'FUNCTOR' object, using ADL with 'std::swap' in
        // the lookup set.  Note that this method hides any 'swap' method in
        // the wrapped 'FUNCTOR' class.  Also note that this overload is needed
        // only so that the free-function 'swap' can be defined, as the native
        // std library 'swap' function does will not accept this class on AIX
        // or Visual C++ prior to VC2010.
};

template <class FUNCTOR, bool ENABLE_SWAP>
inline
DegenerateClass<FUNCTOR, ENABLE_SWAP>::DegenerateClass(const FUNCTOR& base)
: FUNCTOR(base) 
{
}

template <class FUNCTOR, bool ENABLE_SWAP>
inline
DegenerateClass<FUNCTOR, ENABLE_SWAP>::DegenerateClass(
                                               const DegenerateClass& original)
: FUNCTOR(original)
{
}

template <class FUNCTOR, bool ENABLE_SWAP>
inline
DegenerateClass<FUNCTOR, ENABLE_SWAP>
DegenerateClass<FUNCTOR, ENABLE_SWAP>::cloneBaseObject(const FUNCTOR& base)
{
    return DegenerateClass(base);
}

template <class FUNCTOR, bool ENABLE_SWAP>
inline
void DegenerateClass<FUNCTOR, ENABLE_SWAP>::swap(DegenerateClass& other)
{
    using std::swap;
    swap(static_cast<FUNCTOR&>(*this), static_cast<FUNCTOR&>(other));
}

template <class FUNCTOR>
inline
void swap(DegenerateClass<FUNCTOR, true>& lhs,
          DegenerateClass<FUNCTOR, true>& rhs)
{
    lhs.swap(rhs);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class FUNCTOR>
struct MakeDefaultFunctor {
    static FUNCTOR make() { return FUNCTOR(); }
};

template <class FUNCTOR, bool ENABLE_SWAP>
struct MakeDefaultFunctor<DegenerateClass<FUNCTOR, ENABLE_SWAP> > {
    static DegenerateClass<FUNCTOR, ENABLE_SWAP> make() {
        return DegenerateClass<FUNCTOR, ENABLE_SWAP>::cloneBaseObject(
                                                                    FUNCTOR());
    }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//       test support functions dealing with hash and comparator functors

void setHasherState(bsl::hash<int> *hasher, int id)
{
    (void) hasher;
    (void) id;
}

void setHasherState(StatefulHash<int> *hasher, int id)
{
    hasher->setMixer(id);
}

bool isEqualHasher(const bsl::hash<int>&, const bsl::hash<int>&)
    // Provide an overloaded function to compare hash functors.  Return 'true'
    // because 'bsl::hash' is stateless.
{
    return true;
}

bool isEqualHasher(const StatefulHash<int>& lhs,
                   const StatefulHash<int>& rhs)
    // Provide an overloaded function to compare hash functors.  Return
    // 'lhs == rhs'.
{
    return lhs == rhs;
}

template <class KEY>
void setComparatorState(bsl::equal_to<KEY> *comparator, int id)
{
    (void) comparator;
    (void) id;
}

template <class KEY>
void setComparatorState(TestEqualityComparator<KEY> *comparator, int id)
{
    comparator->setId(id);
}


template <class KEY>
bool isEqualComparator(const bsl::equal_to<KEY>&, const bsl::equal_to<KEY>&)
    // Provide an overloaded function to compare comparators.  Return 'true'
    // because 'bsl::equal_to' is stateless.
{
    return true;
}

template <class KEY>
bool isEqualComparator(const TestEqualityComparator<KEY>& lhs,
                       const TestEqualityComparator<KEY>& rhs)
    // Provide an overloaded function to compare comparators.  Return
    // 'lhs == rhs'.
{
    return lhs == rhs;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class BoolArray {
    // This class holds a set of boolean flags, the number of which is
    // speicifed when such an object is constructed.  This class is a
    // lightweight alternative to 'vector<bool>' as there is no need to
    // introduce such a component dependency for a simple test facility.

  private:
    bool *d_data;

    BoolArray(const BoolArray&); // = delete;
    BoolArray& operator=(const BoolArray&);  // = delete;
    
  public:
    explicit BoolArray(size_t n)
    : d_data(new bool[n])
    {
        for (size_t i = 0; i != n; ++i) {
            d_data[i] = false;
        }
    }

    ~BoolArray()
    {
        delete[] d_data;
    }

    bool& operator[](size_t index) { return d_data[index]; }

    bool operator[](size_t index) const { return d_data[index]; }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//                         test support functions

bool expectPoolToAllocate(int n)
    // Return 'true' if the memory pool used by the container under test is
    // expected to allocate memory on the inserting the specified 'n'th
    // element, and 'false' otherwise.
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when 'n' is a power of 2
}

template<class KEY_CONFIG, class COMPARATOR, class VALUES>
int verifyListContents(Link              *containerList,
                       const COMPARATOR&  compareKeys,
                       const VALUES&      expectedValues,
                       size_t             expectedSize)
    // NOTE: THIS TEST IS EXPENSIVE, WITH QUADRATIC COMPLEXITY ON LIST LENGTH
    //                       DO NOT CALL IN A TIGHT LOOP
    // Verify the specified 'containerList' has the specified 'expectedSize'
    // number of elements, and contains the same values as the array in the
    // specified 'expectedValues', and that the elements in the list are
    // arranged so that elements whose keys compare equal using the specified
    // 'compareKeys' predicate are all arranged contiguously within the list.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;

    // Check to avoid creating an array of length zero.
    if (0 == containerList) {
        ASSERTV(0 == expectedSize);
        return 0;                                                     // RETURN
    }

    BoolArray foundValues(expectedSize);
    size_t i = 0;
    for (Link *cursor = containerList;
         cursor;
         cursor = cursor->nextLink(), ++i)
    {
        const ValueType& element = ImpUtil::extractValue<KEY_CONFIG>(cursor);
        const int nextId = bsltf::TemplateTestFacility::getIdentifier(element);
        size_t j = 0;
        do {
            if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[j])
                                                                   == nextId) {
                ASSERTV(j, expectedValues[j], element, !foundValues[j]);
                foundValues[j] = true;
                break;
            }
        }
        while (++j != expectedSize);
    }
    ASSERTV(expectedSize, i, expectedSize == i);
    if (expectedSize != i) {
        return -2;                                                    // RETURN
    }

    size_t missing = 0;
    for (size_t j = 0; j != expectedSize; ++j) {
        if (!foundValues[j]) {
            ++missing;
        }
    }

    if (missing > 0) {
        return missing;                                               // RETURN
    }

    // All elements are present, check the contiguity requirement
    // Note that this test is quadratic in the length of the list, although we
    // will optimize for the case of duplicates actually occurring.
    for (Link *cursor = containerList; cursor; cursor = cursor->nextLink()) {
        const KeyType& key = ImpUtil::extractKey<KEY_CONFIG>(cursor);

        Link *next = cursor->nextLink();
        // Walk to end of key-equivalent sequence
        while (next &&
               compareKeys(key, ImpUtil::extractKey<KEY_CONFIG>(next))) {
            cursor = next;
            next   = next->nextLink();
        }

        // Check there are no more equivalent keys in the list.
        // Note that this test also serves to check there are no duplicates in
        // the preceding part of the list, as this check would have failed
        // earlier if that were the case.
        while (next &&
               !compareKeys(key, ImpUtil::extractKey<KEY_CONFIG>(next))) {
            next = next->nextLink();
        }

        if (0 != next) {
            return -3; // code for discontiguous list                 // RETURN
        }
    }

    return 0;  // 0 indicates a successful test!
}

template <class KEY_CONFIG, class HASH, class EQUAL, class ALLOC>
Link* insertElement(
                  bslstl::HashTable<KEY_CONFIG, HASH, EQUAL, ALLOC> *hashTable,
                  const typename KEY_CONFIG::ValueType&              value)
    // Insert an element into the specified 'hashTable' and return the address
    // of the new node, unless the insertion would cause the hash table to
    // exceed its 'maxLoadFactor' and rehash, in which case return a null
    // pointer value.
{
    BSLS_ASSERT(hashTable);

    if ((hashTable->size() + 1)
        > hashTable->maxLoadFactor() * hashTable->numBuckets() ) {
        return 0;                                                     // RETURN
    }
    return hashTable->insert(value);
}

template <class KEY_CONFIG, class HASHER>
bool isValidHashTable(bslalg::BidirectionalLink      *listRoot,
                      const bslalg::HashTableBucket&  arrayRoot,
                      native_std::size_t              arrayLength)
{
    // We perform the const-cast inside this function as we know:
    // i/  The function we call does not make any writes to the bucket array.
    // ii/ It is much simpler to cast in this one place than in each of our
    //     call sites.
    bslalg::HashTableAnchor anchor(
                             const_cast<bslalg::HashTableBucket *>(&arrayRoot),
                             arrayLength,
                             listRoot);
    return bslalg::HashTableImpUtil::isWellFormed<KEY_CONFIG, HASHER>(anchor);
}

}  // close unnamed namespace

// ============================================================================
//                         TEST DRIVER HARNESS
// ----------------------------------------------------------------------------

// - - - - - - Configuration policies to instantiate HashTable with - - - - - -

template <class ELEMENT>
struct BasicKeyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that
    // can support a 'HashTable'.  It might be consistent with use as a 'set'
    // or a 'multiset' container.

    typedef ELEMENT KeyType;
    typedef ELEMENT ValueType;

    static const KeyType& extractKey(const ValueType& value)
    {
        return value;
    }
};


template <class ELEMENT>
struct BsltfConfig {
    // This class provides the most primitive possible KEY_CONFIG type that
    // can support a 'HashTable'.  It might be consistent with use as a 'set'
    // or a 'multiset' container.

    typedef int     KeyType;
    typedef ELEMENT ValueType;

    static const int& extractKey(const ValueType& value)
    {
        // Note that this function MUST return a reference, but we have no
        // actual storage to return a reference to.  As we know the specific
        // usage patterns of this test driver, we can ensure that no two hash
        // computations happen while the first result is still held as a
        // reference.  Unfortunately, we have no such guarantee on simultaneous
        // evalutations in the HashTable facility itself, so we cycle through a
        // cache of 16 results, as no reference should be so long lived that we
        // see 16 live references.
        static int results_cache[16] = {};
        static int index = -1;

        if (16 == ++index) {
            index = 0;
        }

        results_cache[index] =
                             bsltf::TemplateTestFacility::getIdentifier(value);
        return results_cache[index];
    }
};

struct TrickyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that
    // can support a 'HashTable'.  It might be consistent with use as a 'set'
    // or a 'multiset' container.

    typedef bsltf::NonEqualComparableTestType KeyType;
    typedef TestTypes::AwkwardMaplikeElement  ValueType;

    static const KeyType& extractKey(const ValueType& value)
    {
        return value.key();
    }
};

// - - - - - Main test driver harness, that implements the test cases - - - - -

template <class KEY_CONFIG,
          class HASHER,
          class COMPARATOR,
          class ALLOCATOR>
class TestDriver {
    // This templatized struct provide a namespace for testing the 'HashTable'
    // container.  The parameterized 'KEY_CONFIG', 'HASHER', 'COMPARATOR' and
    // 'ALLOCATOR' specifies the configuration, hasher type, comparator type
    // and allocator type respectively.  Each "testCase*" method tests a
    // specific aspect of
    // 'HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>'.  Each test case
    // should be invoked with various parameterized type to fully test the
    // container.

  private:
    // TYPES
    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR> Obj;
        // Type under testing.

    typedef typename Obj::SizeType   SizeType;
    typedef typename Obj::KeyType    KeyType;
    typedef typename Obj::ValueType  ValueType;
        // Shorthands

    typedef bsltf::TestValuesArray<ValueType> TestValues;

  public:
    typedef bsltf::StdTestAllocator<ValueType> StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the 'map<KEY, VALUE, COMP, ALLOC>' object.
    //
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
    //                                      // unique but otherwise arbitrary
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Insert the value corresponding to A.
    // "AA"         Insert two values both corresponding to A.
    // "ABC"        Insert three values corresponding to A, B and C.
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert' and white-box
        // manipulator 'clear'.  Optionally specify a zero 'verbose' to
        // suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Note that
        // this function is used to implement 'gg' as well as allow for
        // verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

  public:
    // TEST CASES
    static void testCase13();

    static void testCase12();

    static void testCase11();

    //        Test case 10 is not supported

    static void testCase9();

    static void testCase8();

    static void testCase7();

    static void testCase6();

    //        Test case 5 is not supported

    static void testCase4();

    static void testCase3();

    static void testCase2();

    static void testCase1(typename KEY_CONFIG::ValueType *testValues,
                          size_t                          numValues);
};

// - - - - - Wrapper to forward test-class adapters to the main harness - - - -

template <class CONFIGURED_DRIVER>
struct TestDriver_ForwardTestCasesByConfiguation {

    // TEST CASES
    static void testCase26() { CONFIGURED_DRIVER::testCase26(); }

    static void testCase25() { CONFIGURED_DRIVER::testCase25(); }

    static void testCase24() { CONFIGURED_DRIVER::testCase24(); }

    static void testCase23() { CONFIGURED_DRIVER::testCase23(); }

    static void testCase22() { CONFIGURED_DRIVER::testCase22(); }

    static void testCase21() { CONFIGURED_DRIVER::testCase21(); }

    static void testCase20() { CONFIGURED_DRIVER::testCase20(); }

    static void testCase19() { CONFIGURED_DRIVER::testCase19(); }

    static void testCase18() { CONFIGURED_DRIVER::testCase18(); }

    static void testCase17() { CONFIGURED_DRIVER::testCase17(); }

    static void testCase16() { CONFIGURED_DRIVER::testCase16(); }

    static void testCase15() { CONFIGURED_DRIVER::testCase15(); }

    static void testCase14() { CONFIGURED_DRIVER::testCase14(); }

    static void testCase13() { CONFIGURED_DRIVER::testCase13(); }

    static void testCase12() { CONFIGURED_DRIVER::testCase12(); }

    static void testCase11() { CONFIGURED_DRIVER::testCase11(); }

    // there is no testCase10();

    static void testCase9() { CONFIGURED_DRIVER::testCase9(); }

    static void testCase8() { CONFIGURED_DRIVER::testCase8(); }

    static void testCase7() { CONFIGURED_DRIVER::testCase7(); }

    static void testCase6() { CONFIGURED_DRIVER::testCase6(); }

    // there is no testCase5();

    static void testCase4() { CONFIGURED_DRIVER::testCase4(); }

    static void testCase3() { CONFIGURED_DRIVER::testCase3(); }

    static void testCase2() { CONFIGURED_DRIVER::testCase2(); }

    // We do not forward a generic breathing test, testCase1();
};

// - - - - - - - - - - Pre-packaged test harness adapters - - - - - - - - - - -
// The template test facility, bsltf, requires class templates taking a single
// argument, that is the element type to vary in the test.  We desire a variety
// of configurations pushing the various policy paramters of the 'HashTable'
// class template, such as the type of functors, the key extraction policy,
// etc. so we write some simple adapters that will generate the appropriate
// instantiation of the test harness, from a template parameterized on only the
// element type (to be tested).

template <class ELEMENT>
struct TestDriver_BasicConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestFacilityHasher<ELEMENT>
                     , ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     > 
       > {
};

template <class ELEMENT>
struct TestDriver_StatefulConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestFacilityHasher<ELEMENT, StatefulHash<int> >
                     , TestEqualityComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_DegenerateConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , DegenerateClass<TestFacilityHasher<ELEMENT> >
                     , DegenerateClass<TestEqualityComparator<ELEMENT> >
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_DegenerateConfiguationWithNoSwap
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , DegenerateClass<TestFacilityHasher<ELEMENT>, false>
                     , DegenerateClass<TestEqualityComparator<ELEMENT>, false>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_BsltfConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BsltfConfig<ELEMENT>
                     , ::bsl::hash<int>
                     , ::bsl::equal_to<int>
                     , ::bsl::allocator<ELEMENT>
                     > 
       > {
    // Basic configuration to test a key-type different to the value-type.
    // This is a simple attempt to deliver something approximating a map with
    // 'int' as key, where the 'int' is computed for each element.  This is
    // the simplest way to generate a condiguration compatible with the 'bsltf'
    // template testing framework used above, without rewriting each test case
    // to additional support for separate test tables of pairs to initialize a
    // more traditional-style map, with different math for computed values and
    // separate logic for duplicate elements and groups.
};


struct TestDriver_AwkwardMaplike
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< TrickyConfig
                     , TestFacilityHasher<TrickyConfig::KeyType>
                     , ::bsl::equal_to<TrickyConfig::KeyType>
                     , ::bsl::allocator<TrickyConfig::ValueType>
                     >
       > {
};


// - - - - - - - - - - - Special adapters for test case 6 - - - - - - - - - - -
// As initially written, test case 6 requires special handling with distinct
// functor classes to demonstrate and test the necessary key-equivalent groups.
// We hope to fold these cases in as regular test cases that pass through the
// remaining test cases as well, using the familiar pattern above.

template <class ELEMENT>
struct TestCase_GroupedUniqueKeys
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration "groups" values into buckets by hashing 5 consecutive
    // values to the same hash code, but maintaining a unique key value for
    // each of those cases.  This should lead to a high rate of collisions.
};

template <class ELEMENT>
struct TestCase_GroupedSharedKeys
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , GroupedEqualityComparator<ELEMENT, 5>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration "groups" values into buckets by hashing 5 consecutive
    // values to the same hash code, and similarly arranging for those keys to
    // compare equal to each other.  This should lead to behavior similar to a
    // multiset.
};

template <class ELEMENT>
struct TestCase6_DegenerateConfiguration
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< 
                   BasicKeyConfig<ELEMENT>,
                   DegenerateClass<GroupedHasher<ELEMENT, bsl::hash<int>, 5> >,
                   DegenerateClass<GroupedEqualityComparator<ELEMENT, 5> >,
                   ::bsl::allocator<ELEMENT> >
       > {
};

template <class ELEMENT>
struct TestCase6_DegenerateConfigurationNoSwap
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< 
            BasicKeyConfig<ELEMENT>,
            DegenerateClass<GroupedHasher<ELEMENT, bsl::hash<int>, 5>, false >,
            DegenerateClass<GroupedEqualityComparator<ELEMENT, 5>, false >,
            ::bsl::allocator<ELEMENT> >
       > {
};

struct TestDriverForCase6_AwkwardMaplike
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< TrickyConfig
                     , GroupedHasher<TrickyConfig::KeyType, bsl::hash<int>, 5>
                     , GroupedEqualityComparator<TrickyConfig::KeyType, 5>
                     , ::bsl::allocator<TrickyConfig::ValueType>
                     >
       > {
};


                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
int TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ggg(
                                                           Obj        *object,
                                                           const char *spec,
                                                           int         verbose)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());
    const TestValues VALUES;

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            if (!insertElement(object, VALUES[spec[i] - 'A'])) {
                if (verbose) {
                    printf("Error, spec string longer ('%s') than the"
                           "'HashTable' can support without a rehash.\n",
                           spec);
                }

                // Discontinue processing this spec.

                return i;                                             // RETURN
            }
        }
        else {
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
   }
   return SUCCESS;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>&
TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::gg(Obj        *object,
                                                          const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

//- - - - - - - - - - - - - TEST CASE IMPLEMENTATIONS - - - - - - - - - - - - -

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING REHASH METHODS
    //
    // Concerns:
    //: 1 'rehashForNumBuckets' allocates at least the specified number of
    //:   buckets.
    //:
    //: 2 'rehashForNumElements' allocates sufficient buckets so that, after
    //:   the rehash, 'numBuckets() / maxLoadFactor()' >= the specified number
    //:   of elements.
    //:
    //: 3 Neither rehash function affects the value of the object.
    //:
    //: 4 Neither rehash function affects the order of the inserted elements
    //:   with the same value.
    //:
    //: 5 'rehashForNumBuckets' is a no-op if the requested number of buckets
    //:   is less than the current 'numBuckets' in the object.
    //:
    //: 6 'rehashForNumElements' is a no-op if the requested number of elements
    //:   can already be accomodated without exceeding the 'maxLoadFactor' of
    //:   the object.
    //:
    //: 7 Any memory allocation is from the object allocator.
    //:
    //: 8 The only memory allocation is a single allocation for the new bucket
    //:   array; no new nodes are allocated, and the old array is reclaimed by
    //:   the object allocator.
    //:
    //: 9 'rehashForNumBuckets' provides the strong exception guarantee if the
    //:   hasher does not throw.
    //:
    //:10 'rehashForNumBuckets' will reset the object to an empty container,
    //:   without leaking memory or objects, if a hasher throws.
    //:
    //:11 'rehashForNumElements' provides the strong exception guarantee if the
    //:   hasher does not throw.
    //:
    //:12 'rehashForNumElements' will reset the object to an empty container,
    //:   without leaking memory or objects, if a hasher throws.
    //:
    //
    // Plan:
    //: 1 TBD
    //
    // Testing:
    //   rehashForNumBuckets(SizeType newNumBuckets);
    //   rehashForNumElements(SizeType numElements);
    // ------------------------------------------------------------------------

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const size_t REHASH_SIZE[] = {
        0,
        1,
        2,
        3,
        5,
        8,
        12,
        13,
        2099
    };
    const int NUM_REHASH_SIZE = sizeof REHASH_SIZE / sizeof *REHASH_SIZE;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = (int) strlen(SPEC);

        for (int tj = 0; tj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++tj) {
            const float  MAX_LF      = DEFAULT_MAX_LOAD_FACTOR[tj];
            const size_t NUM_BUCKETS = ceil(LENGTH / MAX_LF);

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(HASH, COMPARE, NUM_BUCKETS, MAX_LF, &scratch);
            const Obj& Z = gg(&mZ,  SPEC);

            for (int tj = 0; tj < NUM_REHASH_SIZE; ++tj) {
                Obj mX(Z, &oa); const Obj& X = mX;

                if (veryVerbose) { T_ P_(LINE) P_(Z) P(X) }

                const size_t OLD_NUM_BUCKETS = X.numBuckets();
                const size_t NEW_NUM_BUCKETS =
                                         ceil(REHASH_SIZE[tj ]/ MAX_LF);

                bslma::TestAllocatorMonitor oam(&oa);

                mX.rehashForNumBuckets(NEW_NUM_BUCKETS);

                ASSERTV(LINE, tj, X == Z);

                ASSERTV(LINE, tj, NEW_NUM_BUCKETS <= X.numBuckets());

                if (NEW_NUM_BUCKETS <= OLD_NUM_BUCKETS) {
                    ASSERTV(LINE, tj, OLD_NUM_BUCKETS == X.numBuckets());
                    ASSERTV(LINE, tj, oam.isTotalSame());
                    ASSERTV(LINE, tj, oam.isInUseSame());
                }
                else {
                    ASSERTV(LINE, tj, oam.numBlocksTotalChange(),
                            1 == oam.numBlocksTotalChange());

                    if (0 < LENGTH ) {
                        ASSERTV(LINE, tj, oam.isInUseSame());
                    }
                    else {
                        ASSERTV(LINE, tj, oam.numBlocksInUseChange(),
                                1 == oam.numBlocksInUseChange());
                    }
                }
            }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING remove METHOD
    //
    // Concerns:
    //: 1 ...
    //:
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //
    // Testing:
    //   remove(bslalg::BidirectionalLink *node);
    // ------------------------------------------------------------------------

    if (verbose) {
        printf("\nTesting is not yet implemented.\n");
    }

}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING DEFAULT CONSTRUCTOR:
    //
    // Concerns:
    //: 1 An object created with the default constructor (with or without a
    //:   supplied allocator) has the contractually specified default value.
    //:
    //: 2 If an allocator is NOT supplied to the default constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the default constructor has no effect on
    //:   subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 There is no temporary allocation from any allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: The default constructor allocates no memory.
    //:
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     7 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //
    // Testing:
    //   HashTable(const A& allocator);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting with various allocator configurations.\n");

    // Create an object that all default constructed 'HashTable' objects should
    // have the same value as.  Note that value does not depend on allocator or
    // the ordering functors, so we take the simplest form that we have already
    // tested back in test case 2.
    const Obj DEFAULT(HASHER(), COMPARATOR(), 0, 1.0);

    for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
        const char CONFIG = cfg;  // how we specify the allocator

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        // ----------------------------------------------------------------

        if (veryVerbose) {
            printf("\n\tTesting default constructor.\n");
        }

        Obj                  *objPtr;
        bslma::TestAllocator *objAllocatorPtr;

        switch (CONFIG) {
          case 'a': {
              objPtr = new (fa) Obj();
              objAllocatorPtr = &da;
          } break;
          case 'b': {
              objPtr = new (fa) Obj(0);
              objAllocatorPtr = &da;
          } break;
          case 'c': {
              objPtr = new (fa) Obj(&sa);
              objAllocatorPtr = &sa;
          } break;
          default: {
              ASSERTV(CONFIG, !"Bad allocator config.");
              return;
          } break;
        }

        Obj&                   mX = *objPtr;  const Obj& X = mX;
        bslma::TestAllocator&  oa = *objAllocatorPtr;
        bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

        // Verify any attribute allocators are installed properly.

        ASSERTV(CONFIG, &oa == X.allocator());

        ASSERTV(CONFIG, DEFAULT == X);

        ASSERTV(CONFIG, 0 == X.size());
        ASSERTV(CONFIG, 1 == X.numBuckets());
        ASSERTV(CONFIG, 1.0 == X.maxLoadFactor());
        ASSERTV(CONFIG, 0 == X.elementListRoot());
        ASSERTV(CONFIG, isEqualComparator(COMPARATOR(), X.comparator()));
        ASSERTV(CONFIG, isEqualHasher(HASHER(), X.hasher()));

        // Verify no allocation from the object/non-object allocators.

        ASSERTV(CONFIG, oa.numBlocksTotal(), 0 ==  oa.numBlocksTotal());
        ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

        // ----------------------------------------------------------------

        // Reclaim dynamically allocated object under test.

        fa.deleteObject(objPtr);

        // Verify all memory is released on object destruction.

        ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
        ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR:
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 2 If allocator propagation is not enabled for copy assignment, the
    //:   allocator address held by the target object is unchanged.
    //:
    //: 3 If allocator propagation is not enabled for copy assignment, any
    //:   memory allocation is from the target object's allocator.
    //:
    //: 4 The signature and return type are standard.
    //:
    //: 5 The reference returned is to the target object (i.e., '*this').
    //:
    //: 6 The value of the source object is not modified.
    //:
    //: 7 The allocator address held by the source object is unchanged.
    //:
    //: 8 QoI: Assigning a source object having the default-constructed value
    //:   allocates no memory.
    //:
    //: 9 Any memory allocation is exception neutral.
    //:
    //:10 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 If allocator propagation is enabled for copy assignment,
    //:   any memory allocation is from the source object's
    //:   allocator.
    //:
    //:13 If allocator propagation is enabled for copy assignment, the
    //:   allocator address held by the target object is changed to that of the
    //:   source.
    //:
    //:14 If allocator propagation is enabled for copy assignment, any memory
    //:   allocation is from the original target allocator will be released
    //:   after copy assignment.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-4)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 4 For each row 'R1' (representing a distinct object value, 'V') in the
    //:   table described in P-3: (C-1..2, 5..8, 11)
    //:
    //:   1 Use the value constructor and a "scratch" allocator to create two
    //:     'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
    //:
    //:   2 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table described
    //:     in P-3:
    //:
    //:   3 For each of the iterations (P-4.2): (C-1..2, 5..8, 11)
    //:
    //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mX', having the value 'W'.
    //:
    //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
    //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
    //:
    //:     4 Verify that the address of the return value is the same as that
    //:       of 'mX'.  (C-5)
    //:
    //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
    //:
    //:       1 The target object, 'mX', now has the same value as that of 'Z'.
    //:         (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
    //:
    //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify that
    //:       the respective allocator addresses held by the target and source
    //:       objects are unchanged.  (C-2, 7)
    //:
    //:     7 Use the appropriate test allocators to verify that: (C-8, 11)
    //:
    //:       1 For an object that (a) is initialized with a value that did NOT
    //:         require memory allocation, and (b) is then assigned a value
    //:         that DID require memory allocation, the target object DOES
    //:         allocate memory from its object allocator only (irrespective of
    //:         the specific number of allocations or the total amount of
    //:         memory allocated); also cross check with what is expected for
    //:         'mX' and 'Z'.
    //:
    //:       2 An object that is assigned a value that did NOT require memory
    //:         allocation, does NOT allocate memory from its object allocator;
    //:         also cross check with what is expected for 'Z'.
    //:
    //:       3 No additional memory is allocated by the source object.  (C-8)
    //:
    //:       4 All object memory is released when the object is destroyed.
    //:         (C-11)
    //:
    //: 5 Repeat steps similar to those described in P-4 except that, this
    //:   time, there is no inner loop (as in P-4.2); instead, the source
    //:   object, 'Z', is a reference to the target object, 'mX', and both 'mX'
    //:   and 'ZZ' are initialized to have the value 'V'.  For each row
    //:   (representing a distinct object value, 'V') in the table described in
    //:   P-3: (C-9)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mX'; also use the value constructor and a distinct "scratch"
    //:     allocator to create a 'const' 'Obj' 'ZZ'.
    //:
    //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions (using
    //:     the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  (C-9)
    //:
    //:   5 Verify that the address of the return value is the same as that of
    //:     'mX'.
    //:
    //:   6 Use the equality-comparison operator to verify that the target
    //:     object, 'mX', still has the same value as that of 'ZZ'.
    //:
    //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still the
    //:     object allocator.
    //:
    //:   8 Use the appropriate test allocators to verify that:
    //:
    //:     1 Any memory that is allocated is from the object allocator.
    //:
    //:     2 No additional (e.g., temporary) object memory is allocated when
    //:       assigning an object value that did NOT initially require
    //:       allocated memory.
    //:
    //:     3 All object memory is released when the object is destroyed.
    //:
    //: 6 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   HashTable& operator=(const HashTable& rhs);
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const float MAX_LF1 = DEFAULT_MAX_LOAD_FACTOR[lfi];

            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;

            const size_t      LENGTH1 = strlen(SPEC1);
            const size_t NUM_BUCKETS1 = ceil(LENGTH1 / MAX_LF1);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(HASH, COMPARE, NUM_BUCKETS1, MAX_LF1, &scratch);
            const Obj& Z  = gg(&mZ,  SPEC1);
            Obj mZZ(HASH, COMPARE, NUM_BUCKETS1, MAX_LF1, &scratch);
            const Obj& ZZ = gg(&mZZ, SPEC1);


            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Create second object
            for (int lfj = 0; lfj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfj) {
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const float MAX_LF2 = DEFAULT_MAX_LOAD_FACTOR[lfj];

                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                const size_t      LENGTH2 = strlen(SPEC2);
                const size_t NUM_BUCKETS2 = ceil(LENGTH2 / MAX_LF2);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(HASH, COMPARE, NUM_BUCKETS2, MAX_LF2, &oa);
                    const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                        ASSERTV(LINE1, LINE2,
                                Z.maxLoadFactor(),   X.maxLoadFactor(),
                                Z.maxLoadFactor() == X.maxLoadFactor());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, &scratch == Z.allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

#if !defined(BDE_BUILD_TARGET_SAFE_2)
                    // The invariant check in the destructor uses the default
                    // allocator in SAFE_2 builds.
                    // Otherwise, no memory should be allocated by the default
                    // allocator.
                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
#endif
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }
            }

#if 0  // THIS IS A DUBIOUS TEST COPIED OVER FROM ASSOCIATIVE CONTAINERS
            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(HASH, COMPARE, NUM_BUCKETS1, MAX_LF1, &oa);
                const Obj& X  = gg(&mX,  SPEC1);
                Obj mZZ(HASH, COMPARE, NUM_BUCKETS1, MAX_LF1, &scratch);
                const Obj& ZZ  = gg(&mZZ,  SPEC1);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                    ASSERTV(LINE1, mR,  &X, mR == &X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, &oa == Z.allocator());

                ASSERTV(LINE1, sam.isTotalSame());
                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }
            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
#endif
        }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase8()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects that use the same
    //   allocator.
    //
    // Concerns:
    //: 1 Both functions exchange the state of the (two) supplied objects,
    //:   comprising their values, their functors, and their 'maxLoadFactor'
    //:   attribute.
    //:
    //: 2 Both functions have standard signatures and return types.
    //:
    //: 3 Using either function to swap an object with itself does not
    //:   affect the value of the object (alias-safety).
    //:
    //: 4 If the two objects being swapped use the same allocator, neither
    //:   function allocates memory from any allocator and the allocator
    //:   address held by both objects is unchanged.
    //:
    //: 5 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'false_type', then both function may allocate memory and the
    //:   allocator address held by both object is unchanged.
    //:
    //: 6 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'true_type', then no memory will be allocated and the allocators will
    //:   also be swapped.
    //:
    //: 7 Both functions provides the strong exception guarantee w.t.r. to
    //:   memory allocation .
    //:
    //: 8 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //:
    //: 9 'swap' does not invalidate any references or pointers to elements
    //:   stored in either hash table, unless allocators are unequal and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'false_type'.
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined
    //:   in this component to initialize, respectively, member-function
    //:   and free-function pointers having the appropriate signatures and
    //:   return types.  (C-2)
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
    //: 4 For each row 'R1' in the table of P-3:  (C-1, 3..7)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable
    //:     'Obj', 'mW', having the value described by 'R1'; also use the
    //:     copy constructor and a "scratch" allocator to create a 'const'
    //:     'Obj' 'XX' from 'mW'.
    //:
    //:   3 Use the member and free 'swap' functions to swap the value of
    //:     'mW' with itself; verify, after each swap, that:  (C-3..4)
    //:
    //:     1 The value is unchanged.  (C-3)
    //:
    //:     2 The allocator address held by the object is unchanged.  (C-4)
    //:
    //:     3 There was no additional object memory allocation.  (C-4)
    //:
    //:   4 For each row 'R2' in the table of P-3:  (C-1, 4)
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
    //:         is unchanged in both objects.  (C-4)
    //:
    //:       3 There was no additional object memory allocation.  (C-4)
    //:
    //:     5 Use the value constructor and 'oaz' to a create a modifiable
    //:       'Obj' 'mZ', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'ZZ', from 'Z'.
    //:
    //:     6 Use the member and free 'swap' functions to swap the values of
    //:       'mX' and 'mZ' respectively (when
    //:       AllocatorTraits::propagate_on_container_swap is an alias to
    //:       false_type) under the presence of exception; verify, after each
    //:       swap, that:  (C-1, 5, 7)
    //:
    //:       1 If exception occurred during the swap, both values are
    //:         unchanged.  (C-7)
    //:
    //:       2 If no exception occurred, the values have been exchanged.
    //:         (C-1)
    //:
    //:       3 The common object allocator address held by 'mX' and 'mZ' is
    //:         unchanged in both objects.  (C-5)
    //:
    //:       4 Temporary memory were allocated from 'oa' if 'mZ' is is not
    //:         empty, and temporary memory were allocated from 'oaz' if 'mX'
    //:         is not empty.  (C-5)
    //:
    //:     7 Create a new object allocator, 'oap'.
    //:
    //:     8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:       'mP', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'PP', from 'P.
    //:
    //:     9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:       be an alias to 'true_type' (Instead of this manual step, use an
    //:       allocator that enables propagate_on_container_swap when
    //:       AllocatorTraits supports it) and use the the member and free
    //:       'swap functions to swap the values 'mX' and 'mZ' respectively;
    //:       verify, after each swap, that: (C-1, 6)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:       3 There was no additional object memory allocation.  (C-6)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-8)
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
    //:   5 Use the 'invokeAdlSwap' helper function template to swap the
    //:     values of 'mX' and 'mY', using the free 'swap' function defined
    //:     in this component, then verify that:  (C-8)
    //:
    //:     1 The values have been exchanged.  (C-1)
    //:
    //:     2 There was no additional object memory allocation.  (C-4)
    //
    // Testing:
    //   void swap(HashTable& other);
    //   void swap(HashTable<K, V, C, A>& a, HashTable<K, V, C, A>& b);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                        "\n==============================\n");

    if (verbose) printf(
                     "\nAssign the address of each function to a variable.\n");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bslstl::swap;

        (void)memberSwap;  // quash potential compiler warnings
        (void)freeSwap;
    }

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const float       MAX_LF1 = DEFAULT_MAX_LOAD_FACTOR[lfi];

        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        const size_t      LENGTH1      = strlen(SPEC1);
        const size_t      NUM_BUCKETS1 = ceil(LENGTH1 / MAX_LF1);

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mW(HASH, COMPARE, NUM_BUCKETS1, MAX_LF1, &oa);
        const Obj& W = gg(&mW,  SPEC1);
        const Obj XX(W, &scratch);

        if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

        // member 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, &oa == W.allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, &oa == W.allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        for (int lfj = 0; lfj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfj) {
        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const float       MAX_LF2 = DEFAULT_MAX_LOAD_FACTOR[lfj];

            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            const size_t      LENGTH2      = strlen(SPEC2);
            const size_t      NUM_BUCKETS2 = ceil(LENGTH2 / MAX_LF2);


            Obj mX(XX, &oa);  const Obj& X = mX;

            Obj mY(HASH, COMPARE, NUM_BUCKETS2, MAX_LF2, &oa);
            const Obj& Y = gg(&mY, SPEC2);
            const Obj YY(Y, &scratch);

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, &oa == X.allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                ASSERTV(LINE1, LINE2, &oa == X.allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            bslma::TestAllocator oaz("z_object", veryVeryVeryVerbose);

            Obj mZ(HASH, COMPARE, NUM_BUCKETS2, MAX_LF2, &oaz);
            const Obj& Z = gg(&mZ, SPEC2);
            const Obj ZZ(Z, &scratch);

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oazm(&oaz);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    mX.swap(mZ);

                    guardX.release();
                    guardZ.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END


                ASSERTV(LINE1, LINE2, ZZ, X, ZZ == X);
                ASSERTV(LINE1, LINE2, XX, Z, XX == Z);
                ASSERTV(LINE1, LINE2, &oa == X.allocator());
                ASSERTV(LINE1, LINE2, &oaz == Z.allocator());

                if (0 == X.size()) {
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oam.isTotalUp());
                }

                if (0 == Z.size()) {
                    ASSERTV(LINE1, LINE2, oazm.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oazm.isTotalUp());
                }
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oazm(&oaz);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    swap(mX, mZ);

                    guardX.release();
                    guardZ.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);
                ASSERTV(LINE1, LINE2, &oa == X.allocator());
                ASSERTV(LINE1, LINE2, &oaz == Z.allocator());

                if (0 == X.size()) {
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oam.isTotalUp());
                }

                if (0 == Z.size()) {
                    ASSERTV(LINE1, LINE2, oazm.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oazm.isTotalUp());
                }
            }

        }
        }
    }
    }

    if (verbose) printf(
            "\nInvoke free 'swap' function in a context where ADL is used.\n");
    {
        // 'A' values: Should cause memory allocation if possible.

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mX(HASH, COMPARE, 0, 1.0, &oa);  const Obj& X = mX;
        const Obj XX(X, &scratch);

        Obj mY(HASH, COMPARE, 40, 0.1, &oa);
        const Obj& Y = gg(&mY, "ABC");
        const Obj YY(Y, &scratch);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(mX, mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERTV(YY, X, YY.maxLoadFactor() == X.maxLoadFactor());
        ASSERTV(XX, Y, XX.maxLoadFactor() == Y.maxLoadFactor());
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //  One additional constraint to bear in mind, while testing the copy
    //  constructor, is that the copy does not give us any control over the
    //  'numBuckets' allocated for the new copy.  Given our primary manipulator
    //  is a special 'insertElement' function that does not allow us to insert
    //  past the precomputed capacity of a HashTable, some test scenarios may
    //  produce copies that cannot be further modified by our chosen primary
    //  manipulator.  Therefor, we will test for a failed insertion in the case
    //  that the number of elements in the test data exactly matches the limit
    //  that can be allocated to the buckets without forcing a rehash, and in
    //  that case our mutate-event will be to clear the container instead.  We
    //  considered selectively avoiding this corner case with careful selection
    //  of test data, but realized that simply left a predictable, untested
    //  pattern in our test driver.  We will validate the proper behavior when
    //  an 'insert' operation forces a rehash when testing the 'insert' methods
    //  in a subsequent test case.
    //
    // Concern:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator), the same 'maxLoadFactor', the
    //:   same 'hasher' and the same 'comparator'.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 5 Subsequent changes ('insert's) on the created object have no
    //:   effect on the original.
    //:
    //: 6 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 The function is exception neutral w.r.t. memory allocation.
    //:
    //: 8 QoI The new object has a 'loadFactor' <= its 'maxLoadFactor'.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For each value in S, initialize objects w and x, copy construct y
    //:   from x and use 'operator==' to verify that both x and y subsequently
    //:   have the same value as w.  Let x go out of scope and again verify
    //:   that w == y.  (C-1..4)
    //:
    //: 3 For each value in S initialize objects w and x, and copy construct y
    //:   from x.  Change the state of y, by using the *primary* *manipulator*
    //:   'insertElement' (or 'removeAll' if 'insertElement' returns a null
    //:   pointer value).  Using the 'operator!=' verify that y differs from x
    //:   and w.  Then apply the same operation to 'w' and verify that y still
    //:   differs from x, and now has the same value as w. (C-5)
    //:
    //: 4 Perform tests performed as P-2:  (C-6)
    //:   1 While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.
    //:   2 Verify neither of global and default allocator is used to supply
    //:     memory.  (C-6)
    //:
    //: 5 Perform tests as P-2 in the presence of exceptions during memory
    //:   allocations using a 'bslma::TestAllocator' and varying its
    //:   *allocation* *limit*.  (C-7)
    //
    // Testing:
    //   HashTable(const HashTable& original);
    //   HashTable(const HashTable& original, const A& allocator);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    typedef typename KEY_CONFIG::KeyType KEY;
    typedef typename KEY_CONFIG::ValueType VALUE;

    const TestValues VALUES;

    const int TYPE_ALLOC =
         bslalg::HasTrait<KEY, bslalg::TypeTraitUsesBslmaAllocator>::VALUE
         + bslalg::HasTrait<VALUE, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;

    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    if (verbose) printf("\nTesting parameters: TYPE_ALLOC = %d.\n",
                        TYPE_ALLOC);
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEFG",
            "HFGEDCBA",
            "CFHEBIDGA",
            "BENCKHGMALJDFOI",
            "IDMLNEFHOPKGBCJA",
            "OIQGDNPMLKBACHFEJ"
        };
        const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const float       MAX_LF      = DEFAULT_MAX_LOAD_FACTOR[lfi];
            const char *const SPEC        = SPECS[ti];

            const size_t      LENGTH      = strlen(SPEC);
            const size_t      NUM_BUCKETS = ceil(LENGTH / MAX_LF);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object w, with space for an extra element.
            Obj mW(HASH, COMPARE, NUM_BUCKETS + 1, MAX_LF);
            const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(HASH, COMPARE, NUM_BUCKETS, MAX_LF, &oa);
            const Obj& X = gg(&mX, SPEC);

            // Sanity check only, previous test cases established this.
            ASSERTV(MAX_LF, SPEC, W, X,  W == X);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4 and 8

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX = new Obj(HASH, COMPARE, NUM_BUCKETS, MAX_LF, &oa);
                gg(pX, SPEC);

                ASSERTV(MAX_LF, SPEC, *pX, X, *pX == X);
                ASSERTV(MAX_LF, SPEC, *pX, W, *pX == W);

                const Obj Y0(*pX);

                ASSERTV(MAX_LF, SPEC, *pX, Y0, *pX == Y0);
                ASSERTV(MAX_LF, SPEC,   X, Y0,   X == Y0);
                ASSERTV(MAX_LF, SPEC,   W, Y0,   W == Y0);
                ASSERTV(MAX_LF, SPEC,   W,  X,   W ==  X);

                ASSERTV(MAX_LF, SPEC, Y0.allocator() ==
                                           bslma::Default::defaultAllocator());
                ASSERTV(MAX_LF, SPEC, pX->maxLoadFactor(), Y0.maxLoadFactor(),
                         pX->maxLoadFactor() == Y0.maxLoadFactor());
                ASSERTV(MAX_LF, SPEC, Y0.loadFactor() <= Y0.maxLoadFactor());
                delete pX;
                ASSERTV(MAX_LF, SPEC, W, Y0, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(W);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                Link *RESULT = insertElement(&Y1, VALUES['Z' - 'A']);
                if(0 == RESULT) {
                    Y1.removeAll();
                }

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : "); P(Y1);
                }

                if (RESULT || 0 != W.size()) {
                    const size_t EXPECTED_LENGTH = RESULT
                                                 ? LENGTH + 1
                                                 : 0;

                    ASSERTV(MAX_LF, SPEC, Y1.size(), EXPECTED_LENGTH,
                            Y1.size() == EXPECTED_LENGTH);
                    ASSERTV(MAX_LF, SPEC, W, Y1, W != Y1);
                    ASSERTV(MAX_LF, SPEC, X, Y1, X != Y1);
                }

                if (RESULT) {
                    RESULT = insertElement(&mW, VALUES['Z' - 'A']);
                    // Remember we reserved enough space at construction.
                    ASSERT(0 != RESULT);
                }
                else {
                    mW.removeAll();
                }

                ASSERTV(MAX_LF, SPEC, W, Y1, W == Y1);
                if (RESULT || 0 < X.size()) {
                    ASSERTV(MAX_LF, SPEC, X, Y1, X != Y1);
                }
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");

                bslma::TestAllocatorMonitor oam(&oa);

                Obj Y11(X, &oa);

                if (LENGTH == 0) {
                    ASSERTV(MAX_LF, SPEC, oam.isTotalSame());
                    ASSERTV(MAX_LF, SPEC, oam.isInUseSame());
                }
                else {
                    //const int TYPE_ALLOCS = TYPE_ALLOC * X.size();
                    //ASSERTV(SPEC, BB, AA, BB + 1 + TYPE_ALLOCS == AA);
                    //ASSERTV(SPEC, B, A, B + 1 + TYPE_ALLOCS ==  A);
                    ASSERTV(MAX_LF, SPEC, oam.isTotalUp());
                    ASSERTV(MAX_LF, SPEC, oam.isInUseUp());
                }

                oam.reset(&oa);

                Y11.removeAll();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : ");
                    P(Y11);
                }

                ASSERTV(SPEC, oam.isTotalSame());

                ASSERTV(SPEC, 0 == Y11.size());
                if (LENGTH != 0) {
                    ASSERTV(MAX_LF, SPEC, W != Y11 || !W.size());
                    ASSERTV(MAX_LF, SPEC, X != Y11);
                }
                ASSERTV(MAX_LF, SPEC, Y11.allocator() == X.allocator());
            }

            if (0 < X.size())
            {   // Exception checking.
                // There is nothing to test if 'X' is empty, and several test
                // conditions would be complicated to allow for 'W == X' in
                // this state.

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    Obj Y2(X, &oa);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(MAX_LF, SPEC,  W, Y2, W  != Y2);
                    ASSERTV(MAX_LF, SPEC, Y2,  X, Y2 ==  X);
                    ASSERTV(MAX_LF, SPEC, Y2.allocator() == X.allocator());

                    Link *RESULT = insertElement(&Y2, VALUES['Z' - 'A']);
                    if(0 == RESULT) {
                        Y2.removeAll();
                    }
                    ASSERTV(MAX_LF, SPEC,  W, Y2, W  == Y2);
                    ASSERTV(MAX_LF, SPEC, Y2,  X, Y2 !=  X);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                if (LENGTH == 0) {
                    ASSERTV(MAX_LF, SPEC, oam.isTotalSame());
                    ASSERTV(MAX_LF, SPEC, oam.isInUseSame());
                }
                else {
                    ASSERTV(MAX_LF, SPEC, oam.isInUseSame());
                }
            }
        }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they contain
    //:   the same values.
    //:
    //: 2 No non-salient attributes participate.  The non-salient attributes of
    //:   a 'HashTable' include the 'allocator', 'loadFactor', 'maxLoadFactor'
    //:   and 'numBuckets'.
    //:
    //: 3 'true  == (X == X)' (i.e., identity)
    //:
    //: 4 'false == (X != X)' (i.e., identity)
    //:
    //: 5 'X == Y' if and only if 'Y == X' (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if 'Y != X' (i.e., commutativity)
    //:
    //: 7 'X != Y' if and only if '!(X == Y)'
    //:
    //: 8 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 9 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //:
    //:10 'operator==' is defined in terms of
    //:   'operator==(KEY_CONFIG::ValueType)' instead of the supplied
    //:   comparator function (which, along with the hasher, is still used to
    //:   establish key-equivalent groups).
    //:
    //:11 No memory allocation occurs as a result of comparison (e.g., the
    //:   arguments are not passed by value).
    //:
    //:12 The equality operator's signature and return type are standard.
    //:
    //:13 The inequality operator's signature and return type are standard.
    //:
    //:14 Two object with the same elements in different permutation compare
    //:   equal to each other.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality- comparison
    //:   operators defined in this component.  (C-8..9, 12..13)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique, specify a set of distinct
    //:   specifications for the 'gg' function.
    //:
    //: 4 For each row 'R1' in the table of P-3: (C-1..7)
    //:
    //:   1 Create a single object, using a comparator that can be disabled and
    //:     a "scratch" allocator, and use it to verify the reflexive
    //:     (anti-reflexive) property of equality (inequality) in the presence
    //:     of aliasing.  (C-3..4)
    //:
    //:   2 For each row 'R2' in the table of P-3: (C-1..2, 5..7)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created from
    //:       'R1' and 'R2', respectively, are expected to have the same value.
    //:
    //:     2 For each of two configurations, 'a' and 'b': (C-1..2, 5..7)
    //:
    //:       1 Create three (object) allocators, 'oax', 'oay' and 'oaz'.
    //:
    //:       2 Create an object 'X', using 'oax', having the value 'R1'.
    //:
    //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
    //:         'oay' in configuration 'b', having the value 'R2', and a
    //:         'maxLoadFactor' of 10.
    //:
    //:       4 Create an object 'Z', using 'oax' in configuration 'a' and
    //:         'oaz' in configuration 'b', having the value 'R2', and a
    //:         'maxLoadFactor' of 0.01.
    //:
    //:       5 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring 'oax', 'oay' and 'oaz' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7)
    //:
    //:       6 Compare the ordering of elements in the lists accessed from
    //:         'Y.elementListRoot()' and 'Z.elementListRoot()', which must
    //:         always be permutations of each other, and set a test-wide flag
    //:         to confirm that the lists have differing orders at least once.
    //:         (C14)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //*  bool operator==(const HashTable& lhs, const HashTable& rhs);
    //*  bool operator!=(const HashTable& lhs, const HashTable& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                        "\n=============================\n");

    if (verbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = bslstl::operator==;
        operatorPtr operatorNe = bslstl::operator!=;

        // quash potential compiler warnings about unused variables

        ASSERT(operatorEq != operatorNe);
    }

    static const struct {
        int         d_line;   // source line number
        int         d_index;  // lexical order
        const char *d_spec;   // specification string, for input to 'gg'
    } DATA[] = {
        //line idx  spec
        //---- ---  --------
        { L_,    0, "" },
        { L_,    1, "A" },
        { L_,    2, "AA" },
        { L_,    3, "AB" },
        { L_,    3, "BA" },
        { L_,    4, "ABC" },
        { L_,    4, "BAC" },
        { L_,    4, "BCA" },
        { L_,    5, "ABD" },
        { L_,    6, "ABCDE" },
        { L_,    6, "CEBAD" },
        { L_,    7, "AAAAA" },
        { L_,    8, "ABCDA" },
        { L_,    9, "ABCDF" },
        { L_,    9, "FDCBA" },
        { L_,   10, "AFKPUZ" },
        { L_,   11, "ABFGKLPQUVZ" },
        { L_,   12, "AAABBCDEEFFFGGGHHIJKKLLLMMMNNOPQQRRRSSSTTUVWWXXXYYZ" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    // Create a variable to confirm that our tested data set has an expected
    // (and important) property.
    bool HAVE_TESTED_DISTINCT_PERMUTATIONS = false;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        const HASHER     HASH  = MakeDefaultFunctor<HASHER>::make();
        const COMPARATOR EQUAL = MakeDefaultFunctor<COMPARATOR>::make();

        // Create first object
        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const float       MAX_LF1  = DEFAULT_MAX_LOAD_FACTOR[lfi];

            const int         LINE1    = DATA[ti].d_line;
            const int         INDEX1   = DATA[ti].d_index;
            const char *const SPEC1    = DATA[ti].d_spec;

            const size_t      LENGTH1     = strlen(SPEC1);
            const size_t      NUM_BUCKETS = ceil(LENGTH1/MAX_LF1);

           if (veryVerbose) { T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(HASH, EQUAL, NUM_BUCKETS, MAX_LF1, &scratch);
                const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int lfj = 0; lfj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfj) {
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const float       MAX_LF2  = DEFAULT_MAX_LOAD_FACTOR[lfj];

                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                const size_t      LENGTH2      = strlen(SPEC2);
                const size_t      NUM_BUCKETS2 = ceil(LENGTH2/MAX_LF2);

                if (veryVerbose) {
                              T_ T_ P_(LINE2) P_(INDEX2) P_(LENGTH2) P(SPEC2) }

                const bool EXP = INDEX1 == INDEX2;  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create three distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    Obj mX(HASH, EQUAL, NUM_BUCKETS,  MAX_LF1, &xa);
                    const Obj& X = gg(&mX, SPEC1);
                    Obj mY(HASH, EQUAL, NUM_BUCKETS2, MAX_LF2, &ya);
                    const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1, X.size(),
                            LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, MAX_LF1, X.maxLoadFactor(),
                            MAX_LF1 == X.maxLoadFactor());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2, Y.size(),
                            LENGTH2 == Y.size());
                    ASSERTV(LINE1, LINE2, CONFIG, MAX_LF2, Y.maxLoadFactor(),
                            MAX_LF2 == Y.maxLoadFactor());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG, X, Y,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (Y != X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, oaym.isTotalSame());

                    if (!HAVE_TESTED_DISTINCT_PERMUTATIONS && EXP) {
                        // Walk the lists of both 'Y' and 'X' to see if they
                        // they are distinct permutations.
                        Link *yCursor = Y.elementListRoot();
                        Link *xCursor = X.elementListRoot();

                        while (yCursor) {
                            ASSERT(xCursor); // lists should be the same length

                            if (!(ImpUtil::extractValue<KEY_CONFIG>(yCursor) ==
                                  ImpUtil::extractValue<KEY_CONFIG>(xCursor)))
                            {
                                HAVE_TESTED_DISTINCT_PERMUTATIONS = true;
                                break;
                            }
                            yCursor = yCursor->nextLink();
                            xCursor = xCursor->nextLink();
                        }
                    }
                }
            }
            }
        }
        }
    }

    ASSERT(HAVE_TESTED_DISTINCT_PERMUTATIONS);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - elementListRoot
    //     - size
    //     - allocator
    //   properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 'elementListRoot' refers to the root of a list with exactly 'size()'
    //:   elements, and a null pointer value if 'size() == 0'.
    //:
    //: 5 'bucketAtIndex' returns a valid bucket for all 0 <= index <
    //:   'numBuckets'.
    //:
    //: 6 QoI: Assert precondition violations for 'bucketAtIndex' when
    //:   'size <= index' are detected in SAFE builds.
    //:
    //: 7 For any value of key, 'bucketIndexForKey' returns a bucket number
    //:   less than 'numBuckets'.
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Value construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the correct allocator is installed with the
    //:       'allocator' method.
    //:
    //:     3 Verify the object all attributes are as expected.
    //:
    //:     4 Use 'verifyListContents' to validate the list rooted at
    //:       'elementListRoot'.
    //:
    //:     5 TBD: Use 'validateBucket to validate the buckets returned by
    //:       'bucketAtIndex'.
    //:
    //:     6 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //:
    //: 2 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    //
    // Testing:
    //*  allocator() const;
    //*  comparator() const;
    //*  hasher() const;
    //*  size() const;
    //*  numBuckets() const;
    //*  maxLoadFactor() const;
    //*  elementListRoot() const;
    //*  bucketAtIndex(SizeType index) const;
    //*  bucketIndexForKey(const KeyType& key) const;
    // ------------------------------------------------------------------------

    static const struct {
        int         d_line;           // source line number
        const char *d_spec;           // specification string
        float       d_maxLoadFactor;  // max load factor
        size_t      d_numBuckets;     // number of buckets
        const char *d_results;        // expected results
    } DATA[] = {
        //line  spec                 result
        //----  --------             ------
        { L_,   "",       1.0,   1,  ""       },
        { L_,   "A",      0.9,   2,  "A"      },
        { L_,   "AB",     0.8,   3,  "AB"     },
        { L_,   "ABC",    0.7,   5,  "ABC"    },
        { L_,   "ABCD",   0.6,   8,  "ABCD"   },
        { L_,   "ABCDE",  0.5,  13,  "ABCDE"  }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    const HASHER     HASH  = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR EQUAL = MakeDefaultFunctor<COMPARATOR>::make();

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE         = DATA[ti].d_line;
            const char *const SPEC         = DATA[ti].d_spec;
            const int         LENGTH       = strlen(DATA[ti].d_results);
            const float       MAX_LF       = DATA[ti].d_maxLoadFactor;
            const size_t      NUM_BUCKETS  = DATA[ti].d_numBuckets;
            const TestValues  EXP(DATA[ti].d_results);

            HASHER hash = HASH;
            setHasherState(bsls::Util::addressOf(hash), ti);
            COMPARATOR comp = EQUAL;
            setComparatorState(bsls::Util::addressOf(comp), ti);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",    veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint",  veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1", veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(hash, comp, NUM_BUCKETS, MAX_LF);
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(hash,
                                            comp,
                                            NUM_BUCKETS,
                                            MAX_LF,
                                            (bslma::Allocator *)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(hash,
                                            comp,
                                            NUM_BUCKETS,
                                            MAX_LF,
                                            &sa1);
                      objAllocatorPtr = &sa1;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(hash,
                                            comp,
                                            NUM_BUCKETS,
                                            MAX_LF,
                                            &sa2);
                      objAllocatorPtr = &sa2;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                      return;
                  } break;
                }

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                         ? da
                                         : sa1;

                // --------------------------------------------------------

                // Verify basic accessor

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, &oa == X.allocator());
                ASSERTV(LINE, SPEC, CONFIG,
                        isEqualComparator(comp, X.comparator()));
                ASSERTV(LINE, SPEC, CONFIG,
                        isEqualHasher(hash, X.hasher()));
                ASSERTV(LINE, SPEC, CONFIG, NUM_BUCKETS <= X.numBuckets());
                ASSERTV(LINE, SPEC, CONFIG,
                        MAX_LF == X.maxLoadFactor());
                ASSERTV(LINE, SPEC, CONFIG, LENGTH == (int)X.size());

                ASSERT(0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           EQUAL,
                                                           EXP,
                                                           LENGTH));

                ASSERT(oam.isTotalSame());

                // --------------------------------------------------------

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify no allocation from the non-object allocator.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
                // The invariant check in the destructor uses the default
                // allocator in SAFE_2 builds.
                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());
#endif

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa1.numBlocksInUse(),
                        0 == sa1.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa2.numBlocksInUse(),
                        0 == sa2.numBlocksInUse());
            }
        }
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);


        if (veryVerbose) printf("\t'bucketAtIndex'\n");
        {
            Obj mX(HASH, EQUAL, 1, 1.0);  const Obj& X = mX;
            int numBuckets = X.numBuckets();
            ASSERT_SAFE_PASS(X.bucketAtIndex(numBuckets - 1));
            ASSERT_SAFE_FAIL(X.bucketAtIndex(numBuckets));
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions
    //
    // Concerns:
    //: 1 Valid generator syntax produces expected results
    //:
    //: 2 Invalid syntax is detected and reported.
    //:
    //: 3 'verifyListContents' confirms there is a one-to-one mapping between
    //:   the supplied list and the expected values array, or both are empty.
    //:
    //: 4 'isValidHashTable' returns 'true' if the supplied arguments can
    //:   create a well-formed hash table anchor, and 'false' otherwise.
    //
    // Plan:
    //: 1 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length:
    //:
    //:   1 Use the primitive generator function 'gg' to set the state of a
    //:     newly created object.
    //:
    //:   2 Verify that 'gg' returns a valid reference to the modified argument
    //:     object.
    //:
    //:   3 Use the basic accessors to verify that the value of the object is
    //:     as expected.  (C-1)
    //:
    //: 2 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'ggg'
    //:   to set the state of a newly created object.
    //:
    //:   1 Verify that 'ggg' returns the expected value corresponding to the
    //:     location of the first invalid value of the 'spec'.  (C-2)
    //
    // Testing:
    //*  int ggg(HashTable *object, const char *spec, int verbose = 1);
    //*  HashTable& gg(HashTable *object, const char *spec);
    //*  verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
    //*  bool isValidHashTable(Link *, const HashTableBucket&, int numBuckets);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    const HASHER     HASH  = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR EQUAL = MakeDefaultFunctor<COMPARATOR>::make();

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        static const struct {
            int         d_line;                 // source line number
            const char *d_spec;                 // specification string
            const char *d_results;  // expected element values
        } DATA[] = {
            //line  spec      results
            //----  --------  -------
            { L_,   "",       ""      },
            { L_,   "A",      "A"     },
            { L_,   "B",      "B"     },
            { L_,   "AB",     "AB"    },
            { L_,   "CD",     "CD"    },
            { L_,   "ABC",    "ABC"   },
            { L_,   "ABCD",   "ABCD"  },
            { L_,   "ABCDE",  "ABCDE" },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);
            const int         curLen = (int)strlen(SPEC);

            Obj mX(HASH, EQUAL, LENGTH, 1.0f, &oa);
            const Obj& X = gg(&mX, SPEC);   // original spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                T_ T_ T_ P(X);
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERT(0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                       EQUAL,
                                                       EXP,
                                                       LENGTH));
        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec;     // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,     }, // control

            { L_,   "A",      -1,     }, // control
            { L_,   " ",       0,     },
            { L_,   ".",       0,     },
            { L_,   "E",       -1,    }, // control
            { L_,   "a",       0,     },
            { L_,   "z",       0,     },

            { L_,   "AE",     -1,     }, // control
            { L_,   "aE",      0,     },
            { L_,   "Ae",      1,     },
            { L_,   ".~",      0,     },
            { L_,   "~!",      0,     },
            { L_,   "  ",      0,     },

            { L_,   "ABC",    -1,     }, // control
            { L_,   " BC",     0,     },
            { L_,   "A C",     1,     },
            { L_,   "AB ",     2,     },
            { L_,   "?#:",     0,     },
            { L_,   "   ",     0,     },

            { L_,   "ABCDE",  -1,     }, // control
            { L_,   "aBCDE",   0,     },
            { L_,   "ABcDE",   2,     },
            { L_,   "ABCDe",   4,     },
            { L_,   "AbCdE",   1,     }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;
            const size_t      LENGTH = (int)strlen(SPEC);

            Obj mX(HASH, EQUAL, LENGTH, 1.5f, &oa);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n", LENGTH);
                 ASSERTV(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that a 'HashTable' object can be constructed into
    //   a (valid) default state, then through use of manipulators brought into
    //   any other valid state default constructor, and finally that the object
    //   destroys all its elements and leaks no memory on destruction.  For the
    //   purposes of testing, the default state will be a 'HashTable' having no
    //   elements, having default constructed hasher, comparator and allocator,
    //   and initially having no buckets.  The primary manipulators will be a
    //   free function that inserts an element of a specific type (created for
    //   the purpose of testing) and the 'removeAll' method.
    //
    // Concerns:
    //: 1 An object created with the value constructor (with or without a
    //:   supplied allocator) has the supplied hasher, comparator, at least the
    //:   initial number of buckets and allocator.
    //:
    //: 2 The number of buckets is 1 or a prime number.
    //:
    //: 3 If the allocator is a 'bsl::allocator' and an allocator is NOT
    //:   supplied to the value constructor, the default allocator in effect at
    //:   the time of construction becomes the object allocator for the
    //:   resulting object.
    //:
    //: 4 If the allocator is not a 'bsl::allocator' and an allocator is NOT
    //:   supplied to the value constructor, the default constructed allocator
    //:   becomes the object allocator for the resulting object.
    //:
    //: 5 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 6 If the allocator is a 'bsl::allocator', supplying a null allocator
    //:   address has the same effect as not supplying an allocator.
    //:
    //: 7 Supplying an allocator to the value constructor has no effect on
    //:   subsequent object values.
    //:
    //: 8 Any memory allocation is from the object allocator.
    //:
    //: 9 There is no temporary allocation from any allocator.
    //:
    //:10 Every object releases any allocated memory at destruction.
    //:
    //:11 QoI: The value constructor allocates no memory if the initial number
    //:   of bucket is 0.
    //:
    //:12 'insertElement' increase the size of the object by 1.
    //:
    //:13 'insertElement' returns the address of the newly added element.
    //:
    //:14 'insertElement' puts the element into the list of element defined by
    //:   'elementListRoot'.
    //:
    //:15 'insertElement' adds an additional element in the bucket returned by
    //:   the 'bucketFromKey' method.
    //:
    //:16 'insertElement' returns a null pointer if adding one more element
    //:   will exceed the 'maxLoadFactor'.
    //:
    //:17 Elements having the same keys (retrieved from the 'extractKey' method
    //:   of the KEY_CONFIG) according to the supplied comparator are inserted
    //:   contiguously of the beginning of the range of existing equivalent
    //:   elements, without changing their relative order.
    //:
    //:18 'removeAll' properly destroys each contained element value.
    //:
    //:19 'removeAll' does not allocate memory.
    //:
    //:20 'setBootstrapMaxLoadFactor' modifies the 'maxLoadFactor' attribute
    //:   unless the supplied value is less than or equal to 'loadFactor'.
    //:
    //:21 'setBootstrapMaxLoadFactor' returns 'true' if it successfully changes
    //:   the 'maxLoadFactor', and 'false' otherwise.
    //:
    //:22 Any argument can be 'const'.
    //:
    //:23 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, value-construct three distinct empty
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Insert 'L - 1' elements in order of increasing value into the
    //:       container.
    //:
    //:     7 Insert the 'L'th value in the presense of exception and use the
    //:       (as yet unproven) basic accessors to verify the container has the
    //:       expected values.  Verify the number of allocation is as expected.
    //:       (C-5..6, 14..15)
    //:
    //:     8 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     9 Invoke 'clear' and verify that the container is empty.  Verify
    //:       that no memory is allocated.  (C-12..13)
    //:
    //:    10 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //:
    //:    11 Insert 'L' distinct elements and record the iterators returned.
    //:
    //:    12 Insert the same 'L' elements again and verify that incrementing
    //:       the iterators returned gives the iterator to the next smallest
    //:       value.
    //:
    //:    13 Perform P-1.2.12 again.  (C-11)
    //
    //
    // Testing:
    //*  HashTable(const HASHER&, const COMPARATOR&, SizeType, const ALLOC&)
    //*  ~HashTable();
    //*  void removeAll();
    //*  insertElement      (test driver function, proxy for basic manipulator)
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::ValueType Element;
    typedef bslalg::HashTableImpUtil       ImpUtil;
    typedef typename Obj::SizeType         SizeType;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOCATOR); }

    const TestValues VALUES;  // contains 52 distinct increasing values


    // Probably want to pick these up as values from some injected policy, so
    // that we can test with stateful variants
    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    const size_t MAX_LENGTH = 9;

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const float    MAX_LF = DEFAULT_MAX_LOAD_FACTOR[lfi];
        const SizeType LENGTH = ti;

        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            const size_t NUM_BUCKETS = ceil(3 * LENGTH/MAX_LF);

            switch (CONFIG) {
              case 'a': {
                  objPtr = new (fa) Obj(HASH, COMPARE, NUM_BUCKETS, MAX_LF);
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj(HASH,
                                        COMPARE,
                                        NUM_BUCKETS,
                                        MAX_LF,
                                        (bslma::Allocator *)0);
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  objPtr = new (fa) Obj(HASH,
                                        COMPARE,
                                        NUM_BUCKETS,
                                        MAX_LF,
                                        &sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
                  return;
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify any attribute allocators are installed properly.

            ASSERTV(MAX_LF, LENGTH, CONFIG, &oa == X.allocator());

            // QoI: Verify no allocation from the object/non-object allocators
            // if no buckets are requested (as per the default constructor).
            if (0 == LENGTH) {
                ASSERTV(MAX_LF, LENGTH, CONFIG, oa.numBlocksTotal(),
                        0 ==  oa.numBlocksTotal());
            }
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            // Record blocks used by the initial bucket array
            const bsls::Types::Int64 INITIAL_OA_BLOCKS  =  oa.numBlocksTotal();

            // Verify attributes of an empty container.
            // Note that not all of these attributes are salient to value.
            // None of these accessors are deemed tested until their own test
            // case, but many witnesses give us some confidence in the state.
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket = X.bucketAtIndex(0);
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.last());

            // Verify that remove-all on a default container has no effect.
            // Specifically, no memory allocated, and the root of list and
            // bucket array are unchanged.

            mX.removeAll();

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(MAX_LF, LENGTH, CONFIG, oa.numBlocksTotal(),
                    INITIAL_OA_BLOCKS ==  oa.numBlocksTotal());
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            // Verify attributes of an empty container.
            // Note that not all of these attributes are salient to value.

            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket2 = X.bucketAtIndex(0);
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.last());

            ASSERTV(MAX_LF, LENGTH, CONFIG, &bucket == &bucket2);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting 'insertElement' (bootstrap function).\n");
            }
            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        oa.numBlocksTotal(),   oa.numBlocksInUse(),
                        oa.numBlocksTotal() == oa.numBlocksInUse());

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

#if 0 // Test for exception safe behavior in full 'insert' test after bootstrap
                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    // This will fail on the initial insert as we must also
                    // create the bucket array, so there is an extra pass.
                    // Not sure why that means the block counts get out of
                    // synch though, is this catching a real bug?
                    ASSERTV(CONFIG, LENGTH,
                            oa.numBlocksTotal(),   oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());


                    bslma::TestAllocatorMonitor tam(&oa);
                    Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                    ASSERT(0 != RESULT);

                    // These tests assume that the object allocator is used
                    // only is stored elements also allocate memory.  This
                    // does not allow for rehashes as the container grows.
                    if (VALUE_TYPE_USES_ALLOCATOR  ||
                                                expectPoolToAllocate(LENGTH)) {
                        ASSERTV(CONFIG, tam.isTotalUp());
                        ASSERTV(CONFIG, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, tam.isTotalSame());
                        ASSERTV(CONFIG, tam.isInUseSame());
                    }

                    // Verify no temporary memory is allocated from the object
                    // allocator.
                    // BROKEN TEST CONDITION
                    // We need to think carefully about how we allow for the
                    // allocation of the bucket-array

                    ASSERTV(CONFIG, LENGTH,
                            oa.numBlocksTotal(),   oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    ASSERTV(CONFIG, LENGTH - 1,
                            KEY_CONFIG::extractKey(VALUES[LENGTH - 1]) ==
                                      ImpUtil::extractKey<KEY_CONFIG>(RESULT));
                    ASSERTV(CONFIG, LENGTH - 1,
                            VALUES[LENGTH - 1] ==
                                    ImpUtil::extractValue<KEY_CONFIG>(RESULT));

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#else
                Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                ASSERT(0 != RESULT);
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(KEY_CONFIG::extractKey(VALUES[LENGTH - 1]),
                                  ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(VALUES[LENGTH - 1],
                                  ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
#endif

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        LENGTH == X.size());

                ASSERT(0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           COMPARE,
                                                           VALUES,
                                                           LENGTH));
                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    int *foundKeys = new int[X.size()];
                    for (SizeType j = 0;j != X.size(); ++j) {
                        foundKeys[j] = 0;
                    }

                    SizeType i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        for (SizeType j = 0; j != X.size(); ++j) {
                            if (BSL_TF_EQ(
                                        KEY_CONFIG::extractKey(VALUES[j]),
                                        ImpUtil::extractKey<KEY_CONFIG>(it))) {
                                ASSERTV(MAX_LF, LENGTH, CONFIG, VALUES[j],
                                        !foundKeys[j]);
                                ++foundKeys[j];
                            }
                        }
                    }
                    SizeType missing = 0;
                    for (SizeType j = 0; j != X.size(); ++j) {
                        if (!foundKeys[j]) { ++missing; }
                    }
                    ASSERTV(MAX_LF, LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundKeys;

                    ASSERTV(MAX_LF, LENGTH, CONFIG, X.size() == i);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'removeAll'.\n"); }
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();

                mX.removeAll();

                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
                ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();

                ASSERTV(MAX_LF, LENGTH, CONFIG, BB == AA);
            }

            // ----------------------------------------------------------------

//            const typename Obj::SizeType bucketCount = X.numBuckets();
            if (veryVerbose) { printf(
                  "\n\tRepeat testing 'insertElement', with memory checks.\n");
            }
            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                for (SizeType tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

#if 0 // Test for exception safe behavior in full 'insert' test after bootstrap
                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    bslma::TestAllocatorMonitor tam(&oa);
                    Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                    ASSERT(0 != RESULT);

                    // The number of buckets should not have changed, so no
                    // reason to allocate a fresh bucket array
                    ASSERTV(MAX_LF, LENGTH, CONFIG, bucketCount, X.numBuckets(),
                            bucketCount == X.numBuckets());

                    // These tests assume that the object allocator is used
                    // only if stored elements also allocate memory.  This
                    // does not allow for rehashes as the container grows.
                    // Hence we run the same test sequence a second time after
                    // clearing the container, so we can validate knowing that
                    // no rehashes should be necessary, and will in fact show
                    // up as a memory use error.  'LENGTH' was the high-water
                    // mark of the initial run on the container before removing
                    // all elements.
                    if ((LENGTH < X.size() && expectPoolToAllocate(LENGTH))
                        || VALUE_TYPE_USES_ALLOCATOR) {
                        ASSERTV(CONFIG, LENGTH, tam.isTotalUp());
                        ASSERTV(CONFIG, LENGTH, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, LENGTH, tam.isTotalSame());
                        ASSERTV(CONFIG, LENGTH, tam.isInUseSame());
                    }

                    ASSERTV(MAX_LF, LENGTH, CONFIG,
                            KEY_CONFIG::extractKey(VALUES[LENGTH - 1]) ==
                                      ImpUtil::extractKey<KEY_CONFIG>(RESULT));
                    ASSERTV(MAX_LF, LENGTH, CONFIG,
                            VALUES[LENGTH - 1] ==
                                    ImpUtil::extractValue<KEY_CONFIG>(RESULT));

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#else
                Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                ASSERT(0 != RESULT);
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(KEY_CONFIG::extractKey(VALUES[LENGTH - 1]),
                                  ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(VALUES[LENGTH - 1],
                                  ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

#endif

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH == X.size());

                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    int *foundKeys = new int[X.size()];
                    for (SizeType j = 0; j != X.size(); ++j) {
                        foundKeys[j] = 0;
                    }

                    size_t i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        for (SizeType j = 0; j != X.size(); ++j) {
                            if (BSL_TF_EQ(
                                        KEY_CONFIG::extractKey(VALUES[j]),
                                        ImpUtil::extractKey<KEY_CONFIG>(it))) {
                                ASSERTV(MAX_LF, LENGTH, CONFIG, VALUES[j],
                                        !foundKeys[j]);
                                ++foundKeys[j];
                            }
                        }
                    }
                    SizeType missing = 0;
                    for (SizeType j = 0; j != X.size(); ++j) {
                        if (!foundKeys[j]) { ++missing; }
                    }
                    ASSERTV(MAX_LF, LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundKeys;

                    ASSERTV(MAX_LF, LENGTH, CONFIG, X.size() == i);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf(
                                "\n\tTesting 'insert' duplicated values.\n"); }
            {
                Link *ITER[MAX_LENGTH + 1];

                // The first loop adds a duplicate in front of each already
                // inserted element
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    ITER[tj] = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != ITER[tj]);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   KEY_CONFIG::extractKey(VALUES[tj]),
                                   ImpUtil::extractKey<KEY_CONFIG>(ITER[tj])));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                 VALUES[tj],
                                 ImpUtil::extractValue<KEY_CONFIG>(ITER[tj])));
                }
                ITER[LENGTH] = 0;

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(), 2 * LENGTH == X.size());

                // The second loop adds another duplicate in front of each
                // the items from the previous loop, and not in the middle of
                // any subranges.
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            ITER[tj] == RESULT->nextLink());
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(), 3 * LENGTH == X.size());
            }

            // ----------------------------------------------------------------

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(MAX_LF, LENGTH, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase1(
                                    typename KEY_CONFIG::ValueType *testValues,
                                    size_t                          numValues)
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //: 1 Execute each methods to verify functionality for simple case.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------


    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR> Obj;
    typedef typename Obj::ValueType  Value;

    bslma::TestAllocator defaultAllocator("defaultAllocator");
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma::TestAllocator objectAllocator("objectAllocator");

    // Sanity check.

    ASSERTV(0 < numValues);
    ASSERTV(8 > numValues);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Construct an empty HashTable.\n");
    }
    {
        // Note that 'HashTable' does not have a default constructor, so we
        // must explicitly supply a default for each attribute.
        Obj x(HASHER(), COMPARATOR(), 0, 1.0f, &objectAllocator);
        const Obj& X = x;
        ASSERTV(0    == X.size());
        ASSERTV(0    <  X.maxSize());
        ASSERTV(0    == defaultAllocator.numBytesInUse());
        ASSERTV(0    == objectAllocator.numBytesInUse());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test use of allocators.\n");
    }
    {
        bslma::TestAllocator objectAllocator1("objectAllocator1");
        bslma::TestAllocator objectAllocator2("objectAllocator2");

        Obj o1(HASHER(), COMPARATOR(), 0, 1.0f, &objectAllocator1);
        const Obj& O1 = o1;
        ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

        for (size_t i = 0; i < numValues; ++i) {
            o1.insert(testValues[i]);
        }
        ASSERTV(numValues == O1.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 == objectAllocator2.numBytesInUse());
    }
    {
        bslma::TestAllocator objectAllocator1("objectAllocator1");
        bslma::TestAllocator objectAllocator2("objectAllocator2");

        Obj o1(HASHER(), COMPARATOR(), 0, 1.0f, &objectAllocator1);
        const Obj& O1 = o1;
        ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

        for (size_t i = 0; i < numValues; ++i) {
            bool isInsertedFlag = false;
            o1.insertIfMissing(&isInsertedFlag, testValues[i]);
            ASSERTV(isInsertedFlag, true == isInsertedFlag);
        }
        ASSERTV(numValues == O1.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 == objectAllocator2.numBytesInUse());


        // Copied code from below, under evaluation
        if (veryVerbose) printf("Use a different allocator\n");
        {
            bslma::TestAllocatorMonitor monitor(&objectAllocator1);
            Obj o2(O1, &objectAllocator2); const Obj& O2 = o2;
            ASSERTV(&objectAllocator2 == O2.allocator().mechanism());
            ASSERTV(monitor.isInUseSame());
            ASSERTV(monitor.isTotalSame());
            ASSERTV(0 <  objectAllocator1.numBytesInUse());
            ASSERTV(0 <  objectAllocator2.numBytesInUse());
        }
        ASSERTV(0 ==  objectAllocator2.numBytesInUse());


        if (veryVerbose) printf("Copy construct O2(O1)\n");

        Obj o2(O1, &objectAllocator1); const Obj& O2 = o2;

        ASSERTV(&objectAllocator1 == O2.allocator().mechanism());

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());

        if (veryVerbose) printf("Default construct O3 and swap with O1\n");
        Obj o3(HASHER(), COMPARATOR(), 0, 1.0f, &objectAllocator1);
        const Obj& O3 = o3;
        ASSERTV(&objectAllocator1 == O3.allocator().mechanism());

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(0         == O3.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());

        bslma::TestAllocatorMonitor monitor1(&objectAllocator1);
        o1.swap(o3);
        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());

        if (veryVerbose) printf("swap O3 with O2\n");
        o3.swap(o2);
        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());
        ASSERTV(!monitor1.isInUseUp());  // Memory usage may go down depending
                                        // on implementation
        ASSERTV(!monitor1.isTotalUp());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());

        ASSERTV(&objectAllocator1 == O1.allocator().mechanism());
        ASSERTV(&objectAllocator1 == O2.allocator().mechanism());
        ASSERTV(&objectAllocator1 == O3.allocator().mechanism());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test primary manipulators/accessors on every permutation.\n");
    }

    native_std::sort(testValues, testValues + numValues);
    do {
        // For each possible permutation of values, insert values, iterate over
        // the resulting container, find values, and then erase values.

        Obj x(HASHER(), COMPARATOR(), 0, 1.0f, &objectAllocator);
        const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Obj y(X, &objectAllocator); const Obj& Y = y;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));

            ASSERTV(i, 0 == X.find(KEY_CONFIG::extractKey(testValues[i])));

            // Test 'insert'.
            Value value(testValues[i]);
            bool isInsertedFlag = false;
            Link *link = x.insertIfMissing(&isInsertedFlag, value);
            ASSERTV(0             != link);
            ASSERTV(true          == isInsertedFlag);
            ASSERTV(KEY_CONFIG::extractKey(testValues[i] ==
                                       ImpUtil::extractKey<KEY_CONFIG>(link)));
            ASSERTV(testValues[i] == ImpUtil::extractValue<KEY_CONFIG>(link));

            // Test size, empty.
            ASSERTV(i + 1 == X.size());
            ASSERTV(0 != X.size());

            // Test insert duplicate key
            ASSERTV(link    == x.insertIfMissing(&isInsertedFlag, value));
            ASSERTV(false   == isInsertedFlag);
            ASSERTV(i + 1   == X.size());

            // Test find
            Link *it     = X.find(KEY_CONFIG::extractKey(testValues[i]));
            ASSERTV(ImpUtil::extractKey<KEY_CONFIG>(link) ==
                    ImpUtil::extractKey<KEY_CONFIG>(it));

            ASSERTV(X != Y);
            ASSERTV(!(X == Y));

            y = x;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));
        }

        ASSERTV(0 != objectAllocator.numBytesInUse());
        ASSERTV(0 == defaultAllocator.numBytesInUse());

        // Use remove(link) on all the elements.
        for (size_t i = 0; i < numValues; ++i) {
            Link *it     = x.find(KEY_CONFIG::extractKey(testValues[i]));
            Link *nextIt = it->nextLink();

            ASSERTV(0       != it);
            ASSERTV(KEY_CONFIG::extractKey(testValues[i] ==
                                         ImpUtil::extractKey<KEY_CONFIG>(it)));
            ASSERTV(testValues[i] == ImpUtil::extractValue<KEY_CONFIG>(it));
            Link *resIt = x.remove(it);
            ASSERTV(resIt == nextIt);

            Link *resFind = x.find(KEY_CONFIG::extractKey(testValues[i]));
            ASSERTV(0 == resFind);

            ASSERTV(numValues - i - 1 == X.size());
        }
    } while (native_std::next_permutation(testValues,
                                          testValues + numValues));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    native_std::random_shuffle(testValues,  testValues + numValues);
    if (veryVerbose) printf("Test 'remove(bslalg::BidirectionalLink *)'.\n");
    {
        Obj x(HASHER(), COMPARATOR(), 0, 1.0f, &objectAllocator);
        const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Value value(testValues[i]);
            Link *result1 = x.insert(value);
            ASSERTV(0 != result1);
            Link *result2 = x.insert(value);
            ASSERTV(0 != result2);
            ASSERTV(result1 != result2);
            ASSERTV(2 * (i + 1) == X.size());

            Link *start;
            Link *end;
            const KeyType& key = KEY_CONFIG::extractKey(testValues[i]);
            x.findRange(&start, &end, key);
            ASSERTV(ImpUtil::extractKey<KEY_CONFIG>(start) == key);
            ASSERTV(ImpUtil::extractKey<KEY_CONFIG>(start->nextLink()) == key);
            ASSERTV(start->nextLink()->nextLink() == end);
        }

        for (size_t i = 0; i < numValues; ++i) {
            Link *const initialRoot = x.elementListRoot();
            KeyType key = ImpUtil::extractKey<KEY_CONFIG>(initialRoot);
            Link *resIt1 = x.remove(x.elementListRoot());
            ASSERTV(initialRoot != resIt1);
            ASSERTV(initialRoot != x.elementListRoot());
            ASSERTV(x.elementListRoot() == resIt1);
            ASSERTV(x.find(key) == resIt1);
            
            ASSERTV(X.size(), (2 * numValues - (2 * (i + 1) - 1)) == X.size());
            Link *resIt2 = x.remove(x.elementListRoot());
            ASSERTV(x.elementListRoot() == resIt2);
            ASSERTV(resIt2 != resIt1);
            ASSERTV(x.find(key) == 0);
            ASSERTV(X.size(), (2 * numValues - 2 * (i + 1)) == X.size());
        }
    }

    if (veryVerbose) {
        printf("Test 'equal' and 'hasher'\n");
    }

}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
#if 0  // Planned test cases, not yet implemented
      case 23: {
        // --------------------------------------------------------------------
        // TESTING PUBLIC TYPEDEFS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING "max" FUNCTIONS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase22,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase22,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'countElementsInBucket'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'findRange'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'find'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase19,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase19,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'insertIfMissing(const KeyType& key);
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'insertIfMissing(bool *, VALUE)'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'insert'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'setMaxLoadFactor'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'find'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

      } break;
#endif
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'rehash...' FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'rehash'"
                            "\n================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase13,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase13,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase13,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase13,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Remaining special cases
        TestDriver_AwkwardMaplike::testCase13();

      } break;
      case 12: {
        // --------------------------------------------------------------------
        //  TESTING 'remove'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'remove'"
                            "\n================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // Remaining special cases
        TestDriver_AwkwardMaplike::testCase12();

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting default constructor"
                            "\n===========================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase11,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase11,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Remaining special cases
//        TestDriver_AwkwardMaplike::testCase11();

        // Degenerate test cases are not available for the default constructor.
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Streaming Functionality"
                            "\n===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Assignment Operator"
                            "\n===========================\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase9,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase9,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Remaining special cases
        TestDriver_AwkwardMaplike::testCase9();

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase8,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase8,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Remaining special cases
        TestDriver_AwkwardMaplike::testCase8();

     } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Remaining special cases
        TestDriver_AwkwardMaplike::testCase7();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        // Note that the 'NonEqualComparableTestType' is not appropriate here.
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Grouped tests
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase_GroupedSharedKeys,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Remaining special cases
        TestDriverForCase6_AwkwardMaplike::testCase6();

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Output (<<) Operator"
                            "\n============================\n");

        if (verbose)
                   printf("There is no output operator for this component.\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

#if 0
        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        // Remaining special cases
//        TestDriver_AwkwardMaplike::testCase4();
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg'"
                            "\n============\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        // Remaining special cases
        TestDriver_AwkwardMaplike::testCase3();

        // Further, need to validate the basic test facilities:
        //   verifyListContents
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BOOTSTRAP CONSTRUCTOR AND PRIMARY MANIPULATORS
        //   This case is implemented as a method of a template test harness,
        //   where the runtime concerns and test plan are documented.  The test
        //   harness will be instantiated and run with a variety of types to
        //   address the template parameter concerns below.  We note that the
        //   bootstrap case has the widest variety of paraterizing concerns to
        //   test, as latest test cases may be able to place additional
        //   requirements on the types that they operate with, but the primary
        //   bootstrap has to validate bringing any valid container into any
        //   valid state for any of the later cases.
        //
        // Concerns:
        //: 1 The class bootstraps with the default template arguments for the
        //:   unordered containers as policy parameters
        //:
        //: 2 The class supports a wide variery of troublesome element types,
        //:   as covered extensively in the template test facility, 'bsltf'.
        //:
        //:*3 STL allocators that are not (BDE) polymorphic, and that never
        //:   propagate on any operations, just like BDE
        //:
        //:*4 STL allocators that are not (BDE) polymorphic, and propagate on
        //:   all possible operations.
        //:
        //: 5 functors that do not const-qualify 'operator()'
        //:
        //: 6 stateful functors
        //:
        //:*7 function pointers as functors
        //:
        //: 8 non-default-constructible functors
        //:
        //: 9 functors overloading 'operator&'
        //:
        //:10 functors overloading 'operator,'
        //:
        //:11 functors that cannot be swapped ('swap' is required to support
        //:   assignment, not bootstrap, default constructor, or most methods)
        //:
        //:*12 functors whose argument(s) are convertible-form the key-type
        //:
        //:*13 functors with templated function-call operators
        //:
        //:14 support for simple set-like policy
        //:
        //:15 support for basic map-like policy, where key is a sub-state of
        //:   the element's value
        //:
        //:16 support for a minimal key type, with equality-comparable element
        //:   type
        //
        // Plan:
        //: 1 Run the test harness in a variety of configurations that each, in
        //:   turn, address the concerns and verify that the behavior is as
        //:   expected.
        //
        // Testing:
        //   BOOTSTRAP
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);


        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

#if 0
        // These 3 cases do not have unique keys, which fools the final part of
        // the basic test case.  Will review test logic later, to re-enable
        // these tests.
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);
#endif
        // Remaining special cases
        TestDriver_AwkwardMaplike::testCase2();

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
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        {
            int INT_VALUES[]   = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
            int NUM_INT_VALUES = sizeof(INT_VALUES) / sizeof(*INT_VALUES);

            typedef BasicKeyConfig<int>          BasicSetOfIntPolicy;
            typedef bsltf::StdTestAllocator<int> StlTestIntAllocator;
            typedef ::bsl::hash<int>             TestIntHash;
            typedef ::bsl::equal_to<int>         TestIntEqual;

            TestDriver<BasicSetOfIntPolicy,
                       TestIntHash,
                       TestIntEqual,
                       StlTestIntAllocator >::testCase1(INT_VALUES,
                                                        NUM_INT_VALUES);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
