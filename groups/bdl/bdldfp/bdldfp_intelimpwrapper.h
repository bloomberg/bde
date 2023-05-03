// bdldfp_intelimpwrapper.h                                           -*-C++-*-
#ifndef INCLUDED_BDLDFP_INTELIMPWRAPPER
#define INCLUDED_BDLDFP_INTELIMPWRAPPER

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide a central mechanism to configure the Intel DFP library.
//
//@CLASSES:
//
//@SEE_ALSO: bdldfp_decimalimputil_inteldfp, bdldfp_decimalplatform
//
//@DESCRIPTION: This component provides a centralized mechanism to include the
// requisite Intel DFP headers, by issuing the appropriate '#define'
// configuration macros before including the Intel headers.  This component is
// for internal use only by the 'bdldfp_decimal*' components.  Direct use of
// any names declared in this component by any other code invokes undefined
// behavior.  In other words: this code may change, disappear, break, move
// without notice, and no support whatsoever will ever be provided for it.
//
///Usage
///-----
// This section shows the intended use of this component.

#include <bdlscm_version.h>

#include <bdldfp_decimalplatform.h>

#include <bsls_platform.h>

#include <bsl_cstddef.h>

#if defined(BDLDFP_DECIMALPLATFORM_INTELDFP)

#  if !defined(INCLUDED_BID_FUNCTIONS) \
   && !defined(INCLUDED_BID_CONF) \
   && !defined(INCLUDED_BID_INTERNAL)

// Controlling macros for the Intel library configuration

#    define DECIMAL_CALL_BY_REFERENCE      0
#    define DECIMAL_GLOBAL_ROUNDING        1
#    define DECIMAL_GLOBAL_EXCEPTION_FLAGS 0
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
// it is disabled in the wrapper-package for that compiler as well.  Also, to
// prevent redefinition of the 'fexcept_t' type, we signal that we behave like
// '__QNX__' so that the Intel library includes '<fenv.h>'.  As a result, if
// we're not using GCC or Clang on Linux, we pretend to be "QNX", since the
// Intel library has the right options chosen for that.
//
// 20201110 - hrosen4 - We should revisit this BID_THREAD logic eventually.
// Our production Sun compiler now supports '__thread', and IBM claims to
// support it when thread-local storage is enabled by option (which our build
// systems do not).  The Intel DFP library uses thread-local storage, when
// available, for global variables related to rounding mode and such, and the
// 'bdldfp' library does not modify or provide access to those, so this is not
// urgent.

#    if !(defined(BSLS_PLATFORM_OS_LINUX) && defined(BSLS_PLATFORM_CMP_GNU))  \
     && !(defined(BSLS_PLATFORM_OS_LINUX) && defined(BSLS_PLATFORM_CMP_CLANG))\
     && !defined(BSLS_PLATFORM_OS_WINDOWS)
// Prevent the intel dfp library from using thread-local storage
#      undef  BID_THREAD
#      define BID_THREAD
#      define __QNX__
#      define BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_QNX
#    endif

// In C++, there's always a 'wchar_t' type, so we need to tell Intel's library
// about this.

#    ifndef _WCHAR_T_DEFINED
#      define _WCHAR_T_DEFINED
#    endif

#    ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#      pragma GCC diagnostic push
#      pragma GCC diagnostic ignored "-Wconversion"
#      pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#    endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// calcrt2.h exports a SWAP macro which we want to preserve.

#ifdef SWAP
    #if SWAP == 99
        #define BDLDFP_INTELIMPWRAPPER_SWAP_WAS_99 1
    #endif
#endif

#endif // BDE_OMIT_INTERNAL_DEPRECATED

// bid_internal.h exports a SWAP macro.
#undef SWAP

extern "C" {
    #include <inteldfp/bid_conf.h>
    #include <inteldfp/bid_functions.h>
    #include <inteldfp/bid_internal.h>
}

#undef SWAP

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// Restore SWAP macro from calcrt2.h.

#if BDLDFP_INTELIMPWRAPPER_SWAP_WAS_99
    #define SWAP 99
    #undef BDLDFP_INTELIMPWRAPPER_SWAP_WAS_99
#endif

#endif // BDE_OMIT_INTERNAL_DEPRECATED

#    ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#      pragma GCC diagnostic pop
#    endif

// Intel #define's several symbols we don't want to leak out.

#    undef P7
#    undef P16
#    undef P34

#    undef DECIMAL_CALL_BY_REFERENCE
#    undef DECIMAL_GLOBAL_ROUNDING
#    undef DECIMAL_GLOBAL_EXCEPTION_FLAGS

#    ifdef BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_LINUX
#      undef LINUX
#      undef BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_LINUX
#    endif

#    ifdef BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_QNX
#      undef __thread
#      undef __QNX__
#      undef BDLDFP_INTELIMPWRAPPER_FAKE_DEFINE_QNX
#    endif

#    define INCLUDED_BID_CONF
#    define INCLUDED_BID_FUNCTIONS
#    define INCLUDED_BID_INTERNAL
#  endif

#endif // if defined(BDLDFP_DECIMALPLATFORM_INTELDFP)

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
