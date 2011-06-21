// bslalg_swaputil.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_SWAPUTIL
#define INCLUDED_BSLALG_SWAPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a simple to use 'swap' algorithm.
//
//@CLASSES:
//  bslalg_SwapUtil: namespace for the 'swap' utility function.
//
//@SEE_ALSO: bsl_algorithm.h
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides an utility to swap two objects, which
// is intended to be a simpler alternative to the standard 'swap' algorithm.
// The standard 'swap' algorithm is provided in the 'bsl' namespace in a
// generic form and overloaded for specific classes in the namespaces of those
// classes.  When the 'swap' algorithm is used, its specific implementation is
// supposed to be found by Argument Dependent Lookup (ADL).  Finding the proper
// 'swap' function with ADL requires bringing the 'bsl::swap' into the current
// scope with the 'using' directive and then calling a 'swap' function without
// the namespace qualification.  The 'swap' utility static function provided by
// this component relieves the end-user from a need to remember those details
// of the proper usage of the 'swap' algorithm.
//
///Usage
///-----
///Example 1: using 'bslalg_SwapUtil::swap'
/// - - - - - - - - - - - - - - - - - - - -
// In this section we show the intended usage of this component.  We start by
// defining a class 'Container' in the 'xyz' namespace.  Further we assume that 
// 'Container' has some expensive-to-copy data, so we provide a custom 'swap'
// algorithm to efficiently swap the data between a two objects this class by
// defining a 'swap' method and a 'swap' free function.
//..
//  namespace xyz {
//
//  class Container {
//    private:
//      int d_expensiveData;
//
//    public:
//      void swap(Container& other);
//  };
//
//  void swap(Container& a, Container& b);
//
//  }
//..
// Note that the free function 'swap' is overloaded in the namespace of the
// class 'Container', which is 'xyz'.
//
// Next, we implemente the 'swap' method using the 'bslalg_SwapUtil::swap' to
// swap the individual data elements:
//..
//  inline
//  void xyz::Container::swap(Container& other)
//  {
//      bslalg_SwapUtil::swap(&d_expensiveData, &other.d_expensiveData);
//  }
//..
// Note that calling 'bslalg_SwapUtil::swap' is equivalent to making the
// 'bsl::swap' available in the current scope by doing 'using bsl::swap' and
// making a subsequent call to an unqualified 'swap' function.
//
// Then, we implement the 'swap' free function:
//..
//  inline
//  void xyz::swap(Container& a, Container& b)
//  {
//      a.swap(b);
//  }
//..
// Finally we can use 'bslalg_SwapUtil::swap' to swap two objects of class
// 'xyz::Container':
//..
//  xyz::Container c1, c2;
//
//  bslalg_SwapUtil::swap(&c1, &c2);
//..
// The above code correctly calls the 'xyz::swap' overload for the 'Container'
// class.

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_ALGORITHM
#define INCLUDED_ALGORITHM
#include <algorithm>
#endif

namespace BloombergLP {

                           // ======================
                           // struct bslalg_SwapUtil
                           // ======================

class bslalg_SwapUtil {
    // This class provides a namespace for the 'swap' utility method.

  public:
    // CLASS METHODS
    template <typename T>
    static
    void swap(T *a, T *b);
        // Exchange the values in the specified 'a' and 'b' using either a
        // 'swap' free function overloaded for type 'T' in the namespace of
        // type 'T' if it's available and the default generic 'bsl::swap'
        // otherwise.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                           // ----------------------
                           // struct bslalg_SwapUtil
                           // ----------------------

// CLASS METHODS
template <typename T>
void bslalg_SwapUtil::swap(T *a, T *b)
{
    BSLS_ASSERT_SAFE(a != NULL);
    BSLS_ASSERT_SAFE(b != NULL);

    using std::swap;
    swap(*a, *b);
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
