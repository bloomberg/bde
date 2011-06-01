// bslalg_swaputil.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_SWAPUTIL
#define INCLUDED_BSLALG_SWAPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a simpler usage for the 'swap' algorithm.
//
//@CLASSES:
// bslalg_SwapUtil: namespace for the 'swap' utility function which purpose is
// to simplify the use of ADL-sensitive 'swap' algorithm.
//
//@SEE_ALSO: bsl_algorithm.h
//
//@AUTHOR: Alexei Zakharov (azakhar1)
//
//@DESCRIPTION: This component provides an utility to simplify the usage of the
// 'swap' algorithm.  The Standard 'swap' algorithm provided in the 'bsl'
// namespace in a generic form.  It can and is overloaded in other namespaces
// for specific classes in order to provide specialized implementation of the
// 'swap' algorithm for those classes.  Consequently, when the 'swap'
// algorithm is used, its specific implementation is supposed to be found by
// ADL.  Being dependent on ADL, the 'swap' algorithm is somewhat confusing to
// use.  The proper usage requires bringing the 'bsl::swap' into the current
// scope with the using directive and then calling an unqualified 'swap'.  The
// static 'swap' function in this component relieves the end user from a need
// to remember the details of the proper 'swap' usage.
//
///Usage
///-----
// Let's suppose we have a class 'Container' in a namespace 'xyz' which stores
// some expensive to copy data.  It implements a custom version of the 'swap'
// algorithm to efficiently swap the data between a two objects this class.
//..
// namespace xyz {
//     class Container {
//       private:
//         int d_expensiveData;
//
//       public:
//         void swap(Container& other);
//     };
//
//     void swap(Container& a, Container& b);
// }
//..
// Note that the free function 'swap' is overloaded in the namespace of the
// class 'Container', which is 'xyz'.
//
// The implementation of 'swap' method and free function look like this:
//..
// inline
// void xyz::Container::swap(Container& other)
// {
//     // either do this:
//     // using bsl::swap;
//     // swap(d_expensiveData, other.d_expensiveData);
//
//     // or this:
//     bslalg_SwapUtil::swap(&d_expensiveData, &other.d_expensiveData);
// }
//
// inline
// void xyz::swap(Container& a, Container& b)
// {
//     a.swap(b);
// }
//..
// Calling 'bslalg_SwapUtil::swap' is exactly equivalent to doing
// 'using bsl::swap' and then calling an unqualified 'swap'.
//
// Now, if we have two objects of type 'Container':
//..
// xyz::Container c1, c2;
//..
// and we want to swap them, we can just use the 'bslalg_SwapUtil' and not care
// about the details of the 'bsl::swap' usage.
//..
// bslalg_SwapUtil::swap(&c1, &c2);
//..
// The above code will correctly call 'xyz::swap' overload for the 'Container'
// class.

#ifndef INCLUDED_BSL_ALGORITHM
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
        // 'swap' overloaded for type 'T' if it's available or the default
        // generic 'bsl::swap'.
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
