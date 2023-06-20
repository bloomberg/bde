// bslstl_ostream.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_OSTREAM
#define INCLUDED_BSLSTL_OSTREAM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bslstl_iosfwd.h>
#include <bslstl_ios.h>

#include <bsls_platform.h>

#include <ostream>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace

    using std::basic_ostream;
    using std::endl;
    using std::ends;
    using std::flush;
    using std::ostream;
    using std::wostream;

    // This file transitively includes standard <exception> header, so the
    // following symbol needs to be added into bsl namespace.
    using std::exception;

    template <class CHAR, class TRAITS>
    basic_ostream<CHAR,TRAITS>& emit_on_flush(basic_ostream<CHAR,TRAITS>& os)
        // If, for the specified 'os', 'os.rdbuf()' is a
        // 'basic_syncbuf<CHAR,TRAITS,ALLOCATOR>', make 'os' emit (i.e.,
        // transmit data to the wrapped stream buffer) when flushed, otherwise
        // has no effect.  Return 'os'.
    {
        syncbuf_Base<CHAR,TRAITS> *p =
                          dynamic_cast<syncbuf_Base<CHAR,TRAITS>*>(os.rdbuf());
        if (p) {
            p->set_emit_on_sync(true);
        }
        return os;
    }

    template <class CHAR, class TRAITS>
    basic_ostream<CHAR,TRAITS>& flush_emit(basic_ostream<CHAR,TRAITS>& os)
        // Flush the specified 'os' as if by calling 'os.flush()'.  Then, if
        // 'os.rdbuf()' actually points to a
        // 'basic_syncbuf<CHAR,TRAITS,ALLOCATOR>' 'buf', call 'buf.emit()'.
        // Return 'os'.
    {
        os.flush();
        syncbuf_Base<CHAR,TRAITS> *p =
                          dynamic_cast<syncbuf_Base<CHAR,TRAITS>*>(os.rdbuf());
        if (p) {
            typename basic_ostream<CHAR,TRAITS>::sentry ok(os);
            if (!ok) {
                os.setstate(ios_base::badbit);
            }
            else {
                if (!p->emitInternal()) {
                    os.setstate(ios_base::badbit);
                }
            }
        }
        return os;
    }

    template <class CHAR, class TRAITS>
    basic_ostream<CHAR,TRAITS>& noemit_on_flush(basic_ostream<CHAR,TRAITS>& os)
        // If, for the specified 'os', 'os.rdbuf()' is a
        // 'basic_syncbuf<CHAR,TRAITS,ALLOCATOR>', make 'os' not emit (i.e.,
        // don't transmit data to the wrapped stream buffer) when flushed,
        // otherwise has no effect.  Return 'os'.
    {
        syncbuf_Base<CHAR,TRAITS> *p =
                          dynamic_cast<syncbuf_Base<CHAR,TRAITS>*>(os.rdbuf());
        if (p) {
            p->set_emit_on_sync(false);
        }
        return os;
    }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Export additional names, leaked to support transitive dependencies in
    // higher level (non BDE) Bloomberg code.
# if !defined(BSLS_PLATFORM_CMP_MSVC) && __cplusplus < 201703L
    // As some of these names are removed from C++17, take a sledgehammer to
    // crack this nut, and remove all non-standard exports.
    using std::bad_exception;
    using std::basic_ios;
    using std::basic_streambuf;
    using std::bidirectional_iterator_tag;
    using std::ctype;
    using std::ctype_base;
    using std::ctype_byname;
    using std::forward_iterator_tag;
    using std::input_iterator_tag;
    using std::ios_base;
    using std::istreambuf_iterator;
    using std::iterator;
    using std::locale;
    using std::numpunct;
    using std::numpunct_byname;
    using std::ostreambuf_iterator;
    using std::output_iterator_tag;
    using std::random_access_iterator_tag;
    using std::set_terminate;
    using std::set_unexpected;
    using std::swap;
    using std::terminate;
    using std::terminate_handler;
    using std::uncaught_exception;
    using std::unexpected;
    using std::unexpected_handler;
    using std::use_facet;
# endif // MSVC, or C++2017
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
}  // close package namespace

#endif

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
