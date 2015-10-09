// bsl_atomic.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_ATOMIC
#define INCLUDED_BSL_ATOMIC

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

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#include <atomic>

namespace bsl {

    using native_std::atomic;
    using native_std::atomic_compare_exchange_weak;
    using native_std::atomic_compare_exchange_weak_explicit;
    using native_std::atomic_compare_exchange_strong;
    using native_std::atomic_compare_exchange_strong_explicit;
    using native_std::atomic_exchange;
    using native_std::atomic_exchange_explicit;
    using native_std::atomic_fetch_add;
    using native_std::atomic_fetch_add_explicit;
    using native_std::atomic_fetch_sub;
    using native_std::atomic_fetch_sub_explicit;
    using native_std::atomic_fetch_and;
    using native_std::atomic_fetch_and_explicit;
    using native_std::atomic_fetch_or;
    using native_std::atomic_fetch_or_explicit;
    using native_std::atomic_fetch_xor;
    using native_std::atomic_fetch_xor_explicit;
    using native_std::atomic_flag;
    using native_std::atomic_flag_test_and_set;
    using native_std::atomic_flag_test_and_set_explicit;
    using native_std::atomic_flag_clear;
    using native_std::atomic_flag_clear_explicit;
    using native_std::atomic_load;
    using native_std::atomic_load_explicit;
    using native_std::atomic_init;
    using native_std::atomic_is_lock_free;
    using native_std::atomic_signal_fence;
    using native_std::atomic_store;
    using native_std::atomic_thread_fence;
    using native_std::kill_dependency;
    using native_std::memory_order;

}  // close package namespace


// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

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
