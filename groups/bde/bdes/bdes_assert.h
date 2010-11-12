// bdes_assert.h                  -*-C++-*-
#ifndef INCLUDED_BDES_ASSERT
#define INCLUDED_BDES_ASSERT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide macros and setting mechanisms for user-defined "asserts".
//
//@DEPRECATED: Use 'bsls_assert' instead.
//
//@CLASSES:
//              bdes_Assert: build-contingent "assert"  management functions
//  bdes_AssertFailureHandlerGuard: scoped guard for changing handlers safely
//
//@SEE_ALSO:
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component provides a pair of "assert-like" macros to
// implement a compile-time-selectable and run-time-configurable "defensive" or
// "safe" mode, and a suite of administration methods to configure the macros
// at runtime.  Also provided is an 'invokeHandler' method that is used to
// implement the macros and enable the runtime configuration, and a suite of
// "assert-failure handler" methods suitable for loading into the macros via
// the administrative methods.  Users may also supply their own handler
// methods, so long as the signature matches the 'Handler' 'typedef'.
//
// To allow the user more flexibility in changing handlers during program
// execution, this component also provides a scoped guard,
// 'bdes_AssertFailureHandlerGuard', that allows safe, temporary changing of
// the failure handler in a limited scope.
//
///Macro Definitions
///-----------------
// 'bdes_assert' defines the macro 'BDE_ASSERT_H(X)' for use in 'inline' and
// function template definitions in header ('.h') files and
// 'BDE_ASSERT_CPP(X)' for use in out-of-line function definitions in
// implementation ('.cpp') files.  The behavior of each macro depends on both
// compile-time flags and the effects of run-time administration methods.  If
// 'BDE_BUILD_TARGET_SAFE' is not defined, then 'BDE_ASSERT_H(X)' is disabled
// (i.e., expands to nothing in the preprocessor).  If neither
// 'BDE_BUILD_TARGET_SAFE' nor 'BDE_BUILD_TARGET_DBG' is defined, then both
// "assert" macros are disabled.  The default behavior (i.e., with the build
// flag 'BDE_BUILD_TARGET_DBG' defined but 'BDE_BUILD_TARGET_SAFE' not
// defined) is that 'BDE_ASSERT_H(X)' is disabled and 'BDE_ASSERT_CPP(X)' is
// enabled.  Just as in the case of the standard 'assert' macro, the
// predicate, 'X', must not have side effects, or else the observable behavior
// of the program will change based on compile-time debugging flags.
//
// When enabled, each macro tests the predicate 'X', and, if 'X' is false,
// invokes the installed handler function having the signature (const char *,
// const char *, int), passing the text rendering of the predicate ('#X'),
// '__FILE__', and '__LINE__'.  If the user has not configured the handler
// with the administration method 'setAssertFailureHandler' then the
// 'failAbort' function is invoked; otherwise the user-installed function is
// invoked with the above arguments.
//
///Exception-throwing Failure Handlers
///-----------------------------------
// Among the failure handlers provided is 'assertThrowLogicError', which throws
// 'std::logic_error'.  The user must install this (or another
// exception-throwing handler) explicitly, since throwing is not safe in all
// environments and since deliberately aborting is more useful in a debugging
// context than throwing an unhandled exception.  The
// 'bdes_AssertFailureHandlerGuard' can be used to temporarily set an
// exception-throwing handler within a 'try' block, automatically restoring
// the previous handler when the 'try' block exits.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

    // No symbols or macros are defined here (see 'bsls_assert' component).
    // No aliases or macros for the 'bdes_Assert' types or 'BDE_ASSERT_*'
    // macros are defined here, instead they are defined in 'bsls_assert' so
    // that clients that rely on these via transitive includes may still have
    // those aliases or macros defined.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
