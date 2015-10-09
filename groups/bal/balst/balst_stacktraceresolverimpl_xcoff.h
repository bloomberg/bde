// balst_stacktraceresolverimpl_xcoff.h                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_STACKTRACERESOLVERIMPL_XCOFF
#define INCLUDED_BALST_STACKTRACERESOLVERIMPL_XCOFF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility to resolve xcoff symbols in a stack trace.
//
//@CLASSES:
//   balst::StackTraceResolverImpl<Xcoff>: symbol resolution for Xcoff objects
//
//@SEE_ALSO: balst_stacktraceresolverimpl_elf,
//           balst_stacktraceresolverimpl_windows
//
//@DESCRIPTION: This component provides a class,
// balst::StackTraceResolver<Xcoff>, that, given a vector of
// 'balst::StackTraceFrame's that have only their 'address' fields set,
// resolves all other fields in those frames.  Xcoff objects are used on AIX
// platforms.
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

#if defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)

#ifndef INCLUDED_BALST_STACKTRACE
#include <balst_stacktrace.h>
#endif

#ifndef INCLUDED_BALST_STACKTRACEFRAME
#include <balst_stacktraceframe.h>
#endif

#ifndef INCLUDED_BALST_STACKTRACERESOLVER_FILEHELPER
#include <balst_stacktraceresolver_filehelper.h>
#endif

#ifndef INCLUDED_BSLMT_QLOCK
#include <bslmt_qlock.h>
#endif

#ifndef INCLUDED_BDLMA_HEAPBYPASSALLOCATOR
#include <bdlma_heapbypassallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

// These 2 symbols are needed by 'syms.h'.

#undef __XCOFF64__
#undef __XCOFF32__

#ifdef BSLS_PLATFORM_CPU_64_BIT
# define __XCOFF64__
#else
# define __XCOFF32__
#endif

#ifndef INCLUDED_SYMS
#include <syms.h>               // SYMENT, AUXENT
#define INCLUDED_SYMS
#endif

#define BALST_STACKTRACERESOLVERIMPL_XCOFF_LINE __LINE__

namespace BloombergLP {

namespace balst {

template <typename RESOLVER_POLICY>
class StackTraceResolverImpl;

           // =====================================================
           // class StackTraceResolverImpl<ObjectFileFormat::Xcoff>
           // =====================================================

template <>
class StackTraceResolverImpl<ObjectFileFormat::Xcoff> {
    // This class is for resolving symbols in Xcoff executables.  Given a
    // vector of 'StackTraceFrame's that have only their 'address' fields
    // set, it resolves all other fields in those frames.  Xcoff objects are
    // used on AIX platforms.  Note that all methods, including the
    // constructor, are private except for that static method 'resolve' which
    // constructs and destroys the object.

    // TYPES
    struct AuxInfo;                       // Internal -- fleshed out in the
                                          // implementation file
    struct LoadAuxInfosInfo;              // Internal -- fleshed out in the
                                          // implementation file
    typedef bsls::Types::UintPtr UintPtr; // 32 bit unsigned on 32 bit, 64 bit
                                          // unsigned on 64 bit, usually used
                                          // for absolute offsets into a file

    // CONSTANTS
    enum FindIncludeFileFlags {
        // flags returned by 'findIncludeFile'

        k_FOUND_INCLUDE_FILE      = 0x1,
        k_LINE_NUMBER_IS_ABSOLUTE = 0x2
    };

    // DATA
    StackTraceResolver_FileHelper
                          *d_helper;          // helper for reading files

    StackTrace      *d_stackTrace_p;    // pointer to stack trace object
                                              // to be populated by resolution.
                                              // Note only the 'address' fields
                                              // are initialized at the start,
                                              // our goal is to initialize all
                                              // the other fields.  Held, not
                                              // owned.

    StackTraceFrame
                         **d_segFramePtrs_p;  // pointers to stack trace frames
                                              // contained in 'd_stackTrace_p'
                                              // listing only those frames
                                              // whose 'address' fields point
                                              // into the current segment

    const void           **d_segAddresses_p;  // the 'address' fields from
                                              // 'd_segFramePtrs_p' in 1-1
                                              // correspondence, note this
                                              // duplication of information is
                                              // a performance optimization

    AuxInfo               *d_segAuxInfos_p;   // array of aux infos (AuxInfo is
                                              // a struct private to this
                                              // class, defined in the imp) in
                                              // 1-1 correspondence with the
                                              // entries of 'd_segFramePtrs_p'

    int                    d_numCurrAddresses;// number of 'address' fields in
                                              // 'd_stackTrace_p' that point
                                              // into the current segment, also
                                              // the length of
                                              // 'd_segFramePtrs_p',
                                              // 'd_segAddresses_p', and
                                              // 'd_segAuxInfos_p', note all 3
                                              // are allocated to have
                                              // 'd_stackTrace_p->length()'
                                              // (worst case) length

    char                  *d_scratchBuf_p;    // scratch buffer

    char                  *d_symbolBuf_p;     // buffer for reading symbols

    bsls::Types::IntPtr    d_virtualToPhysicalOffset;
                                              // translation from an address
                                              // given in the file to an
                                              // address in memory for the
                                              // current segment

    UintPtr                d_archiveMemberOffset;
                                              // archive member offset, or 0 if
                                              // the segment is not an archive
                                              // member

    UintPtr                d_archiveMemberSize;
                                              // archive member size, or size
                                              // of the whole file if the
                                              // segment is not an archive
                                              // member

    UintPtr                d_symTableOffset;  // absolute offset of symbol
                                              // table in the current file

    UintPtr                d_stringTableOffset;
                                              // absolute offset of string
                                              // table in the current file

    bool                   d_demangle;        // flag indicating whether
                                              // demangling is to be done

    bdlma::HeapBypassAllocator
                           d_hbpAlloc;        // heap bypass allocator, all
                                              // memory allocated by this
                                              // object will be freed when this
                                              // allocator is destroyed.

    static bslmt::QLock     s_demangleQLock;   // 'QLock' to guard access to
                                              // the non-thread-safe 'Demangle'
                                              // function.

  private:
    // NOT IMPLEMENTED
    StackTraceResolverImpl(const StackTraceResolverImpl&);
    StackTraceResolverImpl& operator=(
                                          const StackTraceResolverImpl&);

    // PRIVATE CREATORS
    StackTraceResolverImpl(StackTrace *stackTrace,
                                 bool              demangle);
        // Create an stack trace reolver that can populate other fields of the
        // specified 'stackFrames' object given previously populated 'address'
        // fields.  Specify 'demangle', which indicates whether demangling of
        // symbols is to occur, and 'basicAllocator', which is to be used for
        // memory allocation.  Note that the behavior is undefined if
        // 'basicAllocator' is 0 or unspecified, the intention is that it
        // should be of type 'bdema::HeapByPassAllocator'.

    ~StackTraceResolverImpl();
        // Destroy this stack trace resolver object.

    // PRIVATE MANIPULATORS
    bslma::Allocator *allocator();
        // Return a pointer to this object's heap bypass allocator.

    int findArchiveMember(const char *memberName);
        // Locate the archive member with the specified 'memberName' in the
        // current archive file and save the member's offset from the beginning
        // of the archive and the member's size.  Return zero on success, and a
        // negative value otherwise.  Note that this is never called on an
        // executable, only on archives.

    UintPtr findCsectIndex(const char *symbolAddress,
                           const char *csectEndAddress,
                           UintPtr     primarySymIndex);
        // Iterate through all the addresses in d_segAddresses_p, returning the
        // specified 'primarySymIndex', which is the index of the current
        // symbol, if any of them are in the range
        // '[symbolAddress, csectEndAddress)' and 'UintPtr(-1)' otherwise.

    int findIncludeFile(SYMENT  *includeSymEnt,
                        UintPtr  firstLineNumberOffset,
                        UintPtr  lineNumberOffset,
                        UintPtr  symStartIndex,
                        UintPtr  symEndIndex);
        // Read the portion of the symbol table of the current segment starting
        // at the specified 'symStartIndex' and ending at the specified
        // 'symEndIndex' to determine if the specified 'lineNumberOffset' is in
        // an include file.  Return a positive value which is a bitwise or of
        // the appropriate 'FindIncludeFileFlags' if found, 0 if not found, and
        // a negative value if an error is encountered.  The positive value
        // returned on success is a bitwise or of the flags defined by enum
        // 'FindIncludeFileFlags' defined in the class, indicating whether the
        // file was found, and whether the line number we have (which is not
        // passed to this routine) is absolute or relative.  Note that the line
        // numbers corresponding to include files are sometimes absolute, while
        // other line numbers are relative to the beginning of the function in
        // which they occur.  Also note that this routine is called immediately
        // after 'findLineNumber'.

    int findLineNumber(int        *outLineNumber_p,
                       UintPtr    *outLineNumberOffset_p,
                       UintPtr     lineBufStartOffset,
                       const void *segAddress);
        // Find a line number and line number offset of the source that refers
        // to the specified 'address', and load the results into the specified
        // '*outLineNumber_p' and '*outLineNumberOffset_p'.  Begin the search
        // at the specified 'lineBufStartOffset' in the file and end either at
        // the end of that function, at the end of the archive member, or the
        // end of the file.  Return zero on success, and a nonzero value
        // otherwise.  Note that the line number may be either relative to the
        // beginning of the function, or absolute (see 'findIncludeFile').
        // Note that 'lineBufStartOffset' points to the beginning of line
        // number records describing the function containing the code
        // 'segAddress' refers to.

    void loadAuxInfos(const LoadAuxInfosInfo *laiInfo,
                      const char             *functionBeginAddress,
                      const char             *functionEndAddress);
        // Iterate through 'd_segAddresses_p' and, for each address that refers
        // to code in the function specified by 'functionBeginAddress' and
        // 'functionEndAddress', initialize the 'offsetFromSymbol' field of the
        // corresponding stack trace frame, and initialize the corresponding
        // 'AuxInfo' struct with information from variables local to the
        // calling 'loadSymbols' function accessed through pointers in the
        // specified 'laiInfo' struct.

    int loadSymbols(UintPtr numSyms,
                    int     textSectionNum);
        // Read the specified 'numSym' symbols from the symbol table associated
        // with this segment, skipping those symbols not associated with the
        // text section indicated by the specified 'textSectionNum'.  Return 0
        // on success and a non-zero value otherwise.

    const char *getSourceName(const AUXENT *auxent);
        // Allocate memory for, and return a pointer to, a string containing
        // the name of the source file referred to by the specified 'auxent'.

    const char *getSymbolName(const SYMENT *syment);
        // Allocate memory for, and return a pointer to, a string containing
        // the name of the symbol defined by the specified 'syment'.  Note the
        // symbol is sometimes a function name, sometimes a source file name.

    int resolveSegment(void       *segmentPtr,
                       UintPtr     segmentSize,
                       const char *libraryFileName,
                       const char *displayFileName,
                       const char *archiveMemberName);
        // Populate those stack trace frames whose 'address' fields reside
        // within the segment specfied by 'segmentPtr' and 'segmentSize'.  The
        // segment is in the specified 'libraryFileName', with the specified
        // 'archiveMemberName'.  Specify the 'displayFileName' used to identify
        // the library file name in the stack trace.  Note that
        // 'displayFileName' may be different from 'libraryFileName' because
        // AIX truncates the filename of the executable file to be 32 chars
        // long, so we use another means to open the executable file.  Note
        // that if 'archiveMemberName' is unspecified, the whole library file
        // has a single segment.

  public:
    // PUBLIC CLASS METHODS
    static
    int resolve(StackTrace *stackTrace,
                bool              demangle);
        // Populate information for the specified 'stackFrames', a vector of
        // stack trace frames in a stack trace object.  Specify 'demangle', to
        // determine whether demangling is to occur, and 'basicAllocator',
        // which is to be used for memory allocation.  The behavior is
        // undefined unless all the 'address' field in 'stackFrames' are valid
        // and other fields are invalid, and 'basicAllocator != 0'.

    static inline
    int testFunc();
        // For testing only.  Do some random garbage and return a line number
        // from within the routine.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // class StackTraceResolverImpl
                        // ----------------------------

// PRIVATE MANIPULATORS
inline
bslma::Allocator *StackTraceResolverImpl<
                                    ObjectFileFormat::Xcoff>::allocator()
{
    return &d_hbpAlloc;
}

// CLASS METHODS
inline
int StackTraceResolverImpl<ObjectFileFormat::Xcoff>::testFunc()
{
    // Do some random garbage to generate some code, then return a line number
    // within this routine

    int line = 0, lineCopy = 0;

    for (int i = 0; true; ++i) {
        BSLS_ASSERT_OPT(line == lineCopy);

        const int loopGuard = 0x8edf0000;    // garbage with a lot of trailing
                                             // 0's.
        const int mask      = 0xa72c3dca;    // pure garbage

        enum { k_LINE = __LINE__ };

        for (int i = 0; !(i & loopGuard); ++i) {
            line ^= (i & mask);
        }

        // The above loop will leave the value of 'line' unchanged.  See
        // 'foilOptimizer' in the test driver.

        BSLS_ASSERT_OPT(line == lineCopy);

        if (line != 0) {
            break;
        }

        line = k_LINE;
        lineCopy = line;
    }

    return line;
}
}  // close package namespace

}  // close namespace BloombergLP

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
