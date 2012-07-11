// bael_defaultobserver.cpp                                           -*-C++-*-
#include <bael_defaultobserver.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_defaultobserver_cpp,"$Id$ $CSID$")

#include <bael_context.h>
#include <bael_record.h>
#include <bael_recordattributes.h>
#include <bael_severity.h>

#include <bcemt_lockguard.h>

#include <bdem_list.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                           // --------------------------
                           // class bael_DefaultObserver
                           // --------------------------

// CREATORS
bael_DefaultObserver::~bael_DefaultObserver()
{
}

// MANIPULATORS
void bael_DefaultObserver::publish(const bael_Record&  record,
                                   const bael_Context&)
{
    bcemt_LockGuard<bcemt_Mutex> guard(&d_mutex);

    const bael_RecordAttributes& fixedFields = record.fixedFields();

    // convert the severity level to ASCII value for publishing
    bael_Severity::Level severityLevel =
                                  (bael_Severity::Level)fixedFields.severity();

    *d_stream << '\n';

    *d_stream << fixedFields.timestamp()               << ' '
              << fixedFields.processID()               << ' '
              << fixedFields.threadID()                << ' '
              << bael_Severity::toAscii(severityLevel) << ' '
              << fixedFields.fileName()                << ' '
              << fixedFields.lineNumber()              << ' '
              << fixedFields.category()                << ' ';

    bslstl_StringRef message = fixedFields.messageRef();
    int length = message.length();
    const char *str = message.data();

    // The terminating '\0' of string reference is not written if it exists.

    d_stream->write(str, (!length || '\0' != str[length - 1]) ? length
                                                              : length - 1);
    *d_stream << ' ';

    const bdem_List& userFields = record.userFields();
    const int numUserFields = userFields.length();
    for (int i = 0; i < numUserFields; ++i) {
        *d_stream << userFields[i] << ' ';
    }

    *d_stream << '\n' << bsl::flush;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
