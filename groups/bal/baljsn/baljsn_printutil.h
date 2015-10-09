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
//..
//  Simple Type          JSON Type  Notes
//  -----------          ---------  -----
//  char                 number
//  unsigned char        number
//  int                  number
//  unsigned int         number
//  bsls::Types::Int64   number
//  bsls::Types::Uint64  number
//  float                number
//  double               number
//  char *               string
//  bsl::string          string
//  bdlt::Date           string     ISO 8601 format
//  bdlt::DateTz         string     ISO 8601 format
//  bdlt::Time           string     ISO 8601 format
//  bdlt::TimeTz         string     ISO 8601 format
//  bdlt::Datetime       string     ISO 8601 format
//  bdlt::DatetimeTz     string     ISO 8601 format
//..
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

#ifndef INCLUDED_BDLT_ISO8601UTIL
#include <bdlt_iso8601util.h>
#endif

#ifndef INCLUDED_BDLB_FLOAT
#include <bdlb_float.h>
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

namespace BloombergLP {
namespace baljsn {

                              // ===============
                              // class PrintUtil
                              // ===============

struct PrintUtil {
    // This 'struct' provides functions for printing objects to output streams
    // in JSON format.

    // PRIVATE CLASS METHODS
    template <class TYPE>
    static int printDateAndTime(bsl::ostream& stream, const TYPE& value);
        // Encode the specified 'value' into JSON using ISO 8601 format and
        // output the result to the specified 'stream'.

    template <class TYPE>
    static int printFloatingPoint(bsl::ostream& stream, TYPE value);
        // Encode the specified floating point 'value' into JSON and output the
        // result to the specified 'stream'.

    static int printString(bsl::ostream&            stream,
                           const bslstl::StringRef& value);
        // Encode the specified string 'value' into JSON format and output the
        // result to the specified 'stream'.

  public:
    // CLASS METHODS
    static int printValue(bsl::ostream& stream, bool                    value);
    static int printValue(bsl::ostream& stream, char                    value);
    static int printValue(bsl::ostream& stream, signed char             value);
    static int printValue(bsl::ostream& stream, unsigned char           value);
    static int printValue(bsl::ostream& stream, short                   value);
    static int printValue(bsl::ostream& stream, unsigned short          value);
    static int printValue(bsl::ostream& stream, int                     value);
    static int printValue(bsl::ostream& stream, unsigned int            value);
    static int printValue(bsl::ostream& stream, bsls::Types::Int64      value);
    static int printValue(bsl::ostream& stream, bsls::Types::Uint64     value);
    static int printValue(bsl::ostream& stream, float                   value);
    static int printValue(bsl::ostream& stream, double                  value);
    static int printValue(bsl::ostream& stream, const char             *value);
    static int printValue(bsl::ostream& stream, const bsl::string&      value);
    static int printValue(bsl::ostream& stream, const bdlt::Time&       value);
    static int printValue(bsl::ostream& stream, const bdlt::Date&       value);
    static int printValue(bsl::ostream& stream, const bdlt::Datetime&   value);
    static int printValue(bsl::ostream& stream, const bdlt::TimeTz&     value);
    static int printValue(bsl::ostream& stream, const bdlt::DateTz&     value);
    static int printValue(bsl::ostream& stream, const bdlt::DatetimeTz& value);
        // Encode the specified 'value' into JSON format and output the result
        // to the specified 'stream'.  Return 0 on success and a non-zero value
        // otherwise.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // struct PrintUtil
                              // ----------------

// PRIVATE MANIPULATORS
template <class TYPE>
inline
int PrintUtil::printDateAndTime(bsl::ostream& stream, const TYPE& value)
{
    char buffer[bdlt::Iso8601Util::k_MAX_STRLEN + 1];
    bdlt::Iso8601Util::generate(buffer, sizeof buffer, value);
    return printValue(stream, buffer);
}

template <class TYPE>
int PrintUtil::printFloatingPoint(bsl::ostream& stream, TYPE value)
{
    if (bdlb::Float::isNan(value)
     || value == bsl::numeric_limits<TYPE>::infinity()
     || value == -bsl::numeric_limits<TYPE>::infinity()) {
        return -1;                                                    // RETURN
    }

    bsl::streamsize         prec  = stream.precision();
    bsl::ios_base::fmtflags flags = stream.flags();

    stream.precision(bsl::numeric_limits<TYPE>::digits10);

    stream << value;

    stream.precision(prec);
    stream.flags(flags);
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, bool value)
{
    stream << (value ? "true" : "false");
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, short value)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, int value)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, bsls::Types::Int64 value)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, unsigned char value)
{
    stream << static_cast<int>(value);
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, unsigned short value)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, unsigned int value)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, bsls::Types::Uint64 value)
{
    stream << value;
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, float value)
{
    return printFloatingPoint(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream, double value)
{
    return printFloatingPoint(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream, const char *value)
{
    return printString(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream, char value)
{
    signed char tmp(value);  // Note that 'char' is unsigned on IBM.

    stream << static_cast<int>(tmp);
    return 0;
}

inline
int PrintUtil::printValue(bsl::ostream& stream, const bsl::string& value)
{
    return printString(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream, const bdlt::Time& value)
{
    return printDateAndTime(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream, const bdlt::Date& value)
{
    return printDateAndTime(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream, const bdlt::Datetime& value)
{
    return printDateAndTime(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream, const bdlt::TimeTz& value)
{
    return printDateAndTime(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream, const bdlt::DateTz& value)
{
    return printDateAndTime(stream, value);
}

inline
int PrintUtil::printValue(bsl::ostream& stream, const bdlt::DatetimeTz& value)
{
    return printDateAndTime(stream, value);
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
