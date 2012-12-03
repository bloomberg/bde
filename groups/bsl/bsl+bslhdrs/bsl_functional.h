// bsl_functional.h                                                   -*-C++-*-
#ifndef INCLUDED_BSL_FUNCTIONAL
#define INCLUDED_BSL_FUNCTIONAL

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

#include <functional>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::binary_function;
    using native_std::binary_negate;
    using native_std::bind1st;
    using native_std::bind2nd;
    using native_std::binder1st;
    using native_std::binder2nd;
    using native_std::const_mem_fun1_ref_t;
    using native_std::const_mem_fun1_t;
    using native_std::const_mem_fun_ref_t;
    using native_std::const_mem_fun_t;
    using native_std::divides;
    using native_std::greater;
    using native_std::greater_equal;
    using native_std::less;
    using native_std::less_equal;
    using native_std::logical_and;
    using native_std::logical_not;
    using native_std::logical_or;
    using native_std::mem_fun1_ref_t;
    using native_std::mem_fun1_t;
    using native_std::mem_fun;
    using native_std::mem_fun_ref;
    using native_std::mem_fun_ref_t;
    using native_std::mem_fun_t;
    using native_std::minus;
    using native_std::modulus;
    using native_std::multiplies;
    using native_std::negate;
    using native_std::not1;
    using native_std::not2;
    using native_std::not_equal_to;
    using native_std::plus;
    using native_std::pointer_to_binary_function;
    using native_std::pointer_to_unary_function;
    using native_std::ptr_fun;
    using native_std::unary_function;
    using native_std::unary_negate;
}

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

#ifndef BSL_OVERRIDES_STD


#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
