// bsltf_degeneratefunctor.h                                          -*-C++-*-
#ifndef INCLUDED_BSLTF_DEGENERATEFUNCTOR
#define INCLUDED_BSLTF_DEGENERATEFUNCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an awkward type to adapt a user-supplied functor.
//
//@CLASSES:
//  bsltf::DegenerateFunctor: awkward type that adapts a user-supplied functor
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a functor adaptor, primarily for use
// when testing templates that make use of Callable template parameters.  The
// adaptor defined in this component provides an interface that is purposefully
// as awkward as possible, yet should accepted by generic code, particularly
// code conforming to the widest interpretation of the C++ standard library.
// Many common operations that would be implicitly supplied, such as the
// address-of operator and the comma operator, are explicitly disabled.  While
// the adapter remains CopyConstructible so that it may be used as a member of
// a class, such as a standard container, it is not CopyAssignable, and so
// typically is not Swappable.  An additional boolean template argument
// optionally creates an adapter that supports swapping.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: TBD
/// - - - - - - -

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP {

namespace bsltf {

                       // =======================
                       // class DegenerateFunctor
                       // =======================

template <class FUNCTOR, bool ENABLE_SWAP = true>
class DegenerateFunctor : private FUNCTOR {
    // This test class template adapts a CopyConstructible class to offer
    // a minimal or outright obstructive interface for testing generic code.
    // To support the testing of standard containers this adapter will be
    // CopyConstructible, nothrow Destructible, and (optionally) Swappable as
    // long as the adapted 'FUNCTOR' satisfies the same requirements.  The
    // (inherited) function call operator should be the only other available
    // method, no other operation (e.g., the unary address-of operator) should
    // be usable.  We take advantage of the fact that defining a copy
    // constructor inhibits the generation of a default constructor, and that
    // constructors are not inherited by a derived class.  'DegenerateFunctor'
    // objects must be created through either the copy constructor, or by
    // wrapping a 'FUNCTOR' object through the static factory method of this
    // class, 'cloneBaseObject'.

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


// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

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

    bslalg::SwapUtil::swap(static_cast<FUNCTOR *>(this),
                           static_cast<FUNCTOR *>(other) );
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
