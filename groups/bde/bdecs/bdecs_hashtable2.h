// bdecs_hashtable2.h                                                 -*-C++-*-
#ifndef INCLUDED_BDECS_HASHTABLE2
#define INCLUDED_BDECS_HASHTABLE2

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a double-hashed table with utility.
//
//@CLASSES:
//  bdecs_HashTable2              : double-hashed table
//  bdecs_HashTable2DefaultTraits : default traits
//  bdecs_HashTable2DefaultHash1  : default hash functor 1
//  bdecs_HashTable2DefaultHash2  : default hash functor 2
//
//@SEE_ALSO: bdeu_hashutil
//
//@AUTHOR: Vladimir Kliatchko (vkliatch)
//         Shezan Baig        (sbaig)
//         Olga Zykova        (ozykova)
//
//@DESCRIPTION: This component provides a mechanism, 'bdecs_HashTable2', for
// efficiently finding elements identified by a parameterized 'KEY'.  Elements
// can also have an associated value by specifying an optional 'VALUE' template
// parameter.  Also, an optional 'TRAITS' parameter can be supplied so that
// clients can override the default traits of the hash table.
//
// The 'bdecs_HashTable2' class achieves efficient lookup by using a
// double-hash algorithm, which will be explained later.  Optional 'HASH1' and
// 'HASH2' parameters can be supplied so that clients can override the default
// hash functions used by the hash table.  Hash functors may also optionally be
// specified at construction time, in case the functors contain state (e.g.,
// if 'bdef_Function' is used).
//
// The constructor for 'bdecs_HashTable2' takes a 'capacityHint' argument.
// This 'capacityHint' is used to calculate the capacity of the hash table
// (i.e., the maximum number of elements that can be stored at any one time).
// Once constructed, the capacity cannot be changed.  The capacity hint can be
// either a positive integer or a negative integer.  If the capacity hint is
// positive, then the capacity of the hash table will be the first available
// prime number larger than, or equal to, the capacity hint.  Otherwise, the
// capacity of the hash table will be the first available prime number smaller
// than, or equal to, the capacity hint.  The list of available prime numbers
// is obtained from an array in the 'bdecs_hashtable2.cpp' file.
//
///Traditional Hash Algorithm
///--------------------------
// A typical hash table implementation uses only a single hash function to
// determine the index in the hash table to store a given element.  This
// approach results in constant time access if there are no collisions.  To
// handle cases where there are hash collisions, the hash table needs to
// maintain a linked list or tree of elements for each index in the table.
// This data structure is illustrated in the diagram below:
//..
//               Hash Table
//               ----------
//
//                :      :
//                :      :
//                :......:
//                :      :
//     index - 2  :      :
//                :______:
//                |      |
//     index - 1  |      |
//                |______|     ______      ______      ______
//                |      |    |      |    |      |    |      |
//      index     |      | -> |      | -> |      | -> |      | -> NULL
//                |______|    |______|    |______|    |______|
//                |      |
//     index + 1  |      |    element1    element2    element3
//                |______|
//                |      |
//     index + 2  |      |
//                |______|
//                :      :
//                :      :
//                :......:
//                :      :
//                :      :
//..
// In the diagram above, 'element1', 'element2', and 'element3' hash to the
// 'index'th bucket in the hash table.  Because of this collision, they are
// maintained in a linked list, which results in linear time complexity.
//
///Double-Hash Algorithm
///---------------------
// The double-hash algorithm improves on the traditional algorithm by using a
// second hash function to compute an increment value.  The index is
// incremented by the increment value until an available bucket is found.
// This augmented algorithm is illustrated in the following diagrams.  Suppose
// we have a hash table that is initially empty:
//..
//                            Hash Table
//                            ----------
//
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                  index - 2  :      :
//                             :______:
//                             |      |
//                  index - 1  |      |
//                             |______|
//                             |      |
//                   index     |      |
//                             |______|
//                             |      |
//                  index + 1  |      |
//                             |______|
//                             |      |
//                  index + 2  |      |
//                             |______|
//                             |      |
//                  index + 3  |      |
//                             |______|
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                             :      :
//..
// Now suppose we insert 'element1'.  The first hash function evaluates to the
// 'index'th bucket in the hash table:
//..
//                            Hash Table
//                            ----------
//
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                  index - 2  :      :
//                             :______:
//                             |      |
//                  index - 1  |      |
//                             |______|     ______
//                             |      |    |      |
//                   index     |      | -> |      |   element1
//                             |______|    |______|
//                             |      |
//                  index + 1  |      |
//                             |______|
//                             |      |
//                  index + 2  |      |
//                             |______|
//                             |      |
//                  index + 3  |      |
//                             |______|
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                             :      :
//..
// Now suppose we want to insert 'element2', for which the first hash function
// also evaluates to the 'index'th bucket in the hash table; however, there is
// a collision.  So, we will calculate an increment using the second hash
// function.  Suppose the increment value is 3, we will insert 'element2' at
// 'index + 3':
//..
//                            Hash Table
//                            ----------
//
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                  index - 2  :      :
//                             :______:
//                             |      |
//                  index - 1  |      |
//                             |______|     ______
//                             |      |    |      |
//            .----  index     |      | -> |      |   element1
//            |                |______|    |______|
//            |                |      |
//            |     index + 1  |      |
//            |                |______|
//            |                |      |
//            |     index + 2  |      |
//            |                |______|     ______
//            |                |      |    |      |
//            `---> index + 3  |      | -> |      |   element2
//                             |______|    |______|
//                             |      |
//                  index + 4  |      |
//                             |______|
//                             |      |
//                  index + 5  |      |
//                             |______|
//                             |      |
//                  index + 6  |      |
//                             |______|
//                             |      |
//                  index + 7  |      |
//                             |______|
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                             :      :
//..
// The entry for 'element2' is said to be "chained" through node 'index'.
//
// Now suppose we want to insert 'element3', for which the first hash function
// also evaluates to the 'index'th bucket in the hash table.  Again, there is a
// collision.  So, we will calculate an increment using the second hash
// function.  Suppose the increment value is 5, we will insert 'element3' at
// 'index + 5':
//..
//                            Hash Table
//                            ----------
//
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                  index - 2  :      :
//                             :______:
//                             |      |
//                  index - 1  |      |
//                             |______|     ______
//                             |      |    |      |
//      .-----.----  index     |      | -> |      |   element1
//      |     |                |______|    |______|
//      |     |                |      |
//      |     |     index + 1  |      |
//      |     |                |______|
//      |     |                |      |
//      |     |     index + 2  |      |
//      |     |                |______|     ______
//      |     |                |      |    |      |
//      |     `---> index + 3  |      | -> |      |   element2
//      |                      |______|    |______|
//      |                      |      |
//      |           index + 4  |      |
//      |                      |______|     ______
//      |                      |      |    |      |
//      `---------> index + 5  |      | -> |      |   element3
//                             |______|    |______|
//                             |      |
//                  index + 6  |      |
//                             |______|
//                             |      |
//                  index + 7  |      |
//                             |______|
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                             :      :
//..
// The entry for 'element3' is also "chained" through node 'index'.
//
// If there is a collision even after applying the increment, then the
// increment can be applied again to form a longer chain, until an available
// bucket is found.  For example, suppose we want to insert 'element4', for
// which the first hash function evaluates to the 'index'th bucket.  Since
// there is a collision, we calculate an increment using the second hash
// function.  Suppose the increment value is 3, we will get another collision
// because 'element2' occupies the bucket at 'index + 3'.  Therefore, we apply
// the increment again and we get 'index + 3 + 3', i.e., 'index + 6'.  This
// bucket is empty, so we can store 'element4' here:
//..
//                            Hash Table
//                            ----------
//
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                  index - 2  :      :
//                             :______:
//                             |      |
//                  index - 1  |      |
//                             |______|     ______
//                             |      |    |      |
//      .-----.----  index     |      | -> |      |   element1
//      |     |                |______|    |______|
//      |     |                |      |
//      |     |     index + 1  |      |
//      |     |                |______|
//      |     |                |      |
//      |     |     index + 2  |      |
//      |     |                |______|     ______
//      |     |                |      |    |      |
//      |     `---> index + 3  |      | -> |      |   element2
//      |     .----            |______|    |______|
//      |     |                |      |
//      |     |     index + 4  |      |
//      |     |                |______|     ______
//      |     |                |      |    |      |
//      `-----+---> index + 5  |      | -> |      |   element3
//            |                |______|    |______|
//            |                |      |    |      |
//            `---> index + 6  |      | -> |      |   element4
//                             |______|    |______|
//                             |      |
//                  index + 7  |      |
//                             |______|
//                             :      :
//                             :      :
//                             :......:
//                             :      :
//                             :      :
//..
// The entry for 'element4' is chained through nodes 'index' and 'index + 3'.
//
// If the total number of buckets in the hash table and the increment value
// are relatively prime (i.e., their greatest common divisor is 1), then it is
// guaranteed that every bucket will be visited before looping back to 'index'.
//
// The 'bdecs_HashTable2' container makes sure that the number of buckets in
// the hash table and the increment values are relatively prime.  The
// 'bdecs_HashTable2' container also keeps track of the maximum chain length,
// number of collisions, and the total chain length, which can be used for
// statistical purposes when evaluating different hash functions.
//
///Bucket Type
///-----------
// The 'bdecs_HashTable2' class treats individual buckets as value-semantic
// types.  The type of the buckets depends on the 'KEY' and 'VALUE' parameters
// used to instantiate the 'bdecs_HashTable2' template.  If the 'VALUE'
// parameter is 'bslmf_Nil', then the type of the buckets is 'KEY'.  Otherwise,
// the type of the buckets is 'bsl::pair<KEY, VALUE>'.  For convenience, we
// will refer to the bucket type as 'Bucket' throughout this documentation.
//
// The 'bdecs_HashTable2' class reserves two distinct values from 'Bucket's
// value-space to represent a "null" bucket and a "removed" bucket.  These
// values are determined by the 'TRAITS' parameter, which is described in the
// next section.  Since these two values are reserved for the internal use of
// the 'bdecs_HashTable2' class, the behavior is undefined if one of these
// values is inserted into the hash table.  Taking these values from the
// value-space of 'Bucket' allows the storage space required for each bucket to
// be as compact as possible.
//
///Traits
///------
// An optional 'TRAITS' parameter can be specified when instantiating the
// 'bdecs_HashTable2' template.  This component provides a default traits
// implementation, 'bdecs_HashTable2DefaultTraits', which will be described
// later.
//
// The 'TRAITS' parameter allows clients to specify how to load a bucket and
// how to compare keys.  It also allows clients to classify two distinct values
// to represent "null" and "removed" buckets (see "Bucket Type" for more
// information about these reserved values).
//
// In the following description, 'key1' and 'key2' refer to objects of type
// 'KEY'.  'bucket', 'dstBucket', and 'srcBucket' refer to objects of type
// 'Bucket'.
//
// The following expressions must be supported by the 'TRAITS' parameter:
//..
//  Expression                            Semantics
//  ----------                            ---------
//  TRAITS::load(&dstBucket, srcBucket)   Load the value of the specified
//                                        'srcBucket' into the specified
//                                        'dstBucket'.
//
//  TRAITS::areEqual(key1, key2)          Return true if the specified 'key1'
//                                        matches the specified 'key2', and
//                                        false otherwise.
//
//  TRAITS::isNull(bucket)                Return true if the specified 'bucket'
//                                        has the reserved "null" value, and
//                                        false otherwise.
//
//  TRAITS::setToNull(&bucket)            Load the reserved "null" value into
//                                        the specified 'bucket'.
//
//  TRAITS::isRemoved(bucket)             Return true if the specified 'bucket'
//                                        has the reserved "removed" value, and
//                                        false otherwise.
//
//  TRAITS::setToRemoved(&bucket)         Load the reserved "removed" value
//                                        into the specified 'bucket'.
//..
//
///Default Traits
///- - - - - - - -
// The default traits, identified by 'bdecs_HashTable2DefaultTraits', can be
// used when 'KEY' and 'VALUE' are either:
//..
//    o const char*
//    o bsl::string
//    o POD types
//..
// The following expressions are implemented as:
//..
//  Expression                                Implementation
//  ----------                                --------------
//  TRAITS::load(&dstBucket, srcBucket)       This function is implemented as
//                                            '*dstBucket = srcBucket'.
//
//  TRAITS::areEqual(key1, key2)              If 'KEY' is 'const char*', this
//                                            function is implemented as
//                                            'bsl::strcmp(key1, key2)'.
//                                            Otherwise, this function is
//                                            implemented as 'key1 == key2'.
//..
// The 'isNull', 'setToNull', 'isRemoved', and 'setToRemoved' functions are
// implemented by checking for and assigning the appropriate "null" or
// "removed" values, respectively.  These values are defined in the following
// table:
//..
//  Bucket Type        Null Value                    Removed Value
//  -----------        ----------                    -------------
//  const char*        0x00000000 address            0xFFFFFFFF address
//
//  bsl::string        ""                            "(* REMOVED *)"
//
//  All other types    All bytes in the footprint    All bytes in the footprint
//                     are 0x00                      are 0xFF
//..
// If 'Bucket' is of type 'bsl::pair<KEY, VALUE>', then the "null" and
// "removed" values are applied to both the 'KEY' and the 'VALUE'.
//
// Since the default traits may write directly into the footprint of the bucket
// (except for 'bsl::string'), it is important to note that the 'KEY' and
// 'VALUE' types should be POD types if the default traits are used.
//
///Hash Functors
///-------------
// Optional 'HASH1' and 'HASH2' parameters can be specified when instantiating
// the 'bdecs_HashTable2' template.  This component provides a default hash
// functors, 'bdecs_HashTable2DefaultHash1' and 'bdecs_HashTable2DefaultHash2',
// which will be described later.
//
// The 'HASH1' and 'HASH2' parameters allow clients to specify hash functor
// policies for the first and second hash functions, respectively.
//
// In the following description, 'key' refers to an object of type 'KEY', and
// 'functor' refers to an immutable object of type 'HASH1' or 'HASH2'.
//
// The following expression must be supported by the supplied 'HASH1' and
// 'HASH2' parameters:
//..
//  Expression      Semantics                                      Return Type
//  ----------      ---------                                      -----------
//  functor(&key)   Return a hash value for the specified 'key'    unsigned int
//..
//
///Default Hash Functors
///- - - - - - - - - - -
// The default hash functors, identified by 'bdecs_HashTable2DefaultHash1' and
// 'bdecs_HashTable2DefaultHash2', can be used when 'KEY' is either:
//..
//    o const char*
//    o bsl::string
//    o a POD type
//..
// The 'bdecs_HashTable2DefaultHash1' functor is implemented using
// 'bdeu_HashUtil::hash1' and the 'bdecs_HashTable2DefaultHash2' functor is
// implemented using 'bdeu_HashUtil::hash2'.
//
// Note that 'bdeu_HashUtil::hash1' and 'bdeu_HashUtil::hash2' calculate hash
// value from a fixed length block of memory.  This block of memory is obtained
// based on the following table:
//..
//  KEY Type            Block Data                             Block Length
//  --------            ----------                             ------------
//  const char*         key                                    bsl::strlen(key)
//
//  bsl::string         key.data()                             key.length()
//
//  All other types     reinterpret_cast<const char *>(&key)   sizeof(key)
//..
// Since the default hash functors use the footprint of the key (except for
// 'const char*' and 'bsl::string') to compute hash values, it is important to
// note that the 'KEY' type should be a POD type if the default hash functors
// are used.
//
///Disabling Support for 'remove'
///------------------------------
// By default (i.e., when using the default traits), the 'remove' method can be
// used to remove an element from the hash table.  However, there are cases
// when it is desirable not to allow elements to be removed.  This can be
// achieved by parameterizing the 'bdecs_HashTable2' template with a 'TRAITS'
// parameter that:
//..
//    o always returns false for the 'TRAITS::isRemoved(bucket)' expression
//    o AND does not implemented the 'TRAITS::setToRemoved(&bucket)' expression
//..
// This effectively describes a trait that does not define a special "removed"
// bucket value.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we wanted to store a table of 'int' keys with 'double' values.  We
// will use a capacity hint of 10, default traits, and default hash functors
// for demonstration purposes:
//..
//  #include <bdecs_hashtable2.h>
//
//  using namespace BloombergLP;
//
//  void usageExample()
//  {
//      typedef bdecs_HashTable2<int, double> TableType;
//
//      TableType table(10);
//..
// Now we can insert elements into this object:
//..
//      TableType::Handle handles[3];
//
//      struct {
//          int    d_key;
//          double d_value;
//      } DATA[] = {
//          {  10,   2.34   },
//          {  92,   94.2   },
//          { 236,   9.1    },
//      };
//
//      table.insert(&handles[0], DATA[0].d_key, DATA[0].d_value);
//      assert(DATA[0].d_key   == table.key(handles[0]));
//      assert(DATA[0].d_value == table.value(handles[0]));
//
//      table.insert(&handles[1], DATA[1].d_key, DATA[1].d_value);
//      assert(DATA[1].d_key   == table.key(handles[1]));
//      assert(DATA[1].d_value == table.value(handles[1]));
//
//      table.insert(&handles[2], DATA[2].d_key, DATA[2].d_value);
//      assert(DATA[2].d_key   == table.key(handles[2]));
//      assert(DATA[2].d_value == table.value(handles[2]));
//..
// Now we can find elements in this object using the key:
//..
//      TableType::Handle otherHandles[3];
//
//      table.find(&otherHandles[0], DATA[0].d_key);
//      assert(DATA[0].d_key   == table.key(otherHandles[0]));
//      assert(DATA[0].d_value == table.value(otherHandles[0]));
//
//      table.find(&otherHandles[1], DATA[1].d_key);
//      assert(DATA[1].d_key   == table.key(otherHandles[1]));
//      assert(DATA[1].d_value == table.value(otherHandles[1]));
//
//      table.find(&otherHandles[2], DATA[2].d_key);
//      assert(DATA[2].d_key   == table.key(otherHandles[2]));
//      assert(DATA[2].d_value == table.value(otherHandles[2]));
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>  // @DEPRECATED
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BDEU_HASHUTIL
#include <bdeu_hashutil.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif


namespace BloombergLP {

// FORWARD DECLARATIONS

class bslma_Allocator;

struct bdecs_HashTable2DefaultTraits;
struct bdecs_HashTable2DefaultHash1;
struct bdecs_HashTable2DefaultHash2;

          // ========================================================
          // class bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>
          // ========================================================

template <typename KEY,
          typename VALUE  = bslmf_Nil,
          typename TRAITS = bdecs_HashTable2DefaultTraits,
          typename HASH1  = bdecs_HashTable2DefaultHash1,
          typename HASH2  = bdecs_HashTable2DefaultHash2>
class bdecs_HashTable2 {
    // This class is a double-hashed table.  The 'VALUE' template parameter is
    // optional.  The 'capacityHint' specified at construction time will be
    // used to compute the number of buckets (capacity) in this object.  Also,
    // two hash functions may optionally be specified at construction time.
    // Elements can be inserted using the 'insert' method.  If the 'VALUE'
    // parameter is not 'bslmf_Nil', then both key and value must be supplied
    // to the 'insert' method.  Otherwise, only the key should be supplied.
    // The 'find' method can be used to lookup elements by a specified key.
    // The optional 'TRAITS' parameter can be used to classify "null" and
    // "removed" values.  See the component-level documentation for more
    // details.

  public:
    // TYPES
    typedef bsls_Types::Int64 Handle;
        // Data type to handle elements in the double-hashed table.  This value
        // is guaranteed to be between 0 and the capacity of the hash table.

  private:
    // PRIVATE TYPES
    typedef typename
    bslmf_If<bslmf_IsSame<bslmf_Nil, VALUE>::VALUE,
             KEY, bsl::pair<KEY, VALUE> >::Type Bucket;
        // Type of the element stored in this object.  If the 'VALUE' parameter
        // is 'bslmf_Nil', then 'Bucket' is of type 'KEY', otherwise 'Bucket'
        // is of type 'bsl::pair<KEY, VALUE>'.

    typedef bslalg_ConstructorProxy<HASH1> Hash1CP;
        // Constructor proxy for 'HASH1'.

    typedef bslalg_ConstructorProxy<HASH2> Hash2CP;
        // Constructor proxy for 'HASH2'.

    // DATA
    bsl::vector<Bucket> d_buckets;        // array of buckets
    bsls_Types::Int64   d_capacityHint;   // capacity hint
    Hash1CP             d_hashFunctor1;   // first hash function
    Hash2CP             d_hashFunctor2;   // second hash function
    bsls_Types::Int64   d_maxChain;       // maximum chain length
    bsls_Types::Int64   d_numCollisions;  // number of collisions
    bsls_Types::Int64   d_numElements;    // number of elements
    bsls_Types::Int64   d_totalChain;     // total chain length

    // NOT IMPLEMENTED
    bdecs_HashTable2(const bdecs_HashTable2&);
    bdecs_HashTable2& operator=(const bdecs_HashTable2&);

    // PRIVATE CLASS METHODS
    static const KEY& keyFromBucket(const KEY& bucket);
    static const KEY& keyFromBucket(const bsl::pair<KEY, VALUE>& bucket);
        // Return the key from the specified 'bucket'.  If 'bucket' is of type
        // 'KEY', then 'bucket' is returned.  If 'bucket' is of type
        // 'bsl::pair<KEY, VALUE>', then 'bucket.first' is returned.

    // PRIVATE MANIPULATORS
    void loadElementAt(Handle            *handle,
                       bsls_Types::Int64  index,
                       const Bucket&      element,
                       bsls_Types::Int64  chainLength);
        // Load the specified 'element' into the bucket with the specified
        // 'index'; load a handle to the element in the specified 'handle';
        // update chain statistics with the specified 'chainLength'.

    bool insertElement(Handle *handle, const Bucket& element);
        // Insert the specified 'element' into this object; load a handle to
        // the element into the specified 'handle'.  Return true if successful,
        // and false otherwise.

    // PRIVATE ACCESSORS
    void findImp(bool              *isKeyFound,
                 bsls_Types::Int64 *index,
                 bsls_Types::Int64 *chainLength,
                 bsls_Types::Int64 *removedIndex,
                 const KEY&         key) const;
        // Implement the double-hash algorithm to find a bucket with the
        // specified 'key'; load true into the specified 'isKeyFound' if the an
        // element with the specified 'key' is found, and false otherwise; load
        // the index of the bucket into the specified 'index' if an element
        // with the specified 'key' is found, and the index of the "null"
        // bucket that terminates the chain otherwise; load the chain length
        // into the specified 'chainLength'; load the index of the first
        // "removed" bucket along the chain into the specified 'removedIndex',
        // or -1 if no "removed" buckets were found.  Note that if the key is
        // not found and there are no "null" buckets to terminate the chain,
        // then -1 will be loaded into 'index'.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdecs_HashTable2,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit bdecs_HashTable2(bsls_Types::Int64  capacityHint,
                              bslma_Allocator   *basicAllocator = 0);
        // Create a double-hash table using the specified 'capacityHint'; use
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 != capacityHint'.  Note
        // that 'capacityHint' can be either a positive integer or a negative
        // integer.  If 'capacityHint' is positive, then the capacity of the
        // hash table will be the first available prime number larger than, or
        // equal to, 'capacityHint'.  Otherwise, the capacity of the hash table
        // will be the first available prime number smaller than, or equal to,
        // 'capacityHint'.  Also note that 'HASH1' will be used as the first
        // hash function, and 'HASH2' will be used as the second hash
        // function.

    bdecs_HashTable2(bsls_Types::Int64  capacityHint,
                     const HASH1&       hashFunctor1,
                     const HASH2&       hashFunctor2,
                     bslma_Allocator   *basicAllocator = 0);
        // Create a double-hash table with the specified 'capacityHint'; use
        // the specified 'hashFunctor1' as the first hash function; use the
        // specified 'hashFunctor2' as the second hash function; use the
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  The behavior
        // is undefined unless '0 != capacityHint', and 'hashFunction1' and
        // 'hashFunction2' are valid.  Note that 'capacityHint' can be either a
        // positive integer or a negative integer.  If 'capacityHint' is
        // positive, then the capacity of the hash table will be the first
        // available prime number larger than, or equal to, 'capacityHint'.
        // Otherwise, the capacity of the hash table will be the first
        // available prime number smaller than, or equal to, 'capacityHint'.

    ~bdecs_HashTable2();
        // Destroy this object.

    // MANIPULATORS
    bool insert(Handle *handle, const KEY& key);
        // Insert an element with the specified 'key' into this object; load a
        // handle to the new element into the specified 'handle'.  Return true
        // if successful, and false otherwise.  The behavior is undefined
        // unless 'key' does not evaluate to a "null" or "removed" bucket, as
        // defined by the parameterized 'TRAITS' (see the component-level
        // documentation for more details).  Note that this method will fail to
        // compile unless the 'VALUE' parameter is 'bslmf_Nil'.

    bool insert(Handle *handle, const KEY& key, const VALUE& value);
        // Insert an element with the specified 'key' and the specified 'value'
        // into this object; load a handle to the new element into the
        // specified 'handle'.  Return true if successful, and false otherwise.
        // The behavior is undefined unless 'key' and 'value' do not evaluate
        // to a "null" or "removed" bucket, as defined by the parameterized
        // 'TRAITS' (see the component-level documentation for more details).
        // This method will fail to compile unless the 'VALUE' parameter is not
        // 'bslmf_Nil'.

    void remove(const Handle& handle);
        // Remove the element identified by the specified 'handle' from this
        // object.  The behavior is undefined unless 'handle' is valid.  Note
        // that 'handle' will become invalid when this method returns.

    VALUE& value(const Handle& handle);
        // Return the reference to the modifiable value of the element
        // identified by the specified 'handle'.  The behavior is undefined
        // unless 'handle' is valid.  Note that this method will fail to
        // compile unless the 'VALUE' parameter is not 'bslmf_Nil'.

    // ACCESSORS
    bsls_Types::Int64 capacity() const;
        // Return the maximum number of elements that can be stored in this
        // object.  Note that this value is computed based on the capacity hint
        // used upon construction.

    bsls_Types::Int64 capacityHint() const;
        // Return the capacity hint that was used to determine the capacity of
        // this object.

    bool find(Handle *handle, const KEY& key) const;
        // Find an element having the specified 'key'; load a handle to the
        // element into the specified 'handle'.  Return true if successful, and
        // false otherwise.

    const KEY& key(const Handle& handle) const;
        // Return the reference to the non-modifiable key of the element
        // identified by the specified 'handle'.  The behavior is undefined
        // unless 'handle' is valid.

    bsls_Types::Int64 maxChain() const;
        // Return the maximum chain length encountered by this object.

    bsls_Types::Int64 numCollisions() const;
        // Return the number of collisions encountered by this object.

    bsls_Types::Int64 size() const;
        // Return the number of elements stored in this object.

    bsls_Types::Int64 totalChain() const;
        // Return the total chain length encountered by this object.

    const VALUE& value(const Handle& handle) const;
        // Return the reference to the non-modifiable value of the element
        // identified by the specified 'handle'.  The behavior is undefined
        // unless 'handle' is valid.  Note that this method will fail to
        // compile unless the 'VALUE' parameter is not 'bslmf_Nil'.
};

                    // ====================================
                    // struct bdecs_HashTable2DefaultTraits
                    // ====================================

struct bdecs_HashTable2DefaultTraits {
    // Default traits provided by this component.  See component-level
    // documentation for more details.  Note that this class is not intended to
    // be used by clients, but the name of this struct must be public so that
    // clients can explicitly specify this struct when default traits are
    // needed.

  private:
    // TYPES
    typedef const char* ConstCharPtr;     // Alias for 'const char*'.

    // CONSTANTS
    static const char REMOVED_KEYWORD[];  // Keyword to be used for removed
                                          // objects for 'bsl::string' types.

  public:
    // CLASS METHODS
    template <typename BUCKET>
    static void load(BUCKET *dstBucket, const BUCKET& srcBucket);
        // Load the specified 'srcBucket' into the specified 'dstBucket'.

    template <typename KEY>
    static bool areEqual(const KEY& key1, const KEY& key2);
    static bool areEqual(const ConstCharPtr& key1, const ConstCharPtr& key2);
        // Return true if the specified 'key1' and the specified 'key2' are
        // equal, and false otherwise.

    template <typename BUCKET>
    static bool isNull(const BUCKET& bucket);
    static bool isNull(const bsl::string& bucket);
    static bool isNull(const ConstCharPtr& bucket);
    template <typename KEY, typename VALUE>
    static bool isNull(const bsl::pair<KEY, VALUE>& bucket);
        // Return true if the specified 'bucket' has a null value, and false
        // otherwise.

    template <typename BUCKET>
    static void setToNull(BUCKET *bucket);
    static void setToNull(bsl::string *bucket);
    static void setToNull(ConstCharPtr *bucket);
    template <typename KEY, typename VALUE>
    static void setToNull(bsl::pair<KEY, VALUE> *bucket);
        // Load a null value into the specified 'bucket'.

    template <typename BUCKET>
    static bool isRemoved(const BUCKET& bucket);
    static bool isRemoved(const bsl::string& bucket);
    static bool isRemoved(const ConstCharPtr& bucket);
    template <typename KEY, typename VALUE>
    static bool isRemoved(const bsl::pair<KEY, VALUE>& bucket);
        // Return true if the specified 'bucket' has a removed value, and false
        // otherwise.

    template <typename BUCKET>
    static void setToRemoved(BUCKET *bucket);
    static void setToRemoved(bsl::string *bucket);
    static void setToRemoved(ConstCharPtr *bucket);
    template <typename KEY, typename VALUE>
    static void setToRemoved(bsl::pair<KEY, VALUE> *bucket);
        // Load a removed value into the specified 'bucket'.
};

                    // ===================================
                    // struct bdecs_HashTable2DefaultHash1
                    // ===================================

struct bdecs_HashTable2DefaultHash1 {
    // Default hash function provided by this component.  See component-level
    // documentation for more details.  Note that this class is not intended to
    // be used by clients, but the name of this struct must be public so that
    // clients can explicitly specify this struct when default hash function is
    // needed.  Note that this functor is implemented using
    // 'bdeu_HashUtil::hash1'.

    // TYPES
    typedef const char* ConstCharPtr;  // Alias for 'const char*'.

    // CLASS METHODS
    template <typename KEY>
    unsigned int operator()(const KEY& key) const;
    unsigned int operator()(const ConstCharPtr& key) const;
    unsigned int operator()(const bsl::string& key) const;
        // Return the result of 'bdeu_HashUtil::hash1' using key data and key
        // length.  If 'key' is not of type 'const char*' or 'bsl::string',
        // then the footprint and size of the object are used as key data and
        // key length, respectively.
};

                    // ===================================
                    // struct bdecs_HashTable2DefaultHash2
                    // ===================================

struct bdecs_HashTable2DefaultHash2 {
    // Default hash function provided by this component.  See component-level
    // documentation for more details.  Note that this class is not intended to
    // be used by clients, but the name of this struct must be public so that
    // clients can explicitly specify this struct when default hash function is
    // needed.  Note that this functor is implemented using
    // 'bdeu_HashUtil::hash2'.

    // TYPES
    typedef const char* ConstCharPtr;  // Alias for 'const char*'.

    // CLASS METHODS
    template <typename KEY>
    unsigned int operator()(const KEY& key) const;
    unsigned int operator()(const ConstCharPtr& key) const;
    unsigned int operator()(const bsl::string& key) const;
        // Return the result of 'bdeu_HashUtil::hash2' using key data and key
        // length.  If 'key' is not of type 'const char*' or 'bsl::string',
        // then the footprint and size of the object are used as key data and
        // key length, respectively.
};

//  ---  Anything below this line is implementation specific.  Do not use.  ---

                  // =======================================
                  // private struct bdecs_HashTable2_ImpUtil
                  // =======================================

struct bdecs_HashTable2_ImpUtil {
    // Component-private struct.  Do not use.  Implementation helper functions
    // for this component.

    // CLASS DATA
    static const unsigned int *PRIME_NUMBERS;      // provide access to the
    static const int           NUM_PRIME_NUMBERS;  // array of prime numbers so
                                                   // that they can be tested
                                                   // in the test driver

    // CLASS METHODS
    static unsigned int hashSize(bsls_Types::Int64 hint);
        // Return the hash size based on the specified 'hint'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

          // --------------------------------------------------------
          // class bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>
          // --------------------------------------------------------

// PRIVATE CLASS METHODS
template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
const KEY& bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::keyFromBucket(
                                                             const KEY& bucket)
{
    return bucket;
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
const KEY& bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::keyFromBucket(
                                           const bsl::pair<KEY, VALUE>& bucket)
{
    return bucket.first;
}

// PRIVATE MANIPULATORS
template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
void bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::loadElementAt(
                                                Handle            *handle,
                                                bsls_Types::Int64  index,
                                                const Bucket&      element,
                                                bsls_Types::Int64  chainLength)
{
    BSLS_ASSERT(handle);

    typedef typename bsl::vector<Bucket>::size_type size_type;
    TRAITS::load(&d_buckets[(size_type)index], element);
    *handle = index;
    ++d_numElements;

    if (chainLength) {
        d_maxChain    = bsl::max(d_maxChain, chainLength);
        d_totalChain += chainLength;
        ++d_numCollisions;
    }
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
bool bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::insertElement(
                                                        Handle        *handle,
                                                        const Bucket&  element)
{
    BSLS_ASSERT(handle);

    if (size() == capacity()) {
        return false;
    }

    bool              isKeyFound;
    bsls_Types::Int64 nullIndex, chainLength, removedIndex;

    findImp(&isKeyFound, &nullIndex, &chainLength, &removedIndex,
            keyFromBucket(element));

    if (isKeyFound) {
        return false;
    }

    if (-1 != removedIndex) {
        loadElementAt(handle, removedIndex, element, chainLength);
    }
    else {
        BSLS_ASSERT_SAFE(-1 != nullIndex);

        loadElementAt(handle, nullIndex, element, chainLength);
    }

    return true;
}

// PRIVATE ACCESSORS
template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
void bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::findImp(
                                               bool              *isKeyFound,
                                               bsls_Types::Int64 *index,
                                               bsls_Types::Int64 *chainLength,
                                               bsls_Types::Int64 *removedIndex,
                                               const KEY&         key) const
{
    BSLS_ASSERT(isKeyFound);
    BSLS_ASSERT(index);
    BSLS_ASSERT(chainLength);
    BSLS_ASSERT(removedIndex);

    typedef typename bsl::vector<Bucket>::size_type size_type;

    *chainLength  = 0;
    *removedIndex = -1;

    unsigned int capacity = d_buckets.size();

    bsls_Types::Int64 bucketIndex = d_hashFunctor1.object()(key) % capacity;

    if (TRAITS::isNull(d_buckets[(size_type)bucketIndex])) {
        *isKeyFound = false;
        *index      = bucketIndex;
        return;
    }
    else if (TRAITS::isRemoved(d_buckets[(size_type)bucketIndex])) {
        *removedIndex = bucketIndex;
    }
    else if (TRAITS::areEqual(keyFromBucket(d_buckets[(size_type)bucketIndex]),
                              key)) {
        *isKeyFound = true;
        *index      = bucketIndex;
        return;
    }

    bsls_Types::Int64 increment = (d_hashFunctor2.object()(key)
                                                         % (capacity - 1)) + 1;
                                             // must be between [1, capacity-1]

    while (*chainLength < capacity) {
        ++*chainLength;
        bucketIndex = (bucketIndex + increment) % capacity;

        if (TRAITS::isNull(d_buckets[(size_type)bucketIndex])) {
            *isKeyFound = false;
            *index      = bucketIndex;
            return;
        }
        else if (TRAITS::isRemoved(d_buckets[(size_type)bucketIndex])) {
            if (*removedIndex == -1) {
                *removedIndex = bucketIndex;
            }
        }
        else
        if (TRAITS::areEqual(keyFromBucket(d_buckets[(size_type)bucketIndex]),
                             key)) {
            *isKeyFound = true;
            *index      = bucketIndex;
            return;
        }
    }

    *isKeyFound = false;
    *index      = -1;
}

// CREATORS
template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::bdecs_HashTable2(
                                             bsls_Types::Int64  capacityHint,
                                             bslma_Allocator   *basicAllocator)
: d_buckets(bdecs_HashTable2_ImpUtil::hashSize(capacityHint), Bucket(),
            basicAllocator)
, d_capacityHint(capacityHint)
, d_hashFunctor1(basicAllocator)
, d_hashFunctor2(basicAllocator)
, d_maxChain(0)
, d_numCollisions(0)
, d_numElements(0)
, d_totalChain(0)
{
    BSLS_ASSERT(capacityHint != 0);

    typedef typename bsl::vector<Bucket>::iterator Iterator;

    for (Iterator it = d_buckets.begin(); it != d_buckets.end(); ++it) {
        TRAITS::setToNull(&(*it));
    }
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::bdecs_HashTable2(
                                             bsls_Types::Int64  capacityHint,
                                             const HASH1&       hashFunctor1,
                                             const HASH2&       hashFunctor2,
                                             bslma_Allocator   *basicAllocator)
: d_buckets(bdecs_HashTable2_ImpUtil::hashSize(capacityHint), Bucket(),
            basicAllocator)
, d_capacityHint(capacityHint)
, d_hashFunctor1(hashFunctor1, basicAllocator)
, d_hashFunctor2(hashFunctor2, basicAllocator)
, d_maxChain(0)
, d_numCollisions(0)
, d_numElements(0)
, d_totalChain(0)
{
    BSLS_ASSERT(capacityHint != 0);

    typedef typename bsl::vector<Bucket>::iterator Iterator;

    for (Iterator it = d_buckets.begin(); it != d_buckets.end(); ++it) {
        TRAITS::setToNull(&(*it));
    }
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::~bdecs_HashTable2()
{
}

// MANIPULATORS
template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bool bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::insert(
                                                            Handle     *handle,
                                                            const KEY&  key)
{
    BSLS_ASSERT_SAFE(handle);

    BSLMF_ASSERT((bslmf_IsSame<bslmf_Nil, VALUE>::VALUE));

    return insertElement(handle, key);
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bool bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::insert(
                                                          Handle       *handle,
                                                          const KEY&    key,
                                                          const VALUE&  value)
{
    BSLS_ASSERT_SAFE(handle);

    BSLMF_ASSERT((!bslmf_IsSame<bslmf_Nil, VALUE>::VALUE));

    return insertElement(handle, bsl::make_pair(key, value));
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
void bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::remove(
                                                          const Handle& handle)
{
    typedef typename bsl::vector<Bucket>::size_type size_type;

    BSLS_ASSERT_SAFE(!TRAITS::isNull   (d_buckets[(size_type)handle]));
    BSLS_ASSERT_SAFE(!TRAITS::isRemoved(d_buckets[(size_type)handle]));

    TRAITS::setToRemoved(&d_buckets[(size_type)handle]);
    --d_numElements;
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
VALUE& bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::value(
                                                          const Handle& handle)
{
    typedef typename bsl::vector<Bucket>::size_type size_type;
    BSLMF_ASSERT((!bslmf_IsSame<bslmf_Nil, VALUE>::VALUE));

    BSLS_ASSERT_SAFE(!TRAITS::isNull   (d_buckets[(size_type)handle]));
    BSLS_ASSERT_SAFE(!TRAITS::isRemoved(d_buckets[(size_type)handle]));

    return d_buckets[(size_type)handle].second;
}

// ACCESSORS
template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bsls_Types::Int64
bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::capacity() const
{
    return d_buckets.size();
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bsls_Types::Int64
bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::capacityHint() const
{
    return d_capacityHint;
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bool bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::find(
                                                         Handle     *handle,
                                                         const KEY&  key) const
{
    BSLS_ASSERT_SAFE(handle);

    bool              isKeyFound;
    bsls_Types::Int64 chainLength, removedIndex;

    findImp(&isKeyFound, handle, &chainLength, &removedIndex, key);

    return isKeyFound;
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
const KEY& bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::key(
                                                    const Handle& handle) const
{
    typedef typename bsl::vector<Bucket>::size_type size_type;
    BSLS_ASSERT_SAFE(!TRAITS::isNull   (d_buckets[(size_type)handle]));
    BSLS_ASSERT_SAFE(!TRAITS::isRemoved(d_buckets[(size_type)handle]));

    return keyFromBucket(d_buckets[(size_type)handle]);
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bsls_Types::Int64
bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::maxChain() const
{
    return d_maxChain;
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bsls_Types::Int64
bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::numCollisions() const
{
    return d_numCollisions;
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bsls_Types::Int64
bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::size() const
{
    return d_numElements;
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
bsls_Types::Int64
bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::totalChain() const
{
    return d_totalChain;
}

template <typename KEY, typename VALUE, typename TRAITS,
                                        typename HASH1,
                                        typename HASH2>
inline
const VALUE& bdecs_HashTable2<KEY, VALUE, TRAITS, HASH1, HASH2>::value(
                                                    const Handle& handle) const
{
    typedef typename bsl::vector<Bucket>::size_type size_type;
    BSLMF_ASSERT((!bslmf_IsSame<bslmf_Nil, VALUE>::VALUE));

    BSLS_ASSERT_SAFE(!TRAITS::isNull   (d_buckets[(size_type)handle]));
    BSLS_ASSERT_SAFE(!TRAITS::isRemoved(d_buckets[(size_type)handle]));

    return d_buckets[(size_type)handle].second;
}

                // --------------------------------------------
                // private struct bdecs_HashTable2DefaultTraits
                // --------------------------------------------

template <typename BUCKET>
inline
void bdecs_HashTable2DefaultTraits::load(BUCKET        *dstBucket,
                                         const BUCKET&  srcBucket)
{
    BSLS_ASSERT_SAFE(dstBucket);

    *dstBucket = srcBucket;
}

template <typename KEY>
inline
bool bdecs_HashTable2DefaultTraits::areEqual(const KEY& key1, const KEY& key2)
{
    return key1 == key2;
}

inline
bool bdecs_HashTable2DefaultTraits::areEqual(const ConstCharPtr& key1,
                                             const ConstCharPtr& key2)
{
    BSLS_ASSERT_SAFE(key1);
    BSLS_ASSERT_SAFE(key2);

    return 0 == bsl::strcmp(key1, key2);
}

template <typename BUCKET>
inline
bool bdecs_HashTable2DefaultTraits::isNull(const BUCKET& bucket)
{
    enum {
        BDECS_IS_POD = bslalg_HasTrait<
                           BUCKET,
                           bslalg_TypeTraitHasTrivialDefaultConstructor>::VALUE
    };

    BSLMF_ASSERT(BDECS_IS_POD);

    const char  null  = 0;
    const char *begin = reinterpret_cast<const char *>(&bucket);
    const char *end   = begin + sizeof bucket;

    return end == bsl::find_if(begin, end,
                               bsl::bind2nd(bsl::not_equal_to<char>(), null));
}

inline
bool bdecs_HashTable2DefaultTraits::isNull(const bsl::string& bucket)
{
    return 0 == bucket.length();
}

inline
bool bdecs_HashTable2DefaultTraits::isNull(const ConstCharPtr& bucket)
{
    return 0 == bucket;
}

template <typename KEY, typename VALUE>
inline
bool bdecs_HashTable2DefaultTraits::isNull(const bsl::pair<KEY, VALUE>& bucket)
{
    return isNull(bucket.first) && isNull(bucket.second);
}

template <typename BUCKET>
inline
void bdecs_HashTable2DefaultTraits::setToNull(BUCKET *bucket)
{
    BSLS_ASSERT_SAFE(bucket);

    enum {
        BDECS_IS_POD = bslalg_HasTrait<
                           BUCKET,
                           bslalg_TypeTraitHasTrivialDefaultConstructor>::VALUE
    };

    BSLMF_ASSERT(BDECS_IS_POD);

    const char  null  = 0;
    char       *begin = reinterpret_cast<char *>(bucket);

    bsl::fill_n(begin, sizeof(BUCKET), null);
}

inline
void bdecs_HashTable2DefaultTraits::setToNull(bsl::string *bucket)
{
    BSLS_ASSERT_SAFE(bucket);

    bucket->clear();
}

inline
void bdecs_HashTable2DefaultTraits::setToNull(ConstCharPtr *bucket)
{
    BSLS_ASSERT_SAFE(bucket);

    *bucket = 0;
}

template <typename KEY, typename VALUE>
inline
void bdecs_HashTable2DefaultTraits::setToNull(bsl::pair<KEY, VALUE> *bucket)
{
    BSLS_ASSERT_SAFE(bucket);

    setToNull(&bucket->first);
    setToNull(&bucket->second);
}

template <typename BUCKET>
inline
bool bdecs_HashTable2DefaultTraits::isRemoved(const BUCKET& bucket)
{
    enum {
        BDECS_IS_POD = bslalg_HasTrait<
                           BUCKET,
                           bslalg_TypeTraitHasTrivialDefaultConstructor>::VALUE
    };

    BSLMF_ASSERT(BDECS_IS_POD);

    const char  removed = (char)0xFF;
    const char *begin   = reinterpret_cast<const char *>(&bucket);
    const char *end     = begin + sizeof bucket;

    return end == bsl::find_if(
                             begin, end,
                             bsl::bind2nd(bsl::not_equal_to<char>(), removed));
}

inline
bool bdecs_HashTable2DefaultTraits::isRemoved(const bsl::string& bucket)
{
    return 0 == bsl::strcmp(bucket.c_str(), REMOVED_KEYWORD);
}

inline
bool bdecs_HashTable2DefaultTraits::isRemoved(const ConstCharPtr& bucket)
{
#if defined(BSLS_PLATFORM__CPU_32_BIT)
    const char *removed = reinterpret_cast<const char *>(0xFFFFFFFF);
#else
    const char *removed = reinterpret_cast<const char *>(0xFFFFFFFFFFFFFFFF);
#endif

    return removed == bucket;
}

template <typename KEY, typename VALUE>
inline
bool bdecs_HashTable2DefaultTraits::isRemoved(
                                           const bsl::pair<KEY, VALUE>& bucket)
{
    return isRemoved(bucket.first) && isRemoved(bucket.second);
}

template <typename BUCKET>
inline
void bdecs_HashTable2DefaultTraits::setToRemoved(BUCKET *bucket)
{
    BSLS_ASSERT_SAFE(bucket);

    enum {
        BDECS_IS_POD = bslalg_HasTrait<
                           BUCKET,
                           bslalg_TypeTraitHasTrivialDefaultConstructor>::VALUE
    };

    BSLMF_ASSERT(BDECS_IS_POD);

    const char  removed = (char)0xFF;
    char       *begin   = reinterpret_cast<char *>(bucket);

    bsl::fill_n(begin, sizeof(BUCKET), removed);
}

inline
void bdecs_HashTable2DefaultTraits::setToRemoved(bsl::string *bucket)
{
    BSLS_ASSERT_SAFE(bucket);

    *bucket = REMOVED_KEYWORD;
}

inline
void bdecs_HashTable2DefaultTraits::setToRemoved(ConstCharPtr *bucket)
{
    BSLS_ASSERT_SAFE(bucket);

#if defined(BSLS_PLATFORM__CPU_32_BIT)
    const char *removed = reinterpret_cast<const char *>(0xFFFFFFFF);
#else
    const char *removed = reinterpret_cast<const char *>(0xFFFFFFFFFFFFFFFF);
#endif

    *bucket = removed;
}

template <typename KEY, typename VALUE>
inline
void bdecs_HashTable2DefaultTraits::setToRemoved(bsl::pair<KEY, VALUE> *bucket)
{
    BSLS_ASSERT_SAFE(bucket);

    setToRemoved(&bucket->first);
    setToRemoved(&bucket->second);
}

                    // -----------------------------------
                    // struct bdecs_HashTable2DefaultHash1
                    // -----------------------------------

template <typename KEY>
inline
unsigned int bdecs_HashTable2DefaultHash1::operator()(const KEY& key) const
{
    const char *keyData   = reinterpret_cast<const char *>(&key);
    int         keyLength = sizeof key;

    return bdeu_HashUtil::hash1(keyData, keyLength);
}

inline
unsigned int bdecs_HashTable2DefaultHash1::operator()(
                                                 const ConstCharPtr& key) const
{
    const char *keyData   = key;
    int         keyLength = bsl::strlen(key);

    return bdeu_HashUtil::hash1(keyData, keyLength);
}

inline
unsigned int bdecs_HashTable2DefaultHash1::operator()(
                                                  const bsl::string& key) const
{
    const char *keyData   = key.data();
    int         keyLength = key.length();

    return bdeu_HashUtil::hash1(keyData, keyLength);
}

                    // -----------------------------------
                    // struct bdecs_HashTable2DefaultHash2
                    // -----------------------------------

template <typename KEY>
inline
unsigned int bdecs_HashTable2DefaultHash2::operator()(const KEY& key) const
{
    const char *keyData   = reinterpret_cast<const char *>(&key);
    int         keyLength = sizeof key;

    return bdeu_HashUtil::hash2(keyData, keyLength);
}

inline
unsigned int bdecs_HashTable2DefaultHash2::operator()(
                                                 const ConstCharPtr& key) const
{
    const char *keyData   = key;
    int         keyLength = bsl::strlen(key);

    return bdeu_HashUtil::hash2(keyData, keyLength);
}

inline
unsigned int bdecs_HashTable2DefaultHash2::operator()(
                                                  const bsl::string& key) const
{
    const char *keyData   = key.data();
    int         keyLength = key.length();

    return bdeu_HashUtil::hash2(keyData, keyLength);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
