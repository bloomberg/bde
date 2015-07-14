// bdlb_functionoutputiterator.h                                     -*-C++-*-
#ifndef INCLUDED_BDLB_FUNCTIONOUTPUTITERATOR
#define INCLUDED_BDLB_FUNCTIONOUTPUTITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provides a output iterator for a client supplied functor.
//
//@CLASSES:
//  bdlb::FunctionOutputIterator: function output iterator template
//
//@SEE_ALSO: bdlb_nulloutputiterator
//
//@AUTHOR: Sergey Moiseev (smoiseev)
//
//@DESCRIPTION: This component provides an iterator template mechanism,
// 'bdlb::FunctionOutputIterator', that adapts a client supplied functor (or
// function pointer) to a C++ compliant output iterator.  This component allows
// clients to more easily create custom output iterators.
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
//:   of the functional object owned by iterator.  The value assigned to the
//:   dereferenced iterator is passed to a call of the function or functor
//:   owned by the iterator as a constant reference.  Basically the assignment
//:   '*it = value' causes the call 'function(value)'.
//:
//: o Incrementing an iterator is a no-op.
//
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Adapting a Free-Function to an Output Iterator
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want use a provided function 'foo' that prints integers in some
// predefined format to print each unique element of an array.  Instead of
// manually writing a loop and checking for duplicate elements, we would like
// to use a standard algorithm such as 'unique_copy' do that.  However,
// 'unique_copy' takes in an output iterator instead of a free function.  We
// can use 'bdlb::FunctionOutputIterator' to adapt 'foo' into an output
// iterator that is acceptable by 'unique_copy'.
//
// First, we define that function foo:
//..
//  typedef void (*Function)(const int&);
//  void foo(const int& value)
//  {
//      bsl::cout << value << " ";
//  }
//..
// Next, we define a function, 'outputArray':
//..
//  void outputArray()
//  {
//      enum { NUM_VALUES = 7 };
//      const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
//..
// Here, we call the algorithm 'unique_copy' to iterate over all elements in
// supplied range (except consecutive duplicates) and "copy" them to the
// supplied output iterator.  We wrap the function 'foo' into a
// 'bdlb::FunctionOutputIterator' so that when we pass as that output iterator
// to the  'bsl::unique_copy' algorithm 'foo' will be called for each unique
// value in 'array':
//..
//      bsl::unique_copy(
//          array,
//          array + NUM_VALUES,
//          bdlb::FunctionOutputIterator<Function>(&foo));
//  }
//..
// Finally, we observe the resulting console output looks like:
//..
//  2 3 5 7 11
//..
//
///Example 2: Adapting A Functor to An Output Iterator
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates using a 'bdlb::FunctionOutputIterator'
// with a user defined functor object.  Consider we have an 'Accumulator'
// class, for accumulating integer values into a total, and we want to adapt
// it to use with the algorithm 'bsl::unique_copy'.
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
//      // This class implements function object that invokes 'increment' in
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
// Now, we define a function 'accumulateArray' that will create a
// 'bdlb::FunctionOutputIterator' for 'AccumulatorFunctor' and supply it to
// the 'bsl::unique_copy' algorithm to accumulate a sequence of values:
//..
//  void accumulateArray()
//  {
//      enum { NUM_VALUES = 7 };
//      const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
//      Accumulator accumulator;
//      bsl::unique_copy(
//          array,
//          array + NUM_VALUES,
//          bdlb::FunctionOutputIterator<AccumulatorFunctor>(
//              AccumulatorFunctor(&accumulator)));
//..
// Finally, we observe that 'accumulator' holds the accumulated total of
// unique values in 'array':
//..
//      assert(28 == accumulator.total());
//  }
//..
//
///Example 3: Adapting a Functor Using bdlf::Function and bdlf::BindUtil::bind
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates using a 'bdlb::FunctionOutputIterator'
// with a functor created using 'bdlf::BindUtil'.  Consider the 'Accumulator'
// class defined in Example 2, which we want to adapt to accumulate a set of
// integer values.
//
// First, define an alias to a 'bdlf::Function' having the signature of the
// accumulation functor (note that this matches the signature of
// 'Accumulator::increment' defined in example 2):
//..
//  typedef bdlf::Function<void (*)(int)> AccumulatorFunction;
//      // Define an alias for the functor type.
//
//..
// Then, we define a function 'accumulateArray2' that will accumulate the
// integer values in an array:
//..
//  void accumulateArray2()
//  {
//      enum { NUM_VALUES = 7 };
//      const int array[NUM_VALUES] = { 2, 3, 3, 5, 7, 11, 11 };
//      Accumulator accumulator;
//..
// Here, we create a 'bdlb::FunctionOutputIterator' for a functor created using
// 'bdlf::BindUtil' (matching the 'AccumulatorFunction' alias), and supply
// it to the 'bsl::unique_copy' algorithm to accumulate a sequence of values:
//..
//
//      bsl::unique_copy(
//          array,
//          array + NUM_VALUES,
//          bdlb::FunctionOutputIterator<AccumulatorFunction>(
//              bdlf::BindUtil::bind(
//              &Accumulator::increment,
//              &accumulator,
//              bdlf::PlaceHolders::_1)));
//..
// Finally, we observe that 'accumulator' holds the accumulated total of
// unique values in 'array':
//..
//      assert(28 == accumulator.total());
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

namespace BloombergLP {


#if defined(BSLS_PLATFORM_CMP_SUN) && !defined(BDE_BUILD_TARGET_STLPORT)
// Sun Studio comilers have non-standard iterator behavior requiring iterators
// to inherit from 'iterator' (rather than simply meeting the needs of
// 'std::iterator_traits').  In addition, Sun Studio requires the 'value_type'
// of the iterator to be instantiable (i.e., not 'void' as permitted by the
// C++ standard).
#define BDEUT_SUNITERATORWORKAROUND \
     : public bsl::iterator<bsl::output_iterator_tag, void *, void, void, void>
#else
#define BDEUT_SUNITERATORWORKAROUND 
#endif

namespace bdlb {                                                            

                     // ==================================
                     // class FunctionOutputIterator
                     // ==================================

template <class FUNCTION>
class FunctionOutputIterator BDEUT_SUNITERATORWORKAROUND {
    // Provide an output iterator that calls an object of the (template
    // parameter) type 'FUNCTION'.  If 'FUNCTION' is a functor, dereferencing
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
                              // when value assigned to the instance of
                              // this class
      public:
        // CREATORS
        explicit AssignmentProxy(FUNCTION& function);
            // Create 'AssignmentProxy' object having the specified 'function'
            // value.

        // MANIPULATORS
        template <class TYPE>
        AssignmentProxy& operator=(const TYPE& rhs);
            // Invoke d_function(rhs).  The behavior is undefined if
            // 'FUNCTION' is a function pointer type and a valid function
            // pointer was not supplied at construction.
    };

    // DATA
    FUNCTION d_function; // functional object to be invoked when value is
                         // assigned to dereferenced instance of this class

  public:
    // PUBLIC
    typedef bsl::output_iterator_tag iterator_category;
    typedef void                     difference_type;
    typedef void                     value_type;
    typedef void                     reference;
    typedef void                     pointer;
        // Provide type aliases required by C++ standard 'iterator_traits'.


    // CREATORS
    FunctionOutputIterator();
        // Create 'FunctionOutputIterator' object that, when an
        // assignment is performed on the dereferenced object, will call a
        // default constructed instance of the (template parameter) type
        // 'FUNCTION' passing the assigned value as the argument.  Note that
        // if 'FUNCTION' is a function pointer type, then the default
        // constructed 'FUNCTION' will be 0, and the behavior when assigning to
        // a deferenced iterator will be undefined.

    explicit FunctionOutputIterator(const FUNCTION& function);
        // Create 'FunctionOutputIterator' object that, when an
        // assignment is performed on the dereferenced object, will call the
        // specified 'function' passing the assigned value as the argument.

    //! ~FunctionOutputIterator(
    //                      const FunctionOutputIterator &rhs) = default;
        // Create 'FunctionOutputIterator' object that, when an
        // assignment is performed on the dereferenced object, will call the
        // same function or functor used by the specified 'rhs' object.

    //! ~FunctionOutputIterator() = default;
        // Destroy this object.


    // MANIPULATORS
    //! ~FunctionOutputIterator& operator=(
    //                      const FunctionOutputIterator &rhs) = default;
        // Create 'FunctionOutputIterator' object that, when an
        // assignment is performed on the dereferenced object, will call the
        // function or functor used by the specified 'rhs'.

    AssignmentProxy operator*();
        // Return an object that can appear on the left-hand side of
        // an assignment from 'TYPE'.  When a value is assinged to the
        // returned value, invoke the functor or function indicated at
        // construction supplying the assigned value as the parameter.  This
        // function is non-const in accordance with the input iterator
        // requirements, even though '*this' is not modified.   Note that
        // if 'FUNCTION' is a function pointer type and a valid function
        // pointer was not supplied at construction, then the behavior when
        // assigning to a deferenced iterator will be undefined.
};

// FREE OPERATORS
template <class FUNCTION>
inline
FunctionOutputIterator<FUNCTION>& operator++(
                             FunctionOutputIterator<FUNCTION>& iterator);
  // Do nothing and return 'iterator'.

template <class FUNCTION>
inline
FunctionOutputIterator<FUNCTION> operator++(
                        FunctionOutputIterator<FUNCTION>& iterator, int);
}  // close package namespace
  // Do nothing and return 'iterator'.

// ============================================================================
//                 INLINE DEFINITIONS
// ============================================================================

              // ---------------------------------------------------
              // class bdlb::FunctionOutputIterator::AssignmentProxy
              // ---------------------------------------------------

// CREATORS
template <class FUNCTION>
inline
bdlb::FunctionOutputIterator<FUNCTION>::AssignmentProxy::AssignmentProxy(
    FUNCTION& function)
    : d_function(function)
{
}

// MANIPULATORS
template <class FUNCTION>
template <class TYPE>
inline
typename bdlb::FunctionOutputIterator<FUNCTION>::AssignmentProxy&
bdlb::FunctionOutputIterator<FUNCTION>::AssignmentProxy::operator=(
                                                               const TYPE& rhs)
{
    d_function(rhs);
    return *this;
}

namespace bdlb {
                        // ----------------------------------
                        // class FunctionOutputIterator
                        // ----------------------------------

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
}  // close package namespace

// FREE OPERATORS
template <class FUNCTION>
inline
bdlb::FunctionOutputIterator<FUNCTION>&
bdlb::operator++(FunctionOutputIterator<FUNCTION>& iterator)
{
    return iterator;
}

template <class FUNCTION>
inline
bdlb::FunctionOutputIterator<FUNCTION>
bdlb::operator++(FunctionOutputIterator<FUNCTION>& iterator, int)
{
    return iterator;
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
