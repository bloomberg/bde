// balj_journalparameters.cpp -*-C++-*-

#include <balj_journalparameters.h>

#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                        // ----------------------------
                        // class balj::JournalParameters
                        // ----------------------------

// CONSTANTS
const int balj::JournalParameters::DEFAULT_BLOCK_SIZE = 256;
    // default value of 'BlockSize' attribute

const int balj::JournalParameters::DEFAULT_BLOCKS_PER_PAGE = 4096;
    // default value of 'BlocksPerPage' attribute

const int balj::JournalParameters::DEFAULT_PAGES_PER_SET = 64;
    // default value of 'PagesPerSet' attribute

const int balj::JournalParameters::DEFAULT_FREE_BLOCK_THRESHOLD = 50;

namespace balj {    // default value of 'FreeBlockThreshold' attribute

// ACCESSORS

bsl::ostream& JournalParameters::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "BlockSize = ";
        bdlb::PrintMethods::print(stream, d_blockSize,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "BlocksPerPage = ";
        bdlb::PrintMethods::print(stream, d_blocksPerPage,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "PagesPerSet = ";
        bdlb::PrintMethods::print(stream, d_pagesPerSet,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "FreeBlockThreshold = ";
        bdlb::PrintMethods::print(stream, d_freeBlockThreshold,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "BlockSize = ";
        bdlb::PrintMethods::print(stream, d_blockSize,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "BlocksPerPage = ";
        bdlb::PrintMethods::print(stream, d_blocksPerPage,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "PagesPerSet = ";
        bdlb::PrintMethods::print(stream, d_pagesPerSet,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "FreeBlockThreshold = ";
        bdlb::PrintMethods::print(stream, d_freeBlockThreshold,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}
}  // close package namespace

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
