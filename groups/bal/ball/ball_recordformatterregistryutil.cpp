// ball_recordformatterregistryutil.cpp                               -*-C++-*-
#include <ball_recordformatterregistryutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_recordformatterregistryutil_cpp, "$Id$ $CSID$")

#include <ball_record.h>                  // for testing only
#include <ball_recordattributes.h>        // for testing only
#include <ball_recordjsonformatter.h>
#include <ball_recordstringformatter.h>
#include <ball_severity.h>                // for testing only
#include <ball_userfields.h>              // for testing only

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace ball {

// CONSTANTS
const bsl::string_view RecordFormatterRegistryUtil::k_FALLBACK_LOG_FORMAT_URI =
                                         "text://%d %p:%t %s %f:%l %c %a %m\n";

                     // ---------------------------------
                     // class RecordFormatterRegistryUtil
                     // ---------------------------------

// CLASS METHODS
int RecordFormatterRegistryUtil::createRecordFormatter(
                               RecordFormatterFunctor::Type  *result,
                               const bsl::string_view&        format,
                               const RecordFormatterOptions&  formatOptions)
{
    // Get scheme from start of config
    typedef bsl::string::size_type SizeType;

    const SizeType schemeEndFind = format.find("://");
    const bool     noScheme      = (bsl::string::npos == schemeEndFind);

    const bsl::string_view scheme = noScheme
                                 ? "text"
                                 : format.substr(0, schemeEndFind);
    const bsl::string_view rawFormat =
                               format.substr(noScheme ? 0 : schemeEndFind + 3);

    RecordFormatterFunctor::Type tempFormatter(bsl::allocator_arg,
                                               result->get_allocator());

    int rc = -42;
    if ("text" == scheme) {
        rc = RecordStringFormatter::loadTextSchemeFormatter(
                                                     &tempFormatter,
                                                     rawFormat,
                                                     formatOptions);  // RETURN
    }
    else if ("json" == scheme) {
        rc = RecordJsonFormatter::loadJsonSchemeFormatter(
                                                     &tempFormatter,
                                                     rawFormat,
                                                     formatOptions);  // RETURN
    }
    else if ("qjson" == scheme) {
        rc = RecordJsonFormatter::loadQjsonSchemeFormatter(
                                                     &tempFormatter,
                                                     rawFormat,
                                                     formatOptions);  // RETURN
    }
    else {
        rc = 404;  // Unknown scheme error code

        if (!*result) {
            // In case of error, if 'result' is empty, we need to provide a
            // fallback so logging will output something reasonable.  If it is not
            // empty we assume it has a reasonable config already.
            const int rc2 = createRecordFormatter(&tempFormatter,
                                                k_FALLBACK_LOG_FORMAT_URI,
                                                RecordFormatterOptions());
            BSLS_ASSERT(0 == rc2);  (void)rc2;
        }
    }

    if ((0 == rc) || !*result) {
        result->swap(tempFormatter);
    }

    return rc;
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
