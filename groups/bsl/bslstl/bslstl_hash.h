// bslstl_hash.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASH
#define INCLUDED_BSLSTL_HASH

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for hash functions.
//
//@CLASSES:
//  bsl::hash: hash function for fundamental types
//
//@SEE_ALSO: bsl+stdhdrs
//
//@DESCRIPTION: This component provides a template unary functor,
//'bsl::hash', implementing the 'std::hash' functor.  'bsl::hash' applies a C++
//standard compliant, implementation defined, hash function to fundamental
//types returning the result of such application.
//
/// Standard Hash Function
// According to the C++ standard the requirements of a standard hash function
// 'h' are:
//
//: 1 Return a 'size_t' value between 0 and
//:   'numeric_limits<std::size_t>::max()'
//:
//: 2 The value returned must depend only on the argument 'k'.  For multiple
//:   evaluations with the same argument 'k', the value returned must be
//:   always the same.
//:
//: 3 The function should not modify it's argument.
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
// 'size_t' value which will be very randomized.  Ideally, the slightest change
// in the value of the 'TYPE' object will result in a large change in the value
// returned by the hash function.  In a good hash function, typically half the
// bits of the return value will change for a 1-bit change in the hashed value.
// We then use the result of the hash function to index into our array of
// 'buckets'.  Each 'bucket' is simply a pointer to a value in our original
// array of 'TYPE' objects.  We will resovle hash collisions in our array
// through 'linear probing', where we will search consequetive buckets
// following the bucket where the collision occurred, testing occupied buckets
// for equality with the value we are searching on, and concluding that the
// value is not in the table if we encounter an empty bucket before we
// encounter one referring to an equal element.
//
// An important quality of the hash function is that if two values are
// equivalent, they must yield the same hash value.
//
// First, we define our 'HashCrossReference' template class, with the two
// type parameters 'TYPE" (the type being referenced' and 'HASHER', which
// defaults to 'bsl::hash<TYPE>'.  For common types of 'TYPE' such as 'int',
// a specialization of 'bsl::hash' is already defined:
//..
//  template <typename TYPE, typename HASHER = bsl::hash<TYPE> >
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
//      unsigned          d_bucketArrayMask;    // Will always be '2^N - 1'.
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
// collection of integer values.  We define our array and take its length:
//..
//  const int ints[] = { 23, 42, 47, 56, 57, 61, 62, 63, 70, 72, 79 };
//  enum { NUM_INTS = sizeof ints / sizeof *ints };
//..
// Now, we create our cross-reference 'hcri' and verify it constructed
// properly.  Note that we don't specify the second template parameter 'HASHER'
// and let it default to 'bsl::hash<int>', which is already defined by
// bslstl_hash:
//..
//  HashCrossReference<int> hcri(ints, NUM_INTS);
//  assert(hcri.isValid());
//..
// Finally, we use 'hcri' to verify numbers that were and were not in the
// collection:
//..
//  assert(1 == hcri.count(23));
//  assert(1 == hcri.count(42));
//  assert(1 == hcri.count(47));
//  assert(1 == hcri.count(56));
//  assert(0 == hcri.count( 3));
//  assert(0 == hcri.count(31));
//  assert(0 == hcri.count(37));
//  assert(0 == hcri.count(58));
//..
//
///Example 2: Using Our Hash Cross Reference For a Custom Class
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In Example 1, we demonstrated a hash cross reference for integers, a trivial
// example.  In Example 2, we want to demonstrate specializing 'hash' for a
// custom class.  We will re-use the 'HashCrossReference' template class
// defined in Example 1.
//
// First, we define a 'StringThing' class, which is basically a 'const char *'
// except that 'operator==' will do the right thing on the strings and properly
// compare them:
//..
//  class StringThing {
//      // This class holds a pointer to zero-terminated string.  It is
//      // implicitly convertible to and from a 'const char *'.  The difference
//      // between this type and a 'const char *' is that 'operator==' will
//      // properly compare two objects of this type for equality of strings
//      // rather than equality of pointers.
//
//      // DATA
//      const char *d_string;    // held, not owned
//
//    public:
//      // CREATOR
//      StringThing(const char *string)                             // IMPLICIT
//      : d_string(string)
//          // Create a 'StringThing' object out of the specified 'string'.
//      {}
//
//      // ACCESSOR
//      operator const char *() const
//          // Implicitly cast this 'StringThing' object to a 'const char *'
//          // that refers to the same buffer.
//      {
//          return d_string;
//      }
//  };
//
//  inline
//  bool operator==(const StringThing& lhs, const StringThing& rhs)
//  {
//      return !strcmp(lhs, rhs);
//  }
//
//  inline
//  bool operator!=(const StringThing& lhs, const StringThing& rhs)
//  {
//      return !(lhs == rhs);
//  }
//..
// Then, we need a hash function for 'StringThing'.  We can specialize
// 'bsl::hash' for our 'StringThing' type:
//..
//  namespace bsl {
//
//  template <>
//  struct hash<StringThing> {
//      // We need to specialize 'hash' for our 'StringThing' type.  If we just
//      // called 'hash<const char *>', it would just hash the pointer, so that
//      // pointers to two different buffers containing the same sequence of
//      // chars would hash to different values, which would not be the desired
//      // behavior.
//
//      size_t operator()(const StringThing& st) const
//          // Return the hash of the zero-terminated sequence of bytes
//          // referred to by the specified 'st'.  Note that this is an ad-hoc
//          // hash function thrown together in a few minutes, it has not been
//          // exhaustively tested or mathematically analyzed.  Also note that
//          // even though most of the default specializations of 'hash' have
//          // functions that take their arguments by value, there is nothing
//          // preventing us from chosing to pass it by reference in this case.
//      {
//          enum { SHIFT_DOWN = sizeof(size_t) * 8 - 8 };
//
//  #ifdef BSLS_PLATFORM__CPU_64_BIT
//          const size_t MULTIPLIER = 0x5555555555555555ULL; // 16 '5's
//  #else
//          const size_t MULTIPLIER = 0x55555555;            //  8 '5's
//  #endif
//
//          size_t ret = 0;
//          unsigned char c;
//          for (const char *pc = st; (c = *pc); ++pc) {
//              ret =  MULTIPLIER * (ret + c);
//              ret += ret >> SHIFT_DOWN;
//          }
//
//          return ret;
//      }
//  };
//
//  }  // close namespace bsl
//..
// Next, in 'main', we want to use our cross reference on a more complex type,
// so we'll use the 'StringThing' type we created.  We create an array of
// unique 'StringThing's and take its length:
//..
//  StringThing stringThings[] = { "woof",
//                                 "meow",
//                                 "bark",
//                                 "arf",
//                                 "bite",
//                                 "chomp",
//                                 "gnaw" };
//  enum { NUM_STRINGTHINGS = sizeof stringThings / sizeof *stringThings };
//..
// Then, we create our cross-reference 'hcrsts' and verify that it constructed
// properly.  Note we don't pass a second parameter template argument and let
// 'HASHER' will define to 'bsl::hash<StringThing>', which we have defined
// above:
//..
//  HashCrossReference<StringThing> hcrsts(stringThings,
//                                         NUM_STRINGTHINGS);
//  assert(hcrsts.isValid());
//..
// Next, we verify that each element in our array registers with count:
//..
//  assert(1 == hcrsts.count("woof"));
//  assert(1 == hcrsts.count("meow"));
//  assert(1 == hcrsts.count("bark"));
//  assert(1 == hcrsts.count("arf"));
//  assert(1 == hcrsts.count("bite"));
//  assert(1 == hcrsts.count("chomp"));
//  assert(1 == hcrsts.count("gnaw"));
//..
// Now, we verify that strings not in our original array are correctly
// identified as not being in the set:
//..
//  assert(0 == hcrsts.count("buy"));
//  assert(0 == hcrsts.count("beg"));
//  assert(0 == hcrsts.count("borrow"));
//  assert(0 == hcrsts.count("or"));
//  assert(0 == hcrsts.count("steal"));
//..
// Finally, to make sure that our lookup is independent of string location, we
// copy some strings into a buffer and make sure that our results are as
// expected.
//..
//  char buffer[10];
//  strcpy(buffer, "woof");
//  assert(1 == hcrsts.count(buffer));
//  strcpy(buffer, "chomp");
//  assert(1 == hcrsts.count(buffer));
//  strcpy(buffer, "buy");
//  assert(0 == hcrsts.count(buffer));
//  strcpy(buffer, "steal");
//  assert(0 == hcrsts.count(buffer));
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_hash.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASHUTIL
#include <bslalg_hashutil.h>
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

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace bsl {

                          // ==================
                          // class bslstl::hash
                          // ==================

template <class TYPE>
struct hash;
    // Empty base class for hashing.  No general hash struct defined, each type
    // requires a specialization.  Leaving this struct declared but undefined
    // will generate error messages that are more clear when someone tries to
    // use a key that does not have a corresponding hash function.

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL 'hash'
//: o 'bsl::hash<TYPE>' is trivially default constructible.
//: o 'bsl::hash<TYPE>' is trivially copyable.
//: o 'bsl::hash<TYPE>' is bitwise movable.

template <class TYPE>
struct is_trivially_default_constructible<hash<TYPE> >
: bsl::true_type
{};

template <class TYPE>
struct is_trivially_copyable<hash<TYPE> >
: bsl::true_type
{};

}  // close namespace bsl

namespace bsl {

// ============================================================================
//                  SPECIALIZATIONS FOR FUNDAMENTAL TYPES
// ============================================================================

template <class BSLSTL_KEY>
struct hash<const BSLSTL_KEY> : hash<BSLSTL_KEY> {
    // This class provides hashing functionality for constant key types, by
    // delegating to the same function for non-constant key types.
};

template <class TYPE>
struct hash<TYPE *> {
    // Specialization of 'hash' for pointers.

    // STANDARD TYPEDEFS
    typedef TYPE *argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(TYPE *x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<bool> {
    // Specialization of 'hash' for 'bool' values.

    // STANDARD TYPEDEFS
    typedef bool argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(bool x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<char> {
    // Specialization of 'hash' for 'char' values.

    // STANDARD TYPEDEFS
    typedef char argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(char x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<signed char> {
    // Specialization of 'hash' for 'signed' 'char' values.

    // STANDARD TYPEDEFS
    typedef signed char argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(signed char x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<unsigned char> {
    // Specialization of 'hash' for 'unsigned' 'char' values.

    // STANDARD TYPEDEFS
    typedef unsigned char argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(unsigned char x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<wchar_t> {
    // Specialization of 'hash' for 'wchar_t' values.

    // STANDARD TYPEDEFS
    typedef wchar_t argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(wchar_t x) const;
        // Return a hash value computed using the specified 'x'.
};

#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
template <>
struct hash<char16_t> {
    // Specialization of 'hash' for 'char16_t' values.

    // STANDARD TYPEDEFS
    typedef char16_t argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(char16_t x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<char32_t> {
    // Specialization of 'hash' for 'char32_t' values.

    // STANDARD TYPEDEFS
    typedef char32_t argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(char32_t x) const;
        // Return a hash value computed using the specified 'x'.
};
#endif

template <>
struct hash<short> {
    // Specialization of 'hash' for 'short' values.

    // STANDARD TYPEDEFS
    typedef short argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(short x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<unsigned short> {
    // Specialization of 'hash' for 'unsigned' 'short' values.

    // STANDARD TYPEDEFS
    typedef unsigned short argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(unsigned short x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<int> {
    // Specialization of 'hash' for 'int' values.

    // STANDARD TYPEDEFS
    typedef int argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(int x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<unsigned int> {
    // Specialization of 'hash' for 'unsigned' 'int' values.

    // STANDARD TYPEDEFS
    typedef unsigned int argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(unsigned int x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<long> {
    // Specialization of 'hash' for 'long' values.

    // STANDARD TYPEDEFS
    typedef long argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(long x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<unsigned long> {
    // Specialization of 'hash' for 'unsigned' 'long' values.

    // STANDARD TYPEDEFS
    typedef unsigned long argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(unsigned long x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<long long> {
    // Specialization of 'hash' for 'long long' values.

    // STANDARD TYPEDEFS
    typedef long long argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(long long x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<unsigned long long> {
    // Specialization of 'hash' for 'unsigned' 'long long' values.

    // STANDARD TYPEDEFS
    typedef unsigned long long argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<float> {
    // Specialization of 'hash' for 'float' values.

    // STANDARD TYPEDEFS
    typedef float argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(float x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<double> {
    // Specialization of 'hash' for 'double' values.

    // STANDARD TYPEDEFS
    typedef double argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(double x) const;
        // Return a hash value computed using the specified 'x'.
};

template <>
struct hash<long double> {
    // Specialization of 'hash' for 'long double' values.

    // STANDARD TYPEDEFS
    typedef long double argument_type;
    typedef std::size_t result_type;

    //! hash() = default;
        // Create a 'hash' object.

    //! hash(const hash& original) = default;
        // Create a 'hash' object.  Note that as 'hash' is an empty (stateless)
        // type, this operation will have no observable effect.

    //! ~hash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! hash& operator=(const hash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'hash' is an empty (stateless) type, this operation will
        // have no observable effect.

    // ACCESSORS
    std::size_t operator()(long double x) const;
        // Return a hash value computed using the specified 'x'.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

template<typename TYPE>
inline
std::size_t hash<TYPE *>::operator()(TYPE *x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<bool>::operator()(bool x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<char>::operator()(char x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<signed char>::operator()(signed char x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<unsigned char>::operator()(unsigned char x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<wchar_t>::operator()(wchar_t x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

#if defined BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
inline
std::size_t hash<char>::operator()(char16_t x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<char>::operator()(char32_t x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}
#endif

inline
std::size_t hash<short>::operator()(short x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<unsigned short>::operator()(unsigned short x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<int>::operator()(int x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<unsigned int>::operator()(unsigned int x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<long>::operator()(long x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<unsigned long>::operator()(unsigned long x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<long long>::operator()(long long x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<unsigned long long>::operator()(unsigned long long x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<float>::operator()(float x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<double>::operator()(double x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash(x);
}

inline
std::size_t hash<long double>::operator()(long double x) const
{
    return ::BloombergLP::bslalg::HashUtil::computeHash((double)x);
}

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
