// balcl_optionvalue.cpp                                              -*-C++-*-
#include <balcl_optionvalue.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balcl_optionvalue_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

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

    switch (type) {
      case OptionType::e_VOID: {
        // do nothing
      } break;
#define u_CASE_CREATE_IN_PLACE(theenum, thetype) \
      case OptionType::theenum: d_value.createInPlace<thetype>()

      u_CASE_CREATE_IN_PLACE( e_BOOL,           Bool          );  break;
      u_CASE_CREATE_IN_PLACE( e_CHAR,           Char          );  break;
      u_CASE_CREATE_IN_PLACE( e_INT,            Int           );  break;
      u_CASE_CREATE_IN_PLACE( e_INT64,          Int64         );  break;
      u_CASE_CREATE_IN_PLACE( e_DOUBLE,         Double        );  break;
      u_CASE_CREATE_IN_PLACE( e_STRING,         String        );  break;
      u_CASE_CREATE_IN_PLACE( e_DATETIME,       Datetime      );  break;
      u_CASE_CREATE_IN_PLACE( e_DATE,           Date          );  break;
      u_CASE_CREATE_IN_PLACE( e_TIME,           Time          );  break;

      u_CASE_CREATE_IN_PLACE( e_CHAR_ARRAY,     CharArray     );  break;
      u_CASE_CREATE_IN_PLACE( e_INT_ARRAY,      IntArray      );  break;
      u_CASE_CREATE_IN_PLACE( e_INT64_ARRAY,    Int64Array    );  break;
      u_CASE_CREATE_IN_PLACE( e_DOUBLE_ARRAY,   DoubleArray   );  break;
      u_CASE_CREATE_IN_PLACE( e_STRING_ARRAY,   StringArray   );  break;
      u_CASE_CREATE_IN_PLACE( e_DATETIME_ARRAY, DatetimeArray );  break;
      u_CASE_CREATE_IN_PLACE( e_DATE_ARRAY,     DateArray     );  break;
      u_CASE_CREATE_IN_PLACE( e_TIME_ARRAY,     TimeArray     );  break;
#undef u_CASE_CREATE_IN_PLACE

      default: BSLS_ASSERT_INVOKE_NORETURN(
                            "balcl::OptionValue::init': Unknown option type.");
    }
}

// ACCESSORS
OptionType::Enum OptionValue::type() const
{
    if (d_value.is<OptionValue_NullOf>()) {
        return d_value.the<OptionValue_NullOf>().type();              // RETURN
    }

    // The code that returns the type of the selection depends on the type
    // index of the 'Variant' being the same as the 'OptionType' enumerator
    // value assigned to that same type.  In order to ensure that code will not
    // silently break by changing the order (or number) of the 'Variant' types
    // in the header we employ compile time assertions at the end of this
    // function.  We also verify that we have successfully verified all
    // 'OptionType' enumerators/indexes by checking that the NULL value
    // (handled above) immediately follows the last 'OptionType' index value.

    return static_cast<OptionType::Enum>(d_value.typeIndex());

#define BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ(VALUE, TYPENAME, ENUMERATOR)      \
    BSLMF_ASSERT((bsl::is_same<ValueVariant::Type##VALUE, TYPENAME>::value)   \
              && VALUE == OptionType::ENUMERATOR)

    // Getting a compilation error on any of the
    // 'BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ' compile time assertions means that
    // the number of the order of the 'Types' have been changed in
    // 'ValueVariant', but this verification code was not updated.  Note that
    // the type-index and the 'OptionType' enumerator values *must* match.  A
    // compilation error *only* on the 'OptionValue_NullOf' compile-time
    // assertion means that a new type was added to 'ValueVariant', but the
    // verification code above does not contain it.

    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ( 1, Bool,     e_BOOL    );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ( 2, Char,     e_CHAR    );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ( 3, Int,      e_INT     );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ( 4, Int64,    e_INT64   );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ( 5, Double,   e_DOUBLE  );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ( 6, String,   e_STRING  );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ( 7, Datetime, e_DATETIME);
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ( 8, Date,     e_DATE    );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ( 9, Time,     e_TIME    );

    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ(10, CharArray,     e_CHAR_ARRAY    );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ(11, IntArray,      e_INT_ARRAY     );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ(12, Int64Array,    e_INT64_ARRAY   );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ(13, DoubleArray,   e_DOUBLE_ARRAY  );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ(14, StringArray,   e_STRING_ARRAY  );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ(15, DatetimeArray, e_DATETIME_ARRAY);
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ(16, DateArray,     e_DATE_ARRAY    );
    BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ(17, TimeArray,     e_TIME_ARRAY    );
#undef BALCL_OPTIONVALUE_ASSERT_TYPEIDX_EQ

    BSLMF_ASSERT((bsl::is_same<ValueVariant::Type18,
                 OptionValue_NullOf>::value));
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
