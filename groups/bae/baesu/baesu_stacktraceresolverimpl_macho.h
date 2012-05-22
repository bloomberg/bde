// baesu_stacktraceresolverimpl_macho.h                               -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACERESOLVERIMPL_MACHO
#define INCLUDED_BAESU_STACKTRACERESOLVERIMPL_MACHO

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a utility to resolve Mach-O symbols in a stack trace.
//
//@CLASSES:
//   baesu_StackTraceResolverImpl<MachO>: symbol resolution for Mach-O objects
//
//@SEE_ALSO: baesu_stacktraceresolverimpl_elf,
//           baesu_stacktraceresolverimpl_windows,
//           baesu_stacktraceresolverimpl_xcoff
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a class,
// 'baesu_StackTraceResolver<MachO>', that, given a vector of
// 'baesu_StackTraceFrame's that have only their 'address' fields set, resolves
// some of the other fields in those frames.  The Mach-O object file format is
// used on Apple platforms.  The Mach-O format is described by
// documents at:
//: o 'http://en.wikipedia.org/wiki/Mach-O'
//: o 'https://developer.apple.com/library/mac/#documentation/DeveloperTools/
//:   Conceptual/MachORuntime/Reference/reference.html'
//: o (which shortens to: 'http://bit.ly/M2yytE')
// but you don't need to understand the OMF to understand this code, you just
// need to understand the 'dladdr' routine ('man dladdr' on a apply machine)
// and the 'abi::__cxa_demangle' routine, described in
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

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_MACHO)

template <typename RESOLVER_POLICY>
class baesu_StackTraceResolverImpl;

      // =================================================================
      // class baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::MachO>
      // =================================================================

template <>
class baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::MachO> {
    // This class provides a public static 'resolve' method that, given a
    // vector of 'baesu_StackTraceFrame's that have only their 'address' fields
    // set, resolves all other fields in those frames.  The Mach-O object file
    // format is used on Apple Darwin platforms.

    // TYPES
    typedef bsls_Types::UintPtr UintPtr;    // 32 bit unsigned on 32 bit, 64
                                            // bit unsigned on 64 bit, usually
                                            // used for absolute offsets into a
                                            // file

    // DATA
    baesu_StackTrace  *d_stackTrace_p;      // pointer to stack trace object.
                                            // The frames contained in this
                                            // have their 'address' fields and
                                            // nothing else initialized upon
                                            // entry to 'resolve', which infers
                                            // as many other fields of them as
                                            // possible.

    char              *d_demangleBuf_p;     // scratch space for demangling

    bool               d_demangle;          // whether we demangle names

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
        // specified '*stackTrace' object given previously populated 'address'
        // fields.  Specify 'demangle', which indicates whether demangling of
        // symbols is to occur.

    ~baesu_StackTraceResolverImpl();
        // Destroy this object.

    // PRIVATE MANIPULATORS
    int resolveSymbol(baesu_StackTraceFrame *frame);
        // Resolve the symbol for the 'i'th frame of the stack trace.  Return 
        // 0 on success and a non-zero value otherwise.  The behavior is
        // undefined if 'i < 0' or 'i >= length' where length is the length of
        // the stack trace object.

    int setFrameSymbolName(baesu_StackTraceFrame *frame);
        // Set the 'symbolName' field of the specified 'frame', which must
        // already have the 'mangledSymbolName' field set, to the demangled
        // version of the 'mangledSymbolName' field.  If 'd_demangle' is
        // 'false' or we are otherwise unable to demangle, just set it to the
        // same as 'mangledSymbolName'.  Return 0 on success and a non-zero
        // value otherwise.

  public:
    // CLASS METHODS
    static int resolve(baesu_StackTrace *stackTrace,
                       bool              demanglingPreferredFlag);
        // Populate information for the specified '*stackTrace', which contains
        // a sequence of randomly-accessible stack trace frames.  Specify
        // 'demanglingPreferredFlag', to determine whether demangling is to
        // occur.  The behavior is undefined unless all the 'address' field in
        // '*stackTrace' are valid and other fields are invalid.

    static
    int testFunc();
        // For testing only.  Do some random garbage and return a line number
        // within this routine.
};

                         // ----------------------------------
                         // class baesu_StackTraceResolverImpl
                         // ----------------------------------

inline
int baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::MachO>::testFunc()
{
    // Do some random garbage to generate some code, then return a line number
    // within this routine

    int line = 0, lineCopy = 0;

    for (int i = 0; true; ++i) {
        BSLS_ASSERT_OPT(line == lineCopy);

        const int loopGuard = 0x8edf0000;    // garbage with a lot of trailing
                                             // 0's.
        const int mask      = 0xa72c3dca;    // pure garbage

        enum { LINE = __LINE__ };

        for (int i = 0; !(i & loopGuard); ++i) {
            line ^= (i & mask);
        }

        // The above loop will leave the value of 'line' unchanged.  See
        // 'foilOptimizer' in the test driver.

        BSLS_ASSERT_OPT(line == lineCopy);

        if (line != 0) {
            break;
        }

        line = LINE;
        lineCopy = line;
    }

    return line;
}

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
