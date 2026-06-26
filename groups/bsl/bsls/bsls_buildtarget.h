// bsls_buildtarget.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLS_BUILDTARGET
#define INCLUDED_BSLS_BUILDTARGET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide build-target information in object files.
//
//@MACROS:
//  BDE_BUILD_TARGET_EXC: flag identifying exception-enabled builds
//  BDE_BUILD_TARGET_MT: flag identifying multi-threaded builds (deprecated)
//  BDE_BUILD_TARGET_32: flag identifying 32-bit builds
//  BDE_BUILD_TARGET_64: flag identifying 64-bit builds
//  BDE_BUILD_TARGET_ASAN: flag identifying address sanitizer builds
//  BDE_BUILD_TARGET_MSAN: flag identifying memory sanitizer builds
//  BDE_BUILD_TARGET_OPT: flag identifying an optimized build
//  BDE_BUILD_TARGET_SAFE: flag identifying SAFE level assert check builds
//  BDE_BUILD_TARGET_SAFE_2: flag identifying SAFE_2 level assert check builds
//  BDE_BUILD_TARGET_TSAN: flag identifying thread sanitizer builds
//  BDE_BUILD_TARGET_UBSAN: flag identifying UB sanitizer builds
//  BDE_BUILD_SKIP_VERSION_CHECKS: turn off compiler version checks
//  BDE_DONT_ALLOW_TRANSITIVE_INCLUDES: remove unused #includes from headers
//  BDE_OMIT_DEPRECATED: legacy flag to deprecate a block of code
//  BDE_OMIT_INTERNAL_DEPRECATED: legacy flag to deprecate internal-only code
//  BDE_OPENSOURCE_PUBLICATION: marker for non-deprecated internal-only code
//
//@SEE_ALSO: bsls_deprecate, bsls_assert, bsls_review
//
//@DESCRIPTION: The purpose of this component is to cause a link-time error
// when trying to link an executable with incompatible libraries.  This
// component defines type names that indicate two build target parameters.
// These parameters determine whether the build was exception-enabled (which is
// the case unless overridden by defining the `BDE_BUILD_TARGET_NO_EXC` macro).
// The types defined by this component should not be used directly.  This
// component also documents macros that can be used to disable checks that are
// performed elsewhere.  Note that the deprecated macro `BDE_BUILD_TARGET_MT`
// is always defined for historical reasons where it was used to detect that
// a build supported multiple threads; there is no option to disable support
// for threading in BDE.
//
///Compiler Version Check Macro
///----------------------------
// By design, programs using BDE fail to build if support for a C++ standard
// version is requested but the compiler's support for that standard is not
// sufficiently stable (possibly causing issues at runtime that are very
// difficult to debug).  We provide the macro `BDE_BUILD_SKIP_VERSION_CHECKS`
// to disable these checks for the purpose of testing and experimenting with
// different compiler configurations.  This macro should **not** be defined for
// integrated production builds such as dpkg, as doing so may result in
// unstable code being deployed to production.
//
///Standard Version Inconsistency Check Macro
///------------------------------------------
// C++ object files built using a particular standard version must be linked
// with BDE libraries built using the same standard version, or else the
// program will be ill formed, no diagnostic required (IFNDR) and may exhibit
// issues at runtime that are very difficult to debug.
//
///Deprecation Control Macros
///--------------------------
// In addition to the `BDE_BUILD_TARGET_*` macros that determine the link-time
// compatibility of different libraries built on BDE, this component documents
// a macro that determines whether deprecated interfaces are available to
// programs built on BDE:
//
//: `BDE_OMIT_DEPRECATED`:
//:     This macro, if defined, indicates that all code deprecated before BDE
//:     3.2 will be either omitted from a build of the library (if the code has
//:     not been updated to use `bsls_deprecate`), or identified to the
//:     compiler as deprecated (if the code has been updated to use
//:     `bsls_deprecate`).  New uses of this macro are not supported; use
//:     `bsls_deprecate` instead.
//
// In addition to `BDE_OMIT_DEPRECATED`, there are two other macros that also
// determine whether deprecated interfaces are available to programs built on
// BDE:
//
//: `BDE_OMIT_INTERNAL_DEPRECATED`:
//:     This macro, if defined, indicates that all internal-only code
//:     deprecated before BDE 3.2 will be either omitted from a build of the
//:     library (if the code has not been updated to use `bsls_deprecate`), or
//:     identified to the compiler as deprecated (if the code has been updated
//:     to use `bsls_deprecate`).  This category consists almost entirely of
//:     code that was already deprecated at the time of a component's initial
//:     release to open-source.  New uses of this macro are not supported; use
//:     `bsls_deprecate` instead.
//:
//: `BDE_OPENSOURCE_PUBLICATION`:
//:     This macro marks code that is not deprecated, but is nevertheless
//:     excluded from the open-source release of BDE.  This category consists
//:     primarily of code that exists to support STLPort containers, which were
//:     not included in the open-source release, or that documents backward
//:     compatibility access to the package-prefix versions of non-deprecated
//:     symbols in BSL.  `BDE_OPENSOURCE_PUBLICATION` exists for purposes of
//:     documentation only, and should not be defined for any build of the
//:     library.  In particular, there is no guarantee that the library will
//:     build or will function correctly if `BDE_OPENSOURCE_PUBLICATION` is
//:     defined.
//
// Together with `BDE_OMIT_DEPRECATED`, these macros divide the BDE codebase
// into four categories:
// ```
// +-----------------------------------------+------------------------------+
// | CATEGORY                                | LABEL                        |
// +=========================================+==============================+
// | universally distributed code            | no label                     |
// +-----------------------------------------+------------------------------+
// | Bloomberg-only code                     | BDE_OPENSOURCE_PUBLICATION   |
// +-----------------------------------------+------------------------------+
// | universally distributed code deprecated | BDE_OMIT_DEPRECATED          |
// | before BDE 3.2                          |                              |
// +-----------------------------------------+------------------------------+
// | Bloomberg-only code deprecated before   | BDE_OMIT_INTERNAL_DEPRECATED |
// | BDE 3.2                                 |                              |
// +-----------------------------------------+------------------------------+
// ```
// By default, all code in BDE is both current and universally distributed.
// All code that is deprecated, excluded from our open-source distribution, or
// both, is surrounded with conditional compilation macros to allow test builds
// without that code, and/or to make it easy to prepare an open-source
// distribution from the full internal BDE codebase.  The conditional
// compilation macros are:
//
///Sanitizer Builds
///----------------
// There a flag set by the BDE build system to indicate when a build has a
// given sanitizer enabled:
//
//: `BDE_BUILD_TARGET_ASAN`:
//:     This macro, when defined, indicates that the current build mode
//:     includes address sanitizer.
//:
//: `BDE_BUILD_TARGET_MSAN`:
//:     This macro, when defined, indicates that the current build mode
//:     includes memory sanitizer.
//:
//: `BDE_BUILD_TARGET_TSAN`:
//:     This macro, when defined, indicates that the current build mode
//:     includes thread sanitizer.
//:
//: `BDE_BUILD_TARGET_UBSAN`:
//:     This macro, when defined, indicates that the current build mode
//:     includes undefined behavior sanitizer.
//
// Preprocessor conditionals on these macros are sometimes necessary when
// certain types of tests are incompatible with a given sanitizer, but
// their use should be rare, carefully considered, and appear only in test
// drivers.
//
//
///Build Configuration Information
///-------------------------------
// Several other macros that are defined, or not, to provide information about
// the build target that code might want to adjust for during translation.
//
//: `BDE_BUILD_TARGET_32`:
//:     This macro, when defined, indicates that the current build target will
//:     emit instructions for 32-bit architectures.
//:
//: `BDE_BUILD_TARGET_64`:
//:     This macro, when defined, indicates that the current build target will
//:     emit instructions for 64-bit architectures.
//:
//: `BDE_BUILD_TARGET_OPT`:
//:     This macro, when defined, indicates that the current build target will
//:     aggressively optimize code, potentially to the detriment of debug
//:     support.  Undefined behavior is more likely to demonstrate adverse
//:     consequences that support better optimization.  By default only `OPT`
//:     assertions will be evaluated --- see `bsls_assert` and `bsls_review`
//:     for more details.
//:
//: `BDE_BUILD_TARGET_SAFE`:
//:     This macro, when defined, indicates that the current build target will
//:     enable `SAFE` assertions --- see `bsls_assert` and `bsls_review` for
//:     more details.
//:
//: `BDE_BUILD_TARGET_SAFE_2`:
//:     This macro, when defined, indicates that the current build target will
//:     enable `SAFE_2` assertions --- see `bsls_assert` and `bsls_review` for
//:     more details.
//:
//: `BDE_DONT_ALLOW_TRANSITIVE_INCLUDES`:
//:     This macro, when defined, indicates that the current build target will
//:     exclude `#include` directives that have been marked as present only to
//:     support higher level code that depended implicitly on that `#include`
//:     this is no longer necessary to compile the component itself.  Note
//:     that attempts to define this macro on the command line passed to the
//:     compiler will be ignored, and the macro to force this definition,
//:     `BDE_FORCE_DONT_ALLOW_TRANSITIVE_INCLUDES`, should be defined instead.
//:     See `bsls_ident` for more details where the consistency rules regarding
//:     these macros are enforced.
//
///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

#include <bsls_linkcoercion.h>

namespace BloombergLP {

// ============================================================================
//                           BDE_BUILD_TARGET_EXC/BDE_BUILD_TARGET_NO_EXC
// ============================================================================

// Default to an exception-enabled build unless 'BDE_BUILD_TARGET_NO_EXC' is
// defined.

#if !defined(BDE_BUILD_TARGET_NO_EXC) && !defined(BDE_BUILD_TARGET_EXC)
# define BDE_BUILD_TARGET_EXC
#elif defined(BDE_BUILD_TARGET_NO_EXC) && defined(BDE_BUILD_TARGET_EXC)
# error Do not define both `BDE_BUILD_TARGET_EXC` and `BDE_BUILD_TARGET_NO_EXC`
#endif

#if defined(BDE_BUILD_TARGET_EXC)
namespace bsls {

struct BuildTargetExcYes {
    static const int s_isBuildTargetExc;
};
typedef BuildTargetExcYes BuildTargetExc;

}  // close package namespace
#else
namespace bsls {

struct BuildTargetExcNo {
    static const int s_isBuildTargetExc;
};
typedef BuildTargetExcNo BuildTargetExc;

}  // close package namespace
#endif

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(
                                      const int,
                                      bsls_buildtarget_coerce_exc,
                                      bsls::BuildTargetExc::s_isBuildTargetExc)

// ============================================================================
//                           BDE_BUILD_TARGET_MT
// ============================================================================

// Always define `BDE_BUILD_TARGET_MT` to indicate multithreaded support.

#ifndef BDE_BUILD_TARGET_MT
# define BDE_BUILD_TARGET_MT
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
