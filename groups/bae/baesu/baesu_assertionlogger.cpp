// baesu_assertionlogger.cpp                                          -*-C++-*-
#include <baesu_assertionlogger.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_assertionlogger_cpp,"$Id$ $CSID$")

#include <bael_log.h>
#include <baesu_stacktraceutil.h>

#include <bsls_atomicoperations.h>

#include <bsl_ostream.h>  // ostream
#include <bsl_cstring.h>  // memcpy

namespace BloombergLP {

namespace {

struct StackTrace
    // This 'struct' acts as a tag for the output operator below.
{
};

bsl::ostream& operator<<(bsl::ostream& stream, const StackTrace&)
    // Print a stack trace to the specified 'stream'.
{
    return baesu_StackTraceUtil::hexStackTrace(stream);
}

// STATIC DATA
bsls::AtomicOperations::AtomicTypes::Pointer s_callback = { };
    // Callback function invoked on assertion failure.

bsls::AtomicOperations::AtomicTypes::Pointer s_closure  = { };
    // Closure value passed back to callback function.

bsls::AtomicOperations::AtomicTypes::Int s_severity = {
    bael_Severity::BAEL_FATAL
};
    // Severity level used when no callback is installed.

}  // close unnamed namespace

// CLASS METHODS
void baesu_AssertionLogger::assertionFailureHandler(const char *text,
                                                    const char *file,
                                                    int         line)
{
    LogSeverityCallback  callback;
    void                *closure;

    getLogSeverityCallback(&callback, &closure);
    bael_Severity::Level level =
         callback ? callback(closure, text, file, line) : defaultLogSeverity();

    if (level > bael_Severity::BAEL_OFF) {
        BAEL_LOG_SET_CATEGORY("Assertion.Failure");
        BAEL_LOG_STREAM(level)
            << "Assertion failed: "
            << (! text ? "(* Unspecified Expression Text *)" :
                !*text ? "(* Empty Expression Text *)"       :
                  text)
            << ", file "
            << (! file ? "(* Unspecified File Name *)" :
                !*file ? "(* Empty File Name *)"       :
                  file)
            << ", line "
            << line
            << "\n"
            << "For stack trace, run 'showfunc.tsk <your_program_binary> "
            << StackTrace()
            << "'\n"
        << BAEL_LOG_END
    }
}

void
baesu_AssertionLogger::getLogSeverityCallback(LogSeverityCallback  *callback,
                                              void                **closure)
{
    BSLS_ASSERT(callback);
    BSLS_ASSERT(closure);

    *closure  = bsls::AtomicOperations::getPtrAcquire(&s_closure);

    // Work around warning about converting between data and function pointers.

    void *cb = bsls::AtomicOperations::getPtrAcquire(&s_callback);

    BSLS_ASSERT(sizeof(LogSeverityCallback) == sizeof cb);

    bsl::memcpy(reinterpret_cast<unsigned char *>(callback),
                reinterpret_cast<unsigned char *>(&cb),
                sizeof *callback);
}

void
baesu_AssertionLogger::setLogSeverityCallback(LogSeverityCallback  callback,
                                              void                *closure)
{
    bsls::AtomicOperations::setPtrRelease(&s_closure, closure);

    // Work around warning about converting between data and function pointers.

    void *cb;

    BSLS_ASSERT(sizeof(LogSeverityCallback) == sizeof cb);

    memcpy(reinterpret_cast<unsigned char *>(&cb),
           reinterpret_cast<unsigned char *>(&callback),
           sizeof(cb));
    bsls::AtomicOperations::setPtrRelease(&s_callback, cb);
}

void
baesu_AssertionLogger::setDefaultLogSeverity(bael_Severity::Level severity)
{
    bsls::AtomicOperations::setIntRelaxed(&s_severity, severity);
}

bael_Severity::Level baesu_AssertionLogger::defaultLogSeverity()
{
    return static_cast<bael_Severity::Level>(
                           bsls::AtomicOperations::getIntRelaxed(&s_severity));
}

}  // close enterprise namespace
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
