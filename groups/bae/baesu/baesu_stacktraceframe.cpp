// bdesu_stacktraceframe.cpp                                          -*-C++-*-
#include <bdesu_stacktraceframe.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_stacktraceframe_cpp,"$Id$ $CSID$")

#include <bdesu_objectfileformat.h>

#include <bdeu_print.h>

#include <bsls_platform.h>
#include <bsls_platformutil.h>
#include <bslim_printer.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {

// STATIC METHODS
inline
static bool equal(const char *a, const char *b)
    // Return 'true' if the specified strings 'a' and 'b' have the same value
    // or are both null and 'false' otherwise.
{
    if      (0 == a) {
        return 0 == b;                                                // RETURN
    }
    else if (0 == b) {
        return false;                                                 // RETURN
    }
    else if (a == b) {
        return true;                                                  // RETURN
    }

    return !bsl::strcmp(a, b);
}

                       // ---------------------------
                       // class bdesu_StackTraceFrame
                       // ---------------------------

// ACCESSORS
bsl::ostream& bdesu_StackTraceFrame::print(bsl::ostream& stream,
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
    printer.print(d_address_p, "address");
    printer.print(d_libraryFileName_p, "library file name");
    printer.print(d_lineNumber, "line number");
    printer.print(d_offsetFromSymbol, "offset from symbol");
    printer.print(d_sourceFileName_p, "source file name");
    printer.print(d_mangledSymbolName_p, "mangled symbol name");
    printer.print(d_symbolName_p, "symbol name");
    printer.end();

    stream.flags(fmtFlags);
    return stream;
}

// FREE OPERATORS
bool operator==(const bdesu_StackTraceFrame& lhs,
                const bdesu_StackTraceFrame& rhs)
{
    return lhs.address() ==               rhs.address()
        && equal(lhs.libraryFileName(),   rhs.libraryFileName())
        && lhs.lineNumber() ==            rhs.lineNumber()
        && equal(lhs.mangledSymbolName(), rhs.mangledSymbolName())
        && lhs.offsetFromSymbol() ==      rhs.offsetFromSymbol()
        && equal(lhs.sourceFileName(),    rhs.sourceFileName())
        && equal(lhs.symbolName(),        rhs.symbolName());
}

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdesu_StackTraceFrame& frame)
{
    // Note that we don't print out the mangled symbol name.  If demangling did
    // not happen, the 'symbolName' value will also have the mangled symbol
    // name.

    if (frame.isSymbolNameValid()) {
        stream << frame.d_symbolName_p;
    }
    else {
        if (frame.isMangledSymbolNameValid()) {
            stream << frame.d_mangledSymbolName_p;
        }
        else {
            stream << " --unknown--";
        }
    }
    stream << bsl::hex;
    if (frame.isOffsetFromSymbolValid()) {
        stream << "+0x" << frame.d_offsetFromSymbol;
    }
    if (frame.isAddressValid()) {
        stream << " at 0x" << (bsls_PlatformUtil::UintPtr) frame.d_address_p;
    }
    stream << bsl::dec;
    if (frame.isSourceFileNameValid()) {
#if defined(BDESU_OBJECTFILEFORMAT_RESOLVER_ELF)
        // source file info is only available for statics on Elf

        stream << " static";
#endif
        // point 'pc' to the basename

        const char *pcB = frame.d_sourceFileName_p;
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

        const char *pcB = frame.d_libraryFileName_p;
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
