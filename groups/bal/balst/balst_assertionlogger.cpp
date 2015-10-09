// balst_assertionlogger.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_assertionlogger.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_assertionlogger_cpp,"$Id$ $CSID$")

#include <ball_log.h>
#include <balst_stacktraceutil.h>

#include <bsls_atomicoperations.h>

#include <bsl_ostream.h>  // ostream
#include <bsl_cstring.h>  // memcpy

namespace BloombergLP {

namespace {

struct u_StackTrace
    // This 'struct' acts as a tag for the output operator below.
{
};

bsl::ostream& operator<<(bsl::ostream& stream, const u_StackTrace&)
    // Print a stack trace to the specified 'stream'.
{
    return balst::StackTraceUtil::hexStackTrace(stream);
}

// STATIC DATA
bsls::AtomicOperations::AtomicTypes::Pointer s_callback = { 0 };
    // Callback function invoked on assertion failure.

bsls::AtomicOperations::AtomicTypes::Pointer s_closure  = { 0 };
    // Closure value passed back to callback function.

bsls::AtomicOperations::AtomicTypes::Int s_severity = {
    ball::Severity::e_FATAL
};
    // Severity level used when no callback is installed.

}  // close unnamed namespace

namespace balst {
// CLASS METHODS
void AssertionLogger::assertionFailureHandler(const char *text,
                                              const char *file,
                                              int         line)
{
    LogSeverityCallback  callback;
    void                *closure;

    getLogSeverityCallback(&callback, &closure);
    ball::Severity::Level level =
         callback ? callback(closure, text, file, line) : defaultLogSeverity();

    if (level > ball::Severity::e_OFF) {
        BALL_LOG_SET_CATEGORY("Assertion.Failure");
        BALL_LOG_STREAM(level)
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
            << u_StackTrace()
            << "'\n"
        << BALL_LOG_END
    }
}

void
AssertionLogger::getLogSeverityCallback(LogSeverityCallback  *callback,
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
AssertionLogger::setLogSeverityCallback(LogSeverityCallback  callback,
                                        void                *closure)
{
    bsls::AtomicOperations::setPtrRelease(&s_closure, closure);

    // Work around warning about converting between data and function pointers.

    void *cb;

    BSLS_ASSERT(sizeof(LogSeverityCallback) == sizeof cb);

    bsl::memcpy(reinterpret_cast<unsigned char *>(&cb),
                reinterpret_cast<unsigned char *>(&callback),
                sizeof(cb));
    bsls::AtomicOperations::setPtrRelease(&s_callback, cb);
}

void
AssertionLogger::setDefaultLogSeverity(ball::Severity::Level severity)
{
    bsls::AtomicOperations::setIntRelaxed(&s_severity, severity);
}

ball::Severity::Level AssertionLogger::defaultLogSeverity()
{
    return static_cast<ball::Severity::Level>(
                           bsls::AtomicOperations::getIntRelaxed(&s_severity));
}

}  // close package namespace
}  // close enterprise namespace

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
