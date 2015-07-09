// bblscm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BBLSCM_VERSION
#define INCLUDED_BBLSCM_VERSION

#ifndef INCLUDED_BBLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
// bblscm::Version: namespace for RCS and SCCS versioning information for 'bbl'
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bbl' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bbl' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'bblscm::Version' 'struct' can be used to query version
// information for the 'bbl' package group at runtime.  The following usage
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// '#include' this component header file to get 'bbl' version information
// embedded in an executable.  It is only necessary to use one or more 'bbl'
// components (and, hence, link in the 'bbl' library).
//
///Usage
///-----
// A program can display the version of BBL that was used to build it by
// printing the version string returned by 'bblscm::Version::version()' to
// 'stdout' as follows:
//..
//  bsl::printf("BBL version: %s\n", bblscm::Version::version());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BBLSCM_VERSIONTAG
#include <bblscm_versiontag.h>     // 'BBL_VERSION_MAJOR', 'BBL_VERSION_MINOR'
#endif

#ifndef INCLUDED_BSLS_LINKCOERCION
#include <bsls_linkcoercion.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif


namespace BloombergLP {
namespace bblscm {

struct Version {
    static const char *s_ident;
    static const char *s_what;

#define BBLSCM_CONCAT2(a,b,c,d,e) a ## b ## c ## d ## e
#define BBLSCM_CONCAT(a,b,c,d,e)  BBLSCM_CONCAT2(a,b,c,d,e)

// 'BBLSCM_D_VERSION' is a symbol whose name warns users of version mismatch
// linking errors.  Note that the exact string "compiled_this_object" must be
// present in this version coercion symbol.  Tools may look for this pattern to
// warn users of mismatches.
#define BBLSCM_D_VERSION BBLSCM_CONCAT(                \
                       d_version_BBL_,                 \
                       BBL_VERSION_MAJOR,              \
                       _,                              \
                       BBL_VERSION_MINOR,              \
                       _compiled_this_object)

    static const char *BBLSCM_S_VERSION;

    static const char *s_dependencies;
    static const char *s_buildInfo;
    static const char *s_timestamp;
    static const char *s_sourceControlInfo;

    static const char *version();
};

inline
const char *Version::version()
{
    return BBLSCM_S_VERSION;
}

}  // close package namespace

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char *,
                                          bblscm_version_assertion,
                                          bblscm::Version::BBLSCM_S_VERSION);

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
