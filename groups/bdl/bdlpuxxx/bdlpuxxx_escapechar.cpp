// bdlpuxxx_escapechar.cpp                  -*-C++-*-
#include <bdlpuxxx_escapechar.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlpuxxx_escapechar_cpp,"$Id$ $CSID$")

namespace BloombergLP {

    enum {
        ER = bdlpuxxx::EscapeChar::BDEPU_ERROR,
        AL = bdlpuxxx::EscapeChar::BDEPU_ALERT,
        BA = bdlpuxxx::EscapeChar::BDEPU_BACKSPACE,
        FO = bdlpuxxx::EscapeChar::BDEPU_FORMFEED,
        NE = bdlpuxxx::EscapeChar::BDEPU_NEWLINE,
        CA = bdlpuxxx::EscapeChar::BDEPU_CARRIAGE_RETURN,
        TA = bdlpuxxx::EscapeChar::BDEPU_TAB,
        VE = bdlpuxxx::EscapeChar::BDEPU_VERTICAL_TAB,
        QU = bdlpuxxx::EscapeChar::BDEPU_QUESTION_MARK,
        SI = bdlpuxxx::EscapeChar::BDEPU_SINGLE_QUOTE,
        DB = bdlpuxxx::EscapeChar::BDEPU_DOUBLE_QUOTE,
        HE = bdlpuxxx::EscapeChar::BDEPU_HEXADECIMAL,
        OC = bdlpuxxx::EscapeChar::BDEPU_OCTAL,
        BK = bdlpuxxx::EscapeChar::BDEPU_BACKSLASH,
        EN = bdlpuxxx::EscapeChar::BDEPU_ENDLINE
    };

// This table specifies, for each ASCII character, which escape sequence
// it would be part of were it the *second* character following an initial
// backslash.
static const unsigned char ESCAPE_SEQUENCE_TYPE[256] = {
/*  0*/  EN,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*  8*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/* 16*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/* 24*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/* 32*/  ER,  ER,  DB,  ER,  ER,  ER,  ER,  SI,
/* 40*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/* 48*/  OC,  OC,  OC,  OC,  OC,  OC,  OC,  OC,
/* 56*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  QU,
/* 64*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/* 72*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/* 80*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/* 88*/  ER,  ER,  ER,  ER,  BK,  ER,  ER,  ER,
/* 96*/  ER,  AL,  BA,  ER,  ER,  ER,  FO,  ER,
/*104*/  ER,  ER,  ER,  ER,  ER,  ER,  NE,  ER,
/*112*/  ER,  ER,  CA,  ER,  TA,  ER,  VE,  ER,
/*120*/  HE,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*128*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*136*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*144*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*152*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*160*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*168*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*176*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*184*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*192*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*200*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*208*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*216*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*224*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*232*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*240*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER,
/*248*/  ER,  ER,  ER,  ER,  ER,  ER,  ER,  ER
};

const unsigned char *bdlpuxxx::EscapeChar::s_escapeCodes =
                                                ESCAPE_SEQUENCE_TYPE;

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
