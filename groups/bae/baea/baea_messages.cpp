// baea_messages.cpp       -- GENERATED FILE - DO NOT EDIT --       -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_messages_cpp,"$Id$ $CSID$")

#include <baea_messages.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdede_utf8util.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <bdes_platformutil.h>
#include <bdet_datetimetz.h>
#include <bdeut_nullableallocatedvalue.h>
#include <bdeut_nullablevalue.h>
#include <string>
#include <vector>

#include <bsls_assert.h>

#include <iomanip>
#include <limits>
#include <ostream>

namespace BloombergLP {
namespace baea {

                              // ---------------                               
                              // class CustomInt                               
                              // ---------------                               

// PRIVATE CLASS METHODS

int CustomInt::checkRestrictions(const int& value)
{
    if (1000 < value) {
        return -1;
    }

    return 0;
}

// CONSTANTS

const char CustomInt::CLASS_NAME[] = "CustomInt";



                             // ------------------                             
                             // class CustomString                             
                             // ------------------                             

// PRIVATE CLASS METHODS

int CustomString::checkRestrictions(const std::string& value)
{
    if (8 < bdede_Utf8Util::numCharacters(value.c_str(), value.length())) {
        return -1;
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

const bdeat_EnumeratorInfo Enumerated::ENUMERATOR_INFO_ARRAY[] = {
    {
        Enumerated::NEW_YORK,
        "NEW_YORK",
        sizeof("NEW_YORK") - 1,
        ""
    },
    {
        Enumerated::NEW_JERSEY,
        "NEW_JERSEY",
        sizeof("NEW_JERSEY") - 1,
        ""
    },
    {
        Enumerated::LONDON,
        "LONDON",
        sizeof("LONDON") - 1,
        ""
    }
};

// CLASS METHODS

int Enumerated::fromInt(Enumerated::Value *result, int number)
{
    switch (number) {
      case Enumerated::NEW_YORK:
      case Enumerated::NEW_JERSEY:
      case Enumerated::LONDON:
        *result = (Enumerated::Value)number;
        return 0;
      default:
        return -1;
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
                *result = Enumerated::LONDON;
                return 0;
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
                *result = Enumerated::NEW_YORK;
                return 0;
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
                *result = Enumerated::NEW_JERSEY;
                return 0;
            }
        } break;
    }
    
    return -1;
}

const char *Enumerated::toString(Enumerated::Value value)
{
    switch (value) {
      case NEW_YORK: {
        return "NEW_YORK";
      } break;
      case NEW_JERSEY: {
        return "NEW_JERSEY";
      } break;
      case LONDON: {
        return "LONDON";
      } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}


                            // -------------------                             
                            // class SimpleRequest                             
                            // -------------------                             

// CONSTANTS

const char SimpleRequest::CLASS_NAME[] = "SimpleRequest";

const bdeat_AttributeInfo SimpleRequest::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_DATA,
        "data",
        sizeof("data") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_RESPONSE_LENGTH,
        "responseLength",
        sizeof("responseLength") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *SimpleRequest::lookupAttributeInfo(
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
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA];
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
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *SimpleRequest::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_DATA:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DATA];
      case ATTRIBUTE_ID_RESPONSE_LENGTH:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_RESPONSE_LENGTH];
      default:
        return 0;
    }
}

// CREATORS

SimpleRequest::SimpleRequest(bdema_Allocator *basicAllocator)
: d_data(basicAllocator)
, d_responseLength()
{
}

SimpleRequest::SimpleRequest(const SimpleRequest& original,
                             bdema_Allocator *basicAllocator)
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
    bdeat_ValueTypeFunctions::reset(&d_data);
    bdeat_ValueTypeFunctions::reset(&d_responseLength);
}

// ACCESSORS

std::ostream& SimpleRequest::print(
    std::ostream& stream,
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
        stream << "Data = ";
        bdeu_PrintMethods::print(stream, d_data,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ResponseLength = ";
        bdeu_PrintMethods::print(stream, d_responseLength,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Data = ";
        bdeu_PrintMethods::print(stream, d_data,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ResponseLength = ";
        bdeu_PrintMethods::print(stream, d_responseLength,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << std::flush;
}



                           // ----------------------                           
                           // class UnsignedSequence                           
                           // ----------------------                           

// CONSTANTS

const char UnsignedSequence::CLASS_NAME[] = "UnsignedSequence";

const bdeat_AttributeInfo UnsignedSequence::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *UnsignedSequence::lookupAttributeInfo(
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
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *UnsignedSequence::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
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
    bdeat_ValueTypeFunctions::reset(&d_element1);
    bdeat_ValueTypeFunctions::reset(&d_element2);
    bdeat_ValueTypeFunctions::reset(&d_element3);
}

// ACCESSORS

std::ostream& UnsignedSequence::print(
    std::ostream& stream,
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
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << std::flush;
}



                             // ------------------                             
                             // class VoidSequence                             
                             // ------------------                             

// CONSTANTS

const char VoidSequence::CLASS_NAME[] = "VoidSequence";


// CLASS METHODS

const bdeat_AttributeInfo *VoidSequence::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        }
    return 0;
}

const bdeat_AttributeInfo *VoidSequence::lookupAttributeInfo(int id)
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

VoidSequence::VoidSequence(const VoidSequence& original)
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

std::ostream& VoidSequence::print(
    std::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << " ]";
    }

    return stream << std::flush;
}



                              // ---------------                               
                              // class Sequence3                               
                              // ---------------                               

// CONSTANTS

const char Sequence3::CLASS_NAME[] = "Sequence3";

const bdeat_AttributeInfo Sequence3::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
      | bdeat_FormattingMode::BDEAT_NILLABLE
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Sequence3::lookupAttributeInfo(
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
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Sequence3::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      case ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
      default:
        return 0;
    }
}

// CREATORS

Sequence3::Sequence3(bdema_Allocator *basicAllocator)
: d_element2(basicAllocator)
, d_element4(basicAllocator)
, d_element5(basicAllocator)
, d_element6(basicAllocator)
, d_element1(basicAllocator)
, d_element3()
{
}

Sequence3::Sequence3(const Sequence3& original,
                     bdema_Allocator *basicAllocator)
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
    bdeat_ValueTypeFunctions::reset(&d_element1);
    bdeat_ValueTypeFunctions::reset(&d_element2);
    bdeat_ValueTypeFunctions::reset(&d_element3);
    bdeat_ValueTypeFunctions::reset(&d_element4);
    bdeat_ValueTypeFunctions::reset(&d_element5);
    bdeat_ValueTypeFunctions::reset(&d_element6);
}

// ACCESSORS

std::ostream& Sequence3::print(
    std::ostream& stream,
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
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element6 = ";
        bdeu_PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element6 = ";
        bdeu_PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << std::flush;
}



                              // ---------------                               
                              // class Sequence5                               
                              // ---------------                               

// CONSTANTS

const char Sequence5::CLASS_NAME[] = "Sequence5";

const bdeat_AttributeInfo Sequence5::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdeat_FormattingMode::BDEAT_HEX
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
      | bdeat_FormattingMode::BDEAT_NILLABLE
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Sequence5::lookupAttributeInfo(
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
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
                    } break;
                    case '7': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Sequence5::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      case ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
      case ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
      default:
        return 0;
    }
}

// CREATORS

Sequence5::Sequence5(bdema_Allocator *basicAllocator)
: d_allocator_p(bdema_Default::allocator(basicAllocator))
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

Sequence5::Sequence5(const Sequence5& original,
                     bdema_Allocator *basicAllocator)
: d_allocator_p(bdema_Default::allocator(basicAllocator))
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
    bdeat_ValueTypeFunctions::reset(d_element1);
    bdeat_ValueTypeFunctions::reset(&d_element2);
    bdeat_ValueTypeFunctions::reset(&d_element3);
    bdeat_ValueTypeFunctions::reset(&d_element4);
    bdeat_ValueTypeFunctions::reset(&d_element5);
    bdeat_ValueTypeFunctions::reset(&d_element6);
    bdeat_ValueTypeFunctions::reset(&d_element7);
}

// ACCESSORS

std::ostream& Sequence5::print(
    std::ostream& stream,
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
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, *d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element6 = ";
        bdeu_PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element7 = ";
        bdeu_PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, *d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element6 = ";
        bdeu_PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element7 = ";
        bdeu_PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << std::flush;
}



                              // ---------------                               
                              // class Sequence6                               
                              // ---------------                               

// CONSTANTS

const char Sequence6::CLASS_NAME[] = "Sequence6";

const bdeat_AttributeInfo Sequence6::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT8,
        "element8",
        sizeof("element8") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT9,
        "element9",
        sizeof("element9") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT10,
        "element10",
        sizeof("element10") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
      | bdeat_FormattingMode::BDEAT_NILLABLE
    },
    {
        ATTRIBUTE_ID_ELEMENT11,
        "element11",
        sizeof("element11") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Sequence6::lookupAttributeInfo(
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
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
                    } break;
                    case '7': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
                    } break;
                    case '8': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8];
                    } break;
                    case '9': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9];
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
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10];
                    } break;
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Sequence6::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      case ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
      case ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
      case ATTRIBUTE_ID_ELEMENT8:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8];
      case ATTRIBUTE_ID_ELEMENT9:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9];
      case ATTRIBUTE_ID_ELEMENT10:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10];
      case ATTRIBUTE_ID_ELEMENT11:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11];
      default:
        return 0;
    }
}

// CREATORS

Sequence6::Sequence6(bdema_Allocator *basicAllocator)
: d_element8(basicAllocator)
, d_element10(basicAllocator)
, d_element9(basicAllocator)
, d_element2(basicAllocator)
, d_element6(basicAllocator)
, d_element5(basicAllocator)
, d_element11(basicAllocator)
, d_element3()
, d_element7()
, d_element4()
, d_element1()
{
}

Sequence6::Sequence6(const Sequence6& original,
                     bdema_Allocator *basicAllocator)
: d_element8(original.d_element8, basicAllocator)
, d_element10(original.d_element10, basicAllocator)
, d_element9(original.d_element9, basicAllocator)
, d_element2(original.d_element2, basicAllocator)
, d_element6(original.d_element6, basicAllocator)
, d_element5(original.d_element5, basicAllocator)
, d_element11(original.d_element11, basicAllocator)
, d_element3(original.d_element3)
, d_element7(original.d_element7)
, d_element4(original.d_element4)
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
    }
    return *this;
}

void Sequence6::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_element1);
    bdeat_ValueTypeFunctions::reset(&d_element2);
    bdeat_ValueTypeFunctions::reset(&d_element3);
    bdeat_ValueTypeFunctions::reset(&d_element4);
    bdeat_ValueTypeFunctions::reset(&d_element5);
    bdeat_ValueTypeFunctions::reset(&d_element6);
    bdeat_ValueTypeFunctions::reset(&d_element7);
    bdeat_ValueTypeFunctions::reset(&d_element8);
    bdeat_ValueTypeFunctions::reset(&d_element9);
    bdeat_ValueTypeFunctions::reset(&d_element10);
    bdeat_ValueTypeFunctions::reset(&d_element11);
}

// ACCESSORS

std::ostream& Sequence6::print(
    std::ostream& stream,
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
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, (int)d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element6 = ";
        bdeu_PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element7 = ";
        bdeu_PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element8 = ";
        bdeu_PrintMethods::print(stream, d_element8,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element9 = ";
        bdeu_PrintMethods::print(stream, d_element9,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element10 = ";
        bdeu_PrintMethods::print(stream, d_element10,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element11 = ";
        bdeu_PrintMethods::print(stream, d_element11,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, (int)d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element6 = ";
        bdeu_PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element7 = ";
        bdeu_PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element8 = ";
        bdeu_PrintMethods::print(stream, d_element8,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element9 = ";
        bdeu_PrintMethods::print(stream, d_element9,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element10 = ";
        bdeu_PrintMethods::print(stream, d_element10,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element11 = ";
        bdeu_PrintMethods::print(stream, d_element11,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << std::flush;
}



                               // -------------                                
                               // class Choice3                                
                               // -------------                                

// CONSTANTS

const char Choice3::CLASS_NAME[] = "Choice3";

const bdeat_SelectionInfo Choice3::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *Choice3::lookupSelectionInfo(
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
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                    } break;
                    case '3': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3];
                    } break;
                    case '4': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *Choice3::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
      case SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3];
      case SELECTION_ID_SELECTION4:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4];
      default:
        return 0;
    }
}

// CREATORS

Choice3::Choice3(
    const Choice3& original,
    bdema_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            Sequence6(
                original.d_selection1.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            unsigned char(original.d_selection2.object());
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            CustomString(
                original.d_selection3.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        new (d_selection4.buffer())
            CustomInt(original.d_selection4.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Choice3&
Choice3::operator=(const Choice3& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case SELECTION_ID_SELECTION3: {
            makeSelection3(rhs.d_selection3.object());
          } break;
          case SELECTION_ID_SELECTION4: {
            makeSelection4(rhs.d_selection4.object());
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Choice3::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        d_selection1.object().~Sequence6();
      } break;
      case SELECTION_ID_SELECTION2: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION3: {
        d_selection3.object().~CustomString();
      } break;
      case SELECTION_ID_SELECTION4: {
        d_selection4.object().~CustomInt();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Choice3::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case SELECTION_ID_SELECTION4: {
        makeSelection4();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int Choice3::makeSelection(const char *name, int nameLength)
{
    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

Sequence6& Choice3::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence6(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

Sequence6& Choice3::makeSelection1(const Sequence6& value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence6(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

unsigned char& Choice3::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
            unsigned char();
    
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

unsigned char& Choice3::makeSelection2(unsigned char value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                unsigned char(value);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

CustomString& Choice3::makeSelection3()
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection3.object());
    }
    else {
        reset();
        new (d_selection3.buffer())
                CustomString(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

CustomString& Choice3::makeSelection3(const CustomString& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = value;
    }
    else {
        reset();
        new (d_selection3.buffer())
                CustomString(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

CustomInt& Choice3::makeSelection4()
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection4.object());
    }
    else {
        reset();
        new (d_selection4.buffer())
            CustomInt();
    
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

CustomInt& Choice3::makeSelection4(const CustomInt& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        d_selection4.object() = value;
    }
    else {
        reset();
        new (d_selection4.buffer())
                CustomInt(value);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return d_selection4.object();
}

// ACCESSORS

std::ostream& Choice3::print(
    std::ostream& stream,
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, (int)d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdeu_PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdeu_PrintMethods::print(stream, d_selection4.object(),
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, (int)d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdeu_PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdeu_PrintMethods::print(stream, d_selection4.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << std::flush;
}


const char *Choice3::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1].name();
      case SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2].name();
      case SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3].name();
      case SELECTION_ID_SELECTION4:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                               // -------------                                
                               // class Choice1                                
                               // -------------                                

// CONSTANTS

const char Choice1::CLASS_NAME[] = "Choice1";

const bdeat_SelectionInfo Choice1::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *Choice1::lookupSelectionInfo(
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
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                    } break;
                    case '3': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3];
                    } break;
                    case '4': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *Choice1::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
      case SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3];
      case SELECTION_ID_SELECTION4:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4];
      default:
        return 0;
    }
}

// CREATORS

Choice1::Choice1(
    const Choice1& original,
    bdema_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            int(original.d_selection1.object());
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            double(original.d_selection2.object());
      } break;
      case SELECTION_ID_SELECTION3: {
        d_selection3 = new (*d_allocator_p)
                Sequence4(*original.d_selection3, d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        d_selection4 = new (*d_allocator_p)
                Choice2(*original.d_selection4, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Choice1&
Choice1::operator=(const Choice1& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case SELECTION_ID_SELECTION3: {
            makeSelection3(*rhs.d_selection3);
          } break;
          case SELECTION_ID_SELECTION4: {
            makeSelection4(*rhs.d_selection4);
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Choice1::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION2: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION3: {
        d_allocator_p->deleteObject(d_selection3);
      } break;
      case SELECTION_ID_SELECTION4: {
        d_allocator_p->deleteObject(d_selection4);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Choice1::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case SELECTION_ID_SELECTION4: {
        makeSelection4();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int Choice1::makeSelection(const char *name, int nameLength)
{
    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

int& Choice1::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
            int();
    
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

int& Choice1::makeSelection1(int value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                int(value);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

double& Choice1::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
            double();
    
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

double& Choice1::makeSelection2(double value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                double(value);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

Sequence4& Choice1::makeSelection3()
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(d_selection3);
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Sequence4(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

Sequence4& Choice1::makeSelection3(const Sequence4& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        *d_selection3 = value;
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Sequence4(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

Choice2& Choice1::makeSelection4()
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(d_selection4);
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Choice2(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

Choice2& Choice1::makeSelection4(const Choice2& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        *d_selection4 = value;
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Choice2(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

// ACCESSORS

std::ostream& Choice1::print(
    std::ostream& stream,
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdeu_PrintMethods::print(stream, *d_selection3,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdeu_PrintMethods::print(stream, *d_selection4,
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdeu_PrintMethods::print(stream, *d_selection3,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdeu_PrintMethods::print(stream, *d_selection4,
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << std::flush;
}


const char *Choice1::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1].name();
      case SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2].name();
      case SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3].name();
      case SELECTION_ID_SELECTION4:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                               // -------------                                
                               // class Choice2                                
                               // -------------                                

// CONSTANTS

const char Choice2::CLASS_NAME[] = "Choice2";

const bdeat_SelectionInfo Choice2::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *Choice2::lookupSelectionInfo(
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
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                    } break;
                    case '3': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *Choice2::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
      case SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3];
      default:
        return 0;
    }
}

// CREATORS

Choice2::Choice2(
    const Choice2& original,
    bdema_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            bool(original.d_selection1.object());
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            std::string(
                original.d_selection2.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION3: {
        d_selection3 = new (*d_allocator_p)
                Choice1(*original.d_selection3, d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Choice2&
Choice2::operator=(const Choice2& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case SELECTION_ID_SELECTION3: {
            makeSelection3(*rhs.d_selection3);
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Choice2::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION2: {
        typedef std::string Type;
        d_selection2.object().~Type();
      } break;
      case SELECTION_ID_SELECTION3: {
        d_allocator_p->deleteObject(d_selection3);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Choice2::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int Choice2::makeSelection(const char *name, int nameLength)
{
    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

bool& Choice2::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
            bool();
    
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

bool& Choice2::makeSelection1(bool value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                bool(value);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

std::string& Choice2::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
                std::string(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

std::string& Choice2::makeSelection2(const std::string& value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                std::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

Choice1& Choice2::makeSelection3()
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(d_selection3);
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Choice1(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

Choice1& Choice2::makeSelection3(const Choice1& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        *d_selection3 = value;
    }
    else {
        reset();
        d_selection3 = new (*d_allocator_p)
                Choice1(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return *d_selection3;
}

// ACCESSORS

std::ostream& Choice2::print(
    std::ostream& stream,
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdeu_PrintMethods::print(stream, *d_selection3,
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdeu_PrintMethods::print(stream, *d_selection3,
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << std::flush;
}


const char *Choice2::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1].name();
      case SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2].name();
      case SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                              // ---------------                               
                              // class Sequence4                               
                              // ---------------                               

// CONSTANTS

const char Sequence4::CLASS_NAME[] = "Sequence4";

const bdeat_AttributeInfo Sequence4::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_HEX
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT6,
        "element6",
        sizeof("element6") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT7,
        "element7",
        sizeof("element7") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT8,
        "element8",
        sizeof("element8") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT9,
        "element9",
        sizeof("element9") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT10,
        "element10",
        sizeof("element10") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT11,
        "element11",
        sizeof("element11") - 1,
        "",
        bdeat_FormattingMode::BDEAT_HEX
    },
    {
        ATTRIBUTE_ID_ELEMENT12,
        "element12",
        sizeof("element12") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT13,
        "element13",
        sizeof("element13") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT14,
        "element14",
        sizeof("element14") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT15,
        "element15",
        sizeof("element15") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT16,
        "element16",
        sizeof("element16") - 1,
        "",
        bdeat_FormattingMode::BDEAT_HEX
    },
    {
        ATTRIBUTE_ID_ELEMENT17,
        "element17",
        sizeof("element17") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT18,
        "element18",
        sizeof("element18") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT19,
        "element19",
        sizeof("element19") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Sequence4::lookupAttributeInfo(
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
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
                    } break;
                    case '7': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
                    } break;
                    case '8': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8];
                    } break;
                    case '9': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9];
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
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10];
                    } break;
                    case '1': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13];
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14];
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15];
                    } break;
                    case '6': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16];
                    } break;
                    case '7': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17];
                    } break;
                    case '8': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18];
                    } break;
                    case '9': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Sequence4::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      case ATTRIBUTE_ID_ELEMENT6:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6];
      case ATTRIBUTE_ID_ELEMENT7:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7];
      case ATTRIBUTE_ID_ELEMENT8:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8];
      case ATTRIBUTE_ID_ELEMENT9:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9];
      case ATTRIBUTE_ID_ELEMENT10:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10];
      case ATTRIBUTE_ID_ELEMENT11:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11];
      case ATTRIBUTE_ID_ELEMENT12:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12];
      case ATTRIBUTE_ID_ELEMENT13:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13];
      case ATTRIBUTE_ID_ELEMENT14:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14];
      case ATTRIBUTE_ID_ELEMENT15:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15];
      case ATTRIBUTE_ID_ELEMENT16:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16];
      case ATTRIBUTE_ID_ELEMENT17:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17];
      case ATTRIBUTE_ID_ELEMENT18:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18];
      case ATTRIBUTE_ID_ELEMENT19:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19];
      default:
        return 0;
    }
}

// CREATORS

Sequence4::Sequence4(bdema_Allocator *basicAllocator)
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
                     bdema_Allocator *basicAllocator)
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
    bdeat_ValueTypeFunctions::reset(&d_element1);
    bdeat_ValueTypeFunctions::reset(&d_element2);
    bdeat_ValueTypeFunctions::reset(&d_element3);
    bdeat_ValueTypeFunctions::reset(&d_element4);
    bdeat_ValueTypeFunctions::reset(&d_element5);
    bdeat_ValueTypeFunctions::reset(&d_element6);
    bdeat_ValueTypeFunctions::reset(&d_element7);
    bdeat_ValueTypeFunctions::reset(&d_element8);
    bdeat_ValueTypeFunctions::reset(&d_element9);
    bdeat_ValueTypeFunctions::reset(&d_element10);
    bdeat_ValueTypeFunctions::reset(&d_element11);
    bdeat_ValueTypeFunctions::reset(&d_element12);
    bdeat_ValueTypeFunctions::reset(&d_element13);
    bdeat_ValueTypeFunctions::reset(&d_element14);
    bdeat_ValueTypeFunctions::reset(&d_element15);
    bdeat_ValueTypeFunctions::reset(&d_element16);
    bdeat_ValueTypeFunctions::reset(&d_element17);
    bdeat_ValueTypeFunctions::reset(&d_element18);
    bdeat_ValueTypeFunctions::reset(&d_element19);
}

// ACCESSORS

std::ostream& Sequence4::print(
    std::ostream& stream,
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
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element6 = ";
        bdeu_PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element7 = ";
        bdeu_PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element8 = ";
        bdeu_PrintMethods::print(stream, d_element8,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element9 = ";
        bdeu_PrintMethods::print(stream, d_element9,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element10 = ";
        bdeu_PrintMethods::print(stream, d_element10,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element11 = ";
        bdeu_PrintMethods::print(stream, d_element11,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element12 = ";
        bdeu_PrintMethods::print(stream, d_element12,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element13 = ";
        bdeu_PrintMethods::print(stream, d_element13,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element14 = ";
        bdeu_PrintMethods::print(stream, d_element14,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element15 = ";
        bdeu_PrintMethods::print(stream, d_element15,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element16 = ";
        bdeu_PrintMethods::print(stream, d_element16,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element17 = ";
        bdeu_PrintMethods::print(stream, d_element17,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element18 = ";
        bdeu_PrintMethods::print(stream, d_element18,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element19 = ";
        bdeu_PrintMethods::print(stream, d_element19,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element6 = ";
        bdeu_PrintMethods::print(stream, d_element6,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element7 = ";
        bdeu_PrintMethods::print(stream, d_element7,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element8 = ";
        bdeu_PrintMethods::print(stream, d_element8,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element9 = ";
        bdeu_PrintMethods::print(stream, d_element9,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element10 = ";
        bdeu_PrintMethods::print(stream, d_element10,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element11 = ";
        bdeu_PrintMethods::print(stream, d_element11,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element12 = ";
        bdeu_PrintMethods::print(stream, d_element12,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element13 = ";
        bdeu_PrintMethods::print(stream, d_element13,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element14 = ";
        bdeu_PrintMethods::print(stream, d_element14,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element15 = ";
        bdeu_PrintMethods::print(stream, d_element15,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element16 = ";
        bdeu_PrintMethods::print(stream, d_element16,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element17 = ";
        bdeu_PrintMethods::print(stream, d_element17,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element18 = ";
        bdeu_PrintMethods::print(stream, d_element18,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element19 = ";
        bdeu_PrintMethods::print(stream, d_element19,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << std::flush;
}



                              // ---------------                               
                              // class Sequence1                               
                              // ---------------                               

// CONSTANTS

const char Sequence1::CLASS_NAME[] = "Sequence1";

const bdeat_AttributeInfo Sequence1::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Sequence1::lookupAttributeInfo(
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
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Sequence1::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      default:
        return 0;
    }
}

// CREATORS

Sequence1::Sequence1(bdema_Allocator *basicAllocator)
: d_allocator_p(bdema_Default::allocator(basicAllocator))
, d_element4(basicAllocator)
, d_element2(basicAllocator)
, d_element1(basicAllocator)
{
    d_element3 = new (*d_allocator_p)
            Choice2(d_allocator_p);
}

Sequence1::Sequence1(const Sequence1& original,
                     bdema_Allocator *basicAllocator)
: d_allocator_p(bdema_Default::allocator(basicAllocator))
, d_element4(original.d_element4, basicAllocator)
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
    }
    return *this;
}

void Sequence1::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_element1);
    bdeat_ValueTypeFunctions::reset(&d_element2);
    bdeat_ValueTypeFunctions::reset(d_element3);
    bdeat_ValueTypeFunctions::reset(&d_element4);
}

// ACCESSORS

std::ostream& Sequence1::print(
    std::ostream& stream,
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
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, *d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, *d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << std::flush;
}



                              // ---------------                               
                              // class Sequence2                               
                              // ---------------                               

// CONSTANTS

const char Sequence2::CLASS_NAME[] = "Sequence2";

const bdeat_AttributeInfo Sequence2::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_ELEMENT1,
        "element1",
        sizeof("element1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_ELEMENT2,
        "element2",
        sizeof("element2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_ELEMENT3,
        "element3",
        sizeof("element3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT4,
        "element4",
        sizeof("element4") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        ATTRIBUTE_ID_ELEMENT5,
        "element5",
        sizeof("element5") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *Sequence2::lookupAttributeInfo(
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
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
                    } break;
                    case '2': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
                    } break;
                    case '3': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
                    } break;
                    case '4': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
                    } break;
                    case '5': {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
                    } break;
                }
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *Sequence2::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1];
      case ATTRIBUTE_ID_ELEMENT2:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2];
      case ATTRIBUTE_ID_ELEMENT3:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3];
      case ATTRIBUTE_ID_ELEMENT4:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4];
      case ATTRIBUTE_ID_ELEMENT5:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5];
      default:
        return 0;
    }
}

// CREATORS

Sequence2::Sequence2(bdema_Allocator *basicAllocator)
: d_element5()
, d_element4(basicAllocator)
, d_element3()
, d_element1(basicAllocator)
, d_element2()
{
}

Sequence2::Sequence2(const Sequence2& original,
                     bdema_Allocator *basicAllocator)
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
    bdeat_ValueTypeFunctions::reset(&d_element1);
    bdeat_ValueTypeFunctions::reset(&d_element2);
    bdeat_ValueTypeFunctions::reset(&d_element3);
    bdeat_ValueTypeFunctions::reset(&d_element4);
    bdeat_ValueTypeFunctions::reset(&d_element5);
}

// ACCESSORS

std::ostream& Sequence2::print(
    std::ostream& stream,
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
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, (int)d_element2,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "Element1 = ";
        bdeu_PrintMethods::print(stream, d_element1,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element2 = ";
        bdeu_PrintMethods::print(stream, (int)d_element2,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element3 = ";
        bdeu_PrintMethods::print(stream, d_element3,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element4 = ";
        bdeu_PrintMethods::print(stream, d_element4,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Element5 = ";
        bdeu_PrintMethods::print(stream, d_element5,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << std::flush;
}



                          // ------------------------                          
                          // class FeatureTestMessage                          
                          // ------------------------                          

// CONSTANTS

const char FeatureTestMessage::CLASS_NAME[] = "FeatureTestMessage";

const bdeat_SelectionInfo FeatureTestMessage::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SELECTION1,
        "selection1",
        sizeof("selection1") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION2,
        "selection2",
        sizeof("selection2") - 1,
        "",
        bdeat_FormattingMode::BDEAT_HEX
    },
    {
        SELECTION_ID_SELECTION3,
        "selection3",
        sizeof("selection3") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION4,
        "selection4",
        sizeof("selection4") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION5,
        "selection5",
        sizeof("selection5") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION6,
        "selection6",
        sizeof("selection6") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        SELECTION_ID_SELECTION7,
        "selection7",
        sizeof("selection7") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION8,
        "selection8",
        sizeof("selection8") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION9,
        "selection9",
        sizeof("selection9") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_SELECTION10,
        "selection10",
        sizeof("selection10") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *FeatureTestMessage::lookupSelectionInfo(
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
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
                    } break;
                    case '2': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
                    } break;
                    case '3': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3];
                    } break;
                    case '4': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4];
                    } break;
                    case '5': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5];
                    } break;
                    case '6': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6];
                    } break;
                    case '7': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7];
                    } break;
                    case '8': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8];
                    } break;
                    case '9': {
                        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION9];
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
             && name[9]=='1'
             && name[10]=='0')
            {
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION10];
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *FeatureTestMessage::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SELECTION1:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1];
      case SELECTION_ID_SELECTION2:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2];
      case SELECTION_ID_SELECTION3:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3];
      case SELECTION_ID_SELECTION4:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4];
      case SELECTION_ID_SELECTION5:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5];
      case SELECTION_ID_SELECTION6:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6];
      case SELECTION_ID_SELECTION7:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7];
      case SELECTION_ID_SELECTION8:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8];
      case SELECTION_ID_SELECTION9:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION9];
      case SELECTION_ID_SELECTION10:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION10];
      default:
        return 0;
    }
}

// CREATORS

FeatureTestMessage::FeatureTestMessage(
    const FeatureTestMessage& original,
    bdema_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        new (d_selection1.buffer())
            Sequence1(
                original.d_selection1.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION2: {
        new (d_selection2.buffer())
            std::vector<char>(
                original.d_selection2.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION3: {
        new (d_selection3.buffer())
            Sequence2(
                original.d_selection3.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION4: {
        d_selection4 = new (*d_allocator_p)
                Sequence3(*original.d_selection4, d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION5: {
        new (d_selection5.buffer())
            bdet_DatetimeTz(original.d_selection5.object());
      } break;
      case SELECTION_ID_SELECTION6: {
        new (d_selection6.buffer())
            CustomString(
                original.d_selection6.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION7: {
        new (d_selection7.buffer())
            Enumerated::Value(original.d_selection7.object());
      } break;
      case SELECTION_ID_SELECTION8: {
        new (d_selection8.buffer())
            Choice3(
                original.d_selection8.object(), d_allocator_p);
      } break;
      case SELECTION_ID_SELECTION9: {
        new (d_selection9.buffer())
            VoidSequence(original.d_selection9.object());
      } break;
      case SELECTION_ID_SELECTION10: {
        new (d_selection10.buffer())
            UnsignedSequence(original.d_selection10.object());
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

FeatureTestMessage&
FeatureTestMessage::operator=(const FeatureTestMessage& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SELECTION1: {
            makeSelection1(rhs.d_selection1.object());
          } break;
          case SELECTION_ID_SELECTION2: {
            makeSelection2(rhs.d_selection2.object());
          } break;
          case SELECTION_ID_SELECTION3: {
            makeSelection3(rhs.d_selection3.object());
          } break;
          case SELECTION_ID_SELECTION4: {
            makeSelection4(*rhs.d_selection4);
          } break;
          case SELECTION_ID_SELECTION5: {
            makeSelection5(rhs.d_selection5.object());
          } break;
          case SELECTION_ID_SELECTION6: {
            makeSelection6(rhs.d_selection6.object());
          } break;
          case SELECTION_ID_SELECTION7: {
            makeSelection7(rhs.d_selection7.object());
          } break;
          case SELECTION_ID_SELECTION8: {
            makeSelection8(rhs.d_selection8.object());
          } break;
          case SELECTION_ID_SELECTION9: {
            makeSelection9(rhs.d_selection9.object());
          } break;
          case SELECTION_ID_SELECTION10: {
            makeSelection10(rhs.d_selection10.object());
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void FeatureTestMessage::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1: {
        d_selection1.object().~Sequence1();
      } break;
      case SELECTION_ID_SELECTION2: {
        typedef std::vector<char> Type;
        d_selection2.object().~Type();
      } break;
      case SELECTION_ID_SELECTION3: {
        d_selection3.object().~Sequence2();
      } break;
      case SELECTION_ID_SELECTION4: {
        d_allocator_p->deleteObject(d_selection4);
      } break;
      case SELECTION_ID_SELECTION5: {
        // no destruction required
      } break;
      case SELECTION_ID_SELECTION6: {
        d_selection6.object().~CustomString();
      } break;
      case SELECTION_ID_SELECTION7: {
        typedef Enumerated::Value Type;
        d_selection7.object().~Type();
      } break;
      case SELECTION_ID_SELECTION8: {
        d_selection8.object().~Choice3();
      } break;
      case SELECTION_ID_SELECTION9: {
        d_selection9.object().~VoidSequence();
      } break;
      case SELECTION_ID_SELECTION10: {
        d_selection10.object().~UnsignedSequence();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int FeatureTestMessage::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SELECTION1: {
        makeSelection1();
      } break;
      case SELECTION_ID_SELECTION2: {
        makeSelection2();
      } break;
      case SELECTION_ID_SELECTION3: {
        makeSelection3();
      } break;
      case SELECTION_ID_SELECTION4: {
        makeSelection4();
      } break;
      case SELECTION_ID_SELECTION5: {
        makeSelection5();
      } break;
      case SELECTION_ID_SELECTION6: {
        makeSelection6();
      } break;
      case SELECTION_ID_SELECTION7: {
        makeSelection7();
      } break;
      case SELECTION_ID_SELECTION8: {
        makeSelection8();
      } break;
      case SELECTION_ID_SELECTION9: {
        makeSelection9();
      } break;
      case SELECTION_ID_SELECTION10: {
        makeSelection10();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int FeatureTestMessage::makeSelection(const char *name, int nameLength)
{
    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

Sequence1& FeatureTestMessage::makeSelection1()
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection1.object());
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence1(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

Sequence1& FeatureTestMessage::makeSelection1(const Sequence1& value)
{
    if (SELECTION_ID_SELECTION1 == d_selectionId) {
        d_selection1.object() = value;
    }
    else {
        reset();
        new (d_selection1.buffer())
                Sequence1(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION1;
    }

    return d_selection1.object();
}

std::vector<char>& FeatureTestMessage::makeSelection2()
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection2.object());
    }
    else {
        reset();
        new (d_selection2.buffer())
                std::vector<char>(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

std::vector<char>& FeatureTestMessage::makeSelection2(const std::vector<char>& value)
{
    if (SELECTION_ID_SELECTION2 == d_selectionId) {
        d_selection2.object() = value;
    }
    else {
        reset();
        new (d_selection2.buffer())
                std::vector<char>(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION2;
    }

    return d_selection2.object();
}

Sequence2& FeatureTestMessage::makeSelection3()
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection3.object());
    }
    else {
        reset();
        new (d_selection3.buffer())
                Sequence2(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

Sequence2& FeatureTestMessage::makeSelection3(const Sequence2& value)
{
    if (SELECTION_ID_SELECTION3 == d_selectionId) {
        d_selection3.object() = value;
    }
    else {
        reset();
        new (d_selection3.buffer())
                Sequence2(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION3;
    }

    return d_selection3.object();
}

Sequence3& FeatureTestMessage::makeSelection4()
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(d_selection4);
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Sequence3(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

Sequence3& FeatureTestMessage::makeSelection4(const Sequence3& value)
{
    if (SELECTION_ID_SELECTION4 == d_selectionId) {
        *d_selection4 = value;
    }
    else {
        reset();
        d_selection4 = new (*d_allocator_p)
                Sequence3(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION4;
    }

    return *d_selection4;
}

bdet_DatetimeTz& FeatureTestMessage::makeSelection5()
{
    if (SELECTION_ID_SELECTION5 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection5.object());
    }
    else {
        reset();
        new (d_selection5.buffer())
            bdet_DatetimeTz();
    
        d_selectionId = SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

bdet_DatetimeTz& FeatureTestMessage::makeSelection5(const bdet_DatetimeTz& value)
{
    if (SELECTION_ID_SELECTION5 == d_selectionId) {
        d_selection5.object() = value;
    }
    else {
        reset();
        new (d_selection5.buffer())
                bdet_DatetimeTz(value);
        d_selectionId = SELECTION_ID_SELECTION5;
    }

    return d_selection5.object();
}

CustomString& FeatureTestMessage::makeSelection6()
{
    if (SELECTION_ID_SELECTION6 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection6.object());
    }
    else {
        reset();
        new (d_selection6.buffer())
                CustomString(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

CustomString& FeatureTestMessage::makeSelection6(const CustomString& value)
{
    if (SELECTION_ID_SELECTION6 == d_selectionId) {
        d_selection6.object() = value;
    }
    else {
        reset();
        new (d_selection6.buffer())
                CustomString(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION6;
    }

    return d_selection6.object();
}

Enumerated::Value& FeatureTestMessage::makeSelection7()
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection7.object());
    }
    else {
        reset();
        new (d_selection7.buffer())
                    Enumerated::Value(static_cast<Enumerated::Value>(0));
    
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return d_selection7.object();
}

Enumerated::Value& FeatureTestMessage::makeSelection7(Enumerated::Value value)
{
    if (SELECTION_ID_SELECTION7 == d_selectionId) {
        d_selection7.object() = value;
    }
    else {
        reset();
        new (d_selection7.buffer())
                Enumerated::Value(value);
        d_selectionId = SELECTION_ID_SELECTION7;
    }

    return d_selection7.object();
}

Choice3& FeatureTestMessage::makeSelection8()
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection8.object());
    }
    else {
        reset();
        new (d_selection8.buffer())
                Choice3(d_allocator_p);

        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return d_selection8.object();
}

Choice3& FeatureTestMessage::makeSelection8(const Choice3& value)
{
    if (SELECTION_ID_SELECTION8 == d_selectionId) {
        d_selection8.object() = value;
    }
    else {
        reset();
        new (d_selection8.buffer())
                Choice3(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SELECTION8;
    }

    return d_selection8.object();
}

VoidSequence& FeatureTestMessage::makeSelection9()
{
    if (SELECTION_ID_SELECTION9 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection9.object());
    }
    else {
        reset();
        new (d_selection9.buffer())
            VoidSequence();
    
        d_selectionId = SELECTION_ID_SELECTION9;
    }

    return d_selection9.object();
}

VoidSequence& FeatureTestMessage::makeSelection9(const VoidSequence& value)
{
    if (SELECTION_ID_SELECTION9 == d_selectionId) {
        d_selection9.object() = value;
    }
    else {
        reset();
        new (d_selection9.buffer())
                VoidSequence(value);
        d_selectionId = SELECTION_ID_SELECTION9;
    }

    return d_selection9.object();
}

UnsignedSequence& FeatureTestMessage::makeSelection10()
{
    if (SELECTION_ID_SELECTION10 == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_selection10.object());
    }
    else {
        reset();
        new (d_selection10.buffer())
            UnsignedSequence();
    
        d_selectionId = SELECTION_ID_SELECTION10;
    }

    return d_selection10.object();
}

UnsignedSequence& FeatureTestMessage::makeSelection10(const UnsignedSequence& value)
{
    if (SELECTION_ID_SELECTION10 == d_selectionId) {
        d_selection10.object() = value;
    }
    else {
        reset();
        new (d_selection10.buffer())
                UnsignedSequence(value);
        d_selectionId = SELECTION_ID_SELECTION10;
    }

    return d_selection10.object();
}

// ACCESSORS

std::ostream& FeatureTestMessage::print(
    std::ostream& stream,
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdeu_PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdeu_PrintMethods::print(stream, *d_selection4,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION5: {
            stream << "Selection5 = ";
            bdeu_PrintMethods::print(stream, d_selection5.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION6: {
            stream << "Selection6 = ";
            bdeu_PrintMethods::print(stream, d_selection6.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION7: {
            stream << "Selection7 = ";
            bdeu_PrintMethods::print(stream, d_selection7.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION8: {
            stream << "Selection8 = ";
            bdeu_PrintMethods::print(stream, d_selection8.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION9: {
            stream << "Selection9 = ";
            bdeu_PrintMethods::print(stream, d_selection9.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION10: {
            stream << "Selection10 = ";
            bdeu_PrintMethods::print(stream, d_selection10.object(),
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
          case SELECTION_ID_SELECTION1: {
            stream << "Selection1 = ";
            bdeu_PrintMethods::print(stream, d_selection1.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION2: {
            stream << "Selection2 = ";
            bdeu_PrintMethods::print(stream, d_selection2.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION3: {
            stream << "Selection3 = ";
            bdeu_PrintMethods::print(stream, d_selection3.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION4: {
            stream << "Selection4 = ";
            bdeu_PrintMethods::print(stream, *d_selection4,
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION5: {
            stream << "Selection5 = ";
            bdeu_PrintMethods::print(stream, d_selection5.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION6: {
            stream << "Selection6 = ";
            bdeu_PrintMethods::print(stream, d_selection6.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION7: {
            stream << "Selection7 = ";
            bdeu_PrintMethods::print(stream, d_selection7.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION8: {
            stream << "Selection8 = ";
            bdeu_PrintMethods::print(stream, d_selection8.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION9: {
            stream << "Selection9 = ";
            bdeu_PrintMethods::print(stream, d_selection9.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_SELECTION10: {
            stream << "Selection10 = ";
            bdeu_PrintMethods::print(stream, d_selection10.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << std::flush;
}


const char *FeatureTestMessage::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1].name();
      case SELECTION_ID_SELECTION2:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2].name();
      case SELECTION_ID_SELECTION3:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3].name();
      case SELECTION_ID_SELECTION4:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4].name();
      case SELECTION_ID_SELECTION5:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5].name();
      case SELECTION_ID_SELECTION6:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6].name();
      case SELECTION_ID_SELECTION7:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7].name();
      case SELECTION_ID_SELECTION8:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8].name();
      case SELECTION_ID_SELECTION9:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION9].name();
      case SELECTION_ID_SELECTION10:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION10].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                               // -------------                                
                               // class Request                                
                               // -------------                                

// CONSTANTS

const char Request::CLASS_NAME[] = "Request";

const bdeat_SelectionInfo Request::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SIMPLE_REQUEST,
        "simpleRequest",
        sizeof("simpleRequest") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    },
    {
        SELECTION_ID_FEATURE_REQUEST,
        "featureRequest",
        sizeof("featureRequest") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *Request::lookupSelectionInfo(
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
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_REQUEST];
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
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_REQUEST];
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *Request::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SIMPLE_REQUEST:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_REQUEST];
      case SELECTION_ID_FEATURE_REQUEST:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_REQUEST];
      default:
        return 0;
    }
}

// CREATORS

Request::Request(
    const Request& original,
    bdema_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST: {
        new (d_simpleRequest.buffer())
            SimpleRequest(
                original.d_simpleRequest.object(), d_allocator_p);
      } break;
      case SELECTION_ID_FEATURE_REQUEST: {
        new (d_featureRequest.buffer())
            FeatureTestMessage(
                original.d_featureRequest.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Request&
Request::operator=(const Request& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SIMPLE_REQUEST: {
            makeSimpleRequest(rhs.d_simpleRequest.object());
          } break;
          case SELECTION_ID_FEATURE_REQUEST: {
            makeFeatureRequest(rhs.d_featureRequest.object());
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Request::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST: {
        d_simpleRequest.object().~SimpleRequest();
      } break;
      case SELECTION_ID_FEATURE_REQUEST: {
        d_featureRequest.object().~FeatureTestMessage();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Request::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST: {
        makeSimpleRequest();
      } break;
      case SELECTION_ID_FEATURE_REQUEST: {
        makeFeatureRequest();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int Request::makeSelection(const char *name, int nameLength)
{
    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

SimpleRequest& Request::makeSimpleRequest()
{
    if (SELECTION_ID_SIMPLE_REQUEST == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_simpleRequest.object());
    }
    else {
        reset();
        new (d_simpleRequest.buffer())
                SimpleRequest(d_allocator_p);

        d_selectionId = SELECTION_ID_SIMPLE_REQUEST;
    }

    return d_simpleRequest.object();
}

SimpleRequest& Request::makeSimpleRequest(const SimpleRequest& value)
{
    if (SELECTION_ID_SIMPLE_REQUEST == d_selectionId) {
        d_simpleRequest.object() = value;
    }
    else {
        reset();
        new (d_simpleRequest.buffer())
                SimpleRequest(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_REQUEST;
    }

    return d_simpleRequest.object();
}

FeatureTestMessage& Request::makeFeatureRequest()
{
    if (SELECTION_ID_FEATURE_REQUEST == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_featureRequest.object());
    }
    else {
        reset();
        new (d_featureRequest.buffer())
                FeatureTestMessage(d_allocator_p);

        d_selectionId = SELECTION_ID_FEATURE_REQUEST;
    }

    return d_featureRequest.object();
}

FeatureTestMessage& Request::makeFeatureRequest(const FeatureTestMessage& value)
{
    if (SELECTION_ID_FEATURE_REQUEST == d_selectionId) {
        d_featureRequest.object() = value;
    }
    else {
        reset();
        new (d_featureRequest.buffer())
                FeatureTestMessage(value, d_allocator_p);
        d_selectionId = SELECTION_ID_FEATURE_REQUEST;
    }

    return d_featureRequest.object();
}

// ACCESSORS

std::ostream& Request::print(
    std::ostream& stream,
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
          case SELECTION_ID_SIMPLE_REQUEST: {
            stream << "SimpleRequest = ";
            bdeu_PrintMethods::print(stream, d_simpleRequest.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_FEATURE_REQUEST: {
            stream << "FeatureRequest = ";
            bdeu_PrintMethods::print(stream, d_featureRequest.object(),
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
          case SELECTION_ID_SIMPLE_REQUEST: {
            stream << "SimpleRequest = ";
            bdeu_PrintMethods::print(stream, d_simpleRequest.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_FEATURE_REQUEST: {
            stream << "FeatureRequest = ";
            bdeu_PrintMethods::print(stream, d_featureRequest.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << std::flush;
}


const char *Request::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_REQUEST].name();
      case SELECTION_ID_FEATURE_REQUEST:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_REQUEST].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}

                               // --------------                               
                               // class Response                               
                               // --------------                               

// CONSTANTS

const char Response::CLASS_NAME[] = "Response";

const bdeat_SelectionInfo Response::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_RESPONSE_DATA,
        "responseData",
        sizeof("responseData") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        SELECTION_ID_FEATURE_RESPONSE,
        "featureResponse",
        sizeof("featureResponse") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEFAULT
    }
};

// CLASS METHODS

const bdeat_SelectionInfo *Response::lookupSelectionInfo(
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
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_RESPONSE_DATA];
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
                return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_RESPONSE];
            }
        } break;
    }
    return 0;
}

const bdeat_SelectionInfo *Response::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_RESPONSE_DATA:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_RESPONSE_DATA];
      case SELECTION_ID_FEATURE_RESPONSE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_RESPONSE];
      default:
        return 0;
    }
}

// CREATORS

Response::Response(
    const Response& original,
    bdema_Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA: {
        new (d_responseData.buffer())
            std::string(
                original.d_responseData.object(), d_allocator_p);
      } break;
      case SELECTION_ID_FEATURE_RESPONSE: {
        new (d_featureResponse.buffer())
            FeatureTestMessage(
                original.d_featureResponse.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

// MANIPULATORS

Response&
Response::operator=(const Response& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_RESPONSE_DATA: {
            makeResponseData(rhs.d_responseData.object());
          } break;
          case SELECTION_ID_FEATURE_RESPONSE: {
            makeFeatureResponse(rhs.d_featureResponse.object());
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }
    return *this;
}

void Response::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA: {
        typedef std::string Type;
        d_responseData.object().~Type();
      } break;
      case SELECTION_ID_FEATURE_RESPONSE: {
        d_featureResponse.object().~FeatureTestMessage();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Response::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_RESPONSE_DATA: {
        makeResponseData();
      } break;
      case SELECTION_ID_FEATURE_RESPONSE: {
        makeFeatureResponse();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int Response::makeSelection(const char *name, int nameLength)
{
    const bdeat_SelectionInfo *selectionInfo =
           lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

std::string& Response::makeResponseData()
{
    if (SELECTION_ID_RESPONSE_DATA == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_responseData.object());
    }
    else {
        reset();
        new (d_responseData.buffer())
                std::string(d_allocator_p);

        d_selectionId = SELECTION_ID_RESPONSE_DATA;
    }

    return d_responseData.object();
}

std::string& Response::makeResponseData(const std::string& value)
{
    if (SELECTION_ID_RESPONSE_DATA == d_selectionId) {
        d_responseData.object() = value;
    }
    else {
        reset();
        new (d_responseData.buffer())
                std::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_RESPONSE_DATA;
    }

    return d_responseData.object();
}

FeatureTestMessage& Response::makeFeatureResponse()
{
    if (SELECTION_ID_FEATURE_RESPONSE == d_selectionId) {
        bdeat_ValueTypeFunctions::reset(&d_featureResponse.object());
    }
    else {
        reset();
        new (d_featureResponse.buffer())
                FeatureTestMessage(d_allocator_p);

        d_selectionId = SELECTION_ID_FEATURE_RESPONSE;
    }

    return d_featureResponse.object();
}

FeatureTestMessage& Response::makeFeatureResponse(const FeatureTestMessage& value)
{
    if (SELECTION_ID_FEATURE_RESPONSE == d_selectionId) {
        d_featureResponse.object() = value;
    }
    else {
        reset();
        new (d_featureResponse.buffer())
                FeatureTestMessage(value, d_allocator_p);
        d_selectionId = SELECTION_ID_FEATURE_RESPONSE;
    }

    return d_featureResponse.object();
}

// ACCESSORS

std::ostream& Response::print(
    std::ostream& stream,
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
          case SELECTION_ID_RESPONSE_DATA: {
            stream << "ResponseData = ";
            bdeu_PrintMethods::print(stream, d_responseData.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_FEATURE_RESPONSE: {
            stream << "FeatureResponse = ";
            bdeu_PrintMethods::print(stream, d_featureResponse.object(),
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
          case SELECTION_ID_RESPONSE_DATA: {
            stream << "ResponseData = ";
            bdeu_PrintMethods::print(stream, d_responseData.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          case SELECTION_ID_FEATURE_RESPONSE: {
            stream << "FeatureResponse = ";
            bdeu_PrintMethods::print(stream, d_featureResponse.object(),
                                     -levelPlus1, spacesPerLevel);
          } break;
          default:
            stream << "SELECTION UNDEFINED";
        }

        stream << " ]";
    }

    return stream << std::flush;
}


const char *Response::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_RESPONSE_DATA].name();
      case SELECTION_ID_FEATURE_RESPONSE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_RESPONSE].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}
}  // close namespace baea
}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.6.7 Thu Sep  8 14:56:16 2011
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
