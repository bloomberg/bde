// bsls_deprecate.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLS_DEPRECATE
#define INCLUDED_BSLS_DEPRECATE

//@PURPOSE: Provide machinery to deprecate interfaces on a per-version basis.
//
//@MACROS:
//  BSLS_DEPRECATE:           tag an interface as deprecated
//  BSLS_DEPRECATE_IS_ACTIVE: conditionally activate deprecation by UOR version
//  BSLS_DEPRECATE_MAKE_VER:  render UOR version for deprecation threshold
//
//@DESCRIPTION: This component defines a suite of macros to control (on a
// per-version, per-UOR basis) the deprecation of functions, user-defined
// types, and 'typedef's, and the conditional compilation of enumerators and
// preprocessor macros.  The 'bsls_deprecate' facility operates by triggering
// compiler warnings when types or interfaces are used that have been tagged
// with deprecation macros defined by this component.  Unlike previous
// deprecation facilities based exclusively on the use of '#ifndef' with global
// macros (such as 'BDE_OMIT_DEPRECATED' and 'BDE_OMIT_INTERNAL_DEPRECATED'),
// supported use of the 'bsls_deprecate' facility does *not* affect a UOR's
// ABI.  It is therefore safe to link applications based on libraries built
// with different deprecation policies.
//
///Overview: Common Uses
///---------------------
//
///Applying a Deprecation Tag to an Interface
/// - - - - - - - - - - - - - - - - - - - - -
// UOR owners who wish to mark an interface as deprecated can do so by tagging
// the declaration of that interface with the 'BSLS_DEPRECATE' macro, wrapped
// in a '#if' block to apply 'BSLS_DEPRECATE' only when
// 'BSLS_DEPRECATE_IS_ACTIVE(UOR, M, N)' evaluates to true for a given version
// 'M.N' of the specified 'UOR':
//..
//  #if BSLS_DEPRECATE_IS_ACTIVE(BDE, 3, 2)
//  BSLS_DEPRECATE
//  #endif
//  int foo(const char *bar);
//      // !DEPRECATED!: Use 'newFoo' instead.
//..
// The above application of 'BSLS_DEPRECATE_IS_ACTIVE' indicates that 'foo' is
// deprecated starting with 'bde' version 3.2.  Once the deprecation threshold
// for 'bde' advances to version 3.2, code calling 'foo' will generate a
// deprecation warning with compilers that support deprecation attributes.
// (See {Version Control Macros for Library Authors} for information on
// defining a deprecation threshold for a UOR.)  Note that in the absence of an
// explicit deprecation threshold ('BDE_VERSION_DEPRECATION_THRESHOLD', in this
// case), code calling 'foo' would begin generating deprecation warnings in the
// very next minor or major release of 'bde' (3.3 or 4.0, whichever applies).
//
// If an interface has several entities being deprecated at the same time,
// clients can define a new deprecation macro within that header to avoid
// repeated use of 'BSLS_DEPRECATE_IS_ACTIVE':
//..
//  // bdexyz_component.h                                             -*-C++-*-
//
//  #if BSLS_DEPRECATE_IS_ACTIVE(BDE, 3, 2)
//  #define BDEXYZ_COMPONENT_DEPRECATED_3_2 BSLS_DEPRECATE
//  #else
//  #define BDEXYZ_COMPONENT_DEPRECATED_3_2
//  #endif
//
//  // ...
//
//  BDEXYZ_COMPONENT_DEPRECATED_3_2
//  int foo();
//
//  BDEXYZ_COMPONENT_DEPRECATED_3_2
//  int bar();
//
//  // ...
//
//  #undef BDEXYZ_COMPONENT_DEPRECATED_3_2
//..
//
///Keeping Your Code Free of Calls to Deprecated Interfaces
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When an interface is tagged with 'BSLS_DEPRECATE' as shown above, the
// deprecation is initially _not_ _enforced_ by default.  That is, a normal
// build of code calling the deprecated interface will not emit a deprecation
// warning.
//
// Downstream developers who wish to make sure that their code uses no
// deprecated interfaces can do so by defining the symbol
// 'BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY' in their build system.
//..
//  $ CXXFLAGS=-DBB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY make my_application
//  # A compiler that supports 'BSLS_DEPRECATE' will emit a warning if any
//  # deprecated interfaces are used in 'my_application', even if those
//  # deprecations are scheduled to take effect in a future release.
//..
// *NEVER* define 'BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY' in a
// *PRODUCTION* *BUILD* *CONFIGURATION*.  If you do so, all libraries that you
// depend on will be prevented from deprecating more code in future versions.
//
///Preventing New Uses of Already-Deprecated Interfaces
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// At some point after an interface has been tagged with 'BSLS_DEPRECATE', the
// library owner can make new uses of that interface generate warnings by
// defining a deprecation threshold for the UOR that contains the deprecated
// interface (or by adjusting the deprecation threshold for the UOR if it
// already exists).  Defining a deprecation threshold _enforces_ deprecations
// made in all versions up to and including the threshold.  If the version
// number of the deprecation threshold is greater than or equal to the version
// number specified in the 'BSLS_DEPRECATE_IS_ACTIVE(UOR, M, N)' macro, then
// the 'BSLS_DEPRECATE' macro will be enabled and generate a warning.
//
// This is the recommended way to define a deprecation threshold (see {Version
// Control Macros for Library Authors}):
//..
//  // bdescm_versiontag.h
//
//  // ...
//
//  #define BDE_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(3, 2)
//
//  // All 'bde' deprecations tied to release 3.2 or earlier will trigger
//  // compiler warnings.
//..
//
///Background
///----------
// Prior to the availability of this component, when a developer wanted to
// deprecate an API they might either apply an attribute to the API that would
// generate a warning, or they would use '#ifdef' to remove deprecated code
// when it is built with appropriate options.  These solutions have a practical
// shortcoming in a production environment like Bloomberg's, where code changes
// in lower-level libraries cannot be checked in if they break the build of
// (higher-level) client libraries.  In such a system, well-meaning clients
// might build their libraries using '-Werror' (to turn compilation warnings
// into errors) or with appropriate '#ifdef's to ensure their code does not use
// deprecated APIs, but in so doing they hinder the introduction of any new
// deprecations.  In addition, the use of '#ifdef' results in ABI compatibility
// issues, as some clients may build with deprecated code removed, and others
// may not.
//
// This deprecation facility is based around two concepts that attempt to
// address these shortcomings:
//
//: 1 This facility is designed to provide ABI compatibility.
//:   'BSLS_DEPRECATE_*' macros are used to trigger *compilation* *warnings*
//:   on platforms that support deprecation attributes, instead of *removing*
//:   *code* from the codebase.
//:
//: 2 Typically, use of this facility will not immediately generate a warning
//:   in client code.  The use of the 'BSLS_DEPRECATE' macro is generally
//:   guarded by a version check using 'BSLS_DEPRECATE_IS_ACTIVE'.  In an
//:   environment where compiler warnings are considered to be build failures,
//:   it is possible (and encouraged) to tag a C++ entity with a deprecation
//:   macro in one release cycle, and not have that deprecation affect any
//:   clients by default until a later release cycle.  During the intervening
//:   period, clients have an opportunity to proactively check their code for
//:   uses of newly-deprecated code.
//
// Notice that the cost for maintaining ABI compatibility is that clients
// cannot check whether they are using deprecated interfaces unless they build
// their software on certain platforms with warnings activated.  For this
// facility to be effective across an enterprise, it is required that such
// warning-enabled builds be part of the standard process for checking in code
// in the enterprise.
//
///Mechanics
///---------
// This component stipulates two sets of macros.  One set of deprecation
// macros, defined in this component, are used to identify a C++ entity as
// being deprecated in a given version of a given UOR.  A second set of control
// macros, defined by clients of this component, dictates which deprecation
// macros are enforced at any point in the code during compilation.
//
///Deprecation Macros
/// - - - - - - - - -
//: 'BSLS_DEPRECATE':
//:   Expands to a particular deprecation attribute with compilers that have
//:   such support; otherwise, 'BSLS_DEPRECATE' expands to nothing.
//:   'BSLS_DEPRECATE' can be applied to 'class' or 'struct' definitions,
//:   function declarations, and 'typedef's.
//:
//: 'BSLS_DEPRECATE_IS_ACTIVE(UOR, M, N)':
//:   Expands to 1 if deprecations are enforced for the specified version 'M.N'
//:   of the specified 'UOR', and to 0 otherwise.
//
// These two macros are intended to be placed together in a preprocessor '#if'
// block in front of a function, type, or 'typedef' declaration, with
// 'BSLS_DEPRECATE_IS_ACTIVE(UOR, M, N)' controlling whether or not
// 'BSLS_DEPRECATE' is applied to the declaration.  The exact placement of the
// block should match the requirements of the C++14 '[[deprecated]]' attribute.
//
// Examples:
//..
//  class
//  #if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
//  BSLS_DEPRECATE
//  #endif
//  SomeType {
//      // ...
//  };
//
//  struct SomeUtil {
//    #if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
//    BSLS_DEPRECATE
//    #endif
//    static int someFunction();
//  };
//
//  #if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
//  BSLS_DEPRECATE
//  #endif
//  typedef SupportedType DeprecatedType;
//..
// At present, the underlying compiler intrinsics represented by
// 'BSLS_DEPRECATE' cannot be applied uniformly to some C++ constructs, most
// notably variables, enumerators, and preprocessor macros.  Fortunately, these
// constructs can often be removed from a library without otherwise affecting
// ABI, so '!BSLS_DEPRECATE_IS_ACTIVE(UOR, M, N)' can be used with a '#if'
// directive to entirely remove blocks of code containing those C++ constructs.
//
// Example:
//..
//  #if !BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
//
//  #define MY_DEPRECATED_MACRO
//      // Macro definitions can be removed with 'BSLS_DEPRECATE_IS_ACTIVE'.
//
//  #endif  // !BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
//
//  namespace grppkg {
//
//  #if !BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 3)
//
//  SomeType myDeprecatedGlobalVariable;
//      // Variables at 'namespace' or global scope can be removed with
//      // 'BSLS_DEPRECATE_IS_ACTIVE'.
//
//  #endif  // !BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 3)
//
//  // ...
//
//  }  // close namespace 'grppkg'
//..
// Note the use of the '!' operator: deprecated code is compiled only if
// deprecations are *not* enforced for the specified UOR version.
//
// Particular care must be taken to ensure that deprecating one or more
// enumerators does not (inadvertently) change the values of other enumerators
// in the same 'enum':
//..
//  enum MyEnum {
//      e_FIRST,
//      e_SECOND,
//      e_THIRD,
//  #if !BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
//      // These legacy enumerators can be deprecated because their removal
//      // does not affect the values of any other enumerators.
//
//      FIRST  = e_FIRST,
//      SECOND = e_SECOND,
//      THIRD  = e_THIRD
//  #endif
//  };
//..
//
///Version Control Macros for Library Authors
/// - - - - - - - - - - - - - - - - - - - - -
// A UOR-specific deprecation threshold can be (and typically *should* be)
// specified by the authors of a UOR to govern which of their deprecations are
// active by default:
//: '<UOR>_VERSION_DEPRECATION_THRESHOLD':
//:   This macro should be defined in '<uor>scm_versiontag.h' alongside
//:   '<UOR>_VERSION_MAJOR' and '<UOR>_VERSION_MINOR' to indicate the greatest
//:   version of the unit of release 'UOR' for which deprecations are enforced
//:   by default.
//
// Example:
//..
//  // abcscm_versiontag.h
//
//  #define ABC_VERSION_MAJOR 1
//  #define ABC_VERSION_MINOR 4
//
//  #define ABC_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(1, 2)
//..
// In this example, 'BSLS_DEPRECATE_IS_ACTIVE(ABC, M, N)' will expand to 1 for
// all versions 'M.N' of 'ABC' up to and including version 1.2.  For 'M.N'
// later than 1.2 (e.g., 1.3 or 2.0), 'BSLS_DEPRECATE_IS_ACTIVE(ABC, M, N)'
// will expand to 1 only if the 'BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY'
// macro is defined by the user (see {Build Control Macros for Clients}).
//
// Note that if a deprecation threshold is *not* explicitly defined for a UOR
// that defines '<UOR>_VERSION_MAJOR' and '<UOR>_VERSION_MINOR', then
// 'BSLS_DEPRECATE_IS_ACTIVE(UOR, M, N)' will expand to 1 once the version
// indicated by '<UOR>_VERSION_MAJOR' and '<UOR>_VERSION_MINOR' becomes greater
// than 'M.N'.  For example, 'BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 4)' will expand
// to 1 in version 1.5 of 'ABC' (or 2.0 if there is no release 1.5) if
// 'ABC_VERSION_DEPRECATION_THRESHOLD' is not defined.  For this reason, it is
// highly recommended that UOR authors explicitly define a deprecation
// threshold to avoid unexpected build failures when a new release is issued,
// especially in environments where warnings are considered fatal.
//
// A second UOR-specific macro is available to the authors of a UOR that must,
// for whatever reason, continue to use interfaces that are deprecated in their
// own library:
//: 'BB_SILENCE_DEPRECATIONS_FOR_BUILDING_UOR_<UOR>':
//:   This macro prevents 'bsls_deprecate' from enforcing deprecations for all
//:   versions of 'UOR'.  This macro must be defined in each '.cpp' file of
//:   'UOR' that either uses a deprecated interface from the *same* UOR that
//:   has reached the deprecation threshold for 'UOR', or includes a header
//:   file of 'UOR' that uses such an interface in inline code.  This macro
//:   must be defined before the first '#include' of a header from 'UOR'.
//
// Example:
//..
//  // abcxyz_somecomponent.cpp
//
//  #define BB_SILENCE_DEPRECATIONS_FOR_BUILDING_UOR_ABC
//
//  #include <abcxyz_somecomponent.h>
//
//  // ...
//..
//
///Build Control Macros for Clients
/// - - - - - - - - - - - - - - - -
// The following two macros are intended for client use during builds, either
// to *enable* *all* deprecations or to *suppress* *selected* deprecations:
//: 'BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY':
//:   This macro should be defined as a '-D' parameter during test builds of
//:   components that are intended to be deprecation-clean.  When this macro is
//:   defined, deprecations will be enforced for all versions of all UORs,
//:   except as overridden by 'BB_SILENCE_DEPRECATIONS_FOR_BUILDING_UOR_<UOR>'
//:   (see {Version Control Macros for Library Authors} or
//:   'BB_SILENCE_DEPRECATIONS_<UOR>_<M>_<N>' (see below).  This macro must
//:   *never* appear in source code, and must *never* be defined for any
//:   production or check-in build configuration.
//:
//: 'BB_SILENCE_DEPRECATIONS_<UOR>_<M>_<N>':
//:   This macro should be defined by clients of 'UOR' who still need to use an
//:   interface that was deprecated in version 'M.N' after the deprecation
//:   threshold for 'UOR' has reached (or exceeded) 'M.N'.  This macro should
//:   be defined *before* the first '#include' of a header from 'UOR'.  This
//:   macro must *never* be defined in a header file.
//
// Example:
//..
//  // grppkg_fooutil.cpp
//
//  #define BB_SILENCE_DEPRECATIONS_ABC_1_2
//      // 'BB_SILENCE_DEPRECATIONS_ABC_1_2' must be defined before the
//      // component's own '#include' directive in case 'grppkg_fooutil.h'
//      // includes headers from 'abc' (directly or transitively).
//
//  #include <grppkg_fooutil.h>
//
//  // Interfaces from 'abcxyz_someutil' deprecated in version 1.2 of 'abc'
//  // will not trigger compiler warnings when used in 'grppkg_fooutil.cpp'.
//
//  #include <abcxyz_someutil.h>
//  ...
//
//  namespace grppkg {
//
//  void FooUtil::foo()
//  {
//      int result = abcxyz::SomeUtil::someFunction();
//      // ...
//  }
//
//  }  // close package namespace
//..
//
///Supporting Compilers
///--------------------
// 'BSLS_DEPRECATE' will produce a warning with the following compilers:
//: o gcc 4.3+
//: o clang 3.4+
//: o Xcode 4.4+
//: o Microsoft Visual Studio 2010 or later
//
// Additionally, 'BSLS_DEPRECATE' will produce a warning with any compiler that
// provides the C++14 'deprecated' attribute.
//
///Suggested Process for Deprecating Code (the Deprecation/Deletion Tango)
///-----------------------------------------------------------------------
// Deprecation is a negotiation process between code authors and code consumers
// to allow old code to be removed from the codebase.  This component supports
// a deprecation model where code moves through four steps from being fully
// supported, to optionally deprecated, to fully deprecated, and finally to
// being deleted.  At each step, responsibility for moving the process forward
// is passed back and forth between library authors and library users.
//
// When the owners of a library want to deprecate an interface, they start by
// adding appropriate deprecation macros, specifying their UOR and the version
// of their next release.  For example, suppose package group 'abc' is
// currently at version 1.1.  If the owners of 'abc' want to deprecate a
// function 'abcxyz::SomeUtil::someFunction', they could add the deprecation
// macros 'BSLS_DEPRECATE' and 'BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)' in front
// of the declaration of 'someFunction':
//..
//  // abcxyz_someutil.h
//
//  ...
//
//  struct SomeUtil {
//
//  ...
//
//  #if BSLS_DEPRECATE_IS_ACTIVE(ABC, 1, 2)
//  BSLS_DEPRECATE
//  #endif
//  static int someFunction();
//      // DEPRECATED: use 'abcdef::OtherUtil::otherFunction' instead.
//
//  ...
//
//  };
//..
// At this point deprecations are not enforced by default for version 1.2 of
// 'abc', so the deprecation macro alone will not have any affect on clients.
// A client building their code normally will trigger no compiler warnings due
// to this new deprecation:
//..
//  $ make grppkg_fooutil
//  ... dependencies: abc version 1.2 ...
//  ... no warnings ...
//..
// If the owners of a client library or application want to check that their
// code uses no deprecated interfaces, they can define the
// 'BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY' flag in their *test* build
// process.  A compiler that supports deprecation attributes will then trigger
// compiler warnings:
//..
//  $ CXXFLAGS=-DBB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY make grppkg_fooutil
//  ... dependencies: abc version 1.2 ...
//  grppkg_fooutil.cpp:43: warning: function 'abcxyz::SomeUtil::someFunction'
//  is explicitly deprecated.
//..
// !WARNING!: Clients at Bloomberg *must* *not* define
// 'BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY' in a production build.  This
// flag should be used for development builds only.
//
// Now the owners have the opportunity to fix their code by removing the
// dependency on 'abcxyz::SomeUtil::someFunction'.
//
// At some point in the future, possibly on release of 'abc' version 1.3, the
// owners of 'abc' will make deprecations enforced by default for version 1.2
// of 'abc', by setting the deprecation threshold for 'abc' in their version
// control headers:
//..
//  // abcscm_versiontag.h
//
//  #define ABC_VERSION_MAJOR 1
//  #define ABC_VERSION_MINOR 3
//
//  ...
//
//  #define ABC_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(1, 2)
//..
// If the owners of 'grp' have cleaned up their code, normal builds will
// trigger no compiler warnings.  However, any new development by any clients
// will trigger warnings if they add a new use of
// 'abcxyz::SomeUtil::someFunction':
//..
//  $ make foobar_bazutil
//  ... dependencies: abc version 1.3 ...
//  foobar_bazutil.cpp:177: warning: function 'abcxyz::SomeUtil::someFunction'
//  is explicitly deprecated.
//..
// But what if the owners of 'grp' will not, or for some reason cannot, clean
// up their code in the near term?  Moving the threshold for 'abc' to version
// 1.2 will of course trigger warnings when the owners of 'grp' next try to
// build 'grppkg_fooutil'.  In a development context requiring that all
// production builds remain warning-free, we would be at an impasse: either the
// owners of 'grp' must clean up their code immediately, or the owners of 'abc'
// will not be able to enforce deprecations by default for version 1.2.
//
// This impasse can be resolved by allowing the owners of 'grp' to locally
// silence warnings caused by deprecations for version 1.2 of 'abc'.  This is
// done by adding a definition of 'BB_SILENCE_DEPRECATIONS_ABC_1_2' to any
// '.cpp' files that use 'abcxyz::SomeUtil::someFunction':
//..
//  // grppkg_fooutil.cpp
//  #define BB_SILENCE_DEPRECATIONS_ABC_1_2
//
//  ...
//..
// Now the entire codebase can build warning-free again.
//
// Managers can easily detect 'BB_SILENCE_DEPRECATIONS_<UOR>_<M>_<N>' macros in
// the codebase, and put pressure on teams to remove such remnant uses of
// deprecated code.  When all remnant uses have been removed, then the
// deprecated function can be deleted entirely:
//..
//  // abcxyz_someutil.h
//
//  ...
//
//  struct SomeUtil {
//
//  ...
//
//  // RIP: The function formerly known as 'someFunction'.
//
//  ...
//
//  };
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Tagging a Function as Deprecated
///- - - - - - - - - - - - - - - - - - - - - -
// When one piece of code has been superseded by another, we would like to get
// users to adopt the new code and stop using the old code.  Being able to
// inform clients that they need to clean up existing uses of the old code, and
// also to prevent *new* uses of that code, makes it easier to get to the point
// where old code actually has zero uses and can be deleted.  The deprecation
// macros 'BSLS_DEPRECATE' and 'BSLS_DEPRECATE_IS_ACTIVE', and their associated
// control macros, can be used to gradually reduce the number of uses of
// deprecated code, so that it can be removed eventually.
//
// Suppose we own package group 'xxx' that is currently at version 7.6.  One of
// our components contains a function 'foo' that has been superseded by another
// function 'bar'.
//..
//  int foo(int *coefficient, int n);
//      // Load into the specified 'coefficient' the (positive) Winkelbaum
//      // Coefficient of the specified 'n'.  Return 0 on success, and a
//      // negative number if there is no coefficient corresponding to 'n'.
//      // Note that every integer divisible by the Winkelbaum Modulus (17) has
//      // a corresponding Winkelbaum Coefficient.
//
//  // ...
//
//  int bar(int n);
//      // Return the (positive) Winkelbaum Coefficient of the specified 'n'.
//      // The behavior is undefined unless 'n' is divisible by 17 (the
//      // Winkelbaum Modulus).
//..
// First, we add a deprecation tag to the declaration of 'foo', showing that it
// will be deprecated starting with version 7.7, and update the documentation
// accordingly:
//..
//  #if BSLS_DEPRECATE_IS_ACTIVE(XXX, 7, 7)
//  BSLS_DEPRECATE
//  #endif
//  int foo(int *coefficient, int n);
//      // !DEPRECATED!: Use 'bar' instead.
//      //
//      // Load into the specified 'coefficient' the (positive) Winkelbaum
//      // Coefficient of the specified 'n'.  Return 0 on success, and a
//      // negative number if there is no coefficient corresponding to 'n'.
//      // Note that every integer divisible by the Winkelbaum Modulus (17) has
//      // a corresponding Winkelbaum Coefficient.
//
//  // ...
//
//  int bar(int n);
//      // Return the (positive) Winkelbaum Coefficient of the specified 'n'.
//      // The behavior is undefined unless 'n' is divisible by 17 (the
//      // Winkelbaum Modulus).
//..
// When we release version 7.7, the added deprecation tag will not immediately
// affect any of the users of 'foo'.  However if any of those users do a test
// build of their code with '-DBB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY', they
// will see a warning that 'foo' has been deprecated.
//
// Finally, when enough time has passed to allow all users of 'foo' to switch
// over to using 'bar', probably on or after the release of 'xxx' version 7.8,
// we can enforce the deprecation of 'foo' by moving the deprecation threshold
// for 'xxx' to version 7.7, to indicate that all interfaces deprecated for
// version 7.7 are disallowed by default:
//..
//  // xxxscm_versiontag.h
//
//  #define XXX_VERSION_MAJOR 7
//  #define XXX_VERSION_MINOR 8
//
//  // ...
//
//  #define XXX_VERSION_DEPRECATION_THRESHOLD BSLS_DEPRECATE_MAKE_VER(7, 7)
//..

                               // ==============
                               // BSLS_DEPRECATE
                               // ==============

// First, try to determine if the C++14 'deprecated' attribute is supported,
// using standard feature-detection facilities.

#if defined(__has_cpp_attribute)
#  if __has_cpp_attribute(deprecated)
#    define BSLS_DEPRECATE [[deprecated]]

#    if defined(__clang__) && __cplusplus < 201402L
//     Clang issues a warning when the C++14 attribute is used before C++14,
//     where it is offered as an extension rather than a feature.
#      undef BSLS_DEPRECATE
#    endif

#    if defined(__GNUC__) && __GNUC__ <= 5 && __cplusplus < 201103L
//     G++ version 5 passes the '__has_cpp_attribute(deprecated)' test, but
//     still produces an error when '[[deprecated]]' is used in non-C++11 mode.
#      undef BSLS_DEPRECATE
#    endif

#  endif
#endif

// Then, try to determine if a GCC-style deprecation attribute is supported,
// using semi-standard feature-detection facilities.

#ifndef BSLS_DEPRECATE
#  if defined(__has_attribute)
#    if __has_attribute(deprecated)
#      define BSLS_DEPRECATE __attribute__ ((deprecated))
#    endif
#  endif
#endif

// Next, define non-standard attributes for platforms known to support them.

#ifndef BSLS_DEPRECATE
#  if defined(__GNUC__)
//   All compilers in the GNUC interface family (g++ and clang) provide the
//   simple gcc extension attribute.
#    define BSLS_DEPRECATE __attribute__ ((deprecated))
#  elif defined (_MSC_VER)
//   MSVC supports a 'deprecated' declaration specifier starting with Visual
//   Studio 2010.
#    if 1600 <= _MSC_VER
#      define BSLS_DEPRECATE __declspec(deprecated)
#    endif
#  endif
#endif

// Finally, record whether the current compiler supports some form of
// deprecation attribute.

#ifdef BSLS_DEPRECATE
#  define BSLS_DEPRECATE_COMPILER_SUPPORT 1
#else
#  define BSLS_DEPRECATE_COMPILER_SUPPORT 0
// Provide a fallback empty definition when the compiler does not support
// deprecation attributes.
#  define BSLS_DEPRECATE
#endif

// ============================================================================
//                            INTERNAL MACHINERY
// ============================================================================

                             // ==================
                             // BSLS_DEPRECATE_CAT
                             // ==================

#define BSLS_DEPRECATE_CAT(X, Y)   BSLS_DEPRECATE_CAT_A(X, Y)
    // Expand to the expansion of the specified 'X', joined to the expansion of
    // the specified 'Y'.

#define BSLS_DEPRECATE_CAT_A(X, Y) BSLS_DEPRECATE_CAT_B(X, Y)
#define BSLS_DEPRECATE_CAT_B(X, Y) X ## Y
    // Internal implementation machinery for 'BSLS_DEPRECATE_CAT'.

                          // ========================
                          // BSLS_DEPRECATE_ISDEFINED
                          // ========================

#define BSLS_DEPRECATE_ISDEFINED(...) BSLS_DEPRECATE_ISDEFINED_A(__VA_ARGS__)
    // Expand to an expression evaluating to 'true' in a preprocessor context
    // if the deprecation control macro symbol supplied as an argument has been
    // '#define'd as nil, 0, or 1, and expand to an expression evaluating to
    // 'false' otherwise.  The behavior is undefined unless this macro is
    // evaluated with a single argument having the form of a deprecation
    // control macro symbol, and that symbol has either not been '#define'd at
    // all or has been '#define'd as nil, 0, or 1.

#define BSLS_DEPRECATE_ISDEFINED_A(...) ((__VA_ARGS__ ## 1L) != 0)
    // Internal implementation machinery for 'BSLS_DEPRECATE_ISDEFINED'.

                          // ========================
                          // BSLS_DEPRECATE_ISNONZERO
                          // ========================

#define BSLS_DEPRECATE_ISNONZERO(...) BSLS_DEPRECATE_ISNONZERO_A(__VA_ARGS__)
    // Expand to an expression evaluating to 'true' in a preprocessor context
    // if the deprecation control macro symbol supplied as an argument has been
    // '#define'd as an expression that evaluates to a non-zero value, and
    // expand to an expression evaluating to 'false' otherwise.  The behavior
    // is undefined unless this macro is evaluated with a single argument
    // having the form of a deprecation control macro symbol, and that symbol
    // has either not been '#define'd at all, or has been '#define'd as nil or
    // an arithmetic expression.

#define BSLS_DEPRECATE_ISNONZERO_A(...) (__VA_ARGS__ + 1 != 1)
    // Internal implementation machinery for 'BSLS_DEPRECATE_ISNONZERO'.

                          // =======================
                          // BSLS_DEPRECATE_MAKE_VER
                          // =======================

#define BSLS_DEPRECATE_MAKE_VER(M, N) ((M) * 1000 + (N))
    // Expand to an opaque sequence of symbols encoding a UOR version where
    // the specified 'M' is the major version number and the specified 'N' is
    // the minor version number.

                       // ==============================
                       // BSLS_DEPRECATE_ISPASTTHRESHOLD
                       // ==============================

#define BSLS_DEPRECATE_ISPASTTHRESHOLD(U, M, N)                               \
    (   (   BSLS_DEPRECATE_ISNONZERO(BSLS_DEPRECATE_ISPASTTHRESHOLD_A(U))     \
         && BSLS_DEPRECATE_MAKE_VER(M, N)                                     \
                <= BSLS_DEPRECATE_ISPASTTHRESHOLD_A(U))                       \
     || (  !BSLS_DEPRECATE_ISNONZERO(BSLS_DEPRECATE_ISPASTTHRESHOLD_A(U))     \
         && BSLS_DEPRECATE_MAKE_VER(M, N)                                     \
                <= BSLS_DEPRECATE_MAKE_VER(                                   \
                       BSLS_DEPRECATE_CAT(U, _VERSION_MAJOR),                 \
                       BSLS_DEPRECATE_CAT(U, _VERSION_MINOR)) - 1))
    // Expand to an expression evaluating to 'true' in a preprocessor context
    // if the specified version 'M.N' of the specified UOR 'U' is past the
    // deprecation threshold for 'U', and expand to an expression evaluating to
    // 'false' otherwise.  Version 'M.N' of 'U' is past the deprecation
    // threshold for 'U' if:
    //: o An explicit deprecation threshold has been defined for UOR 'U' and
    //:   that threshold is greater than or equal to version 'M.N', or
    //:
    //: o An explicit deprecation threshold has *not* been defined for UOR 'U',
    //:   the versioning package for UOR 'U' defines '<U>_VERSION_MAJOR' and
    //:   '<U>_VERSION_MINOR', and the version so designated is greater than
    //:   version 'M.N'.

#define BSLS_DEPRECATE_ISPASTTHRESHOLD_A(U)                                   \
    BSLS_DEPRECATE_CAT(U, _VERSION_DEPRECATION_THRESHOLD)
    // Internal implementation machinery for 'BSLS_DEPRECATE_ISPASTTHRESHOLD'.

                         // =========================
                         // BSLS_DEPRECATE_ISRETAINED
                         // =========================

#define BSLS_DEPRECATE_ISRETAINED(U, M, N) BSLS_DEPRECATE_ISRETAINED_A(U, M, N)
    // Expand to an expression evaluating to 'true' in a preprocessor context
    // if deprecations in the specified version 'M.N' of the specified UOR 'U'
    // have been deactivated in this build by defining either the corresponding
    // 'BB_SILENCE_DEPRECATIONS_<U>_<M>_<N>' macro or the corresponding
    // 'BB_SILENCE_DEPRECATIONS_FOR_BUILDING_UOR_<U>' macro, and expand to an
    // expression evaluating to 'false' otherwise.

#define BSLS_DEPRECATE_ISRETAINED_A(U, M, N)                                  \
    BSLS_DEPRECATE_ISRETAINED_B(U, M, N)
#define BSLS_DEPRECATE_ISRETAINED_B(U, M, N)                                  \
    (BSLS_DEPRECATE_ISDEFINED(BB_SILENCE_DEPRECATIONS_## U ##_## M ##_## N)   \
  || BSLS_DEPRECATE_ISDEFINED(BB_SILENCE_DEPRECATIONS_FOR_BUILDING_UOR_ ## U))
    // Internal implementation machinery for 'BSLS_DEPRECATE_ISRETAINED'.

// ============================================================================
//                               PUBLIC MACROS
// ============================================================================

                          // ========================
                          // BSLS_DEPRECATE_IS_ACTIVE
                          // ========================

#define BSLS_DEPRECATE_IS_ACTIVE(U, M, N)                                     \
    (  !BSLS_DEPRECATE_ISRETAINED(U, M, N)                                    \
     && (  BSLS_DEPRECATE_ISDEFINED(BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY)\
        || BSLS_DEPRECATE_ISPASTTHRESHOLD(U, M, N)))
    // Expand to an expression evaluating to 'true' in a preprocessor context
    // if deprecations are being enforced for the specified version 'M.N' of
    // the specified UOR 'U', and expand to an expression evaluating to 'false'
    // otherwise.  Deprecations will be enforced for version 'M.N' of UOR 'U'
    // if:
    //: o 'BB_SILENCE_DEPRECATIONS_<U>_<M>_<N>' has not been defined in this
    //:   translation unit, and
    //:
    //: o 'BB_SILENCE_DEPRECATIONS_FOR_BUILDING_UOR_<U>' has not been defined
    //:   in this translation unit, and
    //:
    //: o One of the following holds true:
    //:   o 'BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY' has been defined, or
    //:
    //:   o An explicit deprecation threshold has been defined for UOR 'U' and
    //:     that threshold is greater than or equal to version 'M.N', or
    //:
    //:   o An explicit deprecation threshold has *not* been defined for UOR
    //:     'U', the versioning package for UOR 'U' defines '<U>_VERSION_MAJOR'
    //:     and '<U>_VERSION_MINOR', and the version so designated is greater
    //:     than version 'M.N'.

#endif  // INCLUDED_BSLS_DEPRECATE

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
