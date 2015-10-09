// bslstl_equalto.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_EQUALTO
#define INCLUDED_BSLSTL_EQUALTO

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a binary functor conforming to the C++11 'equal_to' spec.
//
//@CLASSES:
//  equal_to: C++11-compliant binary functor applying 'operator=='
//
//@SEE_ALSO: bslstl_unorderedmap, bslstl_unorderedset
//
//@DESCRIPTION: This component provides the C+11 standard binary comparison
// functor, 'bsl::equal_to', that evaluates equality of two 'VALUE_TYPE'
// objects through the 'operator=='.  The application of the functor to two
// different objects 'o1' and 'o2' returns true if 'o1 == o2'.  Note that this
// the for use as keys in the standard unordered associative containers such as
// 'bsl::unordered_map' and 'bsl::unordered_set'.  Also note that this class is
// an empty POD type.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a List Set
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to keep a set of a small number of elements, and the only
// comparison operation we have on the type of the elements is an equality
// operator.  We can keep a singly-linked list of the elements, and
// exhaustively use the comparison operator to see if a given value exists in
// the list, forming a primitive set.
//
// First, we define our 'ListSet' template class:
//..
//  template <typename TYPE, typename EQUALS = bsl::equal_to<TYPE> >
//  class ListSet {
//      // This class implements a crude implementation of a set, that will
//      // keep a set of values and be able to determine if an element is a
//      // member of the set.  Unlike a 'bsl::set' or 'bsl::unordered_set', no
//      // hash function or transitive 'operator<' is required -- only a
//      // transitive 'EQUALS' operator.
//      //
//      // The 'TYPE' template parameter must have a public copy constructor
//      // and destructor available.
//      //
//      // The 'EQUALS' template parameter must a function with a function
//      // whose signature is
//      //..
//      //  bool operator()(const TYPE& lhs, const TYPE& rhs) const;
//      //..
//      // and which returns 'true' if 'lhs' and 'rhs' are equivalent and
//      // 'false' otherwise.  This equivalence relation must be transitive and
//      // symmetric.  The comparator must have a default constructor and
//      // destructor which are public.
//
//      // PRIVATE TYPES
//      struct Node {
//          TYPE  d_value;
//          Node *d_next;
//      };
//
//      // DATA
//      EQUALS            d_comparator;
//      Node             *d_nodeList;
//      bslma::Allocator *d_allocator_p;
//
//    private:
//      // NOT IMPLEMENTED
//      ListSet(const ListSet&);
//      ListSet& operator=(const ListSet&);
//
//    public:
//      // CREATORS
//      explicit
//      ListSet(bslma::Allocator *allocator = 0)
//      : d_comparator()
//      , d_nodeList(0)
//      , d_allocator_p(bslma::Default::allocator(allocator))
//          // Create an empty "ListSet' using the specified 'allocator', or
//          // the default allocator if none is specified.
//      {}
//
//      ~ListSet()
//          // Release all memory used by this 'ListSet'
//      {
//          for (Node *node = d_nodeList; node; ) {
//              Node *toDelete = node;
//              node = node->d_next;
//
//              d_allocator_p->deleteObject(toDelete);
//          }
//      }
//
//      // MANIPULATOR
//      bool insert(const TYPE& value)
//          // If 'value' isn't contained in this 'ListSet', add it and return
//          // 'true', otherwise, return 'false' with no change to the
//          // 'ListSet'.
//      {
//          if (count(value)) {
//              return false;                                         // RETURN
//          }
//
//          Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
//          bslalg::ScalarPrimitives::copyConstruct(&node->d_value,
//                                                  value,
//                                                  d_allocator_p);
//          node->d_next = d_nodeList;
//          d_nodeList = node;
//
//          return true;
//      }
//
//      int count(const TYPE& value) const
//          // Return the number of nodes whose 'd_value' field is equivalent
//          // to the specified 'value', which will always be 0 or 1.
//      {
//          for (Node *node = d_nodeList; node; node = node->d_next) {
//              if (d_comparator(node->d_value, value)) {
//                  return 1;                                         // RETURN
//              }
//          }
//
//          return 0;
//      }
//  };
//..
// Then, in 'main', we declare an instance of 'ListSet' storing 'int's.  The
// default definition of 'bsl::equal_to' will work nicely:
//..
//  ListSet<int> lsi;
//..
// Now, we insert several values into our 'ListSet'.  Note that successful
// insertions return 'true' while redundant ones return 'false' with no effect:
//..
//  assert(true  == lsi.insert( 5));
//  assert(false == lsi.insert( 5));
//  assert(false == lsi.insert( 5));
//  assert(true  == lsi.insert(11));
//  assert(true  == lsi.insert(17));
//  assert(true  == lsi.insert(81));
//  assert(true  == lsi.insert(32));
//  assert(false == lsi.insert(17));
//..
// Finally, we observe that our 'count' method successfully distinguishes
// between values that have been stored in our 'ListSet' and those that
// haven't:
//..
//  assert(0 == lsi.count( 7));
//  assert(1 == lsi.count( 5));
//  assert(0 == lsi.count(13));
//  assert(1 == lsi.count(11));
//  assert(0 == lsi.count(33));
//  assert(1 == lsi.count(32));
//..
//
///Example 2: Using Our List Set For a Custom Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to have a list set containing objects of a custom type.  We
// can specialize the 'equal_to' comparator to also work on our custom type
// as well.  We will re-use the 'ListSet' template class from example 1, and
// create a new custom type.
//
// First, we define a type 'StringThing', which will contain a 'const char *'
// pointer, it will be a very simple type, that is implicitly castable to or
// from a 'const char *'.  It has no 'operator==' defined, so 'equal_to' will
// need to be explicitly specialized for it:
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
//..
// Then, we specialize 'equal_to' to be able to compare two 'StringThing's.
//..
//  namespace bsl {
//
//  template <>
//  struct equal_to<StringThing> {
//      bool operator()(const StringThing& lhs,
//                      const StringThing& rhs) const
//      {
//          return !strcmp(lhs, rhs);
//      }
//  };
//
//  }  // close namespace bsl
//..
// Next, in 'main', we declare a 'ListSet' containing 'StringThing's:
//..
//  ListSet<StringThing> lsst;
//..
// Then, we insert a number of values, and observe that redundant inserts
// return 'false' with no effect:
//..
//  assert(true  == lsst.insert("woof"));
//  assert(true  == lsst.insert("meow"));
//  assert(true  == lsst.insert("arf"));
//  assert(false == lsst.insert("woof"));
//  assert(true  == lsst.insert("bark"));
//  assert(false == lsst.insert("meow"));
//  assert(false == lsst.insert("woof"));
//..
// Now, we observe that our 'count' method successfully distinguishes between
// values that have been stored in 'lsst' and those that haven't:
//..
//  assert(1 == lsst.count("meow"));
//  assert(0 == lsst.count("woo"));
//  assert(1 == lsst.count("woof"));
//  assert(1 == lsst.count("arf"));
//  assert(0 == lsst.count("chomp"));
//..
// Finally, we copy values into a buffer and observe that this makes no
// difference to 'count's results:
//..
//  char buffer[10];
//  strcpy(buffer, "meow");
//  assert(1 == lsst.count(buffer));
//  strcpy(buffer, "bite");
//  assert(0 == lsst.count(buffer));
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_equalto.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#include <bslmf_istriviallydefaultconstructible.h>
#endif

namespace bsl
{

                       // ===============
                       // struct equal_to
                       // ===============

template<class VALUE_TYPE>
struct equal_to {
    // This 'struct' defines a binary comparison functor applying 'operator=='
    // to two 'VALUE_TYPE' objects.  This class conforms to the C++11 standard
    // specification of 'std::equal_to' that does not require inheriting from
    // 'std::binary_function'.  Note that this class is an empty POD type.

    // PUBLIC TYPES -- STANDARD TYPEDEFS
    typedef VALUE_TYPE first_argument_type;
    typedef VALUE_TYPE second_argument_type;
    typedef bool       result_type;

    //! equal_to() = default;
        // Create a 'equal_to' object.

    //! equal_to(const equal_to& original) = default;
        // Create a 'equal_to' object.  Note that as
        // 'equal_to' is an empty (stateless) type, this operation
        // will have no observable effect.

    //! ~equal_to() = default;
        // Destroy this object.

    // MANIPULATORS
    //! equal_to& operator=(const equal_to&) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // a return a reference providing modifiable access to this object.
        // Note that as 'equal_to' is an empty (stateless) type,
        // this operation will have no observable effect.

    // ACCESSORS
    bool operator()(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs) const;
        // Return 'true' if the specified 'lhs' compares equal to the specified
        // 'rhs' using the equality-comparison operator, 'lhs == rhs'.
};

}  // close namespace bsl

namespace bsl {

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // --------------------
                       // struct bsl::equal_to
                       // --------------------

// ACCESSORS
template<class VALUE_TYPE>
inline
bool equal_to<VALUE_TYPE>::operator()(const VALUE_TYPE& lhs,
                                      const VALUE_TYPE& rhs) const
{
    return lhs == rhs;
}

}  // close namespace bsl

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for 'equal_to'
//: o 'equal_to' is a stateless POD, trivially constructible, copyable, and
//:   moveable.

namespace bsl {

template<class VALUE_TYPE>
struct is_trivially_default_constructible<equal_to<VALUE_TYPE> >
: bsl::true_type
{};

template<class VALUE_TYPE>
struct is_trivially_copyable<equal_to<VALUE_TYPE> >
: bsl::true_type
{};

}  // close namespace bsl

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
