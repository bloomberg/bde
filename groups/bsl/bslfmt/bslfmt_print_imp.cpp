// bslfmt_print_imp.cpp                                               -*-C++-*-
#include <bslfmt_print_imp.h>

#include <bsls_platform.h>

#include <bslstl_systemerror.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslfmt_print_imp_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bslfmt {

void println(std::FILE *stream)
{
    if (std::putc('\n', stream) == EOF) {
        Print_Imp_ThrowIoError();
    }
}

void vprint_nonunicode(std::FILE        *stream,
                       bsl::string_view  fmt,
                       bsl::format_args  args)
{
    // Just delegate
    bslfmt::vprint_nonunicode_buffered(stream, fmt, args);
}

void vprint_nonunicode_buffered(std::FILE        *stream,
                                bsl::string_view  fmt,
                                bsl::format_args  args)
{
    bsl::string out = bsl::vformat(fmt, args);
    if (std::fwrite(out.data(), 1, out.size(), stream) != out.size()) {
        Print_Imp_ThrowIoError();
    }
}

void vprint_unicode(std::FILE        *stream,
                    bsl::string_view  fmt,
                    bsl::format_args  args)
{
    // Just delegate
    bslfmt::vprint_nonunicode_buffered(stream, fmt, args);
}

void vprint_unicode_buffered(std::FILE        *stream,
                             bsl::string_view  fmt,
                             bsl::format_args  args)
{
    // Just delegate
    bslfmt::vprint_nonunicode_buffered(stream, fmt, args);
}

void Print_Imp_ThrowIoError()
{
    throw bsl::system_error(bsl::make_error_code(bsl::errc::io_error));
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
