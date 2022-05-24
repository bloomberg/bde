// bsls_deprecatefeature.h                                            -*-C++-*-
#ifndef INCLUDED_BSLS_DEPRECATEFEATURE
#define INCLUDED_BSLS_DEPRECATEFEATURE

//@PURPOSE: Provide machinery to deprecate entities in C++ code.
//
//@MACROS:
//  BSLS_DEPRECATE_FEATURE: mark a C++ entity as deprecated
//
//@DESCRIPTION:  This component provides facilities to identify deprecated C++
// entities.  The deprecation annotations supplied by this component may,
// depending on the build configuration, instantiate as C++ '[[deprecated]]'
// annotations for which the compiler will emit a warning.  Each deprecated
// entity annotated by the macros in this component is identified by a 'UOR'
// and 'FEATURE', allowing the use of that deprecated entity to be more easily
// uniquely identified and tracked over time via tooling.  Here "UOR" means
// Unit-Of-Release, which is typically a package, package-group or library.
//
// **WARNING**: The build configuration flag,
// 'BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING', that enables compiler
// warnings for 'BSLS_DEPRECATE_FEATURE' should not be used in
// cross-organizational integration builds such as a production 'unstable'
// dpkg build.
//
///Concerns with Compiler Warnings
///- - - - - - - - - - - - - - - -
// In large development organization, where many teams may enable "warnings as
// errors", enabling deprecation warnings in a cross-organizational integration
// build will frequently prevent lower-level software from applying the
// deprecation annotation to any newly deprecated code.
//
// Instead this component is designed to support static analysis tools that
// identify deprecated entities, and allow them to be tracked and reported by
// systems outside of the normal compilation process.  The expectation is that
// developers would use configuration options that generate compiler warnings
// in local builds when they are actively working to remove the use of
// deprecated code.
//
///Macro Reference
///---------------
// This section documents the preprocessor macros defined in this component.
//
//: 'BSLS_DEPRECATE_FEATURE(UOR, FEATURE, MESSAGE)':
//:    This macro is used to annotate code to indicate that a name or entity
//:    has been deprecated, and is associated with the specified 'UOR'
//:    (Unit-Of-Release), deprecated 'FEATURE', and 'MESSAGE'.  This macro can
//:    be used as if it were the C++ standard attribute '[[deprecated]]', and
//:    in appropriate build configurations will instantiate as a C++
//:    '[[deprecated]]' annotation.  'UOR' and 'FEATURE' are character strings
//:    intended to uniquely identify one or more related entities that have
//:    been deprecated.  'MESSAGE' is a descriptive text intended for the a
//:    user of the deprecated feature (for example informing them of a
//:    replacement feature).  For example, if several of the date and time
//:    types in the 'bde' library were deprecated they might be marked with the
//:    annotation:
//:    'BSLS_DEPRECATE_FEATURE("bde", "date-and-time", "Use bdlt instead")'.
//:    'UOR' and 'FEATURE' are meant to help uniquely identify a deprecation in
//:    external systems (e.g., a dashboard monitoring the state of a
//:    deprecation) so the supplied strings should start with a letter, and
//:    contain only letters, numbers, underscore, and dash characters (i.e.,
//:    matching the regular expression "[a-zA-Z][\w\-]*").
//:
//: 'BSLS_DEPRECATE_FEATURE_IS_SUPPORTED':  This macro is defined if the
//:    current platform supports instantiating the deprecation annotation
//:    macros into annotation understood by the compiler.
//:
//: 'BSLS_DEPRECATE_FEATURE_ANNOTATION_IS_ACTIVE': This macro is defined if
//:    deprecation annotation macros defined in this component *will* be
//:    instantiated into annotations understood by the compiler (i.e., this
//:    will be defined if 'BSLS_DEPRECATE_FEATURE_SUPPORTED_PLATFORM' is
//:    defined and the build configuration macros are configured in a way
//:    that the annotations will instantiate as the '[[deprecated]]'
//:    attribute).
//
///Configuration Reference
///-----------------------
// There are a set of macros, not defined by this component, that users may
// supply (e.g., to their build system) to configure the behavior of the
// deprecation annotation macros provided by this component.
//
// The available configuration macros are described below:
//..
// * 'BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING': This macro, when
//   defined, enables the instantiation of every deprecation macro as a C++
//   '[[deprecated]]' annotation.  This *MUST* *NOT* be defined as part of
//   cross-organization integration build such as an 'unstable' dpkg build
//   (see {Concerns with Compiler Warnings}).
//
// * 'BB_DEPRECATE_ENABLE_JSON_MESSAGE': Changes the messages reported by
//   compiler deprecation annotations to be a JSON document intended to
//   be useful for tools looking to identify and categorize deprecations.
//
// * 'BSLS_DEPRECATE_FEATURE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING': This macro
//   is a synonym for 'BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING'.
//
// * 'BSLS_DEPRECATE_FEATURE_ENABLE_JSON_MESSAGE': This macro is a synonym for
//   'BB_DEPRECATE_ENABLE_JSON_MESSAGE'.
//..
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Deprecating a Feature
/// - - - - - - - - - - - - - - - -
// The following example demonstrates using the 'BSLS_DEPRECATE_FEATURE' macro
// to deprecate several C++ entities.
//
// The 'BSLS_DEPRECATE_FEATURE' macro can be applied in the same way as the C++
// '[[deprecated]]' annotation.  For example, imagine we are deprecating a
// function 'oldFunction' in the 'bsl' library as part of migrating software to
// the linux platform, we might write:
//..
//  BSLS_DEPRECATE_FEATURE("bsl", "oldFunction", "Use newFunction instead")
//  void oldFunction();
//..
// Here the string "bsl" refers to the library or Unit-Of-Release (UOR) that
// the deprecation occurs in.  "oldFunction" is an arbitrary identifier for
// the feature being deprecated.  Together the 'UOR' and 'FEATURE' are
// intended to form a unique enterprise-wide identifier for the feature being
// deprecated.  Finally the string "Use newFunction instead" is a message for
// users of the deprecated feature.
//
// Marking 'oldFunction' in this way makes the deprecation of 'oldFunction'
// visible to code analysis tools.  In addition, in a local build, warnings
// for uses of the deprecated entity can be enabled using a build macro
// 'BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING' (this macro *MUST* *NOT*
// be used as part of a cross-organization integration build such as a
// 'unstable' dpkg build, see {Concerns with Compiler Warnings}).
//
// Similarly, if we were deprecating a class 'OldType' we might write:
//..
//
//  class BSLS_DEPRECATE_FEATURE("bsl", "OldType", "Use NewType instead")
//                                                                    OldType {
//      // ...
//  };
//..
// Frequently, more than one C++ related entity may be associated with a
// deprecated feature.  In that case we would want to use the same identifier
// for each entity we mark deprecated.  To simplify this we might create a
// deprecation macro that is local to the component.  For example, if we were
// deprecating a queue and its iterator in the 'bde' library we might write:
//..
//  #define BDEC_QUEUE_DEPRECATE                                             \.
//      BSLS_DEPRECATE_FEATURE("bde", "bdec_queue", "Use bsl::queue instead")
//
//  class BDEC_QUEUE_DEPRECATE bdec_Queue {
//      //...
//  };
//
//  class BDEC_QUEUE_DEPRECATE bdec_QueueIterator {
//      //...
//  };
//..
// Sometimes several entities are deprecated as part of the same feature where
// separate messages are appropriate.  For example, imagine we had a component
// 'bsls_measurementutil' that we were converting from imperial to metric
// units:
//..
//  #define BSLS_MEASUREMEANTUTIL_DEPRECATE_IMPERIAL(MESSAGE)                \.
//      BSLS_DEPRECATE_FEATURE("bsl", "deprecate-imperial-units", MESSAGE)
//
//  struct MeasurementUtil {
//
//      BSLS_MEASUREMEANTUTIL_DEPRECATE_IMPERIAL("Use getKilometers instead")
//      static double getMiles();
//
//      BSLS_MEASUREMEANTUTIL_DEPRECATE_IMPERIAL("Use getKilograms instead")
//      static double getPounds();
//  };
//..
// Finally there may be entities across multiple components that require
// deprecation.  For example, we may want to deprecate all the date and time
// types in the 'bde' library.  In those instances one may define a macro in
// the lowest level component (e.g., 'BDET_DATE_DEPRECATE_DATE_AND_TIME' in
// 'bdet_date'), or create a component specifically for the deprecation (e.g.,
// 'BDET_DEPRECATE_DATE_AND_TIME' in a newly created 'bdet_deprecate'
// component).

                       // ==============================
                       // Component Configuration Macros
                       // ==============================

#if defined(__cplusplus) && (__cplusplus >= 201703L)
#define BSLS_DEPRECATE_FEATURE_IS_SUPPORTED
#endif

#if defined(BSLS_DEPRECATE_FEATURE_IS_SUPPORTED) &&                           \
    (defined(BB_DEPRECATE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING) ||             \
     defined(BSLS_DEPRECATE_FEATURE_ENABLE_ALL_DEPRECATIONS_FOR_TESTING))
#define BSLS_DEPRECATE_FEATURE_ANNOTATION_IS_ACTIVE
#else
#undef BSLS_DEPRECATE_FEATURE_ANNOTATION_IS_ACTIVE
#endif

                    // ====================================
                    // Implementation Details: Do *NOT* Use
                    // ====================================

#ifndef BSLS_DEPRECATE_FEATURE_ANNOTATION_IS_ACTIVE
#define BSLS_DEPRECATE_FEATURE_IMP(UOR, FEATURE, MESSAGE)
#else
#if defined(BB_DEPRECATE_ENABLE_JSON_MESSAGE) ||                              \
    defined(BSLS_DEPRECATE_FEATURE_ENABLE_JSON_MESSAGE)
#define BSLS_DEPRECATE_FEATURE_IMP(UOR, FEATURE, MESSAGE)                     \
    [[deprecated("{\"library\": \"" UOR "\", \"feature\": \"" FEATURE         \
    "\", \"message\": \"" MESSAGE "\"}")]]
#else
#define BSLS_DEPRECATE_FEATURE_IMP(UOR, FEATURE, MESSAGE)                     \
    [[deprecated((MESSAGE))]]
#endif  // BB_DEPRECATE_ENABLE_JSON_MESSAGE
#endif

// If the number of arguments needs to be expanded, commit
// abd14c70a7c9fb38d9bf3fe225eb0966cb036c9f contains a variadic implementation
// similar to 'BSLIM_TESTUTIL_ASSERTV'.

                             // =================
                             // Annotation Macros
                             // =================

#define BSLS_DEPRECATE_FEATURE(UOR, FEATURE, MESSAGE)                         \
    BSLS_DEPRECATE_FEATURE_IMP(UOR, FEATURE, MESSAGE)

#endif  // INCLUDED_BSLS_DEPRECATEFEATURE

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
