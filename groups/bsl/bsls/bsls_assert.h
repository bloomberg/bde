// bsls_assert.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLS_ASSERT
#define INCLUDED_BSLS_ASSERT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide build-specific, runtime-configurable assertion macros.
//
//@CLASSES:
//  bsls::Assert: namespace for "assert" management functions
//  bsls::AssertFailureHandlerGuard: scoped guard for changing handlers safely
//
//@MACROS:
// BSLS_ASSERT: runtime check typically enabled in safe and non-opt build modes
// BSLS_ASSERT_SAFE: runtime check typically only enabled in safe build modes
// BSLS_ASSERT_OPT: runtime check typically enabled in all build modes
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides three "assert-like" macros,
// 'BSLS_ASSERT', 'BSLS_ASSERT_SAFE', and 'BSLS_ASSERT_OPT', which can be used
// to enable optional *redundant* runtime checks in corresponding build modes.
// If an assertion argument evaluates to 0, a runtime-configurable "handler"
// function is invoked with the current filename, line number, and (0-valued
// expression) argument text.
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
// There are three important aspects of assertions: (1) !Every! !assertion!
// !is! !redundant!; it is essential that if all assertions are compiled out of
// a program that is defect-free, apart from improved runtime performance, the
// program behaves identically.  Hence, (2) !each! !boolean-valued! !assert!
// !argument! !must! !have! !no! !side-effects!.  Finally, (3) !assertions!
// !do! !not! !affect! !binary! !compatibility!; hence, translation units with
// different assertion levels (but not necessarily build targets) can safely be
// combined into a single program (see "Build Modes" and "Assertions in Header
// Files" below).  Note that the build target 'BDE_BUILD_TARGET_SAFE_2' does
// permit binary incompatibility for conditionally compiled source code, but
// there is no corresponding 'BSLS_ASSERT_SAFE_2' assertion macro (see {Usage}
// below).
//
///Detailed Behavior
///- - - - - - - - -
// If an assertion fires (i.e., due to a 0-valued expression argument), there
// is either a contract violation or some other logic error; the program is now
// in an undefined state, and the goal of the assertion (if enabled) is to
// report the precise location and nature of the defect *quickly* and *loudly*.
// Assertions are enabled at compile time by the presence or absence of
// relevant *build* *targets* and *assertion-level* *flags* (see "Build Modes"
// below).  When enabled, each of the three flavors of (BSLS) "ASSERT" macros
// provided in this component does the same thing: Each macro tests the
// predicate expression 'X', and, if '!(X)' is 'true', invokes the currently
// installed ('void'-valued) handler function having the signature
// '(const char *, const char *, int)', passing the textual rendering of the
// predicate ('#X'), '__FILE__', and '__LINE__'.  Note that if an "ASSERT"
// macro is not enabled by the appropriate build flags, it is not instantiated
// (i.e., it expands to nothing in the preprocessor).
//
///Selecting Which of the Three ASSERT Macros to Use
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The choice of which specific macro to use is governed primarily by the
// relative runtime cost of (a) evaluating the expression needed to detect a
// potential defect (e.g., an out-of-range argument to a function) compared
// with (b) the useful productive work being done (e.g., by the function):
//..
//      Recommended    Relative overhead (a/b) of (a) evaluating the particular
//    (BSLS) "ASSERT"   argument expression compared with (b) the runtime cost
//   Macro to be used.   of performing concomitant useful (productive) work.
//  --------------------  ---------------------------------------------------
//   #       Macro Name         Rel.  Cost      Description of ratio (a/b)
//  ---   ----------------     ------------    ----------------------------
//  I.    BSLS_ASSERT_SAFE     Expensive       (order of magnitude overhead)
//
//  II.   BSLS_ASSERT          Inexpensive     (no more than 5-10% overhead)
//
//  III.  BSLS_ASSERT_OPT      Negligible      (no observable overhead)
//..
// The most typical asserts are 'BSLS_ASSERT', which may add some measurable
// overhead, compared to the runtime cost of a (non-'inline') function call,
// but not much.  We would expect to incur this overhead in, say, a "debug
// mode" build, but not, for example, in an "opt mode" one.  For inline
// functions with even simple defensive checks, the runtime cost of performing
// the check may be substantial (i.e., > 10%) compared to that of the useful
// work being done.  In such cases, we will prefer the 'BSLS_ASSERT_SAFE' macro
// to ensure that instantiation occurs only in "safe mode", where the expensive
// extra check is presumably needed to help resolve a specific problem, and
// substantial extra overhead is a non-issue.  Finally, in situations where the
// cost of the defensive check is truly negligible and failing to detect a
// defect or misuse could result in catastrophe, we may instead elect to use
// the 'BSLS_ASSERT_OPT' macro, which instantiates even in "opt mode", where
// runtime performance is presumed to be at a premium.
//
///Build Modes (Build Targets and Assertion-Level Flags)
///-----------------------------------------------------
// The *build* *mode* for this component defines the amount of defensive
// checking that will be incorporated (at compile time) into the translation
// unit.  For example, "safe mode" implies that all three of the (BSL) "ASSERT"
// macros above will instantiate, while "debug mode" implies that only
// 'BSLS_ASSERT' and 'BSLS_ASSERT_OPT' will do so.  Building in "opt mode"
// implies that only 'BSLS_ASSERT_OPT' instantiates, and "none mode" would mean
// that all "ASSERT" macros will be compiled out.  Note that this final build
// mode is useful during testing to ensure, as required (see "Assertion
// Semantics" above), that defensive checks performed using the
// 'BSLS_ASSERT_OPT' macro (1) incur no significant (observable) runtime
// overhead, (2) cause no change in behavior, and (3) do not affect binary
// compatibility.
//
// A particular build mode is implied by the relevant (BDE) build targets and
// (BSLS) assertion-level flags (i.e., preprocessor macros) that are defined at
// compilation (preprocessing) time -- e.g., by supplying one or more '-D'
// options, each followed immediately by the relevant macro name on the (Unix)
// command line.  The presence (or absence) of definitions for the following
// build targets and (subsequently) assertion-level flags determine the build
// mode, and therefore whether the bodies of each of the three "ASSERT" macros
// (see previous section) are compiled into the translation unit.
//
// (BDE) build targets are defined by the development environment, and not by
// any component within the software repository.  The following table shows the
// three (BDE) build targets that can affect which (BSLS) "ASSERT" macros
// instantiate.
//..
//        (BDE) Build Targets
//      -----------------------
//  (A) BDE_BUILD_TARGET_SAFE_2
//  (B) BDE_BUILD_TARGET_SAFE
//  (C) BDE_BUILD_TARGET_OPT
//..
// *Any* of the 8 possible combinations of the three build targets, e.g.,
// 'BDE_BUILD_TARGET_OPT' and 'BDE_BUILD_TARGET_SAFE_2', may be defined.  By
// contrast, the four (BSLS) assertion-level flags, shown below (e.g.,
// 'BSLS_ASSERT_LEVEL_NONE'), are specific to this component.
//
// The following table shows the four mutually exclusive (BSLS) assertion-level
// flags.  (Note that, by default, the 'BSLS_ASSERT_SAFE' macro is disabled,
// and the 'BSLS_ASSERT' and 'BSLS_ASSERT_OPT' macros are enabled; the
// 'BSLS_ASSERT' macro is disabled when the 'BDE_BUILD_TARGET_OPT' flag is
// defined and not overruled by also defining the 'BDE_BUILD_TARGET_SAFE'
// macro.)
//..
//       (BSLS) Assertion-Level Flags
//      ------------------------------
//  (1) BSLS_ASSERT_LEVEL_ASSERT_SAFE
//  (2) BSLS_ASSERT_LEVEL_ASSERT
//  (3) BSLS_ASSERT_LEVEL_ASSERT_OPT
//  (4) BSLS_ASSERT_LEVEL_NONE
//..
// At most *one* of the above flags may be defined during the translation of
// any component that includes 'bsls_assert.h', or that component will fail to
// compile (producing a useful diagnostic message).  If an assertion-level flag
// is defined, it overrides the default instantiation behavior for every
// combination of build-target definitions.  Note, however, that the precise
// behavior of each of the three (BSLS) "ASSERT" macros, when enabled, depends
// on the particular assert handler that is currently installed (see
// "Runtime-Configurable Assertion-Failure Behavior" below).
//
///Build Targets Only
/// - - - - - - - - -
// If no assertion-level flags are defined (see "Assertion-Level Flags" below),
// the presence or absence of the definitions of the three relevant build
// targets control the build mode, and, therefore, which of the (BSLS) "ASSERT"
// macros will be active (i.e., will instantiate):
//..
//  ===========================================================================
//   "ASSERT" Macro Instantiation Assuming NO Assertion-Level Flags are Defined
//  ===========================================================================
//  --- BDE_BUILD_TARGET ----   ------------ BSLS "ASSERT" MACRO --------------
//    (A)      (B)      (C)            I.            II.            III.
//  _SAFE_2   _SAFE    _OPT     BSLS_ASSERT_SAFE  BSLS_ASSERT   BSLS_ASSERT_OPT
//  -------  -------  -------   ----------------  ------------  ---------------
//                                                INSTANTIATES   INSTANTIATES
//                    DEFINED                                    INSTANTIATES
//           DEFINED             INSTANTIATES     INSTANTIATES   INSTANTIATES
//           DEFINED  DEFINED    INSTANTIATES     INSTANTIATES   INSTANTIATES
//  DEFINED                      INSTANTIATES     INSTANTIATES   INSTANTIATES
//  DEFINED           DEFINED    INSTANTIATES     INSTANTIATES   INSTANTIATES
//  DEFINED  DEFINED             INSTANTIATES     INSTANTIATES   INSTANTIATES
//  DEFINED  DEFINED  DEFINED    INSTANTIATES     INSTANTIATES   INSTANTIATES
//
//  ==========================================================================
//..
// As the table above illustrates, if no build targets are defined, then both
// 'BSLS_ASSERT' and 'BSLS_ASSERT_OPT' instantiate.  If 'BDE_BUILD_TARGET_OPT'
// is defined and neither 'BDE_BUILD_TARGET_SAFE' nor 'BDE_BUILD_TARGET_SAFE_2'
// is defined, then only 'BSLS_ASSERT_OPT' instantiates.  If, however, either
// 'BDE_BUILD_TARGET_SAFE' or 'BDE_BUILD_TARGET_SAFE_2' is defined, then all
// three "ASSERT" macros, 'BSLS_ASSERT_SAFE', 'BSLS_ASSERT', and
// 'BSLS_ASSERT_OPT', instantiate (regardless of whether or not
// 'BDE_BUILD_TARGET_OPT' happens to be defined).
//
///Assertion-Level Flags
///- - - - - - - - - - -
// With respect to the instantiation of the three (BSLS) "ASSERT" macros, if
// any one of the four assertion-level flags is defined, all definitions of
// *build* *targets* are ignored, and that flag alone controls which (BSLS)
// "ASSERT" macros will instantiate.  (Multiple assertion-level flag
// definitions are reported as errors at compile time for every translation
// unit that includes 'bsls_assert.h'.)  The following table indicates the
// effect of defining each of the four mutually exclusive (BSLS) assertion-
// level flags on the instantiation of each of the four (BSLS) "ASSERT" macros:
//..
//  ===========================================================================
//  "ASSERT" Macro Instantiation Assuming *ONE* Assertion-Level Flag is Defined
//  ===========================================================================
//   BSLS Assertion-Level Flag |~~~~~~~~~~~~ BSLS "ASSERT" MACROS ~~~~~~~~~~~~|
//                                    I.              II.            III.
//         BSLS_ASSERT_...     BSLS_ASSERT_SAFE  BSLS_ASSERT   BSLS_ASSERT_OPT
//      ---------------------  ----------------  ------------  ---------------
//  (1) ..._LEVEL_ASSERT_SAFE    INSTANTIATES    INSTANTIATES    INSTANTIATES
//  (2) ..._LEVEL_ASSERT                         INSTANTIATES    INSTANTIATES
//  (3) ..._LEVEL_ASSERT_OPT                                     INSTANTIATES
//  (4) ..._LEVEL_NONE
//
//  ==========================================================================
//..
// As the table above illustrates, if the 'BSLS_ASSERT_LEVEL_ASSERT_SAFE'
// assertion-level flag is defined, then all three macros, 'BSLS_ASSERT_SAFE',
// 'BSLS_ASSERT', and 'BSLS_ASSERT_OPT', instantiate.  If instead the
// 'BSLS_ASSERT_LEVEL_ASSERT' flag is defined, then only 'BSLS_ASSERT' and
// 'BSLS_ASSERT_OPT' instantiate.  Otherwise, if 'BSLS_ASSERT_LEVEL_ASSERT_OPT'
// is the only assertion-level flag defined, then just 'BSLS_ASSERT_OPT'
// instantiates.  Finally, if the 'BSLS_ASSERT_LEVEL_NONE' flag is the one
// defined then none of the three "ASSERT" macros will instantiate.
//
///Runtime-Configurable Assertion-Failure Behavior
///-----------------------------------------------
// In addition to the three (BSLS) "ASSERT" macros, 'BSLS_ASSERT',
// 'BSLS_ASSERT_SAFE', and 'BSLS_ASSERT_OPT', this component provides (1) an
// 'invokeHandler' method used (primarily) to implement these three "ASSERT"
// macros and enable their runtime configuration, (2) administration methods to
// configure, at runtime, the behavior resulting from an assertion failure
// (i.e., by installing an appropriate assertion-failure handler function), and
// (3) a suite of standard ("off-the-shelf") assertion-failure handler
// functions, to be installed via the administrative methods (if desired), and
// invoked by the 'invokeHandler' method on an assertion failure.
//
// When an assertion fails, the currently installed *failure* *handler*
// ("callback") function is invoked.  The default handler is the ('static')
// 'bsls::Assert::failAbort' method; a user may replace this default handler by
// using the ('static') 'bsls::Assert::setFailureHandler' administrative method
// and passing it (the address of) a function whose signature conforms to the
// 'bsls::Assert::Handler' 'typedef'.  This handler may be one of the other
// handler methods provided in 'bsls::Assert', or a new "custom" function,
// written by the user (see {Usage} below).
//
///Exception-Throwing Failure Handlers and 'bsls::AssertFailureHandlerGuard'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Among the failure handlers provided is 'bsls::Assert::failThrow', which
// throws an 'bsls::AssertTestException' object.  Throwing an exception,
// however, is not safe in all environments and deliberately aborting is more
// useful in a debugging context than throwing an unhandled exception.  Hence,
// in order for an 'bsls::AssertTestException' object to be thrown on an
// assertion failure, the user must first install the 'bsls::Assert::failThrow'
// handler (or another exception-throwing handler) explicitly.
//
// Note that an object of type 'bsls::AssertFailureHandlerGuard' can be used to
// temporarily set an exception-throwing handler within a 'try' block,
// automatically restoring the previous handler when the 'try' block exits (see
// {Usage} below).
//
///Assertions in Header Files (Mixing Build Options Across Translation Units)
///--------------------------------------------------------------------------
// Mixing build modes across translation units, although not strictly
// conformant with the the C++ language standard, is permissible in practice;
// however, the defensive checks that are enabled may be unpredictable.  The
// *one-definition* *rule* states that if there are multiple definitions of an
// object or function within a program, these definitions *must* be identical
// or else the program is *ill-formed*.  Unfortunately, definitions in header
// files may not be identical across object ('.o') files if the build targets
// or assertion-level flags defined during translation (preprocessing) are not
// the same.
//
// For example, consider an 'inline' function that sets the width of a 'Square'
// and optionally checks for (defends against) a negative 'width' argument:
//..
//  // our_square.h
//  // ...
//
//  inline
//  void Square::setWidth(int width)
//  {
//      BSLS_ASSERT_SAFE(width >= 0);
//
//      d_width = width;
//  }
//..
// Now consider a client that uses this 'setWidth' method:
//..
//  // my_client.cpp
//  // ...
//  void f()
//  {
//      Square s;
//      s.setWidth(-5);
//  }
//..
// We can build the 'our_square' component in "safe mode" -- e.g., by
// incorporating '-DBSLS_ASSERT_LEVEL_ASSERT_SAFE' on the (Unix) command line.
// Notice, however, that building client software against a version of
// 'our_square.o' compiled in "safe mode" does *not* ensure that all of the
// 'BSLS_ASSERT_SAFE' macros will be active (will instantiate); instead, the
// client's build mode will (most likely) govern those instantiations of the
// 'BSLS_ASSERT_SAFE' macro located within the library.  The only way to ensure
// that all of the 'BSLS_ASSERT_SAFE' macros instantiate is to build the
// *client* as well as the library software in "safe mode".
//
// Inline functions are not the only source of multiple inconsistent
// definitions.  Consider a non-'inline' method 'reserveCapacity' on a 'List'
// template, parameterized by element 'TYPE':
//..
//  // our_list.h
//  // ...
//
//  template <class TYPE>
//  void List<TYPE>::reserveCapacity(int numElements)
//  {
//      BSLS_ASSERT(numElements >= 0);
//      // ...
//  }
//..
// Each different translation unit that invokes 'reserveCapacity' potentially
// generates another instantiation of this function template.  Those
// translation units that are compiled in "debug mode" (or "safe mode") --
// e.g., with 'BSLS_ASSERT_LEVEL_ASSERT' (or 'BSLS_ASSERT_LEVEL_ASSERT_SAFE')
// defined -- will incorporate code corresponding to each use of the
// 'BSLS_ASSERT' macro therein; the rest will not.  Which one of these template
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
// Note that all build modes (except for when 'BDE_BUILD_TARGET_SAFE_2' is
// defined, see below) are required to be binary compatible (e.g., fields
// cannot be added to the middle of a 'struct').  Since a component's contract
// makes no explicit promise about what checking will occur, that contract is
// not violated when different parts of a program are compiled with different
// levels of assertion-enabling build options.  The only consequence is that a
// smaller (or larger) number of defensive checks may be active than might
// otherwise be expected.
//
///Conditional Compilation: 'BDE_BUILD_TARGET_SAFE_2' and Assertion Predicates
///---------------------------------------------------------------------------
// To recap, there are three (mutually compatible) general *build* *targets*
//: o 'BDE_BUILD_TARGET_OPT'
//: o 'BDE_BUILD_TARGET_SAFE'
//: o 'BDE_BUILD_TARGET_SAFE_2'
// and four (mutually exclusive) component-specific *assertion* *levels*
//: o 'BSLS_ASSERT_LEVEL_ASSERT_SAFE'
//: o 'BSLS_ASSERT_LEVEL_ASSERT'
//: o 'BSLS_ASSERT_LEVEL_ASSERT_OPT'
//: o 'BSLS_ASSERT_LEVEL_NONE'
// that can be defined (externally) by the build environment to affect which of
// the three *assert* *macros*
//: 1 'BSLS_ASSERT_SAFE(boolean-valued expression)'
//: 2 'BSLS_ASSERT(boolean-valued expression)'
//: 3 'BSLS_ASSERT_OPT(boolean-valued expression)'
// will be active (i.e., instantiate).
//
// The public interface of this component also explicitly supports three
// additional, intermediate input macros, called *assertion* *predicates*
//: 1 'BSLS_ASSERT_SAFE_IS_ACTIVE'
//: 2 'BSLS_ASSERT_IS_ACTIVE'
//: 3 'BSLS_ASSERT_OPT_IS_ACTIVE'
// that are derived from the various combinations of the external inputs, and
// indicate whether each respective kind of (BSLS) assertion macro is active.
// These additional "predicate" macros, along with 'BDE_BUILD_TARGET_SAFE_2',
// can be used directly by clients of this component to conditionally compile
// source code other than just (BSLS) assertions.
//
// For example, additional source code that would affect binary compatibility
// must be conditionally compiled using 'BDE_BUILD_TARGET_SAFE_2':
//..
//  #ifdef BDE_BUILD_TARGET_SAFE_2
//
//  // additional (redundant) defensive code that breaks binary compatibility
//
//  #endif
//..
// However, there may be times when we want to provide code that is binary
// compatible (and has no other undesirable side-effects) whenever, say,
// 'BSLS_ASSERT' is active:
//..
//  #ifdef BSLS_ASSERT_IS_ACTIVE
//
//  // additional (redundant) defensive code that is binary compatible and
//  // should exist whenever 'BSLS_ASSERT' is active, but not otherwise
//
//  #endif
//..
// Note that 'BSLS_ASSERT_IS_ACTIVE' implies 'BSLS_ASSERT_OPT_IS_ACTIVE', but
// not 'BSLS_ASSERT_SAFE_IS_ACTIVE'.  Also note that (redundant) defensive code
// that is intended to be active in *all* build modes -- like 'BSLS_ASSERT_OPT'
// -- should nonetheless be conditionally compiled (to enable verification that
// the runtime overhead is truly negligible):
//..
//  #ifdef BSLS_ASSERT_OPT_IS_ACTIVE
//
//  // additional (redundant) defensive code that is binary compatible and
//  // should exist even in an optimized build
//
//  #endif
//..
// See {Example 6} and {Example 7}, respectively, for how
// 'BDE_BUILD_TARGET_SAFE_2' and intermediate assertion predicate macros, such
// as 'BSLS_ASSERT_SAFE_IS_ACTIVE' (and even 'BSLS_ASSERT_OPT_IS_ACTIVE'), can
// be used profitably in practice.
//
///Usage
///-----
// The following examples illustrate (1) when to use each of the three kinds of
// (BSLS) "ASSERT" macros, (2) when and how to call the 'invokeHandler' method
// directly, (3) how to configure, at runtime, the behavior resulting from an
// assertion failure using "off-the-shelf" handler methods, (4) how to create
// your own custom assertion-failure handler function, (5) proper use of
// 'bsls::AssertFailureHandlerGuard' to install, temporarily, an
// exception-producing assert handler, (6) how "ASSERT" macros would be used in
// conjunction with portions of the source code (affecting binary
// compatibility) that are incorporated only when 'BDE_BUILD_TARGET_SAFE_2' is
// defined, and (7) how assertion predicates (e.g.,
// 'BSLS_ASSERT_SAFE_IS_ACTIVE') are used to conditionally compile additional
// (redundant) defensive source code (not affecting binary compatibility)
// precisely when the corresponding (BSLS) "ASSERT" macro (e.g.,
// 'BSLS_ASSERT_SAFE') is active.
//
///Example 1: Using 'BSLS_ASSERT', 'BSLS_ASSERT_SAFE', and 'BSLS_ASSERT_OPT'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component provides three different variants of (BSLS) "ASSERT" macros.
// This first usage example illustrates how one might select each of the
// particular variants, based on the runtime cost of the defensive check
// relative to that of the useful work being done.
//
// Use of the 'BSLS_ASSERT_SAFE' macro is often appropriate when the defensive
// check occurs within the body of an 'inline' function.  The
// 'BSLS_ASSERT_SAFE' macro minimizes the impact on runtime performance as it
// is instantiated only when requested (i.e., by building in "safe mode").  For
// example, consider a light-weight point class 'Kpoint' that maintains 'x' and
// 'y' coordinates in the range '[ -1000 .. 1000 ]':
//..
//  // my_kpoint.h
//  // ...
//
//  class Kpoint {
//      short int d_x;
//      short int d_y;
//    public:
//      Kpoint(short int x, short int y);
//          // ...
//          // The behavior is undefined unless '-1000 <= x <= 1000'
//          // and '-1000 <= y <= 1000'.
//      // ...
//  };
//
//  // ...
//..
// Since the cost of validation here is significant compared with the useful
// work being done, we might choose to implement defensive checks using
// 'BSLS_ASSERT_SAFE' as follows:
//..
//  // ...
//
//  inline
//  Kpoint::Kpoint(short int x, short int y)
//  : d_x(x)
//  , d_y(y)
//  {
//      BSLS_ASSERT_SAFE(-1000 <= x); BSLS_ASSERT_SAFE(x <= 1000);
//      BSLS_ASSERT_SAFE(-1000 <= y); BSLS_ASSERT_SAFE(y <= 1000);
//  }
//..
// For more substantial (non-'inline') functions, we would be more likely to
// use the 'BSLS_ASSERT' macro because the runtime overhead due to defensive
// checks is likely to be much less significant.  For example, consider a
// hash-table class that allows the client to resize the underlying table:
//..
//  // my_hashtable.h
//  // ...
//
//  class HashTable {
//      // ...
//    public:
//      // ...
//
//      void resize(double loadFactor);
//          // Adjust the size of the underlying hash table to be
//          // approximately the current number of elements divided
//          // by the specified 'loadFactor'.  The behavior is undefined
//          // unless '0 < loadFactor'.
//  };
//..
// Since the relative runtime cost of validating the input argument is quite
// small (e.g., less than 10%) compared to the typical work being done, we
// might choose to implement the defensive check using 'BSLS_ASSERT' as
// follows:
//..
//  // my_hashtable.cpp
//  // ...
//
//  void HashTable::resize(double loadFactor)
//  {
//      BSLS_ASSERT(0 < loadFactor);
//
//      // ...
//  }
//..
// In some cases, the runtime cost of checking is always negligible when
// compared with the runtime cost of performing the useful work; moreover, the
// consequences of continuing in an undefined state for certain applications
// could be catastrophic.  Instead of using 'BSLS_ASSERT' in such cases, we
// might consider using 'BSLS_ASSERT_OPT'.  For example, suppose we have a
// financial application class 'TradingSystem' that performs trades:
//..
//  // my_tradingsystem.h
//  // ...
//
//  class TradingSystem {
//      // ...
//    public:
//      // ...
//..
// Further suppose that there is a particular method 'executeTrade' that takes,
// as a scaling factor, an integer that must be a multiple of 100 or the
// behavior is undefined (and might actually execute a trade):
//..
//
//      void executeTrade(int scalingFactor);
//          // Execute the current trade using the specified 'scalingFactor'.
//          // The behavior is undefined unless '0 <= scalingFactor' and
//          // '100' evenly divides 'scalingFactor'.
//      // ...
//  };
//..
// Because the cost of the two checks is likely not even measurable compared to
// the overhead of accessing databases and executing the trade, and because the
// consequences of specifying a bad scaling factor are virtually unbounded, we
// might choose to implement these defensive checks using 'BSLS_ASSERT_OPT' as
// follows:
//..
//  // my_tradingsystem.cpp
//  // ...
//
//  void TradingSystem::executeTrade(int scalingFactor)
//  {
//      BSLS_ASSERT_OPT(0 <= scalingFactor);
//      BSLS_ASSERT_OPT(0 == scalingFactor % 100);
//
//      // ...
//  }
//..
// Notice that in each case, the choice of which of the three (BSLS) "ASSERT"
// macros to use is governed primarily by the relative runtime cost compared
// with that of the useful work being done (and only secondarily by the
// potential consequences of continuing execution in an undefined state).
//
///Example 2: When and How to Call the 'invokeHandler' Method Directly
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There *may* be times (but this is yet to be demonstrated) when we might
// reasonably choose to invoke the currently installed assertion-failure
// handler directly -- i.e., instead of via one of the three (BSLS) "ASSERT"
// macros provided in this component.  Suppose that we are currently in the
// body of some function 'someFunc' and, for whatever reason, feel compelled to
// invoke the currently installed assertion-failure handler based on some
// criteria other than the current build mode.  The call might look as follows:
//..
//  void someFunc(bool a, bool b, bool c)
//  {
//      bool someCondition = a && b && !c;
//
//      if (someCondition) {
//          bsls::Assert::invokeHandler("Bad News", __FILE__, __LINE__);
//      }
//  }
//..
// If presented with invalid arguments, 'someFunc' (above) will produce output
// similar to the following:
//..
//  Assertion failed: Bad News, file bsls_assert.t.cpp, line 365
//  Abort (core dumped)
//..
//
///Example 3: Runtime Configuration of the 'bsls::Assert' Facility
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// By default, any assertion failure will result in the invocation of the
// 'bsls::Assert::failAbort' handler function.  We can replace this behavior
// with that of one of the other static failure handler methods supplied in
// 'bsls::Assert' as follows.  Let's assume we are at the top of our
// application called 'myMain' (which would typically be 'main'):
//..
//  void myMain()
//  {
//..
// First observe that the default assertion-failure handler function is, in
// fact, 'bsls::Assert::failAbort':
//..
//  assert(&bsls::Assert::failAbort == bsls::Assert::failureHandler());
//..
// Next, we install a new assertion-failure handler function,
// 'bsls::Assert::failSleep', from the suite of "off-the-shelf" handlers
// provided as 'static' methods of 'bsls::Assert':
//..
//  bsls::Assert::setFailureHandler(&bsls::Assert::failSleep);
//..
// Observe that 'bsls::Assert::failSleep' is the new, currently-installed
// assertion-failure handler:
//..
//  assert(&bsls::Assert::failSleep == bsls::Assert::failureHandler());
//..
// Note that if we were to explicitly invoke the current assertion-failure
// handler as follows:
//..
//  bsls::Assert::invokeHandler("message", "file", 27);  // This will hang!
//..
// the program will hang since 'bsls::Assert::failSleep' repeatedly sleeps for
// a period of time within an infinite loop.  Thus, this assertion-failure
// handler is useful for hanging a process so that a debugger may be attached
// to it.
//
// We may now decide to disable the 'setFailureHandler' method using the
// 'bsls::Assert::lockAssertAdministration()' method to ensure that no one else
// will override our decision globally.  Note, however, that the
// 'bsls::AssertFailureHandlerGuard' is not affected, and can still be used to
// supplant the currently installed handler (see below):
//..
//  bsls::Assert::lockAssertAdministration();
//..
// Attempting to change the currently installed handler now will fail:
//..
//      bsls::Assert::setFailureHandler(&bsls::Assert::failAbort);
//
//      assert(&bsls::Assert::failAbort != bsls::Assert::failureHandler());
//
//      assert(&bsls::Assert::failSleep == bsls::Assert::failureHandler());
//  }
//..
//
///Example 4: Creating a Custom Assertion Handler
/// - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes, especially during testing, we may need to write our own custom
// assertion-failure handler function.  The only requirements are that the
// function have the same prototype (i.e., the same respective parameter and
// return types) as the 'bsls::Assert::Handle' 'typedef', and that the function
// should not return (i.e., it must 'abort', 'exit', 'terminate', 'throw', or
// hang).  To illustrate, we will create a 'static' method at file scope that
// conforms to the required structure (notice the explicit use of 'std::printf'
// from '<cstdio>' instead of 'std::cout' from '<iostream>' to avoid
// interaction with the C++ memory allocation layer):
//..
//  static bool globalEnableOurPrintingFlag = true;
//
//  static
//  void ourFailureHandler(const char *text, const char *file, int line)
//      // Print the specified expression 'text', 'file' name, and 'line'
//      // number to 'stdout' as a comma-separated list, replacing null
//      // string-argument values with empty strings (unless printing has been
//      // disabled by the 'globalEnableOurPrintingFlag' variable), then
//      // unconditionally abort.
//  {
//      if (!text) {
//          text = "";
//      }
//      if (!file) {
//          file = "";
//      }
//      if (globalEnableOurPrintingFlag) {
//          std::printf("%s, %s, %d\n", text, file, line);
//      }
//      std::abort();
//  }
//..
// At the top level of our application we have the following:
//..
//  void ourMain()
//  {
//..
// First, let's observe that we can assign this new function to a function
// pointer of type 'bsls::Assert::Handler':
//..
//  bsls::Assert::Handler f = &::ourFailureHandler;
//..
// Now we can install it just as we would any other handler:
//..
//  bsls::Assert::setFailureHandler(&::ourFailureHandler);
//..
// We can now invoke the default handler directly:
//..
//      bsls::Assert::invokeHandler("str1", "str2", 3);
//  }
//..
// With the resulting output as follows:
//..
//  str1, str2, 3
//  Abort (core dumped)
//..
//
///Example 5: Using the 'bsls::AssertFailureHandlerGuard'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Sometimes we may want to replace, temporarily (i.e., within some local
// lexical scope), the currently installed assertion-failure handler function.
// In particular, we sometimes use the 'bsls::AssertFailureHandlerGuard' class
// to replace the current handler with one that throws an exception (because we
// know that such an exception is safe in the local context).  Let's start with
// the simple factorial function below, which validates, in "debug mode" (or
// "safe mode"), that its input is non-negative:
//..
//  double fact(int n)
//      // Return 'n!'.  The behavior is undefined unless '0 <= n'.
//  {
//      BSLS_ASSERT(0 <= n);
//
//      double result = 1.0;
//      while (n > 1) {
//          result *= n--;
//      }
//      return result;
//  }
//..
// Now consider the following integer-valued 'extern "C"' C++ function,
// 'wrapperFunc', which can be called from C and FORTRAN, as well as from C++:
//..
//  extern "C" int wrapperFunc(bool verboseFlag)
//  {
//      enum { GOOD = 0, BAD } result = GOOD;
//..
// The purpose of this function is to allow assertion failures in subroutine
// calls below this function to be handled by throwing an exception, which is
// then caught by the wrapper and reported to the caller as a "bad" status.
// Hence, when within the runtime scope of this function, we want to install,
// temporarily, the assertion-failure handler 'bsls::Assert::failThrow', which,
// when invoked, causes an 'bsls::AssertTestException' object to be thrown.
// (Note that we are not advocating this approach for "recovery", but rather
// for an orderly shut-down, or perhaps during testing.)  The
// 'bsls::AssertFailureHandlerGuard' class is provided for just this purpose:
//..
//      assert(&bsls::Assert::failAbort == bsls::Assert::failureHandler());
//
//      bsls::AssertFailureHandlerGuard guard(&bsls::Assert::failThrow);
//
//      assert(&bsls::Assert::failThrow == bsls::Assert::failureHandler());
//..
// Next we open up a 'try' block, and somewhere within the 'try' we
// "accidentally" invoke 'fact' with an out-of-contract value (i.e., '-1'):
//..
//  #ifdef BDE_BUILD_TARGET_EXC
//      try
//  #endif
//          {
//
//          // ...
//
//          double d = fact(-1);        // Out-of-contract call to 'fact'.
//
//          // ...
//      }
//  #ifdef BDE_BUILD_TARGET_EXC
//      catch (const bsls::AssertTestException& e) {
//          result = BAD;
//          if (verboseFlag) {
//              std::cout << "Internal Error: " << e.what() << std::endl;
//          }
//      }
//  #endif
//      return result;
//  }
//..
// Assuming exceptions are enabled (i.e., 'BDE_BUILD_TARGET_EXC' is defined),
// if an 'bsls::AssertTestException' occurs below this wrapper function, the
// exception will be caught, a message will be printed to 'stdout', e.g.,
//..
//  Internal Error: bsls_assert.t.cpp:500: 0 <= n
//..
// and the 'wrapperFunc' function will return a bad status (i.e., 1) to its
// caller.  Note that if exceptions are not enabled, 'bsls::Assert::failThrow'
// will behave as 'bsls::Assert::failAbort', and dump core immediately:
//..
// Assertion failed: 0 <= n, file bsls_assert.t.cpp, line 500
// Abort (core dumped)
//..
// Finally note that the 'bsls::AssertFailureHandlerGuard' is not thread-aware.
// In particular, a guard that is created in one thread will also affect the
// failure handlers that are used in other threads.  Care should be taken when
// using this guard when more than a single thread is executing.
//
///Example 6: Using (BSLS) "ASSERT" Macros Along With 'BDE_BUILD_TARGET_SAFE_2'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Recall that assertions do not affect binary compatibility; however, software
// built with 'BDE_BUILD_TARGET_SAFE_2' defined need not be binary compatible
// with software built otherwise.  In this example, we look at how we might use
// the (BSLS) "ASSERT" family of macros in conjunction with code that is
// incorporated (at compile time) only when 'BDE_BUILD_TARGET_SAFE_2' is
// defined.
//
// As a simple example, let's consider an elided implementation of a
// singly-linked integer list and its iterator.  Whenever
// 'BDE_BUILD_TARGET_SAFE_2' is defined, we want to defend against the
// possibility that a client mistakenly passes a 'ListIter' object into a
// 'List' object method (e.g., 'List::insert') where that 'ListIter' object did
// not originate from the same 'List' object.
//
// We'll start by defining a local helper 'List_Link' 'struct' as follows:
//..
//  struct List_Link {
//      List_Link *d_next_p;
//      int        d_data;
//      List_Link(List_Link *next, int data) : d_next_p(next), d_data(data) { }
//  };
//..
// Next, we'll define 'ListIter', which always identifies the current position
// in a sequence of links, but whenever 'BDE_BUILD_TARGET_SAFE_2' is defined,
// also maintains a pointer to its parent 'List' object:
//..
//  class List;                         // Forward declaration.
//
//  class ListIter {
//  #ifdef BDE_BUILD_TARGET_SAFE_2
//      List *d_parent_p;               // Exists only in "safe 2 mode".
//  #endif
//      List_Link **d_current_p;
//      friend class List;
//      friend bool operator==(const ListIter&, const ListIter&);
//    private:
//      ListIter(List_Link **current,
//               List *
//  #ifdef BDE_BUILD_TARGET_SAFE_2
//                     parent           // Not used unless in "safe 2 mode".
//  #endif
//              )
//      : d_current_p(current)
//  #ifdef BDE_BUILD_TARGET_SAFE_2
//      , d_parent_p(parent)            // Initialize only in "safe 2 mode".
//  #endif
//      { }
//    public:
//      ListIter& operator++() { /* ... */ return *this; }
//      // ...
//  };
//  bool operator==(const ListIter& lhs, const ListIter& rhs);
//  bool operator!=(const ListIter& lhs, const ListIter& rhs);
//..
// Finally we define the 'List' class itself with most of the operations
// elided; the methods of particular interest here are 'begin' and 'insert':
//..
//
//  class List {
//      List_Link *d_head_p;
//    public:
//      // CREATORS
//      List() : d_head_p(0) { }
//      List(const List& original) { /* ... */ }
//      ~List() { /* ... */ }
//
//      // MANIPULATORS
//      List& operator=(const List& rhs) { /* ... */ return *this; }
//
//      //| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//      //v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
//      //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//      ListIter begin()
//          // Return an iterator referring to the beginning of this list.
//      {
//          return ListIter(&d_head_p, this);
//      }
//      //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//      //| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//      //v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
//      //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//      void insert(const ListIter& position, int data)
//          // Insert the specified 'data' value into this list at the
//          // specified 'position'.
//      {
//  #ifdef BDE_BUILD_TARGET_SAFE_2
//          BSLS_ASSERT_SAFE(this == position.d_parent_p);  // "safe 2 mode"
//  #endif
//          *position.d_current_p = new List_Link(*position.d_current_p, data);
//      }
//      //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//
//      // ACCESSORS
//      void print(std::ostream& stream) const
//      {
//          stream << '[';
//          for (List_Link *p = d_head_p; p; p = p->d_next_p) {
//              stream << ' ' << p->d_data;
//          }
//          stream << " ]" << std::endl;
//      }
//  };
//..
// Outside of "safe 2 mode", it is possible to pass an iterator object obtained
// from the 'begin' method of one 'List' object into the 'insert' method of
// another, having, perhaps, unexpected results:
//..
//  void sillyFunc(bool printFlag)
//  {
//      List a;
//      ListIter aIt = a.begin();
//      a.insert(aIt, 1);
//      a.insert(aIt, 2);
//      a.insert(aIt, 3);
//
//      if (printFlag) {
//          std::cout << "a = "; a.print(std::cout);
//      }
//
//      List b;
//      ListIter bIt = b.begin();
//      a.insert(bIt, 4);       // Oops!  Should have been: 'b.insert(bIt, 4);'
//      a.insert(bIt, 5);       // Oops!    "     "     "   '    "     "   5  '
//      a.insert(bIt, 6);       // Oops!    "     "     "   '    "     "   6  '
//
//      if (printFlag) {
//          std::cout << "a = "; a.print(std::cout);
//          std::cout << "b = "; b.print(std::cout);
//      }
//  }
//..
// In the example above, we have "accidentally" passed the iterator 'bIt'
// obtained from 'List' object 'b' into the 'insert' method for 'List' object
// 'a'.  The resulting undefined behavior (in other than "safe 2 mode") might
// produce output that looks as follows:
//..
//  a = [ 3 2 1 ]
//  a = [ 3 2 1 ]
//  b = [ 6 5 4 ]
//..
// If the same 'sillyFunc' were compiled in "safe 2 mode" (i.e., with
// 'BDE_BUILD_TARGET_SAFE_2' defined) the undefined behavior would be detected
// and the output would, by default, look more like the following:
//..
//  a = [ 3 2 1 ]
//  Assertion failed: this == position.d_parent_p, file my_list.cpp, line 56
//  Abort (core dumped)
//..
// thereby quickly exposing the misuse by the client.
//
///Example 7: Conditional Compilation Associated with Enabled Assertion Levels
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In cases where we want to tie code, other than just an assertion, to a
// specific level of enabled assertions, we will want to use the corresponding
// intermediate predicate that enables that level of assertions:
//
//: o For 'BSLS_ASSERT_SAFE', use 'BSLS_ASSERT_SAFE_IS_ACTIVE'.
//:
//: o For 'BSLS_ASSERT', use 'BSLS_ASSERT_IS_ACTIVE'.
//:
//: o For 'BSLS_ASSERT_OPT', use 'BSLS_ASSERT_OPT_IS_ACTIVE'.
//
// Suppose that we have a class such as 'MyDate' (below) that, except for
// checking its invariants, would have a trivial destructor.  By not declaring
// a destructor at all, we may realize performance advantages, but then we lose
// the ability to validate our invariants in "debug" or "safe" mode.  What we
// want to do is to declare (and later define) the destructor in precisely
// those build modes for which we would want to assert invariants.
//
// An elided class 'MyDate', which is based on a serial-date implementation, is
// provided for reference:
//..
//  class MyDate {
//      // This class implements a value-semantic "date" type representing
//      // valid date values in the range '[ 0001Jan01 .. 9999Dec31 ]'.
//
//      // DATA
//      int d_serialDate;  // sequential representation within a valid range
//
//    public:
//       // CLASS METHODS
//
//       // ...
//
//       // CREATORS
//       MyDate();
//           // Create a 'MyDate' object having the value '0001Jan01'.
//
//       // ...
//
//       MyDate(const MyDate& original);
//           // Create a 'MyDate' object having the same value as the
//           // specified 'original' object.
//
//  #if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
//       ~MyDate();
//           // Destroy this object.  Note that in some build modes the
//           // destructor generated by the compiler is trivial.
//  #endif
//
//      // ...
//  };
//
//  // ...
//
//  // =======================================================================
//  //                      INLINE FUNCTION DEFINITIONS
//  // =======================================================================
//
//  // ...
//
//  // CREATORS
//  inline
//  MyDate::MyDate()
//  : d_serialDate(1)  // 0001Jan01
//  {
//  }
//
//  inline
//  MyDate::MyDate(const MyDate& original)
//  : d_serialDate(original.d_serialDate)
//  {
//  }
//
//  // ...
//
//  #if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
//  inline
//  MyDate::~MyDate()
//  {
//      BSLS_ASSERT_SAFE(1 <= d_serialDate);             // 0001Jan01
//      BSLS_ASSERT_SAFE(     d_serialDate <= 3652061);  // 9999Dec31
//  }
//  #endif
//
//  // ...
//..
// In practice, however, we would probably implement an 'isValidSerialDate'
// method in a lower-level utility class, e.g., 'MyDateImpUtil', leading to
// code that is more fine-grained, modular, and hierarchically reusable:
//..
//  inline
//  bool MyDateImpUtil::isValidSerialDate(int d_date)
//  {
//      return 1 <= d_serialDate && d_serialDate <= 3652061;
//  }
//..
// Like other aspects of 'BSLS_ASSERT_SAFE', the example above violates the
// one-definition rule for mixed-mode builds.  Note that all code conditionally
// compiled based on 'BSLS_ASSERT_SAFE_IS_ACTIVE', 'BSLS_ASSERT_IS_ACTIVE', and
// 'BSLS_ASSERT_OPT_IS_ACTIVE' should be binary compatible for mixed-mode
// builds.  If the conditionally-compiled code would not be binary compatible,
// use 'BDE_BUILD_TARGET_SAFE_2' instead.
//
// WARNING - In practice, declaring a destructor in some build modes but not
// others has led to subtle and difficult-to-diagnose failures.  DON'T DO IT!
//
// Finally, in very rare cases, we may want to put in (redundant) defensive
// code (in the spirit of 'BSLS_ASSERT_OPT') that is not part of the
// component-level contract, yet (1) is known to have negligible runtime cost
// and (2) is deemed to be so important as to be necessary even for optimized
// builds.
//
// For example, consider again the 'MyDate' class above that now also declares
// a non-'inline' 'print' method to format the current date value in some
// human-readable, but otherwise unspecified format:
//..
//  // xyza_mydate.h
//  // ...
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
//      std::ostream& print(std::ostream& stream, ...) const;
//          // Write the value of this object to the specified output 'stream'
//          // in some human-readable format, and return a reference to
//          // 'stream'.  Optionally specify ...
//
//      // ...
//
//  };
//..
// Successfully writing bad data is among the most insidious of bugs, because a
// latent error can persist and not be discovered until long after the program
// terminates.  Writing the value of a corrupted 'MyDate' object in a
// *machine-readable* (binary) format is an error so serious as to warrant
// invoking
//..
//  BSLS_ASSERT_OPT(MyDateImpUtil::isValidSerialDate(d_serialDate));
//..
// each time we attempt the output operation; however, printing the value in a
// human-readable format intended primarily for debugging purposes is another
// matter.  In anything other than a safe build (which in this case would
// enforce essentially all method preconditions), it would be unfortunate if a
// developer, knowing that there was a problem involving the use of 'MyDate',
// inserted print statements to identify that problem, only to have the 'print'
// method itself ruthlessly invoke the assert handler, likely terminating the
// process).  Moreover, it may also be unsafe even to attempt to format the
// value of a 'MyDate' object whose 'd_serialDate' value violates its
// invariants (e.g., due to a static table lookup).  In such cases we may, as
// sympathetic library developers, choose to implement different undefined
// (undocumented) redundant defensive behaviors, depending on the desired level
// of assertions:
//..
//  // xyza_mydate.cpp
//  // ...
//  #include <xyza_mydateimputil.h>
//  // ...
//
//  std::ostream& MyDate::print(std::ostream& stream, ...) const
//  {
//      BSLS_ASSERT(/* any *argument* preconditions for this function */);
//
//      // Handle case where the invariants have been violated.
//
//  #ifdef BSLS_ASSERT_OPT_IS_ACTIVE
//      // Note that if 'BSLS_ASSERT_LEVEL_NONE' has been set, this
//      // code -- along with all 'BSLS_ASSERT_OPT' macros -- will not
//      // instantiate, enabling us to verify that the combined runtime
//      // overhead of all such (redundant) defensive code is at worst
//      // negligible, if not unmeasurable.
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
//  #endif // BSLS_ASSERT_OPT_IS_ACTIVE
//
//      }
//
//      // If we get here in a production build, this object is "sane":
//      // Do whatever this 'print' method would normally do, assuming
//      // that no method preconditions or object invariants are violated.
//
//      // ...  <*** Your (Normal-Case) Formatting Code Here! ***>
//  }
//
//..

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
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

#if !(defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) ||                              \
      defined(BSLS_ASSERT_LEVEL_ASSERT) ||                                   \
      defined(BSLS_ASSERT_LEVEL_ASSERT_OPT) ||                               \
      defined(BSLS_ASSERT_LEVEL_NONE))
    #define BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED 1
#else
    #define BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED 0
#endif

#define BSLS_ASSERT_ASSERT(X) do {                                           \
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!(X))) {                   \
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;                              \
            BloombergLP::bsls::Assert::invokeHandler(#X, __FILE__, __LINE__);\
        }                                                                    \
    } while (false)

                        // ================
                        // BSLS_ASSERT_SAFE
                        // ================

// Determine if 'BSLS_ASSERT_SAFE' should be active.

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE)                                   \
    || BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED && (                          \
           defined(BDE_BUILD_TARGET_SAFE_2) ||                               \
           defined(BDE_BUILD_TARGET_SAFE)         )

    #define BSLS_ASSERT_SAFE_IS_ACTIVE  // also usable directly in client code
#endif

// Define 'BSLS_ASSERT_SAFE' accordingly.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    #define BSLS_ASSERT_SAFE(X) BSLS_ASSERT_ASSERT(X)
#else
    #define BSLS_ASSERT_SAFE(X)
#endif

                        // ===========
                        // BSLS_ASSERT
                        // ===========

// Determine if 'BSLS_ASSERT' should be active.

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) ||                                \
    defined(BSLS_ASSERT_LEVEL_ASSERT)                                        \
    || BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED && (                          \
           defined(BDE_BUILD_TARGET_SAFE_2) ||                               \
           defined(BDE_BUILD_TARGET_SAFE)   ||                               \
           !defined(BDE_BUILD_TARGET_OPT)         )

    #define BSLS_ASSERT_IS_ACTIVE       // also usable directly in client code
#endif

// Define 'BSLS_ASSERT' accordingly.

#if defined(BSLS_ASSERT_IS_ACTIVE)
    #define BSLS_ASSERT(X) BSLS_ASSERT_ASSERT(X)
#else
    #define BSLS_ASSERT(X)
#endif

                        // ===============
                        // BSLS_ASSERT_OPT
                        // ===============

// Determine if 'BSLS_ASSERT_OPT' should be active.

#if !defined(BSLS_ASSERT_LEVEL_NONE)
    #define BSLS_ASSERT_OPT_IS_ACTIVE   // also usable directly in client code
#endif

// Define 'BSLS_ASSERT_OPT' accordingly.

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    #define BSLS_ASSERT_OPT(X) BSLS_ASSERT_ASSERT(X)
#else
    #define BSLS_ASSERT_OPT(X)
#endif

#ifdef BSLS_ASSERT_NORETURN
#error BSLS_ASSERT_NORETURN must be a macro scoped locally to this header file
#endif

// Note that a portable syntax for 'noreturn' will be available once we have
// access to conforming C++0x compilers.
//# define BSLS_ASSERT_NORETURN [[noreturn]]

#if defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLS_ASSERT_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERT_NORETURN
#endif

// A nested include guard is needed to support the test driver implementation.
#ifndef RECUSRSIVELY_INCLUDED_BSLS_ASSERT_TESTDRIVER_GUARD
#define RECUSRSIVELY_INCLUDED_BSLS_ASSERT_TESTDRIVER_GUARD

#ifdef BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER
#define BSLS_ASSERT_NORETURN_INVOKE_HANDLER  BSLS_ASSERT_NORETURN
#else
#define BSLS_ASSERT_NORETURN_INVOKE_HANDLER
#endif

namespace BloombergLP {

// FORWARD DECLARATIONS

namespace bsls {

                        // ============
                        // class Assert
                        // ============

class Assert {
    // This "utility" class maintains a pointer containing the address of the
    // current assertion-failure handler function (of type 'Assert::Handler')
    // and provides methods to administer this function pointer.  The
    // 'invokeHandler' method calls the currently-installed failure handler.
    // This class also provides a suite of standard failure-handler functions
    // that are suitable to be installed as the current 'Assert::Handler'
    // function.  Note that clients are free to install any of these
    // ("off-the-shelf") handlers, or to provide their own ("custom")
    // assertion-failure handler functions when using this facility.  Also note
    // that assertion-failure handler functions must not return (i.e., they
    // must 'abort', 'exit', 'terminate', 'throw', or hang).

  public:
    // TYPES
    typedef void (*Handler)(const char *, const char *, int);
        // 'Handler' is an alias for a pointer to a function returning 'void',
        // and taking, as parameters, two null-terminated strings and an 'int',
        // which is the structure of all assertion-failure handler functions
        // supported by this class -- e.g.,
        //..
        //  void myHandler(const char *text, const char *file, int line);
        //..

  private:
    // FRIENDS
    friend class AssertFailureHandlerGuard;

    // CLASS DATA
    static bsls::AtomicOperations::AtomicTypes::Pointer
                        s_handler;     // assertion-failure handler function
    static bsls::AtomicOperations::AtomicTypes::Int
                        s_lockedFlag;  // lock to disable 'setFailureHandler'

    // PRIVATE CLASS METHODS
    static void setFailureHandlerRaw(Assert::Handler function);
        // Make the specified handler 'function' the current assertion-failure
        // handler.

  public:
    // CLASS METHODS

                      // Administrative Methods

    static void setFailureHandler(Assert::Handler function);
        // Make the specified handler 'function' the current assertion-failure
        // handler.  This method has no effect if the
        // 'lockAssertAdministration' method has been called.

    static void lockAssertAdministration();
        // Disable all subsequent calls to 'setFailureHandler'.  Note that this
        // method has no effect on the behavior of a
        // 'AssertFailureHandlerGuard' object.

    static Assert::Handler failureHandler();
        // Return the address of the currently installed assertion-failure
        // handler function.

                      // Dispatcher Method (called from within macros)

    BSLS_ASSERT_NORETURN_INVOKE_HANDLER
    static void invokeHandler(const char *text, const char *file, int line);
        // Invoke the currently installed assertion-failure handler function
        // with the specified expression 'text', 'file' name, and 'line' number
        // as its arguments.  The behavior is undefined unless the macro
        // 'BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER' is not defined, or
        // the currently installed assertion-failure handler function does not
        // return (i.e., it must 'abort', 'exit', 'terminate', 'throw', or
        // hang).  Note that this function is intended for use by the (BSLS)
        // "ASSERT" macros, but may also be called by clients directly as
        // needed.  Also note that the configuration macro
        // 'BSLS_ASSERT_ENABLE_NORETURN_FOR_INVOKE_HANDLER' is intended to
        // support static analysis tools, which require an annotation to see
        // that a failed "ASSERT" prevents further execution of a function with
        // "bad" values.

                      // Standard Assertion-Failure Handlers

    BSLS_ASSERT_NORETURN
    static void failAbort(const char *text, const char *file, int line);
        // (Default Handler) Emulate the invocation of the standard 'assert'
        // macro with a 'false' argument, using the specified expression
        // 'text', 'file' name, and 'line' number to generate a helpful output
        // message and then, after printing this message to 'stderr',
        // unconditionally aborting.  Note that this handler function is
        // initially the currently installed one by default.

    BSLS_ASSERT_NORETURN
    static void failSleep(const char *text, const char *file, int line);
        // Use the specified expression 'text', 'file' name, and 'line' number
        // to generate a helpful output message and then, after printing this
        // message to 'stderr', sleep within an infinite loop.  Note that this
        // handler function is useful for hanging a process so that a debugger
        // may be attached to it.

    BSLS_ASSERT_NORETURN
    static void failThrow(const char *text, const char *file, int line);
        // Throw a 'AssertTestException' (whose attributes are 'expression',
        // 'filename', and 'lineNumber'), provided that 'BDE_BUILD_TARGET_EXC'
        // is defined; otherwise, write an appropriate message to 'stderr' and
        // abort the program (similar to 'failAbort').
};

                    // ===============================
                    // class AssertFailureHandlerGuard
                    // ===============================

class AssertFailureHandlerGuard {
    // An object of this class saves the current assert handler and installs
    // the one specified on construction.  On destruction, the original assert
    // handler is restored.  Note that two objects of this class cannot be
    // safely used concurrently from two separate threads (but may of course
    // appear sequentially, including in nested blocks and function invocations
    // within a single thread).  Note that the behavior of objects of this
    // class is unaffected by the ('static') 'Assert::lockAssertAdministration'
    // method (i.e., the temporary replacement will occur, regardless of
    // whether that method has been invoked.)

    // DATA
    Assert::Handler d_original;  // original (restored at destruction)

  private:
    // NOT IMPLEMENTED
    AssertFailureHandlerGuard(const AssertFailureHandlerGuard&);
    AssertFailureHandlerGuard& operator=(const AssertFailureHandlerGuard&);

  public:
    // CREATORS
    explicit AssertFailureHandlerGuard(Assert::Handler temporary);
        // Create a guard object that installs the specified 'temporary'
        // failure handler and automatically restores the original handler on
        // destruction.

    ~AssertFailureHandlerGuard();
        // Restore the failure handler that was in place when this object was
        // created and destroy this guard.
};

}  // close package namespace


// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close enterprise namespace

#endif // deeper include guard

// ============================================================================
// UNDEFINE THE LOCALLY-SCOPED IMPLEMENTATION DETAIL MACROS
// ========================================================

#undef BSLS_ASSERT_NORETURN
#undef BSLS_ASSERT_NO_ASSERTION_MACROS_DEFINED

// ============================================================================
// IMPLEMENTATION USING THE C++ PREPROCESSOR
// =========================================
// At most one of the following build options may be set during the compilation
// of any component that includes 'bsls_assert.h':
//..
//  BSLS_ASSERT_LEVEL_ASSERT_SAFE
//  BSLS_ASSERT_LEVEL_ASSERT
//  BSLS_ASSERT_LEVEL_ASSERT_OPT
//  BSLS_ASSERT_LEVEL_NONE
//..
// ----------------------------------------------------------------------------

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) && \
    defined(BSLS_ASSERT_LEVEL_ASSERT)
#error incompatible BSLS_ASSERT levels: \
..._LEVEL_ASSERT_SAFE and ..._LEVEL_ASSERT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) && \
    defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)
#error incompatible BSLS_ASSERT levels: \
..._LEVEL_ASSERT_SAFE and ..._LEVEL_ASSERT_OPT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE) && \
    defined(BSLS_ASSERT_LEVEL_NONE)
#error incompatible BSLS_ASSERT levels: \
..._LEVEL_ASSERT_SAFE and ..._LEVEL_NONE
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT) && \
    defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)
#error incompatible BSLS_ASSERT levels: \
..._LEVEL_ASSERT and ..._LEVEL_ASSERT_OPT
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT) && \
    defined(BSLS_ASSERT_LEVEL_NONE)
#error incompatible BSLS_ASSERT levels: \
..._LEVEL_ASSERT and ..._LEVEL_NONE
#endif

#if defined(BSLS_ASSERT_LEVEL_ASSERT_OPT) && \
    defined(BSLS_ASSERT_LEVEL_NONE)
#error incompatible BSLS_ASSERT levels: \
..._LEVEL_ASSERT_OPT and ..._LEVEL_NONE
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
