// bslstl_vector.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTL_VECTOR
#define INCLUDED_BSLSTL_VECTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant vector class.
//
//@CLASSES:
//  bsl::vector: STL-compatible vector template
//
//@SEE_ALSO: bslstl_deque
//
//@DESCRIPTION: This component defines a single class template, 'bsl::vector',
// implementing the standard sequential container, 'std::vector', holding a
// dynamic array of values of a template parameter type.
//
// An instantiation of 'vector' is an allocator-aware, value-semantic type
// whose salient attributes are its size (number of values) and the sequence of
// values the vector contains.  If 'vector' is instantiated with a value type
// that is not value-semantic, then the vector will not retain all of its
// value-semantic qualities.  In particular, if a value type cannot be tested
// for equality, then a 'vector' containing objects of that type cannot be
// tested for equality.  It is even possible to instantiate 'vector' with a
// value type that does not have a copy-constructor, in which case the 'vector'
// will not be copyable.
//
// A vector meets the requirements of a sequential container with random access
// iterators in the C++ standard [vector].  The 'vector' implemented here
// adheres to the C++11 standard when compiled with a C++11 compiler, and makes
// the best approximation when compiled with a C++03 compiler.  In particular,
// for C++03 we emulate move semantics, but limit forwarding (in 'emplace') to
// 'const' lvalues, and make no effort to emulate 'noexcept' or initializer
// lists.
//
///Requirements on 'VALUE_TYPE'
///----------------------------
// A 'vector' is a fully Value-Semantic Type (see {'bsldoc_glossary'}) only
// if the supplied 'VALUE_TYPE' template parameter is fully value-semantic.  It
// is possible to instantiate a 'vector' with a 'VALUE_TYPE' parameter that
// does not have a full set of value-semantic operations, but then some methods
// of the container may not be instantiable.  The following terminology,
// adopted from the C++11 standard, is used in the function documentation of
// 'vector' to describe a function's requirements for the 'VALUE_TYPE' template
// parameter.  These terms are also defined in section [17.6.3.1] of the C++11
// standard.  Note that, in the context of a 'vector' instantiation, the
// requirements apply specifically to the vector's entry type, 'value_type',
// which is an alias for 'VALUE_TYPE'.
//
// Legend
// ------
// 'X'    - denotes an allocator-aware container type (e.g., 'vector')
// 'T'    - 'value_type' associated with 'X'
// 'A'    - type of the allocator used by 'X'
// 'm'    - lvalue of type 'A' (allocator)
// 'p'    - address ('T *') of uninitialized storage for a 'T' within an 'X'
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
///Memory Allocation
///-----------------
// The type supplied as a vector's 'ALLOCATOR' template parameter determines
// how that vector will allocate memory.  The 'vector' template supports
// allocators meeting the requirements of the C++03 standard; in addition, it
// supports scoped-allocators derived from the 'bslma::Allocator' memory
// allocation protocol.  Clients intending to use 'bslma'-style allocators
// should use the template's default 'ALLOCATOR' type: The default type for the
// 'ALLOCATOR' template parameter, 'bsl::allocator', provides a C++11
// standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of a 'vector' instantiation'
// is 'bsl::allocator', then objects of that vector type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a vector
// accepts an optional 'bslma::Allocator' argument at construction.  If the
// address of a 'bslma::Allocator' object is explicitly supplied at
// construction, it is used to supply memory for the vector throughout its
// lifetime; otherwise, the vector will use the default allocator installed at
// the time of the vector's construction (see 'bslma_default').  In addition to
// directly allocating memory from the indicated 'bslma::Allocator', a vector
// supplies that allocator's address to the constructors of contained objects
// of the (template parameter) type 'VALUE_TYPE', if it defines the
// 'bslma::UsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'vector':
//..
//  Legend
//  ------
//  'V'             - (template parameter) 'VALUE_TYPE' of the vector
//  'a', 'b'        - two distinct objects of type 'vector<V>'
//  'rv'            - modifiable rvalue of type 'vector<V>'
//  'n', 'm'        - number of elements in 'a' and 'b', respectively
//  'k'             - non-negative integer
//  'al'            - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'V' objects
//  'il'            - object of type 'std::initializer_list<V>'
//  'lil'           - length of 'il'
//  'vt'            - object of type 'VALUE_TYPE'
//  'rvt'           - modifiable rvalue of type 'VALUE_TYPE'
//  'p1', 'p2'      - two 'const' iterators belonging to 'a'
//  distance(i1,i2) - the number of elements in the range [i1, i2)
//
//  |-----------------------------------------+-------------------------------|
//  | Operation                               | Complexity                    |
//  |=========================================+===============================|
//  | vector<V> a      (default construction) | O[1]                          |
//  | vector<V> a(al)                         |                               |
//  |-----------------------------------------+-------------------------------|
//  | vector<V> a(b)   (copy construction)    | O[n]                          |
//  | vector<V> a(b, al)                      |                               |
//  |-----------------------------------------+-------------------------------|
//  | vector<V> a(rv)  (move construction)    | O[1] if 'a' and 'rv' use the  |
//  | vector<V> a(rv, al)                     | same allocator; O[n] otherwise|
//  |-----------------------------------------+-------------------------------|
//  | vector<V> a(k)                          | O[k]                          |
//  | vector<V> a(k, al)                      |                               |
//  | vector<V> a(k, vt)                      |                               |
//  | vector<V> a(k, vt, al)                  |                               |
//  |-----------------------------------------+-------------------------------|
//  | vector<V> a(i1, i2)                     | O[distance(i1, i2)]           |
//  | vector<V> a(i1, i2, al)                 |                               |
//  |-----------------------------------------+-------------------------------|
//  | vector<V> a(il)                         | O[lil]                        |
//  | vector<V> a(il, al)                     |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.~vector<V>()  (destruction)           | O[n]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.assign(k, vt)                         | O[k]                          |
//  | a.assign(k, rvt)                        |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.assign(i1, i2)                        | O[distance(i1, i2)]           |
//  |-----------------------------------------+-------------------------------|
//  | a.assign(il)                            | O[lil]                        |
//  |-----------------------------------------+-------------------------------|
//  | get_allocator()                         | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.begin(), a.end(),                     | O[1]                          |
//  | a.cbegin(), a.cend(),                   |                               |
//  | a.rbegin(), a.rend(),                   |                               |
//  | a.crbegin(), a.crend()                  |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.size()                                | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.max_size()                            | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.resize(k)                             | O[k]                          |
//  | a.resize(k, vt)                         |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.empty()                               | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.reserve(k)                            | O[k]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.shrink_to_fit()                       | O[n]                          |
//  |-----------------------------------------+-------------------------------|
//  | a[k]                                    | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.at(k)                                 | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.front()                               | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.back()                                | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.push_back(vt)                         | O[1]                          |
//  | a.push_back(rvt)                        |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.pop_back()                            | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.emplace_back(args)                    | O[1]                          |
//  |-----------------------------------------+-------------------------------|
//  | a.emplace(p1, args)                     | O[1 + distance(p1, a.end())]  |
//  |-----------------------------------------+-------------------------------|
//  | a.insert(p1, vt)                        | O[1 + distance(p1, a.end())]  |
//  | a.insert(p1, rvt)                       |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.insert(p1, k, vt)                     | O[k + distance(p1, a.end())]  |
//  | a.insert(p1, k, rvt)                    |                               |
//  |-----------------------------------------+-------------------------------|
//  | a.insert(p1, i1, i2)                    | O[distance(i1, i2)            |
//  |                                         |      + distance(p1, a.end())] |
//  |-----------------------------------------+-------------------------------|
//  | a.insert(p1, il)                        | O[lil                         |
//  |                                         |      + distance(p1, a.end())] |
//  |-----------------------------------------+-------------------------------|
//  | a.erase(p1)                             | O[1 + distance(p1, a.end())]  |
//  |-----------------------------------------+-------------------------------|
//  | a.erase(p1, p2)                         | O[distance(p1, p2)            |
//  |                                         |      + distance(p1, a.end())] |
//  |-----------------------------------------+-------------------------------|
//  | a.swap(b), swap(a,b)                    | O[1] if 'a' and 'b' use the   |
//  |                                         | same allocator; O[n + m]      |
//  |                                         | otherwise                     |
//  |-----------------------------------------+-------------------------------|
//  | a.clear()                               | O[n]                          |
//  |-----------------------------------------+-------------------------------|
//  | a = b;           (copy assignment)      | O[n]                          |
//  |-----------------------------------------+-------------------------------|
//  | a = rv;          (move assignment)      | O[1] if 'a' and 'rv' use the  |
//  |                                         | same allocator; O[n] otherwise|
//  |-----------------------------------------+-------------------------------|
//  | a = il;                                 | O[lil]                        |
//  |-----------------------------------------+-------------------------------|
//  | a == b, a != b                          | O[n]                          |
//  |-----------------------------------------+-------------------------------|
//  | a < b, a <= b, a > b, a >= b            | O[n]                          |
//  |-----------------------------------------+-------------------------------|
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Matrix Type
///- - - - - - - - - - - - - - - - -
// Suppose we want to define a value-semantic type representing a dynamically
// resizable two-dimensional matrix.
//
// First, we define the public interface for the 'MyMatrix' class template:
//..
//  template <class TYPE>
//  class MyMatrix {
//      // This value-semantic type characterizes a two-dimensional matrix of
//      // objects of the (template parameter) 'TYPE'.  The numbers of columns
//      // and rows of the matrix can be specified at construction and, at any
//      // time, via the 'reset', 'insertRow', and 'insertColumn' methods.  The
//      // value of each element in the matrix can be set and accessed using
//      // the 'theValue', and 'theModifiableValue' methods respectively.
//
//    public:
//      // PUBLIC TYPES
//..
// Here, we create a type alias, 'RowType', for an instantiation of
// 'bsl::vector' to represent a row of 'TYPE' objects in the matrix.  We create
// another type alias, 'MatrixType', for an instantiation of 'bsl::vector' to
// represent the entire matrix of 'TYPE' objects as a list of rows:
//..
//      typedef bsl::vector<TYPE>    RowType;
//          // This is an alias representing a row of values of the (template
//          // parameter) 'TYPE'.
//
//      typedef bsl::vector<RowType> MatrixType;
//          // This is an alias representing a two-dimensional matrix of values
//          // of the (template parameter) 'TYPE'.
//
//    private:
//      // DATA
//      MatrixType d_matrix;      // matrix of values
//      int        d_numColumns;  // number of columns
//
//      // FRIENDS
//      template <class T>
//      friend bool operator==(const MyMatrix<T>&, const MyMatrix<T>&);
//
//    public:
//      // PUBLIC TYPES
//      typedef typename MatrixType::const_iterator ConstRowIterator;
//
//      // CREATORS
//      MyMatrix(int               numRows,
//               int               numColumns,
//               bslma::Allocator *basicAllocator = 0);
//          // Create a 'MyMatrix' object having the specified 'numRows' and
//          // the specified 'numColumns'.  All elements of the (template
//          // parameter) 'TYPE' in the matrix will have the
//          // default-constructed value.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.  The
//          // behavior is undefined unless '0 <= numRows' and
//          // '0 <= numColumns'
//
//      MyMatrix(const MyMatrix&   original,
//               bslma::Allocator *basicAllocator = 0);
//          // Create a 'MyMatrix' object having the same value as the
//          // specified 'original' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      //! ~MyMatrix = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      MyMatrix& operator=(const MyMatrix& rhs);
//          // Assign to this object the value of the specified 'rhs' object,
//          // and return a reference providing modifiable access to this
//          // object.
//
//      void clear();
//          // Remove all rows and columns from this object.
//
//      void insertRow(int rowIndex);
//          // Insert, into this matrix, a row at the specified 'rowIndex'.
//          // All elements of the (template parameter) 'TYPE' in the row will
//          // have the default-constructed value.  The behavior is undefined
//          // unless '0 <= rowIndex <= numRows()'.
//
//      void insertColumn(int columnIndex);
//          // Insert, into this matrix, an column at the specified
//          // 'columnIndex'.  All elements of the (template parameter) 'TYPE'
//          // in the column will have the default-constructed value.  The
//          // behavior is undefined unless '0 <= columnIndex <= numColumns()'.
//
//      TYPE& theModifiableValue(int rowIndex, int columnIndex);
//          // Return a reference providing modifiable access to the element at
//          // the specified 'rowIndex' and the specified 'columnIndex' in this
//          // matrix.  The behavior is undefined unless
//          // '0 <= rowIndex < numRows()' and
//          // '0 <= columnIndex < numColumns()'.
//
//      // ACCESSORS
//      int numRows() const;
//          // Return the number of rows in this matrix.
//
//      int numColumns() const;
//          // Return the number of columns in this matrix.
//
//      ConstRowIterator beginRow() const;
//          // Return an iterator providing non-modifiable access to the
//          // 'RowType' objects representing the first row in this matrix.
//
//      ConstRowIterator endRow() const;
//          // Return an iterator providing non-modifiable access to the
//          // 'RowType' objects representing the past-the-end row in this
//          // matrix.
//
//      const TYPE& theValue(int rowIndex, int columnIndex) const;
//          // Return a reference providing non-modifiable access to the
//          // element at the specified 'rowIndex' and the specified
//          // 'columnIndex' in this matrix.  The behavior is undefined unless
//          // '0 <= rowIndex < numRows()' and
//          // '0 <= columnIndex < numColumns()'.
//  };
//..
// Then we declare the free operator for 'MyMatrix':
//..
//  // FREE OPERATORS
//  template <class TYPE>
//  MyMatrix<TYPE> operator==(const MyMatrix<TYPE>& lhs,
//                            const MyMatrix<TYPE>& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
//      // value, and 'false' otherwise.  Two 'MyMatrix' objects have the same
//      // value if they have the same number of rows and columns and every
//      // element in both matrices compare equal.
//
//  template <class TYPE>
//  MyMatrix<TYPE> operator!=(const MyMatrix<TYPE>& lhs,
//                            const MyMatrix<TYPE>& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'MyMatrix' objects do
//      // not have the same value if they do not have the same number of rows
//      // and columns or every element in both matrices do not compare equal.
//
//  template <class TYPE>
//  MyMatrix<TYPE> operator*(const MyMatrix<TYPE>& lhs,
//                           const MyMatrix<TYPE>& rhs);
//      // Return a 'MyMatrix' objects that is the product of the specified
//      // 'lhs' and 'rhs'.  The behavior is undefined unless
//      // 'lhs.numColumns() == rhs.numRows()'.
//..
// Now, we define the methods of 'MyMatrix':
//..
//  // CREATORS
//  template <class TYPE>
//  MyMatrix<TYPE>::MyMatrix(int numRows,
//                           int numColumns,
//                           bslma::Allocator *basicAllocator)
//  : d_matrix(numRows, basicAllocator)
//  , d_numColumns(numColumns)
//  {
//      BSLS_ASSERT(0 <= numRows);
//      BSLS_ASSERT(0 <= numColumns);
//
//      for (typename MatrixType::iterator itr = d_matrix.begin();
//           itr != d_matrix.end();
//           ++itr) {
//          itr->resize(d_numColumns);
//      }
//  }
//  template <class TYPE>
//  MyMatrix<TYPE>::MyMatrix(const MyMatrix& original,
//                           bslma::Allocator *basicAllocator)
//  : d_matrix(original.d_matrix, basicAllocator)
//  , d_numColumns(original.d_numColumns)
//  {
//  }
//..
// Notice that we pass the contained 'bsl::vector' ('d_matrix') the allocator
// specified at construction to supply memory.  If the (template parameter)
// 'TYPE' of the elements has the 'bslalg_TypeTraitUsesBslmaAllocator' trait,
// this allocator will be passed by the vector to the elements as well.
//..
//  // MANIPULATORS
//  template <class TYPE>
//  MyMatrix<TYPE>& MyMatrix<TYPE>::operator=(const MyMatrix& rhs)
//  {
//      d_matrix = rhs.d_matrix;
//      d_numColumns = rhs.d_numColumns;
//  }
//
//  template <class TYPE>
//  void MyMatrix<TYPE>::clear()
//  {
//      d_matrix.clear();
//      d_numColumns = 0;
//  }
//
//  template <class TYPE>
//  void MyMatrix<TYPE>::insertRow(int rowIndex)
//  {
//      typename MatrixType::iterator itr =
//          d_matrix.insert(d_matrix.begin() + rowIndex, RowType());
//      itr->resize(d_numColumns);
//  }
//
//  template <class TYPE>
//  void MyMatrix<TYPE>::insertColumn(int colIndex) {
//      for (typename MatrixType::iterator itr = d_matrix.begin();
//           itr != d_matrix.end();
//           ++itr) {
//          itr->insert(itr->begin() + colIndex, TYPE());
//      }
//      ++d_numColumns;
//  }
//
//  template <class TYPE>
//  TYPE& MyMatrix<TYPE>::theModifiableValue(int rowIndex, int columnIndex)
//  {
//      BSLS_ASSERT(0 <= rowIndex);
//      BSLS_ASSERT(rowIndex < d_matrix.size());
//      BSLS_ASSERT(0 <= columnIndex);
//      BSLS_ASSERT(columnIndex < d_numColumns);
//
//      return d_matrix[rowIndex][columnIndex];
//  }
//
//  // ACCESSORS
//  template <class TYPE>
//  int MyMatrix<TYPE>::numRows() const
//  {
//      return d_matrix.size();
//  }
//
//  template <class TYPE>
//  int MyMatrix<TYPE>::numColumns() const
//  {
//      return d_numColumns;
//  }
//
//  template <class TYPE>
//  typename MyMatrix<TYPE>::ConstRowIterator MyMatrix<TYPE>::beginRow() const
//  {
//      return d_matrix.begin();
//  }
//
//  template <class TYPE>
//  typename MyMatrix<TYPE>::ConstRowIterator MyMatrix<TYPE>::endRow() const
//  {
//      return d_matrix.end();
//  }
//
//  template <class TYPE>
//  const TYPE& MyMatrix<TYPE>::theValue(int rowIndex, int columnIndex) const
//  {
//      BSLS_ASSERT(0 <= rowIndex);
//      BSLS_ASSERT(rowIndex < d_matrix.size());
//      BSLS_ASSERT(0 <= columnIndex);
//      BSLS_ASSERT(columnIndex < d_numColumns);
//
//      return d_matrix[rowIndex][columnIndex];
//  }
//..
// Finally, we defines the free operators for 'MyMatrix':
//..
//  // FREE OPERATORS
//  template <class TYPE>
//  MyMatrix<TYPE> operator==(const MyMatrix<TYPE>& lhs,
//                            const MyMatrix<TYPE>& rhs)
//  {
//      return lhs.d_numColumns == rhs.d_numColumns &&
//                                                lhs.d_matrix == rhs.d_matrix;
//  }
//
//  template <class TYPE>
//  MyMatrix<TYPE> operator!=(const MyMatrix<TYPE>& lhs,
//                            const MyMatrix<TYPE>& rhs)
//  {
//      return !(lhs == rhs);
//  }
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_vector.h> instead of <bslstl_vector.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#include <bslalg_arraydestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYPRIMITIVES
#include <bslalg_arrayprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_CONTAINERBASE
#include <bslalg_containerbase.h>
#endif

#ifndef INCLUDED_BSLALG_RANGECOMPARE
#include <bslalg_rangecompare.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLH_HASH
#include <bslh_hash.h>
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

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHARITHMETICTYPE
#include <bslmf_matcharithmetictype.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_CPP11
#include <bsls_cpp11.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLSTL_HASH
#include <bslstl_hash.h>
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#ifndef INCLUDED_INITIALIZER_LIST
#include <initializer_list>
#define INCLUDED_INITIALIZER_LIST
#endif

#endif
#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_STDEXCEPT
#include <stdexcept>
#define INCLUDED_STDEXCEPT
#endif

#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) || defined(BSLS_PLATFORM_CMP_SUN)
# define BSLSTL_VECTOR_INCONSISTENT_CV_FOR_FUNCTION_POINTERS 1
// Both Microsoft Visual C++ and the Sun CC 12.4 compiler have a bug where
// partial specialization of templates matches function-pointers as 'T *' but
// overload partial-ordering prefers to match as 'const T *'.  This means that
// free-function overloads will dispatch to members of the wrong class template
// specialization.  Note that this bug is new to Sun CC 12.4, and we have not
// tested for whether the bug is fixed in later Microsoft compilers.  However,
// defining this macro universally for these compilers does not lead to any new
// problems, as the workarounds we deploy are backwards compatible.
#endif

namespace bsl {

                          // ==================
                          // struct Vector_Util
                          // ==================

struct Vector_Util {
    // This 'struct' provides a namespace for implementing the 'swap' member
    // function of 'Vector_Imp<VALUE_TYPE, ALLOCATOR>'.  'swap' can be
    // implemented irrespective of the 'VALUE_TYPE' or 'ALLOCATOR' template
    // parameters, which is why we implement it in this non-parameterized,
    // non-inlined utility.

    // CLASS METHODS
    static std::size_t computeNewCapacity(std::size_t newLength,
                                          std::size_t capacity,
                                          std::size_t maxSize);
        // Return a capacity that is at least the specified 'newLength' and at
        // least the minimum of twice the specified 'capacity' and the
        // specified 'maxSize'.  The behavior is undefined unless
        // 'capacity < newLength' and 'newLength <= maxSize'.  Note that the
        // returned value is always at most 'maxSize'.

    static void swap(void *a, void *b);
        // Exchange the value of the specified 'a' vector with that of the
        // specified 'b' vector.
};

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

                          // =======================
                          // class Vector_RangeCheck
                          // =======================

template <class BSLSTL_ITERATOR, bool BSLSTL_NOTSPECIALIZED
                   = BloombergLP::bslmf::IsFundamental<BSLSTL_ITERATOR>::value>
struct Vector_DeduceIteratorCategory {
    typedef typename bsl::iterator_traits<BSLSTL_ITERATOR>::iterator_category
                                                                          type;
};

template <class BSLSTL_ITERATOR>
struct Vector_DeduceIteratorCategory<BSLSTL_ITERATOR, true> {
    typedef BloombergLP::bslmf::Nil type;
};


template <class BSLSTL_ITERATOR>
struct Vector_IsRandomAccessIterator :
    bsl::is_same<
        typename Vector_DeduceIteratorCategory<BSLSTL_ITERATOR>::type,
                                         bsl::random_access_iterator_tag>::type
{
};


struct Vector_RangeCheck {
    // This utility class provides a test-support facility to diagnose when a
    // pair of iterators do *not* form a valid range.  This support is offered
    // only for random access iterators, and identifies only the case of two
    // valid iterators into the same range forming a "reverse" range.  Note
    // that the two functions declared using 'enable_if' must be defined inline
    // in the class definition due to a bug in the Microsoft C++ compiler (see
    // 'bslmf_enableif').

    // CLASS METHODS
    template <class BSLSTL_ITERATOR>
    static
    typename bsl::enable_if<
           !Vector_IsRandomAccessIterator<BSLSTL_ITERATOR>::VALUE, bool>::type
    isInvalidRange(BSLSTL_ITERATOR, BSLSTL_ITERATOR)
        // Return 'false'.  Note that we know of no way to identify an input
        // iterator range that is guaranteed to be invalid.
    {
        return false;
    }

    template <class BSLSTL_ITERATOR>
    static
    typename bsl::enable_if<
           Vector_IsRandomAccessIterator<BSLSTL_ITERATOR>::VALUE, bool>::type
    isInvalidRange(BSLSTL_ITERATOR first, BSLSTL_ITERATOR last)
        // Return 'true' if 'last < first', and 'false' otherwise.  The
        // behavior is undefined unless both 'first' and 'last' are valid
        // iterators that refer to the same range.
    {
        return last < first;
    }
};

#endif

                          // ====================
                          // class Vector_ImpBase
                          // ====================

template <class VALUE_TYPE>
class Vector_ImpBase {
    // This class describes the basic layout for a vector class, to be included
    // into the 'Vector_Imp' layout *before* the allocator (provided by
    // 'bslalg::ContainerBase') to take better advantage of cache prefetching.
    // It is parameterized by 'VALUE_TYPE' only, and implements the portion of
    // 'Vector_Imp' that does not need to know about its (template parameter)
    // type 'ALLOCATOR' (in order to generate shorter debug strings).  This
    // class intentionally has *no* creators (other than the compiler-generated
    // ones).

    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil                 MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

  protected:
    // DATA
    VALUE_TYPE  *d_dataBegin_p;  // beginning of data storage (owned)
    VALUE_TYPE  *d_dataEnd_p;    // one past the end of data storage
    std::size_t  d_capacity;     // capacity of data storage in # of elements

  public:
    // PUBLIC TYPES
    typedef VALUE_TYPE&                            reference;
    typedef VALUE_TYPE const&                      const_reference;
    typedef VALUE_TYPE                            *iterator;
    typedef VALUE_TYPE const                      *const_iterator;
    typedef std::size_t                            size_type;
    typedef std::ptrdiff_t                         difference_type;
    typedef VALUE_TYPE                             value_type;
    typedef bsl::reverse_iterator<iterator>        reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

  public:
    // CREATORS
    Vector_ImpBase();
        // Create an empty base object with no capacity.

    // MANIPULATORS
    void adopt(BloombergLP::bslmf::MovableRef<Vector_ImpBase> base);
        // Adopt all outstanding memory allocations associated with the
        // specified 'base' object.  The behavior is undefined unless this
        // object is in a default-constructed state.

                             // *** iterators ***

    iterator begin() BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing modifiable access to the first element
        // in this vector, or the past-the-end iterator if this vector is
        // empty.

    iterator end() BSLS_CPP11_NOEXCEPT;
        // Return the past-the-end iterator providing modifiable access to this
        // vector.

    reverse_iterator rbegin() BSLS_CPP11_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // element in this vector, and the past-the-end reverse iterator if
        // this vector is empty.

    reverse_iterator rend() BSLS_CPP11_NOEXCEPT;
        // Return the past-the-end reverse iterator providing modifiable access
        // to this vector.

                          // *** element access ***

    reference operator[](size_type position);
        // Return a reference providing modifiable access to the element at the
        // specified 'position' in this vector.  The behavior is undefined
        // unless 'position < size()'.

    reference at(size_type position);
        // Return a reference providing modifiable access to the element at the
        // specified 'position' in this vector.  Throw a 'bsl::out_of_range'
        // exception if 'position >= size()'.

    reference front();
        // Return a reference providing modifiable access to the first element
        // in this vector.  The behavior is undefined unless this vector is not
        // empty.

    reference back();
        // Return a reference providing modifiable access to the last element
        // in this vector.  The behavior is undefined unless this vector is not
        // empty.

    VALUE_TYPE *data() BSLS_CPP11_NOEXCEPT;
        // Return the address of the modifiable first element in this vector,
        // or a valid, but non-dereferenceable pointer value if this vector is
        // empty.

    // ACCESSORS

                             // *** iterators ***

    const_iterator  begin() const BSLS_CPP11_NOEXCEPT;
    const_iterator cbegin() const BSLS_CPP11_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // element in this vector, and the past-the-end iterator if this vector
        // is empty.

    const_iterator  end() const BSLS_CPP11_NOEXCEPT;
    const_iterator cend() const BSLS_CPP11_NOEXCEPT;
        // Return the past-the-end (forward) iterator providing non-modifiable
        // access to this vector.

    const_reverse_iterator  rbegin() const BSLS_CPP11_NOEXCEPT;
    const_reverse_iterator crbegin() const BSLS_CPP11_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last element in this vector, and the past-the-end reverse iterator
        // if this vector is empty.

    const_reverse_iterator  rend() const BSLS_CPP11_NOEXCEPT;
    const_reverse_iterator crend() const BSLS_CPP11_NOEXCEPT;
        // Return the past-the-end reverse iterator providing non-modifiable
        // access to this vector.

                            // *** capacity ***

    size_type size() const BSLS_CPP11_NOEXCEPT;
        // Return the number of elements in this vector.

    size_type capacity() const BSLS_CPP11_NOEXCEPT;
        // Return the capacity of this vector, i.e., the maximum number of
        // elements for which resizing is guaranteed not to trigger a
        // reallocation.

    bool empty() const BSLS_CPP11_NOEXCEPT;
        // Return 'true' if this vector has size 0, and 'false' otherwise.

                          // *** element access ***

    const_reference operator[](size_type position) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'position' in this vector.  The behavior is undefined
        // unless 'position < size()'.

    const_reference at(size_type position) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'position' in this vector.  Throw a
        // 'bsl::out_of_range' exception if 'position >= size()'.

    const_reference front() const;
        // Return a reference providing non-modifiable access to the first
        // element in this vector.  The behavior is undefined unless this
        // vector is not empty.

    const_reference back() const;
        // Return a reference providing non-modifiable access to the last
        // element in this vector.  The behavior is undefined unless this
        // vector is not empty.

    const VALUE_TYPE *data() const BSLS_CPP11_NOEXCEPT;
        // Return the address of the non-modifiable first element in this
        // vector, or a valid, but non-dereferenceable pointer value if this
        // vector is empty.

};

                        // ================
                        // class Vector_Imp
                        // ================

template <class VALUE_TYPE, class ALLOCATOR = bsl::allocator<VALUE_TYPE> >
class Vector_Imp : public Vector_ImpBase<VALUE_TYPE>
                 , private BloombergLP::bslalg::ContainerBase<ALLOCATOR> {
    // This class template provides an STL-compliant 'vector' that conforms to
    // the 'bslma::Allocator' model.  For the requirements of a vector class,
    // consult the C++11 standard.  In particular, this implementation offers
    // the general rules that:
    //..
    //: 1 A call to any method that would result in a vector having a size
    //:   or capacity greater than the value returned by 'max_size' triggers a
    //:   call to 'bslstl::StdExceptUtil::throwLengthError'.
    //:
    //: 2 A call to an 'at' method that attempts to access a position outside
    //:   of the valid range of a vector triggers a call to
    //:   'bslstl::StdExceptUtil::throwOutOfRange'.
    //..
    // Note that portions of the standard methods are implemented in
    // 'Vector_ImpBase', which is parameterized on only 'VALUE_TYPE' in order
    // to generate smaller debug strings.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'BDEX' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.
    //
    // In addition, the following members offer a full guarantee of rollback:
    // if an exception is thrown during the invocation of 'push_back' or
    // 'insert' with a single element at the end of a pre-existing object, the
    // object is left in a valid state and its value is unchanged.

    // PRIVATE TYPES
    typedef BloombergLP::bslalg::ArrayPrimitives               ArrayPrimitives;
        // This 'typedef' is an alias for a utility class that provides many
        // useful functions that operate on arrays.

    typedef BloombergLP::bslmf::MovableRefUtil                 MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

    typedef bsl::allocator_traits<ALLOCATOR>                   AllocatorTraits;
        // This 'typedef' is an alias for the allocator traits type associated
        // with this container.

  public:
    // PUBLIC TYPES:
    typedef VALUE_TYPE                                value_type;
    typedef ALLOCATOR                                 allocator_type;
    typedef typename ALLOCATOR::reference             reference;
    typedef typename ALLOCATOR::const_reference       const_reference;

    typedef typename AllocatorTraits::size_type       size_type;
    typedef typename AllocatorTraits::difference_type difference_type;
    typedef typename AllocatorTraits::pointer         pointer;
    typedef typename AllocatorTraits::const_pointer   const_pointer;

    typedef VALUE_TYPE                               *iterator;
    typedef VALUE_TYPE const                         *const_iterator;
    typedef bsl::reverse_iterator<iterator>           reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>     const_reverse_iterator;

  private:
    // PRIVATE TYPES
    typedef Vector_ImpBase<VALUE_TYPE>                    ImpBase;
        // Implementation base type, with iterator-related functionality.

    typedef BloombergLP::bslalg::ContainerBase<ALLOCATOR> ContainerBase;
        // Container base type, containing the allocator and applying the empty
        // base class optimization (EBO) whenever appropriate.

    class Proctor {
        // This class provides a proctor for deallocating an array of
        // 'VALUE_TYPE' objects, to be used in the 'Vector_Imp' constructors.

        // DATA
        VALUE_TYPE          *d_data_p;       // array pointer
        std::size_t          d_capacity;     // capacity of the array
        ContainerBase       *d_container_p;  // container base pointer

      private:
        // NOT IMPLEMENTED
        Proctor(const Proctor&);
        Proctor& operator=(const Proctor&);

      public:
        // CREATORS
        Proctor(VALUE_TYPE    *data,
                std::size_t    capacity,
                ContainerBase *container);
            // Create a proctor for the specified 'data' array of the specified
            // 'capacity', using the 'deallocateN' method of the specified
            // 'container' to return 'data' to its allocator upon destruction,
            // unless this proctor's 'release' is called prior.

        ~Proctor();
            // Destroy this proctor, deallocating any data under management.

        // MANIPULATORS
        void release();
            // Release the data from management by this proctor.
    };

    // PRIVATE MANIPULATORS
    template <class INPUT_ITER>
    void privateInsertDispatch(
                              const_iterator                          position,
                              INPUT_ITER                              count,
                              INPUT_ITER                              value,
                              BloombergLP::bslmf::MatchArithmeticType ,
                              BloombergLP::bslmf::Nil                 );
        // Match integral type for 'INPUT_ITER'.

    template <class INPUT_ITER>
    void privateInsertDispatch(const_iterator              position,
                               INPUT_ITER                  first,
                               INPUT_ITER                  last,
                               BloombergLP::bslmf::MatchAnyType ,
                               BloombergLP::bslmf::MatchAnyType );
        // Match non-integral type for 'INPUT_ITER'.

    template <class INPUT_ITER>
    void privateInsert(const_iterator position,
                       INPUT_ITER     first,
                       INPUT_ITER     last,
                       const          std::input_iterator_tag&);
        // Specialized insertion for input iterators.

    template <class FWD_ITER>
    void privateInsert(const_iterator position,
                       FWD_ITER       first,
                       FWD_ITER       last,
                       const          std::forward_iterator_tag&);
        // Specialized insertion for forward, bidirectional, and random-access
        // iterators.

    void privateMoveInsert(Vector_Imp     *fromVector,
                           const_iterator  position);
        // Destructive move insertion from a temporary vector, to avoid
        // duplicate copies after importing from an input iterator into a
        // temporary vector.

    void privateReserveEmpty(size_type numElements);
        // Reserve exactly the specified 'numElements'.  The behavior is
        // undefined unless this vector is empty and has no capacity.

  public:
    // CREATORS

                      // *** construct/copy/destroy ***

    explicit
    Vector_Imp(const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create an empty vector.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is not specified, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // and 'basicAllocator' is not supplied, the currently installed
        // default allocator is used.  Note that a 'bslma::Allocator *' can be
        // supplied for 'basicAllocator' if the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    explicit
    Vector_Imp(size_type         initialSize,
               const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create a vector of the specified 'initialSize' whose every element
        // is a default-constructed object of the (template parameter) type
        // 'VALUE_TYPE'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not specified, a default-constructed
        // object of the (template parameter) type 'ALLOCATOR' is used.  If the
        // type 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used.  Throw
        // 'std::length_error' if 'initialSize > max_size()'.  This method
        // requires that the type 'VALUE_TYPE' be 'default-insertable' into
        // this vector (see {Requirements on 'VALUE_TYPE'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).

    Vector_Imp(size_type         initialSize,
               const VALUE_TYPE& value,
               const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create a vector of the specified 'initialSize' whose every element
        // is a copy of the specified 'value'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  Throw 'std::length_error' if
        // 'initialSize > max_size()'.  This method requires that the (template
        // parameter) type 'VALUE_TYPE' be 'copy-insertable' into this vector
        // (see {Requirements on 'VALUE_TYPE'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).

    template <class INPUT_ITER>
    Vector_Imp(INPUT_ITER       first,
               INPUT_ITER       last,
               const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create a vector, and insert (in order) each 'VALUE_TYPE' object in
        // the range starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // not specified, a default-constructed object of the (template
        // parameter) type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  Throw 'std::length_error' if
        // the number of elements in '[first .. last)' exceeds the value
        // returned by the method 'max_size'.  The (template parameter) type
        // 'INPUT_ITER' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type', and 'value_type' must be
        // 'emplace-constructible' from '*i' into this vector, where 'i' is a
        // dereferenceable iterator in the range '[first .. last)' (see
        // {Requirements on 'VALUE_TYPE'}).  The behavior is undefined unless
        // 'first' and 'last' refer to a range of valid values where 'first'
        // is at a position at or before 'last'.  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).

    Vector_Imp(const Vector_Imp& original);
        // Create a vector having the same value as the specified 'original'
        // object.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  This method requires that the (template parameter)
        // type 'VALUE_TYPE' be 'copy-insertable' into this vector (see
        // {Requirements on 'VALUE_TYPE'}).

    Vector_Imp(BloombergLP::bslmf::MovableRef<Vector_Imp> original);// IMPLICIT
        // Create a vector having the same value as the specified 'original'
        // object by moving (in constant time) the contents of 'original' to
        // the new vector.  The allocator associated with 'original' is
        // propagated for use in the newly-created vector.  'original' is left
        // in a valid but unspecified state.

    Vector_Imp(const Vector_Imp& original, const ALLOCATOR& basicAllocator);
        // Create a vector having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // This method requires that the (template parameter) type 'VALUE_TYPE'
        // be 'copy-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).  Note that a 'bslma::Allocator *' can be supplied
        // for 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    Vector_Imp(BloombergLP::bslmf::MovableRef<Vector_Imp> original,
               const ALLOCATOR&                           basicAllocator);
        // Create a vector having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // The contents of 'original' are moved (in constant time) to the new
        // vector if 'basicAllocator == original.get_allocator()', and are
        // move-inserted (in linear time) using 'basicAllocator' otherwise.
        // 'original' is left in a valid but unspecified state.  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'move-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).  Note that a 'bslma::Allocator *' can be supplied
        // for 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    ~Vector_Imp();
        // Destroy this vector.

    // MANIPULATORS
    Vector_Imp& operator=(const Vector_Imp& rhs);
        // Assign to this object the value of the specified 'rhs' object,
        // propagate to this object the allocator of 'rhs' if the 'ALLOCATOR'
        // type has trait 'propagate_on_container_copy_assignment', and return
        // a reference providing modifiable access to this object.  If an
        // exception is thrown, '*this' is left in a valid but unspecified
        // state.  This method requires that the (template parameter) type
        // 'VALUE_TYPE' be 'copy-assignable' and 'copy-insertable' into this
        // vector (see {Requirements on 'VALUE_TYPE'}).

    Vector_Imp& operator=(BloombergLP::bslmf::MovableRef<Vector_Imp> rhs);
        // Assign to this object the value of the specified 'rhs' object,
        // propagate to this object the allocator of 'rhs' if the 'ALLOCATOR'
        // type has trait 'propagate_on_container_move_assignment', and return
        // a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this vector if
        // 'get_allocator() == rhs.get_allocator()' (after accounting for the
        // aforementioned trait); otherwise, all elements in this vector are
        // either destroyed or move-assigned to and each additional element in
        // 'rhs' is move-inserted into this vector.  'rhs' is left in a valid
        // but unspecified state, and if an exception is thrown, '*this' is
        // left in a valid but unspecified state.  This method requires that
        // the (template parameter) type 'VALUE_TYPE' be 'move-assignable' and
        // 'move-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last);
        // Assign to this object the value resulting from first clearing this
        // vector and then inserting (in order) each 'value_type' object in the
        // range starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  If an exception is
        // thrown, '*this' is left in a valid but unspecified state.  Throw
        // 'std::length_error' if 'distance(first,last) > max_size()'. The
        // (template parameter) type 'INPUT_ITER' shall meet the requirements
        // of an input iterator defined in the C++11 standard [24.2.3]
        // providing access to values of a type convertible to 'value_type',
        // and 'value_type' must be 'emplace-constructible' from '*i' into this
        // vector, where 'i' is a dereferenceable iterator in the range
        // '[first .. last)' (see {Requirements on 'VALUE_TYPE'}).  The
        // behavior is undefined unless 'first' and 'last' refer to a range of
        // valid values where 'first' is at a position at or before 'last'.

    void assign(size_type numElements, const VALUE_TYPE& value);
        // Assign to this object the value resulting from first clearing this
        // vector and then inserting the specified 'numElements' copies of the
        // specified 'value'.  If an exception is thrown, '*this' is left in a
        // valid but unspecified state.  Throw 'std::length_error' if
        // 'numElements > max_size()'.  This method requires that the (template
        // parameter) type 'VALUE_TYPE' be 'copy-insertable' into this vector
        // (see {Requirements on 'VALUE_TYPE'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void assign(std::initializer_list<VALUE_TYPE> values);
        // Assign to this object the value resulting from first clearing this
        // vector and then inserting (in order) each 'value_type' object in the
        // specified 'values' initializer list.  If an exception is thrown,
        // '*this' is left in a valid but unspecified state.  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'copy-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).
#endif

                             // *** capacity ***

    void resize(size_type newSize);
        // Change the size of this vector to the specified 'newSize'.  If
        // 'newSize < size()', the elements in the range '[newSize .. size())'
        // are erased, and this function does not throw.  If
        // 'newSize > size()', the (newly created) elements in the range
        // '[size() .. newSize)' are default-constructed 'value_type' objects,
        // and if an exception is thrown (other than by the move constructor of
        // a non-copy-insertable 'value_type'), '*this' is unaffected.  Throw
        // 'std::length_error' if 'newSize > max_size()'.  This method requires
        // that the (template parameter) type 'VALUE_TYPE' be
        // 'default-insertable' and 'move-insertable' into this vector (see
        // {Requirements on 'VALUE_TYPE'}).

    void resize(size_type newSize, const VALUE_TYPE& value);
        // Change the size of this vector to the specified 'newSize', inserting
        // 'newSize - size()' copies of the specified 'value' at the end of
        // this vector if 'newSize > size()'.  If 'newSize < size()', the
        // elements in the range '[newSize .. size())' are erased, 'value' is
        // ignored, and this method does not throw.  If 'newSize > size()' and
        // an exception is thrown, '*this' is unaffected.  Throw
        // 'std::length_error' if 'newSize > max_size()'.  This method requires
        // that the (template parameter) type 'VALUE_TYPE' be 'copy-insertable'
        // into this vector (see {Requirements on 'VALUE_TYPE'}).

    void reserve(size_type newCapacity);
        // Change the capacity of this vector to the specified 'newCapacity'.
        // If an exception is thrown (other than by the move constructor of a
        // non-copy-insertable 'value_type'), '*this' is unaffected.  Throw
        // 'bsl::length_error' if 'newCapacity > max_size()'.  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'move-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).  Note that the capacity of this vector after this
        // operation has completed may be greater than 'newCapacity'.

    void shrink_to_fit();
        // Reduce the capacity of this vector to its size.  If an exception is
        // thrown (other than by the move constructor of a non-copy-insertable
        // 'value_type'), '*this' is unaffected.  Note that this method has no
        // effect if the capacity is equivalent to the size.

                            // *** modifiers ***

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    void emplace_back(Args&&... arguments);
        // Append to the end of this vector a newly created 'value_type'
        // object, constructed by forwarding 'get_allocator()' (if required)
        // and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'value_type'.  If an exception is
        // thrown (other than by the move constructor of a non-copy-insertable
        // 'value_type'), '*this' is unaffected.  This method requires that the
        // (template parameter) type 'VALUE_TYPE' be 'move-insertable' into
        // this vector and 'emplace-constructible' from 'arguments' (see
        // {Requirements on 'VALUE_TYPE'}).
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_vector.h
    void emplace_back();

    template <class Args_01>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class Args_01,
              class Args_02>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class Args_01,
              class Args_02,
              class Args_03>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09,
              class Args_10>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... Args>
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif

    void push_back(const VALUE_TYPE& value);
        // Append to the end of this vector a copy of the specified 'value'.
        // If an exception is thrown, '*this' is unaffected.  Throw
        // 'std::length_error' if 'size() == max_size()'.  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'copy-constructible' (see {Requirements on 'VALUE_TYPE'}).

    void push_back(BloombergLP::bslmf::MovableRef<VALUE_TYPE> value);
        // Append to the end of this vector the specified move-insertable
        // 'value'.  'value' is left in a valid but unspecified state.  If
        // an exception is thrown (other than by the move constructor of a
        // non-copy-insertable 'value_type'), '*this' is unaffected.  Throw
        // 'std::length_error' if 'size() == max_size()'.  This method requires
        // that the (template parameter) type 'VALUE_TYPE' be 'move-insertable'
        // into this vector (see {Requirements on 'VALUE_TYPE'}).

    void pop_back();
        // Erase the last element from this vector.  The behavior is undefined
        // if this vector is empty.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    iterator emplace(const_iterator position, Args&&... arguments)
        // Insert at the specified 'position' in this vector a newly created
        // 'value_type' object, constructed by forwarding 'get_allocator()' (if
        // required) and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'value_type', and return an iterator
        // referring to the newly created and inserted element.  If an
        // exception is thrown (other than by the copy constructor, move
        // constructor, assignment operator, or move assignment operator of
        // 'value_type'), '*this' is unaffected.  Throw 'std::length_error' if
        // 'size() == max_size()'.  The behavior is undefined unless 'position'
        // is an iterator in the range '[begin() .. end()]' (both endpoints
        // included).  This method requires that the (template parameter) type
        // 'VALUE_TYPE' be 'move-insertable' into this vector and
        // 'emplace-constructible' from 'arguments' (see {Requirements on
        // 'VALUE_TYPE'}).
        //
        // NOTE: This function has been implemented inline due to an issue with
        // the Sun compiler.
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_vector.h
    iterator emplace(const_iterator position)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator());

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator());
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01,
              class Args_02>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01,
              class Args_02,
              class Args_03>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09,
              class Args_10>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... Args>
    iterator emplace(const_iterator position,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());

        const size_type index = position - this->begin();

        const iterator& pos = const_cast<const iterator&>(position);

        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 >
                                                  maxSize - this->size())) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::emplace(pos,arguments): vector too long");
        }

        const size_type newSize = this->size() + 1;
        if (newSize > this->d_capacity) {
            size_type newCapacity = Vector_Util::computeNewCapacity(
                                           newSize, this->d_capacity, maxSize);
            Vector_Imp temp(this->get_allocator());
            temp.privateReserveEmpty(newCapacity);

            ArrayPrimitives::destructiveMoveAndEmplace(
                temp.d_dataBegin_p,
                &this->d_dataEnd_p,
                this->d_dataBegin_p,
                pos,
                this->d_dataEnd_p,
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

            temp.d_dataEnd_p += newSize;
            Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        }
        else {
            ArrayPrimitives::emplace(
                pos,
                this->end(),
                ContainerBase::allocator(),
                BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
            ++this->d_dataEnd_p;
        }

        return this->begin() + index;
    }
// }}} END GENERATED CODE
#endif

    iterator insert(const_iterator position, const VALUE_TYPE& value);
        // Insert at the specified 'position' in this vector a copy of the
        // specified 'value', and return an iterator referring to the newly
        // inserted element.  If an exception is thrown (other than by the copy
        // constructor, move constructor, assignment operator, or move
        // assignment operator of 'value_type'), '*this' is unaffected.  Throw
        // 'std::length_error' if 'size() == max_size()'.  The behavior is
        // undefined unless 'position' is an iterator in the range
        // '[begin() .. end()]' (both endpoints included).  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'copy-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).

    iterator insert(const_iterator                             position,
                    BloombergLP::bslmf::MovableRef<VALUE_TYPE> value);
        // Insert at the specified 'position' in this vector the specified
        // move-insertable 'value', and return an iterator referring to the
        // newly inserted element.  'value' is left in a valid but unspecified
        // state.  If an exception is thrown (other than by the copy
        // constructor, move constructor, assignment operator, or move
        // assignment operator of 'value_type'), 'this' is unaffected.  Throw
        // 'std::length_error' if 'size() == max_size()'.  The behavior is
        // undefined unless 'position' is an iterator in the range
        // '[begin() .. end()]' (both endpoints included).  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'move-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).

    iterator insert(const_iterator    position,
                    size_type         numElements,
                    const VALUE_TYPE& value);
        // Insert at the specified 'position' in this vector the specified
        // 'numElements' copies of the specified 'value', and return an
        // iterator referring to the first newly inserted element.  If an
        // exception is thrown (other than by the copy constructor, move
        // constructor, assignment operator, or move assignment operator of
        // 'value_type'), '*this' is unaffected.  Throw 'std::length_error' if
        // 'size() + numElements > max_size()'.  The behavior is undefined
        // unless 'position' is an iterator in the range '[begin() .. end()]'
        // (both endpoints included).  This method requires that the (template
        // parameter) type 'VALUE_TYPE' be 'copy-insertable' into this vector
        // (see {Requirements on 'VALUE_TYPE'}).

    template <class INPUT_ITER>
    iterator insert(const_iterator position, INPUT_ITER first, INPUT_ITER last)
        // Insert at the specified 'position' in this vector the values in the
        // range starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Return an iterator
        // referring to the first newly inserted element.  If an exception is
        // thrown (other than by the copy constructor, move constructor,
        // assignment operator, or move assignment operator of 'value_type'),
        // '*this' is unaffected.  Throw 'std::length_error' if
        // 'size() + distance(first, last) > max_size()'.  The (template
        // parameter) type 'INPUT_ITER' shall meet the requirements of an input
        // iterator defined in the C++11 standard [24.2.3] providing access to
        // values of a type convertible to 'value_type', and 'value_type' must
        // be 'emplace-constructible' from '*i' into this vector, where 'i' is
        // a dereferenceable iterator in the range '[first .. last)' (see
        // {Requirements on 'VALUE_TYPE'}).  The behavior is undefined unless
        // 'position' is an iterator in the range '[begin() .. end()]' (both
        // endpoints included), and 'first' and 'last' refer to a range of
        // valid values where 'first' is at a position at or before 'last'.
        //
        // NOTE: This function has been implemented inline due to an issue with
        // the Sun compiler.
    {
        BSLS_ASSERT_SAFE(this->begin() <= position);
        BSLS_ASSERT_SAFE(position      <= this->end());
        BSLS_ASSERT_SAFE(!Vector_RangeCheck::isInvalidRange(first, last));

        // If 'first' and 'last' are integral, then they are not iterators and
        // we should call 'insert(position, first, last)', where 'first' is
        // actually a misnamed count, and 'last' is a misnamed value.  We can
        // assume that any fundamental type passed to this function is integral
        // or else compilation errors will result.  The extra argument,
        // 'bslmf::Nil()', is to avoid an overloading ambiguity: In case
        // 'first' is an integral type, it would be convertible both to
        // 'bslmf::MatchArithmeticType' and 'bslmf::MatchAnyType'; but the
        // 'bslmf::Nil()' will be an exact match to 'bslmf::Nil', so the
        // overload with 'bslmf::MatchArithmeticType' will be preferred.

        const size_type index = position - this->begin();
        privateInsertDispatch(
            position, first, last, first, BloombergLP::bslmf::Nil());
        return this->begin() + index;
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    iterator insert(const_iterator                    position,
                    std::initializer_list<VALUE_TYPE> values);
        // Insert at the specified 'position' in this vector each 'value_type'
        // object in the specified 'values' initializer list, and return an
        // iterator referring to the first newly inserted element.  If an
        // exception is thrown (other than by the copy constructor, move
        // constructor, assignment operator, and move assignment operator of
        // 'value_type'), '*this' is unaffected.  Throw 'std::length_error' if
        // 'size() + values.size() > max_size()'.  The behavior is undefined
        // unless 'position' is an iterator in the range '[begin() .. end()]'
        // (both endpoints included).  This method requires that the (template
        // parameter) type 'VALUE_TYPE' be 'copy-insertable' into this vector
        // (see {Requirements on 'VALUE_TYPE'}).
#endif

    iterator erase(const_iterator position);
        // Remove from this vector the element at the specified 'position', and
        // return an iterator providing modifiable access to the element
        // immediately following the removed element, or the position returned
        // by the method 'end' if the removed element was the last in the
        // sequence.  The behavior is undefined unless 'position' is an
        // iterator in the range '[cbegin() .. cend())'.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this vector the sequence of elements starting at the
        // specified 'first' position and ending before the specified 'last'
        // position, and return an iterator providing modifiable access to the
        // element immediately following the last removed element, or the
        // position returned by the method 'end' if the removed elements were
        // last in the sequence.  The behavior is undefined unless 'first' is
        // an iterator in the range '[cbegin() .. cend()]' (both endpoints
        // included) and 'last' is an iterator in the range
        // '[first .. cend()]' (both endpoints included).

    void swap(Vector_Imp& other)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
        // Exchange the value of this object with the value of the specified
        // 'other' object.  Additionally, if
        // 'bsl::allocator_traits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true', then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee and
        // guarantees 'O[1]' complexity.  The behavior is undefined unless
        // either this object was created with the same allocator as 'other' or
        // 'propagate_on_container_swap' is 'true'.

    void clear() BSLS_CPP11_NOEXCEPT;
        // Remove all elements from this vector making its size 0.  Note that
        // although this vector is empty after this method returns, it
        // preserves the same capacity it had before the method was called.

    // ACCESSORS
    allocator_type get_allocator() const BSLS_CPP11_NOEXCEPT;
        // Return (a copy of) the allocator used for memory allocation by this
        // vector.

    size_type max_size() const BSLS_CPP11_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of elements
        // that this vector could possibly hold.  Note that there is no
        // guarantee that the vector can successfully grow to the returned
        // size, or even close to that size without running out of resources.
        // Also note that requests to create a vector longer than this number
        // of elements are guaranteed to raise a 'std::length_error' exception.
};

// FREE OPERATORS

                       // *** relational operators ***

template <class VALUE_TYPE, class ALLOCATOR>
bool operator==(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'vector' objects 'lhs' and 'rhs' have
    // the same value if they have the same number of elements, and each
    // element in the ordered sequence of elements of 'lhs' has the same value
    // as the corresponding element in the ordered sequence of elements of
    // 'rhs'.  This method requires that the (template parameter) type
    // 'VALUE_TYPE' be 'equality-comparable' (see {Requirements on
    // 'VALUE_TYPE'}).

template <class VALUE_TYPE, class ALLOCATOR>
bool operator!=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'vector' objects 'lhs' and 'rhs'
    // do not have the same value if they do not have the same number of
    // elements, or some element in the ordered sequence of elements of 'lhs'
    // does not have the same value as the corresponding element in the ordered
    // sequence of elements of 'rhs'.  This method requires that the (template
    // parameter) type 'VALUE_TYPE' be 'equality-comparable' (see {Requirements
    // on 'VALUE_TYPE'}).

template <class VALUE_TYPE, class ALLOCATOR>
bool operator<(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
               const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' vector is
    // lexicographically less than that of the specified 'rhs' vector, and
    // 'false' otherwise.  Given iterators 'i' and 'j' over the respective
    // sequences '[lhs.begin() .. lhs.end())' and '[rhs.begin() .. rhs.end())',
    // the value of vector 'lhs' is lexicographically less than that of vector
    // 'rhs' if 'true == *i < *j' for the first pair of corresponding iterator
    // positions where '*i < *j' and '*j < *i' are not both 'false'.  If no
    // such corresponding iterator position exists, the value of 'lhs' is
    // lexicographically less than that of 'rhs' if 'lhs.size() < rhs.size()'.
    // This method requires that 'operator<', inducing a total order, be
    // defined for 'value_type'.

template <class VALUE_TYPE, class ALLOCATOR>
bool operator>(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
               const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' vector is
    // lexicographically greater than that of the specified 'rhs' vector, and
    // 'false' otherwise.  The value of vector 'lhs' is lexicographically
    // greater than that of vector 'rhs' if 'rhs' is lexicographically less
    // than 'lhs' (see 'operator<').  This method requires that 'operator<',
    // inducing a total order, be defined for 'value_type'.  Note that this
    // operator returns 'rhs < lhs'.

template <class VALUE_TYPE, class ALLOCATOR>
bool operator<=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' vector is
    // lexicographically less than or equal to that of the specified 'rhs'
    // vector, and 'false' otherwise.  The value of vector 'lhs' is
    // lexicographically less than or equal to that of vector 'rhs' if 'rhs' is
    // not lexicographically less than 'lhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(rhs < lhs)'.

template <class VALUE_TYPE, class ALLOCATOR>
bool operator>=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' vector is
    // lexicographically greater than or equal to that of the specified 'rhs'
    // vector, and 'false' otherwise.  The value of vector 'lhs' is
    // lexicographically greater than or equal to that of vector 'rhs' if 'lhs'
    // is not lexicographically less than 'rhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(lhs < rhs)'.

                            // ============
                            // class vector
                            // ============

template <class VALUE_TYPE, class ALLOCATOR = bsl::allocator<VALUE_TYPE> >
class vector : public Vector_Imp<VALUE_TYPE, ALLOCATOR>
    // Note that members which don't need to be redefined are inherited
    // straightforwardly from the 'Base', although if an overloaded method
    // needs to be redefined, then all its overloads need to be redefined.
{
    // PRIVATE TYPES
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR>                  Base;

    typedef bsl::allocator_traits<ALLOCATOR>                   AllocatorTraits;
        // This 'typedef' is an alias for the allocator traits type associated
        // with this container.

    typedef BloombergLP::bslmf::MovableRefUtil                 MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

    typedef BloombergLP::bslalg::ContainerBase<ALLOCATOR> ContainerBase;

  public:
    // PUBLIC TYPES
    typedef typename Base::value_type             value_type;
    typedef typename Base::allocator_type         allocator_type;
    typedef typename Base::reference              reference;
    typedef typename Base::const_reference        const_reference;

    typedef typename Base::size_type              size_type;
    typedef typename Base::difference_type        difference_type;
    typedef typename Base::pointer                pointer;
    typedef typename Base::const_pointer          const_pointer;

    typedef typename Base::iterator               iterator;
    typedef typename Base::const_iterator         const_iterator;
    typedef typename Base::reverse_iterator       reverse_iterator;
    typedef typename Base::const_reverse_iterator const_reverse_iterator;

  public:
    // CREATORS

    // *** construct/copy/destroy ***

    vector() BSLS_CPP11_NOEXCEPT;
    explicit vector(const ALLOCATOR& basicAllocator) BSLS_CPP11_NOEXCEPT;
        // Create an empty vector.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is not specified, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // and 'basicAllocator' is not supplied, the currently installed
        // default allocator is used.  Note that a 'bslma::Allocator *' can be
        // supplied for 'basicAllocator' if the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    explicit vector(size_type        initialSize,
                    const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create a vector of the specified 'initialSize' whose every element
        // is a default-constructed object of the (template parameter) type
        // 'VALUE_TYPE'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not specified, a default-constructed
        // object of the (template parameter) type 'ALLOCATOR' is used.  If the
        // type 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used.  Throw
        // 'std::length_error' if 'initialSize > max_size()'.  This method
        // requires that the type 'VALUE_TYPE' be 'default-insertable' into
        // this vector (see {Requirements on 'VALUE_TYPE'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).

    vector(size_type         initialSize,
           const VALUE_TYPE& value,
           const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create a vector of the specified 'initialSize' whose every element
        // is a copy of the specified 'value'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  Throw 'std::length_error' if
        // 'initialSize > max_size()'.  This method requires that the (template
        // parameter) type 'VALUE_TYPE' be 'copy-insertable' into this vector
        // (see {Requirements on 'VALUE_TYPE'}).  Note that a
        // bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).

    template <class INPUT_ITER>
    vector(INPUT_ITER       first,
           INPUT_ITER       last,
           const ALLOCATOR& basicAllocator = ALLOCATOR());
        // Create a vector, and insert (in order) each 'VALUE_TYPE' object in
        // the range starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // not specified, a default-constructed object of the (template
        // parameter) type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  Throw 'std::length_error' if
        // the number of elements in '[first .. last)' exceeds the value
        // returned by the method 'max_size'.  The (template parameter) type
        // 'INPUT_ITER' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type', and 'value_type' must be
        // 'emplace-constructible' from '*i' into this vector, where 'i' is a
        // dereferenceable iterator in the range '[first .. last)' (see
        // {Requirements on 'VALUE_TYPE'}).  The behavior is undefined unless
        // 'first' and 'last' refer to a range of valid values where 'first'
        // is at a position at or before 'last'.  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).

    vector(const vector& original);
        // Create a vector having the same value as the specified 'original'
        // object.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  This method requires that the (template parameter)
        // type 'VALUE_TYPE' be 'copy-insertable' into this vector (see
        // {Requirements on 'VALUE_TYPE'}).

    vector(BloombergLP::bslmf::MovableRef<vector> original)
                                               BSLS_CPP11_NOEXCEPT; // IMPLICIT
        // Create a vector having the same value as the specified 'original'
        // object by moving (in constant time) the contents of 'original' to
        // the new vector.  The allocator associated with 'original' is
        // propagated for use in the newly-created vector.  'original' is left
        // in a valid but unspecified state.

    vector(const vector& original, const ALLOCATOR& basicAllocator);
        // Create a vector having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // This method requires that the (template parameter) type 'VALUE_TYPE'
        // be 'copy-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).  Note that a 'bslma::Allocator *' can be supplied
        // for 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    vector(BloombergLP::bslmf::MovableRef<vector> original,
           const ALLOCATOR&                       basicAllocator);
        // Create a vector having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // The contents of 'original' are moved (in constant time) to the new
        // vector if 'basicAllocator == original.get_allocator()', and are
        // move-inserted (in linear time) using 'basicAllocator' otherwise.
        // 'original' is left in a valid but unspecified state.  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'move-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).  Note that a 'bslma::Allocator *' can be supplied
        // for 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    vector(std::initializer_list<VALUE_TYPE> values,
           const ALLOCATOR&                  basicAllocator = ALLOCATOR());
        // Create a vector and insert (in order) each 'value_type' object in
        // the specified 'values' initializer list.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  This method requires that the
        // (template parameter) type 'VALUE_TYPE' be 'copy-insertable' into
        // this vector (see {Requirements on 'VALUE_TYPE'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default).
#endif

    ~vector();
        // Destroy this vector.

    // MANIPULATORS
    vector& operator=(const vector& rhs);
        // Assign to this object the value of the specified 'rhs' object,
        // propagate to this object the allocator of 'rhs' if the 'ALLOCATOR'
        // type has trait 'propagate_on_container_copy_assignment', and return
        // a reference providing modifiable access to this object.  If an
        // exception is thrown, '*this' is left in a valid but unspecified
        // state.  This method requires that the (template parameter) type
        // 'VALUE_TYPE' be 'copy-assignable' and 'copy-insertable' into this
        // vector (see {Requirements on 'VALUE_TYPE'}).

    vector& operator=(BloombergLP::bslmf::MovableRef<vector> rhs)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);
        // Assign to this object the value of the specified 'rhs' object,
        // propagate to this object the allocator of 'rhs' if the 'ALLOCATOR'
        // type has trait 'propagate_on_container_move_assignment', and return
        // a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this vector if
        // 'get_allocator() == rhs.get_allocator()' (after accounting for the
        // aforementioned trait); otherwise, all elements in this vector are
        // either destroyed or move-assigned to and each additional element in
        // 'rhs' is move-inserted into this vector.  'rhs' is left in a valid
        // but unspecified state, and if an exception is thrown, '*this' is
        // left in a valid but unspecified state.  This method requires that
        // the (template parameter) type 'VALUE_TYPE' be 'move-assignable' and
        // 'move-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    vector& operator=(std::initializer_list<VALUE_TYPE> values);
        // Assign to this object the value resulting from first clearing this
        // vector and then inserting (in order) each 'value_type' object in the
        // specified 'values' initializer list.  If an exception is thrown,
        // '*this' is left in a valid but unspecified state.  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'copy-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).
#endif
};

// FREE OPERATORS

                       // *** relational operators ***

template <class VALUE_TYPE, class ALLOCATOR>
bool operator==(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'vector' objects 'lhs' and 'rhs' have
    // the same value if they have the same number of elements, and each
    // element in the ordered sequence of elements of 'lhs' has the same value
    // as the corresponding element in the ordered sequence of elements of
    // 'rhs'.  This method requires that the (template parameter) type
    // 'VALUE_TYPE' be 'equality-comparable' (see {Requirements on
    // 'VALUE_TYPE'}).

template <class VALUE_TYPE, class ALLOCATOR>
bool operator!=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'vector' objects 'lhs' and 'rhs'
    // do not have the same value if they do not have the same number of
    // elements, or some element in the ordered sequence of elements of 'lhs'
    // does not have the same value as the corresponding element in the ordered
    // sequence of elements of 'rhs'.  This method requires that the (template
    // parameter) type 'VALUE_TYPE' be 'equality-comparable' (see {Requirements
    // on 'VALUE_TYPE'}).

template <class VALUE_TYPE, class ALLOCATOR>
bool operator<(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
               const vector<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' vector is
    // lexicographically less than that of the specified 'rhs' vector, and
    // 'false' otherwise.  Given iterators 'i' and 'j' over the respective
    // sequences '[lhs.begin() .. lhs.end())' and '[rhs.begin() .. rhs.end())',
    // the value of vector 'lhs' is lexicographically less than that of vector
    // 'rhs' if 'true == *i < *j' for the first pair of corresponding iterator
    // positions where '*i < *j' and '*j < *i' are not both 'false'.  If no
    // such corresponding iterator position exists, the value of 'lhs' is
    // lexicographically less than that of 'rhs' if 'lhs.size() < rhs.size()'.
    // This method requires that 'operator<', inducing a total order, be
    // defined for 'value_type'.

template <class VALUE_TYPE, class ALLOCATOR>
bool operator>(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
               const vector<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' vector is
    // lexicographically greater than that of the specified 'rhs' vector, and
    // 'false' otherwise.  The value of vector 'lhs' is lexicographically
    // greater than that of vector 'rhs' if 'rhs' is lexicographically less
    // than 'lhs' (see 'operator<').  This method requires that 'operator<',
    // inducing a total order, be defined for 'value_type'.  Note that this
    // operator returns 'rhs < lhs'.

template <class VALUE_TYPE, class ALLOCATOR>
bool operator<=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' vector is
    // lexicographically less than or equal to that of the specified 'rhs'
    // vector, and 'false' otherwise.  The value of vector 'lhs' is
    // lexicographically less than or equal to that of vector 'rhs' if 'rhs' is
    // not lexicographically less than 'lhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(rhs < lhs)'.

template <class VALUE_TYPE, class ALLOCATOR>
bool operator>=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' vector is
    // lexicographically greater than or equal to that of the specified 'rhs'
    // vector, and 'false' otherwise.  The value of vector 'lhs' is
    // lexicographically greater than or equal to that of vector 'rhs' if 'lhs'
    // is not lexicographically less than 'rhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(lhs < rhs)'.

// FREE FUNCTIONS
template <class VALUE_TYPE, class ALLOCATOR>
void swap(vector<VALUE_TYPE, ALLOCATOR>& a,
          vector<VALUE_TYPE, ALLOCATOR>& b)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);

                   // =====================================
                   // class vector<VALUE_TYPE *, ALLOCATOR>
                   // =====================================

template <class VALUE_TYPE, class ALLOCATOR>
class vector< VALUE_TYPE *, ALLOCATOR >
: public Vector_Imp<void *,
                    typename ALLOCATOR::template rebind<void *>::other> {
    // This partial specialization of 'vector' for pointer types to a
    // (template parameter) 'VALUE_TYPE' type (not 'const') is implemented in
    // terms of 'Vector_Imp<void *>' to reduce the amount of code generated.
    // Note that this specialization rebinds the (template parameter)
    // 'ALLOCATOR' type to an allocator of 'void *' so as to satisfy the
    // invariant in 'Vector_Imp'.  Also note that members which do not need to
    // be redefined are inherited straightforwardly from the 'Base', although
    // if an overloaded method needs to be redefined, then all its overloads
    // need to be redefined.

    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;
    typedef BloombergLP::bslmf::MovableRefUtil                 MoveUtil;

  public:
    typedef typename ALLOCATOR::reference         reference;
    typedef typename ALLOCATOR::const_reference   const_reference;
    typedef VALUE_TYPE                          **iterator;
    typedef VALUE_TYPE *const                    *const_iterator;
    typedef std::size_t                           size_type;
    typedef std::ptrdiff_t                        difference_type;
    typedef VALUE_TYPE                           *value_type;
    typedef ALLOCATOR                             allocator_type;
    typedef typename ALLOCATOR::pointer           pointer;
    typedef typename ALLOCATOR::const_pointer     const_pointer;
    typedef bsl::reverse_iterator<iterator>       reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

                      // *** construct/copy/destroy ***

    // CREATORS
    explicit vector(const ALLOCATOR& basicAllocator = ALLOCATOR())
                                                            BSLS_CPP11_NOEXCEPT
    : Base(BaseAlloc(basicAllocator))
    {
    }

    explicit vector(size_type initialSize,
                    const ALLOCATOR& basicAllocator = ALLOCATOR())
    : Base(initialSize, BaseAlloc(basicAllocator))
    {
    }

    vector(size_type         initialSize,
           VALUE_TYPE       *value,
           const ALLOCATOR&  basicAllocator = ALLOCATOR())
    : Base(initialSize, (void *) value, BaseAlloc(basicAllocator))
    {
    }

    template <class INPUT_ITER>
    vector(INPUT_ITER       first,
           INPUT_ITER       last,
           const ALLOCATOR& basicAllocator = ALLOCATOR())
    : Base(first, last, BaseAlloc(basicAllocator))
    {
    }

    vector(const vector& original)
    : Base(original)
    {
    }

    vector(BloombergLP::bslmf::MovableRef<vector> original)
                                                BSLS_CPP11_NOEXCEPT // IMPLICIT
    : Base(MoveUtil::move(static_cast<Base&>(original)))
    {
    }

    vector(const vector& original, const ALLOCATOR& basicAllocator)
    : Base(original, BaseAlloc(basicAllocator))
    {
    }

    vector(BloombergLP::bslmf::MovableRef<vector> original,
           const ALLOCATOR& basicAllocator)
    : Base(MoveUtil::move(static_cast<Base&>(original)),
           BaseAlloc(basicAllocator))
    {
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    vector(std::initializer_list<VALUE_TYPE *> values,
           const ALLOCATOR&           basicAllocator = ALLOCATOR())
    : Base(values.begin(), values.end(), basicAllocator)
    {
    }
#endif

    ~vector()
    {
    }

    // MANIPULATORS
    vector& operator=(const vector& rhs)
    {
        Base::operator=(rhs);
        return *this;
    }

    vector& operator=(BloombergLP::bslmf::MovableRef<vector> rhs)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
    {
        Base::operator=(MoveUtil::move(static_cast<Base&>(rhs)));
        return *this;
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    vector& operator=(std::initializer_list<VALUE_TYPE *> values)
    {
        Base::assign(values.begin(), values.end());
        return *this;
    }
#endif

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last)
    {
        Base::assign(first, last);
    }

    void assign(size_type numElements, VALUE_TYPE *value)
    {
        Base::assign(numElements, (void *) value);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void assign(std::initializer_list<VALUE_TYPE *> values)
    {
        Base::assign(values.begin(), values.end());
    }
#endif
                             // *** iterators ***

    iterator begin() BSLS_CPP11_NOEXCEPT
    {
        return (iterator) Base::begin();
    }

    iterator end() BSLS_CPP11_NOEXCEPT
    {
        return (iterator) Base::end();
    }

    reverse_iterator rbegin() BSLS_CPP11_NOEXCEPT
    {
        return reverse_iterator((iterator) Base::rbegin().base());
    }
    reverse_iterator rend() BSLS_CPP11_NOEXCEPT
    {
        return reverse_iterator((iterator) Base::rend().base());
    }

                          // *** element access ***

    reference operator[](size_type position)
    {
        return (reference) Base::operator[](position);
    }

    reference at(size_type position)
    {
        return (reference) Base::at(position);
    }

    reference front()
    {
        return (reference) Base::front();
    }

    reference back()
    {
        return (reference) Base::back();
    }

    VALUE_TYPE **data() BSLS_CPP11_NOEXCEPT
    {
        return (VALUE_TYPE **) Base::data();
    }

                             // *** capacity ***

    void resize(size_type newLength)
    {
        Base::resize(newLength);
    }

    void resize(size_type newLength, VALUE_TYPE *value)
    {
        Base::resize(newLength, (void *) value);
    }

    // void reserve(size_type newCapacity);
    //   This method can be inherited from Base without a cast.

    // void shrink_to_fit();
    //   This method can be inherited from Base without a cast.

                            // *** modifiers ***

    void push_back(VALUE_TYPE *value)
    {
        Base::push_back((void *) value);
    }

    // void pop_back();
    //   This method can be inherited from Base without a cast.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    iterator emplace(const_iterator position, Args&&... arguments)
    {
        return (iterator) Base::emplace((void *const *) position,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_vector.h
    iterator emplace(const_iterator position)
    {
        return (iterator) Base::emplace((void *const *) position);
    }

    template <class Args_01>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));
    }

    template <class Args_01,
              class Args_02>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
    }

    template <class Args_01,
              class Args_02,
              class Args_03>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                         BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                         BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                         BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09,
              class Args_10>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
    {
        return (iterator) Base::emplace((void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                         BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                         BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                         BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));
    }

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... Args>
    iterator emplace(const_iterator position,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
    {
        return (iterator) Base::emplace((void *const *) position,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }
// }}} END GENERATED CODE
#endif

    iterator insert(const_iterator position, VALUE_TYPE *value)
    {
        return (iterator) Base::insert((void *const *) position,
                                       (void *) value);
    }

    iterator insert(const_iterator  position,
                    size_type       numElements,
                    VALUE_TYPE     *value)
    {
        return (iterator) Base::insert((void *const *) position, numElements,
                                       (void *) value);
    }

    template <class INPUT_ITER>
    iterator insert(const_iterator position, INPUT_ITER first, INPUT_ITER last)
    {
        return (iterator) Base::insert((void *const *) position, first, last);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    iterator insert(const_iterator position,
                    std::initializer_list<VALUE_TYPE *> values)
    {
        return (iterator) Base::insert((void *const *) position,
                                       values.begin(),
                                       values.end());
    }
#endif

    iterator erase(const_iterator position)
    {
        return (iterator) Base::erase((void *const *) position);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        return (iterator) Base::erase((void *const *) first,
                                      (void *const *) last);
    }

    // void swap(Vector_Imp& other);
    // void clear();
    //   These methods can be inherited from Base without a cast.

    // ACCESSORS
    allocator_type get_allocator() const BSLS_CPP11_NOEXCEPT
    {
        return ALLOCATOR(Base::get_allocator());
    }

    // size_type max_size();
    //   This method can be inherited from Base without a cast.

                             // *** iterators ***

    const_iterator begin() const BSLS_CPP11_NOEXCEPT
    {
        return (const_iterator) Base::begin();
    }

    const_iterator cbegin() const BSLS_CPP11_NOEXCEPT
    {
        return (const_iterator) Base::cbegin();
    }

    const_iterator end() const BSLS_CPP11_NOEXCEPT
    {
        return (const_iterator) Base::end();
    }

    const_iterator cend() const BSLS_CPP11_NOEXCEPT
    {
        return (const_iterator) Base::cend();
    }

    const_reverse_iterator rbegin() const BSLS_CPP11_NOEXCEPT
    {
        return const_reverse_iterator((const_iterator) Base::rbegin().base());
    }

    const_reverse_iterator crbegin() const BSLS_CPP11_NOEXCEPT
    {
        return const_reverse_iterator((const_iterator) Base::crbegin().base());
    }

    const_reverse_iterator rend() const BSLS_CPP11_NOEXCEPT
    {
        return const_reverse_iterator((const_iterator) Base::rend().base());
    }

    const_reverse_iterator crend() const BSLS_CPP11_NOEXCEPT
    {
        return const_reverse_iterator((const_iterator) Base::crend().base());
    }

                             // *** capacity ***

    // size_type size();
    // size_type capacity();
    // bool empty();
    //   These methods can be inherited from Base without a cast.

                          // *** element access ***

    const_reference operator[](size_type position) const
    {
        return (const_reference) Base::operator[](position);
    }

    const_reference at(size_type position) const
    {
        return (const_reference) Base::at(position);
    }

    const_reference front() const
    {
        return (const_reference) Base::front();
    }

    const_reference back() const
    {
        return (const_reference) Base::back();
    }

    VALUE_TYPE *const *data() const BSLS_CPP11_NOEXCEPT
    {
        return (VALUE_TYPE *const *) Base::data();
    }
};

// FREE OPERATORS
template <class VALUE_TYPE, class ALLOCATOR>
bool operator==(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator!=(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator<(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator>(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator<=(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator>=(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<VALUE_TYPE *,ALLOCATOR>& rhs);

// FREE FUNCTIONS
template <class VALUE_TYPE, class ALLOCATOR>
void swap(vector<VALUE_TYPE *, ALLOCATOR>& a,
          vector<VALUE_TYPE *, ALLOCATOR>& b)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);

             // ===========================================
             // class vector<const VALUE_TYPE *, ALLOCATOR>
             // ===========================================

template <class VALUE_TYPE, class ALLOCATOR>
class vector< const VALUE_TYPE *, ALLOCATOR >
: public Vector_Imp<const void *,
                    typename ALLOCATOR::template rebind<const void *>::other> {
    // This partial specialization of 'vector' for pointer types to a
    // 'const'-qualified (template parameter) 'VALUE_TYPE' is implemented in
    // terms of 'Vector_Imp<const void *>' to reduce the amount of code
    // generated.  Note that this specialization rebinds the (template
    // parameter) 'ALLOCATOR' type to an allocator of 'const void *' so as to
    // satisfy the invariant in 'Vector_Imp'.  Also note that members which do
    // not need to be redefined are inherited straightforwardly from the
    // 'Base', although if an overloaded method needs to be redefined, then all
    // its overloads need to be redefined.

    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
    typedef BloombergLP::bslmf::MovableRefUtil                       MoveUtil;

  public:
    typedef typename ALLOCATOR::reference         reference;
    typedef typename ALLOCATOR::const_reference   const_reference;
    typedef const VALUE_TYPE                    **iterator;
    typedef const VALUE_TYPE *const              *const_iterator;
    typedef std::size_t                           size_type;
    typedef std::ptrdiff_t                        difference_type;
    typedef const VALUE_TYPE                     *value_type;
    typedef ALLOCATOR                             allocator_type;
    typedef typename ALLOCATOR::pointer           pointer;
    typedef typename ALLOCATOR::const_pointer     const_pointer;
    typedef bsl::reverse_iterator<iterator>       reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

                      // *** construct/copy/destroy ***

    // CREATORS
    explicit vector(const ALLOCATOR& basicAllocator = ALLOCATOR())
                                                            BSLS_CPP11_NOEXCEPT
    : Base(BaseAlloc(basicAllocator))
    {
    }

    explicit vector(size_type        initialSize,
                    const ALLOCATOR& basicAllocator = ALLOCATOR())
    : Base(initialSize, BaseAlloc(basicAllocator))
    {
    }

    vector(size_type         initialSize,
           const VALUE_TYPE *value,
           const ALLOCATOR&  basicAllocator = ALLOCATOR())
    : Base(initialSize, (const void *)value, BaseAlloc(basicAllocator))
    {
    }

    template <class INPUT_ITER>
    vector(INPUT_ITER       first,
           INPUT_ITER       last,
           const ALLOCATOR& basicAllocator = ALLOCATOR())
    : Base(first, last, BaseAlloc(basicAllocator))
    {
    }

    vector(const vector& original)
    : Base(original)
    {
    }

    vector(BloombergLP::bslmf::MovableRef<vector> original)
                                                BSLS_CPP11_NOEXCEPT // IMPLICIT
    : Base(MoveUtil::move(static_cast<Base&>(original)))
    {
    }

    vector(const vector& original, const ALLOCATOR& basicAllocator)
    : Base(original, BaseAlloc(basicAllocator))
    {
    }

    vector(BloombergLP::bslmf::MovableRef<vector> original,
           const ALLOCATOR&                       basicAllocator)
    : Base(MoveUtil::move(static_cast<Base&>(original)),
           BaseAlloc(basicAllocator))
    {
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    vector(
        std::initializer_list<const VALUE_TYPE *> values,
        const ALLOCATOR&                          basicAllocator = ALLOCATOR())
    : Base(values.begin(), values.end(), basicAllocator)
    {
    }
#endif

    ~vector() {}

    // MANIPULATORS

    vector& operator=(const vector& rhs)
    {
        Base::operator=(rhs);
        return *this;
    }

    vector& operator=(BloombergLP::bslmf::MovableRef<vector> rhs)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
    {
        Base::operator=(MoveUtil::move(static_cast<Base&>(rhs)));
        return *this;
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    vector& operator=(std::initializer_list<const VALUE_TYPE *> values)
    {
        Base::assign(values.begin(), values.end());
        return *this;
    }
#endif

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last)
    {
        Base::assign(first, last);
    }

    void assign(size_type numElements, const VALUE_TYPE *value)
    {
        Base::assign(numElements, (const void *) value);
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void assign(std::initializer_list<const VALUE_TYPE *> values)
    {
        Base::assign(values.begin(), values.end());
    }
#endif
                             // *** iterators ***

    iterator begin() BSLS_CPP11_NOEXCEPT
    {
        return (iterator) Base::begin();
    }

    iterator end() BSLS_CPP11_NOEXCEPT
    {
        return (iterator) Base::end();
    }

    reverse_iterator rbegin() BSLS_CPP11_NOEXCEPT
    {
        return reverse_iterator((iterator) Base::rbegin().base());
    }

    reverse_iterator rend() BSLS_CPP11_NOEXCEPT
    {
        return reverse_iterator((iterator) Base::rend().base());
    }

                          // *** element access ***

    reference operator[](size_type position)
    {
        return (reference) Base::operator[](position);
    }

    reference at(size_type position)
    {
        return (reference) Base::at(position);
    }

    reference front()
    {
        return (reference) Base::front();
    }

    reference back()
    {
        return (reference) Base::back();
    }

    const VALUE_TYPE **data() BSLS_CPP11_NOEXCEPT
    {
        return (const VALUE_TYPE **) Base::data();
    }

                             // *** capacity ***

    void resize(size_type newLength)
    {
        Base::resize(newLength);
    }

    void resize(size_type newLength, const VALUE_TYPE *value)
    {
        Base::resize(newLength, (const void *) value);
    }

    // void reserve(size_type newCapacity);
    //   This method can be inherited from Base without a cast.

                            // *** modifiers ***

    void push_back(const VALUE_TYPE *value)
    {
        Base::push_back((const void *) value);
    }

    // void pop_back();
    //   This method can be inherited from Base without a cast.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    iterator emplace(const_iterator position, Args&&... arguments)
    {
        return (iterator) Base::emplace((const void *const *) position,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_vector.h
    iterator emplace(const_iterator position)
    {
        return (iterator) Base::emplace((const void *const *) position);
    }

    template <class Args_01>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));
    }

    template <class Args_01,
              class Args_02>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
    }

    template <class Args_01,
              class Args_02,
              class Args_03>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                         BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                         BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                         BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09,
              class Args_10>
    iterator emplace(const_iterator position,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
    {
        return (iterator) Base::emplace((const void *const *) position,
                         BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                         BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                         BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                         BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                         BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                         BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                         BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                         BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                         BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                         BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));
    }

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class... Args>
    iterator emplace(const_iterator position,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
    {
        return (iterator) Base::emplace((const void *const *) position,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }
// }}} END GENERATED CODE
#endif

    iterator insert(const_iterator position, const VALUE_TYPE *value)
    {
        return (iterator) Base::insert((const void *const *) position,
                                       (const void *) value);
    }

    iterator insert(const_iterator    position,
                    size_type         numElements,
                    const VALUE_TYPE *value)
    {
        return (iterator) Base::insert((const void *const *) position,
                                       numElements,
                                       (const void *) value);
    }

    template <class INPUT_ITER>
    iterator insert(const_iterator position,
                    INPUT_ITER     first,
                    INPUT_ITER     last)
    {
        return (iterator) Base::insert((const void *const *) position,
                                       first,
                                       last);
    }

    iterator erase(const_iterator position)
    {
        return (iterator) Base::erase((const void *const *) position);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        return (iterator) Base::erase((const void *const *)first,
                                      (const void *const *)last);
    }

    // void swap(vector &other);
    // void clear();
    //  These methods can be inherited from Base without a cast.

    // ACCESSORS

    allocator_type get_allocator() const BSLS_CPP11_NOEXCEPT
    {
        return ALLOCATOR(Base::get_allocator());
    }

    // size_type max_size();
    //   This method can be inherited from Base without a cast.

                             // *** iterators ***

    const_iterator begin() const BSLS_CPP11_NOEXCEPT
    {
        return (const_iterator) Base::begin();
    }

    const_iterator cbegin() const BSLS_CPP11_NOEXCEPT
    {
        return (const_iterator) Base::cbegin();
    }

    const_iterator end() const BSLS_CPP11_NOEXCEPT
    {
        return (const_iterator) Base::end();
    }

    const_iterator cend() const BSLS_CPP11_NOEXCEPT
    {
        return (const_iterator) Base::cend();
    }

    const_reverse_iterator rbegin() const BSLS_CPP11_NOEXCEPT
    {
        return const_reverse_iterator((const_iterator) Base::rbegin().base());
    }

    const_reverse_iterator crbegin() const BSLS_CPP11_NOEXCEPT
    {
        return const_reverse_iterator((const_iterator) Base::crbegin().base());
    }

    const_reverse_iterator rend() const BSLS_CPP11_NOEXCEPT
    {
        return const_reverse_iterator((const_iterator) Base::rend().base());
    }

    const_reverse_iterator crend() const BSLS_CPP11_NOEXCEPT
    {
        return const_reverse_iterator((const_iterator) Base::crend().base());
    }

                             // *** capacity ***

    // 'size()', 'capacity()', 'empty()' can be inherited from Base without
    // cast.

                          // *** element access ***

    const_reference operator[](size_type position) const
    {
        return (const_reference) Base::operator[](position);
    }

    const_reference at(size_type position) const
    {
        return (const_reference) Base::at(position);
    }

    const_reference front() const
    {
        return (const_reference) Base::front();
    }

    const_reference back() const
    {
        return (const_reference) Base::back();
    }

    const VALUE_TYPE *const *data() const BSLS_CPP11_NOEXCEPT
    {
        return (const VALUE_TYPE *const *) Base::data();
    }
};

// FREE OPERATORS
template <class VALUE_TYPE, class ALLOCATOR>
bool operator==(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator!=(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator<(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<const VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator>(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<const VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator<=(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool operator>=(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs);

// FREE FUNCTIONS
template <class VALUE_TYPE, class ALLOCATOR>
void swap(vector<const VALUE_TYPE *, ALLOCATOR>& a,
          vector<const VALUE_TYPE *, ALLOCATOR>& b)
             BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE);

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================
// See IMPLEMENTATION NOTES in the .cpp before modifying anything below.

                          // --------------------
                          // class Vector_ImpBase
                          // --------------------

// CREATORS
template <class VALUE_TYPE>
inline
Vector_ImpBase<VALUE_TYPE>::Vector_ImpBase()
: d_dataBegin_p(0)
, d_dataEnd_p(0)
, d_capacity(0)
{
}

// MANIPULATORS

template <class VALUE_TYPE>
inline
void
Vector_ImpBase<VALUE_TYPE>::adopt(
                       BloombergLP::bslmf::MovableRef<Vector_ImpBase> original)
{
    BSLS_ASSERT_SAFE(0 == d_dataBegin_p);
    BSLS_ASSERT_SAFE(0 == d_dataEnd_p);
    BSLS_ASSERT_SAFE(0 == d_capacity);

    Vector_ImpBase& lvalue = original;
    d_dataBegin_p          = lvalue.d_dataBegin_p;
    d_dataEnd_p            = lvalue.d_dataEnd_p;
    d_capacity             = lvalue.d_capacity;

    lvalue.d_dataBegin_p = 0;
    lvalue.d_dataEnd_p   = 0;
    lvalue.d_capacity    = 0;
}
                             // *** iterators ***
template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::iterator
Vector_ImpBase<VALUE_TYPE>::begin() BSLS_CPP11_NOEXCEPT
{
    return d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::iterator
Vector_ImpBase<VALUE_TYPE>::end() BSLS_CPP11_NOEXCEPT
{
    return d_dataEnd_p;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reverse_iterator
Vector_ImpBase<VALUE_TYPE>::rbegin() BSLS_CPP11_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reverse_iterator
Vector_ImpBase<VALUE_TYPE>::rend() BSLS_CPP11_NOEXCEPT
{
    return reverse_iterator(begin());
}

                          // *** element access ***

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reference
Vector_ImpBase<VALUE_TYPE>::operator[](size_type position)
{
    BSLS_ASSERT_SAFE(size() > position);

    return d_dataBegin_p[position];
}

template <class VALUE_TYPE>
typename Vector_ImpBase<VALUE_TYPE>::reference
Vector_ImpBase<VALUE_TYPE>::at(size_type position)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position >= size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                "vector<...>::at(position): invalid position");
    }
    return d_dataBegin_p[position];
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reference
Vector_ImpBase<VALUE_TYPE>::front()
{
    BSLS_ASSERT_SAFE(!empty());

    return *d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reference
Vector_ImpBase<VALUE_TYPE>::back()
{
    BSLS_ASSERT_SAFE(!empty());

    return *(d_dataEnd_p - 1);
}

template <class VALUE_TYPE>
inline
VALUE_TYPE *
Vector_ImpBase<VALUE_TYPE>::data() BSLS_CPP11_NOEXCEPT
{
    return d_dataBegin_p;
}

// ACCESSORS

                             // *** iterators ***
template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_iterator
Vector_ImpBase<VALUE_TYPE>::begin() const BSLS_CPP11_NOEXCEPT
{
    return d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_iterator
Vector_ImpBase<VALUE_TYPE>::cbegin() const BSLS_CPP11_NOEXCEPT
{
    return d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_iterator
Vector_ImpBase<VALUE_TYPE>::end() const BSLS_CPP11_NOEXCEPT
{
    return d_dataEnd_p;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_iterator
Vector_ImpBase<VALUE_TYPE>::cend() const BSLS_CPP11_NOEXCEPT
{
    return d_dataEnd_p;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reverse_iterator
Vector_ImpBase<VALUE_TYPE>::rbegin() const BSLS_CPP11_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reverse_iterator
Vector_ImpBase<VALUE_TYPE>::crbegin() const BSLS_CPP11_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reverse_iterator
Vector_ImpBase<VALUE_TYPE>::rend() const BSLS_CPP11_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reverse_iterator
Vector_ImpBase<VALUE_TYPE>::crend() const BSLS_CPP11_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

                             // *** capacity ***

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::size_type
Vector_ImpBase<VALUE_TYPE>::size() const BSLS_CPP11_NOEXCEPT
{
    return d_dataEnd_p - d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::size_type
Vector_ImpBase<VALUE_TYPE>::capacity() const BSLS_CPP11_NOEXCEPT
{
    return d_capacity;
}

template <class VALUE_TYPE>
inline
bool Vector_ImpBase<VALUE_TYPE>::empty() const BSLS_CPP11_NOEXCEPT
{
    return d_dataEnd_p == d_dataBegin_p;
}

                          // *** element access ***
template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reference
Vector_ImpBase<VALUE_TYPE>::operator[](size_type position) const
{
    BSLS_ASSERT_SAFE(size() > position);

    return d_dataBegin_p[position];
}

template <class VALUE_TYPE>
typename Vector_ImpBase<VALUE_TYPE>::const_reference
Vector_ImpBase<VALUE_TYPE>::at(size_type position) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(position >= size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                          "const vector<...>::at(position): invalid position");
    }
    return d_dataBegin_p[position];
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reference
Vector_ImpBase<VALUE_TYPE>::front() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reference
Vector_ImpBase<VALUE_TYPE>::back() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *(d_dataEnd_p - 1);
}

template <class VALUE_TYPE>
inline
const VALUE_TYPE *
Vector_ImpBase<VALUE_TYPE>::data() const BSLS_CPP11_NOEXCEPT
{
    return d_dataBegin_p;
}

             // ------------------------------------------------
             // class Vector_Imp<VALUE_TYPE, ALLOCATOR>::Proctor
             // ------------------------------------------------

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Proctor::Proctor(VALUE_TYPE    *data,
                                                    std::size_t    capacity,
                                                    ContainerBase *container)
: d_data_p(data)
, d_capacity(capacity)
, d_container_p(container)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Proctor::~Proctor()
{
    if (d_data_p) {
        d_container_p->deallocateN(d_data_p, d_capacity);
    }
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::Proctor::release()
{
    d_data_p = 0;
}

                            // ----------------
                            // class Vector_Imp
                            // ----------------

// PRIVATE MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateInsertDispatch(
                              const_iterator                          position,
                              INPUT_ITER                              count,
                              INPUT_ITER                              value,
                              BloombergLP::bslmf::MatchArithmeticType ,
                              BloombergLP::bslmf::Nil                 )
{
    // 'count' and 'value' are integral types that just happen to be the same.
    // They are not iterators, so we call 'insert(position, count, value)'.

    this->insert(position,
                 static_cast<size_type>(count),
                 static_cast<VALUE_TYPE>(value));
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateInsertDispatch(
                                          const_iterator              position,
                                          INPUT_ITER                  first,
                                          INPUT_ITER                  last,
                                          BloombergLP::bslmf::MatchAnyType ,
                                          BloombergLP::bslmf::MatchAnyType )
{
    // Dispatch based on iterator category.
    BSLS_ASSERT_SAFE(!Vector_RangeCheck::isInvalidRange(first, last));

    typedef typename bsl::iterator_traits<INPUT_ITER>::iterator_category Tag;
    this->privateInsert(position, first, last, Tag());
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateInsert(
                                      const_iterator                  position,
                                      INPUT_ITER                      first,
                                      INPUT_ITER                      last,
                                      const std::input_iterator_tag&)
{
    // IMPLEMENTATION NOTES: We can't compute the size in advance.  Bootstrap
    // insertion with random-access iterator by using a temporary vector (which
    // will also guarantee that if allocator throws, the vector is unchanged).
    // Also, use the same allocator for the temporary vector so that its
    // elements can be moved into the current one, rather than copied.
    // Finally, construct the temporary vector by iterated 'push_back', which
    // may reallocate memory associated with the temporary vector multiple
    // times, but unfortunately is required because we can't compute the size
    // in advance (as with 'forward_iterator_tag') because input iterators can
    // be traversed only once.

    BSLS_ASSERT_SAFE(!Vector_RangeCheck::isInvalidRange(first, last));

    if (first == last) {
        // Avoid creating a 'temp' vector in that case.

        return;                                                       // RETURN
    }

    // Make sure we don't shrink the capacity if this vector is empty, because
    // it will be swapped.  Note that '[first .. last)' is not empty, so there
    // is no harm reserving one element; in fact, it will speed up the first
    // 'push_back'.

    const bool isEmpty = this->empty();
    Vector_Imp temp(this->get_allocator());
    temp.privateReserveEmpty(isEmpty ? this->capacity() : size_type(1));

    while (first != last) {
        temp.push_back(*first);
        ++first;
    }

    if (isEmpty) {
        // Optimization: no need to insert into an empty vector, just swap.

        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
        return;                                                       // RETURN
    }

    this->privateMoveInsert(&temp, position);
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class FWD_ITER>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateInsert(
                                    const_iterator                    position,
                                    FWD_ITER                          first,
                                    FWD_ITER                          last,
                                    const std::forward_iterator_tag&)
{
    // Specialization for all iterators except input iterators: 'size' can be
    // computed in advance.
    BSLS_ASSERT_SAFE(!Vector_RangeCheck::isInvalidRange(first, last));

    const iterator& pos = const_cast<iterator>(position);

    const size_type maxSize = max_size();
    const size_type n = bsl::distance(first, last);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(n > maxSize - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                       "vector<...>::insert(pos,first,last): vector too long");
    }

    const size_type newSize = this->size() + n;
    if (newSize > this->d_capacity) {
        size_type newCapacity = Vector_Util::computeNewCapacity(
                                                              newSize,
                                                              this->d_capacity,
                                                              maxSize);
        Vector_Imp temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        ArrayPrimitives::destructiveMoveAndInsert(temp.d_dataBegin_p,
                                                  &this->d_dataEnd_p,
                                                  this->d_dataBegin_p,
                                                  pos,
                                                  this->d_dataEnd_p,
                                                  first,
                                                  last,
                                                  n,
                                                  ContainerBase::allocator());
        temp.d_dataEnd_p += newSize;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
    else {
        ArrayPrimitives::insert(pos,
                                this->end(),
                                first,
                                last,
                                n,
                                ContainerBase::allocator());
        this->d_dataEnd_p += n;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateMoveInsert(
                                                    Vector_Imp     *fromVector,
                                                    const_iterator  position)
{
    const iterator& pos = const_cast<const iterator&>(position);

    const size_type maxSize = max_size();
    const size_type n = fromVector->size();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(n > maxSize - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                       "vector<...>::insert(pos,first,last): vector too long");
    }

    const size_type newSize = this->size() + n;
    if (newSize > this->d_capacity) {
        const size_type newCapacity = Vector_Util::computeNewCapacity(
                                                              newSize,
                                                              this->d_capacity,
                                                              maxSize);
        Vector_Imp temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        ArrayPrimitives::destructiveMoveAndMoveInsert(
            temp.d_dataBegin_p,
            &this->d_dataEnd_p,
            &fromVector->d_dataEnd_p,
            this->d_dataBegin_p,
            pos,
            this->d_dataEnd_p,
            fromVector->d_dataBegin_p,
            fromVector->d_dataEnd_p,
            n,
            ContainerBase::allocator());
        temp.d_dataEnd_p += newSize;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
    else {
        ArrayPrimitives::moveInsert(pos,
                                    this->end(),
                                    &fromVector->d_dataEnd_p,
                                    fromVector->d_dataBegin_p,
                                    fromVector->d_dataEnd_p,
                                    n,
                                    ContainerBase::allocator());
        this->d_dataEnd_p += n;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateReserveEmpty(
                                                         size_type numElements)
{
    BSLS_ASSERT_SAFE(this->empty());
    BSLS_ASSERT_SAFE(0 == this->capacity());

    this->d_dataBegin_p = this->d_dataEnd_p = this->allocateN(
                                                (VALUE_TYPE *) 0, numElements);
    this->d_capacity = numElements;
}

// CREATORS

                      // *** construct/copy/destroy ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(const ALLOCATOR& basicAllocator)
: Vector_ImpBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(size_type        initialSize,
                                              const ALLOCATOR& basicAllocator)
: Vector_ImpBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(initialSize > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                  "vector<...>::vector(n,v): vector too long");
    }
    if (initialSize > 0) {
        privateReserveEmpty(initialSize);
        Proctor proctor(this->d_dataBegin_p,
                        this->d_capacity,
                        static_cast<ContainerBase *>(this));

        ArrayPrimitives::defaultConstruct(this->d_dataBegin_p,
                                          initialSize,
                                          ContainerBase::allocator());

        proctor.release();
        this->d_dataEnd_p += initialSize;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(size_type         initialSize,
                                              const VALUE_TYPE& value,
                                              const ALLOCATOR&  basicAllocator)
: Vector_ImpBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(initialSize > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                                  "vector<...>::vector(n,v): vector too long");
    }
    if (initialSize > 0) {
        privateReserveEmpty(initialSize);
        Proctor proctor(this->d_dataBegin_p,
                        this->d_capacity,
                        static_cast<ContainerBase *>(this));

        ArrayPrimitives::uninitializedFillN(this->d_dataBegin_p,
                                            initialSize,
                                            value,
                                            ContainerBase::allocator());

        proctor.release();
        this->d_dataEnd_p += initialSize;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
BSLS_PLATFORM_AGGRESSIVE_INLINE
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(INPUT_ITER       first,
                                              INPUT_ITER       last,
                                              const ALLOCATOR& basicAllocator)
: Vector_ImpBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    insert(this->begin(), first, last);
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(const Vector_Imp& original)
: Vector_ImpBase<VALUE_TYPE>()
, ContainerBase(AllocatorTraits::select_on_container_copy_construction(
                                          original.ContainerBase::allocator()))
{
    if (original.size() > 0) {
        privateReserveEmpty(original.size());
        Proctor proctor(this->d_dataBegin_p,
                        this->d_capacity,
                        static_cast<ContainerBase *>(this));

        ArrayPrimitives::copyConstruct(this->d_dataBegin_p,
                                       original.begin(),
                                       original.end(),
                                       ContainerBase::allocator());

        proctor.release();
        this->d_dataEnd_p += original.size();
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::
Vector_Imp(const Vector_Imp& original, const ALLOCATOR& basicAllocator)
: Vector_ImpBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    if (original.size() > 0) {
        privateReserveEmpty(original.size());
        Proctor proctor(this->d_dataBegin_p,
                        this->d_capacity,
                        static_cast<ContainerBase *>(this));

        ArrayPrimitives::copyConstruct(this->d_dataBegin_p,
                                       original.begin(),
                                       original.end(),
                                       ContainerBase::allocator());

        proctor.release();
        this->d_dataEnd_p += original.size();
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(
                           BloombergLP::bslmf::MovableRef<Vector_Imp> original)
: Vector_ImpBase<VALUE_TYPE>()
, ContainerBase(MoveUtil::access(original).get_allocator())
{
    Vector_Imp& lvalue = original;
    ImpBase::adopt(MoveUtil::move(static_cast<ImpBase&>(lvalue)));
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(
                     BloombergLP::bslmf::MovableRef<Vector_Imp> original,
                     const ALLOCATOR&                           basicAllocator)
: Vector_ImpBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    Vector_Imp& lvalue = original;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(get_allocator() ==
                                            lvalue.get_allocator())) {
        ImpBase::adopt(MoveUtil::move(static_cast<ImpBase&>(lvalue)));
    }
    else {
        if (lvalue.size() > 0) {
            privateReserveEmpty(lvalue.size());
            Proctor proctor(this->d_dataBegin_p,
                            this->d_capacity,
                            static_cast<ContainerBase *>(this));

            ArrayPrimitives::moveConstruct(this->d_dataBegin_p,
                                           lvalue.begin(),
                                           lvalue.end(),
                                           ContainerBase::allocator());

            proctor.release();
            this->d_dataEnd_p += lvalue.size();
        }
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
Vector_Imp<VALUE_TYPE, ALLOCATOR>::~Vector_Imp()
{
    if (this->d_dataBegin_p) {
        BloombergLP::bslalg::ArrayDestructionPrimitives::destroy(
                                            this->d_dataBegin_p,
                                            this->d_dataEnd_p,
                                            ContainerBase::allocator());
        this->deallocateN(this->d_dataBegin_p, this->d_capacity);
    }
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>&
Vector_Imp<VALUE_TYPE, ALLOCATOR>::operator=(const Vector_Imp& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        if (AllocatorTraits::propagate_on_container_copy_assignment::value) {
            Vector_Imp other(rhs, rhs.get_allocator());
            Vector_Util::swap(&this->d_dataBegin_p, &other.d_dataBegin_p);
            using std::swap;
            swap(ContainerBase::allocator(), other.ContainerBase::allocator());
        }
        else {
            // Invoke 'erase' only if the current vector is not empty.
            if (!this->empty()) {
                erase(this->begin(), this->end());
            }
            insert(this->begin(), rhs.begin(), rhs.end());
        }
    }
    return *this;
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>&
Vector_Imp<VALUE_TYPE, ALLOCATOR>::operator=(
                                BloombergLP::bslmf::MovableRef<Vector_Imp> rhs)
{
    Vector_Imp& lvalue = rhs;
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &lvalue)) {
        if (get_allocator() == lvalue.get_allocator()) {
            Vector_Imp other(MoveUtil::move(lvalue));
            Vector_Util::swap(&this->d_dataBegin_p, &other.d_dataBegin_p);
        }
        else if (
              AllocatorTraits::propagate_on_container_move_assignment::value) {
            Vector_Imp other(MoveUtil::move(lvalue));
            using std::swap;
            swap(ContainerBase::allocator(), other.ContainerBase::allocator());
            Vector_Util::swap(&this->d_dataBegin_p, &other.d_dataBegin_p);
        }
        else {
            Vector_Imp other(MoveUtil::move(lvalue),
                             ContainerBase::allocator());
            Vector_Util::swap(&this->d_dataBegin_p, &other.d_dataBegin_p);
        }
    }
    return *this;
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::assign(INPUT_ITER first,
                                               INPUT_ITER last)
{
    BSLS_ASSERT_SAFE(!Vector_RangeCheck::isInvalidRange(first, last));

    if (!this->empty()) {
        erase(this->begin(), this->end());
    }
    insert(this->begin(), first, last);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::assign(size_type         numElements,
                                               const VALUE_TYPE& value)
{
    if (!this->empty()) {
        erase(this->begin(), this->end());
    }
    insert(this->begin(), numElements, value);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::assign(
                                      std::initializer_list<VALUE_TYPE> values)
{
    assign(values.begin(), values.end());
}
#endif

                             // *** capacity ***

template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::resize(size_type newSize)
{
    // This function provides the *strong* exception guarantee (except when
    // the move constructor of a non-copy-insertable 'value_type' throws).

    // Cannot use copy constructor since the only requirements on 'VALUE_TYPE'
    // are 'move-insertable' and 'default-constructible'.

    if (newSize <= this->size()) {
        BloombergLP::bslalg::ArrayDestructionPrimitives::destroy(
                                            this->d_dataBegin_p + newSize,
                                            this->d_dataEnd_p,
                                            ContainerBase::allocator());
        this->d_dataEnd_p = this->d_dataBegin_p + newSize;
    }
    else if (0 == this->d_capacity) {
        // Because of {DRQS 99966534}, we check for zero capacity here and
        // handle it separately rather than falling into the case below.
        Vector_Imp temp(newSize, this->get_allocator());
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
    else if (newSize > this->d_capacity) {
        const size_type maxSize = max_size();
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newSize > maxSize)) {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                "vector<...>::resize(n): vector too long");
        }

        size_type newCapacity = Vector_Util::computeNewCapacity(
                                       newSize, this->d_capacity, maxSize);
        Vector_Imp temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        ArrayPrimitives::destructiveMoveAndInsert(
            temp.d_dataBegin_p,
            &this->d_dataEnd_p,
            this->d_dataBegin_p,
            this->d_dataEnd_p,
            this->d_dataEnd_p,
            newSize - this->size(),
            ContainerBase::allocator());

        temp.d_dataEnd_p += newSize;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
    else {
        ArrayPrimitives::defaultConstruct(this->d_dataEnd_p,
                                          newSize - this->size(),
                                          ContainerBase::allocator());
        this->d_dataEnd_p = this->d_dataBegin_p + newSize;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::resize(size_type         newSize,
                                               const VALUE_TYPE& value)
{
    // This function provides the *strong* exception guarantee (except when
    // the move constructor of a non-copy-insertable 'value_type' throws).

    if (newSize <= this->size()) {
        BloombergLP::bslalg::ArrayDestructionPrimitives::destroy(
                                            this->d_dataBegin_p + newSize,
                                            this->d_dataEnd_p,
                                            ContainerBase::allocator());
        this->d_dataEnd_p = this->d_dataBegin_p + newSize;
    }
    else {
        insert(this->d_dataEnd_p, newSize - this->size(), value);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::reserve(size_type newCapacity)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newCapacity > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                         "vector<...>::reserve(newCapacity): vector too long");
    }
    if (0 == this->d_capacity && 0 != newCapacity) {
        privateReserveEmpty(newCapacity);
    }
    else if (this->d_capacity < newCapacity) {
        Vector_Imp temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        ArrayPrimitives::destructiveMove(temp.d_dataBegin_p,
                                         this->d_dataBegin_p,
                                         this->d_dataEnd_p,
                                         ContainerBase::allocator());

        temp.d_dataEnd_p += this->size();
        this->d_dataEnd_p = this->d_dataBegin_p;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::shrink_to_fit()
{
    if (this->size() < this->d_capacity) {
        Vector_Imp temp(this->get_allocator());
        temp.privateReserveEmpty(this->size());
        ArrayPrimitives::destructiveMove(temp.d_dataBegin_p,
                                         this->d_dataBegin_p,
                                         this->d_dataEnd_p,
                                         ContainerBase::allocator());

        temp.d_dataEnd_p += this->size();
        this->d_dataEnd_p = this->d_dataBegin_p;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
}

                            // *** modifiers ***

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class VALUE_TYPE, class ALLOCATOR>
template <class... Args>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(Args&&...arguments)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_vector.h
template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                           )
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p);
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01,
          class Args_02>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
            BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
            BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
            BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
            BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
            BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
            BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
            BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
            BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
            BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
            BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
            BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
            BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
            BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
            BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
            BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
            BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
            BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
            BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
            BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
            BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
            BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
            BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
            BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
            BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
            BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
            BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
            BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
            BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
            BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
            BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
            BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
            BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
            BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
            BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
            BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
            BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class Args_01,
          class Args_02,
          class Args_03,
          class Args_04,
          class Args_05,
          class Args_06,
          class Args_07,
          class Args_08,
          class Args_09,
          class Args_10>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
            BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
            BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
            BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
            BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
            BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
            BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
            BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
            BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
            BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args_01, arguments_01),
                BSLS_COMPILERFEATURES_FORWARD(Args_02, arguments_02),
                BSLS_COMPILERFEATURES_FORWARD(Args_03, arguments_03),
                BSLS_COMPILERFEATURES_FORWARD(Args_04, arguments_04),
                BSLS_COMPILERFEATURES_FORWARD(Args_05, arguments_05),
                BSLS_COMPILERFEATURES_FORWARD(Args_06, arguments_06),
                BSLS_COMPILERFEATURES_FORWARD(Args_07, arguments_07),
                BSLS_COMPILERFEATURES_FORWARD(Args_08, arguments_08),
                BSLS_COMPILERFEATURES_FORWARD(Args_09, arguments_09),
                BSLS_COMPILERFEATURES_FORWARD(Args_10, arguments_10));
    }
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class VALUE_TYPE, class ALLOCATOR>
template <class... Args>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::emplace_back(
                           BSLS_COMPILERFEATURES_FORWARD_REF(Args)...arguments)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        emplace(this->d_dataEnd_p,
                BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
    }
}
// }}} END GENERATED CODE
#endif

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::push_back(const VALUE_TYPE& value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(ContainerBase::allocator(),
                                   this->d_dataEnd_p,
                                   value);
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        insert(this->d_dataEnd_p, value);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::push_back(
                              BloombergLP::bslmf::MovableRef<VALUE_TYPE> value)
{
    VALUE_TYPE& lvalue = value;
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(ContainerBase::allocator(),
                                   this->d_dataEnd_p,
                                   MoveUtil::move(lvalue));
        ++this->d_dataEnd_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        insert(this->d_dataEnd_p, MoveUtil::move(lvalue));
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::pop_back()
{
    BSLS_ASSERT_SAFE(!this->empty());

    AllocatorTraits::destroy(ContainerBase::allocator(),
                             --this->d_dataEnd_p);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename Vector_Imp<VALUE_TYPE, ALLOCATOR>::iterator
Vector_Imp<VALUE_TYPE, ALLOCATOR>::insert(const_iterator    position,
                                          const VALUE_TYPE& value)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <= this->end());

    const size_type index = position - this->begin();
    insert(position, size_type(1), value);
    return this->begin() + index;
}

template <class VALUE_TYPE, class ALLOCATOR>
typename Vector_Imp<VALUE_TYPE, ALLOCATOR>::iterator
Vector_Imp<VALUE_TYPE, ALLOCATOR>::insert(
                           const_iterator                             position,
                           BloombergLP::bslmf::MovableRef<VALUE_TYPE> value)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <= this->end());

    VALUE_TYPE& lvalue = value;

    const size_type index = position - this->begin();

    const iterator& pos = const_cast<const iterator&>(position);

    const size_type maxSize = max_size();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 > maxSize - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                               "vector<...>::insert(pos,rv): vector too long");
    }

    const size_type newSize = this->size() + 1;
    if (newSize > this->d_capacity) {
        size_type newCapacity = Vector_Util::computeNewCapacity(
                                                              newSize,
                                                              this->d_capacity,
                                                              maxSize);
        Vector_Imp temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        ArrayPrimitives::destructiveMoveAndEmplace(temp.d_dataBegin_p,
                                                   &this->d_dataEnd_p,
                                                   this->d_dataBegin_p,
                                                   pos,
                                                   this->d_dataEnd_p,
                                                   ContainerBase::allocator(),
                                                   MoveUtil::move(lvalue));

        temp.d_dataEnd_p += newSize;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
    else {
        ArrayPrimitives::insert(pos,
                                this->end(),
                                MoveUtil::move(lvalue),
                                ContainerBase::allocator());
        ++this->d_dataEnd_p;
    }

    return this->begin() + index;
}

template <class VALUE_TYPE, class ALLOCATOR>
typename Vector_Imp<VALUE_TYPE, ALLOCATOR>::iterator
Vector_Imp<VALUE_TYPE, ALLOCATOR>::insert(const_iterator    position,
                                          size_type         numElements,
                                          const VALUE_TYPE& value)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <= this->end());

    const size_type index = position - this->begin();
    const iterator& pos = const_cast<const iterator&>(position);

    const size_type maxSize = max_size();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                       numElements > maxSize - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                              "vector<...>::insert(pos,n,v): vector too long");
    }

    const size_type newSize = this->size() + numElements;
    if (newSize > this->d_capacity) {
        size_type newCapacity = Vector_Util::computeNewCapacity(
                                                              newSize,
                                                              this->d_capacity,
                                                              maxSize);
        Vector_Imp temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        ArrayPrimitives::destructiveMoveAndInsert(temp.d_dataBegin_p,
                                                  &this->d_dataEnd_p,
                                                  this->d_dataBegin_p,
                                                  pos,
                                                  this->d_dataEnd_p,
                                                  value,
                                                  numElements,
                                                  ContainerBase::allocator());

        temp.d_dataEnd_p += newSize;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
    else {
        ArrayPrimitives::insert(pos,
                                this->end(),
                                value,
                                numElements,
                                ContainerBase::allocator());
        this->d_dataEnd_p += numElements;
    }
    return this->begin() + index;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE_TYPE, class ALLOCATOR>
inline
typename Vector_Imp<VALUE_TYPE, ALLOCATOR>::iterator
Vector_Imp<VALUE_TYPE, ALLOCATOR>::insert(const_iterator position,
                                      std::initializer_list<VALUE_TYPE> values)
{
    return insert(position, values.begin(), values.end());
}
#endif

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename Vector_Imp<VALUE_TYPE, ALLOCATOR>::iterator
Vector_Imp<VALUE_TYPE, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <  this->end());

    return erase(position, position + 1);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename Vector_Imp<VALUE_TYPE, ALLOCATOR>::iterator
Vector_Imp<VALUE_TYPE, ALLOCATOR>::erase(const_iterator first,
                                         const_iterator last)
{
    BSLS_ASSERT_SAFE(this->begin() <= first);
    BSLS_ASSERT_SAFE(first         <= this->end());
    BSLS_ASSERT_SAFE(first         <= last);
    BSLS_ASSERT_SAFE(last          <= this->end());

    const size_type n = last - first;
    ArrayPrimitives::erase(const_cast<VALUE_TYPE *>(first),
                           const_cast<VALUE_TYPE *>(last),
                           this->d_dataEnd_p,
                           ContainerBase::allocator());
    this->d_dataEnd_p -= n;
    return const_cast<VALUE_TYPE *>(first);
}

template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::swap(
                                      Vector_Imp<VALUE_TYPE, ALLOCATOR>& other)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    if (AllocatorTraits::propagate_on_container_swap::value) {
        Vector_Util::swap(&this->d_dataBegin_p, &other.d_dataBegin_p);
        using std::swap;
        swap(ContainerBase::allocator(), other.ContainerBase::allocator());
    }
    else {
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                             this->get_allocator() == other.get_allocator())) {
            Vector_Util::swap(&this->d_dataBegin_p, &other.d_dataBegin_p);
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            Vector_Imp toOtherCopy(MoveUtil::move(*this),
                                   other.get_allocator());
            Vector_Imp toThisCopy( MoveUtil::move(other),
                                   this->get_allocator());

            Vector_Util::swap(&toOtherCopy.d_dataBegin_p,
                              &other.d_dataBegin_p);
            Vector_Util::swap(&toThisCopy. d_dataBegin_p,
                              &this->d_dataBegin_p);
        }
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::clear() BSLS_CPP11_NOEXCEPT
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(!this->empty())) {
        erase(this->begin(), this->end());
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
}

// ACCESSORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
typename Vector_Imp<VALUE_TYPE, ALLOCATOR>::allocator_type
Vector_Imp<VALUE_TYPE, ALLOCATOR>::get_allocator() const BSLS_CPP11_NOEXCEPT
{
    return ContainerBase::allocator();
}

                         // *** capacity ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename Vector_Imp<VALUE_TYPE, ALLOCATOR>::size_type
Vector_Imp<VALUE_TYPE, ALLOCATOR>::max_size() const BSLS_CPP11_NOEXCEPT
{
    return ContainerBase::allocator().max_size();
}

// FREE OPERATORS

                       // *** relational operators ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator==(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator!=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return ! (lhs == rhs);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator< (const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return 0 > BloombergLP::bslalg::RangeCompare::lexicographical(lhs.begin(),
                                                                  lhs.end(),
                                                                  lhs.size(),
                                                                  rhs.begin(),
                                                                  rhs.end(),
                                                                  rhs.size());
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator> (const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return rhs < lhs;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return !(rhs < lhs);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return !(lhs < rhs);
}

                            // ------------
                            // class vector
                            // ------------

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector() BSLS_CPP11_NOEXCEPT
: Base(ALLOCATOR())
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(const ALLOCATOR& basicAllocator)
                                                            BSLS_CPP11_NOEXCEPT
: Base(basicAllocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(size_type        initialSize,
                                      const ALLOCATOR& basicAllocator)
: Base(initialSize, basicAllocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(size_type         initialSize,
                                      const VALUE_TYPE& value,
                                      const ALLOCATOR&  basicAllocator)
: Base(initialSize, value, basicAllocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(INPUT_ITER       first,
                                      INPUT_ITER       last,
                                      const ALLOCATOR& basicAllocator)
: Base(first, last, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(
                              std::initializer_list<VALUE_TYPE> values,
                              const ALLOCATOR&                  basicAllocator)
: Base(values.begin(), values.end(), basicAllocator)
{
}
#endif

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(const vector& original)
: Base(original)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(
                               BloombergLP::bslmf::MovableRef<vector> original)
                                                            BSLS_CPP11_NOEXCEPT
: Base(MoveUtil::move(static_cast<Base&>(original)))
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(const vector&    original,
                                      const ALLOCATOR& basicAllocator)
: Base(original, basicAllocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(
                         BloombergLP::bslmf::MovableRef<vector> original,
                         const ALLOCATOR&                       basicAllocator)
: Base(MoveUtil::move(static_cast<Base&>(original)), basicAllocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::~vector()
{
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>&
vector<VALUE_TYPE, ALLOCATOR>::operator=(const vector& rhs)
{
    Base::operator=(rhs);
    return *this;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>&
vector<VALUE_TYPE, ALLOCATOR>::operator=(
                                    BloombergLP::bslmf::MovableRef<vector> rhs)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    vector& lvalue = rhs;
    Base::operator=(MoveUtil::move(*static_cast<Base *>(&lvalue)));
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>&
vector<VALUE_TYPE, ALLOCATOR>::operator=(
                                      std::initializer_list<VALUE_TYPE> values)
{
    Base::assign(values.begin(), values.end());
    return *this;
}
#endif

// FREE OPERATORS
template <class VALUE_TYPE,  class ALLOCATOR>
inline
bool operator==(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR> Base;
    return operator==(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE,  class ALLOCATOR>
inline
bool operator!=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR> Base;
    return operator!=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE,  class ALLOCATOR>
inline
bool operator<(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
               const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR> Base;
    return operator<(static_cast<const Base&>(lhs),
                     static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE,  class ALLOCATOR>
inline
bool operator>(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
               const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR> Base;
    return operator>(static_cast<const Base&>(lhs),
                     static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE,  class ALLOCATOR>
inline
bool operator<=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR> Base;
    return operator<=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE,  class ALLOCATOR>
inline
bool operator>=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR> Base;
    return operator>=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

// FREE FUNCTIONS

                       // *** specialized algorithms ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
void swap(vector<VALUE_TYPE, ALLOCATOR>& a,
          vector<VALUE_TYPE, ALLOCATOR>& b)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR> Base;

#if defined(BSLS_PLATFORM_CMP_MSVC)
    // Some platforms might not be willing to do a 'static_cast' here.

    Base *pa = reinterpret_cast<Base *>(&a);
    Base *pb = reinterpret_cast<Base *>(&b);
#else
    Base *pa = &a;
    Base *pb = &b;
#endif

    pa->swap(*pb);
}

// HASH SPECIALIZATIONS
template <class HASHALG, class VALUE_TYPE, class ALLOCATOR>
inline
void hashAppend(HASHALG& hashAlg, const vector<VALUE_TYPE, ALLOCATOR>& input)
{
    using ::BloombergLP::bslh::hashAppend;
    typedef typename vector<VALUE_TYPE, ALLOCATOR>::const_iterator ci_t;
    hashAppend(hashAlg, input.size());
    for (ci_t b = input.begin(), e = input.end(); b != e; ++b) {
        hashAppend(hashAlg, *b);
    }
}

                   // -------------------------------------
                   // class vector<VALUE_TYPE *, ALLOCATOR>
                   // -------------------------------------

// FREE OPERATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator==(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;
    return operator==(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator!=(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;
    return operator!=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;
    return operator<(static_cast<const Base&>(lhs),
                     static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;
    return operator>(static_cast<const Base&>(lhs),
                     static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<=(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;
    return operator<=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>=(const vector<VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;
    return operator>=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

// FREE FUNCTIONS
template <class VALUE_TYPE, class ALLOCATOR>
inline
void swap(vector<VALUE_TYPE *, ALLOCATOR>& a,
          vector<VALUE_TYPE *, ALLOCATOR>& b)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;

#if defined(BSLS_PLATFORM_CMP_MSVC)
    // Windows is not willing to do a 'static_cast' here.

    Base *pa = reinterpret_cast<Base *>(&a);
    Base *pb = reinterpret_cast<Base *>(&b);
#else
    Base *pa = &a;
    Base *pb = &b;
#endif

    pa->swap(*pb);
}

             // -------------------------------------------
             // class vector<const VALUE_TYPE *, ALLOCATOR>
             // -------------------------------------------

// FREE OPERATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator==(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
#if defined(BSLSTL_VECTOR_INCONSISTENT_CV_FOR_FUNCTION_POINTERS)
    typedef typename bsl::conditional<bsl::is_function<VALUE_TYPE>::value,
                                      void,
                                      const void>::type VoidType;
    typedef typename ALLOCATOR::template rebind<VoidType *>::other BaseAlloc;
    typedef Vector_Imp<VoidType *, BaseAlloc>                      Base;
#else
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
#endif
    return operator==(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator!=(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
#if defined(BSLSTL_VECTOR_INCONSISTENT_CV_FOR_FUNCTION_POINTERS)
    typedef typename bsl::conditional<bsl::is_function<VALUE_TYPE>::value,
                                      void,
                                      const void>::type VoidType;
    typedef typename ALLOCATOR::template rebind<VoidType *>::other BaseAlloc;
    typedef Vector_Imp<VoidType *, BaseAlloc>                      Base;
#else
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
#endif
    return operator!=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
#if defined(BSLSTL_VECTOR_INCONSISTENT_CV_FOR_FUNCTION_POINTERS)
    typedef typename bsl::conditional<bsl::is_function<VALUE_TYPE>::value,
                                      void,
                                      const void>::type VoidType;
    typedef typename ALLOCATOR::template rebind<VoidType *>::other BaseAlloc;
    typedef Vector_Imp<VoidType *, BaseAlloc>                      Base;
#else
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
#endif
    return operator<(static_cast<const Base&>(lhs),
                     static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
#if defined(BSLSTL_VECTOR_INCONSISTENT_CV_FOR_FUNCTION_POINTERS)
    typedef typename bsl::conditional<bsl::is_function<VALUE_TYPE>::value,
                                      void,
                                      const void>::type VoidType;
    typedef typename ALLOCATOR::template rebind<VoidType *>::other BaseAlloc;
    typedef Vector_Imp<VoidType *, BaseAlloc>                      Base;
#else
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
#endif
    return operator>(static_cast<const Base&>(lhs),
                     static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<=(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
#if defined(BSLSTL_VECTOR_INCONSISTENT_CV_FOR_FUNCTION_POINTERS)
    typedef typename bsl::conditional<bsl::is_function<VALUE_TYPE>::value,
                                      void,
                                      const void>::type VoidType;
    typedef typename ALLOCATOR::template rebind<VoidType *>::other BaseAlloc;
    typedef Vector_Imp<VoidType *, BaseAlloc>                      Base;
#else
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
#endif
    return operator<=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>=(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
#if defined(BSLSTL_VECTOR_INCONSISTENT_CV_FOR_FUNCTION_POINTERS)
    typedef typename bsl::conditional<bsl::is_function<VALUE_TYPE>::value,
                                      void,
                                      const void>::type VoidType;
    typedef typename ALLOCATOR::template rebind<VoidType *>::other BaseAlloc;
    typedef Vector_Imp<VoidType *, BaseAlloc>                      Base;
#else
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
#endif
    return operator>=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

// FREE FUNCTIONS
template <class VALUE_TYPE, class ALLOCATOR>
inline
void swap(vector<const VALUE_TYPE *, ALLOCATOR>& a,
          vector<const VALUE_TYPE *, ALLOCATOR>& b)
              BSLS_CPP11_NOEXCEPT_SPECIFICATION(BSLS_CPP11_PROVISIONALLY_FALSE)
{
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;

#if defined(BSLSTL_VECTOR_INCONSISTENT_CV_FOR_FUNCTION_POINTERS)
    Base *pa = reinterpret_cast<Base *>(&a);
    Base *pb = reinterpret_cast<Base *>(&b);
#else
    Base *pa = &a;
    Base *pb = &b;
#endif

    pa->swap(*pb);
}

}  // close namespace bsl

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *sequence* containers:
//: o A sequence container defines STL iterators.
//: o A sequence container is bitwise movable if the allocator is bitwise
//:     movable.
//: o A sequence container uses 'bslma' allocators if the (template parameter)
//:     type 'ALLOCATOR' is convertible from 'bslma::Allocator *'.

namespace BloombergLP {

namespace bslalg {

template <class VALUE_TYPE, class ALLOCATOR>
struct HasStlIterators<bsl::vector<VALUE_TYPE, ALLOCATOR> > : bsl::true_type
{};

}  // close namespace bslalg

namespace bslmf {

template <class VALUE_TYPE, class ALLOCATOR>
struct IsBitwiseMoveable<bsl::vector<VALUE_TYPE, ALLOCATOR> >
    : IsBitwiseMoveable<ALLOCATOR>
{};

}  // close namespace bslmf

namespace bslma {

template <class VALUE_TYPE, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::vector<VALUE_TYPE, ALLOCATOR> >
    : bsl::is_convertible<Allocator *, ALLOCATOR>::type
{};

}  // close namespace bslma

}  // close enterprise namespace

#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
extern template class bsl::Vector_ImpBase<bool>;
extern template class bsl::Vector_ImpBase<char>;
extern template class bsl::Vector_ImpBase<signed char>;
extern template class bsl::Vector_ImpBase<unsigned char>;
extern template class bsl::Vector_ImpBase<short>;
extern template class bsl::Vector_ImpBase<unsigned short>;
extern template class bsl::Vector_ImpBase<int>;
extern template class bsl::Vector_ImpBase<unsigned int>;
extern template class bsl::Vector_ImpBase<long>;
extern template class bsl::Vector_ImpBase<unsigned long>;
extern template class bsl::Vector_ImpBase<long long>;
extern template class bsl::Vector_ImpBase<unsigned long long>;
extern template class bsl::Vector_ImpBase<float>;
extern template class bsl::Vector_ImpBase<double>;
extern template class bsl::Vector_ImpBase<long double>;
extern template class bsl::Vector_ImpBase<void *>;  // common base for all
                                                    // vectors of pointers
extern template class bsl::Vector_Imp<bool>;
extern template class bsl::Vector_Imp<char>;
extern template class bsl::Vector_Imp<signed char>;
extern template class bsl::Vector_Imp<unsigned char>;
extern template class bsl::Vector_Imp<short>;
extern template class bsl::Vector_Imp<unsigned short>;
extern template class bsl::Vector_Imp<int>;
extern template class bsl::Vector_Imp<unsigned int>;
extern template class bsl::Vector_Imp<long>;
extern template class bsl::Vector_Imp<unsigned long>;
extern template class bsl::Vector_Imp<long long>;
extern template class bsl::Vector_Imp<unsigned long long>;
extern template class bsl::Vector_Imp<float>;
extern template class bsl::Vector_Imp<double>;
extern template class bsl::Vector_Imp<long double>;
extern template class bsl::Vector_Imp<void *>;  // common base for all
                                                // vectors of pointers
extern template class bsl::vector<bool>;
extern template class bsl::vector<char>;
extern template class bsl::vector<signed char>;
extern template class bsl::vector<unsigned char>;
extern template class bsl::vector<short>;
extern template class bsl::vector<unsigned short>;
extern template class bsl::vector<int>;
extern template class bsl::vector<unsigned int>;
extern template class bsl::vector<long>;
extern template class bsl::vector<unsigned long>;
extern template class bsl::vector<long long>;
extern template class bsl::vector<unsigned long long>;
extern template class bsl::vector<float>;
extern template class bsl::vector<double>;
extern template class bsl::vector<long double>;
extern template class bsl::vector<void *>;
#endif

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
