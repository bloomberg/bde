// bsltf_degeneratefunctor.h                                          -*-C++-*-
#ifndef INCLUDED_BSLTF_DEGENERATEFUNCTOR
#define INCLUDED_BSLTF_DEGENERATEFUNCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumerated test type.
//
//@CLASSES:
//   bsltf::EnumeratedTestType: enumerated test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides ... (TBD)
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: (TBD)
/// - - - - - - - - - - - - - - - -
// First, we ...

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>  // 'swap', supplied by <utility> in C++11
#define INCLUDED_ALGORITHM
#endif

namespace BloombergLP {

namespace bsltf {

                       // =======================
                       // class DegenerateFunctor
                       // =======================

template <class FUNCTOR, bool ENABLE_SWAP = true>
class DegenerateFunctor : private FUNCTOR {
    // This test class template adapts a DefaultConstructible class to offer
    // a minimal or outright obstructive interface for testing generic code.
    // We expect to use this to supply Hasher and Comparator classes to test
    // 'HashTable', which must be CopyConstructible, Swappable, and nothrow
    // Destructible, and offer the (inherited) function call operator as their
    // public interface.  No other operation should be usable.  We take
    // advantage of the fact that defining a copy constructor inhibits the
    // generation of a default constructor, and that constructors are not
    // inherited by a derived class.

  private:
    // PRIVATE CREATORS
    explicit DegenerateFunctor(const FUNCTOR& base);
        // Create a 'DegenerateFunctor' wrapping a copy of the specified
        // 'base'.

  private:
    // NOT IMPLEMENTED
    DegenerateFunctor& operator=(const DegenerateFunctor&); // = delete;
        // Not implemented

    void operator&();  // = delete;
        // not implemented

    template<class T>
    void operator,(const T&); // = delete;
        // not implemented

    template<class T>
    void operator,(T&); // = delete;
        // not implemented

    template<class T>
    void swap(T&); // = delete;
        // Not implemented.  This method hides a frequently supplied member
        // function that may be sniffed out by clever template code when it
        // is declared in the base class.  When 'ENABLE_SWAP' is 'false', we
        // want to be sure that this class does not accidentally allow
        // swapping through an unexpected back door.  When 'ENABLE_SWAP' is
        // 'true', we provide a differently named hook, to minimize the chance
        // that a clever template library can sniff it out.

  public:
    static DegenerateFunctor cloneBaseObject(const FUNCTOR& base);
        // Create a DegenerateFunctor object wrapping a copy of the specified
        // 'base'.  Note that this method is supplied so that the only
        // publicly accessible constructor is the copy constructor.

    // CREATORS
    DegenerateFunctor(const DegenerateFunctor& original);
        // Create a 'DegenerateFunctor' having the same value the specified
        // 'original'.

    // MANIPULATORS
    using FUNCTOR::operator();
        // Expose the overloaded function call operator from the parameterizing
        // class 'FUNCTOR'.

    void exchangeValues(DegenerateFunctor *other);
        // Swap the wrapped 'FUNCTOR' object, using ADL with 'std::swap' in
        // the lookup set, with the functor wrapper by the specified '*other'.
        // Note that this function is deliberately *not* named 'swap' as some
        // "clever" template libraries may try to call a member-swap function
        // when they can find it, and ADL-swap is not available.  Also note
        // that this overload is needed only so that the ADL-enabling
        // free-function 'swap' can be defined, as the native std library
        // 'swap' function does will not accept this class on AIX or Visual C++
        // prior to VC2010.
};

template <class FUNCTOR>
void swap(DegenerateFunctor<FUNCTOR, true>& lhs,
          DegenerateFunctor<FUNCTOR, true>& rhs);
    // Exchange the values of the specified 'lhs' and 'rhs' objects.


// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                       // -----------------------
                       // class DegenerateFunctor
                       // -----------------------

// CREATORS
template <class FUNCTOR, bool ENABLE_SWAP>
inline
DegenerateFunctor<FUNCTOR, ENABLE_SWAP>::DegenerateFunctor(const FUNCTOR& base)
: FUNCTOR(base)
{
}

template <class FUNCTOR, bool ENABLE_SWAP>
inline
DegenerateFunctor<FUNCTOR, ENABLE_SWAP>::DegenerateFunctor(
                                             const DegenerateFunctor& original)
: FUNCTOR(original)
{
}

template <class FUNCTOR, bool ENABLE_SWAP>
inline
DegenerateFunctor<FUNCTOR, ENABLE_SWAP>
DegenerateFunctor<FUNCTOR, ENABLE_SWAP>::cloneBaseObject(const FUNCTOR& base)
{
    return DegenerateFunctor(base);
}

template <class FUNCTOR, bool ENABLE_SWAP>
inline
void
DegenerateFunctor<FUNCTOR, ENABLE_SWAP>::exchangeValues(
                                                      DegenerateFunctor *other)
{
    BSLS_ASSERT_SAFE(other);

    using std::swap;
    swap(static_cast<FUNCTOR&>(*this), static_cast<FUNCTOR&>(*other));
}

}  // close package namespace

template <class FUNCTOR>
inline
void bsltf::swap(DegenerateFunctor<FUNCTOR, true>& lhs,
                 DegenerateFunctor<FUNCTOR, true>& rhs)
{
    lhs.exchangeValues(bsls::Util::addressOf(rhs));
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
