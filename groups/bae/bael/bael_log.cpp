// bael_log.cpp             -*-C++-*-
#include <bael_log.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_log_cpp,"$Id$ $CSID$")

#include <bael_administration.h>              // for testing only
#include <bael_attributecontainer.h>          // for testing only
#include <bael_attributecontext.h>
#include <bael_attribute.h>                   // for testing only
#include <bael_defaultobserver.h>             // for testing only
#include <bael_defaultattributecontainer.h>
#include <bael_loggermanagerconfiguration.h>  // for testing only
#include <bael_record.h>
#include <bael_testobserver.h>                // for testing only

#include <bcemt_lockguard.h>
#include <bcemt_thread.h>

#include <bsls_assert.h>

#include <bsl_cstdarg.h>
#include <bsl_new.h>
#include <bsl_ostream.h>
#include <stdio.h>  // *NOT* <bsl_cstdio.h>, which does not declare 'vsnprintf'

// See the end of this file for implementation notes.

namespace BloombergLP {

                         // ---------------
                         // struct bael_Log
                         // ---------------

// CLASS METHODS
int bael_Log::format(char        *buffer,
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

bael_Record *bael_Log::getRecord(const bael_Category *category,
                                 const char          *file,
                                 int                  line)
{
    if (category) {
        return bael_LoggerManager::singleton().getLogger().getRecord(file,
                                                                     line);
                                                                      // RETURN
    }
    else {
        return bael_LoggerManager::getRecord(file, line);             // RETURN
    }
}

void bael_Log::logMessage(const bael_Category *category,
                          int                  severity,
                          const char          *fileName,
                          int                  lineNumber,
                          const char          *message)
{
    BSLS_ASSERT(1 <= severity);  BSLS_ASSERT(severity <= 255);
    BSLS_ASSERT(fileName);
    BSLS_ASSERT(message);

    if (category) {
        bael_LoggerManager::singleton().getLogger().logMessage(*category,
                                                               severity,
                                                               fileName,
                                                               lineNumber,
                                                               message);
    }
}

void bael_Log::logMessage(const bael_Category *category,
                          int                  severity,
                          bael_Record          *record)
{
    BSLS_ASSERT(1 <= severity);  BSLS_ASSERT(severity <= 255);

    if (category) {
        bael_LoggerManager::singleton().getLogger().logMessage(*category,
                                                               severity,
                                                               record);
    }
    else {
        bael_LoggerManager::logMessage(severity, record);
    }
}

char *bael_Log::obtainMessageBuffer(bcemt_Mutex **mutex, int *bufferSize)
{
    if (bael_LoggerManager::isInitialized()) {
        return bael_LoggerManager::
                   singleton().getLogger().obtainMessageBuffer(mutex,
                                                               bufferSize);
                                                                      // RETURN
    }
    else {
        return bael_LoggerManager::obtainMessageBuffer(mutex, bufferSize);
                                                                      // RETURN
    }
}

void bael_Log::releaseMessageBuffer(bcemt_Mutex *mutex)
{
    mutex->unlock();
}

const bael_Category *bael_Log::setCategory(const char *categoryName)
{
    BSLS_ASSERT(categoryName);

    return bael_LoggerManager::isInitialized()
         ? bael_LoggerManager::singleton().setCategory(categoryName)
         : 0;
}

void bael_Log::setCategory(bael_CategoryHolder *categoryHolder,
                           const char          *categoryName)
{
    BSLS_ASSERT(categoryName);

    if (bael_LoggerManager::isInitialized()) {
        bael_LoggerManager::singleton().setCategory(categoryHolder,
                                                    categoryName);
    }
}

bool bael_Log::isCategoryEnabled(const bael_CategoryHolder *categoryHolder,
                                 int                        severity)
{
    BSLS_ASSERT(categoryHolder);

    if (bael_CategoryHolder::BAEL_UNINITIALIZED_CATEGORY ==
                                              categoryHolder->threshold() ||
        !bael_LoggerManager::isInitialized()                              ||
        !categoryHolder->category()) {

        // If the category is uninitialized (i.e., the category holder is
        // uninitialized, or the logger manager is uninitialized, or the
        // category is 0) then simply test whether the severity is greater than
        // the WARNING level.

        return bael_Severity::BAEL_WARN >= severity;                  // RETURN
    }

    return bael_LoggerManager::singleton().isCategoryEnabled(
                                                   categoryHolder->category(),
                                                   severity);
}

                     // ---------------------
                     // class bael_Log_Stream
                     // ---------------------

// CREATORS
bael_Log_Stream::bael_Log_Stream(const bael_Category *category,
                                 const char          *fileName,
                                 int                  lineNumber,
                                 int                  severity)
: d_category_p(category)
, d_record_p(bael_Log::getRecord(category, fileName, lineNumber))
, d_severity(severity)
, d_stream(&d_record_p->fixedFields().messageStreamBuf())
{
}

bael_Log_Stream::~bael_Log_Stream()
{
    bael_Log::logMessage(d_category_p, d_severity, d_record_p);
}

                     // ------------------------
                     // class bael_Log_Formatter
                     // ------------------------

// CREATORS
bael_Log_Formatter::bael_Log_Formatter(const bael_Category *category,
                                       const char          *fileName,
                                       int                  lineNumber,
                                       int                  severity)
: d_category_p(category)
, d_record_p(bael_Log::getRecord(category, fileName, lineNumber))
, d_severity(severity)
, d_mutex_p(0)
{
    d_buffer_p = bael_Log::obtainMessageBuffer(&d_mutex_p, &d_bufferLen);
}

bael_Log_Formatter::~bael_Log_Formatter()
{
    d_buffer_p[d_bufferLen - 1] = '\0';
    bcemt_LockGuard<bcemt_Mutex> lockGuard(d_mutex_p, 1);
    d_record_p->fixedFields().setMessage(d_buffer_p);
    bael_Log::logMessage(d_category_p, d_severity, d_record_p);
}

}  // close namespace BloombergLP

///IMPLEMENTATION NOTES
///--------------------
// The stream-style logging macro (without a callback) is reproduced here:
//..
//#define BAEL_LOG_STREAM(BAEL_SEVERITY) {                                   \@
//    using BloombergLP::bael_Log;                                           \@
//    using BloombergLP::bael_Log_Stream;                                    \@
//    using BloombergLP::bael_Severity;                                      \@
//    if (BAEL_LOG_THRESHOLD >= BAEL_SEVERITY) {                             \@
//        if (bael_Log::isCategoryEnabled(&BAEL_LOG_CATEGORYHOLDER,          \@
//                                        BAEL_SEVERITY)) {                  \@
//            bael_Log_Stream bael_lOcAl_StReAm(BAEL_LOG_CATEGORY, __FILE__, \@
//                                              __LINE__, BAEL_SEVERITY);    \@
//            BAEL_STREAM
//..
// Note that '@' is appended to each line in the macro that ends with '\' to
// quell a diagnostic from gcc ("warning: multi-line comment").
//..
// This latest version of the macro, as well as the 'printf'-style macros, are
// based on category holders ('bael_CategoryHolder').  The introduction of
// category holders, and the factoring of the innards of the original macros
// into the new helper classes 'bael_Log_Stream' and 'bael_Log_Formatter',
// yielded non-negligible performance improvements.  In addition, Rule-based
// logging can be released at a later time without requiring additional changes
// to 'bael_log.h' or breaking binary-compatibility.  All the logic in this
// component required to support Rule-based logging will go into the new
// 'bael_Log::isCategoryEnabled' method.
//
// As used in this component, the 'threshold' attributes of category holders
// are initialized to a value outside the range '[0 .. 255]'.  Category holders
// corresponding to "static" categories ('BAEL_LOG_SET_CATEGORY' macro') have
// their thresholds initialized to
// 'bael_CategoryHolder::BAEL_UNINITIALIZED_CATEGORY'.  The thresholds of
// category holders corresponding to "dynamic" categories
// ('BAEL_LOG_SET_DYNAMIC_CATEGORY' macro) are initialized to
// 'bael_CategoryHolder::DYNAMIC_CATEGORY'.  The threshold of a static category
// holder is updated by 'bael_Log::setCategory' *if* the logger manager has
// been initialized and not yet destroyed.  The thresholds of dynamic category
// holders never change from their initial value.
//
// The general idea is that the condition:
//..
//    (BAEL_LOG_THRESHOLD >= BAEL_SEVERITY)
//..
// evaluates to 'true' if there is a *possibility* of a logging event based
// simply on the current threshold of the corresponding category holder
// ('BAEL_LOG_CATEGORYHOLDER').  For static categories, this condition is
// 'true' if the logger manager either has not yet been initialized, or it has
// been destroyed.  The 'isCategoryEnabled' method performs a more-refined
// (but relatively cheap, in the vast majority of cases) analysis of whether a
// record must actually be logged (after possibly calling 'setCategory' on
// 'BAEL_LOG_CATEGORYNAME').  Only if 'isCategoryEnabled' returns 'true' is a
// 'bael_Log_Stream' object constructed (or a 'bael_Log_Formatter' object in
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
// in terms of the 'bael_Log' utility functions.  Note, however, that direct
// use of the utility functions is *strongly* discouraged:
//..
//      static const BloombergLP::bael_Category *category =
//                                   bael_Log::setCategory("EQUITY.NASD.SUNW");
//      {
//          using BloombergLP::bael_Log;
//          using BloombergLP::bael_Severity;
//          if (bael_Log::isEnabled(category, bael_Severity::BAEL_INFO)) {
//              const char *formatSpec = "%d shares of %s sold at %f\n";
//              snprintf(bael_Log::messageBuffer(),
//                       bael_Log::messageBufferSize(),
//                       formatSpec, 400, "SUNW", 5.65);
//              bael_Record *record = bael_Log::getRecord();
//              record->fixedFields().setLineNumber(__LINE__);
//              record->fixedFields().setFileName(__FILE__);
//              record->fixedFields().setMessage(bael_Log::messageBuffer());
//              bael_Log::logMessage(category, bael_Severity::BAEL_INFO,
//                                   record);
//          }
//      }
//..
// Note that it is not necessary to set the filename in this usage scenario;
// instead, '__FILE__' is used directly.  However, a category must still be
// established (first time only), by calling 'bael_Log::setCategory'.  The
// remaining code mimics the expansion of the 'BAEL_LOG3_INFO' macro.  First
// the category is queried to determine if it has logging enabled for the
// 'bael_Severity::BAEL_INFO' severity level.  If so, the message is formatted
// into a static buffer managed by 'bael_LoggerManager' and then logged with
// the call to 'bael_Log::logMessage'.  It is readily apparent from this
// example that using the macros defined in this component is much simpler,
// cleaner, and less error prone as compared to direct use of the utilities.

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
