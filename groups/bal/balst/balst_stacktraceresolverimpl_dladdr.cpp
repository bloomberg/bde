// balst_stacktraceresolverimpl_dladdr.cpp                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceresolverimpl_dladdr.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceresolverimpl_dladdr,"$Id$ $CSID$")

#include <balst_objectfileformat.h>

#ifdef BALST_OBJECTFILEFORMAT_RESOLVER_DLADDR

#include <bsl_cstring.h>

#include <bdlb_string.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <dlfcn.h>

// The following is an excerpt from '#include <cxxabi.h>'.  Unfortunately, that
// include file forward defines a class 'std::type_info' that conflicts with
// the one defined in bsl so we can't include it here and we have to resort to
// an extern.  It looks like there's just no way to include 'cxxabi.h' on
// Darwin and still compile.  Probably someone will fix it at some later date.

namespace __cxxabiv1 {
    extern "C"  {
        // 3.4 Demangler API
        extern char* __cxa_demangle(const char* mangled_name,
                                    char*       output_buffer,
                                    size_t*     length,
                                    int*        status);
            // Demangle the mangled symbol name in the specified buffer
            // 'mangled_name' writing to the specified buffer 'output_buffer'
            // of specified length '*length', setting the specified '*status'
            // to the 0 on success and a non-zero value on failure.  If the
            // symbol was a static symbol, 'output_buffer' will contain "" and
            // -2 will be returned.  '*length' will be set to the length of the
            // output, which will also be zero terminated.
    } // extern "C"
}  // close namespace __cxxabiv1
namespace abi = __cxxabiv1;

///Implementation Notes:
///--------------------
// Given an address in memroy referring to a code segment, the 'dladdr'
// function will find the closest symbol preceding it.  'dladdr' populates a
// 'Dl_info' 'struct', which has the following fields:
//..
//   const char* dli_fname     The pathname of the shared object containing
//                             the address, or the name of the executable if
//                             the code was not in a shared library.
//
//   void* dli_fbase           The base address (mach_header) at which the
//                             image is mapped into the address space of the
//                             calling process.
//
//   const char* dli_sname     The name of the nearest run-time symbol with a
//                             value less than or equal to addr.
//
//   void* dli_saddr           The value of the symbol returned in dli_sname.
//..
// Sometimes 'dladdr' may fail to find 'dli_fname' or 'dli_sname', in which
// case those fields will point to "" or be 0.
//
// Where the memory for the strings comes from is unclear.  The man page does
// not say anything about their needing to be freed, so they may point to some
// data that was in-memory for other reasons.
//
// The memory pointed to by 'dli_fname' and 'dli_sname' is not dynamically
// allocated and does not need to be freed -- this was verifid by running
// the tests under valgrind on Darwin and observing that there were no memory
// leaks.

namespace BloombergLP {

namespace {
namespace local {

enum {
    DEMANGLING_BUFFER_LENGTH = (32 * 1024) - 64   // length in bytes of
                                                  // 'd_demangleBuf_p'.  Make
                                                  // less than a power of 2 to
                                                  // avoid wasting a page
};

typedef balst::StackTraceResolverImpl<balst::ObjectFileFormat::Dladdr>
                                                            StackTraceResolver;

}  // close namespace local
}  // close unnamed namespace

// CREATORS
local::StackTraceResolver::StackTraceResolverImpl(
                                     balst::StackTrace *stackTrace,
                                     bool              demanglingPreferredFlag)
: d_stackTrace_p(stackTrace)
, d_demangleFlag(demanglingPreferredFlag)
, d_hbpAlloc()
{
    d_demangleBuf_p = (char *) d_hbpAlloc.allocate(
                                              local::DEMANGLING_BUFFER_LENGTH);
}

local::StackTraceResolver::~StackTraceResolverImpl()
{
    d_hbpAlloc.deallocate(d_demangleBuf_p);
}

// PRIVATE MANIPULATORS
int local::StackTraceResolver::resolveFrame(balst::StackTraceFrame *frame)
{
    Dl_info info;
    bsl::memset(&info, 0, sizeof(info));
    info.dli_saddr = const_cast<void *>(frame->address());

    // Ignore the status returned by 'dladdr' -- it returns 0 on failure, and
    // doesn't set errno, and returns 0 sometimes when it succeeds.

    dladdr(frame->address(), &info);

    if (!info.dli_fname) {
        info.dli_fname = "";
    }
    if (!info.dli_sname) {
        info.dli_sname = "";
    }

    frame->setLibraryFileName(info.dli_fname);
    frame->setMangledSymbolName(info.dli_sname);
    frame->setOffsetFromSymbol((bsls::Types::UintPtr) frame->address() -
                                        (bsls::Types::UintPtr) info.dli_saddr);

    int rc = 0;
    frame->setSymbolName("");
    if (d_demangleFlag) {
        size_t length = local::DEMANGLING_BUFFER_LENGTH;
        const char *demangled = abi::__cxa_demangle(
                                            frame->mangledSymbolName().c_str(),
                                            d_demangleBuf_p,
                                            &length,
                                            &rc);
        frame->setSymbolName(demangled ? demangled : "");
    }

    if (-2 == rc || frame->symbolName().empty()) {
        // Either demangling was turned off, or it was a static symbol.  For
        // some reason, on Darwin, the demangler reduces static symbols to
        // nothing.  If that happened, just use the mangled symbol name.

        frame->setSymbolName(frame->mangledSymbolName());

        // '-2 == rc' if the symbol was not a properly mangled symbol.  It
        // turns out this is the case for 'main'.

        rc = -2 == rc ? 0 : rc;
    }

    return rc;
}

// CLASS METHODS
int local::StackTraceResolver::resolve(
                                    balst::StackTrace *stackTrace,
                                    bool               demanglingPreferredFlag)
{
    int retRc = 0;
    local::StackTraceResolver resolver(stackTrace,
                                       demanglingPreferredFlag);

    for (int i = 0; i < stackTrace->length(); ++i) {
        int rc = resolver.resolveFrame(&(*stackTrace)[i]);
        retRc = rc ? rc : retRc;
    }

    return retRc;
}

}  // close enterprise namespace

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
