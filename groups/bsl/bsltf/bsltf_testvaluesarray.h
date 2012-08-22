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
//           bsltf::TestValuesArray: container for values used for testing.
//   bsltf::TestValuesArrayIterator: iterators for the container
//
//@AUTHOR: Raymond Chiu (schiu49)
//
//@SEE ALSO: bsltf_testfacility
//
//@DESCRIPTION: This component defines a class 'TestValuesArray' providing a
// a uniform interface for creating and accessing an array of test values of
// type that may or may not have a default constructor.
//
// This component also defines an iterator class 'TestValuesArrayIterator'
// providing access to elements in a 'TestValuesArray' object.
// 'TestValuesArrayIterator' is designed to satisfies the minimal requirement
// of an input iterator as defined by the C++11 standard [24.2.3].  It uses the
// 'BSLS_ASSERT' macro to detect undefined behavior.
//
///Iterator
/// - - - -
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
//      const int NUM_DATA = sizeof DATA / sizeof *DATA;
//..
//  Now, for each set of test values, verify that the function return the
//  expected result.
//..
//      for (int i = 0; i < NUM_DATA; ++i) {
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

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslma_default.h>
#endif

#ifndef INCLUDED_ITERATOR
#include <iterator>
#define INCLUDED_ITERATOR
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>
#define INCLUDED_CSTRING
#endif

namespace BloombergLP {
namespace bsltf {

                       // =============================
                       // class TestValuesArrayIterator
                       // =============================

template <class VALUE>
class TestValuesArrayIterator {
    // This class provide a STL-conforming input iterator over values used for
    // testing (see section [24.2.3 input.iterators] of the C++11 standard.  A
    // 'TestValuesArrayIterator' provide access to elements of parameterized
    // type 'VALUE'.  An iterator is considered dereferenceable all of the
    // following are satified:
    //: 1 The iterator refers to a valid element (not 'end').
    //:
    //: 2 The iterator has not been dereferenced.
    //:
    //: 3 The iterator is not a copy of another iterator of which 'operator++'
    //:   have been invoked.
    // An iterator is comparable if the iterator is not a copy of another
    // iterator of which 'operator++' have been invoked.
    //

    // DATA
    const VALUE *d_data_p;              // pointer to array of values (held,
                                        // not owned)

    const VALUE *d_end_p;               // end pointer (held, not owned)

    bool        *d_dereferenceable_p;   // indicate if dereferenceable (held,
                                        // not owned)

    bool        *d_comparable_p;        // indicate if comparable (held, not
                                        // owned)

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
    typedef std::forward_iterator_tag iterator_category;
    typedef VALUE                     value_type;
    typedef std::ptrdiff_t            difference_type;
    typedef VALUE*                    pointer;
    typedef VALUE&                    reference;
        // Standard iterator defined types [24.4.2].

  public:
    // CREATORS
    TestValuesArrayIterator(const VALUE *object,
                            const VALUE *end,
                            bool        *dereferenceable,
                            bool        *comparable);
        // Create an iterator referring to the specified 'object' for a
        // container with the specified 'end', with two arrays of boolean
        // referred to by the specified 'dereferenceable' and 'comparable
        // to indicate whether this iterator and its subsequent values
        // until 'end' is allowed to be dereferenced or compared
        // respectively.

    // MANIPULATORS
    const VALUE& operator *();
        // Return the value referred to by this object.  This object is no
        // longer dereferenceable after a call to this function.  The
        // behavior is undefined unless this iterator is dereferenceable.

    const VALUE *operator->();
        // Return the address of the value (of the parameterized 'VALUE_TYPE')
        // of the element at which this iterator is positioned.  The behavior
        // is undefined unless this iterator dereferenceable.

    TestValuesArrayIterator& operator++();
        // Move this iterator to the next element in the container.  Any copies
        // of this iterator are no longer dereferenceable or comparable.  The
        // behavior is undefined unless this iterator refers to a valid value
        // in the container.

};

template <class VALUE>
bool operator==(const TestValuesArrayIterator<VALUE>& lhs,
                const TestValuesArrayIterator<VALUE>& rhs);
    // Return 'true' if this object and 'rhs' refers to the same element, and
    // 'false' otherwise.  The behavior is undefined unless 'lhs' and 'rhs' are
    // comparable.

template <class VALUE>
bool operator!=(const TestValuesArrayIterator<VALUE>& lhs,
                const TestValuesArrayIterator<VALUE>& rhs);
    // Return 'true' if this object and 'rhs' does *not* refers to the same
    // element, and 'false' otherwise.  The behavior is undefined unless 'lhs'
    // and 'rhs' are comparable.

                       // ========================
                       // class TestTypesConverter
                       // ========================

template <class VALUE>
class TestTypesConverter {
  public:
    VALUE operator()(char value)
    {
        return bsltf::TemplateTestFacility::create<VALUE>(value);
    }
};

                       // ====================
                       // class TestValueArray
                       // ====================

template <class VALUE, class CONVERTER = TestTypesConverter<VALUE> >
class TestValuesArray {
    // This class provide a container to store values of the parameterized
    // 'VALUE', and also provide the iterators to access the values.  The
    // iterators are designed to conform to a standard input iterator, and will
    // report any misuse of the iterator.

    // DATA
    bslma::Allocator *d_allocator_p;      // allocator (held, not owned)

    VALUE            *d_data;             // pointer to memory storing the
                                          // values (owned)

    size_t            d_size;             // number of elements in this object

    bool             *d_dereferenceable;  // pointer to an array to indicate
                                          // which iterator is dereferenceable
                                          // (owned)

    bool             *d_comparable;       // pointer to an array to indicate
                                          // which iterator is comparable
                                          // (owned)

  private:
    // NOT IMPLEMENTED
    TestValuesArray(const TestValuesArray&);

  private:
    // PRIVATE MANIPULATOR
    void initialize(const char *spec, bslma::Allocator *basicAllocator);
        // Initialize this object with the specified 'spec' using the specified
        // 'basicAllocator' to supply memory.

  public:
    typedef TestValuesArrayIterator<VALUE> iterator;
        // Iterator for this container.

  public:
    // CREATORS
    explicit TestValuesArray(bslma::Allocator *basicAllocator = 0);
    explicit TestValuesArray(const char      *spec,
                             bslma::Allocator *basicAllocator = 0);
        // Create an object containing 52 distinct values of type 'VALUE'.
        // Optionally, specified 'spec' to indicate the values this object
        // should contain, where the values are created by invoking the
        // 'bsltf::TemplateTestFacility::create' method on each character of
        // 'spec'.  Optionally, specify 'basicAllocator' to used to supply
        // memory.

    ~TestValuesArray();
        // Destroy this object.

    // MANIPULATORS
    VALUE *data();
        // Return the address of the first element in this object.

    iterator begin();
        // Return an iterator to the first element.

    iterator index(size_t value);
        // Return an iterator to the element at the specified 'index'.

    iterator end();
        // Return an iterator to the past-the-end element.

    void resetIterators();
        // Make all iterators dereferenceable and comparable again.

    // ACCESSORS
    const VALUE *data() const;
        // Return the address of the first element in this object.

    const VALUE& operator[](size_t index) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'index'.

    size_t size() const;
        // Return number of elements in this object.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // class TestValuesArrayIterator
                       // -----------------------------

// CREATORS
template <class VALUE>
TestValuesArrayIterator<VALUE>::TestValuesArrayIterator(
                                                  const VALUE *object,
                                                  const VALUE *end,
                                                  bool        *dereferenceable,
                                                  bool        *comparable)
: d_data_p(object)
, d_end_p(end)
, d_dereferenceable_p(dereferenceable)
, d_comparable_p(comparable)
{
}

// MANIPULATORS
template <class VALUE>
const VALUE& TestValuesArrayIterator<VALUE>::operator *()
{
    BSLS_ASSERT_OPT(*d_dereferenceable_p);

    *d_dereferenceable_p = false;
    return *d_data_p;
}

template <class VALUE>
const VALUE *TestValuesArrayIterator<VALUE>::operator->()
{
    BSLS_ASSERT_OPT(*d_dereferenceable_p);

    *d_dereferenceable_p = false;
    return d_data_p;
}

template <class VALUE>
TestValuesArrayIterator<VALUE>&
TestValuesArrayIterator<VALUE>::operator++()
{
    BSLS_ASSERT_OPT(d_data_p != d_end_p);

    *d_dereferenceable_p = false;
    *d_comparable_p = false;

    ++d_data_p;
    ++d_dereferenceable_p;
    ++d_comparable_p;
    return *this;
}

}  // close package namespace

// FREE OPERATORS
template <class VALUE>
inline
bool bsltf::operator==(const bsltf::TestValuesArrayIterator<VALUE>& lhs,
                       const bsltf::TestValuesArrayIterator<VALUE>& rhs)
{
    BSLS_ASSERT_OPT(*lhs.d_comparable_p);
    BSLS_ASSERT_OPT(*rhs.d_comparable_p);

    return lhs.d_data_p == rhs.d_data_p;
}

template <class VALUE>
inline
bool bsltf::operator!=(const bsltf::TestValuesArrayIterator<VALUE>& lhs,
                       const bsltf::TestValuesArrayIterator<VALUE>& rhs)
{
    return !(lhs == rhs);
}

namespace bsltf {

                       // --------------------
                       // class TestValueArray
                       // --------------------

// PRIVATE MANIPULATORS
template <class VALUE, class CONVERTER>
void TestValuesArray<VALUE, CONVERTER>::initialize(
                                              const char       *spec,
                                              bslma::Allocator *basicAllocator)
{
    d_size = std::strlen(spec);

    d_allocator_p = bslma::Default::allocator(basicAllocator);

    // Allocate all memory in one go.

    d_data = reinterpret_cast<VALUE *>(d_allocator_p->allocate(
                    d_size * sizeof(VALUE) + 2 * (d_size + 1) * sizeof(bool)));

    d_dereferenceable = reinterpret_cast<bool *>(d_data + d_size);
    d_comparable = d_dereferenceable + d_size + 1;

    for (int i = 0; '\0' != spec[i]; ++i) {
        bslalg::ScalarPrimitives::copyConstruct(
                    data() + i,
                    CONVERTER()(spec[i]),
                    d_allocator_p);
    }

    std::memset(d_dereferenceable, true, d_size * sizeof(bool));
    d_dereferenceable[d_size] = false;  // 'end' is not dereferenceable
    std::memset(d_comparable, true, (d_size + 1) * sizeof(bool));
}

// CREATORS
template <class VALUE, class CONVERTER>
TestValuesArray<VALUE, CONVERTER>::TestValuesArray(
                                              bslma::Allocator *basicAllocator)
{
    static const char DEFAULT_SPEC[] =
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    initialize(DEFAULT_SPEC, basicAllocator);
}

template <class VALUE, class CONVERTER>
TestValuesArray<VALUE, CONVERTER>::TestValuesArray(
                                              const char       *spec,
                                              bslma::Allocator *basicAllocator)
{
    initialize(spec, basicAllocator);
}

template <class VALUE, class CONVERTER>
TestValuesArray<VALUE, CONVERTER>::~TestValuesArray()
{
    for (size_t i = 0; i < d_size; ++i) {
        data()[i].~VALUE();
    }
    d_allocator_p->deallocate(d_data);
}

// MANIPULATORS
template <class VALUE, class CONVERTER>
VALUE *TestValuesArray<VALUE, CONVERTER>::data()
{
    return d_data;
}

template <class VALUE, class CONVERTER>
typename TestValuesArray<VALUE, CONVERTER>::iterator
TestValuesArray<VALUE, CONVERTER>::begin()
{
    return iterator(data(), data() + d_size, d_dereferenceable, d_comparable);
}

template <class VALUE, class CONVERTER>
typename TestValuesArray<VALUE, CONVERTER>::iterator
TestValuesArray<VALUE, CONVERTER>::index(size_t value)
{
    BSLS_ASSERT_OPT(value <= size());

    return iterator(data() + value,
                    data() + d_size,
                    d_dereferenceable + value,
                    d_comparable + value);
}

template <class VALUE, class CONVERTER>
typename TestValuesArray<VALUE, CONVERTER>::iterator
TestValuesArray<VALUE, CONVERTER>::end()
{
    return iterator(data() + d_size,
                    data() + d_size,
                    d_dereferenceable + d_size,
                    d_comparable + d_size);
}

template <class VALUE, class CONVERTER>
void TestValuesArray<VALUE, CONVERTER>::resetIterators()
{
    memset(d_dereferenceable, 1, d_size * sizeof(bool));
    d_dereferenceable[d_size] = false;
    memset(d_comparable, 1, (d_size + 1) * sizeof(bool));
}

// ACCESSORS
template <class VALUE, class CONVERTER>
const VALUE *TestValuesArray<VALUE, CONVERTER>::data() const
{
    return d_data;
}

template <class VALUE, class CONVERTER>
const VALUE& TestValuesArray<VALUE, CONVERTER>::operator[](size_t index) const
{
    return data()[index];
}

template <class VALUE, class CONVERTER>
size_t TestValuesArray<VALUE, CONVERTER>::size() const
{
    return d_size;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
