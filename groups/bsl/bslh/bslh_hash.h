// bslh_hash.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLH_HASH
#define INCLUDED_BSLH_HASH

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a struct to run 'bslh' hash algorithms on supported types.
//
//@CLASSES:
//  bslh::Hash: functor that runs 'bslh' hash algorithms on supported types
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a templated 'struct', 'bslh::Hash',
// that defines a hash-functor that can be used with standard containers (a
// drop in replacement for 'bsl::hash'), and which applies the supplied
// (template parameter) 'HASH_ALGORITHM' to the attributes of the (template
// parameter) 'TYPE' which have been identified as salient to hashing.  The
// 'bslh::Hash' template parameter 'HASH_ALGORITHM' must be a hashing algorithm
// that conforms the the requirements outlined below (see {Requirements for
// Regular 'bslh' Hashing Algorithms}).  Note that there are several hashing
// algorithms defined within the 'bslh' package and some, such as those that
// require seeds, will not meet these requirements, meaning they cannot be used
// with 'bslh::Hash'.  A call to 'bslh::Hash::operator()' for a (template
// parameter) 'TYPE' will call the 'hashAppend' free function for 'TYPE' and
// provide 'hashAppend' an instance of the 'HASH_ALGORITHM'.
//
// This component also contains 'hashAppend' definitions for fundamental types,
// which are required by algorithms defined in 'bslh'.  Clients are expected to
// define a free-function 'hashAppend' for each of the types they wish to be
// hashable (see {'hashAppend'} below).  More information can be found in the
// package level documentation for 'bslh' (internal users can also find
// information here {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Modularity
///----------
// 'bslh::Hash' provides a modular system for hashing.  This modularity refers
// to the decoupling of the various tasks associated with hashing.  Using this
// system, type implementers can identify attributes of their type that are
// salient to hashing, without having to write a hashing algorithm.
// Conversely, hashing algorithms can be written independent of types.
// Attributes that are salient to hashing are called out on a type using
// 'hashAppend'.  Hashing algorithms are written to operate on the attributes
// called out by 'hashAppend'.  Some default algorithms have been provided in
// the 'bslh' package.  This modularity allows type creators to avoid writing
// hashing algorithms, which can save work and avoid bad hashing algorithm
// implementations.
//
///'hashAppend'
///------------
// 'hashAppend' is the function that is used to pass attributes that are
// salient to hashing into a hashing algorithm.  A type must define a
// 'hashAppend' overload that can be discovered through ADL in order to be
// hashed using this facility.  A simple implementation of an overload for
// 'hashAppend' might call 'hashAppend' on each of the type's attributes that
// are salient to hashing.  Note that when writing a 'hashAppend' function,
// 'using bslh::hashAppend;' must be included as the first line of code in the
// function.  The using statement ensures that ADL will always be able to find
// the fundamental type 'hashAppend' functions, even when the (template
// parameter) type 'HASH_ALGORITHM' is not implemented in 'bslh'.
//
// Some types may require more subtle implementations for 'hashAppend', such as
// types containing C-strings which are salient to hashing.  These C-strings
// must be passed directly into the (template parameter) type 'HASH_ALGORITHM',
// rather than calling 'hashAppend' with the pointer as an argument.  This
// special case exists because calling 'hashAppend' with a pointer will hash
// the pointer rather than the data that is pointed to.
//
// Within this component, 'hashAppend' has been implemented for all of the
// fundamental types.  When 'hashAppend is reached on a fundamental type, the
// hashing algorithm is no longer propagated, and instead a pointer to the
// beginning of the type in memory is passed to the algorithm, along with the
// length of the type.  There are special cases with floating point numbers and
// boolean values where the data is tweaked before hashing to ensure that
// values that compare equal will be hashed with the same bit-wise
// representation.  The algorithm will then incorporate the type into its
// internal state and return a finalized hash when requested.
//
///Hashing Algorithms
///------------------
// There are algorithms implemented in the 'bslh' package that can be passed in
// and used as template parameters for 'bslh::Hash' or other 'struct's like it.
// Some of these algorithms, such as 'bslh::SpookyHashAlgorithm', are named for
// the algorithm they implement.  These named algorithms are intended for use
// by those who want a specific algorithm.  There are other algorithms, such as
// 'bslh::DefaultHashAlgorithm', which wrap an unspecified algorithm and
// describe the properties of the wrapped algorithm.  The descriptive
// algorithms are intended for use by those who need specific properties and
// want to be updated to a new algorithm when one is published with
// improvements to the desired properties.  'bslh::DefaultHashAlgorithm' has
// the property of being a good default algorithm, specifically for use in a
// hash table.
//
///Requirements for Regular 'bslh' Hashing Algorithms
///--------------------------------------------------
// Users of this modular hashing system are free to write their own hashing
// algorithms.  In order to plug into 'bslh::Hash', the user-implemented
// algorithms must implement the interface shown here:
//..
// class SomeHashAlgorithm
// {
//   public:
//     // TYPES
//     typedef Uint64 result_type;
//
//     // CREATORS
//     SomeHashAlgorithm();
//
//     // MANIPULATORS
//     void operator()(const void *data, size_t numBytes);
//
//     result_type computeHash();
// };
//..
// The 'result_type' 'typedef' must define the return type of this particular
// algorithm.  A default constructor (either implicit or explicit) must be
// supplied that creates an algorithm functor that is in a usable state.  An
// 'operator()' must be supplied that takes a 'const void *' to the data to be
// hashed and a 'size_t' length of bytes to be hashed.  This operator must
// operate on all data uniformly, meaning that regardless of whether data is
// passed in all at once, or one byte at a time, the result returned by
// 'computeHash()' will be the same.  'computeHash()' will return the final
// result of the hashing algorithm, as type 'result_type'.  'computeHash()' is
// allowed to modify the internal state of the algorithm, meaning calling
// 'computeHash()' more than once may not return the correct value.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Keying a Hash Table with a User-Defined Type
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a value-semantic type, 'Box', that contains attributes that
// are salient to hashing as well as attributes that are not salient to
// hashing.  Some of these attributes are themselves user defined types.  We
// want to store objects of type 'Box' in a hash table, so we need to be able
// to produce hash values that represent instances of 'Box'.  We don't want to
// write our own hashing or hash combine algorithm, because we know it is very
// difficult and labor-intensive to write a proper hashing algorithm.  In
// order to hash this 'Box', we will use the modular hashing system supplied in
// 'bslh'.
//
// First, we define 'Point', a class that allows us to identify a location on a
// two dimensional Cartesian plane.
//..
//  class Point {
//      // This class is a value-semantic type that represents a two
//      // dimensional location on a Cartesian plane.
//
//    private:
//      int    d_x;
//      int    d_y;
//      double d_distToOrigin; // This value will be accessed frequently, so we
//                             // cache it rather than recalculate it every
//                             // time.
//
//    public:
//      Point (int x, int y);
//          // Create a 'Point' having the specified 'x' and 'y' coordinates.
//
//      double distanceToOrigin() const;
//          // Return the distance from the origin (0, 0) to this point.
//
//      int getX() const;
//          // Return the x coordinate of this point.
//
//      int getY() const;
//          // Return the y coordinate of this point.
//  };
//
//  inline
//  Point::Point(int x, int y)
//  : d_x(x)
//  , d_y(y)
//  {
//      d_distToOrigin = sqrt(static_cast<double>(d_x * d_x) +
//                            static_cast<double>(d_y * d_y));
//  }
//
//  inline
//  double Point::distanceToOrigin() const
//  {
//      return d_distToOrigin;
//  }
//
//  inline
//  int Point::getX() const
//  {
//      return d_x;
//  }
//
//  inline
//  int Point::getY() const
//  {
//      return d_y;
//  }
//..
// Then, we define 'operator=='.  Notice how it checks only attributes that we
// would want to incorporate into the hashed value.  Note that attributes that
// are salient to hashing tend to be the same as or a subset of the attributes
// that are checked in 'operator=='.
//..
//  bool operator==(const Point &lhs, const Point &rhs)
//      // Return true if the specified 'lhs' and 'rhs' have the same value.
//      // Two 'Point' objects have the same value if they have the same x and
//      // y coordinates.
//  {
//      return (lhs.getX() == rhs.getX()) && (lhs.getY() == rhs.getY());
//  }
//..
// Next, we define 'hashAppend'.  This function will allow any hashing
// algorithm that meets the 'bslh' hashing algorithm requirements to be applied
// to 'Point'.  This is the full extent of the work that needs to be done by
// type creators.  They do not need to implement any algorithms, they just need
// to call out the attributes that are salient to hashing by calling
// 'hashAppend' on them.
//..
//  template <class HASH_ALGORITHM>
//  void hashAppend(HASH_ALGORITHM &hashAlg, const Point &point)
//      // Apply the specified 'hashAlg' to the specified 'point'
//  {
//      using bslh::hashAppend;
//      hashAppend(hashAlg, point.getX());
//      hashAppend(hashAlg, point.getY());
//  }
//..
// Then, we declare another value-semantic type, 'Box' that will have a 'Point'
// as one of its attributes that are salient to hashing.
//..
//  class Box {
//      // This class is a value-semantic type that represents a box drawn on
//      // to a Cartesian plane.
//
//    private:
//      Point d_position;
//      int d_length;
//      int d_width;
//
//    public:
//      Box(Point position, int length, int width);
//          // Create a box having the specified 'length' and 'width', with its
//          // upper left corner at the specified 'position'
//
//      int getLength() const;
//          // Return the length of this box.
//
//      Point getPosition() const;
//          // Return a 'Point' representing the upper left corner of this box
//          // on a Cartesian plane
//
//      int getWidth() const;
//          // Return the width of this box.
//  };
//
//  inline
//  Box::Box(Point position, int length, int width)
//  : d_position(position)
//  , d_length(length)
//  , d_width(width) { }
//
//  int Box::getLength() const
//  {
//      return d_length;
//  }
//
//  Point Box::getPosition() const
//  {
//      return d_position;
//  }
//
//  int Box::getWidth() const
//  {
//      return d_width;
//  }
//..
// Then, we define 'operator=='.  This time all of the data members are salient
// to equality.
//..
//  bool operator==(const Box &lhs, const Box &rhs)
//      // Return true if the specified 'lhs' and 'rhs' have the same value.
//      // Two 'Box' objects have the same value if they have the same length,
//      // width, and position.
//  {
//      return (lhs.getPosition() == rhs.getPosition()) &&
//             (lhs.getLength()   == rhs.getLength()) &&
//             (lhs.getWidth()    == rhs.getWidth());
//  }
//..
// Next, we define 'hashAppend' for 'Box'.  Notice how as well as calling
// 'hashAppend' on fundamental types, we can also call it with our user defined
// type 'Point'.  Calling 'hashAppend' with 'Point' will propogate a reference
// to the hashing algorithm functor 'hashAlg' down to the fundamental types
// that make up 'Point', and those types will then be passed into the
// referenced algorithm functor.
//..
//  template <class HASH_ALGORITHM>
//  void hashAppend(HASH_ALGORITHM &hashAlg, const Box &box)
//      // Apply the specified 'hashAlg' to the specified 'box'
//  {
//      using bslh::hashAppend;
//      hashAppend(hashAlg, box.getPosition());
//      hashAppend(hashAlg, box.getLength());
//      hashAppend(hashAlg, box.getWidth());
//  }
//..
// Then, we declare our hash table (implementation elided).  We simplify the
// problem by requiring the caller to supply an array.  Our hash table takes
// two type parameters: 'TYPE' (the type being referenced) and 'HASHER' (a
// functor that produces the hash).  'HASHER' will default to 'bslh::Hash<>'.
//..
//  template <class TYPE, class HASHER = bslh::Hash<> >
//  class HashTable {
//      // This class template implements a hash table providing fast lookup of
//      // an external, non-owned, array of values of (template parameter)
//      // 'TYPE'.
//      //
//      // The (template parameter) 'TYPE' shall have a transitive, symmetric
//      // 'operator==' function and it will be hashable using 'bslh::Hash'.
//      // Note that there is no requirement that it have any kind of creator
//      // defined.
//      //
//      // The 'HASHER' template parameter type must be a functor with a method
//      // having the following signature:
//      //..
//      //  size_t operator()(TYPE)  const;
//      //                   -OR-
//      //  size_t operator()(const TYPE&) const;
//      //..
//      // and 'HASHER' shall have a publicly accessible default constructor
//      // and destructor.  Here we use 'bslh::Hash' as our default template
//      // argument.  This allows us to hash any type for which 'hashAppend'
//      // has been implemented.
//      //
//      // Note that this hash table has numerous simplifications because we
//      // know the size of the array and never have to resize the table.
//
//      // DATA
//      const TYPE       *d_values;             // Array of values table is to
//                                              // hold
//      size_t            d_numValues;          // Length of 'd_values'.
//      const TYPE      **d_bucketArray;        // Contains ptrs into
//                                              // 'd_values'
//      unsigned          d_bucketArrayMask;    // Will always be '2^N - 1'.
//      HASHER            d_hasher;
//
//    private:
//      // PRIVATE ACCESSORS
//      bool lookup(size_t      *idx,
//                  const TYPE&  value,
//                  size_t       hashValue) const;
//          // Look up the specified 'value', having the specified 'hashValue',
//          // and load its index in 'd_bucketArray' into the specified 'idx'.
//          // If not found, return the vacant entry in 'd_bucketArray' where
//          // it should be inserted.  Return 'true' if 'value' is found and
//          // 'false' otherwise.
//
//    public:
//      // CREATORS
//      HashTable(const TYPE *valuesArray,
//                size_t      numValues);
//          // Create a hash table referring to the specified 'valuesArray'
//          // having length of the specified 'numValues'.  No value in
//          // 'valuesArray' shall have the same value as any of the other
//          // values in 'valuesArray'
//
//      ~HashTable();
//          // Free up memory used by this hash table.
//
//      // ACCESSORS
//      bool contains(const TYPE& value) const;
//          // Return true if the specified 'value' is found in the table and
//          // false otherwise.
//  };
//..
// Next, we will create an array of boxes that we want to store in our hash
// table.
//..
//
//        Box boxes[] = { Box(Point(1, 1), 3, 2),
//                        Box(Point(3, 1), 4, 2),
//                        Box(Point(1, 2), 3, 3),
//                        Box(Point(1, 1), 2, 2),
//                        Box(Point(1, 4), 4, 3),
//                        Box(Point(2, 1), 4, 2),
//                        Box(Point(1, 0), 3, 1)};
//        enum { NUM_BOXES = sizeof boxes / sizeof *boxes };
//
//..
// Then, we create our hash table 'hashTable'.  We pass we use the default
// functor which will pick up the 'hashAppend' function we created:
//..
//
//        HashTable<Box> hashTable(boxes, NUM_BOXES);
//..
// Now, we verify that each element in our array registers with count:
//..
// for ( int i = 0; i < 6; ++i) { ASSERT(hashTable.contains(boxes[i])); }
//..
// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:
//..
// ASSERT(!hashTable.contains(Box(Point(1, 1), 1, 1)));
// ASSERT(!hashTable.contains(Box(Point(0, 0), 0, 0)));
// ASSERT(!hashTable.contains(Box(Point(3, 3), 3, 3)));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_DEFAULTHASHALGORITHM
#include <bslh_defaulthashalgorithm.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISFLOATINGPOINT
#include <bslmf_isfloatingpoint.h>
#endif

#ifndef INCLUDED_BSLMF_ISINTEGRAL
#include <bslmf_isintegral.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#include <bslmf_istriviallydefaultconstructible.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>  // for 'size_t'
#define INCLUDED_STDDEF_H
#endif

namespace BloombergLP {

namespace bslh {

                          // ================
                          // class bslh::Hash
                          // ================

template <class HASH_ALGORITHM = bslh::DefaultHashAlgorithm>
struct Hash {
    // This struct wraps the (template parameter) type 'HASH_ALGORITHM' in an
    // interface that satisfies the 'hash' requirements of the C++11 standard.

    // TYPES
    typedef size_t result_type;
        // The type of the hash value that will be returned by the
        // function-call operator.

    // CREATORS
    //! Hash() = default;
        // Create a 'bslh::Hash' object.

    //! Hash(const Hash& original) = default;
        // Create a 'bslh::Hash' object.  Note that as 'bslh::Hash' is an empty
        // (stateless) type, this operation will have no observable effect.

    //! ~Hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! Hash& operator=(const Hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'bslh::Hash' is an empty (stateless) type, this operation
        // will have no observable effect.

    // ACCESSORS
    template <class TYPE>
    result_type operator()(const TYPE& type) const;
        // Returns a hash value generated by the (template parameter) type
        // 'HASH_ALGORITHM' for the specified 'type'.  The value returned by
        // the 'HASH_ALGORITHM' is cast to 'size_t' before returning.

};

// FREE FUNCTIONS
template <class HASH_ALGORITHM, class TYPE>
inline
typename bsl::enable_if<
    (bsl::is_integral<TYPE>::value ||
     bsl::is_pointer<TYPE>::value  ||
     bsl::is_enum<TYPE>::value)    &&
    !bsl::is_same<TYPE, bool>::value
>::type
hashAppend(HASH_ALGORITHM& hashAlg, TYPE input)
    // Passes the specified 'input' into the specified 'hashAlg' to be combined
    // into the internal state of the algorithm which is used to produce the
    // resulting hash value. Note that the 'enable_if' meta-function is used to
    // enable this 'hashAppend' function for only integral (excluding 'bool'),
    // pointer, and enum types, because these types can all be hashed as a
    // continuous sequence of bytes. Also note that this function is defined
    // inline because MS Visual Studio compilers before 2013 require (some)
    // functions declared using enable_if be in-place inline.
{
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM, class TYPE>
inline
typename bsl::enable_if<
    bsl::is_floating_point<TYPE>::value &&
   !bsl::is_same<TYPE, long double>::value
>::type
hashAppend(HASH_ALGORITHM& hashAlg, TYPE input)
    // Passes the specified 'input' into the specified 'hashAlg' to be combined
    // into the internal state of the algorithm which is used to produce the
    // resulting hash value. Note that the 'enable_if' meta-function is used to
    // enable this 'hashAppend' function for only floating point (excluding
    // 'long double') types, because these types need to have +/-0.0 normalized
    // to 0.0 before they can be hashed as a continuous sequence of bytes. Also
    // note that this function is defined inline because MS Visual Studio
    // compilers before 2013 require (some) functions declared using enable_if
    // be in-place inline.
{
    if (input == 0)
        input = 0;
    hashAlg(&input, sizeof(input));
}

template <class HASH_ALGORITHM, class TYPE>
typename bsl::enable_if< bsl::is_same<TYPE, bool>::value >::type
hashAppend(HASH_ALGORITHM& hashAlg, TYPE input);
    // Passes the specified 'input' into the specified 'hashAlg' to be combined
    // into the internal state of the algorithm which is used to produce the
    // resulting hash value. Note that the 'enable_if' meta-function is used to
    // enable this 'hashAppend' function for only 'bool', because 'bool's can
    // have multiple 'true' representations and need to be normalized before
    // they can be hashed as a continuous sequence of bytes.

template <class HASH_ALGORITHM, class TYPE>
typename bsl::enable_if< bsl::is_same<TYPE, long double>::value >::type
hashAppend(HASH_ALGORITHM& hashAlg, TYPE input);
    // Passes the specified 'input' into the specified 'hashAlg' to be combined
    // into the internal state of the algorithm which is used to produce the
    // resulting hash value. Note that the 'enable_if' meta-function is used to
    // enable this 'hashAppend' function for only 'long double', because on
    // some compilers 'long double's contain garbage and can not be hashed as
    // a continuous sequence of bytes.

template <class HASH_ALGORITHM, size_t N>
void hashAppend(HASH_ALGORITHM& hashAlg, char (&input)[N]);
    // Passes the specified 'input' into the specified 'hashAlg' to be combined
    // into the internal state of the algorithm which is used to produce the
    // resulting hash value. Note that the entire 'char' array will be hashed
    // in only one call to 'hashAlg'. Also note that this 'hashAppend' exists
    // because some platforms don't recognize that adding a const qualifier is
    // a better match for arrays than decaying to a pointer and using the
    // 'hashAppend' function for pointers.

template <class HASH_ALGORITHM, size_t N>
void hashAppend(HASH_ALGORITHM& hashAlg, const char (&input)[N]);
    // Passes the specified 'input' into the specified 'hashAlg' to be combined
    // into the internal state of the algorithm which is used to produce the
    // resulting hash value. Note that the entire 'char' array will be hashed
    // in only one call to 'hashAlg'.

template <class HASH_ALGORITHM, class TYPE, size_t N>
void hashAppend(HASH_ALGORITHM& hashAlg, TYPE (&input)[N]);
    // Passes the specified 'input' into the specified 'hashAlg' to be combined
    // into the internal state of the algorithm which is used to produce the
    // resulting hash value. Note that the elements in 'input' will be hashed
    // one at a time by calling 'hashAppend' because the (template parameter)
    // 'TYPE' might not be hashable as a contiguous sequence of bytes. Also
    // note that this 'hashAppend' exists because some platforms don't
    // recognize that adding a const qualifier is a better match for arrays
    // than decaying to a pointer and using the 'hashAppend' function for
    // pointers.

template <class HASH_ALGORITHM, class TYPE, size_t N>
void hashAppend(HASH_ALGORITHM& hashAlg, const TYPE (&input)[N]);
    // Passes the specified 'input' into the specified 'hashAlg' to be combined
    // into the internal state of the algorithm which is used to produce the
    // resulting hash value. Note that the elements in 'input' will be hashed
    // one at a time by calling 'hashAppend' because the (template parameter)
    // 'TYPE' might not be hashable as a contiguous sequence of bytes.

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// ACCESSORS
template <class HASH_ALGORITHM>
template <class TYPE>
inline
typename bslh::Hash<HASH_ALGORITHM>::result_type
bslh::Hash<HASH_ALGORITHM>::operator()(TYPE const& key) const
{
    HASH_ALGORITHM hashAlg;
    hashAppend(hashAlg, key);
    return static_cast<result_type>(hashAlg.computeHash());
}

// FREE FUNCTIONS
template <class HASH_ALGORITHM, class TYPE>
inline
typename bsl::enable_if< bsl::is_same<TYPE, bool>::value >::type
bslh::hashAppend(HASH_ALGORITHM& hashAlg, TYPE input)
{
    // We need to ensure that any inputs that compare equal produce the same
    // hash.  Any non-zero binary representation of 'input' can be 'true', so
    // we need to normalize 'input' to ensure that we do not pass two different
    // binary representations of 'true' true into our hashing algorithm.
    unsigned char normalizedData = input;

    hashAlg(&normalizedData, sizeof(normalizedData));
}

template <class HASH_ALGORITHM, class TYPE>
inline
typename bsl::enable_if< bsl::is_same<TYPE, long double>::value >::type
bslh::hashAppend(HASH_ALGORITHM& hashAlg, TYPE input)
{
    if (input == 0.0l){
        input = 0;
    }

#if (defined BSLS_PLATFORM_CPU_X86 || defined BSLS_PLATFORM_CPU_X86_64) &&    \
    (defined BSLS_PLATFORM_CMP_GNU || defined BSLS_PLATFORM_CMP_CLANG)
    // This needs to be done to work around issues when compiling with GCC and
    // Clang on 86 machines.  On 64-bit hardware, 'sizeof(long double)' is
    // advertised as 16 bytes, but only 10 bytes of precision is used.  The
    // remaining 6 bytes are padding.
    //
    // For Clang, the final 2 bytes of the padding are zeroed, but the 4 bytes
    // that proceed the final two appear to be garbage.
    //
    //..
    //      Actual Data --+*****************************+
    //                    |                             |
    // Actual long double: 5d e9 79 a9 c2 82 bb ef 2b 40 87 d8 5c 2b  0  0
    //                                                   |          ||   |
    //      Garbage -------------------------------------+**********+|   |
    //      Zeroed --------------------------------------------------+***+
    //..
    //
    // For GCC, the first and last 2 bytes of the padding are zeroed, but the 2
    // bytes in the middle appear to be garbage.
    //
    //..
    //      Garbage -------------------------------------------+****+
    //      Actual Data --+*****************************+     |     |
    //                    |                             |     |     |
    // Actual long double: 5d e9 79 a9 c2 82 bb ef 2b 40  0  0 5c 2b  0  0
    //                                                   |    |      |    |
    //      Zeroed --------------------------------------+****+------+****+
    //..
    //
    // On 32-bit hardware, 'sizeof(long double)' is advertised as 12 bytes, but
    // again, only 10 bytes of precision is used.  The remaining 2 bytes are
    // padding.
    //
    // For Clang, the 2 bytes of the padding appear to be garbage.
    //
    //..
    //      Actual Data --+*****************************+
    //                    |                             |
    // Actual long double: 5d e9 79 a9 c2 82 bb ef 2b 40 87 d8
    //                                                   |    |
    //      Garbage -------------------------------------+****+
    //..
    //
    // For GCC, the 2 bytes of the padding are zeroed.
    //
    //..
    //      Actual Data --+*****************************+
    //                    |                             |
    // Actual long double: 5d e9 79 a9 c2 82 bb ef 2b 40  0  0
    //                                                   |    |
    //      Zeroed --------------------------------------+****+
    //..
    //
    // To address all of these issues, we will pass in only 10 bytes for a
    // 'long double' even if it is longer.
  #if !defined(BSLS_PLATFORM_CMP_CLANG) && BSLS_PLATFORM_CPU_X86_64
    // We cant just check 'defined(BSLS_PLATFORM_CMP_GNU)' because Clang
    // masquerades as GCC.  Since we know that to be in this block we must be
    // using GCC or Clang, we can just check
    // '!defined(BSLS_PLATFORM_CMP_CLANG)' to get the same result.

    if (bsl::is_same<long double, __float128>::value) {
        // We need to handle the posibility that somebody has set the GCC
        // compiler flag that makes 'long double' actually be 128-bit.
        hashAlg(&input, sizeof(input));
        return;                                                       // RETURN
    }
  #endif
    hashAlg(&input, sizeof(input) > 10 ? 10 : sizeof(input));
#else
    hashAlg(&input, sizeof(input));
#endif
}

template <class HASH_ALGORITHM, size_t N>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, char (&input)[N])
{
    hashAlg(&input, sizeof(char)*N);
}

template <class HASH_ALGORITHM, size_t N>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, const char (&input)[N])
{
    hashAlg(&input, sizeof(char)*N);
}

template <class HASH_ALGORITHM, class TYPE, size_t N>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, TYPE (&input)[N])
{

    for (size_t i = 0; i < N; ++i) {
        hashAppend(hashAlg, input[i]);
    }
}


template <class HASH_ALGORITHM, class TYPE, size_t N>
inline
void bslh::hashAppend(HASH_ALGORITHM& hashAlg, const TYPE (&input)[N])
{
    for (size_t i = 0; i < N; ++i) {
        hashAppend(hashAlg, input[i]);
    }
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL 'hash'
//: o 'bsl::hash<TYPE>' is trivially default constructible.
//: o 'bsl::hash<TYPE>' is trivially copyable.
//: o 'bsl::hash<TYPE>' is bitwise movable.

namespace bslmf {
template <class TYPE>
struct IsBitwiseMoveable<bslh::Hash<TYPE> >
    : bsl::true_type {};
}  // close namespace bslmf


}  // close enterprise namespace

namespace bsl {
template <class TYPE>
struct is_trivially_default_constructible< ::BloombergLP::bslh::Hash<TYPE> >
: bsl::true_type
{};

template <class TYPE>
struct is_trivially_copyable< ::BloombergLP::bslh::Hash<TYPE> >
: bsl::true_type
{};
}  // close namespace bsl



#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
