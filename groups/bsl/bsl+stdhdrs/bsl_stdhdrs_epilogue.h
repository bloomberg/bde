// bsl_stdhdrs_epilogue.h                                             -*-C++-*-

//@PURPOSE: Provide final declarations for each C++ standard header file.
//
//@SEE_ALSO: bsl_stdhdrs_prologue, bsl_stdhdrs_epilogue_recursive
//
//@DESCRIPTION: This epilogue file is intended to be included at the end of
// each bsl-supplied C++ standard library header file in "BDE-STL" mode.  This
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
