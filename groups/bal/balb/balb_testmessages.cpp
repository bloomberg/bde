// balb_testmessages.cpp      -- GENERATED FILE - DO NOT EDIT --      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bsls_ident.h>
BSLS_IDENT_RCSID(balb_testmessages_cpp,"$Id$ $CSID$")

#include <balb_testmessages.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlde_utf8util.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlt_datetimetz.h>
#include <bdlb_nullableallocatedvalue.h>
#include <bdlb_nullablevalue.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

#include <iomanip>
#include <limits>
#include <ostream>

namespace BloombergLP {
namespace balb {

                              // ---------------
                              // class CustomInt
                              // ---------------

// PRIVATE CLASS METHODS

int CustomInt::checkRestrictions(const int& value)
{
    if (1000 < value) {
        return -1;                                                    // RETURN
    }

    return 0;
}

// CONSTANTS

const char CustomInt::CLASS_NAME[] = "CustomInt";



                             // ------------------
                             // class CustomString
                             // ------------------

// PRIVATE CLASS METHODS

int CustomString::checkRestrictions(const bsl::string& value)
{
    if (8 < bdlde::Utf8Util::numCharacters(value.c_str(), value.length())) {
        return -1;                                                    // RETURN
    }

    return 0;
}

// CONSTANTS

const char CustomString::CLASS_NAME[] = "CustomString";



                              // ----------------
                              // class Enumerated
                              // ----------------

// CONSTANTS

const char Enumerated::CLASS_NAME[] = "Enumerated";

const bdlat_EnumeratorInfo Enumerated::ENUMERATOR_INFO_ARRAY[] = {
    {
        Enumerated::e_NEW_YORK,
        "NEW_YORK",
        sizeof("NEW_YORK") - 1,
        ""
    },
    {
        Enumerated::e_NEW_JERSEY,
        "NEW_JERSEY",
        sizeof("NEW_JERSEY") - 1,
        ""
    },
    {
        Enumerated::e_LONDON,
        "LONDON",
        sizeof("LONDON") - 1,
        ""
    }
};

// CLASS METHODS

int Enumerated::fromInt(Enumerated::Value *result, int number)
{
    switch (number) {
      case Enumerated::e_NEW_YORK:
      case Enumerated::e_NEW_JERSEY:
      case Enumerated::e_LONDON:
        *result = (Enumerated::Value)number;
        return 0;                                                     // RETURN
      default:
        return -1;                                                    // RETURN
    }
}

int Enumerated::fromString(Enumerated::Value *result,
                            const char         *string,
                            int                 stringLength)
{

    switch(stringLength) {
        case 6: {
            if (string[0]=='L'
             && string[1]=='O'
             && string[2]=='N'
             && string[3]=='D'
             && string[4]=='O'
             && string[5]=='N')
            {
                *result = Enumerated::e_LONDON;
                return 0;                                             // RETURN
            }
        } break;
        case 8: {
            if (string[0]=='N'
             && string[1]=='E'
             && string[2]=='W'
             && string[3]=='_'
             && string[4]=='Y'
             && string[5]=='O'
             && string[6]=='R'
             && string[7]=='K')
            {
                *result = Enumerated::e_NEW_YORK;
                return 0;                                             // RETURN
            }
        } break;
        case 10: {
            if (string[0]=='N'
             && string[1]=='E'
             && string[2]=='W'
             && string[3]=='_'
             && string[4]=='J'
             && string[5]=='E'
             && string[6]=='R'
             && string[7]=='S'
             && string[8]=='E'
             && string[9]=='Y')
            {
                *result = Enumerated::e_NEW_JERSEY;
                return 0;                                             // RETURN
            }
        } break;
    }

    return -1;
}

const char *Enumerated::toString(Enumerated::Value value)
{
    switch (value) {
      case e_NEW_YORK: {
        return "NEW_YORK";                                            // RETURN
      } break;
      case e_NEW_JERSEY: {
        return "NEW_JERSEY";                                          // RETURN
      } break;
      case e_LONDON: {
        return "LONDON";                                              // RETURN
      } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}


                     // ----------------------------------
                     // class SequenceWithAnonymityChoice1
                     // ----------------------------------

// CONSTANTS

const char SequenceWithAnonymityChoice1::CLASS_NAME[] = "SequenceWithAnonymityChoice1";

const bdlat_SelectionInfo SequenceWithAnonymityChoice1::SELECTION_INFO_ARRAY[] = {
    {
        e_SELECTION_ID_SELECTION5,
        "selection5",
        sizeof("selection5") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_SELECTION_ID_SELECTION6,
        "selection6",
        sizeof("selection6") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *SequenceWithAnonymityChoice1::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (name[0]=='s'
             && name[1]=='e'
             && name[2]=='l'
             && name[3]=='e'
             && name[4]=='c'
             && name[5]=='t'
             && name[6]=='i'
             && name[7]=='o'
             && name[8]=='n')
            {
                switch(name[9]) {
                    case '5': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5];
                                                                      // RETURN
                    } break;
                    case '6': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *SequenceWithAnonymityChoice1::lookupSelectionInfo(int id)
{
    switch (id) {
      case e_SELECTION_ID_SELECTION5:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5];
      case e_SELECTION_ID_SELECTION6:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6];
      default:
        return 0;
    }
}

// CREATORS

SequenceWithAnonymityChoice1::SequenceWithAnonymityChoice1(
    const SequenceWithAnonymityChoice1& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION5: {
        new (d_selection5.buffer())
            bool(original.d_selection5.object());
      } break;
      case e_SELECTION_ID_SELECTION6: {
        new (d_selection6.buffer())
            bsl::string(
                original.d_selection6.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

SequenceWithAnonymityChoice1&
SequenceWithAnonymityChoice1::operator=(const SequenceWithAnonymityChoice1& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case e_SELECTION_ID_SELECTION5: {
            makeSelection5(rhs.d_selection5.object());
          } break;
          case e_SELECTION_ID_SELECTION6: {
            makeSelection6(rhs.d_selection6.object());
          } break;
          default:
            BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void SequenceWithAnonymityChoice1::reset()
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION5: {
        // no destruction required
      } break;
      case e_SELECTION_ID_SELECTION6: {
        typedef bsl::string Type;
        d_selection6.object().~Type();
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = e_SELECTION_ID_UNDEFINED;
}

int SequenceWithAnonymityChoice1::makeSelection(int selectionId)
{
    switch (selectionId) {
      case e_SELECTION_ID_SELECTION5: {
        makeSelection5();
      } break;
      case e_SELECTION_ID_SELECTION6: {
        makeSelection6();
      } break;
      case e_SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;                                                    // RETURN
    }
    return 0;
}

int SequenceWithAnonymityChoice1::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;                                                     // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

bool& SequenceWithAnonymityChoice1::makeSelection5()
{
    if (e_SELECTION_ID_SELECTION5 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection5.object());
    }
    else {
        reset();
        new (d_selection5.buffer())
            bool();

        d_selectionId = e_SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

bool& SequenceWithAnonymityChoice1::makeSelection5(bool value)
{
    if (e_SELECTION_ID_SELECTION5 == d_selectionId) {
        d_selection5.object() = value;
    }
    else {
        reset();
        new (d_selection5.buffer())
                bool(value);
        d_selectionId = e_SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

bsl::string& SequenceWithAnonymityChoice1::makeSelection6()
{
    if (e_SELECTION_ID_SELECTION6 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection6.object());
    }
    else {
        reset();
        new (d_selection6.buffer())
                bsl::string(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

bsl::string& SequenceWithAnonymityChoice1::makeSelection6(const bsl::string& value)
{
    if (e_SELECTION_ID_SELECTION6 == d_selectionId) {
        d_selection6.object() = value;
    }
    else {
        reset();
        new (d_selection6.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

// ACCESSORS

bsl::ostream& SequenceWithAnonymityChoice1::print(
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

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION5: {
            stream << "Selection5 = ";
            bdlb::PrintMethods::print(stream, d_selection5.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION6: {
            stream << "Selection6 = ";
            bdlb::PrintMethods::print(stream, d_selection6.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION5: {
            stream << "Selection5 = ";
            bdlb::PrintMethods::print(stream, d_selection5.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION6: {
            stream << "Selection6 = ";
            bdlb::PrintMethods::print(stream, d_selection6.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}


const char *SequenceWithAnonymityChoice1::selectionName() const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION5:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5].name();
      case e_SELECTION_ID_SELECTION6:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6].name();
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
}

                            // -------------------
                            // class SimpleRequest
                            // -------------------

// CONSTANTS

const char SimpleRequest::CLASS_NAME[] = "SimpleRequest";

const bdlat_AttributeInfo SimpleRequest::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_DATA,
        "data",
        sizeof("data") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_RESPONSE_LENGTH,
        "responseLength",
        sizeof("responseLength") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *SimpleRequest::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 4: {
            if (name[0]=='d'
             && name[1]=='a'
             && name[2]=='t'
             && name[3]=='a')
            {
                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DATA];   // RETURN
            }
        } break;
        case 14: {
            if (name[0]=='r'
             && name[1]=='e'
             && name[2]=='s'
             && name[3]=='p'
             && name[4]=='o'
             && name[5]=='n'
             && name[6]=='s'
             && name[7]=='e'
             && name[8]=='L'
             && name[9]=='e'
             && name[10]=='n'
             && name[11]=='g'
             && name[12]=='t'
             && name[13]=='h')
            {
                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_RESPONSE_LENGTH];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *SimpleRequest::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_DATA:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_DATA];
      case e_ATTRIBUTE_ID_RESPONSE_LENGTH:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_RESPONSE_LENGTH];
      default:
        return 0;
    }
}

// CREATORS

SimpleRequest::SimpleRequest(bslma::Allocator *basicAllocator)
: d_data(basicAllocator)
, d_responseLength()
{
}

SimpleRequest::SimpleRequest(const SimpleRequest& original,
                             bslma::Allocator *basicAllocator)
: d_data(original.d_data, basicAllocator)
, d_responseLength(original.d_responseLength)
{
}

SimpleRequest::~SimpleRequest()
{
}

// MANIPULATORS

SimpleRequest&
SimpleRequest::operator=(const SimpleRequest& rhs)
{
    if (this != &rhs) {
        d_data = rhs.d_data;
        d_responseLength = rhs.d_responseLength;
    }
    return *this;
}

void SimpleRequest::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_data);
    bdlat_ValueTypeFunctions::reset(&d_responseLength);
}

// ACCESSORS

bsl::ostream& SimpleRequest::print(
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
        stream << "Data = ";
        bdlb::PrintMethods::print(stream, d_data,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ResponseLength = ";
        bdlb::PrintMethods::print(stream, d_responseLength,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Data = ";
        bdlb::PrintMethods::print(stream, d_data,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ResponseLength = ";
        bdlb::PrintMethods::print(stream, d_responseLength,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}



                           // ----------------------
                           // class UnsignedSequence
                           // ----------------------

// CONSTANTS

const char UnsignedSequence::CLASS_NAME[] = "UnsignedSequence";

const bdlat_AttributeInfo UnsignedSequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *UnsignedSequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (name[0]=='e'
             && name[1]=='l'
             && name[2]=='e'
             && name[3]=='m'
             && name[4]=='e'
             && name[5]=='n'
             && name[6]=='t')
            {
                switch(name[7]) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *UnsignedSequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
      case e_ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
      case e_ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
      default:
        return 0;
    }
}

// CREATORS

UnsignedSequence::UnsignedSequence()
: d_element3()
, d_element1()
, d_element2()
{
}

UnsignedSequence::UnsignedSequence(const UnsignedSequence& original)
: d_element3(original.d_element3)
, d_element1(original.d_element1)
, d_element2(original.d_element2)
{
}

UnsignedSequence::~UnsignedSequence()
{
}

// MANIPULATORS

UnsignedSequence&
UnsignedSequence::operator=(const UnsignedSequence& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
    }
    return *this;
}

void UnsignedSequence::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
}

// ACCESSORS

bsl::ostream& UnsignedSequence::print(
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
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}



                             // ------------------
                             // class VoidSequence
                             // ------------------

// CONSTANTS

const char VoidSequence::CLASS_NAME[] = "VoidSequence";


// CLASS METHODS

const bdlat_AttributeInfo *VoidSequence::lookupAttributeInfo(
        const char *,
        int         nameLength)
{
    switch(nameLength) {
        }
    return 0;
}

const bdlat_AttributeInfo *VoidSequence::lookupAttributeInfo(int id)
{
    switch (id) {
      default:
        return 0;
    }
}

// CREATORS

VoidSequence::VoidSequence()
{
}

VoidSequence::VoidSequence(const VoidSequence& )
{
}

VoidSequence::~VoidSequence()
{
}

// MANIPULATORS

VoidSequence&
VoidSequence::operator=(const VoidSequence& rhs)
{
    if (this != &rhs) {
    }
    return *this;
}

void VoidSequence::reset()
{
}

// ACCESSORS

bsl::ostream& VoidSequence::print(
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

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << " ]";
    }

    return stream << bsl::flush;
}



                              // ---------------
                              // class Sequence3
                              // ---------------

// CONSTANTS

const char Sequence3::CLASS_NAME[] = "Sequence3";

const bdlat_AttributeInfo Sequence3::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence3::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (name[0]=='e'
             && name[1]=='l'
             && name[2]=='e'
             && name[3]=='m'
             && name[4]=='e'
             && name[5]=='n'
             && name[6]=='t')
            {
                switch(name[7]) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
                                                                      // RETURN
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Sequence3::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
      case e_ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
      case e_ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
      case e_ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
      case e_ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
      case e_ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6];
      default:
        return 0;
    }
}

// CREATORS

Sequence3::Sequence3(bslma::Allocator *basicAllocator)
: d_element2(basicAllocator)
, d_element4(basicAllocator)
, d_element5(basicAllocator)
, d_element6(basicAllocator)
, d_element1(basicAllocator)
, d_element3()
{
}

Sequence3::Sequence3(const Sequence3&  original,
                     bslma::Allocator *basicAllocator)
: d_element2(original.d_element2, basicAllocator)
, d_element4(original.d_element4, basicAllocator)
, d_element5(original.d_element5, basicAllocator)
, d_element6(original.d_element6, basicAllocator)
, d_element1(original.d_element1, basicAllocator)
, d_element3(original.d_element3)
{
}

Sequence3::~Sequence3()
{
}

// MANIPULATORS

Sequence3&
Sequence3::operator=(const Sequence3& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
        d_element6 = rhs.d_element6;
    }
    return *this;
}

void Sequence3::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
    bdlat_ValueTypeFunctions::reset(&d_element6);
}

// ACCESSORS

bsl::ostream& Sequence3::print(
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
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element6 = ";
        bdlb::PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element6 = ";
        bdlb::PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}



                              // ---------------
                              // class Sequence5
                              // ---------------

// CONSTANTS

const char Sequence5::CLASS_NAME[] = "Sequence5";

const bdlat_AttributeInfo Sequence5::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        e_ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        e_ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_HEX
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        e_ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        e_ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        e_ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence5::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (name[0]=='e'
             && name[1]=='l'
             && name[2]=='e'
             && name[3]=='m'
             && name[4]=='e'
             && name[5]=='n'
             && name[6]=='t')
            {
                switch(name[7]) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
                                                                      // RETURN
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6];
                                                                      // RETURN
                    } break;
                    case '7': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Sequence5::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
      case e_ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
      case e_ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
      case e_ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
      case e_ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
      case e_ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6];
      case e_ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7];
      default:
        return 0;
    }
}

// CREATORS

Sequence5::Sequence5(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element5(basicAllocator)
, d_element3(basicAllocator)
, d_element4(basicAllocator)
, d_element2(basicAllocator)
, d_element6(basicAllocator)
, d_element7(basicAllocator)
{
    d_element1 = new (*d_allocator_p)
            Sequence3(d_allocator_p);
}

Sequence5::Sequence5(const Sequence5&  original,
                     bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element5(original.d_element5, basicAllocator)
, d_element3(original.d_element3, basicAllocator)
, d_element4(original.d_element4, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_element6(original.d_element6, basicAllocator)
, d_element7(original.d_element7, basicAllocator)
{
    d_element1 = new (*d_allocator_p)
            Sequence3(*original.d_element1, d_allocator_p);
}

Sequence5::~Sequence5()
{
    d_allocator_p->deleteObject(d_element1);
}

// MANIPULATORS

Sequence5&
Sequence5::operator=(const Sequence5& rhs)
{
    if (this != &rhs) {
        *d_element1 = *rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
        d_element6 = rhs.d_element6;
        d_element7 = rhs.d_element7;
    }
    return *this;
}

void Sequence5::reset()
{
    bdlat_ValueTypeFunctions::reset(d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
    bdlat_ValueTypeFunctions::reset(&d_element6);
    bdlat_ValueTypeFunctions::reset(&d_element7);
}

// ACCESSORS

bsl::ostream& Sequence5::print(
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
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, *d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element6 = ";
        bdlb::PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element7 = ";
        bdlb::PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, *d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element6 = ";
        bdlb::PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element7 = ";
        bdlb::PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}



                              // ---------------
                              // class Sequence6
                              // ---------------

// CONSTANTS

const char Sequence6::CLASS_NAME[] = "Sequence6";

const bdlat_AttributeInfo Sequence6::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        e_ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT8,
        "element8",
        sizeof("element8") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT9,
        "element9",
        sizeof("element9") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT10,
        "element10",
        sizeof("element10") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT11,
        "element11",
        sizeof("element11") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT12,
        "element12",
        sizeof("element12") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT13,
        "element13",
        sizeof("element13") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        e_ATTRIBUTE_ID_ELEMENT14,
        "element14",
        sizeof("element14") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT15,
        "element15",
        sizeof("element15") - 1,
        "",
        bdlat_FormattingMode::e_DEC
      | bdlat_FormattingMode::e_NILLABLE
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence6::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (name[0]=='e'
             && name[1]=='l'
             && name[2]=='e'
             && name[3]=='m'
             && name[4]=='e'
             && name[5]=='n'
             && name[6]=='t')
            {
                switch(name[7]) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
                                                                      // RETURN
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6];
                                                                      // RETURN
                    } break;
                    case '7': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7];
                                                                      // RETURN
                    } break;
                    case '8': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8];
                                                                      // RETURN
                    } break;
                    case '9': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
        case 9: {
            if (name[0]=='e'
             && name[1]=='l'
             && name[2]=='e'
             && name[3]=='m'
             && name[4]=='e'
             && name[5]=='n'
             && name[6]=='t'
             && name[7]=='1')
            {
                switch(name[8]) {
                    case '0': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10];
                                                                      // RETURN
                    } break;
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Sequence6::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
      case e_ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
      case e_ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
      case e_ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
      case e_ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
      case e_ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6];
      case e_ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7];
      case e_ATTRIBUTE_ID_ELEMENT8:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8];
      case e_ATTRIBUTE_ID_ELEMENT9:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9];
      case e_ATTRIBUTE_ID_ELEMENT10:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10];
      case e_ATTRIBUTE_ID_ELEMENT11:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11];
      case e_ATTRIBUTE_ID_ELEMENT12:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12];
      case e_ATTRIBUTE_ID_ELEMENT13:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13];
      case e_ATTRIBUTE_ID_ELEMENT14:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14];
      case e_ATTRIBUTE_ID_ELEMENT15:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15];
      default:
        return 0;
    }
}

// CREATORS

Sequence6::Sequence6(bslma::Allocator *basicAllocator)
: d_element12(basicAllocator)
, d_element10(basicAllocator)
, d_element15(basicAllocator)
, d_element13(basicAllocator)
, d_element11(basicAllocator)
, d_element2(basicAllocator)
, d_element7(basicAllocator)
, d_element4()
, d_element6(basicAllocator)
, d_element14(basicAllocator)
, d_element9()
, d_element3()
, d_element8()
, d_element5()
, d_element1()
{
}

Sequence6::Sequence6(const Sequence6&  original,
                     bslma::Allocator *basicAllocator)
: d_element12(original.d_element12, basicAllocator)
, d_element10(original.d_element10, basicAllocator)
, d_element15(original.d_element15, basicAllocator)
, d_element13(original.d_element13, basicAllocator)
, d_element11(original.d_element11, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_element7(original.d_element7, basicAllocator)
, d_element4(original.d_element4)
, d_element6(original.d_element6, basicAllocator)
, d_element14(original.d_element14, basicAllocator)
, d_element9(original.d_element9)
, d_element3(original.d_element3)
, d_element8(original.d_element8)
, d_element5(original.d_element5)
, d_element1(original.d_element1)
{
}

Sequence6::~Sequence6()
{
}

// MANIPULATORS

Sequence6&
Sequence6::operator=(const Sequence6& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
        d_element6 = rhs.d_element6;
        d_element7 = rhs.d_element7;
        d_element8 = rhs.d_element8;
        d_element9 = rhs.d_element9;
        d_element10 = rhs.d_element10;
        d_element11 = rhs.d_element11;
        d_element12 = rhs.d_element12;
        d_element13 = rhs.d_element13;
        d_element14 = rhs.d_element14;
        d_element15 = rhs.d_element15;
    }
    return *this;
}

void Sequence6::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
    bdlat_ValueTypeFunctions::reset(&d_element6);
    bdlat_ValueTypeFunctions::reset(&d_element7);
    bdlat_ValueTypeFunctions::reset(&d_element8);
    bdlat_ValueTypeFunctions::reset(&d_element9);
    bdlat_ValueTypeFunctions::reset(&d_element10);
    bdlat_ValueTypeFunctions::reset(&d_element11);
    bdlat_ValueTypeFunctions::reset(&d_element12);
    bdlat_ValueTypeFunctions::reset(&d_element13);
    bdlat_ValueTypeFunctions::reset(&d_element14);
    bdlat_ValueTypeFunctions::reset(&d_element15);
}

// ACCESSORS

bsl::ostream& Sequence6::print(
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
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, (int)d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element6 = ";
        bdlb::PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element7 = ";
        bdlb::PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element8 = ";
        bdlb::PrintMethods::print(stream, d_element8,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element9 = ";
        bdlb::PrintMethods::print(stream, d_element9,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element10 = ";
        bdlb::PrintMethods::print(stream, d_element10,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element11 = ";
        bdlb::PrintMethods::print(stream, d_element11,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element12 = ";
        bdlb::PrintMethods::print(stream, d_element12,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element13 = ";
        bdlb::PrintMethods::print(stream, d_element13,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element14 = ";
        bdlb::PrintMethods::print(stream, d_element14,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element15 = ";
        bdlb::PrintMethods::print(stream, d_element15,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, (int)d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element6 = ";
        bdlb::PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element7 = ";
        bdlb::PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element8 = ";
        bdlb::PrintMethods::print(stream, d_element8,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element9 = ";
        bdlb::PrintMethods::print(stream, d_element9,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element10 = ";
        bdlb::PrintMethods::print(stream, d_element10,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element11 = ";
        bdlb::PrintMethods::print(stream, d_element11,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element12 = ";
        bdlb::PrintMethods::print(stream, d_element12,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element13 = ";
        bdlb::PrintMethods::print(stream, d_element13,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element14 = ";
        bdlb::PrintMethods::print(stream, d_element14,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element15 = ";
        bdlb::PrintMethods::print(stream, d_element15,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}



                               // -------------
                               // class Choice3
                               // -------------

// CONSTANTS

const char Choice3::CLASS_NAME[] = "Choice3";

const bdlat_SelectionInfo Choice3::SELECTION_INFO_ARRAY[] = {
    {
        e_SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *Choice3::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (name[0]=='s'
             && name[1]=='e'
             && name[2]=='l'
             && name[3]=='e'
             && name[4]=='c'
             && name[5]=='t'
             && name[6]=='i'
             && name[7]=='o'
             && name[8]=='n')
            {
                switch(name[9]) {
                    case '1': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *Choice3::lookupSelectionInfo(int id)
{
    switch (id) {
      case e_SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
      case e_SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
      case e_SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
      case e_SELECTION_ID_SELECTION4:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
      default:
        return 0;
    }
}

// CREATORS

Choice3::Choice3(
    const Choice3&    original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            Sequence6(
                original.d_selection1.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            unsigned char(original.d_selection2.object());
      } break;
      case e_SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            CustomString(
                original.d_selection3.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            CustomInt(original.d_selection4.object());
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Choice3&
Choice3::operator=(const Choice3& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case e_SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case e_SELECTION_ID_SELECTION3: {
            makeSelection3(rhs.d_selection3.object());
          } break;
          case e_SELECTION_ID_SELECTION4: {
            makeSelection4(rhs.d_selection4.object());
          } break;
          default:
            BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Choice3::reset()
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        d_selection1.object().~Sequence6();
      } break;
      case e_SELECTION_ID_SELECTION2: {
        // no destruction required
      } break;
      case e_SELECTION_ID_SELECTION3: {
        d_selection3.object().~CustomString();
      } break;
      case e_SELECTION_ID_SELECTION4: {
        d_selection4.object().~CustomInt();
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = e_SELECTION_ID_UNDEFINED;
}

int Choice3::makeSelection(int selectionId)
{
    switch (selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case e_SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case e_SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case e_SELECTION_ID_SELECTION4: {
        makeSelection4();
      } break;
      case e_SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;                                                    // RETURN
    }
    return 0;
}

int Choice3::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;                                                     // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

Sequence6& Choice3::makeSelection1()
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence6(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

Sequence6& Choice3::makeSelection1(const Sequence6& value)
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence6(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

unsigned char& Choice3::makeSelection2()
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
            unsigned char();

        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

unsigned char& Choice3::makeSelection2(unsigned char value)
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                unsigned char(value);
        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

CustomString& Choice3::makeSelection3()
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection3.object());
    }
    else {
        reset();
        new (d_selection3.buffer())
                CustomString(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

CustomString& Choice3::makeSelection3(const CustomString& value)
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = value;
    }
    else {
        reset();
        new (d_selection3.buffer())
                CustomString(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

CustomInt& Choice3::makeSelection4()
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection4.object());
    }
    else {
        reset();
        new (d_selection4.buffer())
            CustomInt();

        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

CustomInt& Choice3::makeSelection4(const CustomInt& value)
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        d_selection4.object() = value;
    }
    else {
        reset();
        new (d_selection4.buffer())
                CustomInt(value);
        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

// ACCESSORS

bsl::ostream& Choice3::print(
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

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, (int)d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, d_selection4.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, (int)d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, d_selection4.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}


const char *Choice3::selectionName() const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1].name();
      case e_SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2].name();
      case e_SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3].name();
      case e_SELECTION_ID_SELECTION4:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4].name();
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
}

                     // ---------------------------------
                     // class SequenceWithAnonymityChoice
                     // ---------------------------------

// CONSTANTS

const char SequenceWithAnonymityChoice::CLASS_NAME[] = "SequenceWithAnonymityChoice";

const bdlat_SelectionInfo SequenceWithAnonymityChoice::SELECTION_INFO_ARRAY[] = {
    {
        e_SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *SequenceWithAnonymityChoice::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (name[0]=='s'
             && name[1]=='e'
             && name[2]=='l'
             && name[3]=='e'
             && name[4]=='c'
             && name[5]=='t'
             && name[6]=='i'
             && name[7]=='o'
             && name[8]=='n')
            {
                switch(name[9]) {
                    case '1': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *SequenceWithAnonymityChoice::lookupSelectionInfo(int id)
{
    switch (id) {
      case e_SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
      case e_SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
      case e_SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
      case e_SELECTION_ID_SELECTION4:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
      default:
        return 0;
    }
}

// CREATORS

SequenceWithAnonymityChoice::SequenceWithAnonymityChoice(
    const SequenceWithAnonymityChoice& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            Sequence6(
                original.d_selection1.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            unsigned char(original.d_selection2.object());
      } break;
      case e_SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            CustomString(
                original.d_selection3.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            CustomInt(original.d_selection4.object());
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

SequenceWithAnonymityChoice&
SequenceWithAnonymityChoice::operator=(const SequenceWithAnonymityChoice& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case e_SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case e_SELECTION_ID_SELECTION3: {
            makeSelection3(rhs.d_selection3.object());
          } break;
          case e_SELECTION_ID_SELECTION4: {
            makeSelection4(rhs.d_selection4.object());
          } break;
          default:
            BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void SequenceWithAnonymityChoice::reset()
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        d_selection1.object().~Sequence6();
      } break;
      case e_SELECTION_ID_SELECTION2: {
        // no destruction required
      } break;
      case e_SELECTION_ID_SELECTION3: {
        d_selection3.object().~CustomString();
      } break;
      case e_SELECTION_ID_SELECTION4: {
        d_selection4.object().~CustomInt();
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = e_SELECTION_ID_UNDEFINED;
}

int SequenceWithAnonymityChoice::makeSelection(int selectionId)
{
    switch (selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case e_SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case e_SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case e_SELECTION_ID_SELECTION4: {
        makeSelection4();
      } break;
      case e_SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;                                                    // RETURN
    }
    return 0;
}

int SequenceWithAnonymityChoice::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;                                                     // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

Sequence6& SequenceWithAnonymityChoice::makeSelection1()
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence6(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

Sequence6& SequenceWithAnonymityChoice::makeSelection1(const Sequence6& value)
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence6(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

unsigned char& SequenceWithAnonymityChoice::makeSelection2()
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
            unsigned char();

        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

unsigned char& SequenceWithAnonymityChoice::makeSelection2(unsigned char value)
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                unsigned char(value);
        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

CustomString& SequenceWithAnonymityChoice::makeSelection3()
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection3.object());
    }
    else {
        reset();
        new (d_selection3.buffer())
                CustomString(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

CustomString& SequenceWithAnonymityChoice::makeSelection3(const CustomString& value)
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = value;
    }
    else {
        reset();
        new (d_selection3.buffer())
                CustomString(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

CustomInt& SequenceWithAnonymityChoice::makeSelection4()
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection4.object());
    }
    else {
        reset();
        new (d_selection4.buffer())
            CustomInt();

        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

CustomInt& SequenceWithAnonymityChoice::makeSelection4(const CustomInt& value)
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        d_selection4.object() = value;
    }
    else {
        reset();
        new (d_selection4.buffer())
                CustomInt(value);
        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

// ACCESSORS

bsl::ostream& SequenceWithAnonymityChoice::print(
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

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, (int)d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, d_selection4.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, (int)d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, d_selection4.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}


const char *SequenceWithAnonymityChoice::selectionName() const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1].name();
      case e_SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2].name();
      case e_SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3].name();
      case e_SELECTION_ID_SELECTION4:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4].name();
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
}

                               // -------------
                               // class Choice1
                               // -------------

// CONSTANTS

const char Choice1::CLASS_NAME[] = "Choice1";

const bdlat_SelectionInfo Choice1::SELECTION_INFO_ARRAY[] = {
    {
        e_SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *Choice1::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (name[0]=='s'
             && name[1]=='e'
             && name[2]=='l'
             && name[3]=='e'
             && name[4]=='c'
             && name[5]=='t'
             && name[6]=='i'
             && name[7]=='o'
             && name[8]=='n')
            {
                switch(name[9]) {
                    case '1': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *Choice1::lookupSelectionInfo(int id)
{
    switch (id) {
      case e_SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
      case e_SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
      case e_SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
      case e_SELECTION_ID_SELECTION4:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
      default:
        return 0;
    }
}

// CREATORS

Choice1::Choice1(
    const Choice1& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            int(original.d_selection1.object());
      } break;
      case e_SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            double(original.d_selection2.object());
      } break;
      case e_SELECTION_ID_SELECTION3: {
        d_selection3 = new (*d_allocator_p)
                Sequence4(*original.d_selection3, d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION4: {
        d_selection4 = new (*d_allocator_p)
                Choice2(*original.d_selection4, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Choice1&
Choice1::operator=(const Choice1& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case e_SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case e_SELECTION_ID_SELECTION3: {
            makeSelection3(*rhs.d_selection3);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            makeSelection4(*rhs.d_selection4);
          } break;
          default:
            BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Choice1::reset()
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      case e_SELECTION_ID_SELECTION2: {
        // no destruction required
      } break;
      case e_SELECTION_ID_SELECTION3: {
        d_allocator_p->deleteObject(d_selection3);
      } break;
      case e_SELECTION_ID_SELECTION4: {
        d_allocator_p->deleteObject(d_selection4);
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = e_SELECTION_ID_UNDEFINED;
}

int Choice1::makeSelection(int selectionId)
{
    switch (selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case e_SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case e_SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case e_SELECTION_ID_SELECTION4: {
        makeSelection4();
      } break;
      case e_SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;                                                    // RETURN
    }
    return 0;
}

int Choice1::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;                                                     // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

int& Choice1::makeSelection1()
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
            int();

        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

int& Choice1::makeSelection1(int value)
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                int(value);
        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

double& Choice1::makeSelection2()
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
            double();

        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

double& Choice1::makeSelection2(double value)
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                double(value);
        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

Sequence4& Choice1::makeSelection3()
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection3);
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Sequence4(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

Sequence4& Choice1::makeSelection3(const Sequence4& value)
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        *d_selection3 = value;
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Sequence4(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

Choice2& Choice1::makeSelection4()
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection4);
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Choice2(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

Choice2& Choice1::makeSelection4(const Choice2& value)
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        *d_selection4 = value;
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Choice2(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

// ACCESSORS

bsl::ostream& Choice1::print(
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

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, *d_selection3,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, *d_selection4,
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, *d_selection3,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, *d_selection4,
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}


const char *Choice1::selectionName() const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1].name();
      case e_SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2].name();
      case e_SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3].name();
      case e_SELECTION_ID_SELECTION4:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4].name();
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
}

                               // -------------
                               // class Choice2
                               // -------------

// CONSTANTS

const char Choice2::CLASS_NAME[] = "Choice2";

const bdlat_SelectionInfo Choice2::SELECTION_INFO_ARRAY[] = {
    {
        e_SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *Choice2::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (name[0]=='s'
             && name[1]=='e'
             && name[2]=='l'
             && name[3]=='e'
             && name[4]=='c'
             && name[5]=='t'
             && name[6]=='i'
             && name[7]=='o'
             && name[8]=='n')
            {
                switch(name[9]) {
                    case '1': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *Choice2::lookupSelectionInfo(int id)
{
    switch (id) {
      case e_SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
      case e_SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
      case e_SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
      case e_SELECTION_ID_SELECTION4:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
      default:
        return 0;
    }
}

// CREATORS

Choice2::Choice2(
    const Choice2& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            bool(original.d_selection1.object());
      } break;
      case e_SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::string(
                original.d_selection2.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION3: {
        d_selection3 = new (*d_allocator_p)
                Choice1(*original.d_selection3, d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            unsigned int(original.d_selection4.object());
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Choice2&
Choice2::operator=(const Choice2& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case e_SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case e_SELECTION_ID_SELECTION3: {
            makeSelection3(*rhs.d_selection3);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            makeSelection4(rhs.d_selection4.object());
          } break;
          default:
            BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Choice2::reset()
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      case e_SELECTION_ID_SELECTION2: {
        typedef bsl::string Type;
        d_selection2.object().~Type();
      } break;
      case e_SELECTION_ID_SELECTION3: {
        d_allocator_p->deleteObject(d_selection3);
      } break;
      case e_SELECTION_ID_SELECTION4: {
        // no destruction required
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = e_SELECTION_ID_UNDEFINED;
}

int Choice2::makeSelection(int selectionId)
{
    switch (selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case e_SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case e_SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case e_SELECTION_ID_SELECTION4: {
        makeSelection4();
      } break;
      case e_SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;                                                    // RETURN
    }
    return 0;
}

int Choice2::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;                                                     // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

bool& Choice2::makeSelection1()
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
            bool();

        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

bool& Choice2::makeSelection1(bool value)
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                bool(value);
        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

bsl::string& Choice2::makeSelection2()
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::string(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

bsl::string& Choice2::makeSelection2(const bsl::string& value)
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

Choice1& Choice2::makeSelection3()
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection3);
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Choice1(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

Choice1& Choice2::makeSelection3(const Choice1& value)
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        *d_selection3 = value;
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Choice1(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

unsigned int& Choice2::makeSelection4()
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection4.object());
    }
    else {
        reset();
        new (d_selection4.buffer())
            unsigned int();

        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

unsigned int& Choice2::makeSelection4(unsigned int value)
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        d_selection4.object() = value;
    }
    else {
        reset();
        new (d_selection4.buffer())
                unsigned int(value);
        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

// ACCESSORS

bsl::ostream& Choice2::print(
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

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, *d_selection3,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, d_selection4.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, *d_selection3,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, d_selection4.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}


const char *Choice2::selectionName() const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1].name();
      case e_SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2].name();
      case e_SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3].name();
      case e_SELECTION_ID_SELECTION4:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4].name();
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
}

                              // ---------------
                              // class Sequence4
                              // ---------------

// CONSTANTS

const char Sequence4::CLASS_NAME[] = "Sequence4";

const bdlat_AttributeInfo Sequence4::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_HEX
    },
    {
        e_ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT8,
        "element8",
        sizeof("element8") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT9,
        "element9",
        sizeof("element9") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT10,
        "element10",
        sizeof("element10") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT11,
        "element11",
        sizeof("element11") - 1,
        "",
        bdlat_FormattingMode::e_HEX
    },
    {
        e_ATTRIBUTE_ID_ELEMENT12,
        "element12",
        sizeof("element12") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT13,
        "element13",
        sizeof("element13") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT14,
        "element14",
        sizeof("element14") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT15,
        "element15",
        sizeof("element15") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT16,
        "element16",
        sizeof("element16") - 1,
        "",
        bdlat_FormattingMode::e_HEX
    },
    {
        e_ATTRIBUTE_ID_ELEMENT17,
        "element17",
        sizeof("element17") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT18,
        "element18",
        sizeof("element18") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT19,
        "element19",
        sizeof("element19") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence4::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (name[0]=='e'
             && name[1]=='l'
             && name[2]=='e'
             && name[3]=='m'
             && name[4]=='e'
             && name[5]=='n'
             && name[6]=='t')
            {
                switch(name[7]) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
                                                                      // RETURN
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6];
                                                                      // RETURN
                    } break;
                    case '7': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7];
                                                                      // RETURN
                    } break;
                    case '8': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8];
                                                                      // RETURN
                    } break;
                    case '9': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
        case 9: {
            if (name[0]=='e'
             && name[1]=='l'
             && name[2]=='e'
             && name[3]=='m'
             && name[4]=='e'
             && name[5]=='n'
             && name[6]=='t'
             && name[7]=='1')
            {
                switch(name[8]) {
                    case '0': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10];
                                                                      // RETURN
                    } break;
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15];
                                                                      // RETURN
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT16];
                                                                      // RETURN
                    } break;
                    case '7': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT17];
                                                                      // RETURN
                    } break;
                    case '8': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT18];
                                                                      // RETURN
                    } break;
                    case '9': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT19];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Sequence4::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
      case e_ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
      case e_ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
      case e_ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
      case e_ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
      case e_ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT6];
      case e_ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT7];
      case e_ATTRIBUTE_ID_ELEMENT8:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT8];
      case e_ATTRIBUTE_ID_ELEMENT9:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT9];
      case e_ATTRIBUTE_ID_ELEMENT10:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT10];
      case e_ATTRIBUTE_ID_ELEMENT11:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT11];
      case e_ATTRIBUTE_ID_ELEMENT12:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT12];
      case e_ATTRIBUTE_ID_ELEMENT13:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT13];
      case e_ATTRIBUTE_ID_ELEMENT14:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT14];
      case e_ATTRIBUTE_ID_ELEMENT15:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT15];
      case e_ATTRIBUTE_ID_ELEMENT16:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT16];
      case e_ATTRIBUTE_ID_ELEMENT17:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT17];
      case e_ATTRIBUTE_ID_ELEMENT18:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT18];
      case e_ATTRIBUTE_ID_ELEMENT19:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT19];
      default:
        return 0;
    }
}

// CREATORS

Sequence4::Sequence4(bslma::Allocator *basicAllocator)
: d_element10()
, d_element17(basicAllocator)
, d_element15(basicAllocator)
, d_element11(basicAllocator)
, d_element16(basicAllocator)
, d_element14(basicAllocator)
, d_element18(basicAllocator)
, d_element1(basicAllocator)
, d_element19(basicAllocator)
, d_element2(basicAllocator)
, d_element9(basicAllocator)
, d_element3(basicAllocator)
, d_element5()
, d_element6(basicAllocator)
, d_element12()
, d_element4()
, d_element7()
, d_element13(static_cast<Enumerated::Value>(0))
, d_element8()
{
}

Sequence4::Sequence4(const Sequence4& original,
                     bslma::Allocator *basicAllocator)
: d_element10(original.d_element10)
, d_element17(original.d_element17, basicAllocator)
, d_element15(original.d_element15, basicAllocator)
, d_element11(original.d_element11, basicAllocator)
, d_element16(original.d_element16, basicAllocator)
, d_element14(original.d_element14, basicAllocator)
, d_element18(original.d_element18, basicAllocator)
, d_element1(original.d_element1, basicAllocator)
, d_element19(original.d_element19, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_element9(original.d_element9, basicAllocator)
, d_element3(original.d_element3, basicAllocator)
, d_element5(original.d_element5)
, d_element6(original.d_element6, basicAllocator)
, d_element12(original.d_element12)
, d_element4(original.d_element4)
, d_element7(original.d_element7)
, d_element13(original.d_element13)
, d_element8(original.d_element8)
{
}

Sequence4::~Sequence4()
{
}

// MANIPULATORS

Sequence4&
Sequence4::operator=(const Sequence4& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
        d_element6 = rhs.d_element6;
        d_element7 = rhs.d_element7;
        d_element8 = rhs.d_element8;
        d_element9 = rhs.d_element9;
        d_element10 = rhs.d_element10;
        d_element11 = rhs.d_element11;
        d_element12 = rhs.d_element12;
        d_element13 = rhs.d_element13;
        d_element14 = rhs.d_element14;
        d_element15 = rhs.d_element15;
        d_element16 = rhs.d_element16;
        d_element17 = rhs.d_element17;
        d_element18 = rhs.d_element18;
        d_element19 = rhs.d_element19;
    }
    return *this;
}

void Sequence4::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
    bdlat_ValueTypeFunctions::reset(&d_element6);
    bdlat_ValueTypeFunctions::reset(&d_element7);
    bdlat_ValueTypeFunctions::reset(&d_element8);
    bdlat_ValueTypeFunctions::reset(&d_element9);
    bdlat_ValueTypeFunctions::reset(&d_element10);
    bdlat_ValueTypeFunctions::reset(&d_element11);
    bdlat_ValueTypeFunctions::reset(&d_element12);
    bdlat_ValueTypeFunctions::reset(&d_element13);
    bdlat_ValueTypeFunctions::reset(&d_element14);
    bdlat_ValueTypeFunctions::reset(&d_element15);
    bdlat_ValueTypeFunctions::reset(&d_element16);
    bdlat_ValueTypeFunctions::reset(&d_element17);
    bdlat_ValueTypeFunctions::reset(&d_element18);
    bdlat_ValueTypeFunctions::reset(&d_element19);
}

// ACCESSORS

bsl::ostream& Sequence4::print(
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
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element6 = ";
        bdlb::PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element7 = ";
        bdlb::PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element8 = ";
        bdlb::PrintMethods::print(stream, d_element8,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element9 = ";
        bdlb::PrintMethods::print(stream, d_element9,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element10 = ";
        bdlb::PrintMethods::print(stream, d_element10,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element11 = ";
        bdlb::PrintMethods::print(stream, d_element11,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element12 = ";
        bdlb::PrintMethods::print(stream, d_element12,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element13 = ";
        bdlb::PrintMethods::print(stream, d_element13,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element14 = ";
        bdlb::PrintMethods::print(stream, d_element14,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element15 = ";
        bdlb::PrintMethods::print(stream, d_element15,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element16 = ";
        bdlb::PrintMethods::print(stream, d_element16,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element17 = ";
        bdlb::PrintMethods::print(stream, d_element17,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element18 = ";
        bdlb::PrintMethods::print(stream, d_element18,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element19 = ";
        bdlb::PrintMethods::print(stream, d_element19,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element6 = ";
        bdlb::PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element7 = ";
        bdlb::PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element8 = ";
        bdlb::PrintMethods::print(stream, d_element8,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element9 = ";
        bdlb::PrintMethods::print(stream, d_element9,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element10 = ";
        bdlb::PrintMethods::print(stream, d_element10,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element11 = ";
        bdlb::PrintMethods::print(stream, d_element11,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element12 = ";
        bdlb::PrintMethods::print(stream, d_element12,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element13 = ";
        bdlb::PrintMethods::print(stream, d_element13,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element14 = ";
        bdlb::PrintMethods::print(stream, d_element14,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element15 = ";
        bdlb::PrintMethods::print(stream, d_element15,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element16 = ";
        bdlb::PrintMethods::print(stream, d_element16,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element17 = ";
        bdlb::PrintMethods::print(stream, d_element17,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element18 = ";
        bdlb::PrintMethods::print(stream, d_element18,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element19 = ";
        bdlb::PrintMethods::print(stream, d_element19,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}



                              // ---------------
                              // class Sequence1
                              // ---------------

// CONSTANTS

const char Sequence1::CLASS_NAME[] = "Sequence1";

const bdlat_AttributeInfo Sequence1::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_NILLABLE
    },
    {
        e_ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence1::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (name[0]=='e'
             && name[1]=='l'
             && name[2]=='e'
             && name[3]=='m'
             && name[4]=='e'
             && name[5]=='n'
             && name[6]=='t')
            {
                switch(name[7]) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Sequence1::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
      case e_ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
      case e_ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
      case e_ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
      case e_ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
      default:
        return 0;
    }
}

// CREATORS

Sequence1::Sequence1(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element4(basicAllocator)
, d_element5(basicAllocator)
, d_element2(basicAllocator)
, d_element1(basicAllocator)
{
    d_element3 = new (*d_allocator_p)
            Choice2(d_allocator_p);
}

Sequence1::Sequence1(const Sequence1& original,
                     bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_element4(original.d_element4, basicAllocator)
, d_element5(original.d_element5, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_element1(original.d_element1, basicAllocator)
{
    d_element3 = new (*d_allocator_p)
            Choice2(*original.d_element3, d_allocator_p);
}

Sequence1::~Sequence1()
{
    d_allocator_p->deleteObject(d_element3);
}

// MANIPULATORS

Sequence1&
Sequence1::operator=(const Sequence1& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        *d_element3 = *rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
    }
    return *this;
}

void Sequence1::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
}

// ACCESSORS

bsl::ostream& Sequence1::print(
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
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, *d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, *d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}



                              // ---------------
                              // class Sequence2
                              // ---------------

// CONSTANTS

const char Sequence2::CLASS_NAME[] = "Sequence2";

const bdlat_AttributeInfo Sequence2::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *Sequence2::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 8: {
            if (name[0]=='e'
             && name[1]=='l'
             && name[2]=='e'
             && name[3]=='m'
             && name[4]=='e'
             && name[5]=='n'
             && name[6]=='t')
            {
                switch(name[7]) {
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *Sequence2::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT1];
      case e_ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT2];
      case e_ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT3];
      case e_ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
      case e_ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT5];
      default:
        return 0;
    }
}

// CREATORS

Sequence2::Sequence2(bslma::Allocator *basicAllocator)
: d_element5()
, d_element4(basicAllocator)
, d_element3()
, d_element1(basicAllocator)
, d_element2()
{
}

Sequence2::Sequence2(const Sequence2&  original,
                     bslma::Allocator *basicAllocator)
: d_element5(original.d_element5)
, d_element4(original.d_element4, basicAllocator)
, d_element3(original.d_element3)
, d_element1(original.d_element1, basicAllocator)
, d_element2(original.d_element2)
{
}

Sequence2::~Sequence2()
{
}

// MANIPULATORS

Sequence2&
Sequence2::operator=(const Sequence2& rhs)
{
    if (this != &rhs) {
        d_element1 = rhs.d_element1;
        d_element2 = rhs.d_element2;
        d_element3 = rhs.d_element3;
        d_element4 = rhs.d_element4;
        d_element5 = rhs.d_element5;
    }
    return *this;
}

void Sequence2::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_element1);
    bdlat_ValueTypeFunctions::reset(&d_element2);
    bdlat_ValueTypeFunctions::reset(&d_element3);
    bdlat_ValueTypeFunctions::reset(&d_element4);
    bdlat_ValueTypeFunctions::reset(&d_element5);
}

// ACCESSORS

bsl::ostream& Sequence2::print(
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
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, (int)d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdlb::PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdlb::PrintMethods::print(stream, (int)d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdlb::PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdlb::PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}



                     // ----------------------------------
                     // class SequenceWithAnonymityChoice2
                     // ----------------------------------

// CONSTANTS

const char SequenceWithAnonymityChoice2::CLASS_NAME[] = "SequenceWithAnonymityChoice2";

const bdlat_SelectionInfo SequenceWithAnonymityChoice2::SELECTION_INFO_ARRAY[] = {
    {
        e_SELECTION_ID_SELECTION7,
        "selection7",
        sizeof("selection7") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION8,
        "selection8",
        sizeof("selection8") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *SequenceWithAnonymityChoice2::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (name[0]=='s'
             && name[1]=='e'
             && name[2]=='l'
             && name[3]=='e'
             && name[4]=='c'
             && name[5]=='t'
             && name[6]=='i'
             && name[7]=='o'
             && name[8]=='n')
            {
                switch(name[9]) {
                    case '7': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7];
                                                                      // RETURN
                    } break;
                    case '8': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *SequenceWithAnonymityChoice2::lookupSelectionInfo(int id)
{
    switch (id) {
      case e_SELECTION_ID_SELECTION7:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7];
      case e_SELECTION_ID_SELECTION8:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8];
      default:
        return 0;
    }
}

// CREATORS

SequenceWithAnonymityChoice2::SequenceWithAnonymityChoice2(
    const SequenceWithAnonymityChoice2& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION7: {
        d_selection7 = new (*d_allocator_p)
                Sequence4(*original.d_selection7, d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION8: {
        d_selection8 = new (*d_allocator_p)
                Choice2(*original.d_selection8, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

SequenceWithAnonymityChoice2&
SequenceWithAnonymityChoice2::operator=(const SequenceWithAnonymityChoice2& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case e_SELECTION_ID_SELECTION7: {
            makeSelection7(*rhs.d_selection7);
          } break;
          case e_SELECTION_ID_SELECTION8: {
            makeSelection8(*rhs.d_selection8);
          } break;
          default:
            BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void SequenceWithAnonymityChoice2::reset()
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION7: {
        d_allocator_p->deleteObject(d_selection7);
      } break;
      case e_SELECTION_ID_SELECTION8: {
        d_allocator_p->deleteObject(d_selection8);
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = e_SELECTION_ID_UNDEFINED;
}

int SequenceWithAnonymityChoice2::makeSelection(int selectionId)
{
    switch (selectionId) {
      case e_SELECTION_ID_SELECTION7: {
        makeSelection7();
      } break;
      case e_SELECTION_ID_SELECTION8: {
        makeSelection8();
      } break;
      case e_SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;                                                    // RETURN
    }
    return 0;
}

int SequenceWithAnonymityChoice2::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;                                                     // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

Sequence4& SequenceWithAnonymityChoice2::makeSelection7()
{
    if (e_SELECTION_ID_SELECTION7 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection7);
    }
    else {
        reset();
        d_selection7 = new (*d_allocator_p)
                Sequence4(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION7;
    }

    return *d_selection7;
}

Sequence4& SequenceWithAnonymityChoice2::makeSelection7(const Sequence4& value)
{
    if (e_SELECTION_ID_SELECTION7 == d_selectionId) {
        *d_selection7 = value;
    }
    else {
        reset();
        d_selection7 = new (*d_allocator_p)
                Sequence4(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION7;
    }

    return *d_selection7;
}

Choice2& SequenceWithAnonymityChoice2::makeSelection8()
{
    if (e_SELECTION_ID_SELECTION8 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection8);
    }
    else {
        reset();
        d_selection8 = new (*d_allocator_p)
                Choice2(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION8;
    }

    return *d_selection8;
}

Choice2& SequenceWithAnonymityChoice2::makeSelection8(const Choice2& value)
{
    if (e_SELECTION_ID_SELECTION8 == d_selectionId) {
        *d_selection8 = value;
    }
    else {
        reset();
        d_selection8 = new (*d_allocator_p)
                Choice2(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION8;
    }

    return *d_selection8;
}

// ACCESSORS

bsl::ostream& SequenceWithAnonymityChoice2::print(
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

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION7: {
            stream << "Selection7 = ";
            bdlb::PrintMethods::print(stream, *d_selection7,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION8: {
            stream << "Selection8 = ";
            bdlb::PrintMethods::print(stream, *d_selection8,
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION7: {
            stream << "Selection7 = ";
            bdlb::PrintMethods::print(stream, *d_selection7,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION8: {
            stream << "Selection8 = ";
            bdlb::PrintMethods::print(stream, *d_selection8,
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}


const char *SequenceWithAnonymityChoice2::selectionName() const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION7:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7].name();
      case e_SELECTION_ID_SELECTION8:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8].name();
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
}

                        // ---------------------------
                        // class SequenceWithAnonymity
                        // ---------------------------

// CONSTANTS

const char SequenceWithAnonymity::CLASS_NAME[] = "SequenceWithAnonymity";

const bdlat_AttributeInfo SequenceWithAnonymity::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_CHOICE,
        "Choice",
        sizeof("Choice") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        e_ATTRIBUTE_ID_CHOICE1,
        "Choice-1",
        sizeof("Choice-1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        e_ATTRIBUTE_ID_CHOICE2,
        "Choice-2",
        sizeof("Choice-2") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
      | bdlat_FormattingMode::e_UNTAGGED
    },
    {
        e_ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *SequenceWithAnonymity::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    if (bdlb::String::areEqualCaseless("selection1", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE];         // RETURN
    }

    if (bdlb::String::areEqualCaseless("selection2", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE];         // RETURN
    }

    if (bdlb::String::areEqualCaseless("selection3", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE];         // RETURN
    }

    if (bdlb::String::areEqualCaseless("selection4", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE];         // RETURN
    }

    if (bdlb::String::areEqualCaseless("selection5", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE1];        // RETURN
    }

    if (bdlb::String::areEqualCaseless("selection6", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE1];        // RETURN
    }

    if (bdlb::String::areEqualCaseless("selection7", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE2];        // RETURN
    }

    if (bdlb::String::areEqualCaseless("selection8", name, nameLength)) {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE2];        // RETURN
    }

    switch(nameLength) {
        case 6: {
            if (name[0]=='C'
             && name[1]=='h'
             && name[2]=='o'
             && name[3]=='i'
             && name[4]=='c'
             && name[5]=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE]; // RETURN
            }
        } break;
        case 8: {
            switch(name[0]) {
                case 'C': {
                    if (name[1]=='h'
                     && name[2]=='o'
                     && name[3]=='i'
                     && name[4]=='c'
                     && name[5]=='e'
                     && name[6]=='-')
                    {
                        switch(name[7]) {
                            case '1': {
                                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE1];
                                                                      // RETURN
                            } break;
                            case '2': {
                                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE2];
                                                                      // RETURN
                            } break;
                        }
                    }
                } break;
                case 'e': {
                    if (name[1]=='l'
                     && name[2]=='e'
                     && name[3]=='m'
                     && name[4]=='e'
                     && name[5]=='n'
                     && name[6]=='t'
                     && name[7]=='4')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
                                                                      // RETURN
                    }
                } break;
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *SequenceWithAnonymity::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_CHOICE:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE];
      case e_ATTRIBUTE_ID_CHOICE1:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE1];
      case e_ATTRIBUTE_ID_CHOICE2:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_CHOICE2];
      case e_ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ELEMENT4];
      default:
        return 0;
    }
}

// CREATORS

SequenceWithAnonymity::SequenceWithAnonymity(bslma::Allocator *basicAllocator)
: d_choice2(basicAllocator)
, d_choice1(basicAllocator)
, d_choice(basicAllocator)
, d_element4(basicAllocator)
{
}

SequenceWithAnonymity::SequenceWithAnonymity(const SequenceWithAnonymity& original,
                                             bslma::Allocator *basicAllocator)
: d_choice2(original.d_choice2, basicAllocator)
, d_choice1(original.d_choice1, basicAllocator)
, d_choice(original.d_choice, basicAllocator)
, d_element4(original.d_element4, basicAllocator)
{
}

SequenceWithAnonymity::~SequenceWithAnonymity()
{
}

// MANIPULATORS

SequenceWithAnonymity&
SequenceWithAnonymity::operator=(const SequenceWithAnonymity& rhs)
{
    if (this != &rhs) {
        d_choice = rhs.d_choice;
        d_choice1 = rhs.d_choice1;
        d_choice2 = rhs.d_choice2;
        d_element4 = rhs.d_element4;
    }
    return *this;
}

void SequenceWithAnonymity::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_choice);
    bdlat_ValueTypeFunctions::reset(&d_choice1);
    bdlat_ValueTypeFunctions::reset(&d_choice2);
    bdlat_ValueTypeFunctions::reset(&d_element4);
}

// ACCESSORS

bsl::ostream& SequenceWithAnonymity::print(
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
        stream << "Choice = ";
        bdlb::PrintMethods::print(stream, d_choice,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Choice1 = ";
        bdlb::PrintMethods::print(stream, d_choice1,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Choice2 = ";
        bdlb::PrintMethods::print(stream, d_choice2,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Choice = ";
        bdlb::PrintMethods::print(stream, d_choice,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Choice1 = ";
        bdlb::PrintMethods::print(stream, d_choice1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Choice2 = ";
        bdlb::PrintMethods::print(stream, d_choice2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdlb::PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}



                          // ------------------------
                          // class FeatureTestMessage
                          // ------------------------

// CONSTANTS

const char FeatureTestMessage::CLASS_NAME[] = "FeatureTestMessage";

const bdlat_SelectionInfo FeatureTestMessage::SELECTION_INFO_ARRAY[] = {
    {
        e_SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdlat_FormattingMode::e_HEX
    },
    {
        e_SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION5,
        "selection5",
        sizeof("selection5") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION6,
        "selection6",
        sizeof("selection6") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_SELECTION_ID_SELECTION7,
        "selection7",
        sizeof("selection7") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION8,
        "selection8",
        sizeof("selection8") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION9,
        "selection9",
        sizeof("selection9") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION10,
        "selection10",
        sizeof("selection10") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_SELECTION11,
        "selection11",
        sizeof("selection11") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *FeatureTestMessage::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (name[0]=='s'
             && name[1]=='e'
             && name[2]=='l'
             && name[3]=='e'
             && name[4]=='c'
             && name[5]=='t'
             && name[6]=='i'
             && name[7]=='o'
             && name[8]=='n')
            {
                switch(name[9]) {
                    case '1': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
                                                                      // RETURN
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
                                                                      // RETURN
                    } break;
                    case '3': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
                                                                      // RETURN
                    } break;
                    case '4': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
                                                                      // RETURN
                    } break;
                    case '5': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5];
                                                                      // RETURN
                    } break;
                    case '6': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6];
                                                                      // RETURN
                    } break;
                    case '7': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7];
                                                                      // RETURN
                    } break;
                    case '8': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8];
                                                                      // RETURN
                    } break;
                    case '9': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION9];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
        case 11: {
            if (name[0]=='s'
             && name[1]=='e'
             && name[2]=='l'
             && name[3]=='e'
             && name[4]=='c'
             && name[5]=='t'
             && name[6]=='i'
             && name[7]=='o'
             && name[8]=='n'
             && name[9]=='1')
            {
                switch(name[10]) {
                    case '0': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION10];
                                                                      // RETURN
                    } break;
                    case '1': {
                        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION11];
                                                                      // RETURN
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *FeatureTestMessage::lookupSelectionInfo(int id)
{
    switch (id) {
      case e_SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1];
      case e_SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2];
      case e_SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3];
      case e_SELECTION_ID_SELECTION4:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4];
      case e_SELECTION_ID_SELECTION5:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5];
      case e_SELECTION_ID_SELECTION6:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6];
      case e_SELECTION_ID_SELECTION7:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7];
      case e_SELECTION_ID_SELECTION8:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8];
      case e_SELECTION_ID_SELECTION9:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION9];
      case e_SELECTION_ID_SELECTION10:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION10];
      case e_SELECTION_ID_SELECTION11:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION11];
      default:
        return 0;
    }
}

// CREATORS

FeatureTestMessage::FeatureTestMessage(
    const FeatureTestMessage& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            Sequence1(
                original.d_selection1.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            bsl::vector<char>(
                original.d_selection2.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            Sequence2(
                original.d_selection3.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION4: {
        d_selection4 = new (*d_allocator_p)
                Sequence3(*original.d_selection4, d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION5: {
        new (d_selection5.buffer())
            bdlt::DatetimeTz(original.d_selection5.object());
      } break;
      case e_SELECTION_ID_SELECTION6: {
        new (d_selection6.buffer())
            CustomString(
                original.d_selection6.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION7: {
        new (d_selection7.buffer())
            Enumerated::Value(original.d_selection7.object());
      } break;
      case e_SELECTION_ID_SELECTION8: {
        new (d_selection8.buffer())
            Choice3(
                original.d_selection8.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_SELECTION9: {
        new (d_selection9.buffer())
            VoidSequence(original.d_selection9.object());
      } break;
      case e_SELECTION_ID_SELECTION10: {
        new (d_selection10.buffer())
            UnsignedSequence(original.d_selection10.object());
      } break;
      case e_SELECTION_ID_SELECTION11: {
        new (d_selection11.buffer())
            SequenceWithAnonymity(
                original.d_selection11.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

FeatureTestMessage&
FeatureTestMessage::operator=(const FeatureTestMessage& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case e_SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case e_SELECTION_ID_SELECTION3: {
            makeSelection3(rhs.d_selection3.object());
          } break;
          case e_SELECTION_ID_SELECTION4: {
            makeSelection4(*rhs.d_selection4);
          } break;
          case e_SELECTION_ID_SELECTION5: {
            makeSelection5(rhs.d_selection5.object());
          } break;
          case e_SELECTION_ID_SELECTION6: {
            makeSelection6(rhs.d_selection6.object());
          } break;
          case e_SELECTION_ID_SELECTION7: {
            makeSelection7(rhs.d_selection7.object());
          } break;
          case e_SELECTION_ID_SELECTION8: {
            makeSelection8(rhs.d_selection8.object());
          } break;
          case e_SELECTION_ID_SELECTION9: {
            makeSelection9(rhs.d_selection9.object());
          } break;
          case e_SELECTION_ID_SELECTION10: {
            makeSelection10(rhs.d_selection10.object());
          } break;
          case e_SELECTION_ID_SELECTION11: {
            makeSelection11(rhs.d_selection11.object());
          } break;
          default:
            BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void FeatureTestMessage::reset()
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        d_selection1.object().~Sequence1();
      } break;
      case e_SELECTION_ID_SELECTION2: {
        typedef bsl::vector<char> Type;
        d_selection2.object().~Type();
      } break;
      case e_SELECTION_ID_SELECTION3: {
        d_selection3.object().~Sequence2();
      } break;
      case e_SELECTION_ID_SELECTION4: {
        d_allocator_p->deleteObject(d_selection4);
      } break;
      case e_SELECTION_ID_SELECTION5: {
        // no destruction required
      } break;
      case e_SELECTION_ID_SELECTION6: {
        d_selection6.object().~CustomString();
      } break;
      case e_SELECTION_ID_SELECTION7: {
        typedef Enumerated::Value Type;
        d_selection7.object().~Type();
      } break;
      case e_SELECTION_ID_SELECTION8: {
        d_selection8.object().~Choice3();
      } break;
      case e_SELECTION_ID_SELECTION9: {
        d_selection9.object().~VoidSequence();
      } break;
      case e_SELECTION_ID_SELECTION10: {
        d_selection10.object().~UnsignedSequence();
      } break;
      case e_SELECTION_ID_SELECTION11: {
        d_selection11.object().~SequenceWithAnonymity();
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = e_SELECTION_ID_UNDEFINED;
}

int FeatureTestMessage::makeSelection(int selectionId)
{
    switch (selectionId) {
      case e_SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case e_SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case e_SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case e_SELECTION_ID_SELECTION4: {
        makeSelection4();
      } break;
      case e_SELECTION_ID_SELECTION5: {
        makeSelection5();
      } break;
      case e_SELECTION_ID_SELECTION6: {
        makeSelection6();
      } break;
      case e_SELECTION_ID_SELECTION7: {
        makeSelection7();
      } break;
      case e_SELECTION_ID_SELECTION8: {
        makeSelection8();
      } break;
      case e_SELECTION_ID_SELECTION9: {
        makeSelection9();
      } break;
      case e_SELECTION_ID_SELECTION10: {
        makeSelection10();
      } break;
      case e_SELECTION_ID_SELECTION11: {
        makeSelection11();
      } break;
      case e_SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;                                                    // RETURN
    }
    return 0;
}

int FeatureTestMessage::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;                                                     // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

Sequence1& FeatureTestMessage::makeSelection1()
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence1(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

Sequence1& FeatureTestMessage::makeSelection1(const Sequence1& value)
{
    if (e_SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence1(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

bsl::vector<char>& FeatureTestMessage::makeSelection2()
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::vector<char>(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

bsl::vector<char>& FeatureTestMessage::makeSelection2(const bsl::vector<char>& value)
{
    if (e_SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                bsl::vector<char>(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

Sequence2& FeatureTestMessage::makeSelection3()
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection3.object());
    }
    else {
        reset();
        new (d_selection3.buffer())
                Sequence2(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

Sequence2& FeatureTestMessage::makeSelection3(const Sequence2& value)
{
    if (e_SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = value;
    }
    else {
        reset();
        new (d_selection3.buffer())
                Sequence2(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

Sequence3& FeatureTestMessage::makeSelection4()
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(d_selection4);
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Sequence3(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

Sequence3& FeatureTestMessage::makeSelection4(const Sequence3& value)
{
    if (e_SELECTION_ID_SELECTION4 == d_selectionId) {
        *d_selection4 = value;
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Sequence3(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

bdlt::DatetimeTz& FeatureTestMessage::makeSelection5()
{
    if (e_SELECTION_ID_SELECTION5 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection5.object());
    }
    else {
        reset();
        new (d_selection5.buffer())
            bdlt::DatetimeTz();

        d_selectionId = e_SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

bdlt::DatetimeTz& FeatureTestMessage::makeSelection5(const bdlt::DatetimeTz& value)
{
    if (e_SELECTION_ID_SELECTION5 == d_selectionId) {
        d_selection5.object() = value;
    }
    else {
        reset();
        new (d_selection5.buffer())
                bdlt::DatetimeTz(value);
        d_selectionId = e_SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

CustomString& FeatureTestMessage::makeSelection6()
{
    if (e_SELECTION_ID_SELECTION6 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection6.object());
    }
    else {
        reset();
        new (d_selection6.buffer())
                CustomString(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

CustomString& FeatureTestMessage::makeSelection6(const CustomString& value)
{
    if (e_SELECTION_ID_SELECTION6 == d_selectionId) {
        d_selection6.object() = value;
    }
    else {
        reset();
        new (d_selection6.buffer())
                CustomString(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

Enumerated::Value& FeatureTestMessage::makeSelection7()
{
    if (e_SELECTION_ID_SELECTION7 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection7.object());
    }
    else {
        reset();
        new (d_selection7.buffer())
                    Enumerated::Value(static_cast<Enumerated::Value>(0));

        d_selectionId = e_SELECTION_ID_SELECTION7;
    }

    return d_selection7.object();
}

Enumerated::Value& FeatureTestMessage::makeSelection7(Enumerated::Value value)
{
    if (e_SELECTION_ID_SELECTION7 == d_selectionId) {
        d_selection7.object() = value;
    }
    else {
        reset();
        new (d_selection7.buffer())
                Enumerated::Value(value);
        d_selectionId = e_SELECTION_ID_SELECTION7;
    }

    return d_selection7.object();
}

Choice3& FeatureTestMessage::makeSelection8()
{
    if (e_SELECTION_ID_SELECTION8 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection8.object());
    }
    else {
        reset();
        new (d_selection8.buffer())
                Choice3(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION8;
    }

    return d_selection8.object();
}

Choice3& FeatureTestMessage::makeSelection8(const Choice3& value)
{
    if (e_SELECTION_ID_SELECTION8 == d_selectionId) {
        d_selection8.object() = value;
    }
    else {
        reset();
        new (d_selection8.buffer())
                Choice3(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION8;
    }

    return d_selection8.object();
}

VoidSequence& FeatureTestMessage::makeSelection9()
{
    if (e_SELECTION_ID_SELECTION9 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection9.object());
    }
    else {
        reset();
        new (d_selection9.buffer())
            VoidSequence();

        d_selectionId = e_SELECTION_ID_SELECTION9;
    }

    return d_selection9.object();
}

VoidSequence& FeatureTestMessage::makeSelection9(const VoidSequence& value)
{
    if (e_SELECTION_ID_SELECTION9 == d_selectionId) {
        d_selection9.object() = value;
    }
    else {
        reset();
        new (d_selection9.buffer())
                VoidSequence(value);
        d_selectionId = e_SELECTION_ID_SELECTION9;
    }

    return d_selection9.object();
}

UnsignedSequence& FeatureTestMessage::makeSelection10()
{
    if (e_SELECTION_ID_SELECTION10 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection10.object());
    }
    else {
        reset();
        new (d_selection10.buffer())
            UnsignedSequence();

        d_selectionId = e_SELECTION_ID_SELECTION10;
    }

    return d_selection10.object();
}

UnsignedSequence& FeatureTestMessage::makeSelection10(const UnsignedSequence& value)
{
    if (e_SELECTION_ID_SELECTION10 == d_selectionId) {
        d_selection10.object() = value;
    }
    else {
        reset();
        new (d_selection10.buffer())
                UnsignedSequence(value);
        d_selectionId = e_SELECTION_ID_SELECTION10;
    }

    return d_selection10.object();
}

SequenceWithAnonymity& FeatureTestMessage::makeSelection11()
{
    if (e_SELECTION_ID_SELECTION11 == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_selection11.object());
    }
    else {
        reset();
        new (d_selection11.buffer())
                SequenceWithAnonymity(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SELECTION11;
    }

    return d_selection11.object();
}

SequenceWithAnonymity& FeatureTestMessage::makeSelection11(const SequenceWithAnonymity& value)
{
    if (e_SELECTION_ID_SELECTION11 == d_selectionId) {
        d_selection11.object() = value;
    }
    else {
        reset();
        new (d_selection11.buffer())
                SequenceWithAnonymity(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SELECTION11;
    }

    return d_selection11.object();
}

// ACCESSORS

bsl::ostream& FeatureTestMessage::print(
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

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, *d_selection4,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION5: {
            stream << "Selection5 = ";
            bdlb::PrintMethods::print(stream, d_selection5.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION6: {
            stream << "Selection6 = ";
            bdlb::PrintMethods::print(stream, d_selection6.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION7: {
            stream << "Selection7 = ";
            bdlb::PrintMethods::print(stream, d_selection7.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION8: {
            stream << "Selection8 = ";
            bdlb::PrintMethods::print(stream, d_selection8.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION9: {
            stream << "Selection9 = ";
            bdlb::PrintMethods::print(stream, d_selection9.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION10: {
            stream << "Selection10 = ";
            bdlb::PrintMethods::print(stream, d_selection10.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION11: {
            stream << "Selection11 = ";
            bdlb::PrintMethods::print(stream, d_selection11.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case e_SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdlb::PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdlb::PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdlb::PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdlb::PrintMethods::print(stream, *d_selection4,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION5: {
            stream << "Selection5 = ";
            bdlb::PrintMethods::print(stream, d_selection5.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION6: {
            stream << "Selection6 = ";
            bdlb::PrintMethods::print(stream, d_selection6.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION7: {
            stream << "Selection7 = ";
            bdlb::PrintMethods::print(stream, d_selection7.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION8: {
            stream << "Selection8 = ";
            bdlb::PrintMethods::print(stream, d_selection8.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION9: {
            stream << "Selection9 = ";
            bdlb::PrintMethods::print(stream, d_selection9.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION10: {
            stream << "Selection10 = ";
            bdlb::PrintMethods::print(stream, d_selection10.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_SELECTION11: {
            stream << "Selection11 = ";
            bdlb::PrintMethods::print(stream, d_selection11.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}


const char *FeatureTestMessage::selectionName() const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION1].name();
      case e_SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION2].name();
      case e_SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION3].name();
      case e_SELECTION_ID_SELECTION4:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION4].name();
      case e_SELECTION_ID_SELECTION5:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION5].name();
      case e_SELECTION_ID_SELECTION6:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION6].name();
      case e_SELECTION_ID_SELECTION7:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION7].name();
      case e_SELECTION_ID_SELECTION8:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION8].name();
      case e_SELECTION_ID_SELECTION9:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION9].name();
      case e_SELECTION_ID_SELECTION10:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION10].name();
      case e_SELECTION_ID_SELECTION11:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SELECTION11].name();
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
}

                               // -------------
                               // class Request
                               // -------------

// CONSTANTS

const char Request::CLASS_NAME[] = "Request";

const bdlat_SelectionInfo Request::SELECTION_INFO_ARRAY[] = {
    {
        e_SELECTION_ID_SIMPLE_REQUEST,
        "simpleRequest",
        sizeof("simpleRequest") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_SELECTION_ID_FEATURE_REQUEST,
        "featureRequest",
        sizeof("featureRequest") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *Request::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 13: {
            if (name[0]=='s'
             && name[1]=='i'
             && name[2]=='m'
             && name[3]=='p'
             && name[4]=='l'
             && name[5]=='e'
             && name[6]=='R'
             && name[7]=='e'
             && name[8]=='q'
             && name[9]=='u'
             && name[10]=='e'
             && name[11]=='s'
             && name[12]=='t')
            {
                return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SIMPLE_REQUEST];
                                                                      // RETURN
            }
        } break;
        case 14: {
            if (name[0]=='f'
             && name[1]=='e'
             && name[2]=='a'
             && name[3]=='t'
             && name[4]=='u'
             && name[5]=='r'
             && name[6]=='e'
             && name[7]=='R'
             && name[8]=='e'
             && name[9]=='q'
             && name[10]=='u'
             && name[11]=='e'
             && name[12]=='s'
             && name[13]=='t')
            {
                return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_REQUEST];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *Request::lookupSelectionInfo(int id)
{
    switch (id) {
      case e_SELECTION_ID_SIMPLE_REQUEST:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SIMPLE_REQUEST];
      case e_SELECTION_ID_FEATURE_REQUEST:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_REQUEST];
      default:
        return 0;
    }
}

// CREATORS

Request::Request(
    const Request&    original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SIMPLE_REQUEST: {
        new (d_simpleRequest.buffer())
            SimpleRequest(
                original.d_simpleRequest.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_FEATURE_REQUEST: {
        new (d_featureRequest.buffer())
            FeatureTestMessage(
                original.d_featureRequest.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Request&
Request::operator=(const Request& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case e_SELECTION_ID_SIMPLE_REQUEST: {
            makeSimpleRequest(rhs.d_simpleRequest.object());
          } break;
          case e_SELECTION_ID_FEATURE_REQUEST: {
            makeFeatureRequest(rhs.d_featureRequest.object());
          } break;
          default:
            BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Request::reset()
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SIMPLE_REQUEST: {
        d_simpleRequest.object().~SimpleRequest();
      } break;
      case e_SELECTION_ID_FEATURE_REQUEST: {
        d_featureRequest.object().~FeatureTestMessage();
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = e_SELECTION_ID_UNDEFINED;
}

int Request::makeSelection(int selectionId)
{
    switch (selectionId) {
      case e_SELECTION_ID_SIMPLE_REQUEST: {
        makeSimpleRequest();
      } break;
      case e_SELECTION_ID_FEATURE_REQUEST: {
        makeFeatureRequest();
      } break;
      case e_SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;                                                    // RETURN
    }
    return 0;
}

int Request::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;                                                     // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

SimpleRequest& Request::makeSimpleRequest()
{
    if (e_SELECTION_ID_SIMPLE_REQUEST == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_simpleRequest.object());
    }
    else {
        reset();
        new (d_simpleRequest.buffer())
                SimpleRequest(d_allocator_p);

        d_selectionId = e_SELECTION_ID_SIMPLE_REQUEST;
    }

    return d_simpleRequest.object();
}

SimpleRequest& Request::makeSimpleRequest(const SimpleRequest& value)
{
    if (e_SELECTION_ID_SIMPLE_REQUEST == d_selectionId) {
        d_simpleRequest.object() = value;
    }
    else {
        reset();
        new (d_simpleRequest.buffer())
                SimpleRequest(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_SIMPLE_REQUEST;
    }

    return d_simpleRequest.object();
}

FeatureTestMessage& Request::makeFeatureRequest()
{
    if (e_SELECTION_ID_FEATURE_REQUEST == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_featureRequest.object());
    }
    else {
        reset();
        new (d_featureRequest.buffer())
                FeatureTestMessage(d_allocator_p);

        d_selectionId = e_SELECTION_ID_FEATURE_REQUEST;
    }

    return d_featureRequest.object();
}

FeatureTestMessage& Request::makeFeatureRequest(const FeatureTestMessage& value)
{
    if (e_SELECTION_ID_FEATURE_REQUEST == d_selectionId) {
        d_featureRequest.object() = value;
    }
    else {
        reset();
        new (d_featureRequest.buffer())
                FeatureTestMessage(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_FEATURE_REQUEST;
    }

    return d_featureRequest.object();
}

// ACCESSORS

bsl::ostream& Request::print(
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

        switch (d_selectionId) {
          case e_SELECTION_ID_SIMPLE_REQUEST: {
            stream << "SimpleRequest = ";
            bdlb::PrintMethods::print(stream, d_simpleRequest.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_FEATURE_REQUEST: {
            stream << "FeatureRequest = ";
            bdlb::PrintMethods::print(stream, d_featureRequest.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case e_SELECTION_ID_SIMPLE_REQUEST: {
            stream << "SimpleRequest = ";
            bdlb::PrintMethods::print(stream, d_simpleRequest.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_FEATURE_REQUEST: {
            stream << "FeatureRequest = ";
            bdlb::PrintMethods::print(stream, d_featureRequest.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}


const char *Request::selectionName() const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_SIMPLE_REQUEST:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_SIMPLE_REQUEST].name();
      case e_SELECTION_ID_FEATURE_REQUEST:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_REQUEST].name();
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
}

                               // --------------
                               // class Response
                               // --------------

// CONSTANTS

const char Response::CLASS_NAME[] = "Response";

const bdlat_SelectionInfo Response::SELECTION_INFO_ARRAY[] = {
    {
        e_SELECTION_ID_RESPONSE_DATA,
        "responseData",
        sizeof("responseData") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_SELECTION_ID_FEATURE_RESPONSE,
        "featureResponse",
        sizeof("featureResponse") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *Response::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 12: {
            if (name[0]=='r'
             && name[1]=='e'
             && name[2]=='s'
             && name[3]=='p'
             && name[4]=='o'
             && name[5]=='n'
             && name[6]=='s'
             && name[7]=='e'
             && name[8]=='D'
             && name[9]=='a'
             && name[10]=='t'
             && name[11]=='a')
            {
                return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_RESPONSE_DATA];
                                                                      // RETURN
            }
        } break;
        case 15: {
            if (name[0]=='f'
             && name[1]=='e'
             && name[2]=='a'
             && name[3]=='t'
             && name[4]=='u'
             && name[5]=='r'
             && name[6]=='e'
             && name[7]=='R'
             && name[8]=='e'
             && name[9]=='s'
             && name[10]=='p'
             && name[11]=='o'
             && name[12]=='n'
             && name[13]=='s'
             && name[14]=='e')
            {
                return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_RESPONSE];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_SelectionInfo *Response::lookupSelectionInfo(int id)
{
    switch (id) {
      case e_SELECTION_ID_RESPONSE_DATA:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_RESPONSE_DATA];
      case e_SELECTION_ID_FEATURE_RESPONSE:
        return &SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_RESPONSE];
      default:
        return 0;
    }
}

// CREATORS

Response::Response(
    const Response&   original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case e_SELECTION_ID_RESPONSE_DATA: {
        new (d_responseData.buffer())
            bsl::string(
                original.d_responseData.object(), d_allocator_p);
      } break;
      case e_SELECTION_ID_FEATURE_RESPONSE: {
        new (d_featureResponse.buffer())
            FeatureTestMessage(
                original.d_featureResponse.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Response&
Response::operator=(const Response& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case e_SELECTION_ID_RESPONSE_DATA: {
            makeResponseData(rhs.d_responseData.object());
          } break;
          case e_SELECTION_ID_FEATURE_RESPONSE: {
            makeFeatureResponse(rhs.d_featureResponse.object());
          } break;
          default:
            BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Response::reset()
{
    switch (d_selectionId) {
      case e_SELECTION_ID_RESPONSE_DATA: {
        typedef bsl::string Type;
        d_responseData.object().~Type();
      } break;
      case e_SELECTION_ID_FEATURE_RESPONSE: {
        d_featureResponse.object().~FeatureTestMessage();
      } break;
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = e_SELECTION_ID_UNDEFINED;
}

int Response::makeSelection(int selectionId)
{
    switch (selectionId) {
      case e_SELECTION_ID_RESPONSE_DATA: {
        makeResponseData();
      } break;
      case e_SELECTION_ID_FEATURE_RESPONSE: {
        makeFeatureResponse();
      } break;
      case e_SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;                                                    // RETURN
    }
    return 0;
}

int Response::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;                                                     // RETURN
    }

    return makeSelection(selectionInfo->d_id);
}

bsl::string& Response::makeResponseData()
{
    if (e_SELECTION_ID_RESPONSE_DATA == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_responseData.object());
    }
    else {
        reset();
        new (d_responseData.buffer())
                bsl::string(d_allocator_p);

        d_selectionId = e_SELECTION_ID_RESPONSE_DATA;
    }

    return d_responseData.object();
}

bsl::string& Response::makeResponseData(const bsl::string& value)
{
    if (e_SELECTION_ID_RESPONSE_DATA == d_selectionId) {
        d_responseData.object() = value;
    }
    else {
        reset();
        new (d_responseData.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_RESPONSE_DATA;
    }

    return d_responseData.object();
}

FeatureTestMessage& Response::makeFeatureResponse()
{
    if (e_SELECTION_ID_FEATURE_RESPONSE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_featureResponse.object());
    }
    else {
        reset();
        new (d_featureResponse.buffer())
                FeatureTestMessage(d_allocator_p);

        d_selectionId = e_SELECTION_ID_FEATURE_RESPONSE;
    }

    return d_featureResponse.object();
}

FeatureTestMessage& Response::makeFeatureResponse(const FeatureTestMessage& value)
{
    if (e_SELECTION_ID_FEATURE_RESPONSE == d_selectionId) {
        d_featureResponse.object() = value;
    }
    else {
        reset();
        new (d_featureResponse.buffer())
                FeatureTestMessage(value, d_allocator_p);
        d_selectionId = e_SELECTION_ID_FEATURE_RESPONSE;
    }

    return d_featureResponse.object();
}

// ACCESSORS

bsl::ostream& Response::print(
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

        switch (d_selectionId) {
          case e_SELECTION_ID_RESPONSE_DATA: {
            stream << "ResponseData = ";
            bdlb::PrintMethods::print(stream, d_responseData.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_FEATURE_RESPONSE: {
            stream << "FeatureResponse = ";
            bdlb::PrintMethods::print(stream, d_featureResponse.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED\n";
        }
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << "[ ";

        switch (d_selectionId) {
          case e_SELECTION_ID_RESPONSE_DATA: {
            stream << "ResponseData = ";
            bdlb::PrintMethods::print(stream, d_responseData.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case e_SELECTION_ID_FEATURE_RESPONSE: {
            stream << "FeatureResponse = ";
            bdlb::PrintMethods::print(stream, d_featureResponse.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << bsl::flush;
}


const char *Response::selectionName() const
{
    switch (d_selectionId) {
      case e_SELECTION_ID_RESPONSE_DATA:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_RESPONSE_DATA].name();
      case e_SELECTION_ID_FEATURE_RESPONSE:
        return SELECTION_INFO_ARRAY[e_SELECTION_INDEX_FEATURE_RESPONSE].name();
      default:
        BSLS_ASSERT(e_SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";                                     // RETURN
    }
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
