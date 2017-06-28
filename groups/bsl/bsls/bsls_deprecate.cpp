// bsls_deprecate.cpp                                                 -*-C++-*-
#include <bsls_deprecate.h>

// BDE_VERIFY pragma: -SH06  // Levelized below 'bsls_ident'

///Implementation Notes
///--------------------
//
///Extraction of Arguments from '__VA_ARGS__'
/// - - - - - - - - - - - - - - - - - - - - -
// Microsoft Visual Studio preprocessor's handling of variadic arguments
// differs from the handling found in most (all?) other compilers.  To allow
// the macros to evaluate properly on all platforms, a special pattern is
// needed for expanding '__VA_ARGS__' as it is passed to the next macro in a
// macro chain.
//
// Consider the following macro definitions:
//..
//  #define A(...)       B(0, __VA_ARGS__)
//  #define B(X, Y, ...) Y
//..
// With MSVC, the 'Y' argument of 'B' will be the same as the '__VA_ARGS__'
// passed from 'A', joined together as a single unit.  I.e., the expansion of
// 'A' will be simply be the arguments to 'A'.  With other compilers, the 'Y'
// argument of 'B' will be the first argument in '__VA_ARGS__', so the
// expansion of 'A' will be its first argument only.
//
// Therefore, to extract a single argument from a '__VA_ARGS__' on MSVC, we
// have to force re-evaluation of '__VA_ARGS__' outside of an argument list, by
// pasting together a new function-like macro, as follows:
//..
//  #define A(...)       B((__VA_ARGS__))
//  #define B(X)         C X
//  #define C(X, Y, ...) Y
//..
// This pattern, while a little counter-intuitive, works on all known
// platforms.
//
///Internal Macros 'BSLS_DEPRECATE_ISDEFINED' and 'BSLS_DEPRECATE_ISNONZERO'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bsls_deprecate' uses two categories of control macros:
//
//: o numeric control macros that represent version numbers
//:   o '<UOR>_VERSION_DEPRECATION_THRESHOLD'
//:   o '<UOR>_VERSION_MAJOR'
//:   o '<UOR>_VERSION_MINOR'
//: o boolean control macros that turn warnings on or off for one or more
//:   versions:
//:   o BB_WARN_ALL_DEPRECATIONS_FOR_TESTING_ONLY
//:   o BB_SILENCE_DEPRECATION_<UOR>_<M>_<N>
//:   o BB_BUILDING_UOR_<UOR>
//
// In order to make it easy to incorporate 'bsls_deprecate' into existing code,
// none of these macros are required.  The deprecation facility should work
// properly if any or all of them have been left undefined.
//
// Additionally, in the case of the boolean control macros, we want to allow
// the user to define them using the natural idioms, either on the command line
// or in their code.  So the following should be legal:
//..
//  $ g++ ${FLAGS} -DBB_SILENCE_DEPRECATION_ABC_3_1 my_component.cpp
//..
// as should compiling code with this directive:
//..
//  #define BB_SILENCE_DEPRECATION_ABC_3_1
//..
// Now, note that the behavior of '-D...' is not addressed by the standard.
// The symbol so defined could end up expanding to nil (i.e., the same as
// `#define SYMBOL`), or it could end up expanding to either '0' or '1'.
//
// So, if we want to detect whether or not the user has asked to silence
// deprecations for 'abc' version 3.1, we can't just compare
// 'BB_SILENCE_DEPRECATION_ABC_3_1' to some value like 0 or 1.  We need
// something like 'defined(BB_SILENCE_DEPRECATION_ABC_3_1)'.  Unfortunately,
// the preprocessor 'defined(...)' function cannot be used within a macro
// definition.  This is OK:
//..
//  #if defined(BB_BUILDING_UOR_ABC)
//      // ...
//  #else
//      // ...
//  #endif
//..
// but this is not OK:
//..
//  #define IS_BUILDING(U) defined(BB_BUILDING_UOR_ ## U)
//  #if IS_BUILDING(ABC)
//      // ...
//  #else
//      // ...
//  #endif
//..
// The solution is to write our own poor-man's 'defined(SYMBOL)' function.
//
// Earlier drafts of this component tried to approximate the actual behavior of
// 'defined(SYMBOL)', i.e., to distinguish between non-defined and
// defined-to-an-arbitrary-value (including nil).  The resulting macro was
// extremely complex, and still left a few corner cases uncovered.  The logic
// of the component can be radically simplified, though, if we limit the
// allowable states for 'SYMBOL'.  The allowed inputs for the internal Macros
// 'BSLS_DEPRECATE_ISDEFINED(SYMBOL)' and 'BSLS_DEPRECATE_ISNONZERO(SYMBOL)'
// have been limited based on this experience.
//
///Mechanics of 'BSLS_DEPRECATE_ISDEFINED'
///  -  -  -  -  -  -  -  -  -  -  -  -  -
// 'BSLS_DEPRECATE_ISDEFINED(SYMBOL)' can handle:
//: 1 Argument(s) eventually expand to nil.
//: 2 No input, (i.e., no arguments to the macro, a side-effect on some
//:   platforms of 'SYMBOL' expanding to nil).
//: 3 Argument(s) eventually expand to a number (not a numeric expression).
//: 4 Argument(s) eventually expand to a single symbol not recognized by the
//:   pre-processor.  (Such a symbol will evaluate to 0 in a pre-processor
//:   arithmetic context.)
//
// Cases 1-3 above correspond to 'SYMBOL' being defined.  (The no-input case is
// a variant of the expand-to-nil case.)  Case 4 corresponds to 'SYMBOL' being
// undefined.
//
// The macro operates by concatenating '1L' to the end of the expansion of
// 'SYMBOL'.  In each of the cases above, that will result in:
//
//: 1 '1L'.                        This will be non-zero.
//: 2 '1L'.                        This will be non-zero.
//: 3 Some number ending in '1L'.  This will be non-zero.
//: 4 Some symbol ending in '1L'.  This *should* evaluate to zero.
//
// The correct operation of 'BSLS_DEPRECATE_ISDEFINED(SYMBOL)' depends on there
// being no defined preprocessor symbol with the name 'SYMBOL1L'.  Since
// 'BSLS_DEPRECATE_ISDEFINED(SYMBOL)' is used internally, with arguments
// derived from a 'BSLS_DEPRECATE_IS_ACTIVE' macro, we need only worry about
// there being two UORs in the codebase that differ only by the addition of a
// suffix '1l', such as 'abc' and 'abc1l'.  Since 'abc1l' is an illegal UOR
// name, we should be safe.  (Note that any non-zero numeric suffix can be used
// in place of '1L', so the macro can be updated to work with any UOR naming
// convention that places a limit on the longest allowed UOR name.)
//
///Mechanics of 'BSLS_DEPRECATE_ISNONZERO'
///  -  -  -  -  -  -  -  -  -  -  -  -  -
// 'BSLS_DEPRECATE_ISNONZERO(EXPRESSION)' can handle:
//: 1 Argument(s) eventually expand to nil.
//: 2 No input, (i.e., no arguments to the macro, a side-effect on some
//:   platforms of 'SYMBOL' expanding to nil).
//: 3 Argument(s) eventually expand to a numeric expression.
//: 4 Argument(s) eventually expand to a single symbol not recognized by the
//:   pre-processor.  (Such a symbol will evaluate to 0 in a pre-processor
//:   arithmetic context.)
//
// Case 3 above will evaluate to 'true' in a pre-processor context if the
// numeric expression is non-zero, and 'false' otherwise.  The other cases will
// all evaluate to 'false'.
//
// The macro operates by adding '1' to the expanded 'EXPRESSION'.  In each of
// the cases above, that will result in:
//
//: 1 '+ 1'.             This will compare equal to '1'.
//: 2 '+ 1'.             This will compare equal to '1'.
//: 3 'EXPRESSION + 1'.  This will compare equal to '1' only if 'EXPRESSION'
//:                      compares equal to '0'.
//: 4 '0 + 1'.           This will compare equal to '1'.
//
///Use of Deprecated Interfaces in Inline Functions
///------------------------------------------------
// Sometimes, a family of related interfaces in a single component or package
// will be deprecated as a unit, and some of those interfaces may depend on
// other interfaces that are deprecated at the same time.  That dependency
// would normally cause a warning, but must remain in the codebase until the
// entire family of related interfaces are deleted.
//
// For example, consider the following interface:
//..
//  // grppkg_someutility.h
//
//  struct SomeUtility {
//      ConstElementIterator cbegin(const SomeContainer& container);
//          // Return an iterator pointing to the first element in the
//          // specified 'c'.
//
//      // Legacy direct-access API is deprecated.  Use the iterator API
//      // provided by 'cbegin' instead.
//
//      #if BSLS_DEPRECATE_IS_ACTIVE(GRP, 1, 2)
//      BSLS_DEPRECATE
//      #endif
//      Element first(const SomeContainer& container);
//          // Return the 'Element' object at the head of the specified
//          // 'container'.  ...
//
//      #if BSLS_DEPRECATE_IS_ACTIVE(GRP, 1, 2)
//      BSLS_DEPRECATE
//      #endif
//      Element nth(const SomeContainer& container, int index);
//          // Return the 'Element' object in the specified 'index' position in
//          // the specified 'container'. ...
//  };
//..
// 'first' could easily have been implemented in terms of 'nth':
//..
//  // grppkg_someutility.cpp
//  #include <grppkg_someutility.h>
//
//  Element SomeUtility::first(const SomeContainer& container)
//  {
//      return nth(container, 0);
//  }
//
//  Element SomeUtility::nth(const SomeContainer& container, int index)
//  {
//      // Pure magic.
//  }
//..
// The definition of 'nth' will emit a warning once the deprecation threshold
// moves past 'grp' version 1.2.  However, 'grppkg' has a legitimate reason to
// continue to use 'nth' until both it and 'first' are deleted from the
// codebase.
//
// The 'BB_BUILDING_UOR_<u>' control is intended to make it easy to handle this
// situation, by providing an auditable way for a UOR author to suppress
// warnings for internal uses of the UOR's own deprecated interfaces:
//..
//  // grppkg_someutility.cpp
//  #define BB_BUILDING_UOR_GRP
//  #include <grppkg_someutility.h>
//
//  Element SomeUtility::first(const SomeContainer& container)
//  {
//      return nth(container, 0);  // Look Mom, no warning!
//  }
//
//  ...
//..
// However, if 'first' were defined *inline* the problem would be much broader:
// *every* client that even indirectly includes 'grppkg_someutility.h' would
// end up getting a warning about the use of 'nth' in 'first'.
//..
//  // grppkg_someutility.h
//
//  struct SomeUtility {
//      ...
//
//      #if BSLS_DEPRECATE_IS_ACTIVE(GRP, 1, 2)
//      BSLS_DEPRECATE
//      #endif
//      Element nth(const SomeContainer& container, int index);
//          // Return the 'Element' object in the specified 'index' position in
//          // the specified 'container'. ...
//
//      ...
//  };
//
//  ...
//
//  inline
//  Element SomeUtility::first(const SomeContainer& container)
//  {
//      return nth(container, 0);  // This call will be seen during compilation
//                                 // of all dependents!
//  }
//..
//
// In this case, defining 'BB_BUILDING_UOR_GRP' in the 'grppkg_someutility.h'
// header would be inappropriate.  The author of 'grp' has a few options:
//:  o Move the definition of 'first' out-of-line.
//:  o Re-factor 'first' and 'nth' to both work in terms of a non-deprecated
//:    private function or component-private function.
//
// Of these, moving the function out-of-line is the simplest, if the performance
// cost is not too high.  If the function must be kept inline, refactoring
// 'first' and 'nth' would be preferable:
//..
//  // grppkg_someutility.h
//
//  struct SomeUtility {
//    private:
//      Element nthImp(const SomeContainer& container, int index);
//          // Return the 'Element' object in the specified 'index' position in
//          // the specified 'container'. ...
//
//    public:
//      ConstElementIterator cbegin(const SomeContainer& container);
//          // Return an iterator pointing to the first element in the
//          // specified 'c'.
//
//      // Legacy direct-access API is deprecated.  Use the iterator API
//      // provided by 'cbegin' instead.
//
//      #if BSLS_DEPRECATE_IS_ACTIVE(GRP, 1, 2)
//      BSLS_DEPRECATE
//      #endif
//      Element first(const SomeContainer& container);
//          // Return the 'Element' object at the head of the specified
//          // 'container'.  ...
//
//      #if BSLS_DEPRECATE_IS_ACTIVE(GRP, 1, 2)
//      BSLS_DEPRECATE
//      #endif
//      Element nth(const SomeContainer& container, int index);
//          // Return the 'Element' object in the specified 'index' position in
//          // the specified 'container'. ...
//  };
//
//  // ========================================================================
//  //                      INLINE FUNCTION DEFINITIONS
//  // ------------------------------------------------------------------------
//
//  inline
//  Element SomeUtility::nthImp(const SomeContainer& container, int index)
//  {
//      // Pure magic.
//  }
//
//  inline
//  Element SomeUtility::first(const SomeContainer& container)
//  {
//      return nthImp(container, 0);
//  }
//
//  inline
//  Element SomeUtility::nth(const SomeContainer& container, int index)
//  {
//      return nthImp(container, index);
//  }
//..
//
///Future Development
///------------------
//
///Support for Future Compilers
/// - - - - - - - - - - - - - -
// Future compilers should begin to support this facility as they begin to
// implement either a GCC-style deprecation attribute or the C++14 attribute.
// For example, Sun Studio 12.5 is known to provide deprecation of functions
// (only) via the GCC-style attribute.  This support will be detected
// automatically through the '__has_cpp_attribute' and '__has_attribute'
// intrinsics.
//
// It may be desireable, though, to explicitly disable this facility for
// compiler versions that are known to provide only partial support for
// deprecating interfaces (such as Sun Studio 12.5).
//
///Strong Enforcement of Deprecations
/// - - - - - - - - - - - - - - - - -
//
// By default, the 'bsls_deprecate' facility depends on warnings to encourage
// users to stop using deprecated interfaces.  This can be effective in an
// organization where the build system or code review standards forbid warnings
// in production code.  In an organization where that is not the case, stronger
// enforcement could be achieved by forcing deprecation warnings to be errors
// on platforms such as GCC that allow users to customize diagnostic levels:
//..
//  #if BSLS_DEPRECATE_COMPILER_SUPPORT
//  #  if defined(__GNUC__)
//  #    pragma GCC diagnostic error "-Wdeprecated-declarations"
//  #  elif defined(_MSC_VER)
//  #    pragma warning(error:4996)
//  #  endif
//  #endif  // BSLS_DEPRECATE_COMPILER_SUPPORT
//..

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
