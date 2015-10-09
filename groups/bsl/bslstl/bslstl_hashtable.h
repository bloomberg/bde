// bslstl_hashtable.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASHTABLE
#define INCLUDED_BSLSTL_HASHTABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash-container with support for duplicate values.
//
//@CLASSES:
//   bslstl::HashTable : hashed-table container for user-supplied object types
//
//@SEE_ALSO: bsl+stdhdrs
//
//@DESCRIPTION: This component defines a single class template, 'HashTable',
// implementing a value-semantic container that can be used to easily implement
// the four 'unordered' containers specified by the C++11 standard.
//
// An instantiation of 'HashTable' is an allocator-aware, value-semantic type
// whose salient attributes are its size (number of keys) and the ordered
// sequence of keys the 'HashTable' contains.  If 'HashTable' is instantiated
// with a key type that is not itself value-semantic, then it will not retain
// all of its value-semantic qualities.  In particular, if the key type cannot
// be tested for equality, then a HashTable containing that type cannot be
// tested for equality.  It is even possible to instantiate 'HashTable' with a
// key type that does not have a copy-constructor, in which case the
// 'HashTable' will not be copyable.
//
///Requirements on 'KEY_CONFIG'
///----------------------------
// The elements stored in a 'HashTable' and the key by which they are indexed
// are defined by a 'KEY_CONFIG' template type parameter.  The user-supplied
// 'KEY_CONFIG' type must provide two type aliases named 'ValueType' and
// 'KeyType' that name the type of element stored and its associated key type
// respectively.  In addition, a 'KEY_CONFIG' class shall provide a static
// member function which may be called as if it had the following signature:
//..
//  static const KeyType& extractKey(const ValueType& value);
//      // Return a reference offering non-modifiable access to the key for the
//      // specified 'value'.
//..
// Optionally, the 'KEY_CONFIG' class might provide an 'extractKey' function
// with the alternative signature:
//..
//  static KeyType& extractKey(ValueType& value);
//      // Return a reference to the key for the specified 'value'.
//..
// This alternative signature is necessary to support the rare case that a hash
// function or comparator used to configure the 'HashTable' template below take
// their arguments by non-const reference.  This is subject to additional
// constraints that these functions may not modify the passed arguments, and is
// inherently a fragile interface and not recommended.  It is supported only
// for C++ Standard conformance.
//
// A 'HashTable' is a "Value-Semantic Type" (see {'bsldoc_glossary'}) only if
// the configured 'ValueType' is value-semantic.  It is possible to instantiate
// a 'HashTable' configured with a 'ValueType' that does not provide a full
// 'HashTable' of value-semantic operations, but then some methods of the
// container may not be instantiable.  The following terminology, adopted from
// the C++11 standard, is used in the function documentation of 'HashTable' to
// describe a function's requirements for the 'KEY' template parameter.  These
// terms are also defined in [utility.arg.requirements] (section 17.6.3.1 of
// the C++11 standard).  Note that, in the context of a 'HashTable'
// instantiation, the requirements apply specifically to the 'HashTable's
// element type, 'ValueType'.
//
//: "default-constructible": The type provides a default constructor.
//:
//: "copy-constructible": The type provides a copy constructor.
//:
//: "equality-comparable": The type provides an equality-comparison operator
//:     that defines an equivalence relationship and is both reflexive and
//:     transitive.
//
///Memory Allocation
///-----------------
// The type supplied as a HashTable's 'ALLOCATOR' template parameter determines
// how that HashTable will allocate memory.  The 'HashTable' template supports
// allocators meeting the requirements of the C++ standard allocator
// requirements ([allocator.requirements], C++11 17.6.3.5); in addition it
// supports scoped-allocators derived from the 'bslma::Allocator' memory
// allocation protocol.  Clients intending to use 'bslma' style allocators
// should use the template's default 'ALLOCATOR' type: The default type for the
// 'ALLOCATOR' template parameter, 'bsl::allocator', provides a C++11
// standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the parameterized 'ALLOCATOR' type of an 'HashTable' instantiation is
// 'bsl::allocator', then objects of that HashTable type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a HashTable
// accepts an optional 'bslma::Allocator' argument at construction.  If the
// address of a 'bslma::Allocator' object is explicitly supplied at
// construction, it will be used to supply memory for the HashTable throughout
// its lifetime; otherwise, the HashTable will use the default allocator
// installed at the time of the HashTable's construction (see 'bslma_default').
// In addition to directly allocating memory from the indicated
// 'bslma::Allocator', a HashTable supplies that allocator's address to the
// constructors of contained objects of the configured 'ValueType' with the
// 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
///Exception Safety
///----------------
// The operations of a 'HashTable' provide the strong exception guarantee (see
// {'bsldoc_glossary'}) except in the presence of a hash-functor or
// equality-comparator that throws exceptions.  If either the hash-functor or
// equality-comparator throws an exception from a non-const method, 'HashTable'
// provides only the basic exception guarantee, and the operation will leave
// the container in a valid but unspecified (potentially empty) state.
//
///Internal Data Structure
///-----------------------
// This implementation of a hash-table uses a single bidirectional list, to
// hold all the elements stored in the container, and the elements in this list
// are indexed by a dynamic array of buckets, each of which holds a pointer to
// the first and last element in the linked-list whose adjusted hash-values are
// equal to that bucket's index.
//
// As we do not cache the hashed value, if any hash function throws we will
// either do nothing and allow the exception to propagate, or, if some change
// of state has already been made, clear the whole container to provide the
// basic exception guarantee.  There are similar concerns for the 'COMPARATOR'
// predicate.
//
///Usage
///-----
// This section illustrates intended use of this component.  The
// 'bslstl::HashTable' class template provides a common foundation for
// implementing the four standard unordered containers:
//: o 'bsl::unordered_map'
//: o 'bsl::unordered_multiset'
//: o 'bsl::unordered_multimap'
//: o 'bsl::unordered_set'
// This and the subsequent examples in this component use the
// 'bslstl::HashTable' class to implement several model container classes, each
// providing a small but representative sub-set of the functionality of one of
// the standard unordered containers.
//
///Example 1: Implementing a Hashed Set Container
///----------------------------------------------
// Suppose we wish to implement, 'MyHashedSet', a greatly abbreviated version
// of 'bsl::unordered_set'.  The 'bslstl::HashTable' class template can be used
// as the basis of that implementation.
//
// First, we define 'UseEntireValueAsKey', a class template we can use to
// configure 'bslstl::HashTable' to use its entire elements as keys for its
// hasher, a policy suitable for a set container.  (Later, in {Example 2}, we
// will define 'UseFirstValueOfPairAsKey' for use in a map container.  Note
// that, in practice, developers can use the existing classes in
// {'bslstl_unorderedmapkeyconfiguration'} and
// {'bslstl_unorderedsetkeyconfiguration'}.)
//..
//                          // ==========================
//                          // struct UseEntireValueAsKey
//                          // ==========================
//
//  template <class VALUE_TYPE>
//  struct UseEntireValueAsKey {
//      // This 'struct' provides a namespace for types and methods that define
//      // the policy by which the key value of a hashed container (i.e., the
//      // value passed to the hasher) is extracted from the objects stored in
//      // the hashed container (the 'value' type).
//
//      typedef VALUE_TYPE ValueType;
//          // Alias for 'VALUE_TYPE', the type stored in the hashed container.
//
//      typedef ValueType KeyType;
//          // Alias for the type passed to the hasher by the hashed container.
//          // In this policy, that type is 'ValueType'.
//
//      static const KeyType& extractKey(const ValueType& value);
//          // Return the key value for the specified 'value'.  In this policy,
//          // that is 'value' itself.
//  };
//
//                          // --------------------------
//                          // struct UseEntireValueAsKey
//                          // --------------------------
//
//  template <class VALUE_TYPE>
//  inline
//  const typename UseEntireValueAsKey<VALUE_TYPE>::KeyType&
//                 UseEntireValueAsKey<VALUE_TYPE>::extractKey(
//                                                      const ValueType& value)
//  {
//      return value;
//  }
//..
// Next, we define 'MyPair', a class template that can hold a pair of values of
// arbitrary types.  This will be used to in 'MyHashedSet' to return the status
// of the 'insert' method, which must provide an iterator to the inserted value
// and a boolean value indicating if the value is newly inserted if it
// previously exiting in the set.  The 'MyPair' class template will also appear
// in {Example 2} and {Example 3}.  Note that in practice, users can use the
// standard 'bsl::pair' in this role; the 'MyPair class template is used in
// these examples to avoid creating a dependency of 'bslstl_hashtable' on
// 'bslstl_pair'.
//..
//                      // =============
//                      // struct MyPair
//                      // =============
//
//  template <class FIRST_TYPE, class SECOND_TYPE>
//  struct MyPair {
//      // PUBLIC TYPES
//      typedef  FIRST_TYPE  first_type;
//      typedef SECOND_TYPE second_type;
//
//      // DATA
//      first_type  first;
//      second_type second;
//
//      // CREATORS
//      MyPair();
//          // Create a 'MyPair' object with a default constructed 'first'
//          // member and a default constructed 'second' member.
//
//      MyPair(first_type firstValue, second_type secondValue);
//          // Create a 'MyPair' object with a 'first' member equal to the
//          // specified 'firstValue' and the 'second' member equal to the
//          // specified 'secondValue'.
//  };
//
//  // FREE OPERATORS
//  template <class FIRST_TYPE, class SECOND_TYPE>
//  inline
//  bool operator==(const MyPair<FIRST_TYPE, SECOND_TYPE>& lhs,
//                  const MyPair<FIRST_TYPE, SECOND_TYPE>& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' MyPair objects have
//      // the same value, and 'false' otherwise.  'lhs' has the same value as
//      // 'rhs' if 'lhs.first == rhs.first' and 'lhs.second == rhs.second'.
//
//  template <class FIRST_TYPE, class SECOND_TYPE>
//  inline
//  bool operator!=(const MyPair<FIRST_TYPE, SECOND_TYPE>& lhs,
//                  const MyPair<FIRST_TYPE, SECOND_TYPE>& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' MyPair objects do not
//      // have the same value, and 'false' otherwise.  'lhs' does not have the
//      // same value as 'rhs' if 'lhs.first != rhs.first' or
//      // 'lhs.second != rhs.second'.
//
//                      // -------------
//                      // struct MyPair
//                      // -------------
//
//  // CREATORS
//  template <class FIRST_TYPE, class SECOND_TYPE>
//  inline
//  MyPair<FIRST_TYPE,SECOND_TYPE>::MyPair()
//  : first()
//  , second()
//  {
//  }
//
//  template <class FIRST_TYPE, class SECOND_TYPE>
//  inline
//  MyPair<FIRST_TYPE,SECOND_TYPE>::MyPair( first_type firstValue,
//                                         second_type secondValue)
//  : first(firstValue)
//  , second(secondValue)
//  {
//  }
//
//  // FREE OPERATORS
//  template <class FIRST_TYPE, class SECOND_TYPE>
//  inline
//  bool operator==(const MyPair<FIRST_TYPE, SECOND_TYPE>& lhs,
//                  const MyPair<FIRST_TYPE, SECOND_TYPE>& rhs)
//  {
//      return lhs.first == rhs.first && lhs.second == rhs.second;
//  }
//
//  template <class FIRST_TYPE, class SECOND_TYPE>
//  inline
//  bool operator!=(const MyPair<FIRST_TYPE, SECOND_TYPE>& lhs,
//                  const MyPair<FIRST_TYPE, SECOND_TYPE>& rhs)
//  {
//      return lhs.first != rhs.first || lhs.second != rhs.second;
//  }
//..
// Then, we define our 'MyHashedSet' class template with an instance of
// 'bslstl::HashTable' (configured using 'UseEntireValueAsKey') as its sole
// data member.  We provide 'insert' method, to allow us to populate these
// sets, and the 'find' method to allow us to examine those elements.  We also
// provide 'size' and 'bucket_count' accessor methods to let us check the inner
// workings of our class.
//
// Note that the standard classes define aliases for the templated parameters
// and other types.  In the interest of brevity, this model class (and the
// classes in the subsequent examples) do not define such aliases except where
// strictly needed for the example.
//..
//                          // =================
//                          // class MyHashedSet
//                          // =================
//
//  template <class KEY,
//            class HASH      = bsl::hash<KEY>,
//            class EQUAL     = bsl::equal_to<KEY>,
//            class ALLOCATOR = bsl::allocator<KEY> >
//  class MyHashedSet
//  {
//    private:
//      // PRIVATE TYPES
//      typedef bsl::allocator_traits<ALLOCATOR>          AllocatorTraits;
//      typedef typename AllocatorTraits::difference_type difference_type;
//      typedef BloombergLP::bslstl::HashTableIterator<const KEY,
//                                                     difference_type>
//                                                        iterator;
//
//      // DATA
//      BloombergLP::bslstl::HashTable<UseEntireValueAsKey<KEY>,
//                                     HASH,
//                                     EQUAL,
//                                     ALLOCATOR> d_impl;
//    public:
//      // TYPES
//      typedef typename AllocatorTraits::size_type size_type;
//      typedef iterator                            const_iterator;
//
//      // CREATORS
//      explicit MyHashedSet(size_type        initialNumBuckets = 0,
//                           const HASH&      hash              = HASH(),
//                           const EQUAL&     keyEqual          = EQUAL(),
//                           const ALLOCATOR& allocator         = ALLOCATOR());
//          // Create an empty 'MyHashedSet' object having a maximum load
//          // factor of 1.  Optionally specify at least 'initialNumBuckets' in
//          // this container's initial array of buckets.  If
//          // 'initialNumBuckets' is not supplied, an implementation defined
//          // value is used.  Optionally specify a 'hash' used to generate the
//          // hash values associated to the keys extracted from the values
//          // contained in this object.  If 'hash' is not supplied, a
//          // default-constructed object of type 'HASH' is used.  Optionally
//          // specify a key-equality functor 'keyEqual' used to verify that
//          // two key values are the same.  If 'keyEqual' is not supplied, a
//          // default-constructed object of type 'EQUAL' is used.  Optionally
//          // specify an 'allocator' used to supply memory.  If 'allocator' is
//          // not supplied, a default-constructed object of the (template
//          // parameter) type 'ALLOCATOR' is used.  If the 'ALLOCATOR' is
//          // 'bsl::allocator' (the default), then 'allocator' shall be
//          // convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' is
//          // 'bsl::allocator' and 'allocator' is not supplied, the currently
//          // installed default allocator is used to supply memory.
//
//      //! ~MyHashedSet() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      MyPair<const_iterator, bool> insert(const KEY& value);
//          // Insert the specified 'value' into this set if the 'value' does
//          // not already exist in this set; otherwise, this method has no
//          // effect.  Return a pair whose 'first' member is an iterator
//          // providing non-modifiable access to the (possibly newly inserted)
//          // 'KEY' object having 'value' (according to 'EQUAL') and whose
//          // 'second' member is 'true' if a new element was inserted, and
//          // 'false' if 'value' was already present.
//
//      // ACCESSORS
//      size_type bucket_count() const;
//          // Return the number of buckets in this set.
//
//      const_iterator cend() const;
//          // Return an iterator providing non-modifiable access to the
//          // past-the-end element (in the sequence of 'KEY' objects)
//          // maintained by this set.
//
//      const_iterator find(const KEY& value) const;
//          // Return an iterator providing non-modifiable access to the 'KEY'
//          // object in this set having the specified 'value', if such an
//          // entry exists, and the iterator returned by the 'cend' method
//          // otherwise.
//
//      size_type size() const;
//          // Return the number of elements in this set.
//  };
//..
// Next, we implement the methods of 'MyHashedSet'.  In many cases, the
// implementations consist mainly in forwarding arguments to and returning
// values from the underlying 'bslstl::HashTable'.
//..
//                          // =================
//                          // class MyHashedSet
//                          // =================
//
//  // CREATORS
//  template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
//  inline
//  MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::MyHashedSet(
//                                          size_type        initialNumBuckets,
//                                          const HASH&      hash,
//                                          const EQUAL&     keyEqual,
//                                          const ALLOCATOR& allocator)
//  : d_impl(hash, keyEqual, initialNumBuckets, allocator)
//  {
//  }
//..
// Note that the 'insertIfMissing' method of 'bslstl::HashTable' provides the
// semantics needed for adding values (unique values only) to sets.
//..
//  // MANIPULATORS
//  template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
//  inline
//  MyPair<typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::iterator,
//         bool>    MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::insert(
//                                                            const KEY& value)
//  {
//      typedef MyPair<iterator, bool> ResultType;
//
//      bool                       isInsertedFlag = false;
//      bslalg::BidirectionalLink *result         = d_impl.insertIfMissing(
//                                                             &isInsertedFlag,
//                                                             value);
//      return ResultType(iterator(result), isInsertedFlag);
//  }
//
//  // ACCESSORS
//  template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
//  inline
//  typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::size_type
//           MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::bucket_count() const
//  {
//      return d_impl.numBuckets();
//  }
//
//  template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
//  inline
//  typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
//           MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::cend() const
//  {
//      return const_iterator();
//  }
//
//  template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
//  inline
//  typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
//           MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::find(const KEY& key)
//                                                                        const
//  {
//      return const_iterator(d_impl.find(key));
//  }
//
//  template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
//  inline
//  typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::size_type
//           MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::size() const
//  {
//      return d_impl.size();
//  }
//..
// Finally, we create 'mhs', an instance of 'MyHashedSet', exercise it, and
// confirm that it behaves as expected.
//..
//  MyHashedSet<int> mhs;
//  assert( 0    == mhs.size());
//  assert( 1    == mhs.bucket_count());
//..
// Notice that the newly created set is empty and has a single bucket.
//
// Inserting a value (10) succeeds the first time but correctly fails on the
// second attempt.
//..
//  MyPair<MyHashedSet<int>::const_iterator, bool> status;
//
//  status = mhs.insert(10);
//  assert( 1    ==  mhs.size());
//  assert(10    == *status.first)
//  assert(true  ==  status.second);
//
//  status = mhs.insert(10);
//  assert( 1    ==  mhs.size());
//  assert(10    == *status.first)
//  assert(false ==  status.second);
//..
// We can insert a different value (20) and thereby increase the set size to 2.
//..
//  status = mhs.insert(20);
//  assert( 2    ==  mhs.size());
//  assert(20    == *status.first)
//  assert(true  ==  status.second);
//..
// Each of the inserted values (10, 20) can be found in the set.
//..
//  MyHashedSet<int>::const_iterator itr, end = mhs.cend();
//
//  itr = mhs.find(10);
//  assert(end !=  itr);
//  assert(10  == *itr);
//
//  itr = mhs.find(20);
//  assert(end !=  itr);
//  assert(20  == *itr);
//..
// However, a value known to absent from the set (0), is correctly reported as
// not there.
//..
//  itr = mhs.find(0);
//  assert(end ==  itr);
//..
//
///Example 2: Implementing a Hashed Map Container
///----------------------------------------------
// Suppose we wish to implement, 'MyHashedMap', a greatly abbreviated version
// of 'bsl::unordered_map'.  As with 'MyHashedSet' (see {Example 1}), the
// 'bslstl::HashTable' class template can be used as the basis of our
// implementation.
//
// First, we define 'UseFirstValueOfPairAsKey', a class template we can use to
// configure 'bslstl::HashTable' to use the 'first' member of each element,
// each a 'MyPair', as the key-value for hashing.  Note that, in practice,
// developers can use class defined in {'bslstl_unorderedmapkeyconfiguration'}.
//..
//                          // ===============================
//                          // struct UseFirstValueOfPairAsKey
//                          // ===============================
//
//  template <class VALUE_TYPE>
//  struct UseFirstValueOfPairAsKey {
//      // This 'struct' provides a namespace for types and methods that define
//      // the policy by which the key value of a hashed container (i.e., the
//      // value passed to the hasher) is extracted from the objects stored in
//      // the hashed container (the 'value' type).
//
//      typedef VALUE_TYPE ValueType;
//          // Alias for 'VALUE_TYPE', the type stored in the hashed container.
//          // For this policy 'ValueType' must define a public member named
//          // 'first' of type 'first_type'.
//
//      typedef typename ValueType::first_type KeyType;
//          // Alias for the type passed to the hasher by the hashed container.
//          // In this policy, that type is the type of the 'first' element of
//          // 'ValueType'.
//
//      static const KeyType& extractKey(const ValueType& value);
//          // Return the key value for the specified 'value'.  In this policy,
//          // that is the value of the 'first' member of 'value'.
//  };
//
//                          // -------------------------------
//                          // struct UseFirstValueOfPairAsKey
//                          // -------------------------------
//
//  template <class VALUE_TYPE>
//  inline
//  const typename UseFirstValueOfPairAsKey<VALUE_TYPE>::KeyType&
//                 UseFirstValueOfPairAsKey<VALUE_TYPE>::extractKey(
//                                                      const ValueType& value)
//  {
//      return value.first;
//  }
//..
// Next, we define our 'MyHashedMap' class template with an instance of
// 'bslstl::HashTable' (configured using 'UseFirstValueOfPairAsKey') as its
// sole data member.  In this example, we choose to implement 'operator[]'
// (corresponding to the signature method of 'bsl::unordered_map') to allow us
// to populate our maps and to examine their elements.
//..
//                          // =================
//                          // class MyHashedMap
//                          // =================
//
//  template <class KEY,
//            class VALUE,
//            class HASH      = bsl::hash<KEY>,
//            class EQUAL     = bsl::equal_to<KEY>,
//            class ALLOCATOR = bsl::allocator<KEY> >
//  class MyHashedMap
//  {
//    private:
//      // PRIVATE TYPES
//      typedef bsl::allocator_traits<ALLOCATOR>          AllocatorTraits;
//
//      typedef BloombergLP::bslstl::HashTable<
//                      UseFirstValueOfPairAsKey<MyPair<const KEY, VALUE> >,
//                      HASH,
//                      EQUAL,
//                      ALLOCATOR>                     HashTable;
//
//      // DATA
//      HashTable d_impl;
//
//    public:
//      // TYPES
//      typedef typename AllocatorTraits::size_type size_type;
//
//      // CREATORS
//      explicit MyHashedMap(size_type        initialNumBuckets = 0,
//                           const HASH&      hash              = HASH(),
//                           const EQUAL&     keyEqual          = EQUAL(),
//                           const ALLOCATOR& allocator         = ALLOCATOR());
//      // Create an empty 'MyHashedMap' object having a maximum load factor
//      // of 1.  Optionally specify at least 'initialNumBuckets' in this
//      // container's initial array of buckets.  If 'initialNumBuckets' is not
//      // supplied, one empty bucket shall be used and no memory allocated.
//      // Optionally specify 'hash' to generate the hash values associated
//      // with the key-value pairs contained in this unordered map.  If 'hash'
//      // is not supplied, a default-constructed object of (template
//      // parameter) 'HASH' is used.  Optionally specify a key-equality
//      // functor 'keyEqual' used to determine whether two keys have the same
//      // value.  If 'keyEqual' is not supplied, a default-constructed object
//      // of (template parameter) 'EQUAL' is used.  Optionally specify an
//      // 'allocator' used to supply memory.  If 'allocator' is not supplied,
//      // a default-constructed object of the (template parameter) type
//      // 'ALLOCATOR' is used.  If 'ALLOCATOR' is 'bsl::allocator' (the
//      // default), then 'allocator' shall be convertible to
//      // 'bslma::Allocator *'.  If 'ALLOCATOR' is 'bsl::allocator' and
//      // 'allocator' is not supplied, the currently installed default
//      // allocator is used to supply memory.  Note that more than
//      // 'initialNumBuckets' buckets may be created in order to preserve the
//      // bucket allocation strategy of the hash-table (but never fewer).
//
//      //! ~MyHashedMap() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      VALUE& operator[](const KEY& key);
//          // Return a reference providing modifiable access to the
//          // mapped-value associated with the specified 'key' in this
//          // unordered map; if this unordered map does not already contain a
//          // 'value_type' object with 'key', first insert a new 'value_type'
//          // object having 'key' and a default-constructed 'VALUE' object.
//          // Note that this method requires that the (template parameter)
//          // type 'KEY' is "copy-constructible" and the (template parameter)
//          // 'VALUE' is "default-constructible".
//  };
//..
// Then, we implement the methods 'MyHashedMap'.  The construct need merely
// forward its arguments to the constructor of 'd_impl',
//..
//                          // =================
//                          // class MyHashedMap
//                          // =================
//
//  // CREATORS
//  template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
//  inline
//  MyHashedMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::MyHashedMap(
//                                          size_type        initialNumBuckets,
//                                          const HASH&      hash,
//                                          const EQUAL&     keyEqual,
//                                          const ALLOCATOR& allocator)
//  : d_impl(hash, keyEqual, initialNumBuckets, allocator)
//  {
//  }
//..
// As with 'MyHashedSet', the 'insertIfMissing' method of 'bslstl::HashTable'
// provides the semantics we need: an element is inserted only if no such
// element (no element with the same key) in the container, and a reference to
// that element ('node') is returned.  Here, we use 'node' to obtain and return
// a reference offering modifiable access to the 'second' member of the
// (possibly newly added) element.  Note that the 'static_cast' from
// 'HashTableLink *' to 'HashTableNode *' is valid because the nodes derive
// from the link type (see 'bslalg_bidirectionallink' and
// 'bslalg_hashtableimputil').
//..
//  // MANIPULATORS
//  template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
//  inline
//  VALUE& MyHashedMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator[](
//                                                              const KEY& key)
//  {
//      typedef typename HashTable::NodeType           HashTableNode;
//      typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;
//
//      HashTableLink *node = d_impl.insertIfMissing(key);
//      return static_cast<HashTableNode *>(node)->value().second;
//  }
//..
// Finally, we create 'mhm', an instance of 'MyHashedMap', exercise it, and
// confirm that it behaves as expected.  We can add an element (with key value
// of 0).
//..
//  MyHashedMap<int, double> mhm;
//
//  mhm[0] = 1.234;
//  assert(1.234 == mhm[0]);
//..
// We can change the value of the element with key value 0.
//..
//  mhm[0] = 4.321;
//  assert(4.321 == mhm[0]);
//..
// We can add a new element (key value 1), without changing the previously
// existing element (key value 0).
//..
//  mhm[1] = 5.768;
//  assert(5.768 == mhm[1]);
//  assert(4.321 == mhm[0]);
//..
// Accessing a non-existing element (key value 2) creates that element and
// populates it with the default value of the mapped value (0.0).
//..
//  assert(0.000 == mhm[2]);
//..
//
///Example 3: Implementing a Hashed Multi-Map Container
///----------------------------------------------------
// Suppose we wish to implement, 'MyHashedMultiMap', a greatly abbreviated
// version of 'bsl::unordered_multimap'.  As with 'MyHashedSet' and
// 'MyHashedMap' (see {Example 1}, and {Example 2}, respectively), the
// 'bslstl::HashTable' class template can be used as the basis of our
// implementation.
//
// First, we need a class template to configure 'bslstl::HashTable' to extract
// key values in manner appropriate for maps.  The previously defined
// 'UseFirstValueOfPairAsKey' class template (see {Example 2}) suits perfectly.
//
// Next, we define our 'MyHashedMultiMap' class template with an instance of
// 'bslstl::HashTable' (configured using 'UseFirstValueOfPairAsKey') as its
// sole data member.  In this example, we choose to implement an 'insert'
// method to populate our container, and an 'equal_range' method (a signature
// method of the multi containers) to provide access to those elements.
//..
//                          // ======================
//                          // class MyHashedMultiMap
//                          // ======================
//
//  template <class KEY,
//            class VALUE,
//            class HASH      = bsl::hash<KEY>,
//            class EQUAL     = bsl::equal_to<KEY>,
//            class ALLOCATOR = bsl::allocator<KEY> >
//  class MyHashedMultiMap
//  {
//    private:
//      // PRIVATE TYPES
//      typedef MyPair<const KEY, VALUE>                  value_type;
//      typedef bsl::allocator_traits<ALLOCATOR>          AllocatorTraits;
//      typedef typename AllocatorTraits::difference_type difference_type;
//
//      typedef BloombergLP::bslstl::HashTable<
//                         UseFirstValueOfPairAsKey<MyPair<const KEY, VALUE> >,
//                         HASH,
//                         EQUAL,
//                         ALLOCATOR>                     HashTable;
//
//      // DATA
//      HashTable d_impl;
//
//    public:
//      // TYPES
//      typedef typename AllocatorTraits::size_type  size_type;
//
//      typedef BloombergLP::bslstl::HashTableIterator<value_type,
//                                                     difference_type>
//                                                                    iterator;
//      typedef BloombergLP::bslstl::HashTableIterator<const value_type,
//                                                     difference_type>
//                                                              const_iterator;
//
//      // CREATORS
//      explicit MyHashedMultiMap(
//                           size_type        initialNumBuckets = 0,
//                           const HASH&      hash              = HASH(),
//                           const EQUAL&     keyEqual          = EQUAL(),
//                           const ALLOCATOR& allocator         = ALLOCATOR());
//      // Create an empty 'MyHashedMultiMap' object having a maximum load
//      // factor of 1.  Optionally specify at least 'initialNumBuckets' in
//      // this container's initial array of buckets.  If 'initialNumBuckets'
//      // is not supplied, an implementation defined value is used.
//      // Optionally specify a 'hash', a hash-functor used to generate the
//      // hash values associated to the key-value pairs contained in this
//      // object.  If 'hash' is not supplied, a default-constructed object of
//      // (template parameter) 'HASH' type is used.  Optionally specify a
//      // key-equality functor 'keyEqual' used to verify that two key values
//      // are the same.  If 'keyEqual' is not supplied, a default-constructed
//      // object of (template parameter) 'EQUAL' type is used.  Optionally
//      // specify an 'allocator' used to supply memory.  If 'allocator' is not
//      // supplied, a default-constructed object of the (template parameter)
//      // 'ALLOCATOR' type is used.  If 'ALLOCATOR' is 'bsl::allocator' (the
//      // default), then 'allocator' shall be convertible to
//      // 'bslma::Allocator *'.  If the 'ALLOCATOR' is 'bsl::allocator' and
//      // 'allocator' is not supplied, the currently installed default
//      // allocator is used to supply memory.
//
//      //! ~MyHashedMultiMap() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      template <class SOURCE_TYPE>
//      iterator insert(const SOURCE_TYPE& value);
//          // Insert the specified 'value' into this multi-map, and return an
//          // iterator to the newly inserted element.  Note that this method
//          // requires that the (class template parameter) types 'KEY' and
//          // 'VALUE' types both be "copy-constructible", and that the
//          // (function template parameter) 'SOURCE_TYPE' be convertible to
//          // the (class template parameter) 'VALUE' type.
//
//      // ACCESSORS
//      MyPair<const_iterator, const_iterator> equal_range(const KEY& key)
//                                                                       const;
//          // Return a pair of iterators providing non-modifiable access to
//          // the sequence of 'value_type' objects in this container matching
//          // the specified 'key', where the the first iterator is positioned
//          // at the start of the sequence and the second iterator is
//          // positioned one past the end of the sequence.  If this container
//          // contains no 'value_type' objects matching 'key', then the two
//          // returned iterators will have the same value.
//  };
//..
// Then, we implement the methods 'MyHashedMultiMap'.  The construct need
// merely forward its arguments to the constructor of 'd_impl',
//..
//                          // ======================
//                          // class MyHashedMultiMap
//                          // ======================
//
//  // CREATORS
//  template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
//  inline
//  MyHashedMultiMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::MyHashedMultiMap(
//                                         size_type        initialNumBuckets,
//                                         const HASH&      hash,
//                                         const EQUAL&     keyEqual,
//                                         const ALLOCATOR& allocator)
//  : d_impl(hash, keyEqual, initialNumBuckets, allocator)
//  {
//  }
//..
// Note that here we forgo use of the 'insertIfMissing' method and use the
// 'insert' method of 'bslstl::HashTable'.  This method supports the semantics
// of the multi containers: there can be more than one element with the same
// key value.
//..
//  // MANIPULATORS
//  template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
//  template <class SOURCE_TYPE>
//  inline
//  typename MyHashedMultiMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
//           MyHashedMultiMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
//                                                    const SOURCE_TYPE& value)
//  {
//      return iterator(d_impl.insert(value));
//  }
//..
// The 'equal_range' method need only convert the values returned by the
// 'findRange' method to the types expected by the caller.
//..
//  // ACCESSORS
//  template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
//  MyPair<typename MyHashedMultiMap<KEY,
//                                   VALUE,
//                                   HASH,
//                                   EQUAL,
//                                   ALLOCATOR>::const_iterator,
//         typename MyHashedMultiMap<KEY,
//                                   VALUE,
//                                   HASH,
//                                   EQUAL, ALLOCATOR>::const_iterator>
//  MyHashedMultiMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
//                                                        const KEY& key) const
//  {
//      typedef MyPair<const_iterator, const_iterator> ResultType;
//      typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;
//
//      HashTableLink *first;
//      HashTableLink *last;
//      d_impl.findRange(&first, &last, key);
//      return ResultType(const_iterator(first), const_iterator(last));
//  }
//..
// Finally, we create 'mhmm', an instance of 'MyHashedMultiMap', exercise it,
// and confirm that it behaves as expected.
//
// We define several aliases to make our code more concise.
//..
//  typedef MyHashedMultiMap<int, double>::iterator       Iterator;
//  typedef MyHashedMultiMap<int, double>::const_iterator ConstIterator;
//  typedef MyPair<ConstIterator, ConstIterator>          ConstRange;
//..
// Searching for an element (key value 10) in a newly created, empty container
// correctly shows the absence of any such element.
//..
//  MyHashedMultiMap<int, double> mhmm;
//
//  ConstRange range;
//  range = mhmm.equal_range(10);
//  assert(range.first == range.second);
//..
// We can insert a value (the pair 10, 100.00) into the container...
//..
//  MyPair<const int, double> value(10, 100.00);
//
//  Iterator itr;
//
//  itr = mhmm.insert(value);
//  assert(value == *itr);
//..
// ... and we can do so again.
//..
//  itr = mhmm.insert(value);
//  assert(value == *itr);
//..
// We can now find elements with the key value of 10.
//..
//  range = mhmm.equal_range(10);
//  assert(range.first != range.second);
//..
// As expected, there are two such elements, and both are identical in key
// value (10) and mapped value (100.00).
//..
//  int count = 0;
//  for (ConstIterator cur  = range.first,
//                     end  = range.second;
//                     end != cur; ++cur, ++count) {
//      assert(value == *cur);
//  }
//  assert(2 == count);
//..
//  }
//
///Example 4: Implementing a Custom Container
///------------------------------------------
// Although the 'bslstl::HashTable' class was created to be a common
// implementation for the standard unordered classes, this class can also be
// used in its own right to address other user problems.
//
// Suppose that we wish to retain a record of sales orders, that each record is
// characterized by several integer attributes, and that we must be able to
// find records based on *any* of those attributes.  We can use
// 'bslstl::HashTable' to implement a custom container supporting multiple
// key-values.
//
// First, we define 'MySalesRecord', our record class:
//..
//  enum { MAX_DESCRIPTION_SIZE = 16 };
//
//  typedef struct MySalesRecord {
//      int  orderNumber;                        // unique
//      int  customerId;                         // no constraint
//      int  vendorId;                           // no constraint
//      char description[MAX_DESCRIPTION_SIZE];  // ascii string
//  } MySalesRecord;
//..
// Notice that only each 'orderNumber' is unique.  We expect multiple sales to
// any given customer ('customerId') and multiple sales by any given vendor
// ('vendorId').
//
// We will use a 'bslstl::HashTable' object (a hashtable) to save record values
// based on the unique 'orderNumber', and two auxiliary hashtables to provide
// map 'customerId' and 'vendorId' values to the addresses of the records in
// the first 'bslstl::HashTable' object.  Note that this implementation relies
// on the fact that nodes in our hashtables remain stable until they are
// removed and that in this application we do *not* allow the removal (or
// modification) of records once they are inserted.
//
// To configure these hashtables, we will need several policy objects to
// extract relevant portions the 'MySalesRecord' objects for hashing.
//
// Next, define 'UseOrderNumberAsKey', a policy class for the hashtable holding
// the sales record objects.  Note that the 'ValueType' is 'MySalesRecord' and
// that the 'extractKey' method selects the 'orderNumber' attribute:
//..
//                          // ==========================
//                          // struct UseOrderNumberAsKey
//                          // ==========================
//
//  struct UseOrderNumberAsKey {
//      // This 'struct' provides a namespace for types and methods that define
//      // the policy by which the key value of a hashed container (i.e., the
//      // value passed to the hasher) is extracted from the objects stored in
//      // the hashed container (the 'value' type).
//
//      typedef MySalesRecord ValueType;
//          // Alias for 'MySalesRecord', the type stored in the first
//          // hashtable.
//
//      typedef int KeyType;
//          // Alias for the type passed to the hasher by the hashed container.
//          // In this policy, the value passed to the hasher is the
//          // 'orderNumber' attribute, an 'int' type.
//
//      static const KeyType& extractKey(const ValueType& value);
//          // Return the key value for the specified 'value'.  In this policy,
//          // that is the 'orderNumber' attribute of 'value'.
//  };
//
//                          // --------------------------
//                          // struct UseOrderNumberAsKey
//                          // --------------------------
//
//  inline
//  const UseOrderNumberAsKey::KeyType&
//        UseOrderNumberAsKey::extractKey(const ValueType& value)
//  {
//      return value.orderNumber;
//  }
//..
// Then, we define 'UseCustomerIdAsKey', the policy class for the hashtable
// that will multiply map 'customerId' to the addresses of records in the first
// hashtable.  Note that in this policy class the 'ValueType' is
// 'const MySalesRecord *'.
//..
//                          // =========================
//                          // struct UseCustomerIdAsKey
//                          // =========================
//
//  struct UseCustomerIdAsKey {
//      // This 'struct' provides a namespace for types and methods that define
//      // the policy by which the key value of a hashed container (i.e., the
//      // value passed to the hasher) is extracted from the objects stored in
//      // the hashed container (the 'value' type).
//
//      typedef const MySalesRecord *ValueType;
//          // Alias for 'const MySalesRecord *', the type stored in second
//          // hashtable, a pointer to the record stored in the first
//          // hashtable.
//
//      typedef int KeyType;
//          // Alias for the type passed to the hasher by the hashed container.
//          // In this policy, the value passed to the hasher is the
//          // 'orderNumber' attribute, an 'int' type.
//
//      static const KeyType& extractKey(const ValueType& value);
//          // Return the key value for the specified 'value'.  In this policy,
//          // that is the 'customerId' attribute of 'value'.
//  };
//
//                          // -------------------------
//                          // struct UseCustomerIdAsKey
//                          // -------------------------
//
//  inline
//  const UseCustomerIdAsKey::KeyType&
//        UseCustomerIdAsKey::extractKey(const ValueType& value)
//  {
//      return value->customerId;
//  }
//..
// Notice that, since the values in the second hashtable are addresses, the
// key-value is extracted by reference.  This second hashtable allows what
// map-like semantics, *without* having to store key-values; those reside in
// the records in the first hashtable.
//
// The 'UseVendorIdAsKey' class, the policy class for the hashtable providing
// an index by 'vendorId', is almost a near clone of 'UseCustomerIdAsKey'.  It
// is shown for completeness:
//..
//                          // =======================
//                          // struct UseVendorIdAsKey
//                          // ========================
//
//  struct UseVendorIdAsKey {
//      // This 'struct' provides a namespace for types and methods that define
//      // the policy by which the key value of a hashed container (i.e., the
//      // value passed to the hasher) is extracted from the objects stored in
//      // the hashed container (the 'value' type).
//
//      typedef const MySalesRecord *ValueType;
//          // Alias for 'const MySalesRecord *', the type stored in second
//          // hashtable, a pointer to the record stored in the first
//          // hashtable.
//
//      typedef int KeyType;
//          // Alias for the type passed to the hasher by the hashed container.
//          // In this policy, the value passed to the hasher is the
//          // 'vendorId' attribute, an 'int' type.
//
//      static const KeyType& extractKey(const ValueType& value);
//          // Return the key value for the specified 'value'.  In this policy,
//          // that is the 'vendorId' attribute of 'value'.
//  };
//
//                          // -----------------------
//                          // struct UseVendorIdAsKey
//                          // -----------------------
//
//  inline
//  const UseVendorIdAsKey::KeyType&
//        UseVendorIdAsKey::extractKey(const ValueType& value)
//  {
//      return value->vendorId;
//  }
//..
// Next, we define 'MySalesRecordContainer', our customized container:
//..
//                          // ----------------------------
//                          // class MySalesRecordContainer
//                          // ----------------------------
//
//  class MySalesRecordContainer
//  {
//    private:
//      // PRIVATE TYPES
//      typedef BloombergLP::bslstl::HashTable<
//                    UseOrderNumberAsKey,
//                    bsl::hash<    UseOrderNumberAsKey::KeyType>,
//                    bsl::equal_to<UseOrderNumberAsKey::KeyType> >
//                                                        RecordsByOrderNumber;
//      typedef bsl::allocator_traits<
//            bsl::allocator<UseOrderNumberAsKey::ValueType> > AllocatorTraits;
//      typedef AllocatorTraits::difference_type               difference_type;
//..
// The 'ItrByOrderNumber' type is used to provide access to the elements of the
// first hash table, the one that stores the records.
//..
//
//      typedef BloombergLP::bslstl::HashTableIterator<const MySalesRecord,
//                                                     difference_type>
//                                                            ItrByOrderNumber;
//..
// The 'ItrPtrById' type is used to provide access to the elements of the other
// hashtables, the ones that store pointers into the first hashtable.
//..
//      typedef BloombergLP::bslstl::HashTableIterator<const MySalesRecord *,
//                                                     difference_type>
//                                                                  ItrPtrById;
//..
// If we were to provide iterators of type 'ItrPtrById' to our users,
// dereferencing the iterator would provide a 'MySalesRecord' pointer, which
// would then have to be dereferences.  Instead, we use 'ItrPtrById' to define
// 'ItrById' in which accessors have been overridden to provide that extra
// dereference implicitly.
//..
//      class ItrById : public ItrPtrById
//      {
//        public:
//          // CREATORS
//          explicit ItrById(bslalg::BidirectionalLink *node)
//          : ItrPtrById(node)
//          {
//          }
//
//          // ACCESSORS
//          const MySalesRecord& operator*() const
//          {
//              return *ItrPtrById::operator*();
//          }
//
//          const MySalesRecord *operator->() const
//          {
//              return &(*ItrPtrById::operator*());
//          }
//      };
//
//      typedef BloombergLP::bslstl::HashTable<
//                    UseCustomerIdAsKey,
//                    bsl::hash<    UseCustomerIdAsKey::KeyType>,
//                    bsl::equal_to<UseCustomerIdAsKey::KeyType> >
//                                                     RecordsPtrsByCustomerId;
//      typedef BloombergLP::bslstl::HashTable<
//                    UseVendorIdAsKey,
//                    bsl::hash<    UseVendorIdAsKey::KeyType>,
//                    bsl::equal_to<UseVendorIdAsKey::KeyType> >
//                                                       RecordsPtrsByVendorId;
//      // DATA
//      RecordsByOrderNumber    d_recordsByOrderNumber;
//      RecordsPtrsByCustomerId d_recordptrsByCustomerId;
//      RecordsPtrsByVendorId   d_recordptrsByVendorId;
//
//    public:
//      // PUBLIC TYPES
//      typedef ItrByOrderNumber  ConstItrByOrderNumber;
//      typedef ItrById           ConstItrById;
//
//      // CREATORS
//      explicit MySalesRecordContainer(bslma::Allocator *basicAllocator = 0);
//          // Create an empty 'MySalesRecordContainer' object.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//
//      //! ~MySalesRecordContainer() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      MyPair<ConstItrByOrderNumber, bool> insert(const MySalesRecord& value);
//          // Insert the specified 'value' into this set if the 'value' does
//          // not already exist in this set; otherwise, this method has no
//          // effect.  Return a pair whose 'first' member is an iterator
//          // providing non-modifiable access to the (possibly newly inserted)
//          // 'MySalesRecord' object having 'value' and whose 'second' member
//          // is 'true' if a new element was inserted, and 'false' if 'value'
//          // was already present.
//
//      // ACCESSORS
//      ConstItrByOrderNumber cend() const;
//          // Return an iterator providing non-modifiable access to the
//          // past-the-end element (in the sequence of 'MySalesRecord'
//          // objects) maintained by this set.
//
//      ConstItrByOrderNumber findByOrderNumber(int value) const;
//          // Return an iterator providing non-modifiable access to the
//          // 'MySalesRecord' object in this set having the specified 'value',
//          // if such an entry exists, and the iterator returned by the 'cend'
//          // method otherwise.
//..
// Notice that this interface provides map-like semantics for finding records.
// We need only specify the 'orderNumber' attribute of the record of interest;
// however, the return value is set-like: we get access to the record, not the
// more complicated key-value/record pair that a map would have provided.
//
// Internally, the hash table need only store the records themselves.  A map
// would have had to manage key-value/record pairs, where the key-value would
// be a copy of part of the record.
//..
//      MyPair<ConstItrById, ConstItrById> findByCustomerId(int value) const;
//          // Return a pair of iterators providing non-modifiable access to
//          // the sequence of 'MySalesRecord' objects in this container having
//          // a 'customerId' attribute equal to the specified 'value' where
//          // the first iterator is positioned at the start of the sequence
//          // and the second iterator is positioned one past the end of the
//          // sequence.  If this container has no such objects, then the two
//          // iterators will be equal.
//
//      MyPair<ConstItrById, ConstItrById> findByVendorId(int value) const;
//          // Return a pair of iterators providing non-modifiable access to
//          // the sequence of 'MySalesRecord' objects in this container having
//          // a 'vendorId' attribute equal to the specified 'value' where the
//          // first iterator is positioned at the start of the sequence and
//          // the second iterator is positioned one past the end of the
//          // sequence.  If this container has no such objects, then the two
//          // iterators will be equal.
//  };
//..
// Then, we implement the methods of 'MySalesRecordContainer', our customized
// container:
//..
//                          // ----------------------------
//                          // class MySalesRecordContainer
//                          // ----------------------------
//
//  // CREATORS
//  inline
//  MySalesRecordContainer::MySalesRecordContainer(
//                                            bslma::Allocator *basicAllocator)
//  : d_recordsByOrderNumber(basicAllocator)
//  , d_recordptrsByCustomerId(basicAllocator)
//  , d_recordptrsByVendorId(basicAllocator)
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  MyPair<MySalesRecordContainer::ConstItrByOrderNumber, bool>
//  MySalesRecordContainer::insert(const MySalesRecord& value)
//  {
//      // Insert into internal container that will own the record.
//
//      bool                                    isInsertedFlag = false;
//      BloombergLP::bslalg::BidirectionalLink *result         =
//              d_recordsByOrderNumber.insertIfMissing(&isInsertedFlag, value);
//
//      // Index by other record attributes
//
//      RecordsByOrderNumber::NodeType *nodePtr =
//                       static_cast<RecordsByOrderNumber::NodeType *>(result);
//
//      d_recordptrsByCustomerId.insert(&nodePtr->value());
//        d_recordptrsByVendorId.insert(&nodePtr->value());
//
//      // Return of insertion.
//
//      return MyPair<ConstItrByOrderNumber, bool>(
//                                               ConstItrByOrderNumber(result),
//                                               isInsertedFlag);
//  }
//
//  // ACCESSORS
//  inline
//  MySalesRecordContainer::ConstItrByOrderNumber
//  MySalesRecordContainer::cend() const
//  {
//      return ConstItrByOrderNumber();
//  }
//
//  inline
//  MySalesRecordContainer::ConstItrByOrderNumber
//  MySalesRecordContainer::findByOrderNumber(int value) const
//  {
//      return ConstItrByOrderNumber(d_recordsByOrderNumber.find(value));
//  }
//
//  inline
//  MyPair<MySalesRecordContainer::ConstItrById,
//         MySalesRecordContainer::ConstItrById>
//  MySalesRecordContainer::findByCustomerId(int value) const
//  {
//      typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;
//
//      HashTableLink *first;
//      HashTableLink *last;
//      d_recordptrsByCustomerId.findRange(&first, &last, value);
//
//      return MyPair<ConstItrById, ConstItrById>(ConstItrById(first),
//                                                ConstItrById(last));
//  }
//
//  inline
//  MyPair<MySalesRecordContainer::ConstItrById,
//         MySalesRecordContainer::ConstItrById>
//  MySalesRecordContainer::findByVendorId(int value) const
//  {
//      typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;
//
//      HashTableLink *first;
//      HashTableLink *last;
//      d_recordptrsByVendorId.findRange(&first, &last, value);
//
//      return MyPair<ConstItrById, ConstItrById>(ConstItrById(first),
//                                                ConstItrById(last));
//  }
//..
// Now, create an empty container and load it with some sample data.
//..
//      MySalesRecordContainer msrc;
//
//      const MySalesRecord DATA[] = {
//          { 1000, 100, 10, "hello" },
//          { 1001, 100, 20, "world" },
//          { 1002, 200, 10, "how" },
//          { 1003, 200, 20, "are" },
//          { 1004, 100, 10, "you" },
//          { 1005, 100, 20, "today" }
//      };
//      const int numDATA = sizeof DATA / sizeof *DATA;
//
//      printf("Insert sales records into container.\n");
//
//      for (int i = 0; i < numDATA; ++i) {
//          const int orderNumber   = DATA[i].orderNumber;
//          const int customerId    = DATA[i].customerId;
//          const int vendorId      = DATA[i].vendorId;
//          const char *description = DATA[i].description;
//
//          printf("%d: %d %d %s\n",
//                 orderNumber,
//                 customerId,
//                 vendorId,
//                 description);
//          MyPair<MySalesRecordContainer::ConstItrByOrderNumber,
//                 bool> status = msrc.insert(DATA[i]);
//          assert(msrc.cend() != status.first);
//          assert(true        == status.second);
//      }
//..
// We find on standard output:
//..
//  Insert sales records into container.
//  1000: 100 10 hello
//  1001: 100 20 world
//  1002: 200 10 how
//  1003: 200 20 are
//  1004: 100 10 you
//  1005: 100 20 today
//..
// We can search our container by order number and find the expected records.
//..
//      printf("Find sales records by order number.\n");
//      for (int i = 0; i < numDATA; ++i) {
//          const int orderNumber   = DATA[i].orderNumber;
//          const int customerId    = DATA[i].customerId;
//          const int vendorId      = DATA[i].vendorId;
//          const char *description = DATA[i].description;
//
//          printf("%d: %d %d %s\n",
//                 orderNumber,
//                 customerId,
//                 vendorId,
//                 description);
//          MySalesRecordContainer::ConstItrByOrderNumber itr =
//                                         msrc.findByOrderNumber(orderNumber);
//          assert(msrc.cend() != itr);
//          assert(orderNumber == itr->orderNumber);
//          assert(customerId  == itr->customerId);
//          assert(vendorId    == itr->vendorId);
//          assert(0 == strcmp(description, itr->description));
//      }
//..
// We find on standard output:
//..
//  Find sales records by order number.
//  1000: 100 10 hello
//  1001: 100 20 world
//  1002: 200 10 how
//  1003: 200 20 are
//  1004: 100 10 you
//  1005: 100 20 today
//..
// We can search our container by customer identifier and find the expected
// records.
//..
//      printf("Find sales records by customer identifier.\n");
//
//      for (int customerId = 100; customerId <= 200; customerId += 100) {
//          MyPair<MySalesRecordContainer::ConstItrById,
//                 MySalesRecordContainer::ConstItrById> result =
//                                           msrc.findByCustomerId(customerId);
//          int count = std::distance(result.first, result.second);
//          printf("customerId %d, count %d\n", customerId, count);
//
//          for (MySalesRecordContainer::ConstItrById itr  = result.first,
//                                                    end  = result.second;
//                                                    end != itr; ++itr) {
//              printf("\t\t%d %d %d %s\n",
//                     itr->orderNumber,
//                     itr->customerId,
//                     itr->vendorId,
//                     itr->description);
//          }
//      }
//..
// We find on standard output:
//..
//  Find sales records by customer identifier.
//  customerId 100, count 4
//              1005 100 20 today
//              1004 100 10 you
//              1001 100 20 world
//              1000 100 10 hello
//  customerId 200, count 2
//              1003 200 20 are
//              1002 200 10 how
//..
// Lastly, we can search our container by vendor identifier and find the
// expected records.
//..
//      printf("Find sales records by vendor identifier.\n");
//
//      for (int vendorId = 10; vendorId <= 20; vendorId += 10) {
//          MyPair<MySalesRecordContainer::ConstItrById,
//                 MySalesRecordContainer::ConstItrById> result =
//                                               msrc.findByVendorId(vendorId);
//          int count = std::distance(result.first, result.second);
//          printf("vendorId %d, count %d\n", vendorId, count);
//
//          for (MySalesRecordContainer::ConstItrById itr  = result.first,
//                                                    end  = result.second;
//                                                    end != itr; ++itr) {
//              printf("\t\t%d %d %d %s\n",
//                     (*itr).orderNumber,
//                     (*itr).customerId,
//                     (*itr).vendorId,
//                     (*itr).description);
//          }
//      }
//..
// We find on standard output:
//..
//  Find sales records by vendor identifier.
//  vendorId 10, count 3
//              1004 100 10 you
//              1002 200 10 how
//              1000 100 10 hello
//  vendorId 20, count 3
//              1005 100 20 today
//              1003 200 20 are
//              1001 100 20 world
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_hashtable.h> header can't be included directly in \
        BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLSTL_BIDIRECTIONALNODEPOOL
#include <bslstl_bidirectionalnodepool.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLALG_FUNCTORADAPTER
#include <bslalg_functoradapter.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEIMPUTIL
#include <bslalg_hashtableimputil.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#include <bslmf_addlvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_BSLEXCEPTIONUTIL
#include <bsls_bslexceptionutil.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>  // for fill_n, max
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef> // for 'size_t'
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_LIMITS
#include <limits>  // for numeric_limits
#define INCLUDED_LIMITS
#endif

namespace BloombergLP {

namespace bslstl {

template <class KEY_CONFIG,
          class HASHER,
          class COMPARATOR,
          class ALLOCATOR = ::bsl::allocator<typename KEY_CONFIG::ValueType> >
class HashTable;

template <class FACTORY>
class HashTable_ArrayProctor;

template <class FACTORY>
class HashTable_NodeProctor;

template <class FUNCTOR>
class HashTable_ComparatorWrapper;

template <class FUNCTOR>
class HashTable_ComparatorWrapper<const FUNCTOR>;

template <class FUNCTOR>
class HashTable_ComparatorWrapper<FUNCTOR &>;

template <class FUNCTOR>
class HashTable_HashWrapper;

template <class FUNCTOR>
class HashTable_HashWrapper<const FUNCTOR>;

template <class FUNCTOR>
class HashTable_HashWrapper<FUNCTOR &>;

struct HashTable_ImpDetails;
struct HashTable_Util;

                       // ======================
                       // class CallableVariable
                       // ======================

template <class CALLABLE>
struct CallableVariable {
    // This metafunction returns a 'type' that is an alias for 'CALLABLE'
    // unless that is a function type, in which case it is an alias for
    // 'CALLABLE &'.  This should be used to declare variables of an arbitrary
    // callable type, typically a template type parameter, that may turn out to
    // be a function type.  Note that this metafunction is necessary as the C++
    // language does not allow variables of function type, nor may functions
    // return a function type.

    typedef typename bsl::conditional<
                            bsl::is_function<CALLABLE>::value,
                            typename bsl::add_lvalue_reference<CALLABLE>::type,
                            CALLABLE>::type type;
};

                           // ===========================
                           // class HashTable_HashWrapper
                           // ===========================

template <class FUNCTOR>
class HashTable_HashWrapper {
    // This class provides a wrapper around a functor satisfying the 'Hash'
    // requirements ({'bslstl_hash'}) such that the function call operator is
    // always declared as 'const' qualified.
    //
    // TBD Provide an optimization for the case of an empty base functor, where
    //     we can safely const_cast want calling the base class operator.
    //
    // Note that we would only one class, not two, with C++11 variadic
    // templates and perfect forwarding, and we could also easily detect
    // whether or not 'FUNCTOR' provided a const-qualified 'operator()'.

  private:
    mutable FUNCTOR d_functor;

  public:
    // CREATORS
    HashTable_HashWrapper();
        // Create a 'HashTable_HashWrapper' object wrapping a 'FUNCTOR' that
        // has its default value.

    explicit HashTable_HashWrapper(const FUNCTOR& fn);
        // Create a 'HashTable_HashWrapper' object wrapping a 'FUNCTOR' that is
        // a copy of the specified 'fn'.

    // MANIPULATORS
    void swap(HashTable_HashWrapper &other);
        // Exchange the value of this object with the specified 'other' object.

    // ACCESSORS
    template <class ARG_TYPE>
    native_std::size_t operator()(ARG_TYPE& arg) const;
        // Call the wrapped 'functor' with the specified 'arg' and return the
        // result.  Note that 'ARG_TYPE' will typically be deduced as a 'const'
        // type.

    const FUNCTOR& functor() const;
        // Return a reference providing non-modifiable access to the hash
        // functor wrapped by this object.
};

template <class FUNCTOR>
class HashTable_HashWrapper<const FUNCTOR> {
    // This partial specialization handles 'const' qualified functors, that may
    // not be stored as a 'mutable' member in the primary template.  The need
    // to wrap such functors diminishes greatly, as there is no need to play
    // mutable tricks to invoke the function call operator.  An alternative to
    // providing this specialization would be to skip the wrapper entirely if
    // using a 'const' qualified functor in a 'HashTable'.  Note that this type
    // has a 'const' qualified data member, so is neither assignable nor
    // swappable.

  private:
    const FUNCTOR d_functor;

  public:
    // CREATORS
    HashTable_HashWrapper();
        // Create a 'HashTable_HashWrapper' object wrapping a 'FUNCTOR' that
        // has its default value.

    explicit HashTable_HashWrapper(const FUNCTOR& fn);
        // Create a 'HashTable_HashWrapper' object wrapping a 'FUNCTOR' that is
        // a copy of the specified 'fn'.

    // ACCESSORS
    template <class ARG_TYPE>
    native_std::size_t operator()(ARG_TYPE& arg) const;
        // Call the wrapped 'functor' with the specified 'arg' and return the
        // result.  Note that 'ARG_TYPE' will typically be deduced as a 'const'
        // type.

    const FUNCTOR& functor() const;
        // Return a reference providing non-modifiable access to the hash
        // functor wrapped by this object.
};

template <class FUNCTOR>
class HashTable_HashWrapper<FUNCTOR &> {
    // This partial specialization handles 'const' qualified functors, that may
    // not be stored as a 'mutable' member in the primary template.  Note that
    // the 'FUNCTOR' type itself may be 'const'-qualified, so this one partial
    // template specialization also handles 'const FUNCTOR&' references.  In
    // order to correctly parse with the reference-binding rules, we drop the
    // 'const' in front of many of the references to 'FUNCTOR' seen in the
    // primary template definition.  Note that this type has a reference data
    // member, so is not default constructible, assignable or swappable.

  private:
    FUNCTOR& d_functor;

  public:
    // CREATORS
    explicit HashTable_HashWrapper(FUNCTOR& fn);
        // Create a 'HashTable_HashWrapper' object wrapping a 'FUNCTOR' that is
        // a copy of the specified 'fn'.

    // ACCESSORS
    template <class ARG_TYPE>
    native_std::size_t operator()(ARG_TYPE& arg) const;
        // Call the wrapped 'functor' with the specified 'arg' and return the
        // result.  Note that 'ARG_TYPE' will typically be deduced as a 'const'
        // type.

    FUNCTOR& functor() const;
        // Return a reference providing non-modifiable access to the hash
        // functor wrapped by this object.
};

template <class FUNCTOR>
void swap(HashTable_HashWrapper<FUNCTOR> &a,
          HashTable_HashWrapper<FUNCTOR> &b);
    // Swap the functor wrapped by the specified 'a' object with the functor
    // wrapped by the specified 'b' object.

                           // =================================
                           // class HashTable_ComparatorWrapper
                           // =================================

template <class FUNCTOR>
class HashTable_ComparatorWrapper {
    // This class provides a wrapper around a functor that can compare two
    // values and return a 'bool', so that the function call operator is always
    // declared as 'const' qualified.
    //
    // TBD Provide an optimization for the case of an empty base functor, where
    //     we can safely const_cast want calling the base class operator.

  private:
    mutable FUNCTOR d_functor;

  public:
    // CREATORS
    HashTable_ComparatorWrapper();
        // Create a 'HashTable_ComparatorWrapper' object wrapping a 'FUNCTOR'
        // that has its default value.

    explicit HashTable_ComparatorWrapper(const FUNCTOR& fn);
        // Create a 'HashTable_ComparatorWrapper' object wrapping a 'FUNCTOR'
        // that is a copy of the specified 'fn'.

    // MANIPULATORS
    void swap(HashTable_ComparatorWrapper &other);
        // Exchange the value of this object with the specified 'other' object.

    // ACCESSORS
    template <class ARG1_TYPE, class ARG2_TYPE>
    bool operator()(ARG1_TYPE& arg1, ARG2_TYPE& arg2) const;
        // Call the wrapped 'functor' with the specified 'arg1' and 'arg2' (in
        // that order) and return the result.  Note that 'ARGn_TYPE' will
        // typically be deduced as a 'const' type.

    const FUNCTOR& functor() const;
        // Return a reference providing non-modifiable access to the hash
        // functor wrapped by this object.
};

template <class FUNCTOR>
class HashTable_ComparatorWrapper<const FUNCTOR> {
    // This partial specialization handles 'const' qualified functors, that may
    // not be stored as a 'mutable' member in the primary template.  The need
    // to wrap such functors diminishes greatly, as there is no need to play
    // mutable tricks to invoke the function call operator.  An alternative to
    // providing this specialization would be to skip the wrapper entirely if
    // using a 'const' qualified functor in a 'HashTable'.  Note that this type
    // has a 'const' qualified data member, so is neither assignable nor
    // swappable.

  private:
    const FUNCTOR d_functor;

  public:
    // CREATORS
    HashTable_ComparatorWrapper();
        // Create a 'HashTable_ComparatorWrapper' object wrapping a 'FUNCTOR'
        // that has its default value.

    explicit HashTable_ComparatorWrapper(const FUNCTOR& fn);
        // Create a 'HashTable_ComparatorWrapper' object wrapping a 'FUNCTOR'
        // that is a copy of the specified 'fn'.

    // ACCESSORS
    template <class ARG1_TYPE, class ARG2_TYPE>
    bool operator()(ARG1_TYPE& arg1, ARG2_TYPE& arg2) const;
        // Call the wrapped 'functor' with the specified 'arg1' and 'arg2' (in
        // that order) and return the result.  Note that 'ARGn_TYPE' will
        // typically be deduced as a 'const' type.


    const FUNCTOR& functor() const;
        // Return a reference providing non-modifiable access to the hash
        // functor wrapped by this object.
};

template <class FUNCTOR>
class HashTable_ComparatorWrapper<FUNCTOR &> {
    // This partial specialization handles 'const' qualified functors, that may
    // not be stored as a 'mutable' member in the primary template.  Note that
    // the 'FUNCTOR' type itself may be 'const'-qualified, so this one partial
    // template specialization also handles 'const FUNCTOR&' references.  In
    // order to correctly parse with the reference-binding rules, we drop the
    // 'const' in front of many of the references to 'FUNCTOR' seen in the
    // primary template definition.  Note that this type has a reference data
    // member, so is not default constructible, assignable or swappable.

  private:
    FUNCTOR& d_functor;

  public:
    // CREATORS
    explicit HashTable_ComparatorWrapper(FUNCTOR& fn);
        // Create a 'HashTable_ComparatorWrapper' object wrapping a 'FUNCTOR'
        // that is a copy of the specified 'fn'.

    // ACCESSORS
    template <class ARG1_TYPE, class ARG2_TYPE>
    bool operator()(ARG1_TYPE& arg1, ARG2_TYPE& arg2) const;
        // Call the wrapped 'functor' with the specified 'arg1' and 'arg2' (in
        // that order) and return the result.  Note that 'ARGn_TYPE' will
        // typically be deduced as a 'const' type.

    FUNCTOR& functor() const;
        // Return a reference providing non-modifiable access to the hash
        // functor wrapped by this object.
};

template <class FUNCTOR>
void swap(HashTable_ComparatorWrapper<FUNCTOR> &lhs,
          HashTable_ComparatorWrapper<FUNCTOR> &rhs);
    // Swap the functor wrapped by the specified 'lhs' object with the functor
    // wrapped by the specified 'rhs' object.

                           // ===============
                           // class HashTable
                           // ===============

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
class HashTable_ImplParameters;

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
class HashTable {
    // This class template implements a value-semantic container type holding
    // an unordered sequence of (possibly duplicate) elements, that can be
    // rapidly accessed using their key, with the constraint on the container
    // that elements whose keys compare equal according to the specified
    // 'COMPARATOR' will be stored in a stable, contiguous sequence within the
    // container.  The value type and key type of the elements maintained by a
    // 'HashTable' are determined by aliases provided through the (template
    // parameter) type 'KEY_CONFIG'.  Elements in a 'HashTable' are stored in
    // "nodes" that are allocated using an allocator of the specified
    // 'ALLOCATOR' type (rebound to the node type), and elements are
    // constructed directly in the node using the allocator as described in the
    // C++11 standard under the allocator-aware container requirements in
    // ([container.requirements.general], C++11 23.2.1).  The (template
    // parameter) types 'HASHER' and 'COMPARATOR' shall be copy-constructible
    // function-objects.  'HASHER' shall support a function call operator
    // compatible with the following statements:
    //..
    //  HASHER              hash;
    //  KEY_CONFIG::KeyType key;
    //  std::size_t result = hash(key);
    //..
    // where the definition of the called function meets the requirements of a
    // hash function, as specified in {'bslstl_hash'}.  'COMPARATOR' shall
    // support the a function call operator compatible with the following
    // statements:
    //..
    //  COMPARATOR          compare;
    //  KEY_CONFIG::KeyType key1, key2;
    //  bool result = compare(key1, key2);
    //..
    // where the definition of the called function defines an equivalence
    // relationship on keys that is both reflexive and transitive.  The
    // 'HASHER' and 'COMPARATOR' attributes of this class are further
    // constrained, such for any two objects whose keys compare equal by the
    // comparator, shall produce the same value from the hasher.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

  public:
    // TYPES
    typedef ALLOCATOR                              AllocatorType;
    typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
    typedef typename KEY_CONFIG::KeyType           KeyType;
    typedef typename KEY_CONFIG::ValueType         ValueType;
    typedef bslalg::BidirectionalNode<ValueType>   NodeType;
    typedef typename AllocatorTraits::size_type    SizeType;

  private:
    // PRIVATE TYPES
    typedef
    HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>
                                                                ImplParameters;

  private:
    // DATA
    ImplParameters      d_parameters;    // policies governing table behavior
    bslalg::HashTableAnchor
                        d_anchor;        // list root and bucket array
    SizeType            d_size;          // number of elements in this table
    SizeType            d_capacity;      // max number of elements before a
                                         // rehash is required (computed from
                                         // 'd_maxLoadFactor')
    float               d_maxLoadFactor; // maximum permitted load factor

  private:
    // PRIVATE MANIPULATORS
    void copyDataStructure(bslalg::BidirectionalLink *cursor);
        // Copy the sequence of elements from the list starting at the
        // specified 'cursor' and having 'size' elements.  Allocate a bucket
        // array sufficiently large to store 'size' elements while respecting
        // the 'maxLoadFactor', and index the copied list into that new array
        // of hash buckets.  This hash table then takes ownership of the list
        // and bucket array.  Note that this method is intended to be called
        // from copy constructors, which will have assigned some initial values
        // for the 'size' and other attributes that may not be consistent with
        // the class invariants until after this method is called.

    void quickSwapExchangeAllocators(HashTable *other);
        // Efficiently exchange the value, functors, and allocator of this
        // object with those of the specified 'other' object.  This method
        // provides the no-throw exception-safety guarantee.

    void quickSwapRetainAllocators(HashTable *other);
        // Efficiently exchange the value and functors this object with those
        // of the specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    void rehashIntoExactlyNumBuckets(SizeType newNumBuckets,
                                     SizeType capacity);
        // Re-organize this hash-table to have exactly the specified
        // 'newNumBuckets', which will then be able to store the specified
        // 'capacity' number of elements without exceeding the 'maxLoadFactor'.
        // This operation provides the strong exception guarantee (see
        // {'bsldoc_glossary'}) unless the 'hasher' throws, in which case this
        // operation provides the basic exception guarantee, leaving the
        // hash-table in a valid, but otherwise unspecified (and potentially
        // empty), state.  The behavior is undefined unless
        // 'size / newNumBuckets <= maxLoadFactor'.  Note that the caller is
        // responsible for correctly computing the 'capacity' supported by the
        // new number of buckets.  This allows for a minor optimization where
        // the value is computed only once per rehash.

    void removeAllAndDeallocate();
        // Erase all the nodes in this hash-table, and deallocate their memory
        // via the supplied node-factory.  Destroy the array of buckets owned
        // by this hash-table.  If 'd_anchor.bucketAddress()' is the default
        // bucket address ('HashTable_ImpDetails::defaultBucketAddress'), then
        // this hash-table does not own its array of buckets, and it will not
        // be destroyed.

    void removeAllImp();
        // Erase all the nodes in this table and deallocate their memory via
        // the node factory, without performing the necessary bookkeeping to
        // reflect such change.  Note that this (private) method explicitly
        // leaves the HashTable in an inconsistent state, and is expected to be
        // useful when the anchor of this hash table is about to be overwritten
        // with a new value, or when the hash table is going out of scope and
        // the extra bookkeeping is not necessary.

    // PRIVATE ACCESSORS
    template <class DEDUCED_KEY>
    bslalg::BidirectionalLink *find(DEDUCED_KEY&       key,
                                    native_std::size_t hashValue) const;
        // Return the address of the first node in this hash table having a key
        // that compares equal (according to this hash-table's 'comparator') to
        // the specified 'key'.  The behavior is undefined unless the specified
        // 'hashValue' is the hash code for the 'key' according to the 'hasher'
        // functor of this hash table.  Note that this function's
        // implementation relies on the supplied 'hashValue' rather than
        // recomputing it, eliminating some redundant computation for the
        // public methods.

    bslalg::HashTableBucket *getBucketAddress(SizeType bucketIndex) const;
        // Return the address of the bucket at the specified 'bucketIndex' in
        // bucket array of this hash table.  The behavior is undefined unless
        // 'bucketIndex < this->numBuckets()'.

    native_std::size_t hashCodeForNode(bslalg::BidirectionalLink *node) const;
        // Return the hash code for the element stored in the specified 'node'
        // using a copy of the hash functor supplied at construction.  The
        // behavior is undefined unless 'node' points to a list-node of type
        // 'bslalg::BidirectionalNode<KEY_CONFIG::ValueType>'.

  public:
    // CREATORS
    explicit HashTable(const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create an empty 'HashTable' object with a 'maxLoadFactor' of 1.0.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'allocator' is not supplied, a default-constructed object of the
        // (template parameter) type 'ALLOCATOR' is used.  Use a default
        // constructed object of the (template parameter) type 'HASHER' and a
        // default constructed object of the (template parameter) type
        // 'COMPARATOR' to organize elements in the table.  If the 'ALLOCATOR'
        // is 'bsl::allocator' (the default), then 'allocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' is
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed default allocator is used to supply memory.  No memory
        // is allocated unless the 'HASHER' or 'COMPARATOR' types allocate
        // memory in their default constructor.

    HashTable(const HASHER&     hash,
              const COMPARATOR& compare,
              SizeType          initialNumBuckets,
              float             initialMaxLoadFactor,
              const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create an empty hash-table using the specified 'hash' and 'compare'
        // functors to organize elements in the table, which will initially
        // have at least the specified 'initialNumBuckets' and a
        // 'maxLoadFactor' of the specified 'initialMaxLoadFactor'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not supplied, a default-constructed object of
        // the (template parameter) type 'ALLOCATOR' is used.  If the
        // 'ALLOCATOR' is 'bsl::allocator' (the default), then
        // 'basicAllocator', if supplied, shall be convertible to
        // 'bslma::Allocator *'.  If the 'ALLOCATOR' is 'bsl::allocator' and
        // 'basicAllocator' is not supplied, the currently installed default
        // allocator is used to supply memory.  If this constructor tries to
        // allocate a number of buckets larger than can be represented by this
        // hash-table's 'SizeType', a 'std::length_error' exception is thrown.
        // The behavior is undefined unless '0 < initialMaxLoadFactor'.  Note
        // that more than 'initialNumBuckets' buckets may be created in order
        // to preserve the bucket allocation strategy of the hash-table (but
        // never fewer).

    HashTable(const HashTable& original);
        // Create a 'HashTable' having the same value as the specified
        // 'original', and the same 'maxLoadFactor'.  Use a copy of
        // 'original.hasher()' and a copy of 'original.comparator()' to
        // organize elements in this hash-table.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        //  select_on_container_copy_construction(original.allocator())'
        // to allocate memory.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // (the default), the currently installed default allocator is used to
        // supply memory.  Note that this hash-table may have fewer buckets
        // than 'original', and a correspondingly higher 'loadFactor', so long
        // as 'maxLoadFactor' is not exceeded.  Note that the created hash
        // table may have a different 'numBuckets' than 'original', and a
        // correspondingly different 'loadFactor', as long as 'maxLoadFactor'
        // is not exceeded.

    HashTable(const HashTable& original, const ALLOCATOR& allocator);
        // Create a 'HashTable' having the same value and 'maxLoadFactor' as
        // the specified 'original', that will use the specified 'allocator' to
        // supply memory.  Use a copy of 'original.hasher()' and a copy of
        // 'original.comparator()' to organize elements in this hash- table.
        // Note that this hash-table may have a different 'numBuckets' than
        // 'original', and a correspondingly different 'loadFactor', as long as
        // 'maxLoadFactor' is not exceeded.

    ~HashTable();
        // Destroy this object.

    // MANIPULATORS
    HashTable& operator=(const HashTable& rhs);
        // Assign to this object the value, hasher, comparator and
        // 'maxLoadFactor' of the specified 'rhs' object, replace the allocator
        // of this object with the allocator of 'rhs' if the 'ALLOCATOR' type
        // has the trait 'propagate_on_container_copy_assignment', and return a
        // reference providing modifiable access to this object.  Note that
        // this method requires that the parameterized 'HASHER' and
        // 'COMPARATOR' types be "copy-constructible", "copy-assignable" and
        // "swappable" (see {Requirements on 'KEY_CONFIG'}).  Also note that
        // these requirements are modeled after the unordered container
        // requirements table in the C++11 standard, which is imprecise on this
        // operation; these requirements might simplify in the future, if the
        // standard is updated.

    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insert(const SOURCE_TYPE& value);
        // Insert the specified 'value' into this hash-table, and return the
        // address of the new node.  If this hash-table already contains an
        // element having the same key as 'value' (according to this hash-
        // table's 'comparator'), then insert 'value' immediately before the
        // first element having the same key.  Additional buckets are
        // allocated, as needed, to preserve the invariant
        // 'loadFactor <= maxLoadFactor'.  If this function tries to allocate a
        // number of buckets larger than can be represented by this hash
        // table's 'SizeType', a 'std::length_error' exception is thrown.

    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insert(const SOURCE_TYPE&         value,
                                      bslalg::BidirectionalLink *hint);
        // Insert the specified 'value' into this hash table, and return the
        // address of the new node.  If the element stored in the node pointer
        // to by the specified 'hint' has a key that compares equal to that of
        // 'value', then 'value' is inserted immediately preceding 'hint' in
        // the list of this hash table.  Additional buckets will be allocated,
        // as needed, to preserve the invariant 'loadFactor <= maxLoadFactor'.
        // If this function tries to allocate a number of buckets larger than
        // can be represented by this hash table's 'SizeType', a
        // 'std::length_error' exception is thrown.  The behavior is undefined
        // unless 'hint' points to a node in this hash table.

    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insertIfMissing(
                                            bool               *isInsertedFlag,
                                            const SOURCE_TYPE&  value);
    bslalg::BidirectionalLink *insertIfMissing(
                                            bool               *isInsertedFlag,
                                            const ValueType&    value);
        // Return the address of an element in this hash table having a key
        // that compares equal to the key of the specified 'value' using the
        // 'comparator' functor of this hash-table.  If no such element exists,
        // insert a 'value' into this hash-table and return the address of that
        // newly inserted node.  Load 'true' into the specified
        // 'isInsertedFlag' if insertion is performed, and 'false' if an
        // existing element having a matching key was found.  If this
        // hash-table contains more than one element with a matching key,
        // return the first such element (from the contiguous sequence of
        // elements having a matching key).  Additional buckets are allocated,
        // as needed, to preserve the invariant 'loadFactor <= maxLoadFactor'.
        // If this function tries to allocate a number of buckets larger than
        // can be represented by this hash table's 'SizeType', a
        // 'std::length_error' exception is thrown.

    bslalg::BidirectionalLink *insertIfMissing(const KeyType& key);
        // Return the address of a link holding an element whose key has the
        // same value as the specified 'key' (according to this hash-table's
        // 'comparator'), and, if no such link exists, insert a new link having
        // a default value with a key that is the same as 'key'.  If this
        // hash-table contains more than one element with the supplied 'key',
        // return the first such element (from the contiguous sequence of
        // elements having a matching key).  Additional buckets are allocated,
        // as needed, to preserve the invariant 'loadFactor <= maxLoadFactor'.
        // If this function tries to allocate a number of buckets larger than
        // can be represented by this hash table's 'SizeType', a
        // 'std::length_error' exception is thrown.

    void rehashForNumBuckets(SizeType newNumBuckets);
        // Re-organize this hash-table to have at least the specified
        // 'newNumBuckets', preserving the invariant
        // 'loadFactor <= maxLoadFactor'.  If this function tries to allocate a
        // number of buckets larger than can be represented by this hash
        // table's 'SizeType', a 'std::length_error' exception is thrown.  This
        // operation provides the strong exception guarantee (see
        // {'bsldoc_glossary'}) unless the 'hasher' throws, in which case this
        // operation provides the basic exception guarantee, leaving the
        // hash-table in a valid, but otherwise unspecified (and potentially
        // empty), state.  Note that more buckets than requested may be
        // allocated in order to preserve the bucket allocation strategy of the
        // hash table (but never fewer).

    bslalg::BidirectionalLink *remove(bslalg::BidirectionalLink *node);
        // Remove the specified 'node' from this hash-table, and return the
        // address of the node immediately after 'node' this hash-table (prior
        // to its removal), or a null pointer value if 'node' is the last node
        // in the table.  The behavior is undefined unless 'node' refers to a
        // node in this hash-table.

    void removeAll();
        // Remove all the elements from this hash-table.  Note that this
        // hash-table is empty after this call, but allocated memory may be
        // retained for future use.  The destructor of each (non-trivial)
        // element that is remove shall be run.

    void reserveForNumElements(SizeType numElements);
        // Re-organize this hash-table to have a sufficient number of buckets
        // to accommodate at least the specified 'numElements' without
        // exceeding the 'maxLoadFactor', and ensure that that there are
        // sufficient nodes pre-allocated in this object's node pool.  If this
        // function tries to allocate a number of buckets larger than can be
        // represented by this hash table's 'SizeType', a 'std::length_error'
        // exception is thrown.  This operation provides the strong exception
        // guarantee (see {'bsldoc_glossary'}) unless the 'hasher' throws, in
        // which case this operation provides the basic exception guarantee,
        // leaving the hash-table in a valid, but otherwise unspecified (and
        // potentially empty), state.

    void setMaxLoadFactor(float newMaxLoadFactor);
        // Set the maximum load factor permitted by this hash table to the
        // specified 'newMaxLoadFactor', where load factor is the statistical
        // mean number of elements per bucket.  If 'newMaxLoadFactor <
        // loadFactor', allocate at least enough buckets to re-establish the
        // invariant 'loadFactor <= maxLoadFactor'.  If this function tries to
        // allocate a number of buckets larger than can be represented by this
        // hash table's 'SizeType', a 'std::length_error' exception is thrown.
        // The behavior is undefined unless '0 < maxLoadFactor'.

    void swap(HashTable& other);
        // Exchange the value of this object, its 'comparator' functor, its
        // 'hasher' functor, and its 'maxLoadFactor' with those of the
        // specified 'other' object.  Additionally, if
        // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true', then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee unless any
        // of the 'comparator' or 'hasher' functors throw when swapped, leaving
        // both objects in an safely destructible, but otherwise unusable,
        // state.  The operation guarantees 'O[1]' complexity.  The behavior is
        // undefined unless either this object has an allocator that compares
        // equal to the allocator of 'other', or the trait
        // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true'.

    // ACCESSORS
    ALLOCATOR allocator() const;
        // Return a copy of the allocator used to construct this hash table.
        // Note that this is not the allocator used to allocate elements for
        // this hash table, which is instead a copy of that allocator rebound
        // to allocate the nodes used by the internal data structure of this
        // hash table.

    const bslalg::HashTableBucket& bucketAtIndex(SizeType index) const;
        // Return a reference offering non-modifiable access to the
        // 'HashTableBucket' at the specified 'index' position in the array of
        // buckets of this table.  The behavior is undefined unless 'index <
        // numBuckets()'.

    SizeType bucketIndexForKey(const KeyType& key) const;
        // Return the index of the bucket that would contain all the elements
        // having the specified 'key'.

    const COMPARATOR& comparator() const;
        // Return a reference providing non-modifiable access to the
        // key-equality comparison functor used by this hash table.

    SizeType countElementsInBucket(SizeType index) const;
        // Return the number elements contained in the bucket at the specified
        // 'index'.  Note that this operation has linear run-time complexity
        // with respect to the number of elements in the indexed bucket.

    bslalg::BidirectionalLink *elementListRoot() const;
        // Return the address of the first element in this hash table, or a
        // null pointer value if this hash table is empty.

    bslalg::BidirectionalLink *find(const KeyType& key) const;
        // Return the address of a link whose key has the same value as the
        // specified 'key' (according to this hash-table's 'comparator'), and a
        // null pointer value if no such link exists.  If this hash-table
        // contains more than one element having the supplied 'key', return the
        // first such element (from the contiguous sequence of elements having
        // the same key).

    bslalg::BidirectionalLink *findEndOfRange(
                                       bslalg::BidirectionalLink *first) const;
        // Return the address of the first node after any nodes holding a value
        // having the same key as the specified 'first' node (according to this
        // hash-table's 'comparator'), and a null pointer value if all nodes
        // following 'first' hold values with the same key as 'first'.  The
        // behavior is undefined unless 'first' is a link in this hash- table.
        // Note that this hash-table ensures all elements having the same key
        // form a contiguous sequence.

    void findRange(bslalg::BidirectionalLink **first,
                   bslalg::BidirectionalLink **last,
                   const KeyType&              key) const;
        // Load into the specified 'first' and 'last' pointers the respective
        // addresses of the first and last link (in the list of elements owned
        // by this hash table) where the contained elements have a key that
        // compares equal to the specified 'key' using the 'comparator' of this
        // hash-table, and null pointers values if there are no elements
        // matching 'key'.  Note that the output values will form a closed
        // range, where both 'first' and 'last' point to links satisfying the
        // predicate (rather than a semi-open range where 'last' would point to
        // the element following the range).  Also note that this hash-table
        // ensures all elements having the same key form a contiguous sequence.

    bool hasSameValue(const HashTable& other) const;
        // Return 'true' if the specified 'other' has the same value as this
        // object, and 'false' otherwise.  Two 'HashTable' objects have the
        // same value if they have the same number of elements, and for every
        // subset of elements in this object having keys that compare equal
        // (according to that hash table's 'comparator'), a corresponding
        // subset of elements exists in the 'other' object, having the same
        // number of elements, where, for some permutation of the subset in
        // this object, every element in that subset compares equal (using
        // 'operator==') to the corresponding element in the 'other' subset.
        // The behavior is undefined unless both the 'hasher' and 'comparator'
        // of this object and the 'other' return the same value for every valid
        // input.  Note that this method requires that the 'ValueType' of the
        // parameterized 'KEY_CONFIG' be "equality-comparable" (see
        // {Requirements on 'KEY_CONFIG'}).

    const HASHER& hasher() const;
        // Return a reference providing non-modifiable access to the hash
        // functor used by this hash-table.

    float loadFactor() const;
        // Return the current load factor for this table.  The load factor is
        // the statistical mean number of elements per bucket.

    float maxLoadFactor() const;
        // Return the maximum load factor permitted by this hash table object,
        // where the load factor is the statistical mean number of elements per
        // bucket.  Note that this hash table will enforce the maximum load
        // factor by rehashing into a larger array of buckets on any any
        // insertion operation where a successful insertion would exceed the
        // maximum load factor.  The maximum load factor may actually be less
        // than the current load factor if the maximum load factor has been
        // reset, but no insert operations have yet occurred.

    SizeType maxNumBuckets() const;
        // Return a theoretical upper bound on the largest number of buckets
        // that this hash-table could possibly have.  Note that there is no
        // guarantee that the hash-table can successfully maintain that number
        // of buckets, or even close to that number of buckets without running
        // out of resources.

    SizeType maxSize() const;
        // Return a theoretical upper bound on the largest number of elements
        // that this hash-table could possibly hold.  Note that there is no
        // guarantee that the hash-table can successfully grow to the returned
        // size, or even close to that size without running out of resources.

    SizeType numBuckets() const;
        // Return the number of buckets contained in this hash table.

    SizeType rehashThreshold() const;
        // Return the number of elements this hash table can hold without
        // requiring a rehash operation in order to respect the
        // 'maxLoadFactor'.

    SizeType size() const;
        // Return the number of elements in this hash table.
};

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void swap(HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& x,
          HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& y);
    // Swap both the value, the hasher, the comparator and the 'maxLoadFactor'
    // of the specified 'x' object with the value, the hasher, the comparator
    // and the 'maxLoadFactor' of the specified 'y' object.  Additionally, if
    // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true', then exchange the allocator of 'x' with that of 'y', and do not
    // modify either allocator otherwise.  This method guarantees 'O[1]'
    // complexity if 'x' and 'y' have the same allocator or if the allocators
    // propagate on swap, otherwise this operation will typically pay the cost
    // of two copy constructors, which may in turn throw.  If the allocators
    // are the same or propagate, then this method provides the no-throw
    // exception-safety guarantee unless the 'swap' function of the hasher or
    // comparator throw.  Otherwise this method offers only the basic exception
    // safety guarantee.

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bool operator==(
              const HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& lhs,
              const HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'HashTable' objects have the same
    // value if they have the same number of elements, and for every subset of
    // elements in 'lhs' having keys that compare equal (according to that hash
    // table's 'comparator'), a corresponding subset of elements exists in
    // 'rhs', having the same number of elements, where, for some permutation
    // of the 'lhs' subset, every element in that subset compares equal (using
    // 'operator==') to the corresponding element in the 'rhs' subset.  The
    // behavior is undefined unless both the 'hasher' and 'comparator' of 'lhs'
    // and 'rhs' return the same value for every valid input.  Note that this
    // method requires that the 'ValueType' of the parameterized 'KEY_CONFIG'
    // be "equality-comparable" (see {Requirements on 'KEY_CONFIG'}).

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bool operator!=(
              const HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& lhs,
              const HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'HashTable' objects do not have
    // the same value if they do not have the same number of elements, or if,
    // for any key found in 'lhs', the subset of elements having that key
    // (according to the hash-table's 'comparator') in 'lhs' either (1) does
    // not have the same number of elements as the subset of elements having
    // that key in 'rhs', or (2) there exists no permutation of the 'lhs'
    // subset where each element compares equal (using 'operator==') to the
    // corresponding element in the 'rhs' subset.  The behavior is undefined
    // unless both the 'hasher' and 'comparator' of 'lhs' and 'rhs' return the
    // same value for every valid input.  Note that this method requires that
    // the 'ValueType' of the parameterized 'KEY_CONFIG' be
    // "equality-comparable" (see {Requirements on 'KEY_CONFIG'}).

                    // ============================
                    // class HashTable_ArrayProctor
                    // ============================

template <class FACTORY>
class HashTable_ArrayProctor {
    // This class probably already exists in 'bslalg'

  private:
    // DATA
    FACTORY                 *d_factory;
    bslalg::HashTableAnchor *d_anchor;

  private:
    // NOT IMPLEMENTED
    HashTable_ArrayProctor(const HashTable_ArrayProctor&);
    HashTable_ArrayProctor& operator=(const HashTable_ArrayProctor&);

  public:
    // CREATORS
    HashTable_ArrayProctor(FACTORY                 *factory,
                           bslalg::HashTableAnchor *anchor);
        // Create a 'HashTable_ArrayProctor' managing the hash-table data
        // structure owned by the specified 'anchor' that was created using the
        // specified 'factory'.

    ~HashTable_ArrayProctor();
        // Destroy the hash-table data structure managed by this proctor and
        // reclaim all of its resources, unless there was a call to 'release'
        // this proctor.

    // MANIPULATORS
    void release();
        // Release from management the object currently managed by this
        // proctor.  If no object is currently being managed, this method has
        // no effect.
};

                    // ===========================
                    // class HashTable_NodeProctor
                    // ===========================

template <class FACTORY>
class HashTable_NodeProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically deallocates a managed list of
    // nodes upon destruction by recursively invoking the 'deleteNode' method
    // of a supplied factory on each node.  The (template parameter) type
    // 'FACTORY' shall be provide a member function that can be called as if it
    // had the following signature:
    //..
    //  void deleteNode(bslalg::BidirectionalLink *node);
    //..

  private:
    // DATA
    FACTORY                   *d_factory;
    bslalg::BidirectionalLink *d_node;

  private:
    // NOT IMPLEMENTED
    HashTable_NodeProctor(const HashTable_NodeProctor&);
    HashTable_NodeProctor& operator=(const HashTable_NodeProctor&);

  public:
    // CREATORS
    HashTable_NodeProctor(FACTORY                   *factory,
                          bslalg::BidirectionalLink *node);
        // Create a new node-proctor that conditionally manages the specified
        // 'node' (if non-zero), and that uses the specified 'factory' to
        // destroy the node (unless released) upon its destruction.  The
        // behavior is undefined unless 'node' was created by the 'factory'.

    ~HashTable_NodeProctor();
        // Destroy this node proctor, and delete the node that it manages (if
        // any) by invoking the 'deleteNode' method of the factory supplied at
        // construction.  If no node is currently being managed, this method
        // has no effect.

    // MANIPULATORS
    void release();
        // Release from management the node currently managed by this proctor.
        // If no object is currently being managed, this method has no effect.
};

                    // ==========================
                    // class HashTable_ImpDetails
                    // ==========================

struct HashTable_ImpDetails {
    // This utility 'struct' provides a namespace for functions that are useful
    // when implementing a hash table.

    // CLASS METHODS
    static bslalg::HashTableBucket *defaultBucketAddress();
        // Return the address of a statically initialized empty bucket that can
        // be shared as the (un-owned) bucket array by all empty hash tables.

    static size_t growBucketsForLoadFactor(size_t *capacity,
                                           size_t  minElements,
                                           size_t  requestedBuckets,
                                           double  maxLoadFactor);
        // Return the suggested number of buckets to index a linked list that
        // can hold as many as the specified 'minElements' without exceeding
        // the specified 'maxLoadFactor', and supporting at least the specified
        // number of 'requestedBuckets'.  Set the specified '*capacity' to the
        // maximum length of linked list that the returned number of buckets
        // could index without exceeding the 'maxLoadFactor'.  The behavior is
        // undefined unless '0 < maxLoadFactor', '0 < minElements' and
        // '0 < requestedBuckets'.

    static bslma::Allocator *incidentalAllocator();
        // Return that address of an allocator that can be used to allocate
        // temporary storage, but that is neither the default nor global
        // allocator.  Note that this function is intended to support detailed
        // checks in 'SAFE_2' builds, that may need additional storage for the
        // evaluation of a validity check on a large data structure, but that
        // should not change the expected values computed for regular allocator
        // usage of the component as validated by the test driver.

    static size_t nextPrime(size_t n);
        // Return the next prime number greater-than or equal to the specified
        // 'n' in the increasing sequence of primes chosen to disperse hash
        // codes across buckets as uniformly as possible.  Throw a
        // 'std::length_error' exception if 'n' is greater than the last prime
        // number in the sequence.  Note that, typically, prime numbers in the
        // sequence have increasing values that reflect a growth factor (e.g.,
        // each value in the sequence may be, approximately, two times the
        // preceding value).
};

                    // ====================
                    // class HashTable_Util
                    // ====================

struct HashTable_Util {
    // This utility 'struct' provide utilities for initializing and destroying
    // bucket lists in anchors that are managed by a 'HashTable'.  They cannot
    // migrate down to 'bslalg::HashTableImpUtil' as they rely on the standard
    // library 'bslstl_allocatortraits' for their implementation.

    // CLASS METHODS
    template <class TYPE>
    static void assertNotNullPointer(TYPE&);
    template <class TYPE>
    static void assertNotNullPointer(TYPE * const& ptr);
    template <class TYPE>
    static void assertNotNullPointer(TYPE * & ptr);
        // Assert that the passed argument (the specified 'ptr') is not a null
        // pointer value.  Note that this utility is necessary as the
        // 'HashTable' class template may be instantiated with function
        // pointers for the hasher or comparator policies, but there is no easy
        // way to assert in general that the value of a generic type passed to
        // a function is not a null pointer value.

    template<class ALLOCATOR>
    static void destroyBucketArray(bslalg::HashTableBucket *data,
                                   native_std::size_t       bucketArraySize,
                                   const ALLOCATOR&         allocator);
        // Destroy the specified 'data' array of the specified length
        // 'bucketArraySize', that was allocated by the specified 'allocator'.

    template<class ALLOCATOR>
    static void initAnchor(bslalg::HashTableAnchor *anchor,
                           native_std::size_t       bucketArraySize,
                           const ALLOCATOR&         allocator);
        // Load into the specified 'anchor' a (contiguous) array of buckets of
        // the specified 'bucketArraySize' using memory supplied by the
        // specified 'allocator'.  The behavior is undefined unless
        // '0 < bucketArraySize' and '0 == anchor->bucketArraySize()'.  Note
        // that this operation has no effect on 'anchor->listRootAddress()'.
};

                   // ==============================
                   // class HashTable_ImplParameters
                   // ==============================

    // It looks like the 'CallableVariable' adaptation would be more
    // appropriately addressed as part of the 'bslalg::FunctorAdapter' wrapper
    // than intrusively in this component, and in similar ways by any other
    // container trying to support the full range of standard conforming
    // functors.  Given that our intent is to support standard predicates, it
    // may be appropriate to handle calling non-const 'operator()' overloads
    // (via a mutable member) too.

template <class HASHER>
struct HashTable_BaseHasher
     : bslalg::FunctorAdapter<HashTable_HashWrapper<
                                     typename CallableVariable<HASHER>::type> >
{
};

template <class COMPARATOR>
struct HashTable_Comparator
     : bslalg::FunctorAdapter<HashTable_ComparatorWrapper<
                                 typename CallableVariable<COMPARATOR>::type> >
{
};

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
class HashTable_ImplParameters
    : private HashTable_BaseHasher<HASHER>::Type
    , private HashTable_Comparator<COMPARATOR>::Type
{
    // This class holds all the parameterized parts of a 'HashTable' class,
    // efficiently exploiting the empty base optimization without adding
    // unforeseen namespace associations to the 'HashTable' class itself due to
    // the structural inheritance.

    typedef typename HashTable_BaseHasher<HASHER>::Type     BaseHasher;
    typedef typename HashTable_Comparator<COMPARATOR>::Type BaseComparator;

    // typedefs stolen from HashTable
    typedef ALLOCATOR                              AllocatorType;
    typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
    typedef typename KEY_CONFIG::ValueType         ValueType;
    typedef bslalg::BidirectionalNode<ValueType>   NodeType;

  public:
    // PUBLIC TYPES
    typedef HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR> HashTableType;
    typedef typename HashTableType::AllocatorTraits::
                                template rebind_traits<NodeType> ReboundTraits;
    typedef typename ReboundTraits::allocator_type               NodeAllocator;

    typedef
    BidirectionalNodePool<typename HashTableType::ValueType, NodeAllocator>
                                                                   NodeFactory;

  private:
    // DATA
    NodeFactory  d_nodeFactory;     // nested 'struct's have public data by
                                    // convention, but should always be
                                    // accessed through the public methods.

  private:
    // NOT IMPLEMENTED
    HashTable_ImplParameters(const HashTable_ImplParameters&); // = delete;
    HashTable_ImplParameters& operator=(const HashTable_ImplParameters&);
        // = delete;

    // CONSISTENCY CHECKS

    // Assert consistency checks against Machiavellian users, specializing an
    // allocator for a specific type to have different propagation traits to
    // the primary template.

    BSLMF_ASSERT(
       ReboundTraits::propagate_on_container_copy_assignment::value ==
       HashTableType::AllocatorTraits::
                            propagate_on_container_copy_assignment::value);

    BSLMF_ASSERT(
       ReboundTraits::propagate_on_container_move_assignment::value ==
       HashTableType::AllocatorTraits::
                            propagate_on_container_move_assignment::value);

    BSLMF_ASSERT(
       ReboundTraits::propagate_on_container_swap::value ==
       HashTableType::AllocatorTraits::propagate_on_container_swap::value);

  public:
    // CREATORS
    explicit HashTable_ImplParameters(const ALLOCATOR& allocator);
        // Create a 'HashTable_ImplParameters' object having default
        // constructed 'HASHER' and 'COMPARATOR' functors, and using the
        // specified 'allocator' to provide a 'BidirectionalNodePool'.

    HashTable_ImplParameters(const HASHER&     hash,
                             const COMPARATOR& compare,
                             const ALLOCATOR&  allocator);
        // Create a 'HashTable_ImplParameters' object having the specified
        // 'hash' and 'compare' functors, and using the specified 'allocator'
        // to provide a 'BidirectionalNodePool'.

    HashTable_ImplParameters(const HashTable_ImplParameters& original,
                             const ALLOCATOR&                allocator);
        // Create a 'HashTable_ImplParameters' object having the same 'hasher'
        // and 'comparator' attributes as the specified 'original', and
        // providing a 'BidirectionalNodePool' using the specified 'allocator'.

    // MANIPULATORS
    NodeFactory& nodeFactory();
        // Return a reference offering modifiable access to the 'nodeFactory'
        // owned by this object.

    void quickSwapExchangeAllocators(HashTable_ImplParameters *other);
        // Efficiently exchange the value, functor, and allocator of this
        // object with those of the specified 'other' object.  This method
        // provides the no-throw exception-safety guarantee.

    void quickSwapRetainAllocators(HashTable_ImplParameters *other);
        // Efficiently exchange the value and functors this object with those
        // of the specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const BaseComparator& comparator() const;
        // Return a reference offering non-modifiable access to the
        // 'comparator' functor owned by this object.

    template <class DEDUCED_KEY>
    native_std::size_t hashCodeForKey(DEDUCED_KEY& key) const;
        // Return the hash code for the specified 'key' using a copy of the
        // hash functor supplied at construction.  Note that this function is
        // provided as a common way to resolve 'const_cast' issues in the case
        // that the stored hash functor has a function call operator that is
        // not declared as 'const'.

    const BaseHasher& hasher() const;
        // Return a reference offering non-modifiable access to the 'hasher'
        // functor owned by this object.

    const NodeFactory& nodeFactory() const;
        // Return a reference offering non-modifiable access to the
        // 'nodeFactory' owned by this object.

    const COMPARATOR& originalComparator() const;
        // Return a reference offering non-modifiable access to the
        // 'comparator' functor owned by this object.

    const HASHER& originalHasher() const;
        // Return a reference offering non-modifiable access to the 'hasher'
        // functor owned by this object.
};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ---------------------------
                   // class HashTable_HashWrapper
                   // ---------------------------

template <class FUNCTOR>
inline
HashTable_HashWrapper<FUNCTOR>::HashTable_HashWrapper()
: d_functor()
{
}

template <class FUNCTOR>
inline
HashTable_HashWrapper<FUNCTOR>::HashTable_HashWrapper(const FUNCTOR& fn)
: d_functor(fn)
{
}

template <class FUNCTOR>
template <class ARG_TYPE>
inline
native_std::size_t
HashTable_HashWrapper<FUNCTOR>::operator()(ARG_TYPE& arg) const
{
    return d_functor(arg);
}

template <class FUNCTOR>
inline
const FUNCTOR& HashTable_HashWrapper<FUNCTOR>::functor() const
{
    return d_functor;
}

template <class FUNCTOR>
inline
void HashTable_HashWrapper<FUNCTOR>::swap(HashTable_HashWrapper &other)
{
    bslalg::SwapUtil::swap(
                static_cast<FUNCTOR*>(bsls::Util::addressOf(d_functor)),
                static_cast<FUNCTOR*>(bsls::Util::addressOf(other.d_functor)));
}

                 // 'const FUNCTOR' partial specialization

template <class FUNCTOR>
inline
HashTable_HashWrapper<const FUNCTOR>::HashTable_HashWrapper()
: d_functor()
{
}

template <class FUNCTOR>
inline
HashTable_HashWrapper<const FUNCTOR>::HashTable_HashWrapper(const FUNCTOR& fn)
: d_functor(fn)
{
}

template <class FUNCTOR>
template <class ARG_TYPE>
inline
native_std::size_t
HashTable_HashWrapper<const FUNCTOR>::operator()(ARG_TYPE& arg) const
{
    return d_functor(arg);
}

template <class FUNCTOR>
inline
const FUNCTOR& HashTable_HashWrapper<const FUNCTOR>::functor() const
{
    return d_functor;
}

                 // 'FUNCTOR &' partial specialization

template <class FUNCTOR>
inline
HashTable_HashWrapper<FUNCTOR &>::HashTable_HashWrapper(FUNCTOR& fn)
: d_functor(fn)
{
}

template <class FUNCTOR>
template <class ARG_TYPE>
inline
native_std::size_t
HashTable_HashWrapper<FUNCTOR &>::operator()(ARG_TYPE& arg) const
{
    return d_functor(arg);
}

template <class FUNCTOR>
inline
FUNCTOR& HashTable_HashWrapper<FUNCTOR &>::functor() const
{
    return d_functor;
}

                   // ---------------------------------
                   // class HashTable_ComparatorWrapper
                   // ---------------------------------

template <class FUNCTOR>
inline
HashTable_ComparatorWrapper<FUNCTOR>::HashTable_ComparatorWrapper()
: d_functor()
{
}

template <class FUNCTOR>
inline
HashTable_ComparatorWrapper<FUNCTOR>::
HashTable_ComparatorWrapper(const FUNCTOR& fn)
: d_functor(fn)
{
}

template <class FUNCTOR>
template <class ARG1_TYPE, class ARG2_TYPE>
inline
bool
HashTable_ComparatorWrapper<FUNCTOR>::operator()(ARG1_TYPE& arg1,
                                                 ARG2_TYPE& arg2) const
{
    return d_functor(arg1, arg2);
}

template <class FUNCTOR>
const FUNCTOR& HashTable_ComparatorWrapper<FUNCTOR>::functor() const
{
    return d_functor;
}

template <class FUNCTOR>
inline
void
HashTable_ComparatorWrapper<FUNCTOR>::swap(HashTable_ComparatorWrapper &other)
{
    bslalg::SwapUtil::swap(
                static_cast<FUNCTOR*>(bsls::Util::addressOf(d_functor)),
                static_cast<FUNCTOR*>(bsls::Util::addressOf(other.d_functor)));
}

                 // 'const FUNCTOR' partial specialization


template <class FUNCTOR>
inline
HashTable_ComparatorWrapper<const FUNCTOR>::HashTable_ComparatorWrapper()
: d_functor()
{
}

template <class FUNCTOR>
inline
HashTable_ComparatorWrapper<const FUNCTOR>::
HashTable_ComparatorWrapper(const FUNCTOR& fn)
: d_functor(fn)
{
}

template <class FUNCTOR>
template <class ARG1_TYPE, class ARG2_TYPE>
inline
bool
HashTable_ComparatorWrapper<const FUNCTOR>::operator()(ARG1_TYPE& arg1,
                                                       ARG2_TYPE& arg2) const
{
    return d_functor(arg1, arg2);
}

template <class FUNCTOR>
const FUNCTOR& HashTable_ComparatorWrapper<const FUNCTOR>::functor() const
{
    return d_functor;
}

                 // 'FUNCTOR &' partial specialization

template <class FUNCTOR>
inline
HashTable_ComparatorWrapper<FUNCTOR &>::
HashTable_ComparatorWrapper(FUNCTOR& fn)
: d_functor(fn)
{
}

template <class FUNCTOR>
template <class ARG1_TYPE, class ARG2_TYPE>
inline
bool
HashTable_ComparatorWrapper<FUNCTOR &>::operator()(ARG1_TYPE& arg1,
                                                   ARG2_TYPE& arg2) const
{
    return d_functor(arg1, arg2);
}

template <class FUNCTOR>
inline
FUNCTOR& HashTable_ComparatorWrapper<FUNCTOR &>::functor() const
{
    return d_functor;
}

                    // ---------------------------
                    // class HashTable_NodeProctor
                    // ---------------------------

// CREATORS
template <class FACTORY>
inline
HashTable_NodeProctor<FACTORY>::HashTable_NodeProctor(
                                            FACTORY                   *factory,
                                            bslalg::BidirectionalLink *node)
: d_factory(factory)
, d_node(node)
{
    BSLS_ASSERT_SAFE(factory);
}

template <class FACTORY>
inline
HashTable_NodeProctor<FACTORY>::~HashTable_NodeProctor()
{
    if (d_node) {
        d_factory->deleteNode(d_node);
    }
}

// MANIPULATORS
template <class FACTORY>
inline
void HashTable_NodeProctor<FACTORY>::release()
{
    d_node = 0;
}

                    // ----------------------------
                    // class HashTable_ArrayProctor
                    // ----------------------------

// CREATORS
template <class FACTORY>
inline
HashTable_ArrayProctor<FACTORY>::HashTable_ArrayProctor(
                                              FACTORY                 *factory,
                                              bslalg::HashTableAnchor *anchor)
: d_factory(factory)
, d_anchor(anchor)
{
    BSLS_ASSERT_SAFE(factory);
    BSLS_ASSERT_SAFE(anchor);
}

template <class FACTORY>
inline
HashTable_ArrayProctor<FACTORY>::~HashTable_ArrayProctor()
{
    if (d_anchor) {
        HashTable_Util::destroyBucketArray(d_anchor->bucketArrayAddress(),
                                           d_anchor->bucketArraySize(),
                                           d_factory->allocator());

        bslalg::BidirectionalLink *root = d_anchor->listRootAddress();
        while (root) {
            bslalg::BidirectionalLink *next = root->nextLink();
            d_factory->deleteNode(root);
            root = next;
        }
    }
}

// MANIPULATORS
template <class FACTORY>
inline
void HashTable_ArrayProctor<FACTORY>::release()
{
    d_anchor = 0;
}

                    // --------------------
                    // class HashTable_Util
                    // --------------------

template <class TYPE>
inline
void HashTable_Util::assertNotNullPointer(TYPE&)
{
}

template <class TYPE>
inline
void HashTable_Util::assertNotNullPointer(TYPE * const& ptr)
{
    // silence "unused parameter" warning in release builds:
    (void) ptr;
    BSLS_ASSERT(ptr);
}

template <class TYPE>
inline
void HashTable_Util::assertNotNullPointer(TYPE * & ptr)
{
    BSLS_ASSERT(ptr);
}

template <class ALLOCATOR>
inline
void HashTable_Util::destroyBucketArray(
                                     bslalg::HashTableBucket  *data,
                                     native_std::size_t        bucketArraySize,
                                     const ALLOCATOR&          allocator)
{
    BSLS_ASSERT_SAFE(data);
    BSLS_ASSERT_SAFE(
                  (1  < bucketArraySize
                     && HashTable_ImpDetails::defaultBucketAddress() != data)
               || (1 == bucketArraySize
                     && HashTable_ImpDetails::defaultBucketAddress() == data));

    typedef ::bsl::allocator_traits<ALLOCATOR>               ParamAllocTraits;
    typedef typename ParamAllocTraits::template
                      rebind_traits<bslalg::HashTableBucket> BucketAllocTraits;
    typedef typename BucketAllocTraits::allocator_type       ArrayAllocator;
    typedef ::bsl::allocator_traits<ArrayAllocator>       ArrayAllocatorTraits;
    typedef typename ArrayAllocatorTraits::size_type         SizeType;

    BSLS_ASSERT_SAFE(
               bucketArraySize <= native_std::numeric_limits<SizeType>::max());

    if (HashTable_ImpDetails::defaultBucketAddress() != data) {
        ArrayAllocator reboundAllocator(allocator);
        ArrayAllocatorTraits::deallocate(
                                       reboundAllocator,
                                       data,
                                       static_cast<SizeType>(bucketArraySize));
    }
}

template <class ALLOCATOR>
inline
void HashTable_Util::initAnchor(bslalg::HashTableAnchor *anchor,
                                native_std::size_t       bucketArraySize,
                                const ALLOCATOR&         allocator)
{
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(0 != bucketArraySize);

    typedef ::bsl::allocator_traits<ALLOCATOR>               ParamAllocTraits;
    typedef typename ParamAllocTraits::template
                      rebind_traits<bslalg::HashTableBucket> BucketAllocTraits;
    typedef typename BucketAllocTraits::allocator_type       ArrayAllocator;
    typedef ::bsl::allocator_traits<ArrayAllocator>       ArrayAllocatorTraits;
    typedef typename ArrayAllocatorTraits::size_type         SizeType;

    BSLS_ASSERT_SAFE(
               bucketArraySize <= native_std::numeric_limits<SizeType>::max());

    ArrayAllocator reboundAllocator(allocator);

    // This test is necessary to avoid undefined behavior in the non-standard
    // narrow contract of 'bsl::allocator', although it seems like a reasonable
    // assumption to pre-empt other allocators too.

    if (ArrayAllocatorTraits::max_size(reboundAllocator) < bucketArraySize) {
        bsls::BslExceptionUtil::throwBadAlloc();
    }

    // Conversion to exactly the correct type resolves compiler warnings.  The
    // assertions above are a loose safety check that this conversion can never
    // overflow - which would require an allocator using a 'size_type' larger
    // than 'std::size_t', with the requirement that a standard conforming
    // allocator must use a 'size_type' that is a built-in unsigned integer
    // type.

    const SizeType newArraySize = static_cast<SizeType>(bucketArraySize);

    bslalg::HashTableBucket *data = ArrayAllocatorTraits::allocate(
                                       reboundAllocator,
                                       newArraySize);

    native_std::fill_n(data, bucketArraySize, bslalg::HashTableBucket());

    anchor->setBucketArrayAddressAndSize(data, newArraySize);
}

                //-------------------------------
                // class HashTable_ImplParameters
                //-------------------------------

    // CREATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable_ImplParameters(const ALLOCATOR& allocator)
: BaseHasher()
, BaseComparator()
, d_nodeFactory(allocator)
{
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable_ImplParameters(const HASHER&     hash,
                         const COMPARATOR& compare,
                         const ALLOCATOR&  allocator)
: BaseHasher(hash)
, BaseComparator(compare)
, d_nodeFactory(allocator)
{
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable_ImplParameters(const HashTable_ImplParameters& original,
                         const ALLOCATOR&                allocator)
: BaseHasher(static_cast<const BaseHasher&>(original))
, BaseComparator(static_cast<const BaseComparator&>(original))
, d_nodeFactory(allocator)
{
}

// MANIPULATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable_ImplParameters<KEY_CONFIG,
                                  HASHER,
                                  COMPARATOR,
                                  ALLOCATOR>::NodeFactory &
HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
nodeFactory()
{
    return d_nodeFactory;
}



template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
quickSwapExchangeAllocators(HashTable_ImplParameters *other)
{
    BSLS_ASSERT_SAFE(other);

    bslalg::SwapUtil::swap(static_cast<BaseHasher*>(this),
                           static_cast<BaseHasher*>(other));

    bslalg::SwapUtil::swap(static_cast<BaseComparator*>(this),
                           static_cast<BaseComparator*>(other));

    nodeFactory().swapExchangeAllocators(other->nodeFactory());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
quickSwapRetainAllocators(HashTable_ImplParameters *other)
{
    BSLS_ASSERT_SAFE(other);

    bslalg::SwapUtil::swap(static_cast<BaseHasher*>(this),
                           static_cast<BaseHasher*>(other));

    bslalg::SwapUtil::swap(static_cast<BaseComparator*>(this),
                           static_cast<BaseComparator*>(other));

    nodeFactory().swapRetainAllocators(other->nodeFactory());
}

// ACCESSORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const typename HashTable_ImplParameters<KEY_CONFIG,
                                        HASHER,
                                        COMPARATOR,
                                        ALLOCATOR>::BaseComparator &
HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
comparator() const
{
    return *this;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class DEDUCED_KEY>
inline
native_std::size_t HashTable_ImplParameters<KEY_CONFIG,
                                            HASHER,
                                            COMPARATOR,
                                            ALLOCATOR>::
hashCodeForKey(DEDUCED_KEY& key) const
{
    return static_cast<const BaseHasher &>(*this)(key);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const typename HashTable_ImplParameters<KEY_CONFIG,
                                        HASHER,
                                        COMPARATOR,
                                        ALLOCATOR>::BaseHasher &
HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::hasher()
                                                                         const
{
    return *this;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const typename HashTable_ImplParameters<KEY_CONFIG,
                                        HASHER,
                                        COMPARATOR,
                                        ALLOCATOR>::NodeFactory &
HashTable_ImplParameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
nodeFactory() const
{
    return d_nodeFactory;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const COMPARATOR&
HashTable_ImplParameters<KEY_CONFIG,
                         HASHER,
                         COMPARATOR,
                         ALLOCATOR>::originalComparator() const
{
    return static_cast<const BaseComparator *>(this)->functor();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const HASHER& HashTable_ImplParameters<KEY_CONFIG,
                                       HASHER,
                                       COMPARATOR,
                                       ALLOCATOR>::originalHasher() const
{
    return static_cast<const BaseHasher *>(this)->functor();
}

                        //----------------
                        // class HashTable
                        //----------------

// CREATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const ALLOCATOR& basicAllocator)
: d_parameters(basicAllocator)
, d_anchor(HashTable_ImpDetails::defaultBucketAddress(), 1, 0)
, d_size()
, d_capacity()
, d_maxLoadFactor(1.0)
{
    BSLMF_ASSERT(!bsl::is_pointer<HASHER>::value &&
                 !bsl::is_pointer<COMPARATOR>::value);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const HASHER&     hash,
          const COMPARATOR& compare,
          SizeType          initialNumBuckets,
          float             initialMaxLoadFactor,
          const ALLOCATOR&  basicAllocator)
: d_parameters(hash, compare, basicAllocator)
, d_anchor(HashTable_ImpDetails::defaultBucketAddress(), 1, 0)
, d_size()
, d_capacity(0)
, d_maxLoadFactor(initialMaxLoadFactor)
{
    BSLS_ASSERT_SAFE(0.0f < initialMaxLoadFactor);

    if (bsl::is_pointer<HASHER>::value) {
        HashTable_Util::assertNotNullPointer(hash);
    }
    if (bsl::is_pointer<COMPARATOR>::value) {
        HashTable_Util::assertNotNullPointer(compare);
    }

    if (0 != initialNumBuckets) {
        size_t capacity;  // This may be a different type than SizeType.
        size_t numBuckets = HashTable_ImpDetails::growBucketsForLoadFactor(
                                        &capacity,
                                        1,
                                        static_cast<size_t>(initialNumBuckets),
                                        d_maxLoadFactor);
        HashTable_Util::initAnchor(&d_anchor, numBuckets, basicAllocator);
        d_capacity = static_cast<SizeType>(capacity);
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const HashTable& original)
: d_parameters(
  original.d_parameters,
  AllocatorTraits::select_on_container_copy_construction(original.allocator()))
, d_anchor(HashTable_ImpDetails::defaultBucketAddress(), 1, 0)
, d_size(original.d_size)
, d_capacity(0)
, d_maxLoadFactor(original.d_maxLoadFactor)
{
    if (0 < d_size) {
        d_parameters.nodeFactory().reserveNodes(original.d_size);
        this->copyDataStructure(original.d_anchor.listRootAddress());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const HashTable& original, const ALLOCATOR& allocator)
: d_parameters(original.d_parameters, allocator)
, d_anchor(HashTable_ImpDetails::defaultBucketAddress(), 1, 0)
, d_size(original.d_size)
, d_capacity(0)
, d_maxLoadFactor(original.d_maxLoadFactor)
{
    if (0 < d_size) {
        d_parameters.nodeFactory().reserveNodes(original.d_size);
        this->copyDataStructure(original.d_anchor.listRootAddress());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::~HashTable()
{
#if defined(BDE_BUILD_TARGET_SAFE_2)
    // ASSERT class invariant only in SAFE_2 builds.  Note that we specifically
    // use the MallocFree allocator, rather than allowing the default allocator
    // to supply memory to this state-checking function, in case the object
    // allocator *is* the default allocator, and so may be restricted during
    // testing.  This would cause the test below to fail by throwing a bad
    // allocation exception, and so result in a throwing destructor.  While the
    // MallocFree allocator might also run out of resources, that is not the
    // kind of catastrophic failure we are concerned with handling in an
    // invariant check that runs only in SAFE_2 builds from a destructor.

    BSLS_ASSERT_SAFE(bslalg::HashTableImpUtil::isWellFormed<KEY_CONFIG>(
                                 this->d_anchor,
                                 this->d_parameters.hasher(),
                                 HashTable_ImpDetails::incidentalAllocator()));
#endif

    this->removeAllAndDeallocate();
}

// PRIVATE MANIPULATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::copyDataStructure(
                                             bslalg::BidirectionalLink *cursor)
{
    BSLS_ASSERT(0 != cursor);
    BSLS_ASSERT(0 < d_size);

    // This function will completely replace 'this->d_anchor's state.  It is
    // the caller's responsibility to ensure this will not leak resources owned
    // only by the previous state, such as the linked list.

    // Allocate an appropriate number of buckets

    size_t capacity;
    size_t numBuckets = HashTable_ImpDetails::growBucketsForLoadFactor(
                                                   &capacity,
                                                   static_cast<size_t>(d_size),
                                                   2,
                                                   d_maxLoadFactor);

    d_anchor.setListRootAddress(0);
    HashTable_Util::initAnchor(&d_anchor, numBuckets, this->allocator());

    // create a proctor for d_anchor's allocated array, and the list to follow.

    HashTable_ArrayProctor<typename ImplParameters::NodeFactory>
                          arrayProctor(&d_parameters.nodeFactory(), &d_anchor);

    d_capacity = static_cast<SizeType>(capacity);

    do {
        // Computing hash code depends on user-supplied code, and may throw.
        // Therefore, obtain the hash code from the node we are about to copy,
        // before any memory is allocated, so there is no risk of leaking an
        // object.  The hash code must be the same for both elements.

        size_t hashCode = this->hashCodeForNode(cursor);
        bslalg::BidirectionalLink *newNode =
                                 d_parameters.nodeFactory().cloneNode(*cursor);

        bslalg::HashTableImpUtil::insertAtBackOfBucket(&d_anchor,
                                                       newNode,
                                                       hashCode);
    }
    while (0 != (cursor = cursor->nextLink()));

    // release the proctor

    arrayProctor.release();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
quickSwapExchangeAllocators(HashTable *other)
{
    BSLS_ASSERT_SAFE(other);

    d_parameters.quickSwapExchangeAllocators(&other->d_parameters);

    bslalg::SwapUtil::swap(&d_anchor,        &other->d_anchor);
    bslalg::SwapUtil::swap(&d_size,          &other->d_size);
    bslalg::SwapUtil::swap(&d_capacity,      &other->d_capacity);
    bslalg::SwapUtil::swap(&d_maxLoadFactor, &other->d_maxLoadFactor);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
quickSwapRetainAllocators(HashTable *other)
{
    BSLS_ASSERT_SAFE(other);
    BSLS_ASSERT_SAFE(this->allocator() == other->allocator());

    d_parameters.quickSwapRetainAllocators(&other->d_parameters);

    bslalg::SwapUtil::swap(&d_anchor,        &other->d_anchor);
    bslalg::SwapUtil::swap(&d_size,          &other->d_size);
    bslalg::SwapUtil::swap(&d_capacity,      &other->d_capacity);
    bslalg::SwapUtil::swap(&d_maxLoadFactor, &other->d_maxLoadFactor);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
rehashIntoExactlyNumBuckets(SizeType newNumBuckets, SizeType capacity)
{
    class Proctor {
        // An object of this proctor class guarantees that, if an exception is
        // thrown by a user-supplied hash functor, the container remains in a
        // valid, usable (but unspecified) state.  In fact, that state will be
        // empty, as there is no reliable way to re-index a bucket array if the
        // hash functor is throwing, and the array is potentially corrupted
        // following a failed ImpUtil::rehash call.

      private:
        HashTable               *d_this;
        bslalg::HashTableAnchor *d_originalAnchor;
        bslalg::HashTableAnchor *d_newAnchor;

#if !defined(BSLS_PLATFORM_CMP_MSVC)
        // Microsoft warns if these methods are declared private.

      private:
        // NOT IMPLEMENTED
        Proctor(const Proctor&); // = delete;
        Proctor& operator=(const Proctor&); // = delete;
#endif

      public:
        // CREATORS
        Proctor(HashTable               *table,
                bslalg::HashTableAnchor *originalAnchor,
                bslalg::HashTableAnchor *newAnchor)
        : d_this(table)
        , d_originalAnchor(originalAnchor)
        , d_newAnchor(newAnchor)
        {
            BSLS_ASSERT_SAFE(table);
            BSLS_ASSERT_SAFE(originalAnchor);
            BSLS_ASSERT_SAFE(newAnchor);
        }

        ~Proctor()
        {
            if (d_originalAnchor) {
                // Not dismissed, and the newAnchor now holds the correct
                // list-root.

                d_originalAnchor->setListRootAddress(
                                               d_newAnchor->listRootAddress());
                d_this->removeAll();
            }

            // Always destroy the spare anchor's bucket array at the end of
            // scope.  On a non-exceptional run, this will effectively be the
            // original bucket-array, as the anchors are swapped.

            HashTable_Util::destroyBucketArray(
                                             d_newAnchor->bucketArrayAddress(),
                                             d_newAnchor->bucketArraySize(),
                                             d_this->allocator());
        }

        // MANIPULATORS
        void dismiss()
        {
            d_originalAnchor = 0;
        }
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Now that 'anchor' is not default constructible, we take a copy of the
    // anchor in the table.  Would it be better for 'initAnchor' to be replaced
    // with a 'createArrayOfEmptyBuckets' function, and we use the result to
    // construct the 'newAnchor'?

    bslalg::HashTableAnchor newAnchor(0, 0, 0);
    HashTable_Util::initAnchor(&newAnchor,
                               static_cast<size_t>(newNumBuckets),
                               this->allocator());

    Proctor cleanUpIfUserHashThrows(this, &d_anchor, &newAnchor);

    if (d_anchor.listRootAddress()) {
        bslalg::HashTableImpUtil::rehash<KEY_CONFIG>(
                                          &newAnchor,
                                          this->d_anchor.listRootAddress(),
                                          this->d_parameters.hasher());
    }

    cleanUpIfUserHashThrows.dismiss();

    d_anchor.swap(newAnchor);
    d_capacity = capacity;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::removeAllAndDeallocate()
{
    this->removeAllImp();
    HashTable_Util::destroyBucketArray(d_anchor.bucketArrayAddress(),
                                       d_anchor.bucketArraySize(),
                                       this->allocator());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::removeAllImp()
{
    typedef bslalg::BidirectionalLink BidirectionalLink;

    // Doing too much book-keeping of hash table - look for a more efficient
    // dispose-as-we-walk, that simply resets table.Anchor.next = 0, and
    // assigns the buckets index all null pointers

    if (BidirectionalLink *root = d_anchor.listRootAddress()) {
        BidirectionalLink *next;
        do {
            next = root->nextLink();
            d_parameters.nodeFactory().deleteNode(
                                                static_cast<NodeType *>(root));
        }
        while(0 != (root = next));
    }
}

// PRIVATE ACCESSORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class DEDUCED_KEY>
inline
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::find(
                                            DEDUCED_KEY&       key,
                                            native_std::size_t hashValue) const
{
    return bslalg::HashTableImpUtil::find<KEY_CONFIG>(
                                                     d_anchor,
                                                     key,
                                                     d_parameters.comparator(),
                                                     hashValue);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::HashTableBucket *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::getBucketAddress(
                                                    SizeType bucketIndex) const
{
    BSLS_ASSERT_SAFE(bucketIndex < this->numBuckets());

    return d_anchor.bucketArrayAddress() + bucketIndex;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
native_std::size_t
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::hashCodeForNode(
                                         bslalg::BidirectionalLink *node) const
{
    BSLS_ASSERT_SAFE(node);

    return d_parameters.hashCodeForKey(
                       bslalg::HashTableImpUtil::extractKey<KEY_CONFIG>(node));
}

// MANIPULATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::operator=(
                                                          const HashTable& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {

        if (AllocatorTraits::propagate_on_container_copy_assignment::VALUE) {
            HashTable other(rhs, rhs.allocator());
            quickSwapExchangeAllocators(&other);
        }
        else {
            HashTable other(rhs, this->allocator());
            quickSwapRetainAllocators(&other);
        }
    }
    return *this;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insert(
                                                      const SOURCE_TYPE& value)
{
    typedef bslalg::HashTableImpUtil ImpUtil;

    // Rehash (if appropriate) first as it will reduce load factor and so
    // potentially improve the 'find' time.

    if (d_size >= d_capacity) {
        this->rehashForNumBuckets(numBuckets() * 2);
    }

    // Create a node having the new 'value' we want to insert into the table.
    // We can extract the 'key' from this value without accidentally creating a
    // temporary (using the default allocator for any dynamic memory).

    bslalg::BidirectionalLink *newNode =
                                  d_parameters.nodeFactory().createNode(value);

    // This node needs wrapping in a proctor, in case either of the user-
    // supplied functors throws an exception.

    HashTable_NodeProctor<typename ImplParameters::NodeFactory>
                             nodeProctor(&d_parameters.nodeFactory(), newNode);

    // Now we can search for the node in the table, being careful to compute
    // the hash value only once.

    size_t hashCode = this->d_parameters.hashCodeForKey(
                                     ImpUtil::extractKey<KEY_CONFIG>(newNode));
    bslalg::BidirectionalLink *position = this->find(
                                      ImpUtil::extractKey<KEY_CONFIG>(newNode),
                                      hashCode);

    if (!position) {
        ImpUtil::insertAtFrontOfBucket(&d_anchor, newNode, hashCode);
    }
    else {
        ImpUtil::insertAtPosition(&d_anchor, newNode, hashCode, position);
    }
    nodeProctor.release();

    ++d_size;

    return newNode;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insert(
                                              const SOURCE_TYPE&         value,
                                              bslalg::BidirectionalLink *hint)
{
    BSLS_ASSERT(hint);

    typedef bslalg::HashTableImpUtil ImpUtil;

    // Rehash (if appropriate) first as it will reduce load factor and so
    // potentially improve the potential 'find' time later.

    if (d_size >= d_capacity) {
        this->rehashForNumBuckets(numBuckets() * 2);
    }

    // Next we must create the node, to avoid making a temporary of 'ValueType'
    // from the object of template parameter 'SOURCE_TYPE'.

    bslalg::BidirectionalLink *newNode =
                                  d_parameters.nodeFactory().createNode(value);

    // There is potential for the user-supplied hasher and comparator to throw,
    // so now we need to manage our 'newNode' with a proctor.

    HashTable_NodeProctor<typename ImplParameters::NodeFactory>
                             nodeProctor(&d_parameters.nodeFactory(), newNode);

    // Insert logic, first test the hint

    size_t hashCode = this->d_parameters.hashCodeForKey(
                                     ImpUtil::extractKey<KEY_CONFIG>(newNode));
    if (!d_parameters.comparator()(ImpUtil::extractKey<KEY_CONFIG>(newNode),
                                   ImpUtil::extractKey<KEY_CONFIG>(hint))) {
        hint = this->find(ImpUtil::extractKey<KEY_CONFIG>(newNode), hashCode);
    }

    if (!hint) {
        ImpUtil::insertAtFrontOfBucket(&d_anchor, newNode, hashCode);
    }
    else {
        ImpUtil::insertAtPosition(&d_anchor, newNode, hashCode, hint);
    }
    nodeProctor.release();

    ++d_size;

    return newNode;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insertIfMissing(
                                              bool             *isInsertedFlag,
                                              const ValueType&  value)
{
    BSLS_ASSERT(isInsertedFlag);

    size_t hashCode = this->d_parameters.hashCodeForKey(
                                                KEY_CONFIG::extractKey(value));
    bslalg::BidirectionalLink *position = this->find(
                                                 KEY_CONFIG::extractKey(value),
                                                 hashCode);

    *isInsertedFlag = (!position);

    if(!position) {
        if (d_size >= d_capacity) {
            this->rehashForNumBuckets(numBuckets() * 2);
        }

        position = d_parameters.nodeFactory().createNode(value);
        bslalg::HashTableImpUtil::insertAtFrontOfBucket(&d_anchor,
                                                        position,
                                                        hashCode);
        ++d_size;
    }

    return position;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insertIfMissing(
                                            bool               *isInsertedFlag,
                                            const SOURCE_TYPE&  value)
{
    BSLS_ASSERT(isInsertedFlag);

    typedef bslalg::HashTableImpUtil ImpUtil;

    // Rehash (if appropriate) first as it will reduce load factor and so
    // potentially improve the potential 'find' time later.

    if (d_size >= d_capacity) {
        this->rehashForNumBuckets(numBuckets() * 2);
    }

    // Next we must create the node, to avoid making a temporary of 'ValueType'
    // from the object of template parameter 'SOURCE_TYPE'.

    bslalg::BidirectionalLink *newNode =
                                  d_parameters.nodeFactory().createNode(value);

    // There is potential for the user-supplied hasher and comparator to throw,
    // so now we need to manage our 'newNode' with a proctor.

    HashTable_NodeProctor<typename ImplParameters::NodeFactory>
                             nodeProctor(&d_parameters.nodeFactory(), newNode);

    // Insert logic, first test the hint

    size_t hashCode = this->d_parameters.hashCodeForKey(
                                     ImpUtil::extractKey<KEY_CONFIG>(newNode));
    bslalg::BidirectionalLink *position = this->find(
                                      ImpUtil::extractKey<KEY_CONFIG>(newNode),
                                      hashCode);

    *isInsertedFlag = (!position);

    if(!position) {
        if (d_size >= d_capacity) {
            this->rehashForNumBuckets(numBuckets() * 2);
        }

        ImpUtil::insertAtFrontOfBucket(&d_anchor, newNode, hashCode);
        nodeProctor.release();

        ++d_size;
        position = newNode;
    }

    return position;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insertIfMissing(
                                                            const KeyType& key)
{
    size_t hashCode = this->d_parameters.hashCodeForKey(key);
    bslalg::BidirectionalLink *position = this->find(key, hashCode);
    if (!position) {
        if (d_size >= d_capacity) {
            this->rehashForNumBuckets(numBuckets() * 2);
        }

        position = d_parameters.nodeFactory().createNode(
                                            key,
                                            typename ValueType::second_type());

        bslalg::HashTableImpUtil::insertAtFrontOfBucket(&d_anchor,
                                                        position,
                                                        hashCode);
        ++d_size;
    }
    return position;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::rehashForNumBuckets(
                                                        SizeType newNumBuckets)
{
    if (newNumBuckets > this->numBuckets()) {
        // Compute a "good" number of buckets, e.g., pick a prime number from a
        // sorted array of exponentially increasing primes.

        size_t capacity;
        SizeType numBuckets = static_cast<SizeType>(
                              HashTable_ImpDetails::growBucketsForLoadFactor(
                                            &capacity,
                                            d_size + 1u,
                                            static_cast<size_t>(newNumBuckets),
                                            d_maxLoadFactor));

        this->rehashIntoExactlyNumBuckets(numBuckets,
                                          static_cast<SizeType>(capacity));
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::remove(
                                               bslalg::BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(node);
    BSLS_ASSERT_SAFE(node->previousLink()
                  || d_anchor.listRootAddress() == node);

    bslalg::BidirectionalLink *result = node->nextLink();

    bslalg::HashTableImpUtil::remove(&d_anchor,
                                     node,
                                     hashCodeForNode(node));
    --d_size;

    d_parameters.nodeFactory().deleteNode(static_cast<NodeType *>(node));

    return result;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::removeAll()
{
    this->removeAllImp();
    native_std::memset(
                 d_anchor.bucketArrayAddress(),
                 0,
                 sizeof(bslalg::HashTableBucket) * d_anchor.bucketArraySize());

    d_anchor.setListRootAddress(0);
    d_size = 0;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::reserveForNumElements(
                                                          SizeType numElements)
{
    if (numElements < 1) { // Return avoids undefined behavior in node factory.
        return;                                                       // RETURN
    }

    if (numElements > d_capacity) {
        // Compute a "good" number of buckets, e.g., pick a prime number from a
        // sorted array of exponentially increasing primes.

        size_t capacity;
        SizeType numBuckets = static_cast<SizeType>(
                              HashTable_ImpDetails::growBucketsForLoadFactor(
                                       &capacity,
                                       numElements,
                                       static_cast<size_t>(this->numBuckets()),
                                       d_maxLoadFactor));

        this->rehashIntoExactlyNumBuckets(numBuckets,
                                          static_cast<SizeType>(capacity));
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::setMaxLoadFactor(
                                                        float newMaxLoadFactor)
{
    BSLS_ASSERT_SAFE(0.0f < newMaxLoadFactor);

    size_t capacity;
    SizeType numBuckets = static_cast<SizeType>(
             HashTable_ImpDetails::growBucketsForLoadFactor(
                                       &capacity,
                                       native_std::max<SizeType>(d_size, 1u),
                                       static_cast<size_t>(this->numBuckets()),
                                       newMaxLoadFactor));

    this->rehashIntoExactlyNumBuckets(numBuckets,
                                      static_cast<SizeType>(capacity));

    // Always set this last, as there is potential to throw exceptions above.

    d_maxLoadFactor = newMaxLoadFactor;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::swap(HashTable& other)
{
    // This trait should perform 'if' at compile-time.

    if (AllocatorTraits::propagate_on_container_swap::VALUE) {
        quickSwapExchangeAllocators(&other);
    }
    else {
        // C++11 behavior: undefined for unequal allocators
        // BSLS_ASSERT(allocator() == other.allocator());

        BSLS_ASSERT(d_parameters.nodeFactory().allocator() ==
                    other.d_parameters.nodeFactory().allocator());
        quickSwapRetainAllocators(&other);
    }
}

// ACCESSORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
ALLOCATOR HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
                                                              allocator() const
{
    return d_parameters.nodeFactory().allocator();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const bslalg::HashTableBucket&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::bucketAtIndex(
                                                          SizeType index) const
{
    BSLS_ASSERT_SAFE(index < this->numBuckets());

    return d_anchor.bucketArrayAddress()[index];
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::bucketIndexForKey(
                                                      const KeyType& key) const
{
    typedef typename
       HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType SizeType;

    // The following cast will not discard any useful bits, unless 'SizeType'
    // is larger than 'size_t', as the bucket computation takes a mod on the
    // supplied number of buckets.  We use the following 'BSLMF_ASSERT' to
    // assert that assumption at compile time.

    BSLMF_ASSERT(sizeof(SizeType) <= sizeof(size_t));

    size_t hashCode = this->d_parameters.hashCodeForKey(key);
    return static_cast<SizeType>(bslalg::HashTableImpUtil::computeBucketIndex(
                                                  hashCode,
                                                  d_anchor.bucketArraySize()));
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const COMPARATOR&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::comparator() const
{
    return d_parameters.originalComparator();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::countElementsInBucket(
                                                          SizeType index) const
{
    BSLS_ASSERT_SAFE(index < this->numBuckets());

    return static_cast<SizeType>(bucketAtIndex(index).countElements());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::elementListRoot() const
{
    return d_anchor.listRootAddress();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::find(
                                                      const KeyType& key) const
{
    return bslalg::HashTableImpUtil::find<KEY_CONFIG>(
                                             d_anchor,
                                             key,
                                             d_parameters.comparator(),
                                             d_parameters.hashCodeForKey(key));
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::findEndOfRange(
                                        bslalg::BidirectionalLink *first) const
{
    BSLS_ASSERT_SAFE(first);

    typedef bslalg::HashTableImpUtil ImpUtil;

    // The reference to the Key passed to the functor is only optionally
    // const-qualified.  We must be sure to hold a reference with the correct
    // qualification.

    typedef
           typename bslalg::HashTableImpUtil_ExtractKeyResult<KEY_CONFIG>::Type
                                                                        KeyRef;
    KeyRef k = ImpUtil::extractKey<KEY_CONFIG>(first);

    while (0 != (first = first->nextLink()) &&
           d_parameters.comparator()(k,ImpUtil::extractKey<KEY_CONFIG>(first)))
    {
        // This loop body is intentionally left blank.
    }
    return first;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable< KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::findRange(
                                         bslalg::BidirectionalLink **first,
                                         bslalg::BidirectionalLink **last,
                                         const KeyType&              key) const
{
    BSLS_ASSERT_SAFE(first);
    BSLS_ASSERT_SAFE(last);

    *first = this->find(key);
    *last  = *first
           ? this->findEndOfRange(*first)
           : 0;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bool
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::hasSameValue(
                                                  const HashTable& other) const
{
    // TBD: The template bloat of this function can be significantly reduced.
    //..
    // What matters is that the two hash tables:
    // i/   are the same size
    // ii/  have lists that are permutations of each other according to the
    //      element's 'operator=='
    // This means that the implementation should be independent of all four
    // template parameters, but will depend on VALUE_TYPE deduced from the
    // KEY_CONFIG.  Otherwise, after the initial size comparison, the rest
    // depends only on the anchors.
    //..

    typedef typename KEY_CONFIG::ValueType ValueType;
    typedef typename ::bsl::allocator_traits<ALLOCATOR>::size_type SizeType;
    typedef bslalg::HashTableImpUtil ImpUtil;


    // First test - are the containers the same size?

    if (this->size() != other.size()) {
        return false;                                                 // RETURN
    }
    bslalg::BidirectionalLink *cursor = this->elementListRoot();
    if (!cursor) {  // containers are the same size, and empty.
        return true;                                                  // RETURN
    }

    while (cursor) {
        bslalg::BidirectionalLink *rhsFirst =
             ImpUtil::find<KEY_CONFIG>(other.d_anchor,
                                       ImpUtil::extractKey<KEY_CONFIG>(cursor),
                                       other.d_parameters.comparator(),
                                       other.d_parameters.hashCodeForKey(
                                     ImpUtil::extractKey<KEY_CONFIG>(cursor)));
        if (!rhsFirst) {
            return false;  // no matching key                         // RETURN
        }

        bslalg::BidirectionalLink *endRange = this->findEndOfRange(cursor);
        bslalg::BidirectionalLink *rhsLast  = other.findEndOfRange(rhsFirst);

        // Check the key-groups have the same length - a quick-fail test.

        bslalg::BidirectionalLink *endWalker = cursor->nextLink();
        bslalg::BidirectionalLink *rhsWalker = rhsFirst->nextLink();

        while (endWalker != endRange) {


            if (rhsWalker == rhsLast) {
                return false;   // different length subsequences      // RETURN
            }
            endWalker = endWalker->nextLink();
            rhsWalker = rhsWalker->nextLink();
        }

        if (rhsWalker != rhsLast) {
            return false;   // different length subsequences          // RETURN
        }

        // Efficiently compare identical prefixes: O(N) if sequences have the
        // same elements in the same order.  Note that comparison of values in
        // nodes is tested using 'operator==' and not the key-equality
        // comparator stored in the hash table.

        while (cursor != endRange &&
                 (ImpUtil::extractValue<KEY_CONFIG>(cursor) ==
                  ImpUtil::extractValue<KEY_CONFIG>(rhsFirst)))
        {
            cursor   = cursor->nextLink();
            rhsFirst = rhsFirst->nextLink();
        }

        if (cursor == endRange) {
            continue;
        }


        // Now comes the harder part of validating that one subsequence is a
        // permutation of another, by counting elements that compare equal
        // using the equality operator, 'operator=='.  Note that this code
        // could be simplified for hash-tables with unique keys, as we can omit
        // the counting-scan, and merely test for any match within the 'other'
        // range.  Trade off the ease of a single well-tested code path, vs.
        // the importance of an efficient 'operator==' for hash containers.
        // This is currently the only place the hash-table would care about
        // uniqueness, and risk different hash-table types for unique- vs.
        // multi-containers.  Note again that comparison of values in nodes is
        // tested using 'operator==' and not the key-equality comparator stored
        // in the hash tables.

        for (bslalg::BidirectionalLink *marker = cursor;
             marker != endRange;
             marker = marker->nextLink())
        {
            const ValueType& valueAtMarker =
                                    ImpUtil::extractValue<KEY_CONFIG>(marker);

            if (cursor != marker) {  // skip on first pass only
                // Check if the value at 'marker' has already be seen.

                bslalg::BidirectionalLink *scanner = cursor;
                while (scanner != marker &&
                 ImpUtil::extractValue<KEY_CONFIG>(scanner) != valueAtMarker) {
                    scanner = scanner->nextLink();
                }
                if (scanner != marker) {  // We have seen 'lhs' one before.
                    continue;
                }
            }

            SizeType matches = 0;
            for (bslalg::BidirectionalLink *scanner = rhsFirst;
                 scanner != rhsLast;
                 scanner = scanner->nextLink()) {
                if (ImpUtil::extractValue<KEY_CONFIG>(scanner) ==
                                                               valueAtMarker) {
                    ++matches;
                }
            }
            if (!matches) {
                return false;                                         // RETURN
            }

            // Remember, *scanner is by definition a good match

            for (bslalg::BidirectionalLink *scanner = marker->nextLink();
                 scanner != endRange;
                 scanner = scanner->nextLink()) {

                if (ImpUtil::extractValue<KEY_CONFIG>(scanner) ==
                                                               valueAtMarker) {
                    if (!--matches) {  // equal matches, but excluding initial
                        return false;                                 // RETURN
                    }
                }
            }
            if (1 != matches) {
                return false;                                         // RETURN
            }
        }
        cursor = endRange;
    }
    return true;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const HASHER&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::hasher() const
{
    return d_parameters.originalHasher();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
float HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::loadFactor() const
{
    return static_cast<float>(static_cast<double>(this->size())
                                    / static_cast<double>(this->numBuckets()));
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
float
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::maxLoadFactor() const
{
    return d_maxLoadFactor;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::maxNumBuckets() const
{
    // This estimate is still on the high side, we should actually pick the
    // preceding entry from our table of prime numbers used for valid bucket
    // array sizes.  There is no easy way to find that value at the moment
    // though.

    typedef typename AllocatorTraits::
                                template rebind_traits<bslalg::HashTableBucket>
                                                         BucketAllocatorTraits;
    typedef typename BucketAllocatorTraits::allocator_type BucketAllocator;

    return BucketAllocatorTraits::max_size(BucketAllocator(this->allocator()));
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::maxSize() const
{
    return AllocatorTraits::max_size(this->allocator()) / sizeof(NodeType);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::numBuckets() const
{
    return static_cast<SizeType>(d_anchor.bucketArraySize());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::rehashThreshold() const
{
    return d_capacity;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::size() const
{
    return d_size;
}

}  // close package namespace

//-----------------------------------------------------------------------------
//                      free functions and operators
//-----------------------------------------------------------------------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
bslstl::swap(bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& a,
             bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& b)
{
    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>
                                                                     TableType;

    if (::bsl::allocator_traits<ALLOCATOR>::propagate_on_container_swap::VALUE
        || a.allocator() == b.allocator()) {
        a.swap(b);
    }
    else {
        // C++11 behavior: undefined for unequal allocators
        // BSLS_ASSERT(allocator() == other.allocator());

        TableType aCopy(a, b.allocator());
        TableType bCopy(b, a.allocator());

        b.swap(aCopy);
        a.swap(bCopy);
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bool bslstl::operator==(
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& lhs,
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& rhs)
{
    return lhs.hasSameValue(rhs);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bool bslstl::operator!=(
         const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& a,
         const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& b)
{
    return !(a == b);
}

template <class FUNCTOR>
inline
void bslstl::swap(bslstl::HashTable_HashWrapper<FUNCTOR> &a,
                  bslstl::HashTable_HashWrapper<FUNCTOR> &b)
{
    a.swap(b);
}

template <class FUNCTOR>
inline
void bslstl::swap(bslstl::HashTable_ComparatorWrapper<FUNCTOR> &a,
                  bslstl::HashTable_ComparatorWrapper<FUNCTOR> &b)
{
    a.swap(b);
}

// ============================================================================
//                              TYPE TRAITS
// ============================================================================

// Type traits for HashTable:
//: o A HashTable is bitwise movable if the both functors and the allocator are
//:     bitwise movable.
//: o A HashTable uses 'bslma' allocators if the parameterized 'ALLOCATOR' is
//:     convertible from 'bslma::Allocator*'.

namespace bslma
{

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
struct UsesBslmaAllocator<bslstl::HashTable<KEY_CONFIG,
                                            HASHER,
                                            COMPARATOR,
                                            ALLOCATOR> >
    : bsl::is_convertible<Allocator*, ALLOCATOR>::type {
};

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
struct UsesBslmaAllocator<bslstl::HashTable_ImplParameters<KEY_CONFIG,
                                                             HASHER,
                                                             COMPARATOR,
                                                             ALLOCATOR> >
    : bsl::is_convertible<Allocator*, ALLOCATOR>::type {
};

}  // close namespace bslma

namespace bslmf
{

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
struct IsBitwiseMoveable<bslstl::HashTable<KEY_CONFIG,
                                           HASHER,
                                           COMPARATOR,
                                           ALLOCATOR> >
: bsl::integral_constant< bool, bslmf::IsBitwiseMoveable<HASHER>::value
                             && bslmf::IsBitwiseMoveable<COMPARATOR>::value
                             && bslmf::IsBitwiseMoveable<ALLOCATOR>::value>
{};

}  // close namespace bslmf
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
