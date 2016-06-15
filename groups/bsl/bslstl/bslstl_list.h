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
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component defines a single class template, 'bsl::list',
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
// a 'bslma::Allocator' object is explicitly supplied at construction, the list
// uses it to supply memory for the list throughout its lifetime; otherwise,
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
//  'A'             - parameterized 'ALLOCATOR' type of the list
//  'a', 'b'        - distinct objects of type 'list<V>'
//  'ra','rb'       - distinct modifiable rvalue objects of type 'list<V>&&'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'al'            - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'v'             - an object of type 'V'
//  'rv'            - modifiable rvalue object of type 'V&&'
//  'p1', 'p2'      - two iterators belonging to 'a'
//  's1', 's2'      - two iterators belonging to 'b'
//  'pred'          - a unary predicate
//  'binary_pred'   - a binary predicate
//  'comp'          - a binary predicate implementing a strict-weak ordering
//  'args...'       - a variadic list of (up to 10) arguments
//  'init-list'     - C++11-style initiler list of length 'n'
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
//  | list<V> a(rb); (move construction)                 | O(1)               |
//  | list<V> a(rb, al);                                 | O(1) if 'a' and 'b'|
//  |                                                    | use the same       |
//  |                                                    | allocator,         |
//  |                                                    | O(m)     otherwise |
//  +----------------------------------------------------+--------------------+
//  | list<V> a(n);                                      | O(n)               |
//  | list<V> a(n, v);                                   |                    |
//  | list<V> a(n, v, al);                               |                    |
//  +----------------------------------------------------+--------------------+
//  | list<V> a(i1, i2);                                 | O(distance(i1,i2)) |
//  | list<V> a(i1, i2, al);                             |                    |
//  +----------------------------------------------------+--------------------+
//  | list<V> a({init-list}, al = A())                   | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.~list<V>(); (destruction)                        | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a = b;                (copy assignment)            | O(max(n, m))       |
//  +----------------------------------------------------+--------------------+
//  | a = {init-list};      (copy assignment)            | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a = rb;               (move assignment)            | O(1) if 'a' and 'b'|
//  |                                                    | use the same       |
//  |                                                    | allocator,         |
//  |                                                    | O(max(n, m))       |
//  |                                                    | otherwise          |
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
//  |                                                    | O(n + m) otherwise |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.get_allocator()                                  | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.emplace(p1, args...)                             | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, v)                                    | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, n, v)                                 | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, i1, i2)                               | O(distance(i1, i2))|
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, rv)                                   | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | O(distance(p1, p2))|
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.assign(i1,i2)                                    | O(distance(i1, i2))|
//  +----------------------------------------------------+--------------------+
//  | a.assign(n, v)                                     | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.assign({init-list})                              | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.front(), a.back()                                | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.emplace_front(args...), a.emplace_back(args...)  | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.push_front(v),                                   |                    |
//  | a.push_back(v)                                     | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.push_front(rv),                                  |                    |
//  | a.push_back(rv)                                    | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.pop_front(), a.pop_back()                        | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.resize(n), a.resize(n, v)                        | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, b),  a.splice(p, b, s1)                | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, rb), a.splice(p, rb, s1)               | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, b, s1, s2)                             | O(distance(s1, s2))|
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, rb, s1, s2)                            | O(distance(s1, s2))|
//  +----------------------------------------------------+--------------------+
//  | a.remove(t), a.remove_if(pred)                     | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.unique(), a.unique(binary_pred)                  | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.merge(b),  a.merge(b, comp)                      | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.merge(rb), a.merge(rb, comp)                     | O(n)               |
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

#ifndef INCLUDED_BSLMA_ALLOCATORTRAITS
#include <bslma_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLMA_STDALLOCATOR
#include <bslma_stdallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_ISARITHMETIC
#include <bslmf_isarithmetic.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#ifndef INCLUDED_INITIALIZER_LIST
#include <initializer_list>
#define INCLUDED_INITIALIZER_LIST
#endif
#endif

namespace bsl {

                        // =====================
                        // struct bsl::List_Node
                        // =====================

template <class VALUE>
struct List_Node
    // PRIVATE STRUCT TEMPLATE.  For use only by 'bsl::list' implementation.
    // An instance of 'List_Node<T>' is a single node in a doubly-linked list
    // used to implement 'bsl::list<T,A>', for a given element type 'T' and
    // allocator type 'A'.  Note that an instantiation of this 'struct' for by
    // a given 'bsl::list' is independent of the allocator type.
{
    // In C++11 NodePtr would generalized as follows:
    // 'typedef pointer_traits<VoidPtr>::template rebind<List_Node> NodePtr;'

    // DATA
    List_Node *d_prev_p;   // pointer to the previous node in this list
    List_Node *d_next_p;   // pointer to the next node in this list
    VALUE      d_value;    // list element

  private:
    // NOT IMPLEMENTED
    List_Node();                              // = delete;
    List_Node(const List_Node&);              // = delete;
    ~List_Node();                             // = delete;
    List_Node& operator=(const List_Node&);   // = delete;

    // A 'List_Node' is never to be constructed, copied, destroyed, or assigned
    // to.  The 'd_value' field is constructed directly by 'list::emplace', and
    // destroyed directly by 'list::erase'.
};

                        // ========================
                        // class bsl::List_Iterator
                        // ========================

template <class VALUE, class ALLOCATOR>
class list;

#ifdef BSLS_PLATFORM_OS_SOLARIS
// On Solaris just to keep studio12-v4 happy, since algorithms takes only
// iterators inheriting from 'std::iterator'.

template <class VALUE>
class List_Iterator :
                 public std::iterator<std::bidirectional_iterator_tag, VALUE> {
#else
template <class VALUE>
class List_Iterator {
#endif
    // Implementation of std::list::iterator

    // PRIVATE TYPES
    typedef typename remove_cv<VALUE>::type  NcType;
    typedef List_Iterator<NcType>            NcIter;
    typedef List_Node<NcType>                Node;

    // DATA
    Node *d_node_p;      // pointer to list node

    // FRIENDS
    template <class LIST_VALUE, class LIST_ALLOCATOR>
    friend class list;

    template <class ITER_VALUE>
    friend class List_Iterator;

    template <class T1, class T2>
    friend bool operator==(List_Iterator<T1>, List_Iterator<T2>);

    // PRIVATE ACCESSORS
    NcIter unconst() const
        // Return an iterator providing modifiable access to the list element
        // that this list iterator refers to.
    {
        return NcIter(d_node_p);
    }

  public:
    // PUBLIC TYPES
    typedef std::bidirectional_iterator_tag   iterator_category;
    typedef NcType                            value_type;
    typedef BloombergLP::bsls::Types::IntPtr  difference_type;
    typedef VALUE                            *pointer;
    typedef VALUE&                            reference;

    // CREATORS
    List_Iterator();
        // Create a singular iterator (i.e., one that cannot be incremented,
        // decremented, or dereferenced.

    explicit List_Iterator(Node *node_p);
        // Create an iterator that references the value pointed to by the
        // specified pointer 'p'.

    List_Iterator(const NcIter& other);                             // IMPLICIT
        // Create an iterator to 'VALUE' from the corresponding iterator to
        // non-const 'VALUE'.  If 'VALUE' is not const-qualified, then this
        // constructor becomes the copy constructor.  Otherwise, the copy
        // constructor is implicitly generated.  Note that while copy c'tors
        // are always implicit, this one must be marked as such to silence code
        // standards conformance checkers due to 'NcIter' being a (possibly)
        // different type.

    // Compiler-generated copy constructor, destructor, and assignment
    // operators:
    //: o List_Iterator(const List_Iterator&); // Not defaulted
    //: o ~List_Iterator() = default;
    //: o List_Iterator& operator=(const List_Iterator&) = default;

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
template <class T1, class T2>
bool operator==(List_Iterator<T1> lhs, List_Iterator<T2> rhs);
    // Return 'true' if the specified iterators 'lhs' and 'rhs' have the same
    // value and 'false' otherwise.  Two iterators have the same value if both
    // refer to the same element of the same list or both are the end()
    // iterator of the same list.  The return value is undefined unless both
    // 'lhs' and 'rhs' are non-singular.  Note that the different types 'T1'
    // and 'T2' are to facilitate comparisons between 'const' and non-'const'
    // iterators and there will be a compilation error if 'T1' and 'T2' differ
    // in any way other than 'const'-ness.

template <class T1, class T2>
bool operator!=(List_Iterator<T1> lhs, List_Iterator<T2> rhs);
    // Return 'true' if the specified iterators 'lhs' and 'rhs' do not have the
    // same value and 'false' otherwise.  Two iterators have the same value if
    // both refer to the same element of the same list or both are the end()
    // iterator of the same list.  The return value is undefined unless both
    // 'lhs' and 'rhs' are non-singular.  Note that the different types 'T1'
    // and 'T2' are to facilitate comparisons between 'const' and non-'const'
    // iterators and there will be a compilation error if 'T1' and 'T2' differ
    // in any way other than 'const'-ness.

                            // ===============
                            // class bsl::list
                            // ===============

template <class VALUE, class ALLOCATOR = bsl::allocator<VALUE> >
class list {
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

    // PRIVATE TYPES

    // Forward declarations of private nested classes and struct.

    class  AllocAndSizeWrapper;
    struct CompLessThanImp;
    class  NodeProctor;

    // Other private types forwarded from outside this class.

    typedef List_Node<VALUE>                      Node;
        // This typedef is an alias for the node type in this list.

    typedef typename allocator_traits<ALLOCATOR>::template rebind_traits<Node>
                                                  AllocTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    typedef BloombergLP::bslmf::MovableRefUtil    MoveUtil;
        // This typedef is a convenient alias for the utility associated with
        // movable references.

    typedef typename AllocTraits::allocator_type  NodeAlloc;
    typedef typename AllocTraits::pointer         NodePtr;

  public:
    // PUBLIC TYPES
    typedef VALUE&                                             reference;
    typedef const VALUE&                                       const_reference;
    typedef List_Iterator<VALUE>                               iterator;
    typedef List_Iterator<const VALUE>                         const_iterator;
    typedef typename allocator_traits<ALLOCATOR>::pointer      pointer;
    typedef typename allocator_traits<ALLOCATOR>::const_pointer
                                                               const_pointer;

    typedef typename AllocTraits::size_type       size_type;
    typedef typename AllocTraits::difference_type difference_type;
    typedef VALUE                                 value_type;
    typedef ALLOCATOR                             allocator_type;
    typedef bsl::reverse_iterator<iterator>       reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

    // PUBLIC TRAITS
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

  private:
    // DATA
    NodePtr             d_sentinel;        // node pointer of sentinel element
    AllocAndSizeWrapper d_alloc_and_size;  // node allocator

    // PRIVATE MANIPULATORS
    NodeAlloc& allocatorImp();
        // Return a reference providing modifiable access to the allocator used
        // to allocate nodes.

    NodePtr allocateNode();
        // Return a node allocated from the container's allocated.  Before
        // returning, the 'init' function is called to initialize the node's
        // pointers, but the node's constructor is not called.

    void createSentinel();
        // Create the 'd_sentinel' node of this list.  The 'd_sentinel' node
        // does not hold a value.  When first created it's forward and backward
        // pointers point to itself, creating a circular linked list.  This
        // function also sets this list's size to zero.

    void deleteNode(NodePtr node);
        // Destroy 'node->d_value' and free the node's memory.  Do not do
        // any pointer fixup of the node or its neighbors, and do not update
        // 'sizeRef'.

    void destroyAll();
        // Erase all elements, destroy and deallocate the 'd_sentinel' node,
        // leaving this list in an invalid but destructible state (i.e., with
        // 'size == -1').

    void freeNode(NodePtr node);
        // Zero out the pointers and deallocate the node pointed to by the
        // specified 'node'.  Note that the node's destructor is not called,
        // and importantly, the 'd_value' field of the node is not destroyed.
        // The behavior is undefined unless 'node' was allocated using the
        // allocator of this list.

    iterator insertNode(const_iterator position, NodePtr node);
        // Insert the specified 'node' prior to the specified 'position' in
        // this list.  The behavior is undefined unless 'node' was allocated
        // using the allocator of this list.

    void linkNodes(NodePtr prev, NodePtr next);
        // Modify the forward pointer of the specified 'prev' to point to the
        // specified 'next' and the backward pointer of 'next' to point to
        // 'prev'.

    template <class COMPARE>
    NodePtr mergeImp(NodePtr node1,
                     NodePtr node2,
                     NodePtr finish,
                     COMPARE comp);
        // Given a specified pair of nodes 'node1' and 'finish' specifying the
        // range '[node1 .. finish)', with the specified 'node2' pointing
        // somewhere in the middle of the sequence, merge sequence
        // '[node2 .. finish)' into '[node1 .. node2)', and return a pointer to
        // the beginning of the merged sequence, using the specified
        // 'less-than' comparator 'comp' to determine order.  If an exception
        // is thrown, all nodes remain in this list, but their order is
        // unspecified.  If any nodes in the range '[node1, node2)' compare
        // equivalent to any nodes in the range '[node2, finish)' the nodes
        // from '[node1, node2)' will be merged first.  The behavior is
        // undefined unless '[node1 .. node2)' and '[node2 .. finish)' each
        // describe a contiguous sequence of nodes.

    void quickSwap(list *other);
        // Quickly swaps 'd_sentinel' and 'sizeRef()' of '*this' with the
        // specified '*other' without swapping the allocator.  Guaranteed not
        // to throw.

    typename AllocTraits::size_type& sizeRef() BSLS_CPP11_NOEXCEPT;
        // Return a reference providing modifiable access to the data element
        // holding the size of this list.

    template <class COMPARE>
    NodePtr sortImp(NodePtr        *node1_p,
                    size_type       size,
                    const COMPARE&  comp);
        // Sort the sequence of the specified 'size' nodes starting with the
        // specified '*node1_p'.  Modifies '*node1_p' to refer to the first
        // node of the sorted sequence.  Use the specified ordering comparator
        // 'comp' to compare 'VALUE' type objects.  If an exception is thrown,
        // all nodes remain properly linked, but their order is unspecified.
        // The behavior is undefined unless '*node1_p' begins a sequence of at
        // least 'size' nodes, none of which are sentinel nodes.

    // PRIVATE ACCESSORS
    const NodeAlloc& allocatorImp() const;
        // Return a reference providing non-modifiable access to the allocator
        // used to allocate nodes.

    NodePtr headNode() const;
        // Return a pointer to the first node in this list or the sentinel node
        // if this list is empty.

    const typename AllocTraits::size_type& sizeRef() const BSLS_CPP11_NOEXCEPT;
        // Return a reference providing non-modifiable access to the data
        // element holding the size of this list.

  public:
    // CREATORS
    list();
        // Create an empty list, using 'ALLOCATOR()' to allocate memory.

    explicit list(const ALLOCATOR& basicAllocator);
        // Create an empty list.  Use the specified 'basicAllocator' to
        // allocate memory.

    explicit list(size_type n);
        // Create a list containing the specified 'n' elements and using a
        // default-constructed allocator.  The initial elements in this list
        // are constructed by "default-insertion".

    list(size_type        n,
         const ALLOCATOR& basicAllocator);
        // Create a list and insert the specified 'n' number of
        // default-constructed elements.  Use the specified 'basicAllocator' to
        // allocate memory.

    list(size_type         n,
         const value_type& value,
         const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create a list and insert the specified 'n' number of elements
        // created by "copy-insertion" from the specified'value'.  Use an
        // optionally specified 'basicAllocator' to allocate memory.

    template <class INPUT_ITERATOR>
    list(INPUT_ITERATOR   srcBegin,
         INPUT_ITERATOR   srcEnd,
         const ALLOCATOR& basicAllocator = ALLOCATOR(),
         typename enable_if<
                 !is_arithmetic<INPUT_ITERATOR>::value &&
                 !is_enum<INPUT_ITERATOR>::value
         >::type * = 0)
        // Create a list, and insert each 'value_type' object in the sequence
        // starting at the specified 'srcBegin' element, and ending immediately
        // before the specified 'srcEnd' element.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default), then 'basicAllocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used.  This
        // operation has 'O[N]' complexity, where 'N' is the number of elements
        // between 'srcBegin' and 'srcEnd'.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type', and 'value_type' must be
        // 'emplace-constructible' from '*i' into this map, where 'i' is a
        // dereferenceable iterator in the range '[srcBegin .. srcEnd)' (see
        // {Requirements on 'VALUE'}).  The behavior is undefined unless
        // 'srcBegin' and 'srcEnd' refer to a sequence of valid values where
        // 'srcBegin' is at a position at or before 'srcEnd'.  Note that this
        // method does not participate in overload resolution unless
        // 'INPUT_ITERATOR' is an iterator type.
    : d_alloc_and_size(basicAllocator, size_type(-1))
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // enable_if be in-place inline.

        // '*this' is in an invalid but destructible state (size == -1).
        // Create a temporary list, 'tmp' with the specified data.  If an
        // exception is thrown, 'tmp's destructor will clean up.  Otherwise,
        // swap 'tmp' with '*this', leaving 'tmp' in an invalid but
        // destructible state and leaving '*this' fully constructed.

        list tmp(this->allocatorImp());
        tmp.insert(tmp.cbegin(), srcBegin, srcEnd);
        quickSwap(&tmp);
    }

    list(const list& original);
        // Create a list having the same value as the specified 'original'
        // object.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  This method requires that the (template parameter)
        // type 'VALUE' be 'copy-insertable' into this list (see {Requirements
        // on 'VALUE'}).

    list(const list& original, const ALLOCATOR& basicAllocator);
        // Create a list having the same value as that of the specified
        // 'original' object that will use the specified 'basicAllocator' to
        // supply memory.  Each element in the resulting list is constructed by
        // "copy-insertion" from the corresponding element in 'allocator'.

    list(BloombergLP::bslmf::MovableRef<list> original);            // IMPLICIT
        // Create a new list using the contents and allocator from the
        // specified 'original' object.  No copy or move constructors are
        // called for individual elements.  After the construction, the value
        // of 'original' is valid, but unspecified.  Note that this constructor
        // may allocate memory and may, therefore, throw an allocation-related
        // exception.

    list(BloombergLP::bslmf::MovableRef<list> original,
         const ALLOCATOR&                     basicAllocator);
        // Create a list having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // This method requires that the (template parameter) type 'VALUE' be
        // 'copy-insertable' into this map (see {Requirements on 'VALUE'}).
        // Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the (template parameter) 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

 #if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    list(std::initializer_list<value_type> values,
         const ALLOCATOR&                  basicAllocator = ALLOCATOR());
        // Construct a list, initialized from the specified 'values' in the
        // initializer list.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the template parameter 'ALLOCATOR' argument
        // is of type 'bsl::allocator' and 'basicAllocator' is not supplied,
        // the currently installed default allocator is used to supply memory.
        // The items in the list will be in the order in which they appear in
        // the initializer list.  Note that a 'bslma::Allocator *' can be
        // supplied for 'basicAllocator' if the (template parameter)
        // 'ALLOCATOR' is 'bsl::allocator' (the default).
#endif

    ~list();
        // Destroy this list by calling the destructor for each element and
        // deallocating all allocated storage.

    // MANIPULATORS

                            // *** assignment ***

    list& operator=(const list& rhs);
        // Assign to this object the value of the specified 'rhs' object,
        // propagate to this object the allocator of 'rhs' if the 'ALLOCATOR'
        // type has trait 'propagate_on_container_copy_assignment', and return
        // a reference providing modifiable access to this object.  If an
        // exception is thrown, '*this' is left in a valid but unspecified
        // state.  This method requires that the (template parameter) type
        // 'VALUE' be 'copy-assignable' and 'copy-insertable' into this list.
        // Note that, to the extent possible, existing elements of this list
        // are copy-assigned to, to minimize the number of nodes that need to
        // be copy-inserted or erased.

    list& operator=(BloombergLP::bslmf::MovableRef<list> rhs);
        // Assign to this object the value of the specified 'rhs' object,
        // propagate to this object the allocator of 'rhs' if the 'ALLOCATOR'
        // type has trait 'propagate_on_container_copy_assignment', and return
        // a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this list if
        // 'get_allocator() == rhs.get_allocator()' (after accounting for the
        // aforementioned trait); otherwise, 'rhs' is copy-assigned to this
        // object, in which case, to the extent possible, existing elements of
        // this list are move-assigned to, to minimize the number of nodes that
        // need to be move-inserted or erased.  This method requires that the
        // (template parameter) type 'VALUE' be 'move-assignable' and
        // 'move-insertable' into this list (see {Requirements on 'VALUE'}).
        // Note that if this throws, this object may be left in an invalid but
        // destructible state.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    list& operator=(std::initializer_list<value_type> rhs);
        // Assign to this list the value of the of the specified initializer
        // list 'rhs', and return a reference providing modifiable access to
        // this list.  The elements of 'rhs' will appear in the list in the
        // order in which they appear in 'rhs'.  This method requires that the
        // (template parameter) type 'VALUE' be 'copy-assignable' and
        // 'copy-insertable' into this list.  Note that, to the extent
        // possible, existing elements of this list are copy-assigned to, to
        // minimize the number of nodes that need to be copy-inserted or
        // erased.
#endif

    template <class INPUT_ITERATOR>
    void assign(INPUT_ITERATOR srcBegin, INPUT_ITERATOR srcEnd,
                typename enable_if<
                    !is_arithmetic<INPUT_ITERATOR>::value &&
                    !is_enum<INPUT_ITERATOR>::value
                >::type * = 0)
        // Assign to this list the sequence of values of the elements of the
        // specified range '[srcBegin, srcEnd)'.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type', and 'value_type' must be
        // 'emplace-constructible' from '*i' into this list, where 'i' is a
        // dereferenceable iterator in the range '[srcBegin .. srcEnd)'.  The
        // behavior is undefined unless 'srcBegin' and 'srcEnd' refer to a
        // sequence of valid values where 'srcBegin' is at a position at or
        // before 'srcEnd'.  Note that, to the extent possible, existing
        // elements of this list are copy-assigned to, to minimize the number
        // of nodes that need to be copy-inserted or erased.
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // enable_if be in-place inline.

        iterator       dstIt  = this->begin();
        const iterator dstEnd = this->end();

        for (; srcBegin != srcEnd && dstEnd != dstIt; ++srcBegin, ++dstIt) {
            *dstIt = *srcBegin;
        }

        erase(dstIt, dstEnd);

        for (; srcBegin != srcEnd; ++srcBegin) {
            emplace(dstEnd, *srcBegin);
        }
    }

    void assign(size_type n, const value_type& value);
        // Replace the contents of this list with the specified 'n' copies of
        // the specified 'value'.  Each element in this list is set by either
        // copy assignment or "copy insertion" from 'value'.  Note that, to the
        // extent possible, existing elements of this list are copy-assigned
        // to, to minimize the number of nodes that need to be copy-inserted or
        // erased.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void assign(std::initializer_list<value_type> values);
        // Assign to this list the value of the of the specified initializer
        // list 'values', and return a reference providing modifiable access to
        // this list.  The elements of 'values' will appear in the list in the
        // order in which they appear in 'values'.  Note that, to the extent
        // possible, existing elements of this list are copy-assigned to, to
        // minimize the number of nodes that need to be copy-inserted or
        // erased.
#endif

                              // *** iterators ***

    iterator begin() BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects
        // maintained by this list, or the 'end' iterator if this list is
        // empty.

    iterator end() BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing modifiable access to the past-the-end
        // element in the sequence of 'value_type' objects maintained by this
        // list.

    reverse_iterator rbegin() BSLS_CPP11_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // 'value_type' object in the sequence of 'value_type' objects
        // maintained by this list, or 'rend' if this list is empty.

    reverse_iterator rend() BSLS_CPP11_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the
        // prior-to-the-beginning element in the sequence of 'value_type'
        // objects maintained by this list.

                            // *** modify size ***

    void clear() BSLS_CPP11_NOEXCEPT;
        // Remove all the elements from this list.

    void resize(size_type sz);
        // Resize this list to the specified 'sz' elements.  If 'sz' is less
        // than or equal to the previous size of this list, then erase the
        // excess elements from the end.  Otherwise, append additional elements
        // to the end using "default-insertion" until there are a total of 'sz'
        // elements.

    void resize(size_type sz, const value_type& c);
        // Resize this list to the specified 'sz' elements, with added elements
        // being copies of the specified value 'c'.  If 'sz' is less than or
        // equal to the previous size of this list, then erase the excess
        // elements from the end.  Otherwise, append additional elements to the
        // end using "copy-insertion" from 'c' until there are a total of 'sz'
        // elements.

                           // *** element access ***

    reference back();
        // Return a reference providing modifiable access to the last element
        // of this list.  The behavior is undefined unless this list contains
        // at least one element.

    reference front();
        // Return a reference providing modifiable access to the first element
        // of this list.  The behavior is undefined unless this list contains
        // at least one element.

                                // *** end erase ***

    void pop_back();
        // Remove and destroy the last element of this list.  The behavior is
        // undefined unless this list contains at least one element.

    void pop_front();
        // Remove and destroy the first element of this list.  The behavior is
        // undefined unless this list contains at least one element.

                         // *** random access erase ***

    iterator erase(const_iterator position);
        // Remove from this list the element at the specified 'position', and
        // return an iterator providing modifiable access to the element
        // immediately following the removed element, or to the position
        // returned by the 'end' method if the removed element was the last in
        // the sequence.  The behavior is undefined unless 'position' refers to
        // an element in this list.

    iterator erase(const_iterator dstBegin, const_iterator dstEnd);
        // Remove from this list the elements starting at the specified
        // 'dstBegin' position up to, but not including, the specified 'dstEnd'
        // position, and return an non-const iterator equivalent to 'dstEnd'.
        // The behavior is undefined unless 'dstBegin' is an iterator in the
        // range '[begin() .. end()]' and 'dstEnd' is an iterator in the range
        // '[dstBegin .. end()]' (both endpoints included).  Note that
        // 'dstBegin' may be equal to 'dstEnd', in which case the list is not
        // modified.

                            // *** end inserts ***

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

    void push_back(const value_type& value);
        // Append a new element to the end of this list using "copy-insertion"
        // from the specified value 'value'.

    void push_back(BloombergLP::bslmf::MovableRef<value_type> value);
        // Append a new element to the end of this list using "move-insertion"
        // from the specified value 'value'.

    void push_front(const value_type& value);
        // Insert a new element at the front of this list using
        // "copy-insertion" from the specified value 'value'.

    void push_front(BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert a new element at the front of this list using
        // "move-insertion" from the specified value 'value'.

                       // *** random access inserts ***

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    iterator emplace(const_iterator position, ARGS&&... args);
        // Return an interator providing modifiable access to a new element
        // inserted immediately preceding the element at the specified
        // 'position' in this list, constructed by forward the specified
        // (variable number of) 'args' to 'allocator_traits<ALLOC>::construct'
        // which will construct a 'value_type'.
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

    iterator insert(const_iterator position, const value_type& value);
        // Insert into this list a copy of the specified 'value' before the
        // element at the specified 'position' of this list.  The newly
        // inserted element in this list is copy-inserted from 'value'.

    iterator insert(const_iterator                             position,
                    BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert into this list a copy of the specified 'value' before the
        // element at the specified 'position' of this list.  The newly
        // inserted element in this list is move-inserted from 'value'.

    iterator insert(const_iterator    position,
                    size_type         n,
                    const value_type& value);
        // Insert into this list the specified 'n' copies of the specified
        // 'value' before the element at the specified 'position' of this list.
        // Each of the 'n' newly inserted elements in this list are
        // copy-inserted from 'value'.

    template <class INPUT_ITERATOR>
    iterator insert(const_iterator dstPosition,
                    INPUT_ITERATOR srcBegin,
                    INPUT_ITERATOR srcEnd,
                    typename enable_if<
                        !is_arithmetic<INPUT_ITERATOR>::value &&
                        !is_enum<INPUT_ITERATOR>::value
                    >::type * = 0)
        // Insert the specified range '[srcBegin .. srcEnd)' into this list
        // before the specified 'dstPosition', and return an iterator to the
        // srcBegin inserted element or 'dstPosition' if the range is empty.
        // The (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type', and 'value_type' must be 'emplace-constructible' from
        // '*i' into this map, where 'i' is a dereferenceable iterator in the
        // range '[srcBegin .. srcEnd)' (see {Requirements on 'KEY' and
        // 'VALUE'}).  The behavior is undefined unless 'srcBegin' and 'srcEnd'
        // refer to a sequence of valid values where 'srcBegin' is at a
        // dstPosition at or before 'srcEnd'.
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // enable_if be in place inline.

        if (srcBegin == srcEnd) {
            return dstPosition.unconst();                             // RETURN
        }

        // Remember the dstPosition of the srcBegin insertion.  We can't assume
        // 'INPUT_ITERATOR' has a post increment available.

        iterator ret = insert(dstPosition, *srcBegin);
        for (++srcBegin; srcBegin != srcEnd; ++srcBegin) {
            insert(dstPosition, *srcBegin);
        }

        return ret;
    }

                          // *** list operations ***

    void merge(list&                                other);
    void merge(BloombergLP::bslmf::MovableRef<list> other);
        // Merge the specified sorted list 'other' into this sorted list.  The
        // method has no effect if 'other' is this list, otherwise 'other' is
        // left empty.  The behavior is undefined unless both 'other' and this
        // list are sorted in non-decreasing order according to the ordering
        // returned by 'operator<', and unless both 'other' and this list use
        // the same allocator.

    template <class COMPARE>
    void merge(list& other, COMPARE comp);
    template <class COMPARE>
    void merge(BloombergLP::bslmf::MovableRef<list> other, COMPARE comp);
        // Merge the specified sorted list 'other' into this sorted list, using
        // the specified predicate 'comp', which defines a strict weak
        // ordering, to order elements.  The method has no effect if 'other' is
        // this list, otherwise 'other' is left empty.  The behavior is
        // undefined unless both 'other' and this list are sorted in
        // non-decreasing order according to the ordering returned by 'comp',
        // and unless both 'other' and this list use the same allocator.

    void remove(const value_type& value);
        // Erase all the elements having the specified 'value' from this list.

    template <class PREDICATE>
    void remove_if(PREDICATE pred);
        // Remove and destroy all elements in this list for which the specifid
        // 'pred' returns 'true'.

    void reverse();
        // Reverse the order of the elements in this list.

    void sort();
        // Sort this list in non-decreasing order according to the order
        // returned by 'operator<'.  'operator<' must provide a strict weak
        // ordering over 'value_type', meaning that:
        //: o 'a < b' && 'b < c' implies 'a < c'
        //: o 'a < b' implies '!(b < a)'
        //: o '!(a < b)' does not have to necessarily imply that 'b < a'
        // The sort is stable, meaning that if '!(a < b) && !(b < a)',
        // then the ordering of elements 'a' and 'b' in the sequence will be
        // preserved.

    template <class COMPARE>
    void sort(COMPARE comp);
        // Sort this list in non-decreasing order according to the order
        // returned by the specified predicate comparator 'comp'.  'comp' must
        // define a strict weak ordering over 'value_type', meaning that:
        //: o 'comp(a, b)' && 'comp(b, c)' implies 'comp(a, c)'
        //: o 'comp(a, b)' implies '!comp(b, a)'
        //: o '!comp(a, b)' does not have to necessarily imply that
        //:   'comp(b, a)'
        // The sort is stable, meaning that if '!comp(a, b) && !comp(b, a)',
        // then the ordering of elements 'a' and 'b' in the sequence will be
        // preserved.

    void splice(const_iterator                       dstPosition,
                list&                                src);
    void splice(const_iterator                       dstPosition,
                BloombergLP::bslmf::MovableRef<list> src);
        // Insert elements of the specified list 'src' before the element at
        // the specified 'dstPosition' of this list, and remove those elements
        // from 'src'.  The behavior is undefined unless 'src' is not this
        // list, and unless this list and 'src' share the same allocator, and
        // unless 'dstPosition' is a valid position in this list, or is equal
        // to the 'end' iterator of this list, and unless 'src' is not this
        // list.

    void splice(const_iterator                       dstPosition,
                list&                                src,
                const_iterator                       srcNode);
    void splice(const_iterator                       dstPosition,
                BloombergLP::bslmf::MovableRef<list> src,
                const_iterator                       srcNode);
        // Insert the single element at the specified 'srcNode' position in the
        // specified list 'src' before the element at the specified
        // 'dstPosition' of this list, and remove this element from 'src'.  The
        // behavior is undefined unless 'srcNode' refers to a valid element in
        // 'src', and unless this list and 'src' share the same allocator, and
        // unless 'dstPosition' is a valid position in this list, or is equal
        // to the 'end' iterator of this list.  Note that 'src' and '*this' may
        // be the same list.

    void splice(const_iterator                       dstPosition,
                list&                                src,
                const_iterator                       srcBegin,
                const_iterator                       srcEnd);
    void splice(const_iterator                       dstPosition,
                BloombergLP::bslmf::MovableRef<list> src,
                const_iterator                       srcBegin,
                const_iterator                       srcEnd);
        // Insert the elements starting at the specified 'srcBegin' position
        // and before the specified 'srcEnd' position from the specified list
        // 'src' into this list, right before the element at the specified
        // 'dstPosition', and remove those elements from 'src'.  The behavior
        // is undefined unless '[srcBegin .. srcEnd)' represents a range of
        // valid elements in 'src', and 'dstPosition' is not in the range
        // '[srcBegin .. srcEnd)', and unless this list and 'src' share the
        // same allocator, and unless 'dstPosition' is a valid position in this
        // list, or is equal to the 'end' iterator of this list.  Note that
        // 'src' and '*this' may be the same list.

    void unique();
        // Erase from this list all but the first element of every consecutive
        // group of elements that have the same value.

    template <class EqPredicate>
    void unique(EqPredicate binary_pred);
        // Erase from this list all but the first element of every consecutive
        // group of elements for which the specified 'binary_pred' predicate
        // returns 'true' for any two consecutive elements in the group.

                              // *** misc ***

    void swap(list& other);
        // Exchange the value of this object with the value of the specified
        // 'other' object.  Additionally, if
        // 'bsl::allocator_traits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true', then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee and
        // guarantees 'O[1]' complexity.  The behavior is undefined unless
        // either this object was created with the same allocator as 'other' or
        // 'propagate_on_container_swap' is 'true'.

    // ACCESSORS

                               // *** iterators ***

    const_iterator begin() const BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this list, or the 'end' iterator if this list is
        // empty.

    const_iterator cbegin() const BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this list, or the 'cend' iterator if this list is
        // empty.

    const_iterator cend() const BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this list.

    const_reverse_iterator crbegin() const BSLS_CPP11_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this list, or 'crend' if this list is empty.

    const_reverse_iterator crend() const BSLS_CPP11_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this list.

    const_iterator end() const BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this list.

    const_reverse_iterator rbegin() const BSLS_CPP11_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this list, or 'rend' if this list is empty.

    const_reverse_iterator rend() const BSLS_CPP11_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this list.

                                  // *** size ***

    bool empty() const BSLS_CPP11_NOEXCEPT;
        // Return 'true' if this list has no elements and 'false' otherwise.

    size_type max_size() const BSLS_CPP11_NOEXCEPT;
        // Return an upper bound on the largest number of elements that this
        // list could possibly hold.  Note that return value of this function
        // does not guarantee that this list can successfully grow that large,
        // or even close to that large without running out of resources.

    size_type size() const BSLS_CPP11_NOEXCEPT;
        // Return the number of elements in this list.

                           // *** element access ***

    const_reference back() const;
        // Return a reference providing non-modifiable access to the last
        // element of this list.  The behavior is undefined unless this list
        // contains at least one element.

    const_reference front() const;
        // Return a reference providing non-modifiable access to the first
        // element of this list.  The behavior is undefined unless this list
        // contains at least one element.

                                // *** misc ***

    allocator_type get_allocator() const BSLS_CPP11_NOEXCEPT;
        // Return a copy of the allocator used for memory allocation by this
        // list.
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

// FREE FUNCTIONS
template <class VALUE, class ALLOCATOR>
void swap(list<VALUE, ALLOCATOR>& lhs, list<VALUE, ALLOCATOR>& rhs);
    // Exchange the value and comparator of the specified 'a' object with the
    // value and comparator of the specified 'b' object.  Additionally, if
    // 'bsl::allocator_traits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true', then exchange the allocator of 'a' with that of 'b'.  If
    // 'propagate_on_container_swap' is 'true' or 'a' and 'b' were created with
    // the same allocator, then this method provides the no-throw
    // exception-safety guarantee and has 'O[1]' complexity; otherwise, this
    // method has 'O[n + m]' complexity, where 'n' and 'm' are the number of
    // elements in 'a' and 'b', respectively.  Note that 'a' and 'b' are left
    // in valid but unspecified states if an exception is thrown (in the case
    // where 'propagate_on_container_swap' is 'false' and 'a' and 'b' were
    // created with different allocators).

                    // ====================================
                    // class bsl::list::AllocAndSizeWrapper
                    // ====================================

template <class VALUE, class ALLOCATOR>
class list<VALUE, ALLOCATOR>::AllocAndSizeWrapper :
                                     public list<VALUE, ALLOCATOR>::NodeAlloc {
    // This struct is wrapper around the allocator and size data member.  It
    // takes advantage of the empty-base optimization (EBO) so that if the
    // allocator is stateless, it takes up no space.
    //
    // TBD: This struct should eventually be replaced by the use of a general
    // EBO-enabled component that provides a 'pair'-like interface.  (A
    // properly-optimized 'tuple' would do the job.)

    // PRIVATE TYPES
    typedef typename AllocTraits::size_type size_type;

    // DATA
    size_type d_size;  // allocated size

  public:
    // CREATORS
    AllocAndSizeWrapper(const NodeAlloc& basicAllocator,
                        size_type        size);
        // Create an allocator wrapper having the specified 'basicAllocator'
        // and the specified allocated 'size'.

    // MANIPULATOR
    size_type& size();
        // Set the size field of this 'class'.

    // ACCESSOR
    const size_type& size() const;
        // Return the size field of this 'class'.
};

                        // ============================
                        // class bsl::list::NodeProctor
                        // ============================

template <class VALUE, class ALLOCATOR>
class list<VALUE, ALLOCATOR>::NodeProctor {
    // This class provides a proctor to free a node containing an uninitialized
    // 'VALUE' object in the event that an exception is thrown.

    // DATA
    list     *d_list_p;  // list to proctor
    NodePtr   d_node_p;  // node to free upon destruction

  public:
    // CREATORS
    explicit NodeProctor(list *list_p, NodePtr node_p);
        // Create a node proctor object that will use the specified list
        // 'list_p' to free the specified node 'node_p'.

    ~NodeProctor();
        // Destroy this node proctor, and free the node it contains unless the
        // 'release' method has been called before.  Note that the 'd_value'
        // field of the node is not destroyed.

    // MANIPULATORS
    void release();
        // Detach the node contained in this proctor from the proctor.  After
        // calling this 'release' method, the proctor no longer frees any node
        // upon its destruction.
};

                        // ================================
                        // class bsl::list::CompLessThanImp
                        // ================================

template <class VALUE, class ALLOCATOR>
struct list<VALUE, ALLOCATOR>::CompLessThanImp {
    // Binary function predicate object type for comparing two 'VALUE' objects
    // using 'operator<'.  This operation is usually, but not always, the same
    // as that provided by 'std::less<VALUE>'.  The standard requires that
    // certain functions use 'operator<', which means that divergent
    // specializations of 'std::less' are ignored.

    // ACCESSORS
    bool operator()(const VALUE& lhs, const VALUE& rhs) const;
        // Return 'true' if the value of the specified 'lhs' is less than that
        // of the specified 'rhs', and 'false' otherwise.
};

// ============================================================================
//                   INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                           // ------------------------
                           // class bsl::List_Iterator
                           // ------------------------

// CREATORS
template <class VALUE>
inline
List_Iterator<VALUE>::List_Iterator()
: d_node_p()
{
}

template <class VALUE>
inline
List_Iterator<VALUE>::List_Iterator(Node *node_p)
: d_node_p(node_p)
{
}

template <class VALUE>
inline
List_Iterator<VALUE>::List_Iterator(const NcIter& other)
: d_node_p(other.d_node_p)
{
}

// MANIPULATORS
template <class VALUE>
inline
List_Iterator<VALUE>& List_Iterator<VALUE>::operator++()
{
    this->d_node_p = this->d_node_p->d_next_p;
    return *this;
}

template <class VALUE>
inline
List_Iterator<VALUE>& List_Iterator<VALUE>::operator--()
{
    this->d_node_p = this->d_node_p->d_prev_p;
    return *this;
}

template <class VALUE>
inline
List_Iterator<VALUE> List_Iterator<VALUE>::operator++(int)
{
    List_Iterator temp = *this;
    this->operator++();
    return temp;
}

template <class VALUE>
inline
List_Iterator<VALUE> List_Iterator<VALUE>::operator--(int)
{
    List_Iterator temp = *this;
    this->operator--();
    return temp;
}

// ACCESSORS
template <class VALUE>
inline
typename bsl::List_Iterator<VALUE>::reference
                                        List_Iterator<VALUE>::operator*() const
{
    return this->d_node_p->d_value;
}

template <class VALUE>
inline
typename bsl::List_Iterator<VALUE>::pointer
                                       List_Iterator<VALUE>::operator->() const
{
    return BloombergLP::bsls::Util::addressOf(this->d_node_p->d_value);
}

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(bsl::List_Iterator<T1> lhs, bsl::List_Iterator<T2> rhs)
{
    // Make sure that this comparison will only compile if 'T1' and 'T2' match
    // except for a possible difference in 'const'-ness.

    BSLMF_ASSERT((bsl::is_same<typename bsl::remove_cv<T1>::type,
                               typename bsl::remove_cv<T2>::type>::value));

    return lhs.d_node_p == rhs.d_node_p;
}

template <class T1, class T2>
inline
bool operator!=(bsl::List_Iterator<T1> lhs, bsl::List_Iterator<T2> rhs)
{
    // Make sure that this comparison will only compile if 'T1' and 'T2' match
    // except for a possible difference in 'const'-ness.

    BSLMF_ASSERT((bsl::is_same<typename bsl::remove_cv<T1>::type,
                               typename bsl::remove_cv<T2>::type>::value));

    return ! (lhs == rhs);
}

                         // --------------------------
                         // class bsl::list::NodeAlloc
                         // --------------------------

// CREATOR
template <class VALUE, class ALLOCATOR>
inline
list<VALUE, ALLOCATOR>::AllocAndSizeWrapper::AllocAndSizeWrapper(
                                               const NodeAlloc& basicAllocator,
                                               size_type        size)
: NodeAlloc(basicAllocator), d_size(size)
{
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::AllocAndSizeWrapper::size_type&
list<VALUE, ALLOCATOR>::AllocAndSizeWrapper::size()
{
    return d_size;
}

template <class VALUE, class ALLOCATOR>
inline
const typename list<VALUE, ALLOCATOR>::AllocAndSizeWrapper::size_type&
list<VALUE, ALLOCATOR>::AllocAndSizeWrapper::size() const
{
    return d_size;
}

                        // ----------------------------
                        // class bsl::list::NodeProctor
                        // ----------------------------

// CREATORS
template <class VALUE, class ALLOCATOR>
inline
list<VALUE, ALLOCATOR>::NodeProctor::NodeProctor(list *list_p, NodePtr node_p)
: d_list_p(list_p), d_node_p(node_p)
{
    BSLS_ASSERT_SAFE(list_p);
    BSLS_ASSERT_SAFE(node_p);
}

template <class VALUE, class ALLOCATOR>
inline
list<VALUE, ALLOCATOR>::NodeProctor::~NodeProctor()
{
    if (d_node_p) {
        d_list_p->freeNode(d_node_p);
    }
}

// MANIPULATORS
template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::NodeProctor::release()
{
    d_node_p = 0;
}

                        // --------------------------------
                        // class bsl::list::CompLessThanImp
                        // --------------------------------

// ACCESSORS
template <class VALUE, class ALLOCATOR>
inline
bool list<VALUE, ALLOCATOR>::CompLessThanImp::operator()(
                                      const VALUE& lhs, const VALUE& rhs) const
{
    return lhs < rhs;
}

                             // ---------------
                             // class bsl::list
                             // ---------------

// PRIVATE MANIPULATORS
template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::NodeAlloc&
                                         list<VALUE, ALLOCATOR>::allocatorImp()
{
    return d_alloc_and_size;  // Implicit cast to base class
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::NodePtr list<VALUE, ALLOCATOR>::allocateNode()
{
    NodePtr ret = AllocTraits::allocate(allocatorImp(), 1);
    ret->d_prev_p = 0;
    ret->d_next_p = 0;
    return ret;
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::createSentinel()
{
    BSLS_ASSERT_SAFE(size_type(-1) == sizeRef() || 0 == sizeRef());

    d_sentinel = allocateNode();
    linkNodes(d_sentinel, d_sentinel);  // circular
    sizeRef() = 0;
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::deleteNode(NodePtr node)
{
    AllocTraits::destroy(allocatorImp(),
                         BloombergLP::bsls::Util::addressOf(node->d_value));
    AllocTraits::deallocate(allocatorImp(), node, 1);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::destroyAll()
{
    clear();
    freeNode(d_sentinel);
    sizeRef() = size_type(-1);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::freeNode(NodePtr node)
{
    AllocTraits::deallocate(allocatorImp(), node, 1);
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insertNode(const_iterator position, NodePtr node)
{
    typename AllocTraits::pointer next = position.d_node_p;
    typename AllocTraits::pointer prev = next->d_prev_p;
    linkNodes(prev, node);
    linkNodes(node, next);
    ++sizeRef();
    return iterator(node);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::linkNodes(NodePtr prev, NodePtr next)
{
    prev->d_next_p = next;
    next->d_prev_p = prev;
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
typename list<VALUE, ALLOCATOR>::NodePtr
list<VALUE, ALLOCATOR>::mergeImp(NodePtr node1,
                                 NodePtr node2,
                                 NodePtr finish,
                                 COMPARE comp)
{
    NodePtr pre = node1->d_prev_p;

    // The only possible throwing operation is the comparator.  Exception
    // neutrality is created by ensuring that this list is in a valid state,
    // with no disconnected nodes, before the comparator is called.

    // Having the two sublists be contiguous parts of the same list has the
    // following advantages:
    //: 1 When we reach the end of a sublist, there is no "finalization" step
    //:   where the end of the remaining sublist must be spliced onto the
    //:   merged list.
    //: 2 No cleanup needed if an exception is thrown; the size and validity of
    //:   the resulting list needs no adjustment.

    while (node1 != node2 && node2 != finish) {
        // Loop invariants:
        // - The open range (pre, node1) is the current merged result
        // - The half-open range [node1, node2) is the 1st unmerged sequence
        // - The half-open range [node2, finish) is the 2nd unmerged sequence

        if (comp(node2->d_value, node1->d_value)) {
            // 'node2' should come before 'node1'.

            // Find the end of the sequence of elements that belong before
            // node1 so that we can splice them all at once.

            NodePtr lastMove = node2;
            NodePtr next2    = node2->d_next_p;
            while (next2 != finish && comp(next2->d_value, node1->d_value)) {
                lastMove = next2;
                next2 = lastMove->d_next_p;
            }

            linkNodes(node2->d_prev_p, next2);
            linkNodes(node1->d_prev_p, node2);
            linkNodes(lastMove, node1);

            // Advance to next node in the 2nd unmerged sequence.

            node2 = next2;
        }
        else {
            // Advance to next node in the 1st unmerged sequence.

            node1 = node1->d_next_p;
        }
    }

    return pre->d_next_p;
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::quickSwap(list *other)
{
    BSLS_ASSERT_SAFE(allocatorImp() == other->allocatorImp());

    using std::swap;

    swap(d_sentinel, other->d_sentinel);
    swap(sizeRef(),  other->sizeRef());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::AllocTraits::size_type&
list<VALUE, ALLOCATOR>::sizeRef() BSLS_CPP11_NOEXCEPT
{
    return d_alloc_and_size.size();
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
typename list<VALUE, ALLOCATOR>::NodePtr
list<VALUE, ALLOCATOR>::sortImp(NodePtr       *node1_p,
                                size_type      size,
                                const COMPARE& comp)
{
    BSLS_ASSERT(size > 0);

    NodePtr node1 = *node1_p;
    if (size < 2) {
        return node1->d_next_p;                                       // RETURN
    }

    size_type half = size / 2;

    NodePtr node2 = sortImp(&node1, half,        comp);
    NodePtr next  = sortImp(&node2, size - half, comp);

    *node1_p = mergeImp(node1, node2, next, comp);
    return next;
}

// PRIVATE ACCESSORS
template <class VALUE, class ALLOCATOR>
inline
const typename list<VALUE, ALLOCATOR>::NodeAlloc&
list<VALUE, ALLOCATOR>::allocatorImp() const
{
    return d_alloc_and_size;  // Implicit cast to base class
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::NodePtr list<VALUE, ALLOCATOR>::headNode()
                                                                          const
{
    return d_sentinel->d_next_p;
}

template <class VALUE, class ALLOCATOR>
inline
const typename list<VALUE, ALLOCATOR>::AllocTraits::size_type&
list<VALUE, ALLOCATOR>::sizeRef() const BSLS_CPP11_NOEXCEPT
{
    return d_alloc_and_size.size();
}

// CREATORS
template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list()
: d_sentinel()
, d_alloc_and_size(ALLOCATOR(), 0)
{
    createSentinel();
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, 0)
{
    createSentinel();
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(size_type n)
: d_sentinel()
, d_alloc_and_size(ALLOCATOR(), size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(this->allocatorImp());

    // Default-construct (value-initialize) 'n' elements into 'tmp'.  'tmp's
    // destructor will clean up if an exception is thrown.

    iterator pos = tmp.end();
    for (size_type i = 0; i < n; ++i) {
        tmp.emplace(pos);
    }

    quickSwap(&tmp);  // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(size_type        n,
                             const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(this->allocatorImp());

    // Default-construct (value-initialize) 'n' elements into 'tmp'.  'tmp's
    // destructor will clean up if an exception is thrown.

    const_iterator pos = tmp.cend();
    for (size_type i = 0; i < n; ++i) {
        tmp.emplace(pos);
    }

    quickSwap(&tmp);  // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(size_type        n,
                             const VALUE&     value,
                             const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(this->allocatorImp());
    tmp.insert(tmp.cbegin(), n, value);  // 'tmp's destructor will clean up on
                                         // throw.
    quickSwap(&tmp);      // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(const list& original)
: d_sentinel()
, d_alloc_and_size(
   AllocTraits::select_on_container_copy_construction(original.allocatorImp()),
   size_type(-1))
{
    list tmp(this->allocatorImp());

    tmp.insert(tmp.cbegin(), original.begin(), original.end());

    quickSwap(&tmp);  // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(const list&      original,
                             const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    list tmp(this->allocatorImp());

    tmp.insert(tmp.cbegin(), original.begin(), original.end());

    quickSwap(&tmp);  // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(BloombergLP::bslmf::MovableRef<list> original)
: d_sentinel()
, d_alloc_and_size(MoveUtil::access(original).allocatorImp(), 0)
{
    // Allocator should be copied, not moved, to ensure identical allocators
    // between this and 'original', otherwise 'swap' is undefined.

    // An rvalue must be left in a valid state after a move.

    createSentinel();

    // '*this' is now in a valid state.

    quickSwap(&MoveUtil::access(original));
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(
                           BloombergLP::bslmf::MovableRef<list> original,
                           const ALLOCATOR&                     basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list& lvalue = original;
    if (basicAllocator == lvalue.allocatorImp()) {
        // An rvalue must be left in a valid state after a move.

        createSentinel();      // '*this' is now in a valid state.
        quickSwap(&lvalue);
    }
    else {
        // different allocators, must copy

        list tmp(this->allocatorImp());

        // This was 'tmp.insert(tmp.begin(), lvalue.begin(), lvalue.end());',
        // but we have permission to move from 'original' so we should do that
        // to avoid relying on VALUE's copy c'tor.

        NodePtr end_p  = lvalue.d_sentinel;
        for (NodePtr p = lvalue.headNode(); end_p != p;  p = p->d_next_p) {
            tmp.emplace_back(MoveUtil::move(p->d_value));
        }

        quickSwap(&tmp);  // Leave 'tmp' in an invalid but destructible state.
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE, class ALLOCATOR>
inline
list<VALUE, ALLOCATOR>::list(std::initializer_list<VALUE> values,
                             const ALLOCATOR&             basicAllocator)
: d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).  Create a
    // temporary list, 'tmp' with the specified data.  If an exception is
    // thrown, 'tmp's destructor will clean up.  Otherwise, swap 'tmp' with
    // '*this', leaving 'tmp' in an invalid but destructible state and leaving
    // '*this' fully constructed.

    list tmp(this->allocatorImp());
    tmp.insert(tmp.cbegin(), values.begin(), values.end());

    quickSwap(&tmp);
}
#endif

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::~list()
{
    // A size of -1 means a special incompletely-initialized state with no
    // sentinel, which requires no destruction.

    if (sizeRef() != size_type(-1)) {
        destroyAll();
    }
}

// MANIPULATORS

                            // *** assignment ***

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>& list<VALUE, ALLOCATOR>::operator=(const list& rhs)
{
    if (this == &rhs) {
        return *this;                                                 // RETURN
    }

    if (AllocTraits::propagate_on_container_copy_assignment::value
       && allocatorImp() != rhs.allocatorImp()) {
        // We can't simply swap containers, as we aren't allowed to modify
        // 'rhs'.

        // Completely destroy and rebuild list using new allocator.

        // Create a new list with the new allocator.  This operation might
        // throw, so we do it before destroying the old list.

        list tmp(rhs, rhs.allocatorImp());

        // Clear existing list and leave in an invalid but destructible state.

        destroyAll();

        // Assign allocator (here we are relying on the C++11 standard, which
        // requires that the allocator type not to throw on copy or assign) as
        // 'quickSwap' requires the entities to have the same allocator.

        allocatorImp() = rhs.allocatorImp();

        // Now swap lists, leaving 'tmp' in an invalid but destructible state.

        quickSwap(&tmp);
    }
    else {
        assign(rhs.begin(), rhs.end());
    }

    return *this;
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>& list<VALUE, ALLOCATOR>::operator=(
                                      BloombergLP::bslmf::MovableRef<list> rhs)
{
    list& lvalue = rhs;

    if (this == &lvalue) {
        return *this;                                                 // RETURN
    }

    if (this->allocatorImp() == lvalue.allocatorImp()) {
        // Equal allocators, just swap contents, will never throw.

        quickSwap(&lvalue);
    }
    else if (AllocTraits::propagate_on_container_move_assignment::value) {
        // An rvalue must be left in a valid state after a move.  Both '*this'
        // and 'rhs' must be left in valid states after a throw.

        // Note: tearing everything down, then changing the allocator, then
        // doing 'quickSwap(&lvalue)' has a problem that in it could leave
        // 'rhs' in an invalid state, since if 'this->createSentinel()' were
        // called after the tearing down to render '*this' to a valid value,
        // 'createSentinel' might throw, leaving '*this' in an invalid state.

        // Swap everything, including the allocator.  Note that we are relying
        // on the C++11 standard guarantee that the allocator type won't throw
        // on swap, and we know that the other two won't throw.

        using std::swap;

        swap(allocatorImp(), lvalue.allocatorImp());
        swap(d_sentinel,     lvalue.d_sentinel);     // swap of pointers
        swap(sizeRef(),      lvalue.sizeRef());      // swap of 'size_t's
    }
    else {
        // Unequal allocators and no moving of allocators, do linear move copy.
        // Note that if this throws part way through, both '*this' and 'rhs'
        // may be left changed.

        NodePtr              dst_p    = this->headNode();
        const const_iterator dstEnd   = this->cend();
        const NodePtr        dstEnd_p = dstEnd.d_node_p;

        NodePtr              src_p    = lvalue.headNode();
        const NodePtr        srcEnd_p = lvalue.d_sentinel;

        for (; srcEnd_p != src_p && dstEnd_p != dst_p;
                            src_p = src_p->d_next_p, dst_p = dst_p->d_next_p) {
            dst_p->d_value = MoveUtil::move(src_p->d_value);
        }

        erase(const_iterator(dst_p), dstEnd);

        for (; srcEnd_p != src_p; src_p = src_p->d_next_p) {
            emplace(dstEnd, MoveUtil::move(src_p->d_value));
        }
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE, class ALLOCATOR>
inline
list<VALUE, ALLOCATOR>& list<VALUE, ALLOCATOR>::operator=(
                                              std::initializer_list<VALUE> rhs)
{
    assign(rhs.begin(), rhs.end());
    return *this;
}
#endif

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::assign(size_type n, const VALUE& value)
{
    NodePtr              dst_p    = this->headNode();
    const const_iterator dstEnd   = this->cend();
    const NodePtr        dstEnd_p = dstEnd.d_node_p;

    for (; 0 < n && dstEnd_p != dst_p; --n, dst_p = dst_p->d_next_p) {
        dst_p->d_value = value;
    }

    erase(const_iterator(dst_p), dstEnd);

    for (; 0 < n; --n) {
        insert(dstEnd, value);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::assign(std::initializer_list<VALUE> values)
{
    assign(values.begin(), values.end());
}
#endif

                              // *** iterators ***

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::iterator list<VALUE, ALLOCATOR>::begin()
                                                            BSLS_CPP11_NOEXCEPT
{
    return iterator(headNode());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::iterator list<VALUE, ALLOCATOR>::end()
                                                            BSLS_CPP11_NOEXCEPT
{
    return iterator(d_sentinel);
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::reverse_iterator
list<VALUE, ALLOCATOR>::rbegin() BSLS_CPP11_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::reverse_iterator
list<VALUE, ALLOCATOR>::rend() BSLS_CPP11_NOEXCEPT
{
    return reverse_iterator(begin());
}

                            // *** modify size ***

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::clear() BSLS_CPP11_NOEXCEPT
{
    const NodePtr e = d_sentinel;
    for (NodePtr p = d_sentinel->d_next_p; e != p; ) {
        NodePtr condemned = p;
        p = p->d_next_p;
        deleteNode(condemned);
    }

    linkNodes(d_sentinel, d_sentinel);
    sizeRef() = 0;
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::resize(size_type sz)
{
    if (sz > sizeRef()) {
        const_iterator ce = cend();
        do {
            emplace(ce);
        } while (sz > sizeRef());
    }
    else {
        NodePtr e = d_sentinel;
        NodePtr p = e->d_prev_p;
        for (size_type d = sizeRef() - sz; d > 0; --d) {
            NodePtr condemned = p;
            p = p->d_prev_p;
            deleteNode(condemned);
        }
        linkNodes(p, e);
        sizeRef() = sz;
    }
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::resize(size_type sz, const VALUE& c)
{
    if (sz > sizeRef()) {
        const_iterator ce = cend();
        do {
            emplace(ce, c);
        } while (sz > sizeRef());
    }
    else {
        NodePtr e = d_sentinel;
        NodePtr p = e->d_prev_p;
        for (size_type d = sizeRef() - sz; d > 0; --d) {
            NodePtr condemned = p;
            p = p->d_prev_p;
            deleteNode(condemned);
        }
        linkNodes(p, e);
        sizeRef() = sz;
    }
}

                                // element access:

template <class VALUE, class ALLOCATOR>
inline
VALUE& list<VALUE, ALLOCATOR>::back()
{
    BSLS_ASSERT_SAFE(sizeRef() > 0);

    return d_sentinel->d_prev_p->d_value;
}

template <class VALUE, class ALLOCATOR>
inline
VALUE& list<VALUE, ALLOCATOR>::front()
{
    BSLS_ASSERT_SAFE(sizeRef() > 0);

    return headNode()->d_value;
}

                                // *** end erase ***

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::pop_back()
{
    BSLS_ASSERT_SAFE(sizeRef() > 0);

    erase(--cend());
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::pop_front()
{
    BSLS_ASSERT_SAFE(sizeRef() > 0);

    erase(cbegin());
}

                         // *** random access erase ***

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT(position.d_node_p != d_sentinel);

    NodePtr condemned = position.d_node_p;
    iterator ret(condemned->d_next_p);

    linkNodes(condemned->d_prev_p, condemned->d_next_p);
    deleteNode(condemned);
    --sizeRef();
    return ret;
}

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::erase(const_iterator dstBegin, const_iterator dstEnd)
{
    NodePtr       p = dstBegin.d_node_p;
    const NodePtr e = dstEnd.  d_node_p;
    linkNodes(p->d_prev_p, e);

    size_type numDeleted = 0;
    for (; e != p; ++numDeleted) {
        NodePtr condemned = p;
        p = p->d_next_p;
        deleteNode(condemned);
    }

    sizeRef() -= numDeleted;

    return iterator(e);
}

                            // *** end inserts ***

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
    emplace(cend());
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01)
{
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02>
inline
void list<VALUE, ALLOCATOR>::emplace_back(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02)
{
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
}
// }}} END GENERATED CODE
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class ALLOCATOR>
template <class... ARGS>
inline
void list<VALUE, ALLOCATOR>::emplace_front(ARGS&&... args)
{
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
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
    emplace(cbegin());
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01)
{
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01));
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02>
inline
void list<VALUE, ALLOCATOR>::emplace_front(
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02)
{
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
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
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
}
// }}} END GENERATED CODE
#endif

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::push_back(const VALUE& value)
{
    emplace(cend(), value);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::push_back(
                                   BloombergLP::bslmf::MovableRef<VALUE> value)
{
    emplace(cend(), MoveUtil::move(value));
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::push_front(const VALUE& value)
{
    emplace(cbegin(), value);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::push_front(
                                   BloombergLP::bslmf::MovableRef<VALUE> value)
{
    emplace(cbegin(), MoveUtil::move(value));
}

                       // *** random access inserts ***

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class ALLOCATOR>
template <class... ARGS>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position, ARGS&&... args)
{
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
    proctor.release();
    return insertNode(position, p);
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_list.h
template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position)
{
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value));
    proctor.release();
    return insertNode(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01)
{
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01));
    proctor.release();
    return insertNode(position, p);
}

template <class VALUE, class ALLOCATOR>
template <class ARGS_01,
          class ARGS_02>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::emplace(const_iterator position,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_01) args_01,
                            BSLS_COMPILERFEATURES_FORWARD_REF(ARGS_02) args_02)
{
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02));
    proctor.release();
    return insertNode(position, p);
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
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03));
    proctor.release();
    return insertNode(position, p);
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
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04));
    proctor.release();
    return insertNode(position, p);
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
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05));
    proctor.release();
    return insertNode(position, p);
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
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06));
    proctor.release();
    return insertNode(position, p);
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
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_01,args_01),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_02,args_02),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_03,args_03),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_04,args_04),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_05,args_05),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_06,args_06),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS_07,args_07));
    proctor.release();
    return insertNode(position, p);
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
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
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
    return insertNode(position, p);
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
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
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
    return insertNode(position, p);
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
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
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
    return insertNode(position, p);
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
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...);
    proctor.release();
    return insertNode(position, p);
}
// }}} END GENERATED CODE
#endif

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(const_iterator position, const VALUE& value)
{
    return emplace(position, value);
}

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(const_iterator                        position,
                               BloombergLP::bslmf::MovableRef<VALUE> value)
{
    return emplace(position, MoveUtil::move(value));
}

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(const_iterator position,
                               size_type      n,
                               const VALUE&   value)
{
    if (0 == n) {
        return position.unconst();                                    // RETURN
    }

    // Remember the position of the first node inserted before 'position'.

    iterator ret = emplace(position, value);

    // And put the rest of the nodes after it.

    for (--n; n > 0; --n) {
        emplace(position, value);
    }

    return ret;
}

                          // *** list operations ***

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::merge(list& other)
{
    BSLS_ASSERT(this->allocatorImp() == other.allocatorImp());

    merge(other, CompLessThanImp());
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::merge(BloombergLP::bslmf::MovableRef<list> other)
{
    list& lvalue = other;

    BSLS_ASSERT(this->allocatorImp() == lvalue.allocatorImp());

    merge(lvalue, CompLessThanImp());
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
void list<VALUE, ALLOCATOR>::merge(list& other, COMPARE comp)
{
    if (&other == this) {
        return;                                                       // RETURN
    }

    BSLS_ASSERT(this->allocatorImp() == other.allocatorImp());

    if (other.empty()) {
        // This is a important special case to avoid pointing to sentinel.

        return;                                                       // RETURN
    }

    // Splice 'other' to the end of '*this', but remember the first node of the
    // appended sequence.

    NodePtr xfirst = other.d_sentinel->d_next_p;
    splice(end(), other);

    // Call 'mergeImp' with a pointer to the first node of the original list, a
    // pointer to the first node of 'other' (which also ends the original list)
    // and a pointer to the sentinel (which now ends 'other').

    mergeImp(d_sentinel->d_next_p, xfirst, d_sentinel, comp);
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
inline
void list<VALUE, ALLOCATOR>::merge(BloombergLP::bslmf::MovableRef<list> other,
                                   COMPARE                              comp)
{
    list& lvalue = other;

    BSLS_ASSERT(this->allocatorImp() == lvalue.allocatorImp());

    merge(lvalue, comp);
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::remove(const VALUE& value)
{
    const const_iterator e = cend();
    for (const_iterator i = cbegin(); e != i; ) {
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
    const iterator e = end();
    for (iterator i = begin(); e != i; ) {
        if (pred(*i)) {
            i = erase(i);
        }
        else {
            ++i;
        }
    }
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::reverse()
{
    NodePtr sentinel = d_sentinel;
    NodePtr p = sentinel;

    do {
        NodePtr tmp = p->d_next_p;
        p->d_next_p = p->d_prev_p;
        p->d_prev_p = tmp;
        p = tmp;
    } while (p != sentinel);
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::sort()
{
    sort(CompLessThanImp());
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
void list<VALUE, ALLOCATOR>::sort(COMPARE comp)
{
    if (sizeRef() < 2) {
        return;                                                       // RETURN
    }
    NodePtr node1 = d_sentinel->d_next_p;
    sortImp(&node1, size(), comp);
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::splice(const_iterator position, list& src)
{
    BSLS_ASSERT(allocatorImp() == src.allocatorImp());
    BSLS_ASSERT(&src != this);

    if (src.empty()) {
        return;                                                       // RETURN
    }

    typename AllocTraits::pointer pos   = position.d_node_p;
    typename AllocTraits::pointer first = src.headNode();
    typename AllocTraits::pointer last  = src.d_sentinel->d_prev_p;
    size_type n = src.sizeRef();

    // Splice contents out of src.

    linkNodes(src.d_sentinel, src.d_sentinel);
    src.sizeRef() = 0;

    // Splice contents into *this.

    linkNodes(pos->d_prev_p, first);
    linkNodes(last, pos);
    sizeRef() += n;
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::splice(
                              const_iterator                       dstPosition,
                              BloombergLP::bslmf::MovableRef<list> src)
{
    splice(dstPosition, MoveUtil::access(src));
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::splice(const_iterator dstPosition,
                                    list&          src,
                                    const_iterator srcNode)
{
    BSLS_ASSERT(allocatorImp() == src.allocatorImp());

    typename AllocTraits::pointer pos_p          = dstPosition.d_node_p;
    typename AllocTraits::pointer srcNode_p      = srcNode.d_node_p;
    typename AllocTraits::pointer afterSrcNode_p = srcNode_p->d_next_p;

    if (pos_p == srcNode_p || pos_p == afterSrcNode_p) {
        return;                                                       // RETURN
    }

    // Splice contents out of src.

    linkNodes(srcNode_p->d_prev_p, afterSrcNode_p);
    --src.sizeRef();

    // Splice contents into *this.

    linkNodes(pos_p->d_prev_p, srcNode_p);
    linkNodes(srcNode_p,       pos_p);
    ++sizeRef();
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::splice(
                              const_iterator                       dstPosition,
                              BloombergLP::bslmf::MovableRef<list> src,
                              const_iterator                       srcNode)
{
    splice(dstPosition, MoveUtil::access(src), srcNode);
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::splice(const_iterator dstPosition,
                                    list&          src,
                                    const_iterator srcBegin,
                                    const_iterator srcEnd)
{
    BSLS_ASSERT(allocatorImp() == src.allocatorImp());

    size_type n = bsl::distance(srcBegin, srcEnd);

    if (0 == n) {
        return;                                                       // RETURN
    }

    typename AllocTraits::pointer pos_p      = dstPosition.d_node_p;
    typename AllocTraits::pointer srcBegin_p = srcBegin.d_node_p;
    typename AllocTraits::pointer srcEnd_p   = srcEnd.d_node_p;
    typename AllocTraits::pointer srcLast_p  = srcEnd_p->d_prev_p;

    // Splice contents out of src.

    linkNodes(srcBegin_p->d_prev_p, srcEnd_p);
    src.sizeRef() -= n;

    // Splice contents into *this.

    linkNodes(pos_p->d_prev_p, srcBegin_p);
    linkNodes(srcLast_p,       pos_p);
    sizeRef() += n;
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::splice(
                              const_iterator                       dstPosition,
                              BloombergLP::bslmf::MovableRef<list> src,
                              const_iterator                       srcBegin,
                              const_iterator                       srcEnd)
{
    splice(dstPosition, MoveUtil::access(src), srcBegin, srcEnd);
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

                              // *** misc ***

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::swap(list& other)
{
    // C++11 behavior for member 'swap': undefined for unequal allocators.
    // BSLS_ASSERT(allocatorImp() == other.allocatorImp());

    // C++17 behavior for free 'swap': *defined* for unequal allocators (if a
    // Bloomberg proposal to that effect is accepted).  Note that free 'swap'
    // currently forwards to this implementation.

    // backward compatible behavior: swap with copies

    if (AllocTraits::propagate_on_container_swap::value) {
        using std::swap;

        swap(d_sentinel,     other.d_sentinel);
        swap(allocatorImp(), other.allocatorImp());
        swap(sizeRef(),      other.sizeRef());
    }
    else if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                     allocatorImp() == other.allocatorImp())) {
        quickSwap(&other);
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

        // Swap with copies.  Note the copies might throw.

        // We considered making this move-copies rather than copy-copies, but
        // if they throw, that could leave parts of '*this' and 'other' in a
        // moved-from state, and it seems desirable that 'swap' should either
        // succeed completely or leave its arguments completely unchanged.
        // Also, it is unclear how a move-copy with unequal allocators would be
        // any more efficient than an non-move copy.

        list toOtherCopy(*this, other.allocatorImp());
        list toThisCopy( other,       allocatorImp());

        toOtherCopy.quickSwap(&other);
        toThisCopy. quickSwap(&*this);
    }
}

// ACCESSORS

                               // *** iterators ***

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::begin() const BSLS_CPP11_NOEXCEPT
{
    return const_iterator(headNode());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::end() const BSLS_CPP11_NOEXCEPT
{
    return const_iterator(d_sentinel);
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::cbegin() const BSLS_CPP11_NOEXCEPT
{
    return begin();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::cend() const BSLS_CPP11_NOEXCEPT
{
    return end();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::crbegin() const BSLS_CPP11_NOEXCEPT
{
    return rbegin();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::crend() const BSLS_CPP11_NOEXCEPT
{
    return rend();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::rbegin() const BSLS_CPP11_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::rend() const BSLS_CPP11_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

                                  // *** size ***

template <class VALUE, class ALLOCATOR>
inline
bool list<VALUE, ALLOCATOR>::empty() const BSLS_CPP11_NOEXCEPT
{
    return 0 == sizeRef();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::size_type
list<VALUE, ALLOCATOR>::max_size() const BSLS_CPP11_NOEXCEPT
{
    return AllocTraits::max_size(allocatorImp());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::size_type list<VALUE, ALLOCATOR>::size() const
                                                            BSLS_CPP11_NOEXCEPT
{
    return sizeRef();
}

                           // *** element access ***

template <class VALUE, class ALLOCATOR>
inline
const VALUE& list<VALUE, ALLOCATOR>::back() const
{
    BSLS_ASSERT_SAFE(sizeRef() > 0);

    NodePtr last = d_sentinel->d_prev_p;
    return last->d_value;
}

template <class VALUE, class ALLOCATOR>
inline
const VALUE& list<VALUE, ALLOCATOR>::front() const
{
    BSLS_ASSERT_SAFE(sizeRef() > 0);

    return headNode()->d_value;
}

                                // *** misc ***

template <class VALUE, class ALLOCATOR>
inline
ALLOCATOR list<VALUE, ALLOCATOR>::get_allocator() const BSLS_CPP11_NOEXCEPT
{
    return allocatorImp();
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
