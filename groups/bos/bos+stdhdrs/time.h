/* time.h                                                            -*-C-*- */
#ifndef INCLUDED_NATIVE_C_TIME
#define INCLUDED_NATIVE_C_TIME

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

/*
//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bos+stdhdrs
//
//@DESCRIPTION: Provide functionality of the corresponding C++ standard header.
// This file includes the compiler provided native standard header.  In
// addition, in 'bde-stl' mode (used by Bloomberg managed code, see
// 'bos+stdhdrs.txt' for more information) include the corresponding header in
// 'bsl+bslhdrs' as well as 'bos_stdhdrs_prologue.h' and
// 'bos_stdhdrs_epilogue.h'.  This includes the respective 'bsl' types and
// places them in the 'std' namespace.
*/

/*
// Note that 'time.h' is meant for multiple inclusion on some platforms, so
// only the ident is protected by the include guard.
*/

#endif  /* INCLUDED_NATIVE_C_TIME */

#include <bsls_compilerfeatures.h>

#if !defined(BSL_OVERRIDES_STD) || !defined(__cplusplus) \
    || defined(BOS_STDHDRS_PROLOGUE_IN_EFFECT)
        // Don't run the prologue/epilogue pair if we're being included when
        // the prologue is already in effect.  It can happen because 'time.h'
        // is designed for multiple inclusion.
#   include <bos_stdhdrs_incpaths.h>

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
#     include_next <time.h>
#   else
#     include BSL_NATIVE_OS_RTL_HEADER(time.h)
#   endif

#else  /* defined(BSL_OVERRIDES_STD) */

#   ifndef BOS_STDHDRS_PROLOGUE_IN_EFFECT
#   include <bos_stdhdrs_prologue.h>
#   endif

#   ifndef BOS_STDHDRS_RUN_EPILOGUE
#   define BOS_STDHDRS_RUN_EPILOGUE
#   define BOS_STDHDRS_EPILOGUE_RUN_BY_c_time
#   endif
#   include <bos_stdhdrs_incpaths.h>

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
#     include_next <time.h>
#   else
#     include BSL_NATIVE_OS_RTL_HEADER(time.h)
#   endif

// This native header does not define any symbols in namespace 'std' to import,
// so the following include is not necessary:
// #include <bsl_c_time.h>

#   ifdef BOS_STDHDRS_EPILOGUE_RUN_BY_c_time
#   undef BOS_STDHDRS_EPILOGUE_RUN_BY_c_time
#   include <bos_stdhdrs_epilogue.h>
#   endif

#endif  /* BSL_OVERRIDES_STD */

/*
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
*/
