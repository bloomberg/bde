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
//  bslalg::SwapUtil: namespace for the 'swap' utility function.
//
//@SEE_ALSO: bsl_algorithm
//
//@DESCRIPTION: This component provides a namespace for a utility function that
// swaps the value of two objects of the same type.  Using this utility is
// intended to be a simpler alternative to using the standard 'swap' algorithm
// directly.  The standard 'swap' algorithm is provided in the 'bsl' namespace
// in a generic form and overloaded for specific classes in the namespaces of
// those classes.  When the 'swap' algorithm is used, its specific
// implementation is supposed to be found by Argument Dependent Lookup (ADL).
// Finding the proper 'swap' function with ADL requires bringing the
// 'bsl::swap' into the current scope with the 'using' directive and then
// calling a 'swap' function without the namespace qualification.  The 'swap'
// utility static function provided by this component relieves the end-user
// from a need to remember those details of the proper usage of the 'swap'
// algorithm.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: using 'bslalg::SwapUtil::swap'
/// - - - - - - - - - - - - - - - - - - - -
// In this example we define a type 'Container' and use 'bslalg::SwapUtil' to
// both implement a user-defined 'swap' for 'Container', and swap two container
// objects.
//
// We start by defining a class 'Container' in the 'xyz' namespace.  Further we
// assume that 'Container' has some expensive-to-copy data, so we provide a
// custom 'swap' algorithm to efficiently swap the data between a two objects
// this class by defining a 'swap' method and a 'swap' free function.
//..
//  namespace xyz {
//
//  class Container {
//    private:
//      int d_expensiveData;
//
//    public:
//      void swap(Container& other);
//          // Swap the value of 'this' object with the value of the specified
//          // 'other' object.  This method provides the no-throw
//          // exception-safety guarantee.
//  };
//
//  void swap(Container& a, Container& b);
//      // Swap the values of the specified 'a' and 'b' objects.  This function
//      // provides the no-throw exception-safety guarantee.
//..
// Note that the free function 'swap' is overloaded in the namespace of the
// class 'Container', which is 'xyz'.
//
// Next, we implemented the 'swap' method using the 'bslalg::SwapUtil::swap' to
// swap the individual data elements:
//..
//  inline
//  void Container::swap(Container& other)
//  {
//      bslalg::SwapUtil::swap(&d_expensiveData, &other.d_expensiveData);
//
//      // Equivalent to:
//      // using bsl::swap;
//      // bsl::swap(d_expensiveData, other.d_expensiveData);
//  }
//..
// Notice that calling 'bslalg::SwapUtil::swap' is equivalent to making the
// 'bsl::swap' available in the current scope by doing 'using bsl::swap' and
// making a subsequent call to an unqualified 'swap' function.
//
// Then, we implement the 'swap' free function:
//..
//  inline
//  void swap(Container& a, Container& b)
//  {
//      a.swap(b);
//  }
//
//  }  // close namespace xyz
//..
// Finally we can use 'bslalg::SwapUtil::swap' to swap two objects of class
// 'xyz::Container':
//..
//  xyz::Container c1, c2;
//
//  bslalg::SwapUtil::swap(&c1, &c2);
//..
// The above code correctly calls the 'xyz::swap' overload for the 'Container'
// class.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

namespace BloombergLP {

// A workaround for GCC which before version 4.0 had some problems with ADL.
#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR < 40000

class bslalg_SwapUtil_Dummy;

void swap(bslalg_SwapUtil_Dummy);
    // Introduce 'swap' to the 'BloombergLP' namespace.  The dummy argument of
    // a private type prevents this declaration from interfering with anything
    // else.
#endif

namespace bslalg {

                           // ===============
                           // struct SwapUtil
                           // ===============

class SwapUtil {
    // This class provides a namespace for the 'swap' utility method.

  public:
    // CLASS METHODS
    template <typename T>
    static
    void swap(T *a, T *b);
        // Exchange the values of the specified 'a' and 'b' objects using
        // either a 'swap' free function overloaded for type 'T', in the
        // namespace of type 'T' if it's available, and the default generic
        // 'bsl::swap' otherwise.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                           // ---------------
                           // struct SwapUtil
                           // ---------------

// CLASS METHODS
template <typename T>
void SwapUtil::swap(T *a, T *b)
{
    BSLS_ASSERT_SAFE(a != NULL);
    BSLS_ASSERT_SAFE(b != NULL);

    using std::swap;

// A workaround for GCC which before version 4.0 had some problems with ADL.
#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VER_MAJOR < 40000
    using BloombergLP::swap;
#endif

    swap(*a, *b);
}

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
