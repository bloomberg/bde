// bsl_stdhdrs_epilogue.h                                             -*-C++-*-

//@PURPOSE: Provide initial declarations for each C++ standard header file.
//
//@SEE_ALSO: bsl_stdhdrs_prologue, bsl_stdhdrs_epilogue_recursive
//
//@DESCRIPTION: This epilogue file is intended to be included at the end of
// each BSL-supplied C++ standard library header file in "BDE-STL" mode.  This
// header '#include's bsl versions of some standard components, overriding
// those symbols included from the native standard library.  This epilogue also
// defines 'std' as a macro that expands to 'bsl', which contains a blend of
// symbols from the native standard library and Bloomberg defined container
// types.  The inclusion of this file at the end of a 'bsl+stdhdrs' header must
// be paired with the inclusion of 'bsl_stdhdrs_prologue.h' at the start of the
// same header.
//
// Note that there are no include guards.  THIS FILE IS INTENDED FOR MULTIPLE
// INCLUSION.

#ifndef BSL_OVERRIDES_STD
#error "Epilogue should not be included in native STL mode"
#endif

// Include corresponding 'bslstp' and 'bslstl' headers for each native STL
// header.  'bsl_stdhdrs_epilogue_recursive.h' is the implementation of the
// epilogue that gets included recursively until all native STL headers are
// matched with their corresponding 'bslstp' and 'bslstl' headers.
#define USE_BSL_STDHDRS_EPILOGUE_RECURSIVE
#include <bsl_stdhdrs_epilogue_recursive.h>
#undef USE_BSL_STDHDRS_EPILOGUE_RECURSIVE

// Defining the 'std' macro allows Bloomberg-managed code (Robo) to use 'std::'
// instead of 'bsl::' for Bloomberg's implementation of the STL containers.
#ifdef std
# error "A macro for 'std' should not be '#define'd outside of 'bsl+stdhdrs'."
#elif defined(BSL_OVERRIDES_STD)
# define std bsl
# define BSL_STDHDRS_DEFINED_STD_MACRO
#endif

#undef BSL_STDHDRS_PROLOGUE_IN_EFFECT
#undef BSL_STDHDRS_RUN_EPILOGUE

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
