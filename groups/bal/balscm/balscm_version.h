// balscm_version.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALSCM_VERSION
#define INCLUDED_BALSCM_VERSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
//  balscm::Version: namespace for versioning information for 'bal'
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bal' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bal' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'balscm::Version' struct can be used to query version
// information for the 'bal' package group at runtime.  The following {Usage}
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// "#include" this component header file to get 'bal' version information
// embedded in an executable.  It is only necessary to use one or more 'bal'
// components (and, hence, link in the 'bal' library).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Embedding Version Information
/// - - - - - - - - - - - - - - - - - - - -
// The version of the 'bal' package group linked into a program can be
// obtained at runtime using the 'version' 'static' member function as follows:
//..
//        #include <balscm_version.h>
//
//        assert(0 != balscm::Version::version());
//
//        bsl::cout << "BAL version: " << balscm::Version::version()
//                  << bsl::endl;
//..
// Output similar to the following will be printed to 'stdout':
//..
//        BAL version: BLP_LIB_BDE_BAL_0.01.0
//..
// The "0.01.0" portion of the string distinguishes different versions of the
// 'bal' package group.
//
///Example 2: Accessing the Embedded Version informaiton
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// The versioning information embedded into a binary file by this component can
// be examined under UNIX using several well-known utilities.  For example:
//..
//        $ ident a.out
//        a.out:
//             $Id: BLP_LIB_BDE_BAL_0.01.0 $
//
//        $ what a.out | grep BAL
//                BLP_LIB_BDE_BAL_0.01.0
//
//        $ strings a.out | grep BAL
//        $Id: BLP_LIB_BDE_BAL_0.01.0 $
//        @(#)BLP_LIB_BDE_BAL_0.01.0
//        BLP_LIB_BDE_BAL_0.01.0
//..
// Note that 'ident' and 'what' typically will display many version strings
// unrelated to 'bal' depending on the libraries used by 'a.out'.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_LINKCOERCION
#include <bsls_linkcoercion.h>
#endif

namespace BloombergLP {

namespace balscm {
struct Version {
    // This struct provides a namespace for (1) source control management
    // (versioning) information that is embedded in binary executable files,
    // and (2) a facility to query that information at runtime.

    static const char *s_ident;              // RCS-style version string
    static const char *s_what;               // SCCS-style version string

    static const char *s_version;            // BDE-style version string
    static const char *s_dependencies;       // available for future use
    static const char *s_buildInfo;          // available for future use
    static const char *s_timestamp;          // available for future use
    static const char *s_sourceControlInfo;  // available for future use

    // CLASS METHODS
    static const char *version();
        // Return the address of a character string that identifies the version
        // of the 'bal' package group in use.
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
                                          balscm_version_assertion,
                                          balscm::Version::s_version);

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
