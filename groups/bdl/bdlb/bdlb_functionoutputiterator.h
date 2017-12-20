// bdlb_functionoutputiterator.h                                      -*-C++-*-
#ifndef INCLUDED_BDLB_FUNCTIONOUTPUTITERATOR
#define INCLUDED_BDLB_FUNCTIONOUTPUTITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provides an output iterator for a client-supplied functor.
//
//@CLASSES:
//  bdlb::FunctionOutputIterator: function output iterator template
//
//@SEE_ALSO: bdlb_nulloutputiterator
//
//@DESCRIPTION: This component provides an iterator template mechanism,
// 'bdlb::FunctionOutputIterator', that adapts a client supplied functor (or
// function pointer) to a C++ compliant output iterator.  This component allows
// clients to create custom output iterators easily.
//
// A 'bdlb::FunctionOutputIterator' instance's template parameter type
// 'FUNCTION' must be a functor (or function) that can be called as if it has
// the following signature:
//..
//  void operator()(const TYPE&)'
//..
// where 'TYPE' is the type of the object that will be assigned (by clients)
// to the dereferenced iterator.  For example:
//..
//  void myFunction(const int& value) {};
//  typedef void (*MyFunctionPtr)(const int&);
//  typedef bdlb::FunctionOutputIterator<MyFunctionPtr> MyFunctionIterator;
//
//  MyFunctionIterator it(&foo);
//  *it = 5;                       // Calls 'myFunction(5)'!
//..
// Notice that assigning 5 to the dereferenced output iterator invokes the
// function with the value 5.
//
// The provided output iterator has the following attributes:
//
//: o Meets the requirements for an output iterator according to the
//:   C++ Standard (C++11, Section 24.2.4 [output.iterators]).
//:
//: o Dereferencing an iterator and assigning to the result leads to a call
//:   of the functional object owned by the iterator.  The value assigned to
//:   the dereferenced iterator is passed to a call of the function or functor
//:   held by the iterator as a constant reference.  In other words, the
//:   assignment '*it = value' is equivalent to 'function(value)'.
//:
//: o Incrementing an iterator is a no-op.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Adapting a Free-Function to an Output Iterator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want use a provided function 'foo', that prints integers in some
// predefined format, to print each unique element of an array.  Instead of
// manually writing a loop and checking for duplicate elements, we would like
// to use a standard algorithm such as 'unique_copy'.  However, 'unique_copy'
// takes in an output iterator instead of a free function.  We can use
// 'bdlb::FunctionOutputIterator' to adapt 'foo' into an output iterator that
// is acceptable by 'unique_copy'.
//
// First, we define the type 'Function' and a function of that type 'foo':
//..
//  typedef void (*Function)(const int&);
//
//  void foo(const int& value)
//  {
//      bsl::cout << value << " ";
//  }
//..
// Then, we define a data sequence to process:
//..
//  enum { NUM_VALUES_1 = 7 };
//  const int array1[NUM_VALUES_1] = { 2, 3, 3, 5, 7, 11, 11 };
//..
// Next, we use 'bdlb::FunctionOutputIterator' to wrap 'foo' for use in the
// algorithm 'bsl::unqiue_copy':
//..
//  unique_copy(
//      array1,
//      array1 + NUM_VALUES,
//      bdlb::FunctionOutputIterator<Function>(&foo));
//..
// Notice, that each time 'bsl::unique_copy' copies an element from the
// supplied range and assigns it to the output iterator, the function 'foo' is
// called for the element.
//
// Finally, the resulting console output:
//..
//  2 3 5 7 11
//..
//
///Example 2: Adapting A Functor to An Output Iterator
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates using a 'bdlb::FunctionOutputIterator'
// with a user defined functor object.  Consider the 'Accumulator' class for
// accumulating integer values into a total.  We want to adapt 'Accumulator'
// for use with the algorithm 'bsl::unique_copy'.
//
// First, we define an 'Accumulator' class that will total the values supplied
// to the 'increment' method:
//..
//  class Accumulator {
//      // This class provides a value accumulating functionality.
//
//      // DATA
//      int d_sum;
//    public:
//      // CREATORS
//      Accumulator() : d_sum(0) {};
//
//      // MANIPULATORS
//      void increment(int value) { d_sum += value; };
//
//      // ACCESSORS
//      int total() const { return d_sum; }
//  };
//..
// Next, we define a functor, 'AccumulatorFunctor', that adapts 'Accumulator'
// to a function object:
//..
//
//  class AccumulatorFunctor {
//      // This class implements a function object that invokes 'increment' in
//      // response of calling operator()(int).
//
//      // DATA
//      Accumulator *d_accumulator_p;  // accumulator (held, not owned)
//
//    public:
//      // CREATORS
//      explicit AccumulatorFunctor(Accumulator *accumulator)
//     : d_accumulator_p(accumulator)
//      {}
//
//      // MANIPULATORS
//      void operator()(int value) { d_accumulator_p->increment(value); };
//  };
//..
// Then, we define data sequence to process:
//..
//  enum { NUM_VALUES_2 = 7 };
//  const int   array2[NUM_VALUES_2] = { 2, 3, 3, 5, 7, 11, 11 };
//..
// Next, we create a 'bdlb::FunctionOutputIterator' for 'AccumulatorFunctor'
// and supply it to the 'bsl::unique_copy' algorithm to accumulate a sequence
// of values:
//..
//  Accumulator accumulator;
//  unique_copy(
//      array2,
//      array2 + NUM_VALUES_2,
//      bdlb::FunctionOutputIterator<AccumulatorFunctor>(
//          AccumulatorFunctor(&accumulator)));
//..
// Finally, we observe that 'accumulator' holds the accumulated total of
// unique values in 'array':
//..
//  assert(28 == accumulator.total());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

namespace BloombergLP {
namespace bdlb {

#if defined(BSLS_PLATFORM_CMP_SUN) && !defined(BDE_BUILD_TARGET_STLPORT)
// Sun Studio compilers have non-standard iterator behavior requiring iterators
// to inherit from 'iterator' (rather than simply meeting the needs of
// 'std::iterator_traits').  In addition, Sun Studio requires the 'value_type'
// of the iterator to be instantiable (i.e., not 'void' as permitted by the
// C++ standard).
#define BDLB_SUNITERATORWORKAROUND \
     : public bsl::iterator<bsl::output_iterator_tag, void *, void, void, void>
#else
#define BDLB_SUNITERATORWORKAROUND
#endif

                     // ============================
                     // class FunctionOutputIterator
                     // ============================

template <class FUNCTION>
class FunctionOutputIterator BDLB_SUNITERATORWORKAROUND {
    // Provide an output iterator that calls an object of the (template
    // parameter) type 'FUNCTION'.  If 'FUNCTION' is a functor, de-referencing
    // this iterator and assigning to the result (of dereferencing) will call
    // the 'operator()' of the functor with the assigned value as a parameter.
    // Similarly, if 'FUNCTION' if a function pointer type,  assigning to the
    // dereferenced iterator will call the function supplied at construction
    // with the assigned value as a parameter.

    // PRIVATE TYPES
    class AssignmentProxy {
        // Provide an object that can appear on the left side of an assignment
        // from 'TYPE'.  The assignment to an instance of 'AssignmentProxy'
        // results in a call of 'operator(TYPE)' of the functor or function
        // supplied at construction.  Instances of this class are created every
        // time an object of the host class is dereferenced.

        // DATA
        FUNCTION& d_function; // reference to functional object to be invoked
                              // when value assigned to the instance of this
                              // class
      public:
        // CREATORS
        explicit AssignmentProxy(FUNCTION& function);
            // Create 'AssignmentProxy' object having the specified 'function'
            // value.

        // MANIPULATORS
        template <class TYPE>
        AssignmentProxy& operator=(const TYPE& rhs);
            // Invoke 'd_function' with the specified 'rhs' as a parameter.
            // The behavior is undefined unless 'FUNCTION' is a function
            // pointer type and a valid function pointer was supplied at
            // construction.
    };

    // DATA
    FUNCTION d_function; // functional object to be invoked when value is
                         // assigned to dereferenced instance of this class

  public:
    // TYPES
    typedef bsl::output_iterator_tag iterator_category;
    typedef void                     difference_type;
    typedef void                     value_type;
    typedef void                     reference;
    typedef void                     pointer;
        // Provide type aliases required by C++ standard 'iterator_traits'.

    // CREATORS
    FunctionOutputIterator();
        // Create a 'FunctionOutputIterator' object that, when an assignment is
        // performed on the dereferenced object, will call a default
        // constructed instance of the (template parameter) type 'FUNCTION'
        // passing the assigned value as the argument.  Note that if 'FUNCTION'
        // is a function pointer type, then the default constructed 'FUNCTION'
        // will be 0, and the behavior when assigning to a dereferenced
        // iterator will be undefined.

    explicit FunctionOutputIterator(const FUNCTION& function);
        // Create 'FunctionOutputIterator' object that, when an assignment is
        // performed on the dereferenced object, will call the specified
        // 'function' passing the assigned value as the argument.

    //! FunctionOutputIterator(const FunctionOutputIterator &rhs) = default;
        // Create a 'FunctionOutputIterator' object that, when an assignment is
        // performed on the dereferenced object, will call the same function or
        // functor used by the specified 'rhs' object.

    //! ~FunctionOutputIterator() = default;
        // Destroy this object.


    // MANIPULATORS
    //! FunctionOutputIterator& operator=(
    //                            const FunctionOutputIterator &rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    AssignmentProxy operator*();
        // Return an object that can appear on the left-hand side of an
        // assignment from 'TYPE'.  When a value is assigned to the returned
        // value, invoke the functor or function indicated at construction
        // supplying the assigned value as the parameter.  This function is
        // non-const in accordance with the input iterator requirements, even
        // though '*this' is not modified.   Note that if 'FUNCTION' is a
        // function pointer type and a valid function pointer was not supplied
        // at construction, then the behavior when assigning to a dereferenced
        // iterator will be undefined.
};

// FREE OPERATORS
template <class FUNCTION>
inline
FunctionOutputIterator<FUNCTION>& operator++(
                                   FunctionOutputIterator<FUNCTION>& iterator);
    // Do nothing and return specified 'iterator'.

template <class FUNCTION>
inline
FunctionOutputIterator<FUNCTION> operator++(
                              FunctionOutputIterator<FUNCTION>& iterator, int);
    // Do nothing and return specified 'iterator'.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

              // ---------------------------------------------
              // class FunctionOutputIterator::AssignmentProxy
              // ---------------------------------------------

// CREATORS
template <class FUNCTION>
inline
FunctionOutputIterator<FUNCTION>::AssignmentProxy::AssignmentProxy(
                                                            FUNCTION& function)
    : d_function(function)
{
}

// MANIPULATORS
template <class FUNCTION>
template <class TYPE>
inline
typename FunctionOutputIterator<FUNCTION>::AssignmentProxy&
FunctionOutputIterator<FUNCTION>::AssignmentProxy::operator=(const TYPE& rhs)
{
    d_function(rhs);
    return *this;
}
                        // ----------------------------
                        // class FunctionOutputIterator
                        // ----------------------------

// CREATORS
template <class FUNCTION>
inline
FunctionOutputIterator<FUNCTION>::FunctionOutputIterator()
    : d_function()
{
}

template <class FUNCTION>
inline
FunctionOutputIterator<FUNCTION>::FunctionOutputIterator(
                                                      const FUNCTION& function)
    : d_function(function)
{
}

// MANIPULATORS
template <class FUNCTION>
inline
typename FunctionOutputIterator<FUNCTION>::AssignmentProxy
FunctionOutputIterator<FUNCTION>::operator*()
{
    return AssignmentProxy(d_function);
}

// FREE OPERATORS
template <class FUNCTION>
inline
FunctionOutputIterator<FUNCTION>&
operator++(FunctionOutputIterator<FUNCTION>& iterator)
{
    return iterator;
}

template <class FUNCTION>
inline
FunctionOutputIterator<FUNCTION>
operator++(FunctionOutputIterator<FUNCTION>& iterator, int)
{
    return iterator;
}

#undef BDLB_SUNITERATORWORKAROUND

}  // close package namespace
}  // close enterprise namespace

#endif
// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
