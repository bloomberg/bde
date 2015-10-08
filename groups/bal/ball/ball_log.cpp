// ball_log.cpp                                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_log.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_log_cpp,"$Id$ $CSID$")

#include <ball_administration.h>              // for testing only
#include <ball_attributecontainer.h>          // for testing only
#include <ball_attributecontext.h>
#include <ball_attribute.h>                   // for testing only
#include <ball_defaultobserver.h>             // for testing only
#include <ball_defaultattributecontainer.h>
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_record.h>
#include <ball_testobserver.h>                // for testing only

#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>

#include <bsls_assert.h>

#include <bsl_cstdarg.h>
#include <bsl_new.h>
#include <bsl_ostream.h>
#include <bsl_cstdio.h>
#include <stdio.h>  // *NOT* <bsl_cstdio.h>, which does not declare 'vsnprintf'

// See the end of this file for implementation notes.

namespace BloombergLP {

namespace ball {
                         // ----------
                         // struct Log
                         // ----------

// CLASS METHODS
int Log::format(char        *buffer,
                bsl::size_t  numBytes,
                const char  *format,
                             ...)
{
    bsl::va_list args;
    va_start(args, format);
    const int status = vsnprintf(buffer, numBytes, format, args);
    va_end(args);

    // On 'buffer' overflow, most implementations of 'vsnprintf' return the
    // number of characters that would have been written if 'buffer' were large
    // enough.  A few older implementations return -1.  The following code
    // works either way.

    // Note that 'numBytes' is cast to 'signed' to eliminate warnings about
    // 'comparison between signed and unsigned'.

    return (signed)numBytes <= status ? -1 : status;
}

Record *Log::getRecord(const Category *category,
                       const char     *file,
                       int             line)
{
    if (category) {
        return LoggerManager::singleton().getLogger().getRecord(file,
                                                                     line);
                                                                      // RETURN
    }
    else {
        return LoggerManager::getRecord(file, line);                  // RETURN
    }
}

void Log::logMessage(const Category *category,
                     int             severity,
                     const char     *fileName,
                     int             lineNumber,
                     const char     *message)
{
    BSLS_ASSERT(1 <= severity);  BSLS_ASSERT(severity <= 255);
    BSLS_ASSERT(fileName);
    BSLS_ASSERT(message);

    if (category) {
        LoggerManager::singleton().getLogger().logMessage(*category,
                                                          severity,
                                                          fileName,
                                                          lineNumber,
                                                          message);
    }
}

void Log::logMessage(const Category *category,
                     int             severity,
                     Record         *record)
{
    BSLS_ASSERT(1 <= severity);  BSLS_ASSERT(severity <= 255);

    if (category) {
        LoggerManager::singleton().getLogger().logMessage(*category,
                                                               severity,
                                                               record);
    }
    else {
        LoggerManager::logMessage(severity, record);
    }
}

char *Log::obtainMessageBuffer(bslmt::Mutex **mutex, int *bufferSize)
{
    if (LoggerManager::isInitialized()) {
        return LoggerManager::
                   singleton().getLogger().obtainMessageBuffer(mutex,
                                                               bufferSize);
                                                                      // RETURN
    }
    else {
        return LoggerManager::obtainMessageBuffer(mutex, bufferSize);
                                                                      // RETURN
    }
}

void Log::releaseMessageBuffer(bslmt::Mutex *mutex)
{
    mutex->unlock();
}

const Category *Log::setCategory(const char *categoryName)
{
    BSLS_ASSERT(categoryName);

    return LoggerManager::isInitialized()
         ? LoggerManager::singleton().setCategory(categoryName)
         : 0;
}

void Log::setCategory(CategoryHolder *categoryHolder,
                      const char     *categoryName)
{
    BSLS_ASSERT(categoryName);

    if (LoggerManager::isInitialized()) {
        LoggerManager::singleton().setCategory(categoryHolder,
                                               categoryName);
    }
}

bool Log::isCategoryEnabled(const CategoryHolder *categoryHolder,
                            int                   severity)
{
    BSLS_ASSERT(categoryHolder);

    if (CategoryHolder::e_UNINITIALIZED_CATEGORY ==
                                              categoryHolder->threshold() ||
        !LoggerManager::isInitialized()                                   ||
        !categoryHolder->category()) {

        // If the category is uninitialized (i.e., the category holder is
        // uninitialized, or the logger manager is uninitialized, or the
        // category is 0) then simply test whether the severity is greater than
        // the WARNING level.

        return Severity::e_WARN >= severity;                          // RETURN
    }

    return LoggerManager::singleton().isCategoryEnabled(
                                                   categoryHolder->category(),
                                                   severity);
}

                     // ----------------
                     // class Log_Stream
                     // ----------------

// CREATORS
Log_Stream::Log_Stream(const Category *category,
                       const char     *fileName,
                       int             lineNumber,
                       int             severity)
: d_category_p(category)
, d_record_p(Log::getRecord(category, fileName, lineNumber))
, d_severity(severity)
, d_stream(&d_record_p->fixedFields().messageStreamBuf())
{
}

Log_Stream::~Log_Stream()
{
    Log::logMessage(d_category_p, d_severity, d_record_p);
}

                     // -------------------
                     // class Log_Formatter
                     // -------------------

// CREATORS
Log_Formatter::Log_Formatter(const Category *category,
                             const char     *fileName,
                             int             lineNumber,
                             int             severity)
: d_category_p(category)
, d_record_p(Log::getRecord(category, fileName, lineNumber))
, d_severity(severity)
, d_mutex_p(0)
{
    d_buffer_p = Log::obtainMessageBuffer(&d_mutex_p, &d_bufferLen);
}

Log_Formatter::~Log_Formatter()
{
    d_buffer_p[d_bufferLen - 1] = '\0';
    bslmt::LockGuard<bslmt::Mutex> lockGuard(d_mutex_p, 1);
    d_record_p->fixedFields().setMessage(d_buffer_p);
    Log::logMessage(d_category_p, d_severity, d_record_p);
}

}  // close package namespace

}  // close enterprise namespace

///IMPLEMENTATION NOTES
///--------------------
// The stream-style logging macro (without a callback) is reproduced here:
//..
//#define BALL_LOG_STREAM(BALL_SEVERITY) {                                   \@
//    using BloombergLP::ball::Log;                                          \@
//    using BloombergLP::ball::Log_Stream;                                   \@
//    using BloombergLP::ball::Severity;                                     \@
//    if (BALL_LOG_THRESHOLD >= BALL_SEVERITY) {                             \@
//        if (ball::Log::isCategoryEnabled(&BALL_LOG_CATEGORYHOLDER,         \@
//                                        BALL_SEVERITY)) {                  \@
//            ball::Log_Stream ball_lOcAl_StReAm(BALL_LOG_CATEGORY, __FILE__,\@
//                                              __LINE__, BALL_SEVERITY);    \@
//            BALL_STREAM
//..
// Note that '@' is appended to each line in the macro that ends with '\' to
// quell a diagnostic from gcc ("warning: multi-line comment").
//..
// This latest version of the macro, as well as the 'printf'-style macros, are
// based on category holders ('ball::CategoryHolder').  The introduction of
// category holders, and the factoring of the innards of the original macros
// into the new helper classes 'ball::Log_Stream' and 'ball::Log_Formatter',
// yielded non-negligible performance improvements.  In addition, Rule-based
// logging can be released at a later time without requiring additional changes
// to 'ball_log.h' or breaking binary-compatibility.  All the logic in this
// component required to support Rule-based logging will go into the new
// 'ball::Log::isCategoryEnabled' method.
//
// As used in this component, the 'threshold' attributes of category holders
// are initialized to a value outside the range '[0 .. 255]'.  Category holders
// corresponding to "static" categories ('BALL_LOG_SET_CATEGORY' macro') have
// their thresholds initialized to
// 'ball::CategoryHolder::e_UNINITIALIZED_CATEGORY'.  The thresholds of
// category holders corresponding to "dynamic" categories
// ('BALL_LOG_SET_DYNAMIC_CATEGORY' macro) are initialized to
// 'ball::CategoryHolder::DYNAMIC_CATEGORY'.  The threshold of a static
// category holder is updated by 'ball::Log::setCategory' *if* the logger
// manager has been initialized and not yet destroyed.  The thresholds of
// dynamic category holders never change from their initial value.
//
// The general idea is that the condition:
//..
//    (BALL_LOG_THRESHOLD >= BALL_SEVERITY)
//..
// evaluates to 'true' if there is a *possibility* of a logging event based
// simply on the current threshold of the corresponding category holder
// ('BALL_LOG_CATEGORYHOLDER').  For static categories, this condition is
// 'true' if the logger manager either has not yet been initialized, or it has
// been destroyed.  The 'isCategoryEnabled' method performs a more-refined
// (but relatively cheap, in the vast majority of cases) analysis of whether a
// record must actually be logged (after possibly calling 'setCategory' on
// 'BALL_LOG_CATEGORYNAME').  Only if 'isCategoryEnabled' returns 'true' is a
// 'ball::Log_Stream' object constructed (or a 'ball::Log_Formatter' object in
// the case of the 'printf'-style macros) and a record logged.
//
// Note that the above condition is *always* 'true' for dynamic categories.
// Since category holders for dynamic categories are not 'static', as are
// category holders for static categories, dynamic category holders cannot be
// placed on the linked list maintained by the category without adding undue
// complexity to the implementation (e.g., by introducing a scoped guard to
// remove the dynamic category holder from the linked list on destruction).
//
///UTILITY USAGE
///-------------
// (This USAGE section was originally in the component header file, but
// was moved here so as not to encourage direct use of the utility functions.)
//
// The following example recasts the 'snprintf'-based macro usage shown above
// in terms of the 'ball::Log' utility functions.  Note, however, that direct
// use of the utility functions is *strongly* discouraged:
//..
//      static const BloombergLP::ball::Category *category =
//                                  ball::Log::setCategory("EQUITY.NASD.SUNW");
//      {
//          using BloombergLP::ball::Log;
//          using BloombergLP::ball::Severity;
//          if (ball::Log::isEnabled(category, ball::Severity::e_INFO)) {
//              const char *formatSpec = "%d shares of %s sold at %f\n";
//              snprintf(ball::Log::messageBuffer(),
//                       ball::Log::messageBufferSize(),
//                       formatSpec, 400, "SUNW", 5.65);
//              ball::Record *record = ball::Log::getRecord();
//              record->fixedFields().setLineNumber(__LINE__);
//              record->fixedFields().setFileName(__FILE__);
//              record->fixedFields().setMessage(ball::Log::messageBuffer());
//              ball::Log::logMessage(category, ball::Severity::e_INFO,
//                                   record);
//          }
//      }
//..
// Note that it is not necessary to set the filename in this usage scenario;
// instead, '__FILE__' is used directly.  However, a category must still be
// established (first time only), by calling 'ball::Log::setCategory'.  The
// remaining code mimics the expansion of the 'BALL_LOG3_INFO' macro.  First
// the category is queried to determine if it has logging enabled for the
// 'ball::Severity::e_INFO' severity level.  If so, the message is formatted
// into a static buffer managed by 'ball::LoggerManager' and then logged with
// the call to 'ball::Log::logMessage'.  It is readily apparent from this
// example that using the macros defined in this component is much simpler,
// cleaner, and less error prone as compared to direct use of the utilities.

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
