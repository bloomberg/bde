// bsls_bslsourcenameparserutil.h                                     -*-C++-*-
#ifndef INCLUDED_BSLS_BSLSOURCENAMEPARSERUTIL
#define INCLUDED_BSLS_BSLSOURCENAMEPARSERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide low-level functions for parsing source file names.
//
//@CLASSES:
//  bsls::BslSourceNameParserUtil: utility namespace to parse source file names
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a namespace
// `bsls::BslSourceNameParserUtil` for methods to parse source code file names
// (as may be reported by the `__FILE__` macro, or a listing), including
// BDE-style (Lakos-style) component and test driver source file names.  This
// component is not using any C++ library or `bsl` dependencies.  In other
// words, it uses only some functionality of the C standard library, and C++
// core language features.  This is a necessary limitation because parsing file
// names may be used form the lowest levels of `bsl` code, especially in test
// drivers.
//
///BDE-Style Source File Names
///---------------------------
// The majority of source files in a BDE-style code base are component header
// and implementation, and their test driver source files.  There are other,
// special, source files names that we describe only briefly.  In this section
// we introduce the basic construct of the component source file names as well
// as the component test driver file names as implemented by this utility.  We
// do not provide complete coverage of all possible BDE-style source file
// naming conventions, for that see John Lakos: Large Scale C++ Volume 1.
//
///Public Component Names
/// - - - - - - - - - - -
// A component consists of two source files sharing the same file name with
// different extensions: `.h` for the header file and `.cpp` for the
// implementation file.  The component file name itself consists of the package
// name the component belongs to (such as `bsls`), followed by an underscore
// (`_`), followed by the name of the component (such as `atomicoperations`),
// all lowercase letters, e.g., `bsls_atomicoperations.h`,
// `bsls_atomicoperations.cpp`.
//
///Subordinate or Package-Internal Component Names
///- - - - - - - - - - - - - - - - - - - - - - - -
// Subordinate components are considered package-internal and, similarly to
// component-private type names, formed by adding further underscore-delimited
// "sections" to the file name (before the extension), such as:
// `bsls_atomicoperations_default.h`, `bsls_atomicoperations_x64_all_gcc.h`,
// `bsls_assert_macroreset.h`, or `bsls_byteorderutil_impl.h`.
//
// There are two kinds of special subordinate components that are considered
// part of the main component, and they are in separate files only for
// non-structural (technical) reasons.  One such kind is the so-called
// subordinate test component, that will be introduced in the section below.
// The other is generated so-called `_cpp03` components for (mainly) emulating
// variadic templates in C++03.  Such generated subordinate components get
// `_cpp03` appended to their file name, right before the first dot `.` of the
// extension(s).
//
///Test Driver Source File Names
///- - - - - - - - - - - - - - -
// Test driver source file names are normally very simple: the same base name
// as the component, followed by `.t.cpp`.  However some components that
// provide class and functions templates with many template parameters require
// more than one test driver so the test driver files can compile, and compile
// in a reasonable amount of time.  (Most often old, C++03 compilers run out of
// resources or internal limits, but in case of optimized code even new
// compilers may try to use too much memory.)
//
// In the past, special private components were created when a test driver was
// too large to compile.  Those components are called "subordinate test
// (driver) components", and their names were created by having the last
// underscore-delimited "segment" of the name to start with `_test`.
// Optionally, after `_test` there could be a decimal number between 0-9, or
// 00-99.  (Due to legacy code that used text, we also allow alpha characters,
// like `_testconstructors`.)  So for an imaginary
// `bslstl_unordinarymultikeymultimap.h` and `.cpp` we may have the expected
// `bslstl_unordinarymultikeymultimap.t.cpp`, but also
// `bslstl_unordinarymultikeymultimap_test1.t.cpp`/`.h`/`.cpp`, and so on.
//
// Newer complex components (and components that have been modernized) will not
// have subordinate test drivers, but so-called test driver parts.  Test driver
// part source file names are created by inserting a dot and a decimal number
// (between 0-9 or 00-99), right before the `.t.cpp`.  Our example from the
// previous paragraph would then have several test driver source files:
// `bslstl_unordinarymultikeymultimap.0.t.cpp`,
// `bslstl_unordinarymultikeymultimap.1.t.cpp`, and so on, but only one
// component `bslstl_unordinarymultikeymultimap.h` and `.cpp`.
//
// Because the components that need many files for their test driver often use
// variadic templates (that need to be emulated in C++03) we often also have:
// `bslstl_unordinarymultikeymultimap_test1_cpp03.t.cpp`/`.h`/`.cpp`, etc., or
// `bslstl_unordinarymultikeymultimap_cpp03.0.t.cpp` files as well.
//
// For the subordinate test components (including the ones that end with
// `_cpp03`) we consider the main component name
// `bslstl_unordinarymultikeymultimap` to be the component name part, because
// the other components exists only due to compiler technical limitations.
//
///Other Source File Names
///- - - - - - - - - - - -
// Special source files that are not components or component test drivers have
// either a special one letter prefix, a one letter "designator extension", or
// a first segment that is too long to be a package name (more than 6
// characters).  The current parsing logic in this utility does not directly
// recognize such source file names.  The parsing logic makes sure that such
// names will not be successfully parsed in a way that the reported component
// name could match a real component.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Determining Component Name from Source File Name
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to determine the component name from one of its source file
// names.  To ease understanding we use string literals for source file names.
//
// First, we declare the result variables that the parser will fill:
// ```
// const char *componentStart  = 0;
// size_t      componentLength = 0;
// ```
// Next, we call the parser, saving its return value:
// ```
// int returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
//                                       &componentStart,
//                                       &componentLength,
//                                       "groups/abc/abcx/abcx_name_cpp03.h");
// ```
//  Now, we verify that the parsing was successful:
// ```
// assert(0 == returnCode);
// ```
// Finally, we verify that the expected component name is found:
// ```
// assert(9 == componentLength &&
//        0 == memcmp("abcx_name", componentStart, 9));
// ```
// Notice how the "_cpp03" suffix of the generated file has been removed.
//
///Example 2: Determining the Type of a Source File
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to determine, in addition to the component name, what kind
// source file name do we have.  To ease understanding we use string literals
// for source file names.
//
// First, we declare the result variables that the parser will fill:
// ```
// const char *componentStart  = 0;
// size_t      componentLength = 0;
// unsigned    sourceType      = ~0u;
// ```
// Next, we call the parser with the first name, passing the address of the
// optional output variable after the source file name:
// ```
// int returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
//                                        &componentStart,
//                                        &componentLength,
//                                        "groups/abc/abcx/abcx_name_cpp03.h",
//                                        &sourceType);
// ```
// Then, we verify that the parsing was successful, and the expected component
// name is found:
// ```
// assert(0 == returnCode);
// assert(9 == componentLength &&
//        0 == memcmp("abcx_name", componentStart, 9));
// ```
// Next, we verify the determined source file type by examining the "kind",
// stored in the bits masked by `bsls::BslSourceNameParserUtil::k_MASK_KIND`,
// and the flags stored in other bits:
// ```
// typedef bsls::BslSourceNameParserUtil Util;  // For brevity
//
// assert(Util::k_HEADER == (sourceType & Util::k_MASK_KIND));
//
// assert(0 == (sourceType & Util::k_IS_MULTIFILE_TEST  ));
// assert(0 == (sourceType & Util::k_IS_SUBORDINATE_TEST));
// assert(0 != (sourceType & Util::k_IS_CPP03_GENERATED ));
// ```
// Then, we can verify a subordinate test component implementation file name.
// These names, and also headers for subordinate test components are special as
// they are not supposed to contain executable code.  They are just another
// test driver for their main component.
// ```
// returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
//                                     &componentStart,
//                                     &componentLength,
//                                     "groups/abc/abcx/abcx_name_test12.cpp",
//                                     &sourceType);
//
// assert(0 == returnCode);
// assert(9 == componentLength &&
//        0 == memcmp("abcx_name", componentStart, 9));
// ```
// Note that the main component name is reported.
// ```
// assert(Util::k_IMPL == (sourceType & Util::k_MASK_KIND));
//
// assert(0 == (sourceType & Util::k_IS_MULTIFILE_TEST  ));
// assert(0 != (sourceType & Util::k_IS_SUBORDINATE_TEST));
// assert(0 == (sourceType & Util::k_IS_CPP03_GENERATED ));
// ```
// Now, we verify a traditional test driver file name of a subordinate test
// component:
// ```
// returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
//                                   &componentStart,
//                                   &componentLength,
//                                   "groups/abc/abcx/abcx_name_test12.t.cpp",
//                                   &sourceType);
//
// assert(0 == returnCode);
// assert(9 == componentLength &&
//        0 == memcmp("abcx_name", componentStart, 9));
//
// assert(Util::k_TTEST == (sourceType & Util::k_MASK_KIND));
//
// assert(0 == (sourceType & Util::k_IS_MULTIFILE_TEST  ));
// assert(0 != (sourceType & Util::k_IS_SUBORDINATE_TEST));
// assert(0 == (sourceType & Util::k_IS_CPP03_GENERATED ));
// ```
// Finally, we verify a multi-file test driver source:
// ```
// returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
//                                                 &componentStart,
//                                                 &componentLength,
//                                                 "wxya_other_cpp03.0.g.cpp",
//                                                 &sourceType);
//
// assert(0 == returnCode);
// assert(10 == componentLength &&
//        0 == memcmp("wxya_other", componentStart, 10));
//
// assert(Util::k_GTEST == (sourceType & Util::k_MASK_KIND));
//
// assert(0 != (sourceType & Util::k_IS_MULTIFILE_TEST  ));
// assert(0 == (sourceType & Util::k_IS_SUBORDINATE_TEST));
// assert(0 != (sourceType & Util::k_IS_CPP03_GENERATED ));
// ```
//
///Example 3: Reporting Parsing Errors
///- - - - - - - - - - - - - - - - - -
// Suppose we need to parse source file names from an external source, and
// therefore we may need to report the reason for parsing failures for human
// readers (of log files).  To ease understanding we use string literals for
// source file names.
//
// First, we declare the result variables that the parser will fill:
// ```
// const char *componentStart  = 0;
// size_t      componentLength = 0;
// ```
// Next, we can call the parser with a too short file name and save the return
// value:
// ```
// int returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
//                                                           &componentStart,
//                                                           &componentLength,
//                                                           "a.h");
// ```
// Then, we verify that the parsing has failed:
// ```
// assert(0 != returnCode);
// ```
// Next, we output a brief error message to the user if requested:
// ```
// if (verbose) {
//     printf("Error parsing source file name \"%s\": %s\n",
//            "a.h",
//            bsls::BslSourceNameParserUtil::errorMessage(returnCode));
//     // Output will indicate the file name was too short (to be a BDE name)
// }
// ```
// Now, we demonstrate another failing-to-parse source name and its error
// message:
// ```
// returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
//                                                         &componentStart,
//                                                         &componentLength,
//                                                         "abcxyz_name.hpp");
// assert(0 != returnCode);
// if (verbose) {
//     printf("Error parsing source file name \"%s\": %s\n",
//            "abcxyz_name.hpp",
//            bsls::BslSourceNameParserUtil::errorMessage(returnCode));
//     // Output will indicate an unsupported extension
// }
// ```
// Finally, we demonstrate the "missing test driver tag" error:
// ```
// returnCode =  bsls::BslSourceNameParserUtil::getComponentName(
//                                                        &componentStart,
//                                                        &componentLength,
//                                                        "abcx_name..t.cpp");
// assert(0 != returnCode);
// if (verbose) {
//     printf("Error parsing source file name \"%s\": %s\n",
//            "abcx_name..t.cpp",
//            bsls::BslSourceNameParserUtil::errorMessage(returnCode));
//     // Output will indicate two dots next to each other in the file name
// }
// ```

#include <stddef.h>  // 'size_t'

namespace BloombergLP {
namespace bsls {

                      //===============================
                      // struct BslSourceNameParserUtil
                      //===============================

/// This `struct` provides a namespace for `static` utility functions that
/// parse source file names (as may be reported by the `__FILE__` macro),
/// including Lakos-style component source and test driver names.
struct BslSourceNameParserUtil {

    // PUBLIC TYPES
    enum SourceTypes {
        // Bit masks and constants that describe the meaning of the 'type_p'
        // parameter of 'getComponentName' below.  Only some bits of the lowest
        // significant word are used at the moment:
        //..
        //  7 6 5 4 3 2 10
        // |r|r|3|S|M|X|KK|
        //
        // K - two bits describing the extension (kind)
        //
        // X - a bit that is set only the file is a test driver "template"
        //     source file with the extension ".xt.cpp".  These files are valid
        //     C++ code, but they are not compiled directly, as they are too
        //     large/slow to compile (on certain platforms/compilers).  Their
        //     content is selectively copied into temporary '.NN.t.xpp' files
        //     that are actually compiled into executables.  The temporary
        //     '.NN.t.xpp' files use '#line N "filename"' directives to refer
        //     back to the ".xt.cpp" template so the actual build and run time
        //     (ASSERT) messages will point to the actual source that is
        //     visible to the programmer (in the source control system).  (The
        //     generated '.NN.t.xpp' files exist during the build only.)
        //
        // M - a bit that is set only the file is a test driver source file,
        //     and it is a multi-file test driver that has a "segment" of
        //     (normally) decimal digits, e.g., "abcx_name.14.t.cpp".
        //
        // S - a bit that is set if the file belongs to a subordinate test
        //     driver, a file that has "_test", followed by non-underscore
        //     characters (except if it is also a generated simulation file for
        //     C++03, see below)
        //
        // 3 - a bit that is set if the file name is a generated simulation
        //     file for C++03 (simulates some C++11 features such as variadic
        //     templates up to a certain number of parameters etc)
        //
        // r - all other bits are reserved for future use
        //..
        // For example "abcx_name_testq_cpp03.g.cpp" will be

        k_MASK_KIND = 0x3,
        k_MASK_TEST = 0x2,

        k_HEADER    = 0x0,  // .h
        k_IMPL      = 0x1,  // .cpp
        k_TTEST     = 0x2,  // .t.cpp -- traditional test driver
        k_GTEST     = 0x3,  // .g.cpp -- Google test test driver

        k_IS_TEST_XTEMPLATE   = 0x4,   // ".xt.cpp" file

        k_IS_MULTIFILE_TEST   = 0x8,   // "[^a-z0-9]+.(t.cpp|g.cpp)"

        k_IS_SUBORDINATE_TEST = 0x10,  // "_test[^a-z0-9]*.(h|cpp|t.cpp|g.cpp)"

        k_IS_CPP03_GENERATED  = 0x20   // "_cpp03" at the very end, before exts
    };

    // CLASS METHODS

                               // Parsing

    /// Parse the specified Lakos-style `sourceName` source file name with
    /// optional path portion to find the component name part.  Return zero
    /// on success and a non-zero value if parsing failed.  In case of
    /// success, fill the specified `componentNamePtr` with a pointer to the
    /// first character, and the specified `componentNameLength` with the
    /// number of character of the component name found.  Optionally specify
    /// `type_p`.  When `type_p` is not 0 set the bits of the pointed
    /// `unsigned`, according to `SourceTypes`, that describe the type of
    /// the source file that was parsed.
    ///
    /// This function does not validate its input, it assumes that it is a
    /// valid Lakos-style component source or test driver file name, or one
    /// of the special names defined by John Lakos: Large Scale C++ Design
    /// (application, adapter, etc).  If `SourceName` is not as such, the
    /// function may return a non-zero error value, or it may report success
    /// with its output is unspecified.
    ///
    /// Subordinate test component sources are special, as they should not
    /// contain code, only their test drivers.  The component name reported
    /// for subordinate test drivers is the main component name.
    ///
    /// Use the `errorMessage` function (in this utility) to get a static,
    /// brief English textual description of a negative return value.
    static int getComponentName(const char **componentNamePtr,
                                size_t      *componentNameLength,
                                const char  *sourceName,
                                unsigned    *type_p = 0);

                             // Miscellaneous

    /// Return a static, brief English error message that describes the
    /// specified negative parsing `errorCode`.  The behavior is undefined
    /// unless  `errorCode < 0` and was returned by one of the parsing
    /// methods (of this utility) that states in its contract to use this
    /// method to get the description of an error code.
    static const char *errorMessage(int errorCode);
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
