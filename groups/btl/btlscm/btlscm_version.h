// btlscm_version.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLSCM_VERSION
#define INCLUDED_BTLSCM_VERSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
//  btlscm::Version: namespace for RCS and SCCS versioning info for 'btl'
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'btl' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'btl' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'btlscm::Version' struct can be used to query version
// information for the 'btl' package group at runtime.  The following {Usage}
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// "#include" this component header file to get 'btl' version information
// embedded in an executable.  It is only necessary to use one or more 'btl'
// components (and, hence, link in the 'btl' library).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Displaying Verson Information
/// - - - - - - - - - - - - - - - - - - - -
// A program can display the version of BTL that was used to build it by
// printing the version string returned by 'btlscm::Version::version()' to
// 'stdout' as follows:
//..
//  bsl::printf("BTL version: %s\n", btlscm::Version::version());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_LINKCOERCION
#include <bsls_linkcoercion.h>
#endif

namespace BloombergLP {

namespace btlscm {

struct Version {
    static const char *s_ident;
    static const char *s_what;

    static const char *s_version;
    static const char *s_dependencies;
    static const char *s_buildInfo;
    static const char *s_timestamp;
    static const char *s_sourceControlInfo;

    // CLASS METHODS
    static const char *version();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CLASS METHODS
inline
const char *Version::version()
{
    return s_version;
}

}  // close package namespace

// Force linker to pull in this component's object file.

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char *,
                                          btlscm_version_assertion,
                                          btlscm::Version::s_version);

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
