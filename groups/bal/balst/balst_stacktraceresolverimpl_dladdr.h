// balst_stacktraceresolverimpl_dladdr.h                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_STACKTRACERESOLVERIMPL_DLADDR
#define INCLUDED_BALST_STACKTRACERESOLVERIMPL_DLADDR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions for resolving a stack trace using 'dladdr'.
//
//@CLASSES:
//   balst::StackTraceResolverImpl<Dladdr>: symbol resolution using 'dladdr'
//
//@SEE_ALSO: balst_stacktraceresolverimpl_elf,
//           balst_stacktraceresolverimpl_windows,
//           balst_stacktraceresolverimpl_xcoff
//
//@DESCRIPTION: This component provides a class,
// 'balst::StackTraceResolver<Dladdr>', that, given a vector of
// 'balst::StackTraceFrame' objects that have only their 'address' fields set,
// resolves some of the other fields in those frames.  This resolver will work
// for any platform that supports the 'dladdr' function (e.g., Darwin and
// Linux).  Note that 'dladdr' is not a standard system function, but
// documentation is frequently available via 'man dladdr' on supported
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
// This component is an implementation detail of 'balst' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALST_OBJECTFILEFORMAT
#include <balst_objectfileformat.h>
#endif

#ifndef INCLUDED_BALST_STACKTRACE
#include <balst_stacktrace.h>
#endif

#ifndef INCLUDED_BALST_STACKTRACEFRAME
#include <balst_stacktraceframe.h>
#endif

#ifndef INCLUDED_BDLMA_HEAPBYPASSALLOCATOR
#include <bdlma_heapbypassallocator.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_DLADDR)


namespace balst {template <typename RESOLVER_POLICY>
class StackTraceResolverImpl;

           // ======================================================
           // class StackTraceResolverImpl<ObjectFileFormat::Dladdr>
           // ======================================================

template <>
class StackTraceResolverImpl<ObjectFileFormat::Dladdr> {
    // This class provides a public static 'resolve' method that, given a
    // vector of 'StackTraceFrame' objects that have only their 'address'
    // fields set, resolves all other fields in those frames.

    // DATA
    StackTrace  *d_stackTrace_p;      // pointer to stack trace object.
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

    bdlma::HeapBypassAllocator
                       d_hbpAlloc;          // heap bypass allocator -- owned

  private:
    // NOT IMPLEMENTED
    StackTraceResolverImpl(const StackTraceResolverImpl&);
    StackTraceResolverImpl& operator=(
                                          const StackTraceResolverImpl&);

  private:
    // PRIVATE CREATORS
    StackTraceResolverImpl(StackTrace *stackTrace,
                                 bool              demanglingPreferredFlag);
        // Create an stack trace reolver that can populate other fields of the
        // specified 'stackTrace' object given previously populated 'address'
        // fields, and if the specified 'demanglingPreferredFlag' is 'true',
        // attempt to demangle symbol names.

    ~StackTraceResolverImpl();
        // Destroy this object.

    // PRIVATE MANIPULATORS
    int resolveFrame(StackTraceFrame *frame);
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
    static int resolve(StackTrace *stackTrace,
                       bool              demanglingPreferredFlag);
        // Populate information for the specified '*stackTrace', which contains
        // a sequence of randomly-accessible stack trace frames.  Specify
        // 'demanglingPreferredFlag', to determine whether demangling is to
        // occur.  The behavior is undefined unless all the 'address' field in
        // '*stackTrace' are valid and other fields are invalid.
};

}  // close package namespace

#endif

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
