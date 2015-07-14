// bdlsu_processutil.h                 -*-C++-*-
#ifndef INCLUDED_BDLSU_PROCESSUTIL
#define INCLUDED_BDLSU_PROCESSUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide basic platform-independent utilities related to processes.
//
//@CLASSES:
//  bdlsu::ProcessUtil: portable utility methods related to processes
//
//@AUTHOR: Bruce Szablak (bszablak)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a platform-independent interface for
// processes.  Currently, it provides (only) a utility to get the current
// process ID.
//
///Usage
///-----
// Get the current process ID:
//..
//  int pid = bdlsu::ProcessUtil::getProcessId();
//..
// Get the current process name:
//..
//  bsl::string processName;
//  bdlsu::ProcessUtil::getProcessName(&processName);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace bdlsu {
                           // ========================
                           // struct ProcessUtil
                           // ========================

struct ProcessUtil {
    // This 'struct' contains utility methods for platform-independent
    // process operations.

    // CLASS METHODS
    static int getProcessId();
        // Return the system specific process identifier for the currently
        // running process.

    static int getProcessName(bsl::string *result);
        // Load the system specific process name for the currently running
        // process into the specified 'result'.  Return 0 on success, and a
        // non-zero value otherwise.  Note that on many systems, this is the
        // fully qualified path name of the current executable.
};
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
