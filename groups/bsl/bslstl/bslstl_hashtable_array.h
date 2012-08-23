// bslstl_hashtable_array.h          -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASHTABLE_ARRAY
#define INCLUDED_BSLSTL_HASHTABLE_ARRAY

//@PURPOSE: Provide a managed in-place array.
//
//@CLASSES:
//  bslstl::HashTable_Array: managed in-place array
//
//@SEE_ALSO: bslstl_hashtable
//
//@AUTHOR: Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: [!PRIVATE!] This component provides a class,
// 'HashTable_Array', that defines a managed array of in-place elements
// of a templatized type 'TYPE'.  The contained elements are stored
// contiguously in memory and are constructed using the default constructor of
// type 'TYPE'.  A 'HashTable_Array' cannot be copied or assigned, and
// individual elements cannot be appended or removed.  An 'Array' object does
// provide accessors to the individual elements of the array, and
// 'clear' and 'resize' method that deallocate the current array contents, and
// reallocates a new array of the indicated size (using the default
// constructor to initialize the newly-created elements) respectively.
//
// The templatized type 'TYPE' must have a public default constructor, and a
// public destructor.  'TYPE' may optionally accept a 'bslma_Allocator *'
// argument to its default constructor.  The array does not attempt to use the
// copy constructor or assignment operator of type 'TYPE'.
//
// [!WARNING!] This component should not be used directly by client code.
// It is an implementation detail of the 'bslstl_hashtable' component,
// and is subject to change without warning.
//
///Thread Safety
///-------------
// 'HashTable_Array' is *const* *thread-safe*, meaning that accessors
// may be invoked concurrently from different threads, but it is not safe to
// access or modify a 'HashTable_Array' in one thread while another
// thread modifies the same object.
//
///Usage
///-----
// In the following example we demonstrate how to use a
// 'HashTable_Array' to create an in-place array of 'bdema_ManagedPtr'
// objects.  Note that 'bdema_ManagedPtr' does not provide standard assignment
// or copy constructor methods, so it cannot be contained cleanly in a
// 'bsl::vector'.
//
// We start by declaring an array of managed-pointers to 'bdet_Date' objects,
// of length 10.
//..
//  HashTable_Array<bdema_ManagedPtr<bdet_Date> > dateArray(10);
//..
// Each element of the created array is a default constructed
// 'bdema_ManagedPtr<bdet_Date>' object -- i.e., a null managed pointer:
//..
//  assert(10 == dateArray.size());
//  for (int i = 0; i < dateArray.size(); ++i) {
//      assert(!dateArray[i]);
//  }
//..
// We can now allocate each element in the managed pointer array:
//..
//  bslma_TestAllocator allocator;
//  for (int i = 0; i < dateArray.size(); ++i) {
//      dataArray[i].load(new (allocator) bdet_Date(), &allocator);
//      assert(bdet_Date() == *dateArray[i]);
//  }
//  assert(0 < allocator.numBytesInUse());
//..
// Finally we can use 'clearAndResize' to deallocate our array of managed
// pointers, and create a new array.  In this example we supply
// 'cleanAndResize' a new size of 0, so the array will be empty after the
// method returns:
//..
//  dateArray.clearAndResize(0);
//  assert(0 == allocator.numBytesInUse());
//..
// Note that the allocator supplied to the array and the allocators supplied
// to each array element need not be the same.

#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#include <bslalg_arraydestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYPRIMITIVES
#include <bslalg_arrayprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#include <algorithm>
#include <cstddef>
namespace BloombergLP {
namespace bslstl {

                    // ============================
                    // class HashTable_Array
                    // ============================

template<class TYPE>
class HashTable_Array {
    // This class provides a mechanism for managing an in-place array of
    // elements of a templatized type 'TYPE'.  The contained elements are
    // stored contiguously in memory and are constructed using the default
    // constructor of type 'TYPE'.  The templatized type 'TYPE' must have a
    // public default constructor, and a public destructor, but an array does
    // not attempt to use the copy constructor or assignment operator of type
    // 'TYPE'.  Therefore a 'HashTable_Array' cannot be copied or
    // assigned, and individual elements cannot be appended or removed.

    // DATA
    TYPE             *d_data_p;      // 'd_size' elements of type 'TYPE'
    std::size_t       d_size;         // number of elements in 'd_data_p'
    bslma::Allocator *d_allocator_p;  // allocator (held, not owned)

    // NOT IMPLEMENTED
    HashTable_Array(const HashTable_Array&);
    HashTable_Array& operator=(const HashTable_Array&);

  private:
    // PRIVATE MANIPULATORS
    void allocateArray(std::size_t size);
        // Initialize this array to refer to a newly allocated array of the
        // specified 'size' of default constructed objects of the templatized
        // type 'TYPE'.  The behavior is undefined unless this object's
        // current 'size()' is 0 and 'size >= 0'.

  public:
    // TYPES
    typedef TYPE       *iterator;
    typedef const TYPE *const_iterator;

    // TRAITS

    //TBD: deduce bitwise movability from TYPE.
    BSLALG_DECLARE_NESTED_TRAITS(HashTable_Array,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    HashTable_Array(bslma_Allocator *basicAllocator = 0);
        // Create an array of size 0 of the parameterized type 'TYPE'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    HashTable_Array(std::size_t size, bslma_Allocator *basicAllocator = 0);
        // Create an array of the specified 'size' of the parameterized type
        // 'TYPE', using the default constructor of 'TYPE' to initialize the
        // individual elements in the array.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= size'.  Note that if 'size' is 0, no memory
        // is required for this instantiation.

    ~HashTable_Array();
        // Destroy this array.

    // MANIPULATORS
    void clear();
        // Deallocate the current contents of this array.  After this
        // operation 'size()' will be 0, and no memory will remain allocated
        // on behalf of this object from the installed allocator.

    void resize(std::size_t newSize);
    void resize(std::size_t newSize, const TYPE& value);
        // Change the size of this array to the specified 'newSize',
        // erasing elements at the end if 'newSize < size()' or appending
        // the appropriate number of copies of the optionally specified 'value'
        // at the end if 'size() < newSize'.  If 'value' is not specified,
        // a default-constructed value is used.

    TYPE& operator[](std::size_t index);
        // Return a reference to the modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified by invoking any of
        // the 'clear' and 'resize' methods.  The behavior is undefined unless
        // '0 <= index < size()'.

    iterator begin();
        // Return an 'iterator' positioned at the first element in this
        // array if it exists, and 'end()' if the size of this array is 0.

    iterator end();
        // Return a 'iterator' positioned at 'begin() + size()'.

    // ACCESSORS
    
    std::size_t size() const;
        // Return the number of elements in this array.

    const TYPE& operator[](std::size_t index) const;
        // Return a reference to the non-modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified by invoking any of
        // the 'clear' and 'resize' methods.  The behavior is undefined unless
        // '0 <= index < size()'.

    const_iterator begin() const;
        // Return a 'const_iterator' positioned at the first element in this
        // array if it exists, and 'end()' if the size of this array is 0.

    const_iterator end() const;
        // Return a 'const_iterator' positioned at 'begin() + size()'.

    bslma_Allocator *get_allocator() const;
        // Return the object allocator installed in this array.
 };

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ----------------------------
                    // class HashTable_Array
                    // ----------------------------

// PRIVATE MANIPULATORS
template <typename TYPE>
inline
void HashTable_Array<TYPE>::allocateArray(std::size_t newSize)
{
    if (newSize) {
        d_data_p =  
          static_cast<TYPE *>(d_allocator_p->allocate(sizeof(TYPE) * newSize));
        bslma::DeallocatorProctor<bslma_Allocator> autoDealloc(d_data_p,
                                                              d_allocator_p);
        bslalg::ArrayPrimitives::defaultConstruct(begin(), 
                newSize, 
                d_allocator_p);
        autoDealloc.release();
    }
    d_size = newSize;
}

template <typename TYPE> 
inline
void HashTable_Array<TYPE>::resize(std::size_t newSize) {
    if (newSize <= this->size()) {
        BloombergLP::bslalg::ArrayDestructionPrimitives::destroy(
                                                       this->begin() + newSize,
                                                       this->end());
    }
    else {

        bslalg::ConstructorProxy<TYPE> defaultValue(
                                                  bslma::Default::allocator());
        HashTable_Array<TYPE> temp(this->get_allocator());
        temp.allocateArray(newSize);

        TYPE *cursor = this->end();
        bslalg::ArrayPrimitives::destructiveMoveAndInsert(
                                                       temp.begin(),
                                                       &cursor,
                                                       this->begin(),
                                                       this->end(),
                                                       this->end(),
                                                       defaultValue.object(),
                                                       newSize - this->size(),
                                                       this->get_allocator());

        std::swap(this->d_data_p, temp.d_data_p);
        temp.d_size = d_size;
    }
    d_size = newSize;
}

// CREATORS
template <typename TYPE>
inline
HashTable_Array<TYPE>::HashTable_Array(bslma_Allocator *basicAllocator)
: d_data_p(0)
, d_size(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <typename TYPE>
inline
HashTable_Array<TYPE>::HashTable_Array(std::size_t      size,
                                       bslma_Allocator *basicAllocator)
: d_data_p(0)
, d_size(size)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    allocateArray(size);
}

template <typename TYPE>
inline
HashTable_Array<TYPE>::~HashTable_Array()
{
    clear();
}

// MANIPULATORS
template <typename TYPE>
inline
void HashTable_Array<TYPE>::clear()
{
    if (d_size) {
        bslalg_ArrayDestructionPrimitives::destroy(d_data_p,
                                                   d_data_p + d_size);
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
        d_size   = 0;
    }
}

template <typename TYPE>
inline
TYPE& HashTable_Array<TYPE>::operator[](std::size_t index)
{
    return d_data_p[index];
}

template <typename TYPE>
inline
typename HashTable_Array<TYPE>::iterator
HashTable_Array<TYPE>::begin()
{
    return d_data_p;
}

template <typename TYPE>
inline
typename HashTable_Array<TYPE>::iterator
HashTable_Array<TYPE>::end()
{
    return d_data_p + d_size;
}

// ACCESSORS
template <typename TYPE>
inline
bslma::Allocator *HashTable_Array<TYPE>::get_allocator() const
{
    return d_allocator_p;
}

template <typename TYPE>
inline
std::size_t HashTable_Array<TYPE>::size() const
{
    return d_size;
}

template <typename TYPE>
inline
const TYPE& HashTable_Array<TYPE>::operator[](std::size_t index) const
{
    return d_data_p[index];
}

template <typename TYPE>
inline
typename HashTable_Array<TYPE>::const_iterator
HashTable_Array<TYPE>::begin() const
{
    return d_data_p;
}

template <typename TYPE>
inline
typename HashTable_Array<TYPE>::const_iterator
HashTable_Array<TYPE>::end() const
{
    return d_data_p + d_size;
}

}  // close namespace a_comdb2
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
