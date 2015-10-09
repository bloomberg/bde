// bslstl_set.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLSTL_SET
#define INCLUDED_BSLSTL_SET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant set class.
//
//@CLASSES:
//   bsl::set: STL-compatible set template
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
// to the C++11 standard, except that it does not have interfaces that take
// rvalue references, 'initializer_lists', 'emplace', or operations taking a
// variadic number of template parameters.  Note that excluded C++11 features
// are those that require (or are greatly simplified by) C++11 compiler
// support.
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
//: "default-constructible": The type provides a default constructor.
//:
//: "copy-constructible": The type provides a copy constructor.
//:
//: "equality-comparable": The type provides an equality-comparison operator
//:     that defines an equivalence relationship and is both reflexive and
//:     transitive.
//:
//: "less-than-comparable": The type provides a less-than operator, which
//:     defines a strict weak ordering relation on values of the type.
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
// If the (template parameter) type 'ALLOCATOR' of an 'set' instantiation' is
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
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'c'             - comparator providing an ordering for objects of type 'K'
//  'al             - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'k'             - an object of type 'K'
//  'p1', 'p2'      - two iterators belonging to 'a'
//  distance(i1,i2) - the number of elements in the range [i1, i2)
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | set<K> a;    (default construction)                | O[1]               |
//  | set<K> a(al);                                      |                    |
//  | set<K> a(c, al);                                   |                    |
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
//  | a = b;       (assignment)                          | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  | a.rbegin(), a.rend(), a.crbegin(), a.crend()       |                    |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a < b, a <= b, a > b, a >= b                       | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a,b)                               | O[1] if 'a' and    |
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
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, k)                                    | amortized constant |
//  |                                                    | if the value is    |
//  |                                                    | inserted right     |
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
//      // ~MyDate() = default;
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

#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_set.h> instead of <bslstl_set.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLSTL_SETCOMPARATOR
#include <bslstl_setcomparator.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLSTL_TREEITERATOR
#include <bslstl_treeiterator.h>
#endif

#ifndef INCLUDED_BSLSTL_TREENODE
#include <bslstl_treenode.h>
#endif

#ifndef INCLUDED_BSLSTL_TREENODEPOOL
#include <bslstl_treenodepool.h>
#endif

#ifndef INCLUDED_BSLALG_RANGECOMPARE
#include <bslalg_rangecompare.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREEANCHOR
#include <bslalg_rbtreeanchor.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREEUTIL
#include <bslalg_rbtreeutil.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

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
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

    // PRIVATE TYPES
    typedef const KEY                                           ValueType;
        // This typedef is an alias for the type of key objects maintained by
        // this set.

    typedef BloombergLP::bslstl::SetComparator<KEY, COMPARATOR> Comparator;
        // This typedef is an alias for the comparator used internally by this
        // set.

    typedef BloombergLP::bslstl::TreeNode<KEY>                  Node;
        // This typedef is an alias for the type of nodes held by the tree (of
        // nodes) used to implement this set.

    typedef BloombergLP::bslstl::TreeNodePool<KEY, ALLOCATOR>   NodeFactory;
        // This typedef is an alias for the factory type used to create and
        // destroy 'Node' objects.

    typedef bsl::allocator_traits<ALLOCATOR>                   AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    struct DataWrapper : public Comparator {
        // This struct is wrapper around the comparator and allocator data
        // members.  It takes advantage of the empty-base optimization (EBO) so
        // that if the allocator is stateless, it takes up no space.
        //
        // TBD: This struct should eventually be replaced by the use of a
        // general EBO-enabled component that provides a 'pair'-like interface
        // or a 'tuple'.

        NodeFactory d_pool;  // pool of 'Node' objects

        explicit DataWrapper(const COMPARATOR&  comparator,
                             const ALLOCATOR&   basicAllocator);
            // Create a 'DataWrapper' object with the specified 'comparator'
            // and 'basicAllocator'.
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

    void quickSwap(set& other);
        // Efficiently exchange the value and comparator of this object with
        // the value of the specified 'other' object.  This method provides the
        // no-throw exception-safety guarantee.  The behavior is undefined
        // unless this object was created with the same allocator as 'other'.

    // PRIVATE ACCESSORS
    const NodeFactory& nodeFactory() const;
        // Return a reference providing non-modifiable access to the
        // node-allocator for this tree.

    const Comparator& comparator() const;
        // Return a reference providing non-modifiable access to the comparator
        // for this tree.

  public:
    // CREATORS
    explicit set(const COMPARATOR& comparator     = COMPARATOR(),
                 const ALLOCATOR&  basicAllocator = ALLOCATOR())
        // Construct an empty set.  Optionally specify a 'comparator' used to
        // order keys contained in this object.  If 'comparator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'COMPARATOR' is used.  Optionally specify the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the 'ALLOCATOR' argument is of type
        // 'bsl::allocator' (the default), then 'basicAllocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR'
        // argument is of type 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used to
        // supply memory.
    : d_compAndAlloc(comparator, basicAllocator)
    , d_tree()
    {
        // The implementation is placed here in the class definition to
        // workaround an AIX compiler bug, where the constructor can fail to
        // compile because it is unable to find the definition of the default
        // argument.  This occurs when a templatized class wraps around the
        // container and the comparator is defined after the new class.
    }

    explicit set(const ALLOCATOR& basicAllocator);
        // Construct an empty set that will use the specified 'basicAllocator'
        // to supply memory.  Use a default-constructed object of the (template
        // parameter) type 'COMPARATOR' to order the keys contained in this
        // set.  If the template parameter 'ALLOCATOR' argument is of type
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.

    set(const set& original);
        // Construct a set having the same value as the specified 'original'.
        // Use a copy of 'original.key_comp()' to order the keys contained in
        // this set.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.allocator())' to
        // allocate memory.  If the (template parameter) type 'ALLOCATOR' is of
        // type 'bsl::allocator' (the default), the currently installed default
        // allocator is used to supply memory.  Note that this method requires
        // that the (template parameter) type 'KEY' be "copy-constructible"
        // (see {Requirements on 'KEY'}).

    set(const set& original, const ALLOCATOR& basicAllocator);
        // Construct a set having the same value as that of the specified
        // 'original' that will use the specified 'basicAllocator' to supply
        // memory.  Use a copy of 'original.key_comp()' to order the keys
        // contained in this set.  If the template parameter 'ALLOCATOR'
        // argument is of type 'bsl::allocator' (the default), then
        // 'basicAllocator' shall be convertible to 'bslma::Allocator *'.  Note
        // that this method requires that the (template parameter) type 'KEY'
        // be "copy-constructible" (see {Requirements on 'KEY'}).

    template <class INPUT_ITERATOR>
    set(INPUT_ITERATOR    first,
        INPUT_ITERATOR    last,
        const COMPARATOR& comparator = COMPARATOR(),
        const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Construct a set, and insert each 'value_type' object in the sequence
        // starting at the specified 'first' element, and ending immediately
        // before the specified 'last' element, ignoring those keys that
        // appears earlier in the sequence.  Optionally specify a 'comparator'
        // used to order keys contained in this object.  If 'comparator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'COMPARATOR' is used.  Optionally specify the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the template parameter 'ALLOCATOR' argument
        // is of type 'bsl::allocator' (the default), then 'basicAllocator', if
        // supplied, shall be convertible to 'bslma::Allocator *'.  If the
        // template parameter 'ALLOCATOR' argument is of type 'bsl::allocator'
        // and 'basicAllocator' is not supplied, the currently installed
        // default allocator is used to supply memory.  If the sequence 'first'
        // and 'last' is ordered according to the identified 'comparator', then
        // this operation has 'O[N]' complexity, where 'N' is the number of
        // elements between 'first' and 'last', otherwise this operation has
        // 'O[N * log(N)]' complexity.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type'.  The behavior is undefined
        // unless 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.  Note that this method
        // requires that the (template parameter) type 'KEY' be
        // "copy-constructible" (see {Requirements on 'KEY'}).

    ~set();
        // Destroy this object.

    // MANIPULATORS
    set& operator=(const set& rhs);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_copy_assignment',
        // and return a reference providing modifiable access to this object.
        // Note that this method requires that the (template parameter) type
        // 'KEY' type be "copy-constructible" (see {Requirements on 'KEY'}).

    iterator begin();
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this set, or the 'end' iterator if this object is
        // empty.

    iterator end();
        // Return an iterator providing modifiable access to the past-the-end
        // element in the ordered sequence of 'value_type' objects maintained
        // by this set.

    reverse_iterator rbegin();
        // Return a reverse iterator providing modifiable access to the last
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this set, or 'rend' if this object is empty.

    reverse_iterator rend();
        // Return a reverse iterator providing modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this set.

    pair<iterator, bool> insert(const value_type& value);
        // Insert the specified 'value' into this set if the key (the 'first'
        // element) of the 'value' does not already exist in this set;
        // otherwise, if a 'value_type' object the same as 'value' already
        // exists in this set, this method has no effect.  Return a pair whose
        // 'first' member is an iterator referring to the (possibly newly
        // inserted) 'value_type' object in this set whose value is as that of
        // 'value', and whose 'second' member is 'true' if a new value was
        // inserted, and 'false' if the value was already present.  Note that
        // this method requires that the (template parameter) type 'KEY' be
        // "copy-constructible" (see {Requirements on 'KEY'}).

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this set (in amortized constant
        // time if the specified 'hint' is a valid immediate successor to
        // 'value'), if the 'value' does not already exist in this set;
        // otherwise, if a 'value_type' object the same as 'value' already
        // exists in this set, this method has no effect.  Return an iterator
        // referring to the (possibly newly inserted) 'value_type' object that
        // is the same as 'value'.  If 'hint' is not a valid immediate
        // successor to 'value', this operation has 'O[log(N)]' complexity,
        // where 'N' is the size of this set.  The behavior is undefined unless
        // 'hint' is a valid iterator into this set.  Note that this method
        // requires that the (template parameter) type 'KEY' be
        // "copy-constructible" (see {Requirements on 'KEY'}).

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this set the value of each 'value_type' object in the
        // range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator, whose key is not
        // already contained in this set.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type'.  The behavior is undefined
        // unless 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.  Note that this method
        // requires that the (template parameter) type 'KEY' be
        // "copy-constructible" (see {Requirements on 'KEY'}).

    iterator erase(const_iterator position);
        // Remove from this set the 'value_type' object at the specified
        // 'position', and return an iterator referring to the element
        // immediately following the removed element, or to the past-the-end
        // position if the removed element was the last element in the sequence
        // of elements maintained by this set.  The behavior is undefined
        // unless 'position' refers to a 'value_type' object in this set.

    size_type erase(const key_type& key);
        // Remote from this set the specified 'key', if it exists, and return
        // 1; otherwise, if there is no 'value_type' that is the same as 'key',
        // return 0 with no other effect.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this set the 'value_type' objects starting at the
        // specified 'first' position up to, but including the specified 'last'
        // position, and return 'last'.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this set or are the
        // 'end' iterator, and the 'first' position is at or before the 'last'
        // position in the ordered sequence provided by this container.

    void swap(set& other);
        // Exchange the value of this object as well as its comparator with
        // those of the specified 'other' object.  Additionally, if
        // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true', then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee and
        // guarantees 'O[1]' complexity.  The behavior is undefined unless
        // either this object was created with the same allocator as 'other' or
        // 'propagate_on_container_swap' is 'true'.

    void clear();
        // Remove all entries from this set.  Note that the set is empty after
        // this call, but allocated memory may be retained for future use.

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this set that is the same as the specified 'key', if such
        // an entry exists, and the past-the-end ('end') iterator otherwise.

    iterator lower_bound(const key_type& key);
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this set greater-than or
        // equal-to the specified 'key', and the past-the-end iterator if this
        // set does not contain a 'value_type' greater-than or equal-to 'key'.
        // Note that this function returns the *first* position before which
        // 'key' could be inserted into the ordered sequence maintained by this
        // set, while preserving its ordering.

    iterator upper_bound(const key_type& key);
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this set greater than the
        // specified 'key', and the past-the-end iterator if this set does not
        // contain a 'value_type' object whose key is greater-than 'key'.  Note
        // that this function returns the *last* position before which a 'key'
        // could be inserted into the ordered sequence maintained by this set,
        // while preserving its ordering.

    pair<iterator, iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this set the same as the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  The first returned iterator will be
        // 'lower_bound(key)'; the second returned iterator will be
        // 'upper_bound(key)'; and, if this set contains no 'value_type'
        // objects having 'key', then the two returned iterators will have the
        // same value.  Note that since a set maintains unique keys, the range
        // will contain at most one element.

    // ACCESSORS
    allocator_type get_allocator() const;
        // Return (a copy of) the allocator used for memory allocation by this
        // set.

    const_iterator begin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this set, or the 'end' iterator if this set is empty.

    const_iterator end() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type'
        // objects maintained by this set.

    const_reverse_iterator rbegin() const;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this set, or 'rend' if this object is empty.

    const_reverse_iterator rend() const;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this set.

    const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this set, or the 'cend' iterator if this set is empty.

    const_iterator cend() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this set.

    const_reverse_iterator crbegin() const;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this set, or 'crend' if this set is empty.

    const_reverse_iterator crend() const;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this set.

    bool empty() const;
        // Return 'true' if this set contains no elements, and 'false'
        // otherwise.

    size_type size() const;
        // Return the number of elements in this set.

    size_type max_size() const;
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

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this set that is the same as the specified
        // 'key', if such an entry exists, and the past-the-end ('end')
        // iterator otherwise.

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects within this set the the
        // same as the specified 'key'.  Note that since a set maintains unique
        // keys, the returned value will be either 0 or 1.

    const_iterator lower_bound(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this set greater-than
        // or equal-to the specified 'key', and the past-the-end iterator if
        // this set does not contain a 'value_type' greater-than or equal-to
        // 'key'.  Note that this function returns the *first* position before
        // which 'key' could be inserted into the ordered sequence maintained
        // by this set, while preserving its ordering.

    const_iterator upper_bound(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this set greater than
        // the specified 'key', and the past-the-end iterator if this set does
        // not contain a 'value_type' object whose key is greater-than 'key'.
        // Note that this function returns the *last* position before which a
        // 'key' could be inserted into the ordered sequence maintained by this
        // set, while preserving its ordering.

    pair<const_iterator, const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this set the same as the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  The first returned iterator will be
        // 'lower_bound(key)'; the second returned iterator will be
        // 'upper_bound(key)'; and, if this set contains no 'value_type'
        // objects having 'key', then the two returned iterators will have the
        // same value.  Note that since a set maintains unique keys, the range
        // will contain at most one element.

    // NOT IMPLEMENTED
        // The following methods are defined by the C++11 standard, but they
        // are not implemented as they require some level of C++11 compiler
        // support not currently available on all supported platforms.

//    set(set<KEY, COMPARATOR, ALLOCATOR>&& original);

//    set(set&&, const ALLOCATOR&);

//    set(initializer_list<value_type>,
//        const COMPARATOR& = COMPARATOR(),
//        const ALLOCATOR& = ALLOCATOR());

//    set<KEY, COMPARATOR, ALLOCATOR>& operator=(
//                                        set<KEY, COMPARATOR, ALLOCATOR>&& x);

//    set& operator=(initializer_list<value_type>);

//    template <class... Args> pair<iterator, bool> emplace(Args&&... args);

//    template <class... Args> iterator emplace_hint(const_iterator position,
//                                                   Args&&... args);

//     pair<iterator, bool> insert(value_type&& value);

//     iterator insert(const_iterator position, value_type&& value);

//     void insert(initializer_list<value_type>);

};

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator==(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'set' objects have the same value if
    // they have the same number of keys, and each key that is contained in one
    // of the objects is also contained in the other object.  Note that this
    // method requires that the (template parameter) type 'KEY' be
    // "equality-comparable" (see {Requirements on 'KEY'}).

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator!=(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'set' objects do not have the
    // same value if they do not have the same number of keys, or some keys
    // that is contained in one of the objects is not also contained in the
    // other object.  Note that this method requires that the (template
    // parameter) type 'KEY' be "equality-comparable" (see {Requirements on
    // 'KEY'}).

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator< (const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is less than the specified
    // 'rhs' value, and 'false' otherwise.  A set, 'lhs', has a value that is
    // less than that of 'rhs', if, for the first non-equal corresponding key
    // in their respective sequences, the 'lhs' key is less than the 'rhs' key,
    // or, if all their corresponding keys compare equal, 'lhs' has fewer keys
    // than 'rhs'.  Note that this method requires that the (template
    // parameter) type 'KEY' be "less-than-comparable" (see {Requirements on
    // 'KEY'}).

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator> (const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is greater than the specified
    // 'rhs' value, and 'false' otherwise.  A set, 'lhs', has a value that is
    // greater than that of 'rhs', if, for the first non-equal corresponding
    // key in their respective sequences, the 'lhs' key is greater than the
    // 'rhs' key, or, if all their keys compare equal, 'lhs' has more keys than
    // 'rhs'.  Note that this method requires that the (template parameter)
    // type 'KEY' be "less-than-comparable" (see {Requirements on 'KEY'}).

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator<=(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is less-than or equal-to the
    // specified 'rhs' value, and 'false' otherwise.  A set, 'lhs', has a value
    // that is less-than or equal-to that of 'rhs', if, for the first non-equal
    // corresponding key in their respective sequences, the 'lhs' key is less
    // than the 'rhs' key, or, if all of their corresponding keys compare
    // equal, 'lhs' has less-than or equal number of keys as 'rhs'.  Note that
    // this method requires that the (template parameter) type 'KEY' be
    // "less-than-comparable" (see {Requirements on 'KEY'}).

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator>=(const set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const set<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is greater-than or equal-to
    // the specified 'rhs' value, and 'false' otherwise.  A set, 'lhs', has a
    // value that is greater-than or equal-to that of 'rhs', if, for the first
    // corresponding key in their respective sequences, the 'lhs' key is
    // greater than the 'rhs' key, or, if all of their corresponding keys
    // compare equal, 'lhs' has greater-than or equal number of keys 'rhs'.
    // Note that this method requires that the (template parameter) type 'KEY'
    // be "less-than-comparable" (see {Requirements on 'KEY'}).

// specialized algorithms:
template <class KEY, class COMPARATOR, class ALLOCATOR>
void swap(set<KEY, COMPARATOR, ALLOCATOR>& a,
          set<KEY, COMPARATOR, ALLOCATOR>& b);
    // Swap both the value and the comparator of the specified 'a' object with
    // the value and comparator of the specified 'b' object.  Additionally, if
    // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true', then exchange the allocator of 'a' with that of 'b', and do not
    // modify either allocator otherwise.  This method provides the no-throw
    // exception-safety guarantee and guarantees 'O[1]' complexity.  The
    // behavior is undefined unless either this object was created with the
    // same allocator as 'other' or 'propagate_on_container_swap' is 'true'.

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

                             // ---------
                             // class set
                             // ---------

// PRIVATE MANIPULATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::NodeFactory&
set<KEY, COMPARATOR, ALLOCATOR>::nodeFactory()
{
    return d_compAndAlloc.d_pool;
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
void set<KEY, COMPARATOR, ALLOCATOR>::quickSwap(set& other)
{
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &other.d_tree);
    nodeFactory().swap(other.nodeFactory());

    // Work around to avoid the 1-byte swap problem on AIX for an empty class
    // under empty-base optimization.

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
    return d_compAndAlloc.d_pool;
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
                                               nodeFactory().createNode(value);
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
set<KEY, COMPARATOR, ALLOCATOR>::set(const ALLOCATOR& basicAllocator)
: d_compAndAlloc(COMPARATOR(), basicAllocator)
, d_tree()
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
set<KEY, COMPARATOR, ALLOCATOR>::set(const set&       original,
                                     const ALLOCATOR& basicAllocator)
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
            BloombergLP::bslalg::SwapUtil::swap(
                                             &nodeFactory().allocator(),
                                             &other.nodeFactory().allocator());
            quickSwap(other);
        }
        else {
            set other(rhs, nodeFactory().allocator());
            quickSwap(other);
        }
    }
    return *this;
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::begin()
{
    return iterator(d_tree.firstNode());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::end()
{
    return iterator(d_tree.sentinel());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::rbegin()
{
    return reverse_iterator(end());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::rend()
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
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().createNode(value);
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

    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().createNode(value);
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
    while (first != last) {
        insert(*first);
        ++first;
    }
}

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
{
    if (AllocatorTraits::propagate_on_container_swap::value) {
        BloombergLP::bslalg::SwapUtil::swap(&nodeFactory().allocator(),
                                            &other.nodeFactory().allocator());
        quickSwap(other);
    }
    else {
        // C++11 behavior: undefined for unequal allocators
        // BSLS_ASSERT(allocator() == other.allocator());

        // backward compatible behavior: swap with copies
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
               nodeFactory().allocator() == other.nodeFactory().allocator())) {
            quickSwap(other);
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            set thisCopy(*this, other.nodeFactory().allocator());
            set otherCopy(other, nodeFactory().allocator());

            quickSwap(otherCopy);
            other.quickSwap(thisCopy);
        }
    }
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void set<KEY, COMPARATOR, ALLOCATOR>::clear()
{
    BSLS_ASSERT_SAFE(d_tree.firstNode());
    if (d_tree.rootNode()) {
        BSLS_ASSERT_SAFE(0 < d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() != d_tree.sentinel());

        BloombergLP::bslalg::RbTreeUtil::deleteTree(&d_tree, &nodeFactory());
    }
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    else {
        BSLS_ASSERT_SAFE(0 == d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() == d_tree.sentinel());
    }
#endif
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::find(const key_type& key)
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::find(d_tree,
                                                          this->comparator(),
                                                          key));
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::lower_bound(const key_type& key)
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::iterator
set<KEY, COMPARATOR, ALLOCATOR>::upper_bound(const key_type& key)
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
pair<typename set<KEY, COMPARATOR, ALLOCATOR>::iterator,
          typename set<KEY, COMPARATOR, ALLOCATOR>::iterator>
set<KEY, COMPARATOR, ALLOCATOR>::equal_range(const key_type& key)
{
    iterator startIt = lower_bound(key);
    iterator endIt   = startIt;
    if (endIt != end() && !comparator()(key, *endIt.node())) {
        ++endIt;
    }
    return pair<iterator, iterator>(startIt, endIt);
}

// ACCESSORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::allocator_type
set<KEY, COMPARATOR, ALLOCATOR>::get_allocator() const
{
    return nodeFactory().allocator();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::begin() const
{
    return cbegin();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::end() const
{
    return cend();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::rbegin() const
{
    return crbegin();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::rend() const
{
    return crend();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::cbegin() const
{
    return const_iterator(d_tree.firstNode());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::cend() const
{
    return const_iterator(d_tree.sentinel());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
set<KEY, COMPARATOR, ALLOCATOR>::crend() const
{
    return const_reverse_iterator(begin());
}

// capacity:
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
bool set<KEY, COMPARATOR, ALLOCATOR>::empty() const
{
    return 0 == d_tree.numNodes();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::size_type
set<KEY, COMPARATOR, ALLOCATOR>::size() const
{
    return d_tree.numNodes();
}


template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::size_type
set<KEY, COMPARATOR, ALLOCATOR>::max_size() const
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

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::find(const key_type& key) const
{
    return const_iterator(
       BloombergLP::bslalg::RbTreeUtil::find(d_tree, this->comparator(), key));
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::size_type
set<KEY, COMPARATOR, ALLOCATOR>::count(const key_type& key) const
{
    return (find(key) != end()) ? 1 : 0;
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::lower_bound(const key_type& key) const
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator
set<KEY, COMPARATOR, ALLOCATOR>::upper_bound(const key_type& key) const
{
    return const_iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
pair<typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator,
          typename set<KEY, COMPARATOR, ALLOCATOR>::const_iterator>
set<KEY, COMPARATOR, ALLOCATOR>::equal_range(const key_type& key) const
{
    const_iterator startIt = lower_bound(key);
    const_iterator endIt   = startIt;
    if (endIt != end() && !comparator()(key, *endIt.node())) {
        ++endIt;
    }
    return pair<const_iterator, const_iterator>(startIt, endIt);
}

}  // close namespace bsl

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

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator!=(const bsl::set<KEY, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::set<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}

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

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
void bsl::swap(bsl::set<KEY, COMPARATOR, ALLOCATOR>& a,
               bsl::set<KEY, COMPARATOR, ALLOCATOR>& b)
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *ordered* containers:
//: o An ordered container defines STL iterators.
//: o An ordered container uses 'bslma' allocators if the parameterized
//:     'ALLOCATOR' is convertible from 'bslma::Allocator*'.

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
