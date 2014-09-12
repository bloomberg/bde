// bdldfp_intelimpwrapper.h                                           -*-C++-*-
#ifndef INCLUDED_BDLDFP_INTELIMPWRAPPER
#define INCLUDED_BDLDFP_INTELIMPWRAPPER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide a central mechanism to configure the Intel DFP library.
//
//@CLASSES:
//
//@SEE_ALSO: bdldfp_decimalimputil_inteldfp, bdldfp_decimalplatform
//
//@DESCRIPTION: This component provides a centralized mechanism to include the
// requesite Intel DFP headers, by issuing the appropriate '#define'
// configuration macros before including the intel headers.  This component is
// for internal use only by the 'bdldfp_decimal*' components.  Direct use of
// any names declared in this component by any other code invokes undefined
// behavior.  In other words: this code may change, disappear, break, move
// without notice, and no support whatsoever will ever be provided for it.
//
///Usage
///-----
// This section shows the intended use of this component.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#define INCLUDED_BDLDFP_DECIMALPLATFORM
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if !defined(BSLS_PLATFORM_OS_WINDOWS)

#  if !defined(INCLUDED_BID_FUNCTIONS) \
   && !defined(INCLUDED_BID_CONF) \
   && !defined(INCLUDED_BID_INTERNAL)

// Controlling macros for the intel library configuration

#    define DECIMAL_CALL_BY_REFERENCE      0
#    define DECIMAL_GLOBAL_ROUNDING        1
#    define DECIMAL_GLOBAL_EXCEPTION_FLAGS 1
#    ifndef LINUX
#      define LINUX
#      define BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_LINUX
#    endif

#    ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
#      define BID_BIG_ENDIAN 1
#    else
#      define BID_BIG_ENDIAN 0
#    endif

// The IBM compiler has trouble with the embedded '__thread' token, under some
// build modes.  We'll turn it off, for now.  We'll do so for Sun also, since
// it is disabled in the wrapper-package for that compiler as well.

#    if defined(BSLS_PLATFORM_CMP_IBM) || defined(BSLS_PLATFORM_CMP_SUN)
#      define __thread
#    endif

// in C++, there's always a 'wchar_t' type, so we need to tell Intel's library
// about this.

#    define _WCHAR_T_DEFINED

     extern "C" {
#     include <bid_conf.h>
#     include <bid_functions.h>
#     include <bid_internal.h>
     }

#    undef DECIMAL_CALL_BY_REFERENCE
#    undef DECIMAL_GLOBAL_ROUNDING
#    undef DECIMAL_GLOBAL_EXCEPTION_FLAGS
#    undef _WCHAR_T_DEFINED

#    ifdef BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_LINUX
#      undef LINUX
#      undef BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_LINUX
#    endif

#    define INCLUDED_BID_CONF
#    define INCLUDED_BID_FUNCTIONS
#    define INCLUDED_BID_INTERNAL
#  endif

#endif // if !defined(BSLS_PLATFORM_OS_WINDOWS)

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
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

