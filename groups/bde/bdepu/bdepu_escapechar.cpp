// bdepu_escapechar.cpp                  -*-C++-*-
#include <bdepu_escapechar.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdepu_escapechar_cpp,"$Id$ $CSID$")

namespace BloombergLP {

    enum {
        ER = bdepu_EscapeChar::BDEPU_ERROR,
        AL = bdepu_EscapeChar::BDEPU_ALERT,
        BA = bdepu_EscapeChar::BDEPU_BACKSPACE,
        FO = bdepu_EscapeChar::BDEPU_FORMFEED,
        NE = bdepu_EscapeChar::BDEPU_NEWLINE,
        CA = bdepu_EscapeChar::BDEPU_CARRIAGE_RETURN,
        TA = bdepu_EscapeChar::BDEPU_TAB,
        VE = bdepu_EscapeChar::BDEPU_VERTICAL_TAB,
        QU = bdepu_EscapeChar::BDEPU_QUESTION_MARK,
        SI = bdepu_EscapeChar::BDEPU_SINGLE_QUOTE,
        DB = bdepu_EscapeChar::BDEPU_DOUBLE_QUOTE,
        HE = bdepu_EscapeChar::BDEPU_HEXADECIMAL,
        OC = bdepu_EscapeChar::BDEPU_OCTAL,
        BK = bdepu_EscapeChar::BDEPU_BACKSLASH,
        EN = bdepu_EscapeChar::BDEPU_ENDLINE
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

const unsigned char *bdepu_EscapeChar::s_escapeCodes =
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
