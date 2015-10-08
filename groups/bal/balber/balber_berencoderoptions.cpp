// balber_berencoderoptions.cpp - GENERATED FILE -                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_berencoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berencoderoptions_cpp,"$Id$ $CSID$")

#include <bdlat_attributeinfo.h>
#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bsls_assert.h>

#include <iomanip>
#include <limits>
#include <ostream>

namespace BloombergLP {

                      // -------------------------------
                      // class balber::BerEncoderOptions
                      // -------------------------------

// CONSTANTS
const char balber::BerEncoderOptions::CLASS_NAME[] =
                                                   "balber::BerEncoderOptions";
const int  balber::BerEncoderOptions::
              DEFAULT_INITIALIZER_TRACE_LEVEL                          = 0;
const int  balber::BerEncoderOptions::
              DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE              = 10500;
const bool balber::BerEncoderOptions::
              DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS                  = true;
const bool balber::BerEncoderOptions::
              DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY = false;
const bdlat_AttributeInfo balber::BerEncoderOptions::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_TRACE_LEVEL,
        "TraceLevel",
        sizeof("TraceLevel") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE,
        "BdeVersionConformance",
        sizeof("BdeVersionConformance") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        e_ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS,
        "EncodeEmptyArrays",
        sizeof("EncodeEmptyArrays") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        e_ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY,
        "EncodeDateAndTimeTypesAsBinary",
        sizeof("EncodeDateAndTimeTypesAsBinary") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    }
};

namespace balber {

// CLASS METHODS
const bdlat_AttributeInfo *BerEncoderOptions::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
        case 10: {
            if (name[0]=='T'
             && name[1]=='r'
             && name[2]=='a'
             && name[3]=='c'
             && name[4]=='e'
             && name[5]=='L'
             && name[6]=='e'
             && name[7]=='v'
             && name[8]=='e'
             && name[9]=='l')
            {
                return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL];
                                                                      // RETURN
            }
        } break;
        case 17: {
            if (name[0]=='E'
             && name[1]=='n'
             && name[2]=='c'
             && name[3]=='o'
             && name[4]=='d'
             && name[5]=='e'
             && name[6]=='E'
             && name[7]=='m'
             && name[8]=='p'
             && name[9]=='t'
             && name[10]=='y'
             && name[11]=='A'
             && name[12]=='r'
             && name[13]=='r'
             && name[14]=='a'
             && name[15]=='y'
             && name[16]=='s')
            {
                return &ATTRIBUTE_INFO_ARRAY[
                                        e_ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS];
                                                                      // RETURN
            }
        } break;
        case 21: {
            if (name[0]=='B'
             && name[1]=='d'
             && name[2]=='e'
             && name[3]=='V'
             && name[4]=='e'
             && name[5]=='r'
             && name[6]=='s'
             && name[7]=='i'
             && name[8]=='o'
             && name[9]=='n'
             && name[10]=='C'
             && name[11]=='o'
             && name[12]=='n'
             && name[13]=='f'
             && name[14]=='o'
             && name[15]=='r'
             && name[16]=='m'
             && name[17]=='a'
             && name[18]=='n'
             && name[19]=='c'
             && name[20]=='e')
            {
                return &ATTRIBUTE_INFO_ARRAY[
                                    e_ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE];
                                                                      // RETURN
            }
        } break;
        case 30: {
            if (name[0]=='E'
             && name[1]=='n'
             && name[2]=='c'
             && name[3]=='o'
             && name[4]=='d'
             && name[5]=='e'
             && name[6]=='D'
             && name[7]=='a'
             && name[8]=='t'
             && name[9]=='e'
             && name[10]=='A'
             && name[11]=='n'
             && name[12]=='d'
             && name[13]=='T'
             && name[14]=='i'
             && name[15]=='m'
             && name[16]=='e'
             && name[17]=='T'
             && name[18]=='y'
             && name[19]=='p'
             && name[20]=='e'
             && name[21]=='s'
             && name[22]=='A'
             && name[23]=='s'
             && name[24]=='B'
             && name[25]=='i'
             && name[26]=='n'
             && name[27]=='a'
             && name[28]=='r'
             && name[29]=='y')
            {
                return &ATTRIBUTE_INFO_ARRAY[
                       e_ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY];
                                                                      // RETURN
            }
        } break;
    }
    return 0;
}

const bdlat_AttributeInfo *BerEncoderOptions::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_TRACE_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TRACE_LEVEL];
      case e_ATTRIBUTE_ID_BDE_VERSION_CONFORMANCE:
        return &ATTRIBUTE_INFO_ARRAY[
                                    e_ATTRIBUTE_INDEX_BDE_VERSION_CONFORMANCE];
      case e_ATTRIBUTE_ID_ENCODE_EMPTY_ARRAYS:
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_ENCODE_EMPTY_ARRAYS];
      case e_ATTRIBUTE_ID_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY:
        return &ATTRIBUTE_INFO_ARRAY[
                       e_ATTRIBUTE_INDEX_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY];
      default:
        return 0;
    }
}

// CREATORS

BerEncoderOptions::BerEncoderOptions()
: d_traceLevel(DEFAULT_INITIALIZER_TRACE_LEVEL)
, d_bdeVersionConformance(DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE)
, d_encodeEmptyArrays(DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS)
, d_encodeDateAndTimeTypesAsBinary(
                      DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY)
{
}

BerEncoderOptions::BerEncoderOptions(const BerEncoderOptions& original)
: d_traceLevel(original.d_traceLevel)
, d_bdeVersionConformance(original.d_bdeVersionConformance)
, d_encodeEmptyArrays(original.d_encodeEmptyArrays)
, d_encodeDateAndTimeTypesAsBinary(original.d_encodeDateAndTimeTypesAsBinary)
{
}

BerEncoderOptions::~BerEncoderOptions()
{
}

// MANIPULATORS

BerEncoderOptions&
BerEncoderOptions::operator=(const BerEncoderOptions& rhs)
{
    if (this != &rhs) {
        d_traceLevel                     = rhs.d_traceLevel;
        d_bdeVersionConformance          = rhs.d_bdeVersionConformance;
        d_encodeEmptyArrays              = rhs.d_encodeEmptyArrays;
        d_encodeDateAndTimeTypesAsBinary =
                                          rhs.d_encodeDateAndTimeTypesAsBinary;
    }
    return *this;
}

void BerEncoderOptions::reset()
{
    d_traceLevel            = DEFAULT_INITIALIZER_TRACE_LEVEL;
    d_bdeVersionConformance = DEFAULT_INITIALIZER_BDE_VERSION_CONFORMANCE;
    d_encodeEmptyArrays     = DEFAULT_INITIALIZER_ENCODE_EMPTY_ARRAYS;
    d_encodeDateAndTimeTypesAsBinary =
                      DEFAULT_INITIALIZER_ENCODE_DATE_AND_TIME_TYPES_AS_BINARY;
}

// ACCESSORS

bsl::ostream& BerEncoderOptions::print(
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
        stream << "TraceLevel = ";
        bdlb::PrintMethods::print(stream,
                                  d_traceLevel,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "BdeVersionConformance = ";
        bdlb::PrintMethods::print(stream,
                                  d_bdeVersionConformance,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "EncodeEmptyArrays = ";
        bdlb::PrintMethods::print(stream,
                                  d_encodeEmptyArrays,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "EncodeDateAndTimeTypesAsBinary = ";
        bdlb::PrintMethods::print(stream,
                                  d_encodeDateAndTimeTypesAsBinary,
                                  -levelPlus1,
                                  spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "TraceLevel = ";
        bdlb::PrintMethods::print(stream, d_traceLevel,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << ' ';
        stream << "BdeVersionConformance = ";
        bdlb::PrintMethods::print(stream, d_bdeVersionConformance,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << ' ';
        stream << "EncodeEmptyArrays = ";
        bdlb::PrintMethods::print(stream, d_encodeEmptyArrays,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << ' ';
        stream << "EncodeDateAndTimeTypesAsBinary = ";
        bdlb::PrintMethods::print(stream, d_encodeDateAndTimeTypesAsBinary,
                                  -levelPlus1,
                                  spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
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
