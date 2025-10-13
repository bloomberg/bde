// bslstl_complex.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLSTL_COMPLEX
#define INCLUDED_BSLSTL_COMPLEX

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@CANONICAL_HEADER: bsl_complex.h
//
//@DESCRIPTION: This component is for internal use only. Please include
//`<bsl_complex.h>` directly.  This component imports symbols declared in the
// <complex> header file implemented in the standard library provided by the
// compiler vendor.
//
///User-defined literals
///---------------------
// This component provides a set of user-defined literals (UDL) to form
// `bsl::complex` objects with various ranges of values.  The ud-suffixes are
// preceded with the `_` symbol to distinguish between the `bsl`-complex's UDLs
// and the `std`-complex's UDLs introduced in the C++14 standard and
// implemented in the standard library.  Note that `bsl`-complex's UDLs, unlike
// the `std`-complex's UDLs, can be used in a client's code if the current
// compiler supports the C++11 standard.
//
// Also note that `bsl`-complex's UDL operators are declared in the
// `bsl::literals::complex_literals` namespace, where `literals` and
// `complex_literals` are inline namespaces.  Access to these operators can be
// gained with either `using namespace bsl::literals`,
// `using namespace bsl::complex_literals` or
// `using namespace bsl::literals::complex_literals`.  But we recommend
// `using namespace bsl::complex_literals` to minimize the scope of the using
// declaration.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic `bsl`-complex's UDLs Usage
/// - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates basic use of the complex user-defined literal
// operators.
//
// First, we provide an access to `bsl`-complex's UDLs.
// ```
// using namespace bsl::complex_literals;
// ```
// Then, we construct a `bsl::complex` object `z`.
// ```
// auto z = 1.0 + 1_i;
// ```
// Finally, stream the magnitude of the complex number `z` to `stdout`:
// ```
// printf("abs(%.1f,%.1f) = %f\n", z.real(), z.imag(), bsl::abs(z));
// ```
// The streaming operator produces output in the following format on `stdout`:
// ```
// abs(1.0,1.0) = 1.414214
// ```

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <complex>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

    // Import selected symbols into bsl namespace

    using std::abs;
    using std::acos;
    using std::arg;
    using std::asin;
    using std::atan;
    using std::complex;
    using std::conj;
    using std::cos;
    using std::cosh;
    using std::exp;
    using std::imag;
    using std::log;
    using std::log10;
    using std::norm;
    using std::polar;
    using std::pow;
    using std::real;
    using std::sin;
    using std::sinh;
    using std::sqrt;
    using std::tan;
    using std::tanh;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using std::acosh;
    using std::asinh;
    using std::atanh;
    using std::proj;
#endif  // BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    using std::basic_ios;
    using std::basic_iostream;
    using std::basic_istream;
    using std::basic_ostream;
    using std::basic_streambuf;
    using std::ios_base;
    using std::istreambuf_iterator;
    using std::locale;
    using std::ostreambuf_iterator;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    inline namespace literals {
        inline namespace complex_literals {
               using namespace std::literals::complex_literals;
        }  // close complex_literals namespace
    }  // close literals namespace
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)

inline namespace literals {
inline namespace complex_literals {

    /// Create a `bsl::complex<double>` complex number having the specified
    /// `imag` imaginary component and the real part zero.
    BSLS_KEYWORD_CONSTEXPR
    bsl::complex<double> operator""_i(unsigned long long imag);
    BSLS_KEYWORD_CONSTEXPR
    bsl::complex<double> operator""_i(long double imag);

    /// Create a `bsl::complex<float>` complex number having the specified
    /// `imag` imaginary component and the real part zero.
    BSLS_KEYWORD_CONSTEXPR
    bsl::complex<float> operator""_if(unsigned long long imag);
    BSLS_KEYWORD_CONSTEXPR
    bsl::complex<float> operator""_if(long double imag);

    /// Create a `bsl::complex<long double>` complex number having the
    /// specified `imag` imaginary component and the real part zero.
    BSLS_KEYWORD_CONSTEXPR
    bsl::complex<long double> operator""_il(unsigned long long imag);
    BSLS_KEYWORD_CONSTEXPR
    bsl::complex<long double> operator""_il(long double imag);

}  // close complex_literals namespace
}  // close literals namespace
#endif

// ============================================================================
//                   INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY) && \
    defined (BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)

inline namespace literals {
inline namespace complex_literals {
    inline BSLS_KEYWORD_CONSTEXPR
    bsl::complex<double> operator""_i(unsigned long long arg)
    {
        return bsl::complex<double>{0.0, static_cast<double>(arg)};
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::complex<double> operator""_i(long double arg)
    {
        return bsl::complex<double>{0.0, static_cast<double>(arg)};
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::complex<float> operator""_if(unsigned long long arg)
    {
        return bsl::complex<float>{0.0f, static_cast<float>(arg)};
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::complex<float> operator""_if(long double arg)
    {
        return bsl::complex<float>{0.0f, static_cast<float>(arg)};
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::complex<long double> operator""_il(unsigned long long arg)
    {
        return bsl::complex<long double>{0.0L, static_cast<long double>(arg)};
    }

    inline BSLS_KEYWORD_CONSTEXPR
    bsl::complex<long double> operator""_il(long double arg)
    {
        return bsl::complex<long double>{0.0L, static_cast<long double>(arg)};
    }
}  // close complex_literals namespace
}  // close literals namespace
#endif
}  // close package namespace

//#endif
#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
