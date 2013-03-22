// baesu_stacktraceresolverimpl_dladdr.h                              -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACERESOLVERIMPL_DLADDR
#define INCLUDED_BAESU_STACKTRACERESOLVERIMPL_DLADDR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Functions for resolving symbols in a stack trace using 'dladdr'.
//
//@CLASSES:
//   baesu_StackTraceResolverImpl<Dladdr>: symbol resolution using 'dladdr'
//
//@SEE_ALSO: baesu_stacktraceresolverimpl_elf,
//           baesu_stacktraceresolverimpl_windows,
//           baesu_stacktraceresolverimpl_xcoff
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a class,
// 'baesu_StackTraceResolver<Dladdr>', that, given a vector of
// 'baesu_StackTraceFrame' objects that have only their 'address' fields set,
// resolves some of the other fields in those frames.  This resolver will work
// for any platform that supports the 'dladdr' function (e.g. Darwin and
// supported platforms).  Note that 'dladdr' is not a standard system function,
// but documentation is frequently available via 'man dladdr' on supported
// platforms such as Linux and Apple Mac OSX.
//
// Note that this resolving implementation is currently used for the operating
// systems based on the Mach kernel, in particular Apple Mac OSX.
//
// In addition to 'dladdr', this code uses the 'abi::__cxa_demangle' function
// supplied by the gnu and clang compilers for demangling symbol names.
// Documentation can be found:
//: o /usr/include/cxxabi.h
//: o 'http://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html'
//
///Usage
///-----
// This component is an implementation detail of 'baesu' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAESU_OBJECTFILEFORMAT
#include <baesu_objectfileformat.h>
#endif

#ifndef INCLUDED_BAESU_STACKTRACE
#include <baesu_stacktrace.h>
#endif

#ifndef INCLUDED_BAESU_STACKTRACEFRAME
#include <baesu_stacktraceframe.h>
#endif

#ifndef INCLUDED_BDEMA_HEAPBYPASSALLOCATOR
#include <bdema_heapbypassallocator.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_DLADDR)

template <typename RESOLVER_POLICY>
class baesu_StackTraceResolverImpl;

      // ==================================================================
      // class baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Dladdr>
      // ==================================================================

template <>
class baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Dladdr> {
    // This class provides a public static 'resolve' method that, given a
    // vector of 'baesu_StackTraceFrame' objects that have only their 'address'
    // fields set, resolves all other fields in those frames.

    // DATA
    baesu_StackTrace  *d_stackTrace_p;      // pointer to stack trace object.
                                            // The frames contained in this
                                            // have their 'address' fields and
                                            // nothing else initialized upon
                                            // entry to 'resolve', which infers
                                            // as many other fields of them as
                                            // possible.

    char              *d_demangleBuf_p;     // scratch space for demangling,
                                            // length is 'DEMANGLING_BUF_LEN'
                                            // in the imp file.

    bool               d_demangleFlag;      // whether we demangle names

    bdema_HeapBypassAllocator
                       d_hbpAlloc;          // heap bypass allocator -- owned

  private:
    // NOT IMPLEMENTED
    baesu_StackTraceResolverImpl(const baesu_StackTraceResolverImpl&);
    baesu_StackTraceResolverImpl& operator=(
                                          const baesu_StackTraceResolverImpl&);

  private:
    // PRIVATE CREATORS
    baesu_StackTraceResolverImpl(baesu_StackTrace *stackTrace,
                                 bool              demanglingPreferredFlag);
        // Create an stack trace reolver that can populate other fields of the
        // specified 'stackTrace' object given previously populated 'address'
        // fields, and if the specified 'demanglingPreferredFlag' is 'true',
        // attempt to demangle symbol names.

    ~baesu_StackTraceResolverImpl();
        // Destroy this object.

    // PRIVATE MANIPULATORS
    int resolveFrame(baesu_StackTraceFrame *frame);
        // Given the specified 'frame' with all fields uninitializd other than
        // the initialized 'address()' field, populate as many other fields as
        // possible, currently the 'liberaryFileName()', 'mangledSymbolName()',
        // 'offsetFromSymbol()', and 'symbolName()' fields.  If
        // 'd_demangleFlag' is true, 'symbolName()' will be a demangled form of
        // 'mangledSymbolName(), otherwise the two fields will be identical.
        // Return 0 on success and a non-zero value if any problems were
        // encountered.  Note that this function is defined as a member
        // function to make use of the 'd_demanglingBuf_p' buffer, and avoid
        // the use of a stack or heap allocated buffer for demangling symbols
        // -- using additional stack or heap memory may cause problems when
        // generating a stack trace to capture the state of a thread that has
        // failed due to stack or heap corruption.

  public:
    // CLASS METHODS
    static int resolve(baesu_StackTrace *stackTrace,
                       bool              demanglingPreferredFlag);
        // Populate information for the specified '*stackTrace', which contains
        // a sequence of randomly-accessible stack trace frames.  Specify
        // 'demanglingPreferredFlag', to determine whether demangling is to
        // occur.  The behavior is undefined unless all the 'address' field in
        // '*stackTrace' are valid and other fields are invalid.
};

#endif

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
