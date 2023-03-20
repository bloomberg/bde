/* bsl_c_stddef.h                                                    -*-C-*- */
#ifndef INCLUDED_BSL_C_STDDEF
#define INCLUDED_BSL_C_STDDEF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

/*
//@PURPOSE: Provide functionality of the corresponding C Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.
*/

#include <stddef.h>

#ifdef __cplusplus
#include <bsls_platform.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#if defined(BSLS_PLATFORM_CMP_SUN)                                            \
 && (!defined(__SUNPRO_CC_COMPAT) || __SUNPRO_CC_COMPAT  != 'G')

// On SUN since compiler version 5.13 we may operate in GNU compatibility mode,
// which uses the GNU library, and so the SUN workarounds aren't necessary.  We
// do *not* include '<bsls_libraryfeatures.h>' here for a reason (we could use
// the standard library detection mechanism from there to avoid using compiler
// specific macros here).  '<bsls_libraryfeatures.h>' has to '#include' C++
// standard library headers to determine which library is actually used.  Now
// we really, really do not want '<bsl_c_stddef.h>' to '#include <valarray>'.
// Using compiler specific macros here is safe.  The first element of the
// '#if' condition limits evaluation of the rest to the SUN compiler only, and
// these headers inherently depend on many platform specific implementation
// details anyway.  Case in point is the code below.

// It is valid to include a C header inside an 'extern "C"' block, so C++ code
// within C files should be wrapped around an 'extern "C++"' block.
extern "C++" {

// On Sun, 'std::size_t' is defined in 'stddef.h'.
namespace bsl {
    // Import selected symbols into bsl namespace
    using std::size_t;
}

}

#endif  // BSLS_PLATFORM_CMP_SUN

#endif  // __cplusplus

#endif

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
