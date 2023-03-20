// bsl_random.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_RANDOM
#define INCLUDED_BSL_RANDOM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <random>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

    using std::linear_congruential_engine;
    using std::mersenne_twister_engine;
    using std::subtract_with_carry_engine;
    using std::discard_block_engine;
    using std::independent_bits_engine;
    using std::shuffle_order_engine;
    using std::default_random_engine;
    using std::minstd_rand;
    using std::minstd_rand0;
    using std::mt19937;
    using std::mt19937_64;
    using std::ranlux24_base;
    using std::ranlux48_base;
    using std::ranlux24;
    using std::ranlux48;
    using std::knuth_b;
    using std::random_device;
    using std::uniform_int_distribution;
    using std::uniform_real_distribution;
    using std::bernoulli_distribution;
    using std::binomial_distribution;
    using std::geometric_distribution;
    using std::negative_binomial_distribution;
    using std::poisson_distribution;
    using std::exponential_distribution;
    using std::gamma_distribution;
    using std::weibull_distribution;
    using std::extreme_value_distribution;
    using std::normal_distribution;
    using std::lognormal_distribution;
    using std::chi_squared_distribution;
    using std::cauchy_distribution;
    using std::fisher_f_distribution;
    using std::student_t_distribution;
    using std::discrete_distribution;
    using std::piecewise_constant_distribution;
    using std::piecewise_linear_distribution;
    using std::seed_seq;
    using std::generate_canonical;

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
