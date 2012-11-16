/* stdarg.h                                                          -*-C-*- */
#ifndef INCLUDED_NATIVE_C_STDARG
#define INCLUDED_NATIVE_C_STDARG

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

/*
//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@DESCRIPTION: Provide functionality of the corresponding C++ standard header.
// This file includes the compiler provided native standard header.  In
// addition, in 'bde-stl' mode (used by Bloomberg managed code, see
// 'bsl+stdhdrs.txt' for more information) include the corresponding header in
// 'bsl+bslhdrs' as well as 'bsl_stdhdrs_prologue.h' and
// 'bsl_stdhdrs_epilogue.h'.  This includes the respective 'bsl' types and
// places them in the 'std' namespace.
*/

/*
// Note that 'stdarg.h' is meant for multiple inclusion on Linux, so only the
// ident is protected by the include guard.
*/

#endif  /* INCLUDED_NATIVE_C_STDARG */

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#if !defined(BSL_OVERRIDES_STD) || !defined(__cplusplus)

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
#     include_next <stdarg.h>
#   else
#     include BSL_NATIVE_C_LIB_HEADER(stdarg.h)
#   endif

#else  /* defined(BSL_OVERRIDES_STD) */

#   ifndef BSL_STDHDRS_PROLOGUE_IN_EFFECT
#   include <bsl_stdhdrs_prologue.h>
#   endif

#   ifndef BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_RUN_EPILOGUE
#   define BSL_STDHDRS_EPILOGUE_RUN_BY_c_stdarg
#   endif

#   ifndef INCLUDED_BSL_STDHDRS_INCPATHS
#   include <bsl_stdhdrs_incpaths.h>
#   endif

#   if defined(BSLS_COMPILERFEATURES_SUPPORT_INCLUDE_NEXT)
#     include_next <stdarg.h>
#   else
#     include BSL_NATIVE_C_LIB_HEADER(stdarg.h)
#   endif

// This native header does not define any symbols in namespace 'std' to import,
// so the following include is not necessary:
// #include <bsl_c_stdarg.h>

#   ifdef BSL_STDHDRS_EPILOGUE_RUN_BY_c_stdarg
#   undef BSL_STDHDRS_EPILOGUE_RUN_BY_c_stdarg
#   include <bsl_stdhdrs_epilogue.h>
#   endif

#endif  /* BSL_OVERRIDES_STD */

/*
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
*/
