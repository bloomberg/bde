// bsl_atomic.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_ATOMIC
#define INCLUDED_BSL_ATOMIC

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>

#include <atomic>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    // primary atomic templates, types, and functions
    using std::atomic;
    using std::atomic_bool;
    using std::atomic_compare_exchange_weak;
    using std::atomic_compare_exchange_weak_explicit;
    using std::atomic_compare_exchange_strong;
    using std::atomic_compare_exchange_strong_explicit;
    using std::atomic_exchange;
    using std::atomic_exchange_explicit;
    using std::atomic_fetch_add;
    using std::atomic_fetch_add_explicit;
    using std::atomic_fetch_sub;
    using std::atomic_fetch_sub_explicit;
    using std::atomic_fetch_and;
    using std::atomic_fetch_and_explicit;
    using std::atomic_fetch_or;
    using std::atomic_fetch_or_explicit;
    using std::atomic_fetch_xor;
    using std::atomic_fetch_xor_explicit;
    using std::atomic_flag;
    using std::atomic_flag_test_and_set;
    using std::atomic_flag_test_and_set_explicit;
    using std::atomic_flag_clear;
    using std::atomic_flag_clear_explicit;
    using std::atomic_load;
    using std::atomic_load_explicit;
    using std::atomic_init;
    using std::atomic_is_lock_free;
    using std::atomic_signal_fence;
    using std::atomic_store;
    using std::atomic_store_explicit;
    using std::atomic_thread_fence;
    using std::kill_dependency;
    using std::memory_order;
    using std::memory_order_relaxed;
    using std::memory_order_consume;
    using std::memory_order_acquire;
    using std::memory_order_release;
    using std::memory_order_acq_rel;
    using std::memory_order_seq_cst;

    // Full specializations and typedefs for integral types
    using std::atomic_char;
    using std::atomic_schar;
    using std::atomic_uchar;
    using std::atomic_short;
    using std::atomic_ushort;
    using std::atomic_int;
    using std::atomic_uint;
    using std::atomic_long;
    using std::atomic_ulong;
    using std::atomic_llong;
    using std::atomic_ullong;
    using std::atomic_char16_t;
    using std::atomic_char32_t;
    using std::atomic_wchar_t;
    using std::atomic_int_least8_t;
    using std::atomic_uint_least8_t;
    using std::atomic_int_least16_t;
    using std::atomic_uint_least16_t;
    using std::atomic_int_least32_t;
    using std::atomic_uint_least32_t;
    using std::atomic_int_least64_t;
    using std::atomic_uint_least64_t;
    using std::atomic_int_fast8_t;
    using std::atomic_uint_fast8_t;
    using std::atomic_int_fast16_t;
    using std::atomic_uint_fast16_t;
    using std::atomic_int_fast32_t;
    using std::atomic_uint_fast32_t;
    using std::atomic_int_fast64_t;
    using std::atomic_uint_fast64_t;
    using std::atomic_size_t;
    using std::atomic_ptrdiff_t;
    using std::atomic_intmax_t;
    using std::atomic_uintmax_t;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS
    // optional support for exact-bit-width types
    using std::atomic_int8_t;
    using std::atomic_int16_t;
    using std::atomic_int32_t;
    using std::atomic_int64_t;
    using std::atomic_uint8_t;
    using std::atomic_uint16_t;
    using std::atomic_uint32_t;
    using std::atomic_uint64_t;
    using std::atomic_intptr_t;
    using std::atomic_uintptr_t;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_PRECISE_BITWIDTH_ATOMICS

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
    using std::atomic_ref;
    using std::atomic_char8_t;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_LOCK_FREE_TYPE_ALIASES
    using std::atomic_signed_lock_free;
    using std::atomic_unsigned_lock_free;
#endif

    using std::atomic_wait;
    using std::atomic_wait_explicit;
    using std::atomic_notify_one;
    using std::atomic_notify_all;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_WAIT_FREE_FUNCTIONS
    using std::atomic_flag_wait;
    using std::atomic_flag_wait_explicit;
    using std::atomic_flag_notify_one;
    using std::atomic_flag_notify_all;
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_ATOMIC_FLAG_TEST_FREE_FUNCTIONS
    using std::atomic_flag_test;
    using std::atomic_flag_test_explicit;
#endif
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
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
