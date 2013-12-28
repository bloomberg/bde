// bsl_locale.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_LOCALE
#define INCLUDED_BSL_LOCALE

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

#include <locale>

namespace bsl
{
    // Import selected symbols into bsl namespace
#ifdef BDE_OMIT_INTERNAL_DEPRECATED
    using native_std::bad_exception;
    using native_std::basic_ios;
    using native_std::basic_iostream;
    using native_std::basic_istream;
    using native_std::basic_ostream;
    using native_std::basic_streambuf;
    using native_std::bidirectional_iterator_tag;
    using native_std::codecvt;
    using native_std::codecvt_base;
    using native_std::codecvt_byname;
    using native_std::collate;
    using native_std::collate_byname;
    using native_std::ctype;
    using native_std::ctype_base;
    using native_std::ctype_byname;
    using native_std::exception;
    using native_std::forward_iterator_tag;
    using native_std::has_facet;
    using native_std::input_iterator_tag;
    using native_std::ios_base;
    using native_std::istreambuf_iterator;
    using native_std::iterator;
    using native_std::locale;
    using native_std::messages;
    using native_std::messages_base;
    using native_std::messages_byname;
    using native_std::money_base;
    using native_std::money_get;
    using native_std::moneypunct;
    using native_std::moneypunct_byname;
    using native_std::money_put;
    using native_std::num_get;
    using native_std::numpunct;
    using native_std::numpunct_byname;
    using native_std::num_put;
    using native_std::ostreambuf_iterator;
    using native_std::output_iterator_tag;
    using native_std::random_access_iterator_tag;
    using native_std::set_terminate;
    using native_std::set_unexpected;
    using native_std::swap;
    using native_std::terminate;
    using native_std::terminate_handler;
    using native_std::time_base;
    using native_std::time_get;
    using native_std::time_get_byname;
    using native_std::time_put;
    using native_std::time_put_byname;
    using native_std::uncaught_exception;
    using native_std::unexpected;
    using native_std::unexpected_handler;
    using native_std::use_facet;
#else
    using native_std::codecvt;
    using native_std::codecvt_base;
    using native_std::codecvt_byname;
    using native_std::collate;
    using native_std::collate_byname;
    using native_std::ctype;
    using native_std::ctype_base;
    using native_std::ctype_byname;
    using native_std::has_facet;
    using native_std::locale;
    using native_std::messages;
    using native_std::messages_base;
    using native_std::messages_byname;
    using native_std::money_base;
    using native_std::money_get;
    using native_std::moneypunct;
    using native_std::moneypunct_byname;
    using native_std::money_put;
    using native_std::num_get;
    using native_std::numpunct;
    using native_std::numpunct_byname;
    using native_std::num_put;
    using native_std::time_base;
    using native_std::time_get;
    using native_std::time_get_byname;
    using native_std::time_put;
    using native_std::time_put_byname;
    using native_std::use_facet;
#endif
}

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
