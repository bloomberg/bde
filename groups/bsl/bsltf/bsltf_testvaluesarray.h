// bsltf_testvaluesarray.h                                            -*-C++-*-
#ifndef INCLUDED_BSLTF_TESTVALUESARRAY
#define INCLUDED_BSLTF_TESTVALUESARRAY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for values used for testing.
//
//@CLASSES:
//  bsltf::TestValuesArray: container for values used for testing
//  bsltf::TestValuesArrayIterator: iterator for the container
//
//@SEE_ALSO: bsltf_testfacility
//
//@DESCRIPTION: This component defines a class 'bsltf::TestValuesArray'
// providing a uniform interface for creating and accessing a sequence of test
// values of a type that has a copy constructor, and may or may not have a
// default constructor.
//
// This component also defines an iterator class
// 'bsltf::TestValuesArrayIterator' providing access to elements in a
// 'TestValuesArray' object.  'TestValuesArrayIterator' is designed to
// satisfies the minimal requirement of an input iterator as defined by the
// C++11 standard [24.2.3].  It uses the 'BSLS_ASSERT' macro to detect
// undefined behavior.
//
// The sequence described by this container is an input-range, that may be
// traversed exactly once.  Once an iterator is incremented, any other iterator
// at the same position in the sequence is invalidated.  The 'TestValuesArray'
// object provides a 'resetIterators' method that restores the ability to
// iterate the container.
//
///Iterator
///--------
// The requirements of the input iterators as defined by the C++11 standard may
// not be as tight as the users of the input iterators expected.  Incorrect
// assumptions about the properties of the input iterator may result in
// undefined behavior.  'TestValuesArrayIterator' is designed to detect
// possible incorrect usages.  Specifically, 'TestValuesArrayIterator' put
// restriction on when it can be dereferenced or compared.  A
// 'TestValuesArrayIterator' is considered to be *dereferenceable* if it
// satisfies all of the following:
//
//: 1 The iterator refers to a valid element (not 'end').
//:
//: 2 The iterator has not been dereferenced.  (*)
//:
//: 3 The iterator is not a copy of another iterator of which 'operator++'
//:   have been invoked.  (see [table 107] of the C++11 standard)
//
// *note: An input iterator may not be dereferenced more than once is a common
// requirement of a container method that takes input iterators as arguments.
// Other standard algorithms may allow the iterator to be dereferenced more
// than once, in which case, 'TestValuesArrayIterator' is not suitable to be
// used to with those algorithms.
//
// 'TestValuesArrayIterator' is comparable if the iterator is not a copy of
// another iterator of which 'operator++' have been invoked.
//
///Thread Safety
///-------------
// This component is *not* thread-safe, by any definition of the term, and
// should not be used in test scenarios concerned with concurrent code.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing a Simple Template Function
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we have a function that we would like to test.  This function
// take in a range defined by two input iterators and returns the largest value
// in that range.
//
// First, we define the function we would like to test:
//..
//  template <class VALUE, class INPUT_ITERATOR>
//  VALUE myMaxValue(INPUT_ITERATOR first, INPUT_ITERATOR last)
//      // Return the largest value referred to by the iterators in the range
//      // beginning at the specified 'first' and up to, but not including, the
//      // specified 'last'.  The behavior is undefined unless [first, last)
//      // specifies a valid range and 'first != last'.
//  {
//      assert(first != last);
//
//      VALUE largestValue(*first);
//      ++first;
//      for(;first != last; ++first) {
//          // Store in temporary variable to avoid dereferencing twice.
//
//          const VALUE& temp = *first;
//          if (largestValue < temp) {
//              largestValue = temp;
//          }
//      }
//      return largestValue;
//  }
//..
// Next, we implement a test function 'runTest' that allows the function to be
// tested with different types:
//..
//  template <class VALUE>
//  void runTest()
//      // Test driver.
//  {
//..
//  Then, we define a set of test values and expected results:
//..
//      struct {
//          const char *d_spec;
//          const char  d_result;
//      } DATA[] = {
//          { "A",     'A' },
//          { "ABC",   'C' },
//          { "ADCB",  'D' },
//          { "EDCBA", 'E' }
//      };
//      const size_t NUM_DATA = sizeof DATA / sizeof *DATA;
//..
//  Now, for each set of test values, verify that the function return the
//  expected result.
//..
//      for (size_t i = 0; i < NUM_DATA; ++i) {
//          const char *const SPEC = DATA[i].d_spec;
//          const VALUE       EXP  =
//                bsltf::TemplateTestFacility::create<VALUE>(DATA[i].d_result);
//
//          bsltf::TestValuesArray<VALUE> values(SPEC);
//          assert(EXP == myMaxValue<VALUE>(values.begin(), values.end()));
//      }
//  }
//..
// Finally, we invoke the test function to verify our function is implemented
// correctly.  The test function to run without triggering the 'assert'
// statement:
//..
//  runTest<char>();
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLTF_TEMPLATETESTFACILITY
#include <bsltf_templatetestfacility.h>
#endif

#ifndef INCLUDED_BSLMA_STDALLOCATOR
#include <bslma_stdallocator.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_ITERATOR
#include <iterator>
#define INCLUDED_ITERATOR
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>
#define INCLUDED_STRING_H
#endif

namespace BloombergLP {

namespace bsltf {

template <class VALUE, class ALLOCATOR>
struct TestValuesArray_DefaultConverter;

template <class VALUE>
class TestValuesArray_PostIncrementPtr;

                       // =============================
                       // class TestValuesArrayIterator
                       // =============================

template <class VALUE>
class TestValuesArrayIterator {
    // This class provide a STL-conforming input iterator over values used for
    // testing (see section [24.2.3 input.iterators] of the C++11 standard.  A
    // 'TestValuesArrayIterator' provide access to elements of parameterized
    // type 'VALUE'.  An iterator is considered dereferenceable all of the
    // following are satisfied:
    //: 1 The iterator refers to a valid element (not 'end').
    //:
    //: 2 The iterator has not been dereferenced.
    //:
    //: 3 The iterator is not a copy of another iterator of which 'operator++'
    //:   have been invoked.
    // An iterator is comparable if the iterator is not a copy of another
    // iterator of which 'operator++' have been invoked.
    //
    // This class is *not* thread-safe: different iterator objects manipulate
    // shared state without synchronization.  This is rarely a concern for the
    // test scenarios supported by this component.

    // DATA
    const VALUE *d_data_p;             // pointer to array of values (held,
                                       // not owned)

    const VALUE *d_end_p;              // end pointer (held, not owned)

    bool        *d_dereferenceable_p;  // indicate if dereferenceable (held,
                                       // not owned)

    bool        *d_isValid_p;          // indicate not yet invalidated (held,
                                       // not owned)

  private:
    // FRIENDS
    template <class OTHER_VALUE>
    friend bool operator==(const TestValuesArrayIterator<OTHER_VALUE>&,
                           const TestValuesArrayIterator<OTHER_VALUE>&);

    template <class OTHER_VALUE>
    friend bool operator!=(const TestValuesArrayIterator<OTHER_VALUE>&,
                           const TestValuesArrayIterator<OTHER_VALUE>&);

  public:
    // TYPES
    typedef native_std::input_iterator_tag  iterator_category;
    typedef VALUE                           value_type;
    typedef ptrdiff_t                       difference_type;
    typedef const VALUE                    *pointer;
    typedef const VALUE&                    reference;
        // Standard iterator defined types [24.4.2].

  public:
    // CREATORS
    TestValuesArrayIterator(const VALUE *object,
                            const VALUE *end,
                            bool        *dereferenceable,
                            bool        *isValid);
        // Create an iterator referring to the specified 'object' for a
        // container with the specified 'end', with two arrays of boolean
        // referred to by the specified 'dereferenceable' and 'isValid' to
        // indicate whether this iterator and its subsequent values until
        // 'end' is allowed to be dereferenced and is not yet invalidated
        // respectively.

    TestValuesArrayIterator(const TestValuesArrayIterator& original);
        // Create an iterator having the same value as the specified 'original'
        // object.  The behavior is undefined unless 'original' is valid.

    // MANIPULATORS
    TestValuesArrayIterator& operator=(const TestValuesArrayIterator& other);
        // Assign to this object the value of the specified 'other' object.
        // The behavior is undefined unless 'other' is valid.

    TestValuesArrayIterator& operator++();
        // Move this iterator to the next element in the container.  Any copies
        // of this iterator are no longer dereferenceable or comparable.  The
        // behavior is undefined unless this iterator refers to a valid value
        // in the container.

    TestValuesArray_PostIncrementPtr<VALUE> operator++(int);
        // Move this iterator to the next element in the container, and return
        // an object that can be dereferenced to refer to the same object that
        // this iterator initially points to.  Any copies of this iterator are
        // no longer dereferenceable or comparable.  The behavior is undefined
        // unless this iterator refers to a valid value in the container.

    // ACCESSORS
    const VALUE& operator *() const;
        // Return the value referred to by this object.  This object is no
        // longer dereferenceable after a call to this function.  The behavior
        // is undefined unless this iterator is dereferenceable.

    const VALUE *operator->() const;
        // Return the address of the element (of the template parameter
        // 'VALUE') at which this iterator is positioned.  The behavior is
        // undefined unless this iterator dereferenceable.
};

template <class VALUE>
bool operator==(const TestValuesArrayIterator<VALUE>& lhs,
                const TestValuesArrayIterator<VALUE>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' refer to
    // the same element, and 'false' otherwise.  The behavior is undefined
    // unless 'lhs' and 'rhs' are comparable.

template <class VALUE>
bool operator!=(const TestValuesArrayIterator<VALUE>& lhs,
                const TestValuesArrayIterator<VALUE>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' do *not*
    // refer to the same element, and 'false' otherwise.  The behavior is
    // undefined unless 'lhs' and 'rhs' are comparable.

                       // =====================
                       // class TestValuesArray
                       // =====================

template <class VALUE,
          class ALLOCATOR = bsl::allocator<VALUE>,
          class CONVERTER =
              TestValuesArray_DefaultConverter<VALUE, ALLOCATOR> >
class TestValuesArray
{
    // This class provides a container to store values of the (template
    // parameter) type 'VALUE', and also provides the iterators to access the
    // values.  The iterators are designed to conform to a standard input
    // iterator, and report any misuse of the iterator.

  private:
    // PRIVATE TYPES
    typedef typename bsl::allocator_traits<ALLOCATOR>::template
            rebind_traits<bsls::AlignmentUtil::MaxAlignedType> AllocatorTraits;
    typedef typename AllocatorTraits::allocator_type AllocatorType;
    typedef typename AllocatorTraits::size_type      size_type;

    // DATA
    ALLOCATOR  d_allocator;          // allocator (held, not owned)

    VALUE     *d_data_p;             // pointer to memory storing the values
                                     // (owned)

    size_t     d_size;               // number of elements in this container

    bool      *d_dereferenceable_p;  // pointer to an array to indicate if
                                     // value is dereferenceable (owned)

    bool      *d_validIterator_p;    // pointer to an array to indicate if
                                     // value is comparable (owned)

    // NOT IMPLEMENTED
    TestValuesArray(const TestValuesArray&);             // = delete
    TestValuesArray& operator=(const TestValuesArray&);  // = delete

    // PRIVATE MANIPULATOR
    void initialize(const char *spec);
        // Initialize this container, using the specified 'spec' to populate
        // container with test values.

  public:
    // TYPES
    typedef TestValuesArrayIterator<VALUE> iterator;
        // Iterator for this container.

  public:
    // CREATORS
    explicit TestValuesArray();
    explicit TestValuesArray(ALLOCATOR basicAllocator);
    explicit TestValuesArray(const char *spec);
    explicit TestValuesArray(const char *spec, ALLOCATOR basicAllocator);
        // Create a 'TestValuesArray' object.  Optionally, specify 'spec' to
        // indicate the values this object should contain, where the values are
        // created by invoking the 'bsltf::TemplateTestFacility::create' method
        // on each character of 'spec'.  If no 'spec' is supplied, the object
        // will contain 52 distinct values of the (template parameter) type
        // 'VALUE'.  Optionally, specify 'basicAllocator' used to supply
        // memory.  If no allocator is supplied, a 'bslma::MallocFree'
        // allocator is used to supply memory.

    ~TestValuesArray();
        // Destroy this container and all contained elements.

    // MANIPULATORS
    iterator begin();
        // Return an iterator providing non-modifiable access to the first
        // 'VALUE' object in the sequence of 'VALUE' objects maintained by this
        // container, or the 'end' iterator if this container is empty.

    iterator end();
        // Return an iterator providing access to the past-the-end position in
        // the sequence of 'VALUE' objects maintained by this container.

    iterator index(size_t position);
        // Return an iterator to the element at the specified 'position'.  The
        // behavior is undefined unless 'position <= size()'.

    void resetIterators();
        // Make all iterators dereferenceable and comparable again.

    // ACCESSORS
    const VALUE *data() const;
        // Return the address of the non-modifiable first element in this
        // container.

    const VALUE& operator[](size_t index) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'index'.  The behavior is undefined unless
        // '0 < size() && index < size()'.

    size_t size() const;
        // Return number of elements in this container.
};

                       // ======================================
                       // class TestValuesArray_DefaultConverter
                       // ======================================

template <class VALUE, class ALLOCATOR>
struct TestValuesArray_DefaultConverter
    // This 'struct' provides a namespace for an utility function,
    // 'createInplace', that creates an object of the (template parameter) type
    // 'VALUE' from a character identifier.
{
    // CLASS METHODS
    static void createInplace(VALUE *objPtr, char value, ALLOCATOR allocator);
        // Create an object of the (template parameter) type 'VALUE' at the
        // specified 'objPtr' address whose state is unique for the specified
        // 'value'.  Use the specified 'allocator' to supply memory.  The
        // behavior is undefined unless '0 <= value && value < 128' and 'VALUE'
        // is contained in the macro
        // 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL'.
};

                       // ======================================
                       // class TestValuesArray_PostIncrementPtr
                       // ======================================

template <class VALUE>
class TestValuesArray_PostIncrementPtr
    // This class is a wrapper that encapsulates a reference, providing
    // non-modifiable access to the element of 'TestValuesArray' container.
    // Object of this class is returned by post increment operator of
    // TestValuesArray' container.
{
  private:
    // DATA
    const VALUE *d_data_p;  // pointer to the value (not owned)

  public:
    // CREATORS
    explicit TestValuesArray_PostIncrementPtr(const VALUE* ptr);
        // Create a 'TestValuesArray_PostIncrementPtr' object having the value
        // of the specified 'ptr'.

    // ACCESSORS
    const VALUE& operator*() const;
        // Return a reference providing non-modifiable access to the object
        // referred to by this wrapper.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // class TestValuesArrayIterator
                       // -----------------------------

// CREATORS
template <class VALUE>
inline
TestValuesArrayIterator<VALUE>::TestValuesArrayIterator(
                                                  const VALUE *object,
                                                  const VALUE *end,
                                                  bool        *dereferenceable,
                                                  bool        *isValid)
: d_data_p(object)
, d_end_p(end)
, d_dereferenceable_p(dereferenceable)
, d_isValid_p(isValid)
{
    BSLS_ASSERT_SAFE(object);
    BSLS_ASSERT_SAFE(end);
    BSLS_ASSERT_SAFE(dereferenceable);
    BSLS_ASSERT_SAFE(isValid);
    BSLS_ASSERT_SAFE(*isValid);
}

template <class VALUE>
inline
TestValuesArrayIterator<VALUE>::TestValuesArrayIterator(
                                       const TestValuesArrayIterator& original)
: d_data_p(original.d_data_p)
, d_end_p(original.d_end_p)
, d_dereferenceable_p(original.d_dereferenceable_p)
, d_isValid_p(original.d_isValid_p)
{
    BSLS_ASSERT_OPT(*original.d_isValid_p);
}

// MANIPULATORS
template <class VALUE>
TestValuesArrayIterator<VALUE>&
TestValuesArrayIterator<VALUE>::operator=(const TestValuesArrayIterator& other)
{
    BSLS_ASSERT_OPT(*other.d_isValid_p);

    d_data_p            = other.d_data_p;
    d_end_p             = other.d_end_p;
    d_dereferenceable_p = other.d_dereferenceable_p;
    d_isValid_p         = other.d_isValid_p;

    return *this;
}

template <class VALUE>
TestValuesArrayIterator<VALUE>&
TestValuesArrayIterator<VALUE>::operator++()
{
    BSLS_ASSERT_OPT(d_data_p != d_end_p);
    BSLS_ASSERT_OPT(*d_isValid_p);

    *d_dereferenceable_p = false;
    *d_isValid_p = false;

    ++d_data_p;
    ++d_dereferenceable_p;
    ++d_isValid_p;
    return *this;
}

template <class VALUE>
TestValuesArray_PostIncrementPtr<VALUE>
TestValuesArrayIterator<VALUE>::operator++(int)
{
    BSLS_ASSERT_OPT(*d_isValid_p);
    BSLS_ASSERT_OPT(d_data_p != d_end_p);

    TestValuesArray_PostIncrementPtr<VALUE> result(d_data_p);
    this->operator++();
    return result;
}

// ACCESSORS
template <class VALUE>
inline
const VALUE& TestValuesArrayIterator<VALUE>::operator *() const
{
    BSLS_ASSERT_OPT(*d_isValid_p);
    BSLS_ASSERT_OPT(*d_dereferenceable_p);

    *d_dereferenceable_p = false;
    return *d_data_p;
}

template <class VALUE>
inline
const VALUE *TestValuesArrayIterator<VALUE>::operator->() const
{
    BSLS_ASSERT_OPT(*d_isValid_p);
    BSLS_ASSERT_OPT(*d_dereferenceable_p);

    *d_dereferenceable_p = false;
    return d_data_p;
}

}  // close package namespace

// FREE OPERATORS
template <class VALUE>
inline
bool bsltf::operator==(const bsltf::TestValuesArrayIterator<VALUE>& lhs,
                       const bsltf::TestValuesArrayIterator<VALUE>& rhs)
{
    BSLS_ASSERT_OPT(*lhs.d_isValid_p);
    BSLS_ASSERT_OPT(*rhs.d_isValid_p);

    return lhs.d_data_p == rhs.d_data_p;
}

template <class VALUE>
inline
bool bsltf::operator!=(const bsltf::TestValuesArrayIterator<VALUE>& lhs,
                       const bsltf::TestValuesArrayIterator<VALUE>& rhs)
{
    BSLS_ASSERT_OPT(*lhs.d_isValid_p);
    BSLS_ASSERT_OPT(*rhs.d_isValid_p);

    return !(lhs == rhs);
}

namespace bsltf {
                       // ---------------------
                       // class TestValuesArray
                       // ---------------------

// CREATORS
template <class VALUE, class ALLOCATOR, class CONVERTER>
TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::TestValuesArray()
: d_allocator(&bslma::MallocFreeAllocator::singleton())
{
    static const char DEFAULT_SPEC[] =
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    initialize(DEFAULT_SPEC);
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::TestValuesArray(
                                                      ALLOCATOR basicAllocator)
: d_allocator(basicAllocator)
{
    static const char DEFAULT_SPEC[] =
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    initialize(DEFAULT_SPEC);
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
inline
TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::TestValuesArray(const char *spec)
: d_allocator(&bslma::MallocFreeAllocator::singleton())
{
    initialize(spec);
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
inline
TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::TestValuesArray(
                                                    const char *spec,
                                                    ALLOCATOR   basicAllocator)
: d_allocator(basicAllocator)
{
    initialize(spec);
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::~TestValuesArray()
{
    for (size_t i = 0; i < d_size; ++i) {
        bsl::allocator_traits<ALLOCATOR>::destroy(d_allocator, d_data_p + i);
    }

    size_type numBytes = static_cast<size_type>(
                     d_size * sizeof(VALUE) + 2 * (d_size + 1) * sizeof(bool));
    size_type numMaxAlignedType =
                       (numBytes + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
                                     / bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    AllocatorType alignAlloc(d_allocator);
    AllocatorTraits::deallocate(
        alignAlloc,
        reinterpret_cast<bsls::AlignmentUtil::MaxAlignedType *>(
                                           reinterpret_cast<void *>(d_data_p)),
        numMaxAlignedType);
        // The redundant cast to 'void *' persuades gcc/Solaris that there are
        // no alignment issues to warn about.
}

// PRIVATE MANIPULATORS
template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::initialize(const char *spec)
{
    BSLS_ASSERT_SAFE(spec);

    d_size = strlen(spec);

    // Allocate all memory in one go.

    size_type numBytes = static_cast<size_type>(
                     d_size * sizeof(VALUE) + 2 * (d_size + 1) * sizeof(bool));
    size_type numMaxAlignedType =
                       (numBytes + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
                                     / bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    AllocatorType alignAlloc(d_allocator);
    d_data_p = reinterpret_cast<VALUE *>(AllocatorTraits::allocate(
        alignAlloc, numMaxAlignedType));

    d_dereferenceable_p = reinterpret_cast<bool *>(d_data_p + d_size);
    d_validIterator_p = d_dereferenceable_p + d_size + 1;

    for (int i = 0; '\0' != spec[i]; ++i) {
        CONVERTER::createInplace(d_data_p + i, spec[i], d_allocator);
    }

    memset(d_dereferenceable_p, true, d_size * sizeof(bool));
    d_dereferenceable_p[d_size] = false;  // 'end' is never dereferenceable
    memset(d_validIterator_p, true, (d_size + 1) * sizeof(bool));
}

// MANIPULATORS
template <class VALUE, class ALLOCATOR, class CONVERTER>
inline
typename TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::iterator
TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::begin()
{
    return iterator(data(),
                    data() + d_size,
                    d_dereferenceable_p,
                    d_validIterator_p);
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
inline
typename TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::iterator
TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::end()
{
    return iterator(data() + d_size,
                    data() + d_size,
                    d_dereferenceable_p + d_size,
                    d_validIterator_p + d_size);
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
inline
typename TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::iterator
TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::index(size_t position)
{
    BSLS_ASSERT_SAFE(position <= size());

    return iterator(data() + position,
                    data() + d_size,
                    d_dereferenceable_p + position,
                    d_validIterator_p + position);
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
void TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::resetIterators()
{
    memset(d_dereferenceable_p, 1, d_size * sizeof(bool));
    d_dereferenceable_p[d_size] = false;
    memset(d_validIterator_p, 1, (d_size + 1) * sizeof(bool));
}

// ACCESSORS
template <class VALUE, class ALLOCATOR, class CONVERTER>
inline
const VALUE *TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::data() const
{
    return d_data_p;
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
inline
const VALUE& TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::
operator[](size_t index) const
{
    BSLS_ASSERT_SAFE(0 < size() && index < size());

    return data()[index];
}

template <class VALUE, class ALLOCATOR, class CONVERTER>
inline
size_t TestValuesArray<VALUE, ALLOCATOR, CONVERTER>::size() const
{
    return d_size;
}

                       // --------------------------------------
                       // class TestValuesArray_DefaultConverter
                       // --------------------------------------

template <class VALUE, class ALLOCATOR>
inline
void TestValuesArray_DefaultConverter<VALUE, ALLOCATOR>::createInplace(
                                                          VALUE     *objPtr,
                                                          char       value,
                                                          ALLOCATOR  allocator)
{
    bsltf::TemplateTestFacility::emplace(objPtr, value, allocator);
}

                       // --------------------------------------
                       // class TestValuesArray_PostIncrementPtr
                       // --------------------------------------

template <class VALUE>
inline
TestValuesArray_PostIncrementPtr<VALUE>::
TestValuesArray_PostIncrementPtr(const VALUE* ptr)
: d_data_p(ptr)
{
    BSLS_ASSERT_SAFE(ptr);
}

template <class VALUE>
inline
const VALUE& TestValuesArray_PostIncrementPtr<VALUE>::operator*() const
{
    return *d_data_p;
}

}  // close package namespace
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
