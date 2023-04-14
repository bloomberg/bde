// bsls_review.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLS_REVIEW
#define INCLUDED_BSLS_REVIEW

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide assertion macros to safely identify contract violations.
//
//@CLASSES:
//  bsls::Review: namespace for "review" management functions
//  bsls::ReviewFailureHandlerGuard: scoped guard for changing handlers safely
//  bsls::ReviewViolation: attributes describing a failed review check
//
//@MACROS:
//  BSLS_REVIEW: runtime check typically enabled in all non-opt build modes
//  BSLS_REVIEW_SAFE: runtime check typically only enabled in safe build modes
//  BSLS_REVIEW_OPT: runtime check typically enabled in all build modes
//  BSLS_REVIEW_INVOKE: immediately invoke the current review handler
//
//@SEE_ALSO: bsls_assert
//
//@DESCRIPTION: This component provides three "assert-like" macros,
// 'BSLS_REVIEW', 'BSLS_REVIEW_SAFE', and 'BSLS_REVIEW_OPT', that can be used
// to enable optional *redundant* runtime checks in corresponding build modes
// that are designed to log their failures so production systems can be safely
// monitored for contract violations.
//
// This component is designed to allow apparently working production software,
// which nonetheless may harbor contract violations, to increase the number of
// precondition checks used to catch such bugs without negatively impacting the
// existing behavior of the software.  The assumption is that any contract
// violations uncovered through these checks in stable software is (at least
// for the moment) benign.  This component should *not* be used for assertions
// added to new code; new code should rely on 'bsls_assert', as any contract
// violations discovered in new code may not be benign and the resulting
// behavior may be much worse than the task terminating (as would happen using
// 'bsls_assert').
//
// If the argument of a review macro evaluates to 0, a runtime-configurable
// "handler" function is invoked with a 'bsls::ReviewViolation', a
// value-semantic class that encapsulates the current filename, line number,
// level of failed check, (0-valued expression) argument text, and a count of
// how many times that check has already failed.  The default handler logs that
// a failure has occurred and then allows processing to continue, thus not
// adversely impacting the running program.  The class 'bsls::Review' provides
// functions for manipulating the globally configured "handler".  A scoped
// guard for setting and restoring the review handler is provided by
// 'bsls::ReviewFailureHandlerGuard'.
//
// An additional macro, 'BSLS_REVIEW_INVOKE', is included for directly invoking
// the current review handler as if an assertion had failed on the current
// line.
//
///Detailed Behavior
///- - - - - - - - -
// If a review fires (i.e., due to a 0-valued expression argument in a review
// macro that is enabled), there is a violation of the contract that the review
// is checking.  For such a failing review, the program is in an undefined
// state but it is deemed inadvisable to immediately abort without further
// analysis.  It is the goal of the review to be sure to log that a contract
// was violated so the issue (in either the calling code or the contract) can
// be addressed.
//
// Reviews are enabled or disabled at compile time based on the review level,
// assertion level, and build mode flags that have been defined.  It is also
// possible that assert macros (see 'bsls_assert') can be configured in review
// mode, and so they may behave exactly as would review macros with appropriate
// build flags.
//
// When enabled, the review macros will all do essentially the same thing: Each
// macro tests the predicate expression 'X', and if '!(X)' is 'true', tracks a
// count of how many times this particular review has failed and invokes the
// currently installed review handler.  An instance of 'bsls::ReviewViolation'
// will be created and populated with a textual rendering of the predicate
// ('#X'), the current '__FILE__', the current '__LINE__', a string
// representing which particular type of review or assertion has failed, and
// the current count of how many times this predicate has failed (used
// primarily to throttle repeated logging of violations from the same
// location).  This 'violation' is then passed to the currently installed
// review failure handler, a function pointer with the type
// 'bsls::Review::ViolationHandler' having the signature:
//..
//  void(const bsls::ReviewViolation&)
//..
//
///Review Levels and Build Modes
///- - - - - - - - - - - - - - -
// There are a few macros available to control which of the review macros are
// actually enabled.  These macros are for the compilation and build
// environment to provide and are not themselves defined by BDE code -- e.g.,
// by supplying one or more of these macros with '-D' options on the compiler
// command line.  In general, these macros are used to determine a
// 'REVIEW_LEVEL' that can be 'NONE', 'REVIEW_OPT', 'REVIEW', or 'REVIEW_SAFE'.
// Depending on the review level, different review macros will be enabled.  For
// "safer" review configurations, more macros are enabled and all lower-level
// (and presumably lower cost) macros are kept enabled.  The 'NONE' level is
// primarily provided for testing to ensure that the 'REVIEW_OPT' level doesn't
// actually introduce any side-effects or incur measurable overhead when
// enabled, though in practice it should not be deployed.
//..
//   ==================================================
//   "REVIEW" Macro Instantiation Based on Review Level
//   ==================================================
//  BSLS_REVIEW_LEVEL  BSLS_REVIEW_OPT BSLS_REVIEW BSLS_REVIEW_SAFE
//  -----------------  --------------- ----------- ----------------
//  NONE
//  REVIEW_OPT         ENABLED
//  REVIEW             ENABLED         ENABLED
//  REVIEW_SAFE        ENABLED         ENABLED     ENABLED
//..
// The logic for the determination of the review level checks a few different
// macros.  The first check is for one of the 4 mutually exclusive
// 'BSLS_REVIEW_LEVEL' macros that can explicitly set the review level:
//..
//  MACRO                         BSLS_REVIEW_LEVEL
//  -----                         ----------------
//  BSLS_REVIEW_LEVEL_NONE        NONE
//  BSLS_REVIEW_LEVEL_REVIEW_OPT  REVIEW_OPT
//  BSLS_REVIEW_LEVEL_REVIEW      REVIEW
//  BSLS_REVIEW_LEVEL_REVIEW_SAFE REVIEW_SAFE
//..
// If none of those macros are defined, the review level is implemented to be
// exactly the same as the assertion level (see 'bsls_assert').  This is so
// that any introduced review macro will still be enabled (and become an
// assertion) when it is textually replaced with a 'BSLS_ASSERT'.  This means
// that first one of the 7 mutually exclusive 'BSLS_ASSERT_LEVEL' macros are
// checked to determine the review level:
//..
//  MACRO                           BSLS_REVIEW_LEVEL
//  -----                           ----------------
//  BSLS_ASSERT_LEVEL_ASSUME_SAFE   NONE
//  BSLS_ASSERT_LEVEL_ASSUME_ASSERT NONE
//  BSLS_ASSERT_LEVEL_ASSUME_OPT    NONE
//  BSLS_ASSERT_LEVEL_NONE          NONE
//  BSLS_ASSERT_LEVEL_ASSERT_OPT    REVIEW_OPT
//  BSLS_ASSERT_LEVEL_ASSERT        REVIEW
//  BSLS_ASSERT_LEVEL_ASSERT_SAFE   REVIEW_SAFE
//..
// Finally, the default review (and assert) level, if none of the overriding
// review or assert level macros above are defined, is determined by the build
// mode.  With "safer" build modes we incorporate higher-level defensive
// program checks.  A particular build mode is implied by the relevant (BDE)
// build targets that are defined at compilation (preprocessing) time.  The
// following table shows the three (BDE) build targets that can affect the
// assertion and review levels (note that 'BDE_BUILD_TARGET_DBG' plays no
// role):
//..
//        (BDE) Build Targets
//      -----------------------
//  (A) BDE_BUILD_TARGET_SAFE_2
//  (B) BDE_BUILD_TARGET_SAFE
//  (C) BDE_BUILD_TARGET_OPT
//..
// *Any* of the 8 possible combinations of the three build targets is valid;
// e.g., 'BDE_BUILD_TARGET_OPT' and 'BDE_BUILD_TARGET_SAFE_2' may both be
// defined.  The following table shows the review level that is set depending
// on which combination of build target macros have been set:
//..
//   =========================================================
//   "REVIEW" Level Set With no Level-Overriding Flags defined
//   =========================================================
//  --- BDE_BUILD_TARGET ----   BSLS_REVIEW_LEVEL
//  _SAFE_2   _SAFE    _OPT
//  -------  -------  -------   -----------------
//                              REVIEW
//                    DEFINED   REVIEW_OPT
//           DEFINED            REVIEW_SAFE
//           DEFINED  DEFINED   REVIEW_SAFE
//  DEFINED                     REVIEW_SAFE
//  DEFINED           DEFINED   REVIEW_SAFE
//  DEFINED  DEFINED            REVIEW_SAFE
//  DEFINED  DEFINED  DEFINED   REVIEW_SAFE
//..
// As the table above illustrates, with no build target explicitly defined the
// review level defaults to 'REVIEW'.  If only 'BDE_BUILD_TARGET_OPT' is
// defined, the review level will be set to 'REVIEW_OPT'.  If either
// 'BDE_BUILD_TARGET_SAFE' or 'BDE_BUILD_TARGET_SAFE_2' is defined then the
// review level is set to 'REVIEW_SAFE' and ALL review macros will be enabled.
//
///Runtime-Configurable Review-Failure Behavior
///- - - - - - - - - - - - - - - - - - - - - -
// In addition to the three (BSLS) "REVIEW" macros, 'BSLS_REVIEW',
// 'BSLS_REVIEW_SAFE', and 'BSLS_REVIEW_OPT', and the immediate invocation
// macro 'BSLS_REVIEW_INVOKE', this component provides (1) an 'invokeHandler'
// method used (primarily) to implement these "REVIEW" macros and enable their
// runtime configuration, (2) administrative methods to configure, at runtime,
// the behavior resulting from a review failure (i.e., by installing an
// appropriate review-failure handler function), and (3) a suite of standard
// ("off-the-shelf") review-failure handler functions, to be installed via the
// methods (if desired), and invoked by the 'invokeHandler' method of a review
// failure.
//
// When an enabled review fails, the currently installed *failure* *handler*
// ("callback") function is invoked.  The default handler is the ('static')
// 'bsls::Review::failByLog' method, which will log the failure and the current
// callstack when invoked for the first time, and exponentially less frequently
// as additional failures of the same review site occur.  A user may replace
// this default handler by using the ('static')
// 'bsls::Review::setViolationHandler' administrative method and passing it
// (the address of) a function whose signature conforms to the
// 'bsls::Review::ViolationHandler' 'typedef'.  This handler may be one of the
// other handler methods provided in 'bsls::Review', or a "custom" function
// written by the user.
//
// One additional provided class, 'bsls::ReviewFailureHandlerGuard', can be
// used to override the review handler within a specific block of code.  Note
// that this is primarily intended for testing and should be instantiated at a
// non-granular level, as the setting and resetting of the handler done by this
// RAII class has no provisions in it to handle being used concurrently by
// multiple threads.
//
// The primary uses for setting the review handler to a non-default handler are
// to get all reviews to behave like some kind of an assertion, or to get
// reviews to throw exceptions to facilitate testing.  Both of these scenarios
// are primarily encountered in test drivers, and the general workflows for
// reviews and assertions depend on a policy where deployed production tasks
// use the default handlers.
//
///Conditional Compilation
///- - - - - - - - - - - -
// To recap, there are three (mutually compatible) general *build* *targets*:
//: o 'BDE_BUILD_TARGET_OPT'
//: o 'BDE_BUILD_TARGET_SAFE'
//: o 'BDE_BUILD_TARGET_SAFE_2'
// seven (mutually exclusive) component-specific *assertion* *levels*:
//: o 'BSLS_ASSERT_LEVEL_ASSERT_SAFE'
//: o 'BSLS_ASSERT_LEVEL_ASSERT'
//: o 'BSLS_ASSERT_LEVEL_ASSERT_OPT'
//: o 'BSLS_ASSERT_LEVEL_NONE'
//: o 'BSLS_ASSERT_LEVEL_ASSUME_OPT'
//: o 'BSLS_ASSERT_LEVEL_ASSUME_ASSERT'
//: o 'BSLS_ASSERT_LEVEL_ASSUME_SAFE'
// and four (mutually exclusive) component-specific *review* *levels*:
//: o 'BSLS_REVIEW_LEVEL_REVIEW_SAFE'
//: o 'BSLS_REVIEW_LEVEL_REVIEW'
//: o 'BSLS_REVIEW_LEVEL_REVIEW_OPT'
//: o 'BSLS_REVIEW_LEVEL_NONE'
// The above macros can be defined (externally) by the build environment to
// affect which of the three *review* *macros*:
//: o 'BSLS_REVIEW_SAFE(boolean-valued expression)'
//: o 'BSLS_REVIEW(boolean-valued expression)'
//: o 'BSLS_REVIEW_OPT(boolean-valued expression)'
// will be enabled (i.e., instantiated).
//
// The public interface of this component also provides some additional
// intermediate macros to identify how the various 'BSLS_REVIEW' macros have
// been instantiated.  These exist for each level and have the following
// suffixes and meanings:
//: o 'IS_ACTIVE': Defined if the corresponding level is enabled.
//: o 'IS_USED': Defined if the expressions for the corresponding level need to
//:   be valid (i.e., if they are"'ODR-used").
//
// Putting that together, these 3 macros are defined if the corresponding macro
// is enabled:
//: o 'BSLS_REVIEW_SAFE_IS_ACTIVE'
//: o 'BSLS_REVIEW_IS_ACTIVE'
//: o 'BSLS_REVIEW_OPT_IS_ACTIVE'
//
// Finally, three more macros with the 'IS_USED' suffix are defined when the
// expression for the corresponding macro is going to be compiled.  This will
// be true if the macro is enabled or if 'BSLS_REVIEW_VALIDATE_DISABLED_MACROS'
// has been defined.
//: o 'BSLS_REVIEW_SAFE_IS_USED'
//: o 'BSLS_REVIEW_IS_USED'
//: o 'BSLS_REVIEW_OPT_IS_USED'
//
// All of these additional "predicate" macros can be used directly by clients
// of this component to conditioanlly compile code other than just (BSLS)
// reviews, but that should be done with care to be sure code compiles and is
// compatible across all build modes.
//
///Validating Disabled Macro Expressions
///- - - - - - - - - - - - - - - - - - -
// An additional external macro, 'BSLS_REVIEW_VALIDATE_DISABLED_MACROS', can be
// defined to control the compile time behavior of 'bsls_review'.  Enabling
// this macro configures all *disabled* review macros to still instantiate
// their predicates (in a non-evaluated context) to be sure that the predicate
// is still syntactically valid.  This can be used to ensure reviews that are
// rarely enabled have valid expressions.
//
///Uses for 'bsls_review'
///- - - - - - - - - -
// 'bsls_review' exists primarily as a step towards adding or modifying
// instances of 'bsls_assert' macros in code that are already running in
// production.  New 'bsls_assert' macros are risky to add, and enabled
// 'bsls_assert' macros that were not previously enabled is equally risky.
//
// Given code that is already running and deployed, but not "breaking" in
// obvious ways, the review process provides a way to see if contracts are
// being met in those running processes without risking unexpected aborts
// happening in those systems.  The contracts that might be getting violated in
// already deployed applications need to be fixed and avoided, BUT there exists
// no evidence that these violations are currently crashing the system.
//
// In general, 'bsls_review' can also be considered as a way to identify if
// deployed code is violating function contracts in some way.  Once calling
// applications and library code have been altered so all contracts are being
// met, uses of 'bsls_review' macros are then changed into the corresponding
// 'bsls_assert' macros, which enforce the contracts instead of simply
// monitoring them.  In some cases, the 'bsls_review' macros can also be
// replaced by new behavior, e.g., widening narrow contracts without risking
// breaking the expectations of existing applications.
//
///Adding New 'bsls_assert' Checks To New Code
///- - - - - - - - - - - - - - - - - - - - - -
// 'bsls_review' is not appropriate for "new" code that has not yet been
// deployed to a production environment; use 'bsls_assert' instead.
//
///Adding Assertions With 'bsls_review'
///- - - - - - - - - - - - - - - - -
// The introduction of a new assertion using 'bsls_review' should follow these
// steps:
//: 1 Add the appropriate 'bsls_review' macros to your code.
//: 2 Commit these changes and make sure all processes using your code get
//:   rebuilt and deployed to production.
//: 3 Monitor relevant log files for "BSLS_REVIEW failure" messages citing the
//:   location of these reviews.
//: 4 If any failures occurred, fix the calling code, or widen the contract
//:   being violated (which should be rare).
//: 5 Wait for sufficient time to pass to be confident that the contract is not
//:   being violated by the current version of the software in production.
//: 6 Replace 'BSLS_REVIEW' with 'BSLS_ASSERT' and redeploy.
//
///Reducing The Assertion Level For Existing 'bsls_assert' Macros
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In order to reduce the level of an existing assertion, such as changing a
// 'BSLS_ASSERT_SAFE' into a 'BSLS_ASSERT', or a 'BSLS_ASSERT' into a
// 'BSLS_ASSERT_OPT', you should follow a very similar process to the process
// for adding a new assertion of that level.  There are two possible approaches
// that largely depend on how widely deployed the code is that is already built
// with the old assertions enabled.
//: 1 If the existing assertion is not enabled in deployed production code,
//:   such as a 'BSLS_ASSERT_SAFE' where client code rarely or never deploys
//:   safe builds, then simply remove the old assertion and start the process
//:   described above of adding in the higher level assertion as if it was a
//:   newly added assertion.  Here changing a 'BSLS_ASSERT_SAFE' to a
//:   'BSLS_ASSERT' would then begin with changing the 'BSLS_ASSERT_SAFE' into
//:   a 'BSLS_REVIEW' and continuing as above.
//: 2 If the existing assertion is depended on in released production code then
//:   the process involved needs to maintain the assertion at the existing
//:   level while adding in the review at the higher level, eventually
//:   including only the assert at the higher level when the process is
//:   complete.
//
// Therefore, the "safest" way to increase an assertion level is to follow
// these steps:
//: 1 Given an existing 'BSLS_ASSERT' such as this:
//:..
//:  BSLS_ASSERT(some_test());
//:..
//: You can duplicate the test as a 'BSLS_REVIEW_OPT', if the test is very
//: negligible, like this:
//:..
//:  BSLS_ASSERT(some_test());
//:  BSLS_REVIEW_OPT(some_test());
//:..
//: If the duplicated check is unacceptably expensive (which should make you
//: question making this assertion a 'BSLS_ASSERT_OPT' in the first place),
//: then you can opt for the more cumbersome:
//:..
//:  #ifdef BSLS_ASSERT_IS_ACTIVE
//:    BSLS_ASSERT(some_test());
//:  #else
//:    BSLS_REVIEW_OPT(some_test());
//:  #endif
//:..
//: 2 Commit these changes and make sure all processes built at the higher
//:   assertion level are rebuilt and deployed, thus beginning the review
//:   process for this changed assertion.
//: 3 Follow steps 3-5 of the {Adding Assertions With 'bsls_review'} workflow.
//: 4 When complete, replace all changes made in step 1 with:
//:..
//:  BSLS_ASSERT_OPT(some_test());
//:..
//: 5 Revel in the more widespread use of your existing assertion.
//
///Increasing Deployed Assertion Levels With 'BSLS_REVIEW_LEVEL_*'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A common situation in deployed tasks is that they are built with only
// 'BSLS_ASSERT_OPT' enabled.  To improve the robustness of these applications,
// there is then a desire to run them with 'BSLS_ASSERT' enabled, or even
// 'BSLS_ASSERT_SAFE'.  Enabling these assertions, however, brings in the same
// risks of hard failures for contract violations that adding new assertions
// does.  'BSLS_REVIEW' provides a process for making this change without
// risking aborting processes that were previously "running just fine".
//
// Any explicit setting of the 'BSLS_REVIEW_LEVEL' ("review level") to a level
// higher than the 'BSLS_ASSERT_LEVEL' ("assert level") will not only enable
// the 'BSLS_REVIEW' macros at that level, but it will turn all 'BSLS_ASSERT'
// macros at that level into reviews as well, and disable any assumption of
// 'BSLS_ASSERT' assertions.  Given a task that is built with an assertion
// level of 'OPT', if you set the review level to 'REVIEW' you will then get
// logs and notifications of any failed 'BSLS_ASSERT' checks, but those failing
// checks will not immediately abort your application.
//
// So the process for deploying an application with a higher assertion level is
// to follow these steps:
//: 1 Rebuild the task with the review level set to the desired assertion
//:   level, using 'BSLS_REVIEW_LEVEL_REVIEW_OPT', 'BSLS_REVIEW_LEVEL_REVIEW',
//:   or 'BSLS_REVIEW_LEVEL_REVIEW_SAFE'.
//: 2 Deploy the task.
//: 3 Monitor relevant log files for "BSLS_REVIEW failure" messages citing the
//:   location of these reviews.
//: 4 Once all review failures have been addressed and no failures are logged
//:   for a "sufficient" time, remove the explicitly set 'BSLS_REVIEW_LEVEL'
//:   from your build and change to set an explicit 'BSLS_ASSERT_LEVEL' at your
//:   new level.
//: 5 Deploy your newly built application with increased enabled defensive
//:   checks.
//: 6 Revel in the comfort of taking advantage of the additional defensive
//:   checks now enabled in your code.
//
///Checking Library Usage Before Changing it With 'bsls_review'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Occasionally, given code that is in use in production, you may come across a
// case that makes no sense but that you want to reimplement with altered
// behavior.  The previous behavior might have been out of contract or just
// seemingly nonsensical, but you are not confident that no one is actually
// running code that executes that behavior and relies on it.
//
// 'bsls_review' here provides a way to insert a review that checks if the code
// in question is ever being invoked.  To do that, follow these steps:
//: 1 Depending on the structure of the code you want to monitor, add a
//:   'BSLS_REVIEW_INVOKE' or a 'BSLS_REVIEW_OPT' with a check that will fail
//:   on the condition you want to monitor.  Use these macros as they will be
//:   enabled in almost all build modes.
//: 2 Commit your changes and get the applications using your code deployed.
//: 3 If any code hits your review, assess it for why and fix the calling code
//:   or re-assess the behavior you want to change.
//: 4 Once "sufficient" time has passed with no review failures, remove the
//:   'bsls_review' checks entirely.
//: 5 Make changes to your code's behavior that impact only those states where
//:   previously your review would have failed.  Check in and deploy those
//:   changes.
//: 6 Revel in safely having deployed a Liskov-substitutable version of your
//:   library with exciting and new behavior.
//
///Concerns When Adding Reviews or Assertions
///- - - - - - - - - - - - - - - - - - - - -
//: 1 Performance: In general, a new 'bsls_review' check will perform exactly
//:   the same as a 'bsls_assert' with the same predicate.  One subtle
//:   difference is that apparently working software can contain failing
//:   'bsls_review' checks logging failures that may be ignored (whereas
//:   'bsls_assert' failures are designed to be hard to ignore).  Handling
//:   these failures requires maintaining the failure count using atomic
//:   operations that may negatively impact performance if checks are failing
//:   (and ignored) in performance-sensitive code.  Failing checks should
//:   always be addressed as promptly as possible.
//:
//: 2 Downstream Linking: For a library developer, part of the review rollout
//:   process will rely heavily on users of your library relinking multiple
//:   times during the process of adding reviews.  One might assume that this
//:   means the verification that a review is not failing can extend an
//:   indefinite amount of time.  The better overall policy is to realize that,
//:   just like library users can opt-in to rebuilding their tasks more often,
//:   the stability benefits of being involved in the review process by
//:   relinking and rolling out more often are opt-in as well.  For tasks that
//:   are rebuilt very infrequently, they will simply have to accept that
//:   library misuse on their end might result in crashes due to asserts that
//:   were introduced between their own too-infrequent releases.
//:
//: 3 Unit Testing with Reviews: In general, unit test log files are rarely
//:   monitored or read, so a failing 'BSLS_REVIEW' is unlikely to be caught
//:   during unit tests.  Test drivers should almost always set the review
//:   handler to the abort handler so any failed reviews are caught immediately
//:   as test failures.
//:
//: 4 Sufficient Time: All of the review-related workflows mention running
//:   reviewed code for "sufficient" time to know the check is not failing.
//:   "Sufficient" time will vary by application.
//
///Usage
///-----
//
///Example 1: Adding 'BSLS_ASSERT' To An Existing Function
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose you have an existing function, already deployed to production, that
// was not written with defensive programming in mind.  In order to increase
// robustness, you would like to add 'BSLS_ASSERT' macros to this function that
// match the contract originally written when this function was initially
// released.
//
// For example, consider the function 'myFunc' in the class 'FunctionsV1' that
// was implemented like this:
//..
// my_functions.h
// ...
//
//  class FunctionsV1 {
//      // ...
//    public:
//      // ...
//
//      static int myFunc(int x, int y);
//          // Do something with the specified positive integers 'x' and 'y'.
//  };
//
//  inline int FunctionsV1::myFunc(int x, int y)
//  {
//      int output = 0;
//      // ... do stuff with 'x' and 'y'.
//      return output;
//  }
//..
// Notice that there are no checks on 'x' and 'y' within 'myFunc' and no
// assertions to detect use of 'myFunc' outside of its contract.  On the other
// hand, 'myFunc' is part of legacy code that has been in use extensively for
// years or decades, so clearly this is not causing a problem (yet).
//
// Upon reviewing this class you realize that 'myFunc' produces random results
// for values of 'x' or 'y' less than 0.  You, however, do not have enough
// information to conclude that no one is calling it with negative values and
// just using the bad results unknowingly.  There are a number of possibilities
// for how the result of this undefined behavior might be going unnoticed.
//: o The invalid value might be discarded by a bounds check later in the
//:   process.
//: o The invalid value may only result in a small glitch the users have not
//:   noticed or ignored.
//: o The resulting value may actually be valid, but allowing negative input
//:   for 'x' and 'y' may preclude potential future development in ways we do
//:   not want to allow.
// All of these are bad, but adding in checks with 'BSLS_ASSERT' that would
// replace these bad behaviors by process termination would turn silent errors
// into loud errors (potentially worse).  On the other hand, by not adding
// 'BSLS_ASSERT' checks we permit future misuses of this function, which may
// not be innocuous, to potentially reach production systems.  'BSLS_REVIEW'
// here serves as a bridge, from the current state of 'myFunc' (entirely
// unchecked) to the ideal state of 'myFunc' (where misuse is caught loudly and
// immediately through 'BSLS_ASSERT'), following a path that doesn't risk
// turning an un-noticed or irrelevant error into one that will significantly
// hinder ongoing business.
//
// The solution to this is to *initially* reimplement 'myFunc' using
// 'BSLS_REVIEW' like this:
//..
// my_functions.h
// ...
//  #include <bsls_review.h>
// ...
//
//  class FunctionsV2 {
//      // ...
//    public:
//      // ...
//
//      static int myFunc(int x, int y);
//          // Do something with the specified 'x' and 'y'.  The behavior is
//          // undefined unless 'x > 0' and 'y > 0'.
//  };
//
//  inline int FunctionsV2::myFunc(int x, int y)
//  {
//      BSLS_REVIEW(x > 0);
//      BSLS_REVIEW(y > 0);
//      int output = 0;
//      // ... do stuff with 'x' and 'y'.
//      return output;
//  }
//..
// Now you can deploy this code to production and then begin reviewing logs.
// The log messages you should look for are those produced by 'bsls::Review's
// default review failure handler and will be similar to:
//..
//  ERROR myfunction.h::17 BSLS_REVIEW failure (level:R-DBG): 'x > 0'
//                                     Please run "/bb/bin/showfunc.tsk ...
//..
// 'showfunc.tsk' is a Bloomberg application that can be used (along with the
// task binary) to convert the reported stack addresses to a more traditional
// stack trace with a function call stack.
//
// It is important to note that 'BSLS_REVIEW' is purely informative, and adding
// a review will not adversely affect behavior, and may in fact alert the
// library author to common client misconceptions about the intended behavior.
//
// For example, let's say actual usage makes it clear that users expect 0 to be
// valid values for the arguments to 'myFunc', and nothing in the
// implementation prevents us from accepting 0 as input and producing the
// answer clients expect.  Instead of changing all the clients, we may instead
// choose to change the function contract (and implemented checks):
//..
// my_functions.h
// ...
//  #include <bsls_review.h>
// ...
//
//  class FunctionsV3 {
//      // ...
//    public:
//      // ...
//
//      static int myFunc(int x, int y);
//          // Do something with the specified 'x' and 'y'.  The behavior is
//          // undefined unless 'x >= 0' and 'y >= 0'.
//  };
//
//  inline int FunctionsV3::myFunc(int x, int y)
//  {
//      BSLS_REVIEW(x >= 0);
//      BSLS_REVIEW(y >= 0);
//      int output = 0;
//      // ... do stuff with 'x' and 'y'.
//      return output;
//  }
//..
// Finally, at some point, the implementation of 'myFunc' using 'BSLS_REVIEW'
// has been running a suitable amount of time that you are comfortable
// transitioning the use of 'bsls_review' to 'bsls_assert'.  We now use our
// favorite text editor or script to replace "BSLS_REVIEW" with "BSLS_ASSERT":
//..
// my_functions.h
// ...
//  #include <bsls_assert.h>
// ...
//
//  class FunctionsV4 {
//      // ...
//    public:
//      // ...
//
//      static int myFunc(int x, int y);
//          // Do something with the specified 'x' and 'y'.  The behavior is
//          // undefined unless 'x >= 0' and 'y >= 0'.
//  };
//
//  inline int FunctionsV4::myFunc(int x, int y)
//  {
//      BSLS_ASSERT(x >= 0);
//      BSLS_ASSERT(y >= 0);
//      int output = 0;
//      // ... do stuff with 'x' and 'y'.
//      return output;
//  }
//..
// At this point, any contract violations in the use of 'myFunc' in new code
// will be caught immediately (i.e., in appropriate build modes).

#include <bsls_assertimputil.h>
#include <bsls_atomicoperations.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#ifdef BSLS_ASSERT_USE_CONTRACTS
#include <contract>
#endif

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLS_REVIEW)
#error BSLS_REVIEW is already defined!
#endif

#if defined(BSLS_REVIEW_REVIEW_IMP)
#error BSLS_REVIEW_REVIEW_IMP is already defined!
#endif

#if defined(BSLS_REVIEW_REVIEW_COUNT_IMP)
#error BSLS_REVIEW_REVIEW_COUNT_IMP is already defined!
#endif

#if defined(BSLS_REVIEW_DISABLED_IMP)
#error BSLS_REVIEW_DISABLED_IMP is already defined!
#endif

#if defined(BSLS_REVIEW_INVOKE)
#error BSLS_REVIEW_INVOKE is already defined!
#endif

#if defined(BSLS_REVIEW_IS_ACTIVE)
#error BSLS_REVIEW_IS_ACTIVE is already defined!
#endif

#if defined(BSLS_REVIEW_IS_USED)
#error BSLS_REVIEW_IS_USED is already defined!
#endif

#if defined(BSLS_REVIEW_OPT)
#error BSLS_REVIEW_OPT is already defined!
#endif

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
#error BSLS_REVIEW_OPT_IS_ACTIVE is already defined!
#endif

#if defined(BSLS_REVIEW_OPT_IS_USED)
#error BSLS_REVIEW_OPT_IS_USED is already defined!
#endif

#if defined(BSLS_REVIEW_SAFE)
#error BSLS_REVIEW_SAFE is already defined!
#endif

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
#error BSLS_REVIEW_SAFE_IS_ACTIVE is already defined!
#endif

#if defined(BSLS_REVIEW_SAFE_IS_USED)
#error BSLS_REVIEW_SAFE_IS_USED is already defined!
#endif

                     // =================================
                     // (BSLS) "REVIEW" Macro Definitions
                     // =================================

// Implementation Note: We wrap the 'if' statement below in a (seemingly
// redundant) do-while-false loop to require, syntactically, a trailing
// semicolon, and to ensure that the macro behaves properly in an if-then-else
// context -- even if one forgets to wrap, with curly braces, the body of an
// 'if' having just a single 'BSLS_REVIEW*' statement.

               // =============================================
               // Factored Implementation for Internal Use Only
               // =============================================

#if !(defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE) ||                               \
      defined(BSLS_REVIEW_LEVEL_REVIEW) ||                                    \
      defined(BSLS_REVIEW_LEVEL_REVIEW_OPT) ||                                \
      defined(BSLS_REVIEW_LEVEL_NONE))
// In order to replicate the control logic of 'BSLS_ASSERT', if there has been
// no explicit review level set we check to see if there has been an explicit
// assert level set.  If so, we act as though the review level has been
// explicitly set to the same thing (instead of just basing the review level
// off of only the build mode).
    #if defined(BSLS_ASSERT_LEVEL_ASSERT_SAFE)
       #define BSLS_REVIEW_LEVEL_REVIEW_SAFE
       #define BSLS_REVIEW_NO_REVIEW_MACROS_DEFINED 0
    #elif defined(BSLS_ASSERT_LEVEL_ASSERT)
       #define BSLS_REVIEW_LEVEL_REVIEW
       #define BSLS_REVIEW_NO_REVIEW_MACROS_DEFINED 0
    #elif defined(BSLS_ASSERT_LEVEL_ASSERT_OPT)
       #define BSLS_REVIEW_LEVEL_REVIEW_OPT
       #define BSLS_REVIEW_NO_REVIEW_MACROS_DEFINED 0
    #elif defined(BSLS_ASSERT_LEVEL_NONE) ||                                  \
          defined(BSLS_ASSERT_LEVEL_ASSUME_SAFE) ||                           \
          defined(BSLS_ASSERT_LEVEL_ASSUME_ASSERT) ||                         \
          defined(BSLS_ASSERT_LEVEL_ASSUME_OPT)
       #define BSLS_REVIEW_LEVEL_NONE
       #define BSLS_REVIEW_NO_REVIEW_MACROS_DEFINED 0
    #else
       // Only here, with no explicit review level OR assertion level, does
       // this macro finally get set to true, which will trigger
       // buildtarget-based logic for macro configuration
       #define BSLS_REVIEW_NO_REVIEW_MACROS_DEFINED 1
    #endif
#else
    #define BSLS_REVIEW_NO_REVIEW_MACROS_DEFINED 0
#endif

                        // ============================
                        // BSLS_REVIEW_REVIEW_COUNT_IMP
                        // ============================

// This macro is defined in order to maintain a static 'count' where a
// 'BSLS_REVIEW' is used.  When possible, this is done inside a lamba (which
// will only be invoked when a violation happens) in order to facilitate use
// within 'constexpr' functions.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR_CPP14
    #define BSLS_REVIEW_REVIEW_COUNT_IMP                                      \
        int lastCount = BloombergLP::bsls::Review::updateCount(               \
            []{                                                               \
                static BloombergLP::bsls::Review::Count count = {0};          \
                return &count;                                                \
            }() );
#else
    #define BSLS_REVIEW_REVIEW_COUNT_IMP                                      \
        static BloombergLP::bsls::Review::Count count = {0};                  \
        int lastCount = BloombergLP::bsls::Review::updateCount(&count);
#endif

                           // ======================
                           // BSLS_REVIEW_REVIEW_IMP
                           // ======================

#ifdef BSLS_ASSERT_USE_CONTRACTS
#define BSLS_REVIEW_REVIEW_IMP(X,LVL) [[ assert check_maybe_continue : X ]]

#ifdef BSLS_REVIEW_VALIDATE_DISABLED_MACROS
#define BSLS_REVIEW_DISABLED_IMP(X,LVL)   [[ assert ignore : X ]]
#else
#define BSLS_REVIEW_DISABLED_IMP(X,LVL)
#endif

#else // BSLS_ASSERT_USE_CONTRACTS

#define BSLS_REVIEW_REVIEW_IMP(X,LVL) do {                                    \
        if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!(X))) {                    \
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;                               \
            BSLS_REVIEW_REVIEW_COUNT_IMP;                                     \
            BloombergLP::bsls::ReviewViolation violation(                     \
                                                     #X,                      \
                                                     BSLS_ASSERTIMPUTIL_FILE, \
                                                     BSLS_ASSERTIMPUTIL_LINE, \
                                                     LVL,                     \
                                                     lastCount);              \
            BloombergLP::bsls::Review::invokeHandler(violation);              \
        }                                                                     \
    } while (false)

#ifdef BSLS_REVIEW_VALIDATE_DISABLED_MACROS
#define BSLS_REVIEW_DISABLED_IMP(X,LVL) (void)sizeof((X)?true:false)
#else
#define BSLS_REVIEW_DISABLED_IMP(X,LVL)
#endif

#endif

                             // ==================
                             // BSLS_REVIEW_INVOKE
                             // ==================

// 'BSLS_REVIEW_INVOKE' is always active.
#define BSLS_REVIEW_INVOKE(X) do {                                            \
        BSLS_REVIEW_REVIEW_COUNT_IMP;                                         \
        BloombergLP::bsls::ReviewViolation violation(                         \
                                  X,                                          \
                                  BSLS_ASSERTIMPUTIL_FILE,                    \
                                  BSLS_ASSERTIMPUTIL_LINE,                    \
                                  BloombergLP::bsls::Review::k_LEVEL_INVOKE,  \
                                  lastCount);                                 \
        BloombergLP::bsls::Review::invokeHandler(violation);                  \
    } while (false)

                              // ================
                              // BSLS_REVIEW_SAFE
                              // ================

// Determine if 'BSLS_REVIEW_SAFE' should be active.

#if defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE)                                    \
    || BSLS_REVIEW_NO_REVIEW_MACROS_DEFINED && (                              \
           defined(BDE_BUILD_TARGET_SAFE_2) ||                                \
           defined(BDE_BUILD_TARGET_SAFE)         )

    #define BSLS_REVIEW_SAFE_IS_ACTIVE  // also usable directly in client code
#endif

// Indicate when 'BSLS_REVIEW_SAFE' arguments will be ODR-used.
#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE) ||                                    \
    defined(BSLS_REVIEW_VALIDATE_DISABLED_MACROS)
    #define BSLS_REVIEW_SAFE_IS_USED
#endif

// Define 'BSLS_REVIEW_SAFE' accordingly.

#if defined(BSLS_REVIEW_SAFE_IS_ACTIVE)
    #define BSLS_REVIEW_SAFE(X) BSLS_REVIEW_REVIEW_IMP(                       \
                                     X,                                       \
                                     BloombergLP::bsls::Review::k_LEVEL_SAFE)
#else
    #define BSLS_REVIEW_SAFE(X) BSLS_REVIEW_DISABLED_IMP(                     \
                                     X,                                       \
                                     BloombergLP::bsls::Review::k_LEVEL_SAFE)
#endif

                                // ===========
                                // BSLS_REVIEW
                                // ===========

// Determine if 'BSLS_REVIEW' should be active.

#if defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE) ||                                 \
    defined(BSLS_REVIEW_LEVEL_REVIEW)                                         \
    || BSLS_REVIEW_NO_REVIEW_MACROS_DEFINED && (                              \
           defined(BDE_BUILD_TARGET_SAFE_2) ||                                \
           defined(BDE_BUILD_TARGET_SAFE)   ||                                \
           !defined(BDE_BUILD_TARGET_OPT)         )

    #define BSLS_REVIEW_IS_ACTIVE       // also usable directly in client code
#endif

// Indicate when 'BSLS_REVIEW' arguments will be ODR-used.
#if defined(BSLS_REVIEW_IS_ACTIVE) ||                                         \
    defined(BSLS_REVIEW_VALIDATE_DISABLED_MACROS)
    #define BSLS_REVIEW_IS_USED
#endif

// Define 'BSLS_REVIEW' accordingly.

#if defined(BSLS_REVIEW_IS_ACTIVE)
    #define BSLS_REVIEW(X) BSLS_REVIEW_REVIEW_IMP(                            \
                                   X,                                         \
                                   BloombergLP::bsls::Review::k_LEVEL_REVIEW)
#else
    #define BSLS_REVIEW(X) BSLS_REVIEW_DISABLED_IMP(                          \
                                   X,                                         \
                                   BloombergLP::bsls::Review::k_LEVEL_REVIEW)
#endif

                              // ===============
                              // BSLS_REVIEW_OPT
                              // ===============

// Determine if 'BSLS_REVIEW_OPT' should be active.

#if !defined(BSLS_REVIEW_LEVEL_NONE)
    #define BSLS_REVIEW_OPT_IS_ACTIVE   // also usable directly in client code
#endif

// Indicate when 'BSLS_REVIEW_OPT' arguments will be ODR-used.
#if defined(BSLS_REVIEW_OPT_IS_ACTIVE) ||                                     \
    defined(BSLS_REVIEW_VALIDATE_DISABLED_MACROS)
    #define BSLS_REVIEW_OPT_IS_USED
#endif

// Define 'BSLS_REVIEW_OPT' accordingly.

#if defined(BSLS_REVIEW_OPT_IS_ACTIVE)
    #define BSLS_REVIEW_OPT(X) BSLS_REVIEW_REVIEW_IMP(                        \
                                      X,                                      \
                                      BloombergLP::bsls::Review::k_LEVEL_OPT)
#else
    #define BSLS_REVIEW_OPT(X) BSLS_REVIEW_DISABLED_IMP(                      \
                                      X,                                      \
                                      BloombergLP::bsls::Review::k_LEVEL_OPT)
#endif

// A nested include guard is needed to support the test driver implementation.
#ifndef BSLS_REVIEW_RECURSIVELY_INCLUDED_TESTDRIVER_GUARD
#define BSLS_REVIEW_RECURSIVELY_INCLUDED_TESTDRIVER_GUARD

namespace BloombergLP {
namespace bsls {

                           // =====================
                           // class ReviewViolation
                           // =====================

class ReviewViolation {
    // This class is an unconstrained *in-core* value-semantic class that
    // characterizes the details of a review failure that has occurred.

    // DATA
    const char *d_comment_p;      // the comment associated with the violation,
                                  // generally representing the expression that
                                  // failed

    const char *d_fileName_p;     // the name of the file where the violation
                                  // occurred

    int         d_lineNumber;     // the line number where the violation
                                  // occurred

    const char *d_reviewLevel_p;  // the level and type of the violation that
                                  // occurred, generally one of the 'k_LEVEL'
                                  // constants defined in 'bsls::Review' or
                                  // 'bsls::Assert'

    int         d_count;          // the number of times that a particular
                                  // failure has happened in the running
                                  // process

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    ReviewViolation(const char *comment,
                    const char *fileName,
                    int         lineNumber,
                    const char *reviewLevel,
                    int         count);
        // Create a 'ReviewViolation' with the specified 'comment', 'fileName',
        // 'lineNumber', 'reviewLevel', and 'count'.  Note that the supplied
        // 'reviewLevel' will usually be one of the 'k_LEVEL' constants defined
        // in 'bsls::Review' (or see 'bsls::Assert' for the levels that will be
        // passed for assertions that are being reviewed).

    // ACCESSORS
    const char *comment() const;
        // Return the 'comment' attribute of this object.

    int count() const;
        // Return the 'count' attribute of this object.

    const char *fileName() const;
        // Return the 'fileName' attribute of this object.

    int lineNumber() const;
        // Return the 'lineNumber' attribute of this object.

    const char *reviewLevel() const;
        // Return the 'reviewLevel' attribute of this object.
};

                                // ============
                                // class Review
                                // ============

class Review {
    // This "utility" class maintains a pointer containing the address of the
    // current review-failure handler function (of type
    // 'Review::ViolationHandler') and provides methods to administer this
    // function pointer.  The 'invokeHandler' method calls the
    // currently-installed failure handler.  The default installed handler is
    // the 'Review::failByLog' function.
    //
    // This class also provides a suite of standard failure-handler functions
    // that are suitable to be installed as the current
    // 'Review::ViolationHandler' function.  Note that clients are free to
    // install any of these ("off-the-shelf") handlers, or to provide their own
    // ("custom") review-failure handler function when using this facility.
    // Also note that review-failure handlers CAN return, unlike assertion
    // failure handlers, though not returning (thus escalating review behavior
    // implicitly to the level of asserts) is acceptable.
    //
    // Finally, this class defines the constant strings that are used as the
    // 'reviewLevel' in 'ReviewViolation's associated with failed 'BSLS_REVIEW'
    // invocations.

  public:
    // TYPES
    typedef bsls::AtomicOperations::AtomicTypes::Int Count;
        // 'Count' is an alias for an atomic integer.  All 'bsls_review' macros
        // declare a static local 'Count' variable that is used to track how
        // many times that review has failed.  This count gets updated through
        // the 'bsls::Review::updateCount' function.

    typedef void (*ViolationHandler)(const ReviewViolation&);
        // 'ViolationHandler' is an alias for a pointer to a function returning
        // 'void', and taking, as a parameter, a 'const' reference to a
        // 'ReviewViolation' instance.  For example:
        //..
        //  void myHandler(const ReviewViolation& violation);
        //..

  private:
    // FRIENDS
    friend class ReviewFailureHandlerGuard;

    // PRIVATE CLASS METHODS
    static void setViolationHandlerRaw(Review::ViolationHandler function);
        // Make the specified handler 'function' the current review-failure
        // handler.  This method has effect regardless of whether the
        // 'lockReviewAdministration' method has been called.

  public:
    // PUBLIC CONSTANTS

                     // 'reviewLevel' Strings

    static const char k_LEVEL_SAFE[];
    static const char k_LEVEL_OPT[];
    static const char k_LEVEL_REVIEW[];
    static const char k_LEVEL_INVOKE[];

    // CLASS METHODS

                      // Administrative Methods

    static void setViolationHandler(Review::ViolationHandler function);
        // Make the specified handler 'function' the current review-failure
        // handler.  This method has no effect if the
        // 'lockReviewAdministration' method has been called.

    static Review::ViolationHandler violationHandler();
        // Return the address of the currently installed review-failure handler
        // function.  Note that the initial value of the review-failure handler
        // is the 'Review::failByLog' method.

    static void lockReviewAdministration();
        // Disable all subsequent calls to 'setViolationHandler'.  Note that
        // this method has no effect on the behavior of a
        // 'ReviewFailureHandlerGuard' object.

                      // Dispatcher Methods (called from within macros)

    static int updateCount(Count *count);
        // Increment the specified 'count' and return the new value.  Instead
        // of overflowing, when the value is sufficiently large, decrement the
        // value so that large values repeat periodically.

    static void invokeHandler(const ReviewViolation& violation);
        // Invoke the currently installed review-failure handler function with
        // the specified 'violation' as its argument.  Note that this function
        // is intended for use by the (BSLS) "REVIEW" macros, but may also be
        // called by clients directly as needed.

#ifdef BSLS_ASSERT_USE_CONTRACTS
    static void invokeLanguageContractHandler(
                                     const std::contract_violation& violation);
        // Call 'invokeHandler' with a 'ReviewViolation' with properties from
        // the specified 'violation', tracking a 'count' of repeated violations
        // statically.
#endif

                      // Standard Review-Failure Handlers

    static void failByLog(const ReviewViolation& violation);
        // Log a message to 'stdout' that an assertion has failed with
        // information on the failure from the specified 'violation'.  A
        // suitably formatted "cheap stack" is included in the log message that
        // identifies the call site where the failure occurred.

    static void failByAbort(const ReviewViolation& violation);
        // Emulate the invocation of the standard 'assert' macro with a 'false'
        // argument, using the specified 'violation' to generate an output
        // message and then, after logging, unconditionally abort.

    static void failBySleep(const ReviewViolation& violation);
        // Use the specified 'violation' to generate an output message and
        // then, after logging, spin in an infinite loop.  Note that this
        // handler function is useful for hanging a process so that a debugger
        // may be attached to it.

    static void failByThrow(const ReviewViolation& violation);
        // Throw an 'AssertTestException' (whose attributes are 'comment',
        // 'filename', and 'lineNumber' from the specified 'violation'),
        // provided that 'BDE_BUILD_TARGET_EXC' is defined; otherwise, log an
        // appropriate message and abort the program (similar to
        // 'failByAbort').
};

                      // ===============================
                      // class ReviewFailureHandlerGuard
                      // ===============================

class ReviewFailureHandlerGuard {
    // An object of this class saves the current review handler and installs
    // the one specified on construction.  On destruction, the original review
    // handler is restored.  Note that two objects of this class cannot be
    // safely used concurrently from two separate threads (but may of course
    // appear sequentially, including in nested blocks and function invocations
    // within a single thread).  Note that the behavior of objects of this
    // class is unaffected by the ('static') 'Review::lockReviewAdministration'
    // method (i.e., the temporary replacement will occur, regardless of
    // whether that method has been invoked).

    // DATA
    Review::ViolationHandler d_original;  // original (restored at destruction)

  private:
    // NOT IMPLEMENTED
    ReviewFailureHandlerGuard(const ReviewFailureHandlerGuard&);
    ReviewFailureHandlerGuard& operator=(const ReviewFailureHandlerGuard&);

  public:
    // CREATORS
    explicit ReviewFailureHandlerGuard(Review::ViolationHandler temporary);
        // Create a guard object that installs the specified 'temporary' review
        // failure handler and automatically restores the original handler on
        // destruction.

    ~ReviewFailureHandlerGuard();
        // Restore the failure handler that was in place when this object was
        // created and destroy this guard.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class ReviewViolation
                           // ---------------------

// CREATORS
BSLS_KEYWORD_CONSTEXPR
inline
ReviewViolation::ReviewViolation(const char *comment,
                                 const char *fileName,
                                 int         lineNumber,
                                 const char *reviewLevel,
                                 int         count)
: d_comment_p((comment == 0) ? "" : comment)
, d_fileName_p((fileName == 0) ? "" : fileName)
, d_lineNumber(lineNumber)
, d_reviewLevel_p((reviewLevel == 0) ? "" : reviewLevel)
, d_count(count)
{
}

// ACCESSORS
inline
const char *ReviewViolation::comment() const
{
    return d_comment_p;
}

inline
int ReviewViolation::count() const
{
    return d_count;
}

inline
const char *ReviewViolation::fileName() const
{
    return d_fileName_p;
}

inline
int ReviewViolation::lineNumber() const
{
    return d_lineNumber;
}

inline
const char *ReviewViolation::reviewLevel() const
{
    return d_reviewLevel_p;
}

}  // close package namespace
}  // close enterprise namespace

#endif // deeper include guard

          // ========================================================
          // UNDEFINE THE LOCALLY-SCOPED IMPLEMENTATION DETAIL MACROS
          // ========================================================

#undef BSLS_REVIEW_NO_REVIEW_MACROS_DEFINED

                 // =========================================
                 // IMPLEMENTATION USING THE C++ PREPROCESSOR
                 // =========================================
//
// At most one of the following review levels may be set during the compilation
// of any component that includes 'bsls_review.h':
//..
//  BSLS_REVIEW_LEVEL_REVIEW_SAFE
//  BSLS_REVIEW_LEVEL_REVIEW
//  BSLS_REVIEW_LEVEL_REVIEW_OPT
//  BSLS_REVIEW_LEVEL_NONE
//..
// ----------------------------------------------------------------------------

#if defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE) &&                                 \
    defined(BSLS_REVIEW_LEVEL_REVIEW)
#error incompatible BSLS_REVIEW levels:                                       \
..._LEVEL_REVIEW_SAFE and ..._LEVEL_REVIEW
#endif

#if defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE) &&                                 \
    defined(BSLS_REVIEW_LEVEL_REVIEW_OPT)
#error incompatible BSLS_REVIEW levels:                                       \
..._LEVEL_REVIEW_SAFE and ..._LEVEL_REVIEW_OPT
#endif

#if defined(BSLS_REVIEW_LEVEL_REVIEW_SAFE) &&                                 \
    defined(BSLS_REVIEW_LEVEL_NONE)
#error incompatible BSLS_REVIEW levels:                                       \
..._LEVEL_REVIEW_SAFE and ..._LEVEL_NONE
#endif

#if defined(BSLS_REVIEW_LEVEL_REVIEW) &&                                      \
    defined(BSLS_REVIEW_LEVEL_REVIEW_OPT)
#error incompatible BSLS_REVIEW levels:                                       \
..._LEVEL_REVIEW and ..._LEVEL_REVIEW_OPT
#endif

#if defined(BSLS_REVIEW_LEVEL_REVIEW) &&                                      \
    defined(BSLS_REVIEW_LEVEL_NONE)
#error incompatible BSLS_REVIEW levels:                                       \
..._LEVEL_REVIEW and ..._LEVEL_NONE
#endif

#if defined(BSLS_REVIEW_LEVEL_REVIEW_OPT) &&                                  \
    defined(BSLS_REVIEW_LEVEL_NONE)
#error incompatible BSLS_REVIEW levels:                                       \
..._LEVEL_REVIEW_OPT and ..._LEVEL_NONE
#endif

// At most one of the following assert levels may be set during the compilation
// of any component that includes 'bsls_review.h' (this is redundant with
// checks in 'bsls_assert.h', but we want to be sure these are checked even for
// users including only this header):
//..
//  BSLS_ASSERT_LEVEL_ASSERT_SAFE
//  BSLS_ASSERT_LEVEL_ASSERT
//  BSLS_ASSERT_LEVEL_ASSERT_OPT
//  BSLS_ASSERT_LEVEL_NONE
//  BSLS_ASSERT_LEVEL_ASSUME_SAFE
//  BSLS_ASSERT_LEVEL_ASSUME_ASSERT
//  BSLS_ASSERT_LEVEL_ASSUME_OPT
//..
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
