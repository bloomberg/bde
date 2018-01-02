// bsl_random.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_RANDOM
#define INCLUDED_BSL_RANDOM

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

#include <random>

namespace bsl {

    using native_std::linear_congruential_engine;
    using native_std::mersenne_twister_engine;
    using native_std::subtract_with_carry_engine;
    using native_std::discard_block_engine;
    using native_std::independent_bits_engine;
    using native_std::shuffle_order_engine;
    using native_std::default_random_engine;
    using native_std::minstd_rand;
    using native_std::minstd_rand0;
    using native_std::mt19937;
    using native_std::mt19937_64;
    using native_std::ranlux24_base;
    using native_std::ranlux48_base;
    using native_std::ranlux24;
    using native_std::ranlux48;
    using native_std::knuth_b;
    using native_std::random_device;
    using native_std::uniform_int_distribution;
    using native_std::uniform_real_distribution;
    using native_std::bernoulli_distribution;
    using native_std::binomial_distribution;
    using native_std::geometric_distribution;
    using native_std::negative_binomial_distribution;
    using native_std::poisson_distribution;
    using native_std::exponential_distribution;
    using native_std::gamma_distribution;
    using native_std::weibull_distribution;
    using native_std::extreme_value_distribution;
    using native_std::normal_distribution;
    using native_std::lognormal_distribution;
    using native_std::chi_squared_distribution;
    using native_std::cauchy_distribution;
    using native_std::fisher_f_distribution;
    using native_std::student_t_distribution;
    using native_std::discrete_distribution;
    using native_std::piecewise_constant_distribution;
    using native_std::piecewise_linear_distribution;
    using native_std::seed_seq;
    using native_std::generate_canonical;

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
