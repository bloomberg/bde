// bslfwd_buildtarget.h                                               -*-C++-*-
#ifndef INCLUDED_BSLFWD_BUILDTARGET
#define INCLUDED_BSLFWD_BUILDTARGET

//@PURPOSE: Supply a compile-time indicator of whether BSL uses C++ namespaces.
//
//@DEPRECATED: Do not use.
//
//@CLASSES:
//
//@MACROS:
// BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES: set if two-level namespace in 'bsl'
//
//@AUTHOR: Alexei Zakharov (azakharov7)
//
//@DESCRIPTION: This header potentially provides a single macro,
// 'BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES'.  If
// 'BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES' is defined then the types in the
// 'bsl' library are declared in C++ style namespaces (e.g.,
// 'bslma::Allocator').  If 'BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES' is not
// defined then the types in the 'bsl' library are declared using legacy-style
// namespaces (e.g., 'bslma_Allocator').

// Uncomment the following macro when switching to 'bsl' with namespaces to
// enable forward declarations provided by the 'bslfwd' package to be in the
// appropriate namespace.

#define BSLFWD_BUILDTARGET_BSL_USING_NAMESPACES

#endif

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
