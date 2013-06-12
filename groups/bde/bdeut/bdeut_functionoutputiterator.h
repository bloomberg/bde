// bdeut_functionoutputiterator.h                                     -*-C++-*-
#ifndef INCLUDED_BDEUT_FUNCTIONOUTPUTITERATOR
#define INCLUDED_BDEUT_FUNCTIONOUTPUTITERATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provides a function output iterator.
//
//@CLASSES:
//  bdeut_FunctionOutputIterator: function output iterator template
//
//@SEE_ALSO: bdeut_nulloutputiterator
//
//@AUTHOR: Sergey Moiseev (smoiseev)
//
//@DESCRIPTION: This component provides a templated iterator type,
// 'bdeut_FunctionOutputIterator', with the following attributes:
//  - Meets exactly the requirements for an output iterator according to the
//    C++ Standard (C++98, Section 24.1.2 [lib.output.iterators]).
//  - Dereferencing an iterator and assigning to the result leads to a call
//    of functional object owned by iterator. Value assigned to the
//    dereferenced iterator is passed to the call of functional object
//    as a constant reference. Basically the assignmet '*it = value' causes
//    the call 'function(value)'.
//  - Incrementing an iterator is a no-op.
// The motivation for this iterator is to make it easier to create custom
// output iterator.
//
///Attributes
///----------
//..
//  Name      Type      Default  Simple Constrants
//  --------  --------  -------  -----------------------------------------
//  function  FUNCTION           this functional object should define
//                               operator() compatible with a call
//                               'function(value)'.
//..
//: o 'function': functional object of template type (FUNCTION) which
//:   is invoked when value is assigned to dereferenced iterator.
//:   So this functional object should define operator() which is compatible
//:   with the call 'function(value)'. For instance,
//:   operator()(TYPE)
//:   operator()(const TYPE)
//:   operator()(const TYPE&)
//:   The value assigned to the dereferenced iterator will be casted to the
//:   type of the only parameter of operator().
//:   Attribute 'function' is actually a copy of functional object specified
//:   in iterator constructor.
//..
//
// For example, if iterator 'it' is defined the following way:
//..
//  typedef void (*FooFn)(const int&);
//  typedef bdeut_FunctionOutputIterator<FooFn> FooFnIterator;
//  void foo(const int& value) {};
//  FooFnIterator it(&foo);
//..
// then the assignment '*it = 5' causes a call 'foo(5)'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: use of free-standing function with bdeut_FunctionOutputIterator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows how free-standing function can be used with
// 'bdeut_FunctionOutputIterator'.
// Consider we have 'foo' function that prints integers in some predefined
// format and we would like to print out all unique elements of sorted 'array'
// collection. The function 'bsl::unique_copy' iterates over
// all elements in the specified range and copies them to the output
// iterator, except consecutive duplicates. If we wrap 'foo' function into
// 'bdeut_FunctionOutputIterator' we can pass it as output iterator to
// 'bsl::unique_copy()' algorithm. Coping an element form the range to
// the output iterator invokes function 'foo' for each copied element:
//..
//  typedef void (*Function)(const int&);
//  void foo(const int& value)
//  {
//      bsl::cout << "Value=" << value << bsl::endl;
//  }
//
//  void main()
//  {
//      enum { NUM_VALUES = 7 };
//      const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
//      bsl::unique_copy(
//          array,
//          array + NUM_VALUES,
//          bdeut_FunctionOutputIterator<Function>(&foo));
//  }
//..
///Example 2: use of user defined functor
///- - - - - - - - - - - - - - - - - - -
// This example shows how a component with overloaded 'operator()' can be used
// with 'bdeut_FunctionOutputIterator'.
// Consider we have an 'Accumulator' class and we would like to accumulate all
// unique values in 'array' collection. If we wrap user defined function object
// into 'bdeut_FunctionOutputIterator' we can pass it as output iterator to
// 'bsl::unique_copy' algorithm. Coping the result to output iterator invokes
// function 'Accumulator::inc' for each copied element. Thus by the end of
// 'main' 'acc' will contain the sum of all unique elements of 'array':
//..
//  class Accumulator {
//      // This class provides a value accumulating functionality.
//
//      // DATA
//      long int d_sum;
//    public:
//      // CREATORS
//      Accumulator() : d_sum(0) {};
//      // MANIPULATORS
//      void inc(int value) { d_sum += value; };
//  };
//
//  class AccumulatorFn {
//      // This class implements function object that invokes
//      // Accumulator::inc() in response of calling operator()(int).
//
//      // DATA
//      Accumulator& d_accumulator;
//    public:
//      // CREATORS
//      explicit AccumulatorFn(Accumulator& acc) : d_accumulator(acc) {};
//      // MANIPULATORS
//      void operator()(int value) { d_accumulator.inc(value); };
//  };
//
//  void main()
//  {
//      enum { NUM_VALUES = 7 };
//      const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
//      bsl::unique_copy(
//          array,
//          array + NUM_VALUES,
//          bdeut_FunctionOutputIterator<AccumulatorFn>(
//              AccumulatorFn(acc)));
//  }
//..
///Example 3: use of bdef_MemFnInstance function object
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows how 'bdef_MemFnInstance' can be used with
// 'bdeut_FunctionOutputIterator':
//..
//  typedef bdef_MemFnInstance<void (Accumulator::*)(int), Accumulator*>
//      AccumulatorFn;
//      // Define function object
//
//  void main()
//  {
//      enum { NUM_VALUES = 7 };
//      const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
//      bsl::unique_copy(
//          array,
//          array + NUM_VALUES,
//          bdeut_FunctionOutputIterator<AccumulatorFn>(
//              AccumulatorFn(&Accumulator::inc, &acc)));
//  }
//..
///Example 4: use of bdef_Function and bdef_BindUtil::bind()
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows how the functor defined with bdef_Function and
// constructed with 'bdef_BindUtil::bind()' can be used with
// 'bdeut_FunctionOutputIterator':
//..
//  typedef bdef_Function<void (*)(int)> AccumulatorFn;
//      // Define function object
//
//  void main()
//  {
//      enum { NUM_VALUES = 7 };
//      const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
//      bsl::unique_copy(
//          array,
//          array + NUM_VALUES,
//          bdeut_FunctionOutputIterator<AccumulatorFn>(
//              bdef_BindUtil::bind(
//              &Accumulator::inc,
//              &acc,
//              bdef_PlaceHolders::_1)));
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

namespace BloombergLP {

                     // ==================================
                     // class bdeut_FunctionOutputIterator
                     // ==================================

template <class FUNCTION>
class bdeut_FunctionOutputIterator
    : public bsl::iterator<bsl::output_iterator_tag,
                           void,
                           void,
                           void,
                           void> {
    // Provide an output iterator that passes elements to a call of
    // functional object. Dereferencing this iterator and assigning to the
    // result (of dereferencing) causes to a call of operator() of
    // the functor with an assigned value as a parameter. See the Attributes
    // section under @DESCRIPTION in the component-level documentation for
    // information on the class attributes.
    //
    // This class:
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // PRIVATE TYPES
    class AssignmentProxy {
        // Provide an object that can appear on the left side of an assignment
        // from 'TYPE'.  The assignment to the instance of AssignmentProxy is
        // valid and results in call of operator(TYPE) of the functor passed as
        // a parameter of constructor. Instance of this class is created every
        // time when host class is dereferenced.

        // DATA
        FUNCTION& d_function; // reference to functional object to be invoked
                              // when value assigned to the instance of
                              // this class
      public:
        // CREATORS
        explicit AssignmentProxy(FUNCTION& function);
            // Create 'AssignmentProxy' object having the specified 'function'
            // value.

        // MANIPULATORS
        template <class TYPE>
        AssignmentProxy& operator=(const TYPE& value);
            // Invoke d_function(value).
    };

    // DATA
    FUNCTION d_function; // functional object to be invoked when value is
                         // assigned to dereferenced instance of this class

  public:
    // CREATORS
    bdeut_FunctionOutputIterator();
        // Create 'bdeut_FunctionOutputIterator' object.

    explicit bdeut_FunctionOutputIterator(const FUNCTION& function);
        // Create 'bdeut_FunctionOutputIterator' object having the specified
        // 'function' value.

    // MANIPULATORS
    AssignmentProxy operator*();
        // Return an object which can appear on the left-hand side of
        // an assignment from 'TYPE'.  The assignment itself invokes
        // functional object. This function is non-const in accordance
        // with the input iterator requirements, even though '*this'
        // is not modified.

    bdeut_FunctionOutputIterator& operator++();
        // Pre-increment operator
        // Do nothing and return '*this'.  This function is non-const in
        // accordance with the input iterator requirements, even though
        // '*this' is not modified.

    bdeut_FunctionOutputIterator& operator++(int);
        // Post-increment operator
        // Do nothing and return '*this'.  This function is non-const in
        // accordance with the input iterator requirements, even though
        // '*this' is not modified.
};

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class AssignmentProxy
                        // ---------------------

// CREATORS
template <class FUNCTION>
inline
bdeut_FunctionOutputIterator<FUNCTION>::AssignmentProxy::AssignmentProxy(
    FUNCTION& function)
    : d_function(function)
{
}

// MANIPULATORS
template <class FUNCTION>
template <class TYPE>
inline
typename bdeut_FunctionOutputIterator<FUNCTION>::AssignmentProxy&
bdeut_FunctionOutputIterator<FUNCTION>::AssignmentProxy::operator=(
    const TYPE& value)
{
    d_function(value);
    return *this;
}

                        // ----------------------------------
                        // class bdeut_FunctionOutputIterator
                        // ----------------------------------

// CREATORS
template <class FUNCTION>
inline
bdeut_FunctionOutputIterator<FUNCTION>::bdeut_FunctionOutputIterator()
    : d_function()
{
}

template <class FUNCTION>
inline
bdeut_FunctionOutputIterator<FUNCTION>::bdeut_FunctionOutputIterator(
    const FUNCTION& function)
    : d_function(function)
{
}

// MANIPULATORS
template <class FUNCTION>
inline
typename bdeut_FunctionOutputIterator<FUNCTION>::AssignmentProxy
bdeut_FunctionOutputIterator<FUNCTION>::operator*()
{
    return AssignmentProxy(d_function);
}


template <class FUNCTION>
inline
bdeut_FunctionOutputIterator<FUNCTION>&
bdeut_FunctionOutputIterator<FUNCTION>::operator++()
{
    return *this;
}

template <class FUNCTION>
inline
bdeut_FunctionOutputIterator<FUNCTION>&
bdeut_FunctionOutputIterator<FUNCTION>::operator++(int)
{
    return *this;
}

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
