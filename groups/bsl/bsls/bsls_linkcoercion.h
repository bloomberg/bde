// bsls_linkcoercion.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_LINKCOERCION
#define INCLUDED_BSLS_LINKCOERCION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a way to force a link-time dependency into an object.
//
//@MACROS:
//  BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY
//
//@DESCRIPTION: This component provides a facility to force a link-time
// dependency on a symbol of the name supplied to the macro into the
// translation unit being compiled. This macro can be useful for generating
// link-time failures if a library version mismatch occurs.
//
///Macro Summary
///-------------
// This section provides a brief description of the macros defined in this
// component.
//..
//  BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type, refName, referredSymbol);
//      Force a link-time dependency into an object by creating a "symbol" (a
//      static variable with external linkage) having specified 'type'
//      (typically 'const char *'), the specified 'name' (which should be
//      unique), and that will be given the value of the address of the
//      specified 'referredSymbol'.
//..
// Notice that this macro is typically used to prevent to binary incompatible
// object files from being successfully linked together.  This is achieved by
// calling this macro in a way that 'referredSymbol' will have a different
// symbol name for incompatible builds.
//
///Link coercion mechanism
///-----------------------
// Link-time coersion is implemented by creating a static variable that
// references an external symbol defined by the library.  The actual name of
// this external symbol should be different for different build flavors.  When
// the executable is linked against the libraries with link-coercion symbols,
// the linker needs to resolve the symbol name and will fail the link step if
// the required symbol name is not present in the linked library (i.e. the
// library was built with a different set of flags that control the coercion
// symbol creation).  For example, we can create different coercion symbols for
// code built with c++17 and c++20 flags, effectively preventing translation
// units compiled with different c++ standard flags from being linked into an
// working program.
//
///Linker garbage collection
///-------------------------
// Modern linkers may implement link-time garbage collection.  In order to
// minimize the size of the final executable the linker can remove sections of
// object files that are not referenced anywhere from the code being linked.
// Unfortunately, the link coercion symbols are not referenced directly and may
// be garbage collected and, if removed, lead to a silent failure and
// successful linkage of a potentially binary incompatible executable.  To
// avoid such a failure, coercion symbols are placed in the dedicated data
// section (if supported by the compiler) and this section is marked in-use to
// avoid it being garbage collected by the linker.
//
// More information on details on linker garbage collection can be found here:
// https://maskray.me/blog/2021-02-28-linker-garbage-collection
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Prevent Linking Debug and Optimized Translation Units
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we use a link coercion symbol to prevent linking a
// translation unit built against the header in debug mode, with a translation
// unit built against the header in optimized mode.
//
// First we define a macro such that our symbol name will be different for
// debug and optimized builds of our library:
//..
//  mylib_safecompile.h
//  #ifdef NDEBUG
//      #define MYLIB_DEBUG_SYMBOL_NAME mylib_BuiltWithNDebugMode
//  #else
//      #define MYLIB_DEBUG_SYMBOL_NAME mylib_BuiltWithoutNDebugMode
//  #endif
//..
// Now we use the 'BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY' macro to require
// translation units to link against a library having a matching symbol name:
//..
//  extern const char* MYLIB_DEBUG_SYMBOL_NAME;
//  BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char *,
//                                            mylib_coercebasedondebugmode,
//                                            MYLIB_DEBUG_SYMBOL_NAME )
//..
// Note that the referred symbol will have a different name depending on
// whether 'NDEBUG' is defined or not.
//
// Finally, in the corresponding '.cpp' file we must define the appropriate
// symbol name for the library:
//..
//   mylib_safecompile.cpp
//   const char *MYLIB_DEBUG_SYMBOL_NAME = "debug compatibility check: "
//                                          #MYLIB_DEBUG_SYMBOL_NAME;
//..

#include <bsls_platform.h>

namespace BloombergLP {

#if defined(BSLS_PLATFORM_OS_LINUX)
    // Note: we cannot detect the version of the binutils here, so we
    // tie those specific attributes to the version of the compilers that
    // should support them.
    // Please see the "Linker garbage collection" section for details.
    #if  (defined(BSLS_PLATFORM_CMP_GNU)                                      \
             && BSLS_PLATFORM_CMP_VERSION >= 110000)                          \
      || (defined(BSLS_PLATFORM_CMP_CLANG)                                    \
             && BSLS_PLATFORM_CMP_VERSION >= 130000)
        #define BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type,               \
                                                          refName,            \
                                                          referredSymbol)     \
        _Pragma("GCC diagnostic push")                                        \
        _Pragma("GCC diagnostic ignored \"-Wattributes\"")                    \
        static type *refName __attribute__((retain,used,section("coercion"))) \
                                                           = &referredSymbol; \
        _Pragma("GCC diagnostic pop")
    #else
        #define BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type,               \
                                                          refName,            \
                                                          referredSymbol)     \
        static type *refName __attribute__((used)) = &referredSymbol;
    #endif

#elif defined(BSLS_PLATFORM_OS_DARWIN)
    #define BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type,                   \
                                                      refName,                \
                                                      referredSymbol)         \
    static type *refName __attribute__((used)) = &referredSymbol;
#elif defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type,                   \
                                                      refName,                \
                                                      referredSymbol)         \
    static type *refName = &referredSymbol;
#elif defined(BSLS_PLATFORM_CMP_SUN)
    #define BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type,                   \
                                                      refName,                \
                                                      referredSymbol)         \
    type refName() __attribute__((weak));                                     \
    type refName() { return referredSymbol; }
#else
    #define BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(type,                   \
                                                      refName,                \
                                                      referredSymbol)         \
    namespace {                                                               \
        extern type *const refName = &referredSymbol;                         \
    }
#endif

}  // close enterprise namespace

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
