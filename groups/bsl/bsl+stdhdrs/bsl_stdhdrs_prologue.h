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
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

// When a standard C header file (e.g., 'stddef.h') is included within an
// 'extern "C"' block, this file will be included transitively.  To avoid
// defining C++ symbols in an 'extern "C"' block, the C++ portion of this
// header is wrapped around an 'extern "C++"' block.
extern "C++" {

namespace std {
    namespace rel_ops { }
}  // close namespace std

namespace bsl {
    // Only 'rel_ops' is aliased and imported into the 'bsl' namespace here.
    // Other 'std' symbols are imported by files in the 'bsl+bslhdrs' package.
    namespace rel_ops = std::rel_ops;
}  // close package namespace

}  // extern "C++"


#if defined(BSLS_PLATFORM_CMP_SUN)
  // Force the SUN compiler to find *.cc files when including native headers.
  // Without this macro, .cc files are included lazily, which is usually AFTER
  // 'std' has been defined as a macro, breaking their compilation.
# define _RWSTD_COMPILE_INSTANTIATE 1
#endif

#endif // INCLUDED_BSL_STDHDRS_PROLOGUE

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
