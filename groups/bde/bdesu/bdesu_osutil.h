// bdesu_osutil.h                                                     -*-C++-*-
#ifndef INCLUDED_BDESU_OSUTIL
#define INCLUDED_BDESU_OSUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities related to the operating system.
//
//@CLASSES:
//   bdesu::OsUtil: namespace for operating system information utilities
//
//@SEE ALSO:
//
//@DESCRIPTION: This component provides a namespace, 'OsUtil'
// containing utility functions for retrieving information about the runtime
// operating system.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
///- - - - - -
// Display the operating system, version and patch on standard output:
//..
//  bsl::string name;
//  bsl::string version;
//  bsl::string patch;
//  ASSERT(0 == getOsInfo(&name, &version, &patch);
//  std::cout << name << "Version: " << version << " Patch: " << patch << "\n";
//..

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace bdesu {

                        // =============
                        // struct OsUtil
                        // =============

struct OsUtil {
    // This 'struct' provides a namespace for utility functions retrieving
    // information about the operating system.

    // CLASS METHODS
    static int getOsInfo(bsl::string *osName,
                         bsl::string *osVersion,
                         bsl::string *osPatch);
    // Load the OS name, version and patch into the specified 'osName',
    // 'osVersion' and 'osPatch' respectively. Return 0 on success and a
    // non-zero value otherwise. On MS Windows systems 'osName' will be
    // 'Windows', with 'osVersion' set to version, and 'osPatch' to Service
    // Pack number. On Posix systems, 'osName', 'osVersion' and 'osPatch' will
    // be set to 'sysname', 'release', and 'version', respectively, as returned
    // by !uname(2)!.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
