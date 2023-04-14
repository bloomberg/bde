// bslstl_vector.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTL_VECTOR
#define INCLUDED_BSLSTL_VECTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant vector class.
//
//@CLASSES:
//  bsl::vector: STL-compatible vector template
//
//@CANONICAL_HEADER: bsl_vector.h
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
// A 'vector' is a fully Value-Semantic Type (see {'bsldoc_glossary'}) only if
// the supplied 'VALUE_TYPE' template parameter is fully value-semantic.  It is
// possible to instantiate a 'vector' with a 'VALUE_TYPE' parameter that does
// not have a full set of value-semantic operations, but then some methods of
// the container may not be instantiable.  The following terminology, adopted
// from the C++11 standard, is used in the function documentation of 'vector'
// to describe a function's requirements for the 'VALUE_TYPE' template
// parameter.  These terms are also defined in section [17.6.3.1] of the C++11
// standard.  Note that, in the context of a 'vector' instantiation, the
// requirements apply specifically to the vector's entry type, 'value_type',
// which is an alias for 'VALUE_TYPE'.
//
///Glossary
///--------
//..
//  Legend
//  ------
//  'X'    - denotes an allocator-aware container type (e.g., 'vector')
//  'T'    - 'value_type' associated with 'X'
//  'A'    - type of the allocator used by 'X'
//  'm'    - lvalue of type 'A' (allocator)
//  'p'    - address ('T *') of uninitialized storage for a 'T' within an 'X'
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
// If the (template parameter) type 'ALLOCATOR' of a 'vector' instantiation' is
// 'bsl::allocator', then objects of that vector type will conform to the
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
//  | a.swap(b), swap(a, b)                   | O[1] if 'a' and 'b' use the   |
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
///Comparing a vector of floating point values
///-------------------------------------------
// The comparison operator performs a bit-wise comparison for floating point
// types ('float' and 'double'), which produces results for NaN, +0, and -0
// values that do not meet the guarantees provided by the standard.
// The 'bslmf::IsBitwiseEqualityComparable' trait for 'double' and 'float'
// types returns 'true' which is incorrect because a comparison with a NaN
// value is always 'false', and -0 and +0 are equal.
//..
//    bsl::vector<double> v;
//    v.push_back(bsl::numeric_limits<double>::quiet_NaN());
//    ASSERT(v == v);   // This assertion will *NOT* fail!
//..
// Addressing this issue, i.e., updating 'bslmf::IsBitwiseEqualityComparable'
// to return 'false' for floating point types, could potentially destabilize
// production software so the change (for the moment) has not been made.
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
//      void insertColumn(int columnIndex);
//          // Insert, into this matrix, an column at the specified
//          // 'columnIndex'.  All elements of the (template parameter) 'TYPE'
//          // in the column will have the default-constructed value.  The
//          // behavior is undefined unless '0 <= columnIndex <= numColumns()'.
//
//      void insertRow(int rowIndex);
//          // Insert, into this matrix, a row at the specified 'rowIndex'.
//          // All elements of the (template parameter) 'TYPE' in the row will
//          // have the default-constructed value.  The behavior is undefined
//          // unless '0 <= rowIndex <= numRows()'.
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
//  void MyMatrix<TYPE>::insertRow(int rowIndex)
//  {
//      typename MatrixType::iterator itr =
//          d_matrix.insert(d_matrix.begin() + rowIndex, RowType());
//      itr->resize(d_numColumns);
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

#include <bslscm_version.h>

#include <bslstl_algorithm.h>
#include <bslstl_hash.h>
#include <bslstl_iterator.h>
#include <bslstl_iteratorutil.h>
#include <bslstl_stdexceptutil.h>

#include <bslalg_arraydestructionprimitives.h>
#include <bslalg_arrayprimitives.h>
#include <bslalg_containerbase.h>
#include <bslalg_rangecompare.h>
#include <bslalg_synththreewayutil.h>
#include <bslalg_swaputil.h>
#include <bslalg_typetraithasstliterators.h>

#include <bslh_hash.h>

#include <bslma_allocator.h>
#include <bslma_allocatortraits.h>
#include <bslma_autodestructor.h>
#include <bslma_isstdallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isfundamental.h>
#include <bslmf_isintegral.h>
#include <bslmf_issame.h>
#include <bslmf_matchanytype.h>
#include <bslmf_matcharithmetictype.h>
#include <bslmf_movableref.h>
#include <bslmf_nil.h>
#include <bslmf_typeidentity.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <cstddef>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)

#include <initializer_list>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#include <stdexcept>
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslstl_vector.h
# define COMPILING_BSLSTL_VECTOR_H
# include <bslstl_vector_cpp03.h>
# undef COMPILING_BSLSTL_VECTOR_H
#else

namespace bsl {

// Forward declarations

template <class VALUE_TYPE, class ITERATOR>
class vector_UintPtrConversionIterator;

                          // ==================
                          // struct Vector_Util
                          // ==================

struct Vector_Util {
    // This 'struct' provides a namespace for implementing the 'swap' member
    // function of 'vector<VALUE_TYPE, ALLOCATOR>'.  'swap' can be implemented
    // irrespective of the 'VALUE_TYPE' or 'ALLOCATOR' template parameters,
    // which is why we implement it in this non-parameterized, non-inlined
    // utility.

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


                          // ===================================
                          // class Vector_DeduceIteratorCategory
                          // ===================================

template <class BSLSTL_ITERATOR,
          bool  BSLSTL_NOTSPECIALIZED = is_fundamental<BSLSTL_ITERATOR>::value>
struct Vector_DeduceIteratorCategory {
    // This 'struct' provides a primitive means to distinguish between iterator
    // types and fundamental types, in order to dispatch to the correct
    // implementation of a function template (or constructor template) passed
    // two arguments of identical type.  By default, it is assumed that any
    // type that is not a fundamental type, as determined by the type trait
    // 'bsl::is_fundamental', must be an iterator type.  'std::iterator_traits'
    // is updated in C++17 to provide a SFINAE-friendly instantiation of the
    // primary-template for types that do not provide all of the nested typedef
    // names, but we cannot portably rely on such a scheme yet.

    // PUBLIC TYPES
    typedef typename bsl::iterator_traits<BSLSTL_ITERATOR>::iterator_category
                                                                          type;
};

template <class BSLSTL_ITERATOR>
struct Vector_DeduceIteratorCategory<BSLSTL_ITERATOR, true> {
    // This partial specialization of the 'struct' template for fundamental
    // types provides a nested 'type' that is not an iterator category, so can
    // be used to control the internal dispatch of function template overloads
    // taking two arguments of the same type.

    // PUBLIC TYPES
    typedef BloombergLP::bslmf::Nil type;
};


                        // ======================================
                        // class vector_UintPtrConversionIterator
                        // ======================================

template <class TARGET, class ITERATOR, bool = is_integral<ITERATOR>::value>
struct vector_ForwardIteratorForPtrs {
    // This metafunction provides an appropriate iterator adaptor for the
    // specified (template parameter) type 'ITERATOR' in order to implement
    // members of the 'vector' partial template specialization for vectors of
    // pointers to the (template parameter) type 'TARGET'.  The metafunction
    // will return the original 'ITERATOR' type unless it truly is an iterator,
    // using 'is_integral' as a proxy for testing that a type is NOT an
    // iterator.  This is needed to disambiguate only the cases of users
    // passing '0' as a null-pointer value to functions requesting a number of
    // identical copies of an element.

    // PUBLIC TYPES
    typedef ITERATOR type;
};

template <class TARGET, class ITERATOR>
struct vector_ForwardIteratorForPtrs<TARGET, ITERATOR, false> {
    // This metafunction specialization provides an appropriate iterator
    // adaptor for the specified (template parameter) type 'ITERATOR' in order
    // to implement members of the 'vector' partial template specialization for
    // vectors of pointers to the (template parameter) type 'TARGET'.

    // PUBLIC TYPES
    typedef vector_UintPtrConversionIterator<TARGET *, ITERATOR> type;
};

#if defined(BSLS_ASSERT_SAFE_IS_USED)

template <class BSLSTL_ITERATOR>
struct Vector_IsRandomAccessIterator :
    bsl::is_same<typename Vector_DeduceIteratorCategory<BSLSTL_ITERATOR>::type,
                 bsl::random_access_iterator_tag>::type
{
};


                          // =======================
                          // class Vector_RangeCheck
                          // =======================

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
    isInvalidRange(BSLSTL_ITERATOR, BSLSTL_ITERATOR);
        // Return 'false'.  Note that we know of no way to identify an input
        // iterator range that is guaranteed to be invalid.

    template <class BSLSTL_ITERATOR>
    static
    typename bsl::enable_if<
             Vector_IsRandomAccessIterator<BSLSTL_ITERATOR>::VALUE, bool>::type
    isInvalidRange(BSLSTL_ITERATOR first, BSLSTL_ITERATOR last);
        // Return 'true' if 'last < first', and 'false' otherwise.  The
        // behavior is undefined unless both 'first' and 'last' are valid
        // iterators that refer to the same range.
};

#endif

                          // ================
                          // class vectorBase
                          // ================

template <class VALUE_TYPE>
class vectorBase {
    // This class describes the basic layout for a vector class, to be included
    // into the 'vector' layout *before* the allocator (provided by
    // 'bslalg::ContainerBase') to take better advantage of cache prefetching.
    // It is parameterized by 'VALUE_TYPE' only, and implements the portion of
    // 'vector' that does not need to know about its (template parameter) type
    // 'ALLOCATOR' (in order to generate shorter debug strings).  This class
    // intentionally has *no* creators (other than the compiler-generated
    // ones).

    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil     MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

  protected:
    // PROTECTED DATA
    VALUE_TYPE  *d_dataBegin_p;  // beginning of data storage (owned)
    VALUE_TYPE  *d_dataEnd_p;    // one past the end of data storage
    std::size_t  d_capacity;     // capacity of data storage in # of elements

  public:
    // PUBLIC TYPES
    typedef VALUE_TYPE                             value_type;
    typedef VALUE_TYPE&                            reference;
    typedef VALUE_TYPE const&                      const_reference;
    typedef VALUE_TYPE                            *iterator;
    typedef VALUE_TYPE const                      *const_iterator;
    typedef std::size_t                            size_type;
    typedef std::ptrdiff_t                         difference_type;
    typedef bsl::reverse_iterator<iterator>        reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

  public:
    // CREATORS
    vectorBase();
        // Create an empty base object with no capacity.

    // MANIPULATORS
    void adopt(BloombergLP::bslmf::MovableRef<vectorBase> base);
        // Adopt all outstanding memory allocations associated with the
        // specified 'base' object.  The behavior is undefined unless this
        // object is in a default-constructed state.

                             // *** iterators ***

    iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first element
        // in this vector, or the past-the-end iterator if this vector is
        // empty.

    iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end iterator providing modifiable access to this
        // vector.

    reverse_iterator rbegin() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // element in this vector, and the past-the-end reverse iterator if
        // this vector is empty.

    reverse_iterator rend() BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end reverse iterator providing modifiable access
        // to this vector.

                          // *** element access ***

    reference operator[](size_type position);
        // Return a reference providing modifiable access to the element at the
        // specified 'position' in this vector.  The behavior is undefined
        // unless 'position < size()'.

    reference at(size_type position);
        // Return a reference providing modifiable access to the element at the
        // specified 'position' in this vector.  Throw a 'std::out_of_range'
        // exception if 'position >= size()'.

    reference front();
        // Return a reference providing modifiable access to the first element
        // in this vector.  The behavior is undefined unless this vector is not
        // empty.

    reference back();
        // Return a reference providing modifiable access to the last element
        // in this vector.  The behavior is undefined unless this vector is not
        // empty.

    VALUE_TYPE *data() BSLS_KEYWORD_NOEXCEPT;
        // Return the address of the modifiable first element in this vector,
        // or a valid, but non-dereferenceable pointer value if this vector is
        // empty.

    // ACCESSORS

                             // *** iterators ***

    const_iterator  begin() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // element in this vector, and the past-the-end iterator if this vector
        // is empty.

    const_iterator  end() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end (forward) iterator providing non-modifiable
        // access to this vector.

    const_reverse_iterator  rbegin() const BSLS_KEYWORD_NOEXCEPT;
    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last element in this vector, and the past-the-end reverse iterator
        // if this vector is empty.

    const_reverse_iterator  rend() const BSLS_KEYWORD_NOEXCEPT;
    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end reverse iterator providing non-modifiable
        // access to this vector.

                            // *** capacity ***

    size_type size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of elements in this vector.

    size_type capacity() const BSLS_KEYWORD_NOEXCEPT;
        // Return the capacity of this vector, i.e., the maximum number of
        // elements for which resizing is guaranteed not to trigger a
        // reallocation.

    bool empty() const BSLS_KEYWORD_NOEXCEPT;
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

    const VALUE_TYPE *data() const BSLS_KEYWORD_NOEXCEPT;
        // Return the address of the non-modifiable first element in this
        // vector, or a valid, but non-dereferenceable pointer value if this
        // vector is empty.
};

                        // ============
                        // class vector
                        // ============

template <class VALUE_TYPE, class ALLOCATOR = allocator<VALUE_TYPE> >
class vector : public  vectorBase<VALUE_TYPE>
             , private BloombergLP::bslalg::ContainerBase<ALLOCATOR> {
    // This class template provides an STL-compliant 'vector' that conforms to
    // the 'bslma::Allocator' model.  For the requirements of a vector class,
    // consult the C++11 standard.  In particular, this implementation offers
    // the general rules that:
    //
    //: 1 A call to any method that would result in a vector having a size
    //:   or capacity greater than the value returned by 'max_size' triggers a
    //:   call to 'bslstl::StdExceptUtil::throwLengthError'.
    //:
    //: 2 A call to an 'at' method that attempts to access a position outside
    //:   of the valid range of a vector triggers a call to
    //:   'bslstl::StdExceptUtil::throwOutOfRange'.
    //
    // Note that portions of the standard methods are implemented in
    // 'vectorBase', which is parameterized on only 'VALUE_TYPE' in order to
    // generate smaller debug strings.
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
    typedef BloombergLP::bslalg::ArrayPrimitives      ArrayPrimitives;
        // This 'typedef' is an alias for a utility class that provides many
        // useful functions that operate on arrays.

    typedef BloombergLP::bslmf::MovableRefUtil        MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

    typedef allocator_traits<ALLOCATOR>               AllocatorTraits;
        // This 'typedef' is an alias for the allocator traits type associated
        // with this container.

  public:
    // PUBLIC TYPES
    typedef VALUE_TYPE                                value_type;
    typedef ALLOCATOR                                 allocator_type;
    typedef VALUE_TYPE&                               reference;
    typedef const VALUE_TYPE&                         const_reference;

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
    typedef vectorBase<VALUE_TYPE>                    ImpBase;
        // Implementation base type, with iterator-related functionality.

    typedef BloombergLP::bslalg::ContainerBase<ALLOCATOR> ContainerBase;
        // Container base type, containing the allocator and applying the empty
        // base class optimization (EBO) whenever appropriate.

    class Proctor {
        // This class provides a proctor for deallocating an array of
        // 'VALUE_TYPE' objects, to be used in the 'vector' constructors.

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
    template <class FWD_ITER>
    void constructFromRange(FWD_ITER              first,
                            FWD_ITER              last,
                            std::forward_iterator_tag);
    template <class INPUT_ITER>
    void constructFromRange(INPUT_ITER          first,
                            INPUT_ITER          last,
                            std::input_iterator_tag);
        // Populate a default-constructed vector with the values held in the
        // specified range '[first, last)'.  The additional
        // 'std::*iterator__tag' should be a default-constructed tag that
        // corresponds to that found in 'std::iterator_traits' for the
        // (template parameter) '*_ITER' type.  This method should be called
        // only from a constructor.  The behavior is undefined unless
        // 'first != last'.

    template <class INTEGRAL>
    void constructFromRange(INTEGRAL          initialSize,
                            INTEGRAL          value,
                            BloombergLP::bslmf::Nil);
        // Populate a default-constructed vector with the specified
        // 'initialSize' elements, where each such element is a copy of the
        // specified 'value'.  The 'bslmf::Nil' traits value distinguished this
        // overload of two identical (presumed integral) types from the pair of
        // iterator overloads above.  This method should be called only from a
        // constructor.


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

    void privateMoveInsert(vector         *fromVector,
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

    vector() BSLS_KEYWORD_NOEXCEPT;
    explicit vector(const ALLOCATOR& basicAllocator) BSLS_KEYWORD_NOEXCEPT;
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
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
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
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Create a vector having the same value as the specified 'original'
        // object by moving (in constant time) the contents of 'original' to
        // the new vector.  The allocator associated with 'original' is
        // propagated for use in the newly-created vector.  'original' is left
        // in a valid but unspecified state.

    vector(const vector& original,
                const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a vector having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // This method requires that the (template parameter) type 'VALUE_TYPE'
        // be 'copy-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).  Note that a 'bslma::Allocator *' can be supplied
        // for 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    vector(BloombergLP::bslmf::MovableRef<vector> original,
                const typename type_identity<ALLOCATOR>::type& basicAllocator);
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
                                                                    // IMPLICIT
        // Create a vector and insert (in order) each 'VALUE_TYPE' object in
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

    vector& operator=(
            BloombergLP::bslmf::MovableRef<vector<VALUE_TYPE, ALLOCATOR> > rhs)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
              AllocatorTraits::propagate_on_container_move_assignment::value ||
              AllocatorTraits::is_always_equal::value);
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
        // vector and then inserting (in order) each 'VALUE_TYPE' object in the
        // specified 'values' initializer list, and return a reference
        // providing modifiable access to this object.  If an exception is
        // thrown, '*this' is left in a valid but unspecified state.  This
        // method requires that the (template parameter) type 'VALUE_TYPE' be
        // 'copy-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).

    void assign(std::initializer_list<VALUE_TYPE> values);
        // Assign to this object the value resulting from first clearing this
        // vector and then inserting (in order) each 'VALUE_TYPE' object in the
        // specified 'values' initializer list.  If an exception is thrown,
        // '*this' is left in a valid but unspecified state.  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'copy-insertable' into this vector (see {Requirements on
        // 'VALUE_TYPE'}).
#endif

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last);
        // Assign to this object the value resulting from first clearing this
        // vector and then inserting (in order) each 'value_type' object in the
        // range starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  If an exception is
        // thrown, '*this' is left in a valid but unspecified state.  Throw
        // 'std::length_error' if 'distance(first,last) > max_size()'.  The
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
    VALUE_TYPE &emplace_back(Args&&... arguments);
        // Append to the end of this vector a newly created 'value_type'
        // object, constructed by forwarding 'get_allocator()' (if required)
        // and the specified (variable number of) 'arguments' to the
        // corresponding constructor of 'value_type'.  Return a reference
        // providing modifiable access to the inserted element.  If an
        // exception is thrown (other than by the move constructor of a
        // non-copy-insertable 'value_type'), '*this' is unaffected.  This
        // method requires that the (template parameter) type 'VALUE_TYPE' be
        // 'move-insertable' into this vector and 'emplace-constructible' from
        // 'arguments' (see
        // {Requirements on 'VALUE_TYPE'}).
#endif

    void push_back(const VALUE_TYPE& value);
        // Append to the end of this vector a copy of the specified 'value'.
        // If an exception is thrown, '*this' is unaffected.  Throw
        // 'std::length_error' if 'size() == max_size()'.  This method
        // requires that the (template parameter) type 'VALUE_TYPE' be
        // 'copy-constructible' (see {Requirements on 'VALUE_TYPE'}).

    void push_back(BloombergLP::bslmf::MovableRef<VALUE_TYPE> value);
        // Append to the end of this vector the specified move-insertable
        // 'value'.  'value' is left in a valid but unspecified state.  If an
        // exception is thrown (other than by the move constructor of a
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
            vector temp(this->get_allocator());
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
#endif

    iterator insert(const_iterator position, const VALUE_TYPE& value);
        // Insert at the specified 'position' in this vector a copy of the
        // specified 'value', and return an iterator referring to the newly
        // inserted element.  If an exception is thrown (other than by the copy
        // constructor, move constructor, assignment operator, or move
        // assignment operator of 'VALUE_TYPE'), '*this' is unaffected.  Throw
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
        // assignment operator of 'VALUE_TYPE'), 'this' is unaffected.  Throw
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
        // 'VALUE_TYPE'), '*this' is unaffected.  Throw 'std::length_error' if
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
        // Insert at the specified 'position' in this vector each 'VALUE_TYPE'
        // object in the specified 'values' initializer list, and return an
        // iterator referring to the first newly inserted element.  If an
        // exception is thrown (other than by the copy constructor, move
        // constructor, assignment operator, and move assignment operator of
        // 'VALUE_TYPE'), '*this' is unaffected.  Throw 'std::length_error' if
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

    void swap(vector& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                         AllocatorTraits::propagate_on_container_swap::value ||
                         AllocatorTraits::is_always_equal::value);
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

    void clear() BSLS_KEYWORD_NOEXCEPT;
        // Remove all elements from this vector making its size 0.  Note that
        // although this vector is empty after this method returns, it
        // preserves the same capacity it had before the method was called.

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return (a copy of) the allocator used for memory allocation by this
        // vector.

    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
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

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
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
#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class VALUE_TYPE, class ALLOCATOR>
BloombergLP::bslalg::SynthThreeWayUtil::Result<VALUE_TYPE> operator<=>(
                                     const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                                     const vector<VALUE_TYPE, ALLOCATOR>& rhs);
    // Perform a lexicographic three-way comparison of the specified 'lhs' and
    // the specified 'rhs' vectors by using the comparison operators of
    // 'VALUE_TYPE' on each element; return the result of that comparison.

#else

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

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class VALUE_TYPE, class ALLOCATOR, class BDE_OTHER_TYPE>
typename vector<VALUE_TYPE, ALLOCATOR>::size_type
erase(vector<VALUE_TYPE, ALLOCATOR>& vec, const BDE_OTHER_TYPE& value);
    // Erase all the elements in the specified vector 'vec' that compare equal
    // to the specified 'value'.  Return the number of elements erased.

template <class VALUE_TYPE, class ALLOCATOR, class PREDICATE>
typename vector<VALUE_TYPE, ALLOCATOR>::size_type
erase_if(vector<VALUE_TYPE, ALLOCATOR>& vec, PREDICATE predicate);
    // Erase all the elements in the specified vector 'vec' that satisfy the
    // specified predicate 'predicate'.  Return the number of elements erased.

template <class VALUE_TYPE, class ALLOCATOR>
void swap(vector<VALUE_TYPE, ALLOCATOR>& a,
          vector<VALUE_TYPE, ALLOCATOR>& b)
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


                   // =====================================
                   // class vector<VALUE_TYPE *, ALLOCATOR>
                   // =====================================

template <class VALUE_TYPE, class ALLOCATOR>
class vector<VALUE_TYPE *, ALLOCATOR>
{
    // This partial specialization of 'vector' for pointer types to a (template
    // parameter) 'VALUE_TYPE' type is implemented in terms of
    // 'vector<UintPtr>' to reduce the amount of code generated.  Note that
    // this specialization rebinds the (template parameter) 'ALLOCATOR' type to
    // an allocator of 'UintPtr' so as to satisfy the invariant in the 'vector'
    // base class.  Note that the contract for all members is the same as the
    // primary template, so documentation is not repeated to avoid accidentally
    // introducing inconsistency over time.

    // PRIVATE TYPES
    typedef BloombergLP::bsls::Types::UintPtr                   UintPtr;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
    typedef typename allocator_traits<ALLOCATOR>::
                                template rebind_alloc<UintPtr>  ImplAlloc;
#else
    typedef typename ALLOCATOR::template rebind<UintPtr>::other ImplAlloc;
#endif
    typedef vector<UintPtr, ImplAlloc>                          Impl;
    typedef BloombergLP::bslmf::MovableRefUtil                  MoveUtil;

    // PRIVATE DATA
    Impl d_impl;  // The 'UintPtr' vector used for the implementation.

  public:
    // PUBLIC TYPES
    typedef VALUE_TYPE                           *value_type;
    typedef value_type&                           reference;
    typedef const value_type&                     const_reference;
    typedef VALUE_TYPE                          **iterator;
    typedef VALUE_TYPE *const                    *const_iterator;
    typedef std::size_t                           size_type;
    typedef std::ptrdiff_t                        difference_type;
    typedef ALLOCATOR                             allocator_type;
    typedef typename ALLOCATOR::pointer           pointer;
    typedef typename ALLOCATOR::const_pointer     const_pointer;
    typedef bsl::reverse_iterator<iterator>       reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

                      // *** construct/copy/destroy ***

    // CREATORS
    vector() BSLS_KEYWORD_NOEXCEPT;

    explicit vector(const ALLOCATOR& basicAllocator) BSLS_KEYWORD_NOEXCEPT;

    explicit vector(size_type        initialSize,
                    const ALLOCATOR& basicAllocator = ALLOCATOR());

    vector(size_type         initialSize,
           VALUE_TYPE       *value,
           const ALLOCATOR&  basicAllocator = ALLOCATOR());

    template <class INPUT_ITER>
    vector(INPUT_ITER       first,
           INPUT_ITER       last,
           const ALLOCATOR& basicAllocator = ALLOCATOR());

    vector(const vector& original);

    vector(BloombergLP::bslmf::MovableRef<vector> original)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT

    vector(const vector& original,
                const typename type_identity<ALLOCATOR>::type& basicAllocator);

    vector(BloombergLP::bslmf::MovableRef<vector> original,
                const typename type_identity<ALLOCATOR>::type& basicAllocator);


#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    vector(std::initializer_list<VALUE_TYPE *> values,
           const ALLOCATOR&                    basicAllocator = ALLOCATOR());
#endif

    ~vector();

    // MANIPULATORS
    vector& operator=(const vector& rhs);
    vector& operator=(
          BloombergLP::bslmf::MovableRef<vector<VALUE_TYPE *, ALLOCATOR> > rhs)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                       d_impl = MoveUtil::move(MoveUtil::access(rhs).d_impl)));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    vector& operator=(std::initializer_list<VALUE_TYPE *> values);

    void assign(std::initializer_list<VALUE_TYPE *> values);

#endif

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last);
    void assign(size_type numElements, VALUE_TYPE *value);


                             // *** iterators ***

    iterator begin() BSLS_KEYWORD_NOEXCEPT;
    iterator end() BSLS_KEYWORD_NOEXCEPT;

    reverse_iterator rbegin() BSLS_KEYWORD_NOEXCEPT;
    reverse_iterator rend() BSLS_KEYWORD_NOEXCEPT;

                          // *** element access ***

    reference operator[](size_type position);
    reference at(size_type position);

    reference front();
    reference back();

    VALUE_TYPE **data() BSLS_KEYWORD_NOEXCEPT;

                             // *** capacity ***

    void resize(size_type newLength);
    void resize(size_type newLength, VALUE_TYPE *value);

    void reserve(size_type newCapacity);
    void shrink_to_fit();

                            // *** modifiers ***

    value_type &emplace_back();

# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class ARG>
    value_type &emplace_back(ARG&& arg);
# else
    value_type &emplace_back(VALUE_TYPE *ptr);
# endif

    void push_back(VALUE_TYPE *value);

    void pop_back();

    iterator emplace(const_iterator position);

# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class ARG>
    iterator emplace(const_iterator position, ARG&& arg);
# else
    iterator emplace(const_iterator position, VALUE_TYPE *ptr);
# endif

    iterator insert(const_iterator position, VALUE_TYPE *value);
    iterator insert(const_iterator  position,
                    size_type       numElements,
                    VALUE_TYPE     *value);

    template <class INPUT_ITER>
    iterator insert(const_iterator position,
                    INPUT_ITER     first,
                    INPUT_ITER     last)
    {
        // NOTE: This function has been implemented inline due to an issue with
        // the Sun compiler.

        typedef typename vector_ForwardIteratorForPtrs<VALUE_TYPE,
                                                       INPUT_ITER>::type Iter;

        return (iterator)d_impl.insert(
            (const UintPtr *)position, Iter(first), Iter(last));
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    iterator insert(const_iterator position,
                    std::initializer_list<VALUE_TYPE *> values);
#endif

    iterator erase(const_iterator position);
    iterator erase(const_iterator first, const_iterator last);

    void swap(vector<VALUE_TYPE *, ALLOCATOR>& other)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                                                   d_impl.swap(other.d_impl)));

    void clear() BSLS_KEYWORD_NOEXCEPT;

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;

    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;

                             // *** iterators ***

    const_iterator  begin() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator  end() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;

    const_reverse_iterator  rbegin() const BSLS_KEYWORD_NOEXCEPT;
    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;
    const_reverse_iterator  rend() const BSLS_KEYWORD_NOEXCEPT;
    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;

                             // *** capacity ***

    size_type size() const BSLS_KEYWORD_NOEXCEPT;
    size_type capacity() const BSLS_KEYWORD_NOEXCEPT;
    bool empty() const BSLS_KEYWORD_NOEXCEPT;

                          // *** element access ***

    const_reference operator[](size_type position) const;

    const_reference at(size_type position) const;

    const_reference front() const;
    const_reference back() const;

    VALUE_TYPE *const *data() const BSLS_KEYWORD_NOEXCEPT;

    // FRIENDS
    friend
    bool operator==(const vector& lhs, const vector& rhs)
    {
        return lhs.d_impl == rhs.d_impl;
    }

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

    friend BloombergLP::bslalg::SynthThreeWayUtil::Result<Impl>
    operator<=>(const vector& lhs, const vector& rhs)
    {
        return BloombergLP::bslalg::SynthThreeWayUtil::compare(lhs.d_impl,
                                                               rhs.d_impl);
    }

#else

    friend
    bool operator!=(const vector& lhs, const vector& rhs)
    {
        return lhs.d_impl != rhs.d_impl;
    }

    friend
    bool operator<(const vector& lhs, const vector& rhs)
    {
        return lhs.d_impl < rhs.d_impl;
    }

    friend
    bool operator>(const vector& lhs, const vector& rhs)
    {
        return lhs.d_impl > rhs.d_impl;
    }

    friend
    bool operator<=(const vector& lhs, const vector& rhs)
    {
        return lhs.d_impl <= rhs.d_impl;
    }

    friend
    bool operator>=(const vector& lhs, const vector& rhs)
    {
        return lhs.d_impl >= rhs.d_impl;
    }

#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

    friend
    void swap(vector& a, vector& b)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                                                      a.d_impl.swap(b.d_impl)))
    {
        a.d_impl.swap(b.d_impl);
    }
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template <
    class SIZE_TYPE,
    class VALUE,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<VALUE>,
    class = bsl::enable_if_t<
              bsl::is_convertible_v<
                SIZE_TYPE,
                typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
vector(SIZE_TYPE, VALUE, ALLOC *) -> vector<VALUE>;
    // Deduce the template parameter 'VALUE' from the corresponding parameter
    // supplied to the constructor of 'vector'.  This deduction guide does not
    // participate unless the supplied allocator is convertible to
    // 'bsl::allocator<VALUE>'.

template <
    class INPUT_ITERATOR,
    class VALUE =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>
    >
vector(INPUT_ITERATOR, INPUT_ITERATOR) -> vector<VALUE>;
    // Deduce the template parameter 'VALUE' from the 'value_type' of the
    // iterators supplied to the constructor of 'vector'.

template<
    class INPUT_ITERATOR,
    class ALLOCATOR,
    class VALUE =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
vector(INPUT_ITERATOR, INPUT_ITERATOR, ALLOCATOR) -> vector<VALUE, ALLOCATOR>;
    // Deduce the template parameter 'VALUE' from the 'value_type' of the
    // iterators supplied to the constructor of 'vector'.  This deduction
    // guide does not participate unless the supplied allocator meets the
    // requirements of a standard allocator.

template<
    class INPUT_ITERATOR,
    class ALLOC,
    class VALUE =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<VALUE>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
vector(INPUT_ITERATOR, INPUT_ITERATOR, ALLOC *)
-> vector<VALUE>;
    // Deduce the template parameter 'VALUE' from the 'value_type' of the
    // iterators supplied to the constructor of 'vector'.  This deduction
    // guide does not participate unless the supplied allocator is convertible
    // to 'bsl::allocator<VALUE>'.

template<
    class VALUE,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<VALUE>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
vector(std::initializer_list<VALUE>, ALLOC *)
-> vector<VALUE>;
    // Deduce the template parameter 'VALUE' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'vector'.  This
    // deduction guide does not participate unless the supplied allocator is
    // convertible to 'bsl::allocator<VALUE>'.
#endif


// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================
// See IMPLEMENTATION NOTES in the .cpp before modifying anything below.

                        // ======================================
                        // class vector_UintPtrConversionIterator
                        // ======================================

template <class VALUE_TYPE, class ITERATOR>
class vector_UintPtrConversionIterator {
    // This class provides a minimal proxy iterator adapter, transforming
    // pointers to 'uintptr_t' values on the fly, for only the operations
    // needed to implement the member functions and constructors of the
    // 'vector' partial template specialization that take iterator ranges as
    // arguments.  While it does not provide a standard conforming iterator
    // itself, if provides exactly sufficient behavior to implement all the
    // needed members.  'VALUE_TYPE' shall be a pointer type, and 'ITERATOR'
    // shall be a standard conforming iterator that dereferences to a type
    // implicitly convertible to 'VALUE_TYPE'

  private:
    // DATA
    ITERATOR d_iter;

  public:
    // PUBLIC TYPES
    typedef BloombergLP::bsls::Types::UintPtr UintPtr;

    typedef UintPtr  value_type;
    typedef UintPtr *pointer;
    typedef UintPtr  reference;
    typedef typename iterator_traits<ITERATOR>::difference_type
                                                               difference_type;
    typedef typename iterator_traits<ITERATOR>::iterator_category
                                                             iterator_category;

    // CREATORS
    vector_UintPtrConversionIterator(ITERATOR it);                  // IMPLICIT
        // Create a proxy iterator adapting the specified 'it'.

    // MANIPULATORS
    vector_UintPtrConversionIterator& operator++();
        // Increment this iterator to refer to the next element in the
        // underlying sequence, and return a reference to this object.

    // ACCESSORS
    UintPtr operator*() const;
        // Return the value of the pointer this iterator refers to, converted
        // to an unsigned integer.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

    auto operator<=>(const vector_UintPtrConversionIterator&) const = default;

#else

    // FRIENDS
    friend
    bool operator==(const vector_UintPtrConversionIterator& lhs,
                    const vector_UintPtrConversionIterator& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' iterators refer to
        // the same element in the same underlying sequence or both refer to
        // the past-the-end element of the same sequence, and 'false'
        // otherwise.  The behavior is undefined if 'lhs' and 'rhs' do not
        // iterate over the same sequence.
    {
        return lhs.d_iter == rhs.d_iter;
    }

    friend
    bool operator!=(const vector_UintPtrConversionIterator& lhs,
                    const vector_UintPtrConversionIterator& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' iterators do not
        // refer to the same element in the same underlying sequence and no
        // more than one refers to the past-the-end element of the sequence,
        // and 'false' otherwise.  The behavior is undefined if 'lhs' and 'rhs'
        // do not iterate over the same sequence.
    {
        return lhs.d_iter != rhs.d_iter;
    }

    friend
    bool operator<(const vector_UintPtrConversionIterator& lhs,
                   const vector_UintPtrConversionIterator& rhs)
        // Return 'true' if the specified 'lhs' iterator is earlier in the
        // underlying sequence than the specified 'rhs' iterator, and 'false'
        // otherwise.  The behavior is undefined if 'lhs' and 'rhs' do not
        // iterate over the same sequence, or if the (template parameter) type
        // 'ITERATOR' is not a random access iterator.
    {
        return lhs.d_iter < rhs.d_iter;
    }

#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

    friend
    difference_type operator-(const vector_UintPtrConversionIterator& lhs,
                              const vector_UintPtrConversionIterator& rhs)
        // Return the distance between the specified 'lhs' iterator and the
        // specified 'rhs' iterator.  The behavior is undefined if 'lhs' and
        // 'rhs' do not iterate over the same sequence, or if the (template
        // parameter) type 'ITERATOR' is not a random access iterator.
    {
        return lhs.d_iter - rhs.d_iter;
    }
};

                        // --------------------------------------
                        // class vector_UintPtrConversionIterator
                        // --------------------------------------

// CREATORS
template <class VALUE_TYPE, class ITERATOR>
inline
vector_UintPtrConversionIterator<VALUE_TYPE, ITERATOR>::
vector_UintPtrConversionIterator(ITERATOR it)
: d_iter(it)
{
}

// MANIPULATORS
template <class VALUE_TYPE, class ITERATOR>
inline
vector_UintPtrConversionIterator<VALUE_TYPE, ITERATOR>&
vector_UintPtrConversionIterator<VALUE_TYPE, ITERATOR>::operator++()
{
    ++d_iter;
    return *this;
}

// ACCESSORS
template <class VALUE_TYPE, class ITERATOR>
inline
BloombergLP::bsls::Types::UintPtr
vector_UintPtrConversionIterator<VALUE_TYPE, ITERATOR>::operator*() const
{
    VALUE_TYPE const ptr = *d_iter;
    return reinterpret_cast<UintPtr>(ptr);
}


                        // ========================
                        // class Vector_PushProctor
                        // ========================

template <class VALUE_TYPE, class ALLOCATOR>
class Vector_PushProctor {
    // This class template provides a proctor for a newly created object that
    // is managed by an allocator.  The object will be constructed through a
    // call to 'allocator_traits<ALLOCATOR>::construct', and it should be
    // destroyed by a call to 'allocator_traits<ALLOCATOR>::destroy'.  Note
    // that this proctor takes no responsibility for the allocated memory that
    // the supplied value is constructed in.

    // DATA
    VALUE_TYPE *d_target_p;   // managed object
    ALLOCATOR   d_allocator;  // allocator to be used to destroy managed object

  private:
    // NOT IMPLEMENTED
    Vector_PushProctor(const Vector_PushProctor&); // = delete;
    Vector_PushProctor& operator=(const Vector_PushProctor&); // = delete;

  public:
    // CREATORS
    Vector_PushProctor(VALUE_TYPE *target, const ALLOCATOR& allocator);
        // Create a proctor that conditionally manages the specified 'target'
        // object (if non-zero) by destroying the managed object with a call to
        // 'allocator_traits<ALLOCATOR>::destroy' using the specified
        // 'allocator' upon destruction of this proctor, unless the managed
        // objects has been released.

    ~Vector_PushProctor();
        // Destroy this proctor, and destroy the object it manages (if any) by
        // a call to 'allocator_traits<ALLOCATOR>::destroy' using the allocator
        // supplied at construction.  If no object is currently being managed,
        // this method has no effect.

    // MANIPULATORS
    void release();
        // Release from management the object currently managed by this
        // proctor.  If no object is currently being managed, this method has
        // no effect.
};

                        // ------------------------
                        // class Vector_PushProctor
                        // ------------------------

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
Vector_PushProctor<VALUE_TYPE,ALLOCATOR>::Vector_PushProctor(
                                                   VALUE_TYPE       *target,
                                                   const ALLOCATOR&  allocator)
: d_target_p(target)
, d_allocator(allocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
Vector_PushProctor<VALUE_TYPE,ALLOCATOR>::~Vector_PushProctor()
{
    if (d_target_p) {
        bsl::allocator_traits<ALLOCATOR>::destroy(d_allocator, d_target_p);
    }
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_PushProctor<VALUE_TYPE,ALLOCATOR>::release()
{
    d_target_p = 0;
}

#if defined(BSLS_ASSERT_SAFE_IS_USED)
                        // -----------------------
                        // class Vector_RangeCheck
                        // -----------------------

template <class BSLSTL_ITERATOR>
inline
typename enable_if<!Vector_IsRandomAccessIterator<BSLSTL_ITERATOR>::VALUE,
                   bool>::type
Vector_RangeCheck::isInvalidRange(BSLSTL_ITERATOR, BSLSTL_ITERATOR)
{
    return false;
}

template <class BSLSTL_ITERATOR>
inline
typename enable_if<Vector_IsRandomAccessIterator<BSLSTL_ITERATOR>::VALUE,
                   bool>::type
Vector_RangeCheck::isInvalidRange(BSLSTL_ITERATOR first, BSLSTL_ITERATOR last)
{
    return last < first;
}
#endif

                        // ----------------
                        // class vectorBase
                        // ----------------

// CREATORS
template <class VALUE_TYPE>
inline
vectorBase<VALUE_TYPE>::vectorBase()
: d_dataBegin_p(0)
, d_dataEnd_p(0)
, d_capacity(0)
{
}

// MANIPULATORS

template <class VALUE_TYPE>
inline
void
vectorBase<VALUE_TYPE>::adopt(BloombergLP::bslmf::MovableRef<vectorBase> base)
{
    BSLS_ASSERT_SAFE(0 == d_dataBegin_p);
    BSLS_ASSERT_SAFE(0 == d_dataEnd_p);
    BSLS_ASSERT_SAFE(0 == d_capacity);

    vectorBase& lvalue = base;
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
typename vectorBase<VALUE_TYPE>::iterator
vectorBase<VALUE_TYPE>::begin() BSLS_KEYWORD_NOEXCEPT
{
    return d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::iterator
vectorBase<VALUE_TYPE>::end() BSLS_KEYWORD_NOEXCEPT
{
    return d_dataEnd_p;
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::reverse_iterator
vectorBase<VALUE_TYPE>::rbegin() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::reverse_iterator
vectorBase<VALUE_TYPE>::rend() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(begin());
}

                          // *** element access ***

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::reference
vectorBase<VALUE_TYPE>::operator[](size_type position)
{
    BSLS_ASSERT_SAFE(size() > position);

    return d_dataBegin_p[position];
}

template <class VALUE_TYPE>
typename vectorBase<VALUE_TYPE>::reference
vectorBase<VALUE_TYPE>::at(size_type position)
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
typename vectorBase<VALUE_TYPE>::reference
vectorBase<VALUE_TYPE>::front()
{
    BSLS_ASSERT_SAFE(!empty());

    return *d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::reference
vectorBase<VALUE_TYPE>::back()
{
    BSLS_ASSERT_SAFE(!empty());

    return *(d_dataEnd_p - 1);
}

template <class VALUE_TYPE>
inline
VALUE_TYPE *
vectorBase<VALUE_TYPE>::data() BSLS_KEYWORD_NOEXCEPT
{
    return d_dataBegin_p;
}

// ACCESSORS

                             // *** iterators ***
template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_iterator
vectorBase<VALUE_TYPE>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_iterator
vectorBase<VALUE_TYPE>::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_iterator
vectorBase<VALUE_TYPE>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_dataEnd_p;
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_iterator
vectorBase<VALUE_TYPE>::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return d_dataEnd_p;
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_reverse_iterator
vectorBase<VALUE_TYPE>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_reverse_iterator
vectorBase<VALUE_TYPE>::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_reverse_iterator
vectorBase<VALUE_TYPE>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_reverse_iterator
vectorBase<VALUE_TYPE>::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

                             // *** capacity ***

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::size_type
vectorBase<VALUE_TYPE>::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_dataEnd_p - d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::size_type
vectorBase<VALUE_TYPE>::capacity() const BSLS_KEYWORD_NOEXCEPT
{
    return d_capacity;
}

template <class VALUE_TYPE>
inline
bool vectorBase<VALUE_TYPE>::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return d_dataEnd_p == d_dataBegin_p;
}

                          // *** element access ***
template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_reference
vectorBase<VALUE_TYPE>::operator[](size_type position) const
{
    BSLS_ASSERT_SAFE(size() > position);

    return d_dataBegin_p[position];
}

template <class VALUE_TYPE>
typename vectorBase<VALUE_TYPE>::const_reference
vectorBase<VALUE_TYPE>::at(size_type position) const
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
typename vectorBase<VALUE_TYPE>::const_reference
vectorBase<VALUE_TYPE>::front() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *d_dataBegin_p;
}

template <class VALUE_TYPE>
inline
typename vectorBase<VALUE_TYPE>::const_reference
vectorBase<VALUE_TYPE>::back() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *(d_dataEnd_p - 1);
}

template <class VALUE_TYPE>
inline
const VALUE_TYPE *
vectorBase<VALUE_TYPE>::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_dataBegin_p;
}

             // --------------------------------------------
             // class vector<VALUE_TYPE, ALLOCATOR>::Proctor
             // --------------------------------------------

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
vector<VALUE_TYPE, ALLOCATOR>::Proctor::Proctor(VALUE_TYPE    *data,
                                                std::size_t    capacity,
                                                ContainerBase *container)
: d_data_p(data)
, d_capacity(capacity)
, d_container_p(container)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
vector<VALUE_TYPE, ALLOCATOR>::Proctor::~Proctor()
{
    if (d_data_p) {
        d_container_p->deallocateN(d_data_p, d_capacity);
    }
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
void vector<VALUE_TYPE, ALLOCATOR>::Proctor::release()
{
    d_data_p = 0;
}

                            // ------------
                            // class vector
                            // ------------

// PRIVATE MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
template <class FWD_ITER>
void vector<VALUE_TYPE, ALLOCATOR>::constructFromRange(
                                               FWD_ITER                  first,
                                               FWD_ITER                  last,
                                               std::forward_iterator_tag)
{
    // Specialization for all iterators except input iterators: 'size' can be
    // computed in advance.
    BSLS_ASSERT_SAFE(!Vector_RangeCheck::isInvalidRange(first, last));

    const size_type maxSize = max_size();
    const size_type newSize = bsl::distance(first, last);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newSize > maxSize)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                           "vector<...>::(range-constructor): input too long");
    }

    size_type newCapacity = Vector_Util::computeNewCapacity(newSize,
                                                            0,
                                                            maxSize);
    this->privateReserveEmpty(newCapacity);
    Proctor proctor(this->d_dataBegin_p,
                    this->d_capacity,
                    static_cast<ContainerBase *>(this));

    ArrayPrimitives::copyConstruct(this->d_dataEnd_p,
                                   first,
                                   last,
                                   ContainerBase::allocator());
    proctor.release();
    this->d_dataEnd_p += newSize;
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
void vector<VALUE_TYPE, ALLOCATOR>::constructFromRange(
                                                 INPUT_ITER              first,
                                                 INPUT_ITER              last,
                                                 std::input_iterator_tag)
{
    // IMPLEMENTATION NOTES: construct this vector by iterated 'push_back',
    // which may reallocate memory multiple times, but unfortunately is
    // required because we can't compute the size in advance (as with
    // 'forward_iterator_tag') because input iterators can be traversed only
    // once.  A temporary vector is populated and then swapped to ensure that
    // all memory is reclaimed if 'emplace_back' throws, as the destructor will
    // not run when this method is called from a constructor.

    vector temp(this->get_allocator());
    while (first != last) {
        temp.emplace_back(*first);
        ++first;
    }
    Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INTEGRAL>
void vector<VALUE_TYPE, ALLOCATOR>::constructFromRange(
                                            INTEGRAL               initialSize,
                                            INTEGRAL               value,
                                            BloombergLP::bslmf::Nil)
{
    // IMPLEMENTATION NOTES: this constructor is trying to construct a range of
    // 'initialSize' elements having the specified integral 'value'.  Without
    // this extra overload, such calls would match an attempt to construct from
    // a range specified by two iterators.  Note that as 'VALUE_TYPE' must be
    // a (trivial) integral type, a proctor is almost certainly not needed.
    // The only risk of a throw is for user-defined allocators doing strange
    // extra (potentially throwing) work in their 'construct' call.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                           static_cast<size_type>(initialSize) > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                  "vector<...>::(repeated-value constructor): input too long");
    }

    if (initialSize > 0) {
        privateReserveEmpty(initialSize);
        Proctor proctor(this->d_dataBegin_p,
                        this->d_capacity,
                        static_cast<ContainerBase *>(this));

        ArrayPrimitives::uninitializedFillN(this->d_dataBegin_p,
                                            initialSize,
                                            static_cast<VALUE_TYPE>(value),
                                            ContainerBase::allocator());

        proctor.release();
        this->d_dataEnd_p += initialSize;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void vector<VALUE_TYPE, ALLOCATOR>::privateInsertDispatch(
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
void vector<VALUE_TYPE, ALLOCATOR>::privateInsertDispatch(
                                          const_iterator              position,
                                          INPUT_ITER                  first,
                                          INPUT_ITER                  last,
                                          BloombergLP::bslmf::MatchAnyType ,
                                          BloombergLP::bslmf::MatchAnyType )
{
    // Dispatch based on iterator category.
    BSLS_ASSERT_SAFE(!Vector_RangeCheck::isInvalidRange(first, last));

    typedef typename iterator_traits<INPUT_ITER>::iterator_category Tag;
    this->privateInsert(position, first, last, Tag());
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
void vector<VALUE_TYPE, ALLOCATOR>::privateInsert(
                                      const_iterator                  position,
                                      INPUT_ITER                      first,
                                      INPUT_ITER                      last,
                                      const std::input_iterator_tag&)
{
    // IMPLEMENTATION NOTES: We can't compute the size in advance.  Append onto
    // the back of the current vector while capacity remains.  This honors the
    // idea of not allocating unnecessarily for the temporary vector, and so
    // saves important cycles from a sequential allocator.  We then need to
    // shuffle the data back into the correct position.  If capacity must grow,
    // then create a new vector and move just the newly inserted elements into
    // place, moving the original vector elements only in the event that all
    // iterated elements are correctly inserted.

    // Short-circuit if there is nothing to do, do not allocate for an empty
    // 'vector' as that would invalidate 'begin'.

    if (first == last) {
        return;                                                       // RETURN
    }

    if (!this->capacity()) {
        privateReserveEmpty(size_type(1));
        position = this->d_dataBegin_p;       // 'position' must have been null
    }

    size_type insertOffset = position - this->d_dataBegin_p;
    size_type initialEnd   = this->size();
    size_type tailLength   = this->end() - position;

    VALUE_TYPE *emplaceBegin    = this->d_dataEnd_p;
    VALUE_TYPE *emplaceEnd      = this->d_dataBegin_p + this->d_capacity;
    VALUE_TYPE *emplacePosition = emplaceBegin;

    allocator_type alloc(this->get_allocator());  // need non-'const' lvalue

    vector resultState(alloc);  // vector that will build the final state
        // This vector is not used if sufficient capacity can be found in the
        // current vector for all the insertions.  However, it must have a
        // lifetime longer than the destructor guard below, in order to ensure
        // that the guarded elements are destroyed before the allocated storage
        // that holds them if an exception is thrown.

    // TBD: We really need an allocator-aware 'AutoDestructor' that will call
    // 'allocator_traits<ALLOC>::destroy(allocator, pointer)' rather than
    // invoke the destructor directly.  'bslalg::AutoArrayDestructor' is close,
    // but lacks 'reset'.
    BloombergLP::bslma::AutoDestructor<VALUE_TYPE> insertProctor(
                                                              emplacePosition);
    while (emplacePosition != emplaceEnd) {
        AllocatorTraits::construct(alloc, emplacePosition, *first);
        ++insertProctor;
        ++emplacePosition;
        if (++first == last) {
            this->d_dataEnd_p = emplacePosition;
            insertProctor.release();

            ArrayPrimitives::rotate(this->d_dataBegin_p + insertOffset,
                                    this->d_dataBegin_p + initialEnd,
                                    this->d_dataEnd_p);
            return;                                                   // RETURN
        }
    }

    // Now we need to grow a buffer and destructive-move only the new elements.
    // This needs to be handled in a loop that can allow for multiple growth
    // spurts.

    resultState.reserve(this->d_capacity*2);
    emplacePosition = resultState.d_dataBegin_p + insertOffset;
    ArrayPrimitives::destructiveMove(emplacePosition,
                                     emplaceBegin,
                                     emplaceEnd,
                                     alloc);

    size_type emplaceOffset = (emplaceEnd - emplaceBegin);
    insertProctor.reset(emplacePosition);
    emplaceBegin = emplacePosition;
    emplaceEnd   = resultState.d_dataBegin_p + resultState.d_capacity
                                             - tailLength;
    emplacePosition += emplaceOffset;

    while (first != last) {
        if (emplacePosition == emplaceEnd) {
            // need to grow again
            vector nextResult(alloc);
            nextResult.reserve(resultState.d_capacity*2);
            emplacePosition = nextResult.d_dataBegin_p + insertOffset;
            ArrayPrimitives::destructiveMove(emplacePosition,
                                             emplaceBegin,
                                             emplaceEnd,
                                             alloc);

            insertProctor.reset(emplacePosition);
            emplaceOffset = (emplaceEnd - emplaceBegin);
            emplaceBegin  = emplacePosition;
            emplaceEnd    = nextResult.d_dataBegin_p + nextResult.d_capacity
                                                     - tailLength;
            emplacePosition += emplaceOffset;

            Vector_Util::swap(&nextResult.d_dataBegin_p,
                              &resultState.d_dataBegin_p);
        }

        AllocatorTraits::construct(alloc, emplacePosition, *first);
        ++insertProctor;
        ++emplacePosition;
        ++first;
    }

    // move tail
    ArrayPrimitives::destructiveMove(emplacePosition,
                                     this->d_dataBegin_p + insertOffset,
                                     this->d_dataBegin_p + initialEnd,
                                     alloc);

    // reset 'end' in case a throw follows:
    this->d_dataEnd_p = this->d_dataBegin_p + insertOffset;
    emplacePosition += (initialEnd - insertOffset);
    insertProctor.setLength(
         insertProctor.length() + static_cast<int>(initialEnd - insertOffset));

    // move prefix
    ArrayPrimitives::destructiveMove(resultState.d_dataBegin_p,
                                     this->d_dataBegin_p,
                                     this->d_dataBegin_p + insertOffset,
                                     alloc);

    // Nothing after this point can throw.

    // 'resultState' adopts ownership of all elements
    resultState.d_dataEnd_p = emplacePosition;

    // We no longer own any data to protect
    insertProctor.release();
    this->d_dataEnd_p = this->d_dataBegin_p;

    // Finally, swap states
    Vector_Util::swap(&this->d_dataBegin_p, &resultState.d_dataBegin_p);
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class FWD_ITER>
void vector<VALUE_TYPE, ALLOCATOR>::privateInsert(
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

        vector temp(this->get_allocator());
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
void vector<VALUE_TYPE, ALLOCATOR>::privateMoveInsert(
                                                    vector         *fromVector,
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

        vector temp(this->get_allocator());
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
void vector<VALUE_TYPE, ALLOCATOR>::privateReserveEmpty(size_type numElements)
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
vector<VALUE_TYPE, ALLOCATOR>::vector() BSLS_KEYWORD_NOEXCEPT
: vectorBase<VALUE_TYPE>()
, ContainerBase(ALLOCATOR())
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(const ALLOCATOR& basicAllocator)
                                                          BSLS_KEYWORD_NOEXCEPT
: vectorBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
vector<VALUE_TYPE, ALLOCATOR>::vector(size_type        initialSize,
                                      const ALLOCATOR& basicAllocator)
: vectorBase<VALUE_TYPE>()
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
vector<VALUE_TYPE, ALLOCATOR>::vector(size_type         initialSize,
                                      const VALUE_TYPE& value,
                                      const ALLOCATOR&  basicAllocator)
: vectorBase<VALUE_TYPE>()
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
vector<VALUE_TYPE, ALLOCATOR>::vector(INPUT_ITER       first,
                                      INPUT_ITER       last,
                                      const ALLOCATOR& basicAllocator)
: vectorBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    BSLS_ASSERT_SAFE(!Vector_RangeCheck::isInvalidRange(first, last));

    typedef typename Vector_DeduceIteratorCategory<INPUT_ITER>::type Tag;

    if (is_same<Tag, BloombergLP::bslmf::Nil>::value || first != last) {
        // Range-check avoids allocating on an empty sequence.
        constructFromRange(first, last, Tag());
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
vector<VALUE_TYPE, ALLOCATOR>::vector(const vector& original)
: vectorBase<VALUE_TYPE>()
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
vector<VALUE_TYPE, ALLOCATOR>::
vector(const vector& original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: vectorBase<VALUE_TYPE>()
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
vector<VALUE_TYPE, ALLOCATOR>::vector(
                               BloombergLP::bslmf::MovableRef<vector> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: vectorBase<VALUE_TYPE>()
, ContainerBase(MoveUtil::access(original).get_allocator())
{
    vector& lvalue = original;
    ImpBase::adopt(MoveUtil::move(static_cast<ImpBase&>(lvalue)));
}

template <class VALUE_TYPE, class ALLOCATOR>
vector<VALUE_TYPE, ALLOCATOR>::vector(
                 BloombergLP::bslmf::MovableRef<vector>         original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: vectorBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    vector& lvalue = original;

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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(
                              std::initializer_list<VALUE_TYPE> values,
                              const ALLOCATOR&                  basicAllocator)
: vectorBase<VALUE_TYPE>()
, ContainerBase(basicAllocator)
{
    if (values.begin() != values.end()) {
        constructFromRange(values.begin(),
                           values.end(),
                           std::random_access_iterator_tag());
    }
}

#endif


template <class VALUE_TYPE, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
vector<VALUE_TYPE, ALLOCATOR>::~vector()
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
vector<VALUE_TYPE, ALLOCATOR>&
vector<VALUE_TYPE, ALLOCATOR>::operator=(const vector& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        if (AllocatorTraits::propagate_on_container_copy_assignment::value) {
            vector other(rhs, rhs.get_allocator());
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
vector<VALUE_TYPE, ALLOCATOR>& vector<VALUE_TYPE, ALLOCATOR>::operator=(
            BloombergLP::bslmf::MovableRef<vector<VALUE_TYPE, ALLOCATOR> > rhs)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
              AllocatorTraits::propagate_on_container_move_assignment::value ||
              AllocatorTraits::is_always_equal::value)
{
    vector& lvalue = rhs;
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &lvalue)) {
        if (get_allocator() == lvalue.get_allocator()) {
            vector other(MoveUtil::move(lvalue));
            Vector_Util::swap(&this->d_dataBegin_p, &other.d_dataBegin_p);
        }
        else if (AllocatorTraits::
                               propagate_on_container_move_assignment::value) {
            vector other(MoveUtil::move(lvalue));
            using std::swap;
            swap(ContainerBase::allocator(), other.ContainerBase::allocator());
            Vector_Util::swap(&this->d_dataBegin_p, &other.d_dataBegin_p);
        }
        else {
            vector other(MoveUtil::move(lvalue),
                             ContainerBase::allocator());
            Vector_Util::swap(&this->d_dataBegin_p, &other.d_dataBegin_p);
        }
    }
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>&
vector<VALUE_TYPE, ALLOCATOR>::operator=(
                                      std::initializer_list<VALUE_TYPE> values)
{
    this->assign(values.begin(), values.end());
    return *this;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE, ALLOCATOR>::assign(
                                      std::initializer_list<VALUE_TYPE> values)
{
    assign(values.begin(), values.end());
}
#endif

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void vector<VALUE_TYPE, ALLOCATOR>::assign(INPUT_ITER first, INPUT_ITER last)
{
    BSLS_ASSERT_SAFE(!Vector_RangeCheck::isInvalidRange(first, last));

    if (!this->empty()) {
        erase(this->begin(), this->end());
    }

    insert(this->begin(), first, last);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE, ALLOCATOR>::assign(size_type         numElements,
                                           const VALUE_TYPE& value)
{
    if (!this->empty()) {
        erase(this->begin(), this->end());
    }
    insert(this->begin(), numElements, value);
}


                             // *** capacity ***

template <class VALUE_TYPE, class ALLOCATOR>
void vector<VALUE_TYPE, ALLOCATOR>::resize(size_type newSize)
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
        vector temp(newSize, this->get_allocator());
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

        vector temp(this->get_allocator());
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
void vector<VALUE_TYPE, ALLOCATOR>::resize(size_type         newSize,
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
void vector<VALUE_TYPE, ALLOCATOR>::reserve(size_type newCapacity)
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
        vector temp(this->get_allocator());
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
void vector<VALUE_TYPE, ALLOCATOR>::shrink_to_fit()
{
    if (this->size() < this->d_capacity) {
        vector temp(this->get_allocator());
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
VALUE_TYPE &
vector<VALUE_TYPE, ALLOCATOR>::emplace_back(Args&&...arguments)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(
            ContainerBase::allocator(),
            this->d_dataEnd_p,
            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);
        ++this->d_dataEnd_p;
    }
    else {
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(max_size() == this->size())){
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                         "vector<...>:emplace_back(args...): vector too long");
        }

        size_type newCapacity = Vector_Util::computeNewCapacity(
                                                             this->size() + 1,
                                                             this->d_capacity,
                                                             this->max_size());
        vector temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        // Construct before we risk invalidating the reference
        VALUE_TYPE *pos = temp.d_dataBegin_p + this->size();
        AllocatorTraits::construct(
                            ContainerBase::allocator(),
                            pos,
                            BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...);

        // Nothing else should throw, but probably worth guarding the above
        // 'construct' call for types with potentially-throwing destructive
        // moves.
        Vector_PushProctor<VALUE_TYPE, ALLOCATOR> guard(
                                                   pos,
                                                   ContainerBase::allocator());
        ArrayPrimitives::destructiveMove(temp.d_dataBegin_p,
                                         this->d_dataBegin_p,
                                         this->d_dataEnd_p,
                                         ContainerBase::allocator());
        guard.release();  // Nothing after this can throw

        this->d_dataEnd_p = this->d_dataBegin_p;
        temp.d_dataEnd_p = ++pos;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
    return *(this->d_dataEnd_p - 1);
}
#endif

template <class VALUE_TYPE, class ALLOCATOR>
void vector<VALUE_TYPE, ALLOCATOR>::push_back(const VALUE_TYPE& value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        AllocatorTraits::construct(ContainerBase::allocator(),
                                   this->d_dataEnd_p,
                                   value);
        ++this->d_dataEnd_p;
    }
    else {
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(max_size() == this->size())){
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                             "vector<...>:push_back(lvalue): vector too long");
        }

        size_type newCapacity = Vector_Util::computeNewCapacity(
                                                             this->size() + 1,
                                                             this->d_capacity,
                                                             this->max_size());

        vector temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        // Construct before we risk invalidating the reference
        VALUE_TYPE *pos = temp.d_dataBegin_p + this->size();
        AllocatorTraits::construct(ContainerBase::allocator(),
                                   pos,
                                   value);

        // Nothing else should throw, but probably worth guarding the above
        // 'construct' call for types with potentially-throwing destructive
        // moves.
        Vector_PushProctor<VALUE_TYPE, ALLOCATOR> guard(
                                                   pos,
                                                   ContainerBase::allocator());
        ArrayPrimitives::destructiveMove(temp.d_dataBegin_p,
                                         this->d_dataBegin_p,
                                         this->d_dataEnd_p,
                                         ContainerBase::allocator());
        guard.release();  // Nothing after this can throw

        this->d_dataEnd_p = this->d_dataBegin_p;
        temp.d_dataEnd_p = ++pos;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void vector<VALUE_TYPE, ALLOCATOR>::push_back(
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
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(this->size() == max_size())){
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                             "vector<...>:push_back(rvalue): vector too long");
        }

        size_type newCapacity = Vector_Util::computeNewCapacity(
                                                             this->size() + 1,
                                                             this->d_capacity,
                                                             this->max_size());

        vector temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        // Construct before we risk invalidating the reference
        VALUE_TYPE *pos = temp.d_dataBegin_p + this->size();
        AllocatorTraits::construct(ContainerBase::allocator(),
                                   pos,
                                   MoveUtil::move(lvalue));

        // Nothing else should throw, but probably worth guarding the above
        // 'construct' call for types with potentially-throwing destructive
        // moves.
        Vector_PushProctor<VALUE_TYPE, ALLOCATOR> guard(
                                                   pos,
                                                   ContainerBase::allocator());
        ArrayPrimitives::destructiveMove(temp.d_dataBegin_p,
                                         this->d_dataBegin_p,
                                         this->d_dataEnd_p,
                                         ContainerBase::allocator());
        this->d_dataEnd_p = this->d_dataBegin_p;
        guard.release();  // Nothing after this can throw
        temp.d_dataEnd_p = ++pos;
        Vector_Util::swap(&this->d_dataBegin_p, &temp.d_dataBegin_p);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE, ALLOCATOR>::pop_back()
{
    BSLS_ASSERT_SAFE(!this->empty());

    AllocatorTraits::destroy(ContainerBase::allocator(),
                             --this->d_dataEnd_p);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE, ALLOCATOR>::iterator
vector<VALUE_TYPE, ALLOCATOR>::insert(const_iterator    position,
                                      const VALUE_TYPE& value)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <= this->end());

    return insert(position, size_type(1), value);
}

template <class VALUE_TYPE, class ALLOCATOR>
typename vector<VALUE_TYPE, ALLOCATOR>::iterator
vector<VALUE_TYPE, ALLOCATOR>::insert(
                           const_iterator                             position,
                           BloombergLP::bslmf::MovableRef<VALUE_TYPE> value)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <= this->end());

    const size_type maxSize = max_size();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(1 > maxSize - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                               "vector<...>::insert(pos,rv): vector too long");
    }

    VALUE_TYPE& lvalue = value;

    const size_type index   = position - this->begin();
    const iterator& pos     = const_cast<const iterator&>(position);
    const size_type newSize = this->size() + 1;

    if (newSize > this->d_capacity) {
        size_type newCapacity = Vector_Util::computeNewCapacity(
                                                              newSize,
                                                              this->d_capacity,
                                                              maxSize);

        vector temp(this->get_allocator());
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
typename vector<VALUE_TYPE, ALLOCATOR>::iterator
vector<VALUE_TYPE, ALLOCATOR>::insert(const_iterator    position,
                                      size_type         numElements,
                                      const VALUE_TYPE& value)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <= this->end());

    const size_type maxSize = max_size();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                       numElements > maxSize - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl::StdExceptUtil::throwLengthError(
                              "vector<...>::insert(pos,n,v): vector too long");
    }

    const size_type index   = position - this->begin();
    const iterator& pos     = const_cast<const iterator&>(position);
    const size_type newSize = this->size() + numElements;

    if (newSize > this->d_capacity) {
        size_type newCapacity = Vector_Util::computeNewCapacity(
                                                              newSize,
                                                              this->d_capacity,
                                                              maxSize);

        vector temp(this->get_allocator());
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
typename vector<VALUE_TYPE, ALLOCATOR>::iterator
vector<VALUE_TYPE, ALLOCATOR>::insert(
                                    const_iterator                    position,
                                    std::initializer_list<VALUE_TYPE> values)
{
    return insert(position, values.begin(), values.end());
}
#endif

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE, ALLOCATOR>::iterator
vector<VALUE_TYPE, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <  this->end());

    return erase(position, position + 1);
}

// This should not be inlined by default due to an XLC 16 compiler bug whereby
// optimized code can spuriously core dump.  This has been reported to IBM, see
// DRQS 169655225 for details.
template <class VALUE_TYPE, class ALLOCATOR>
BSLS_PLATFORM_AGGRESSIVE_INLINE
typename vector<VALUE_TYPE, ALLOCATOR>::iterator
vector<VALUE_TYPE, ALLOCATOR>::erase(const_iterator first, const_iterator last)
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
void vector<VALUE_TYPE, ALLOCATOR>::swap(vector<VALUE_TYPE, ALLOCATOR>& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                         AllocatorTraits::propagate_on_container_swap::value ||
                         AllocatorTraits::is_always_equal::value)
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

            vector toOtherCopy(MoveUtil::move(*this),
                                   other.get_allocator());
            vector toThisCopy( MoveUtil::move(other),
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
void vector<VALUE_TYPE, ALLOCATOR>::clear() BSLS_KEYWORD_NOEXCEPT
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(!this->empty())) {
        BloombergLP::bslalg::ArrayDestructionPrimitives::destroy(
                                                   this->d_dataBegin_p,
                                                   this->d_dataEnd_p,
                                                   ContainerBase::allocator());
        this->d_dataEnd_p = this->d_dataBegin_p;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    }
}

// ACCESSORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE, ALLOCATOR>::allocator_type
vector<VALUE_TYPE, ALLOCATOR>::get_allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return ContainerBase::allocator();
}

                         // *** capacity ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE, ALLOCATOR>::size_type
vector<VALUE_TYPE, ALLOCATOR>::max_size() const BSLS_KEYWORD_NOEXCEPT
{
    return AllocatorTraits::max_size(ContainerBase::allocator());
}

// FREE OPERATORS

                       // *** relational operators ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator==(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator!=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return ! (lhs == rhs);
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class VALUE_TYPE, class ALLOCATOR>
inline
BloombergLP::bslalg::SynthThreeWayUtil::Result<VALUE_TYPE> operator<=>(
                                      const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                                      const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return lexicographical_compare_three_way(
                              lhs.begin(),
                              lhs.end(),
                              rhs.begin(),
                              rhs.end(),
                              BloombergLP::bslalg::SynthThreeWayUtil::compare);
}

#else

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator< (const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
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
bool operator> (const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return rhs < lhs;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return !(rhs < lhs);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
                const vector<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return !(lhs < rhs);
}

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS

                       // *** specialized algorithms ***

template <class VALUE_TYPE, class ALLOCATOR, class BDE_OTHER_TYPE>
inline typename vector<VALUE_TYPE, ALLOCATOR>::size_type
erase(vector<VALUE_TYPE, ALLOCATOR>& vec, const BDE_OTHER_TYPE& value)
{
    typename vector<VALUE_TYPE, ALLOCATOR>::size_type oldSize = vec.size();
    vec.erase(bsl::remove(vec.begin(), vec.end(), value), vec.end());
    return oldSize - vec.size();
}

template <class VALUE_TYPE, class ALLOCATOR, class PREDICATE>
inline typename vector<VALUE_TYPE, ALLOCATOR>::size_type
erase_if(vector<VALUE_TYPE, ALLOCATOR>& vec, PREDICATE predicate)
{
    typename vector<VALUE_TYPE, ALLOCATOR>::size_type oldSize = vec.size();
    vec.erase(bsl::remove_if(vec.begin(), vec.end(), predicate), vec.end());
    return oldSize - vec.size();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void swap(vector<VALUE_TYPE, ALLOCATOR>& a,
          vector<VALUE_TYPE, ALLOCATOR>& b)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                                                                    a.swap(b)))
{
    a.swap(b);
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

                      // *** construct/copy/destroy ***

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector() BSLS_KEYWORD_NOEXCEPT
: d_impl()
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector(const ALLOCATOR& basicAllocator)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_impl(ImplAlloc(basicAllocator))
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector(size_type        initialSize,
                                        const ALLOCATOR& basicAllocator)
: d_impl(initialSize, ImplAlloc(basicAllocator))
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector(size_type         initialSize,
                                        VALUE_TYPE       *value,
                                        const ALLOCATOR&  basicAllocator)
: d_impl(initialSize, (UintPtr) value, ImplAlloc(basicAllocator))
{
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector(INPUT_ITER       first,
                                        INPUT_ITER       last,
                                        const ALLOCATOR& basicAllocator)
: d_impl(typename vector_ForwardIteratorForPtrs<VALUE_TYPE, INPUT_ITER>::type(
             first),
         typename vector_ForwardIteratorForPtrs<VALUE_TYPE, INPUT_ITER>::type(
             last),
         basicAllocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector(const vector& original)
: d_impl(original.d_impl)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector(
                               BloombergLP::bslmf::MovableRef<vector> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_impl(MoveUtil::move(MoveUtil::access(original).d_impl))
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector(const vector&    original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_impl(original.d_impl, ImplAlloc(basicAllocator))
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector(
    BloombergLP::bslmf::MovableRef<vector>         original,
    const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_impl(MoveUtil::move(MoveUtil::access(original).d_impl),
         ImplAlloc(basicAllocator))
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::vector(
    std::initializer_list<VALUE_TYPE *> values,
    const ALLOCATOR&                    basicAllocator)
: d_impl(typename vector_ForwardIteratorForPtrs<
             VALUE_TYPE,
             typename std::initializer_list<VALUE_TYPE *>::const_iterator>::
             type(values.begin()),
         typename vector_ForwardIteratorForPtrs<
             VALUE_TYPE,
             typename std::initializer_list<VALUE_TYPE *>::const_iterator>::
             type(values.end()),
         basicAllocator)
{
}
#endif

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>::~vector()
{
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>& vector<VALUE_TYPE *, ALLOCATOR>::operator=(
                                                             const vector& rhs)
{
    d_impl = rhs.d_impl;
    return *this;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>&
vector<VALUE_TYPE *, ALLOCATOR>::operator=(
          BloombergLP::bslmf::MovableRef<vector<VALUE_TYPE *, ALLOCATOR> > rhs)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                        d_impl = MoveUtil::move(MoveUtil::access(rhs).d_impl)))
{
    d_impl = MoveUtil::move(MoveUtil::access(rhs).d_impl);
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE *, ALLOCATOR>& vector<VALUE_TYPE *, ALLOCATOR>::operator=(
                                    std::initializer_list<VALUE_TYPE *> values)
{
    assign(values);
    return *this;
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::assign(
                                    std::initializer_list<VALUE_TYPE *> values)
{
    typedef typename std::initializer_list<VALUE_TYPE *>::const_iterator
                                                                      InitIter;

    typedef typename vector_ForwardIteratorForPtrs<VALUE_TYPE, InitIter>::type
                                                                          Iter;

    d_impl.assign(Iter(values.begin()), Iter(values.end()));
}
#endif

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::assign(INPUT_ITER first, INPUT_ITER last)
{
    typedef typename vector_ForwardIteratorForPtrs<VALUE_TYPE,
                                                   INPUT_ITER>::type Iter;

    d_impl.assign(Iter(first), Iter(last));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::assign(size_type   numElements,
                                             VALUE_TYPE *value)
{
    d_impl.assign(numElements, (UintPtr) value);
}

                             // *** iterators ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::begin() BSLS_KEYWORD_NOEXCEPT
{
    return (iterator) d_impl.begin();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::end() BSLS_KEYWORD_NOEXCEPT
{
    return (iterator) d_impl.end();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::reverse_iterator
vector<VALUE_TYPE *, ALLOCATOR>::rbegin() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator((iterator) d_impl.rbegin().base());
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::reverse_iterator
vector<VALUE_TYPE *, ALLOCATOR>::rend() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator((iterator) d_impl.rend().base());
}

                            // *** capacity ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::size_type
vector<VALUE_TYPE *, ALLOCATOR>::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.size();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::size_type
vector<VALUE_TYPE *, ALLOCATOR>::capacity() const BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.capacity();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool
vector<VALUE_TYPE *, ALLOCATOR>::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.empty();
}

                          // *** element access ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::reference
vector<VALUE_TYPE *, ALLOCATOR>::operator[](size_type position)
{
    return (reference) d_impl.operator[](position);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::reference
vector<VALUE_TYPE *, ALLOCATOR>::at(size_type position)
{
    return (reference) d_impl.at(position);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::reference
vector<VALUE_TYPE *, ALLOCATOR>::front()
{
    return (reference) d_impl.front();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::reference
vector<VALUE_TYPE *, ALLOCATOR>::back()
{
    return (reference) d_impl.back();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
VALUE_TYPE **vector<VALUE_TYPE *, ALLOCATOR>::data() BSLS_KEYWORD_NOEXCEPT
{
    return (VALUE_TYPE **) d_impl.data();
}

                             // *** capacity ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::resize(size_type newLength)
{
    d_impl.resize(newLength);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::resize(size_type   newLength,
                                             VALUE_TYPE *value)
{
    d_impl.resize(newLength, (UintPtr) value);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::reserve(size_type newCapacity)
{
    d_impl.reserve(newCapacity);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::shrink_to_fit()
{
    d_impl.shrink_to_fit();
}


                            // *** modifiers ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::reference
vector<VALUE_TYPE *, ALLOCATOR>::emplace_back()
{
    d_impl.emplace_back();
    return back();
}

# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class VALUE_TYPE, class ALLOCATOR>
template <class ARG>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::reference
vector<VALUE_TYPE *, ALLOCATOR>::emplace_back(ARG&& arg)
{
    VALUE_TYPE *ptr(arg);  // Support explicit conversion operators
    d_impl.emplace_back(reinterpret_cast<UintPtr>(ptr));
    return back();
}
# else
template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::reference
vector<VALUE_TYPE *, ALLOCATOR>::emplace_back(VALUE_TYPE *ptr)
{
    d_impl.emplace_back(reinterpret_cast<UintPtr>(ptr));
    return back();
}
# endif

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::push_back(VALUE_TYPE *value)
{
    d_impl.emplace_back(reinterpret_cast<UintPtr>(value));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::pop_back()
{
    d_impl.pop_back();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::emplace(const_iterator position)
{
    return (iterator) d_impl.emplace((const UintPtr*) position);
}

# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class VALUE_TYPE, class ALLOCATOR>
template <class ARG>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::emplace(const_iterator position, ARG&& arg)
{
    VALUE_TYPE *ptr(arg);  // Support explicit conversion operators
    return (iterator) d_impl.emplace((const UintPtr *)position,
                                     reinterpret_cast<UintPtr>(ptr));
}
# else
template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::emplace(const_iterator  position,
                                         VALUE_TYPE     *ptr)
{
    return (iterator) d_impl.emplace((const UintPtr*) position,
                                     reinterpret_cast<UintPtr>(ptr));
}
# endif

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::insert(const_iterator  position,
                                        VALUE_TYPE     *value)
{
    return (iterator) d_impl.emplace((const UintPtr*) position,
                                     reinterpret_cast<UintPtr>(value));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::insert(const_iterator  position,
                                        size_type       numElements,
                                        VALUE_TYPE     *value)
{
    return (iterator) d_impl.insert(
        (const UintPtr *)position, numElements, (UintPtr)value);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::insert(
                                  const_iterator                      position,
                                  std::initializer_list<VALUE_TYPE *> values)
{
    typedef typename std::initializer_list<VALUE_TYPE *>::const_iterator
                                                                      InitIter;

    typedef typename vector_ForwardIteratorForPtrs<VALUE_TYPE, InitIter>::type
                                                                          Iter;

    return (iterator) d_impl.insert(
        (const UintPtr *)position, Iter(values.begin()), Iter(values.end()));
}
#endif

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::erase(const_iterator position)
{
    return (iterator) d_impl.erase((const UintPtr*) position);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::iterator
vector<VALUE_TYPE *, ALLOCATOR>::erase(const_iterator first,
                                       const_iterator last)
{
    return (iterator) d_impl.erase((const UintPtr*) first,
                                   (const UintPtr*) last);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::swap(
                                        vector<VALUE_TYPE *, ALLOCATOR>& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(BSLS_KEYWORD_NOEXCEPT_OPERATOR(
                                                    d_impl.swap(other.d_impl)))
{
    d_impl.swap(other.d_impl);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void vector<VALUE_TYPE *, ALLOCATOR>::clear() BSLS_KEYWORD_NOEXCEPT
{
    d_impl.clear();
}

// ACCESSORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::allocator_type
vector<VALUE_TYPE *, ALLOCATOR>::get_allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return ALLOCATOR(d_impl.get_allocator());
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::size_type
vector<VALUE_TYPE *, ALLOCATOR>::max_size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.max_size();
}


                             // *** iterators ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_iterator
vector<VALUE_TYPE *, ALLOCATOR>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return (const_iterator) d_impl.begin();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_iterator
vector<VALUE_TYPE *, ALLOCATOR>::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return (const_iterator) d_impl.cbegin();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_iterator
vector<VALUE_TYPE *, ALLOCATOR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return (const_iterator) d_impl.end();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_iterator
vector<VALUE_TYPE *, ALLOCATOR>::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return (const_iterator) d_impl.cend();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_reverse_iterator
vector<VALUE_TYPE *, ALLOCATOR>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator((const_iterator) d_impl.rbegin().base());
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_reverse_iterator
vector<VALUE_TYPE *, ALLOCATOR>::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator((const_iterator) d_impl.crbegin().base());
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_reverse_iterator
vector<VALUE_TYPE *, ALLOCATOR>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator((const_iterator) d_impl.rend().base());
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_reverse_iterator
vector<VALUE_TYPE *, ALLOCATOR>::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator((const_iterator) d_impl.crend().base());
}


                          // *** element access ***

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_reference
vector<VALUE_TYPE *, ALLOCATOR>::operator[](size_type position) const
{
    return (const_reference) d_impl.operator[](position);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_reference
vector<VALUE_TYPE *, ALLOCATOR>::at(size_type position) const
{
    return (const_reference) d_impl.at(position);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_reference
vector<VALUE_TYPE *, ALLOCATOR>::front() const
{
    return (const_reference) d_impl.front();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
typename vector<VALUE_TYPE *, ALLOCATOR>::const_reference
vector<VALUE_TYPE *, ALLOCATOR>::back() const
{
    return (const_reference) d_impl.back();
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
VALUE_TYPE *const *vector<VALUE_TYPE *, ALLOCATOR>::data() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return (VALUE_TYPE *const *) d_impl.data();
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

namespace bslma {

template <class VALUE_TYPE, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::vector<VALUE_TYPE, ALLOCATOR> >
    : bsl::is_convertible<Allocator *, ALLOCATOR>::type
{};

}  // close namespace bslma

namespace bslmf {

template <class VALUE_TYPE, class ALLOCATOR>
struct IsBitwiseMoveable<bsl::vector<VALUE_TYPE, ALLOCATOR> >
    : IsBitwiseMoveable<ALLOCATOR>
{};

}  // close namespace bslmf

}  // close enterprise namespace

#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
extern template class bsl::vectorBase<bool>;
extern template class bsl::vectorBase<char>;
extern template class bsl::vectorBase<signed char>;
extern template class bsl::vectorBase<unsigned char>;
extern template class bsl::vectorBase<short>;
extern template class bsl::vectorBase<unsigned short>;
extern template class bsl::vectorBase<int>;
extern template class bsl::vectorBase<unsigned int>;
extern template class bsl::vectorBase<long>;
extern template class bsl::vectorBase<unsigned long>;
extern template class bsl::vectorBase<long long>;
extern template class bsl::vectorBase<unsigned long long>;
extern template class bsl::vectorBase<float>;
extern template class bsl::vectorBase<double>;
extern template class bsl::vectorBase<long double>;
extern template class bsl::vectorBase<void *>;
extern template class bsl::vectorBase<const char *>;

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
extern template class bsl::vector<const char *>;
#endif

#endif // End C++11 code

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
