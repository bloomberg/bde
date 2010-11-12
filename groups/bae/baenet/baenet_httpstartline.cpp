// baenet_httpstartline.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpstartline_cpp,"$Id$ $CSID$ $CCId$")
#include <baenet_httpstartline.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <baenet_httprequestline.h>
#include <baenet_httpstatusline.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>
#include <bsl_iomanip.h>

namespace BloombergLP {

                       // ------------------------------                       
                       // class baenet_HttpStartLine                       
                       // ------------------------------                       

// CONSTANTS

const char baenet_HttpStartLine::CLASS_NAME[] = "baenet_HttpStartLine";
const bdeat_SelectionInfo baenet_HttpStartLine::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_REQUEST_LINE,
        "requestLine",
        sizeof("requestLine") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_STATUS_LINE,
        "statusLine",
        sizeof("statusLine") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *baenet_HttpStartLine::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if ((name[0]|0x20)=='s'
             && (name[1]|0x20)=='t'
             && (name[2]|0x20)=='a'
             && (name[3]|0x20)=='t'
             && (name[4]|0x20)=='u'
             && (name[5]|0x20)=='s'
             && (name[6]|0x20)=='l'
             && (name[7]|0x20)=='i'
             && (name[8]|0x20)=='n'
             && (name[9]|0x20)=='e')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_STATUS_LINE];
            }
        } break;
        case 11: {
            if ((name[0]|0x20)=='r'
             && (name[1]|0x20)=='e'
             && (name[2]|0x20)=='q'
             && (name[3]|0x20)=='u'
             && (name[4]|0x20)=='e'
             && (name[5]|0x20)=='s'
             && (name[6]|0x20)=='t'
             && (name[7]|0x20)=='l'
             && (name[8]|0x20)=='i'
             && (name[9]|0x20)=='n'
             && (name[10]|0x20)=='e')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_REQUEST_LINE];
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *baenet_HttpStartLine::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_REQUEST_LINE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_REQUEST_LINE];
      case SELECTION_ID_STATUS_LINE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_STATUS_LINE];
      default:
        return 0;
    }
}

// CREATORS

baenet_HttpStartLine::baenet_HttpStartLine(
    const baenet_HttpStartLine& original,
    bslma_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_REQUEST_LINE: {
        new (d_requestLine.buffer())
            baenet_HttpRequestLine(
                original.d_requestLine.object(), d_allocator_p);
      } break;
      case SELECTION_ID_STATUS_LINE: {
        new (d_statusLine.buffer())
            baenet_HttpStatusLine(
                original.d_statusLine.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

baenet_HttpStartLine&
baenet_HttpStartLine::operator=(const baenet_HttpStartLine& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_REQUEST_LINE: {
            makeRequestLine(rhs.d_requestLine.object());
          } break;
          case SELECTION_ID_STATUS_LINE: {
            makeStatusLine(rhs.d_statusLine.object());
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void baenet_HttpStartLine::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_REQUEST_LINE: {
        d_requestLine.object().~baenet_HttpRequestLine();
      } break;
      case SELECTION_ID_STATUS_LINE: {
        d_statusLine.object().~baenet_HttpStatusLine();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int baenet_HttpStartLine::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_REQUEST_LINE: {
        makeRequestLine();
      } break;
      case SELECTION_ID_STATUS_LINE: {
        makeStatusLine();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int baenet_HttpStartLine::makeSelection(const char *name, int nameLength)
{
    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

baenet_HttpRequestLine& baenet_HttpStartLine::makeRequestLine()
{
    if (SELECTION_ID_REQUEST_LINE == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_requestLine.object());
    }
    else {
        reset();
        new (d_requestLine.buffer())
                baenet_HttpRequestLine(d_allocator_p);

        d_selectionId = SELECTION_ID_REQUEST_LINE;
    }

    return d_requestLine.object();
}

baenet_HttpRequestLine& baenet_HttpStartLine::makeRequestLine(const baenet_HttpRequestLine& value)
{
    if (SELECTION_ID_REQUEST_LINE == d_selectionId) {
        d_requestLine.object() = value;
    }
    else {
        reset();
        new (d_requestLine.buffer())
                baenet_HttpRequestLine(value, d_allocator_p);
        d_selectionId = SELECTION_ID_REQUEST_LINE;
    }

    return d_requestLine.object();
}

baenet_HttpStatusLine& baenet_HttpStartLine::makeStatusLine()
{
    if (SELECTION_ID_STATUS_LINE == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_statusLine.object());
    }
    else {
        reset();
        new (d_statusLine.buffer())
                baenet_HttpStatusLine(d_allocator_p);

        d_selectionId = SELECTION_ID_STATUS_LINE;
    }

    return d_statusLine.object();
}

baenet_HttpStatusLine& baenet_HttpStartLine::makeStatusLine(const baenet_HttpStatusLine& value)
{
    if (SELECTION_ID_STATUS_LINE == d_selectionId) {
        d_statusLine.object() = value;
    }
    else {
        reset();
        new (d_statusLine.buffer())
                baenet_HttpStatusLine(value, d_allocator_p);
        d_selectionId = SELECTION_ID_STATUS_LINE;
    }

    return d_statusLine.object();
}

// ACCESSORS

bsl::ostream& baenet_HttpStartLine::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);

        switch (d_selectionId) {
          case SELECTION_ID_REQUEST_LINE: {
            stream << "RequestLine = ";
            bdeu_PrintMethods::print(stream, d_requestLine.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_STATUS_LINE: {
            stream << "StatusLine = ";
            bdeu_PrintMethods::print(stream, d_statusLine.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case SELECTION_ID_REQUEST_LINE: {
            stream << "RequestLine = ";
            bdeu_PrintMethods::print(stream, d_requestLine.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_STATUS_LINE: {
            stream << "StatusLine = ";
            bdeu_PrintMethods::print(stream, d_statusLine.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}

const char *baenet_HttpStartLine::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_REQUEST_LINE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_REQUEST_LINE].name();
      case SELECTION_ID_STATUS_LINE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_STATUS_LINE].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}
}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Wed Feb 10 17:14:02 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
