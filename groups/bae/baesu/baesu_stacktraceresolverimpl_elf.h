// baesu_stacktraceresolverimpl_elf.h                                 -*-C++-*-
#ifndef INCLUDED_BAESU_STACKTRACERESOLVERIMPL_ELF
#define INCLUDED_BAESU_STACKTRACERESOLVERIMPL_ELF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a utility to resolve ELF symbols in a stack trace.
//
//@CLASSES:
//   baesu_StackTraceResolverImpl<Elf>: symbol resolution for ELF objects
//
//@SEE_ALSO: baesu_stacktraceresolverimpl_{windows,xcoff}
//
//@AUTHOR: Oleg Semenov (osemenov), Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a class, baesu_StackTraceResolver<Elf>,
// that, given a vector of 'baesu_StackTraceFrame's that have only their
// 'address' fields set, resolves all other fields in those frames.  The Elf
// object file format is used on Linux, Solaris, and HP-UX platforms.
//: The ELF format is described by documents at
//: http://en.wikipedia.org/wiki/Executable_and_Linkable_Format and
//: http://downloads.openwatcom.org/ftp/devel/docs/elf-64-gen.pdf
//: http://www.sco.com/developers/gabi/latest/contents.html
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

#ifndef INCLUDED_BAESU_STACKTRACEFRAME
#include <baesu_stacktraceframe.h>
#endif

#ifndef INCLUDED_BAESU_STACKTRACERESOLVER_FILEHELPER
#include <baesu_stacktraceresolver_filehelper.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF)

template <typename RESOLVER_POLICY>
class baesu_StackTraceResolverImpl;

           // =======================================================
           // class baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Elf>
           // =======================================================

template <>
class baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Elf> {
    // This class provides a public static 'resolve' method that, given a
    // vector of 'baesu_StackTraceFrame's that have only their 'address' fields
    // set, resolves all other fields in those frames.  The Elf object file
    // format is used on Linux, Solaris, and HP-UX platforms.  All other
    // methods in this class are private.

    // TYPES
    typedef bsls_Types::UintPtr UintPtr;    // 32 bit unsigned on 32 bit, 64
                                            // bit unsigned on 64 bit, usually
                                            // used for absolute offsets into a
                                            // file

    struct CurrentSegment;                  // 'struct' that contains
                                            // information pertaining only to
                                            // the current segment being
                                            // resolved (during resolution, the
                                            // resolver iterates over multiple
                                            // segments)

    // DATA
    bsl::vector<baesu_StackTraceFrame>
                      *d_ioAllFrames_p;     // pointer to vector of frames in
                                            // the StackTrace object

    CurrentSegment    *d_seg_p;             // pointer to the 'CurrentSegment'
                                            // struct

    char              *d_scratchBuf_p;      // scratch buffer

    char              *d_symbolBuf_p;       // scratch space for symbols

    bool               d_demangle;          // whether we demangle names

    bslma_Allocator   *d_allocator_p;       // allocator used to supply memory,
                                            // held, not owned

  private:
    // NOT IMPLEMENTED
    baesu_StackTraceResolverImpl(const baesu_StackTraceResolverImpl&);
    baesu_StackTraceResolverImpl& operator=(
                                          const baesu_StackTraceResolverImpl&);

  private:
    // PRIVATE CREATORS
    baesu_StackTraceResolverImpl(
                           bsl::vector<baesu_StackTraceFrame> *ioFrames_p,
                           bool                                demangle,
                           bslma_Allocator                    *basicAllocator);
        // Create an stack trace reolver that can populate other fields of the
        // specified 'ioFrames' object given previously populated 'address'
        // fields.  Specify 'demangle', which indicates whether demangling of
        // symbols is to occur, and 'basicAllocator', which is to be used for
        // memory allocation.  Note that the behavior is undefined if
        // 'basicAllocator' is 0 or unspecified, the intention is that it
        // should be of type 'bdema_HeapByPassAllocator'.

    ~baesu_StackTraceResolverImpl();
        // Destroy this object.

    // PRIVATE MANIPULATORS
    int loadSymbols();
        // Read the symbols from the symbol table of the current segment and
        // update the 'mangledSymbolName', 'symbolName', 'offsetFromSymbol',
        // and sometimes the 'SourceFileName' fields of stack frames constain
        // addresses within the code section of the current segment.  Return 0
        // on success and a non-zero value otherwise.

    int resolveSegment(void       *segmentBaseAddress,
                       void       *segmentPtr,
                       UintPtr     segmentSize,
                       const char *libraryFileName);
        // Identify which stack trace frames in '*d_allFrames_p' are in the
        // segment pointed at by the specified 'segmentPtr' of the specified
        // 'segmentSize', and initialize as many fields of those stack trace
        // frames as possible.  The segment is defined in the executable file
        // or shared library 'libraryFileName'.  Return 0 on success and a
        // non-zero value otherwise.

    // PRIVATE ACCESSORS
    void setFrameSymbolName(baesu_StackTraceFrame *frame) const;
        // Set the 'symbolName' field of the specified 'frame', which must
        // already have the 'mangledSymbolName' field set, to the demangled
        // version of the 'mangledSymbolName' field.  If 'd_demangle' is
        // 'false' or we are otherwise unable to demangle, just set it to the
        // same as 'mangledSymbolName'.

  public:
    // CLASS METHOD
    static int resolve(bsl::vector<baesu_StackTraceFrame> *ioFrames,
                       bool                                demangle,
                       bslma_Allocator                    *basicAllocator);
        // Populate information for the specified 'ioFrames', a vector of stack
        // trace frames in a stack trace object.  Specify 'demangle', to
        // determine whether demangling is to occur, and 'basicAllocator',
        // which is to be used for memory allocation.  The behavior is
        // undefined unless all the 'address' field in 'stackFrames' are valid
        // and other fields are invalid, and 'basicAllocator != 0'.

    // MANIPULATOR
    int processLoadedImage(const char *fileName,
                           const void *programHeaders,
                           int         numProgramHeaders,
                           void       *textSegPtr,
                           void       *baseAddress);
        // Process a loaded image found via the link map, either the main
        // program or some shared library.  The specified 'fileName' is the
        // name of the file containing the image.  If 'fileName == 0', the file
        // is the main program.  The specified 'programHeaders' is a pointer to
        // an array of elf program headers and the specified
        // 'numProgramHeaders' is its length, it is a 'void *' because the type
        // 'ElfProgramHeader' is local to the implementation file.  Specify one
        // of 'textSegPtr' and 'baseAddress', and the other as 0, this method
        // will infer the one specified as 0 from the other.  Return 0 on
        // success and a non-zero value otherwise.  Note that this method is
        // not to be called by external users of this component, it is only
        // public so a static routine in the implementation file can call it.

    static inline
    int testFunc();
        // For testing only.  Do some random garbage and return a line number
        // within this routine.
};

                         // ----------------------------------
                         // class baesu_StackTraceResolverImpl
                         // ----------------------------------

inline
int baesu_StackTraceResolverImpl<baesu_ObjectFileFormat::Elf>::testFunc()
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
