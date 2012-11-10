// bsl_stdhdrs_prologue.h                                             -*-C++-*-

//@PURPOSE: Provide initial declarations for each C++ standard header file.
//
//@SEE_ALSO: bsl_stdhdrs_epilogue
//
//@DESCRIPTION: This prologue header is intended to be included at the start of
// each bsl-supplied C++ standard library header file.  The header ensures that
// 'std' is not defined as a macro while the header is being processed,
// including in nested headers.  It also defines standard namespaces and the
// relationships between them.  The inclusion of this file at the start of a
// header must be paired with the inclusion of 'bsl_stdhdrs_epilogue.h' at the
// end of the same header.  See 'bsl+stdhdrs.txt' for an overview of the
// prologue / epilogue system.
//
// Note that only part of this file is protected by include guards.  THIS FILE
// IS INTENDED FOR MULTIPLE INCLUSION.

#ifndef BSL_OVERRIDES_STD
# error "Prologue shouldn't be included in native stl mode"
#endif

#ifdef BSL_STDHDRS_PROLOGUE_IN_EFFECT
# error "Prologue was included while still in effect"
#endif

#define BSL_STDHDRS_PROLOGUE_IN_EFFECT

#ifdef BSL_STDHDRS_DEFINED_STD_MACRO
# undef std
# undef BSL_STDHDRS_DEFINED_STD_MACRO
#elif defined(std)
# error "A macro for 'std' should not be '#define'd outside of 'bsl+stdhdrs'."
#endif

// The rest of this file is intended to be included only once and is thus
// protected by include guards.
#ifndef INCLUDED_BSL_STDHDRS_PROLOGUE
#define INCLUDED_BSL_STDHDRS_PROLOGUE

#ifndef INCLUDED_BSLS_IDENT
# include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLS_NATIVESTD
# include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
# include <bsls_platform.h>
#endif

// When a standard C header file (e.g., 'stddef.h') is included within an
// 'extern "C"' block, this file will be included transitively.  To avoid
// defining C++ symbols in an 'extern "C"' block, the C++ portion of this
// header is wrapped around an 'extern "C++"' block.
extern "C++" {

namespace std
{
    namespace rel_ops { }
}

namespace bsl
{
    // Only 'rel_ops' is alised and imported into the 'bsl' namespace here.
    // Other 'std' symbols are imported by files in the 'bsl+bslhdrs' package.
    namespace rel_ops = std::rel_ops;
}

}  // extern "C++"


#if defined(BSLS_PLATFORM_CMP_SUN)
  // Force the SUN compiler to find *.cc files when including native headers.
  // Without this macro, .cc files are included lazily, which is usually AFTER
  // 'std' has been defined as a macro, breaking their compilation.
# define _RWSTD_COMPILE_INSTANTIATE 1
#endif

#endif // INCLUDED_BSL_STDHDRS_PROLOGUE

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
