// baesu_stacktraceutil.cpp                                           -*-C++-*-
#include <baesu_stacktraceutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktraceutil_cpp,"$Id$ $CSID$")

#include <baesu_objectfileformat.h>
#include <baesu_stackaddressutil.h>
#include <baesu_stacktraceframe.h>
#include <baesu_stacktraceresolverimpl_elf.h>
#include <baesu_stacktraceresolverimpl_xcoff.h>
#include <baesu_stacktraceresolverimpl_windows.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorguard.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_ostream.h>

#if defined(BSLS_PLATFORM__OS_WINDOWS) && defined(BDE_BUILD_TARGET_OPT)
// 'getStackAddresses' will not be able to trace through our stack frames if
// we're optimized on Windows

#pragma optimize("", off)
#endif

static
const char *findBasename(const char *pathName)
    // Find in the specified 'pathName' the first character following the last
    // pathName separator character (i.e., '/' or '\\') and return its address.
    // If there are no separator characters, return the address of the first
    // character of 'pathName'.  If the last character of 'pathName' is a
    // separator charactor, return the address of the terminating '\0'.
{
    const char *ptr = pathName + bsl::strlen(pathName);

#ifdef BSLS_PLATFORM__OS_WINDOWS
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

int baesu_StackTraceUtil::loadStackTraceFromAddressArray(
                                   baesu_StackTrace   *result,
                                   const void * const  addresses[],
                                   int                 numAddresses,
                                   bool                demanglingPreferredFlag)
{
    BSLS_ASSERT(numAddresses >= 0);
    BSLS_ASSERT(0 == numAddresses || 0 != addresses);

    typedef baesu_ObjectFileFormat::Policy Policy;
    typedef baesu_StackTraceResolverImpl<Policy> Resolver;

    result->removeAll();
    result->resize(numAddresses);

    for (int i = 0; i < numAddresses; ++i) {
        (*result)[i].setAddress(addresses[i]);
    }

    return Resolver::resolve(result,
                             demanglingPreferredFlag);
}

int baesu_StackTraceUtil::loadStackTraceFromStack(
                                     baesu_StackTrace *result,
                                     int               maxFrames,
                                     bool              demanglingPreferredFlag)
{
    enum {
        DEFAULT_MAX_FRAMES = 1024,
        IGNORE_FRAMES      = baesu_StackAddressUtil::BAESU_IGNORE_FRAMES + 1
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
    bslma_DeallocatorGuard<bslma_Allocator> guard(addresses,
                                                  result->allocator());

    int numAddresses = baesu_StackAddressUtil::getStackAddresses(addresses,
                                                                 maxFrames);
    if (numAddresses <= 0 || numAddresses > maxFrames) {
        return -1;                                                    // RETURN
    }

    return loadStackTraceFromAddressArray(result,
                                          addresses    + IGNORE_FRAMES,
                                          numAddresses - IGNORE_FRAMES,
                                          demanglingPreferredFlag);
}

bsl::ostream& baesu_StackTraceUtil::printFormatted(
                                            bsl::ostream&           stream,
                                            const baesu_StackTrace& stackTrace)
{
    for (int i = 0; i < stackTrace.length(); ++i) {
        stream << '(' << i << "): ";
        printFormatted(stream, stackTrace[i]);
        stream << bsl::endl;
    }

    return stream;
}

bsl::ostream& baesu_StackTraceUtil::printFormatted(
                                  bsl::ostream&                stream,
                                  const baesu_StackTraceFrame& stackTraceFrame)
{
    enum { LIBRARY_NAME_LIMIT = 40 };   // Library file names longer than
                                        // this are just printed as the
                                        // basename, otherwise the full path is
                                        // printed.

    // Choose from 'symbolName', 'mangledSymbolName', and "--unknown--",
    // in that order, according to availablity.

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
        stream << " at 0x" << (bsls_Types::UintPtr) stackTraceFrame.address();
    }
    stream.flags(save);

    if (stackTraceFrame.isSourceFileNameKnown()) {
        //  #if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF)
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

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
