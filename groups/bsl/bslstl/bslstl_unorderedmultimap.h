// bslstl_unorderedmultimap.h                                         -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMULTIMAP
#define INCLUDED_BSLSTL_UNORDEREDMULTIMAP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant 'unordered_multimap' container.
//
//@CLASSES:
//   bsl::unordered_multimap : hashed-map container
//
//@CANONICAL_HEADER: bsl_unordered_map.h
//
//@SEE_ALSO: package bos+stdhdrs in the bos package group
//
//@DESCRIPTION: This component defines a single class template,
// 'bsl::unordered_multimap', implementing the standard container holding a
// collection of (possibly equivalent) keys, each mapped to an associated value
// (with minimal guarantees on ordering).
//
// An instantiation of 'unordered_multimap' is an allocator-aware,
// value-semantic type whose salient attributes are its size (number of keys)
// and the set of key-value pairs the 'unordered_multimap' contains, without
// regard to their order.  If 'unordered_multimap' is instantiated with a key
// type or mapped-value type that is not itself value-semantic, then it will
// not retain all of its value-semantic qualities.  In particular, if ether the
// key or value type cannot be tested for equality, then an
// 'unordered_multimap' containing that type cannot be tested for equality.  It
// is even possible to instantiate 'unordered_multimap' with a key or value
// type that does not have an accessible copy-constructor, in which case the
// 'unordered_multimap' will not be copyable.  Note that the equality operator
// for each key-value pair is used to determine when two 'unordered_multimap'
// objects have the same value, and not the object of 'EQUAL' type supplied at
// construction.
//
// An 'unordered_multimap' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [23.2.5].  The
// 'unordered_multimap' implemented here adheres to the C++11 standard when
// compiled with a C++11 compiler, and makes the best approximation when
// compiled with a C++03 compiler.  In particular, for C++03 we emulate move
// semantics, but limit forwarding (in 'emplace') to 'const' lvalues, and make
// no effort to emulate 'noexcept' or initializer-lists.
//
///Requirements on 'KEY' and 'VALUE'
///---------------------------------
// An 'unordered_multimap' is a fully Value-Semantic Type (see
// {'bsldoc_glossary'}) only if the supplied 'KEY' and 'VALUE' template
// parameters are themselves fully value-semantic.  It is possible to
// instantiate an 'unordered_multimap' with 'KEY' and 'VALUE' parameter
// arguments that do not provide a full set of value-semantic operations, but
// then some methods of the container may not be instantiable.  The following
// terminology, adopted from the C++11 standard, is used in the function
// documentation of 'unordered_multimap' to describe a function's requirements
// for the 'KEY' and 'VALUE' template parameters.  These terms are also defined
// in section [17.6.3.1] of the C++11 standard.  Note that, in the context of
// an 'unordered_multimap' instantiation, the requirements apply specifically
// to the unordered multimap's element type, 'value_type', which is an alias
// for 'pair<const KEY, VALUE>'.
//
// Legend
// ------
// 'X'    - denotes an allocator-aware container type ('unordered_multimap')
// 'T'    - 'value_type' associated with 'X'
// 'A'    - type of the allocator used by 'X'
// 'm'    - lvalue of type 'A' (allocator)
// 'p',   - address ('T *') of uninitialized storage for a 'T' within an 'X'
// 'rv'   - rvalue of type (non-'const') 'T'
// 'v'    - rvalue or lvalue of type (possibly 'const') 'T'
// 'args' - 0 or more arguments
//
// The following terms are used to more precisely specify the requirements on
// template parameter types in function-level documentation.
//:
//: *default-insertable*: 'T' has a default constructor.  More precisely, 'T'
//:     is 'default-insertable' into 'X' means that the following expression is
//:     well-formed:
//:
//:      'allocator_traits<A>::construct(m, p)'
//:
//: *move-insertable*: 'T' provides a constructor that takes an rvalue of type
//:     (non-'const') 'T'.  More precisely, 'T' is 'move-insertable' into 'X'
//:     means that the following expression is well-formed:
//:
//:      'allocator_traits<A>::construct(m, p, rv)'
//:
//: *copy-insertable*: 'T' provides a constructor that takes an lvalue or
//:     rvalue of type (possibly 'const') 'T'.  More precisely, 'T' is
//:     'copy-insertable' into 'X' means that the following expression is
//:     well-formed:
//:
//:      'allocator_traits<A>::construct(m, p, v)'
//:
//: *move-assignable*: 'T' provides an assignment operator that takes an rvalue
//:     of type (non-'const') 'T'.
//:
//: *copy-assignable*: 'T' provides an assignment operator that takes an lvalue
//:     or rvalue of type (possibly 'const') 'T'.
//:
//: *emplace-constructible*: 'T' is 'emplace-constructible' into 'X' from
//:     'args' means that the following expression is well-formed:
//:
//:      'allocator_traits<A>::construct(m, p, args)'
//:
//: *erasable*: 'T' provides a destructor.  More precisely, 'T' is 'erasable'
//:     from 'X' means that the following expression is well-formed:
//:
//:      'allocator_traits<A>::destroy(m, p)'
//:
//: *equality-comparable*: The type provides an equality-comparison operator
//:     that defines an equivalence relationship and is both reflexive and
//:     transitive.
//
///Requirements on 'HASH' and 'EQUAL'
///----------------------------------
// The (template parameter) types 'HASH' and 'EQUAL' must be copy-constructible
// function-objects.  Note that this requirement is somewhat stronger than the
// requirement currently in the standard; see the discussion for Issue 2215
// (http://cplusplus.github.com/LWG/lwg-active.html#2215);
//
// 'HASH' shall support a function call operator compatible with the following
// statements:
//..
//  HASH        hash;
//  KEY         key;
//  std::size_t result = hash(key);
//..
// where the definition of the called function meets the requirements of a
// hash function, as specified in {'bslstl_hash'|Standard Hash Function}.
//
// 'EQUAL' shall support a function call operator compatible with the following
// statements:
//..
//  EQUAL equal;
//  KEY   key1, key2;
//  bool  result = equal(key1, key2);
//..
// where the definition of the called function defines an equivalence
// relationship on keys that is both reflexive and transitive.
//
// 'HASH' and 'EQUAL' function-objects are further constrained such that any
// two objects whose keys compare equivalent by the comparator shall also
// produce the same value from the hasher.
//
///Memory Allocation
///-----------------
// The type supplied as an unordered multimap's 'ALLOCATOR' template parameter
// determines how that unordered multimap will allocate memory.  The
// 'unordered_multimap' template supports allocators meeting the requirements
// of the C++11 standard [17.6.3.5].  In addition, it supports
// scoped-allocators derived from the 'bslma::Allocator' memory allocation
// protocol.  Clients intending to use 'bslma'-style allocators should use the
// template's default 'ALLOCATOR' type.  The default type for the 'ALLOCATOR'
// template parameter, 'bsl::allocator', provides a C++11 standard-compatible
// adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of an 'unordered_multimap'
// instantiation is 'bsl::allocator', then objects of that unordered multimap
// type will conform to the standard behavior of a 'bslma'-allocator-enabled
// type.  Such an unordered multimap accepts an optional 'bslma::Allocator'
// argument at construction.  If the address of a 'bslma::Allocator' object is
// explicitly supplied at construction, it is used to supply memory for the
// unordered multimap throughout its lifetime; otherwise, the unordered
// multimap will use the default allocator installed at the time of the
// unordered multimap's construction (see 'bslma_default').  In addition to
// directly allocating memory from the indicated 'bslma::Allocator', an
// unordered multimap supplies that allocator's address to the constructors of
// contained objects of the (template parameter) types 'KEY' and 'VALUE', if
// respectively, the types define the 'bslma::UsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'unordered_multimap':
//..
//  Legend
//  ------
//  'K'             - (template parameter) type 'KEY' of 'unordered_multimap'
//  'V'             - (template parameter) type 'VALUE' of 'unordered_multimap'
//  'a', 'b'        - two distinct objects of type 'unordered_multimap<K, V>'
//  'rv'            - modifiable rvalue of type 'unordered_multimap<K, V>'
//  'n', 'm'        - number of elements in 'a' and 'b', respectively
//  'w'             - number of buckets of 'a'
//  'value_type'    - 'pair<const K, V>'
//  'hf'            - hash function for objects of type 'K'
//  'eq'            - equivalence comparator for objects of type 'K'
//  'al'            - STL-style memory allocator
//  'k'             - an object of type 'K'
//  'v'             - object of type 'V'
//  'vt'            - object of type 'value_type'
//  'rvt'           - modifiable rvalue of type 'value_type'
//  'idx'           - bucket index
//  'li'            - object of type 'initializer_list<value_type>'
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'p1', 'p2'      - two iterators belonging to 'a'
//  distance(i1,i2) - the number of elements in the range '[i1 .. i2)'
//  distance(p1,p2) - the number of elements in the range '[p1 .. p2)'
//  'z'             - floating point value representing a load factor
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | unordered_multimap<K, V> a;    (dflt construction) | O[1]               |
//  | unordered_multimap<K, V> a(al);                    |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multimap<K, V> a(rv);(move construction) | O[1] if 'a' and    |
//  | unordered_multimap<K, V> a(rv, al);                | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | unordered_multimap<K, V> a(b); (copy construction) | Average: O[n]      |
//  | unordered_multimap<K, V> a(b, al);                 | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multimap<K, V> a(w);                     | O[n]               |
//  | unordered_multimap<K, V> a(w, al);                 |                    |
//  | unordered_multimap<K, V> a(w, hf);                 |                    |
//  | unordered_multimap<K, V> a(w, hf, al);             |                    |
//  | unordered_multimap<K, V> a(w, hf, eq);             |                    |
//  | unordered_multimap<K, V> a(w, hf, eq, al);         |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multimap<K, V> a(i1, i2);                | Average: O[N]      |
//  | unordered_multimap<K, V> a(i1, i2, al);            | Worst:   O[N^2]    |
//  | unordered_multimap<K, V> a(i1, i2, w);             | where N =          |
//  | unordered_multimap<K, V> a(i1, i2, w, al);         |  distance(i1, i2)] |
//  | unordered_multimap<K, V> a(i1, i2, w, hf);         |                    |
//  | unordered_multimap<K, V> a(i1, i2, w, hf, al);     |                    |
//  | unordered_multimap<K, V> a(i1, i2, w, hf, eq);     |                    |
//  | unordered_multimap<K, V> a(i1, i2, w, hf, eq, al); |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multimap<K, V> a(li);                    | Average: O[N]      |
//  | unordered_multimap<K, V> a(li, al);                | Worst:   O[N^2]    |
//  | unordered_multimap<K, V> a(li, w);                 | where N =          |
//  | unordered_multimap<K, V> a(li, w, al);             |         'li.size()'|
//  | unordered_multimap<K, V> a(li, w, hf);             |                    |
//  | unordered_multimap<K, V> a(li, w, hf, al);         |                    |
//  | unordered_multimap<K, V> a(li, w, hf, eq);         |                    |
//  | unordered_multimap<K, V> a(li, w, hf, eq, al);     |                    |
//  +----------------------------------------------------+--------------------+
//  | a.~unordered_multimap<K, V>(); (destruction)       | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = rv;                          (move assignment) | O[1] if 'a' and    |
//  |                                                    | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | a = b;                           (copy assignment) | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a = li;                                            | Average: O[N]      |
//  |                                                    | Worst:   O[N^2]    |
//  |                                                    | where N =          |
//  |                                                    |         'li.size()'|
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend()           | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.begin(idx), a.end(idx), a.cbegin(idx),           | O[1]               |
//  | a.cend(idx)                                        |                    |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | Best:  O[n]        |
//  |                                                    | Worst: O[n^2]      |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a, b)                              | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.key_eq()                                         | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.hash_function()                                  | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.allocator()                                      | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(vt)                                       | Average: O[1]      |
//  | a.insert(rvt)                                      | Worst:   O[n]      |
//  | a.emplace(Args&&...)                               |                    |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, vt)                                   | Average: O[1]      |
//  | a.insert(p1, rvt)                                  | Worst:   O[n]      |
//  | a.emplace(p1, Args&&...)                           |                    |
//  +----------------------------------------------------+--------------------+
//  | a.insert(i1, i2)                                   | Average: O[        |
//  |                                                    |   distance(i1, i2)]|
//  |                                                    | Worst:   O[n *     |
//  |                                                    |   distance(i1, i2)]|
//  +----------------------------------------------------+--------------------+
//  | a.insert(li);                                      | Average: O[N]      |
//  |                                                    | Worst:   O[n * N]  |
//  |                                                    | where N =          |
//  |                                                    |         'li.size()'|
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.erase(k)                                         | Average:           |
//  |                                                    |       O[a.count(k)]|
//  |                                                    | Worst:             |
//  |                                                    |       O[n]         |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | Average: O[        |
//  |                                                    |   distance(p1, p2)]|
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.contains(k)                                      | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.find(k)                                          | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.count(k)                                         | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.equal_range(k)                                   | Average: O[        |
//  |                                                    |         a.count(k)]|
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | a.bucket_count()                                   | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_bucket_count()                               | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.bucket(k)                                        | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.bucket_size(idx)                                 | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.load_factor()                                    | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_load_factor()                                | O[1]               |
//  | a.max_load_factor(z)                               | Average: O[1]      |
//  +----------------------------------------------------+--------------------+
//  | a.rehash(w)                                        | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a.reserve(w)                                       | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//..
//
///Iterator, Pointer, and Reference Invalidation
///---------------------------------------------
// No method of 'unordered_multimap' invalidates a pointer or reference to an
// element in the unordered multimap unless it erases that element, such as any
// 'erase' overload, 'clear', or the destructor (which erases all elements).
// Pointers and references are stable through a rehash.
//
// Iterators to elements in the container are invalidated by any rehash, so
// iterators may be invalidated by an 'insert' or 'emplace' call if it triggers
// a rehash (but not otherwise).  Iterators to specific elements are also
// invalidated when those elements are erased.  Note that although the 'end'
// iterator does not refer to any element in the container, it may be
// invalidated by any non-'const' method.
//
///Unordered Multimap Configuration
///---------------------------------
// The unordered multimap has interfaces that can provide insight into, and
// control of, its inner workings.  The syntax and semantics of these
// interfaces for 'bsl::unordered_multimap' are identical to those of
// 'bsl::unordered_map'.  See the discussion in
// {'bslstl_unorderedmap'|Unordered Map Configuration} and the illustrative
// material in {'bslstl_unorderedmap'|Example 2}.
//
///Practical Requirements on 'HASH'
///--------------------------------
// An important factor in the performance of an unordered multimap (and any of
// the other unordered containers) is the choice of a hash function.  Please
// see the discussion in {'bslstl_unorderedmap'|Practical Requirements on
// 'HASH'}.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Concordance
///- - - - - - - - - - - - - - - - -
// Unordered multimaps are useful in situations when there is no meaningful way
// to compare key values, when the order of the keys is irrelevant to the
// problem domain, or (even if there is a meaningful ordering) the benefit of
// ordering the results is outweighed by the higher performance provided by
// unordered multimaps (compared to ordered multimaps).
//
// One uses a multimap (ordered or unordered) when there may be more than one
// mapped value associated with a key value.  In this example we will use
// 'bslstl_unorderedmultimap' to create a concordance (an index of where each
// unique word appears in the set of documents).
//
// Our source of documents is a set of statically initialized arrays:
//..
//  static char document0[] =
//  " IN CONGRESS, July 4, 1776.\n"
//  "\n"
//  " The unanimous Declaration of the thirteen united States of America,\n"
//  "\n"
//  " When in the Course of human events, it becomes necessary for one\n"
//  " people to dissolve the political bands which have connected them with\n"
//  " another, and to assume among the powers of the earth, the separate\n"
//  " and equal station to which the Laws of Nature and of Nature's God\n"
//  " entitle them, a decent respect to the opinions of mankind requires\n"
//  " that they should declare the causes which impel them to the\n"
//  " separation.  We hold these truths to be self-evident, that all men\n"
//  " are created equal, that they are endowed by their Creator with\n"
//  " certain unalienable Rights, that among these are Life, Liberty and\n"
//  " the pursuit of Happiness.--That to secure these rights, Governments\n"
//  " are instituted among Men, deriving their just powers from the consent\n"
//  " of the governed, --That whenever any Form of Government becomes\n"
//  ...
//  " States may of right do.  And for the support of this Declaration,\n"
//  " with a firm reliance on the protection of divine Providence, we\n"
//  " mutually pledge to each other our Lives, our Fortunes and our sacred\n"
//  " Honor.\n";
//
//  static char document1[] =
//  "/The Universal Declaration of Human Rights\n"
//  "/-----------------------------------------\n"
//  "/Preamble\n"
//  "/ - - - -\n"
//  " Whereas recognition of the inherent dignity and of the equal and\n"
//  " inalienable rights of all members of the human family is the\n"
//  " foundation of freedom, justice and peace in the world,\n"
//  ...
//  "/Article 30\n"
//  "/ - - - - -\n"
//  " Nothing in this Declaration may be interpreted as implying for any\n"
//  " State, group or person any right to engage in any activity or to\n"
//  " perform any act aimed at the destruction of any of the rights and\n"
//  " freedoms set forth herein.\n";
//
//  static char document2[] =
//  "/CHARTER OF FUNDAMENTAL RIGHTS OF THE EUROPEAN UNION\n"
//  "/---------------------------------------------------\n"
//  " PREAMBLE\n"
//  "\n"
//  " The peoples of Europe, in creating an ever closer union among them,\n"
//  " are resolved to share a peaceful future based on common values.\n"
//  ...
//  "/Article 54\n"
//  "/-  -  -  -\n"
//  " Prohibition of abuse of rights\n"
//  "\n"
//  " Nothing in this Charter shall be interpreted as implying any right to\n"
//  " engage in any activity or to perform any act aimed at the destruction\n"
//  " of any of the rights and freedoms recognized in this Charter or at\n"
//  " their limitation to a greater extent than is provided for herein.\n";
//
//  static char * const documents[]  = { document0,
//                                       document1,
//                                       document2
//                                     };
//  const int           numDocuments = sizeof documents / sizeof *documents;
//..
// First, we define several aliases to make our code more comprehensible:
//..
//  typedef bsl::pair<int, int>                  WordLocation;
//      // Document code number ('first') and word offset ('second') in that
//      // document specify a word location.  The first word in the document
//      // is at word offset 0.
//
//  typedef bsl::unordered_multimap<bsl::string, WordLocation>
//                                               Concordance;
//  typedef Concordance::const_iterator          ConcordanceConstItr;
//..
// Next, we create an (empty) unordered multimap to hold our word tallies:
//..
//  Concordance concordance;
//..
// Then, we define the set of characters that define word boundaries:
//..
//  const char *delimiters = " \n\t,:;.()[]?!/";
//..
// Next, we extract the words from our documents.  Note that 'strtok' modifies
// the document arrays (which were not made 'const').
//
// As each word is located, we create a map value -- a pair of the word
// converted to a 'bsl::string' and a 'WordLocation' object (itself a pair of
// document code and (word) offset of that word in the document) -- and insert
// the map value into the unordered multimap.  Note that (unlike maps and
// unordered maps) there is no status to check; the insertion succeeds even if
// the key is already present in the unordered multimap.
//..
//  for (int idx = 0; idx < numDocuments; ++idx) {
//      int wordOffset = 0;
//      for (char *cur = strtok(documents[idx], delimiters);
//                 cur;
//                 cur = strtok(NULL,           delimiters)) {
//          WordLocation            location(idx, wordOffset++);
//          Concordance::value_type value(bsl::string(cur), location);
//          concordance.insert(value);
//      }
//  }
//..
// Then, we can print a complete concordance by iterating through the unordered
// multimap:
//..
//  for (ConcordanceConstItr itr  = concordance.begin(),
//                           end  = concordance.end();
//                           end != itr; ++itr) {
//      printf("\"%s\", %2d, %4d\n",
//             itr->first.c_str(),
//             itr->second.first,
//             itr->second.second);
//  }
//..
// Standard output shows:
//..
//  "extent",  2, 3837
//  "greater",  2, 3836
//  "abuse",  2, 3791
//  "constitutions",  2, 3782
//  "affecting",  2, 3727
//  ...
//  "he",  1, 1746
//  "he",  1,  714
//  "he",  0,  401
//  "include",  2,  847
//..
// Next, if there are some particular words of interest, we seek them out using
// the 'equal_range' method of the 'concordance' object:
//..
//  const bsl::string wordsOfInterest[] = { "human",
//                                          "rights",
//                                          "unalienable",
//                                          "inalienable"
//                                        };
//  const size_t numWordsOfInterest = sizeof  wordsOfInterest
//                                    / sizeof *wordsOfInterest;
//
//  for (size_t idx = 0; idx < numWordsOfInterest; ++idx) {
//     bsl::pair<ConcordanceConstItr,
//               ConcordanceConstItr> found = concordance.equal_range(
//                                                       wordsOfInterest[idx]);
//     for (ConcordanceConstItr itr  = found.first,
//                              end  = found.second;
//                              end != itr; ++itr) {
//         printf("\"%s\", %2d, %4d\n",
//                itr->first.c_str(),
//                itr->second.first,
//                itr->second.second);
//     }
//     printf("\n");
//  }
//..
// Finally, we see on standard output:
//..
//  "human",  2, 3492
//  "human",  2, 2192
//  "human",  2,  534
//  ...
//  "human",  1,   65
//  "human",  1,   43
//  "human",  1,   25
//  "human",  0,   20
//
//  "rights",  2, 3583
//  "rights",  2, 3553
//  "rights",  2, 3493
//  ...
//  "rights",  1,   44
//  "rights",  1,   19
//  "rights",  0,  496
//  "rights",  0,  126
//
//  "unalienable",  0,  109
//
//  "inalienable",  1,   18
//
//..
// {'bslstl_unorderedmap'|Example 3} shows how to use the concordance to create
// an inverse concordance, and how to use the inverse concordance to find the
// context (surrounding words) of a word of interest.

#include <bslscm_version.h>

#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#include <bslstl_hashtable.h>
#include <bslstl_hashtablebucketiterator.h>
#include <bslstl_hashtableiterator.h>
#include <bslstl_iteratorutil.h>
#include <bslstl_pair.h>
#include <bslstl_unorderedmapkeyconfiguration.h>

#include <bslalg_bidirectionallink.h>
#include <bslalg_bidirectionalnode.h>
#include <bslalg_typetraithasstliterators.h>

#include <bslma_allocatortraits.h>
#include <bslma_isstdallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_typeidentity.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
# include <initializer_list>
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <type_traits>  // 'std::is_constructible'
    #ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    #error Rvalue references curiously absent despite native 'type_traits'.
    #endif
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslstl_unorderedmultimap.h
# define COMPILING_BSLSTL_UNORDEREDMULTIMAP_H
# include <bslstl_unorderedmultimap_cpp03.h>
# undef COMPILING_BSLSTL_UNORDEREDMULTIMAP_H
#else

namespace bsl {

template <class KEY,
          class VALUE,
          class HASH      = bsl::hash<KEY>,
          class EQUAL     = bsl::equal_to<KEY>,
          class ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE> > >
class unordered_multimap {
    // This class template implements a value-semantic container type holding a
    // collection of (possibly equivalent) keys (of the template parameter type
    // 'KEY'), each mapped to their associated values (of another template
    // parameter type 'VALUE').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for BDEX serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

  private:
    // PRIVATE TYPES
    typedef bsl::allocator_traits<ALLOCATOR>                 AllocatorTraits;
        // This 'typedef' is an alias for the allocator traits type associated
        // with this container.

    typedef pair<const KEY, VALUE>                           ValueType;
        // This 'typedef' is an alias for the type of key-value pair objects
        // maintained by this unordered multimap.

    typedef ::BloombergLP::bslstl::UnorderedMapKeyConfiguration<const KEY,
                                                                ValueType>
                                                             ListConfiguration;
        // This 'typedef' is an alias for the policy used internally by this
        // container to extract the 'KEY' value from the values maintained by
        // this unordered multimap.

    typedef ::BloombergLP::bslstl::HashTable<ListConfiguration,
                                             HASH,
                                             EQUAL,
                                             ALLOCATOR>      Impl;
        // This 'typedef' is an alias for the template instantiation of the
        // underlying 'bslstl::HashTable' used to implement this unordered
        // multimap.

    typedef ::BloombergLP::bslalg::BidirectionalLink         HashTableLink;
        // This 'typedef' is an alias for the type of links maintained by the
        // linked list of elements held by the underlying 'bslstl::HashTable'.

    typedef BloombergLP::bslmf::MovableRefUtil               MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

    // FRIENDS
    template <class KEY2,
              class VALUE2,
              class HASH2,
              class EQUAL2,
              class ALLOCATOR2>
    friend bool operator==(
           const unordered_multimap<KEY2, VALUE2, HASH2, EQUAL2, ALLOCATOR2>&,
           const unordered_multimap<KEY2, VALUE2, HASH2, EQUAL2, ALLOCATOR2>&);

  public:
    // PUBLIC TYPES
    typedef KEY                                        key_type;
    typedef VALUE                                      mapped_type;
    typedef bsl::pair<const KEY, VALUE>                value_type;
    typedef HASH                                       hasher;
    typedef EQUAL                                      key_equal;
    typedef ALLOCATOR                                  allocator_type;

    typedef value_type&                                reference;
    typedef const value_type&                          const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

    typedef ::BloombergLP::bslstl::HashTableIterator<
                         value_type, difference_type>  iterator;

    typedef ::BloombergLP::bslstl::HashTableIterator<
                   const value_type, difference_type>  const_iterator;

    typedef ::BloombergLP::bslstl::HashTableBucketIterator<
                         value_type, difference_type>  local_iterator;

    typedef ::BloombergLP::bslstl::HashTableBucketIterator<
                   const value_type, difference_type>  const_local_iterator;

  private:
    // DATA
    Impl d_impl;

  public:
    // CREATORS
    unordered_multimap();
    explicit unordered_multimap(size_type        initialNumBuckets,
                                const HASH&      hashFunction = HASH(),
                                const EQUAL&     keyEqual = EQUAL(),
                                const ALLOCATOR& basicAllocator = ALLOCATOR());
    unordered_multimap(size_type        initialNumBuckets,
                       const HASH&      hashFunction,
                       const ALLOCATOR& basicAllocator);
    unordered_multimap(size_type        initialNumBuckets,
                       const ALLOCATOR& basicAllocator);
    explicit unordered_multimap(const ALLOCATOR& basicAllocator);
        // Create an empty unordered multimap.  Optionally specify an
        // 'initialNumBuckets' indicating the minimum initial size of the array
        // of buckets of this container.  If 'initialNumBuckets' is not
        // supplied, a single empty bucket is used.  Optionally specify a
        // 'hashFunction' used to generate the hash values for the keys
        // contained in this unordered multimap.  If 'hashFunction' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'HASH' is used.  Optionally specify a key-equivalence functor
        // 'keyEqual' used to verify that two keys are equivalent.  If
        // 'keyEqual' is not supplied, a default-constructed object of the
        // (template parameter) type 'EQUAL' is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default), then 'basicAllocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used.

    unordered_multimap(const unordered_multimap& original);
        // Create an unordered multimap having the same value as the specified
        // 'original' object.  Use a copy of 'original.hash_function()' to
        // generate hash values for the keys contained in this unordered
        // multimap.  Use a copy of 'original.key_eq()' to verify that two keys
        // are equivalent.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  This method requires that the (template parameter)
        // types 'KEY' and 'VALUE' both be 'copy-insertable' into this
        // unordered multimap (see {Requirements on 'KEY' and 'VALUE'}).

    unordered_multimap(
                  BloombergLP::bslmf::MovableRef<unordered_multimap> original);
        // Create an unordered multimap having the same value as the specified
        // 'original' object by moving (in constant time) the contents of
        // 'original' to the new unordered multimap.  Use a copy of
        // 'original.hash_function()' to generate hash values for the keys
        // contained in this unordered multimap.  Use a copy of
        // 'original.key_eq()' to verify that two keys are equivalent.  The
        // allocator associated with 'original' is propagated for use in the
        // newly-created unordered multimap.  'original' is left in a valid but
        // unspecified state.

    unordered_multimap(
                const unordered_multimap&                      original,
                const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create an unordered multimap having the same value as the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  Use a copy of 'original.hash_function()' to generate hash
        // values for the keys contained in this unordered multimap.  Use a
        // copy of 'original.key_eq()' to verify that two keys are equivalent.
        // This method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be 'copy-insertable' into this unordered multimap (see
        // {Requirements on 'KEY' and 'VALUE'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // (template parameter) type 'ALLOCATOR' is 'bsl::allocator' (the
        // default).

    unordered_multimap(
            BloombergLP::bslmf::MovableRef<unordered_multimap> original,
            const typename type_identity<ALLOCATOR>::type&     basicAllocator);
        // Create an unordered multimap having the same value as the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  The contents of 'original' are moved (in constant time) to
        // the new unordered multimap if 'basicAllocator ==
        // original.get_allocator()', and are move-inserted (in linear time)
        // using 'basicAllocator' otherwise.  'original' is left in a valid but
        // unspecified state.  Use a copy of 'original.hash_function()' to
        // generate hash values for the keys contained in this unordered
        // multimap.  Use a copy of 'original.key_eq()' to verify that two keys
        // are equivalent.  This method requires that the (template parameter)
        // types 'KEY' and 'VALUE' both be 'move-insertable' into this
        // unordered multimap (see {Requirements on 'KEY' and 'VALUE'}).  Note
        // that a 'bslma::Allocator *' can be supplied for 'basicAllocator' if
        // the (template parameter) type 'ALLOCATOR' is 'bsl::allocator' (the
        // default).

    template <class INPUT_ITERATOR>
    unordered_multimap(INPUT_ITERATOR   first,
                       INPUT_ITERATOR   last,
                       size_type        initialNumBuckets = 0,
                       const HASH&      hashFunction = HASH(),
                       const EQUAL&     keyEqual = EQUAL(),
                       const ALLOCATOR& basicAllocator = ALLOCATOR());
    template <class INPUT_ITERATOR>
    unordered_multimap(INPUT_ITERATOR   first,
                       INPUT_ITERATOR   last,
                       size_type        initialNumBuckets,
                       const HASH&      hashFunction,
                       const ALLOCATOR& basicAllocator);
    template <class INPUT_ITERATOR>
    unordered_multimap(INPUT_ITERATOR   first,
                       INPUT_ITERATOR   last,
                       size_type        initialNumBuckets,
                       const ALLOCATOR& basicAllocator);
    template <class INPUT_ITERATOR>
    unordered_multimap(INPUT_ITERATOR   first,
                       INPUT_ITERATOR   last,
                       const ALLOCATOR& basicAllocator);
        // Create an unordered multimap, and insert each 'value_type' object in
        // the sequence starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Optionally specify
        // an 'initialNumBuckets' indicating the minimum initial size of the
        // array of buckets of this container.  If 'initialNumBuckets' is not
        // supplied, a single empty bucket is used if 'first' and 'last' denote
        // an empty range, and an unspecified number of buckets is used
        // otherwise.  Optionally specify a 'hashFunction' used to generate
        // hash values for the keys contained in this unordered multimap.  If
        // 'hashFunction' is not supplied, a default-constructed object of
        // (template parameter) type 'HASH' is used.  Optionally specify a
        // key-equivalence functor 'keyEqual' used to verify that two keys are
        // equivalent.  If 'keyEqual' is not supplied, a default-constructed
        // object of (template parameter) type 'EQUAL' is used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not supplied, a default-constructed object of
        // the (template parameter) type 'ALLOCATOR' is used.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used to
        // supply memory.  The (template parameter) type 'INPUT_ITERATOR' shall
        // meet the requirements of an input iterator defined in the C++11
        // standard [24.2.3] providing access to values of a type convertible
        // to 'value_type', and 'value_type' must be 'emplace-constructible'
        // from '*i' into this unordered multimap, where 'i' is a
        // dereferenceable iterator in the range '[first .. last)' (see
        // {Requirements on 'KEY' and 'VALUE'}).  The behavior is undefined
        // unless 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    template <
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t<
                         std::is_invocable_v<EQUAL, const KEY &, const KEY &>>,
    class = bsl::enable_if_t< bsl::IsStdAllocator_v<ALLOCATOR>>
    >
# endif
    unordered_multimap(
               std::initializer_list<value_type> values,
               size_type                         initialNumBuckets = 0,
               const HASH&                       hashFunction = HASH(),
               const EQUAL&                      keyEqual = EQUAL(),
               const ALLOCATOR&                  basicAllocator = ALLOCATOR());
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    template <
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t< bsl::IsStdAllocator_v<ALLOCATOR>>
    >
# endif
    unordered_multimap(std::initializer_list<value_type> values,
                       size_type                         initialNumBuckets,
                       const HASH&                       hashFunction,
                       const ALLOCATOR&                  basicAllocator);
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    template <
    class = bsl::enable_if_t< bsl::IsStdAllocator_v<ALLOCATOR>>
    >
# endif
    unordered_multimap(std::initializer_list<value_type> values,
                       size_type                         initialNumBuckets,
                       const ALLOCATOR&                  basicAllocator);
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    template <
    class = bsl::enable_if_t< bsl::IsStdAllocator_v<ALLOCATOR>>
    >
# endif
    unordered_multimap(std::initializer_list<value_type> values,
                       const ALLOCATOR&                  basicAllocator);
        // Create an unordered multimap and insert each 'value_type' object in
        // the specified 'values' initializer list.  Optionally specify an
        // 'initialNumBuckets' indicating the minimum initial size of the array
        // of buckets of this container.  If 'initialNumBuckets' is not
        // supplied, a single empty bucket is used if 'values' is empty, and an
        // unspecified number of buckets is used otherwise.  Optionally specify
        // a 'hashFunction' used to generate the hash values for the keys
        // contained in this unordered multimap.  If 'hashFunction' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'HASH' is used.  Optionally specify a key-equivalence functor
        // 'keyEqual' used to verify that two keys are equivalent.  If
        // 'keyEqual' is not supplied, a default-constructed object of the
        // (template parameter) type 'EQUAL' is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used to supply memory.  This method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be 'copy-insertable' into this unordered multimap (see {Requirements
        // on 'KEY' and 'VALUE'}).  Note that a 'bslma::Allocator *' can be
        // supplied for 'basicAllocator' if the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).
#endif

    ~unordered_multimap();
        // Destroy this object.

    // MANIPULATORS
    unordered_multimap& operator=(const unordered_multimap& rhs);
        // Assign to this object the value, hash function, and key-equivalence
        // comparator of the specified 'rhs' object, propagate to this object
        // the allocator of 'rhs' if the 'ALLOCATOR' type has trait
        // 'propagate_on_container_copy_assignment', and return a reference
        // providing modifiable access to this object.  If an exception is
        // thrown, '*this' is left in a valid but unspecified state.  This
        // method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be 'copy-assignable' and 'copy-insertable' into this
        // unordered multimap (see {Requirements on 'KEY' and 'VALUE'}).

    unordered_multimap&
    operator=(BloombergLP::bslmf::MovableRef<unordered_multimap> rhs)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                AllocatorTraits::is_always_equal::value &&
                                std::is_nothrow_move_assignable<HASH>::value &&
                                std::is_nothrow_move_assignable<EQUAL>::value);
        // Assign to this object the value, hash function, and key-equivalence
        // comparator of the specified 'rhs' object, propagate to this object
        // the allocator of 'rhs' if the 'ALLOCATOR' type has trait
        // 'propagate_on_container_move_assignment', and return a reference
        // providing modifiable access to this object.  The contents of 'rhs'
        // are moved (in constant time) to this unordered multimap if
        // 'get_allocator() == rhs.get_allocator()' (after accounting for the
        // aforementioned trait); otherwise, all elements in this unordered
        // multimap are either destroyed or move-assigned to, and each
        // additional element in 'rhs' is move-inserted into this unordered
        // multimap.  'rhs' is left in a valid but unspecified state, and if an
        // exception is thrown, '*this' is left in a valid but unspecified
        // state.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be 'move-assignable' and 'move-insertable'
        // into this unordered multimap (see {Requirements on 'KEY' and
        // 'VALUE'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    unordered_multimap& operator=(std::initializer_list<value_type> values);
        // Assign to this object the value resulting from first clearing this
        // unordered multimap and then inserting each 'value_type' object in
        // the specified 'values' initializer list, and return a reference
        // providing modifiable access to this object.  This method requires
        // that the (template parameter) types 'KEY' and 'VALUE' both be
        // 'copy-insertable' into this unordered multimap (see {Requirements on
        // 'KEY' and 'VALUE'}).
#endif

    iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this unordered multimap, or the 'end' iterator if this
        // unordered multimap is empty.

    iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the past-the-end
        // position in the sequence of 'value_type' objects maintained by this
        // unordered multimap.

    local_iterator begin(size_type index);
        // Return a local iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered multimap, or the 'end(index)' iterator
        // if the indexed bucket is empty.  The behavior is undefined unless
        // 'index < bucket_count()'.

    local_iterator end(size_type index);
        // Return a local iterator providing modifiable access to the
        // past-the-end position in the sequence of 'value_type' objects of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered multimap.  The behavior is undefined
        // unless 'index < bucket_count()'.

    void clear() BSLS_KEYWORD_NOEXCEPT;
        // Remove all entries from this unordered multimap.  Note that this
        // object will be empty after this call, but allocated memory may be
        // retained for future use.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      pair<iterator, iterator> >::type
    equal_range(const LOOKUP_KEY& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this unordered multimap with a
        // key equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered
        // multimap contains no 'value_type' objects with a key equivalent to
        // 'key', then the two returned iterators will have the same value.
        // The behavior is undefined unless 'key' is equivalent to the key of
        // the elements of at most one equivalent-key group in this unordered
        // multimap.
        //
        // Note: implemented inline due to Sun CC compilation error.
        {
            typedef bsl::pair<iterator, iterator> ResultType;
            HashTableLink *first;
            HashTableLink *last;
            d_impl.findRange(&first, &last, key);
            return ResultType(iterator(first), iterator(last));
        }

    pair<iterator, iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this unordered multimap with a
        // key equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered
        // multimap contains no 'value_type' objects with a key equivalent to
        // 'key', then the two returned iterators will have the same value.

    size_type erase(const key_type& key);
        // Remove from this unordered multimap all 'value_type' objects with a
        // key equivalent to the specified 'key', if such exist, and return the
        // number of objects erased; otherwise, if there are no 'value_type'
        // objects with a key equivalent to 'key', return 0 with no other
        // effect.  This method invalidates only iterators and references to
        // the removed element and previously saved values of the 'end()'
        // iterator, and preserves the relative order of the elements not
        // removed.

    iterator erase(const_iterator position);
    iterator erase(iterator position);
        // Remove from this unordered multimap the 'value_type' object at the
        // specified 'position', and return an iterator referring to the
        // element immediately following the removed element, or to the
        // past-the-end position if the removed element was the last element in
        // the sequence of elements maintained by this unordered multimap.
        // This method invalidates only iterators and references to the removed
        // element and previously saved values of the 'end()' iterator, and
        // preserves the relative order of the elements not removed.  The
        // behavior is undefined unless 'position' refers to a 'value_type'
        // object in this unordered multimap.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this unordered multimap the 'value_type' objects
        // starting at the specified 'first' position up to, but not including,
        // the specified 'last' position, and return 'last'.  This method
        // invalidates only iterators and references to the removed element and
        // previously saved values of the 'end()' iterator, and preserves the
        // relative order of the elements not removed.  The behavior is
        // undefined unless 'first' and 'last' either refer to elements in this
        // unordered multimap or are the 'end' iterator, and the 'first'
        // position is at or before the 'last' position in the sequence
        // provided by this container.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      iterator>::type
    find(const LOOKUP_KEY& key)
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the sequence of all the 'value_type' objects
        // of this unordered multimap with a key equivalent to the specified
        // 'key', if such entries exist, and the past-the-end ('end') iterator
        // otherwise.  The behavior is undefined unless 'key' is equivalent to
        // the key of the elements of at most one equivalent-key group in this
        // unordered multimap.
        //
        // Note: implemented inline due to Sun CC compilation error.
        {
            return iterator(d_impl.find(key));
        }

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the sequence of all the 'value_type' objects
        // of this unordered multimap with a key equivalent to the specified
        // 'key', if such entries exist, and the past-the-end ('end') iterator
        // otherwise.

    iterator insert(const value_type& value);
        // Insert the specified 'value' into this unordered multimap, and
        // return an iterator referring to the newly inserted 'value_type'
        // object.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be 'copy-insertable' into this unordered
        // multimap (see {Requirements on 'KEY' and 'VALUE'}).

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    template <class ALT_VALUE_TYPE>
    iterator
#elif !defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
    template <class ALT_VALUE_TYPE>
    typename enable_if<is_convertible<ALT_VALUE_TYPE, value_type>::value,
                       iterator>::type
#else
    template <class ALT_VALUE_TYPE>
    typename enable_if<std::is_constructible<value_type,
                                             ALT_VALUE_TYPE&&>::value,
                       iterator>::type
#endif
    insert(BSLS_COMPILERFEATURES_FORWARD_REF(ALT_VALUE_TYPE) value)
        // Insert into this unordered multimap a 'value_type' object created
        // from the specified 'value', and return an iterator referring to the
        // newly inserted 'value_type' object.  This method requires that the
        // (template parameter) types 'KEY' and 'VALUE' both be
        // 'move-insertable' into this unordered multimap (see {Requirements on
        // 'KEY' and 'VALUE'}), and the 'value_type' be constructible from the
        // (template parameter) 'ALT_VALUE_TYPE'.
    {
        // Note that some compilers fail when this method is defined
        // out-of-line.

        return emplace(BSLS_COMPILERFEATURES_FORWARD(ALT_VALUE_TYPE, value));
    }

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this unordered multimap (in
        // constant time if the specified 'hint' refers to an element in this
        // container with a key equivalent to the key of 'value'), and return
        // an iterator referring to the newly inserted 'value_type' object.  If
        // 'hint' does not refer to an element in this container with a key
        // equivalent to the key of 'value', this operation has worst case
        // 'O[N]' and average case constant-time complexity, where 'N' is the
        // size of this unordered multimap.  This method requires that the
        // (template parameter) types 'KEY' and 'VALUE' both be
        // 'copy-insertable' into this unordered multimap (see {Requirements on
        // 'KEY' and 'VALUE'}).  The behavior is undefined unless 'hint' is an
        // iterator in the range '[begin() .. end()]' (both endpoints
        // included).

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    template <class ALT_VALUE_TYPE>
    iterator
#elif !defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
    template <class ALT_VALUE_TYPE>
    typename enable_if<is_convertible<ALT_VALUE_TYPE, value_type>::value,
                       iterator>::type
#else
    template <class ALT_VALUE_TYPE>
    typename enable_if<std::is_constructible<value_type,
                                             ALT_VALUE_TYPE&&>::value,
                       iterator>::type
#endif
    insert(const_iterator                                    hint,
           BSLS_COMPILERFEATURES_FORWARD_REF(ALT_VALUE_TYPE) value)
        // Insert into this unordered multimap a 'value_type' object created
        // from the specified 'value' (in constant time if the specified 'hint'
        // refers to an element in this container with a key equivalent to the
        // key of 'value'), and return an iterator referring to the newly
        // inserted 'value_type' object.  If 'hint' does not refer to an
        // element in this container with a key equivalent to the key of
        // 'value', this operation has worst case 'O[N]' and average case
        // constant-time complexity, where 'N' is the size of this unordered
        // multimap.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be 'move-insertable' into this unordered
        // multimap (see {Requirements on 'KEY' and 'VALUE'}), and the
        // 'value_type' be constructible from the (template parameter)
        // 'ALT_VALUE_TYPE'.  The behavior is undefined unless 'hint' is an
        // iterator in the range '[begin() .. end()]' (both endpoints
        // included).
    {
        // Note that some compilers fail when this method is defined
        // out-of-line.

        return emplace_hint(hint,
                        BSLS_COMPILERFEATURES_FORWARD(ALT_VALUE_TYPE, value));
    }

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this unordered multimap the value of each 'value_type'
        // object in the range starting at the specified 'first' iterator and
        // ending immediately before the specified 'last' iterator.  The
        // (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type', and 'value_type' must be 'emplace-constructible' from
        // '*i' into this unordered multimap, where 'i' is a dereferenceable
        // iterator in the range '[first .. last)' (see {Requirements on 'KEY'
        // and 'VALUE'}).  The behavior is undefined unless 'first' and 'last'
        // refer to a sequence of valid values where 'first' is at a position
        // at or before 'last'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void insert(std::initializer_list<value_type> values);
        // Insert into this unordered multimap the value of each 'value_type'
        // object in the specified 'values' initializer list.  This method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be 'copy-insertable' into this unordered multimap (see {Requirements
        // on 'KEY' and 'VALUE'}).
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=2
    template <class... Args>
    iterator emplace(Args&&... args);
        // Insert into this unordered multimap a newly created 'value_type'
        // object, constructed by forwarding 'get_allocator()' (if required)
        // and the specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type'.  Return an iterator referring to the
        // newly created and inserted object in this unordered multimap.  This
        // method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be 'emplace-constructible' from 'args' (see
        // {Requirements on 'KEY' and 'VALUE'}).

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);
        // Insert into this unordered multimap a newly created 'value_type'
        // object, constructed by forwarding 'get_allocator()' (if required)
        // and the specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type' (in constant time if the specified
        // 'hint' refers to an element in this container with a key equivalent
        // to the key of the newly created 'value_type' object), and return an
        // iterator referring to the newly created and inserted object.  If
        // 'hint' does not refer to an element in this container with a key
        // equivalent to the key of the newly created 'value_type' object, this
        // operation has worst case 'O[N]' and average case constant-time
        // complexity, where 'N' is the size of this unordered multimap.  This
        // method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be 'emplace-constructible' from 'args' (see
        // {Requirements on 'KEY' and 'VALUE'}).  The behavior is undefined
        // unless 'hint' is an iterator in the range '[begin() .. end()]' (both
        // endpoints included).

#endif

    void max_load_factor(float newLoadFactor);
        // Set the maximum load factor of this container to the specified
        // 'newLoadFactor'.

    void rehash(size_type numBuckets);
        // Change the size of the array of buckets maintained by this container
        // to the specified 'numBuckets', and redistribute all the contained
        // elements into the new sequence of buckets, according to their hash
        // values.  Note that this operation has no effect if rehashing the
        // elements into 'numBuckets' would cause this unordered multimap to
        // exceed its 'max_load_factor'.

    void reserve(size_type numElements);
        // Increase the number of buckets of this unordered multimap to a
        // quantity such that the ratio between the specified 'numElements' and
        // the new number of buckets does not exceed 'max_load_factor'.  Note
        // that this guarantees that, after the reserve, elements can be
        // inserted to grow the container to 'size() == numElements' without
        // rehashing.  Also note that memory allocations may still occur when
        // growing the container to 'size() == numElements'.  Also note that
        // this operation has no effect if 'numElements <= size()'.

    void swap(unordered_multimap& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                     AllocatorTraits::is_always_equal::value &&
                                     bsl::is_nothrow_swappable<HASH>::value &&
                                     bsl::is_nothrow_swappable<EQUAL>::value);
        // Exchange the value, hasher, key-equality functor, and
        // 'max_load_factor' of this object with those of the specified 'other'
        // object; also exchange the allocator of this object with that of
        // 'other' if the (template parameter) type 'ALLOCATOR' has the
        // 'propagate_on_container_swap' trait, and do not modify either
        // allocator otherwise.  This method provides the no-throw
        // exception-safety guarantee if and only if both the (template
        // parameter) types 'HASH' and 'EQUAL' provide no-throw swap
        // operations; if an exception is thrown, both objects are left in
        // valid but unspecified states.  This operation guarantees 'O[1]'
        // complexity.  The behavior is undefined unless either this object was
        // created with the same allocator as 'other' or 'ALLOCATOR' has the
        // 'propagate_on_container_swap' trait.

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return (a copy of) the allocator used for memory allocation by this
        // unordered multimap.

    const_iterator  begin() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects
        // maintained by this unordered multimap, or the 'end' iterator if this
        // unordered multimap is empty.

    const_iterator  end() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end position in the sequence of 'value_type' objects
        // maintained by this unordered multimap.

    bool contains(const key_type &key) const;
        // Return 'true' if this unordered map contains an element whose key is
        // equivalent to the specified 'key'.

    template <class LOOKUP_KEY>
    typename enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value &&
            BloombergLP::bslmf::IsTransparentPredicate<EQUAL,
                                                       LOOKUP_KEY>::value,
        bool>::type
    contains(const LOOKUP_KEY& key) const
        // Return 'true' if this unordered map contains an element whose key is
        // equivalent to the specified 'key'.
        //
        // Note: implemented inline due to Sun CC compilation error
    {
        return find(key) != end();
    }

    bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this unordered multimap contains no elements, and
        // 'false' otherwise.

    size_type size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of elements in this unordered multimap.

    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of elements
        // that this unordered multimap could possibly hold.  Note that there
        // is no guarantee that the unordered multimap can successfully grow to
        // the returned size, or even close to that size without running out of
        // resources.

    EQUAL key_eq() const;
        // Return (a copy of) the key-equivalence binary functor that returns
        // 'true' if the value of two 'key_type' objects are equivalent, and
        // 'false' otherwise.

    HASH hash_function() const;
        // Return (a copy of) the hash unary functor used by this unordered
        // multimap to generate a hash value (of type 'size_type') for a
        // 'key_type' object.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      const_iterator>::type
    find(const LOOKUP_KEY& key) const
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in this unordered multimap whose key is
        // equivalent to the specified 'key', if such an entry exists, and the
        // past-the-end ('end') iterator otherwise.  The behavior is undefined
        // unless 'key' is equivalent to the key of the elements of at most one
        // equivalent-key group in this unordered multimap.
        //
        // Note: implemented inline due to Sun CC compilation error.
        {
            return const_iterator(d_impl.find(key));
        }

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects of this
        // unordered multimap with a key equivalent to the specified 'key', if
        // such entries exist, and the past-the-end ('end') iterator otherwise.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      size_type>::type
    count(const LOOKUP_KEY& key) const
        // Return the number of 'value_type' objects in this unordered multimap
        // with a key equivalent to the specified 'key'.  The behavior is
        // undefined unless 'key' is equivalent to the key of the elements of
        // at most one equivalent-key group in this unordered multimap.
        //
        // Note: implemented inline due to Sun CC compilation error.
        {
            typedef ::BloombergLP::bslalg::BidirectionalNode<value_type> BNode;

            size_type result = 0;
            for (HashTableLink *cursor = d_impl.find(key);
                 cursor;
                 ++result, cursor = cursor->nextLink())
            {
                BNode *cursorNode = static_cast<BNode *>(cursor);
                if (!this->key_eq()(
                         key,
                         ListConfiguration::extractKey(cursorNode->value()))) {

                    break;
                }
            }
            return result;
        }

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects in this unordered multimap
        // with a key equivalent to the specified 'key'.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      pair<const_iterator, const_iterator> >::type
    equal_range(const LOOKUP_KEY& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this unordered multimap with a
        // key equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered
        // multimap contains no 'value_type' objects with a key equivalent to
        // 'key', then the two returned iterators will have the same value.
        // The behavior is undefined unless 'key' is equivalent to the key of
        // the elements of at most one equivalent-key group in this unordered
        // multimap.
        //
        // Note: implemented inline due to Sun CC compilation error.
        {
            typedef bsl::pair<const_iterator, const_iterator> ResultType;
            HashTableLink *first;
            HashTableLink *last;
            d_impl.findRange(&first, &last, key);
            return ResultType(const_iterator(first), const_iterator(last));
        }

    pair<const_iterator, const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this unordered multimap with a
        // key equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered
        // multimap contains no 'value_type' objects with a key equivalent to
        // 'key', then the two returned iterators will have the same value.

    const_local_iterator  begin(size_type index) const;
    const_local_iterator cbegin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects) of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered multimap, or the 'end(index)' iterator
        // if the indexed bucket is empty.  The behavior is undefined unless
        // 'index < bucket_count()'.

    const_local_iterator  end(size_type index) const;
    const_local_iterator cend(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end position (in the sequence of 'value_type' objects) of
        // the bucket having the specified 'index' in the array of buckets
        // maintained by this unordered multimap.  The behavior is undefined
        // unless 'index < bucket_count()'.

    size_type bucket(const key_type& key) const;
        // Return the index of the bucket, in the array of buckets of this
        // container, where a value with a key equivalent to the specified
        // 'key' would be inserted.

    size_type bucket_count() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of buckets in the array of buckets maintained by
        // this unordered multimap.

    size_type max_bucket_count() const BSLS_KEYWORD_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of buckets
        // that this container could possibly manage.  Note that there is no
        // guarantee that the unordered multimap can successfully grow to the
        // returned size, or even close to that size without running out of
        // resources.

    size_type bucket_size(size_type index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this
        // container.  The behavior is undefined unless
        // 'index < bucket_count()'.

    float load_factor() const BSLS_KEYWORD_NOEXCEPT;
        // Return the current ratio between the 'size' of this container and
        // the number of buckets.  The load factor is a measure of how full the
        // container is, and a higher load factor typically leads to an
        // increased number of collisions, thus resulting in a loss of
        // performance.

    float max_load_factor() const BSLS_KEYWORD_NOEXCEPT;
        // Return the maximum load factor allowed for this container.  Note
        // that if an insert operation would cause the load factor to exceed
        // the 'max_load_factor', that same insert operation will increase the
        // number of buckets and rehash the elements of the container into
        // those buckets (see 'rehash').
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template <
    class INPUT_ITERATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class HASH = bsl::hash<KEY>,
    class EQUAL = bsl::equal_to<KEY>,
    class ALLOCATOR = bsl::allocator<pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t<
                         std::is_invocable_v<EQUAL, const KEY &, const KEY &>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multimap(INPUT_ITERATOR,
                   INPUT_ITERATOR,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type = 0,
                   HASH = HASH(),
                   EQUAL = EQUAL(),
                   ALLOCATOR = ALLOCATOR())
-> unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'unordered_multimap'.
    // Deduce the template parameters 'HASH', 'EQUAL' and 'ALLOCATOR' from the
    // other parameters passed to the constructor of 'unordered_multimap'.
    //  This deduction guide does not participate unless the supplied allocator
    // meets the requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class HASH,
    class EQUAL,
    class ALLOC,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multimap(
    INPUT_ITERATOR,
    INPUT_ITERATOR,
    typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
    HASH,
    EQUAL,
    ALLOC *)
-> unordered_multimap<KEY, VALUE, HASH, EQUAL>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'unordered_multimap'.
    // Deduce the template parameters 'HASH' and "EQUAL' from the other
    // parameters passed to the constructor of 'unordered_multimap'.  This
    // deduction guide does not participate unless the supplied allocator is
    // convertible to 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class INPUT_ITERATOR,
    class HASH,
    class ALLOCATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multimap(INPUT_ITERATOR,
                   INPUT_ITERATOR,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type,
                   HASH,
                   ALLOCATOR)
-> unordered_multimap<KEY, VALUE, HASH, bsl::equal_to<KEY>, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'unordered_multimap'.
    // Deduce the template parameters 'HASH' and 'ALLOCATOR' from the other
    // parameters passed to the constructor of 'unordered_multimap'.  This
    // deduction guide does not participate unless the supplied hash is
    // invokable with a 'KEY', and the supplied allocator meets the
    // requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class HASH,
    class ALLOC,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multimap(
    INPUT_ITERATOR,
    INPUT_ITERATOR,
    typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
    HASH,
    ALLOC *)
-> unordered_multimap<KEY, VALUE, HASH>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'unordered_multimap'.
    // Deduce the template parameter 'HASH' from the other parameters passed to
    // the constructor of 'unordered_multimap'.  This deduction guide does not
    // participate unless the supplied allocator is convertible to
    // 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class INPUT_ITERATOR,
    class ALLOCATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multimap(INPUT_ITERATOR,
              INPUT_ITERATOR,
              typename bsl::allocator_traits<ALLOCATOR>::size_type,
              ALLOCATOR)
-> unordered_multimap<KEY,
                      VALUE,
                      bsl::hash<KEY>,
                      bsl::equal_to<KEY>,
                      ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'unordered_multimap'.
    // This deduction guide does not participate unless the supplied allocator
    // meets the requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class ALLOC,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multimap(
    INPUT_ITERATOR,
    INPUT_ITERATOR,
    typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
    ALLOC *)
-> unordered_multimap<KEY, VALUE>;
    // of the iterators supplied to the constructor of 'unordered_multimap'.
    // Deduce the template parameter 'ALLOCATOR' from the other parameter
    // passed to the constructor of 'unordered_multimap'.  This deduction guide
    // does not participate unless the supplied allocator meets the
    // requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class ALLOCATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multimap(INPUT_ITERATOR, INPUT_ITERATOR, ALLOCATOR)
-> unordered_multimap<KEY,
                      VALUE,
                      bsl::hash<KEY>,
                      bsl::equal_to<KEY>,
                      ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'unordered_multimap'.
    // Deduce the template parameter 'ALLOCATOR' from the other parameter
    // passed to the constructor of 'unordered_multimap'.  This deduction guide
    // does not participate unless the supplied allocator meets the
    // requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class ALLOC,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multimap(INPUT_ITERATOR, INPUT_ITERATOR, ALLOC *)
-> unordered_multimap<KEY, VALUE>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'unordered_multimap'.
    // This deduction guide does not participate unless the supplied allocator
    // is convertible to 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class KEY,
    class VALUE,
    class HASH = bsl::hash<KEY>,
    class EQUAL = bsl::equal_to<KEY>,
    class ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t<
                         std::is_invocable_v<EQUAL, const KEY &, const KEY &>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multimap(std::initializer_list<bsl::pair<const KEY, VALUE>>,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type = 0,
                   HASH      = HASH(),
                   EQUAL     = EQUAL(),
                   ALLOCATOR = ALLOCATOR())
-> unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of
    // 'unordered_multimap'.  Deduce the template parameters 'HASH', 'EQUAL'
    // and 'ALLOCATOR' from the other parameters supplied to the constructor of
    // 'unordered_multimap'.  This deduction guide does not participate unless:
    // (1) the supplied 'HASH' is invokable with a 'KEY', (2) the supplied
    // 'EQUAL' is invokable with two 'KEY's, and (3) the supplied allocator
    // meets the requirements of a standard allocator.

template <
    class KEY,
    class VALUE,
    class HASH,
    class EQUAL,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multimap(
    std::initializer_list<bsl::pair<const KEY, VALUE>>,
    typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
    HASH,
    EQUAL,
    ALLOC *)
-> unordered_multimap<KEY, VALUE, HASH, EQUAL>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of
    // 'unordered_multimap'.  Deduce the template parameters 'HASH', 'EQUAL'
    // and 'ALLOCATOR' from the other parameters supplied to the constructor of
    // 'unordered_multimap'. This deduction guide does not participate unless
    // the supplied allocator is convertible to
    // 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class KEY,
    class VALUE,
    class HASH,
    class ALLOCATOR,
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multimap(std::initializer_list<bsl::pair<const KEY, VALUE>>,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type,
                   HASH,
                   ALLOCATOR)
-> unordered_multimap<KEY, VALUE, HASH, bsl::equal_to<KEY>, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of
    // 'unordered_multimap'.  Deduce the template parameters 'HASH' and
    // 'ALLOCATOR' from the other parameters supplied to the constructor of
    // 'unordered_multimap'.  This deduction guide does not participate unless
    // the supplied 'HASH' is invokable with a 'KEY', and the supplied
    // allocator meets the requirements of a standard allocator.

template <
    class KEY,
    class VALUE,
    class HASH,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multimap(
    std::initializer_list<bsl::pair<const KEY, VALUE>>,
    typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
    HASH,
    ALLOC *)
-> unordered_multimap<KEY, VALUE, HASH>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of
    // 'unordered_multimap'.  Deduce the template parameter 'HASH' from the
    // other parameters supplied to the constructor of 'unordered_multimap'.
    // This deduction guide does not participate unless the supplied allocator
    // is convertible to 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class KEY,
    class VALUE,
    class ALLOCATOR,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multimap(std::initializer_list<bsl::pair<const KEY, VALUE>>,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type,
                   ALLOCATOR)
-> unordered_multimap<KEY,
                      VALUE,
                      bsl::hash<KEY>,
                      bsl::equal_to<KEY>,
                      ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of
    // 'unordered_multimap'.  This deduction guide does not participate unless
    // the supplied allocator meets the requirements of a standard allocator.

template <
    class KEY,
    class VALUE,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multimap(
    std::initializer_list<bsl::pair<const KEY, VALUE>>,
    typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
    ALLOC *)
-> unordered_multimap<KEY, VALUE>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of
    // 'unordered_multimap'.  This deduction guide does not participate unless
    // the supplied allocator is convertible to
    // 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class KEY,
    class VALUE,
    class ALLOCATOR,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multimap(std::initializer_list<bsl::pair<const KEY, VALUE>>,
                   ALLOCATOR)
-> unordered_multimap<KEY,
                      VALUE,
                      bsl::hash<KEY>,
                      bsl::equal_to<KEY>,
                      ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of
    // 'unordered_multimap'.  Deduce the template parameter 'ALLOCATOR' from
    // the other parameters supplied to the constructor of
    // 'unordered_multimap'.  This deduction guide does not participate unless
    // the supplied allocator meets the requirements of a standard allocator.

template <
    class KEY,
    class VALUE,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multimap(std::initializer_list<bsl::pair<const KEY, VALUE>>, ALLOC *)
-> unordered_multimap<KEY, VALUE>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of
    // 'unordered_multimap'.  This deduction guide does not participate unless
    // the supplied allocator is convertible to
    // 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.
#endif

// FREE OPERATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool operator==(
            const unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
            const unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'unordered_multimap' objects have the
    // same value if they have the same number of key-value pairs, and each
    // key-value pair that is contained in one of the objects is also contained
    // in the other object.  This method requires that the (template parameter)
    // types 'KEY' and 'VALUE' both be 'equality-comparable' (see {Requirements
    // on 'KEY' and 'VALUE'}).

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(
            const unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
            const unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'unordered_multimap' objects do
    // not have the same value if they do not have the same number of key-value
    // pairs, or some key-value pair that is contained in one of the objects is
    // not also contained in the other object.  This method requires that the
    // (template parameter) types 'KEY' and 'VALUE' both be
    // 'equality-comparable' (see {Requirements on 'KEY' and 'VALUE'}).
#endif

// FREE FUNCTIONS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
void swap(unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& a,
          unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& b)
                                    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false);
    // Exchange the value, hasher, key-equality functor, and 'max_load_factor'
    // of the specified 'a' object with those of the specified 'b' object; also
    // exchange the allocator of 'a' with that of 'b' if the (template
    // parameter) type 'ALLOCATOR' has the 'propagate_on_container_swap' trait,
    // and do not modify either allocator otherwise.  This function provides
    // the no-throw exception-safety guarantee if and only if both the
    // (template parameter) types 'HASH' and 'EQUAL' provide no-throw swap
    // operations; if an exception is thrown, both objects are left in valid
    // but unspecified states.  This operation guarantees 'O[1]' complexity.
    // The behavior is undefined unless either 'a' was created with the same
    // allocator as 'b' or 'ALLOCATOR' has the 'propagate_on_container_swap'
    // trait.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //-------------------------
                        // class unordered_multimap
                        //-------------------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap()
: d_impl(HASH(), EQUAL(), 0, 1.0f, ALLOCATOR())
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                            size_type        initialNumBuckets,
                                            const ALLOCATOR& basicAllocator)
: d_impl(HASH(), EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                               const ALLOCATOR& basicAllocator)
: d_impl(basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                            const unordered_multimap& original)
: d_impl(original.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                     original.get_allocator()))
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                   BloombergLP::bslmf::MovableRef<unordered_multimap> original)
: d_impl(MoveUtil::move(MoveUtil::access(original).d_impl))
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                 const unordered_multimap&                      original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_impl(original.d_impl, basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
             BloombergLP::bslmf::MovableRef<unordered_multimap> original,
             const typename type_identity<ALLOCATOR>::type&     basicAllocator)
: d_impl(MoveUtil::move(MoveUtil::access(original).d_impl), basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                            INPUT_ITERATOR   first,
                                            INPUT_ITERATOR   last,
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                            INPUT_ITERATOR   first,
                                            INPUT_ITERATOR   last,
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                            INPUT_ITERATOR   first,
                                            INPUT_ITERATOR   last,
                                            size_type        initialNumBuckets,
                                            const ALLOCATOR& basicAllocator)
: d_impl(HASH(), EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                               INPUT_ITERATOR   first,
                                               INPUT_ITERATOR   last,
                                               const ALLOCATOR& basicAllocator)
: d_impl(HASH(), EQUAL(), 0, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
template <class, class, class>
# endif
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                           std::initializer_list<value_type> values,
                           size_type                         initialNumBuckets,
                           const HASH&                       hashFunction,
                           const EQUAL&                      keyEqual,
                           const ALLOCATOR&                  basicAllocator)
: unordered_multimap(values.begin(),
                     values.end(),
                     initialNumBuckets,
                     hashFunction,
                     keyEqual,
                     basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
template <class, class>
# endif
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                           std::initializer_list<value_type> values,
                           size_type                         initialNumBuckets,
                           const HASH&                       hashFunction,
                           const ALLOCATOR&                  basicAllocator)
: unordered_multimap(values.begin(),
                     values.end(),
                     initialNumBuckets,
                     hashFunction,
                     EQUAL(),
                     basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
template <class>
# endif
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                          std::initializer_list<value_type> values,
                          size_type                         initialNumBuckets,
                          const ALLOCATOR&                  basicAllocator)
: unordered_multimap(values.begin(),
                     values.end(),
                     initialNumBuckets,
                     HASH(),
                     EQUAL(),
                     basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
template <class>
# endif
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                          std::initializer_list<value_type> values,
                          const ALLOCATOR&                  basicAllocator)
: unordered_multimap(values.begin(),
                     values.end(),
                     0,
                     HASH(),
                     EQUAL(),
                     basicAllocator)
{
}
#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::~unordered_multimap()
{
    // All memory management is handled by the base 'd_impl' member.
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>&
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator=(
                                                 const unordered_multimap& rhs)
{
    // Note that we have delegated responsibility for correct handling of
    // allocator propagation to the 'HashTable' implementation.

    d_impl = rhs.d_impl;

    return *this;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>&
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator=(
                        BloombergLP::bslmf::MovableRef<unordered_multimap> rhs)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                AllocatorTraits::is_always_equal::value &&
                                std::is_nothrow_move_assignable<HASH>::value &&
                                std::is_nothrow_move_assignable<EQUAL>::value)
{
    // Note that we have delegated responsibility for correct handling of
    // allocator propagation to the 'HashTable' implementation.

    unordered_multimap& lvalue = rhs;

    d_impl = MoveUtil::move(lvalue.d_impl);

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>&
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator=(
                                      std::initializer_list<value_type> values)
{
    unordered_multimap tmp(values.begin(), values.end(), d_impl.allocator());

    d_impl.swap(tmp.d_impl);

    return *this;
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class... Args>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::emplace(Args&&... args)
{
    return iterator(d_impl.emplace(
                                BSLS_COMPILERFEATURES_FORWARD(Args, args)...));

}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class... Args>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::emplace_hint(
                                                           const_iterator hint,
                                                           Args&&...      args)
{
    return iterator(d_impl.emplaceWithHint(hint.node(),
                                BSLS_COMPILERFEATURES_FORWARD(Args, args)...));
}
#endif

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin()
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end()
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(0, &d_impl.bucketAtIndex(index));
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::clear()
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_impl.removeAll();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::contains(
                                                     const key_type& key) const
{
    return find(key) != end();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::find(
                                                           const key_type& key)
{
    return iterator(d_impl.find(key));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bsl::pair<
     typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator,
     typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                           const key_type& key)
{
    HashTableLink *first;
    HashTableLink *last;
    d_impl.findRange(&first, &last, key);
    return bsl::pair<iterator, iterator>(iterator(first), iterator(last));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(
                                                       const_iterator position)
{
    BSLS_ASSERT(position != this->end());

    return iterator(d_impl.remove(position.node()));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(
                                                             iterator position)
{
    return erase(const_iterator(position));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(
                                                           const key_type& key)
{   // As an alternative implementation, the table could return an extracted
    // "slice" list from the underlying table, and now need merely:
    //   iterate each node, destroying the associated value
    //   reclaim each node (potentially returning to a node-pool)

    typedef ::BloombergLP::bslalg::BidirectionalNode<value_type> BNode;

    if (HashTableLink *target = d_impl.find(key)) {
        target = d_impl.remove(target);
        size_type result = 1;
        while (target &&
               this->key_eq()(key, ListConfiguration::extractKey(
                                    static_cast<BNode *>(target)->value()))) {
            target = d_impl.remove(target);
            ++result;
        }
        return result;                                                // RETURN
    }

    return 0;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(
                                                          const_iterator first,
                                                          const_iterator last)
{
#if defined BDE_BUILD_TARGET_SAFE_2
    if (first != last) {
        iterator it        = this->begin();
        const iterator end = this->end();
        for (; it != first; ++it) {
            BSLS_ASSERT(last != it);
            BSLS_ASSERT(end  != it);
        }
        for (; it != last; ++it) {
            BSLS_ASSERT(end  != it);
        }
    }
#endif

    while (first != last) {
        first = this->erase(first);
    }

    return iterator(first.node());          // convert from const_iterator
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                       const value_type& value)
{
    return iterator(d_impl.insert(value));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                       const_iterator    hint,
                                                       const value_type& value)
{
    return iterator(d_impl.insert(value, hint.node()));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                          INPUT_ITERATOR first,
                                                          INPUT_ITERATOR last)
{
    difference_type maxInsertions =
              ::BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);
    if (0 < maxInsertions) {
        this->reserve(this->size() + maxInsertions);
    }
    else {
        BSLS_ASSERT_SAFE(0 == maxInsertions);
    }

    while (first != last) {
        d_impl.emplace(*first);
        ++first;
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                      std::initializer_list<value_type> values)
{
    insert(values.begin(), values.end());
}
#endif

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_load_factor(
                                                           float newLoadFactor)
{
    d_impl.setMaxLoadFactor(newLoadFactor);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::rehash(
                                                          size_type numBuckets)
{
    d_impl.rehashForNumBuckets(numBuckets);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::reserve(
                                                         size_type numElements)
{
    d_impl.reserveForNumElements(numElements);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::swap(
                                                     unordered_multimap& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                     AllocatorTraits::is_always_equal::value &&
                                     bsl::is_nothrow_swappable<HASH>::value &&
                                     bsl::is_nothrow_swappable<EQUAL>::value)
{
    d_impl.swap(other.d_impl);
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
ALLOCATOR
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::get_allocator() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.allocator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cbegin() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cend() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::
                                                           const_local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cbegin(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::
                                                           const_local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cend(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket(
                                                     const key_type& key) const
{
    return d_impl.bucketIndexForKey(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket_count() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.numBuckets();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket_size(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return d_impl.countElementsInBucket(index);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>:: size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::count(
                                                     const key_type& key) const
{
    typedef ::BloombergLP::bslalg::BidirectionalNode<value_type> BNode;

    size_type result = 0;
    for (HashTableLink *cursor = d_impl.find(key);
         cursor;
         ++result, cursor = cursor->nextLink())
    {
        BNode *cursorNode = static_cast<BNode *>(cursor);
        if (!this->key_eq()(key,
                         ListConfiguration::extractKey(cursorNode->value()))) {

            break;
        }
    }
    return  result;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::find(
                                                     const key_type& key) const
{
    return const_iterator(d_impl.find(key));
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::empty() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_impl.size();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.size();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_size() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return AllocatorTraits::max_size(get_allocator());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::hasher
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::key_equal
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::key_eq() const
{
    return d_impl.comparator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bsl::pair<typename unordered_multimap<KEY,
                                      VALUE,
                                      HASH,
                                      EQUAL,
                                      ALLOCATOR>::const_iterator,
          typename unordered_multimap<KEY,
                                      VALUE,
                                      HASH,
                                      EQUAL,
                                      ALLOCATOR>::const_iterator>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                     const key_type& key) const
{
    HashTableLink *first;
    HashTableLink *last;
    d_impl.findRange(&first, &last, key);
    return bsl::pair<const_iterator, const_iterator>(const_iterator(first),
                                                     const_iterator(last));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>:: max_bucket_count()
                                                                          const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.maxNumBuckets();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::load_factor()
                                                                          const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.loadFactor();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_load_factor()
                                                                          const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.maxLoadFactor();
}

}  // close namespace bsl

// FREE OPERATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator==(
        const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
        const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return lhs.d_impl == rhs.d_impl;
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator!=(
        const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
        const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}
#endif

// FREE FUNCTIONS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void bsl::swap(bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& a,
               bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& b)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *unordered* *associative* containers:
//: o An unordered associative container defines STL iterators.
//: o An unordered associative container is bitwise movable if both functors
//:   and the allocator are bitwise movable.
//: o An unordered associative container uses 'bslma' allocators if the
//:   (template parameter) type 'ALLOCATOR' is convertible from
//:   'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
struct HasStlIterators<bsl::unordered_multimap<KEY,
                                               VALUE,
                                               HASH,
                                               EQUAL,
                                               ALLOCATOR> >
: bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::unordered_multimap<KEY,
                                                  VALUE,
                                                  HASH,
                                                  EQUAL,
                                                  ALLOCATOR> >
: bsl::is_convertible<Allocator*, ALLOCATOR>::type
{};

}  // close namespace bslma

namespace bslmf {

template <class KEY, class MAPPED, class HASH, class EQUAL, class ALLOCATOR>
struct IsBitwiseMoveable<
    bsl::unordered_multimap<KEY, MAPPED, HASH, EQUAL, ALLOCATOR> >
    : ::BloombergLP::bslmf::IsBitwiseMoveable<BloombergLP::bslstl::HashTable<
          ::BloombergLP::bslstl::
              UnorderedMapKeyConfiguration<KEY, bsl::pair<const KEY, MAPPED> >,
          HASH,
          EQUAL,
          ALLOCATOR> >::type
{};

}
}  // close enterprise namespace

#endif // End C++11 code

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
