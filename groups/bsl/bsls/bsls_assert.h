// bsls_assert.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLS_ASSERT
#define INCLUDED_BSLS_ASSERT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide build-specific, runtime-configurable assertion macros.
//
//@CLASSES:
//  bsls::Assert: namespace for "assert" management functions
//  bsls::AssertFailureHandlerGuard: scoped guard for changing handlers safely
//  bsls::AssertViolation: attributes describing a failed assertion
//
//@MACROS:
//  BSLS_ASSERT: runtime check typically enabled in non-opt build modes
//  BSLS_ASSERT_SAFE: runtime check typically only enabled in safe build modes
//  BSLS_ASSERT_OPT: runtime check typically enabled in all build modes
//  BSLS_ASSERT_INVOKE: for directly invoking the current failure handler
//  BSLS_ASSERT_INVOKE_NORETURN: direct invocation always marked to not return
//
//@SEE_ALSO: bsls_review, bsls_asserttest
//
//@DESCRIPTION: This component provides three "assert-like" macros,
// `BSLS_ASSERT`, `BSLS_ASSERT_SAFE`, and `BSLS_ASSERT_OPT`, that can be used
// to enable optional *redundant* runtime checks in corresponding build modes.
// If an assertion argument evaluates to 0, a runtime-configurable "handler"
// function is invoked with a `bsls::AssertViolation`, a value-semantic class
// that encapsulates the current filename, line number, level of failed check,
// and (0-valued expression) argument text.
//
// The class `bsls::Assert` provides functions for manipulating the globally
// configured "handler".  A scoped guard for setting and restoring the assert
// handler is provided by `bsls::AssertFailureHandlerGuard`.
//
// An additional macro, `BSLS_ASSERT_INVOKE`, is provided for direct invocation
// of the current assertion failure handler.  This macro is always enabled
// (i.e., regardless of build mode).
//
///Defensive Programming (DP)
///--------------------------
// Although there is no one agreed-upon definition, in this context we will use
// the term *Defensive* *Programming* (DP) to mean any attempt by the component
// author to provide (optional) runtime validation of the preconditions (or
// invariants) defined in the function-level documentation (contract) for that
// component.  Note that DP is intended to help expose defects early in the
// development process, and *never* to mask or recover from them in production.
//
// Calling a function without satisfying its preconditions results in
// *undefined* *behavior*.  Detecting and reporting undefined behavior due to
// client misuse can sometimes be very helpful at identifying subtle errors.
// Additionally, we may choose to embed redundant (i.e., logically superfluous)
// runtime checks -- both as a form of active documentation, and also to help
// expose our own, latent coding errors that have escaped detection during
// testing.  In either case, these *defensive* (and other) runtime checks can
// add significant overhead.  Hence, this extra runtime overhead should not
// necessarily be incorporated for every build target and assertion mode (see
// "Build Modes" below).  Moreover, the extent of these checks may change
// (i.e., for a particular build mode) from one release to the next.
// Therefore, any defensive (or other redundant) checks provided for a
// particular build mode are *NEVER* part of the function-level contract -- and
// remain solely what is known as a *Quality-of-Implementation* (QoI) issue.
//
///Assertion Semantics
///-------------------
// There are three important aspects of assertions: (1) **Every assertion
// is redundant**; it is essential that if all assertions are compiled
// out of a program that is defect-free, apart from improved runtime
// performance, the program behaves identically.  Hence, (2) **each**
// !boolean-valued! **assert argument must have no**
// !side-effects!.  Finally, (3) **assertions do not affect binary
// compatibility**; hence, translation units with different
// assertion levels (but not necessarily build targets) can safely be combined
// into a single program (see "Build Modes" and "Assertions in Header Files"
// below).  Note that the build target `BDE_BUILD_TARGET_SAFE_2` does permit
// binary incompatibility for conditionally compiled source code, but there is
// no corresponding `BSLS_ASSERT_SAFE_2` assertion macro (see [](#Usage)).
//
///Assertion Modes
///---------------
// Depending on the build, assertion macros can expand in 3 different ways:
//
// 1. A `bsls_assert` macro is "enabled in assert mode", or simply "enabled" if
//    it expands to check its predicate and call the assert failure handler
//    when it is false.
// 2. A `bsls_assert` macro is "enabled in review mode", or simply "in review
//    mode" if it expands to check its predicate and call the *review* failure
//    handler when it is false.  This is identical to a `bsls_review` macro of
//    the same level when it is enabled.
// 3. A `bsls_assert` macro is "disabled" if it expands to do nothing,
//    producing no executed code in the compiled program.
//
///Review Mode
///-----------
// The ability to enable assertions in review mode allows clients to easily and
// safely test, in a production environment, whether assertions having a lower
// threshold than what they currently have deployed are being triggered
// (without terminating the application).  It is intended as an interim step
// towards lowering the assertion level threshold for an existing application.
// See `bsls_review` for a more detailed description of the behavior of
// assertions in review mode and suggested workflows for using this behavior.
//
///Detailed Behavior
///-----------------
// If an assertion fires (i.e., due to a 0-valued expression argument in an
// assert macro that is enabled or in review mode), there is a violation of the
// contract that the assertion is checking.  If the assertion is enabled, the
// goal of the assertion is to report the precise location and nature of the
// defect *quickly* and *loudly* and prevent continued execution of the calling
// function past that point.  If the assertion is in review mode then the
// behavior will match the corresponding `bsls_review` macro and execution
// might continue, which has a priority of just logging the failure location.
//
// When enabled, the assert macros will all do essentially the same thing: Each
// macro tests the predicate expression `X`, and if `!(X)` is `true`, invokes
// the currently installed assertion failure handler.  An instance of
// `bsls::AssertViolation` will be created and populated with a textual
// rendering of the predicate (`#X`), the current `__FILE__`, the current
// `__LINE__`, and a string representing which particular type of assertion has
// failed.  This `violation` is then passed to the currently installed
// assertion failure handler (a function pointer with the type
// `bsls::Assert::ViolationHandler` having the signature:
// ```
// void(const bsls::AssertViolation&);
// ```
//
// On some (currently experimental) platforms with support for some form of the
// upcoming language-level contract facilities there is also the ability to
// configure the assertion macros to introduce an assumption of the truth of
// their predicate.  With this option the predicate will not neccesarily even
// be evaluated, and if it were to return false the compiler will treat the
// situation as undefined behavior ("impossible").  This mode for assertions
// can lead to improved code generation, but be aware that the potential
// downside of being wrong about the truth of your assertions is unbounded, and
// so deploying applications built with any assertions assumed should be done
// with great care - there are no guarantees about anything a program will do
// when an assumed assertion is violated.
//
///Selecting Which ASSERT Macro to Use
///-----------------------------------
// The choice of which specific macro to use is governed primarily by the
// impact that enabling the assertion (in either assert mode or review mode)
// will have on the runtime performance of the function, and in some cases on
// the size of the function.
//
// 1. `BSLS_ASSERT_SAFE` - This macro should be reserved for tests incurring an
//    expensive change to the performance of a function, either a very high
//    constant time increase in execution time of the function, or an increase
//    in the algorithmic complexity of a function.  Note especially that a
//    change in algorithmic complexity breaks the documented contract of many
//    functions (e.g., an `O(n)` check in a function with a documented
//    `O(log(n))` runtime speed) and so checks with that level of cost should
//    be reserved for diagnostic use in "safe" builds.
// 2. `BSLS_ASSERT` - For "inexpensive" checks with only a constant factor
//    overhead.  The majority of checks should fall into this category.
// 3. `BSLS_ASSERT_OPT` - For "negligible" checks that have little to no
//    measurable overhead on a function.  This will often be the case for
//    argument checking in larger functions, or very simple checks in smaller
//    functions.  Keep in mind that these checks will be enabled in all
//    typically deployed build modes, so they should be reserved for larger
//    functions and functions that will not be called in highly performance
//    critical code.
//
///Assertion and Review Levels
///---------------------------
// There are a few macros available to control which of the `bsls_assert`
// macros are disabled, enabled in review mode, or enabled in assert mode (see
// {Assertion Modes} above).  These macros are for the compilation and build
// environment to provide and are not themselves defined by BDE code -- e.g.,
// by supplying one or more of these macros with `-D` options on the compiler
// command line.  In general, these macros are used to determine an
// `ASSERT_LEVEL` that can be (from most aggressive/optimized to safest)
// `ASSUME_SAFE`, `ASSUME_ASSERT`, `ASSUME_OPT`, `NONE`, `ASSERT_OPT`,
// `ASSERT`, or `ASSERT_SAFE`.  Separately, a `REVIEW_LEVEL` is determined that
// can be `NONE`, `REVIEW_OPT`, `REVIEW`, or `REVIEW_SAFE`.  Depending on these
// levels, the various `bsls_assert` macros will be enabled, in review mode,
// assumed, or disabled.  Macros up to the assert level will be enabled.  If
// the review level is higher than the assert level then macros up to the
// review level (and above the assert level) will be enabled in review mode.
// Finally, macros higher than both the review level and the assert level will
// be disabled.  If the review level is `NONE` and the assert level is set to
// one of the assume levels, then macros that would be disabled up to the
// assumed level are instead assumed.  If there is a review level set then no
// macros will ever be assumed.  The following table illustrates this:
// ```
// ===========================================
//  Macro Instantiation Based on Review Level
// ===========================================
// ENABLED   - Assertion is enabled (in "assert mode")
// REVIEW    - Assertion is enabled (in "review mode")
// ASSUMED   - Assertion is assumed (if supported)
// <blank>   - Assertion is ignored
// -----------BSLS... LEVELS----------  ----------BSLS_.. MACROS---------
// BSLS_ASSERT_LEVEL BSLS_REVIEW_LEVEL  ASSERT_OPT ASSERT     ASSERT_SAFE
// ----------------- -----------------  ---------- ---------- -----------
// ASSUME_SAFE       NONE               ASSUMED    ASSUMED    ASSUMED
// ASSUME_ASSERT     NONE               ASSUMED    ASSUMED
// ASSUME_OPT        NONE               ASSUMED
// NONE              NONE
// NONE (or ASSUME*) REVIEW_OPT         REVIEW
// NONE (or ASSUME*) REVIEW             REVIEW     REVIEW
// NONE (or ASSUME*) REVIEW_SAFE        REVIEW     REVIEW     REVIEW
// ASSERT_OPT        NONE               ENABLED
// ASSERT_OPT        REVIEW_OPT         ENABLED
// ASSERT_OPT        REVIEW             ENABLED    REVIEW
// ASSERT_OPT        REVIEW_SAFE        ENABLED    REVIEW     REVIEW
// ASSERT            NONE               ENABLED    ENABLED
// ASSERT            REVIEW_OPT         ENABLED    ENABLED
// ASSERT            REVIEW             ENABLED    ENABLED
// ASSERT            REVIEW_SAFE        ENABLED    ENABLED    REVIEW
// ASSERT_SAFE       NONE               ENABLED    ENABLED    ENABLED
// ASSERT_SAFE       REVIEW_OPT         ENABLED    ENABLED    ENABLED
// ASSERT_SAFE       REVIEW             ENABLED    ENABLED    ENABLED
// ASSERT_SAFE       REVIEW_SAFE        ENABLED    ENABLED    ENABLED
// ```
// See {`bsls_review`} for the logic that determines the review level.  The
// logic that determines the assertion level checks a few different macros.
// The first check is for one of the 7 mutually exclusive `BSLS_ASSERT_LEVEL`
// macros that can explicitly set the assert level:
// ```
// MACRO                           BSLS_ASSERT_LEVEL
// -----                           ----------------
// BSLS_ASSERT_LEVEL_ASSUME_SAFE   ASSUME_SAFE
// BSLS_ASSERT_LEVEL_ASSUME_ASSERT ASSUME_ASSERT
// BSLS_ASSERT_LEVEL_ASSUME_OPT    ASSUME_OPT
// BSLS_ASSERT_LEVEL_NONE          NONE
// BSLS_ASSERT_LEVEL_ASSERT_OPT    ASSERT_OPT
// BSLS_ASSERT_LEVEL_ASSERT        ASSERT
// BSLS_ASSERT_LEVEL_ASSERT_SAFE   ASSERT_SAFE
// ```
// If none of these are defined, the assert level is determined by the build
// mode.  With "safer" build modes we incorporate higher level defensive
// checks.  A particular build mode is implied by the relevant (BDE) build
// targets that are defined at compilation (preprocessing) time.  The following
// table shows the three (BDE) build targets that can affect the assertion and
// review levels:
// ```
//       (BDE) Build Targets
//     -----------------------
// (A) BDE_BUILD_TARGET_SAFE_2
// (B) BDE_BUILD_TARGET_SAFE
// (C) BDE_BUILD_TARGET_OPT
// ```
// *Any* of the 8 possible combinations of the three build targets is valid:
// e.g., `BDE_BUILD_TARGET_OPT` and `BDE_BUILD_TARGET_SAFE_2` may both be
// defined.  The following table shows the assert level that is set depending
// on which combination of build target macros have been set:
// ```
//  =========================================================
//  "ASSERT" Level Set With no Level-Overriding Flags defined
//  =========================================================
// --- BDE_BUILD_TARGET ----   BSLS_ASSERT_LEVEL
// _SAFE_2   _SAFE    _OPT
// -------  -------  -------   -----------------
//                             ASSERT
//                   DEFINED   ASSERT_OPT
//          DEFINED            ASSERT_SAFE
//          DEFINED  DEFINED   ASSERT_SAFE
// DEFINED                     ASSERT_SAFE
// DEFINED           DEFINED   ASSERT_SAFE
// DEFINED  DEFINED            ASSERT_SAFE
// DEFINED  DEFINED  DEFINED   ASSERT_SAFE
// ```
// As the table above illustrates, with no build target explicitly defined the
// assert level defaults to `ASSERT`.  If only `BDE_BUILD_TARGET_OPT` is
// defined, the assert level will be set to `ASSERT_OPT`.  If either
// `BDE_BUILD_TARGET_SAFE` or `BDE_BUILD_TARGET_SAFE_2` is defined then the
// assert level is set to `ASSERT_SAFE` and ALL assert macros will be enabled.
//
///Runtime-Configurable Assertion-Failure Behavior
///-----------------------------------------------
// In addition to the three (BSLS) "ASSERT" macros, `BSLS_ASSERT`,
// `BSLS_ASSERT_SAFE`, and `BSLS_ASSERT_OPT`, and the immediate invocation
// macro `BSLS_ASSERT_INVOKE`, this component provides (1) an `invokeHandler`
// method used (primarily) to implement these "ASSERT" macros and enable their
// runtime configuration, (2) administration methods to configure, at runtime,
// the behavior resulting from an assertion failure (i.e., by installing an
// appropriate assertion-failure handler function), and (3) a suite of standard
// ("off-the-shelf") assertion-failure handler functions, to be installed via
// the administrative methods (if desired), and invoked by the `invokeHandler`
// method on an assertion failure.
//
// When an enabled assertion fails, the currently installed *failure* *handler*
// ("callback") function is invoked.  The default handler is the (`static`)
// `bsls::Assert::failByAbort` method; a user may replace this default handler
// by using the (`static`) `bsls::Assert::setViolationHandler` administrative
// method and passing it (the address of) a function whose signature conforms
// to the `bsls::Assert::ViolationHandler` `typedef`.  This handler may be one
// of the other handler methods provided in `bsls::Assert`, or a new "custom"
// function, written by the user (see [](#Usage)).
//
///Exception-Throwing Failure Handlers and `bsls::AssertFailureHandlerGuard`
///-------------------------------------------------------------------------
// Among the failure handlers provided is `bsls::Assert::failByThrow`, which
// throws a `bsls::AssertTestException` object.  Throwing an exception,
// however, is not safe in all environments and deliberately aborting is more
// useful in a debugging context than throwing an unhandled exception.  Hence,
// in order for an `bsls::AssertTestException` object to be thrown on an
// assertion failure, the user must first install the
// `bsls::Assert::failByThrow` handler (or another exception-throwing handler)
// explicitly.
//
// Note that an object of type `bsls::AssertFailureHandlerGuard` can be used to
// temporarily set an exception-throwing handler within a `try` block,
// automatically restoring the previous handler when the `try` block exits (see
// [](#Usage)).
//
///Assertion Handler Policy
///------------------------
// Bloomberg policy is that (by default) tasks may not install an assertion
// handler that returns control to the point immediately following the
// detection of a failed assertion.  So an assertion handler may, for example,
// terminate the task or throw an exception, but may not log the problem and
// return.  `bsls_assert`, by default, enforces that policy by terminating the
// task if an installed assertion handler function chooses to returns normally.
//
///Configuring an Exception to the Assertion Handler Policy
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// `bsls_assert` provides a two-part mechanism to permit returning after the
// detection of failed assertions.
//
// It is a violation of Bloomberg policy to modify this default configuration
// without permission from senior management.  (Internal Bloomberg users should
// contact the BDE team if you feel your application needs an exception to this
// policy).
//
// The intention is to provide a means to override the assertion failure policy
// that can be enabled quickly, but requires the explicit (and obvious) choice
// from both the owner of the application's `main` function, and the person
// responsible for building the application.  In order to enable a policy
// exception, `permitOutOfPolicyReturningFailureHandler` must be called, and
// the task must be linked with a special build of `bsls_assert.o` (in which
// the `k_permitOutOfPolicyReturningAssertionBuildKey` constant has the value
// "bsls-PermitOutOfPolicyReturn").
//
///Legacy Handler Functions
///------------------------
// Prior to the introduction of `bsls::AssertViolation`, the signature for
// `bsls::Assert::ViolationHandler` was this:
// ```
// void(const char*, const char*,int)
// ```
// This signature for a handler is still supported (though deprecated) under
// its original name `bsls::Assert::Handler`.  Overloads that take a
// `bsls::Assert::Handler` exist for `bsls::AssertFailureHandler` and the
// constructor for `bsls::AssertFailureHandlerGuard`, so code that uses the old
// handler signature should work without changes.
//
// If a legacy handler is set as the current handler, the function
// `bsls::Assert::failureHandler()` will return a pointer to that function,
// while `bsls::Assert::violationHandler()` will return an internal function
// that extracts the appropriate arguments from the generated
// `bsls::AssertViolation` object and passes them to the installed `Handler`.
//
///Assertions in Header Files (Mixing Build Options Across Translation Units)
///--------------------------------------------------------------------------
// Mixing build modes across translation units, although not strictly
// conformant with the C++ language standard, is permissible in practice;
// however, the defensive checks that are enabled may be unpredictable.  The
// *one-definition* *rule* states that if there are multiple definitions of an
// object or function within a program, these definitions *must* be identical
// or else the program is *ill-formed*.  Unfortunately, definitions in header
// files may not be identical across object (`.o`) files if the build targets
// or assertion-level flags defined during translation (preprocessing) are not
// the same.
//
// For example, consider an `inline` function that sets the width of a `Square`
// and optionally checks for (defends against) a negative `width` argument:
// ```
// // our_square.h
//
// inline
// void Square::setWidth(int width)
// {
//     BSLS_ASSERT_SAFE(width >= 0);
//
//     d_width = width;
// }
// ```
// Now consider a client that uses this `setWidth` method:
// ```
// // my_client.cpp
//
// void f()
// {
//     Square s;
//     s.setWidth(-5);
// }
// ```
// We can build the `our_square` component in "safe mode" -- e.g., by
// incorporating `-DBSLS_ASSERT_LEVEL_ASSERT_SAFE` on the (Unix) command line.
// Notice, however, that building client software against a version of
// `our_square.o` compiled in "safe mode" does *not* ensure that all of the
// `BSLS_ASSERT_SAFE` macros will be active (will instantiate); instead, the
// client's build mode will (most likely) govern those instantiations of the
// `BSLS_ASSERT_SAFE` macro located within the library.  The only way to ensure
// that all of the `BSLS_ASSERT_SAFE` macros instantiate is to build the
// *client* as well as the library software in "safe mode".
//
// Inline functions are not the only source of multiple inconsistent
// definitions.  Consider a non-`inline` method `reserveCapacity` on a `List`
// template, parameterized by element `TYPE`:
// ```
// // our_list.h
//
// template <class TYPE>
// void List<TYPE>::reserveCapacity(int numElements)
// {
//     BSLS_ASSERT(numElements >= 0);
//     // ...
// }
// ```
// Each different translation unit that invokes `reserveCapacity` potentially
// generates another instantiation of this function template.  Those
// translation units that are compiled in "debug mode" (or "safe mode") --
// e.g., with `BSLS_ASSERT_LEVEL_ASSERT` (or `BSLS_ASSERT_LEVEL_ASSERT_SAFE`)
// defined -- will incorporate code corresponding to each use of the
// `BSLS_ASSERT` macro therein; the rest will not.  Which one of these template
// instantiations the linker uses in the final program is undefined and highly
// unpredictable.
//
// The bottom line is that, unless clients of a library are compiled with (at
// least) the same level of assertion enabling as the library itself, not all
// of the library's defensive checking (for the assertion-level for which the
// library was compiled) will necessarily be incorporated into the client code.
// Similarly, compiling a client in a higher-level of defensive checking (e.g.,
// "safe mode") than the library was compiled (e.g., "debug mode") may result
// in additional defensive checks beyond what the library author intended for
// the mode (e.g., "debug mode") in which the library was compiled.
//
// Note that all build modes (except for when `BDE_BUILD_TARGET_SAFE_2` is
// defined, see below) are required to be binary compatible (e.g., fields
// cannot be added to the middle of a `struct`).  Since a component's contract
// makes no explicit promise about what checking will occur, that contract is
// not violated when different parts of a program are compiled with different
// levels of assertion-enabling build options.  The only consequence is that a
// smaller (or larger) number of defensive checks may be active than might
// otherwise be expected.
//
///Conditional Compilation
///-----------------------
// To recap, there are three (mutually compatible) general **build targets**:
// * `BDE_BUILD_TARGET_OPT`
// * `BDE_BUILD_TARGET_SAFE`
// * `BDE_BUILD_TARGET_SAFE_2`
//
// seven (mutually exclusive) component-specific **assertion levels**:
// * `BSLS_ASSERT_LEVEL_ASSERT_SAFE`
// * `BSLS_ASSERT_LEVEL_ASSERT`
// * `BSLS_ASSERT_LEVEL_ASSERT_OPT`
// * `BSLS_ASSERT_LEVEL_NONE`
// * `BSLS_ASSERT_LEVEL_ASSUME_OPT`
// * `BSLS_ASSERT_LEVEL_ASSUME_ASSERT`
// * `BSLS_ASSERT_LEVEL_ASSUME_SAFE`
//
// and four (mutually exclusive) component-specific **review levels**:
// * `BSLS_REVIEW_LEVEL_REVIEW_SAFE`
// * `BSLS_REVIEW_LEVEL_REVIEW`
// * `BSLS_REVIEW_LEVEL_REVIEW_OPT`
// * `BSLS_REVIEW_LEVEL_NONE`
//
// The above macros can be defined (externally) by the build environment to
// affect which of the three **assert macros**:
// * `BSLS_ASSERT_SAFE(boolean-valued expression)`
// * `BSLS_ASSERT(boolean-valued expression)`
// * `BSLS_ASSERT_OPT(boolean-valued expression)`
//
// will be enabled in assert mode, which will be in review mode, which will be
// assumed, and which will be disabled.
//
// The public interface of this component also explicitly provides a number of
// additional intermediate macros to identify how the various `BSLS_ASSERT`
// macros have been instantiated.  These each exist for each level and have the
// following suffixes and meanings:
// * `IS_ACTIVE`: Defined if the corresponding level is enabled in assert or
//   review mode.  For example, `BSLS_ASSERT_SAFE_IS_ACTIVE` is defined if
//   (and only if) the conditions expressed using `BSLS_ASSERT_SAFE` will be
//   checked at runtime (either as assertions or reviews).
// * `IS_REVIEW`: Defined if the corresponding level is enabled in review
//   mode.
// * `IS_ASSUMED`: Defined if the corresponding level is assumed.
// * `IS_USED`: Defined if assert expressions for the corresponding level need
//   to be valid (i.e., if they are "ODR-used").  For example,
//   `BSLS_ASSERT_SAFE_IS_USED` is defined if (and only if) the conditions
//   expressed using `BSLS_ASSERT_SAFE` will be compiled.  Note that this is a
//   super-set of the cases where `BSLS_ASSERT_SAFE_IS_ACTIVE` will be
//   defined, which is when the conditions will be checked at runtime, while
//   `BSLS_ASSERT_SAFE_IS_USED` is also defined if the conditions are assumed
//   or if `BSLS_ASSERT_VALIDATE_DISABLED_MACROS` is defined.
//
// Putting that together, these 3 macros are defined if the corresponding macro
// is in assert or review mode - and thus the expression will be checked and a
// violation handler will be invoked on failure:
// * `BSLS_ASSERT_SAFE_IS_ACTIVE`
// * `BSLS_ASSERT_IS_ACTIVE`
// * `BSLS_ASSERT_OPT_IS_ACTIVE`
//
// These three are defined if the corresponding macro is in review mode - and
// thus the expression will be checked and the review violation handler will be
// invoked on failure.  These will be defined when the review level has been
// set to a level higher than the assert level:
// * `BSLS_ASSERT_SAFE_IS_REVIEW`
// * `BSLS_ASSERT_IS_REVIEW`
// * `BSLS_ASSERT_OPT_IS_REVIEW`
//
// These three are defined if the corresponding macro is being assumed, and it
// will be hard undefined behavior to violate these expressions:
// * `BSLS_ASSERT_SAFE_IS_ASSUMED`
// * `BSLS_ASSERT_IS_ASSUMED`
// * `BSLS_ASSERT_OPT_IS_ASSUMED`
//
// Finally, three more macros with the `IS_USED` suffix are defined when the
// expression for the corresponding macro is going to be compiled.  This will
// be true for macros in assert, review or assumed modes, and it will be true
// for all macros if `BSLS_ASSERT_VALIDATE_DISABLED_MACROS` has been defined.
// * `BSLS_ASSERT_SAFE_IS_USED`
// * `BSLS_ASSERT_IS_USED`
// * `BSLS_ASSERT_OPT_IS_USED`
//
// Note that any of the `IS_ACTIVE`, `IS_REVIEW`, and `IS_ASSUMED` macros being
// defined will imply that the corresponding `IS_USED` macro is also defined.
//
// Which of these macros to use to conditionally compile supporting code is
// based on when that supporting code needs to be compiled:
// * Use `#if defined(..._IS_USED)` when:
//   - Writing functions that are only accessible to and needed for assertions
//     of the corresponding level.  This could be private member functions,
//     static functions, or functions in an anonymous namespace.  See
//     {Example 8} for details on this use.
// * Use `#if !defined(..._IS_ACTIVE) && !defined(..._IS_ASSUMED)` when:
//   - You are writing (test) code that will intentionally violate a contract
//     when there is not going to be any intrinsic ill effect to that
//     violation.  Generally this should only be required when there is a need
//     to validate out-of-contract behavior of a component from within its own
//     test driver.
// * Use `#if defined(...IS_ACTIVE)` when:
//   - You are doing negative testing and want to be sure that when you call
//     your function out of contract that the violation handler will be
//     invoked.  See `bsls_asserttest` for tools to do this without having
//     to manually check these macros.
//   - Writing redundant defensive code that should only execute when the
//     corresponding assertions are going to be enabled.  The assertion itself
//     should also be included in the same preprocessor block.  See
//     {Example 9} for details on this use.
//   - Note that historically this was the only macro available, and it is
//     often used for blocks of code where the checks above would be more
//     appropriate.  This can often lead to code that fails to compile with
//     `BSLS_ASSERT_VALIDATE_DISABLED_MACROS` enabled or which will not work
//     correctly when assumptions are turned on.
//
// See {Example 6} and {Example 7}, respectively, for how
// `BDE_BUILD_TARGET_SAFE_2` and intermediate assertion predicate macros, such
// as `BSLS_ASSERT_SAFE_IS_ACTIVE` (and even `BSLS_ASSERT_OPT_IS_ACTIVE`), can
// be used profitably in practice.
//
///Validating Disabled Macro Expressions
///-------------------------------------
// An additional external macro, `BSLS_ASSERT_VALIDATE_DISABLED_MACROS`, can be
// defined to control the compile time behavior of `bsls_assert`.  Enabling
// this macro configures all **disabled** assert macros to still instantiate
// their predicates (in a non-evaluated context) to be sure that the predicate
// is still syntactically valid.  This can be used to ensure assertions that
// are rarely enabled have valid expressions.
//
///Language-Level Contracts
///------------------------
// Contracts were proposed, accepted into the draft C++20 standard, and then
// removed.  Implementations of that facility exist and it is expected future
// implementations will begin to arrive as work on new proposals comes to
// fruition.  Defining the macro `BSLS_ASSERT_USE_CONTRACTS` will cause all
// `BSLS_ASSERT` (and, if possible, `BSLS_REVIEW`) macros to go through the
// language-level contract implementation if it is available (currently only on
// an experimental version of the gcc-compiler), otherwise a diagnostic will be
// issued.
//
// Note that mixing builds that do and do not use `BSLS_ASSERT_USE_CONTRACTS`
// is not supported.  Attempting to link against a library bult with a
// different mode for this option will cause a link-time error.
//
///Usage
///-----
// The following examples illustrate (1) when to use each of the three kinds of
// (BSLS) "ASSERT" macros, (2) when and how to call the `invokeHandler` method
// directly, (3) how to configure, at runtime, the behavior resulting from an
// assertion failure using "off-the-shelf" handler methods, (4) how to create
// your own custom assertion-failure handler function, (5) proper use of
// `bsls::AssertFailureHandlerGuard` to install, temporarily, an
// exception-producing assert handler, (6) how "ASSERT" macros would be used in
// conjunction with portions of the source code (affecting binary
// compatibility) that are incorporated only when `BDE_BUILD_TARGET_SAFE_2` is
// defined, and (7) how assertion predicates (e.g.,
// `BSLS_ASSERT_SAFE_IS_ACTIVE`) are used to conditionally compile additional
// (redundant) defensive source code (not affecting binary compatibility)
// precisely when the corresponding (BSLS) "ASSERT" macro (e.g.,
// `BSLS_ASSERT_SAFE`) is active.
//
///Example 1: Using `BSLS_ASSERT`, `BSLS_ASSERT_SAFE`, and `BSLS_ASSERT_OPT`
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component provides three different variants of (BSLS) "ASSERT" macros.
// This first usage example illustrates how one might select each of the
// particular variants, based on the runtime cost of the defensive check
// relative to that of the useful work being done.
//
// Use of the `BSLS_ASSERT_SAFE` macro is often appropriate when the defensive
// check occurs within the body of an `inline` function.  The
// `BSLS_ASSERT_SAFE` macro minimizes the impact on runtime performance as it
// is instantiated only when requested (i.e., by building in "safe mode").  For
// example, consider a light-weight point class `Kpoint` that maintains `x` and
// `y` coordinates in the range `[-1000 .. 1000]`:
// ```
// my_kpoint.h
//
// class Kpoint {
//     short int d_x;
//     short int d_y;
//   public:
//
//     /// ...
//     /// The behavior is undefined unless `-1000 <= x <= 1000` and
//     /// `-1000 <= y <= 1000`.
//     Kpoint(short int x, short int y);
//
//     // ...
//  };
//
// ...
// ```
// Since the cost of validation here is significant compared with the useful
// work being done, we might choose to implement defensive checks using
// `BSLS_ASSERT_SAFE` as follows:
// ```
// ...
//
//  inline
//  Kpoint::Kpoint(short int x, short int y)
//  : d_x(x)
//  , d_y(y)
//  {
//      BSLS_ASSERT_SAFE(-1000 <= x); BSLS_ASSERT_SAFE(x <= 1000);
//      BSLS_ASSERT_SAFE(-1000 <= y); BSLS_ASSERT_SAFE(y <= 1000);
//  }
// ```
// For more substantial (non-`inline`) functions, we would be more likely to
// use the `BSLS_ASSERT` macro because the runtime overhead due to defensive
// checks is likely to be much less significant.  For example, consider a
// hash-table class that allows the client to resize the underlying table:
// ```
// my_hashtable.h
// ...
//
//  /// ...
//  class HashTable {
//    public:
//      // ...
//
//      /// Adjust the size of the underlying hash table to be approximately
//      /// the current number of elements divided by the specified
//      /// `loadFactor`.  The behavior is undefined unless
//      /// `0 < loadFactor`.
//      void resize(double loadFactor);
//  };
// ```
// Since the relative runtime cost of validating the input argument is quite
// small (e.g., less than 10%) compared to the typical work being done, we
// might choose to implement the defensive check using `BSLS_ASSERT` as
// follows:
// ```
// my_hashtable.cpp
// ...
//
//  void HashTable::resize(double loadFactor)
//  {
//      BSLS_ASSERT(0 < loadFactor);
//
//      // ...
//  }
// ```
// In some cases, the runtime cost of checking is always negligible when
// compared with the runtime cost of performing the useful work; moreover, the
// consequences of continuing in an undefined state for certain applications
// could be catastrophic.  Instead of using `BSLS_ASSERT` in such cases, we
// might consider using `BSLS_ASSERT_OPT`.  For example, suppose we have a
// financial application class `TradingSystem` that performs trades:
// ```
// my_tradingsystem.h
// ...
//
//  /// ...
//  class TradingSystem {
//    public:
//      // ...
// ```
// Further suppose that there is a particular method `executeTrade` that takes,
// as a scaling factor, an integer that must be a multiple of 100 or the
// behavior is undefined (and might actually execute a trade):
// ```
//     /// Execute the current trade using the specified `scalingFactor`.
//     /// The behavior is undefined unless `0 <= scalingFactor` and `100`
//     /// evenly divides `scalingFactor`.
//     void executeTrade(int scalingFactor);
//
// };
// ```
// Because the cost of the two checks is likely not even measurable compared to
// the overhead of accessing databases and executing the trade, and because the
// consequences of specifying a bad scaling factor are virtually unbounded, we
// might choose to implement these defensive checks using `BSLS_ASSERT_OPT` as
// follows:
// ```
// my_tradingsystem.cpp
// ...
//
//  void TradingSystem::executeTrade(int scalingFactor)
//  {
//      BSLS_ASSERT_OPT(0 <= scalingFactor);
//      BSLS_ASSERT_OPT(0 == scalingFactor % 100);
//
//      // ...
//  }
// ```
// Notice that in each case, the choice of which of the three (BSLS) "ASSERT"
// macros to use is governed primarily by the relative runtime cost compared
// with that of the useful work being done (and only secondarily by the
// potential consequences of continuing execution in an undefined state).
//
///Example 2: When and How to Call the `invokeHandler` Method Directly
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There *may* be times (but this is yet to be demonstrated) when we might
// reasonably choose to unconditionally invoke the currently installed
// assertion-failure handler directly -- i.e., instead of via one of the three
// (BSLS) "ASSERT" macros provided in this component.  Suppose that we are
// currently in the body of some function `someFunc` and, for whatever reason,
// feel compelled to invoke the currently installed assertion-failure handler
// based on some criteria other than the current build mode.
// `BSLS_ASSERT_INVOKE` is provided for this purpose.  The call might look as
// follows:
// ```
// void someFunc(bool a, bool b, bool c)
// {
//     bool someCondition = a && b && !c;
//
//     if (someCondition) {
//         BSLS_ASSERT_INVOKE("Bad News");
//     }
// }
// ```
// If presented with invalid arguments, `someFunc` (above) will produce output
// similar to the following:
// ```
// Assertion failed: Bad News, file bsls_assert.t.cpp, line 609
// Abort (core dumped)
// ```
// If a piece of code needs to be guaranteed to not return, the additional
// macro `BSLS_ASSERT_INVOKE_NORETURN` is also available.  It behaves the same
// way as `BSLS_ASSERT_INVOKE`, but if the installed handler *does* return
// `failByAbort` will be immediately called.  On supported platforms it is
// marked appropriately to not return to support compiler optimizations and
// static analysis tools.
//
// Note that it is possible to use the two `INVOKE` macros with a dynamically
// generated string, but it is generally inadvisable to do so based on how
// downstream systems are likely to handle the logs produced for the triggered
// violation.   Additional information that will be useful to diagnose a
// problem should be logged (using, for example, `bsls_log`) prior to using
// the `INVOKE` macro, and the string passed is ideally a string to identify
// the character of the error, not to encode additional identifying
// information.  
//
///Example 3: Runtime Configuration of the `bsls::Assert` Facility
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// By default, any assertion failure will result in the invocation of the
// `bsls::Assert::failByAbort` handler function.  We can replace this behavior
// with that of one of the other static failure handler methods supplied in
// `bsls::Assert` as follows.  Let's assume we are at the top of our
// application called `myMain` (which would typically be `main`):
// ```
// void myMain()
// {
// ```
// First observe that the default assertion-failure handler function is, in
// fact, `bsls::Assert::failByAbort`:
// ```
// assert(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());
// ```
// Next, we install a new assertion-failure handler function,
// `bsls::Assert::failBySleep`, from the suite of "off-the-shelf" handlers
// provided as `static` methods of `bsls::Assert`:
// ```
// bsls::Assert::setViolationHandler(&bsls::Assert::failBySleep);
// ```
// Observe that `bsls::Assert::failBySleep` is the new, currently-installed
// assertion-failure handler:
// ```
// assert(&bsls::Assert::failBySleep == bsls::Assert::violationHandler());
// ```
// Note that if we were to explicitly invoke the current assertion-failure
// handler as follows:
// ```
// BSLS_ASSERT_INVOKE("message");  // This will hang!
// ```
// the program will hang since `bsls::Assert::failBySleep` repeatedly sleeps
// for a period of time within an infinite loop.  Thus, this assertion-failure
// handler is useful for hanging a process so that a debugger may be attached
// to it.
//
// We may now decide to disable the `setViolationHandler` method using the
// `bsls::Assert::lockAssertAdministration()` method to ensure that no one else
// will override our decision globally.  Note, however, that the
// `bsls::AssertFailureHandlerGuard` is not affected, and can still be used to
// supplant the currently installed handler (see below):
// ```
// bsls::Assert::lockAssertAdministration();
// ```
// Attempting to change the currently installed handler now will fail:
// ```
//     bsls::Assert::setViolationHandler(&bsls::Assert::failByAbort);
//
//     assert(&bsls::Assert::failByAbort != bsls::Assert::violationHandler());
//
//     assert(&bsls::Assert::failBySleep == bsls::Assert::violationHandler());
// }
// ```
//
///Example 4: Creating a Custom Assertion Handler
/// - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes, especially during testing, we may need to write our own custom
// assertion-failure handler function.  The only requirements are that the
// function have the same prototype (i.e., the same respective parameter and
// return types) as the `bsls::Assert::Handle` `typedef`, and that the function
// should not return (i.e., it must `abort`, `exit`, `terminate`, `throw`, or
// hang).  To illustrate, we will create a `static` method at file scope that
// conforms to the required structure (notice the explicit use of `std::printf`
// from `<cstdio>` instead of `std::cout` from `<iostream>` to avoid
// interaction with the C++ memory allocation layer):
// ```
// static bool globalEnableOurPrintingFlag = true;
//
// /// Print the expression `comment`, `file` name, and `line` number from
// /// the specified `violation` to `stdout` as a comma-separated list,
// /// replacing null string-argument values with empty strings (unless
// /// printing has been disabled by the `globalEnableOurPrintingFlag`
// /// variable), then unconditionally abort.
// static
// void ourFailureHandler(const bsls::AssertViolation& violation)
// {
//     const char *comment = violation.comment();
//     if (!comment) {
//         comment = "";
//     }
//     const char *file = violation.fileName();
//     if (!file) {
//         file = "";
//     }
//     int line = violation.lineNumber();
//     if (globalEnableOurPrintingFlag) {
//         std::printf("%s, %s, %d\n", comment, file, line);
//     }
//     std::abort();
// }
// ```
// At the top level of our application we have the following:
// ```
// void ourMain()
// {
// ```
// First, let's observe that we can assign this new function to a function
// pointer of type `bsls::Assert::Handler`:
// ```
// bsls::Assert::ViolationHandler f = &ourFailureHandler;
// ```
// Now we can install it just as we would any other handler:
// ```
// bsls::Assert::setViolationHandler(&ourFailureHandler);
// ```
// We can now invoke the default handler directly:
// ```
// BSLS_ASSERT_INVOKE("str1");
// }
// ```
// With the resulting output something like as follows:
// ```
// str1, my_file.cpp, 17
// Abort (core dumped)
// ```
//
///Example 5: Using the `bsls::AssertFailureHandlerGuard`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes we may want to replace, temporarily (i.e., within some local
// lexical scope), the currently installed assertion-failure handler function.
// In particular, we sometimes use the `bsls::AssertFailureHandlerGuard` class
// to replace the current handler with one that throws an exception (because we
// know that such an exception is safe in the local context).  Let's start with
// the simple factorial function below, which validates, in "debug mode" (or
// "safe mode"), that its input is non-negative:
// ```
// /// Return `n!`.  The behavior is undefined unless `0 <= n`.
// double fact(int n)
// {
//     BSLS_ASSERT(0 <= n);
//
//     double result = 1.0;
//     while (n > 1) {
//         result *= n--;
//     }
//     return result;
// }
// ```
// Now consider the following integer-valued `extern "C"` C++ function,
// `wrapperFunc`, which can be called from C and FORTRAN, as well as from C++:
// ```
// extern "C" int wrapperFunc(bool verboseFlag)
// {
//     enum { GOOD = 0, BAD } result = GOOD; (void) verboseFlag;
// ```
// The purpose of this function is to allow assertion failures in subroutine
// calls below this function to be handled by throwing an exception, which is
// then caught by the wrapper and reported to the caller as a "bad" status.
// Hence, when within the runtime scope of this function, we want to install,
// temporarily, the assertion-failure handler `bsls::Assert::failByThrow`,
// which, when invoked, causes an `bsls::AssertTestException` object to be
// thrown.  (Note that we are not advocating this approach for "recovery", but
// rather for an orderly shut-down, or perhaps during testing.)  The
// `bsls::AssertFailureHandlerGuard` class is provided for just this purpose:
// ```
//     assert(&bsls::Assert::failByAbort == bsls::Assert::violationHandler());
//
//     bsls::AssertFailureHandlerGuard guard(&bsls::Assert::failByThrow);
//
//     assert(&bsls::Assert::failByThrow == bsls::Assert::violationHandler());
// ```
// Next we open up a `try` block, and somewhere within the `try` we
// "accidentally" invoke `fact` with an out-of-contract value (i.e., `-1`):
// ```
// #ifdef BDE_BUILD_TARGET_EXC
//     try
// #endif
//         {
//
//         // ...
//
//         double d = fact(-1);        // Out-of-contract call to 'fact'.
//
//         // ...
//     }
// #ifdef BDE_BUILD_TARGET_EXC
//     catch (const bsls::AssertTestException& e) {
//         result = BAD;
//         if (verboseFlag) {
//             std::printf( "Internal Error: %s, %s, %d\n",
//                          e.expression(),
//                          e.filename(),
//                          e.lineNumber() );
//         }
//     }
// #endif
//     return result;
// }
// ```
// Assuming exceptions are enabled (i.e., `BDE_BUILD_TARGET_EXC` is defined),
// if an `bsls::AssertTestException` occurs below this wrapper function, the
// exception will be caught, a message will be printed to `stdout`, e.g.,
// ```
// Internal Error: bsls_assert.t.cpp:500: 0 <= n
// ```
// and the `wrapperFunc` function will return a bad status (i.e., 1) to its
// caller.  Note that if exceptions are not enabled,
// `bsls::Assert::failByThrow` will behave as `bsls::Assert::failByAbort`, and
// dump core immediately:
// ```
// Assertion failed: 0 <= n, file bsls_assert.t.cpp, line 500
// Abort (core dumped)
// ```
// Finally note that the `bsls::AssertFailureHandlerGuard` is not thread-aware.
// In particular, a guard that is created in one thread will also affect the
// failure handlers that are used in other threads.  Care should be taken when
// using this guard when more than a single thread is executing.
//
///Example 6: Using (BSLS) "ASSERT" Macros Along With `BDE_BUILD_TARGET_SAFE_2`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Recall that assertions do not affect binary compatibility; however, software
// built with `BDE_BUILD_TARGET_SAFE_2` defined need not be binary compatible
// with software built otherwise.  In this example, we look at how we might use
// the (BSLS) "ASSERT" family of macros in conjunction with code that is
// incorporated (at compile time) only when `BDE_BUILD_TARGET_SAFE_2` is
// defined.
//
// As a simple example, let's consider an elided implementation of a
// singly-linked integer list and its iterator.  Whenever
// `BDE_BUILD_TARGET_SAFE_2` is defined, we want to defend against the
// possibility that a client mistakenly passes a `ListIter` object into a
// `List` object method (e.g., `List::insert`) where that `ListIter` object did
// not originate from the same `List` object.
//
// We'll start by defining a local helper `List_Link` `struct` as follows:
// ```
// struct List_Link {
//     List_Link *d_next_p;
//     int        d_data;
//     List_Link(List_Link *next, int data) : d_next_p(next), d_data(data) { }
// };
// ```
// Next, we'll define `ListIter`, which always identifies the current position
// in a sequence of links, but whenever `BDE_BUILD_TARGET_SAFE_2` is defined,
// also maintains a pointer to its parent `List` object:
// ```
// class List;                         // Forward declaration.
//
// class ListIter {
// #ifdef BDE_BUILD_TARGET_SAFE_2
//     List *d_parent_p;               // Exists only in "safe 2 mode".
// #endif
//     List_Link **d_current_p;
//     friend class List;
//     friend bool operator==(const ListIter&, const ListIter&);
//   private:
//     ListIter(List_Link **current,
//              List *
// #ifdef BDE_BUILD_TARGET_SAFE_2
//                    parent           // Not used unless in "safe 2 mode".
// #endif
//             )
//     : d_current_p(current)
// #ifdef BDE_BUILD_TARGET_SAFE_2
//     , d_parent_p(parent)            // Initialize only in "safe 2 mode".
// #endif
//     { }
//   public:
//     ListIter& operator++() { /* ... */ return *this; }
//     // ...
// };
// bool operator==(const ListIter& lhs, const ListIter& rhs);
// bool operator!=(const ListIter& lhs, const ListIter& rhs);
// ```
// Finally we define the `List` class itself with most of the operations
// elided; the methods of particular interest here are `begin` and `insert`:
// ```
//
// class List {
//     List_Link *d_head_p;
//   public:
//     // CREATORS
//     List() : d_head_p(0) { }
//     List(const List&) { /* ... */ }
//     ~List() { /* ... */ }
//
//     // MANIPULATORS
//     List& operator=(const List&) { /* ... */ return *this; }
//
//     //| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//     //v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
//     //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//     /// Return an iterator referring to the beginning of this list.
//     ListIter begin()
//     {
//         return ListIter(&d_head_p, this);
//     }
//     //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//     //| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//     //v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
//     //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//     /// Insert the specified `data` value into this list at the specified
//     /// `position`.
//     void insert(const ListIter& position, int data)
//     {
// #ifdef BDE_BUILD_TARGET_SAFE_2
//         BSLS_ASSERT_SAFE(this == position.d_parent_p);  // "safe 2 mode"
// #endif
//         *position.d_current_p = new List_Link(*position.d_current_p, data);
//     }
//     //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//     // ACCESSORS
//
//     /// Output the contents of this list to `stdout`.
//     void print()
//     {
//         printf( "[" );
//         for (List_Link *p = d_head_p; p; p = p->d_next_p) {
//             printf( " %d", p->d_data );
//         }
//         printf(" ]\n");
//     }
// };
// ```
// Outside of "safe 2 mode", it is possible to pass an iterator object obtained
// from the `begin` method of one `List` object into the `insert` method of
// another, having, perhaps, unexpected results:
// ```
// void sillyFunc(bool printFlag)
// {
//     List a;
//     ListIter aIt = a.begin();
//     a.insert(aIt, 1);
//     a.insert(aIt, 2);
//     a.insert(aIt, 3);
//
//     if (printFlag) {
//         std::printf( "a = "); a.print();
//     }
//
//     List b;
//     ListIter bIt = b.begin();
//     a.insert(bIt, 4);       // Oops!  Should have been: 'b.insert(bIt, 4);'
//     a.insert(bIt, 5);       // Oops!    "     "     "   '    "     "   5  '
//     a.insert(bIt, 6);       // Oops!    "     "     "   '    "     "   6  '
//
//     if (printFlag) {
//         std::printf( "a = "); a.print();
//         std::printf( "b = "); b.print();
//     }
// }
// ```
// In the example above, we have "accidentally" passed the iterator `bIt`
// obtained from `List` object `b` into the `insert` method for `List` object
// `a`.  The resulting undefined behavior (in other than "safe 2 mode") might
// produce output that looks as follows:
// ```
// a = [ 3 2 1 ]
// a = [ 3 2 1 ]
// b = [ 6 5 4 ]
// ```
// If the same `sillyFunc` were compiled in "safe 2 mode" (i.e., with
// `BDE_BUILD_TARGET_SAFE_2` defined) the undefined behavior would be detected
// and the output would, by default, look more like the following:
// ```
// a = [ 3 2 1 ]
// FATAL my_list.cpp:56 Assertion failed: this == position.d_parent_p
// Abort (core dumped)
// ```
// thereby quickly exposing the misuse by the client.
//
///Example 7: Conditional Compilation Associated with Enabled Assertion Levels
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In cases where we want to tie code, other than just an assertion, to a
// specific level of enabled assertions, we will want to use the corresponding
// intermediate predicate that enables that level of assertions:
//
// * For `BSLS_ASSERT_SAFE`, use `BSLS_ASSERT_SAFE_IS_ACTIVE`.
// * For `BSLS_ASSERT`, use `BSLS_ASSERT_IS_ACTIVE`.
// * For `BSLS_ASSERT_OPT`, use `BSLS_ASSERT_OPT_IS_ACTIVE`.
//
// Suppose that we have a class such as `MyDate` (below) that, except for
// checking its invariants, would have a trivial destructor.  By not declaring
// a destructor at all, we may realize performance advantages, but then we lose
// the ability to validate our invariants in "debug" or "safe" mode.  What we
// want to do is to declare (and later define) the destructor in precisely
// those build modes for which we would want to assert invariants.
//
// An elided class `MyDate`, which is based on a serial-date implementation, is
// provided for reference:
// ```
// /// This class implements a value-semantic "date" type representing
// /// valid date values in the range `[ 0001Jan01 .. 9999Dec31 ]`.
// class MyDate {
//
//     // DATA
//     int d_serialDate;  // sequential representation within a valid range
//
//   public:
//      // CLASS METHODS
//
//      // ...
//
//      // CREATORS
//
//      /// Create a `MyDate` object having the value `0001Jan01`.
//      MyDate();
//
//      // ...
//
//      /// Create a `MyDate` object having the same value as the specified
//      /// `original` object.
//      MyDate(const MyDate& original);
//
// #if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
//      /// Destroy this object.  Note that in some build modes the
//      /// destructor generated by the compiler is trivial.
//      ~MyDate();
// #endif
//
//     // ...
// };
//
// ...
//
// ========================================================================
//                 INLINE FUNCTION DEFINITIONS
// ========================================================================
//
// ...
//
// CREATORS
// inline
// MyDate::MyDate()
// : d_serialDate(1)  // 0001Jan01
// {
// }
//
// inline
// MyDate::MyDate(const MyDate& original)
// : d_serialDate(original.d_serialDate)
// {
// }
//
// ...
//
// #if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
// inline
// MyDate::~MyDate()
// {
//     BSLS_ASSERT_SAFE(1 <= d_serialDate);             // 0001Jan01
//     BSLS_ASSERT_SAFE(     d_serialDate <= 3652061);  // 9999Dec31
// }
// #endif
//
// ...
// ```
// In practice, however, we would probably implement an `isValidSerialDate`
// method in a lower-level utility class, e.g., `MyDateImpUtil`, leading to
// code that is more fine-grained, modular, and hierarchically reusable:
// ```
// struct MyDateImpUtil {
//     /// Return `true` if the specified `d_date` represents a valid date
//     /// value, and `false` otherwise.
//     static bool isValidSerialDate(int d_date);
// };
//
// inline
// bool MyDateImpUtil::isValidSerialDate(int d_date)
// {
//     return 1 <= d_date && d_date <= 3652061;
// }
// ```
// Like other aspects of `BSLS_ASSERT_SAFE`, the example above violates the
// one-definition rule for mixed-mode builds.  Note that all code conditionally
// compiled based on `BSLS_ASSERT_SAFE_IS_ACTIVE`, `BSLS_ASSERT_IS_ACTIVE`, and
// `BSLS_ASSERT_OPT_IS_ACTIVE` should be binary compatible for mixed-mode
// builds.  If the conditionally-compiled code would not be binary compatible,
// use `BDE_BUILD_TARGET_SAFE_2` instead.
//
// WARNING - In practice, declaring a destructor in some build modes but not
// others has led to subtle and difficult-to-diagnose failures.  DON'T DO IT!
//
// Finally, in very rare cases, we may want to put in (redundant) defensive
// code (in the spirit of `BSLS_ASSERT_OPT`) that is not part of the
// component-level contract, yet (1) is known to have negligible runtime cost
// and (2) is deemed to be so important as to be necessary even for optimized
// builds.
//
// For example, consider again the `MyDate` class above that now also declares
// a non-`inline` `print` method to format the current date value in some
// human-readable, but otherwise unspecified format:
// ```
// xyza_mydate.h
// ...
//  class MyDate {
//      // ...
//
//      // DATA
//      int d_serialDate;  // sequential representation within a valid range
//
//    public:
//      // ...
//      // ACCESSORS
//      // ...
//
//      /// Write the value of this object to the specified output `stream`
//      /// in some human-readable format, and return a reference to
//      /// `stream`.  Optionally specify ...
//      std::ostream& print(std::ostream& stream, ...) const;
//
//      // ...
//
//  };
// ```
// Successfully writing bad data is among the most insidious of bugs, because a
// latent error can persist and not be discovered until long after the program
// terminates.  Writing the value of a corrupted `MyDate` object in a
// **machine-readable** (binary) format is an error so serious as to warrant
// invoking
// ```
// void testFunction(int d_serialDate) {
//     BSLS_ASSERT_OPT(MyDateImpUtil::isValidSerialDate(d_serialDate));
// }
// ```
// each time we attempt the output operation; however, printing the value in a
// human-readable format intended primarily for debugging purposes is another
// matter.  In anything other than a safe build (which in this case would
// enforce essentially all method preconditions), it would be unfortunate if a
// developer, knowing that there was a problem involving the use of `MyDate`,
// inserted print statements to identify that problem, only to have the `print`
// method itself ruthlessly invoke the assert handler, likely terminating the
// process).  Moreover, it may also be unsafe even to attempt to format the
// value of a `MyDate` object whose `d_serialDate` value violates its
// invariants (e.g., due to a static table lookup).  In such cases we may, as
// sympathetic library developers, choose to implement different undefined
// (undocumented) redundant defensive behaviors, depending on the desired level
// of assertions:
// ```
// xyza_mydate.cpp
// ...
// #include <xyza_mydateimputil.h>
// ...
//
//  std::ostream& MyDate::print(std::ostream& stream, ...) const
//  {
//      // BSLS_ASSERT(/* any *argument* preconditions for this function */);
//
//      // Handle case where the invariants have been violated.
//
//  #ifdef BSLS_ASSERT_OPT_IS_ACTIVE
//      // Note that if 'BSLS_ASSERT_LEVEL_NONE' has been set, this code --
//      // along with all 'BSLS_ASSERT_OPT' macros -- will not instantiate,
//      // enabling us to verify that the combined runtime overhead of all such
//      // (redundant) defensive code is at worst negligible, if not
//      // unmeasurable.
//
//      if (!MyDateImpUtil::isValidSerialDate(d_serialDate)) {
//
//          // Our invariant is corrupted.
//
//  #ifdef BSLS_ASSERT_IS_ACTIVE
//          // Providing debugging information in this mode would be useful.
//
//          std::cerr << "\nxyza::MyDate: Invalid internal serial date value "
//                    << d_serialDate << '.' << std::endl;
//
//  #endif // BSLS_ASSERT_IS_ACTIVE
//
//          // In safe mode, each of the 'MyClass' methods fully guards its
//          // preconditions: There is simply no easy way to get here!
//
//          BSLS_ASSERT_SAFE("Probable rogue memory overwrite!" && 0);
//
//          // If we get here, we're corrupted, but not in safe mode!
//
//          return stream << "(* Invalid 'MyDate' State "
//                        << d_serialDate
//                        << " *)" << std::flush;                     // RETURN
//
//      }
//  #endif // BSLS_ASSERT_OPT_IS_ACTIVE
//
//      // If we get here in a production build, this object is "sane": Do
//      // whatever this 'print' method would normally do, assuming that no
//      // method preconditions or object invariants are violated.
//
//      // ...  <*** Your (Normal-Case) Formatting Code Here! ***>
//
//      return stream;
//  }
// ```
///Example 8: Conditional Compilation of Support Functions
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Occasionally a function may exist only to support a specific set of
// assertions.  Often this can happen when a large expression that captures a
// complicated precondition wants to be refactored into a distinct location to
// ease understanding of it.  When this happens the function might still remain
// as a private implementation detail of the class.
//
// When the only assertion macros that use the function are disabled this can
// lead to a compiler warning about a function being unused, and the
// corresponding code bloat from having the function available might be an
// overhead that is not desired.
//
// In order to totally remove the function when it is not needed, the `IS_USED`
// suffixed macros can be used to guard the declaration and definition of the
// function.  Suppose we have a `class` with a function having a complex
// precondition, and that precondition check is both private and only needed
// when the assertions that use it are enabled.  In that case, we can guard the
// definitions and declarations against even being compiled like this:
// ```
// class ComplexObject {
//     // ...
// #if defined(BSLS_ASSERT_SAFE_IS_USED)
//     /// Return `true` if the current state of this object fits within the
//     /// complex requirements of being sufficiently purple, false otherwise.
//     bool isPurplish() const;
// #endif
//     // ...
//   public:
//     // MANIPULATORS
//
//     /// Do something purpley.  The behavior is undefined unless this object
//     /// is currently purplish (contact customer support to know the current
//     /// threshholds for purplishness).
//     void doSomethingPurpley();
// };
//
// #if defined(BSLS_ASSERT_SAFE_IS_USED)
// bool ComplexObject::isPurplish() const
// {
//     // The real implementation would encode the complex logic of needing to
//     // determine if this object feels purplish at the moment.
//     return true;
// }
// #endif
//
// void ComplexObject::doSomethingPurpley()
// {
//     BSLS_ASSERT_SAFE(isPurplish());
// }
// ```
// Now, the `ComplexObject::isPurplish` function will only exist in a subset of
// builds:
// * When `BSLS_ASSERT_SAFE` assertions are enabled in assert or review mode,
//   the function will be compiled and invoked.
// * When `BSLS_ASSERT_VALIDATE_DISABLED_MACROS` is defined the function will
//   be compiled.  This will make sure that a future change does not
//   invalidate the implementation of `isPurplish()` even though it is not
//   used.
// * When `BSLS_ASSERT_SAFE` assertions are assumed the function will be
//   compiled and might be invoked, or at least have its implementation
//   inspected by the compiler to improve code generation.
//
///Example 9: Conditional Compilation of Support Code
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes there is code that needs to run in a function before an assertion
// to gather information needed only by that assertion.  Often this can be
// capturing input values or other system state before it is modified and
// verifying at the end of a function that the values are changed (or not)
// appropriately.
//
// When the corresponding assertion macro is not active in assert or review
// mode the supporting code should not be executed at all.  Importantly,
// because the capturing of additional information is an extra cost, the
// assertion itself does not lend itself to being assumed.
//
// Suppose we have a function that wishes to swap the values of its input:
// ```
// struct MySwapper {
//
//     /// Exchange the values of the specified `lhs` and `rhs`.
//     template <class T>
//     static void swap(T& lhs, T& rhs)
//     {
//         T tmp = lhs;
//         lhs = rhs;
//         rhs = tmp;
//     }
// };
// ```
// This works great as a simple `swap` implementation, but we would like to
// assert in safe mode that it is doing the correct thing.  In order to do that
// we need to capture the initial values of our inputs before doing anything
// else, and we want to do this only when the respective assertions are
// enabled.  Here we would guard our code and our assertions in a check that
// `BSLS_ASSERT_SAFE_IS_ACTIVE` is defined, like this:
// ```
// struct MySwapper {
//
//     /// Exchange the values of the specified `lhs` and `rhs`.
//     template <class T>
//     static void swap(T& lhs, T& rhs)
//     {
// #if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
//         T origLhs(lhs);
//         T origRhs(rhs);
// #endif
//         T tmp = lhs;
//         lhs = rhs;
//         rhs = tmp;
// #if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
//         BSLS_ASSERT_SAFE(rhs == origLhs);
//         BSLS_ASSERT_SAFE(lhs == origRhs);
// #endif
//     }
// };
// ```

#include <bsls_annotation.h>
#include <bsls_assertimputil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#ifdef BSLS_ASSERT_USE_CONTRACTS
#include <contract>
#endif

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLS_ASSERT_OPT)
#error BSLS_ASSERT_OPT is already defined!
#endif

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
#error BSLS_ASSERT_OPT_IS_ACTIVE is already defined!
#endif

#if defined(BSLS_ASSERT_OPT_IS_ASSUMED)
#error BSLS_ASSERT_OPT_IS_ASSUMED is already defined!
#endif

#if defined(BSLS_ASSERT_OPT_IS_REVIEW)
#error BSLS_ASSERT_OPT_IS_REVIEW is already defined!
#endif

#if defined(BSLS_ASSERT_OPT_IS_USED)
#error BSLS_ASSERT_OPT_IS_USED is already defined!
#endif

#if defined(BSLS_ASSERT)
#error BSLS_ASSERT is already defined!
#endif

#if defined(BSLS_ASSERT_IS_ACTIVE)
#error BSLS_ASSERT_IS_ACTIVE is already defined!
#endif

#if defined(BSLS_ASSERT_IS_ASSUMED)
#error BSLS_ASSERT_IS_ASSUMED is already defined!
#endif

#if defined(BSLS_ASSERT_IS_REVIEW)
#error BSLS_ASSERT_IS_REVIEW is already defined!
#endif

#if defined(BSLS_ASSERT_IS_USED)
#error BSLS_ASSERT_IS_USED is already defined!
#endif

#if defined(BSLS_ASSERT_SAFE)
#error BSLS_ASSERT_SAFE is already defined!
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
#error BSLS_ASSERT_SAFE_IS_ACTIVE is already defined!
#endif

#if defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
#error BSLS_ASSERT_SAFE_IS_ASSUMED is already defined!
#endif

#if defined(BSLS_ASSERT_SAFE_IS_REVIEW)
#error BSLS_ASSERT_SAFE_IS_REVIEW is already defined!
#endif

#if defined(BSLS_ASSERT_SAFE_IS_USED)
#error BSLS_ASSERT_SAFE_IS_USED is already defined!
#endif

#if defined(BSLS_ASSERT_INVOKE)
#error BSLS_ASSERT_INVOKE is already defined!
#endif

#if defined(BSLS_ASSERT_INVOKE_NORETURN)
#error BSLS_ASSERT_INVOKE_NORETURN is already defined!
#endif

                     // =================================
                     // (BSLS) "ASSERT" Macro Definitions
                     // =================================

// Implementation Note: We wrap the 'if' statement below in a (seemingly
// redundant) do-while-false loop to require, syntactically, a trailing
// semicolon, and to ensure that the macro behaves properly in an if-then-else
// context -- even if one forgets to wrap, with curly braces, the body of an
// 'if' having just a single 'BSLS_ASSERT*' statement.

               // =============================================
               // Factored Implementation for Internal Use Only
               // =============================================

#if !(defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE)   ||                             \
      defined(BSLS_ASSERT_LEVEL_ASSERT)        ||                             \
      defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)    ||                             \
      defined(BSLS_ASSERT_LEVEL_NONE)          ||                             \
      defined(BSLS_ASSERT_LEVEL_ASSUME_OPT)    ||                             \
      defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT) ||                             \
      defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE))
    #define BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED 1
#else
    #define BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED 0
#endif

#ifdef BSLS_ASSERT_USE_CONTRACTS
#define BSLS_ASSERT_ASSERT_IMP(X,LVL) [[ assert check_never_continue : X ]]

#define BSLS_ASSERT_ASSUME_IMP(X,LVL) [[ assert assume : X ]]
#define BSLS_ASSERT_ASSUME_ENABLED

#ifdef BSLS_ASSERT_VALIDATE_DISABLED_MACROS
#define BSLS_ASSERT_DISABLED_IMP(X,LVL) [[ assert ignore : X ]]
#else
#define BSLS_ASSERT_DISABLED_IMP(X,LVL)
#endif

#else
#define BSLS_ASSERT_ASSERT_IMP(X,LVL) do {                                    \
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!(X))) {                    \
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;                               \
            BloombergLP::bsls::Assert::invokeHandler(                         \
                BloombergLP::bsls::AssertViolation(#X,                        \
                                                   BSLS_ASSERTIMPUTIL_FILE,   \
                                                   BSLS_ASSERTIMPUTIL_LINE,   \
                                                   LVL));                     \
        }                                                                     \
    } while (false)

#if defined(BSLS_PLATFORM_CMP_CLANG)

#define BSLS_ASSERT_ASSUME_IMP(X,LVL) __builtin_assume((X)?true:false)
#define BSLS_ASSERT_ASSUME_ENABLED

#elif defined(BSLS_PLATFORM_CMP_GNU)
// While a common practice with GCC is to implement assumptions using an
// expression of the form 'if (!(X)) __builtin_unreachable();', this idiom
// relies on the expression 'X' being inline and truly sideeffect free before
// GCC will consistently elide the check itself, making this a pessimisation
// when used arbitrarily.  Because of this we do not attempt to leverage
// assumption on GCC and leave the following commented out:
//..
// #define BSLS_ASSERT_ASSUME_IMP(X,LVL) if (!(X)) { __builtin_unreachable(); }
// #define BSLS_ASSERT_ASSUME_ENABLED
//..

#elif defined (BSLS_PLATFORM_CMP_MSVC)
#define BSLS_ASSERT_ASSUME_IMP(X,LVL) __assume((X)?true:false)
#define BSLS_ASSERT_ASSUME_ENABLED

#endif

#if !defined(BSLS_ASSERT_ASSUME_IMP)
// The above flavors of 'assumption' are the only available ones that we
// support, and the only platforms where we currently attempt to use
// assumption.
#define BSLS_ASSERT_ASSUME_IMP(X,LVL) BSLS_ASSERT_DISABLED_IMP(X,LVL)
#endif

#ifdef BSLS_ASSERT_VALIDATE_DISABLED_MACROS
#define BSLS_ASSERT_DISABLED_IMP(X,LVL) (void)sizeof((!(X))?true:false)
#else
#define BSLS_ASSERT_DISABLED_IMP(X,LVL)
#endif
#endif

                              // ================
                              // BSLS_ASSERT_SAFE
                              // ================

// Determine if 'BSLS_ASSERT_SAFE' should be active.

// Define the control macros, also usable from client code.
#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE)                                    \
    || BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED && (                           \
           defined(BDE_BUILD_TARGET_SAFE_2) ||                                \
           defined(BDE_BUILD_TARGET_SAFE)         )
    #define BSLS_ASSERT_SAFE_IS_ACTIVE
#elif defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
    #define BSLS_ASSERT_SAFE_IS_REVIEW
#elif defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE) &&                               \
      !defined(BSLS_REVIEW_OPT_IS_ACTIVE) &&                                  \
      !defined(BSLS_REVIEW_IS_ACTIVE) &&                                      \
      !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
    #define BSLS_ASSERT_SAFE_IS_ASSUMED
#endif

// Indicate when 'BSLS_ASSERT_SAFE' arguments will be ODR-used.
#if defined(BSLS_ASSERT_SAFE_IS_REVIEW) ||                                    \
    defined(BSLS_ASSERT_SAFE_IS_ACTIVE) ||                                    \
    (defined(BSLS_ASSERT_SAFE_IS_ASSUMED)                                     \
     && defined(BSLS_ASSERT_ASSUME_ENABLED)) ||                               \
    defined(BSLS_ASSERT_VALIDATE_DISABLED_MACROS)
    #define BSLS_ASSERT_SAFE_IS_USED
#endif

// Define 'BSLS_ASSERT_SAFE' accordingly.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BSLS_ASSERT_SAFE(X) BSLS_ASSERT_ASSERT_IMP(                       \
                                     X,                                       \
                                     BloombergLP::bsls::Assert::k_LEVEL_SAFE)
#elif defined(BSLS_ASSERT_SAFE_IS_REVIEW)
    #define BSLS_ASSERT_SAFE(X) BSLS_REVIEW_REVIEW_IMP(                       \
                                     X,                                       \
                                     BloombergLP::bsls::Assert::k_LEVEL_SAFE)
#elif defined(BSLS_ASSERT_SAFE_IS_ASSUMED)
    #define BSLS_ASSERT_SAFE(X) BSLS_ASSERT_ASSUME_IMP(                       \
                                     X,                                       \
                                     BloombergLP::bsls::Assert::k_LEVEL_SAFE)
#else
    #define BSLS_ASSERT_SAFE(X) BSLS_ASSERT_DISABLED_IMP(                     \
                                     X,                                       \
                                     BloombergLP::bsls::Assert::k_LEVEL_SAFE)
#endif


                                // ===========
                                // BSLS_ASSERT
                                // ===========

// Determine if 'BSLS_ASSERT' should be active.

// Define the control macros, also usable from client code.
#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) ||                                 \
    defined(BSLS_ASSERT_LEVEL_ASSERT)                                         \
    || BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED && (                           \
           defined(BDE_BUILD_TARGET_SAFE_2) ||                                \
           defined(BDE_BUILD_TARGET_SAFE)   ||                                \
           !defined(BDE_BUILD_TARGET_OPT)         )
    #define BSLS_ASSERT_IS_ACTIVE
#elif defined(BSLS_REVIEW_IS_ACTIVE)
    #define BSLS_ASSERT_IS_REVIEW
#elif (defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE) ||                              \
       defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT)) &&                           \
      !defined(BSLS_REVIEW_OPT_IS_ACTIVE) &&                                  \
      !defined(BSLS_REVIEW_IS_ACTIVE) &&                                      \
      !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
    #define BSLS_ASSERT_IS_ASSUMED
#endif

// Indicate when 'BSLS_ASSERT' arguments will be ODR-used.
#if defined(BSLS_ASSERT_IS_REVIEW) ||                                         \
    defined(BSLS_ASSERT_IS_ACTIVE) ||                                         \
    (defined(BSLS_ASSERT_IS_ASSUMED)                                          \
     && defined(BSLS_ASSERT_ASSUME_ENABLED)) ||                               \
    defined(BSLS_ASSERT_VALIDATE_DISABLED_MACROS)
    #define BSLS_ASSERT_IS_USED
#endif

// Define 'BSLS_ASSERT' accordingly.

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLS_ASSERT(X) BSLS_ASSERT_ASSERT_IMP(                            \
                                   X,                                         \
                                   BloombergLP::bsls::Assert::k_LEVEL_ASSERT)
#elif defined(BSLS_ASSERT_IS_REVIEW)
    #define BSLS_ASSERT(X) BSLS_REVIEW_REVIEW_IMP(                            \
                                   X,                                         \
                                   BloombergLP::bsls::Assert::k_LEVEL_ASSERT)
#elif defined(BSLS_ASSERT_IS_ASSUMED)
    #define BSLS_ASSERT(X) BSLS_ASSERT_ASSUME_IMP(                            \
                                   X,                                         \
                                   BloombergLP::bsls::Assert::k_LEVEL_ASSERT)
#else
    #define BSLS_ASSERT(X) BSLS_ASSERT_DISABLED_IMP(                          \
                                   X,                                         \
                                   BloombergLP::bsls::Assert::k_LEVEL_ASSERT)
#endif

                              // ===============
                              // BSLS_ASSERT_OPT
                              // ===============

// Determine if 'BSLS_ASSERT_OPT' should be active.

// Define the control macros, also usable from client code.
#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) ||                                 \
    defined(BSLS_ASSERT_LEVEL_ASSERT) ||                                      \
    defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)                                     \
    || BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED
    #define BSLS_ASSERT_OPT_IS_ACTIVE
#elif defined(BSLS_REVIEW_OPT_IS_ACTIVE)
    #define BSLS_ASSERT_OPT_IS_REVIEW
#elif (defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE) ||                              \
       defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT) ||                            \
       defined(BSLS_ASSERT_LEVEL_ASSUME_OPT)) &&                              \
      !defined(BSLS_REVIEW_OPT_IS_ACTIVE) &&                                  \
      !defined(BSLS_REVIEW_IS_ACTIVE) &&                                      \
      !defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
    #define BSLS_ASSERT_OPT_IS_ASSUMED
#endif

// Indicate when 'BSLS_ASSERT_OPT' arguments will be ODR-used.
#if defined(BSLS_ASSERT_OPT_IS_REVIEW) ||                                     \
    defined(BSLS_ASSERT_OPT_IS_ACTIVE) ||                                     \
    (defined(BSLS_ASSERT_OPT_IS_ASSUMED)                                      \
     && defined(BSLS_ASSERT_ASSUME_ENABLED)) ||                               \
    defined(BSLS_ASSERT_VALIDATE_DISABLED_MACROS)
    #define BSLS_ASSERT_OPT_IS_USED
#endif

// Define 'BSLS_ASSERT_OPT' accordingly.

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLS_ASSERT_OPT(X) BSLS_ASSERT_ASSERT_IMP(                        \
                                      X,                                      \
                                      BloombergLP::bsls::Assert::k_LEVEL_OPT)
#elif defined(BSLS_ASSERT_OPT_IS_REVIEW)
    #define BSLS_ASSERT_OPT(X) BSLS_REVIEW_REVIEW_IMP(                        \
                                      X,                                      \
                                      BloombergLP::bsls::Assert::k_LEVEL_OPT)
#elif defined(BSLS_ASSERT_OPT_IS_ASSUMED)
    #define BSLS_ASSERT_OPT(X) BSLS_ASSERT_ASSUME_IMP(                        \
                                      X,                                      \
                                      BloombergLP::bsls::Assert::k_LEVEL_OPT)
#else
    #define BSLS_ASSERT_OPT(X) BSLS_ASSERT_DISABLED_IMP(                      \
                                      X,                                      \
                                      BloombergLP::bsls::Assert::k_LEVEL_OPT)
#endif

                             // ==================
                             // BSLS_ASSERT_INVOKE
                             // ==================

// 'BSLS_ASSERT_INVOKE' is always active and never in review mode or disabled.
#define BSLS_ASSERT_INVOKE(X) do {                                            \
        BloombergLP::bsls::Assert::invokeHandler(                             \
            BloombergLP::bsls::AssertViolation(                               \
                                  X,                                          \
                                  BSLS_ASSERTIMPUTIL_FILE,                    \
                                  BSLS_ASSERTIMPUTIL_LINE,                    \
                                  BloombergLP::bsls::Assert::k_LEVEL_INVOKE));\
    } while (false)

// 'BSLS_ASSERT_INVOKE_NORETURN' is always active and guaranteed to never
// return (by calling 'bsls::Assert::failByAbort') even if the installed
// handler does return.  Note that this macro expands into an infinite loop
// (that will always terminate the program in its first iteration) in order to
// suppress warnings even on platforms that do not understand the
// '[[noreturn]]' attribute.  Therefore, this macro can be used to implement
// code paths that are meant to be unreachable within functions having non-void
// return types.
#define BSLS_ASSERT_INVOKE_NORETURN(X) do {                                   \
        BloombergLP::bsls::Assert::invokeHandlerNoReturn(                     \
            BloombergLP::bsls::AssertViolation(                               \
                                  X,                                          \
                                  BSLS_ASSERTIMPUTIL_FILE,                    \
                                  BSLS_ASSERTIMPUTIL_LINE,                    \
                                  BloombergLP::bsls::Assert::k_LEVEL_INVOKE));\
    } while (true)

                    // ===================================
                    // BSLS_ASSERT_NORETURN_INVOKE_HANDLER
                    // ===================================

#ifdef BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER
#define BSLS_ASSERT_NORETURN_INVOKE_HANDLER BSLS_ANNOTATION_NORETURN
#else
#define BSLS_ASSERT_NORETURN_INVOKE_HANDLER
#endif

// A nested include guard is needed to support the test driver implementation.
#ifndef BSLS_ASSERT_RECURSIVELY_INCLUDED_TESTDRIVER_GUARD
#define BSLS_ASSERT_RECURSIVELY_INCLUDED_TESTDRIVER_GUARD

namespace BloombergLP {
namespace bsls {

                           // =====================
                           // class AssertViolation
                           // =====================

/// This class is an unconstrained *in-core* value-semantic class that
/// characterizes the details of a assert failure that has occurred.
class AssertViolation {

    // DATA
    const char *d_comment_p;      // the comment associated with the violation,
                                  // generally representing the expression that
                                  // failed

    const char *d_fileName_p;     // the name of the file where the violation
                                  // occurred

    int         d_lineNumber;     // the line number where the violation
                                  // occurred

    const char *d_assertLevel_p;  // the level and type of the violation that
                                  // occurred, generally one of the 'k_LEVEL'
                                  // constants defined in 'bsls::Review' or
                                  // 'bsls::Assert'

  public:
    // CREATORS

    /// Create a `AssertViolation` with the specified `comment`, `fileName`,
    /// `lineNumber`, and `assertLevel`.  Note that the supplied
    /// `assertLevel` will usually be one of the `k_LEVEL` constants defined
    /// in `bsls::Assert`
    BSLS_KEYWORD_CONSTEXPR
    AssertViolation(const char *comment,
                    const char *fileName,
                    int         lineNumber,
                    const char *assertLevel);

    // ACCESSORS

    /// Return the `assertLevel` attribute of this object.
    const char *assertLevel() const;

    /// Return the `comment` attribute of this object.
    const char *comment() const;

    /// Return the `fileName` attribute of this object.
    const char *fileName() const;

    /// Return the `lineNumber` attribute of this object.
    int lineNumber() const;
};

                                // ============
                                // class Assert
                                // ============

/// This "utility" class maintains a pointer containing the address of the
/// current assertion-failure handler function (of type
/// `Assert::ViolationHandler`) and provides methods to administer this
/// function pointer.  The `invokeHandler` method calls the
/// currently-installed failure handler.  This class also provides a suite
/// of standard failure-handler functions that are suitable to be installed
/// as the current `Assert::ViolationHandler` function.  Note that clients
/// are free to install any of these ("off-the-shelf") handlers, or to
/// provide their own ("custom") assertion-failure handler functions when
/// using this facility.  Also note that assertion-failure handler functions
/// must not return (i.e., they must `abort`, `exit`, `terminate`, `throw`,
/// or hang).
///
/// Finally, this class defines the constant strings that are passed as the
/// `reviewLevel` to the `bsls_review` handler for checks that failed in
/// "review mode" (see {Assertion Modes}).
class Assert {

  public:
    // TYPES

    /// `ViolationHandler` is an alias for a pointer to a function returning
    /// `void`, and taking, as a parameter a single `const` reference to a
    /// `bsls::AssertViolation` -- e.g.,
    /// ```
    /// void myHandler(const bsls::AssertViolation&);
    /// ```
    typedef void (*ViolationHandler)(const AssertViolation&);

    /// `Handler` is an alias for a pointer to a function returning `void`,
    /// and taking, as parameters, two null-terminated strings and an `int`,
    /// which is the structure of all assertion-failure handler functions
    /// supported by this class -- e.g.,
    /// ```
    /// void myHandler(const char *text, const char *file, int line);
    /// ```
    typedef void (*Handler)(const char *, const char *, int);

  private:
    // FRIENDS
    friend class AssertFailureHandlerGuard;

    // PRIVATE CLASS METHODS

    /// Make the specified handler `function` the current assertion-failure
    /// handler.
    static void setViolationHandlerRaw(Assert::ViolationHandler function);

    /// Make the specified `function` the current legacy handler and set the
    /// current assertion-failure handler to `failOnViolation`.
    static void setFailureHandlerRaw(Assert::Handler function);

    /// Get the `comment`, `fileName`, and `lineNumber` from the specified
    /// `violation` and pass them to the registered `Handler`.  This
    /// function exists to provide support for the older signature for
    /// assertion-failure handlers specified by `Handler`.
    static void failOnViolation(const AssertViolation& violation);

  public:
    // PUBLIC CONSTANTS

                     // 'assertLevel' Strings

    static const char k_LEVEL_SAFE[];
    static const char k_LEVEL_OPT[];
    static const char k_LEVEL_ASSERT[];
    static const char k_LEVEL_INVOKE[];

    // PUBLIC CLASS DATA
    static const char *k_permitOutOfPolicyReturningAssertionBuildKey;
                                       // This constant has the value "No".
                                       // See {Assertion Handler Policy}.

    // CLASS METHODS

                      // Administrative Methods

    /// Make the specified violation handler `function` the current
    /// assertion-failure handler.  This method has no effect if the
    /// `lockAssertAdministration` method has been called.
    static void setViolationHandler(Assert::ViolationHandler function);

    /// Make the specified handler `function` the current assertion-failure
    /// handler.  This method has no effect if the
    /// `lockAssertAdministration` method has been called.
    static void setFailureHandler(Assert::Handler function);

    /// Disable all subsequent calls to `setFailureHandler`.  Note that this
    /// method has no effect on the behavior of a
    /// `AssertFailureHandlerGuard` object.
    static void lockAssertAdministration();

    /// Return the address of the currently installed assertion-failure
    /// handler function if it is a `Handler` (and not a
    /// `ViolationHandler`); otherwise, return `NULL`.
    static Assert::Handler failureHandler();

    /// Return the address of the currently installed assertion-failure
    /// handler function.
    static Assert::ViolationHandler violationHandler();

                // Dispatcher Method (called from within macros)

    /// Invoke the currently installed assertion-failure handler function
    /// with the specified `violation`.  The behavior is undefined if the
    /// macro `BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER` is defined,
    /// and the currently installed assertion-failure handler function
    /// returns to the caller (i.e., the assertion handler does *not*
    /// `abort`, `exit`, `terminate`, `throw`, or hang).  Note that this
    /// function is intended for use by the (BSLS) "ASSERT" macros, but may
    /// also be called by clients directly as needed (preferably with
    /// `BSLS_ASSERT_INVOKE`).  Also note that the configuration macro
    /// `BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER` is intended to
    /// support static analysis tools, which require an annotation to see
    /// that a failed "ASSERT" prevents further execution of a function with
    /// "bad" values.
    BSLS_ASSERT_NORETURN_INVOKE_HANDLER
    static void invokeHandler(const AssertViolation& violation);

    /// Invoke the currently installed assertion-failure handler function
    /// with the specified expression `text`, `file` name, and `line` number
    /// as its arguments.  The behavior is undefined if the macro
    /// `BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER` is defined, and the
    /// currently installed assertion-failure handler function returns to
    /// the caller (i.e., the assertion handler does *not* `abort`, `exit`,
    /// `terminate`, `throw`, or hang).  Note that this function is
    /// deprecated, as the (BSLS) "ASSERT" macros all now use the
    /// `bsls::AssertViolation` overload of `invokeHandler` instead.
    ///
    /// @DEPRECATED: Use `invokeHandler(const AssertViolation&)` instead.
    BSLS_ASSERT_NORETURN_INVOKE_HANDLER
    static void invokeHandler(const char *text, const char *file, int line);

    /// Invoke the currently installed assertion-failure handler function
    /// with the specified `violation`.  If the handler returns normally,
    /// invoke `bsls::Assert::failByAbort`.
    BSLS_ANNOTATION_NORETURN
    static void invokeHandlerNoReturn(const AssertViolation &violation);

#ifdef BSLS_ASSERT_USE_CONTRACTS
    /// Call `invokeHandler` with an `AssertViolation` with properties from
    /// the specified `violation`.
    static void invokeLanguageContractHandler(
                                     const std::contract_violation& violation);
#endif

                      // Standard Assertion-Failure Handlers

    /// (Default Handler) Emulate the invocation of the standard `assert`
    /// macro with a `false` argument, using the expression `comment`,
    /// `file` name, and `line` number from the specified `violation` to
    /// generate a helpful output message and then, after logging,
    /// unconditionally aborting.  Note that this handler function is the
    /// default installed assertion handler.
    BSLS_ANNOTATION_NORETURN
    static void failByAbort(const AssertViolation& violation);

    /// Use the expression `comment`, `file` name, and `line` number from
    /// the specified `violation` to generate a helpful output message and
    /// then, after logging, spin in an infinite loop.  Note that this
    /// handler function is useful for hanging a process so that a debugger
    /// may be attached to it.
    BSLS_ANNOTATION_NORETURN
    static void failBySleep(const AssertViolation& violation);

    /// Throw an `AssertTestException` whose attributes are the `comemnt`,
    /// `file`, `line`, and `level` from the specified `violation` provided
    /// that `BDE_BUILD_TARGET_EXC` is defined; otherwise, log an
    /// appropriate message and abort the program (similar to
    /// `failByAbort`).
    BSLS_ANNOTATION_NORETURN
    static void failByThrow(const AssertViolation& violation);

    /// **DEPRECATED**: Use `failByAbort` instead.
    ///
    /// Emulate the invocation of the standard `assert` macro with a `false`
    /// argument, using the specified expression `comment`, `file` name, and
    /// `line` number to generate a helpful output message and then, after
    /// logging, unconditionally aborting.
    BSLS_ANNOTATION_NORETURN
    static void failAbort(const char *comment, const char *file, int line);

    /// **DEPRECATED**: Use `failBySleep` instead.
    ///
    /// Use the specified expression `comment`, `file` name, and `line`
    /// number to generate a helpful output message and then, after logging,
    /// spin in an infinite loop.  Note that this handler function is useful
    /// for hanging a process so that a debugger may be attached to it.
    BSLS_ANNOTATION_NORETURN
    static void failSleep(const char *comment, const char *file, int line);

    /// **DEPRECATED**: Use `failByThrow` instead.
    ///
    /// Throw an `AssertTestException` whose attributes are the specified
    /// `comemnt`, `file`, `line`, and `level` provided that
    /// `BDE_BUILD_TARGET_EXC` is defined; otherwise, log an appropriate
    /// message and abort the program (similar to `failAbort`).
    BSLS_ANNOTATION_NORETURN
    static void failThrow(const char *comment, const char *file, int line);


                    // Assertion Handler Policy Enforcement

    /// Return `true` if `k_permitOutOfPolicyReturningAssertionBuildKey`
    /// does not have the value "bsls-PermitOutOfPolicyReturn" or
    /// `permitOutOfPolicyReturningFailureHandler` has not previously been
    /// invoked, and `false` otherwise.  Note that returning `true`
    /// indicates that `bsls::Assert` should abort the task if the currently
    /// installed assertion-failure handler returns normally (after the
    /// detection of a failed assertion).
    static bool abortUponReturningAssertionFailureHandler();

    /// DO NOT USE!  It is a violation of Bloomberg policy to invoke this
    /// function without having prior authorization from senior management.
    ///
    /// Allow an assertion handler to return control to the calling function
    /// (after a failed assertion).  The behavior is undefined if
    /// `BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER` is defined (and
    /// thus `invokeHandler` would not be able to return anyway).  Note
    /// that, by default, an assertion handler that attempts to return
    /// normally will cause the program to be aborted.
    ///
    /// Internal Bloomberg users should contact the BDE team if they feel
    /// their application might need to violate Bloomberg policy by allowing
    /// the currently installed assertion handler to return normally (after
    /// a failed assertion).
    static void permitOutOfPolicyReturningFailureHandler();
};

                      // ===============================
                      // class AssertFailureHandlerGuard
                      // ===============================

/// An object of this class saves the current assert handler and installs
/// the one specified on construction.  On destruction, the original assert
/// handler is restored.  Note that two objects of this class cannot be
/// safely used concurrently from two separate threads (but may of course
/// appear sequentially, including in nested blocks and function invocations
/// within a single thread).  Note that the behavior of objects of this
/// class is unaffected by the (`static`) `Assert::lockAssertAdministration`
/// method (i.e., the temporary replacement will occur, regardless of
/// whether that method has been invoked.)
class AssertFailureHandlerGuard {

    // DATA
    Assert::ViolationHandler d_original;       // original handler
    Assert::Handler          d_legacyOriginal; // original legacy handler

  private:
    // NOT IMPLEMENTED
    AssertFailureHandlerGuard(const AssertFailureHandlerGuard&);
    AssertFailureHandlerGuard& operator=(const AssertFailureHandlerGuard&);

  public:
    // CREATORS

    /// Create a guard object that installs the specified `temporary`
    /// failure handler and automatically restores the original handler on
    /// destruction.
    explicit AssertFailureHandlerGuard(Assert::ViolationHandler temporary);
    explicit AssertFailureHandlerGuard(Assert::Handler temporary);

    /// Restore the failure handler that was in place when this object was
    /// created and destroy this guard.
    ~AssertFailureHandlerGuard();
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY

// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -SLM01
// BDE_VERIFY pragma: -CP01
// BDE_VERIFY pragma: -TR04
// BDE_VERIFY pragma: -TR17

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                         // =========================
                         // BDE_ASSERT_H (deprecated)
                         // =========================

// Active in "Safe Mode"

#define BDE_ASSERT_H(X) BSLS_ASSERT_SAFE(X)
#define BSL_ASSERT_H(X) BSLS_ASSERT_SAFE(X)    // introduced during migration

                        // ===========================
                        // BDE_ASSERT_CPP (deprecated)
                        // ===========================

// Active in "Safe Mode" and "Debug Mode"

#define BDE_ASSERT_CPP(X) BSLS_ASSERT(X)
#define BSL_ASSERT_CPP(X) BSLS_ASSERT(X)       // introduced during migration

/// This alias is defined for backward compatibility.
typedef bsls::Assert bdes_Assert;

/// This alias is defined for backward compatibility.
typedef bsls::AssertFailureHandlerGuard bdes_AssertFailureHandlerGuard;

                         // ==========================
                         // BSLS_ASSERT_H (deprecated)
                         // ==========================

// Old 'BSLS_ASSERT' implementation macro that was incorrectly used externally.

#define BSLS_ASSERT_ASSERT(X) BSLS_ASSERT_ASSERT_IMP(                         \
                                   X,                                         \
                                   BloombergLP::bsls::Assert::k_LEVEL_ASSERT)

#endif // BDE_OMIT_INTERNAL_DEPRECATED

/// This alias is defined for backward compatibility.
typedef bsls::Assert bsls_Assert;

/// This alias is defined for backward compatibility.
typedef bsls::AssertFailureHandlerGuard bsls_AssertFailureHandlerGuard;

#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

// BDE_VERIFY pragma: pop

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace bsls {
                           // ---------------------
                           // class AssertViolation
                           // ---------------------

// CREATORS
BSLS_KEYWORD_CONSTEXPR
inline
AssertViolation::AssertViolation(const char *comment,
                                 const char *fileName,
                                 int         lineNumber,
                                 const char *assertLevel)
: d_comment_p((comment == 0) ? "" : comment)
, d_fileName_p((fileName == 0) ? "" : fileName)
, d_lineNumber(lineNumber)
, d_assertLevel_p((assertLevel == 0) ? "" : assertLevel)
{
}

// ACCESSORS
inline
const char *AssertViolation::assertLevel() const
{
    return d_assertLevel_p;
}

inline
const char *AssertViolation::comment() const
{
    return d_comment_p;
}

inline
const char *AssertViolation::fileName() const
{
    return d_fileName_p;
}

inline
int AssertViolation::lineNumber() const
{
    return d_lineNumber;
}

}  // close package namespace
}  // close enterprise namespace

#endif // deeper include guard

          // ========================================================
          // UNDEFINE THE LOCALLY-SCOPED IMPLEMENTATION DETAIL MACROS
          // ========================================================

#undef BSLS_ASSERT_NORETURN_INVOKE_HANDLER
#undef BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED
#undef BSLS_ASSERT_ASSUME_ENABLED

                 // =========================================
                 // IMPLEMENTATION USING THE C++ PREPROCESSOR
                 // =========================================

// At most one of the following build options may be set during the compilation
// of any component that includes 'bsls_assert.h':
// ```
// BSLS_ASSERT_LEVEL_ASSERT_SAFE
// BSLS_ASSERT_LEVEL_ASSERT
// BSLS_ASSERT_LEVEL_ASSERT_OPT
// BSLS_ASSERT_LEVEL_NONE
// BSLS_ASSERT_LEVEL_ASSUME_SAFE
// BSLS_ASSERT_LEVEL_ASSUME_ASSERT
// BSLS_ASSERT_LEVEL_ASSUME_OPT
// ```
// ----------------------------------------------------------------------------

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) &&                                 \
    defined(BSLS_ASSERT_LEVEL_ASSERT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_SAFE and ..._LEVEL_ASSERT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) &&                                 \
    defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_SAFE and ..._LEVEL_ASSERT_OPT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) &&                                 \
    defined(BSLS_ASSERT_LEVEL_NONE)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_SAFE and ..._LEVEL_NONE
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) &&                                 \
    defined(BSLS_ASSERT_LEVEL_ASSUME_OPT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_SAFE and ..._LEVEL_ASSUME_OPT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) &&                                 \
    defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_SAFE and ..._LEVEL_ASSUME_ASSERT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) &&                                 \
    defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_SAFE and ..._LEVEL_ASSUME_SAFE
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT) &&                                      \
    defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT and ..._LEVEL_ASSERT_OPT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT) &&                                      \
    defined(BSLS_ASSERT_LEVEL_NONE)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT and ..._LEVEL_NONE
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT) &&                                      \
    defined(BSLS_ASSERT_LEVEL_ASSUME_OPT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT and ..._LEVEL_ASSUME_OPT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT) &&                                      \
    defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT and ..._LEVEL_ASSUME_ASSERT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT) &&                                      \
    defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT and ..._LEVEL_ASSUME_SAFE
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_OPT) &&                                  \
    defined(BSLS_ASSERT_LEVEL_NONE)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_OPT and ..._LEVEL_NONE
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_OPT) &&                                  \
    defined(BSLS_ASSERT_LEVEL_ASSUME_OPT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_OPT and ..._LEVEL_ASSUME_OPT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_OPT) &&                                  \
    defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_OPT and ..._LEVEL_ASSUME_ASSERT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_OPT) &&                                  \
    defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSERT_OPT and ..._LEVEL_ASSUME_SAFE
#endif

#if defined(BSLS_ASSERT_LEVEL_NONE) &&                                        \
    defined(BSLS_ASSERT_LEVEL_ASSUME_OPT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_NONE and ..._LEVEL_ASSUME_OPT
#endif

#if defined(BSLS_ASSERT_LEVEL_NONE) &&                                        \
    defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_NONE and ..._LEVEL_ASSUME_ASSERT
#endif

#if defined(BSLS_ASSERT_LEVEL_NONE) &&                                        \
    defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_NONE and ..._LEVEL_ASSUME_SAFE
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSUME_OPT) &&                                  \
    defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSUME_OPT and ..._LEVEL_ASSUME_ASSERT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSUME_OPT) &&                                  \
    defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSUME_OPT and ..._LEVEL_ASSUME_SAFE
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT) &&                               \
    defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE)
#error incompatible BSLS_ASSERT levels:                                       \
..._LEVEL_ASSUME_ASSERT and ..._LEVEL_ASSUME_SAFE
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
