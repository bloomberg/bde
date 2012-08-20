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
//@DESCRIPTION: The 'list' class is a homogeneous container class into which
// elements of a single type can be inserted, removed, and reordered.  All
// operations involving single elements are constant-time, including insertion
// and removal of elements anywhere in the list.  Operations that do not
// change the number of elements are performed without calling constructors,
// destructors, swap, or assignment on the individual elements.  (I.e., they
// are performed by pointer-manipulation alone.)  A 'list' does not provide
// random access to its elements; although access to the first and last
// element of a 'list' is constant-time, other elements can be accessed only by
// traversing the list (forwards or backwards) from the beginning or end.
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
// rvalue references, 'initializer_lists', 'emplace', or operations taking a
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
///Operations
///----------
// 'list' supports the following operations for lists 'a' and 'b' containing
// 'n' elements of type 'T', where 'i1' and 'i2' are input iterators, 'p',
// 'q1' and 'q2' are list iterators belonging to 'a', 's1' and 's2' are
// iterators belonging to 'b', 't' is an expression of type 'T', 'args...' is
// a variadic list of (up to 5) arguments for constructing a 'T' object,
// 'comp' is a binary predicate implementing a strict-weak ordering, 'pred' is
// a unary predicate, and 'binary_pred' is a binary predicate, and 'm' is an
// (STL-style) allocator.
//..
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | list<T> a;    (default construction)               | O(1)               |
//  | list<T> a(m);                                      |                    |
//  +----------------------------------------------------+--------------------+
//  | list<T> a(b); (copy construction)                  | O(n)               |
//  | list<T> a(b, m);                                   |                    |
//  +----------------------------------------------------+--------------------+
//  | list<T> a(n);                                      | O(n)               |
//  | list<T> a(n, t);                                   |                    |
//  | list<T> a(n, t, m);                                |                    |
//  +----------------------------------------------------+--------------------+
//  | list<T> a(i1, i2);                                 | O(distance(i1,i2)) |
//  | list<T> a(i1, i2, m);                              |                    |
//  +----------------------------------------------------+--------------------+
//  | a.~list<T>(); (destruction)                        | O(n)               |
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
//  | a.swap(b), swap(a,b)                               | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | get_allocator()                                    | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.emplace(p, args...)                              | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p, t)                                     | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p, n, t)                                  | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p, i1, i2)                                | O(distance(i1,i2)) |
//  +----------------------------------------------------+--------------------+
//  | a.erase(q1)                                        | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.erase(q1, q2)                                    | O(distance(q1,q2)) |
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.assign(i1,i2)                                    | O(distance(i1,i2)) |
//  +----------------------------------------------------+--------------------+
//  | a.assign(n,t)                                      | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.front(), a.back()                                | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.emplace_front(args...), a.emplace_back(args...)  | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.push_front(t), a.push_back(t)                    | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.pop_front(), a.pop_back()                        | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.resize(n), a.resize(n, t)                        | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, b), a.splice(p, b, s1)                 | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | a.splice(p, b, s1, s2)                             | O(distance(s1,s2)) |
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
// the iterator is a 'const_iterator' or the reference is a reference-to-const.
//
///Usage
///-----
// This section illustates intended usage of this component
//
///Usage 1: Using push_back, erase, and iteration
/// - - - - - - - - - - - - - - - - - - - - - - -
// An observatory needs to analyze the results of a sky survey.  The raw data
// is a text file of star observations where each star is represented by a
// tuple of three numbers: (x, y, b), where x and y represent the angular
// coordinates of the star in the sky and b represents its brightness on a
// scale of 0 to 100.  The class 'Star' encapsulates a single tuple, and
// provides accessors functions 'x', 'y', and 'brightness', file I/O functions
// 'read' and 'write', and free operators '==', '!=', and '<':
//..
//  #include <cstdio>
//  using namespace std;
//
//  class Star {
//      // Representation of a star as seen through a digital telescope
//      double d_x, d_y;     // Coordinates
//      int    d_brightness; // Brightness on a scale of 0 to 100
//
//  public:
//      Star() : d_x(0), d_y(0), d_brightness(0) { }
//      Star(double x, double y, int b) : d_x(x), d_y(y), d_brightness(b) { }
//
//      // Compiler-generated copy construction, assignment, and destructor
//      // Star(const Star&) = default;
//      // Star& operator=(const Star&) = default;
//      // ~Star() = default;
//
//      double x()       const { return d_x; }
//      double y()       const { return d_y; }
//      int brightness() const { return d_brightness; }
//
//      bool read(FILE *input);
//          // Read x, y, and brightness from the specified 'input' file.
//          // Return 'true' if the read succeeded and 'false' otherwise.
//
//      void write(FILE *output) const;
//          // Write x, y, and brightness to the specified 'output' file
//          // followed by a newline.
//  };
//
//  bool operator==(const Star& a, const Star& b);
//  bool operator!=(const Star& a, const Star& b);
//  bool operator<(const Star& a, const Star& b);
//..
// The first task is to read a file of data points and append each onto a
// list.  The stars are stored in the data file in ascending sorted order by x
// and y coordinates.
//..
//  #include <bslstl_list.h>
//  using namespace bsl;
//
//  void readData(list<Star> *lst, FILE *input)
//  {
//      Star s;
//      while (s.read(input)) {
//          lst->push_back(s);
//      }
//  }
//..
// We are interested in only the brightest starts.  The 'filter' function is
// responsible for removing stars with a brightness of less than 75 from the
// data set.  It does this by iterating over the list and erasing any element
// that does not pass the filter.  The list object features a fast 'erase'
// member function.  The return value of 'erase' is an iterator
// to the element immediately following the erased element:
//..
//  void filter(list<Star> *lst)
//  {
//      static const int threshold = 75;
//
//      list<Star>::iterator i = lst->begin();
//      while (i != lst->end()) {
//          if (i->brightness() < threshold) {
//              i = lst->erase(i); // Erase and advance to next element.
//          }
//          else {
//              ++i;  // Advance to next element without erasing
//          }
//      }
//  }
//..
// Our first program will read a data file, filter out the dim stars, and
// count the ones left in the list.  Our test data set has been selected such
// that there are 10 stars in the set, of which 4 are sufficiently bright as
// to pass our filter.
//..
//  int main()
//  {
//      FILE *input = fopen("star_data1.txt", "r");
//      assert(input);
//
//      list<Star> lst;
//      assert(lst.empty());
//
//      readData(&lst, input);
//      assert(10 == lst.size());
//      fclose(input);
//
//      filter(&lst);
//      assert(4 == lst.size());
//
//      if (veryVerbose) {
//          for (list<Star>::const_iterator i = lst.begin(); i != lst.end();
//               ++i) {
//              i->write(stdout);
//          }
//      }
//
//      return 0;
//  }
//..
///Usage 2: Using insert, splice, merge, and unique
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Now we want to combine the results from two star surveys into a single
// list.  We begin by reading both lists and filtering them.  (Our test data is
// selected so that the second data file contains 8 starts of which 3 are
// sufficiently bright as to pass our filter:
//..
//  int main()
//  {
//      FILE *input = fopen("star_data1.txt", "r");
//      assert(input);
//
//      list<Star> lst1;
//      assert(lst1.empty());
//
//      readData(&lst1, input);
//      assert(10 == lst1.size());
//      fclose(input);
//
//      input = fopen("star_data2.txt", "r");
//      assert(input);
//
//      list<Star> lst2;
//      assert(lst2.empty());
//
//      readData(&lst2, input);
//      assert(8 == lst2.size());
//      fclose(input);
//
//      filter(&lst1);
//      assert(4 == lst1.size());
//
//      filter(&lst2);
//      assert(3 == lst2.size());
//..
// One way to combine the lists is simply to insert the second list at the end
// of the first:
//..
//      list<Star> tmp1(lst1);  // Make a copy of the first list
//      list<Star> tmp2(lst2);  // Make a copy of the second list
//      tmp1.insert(tmp1.end(), tmp2.begin(), tmp2.end());
//      assert(7 == tmp1.size());  // Combination of lst1 and lst2 sizes
//      assert(3 == tmp2.size());  // Unchanged
//..
// The above insert function appends a copy of each element in 'tmp2' onto the
// end of 'tmp1'.  This copy is unnecessary because we have no need for 'tmp2'
// after the lists have been combined.  A faster and less-memory-intensive
// technique is to use the 'splice' function, which *moves* rather than
// *copies* elements from one list to another:
//..
//      tmp1 = lst1;
//      tmp2 = lst2;
//      tmp1.splice(tmp1.begin(), tmp2);
//      assert(7 == tmp1.size());  // Combination of lst1 and lst2 sizes
//      assert(0 == tmp2.size());  // Emptied by the splice
//..
// Unfortunately, while the original lists were sorted in ascending order
// (because the data files were originally sorted), the combined list is no
// longer sorted.  One way to fix it is simply to sort it using the 'sort'
// member function:
//..
//      tmp1.sort();
//..
// However, the best approach is take advantage of the fact that the lists
// were originally sorted, using the 'merge' function:
//..
//      lst1.merge(lst2);          // Merge lst2 into lst1
//      assert(7 == lst1.size());  // Combined size
//      assert(0 == lst2.size());  // lst2 was emptied by the merge
//..
// Since the two star surveys may overlap, we want to eliminate duplicates.
// We accomplish this task using the 'unique' member function:
//..
//      lst1.unique();
//      assert(6 == lst1.size());
//..
// Finally, we print the result:
//..
//      if (veryVerbose) {
//          for (list<Star>::const_iterator i = lst1.begin(); i != lst1.end();
//               ++i)
//          {
//              i->write(stdout);
//          }
//      }
//
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
//  bool operator==(const Star& a, const Star& b)
//  {
//      return a.x() == b.x()
//          && a.y() == b.y()
//          && a.brightness() == b.brightness();
//  }
//
//  bool operator!=(const Star& a, const Star& b)
//  {
//      return ! (a == b);
//  }
//
//  bool operator<(const Star& a, const Star& b)
//  {
//      if (a.x() < b.x())
//          return true;
//      else if (b.x() < a.x())
//          return false;
//      else if (a.y() < b.y())
//          return true;
//      else if (b.y() < a.y())
//          return true;
//      else
//          return a.brightness() < b.brightness();
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

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLALG_RANGECOMPARE
#include <bslalg_rangecompare.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

namespace bsl {

                        // =====================
                        // struct bsl::List_Node
                        // =====================

template <class TYPE, class ALLOCATOR> class list;

template <class TYPE>
struct List_node {
    // PRIVATE CLASS TEMPLATE.  For use only by 'bsl::list' implementation.
    // An instance of 'List_node<T>' is a single node in a doubly-linked list
    // used to implement 'bsl::list<T,A>', for a given element type 'T' and
    // allocator type 'A'.  Note that an instantiation of this class for by a
    // given 'bsl::list' is independent of the allocator type.

    // PUBLIC TYPES

    // In C++11 NodePtr would generalized as follows:
    // typedef pointer_traits<VoidPtr>::template rebind<List_node> NodePtr;

    typedef List_node* NodePtr;

    // PUBLIC DATA
    NodePtr d_prev;  // Pointer to the previous node in the list
    NodePtr d_next;  // Pointer to the next node in the list
    TYPE    d_value; // List element

    // MANIPULATORS
    void init();
        // Value-initialize the 'd_prev' and 'd_next' members of this node,
        // but do not touch 'd_value'. The behavior is undefined unless 'this'
        // points to allocated (uninitialized) storage'.  Note: 'd_value'
        // continues to hold uninitialized storage after this call.

    void deinit();
        // De-initialize the 'd_prev' and 'd_next' members of this node but do
        // not touch 'd_value'.  After calling 'deinit', 'this' points to
        // allocated (uninitialized) storage.  The behavior is undefiled
        // unless 'd_prev' and 'd_next' have valid values and 'd_value' holds
        // allocated (uninitialized) storage.

  private:
    // PRIVATE CREATORS

    // The following special functions are not defined because a List_node
    // should never be constructed, destructed, or assigned.  Rather, a
    // List_node should be initialized using the 'init' function and destroyed
    // using the 'deinit' function.  The 'd_value' member should be separately
    // constructed and destroyed using an allocator's 'construct' and
    // 'destroy' methods.
    List_node();                              // Declared but not defined
    List_node(const List_node&);              // Declared but not defined
    List_node& operator=(const List_node&);   // Declared but not defined
    ~List_node();                             // Declared but not defined
};

                        // ========================
                        // class bsl::List_iterator
                        // ========================

template <class TYPE, class NODEPTR, class DIFFTYPE>
class List_iterator
{
    // Implementation of std::list::iterator

    // FRIENDS
    template <class T, class A> friend class list;
    friend class List_iterator<const TYPE, NODEPTR, DIFFTYPE>;

    template <class T1, class T2, class NODEP, class DIFFT>
    friend bool operator==(List_iterator<T1, NODEP, DIFFT> a,
                           List_iterator<T2, NODEP, DIFFT> b);

    // PRIVATE TYPES
    typedef typename BloombergLP::bslmf::RemoveCvq<TYPE>::Type  NcType;
    typedef List_iterator<NcType, NODEPTR, DIFFTYPE>            NcIter;
    typedef List_node<NcType>                                   Node;

    // DATA
    NODEPTR d_nodeptr;

    // PRIVATE FUNCTIONS
    NcIter unconst() const { return NcIter(d_nodeptr); }

  public:
    // PUBLIC TYPES
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef NcType                          value_type;
    typedef DIFFTYPE                        difference_type;
    typedef TYPE*                           pointer;
    typedef TYPE&                           reference;

    // CREATORS
    List_iterator();
        // Construct a singular iterator (i.e., one that cannot be incremented,
        // decremented, or dereferenced.

    explicit List_iterator(NODEPTR p);
        // Construct an iterator that references the value pointed to by the
        // specified pointer 'p'.

    List_iterator(const NcIter& other);
        // Construct an iterator to 'TYPE' from the corresponding iterator to
        // non-const 'TYPE'.  If 'TYPE' is not const-qualified, then this
        // constructor becomes the copy constructor.  Otherwise, the copy
        // constructor is implicitly generated.

    // Compiler-generated copy constructor, destructor, and assignment
    // operators:
    // List_iterator(const List_iterator&); // Not defaulted
    //! ~List_iterator() = default;
    //! List_iterator& operator=(const List_iterator&) = default;

    // MANIPULATORS
    List_iterator& operator++();
        // Advance this iterator to the next element in the list.
        // The behavior is undefined unless this iterator is in the range
        // [begin(), end()) for some list (i.e., the iterator is not singular,
        // is not end() and has not be invalidated).

    List_iterator& operator--();
        // Move this iterator to the previous element in the list.
        // The behavior is undefined unless this iterator is in the range
        // (begin(), end()] for some list (i.e., the iterator is not singular,
        // is not begin() and has not be invalidated).

    List_iterator operator++(int);
        // Advance this iterator to the next element in the list and return
        // its previous value.  The behavior is undefined unless this iterator
        // is in the range [begin(), end()) for some list (i.e., the iterator
        // is not singular, is not end() and has not be invalidated).

    List_iterator operator--(int);
        // Move this iterator to the previous element in the list and return
        // its previous value.  The behavior is undefined unless this iterator
        // is in the range (begin(), end()] for some list (i.e., the iterator
        // is not singular, is not begin() and has not be invalidated).

    // ACCESSORS
    reference operator*() const;
        // Return a reference to the list object referenced by this iterator.
        // The behavior is undefined unless this iterator is in the range
        // [begin(), end()) for some list (i.e., the iterator is not signular,
        // is not end() and has not be invalidated).

    pointer operator->() const;
        // Return a pointer to the list object referenced by this iterator.
        // The behavior is undefined unless this iterator is in the range
        // [begin(), end()) for some list (i.e., the iterator is not singular,
        // is not end() and has not be invalidated).
};

// FREE OPERATORS
template <class T1, class T2, class NODEPTR, class DIFFTYPE>
inline
bool operator==(List_iterator<T1, NODEPTR, DIFFTYPE> a,
                List_iterator<T2, NODEPTR, DIFFTYPE> b);
    // Return true if the specified iterators 'a' and 'b' have the same value
    // and false otherwise.  Two iterators have the same value if both refer
    // to the same element of the same list or both are the end() iterator of
    // the same list.  The return value is undefined unless both 'a' and
    // 'b' are non-singular.

template <class T1, class T2, class NODEPTR, class DIFFTYPE>
inline
bool operator!=(List_iterator<T1, NODEPTR, DIFFTYPE> a,
                List_iterator<T2, NODEPTR, DIFFTYPE> b);
    // Return true if the specified iterators 'a' and 'b' do not have the same
    // value and false otherwise.  Two iterators have the same value if both
    // refer to the same element of the same list or both are the end()
    // iterator of the same list.  The return value is undefined unless both
    // 'a' and 'b' are non-singular.


                        // ===============
                        // class bsl::list
                        // ===============

template <class TYPE, class ALLOCATOR = bsl::allocator<TYPE> >
class list {
    // PRIVATE TYPES
    typedef List_node<TYPE>                       Node;

    typedef typename allocator_traits<ALLOCATOR>::template rebind_traits<Node>
        AllocTraits;

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

        typedef typename AllocTraits::size_type size_type;

        size_type d_size;

        explicit AllocAndSizeWrapper(const NodeAlloc& a, size_type s)
            : NodeAlloc(a), d_size(s) { }
    };

    class NodeProctor;
    friend class NodeProctor;

    class NodeProctor {
        // This class provides a proctor to free a node containing an
        // uninitialized 'TYPE' object in the event that an exception is
        // thrown.

        list     *d_list;
        NodePtr   d_p;
      public:
        explicit NodeProctor(list *l, NodePtr p) : d_list(l), d_p(p) { }
        ~NodeProctor() { if (d_p) d_list->free_node(d_p); }
        void release() { d_p = 0; }
    };

    struct comp_elems {
        // Binary function predicate object type for comparing two 'TYPE'
        // objects using 'operator<'.  This operation is usually, but not
        // always, the same as that provided by 'std::less<TYPE>'.  The
        // standard requires that certain functions use 'operator<', which
        // means that divergent specializations of 'std::less' are ignored.

        bool operator()(const TYPE& a, const TYPE& b) const
            { return a < b; }
    };

    // PRIVATE DATA
    NodePtr             d_sentinel;
    AllocAndSizeWrapper d_alloc_and_size;

    // PRIVATE FUNCTIONS
    NodeAlloc& allocator();
        // Return a modifiable reference to the allocator used to allocate
        // nodes.
    const NodeAlloc& allocator() const;
        // Return a non-modifiable reference to the allocator used to allocate
        // nodes.

    NodePtr head() const;
        // Return a pointer to the first node in the list or the sentinel node
        // if the list is empty.

    typename AllocTraits::size_type& size_ref();
        // Return a modifiable reference to the data element holding the size
        // of the list.
    const typename AllocTraits::size_type& size_ref() const;
        // Return a non-modifiable reference to the data element holding the
        // size of the list.

    NodePtr allocate_node();
        // Return a node allocated from the container's allocated.  Before
        // returning, the 'init' function is called to initialize the node's
        // pointers, but the node's constructor is not called.

    void free_node(NodePtr np);
        // Call 'np->deinit()' and deallocate the node pointed to by 'np'.
        // Note that the node's destructor is not called.

    void link_nodes(NodePtr prev, NodePtr next);
        // Modify the forward pointer of 'prev' to point to 'next' and the
        // backward pointer of 'next' to point to 'prev'.

    void create_sentinel();
        // Create the 'd_sentinel' node of the list.  The 'd_sentinel' node
        // does not hold a value.  When first created it's forward and
        // backward pointers point to itself, creating a circular linked list.
        // This function also sets the list's size to zero.

    void destroy_all();
        // Erase all elements, destroy and deallocate the 'd_sentinel' node,
        // and the list in an invalid but destructible state (i.e., with size
        // == -1).

    void quick_swap(list& other);
        // Quickly swaps 'd_sentinel' and 'size_ref()' of '*this' with 'other'
        // without checking the allocator.

  public:
    // PUBLIC TYPES
    typedef BloombergLP::bslalg::PassthroughTrait<
                                 ALLOCATOR,
                                 BloombergLP::bslalg::TypeTraitBitwiseMoveable>
            AllocatorBitwiseMoveableTrait;

    // Declare nested type traits for this class.

    BSLALG_DECLARE_NESTED_TRAITS3(
               list,
               BloombergLP::bslalg::TypeTraitHasStlIterators,
               AllocatorBitwiseMoveableTrait,
               BloombergLP::bslalg::PassthroughTraitBslmaAllocator<ALLOCATOR>);

    typedef TYPE&                                              reference;
    typedef const TYPE&                                        const_reference;
    typedef List_iterator<TYPE,NodePtr,DiffType>               iterator;
    typedef List_iterator<const TYPE,NodePtr,DiffType>         const_iterator;
    typedef typename allocator_traits<ALLOCATOR>::pointer      pointer;
    typedef typename allocator_traits<ALLOCATOR>::const_pointer
                                                               const_pointer;

    typedef typename AllocTraits::size_type       size_type;
    typedef typename AllocTraits::difference_type difference_type;
    typedef TYPE                                  value_type;
    typedef ALLOCATOR                             allocator_type;
    typedef bsl::reverse_iterator<iterator>       reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

    // CREATORS

    // 23.3.5.2 construct/copy/destroy
    explicit list(const ALLOCATOR& a = ALLOCATOR());
        // Construct an empty list that allocates memory using the specified
        // allocator 'a'.

    explicit list(size_type n);
        // Construct a list containing the specified 'n' elements and using a
        // default-constructed allocator.  The initial elements in the list
        // are constructed by "default-insertion" (see {Terminology}).

    list(size_type n, const TYPE& value, const ALLOCATOR& a = ALLOCATOR());
        // Construct a list using the specified allocator 'a' and insert the
        // specified 'n' number of elements created by "copy-insertion" (see
        // {Terminology}) from 'value'.

    template <class InputIter>
      list(InputIter first, InputIter last, const ALLOCATOR& a = ALLOCATOR(),
           typename BloombergLP::bslmf::EnableIf<
               !BloombergLP::bslmf::IsFundamental<InputIter>::VALUE
           >::type* = 0)
        // Construct a list using the specified allocator 'a' and insert the
        // number of elements determined by the size of the specified range
        // '[first, last)'.  Each initial element is created by
        // "copy-insertion" (see {Terminology}) from the corresponding element
        // in '[first, last)'.  Does not participate in overload resolution
        // unless 'InputIter' is an iterator type.  The behavior is undefined
        // unless '[first, last)' defines a range of valid objects.
        //
        // TBD: This function's signature is specified using 'bslmf::EnableIf'
        // in such a way that it will not be selected during overload
        // resolution if 'InputIter' is a fundamental type.  Unfortunately,
        // this function *will* (incorrectly) be selected if 'InputIter' is an
        // enumerated type.  The best way to correct this problem would be to
        // use 'std::is_arithmetic' (a currently unavailable metafunction that
        // would include enums) instead of 'bslmf::IsFundamental' in the
        // 'bslmf::EnableIf' expression.
      : d_alloc_and_size(a, size_type(-1))
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // EnableIf be in-place inline.

        // '*this' is in an invalid but destructible state (size == -1).
        // Create a temporary list, 'tmp' with the specified data.  If an
        // exception is thrown, 'tmp's destructor will clean up.  Otherwise,
        // swap 'tmp' with '*this', leaving 'tmp' in an invalid but
        // destructible state and leaving '*this' fully constructed.

        list tmp(allocator());
        tmp.assign(first, last);
        quick_swap(tmp);
    }

    list(const list& x);
        // Construct a copy of the specified list 'x'.  If 'ALLOCATOR' is
        // convertible from 'bslma_Allocator*', then the resulting list will
        // use the default allocator; otherwise, the resulting list will use a
        // copy of 'x.get_allocator()'.  Each element in the resulting list is
        // constructed by "copy-insertion" (see {Terminology}) from the
        // corresponding element in 'x'.

    list(const list&, const ALLOCATOR& a);
        // Construct a copy of the specified list 'x' using a copy of the
        // specified allocator 'a'.  Each element in the resulting list is
        // constructed by "copy-insertion" (See {Terminology}) from the
        // corresponding element in 'x'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    list(list&& x);
        // Construct a new list using the contents and allocator from the
        // specified list 'x'.  No copy or move constructors are called for
        // individual elements.  After the construction, the value of 'x' is
        // valid, but unspecified.  Note that this constructor may allocate
        // memory and may, therfore, throw an allocation-related exception.

    list(list&& x, const ALLOCATOR& a);
        // Construct a new list using the contents from the specified list 'x'
        // and using a copy of the specified allocator 'a'.  If 'a ==
        // a.get_allocator()', then no copy or move constructors are called
        // for individual elements.  Otherwise, each element in the resulting
        // list is constructed by "copy-insertion" (See {Terminology}) from
        // the corresponding element in 'x'.  After the construction, the
        // value of 'x' is valid, but unspecified.

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    ~list();
        // Destroy this list by calling the destructor for each element and
        // deallocating all allocated storage.

    list& operator=(const list& x);
        // Replace the elements of this list with copies of the elements of
        // the specified list 'x'.  Each element of 'x' is either
        // copy-assigned or copy-inserted from the corresponding element of
        // 'x'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    list& operator=(list&& x);
        // Replace the elements of this list with the elements or copies of
        // the elements of the specified list 'x'.  If 'x.get_allocator() ==
        // this->get_allocator()', then no move or copy operations are applied
        // to any individual elements; otherwise each element this list is
        // modified by either copy-assignment or "copy-insertion" (see
        // {Terminology}) from the corresponding element of 'x'.  After the
        // construction, the value of 'x' is valid, but unspecified.

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    // MANIPULATORS
    template <class InputIter>
    void assign(InputIter first, InputIter last,
                typename BloombergLP::bslmf::EnableIf<
                    !BloombergLP::bslmf::IsFundamental<InputIter>::VALUE
                >::type * = 0)
        // Replace the contents of this list with copies of the elements in
        // the specified range '[first, last)'.  Each element in this list is
        // set by either copy-assignment or "copy-insertion" (see
        // {Terminology}) from the corresponding element in '[first, last)'.
        // Does not participate in overload resolution unless 'InputIter' is
        // an iterator type.  The behavior is undefined unless '[first, last)'
        // is a range of valid iterators not into this list.
        //
        // TBD: This function's signature is specified using 'bslmf::EnableIf'
        // in such a way that it will not be selected during overload
        // resolution if 'InputIter' is a fundamental type.  Unfortunately,
        // this function *will* (incorrectly) be selected if 'InputIter' is an
        // enumerated type.  The best way to correct this problem is to use
        // 'bslmf::IsArithmetic' (a currently unimplemented metafunction that
        // would include enums) instead of 'bslmf::IsFundamental' in the
        // 'bslmf::EnableIf' expression.
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // EnableIf be in-place inline.

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

    void assign(size_type n, const TYPE& t);
        // Replace the contents of this list with the specified 'n' copies of
        // the specified value 't'.  Each element in this list is set by
        // either copy-assignment or "copy-insertion" (See {Terminology}) from
        // 't'.

    // 23.3.5.4 modifiers
#ifdef BSLS_SIMULATED_VARIADIC_TEMPLATES
    template <class... Args>
    void emplace_front(Args&&... args);
        // Insert a new element at the front of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // 'args'.
#else
    void emplace_front();
        // Insert a new element at the front of this list and construct it
        // using "default-insertion" (see {Terminology}).
    template <class ARG1>
    void emplace_front(const ARG1& a1);
        // Insert a new element at the front of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // argument 'a1'.
    template <class ARG1, class ARG2>
    void emplace_front(const ARG1& a1, const ARG2& a2);
        // Insert a new element at the front of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // arguments 'a1' and 'a2'.
    template <class ARG1, class ARG2, class ARG3>
    void emplace_front(const ARG1& a1, const ARG2& a2, const ARG3& a3);
        // Insert a new element at the front of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // arguments 'a1', 'a2', and 'a3'.
    template <class ARG1, class ARG2, class ARG3, class ARG4>
    void emplace_front(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                       const ARG4& a4);
        // Insert a new element at the front of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // arguments 'a1', 'a2', 'a3', and 'a4'.
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    void emplace_front(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                       const ARG4& a4, const ARG5& a5);
        // Insert a new element at the front of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // arguments 'a1', 'a2', 'a3', 'a4', and 'a5'.
#endif
    void pop_front();
        // Remove and destroy the first element of this list.  The behavior is
        // undefined unless this list contains at least one element.

#ifdef BSLS_SIMULATED_VARIADIC_TEMPLATES
    template <class... Args>
    void emplace_back(Args&&... args);
        // Insert a new element at the back of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // 'args'.
#else
    void emplace_back();
        // Insert a new element at the back of this list and construct it
        // using "default-insertion" (see {Terminology}).
    template <class ARG1>
    void emplace_back(const ARG1& a1);
        // Insert a new element at the back of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // argument 'a1'.
    template <class ARG1, class ARG2>
    void emplace_back(const ARG1& a1, const ARG2& a2);
        // Insert a new element at the back of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // arguments 'a1' and 'a2'.
    template <class ARG1, class ARG2, class ARG3>
    void emplace_back(const ARG1& a1, const ARG2& a2, const ARG3& a3);
        // Insert a new element at the back of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // arguments 'a1', 'a2', and 'a3'.
    template <class ARG1, class ARG2, class ARG3, class ARG4>
    void emplace_back(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                       const ARG4& a4);
        // Insert a new element at the back of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // arguments 'a1', 'a2', 'a3', and 'a4'.
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    void emplace_back(const ARG1& a1, const ARG2& a2, const ARG3& a3,
                       const ARG4& a4, const ARG5& a5);
        // Insert a new element at the back of this list and construct it
        // using "emplace-construction" (see {Terminology}) from the specified
        // arguments 'a1', 'a2', 'a3', 'a4', and 'a5'.
#endif

    void pop_back();
        // Remove and destroy the last element of this list.  The behavior is
        // undefined unless this list contains at least one element.

    void push_front(const TYPE& x);
        // Insert a new element at the front of this list using
        // "copy-insertion" (see {Terminology}) from the specified value 'x'.

    void push_back(const TYPE& x);
        // Append a new element to the end of this list using
        // "copy-insertion" (see {Terminology}) from the specified value 'x'.

#ifdef BSLS_SIMULATED_VARIADIC_TEMPLATES
    template <class... Args>
    iterator emplace(const_iterator position, Args&&... args);
        // Insert a new element into this list before the element at the
        // specified 'position' using "emplace-construction" (see
        // {Terminology}) from the specified 'args'.
#else
    iterator emplace(const_iterator position);
        // Insert a new element into this list before the element at the
        // specified 'position' using "default-insertion" (see {Terminology}).
    template <class ARG1>
    iterator emplace(const_iterator position, const ARG1& a1);
        // Insert a new element into this list before the element at the
        // specified 'position' using "emplace-construction" (see
        // {Terminology}) from the specified argument 'a1'.
    template <class ARG1, class ARG2>
    iterator emplace(const_iterator position, const ARG1& a1, const ARG2& a2);
        // Insert a new element into this list before the element at the
        // specified 'position' using "emplace-construction" (see
        // {Terminology}) from the specified arguments 'a1' and 'a2'.
    template <class ARG1, class ARG2, class ARG3>
    iterator emplace(const_iterator position, const ARG1& a1, const ARG2& a2,
                     const ARG3& a3);
        // Insert a new element into this list before the element at the
        // specified 'position' using "emplace-construction" (see
        // {Terminology}) from the specified arguments 'a1', 'a2', and 'a3'.
    template <class ARG1, class ARG2, class ARG3, class ARG4>
    iterator emplace(const_iterator position, const ARG1& a1, const ARG2& a2,
                     const ARG3& a3, const ARG4& a4);
        // Insert a new element into this list before the element at the
        // specified 'position' using "emplace-construction" (see
        // {Terminology}) from the specified arguments 'a1', 'a2', 'a3', and
        // 'a4'.
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
    iterator emplace(const_iterator position, const ARG1& a1, const ARG2& a2,
                     const ARG3& a3, const ARG4& a4, const ARG5& a5);
        // Insert a new element into this list before the element at the
        // specified 'position' using "emplace-construction" (see
        // {Terminology}) from the specified arguments 'a1', 'a2', 'a3', 'a4',
        // and 'a5'.
#endif
    iterator insert(const_iterator position, const TYPE& x);
    iterator insert(const_iterator position, size_type n, const TYPE& x);
    template <class InputIter>
    iterator insert(const_iterator position, InputIter first, InputIter last,
                    typename BloombergLP::bslmf::EnableIf<
                        !BloombergLP::bslmf::IsFundamental<InputIter>::VALUE
                    >::type * = 0)
        // Insert the specified range '[first, last)' into this list at the
        // specified 'position' and return an iterator to the first inserted
        // element or 'position' if the range is empty.  Does not participate
        // in overload resolution unless 'InputIter' is an iterator type.
        //
        // TBD: This function's signature is specified using 'bslmf::EnableIf'
        // in such a way that it will not be selected during overload
        // resolution if 'InputIter' is a fundamental type.  Unfortunately,
        // this function *will* (incorrectly) be selected if 'InputIter' is an
        // enumerated type.  The best way to correct this problem is to use
        // 'bslmf::IsArithmetic' (a currently unimplemented metafunction that
        // would include enums) instead of 'bslmf::IsFundamental' in the
        // 'bslmf::EnableIf' expression.
    {
        // MS Visual Studio 2008 compiler requires that a function using
        // EnableIf be inplace inline.

        if (first == last) {
            return position.unconst();
        }

        // Remember the position of the first insertion
        iterator ret = insert(position, *first);
        for (++first; first != last; ++first) {
            insert(position, *first);
        }

        return ret;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void push_front(TYPE&& x);
    void push_back(TYPE&& x);
    iterator insert(const_iterator position, TYPE&& x);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    iterator erase(const_iterator position);
        // Remove from this list the element at the specified 'position', and
        // return an iterator pointing to the element immediately following the
        // removed element, or to the position returned by the 'end' method if
        // the removed element was the last in the sequence.  The behavior is
        // undefined unless 'position' is an iterator in the range
        // '[ begin(), end() )'.

    iterator erase(const_iterator position, const_iterator last);
        // Remove from this list the elements starting at the specified
        // 'first' position that are before the specified 'last' position, and
        // return an iterator pointing to the element immediately following the
        // last removed element, or the position returned by the method 'end'
        // if the removed elements were last in the sequence.  The behavior is
        // undefined unless 'first' is an iterator in the range
        // '[ begin(), end() ]' and 'last' is an iterator in the range
        // '[ first, end() ]' (both endpoints included).

    void swap(list& other);
        // Exchange the value of this list with that of the specified 'other'
        // list, such that each list has, upon return, the value of the other
        // list prior to this call.  This method does not throw or invalidate
        // iterators if 'get_allocator', invoked on this list and 'other',
        // returns the same value.

    void clear();
        // Remove all the elements from this list.

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
        // '[first, last)' represents a range of valid elements in 'x', and
        // 'position' is not in the range '[first, last)'.


#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void splice(const_iterator position, list&& x);
    void splice(const_iterator position, list&& x, const_iterator i);
    void splice(const_iterator position,
                list&& x,
                const_iterator first,
                const_iterator last);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    void remove(const TYPE& value);
        // Erase all the elements having the specified 'value' from this list.

    template <class Predicate>
    void remove_if(Predicate pred);
        // Erase from the list all the elements that are not predicted 'false'
        // by the specified predicate 'pred'.

    void unique();
        // Erase from this list all but the first element of every consecutive
        // group of elements that have the same value.

    template <class EqPredicate>
    void unique(EqPredicate binary_pred);
        // Erase from this list all but the first element of every consecutive
        // group of elements that have the specified 'binary_pred' predicates
        // 'true' for any two consecutive elements in the group.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void merge(list&& x);
    template <class COMPARE>
    void merge(list&& x, COMPARE comp);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void merge(list& x);
        // Merge the specified sorted list 'x' into this sorted list.  The
        // method has no effect if 'x' is this list.  The behavior is undefined
        // unless both 'x' and this list are sorted in non-decreasing order
        // according to the ordering returned by 'operator<'.

    template <class COMPARE>
    void merge(list& x, COMPARE comp);
        // Merge the specified sorted list 'x' into this sorted list, using the
        // specified 'comp', which defines a strict weak ordering, to order
        // elements.  The method has no effect if 'x' is this list.  The
        // behavior is undefined unless both 'x' and this list are sorted in
        // non-decreasing order according to the ordering returned by 'comp'.

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
    allocator_type get_allocator() const;
        // Return a copy of the allocator used for memory allocation by this
        // list.

    iterator begin();
        // Return a mutating iterator pointing to the first element in the
        // list, if any, or one past the end of the list if the list is empty.

    const_iterator begin() const;
        // Return a non-mutating iterator pointing to the first element in the
        // list, if any, or one past the end of the list if the list is empty.

    iterator end();
        // Return a mutating iterator pointing one past the end of the list.

    const_iterator end() const;
        // Return a non-mutating iterator pointing one past the end of the list.

    reverse_iterator rbegin();
        // Return  a mutating reverse iterator pointing to the last element
        // of the list (i.e., the first element of the reverse sequence), if
        // any, or one before the start if the list is empty.

    const_reverse_iterator rbegin() const;
        // Return  a const reverse iterator pointing to the last element
        // of the list (i.e., the first element of the reverse sequence), if
        // any, or one before the start if the list is empty.

    reverse_iterator rend();
        // Return a mutating reverse iterator pointing one before the start of
        // the list.

    const_reverse_iterator rend() const;
        // Return a const reverse iterator pointing one before the start of
        // the list.

    const_iterator cbegin() const;
        // Return a non-mutating iterator pointing to the first element in the
        // list, if any, or one past the end of the list if the list is empty.

    const_iterator cend() const;
        // Return a non-mutating iterator pointing one past the end of the
        // list.

    const_reverse_iterator crbegin() const;
        // Return  a const reverse iterator pointing to the last element
        // of the list (i.e., the first element of the reverse sequence), if
        // any, or one before the start if the list is empty.

    const_reverse_iterator crend() const;
        // Return a const reverse iterator pointing one before the start of
        // the list.

    // 23.3.5.3 capacity
    bool empty() const;
        // Return true if the list has no elements and false otherwise.

    size_type size() const;
        // Return the number of elements in this list.

    size_type max_size() const;
        // Return an upper bound on the largest number of elements that this
        // list could possibly hold.  Note that return value of this function
        // does not guarantee that the list can succesfully grow that large,
        // or even close to that large without running out of resources.

    void resize(size_type sz);
        // Resize this list to the specified 'sz' elements.  If 'sz' is less
        // than or equal to the previous size of this list, then erase the
        // excess elements from the end.  Otherwise, append additional
        // elements to the end using "default-insertion" (see {Terminology})
        // until there are a total of 'sz' elements.

    void resize(size_type sz, const TYPE& c);
        // Resize this list to the specified 'sz' elements, with added
        // elements being copies of the specified value 'c'.  If 'sz' is less
        // than or equal to the previous size of this list, then erase the
        // excess elements from the end.  Otherwise, append additional
        // elements to the end using "copy-insertion" (see {Terminology}) from
        // 'c' until there are a total of 'sz' elements.

    reference front();
        // Return a modifiable reference to the first element of this list.
        // The behavior is undefined unless this list contains at least one
        // element.

    const_reference front() const;
        // Return a non-modifiable reference to the first element of this list.
        // The behavior is undefined unless this list contains at least one
        // element.

    reference back();
        // Return a modifiable reference to the last element of this list.
        // The behavior is undefined unless this list contains at least one
        // element.

    const_reference back() const;
        // Return a non-modifiable reference to the last element of this list.
        // The behavior is undefined unless this list contains at least one
        // element.

  private:
    // These private functions cannot be declared until 'const_iterator'
    // and 'size_type' have been declared.

    iterator insert_node(const_iterator position, NodePtr node);
        // Insert the specified 'node' prior to the specified 'position' in
        // the list.

    template <class COMPARE>
    NodePtr merge_imp(NodePtr node1,
                      NodePtr node2,
                      NodePtr finish,
                      COMPARE comp);
        // Given a contiguous sequence of nodes, '[node1, finish)' with
        // 'node2', pointing somewhere in the middle of the sequence, merge
        // sequence '[node2, finish)' into '[node1, node2)' and return a
        // pointer to the beginning of the merged sequence.  If an exception is
        // thrown, all nodes remain in the list, but their order is
        // unspecified.  The behavior is undefined unless '[node1, node2)' and
        // '[node2, finish)' each describe a contiguous sequence of nodes.

    template <class COMPARE>
    NodePtr sort_imp(NodePtr*       pnode1,
                     size_type      size,
                     const COMPARE& comp);
        // Sort the sequence of 'size' nodes starting with '*pnode1'.
        // Modifies '*pnode1' to refer to the first node of the sorted
        // sequence.  If an exception is thrown, all nodes remain properly
        // linked, but their order is unspecified.  The behavior is undefined
        // unless '*pnode1' begins a sequence of at least 'size' nodes, none
        // of which are sentinel nodes.
};

// FREE OPERATORS
template <class TYPE, class ALLOCATOR> inline
bool operator==(const list<TYPE, ALLOCATOR>& x,
                const list<TYPE, ALLOCATOR>& y);

template <class TYPE, class ALLOCATOR> inline
bool operator< (const list<TYPE, ALLOCATOR>& x,
                const list<TYPE, ALLOCATOR>& y);

template <class TYPE, class ALLOCATOR> inline
bool operator!=(const list<TYPE, ALLOCATOR>& x,
                const list<TYPE, ALLOCATOR>& y);

template <class TYPE, class ALLOCATOR> inline
bool operator> (const list<TYPE, ALLOCATOR>& x,
                const list<TYPE, ALLOCATOR>& y);

template <class TYPE, class ALLOCATOR> inline
bool operator>=(const list<TYPE, ALLOCATOR>& x,
                const list<TYPE, ALLOCATOR>& y);

template <class TYPE, class ALLOCATOR> inline
bool operator<=(const list<TYPE, ALLOCATOR>& x,
                const list<TYPE, ALLOCATOR>& y);

// SPECIALIZED ALGORITHMS
template <class TYPE, class ALLOCATOR>
void swap(list<TYPE, ALLOCATOR>& x, list<TYPE, ALLOCATOR>& y);

}  // close namespace bsl

// ===========================================================================
//                   INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                        // ---------------------
                        // struct bsl::List_Node
                        // ---------------------

template <class TYPE>
inline void bsl::List_node<TYPE>::init()
{
    // If 'List_Node' is ever enhanced to allow for generalized pointers
    // (e.g., a 'NodePtr' that is not a raw pointer), then this function
    // will be responsible for constructing 'd_prev' and 'd_next', rather
    // than just setting them to null:
    //
    //    new ((void*) BloombergLP::bsls::Util::addressOf(d_prev))
    //                                                        NodePtr(nullptr);
    //    new ((void*) BloombergLP::bsls::Util::addressOf(d_next))
    //                                                        NodePtr(nullptr);
    d_prev = d_next = 0;
}

template <class TYPE>
inline void bsl::List_node<TYPE>::deinit()
{
    // If 'List_Node' is ever enhanced to allow for generalized pointers
    // (e.g., a 'NodePtr' that is not a raw pointer), then this function
    // will be responsible for calling the destructors for 'd_prev' and
    // 'd_next':
    //
    //    d_prev.~NodePtr();
    //    d_next.~NodePtr();
}

                        // ------------------------
                        // class bsl::List_iterator
                        // ------------------------

template <class TYPE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::List_iterator() /* = default; */
{
}

template <class TYPE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::List_iterator(NODEPTR p)
    : d_nodeptr(p)
{
}

template <class TYPE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::List_iterator(const NcIter& other)
    : d_nodeptr(other.d_nodeptr)
{
}

template <class TYPE, class NODEPTR, class DIFFTYPE>
inline
typename bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::reference
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::operator*() const
{
    return this->d_nodeptr->d_value;
}

template <class TYPE, class NODEPTR, class DIFFTYPE>
inline
typename bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::pointer
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::operator->() const
{
    return BloombergLP::bsls::Util::addressOf(this->d_nodeptr->d_value);
}

template <class TYPE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>&
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::operator++()
{
    this->d_nodeptr = this->d_nodeptr->d_next;
    return *this;
}

template <class TYPE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>&
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::operator--()
{
    this->d_nodeptr = this->d_nodeptr->d_prev;
    return *this;
}

template <class TYPE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::operator++(int)
{
    List_iterator temp = *this;
    this->operator++();
    return temp;
}

template <class TYPE, class NODEPTR, class DIFFTYPE>
inline
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>
bsl::List_iterator<TYPE, NODEPTR, DIFFTYPE>::operator--(int)
{
    List_iterator temp = *this;
    this->operator--();
    return temp;
}

template <class T1, class T2, class NODEPTR, class DIFFTYPE>
inline
bool bsl::operator==(List_iterator<T1,NODEPTR,DIFFTYPE> a,
                     List_iterator<T2,NODEPTR,DIFFTYPE> b)
{
    // 'a' and 'b' have the same 'NODEPTR' type; their 'd_nodeptr' members can
    // thus be compared for equality regardless of 'T1' and 'T2'.  However,
    // all instantiations of 'List_iterator' with the same 'NODEPTR' will have
    // types 'T1' or 'T2' that differ only in their const qualification.
    return a.d_nodeptr == b.d_nodeptr;
}

template <class T1, class T2, class NODEPTR, class DIFFTYPE>
inline
bool bsl::operator!=(List_iterator<T1,NODEPTR,DIFFTYPE> a,
                     List_iterator<T2,NODEPTR,DIFFTYPE> b)
{
    return ! (a == b);
}

                        // ---------------
                        // class bsl::list
                        // ---------------

namespace bsl {

// PRIVATE FUNCTIONS
template <class TYPE, class ALLOCATOR>
inline
typename list<TYPE, ALLOCATOR>::NodeAlloc& list<TYPE, ALLOCATOR>::allocator()
{
    return d_alloc_and_size;  // Implicit cast to base class
}

template <class TYPE, class ALLOCATOR>
inline
const typename list<TYPE, ALLOCATOR>::NodeAlloc&
list<TYPE, ALLOCATOR>::allocator() const
{
    return d_alloc_and_size;  // Implicit cast to base class
}

template <class TYPE, class ALLOCATOR>
inline
typename list<TYPE, ALLOCATOR>::NodePtr list<TYPE, ALLOCATOR>::head() const
{
    return d_sentinel->d_next;
}

template <class TYPE, class ALLOCATOR>
inline
typename list<TYPE, ALLOCATOR>::AllocTraits::size_type&
list<TYPE, ALLOCATOR>::size_ref()
{
    return d_alloc_and_size.d_size;
}

template <class TYPE, class ALLOCATOR>
inline
const typename list<TYPE, ALLOCATOR>::AllocTraits::size_type&
list<TYPE, ALLOCATOR>::size_ref() const
{
    return d_alloc_and_size.d_size;
}

template <class TYPE, class ALLOCATOR>
inline
typename list<TYPE, ALLOCATOR>::NodePtr list<TYPE, ALLOCATOR>::allocate_node()
{
    NodePtr ret = AllocTraits::allocate(allocator(), 1);
    ret->init();
    return ret;
}

template <class TYPE, class ALLOCATOR>
inline
void list<TYPE, ALLOCATOR>::free_node(NodePtr np)
{
    np->deinit();
    AllocTraits::deallocate(allocator(), np, 1);
}

template <class TYPE, class ALLOCATOR>
inline
void list<TYPE, ALLOCATOR>::link_nodes(NodePtr prev, NodePtr next)
{
    prev->d_next = next;
    next->d_prev = prev;
}

template <class TYPE, class ALLOCATOR>
inline
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::insert_node(const_iterator position, NodePtr node)
{
    typename AllocTraits::pointer next = position.d_nodeptr;
    typename AllocTraits::pointer prev = next->d_prev;
    link_nodes(prev, node);
    link_nodes(node, next);
    ++size_ref();
    return iterator(node);
}

template <class TYPE, class ALLOCATOR>
inline
void list<TYPE, ALLOCATOR>::create_sentinel()
{
    d_sentinel = allocate_node();
    link_nodes(d_sentinel, d_sentinel);  // circular
    size_ref() = 0;
}

template <class TYPE, class ALLOCATOR>
inline
void list<TYPE, ALLOCATOR>::destroy_all()
{
    clear();
    free_node(d_sentinel);
    size_ref() = size_type(-1);
}

template <class TYPE, class ALLOCATOR>
inline
void list<TYPE, ALLOCATOR>::quick_swap(list& other)
{
    NodePtr tmpSentinel = d_sentinel;
    d_sentinel = other.d_sentinel;
    other.d_sentinel = tmpSentinel;

    size_type tmpSize = size_ref();
    size_ref() = other.size_ref();
    other.size_ref() = tmpSize;
}

// CREATORS

// 23.3.5.2 construct/copy/destroy
template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>::list(const ALLOCATOR& a)
    : d_alloc_and_size(a, 0) { create_sentinel(); }

template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>::list(size_type n)
    : d_alloc_and_size(ALLOCATOR(), size_type(-1))
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

template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>::list(size_type n, const TYPE& value, const ALLOCATOR& a)
    : d_alloc_and_size(a, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).
    list tmp(allocator());
    tmp.assign(n, value); // 'tmp's destructor will clean up on throw.
    quick_swap(tmp);  // Leave 'tmp' in an invalid but destructible state.
}

template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>::list(const list& x)
    : d_alloc_and_size(
       AllocTraits::select_on_container_copy_construction(x.allocator()),
       size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).
    list tmp(allocator());
    tmp.assign(x.begin(), x.end()); // 'tmp's destructor will clean up on throw
    quick_swap(tmp);  // Leave 'tmp' in an invalid but destructible state.
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>::list(list&& x)
    // Allocator should be copied, not moved, to ensure identical allocators
    // between this and 'x', otherwise 'swap' will be undefined.
    : d_alloc_and_size(x.allocator(), 0)
{
    create_sentinel();
    quick_swap(x);
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>::list(const list& x, const ALLOCATOR& a)
    : d_alloc_and_size(a, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).
    list tmp(allocator());
    tmp.assign(x.begin(), x.end()); // 'tmp's destructor will clean up on throw
    quick_swap(tmp);  // Leave 'tmp' in an invalid but destructible state.
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>::list(list&& x, const ALLOCATOR& a)
    : d_alloc_and_size(a, size_type(-1))
{
    // '*this' is in an invalid but destructible state (size == -1).
    if (a == x.allocator()) {
        create_sentinel();
        size_ref() = 0; // '*this' is now in a valid state
        quick_swap(x);
    }
    else {
        list tmp(allocator());
        tmp.assign(x.begin(), x.end()); // 'tmp's destructor will clean up
        quick_swap(tmp);  // Leave 'tmp' in an invalid but destructible state.
    }
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>::~list()
{
    // A size of -1 is a special incompletely-initialized or
    // destructively-moved-from state.
    if (size_ref() != size_type(-1)) {
        destroy_all();
    }
}

template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>& list<TYPE, ALLOCATOR>::operator=(const list& x)
{
    if (this == &x) {
        return *this;
    }

    if (AllocTraits::propagate_on_container_copy_assignment::VALUE &&
        allocator() != x.allocator()) {
        // Completely destroy and rebuild list using new allocator.

        // Create a new list with the new allocator.  This operation might
        // throw, so we do it before destroying the old list.
        list temp(x.get_allocator());

        // Clear existing list and leave in an invalid but destructible state.
        destroy_all();

        // Assign allocator (required not to throw).
        allocator() = x.allocator();

        // Now swap lists, leaving 'temp' in an invalid but destructible state.
        quick_swap(temp);
    }

    assign(x.begin(), x.end());
    return *this;
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE, class ALLOCATOR>
list<TYPE, ALLOCATOR>& list<TYPE, ALLOCATOR>::operator=(list&& x)
{
    if (this == &x) {
        return *this;
    }

    if (allocator() == x.allocator()) {
        // Equal allocators, just swap contents:
        quick_swap(x);
    }
    else if (AllocTraits::propagate_on_container_move_assignment::VALUE) {
        // Completely destroy and rebuild list using new allocator.

        // Create a new list with the new allocator and new contents.  This
        // operation might throw, so we do it before destroying the old list.
        list temp(std::forward<list>(x));

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
        assign(x.begin(), x.end());
    }

    return *this;
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::assign(size_type n, const TYPE& t)
{
    iterator i = this->begin();
    iterator e = this->end();

    for (; n > 0 && i != e; --n, ++i) {
        *i = t;
    }

    erase(i, e);

    for (; n > 0; --n) {
        insert(e, t);
    }
}

template <class TYPE, class ALLOCATOR>
ALLOCATOR list<TYPE, ALLOCATOR>::get_allocator() const
{
    return allocator();
}

// iterators:
template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::iterator list<TYPE, ALLOCATOR>::begin()
{
    return iterator(head());
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::const_iterator
list<TYPE, ALLOCATOR>::begin() const
{
    return const_iterator(head());
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::iterator list<TYPE, ALLOCATOR>::end()
{
    return iterator(d_sentinel);
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::const_iterator
list<TYPE, ALLOCATOR>::end() const
{
    return const_iterator(d_sentinel);
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::reverse_iterator
list<TYPE, ALLOCATOR>::rbegin()
{
    return reverse_iterator(end());
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::const_reverse_iterator
list<TYPE, ALLOCATOR>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::reverse_iterator list<TYPE, ALLOCATOR>::rend()
{
    return reverse_iterator(begin());
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::const_reverse_iterator
list<TYPE, ALLOCATOR>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::const_iterator
list<TYPE, ALLOCATOR>::cbegin() const
{
    return begin();
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::const_iterator
list<TYPE, ALLOCATOR>::cend() const
{
    return end();
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::const_reverse_iterator
list<TYPE, ALLOCATOR>::crbegin() const
{
    return rbegin();
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::const_reverse_iterator
list<TYPE, ALLOCATOR>::crend() const
{
    return rend();
}

// 23.3.5.3 capacity
template <class TYPE, class ALLOCATOR>
bool list<TYPE, ALLOCATOR>::empty() const
{
    return 0 == size_ref();
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::size_type list<TYPE, ALLOCATOR>::size() const
{
    return size_ref();
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::size_type
list<TYPE, ALLOCATOR>::max_size() const
{
    return AllocTraits::max_size(allocator());
}

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::resize(size_type sz)
{
    if (sz > size()) {
        emplace_back();
        TYPE& c = back();
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

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::resize(size_type sz, const TYPE& c)
{
    while (sz > size()) {
        push_back(c);
    }

    while (sz < size()) {
        pop_back();
    }
}

// element access:
template <class TYPE, class ALLOCATOR>
TYPE& list<TYPE, ALLOCATOR>::front()
{
    BSLS_ASSERT_SAFE(size_ref() > 0);
    return head()->d_value;
}

template <class TYPE, class ALLOCATOR>
const TYPE& list<TYPE, ALLOCATOR>::front() const
{
    BSLS_ASSERT_SAFE(size_ref() > 0);
    return head()->d_value;
}

template <class TYPE, class ALLOCATOR>
TYPE& list<TYPE, ALLOCATOR>::back()
{
    BSLS_ASSERT_SAFE(size_ref() > 0);
    NodePtr last = d_sentinel->d_prev;
    return last->d_value;
}

template <class TYPE, class ALLOCATOR>
const TYPE& list<TYPE, ALLOCATOR>::back() const
{
    BSLS_ASSERT_SAFE(size_ref() > 0);
    NodePtr last = d_sentinel->d_prev;
    return last->d_value;
}

// 23.3.5.4 modifiers
#ifdef BSLS_SIMULATED_VARIADIC_TEMPLATES
template <class TYPE, class ALLOCATOR>
    template <class... Args>
void list<TYPE, ALLOCATOR>::emplace_front(Args&&... args)
{
    emplace(begin(), std::forward<Args>(args)...);
}
#else
template <class TYPE, class ALLOCATOR>
inline
void list<TYPE, ALLOCATOR>::emplace_front()
{
    emplace(begin());
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1>
inline
void list<TYPE, ALLOCATOR>::emplace_front(const ARG1& a1)
{
    emplace(begin(), a1);
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1, class ARG2>
inline
void list<TYPE, ALLOCATOR>::emplace_front(const ARG1& a1, const ARG2& a2)
{
    emplace(begin(), a1, a2);
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1, class ARG2, class ARG3>
inline
void list<TYPE, ALLOCATOR>::emplace_front(const ARG1& a1,
                                          const ARG2& a2,
                                          const ARG3& a3)
{
    emplace(begin(), a1, a2, a3);
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1, class ARG2, class ARG3, class ARG4>
inline
void list<TYPE, ALLOCATOR>::emplace_front(const ARG1& a1,
                                          const ARG2& a2,
                                          const ARG3& a3,
                                          const ARG4& a4)
{
    emplace(begin(), a1, a2, a3, a4);
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
inline
void list<TYPE, ALLOCATOR>::emplace_front(const ARG1& a1,
                                          const ARG2& a2,
                                          const ARG3& a3,
                                          const ARG4& a4,
                                          const ARG5& a5)
{
    emplace(begin(), a1, a2, a3, a4, a5);
}
#endif // ! BSLS_SIMULATED_VARIADIC_TEMPLATES

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::pop_front()
{
    BSLS_ASSERT_SAFE(size_ref() > 0);
    erase(begin());
}

#ifdef BSLS_SIMULATED_VARIADIC_TEMPLATES
template <class TYPE, class ALLOCATOR>
    template <class... Args>
void list<TYPE, ALLOCATOR>::emplace_back(Args&&... args)
{
    emplace(end(), std::forward<Args>(args)...);
}
#else
template <class TYPE, class ALLOCATOR>
inline
void list<TYPE, ALLOCATOR>::emplace_back()
{
    emplace(end());
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1>
inline
void list<TYPE, ALLOCATOR>::emplace_back(const ARG1& a1)
{
    emplace(end(), a1);
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1, class ARG2>
inline
void list<TYPE, ALLOCATOR>::emplace_back(const ARG1& a1, const ARG2& a2)
{
    emplace(end(), a1, a2);
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1, class ARG2, class ARG3>
inline
void list<TYPE, ALLOCATOR>::emplace_back(const ARG1& a1,
                                         const ARG2& a2,
                                         const ARG3& a3)
{
    emplace(end(), a1, a2, a3);
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1, class ARG2, class ARG3, class ARG4>
inline
void list<TYPE, ALLOCATOR>::emplace_back(const ARG1& a1,
                                         const ARG2& a2,
                                         const ARG3& a3,
                                         const ARG4& a4)
{
    emplace(end(), a1, a2, a3, a4);
}

template <class TYPE, class ALLOCATOR>
    template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
inline
void list<TYPE, ALLOCATOR>::emplace_back(const ARG1& a1,
                                         const ARG2& a2,
                                         const ARG3& a3,
                                         const ARG4& a4,
                                         const ARG5& a5)
{
    emplace(end(), a1, a2, a3, a4, a5);
}
#endif // ! BSLS_SIMULATED_VARIADIC_TEMPLATES

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::pop_back()
{
    BSLS_ASSERT_SAFE(size_ref() > 0);
    erase(--end());
}

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::push_front(const TYPE& x)
{
    emplace(begin(), x);
}

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::push_back(const TYPE& x)
{
    emplace(end(), x);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::push_front(TYPE&& x)
{
    emplace(begin(), std::move(x));
}

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::push_back(TYPE&& x)
{
    emplace(end(), std::move(x));
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

#ifdef BSLS_SIMULATED_VARIADIC_TEMPLATES
template <class TYPE, class ALLOCATOR>
  template <class... Args>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::emplace(const_iterator position, Args&&... args)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(allocator(), p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           std::forward<Args>(args)...);
    proctor.release();
    return insert_node(position, p);
}
#else
template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::emplace(const_iterator position)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value));
    proctor.release();
    return insert_node(position, p);
}

template <class TYPE, class ALLOCATOR>
  template <class ARG1>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::emplace(const_iterator position, const ARG1& a1)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           a1);
    proctor.release();
    return insert_node(position, p);
}

template <class TYPE, class ALLOCATOR>
  template <class ARG1, class ARG2>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::emplace(const_iterator position,
                               const ARG1& a1,
                               const ARG2& a2)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                       BloombergLP::bsls::Util::addressOf(p->d_value), a1, a2);
    proctor.release();
    return insert_node(position, p);
}

template <class TYPE, class ALLOCATOR>
  template <class ARG1, class ARG2, class ARG3>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::emplace(const_iterator position,
                               const ARG1& a1,
                               const ARG2& a2,
                               const ARG3& a3)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           a1,
                           a2,
                           a3);
    proctor.release();
    return insert_node(position, p);
}

template <class TYPE, class ALLOCATOR>
  template <class ARG1, class ARG2, class ARG3, class ARG4>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::emplace(const_iterator position,
                               const ARG1& a1,
                               const ARG2& a2,
                               const ARG3& a3,
                               const ARG4& a4)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           a1,
                           a2,
                           a3,
                           a4);
    proctor.release();
    return insert_node(position, p);
}

template <class TYPE, class ALLOCATOR>
  template <class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::emplace(const_iterator position,
                               const ARG1& a1,
                               const ARG2& a2,
                               const ARG3& a3,
                               const ARG4& a4,
                               const ARG5& a5)
{
    NodePtr p = allocate_node();
    NodeProctor proctor(this, p);
    AllocTraits::construct(allocator(),
                           BloombergLP::bsls::Util::addressOf(p->d_value),
                           a1,
                           a2,
                           a3,
                           a4,
                           a5);
    proctor.release();
    return insert_node(position, p);
}

#endif // ! BSLS_SIMULATED_VARIADIC_TEMPLATES

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::insert(const_iterator position, const TYPE& x)
{
    return emplace(position, x);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::insert(const_iterator position, TYPE&& x)
{
    return emplace(position, std::move(x));
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::insert(const_iterator position,
                              size_type n,
                              const TYPE& x)
{
    if (0 == n) {
        return position.unconst();
    }

    // Remember the position of the first insertion
    iterator ret = emplace(position, x);
    for (--n; n > 0; --n) {
        emplace(position, x);
    }

    return ret;
}

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::erase(const_iterator position)
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

template <class TYPE, class ALLOCATOR>
typename list<TYPE, ALLOCATOR>::iterator
list<TYPE, ALLOCATOR>::erase(const_iterator position, const_iterator last)
{
    while (position != last) {
        const_iterator curr = position;
        ++position;
        erase(curr);
    }

    return position.unconst();
}

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::swap(list& other)
{
    using std::swap;

    if (AllocTraits::propagate_on_container_swap::VALUE) {
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

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::clear()
{
    erase(begin(), end());
}

// 23.3.5.5 list operations
template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::splice(const_iterator position, list& x)
{
    BSLS_ASSERT(allocator() == x.allocator());
    if (x.empty()) {
        return;
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

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::splice(const_iterator position,
                                   list& x,
                                   const_iterator i)
{
    BSLS_ASSERT(allocator() == x.allocator());
    typename AllocTraits::pointer pos   = position.d_nodeptr;
    typename AllocTraits::pointer xnode = i.d_nodeptr;
    typename AllocTraits::pointer xnext = xnode->d_next;

    if (pos == xnode || pos == xnext) {
        return;  // Do nothing
    }

    // Splice contents out of x.
    link_nodes(xnode->d_prev, xnext);
    --x.size_ref();

    // Splice contents into *this.
    link_nodes(pos->d_prev, xnode);
    link_nodes(xnode, pos);
    ++size_ref();
}

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::splice(const_iterator position,
                                   list& x,
                                   const_iterator first,
                                   const_iterator last)
{
    BSLS_ASSERT(allocator() == x.allocator());
    size_type n = bsl::distance(first, last);

    if (0 == n) {
        return;
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

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::remove(const TYPE& value)
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

template <class TYPE, class ALLOCATOR>
template <class Predicate>
void list<TYPE, ALLOCATOR>::remove_if(Predicate pred)
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

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::unique()
{
    if (size() < 2) {
        return;
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

template <class TYPE, class ALLOCATOR>
template <class EqPredicate>
void list<TYPE, ALLOCATOR>::unique(EqPredicate binary_pred)
{
    if (size() < 2) {
        return;
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

template <class TYPE, class ALLOCATOR>
  template <class COMPARE>
typename list<TYPE, ALLOCATOR>::NodePtr
list<TYPE, ALLOCATOR>::merge_imp(NodePtr node1,
                                 NodePtr node2,
                                 NodePtr finish,
                                 COMPARE comp)
{
    NodePtr pre     = node1->d_prev;

    // The only possible throwing operation is the comparison functor.
    // Exception neutrality is created by ensuring that the list is in a valid
    // state, with no disconnected nodes, before the comparison functor is
    // called.

    // Having the two sublists be contiguous parts of the same list has
    // the following advantages:
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

            // Advance to next node in the 2nd unmerged sequence
            node2 = next2;
        }
        else {
            // Advance to next node in the 1st unmerged sequence
            node1 = node1->d_next;
        }
    }

    return pre->d_next;
}

template <class TYPE, class ALLOCATOR>
template <class COMPARE>
void list<TYPE, ALLOCATOR>::merge(list& x, COMPARE comp)
{
    if (x.empty()) {
        return;  // Important special case to avoid pointing to sentinel
    }

    // Splice 'x' to the end of '*this', but remember the first node of the
    // appended sequence.
    NodePtr xfirst = x.d_sentinel->d_next;
    splice(end(), x);

    // Call merge_imp with a pointer to the first node of the original list, a
    // pointer to the first node of 'x' (which also ends the original list)
    // and a pointer to the sentinel (which now ends 'x').
    merge_imp(d_sentinel->d_next, xfirst, d_sentinel, comp);
}

template <class TYPE, class ALLOCATOR>
inline
void list<TYPE, ALLOCATOR>::merge(list& x)
{
    merge(x, comp_elems());
}

template <class TYPE, class ALLOCATOR>
  template <class COMPARE>
typename list<TYPE, ALLOCATOR>::NodePtr
list<TYPE, ALLOCATOR>::sort_imp(NodePtr*       pnode1,
                                size_type      size,
                                const COMPARE& comp)
{
    BSLS_ASSERT(size > 0);

    NodePtr node1 = *pnode1;
    if (size < 2) {
        return node1->d_next;
    }

    size_type half = size / 2;

    NodePtr node2 = sort_imp(&node1, half,        comp);
    NodePtr next  = sort_imp(&node2, size - half, comp);

    *pnode1 = merge_imp(node1, node2, next, comp);
    return next;
}

template <class TYPE, class ALLOCATOR>
  template <class COMPARE>
void list<TYPE, ALLOCATOR>::sort(COMPARE comp)
{
    if (size_ref() < 2) {
        return;
    }
    NodePtr node1 = d_sentinel->d_next;
    sort_imp(&node1, size(), comp);
}

template <class TYPE, class ALLOCATOR>
inline
void list<TYPE, ALLOCATOR>::sort()
{
    sort(comp_elems());
}

template <class TYPE, class ALLOCATOR>
void list<TYPE, ALLOCATOR>::reverse()
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

// FREE OPERATORS
template <class TYPE, class ALLOCATOR> inline
bool operator==(const list<TYPE, ALLOCATOR>& lhs,
                const list<TYPE, ALLOCATOR>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

template <class TYPE, class ALLOCATOR> inline
bool operator!=(const list<TYPE, ALLOCATOR>& lhs,
                const list<TYPE, ALLOCATOR>& rhs)
{
    return ! (lhs == rhs);
}

template <class TYPE, class ALLOCATOR> inline
bool operator< (const list<TYPE, ALLOCATOR>& lhs,
                const list<TYPE, ALLOCATOR>& rhs)
{
    return 0 > BloombergLP::bslalg::RangeCompare::lexicographical(lhs.begin(),
                                                                  lhs.end(),
                                                                  lhs.size(),
                                                                  rhs.begin(),
                                                                  rhs.end(),
                                                                  rhs.size());
}

template <class TYPE, class ALLOCATOR> inline
bool operator> (const list<TYPE, ALLOCATOR>& x, const list<TYPE, ALLOCATOR>& y)
{
    return y < x;
}

template <class TYPE, class ALLOCATOR> inline
bool operator>=(const list<TYPE, ALLOCATOR>& x, const list<TYPE, ALLOCATOR>& y)
{
    return ! (x < y);
}

template <class TYPE, class ALLOCATOR> inline
bool operator<=(const list<TYPE, ALLOCATOR>& x, const list<TYPE, ALLOCATOR>& y)
{
    return ! (y < x);
}

// specialized algorithms:
template <class TYPE, class ALLOCATOR>
inline
void swap(list<TYPE, ALLOCATOR>& x, list<TYPE, ALLOCATOR>& y)
{
    x.swap(y);
}

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
