// bdlc_hashtable.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bdlc_hashtable.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_objectbuffer.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is a mechanism for inserting, finding, and removing
// based on a given key.  This is *not* a value semantic type.
//
// After breathing the component [1], the implementation struct will be tested
// [2].  Then the default traits will be tested [3].  Then the default hash
// functions will be tested [4].  Then the object's constructors will be tested
// [5].  Once objects can be constructed correctly, the 'insert' and 'remove'
// methods will be tested [6].  Then the 'find' accessor will be tested [7].
// Then the 'value' manipulator will be tested [8].  Finally, the usage example
// in the component header file will be tested [9].
//
// The accessors will be used and tested throughout the test to verify the
// results of the manipulators.
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE
// ----------------------------------------------------------------------------
// [ 2] const unsigned int *ImpUtil::PRIME_NUMBERS;
// [ 2] const unsigned int *ImpUtil::NUM_PRIME_NUMBERS;
// [ 2] unsigned int ImpUtil::hashSize(Int64 hint);
// ----------------------------------------------------------------------------
// [ 3] void DefTraits::load(BUCKET *dst, const BUCKET& src);
// [ 3] bool DefTraits::areEqual(const KEY& key1, const key2);
// [ 3] bool DefTraits::isNull(const BUCKET& bucket);
// [ 3] void DefTraits::setToNull(BUCKET *bucket);
// [ 3] bool DefTraits::isRemoved(const BUCKET& bucket);
// [ 3] void DefTraits::setToRemoved(BUCKET *bucket);
// ----------------------------------------------------------------------------
// [ 4] unsigned int DefHash1::operator()(const KEY& key);
// [ 4] unsigned int DefHash1::operator()(const ConstCharPtr& key);
// [ 4] unsigned int DefHash1::operator()(const bsl::string& key);
// ----------------------------------------------------------------------------
// [ 4] unsigned int DefHash2::operator()(const KEY& key);
// [ 4] unsigned int DefHash2::operator()(const ConstCharPtr& key);
// [ 4] unsigned int DefHash2::operator()(const bsl::string& key);
// ----------------------------------------------------------------------------
// [ 5] bdlc::HashTable(Int64 capacityHint, b_A *ba);
// [ 5] bdlc::HashTable(Int64 capacityHint, HF hf1, HF hf2, b_A *ba);
// [ 5] ~bdlc::HashTable();
// [ 6] bool insert(Handle *handle, const KEY& key);
// [ 6] bool insert(Handle *handle, const KEY& key, const VALUE& value);
// [ 6] void remove(const Handle& handle);
// [ 8] VALUE& value(const Handle& handle);
// [ 5] Int64 capacity() const;
// [ 5] Int64 capacityHint() const;
// [ 7] bool find(Handle *handle, const KEY& key) const;
// [ 6] const KEY& key(const Handle& handle) const;
// [ 6] Int64 maxChain() const;
// [ 6] Int64 numCollisions() const;
// [ 6] Int64 size() const;
// [ 6] Int64 totalChain() const;
// [ 6] const VALUE& value(const Handle& handle) const;

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

namespace {

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

int PRIMES_1000[] = {
      2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,
     53,  59,  61,  67,  71,  73,  79,  83,  89,  97, 101, 103, 107, 109, 113,
    127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197,
    199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
    283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379,
    383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
    467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571,
    577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
    661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761,
    769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
    877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977,
    983, 991, 997,
};

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace TestCase3 {

template <class TYPE>
void testIsNullFootprint(TYPE *ignored)
{
    typedef TYPE                         Bucket;
    typedef bdlc::HashTableDefaultTraits Traits;

    bsls::ObjectBuffer<Bucket> d_bucket;

    bsl::memset(d_bucket.buffer(), 0, sizeof(Bucket));
    ASSERT(Traits::isNull(d_bucket.object()));

    for (int i = 0; i < sizeof(Bucket); ++i) {
        bsl::memset(d_bucket.buffer(), 0, sizeof(Bucket));
        d_bucket.buffer()[i] = 1;
        ASSERT(!Traits::isNull(d_bucket.object()));
    }
}

template <class TYPE>
void testIsRemovedFootprint(TYPE *ignored)
{
    typedef TYPE                         Bucket;
    typedef bdlc::HashTableDefaultTraits Traits;

    bsls::ObjectBuffer<Bucket> d_bucket;

    bsl::memset(d_bucket.buffer(), 0xFF, sizeof(Bucket));
    ASSERT(Traits::isRemoved(d_bucket.object()));

    for (int i = 0; i < sizeof(Bucket); ++i) {
        bsl::memset(d_bucket.buffer(), 0xFF, sizeof(Bucket));
        d_bucket.buffer()[i] = 0;
        ASSERT(!Traits::isRemoved(d_bucket.object()));
    }
}

}  // close namespace TestCase3

namespace TestCase2 {

bool isPrime(unsigned int n)
    // Return true if the specified 'n' is prime, and false otherwise.
{
    unsigned int upper = (unsigned int)(bsl::sqrt((double)n) + 0.5);

    for (unsigned int i = 2; i <= upper; ++i) {
        if (0 == (n % i)) {
            LOOP2_ASSERT(n, i, 0);
            return false;                                             // RETURN
        }
    }

    return true;
}

}  // close namespace TestCase2

namespace TestCase5 {

bool g_testHash1Called;
unsigned int testHash1(int key)
{
    g_testHash1Called = true;
    return 0;
}

bool g_testHash2Called;
unsigned int testHash2(int key)
{
    g_testHash2Called = true;
    return 0;
}

bool g_testHash3Called;
unsigned int testHash3(int key)
{
    g_testHash3Called = true;
    return 0;
}

}  // close namespace TestCase5

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// The following snippets of code illustrate the usage of this component.
// Suppose we wanted to store a table of 'int' keys and 'double' values.  We
// will use a capacity hint of 10 for demonstration purposes:
//..
//  #include <bdlc_hashtable.h>
//
void usageExample()
{
    typedef bdlc::HashTable<int, double> TableType;

    TableType table(10);
//..
// Now we can insert elements into this object:
//..
    TableType::Handle handles[3];

    struct {
        int    d_key;
        double d_value;
    } DATA[] = {
        {  10,   2.34   },
        {  92,   94.2   },
        { 236,   9.1    },
    };

    table.insert(&handles[0], DATA[0].d_key, DATA[0].d_value);
    ASSERT(DATA[0].d_key   == table.key(handles[0]));
    ASSERT(DATA[0].d_value == table.value(handles[0]));

    table.insert(&handles[1], DATA[1].d_key, DATA[1].d_value);
    ASSERT(DATA[1].d_key   == table.key(handles[1]));
    ASSERT(DATA[1].d_value == table.value(handles[1]));

    table.insert(&handles[2], DATA[2].d_key, DATA[2].d_value);
    ASSERT(DATA[2].d_key   == table.key(handles[2]));
    ASSERT(DATA[2].d_value == table.value(handles[2]));
//..
// Now we can find elements in this object using the key:
//..
    TableType::Handle findHandles[3];

    table.find(&findHandles[0], DATA[0].d_key);
    ASSERT(DATA[0].d_key   == table.key(findHandles[0]));
    ASSERT(DATA[0].d_value == table.value(findHandles[0]));

    table.find(&findHandles[1], DATA[1].d_key);
    ASSERT(DATA[1].d_key   == table.key(findHandles[1]));
    ASSERT(DATA[1].d_value == table.value(findHandles[1]));

    table.find(&findHandles[2], DATA[2].d_key);
    ASSERT(DATA[2].d_key   == table.key(findHandles[2]));
    ASSERT(DATA[2].d_value == table.value(findHandles[2]));
}
//..

}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;
    int testAllocatorVerbosity = argc > 6;  // always the last

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(testAllocatorVerbosity);

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example provided in the component header
        //   file.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example from the component's header file, replacing
        //   'assert' with 'ASSERT'.
        //
        // Testing:
        //   Usage example.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                             "\n=====================\n";

        usageExample();

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING VALUE MANIPULATOR
        //   This will test the 'bdlc::HashTable::value(handle)' method.
        //
        // Concerns:
        //   This function must return a reference to the modifiable value
        //   identified by the provided handle.
        //
        // Plan:
        //   Create a 'bdlc::HashTable' object and insert some elements.
        //   Exercise the 'value' manipulator and verify that a reference to
        //   the modifiable value is returned.  Modify the value and check the
        //   result.
        //
        // Testing:
        //   VALUE& value(const Handle& handle);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Value Manipulator"
                             "\n=========================\n";

        if (verbose) cout << "\nTesting using 'bdlc::HashTable"
                             "<int, double>'\n";
        {
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING FIND ACCESSOR
        //   This will test the 'bdlc::HashTable::find(key)' method.
        //
        // Concerns:
        //   This accessor must return false if there is no element with the
        //   specified 'key'.  If the element exists, then the function must
        //   return true and load the correct 'd_index' into the provided
        //   handle.
        //
        // Plan:
        //   Construct a 'bdlc::HashTable' object and insert some elements.
        //   Use the 'find' accessor with a set of keys and check that the
        //   results are as expected.
        //
        //   Run the test for objects with 'VALUE' and also objects without
        //   'VALUE'.
        //
        // Testing:
        //   bool find(Handle *handle, const KEY& key) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Find Accessor"
                             "\n=====================\n";

        if (verbose) cout << "\nTesting with 'bdlc::HashTable<int>'\n";
        {
        }

        if (verbose) cout << "\nTesting with 'bdlc::HashTable"
                          << "<int, double>'\n";
        {
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING INSERT AND REMOVE METHODS
        //   This will test the 'bdlc::HashTable::insert(key)' method , the
        //   'bdlc::HashTable::insert(key, value)' and the
        //   'bdlc::HashTable::remove(handle)' method.
        //
        // Concerns:
        //   If the object is full, or if an element with the specified key
        //   already exists, then the 'insert' function must return false,
        //   otherwise it must return true and load the element in the expected
        //   position.  The returned handle must contain this expected index.
        //   Also, the 'insert' function must reuse table indices that have
        //   been removed.
        //
        // Plan:
        //   For a given scale, prepare a set S of elements that will be
        //   inserted to the 'bdlc::HashTable' object.  After each call to
        //   'insert', use the 'maxChain', 'numCollisions', 'size', and
        //   'totalChain' accessors and verify that they have the expected
        //   values.  Also verify that the handle is as expected.  Use the
        //   'key' and, if appropriate, 'value' accessors to check that the
        //   element has been loaded correctly.
        //
        //   Exercise the 'remove' method by removing an element from the hash
        //   table, then verify that the 'insert' method reuses the removed
        //   table index.
        //
        // Testing:
        //   bool insert(Handle *handle, const KEY& key);
        //   bool insert(Handle *handle, const KEY& key, const VALUE& value);
        //   void remove(const Handle& handle);
        //   const KEY& key(const Handle& handle) const;
        //   Int64 maxChain() const;
        //   Int64 numCollisions() const;
        //   Int64 size() const;
        //   Int64 totalChain() const;
        //   const VALUE& value(const Handle& handle) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Insert Methods"
                             "\n======================\n";

        if (verbose) cout << "\nTesting: scale = 3\n";
        {
        }

        if (verbose) cout << "\nTesting: scale = 7\n";
        {
        }

        if (verbose) cout << "\nTesting: scale = 13\n";
        {
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //   This will test the 'bdlc::HashTable' constructors.
        //
        // Concerns:
        //   The 'scale' parameter must be used to determine the capacity of
        //   the object.  If 'basicAllocator' is not specified, the default
        //   allocator must be used.  If 'basicAllocator' is specified, then
        //   the default allocator must *not* be used.  If hash functions are
        //   specified, they must be used.
        //
        // Plan:
        //   For a given set of scales, in increasing order, construct a
        //   'bdlc::HashTable' object and check that its capacity is as
        //   expected.  Verify that the currently installed default allocator
        //   was used.  Construct another 'bdlc::HashTable' object, this time
        //   specifying an allocator.  Verify that the specified allocator was
        //   used and the default allocator was not used.
        //
        //   For a given set of scales, and a given set of hash functions,
        //   construct a 'bdlc::HashTable' object and check that its capacity
        //   is as expected.  Use the 'insert' method and check that the first
        //   hash function invoked.  Use the 'insert' method again, and check
        //   that this time the second hash function is invoked.  Repeat this
        //   test, this time specifying an allocator.  Verify that the
        //   specified allocator was used and the default allocator was not
        //   used.
        //
        //   For each test, use the 'capacity' and 'scale' accessors to verify
        //   the correctness of the constructor.
        //
        //   Note that the destructor is tested implicitly each time the object
        //   goes out of scope.
        //
        // Testing:
        //   bdlc::HashTable(Int64 scale, b_A *ba);
        //   bdlc::HashTable(Int64 scale, HF hf1, HF hf2, b_A *ba);
        //   ~bdlc::HashTable();
        //   int capacity() const;
        //   int scale() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Constructors"
                             "\n====================\n";

        if (verbose) cout << "\nTesting 'bdlc::HashTable"
                             "(Int64 scale, b_A *ba);'\n";
        {
        }

        if (verbose) cout << "\nTesting 'bdlc::HashTable"
                             "(Int64 scale, HF hf1, HF hf2, b_A *ba);'\n";
        {
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT HASH FUNCTORS
        //   This will test the 'bdlc::HashTableDefaultHash1' struct and the
        //   'bdlc::HashTableDefaultHash2' struct.
        //
        // Concerns:
        //   The operators in this struct must behave as expected.
        //
        // Plan:
        //   For each function in this struct, exercise the function with a
        //   range of arguments.  Compare the returned results with an oracle.
        //
        // Testing:
        //   unsigned int DefHash1::operator()(const KEY& key);
        //   unsigned int DefHash1::operator()(const ConstCharPtr& key);
        //   unsigned int DefHash1::operator()(const bsl::string& key);
        //   unsigned int DefHash2::operator()(const KEY& key);
        //   unsigned int DefHash2::operator()(const ConstCharPtr& key);
        //   unsigned int DefHash2::operator()(const bsl::string& key);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Default Hash Functors"
                             "\n=============================\n";

        if (verbose) cout << "\nTesting 'unsigned int "
                             "DefHash1::operator()(const KEY& key);'\n";
        {
            // Use oracle:
            //     bdlb::HashUtil::hash1((const char*)&key, sizeof key);
            // Use types:
            //     int, double, bsls::TimeInterval

            typedef bdlc::HashTableDefaultHash1 Functor;

            if (veryVerbose) cout << "\tUsing 'int'\n";
            {
                typedef int Key;

                static const struct {
                    int d_lineNum;
                    Key d_key;
                } DATA[] = {
                    //line     key
                    //----     ---
                    { L_,      INT_MIN,         },
                    { L_,      INT_MIN+1,       },
                    { L_,      -283,            },
                    { L_,      -2,              },
                    { L_,      -1,              },
                    { L_,      0,               },
                    { L_,      +1,              },
                    { L_,      +2,              },
                    { L_,      +244,            },
                    { L_,      +7673,           },
                    { L_,      INT_MAX-1,       },
                    { L_,      INT_MAX,         },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int LINE = DATA[i].d_lineNum;
                    const Key KEY  = DATA[i].d_key;

                    unsigned int expected =
                          bdlb::HashUtil::hash1((const char*)&KEY, sizeof KEY);

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(KEY) P(expected)
                    }

                    Functor functor;

                    unsigned int result = functor(KEY);

                    LOOP3_ASSERT(LINE, expected, result, expected == result);
                }
            }

            if (veryVerbose) cout << "\tUsing 'double'\n";
            {
                typedef double Key;

                static const struct {
                    int d_lineNum;
                    Key d_key;
                } DATA[] = {
                    //line     key
                    //----     ---
                    { L_,      -32.435,         },
                    { L_,      -2.982,          },
                    { L_,      -0.1,            },
                    { L_,      -0.0,            },
                    { L_,      +0.0,            },
                    { L_,      +0.1,            },
                    { L_,      +9843.598,       },
                    { L_,      +87325.3,        },
                    { L_,      +908912.32564,   },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int LINE = DATA[i].d_lineNum;
                    const Key KEY  = DATA[i].d_key;

                    unsigned int expected =
                          bdlb::HashUtil::hash1((const char*)&KEY, sizeof KEY);

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(KEY) P(expected)
                    }

                    Functor functor;

                    unsigned int result = functor(KEY);

                    LOOP3_ASSERT(LINE, expected, result, expected == result);
                }
            }

            if (veryVerbose) cout << "\tUsing 'bsls::TimeInterval'\n";
            {
                typedef bsls::TimeInterval Key;

                static const struct {
                    int d_lineNum;
                    Key d_key;
                } DATA[] = {
                    //line     key
                    //----     ---
                    { L_,      bsls::TimeInterval(  5000,       3) },
                    { L_,      bsls::TimeInterval( 12702,    1274) },
                    { L_,      bsls::TimeInterval(167912, 1400074) },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int LINE = DATA[i].d_lineNum;
                    const Key KEY  = DATA[i].d_key;

                    unsigned int expected =
                          bdlb::HashUtil::hash1((const char*)&KEY, sizeof KEY);

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(KEY) P(expected)
                    }

                    Functor functor;

                    unsigned int result = functor(KEY);

                    LOOP3_ASSERT(LINE, expected, result, expected == result);
                }
            }
        }

        if (verbose)
            cout << "\nTesting 'unsigned int "
                    "DefHash1::operator()(const ConstCharPtr& key);'\n";
        {
            // Use oracle:
            //     bdlb::HashUtil::hash1(key, bsl::strlen(key));

            typedef bdlc::HashTableDefaultHash1 Functor;
            typedef const char*                 Key;

            static const struct {
                int d_lineNum;
                Key d_key;
            } DATA[] = {
                //line     key
                //----     ---
                { L_,      "",              },
                { L_,      "a",             },
                { L_,      "ab",            },
                { L_,      "abc",           },
                { L_,      "abcd",          },
                { L_,      "abcde",         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const Key KEY  = DATA[i].d_key;

                unsigned int expected =
                                  bdlb::HashUtil::hash1(KEY, bsl::strlen(KEY));

                if (veryVeryVerbose) {
                    T_ T_ P_(LINE) P_(KEY) P(expected)
                }

                Functor functor;

                unsigned int result = functor(KEY);

                LOOP3_ASSERT(LINE, expected, result, expected == result);
            }
        }

        if (verbose)
            cout << "\nTesting 'unsigned int "
                    "DefHash1::operator()(const bsl::string& key);'\n";
        {
            // Use oracle:
            //     bdlb::HashUtil::hash1(key.data(), key.length());

            typedef bdlc::HashTableDefaultHash1 Functor;
            typedef bsl::string                 Key;

            static const struct {
                int d_lineNum;
                Key d_key;
            } DATA[] = {
                //line     key
                //----     ---
                { L_,      "",              },
                { L_,      "a",             },
                { L_,      "ab",            },
                { L_,      "abc",           },
                { L_,      "abcd",          },
                { L_,      "abcde",         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const Key KEY  = DATA[i].d_key;

                unsigned int expected =
                               bdlb::HashUtil::hash1(KEY.data(), KEY.length());

                if (veryVeryVerbose) {
                    T_ T_ P_(LINE) P_(KEY) P(expected)
                }

                Functor functor;

                unsigned int result = functor(KEY);

                LOOP3_ASSERT(LINE, expected, result, expected == result);
            }
        }

        if (verbose) cout << "\nTesting 'unsigned int "
                             "DefHash2::operator()(const KEY& key);'\n";
        {
            // Use oracle:
            //     bdlb::HashUtil::hash2((const char*)&key, sizeof key);
            // Use types:
            //     int, double, bsls::TimeInterval

            typedef bdlc::HashTableDefaultHash2 Functor;

            if (veryVerbose) cout << "\tUsing 'int'\n";
            {
                typedef int Key;

                static const struct {
                    int d_lineNum;
                    Key d_key;
                } DATA[] = {
                    //line     key
                    //----     ---
                    { L_,      INT_MIN,         },
                    { L_,      INT_MIN+1,       },
                    { L_,      -284,            },
                    { L_,      -6,              },
                    { L_,      -1,              },
                    { L_,      0,               },
                    { L_,      +1,              },
                    { L_,      +8,              },
                    { L_,      +224,            },
                    { L_,      +7683,           },
                    { L_,      INT_MAX-1,       },
                    { L_,      INT_MAX,         },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int LINE = DATA[i].d_lineNum;
                    const Key KEY  = DATA[i].d_key;

                    unsigned int expected =
                          bdlb::HashUtil::hash2((const char*)&KEY, sizeof KEY);

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(KEY) P(expected)
                    }

                    Functor functor;

                    unsigned int result = functor(KEY);

                    LOOP3_ASSERT(LINE, expected, result, expected == result);
                }
            }

            if (veryVerbose) cout << "\tUsing 'double'\n";
            {
                typedef double Key;

                static const struct {
                    int d_lineNum;
                    Key d_key;
                } DATA[] = {
                    //line     key
                    //----     ---
                    { L_,      -31.435,         },
                    { L_,      -3.982,          },
                    { L_,      -0.1,            },
                    { L_,      -0.0,            },
                    { L_,      +0.0,            },
                    { L_,      +0.1,            },
                    { L_,      +9899.598,       },
                    { L_,      +87325.5,        },
                    { L_,      +908999.32564,   },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int LINE = DATA[i].d_lineNum;
                    const Key KEY  = DATA[i].d_key;

                    unsigned int expected =
                          bdlb::HashUtil::hash2((const char*)&KEY, sizeof KEY);

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(KEY) P(expected)
                    }

                    Functor functor;

                    unsigned int result = functor(KEY);

                    LOOP3_ASSERT(LINE, expected, result, expected == result);
                }
            }

            if (veryVerbose) cout << "\tUsing 'bsls::TimeInterval'\n";
            {
                typedef bsls::TimeInterval Key;

                static const struct {
                    int d_lineNum;
                    Key d_key;
                } DATA[] = {
                    //line     key
                    //----     ---
                    { L_,      bsls::TimeInterval(  5000,       3) },
                    { L_,      bsls::TimeInterval( 12702,    1274) },
                    { L_,      bsls::TimeInterval(167912, 1400074) },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int LINE = DATA[i].d_lineNum;
                    const Key KEY  = DATA[i].d_key;

                    unsigned int expected =
                          bdlb::HashUtil::hash2((const char*)&KEY, sizeof KEY);

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(KEY) P(expected)
                    }

                    Functor functor;

                    unsigned int result = functor(KEY);

                    LOOP3_ASSERT(LINE, expected, result, expected == result);
                }
            }
        }


        if (verbose)
            cout << "\nTesting 'unsigned int "
                    "DefHash2::operator()(const ConstCharPtr& key);'\n";
        {
            // Use oracle:
            //     bdlb::HashUtil::hash2(key, bsl::strlen(key));

            typedef bdlc::HashTableDefaultHash2 Functor;
            typedef const char*                  Key;

            static const struct {
                int d_lineNum;
                Key d_key;
            } DATA[] = {
                //line     key
                //----     ---
                { L_,      "",              },
                { L_,      "a",             },
                { L_,      "ab",            },
                { L_,      "abc",           },
                { L_,      "abcd",          },
                { L_,      "abcde",         },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const Key KEY  = DATA[i].d_key;

                unsigned int expected =
                                  bdlb::HashUtil::hash2(KEY, bsl::strlen(KEY));

                if (veryVeryVerbose) {
                    T_ T_ P_(LINE) P_(KEY) P(expected)
                }

                Functor functor;

                unsigned int result = functor(KEY);

                LOOP3_ASSERT(LINE, expected, result, expected == result);
            }
        }


        if (verbose)
            cout << "\nTesting 'unsigned int "
                    "DefHash2::operator()(const bsl::string& key);'\n";
        {
            // Use oracle:
            //     bdlb::HashUtil::hash2(key.data(), key.length());

            typedef bdlc::HashTableDefaultHash2 Functor;
            typedef bsl::string                 Key;

            static const struct {
                int d_lineNum;
                Key d_key;
            } DATA[] = {
                //line     key
                //----     ---
                { L_,      "",              },
                { L_,      "a",             },
                { L_,      "ab",            },
                { L_,      "abc",           },
                { L_,      "abcd",          },
                { L_,      "abcde",         },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNum;
                const Key KEY  = DATA[i].d_key;

                unsigned int expected =
                               bdlb::HashUtil::hash2(KEY.data(), KEY.length());

                if (veryVeryVerbose) {
                    T_ T_ P_(LINE) P_(KEY) P(expected)
                }

                Functor functor;

                unsigned int result = functor(KEY);

                LOOP3_ASSERT(LINE, expected, result, expected == result);
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT TRAITS
        //   This will test the 'bdlc::HashTableDefaultTraits' struct.
        //
        // Concerns:
        //   The 'load' function must be implemented using 'operator='.  The
        //   'areEqual' function must use 'operator==' for all types, except
        //   'const char*', which will use 'strcmp'.  The 'isNull',
        //   'isRemoved', 'setToNull', and 'setToRemoved' functions must work
        //   correctly for all POD types, 'bsl::string', 'const char*', and
        //   'bsl::pair' types.
        //
        // Plan:
        //   To test the 'load' function, specify a set S of (unique) objects
        //   with substantial and varied differences in value.  Construct and
        //   initialize all combinations (u, v) in the cross product S X S,
        //   copy construct a control w from v, load v into u, and assert that
        //   w == u and w == v.  Use 'int' for the 'BUCKET' parameter.
        //
        //   To test the 'areEqual' function, specify a set S of (unique)
        //   objects with similar but different data values.  Then loop through
        //   the cross product of test data.  For each tuple, check that the
        //   'areEqual' function returns the correct result.  Exercise the test
        //   using 'int' and 'const char*' for the 'KEY' parameter.
        //
        //   To test the 'isNull' function, prepare an 'int' with all bytes set
        //   to 0.  Verify that 'isNull' returns true.  Then test the 'isNull'
        //   function using an 'int' where all but one bytes is 0.  Verify that
        //   'isNull' returns false.  Repeat this test with 'const char*' and
        //   also 'bsl::pair<int, int>'.  Next, test the 'isNull' function
        //   using a set of 'bsl::string' objects.  Verify that 'isNull'
        //   returns true only for an empty string.
        //
        //   Test the 'setToNull' function using 'int', 'const char*',
        //   'bsl::pair<int, int>', and 'bsl::string' objects.  Verify that
        //   'isNull' returns true.
        //
        //   To test the 'isRemoved' function, prepare an 'int' with all bytes
        //   set to 0xFF.  Verify that 'isRemoved' returns true.  Then test the
        //   'isRemoved' function using an 'int' where all but one bytes is
        //   0xFF.  Verify that 'isRemoved' returns false.  Repeat this test
        //   with 'const char*' and 'bsl::pair<int, int>'.  Next, test the
        //   'isRemoved' function using a set of 'bsl::string' objects.  Verify
        //   that 'isRemoved' returns true only for "(* REMOVED *)".
        //
        //   Test the 'setToRemoved' function using 'int', 'const char*',
        //   'bsl::pair<int, int>', and 'bsl::string' objects.  Verify that
        //   'isRemoved' returns true.
        //
        // Testing:
        //   void DefTraits::load(BUCKET *dst, const BUCKET& src);
        //   bool DefTraits::areEqual(const KEY& key1, const key2);
        //   bool DefTraits::isNull(const BUCKET& bucket);
        //   void DefTraits::setToNull(BUCKET *bucket);
        //   bool DefTraits::isRemoved(const BUCKET& bucket);
        //   void DefTraits::setToRemoved(BUCKET *bucket);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Default Traits"
                             "\n======================\n";

        typedef bdlc::HashTableDefaultTraits Traits;

        if (verbose) cout << "\nTesting 'load' function.\n";
        {
            typedef int Bucket;

            static const struct {
                int    d_lineNum;
                Bucket d_bucket;
            } DATA[] = {
                //line   bucket
                //----   ------
                { L_,    INT_MIN        },
                { L_,    INT_MIN + 1    },
                { L_,    -2             },
                { L_,    -1             },
                { L_,    0              },
                { L_,    1              },
                { L_,    2              },
                { L_,    INT_MAX - 1    },
                { L_,    INT_MAX        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int    U_LINE   = DATA[i].d_lineNum;
                const Bucket U_BUCKET = DATA[i].d_bucket;

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int    V_LINE   = DATA[j].d_lineNum;
                    const Bucket V_BUCKET = DATA[j].d_bucket;

                    if (veryVerbose) {
                        T_ P_(U_LINE) P_(U_BUCKET) P_(V_LINE) P(V_BUCKET)
                    }

                    Bucket mU(U_BUCKET);  const Bucket& U = mU;
                    Bucket mV(V_BUCKET);  const Bucket& V = mV;

                    const Bucket W(V);

                    Traits::load(&mU, V);

                    LOOP2_ASSERT(U_LINE, V_LINE, U == W);
                    LOOP2_ASSERT(U_LINE, V_LINE, V == W);
                }
            }
        }

        if (verbose) cout << "\nTesting 'areEqual'.\n";
        {
            if (veryVerbose) cout << "\nUsing 'int'\n";
            {
                typedef int Key;

                static const struct {
                    int d_lineNum;
                    Key d_key;
                } DATA[] = {
                    //line     key
                    //----     ---
                    { L_,      INT_MIN,         },
                    { L_,      INT_MIN+1,       },
                    { L_,      -283,            },
                    { L_,      -2,              },
                    { L_,      -1,              },
                    { L_,      0,               },
                    { L_,      +1,              },
                    { L_,      +2,              },
                    { L_,      +244,            },
                    { L_,      +7673,           },
                    { L_,      INT_MAX-1,       },
                    { L_,      INT_MAX,         },
                };

                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int I_LINE = DATA[i].d_lineNum;
                    const Key I_KEY  = DATA[i].d_key;

                    if (veryVeryVerbose) {
                        T_ T_ P_(I_LINE) P(I_KEY)
                    }

                    for (int j = 0; j < NUM_DATA; ++j) {
                        const int J_LINE = DATA[j].d_lineNum;
                        const Key J_KEY  = DATA[j].d_key;

                        if (veryVeryVeryVerbose) {
                            T_ T_ T_ P_(J_LINE) P(J_KEY)
                        }

                        LOOP2_ASSERT(
                                   I_LINE, J_LINE,
                                   (i == j) == Traits::areEqual(I_KEY, J_KEY));
                    }
                }
            }

            if (veryVerbose) cout << "\nUsing 'const char*'\n";
            {
                typedef const char* Key;

                static const struct {
                    int d_lineNum;
                    Key d_key;
                } DATA[] = {
                    //line     key
                    //----     ---
                    { L_,      "",        },
                    { L_,      "a",       },
                    { L_,      "ab",      },
                    { L_,      "abc",     },
                    { L_,      "abcd",    },
                    { L_,      "abcde",   },
                };

                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int I_LINE = DATA[i].d_lineNum;

                    char I[10];
                    bsl::strncpy(I, DATA[i].d_key, sizeof I);

                    const Key I_KEY = I;

                    if (veryVeryVerbose) {
                        T_ T_ P_(I_LINE) P(I_KEY)
                    }

                    for (int j = 0; j < NUM_DATA; ++j) {
                        const int J_LINE = DATA[j].d_lineNum;

                        char J[10];
                        bsl::strncpy(J, DATA[j].d_key, sizeof J);

                        const Key J_KEY = J;

                        if (veryVeryVeryVerbose) {
                            T_ T_ T_ P_(J_LINE) P(J_KEY)
                        }

                        LOOP2_ASSERT(I_LINE, J_LINE, I_KEY != J_KEY);
                        LOOP2_ASSERT(
                                   I_LINE, J_LINE,
                                   (i == j) == Traits::areEqual(I_KEY, J_KEY));
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting 'isNull'.\n";
        {
            if (veryVerbose) cout << "\nUsing 'int'.\n";
            {
                typedef int Bucket;
                TestCase3::testIsNullFootprint((Bucket*)0);
            }

            if (veryVerbose) cout << "\nUsing 'const char*'.\n";
            {
                typedef const char* Bucket;
                TestCase3::testIsNullFootprint((Bucket*)0);
            }

            if (veryVerbose) cout << "\nUsing 'bsl::pair<int, int>'.\n";
            {
                typedef bsl::pair<int, int> Bucket;
                TestCase3::testIsNullFootprint((Bucket*)0);
            }

            if (veryVerbose) cout << "\nUsing 'bsl::string'.\n";
            {
                typedef bsl::string Bucket;

                static const struct {
                    int    d_lineNum;
                    Bucket d_bucket;
                    bool   d_isNull;
                } DATA[] = {
                    //line   bucket               isNull
                    //----   ------               ------
                    { L_,    "",                  true,      },
                    { L_,    "a",                 false,     },
                    { L_,    "ab",                false,     },
                    { L_,    "abc",               false,     },
                    { L_,    "abcd",              false,     },
                    { L_,    "(* REMOVED *)",     false,     },
                };

                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int     LINE    = DATA[i].d_lineNum;
                    const Bucket& BUCKET  = DATA[i].d_bucket;
                    const bool    IS_NULL = DATA[i].d_isNull;

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(BUCKET) P(IS_NULL)
                    }

                    LOOP_ASSERT(LINE, IS_NULL == Traits::isNull(BUCKET));
                }
            }
        }

        if (verbose) cout << "\nTesting 'setToNull'.\n";
        {
            if (veryVerbose) cout << "Using 'int'.\n";
            {
                int obj = 1;
                ASSERT(!Traits::isNull(obj));
                Traits::setToNull(&obj);
                ASSERT(Traits::isNull(obj));
            }

            if (veryVerbose) cout << "Using 'const char*'.\n";
            {
                const char *obj = "Hello";
                ASSERT(!Traits::isNull(obj));
                Traits::setToNull(&obj);
                ASSERT(Traits::isNull(obj));
            }

            if (veryVerbose) cout << "Using 'bsl::pair<int, int>'.\n";
            {
                bsl::pair<int, int> obj(1, 0);
                ASSERT(!Traits::isNull(obj));
                Traits::setToNull(&obj);
                ASSERT(Traits::isNull(obj));
            }
            {
                bsl::pair<int, int> obj(0, 1);
                ASSERT(!Traits::isNull(obj));
                Traits::setToNull(&obj);
                ASSERT(Traits::isNull(obj));
            }
            {
                bsl::pair<int, int> obj(1, 1);
                ASSERT(!Traits::isNull(obj));
                Traits::setToNull(&obj);
                ASSERT(Traits::isNull(obj));
            }

            if (veryVerbose) cout << "Using 'bsl::string'.\n";
            {
                bsl::string obj = "Hello";
                ASSERT(!Traits::isNull(obj));
                Traits::setToNull(&obj);
                ASSERT(Traits::isNull(obj));
            }
        }

        if (verbose) cout << "\nTesting 'isRemoved'.\n";
        {
            if (veryVerbose) cout << "\nUsing 'int'.\n";
            {
                typedef int Bucket;
                TestCase3::testIsRemovedFootprint((Bucket*)0);
            }

            if (veryVerbose) cout << "\nUsing 'const char*'.\n";
            {
                typedef const char* Bucket;
                TestCase3::testIsRemovedFootprint((Bucket*)0);
            }

            if (veryVerbose) cout << "\nUsing 'bsl::pair<int, int>'.\n";
            {
                typedef bsl::pair<int, int> Bucket;
                TestCase3::testIsRemovedFootprint((Bucket*)0);
            }

            if (veryVerbose) cout << "\nUsing 'bsl::string'.\n";
            {
                typedef bsl::string Bucket;

                static const struct {
                    int    d_lineNum;
                    Bucket d_bucket;
                    bool   d_isRemoved;
                } DATA[] = {
                    //line   bucket               isRemoved
                    //----   ------               ---------
                    { L_,    "",                  false,     },
                    { L_,    "a",                 false,     },
                    { L_,    "ab",                false,     },
                    { L_,    "abc",               false,     },
                    { L_,    "abcd",              false,     },
                    { L_,    "(* REMOVED *)",     true,      },
                };

                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int     LINE       = DATA[i].d_lineNum;
                    const Bucket& BUCKET     = DATA[i].d_bucket;
                    const bool    IS_REMOVED = DATA[i].d_isRemoved;

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(BUCKET) P(IS_REMOVED)
                    }

                    LOOP_ASSERT(LINE, IS_REMOVED == Traits::isRemoved(BUCKET));
                }
            }
        }

        if (verbose) cout << "\nTesting 'setToRemoved'.\n";
        {
            if (veryVerbose) cout << "Using 'int'.\n";
            {
                int obj = 0;
                ASSERT(!Traits::isRemoved(obj));
                Traits::setToRemoved(&obj);
                ASSERT(Traits::isRemoved(obj));
            }

            if (veryVerbose) cout << "Using 'const char*'.\n";
            {
                const char *obj = "Hello";
                ASSERT(!Traits::isRemoved(obj));
                Traits::setToRemoved(&obj);
                ASSERT(Traits::isRemoved(obj));
            }

            if (veryVerbose) cout << "Using 'bsl::pair<int, int>'.\n";
            {
                bsl::pair<int, int> obj(1, 1);
                Traits::setToRemoved(&obj.first);
                ASSERT(!Traits::isRemoved(obj));
                Traits::setToRemoved(&obj);
                ASSERT(Traits::isRemoved(obj));
            }
            {
                bsl::pair<int, int> obj(1, 1);
                Traits::setToRemoved(&obj.second);
                ASSERT(!Traits::isRemoved(obj));
                Traits::setToRemoved(&obj);
                ASSERT(Traits::isRemoved(obj));
            }
            {
                bsl::pair<int, int> obj(1, 1);
                ASSERT(!Traits::isRemoved(obj));
                Traits::setToRemoved(&obj);
                ASSERT(Traits::isRemoved(obj));
            }

            if (veryVerbose) cout << "Using 'bsl::string'.\n";
            {
                bsl::string obj = "Hello";
                ASSERT(!Traits::isRemoved(obj));
                Traits::setToRemoved(&obj);
                ASSERT(Traits::isRemoved(obj));
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING IMP UTIL
        //   This will test the 'bdlc::HashTable_ImpUtil' struct.
        //
        // Concerns:
        //   The 'PRIME_NUMBERS' array must contain only prime numbers and they
        //   must be sorted.  All the static functions in this class must
        //   behave as expected.
        //
        // Plan:
        //  Loop through the 'PRIME_NUMBERS' array and verify that all entries
        //  are prime numbers and sorted.  For each function in this struct,
        //  exercise the function with a range of arguments.  Compare the
        //  returned results with an oracle.
        //
        // Testing:
        //   const unsigned int *ImpUtil::PRIME_NUMBERS;
        //   const unsigned int *ImpUtil::NUM_PRIME_NUMBERS;
        //   unsigned int ImpUtil::hashSize(int hint);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Imp Util"
                             "\n================\n";

        typedef bdlc::HashTable_ImpUtil ImpUtil;

        if (verbose) cout << "\nTesting 'PRIME_NUMBERS' array.\n";
        {
            // Use oracle: bool TestCase2::isPrime(unsigned int);

            LOOP_ASSERT(ImpUtil::PRIME_NUMBERS[0],
                        TestCase2::isPrime(ImpUtil::PRIME_NUMBERS[0]));

            for (int i = 1; i < ImpUtil::NUM_PRIME_NUMBERS; ++i) {
                LOOP3_ASSERT(
                    i,
                    ImpUtil::PRIME_NUMBERS[i - 1],  ImpUtil::PRIME_NUMBERS[i],
                    ImpUtil::PRIME_NUMBERS[i - 1] < ImpUtil::PRIME_NUMBERS[i]);

                LOOP2_ASSERT(i, ImpUtil::PRIME_NUMBERS[i],
                             TestCase2::isPrime(ImpUtil::PRIME_NUMBERS[i]));
            }
        }

        if (verbose) cout << "\nTesting 'unsigned int ImpUtil::hashSize"
                             "(int hint);'\n";
        {
            static const struct {
                int                d_lineNum;
                bsls::Types::Int64 d_hint;
                unsigned int       d_expected;
            } DATA[] = {
                //line    hint            expected
                //----    ----            --------
                { L_,     2,              3,            },
                { L_,     3,              3,            },
                { L_,     4,              5,            },
                { L_,     5,              5,            },
                { L_,     6,              7,            },
                { L_,     7,              7,            },
                { L_,     8,              11,           },
                { L_,     9,              11,           },
                { L_,     10,             11,           },
                { L_,     11,             11,           },
                { L_,     12,             13,           },
                { L_,     13,             13,           },
                { L_,     14,             19,           },
                { L_,     15,             19,           },
                { L_,     16,             19,           },
                { L_,     17,             19,           },
                { L_,     18,             19,           },
                { L_,     19,             19,           },
                { L_,     20,             23,           },
                { L_,     21,             23,           },
                { L_,     1000,           1009,         },
                { L_,     10000,          10369,        },
                { L_,     100000,         102523,       },
                { L_,     1000000,        1010203,      },

                { L_,     -1,             3,            },
                { L_,     -2,             3,            },
                { L_,     -3,             3,            },
                { L_,     -4,             3,            },
                { L_,     -5,             5,            },
                { L_,     -6,             5,            },
                { L_,     -7,             7,            },
                { L_,     -8,             7,            },
                { L_,     -9,             7,            },
                { L_,     -10,            7,            },
                { L_,     -11,            11,           },
                { L_,     -12,            11,           },
                { L_,     -13,            13,           },
                { L_,     -14,            13,           },
                { L_,     -15,            13,           },
                { L_,     -16,            13,           },
                { L_,     -17,            13,           },
                { L_,     -18,            13,           },
                { L_,     -19,            19,           },
                { L_,     -20,            19,           },
                { L_,     -21,            19,           },
                { L_,     -1000,          997,          },
                { L_,     -10000,         9413,         },
                { L_,     -100000,        93199,        },
                { L_,     -1000000,       918361,       },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE     = DATA[i].d_lineNum;
                const bsls::Types::Int64 HINT     = DATA[i].d_hint;
                const unsigned int       EXPECTED = DATA[i].d_expected;

                if (veryVerbose) {
                    T_ P_(LINE) P_(HINT) P(EXPECTED)
                }

                unsigned int result = ImpUtil::hashSize(HINT);

                LOOP2_ASSERT(LINE, result, EXPECTED == result);
            }
        }

        if (verbose) cout << "\nEnd of test.\n";
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to run some preliminary functions on this component, and
        //   verify that the results are as expected.
        //
        // Plan:
        //   Construct a 'bdlc::HashTable' object with key and value.  Insert
        //   elements into the object and verify the results.
        //
        //   Repeat with a 'bdlc::HashTable' object with only key.
        //
        // Testing:
        //   This test case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        if (verbose) cout << "\nUsing '<const char*, int>'\n";
        {
            typedef const char                  *Key;
            typedef int                          Value;
            typedef bdlc::HashTable<Key, Value>  Class;

            Class mX(1, &testAllocator);  const Class& X = mX;

            ASSERT(3 == X.capacity());
            ASSERT(1 == X.capacityHint());
            ASSERT(0 == X.maxChain());
            ASSERT(0 == X.numCollisions());
            ASSERT(0 == X.size());
            ASSERT(0 == X.totalChain());

            const Key   KEY   = "Hello";
            const Value VALUE = 4;

            Class::Handle handle;

            ASSERT(true  == mX.insert(&handle, KEY, VALUE));

            ASSERT(3 == X.capacity());
            ASSERT(1 == X.capacityHint());
            ASSERT(0 == X.maxChain());
            ASSERT(0 == X.numCollisions());
            ASSERT(1 == X.size());
            ASSERT(0 == X.totalChain());

            ASSERT(KEY   == X.key(handle));
            ASSERT(VALUE == X.value(handle));
        }

        if (verbose) cout << "\nUsing '<const char*>'\n";
        {
            typedef const char           *Key;
            typedef bdlc::HashTable<Key>  Class;

            Class mX(-3, &testAllocator);  const Class& X = mX;

            ASSERT(3  == X.capacity());
            ASSERT(-3 == X.capacityHint());
            ASSERT(0  == X.maxChain());
            ASSERT(0  == X.numCollisions());
            ASSERT(0  == X.size());
            ASSERT(0  == X.totalChain());

            const Key KEY = "Hello";

            Class::Handle handle;

            ASSERT(true  == mX.insert(&handle, KEY));

            ASSERT(3  == X.capacity());
            ASSERT(-3 == X.capacityHint());
            ASSERT(0  == X.maxChain());
            ASSERT(0  == X.numCollisions());
            ASSERT(1  == X.size());
            ASSERT(0  == X.totalChain());

            ASSERT(KEY == X.key(handle));
        }

        if (verbose) cout << "\nEnd of Test.\n";
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << ".\n";
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
