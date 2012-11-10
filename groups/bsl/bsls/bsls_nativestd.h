// bsls_nativestd.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLS_NATIVESTD
#define INCLUDED_BSLS_NATIVESTD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Define the namespace 'native_std' as an alias for '::std'.
//
//@CLASSES:
//
//@SEE_ALSO: bsl_stdhdrs_prologue
//
//@DESCRIPTION: This component defines a namespace alias, 'native_std', that
// serves as an alias for the 'std' namespace.  The 'bsl' package group
// contains a Bloomberg implementation of the C++ standard library, which, when
// compiled in bde-stl mode, will override the STL implementation in namespace
// 'std'.  With the 'native_std' alias, users can refer to the STL types from
// the compiler's native library, even if the 'std' is configured to refer to
// Bloomberg provided STL types.  The 'native_std' namespace remains in effect
// even if 'std' is redefined as a preprocessor macro.  Note that this
// component is mainly provided for code being compiled in bde-stl mode.
//
// Note: It is safe to '#include' this header only if 'std' is not yet defined
// as a macro or if 'BSL_DEFINED_NATIVE_STD' is already defined.  Any other
// header that defines the 'native_std' namespace should also define the
// 'BSL_DEFINED_NATIVE_STD' macro.  For more info on bde-stl mode, please see
// 'bsl+stdhdrs/doc/bsl+stdhdrs.txt'.
//
///Usage
///-----
// In the following example we use the 'native_std' prefix to access the native
// implementation of 'std::string', even when the compilation is configured to
// override the use of native STL types with Bloomberg provided STL types
// (i.e., bde-stl mode, used by Bloomberg managed code):
//..
//  #include <bsls_nativestd.h>
//  #include <string>
//
//  std::string        s1;  // might be 'bsl::string'
//  native_std::string s2;  // guaranteed to be the compiler's native string
//..

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

// '<bsls_platform.h>' is included early in the header sequence, as it contains
// several macros that control the configuration of the native STL on the
// Windows platform, if no other.  The main reason is to disable Microsoft's
// deprecation warnings, as they deprecated the entire standard library unless
// certain configuration macros are defined.  We put those macros into our
// common platform header, rather than require the user to supply lots of
// options to every project they create in Visual Studio in order to use BDE.

#ifndef BSL_DEFINED_NATIVE_STD

# ifdef std
#   error "Cannot include bsls_nativestd.h when 'std' is a macro."
# endif

namespace std { }
namespace native_std = ::std;

#endif  // ! defined(BSL_DEFINED_NATIVE_STD)

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
