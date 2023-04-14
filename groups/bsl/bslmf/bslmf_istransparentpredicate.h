// bslmf_istransparentpredicate.h                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_ISTRANSPARENTPREDICATE
#define INCLUDED_BSLMF_ISTRANSPARENTPREDICATE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Support detection of whether a predicate functor is transparent.
//
//@CLASSES:
//  bslmf::IsTransparentPredicate: Detects 'is_transparent'
//
//@SEE_ALSO: bslstl_map, bslstl_set
//
//@DESCRIPTION: This component provides a metafunction,
// 'bslmf::IsTransparentPredicate', that can be used to detect whether a
// comparator is transparent (supports heterogeneous comparisons).  If the
// comparator has a nested type named 'is_transparent', the template inherits
// from 'true_type', otherwise it inherits from 'false_type'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Specifying Behavior Of Comparator
/// - - - - - - - - - - - - - - - - - - - - - -
// In this example, we demonstrate the use of 'IsTransparentPredicate' to
// determine different comparator's behavior.  Our goal is to create an
// overload of an associative container's method that participates in overload
// resolution only if the comparator is transparent, otherwise we fall back on
// a default behavior.
//
// First, we define simple container, 'Vector', that is used as a foundation
// for our associative container:
//..
//  template <class TYPE>
//  class Vector {
//      // DATA
//      const TYPE * d_begin_p;   // pointer to the beginning
//      const TYPE * d_end_p;     // pointer to the end
//
//    public:
//      // CREATORS
//      Vector(const TYPE *first, const TYPE *last)
//          // Construct an object that references the specified 'first' and
//          // 'last' items in the sequence.
//      : d_begin_p(first)
//      , d_end_p(last)
//      {
//      }
//
//      // ACCESSORS
//      const TYPE *begin() const
//          // Return a reference providing non-modifiable access to the first
//          // object in the underlying sequence.
//      {
//          return d_begin_p;
//      }
//
//      const TYPE *end() const
//          // Return a reference providing non-modifiable access to the last
//          // object in the underlying sequence.
//      {
//          return d_end_p;
//      }
//  };
//..
// Then we define simple type, 'String', that is used as a key.  Note that we
// have comparison operators that allow to compare both two 'String' objects
// and 'String' object with character sequence.
//..
//  class String {
//      // CLASS DATA
//      static int  s_numObjectsCreated;  // total number of created instances
//
//      // DATA
//      const char *d_data_p;             // reference to a string
//
//    public:
//      // CLASS METHODS
//      static int numObjectsCreated()
//          // Return the total number of created instances of class 'String'.
//      {
//          return s_numObjectsCreated;
//      }
//
//      // CREATORS
//      String()
//          // Construct an empty string
//      : d_data_p("")
//      {
//          ++s_numObjectsCreated;
//      }
//
//      String(const char *data)  // IMPLICIT
//          // Construct a string that references the specified 'data'.  The
//          // behavior is undefined unless 'data' points to a null-terminated
//          // string.
//      : d_data_p(data)
//      {
//          ++s_numObjectsCreated;
//      }
//
//      // FREE FUNCTIONS
//      friend bool operator<(const String& lhs, const String& rhs)
//          // Return 'true' if the value of the specified 'lhs' string is
//          // lexicographically less than that of the specified 'rhs' string,
//          // and 'false' otherwise.
//      {
//          const char *lhsData = lhs.d_data_p;
//          const char *rhsData = rhs.d_data_p;
//          while (true) {
//              if (*lhsData < *rhsData) {
//                  return true;                                      // RETURN
//              }
//              else if (*lhsData > *rhsData || *lhsData == '\0') {
//                  return false;                                     // RETURN
//              }
//              ++lhsData;
//              ++rhsData;
//          }
//      }
//
//      friend bool operator<(const String& lhs, const char *rhs)
//          // Return 'true' if the value of the specified 'lhs' string is
//          // lexicographically less than the specified 'rhs' character
//          // sequence, and 'false' otherwise.
//      {
//          const char *lhsData = lhs.d_data_p;
//          while (true) {
//              if (*lhsData < *rhs) {
//                  return true;                                      // RETURN
//              }
//              else if (*lhsData > *rhs || *lhsData == '\0') {
//                  return false;                                     // RETURN
//              }
//              ++lhsData;
//              ++rhs;
//          }
//      }
//
//      friend bool operator<(const char *lhs, const String& rhs)
//          // Return 'true' if the specified 'lhs' character sequence is
//          // lexicographically less than the value of the specified 'rhs'
//          // string, and 'false' otherwise.
//      {
//          const char *rhsData = rhs.d_data_p;
//          while (true) {
//              if (*lhs < *rhsData) {
//                  return true;                                      // RETURN
//              }
//              else if (*lhs > *rhsData || *lhs == '\0') {
//                  return false;                                     // RETURN
//              }
//              ++lhs;
//              ++rhsData;
//          }
//      }
//  };
//
//  int String::s_numObjectsCreated = 0;
//..
// Next we define our associative container.  Note that we use
// 'IsTransparentPredicate' for a method with 'enable_if' in the return type.
// This adds our function template into the function overload resolution set if
// and only if the comparison function object is considered transparent, using
// a technique known as "SFINAE".
//..
//  template <class t_COMPARATOR>
//  class FlatStringSet {
//
//      // DATA
//      t_COMPARATOR   d_comparator;  // comparison object
//      Vector<String> d_data;        // stores the data
//
//    public:
//      // TYPES
//      typedef const String * const_iterator;
//
//      // CREATORS
//      template <class INPUT_ITERATOR>
//      FlatStringSet(INPUT_ITERATOR    first,
//                    INPUT_ITERATOR    last,
//                    const t_COMPARATOR& comparator = t_COMPARATOR());
//          // Create a set, and insert each 'String' object in the sequence
//          // starting at the specified 'first' element, and ending
//          // immediately before the specified 'last' element, ignoring those
//          // keys having a value equivalent to that which appears earlier in
//          // the sequence.  Optionally specify a 'comparator' used to order
//          // keys contained in this object.  If 'comparator' is not supplied,
//          // a default-constructed object of the (template parameter) type
//          // 't_COMPARATOR' is used.  This operation has 'O[N]' complexity,
//          // where 'N' is the number of elements between 'first' and 'last'.
//          // The (template parameter) type 'INPUT_ITERATOR' shall meet the
//          // requirements of an input iterator defined in the C++11 standard
//          // [24.2.3] providing access to values of a type convertible to
//          // 'String', and 'String' must be 'emplace-constructible' from '*i'
//          // into this set, where 'i' is a dereferenceable iterator in the
//          // range '[first .. last)'.  The behavior is undefined unless
//          // 'first' and 'last' refer to a sequence of valid values where
//          // 'first' is at a position at or before 'last', the range is
//          // ordered according to 't_COMPARATOR', and there are no duplicates
//          // in the range.
//
//      // ACCESSORS
//      const_iterator begin() const;
//          // Return an iterator providing non-modifiable access to the first
//          // 'String' object in the ordered sequence of 'String' objects
//          // maintained by this set, or the 'end' iterator if this set is
//          // empty.
//
//      const_iterator end() const;
//          // Return an iterator providing non-modifiable access to the
//          // past-the-end element in the ordered sequence of 'String' objects
//          // maintained by this set.
//
//      const_iterator find(const String& key) const
//          // Return an iterator to the element that compares equal to the
//          // specified 'key' if such an element exists, and an end iterator
//          // otherwise.
//          //
//          // Note: implemented inline due to Sun CC compilation error.
//      {
//          for (const_iterator first = begin(); first != end(); ++first) {
//              if (d_comparator(key, *first)) {
//                  return end();                                     // RETURN
//              }
//              if (!d_comparator(*first, key)) {
//                  return first;                                     // RETURN
//              }
//          }
//          return end();
//      }
//
//      template <class t_KEY>
//      typename bsl::enable_if<IsTransparentPredicate<t_COMPARATOR,
//                                                     t_KEY>::value,
//                              const_iterator>::type
//      find(const t_KEY& key) const
//          // Return an iterator to the element that compares equal to the
//          // specified 'key' if such an element exists, and an end iterator
//          // otherwise.
//          //
//          // Note: implemented inline due to Sun CC compilation error.
//      {
//          for (const_iterator first = begin(); first != end(); ++first) {
//              if (d_comparator(key, *first)) {
//                  return end();                                     // RETURN
//              }
//              if (!d_comparator(*first, key)) {
//                  return first;                                     // RETURN
//              }
//          }
//          return end();
//      }
//
//      int size() const;
//          // Return the number of elements in this container.
//  };
//
//  // CREATORS
//  template <class t_COMPARATOR>
//  template <class INPUT_ITERATOR>
//  FlatStringSet<t_COMPARATOR>::FlatStringSet(INPUT_ITERATOR    first,
//                                           INPUT_ITERATOR    last,
//                                           const t_COMPARATOR& comparator)
//  : d_comparator(comparator),
//    d_data(first, last)
//  {
//  }
//
//  // ACCESSORS
//  template <class t_COMPARATOR>
//  typename FlatStringSet<t_COMPARATOR>::const_iterator
//  FlatStringSet<t_COMPARATOR>::begin() const
//  {
//      return d_data.begin();
//  }
//
//  template <class t_COMPARATOR>
//  typename FlatStringSet<t_COMPARATOR>::const_iterator
//  FlatStringSet<t_COMPARATOR>::end() const
//  {
//      return d_data.end();
//  }
//
//  template <class t_COMPARATOR>
//  int FlatStringSet<t_COMPARATOR>::size() const
//  {
//      return static_cast<int>(end() - begin());
//  }
//..
// Then we define two comparators.  These classes are completely identical
// except that one defines an 'is_transparent' type, and the other does not.
//..
//  struct TransparentComp
//      // This class can be used as a comparator for containers.  It has a
//      // nested type 'is_transparent', so it is classified as transparent by
//      // the 'bslmf::IsTransparentPredicate' metafunction and can be used for
//      // heterogeneous comparison.
//  {
//      // TYPES
//      typedef void is_transparent;
//
//      // ACCESSORS
//      template <class LHS, class RHS>
//      bool operator()(const LHS& lhs, const RHS& rhs) const
//          // Return 'true' if the specified 'lhs' is less than the specified
//          // 'rhs' and 'false' otherwise.
//      {
//          return lhs < rhs;
//      }
//  };
//
//  struct NonTransparentComp
//      // This class can be used as a comparator for containers.  It has no
//      // nested type 'is_transparent', so it is classified as
//      // non-transparent by the 'bslmf::IsTransparentPredicate' metafunction
//      // and can not be used for heterogeneous comparison.
//  {
//      template <class LHS, class RHS>
//      bool operator()(const LHS& lhs, const RHS& rhs) const
//          // Return 'true' if the specified 'lhs' is less than the specified
//          // 'rhs' and 'false' otherwise.
//      {
//          return lhs < rhs;
//      }
//  };
//..
// Next we create an array of 'String' objects and two 'FlatStringSet' objects,
// basing on this array and having different comparators, transparent and
// non-transparent.  Note that creation of 'FlatStringSet' object does not
// increase number of created 'String' objects, because it just holds pointers
// to these objects, but does not copy them.
//..
//  int OBJECTS_NUMBER = String::numObjectsCreated();
//  assert(0 == OBJECTS_NUMBER);
//
//  String data[] = { "1", "2", "3", "5" };
//  enum { dataSize = sizeof data / sizeof *data };
//
//  assert(OBJECTS_NUMBER + 4 == String::numObjectsCreated());
//  OBJECTS_NUMBER = String::numObjectsCreated();
//
//  FlatStringSet<NonTransparentComp> nts(data, data + dataSize);
//  FlatStringSet<   TransparentComp>  ts(data, data + dataSize);
//
//  assert(4                  == nts.size()                 );
//  assert(4                  ==  ts.size()                 );
//  assert(OBJECTS_NUMBER     == String::numObjectsCreated());
//..
// Then we call 'find' method of set, having non-transparent comparator.
// Explicit creation of temporary 'String' object predictably increases the
// number of created 'String' objects.  But using character sequence as a
// parameter for 'find' method increases it too.  Because comparison operator
// that accepts two 'String' objects is used instead of operator that accepts
// 'String' and char sequence, so 'String' constructor is called implicitly.
//..
//  assert(nts.begin() + 0    == nts.find(String("1"))      );
//  assert(nts.begin() + 1    == nts.find(String("2"))      );
//  assert(nts.begin() + 2    == nts.find(String("3"))      );
//  assert(nts.begin() + 3    == nts.find(String("5"))      );
//  assert(nts.end()          == nts.find(String("6"))      );
//
//  assert(OBJECTS_NUMBER + 5 == String::numObjectsCreated());
//  OBJECTS_NUMBER = String::numObjectsCreated();
//
//  assert(nts.begin() + 0    == nts.find(       "1" )      );
//  assert(nts.begin() + 1    == nts.find(       "2" )      );
//  assert(nts.begin() + 2    == nts.find(       "3" )      );
//  assert(nts.begin() + 3    == nts.find(       "5" )      );
//  assert(nts.end()          == nts.find(       "6" )      );
//
//  assert(OBJECTS_NUMBER + 5 == String::numObjectsCreated());
//  OBJECTS_NUMBER = String::numObjectsCreated();
//..
// Finally we call 'find' method of set, having transparent comparator.
// Explicit creation of temporary 'String' object still increases the number of
// created 'String' objects.  But using character sequence as a parameter for
// 'find' method does not.  Because comparison operator that accepts 'String'
// and char sequence is available and more appropriate then operator accepting
// two 'String' objects.  So there is no need for implicit constructor
// invocation.
//..
//  assert( ts.begin() + 0    ==  ts.find(String("1"))      );
//  assert( ts.begin() + 1    ==  ts.find(String("2"))      );
//  assert( ts.begin() + 2    ==  ts.find(String("3"))      );
//  assert( ts.begin() + 3    ==  ts.find(String("5"))      );
//  assert( ts.end()          ==  ts.find(String("6"))      );
//
//  assert(OBJECTS_NUMBER + 5 == String::numObjectsCreated());
//  OBJECTS_NUMBER = String::numObjectsCreated();
//
//  assert( ts.begin() + 0    ==  ts.find(        "1" )     );
//  assert( ts.begin() + 1    ==  ts.find(        "2" )     );
//  assert( ts.begin() + 2    ==  ts.find(        "3" )     );
//  assert( ts.begin() + 3    ==  ts.find(        "5" )     );
//  assert( ts.end()          ==  ts.find(        "6" )     );
//
//  assert(OBJECTS_NUMBER     == String::numObjectsCreated());
//..

#include <bslmf_integralconstant.h>
#include <bslmf_voidtype.h>

namespace BloombergLP {
namespace bslmf {

template <class t_COMPARATOR, class t_KEY, class = void>
struct IsTransparentPredicate : bsl::false_type {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 't_COMPARATOR' is transparent (has a publicly
    // accessible member that is a type named 'is_transparent').  This generic
    // default template derives from 'bsl::false_type'.  Template
    // specializations are provided (below) that derive from 'bsl::true_type'.
};

template <class t_COMPARATOR, class t_KEY>
struct IsTransparentPredicate<
    t_COMPARATOR,
    t_KEY,
    BSLMF_VOIDTYPE(typename t_COMPARATOR::is_transparent)> : bsl::true_type {
    // This specialization of 'IsTransparentPredicate', for when the (template
    // parameter) 't_COMPARATOR' is transparent, derives from 'bsl::true_type'.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
