// memory.h                                                           -*-C++-*-

//@PURPOSE: Provide backwards compatibility for STLPort header
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component provides backwards compatibility for Bloomberg
// libraries that depend on non-standard STLPort header files.  This header
// includes the corresponding C++ header and provides any additional symbols
// that the STLPort header would define via transitive includes.

/* STLPort reasoning: */
/* The header <memory.h> is not part of either the C or C++ standards, but is
 * in widespread use none the less.  It contains a portion of what is included
 * in the standard C header, <string.h>.  In the Sun compiler, <memory.h> is
 * broken wrt the use of namespaces, but <string.h> is fine.  Therefore, the
 * easiest way to provide the functionality of <memory.h> is to simply forward
 * it to <string.h>
 */

#if !defined(__cplusplus)

#include <string.h>

#else  /* defined(__cplusplus) */

#include <bsl_cstring.h>

using std::memmove;
using std::memcpy;
using std::memchr;
using std::memcmp;
using std::memset;

#endif  /* __cplusplus */

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
