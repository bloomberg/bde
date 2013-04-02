// btemt_log.h                                                        -*-C++-*-
#ifndef INCLUDED_BTEMT_LOG
#define INCLUDED_BTEMT_LOG

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Placeholde for logging facilities in btemt
//
//@CLASSES:
//   btemt::Log: namespace for placeholders for logging
//
//@SEE ALSO:
//
//@DESCRIPTION: Placeholder, while deciding how to replace BAEL functions in
//  SOCKS5
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
///- - - - - -
// Suppose that ...
//

namespace BloombergLP {
namespace btemt {

#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_AUTH_FAILED
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_DNS_DEST_NAME
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_METHOD
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_REPLY
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_BAD_VER
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_CONN_FAILED
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_RECV_FAILED
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_RSLV_FAILED
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_SEND_FAILED
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_START(label, string)
#define BTEMT_LOG_DIAGNOSTICCODE_PROXY_SUCCESS


                        // ==========
                        // struct Log
                        // ==========

struct Log {
    // This 'struct' provides a namespace for utility functions that support
    // non-BAEL logging.

    // TYPES
    class LogStream {
    };

    // CLASS DATA
    static Log::LogStream s_logStream;
};

// BAEL-like macros
#define BTEMT_LOG_SET_CATEGORY(CATEGORY)

#define BTEMT_LOG_TRACE { btemt::Log::s_logStream
#define BTEMT_LOG_DEBUG { btemt::Log::s_logStream
#define BTEMT_LOG_INFO  { btemt::Log::s_logStream
#define BTEMT_LOG_ERROR { btemt::Log::s_logStream
#define BTEMT_LOG_WARN  { btemt::Log::s_logStream
#define BTEMT_LOG_ERROR { btemt::Log::s_logStream
#define BTEMT_LOG_FATAL { btemt::Log::s_logStream
#define BTEMT_LOG_END    ""; }


// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------
                        // struct Log
                        // ----------

template <class TYPE>
inline
Log::LogStream& operator<<(Log::LogStream& stream, TYPE object)
{
    return stream;
}

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
