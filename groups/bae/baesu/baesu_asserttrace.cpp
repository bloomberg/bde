// baesu_asserttrace.cpp                                              -*-C++-*-
#include <baesu_asserttrace.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_asserttrace_cpp,"$Id$ $CSID$")

#include <bael_log.h>
#include <baesu_stacktraceprintutil.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace {

struct StackTrace
    // This 'struct' acts as a tag for the output operator below.
{
};

bsl::ostream& operator<<(bsl::ostream& stream, const StackTrace&)
    // Print a statck trace to the specified 'stream'.
{
    return baesu_StackTracePrintUtil::printStackTrace(stream);
}

}  // close unnamed namespace

// CLASS DATA
bsls::AtomicOperations::AtomicTypes::Pointer
    baesu_AssertTrace::s_callback = { };
bsls::AtomicOperations::AtomicTypes::Pointer
    baesu_AssertTrace::s_closure  = { };
bsls::AtomicOperations::AtomicTypes::Int
    baesu_AssertTrace::s_severity = { bael_Severity::BAEL_FATAL };

// CLASS METHODS
void baesu_AssertTrace::failTrace(const char *text, const char *file, int line)
{
    LevelCB  callback;
    void    *closure;

    getLevelCB(&callback, &closure);
    bael_Severity::Level level =
                   callback ? callback(closure, text, file, line) : severity();

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
            << StackTrace()
        << BAEL_LOG_END
    }
}

void baesu_AssertTrace::getLevelCB(LevelCB *callback, void **closure)
{
    BSLS_ASSERT(callback);
    BSLS_ASSERT(closure);

    *closure  = bsls::AtomicOperations::getPtrAcquire(&s_closure);

    // Work around warning about converting between data and function pointers.

    void *cb = bsls::AtomicOperations::getPtrAcquire(&s_callback);

    BSLS_ASSERT(sizeof(LevelCB) == sizeof cb);

    memcpy(reinterpret_cast<unsigned char *>(callback),
           reinterpret_cast<unsigned char *>(&cb),
           sizeof *callback);
}

void baesu_AssertTrace::setLevelCB(LevelCB callback, void *closure)
{
    bsls::AtomicOperations::setPtrRelease(&s_closure, closure);

    // Work around warning about converting between data and function pointers.

    void *cb;

    BSLS_ASSERT(sizeof(LevelCB) == sizeof cb);

    memcpy(reinterpret_cast<unsigned char *>(&cb),
           reinterpret_cast<unsigned char *>(&callback),
           sizeof(cb));
    bsls::AtomicOperations::setPtrRelease(&s_callback, cb);
}

void baesu_AssertTrace::setSeverity(bael_Severity::Level severity)
{
    bsls::AtomicOperations::setIntRelease(&s_severity, severity);
}

bael_Severity::Level baesu_AssertTrace::severity()
{
    return static_cast<bael_Severity::Level>(
                           bsls::AtomicOperations::getIntAcquire(&s_severity));
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
