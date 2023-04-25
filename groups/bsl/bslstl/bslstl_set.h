// bslstl_set.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLSTL_SET
#define INCLUDED_BSLSTL_SET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant set class.
//
//@CLASSES:
//   bsl::set: STL-compatible set template
//
//@CANONICAL_HEADER: bsl_set.h
//
//@SEE_ALSO: bslstl_multiset, bslstl_map
//
//@DESCRIPTION: This component defines a single class template 'bsl::set',
// implementing the standard container holding an ordered sequence of unique
// keys.
//
// An instantiation of 'set' is an allocator-aware, value-semantic type whose
// salient attributes are its size (number of keys) and the ordered sequence of
// keys the 'set' contains.  If 'set' is instantiated with a key type that is
// not itself value-semantic, then it will not retain all of its value-semantic
// qualities.  In particular, if the key type cannot be tested for equality,
// then a set containing that type cannot be tested for equality.  It is even
// possible to instantiate 'set' with a key type that does not have a
// copy-constructor, in which case the 'set' will not be copyable.
//
// A set meets the requirements of an associative container with bidirectional
// iterators in the C++ standard [23.2.4].  The 'set' implemented here adheres
// to the C++11 standard when compiled with a C++11 compiler, and makes the
// best approximation when compiled with a C++03 compiler.  In particular, for
// C++03 we emulate move semantics, but limit forwarding (in 'emplace') to
// 'const' lvalues, and make no effort to emulate 'noexcept' or
// initializer-lists.
//
///Requirements on 'KEY'
///---------------------
// A 'set' is a fully "Value-Semantic Type" (see {'bsldoc_glossary'}) only if
// the supplied 'KEY' template parameters is fully value-semantic.  It is
// possible to instantiate a 'set' with 'KEY' parameter arguments that do not
// provide a full set of value-semantic operations, but then some methods of
// the container may not be instantiable.  The following terminology, adopted
// from the C++11 standard, is used in the function documentation of 'set' to
// describe a function's requirements for the 'KEY' template parameter.  These
// terms are also defined in section [17.6.3.1] of the C++11 standard.  Note
// that, in the context of a 'set' instantiation, the requirements apply
// specifically to the set's entry type, 'value_type', which is an alias for
// 'KEY'.
//
///Glossary
///--------
//..
//  Legend
//  ------
//  'X'    - denotes an allocator-aware container type (e.g., 'set')
//  'T'    - 'value_type' associated with 'X'
//  'A'    - type of the allocator used by 'X'
//  'm'    - lvalue of type 'A' (allocator)
//  'p',   - address ('T *') of uninitialized storage for a 'T' within an 'X'
//  'rv'   - rvalue of type (non-'const') 'T'
//  'v'    - rvalue or lvalue of type (possibly 'const') 'T'
//  'args' - 0 or more arguments
//..
// The following terms are used to more precisely specify the requirements on
// template parameter types in function-level documentation.
//
//: *default-insertable*: 'T' has a default constructor.  More precisely, 'T'
//:   is 'default-insertable' into 'X' means that the following expression is
//:   well-formed:
//:   'allocator_traits<A>::construct(m, p)'
//:
//: *move-insertable*: 'T' provides a constructor that takes an rvalue of type
//:   (non-'const') 'T'.  More precisely, 'T' is 'move-insertable' into 'X'
//:   means that the following expression is well-formed:
//:   'allocator_traits<A>::construct(m, p, rv)'
//:
//: *copy-insertable*: 'T' provides a constructor that takes an lvalue or
//:   rvalue of type (possibly 'const') 'T'.  More precisely, 'T' is
//:   'copy-insertable' into 'X' means that the following expression is
//:   well-formed:
//:   'allocator_traits<A>::construct(m, p, v)'
//:
//: *move-assignable*: 'T' provides an assignment operator that takes an rvalue
//:   of type (non-'const') 'T'.
//:
//: *copy-assignable*: 'T' provides an assignment operator that takes an lvalue
//:   or rvalue of type (possibly 'const') 'T'.
//:
//: *emplace-constructible*: 'T' is 'emplace-constructible' into 'X' from
//:   'args' means that the following expression is well-formed:
//:   'allocator_traits<A>::construct(m, p, args)'
//:
//: *erasable*: 'T' provides a destructor.  More precisely, 'T' is 'erasable'
//:   from 'X' means that the following expression is well-formed:
//:   'allocator_traits<A>::destroy(m, p)'
//:
//: *equality-comparable*: The type provides an equality-comparison operator
//:   that defines an equivalence relationship and is both reflexive and
//:   transitive.
//
///Memory Allocation
///-----------------
// The type supplied as a set's 'ALLOCATOR' template parameter determines how
// that set will allocate memory.  The 'set' template supports allocators
// meeting the requirements of the C++11 standard [17.6.3.5], in addition it
// supports scoped-allocators derived from the 'bslma::Allocator' memory
// allocation protocol.  Clients intending to use 'bslma' style allocators
// should use the template's default 'ALLOCATOR' type: The default type for
// the 'ALLOCATOR' template parameter, 'bsl::allocator', provides a C++11
// standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of a 'set' instantiation' is
// 'bsl::allocator', then objects of that set type will conform to the standard
// behavior of a 'bslma'-allocator-enabled type.  Such a set accepts an
// optional 'bslma::Allocator' argument at construction.  If the address of a
// 'bslma::Allocator' object is explicitly supplied at construction, it is used
// to supply memory for the set throughout its lifetime; otherwise, the set
// will use the default allocator installed at the time of the set's
// construction (see 'bslma_default').  In addition to directly allocating
// memory from the indicated 'bslma::Allocator', a set supplies that
// allocator's address to the constructors of contained objects of the
// (template parameter) type 'KEY' with the 'bslma::UsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'set':
//..
//  Legend
//  ------
//  'K'             - (template parameter) type 'KEY' of the set
//  'a', 'b'        - two distinct objects of type 'set<K>'
//  'rv'            - modifiable rvalue of type 'set<K>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'c'             - comparator providing an ordering for objects of type 'K'
//  'al             - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'k'             - an object of type 'K'
//  'rk'            - modifiable rvalue of type 'K'
//  'p1', 'p2'      - two 'const' iterators belonging to 'a'
//  distance(i1,i2) - the number of elements in the range [i1, i2)
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | set<K> a;    (default construction)                | O[1]               |
//  | set<K> a(al);                                      |                    |
//  | set<K> a(c, al);                                   |                    |
//  +----------------------------------------------------+--------------------+
//  | set<K> a(rv); (move construction)                  | O[1] if 'a' and    |
//  | set<K> a(rv, al);                                  | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | set<K> a(b); (copy construction)                   | O[n]               |
//  | set<K> a(b, al);                                   |                    |
//  +----------------------------------------------------+--------------------+
//  | set<K> a(i1, i2);                                  | O[N] if [i1, i2)   |
//  | set<K> a(i1, i2, al);                              | is sorted with     |
//  | set<K> a(i1, i2, c, al);                           | 'a.value_comp()',  |
//  |                                                    | O[N * log(N)]      |
//  |                                                    | otherwise, where N |
//  |                                                    | is distance(i1,i2) |
//  +----------------------------------------------------+--------------------+
//  | a.~set<K>(); (destruction)                         | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = rv;      (move assignment)                     | O[1] if 'a' and    |
//  |                                                    | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | a = b;       (copy assignment)                     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  | a.rbegin(), a.rend(), a.crbegin(), a.crend()       |                    |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a < b, a <= b, a > b, a >= b                       | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a, b)                              | O[1] if 'a' and    |
//  |                                                    | 'b' use the same   |
//  |                                                    | allocator,         |
//  |                                                    | O[n + m] otherwise |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | get_allocator()                                    | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(k)                                        | O[log(n)]          |
//  | a.insert(rk)                                       |                    |
//  | a.emplace(Args&&...)                               |                    |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, k)                                    | amortized constant |
//  | a.insert(p1, rk)                                   | if the value is    |
//  | a.emplace(p1, Args&&...)                           | inserted right     |
//  |                                                    | before p1,         |
//  |                                                    | O[log(n)]          |
//  |                                                    | otherwise          |
//  +----------------------------------------------------+--------------------+
//  | a.insert(i1, i2)                                   | O[log(N) *         |
//  |                                                    |   distance(i1,i2)] |
//  |                                                    |                    |
//  |                                                    | where N is         |
//  |                                                    | n + distance(i1,i2)|
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | amortized constant |
//  +----------------------------------------------------+--------------------+
//  | a.erase(k)                                         | O[log(n) +         |
//  |                                                    | a.count(k)]        |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | O[log(n) +         |
//  |                                                    | distance(p1, p2)]  |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | O[log(n) +         |
//  |                                                    | distance(p1, p2)]  |
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.key_comp()                                       | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.value_comp()                                     | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.contains(k)                                      | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.find(k)                                          | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.count(k)                                         | O[log(n) +         |
//  |                                                    | a.count(k)]        |
//  +----------------------------------------------------+--------------------+
//  | a.lower_bound(k)                                   | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.upper_bound(k)                                   | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.equal_range(k)                                   | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Holiday Calendar
/// - - - - - - - - - - - - - - - - - - -
// In this example, we will utilize 'bsl::set' to define and implement a class,
// 'HolidayCalendar', that provides a calendar that allows client to add and
// remove holiday dates and determine whether a particular date is a holiday.
//
// First, we define and implement the methods of a value-semantic type,
// 'MyDate', that represents a date: (Note that for brevity, we do not
// explicitly document the invariants of a valid date.)
//..
//  class MyDate {
//      // This class implements a value-semantic attribute class
//      // characterizing a date according to the (Gregorian) Unix date
//      // convention.
//
//      // DATA
//      int d_year;
//      int d_month;
//      int d_day;
//
//    public:
//      // CREATORS
//      MyDate(int year, int month, int day)
//          // Create a 'MyDate' object having the value represented by the
//          // specified 'year', 'month', and 'day.  The behavior is undefined
//          // unless the value represented by 'year', 'month', and 'day is
//          // valid.
//      : d_year(year), d_month(month), d_day(day)
//      {
//      }
//
//      MyDate(const MyDate& original)
//          // Create a 'MyDate' object having the same value as the specified
//          // 'original' object.
//      : d_year(original.d_year)
//      , d_month(original.d_month)
//      , d_day(original.d_day)
//      {
//      }
//
//      //! ~MyDate() = default;
//          // Destroy this object
//
//      // MANIPULATORS
//      MyDate& operator=(const MyDate& rhs)
//          // Assign to this object the value of the specified 'rhs' object,
//          // and return a reference providing modifiable access to this
//          // object.
//      {
//          d_year = rhs.d_year;
//          d_month = rhs.d_month;
//          d_day = rhs.d_day;
//          return *this;
//      }
//
//      // ACCESSORS
//      int year() const
//          // Return the year of this date.
//      {
//          return d_year;
//      }
//
//      int month() const
//          // Return the month of this date.
//      {
//          return d_month;
//      }
//
//      int day() const
//          // Return the day of this date.
//      {
//          return d_day;
//      }
//
//  };
//
//  // FREE FUNCTIONS
//  inline
//  bool operator==(const MyDate& lhs, const MyDate& rhs)
//      // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
//      // value, and 'false' otherwise.  Two 'MyDate' objects have the same
//      // value if each of their corresponding 'year', 'month', and 'day'
//      // attributes respective have the same value.
//  {
//      return lhs.year()  == rhs.year() &&
//             lhs.month() == rhs.month() &&
//             lhs.day()   == rhs.day();
//  }
//
//  inline
//  bool operator!=(const MyDate& lhs, const MyDate& rhs)
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'MyDate' objects do not
//      // have the same value if each of their corresponding 'year', 'month',
//      // and 'day' attributes respective do not have the same value.
//  {
//      return !(lhs == rhs);
//  }
//..
// Then, we define a comparison functor for 'MyDate' objects in order for them
// to be stored in a 'bsl::set' object:
//..
//  struct MyDateLess {
//      // This 'struct' defines an ordering on 'MyDate' objects, allowing them
//      // to be included in associative containers such as 'bsl::set'.
//
//      bool operator() (const MyDate& lhs, const MyDate& rhs) const
//          // Return 'true' if the value of the specified 'lhs' is less than
//          // (ordered before) the value of the specified 'rhs', and 'false'
//          // otherwise.  The 'lhs' value is considered less than the 'rhs'
//          // value if the date represented by 'lhs' is earlier than the date
//          // represented by 'rhs' in time.
//      {
//          if (lhs.year() < rhs.year()) return true;
//          if (lhs.year() == rhs.year()) {
//              if (lhs.month() < rhs.month()) return true;
//              if (lhs.month() == rhs.month()) {
//                  if (lhs.day() < rhs.day()) return true;
//              }
//          }
//          return false;
//      }
//  };
//..
// Next, we define 'HolidayCalendar':
//..
//  class HolidayCalendar {
//      // This class provides a value-semantic type that allows clients to
//      // modify and query a set of dates considered to be holidays.
//..
// Here, we create a type alias, 'DateSet', for a 'bsl::set' that will serve as
// the data member for a 'HolidayCalendar'.  A 'DateSet' has keys of type
// 'MyDate', and a comparator of type 'MyDateLess'.  We use the default
// 'ALLOCATOR' template parameter as we intend to use 'HolidayCalendar' with
// 'bslma' style allocators:
//..
//      // PRIVATE TYPES
//      typedef bsl::set<MyDate, MyDateLess> DateSet;
//          // This 'typedef' is an alias for a set of 'MyDate' objects.
//
//      // DATA
//      DateSet d_holidayDates;  // set of dates considered to be holidays
//
//    public:
//      // PUBLIC TYPES
//      typedef DateSet::const_iterator ConstIterator;
//          // This 'typedef' provides an alias for the type of an iterator
//          // providing non-modifiable access to holiday dates in a
//          // 'HolidayCalendar'.
//
//      // CREATORS
//      HolidayCalendar(bslma::Allocator *basicAllocator = 0);
//          // Create an empty 'HolidayCalendar' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      HolidayCalendar(const HolidayCalendar&  original,
//                      bslma::Allocator       *basicAllocator = 0);
//          // Create a 'HolidayCalendar' object having the same value as the
//          // specified 'original' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      //! ~HolidayCalendar() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      void addHolidayDate(const MyDate& date);
//          // Add the specified 'date' as a holiday date maintained by this
//          // calendar.  If 'date' is already a holiday date, this method has
//          // no effect.
//
//      void removeHolidayDate(const MyDate& date);
//          // Remove the specify 'date' from the set of holiday dates
//          // maintained by this calendar.  If 'date' is not a holiday date,
//          // this method has no effect.
//
//      // ACCESSORS
//      bool isHolidayDate(const MyDate& date) const;
//          // Return 'true' if the specified 'date' is in the set of holiday
//          // dates maintained by this calendar, and return 'false' otherwise.
//
//      ConstIterator beginHolidayDates() const;
//          // Return an iterator providing non-modifiable access to the first
//          // date in the ordered sequence of holiday dates maintained by this
//          // calendar.
//
//      ConstIterator endHolidayDates() const;
//          // Return an iterator providing non-modifiable access to
//          // past-the-end date in the ordered sequence of holiday dates
//          // maintained by this calendar.
//  };
//..
// Then, we declare the free operators for 'HolidayCalendar':
//..
//  inline
//  bool operator==(const HolidayCalendar& lhs, const HolidayCalendar& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
//      // value, and 'false' otherwise.  Two 'HolidayCalendar' objects have
//      // the same value if they have the same number of holiday dates, and
//      // each corresponding holiday date, in their respective ordered
//      // sequence of dates, is the same.
//
//  inline
//  bool operator!=(const HolidayCalendar& lhs, const HolidayCalendar& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'HolidayCalendar'
//      // objects do not have the same value if they either differ in their
//      // number of holiday dates, or if any of the corresponding holiday
//      // dates, in their respective ordered sequences of dates, is not the
//      // same.
//..
// Now, we define the implementations methods of the 'HolidayCalendar' class:
//..
//  // CREATORS
//  HolidayCalendar::HolidayCalendar(bslma::Allocator *basicAllocator)
//  : d_holidayDates(basicAllocator)
//  {
//  }
//..
// Notice that, on construction, we pass the 'bsl::set' object the specified
// 'bsl::Allocator' object.
//..
//  // MANIPULATORS
//  void HolidayCalendar::addHolidayDate(const MyDate& date)
//  {
//      d_holidayDates.insert(date);
//  }
//
//  void HolidayCalendar::removeHolidayDate(const MyDate& date)
//  {
//      d_holidayDates.erase(date);
//  }
//
//  // ACCESSORS
//  bool HolidayCalendar::isHolidayDate(const MyDate& date) const
//  {
//      return d_holidayDates.find(date) != d_holidayDates.end();
//  }
//
//  HolidayCalendar::ConstIterator HolidayCalendar::beginHolidayDates() const
//  {
//      return d_holidayDates.begin();
//  }
//
//  HolidayCalendar::ConstIterator HolidayCalendar::endHolidayDates() const
//  {
//      return d_holidayDates.end();
//  }
//..
// Finally, we implement the free operators for 'HolidayCalendar':
//..
//  inline
//  bool operator==(const HolidayCalendar& lhs, const HolidayCalendar& rhs)
//  {
//      return lhs.d_holidayDates == rhs.d_holidayDates;
//  }
//
//  inline
//  bool operator!=(const HolidayCalendar& lhs, const HolidayCalendar& rhs)
//  {
//      return !(lhs == rhs);
//  }
//..

#include <bslscm_version.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
#include <bslstl_iteratorutil.h>
#include <bslstl_pair.h>
#include <bslstl_setcomparator.h>
#include <bslstl_stdexceptutil.h>
#include <bslstl_treeiterator.h>
#include <bslstl_treenode.h>
#include <bslstl_treenodepool.h>

#include <bslalg_rangecompare.h>
#include <bslalg_rbtreeanchor.h>
#include <bslalg_rbtreenode.h>
#include <bslalg_rbtreeutil.h>
#include <bslalg_swaputil.h>
#include <bslalg_synththreewayutil.h>
#include <bslalg_typetraithasstliterators.h>

#include <bslma_isstdallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isconvertible.h>
#include <bslmf_isnothrowswappable.h>
#include <bslmf_istransparentpredicate.h>
#include <bslmf_movableref.h>
#include <bslmf_typeidentity.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <functional>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
# include <initializer_list>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <type_traits>  // 'std::is_nothrow_move_assignable'
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslstl_set.h
# define COMPILING_BSLSTL_SET_H
# include <bslstl_set_cpp03.h>
# undef COMPILING_BSLSTL_SET_H
#else

namespace bsl {

                             // =========
                             // class set
                             // =========

template <class KEY,
          class COMPARATOR  = std::less<KEY>,
          class ALLOCATOR = allocator<KEY> >
class set {
    // This class template implements a value-semantic container type holding
    // an ordered sequence of unique keys (of the template parameter type,
    // 'KEY').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   except for 'BDEX' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

    // PRIVATE TYPES
    typedef const KEY                                          ValueType;
        // This typedef is an alias for the type of key objects maintained by
        // this set.

    typedef BloombergLP::bslstl::SetComparator<KEY, COMPARATOR> Comparator;
        // This typedef is an alias for the comparator used internally by this
        // set.

    typedef BloombergLP::bslstl::TreeNode<KEY>                 Node;
        // This typedef is an alias for the type of nodes held by the tree (of
        // nodes) used to implement this set.

    typedef BloombergLP::bslstl::TreeNodePool<KEY, ALLOCATOR>  NodeFactory;
        // This typedef is an alias for the factory type used to create and
        // destroy 'Node' objects.

    typedef bsl::allocator_traits<ALLOCATOR>                   AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    typedef BloombergLP::bslmf::MovableRefUtil                 MoveUtil;
        // This typedef is a convenient alias for the utility associated with
        // movable references.

    class DataWrapper : public Comparator {
        // This class is a wrapper around the comparator and allocator data
        // members.  It takes advantage of the empty-base optimization (EBO) so
        // that if the comparator is stateless, it takes up no space.
        //
        // TBD: This struct should eventually be replaced by the use of a
        // general EBO-enabled component that provides a 'pair'-like interface
        // or a 'tuple'.

        // DATA
        NodeFactory d_pool;  // pool of 'Node' objects

      private:
        // NOT IMPLEMENTED
        DataWrapper(const DataWrapper&);
        DataWrapper& operator=(const DataWrapper&);

      public:
        // CREATORS
        explicit DataWrapper(const COMPARATOR& comparator,
                             const ALLOCATOR&  basicAllocator);
            // Create a data wrapper using a copy of the specified 'comparator'
            // to order keys and a copy of the specified 'basicAllocator' to
            // supply memory.

        DataWrapper(
              BloombergLP::bslmf::MovableRef<DataWrapper> original);// IMPLICIT
            // Create a data wrapper initialized to the contents of the 'pool'
            // associated with the specified 'original' data wrapper.  The
            // comparator and allocator associated with 'original' are
            // propagated to the new data wrapper.  'original' is left in a
            // valid but unspecified state.

        // MANIPULATORS
        NodeFactory& nodeFactory();
            // Return a reference providing modifiable access to the node
            // factory associated with this data wrapper.

        // ACCESSORS
        const NodeFactory& nodeFactory() const;
            // Return a reference providing non-modifiable access to the node
            // factory associated with this data wrapper.
    };

    // DATA
    DataWrapper                       d_compAndAlloc;
                                               // comparator and pool of 'Node'
                                               // objects

    BloombergLP::bslalg::RbTreeAnchor d_tree;  // balanced tree of 'Node'
                                               // objects

  public:
    // PUBLIC TYPES
    typedef KEY                                        key_type;
    typedef KEY                                        value_type;
    typedef COMPARATOR                                 key_compare;
    typedef COMPARATOR                                 value_compare;
    typedef ALLOCATOR                                  allocator_type;
    typedef value_type&                                reference;
    typedef const value_type&                          const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

    typedef BloombergLP::bslstl::TreeIterator<const value_type,
                                              Node,
                                              difference_type> iterator;
    typedef BloombergLP::bslstl::TreeIterator<const value_type,
                                              Node,
                                              difference_type> const_iterator;
    typedef bsl::reverse_iterator<iterator>            reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>      const_reverse_iterator;

  private:
    // PRIVATE MANIPULATORS
    NodeFactory& nodeFactory();
        // Return a reference providing modifiable access to the node-allocator
        // for this tree.

    Comparator& comparator();
        // Return a reference providing modifiable access to the comparator for
        // this tree.

    void quickSwapExchangeAllocators(set& other);
        // Efficiently exchange the value, comparator, and allocator of this
        // object with the value, comparator, and allocator of the specified
        // 'other' object.  This method provides the no-throw exception-safety
        // guarantee, *unless* swapping the (user-supplied) comparator or
        // allocator objects can throw.

    void quickSwapRetainAllocators(set& other);
        // Efficiently exchange the value and comparator of this object with
        // the value and comparator of the specified 'other' object.  This
        // method provides the no-throw exception-safety guarantee, *unless*
        // swapping the (user-supplied) comparator objects can throw.  The
        // behavior is undefined unless this object was created with the same
        // allocator as 'other'.

    // PRIVATE ACCESSORS
    const NodeFactory& nodeFactory() const;
        // Return a reference providing non-modifiable access to the
        // node-allocator for this tree.

    const Comparator& comparator() const;
        // Return a reference providing non-modifiable access to the comparator
        // for this tree.

  public:
    // CREATORS
    set();
    explicit set(const COMPARATOR& comparator,
                 const ALLOCATOR&  basicAllocator = ALLOCATOR())
        // Create an empty set.  Optionally specify a 'comparator' used to
        // order keys contained in this object.  If 'comparator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'COMPARATOR' is used.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // and 'basicAllocator' is not supplied, the currently installed
        // default allocator is used.  Note that a 'bslma::Allocator *' can be
        // supplied for 'basicAllocator' if the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).
        : d_compAndAlloc(comparator, basicAllocator),
          d_tree()
    {
        // The implementation is placed here in the class definition to work
        // around an AIX compiler bug, where the constructor can fail to
        // compile because it is unable to find the definition of the default
        // argument.  This occurs when a templatized class wraps around the
        // container and the comparator is defined after the new class.
    }

    explicit set(const ALLOCATOR& basicAllocator);
        // Create an empty set that uses the specified 'basicAllocator' to
        // supply memory.  Use a default-constructed object of the (template
        // parameter) type 'COMPARATOR' to order the keys contained in this
        // set.  Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    set(const set& original);
        // Create a set having the same value as the specified 'original'
        // object.  Use a copy of 'original.key_comp()' to order the keys
        // contained in this set.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  This method requires that the (template parameter)
        // type 'KEY' be 'copy-insertable' into this set (see {Requirements on
        // 'KEY'}).

    set(BloombergLP::bslmf::MovableRef<set> original);              // IMPLICIT
        // Create a set having the same value as the specified 'original'
        // object by moving (in constant time) the contents of 'original' to
        // the new set.  Use a copy of 'original.key_comp()' to order the keys
        // contained in this set.  The allocator associated with 'original' is
        // propagated for use in the newly-created set.  'original' is left in
        // a valid but unspecified state.

    set(const set&                                     original,
        const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a set having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // Use a copy of 'original.key_comp()' to order the keys contained in
        // this set.  This method requires that the (template parameter) type
        // 'KEY' be 'copy-insertable' into this set (see {Requirements on
        // 'KEY'}).  Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    set(BloombergLP::bslmf::MovableRef<set>            original,
        const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a set having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // The contents of 'original' are moved (in constant time) to the new
        // set if 'basicAllocator == original.get_allocator()', and are move-
        // inserted (in linear time) using 'basicAllocator' otherwise.
        // 'original' is left in a valid but unspecified state.  Use a copy of
        // 'original.key_comp()' to order the keys contained in this set.  This
        // method requires that the (template parameter) type 'KEY' be
        // 'move-insertable' into this set (see {Requirements on 'KEY'}).  Note
        // that a 'bslma::Allocator *' can be supplied for 'basicAllocator' if
        // the (template parameter) type 'ALLOCATOR' is 'bsl::allocator' (the
        // default).

    template <class INPUT_ITERATOR>
    set(INPUT_ITERATOR    first,
        INPUT_ITERATOR    last,
        const COMPARATOR& comparator = COMPARATOR(),
        const ALLOCATOR&  basicAllocator = ALLOCATOR());
    template <class INPUT_ITERATOR>
    set(INPUT_ITERATOR    first,
        INPUT_ITERATOR    last,
        const ALLOCATOR&  basicAllocator);
        // Create a set, and insert each 'value_type' object in the sequence
        // starting at the specified 'first' element, and ending immediately
        // before the specified 'last' element, ignoring those keys having a
        // value equivalent to that which appears earlier in the sequence.
        // Optionally specify a 'comparator' used to order keys contained in
        // this object.  If 'comparator' is not supplied, a default-constructed
        // object of the (template parameter) type 'COMPARATOR' is used.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not supplied, a default-constructed object of
        // the (template parameter) type 'ALLOCATOR' is used.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used.  If the
        // sequence 'first' to 'last' is ordered according to 'comparator',
        // then this operation has 'O[N]' complexity, where 'N' is the number
        // of elements between 'first' and 'last', otherwise this operation has
        // 'O[N * log(N)]' complexity.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type', and 'value_type' must be
        // 'emplace-constructible' from '*i' into this set, where 'i' is a
        // dereferenceable iterator in the range '[first .. last)' (see
        // {Requirements on 'KEY'}).  The behavior is undefined unless 'first'
        // and 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last'.  Note that a 'bslma::Allocator *' can
        // be supplied for 'basicAllocator' if the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    set(std::initializer_list<KEY> values,
        const COMPARATOR&          comparator = COMPARATOR(),
        const ALLOCATOR&           basicAllocator = ALLOCATOR());
    set(std::initializer_list<KEY> values,
        const ALLOCATOR&           basicAllocator);
        // Create a set and insert each 'value_type' object in the specified
        // 'values' initializer list, ignoring those keys having a value
        // equivalent to that which appears earlier in the list.  Optionally
        // specify a 'comparator' used to order keys contained in this object.
        // If 'comparator' is not supplied, a default-constructed object of the
        // (template parameter) type 'COMPARATOR' is used.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // not supplied, a default-constructed object of the (template
        // parameter) type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  If 'values' is ordered
        // according to 'comparator', then this operation has 'O[N]'
        // complexity, where 'N' is the number of elements in 'list'; otherwise
        // this operation has 'O[N * log(N)]' complexity.  This method requires
        // that the (template parameter) type 'KEY' be 'copy-insertable' into
        // this set (see {Requirements on 'KEY'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).
#endif

    ~set();
        // Destroy this object.

    // MANIPULATORS
    set& operator=(const set& rhs);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_copy_assignment',
        // and return a reference providing modifiable access to this object.
        // If an exception is thrown, '*this' is left in a valid but
        // unspecified state.  This method requires that the (template
        // parameter) type 'KEY' be 'copy-assignable' and 'copy-insertable'
        // into this set (see {Requirements on 'KEY'}).

    set& operator=(BloombergLP::bslmf::MovableRef<set> rhs)
                       BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                           AllocatorTraits::is_always_equal::value
                        && std::is_nothrow_move_assignable<COMPARATOR>::value);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_move_assignment',
        // and return a reference providing modifiable access to this object.
        // The contents of 'rhs' are moved (in constant time) to this set if
        // 'get_allocator() == rhs.get_allocator()' (after accounting for the
        // aforementioned trait); otherwise, all elements in this set are
        // either destroyed or move-assigned to and each additional element in
        // 'rhs' is move-inserted into this set.  'rhs' is left in a valid but
        // unspecified state, and if an exception is thrown, '*this' is left
        // in a valid but unspecified state.  This method requires that the
        // (template parameter) type 'KEY' be 'move-assignable' and
        // 'move-insertable' into this set (see {Requirements on 'KEY'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    set& operator=(std::initializer_list<KEY> values);
        // Assign to this object the value resulting from first clearing this
        // set and then inserting each 'value_type' object in the specified
        // 'values' initializer list, ignoring those keys having a value
        // equivalent to that which appears earlier in the list; return a
        // reference providing modifiable access to this object.  This method
        // requires that the (template parameter) type 'KEY' be
        // 'copy-insertable' into this set (see {Requirements on 'KEY'}).
#endif

    iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this set, or the 'end' iterator if this set is empty.

    iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the past-the-end
        // element in the ordered sequence of 'value_type' objects maintained
        // by this set.

    reverse_iterator rbegin() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this set, or 'rend' if this object is empty.

    reverse_iterator rend() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this set.

    pair<iterator, bool> insert(const value_type& value);
        // Insert the specified 'value' into this set if a key equivalent to
        // 'value' does not already exist in this set; otherwise, if a key
        // equivalent to 'value' already exists in this set, this method has no
        // effect.  Return a pair whose 'first' member is an iterator referring
        // to the (possibly newly inserted) 'value_type' object in this set
        // that is equivalent to 'value', and whose 'second' member is 'true'
        // if a new value was inserted, and 'false' if the key was already
        // present.  This method requires that the (template parameter) type
        // 'KEY' be 'copy-insertable' into this set (see {Requirements on
        // 'KEY'}).

    pair<iterator, bool> insert(
                             BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert the specified 'value' into this set if a key equivalent to
        // 'value' does not already exist in this set; otherwise, if a key
        // equivalent to 'value' already exists in this set, this method has no
        // effect.  'value' is left in a valid but unspecified state.  Return a
        // pair whose 'first' member is an iterator referring to the (possibly
        // newly inserted) 'value_type' object in this set that is equivalent
        // to 'value', and whose 'second' member is 'true' if a new value was
        // inserted, and 'false' if the key was already present.  This method
        // requires that the (template parameter) type 'KEY' be
        // 'move-insertable' (see {Requirements on 'KEY'}).

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this set (in amortized constant
        // time if the specified 'hint' is a valid immediate successor to
        // 'value'), if a key equivalent to 'value' does not already exist in
        // this set; otherwise, if a key equivalent to 'value' already exists
        // in this set, this method has no effect.  Return an iterator
        // referring to the (possibly newly inserted) 'value_type' object in
        // this set that is equivalent to 'value'.  If 'hint' is not a valid
        // immediate successor to 'value', this operation has 'O[log(N)]'
        // complexity, where 'N' is the size of this set.  This method requires
        // that the (template parameter) type 'KEY' be 'copy-insertable' into
        // this set (see {Requirements on 'KEY'}).  The behavior is undefined
        // unless 'hint' is an iterator in the range '[begin() .. end()]' (both
        // endpoints included).

    iterator insert(const_iterator                             hint,
                    BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert the specified 'value' into this set (in amortized constant
        // time if the specified 'hint' is a valid immediate successor to
        // 'value') if a key equivalent to 'value' does not already exist in
        // this set; otherwise, this method has no effect.  'value' is left in
        // a valid but unspecified state.  Return an iterator referring to the
        // (possibly newly inserted) 'value_type' object in this set that is
        // equivalent to 'value'.  If 'hint' is not a valid immediate successor
        // to 'value', this operation has 'O[log(N)]' complexity, where 'N' is
        // the size of this set.  This method requires that the (template
        // parameter) type 'KEY' be 'move-insertable' (see {Requirements on
        // 'KEY'}).  The behavior is undefined unless 'hint' is an iterator in
        // the range '[begin() .. end()]' (both endpoints included).

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this set the value of each 'value_type' object in the
        // range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator, if a key
        // equivalent to the object is not already contained in this set.  The
        // (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type', and 'value_type' must be 'emplace-constructible' from
        // '*i' into this set, where 'i' is a dereferenceable iterator in the
        // range '[first .. last)' (see {Requirements on 'KEY'}).  The behavior
        // is undefined unless 'first' and 'last' refer to a sequence of valid
        // values where 'first' is at a position at or before 'last'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void insert(std::initializer_list<KEY> values);
        // Insert into this set the value of each 'value_type' object in the
        // specified 'values' initializer list if a key equivalent to the
        // object is not already contained in this set.  This method requires
        // that the (template parameter) type 'KEY' be 'copy-insertable' (see
        // {Requirements on 'KEY'}).
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

    template <class... Args>
    pair<iterator, bool> emplace(Args&&... arguments);
        // Insert into this set a newly created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'arguments' to the corresponding
        // constructor of 'value_type', if a key equivalent to such a value
        // does not already exist in this set; otherwise, this method has no
        // effect (other than possibly creating a temporary 'value_type'
        // object).  Return a pair whose 'first' member is an iterator
        // referring to the (possibly newly created and inserted) object in
        // this set whose value is equivalent to that of an object constructed
        // from 'arguments', and whose 'second' member is 'true' if a new value
        // was inserted, and 'false' if an equivalent key was already present.
        // This method requires that the (template parameter) type 'KEY' be
        // 'emplace-constructible' from 'arguments' (see {Requirements on
        // 'KEY'}).

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... arguments);
        // Insert into this set a newly created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'arguments' to the corresponding
        // constructor of 'value_type', (in amortized constant time if the
        // specified 'hint' is a valid immediate successor to the 'value_type'
        // object constructed from 'arguments'), if a key equivalent to such a
        // value does not already exist in this set; otherwise, this method has
        // no effect (other than possibly creating a temporary 'value_type'
        // object).  Return an iterator referring to the (possibly newly
        // created and inserted) object in this set whose value is equivalent
        // to that of an object constructed from 'arguments'.  If 'hint' is not
        // a valid immediate successor to the 'value_type' object implied by
        // 'arguments', this operation has 'O[log(N)]' complexity where 'N' is
        // the size of this set.  This method requires that the (template
        // parameter) type 'KEY' be 'emplace-constructible' from 'arguments'
        // (see {Requirements on 'KEY'}).  The behavior is undefined unless
        // 'hint' is an iterator in the range '[begin() .. end()]' (both
        // endpoints included).

#endif

    iterator erase(const_iterator position);
        // Remove from this set the 'value_type' object at the specified
        // 'position', and return an iterator referring to the element
        // immediately following the removed element, or to the past-the-end
        // position if the removed element was the last element in the sequence
        // of elements maintained by this set.   This method invalidates only
        // iterators and references to the removed element and previously saved
        // values of the 'end()' iterator.  The behavior is undefined unless
        // 'position' refers to a 'value_type' object in this set.

    size_type erase(const key_type& key);
        // Remove from this set the 'value_type' object that is equivalent to
        // the specified 'key', if such an entry exists, and return 1;
        // otherwise, if there is no 'value_type' object that is equivalent to
        // 'key', return 0 with no other effect.   This method invalidates only
        // iterators and references to the removed element and previously saved
        // values of the 'end()' iterator.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this set the 'value_type' objects starting at the
        // specified 'first' position up to, but including the specified 'last'
        // position, and return 'last'.   This method invalidates only
        // iterators and references to the removed element and previously saved
        // values of the 'end()' iterator.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this set or are the
        // 'end' iterator, and the 'first' position is at or before the 'last'
        // position in the ordered sequence provided by this container.

    void swap(set& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                 AllocatorTraits::is_always_equal::value
                              && bsl::is_nothrow_swappable<COMPARATOR>::value);
        // Exchange the value and comparator of this object with those of the
        // specified 'other' object; also exchange the allocator of this object
        // with that of 'other' if the (template parameter) type 'ALLOCATOR'
        // has the 'propagate_on_container_swap' trait, and do not modify
        // either allocator otherwise.  This method provides the no-throw
        // exception-safety guarantee if and only if the (template parameter)
        // type 'COMPARATOR' provides a no-throw swap operation, and provides
        // the basic exception-safety guarantee otherwise; if an exception is
        // thrown, both objects are left in valid but unspecified states.  This
        // operation has 'O[1]' complexity if either this object was created
        // with the same allocator as 'other' or 'ALLOCATOR' has the
        // 'propagate_on_container_swap' trait; otherwise, it has 'O[n + m]'
        // complexity, where 'n' and 'm' are the number of elements in this
        // object and 'other', respectively.  Note that this method's support
        // for swapping objects created with different allocators when
        // 'ALLOCATOR' does not have the 'propagate_on_container_swap' trait is
        // a departure from the C++ Standard.

    void clear() BSLS_KEYWORD_NOEXCEPT;
        // Remove all entries from this set.  Note that the set is empty after
        // this call, but allocated memory may be retained for future use.

    // Turn off complaints about necessarily class-defined methods.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CD01

    iterator find(const key_type& key)
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this set that is equivalent to the specified 'key', if
        // such an entry exists, and the past-the-end ('end') iterator
        // otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        iterator>::type
    find(const LOOKUP_KEY& key)
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this set that is equivalent to the specified 'key', if
        // such an entry exists, and the past-the-end ('end') iterator
        // otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    iterator lower_bound(const key_type& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this set greater-than or
        // equal-to the specified 'key', and the past-the-end iterator if this
        // set does not contain a 'value_type' object greater-than or equal-to
        // 'key'.  Note that this function returns the *first* position before
        // which a 'value_type' object equivalent to 'key' could be inserted
        // into the ordered sequence maintained by this set, while preserving
        // its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        iterator>::type
    lower_bound(const LOOKUP_KEY& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this set greater-than or
        // equal-to the specified 'key', and the past-the-end iterator if this
        // set does not contain a 'value_type' object greater-than or equal-to
        // 'key'.  Note that this function returns the *first* position before
        // which a 'value_type' object equivalent to 'key' could be inserted
        // into the ordered sequence maintained by this set, while preserving
        // its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    iterator upper_bound(const key_type& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this set greater than the
        // specified 'key', and the past-the-end iterator if this set does not
        // contain a 'value_type' object greater-than 'key'.  Note that this
        // function returns the *last* position before which a 'value_type'
        // object equivalent to 'key' could be inserted into the ordered
        // sequence maintained by this set, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        iterator>::type
    upper_bound(const LOOKUP_KEY& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this set greater than the
        // specified 'key', and the past-the-end iterator if this set does not
        // contain a 'value_type' object greater-than 'key'.  Note that this
        // function returns the *last* position before which a 'value_type'
        // object equivalent to 'key' could be inserted into the ordered
        // sequence maintained by this set, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    pair<iterator, iterator> equal_range(const key_type& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this set that are equivalent to
        // the specified 'key', where the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  The first returned iterator will be
        // 'lower_bound(key)'; the second returned iterator will be
        // 'upper_bound(key)'; and, if this set contains no 'value_type'
        // objects equivalent to 'key', then the two returned iterators will
        // have the same value.  Note that since a set maintains unique keys,
        // the range will contain at most one element.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        iterator startIt = lower_bound(key);
        iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            ++endIt;
        }
        return pair<iterator, iterator>(startIt, endIt);
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        pair<iterator, iterator> >::type
    equal_range(const LOOKUP_KEY& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this set that are equivalent to
        // the specified 'key', where the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  The first returned iterator will be
        // 'lower_bound(key)'; the second returned iterator will be
        // 'upper_bound(key)'; and, if this set contains no 'value_type'
        // objects equivalent to 'key', then the two returned iterators will
        // have the same value.  Note that although a set maintains unique
        // keys, the range may contain more than one element, because a
        // transparent comparator may have been supplied that provides a
        // different (but compatible) partitioning of keys for 'LOOKUP_KEY' as
        // the comparisons used to order the keys in the set.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        iterator startIt = lower_bound(key);
        iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            ++endIt;

            // Typically, even with a transparent comparator, we expect to find
            // either 0 or 1 matching keys. We test for those two common cases
            // before performing a logarithmic search via 'upper_bound' to
            // determine the end of the range.

            if (endIt != end() && !comparator()(key, *endIt.node())) {
                endIt = upper_bound(key);
            }
        }
        return pair<iterator, iterator>(startIt, endIt);
    }

    // BDE_VERIFY pragma: pop

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return (a copy of) the allocator used for memory allocation by this
        // set.

    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this set, or the 'end' iterator if this set is empty.

    const_iterator end() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type'
        // objects maintained by this set.

    const_reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this set, or 'rend' if this object is empty.

    const_reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this set.

    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this set, or the 'cend' iterator if this set is empty.

    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this set.

    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this set, or 'crend' if this set is empty.

    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this set.

    bool contains(const key_type &key) const;
        // Return 'true' if this map contains an element whose key is
        // equivalent to the specified 'key'.

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        bool>::type
    contains(const LOOKUP_KEY& key) const
        // Return 'true' if this map contains an element whose key is
        // equivalent to the specified 'key'.
        //
        // Note: implemented inline due to Sun CC compilation error
    {
        return find(key) != end();
    }

    bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this set contains no elements, and 'false'
        // otherwise.

    size_type size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of elements in this set.

    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of elements
        // that this set could possibly hold.  Note that there is no guarantee
        // that the set can successfully grow to the returned size, or even
        // close to that size without running out of resources.

    key_compare key_comp() const;
        // Return the key-comparison functor (or function pointer) used by this
        // set; if a comparator was supplied at construction, return its value,
        // otherwise return a default constructed 'key_compare' object.  Note
        // that this comparator compares objects of type 'KEY', which is the
        // type of the 'value_type' objects contained in this set.

    value_compare value_comp() const;
        // Return a functor for comparing two 'value_type' objects using
        // 'key_comp()'.  Note that since 'value_type' is an alias to 'KEY' for
        // 'set', this method returns the same functor as 'key_comp()'.

    // Turn off complaints about necessarily class-defined methods.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CD01

    const_iterator find(const key_type& key) const
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this set that is equivalent to the specified
        // 'key', if such an entry exists, and the past-the-end ('end')
        // iterator otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        const_iterator>::type
    find(const LOOKUP_KEY& key) const
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this set that is equivalent to the specified
        // 'key', if such an entry exists, and the past-the-end ('end')
        // iterator otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    size_type count(const key_type& key) const
        // Return the number of 'value_type' objects within this set that are
        // equivalent to the specified 'key'.  Note that since a set maintains
        // unique keys, the returned value will be either 0 or 1.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return (find(key) != end()) ? 1 : 0;
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        size_type>::type
    count(const LOOKUP_KEY& key) const
        // Return the number of 'value_type' objects within this set that are
        // equivalent to the specified 'key'.  Note that although a set
        // maintains unique keys, the returned value can be other than 0 or 1,
        // because a transparent comparator may have been supplied that
        // provides a different (but compatible) partitioning of keys for
        // 'LOOKUP_KEY' as the comparisons used to order the keys in the set.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        int            count = 0;
        const_iterator it    = lower_bound(key);

        while (it != end() && !comparator()(key, *it.node())) {
            ++it;
            ++count;
        }
        return count;
    }

    const_iterator lower_bound(const key_type& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this set greater-than
        // or equal-to the specified 'key', and the past-the-end iterator if
        // this set does not contain a 'value_type' greater-than or equal-to
        // 'key'.  Note that this function returns the *first* position before
        // which a 'value_type' object equivalent to 'key' could be inserted
        // into the ordered sequence maintained by this set, while preserving
        // its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        const_iterator>::type
    lower_bound(const LOOKUP_KEY& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this set greater-than
        // or equal-to the specified 'key', and the past-the-end iterator if
        // this set does not contain a 'value_type' greater-than or equal-to
        // 'key'.  Note that this function returns the *first* position before
        // which a 'value_type' object equivalent to 'key' could be inserted
        // into the ordered sequence maintained by this set, while preserving
        // its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    const_iterator upper_bound(const key_type& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this set greater than
        // the specified 'key', and the past-the-end iterator if this set does
        // not contain a 'value_type' object greater-than 'key'.  Note that
        // this function returns the *last* position before which a
        // 'value_type' object equivalent to 'key' could be inserted into the
        // ordered sequence maintained by this set, while preserving its
        // ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        const_iterator>::type
    upper_bound(const LOOKUP_KEY& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this set greater than
        // the specified 'key', and the past-the-end iterator if this set does
        // not contain a 'value_type' object greater-than 'key'.  Note that
        // this function returns the *last* position before which a
        // 'value_type' object equivalent to 'key' could be inserted into the
        // ordered sequence maintained by this set, while preserving its
        // ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this set that are equivalent to
        // the specified 'key', where the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  The first returned iterator will be
        // 'lower_bound(key)'; the second returned iterator will be
        // 'upper_bound(key)'; and, if this set contains no 'value_type'
        // objects equivalent to 'key', then the two returned iterators will
        // have the same value.  Note that since a set maintains unique keys,
        // the range will contain at most one element.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        const_iterator startIt = lower_bound(key);
        const_iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            ++endIt;
        }
        return pair<const_iterator, const_iterator>(startIt, endIt);
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        pair<const_iterator, const_iterator> >::type
    equal_range(const LOOKUP_KEY& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this set that are equivalent to
        // the specified 'key', where the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  The first returned iterator will be
        // 'lower_bound(key)'; the second returned iterator will be
        // 'upper_bound(key)'; and, if this set contains no 'value_type'
        // objects equivalent to 'key', then the two returned iterators will
        // have the same value.  Note that although a set maintains unique
        // keys, the range may contain more than one element, because a
        // transparent comparator may have been supplied that provides a
        // different (but compatible) partitioning of keys for 'LOOKUP_KEY' as
        // the comparisons used to order the keys in the set.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        const_iterator startIt = lower_bound(key);
        const_iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            ++endIt;

            // Typically, even with a transparent comparator, we expect to find
            // either 0 or 1 matching keys. We test for those two common cases
            // before performing a logarithmic search via 'upper_bound' to
            // determine the end of the range.

            if (endIt != end() && !comparator()(key, *endIt.node())) {
                endIt = upper_bound(key);
            }
        }
        return pair<const_iterator, const_iterator>(startIt, endIt);
    }

    // BDE_VERIFY pragma: pop
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template <
    class INPUT_ITERATOR,
    class KEY = typename bsl::iterator_traits<INPUT_ITERATOR>::value_type,
    class COMPARATOR = std::less<KEY>,
    class ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<!bsl::IsStdAllocator_v<COMPARATOR>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
set(INPUT_ITERATOR,
    INPUT_ITERATOR,
    COMPARATOR = COMPARATOR(),
    ALLOCATOR = ALLOCATOR())
-> set<KEY, COMPARATOR, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'set'.  Deduce the template
    // parameters 'COMPARATOR' and 'ALLOCATOR' from the other parameters passed
    // to the constructor.  This guide does not participate unless the
    // supplied (or defaulted) 'ALLOCATOR' meets the requirements of a
    // standard allocator.

template <
    class INPUT_ITERATOR,
    class COMPARATOR,
    class ALLOC,
    class KEY =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
set(INPUT_ITERATOR, INPUT_ITERATOR, COMPARATOR, ALLOC *)
-> set<KEY, COMPARATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'set'.  Deduce the template
    // parameter 'COMPARATOR' from the other parameter passed to the
    // constructor.  This deduction guide does not participate unless the
    // specified 'ALLOC' is convertible to 'bsl::allocator<KEY>'.

template <
    class INPUT_ITERATOR,
    class ALLOCATOR,
    class KEY =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
set(INPUT_ITERATOR, INPUT_ITERATOR, ALLOCATOR)
-> set<KEY, std::less<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'set'.  Deduce the template
    // parameter 'ALLOCATOR' from the other parameter passed to the
    // constructor.  This deduction guide does not participate unless the
    // supplied allocator meets the requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class ALLOC,
    class KEY =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
set(INPUT_ITERATOR, INPUT_ITERATOR, ALLOC *)
-> set<KEY>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'set'.  This deduction guide
    // does not participate unless the specified 'ALLOC' is convertible to
    // 'bsl::allocator<KEY>'.

template <
    class KEY,
    class COMPARATOR = std::less<KEY>,
    class ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<!bsl::IsStdAllocator_v<COMPARATOR>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
set(std::initializer_list<KEY>,
    COMPARATOR = COMPARATOR(),
    ALLOCATOR = ALLOCATOR())
-> set<KEY, COMPARATOR, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'set'.  Deduce the
    // template parameters 'COMPARATOR' and 'ALLOCATOR' from the other
    // parameters passed to the constructor.

template <
    class KEY,
    class COMPARATOR,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
set(std::initializer_list<KEY>, COMPARATOR, ALLOC *)
-> set<KEY, COMPARATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'set'.  Deduce the
    // template parameter 'COMPARATOR' from the other parameter passed to the
    // constructor.  This deduction guide does not participate unless the
    // specified 'ALLOC' is convertible to 'bsl::allocator<KEY>'.

template <
    class KEY,
    class ALLOCATOR,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
set(std::initializer_list<KEY>, ALLOCATOR)
-> set<KEY, std::less<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'set'.  Deduce the
    // template parameter 'ALLOCATOR' from the other parameter passed to the
    // constructor.

template <
    class KEY,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
set(std::initializer_list<KEY>, ALLOC *)
-> set<KEY>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'set'.  This deduction
    // guide does not participate unless the specified 'ALLOC' is convertible
    // to 'bsl::allocator<KEY>'.

#endif

// FREE OPERATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator==(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'set' objects 'lhs' and 'rhs' have
    // the same value if they have the same number of keys, and each element
    // in the ordered sequence of keys of 'lhs' has the same value as the
    // corresponding element in the ordered sequence of keys of 'rhs'.  This
    // method requires that the (template parameter) type 'KEY' be
    // 'equality-comparable' (see {Requirements on 'KEY'}).

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator!=(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'set' objects 'lhs' and 'rhs' do
    // not have the same value if they do not have the same number of keys, or
    // some element in the ordered sequence of keys of 'lhs' does not have the
    // same value as the corresponding element in the ordered sequence of keys
    // of 'rhs'.  This method requires that the (template parameter) type 'KEY'
    // be 'equality-comparable' (see {Requirements on 'KEY'}).
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class KEY, class COMPARATOR, class ALLOCATOR>
BloombergLP::bslalg::SynthThreeWayUtil::Result<KEY>
operator<=>(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
            const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Perform a lexicographic three-way comparison of the specified 'lhs' and
    // the specified 'rhs' sets by using the comparison operators of 'KEY' on
    // each element; return the result of that comparison.

#else

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator< (const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' set is
    // lexicographically less than that of the specified 'rhs' set, and 'false'
    // otherwise.  Given iterators 'i' and 'j' over the respective sequences
    // '[lhs.begin() .. lhs.end())' and '[rhs.begin() .. rhs.end())', the value
    // of set 'lhs' is lexicographically less than that of set 'rhs' if
    // 'true == *i < *j' for the first pair of corresponding iterator positions
    // where '*i < *j' and '*j < *i' are not both 'false'.  If no such
    // corresponding iterator position exists, the value of 'lhs' is
    // lexicographically less than that of 'rhs' if 'lhs.size() < rhs.size()'.
    // This method requires that 'operator<', inducing a total order, be
    // defined for 'value_type'.

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator> (const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' set is
    // lexicographically greater than that of the specified 'rhs' set, and
    // 'false' otherwise.  The value of set 'lhs' is lexicographically greater
    // than that of set 'rhs' if 'rhs' is lexicographically less than 'lhs'
    // (see 'operator<').  This method requires that 'operator<', inducing a
    // total order, be defined for 'value_type'.  Note that this operator
    // returns 'rhs < lhs'.

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator<=(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' set is
    // lexicographically less than or equal to that of the specified 'rhs' set,
    // and 'false' otherwise.  The value of set 'lhs' is lexicographically less
    // than or equal to that of set 'rhs' if 'rhs' is not lexicographically
    // less than 'lhs' (see 'operator<').  This method requires that
    // 'operator<', inducing a total order, be defined for 'value_type'.  Note
    // that this operator returns '!(rhs < lhs)'.

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator>=(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' set is
    // lexicographically greater than or equal to that of the specified 'rhs'
    // set, and 'false' otherwise.  The value of set 'lhs' is lexicographically
    // greater than or equal to that of set 'rhs' if 'lhs' is not
    // lexicographically less than 'rhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(lhs < rhs)'.

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class KEY, class COMPARATOR, class ALLOCATOR, class PREDICATE>
typename set<KEY, COMPARATOR, ALLOCATOR>::size_type
erase_if(set<KEY, COMPARATOR, ALLOCATOR>& s, PREDICATE predicate);
    // Erase all the elements in the specified set 's' that satisfy the
    // specified predicate 'predicate'.  Return the number of elements erased.

template <class KEY, class COMPARATOR, class ALLOCATOR>
void swap(set<KEY, COMPARATOR, ALLOCATOR>& a,
          set<KEY, COMPARATOR, ALLOCATOR>& b)
                                BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                    BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
    // Exchange the value and comparator of the specified 'a' object with those
    // of the specified 'b' object; also exchange the allocator of 'a' with
    // that of 'b' if the (template parameter) type 'ALLOCATOR' has the
    // 'propagate_on_container_swap' trait, and do not modify either allocator
    // otherwise.  This function provides the no-throw exception-safety
    // guarantee if and only if the (template parameter) type 'COMPARATOR'
    // provides a no-throw swap operation, and provides the basic
    // exception-safety guarantee otherwise; if an exception is thrown, both
    // objects are left in valid but unspecified states.  This operation has
    // 'O[1]' complexity if either 'a' was created with the same allocator as
    // 'b' or 'ALLOCATOR' has the 'propagate_on_container_swap' trait;
    // otherwise, it has 'O[n + m]' complexity, where 'n' and 'm' are the
    // number of elements in 'a' and 'b', respectively.  Note that this
    // function's support for swapping objects created with different
    // allocators when 'ALLOCATOR' does not have the
    // 'propagate_on_container_swap' trait is a departure from the C++
    // Standard.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -----------------
                             // class DataWrapper
                             // -----------------

// CREATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::DataWrapper::DataWrapper(
                                              const COMPARATOR& comparator,
                                              const ALLOCATOR&  basicAllocator)
: ::bsl::set<KEY, COMPARATOR, ALLOCATOR>::Comparator(comparator)
, d_pool(basicAllocator)
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::DataWrapper::DataWrapper(
                          BloombergLP::bslmf::MovableRef<DataWrapper> original)
: ::bsl::set<KEY, COMPARATOR, ALLOCATOR>::Comparator(
                                    MoveUtil::access(original).keyComparator())
, d_pool(MoveUtil::move(MoveUtil::access(original).d_pool))
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::NodeFactory&
set<KEY, COMPARATOR, ALLOCATOR>::DataWrapper::nodeFactory()
{
    return d_pool;
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
const typename set<KEY, COMPARATOR, ALLOCATOR>::NodeFactory&
set<KEY, COMPARATOR, ALLOCATOR>::DataWrapper::nodeFactory() const
{
    return d_pool;
}

                             // ---------
                             // class set
                             // ---------

// PRIVATE MANIPULATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::NodeFactory&
set<KEY, COMPARATOR, ALLOCATOR>::nodeFactory()
{
    return d_compAndAlloc.nodeFactory();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::Comparator&
set<KEY, COMPARATOR, ALLOCATOR>::comparator()
{
    return d_compAndAlloc;
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void set<KEY, COMPARATOR, ALLOCATOR>::quickSwapExchangeAllocators(set& other)
{
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &other.d_tree);
    nodeFactory().swapExchangeAllocators(other.nodeFactory());

    // 'DataWrapper' contains a 'NodeFactory' object and inherits from
    // 'Comparator'.  If the empty-base-class optimization has been applied to
    // 'Comparator', then we must not call 'swap' on it because
    // 'sizeof(Comparator) > 0' and, therefore, we will incorrectly swap bytes
    // of the 'NodeFactory' members!

    if (sizeof(NodeFactory) != sizeof(DataWrapper)) {
        comparator().swap(other.comparator());
    }
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void set<KEY, COMPARATOR, ALLOCATOR>::quickSwapRetainAllocators(set& other)
{
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &other.d_tree);
    nodeFactory().swapRetainAllocators(other.nodeFactory());

    // See 'quickSwapExchangeAllocators' (above).

    if (sizeof(NodeFactory) != sizeof(DataWrapper)) {
        comparator().swap(other.comparator());
    }
}

// PRIVATE ACCESSORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
const typename set<KEY, COMPARATOR, ALLOCATOR>::NodeFactory&
set<KEY, COMPARATOR, ALLOCATOR>::nodeFactory() const
{
    return d_compAndAlloc.nodeFactory();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
const typename set<KEY, COMPARATOR, ALLOCATOR>::Comparator&
set<KEY, COMPARATOR, ALLOCATOR>::comparator() const
{
    return d_compAndAlloc;
}

// CREATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set()
: d_compAndAlloc(COMPARATOR(), ALLOCATOR())
, d_tree()
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(const ALLOCATOR& basicAllocator)
: d_compAndAlloc(COMPARATOR(), basicAllocator)
, d_tree()
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(const set& original)
: d_compAndAlloc(original.comparator().keyComparator(),
                 AllocatorTraits::select_on_container_copy_construction(
                                          original.nodeFactory().allocator()))
, d_tree()
{
    if (0 < original.size()) {
        nodeFactory().reserveNodes(original.size());
        BloombergLP::bslalg::RbTreeUtil::copyTree(&d_tree,
                                                  original.d_tree,
                                                  &nodeFactory());
    }
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(
                                  BloombergLP::bslmf::MovableRef<set> original)
: d_compAndAlloc(MoveUtil::move(MoveUtil::access(original).d_compAndAlloc))
, d_tree()
{
    set& lvalue = original;
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &lvalue.d_tree);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(
                 const set&                                     original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_compAndAlloc(original.comparator().keyComparator(), basicAllocator)
, d_tree()
{
    if (0 < original.size()) {
        nodeFactory().reserveNodes(original.size());
        BloombergLP::bslalg::RbTreeUtil::copyTree(&d_tree,
                                                  original.d_tree,
                                                  &nodeFactory());
    }
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(
                 BloombergLP::bslmf::MovableRef<set>            original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_compAndAlloc(MoveUtil::access(original).comparator().keyComparator(),
                 basicAllocator)
, d_tree()
{
    set& lvalue = original;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
              nodeFactory().allocator() == lvalue.nodeFactory().allocator())) {
        d_compAndAlloc.nodeFactory().adopt(
                          MoveUtil::move(lvalue.d_compAndAlloc.nodeFactory()));
        BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &lvalue.d_tree);
    }
    else {
        if (0 < lvalue.size()) {
            nodeFactory().reserveNodes(lvalue.size());
            BloombergLP::bslalg::RbTreeUtil::moveTree(&d_tree,
                                                      &lvalue.d_tree,
                                                      &nodeFactory(),
                                                      &lvalue.nodeFactory());
        }
    }
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(INPUT_ITERATOR    first,
                                     INPUT_ITERATOR    last,
                                     const COMPARATOR& comparator,
                                     const ALLOCATOR&  basicAllocator)
: d_compAndAlloc(comparator, basicAllocator)
, d_tree()
{
    if (first != last) {

        size_type numElements =
                BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);

        if (0 < numElements) {
            nodeFactory().reserveNodes(numElements);
        }

        BloombergLP::bslalg::RbTreeUtilTreeProctor<NodeFactory> proctor(
                                                               &d_tree,
                                                               &nodeFactory());

        // The following loop guarantees amortized linear time to insert an
        // ordered sequence of values (as required by the standard).   If the
        // values are in sorted order, we are guaranteed the next node can be
        // inserted as the right child of the previous node, and can call
        // 'insertAt' without 'findUniqueInsertLocation'.

        insert(*first);
        BloombergLP::bslalg::RbTreeNode *prevNode = d_tree.rootNode();
        while (++first != last) {
            // The values are not in order, so insert them normally.

            const value_type& value = *first;
            if (this->comparator()(value, *prevNode)) {
                insert(value);
                insert(++first, last);
                break;
            }

            if (this->comparator()(*prevNode, value)) {
                BloombergLP::bslalg::RbTreeNode *node =
                                       nodeFactory().emplaceIntoNewNode(value);
                BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                                          prevNode,
                                                          false,
                                                          node);
                prevNode = node;
            }
        }

        proctor.release();
    }
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(INPUT_ITERATOR    first,
                                     INPUT_ITERATOR    last,
                                     const ALLOCATOR&  basicAllocator)
: d_compAndAlloc(COMPARATOR(), basicAllocator)
, d_tree()
{
    if (first != last) {

        size_type numElements =
                BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);

        if (0 < numElements) {
            nodeFactory().reserveNodes(numElements);
        }

        BloombergLP::bslalg::RbTreeUtilTreeProctor<NodeFactory> proctor(
                                                               &d_tree,
                                                               &nodeFactory());

        // The following loop guarantees amortized linear time to insert an
        // ordered sequence of values (as required by the standard).   If the
        // values are in sorted order, we are guaranteed the next node can be
        // inserted as the right child of the previous node, and can call
        // 'insertAt' without 'findUniqueInsertLocation'.

        insert(*first);
        BloombergLP::bslalg::RbTreeNode *prevNode = d_tree.rootNode();
        while (++first != last) {
            // The values are not in order, so insert them normally.

            const value_type& value = *first;
            if (this->comparator()(value, *prevNode)) {
                insert(value);
                insert(++first, last);
                break;
            }

            if (this->comparator()(*prevNode, value)) {
                BloombergLP::bslalg::RbTreeNode *node =
                                       nodeFactory().emplaceIntoNewNode(value);
                BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                                          prevNode,
                                                          false,
                                                          node);
                prevNode = node;
            }
        }

        proctor.release();
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(std::initializer_list<KEY> values,
                                     const COMPARATOR&          comparator,
                                     const ALLOCATOR&           basicAllocator)
: set(values.begin(), values.end(), comparator, basicAllocator)
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(std::initializer_list<KEY> values,
                                     const ALLOCATOR&           basicAllocator)
: set(values.begin(), values.end(), COMPARATOR(), basicAllocator)
{
}
#endif

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::~set()
{
    clear();
}

// MANIPULATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>&
set<KEY, COMPARATOR, ALLOCATOR>::operator=(const set& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        if (AllocatorTraits::propagate_on_container_copy_assignment::value) {
            set other(rhs, rhs.nodeFactory().allocator());
            quickSwapExchangeAllocators(other);
        }
        else {
            set other(rhs, nodeFactory().allocator());
            quickSwapRetainAllocators(other);
        }
    }
    return *this;
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>&
set<KEY, COMPARATOR, ALLOCATOR>::operator=(
                                       BloombergLP::bslmf::MovableRef<set> rhs)
                        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                            AllocatorTraits::is_always_equal::value
                         && std::is_nothrow_move_assignable<COMPARATOR>::value)
{
    set& lvalue = rhs;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &lvalue)) {
        if (nodeFactory().allocator() == lvalue.nodeFactory().allocator()) {
            set other(MoveUtil::move(lvalue));
            quickSwapRetainAllocators(other);
        }
        else if (
              AllocatorTraits::propagate_on_container_move_assignment::value) {
            set other(MoveUtil::move(lvalue));
            quickSwapExchangeAllocators(other);
        }
        else {
            set other(MoveUtil::move(lvalue), nodeFactory().allocator());
            quickSwapRetainAllocators(other);
        }
    }
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>&
set<KEY, COMPARATOR, ALLOCATOR>::operator=(std::initializer_list<KEY> values)
{
    clear();
    insert(values.begin(), values.end());
    return *this;
}
#endif

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::begin() BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_tree.firstNode());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::end() BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_tree.sentinel());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::rbegin() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::rend() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(begin());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
pair<typename set<KEY, COMPARATOR, ALLOCATOR>::iterator, bool>
set<KEY, COMPARATOR, ALLOCATOR>::insert(const value_type& value)
{
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                                            &comparisonResult,
                                                            &d_tree,
                                                            this->comparator(),
                                                            value);
    if (!comparisonResult) {
        return pair<iterator, bool>(iterator(insertLocation), false);
                                                                      // RETURN
    }
    BloombergLP::bslalg::RbTreeNode *node =
                                       nodeFactory().emplaceIntoNewNode(value);
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
pair<typename set<KEY, COMPARATOR, ALLOCATOR>::iterator, bool>
set<KEY, COMPARATOR, ALLOCATOR>::insert(
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    value_type& lvalue = value;
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                                            &comparisonResult,
                                                            &d_tree,
                                                            this->comparator(),
                                                            lvalue);
    if (!comparisonResult) {
        return pair<iterator, bool>(iterator(insertLocation), false);
                                                                      // RETURN
    }
    BloombergLP::bslalg::RbTreeNode *node =
                      nodeFactory().emplaceIntoNewNode(MoveUtil::move(lvalue));
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::insert(const_iterator    hint,
                                        const value_type& value)
{
    BloombergLP::bslalg::RbTreeNode *hintNode =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                                            &comparisonResult,
                                                            &d_tree,
                                                            this->comparator(),
                                                            value,
                                                            hintNode);
    if (!comparisonResult) {
        return iterator(insertLocation);                              // RETURN
    }

    BloombergLP::bslalg::RbTreeNode *node =
                                       nodeFactory().emplaceIntoNewNode(value);
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return iterator(node);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::insert(const_iterator    hint,
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    value_type& lvalue = value;
    BloombergLP::bslalg::RbTreeNode *hintNode =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                                            &comparisonResult,
                                                            &d_tree,
                                                            this->comparator(),
                                                            lvalue,
                                                            hintNode);
    if (!comparisonResult) {
        return iterator(insertLocation);                              // RETURN
    }

    BloombergLP::bslalg::RbTreeNode *node =
                      nodeFactory().emplaceIntoNewNode(MoveUtil::move(lvalue));

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return iterator(node);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
void set<KEY, COMPARATOR, ALLOCATOR>::insert(INPUT_ITERATOR first,
                                             INPUT_ITERATOR last)
{
    ///Implementation Notes
    ///--------------------
    // First, consume currently held free nodes.  If those nodes are
    // insufficient *and* one can calculate the remaining number of elements,
    // then reserve exactly that many free nodes.  There is no more than one
    // call to 'reserveNodes' per invocation of this method, hence the use of
    // 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'.  When reserving nodes, we
    // assume the elements remaining to be inserted are unique and do not
    // duplicate any elements already in the container.  If there are any
    // duplicates, this container will have free nodes on return from this
    // method.

    const bool canCalculateInsertDistance =
             is_convertible<typename
                            iterator_traits<INPUT_ITERATOR>::iterator_category,
                            forward_iterator_tag>::value;

    while (first != last) {
        if (canCalculateInsertDistance
        && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                              !nodeFactory().hasFreeNodes())) {
            const size_type numElements =
                BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);

            nodeFactory().reserveNodes(numElements);
        }
        insert(*first);
        ++first;
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void set<KEY, COMPARATOR, ALLOCATOR>::insert(std::initializer_list<KEY> values)
{
    insert(values.begin(), values.end());
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
pair<typename set<KEY, COMPARATOR, ALLOCATOR>::iterator, bool>
set<KEY, COMPARATOR, ALLOCATOR>::emplace(Args&&... arguments)
{
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
        BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                     &comparisonResult,
                                     &d_tree,
                                     this->comparator(),
                                     static_cast<const Node *>(node)->value());
    if (!comparisonResult) {
        nodeFactory().deleteNode(node);
        return pair<iterator, bool>(iterator(insertLocation), false);
                                                                      // RETURN
    }
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::emplace_hint(const_iterator hint,
                                              Args&&...      arguments)
{
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
        BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    BloombergLP::bslalg::RbTreeNode *hintNode =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                                            &comparisonResult,
                                                            &d_tree,
                                                            this->comparator(),
                                      static_cast<const Node *>(node)->value(),
                                                            hintNode);
    if (!comparisonResult) {
        nodeFactory().deleteNode(node);
        return iterator(insertLocation);                              // RETURN
    }

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return iterator(node);
}
#endif

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT_SAFE(position != end());

    BloombergLP::bslalg::RbTreeNode *node =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(position.node());
    BloombergLP::bslalg::RbTreeNode *result =
                                   BloombergLP::bslalg::RbTreeUtil::next(node);
    BloombergLP::bslalg::RbTreeUtil::remove(&d_tree, node);
    nodeFactory().deleteNode(node);
    return iterator(result);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::size_type
set<KEY, COMPARATOR, ALLOCATOR>::erase(const key_type& key)
{
    const_iterator it = find(key);
    if (it == end()) {
        return 0;                                                     // RETURN
    }
    erase(it);
    return 1;
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::erase(const_iterator first,
                                       const_iterator last)
{
    while (first != last) {
        first = erase(first);
    }
    return iterator(last.node());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void set<KEY, COMPARATOR, ALLOCATOR>::swap(set& other)
                              BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                  AllocatorTraits::is_always_equal::value
                               && bsl::is_nothrow_swappable<COMPARATOR>::value)
{
    if (AllocatorTraits::propagate_on_container_swap::value) {
        quickSwapExchangeAllocators(other);
    }
    else {
        // C++11 behavior for member 'swap': undefined for unequal allocators.
        // BSLS_ASSERT(allocator() == other.allocator());

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
               nodeFactory().allocator() == other.nodeFactory().allocator())) {
            quickSwapRetainAllocators(other);
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            set toOtherCopy(MoveUtil::move(*this),
                            other.nodeFactory().allocator());
            set toThisCopy(MoveUtil::move(other), nodeFactory().allocator());

            this->quickSwapRetainAllocators(toThisCopy);
            other.quickSwapRetainAllocators(toOtherCopy);
        }
    }
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void set<KEY, COMPARATOR, ALLOCATOR>::clear() BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT_SAFE(d_tree.firstNode());

    if (d_tree.rootNode()) {
        BSLS_ASSERT_SAFE(0 < d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() != d_tree.sentinel());

        BloombergLP::bslalg::RbTreeUtil::deleteTree(&d_tree, &nodeFactory());
    }
#if defined(BSLS_ASSERT_SAFE_IS_USED)
    else {
        BSLS_ASSERT_SAFE(0 == d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() == d_tree.sentinel());
    }
#endif
}

// ACCESSORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::allocator_type
set<KEY, COMPARATOR, ALLOCATOR>::get_allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return nodeFactory().allocator();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return cbegin();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return cend();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return crbegin();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return crend();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_tree.firstNode());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_tree.sentinel());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
bool set<KEY, COMPARATOR, ALLOCATOR>::contains(const key_type& key) const
{
    return find(key) != end();
}


// capacity:
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
bool set<KEY, COMPARATOR, ALLOCATOR>::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_tree.numNodes();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::size_type
set<KEY, COMPARATOR, ALLOCATOR>::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_tree.numNodes();
}


template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::size_type
set<KEY, COMPARATOR, ALLOCATOR>::max_size() const BSLS_KEYWORD_NOEXCEPT
{
    return AllocatorTraits::max_size(get_allocator());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::key_compare
set<KEY, COMPARATOR, ALLOCATOR>::key_comp() const
{
    return comparator().keyComparator();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::value_compare
set<KEY, COMPARATOR, ALLOCATOR>::value_comp() const
{
    return value_compare(key_comp());
}

}  // close namespace bsl

// FREE OPERATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
bool bsl::operator==(const bsl::set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::set<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator!=(const bsl::set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::set<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
BloombergLP::bslalg::SynthThreeWayUtil::Result<KEY>
bsl::operator<=>(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                 const set<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return bsl::lexicographical_compare_three_way(
                              lhs.begin(),
                              lhs.end(),
                              rhs.begin(),
                              rhs.end(),
                              BloombergLP::bslalg::SynthThreeWayUtil::compare);
}

#else

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator<(const bsl::set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                    const bsl::set<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return 0 > BloombergLP::bslalg::RangeCompare::lexicographical(lhs.begin(),
                                                                  lhs.end(),
                                                                  lhs.size(),
                                                                  rhs.begin(),
                                                                  rhs.end(),
                                                                  rhs.size());
}

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator>(const bsl::set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                    const bsl::set<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return rhs < lhs;
}

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator<=(const bsl::set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::set<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(rhs < lhs);
}


template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator>=(const bsl::set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::set<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs < rhs);
}

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class KEY,  class COMPARATOR,  class ALLOCATOR, class PREDICATE>
inline
typename bsl::set<KEY, COMPARATOR, ALLOCATOR>::size_type
bsl::erase_if(set<KEY, COMPARATOR, ALLOCATOR>& s, PREDICATE predicate)
{
    return BloombergLP::bslstl::AlgorithmUtil::containerEraseIf(s, predicate);
}

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
void bsl::swap(bsl::set<KEY, COMPARATOR, ALLOCATOR>& a,
               bsl::set<KEY, COMPARATOR, ALLOCATOR>& b)
                                 BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)))
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *ordered* containers:
//: o An ordered container defines STL iterators.
//: o An ordered container uses 'bslma' allocators if the (template parameter)
//:   type 'ALLOCATOR' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
struct HasStlIterators<bsl::set<KEY, COMPARATOR, ALLOCATOR> >
    : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
struct UsesBslmaAllocator<bsl::set<KEY, COMPARATOR, ALLOCATOR> >
    : bsl::is_convertible<Allocator*, ALLOCATOR>
{};

}  // close namespace bslma

}  // close enterprise namespace

#endif // End C++11 code

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
