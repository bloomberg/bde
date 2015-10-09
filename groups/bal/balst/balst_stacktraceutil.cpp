// balst_stacktraceutil.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stacktraceutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stacktraceutil_cpp,"$Id$ $CSID$")

#include <balst_objectfileformat.h>
#include <balst_stackaddressutil.h>
#include <balst_stacktraceframe.h>
#include <balst_stacktraceresolverimpl_dladdr.h>
#include <balst_stacktraceresolverimpl_elf.h>
#include <balst_stacktraceresolverimpl_xcoff.h>
#include <balst_stacktraceresolverimpl_windows.h>

#include <bdlma_heapbypassallocator.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorguard.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ios.h>
#include <bsl_ostream.h>

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_OPT)
// 'getStackAddresses' will not be able to trace through our stack frames if
// we're optimized on Windows

#pragma optimize("", off)
#endif

static
const char *findBasename(const char *pathName)
    // Find in the specified 'pathName' the first character following the last
    // 'pathName' separator character (i.e., '/' or '\\') and return its
    // address.  If there are no separator characters, return the address of
    // the first character of 'pathName'.  If the last character of 'pathName'
    // is a separator charactor, return the address of the terminating '\0'.
{
    const char *ptr = pathName + bsl::strlen(pathName);

#ifdef BSLS_PLATFORM_OS_WINDOWS
    const char separator = '\\';
#else
    const char separator = '/';
#endif

    while (pathName < ptr && separator != ptr[-1]) {
        --ptr;
    }

    return ptr;
}

namespace BloombergLP {


namespace balst {

template <class RESOLVER_POLICY>
class StackTraceResolverImpl;

template <>
class StackTraceResolverImpl<ObjectFileFormat::Dummy>
{
  public:
    // PUBLIC CLASS METHODS
    static
    int resolve(StackTrace *,    // 'stackTrace'
                bool        )    // 'demangle'
        // Populate information for the specified 'stackFrames', a vector of
        // stack trace frames in a stack trace object.  Specify 'demangle', to
        // determine whether demangling is to occur, and 'basicAllocator',
        // which is to be used for memory allocation.  The behavior is
        // undefined unless all the 'address' field in 'stackFrames' are valid
        // and other fields are invalid, and 'basicAllocator != 0'.
    {
        return -1;
    }

    static inline
    int testFunc()
        // For testing only.  Do some random garbage and return a line number
        // from within the routine.
    {
        return -1;
    }
};

bsl::ostream& StackTraceUtil::hexStackTrace(bsl::ostream &stream)
{
    // This routine is just calling 'printHexStackTrace' with two additional
    // function calls on top of that -- the '<<' that this function pointer is
    // is passed to, and this function itself.  However, even in a
    // non-optimized build, the '<<' call may be inlined.  Furthermore, in
    // Solaris optimized builds, the call to 'printHexStackTrace' is called
    // through chaining.  It is better to err on the side of printing too many
    // than too few stack frames.  So in optimized builds we will pass 0 to
    // 'additionalIgnoreFrames', and in debug builds we will pass 1.

#if defined(BDE_BUILD_TARGET_OPT)
    return printHexStackTrace(stream, ' ', -1, 0);
#else
    return printHexStackTrace(stream, ' ', -1, 1);
#endif
}

int StackTraceUtil::loadStackTraceFromAddressArray(
                                   StackTrace         *result,
                                   const void * const  addresses[],
                                   int                 numAddresses,
                                   bool                demanglingPreferredFlag)
{
    BSLS_ASSERT(numAddresses >= 0);
    BSLS_ASSERT(0 == numAddresses || 0 != addresses);

    typedef ObjectFileFormat::Policy Policy;
    typedef StackTraceResolverImpl<Policy> Resolver;

    result->removeAll();
    result->resize(numAddresses);

    for (int i = 0; i < numAddresses; ++i) {
        (*result)[i].setAddress(addresses[i]);
    }

    return Resolver::resolve(result,
                             demanglingPreferredFlag);
}

int StackTraceUtil::loadStackTraceFromStack(
                                           StackTrace *result,
                                           int         maxFrames,
                                           bool        demanglingPreferredFlag)
{
    enum {
        DEFAULT_MAX_FRAMES = 1024,
        IGNORE_FRAMES      = StackAddressUtil::k_IGNORE_FRAMES + 1
    };

    if (maxFrames < 0) {
        maxFrames = DEFAULT_MAX_FRAMES;
    }

    // The value 'IGNORE_FRAMES' indicates the number of additional frames to
    // be ignored because they contained function calls within the stack trace
    // facility.

    maxFrames += IGNORE_FRAMES;

    void **addresses = (void **)
                     result->allocator()->allocate(maxFrames * sizeof(void *));
    bslma::DeallocatorGuard<bslma::Allocator> guard(addresses,
                                                   result->allocator());

#if !defined(BSLS_PLATFORM_OS_CYGWIN)
    int numAddresses = StackAddressUtil::getStackAddresses(addresses,
                                                                 maxFrames);
#else
    int numAddresses = 0;
#endif
    if (numAddresses <= 0 || numAddresses > maxFrames) {
        return -1;                                                    // RETURN
    }

    return loadStackTraceFromAddressArray(result,
                                          addresses    + IGNORE_FRAMES,
                                          numAddresses - IGNORE_FRAMES,
                                          demanglingPreferredFlag);
}

bsl::ostream& StackTraceUtil::printFormatted(bsl::ostream&     stream,
                                             const StackTrace& stackTrace)
{
    for (int i = 0; i < stackTrace.length(); ++i) {
        stream << '(' << i << "): ";
        printFormatted(stream, stackTrace[i]);
        stream << bsl::endl;
    }

    return stream;
}

bsl::ostream& StackTraceUtil::printFormatted(
                                        bsl::ostream&          stream,
                                        const StackTraceFrame& stackTraceFrame)
{
    enum { LIBRARY_NAME_LIMIT = 40 };   // Library file names longer than this
                                        // are just printed as the basename,
                                        // otherwise the full path is printed.

    // Choose from 'symbolName', 'mangledSymbolName', and "--unknown--", in
    // that order, according to availability.

    const bool isnk  = stackTraceFrame.isSymbolNameKnown();
    const bool imsnk = stackTraceFrame.isMangledSymbolNameKnown();

    stream << (isnk ? stackTraceFrame.symbolName().c_str()
                    : imsnk ? stackTraceFrame.mangledSymbolName().c_str()
                            :  "--unknown--");

    bsl::ios_base::fmtflags save = stream.flags();
    stream << bsl::hex;
    if (stackTraceFrame.isOffsetFromSymbolKnown()) {
        stream << "+0x" << stackTraceFrame.offsetFromSymbol();
    }
    if (stackTraceFrame.isAddressKnown()) {
        stream << " at 0x" << (bsls::Types::UintPtr) stackTraceFrame.address();
    }
    stream.flags(save);

    if (stackTraceFrame.isSourceFileNameKnown()) {
        //  #if defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)
        //      // source file info is only available for statics on Elf
        //
        //      stream << " static";
        //  #endif

        const char *basename =
                        findBasename(stackTraceFrame.sourceFileName().c_str());
        stream << " source:" << (*basename ? basename : "--unknown--");

        if (stackTraceFrame.isLineNumberKnown()) {
            stream << ":" << stackTraceFrame.lineNumber();
        }
    }

    if (stackTraceFrame.isLibraryFileNameKnown()) {
        stream << " in " <<
               (LIBRARY_NAME_LIMIT > stackTraceFrame.libraryFileName().length()
                    ? stackTraceFrame.libraryFileName().c_str()
                    : findBasename(stackTraceFrame.libraryFileName().c_str()));
    }

    return stream;
}

bsl::ostream& StackTraceUtil::printHexStackTrace(
                                      bsl::ostream&     stream,
                                      char              delimiter,
                                      int               maxFrames,
                                      int               additionalIgnoreFrames,
                                      bslma::Allocator *allocator)
{
    BSLS_ASSERT(0 != delimiter);

#if defined(BSLS_PLATFORM_OS_CYGWIN)
    return stream;
#else
    enum {
        DEFAULT_MAX_FRAMES = 1024
    };

    if (maxFrames < 0) {
        maxFrames = DEFAULT_MAX_FRAMES;
    }

    // The value 'ignoreFrames' indicates the number of additional frames to
    // be ignored because they contained function calls within the stack trace
    // facility.

    const int ignoreFrames = StackAddressUtil::k_IGNORE_FRAMES + 1 +
                                                        additionalIgnoreFrames;
    maxFrames += ignoreFrames;

    bdlma::HeapBypassAllocator hbpAlloc;
    if (0 == allocator) {
        allocator = &hbpAlloc;
    }

    void **addresses =
                     (void **) allocator->allocate(maxFrames * sizeof(void *));
    bslma::DeallocatorGuard<bslma::Allocator> guard(addresses, allocator);

    int numAddresses = StackAddressUtil::getStackAddresses(addresses,
                                                                 maxFrames);
    if (numAddresses <= ignoreFrames) {
        return stream;                                                // RETURN
    }

    bsl::ios_base::fmtflags saveOptions = stream.flags();
    stream << bsl::hex;

    for (int i = ignoreFrames; i < numAddresses; ++i) {
        if (i > ignoreFrames) {
            stream << delimiter;
        }

        // If we just output the addresses as pointers, the behavior would be
        // inconsistent across platforms, with some platforms printing the '0x'
        // prefix and some not.  So we print them out as an integral type
        // formatted as hex and explicitly add the "0x" to get consistent
        // behavior.

        stream << "0x" << (bsls::Types::UintPtr) addresses[i];
    }

    stream << bsl::flush;
    stream.flags(saveOptions);

    return stream;
#endif
}

}  // close package namespace
}  // close enterprise namespace

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
