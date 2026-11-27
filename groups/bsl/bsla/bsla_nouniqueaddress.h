// bsla_nouniqueaddress.h                                             -*-C++-*-
#ifndef INCLUDED_BSLA_NOUNIQUEADDRESS
#define INCLUDED_BSLA_NOUNIQUEADDRESS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to allow non-static data members to overlap.
//
//@MACROS:
//  BSLA_NO_UNIQUE_ADDRESS: data member need not have a unique address
//  BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE: defined if the annotation is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides a preprocessor macro that indicates
// that a non-static data member need not have an address distinct from all
// other non-static data members of its class.  A so-annotated member of an
// empty type may occupy no storage.
//
// The macro expands to `[[no_unique_address]]` where supported.  On MSVC
// (including `clang-cl`), which ignores the standard attribute, it expands to
// `[[msvc::no_unique_address]]`.  Otherwise, it expands to nothing.
//
// WARNING: `BSLA_NO_UNIQUE_ADDRESS` can change the size and layout of the
// class in which it is used.  Linking together code compiled with and code
// compiled without support for this attribute (e.g., with different compilers
// or language standards) will introduce binary incompatibility for any class
// using `BSLA_NO_UNIQUE_ADDRESS` that is shared between that code.  To prevent
// this, this component uses `bsls_linkcoercion` so that such a link fails.
//
///Macro Reference
///---------------
//: `BSLA_NO_UNIQUE_ADDRESS`:
//:     This annotation, placed before the declaration of a non-static data
//:     member, indicates that the member need not have a unique address.
//:
//: `BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE`:
//:     The macro `BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE` is defined if
//:     `BSLA_NO_UNIQUE_ADDRESS` expands to something with the desired effect;
//:     otherwise `BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE` is not defined and
//:     `BSLA_NO_UNIQUE_ADDRESS` expands to nothing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Annotating a Data Member of an Empty Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define an empty type, `DefaultPolicy`, and a class, `Widget`,
// having a data member of type `DefaultPolicy` annotated with
// `BSLA_NO_UNIQUE_ADDRESS`:
// ```
// struct DefaultPolicy {
// };
//
// struct Widget {
//     BSLA_NO_UNIQUE_ADDRESS DefaultPolicy d_policy;
//     int                                  d_value;
// };
// ```
// Then, we observe that, if the annotation is active, `d_policy` occupies no
// storage:
// ```
// #ifdef BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE
//     assert(sizeof(Widget) == sizeof(int));
// #endif
// ```

#include <bsls_compilerfeatures.h>
#include <bsls_linkcoercion.h>
#include <bsls_platform.h>

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_NO_UNIQUE_ADDRESS)
#error BSLA_NO_UNIQUE_ADDRESS is already defined!
#endif

#if defined(BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE)
#error BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE is already defined!
#endif

                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NO_UNIQUE_ADDRESS)
    #if defined(BSLS_PLATFORM_CMP_MSVC)  // MSVC ignores the standard spelling
        #define BSLA_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
    #else
        #define BSLA_NO_UNIQUE_ADDRESS [[no_unique_address]]
    #endif

    #define BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE 1
#else
    #define BSLA_NO_UNIQUE_ADDRESS
#endif

                       // ==================
                       // Link-time Coercion
                       // ==================

namespace BloombergLP {
namespace bsla {

#if defined(BSLA_NO_UNIQUE_ADDRESS_IS_ACTIVE)
/// This component-private `struct` provides a symbol, defined only if
/// `BSLA_NO_UNIQUE_ADDRESS` is active when this library is built.
struct NoUniqueAddress_Active {
    static const int s_value;
};
typedef NoUniqueAddress_Active NoUniqueAddress_Coerce;
#else
/// This component-private `struct` provides a symbol, defined only if
/// `BSLA_NO_UNIQUE_ADDRESS` is not active when this library is built.
struct NoUniqueAddress_Inactive {
    static const int s_value;
};
typedef NoUniqueAddress_Inactive NoUniqueAddress_Coerce;
#endif

}  // close package namespace

// Fail to link code built with a different `BSLA_NO_UNIQUE_ADDRESS` setting
// than this library.
BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(
                                         const int,
                                         bsla_nouniqueaddress_coerce,
                                         bsla::NoUniqueAddress_Coerce::s_value)

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
