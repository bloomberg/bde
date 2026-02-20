// ball_recordformatterregistryutil.h                                 -*-C++-*-
#ifndef INCLUDED_BALL_RECORDFORMATTERREGISTRYUTIL
#define INCLUDED_BALL_RECORDFORMATTERREGISTRYUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for creating log record formatters by scheme.
//
//@CLASSES:
//  ball::RecordFormatterRegistryUtil: utility for formatter creation
//
//@SEE_ALSO: ball_recordstringformatter, ball_recordjsonformatter
//
//@DESCRIPTION: This component provides a utility `struct`,
// `ball::RecordFormatterRegistryUtil`, that defines a namespace for functions
// to create log record formatters based on URI-like scheme identifiers.  The
// primary function, `createRecordFormatter`, interprets a format string
// beginning with a scheme (e.g., "text://", "json://", "qjson://") and
// delegates to the appropriate specialized formatter factory.  This allows
// for flexible configuration of log output formats using a single,
// scheme-based interface.
//
// The component supports the following schemes:
// * **text**: Creates a text-based formatter using
//   `ball::RecordStringFormatter`
// * **json**: Creates a JSON formatter using `ball::RecordJsonFormatter`
// * **qjson**: Creates a quoted JSON formatter using
//   `ball::RecordJsonFormatter`
//
// The scheme determines which formatter will be used and the syntax of the
// format specification.  The following schemes are currently supported: text,
// json, qjson.  See [Scheme-Based Formatters](ball#Scheme-Based Formatters)
// for more details of the supported schemes and their accompanying format
// specification syntaxes.
//
///Behavior with Invalid Formats
///-----------------------------
// Trying to create a formatter with an invalid format string or an unknown
// scheme will result in a non-zero error code being returned.  If the `result`
// formatter is not empty the method assumes that it is loaded with a
// reasonable (default or otherwise) format and will not change it.  In
// case `result` is an `empty` `bsl::function` `createRecordFormatter` will
// return a non-zero error code and loads a default/fallback formatter into the
// `result` parameter.  Said fallback formatter will be a sensible default for
// the given scheme if the scheme is recognized, or a default/fallback text
// formatter if the scheme is not one of the supported ones.
//
// An example with an unknown scheme:
// ```
//  bsl::string unknownScheme = "xml://some-format";
//  ball::RecordFormatterFunctor::Type fallbackFormatter;  // empty formatter
//  rc = ball::RecordFormatterRegistryUtil::createRecordFormatter(
//                                                        &fallbackFormatter,
//                                                        unknownScheme,
//                                                        options);
//
//  assert(0 != rc);  // Returns error code for unknown scheme
// ```
// `fallbackFormatter` is still usable as it was loaded with a sensible default
// text formatter, allowing the application to continue logging even when the
// configuration is incorrect.
// ```
//  bsl::ostringstream fallbackOss;
//  fallbackFormatter(fallbackOss, record);
//  assert(!fallbackOss.str().empty());  // Fallback formatter still works:
// ```
// The output of the above code will look something like:
// ```
// 27AUG2007_16:09:46.161 2040:1 WARN adir/src.cpp:97 CATEGORY Log message\n
// ```
// Currently the text formatter just ignores errors in its format string so
// only the "json://" and "qjson://" schemes perform some form of validation.
// That means that a meaningless "text://" format will succeed and may not
// produce any useful log output,  while some invalid "json://" or "qjson://"
// formats will fall back to a reasonable default with a non-zero return code.
// ```
//  bsl::string unknownScheme = "test://%73\n";
//  ball::RecordFormatterFunctor::Type fallbackFormatter;  // empty formatter
//  rc = ball::RecordFormatterRegistryUtil::createRecordFormatter(
//                                                        &fallbackFormatter,
//                                                        unknownScheme,
//                                                        options);
//
//  assert(0 == rc);  // The text scheme never reports an error
//
//  bsl::ostringstream oss;
//  fallbackFormatter(oss, record);
//  assert("%73\n" == oss.str());  // Gibberish output due to set format
// ```
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating Formatters from Various Scheme URIs
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have configuration entries that define log record formatters
// using URI-like schemes, and we need to create the corresponding formatters.
// This component supports "text://", "json://", and "qjson://" schemes.
//
// First, we create formatter options and a test record:
// ```
//  ball::RecordFormatterOptions options(
//                                   ball::RecordFormatterTimezone::e_UTC);
//  ball::Record record = createTestRecord();
// ```
// Now, we demonstrate creating formatters with different schemes.  For text
// output, we use the "text://" scheme with printf-style format specifiers:
// ```
//  bsl::string textFormat = "text://%d %p:%t %s %f:%l %c %m\n";
//  ball::RecordFormatterFunctor::Type textFormatter;
//  int rc = ball::RecordFormatterRegistryUtil::createRecordFormatter(
//                                                            &textFormatter,
//                                                            textFormat,
//                                                            options);
//  assert(0 == rc);
// ```
// For structured JSON output, we use the "json://" scheme with a JSON array
// specifying which fields to include:
// ```
//  bsl::string jsonFormat = "json://[\"tid\",\"severity\",\"message\"]";
//  ball::RecordFormatterFunctor::Type jsonFormatter;
//  rc = ball::RecordFormatterRegistryUtil::createRecordFormatter(
//                                                            &jsonFormatter,
//                                                            jsonFormat,
//                                                            options);
//  assert(0 == rc);
// ```
// For simplified JSON using printf-style specifiers, we use the "qjson://"
// scheme:
// ```
//  bsl::string qjsonFormat = "qjson://%i %s %c %m";
//  ball::RecordFormatterFunctor::Type qjsonFormatter;
//  rc = ball::RecordFormatterRegistryUtil::createRecordFormatter(
//                                                           &qjsonFormatter,
//                                                           qjsonFormat,
//                                                           options);
//  assert(0 == rc);
// ```
// Each formatter can then be used to format log records:
// ```
//  bsl::ostringstream textOss, jsonOss, qjsonOss;
//  textFormatter(textOss, record);
//  jsonFormatter(jsonOss, record);
//  qjsonFormatter(qjsonOss, record);
//
//  assert(textOss.str().find("Test message")  != bsl::string::npos);
//  assert(jsonOss.str().find("\"message\"")   != bsl::string::npos);
//  assert(qjsonOss.str().find("Test message") != bsl::string::npos);
// ```

#include <balscm_version.h>

#include <ball_recordformatterfunctor.h>
#include <ball_recordformatteroptions.h>

#include <bslma_allocator.fwd.h>

#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {

namespace ball {

class Record;

                    // ==================================
                    // struct RecordFormatterRegistryUtil
                    // ==================================

/// This utility provides a namespace for functions to create log record
/// formatters based on URI-like scheme identifiers.
struct RecordFormatterRegistryUtil {

    // CONSTANTS
    static const bsl::string_view k_FALLBACK_LOG_FORMAT_URI;

    // CLASS METHODS

    /// Load, into the specified `result` a formatter functor that formats as
    /// instructed by the specified `format` and `formatOptions`.  The `format`
    /// may start with a URI-like scheme (e.g., `"qjson://"`) that selects both
    /// the formatter (class) and the syntax of the format specification that
    /// follows the scheme.  In case `format` does not start with a scheme the
    /// "text://" scheme is assumed.  Return 0 if the requested log record
    /// formatter was successfully created and loaded into `result`.
    /// Otherwise, if `results is empty, load a default/fallback-formatter into
    /// `result`, with the requested scheme if that exists or with a sensible
    /// fallback text log format otherwise.  For further details on behavior
    /// in case of errors see {Behavior with Invalid Formats}.  This method
    /// currently supports the "text", "qjson", and "json" schemes.
    static int createRecordFormatter(
                              RecordFormatterFunctor::Type  *result,
                              const bsl::string_view&        format,
                              const RecordFormatterOptions&  formatOptions);
};

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BALL_RECORDFORMATTERREGISTRYUTIL

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
