// bslstl_ostream.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_OSTREAM
#define INCLUDED_BSLSTL_OSTREAM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide aliases and implementations matching standard <ostream>.
//
//@CANONICAL_HEADER: bsl_ostream.h
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_ostream.h>' instead.  This component provides a namespace for
// implementations for standard 'osynstream' I/O manipulators.

#include <bslstl_iosfwd.h>
#include <bslstl_ios.h>
#include <bslstl_syncbufbase.h>

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
    basic_ostream<CHAR,TRAITS>& emit_on_flush(
                                           basic_ostream<CHAR,TRAITS>& stream);
        // If, for the specified 'stream', 'stream.rdbuf()' is a
        // 'basic_syncbuf<CHAR,TRAITS,ALLOCATOR>', make 'stream' emit (i.e.,
        // transmit data to the wrapped stream buffer) when flushed, otherwise
        // has no effect.  Return 'stream'.

    template <class CHAR, class TRAITS>
    basic_ostream<CHAR,TRAITS>& flush_emit(basic_ostream<CHAR,TRAITS>& stream);
        // Flush the specified 'stream' as if by calling 'stream.flush()'.
        // Then, if 'stream.rdbuf()' actually points to a
        // 'basic_syncbuf<CHAR,TRAITS,ALLOCATOR>' 'buf', call 'buf.emit()'.
        // Return 'stream'.

    template <class CHAR, class TRAITS>
    basic_ostream<CHAR,TRAITS>& noemit_on_flush(
                                           basic_ostream<CHAR,TRAITS>& stream);
        // If, for the specified 'stream', 'stream.rdbuf()' is a
        // 'basic_syncbuf<CHAR,TRAITS,ALLOCATOR>', make 'stream' not emit
        // (i.e., don't transmit data to the wrapped stream buffer) when
        // flushed, otherwise has no effect.  Return 'stream'.

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

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

template <class CHAR, class TRAITS>
basic_ostream<CHAR,TRAITS>& emit_on_flush(basic_ostream<CHAR,TRAITS>& stream)
{
    using BloombergLP::bslstl::SyncBufBase;
    if (SyncBufBase *p = dynamic_cast<SyncBufBase*>(stream.rdbuf())) {
        BloombergLP::bslstl::SyncBufBaseUtil::setEmitOnSync(p, true);
    }
    return stream;
}

template <class CHAR, class TRAITS>
basic_ostream<CHAR,TRAITS>& flush_emit(basic_ostream<CHAR,TRAITS>& stream)
{
    using BloombergLP::bslstl::SyncBufBase;
    stream.flush();
    if (SyncBufBase *p = dynamic_cast<SyncBufBase*>(stream.rdbuf())) {
        typename basic_ostream<CHAR,TRAITS>::sentry ok(stream);
        if (!ok) {
            stream.setstate(ios_base::badbit);
        }
        else {
            if (!BloombergLP::bslstl::SyncBufBaseUtil::emit(p)) {
                stream.setstate(ios_base::badbit);
            }
        }
    }
    return stream;
}

template <class CHAR, class TRAITS>
basic_ostream<CHAR,TRAITS>& noemit_on_flush(basic_ostream<CHAR,TRAITS>& stream)
{
    using BloombergLP::bslstl::SyncBufBase;
    if (SyncBufBase *p = dynamic_cast<SyncBufBase*>(stream.rdbuf())) {
        BloombergLP::bslstl::SyncBufBaseUtil::setEmitOnSync(p, false);
    }
    return stream;
}

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
