// bslstl_list.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_LIST
#define INCLUDED_BSLSTL_LIST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant list class.
//
//@CLASSES:
//   bsl::list: STL-compatible list template
//
//@CANONICAL_HEADER: bsl_list.h
//
//@SEE_ALSO: bslstl_deque
//
//@DESCRIPTION: This component defines a single class template, 'bsl::list',
// implementing the standard container holding a sequence of elements (of a
// template parameter type, 'VALUE').  All list operations involving a single
// element are constant-time, including insertion and removal of an element
// anywhere in the list.  Operations that do not change the number of elements
// are performed without calling constructors, destructors, swap, or assignment
// on the individual elements.  (I.e., they are performed by
// pointer-manipulation alone.)  A 'list' does not provide random access to its
// elements; although access to the first and last elements of a 'list' is
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
// to the C++11 standard when compiled with a C++11 compiler, and makes the
// best approximation when compiled with a C++03 compiler.  In particular, for
// C++03 we emulate move semantics, but limit forwarding (in 'emplace') to
// 'const' lvalues, and make no effort to emulate 'noexcept' or
// initializer-lists.
//
///Requirements on 'VALUE'
///-----------------------
// A 'list' is a fully Value-Semantic Type (see {'bsldoc_glossary'}) only if
// the supplied 'VALUE' template parameter is itself fully value-semantic.  It
// is possible to instantiate a 'list' with a 'VALUE' parameter argument that
// does not provide a full set of value-semantic operations, but then some
// methods of the container may not be instantiable.  The following
// terminology, adopted from the C++11 standard, is used in the function
// documentation of 'list' to describe a function's requirements for the
// 'VALUE' template parameter.  These terms are also defined in sections
// [utility.arg.requirements] and [container.requirements.general] of the C++11
// standard.
//
///Memory Allocation
///-----------------
// The type supplied as a list's 'ALLOCATOR' template parameter determines how
// that list will allocate memory.  The 'list' template supports allocators
// meeting the requirements of the C++11 standard [17.6.3.5]; in addition, it
// supports scoped-allocators derived from the 'bslma::Allocator' memory
// allocation protocol.  Clients intending to use 'bslma'-style allocators
// should use the template's default 'ALLOCATOR' type: The default type for the
// 'ALLOCATOR' template parameter, 'bsl::allocator', provides a C++11
// standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of a 'list' instantiation is
// 'bsl::allocator', then objects of that list type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a list accepts
// an optional 'bslma::Allocator' argument at construction.  If the address of
// a 'bslma::Allocator' object is explicitly supplied at construction, the list
// uses it to supply memory for the list throughout its lifetime; otherwise,
// the list will use the default allocator installed at the time of the list's
// construction (see 'bslma_default').  In addition to directly allocating
// memory from the indicated 'bslma::Allocator', a list supplies that
// allocator's address to the constructors of contained objects of the
// (template parameter) 'VALUE' type if it defines the
// 'bslma::UsesBslmaAllocator' trait.
//
///Comparators and Strict Weak Ordering
/// - - - - - - - - - - - - - - - - - -
// A comparator function 'comp(a, b)' defines a *strict* *weak* *ordering* if
//: o 'comp(a, b)' && 'comp(b, c)' implies 'comp(a, c)'
//: o 'comp(a, b)' implies '!comp(b, a)'
//: o '!comp(a, b)' does not imply that 'comp(b, a)'
//
///Glossary
///--------
//..
//  Legend
//  ------
//  'X'    - denotes an allocator-aware container type (e.g., 'list')
//  'T'    - 'value_type' associated with 'X'
//  'A'    - type of the allocator used by 'X'
//  'm'    - lvalue of type 'A' (allocator)
//  'p',   - address ('T *') of uninitialized storage for a 'T' within an 'X'
//  'rv'   - rvalue of type (non-'const') 'T&&'
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
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'list':
//..
//  Legend
//  ------
//  'V'             - template parameter 'VALUE' type of the list
//  'A'             - template parameter 'ALLOCATOR' type of the list
//  'a', 'b'        - distinct objects of type 'list<V>'
//  'k'             - unsigned integral constant
//  'ra','rb'       - distinct modifiable rvalue objects of type 'list<V>&&'
//  'n', 'm'        - number of elements in 'a' and 'b', respectively
//  'al'            - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'V' objects
//  'v'             - an object of type 'V'
//  'rv'            - modifiable rvalue object of type 'V&&'
//  'p1', 'p2'      - two iterators belonging to 'a'
//  's1', 's2'      - two iterators belonging to 'b'
//  'pred'          - a unary predicate
//  'binary_pred'   - a binary predicate
//  'comp'          - a binary predicate implementing a strict-weak ordering
//  'args...'       - a variadic list of (up to 10) arguments
//  '{*}'           - C++11-style initializer list of length 'ni'
//  distance(i1,i2) - the number of elements in the range '[i1 .. i2)'
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | list<V> a;    (default construction)               | O[1]               |
//  | list<V> a(al);                                     |                    |
//  +----------------------------------------------------+--------------------+
//  | list<V> a(b); (copy construction)                  | O[m]               |
//  | list<V> a(b, al);                                  |                    |
//  +----------------------------------------------------+--------------------+
//  | list<V> a(rb); (move construction)                 | O[1]               |
//  | list<V> a(rb, al);                                 | O[1] if 'a' and 'b'|
//  |                                                    | use the same       |
//  |                                                    | allocator,         |
//  |                                                    | O[m]     otherwise |
//  +----------------------------------------------------+--------------------+
//  | list<V> a(k);                                      | O[k]               |
//  | list<V> a(k, v);                                   |                    |
//  | list<V> a(k, v, al);                               |                    |
//  +----------------------------------------------------+--------------------+
//  | list<V> a(i1, i2);                                 | O[distance(i1, i2)]|
//  | list<V> a(i1, i2, al);                             |                    |
//  +----------------------------------------------------+--------------------+
//  | list<V> a({*}, al = A())                           | O[ni]              |
//  +----------------------------------------------------+--------------------+
//  | a.~list<V>(); (destruction)                        | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;                (copy assignment)            | O[max(n, m)]       |
//  +----------------------------------------------------+--------------------+
//  | a = {*};              (copy assignment)            | O[max(n, ni)]      |
//  +----------------------------------------------------+--------------------+
//  | a = rb;               (move assignment)            | O[1] if 'a' and 'b'|
//  |                                                    | use the same       |
//  |                                                    | allocator,         |
//  |                                                    | O[max(n, m)]       |
//  |                                                    | otherwise          |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  | a.rbegin(), a.rend(), a.crbegin(), a.crend()       |                    |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a < b, a <= b, a > b, a >= b                       | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a, b)                              | O[1] if 'a' and 'b'|
//  |                                                    | use the same       |
//  |                                                    | allocator,         |
//  |                                                    | O[n + m] otherwise |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.get_allocator()                                  | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.emplace(p1, args...)                             | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, v)                                    | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, k, v)                                 | O[k]               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, i1, i2)                               | O[distance(i1, i2)]|
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, rv)                                   | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, {*})                                  | O[ni]              |
//  +----------------------------------------------------+--------------------|
//  | a.erase(p1)                                        | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | O[distance(p1, p2)]|
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.assign(i1,i2)                                    | O[distance(i1, i2)]|
//  +----------------------------------------------------+--------------------+
//  | a.assign(k, v)                                     | O[max(n, k)]       |
//  +----------------------------------------------------+--------------------+
//  | a.assign({*})                                      | O[max(n, ni)]      |
//  +----------------------------------------------------+--------------------+
//  | a.front(), a.back()                                | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.emplace_front(args...), a.emplace_back(args...)  | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.push_front(v),                                   |                    |
//  | a.push_back(v)                                     | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.push_front(rv),                                  |                    |
//  | a.push_back(rv)                                    | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.pop_front(), a.pop_back()                        | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.resize(k), a.resize(k, v)                        | O[k]               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, b),  a.splice(p, b, s1)                | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, rb), a.splice(p, rb, s1)               | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, b, s1, s2)                             | O[distance(s1, s2)]|
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, rb, s1, s2)                            | O[distance(s1, s2)]|
//  +----------------------------------------------------+--------------------+
//  | a.remove(t), a.remove_if(pred)                     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.unique(), a.unique(binary_pred)                  | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.merge(b),  a.merge(b, comp)                      | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.merge(rb), a.merge(rb, comp)                     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.sort(), a.sort(comp)                             | O[n*log(n)]        |
//  +----------------------------------------------------+--------------------+
//  | a.reverse()                                        | O[n]               |
//  +----------------------------------------------------+--------------------+
//..
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

#include <bslscm_version.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
#include <bslstl_iteratorutil.h>

#include <bslalg_rangecompare.h>
#include <bslalg_synththreewayutil.h>
#include <bslalg_typetraithasstliterators.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_isstdallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isenum.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_removecv.h>
#include <bslmf_typeidentity.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_performancehint.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <algorithm>   // for std::swap in C++03 or earlier

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>
#endif

#include <utility>   // for std::swap in C++11 or later

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslstl_list.h
# define COMPILING_BSLSTL_LIST_H
# include <bslstl_list_cpp03.h>
# undef COMPILING_BSLSTL_LIST_H
#else

namespace bsl {

                        // =====================
                        // struct bsl::List_Node
                        // =====================

template <class VALUE>
class List_Node {
    // PRIVATE CLASS TEMPLATE.  For use only by 'bsl::list' implementation.
    // An instance of 'List_Node<T>' is a single node in a doubly-linked list
    // used to implement 'bsl::list<T,A>', for a given element type 'T' and
    // allocator type 'A'.  Note that an instantiation of this 'class' for a
    // given 'bsl::list' is independent of the allocator type.

    // DATA
    List_Node *d_prev_p;   // pointer to the previous node in the list
    List_Node *d_next_p;   // pointer to the next node in the list
    VALUE      d_value;    // list element

    // FRIENDS
    template <class LIST_VALUE, class LIST_ALLOCATOR>
    friend class list;

    template <class ITER_VALUE>
    friend class List_Iterator;

  private:
    // NOT IMPLEMENTED
    List_Node();                              // = delete;
    List_Node(const List_Node&);              // = delete;
    ~List_Node();                             // = delete;
    List_Node& operator=(const List_Node&);   // = delete;

    // A 'List_Node' is never constructed, copied, destroyed, or assigned to.
    // The 'd_value' field is constructed directly by 'list::emplace', and
    // destroyed directly by 'list::erase'.
};

                        // ========================
                        // class bsl::List_Iterator
                        // ========================

#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
// On Solaris studio12-v4, <algorithm> is compatible only with iterators
// inheriting from 'std::iterator'.

template <class VALUE>
class List_Iterator :
                 public std::iterator<std::bidirectional_iterator_tag, VALUE> {
#else
template <class VALUE>
class List_Iterator {
#endif
    // Implementation of 'bsl::list::iterator'.

    // PRIVATE TYPES
    typedef typename remove_cv<VALUE>::type  NcType;
    typedef List_Iterator<NcType>            NcIter;
    typedef List_Node<NcType>                Node;

    // DATA
    Node *d_node_p;      // pointer to list node

    // FRIENDS
    template <class LIST_VALUE, class LIST_ALLOCATOR>
    friend class list;

    template <class ITER_VALUE>    // This 'friend' statement is needed for the
    friend class List_Iterator;    // case where 'VALUE' != 'ITER_VALUE'.

    template <class T1, class T2>
    friend bool operator==(List_Iterator<T1>, List_Iterator<T2>);

  private:
    // PRIVATE ACCESSORS
    NcIter unconst() const;
        // Return an iterator providing modifiable access to the list element
        // that this list iterator refers to.

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
        // decremented, or dereferenced until assigned a non-singular value).

    explicit List_Iterator(Node *nodePtr);
        // Create an iterator that references the value pointed to by the
        // specified 'nodePtr'.  If '0 == nodePtr' the iterator will be
        // singular.

    List_Iterator(const NcIter& other);                             // IMPLICIT
        // Create an iterator that has the same value as the specified 'other'
        // iterator.  If the (template parameter) type 'VALUE' is not
        // 'const'-qualified, then this constructor is the copy constructor;
        // otherwise, the copy constructor is implicitly generated.  Note that
        // this method is marked "IMPLICIT" in case it is not the copy
        // constructor.
        //
        // Note that this means that a 'List_Iterator<const VALUE>' can be copy
        // constructed or assigned to from a 'List_Iterator<VALUE>', but not
        // vice-versa.

    // Compiler-generated copy constructor, destructor, and copy-assignment
    // operator:
    //: o List_Iterator(const List_Iterator&); // Maybe defaulted (see above).
    //: o ~List_Iterator() = default;
    //: o List_Iterator& operator=(const List_Iterator&) = default;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
    ~List_Iterator() = default;
        // Default compiler-generated destructor.

    List_Iterator& operator=(const List_Iterator&) = default;
        // Default compiler-generated copy-assignment operator.
#endif

    // MANIPULATORS
    List_Iterator& operator++();
        // Advance this iterator to the next element in the list and return its
        // new value.  The behavior is undefined unless this iterator is in the
        // range '[begin() .. end())' for some list (i.e., the iterator is not
        // singular, is not 'end()', and has not been invalidated).

    List_Iterator& operator--();
        // Regress this iterator to the previous element in the list and return
        // its new value.  The behavior is undefined unless this iterator is in
        // the range '(begin() .. end()]' for some list (i.e., the iterator is
        // not singular, is not 'begin()', and has not been invalidated).

    List_Iterator operator++(int);
        // Advance this iterator to the next element in the list and return its
        // previous value.  The behavior is undefined unless this iterator is
        // in the range '[begin() .. end())' for some list (i.e., the iterator
        // is not singular, is not 'end()', and has not been invalidated).

    List_Iterator operator--(int);
        // Regress this iterator to the previous element in the list and return
        // its previous value.  The behavior is undefined unless this iterator
        // is in the range '(begin() .. end()]' for some list (i.e., the
        // iterator is not singular, is not 'begin()', and has not been
        // invalidated).

    // ACCESSORS
    reference operator*() const;
        // Return a reference providing modifiable access to the element
        // referenced by this iterator.  The behavior is undefined unless this
        // iterator is in the range '[begin() .. end())' for some list (i.e.,
        // the iterator is not singular, is not 'end()', and has not been
        // invalidated).

    pointer operator->() const;
        // Return a pointer providing modifiable access to the element
        // referenced by this iterator.  The behavior is undefined unless this
        // iterator is in the range '[begin() .. end())' for some list (i.e.,
        // the iterator is not singular, is not 'end()', and has not been
        // invalidated).
};

// FREE OPERATORS
template <class T1, class T2>
bool operator==(List_Iterator<T1> lhs, List_Iterator<T2> rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators have the same
    // value, and 'false' otherwise.  Two iterators have the same value if both
    // refer to the same element of the same list or both are the 'end()'
    // iterator of the same list.  The behavior is undefined unless both 'lhs'
    // and 'rhs' refer to the same list.  Note that the different types 'T1'
    // and 'T2' are to facilitate comparisons between 'const' and non-'const'
    // iterators and there will be a compilation error if 'T1' and 'T2' differ
    // in any way other than 'const'-ness.

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class T1, class T2>
bool operator!=(List_Iterator<T1> lhs, List_Iterator<T2> rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not have the
    // same value, and 'false' otherwise.  Two iterators do not have the same
    // value unless both refer to the same element of the same list or unless
    // both are the 'end()' iterator of the same list.  The behavior is
    // undefined unless both 'lhs' and 'rhs' refer to the same list.  Note that
    // the different types 'T1' and 'T2' are to facilitate comparisons between
    // 'const' and non-'const' iterators and there will be a compilation error
    // if 'T1' and 'T2' differ in any way other than 'const'-ness.
#endif

                         // ===========================
                         // struct List_DefaultLessThan
                         // ===========================

template <class VALUE>
struct List_DefaultLessThan {
    // Binary predicate type for comparing two 'VALUE' objects using
    // 'operator<'.  This operation is usually, but not always, the same as
    // that provided by 'std::less<VALUE>'.  The standard requires that certain
    // functions use 'operator<', which means that divergent specializations of
    // 'std::less' are to be ignored.

    // ACCESSORS
    bool operator()(const VALUE& lhs, const VALUE& rhs) const;
        // Return 'true' if the value of the specified 'lhs' is less than that
        // of the specified 'rhs', and 'false' otherwise.
};

                        // ==============================
                        // class List_AllocAndSizeWrapper
                        // ==============================

template <class VALUE, class ALLOCATOR>
class List_AllocAndSizeWrapper : public allocator_traits<ALLOCATOR>::
                    template rebind_traits<List_Node<VALUE> >::allocator_type {
    // This struct is a wrapper around the allocator and size data members of a
    // 'list'.  It takes advantage of the empty-base optimization (EBO) so that
    // if the allocator is stateless, it takes up no space.
    //
    // TBD: This struct should eventually be replaced by the use of a general
    // EBO-enabled component that provides a 'pair'-like interface.  (A
    // properly-optimized 'tuple' would do the job.)

    // PRIVATE TYPES
    typedef List_Node<VALUE>                      Node;

    typedef typename allocator_traits<ALLOCATOR>::template rebind_traits<Node>
                                                  AllocTraits;
        // Alias for the allocator traits type associated with the 'bsl::list'
        // container.

    typedef typename AllocTraits::allocator_type  NodeAlloc;    // base class

    typedef typename AllocTraits::size_type       size_type;

    // DATA
    size_type d_size;      // Number of elements in the list (not including the
                           // sentinel).

  private:
    // NOT IMPLEMENTED
    List_AllocAndSizeWrapper(const List_AllocAndSizeWrapper&);
    List_AllocAndSizeWrapper& operator=(const List_AllocAndSizeWrapper&);

  public:
    // CREATORS
    List_AllocAndSizeWrapper(const NodeAlloc& basicAllocator,
                             size_type        size);
        // Create an allocator and size wrapper having the specified
        // 'basicAllocator' and (initial) 'size'.

    // ~List_AllocAndSizeWrapper() = default;

    // MANIPULATORS
    size_type& size();
        // Return a reference providing modifiable access to the 'size' field
        // of this object.

    // ACCESSORS
    const size_type& size() const;
        // Return a reference providing non-modifiable access to the 'size'
        // field of this object.
};

template <class VALUE, class ALLOCATOR>
class list;
    // Forward declaration required by 'List_NodeProctor'.

                            // ======================
                            // class List_NodeProctor
                            // ======================

template <class VALUE, class ALLOCATOR>
class List_NodeProctor {
    // This class provides a proctor to free a node containing an uninitialized
    // 'VALUE' object in the event that an exception is thrown.

    // PRIVATE TYPES
    typedef List_Node<VALUE>                      Node;

    typedef typename allocator_traits<ALLOCATOR>::template rebind_traits<Node>
                                                  AllocTraits;
        // Alias for the allocator traits type associated with the 'bsl::list'
        // container.

  public:
    // PUBLIC TYPES

    // In C++11 'NodePtr' would be generalized as follows:
    // 'typedef pointer_traits<VoidPtr>::template rebind<List_Node> NodePtr;'

    typedef typename AllocTraits::pointer         NodePtr;

  private:
    // DATA
    list<VALUE, ALLOCATOR>  *d_list_p;  // list to proctor
    NodePtr                  d_node_p;  // node to free upon destruction

  private:
    // NOT IMPLEMENTED
    List_NodeProctor(const List_NodeProctor&);
    List_NodeProctor &operator=(const List_NodeProctor&);

  public:
    // CREATORS
    List_NodeProctor(list<VALUE, ALLOCATOR> *listPtr, NodePtr nodePtr);
        // Create a node proctor object that will use the specified list
        // 'listPtr' to free the specified 'nodePtr'.  The behavior is
        // undefined unless 'nodePtr' was allocated by the allocator of
        // '*listPtr'.

    ~List_NodeProctor();
        // Destroy this node proctor, and free the node it contains unless the
        // 'release' method has been called before.  Note that the 'd_value'
        // field of the node is not destroyed.

    // MANIPULATORS
    void release();
        // Detach the node contained in this proctor from the proctor.  After
        // calling this 'release' method, the proctor no longer frees any node
        // upon its destruction.
};

                            // ===============
                            // class bsl::list
                            // ===============

template <class VALUE, class ALLOCATOR = bsl::allocator<VALUE> >
class list {
    // This class template implements a value-semantic container type holding a
    // sequence of elements of the (template parameter) 'VALUE' type.

    // PRIVATE TYPES
    typedef List_DefaultLessThan<VALUE>           DefaultLessThan;
        // Default comparator.

    typedef List_Node<VALUE>                      Node;
        // Alias for the node type in this list.

    typedef List_NodeProctor<VALUE, ALLOCATOR>    NodeProctor;
        // Proctor for guarding a newly allocated node.

    typedef typename allocator_traits<ALLOCATOR>::template rebind_traits<Node>
                                                  AllocTraits;
        // Alias for the allocator traits type associated with this container.

    typedef BloombergLP::bslmf::MovableRefUtil    MoveUtil;
        // Alias for the utility associated with movable references.

    typedef List_AllocAndSizeWrapper<VALUE, ALLOCATOR>
                                                  AllocAndSizeWrapper;
        // Alias for the wrapper containing the (usually stateless) allocator
        // and number of elements stored in this container.

    typedef typename AllocTraits::allocator_type  NodeAlloc;
        // Base class of 'List_AllocAndSizeWrapper' containing the allocator.

    // In C++11 'NodePtr' would be generalized as follows:
    // 'typedef pointer_traits<VoidPtr>::template rebind<List_Node> NodePtr;'

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

    typedef typename allocator_traits<ALLOCATOR>::size_type    size_type;
    typedef typename allocator_traits<ALLOCATOR>::difference_type
                                                               difference_type;
    typedef VALUE                                 value_type;
    typedef ALLOCATOR                             allocator_type;
    typedef bsl::reverse_iterator<iterator>       reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

  private:
    // DATA
    NodePtr             d_sentinel;        // node pointer of sentinel element
    AllocAndSizeWrapper d_alloc_and_size;  // node allocator

    // FRIENDS
    friend class List_NodeProctor<VALUE, ALLOCATOR>;

    // PRIVATE MANIPULATORS
    NodeAlloc& allocatorImp();
        // Return a reference providing modifiable access to the allocator used
        // to allocate nodes.

    NodePtr allocateNode();
        // Return a pointer to a node allocated from the container's allocator.
        // Before returning, the node's pointers are zeroed, but the
        // constructor of the 'value_type' is not called.

    void createSentinel();
        // Create the sentinel node of this list.  The sentinel node does not
        // hold a value.  When first created, its forward and backward pointers
        // point to itself, creating a circular linked list.  This function
        // also sets this list's size to 0.

    void deleteNode(NodePtr node);
        // Destroy the value part of the specified 'node' and free the node's
        // memory.  Do not do any pointer fix-up of the node or its neighbors,
        // and do not update 'sizeRef'.  The behavior is undefined unless
        // 'node' was allocated using the allocator of this list.

    void destroyAll();
        // Erase all elements and deallocate the sentinel node, leaving this
        // list in an invalid but destructible state (i.e., with 'size == -1').

    void freeNode(NodePtr node);
        // Zero out the pointers and deallocate the node pointed to by the
        // specified 'node'.  The behavior is undefined unless 'node' was
        // allocated using the allocator of this list.  Note that 'node's
        // destructor is not called, and, importantly, the value field of
        // 'node' is not destroyed.

    iterator insertNode(const_iterator position, NodePtr node);
        // Insert the specified 'node' prior to the specified 'position' in
        // this list.  The behavior is undefined unless 'node' was allocated
        // using the allocator of this list and 'position' is in the range
        // '[begin() .. end()]'.

    void linkNodes(NodePtr prev, NodePtr next);
        // Modify the forward pointer of the specified 'prev' to point to the
        // specified 'next' and the backward pointer of 'next' to point to
        // 'prev'.  The behavior is undefined unless 'prev' and 'next' point to
        // nodes created with 'allocateNode'.

    template <class COMPARE>
    NodePtr mergeImp(NodePtr node1,
                     NodePtr node2,
                     NodePtr finish,
                     COMPARE comparator);
        // Given a specified pair of nodes 'node1' and 'finish' specifying the
        // range '[node1 .. finish)', with the specified 'node2' pointing
        // somewhere in the middle of the sequence, merge sequence
        // '[node2 .. finish)' into '[node1 .. node2)', and return a pointer to
        // the beginning of the merged sequence, using the specified
        // 'comparator' to determine order.  If an exception is thrown, all
        // nodes remain in this list, but their order is unspecified.  If any
        // nodes in the range '[node1 .. node2)' compare equivalent to any
        // nodes in the range '[node2 .. finish)', the nodes from
        // '[node1 .. node2)' will be merged first.  The behavior is undefined
        // unless '[node1 .. node2)' and '[node2 .. finish)' each describe a
        // contiguous sequence of nodes.

    void quickSwap(list *other);
        // Efficiently exchange the value of this object with that of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    typename AllocTraits::size_type& sizeRef() BSLS_KEYWORD_NOEXCEPT;
        // Return a reference providing modifiable access to the data element
        // holding the size of this list.

    template <class COMPARE>
    NodePtr sortImp(NodePtr        *nodePtrPtr,
                    size_type       size,
                    const COMPARE&  comparator);
        // Sort the sequence of the specified 'size' nodes starting with the
        // specified '*nodePtrPtr', and modify '*nodePtrPtr' to refer to the
        // first node of the sorted sequence.  Return the pointer to the node
        // following the sequence of nodes to be sorted.  Use the specified
        // 'comparator' to compare 'VALUE' type objects.  If an exception is
        // thrown, all nodes remain properly linked, but their order is
        // unspecified.  The behavior is undefined unless '*nodePtrPtr' begins
        // a sequence of at least 'size' nodes, none of which is the sentinel
        // node, and '0 < size'.

    // PRIVATE ACCESSORS
    const NodeAlloc& allocatorImp() const;
        // Return a reference providing non-modifiable access to the allocator
        // used to allocate nodes.

    NodePtr headNode() const;
        // Return a pointer providing modifiable access to the first node in
        // this list or the sentinel node if this list is empty.

    const typename AllocTraits::size_type& sizeRef() const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return a reference providing non-modifiable access to the data
        // element holding the size of this list.

  public:
    // CREATORS
    list();
        // Create an empty list.  A default-constructed object of the (template
        // parameter) type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator', the currently installed default allocator is used.

    explicit list(const ALLOCATOR& basicAllocator);
        // Create an empty list.  Use the specified 'basicAllocator' to supply
        // memory.  If the type 'ALLOCATOR' is 'bsl::allocator' (the default),
        // then 'basicAllocator' shall be convertible to 'bslma::Allocator *'.

    explicit list(size_type numElements);
        // Create a list of the specified 'numElements' size whose every
        // element is default-constructed.  A default-constructed object of the
        // (template parameter) type 'ALLOCATOR' is used.  If the type
        // 'ALLOCATOR' is 'bsl::allocator', the currently installed default
        // allocator is used.  Throw 'bsl::length_error' if
        // 'numElements > max_size()'.  This method requires that the (template
        // parameter) 'VALUE' be 'default-insertable' into this list (see
        // {Requirements on 'VALUE'}).

    list(size_type        numElements,
         const ALLOCATOR& basicAllocator);
        // Create a list of the specified 'numElements' size whose every
        // element is default-constructed.  Use the specified 'basicAllocator'
        // to supply memory.  If the type 'ALLOCATOR' is 'bsl::allocator' (the
        // default), then 'basicAllocator' shall be convertible to
        // 'bslma::Allocator *'.  Throw 'bsl::length_error' if
        // 'numElements > max_size()'.  This method requires that the (template
        // parameter) 'VALUE' be 'default-insertable' into this list (see
        // {Requirements on 'VALUE'}).

    list(size_type         numElements,
         const value_type& value,
         const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create a list of the specified 'numElements' size whose every
        // element has the specified 'value'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default), then 'basicAllocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used.  Throw
        // 'bsl::length_error' if 'numElements > max_size()'.  This method
        // requires that the (template parameter) 'VALUE' be 'copy-insertable'
        // into this list (see {Requirements on 'VALUE'}).

    template <class INPUT_ITERATOR>
    list(INPUT_ITERATOR   first,
         INPUT_ITERATOR   last,
         const ALLOCATOR& basicAllocator = ALLOCATOR(),
         typename enable_if<
                 !is_arithmetic<INPUT_ITERATOR>::value &&
                 !is_enum<INPUT_ITERATOR>::value
         >::type * = 0)
        // Create a list initially containing copies of the values in the range
        // starting at the specified 'first' and ending immediately before the
        // specified 'last' iterators of the (template parameter) type
        // 'INPUT_ITERATOR'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // (the default), then 'basicAllocator', if supplied, shall be
        // convertible to 'bslma::Allocator *'.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  Throw 'bsl::length_error' if
        // the number of elements in '[first .. last)' exceeds the size
        // returned by 'max_size'.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [input.iterators] providing access to
        // values of a type convertible to 'value_type', and 'value_type' must
        // be 'emplace-constructible' from '*i' into this list, where 'i' is a
        // dereferenceable iterator in the range '[first .. last)' (see
        // {Requirements on 'VALUE'}).  The behavior is undefined unless
        // 'first' and 'last' refer to a sequence of valid values where 'first'
        // is at a position at or before 'last'.
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
        tmp.insert(tmp.cbegin(), first, last);
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

    list(const list&                                    original,
         const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a list that has the same value as the specified 'original'
        // object.  Use the specified 'basicAllocator' to supply memory.  This
        // method requires that the (template parameter) 'VALUE' be
        // 'copy-insertable' into this list (see {Requirements on 'VALUE'}).
        // Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    list(BloombergLP::bslmf::MovableRef<list> original);            // IMPLICIT
        // Create a list having the same value as the specified 'original'
        // object by moving (in constant time) the contents of 'original' to
        // the new list.  The allocator associated with 'original' is
        // propagated for use in the newly-created list.  'original' is left in
        // a valid but unspecified state.

    list(BloombergLP::bslmf::MovableRef<list>           original,
         const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a list having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // The contents of 'original' are moved (in constant time) to the new
        // list if 'basicAllocator == original.get_allocator()', and are move-
        // inserted (in linear time) using 'basicAllocator' otherwise.
        // 'original' is left in a valid but unspecified state.  This method
        // requires that the (template parameter) 'VALUE' be 'move-insertable'
        // into this list (see {Requirements on 'VALUE'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // (template parameter) 'ALLOCATOR' is 'bsl::allocator' (the default).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    list(std::initializer_list<value_type> values,
         const ALLOCATOR&                  basicAllocator = ALLOCATOR());
                                                                    // IMPLICIT
        // Create a list and append each 'value_type' object in the specified
        // 'values' initializer list.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // (the default), then 'basicAllocator', if supplied, shall be
        // convertible to 'bslma::Allocator *'.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  This method requires that the
        // (template parameter) 'VALUE' be 'copy-insertable' into this list
        // (see {Requirements on 'VALUE'}).
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

    list& operator=(BloombergLP::bslmf::MovableRef<list> rhs)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                          AllocTraits::is_always_equal::value);
        // Assign to this object the value of the specified 'rhs' object,
        // propagate to this object the allocator of 'rhs' if the 'ALLOCATOR'
        // type has trait 'propagate_on_container_move_assignment', and return
        // a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this list if
        // 'get_allocator() == rhs.get_allocator()' (after accounting for the
        // aforementioned trait); otherwise, all elements in this list are
        // either destroyed or move-assigned to and each additional element in
        // 'rhs' is move-inserted into this list.  'rhs' is left in a valid but
        // unspecified state, and if an exception is thrown, '*this' is left in
        // a valid but unspecified state.  This method requires that the
        // (template parameter) type 'VALUE' be 'move-assignable' and
        // 'move-insertable' into this list (see {Requirements on 'VALUE'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    list& operator=(std::initializer_list<value_type> rhs);
        // Assign to this list, in order, the sequence of values in the
        // specified 'rhs' initializer list, and return a reference providing
        // modifiable access to this list.  This method requires that the
        // (template parameter) type 'VALUE' be 'copy-assignable' and
        // 'copy-insertable' into this list.  Note that, to the extent
        // possible, existing elements of this list are copy-assigned to, to
        // minimize the number of nodes that need to be copy-inserted or
        // erased.
#endif

    template <class INPUT_ITERATOR>
    void assign(INPUT_ITERATOR first,
                INPUT_ITERATOR last,
                typename enable_if<
                    !is_arithmetic<INPUT_ITERATOR>::value &&
                    !is_enum<INPUT_ITERATOR>::value
                >::type * = 0)
        // Assign to this list the sequence of values, in order, of the
        // elements of the specified range '[first .. last)'.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'value_type', and
        // 'value_type' must be 'emplace-constructible' from '*i' into this
        // list, where 'i' is a dereferenceable iterator in the range
        // '[first .. last)'.  The behavior is undefined unless 'first' and
        // 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last'.  Note that, to the extent possible,
        // existing elements of this list are copy-assigned to, to minimize the
        // number of nodes that need to be copy-inserted or erased.  If an
        // exception is thrown, '*this' is left in a valid but unspecified
        // state.
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // enable_if be in-place inline.

        iterator       dstIt  = this->begin();
        const iterator dstEnd = this->end();

        for (; first != last && dstEnd != dstIt; ++first, ++dstIt) {
            *dstIt = *first;
        }

        erase(dstIt, dstEnd);

        for (; first != last; ++first) {
            emplace(dstEnd, *first);
        }
    }

    void assign(size_type numElements, const value_type& value);
        // Replace the contents of this list with the specified 'numElements'
        // copies of the specified 'value'.  Note that, to the extent possible,
        // existing elements of this list are copy-assigned to, to minimize the
        // number of nodes that need to be copy-inserted or erased.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void assign(std::initializer_list<value_type> values);
        // Assign to this list, in order, the sequence of values in the
        // specified 'values' initializer list.  This method requires that the
        // (template parameter) type 'VALUE' be 'copy-assignable' and
        // 'copy-insertable' into this list.  Note that, to the extent
        // possible, existing elements of this list are copy-assigned to, to
        // minimize the number of nodes that need to be copy-inserted or
        // erased.
#endif

                              // *** iterators ***

    iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first element
        // in this list, and the past-the-end iterator if this list is empty.

    iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end (forward) iterator providing modifiable
        // access to this list.

    reverse_iterator rbegin() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // element in this list, and the past-the-end reverse iterator if this
        // list is empty.

    reverse_iterator rend() BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end reverse iterator providing modifiable access
        // to this list.

                            // *** modify size ***

    void clear() BSLS_KEYWORD_NOEXCEPT;
        // Remove all the elements from this list.

    void resize(size_type newSize);
    void resize(size_type newSize, const value_type& value);
        // Change the size of this list to the specified 'newSize'.  Erase
        // 'size() - newSize' elements at the back if 'newSize < size()'.
        // Append 'newSize - size()' elements at the back having the optionally
        // specified 'value' if 'newSize > size()'; if 'value' is not
        // specified, default-constructed objects of the (template parameter)
        // 'VALUE' are emplaced.  This method has no effect if
        // 'newSize == size()'.  Throw 'bsl::length_error' if
        // 'newSize > max_size()'.

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
        // position, and return a non-'const' iterator equivalent to 'dstEnd'.
        // The behavior is undefined unless 'dstBegin' is an iterator in the
        // range '[begin() .. end()]' and 'dstEnd' is an iterator in the range
        // '[dstBegin .. end()]' (both endpoints included).  Note that
        // 'dstBegin' may be equal to 'dstEnd', in which case the list is not
        // modified.

                            // *** end inserts ***

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    reference emplace_back(ARGS&&... arguments);
        // Append to the back of this list a newly created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'arguments' to the corresponding
        // constructor of 'value_type'.  Return a reference providing
        // modifiable access to the inserted element.  If an exception is
        // thrown (other than by the move constructor of a non-copy-insertable
        // 'value_type'), this method has no effect.  This method requires that
        // the (template parameter) 'VALUE' be 'move-insertable' into this list
        // and 'emplace-constructible' from 'arguments' (see {Requirements on
        // 'VALUE'}).
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    reference emplace_front(ARGS&&... arguments);
        // Prepend to the front of this list a newly created 'value_type'
        // object, constructed by forwarding 'get_allocator()' (if required)
        // and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'value_type'.  Return a reference
        // providing modifiable access to the inserted element.  If an
        // exception is thrown (other than by the move constructor of a
        // non-copy-insertable 'value_type'), this method has no effect.  This
        // method requires that the (template parameter) 'VALUE' be
        // 'move-insertable' into this list and 'emplace-constructible' from
        // 'arguments' (see {Requirements on 'VALUE'}).
#endif

    void push_back(const value_type& value);
        // Append to the back of this list a copy of the specified 'value'.
        // This method offers full guarantee of rollback in case an exception
        // is thrown.  This method requires that the (template parameter)
        // 'VALUE' be 'copy-constructible' (see {Requirements on 'VALUE'}).

    void push_back(BloombergLP::bslmf::MovableRef<value_type> value);
        // Append to the back of this list the specified move-insertable
        // 'value'.  'value' is left in a valid but unspecified state.  If an
        // exception is thrown (other than by the move constructor of a
        // non-copy-insertable 'value_type'), this method has no effect.  This
        // method requires that the (template parameter) 'VALUE' be
        // 'move-insertable' into this list (see {Requirements on 'VALUE'}).

    void push_front(const value_type& value);
        // Prepend to the front of this list a copy of the specified 'value'.
        // This method offers full guarantee of rollback in case an exception
        // is thrown.  This method requires that the (template parameter)
        // 'VALUE' be 'copy-constructible' (see {Requirements on 'VALUE'}).

    void push_front(BloombergLP::bslmf::MovableRef<value_type> value);
        // Prepend to the front of this list the specified move-insertable
        // 'value'.  'value' is left in a valid but unspecified state.  If an
        // exception is thrown (other than by the move constructor of a
        // non-copy-insertable 'value_type'), this method has no effect.  This
        // method requires that the (template parameter) 'VALUE' be
        // 'move-insertable' into this list (see {Requirements on 'VALUE'}).

                       // *** random access inserts ***

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... ARGS>
    iterator emplace(const_iterator position, ARGS&&... arguments);
        // Insert at the specified 'position' in this list a newly created
        // 'value_type' object, constructed by forwarding 'get_allocator()' (if
        // required) and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'value_type', and return an iterator
        // providing modifiable access to the newly created and inserted
        // element.  If an exception is thrown (other than by the copy
        // constructor, move constructor, assignment operator, or move
        // assignment operator of 'value_type'), this method has no effect.
        // This method requires that the (template parameter) 'VALUE' be
        // 'move-insertable' into this list and 'emplace-constructible' from
        // 'arguments' (see {Requirements on 'VALUE'}).  The behavior is
        // undefined unless 'position' is an iterator in the range
        // '[cbegin() .. cend()]' (both endpoints included).
#endif

    iterator insert(const_iterator dstPosition, const value_type& value);
        // Insert at the specified 'dstPosition' in this list a copy of the
        // specified 'value', and return an iterator providing modifiable
        // access to the newly inserted element.  This method offers full
        // guarantee of rollback in case an exception is thrown other than by
        // the 'VALUE' copy constructor or assignment operator.  This method
        // requires that the (template parameter) 'VALUE' be 'copy-insertable'
        // into this list (see {Requirements on 'VALUE'}).  The behavior is
        // undefined unless 'dstPosition' is an iterator in the range
        // '[cbegin() .. cend()] (both endpoints included)'.

    iterator insert(const_iterator                             dstPosition,
                    BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert at the specified 'dstPosition' in this list the specified
        // move-insertable 'value', and return an iterator providing modifiable
        // access to the newly inserted element.  'value' is left in a valid
        // but unspecified state.  If an exception is thrown (other than by the
        // copy constructor, move constructor, assignment operator, or move
        // assignment operator of 'value_type'), this method has no effect.
        // This method requires that the (template parameter) 'VALUE' be
        // 'move-insertable' into this list (see {Requirements on 'VALUE'}).
        // The behavior is undefined unless 'dstPosition' is an iterator in the
        // range '[cbegin() .. cend()]' (both endpoints included).

    iterator insert(const_iterator    dstPosition,
                    size_type         numElements,
                    const value_type& value);
        // Insert at the specified 'dstPosition' in this list the specified
        // 'numElements' copies of the specified 'value', and return an
        // iterator providing modifiable access to the first element in the
        // newly inserted sequence of elements.  This method requires that the
        // (template parameter) 'VALUE' be 'copy-insertable' into this list
        // (see {Requirements on 'VALUE'}).  The behavior is undefined unless
        // 'dstPosition' is an iterator in the range '[cbegin() .. cend()]'
        // (both endpoints included).

    template <class INPUT_ITERATOR>
    iterator insert(const_iterator dstPosition,
                    INPUT_ITERATOR first,
                    INPUT_ITERATOR last,
                    typename enable_if<
                        !is_arithmetic<INPUT_ITERATOR>::value &&
                        !is_enum<INPUT_ITERATOR>::value
                    >::type * = 0)
        // Insert at the specified 'dstPosition' in this list the values in the
        // range starting at the specified 'first' and ending immediately
        // before the specified 'last' iterators of the (template parameter)
        // type 'INPUT_ITERATOR', and return an iterator providing modifiable
        // access to the first element in the newly inserted sequence of
        // elements.  The (template parameter) type 'INPUT_ITERATOR' shall meet
        // the requirements of an input iterator defined in the C++11 standard
        // [input.iterators] providing access to values of a type convertible
        // to 'value_type', and 'value_type' must be 'emplace-constructible'
        // from '*i' into this list, where 'i' is a dereferenceable iterator in
        // the range '[first .. last)' (see {Requirements on 'VALUE'}).  The
        // behavior is undefined unless 'dstPosition' is an iterator in the
        // range '[cbegin() .. cend()]' (both endpoints included), and 'first'
        // and 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last'.
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // enable_if be in place inline.

        if (first == last) {
            return dstPosition.unconst();                             // RETURN
        }

        // The return value should indicate the first node inserted.  We can't
        // assume 'INPUT_ITERATOR' has a post-increment available.

        iterator ret = insert(dstPosition, *first);
        for (++first; first != last; ++first) {
            insert(dstPosition, *first);
        }

        return ret;
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    iterator insert(const_iterator                    dstPosition,
                    std::initializer_list<value_type> values);
        // Insert at the specified 'dstPosition' in this list the value of each
        // 'value_type' object in the specified 'values' initializer list, and
        // return an iterator providing modifiable access to the first element
        // in the newly inserted sequence of elements.  This method requires
        // that the (template parameter) 'VALUE' be 'copy-insertable' into this
        // list (see {Requirements on 'VALUE'}).  The behavior is undefined
        // unless 'dstPosition' is an iterator in the range
        // '[cbegin() .. cend()]' (both endpoints included).
#endif

                          // *** list operations ***

    void merge(list&                                other);
    void merge(BloombergLP::bslmf::MovableRef<list> other);
        // Merge the specified sorted 'other' list into this sorted list.  This
        // method has no effect if 'other' is this list; otherwise, 'other' is
        // left empty.  The behavior is undefined unless both 'other' and this
        // list are sorted in non-decreasing order according to the ordering
        // provided by 'operator<', and unless both 'other' and this list use
        // the same allocator.  'operator<' must define a strict weak ordering
        // per 'value_type' (see {Comparators and Strict Weak Ordering}).

    template <class COMPARE>
    void merge(list& other, COMPARE comparator);
    template <class COMPARE>
    void merge(BloombergLP::bslmf::MovableRef<list> other, COMPARE comparator);
        // Merge the specified sorted 'other' list into this sorted list, using
        // the specified binary 'comparator' predicate to order elements.  This
        // method has no effect if 'other' is this list; otherwise, 'other' is
        // left empty.  The behavior is undefined unless both 'other' and this
        // list are sorted in non-decreasing order according to the ordering
        // provided by 'comparator', and unless both 'other' and this list use
        // the same allocator.

    void remove(const value_type& value);
        // Erase all the elements having the specified 'value' from this list.

    template <class PREDICATE>
    void remove_if(PREDICATE predicate);
        // Remove and destroy all elements in this list for which the specified
        // unary 'predicate' returns 'true'.

    void reverse() BSLS_KEYWORD_NOEXCEPT;
        // Reverse the order of the elements in this list.

    void sort();
        // Sort this list in non-decreasing order according to the order
        // provided by 'operator<'.  'operator<' must provide a strict weak
        // ordering over 'value_type' (see {Comparators and Strict Weak
        // Ordering}).  The sort is stable, meaning that if
        // '!(a < b) && !(b < a)', then the ordering of elements 'a' and 'b' in
        // the sequence is preserved.

    template <class COMPARE>
    void sort(COMPARE comparator);
        // Sort this list in non-decreasing order according to the order
        // provided by the specified 'comparator' predicate.  'comparator' must
        // define a strict weak ordering over 'value_type' (see {Comparators
        // and Strict Weak Ordering}).  The sort is stable, meaning that if
        // '!comparator(a, b) && !comparator(b, a)', then the ordering of
        // elements 'a' and 'b' in the sequence is preserved.

    void splice(const_iterator                       dstPosition,
                list&                                src);
    void splice(const_iterator                       dstPosition,
                BloombergLP::bslmf::MovableRef<list> src);
        // Remove all elements of the specified 'src' list and insert them, in
        // the same order, in this list at the specified 'dstPosition'.  The
        // behavior is undefined unless 'src' is not this list, this list and
        // 'src' use the same allocator, and 'dstPosition' is in the range
        // '[begin() .. end()]' (note both endpoints included).

    void splice(const_iterator                       dstPosition,
                list&                                src,
                const_iterator                       srcNode);
    void splice(const_iterator                       dstPosition,
                BloombergLP::bslmf::MovableRef<list> src,
                const_iterator                       srcNode);
        // Remove the single element at the specified 'srcNode' from the
        // specified 'src' list, and insert it at the specified 'dstPosition'
        // in this list.  The behavior is undefined unless 'srcNode' refers to
        // a valid element in 'src', this list and 'src' use the same
        // allocator, and 'dstPosition' is in the range '[begin() .. end()]'
        // (note both endpoints included).  Note that 'src' and '*this' may be
        // the same list, in which case the element is moved to a (possibly)
        // new position in the list.

    void splice(const_iterator                       dstPosition,
                list&                                src,
                const_iterator                       first,
                const_iterator                       last);
    void splice(const_iterator                       dstPosition,
                BloombergLP::bslmf::MovableRef<list> src,
                const_iterator                       first,
                const_iterator                       last);
        // Remove the elements in the specified range '[first .. last)' from
        // the specified 'src' list, and insert them, in the same order, at the
        // specified 'dstPosition' in this list.  The behavior is undefined
        // unless '[first .. last)' represents a range of valid elements in
        // 'src', 'dstPosition' is not in the range '[first .. last)', this
        // list and 'src' use the same allocator, and 'dstPosition' is in the
        // range '[begin() .. end()]' (note both endpoints included).  Note
        // that 'src' and '*this' may be the same list, in which case an entire
        // sequence of nodes is moved to a (possibly) new position in this
        // list.

    void unique();
        // Erase from this list all but the first element of every consecutive
        // group of elements that have the same value.

    template <class EQ_PREDICATE>
    void unique(EQ_PREDICATE binaryPredicate);
        // Erase from this list all but the first element of every consecutive
        // group of elements for which the specified 'binaryPredicate' returns
        // 'true' for any two consecutive elements in the group.

                              // *** misc ***

    void swap(list& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                          AllocTraits::is_always_equal::value);
        // Exchange the value of this object with that of the specified 'other'
        // object; also exchange the allocator of this object with that of
        // 'other' if the (template parameter) type 'ALLOCATOR' has the
        // 'propagate_on_container_swap' trait, and do not modify either
        // allocator otherwise.  This method provides the no-throw
        // exception-safety guarantee.  This operation has 'O[1]' complexity if
        // either this object was created with the same allocator as 'other' or
        // 'ALLOCATOR' has the 'propagate_on_container_swap' trait; otherwise,
        // it has 'O[n + m]' complexity, where 'n' and 'm' are the number of
        // elements in this object and 'other', respectively.  Note that this
        // method's support for swapping objects created with different
        // allocators when 'ALLOCATOR' does not have the
        // 'propagate_on_container_swap' trait is a departure from the
        // C++ Standard.

    // ACCESSORS

                               // *** iterators ***

    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this list, or the 'end' iterator if this list is
        // empty.

    const_iterator end() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end (forward) iterator providing non-modifiable
        // access to this list.

    const_reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;
    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last element in this list, and the past-the-end reverse iterator if
        // this list is empty.

    const_reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;
    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end reverse iterator providing non-modifiable
        // access to this list.

                                  // *** size ***

    bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this list has no elements, and 'false' otherwise.

    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
        // Return an upper bound on the largest number of elements that this
        // list could possibly hold.  Note that the return value of this
        // function does not guarantee that this list can successfully grow
        // that large, or even close to that large without running out of
        // resources.

    size_type size() const BSLS_KEYWORD_NOEXCEPT;
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

    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return a copy of the allocator used for memory allocation by this
        // list.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template <
    class SIZE_TYPE,
    class VALUE,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<VALUE>,
    class = bsl::enable_if_t<
                            bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>,
    class = bsl::enable_if_t<
              bsl::is_convertible_v<
                 SIZE_TYPE,
                 typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type>>
    >
list(SIZE_TYPE, VALUE, ALLOC *) -> list<VALUE>;
    // Deduce the template parameter 'VALUE' from the corresponding parameter
    // supplied to the constructor of 'list'.  This deduction guide does not
    // participate unless the supplied allocator is convertible to
    // 'bsl::allocator<VALUE>'.

template <
    class INPUT_ITERATOR,
    class VALUE = typename
                   BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>
    >
list(INPUT_ITERATOR, INPUT_ITERATOR) -> list<VALUE>;
    // Deduce the template parameter 'VALUE' from the 'value_type' of the
    // iterators supplied to the constructor of 'list'.

template<
    class INPUT_ITERATOR,
    class ALLOCATOR,
    class VALUE = typename
                  BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>>
list(INPUT_ITERATOR, INPUT_ITERATOR, ALLOCATOR) -> list<VALUE, ALLOCATOR>;
    // Deduce the template parameter 'VALUE' from the 'value_type' of the
    // iterators supplied to the constructor of 'list'.  Deduce the template
    // parameter 'ALLOCATOR' from the allocator supplied to the constructor of
    // 'list'.  This deduction guide does not participate unless the supplied
    // allocator meets the requirements of a standard allocator.

template<
    class INPUT_ITERATOR,
    class ALLOC,
    class VALUE = typename
                  BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<VALUE>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
list(INPUT_ITERATOR, INPUT_ITERATOR, ALLOC *)
-> list<VALUE>;
    // Deduce the template parameter 'VALUE' from the value_type of the
    // iterators supplied to the constructor of 'list'.  This deduction guide
    // does not participate unless the specified 'ALLOC' is convertible to
    // 'bsl::allocator<CHAR_TYPE>'.

template<
    class VALUE,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<VALUE>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
list(std::initializer_list<VALUE>, ALLOC *)
-> list<VALUE>;
    // Deduce the template parameter 'VALUE' from the value_type of the
    // intializer_list supplied to the constructor of 'list'.  This deduction
    // guide does not participate unless the specified 'ALLOC' is convertible
    // to 'bsl::allocator<CHAR_TYPE>'.
#endif

// FREE OPERATORS
template <class VALUE, class ALLOCATOR>
bool operator==(const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'list' objects 'lhs' and 'rhs' have
    // the same value if they have the same number of elements, and each
    // element in the ordered sequence of elements of 'lhs' has the same value
    // as the corresponding element in the ordered sequence of elements of
    // 'rhs'.  This method requires that the (template parameter) type 'VALUE'
    // be 'equality-comparable' (see {Requirements on 'VALUE'}).

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

template <class VALUE, class ALLOCATOR>
bool operator!=(const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'list' objects 'lhs' and 'rhs'
    // do not have the same value if they do not have the same number of
    // elements, or some element in the ordered sequence of elements of 'lhs'
    // does not have the same value as the corresponding element in the ordered
    // sequence of elements of 'rhs'.  This method requires that the
    // (template parameter) type 'VALUE' be 'equality-comparable' (see
    // {Requirements on 'VALUE'}).

#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class VALUE, class ALLOCATOR>
BloombergLP::bslalg::SynthThreeWayUtil::Result<VALUE> operator<=>(
                                            const list<VALUE, ALLOCATOR>& lhs,
                                            const list<VALUE, ALLOCATOR>& rhs);
    // Perform a lexicographic three-way comparison of the specified 'lhs' and
    // the specified 'rhs' lists by using the comparison operators of 'VALUE'
    // on each element; return the result of that comparison.

#else

template <class VALUE, class ALLOCATOR>
bool operator< (const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' list is
    // lexicographically less than that of the specified 'rhs' list, and
    // 'false' otherwise.  Given iterators 'i' and 'j' over the respective
    // sequences '[lhs.begin() .. lhs.end())' and '[rhs.begin() .. rhs.end())',
    // the value of list 'lhs' is lexicographically less than that of list
    // 'rhs' if 'true == *i < *j' for the first pair of corresponding iterator
    // positions where '*i < *j' and '*j < *i' are not both 'false'.  If no
    // such corresponding iterator position exists, the value of 'lhs' is
    // lexicographically less than that of 'rhs' if 'lhs.size() < rhs.size()'.
    // This method requires that 'operator<', inducing a total order, be
    // defined for 'value_type'.

template <class VALUE, class ALLOCATOR>
bool operator> (const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' list is
    // lexicographically greater than that of the specified 'rhs' list, and
    // 'false' otherwise.  The value of list 'lhs' is lexicographically greater
    // than that of list 'rhs' if 'rhs' is lexicographically less than 'lhs'
    // (see 'operator<').  This method requires that 'operator<', inducing a
    // total order, be defined for 'value_type'.  Note that this operator
    // returns 'rhs < lhs'.

template <class VALUE, class ALLOCATOR>
bool operator<=(const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' list is
    // lexicographically less than or equal to that of the specified 'rhs'
    // list, and 'false' otherwise.  The value of list 'lhs' is
    // lexicographically less than or equal to that of list 'rhs' if 'rhs' is
    // not lexicographically less than 'lhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(rhs < lhs)'.

template <class VALUE, class ALLOCATOR>
bool operator>=(const list<VALUE, ALLOCATOR>& lhs,
                const list<VALUE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' list is
    // lexicographically greater than or equal to that of the specified 'rhs'
    // list, and 'false' otherwise.  The value of list 'lhs' is
    // lexicographically greater than or equal to that of list 'rhs' if 'lhs'
    // is not lexicographically less than 'rhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(lhs < rhs)'.

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class VALUE, class ALLOCATOR, class BDE_OTHER_TYPE>
typename list<VALUE, ALLOCATOR>::size_type
erase(list<VALUE, ALLOCATOR>& l, const BDE_OTHER_TYPE& value);
    // Erase all the elements in the specified list 'l' that compare equal to
    // the specified 'value'.  Return the number of elements erased.

template <class VALUE, class ALLOCATOR, class PREDICATE>
typename list<VALUE, ALLOCATOR>::size_type
erase_if(list<VALUE, ALLOCATOR>& l, PREDICATE predicate);
    // Erase all the elements in the specified list 'l' that satisfy the
    // specified predicate 'predicate'.  Return the number of elements erased.

template <class VALUE, class ALLOCATOR>
void swap(list<VALUE, ALLOCATOR>& a, list<VALUE, ALLOCATOR>& b)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                                                                   a.swap(b)));
    // Exchange the value of the specified 'a' object with that of the
    // specified 'b' object; also exchange the allocator of 'a' with that of
    // 'b' if the (template parameter) type 'ALLOCATOR' has the
    // 'propagate_on_container_swap' trait, and do not modify either allocator
    // otherwise.  This function provides the no-throw exception-safety
    // guarantee.  This operation has 'O[1]' complexity if either 'a' was
    // created with the same allocator as 'b' or 'ALLOCATOR' has the
    // 'propagate_on_container_swap' trait; otherwise, it has 'O[n + m]'
    // complexity, where 'n' and 'm' are the number of elements in 'a' and 'b',
    // respectively.  Note that this function's support for swapping objects
    // created with different allocators when 'ALLOCATOR' does not have the
    // 'propagate_on_container_swap' trait is a departure from the C++
    // Standard.

// ============================================================================
//                   INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                           // ------------------------
                           // class bsl::List_Iterator
                           // ------------------------

// PRIVATE ACCESSORS
template <class VALUE>
inline
typename List_Iterator<VALUE>::NcIter List_Iterator<VALUE>::unconst() const
{
    return NcIter(d_node_p);
}

// CREATORS
template <class VALUE>
inline
List_Iterator<VALUE>::List_Iterator()
: d_node_p()
{
}

template <class VALUE>
inline
List_Iterator<VALUE>::List_Iterator(Node *nodePtr)
: d_node_p(nodePtr)
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

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
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
#endif

                          // ------------------------------
                          // class List_AllocAndSizeWrapper
                          // ------------------------------

// CREATOR
template <class VALUE, class ALLOCATOR>
inline
List_AllocAndSizeWrapper<VALUE, ALLOCATOR>::List_AllocAndSizeWrapper(
                                               const NodeAlloc& basicAllocator,
                                               size_type        size)
: NodeAlloc(basicAllocator)
, d_size(size)
{
}

// MANIPULATORS
template <class VALUE, class ALLOCATOR>
inline
typename List_AllocAndSizeWrapper<VALUE, ALLOCATOR>::size_type&
List_AllocAndSizeWrapper<VALUE, ALLOCATOR>::size()
{
    return d_size;
}

// ACCESSORS
template <class VALUE, class ALLOCATOR>
inline
const typename List_AllocAndSizeWrapper<VALUE, ALLOCATOR>::size_type&
List_AllocAndSizeWrapper<VALUE, ALLOCATOR>::size() const
{
    return d_size;
}

                          // ----------------------
                          // class List_NodeProctor
                          // ----------------------

// CREATORS
template <class VALUE, class ALLOCATOR>
inline
List_NodeProctor<VALUE, ALLOCATOR>::List_NodeProctor(
                                               list<VALUE, ALLOCATOR> *listPtr,
                                               NodePtr                 nodePtr)
: d_list_p(listPtr)
, d_node_p(nodePtr)
{
    BSLS_ASSERT_SAFE(listPtr);
    BSLS_ASSERT_SAFE(nodePtr);
}

template <class VALUE, class ALLOCATOR>
inline
List_NodeProctor<VALUE, ALLOCATOR>::~List_NodeProctor()
{
    if (d_node_p) {
        d_list_p->freeNode(d_node_p);
    }
}

// MANIPULATORS
template <class VALUE, class ALLOCATOR>
inline
void List_NodeProctor<VALUE, ALLOCATOR>::release()
{
    d_node_p = 0;
}

                        // --------------------------
                        // class List_DefaultLessThan
                        // --------------------------

// ACCESSORS
template <class VALUE>
inline
bool List_DefaultLessThan<VALUE>::operator()(
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
    return d_alloc_and_size;  // implicit cast to base class
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
    BSLS_ASSERT_SAFE(node);

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
    NodePtr next = position.d_node_p;
    NodePtr prev = next->d_prev_p;
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
                                 COMPARE comparator)
{
    NodePtr pre = node1->d_prev_p;

    // The only possible throwing operation is the comparator.  Exception
    // neutrality is achieved by ensuring that this list is in a valid state,
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

        if (comparator(node2->d_value, node1->d_value)) {
            // 'node2' should come before 'node1'.

            // Find the end of the sequence of elements that belong before
            // node1 so that we can splice them all at once.

            NodePtr lastMove = node2;
            NodePtr next2    = node2->d_next_p;
            while (next2 != finish && comparator(next2->d_value,
                                                 node1->d_value)) {
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
list<VALUE, ALLOCATOR>::sizeRef() BSLS_KEYWORD_NOEXCEPT
{
    return d_alloc_and_size.size();
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
typename list<VALUE, ALLOCATOR>::NodePtr
list<VALUE, ALLOCATOR>::sortImp(NodePtr        *nodePtrPtr,
                                size_type       size,
                                const COMPARE&  comparator)
{
    BSLS_ASSERT(size > 0);

    NodePtr node1 = *nodePtrPtr;
    if (size < 2) {
        return node1->d_next_p;                                       // RETURN
    }

    size_type half = size / 2;

    NodePtr node2 = sortImp(&node1, half,        comparator);
    NodePtr next  = sortImp(&node2, size - half, comparator);

    *nodePtrPtr = mergeImp(node1, node2, next, comparator);
    return next;
}

// PRIVATE ACCESSORS
template <class VALUE, class ALLOCATOR>
inline
const typename list<VALUE, ALLOCATOR>::NodeAlloc&
                                   list<VALUE, ALLOCATOR>::allocatorImp() const
{
    return d_alloc_and_size;  // implicit cast to base class
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
list<VALUE, ALLOCATOR>::sizeRef() const BSLS_KEYWORD_NOEXCEPT
{
    return d_alloc_and_size.size();
}

// CREATORS
template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list()
: d_sentinel()
, d_alloc_and_size(ALLOCATOR(), 0)
{
    BSLMF_ASSERT((bsl::is_same<size_type,
                                     typename AllocTraits::size_type>::value));
    BSLMF_ASSERT((bsl::is_same<difference_type,
                               typename AllocTraits::difference_type>::value));
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
list<VALUE, ALLOCATOR>::list(size_type numElements)
: d_sentinel()
, d_alloc_and_size(ALLOCATOR(), size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(this->allocatorImp());

    // Default-construct (value-initialize) 'n' elements into 'tmp'.  'tmp's
    // destructor will clean up if an exception is thrown.

    iterator pos = tmp.end();
    for (size_type i = 0; i < numElements; ++i) {
        tmp.emplace(pos);
    }

    quickSwap(&tmp);  // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(size_type        numElements,
                             const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(this->allocatorImp());

    // Default-construct (value-initialize) 'n' elements into 'tmp'.  'tmp's
    // destructor will clean up if an exception is thrown.

    const_iterator pos = tmp.cend();
    for (size_type i = 0; i < numElements; ++i) {
        tmp.emplace(pos);
    }

    quickSwap(&tmp);  // Leave 'tmp' in an invalid but destructible state.
}

template <class VALUE, class ALLOCATOR>
list<VALUE, ALLOCATOR>::list(size_type        numElements,
                             const VALUE&     value,
                             const ALLOCATOR& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list tmp(this->allocatorImp());
    tmp.insert(tmp.cbegin(), numElements, value);    // 'tmp's destructor will
                                                     // clean up on throw.
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
list<VALUE, ALLOCATOR>::list(const list&                        original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
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
                 BloombergLP::bslmf::MovableRef<list>           original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_sentinel()
, d_alloc_and_size(basicAllocator, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).

    list& lvalue = original;
    if (this->allocatorImp() == lvalue.allocatorImp()) {
        // An rvalue must be left in a valid state after a move.

        createSentinel();      // '*this' is now in a valid state.
        quickSwap(&lvalue);
    }
    else {
        // different allocators, must copy

        list tmp(this->allocatorImp());

        // Avoid relying on VALUE's copy c'tor unless no move c'tor is
        // available.

        NodePtr endPtr = lvalue.d_sentinel;
        for (NodePtr p = lvalue.headNode(); endPtr != p;  p = p->d_next_p) {
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
    // temporary list, 'tmp', with the specified data.  If an exception is
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
        // requires that the allocator type not throw on copy or assign) as
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
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(AllocTraits::is_always_equal::value)
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
        // doing 'quickSwap(&lvalue)' has a problem in that it could leave
        // 'rhs' in an invalid state, since if 'this->createSentinel()' were
        // called after the tearing down to render '*this' to a valid value,
        // 'createSentinel' might throw, leaving '*this' in an invalid state.

        // Swap everything, including the allocator (here we are relying on the
        // C++11 standard, which requires that the allocator type not throw on
        // copy or assign).

        list other(MoveUtil::move(lvalue));

        using std::swap;

        swap(allocatorImp(), other.allocatorImp()); // won't throw
        swap(d_sentinel,     other.d_sentinel);     // swap of pointer type
        swap(sizeRef(),      other.sizeRef());      // swap of fundamental type
    }
    else {
        // Unequal allocators and the allocator of the destination is to remain
        // unchanged.  Copy using 'move', which will use copy functions where
        // 'value_type' doesn't support moving.  Note that if this throws part
        // way through, both '*this' and 'rhs' may be left changed.

        NodePtr              dstPtr    = this->headNode();
        const const_iterator dstEnd    = this->cend();
        const NodePtr        dstEndPtr = dstEnd.d_node_p;

        NodePtr              srcPtr    = lvalue.headNode();
        const NodePtr        srcEndPtr = lvalue.d_sentinel;

        for (; srcEndPtr != srcPtr && dstEndPtr != dstPtr;
                        srcPtr = srcPtr->d_next_p, dstPtr = dstPtr->d_next_p) {
            dstPtr->d_value = MoveUtil::move(srcPtr->d_value);
        }

        erase(const_iterator(dstPtr), dstEnd);

        for (; srcEndPtr != srcPtr; srcPtr = srcPtr->d_next_p) {
            emplace(dstEnd, MoveUtil::move(srcPtr->d_value));
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
void list<VALUE, ALLOCATOR>::assign(size_type numElements, const VALUE& value)
{
    NodePtr              dst_p    = this->headNode();
    const const_iterator dstEnd   = this->cend();
    const NodePtr        dstEnd_p = dstEnd.d_node_p;

    for (; 0 < numElements && dstEnd_p != dst_p;
                                      --numElements, dst_p = dst_p->d_next_p) {
        dst_p->d_value = value;
    }

    erase(const_iterator(dst_p), dstEnd);

    for (; 0 < numElements; --numElements) {
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
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return iterator(headNode());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::iterator list<VALUE, ALLOCATOR>::end()
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_sentinel);
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::reverse_iterator
list<VALUE, ALLOCATOR>::rbegin() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::reverse_iterator
list<VALUE, ALLOCATOR>::rend() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(begin());
}

                            // *** modify size ***

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::clear() BSLS_KEYWORD_NOEXCEPT
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
void list<VALUE, ALLOCATOR>::resize(size_type newSize)
{
    if (newSize > sizeRef()) {
        const_iterator ce = cend();
        do {
            emplace(ce);
        } while (newSize > sizeRef());
    }
    else {
        NodePtr e = d_sentinel;
        NodePtr p = e->d_prev_p;
        for (size_type d = sizeRef() - newSize; d > 0; --d) {
            NodePtr condemned = p;
            p = p->d_prev_p;
            deleteNode(condemned);
        }
        linkNodes(p, e);
        sizeRef() = newSize;
    }
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::resize(size_type newSize, const VALUE& value)
{
    if (newSize > sizeRef()) {
        const_iterator ce = cend();
        do {
            emplace(ce, value);
        } while (newSize > sizeRef());
    }
    else {
        NodePtr e = d_sentinel;
        NodePtr p = e->d_prev_p;
        for (size_type d = sizeRef() - newSize; d > 0; --d) {
            NodePtr condemned = p;
            p = p->d_prev_p;
            deleteNode(condemned);
        }
        linkNodes(p, e);
        sizeRef() = newSize;
    }
}

                                // element access:

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::reference
list<VALUE, ALLOCATOR>::back()
{
    BSLS_ASSERT_SAFE(sizeRef() > 0);

    return d_sentinel->d_prev_p->d_value;
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::reference
list<VALUE, ALLOCATOR>::front()
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

    NodePtr  condemned = position.d_node_p;
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
typename list<VALUE, ALLOCATOR>::reference
list<VALUE, ALLOCATOR>::emplace_back(ARGS&&... arguments)
{
    emplace(cend(), BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    return back();
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE, class ALLOCATOR>
template <class... ARGS>
inline
typename list<VALUE, ALLOCATOR>::reference
list<VALUE, ALLOCATOR>::emplace_front(ARGS&&... arguments)
{
    emplace(cbegin(), BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    return front();
}
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
list<VALUE, ALLOCATOR>::emplace(const_iterator position, ARGS&&... arguments)
{
    NodePtr p = allocateNode();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocatorImp(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           BSLS_COMPILERFEATURES_FORWARD(ARGS, arguments)...);
    proctor.release();
    return insertNode(position, p);
}
#endif

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(const_iterator dstPosition, const VALUE& value)
{
    return emplace(dstPosition, value);
}

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(
                             const_iterator                        dstPosition,
                             BloombergLP::bslmf::MovableRef<VALUE> value)
{
    return emplace(dstPosition, MoveUtil::move(value));
}

template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(const_iterator dstPosition,
                               size_type      numElements,
                               const VALUE&   value)
{
    if (0 == numElements) {
        return dstPosition.unconst();                                 // RETURN
    }

    // Remember the position of the first node inserted before 'dstPosition'.

    iterator ret = emplace(dstPosition, value);

    // And put the rest of the nodes after it.

    for (--numElements; numElements > 0; --numElements) {
        emplace(dstPosition, value);
    }

    return ret;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE, class ALLOCATOR>
typename list<VALUE, ALLOCATOR>::iterator
list<VALUE, ALLOCATOR>::insert(const_iterator               dstPosition,
                               std::initializer_list<VALUE> values)
{
    return insert(dstPosition, values.begin(), values.end());
}
#endif

                          // *** list operations ***

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::merge(list& other)
{
    BSLS_ASSERT_SAFE(this->allocatorImp() == other.allocatorImp());

    merge(other, DefaultLessThan());
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::merge(BloombergLP::bslmf::MovableRef<list> other)
{
    list& lvalue = other;

    BSLS_ASSERT_SAFE(this->allocatorImp() == lvalue.allocatorImp());

    merge(lvalue, DefaultLessThan());
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
void list<VALUE, ALLOCATOR>::merge(list& other, COMPARE comparator)
{
    if (&other == this) {
        return;                                                       // RETURN
    }

    BSLS_ASSERT(this->allocatorImp() == other.allocatorImp());

    if (other.empty()) {
        // This is an important special case to avoid pointing to sentinel.

        return;                                                       // RETURN
    }

    // Splice 'other' to the end of '*this', but remember the first node of the
    // appended sequence.

    NodePtr xfirst = other.d_sentinel->d_next_p;
    splice(end(), other);

    // Call 'mergeImp' with a pointer to the first node of the original list, a
    // pointer to the first node of 'other' (which also ends the original
    // list), and a pointer to the sentinel (which now ends 'other').

    mergeImp(d_sentinel->d_next_p, xfirst, d_sentinel, comparator);
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
inline
void list<VALUE, ALLOCATOR>::merge(
                               BloombergLP::bslmf::MovableRef<list> other,
                               COMPARE                              comparator)
{
    list& lvalue = other;

    BSLS_ASSERT_SAFE(this->allocatorImp() == lvalue.allocatorImp());

    merge(lvalue, comparator);
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::remove(const VALUE& value)
{
    const const_iterator e = cend();
    for (const_iterator i = cbegin(); e != i; ) {
        // Standard says to use 'operator==', not 'std::equal_to'.

        if (value == *i) {
            i = erase(i);
        }
        else {
            ++i;
        }
    }
}

template <class VALUE, class ALLOCATOR>
template <class PREDICATE>
void list<VALUE, ALLOCATOR>::remove_if(PREDICATE predicate)
{
    const iterator e = end();
    for (iterator i = begin(); e != i; ) {
        if (predicate(*i)) {
            i = erase(i);
        }
        else {
            ++i;
        }
    }
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::reverse() BSLS_KEYWORD_NOEXCEPT
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
    sort(DefaultLessThan());
}

template <class VALUE, class ALLOCATOR>
template <class COMPARE>
void list<VALUE, ALLOCATOR>::sort(COMPARE comparator)
{
    if (sizeRef() < 2) {
        return;                                                       // RETURN
    }
    NodePtr node1 = d_sentinel->d_next_p;
    sortImp(&node1, size(), comparator);
}

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::splice(const_iterator dstPosition, list& src)
{
    BSLS_ASSERT(allocatorImp() == src.allocatorImp());
    BSLS_ASSERT(&src != this);

    if (src.empty()) {
        return;                                                       // RETURN
    }

    NodePtr   pPos   = dstPosition.d_node_p;
    NodePtr   pFirst = src.headNode();
    NodePtr   pLast  = src.d_sentinel->d_prev_p;
    size_type n      = src.sizeRef();

    // Splice contents out of 'src'.

    linkNodes(src.d_sentinel, src.d_sentinel);
    src.sizeRef() = 0;

    // Splice contents into '*this'.

    linkNodes(pPos->d_prev_p, pFirst);
    linkNodes(pLast,          pPos);
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

    NodePtr pPos          = dstPosition.d_node_p;
    NodePtr pSrcNode      = srcNode.d_node_p;
    NodePtr pAfterSrcNode = pSrcNode->d_next_p;

    if (pPos == pSrcNode || pPos == pAfterSrcNode) {
        return;                                                       // RETURN
    }

    // Splice contents out of 'src'.

    linkNodes(pSrcNode->d_prev_p, pAfterSrcNode);
    --src.sizeRef();

    // Splice contents into '*this'.

    linkNodes(pPos->d_prev_p, pSrcNode);
    linkNodes(pSrcNode,       pPos);
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
                                    const_iterator first,
                                    const_iterator last)
{
    BSLS_ASSERT(allocatorImp() == src.allocatorImp());

    size_type n = bsl::distance(first, last);

    if (0 == n) {
        return;                                                       // RETURN
    }

    NodePtr pPos     = dstPosition.d_node_p;
    NodePtr pFirst   = first.d_node_p;
    NodePtr pLast    = last.d_node_p;
    NodePtr pSrcLast = pLast->d_prev_p;

    // Splice contents out of 'src'.

    linkNodes(pFirst->d_prev_p, pLast);
    src.sizeRef() -= n;

    // Splice contents into '*this'.

    linkNodes(pPos->d_prev_p, pFirst);
    linkNodes(pSrcLast,       pPos);
    sizeRef() += n;
}

template <class VALUE, class ALLOCATOR>
inline
void list<VALUE, ALLOCATOR>::splice(
                              const_iterator                       dstPosition,
                              BloombergLP::bslmf::MovableRef<list> src,
                              const_iterator                       first,
                              const_iterator                       last)
{
    splice(dstPosition, MoveUtil::access(src), first, last);
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
template <class EQ_PREDICATE>
void list<VALUE, ALLOCATOR>::unique(EQ_PREDICATE binaryPredicate)
{
    if (size() < 2) {
        return;                                                       // RETURN
    }

    iterator i = begin();
    iterator e = end();
    while (i != e) {
        reference match = *i++;
        while (i != e && binaryPredicate(*i, match)) {
            i = erase(i);
        }
    }
}

                              // *** misc ***

template <class VALUE, class ALLOCATOR>
void list<VALUE, ALLOCATOR>::swap(list& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(AllocTraits::is_always_equal::value)
{
    // C++11 behavior for member 'swap': undefined for unequal allocators.
    // BSLS_ASSERT(allocatorImp() == other.allocatorImp());

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

        // Create copies using the move constructor, then swap both containers
        // with them.  Note that if no move constructor exists, but a copy
        // constructor does, the copy constructor will be used.

        // Also note that if either of these copies throws, it could leave the
        // two containers in a changed state.  They are, however, guaranteed to
        // be left in valid state.

        list toOtherCopy(MoveUtil::move(*this), other.allocatorImp());
        list toThisCopy( MoveUtil::move(other), this->allocatorImp());

        toOtherCopy.quickSwap(&other);
        toThisCopy .quickSwap(this);
    }
}

// ACCESSORS

                               // *** iterators ***

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(headNode());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_sentinel);
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return begin();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_iterator
list<VALUE, ALLOCATOR>::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return end();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return rbegin();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return rend();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reverse_iterator
list<VALUE, ALLOCATOR>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

                                  // *** size ***

template <class VALUE, class ALLOCATOR>
inline
bool list<VALUE, ALLOCATOR>::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return 0 == sizeRef();
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::size_type
list<VALUE, ALLOCATOR>::max_size() const BSLS_KEYWORD_NOEXCEPT
{
    return AllocTraits::max_size(allocatorImp());
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::size_type list<VALUE, ALLOCATOR>::size() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return sizeRef();
}

                           // *** element access ***

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reference
list<VALUE, ALLOCATOR>::back() const
{
    BSLS_ASSERT_SAFE(sizeRef() > 0);

    return d_sentinel->d_prev_p->d_value;
}

template <class VALUE, class ALLOCATOR>
inline
typename list<VALUE, ALLOCATOR>::const_reference
list<VALUE, ALLOCATOR>::front() const
{
    BSLS_ASSERT_SAFE(sizeRef() > 0);

    return headNode()->d_value;
}

                                // *** misc ***

template <class VALUE, class ALLOCATOR>
inline
ALLOCATOR list<VALUE, ALLOCATOR>::get_allocator() const BSLS_KEYWORD_NOEXCEPT
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

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

template <class VALUE, class ALLOCATOR>
inline
bool bsl::operator!=(const list<VALUE, ALLOCATOR>& lhs,
                     const list<VALUE, ALLOCATOR>& rhs)
{
    return ! (lhs == rhs);
}

#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class VALUE, class ALLOCATOR>
inline
BloombergLP::bslalg::SynthThreeWayUtil::Result<VALUE> bsl::operator<=>(
                                             const list<VALUE, ALLOCATOR>& lhs,
                                             const list<VALUE, ALLOCATOR>& rhs)
{
    return bsl::lexicographical_compare_three_way(
                              lhs.begin(),
                              lhs.end(),
                              rhs.begin(),
                              rhs.end(),
                              BloombergLP::bslalg::SynthThreeWayUtil::compare);
}

#else

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
bool bsl::operator<=(const list<VALUE, ALLOCATOR>& lhs,
                     const list<VALUE, ALLOCATOR>& rhs)
{
    return !(rhs < lhs);
}

template <class VALUE, class ALLOCATOR>
inline
bool bsl::operator>=(const list<VALUE, ALLOCATOR>& lhs,
                     const list<VALUE, ALLOCATOR>& rhs)
{
    return !(lhs < rhs);
}

#endif // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class VALUE, class ALLOCATOR, class BDE_OTHER_TYPE>
inline typename bsl::list<VALUE, ALLOCATOR>::size_type
bsl::erase(list<VALUE, ALLOCATOR>& l, const BDE_OTHER_TYPE& value)
{
    // We could use the erase/remove idiom here like we do in the other
    // sequence containers, but this is more efficient, since we just unlink
    // and delete nodes from the list.
    typename list<VALUE, ALLOCATOR>::size_type oldSize = l.size();
    for (typename list<VALUE, ALLOCATOR>::iterator it = l.begin();
                                                                it != l.end();)
    {
        if (value == *it) {
            it = l.erase(it);
        }
        else {
            ++it;
        }
    }
    return oldSize - l.size();
}

template <class VALUE, class ALLOCATOR, class PREDICATE>
inline typename bsl::list<VALUE, ALLOCATOR>::size_type
bsl::erase_if(list<VALUE, ALLOCATOR>& l, PREDICATE predicate)
{
    return BloombergLP::bslstl::AlgorithmUtil::containerEraseIf(l, predicate);
}

template <class VALUE, class ALLOCATOR>
inline
void bsl::swap(list<VALUE, ALLOCATOR>& a, list<VALUE, ALLOCATOR>& b)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                                                                    a.swap(b)))
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *sequence* containers:
//: o A sequence container defines STL iterators.
//: o A sequence container uses 'bslma' allocators if the (template parameter)
//:   type 'ALLOCATOR' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class VALUE, class ALLOCATOR>
struct HasStlIterators<bsl::list<VALUE, ALLOCATOR> >
    : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class VALUE, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::list<VALUE, ALLOCATOR> >
    : bsl::is_convertible<Allocator*, ALLOCATOR>
{};

}  // close namespace bslma

namespace bslmf {

// A list is bitwise movable if its allocator is bitwise movable.

template <class VALUE, class ALLOCATOR>
struct IsBitwiseMoveable<bsl::list<VALUE, ALLOCATOR> >
    : BloombergLP::bslmf::IsBitwiseMoveable<ALLOCATOR>
{};

}  // close namespace bslmf
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
