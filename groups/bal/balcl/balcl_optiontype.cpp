// balcl_optiontype.cpp                                               -*-C++-*-
#include <balcl_optiontype.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balcl_optiontype_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace balcl {

                     // -----------------
                     // struct OptionType
                     // -----------------

typedef OptionType Ot;

// BDE_VERIFY pragma: -MN04: // Pointer member names must end in '_p'
                             // The rule not intended for public data.

// PUBLIC CLASS DATA
Ot::Bool          * const Ot::k_BOOL           = 0;
Ot::Char          * const Ot::k_CHAR           = 0;
Ot::Int           * const Ot::k_INT            = 0;
Ot::Int64         * const Ot::k_INT64          = 0;
Ot::Double        * const Ot::k_DOUBLE         = 0;
Ot::String        * const Ot::k_STRING         = 0;
Ot::Datetime      * const Ot::k_DATETIME       = 0;
Ot::Date          * const Ot::k_DATE           = 0;
Ot::Time          * const Ot::k_TIME           = 0;
Ot::CharArray     * const Ot::k_CHAR_ARRAY     = 0;
Ot::IntArray      * const Ot::k_INT_ARRAY      = 0;
Ot::Int64Array    * const Ot::k_INT64_ARRAY    = 0;
Ot::DoubleArray   * const Ot::k_DOUBLE_ARRAY   = 0;
Ot::StringArray   * const Ot::k_STRING_ARRAY   = 0;
Ot::DatetimeArray * const Ot::k_DATETIME_ARRAY = 0;
Ot::DateArray     * const Ot::k_DATE_ARRAY     = 0;
Ot::TimeArray     * const Ot::k_TIME_ARRAY     = 0;
// BDE_VERIFY pragma: +MN04: // Pointer member names must end in '_p'

                        // -----------------------------
                        // struct OptionType::TypeToEnum
                        // -----------------------------

// PUBLIC CLASS DATA
const Ot::Enum Ot::TypeToEnum<void>             ::value = Ot::e_VOID;
const Ot::Enum Ot::TypeToEnum<Ot::Bool>         ::value = Ot::e_BOOL;
const Ot::Enum Ot::TypeToEnum<Ot::Char>         ::value = Ot::e_CHAR;
const Ot::Enum Ot::TypeToEnum<Ot::Int>          ::value = Ot::e_INT;
const Ot::Enum Ot::TypeToEnum<Ot::Int64>        ::value = Ot::e_INT64;
const Ot::Enum Ot::TypeToEnum<Ot::Double>       ::value = Ot::e_DOUBLE;
const Ot::Enum Ot::TypeToEnum<Ot::String>       ::value = Ot::e_STRING;
const Ot::Enum Ot::TypeToEnum<Ot::Datetime>     ::value = Ot::e_DATETIME;
const Ot::Enum Ot::TypeToEnum<Ot::Date>         ::value = Ot::e_DATE;
const Ot::Enum Ot::TypeToEnum<Ot::Time>         ::value = Ot::e_TIME;
const Ot::Enum Ot::TypeToEnum<Ot::CharArray>    ::value = Ot::e_CHAR_ARRAY;
const Ot::Enum Ot::TypeToEnum<Ot::IntArray>     ::value = Ot::e_INT_ARRAY;
const Ot::Enum Ot::TypeToEnum<Ot::Int64Array>   ::value = Ot::e_INT64_ARRAY;
const Ot::Enum Ot::TypeToEnum<Ot::DoubleArray>  ::value = Ot::e_DOUBLE_ARRAY;
const Ot::Enum Ot::TypeToEnum<Ot::StringArray>  ::value = Ot::e_STRING_ARRAY;
const Ot::Enum Ot::TypeToEnum<Ot::DatetimeArray>::value = Ot::e_DATETIME_ARRAY;
const Ot::Enum Ot::TypeToEnum<Ot::DateArray>    ::value = Ot::e_DATE_ARRAY;
const Ot::Enum Ot::TypeToEnum<Ot::TimeArray>    ::value = Ot::e_TIME_ARRAY;

// CLASS METHODS
bsl::ostream& OptionType::print(bsl::ostream&    stream,
                                OptionType::Enum value,
                                int              level,
                                int              spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream << toAscii(value);
    printer.end(true);

    return stream;
}

const char *OptionType::toAscii(OptionType::Enum value)
{
    switch (value) {
      case e_VOID          : return "VOID";                           // RETURN
      case e_BOOL          : return "BOOL";                           // RETURN
      case e_CHAR          : return "CHAR";                           // RETURN
      case e_INT           : return "INT";                            // RETURN
      case e_INT64         : return "INT64";                          // RETURN
      case e_DOUBLE        : return "DOUBLE";                         // RETURN
      case e_STRING        : return "STRING";                         // RETURN
      case e_DATETIME      : return "DATETIME";                       // RETURN
      case e_DATE          : return "DATE";                           // RETURN
      case e_TIME          : return "TIME";                           // RETURN
      case e_CHAR_ARRAY    : return "CHAR_ARRAY";                     // RETURN
      case e_INT_ARRAY     : return "INT_ARRAY";                      // RETURN
      case e_INT64_ARRAY   : return "INT64_ARRAY";                    // RETURN
      case e_DOUBLE_ARRAY  : return "DOUBLE_ARRAY";                   // RETURN
      case e_STRING_ARRAY  : return "STRING_ARRAY";                   // RETURN
      case e_DATETIME_ARRAY: return "DATETIME_ARRAY";                 // RETURN
      case e_DATE_ARRAY    : return "DATE_ARRAY";                     // RETURN
      case e_TIME_ARRAY    : return "TIME_ARRAY";                     // RETURN
      default              : return "(* UNKNOWN *)";                  // RETURN
    }
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
