// bsltf_degeneratefunctor.h                                          -*-C++-*-
#ifndef INCLUDED_BSLTF_DEGENERATEFUNCTOR
#define INCLUDED_BSLTF_DEGENERATEFUNCTOR

#include <bsls_ident.h>
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

#include <bslscm_version.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_util.h>

#include <new>

namespace BloombergLP {

namespace bsltf {

                       // =======================
                       // class DegenerateFunctor
                       // =======================

/// This test class template adapts a CopyConstructible class to offer
/// a minimal or outright obstructive interface for testing generic code.
/// To support the testing of standard containers, this adapter will be
/// MoveConstructible, CopyConstructible, and nothrow Destructible as long
/// as the adapted `FUNCTOR` satisfies the same requirements.  This class
/// will further be Swappable if (the template parameter) `ENABLE_SWAP` is
/// `true` and the adapted `FUNCTOR` is MoveConstructible.  The (inherited)
/// function call operator should be the only other available method, no
/// other operation (e.g., the unary address-of operator) should be usable.
/// We take advantage of the fact that defining a copy constructor inhibits
/// the generation of a default constructor, and that constructors are not
/// inherited by a derived class.  `DegenerateFunctor` objects must be
/// created through either the copy constructor, or by wrapping a `FUNCTOR`
/// object through the static factory method of this class,
/// `cloneBaseObject`.
template <class FUNCTOR, bool ENABLE_SWAP = true>
class DegenerateFunctor : private FUNCTOR {

  private:
    // PRIVATE CREATORS

    /// Create a `DegenerateFunctor` wrapping a copy of the specified
    /// `base`.
    explicit DegenerateFunctor(const FUNCTOR& base);

  private:
    // NOT IMPLEMENTED

    /// Not implemented
    DegenerateFunctor& operator=(const DegenerateFunctor&); // = delete;

    /// not implemented
    void operator&();  // = delete;

    /// not implemented
    template<class T>
    void operator,(const T&); // = delete;

    /// not implemented
    template<class T>
    void operator,(T&); // = delete;

    /// Not implemented.  This method hides a frequently supplied member
    /// function that may be sniffed out by clever template code when it
    /// is declared in the base class.  When `ENABLE_SWAP` is `false`, we
    /// want to be sure that this class does not accidentally allow
    /// swapping through an unexpected back door.  When `ENABLE_SWAP` is
    /// `true`, we provide a differently named hook, to minimize the chance
    /// that a clever template library can sniff it out.
    template<class T>
    void swap(T&); // = delete;

  public:
    /// Create a DegenerateFunctor object wrapping a copy of the specified
    /// `base`.  Note that this method is supplied so that the only
    /// publicly accessible constructor is the copy constructor.
    static DegenerateFunctor cloneBaseObject(const FUNCTOR& base);

    // CREATORS

    /// Create a `DegenerateFunctor` having the same value the specified
    /// `original`.
    DegenerateFunctor(const DegenerateFunctor& original);

    /// Create a `DegenerateFunctor` having the same value the specified
    /// `original, and leave `original' in an unspecified (but valid) state.
    DegenerateFunctor(bslmf::MovableRef<DegenerateFunctor> original);

    // MANIPULATORS

    /// Expose the overloaded function call operator from the parameterizing
    /// class `FUNCTOR`.
    using FUNCTOR::operator();

    /// Swap the wrapped `FUNCTOR` object, by move-constructing a temporary
    /// object from the specified `*other`, then alternately destroying and
    /// in-place move-constructing new values for each of `*other` and
    /// `*this`.  Note that this function is deliberately *not* named `swap`
    /// as some "clever" template libraries may try to call a member-swap
    /// function when they can find it, and ADL-swap is not available.  Also
    /// note that this overload is needed only so that the ADL-enabling
    /// free-function `swap` can be defined, as the native std library
    /// `std::swap` function will not accept this class (with its deliberate
    /// degenerate nature) on AIX, or on Windows with Visual C++ prior to
    /// VC2010.
    void exchangeValues(DegenerateFunctor *other);
};

/// Exchange the values of the specified `lhs` and `rhs` objects.
template <class FUNCTOR>
void swap(DegenerateFunctor<FUNCTOR, true>& lhs,
          DegenerateFunctor<FUNCTOR, true>& rhs);


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
DegenerateFunctor<FUNCTOR, ENABLE_SWAP>::DegenerateFunctor(
                                 bslmf::MovableRef<DegenerateFunctor> original)
: FUNCTOR(bslmf::MovableRefUtil::move(static_cast<FUNCTOR&>(original)))
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

    DegenerateFunctor temp(bslmf::MovableRefUtil::move(*other));
    other->~DegenerateFunctor();
    ::new((void *)other) DegenerateFunctor(bslmf::MovableRefUtil::move(*this));
    this->~DegenerateFunctor();
    ::new((void *)this) DegenerateFunctor(bslmf::MovableRefUtil::move(temp));
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
