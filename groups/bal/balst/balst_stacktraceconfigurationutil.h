// balst_stacktraceconfigurationutil.h                                -*-C++-*-
#ifndef INCLUDED_BALST_STACKTRACECONFIGURATIONUTIL
#define INCLUDED_BALST_STACKTRACECONFIGURATIONUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utility for global configuration of stack trace.
//
//@CLASSES:
//  balst::StackTraceConfigurationUtil: global configuration of stack trace
//
//@DESCRIPTION: This component provides global configuration for stack traces
// provided by the 'balst' package.
//
// Resolution of symbols, line numbers, and source file names is generally very
// expensive and involves a lot of disk access.  On Windows, such resolution
// can also be very problematic for other reasons.
//
// Note that line number and source file name information is not available on
// all platforms.  If resolution is enabled, the stack trace will give as much
// information as 'balst' has implemented for the platform.  All platforms
// support symbol names.
//
///Configuration Options
///------------------------
// Currently this component provides one configuration option:
//
//: o [disable|enable]Resolution: Configures whether operations in 'balst'
//:   resolve symbols, line numbers, and source file names.  By default,
//:   resolution is enabled.  Resolution of symbols, line numbers, and source
//:   file names is generally expensive and involves disk access.  On Windows
//:   platforms there are also thread-safety concerns with 'dbghlp.dll'.  This
//:   option allows an application owner to globally prevent any resolution of
//:   symbols, line numbers, or source file names (by 'balst) in an
//:   application.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Evaluating Boolean Value
///- - - - - - - - - - - - - - - - - -
// If neither 'enableResolution' nor 'disableResolution' have been called, the
// default value of 'isResolutionDisabled' is 'false'.
//..
//  assert(false ==
//                 balst::StackTraceConfigurationUtil::isResolutionDisabled());
//..
// After that, the value tracks whether 'disableResolution' or
// 'enableResolution' has been called.
//..
//  balst::StackTraceConfigurationUtil::disableResolution();
//
//  assert(true == balst::StackTraceConfigurationUtil::isResolutionDisabled());
//
//  balst::StackTraceConfigurationUtil::enableResolution();
//
//  assert(false ==
//                 balst::StackTraceConfigurationUtil::isResolutionDisabled());
//..

#include <balscm_version.h>

namespace BloombergLP {
namespace balst {

                    // =================================
                    // class StackTraceConfigurationUtil
                    // =================================

struct StackTraceConfigurationUtil {
    // This 'struct' provides a namespace for functions that configure the
    // behavior of stack traces performed by 'balst'.

    // CLASS METHODS
    static
    void disableResolution();
        // Disable symbol, source file name, and line number resolution in
        // stack traces performed by 'balst'.  Note that such resolution is
        // expensive, involving disk access, and on Windows platforms has
        // thread-safety concerns if the application is using 'dbghelp.dll'.
        // Disabling resolution allows an application owner to globally prevent
        // any resolution of symbols (by 'balst') in the process.

    static
    void enableResolution();
        // Enable symbol resolution and source file name & line number
        // resolution on platforms that support them in stack traces performed
        // by 'balst'.  Note that such resolution is expensive, involving disk
        // access, and on Windows platforms has thread-safety concerns if the
        // application is using 'dbghelp.dll'.  Enabling resolution (the
        // default state) results in 'balst' stack traces giving as much
        // information as is available on the platform.

    static
    bool isResolutionDisabled();
        // Return whether symbol and line number resolution in stack traces is
        // disabled.  Note that if neither 'disableResolution' nor
        // 'enableResolution' have ever been called, this defaults to 'false'.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
