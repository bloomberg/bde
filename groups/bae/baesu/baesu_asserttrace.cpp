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
baesu_AssertTrace::LevelCB  baesu_AssertTrace::s_callback;
void                       *baesu_AssertTrace::s_closure;
bael_Severity::Level        baesu_AssertTrace::s_severity =
                                                     bael_Severity::BAEL_FATAL;

// CLASS METHODS
void baesu_AssertTrace::failTrace(const char *text, const char *file, int line)
{
    bael_Severity::Level severity =
             s_callback ? s_callback(s_closure, text, file, line) : s_severity;

    BAEL_LOG_SET_CATEGORY("Assertion.Failure");
    BAEL_LOG_STREAM(severity)
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

void baesu_AssertTrace::getLevelCB(LevelCB *callback, void **closure)
{
    BSLS_ASSERT(callback);
    BSLS_ASSERT(closure);

    *callback = s_callback;
    *closure  = s_closure;
}

void baesu_AssertTrace::setLevelCB(LevelCB callback, void *closure)
{
    s_callback = callback;
    s_closure  = closure;
}

void baesu_AssertTrace::setSeverity(bael_Severity::Level severity)
{
    s_severity = severity;
}

bael_Severity::Level baesu_AssertTrace::severity()
{
    return s_severity;
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
