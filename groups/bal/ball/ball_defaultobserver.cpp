// ball_defaultobserver.cpp                                           -*-C++-*-
#include <ball_defaultobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_defaultobserver_cpp,"$Id$ $CSID$")

#include <ball_context.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>

#include <bdlqq_lockguard.h>

#include <bdlmxxx_list.h>

#include <bsl_ostream.h>

#include <bslstl_stringref.h>

namespace BloombergLP {

namespace ball {
                           // --------------------------
                           // class DefaultObserver
                           // --------------------------

// CREATORS
DefaultObserver::~DefaultObserver()
{
}

// MANIPULATORS
void DefaultObserver::publish(const Record&  record,
                                   const Context&)
{
    bdlqq::LockGuard<bdlqq::Mutex> guard(&d_mutex);

    const RecordAttributes& fixedFields = record.fixedFields();

    // convert the severity level to ASCII value for publishing
    Severity::Level severityLevel =
                                  (Severity::Level)fixedFields.severity();

    *d_stream << '\n';

    *d_stream << fixedFields.timestamp()               << ' '
              << fixedFields.processID()               << ' '
              << fixedFields.threadID()                << ' '
              << Severity::toAscii(severityLevel) << ' '
              << fixedFields.fileName()                << ' '
              << fixedFields.lineNumber()              << ' '
              << fixedFields.category()                << ' ';

    bslstl::StringRef message = fixedFields.messageRef();
    d_stream->write(message.data(), message.length());

    *d_stream << ' ';

    const bdlmxxx::List& userFields = record.userFields();
    const int numUserFields = userFields.length();
    for (int i = 0; i < numUserFields; ++i) {
        *d_stream << userFields[i] << ' ';
    }

    *d_stream << '\n' << bsl::flush;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
