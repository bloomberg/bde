// bslh_filesystem.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLH_FILESYSTEM
#define INCLUDED_BSLH_FILESYSTEM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide 'hash' for 'std::filesystem::path'.
//
//@CLASSES:
//
//@SEE_ALSO: bslh_hash
//
//@DESCRIPTION: Provides a 'hash' specialization for 'std::filesystem::path',
// delegating to 'std::filesystem::path::hash_value' for the implementation
// since it is already available and conforming.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a Hash Cross Reference
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we already have an array of unique values of type 'TYPE', for which
// 'operator==' is defined, and we want to be able to quickly look up whether
// an element is in the array, without exhaustively applying 'operator==' to
// all the elements in sequence.  The array itself is guaranteed not to change
// for the duration of our interest in it.
//
// The problem is much simpler than building a general-purpose hash table,
// because we know how many elements our cross reference will contain in
// advance, so we will never have to dynamically grow the number of 'buckets'.
// We do not need to copy the values into our own area, so we don't have to
// create storage for them, or require that a copy constructor or destructor be
// available.  We only require that they have a transitive, symmetric
// equivalence operation 'bool operator==' and that a hash function be
// provided.
//
// We will need a hash function -- the hash function is a function that will
// take as input an object of the type stored in our array, and yield a
// 'size_t' value that will be very randomized.  Ideally, the slightest change
// in the value of the 'TYPE' object will result in a large change in the value
// returned by the hash function.  In a good hash function, typically half the
// bits of the return value will change for a 1-bit change in the hashed value.
// We then use the result of the hash function to index into our array of
// 'buckets'.  Each 'bucket' is simply a pointer to a value in our original
// array of 'TYPE' objects.  We will resolve hash collisions in our array
// through 'linear probing', where we will search consecutive buckets following
// the bucket where the collision occurred, testing occupied buckets for
// equality with the value we are searching on, and concluding that the value
// is not in the table if we encounter an empty bucket before we encounter one
// referring to an equal element.
//
// An important quality of the hash function is that if two values are
// equivalent, they must yield the same hash value.
//
// First, we define our 'HashCrossReference' template class, with the two type
// parameters 'TYPE' (the type being referenced') and 'HASHER', which defaults
// to 'bslh::Hash<TYPE>'.  This component provides the specialization of
// 'bslh::Hash' for 'std::filesystem::path':
//..
//  template <class TYPE, class HASHER = bslh::Hash<TYPE> >
//  class HashCrossReference {
//      // This table leverages a hash table to provide a fast lookup of an
//      // external, non-owned, array of values of configurable type.
//      //
//      // The only requirement for 'TYPE' is that it have a transitive,
//      // symmetric 'operator==' function.  There is no requirement that it
//      // have any kind of creator defined.
//      //
//      // The 'HASHER' template parameter type must be a functor with a
//      // function of the following signature:
//      //..
//      //  size_t operator()(const TYPE)  const; or
//      //  size_t operator()(const TYPE&) const; or
//      //..
//      // and 'HASHER' must have a publicly available default constructor and
//      // destructor.
//
//      // DATA
//      const TYPE       *d_values;             // Array of values table is to
//                                              // cross-reference.  Held, not
//                                              // owned.
//      size_t            d_numValues;          // Length of 'd_values'.
//      const TYPE      **d_bucketArray;        // Contains ptrs into
//                                              // 'd_values'
//      size_t            d_bucketArrayMask;    // Will always be '2^N - 1'.
//      HASHER            d_hasher;
//      bool              d_valid;              // Object was properly
//                                              // initialized.
//      bslma::Allocator *d_allocator_p;        // held, not owned
//
//    private:
//      // PRIVATE ACCESSORS
//      bool lookup(size_t      *idx,
//                  const TYPE&  value,
//                  size_t       hashValue) const
//          // Look up the specified 'value', having hash value 'hashValue',
//          // and return its index in 'd_bucketArray'.  If not found, return
//          // the vacant entry in 'd_bucketArray' where it should be inserted.
//          // Return 'true' if 'value is found and 'false' otherwise.
//      {
//          const TYPE *ptr;
//          for (*idx = hashValue & d_bucketArrayMask;
//                                (ptr = d_bucketArray[*idx]);
//                                     *idx = (*idx + 1) & d_bucketArrayMask) {
//              if (value == *ptr) {
//                  return true;                                      // RETURN
//              }
//          }
//          // value was not found in table
//
//          return false;
//      }
//
//    public:
//      // CREATORS
//      HashCrossReference(const TYPE       *valuesArray,
//                         size_t            numValues,
//                         bslma::Allocator *allocator = 0)
//          // Create a hash cross reference referring to the array of value.
//      : d_values(valuesArray)
//      , d_numValues(numValues)
//      , d_hasher()
//      , d_valid(true)
//      , d_allocator_p(bslma::Default::allocator(allocator))
//      {
//          size_t bucketArrayLength = 4;
//          while (bucketArrayLength < numValues * 4) {
//              bucketArrayLength *= 2;
//              BSLS_ASSERT_OPT(bucketArrayLength);
//          }
//          d_bucketArrayMask = bucketArrayLength - 1;
//          d_bucketArray = (const TYPE **) d_allocator_p->allocate(
//                                        bucketArrayLength * sizeof(TYPE **));
//          memset(d_bucketArray,  0, bucketArrayLength * sizeof(TYPE *));
//
//          for (unsigned i = 0; i < numValues; ++i) {
//              const TYPE& value = d_values[i];
//              size_t idx;
//              if (lookup(&idx, value, d_hasher(value))) {
//                  // Duplicate value.  Fail.
//
//                  printf("Error: entries %u and %u have the same value\n",
//                              i, (unsigned) (d_bucketArray[idx] - d_values));
//                  d_valid = false;
//
//                  // don't return, continue reporting other redundant
//                  // entries.
//              }
//              else {
//                  d_bucketArray[idx] = &d_values[i];
//              }
//          }
//      }
//
//      ~HashCrossReference()
//          // Free up memory used by this cross-reference.
//      {
//          d_allocator_p->deallocate(d_bucketArray);
//      }
//
//      // ACCESSORS
//      int count(const TYPE& value) const
//          // Return 1 if the specified 'value' is found in the cross
//          // reference and 0 otherwise.
//      {
//          BSLS_ASSERT_OPT(d_valid);
//
//          size_t idx;
//          return lookup(&idx, value, d_hasher(value));
//      }
//
//      bool isValid() const
//          // Return 'true' if this cross reference was successfully
//          // constructed and 'false' otherwise.
//      {
//          return d_valid;
//      }
//  };
//..
// Then, In 'main', we will first use our cross-reference to cross-reference a
// collection of 'std::filesystem::path' values.  Note that the '/' separator
// is equally valid on Windows and Unix-derived systems when used
// programmatically.  We define our array and take its length:
//..
//  const std::filesystem::path paths[] = { "/2/3",
//                                          "/4/2",
//                                          "/4/7",
//                                          "/5/6",
//                                          "/5/7",
//                                          "/6/1",
//                                          "/6/2",
//                                          "/6/3",
//                                          "/7/0",
//                                          "/7/2",
//                                          "/7/9"
//                                       };
//  enum { NUM_PATHS = sizeof paths / sizeof *paths };
//..
// Now, we create our cross-reference 'hcri' and verify it constructed
// properly.  Note that we don't specify the second template parameter 'HASHER'
// and let it default to 'bslh::Hash<std::filesystem::path>', which is already
// defined by this component:
//..
//  HashCrossReference<std::filesystem::path> hcri(paths, NUM_PATHS);
//  assert(hcri.isValid());
//..
// Finally, we use 'hcri' to verify numbers that were and were not in the
// collection:
//..
//  assert(1 == hcri.count("/2/3"));
//  assert(1 == hcri.count("/4/2"));
//  assert(1 == hcri.count("/4/7"));
//  assert(1 == hcri.count("/5/6"));
//  assert(0 == hcri.count("/a/3"));
//  assert(0 == hcri.count("/3/1"));
//  assert(0 == hcri.count("/3/7"));
//  assert(0 == hcri.count("/5/8"));
//..

#include <bslscm_version.h>

#include <bslh_hash.h>

#include <bsls_deprecatefeature.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
#include <filesystem>

#define BSLH_FILESYSTEM_DEPRECATED_CPP17                                      \
    BSLS_DEPRECATE_FEATURE("bsl",                                             \
                           "deprecated_cpp17_standard_library_features",      \
                           "do not use")
namespace BloombergLP {
namespace bslh {


                          // =========================
                          // class hash specialization
                          // =========================

template <>
struct Hash<std::filesystem::path> {
    // Specialization of 'hash' for 'std::filesystem::path' values.

    // STANDARD TYPEDEFS
    BSLH_FILESYSTEM_DEPRECATED_CPP17
    typedef std::filesystem::path argument_type;
        // !DEPRECATED!: This typedef is depreacted in C++17, for details see
        // https://isocpp.org/files/papers/p0005r4.html.

    BSLH_FILESYSTEM_DEPRECATED_CPP17
    typedef std::size_t result_type;
        // !DEPRECATED!: This typedef is depreacted in C++17, for details see
        // https://isocpp.org/files/papers/p0005r4.html.

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation has no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation has no
        // observable effect.

    // ACCESSORS
    std::size_t operator()(const std::filesystem::path &x) const;
        // Return a hash value computed using the specified 'x'.
};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
std::size_t Hash<std::filesystem::path>::operator()(
                                          const std::filesystem::path& x) const
{
    return std::filesystem::hash_value(x);
}

template <class HASHALG>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void hashAppend(HASHALG& hashAlg, const std::filesystem::path& path)
    // Pass the specified 'input' path to the specified 'hashAlg' hashing
    // algorithm of the (template parameter) type 'HASHALG'.  Note that this
    // function violates the BDE coding standard, adding a function for a
    // namespace for a different package, and none of the function parameters
    // are from this package either.  This is necessary in order to provide an
    // implementation of 'bslh::hashAppend' for the (native) standard library
    // 'std::filesystem::path' type as we are not allowed to add overloads
    // directly into namespace 'std'.  Also note that this will NOT be found by
    // the compiler if HASHALG is not in 'BloombergLP::bslh'.
{
    using BloombergLP::bslh::hashAppend;
    BloombergLP::bslh::Hash<std::filesystem::path> hashFunctor;

    hashAppend(hashAlg, hashFunctor(path));
}

}  // close namespace bslh
}  // close enterprise namespace

#undef BSLH_FILESYSTEM_DEPRECATED_CPP17

#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM

#endif // INCLUDED_BSLH_FILESYSTEM

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
