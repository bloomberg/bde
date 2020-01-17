// balcl_optionvalue.cpp                                              -*-C++-*-
#include <balcl_optionvalue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balcl_optionvalue_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace balcl {

                             // -----------------
                             // class OptionValue
                             // -----------------

// PRIVATE MANIPULATORS
void OptionValue::init(OptionType::Enum type)
{
    BSLS_ASSERT(d_value.isUnset());

    typedef OptionType Ot;

    switch (type) {
        case Ot::e_VOID:           { // do nothing
                                   } break;
        case Ot::e_BOOL:           { d_value.createInPlace<Bool>();
                                     d_value.the<Bool>().makeValue();
                                   } break;
        case Ot::e_CHAR:           { d_value.createInPlace<Char>();
                                     d_value.the<Char>().makeValue();
                                   } break;
        case Ot::e_INT:            { d_value.createInPlace<Int>();
                                     d_value.the<Int>().makeValue();
                                   } break;
        case Ot::e_INT64:          { d_value.createInPlace<Int64>();
                                     d_value.the<Int64>().makeValue();
                                   } break;
        case Ot::e_DOUBLE:         { d_value.createInPlace<Double>();
                                     d_value.the<Double>().makeValue();
                                   } break;
        case Ot::e_STRING:         { d_value.createInPlace<String>();
                                     d_value.the<String>().makeValue();
                                   } break;
        case Ot::e_DATETIME:       { d_value.createInPlace<Datetime>();
                                     d_value.the<Datetime>().makeValue();
                                   } break;
        case Ot::e_DATE:           { d_value.createInPlace<Date>();
                                     d_value.the<Date>().makeValue();
                                   } break;
        case Ot::e_TIME:           { d_value.createInPlace<Time>();
                                     d_value.the<Time>().makeValue();
                                   } break;
        case Ot::e_CHAR_ARRAY:     { d_value.createInPlace<CharArray>();
                                     d_value.the<CharArray>().makeValue();
                                   } break;
        case Ot::e_INT_ARRAY:      { d_value.createInPlace<IntArray>();
                                     d_value.the<IntArray>().makeValue();
                                   } break;
        case Ot::e_INT64_ARRAY:    { d_value.createInPlace<Int64Array>();
                                     d_value.the<Int64Array>().makeValue();
                                   } break;
        case Ot::e_DOUBLE_ARRAY:   { d_value.createInPlace<DoubleArray>();
                                     d_value.the<DoubleArray>().makeValue();
                                   } break;
        case Ot::e_STRING_ARRAY:   { d_value.createInPlace<StringArray>();
                                     d_value.the<StringArray>().makeValue();
                                   } break;
        case Ot::e_DATETIME_ARRAY: { d_value.createInPlace<DatetimeArray>();
                                     d_value.the<DatetimeArray>().makeValue();
                                   } break;
        case Ot::e_DATE_ARRAY:     { d_value.createInPlace<DateArray>();
                                     d_value.the<DateArray>().makeValue();
                                   } break;
        case Ot::e_TIME_ARRAY:     { d_value.createInPlace<TimeArray>();
                                     d_value.the<TimeArray>().makeValue();
                                   } break;
    }
}

// ACCESSORS
OptionType::Enum OptionValue::type() const
{
    switch (d_value.typeIndex()) {
      case  0: {
        BSLS_ASSERT(d_value.isUnset());
        return OptionType::e_VOID;                                    // RETURN
      } break;
      case  1: {
        BSLS_ASSERT(d_value.is<Bool>());
        return OptionType::e_BOOL;                                    // RETURN
      } break;
      case  2: {
        BSLS_ASSERT(d_value.is<Char>());
        return OptionType::e_CHAR;                                    // RETURN
      } break;
      case  3: {
        BSLS_ASSERT(d_value.is<Int>());
        return OptionType::e_INT;                                     // RETURN
      } break;
      case  4: {
        BSLS_ASSERT(d_value.is<Int64>());
        return OptionType::e_INT64;                                   // RETURN
      } break;
      case  5: {
        BSLS_ASSERT(d_value.is<Double>());
        return OptionType::e_DOUBLE;                                  // RETURN
      } break;
      case  6: {
        BSLS_ASSERT(d_value.is<String>());
        return OptionType::e_STRING;                                  // RETURN
      } break;
      case  7: {
        BSLS_ASSERT(d_value.is<Datetime>());
        return OptionType::e_DATETIME;                                // RETURN
      } break;
      case  8: {
        BSLS_ASSERT(d_value.is<Date>());
        return OptionType::e_DATE;                                    // RETURN
      } break;
      case  9: {
        BSLS_ASSERT(d_value.is<Time>());
        return OptionType::e_TIME;                                    // RETURN
      } break;
      case 10: {
        BSLS_ASSERT(d_value.is<CharArray>());
        return OptionType::e_CHAR_ARRAY;                              // RETURN
      } break;
      case 11: {
        BSLS_ASSERT(d_value.is<IntArray>());
        return OptionType::e_INT_ARRAY;                               // RETURN
      } break;
      case 12: {
        BSLS_ASSERT(d_value.is<Int64Array>());
        return OptionType::e_INT64_ARRAY;                             // RETURN
      } break;
      case 13: {
        BSLS_ASSERT(d_value.is<DoubleArray>());
        return OptionType::e_DOUBLE_ARRAY;                            // RETURN
      } break;
      case 14: {
        BSLS_ASSERT(d_value.is<StringArray>());
        return OptionType::e_STRING_ARRAY;                            // RETURN
      } break;
      case 15: {
        BSLS_ASSERT(d_value.is<DatetimeArray>());
        return OptionType::e_DATETIME_ARRAY;                          // RETURN
      } break;
      case 16: {
        BSLS_ASSERT(d_value.is<DateArray>());
        return OptionType::e_DATE_ARRAY;                              // RETURN
      } break;
      case 17: {
        BSLS_ASSERT(d_value.is<TimeArray>());
        return OptionType::e_TIME_ARRAY;                              // RETURN
      } break;
    }

    BSLS_ASSERT_OPT(!"Reached");
    return OptionType::e_VOID;  // silence warning
}

                                  // Aspects

bsl::ostream& OptionValue::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);    // 'true' -> suppress '['
    stream << d_value;
    printer.end(true);      // 'true' -> suppress ']'

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
