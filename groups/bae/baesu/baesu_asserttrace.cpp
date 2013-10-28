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
{
};

bsl::ostream& operator<<(bsl::ostream& o, const StackTrace&)
{
    return baesu_StackTracePrintUtil::printStackTrace(o);
}

}  // close unnamed namespace

bael_Severity::Level baesu_AssertTrace::s_severity = bael_Severity::BAEL_FATAL;

void baesu_AssertTrace::setSeverity(bael_Severity::Level severity)
{
    s_severity = severity;
}

bael_Severity::Level baesu_AssertTrace::severity()
{
    return s_severity;
}

void baesu_AssertTrace::failTrace(const char *text, const char *file, int line)
{
    BAEL_LOG_SET_CATEGORY("Assertion.Failure");
    BAEL_LOG_STREAM(s_severity)
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

}  // close enterprise namespace
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
