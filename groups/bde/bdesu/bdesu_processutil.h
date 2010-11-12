// bdesu_processutil.h                 -*-C++-*-
#ifndef INCLUDED_BDESU_PROCESSUTIL
#define INCLUDED_BDESU_PROCESSUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide basic platform-independent utilities related to processes.
//
//@CLASSES:
//  bdesu_ProcessUtil: portable utility methods related to processes
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
//  int pid = bdesu_ProcessUtil::getProcessId();
//..
// Get the current process name:
//..
//  bsl::string processName;
//  bdesu_ProcessUtil::getProcessName(&processName);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                           // ========================
                           // struct bdesu_ProcessUtil
                           // ========================

struct bdesu_ProcessUtil {
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
