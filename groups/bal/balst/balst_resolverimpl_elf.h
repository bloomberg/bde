// balst_resolverimpl_elf.h                                           -*-C++-*-
#ifndef INCLUDED_BALST_RESOLVERIMPL_ELF
#define INCLUDED_BALST_RESOLVERIMPL_ELF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility to resolve ELF symbols in a stack trace.
//
//@CLASSES:
//   balst::ResolverImpl<Elf>: symbol resolution for ELF objects
//
//@SEE_ALSO: balst_resolver_dwarfreader,
//           balst_resolverimpl_windows,
//           balst_resolverimpl_xcoff
//
//@DESCRIPTION: This component provides a class,
// `balst::Resolver<Elf>`, that, given a vector of
// `balst::StackTraceFrame`s that have only their `address` fields set,
// resolves all other fields in those frames.  The Elf object file format is
// used on Linux and Solaris platforms.  The Elf format is described by
// documents at:
// * `http://en.wikipedia.org/wiki/Executable_and_Linkable_Format`
// * `ftp://ftp.openwatcom.org/pub/devel/docs/elf-64-gen.pdf`
// * `http://www.sco.com/developers/gabi/latest/contents.html`
//
///Usage
///-----
// This component is an implementation detail of `balst` and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <balscm_version.h>

#include <balst_objectfileformat.h>

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)
#include <balst_stacktrace.h>
#include <balst_stacktraceframe.h>
#include <balst_resolver_filehelper.h>

#include <bdlma_heapbypassallocator.h>

#include <bdls_filesystemutil.h>

#include <bsls_types.h>

#include <bsl_vector.h>

namespace BloombergLP {
namespace balst {

template <class RESOLVER_POLICY>
class ResolverImpl;

                    // =========================================
                    // class ResolverImpl<ObjectFileFormat::Elf>
                    // =========================================

/// This class provides a public static `resolve` method that, given a
/// vector of `StackTraceFrame`s that have only their `address` fields set,
/// resolves as many other fields in those frames as possible.  The Elf
/// object file format is used on Linux and Solaris platforms.  On Linux,
/// some Elf sections contain data in the DWARF format, which makes it
/// possible to resolve line numbers and file names.
template <>
class ResolverImpl<ObjectFileFormat::Elf> {

    // TYPES
    typedef bsls::Types::UintPtr UintPtr;   // 32 bit unsigned on 32 bit, 64
                                            // bit unsigned on 64 bit.
    typedef bsls::Types::IntPtr  IntPtr;    // 32 bit signed on 32 bit, 64
                                            // bit signed on 64 bit.

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

    char              *d_scratchBufE_p;     // scratch buffer E

    HiddenRec&         d_hidden;            // reference to the 'HiddenRec'.

    bool               d_demangle;          // whether we demangle names

  private:
    // NOT IMPLEMENTED
    ResolverImpl(const ResolverImpl&);
    ResolverImpl& operator=(const ResolverImpl&);

  private:
    // PRIVATE CREATORS

    /// Create an stack trace reolver that can populate other fields of the
    /// specified `*stackTrace` object given previously populated `address`
    /// fields.  Specify `demangle`, which indicates whether demangling of
    /// symbols is to occur.
    ResolverImpl(StackTrace *stackTrace,
                 bool        demanglingPreferredFlag);

    /// Destroy this object.
    //! ~ResolverImpl() = default;

    // PRIVATE MANIPULATORS

    /// Read the symbols from the symbol table of the current segment and
    /// update the `mangledSymbolName`, `symbolName`, `offsetFromSymbol`,
    /// and sometimes the `SourceFileName` fields of stack frames constain
    /// addresses within the code section of the current segment, where the
    /// specified `matched` is the number of addresses in the current
    /// segment.  Return 0 on success and a non-zero value otherwise.
    int loadSymbols(int matched);

    /// Identify which stack trace frames in `*d_stackTrace_p` are in the
    /// segment pointed at by the specified `segmentPtr` of the specified
    /// `segmentSize`, and initialize as many fields of those stack trace
    /// frames as possible.  The segment is defined in the executable file
    /// or shared library `libraryFileName`.  Return 0 on success and a
    /// non-zero value otherwise.
    int resolveSegment(void       *segmentBaseAddress,
                       void       *segmentPtr,
                       UintPtr     segmentSize,
                       const char *libraryFileName);

    // PRIVATE ACCESSORS

    /// Set the `symbolName` field of the specified `frame`, which must
    /// already have the `mangledSymbolName` field set, to the demangled
    /// version of the `mangledSymbolName` field.  Use the specified
    /// `buffer` of specified length `bufferLen` for temporary storage.  If
    /// `d_demangle` is `false` or we are otherwise unable to demangle, just
    /// set it to the same as `mangledSymbolName`.
    void setFrameSymbolName(StackTraceFrame *frame,
                            char            *buffer,
                            bsl::size_t      bufferLen) const;

  public:
    // CLASS METHOD

    /// Populate information for the specified `*stackTrace`, which contains
    /// a sequence of randomly-accessible stack trace frames.  Specify
    /// `demanglingPreferredFlag`, to determine whether demangling is to
    /// occur.  The behavior is undefined unless all the `address` field in
    /// `*stackTrace` are valid and other fields are invalid.
    static int resolve(StackTrace *stackTrace,
                       bool        demanglingPreferredFlag);

    /// This function is just there to test how code deals with inline
    /// functions in an include file.  It does not provide any otherwise
    /// useful functionality.  Return a line number near the beginning of
    /// the function in the low-order 14 bits of the result.  Other bits of
    /// the result are to be considered garbage.
    static int test();

    // MANIPULATOR

    /// Process a loaded image found via the link map, either the main
    /// program or some shared library.  The specified `fileName` is the
    /// name of the file containing the image.  If `fileName == 0`, the file
    /// is the main program.  The specified `programHeaders` is a pointer to
    /// an array of elf program headers and the specified
    /// `numProgramHeaders` is its length, it is a `void *` because the type
    /// `ElfProgramHeader` is local to the implementation file.  Specify one
    /// of `textSegPtr` and `baseAddress`, and the other as 0, this method
    /// will infer the one specified as 0 from the other.  Return 0 on
    /// success and a non-zero value otherwise.  Note that this method is
    /// not to be called by external users of this component, it is only
    /// public so a static routine in the implementation file can call it.
    int processLoadedImage(const char *fileName,
                           const void *programHeaders,
                           int         numProgramHeaders,
                           void       *textSegPtr,
                           void       *baseAddress);

    // ACCESSOR

    /// Return the number of frames in the stack trace that are still
    /// unmatched.
    int numUnmatchedFrames() const;
};

inline
int ResolverImpl<ObjectFileFormat::Elf>::test()
{

    StackTrace st;

    int ret = __LINE__;
    ResolverImpl<ObjectFileFormat::Elf> resolver(&st, true);

    return (resolver.numUnmatchedFrames() << 14) | ret;
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
