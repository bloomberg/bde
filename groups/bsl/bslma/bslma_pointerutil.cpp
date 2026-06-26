// bslma_pointerutil.cpp                                              -*-C++-*-
#include <bslma_pointerutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

///Implementation Notes
///--------------------
// The initial design for `unqualify` was a single overload taking
// `const volatile TYPE *` and returning `TYPE *`.  That design failed to
// support function types, since a function type such as `void()` cannot be
// cv-qualified, so an additional pass-through overload taking `TYPE *` is
// needed.  That overload, in turn, would match pointers such as
// `const int *`, deducing `TYPE` as `const int` and returning `const int *`
// instead of `int *`.  We therefore provide overloads for all four
// cv-qualifications of `TYPE` and rely on partial ordering to select the
// most qualified overload for each case.
//
// On the Sun CC compiler, partial ordering erroneously selects the
// `const volatile TYPE *` overload for function pointers, then fails because
// `const_cast` cannot be applied to a function pointer.  On that platform we
// provide only the `const volatile TYPE *` overload and use tag dispatch on
// `bsl::is_function<TYPE>` to separate function pointers (returned unchanged)
// from object pointers (cast via `const_cast`).

namespace BloombergLP {
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
