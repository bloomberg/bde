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
#include <ball_attribute.h>                   // for testing only
#include <ball_attributecontainer.h>          // for testing only
#include <ball_attributecontext.h>
#include <ball_defaultattributecontainer.h>
#include <ball_loggercategoryutil.h>
#include <ball_loggermanagerconfiguration.h>  // for testing only
#include <ball_record.h>
#include <ball_streamobserver.h>              // for testing only
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
int Log::format(char *buffer, bsl::size_t numBytes, const char *format, ...)
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

Record *Log::getRecord(const Category *category, const char *file, int line)
{
    if (category) {
        return LoggerManager::singleton().getLogger().getRecord(file, line);
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
        LoggerManager::singleton().setCategory(categoryHolder, categoryName);
    }
}

const Category *Log::setCategoryHierarchically(CategoryHolder *categoryHolder,
                                               const char     *categoryName)
{
    BSLS_ASSERT(categoryName);

    if (!LoggerManager::isInitialized()) {
        return 0;                                                     // RETURN
    }

    LoggerManager& loggerManager = LoggerManager::singleton();

    // TBD: enable 'addCategoryHierarchically' to apply the name filter and
    // access the thresholds determined by the logger manager's default
    // threshold levels callback, if set and no hierarchical parent category
    // found.

    LoggerCategoryUtil::addCategoryHierarchically(&loggerManager,
                                                  categoryName);

    // The above call returns a 'Category *', but it's not useful to us:
    //: o It may have returned 0
    //:   1 if the category already existed, or
    //:
    //:   2 if the logger manager's category registry was full.
    //:
    //: o The only functions we have access to that will set the category
    //:   holder to a category identify the category by name rather than by
    //:   category pointer.

    // Note that 'setCategory' below can handle the case where
    // '0 == categoryHolder'.  If the logger manager's category registry is
    // full, the default category will be returned.  0 will never be returned.

    return loggerManager.setCategory(categoryHolder, categoryName);
}

bool Log::isCategoryEnabled(const CategoryHolder *categoryHolder, int severity)
{
    BSLS_ASSERT(categoryHolder);

    if (CategoryHolder::e_UNINITIALIZED_CATEGORY == categoryHolder->threshold()
        || !LoggerManager::isInitialized()
        || !categoryHolder->category()) {

        // If the category is uninitialized (i.e., the category holder is in
        // its initial state, the logger manager singleton is uninitialized, or
        // the category is 0), then simply test whether the severity is greater
        // than the WARN level.

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
{}

Log_Formatter::~Log_Formatter()
{
    Log::logMessage(d_category_p, d_severity, d_record_p);
}

// MANIPULATORS
void Log_Formatter::format(const char *format, ...)
{
    enum { BUFFER_SIZE = 8192 };

    char buffer[BUFFER_SIZE];

    bsl::va_list args;
    va_start(args, format);
    const int status = vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);

    // On 'buffer' overflow, most implementations of 'vsnprintf' return the
    // number of characters that would have been written if 'buffer' were large
    // enough.  A few older implementations return -1.  The following code
    // works either way.

    if (status > 0) {
        buffer[BUFFER_SIZE - 1] = '\0';
        d_record_p->fixedFields().setMessage(
                            buffer,
                            BUFFER_SIZE <= status ? BUFFER_SIZE - 1 : status );
    }
}

}  // close package namespace
}  // close enterprise namespace

///IMPLEMENTATION NOTES
///--------------------
// The stream-style logging macro (without a callback) is reproduced here:
//..
//#define BALL_LOG_STREAM_BLOCK(SEVERITY)                                    \@
//for (const BloombergLP::ball::CategoryHolder *ball_log_cAtEgOrYhOlDeR =    \@
//                      ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER); \@
//     ball_log_cAtEgOrYhOlDeR                                               \@
//     && ball_log_cAtEgOrYhOlDeR->threshold() >= (SEVERITY)                 \@
//     && BloombergLP::ball::Log::isCategoryEnabled(ball_log_cAtEgOrYhOlDeR, \@
//                                                  (SEVERITY));             \@
//     )                                                                     \@
//for (BloombergLP::ball::Log_Stream ball_log_lOg_StReAm(                    \@
//                                       ball_log_cAtEgOrYhOlDeR->category(),\@
//                                       __FILE__,                           \@
//                                       __LINE__,                           \@
//                                       (severity));                        \@
//     ball_log_cAtEgOrYhOlDeR;                                              \@
//     ball_log_cAtEgOrYhOlDeR = 0)
//
//#define BALL_LOG_STREAM(SEVERITY)                                          \@
//    BALL_LOG_STREAM_BLOCK((SEVERITY)) BALL_LOG_OUTPUT_STREAM
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
// corresponding to "static" categories ('BALL_LOG_SET_CATEGORY' macro) have
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
//    (ball_log_cAtEgOrYhOlDeR->threshold() >= SEVERITY)
//..
// evaluates to 'true' if there is a *possibility* of a logging event based
// simply on the current threshold of the corresponding category holder
// ('ball_log_cAtEgOrYhOlDeR').  For static categories, this condition is
// 'true' if the logger manager either has not yet been initialized, or it has
// been destroyed.  The 'isCategoryEnabled' method performs a more-refined
// (but relatively cheap, in the vast majority of cases) analysis of whether a
// record must actually be logged.  Only if 'isCategoryEnabled' returns 'true'
// is a 'ball::Log_Stream' object constructed (or a 'ball::Log_Formatter'
// object in the case of the 'printf'-style macros) and a record logged.
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
// remaining code mimics the expansion of the 'BALL_LOGVA_INFO' macro.  First
// the category is queried to determine if it has logging enabled for the
// 'ball::Severity::e_INFO' severity level.  If so, the message is formatted
// into a static buffer managed by 'ball::LoggerManager' and then logged with
// the call to 'ball::Log::logMessage'.  It is readily apparent from this
// example that using the macros defined in this component is much simpler,
// cleaner, and less error prone as compared to direct use of the utilities.
//
///Logging Macro Reuse
///-------------------
// The 'ball_log' macros are crafted so that the *same* logging macros are used
// whether the category being logged to is provided by a block-scope category
// holder ('BALL_LOG_SET_CATEGORY' and 'BALL_LOG_SET_DYNAMIC_CATEGORY') or a
// class-scope category holder ('BALL_LOG_SET_CLASS_CATEGORY').  Two naming
// gimmicks, involving 'ball_log_getCategoryHolder' and
// 'BALL_LOG_CATEGORYHOLDER', facilitate this macro reuse.  The tricks simply
// leverage the usual C++ name lookup and overload resolution rules to make it
// all work.
//
// 'ball_log_getCategoryHolder' names both the lone free function defined in
// the header file and the (overloaded) class methods generated by
// 'BALL_LOG_SET_CLASS_CATEGORY'.  'BALL_LOG_CATEGORYHOLDER' names
// both the category holders generated by 'BALL_LOG_SET_CATEGORY' and
// 'BALL_LOG_SET_DYNAMIC_CATEGORY', and the class-scope enumerators generated
// by 'BALL_LOG_SET_CLASS_CATEGORY'.
//
// Given that, note that all calls to 'ball_log_getCategoryHolder' that occur
// in 'ball_log' macros are of the form:
//..
//  ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER)
//..
// There are two cases to consider.
//
// First suppose that 'ball_log_getCategoryHolder' is called from a free
// function or free operator, or from a method of a class that does *not* use
// 'BALL_LOG_SET_CLASS_CATEGORY'.  In this case, 'ball_log_getCategoryHolder'
// resolves to the free function and the 'BALL_LOG_CATEGORYHOLDER' that is
// supplied must necessarily be the category holder provided by the "nearest"
// use of 'BALL_LOG_SET_CATEGORY' or 'BALL_LOG_SET_DYNAMIC_CATEGORY'.
//
// The more interesting case is where 'ball_log_getCategoryHolder' is called
// from a method of a class that *does* use 'BALL_LOG_SET_CLASS_CATEGORY'.  In
// this case, 'ball_log_getCategoryHolder' resolves to the class method, with
// two sub-cases to consider.
//
// If the class-scope 'BALL_LOG_CATEGORYHOLDER' enumerator is hidden by a
// like-named category holder from an expansion of 'BALL_LOG_SET_CATEGORY' or
// 'BALL_LOG_SET_DYNAMIC_CATEGORY', then that holder is passed to the
// 'ball_log_getCategoryHolder' class method taking 'ball::CategoryHolder&'.
// That method simply returns the (address of) the holder and the block-scope
// category is logged to.
//
// If the class-scope 'BALL_LOG_CATEGORYHOLDER' enumerator is *not* hidden,
// then the 'ball_log_getCategoryHolder' class method taking 'int' is invoked
// instead.  That method, which ignores its argument, returns the (address of)
// the class-scope holder that is housed within the method and the class-scope
// category is logged to.

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
