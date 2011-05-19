// baesu_stacktraceframe.cpp                                          -*-C++-*-
#include <baesu_stacktraceframe.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stacktraceframe_cpp,"$Id$ $CSID$")

#include <baesu_objectfileformat.h>

#include <bdeu_print.h>

#include <bsls_platform.h>
#include <bsls_platformutil.h>
#include <bslim_printer.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                       // ---------------------------
                       // class baesu_StackTraceFrame
                       // ---------------------------

// ACCESSORS
bsl::ostream& baesu_StackTraceFrame::print(bsl::ostream& stream,
                                           int           level,
                                           int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }
    bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha;

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(d_address_p,                 "address");
    printer.print(d_libraryFileName.c_str(),   "library file name");
    printer.print(d_lineNumber,                "line number");
    printer.print(d_offsetFromSymbol,          "offset from symbol");
    printer.print(d_sourceFileName.c_str(),    "source file name");
    printer.print(d_mangledSymbolName.c_str(), "mangled symbol name");
    printer.print(d_symbolName.c_str(),        "symbol name");
    printer.end();

    stream.flags(fmtFlags);
    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baesu_StackTraceFrame& frame)
{
    // Note that we don't print out the mangled symbol name.  If demangling did
    // not happen, the 'symbolName' value will also have the mangled symbol
    // name.

    if (frame.isSymbolNameValid()) {
        stream << frame.symbolName();
    }
    else {
        if (frame.isMangledSymbolNameValid()) {
            stream << frame.mangledSymbolName();
        }
        else {
            stream << " --unknown--";
        }
    }
    stream << bsl::hex;
    if (frame.isOffsetFromSymbolValid()) {
        stream << "+0x" << frame.offsetFromSymbol();
    }
    if (frame.isAddressValid()) {
        stream << " at 0x" << (bsls_PlatformUtil::UintPtr) frame.address();
    }
    stream << bsl::dec;
    if (frame.isSourceFileNameValid()) {
#if defined(BAESU_OBJECTFILEFORMAT_RESOLVER_ELF)
        // source file info is only available for statics on Elf

        stream << " static";
#endif
        // point 'pc' to the basename

        const char *pcB = frame.sourceFileName().c_str();
        const char *pc = pcB + bsl::strlen(pcB);
        while (pc > pcB && '/' != pc[-1] && '\\' != pc[-1]) {
            --pc;
        }

        stream << " source:" << pc;
        if (frame.isLineNumberValid()) {
            stream << ":" << frame.lineNumber();
        }
    }
    if (frame.isLibraryFileNameValid()) {
        // unless it's a shared library, point 'pc' to the basename

        const char *pcB = frame.libraryFileName().c_str();
        const char *pc = pcB + bsl::strlen(pcB);
#ifdef BSLS_PLATFORM__OS_WINDOWS
        const bool isUnix = false;
#else
        const bool isUnix = true;
#endif
        if ((!isUnix && pc >= pcB + 4 && bsl::strcmp(pc-4, ".DLL")) ||
            ( isUnix && pc >= pcB + 2 && !bsl::strstr(pcB, ".so") &&
                        bsl::strcmp(pc-2, ".a") && !bsl::strstr(pcB, ".ko"))) {
            while (pc > pcB && '/' != pc[-1] && '\\' != pc[-1]) {
                --pc;
            }
        }
        else {
            pc = pcB;
        }

        stream << " in " << pc;
    }

    return stream;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
