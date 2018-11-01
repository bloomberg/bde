// bslmf_util.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLMF_UTIL
#define INCLUDED_BSLMF_UTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide low-level functions on 'bslmf' types.
//
//@CLASSES:
//  bslmf::Util: utility class providing low-level functionality
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a utility 'struct', 'bslmf::Util', that
// serves as a namespace for a suite of functions that supply low-level
// functionality for implementing portable generic facilities such as might be
// found in the C++ standard library.
//
// TBD: put in a note that we only put this in because we needed a way to
//      distinguish movable ref vs anything else for C++03
//      otherwise identical to std::forawrd
//
///Usage
///-----
// TBD...

#include <bslscm_version.h>

#include <bslmf_addlvaluereference.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_movableref.h>
#include <bslmf_removereference.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace BloombergLP {

namespace bslmf {

                      // ===========
                      // struct Util
                      // ===========

struct Util {
    // This struct provides several functions that are specified in the
    // <utility> header of the C++ Standard, in order to support the 'bsl'
    // library implementation without cycles into the native standard library,
    // and on platforms with only C++03 compilers available, where library
    // features may be emulated.

    // CLASS METHODS

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    template <class TYPE>
    BSLS_KEYWORD_CONSTEXPR
    static TYPE&& forward(typename bsl::remove_reference<TYPE>::type&  t)
                                                         BSLS_KEYWORD_NOEXCEPT;
    template <class TYPE>
    BSLS_KEYWORD_CONSTEXPR
    static TYPE&& forward(typename bsl::remove_reference<TYPE>::type&& t)
                                                         BSLS_KEYWORD_NOEXCEPT;
#else
    template <class TYPE>
    BSLS_KEYWORD_CONSTEXPR
    static const TYPE& forward(const TYPE& t) BSLS_KEYWORD_NOEXCEPT;
    template <class TYPE>
    BSLS_KEYWORD_CONSTEXPR
    static MovableRef<TYPE> forward(MovableRef<TYPE> t) BSLS_KEYWORD_NOEXCEPT;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Correctly forward the specified 't' argument based on the current
        // compilation environment.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    template <class TYPE>
    BSLS_KEYWORD_CONSTEXPR
    static typename bsl::add_rvalue_reference<TYPE>::type declval()
                                                         BSLS_KEYWORD_NOEXCEPT;
#else
    template <class TYPE>
    BSLS_KEYWORD_CONSTEXPR
    static typename bsl::add_lvalue_reference<TYPE>::type declval()
                                                         BSLS_KEYWORD_NOEXCEPT;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // This function has no implementation.  It exists to allow for the
        // appearance of a temporary object of the specified type that can be
        // used in unevaluated contexts.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CLASS METHODS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
inline
TYPE&& Util::forward(typename bsl::remove_reference<TYPE>::type& t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return static_cast<TYPE&&>(t);
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
inline
TYPE&& Util::forward(typename bsl::remove_reference<TYPE>::type&& t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return static_cast<TYPE&&>(t);
}

#else

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
inline
const TYPE& Util::forward(const TYPE& t) BSLS_KEYWORD_NOEXCEPT
{
    return t;
}

template <class TYPE>
BSLS_KEYWORD_CONSTEXPR
inline
bslmf::MovableRef<TYPE> Util::forward(bslmf::MovableRef<TYPE> t)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return t;
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
