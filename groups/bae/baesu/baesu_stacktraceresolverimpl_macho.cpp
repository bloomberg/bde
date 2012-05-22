// baesu_stacktraceresolverimpl_macho.cpp                             -*-C++-*-
#include <baesu_stacktraceresolverimpl_macho.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktraceresolverimpl_macho_cpp,"$Id$ $CSID$")

#include <baesu_objectfileformat.h>

#ifdef BAESU_OBJECTFILEFORMAT_RESOLVER_MACHO

#include <bdeu_string.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cstring.h>

#include <dlfcn.h>

// The following is an excerpt from '#include <cxxabi.h>'.  Unfortunately,
// that include file defines a class 'type_info' that conflicts with one
// defined in bsl so we can't include it here and we have to resort to an
// extern.

namespace __cxxabiv1 {
  extern "C"  {
    // 3.4 Demangler API
    extern char* __cxa_demangle(const char* mangled_name, 
                                char*       output_buffer,
                                size_t*     length, 
                                int*        status);
  } // extern "C"
} // namespace __cxxabiv1
namespace abi = __cxxabiv1;

//=============================================================================
//           Debugging trace macros: 'eprintf' and 'zprintf'
//=============================================================================

///Implementation Notes:
///--------------------
//
// Given a pointer to some code, 'dladdr' will find t he closest symbol
// before it.  'dladdr' populates a 'Dl_info' struct. which has the following
// fields:
//
//   const char* dli_fname     The pathname of the shared object containing
//                             the address.
//
//   void* dli_fbase           The base address (mach_header) at which the
//                             image is mapped into the address space of the
//                             calling process.
//
//   const char* dli_sname     The name of the nearest run-time symbol with a
//                             value less than or equal to addr.
//
//   void* dli_saddr           The value of the symbol returned in dli_sname.
//
// Where the memory for the strings comes from is unclear.  The man page does
// not say anything about their needing to be freed, so they may point to some
// data that was in-memory for other reasons.
//
// TBD: Does the memory for the char strings have to be freed?  Don't free
// them at first but run valgrind or purify to determine if memory is being
// leaked.

namespace BloombergLP {


namespace {

namespace Local {

enum {
    DEMANGLE_BUF_LEN= (32 * 1024) - 64  // length in bytes of
                                        // 'd_demangleBuf_p'.  Make less than
                                        // a power of 2 to avoid wasting a page
};

typedef baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::MachO>
                                                            StackTraceResolver;

}  // close namespace Local

}  // close unnamed namespace

// CREATORS
Local::StackTraceResolver::baesu_StackTraceResolverImpl(
                                     baesu_StackTrace *stackTrace,
                                     bool              demanglingPreferredFlag)
: d_stackTrace_p(stackTrace)
, d_demangle(demanglingPreferredFlag)
, d_hbpAlloc()
{
    d_demangleBuf_p = (char *) d_hbpAlloc.allocate(Local::DEMANGLE_BUF_LEN);
}

Local::StackTraceResolver::~baesu_StackTraceResolverImpl()
{
    d_hbpAlloc.deallocate(d_demangleBuf_p);
}

// PRIVATE MANIPULATORS
int Local::StackTraceResolver::resolveSymbol(baesu_StackTraceFrame *frame)
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
    frame->setOffsetFromSymbol((UintPtr) frame->address() -
                                                     (UintPtr) info.dli_saddr);

    return setFrameSymbolName(frame);
}

int Local::StackTraceResolver::setFrameSymbolName(
                                                  baesu_StackTraceFrame *frame)
{
    int rc = 0;
    frame->setSymbolName("");
    if (d_demangle) {
        size_t length = Local::DEMANGLE_BUF_LEN;
        frame->setSymbolName(abi::__cxa_demangle(
                                            frame->mangledSymbolName().c_str(),
                                            d_demangleBuf_p,
                                            &length,
                                            &rc));
    }

    if (-2 == rc || frame->symbolName().empty()) {
        // Either demangling was turned off, or it was a static symbol.  For
        // some reason, on Darwin, the demangler reduces static symbols to
        // nothing.  If that happened, just use the mangled symbol name.

        frame->setSymbolName(frame->mangledSymbolName());

        // '-2 == rc' if the symbol was not a properly mangled symbol.  It
        // turns out this is the case for 'main'.
        rc = 0;
    }

    return rc;
}

// CLASS METHODS
int Local::StackTraceResolver::resolve(
                                     baesu_StackTrace *stackTrace,
                                     bool              demanglingPreferredFlag)
{
    int retRc = 0;
    Local::StackTraceResolver resolver(stackTrace,
                                       demanglingPreferredFlag);
    baesu_StackTrace& st = *stackTrace;

    for (int i = 0; i < st.length(); ++i) {
        int rc = resolver.resolveSymbol(&st[i]);
        retRc = rc ? rc : retRc;
    }

    return retRc;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
