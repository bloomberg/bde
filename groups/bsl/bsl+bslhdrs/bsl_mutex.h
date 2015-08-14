// bsl_mutex.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_MUTEX
#define INCLUDED_BSL_MUTEX

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <mutex>

namespace bsl {

    using native_std::mutex;
    using native_std::recursive_mutex;
    using native_std::timed_mutex;
    using native_std::recursive_timed_mutex;
    using native_std::lock_guard;
    using native_std::unique_lock;
    using native_std::once_flag;
    using native_std::adopt_lock_t;
    using native_std::defer_lock_t;
    using native_std::try_to_lock_t;
    using native_std::adopt_lock;
    using native_std::defer_lock;
    using native_std::try_to_lock;
    using native_std::try_lock;
    using native_std::lock;
    using native_std::call_once;

}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
