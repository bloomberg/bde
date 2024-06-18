// bslfmt_formatimp.h                                                    -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATIMP_ERROR
#define INCLUDED_BSLFMT_FORMATIMP_ERROR

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_string.h>

#include <stdexcept>

//#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
//// Include version that can be compiled with C++03
//// Generated on Tue Jun 11 07:56:24 2024
//// Command line: sim_cpp11_features.pl bslfmt_formatimp.h
//# define COMPILING_BSLFMT_FORMATIMP_H
//# include <bslfmt_formaterror_cpp03.h>
//# undef COMPILING_BSLFMT_FORMATIMP_H
//#else

namespace BloombergLP {
namespace bslfmt {

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
using std::format_error;
#else
class format_error : public std::runtime_error {
  public:
    // CREATORS
    BSLS_KEYWORD_EXPLICIT format_error(const std::string& what_arg)
    : runtime_error(what_arg)
    {
    }

    BSLS_KEYWORD_EXPLICIT format_error(const char *what_arg)
    : runtime_error(what_arg)
    {
    }

    // If a 'bsl::string' is passed to the 'std::string' constructor, two
    // copies occur (one to initialize 'what_arg', and one to initialize the
    // internal reference-counted string).  This constructor ensures that only
    // a single copy needs to be performed.
    BSLS_KEYWORD_EXPLICIT format_error(const bsl::string& what_arg)
    : runtime_error(what_arg.c_str())
    {
    }

    format_error(const format_error& other) BSLS_KEYWORD_NOEXCEPT
    : runtime_error(other)
    {
    }
};
#endif

} // close namespace bslfmt
} // close enterprise namespace

namespace bsl {

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
    using std::format_error;
#else
    using BloombergLP::bslfmt::format_error;
#endif

}  // close namespace bsl


//#endif // End C++11 code

#endif  // INCLUDED_BSLSTL_FORMATIMP

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
