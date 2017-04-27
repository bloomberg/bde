// balst_stacktraceresolverimpl_elf.h                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_STACKTRACERESOLVERIMPL_ELF
#define INCLUDED_BALST_STACKTRACERESOLVERIMPL_ELF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility to resolve ELF symbols in a stack trace.
//
//@CLASSES:
//   balst::StackTraceResolverImpl<Elf>: symbol resolution for ELF objects
//
//@SEE_ALSO: balst_stacktraceresolver_dwarfreader,
//           balst_stacktraceresolverimpl_windows,
//           balst_stacktraceresolverimpl_xcoff
//
//@DESCRIPTION: This component provides a class,
// 'balst::StackTraceResolver<Elf>', that, given a vector of
// 'balst::StackTraceFrame's that have only their 'address' fields set,
// resolves all other fields in those frames.  The Elf object file format is
// used on Linux, Solaris, and HP-UX platforms.  The Elf format is described by
// documents at:
//: o 'http://en.wikipedia.org/wiki/Executable_and_Linkable_Format'
//: o 'ftp://ftp.openwatcom.org/pub/devel/docs/elf-64-gen.pdf'
//: o 'http://www.sco.com/developers/gabi/latest/contents.html'
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

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)

#ifndef INCLUDED_BALST_STACKTRACE
#include <balst_stacktrace.h>
#endif

#ifndef INCLUDED_BALST_STACKTRACEFRAME
#include <balst_stacktraceframe.h>
#endif

#ifndef INCLUDED_BALST_STACKTRACERESOLVER_FILEHELPER
#include <balst_stacktraceresolver_filehelper.h>
#endif

#ifndef INCLUDED_BDLMA_HEAPBYPASSALLOCATOR
#include <bdlma_heapbypassallocator.h>
#endif

#ifndef INCLUDED_BDLS_FILESYSTEMUTIL
#include <bdls_filesystemutil.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace balst {

template <class RESOLVER_POLICY>
class StackTraceResolverImpl;

            // ===================================================
            // class StackTraceResolverImpl<ObjectFileFormat::Elf>
            // ===================================================

template <>
class StackTraceResolverImpl<ObjectFileFormat::Elf> {
    // This class provides a public static 'resolve' method that, given a
    // vector of 'StackTraceFrame's that have only their 'address' fields set,
    // resolves as many other fields in those frames as possible.  The Elf
    // object file format is used on Linux, Solaris, and HP-UX platforms.  On
    // Linux, some Elf sections contain data in the DWARF format, which makes
    // it possible to resolve line numbers and file names.

    // TYPES
    typedef bsls::Types::UintPtr UintPtr;   // 32 bit unsigned on 32 bit, 64
                                            // bit unsigned on 64 bit.

    typedef bdls::FilesystemUtil::Offset
                                 Offset;    // Usually used for relative
                                            // offsets into a file.

    struct HiddenRec;                       // 'struct' defined locally in
                                            // in the imp file containing
                                            // additional information

    // DATA
    bdlma::HeapBypassAllocator
                       d_hbpAlloc;          // heap bypass allocator -- owned

    StackTrace        *d_stackTrace_p;      // pointer to stack trace object.
                                            // The frames contained in this
                                            // have their 'address' fields and
                                            // nothing else initialized upon
                                            // entry to 'resolve', which infers
                                            // as many other fields of them as
                                            // possible.

    char              *d_scratchBufA_p;     // scratch buffer A

    char              *d_scratchBufB_p;     // scratch buffer B

    char              *d_scratchBufC_p;     // scratch buffer C

    char              *d_scratchBufD_p;     // scratch buffer D

    HiddenRec&         d_hidden;            // reference to the 'HiddenRec'.

    bool               d_demangle;          // whether we demangle names

  private:
    // NOT IMPLEMENTED
    StackTraceResolverImpl(const StackTraceResolverImpl&);
    StackTraceResolverImpl& operator=(const StackTraceResolverImpl&);

  private:
    // PRIVATE CREATORS
    StackTraceResolverImpl(StackTrace *stackTrace,
                           bool        demanglingPreferredFlag);
        // Create an stack trace reolver that can populate other fields of the
        // specified '*stackTrace' object given previously populated 'address'
        // fields.  Specify 'demangle', which indicates whether demangling of
        // symbols is to occur.

    // ~StackTraceResolverImpl() = default;
        // Destroy this object.

    // PRIVATE MANIPULATORS
    int loadSymbols(int matched);
        // Read the symbols from the symbol table of the current segment and
        // update the 'mangledSymbolName', 'symbolName', 'offsetFromSymbol',
        // and sometimes the 'SourceFileName' fields of stack frames constain
        // addresses within the code section of the current segment, where the
        // specified 'matched' is the number of addresses in the current
        // segment.  Return 0 on success and a non-zero value otherwise.

    int resolveSegment(void       *segmentBaseAddress,
                       void       *segmentPtr,
                       UintPtr     segmentSize,
                       const char *libraryFileName);
        // Identify which stack trace frames in '*d_stackTrace_p' are in the
        // segment pointed at by the specified 'segmentPtr' of the specified
        // 'segmentSize', and initialize as many fields of those stack trace
        // frames as possible.  The segment is defined in the executable file
        // or shared library 'libraryFileName'.  Return 0 on success and a
        // non-zero value otherwise.

    // PRIVATE ACCESSORS
    void setFrameSymbolName(StackTraceFrame *frame,
                            char            *buffer,
                            bsl::size_t      bufferLen) const;
        // Set the 'symbolName' field of the specified 'frame', which must
        // already have the 'mangledSymbolName' field set, to the demangled
        // version of the 'mangledSymbolName' field.  Use the specified
        // 'buffer' of specified length 'bufferLen' for temporary storage.  If
        // 'd_demangle' is 'false' or we are otherwise unable to demangle, just
        // set it to the same as 'mangledSymbolName'.

  public:
    // CLASS METHOD
    static int resolve(StackTrace *stackTrace,
                       bool        demanglingPreferredFlag);
        // Populate information for the specified '*stackTrace', which contains
        // a sequence of randomly-accessible stack trace frames.  Specify
        // 'demanglingPreferredFlag', to determine whether demangling is to
        // occur.  The behavior is undefined unless all the 'address' field in
        // '*stackTrace' are valid and other fields are invalid.

    static void test();
        // This function is just there to test how code deals with inline
        // functions in an include file.  It does not provide any otherwise
        // useful functionality.

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

    // ACCESSOR
    int numUnmatchedFrames() const;
        // Return the number of frames in the stack trace that are still
        // unmatched.
};

inline
void StackTraceResolverImpl<ObjectFileFormat::Elf>::test()
{
    StackTrace st;
    StackTraceResolverImpl<ObjectFileFormat::Elf> resolver(&st, true);

    (void) resolver.numUnmatchedFrames();
}

}  // close package namespace
}  // close enterprise namespace

#endif
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
