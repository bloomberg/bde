// bdesu_pipeutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDESU_PIPEUTIL
#define INCLUDED_BDESU_PIPEUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide basic portable named-pipe utilities.
//
//@CLASSES:
//  bdesu_PipeUtil: Portable utility methods for naming and accessing pipes
//
//@AUTHOR: David Schumann (dschumann1)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides portable utility methods for named
// pipes.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif


namespace BloombergLP {

                              // =====================
                              // struct bdesu_PipeUtil
                              // =====================
struct bdesu_PipeUtil {
    // This struct contains utility methods for platform-independent
    // named pipe operations.

    static int makeCanonicalName(bsl::string            *pipeName,
                                 const bdeut_StringRef&  baseName);
        // Load into the specified 'pipeName' the system-dependent canonical
        // pipe name corresponding to the specified 'baseName'.  On Unix
        // systems, attempts to load the environment variable TMPDIR to create
        // the pipe name.  Return 0 on success, and a nonzero value if
        // 'baseName' cannot be part of a pipe name on this system.

    static int send(const bdeut_StringRef& pipeName,
                    const bdeut_StringRef& message);
        // Send the specified 'message' to the pipe with the specified
        // 'pipeName'.  Return 0 on success, and a nonzero value otherwise.

    static bool isOpenForReading(const bdeut_StringRef& pipeName);
        // Return 'true' if the pipe with the specified 'pipeName' exists and
        // is currently open for reading by some process, and 'false'
        // otherwise.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
