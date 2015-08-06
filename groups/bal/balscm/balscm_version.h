// balscm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BALSCM_VERSION
#define INCLUDED_BALSCM_VERSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
//  balscm::Version: namespace for RCS and SCCS versioning information for 'bae'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bae' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bae' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'balscm::Version' struct can be used to query version
// information for the 'bae' package group at runtime.  The following USAGE
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// "#include" this component header file to get 'bae' version information
// embedded in an executable.  It is only necessary to use one or more 'bae'
// components (and, hence, link in the 'bae' library).
//
///USAGE 1
///-------
// The version of the 'bae' package group linked into a program can be
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
//        BAL version: BLP_LIB_BAL_BAL_0.01.0
//..
// The "0.01.0" portion of the string distinguishes different versions of the
// 'bae' package group.
//
///USAGE 2
///-------
// The versioning information embedded into a binary file by this component can
// be examined under UNIX using several well-known utilities.  For example:
//..
//        $ ident a.out
//        a.out:
//             $Id: BLP_LIB_BAL_BAL_0.01.0 $
//
//        $ what a.out | grep BAL
//                BLP_LIB_BAL_BAL_0.01.0
//
//        $ strings a.out | grep BAL
//        $Id: BLP_LIB_BAL_BAL_0.01.0 $
//        @(#)BLP_LIB_BAL_BAL_0.01.0
//        BLP_LIB_BAL_BAL_0.01.0
//..
// Note that 'ident' and 'what' typically will display many version strings
// unrelated to 'bae' depending on the libraries used by 'a.out'.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

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
        // of the 'bae' package group in use.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CLASS METHODS
inline
const char *Version::version()
{
    return s_version;
}
}  // close package namespace

// Force linker to pull in this component's object file.

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char *,
                                          baescm_version_assertion,
                                          balscm::Version::s_version);

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
