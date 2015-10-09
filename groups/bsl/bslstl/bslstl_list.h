// bslstl_list.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_LIST
#define INCLUDED_BSLSTL_LIST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant list class.
//
//@CLASSES:
//   bsl::list: STL-compatible list template
//
//@SEE_ALSO: bslstl_deque
//
//@DESCRIPTION: This component defines a single class template, 'list',
// implementing the standard container holding a sequence of elements (of a
// template parameter type, 'VALUE').  All list operations involving single
// element are constant-time, including insertion and removal of elements
// anywhere in the list.  Operations that do not change the number of elements
// are performed without calling constructors, destructors, swap, or assignment
// on the individual elements.  (I.e., they are performed by
// pointer-manipulation alone.)  A 'list' does not provide random access to its
// elements; although access to the first and last element of a 'list' is
// constant-time, other elements can be accessed only by traversing the list
// (forwards or backwards) from the beginning or end.
//
// An instantiation of 'list' is an allocator-aware, in-core value-semantic
// type whose salient attributes are its size (number of elements) and the
// sequence of its contained element values (in order).  If 'list' is
// instantiated with a type that is not itself value-semantic, then it will not
// retain all of its value-semantic qualities.  In particular, if a type cannot
// be tested for equality, then a 'list' containing that type cannot be tested
// for equality.  It is even possible to instantiate 'list' with a type that
// does not have a copy-constructor, in which case the 'list' will not be
// copyable.
//
// A 'list' meets the requirements of a sequence container with bidirectional
// iterators in the C++ standard [23.3].  The 'list' implemented here adheres
// to the C++11 standard, except that it does not have interfaces that take
// rvalue references, 'initializer_list', 'emplace', or operations taking a
// variadic number of template parameters.  Note that excluded C++11 features
// are those that require C++11 compiler support.
//
///Memory Allocation
///-----------------
// The type supplied as a list's 'ALLOCATOR' template parameter determines how
// that list will allocate memory.  The 'list' template supports allocators
// meeting the requirements of the C++11 standard [17.6.3.5], in addition it
// supports scoped-allocators derived from the 'bslma::Allocator' memory
// allocation protocol.  Clients intending to use 'bslma' style allocators
// should use the template's default 'ALLOCATOR' type: The default type for the
// 'ALLOCATOR' template parameter, 'bsl::allocator', provides a C++11
// standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the parameterized 'ALLOCATOR' type of an 'list' instantiation is
// 'bsl::allocator', then objects of that list type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a list accepts
// an optional 'bslma::Allocator' argument at construction.  If the address of
// a 'bslma::Allocator' object is explicitly supplied at construction, it will
// be used to supply memory for the list throughout its lifetime; otherwise,
// the list will use the default allocator installed at the time of the list's
// construction (see 'bslma_default').  In addition to directly allocating
// memory from the indicated 'bslma::Allocator', a list supplies that
// allocator's address to the constructors of contained objects of the
// (template parameter) 'VALUE' type, if respectively, the parameterized types
// define the 'bslma::UsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'list':
//..
//  Legend
//  ------
//  'V'             - parameterized 'VALUE' type of the list
//  'a', 'b'        - two distinct objects of type 'list<V>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'value_type'    - list<V>::value_type
//  'al'            - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'v'             - an object of type 'V'
//  'p1', 'p2'      - two iterators belonging to 'a'
//  's1', 's2'      - two iterators belonging to 'b'
//  'pred'          - a unary predicate
//  'binary_pred'   - a binary predicate
//  'comp'          - a binary predicate implementing a strict-weak ordering
//  'args...'       - a variadic list of (up to 5) arguments
//  distance(i1,i2) - the number of elements in the range [i1, i2)
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | list<V> a;    (default construction)               | O(1)               |
//  | list<V> a(al);                                     |                    |
//  +----------------------------------------------------+--------------------+
//  | list<V> a(b); (copy construction)                  | O(n)               |
//  | list<V> a(b, al);                                  |                    |
//  +----------------------------------------------------+--------------------+
//  | list<V> a(n);                                      | O(n)               |
//  | list<V> a(n, value_type(v));                       |                    |
//  | list<V> a(n, value_type(v), al);                   |                    |
//  +----------------------------------------------------+--------------------+
//  | list<V> a(i1, i2);                                 | O(distance(i1,i2)) |
//  | list<V> a(i1, i2, al);                             |                    |
//  +----------------------------------------------------+--------------------+
//  | a.~list<V>(); (destruction)                        | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a = b;        (assignment)                         | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O(1)               |
//  | a.rbegin(), a.rend(), a.crbegin(), a.crend()       |                    |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a < b, a <= b, a > b, a >= b                       | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a,b)                               | O(1) if 'a' and 'b'|
//  |                                                    | use the same       |
//  |                                                    | allocator,         |
//  |                                                    | O[n + m] otherwise |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | get_allocator()                                    | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.emplace(p1, args...)                             | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, value_type(v))                        | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, n, value_type(v))                     | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, i1, i2)                               | O(distance(i1, i2))|
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | O(distance(p1, p2))|
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.assign(i1,i2)                                    | O(distance(i1, i2))|
//  +----------------------------------------------------+--------------------+
//  | a.assign(n, value_type(v))                         | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.front(), a.back()                                | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.emplace_front(args...), a.emplace_back(args...)  | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.push_front(value_type(v)),                       |                    |
//  | a.push_back(value_type(v))                         | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.pop_front(), a.pop_back()                        | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.resize(n), a.resize(n, value_type(v))            | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, b), a.splice(p, b, s1)                 | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, b, s1, s2)                             | O(distance(s1, s2))|
//  +----------------------------------------------------+--------------------+
//  | a.remove(t), a.remove_if(pred)                     | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.unique(), a.unique(binary_pred)                  | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.merge(b), a.merge(b, comp)                       | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.sort(), a.sort(comp)                             | O(n*log(n))        |
//  +----------------------------------------------------+--------------------+
//  | a.reverse()                                        | O(n)               |
//  +----------------------------------------------------+--------------------+
//..
//
///Thread Safety
///-------------
// 'list' is "'const' Thread-Safe [TS.2]" (see {'bsldoc_glossary'}).  Separate
// threads can safely access and modify separate 'list' objects.  Separate
// threads can safely perform 'const' operations on a single 'list' object.
// Separate threads cannot safely perform operations on a single 'list' object
// if at least one of those operations modifies the list.  If an iterator or
// reference to a list element is obtained in one thread, it may become
// invalidated if the list is modified in the same or another thread, even if
// the iterator is a 'const_iterator' or the reference is a
// reference-to-'const'.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Filter "Twinkle Star"
/// - - - - - - - - - - - - - - - -
// Suppose an observatory needs to analyze the results of a sky survey.  The
// raw data is a text file of star observations where each star is represented
// by a tuple of three numbers: (x, y, b), where x and y represent the angular
// coordinates of the star in the sky and b represents its brightness on a
// scale of 0 to 100.  A star having brightness 75 or higher is of particular
// interest, which is called "twinkle star".
//
// Our first example will read such a data file as described above, filter out
// the dim stars (brightness less than 75), and count the twinkle stars left
// in the list.  Our test data set has been selected such that there are 10
// stars in the set, of which 4 are sufficiently bright as to pass our filter.
//
// First, we define the class 'Star' that encapsulates a single tuple, and
// provides accessors functions 'x', 'y', and 'brightness', file I/O functions
// 'read' and 'write', and free operators '==', '!=', and '<':
//..
//  #include <cstdio>
//  using namespace std;
//
//  class Star
//      // This class represents a star as seen through a digital telescope.
//  {
//      // DATA
//      double d_x, d_y;     // coordinates
//
//      int    d_brightness; // brightness on a scale of 0 to 100
//
//  public:
//      // CREATORS
//      Star()
//          // Create a 'Star' object located at coordinates '(0, 0)' having
//          // '0' brightness.
//      : d_x(0), d_y(0), d_brightness(0)
//      {
//      }
//
//      Star(double x, double y, int b)
//          // Create a 'Star' object located at the specified coordinates
//          // '(x, y)' having the specified 'b' brightness.
//      : d_x(x), d_y(y), d_brightness(b)
//      {
//      }
//
//      // Compiler-generated copy construction, assignment, and destructor
//      // Star(const Star&) = default;
//      // Star& operator=(const Star&) = default;
//      // ~Star() = default;
//
//      // MANIPULATORS
//      bool read(FILE *input);
//          // Read x, y, and brightness from the specified 'input' file.
//          // Return 'true' if the read succeeded and 'false' otherwise.
//
//      void write(FILE *output) const;
//          // Write x, y, and brightness to the specified 'output' file
//          // followed by a newline.
//
//      // ACCESSORS
//      double x() const
//          // Return the x coordinate of this 'Star' object.
//      {
//          return d_x;
//      }
//
//      double y() const
//          // Return the y coordinate of this 'Star' object.
//      {
//          return d_y;
//      }
//
//      int brightness() const
//          // Return the brightness of this 'Star' object.
//      {
//          return d_brightness;
//      }
//  };
//
//  // FREE FUNCTIONS
//  bool operator==(const Star& lhs, const Star& rhs);
//  bool operator!=(const Star& lhs, const Star& rhs);
//  bool operator< (const Star& lhs, const Star& rhs);
//..
// Then, we define a 'readData' method that reads a file of data points and
// appends each onto a list.  The stars are stored in the data file in
// ascending sorted order by x and y coordinates.
//..
//  void readData(list<Star> *starList, FILE *input)
//  {
//      Star s;
//      while (s.read(input)) {
//          starList->push_back(s);
//      }
//  }
//..
// Now, we define the 'filter' method, which is responsible for removing stars
// with a brightness of less than 75 from the data set.  It does this by
// iterating over the list and erasing any element that does not pass the
// filter.  The list object features a fast 'erase' member function.  The
// return value of 'erase' is an iterator to the element immediately following
// the erased element:
//..
//  void filter(list<Star> *starList)
//  {
//      static const int threshold = 75;
//
//      list<Star>::iterator i = starList->begin();
//      while (i != starList->end()) {
//          if (i->brightness() < threshold) {
//              i = starList->erase(i);  // Erase and advance to next element.
//          }
//          else {
//              ++i;  // Advance to next element without erasing
//          }
//      }
//  }
//..
// Finally, we use the methods defined in above steps to put together our
// program to find twinkle stars:
//..
//  int usageExample1(int verbose)
//  {
//      FILE *input = fopen("star_data1.txt", "r");  // Open input file.
//      assert(input);
//
//      list<Star> starList;                         // Define a list of stars.
//      assert(starList.empty());                    // A list should be empty
//                                                   // after default
//                                                   // construction.
//
//      readData(&starList, input);                  // Read input to the list.
//      assert(10 == starList.size());               // Verify correct reading.
//      fclose(input);                               // Close input file.
//
//      filter(&starList);                           // Pick twinkle stars.
//      assert(4 == starList.size());                // Verify correct filter.
//
//      // Print out twinkle stars.
//      if (verbose) {
//          for (list<Star>::const_iterator i = starList.begin();
//               i != starList.end(); ++i) {
//              i->write(stdout);
//          }
//      }
//      return 0;
//  }
//..
//
///Example 2: Combine Two Star Surveys
///- - - - - - - - - - - - - - - - - -
// In the second example, we want to combine the results from two star surveys
// into a single list, using the same 'Star' class defined in the first usage
// example.
//
// First, we begin by reading both lists and filtering them.  (Our test data is
// selected so that the second data file contains 8 stars of which 3 are
// sufficiently bright as to pass our filter:
//..
//  int usageExample2(int verbose)
//  {
//      FILE *input = fopen("star_data1.txt", "r");  // Open first input file.
//      assert(input);
//
//      list<Star> starList1;                        // Define first star list.
//      assert(starList1.empty());
//
//      readData(&starList1, input);                 // Read input into list.
//      assert(10 == starList1.size());
//      fclose(input);                               // Close first input file.
//
//      input = fopen("star_data2.txt", "r");        // Open second input file.
//      assert(input);
//
//      list<Star> starList2;                        // Define second list.
//      assert(starList2.empty());
//
//      readData(&starList2, input);                 // Read input into list.
//      assert(8 == starList2.size());
//      fclose(input);                               // Close input file.
//
//      filter(&starList1);                          // Pick twinkle stars from
//                                                   // the first star list.
//      assert(4 == starList1.size());
//
//      filter(&starList2);                          // Pick twinkle stars from
//                                                   // the second star list.
//      assert(3 == starList2.size());
//..
// Then, we combine the two lists, 'starList1' and 'starList2'.  One way to do
// this is to simply insert the second list at the end of the first:
//..
//      list<Star> tmp1(starList1);  // Make a copy of the first list
//      list<Star> tmp2(starList2);  // Make a copy of the second list
//      tmp1.insert(tmp1.end(), tmp2.begin(), tmp2.end());
//      assert(7 == tmp1.size());    // Verify combined size.
//      assert(3 == tmp2.size());    // 'tmp2' should be unchanged.
//..
// Next, let's have a closer look of the above code and see if we can improve
// the combination performance.  The above 'insert' method appends a copy of
// each element in 'tmp2' onto the end of 'tmp1'.  This copy is unnecessary
// because we have no need for 'tmp2' after the lists have been combined.  A
// faster and less-memory-intensive technique is to use the 'splice' function,
// which *moves* rather than *copies* elements from one list to another:
//..
//      tmp1 = starList1;
//      tmp2 = starList2;
//      tmp1.splice(tmp1.begin(), tmp2);
//      assert(7 == tmp1.size());    // Verify combined size.
//      assert(0 == tmp2.size());    // 'tmp2' should be emptied by the splice.
//..
// Notice that, while the original lists were sorted in ascending order
// (because the data files were originally sorted), the combined list is no
// longer sorted.  To fix it, we sort 'tmp1' using the 'sort' member function:
//..
//      tmp1.sort();
//..
// Then, we suggest a third, and also the best approach to combine two lists,
// which is to take advantage of the fact that the lists were originally
// sorted, using the 'merge' function:
//..
//      starList1.merge(starList2);     // Merge 'starList2' into 'starList1'.
//      assert(7 == starList1.size());  // Verify combined size.
//      assert(0 == starList2.size());  // starList2 should be emptied by the
//                                      // merge.
//..
// Now, since the two star surveys may overlap, we want to eliminate
// duplicates.  We accomplish this by using the 'unique' member function:
//..
//      starList1.unique();             // Eliminate duplicates in 'starList1'.
//      assert(6 == starList1.size());  // Verify size after elimination.
//..
// Finally, we print the result:
//..
//      if (verbose) {
//          for (list<Star>::const_iterator i = starList1.begin();
//               i != starList1.end(); ++i) {
//              i->write(stdout);
//          }
//      }
//      return 0;
//  }
//..
// For completeness, the implementations of the 'read', 'write', and comparison
// functions for class 'Star' are shown below:
//..
//  bool Star::read(FILE *input)
//  {
//      int ret = fscanf(input, "%lf %lf %d", &d_x, &d_y, &d_brightness);
//      return 3 == ret;
//  }
//
//  void Star::write(FILE *output) const
//  {
//      fprintf(output, "%f %f %d\n", d_x, d_y, d_brightness);
//  }
//
//  bool operator==(const Star& lhs, const Star& rhs)
//  {
//      return lhs.x() == rhs.x()
//          && lhs.y() == rhs.y()
//          && lhs.brightness() == rhs.brightness();
//  }
//
//  bool operator!=(const Star& lhs, const Star& rhs)
//  {
//      return ! (lhs == rhs);
//  }
//
//  bool operator<(const Star& lhs, const Star& rhs)
//  {
//      if (lhs.x() < rhs.x())
//          return true;
//      else if (rhs.x() < lhs.x())
//          return false;
//      else if (lhs.y() < rhs.y())
//          return true;
//      else if (rhs.y() < lhs.y())
//          return true;
//      else
//          return lhs.brightness() < rhs.brightness();
//  }
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_list.h> instead of <bslstl_list.h> in \
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

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLALG_RANGECOMPARE
#include <bslalg_rangecompare.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace bsl {

                        // =====================
                        // struct bsl::List_Node
                        // =====================

template <class VALUE, class ALLOCATOR>
class list;

template <class VALUE>
struct List_Node
    // PRIVATE CLASS TEMPLATE.  For use only by 'bsl::list' implementation.
    // An instance of 'List_Node<T>' is a single node in a doubly-linked list
    // used to implement 'bsl::list<T,A>', for a given element type 'T' and
    // allocator type 'A'.  Note that an instantiation of this class for by a
    // given 'bsl::list' is independent of the allocator type.
{
  private:
    // PRIVATE CREATORS
    List_Node();                              // Declared but not defined
    List_Node(const List_Node& original);     // Declared but not defined
    ~List_Node();                             // Declared but not defined
        // A 'List_Node' should never be constructed or destructed.  Rather, a
        // 'List_Node' should be initialized using the 'init' function and
        // destroyed using the 'destroy' function.  The 'd_value' member of a
        // 'List_Node' object should be separately constructed and destroyed
        // using an allocator's 'construct' and 'destroy' methods.

    // PRIVATE MANIPULATORS
    List_Node& operator=(const List_Node&);   // Declared but not defined
        // A 'List_Node' should never be assigned.  Rather, a 'List_Node'
        // should be initialized using the 'init' function and destroyed using
        // the 'destroy' function.  The 'd_value' member of a 'List_Node'
        // object should be separately constructed and destroyed using an
        // allocator's 'construct' and 'destroy' methods.

  public:
    // PUBLIC TYPES

    // In C++11 NodePtr would generalized as follows:
    // typedef pointer_traits<VoidPtr>::template rebind<List_Node> NodePtr;

    typedef List_Node *NodePtr;

    // PUBLIC DATA
    NodePtr d_prev;   // pointer to the previous node in this list
    NodePtr d_next;   // pointer to the next node in this list
    VALUE   d_value;  // list element

    // MANIPULATORS
    void init();
        // Value-initialize the 'd_prev' and 'd_next' members of this node,
        // but do not touch 'd_value'.  The behavior is undefined unless 'this'
        // points to allocated (uninitialized) storage'.  Note: 'd_value'
        // continues to hold uninitialized storage after this call.

    void destroy();
        // Release the 'd_prev' and 'd_next' members of this node but do not
        // touch 'd_value'.  After calling 'destroy', 'this' points to
        // allocated (uninitialized) storage.  The behavior is undefined unless
        // 'd_prev' and 'd_next' have valid values and 'd_value' holds
        // allocated (uninitialized) storage.

};

                        // ========================
                        // class bsl::List_Iterator
                        // ========================

template <class VALUE, class NODEPTR, class DIFFTYPE>
class List_Iterator
#ifdef BSLS_PLATFORM_OS_SOLARIS
    : public std::iterator<std::bidirectional_iterator_tag, VALUE>
// On Solaris just to keep studio12-v4 happy, since algorithms takes only
// iterators inheriting from 'std::iterator'.
#endif
{
    // Implementation of std::list::iterator

    // FRIENDS
    template <class LIST_VALUE, class LIST_ALLOCATOR>
    friend class list;

    friend class List_Iterator<const VALUE, NODEPTR, DIFFTYPE>;

    template <class T1, class T2, class NODEP, class DIFFT>
    friend bool operator==(List_Iterator<T1, NODEP, DIFFT>,
                           List_Iterator<T2, NODEP, DIFFT>);

    // PRIVATE TYPES
    typedef typename remove_cv<VALUE>::type          NcType;
    typedef List_Iterator<NcType, NODEPTR, DIFFTYPE> NcIter;
    typedef List_Node<NcType>                        Node;

    // DATA
    NODEPTR d_nodeptr;  // pointer to list node

    // PRIVATE ACCESSORS
    NcIter unconst() const
        // Return an iterator providing modifiable access to the list element
        // that this list iterator refers to.
    {
        return NcIter(d_nodeptr);
    }

  public:
    // PUBLIC TYPES
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef NcType                          value_type;
    typedef DIFFTYPE                        difference_type;
    typedef VALUE                          *pointer;
    typedef VALUE&                          reference;

    // CREATORS
    List_Iterator();
        // Create a singular iterator (i.e., one that cannot be incremented,
        // decremented, or dereferenced.

    explicit List_Iterator(NODEPTR p);
        // Create an iterator that references the value pointed to by the
        // specified pointer 'p'.

    List_Iterator(const NcIter& other);
        // Create an iterator to 'VALUE' from the corresponding iterator to
        // non-const 'VALUE'.  If 'VALUE' is not const-qualified, then this
        // constructor becomes the copy constructor.  Otherwise, the copy
        // constructor is implicitly generated.

    // Compiler-generated copy constructor, destructor, and assignment
    // operators:
    // List_Iterator(const List_Iterator&); // Not defaulted
    //! ~List_Iterator() = default;
    //! List_Iterator& operator=(const List_Iterator&) = default;

    // MANIPULATORS
    List_Iterator& operator++();
        // Advance this iterator to the next element in this list and return
        // its new value.  The behavior is undefined unless this iterator is in
        // the range '[begin() .. end())' for some list (i.e., the iterator is
        // not singular, is not 'end()', and has not been invalidated).

    List_Iterator& operator--();
        // Move this iterator to the previous element in this list and return
        // its new value.  The behavior is undefined unless this iterator is in
        // the range '( begin(), end() ]' for some list (i.e., the iterator is
        // not singular, is not 'begin()', and has not been invalidated).

    List_Iterator operator++(int);
        // Advance this iterator to the next element in this list and return
        // its previous value.  The behavior is undefined unless this iterator
        // is in the range '[begin() .. end())' for some list (i.e., the
        // iterator is not singular, is not 'end()', and has not been
        // invalidated).

    List_Iterator operator--(int);
        // Move this iterator to the previous element in this list and return
        // its previous value.  The behavior is undefined unless this iterator
        // is in the range '( begin(), end() ]' for some list (i.e., the
        // iterator is not singular, is not 'begin()', and has not been
        // invalidated).

    // ACCESSORS
    reference operator*() const;
        // Return a reference to this list object referenced by this iterator.
        // The behavior is undefined unless this iterator is in the range
        // '[begin() .. end())' for some list (i.e., the iterator is not
        // singular, is not 'end()', and has not been invalidated).

    pointer operator->() const;
        // Return a pointer to this list object referenced by this iterator.
        // The behavior is undefined unless this iterator is in the range
        // '[begin() .. end())' for some list (i.e., the iterator is not
        // singular, is not 'end()', and has not been invalidated).
};

// FREE OPERATORS
template <class T1, class T2, class NODEPTR, class DIFFTYPE>
bool operator==(List_Iterator<T1, NODEPTR, DIFFTYPE> lhs,
                List_Iterator<T2, NODEPTR, DIFFTYPE> rhs);
    // Return 'true' if the specified iterators 'lhs' and 'rhs' have the same
    // value and 'false' otherwise.  Two iterators have the same value if both
    // refer to the same element of the same list or both are the end()
    // iterator of the same list.  The return value is undefined unless both
    // 'lhs' and 'rhs' are non-singular.

template <class T1, class T2, class NODEPTR, class DIFFTYPE>
bool operator!=(List_Iterator<T1, NODEPTR, DIFFTYPE> lhs,
                List_Iterator<T2, NODEPTR, DIFFTYPE> rhs);
    // Return 'true' if the specified iterators 'lhs' and 'rhs' do not have the
    // same value and 'false' otherwise.  Two iterators have the same value if
    // both refer to the same element of the same list or both are the end()
    // iterator of the same list.  The return value is undefined unless both
    // 'lhs' and 'rhs' are non-singular.


                        // ===============
                        // class bsl::list
                        // ===============

template <class VALUE, class ALLOCATOR = bsl::allocator<VALUE> >
class list
    // This class template implements a value-semantic container type holding a
    // sequence of elements of the (template parameter) 'VALUE' type.  For
    // convenience and consistency in describing the interfaces of this class,
    // the following terms are defined and used throughout the function level
    // documentation:
    //
    //  "default-insertion": Construction of a new element 'e' into this list.
    //      If the (template parameter) 'ALLOCATOR' is convertible from
    //      'bslma_Allocator*', and 'VALUE' conforms to the bslma allocator
    //      protocol, then 'get_allocator().mechanism()' is passed as the sole
    //      constructor argument to 'e'; otherwise, no arguments are passed to
    //      the constructor of 'e'.
    //
    //  "copy-insertion": Construction of a new element 'e' into this list
    //      using the constructor argument 'v'.  If 'ALLOCATOR' type is
    //      convertible from 'bslma_Allocator*', and 'VALUE' conforms to the
    //      bslma allocator protocol, pass 'get_allocator().mechanism()' as an
    //      additional (second) constructor argument to 'e'; otherwise,
    //      construct 'e' from 'v' with no allocator argument.
    //
    //  "emplace-construction" (from *'args'*): Construction of a new element
    //      'e' into this list using zero or more constructor arguments,
    //      *'args'*.  If 'ALLOCATOR' is convertible from 'bslma_Allocator*',
    //      and 'VALUE' conforms to the bslma allocator protocol, pass
    //      'get_allocator().mechanism()' as an additional (final) constructor
    //      argument to 'e'; otherwise, construct 'e' from 'args' with no
    //      allocator argument.
{
    // PRIVATE TYPES
    typedef List_Node<VALUE>                      Node;
        // This typedef is an alias for the node type in this list.

    typedef typename allocator_traits<ALLOCATOR>::template rebind_traits<Node>
        AllocTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    typedef typename AllocTraits::allocator_type  NodeAlloc;
    typedef typename AllocTraits::pointer         NodePtr;
    typedef typename AllocTraits::difference_type DiffType;

    struct AllocAndSizeWrapper;
    friend struct AllocAndSizeWrapper;

    struct AllocAndSizeWrapper : public NodeAlloc {
        // This struct is wrapper around the allocator and size data member.
        // It takes advantage of the empty-base optimization (EBO) so that if
        // the allocator is stateless, it takes up no space.
        //
        // TBD: This struct should eventually be replaced by the use of a
        // general EBO-enabled component that provides a 'pair'-like
        // interface.  (A properly-optimized 'tuple' would do the job.)

        // PUBLIC TYPES
        typedef typename AllocTraits::size_type size_type;

        // DATA
        size_type d_size;  // allocated size

        // CREATORS
        explicit AllocAndSizeWrapper(const NodeAlloc& basicAllocator,
                                     size_type size)
        : NodeAlloc(basicAllocator), d_size(size)
            // Create an allocator wrapper having the specified allocator type
            // 'allocator' and the specified allocated 'size'.
        {
        }
    };

    class NodeProctor;
    friend class NodeProctor;

    class NodeProctor
        // This class provides a proctor to free a node containing an
        // uninitialized 'VALUE' object in the event that an exception is
        // thrown.
    {
        // DATA
        list     *d_list;  // list to proctor
        NodePtr   d_p;     // node to free upon destruction

      public:
        // CREATORS
        explicit NodeProctor(list *l, NodePtr p)
            // Create a node proctor object that will use the specified list
            // 'l' to free the specified node 'p'.
        : d_list(l), d_p(p)
        {
        }

        ~NodeProctor()
            // Destroy this node proctor, and free the node it contains unless
            // the 'release' method has been called before.
        {
            if (d_p) {
                d_list->free_node(d_p);
            }
        }

        // MANIPULATORS
        void release()
            // Detach the node contained in this proctor from the proctor.
            // After calling this 'release' method, the proctor no longer frees
            // any node upon its destruction.
        {
            d_p = 0;
        }
    };

    struct Comp_Elems {
        // Binary function predicate object type for comparing two 'VALUE'
        // objects using 'operator<'.  This operation is usually, but not
        // always, the same as that provided by 'std::less<VALUE>'.  The
        // standard requires that certain functions use 'operator<', which
        // means that divergent specializations of 'std::less' are ignored.

        // ACCESSORS
        bool operator()(const VALUE& lhs, const VALUE& rhs) const
            // Return 'true' if the value of the specified 'lhs' is less than
            // that of the specified 'rhs', and 'false' otherwise.
        {
            return lhs < rhs;
        }
    };

    // DATA
    NodePtr             d_sentinel;        // node pointer of sentinel element
    AllocAndSizeWrapper d_alloc_and_size;  // node allocator

    // PRIVATE MANIPULATORS
    NodeAlloc& allocator();
        // Return a reference providing modifiable access to the allocator used
        // to allocate nodes.

    typename AllocTraits::size_type& size_ref();
        // Return a reference providing modifiable access to the data element
        // holding the size of this list.

    NodePtr allocate_node();
        // Return a node allocated from the container's allocated.  Before
        // returning, the 'init' function is called to initialize the node's
        // pointers, but the node's constructor is not called.

    void free_node(NodePtr np);
        // Call 'np->destroy()' and deallocate the node pointed to by 'np'.
        // Note that the node's destructor is not called.

    void link_nodes(NodePtr prev, NodePtr next);
        // Modify the forward pointer of 'prev' to point to 'next' and the
        // backward pointer of 'next' to point to 'prev'.

    void create_sentinel();
        // Create the 'd_sentinel' node of this list.  The 'd_sentinel' node
        // does not hold a value.  When first created it's forward and backward
        // pointers point to itself, creating a circular linked list.  This
        // function also sets this list's size to zero.

    void destroy_all();
        // Erase all elements, destroy and deallocate the 'd_sentinel' node,
        // and this list in an invalid but destructible state (i.e., with size
        // == -1).

    void quick_swap(list& other);
        // Quickly swaps 'd_sentinel' and 'size_ref()' of '*this' with 'other'
        // without checking the allocator.

    // PRIVATE ACCESSORS
    const NodeAlloc& allocator() const;
        // Return a reference providing non-modifiable access to the allocator
        // used to allocate nodes.

    NodePtr head() const;
        // Return a pointer to the first node in this list or the sentinel node
        // if this list is empty.

    const typename AllocTraits::size_type& size_ref() const;
        // Return a reference providing non-modifiable access to the data
        // element holding the size of this list.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        list,
        BloombergLP::bslmf::IsBitwiseMoveable,
        BloombergLP::bslmf::IsBitwiseMoveable<ALLOCATOR>::value);
    BSLMF_NESTED_TRAIT_DECLARATION(list,
                                   BloombergLP::bslalg::HasStlIterators);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        list,
        BloombergLP::bslma::UsesBslmaAllocator,
        (is_convertible<BloombergLP::bslma::Allocator*, ALLOCATOR>::value));

    // PUBLIC TYPES
    typedef VALUE&                                             reference;
    typedef const VALUE&                                       const_reference;
    typedef List_Iterator<VALUE,NodePtr,DiffType>              iterator;
    typedef List_Iterator<const VALUE,NodePtr,DiffType>        const_iterator;
    typedef typename allocator_traits<ALLOCATOR>::pointer      pointer;
    typedef typename allocator_traits<ALLOCATOR>::const_pointer
                                                               const_pointer;

    typedef typename AllocTraits::size_type       size_type;
    typedef typename AllocTraits::difference_type difference_type;
    typedef VALUE                                 value_type;
    typedef ALLOCATOR                             allocator_type;
    typedef bsl::reverse_iterator<iterator>       reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

  private:
    // These private manipulators cannot be declared until 'const_iterator'
    // and 'size_type' have been declared.

    // PRIVATE MANIPULATORS
    iterator insert_node(const_iterator position, NodePtr node);
        // Insert the specified 'node' prior to the specified 'position' in
        // this list.

    template <class COMPARE>
    NodePtr merge_imp(NodePtr node1,
                      NodePtr node2,
                      NodePtr finish,
                      COMPARE comp);
        // Given a contiguous sequence of nodes, '[node1 .. finish)', with
        // 'node2' pointing somewhere in the middle of the sequence, merge
        // sequence '[node2 .. finish)' into '[node1 .. node2)', and return a
        // pointer to the beginning of the merged sequence.  If an exception is
        // thrown, all nodes remain in this list, but their order is
        // unspecified.  The behavior is undefined unless '[node1 .. node2)'
        // and '[node2 .. finish)' each describe a contiguous sequence of
        // nodes.

    template <class COMPARE>
    NodePtr sort_imp(NodePtr       *pnode1,
                     size_type      size,
                     const COMPARE& comp);
        // Sort the sequence of 'size' nodes starting with '*pnode1'.  Modifies
        // '*pnode1' to refer to the first node of the sorted sequence.  If an
        // exception is thrown, all nodes remain properly linked, but their
        // order is unspecified.  The behavior is undefined unless '*pnode1'
        // begins a sequence of at least 'size' nodes, none of which are
        // sentinel nodes.

  public:
    // CREATORS

    // 23.3.5.2 construct/copy/destroy:

    explicit list(const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create an empty list that allocates memory using the specified
        // 'basicAllocator'.

    explicit list(size_type n);
        // Create a list containing the specified 'n' elements and using a
        // default-constructed allocator.  The initial elements in this list
        // are constructed by "default-insertion".

    list(size_type n,
         const VALUE& value,
         const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create a list using the specified 'basicAllocator' and insert the
        // specified 'n' number of elements created by "copy-insertion" from
        // 'value'.

    template <class InputIter>
    list(InputIter first,
         InputIter last,
         const ALLOCATOR& basicAllocator = ALLOCATOR(),
         typename enable_if<
             !is_fundamental<InputIter>::value && !is_enum<InputIter>::value
         >::type * = 0)
        // Create a list using the specified 'basicAllocator' and insert the
        // number of elements determined by the size of the specified range
        // '[first .. last)'.  Each initial element is created by
        // "copy-insertion" from the corresponding element in
        // '[first .. last)'.  The behavior is undefined unless 'first' and
        // 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last'.  Note that this method does not
        // participate in overload resolution unless 'InputIter' is an iterator
        // type.
        //
        // TBD: It would be better to use 'std::is_arithmetic' (a currently
        // unavailable metafunction) instead of 'is_fundamental' in the
        // 'enable_if' expression.
    : d_alloc_and_size(basicAllocator, size_type(-1))
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // enable_if be in-place inline.

        // '*this' is in an invalid but destructible state (size == -1).
        // Create a temporary list, 'tmp' with the specified data.  If an
        // exception is thrown, 'tmp's destructor will clean up.  Otherwise,
        // swap 'tmp' with '*this', leaving 'tmp' in an invalid but
        // destructible state and leaving '*this' fully constructed.

        list tmp(this->allocator());
        tmp.insert(tmp.begin(), first, last);
        quick_swap(tmp);
    }

    list(const list& original);
        // Create a list having the same value as that of the specified
        // 'original'.  If 'ALLOCATOR' is convertible from 'bslma_Allocator*',
        // then the resulting list will use the default allocator; otherwise,
        // the resulting list will use a copy of 'original.get_allocator()'.
        // Each element in the resulting list is constructed by
        // "copy-insertion" from the corresponding element in 'original'.

    list(const list& original, const ALLOCATOR& basicAllocator);
        // Create a list having the same value as that of the specified
        // 'original' that will use the specified 'basicAllocator' to supply
        // memory.  Each element in the resulting list is constructed by
        // "copy-insertion" from the corresponding element in 'allocator'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    list(list&& original);
        // Create a new list using the contents and allocator from the
        // specified list 'original'.  No copy or move constructors are called
        // for individual elements.  After the construction, the value of
        // 'original' is valid, but unspecified.  Note that this constructor
        // may allocate memory and may, therefore, throw an allocation-related
        // exception.

    list(list&& original, const ALLOCATOR& basicAllocator);
        // Create a new list using the contents from the specified list
        // 'original' and using a copy of the specified 'basicAllocator'.  If
        // 'basicAllocator == original.get_allocator()', then no copy or move
        // constructors are called for individual elements.  Otherwise, each
        // element in the resulting list is constructed by "copy-insertion"
        // from the corresponding element in 'original'.  After the
        // construction, the value of 'original' is valid, but unspecified.

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    ~list();
        // Destroy this list by calling the destructor for each element and
        // deallocating all allocated storage.

    // MANIPULATORS
    list& operator=(const list& rhs);
        // Assign to this list the value of the specified list 'rhs', and
        // return a reference providing modifiable access to this list.  Each
        // element of this list is either copy-assigned or copy-inserted from
        // the corresponding element of 'rhs'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    list& operator=(list&& rhs);
        // Assign to this list the value of the of the specified list 'rhs',
        // and return a reference providing modifiable access to this list.  If
        // 'rhs.get_allocator() == this->get_allocator()', then no move or copy
        // operations are applied to any individual elements; otherwise each
        // element of this list is created by either copy assignment or
        // "copy insertion" from the corresponding element of 'rhs'.  After the
        // construction, the value of 'rhs' is valid, but unspecified.

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    template <class InputIter>
    void assign(InputIter first, InputIter last,
                typename enable_if<
                    !is_fundamental<InputIter>::value &&
                    !is_enum<InputIter>::value
                >::type * = 0)
        // Assign to this list the values of the elements in the specified
        // range '[first .. last)'.  Each element in this list is set by either
        // copy-assignment or "copy-insertion" from the corresponding element
        // in '[first .. last)'.  The behavior is undefined unless 'first' and
        // 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last', and the sequence is not in this list.
        // Note that this method does not participate in overload resolution
        // unless 'InputIter' is an iterator type.
        //
        // TBD: It would be better to use 'std::is_arithmetic' (a currently
        // unavailable metafunction) instead of 'is_fundamental' in the
        // 'enable_if' expression.
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // enable_if be in-place inline.

        iterator i = this->begin();
        iterator e = this->end();

        for (; first != last && i != e; ++first, ++i) {
            *i = *first;
        }

        erase(i, e);

        for (; first != last; ++first) {
            emplace(e, *first);
        }
    }

    void assign(size_type n, const VALUE& value);
        // Replace the contents of this list with the specified 'n' copies of
        // the specified 'value'.  Each element in this list is set by either
        // copy assignment or "copy insertion" from 'value'.

    // iterators:

    iterator begin();
        // Return a mutating iterator referring to the first element in the
        // list, if any, or one past the end of this list if this list is
        // empty.

    iterator end();
        // Return a mutating iterator referring to one past the end of this
        // list.

    reverse_iterator rbegin();
        // Return a mutating reverse iterator referring to the last element of
        // this list (i.e., the first element of the reverse sequence), if any,
        // or one before the start if this list is empty.

    reverse_iterator rend();
        // Return a mutating reverse iterator referring to one before the start
        // of this list.

    // 23.3.5.3 capacity:

    void resize(size_type sz);
        // Resize this list to the specified 'sz' elements.  If 'sz' is less
        // than or equal to the previous size of this list, then erase the
        // excess elements from the end.  Otherwise, append additional elements
        // to the end using "default-insertion" until there are a total of 'sz'
        // elements.

    void resize(size_type sz, const VALUE& c);
        // Resize this list to the specified 'sz' elements, with added elements
        // being copies of the specified value 'c'.  If 'sz' is less than or
        // equal to the previous size of this list, then erase the excess
        // elements from the end.  Otherwise, append additional elements to the
        // end using "copy-insertion" from 'c' until there are a total of 'sz'
        // elements.

    // element access:

    reference front();
        // Return a reference providing modifiable access to the first element
        // of this list.  The behavior is undefined unless this list contains
        // at least one element.

    reference back();
        // Return a reference providing modifiable access to the last element
        // of this list.  The behavior is undefined unless this list contains
        // at least one element.

    // 23.3.5.4 modifiers:
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    void emplace_front(ARGS&&... args);
        // Insert a new element at the front of this list and construct it
        // using "emplace-construction" from the specified 'args'.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_list.h
    void emplace_front();

    template <class ARGS_01>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01);

    template <class ARGS_01,
              class ARGS_02>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09,
              class ARGS_10>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10) args_10);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... ARGS>
    void emplace_front(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
// }}} END GENERATED CODE
#endif

    void pop_front();
        // Remove and destroy the first element of this list.  The behavior is
        // undefined unless this list contains at least one element.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    void emplace_back(ARGS&&... args);
        // Insert a new element at the back of this list and construct it using
        // "emplace-construction" from the specified 'args'.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_list.h
    void emplace_back();

    template <class ARGS_01>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01);

    template <class ARGS_01,
              class ARGS_02>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09,
              class ARGS_10>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10) args_10);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... ARGS>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
// }}} END GENERATED CODE
#endif

    void pop_back();
        // Remove and destroy the last element of this list.  The behavior is
        // undefined unless this list contains at least one element.

    void push_front(const VALUE& value);
        // Insert a new element at the front of this list using
        // "copy-insertion" from the specified value 'value'.

    void push_back(const VALUE& value);
        // Append a new element to the end of this list using "copy-insertion"
        // from the specified value 'value'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void push_front(VALUE&& value);
    void push_back(VALUE&& value);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    iterator emplace(const_iterator position, ARGS&&... args);
        // Insert a new element into this list before the element at the
        // specified 'position' using "emplace-construction" from the specified
        // 'args'.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_list.h
    iterator emplace(const_iterator position);

    template <class ARGS_01>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01);

    template <class ARGS_01,
              class ARGS_02>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09);

    template <class ARGS_01,
              class ARGS_02,
              class ARGS_03,
              class ARGS_04,
              class ARGS_05,
              class ARGS_06,
              class ARGS_07,
              class ARGS_08,
              class ARGS_09,
              class ARGS_10>
    iterator emplace(const_iterator position,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09,
                           BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10) args_10);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... ARGS>
    iterator emplace(const_iterator position,
                              BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args);
// }}} END GENERATED CODE
#endif

    iterator insert(const_iterator position, const VALUE& value);
        // Insert into this list a copy of the specified 'value' before the
        // element at the specified 'position' of this list.  The newly
        // inserted elements in this list is copy-constructed from 'value'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    iterator insert(const_iterator position, VALUE&& value);
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    iterator insert(const_iterator position, size_type n, const VALUE& value);
        // Insert into this list the specified 'n' copies of the specified
        // 'value' before the element at the specified 'position' of this list.
        // Each of the 'n' newly inserted elements in this list is
        // copy-constructed from 'value'.

    template <class InputIter>
    iterator insert(const_iterator position, InputIter first, InputIter last,
                    typename enable_if<
                        !is_fundamental<InputIter>::value &&
                        !is_enum<InputIter>::value
                    >::type * = 0)
        // Insert the specified range '[first .. last)' into this list at the
        // specified 'position', and return an iterator to the first inserted
        // element or 'position' if the range is empty.  The behavior is
        // undefined unless 'first' and 'last' refer to a sequence of valid
        // values where 'first' is at a position at or before 'last'.  Note
        // that this method does not participate in overload resolution unless
        // 'InputIter' is an iterator type.
        //
        // TBD: It would be better to use 'std::is_arithmetic' (a currently
        // unavailable metafunction) instead of 'is_fundamental' in the
        // 'enable_if' expression.
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // enable_if be inplace inline.

        if (first == last) {
            return position.unconst();                                // RETURN
        }

        // Remember the position of the first insertion

        iterator ret = insert(position, *first);
        for (++first; first != last; ++first) {
            insert(position, *first);
        }

        return ret;
    }

    iterator erase(const_iterator position);
        // Remove from this list the element at the specified 'position', and
        // return an iterator referring to the element immediately following
        // the removed element, or to the position returned by the 'end' method
        // if the removed element was the last in the sequence.  The behavior
        // is undefined unless 'position' is an iterator in the range
        // '[ begin() .. end())'.

    iterator erase(const_iterator position, const_iterator last);
        // Remove from this list the elements starting at the specified 'first'
        // position that are before the specified 'last' position, and return
        // an iterator referring to the element immediately following the last
        // removed element, or the position returned by the method 'end' if the
        // removed elements were last in the sequence.  The behavior is
        // undefined unless 'first' is an iterator in the range
        // '[begin() .. end()]' and 'last' is an iterator in the range
        // '[first .. end()]' (both endpoints included).

    void swap(list& other);
        // Exchange the value of this list with that of the specified 'other'
        // list, such that each list has, upon return, the value of the other
        // list prior to this call.  This method does not throw or invalidate
        // iterators if 'get_allocator', invoked on this list and 'other',
        // returns the same value.

    void clear();
        // Remove all the elements from this list.

    // 23.3.5.5 list operations:

    void splice(const_iterator position, list& x);
        // Insert elements of the specified list 'x' before the element at the
        // specified 'position' of this list, and remove those elements from
        // 'x'.  The behavior is undefined unless 'x' is not this list.

    void splice(const_iterator position, list& x, const_iterator i);
        // Insert the element at the specified 'i' position before the element
        // at the specified 'position' of this list, and remove this element
        // from the specified list 'x'.  The behavior is undefined unless 'i'
        // refers to a valid element in 'x'.

    void splice(const_iterator position,
                list& x,
                const_iterator first,
                const_iterator last);
        // Insert the elements starting at the specified 'first' position and
        // before the specified 'last' position into this list, right before
        // the element at the specified 'position', and remove those elements
        // from the specified list 'x'.  The behavior is undefined unless
        // '[first .. last)' represents a range of valid elements in 'x', and
        // 'position' is not in the range '[first .. last)'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void splice(const_iterator position, list&& x);
    void splice(const_iterator position, list&& x, const_iterator i);
    void splice(const_iterator position,
                list&& x,
                const_iterator first,
                const_iterator last);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    void remove(const VALUE& value);
        // Erase all the elements having the specified 'value' from this list.

    template <class Predicate>
    void remove_if(Predicate pred);
        // Erase from this list all the elements that are not predicted 'false'
        // by the specified predicate 'pred'.

    void unique();
        // Erase from this list all but the first element of every consecutive
        // group of elements that have the same value.

    template <class EqPredicate>
    void unique(EqPredicate binary_pred);
        // Erase from this list all but the first element of every consecutive
        // group of elements that have the specified 'binary_pred' predicates
        // 'true' for any two consecutive elements in the group.

    void merge(list& other);
        // Merge the specified sorted list 'other' into this sorted list.  The
        // method has no effect if 'other' is this list.  The behavior is
        // undefined unless both 'other' and this list are sorted in
        // non-decreasing order according to the ordering returned by
        // 'operator<'.

    template <class COMPARE>
    void merge(list& other, COMPARE comp);
        // Merge the specified sorted list 'other' into this sorted list, using
        // the specified 'comp', which defines a strict weak ordering, to order
        // elements.  The method has no effect if 'other' is this list.  The
        // behavior is undefined unless both 'other' and this list are sorted
        // in non-decreasing order according to the ordering returned by
        // 'comp'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void merge(list&& other);

    template <class COMPARE>
    void merge(list&& other, COMPARE comp);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    void sort();
        // Sort this list in non-decreasing order according to the orders
        // returned by 'operator<'.

    template <class COMPARE>
    void sort(COMPARE comp);
        // Sort this list in non-decreasing order according to the orders
        // returned by the specified 'comp' comparator.

    void reverse();
        // Reverse the order of the elements in this list.

    // ACCESSORS

    // 23.3.5.2 construct/copy/destroy:

    allocator_type get_allocator() const;
        // Return a copy of the allocator used for memory allocation by this
        // list.

    // iterators:

    const_iterator begin() const;
        // Return a non-mutating iterator referring to the first element in the
        // list, if any, or one past the end of this list if this list is
        // empty.

    const_iterator end() const;
        // Return a non-mutating iterator referring to one past the end of this
        // list.

    const_reverse_iterator rbegin() const;
        // Return a const reverse iterator referring to the last element of
        // this list (i.e., the first element of the reverse sequence), if any,
        // or one before the start if this list is empty.

    const_reverse_iterator rend() const;
        // Return a const reverse iterator referring to one before the start of
        // this list.

    const_iterator cbegin() const;
        // Return a non-mutating iterator referring to the first element in the
        // list, if any, or one past the end of this list if this list is
        // empty.

    const_iterator cend() const;
        // Return a non-mutating iterator referring to one past the end of the
        // list.

    const_reverse_iterator crbegin() const;
        // Return a const reverse iterator referring to the last element of
        // this list (i.e., the first element of the reverse sequence), if any,
        // or one before the start if this list is empty.

    const_reverse_iterator crend() const;
        // Return a const reverse iterator referring to one before the start of
        // this list.

    // 23.3.5.3 capacity:

    bool empty() const;
        // Return 'true' if this list has no elements and 'false' otherwise.

    size_type size() const;
        // Return the number of elements in this list.

    size_type max_size() const;
        // Return an upper bound on the largest number of elements that this
        // list could possibly hold.  Note that return value of this function
        // does not guarantee that this list can successfully grow that large,
        // or even close to that large without running out of resources.

    // element access:

    const_reference front() const;
        // Return a reference providing non-modifiable access to the first
        // element of this list.  The behavior is undefined unless this list
        // contains at least one element.

    const_reference back() const;
        // Return a reference providing non-modifiable access to the last
        // element of this list.  The behavior is undefined unless this list
        // contains at least one element.
};

// FREE OPERATORS
template <class VALUE, class ALLOCATOR>
bool operator==(const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' lists have the same
    // value, and 'false' otherwise.  The 'lhs' and the 'rhs' objects have the
    // same value if they have the same number of elements, and each element of
    // 'lhs' has same value as that of the corresponding element in 'rhs'.
    // Note that this method requires that the (template parameter) 'VALUE'
    // type has 'operator==' defined.

template <class VALUE, class ALLOCATOR>
bool operator!=(const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' lists do not have the
    // same value, and 'false' otherwise.  The 'lhs' and the 'rhs' objects do
    // have the same value if they do not have the same number of elements, or
    // at least one element of 'lhs' does not have same value as that of the
    // corresponding element in 'rhs'.  Note that this method requires that the
    // (template parameter) 'VALUE' type has 'operator==' defined.

template <class VALUE, class ALLOCATOR>
bool operator< (const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' list is lexicographically smaller
    // than the specified 'rhs' list, and 'false' otherwise.  The 'lhs' is
    // lexicographically smaller than the 'rhs' if there exists an element 'v'
    // in 'lhs' such that 'v' is smaller than the corresponding element in
    // 'rhs', and all elements before 'v' in 'lhs' have the same values as
    // those of the corresponding elements in 'rhs'.  Note that this method
    // requires that the (template parameter) 'VALUE' type has 'operator<'
    // defined.

template <class VALUE, class ALLOCATOR>
bool operator> (const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' list is lexicographically larger
    // than the specified 'rhs' list, and 'false' otherwise.  Note that this
    // method requires that the (template parameter) 'VALUE' type has
    // 'operator<' defined.

template <class VALUE, class ALLOCATOR>
bool operator<=(const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' list is lexicographically smaller
    // than or equal to the specified 'rhs' list, and 'false' otherwise.  Note
    // that this method requires that the (template parameter) 'VALUE' type has
    // 'operator<' defined.

template <class VALUE, class ALLOCATOR>
bool operator>=(const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' list is lexicographically larger
    // than or equal to the specified 'rhs' list, and 'false' otherwise.  Note
    // that this method requires that the (template parameter) 'VALUE' type has
    // 'operator<' defined.

// SPECIALIZED ALGORITHMS
template <class VALUE, class ALLOCATOR>
void swap(list<VALUE, ALLOCATOR>& lhs, list<VALUE, ALLOCATOR>& rhs);
    // Exchange the value of the specified 'lhs' list with that of the
    // specified 'rhs' list, such that each list has upon return the value of
    // the other list prior to this call.  Note that this function does not
    // throw if 'lhs.get_allocator()' and 'rhs.get_allocator()' are equal.

}  // close namespace bsl

// ============================================================================
//                   INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------
                        // struct bsl::List_Node
                        // ---------------------

template <class VALUE>
inline
void bsl::List_Node<VALUE>::init()
{
    // If 'List_Node' is ever enhanced to allow for generalized pointers (e.g.,
    // a 'NodePtr' that is not a raw pointer), then this function will be
    // responsible for constructing 'd_prev' and 'd_next', rather than just
    // setting them to null:
    //
    //    new ((void*) BloombergLP::bsls::Util::addressOf(d_prev))
    //                                                        NodePtr(nullptr);
    //    new ((void*) BloombergLP::bsls::Util::addressOf(d_next))
    //                                                        NodePtr(nullptr);

    d_prev = d_next = 0;
}

template <class VALUE>
inline
void bsl::List_Node<VALUE>::destroy()
{
    // If 'List_Node' is ever enhanced to allow for generalized pointers (e.g.,
    // a 'NodePtr' that is not a raw pointer), then this function will be
    // responsible for calling the destructors for 'd_prev' and 'd_next':
    //
    //    d_prev.~NodePtr();
    //    d_next.~NodePtr();
}

                        // ------------------------
                        // class bsl::List_Iterator
                        // ------------------------

template <class VALUE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::List_Iterator() /* = default; */
{
}

template <class VALUE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::List_Iterator(NODEPTR p)
: d_nodeptr(p)
{
}

template <class VALUE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::List_Iterator(
                                                           const NcIter& other)
: d_nodeptr(other.d_nodeptr)
{
}

template <class VALUE, class NODEPTR, class DIFFTYPE>
inline
typename bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::reference
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::operator*() const
{
    return this->d_nodeptr->d_value;
}

template <class VALUE, class NODEPTR, class DIFFTYPE>
inline
typename bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::pointer
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::operator->() const
{
    return BloombergLP::bsls::Util::addressOf(this->d_nodeptr->d_value);
}

template <class VALUE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>&
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::operator++()
{
    this->d_nodeptr = this->d_nodeptr->d_next;
    return *this;
}

template <class VALUE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>&
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::operator--()
{
    this->d_nodeptr = this->d_nodeptr->d_prev;
    return *this;
}

template <class VALUE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::operator++(int)
{
    List_Iterator temp = *this;
    this->operator++();
    return temp;
}

template <class VALUE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>
bsl::List_Iterator<VALUE, NODEPTR, DIFFTYPE>::operator--(int)
{
    List_Iterator temp = *this;
    this->operator--();
    return temp;
}

template <class T1, class T2, class NODEPTR, class DIFFTYPE>
inline
bool bsl::operator==(List_Iterator<T1,NODEPTR,DIFFTYPE> lhs,
                     List_Iterator<T2,NODEPTR,DIFFTYPE> rhs)
{
    // 'lhs' and 'rhs' have the same 'NODEPTR' type; their 'd_nodeptr' members
    // can thus be compared for equality regardless of 'T1' and 'T2'.  However,
    // all instantiations of 'List_Iterator' with the same 'NODEPTR' will have
    // types 'T1' or 'T2' that differ only in their 'const' qualification.

    return lhs.d_nodeptr == rhs.d_nodeptr;
}

template <class T1, class T2, class NODEPTR, class DIFFTYPE>
inline
bool bsl::operator!=(List_Iterator<T1,NODEPTR,DIFFTYPE> lhs,
                     List_Iterator<T2,NODEPTR,DIFFTYPE> rhs)
{
    return ! (lhs == rhs);
}

                             // ---------------
                             // class bsl::list
                             // ---------------

namespace bsl {

// PRIVATE MANIPULATORS
template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::NodeAlloc& list<VALUE, ALLOCATOR>::allocator()
{
    return d_alloc_and_size;  // Implicit cast to base class
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::AllocTraits::size_type&
list<VALUE, ALLOCATOR>::size_ref()
{
    return d_alloc_and_size.d_size;
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::NodePtr
list<VALUE, ALLOCATOR>::allocate_node()
{
    NodePtr ret = AllocTraits::allocate(allocator(), 1);
    ret->init();
    return ret;
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::free_node(NodePtr np)
{
    np->destroy();
    AllocTraits::deallocate(allocator(), np, 1);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::link_nodes(NodePtr prev, NodePtr next)
{
    prev->d_next = next;
    next->d_prev = prev;
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::create_sentinel()
{
    d_sentinel = allocate_node();
    link_nodes(d_sentinel, d_sentinel);  // circular
    size_ref() = 0;
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::destroy_all()
{
    clear();
    free_node(d_sentinel);
    size_ref() = size_type(-1);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::quick_swap(list& other)
{
    NodePtr tmpSentinel = d_sentinel;
    d_sentinel = other.d_sentinel;
    other.d_sentinel = tmpSentinel;

    size_type tmpSize = size_ref();
    size_ref() = other.size_ref();
    other.size_ref() = tmpSize;
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert_node(const_iterator position, NodePtr node)
{
    typename AllocTraits::pointer next = position.d_nodeptr;
    typename AllocTraits::pointer prev = next->d_prev;
    link_nodes(prev, node);
    link_nodes(node, next);
    ++size_ref();
    return iterator(node);
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
typename list<VALUE, ALLOCATOR>::NodePtr
list<VALUE, ALLOCATOR>::merge_imp(NodePtr node1,
                                 NodePtr node2,
                                 NodePtr finish,
                                 COMPARE comp)
{
    NodePtr pre = node1->d_prev;

    // The only possible throwing operation is the comparison functor.
    // Exception neutrality is created by ensuring that this list is in a valid
    // state, with no disconnected nodes, before the comparison functor is
    // called.

    // Having the two sublists be contiguous parts of the same list has the
    // following advantages:
    // 1. When we reach the end of a sublist, there is no "finalization"
    //    step where the end of the remaining sublist must be spliced onto the
    //    merged list.
    // 2. No cleanup needed if an exception is thrown; the size and validity
    //    of the resulting list needs no adjustment.

    while (node1 != node2 && node2 != finish) {
        // Loop invariants:
        // - The open range (pre, node1) is the current merged result
        // - The half-open range [node1, node2) is the 1st unmerged sequence
        // - The half-open range [node2, finish) is the 2nd unmerged sequence

        if (comp(node2->d_value, node1->d_value)) {
            // node2 should come before node1.
            // Find the end of the sequence of elements that belong before
            // node1 so that we can splice them all at once.

            NodePtr lastMove = node2;
            NodePtr next2    = node2->d_next;
            while (next2 != finish && comp(next2->d_value, node1->d_value)) {
                lastMove = next2;
                next2 = lastMove->d_next;
            }

            link_nodes(node2->d_prev, next2);
            link_nodes(node1->d_prev, node2);
            link_nodes(lastMove, node1);

            // Advance to next node in the 2nd unmerged sequence.

            node2 = next2;
        }
        else {
            // Advance to next node in the 1st unmerged sequence.

            node1 = node1->d_next;
        }
    }

    return pre->d_next;
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
typename list<VALUE, ALLOCATOR>::NodePtr
list<VALUE, ALLOCATOR>::sort_imp(NodePtr       *pnode1,
                                 size_type      size,
                                 const COMPARE& comp)
{
    BSLS_ASSERT(size > 0);

    NodePtr node1 = *pnode1;
    if (size < 2) {
        return node1->d_next;                                         // RETURN
    }

    size_type half = size / 2;

    NodePtr node2 = sort_imp(&node1, half,        comp);
    NodePtr next  = sort_imp(&node2, size - half, comp);

    *pnode1 = merge_imp(node1, node2, next, comp);
    return next;
}

// PRIVATE ACCESSORS
template <class VALUE, class ALLOCATOR>
inline
const typename list<VALUE, ALLOCATOR>::NodeAlloc&
list<VALUE, ALLOCATOR>::allocator() const
{
    return d_alloc_and_size;  // Implicit cast to base class
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::NodePtr list<VALUE, ALLOCATOR>::head() const
{
    return d_sentinel->d_next;
}

template <class VALUE, class ALLOCATOR>
inline
const typename list<VALUE, ALLOCATOR>::AllocTraits::size_type&
list<VALUE, ALLOCATOR>::size_ref() const
{
    return d_alloc_and_size.d_size;
}

// CREATORS

// 23.3.5.2 construct/copy/destroy:
template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, 0)
{
    create_sentinel();
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(size_type n)
: d_sentinel()
, d_alloc_and_size(ALLOCATOR(), size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(allocator());

    // Default-construct (value-initialize) 'n' elements into 'tmp'.  'tmp's
    // destructor will clean up if an exception is thrown.

    iterator pos = tmp.end();
    for (size_type i = 0; i < n; ++i) {
        tmp.emplace(pos);
    }

    quick_swap(tmp);  // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(size_type n,
                             const VALUE& value,
                             const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(this->allocator());
    tmp.insert(tmp.begin(), n, value);  // 'tmp's destructor will clean up on
                                        // throw.
    quick_swap(tmp);      // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(const list& original)
: d_sentinel()
, d_alloc_and_size(
      AllocTraits::select_on_container_copy_construction(original.allocator()),
      size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(allocator());

    // 'tmp's destructor will clean up on throw.

    tmp.insert(tmp.begin(), original.begin(), original.end());

    quick_swap(tmp);  // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(const list&      original,
                             const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(this->allocator());

    // 'tmp's destructor will clean up on throw.

    tmp.insert(tmp.begin(), original.begin(), original.end());

    quick_swap(tmp);  // Leave 'tmp' in an invalid but destructible state.
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(list&& original)
: d_sentinel()
, d_alloc_and_size(original.allocator(), 0)
{
    // Allocator should be copied, not moved, to ensure identical allocators
    // between this and 'original', otherwise 'swap' is undefined.

    create_sentinel();
    quick_swap(original);
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(list&& original, const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    if (basicAllocator == original.allocator()) {
        create_sentinel();
        size_ref() = 0;  // '*this' is now in a valid state.
        quick_swap(original);
    }
    else {
        list tmp(this->allocator());

        // 'tmp's destructor will clean up on throw.

        tmp.insert(tmp.begin(), original.begin(), original.end());

        quick_swap(tmp);  // Leave 'tmp' in an invalid but destructible state.
    }
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::~list()
{
    // A size of -1 is a special incompletely-initialized or
    // destructively-moved-from state.

    if (size_ref() != size_type(-1)) {
        destroy_all();
    }
}

// MANIPULATORS
template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>& list<VALUE, ALLOCATOR>::operator=(const list& rhs)
{
    if (this == &rhs) {
        return *this;                                                 // RETURN
    }

    if (AllocTraits::propagate_on_container_copy_assignment::value &&
        allocator() != rhs.allocator()) {
        // Completely destroy and rebuild list using new allocator.

        // Create a new list with the new allocator.  This operation might
        // throw, so we do it before destroying the old list.

        list temp(rhs.get_allocator());

        // Clear existing list and leave in an invalid but destructible state.

        destroy_all();

        // Assign allocator (required not to throw).

        allocator() = rhs.allocator();

        // Now swap lists, leaving 'temp' in an invalid but destructible state.

        quick_swap(temp);
    }

    assign(rhs.begin(), rhs.end());
    return *this;
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>& list<VALUE, ALLOCATOR>::operator=(list&& original)
{
    if (this == &original) {
        return *this;                                                 // RETURN
    }

    if (allocator() == original.allocator()) {
        // Equal allocators, just swap contents:

        quick_swap(original);
    }
    else if (AllocTraits::propagate_on_container_move_assignment::value) {
        // Completely destroy and rebuild list using new allocator.

        // Create a new list with the new allocator and new contents.  This
        // operation might throw, so we do it before destroying the old list.

        list temp(BSLS_COMPILERFEATURES_FORWARD(list,original));

        // Clear existing list and leave in an invalid but destructible state.

        destroy_all();

        // Assign allocator (required not to throw).

        allocator() = temp.allocator();

        // Now swap lists, leaving 'temp' in an invalid but destructible state.

        quick_swap(temp);
    }
    else
    {
        // Unequal allocators and no moving of allocators, do linear copy

        assign(original.begin(), original.end());
    }

    return *this;
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::assign(size_type n, const VALUE& value)
{
    iterator i = this->begin();
    iterator e = this->end();

    for (; n > 0 && i != e; --n, ++i) {
        *i = value;
    }

    erase(i, e);

    for (; n > 0; --n) {
        insert(e, value);
    }
}

// iterators:
template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::iterator list<VALUE, ALLOCATOR>::begin()
{
    return iterator(head());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::iterator list<VALUE, ALLOCATOR>::end()
{
    return iterator(d_sentinel);
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::reverse_iterator
list<VALUE, ALLOCATOR>::rbegin()
{
    return reverse_iterator(end());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::reverse_iterator
list<VALUE, ALLOCATOR>::rend()
{
    return reverse_iterator(begin());
}

// 23.3.5.3 capacity
template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::resize(size_type sz)
{
    if (sz > size()) {
        emplace_back();
        VALUE& c = back();
        while (sz > size()) {
            push_back(c);
        }
    }
    else {
        while (sz < size()) {
            pop_back();
        }
    }
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::resize(size_type sz, const VALUE& c)
{
    while (sz > size()) {
        push_back(c);
    }

    while (sz < size()) {
        pop_back();
    }
}

// element access:
template <class VALUE, class ALLOCATOR>
inline
VALUE& list<VALUE, ALLOCATOR>::front()
{
    BSLS_ASSERT_SAFE(size_ref() > 0);

    return head()->d_value;
}

template <class VALUE, class ALLOCATOR>
inline
VALUE& list<VALUE, ALLOCATOR>::back()
{
    BSLS_ASSERT_SAFE(size_ref() > 0);

    NodePtr last = d_sentinel->d_prev;
    return last->d_value;
}

// 23.3.5.4 modifiers:
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class ALLOCATOR>
template <class... ARGS>
inline
void list<VALUE, ALLOCATOR>::emplace_front(ARGS&&... args)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_list.h
template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                               )
{
    emplace(begin());
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_08,args_08));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_08,args_08),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_09,args_09));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09,
          class ARGS_10>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10) args_10)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_08,args_08),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_09,args_09),
                     BSLS_COMPILERFEATURES_FORWARD(ARGS_10,args_10));
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class VALUE, class ALLOCATOR>
template <class... ARGS>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(begin(), BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
}
// }}} END GENERATED CODE
#endif

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::pop_front()
{
    BSLS_ASSERT_SAFE(size_ref() > 0);

    erase(begin());
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class ALLOCATOR>
template <class... ARGS>
inline
void list<VALUE, ALLOCATOR>::emplace_back(ARGS&&... args)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_list.h
template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                               )
{
    emplace(end());
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_08,args_08));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_08,args_08),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_09,args_09));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09,
          class ARGS_10>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10) args_10)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_08,args_08),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_09,args_09),
                   BSLS_COMPILERFEATURES_FORWARD(ARGS_10,args_10));
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class VALUE, class ALLOCATOR>
template <class... ARGS>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    emplace(end(), BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
}
// }}} END GENERATED CODE
#endif

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::pop_back()
{
    BSLS_ASSERT_SAFE(size_ref() > 0);

    erase(--end());
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::push_front(const VALUE& value)
{
    emplace(begin(), value);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::push_back(const VALUE& value)
{
    emplace(end(), value);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::push_front(VALUE&& value)
{
    using namespace BloombergLP;
    emplace(begin(), bslmf::MovableRefUtil::move(value));
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::push_back(VALUE&& value)
{
    using namespace BloombergLP;
    emplace(end(), bslmf::MovableRefUtil::move(value));
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class ALLOCATOR>
template <class... ARGS>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position, ARGS&&... args)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
    proctor.release();
    return insert_node(position, p);
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_list.h
template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_08,args_08));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_08,args_08),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_09,args_09));
    proctor.release();
    return insert_node(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09,
          class ARGS_10>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_03) args_03,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_04) args_04,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_05) args_05,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_06) args_06,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_07) args_07,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_08) args_08,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_09) args_09,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_10) args_10)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_08,args_08),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_09,args_09),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_10,args_10));
    proctor.release();
    return insert_node(position, p);
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class VALUE, class ALLOCATOR>
template <class... ARGS>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                               BSLS_COMPILERFEATURES_FORWARD_REF(ARGS)... args)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
    proctor.release();
    return insert_node(position, p);
}
// }}} END GENERATED CODE
#endif

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(const_iterator position, const VALUE& value)
{
    return emplace(position, value);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(const_iterator position, VALUE&& value)
{
    using namespace BloombergLP;
    return emplace(position, bslmf::MovableRefUtil::move(value));
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(const_iterator position,
                              size_type n,
                              const VALUE& value)
{
    if (0 == n) {
        return position.unconst();                                    // RETURN
    }

    // Remember the position of the first insertion

    iterator ret = emplace(position, value);
    for (--n; n > 0; --n) {
        emplace(position, value);
    }

    return ret;
}

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT(position.d_nodeptr != d_sentinel);

    typename AllocTraits::pointer p = position.d_nodeptr;
    iterator ret(p->d_next);

    link_nodes(p->d_prev, p->d_next);
    AllocTraits::destroy(allocator(),
                         BloombergLP::bsls::Util::addressOf(p->d_value));
    free_node(p);
    --size_ref();
    return ret;
}

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::erase(const_iterator position, const_iterator last)
{
    while (position != last) {
        const_iterator curr = position;
        ++position;
        erase(curr);
    }

    return position.unconst();
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::swap(list& other)
{
    using std::swap;

    if (AllocTraits::propagate_on_container_swap::value) {
        swap(allocator(), other.allocator());
        quick_swap(other);
    }
    else {
        // C++0x behavior: undefined for unequal allocators
        // BSLS_ASSERT(allocator() == other.allocator());

        // backward compatible behavior: swap with copies

        if (allocator() == other.allocator()) {
            quick_swap(other);
        }
        else {
            list thisCopy(*this, other.get_allocator());
            list xCopy(other, get_allocator());

            thisCopy.quick_swap(other);
            xCopy.quick_swap(*this);
        }
    }
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::clear()
{
    erase(begin(), end());
}

// 23.3.5.5 list operations:
template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::splice(const_iterator position, list& x)
{
    BSLS_ASSERT(allocator() == x.allocator());

    if (x.empty()) {
        return;                                                       // RETURN
    }

    typename AllocTraits::pointer pos = position.d_nodeptr;
    typename AllocTraits::pointer first = x.head();
    typename AllocTraits::pointer last  = x.d_sentinel->d_prev;
    size_type n = x.size_ref();

    // Splice contents out of x.

    link_nodes(x.d_sentinel, x.d_sentinel);
    x.size_ref() = 0;

    // Splice contents into *this.

    link_nodes(pos->d_prev, first);
    link_nodes(last, pos);
    size_ref() += n;
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::splice(const_iterator position,
                                    list& x,
                                    const_iterator i)
{
    BSLS_ASSERT(allocator() == x.allocator());

    typename AllocTraits::pointer pos   = position.d_nodeptr;
    typename AllocTraits::pointer xnode = i.d_nodeptr;
    typename AllocTraits::pointer xnext = xnode->d_next;

    if (pos == xnode || pos == xnext) {
        return;                                                       // RETURN
    }

    // Splice contents out of x.

    link_nodes(xnode->d_prev, xnext);
    --x.size_ref();

    // Splice contents into *this.

    link_nodes(pos->d_prev, xnode);
    link_nodes(xnode, pos);
    ++size_ref();
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::splice(const_iterator position,
                                   list& x,
                                   const_iterator first,
                                   const_iterator last)
{
    BSLS_ASSERT(allocator() == x.allocator());

    size_type n = bsl::distance(first, last);

    if (0 == n) {
        return;                                                       // RETURN
    }

    typename AllocTraits::pointer posp   = position.d_nodeptr;
    typename AllocTraits::pointer firstp = first.d_nodeptr;
    typename AllocTraits::pointer nextp  = last.d_nodeptr;
    typename AllocTraits::pointer lastp  = last.d_nodeptr->d_prev;

    // Splice contents out of x.

    link_nodes(firstp->d_prev, nextp);
    x.size_ref() -= n;

    // Splice contents into *this.

    link_nodes(posp->d_prev, firstp);
    link_nodes(lastp, posp);
    size_ref() += n;
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::remove(const VALUE& value)
{
    const_iterator e = cend();
    for (const_iterator i = cbegin(); i != e; ) {
        // Standard says to use operator==, not std::equal_to.

        if (value == *i) {
            i = erase(i);
        }
        else {
            ++i;
        }
    }
}

template <class VALUE, class ALLOCATOR>
template <class Predicate>
void list<VALUE, ALLOCATOR>::remove_if(Predicate pred)
{
    iterator e = end();
    for (iterator i = begin(); i != e; ) {
        if (pred(*i)) {
            i = erase(i);
        }
        else {
            ++i;
        }
    }
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::unique()
{
    if (size() < 2) {
        return;                                                       // RETURN
    }

    iterator i = begin();
    iterator e = end();
    while (i != e) {
        reference match = *i++;
        while (i != e && *i == match) {
            i = erase(i);
        }
    }
}

template <class VALUE, class ALLOCATOR>
template <class EqPredicate>
void list<VALUE, ALLOCATOR>::unique(EqPredicate binary_pred)
{
    if (size() < 2) {
        return;                                                       // RETURN
    }

    iterator i = begin();
    iterator e = end();
    while (i != e) {
        reference match = *i++;
        while (i != e && binary_pred(*i, match)) {
            i = erase(i);
        }
    }
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
void list<VALUE, ALLOCATOR>::merge(list& other, COMPARE comp)
{
    if (other.empty()) {
        // This is a important special case to avoid pointing to sentinel.

        return;                                                       // RETURN
    }

    // Splice 'other' to the end of '*this', but remember the first node of the
    // appended sequence.

    NodePtr xfirst = other.d_sentinel->d_next;
    splice(end(), other);

    // Call merge_imp with a pointer to the first node of the original list, a
    // pointer to the first node of 'other' (which also ends the original list)
    // and a pointer to the sentinel (which now ends 'other').

    merge_imp(d_sentinel->d_next, xfirst, d_sentinel, comp);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::merge(list& other)
{
    merge(other, Comp_Elems());
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
void list<VALUE, ALLOCATOR>::sort(COMPARE comp)
{
    if (size_ref() < 2) {
        return;                                                       // RETURN
    }
    NodePtr node1 = d_sentinel->d_next;
    sort_imp(&node1, size(), comp);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::sort()
{
    sort(Comp_Elems());
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::reverse()
{
    NodePtr sentinel = d_sentinel;
    NodePtr p = sentinel;

    do {
        NodePtr tmp = p->d_next;
        p->d_next = p->d_prev;
        p->d_prev = tmp;
        p = tmp;
    } while (p != sentinel);
}

// ACCESSORS

// 23.3.5.2 construct/copy/destroy:
template <class VALUE, class ALLOCATOR>
inline
ALLOCATOR list<VALUE, ALLOCATOR>::get_allocator() const
{
    return allocator();
}

// iterators:
template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::begin() const
{
    return const_iterator(head());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::end() const
{
    return const_iterator(d_sentinel);
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::cbegin() const
{
    return begin();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::cend() const
{
    return end();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::crbegin() const
{
    return rbegin();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::crend() const
{
    return rend();
}

// 23.3.5.3 capacity:
template <class VALUE, class ALLOCATOR>
inline
bool list<VALUE, ALLOCATOR>::empty() const
{
    return 0 == size_ref();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::size_type list<VALUE, ALLOCATOR>::size() const
{
    return size_ref();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::size_type
list<VALUE, ALLOCATOR>::max_size() const
{
    return AllocTraits::max_size(allocator());
}

// element access:
template <class VALUE, class ALLOCATOR>
inline
const VALUE& list<VALUE, ALLOCATOR>::front() const
{
    BSLS_ASSERT_SAFE(size_ref() > 0);

    return head()->d_value;
}

template <class VALUE, class ALLOCATOR>
inline
const VALUE& list<VALUE, ALLOCATOR>::back() const
{
    BSLS_ASSERT_SAFE(size_ref() > 0);

    NodePtr last = d_sentinel->d_prev;
    return last->d_value;
}

}  // close namespace bsl

// FREE OPERATORS
template <class VALUE, class ALLOCATOR>
inline
bool bsl::operator==(const list<VALUE, ALLOCATOR>& lhs,
                     const list<VALUE, ALLOCATOR>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

template <class VALUE, class ALLOCATOR>
inline
bool bsl::operator!=(const list<VALUE, ALLOCATOR>& lhs,
                     const list<VALUE, ALLOCATOR>& rhs)
{
    return ! (lhs == rhs);
}

template <class VALUE, class ALLOCATOR>
inline
bool bsl::operator< (const list<VALUE, ALLOCATOR>& lhs,
                     const list<VALUE, ALLOCATOR>& rhs)
{
    return 0 > BloombergLP::bslalg::RangeCompare::lexicographical(lhs.begin(),
                                                                  lhs.end(),
                                                                  lhs.size(),
                                                                  rhs.begin(),
                                                                  rhs.end(),
                                                                  rhs.size());
}

template <class VALUE, class ALLOCATOR>
inline
bool bsl::operator> (const list<VALUE, ALLOCATOR>& lhs,
                     const list<VALUE, ALLOCATOR>& rhs)
{
    return rhs < lhs;
}

template <class VALUE, class ALLOCATOR>
inline
bool bsl::operator>=(const list<VALUE, ALLOCATOR>& lhs,
                     const list<VALUE, ALLOCATOR>& rhs)
{
    return ! (lhs < rhs);
}

template <class VALUE, class ALLOCATOR>
inline
bool bsl::operator<=(const list<VALUE, ALLOCATOR>& lhs,
                     const list<VALUE, ALLOCATOR>& rhs)
{
    return ! (rhs < lhs);
}

// specialized algorithms:
template <class VALUE, class ALLOCATOR>
inline
void bsl::swap(list<VALUE, ALLOCATOR>& lhs, list<VALUE, ALLOCATOR>& rhs)
{
    lhs.swap(rhs);
}


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
