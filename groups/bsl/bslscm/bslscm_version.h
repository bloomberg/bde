// bslscm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSCM_VERSION
#define INCLUDED_BSLSCM_VERSION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
// bslscm::Version: namespace for RCS and SCCS versioning information for 'bsl'
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bsl' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bsl' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'bslscm::Version' 'struct' can be used to query version
// information for the 'bsl' package group at runtime.  The following usage
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// '#include' this component header file to get 'bsl' version information
// embedded in an executable.  It is only necessary to use one or more 'bsl'
// components (and, hence, link in the 'bsl' library).
//
///Usage
///-----
// A program can display the version of BSL that was used to build it by
// printing the version string returned by 'bslscm::Version::version()' to
// 'stdout' as follows:
//..
//  std::printf("BSL version: %s\n", bslscm::Version::version());
//..

#include <bsls_buildtarget.h>      // need to ensure consistent build options
#include <bsls_platform.h>
#include <bsls_linkcoercion.h>

#include <bslscm_versiontag.h> // BSL_VERSION_MAJOR, BSL_VERSION_MINOR

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // This macro controls whether we allow features which we must continue to
    // support for our clients but do not want to rely on in our own code base.
    // Clients who want to continue using these features need take no special
    // steps.  In order to make sure an entire code base does not rely on these
    // features, recompile with this macro defined.  Examples of such features
    // are: including '<stdheader>' as opposed to '<bsl_stdheader.h>', or using
    // 'DEBUG' instead of 'e_DEBUG'.
#else
#define BDE_DONT_ALLOW_TRANSITIVE_INCLUDES 1
    // When we don't want to rely on legacy features, we also want to make sure
    // we are not picking up macros or type aliases via (direct or transitive)
    // includes of headers that have migrated from 'bde' to 'bsl' libraries.
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

namespace BloombergLP {

namespace bslscm {

                         // =======
                         // Version
                         // =======

struct Version {
    // This struct provides a namespace for (1) source control management
    // (versioning) information that is embedded in binary executable files,
    // and (2) a facility to query that information at runtime.

    // CLASS DATA
    static const char *s_ident;
    static const char *s_what;

#define BSLSCM_CONCAT2(a,b,c,d,e,f) a ## b ## c ## d ## e ## f
#define BSLSCM_CONCAT(a,b,c,d,e,f)  BSLSCM_CONCAT2(a,b,c,d,e,f)

// 'BSLSCM_D_VERSION' is a symbol whose name warns users of version mismatch
// linking errors.  Note that the exact string "compiled_this_object" must be
// present in this version coercion symbol.  Tools may look for this pattern to
// warn users of mismatches.

#define BSLSCM_S_VERSION BSLSCM_CONCAT(s_version_BSL_,       \
                                       BSL_VERSION_MAJOR, _, \
                                       BSL_VERSION_MINOR, _, \
                                       compiled_this_object)

    static const char *BSLSCM_S_VERSION;

    static const char *s_dependencies;
    static const char *s_buildInfo;
    static const char *s_timestamp;
    static const char *s_sourceControlInfo;

    // CLASS METHODS
    static const char *version();
        // Return the address of a character string that identifies the version
        // of the 'bsl' package group in use.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------
                         // struct Version
                         // --------------

// CLASS METHODS
inline
const char *Version::version()
{
    return BSLSCM_S_VERSION;
}

}  // close package namespace

// Force linker to pull in this component's object file.

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char *,
                                          bslscm_version_assertion,
                                          bslscm::Version::BSLSCM_S_VERSION)

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
