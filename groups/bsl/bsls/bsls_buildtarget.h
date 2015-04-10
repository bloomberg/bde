// bsls_buildtarget.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLS_BUILDTARGET
#define INCLUDED_BSLS_BUILDTARGET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide build-target information in object files.
//
//@MACROS:
//  BDE_BUILD_TARGET_EXC: flag identifying exception-enabled builds
//  BDE_BUILD_TARGET_NO_EXC: flag identifying exception-disabled builds
//  BDE_BUILD_TARGET_MT: flag identifying multi-threaded builds
//  BDE_BUILD_TARGET_NO_MT: flag identifying builds that do not support threads
//  BDE_OMIT_DEPRECATED: flag for omitting deprecated code from BDE source
#ifndef BDE_OMIT_TRANSITIONAL  // internal code management
//  BDE_OMIT_INTERNAL_DEPRECATED: flag to omit internal-only deprecated code
//  BDE_OMIT_TRANSITIONAL: marker for non-deprecated internal-only code
#endif  // BDE_OMIT_TRANSITIONAL -- internal code management
//
//@DESCRIPTION: The purpose of this component is to cause a link-time error
// when trying to link an executable with incompatible libraries.  This
// component defines type names that indicate two build target parameters.
// These parameters determine whether the build was exception-enabled (which is
// the case unless overridden by defining the 'BDE_BUILD_TARGET_NO_EXC' macro),
// and whether it was multi-threaded (which is enabled unless overridden by
// defining the 'BDE_BUILD_TARGET_NO_MT' macro).  The types defined by this
// component should not be used directly.
//
///Deprecation Control Macros
///--------------------------
// In addition to the 'BDE_BUILD_TARGET_*' macros that determine the link-time
// compatibility of different libraries built on BDE, this component documents
// a macro that determines whether deprecated interfaces are available to
// programs built on BDE:
//
//: o BDE_OMIT_DEPRECATED: This macro, if defined, indicates that all
//:   deprecated code is excluded from a build of the library.
//
#ifndef BDE_OMIT_TRANSITIONAL  // internal code management
// In addition to 'BDE_OMIT_DEPRECATED', there are two other macros that also
// determine whether deprecated interfaces are available to programs built on
// BDE:
//
//: o BDE_OMIT_INTERNAL_DEPRECATED: This macro, if defined, indicates that all
//:   deprecated code that appears only in the internal Bloomberg BDE codebase
//:   is excluded from a build of the library.  This category consists almost
//:   entirely of code that was already deprecated at the time of a component's
//:   initial release to open-source.  Code that is deprecated after a
//:   component's open-source release should be marked as 'BDE_OMIT_DEPRECATED'
//:   instead.
//:
//: o BDE_OMIT_TRANSITIONAL: This macro marks code that is not deprecated, but
//:   is nevertheless excluded from the open-source release of BDE.  This
//:   category consists primarily of code that exists to support STLPort
//:   containers, which were not included in the open-source release, or that
//:   documents backward compatibility access to the package-prefix versions of
//:   non-deprecated symbols in BSL.  'BDE_OMIT_TRANSITIONAL' exists for
//:   purposes of documentation only, and should not be defined for any build
//:   of the library.  In particular, there is no guarantee that the library
//:   can be built or will function correctly if 'BDE_OMIT_TRANSITIONAL' is
//:   defined.
//
// Together with 'BDE_OMIT_DEPRECATED', these macros divide the BDE codebase
// into four categories:
//
//: o current universally distributed code     [no label]
//:
//: o current Bloomberg-only code              ['BDE_OMIT_TRANSITIONAL']
//:
//: o deprecated universally distributed code  ['BDE_OMIT_DEPRECATED']
//:
//: o deprecated Bloomberg-only code           ['BDE_OMIT_INTERNAL_DEPRECATED']
//:
//
// By default, all code in BDE is both current and universally distributed.
// All code that is deprecated, excluded from our open-source distribution, or
// both, is surrounded with conditional compilation macros to allow test builds
// without that code, and/or to make it easy to prepare an open-source
// distribution from the full internal BDE codebase.  The conditional
// compilation macros are:
//
#endif  // BDE_OMIT_TRANSITIONAL -- internal code management
///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

#ifndef INCLUDED_BSLS_LINKCOERCION
#include <bsls_linkcoercion.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

// Default to an exception-enabled build unless 'BDE_BUILD_TARGET_NO_EXC' is
// defined.

#ifndef BDE_BUILD_TARGET_NO_EXC

#ifndef BDE_BUILD_TARGET_EXC
#define BDE_BUILD_TARGET_EXC
#endif

namespace bsls {

struct BuildTargetExcYes {
    static const int s_isBuildTargetExc;
};
typedef BuildTargetExcYes BuildTargetExc;

}  // close package namespace

#else

#ifdef BDE_BUILD_TARGET_EXC
#error Do not define both BDE_BUILD_TARGET_EXC and BDE_BUILD_TARGET_NO_EXC
#endif

namespace bsls {

struct BuildTargetExcNo {
    static const int s_isBuildTargetExc;
};
typedef BuildTargetExcNo BuildTargetExc;

}  // close package namespace

#endif

// Default to a threaded (MT) build unless 'BDE_BUILD_TARGET_NO_MT' is defined.

#ifndef BDE_BUILD_TARGET_NO_MT

#ifndef BDE_BUILD_TARGET_MT
#define BDE_BUILD_TARGET_MT
#endif

namespace bsls {

struct BuildTargetMtYes {
    static const int s_isBuildTargetMt;
};
typedef BuildTargetMtYes BuildTargetMt;

}  // close package namespace

#else

#ifdef BDE_BUILD_TARGET_MT
#error Do not define both BDE_BUILD_TARGET_MT and BDE_BUILD_TARGET_NO_MT
#endif

namespace bsls {

struct BuildTargetMtNo {
    static const int s_isBuildTargetMt;
};
typedef BuildTargetMtNo BuildTargetMt;

}  // close package namespace

#endif

// Force linker to pull in this component's object file.

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(
                                      const int,
                                      bsls_buildtarget_assertion1,
                                      bsls::BuildTargetExc::s_isBuildTargetExc)

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(
                                        const int,
                                        bsls_buildtarget_assertion2,
                                        bsls::BuildTargetMt::s_isBuildTargetMt)

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bsls::BuildTargetExc bsls_ExcBuildTarget;
    // This alias is defined for backward compatibility.

typedef bsls::BuildTargetMt bsls_MtBuildTarget;
    // This alias is defined for backward compatibility.

#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

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
