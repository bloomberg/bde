// bdlar_simpletypeid.h                                               -*-C++-*-
#ifndef INCLUDED_BDLAR_SIMPLETYPEID
#define INCLUDED_BDLAR_SIMPLETYPEID

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::SimpleTypeId: ...
//  bdlar::SimpleTypeById: ...
//  bdlar::SimpleTypeIdOf: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdldfp_decimal.h>

#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>

#include <bsl_string.h>
#include <bsl_type_traits.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace bdlar {

                           // =================
                           // enum SimpleTypeId
                           // =================

enum SimpleTypeId {
    e_INT_ID,
    e_UINT_ID,
    e_CHAR_ID,
    e_SCHAR_ID,
    e_UCHAR_ID,
    e_SHORT_ID,
    e_USHORT_ID,
    e_LONG_ID,
    e_ULONG_ID,
    e_LONGLONG_ID,
    e_ULONGLONG_ID,
    e_BOOL_ID,
    e_FLOAT_ID,
    e_DOUBLE_ID,
    e_DECIMAL64_ID,
    e_STRING_ID,
    e_VECTOR_CHAR_ID,
    e_DATE_ID,
    e_DATETZ_ID,
    e_DATETIME_ID,
    e_DATETIMETZ_ID,
    e_TIME_ID,
    e_TIMETZ_ID
};

                           // =====================
                           // struct SimpleTypeById
                           // =====================

template <SimpleTypeId t_ID>
struct SimpleTypeById {
    // must be specialized
    //typedef <TYPE> type;
};

                           // =====================
                           // struct SimpleTypeIdOf
                           // =====================

template <class t_TYPE>
struct SimpleTypeIdOf {
    // must be specialized
    //static constexpr SimpleTypeId value = <VALUE>;
};

#define DECLARE_SIMPLE_TYPE(id, Type)                                         \
    template <> struct SimpleTypeById<id> { typedef Type type; };             \
    template <> struct SimpleTypeIdOf<Type>                                   \
        : bsl::integral_constant<SimpleTypeId, id> {};

DECLARE_SIMPLE_TYPE(e_INT_ID,         int)
DECLARE_SIMPLE_TYPE(e_UINT_ID,        unsigned int)
DECLARE_SIMPLE_TYPE(e_CHAR_ID,        char)
DECLARE_SIMPLE_TYPE(e_SCHAR_ID,       signed char)
DECLARE_SIMPLE_TYPE(e_UCHAR_ID,       unsigned char)
DECLARE_SIMPLE_TYPE(e_SHORT_ID,       short)
DECLARE_SIMPLE_TYPE(e_USHORT_ID,      unsigned short)
DECLARE_SIMPLE_TYPE(e_LONG_ID,        long)
DECLARE_SIMPLE_TYPE(e_ULONG_ID,       unsigned long)
DECLARE_SIMPLE_TYPE(e_LONGLONG_ID,    long long)
DECLARE_SIMPLE_TYPE(e_ULONGLONG_ID,   unsigned long long)
DECLARE_SIMPLE_TYPE(e_BOOL_ID,        bool)
DECLARE_SIMPLE_TYPE(e_FLOAT_ID,       float)
DECLARE_SIMPLE_TYPE(e_DOUBLE_ID,      double)
DECLARE_SIMPLE_TYPE(e_DECIMAL64_ID,   bdldfp::Decimal64)
DECLARE_SIMPLE_TYPE(e_STRING_ID,      bsl::string)
DECLARE_SIMPLE_TYPE(e_VECTOR_CHAR_ID, bsl::vector<char>)
DECLARE_SIMPLE_TYPE(e_DATE_ID,        bdlt::Date)
DECLARE_SIMPLE_TYPE(e_DATETZ_ID,      bdlt::DateTz)
DECLARE_SIMPLE_TYPE(e_DATETIME_ID,    bdlt::Datetime)
DECLARE_SIMPLE_TYPE(e_DATETIMETZ_ID,  bdlt::DatetimeTz)
DECLARE_SIMPLE_TYPE(e_TIME_ID,        bdlt::Time)
DECLARE_SIMPLE_TYPE(e_TIMETZ_ID,      bdlt::TimeTz)

#undef DECLARE_SIMPLE_TYPE

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
