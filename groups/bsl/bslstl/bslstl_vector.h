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
//   bsl::vector: STL-compatible vector template
//
//@SEE_ALSO: bslstl_deque
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<vector>' instead and use 'bsl::vector' directly.  This component
// implements a dynamic array class that supports the 'bslma_Allocator' model
// and is suitable for use as an implementation of the 'std::vector' class
// template.
//
///Usage
///-----
// This component is for use by the 'bsl+stdhdrs' package.  Use 'std::vector'
// directly.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_CONTAINERBASE
#include <bslstl_containerbase.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLSTL_UTIL
#include <bslstl_util.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#include <bslalg_arraydestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYPRIMITIVES
#include <bslalg_arrayprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_RANGECOMPARE
#include <bslalg_rangecompare.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLSEQUENCE
#include <bslalg_typetraitsgroupstlsequence.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ANYTYPE
#include <bslmf_anytype.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
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

namespace bsl {

                          // ==================
                          // struct Vector_Util
                          // ==================

struct Vector_Util {
    // This 'struct' provides a namespace to implement the 'swap' member
    // function of 'Vector_Imp<VALUE_TYPE, ALLOCATOR>'.  This function can
    // be implemented irrespective of the 'VALUE_TYPE' or 'ALLOCATOR' template
    // parameters which is why we implement it in this non-templated,
    // non-inlined utility.

    // CLASS METHODS
    static std::size_t computeNewCapacity(std::size_t newLength,
                                          std::size_t capacity,
                                          std::size_t maxSize);
        // Return a capacity at least the specified 'newLength' and at least
        // the minimum of twice the specified 'capacity' and the specified
        // 'maxSize'.  The behavior is undefined unless 'capacity < newLength'
        // and 'newLength <= maxSize'.  Note that the returned value is always
        // at most 'maxSize'.

    static void swap(void *a, void *b);
        // Exchange the value of the specified 'a' vector with that of the
        // specified 'b' vector.
};

                          // ====================
                          // class Vector_ImpBase
                          // ====================

template <class VALUE_TYPE>
class Vector_ImpBase {
    // This class describes the basic layout for a vector class, to be included
    // into the 'Vector_Imp' layout *before* the allocator (provided by
    // 'bslstl_ContainerBase') to take better advantage of cache prefetching.
    // It is parameterized by 'VALUE_TYPE' only, and implements the portion of
    // 'Vector_Imp' that does not need to know about its parameterized
    // 'ALLOCATOR' (in order to generate shorter debug strings).  This class
    // intentionally has *no* creators (other than the compiler-generated
    // ones).

  protected:
    // DATA
    VALUE_TYPE  *d_dataBegin;  // beginning of data storage (owned)
    VALUE_TYPE  *d_dataEnd;    // end of data storage (owned)
    std::size_t  d_capacity;   // length of storage

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
        // Initialize this object to empty with 0 capacity.

    // MANIPULATORS

                             // *** iterators: ***

    iterator begin();
        // Return an iterator pointing the first element in this modifiable
        // vector (or the past-the-end iterator if this vector is empty).

    iterator end();
        // Return the past-the-end iterator for this modifiable vector.

    reverse_iterator rbegin();
        // Return a reverse iterator pointing the last element in this
        // modifiable vector (or the past-the-end reverse iterator if this
        // vector is empty).

    reverse_iterator rend();
        // Return the past-the-end reverse iterator for this modifiable vector.

                          // *** element access: ***

    reference operator[](size_type position);
        // Return a reference to the modifiable element at the specified
        // 'position' in this vector.  The behavior is undefined unless
        // '0 <= position < size()'.

    reference at(size_type position);
        // Return a reference to the modifiable element at the specified
        // 'position' in this vector.  Throws 'std::out_of_range' if
        // 'position >= size()'.

    reference front();
        // Return a reference to the modifiable element at the first position
        // in this vector.  The behavior is undefined if this vector is empty.

    reference back();
        // Return a reference to the modifiable element at the last position
        // in this vector.  The behavior is undefined if this vector is empty.
        // Note that the last position is 'size() - 1'.

    //ACCESSORS

                             // *** iterators: ***

    const_iterator begin() const;
        // Return an iterator pointing the first element in this non-modifiable
        // vector (or the past-the-end iterator if this vector is empty).

    const_iterator cbegin() const;
        // Return an iterator pointing the first element in this non-modifiable
        // vector (or the past-the-end iterator if this vector is empty).

    const_iterator end() const;
        // Return the past-the-end iterator for this non-modifiable vector.

    const_iterator cend() const;
        // Return the past-the-end iterator for this non-modifiable vector.

    const_reverse_iterator rbegin() const;
        // Return a reverse iterator pointing the last element in this
        // non-modifiable vector (or the past-the-end reverse iterator if this
        // vector is empty).

    const_reverse_iterator crbegin() const;
        // Return a reverse iterator pointing the last element in this
        // non-modifiable vector (or the past-the-end reverse iterator if this
        // vector is empty).

    const_reverse_iterator rend() const;
        // Return the past-the-end reverse iterator for this non-modifiable
        // vector.

    const_reverse_iterator crend() const;
        // Return the past-the-end reverse iterator for this non-modifiable
        // vector.

                         // *** 23.2.4.2 capacity: ***

    size_type size() const;
        // Return the number of elements in this vector.

    size_type capacity() const;
        // Return the capacity of this vector, i.e., the maximum number of
        // elements for which resizing is guaranteed not to trigger a
        // reallocation.

    bool empty() const;
        // Return 'true' if this vector has size 0, and 'false' otherwise.

                          // *** element access: ***

    const_reference operator[](size_type position) const;
        // Return a reference to the non-modifiable element at the specified
        // 'position' in this vector.  The behavior is undefined unless
        // '0 <= position < size()'.

    const_reference at(size_type position) const;
        // Return a reference to the non-modifiable element at the specified
        // 'position'.  Throws 'std::out_of_range' if 'position >= size()'.

    const_reference front() const;
        // Return a reference to the non-modifiable element at the first
        // position in this vector.  The behavior is undefined if this vector
        // is empty.

    const_reference back() const;
        // Return a reference to the non-modifiable element at the last
        // position in this vector.  The behavior is undefined if this vector
        // is empty.  Note that the last position is 'size() - 1'.
};

                        // ================
                        // class Vector_Imp
                        // ================

template <class VALUE_TYPE, class ALLOCATOR = bsl::allocator<VALUE_TYPE> >
class Vector_Imp : public Vector_ImpBase<VALUE_TYPE>
                 , private BloombergLP::bslstl_ContainerBase<ALLOCATOR> {
    // This class template provides an STL-compliant 'vector' that conforms to
    // the 'bslma_Allocator' model.  For the requirements of a vector class,
    // consult the second revision of the "ISO/IEC 14882 Programming Language
    // C++ (Working Paper, 2009)".  In particular, this implementation offers
    // the general rules that:
    //..
    //   (1) a call to any methods that would result in a vector of size larger
    //       than 'max_size()' triggers a 'std::throwLengthError' exception.
    //   (2) a call to the 'at' method that attempts to access a position
    //       outside the valid range of a vector triggers a 'std::out_of_range'
    //       exception.
    //..
    // More generally, this class supports an almost complete set of *in-core*
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison (but excluding 'ostream' printing since this is
    // component is below STL).  A precise operational definition of when two
    // objects have the same value can be found in the description of
    // 'operator==' for the class.  This class is *exception* *neutral* with no
    // guarantee of rollback: if an exception is thrown during the invocation
    // of a method on a pre-existing object, the object is left in a valid
    // state, but its value is undefined.  In addition, the following members
    // offer a full guarantee of rollback: if an exception is thrown during the
    // invocation of 'insert' or 'push_back' on a pre-existing object, by
    // other than the 'VALUE_TYPE' constructors or assignment operator, the
    // object is left in a valid state and its value is unchanged.  In no event
    // is memory leaked.  Finally, *aliasing* (e.g., using all or part of an
    // object as both source and destination) is *not* supported.

  public:
    // PUBLIC TYPES:
    typedef typename ALLOCATOR::reference          reference;
    typedef typename ALLOCATOR::const_reference    const_reference;
    typedef VALUE_TYPE                            *iterator;
    typedef VALUE_TYPE const                      *const_iterator;
    typedef std::size_t                            size_type;
    typedef std::ptrdiff_t                         difference_type;
    typedef VALUE_TYPE                             value_type;
    typedef ALLOCATOR                              allocator_type;
    typedef typename ALLOCATOR::pointer            pointer;
    typedef typename ALLOCATOR::const_pointer      const_pointer;
    typedef bsl::reverse_iterator<iterator>        reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>  const_reverse_iterator;

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslstl_ContainerBase<ALLOCATOR> VectorContainerBase;
        // Container base type, containing the allocator and applying
        // empty base class optimization (EBO) whenever appropriate.

    class Guard {
        // This class provides a proctor for deallocating an array of
        // 'VALUE_TYPE' objects, to be used in the 'Vector_Imp'
        // constructors.

        // DATA
        VALUE_TYPE          *d_data_p;
        std::size_t          d_capacity;
        VectorContainerBase *d_container_p;

      public:
        // CREATORS
        Guard(VALUE_TYPE          *data,
              std::size_t          capacity,
              VectorContainerBase *container);
            // Create a proctor for the specified 'data' array of the specified
            // 'capacity', using the 'deallocateN' method of the specified
            // 'container' to return 'data' to its allocator upon destruction,
            // unless this proctor's 'release' is called prior.

        ~Guard();
            // Destroy this proctor, deallocating any data under management.

        // MANIPULATORS
        void release();
            // Release the data from management by this proctor.
    };

    // PRIVATE MANIPULATORS
    template <class INPUT_ITER>
    void privateInsertDispatch(const_iterator                   position,
                               INPUT_ITER                       count,
                               INPUT_ITER                       value,
                               BloombergLP::bslstl_UtilIterator,
                               int);
        // Match integral type for 'INPUT_ITER'.

    template <class INPUT_ITER>
    void privateInsertDispatch(const_iterator             position,
                               INPUT_ITER                 first,
                               INPUT_ITER                 last,
                               BloombergLP::bslmf_AnyType,
                               BloombergLP::bslmf_AnyType);
        // Match non-integral type for 'INPUT_ITER'.

    template <class INPUT_ITER>
    void privateInsert(const_iterator                 position,
                       INPUT_ITER                     first,
                       INPUT_ITER                     last,
                       const std::input_iterator_tag&);
        // Specialized insertion for input iterators.

    template <class FWD_ITER>
    void privateInsert(const_iterator                   position,
                       FWD_ITER                         first,
                       FWD_ITER                         last,
                       const std::forward_iterator_tag&);
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
    // TRAITS
    typedef BloombergLP::
            bslalg_TypeTraitsGroupStlSequence<VALUE_TYPE,
                                              ALLOCATOR>  VectorTypeTraits;

    BSLALG_DECLARE_NESTED_TRAITS(Vector_Imp, VectorTypeTraits);
        // Declare nested type traits for this class.

    // CREATORS

                  // *** 23.2.5.1 construct/copy/destroy: ***

    explicit
    Vector_Imp(const ALLOCATOR& allocator = ALLOCATOR());
        // Create an empty vector.  Optionally specify an 'allocator' used to
        // supply memory.  If 'allocator' is not specified, a
        // default-constructed allocator is used.

    explicit
    Vector_Imp(size_type         initialSize,
               const ALLOCATOR&  allocator = ALLOCATOR());
        // Create a vector of the specified 'initialSize' whose every
        // element is default-constructed.  Optionally specify an 'allocator'
        // used to supply memory.  If 'allocator' is not specified, a
        // default-constructed allocator is used.  Throw 'std::length_error' if
        // 'initialSize > max_size()'.

    explicit
    Vector_Imp(size_type         initialSize,
               const VALUE_TYPE& value,
               const ALLOCATOR&  allocator = ALLOCATOR());
        // Create a vector of the specified 'initialSize' whose every
        // element equals the specified 'value'.  Optionally specify an
        // 'allocator' used to supply memory.  If 'allocator' is not specified,
        // a default-constructed allocator is used.  Throw 'std::length_error'
        // if 'initialSize > max_size()'.

    template <class INPUT_ITER>
    Vector_Imp(INPUT_ITER       first,
               INPUT_ITER       last,
               const ALLOCATOR& allocator = ALLOCATOR());
        // Create a vector initially containing copies of the values in the
        // range starting at the specified 'first' and ending immediately
        // before the specified 'last' iterators of the parameterized
        // 'INPUT_ITER' type.  Optionally specify an 'allocator' used to supply
        // memory.  If 'allocator' is not specified, a default-constructed
        // allocator is used.  Throw 'std::length_error' if the number of
        // elements in '[ first, last )' exceeds the value returned by the
        // method 'max_size'.

    Vector_Imp(const Vector_Imp& original);
    Vector_Imp(const Vector_Imp& original, const ALLOCATOR& allocator);
        // Create a vector that has the same value as the specified 'original'
        // vector.  Optionally specify an 'allocator' used to supply memory.
        // If 'allocator' is not specified, then if 'ALLOCATOR' is convertible
        // from 'bslma_Allocator *', the currently installed default
        // allocator is used, otherwise the 'original' allocator is used (as
        // mandated per the ISO standard).

    ~Vector_Imp();
        // Destroy this vector.

    // MANIPULATORS

                  // *** 23.2.5.1 construct/copy/assignment: ***

    Vector_Imp& operator=(const Vector_Imp& other);
        // Assign to this vector the value of the specified 'other' vector and
        // return a reference to this modifiable vector.

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last);
        // Assign to this vector the values in the range starting at the
        // specified 'first' and ending immediately before the specified 'last'
        // iterators of the parameterized 'INPUT_ITER' type.  Note that this
        // vector will be left in an empty state in case an exception is thrown
        // other than by the 'VALUE_TYPE' copy constructor or assignment
        // operator.

    void assign(size_type numElements, const VALUE_TYPE& value);
        // Assign to this vector the value of the vector of the specified
        // 'numElements' size whose every elements equal the specified
        // 'value'.  Note that this vector will be left in an empty state in
        // case an exception is thrown other than by the 'VALUE_TYPE'
        // copy constructor or assignment operator.

                         // *** 23.2.4.2 capacity: ***

    void resize(size_type newSize);
    void resize(size_type newSize, const VALUE_TYPE& value);
        // Change the size of this vector to the specified 'newSize',
        // erasing elements at the end if 'newSize < size()' or appending
        // the appropriate number of copies of the optionally specified 'value'
        // at the end if 'size() < newSize'.  If 'value' is not specified,
        // a default-constructed value is used.  Throw 'std::length_error' if
        // 'newSize > max_size()'.

    void reserve(size_type newCapacity);
        // Change the capacity of this vector to the specified 'newCapacity'.
        // Note that the capacity of a vector is the maximum number of elements
        // it can accommodate without reallocation.  The actual storage
        // allocated may be higher.

                        // *** 23.2.4.3 modifiers: ***

    void push_back(const VALUE_TYPE& value);
        // Append a copy of the specified 'value' at the end of this vector.
        // If 'value' has move semantics, then its value is valid but
        // unspecified upon returning from this function.  Note that this
        // method offers full guarantee of rollback in case an exception is
        // thrown other than by the 'VALUE_TYPE' copy constructor or assignment
        // operator.

    void pop_back();
        // Erase the last element from this vector.  The behavior is undefined
        // if this vector is empty.

    iterator insert(const_iterator position, const VALUE_TYPE& value);
        // Insert at the specified 'position' in this vector a copy of the
        // specified 'value', and return an iterator pointing to the newly
        // inserted element.  If 'value' has move semantics, then its value is
        // valid but unspecified upon returning from this function.  The
        // behavior is undefined unless 'position' is an iterator in the range
        // '[ begin(), end() ]' (both endpoints included).  Note that this
        // method offers full guarantee of rollback in case an exception is
        // thrown other than by the 'VALUE_TYPE' copy constructor or assignment
        // operator.

    void insert(const_iterator    position,
                size_type         numElements,
                const VALUE_TYPE& value);
        // Insert at the specified 'position' in this vector a number equal to
        // the 'numElements' of copies of the specified 'value'.  The behavior
        // is undefined unless 'position' is an iterator in the range '[
        // begin(), end() ]' (both endpoints included).  Note that this method
        // offers full guarantee of rollback in case an exception is throw is
        // thrown other than by the 'VALUE_TYPE' copy constructor or assignment
        // operator.

    template <class INPUT_ITER>
    void insert(const_iterator position, INPUT_ITER first, INPUT_ITER last);
        // Insert at the specified 'position' in this vector the values in the
        // range starting at the specified 'first' and ending immediately
        // before the specified 'last' iterators of the parameterized
        // 'INPUT_ITER' type.  The behavior is undefined unless 'position' is
        // an iterator in the range '[ begin(), end() ]' (both endpoints
        // included).  Note that this method offers full guarantee of rollback
        // in case an exception is thrown other than by the 'VALUE_TYPE' copy
        // constructor or assignment operator.

    iterator erase(const_iterator position);
        // Remove from this vector the element at the specified 'position', and
        // return an iterator pointing to the element immediately following the
        // removed element, or to the position returned by the 'end' method if
        // the removed element was the last in the sequence.  The behavior is
        // undefined unless 'position' is an iterator in the range
        // '[ begin(), end() )'.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this vector the elements starting at the specified
        // 'first' position that are before the specified 'last' position, and
        // return an iterator pointing to the element immediately following the
        // last removed element, or the position returned by the method 'end'
        // if the removed elements were last in the sequence.  The behavior is
        // undefined unless 'first' is an iterator in the range
        // '[ begin(), end() ]' and 'last' is an iterator in the range
        // '[ first, end() ]' (both endpoints included).

    void swap(Vector_Imp& other);
        // Exchange the value of this vector with that of the specified 'other'
        // vector, such that each vector has, upon return, the value of the
        // other vector prior to this call.  This method does not throw or
        // invalidate iterators if 'get_allocator', invoked on this vector and
        // 'other', returns the same value.

    void clear();
        // Remove all the elements from this vector.  Note that this vector is
        // empty after this call, but conserves the same capacity.

    // ACCESSORS

                  // *** 23.2.4.1 construct/copy/assignment: ***

    allocator_type get_allocator() const;
        // Return the allocator used by this vector to supply memory.

    size_type max_size() const;
        // Return the maximum possible size for this vector.  Note that
        // requests to create a vector longer than this number of elements are
        // guaranteed to raise a 'std::length_error' exception.
};

// FREE OPERATORS

                       // *** relational operators: ***
template <class VALUE_TYPE, class ALLOCATOR>
bool operator==(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' vector has the same value as the
    // specified 'rhs' vector.  Two vectors have the same value if they have
    // the same number of elements and the same element value at each
    // index position in the range 0 to 'size() - 1'.  This operator may only
    // be used when the class 'VALUE_TYPE' defines the operator:
    //..
    //  bool operator==(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..

template <class VALUE_TYPE, class ALLOCATOR>
bool operator!=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' vector does not have the same value
    // as the specified 'rhs' vector.  Two vectors do not have the same value
    // if they have different numbers of elements or different element values
    // in at least one index position in the range 0 to 'size() - 1'.  This
    // operator may only be used when the class 'VALUE_TYPE' defines the
    // operator:
    //..
    //  bool operator==(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..
    // Note that this operator returns '!(lhs == rhs)'.

template <class VALUE_TYPE, class ALLOCATOR>
bool operator< (const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' vector is lexicographically smaller
    // than the specified 'rhs' vector, and 'false' otherwise.  A vector 'lhs'
    // is lexicographically smaller than another vector 'rhs' if there exists
    // an index 'i' between 0 and the minimum of 'lhs.size()' and 'rhs.size()'
    // such that 'lhs[i] == rhs[j]' for every '0 <= j < i', 'i < rhs.size()',
    // and either 'i == lhs.size()' or 'lhs[i] < rhs[i]'.  This operator may
    // only be used when the class 'VALUE_TYPE' defines the operator:
    //..
    //  bool operator<(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..

template <class VALUE_TYPE, class ALLOCATOR>
bool operator> (const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' vector is lexicographically larger
    // than the specified 'rhs' vector, and 'false' otherwise.  This operator
    // may only be used when the class 'VALUE_TYPE' defines the operator:
    //..
    //  bool operator<(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..
    // Note that this operator returns 'rhs < lhs'.

template <class VALUE_TYPE, class ALLOCATOR>
bool operator<=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' vector is lexicographically larger
    // than or equal to the specified 'rhs' vector, and 'false' otherwise.
    // This operator may only be used when the class 'VALUE_TYPE' defines the
    // operator:
    //..
    //  bool operator<(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..
    // Note that this operator returns '!(rhs < lhs)'.

template <class VALUE_TYPE, class ALLOCATOR>
bool operator>=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' vector is lexicographically larger
    // than the specified 'rhs' vector, and 'false' otherwise.  This operator
    // may only be used when the class 'VALUE_TYPE' defines the operator:
    //..
    //  bool operator<(const VALUE_TYPE& lhs, const VALUE_TYPE& rhs);
    //..
    // Note that this operator returns '!(lhs < rhs)'.

                      // *** specialized algorithms: ***
template <class VALUE_TYPE, class ALLOCATOR>
void swap(Vector_Imp<VALUE_TYPE, ALLOCATOR>& a,
          Vector_Imp<VALUE_TYPE, ALLOCATOR>& b);
    // Exchange the value of the specified 'a' vector with that of the
    // specified 'b' vector, such that each vector has upon return the value
    // of the other vector prior to this call.  Note that this function does
    // not throw if 'lhs.get_allocator()' and 'rhs.get_allocator()' are equal.

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
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR> Base;

  public:
    // PUBLIC TYPES
    typedef typename Base::size_type          size_type;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(vector,
                                 BloombergLP::bslalg_TypeTraits<Base>);

  public:
    // 23.2.4.1 construct/copy/destroy:

    // CREATORS
    explicit vector(const ALLOCATOR&  alloc = ALLOCATOR());
        // Create an empty vector.  Optionally specify an allocator 'alloc'
        // used to supply memory.  If 'alloc' is not specified, a
        // default-constructed allocator is used.

    explicit vector(size_type        n,
                    const ALLOCATOR& alloc = ALLOCATOR());
        // Create a vector of the specified size 'n' whose every element is
        // default-constructed.  Optionally specify an 'allocator' used to
        // supply memory.  If 'allocator' is not specified, a
        // default-constructed allocator is used.

    vector(size_type         n,
           const VALUE_TYPE& value,
           const ALLOCATOR&  alloc = ALLOCATOR());
        // Create a vector of the specified size 'n' whose every element equals
        // the specified 'value'.  Optionally specify an allocator 'alloc' used
        // to supply memory.  If 'alloc' is not specified, a
        // default-constructed allocator is used.

    template <class INPUT_ITER>
    vector(INPUT_ITER       first,
           INPUT_ITER       last,
           const ALLOCATOR& alloc = ALLOCATOR());
        // Create a vector initially containing copies of the values in the
        // range starting at the specified 'first' and ending immediately
        // before the specified 'last' iterators of the parameterized
        // 'INPUT_ITER' type.  Optionally specify an allocator 'alloc' used to
        // supply memory.  If 'alloc' is not specified, a default-constructed
        // allocator is used.

    vector(const vector& original);
    vector(const vector& original, const ALLOCATOR& alloc);
        // Create a vector that has the same value as the specified 'original'
        // vector.  Optionally specify an allocator 'alloc' used to supply
        // memory.  If 'alloc' is not specified, then if 'ALLOCATOR' is
        // convertible from 'bslma_Allocator *', the currently installed
        // default allocator is used, otherwise the 'original' allocator is
        // used (as mandated per the ISO standard).

    ~vector();
        // Destroy this vector.

    vector& operator=(const vector& other);
        // Assign to this vector the value of the specified 'other' vector and
        // return a reference to this modifiable vector.
};

template <class VALUE_TYPE, class ALLOCATOR>
bool
operator==(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
           const vector<VALUE_TYPE, ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool
operator!=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
           const vector<VALUE_TYPE, ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool
operator<(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
          const vector<VALUE_TYPE, ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool
operator>(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
          const vector<VALUE_TYPE, ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool
operator<=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
           const vector<VALUE_TYPE, ALLOCATOR>& rhs);

template <class VALUE_TYPE, class ALLOCATOR>
bool
operator>=(const vector<VALUE_TYPE, ALLOCATOR>& lhs,
           const vector<VALUE_TYPE, ALLOCATOR>& rhs);

                   // =====================================
                   // class vector<VALUE_TYPE *, ALLOCATOR>
                   // =====================================

template <class VALUE_TYPE, class ALLOCATOR>
class vector< VALUE_TYPE *, ALLOCATOR >
: public Vector_Imp<void *,
                    typename ALLOCATOR::template rebind<void *>::other> {
    // This partial specialization of 'vector' for pointer types to a
    // parameterized 'T' type (not 'const') is implemented in terms of the
    // 'Vector_Imp<void *>' to reduce the amount of code generated.  Note
    // that this specialization rebinds the parameterized 'ALLOCATOR' type to
    // an allocator of 'void *' so as to satisfy the invariant in 'Vector_Imp'.
    // Also note that members which don't need to be redefined are inherited
    // straightforwardly from the 'Base', although if an overloaded method
    // needs to be redefined, then all its overloads need to be redefined.

    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;

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

    BSLALG_DECLARE_NESTED_TRAITS(vector,
                                 BloombergLP::bslalg_TypeTraits<Base>);

    // 23.2.4.1 construct/copy/destroy:

    explicit vector(const ALLOCATOR& alloc = ALLOCATOR())
    : Base(BaseAlloc(alloc)) { }

    explicit vector(size_type n, const ALLOCATOR& alloc = ALLOCATOR())
    : Base(n, BaseAlloc(alloc)) { }

    vector(size_type         n,
           VALUE_TYPE       *value,
           const ALLOCATOR&  alloc = ALLOCATOR())
    : Base(n, (void *)value, BaseAlloc(alloc)) { }

    template <class INPUT_ITER>
    vector(INPUT_ITER       first,
           INPUT_ITER       last,
           const ALLOCATOR& alloc = ALLOCATOR())
    : Base(first, last, BaseAlloc(alloc)) { }

    vector(const vector& original)
    : Base(original) { }

    vector(const vector& original, const ALLOCATOR& alloc)
    : Base(original, BaseAlloc(alloc)) { }

    ~vector() { }

                  // *** 23.2.5.1 construct/copy/assignment: ***

    vector& operator=(const vector& other)
        { Base::operator=(other); return *this; }

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last)
        { Base::assign(first, last); }

    void assign(size_type numElements, VALUE_TYPE *value)
        { Base::assign(numElements, (void *)value); }

                             // *** iterators: ***

    iterator begin()
        { return (iterator)Base::begin(); }
    iterator end()
        { return (iterator)Base::end(); }
    reverse_iterator rbegin()
        { return reverse_iterator((iterator)Base::rbegin().base()); }
    reverse_iterator rend()
        { return reverse_iterator((iterator)Base::rend().base()); }

                          // *** element access: ***

    reference operator[](size_type position)
        { return (reference)Base::operator[](position); }
    reference at(size_type position)
        { return (reference)Base::at(position); }
    reference front()
        { return (reference)Base::front(); }
    reference back()
        { return (reference)Base::back(); }

                         // *** 23.2.4.2 capacity: ***

    void resize(size_type newLength)
        { Base::resize(newLength); }
    void resize(size_type newLength, VALUE_TYPE *value)
        { Base::resize(newLength, (void *)value); }

    // void reserve(size_type newCapacity);
    // can be inherited from Base without cast

                        // *** 23.2.4.3 modifiers: ***

    void push_back(VALUE_TYPE *value)
        { Base::push_back((void *)value); }

    // void pop_back();
    // can be inherited from Base without cast.

    iterator insert(const_iterator position, VALUE_TYPE *value)
        { return (iterator)Base::insert((void *const *)position,
                                        (void *)value); }

    void insert(const_iterator  position,
                size_type       numElements,
                VALUE_TYPE     *value)
        { Base::insert((void *const *)position, numElements, (void *)value); }

    template <class INPUT_ITER>
    void insert(const_iterator position, INPUT_ITER first, INPUT_ITER last)
        { Base::insert((void *const *)position, first, last); }

    iterator erase(const_iterator position)
        { return (iterator)Base::erase((void *const *)position); }

    iterator erase(const_iterator first, const_iterator last)
        { return (iterator)Base::erase((void *const *)first,
                                       (void *const *)last); }

    // void swap(Vector_Imp& other);
    // void clear();
    // can be inherited from Base without cast.

    // ACCESSORS

                  // *** 23.2.4.1 construct/copy/assignment: ***

    allocator_type get_allocator() const
        { return ALLOCATOR(Base::get_allocator()); }

    // size_type max_size();
    // can be inherited from Base without cast

                             // *** iterators: ***

    const_iterator begin() const
        { return (const_iterator)Base::begin(); }
    const_iterator cbegin() const
        { return (const_iterator)Base::cbegin(); }

    const_iterator end() const
        { return (const_iterator)Base::end(); }
    const_iterator cend() const
        { return (const_iterator)Base::cend(); }

    const_reverse_iterator rbegin() const
        { return const_reverse_iterator(
                                      (const_iterator)Base::rbegin().base()); }
    const_reverse_iterator crbegin() const
        { return const_reverse_iterator(
                                     (const_iterator)Base::crbegin().base()); }
    const_reverse_iterator rend() const
        { return const_reverse_iterator((const_iterator)Base::rend().base()); }
    const_reverse_iterator crend() const
        { return const_reverse_iterator(
                                       (const_iterator)Base::crend().base()); }

                         // *** 23.2.4.2 capacity: ***

    // size_type size();
    // size_type capacity();
    // bool empty();
    // can be inherited from Base without cast.

                          // *** element access: ***

    const_reference operator[](size_type position) const
        { return (const_reference)Base::operator[](position); }

    const_reference at(size_type position) const
        { return (const_reference)Base::at(position); }

    const_reference front() const
        { return (const_reference)Base::front(); }

    const_reference back() const
        { return (const_reference)Base::back(); }
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

template <class VALUE_TYPE, class ALLOCATOR>
void swap(vector<VALUE_TYPE *, ALLOCATOR>& a,
          vector<VALUE_TYPE *, ALLOCATOR>& b);

             // ===========================================
             // class vector<const VALUE_TYPE *, ALLCOATOR>
             // ===========================================

template <class VALUE_TYPE, class ALLOCATOR>
class vector< const VALUE_TYPE *, ALLOCATOR >
: public Vector_Imp<const void *,
                    typename ALLOCATOR::template rebind<const void *>::other> {
    // This partial specialization of 'vector' for pointer types to a
    // parameterized 'const VALUE_TYPE' is implemented in terms of the
    // 'Vector_Imp<const void *>' to reduce the amount of code generated.
    // Note that this specialization rebinds the parameterized 'ALLOCATOR' type
    // to an allocator of 'const void *' so as to satisfy the invariant in
    // 'Vector_Imp'.  Also note that members which don't need to be redefined
    // are inherited straightforwardly from the 'Base', although if an
    // overloaded method needs to be redefined, then all its overloads need to
    // be redefined.

    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;

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

    BSLALG_DECLARE_NESTED_TRAITS(vector,
                                 BloombergLP::bslalg_TypeTraits<Base>);

    // 23.2.4.1 construct/copy/destroy:

    explicit vector(const ALLOCATOR&  alloc = ALLOCATOR())
    : Base(BaseAlloc(alloc)) { }

    explicit vector(size_type         n,
                    const ALLOCATOR&  alloc = ALLOCATOR())
    : Base(n, BaseAlloc(alloc)) { }

    vector(size_type         n,
           const VALUE_TYPE *value,
           const ALLOCATOR&  alloc = ALLOCATOR())
    : Base(n, (const void *)value, BaseAlloc(alloc)) { }

    template <class INPUT_ITER>
    vector(INPUT_ITER       first,
           INPUT_ITER       last,
           const ALLOCATOR& alloc = ALLOCATOR())
    : Base(first, last, BaseAlloc(alloc)) { }

    vector(const vector& original)
    : Base(original) { }

    vector(const vector& original, const ALLOCATOR& alloc)
    : Base(original, BaseAlloc(alloc)) { }

    ~vector() { }

                  // *** 23.2.5.1 construct/copy/assignment: ***

    vector& operator=(const vector& rhs)
        { Base::operator=(rhs); return *this; }

    template <class INPUT_ITER>
    void assign(INPUT_ITER first, INPUT_ITER last)
        { Base::assign(first, last); }

    void assign(size_type numElements, const VALUE_TYPE *value)
        { Base::assign(numElements, (const void *)value); }

                             // *** iterators: ***

    iterator begin()
        { return (iterator)Base::begin(); }
    iterator end()
        { return (iterator)Base::end(); }
    reverse_iterator rbegin()
        { return reverse_iterator((iterator)Base::rbegin().base()); }
    reverse_iterator rend()
        { return reverse_iterator((iterator)Base::rend().base()); }

                          // *** element access: ***

    reference operator[](size_type position)
        { return (reference)Base::operator[](position); }
    reference at(size_type position)
        { return (reference)Base::at(position); }
    reference front()
        { return (reference)Base::front(); }
    reference back()
        { return (reference)Base::back(); }

                         // *** 23.2.4.2 capacity: ***

    void resize(size_type newLength)
        { Base::resize(newLength); }
    void resize(size_type newLength, const VALUE_TYPE *value)
        { Base::resize(newLength, (const void *)value); }

    // void reserve(size_type newCapacity);
    // can be inherited from Base without cast.

                        // *** 23.2.4.3 modifiers: ***

    void push_back(const VALUE_TYPE *value)
        { Base::push_back((const void *)value); }

    // void pop_back();
    // can be inherited from Base without cast.

    iterator insert(const_iterator position, const VALUE_TYPE *value)
        { return (iterator)Base::insert((const void *const *)position,
                                        (const void *)value); }

    void insert(const_iterator    position,
                size_type         numElements,
                const VALUE_TYPE *value)
        { Base::insert((const void *const *) position,
                       numElements,
                       (const void *)value); }

    template <class INPUT_ITER>
    void insert(const_iterator position,
                INPUT_ITER     first,
                INPUT_ITER     last)
        { Base::insert((const void *const *)position, first, last); }

    iterator erase(const_iterator position)
        { return (iterator)Base::erase((const void *const *)position); }

    iterator erase(const_iterator first, const_iterator last)
        { return (iterator)Base::erase((const void *const *)first,
                                       (const void *const *)last); }

    // void swap(vector &other);
    // void clear();
    // can be inherited from Base without cast.

    // ACCESSORS

                  // *** 23.2.4.1 construct/copy/assignment: ***

    allocator_type get_allocator() const
        { return ALLOCATOR(Base::get_allocator()); }

    // size_type max_size();
    // can be inherited from Base without cast.

                             // *** iterators: ***

    const_iterator begin() const
        { return (const_iterator)Base::begin(); }
    const_iterator cbegin() const
        { return (const_iterator)Base::cbegin(); }

    const_iterator end() const
        { return (const_iterator)Base::end(); }
    const_iterator cend() const
        { return (const_iterator)Base::cend(); }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator((const_iterator)Base::rbegin().base());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator((const_iterator)Base::crbegin().base());
    }
    const_reverse_iterator rend() const
        { return const_reverse_iterator((const_iterator)Base::rend().base()); }
    const_reverse_iterator crend() const {
        return const_reverse_iterator((const_iterator)Base::crend().base());
    }

                         // *** 23.2.4.2 capacity: ***

    // 'size()', 'capacity()', 'empty()' can be inherited from Base without
    // cast.

                          // *** element access: ***

    const_reference operator[](size_type position) const
        { return (const_reference)Base::operator[](position); }

    const_reference at(size_type position) const
        { return (const_reference)Base::at(position); }

    const_reference front() const
        { return (const_reference)Base::front(); }

    const_reference back() const
        { return (const_reference)Base::back(); }
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

template <class VALUE_TYPE, class ALLOCATOR>
void swap(vector<const VALUE_TYPE *, ALLOCATOR>& a,
          vector<const VALUE_TYPE *, ALLOCATOR>& b);

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================
// See IMPLEMENTATION NOTES in the .cpp before modifying anything below.

                          // --------------------
                          // class Vector_ImpBase
                          // --------------------

// CREATORS
template <class VALUE_TYPE>
inline
Vector_ImpBase<VALUE_TYPE>::Vector_ImpBase()
: d_dataBegin(0)
, d_dataEnd(0)
, d_capacity(0)
{
}

// MANIPULATORS

                             // *** iterators: ***
template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::iterator
Vector_ImpBase<VALUE_TYPE>::begin()
{
    return d_dataBegin;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::iterator
Vector_ImpBase<VALUE_TYPE>::end()
{
    return d_dataEnd;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reverse_iterator
Vector_ImpBase<VALUE_TYPE>::rbegin()
{
    return reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reverse_iterator
Vector_ImpBase<VALUE_TYPE>::rend()
{
    return reverse_iterator(begin());
}

                          // *** element access: ***

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reference
Vector_ImpBase<VALUE_TYPE>::operator[](size_type position)
{
    typedef BloombergLP::bsls_Types::UintPtr Uint;

    BSLS_ASSERT_SAFE(static_cast<Uint>(size()) > static_cast<Uint>(position));

    return d_dataBegin[position];
}

template <class VALUE_TYPE>
typename Vector_ImpBase<VALUE_TYPE>::reference
Vector_ImpBase<VALUE_TYPE>::at(size_type position)
{
    typedef BloombergLP::bsls_Types::UintPtr Uint;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                   static_cast<Uint>(position) >= static_cast<Uint>(size()))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                               "vector<...>::at(position): invalid position");
    }
    return d_dataBegin[position];
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reference
Vector_ImpBase<VALUE_TYPE>::front()
{
    BSLS_ASSERT_SAFE(!empty());

    return *d_dataBegin;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::reference
Vector_ImpBase<VALUE_TYPE>::back()
{
    BSLS_ASSERT_SAFE(!empty());

    return *(d_dataEnd - 1);
}

// ACCESSORS

                             // *** iterators: ***
template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_iterator
Vector_ImpBase<VALUE_TYPE>::begin() const
{
    return d_dataBegin;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_iterator
Vector_ImpBase<VALUE_TYPE>::cbegin() const
{
    return d_dataBegin;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_iterator
Vector_ImpBase<VALUE_TYPE>::end() const
{
    return d_dataEnd;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_iterator
Vector_ImpBase<VALUE_TYPE>::cend() const
{
    return d_dataEnd;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reverse_iterator
Vector_ImpBase<VALUE_TYPE>::rbegin() const
{
    return const_reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reverse_iterator
Vector_ImpBase<VALUE_TYPE>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reverse_iterator
Vector_ImpBase<VALUE_TYPE>::rend() const
{
    return const_reverse_iterator(begin());
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reverse_iterator
Vector_ImpBase<VALUE_TYPE>::crend() const
{
    return const_reverse_iterator(begin());
}

                         // *** 23.2.4.2 capacity: ***
template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::size_type
Vector_ImpBase<VALUE_TYPE>::size() const
{
    return d_dataEnd - d_dataBegin;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::size_type
Vector_ImpBase<VALUE_TYPE>::capacity() const
{
    return d_capacity;
}

template <class VALUE_TYPE>
inline
bool Vector_ImpBase<VALUE_TYPE>::empty() const
{
    return d_dataEnd == d_dataBegin;
}

                          // *** element access: ***
template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reference
Vector_ImpBase<VALUE_TYPE>::operator[](size_type position) const
{
    typedef BloombergLP::bsls_Types::UintPtr Uint;

    BSLS_ASSERT_SAFE(static_cast<Uint>(size()) > static_cast<Uint>(position));

    return d_dataBegin[position];
}

template <class VALUE_TYPE>
typename Vector_ImpBase<VALUE_TYPE>::const_reference
Vector_ImpBase<VALUE_TYPE>::at(size_type position) const
{
    typedef BloombergLP::bsls_Types::UintPtr Uint;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(static_cast<Uint>(position) >=
                                                  static_cast<Uint>(size()))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwOutOfRange(
                          "const vector<...>::at(position): invalid position");
    }
    return d_dataBegin[position];
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reference
Vector_ImpBase<VALUE_TYPE>::front() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *d_dataBegin;
}

template <class VALUE_TYPE>
inline
typename Vector_ImpBase<VALUE_TYPE>::const_reference
Vector_ImpBase<VALUE_TYPE>::back() const
{
    BSLS_ASSERT_SAFE(!empty());

    return *(d_dataEnd - 1);
}

             // ----------------------------------------------
             // class Vector_Imp<VALUE_TYPE, ALLOCATOR>::Guard
             // ----------------------------------------------

// CREATORS
template <typename VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Guard::Guard(VALUE_TYPE          *data,
                                                std::size_t          capacity,
                                                VectorContainerBase *container)
: d_data_p(data)
, d_capacity(capacity)
, d_container_p(container)
{
}

template <typename VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Guard::~Guard()
{
    if (d_data_p) {
        d_container_p->deallocateN(d_data_p, d_capacity);
    }
}

// MANIPULATORS
template <typename VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::Guard::release()
{
    d_data_p = 0;
}

                            // ----------------
                            // class Vector_Imp
                            // ----------------

// PRIVATE MANIPULATORS
template <typename VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateInsertDispatch(
                                     const_iterator                   position,
                                     INPUT_ITER                       count,
                                     INPUT_ITER                       value,
                                     BloombergLP::bslstl_UtilIterator,
                                     int)
{
    // 'count' and 'value' are integral types that just happen to be the same.
    // They are not iterators, so we call 'insert(position, count, value)'.

    this->insert(position,
                 static_cast<size_type>(count),
                 static_cast<VALUE_TYPE>(value));
}

template <typename VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateInsertDispatch(
                                           const_iterator             position,
                                           INPUT_ITER                 first,
                                           INPUT_ITER                 last,
                                           BloombergLP::bslmf_AnyType,
                                           BloombergLP::bslmf_AnyType)
{
    // Dispatch based on iterator category.

    typedef typename bsl::iterator_traits<INPUT_ITER>::iterator_category Tag;
    this->privateInsert(position, first, last, Tag());
}

template <typename VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateInsert(
                                      const_iterator                  position,
                                      INPUT_ITER                      first,
                                      INPUT_ITER                      last,
                                      const std::input_iterator_tag&)
{
    // IMPLEMENTATION NOTES: We can't compute size in advance.  Bootstrap
    // insertion with random-access iterator by using temporary vector (which
    // will also guarantee that if allocator throws, then vector is unchanged).
    // Also, use the same allocator for temporary vector so that its elements
    // can be moved into the current one, rather than copied.  Finally,
    // construct temporary vector by iterated 'push_back', which may reallocate
    // temporary vector several times, but unfortunately we can't compute the
    // size in advance (as with 'forward_iterator_tag') because input
    // iterators can only be traversed once.

    if (first == last) {
        // Avoid creating a 'temp' vector in that case.

        return;                                                       // RETURN
    }

    // Make sure we don't shrink the capacity if this vector is empty, because
    // it will be swapped.  Note that '[ first, last )' is not empty so there
    // is no harm reserving one element otherwise, in fact, it will speed up
    // the first 'push_back'.

    const bool isEmpty = this->empty();
    Vector_Imp temp(this->get_allocator());
    temp.privateReserveEmpty(isEmpty ? this->capacity() : size_type(1));

    while (first != last) {
        temp.push_back(*first);
        ++first;
    }

    if (isEmpty) {
        // Optimization: no need to insert in an empty vector, just swap.

        Vector_Util::swap(&this->d_dataBegin, &temp.d_dataBegin);
        return;                                                       // RETURN
    }

    this->privateMoveInsert(&temp, position);
}

template <typename VALUE_TYPE, class ALLOCATOR>
template <typename FWD_ITER>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateInsert(
                                    const_iterator                    position,
                                    FWD_ITER                          first,
                                    FWD_ITER                          last,
                                    const std::forward_iterator_tag&)
{
    // Specialization for all iterators except input iterators.
    // Size can be computed in advance.

    const iterator& pos = const_cast<iterator>(position);

    const size_type maxSize = max_size();
    const size_type n = bsl::distance(first, last);

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(n > maxSize - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
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

        BloombergLP::bslalg_ArrayPrimitives::destructiveMoveAndInsert(
                                                       temp.d_dataBegin,
                                                       &this->d_dataEnd,
                                                       this->d_dataBegin,
                                                       pos,
                                                       this->d_dataEnd,
                                                       first,
                                                       last,
                                                       n,
                                                       this->bslmaAllocator());
        temp.d_dataEnd += newSize;
        Vector_Util::swap(&this->d_dataBegin, &temp.d_dataBegin);
    }
    else {
        BloombergLP::bslalg_ArrayPrimitives::insert(pos,
                                                    this->end(),
                                                    first,
                                                    last,
                                                    n,
                                                    this->bslmaAllocator());
        this->d_dataEnd += n;
    }
}

template <typename VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateMoveInsert(
                                                    Vector_Imp     *fromVector,
                                                    const_iterator  position)
{
    const iterator& pos = const_cast<const iterator&>(position);

    const size_type maxSize = max_size();
    const size_type n = fromVector->size();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(n > maxSize - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
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

        BloombergLP::bslalg_ArrayPrimitives::destructiveMoveAndMoveInsert(
                                                       temp.d_dataBegin,
                                                       &this->d_dataEnd,
                                                       &fromVector->d_dataEnd,
                                                       this->d_dataBegin,
                                                       pos,
                                                       this->d_dataEnd,
                                                       fromVector->d_dataBegin,
                                                       fromVector->d_dataEnd,
                                                       n,
                                                       this->bslmaAllocator());
        temp.d_dataEnd += newSize;
        Vector_Util::swap(&this->d_dataBegin, &temp.d_dataBegin);
    }
    else {
        BloombergLP::bslalg_ArrayPrimitives::moveInsert(
                                                       pos,
                                                       this->end(),
                                                       &fromVector->d_dataEnd,
                                                       fromVector->d_dataBegin,
                                                       fromVector->d_dataEnd,
                                                       n,
                                                       this->bslmaAllocator());
        this->d_dataEnd += n;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::privateReserveEmpty(
                                                         size_type numElements)
{
    BSLS_ASSERT_SAFE(this->empty());
    BSLS_ASSERT_SAFE(0 == this->capacity());

    this->d_dataBegin = this->d_dataEnd = this->allocateN(
                                                (VALUE_TYPE *) 0, numElements);
    this->d_capacity = numElements;
}

// CREATORS

                  // *** 23.2.4.1 construct/copy/destroy: ***
template <class VALUE_TYPE, class ALLOCATOR>
inline
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(const ALLOCATOR& allocator)
: Vector_ImpBase<VALUE_TYPE>()
, VectorContainerBase(allocator)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(size_type        initialSize,
                                              const ALLOCATOR& allocator)
: Vector_ImpBase<VALUE_TYPE>()
, VectorContainerBase(allocator)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(initialSize > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                                  "vector<...>::vector(n,v): vector too long");
    }
    if (initialSize > 0) {
        privateReserveEmpty(initialSize);
        Guard guard(this->d_dataBegin,
                    this->d_capacity,
                    static_cast<VectorContainerBase *>(this));

        BloombergLP::bslalg_ArrayPrimitives::defaultConstruct(
                                                       this->d_dataBegin,
                                                       initialSize,
                                                       this->bslmaAllocator());

        guard.release();
        this->d_dataEnd += initialSize;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(size_type         initialSize,
                                              const VALUE_TYPE& value,
                                              const ALLOCATOR&  allocator)
: Vector_ImpBase<VALUE_TYPE>()
, VectorContainerBase(allocator)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(initialSize > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                              "vector<...>::insert(pos,n,v): vector too long");
    }
    if (initialSize > 0) {
        privateReserveEmpty(initialSize);
        Guard guard(this->d_dataBegin,
                    this->d_capacity,
                    static_cast<VectorContainerBase *>(this));

        BloombergLP::bslalg_ArrayPrimitives::uninitializedFillN(
                                                       this->d_dataBegin,
                                                       initialSize,
                                                       value,
                                                       this->bslmaAllocator());

        guard.release();
        this->d_dataEnd += initialSize;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::Vector_Imp(INPUT_ITER       first,
                                              INPUT_ITER       last,
                                              const ALLOCATOR& allocator)
: Vector_ImpBase<VALUE_TYPE>()
, VectorContainerBase(allocator)
{
    insert(this->begin(), first, last);
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::
Vector_Imp(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& original)
: Vector_ImpBase<VALUE_TYPE>()
, VectorContainerBase(original)
{
    if (original.size() > 0) {
        privateReserveEmpty(original.size());
        Guard guard(this->d_dataBegin,
                    this->d_capacity,
                    static_cast<VectorContainerBase *>(this));

        BloombergLP::bslalg_ArrayPrimitives::copyConstruct(
                                                       this->d_dataBegin,
                                                       original.begin(),
                                                       original.end(),
                                                       this->bslmaAllocator());

        guard.release();
        this->d_dataEnd += original.size();
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::
Vector_Imp(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& original,
           const ALLOCATOR&                         allocator)
: Vector_ImpBase<VALUE_TYPE>()
, VectorContainerBase(allocator)
{
    if (original.size() > 0) {
        privateReserveEmpty(original.size());
        Guard guard(this->d_dataBegin,
                    this->d_capacity,
                    static_cast<VectorContainerBase *>(this));

        BloombergLP::bslalg_ArrayPrimitives::copyConstruct(
                                                       this->d_dataBegin,
                                                       original.begin(),
                                                       original.end(),
                                                       this->bslmaAllocator());

        guard.release();
        this->d_dataEnd += original.size();
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>::~Vector_Imp()
{
    if (this->d_dataBegin) {
        BloombergLP::
                  bslalg_ArrayDestructionPrimitives::destroy(this->d_dataBegin,
                                                             this->d_dataEnd);
        this->deallocateN(this->d_dataBegin, this->d_capacity);
    }
}

// MANIPULATORS
template <class VALUE_TYPE, class ALLOCATOR>
Vector_Imp<VALUE_TYPE, ALLOCATOR>&
Vector_Imp<VALUE_TYPE, ALLOCATOR>::operator=(
                                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& other)
{
    if (this != &other) {

        // Invoke 'erase' only if the current vector is not empty.

        if (!this->empty()) {
            erase(this->begin(), this->end());
        }

        insert(this->begin(), other.begin(), other.end());
    }
    return *this;
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::assign(INPUT_ITER first,
                                               INPUT_ITER last)
{
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

                         // *** 23.2.4.2 capacity: ***
template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::resize(size_type newSize)
{
    if (newSize <= this->size()) {
        BloombergLP::bslalg_ArrayDestructionPrimitives::destroy(
                                                   this->d_dataBegin + newSize,
                                                   this->d_dataEnd);
        this->d_dataEnd = this->d_dataBegin + newSize;
    }
    else {

        // Check whether there's room at the end of the list.  If there is,
        // build the temporary directly there.  Note that 'newSize' cannot
        // be 0, since it is always true that 'this->size() >= 0'.

        if (newSize < this->d_capacity) {

            BloombergLP::bslalg_ScalarPrimitives::defaultConstruct(
                                                       this->d_dataEnd,
                                                       this->bslmaAllocator());

            const VALUE_TYPE& defaultValue = *(this->d_dataEnd);
            ++this->d_dataEnd;  // prevent memory leak in case of exception

            insert(this->d_dataEnd,
                   newSize - this->size(),
                   defaultValue);
        }
        else {

            BloombergLP::bslalg_ConstructorProxy<VALUE_TYPE> defaultValue(
                                      BloombergLP::bslma_Default::allocator());
            insert(this->d_dataEnd,
                   newSize - this->size(),
                   defaultValue.object());
        }
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::resize(size_type         newSize,
                                               const VALUE_TYPE& value)
{
    if (newSize < this->size()) {
        BloombergLP::bslalg_ArrayDestructionPrimitives::destroy(
                                                   this->d_dataBegin + newSize,
                                                   this->d_dataEnd);
        this->d_dataEnd = this->d_dataBegin + newSize;
    }
    else {
        insert(this->d_dataEnd, newSize - this->size(), value);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::reserve(size_type newCapacity)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(newCapacity > max_size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
                         "vector<...>::reserve(newCapacity): vector too long");
    }
    if (0 == this->d_capacity && 0 != newCapacity) {
        privateReserveEmpty(newCapacity);
    }
    else if (this->d_capacity < newCapacity) {
        Vector_Imp temp(this->get_allocator());
        temp.privateReserveEmpty(newCapacity);

        BloombergLP::bslalg_ArrayPrimitives::destructiveMove(
                                                       temp.d_dataBegin,
                                                       this->d_dataBegin,
                                                       this->d_dataEnd,
                                                       this->bslmaAllocator());

        temp.d_dataEnd += this->size();
        this->d_dataEnd = this->d_dataBegin;
        Vector_Util::swap(&this->d_dataBegin, &temp.d_dataBegin);
    }
}

                        // *** 23.2.4.3 modifiers: ***
template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::push_back(const VALUE_TYPE& value)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this->d_capacity > this->size())) {
        BloombergLP::bslalg_ScalarPrimitives::copyConstruct(
                                                       this->d_dataEnd,
                                                       value,
                                                       this->bslmaAllocator());
        ++this->d_dataEnd;
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        insert(this->d_dataEnd, value);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::pop_back()
{
    BSLS_ASSERT_SAFE(!this->empty());

    BloombergLP::bslalg_ScalarDestructionPrimitives
                                                  ::destroy(--this->d_dataEnd);
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
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::insert(const_iterator    position,
                                               size_type         numElements,
                                               const VALUE_TYPE& value)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <= this->end());

    const iterator& pos = const_cast<const iterator&>(position);

    const size_type maxSize = max_size();
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                       numElements > maxSize - this->size())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        BloombergLP::bslstl_StdExceptUtil::throwLengthError(
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

        BloombergLP::bslalg_ArrayPrimitives::destructiveMoveAndInsert(
                                                       temp.d_dataBegin,
                                                       &this->d_dataEnd,
                                                       this->d_dataBegin,
                                                       pos,
                                                       this->d_dataEnd,
                                                       value,
                                                       numElements,
                                                       this->bslmaAllocator());

        temp.d_dataEnd += newSize;
        Vector_Util::swap(&this->d_dataBegin, &temp.d_dataBegin);
    }
    else {
        BloombergLP::bslalg_ArrayPrimitives::insert(pos,
                                                    this->end(),
                                                    value,
                                                    numElements,
                                                    this->bslmaAllocator());
        this->d_dataEnd += numElements;
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::insert(const_iterator position,
                                               INPUT_ITER     first,
                                               INPUT_ITER     last)
{
    BSLS_ASSERT_SAFE(this->begin() <= position);
    BSLS_ASSERT_SAFE(position      <= this->end());

    // If 'first' and 'last' are integral, then they are not iterators and we
    // should call 'insert(position, first, last)', where 'first' is actually
    // a misnamed count, and 'last' is a misnamed value.  We can assume that
    // any fundamental type passed to this function is integral or else
    // compilation errors will result.  The extra argument, 0, is to avoid an
    // overloading ambiguity: In case 'first' is an integral type, it would be
    // convertible both to 'bslstl_UtilIterator' and 'bslmf_AnyType'; but the 0
    // will be an exact match to 'int', so the overload with
    // 'bslstl_UtilIterator' will be preferred.

    privateInsertDispatch(position, first, last, first, 0);
}

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
    BloombergLP::bslalg_ArrayPrimitives::erase(const_cast<VALUE_TYPE *>(first),
                                               const_cast<VALUE_TYPE *>(last),
                                               this->d_dataEnd,
                                               this->bslmaAllocator());
    this->d_dataEnd -= n;
    return const_cast<VALUE_TYPE *>(first);
}

template <class VALUE_TYPE, class ALLOCATOR>
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::swap(
                                      Vector_Imp<VALUE_TYPE, ALLOCATOR>& other)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                             this->get_allocator() == other.get_allocator())) {
        Vector_Util::swap(&this->d_dataBegin, &other.d_dataBegin);
    }
    else {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        Vector_Imp v1(other, this->get_allocator());
        Vector_Imp v2(*this, other.get_allocator());

        Vector_Util::swap(&v1.d_dataBegin, &this->d_dataBegin);
        Vector_Util::swap(&v2.d_dataBegin, &other.d_dataBegin);
    }
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void Vector_Imp<VALUE_TYPE, ALLOCATOR>::clear()
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
Vector_Imp<VALUE_TYPE, ALLOCATOR>::get_allocator() const
{
    return VectorContainerBase::allocator();
}

                         // *** 23.2.4.2 capacity: ***
template <class VALUE_TYPE, class ALLOCATOR>
inline
typename Vector_Imp<VALUE_TYPE, ALLOCATOR>::size_type
Vector_Imp<VALUE_TYPE, ALLOCATOR>::max_size() const
{
    return VectorContainerBase::allocator().max_size();
}

// FREE OPERATORS

                       // *** relational operators: ***
template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator==(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return BloombergLP::bslalg_RangeCompare::equal(lhs.begin(),
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
    return 0 > BloombergLP::bslalg_RangeCompare::lexicographical(lhs.begin(),
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
bool operator>=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return ! (lhs < rhs);
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<=(const Vector_Imp<VALUE_TYPE, ALLOCATOR>& lhs,
                const Vector_Imp<VALUE_TYPE, ALLOCATOR>& rhs)
{
    return ! (rhs < lhs);
}

                            // ------------
                            // class vector
                            // ------------

// CREATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(const ALLOCATOR& alloc)
: Base(alloc)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(size_type n, const ALLOCATOR& alloc)
: Base(n, alloc)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(size_type         n,
                                      const VALUE_TYPE& value,
                                      const ALLOCATOR&  alloc)
: Base(n, value, alloc)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
template <class INPUT_ITER>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(INPUT_ITER       first,
                                      INPUT_ITER       last,
                                      const ALLOCATOR& alloc)
: Base(first, last, alloc)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(
                                 const vector<VALUE_TYPE, ALLOCATOR>& original)
: Base(original)
{
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
vector<VALUE_TYPE, ALLOCATOR>::vector(
                                 const vector<VALUE_TYPE, ALLOCATOR>& original,
                                 const ALLOCATOR&                     alloc)
: Base(original, alloc)
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
vector<VALUE_TYPE, ALLOCATOR>::operator=(
                                    const vector<VALUE_TYPE, ALLOCATOR>& other)
{
    Base::operator=(other);
    return *this;
}

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

                       // *** specialized algorithms: ***
template <class VALUE_TYPE, class ALLOCATOR>
inline
void swap(vector<VALUE_TYPE, ALLOCATOR>& a,
          vector<VALUE_TYPE, ALLOCATOR>& b)
{
    typedef Vector_Imp<VALUE_TYPE, ALLOCATOR> Base;
    static_cast<Base&>(a).swap(static_cast<Base&>(b));
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

template <class VALUE_TYPE, class ALLOCATOR>
inline
void swap(vector<VALUE_TYPE *, ALLOCATOR>& a,
          vector<VALUE_TYPE *, ALLOCATOR>& b)
{
    typedef typename ALLOCATOR::template rebind<void *>::other BaseAlloc;
    typedef Vector_Imp<void *, BaseAlloc>                      Base;
    static_cast<Base&>(a).swap(static_cast<Base&>(b));
}

             // -------------------------------------------
             // class vector<const VALUE_TYPE *, ALLCOATOR>
             // -------------------------------------------

// FREE OPERATORS
template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator==(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
    return operator==(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator!=(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
    return operator!=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
    return operator<(static_cast<const Base&>(lhs),
                     static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
               const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
    return operator>(static_cast<const Base&>(lhs),
                     static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator<=(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
    return operator<=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
bool operator>=(const vector<const VALUE_TYPE *,ALLOCATOR>& lhs,
                const vector<const VALUE_TYPE *,ALLOCATOR>& rhs)
{
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
    return operator>=(static_cast<const Base&>(lhs),
                      static_cast<const Base&>(rhs));
}

template <class VALUE_TYPE, class ALLOCATOR>
inline
void swap(vector<const VALUE_TYPE *, ALLOCATOR>& a,
          vector<const VALUE_TYPE *, ALLOCATOR>& b)
{
    typedef typename ALLOCATOR::template rebind<const void *>::other BaseAlloc;
    typedef Vector_Imp<const void *, BaseAlloc>                      Base;
    static_cast<Base&>(a).swap(static_cast<Base&>(b));
}

}  // close namespace bsl

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
