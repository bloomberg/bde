// baljsn_printutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BALJSN_PRINTUTIL
#define INCLUDED_BALJSN_PRINTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for encoding simple types in the JSON format.
//
//@CLASSES:
//  baljsn::PrintUtil: utility for printing simple types in JSON
//
//@SEE_ALSO: baljsn_encoder, baljsn_parserutil
//
//@DESCRIPTION: This component provides a 'struct' of utility functions,
// 'baljsn::PrintUtil', for encoding a 'bdeat' Simple type in the JSON format.
// The primary method is 'printValue', which encodes a specified object and is
// overloaded for all 'bdeat' Simple types.  The following table describes the
// format in which various Simple types are encoded.
//
// Refer to the details of the JSON encoding format supported by this utility
// in the package documentation file (doc/baljsn.txt).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding a Simple 'struct' into JSON
///-----------------------------------------------
// Suppose we want to serialize some data into JSON.
//
// First, we define a struct, 'Employee', to contain the data:
//..
//  struct Employee {
//      const char *d_firstName;
//      const char *d_lastName;
//      int         d_age;
//  };
//..
// Then, we create an 'Employee' object and populate it with data:
//..
//  Employee john;
//  john.d_firstName = "John";
//  john.d_lastName = "Doe";
//  john.d_age = 20;
//..
//  Now, we create an output stream and manually construct the JSON string
//  using 'baljsn::PrintUtil':
//..
//  bsl::ostringstream oss;
//  oss << '{' << '\n';
//  baljsn::PrintUtil::printValue(oss, "firstName");
//  oss << ':';
//  baljsn::PrintUtil::printValue(oss, john.d_firstName);
//  oss << ',' << '\n';
//  baljsn::PrintUtil::printValue(oss, "lastName");
//  oss << ':';
//  baljsn::PrintUtil::printValue(oss, john.d_lastName);
//  oss << ',' << '\n';
//  baljsn::PrintUtil::printValue(oss, "age");
//  oss << ':';
//  baljsn::PrintUtil::printValue(oss, john.d_age);
//  oss << '\n' << '}';
//..
//  Finally, we print out the JSON string:
//..
//  if (verbose) {
//      bsl::cout << oss.str();
//  }
//..
//  The output should look like:
//..
//  {
//  "firstName":"John",
//  "lastName":"Doe",
//  "age":20
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALJSN_ENCODEROPTIONS
#include <baljsn_encoderoptions.h>
#endif

#ifndef INCLUDED_BDLT_ISO8601UTIL
#include <bdlt_iso8601util.h>
#endif

#ifndef INCLUDED_BDLB_FLOAT
#include <bdlb_float.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMAL
#include <bdldfp_decimal.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL
#include <bdldfp_decimalconvertutil.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOMANIP
#include <bsl_iomanip.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_C_STDIO
#include <bsl_c_stdio.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALUTIL
#include <bdldfp_decimalutil.h>
#endif

namespace BloombergLP {
namespace baljsn {

                              // ===============
                              // class PrintUtil
                              // ===============

struct PrintUtil {
    // This 'struct' provides functions for printing objects to output streams
    // in JSON format.

  private:
    // PRIVATE CLASS METHODS
    template <class TYPE>
    static int maxStreamPrecision(const baljsn::EncoderOptions *options);
        // Return the maximum precision for streaming values of the specified
        // template parameter 'TYPE' using the specified 'options' to decide.
        // The behavior is undefined unless 'TYPE' is 'float' or 'double'.

  public:
    // CLASS METHODS
    template <class TYPE>
    static int printDateAndTime(bsl::ostream&         stream,
                                const TYPE&           value,
                                const EncoderOptions *options);
        // Encode the specified 'value' into JSON using ISO 8601 format and
        // output the result to the specified 'stream' using the specified
        // 'options'.

    template <class TYPE>
    static int printFloatingPoint(bsl::ostream&         stream,
                                  TYPE                  value,
                                  const EncoderOptions *options);
        // Encode the specified floating point 'value' into JSON and output the
        // result to the specified 'stream'.  Use the optionally-specified
        // 'options' to decide how 'value' is encoded.

    static int printString(bsl::ostream&             stream,
                           const bslstl::StringRef&  value);
        // Encode the specified string 'value' into JSON format and output the
        // result to the specified 'stream'.

  public:
    // CLASS METHODS
    static int printValue(bsl::ostream&         stream,
                          bool                  value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          char                  value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          signed char           value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          unsigned char         value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          short                 value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          unsigned short        value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          int                   value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          unsigned int          value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          bsls::Types::Int64    value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          bsls::Types::Uint64   value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          float                 value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          double                value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          bdldfp::Decimal64     value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          const char           *value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          const bsl::string&    value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          const bdlt::Time&     value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          const bdlt::Date&     value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&          stream,
                          const bdlt::Datetime&  value,
                          const EncoderOptions  *options = 0);
    static int printValue(bsl::ostream&         stream,
                          const bdlt::TimeTz&   value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&         stream,
                          const bdlt::DateTz&   value,
                          const EncoderOptions *options = 0);
    static int printValue(bsl::ostream&            stream,
                          const bdlt::DatetimeTz&  value,
                          const EncoderOptions    *options = 0);
        // Encode the specified 'value' into JSON format and output the result
        // to the specified 'stream' using the specified 'options'.  Return 0
        // on success and a non-zero value otherwise.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // struct PrintUtil
                              // ----------------

// PRIVATE CLASS METHODS
template <>
inline
int PrintUtil::maxStreamPrecision<float>(const baljsn::EncoderOptions *options)
{
    return options
           ? options->maxFloatPrecision()
           : bsl::numeric_limits<float>::digits10;
}

template <>
inline
int PrintUtil::maxStreamPrecision<double>(
                                         const baljsn::EncoderOptions *options)
{

    return options
           ? options->maxDoublePrecision()
           : bsl::numeric_limits<double>::digits10;
}

// CLASS METHODS
template <class TYPE>
inline
int PrintUtil::printDateAndTime(bsl::ostream&         stream,
                                const TYPE&           value,
                                const EncoderOptions *options)
{
    char buffer[bdlt::Iso8601Util::k_MAX_STRLEN + 1];
    bdlt::Iso8601UtilConfiguration config;

    if (options) {
        config.setFractionalSecondPrecision(
                                 options->datetimeFractionalSecondPrecision());
    }
    else {
        config.setFractionalSecondPrecision(3);
    }

    bdlt::Iso8601Util::generate(buffer, sizeof buffer, value, config);
    return printValue(stream, buffer);
}

template <class TYPE>
int PrintUtil::printFloatingPoint(bsl::ostream&                 stream,
                                  TYPE                          value,
                                  const baljsn::EncoderOptions *options)
{
    switch (bdlb::Float::classifyFine(value)) {
      case bdlb::Float::k_POSITIVE_INFINITY: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            stream << "\"+inf\"";
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      case bdlb::Float::k_NEGATIVE_INFINITY: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            stream << "\"-inf\"";
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      case bdlb::Float::k_QNAN:                                 // FALL-THROUGH
      case bdlb::Float::k_SNAN: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            stream << (bdlb::Float::signBit(value) ? "\"-nan\"" : "\"nan\"");
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      default: {
        const int SIZE = 32;
        char      buffer[SIZE];

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

        const int len = snprintf(buffer,
                                 SIZE,
                                 "%-1.*g",
                                 maxStreamPrecision<TYPE>(options),
                                 value);

#if defined(BSLS_PLATFORM_CMP_MSVC)
#undef snprintf
#endif
        stream.write(buffer, len);
      }
    }
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream,
                          bool          value,
                          const EncoderOptions *)
{
    stream << (value ? "true" : "false");
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream,
                          short         value,
                          const EncoderOptions *)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream,
                          int           value,
                          const EncoderOptions *)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream&      stream,
                          bsls::Types::Int64 value,
                          const EncoderOptions *)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream,
                          unsigned char value,
                          const EncoderOptions *)
{
    stream << static_cast<int>(value);
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream&  stream,
                          unsigned short value,
                          const EncoderOptions *)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream,
                          unsigned int  value,
                          const EncoderOptions *)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream&       stream,
                          bsls::Types::Uint64 value,
                          const EncoderOptions *)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream&         stream,
                          float                 value,
                          const EncoderOptions *options)
{
    return printFloatingPoint(stream, value, options);
}

inline
int PrintUtil::printValue(bsl::ostream&         stream,
                          double                value,
                          const EncoderOptions *options)
{
    return printFloatingPoint(stream, value, options);
}

inline
int PrintUtil::printValue(bsl::ostream&         stream,
                          bdldfp::Decimal64     value,
                          const EncoderOptions *options)
{
    switch (bdldfp::DecimalUtil::classify(value)) {
      case FP_INFINITE: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            stream <<
                  (value == bsl::numeric_limits<bdldfp::Decimal64>::infinity()
                  ? "\"+inf\""
                  : "\"-inf\"");
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      case FP_NAN: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            stream << (bdlb::Float::signBit(
                            bdldfp::DecimalConvertUtil::decimalToDouble(value))
                      ? "\"-nan\""
                      : "\"nan\"");
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      default: {
        stream << value;
        if (stream.bad()) {
            return -1;                                                // RETURN
        }
      }
    }
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream&  stream,
                          const char    *value,
                          const EncoderOptions *)
{
    return printString(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream,
                          char          value,
                          const EncoderOptions *)
{
    signed char tmp(value);  // Note that 'char' is unsigned on IBM.

    stream << static_cast<int>(tmp);
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream&      stream,
                          const bsl::string& value,
                          const EncoderOptions *)
{
    return printString(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream&         stream,
                          const bdlt::Time&     value,
                          const EncoderOptions *options)
{
    return printDateAndTime(stream, value, options);
}

inline
int PrintUtil::printValue(bsl::ostream&         stream,
                          const bdlt::Date&     value,
                          const EncoderOptions *options)
{
    return printDateAndTime(stream, value, options);
}

inline
int PrintUtil::printValue(bsl::ostream&          stream,
                          const bdlt::Datetime&  value,
                          const EncoderOptions  *options)
{
    return printDateAndTime(stream, value, options);
}

inline
int PrintUtil::printValue(bsl::ostream&         stream,
                          const bdlt::TimeTz&   value,
                          const EncoderOptions *options)
{
    return printDateAndTime(stream, value, options);
}

inline
int PrintUtil::printValue(bsl::ostream&         stream,
                          const bdlt::DateTz&   value,
                          const EncoderOptions *options)
{
    return printDateAndTime(stream, value, options);
}

inline
int PrintUtil::printValue(bsl::ostream&            stream,
                          const bdlt::DatetimeTz&  value,
                          const EncoderOptions    *options)
{
    return printDateAndTime(stream, value, options);
}
}  // close package namespace

}  // close enterprise namespace

#endif

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
