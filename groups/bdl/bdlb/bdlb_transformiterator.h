// bdlb_transformiterator.h                                           -*-C++-*-
#ifndef INCLUDED_BDLB_TRANSFORMITERATOR
#define INCLUDED_BDLB_TRANSFORMITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapping iterator that invokes a functor on dereference.
//
//@CLASSES:
//  bdlb::TransformIterator: functor-invoking iterator wrapper
//
//@DESCRIPTION: This component implements a class template,
// 'bdlb::TransformIterator', that stores an underlying iterator and a
// one-argument functor.  Iterator operations are passed through to the
// underlying iterator, with the exception of dereference.  For dereference,
// the functor is invoked on the result of dereferencing the underlying
// iterator, and the result of the functor invocation is returned.
//
// The template expects two parameters.  The first parameter, designated
// 'FUNCTOR', is the type of a callable object that can be invoked with a
// single argument.  When compiling with C++03, this type must be either a
// function pointer or otherwise have a type from which 'bslmf::ResultType' can
// determine the result type of invoking the functor (see {bslmf_resulttype}).
// The second parameter, designated 'ITERATOR', is the type of an object that
// models an iterator from which values may be obtained, i.e., a type such that
// 'bsl::iterator<ITERATOR>' exists and for which
// 'bsl::iterator<ITERATOR>::iterator_category' derives from
// 'bsl::input_iterator_tag' (see {bslstl_iterator}).  Note that object pointer
// types qualify.
//
// Note that 'bdlb::TransformIterator' is more useful in C++11 or later than in
// C++03, because lambdas can be used as function objects to match a 'FUNCTOR'
// of type 'bsl::function<RETURN_TYPE(INPUT_TYPE)>'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Summing Absolute Values
/// - - - - - - - - - - - - - - - - -
// Suppose we have a sequence of numbers and we would like to sum their
// absolute values.  We can use 'bdlb::TransformIterator' for this purpose.
//
// First, we set up the numbers:
//..
//  int data[5] = { 1, -1, 2, -2, 3 };
//..
// Then, we create the transform iterators that will convert a number to its
// absolute value.  We need iterators for both the beginning and end of the
// sequence:
//..
//  int (*abs)(int) = &bsl::abs;
//
//  bdlb::TransformIterator<int(*)(int), int*> dataBegin(data + 0, abs);
//  bdlb::TransformIterator<int(*)(int), int*> dataEnd  (data + 5, abs);
//..
// Now, we compute the sum of the absolute values of the numbers:
//..
//  int sum = bsl::accumulate(dataBegin, dataEnd, 0);
//..
// Finally, we verify that we have computed the sum correctly:
//..
//  assert(9 == sum);
//..
//
///Example 2: Totaling a Grocery List
/// - - - - - - - - - - - - - - - - -
// Suppose we have a shopping list of products and we want to compute how much
// it will cost to buy the selected items.  We can use
// 'bdlb::TransformIterator' to do the computation, looking up the price of
// each item.
//
// First, we set up the price list:
//..
//  bsl::map<bsl::string, double> prices;
//  prices["pudding"] = 1.25;
//  prices["apple"] = 0.33;
//  prices["milk"] = 2.50;
//..
// Then, we set up our shopping list:
//..
//  bsl::list<bsl::string> list;
//  list.push_back("milk");
//  list.push_back("milk");
//  list.push_back("pudding");
//..
// Next, we create a functor that will return a price given a product.  The
// following rather prolix functor at namespace scope is necessary for C++03:
//..
//  #ifndef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
//  class Pricer {
//    private:
//      // DATA
//      bsl::map<bsl::string, double> *d_prices;  // price list
//
//    public:
//      // PUBLIC TYPES
//      typedef double result_type;
//
//      // CREATORS
//      Pricer(bsl::map<bsl::string, double>& prices);
//          // Create a Pricer object using the specified 'prices'.  The
//          // lifetime of 'prices' must be at least as long as this object.
//
//      // ACCESSORS
//      double operator()(const bsl::string& product) const;
//          // Return the price of the specified 'product'.
//  };
//
//  // CREATORS
//  Pricer::Pricer(bsl::map<bsl::string, double>& prices)
//  : d_prices(&prices)
//  {
//  }
//
//  double Pricer::operator()(const bsl::string& product) const
//  {
//      return (*d_prices)[product];
//  }
//  #endif
//..
// Then, we create the functor object.  In C++11 or later, the explicit functor
// class above is unnecessary since we can use a lambda:
//..
//  #ifndef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
//  Pricer pricer(prices);
//  #else
//  auto pricer = [&](const bsl::string &product) { return prices[product]; };
//  #endif
//..
// Next, we create a pair of transform iterators to process our grocery list.
// Note that we use 'bsl::function' as the functor type to abstract away the
// difference between the C++03 and C++11 function objects being used.
//..
//  typedef bdlb::TransformIterator<bsl::function<double(const bsl::string&)>,
//                                  bsl::list<bsl::string>::iterator> ti;
//  ti groceryBegin(list.begin(), pricer);
//  ti groceryEnd(list.end(), pricer);
//..
// Now, we add up the prices of our groceries:
//..
//  double total = bsl::accumulate(groceryBegin, groceryEnd, 0.0);
//..
// Finally, we verify that we have the correct total:
//..
//  assert(6.25 == total);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

#ifndef INCLUDED_BSLMF_RESULTTYPE
#include <bslmf_resulttype.h>
#endif

#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

namespace BloombergLP {
namespace bdlb {

// FORWARD DECLARATIONS
template <class FUNCTOR, class ITERATOR>
class TransformIterator;

                      // ===============================
                      // struct TransformIterator_Traits
                      // ===============================

template <class FUNCTOR, class ITERATOR>
struct TransformIterator_Traits {
    // This component-private class defines various types that are used in the
    // implementation of the transform iterator.

    // PUBLIC TYPES

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    typedef typename bslmf::ResultType<FUNCTOR>::type ResultType;
        // Define the result type returned by the functor.  This is not
        // necessarily the same type as the dereference of the iterator.  In
        // C++03, the functor must have a 'result_type' type member.  The
        // specializations below transform function pointers to 'bsl::function'
        // so this works for those types as well.
#else
    typedef decltype(bsl::declval<FUNCTOR>()(*bsl::declval<ITERATOR>()))
                                                      ResultType;
        // Define the result type returned by the functor.  This is not
        // necessarily the same type as the dereference of the iterator.  In
        // C++11, the result type can be determined automatically.  Note that
        // various iterations of the language standard might want to instead
        // use 'std::result_of' or 'std::invoke_result' (which have been
        // variously added and then deprecated), but the following works from
        // C++11 onwards.
#endif

    typedef typename bsl::iterator_traits<ITERATOR> BaseIteratorTraits;
        // Define the iterator traits class of the underlying iterator.

    typedef typename bsl::conditional<
        bsl::is_reference<ResultType>::value,
        typename BaseIteratorTraits::iterator_category,
        bsl::input_iterator_tag>::type                  iterator_category;
        // Define the iterator category of the transform iterator.  If the
        // functor returns a reference type, we pass through the iterator
        // category of the underlying iterator, otherwise we use the input
        // iterator tag (because all the other tags require that dereferencing
        // produces a reference).

    typedef typename BaseIteratorTraits::difference_type difference_type;
    typedef typename bsl::remove_cv<
        typename bsl::remove_reference<ResultType>::type>::type  value_type;
    typedef ResultType                                           reference;
    typedef typename bsl::remove_reference<ResultType>::type    *pointer;
        // Define the remaining standard types of the transform iterator.

    typedef bsl::iterator<iterator_category,
                          value_type,
                          difference_type,
                          pointer,
                          ResultType>        Iterator;
        // Define the standard iterator specialization that will apply to the
        // transform iterator.
};

// Specialize the transform iterator traits template for functors that are
// function or function pointer types.  It is sufficient to inherit from the
// version of the traits class that corresponds to a 'bsl::function' of the
// function type parameter.

template <class RESULT, class ARGUMENT, class ITERATOR>
struct TransformIterator_Traits<RESULT (*)(ARGUMENT), ITERATOR>
: public TransformIterator_Traits<bsl::function<RESULT(ARGUMENT)>, ITERATOR> {
};

template <class RESULT, class ARGUMENT, class ITERATOR>
struct TransformIterator_Traits<RESULT(ARGUMENT), ITERATOR>
: public TransformIterator_Traits<bsl::function<RESULT(ARGUMENT)>, ITERATOR> {
};

// The transform iterator uses allocators only if at least one of its iterator
// or its functor do.  Retrieving the allocator of the transform iterator, if
// it exists, therefore can be implemented by querying subobjects.  We will use
// implementation inheritance to supply the transform iterator with an
// allocator method that will exist only when necessary.

             // ==================================================
             // struct TransformIterator_AllocatorOfIteratorMethod
             // ==================================================

template <class BASE_TYPE, bool>
struct TransformIterator_AllocatorOfIteratorMethod {
    // The 'TransformIterator_AllocatorOfIteratorMethod' class template has an
    // allocator method when its boolean template parameter is 'true', which
    // will be made to be the case when the iterator of the transform iterator
    // uses allocators.  The transform iterator type itself is supplied as
    // 'BASE_TYPE'.
};

template <class BASE_TYPE>
struct TransformIterator_AllocatorOfIteratorMethod<BASE_TYPE, true> {
    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by the underlying iterator of the
        // associated transform iterator to supply memory.  Note that this
        // class must be a base class of the transform iterator.
};

             // =================================================
             // struct TransformIterator_AllocatorOfFunctorMethod
             // =================================================

template <class BASE_TYPE, bool>
struct TransformIterator_AllocatorOfFunctorMethod {
    // The 'TransformIterator_AllocatorOfFunctorMethod' class template has an
    // allocator method when its boolean template parameter is 'true', which
    // will be made to be the case when the iterator of the transform iterator
    // does not use allocators and the functor of the transform iterator uses
    // allocators.  The transform iterator type itself is supplied as
    // 'BASE_TYPE'.
};

template <class BASE_TYPE>
struct TransformIterator_AllocatorOfFunctorMethod<BASE_TYPE, true> {
    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by the transforming functor of the
        // associated transform iterator to supply memory.  Note that this
        // class must be a base class of the transform iterator.
};

                          // =======================
                          // class TransformIterator
                          // =======================

template <class FUNCTOR, class ITERATOR>
class TransformIterator
: public TransformIterator_AllocatorOfIteratorMethod<
      TransformIterator<FUNCTOR, ITERATOR>,
      bslma::UsesBslmaAllocator<ITERATOR>::value>,
  public TransformIterator_AllocatorOfFunctorMethod<
      TransformIterator<FUNCTOR, ITERATOR>,
      !bslma::UsesBslmaAllocator<ITERATOR>::value &&
          bslma::UsesBslmaAllocator<FUNCTOR>::value> {
    // The transform iterator class itself.  Its job is to hold a functor and
    // an iterator, pass through all iterator-related operations to the held
    // iterator, and on dereference, call the functor on the result of
    // dereferencing the iterator and return the result of the call instead.

  private:
    // PRIVATE TYPES
    typedef TransformIterator_Traits<FUNCTOR, ITERATOR> Traits;
    typedef typename Traits::Iterator                   Iterator;

    // DATA
    bslalg::ConstructorProxy<ITERATOR> d_iterator;  // underlying iterator
    bslalg::ConstructorProxy<FUNCTOR>  d_functor;   // transforming functor

  public:
    // PUBLIC TYPES
    typedef typename Iterator::difference_type   difference_type;
    typedef typename Iterator::reference         reference;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::iterator_category iterator_category;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
        TransformIterator,
        bslma::UsesBslmaAllocator,
        bslma::UsesBslmaAllocator<ITERATOR>::value ||
        bslma::UsesBslmaAllocator<FUNCTOR> ::value)

    // CREATORS
    TransformIterator();
    explicit TransformIterator(bslma::Allocator *basicAllocator);
        // Construct an object of this type.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    TransformIterator(const ITERATOR&   iterator,
                      FUNCTOR           functor,
                      bslma::Allocator *basicAllocator = 0);
        // Create a 'TransformIterator' using the specified 'iterator' and
        // 'functor'.  Optionally specify 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    TransformIterator(const TransformIterator&  original,
                      bslma::Allocator         *basicAllocator = 0);
        // Create a 'TransformIterator' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    //! ~TransformIterator() = default;
        // Destroy this object.

    // MANIPULATORS
    TransformIterator& operator=(const TransformIterator& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    TransformIterator& operator+=(difference_type offset);
        // Advance the the underlying iterator of this object by the specified
        // (signed) 'offset', and return a reference providing modifiable
        // access to this object.  The behavior is undefined if so advancing
        // the underlying iterator is undefined.

    TransformIterator& operator-=(difference_type offset);
        // Regress the the underlying iterator of this object by the specified
        // (signed) 'offset', and return a reference providing modifiable
        // access to this object.  The behavior is undefined if so regressing
        // the underlying iterator is undefined.

    TransformIterator& operator++();
        // Increment the underlying iterator of this object, and return a
        // reference providing modifiable access to this object.  The behavior
        // is undefined if incrementing the underlying iterator is undefined.

    TransformIterator& operator--();
        // Decrement the underlying iterator of this object, and return a
        // reference providing modifiable access to this object.  The behavior
        // is undefined if decrementing the underlying iterator is undefined.

    reference operator*();
        // Return the result of applying the functor of this object to the
        // result of dereferencing the underlying iterator.  The behavior is
        // undefined if dereferencing the underlying iterator is undefined.
        // Note that the behavior of this method is equivalent to:
        //..
        //  functor()(*iterator())
        //..

    pointer operator->();
        // Return the address of the result of applying the functor of this
        // object to the result of dereferencing the underlying iterator.  The
        // behavior is undefined if dereferencing the underlying iterator is
        // undefined.  Note that the behavior of this method is equivalent to:
        //..
        //  &functor()(*iterator())
        //..
        // Also note that the functor must return a reference type for this
        // method to be used.

    reference operator[](difference_type offset);
        // Return the result of applying the functor of this object to the
        // result of dereferencing the underlying iterator advanced by the
        // specified (signed) 'offset'.  The behavior is undefined if so
        // advancing or dereferencing the underlying iterator is undefined.
        // Note that the behavior of this method is equivalent to:
        //..
        //  functor()(iterator()[offset])
        //..

    FUNCTOR& functor();
        // Return a reference providing modifiable access to the functor of
        // this object.

    ITERATOR& iterator();
        // Return a reference providing modifiable access to the underlying
        // iterator of this object.

                                  // Aspects

    void swap(TransformIterator& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object by applying 'swap' to each of the functor
        // and underlying iterator fields of the two objects.

    // ACCESSORS
    reference operator*() const;
        // Return the result of applying the functor of this object to the
        // result of dereferencing the underlying iterator.  The behavior is
        // undefined if dereferencing the underlying iterator is undefined.
        // Note that the behavior of this method is equivalent to:
        //..
        //  functor()(*iterator())
        //..

    pointer operator->() const;
        // Return the address of the result of applying the functor of this
        // object to the result of dereferencing the underlying iterator.  The
        // behavior is undefined if dereferencing the underlying iterator is
        // undefined.  Note that the behavior of this method is equivalent to:
        //..
        //  &functor()(*iterator())
        //..
        // Also note that the functor must return a reference type for this
        // method to be used.

    reference operator[](difference_type offset) const;
        // Return the result of applying the functor of this object to the
        // result of dereferencing the underlying iterator advanced by the
        // specified (signed) 'offset'.  The behavior is undefined if so
        // advancing or dereferencing the underlying iterator is undefined.
        // Note that the behavior of this method is equivalent to:
        //..
        //  functor()(iterator()[offset])
        //..

    const FUNCTOR& functor() const;
        // Return a 'const' reference to the functor of this object.

    const ITERATOR& iterator() const;
        // Return a 'const' reference to the underlying iterator of this
        // object.
};

// FREE OPERATORS

template <class FUNCTOR, class ITERATOR>
bool operator==(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' if the underlying iterator of the specified 'lhs' compares
    // equal to the underlying iterator of the specified 'rhs', and 'false'
    // otherwise.  The behavior is undefined if comparing the underlying
    // iterators in this way is undefined.  Note that the functors are not
    // compared.

template <class FUNCTOR, class ITERATOR>
bool operator!=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' if the underlying iterator of the specified 'lhs' compares
    // unequal to the underlying iterator of the specified 'rhs', and 'false'
    // otherwise.  The behavior is undefined if comparing the underlying
    // iterators in this way is undefined.  Note that the functors are not
    // compared.

template <class FUNCTOR, class ITERATOR>
bool operator<(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
               const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' if the underlying iterator of the specified 'lhs' compares
    // less than the underlying iterator of the specified 'rhs', and 'false'
    // otherwise.  The behavior is undefined if comparing the underlying
    // iterators in this way is undefined.  Note that the functors are not
    // compared.

template <class FUNCTOR, class ITERATOR>
bool operator>(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
               const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' if the underlying iterator of the specified 'lhs' compares
    // greater than the underlying iterator of the specified 'rhs', and 'false'
    // otherwise.  The behavior is undefined if comparing the underlying
    // iterators in this way is undefined.  Note that the functors are not
    // compared.

template <class FUNCTOR, class ITERATOR>
bool operator<=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' if the underlying iterator of the specified 'lhs' compares
    // less than or equal to the underlying iterator of the specified 'rhs',
    // and 'false' otherwise.  The behavior is undefined if comparing the
    // underlying iterators in this way is undefined.  Note that the functors
    // are not compared.

template <class FUNCTOR, class ITERATOR>
bool operator>=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs);
    // Return 'true' if the underlying iterator of the specified 'lhs' compares
    // greater than or equal to the underlying iterator of the specified 'rhs',
    // and 'false' otherwise.  The behavior is undefined if comparing the
    // underlying iterators in this way is undefined.  Note that the functors
    // are not compared.

template <class FUNCTOR, class ITERATOR>
TransformIterator<FUNCTOR, ITERATOR> operator++(
                                TransformIterator<FUNCTOR, ITERATOR>& iterator,
                                int);
    // Increment the underlying iterator of the specified 'iterator', and
    // return a copy of 'iterator' *before* the increment.  The behavior is
    // undefined if incrementing the underlying iterator is undefined.

template <class FUNCTOR, class ITERATOR>
TransformIterator<FUNCTOR, ITERATOR> operator--(
                                TransformIterator<FUNCTOR, ITERATOR>& iterator,
                                int);
    // Decrement the underlying iterator of the specified 'iterator', and
    // return a copy of 'iterator' *before* the decrement.  The behavior is
    // undefined if decrementing the underlying iterator is undefined.

template <class FUNCTOR, class ITERATOR>
TransformIterator<FUNCTOR, ITERATOR> operator+(
       const TransformIterator<FUNCTOR, ITERATOR>&                    iterator,
       typename TransformIterator<FUNCTOR, ITERATOR>::difference_type offset);
    // Return a copy of the specified 'iterator' object with its underlying
    // iterator advanced by the specified (signed) 'offset' from that of
    // 'iterator'.  The behavior is undefined if so advancing the underlying
    // iterator is undefined.

template <class FUNCTOR, class ITERATOR>
TransformIterator<FUNCTOR, ITERATOR> operator+(
      typename TransformIterator<FUNCTOR, ITERATOR>::difference_type offset,
      const TransformIterator<FUNCTOR, ITERATOR>&                    iterator);
    // Return a copy of the specified 'iterator' object with its underlying
    // iterator advanced by the specified (signed) 'offset' from that of
    // 'iterator'.  The behavior is undefined if so advancing the underlying
    // iterator is undefined.

template <class FUNCTOR, class ITERATOR>
TransformIterator<FUNCTOR, ITERATOR> operator-(
       const TransformIterator<FUNCTOR, ITERATOR>&                    iterator,
       typename TransformIterator<FUNCTOR, ITERATOR>::difference_type offset);
    // Return a copy of the specified 'iterator' object with its underlying
    // iterator regressed by the specified (signed) 'offset' from that of
    // 'iterator'.  The behavior is undefined if so regressing the underlying
    // iterator is undefined.

template <class FUNCTOR, class ITERATOR>
typename TransformIterator<FUNCTOR, ITERATOR>::difference_type operator-(
                                const TransformIterator<FUNCTOR, ITERATOR>& a,
                                const TransformIterator<FUNCTOR, ITERATOR>& b);
    // Return the result of subtracting the underlying iterator of the
    // specified 'a' object from the underlying iterator of the specified 'b'
    // object.  The behavior is undefined if this subtraction is undefined.

// FREE FUNCTIONS
template <class FUNCTOR, class ITERATOR>
void swap(TransformIterator<FUNCTOR, ITERATOR>& a,
          TransformIterator<FUNCTOR, ITERATOR>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects by
    // applying 'swap' to each of the functor and underlying iterator fields of
    // the two objects.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

             // --------------------------------------------------
             // struct TransformIterator_AllocatorOfIteratorMethod
             // --------------------------------------------------

// ACCESSORS
template <class BASE_TYPE>
inline
bslma::Allocator *TransformIterator_AllocatorOfIteratorMethod<BASE_TYPE, true>
::allocator() const
{
    return static_cast<const BASE_TYPE&>(*this).iterator().allocator();
}

             // -------------------------------------------------
             // struct TransformIterator_AllocatorOfFunctorMethod
             // -------------------------------------------------

// ACCESSORS
template <class BASE_TYPE>
inline
bslma::Allocator *TransformIterator_AllocatorOfFunctorMethod<BASE_TYPE, true>
::allocator() const
{
    return static_cast<const BASE_TYPE&>(*this).functor().allocator();
}

                          //------------------------
                          // class TransformIterator
                          //------------------------

// CREATORS
template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator()
: d_iterator(0)
, d_functor(0)
{
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator(
                                              bslma::Allocator *basicAllocator)
: d_iterator(basicAllocator)
, d_functor(basicAllocator)
{
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator(
                                              const ITERATOR&   iterator,
                                              FUNCTOR           functor,
                                              bslma::Allocator *basicAllocator)
: d_iterator(iterator, basicAllocator)
, d_functor(functor, basicAllocator)
{
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator(
                                      const TransformIterator&  original,
                                      bslma::Allocator         *basicAllocator)
: d_iterator(original.iterator(), basicAllocator)
, d_functor(original.functor(), basicAllocator)
{
}

// MANIPULATORS
template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator=(const TransformIterator& rhs)
{
    iterator() = rhs.iterator();
    functor()  = rhs.functor();

    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator+=(difference_type offset)
{
    iterator() += offset;
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator-=(difference_type offset)
{
    iterator() -= offset;
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator++()
{
    ++iterator();
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator--()
{
    --iterator();
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::reference
TransformIterator<FUNCTOR, ITERATOR>::operator*()
{
    return functor()(*iterator());
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::pointer
TransformIterator<FUNCTOR, ITERATOR>::operator->()
{
    return bsls::Util::addressOf(functor()(*iterator()));
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::reference
TransformIterator<FUNCTOR, ITERATOR>::operator[](difference_type offset)
{
    return functor()(iterator()[offset]);
}

template <class FUNCTOR, class ITERATOR>
inline
FUNCTOR& TransformIterator<FUNCTOR, ITERATOR>::functor()
{
    return d_functor.object();
}

template <class FUNCTOR, class ITERATOR>
inline
ITERATOR& TransformIterator<FUNCTOR, ITERATOR>::iterator()
{
    return d_iterator.object();
}

                                  // Aspects

template <class FUNCTOR, class ITERATOR>
inline
void TransformIterator<FUNCTOR, ITERATOR>::swap(
                                   TransformIterator<FUNCTOR, ITERATOR>& other)
{
    using bsl::swap;
    swap(functor(), other.functor());
    swap(iterator(), other.iterator());
}

// ACCESSORS
template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::reference
TransformIterator<FUNCTOR, ITERATOR>::operator*() const
{
    return functor()(*iterator());
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::pointer
TransformIterator<FUNCTOR, ITERATOR>::operator->() const
{
    return bsls::Util::addressOf(functor()(*iterator()));
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::reference
TransformIterator<FUNCTOR, ITERATOR>::operator[](difference_type offset) const
{
    return functor()(iterator()[offset]);
}

template <class FUNCTOR, class ITERATOR>
inline
const FUNCTOR& TransformIterator<FUNCTOR, ITERATOR>::functor() const
{
    return d_functor.object();
}

template <class FUNCTOR, class ITERATOR>
inline
const ITERATOR& TransformIterator<FUNCTOR, ITERATOR>::iterator() const
{
    return d_iterator.object();
}

}  // close package namespace

// FREE OPERATORS
template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator==(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                      const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() == rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator!=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                      const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() != rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator<(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                     const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() < rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator>(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                     const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() > rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator<=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                      const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() <= rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bool bdlb::operator>=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                      const TransformIterator<FUNCTOR, ITERATOR>& rhs)
{
    return lhs.iterator() >= rhs.iterator();
}

template <class FUNCTOR, class ITERATOR>
inline
bdlb::TransformIterator<FUNCTOR, ITERATOR> bdlb::operator++(
                                TransformIterator<FUNCTOR, ITERATOR>& iterator,
                                int)
{
    return TransformIterator<FUNCTOR, ITERATOR>(iterator.iterator()++,
                                                iterator.functor());
}

template <class FUNCTOR, class ITERATOR>
inline
bdlb::TransformIterator<FUNCTOR, ITERATOR> bdlb::operator--(
                                TransformIterator<FUNCTOR, ITERATOR>& iterator,
                                int)
{
    return TransformIterator<FUNCTOR, ITERATOR>(iterator.iterator()--,
                                                iterator.functor());
}

template <class FUNCTOR, class ITERATOR>
inline
bdlb::TransformIterator<FUNCTOR, ITERATOR> bdlb::operator+(
       const TransformIterator<FUNCTOR, ITERATOR>&                    iterator,
       typename TransformIterator<FUNCTOR, ITERATOR>::difference_type offset)
{
    return TransformIterator<FUNCTOR, ITERATOR>(iterator.iterator() + offset,
                                                iterator.functor());
}

template <class FUNCTOR, class ITERATOR>
inline
bdlb::TransformIterator<FUNCTOR, ITERATOR> bdlb::operator+(
       typename TransformIterator<FUNCTOR, ITERATOR>::difference_type offset,
       const TransformIterator<FUNCTOR, ITERATOR>&                    iterator)
{
    return TransformIterator<FUNCTOR, ITERATOR>(iterator.iterator() + offset,
                                                iterator.functor());
}

template <class FUNCTOR, class ITERATOR>
inline
bdlb::TransformIterator<FUNCTOR, ITERATOR> bdlb::operator-(
       const TransformIterator<FUNCTOR, ITERATOR>&                    iterator,
       typename TransformIterator<FUNCTOR, ITERATOR>::difference_type offset)
{
    return TransformIterator<FUNCTOR, ITERATOR>(iterator.iterator() - offset,
                                                iterator.functor());
}

template <class FUNCTOR, class ITERATOR>
inline
typename bdlb::TransformIterator<FUNCTOR, ITERATOR>::difference_type
bdlb::operator-(const TransformIterator<FUNCTOR, ITERATOR>& a,
                const TransformIterator<FUNCTOR, ITERATOR>& b)
{
    return a.iterator() - b.iterator();
}

// FREE FUNCTIONS
template <class FUNCTOR, class ITERATOR>
inline
void bdlb::swap(TransformIterator<FUNCTOR, ITERATOR>& a,
                TransformIterator<FUNCTOR, ITERATOR>& b)
{
    using bsl::swap;
    swap(a.functor(), b.functor());
    swap(a.iterator(), b.iterator());
}

}  // close enterprise namespace

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
