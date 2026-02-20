// ball_cstdioobserver.cpp                                            -*-C++-*-
#include <ball_cstdioobserver.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_cstdioobserver_cpp,"$Id$ $CSID$")

#include <ball_context.h>                // for testing only
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_recordformatterregistryutil.h>
#include <ball_severity.h>
#include <ball_recordformattertimezone.h>
#include <ball_userfields.h>
#include <ball_userfieldvalue.h>

#include <bdlt_datetime.h>

#include <bslmt_lockguard.h>

#include <bslstl_stringref.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace ball {

namespace {

static const bsl::string_view k_DEFAULT_FORMAT =
                                              "\n%d %p %t %s %f %l %c %m %u\n";
}  // close unnamed namespace

                           // --------------------
                           // class CstdioObserver
                           // --------------------

// CREATORS
CstdioObserver::CstdioObserver(FILE *stream, const allocator_type& allocator)
: d_file_p(stream)
, d_mutex()
, d_observerFormatterImp(k_DEFAULT_FORMAT,
                         RecordFormatterTimezone::e_UTC,
                         allocator)
{
    BSLS_ASSERT(d_file_p);
}

CstdioObserver::~CstdioObserver()
{
}

// MANIPULATORS
void CstdioObserver::publish(const bsl::shared_ptr<const Record>& record,
                             const Context&)
{
    BSLS_ASSERT(record);

    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    bsl::ostringstream oss;
    d_observerFormatterImp.formatLogRecord(oss, record);

    // Use 'fwrite' to specify the length to write.

    bsl::fwrite(oss.str().c_str(), 1, oss.str().length(), d_file_p);
    bsl::fflush(d_file_p);
}

void CstdioObserver::setFormatFunctor(const RecordFormatter& formatter)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);

    d_observerFormatterImp.setFormatFunctor(formatter);
}

int CstdioObserver::setFormat(const bsl::string_view& format)
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_observerFormatterImp.setFormat(format);
}

void CstdioObserver::disablePublishInLocalTime()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    d_observerFormatterImp.setTimezoneDefault(RecordFormatterTimezone::e_UTC);
}

void CstdioObserver::enablePublishInLocalTime()
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    d_observerFormatterImp.setTimezoneDefault(
                                           RecordFormatterTimezone::e_LOCAL);
}

// ACCESSORS
bool CstdioObserver::isPublishInLocalTimeEnabled() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return RecordFormatterTimezone::e_LOCAL ==
                               d_observerFormatterImp.getTimezoneDefault();
}

const bsl::string& CstdioObserver::getFormat() const
{
    bslmt::LockGuard<bslmt::Mutex> guard(&d_mutex);
    return d_observerFormatterImp.getFormat();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
