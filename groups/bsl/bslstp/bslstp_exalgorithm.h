// bslstp_exalgorithm.h                                               -*-C++-*-
#ifndef INCLUDED_BSLSTP_EXALGORITHM
#define INCLUDED_BSLSTP_EXALGORITHM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for algorithm extensions.
//
//@INTERNAL_DEPRECATED: Do not use directly.
//
//@CLASSES:
//  bslstp::exalgorithm::copy_n: copy 'n' elements
//
//@SEE_ALSO: bsl_algorithm
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_algorithm>' instead.  This component provides a namespace for
// algorithm extensions that are not specified in the C++ standard.  This
// provides backward compatibility to code using extensions that STLPort
// provides.
//
// Note that the functions in this component are based on STLPort's
// implementation, with copyright notice as follows:
//..
//-----------------------------------------------------------------------------
// Copyright (c) 1994
// Hewlett-Packard Company
//
// Copyright (c) 1996,1997
// Silicon Graphics Computer Systems, Inc.
//
// Copyright (c) 1997
// Moscow Center for SPARC Technology
//
// Copyright (c) 1999
// Boris Fomitchev
//
// This material is provided "as is", with absolutely no warranty expressed
// or implied.  Any use is at your own risk.
//
// Permission to use or copy this software for any purpose is hereby granted
// without fee, provided the above notices are retained on all copies.
// Permission to modify the code and to distribute modified code is granted,
// provided the above notices are retained, and a notice that the code was
// modified is included with the above copyright notice.
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----
// This component is for internal use only.

#ifdef BDE_OPENSOURCE_PUBLICATION // STP
#error "bslstp_exalgorithm is not for publication"
#endif

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_algorithm.h> instead of <bslstp_exalgorithm.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>  // iterator tags
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>          // 'copy'
#define INCLUDED_ALGORITHM
#endif

namespace bsl {

// PRIVATE FREE FUNCTIONS
template <class InputIter, class Size, class OutputIter>
pair<InputIter, OutputIter>
bslstp_ExAlgorithm_CopyNImp(InputIter  first,
                            Size       count,
                            OutputIter result,
                            const      input_iterator_tag&)
{
    for ( ; count > 0; --count) {
        *result = *first;
        ++first;
        ++result;
    }
    return pair<InputIter, OutputIter>(first, result);
}

template <class RAIter, class Size, class OutputIter>
inline
pair<RAIter, OutputIter>
bslstp_ExAlgorithm_CopyNImp(RAIter     first,
                            Size       count,
                            OutputIter result,
                            const      random_access_iterator_tag&)
{
    RAIter last = first + count;
    return pair<RAIter, OutputIter>(last,
                                    native_std::copy(first, last, result));
}

// FREE FUNCTIONS
template <class InputIter, class Size, class OutputIter>
inline
pair<InputIter, OutputIter> copy_n(InputIter  first,
                                   Size       count,
                                   OutputIter result)
    // Copy the specified 'count' elements from the specified 'first' address
    // to the specified 'result' address.
{
    typedef typename iterator_traits<InputIter>::iterator_category tag;
    return bslstp_ExAlgorithm_CopyNImp(first, count, result, tag());
}

}  // close namespace bsl

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
