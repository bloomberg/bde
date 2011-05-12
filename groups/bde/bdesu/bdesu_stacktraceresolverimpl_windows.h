// bdesu_stacktraceresolverimpl_windows.h                             -*-C++-*-
#ifndef INCLUDED_BDESU_STACKTRACERESOLVERIMPL_WINDOWS
#define INCLUDED_BDESU_STACKTRACERESOLVERIMPL_WINDOWS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide resolution of symbols in stack trace for Windows objects
//
//@CLASSES:
//   bdesu_StackTraceResolverImpl<Windows>: symbol resolution for Windows objs
//
//@SEE_ALSO:
//
//@AUTHOR: Oleg Semenov, Bill Chapman
//
//@DESCRIPTION: This class provides a class this able to take a vector of
// 'bdesu_StackTraceFrame's that have only their 'address' fields set, and sets
// as many of the other fields in the stack trace frames as possible.  Elf
// objects are used on Solaris, Linux, and HPUX platforms.
//
///Usage
///-----
// This component is an implementation detail of 'bdesu' and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDESU_OBJECTFILEFORMAT
#include <bdesu_objectfileformat.h>
#endif

#ifndef INCLUDED_BDESU_STACKTRACEFRAME
#include <bdesu_stacktraceframe.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

#if defined(BDESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)

template <typename RESOLVER_POLICY>
class bdesu_StackTraceResolverImpl;

    // ===================================================================
    // class bdesu_StackTraceResolverImpl<bdesu_ObjectFileFormat::Windows>
    // ===================================================================

template <>
class bdesu_StackTraceResolverImpl<bdesu_ObjectFileFormat::Windows> {
    // This type is for resolving symbols in Windows executables.  Resolving
    // symbols on Windows is straightforward using the dbghelp.dll library,
    // described at
    // http://msdn.microsoft.com/en-us/library/ms681412(v=VS.85).aspx

  private:
    // NOT IMPLEMENTED
    bdesu_StackTraceResolverImpl();
    bdesu_StackTraceResolverImpl(const bdesu_StackTraceResolverImpl&);
    bdesu_StackTraceResolverImpl& operator=(
                                          const bdesu_StackTraceResolverImpl&);
    ~bdesu_StackTraceResolverImpl();

  public:
    // CLASS METHODS
    static int resolve(bsl::vector<bdesu_StackTraceFrame> *outFrames,
                       bool                                demangle,
                       bslma_Allocator                    *basicAllocator);
        // Given a specified vector 'outFrames' of stack trace frames with only
        // their 'address' fields valid, set as many other fields of the frames
        // as possible.  The 'demangle' argument is ignored, demangling always
        // happens on Windows.  Specify 'basicAllocator' which will be used for
        // all other memory allocation.  Return 0 if successful and a non-zero
        // value otherwise.

    static inline
    int testFunc();
        // For testing only.  Do some random garbage and return a line number
        // from within the routine.
};

                         // ----------------------------------
                         // class bdesu_StackTraceResolverImpl
                         // ----------------------------------

inline
int bdesu_StackTraceResolverImpl<bdesu_ObjectFileFormat::Windows>::testFunc()
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
