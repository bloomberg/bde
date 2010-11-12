// bael_defaultobserver.h                                             -*-C++-*-
#ifndef INCLUDED_BAEL_DEFAULTOBSERVER
#define INCLUDED_BAEL_DEFAULTOBSERVER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a default observer that emits log records to 'stdout'.
//
//@CLASSES:
//    bael_DefaultObserver: observer that outputs log records to 'stdout'
//
//@SEE_ALSO: bael_record, bael_context, bael_loggermanager
//
//@AUTHOR: Banyar Aung
//
//@DESCRIPTION: This component provides a default concrete implementation of
// the 'bael_Observer' protocol for receiving and processing log records:
//..
//               ( bael_DefaultObserver )
//                           |              ctor
//                           |
//                           V
//                   ( bael_Observer )
//                                          dtor
//                                          publish
//..
// 'bael_DefaultObserver' is a concrete class derived from 'bael_Observer'
// that processes the log records it receives through its 'publish' method
// by printing them to 'stdout'.  Given its minimal functionality,
// 'bael_DefaultObserver' is intended for development use only.
// 'bael_DefaultObserver' is not recommended for use in a production
// environment.
//
///Usage
///-----
// The following code fragments illustrate the essentials of using a default
// observer within a 'bael' logging system.
//
// First create a 'bael_DefaultObserver' named 'defaultObserver':
//..
//     bael_DefaultObserver defaultObserver(&bsl::cout);
//..
// The default observer must then be installed within a 'bael' logging system.
// This is done by passing 'defaultObserver' to the 'bael_LoggerManager'
// constructor, which also requires a 'bael_LoggerManagerConfiguration' object.
//..
//     bael_LoggerManagerConfiguration lmc;
//     bael_LoggerManager              loggerManager(&defaultObserver, lmc);
//..
// Henceforth, all messages that are published by the logging system will be
// transmitted to the 'publish' method of 'defaultObserver'.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_OBSERVER
#include <bael_observer.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bael_Context;
class bael_Record;

                           // ==========================
                           // class bael_DefaultObserver
                           // ==========================

class bael_DefaultObserver : public bael_Observer {
    // This class provides a default implementation of the 'bael_Observer'
    // protocol.  The 'publish' method of this class outputs the log records
    // that it receives to an instance of 'bsl::ostream' that is supplied
    // at construction.

    // DATA
    bsl::ostream *d_stream;  // output sink for log records
    bcemt_Mutex   d_mutex;   // serializes concurrent calls to 'publish'

    // NOT IMPLEMENTED
    bael_DefaultObserver(const bael_DefaultObserver&);
    bael_DefaultObserver& operator=(const bael_DefaultObserver&);

  public:
    // CREATORS
    bael_DefaultObserver(bsl::ostream *stream);
        // Create a default observer that transmits log records to the
        // specified 'stream'.

    bael_DefaultObserver(bsl::ostream& stream);
        // Create a default observer that transmits log records to the
        // specified 'stream'.
        //
        // DEPRECATED: replaced by 'bael_DefaultObserver(bsl::ostream *stream)'

    virtual ~bael_DefaultObserver();
        // Destroy this default observer.

    // MANIPULATORS
    virtual void publish(const bael_Record&  record,
                         const bael_Context& context);
        // Process the specified log 'record' having the specified publishing
        // 'context'.
        //
        // Print 'record' and 'context' to the 'bsl::ostream' supplied at
        // construction.  The behavior is undefined if 'record' or 'context' is
        // modified during the execution of this method.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                           // --------------------------
                           // class bael_DefaultObserver
                           // --------------------------

// CREATORS
inline
bael_DefaultObserver::bael_DefaultObserver(bsl::ostream& stream)
: d_stream(&stream)
{
}

inline
bael_DefaultObserver::bael_DefaultObserver(bsl::ostream *stream)
: d_stream(stream)
{
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
