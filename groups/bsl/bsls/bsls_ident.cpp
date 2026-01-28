// bsls_ident.cpp                                                     -*-C++-*-
#include <bsls_ident.h>

BSLS_IDENT("$Id: $")

// Technical notes:
//
// There is a cost to including extra ident information in the binary objects,
// which may include disk space usage and runtime memory usage, as well as
// side effects such as locality of strings in memory at runtime.
//
// Some vendors provide a mechanism to include ident strings in a non-loadable
// .comment section of the binary so that strings are available for review in
// the on-disk binary, but are not loaded into memory at runtime.
// (Alternatively, this separate non-loadable section might selectively be
// stripped from the binary, saved in a database with the checksum of the
// stripped binary, and then the resulting smaller binary moved to production.)
//
// Clang is detected and supported as a GCC variant so does not have any unique
// handling of its own.
//
// `bsls_ident.h` encapsulates ident mechanisms so that includers of this
// header need not be concerned with the mechanism applied.  The mechanism may
// be changed as better methods become available and the includer can obtain
// the changes simply by recompiling (without needing to modify all other
// source code).
//
// `ident` shows only ident-style strings.  `man ident` for more info.
// The tokens passed to the pragmas are not necessarily ident-style.
// They are still visible with `strings -a`.
//
// IBM xlC supports `#pragma comment`
// IBM xlC warns about `#pragma ident` and errors for `#ident`
// Sun CC supports `#pragma ident` and `#ident`
// Sun CC silently ignores `#pragma comment`
// GCC supports `#ident`
// GCC warns about unrecognized `#pragma ident` and `#pragma comment`
// While these can all be worked around by disabling specific warnings/errors,
// doing so might mask other warnings/errors.  This header allows for
// encapsulation of all the conditional logic to use the correct pragma with
// each compiler without having to duplicate these conditions in every source
// file.
//
// `#ident`, `#pragma ident`, `#pragma comment` each can take a -single-
// user-defined token (no concatenation of string constants or preprocessor
// macro `##` concatenation)  (MS Visual Studio will do string concatenation)
//
// C99 `_Pragma()` can expand in macros and can be used in place of `#pragma`.
// GCC does not implement a` #pragma` version of gcc `#ident` preprocessor
// directive
//
// AIX `strip` removes strings inserted by `#pragma comment`.
// Solaris `mcs -d` removes strings inserted by `#ident` and `#pragma ident`
//   (equivalent to compiling with cc -mr)
//   `mcs -c` uniquifies the strings (equivalent to compiling with cc -mc)
//   ('strip' does not remove these strings)
// GNU strip: `strip -R comment` will remove .comment section
//
// AIX multiple `#pragma comment` end up concatenated on a single line,
// allowing for pasting together of individual tokens.  `ident` prints each
// ident-style string on its own line.  Solaris `#ident` places each token on
// its own line.

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
