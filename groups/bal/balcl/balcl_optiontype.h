// balcl_optiontype.h                                                 -*-C++-*-
#ifndef INCLUDED_BALCL_OPTIONTYPE
#define INCLUDED_BALCL_OPTIONTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Enumerate the types supported for command-line-option values.
//
//@CLASSES:
//  balcl::OptionType: command-line option types 'enum' and utilities
//
//@SEE ALSO: balcl_optionvalue, balcl_commandline
//
//@DESCRIPTION: This component provides a 'struct', 'balcl::OptionType',
// that is a namespace for:
//: o A 'typedef' for each of the types allowed for command-line-option values.
//: o An enumerator that represents each of those types.
//: o A statically-initialized null pointer for each of those types.
//: o Utility functions.
//
// The enumerator values are used to control type-dependent operations where
// the type is not known until runtime.
//
// The names of related symbols are similar to each other.  For example, the
// supported option-value type 'bsl::vector<bdtl::Datetime>' has:
//..
//  Symbol                               Description
//  ------------------------------------ --------------------------------
//  balcl::OptionType::DatetimeArray     alias ('typedef')
//  balcl::OptionType::e_DATETIME_ARRAY  enumerator
//  balcl::OptionType::k_DATETIME_ARRAY  statically-initialized null pointer
//..
// The null pointer symbols are typically used with the constructor of
// 'balcl::TypeInfo'.  They provide a clearly named way to select the
// constructor to create an object having the desired type attribute.  See
// {'balcl_typeinfo'} and {'balcl_commandline'}.
//
// In addition to the conventional enumeration methods, this component also
// provides some utility functions that categorize the represented types.  For
// example, given an enumerator representing a scalar type, the
// 'balcl::OptionType::toArrayType' method returns the enumerator representing
// an array of that scalar type.
//
///Enumerators
///-----------
//..
//  Enumerator       Type Alias    Type
//  ---------------- ------------- -------------------------------
//  e_VOID           n/a           void
//  e_BOOL           Bool          bool
//  e_CHAR           Char          char
//  e_INT            Int           int
//  e_INT64          Int64         bsls::Types::Int64
//  e_DOUBLE         Double        double
//  e_STRING         String        bsl::string
//  e_DATETIME       Datetime      bdlt::Datetime
//  e_DATE           Date          bdlt::Date
//  e_TIME           Time          bdlt::Time
//  e_CHAR_ARRAY     CharArray     bsl::vector<char>
//  e_INT_ARRAY      IntArray      bsl::vector<int>
//  e_INT64_ARRAY    Int64Array    bsl::vector<bsls::Types::Int64>
//  e_DOUBLE_ARRAY   DoubleArray   bsl::vector<double>
//  e_STRING_ARRAY   StringArray   bsl::vector<bsl::string>
//  e_DATETIME_ARRAY DatetimeArray bsl::vector<bdlt::Datetime>
//  e_DATE_ARRAY     DateArray     bsl::vector<bdlt::Date>
//  e_TIME_ARRAY     TimeArray     bsl::vector<bdlt::Time>
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of
// 'balcl::OptionType' usage.
//
// First, we create a variable 'value' of type 'balcl::OptionType::Enum' and
// initialize it to the value 'balcl::OptionType::e_STRING':
//..
//  balcl::OptionType::Enum value = balcl::OptionType::e_STRING;
//..
// Next, we store a pointer to its ASCII representation in a variable
// 'asciiValue' of type 'const char *':
//..
//  const char *asciiValue = balcl::OptionType::toAscii(value);
//  assert(0 == bsl::strcmp(asciiValue, "STRING"));
//..
// Finally, we print the value to 'bsl::cout':
//..
//  bsl::cout << value << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  STRING
//..
//
///Example 2: Utility Methods
/// - - - - - - - - - - - - -
// In a software system devoted to assembling option values of various types,
// the code is often governed in terms of the enumerated values
// ('balcl::OptionType::Enum') corresponding to the various types.  In
// particular, in order to assemble an option value of one of the array types
// (e.g., 'balcl::OptionType::e_STRING_ARRAY'), one must first construct the
// constitute elements.
//
// Suppose we have a class, 'MyMultitypeValue', that can, at runtime, be set to
// contain a value of one of the types named by 'balcl::OptionType'.  We may
// want to initialize a 'MyMultitypeValue' object from an input stream using a
// utility function 'MyMultitypeValueUtil::parse':
//..
//  int MyMultitypeValueUtil::parse(MyMultitypeValue        *result,
//                                  bsl::ostream&            input,
//                                  balcl::OptionType::Enum  type)
//  {
//      BSLS_ASSERT(result);
//
//      result->setType(type);
//..
// If 'type' is not one of the array types, as determined by the
// 'balcl::OptionType::isArrayType' method, one calls
// 'MyMultitypeValueUtil::parseScalar':
//..
//      if (!balcl::OptionType::isArrayType(type)) {
//          return MyMultitypeValueUtil::parseScalar(result, input, type);
//                                                                    // RETURN
//      } else {
//..
// Otherwise, we have an array type.  In this case, we must call 'parseScalar'
// repeatedly and build a vector of those scalar values.  The scalar type can
// be calculated from the given array type by the
// 'balcl::OptionType::fromArrayType' method:
//..
//          balcl::OptionType::Enum scalarType =
//                                      balcl::OptionType::fromArrayType(type);
//
//          MyMultitypeValue element(scalarType);
//
//          int rc;
//          while (0 == (rc = MyMultitypeValueUtil::parseScalar(&element,
//                                                              input,
//                                                              scalarType))) {
//              result->append(element);
//          }
//          return rc;                                                // RETURN
//      }
//  }
//..

#include <balscm_version.h>

#include <bsls_assert.h>
#include <bsls_types.h>  // 'bsls::Types::Int64'

#include <bsl_iosfwd.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace bdlt { class Datetime; }
namespace bdlt { class Date; }
namespace bdlt { class Time; }

namespace balcl {
                        // =================
                        // struct OptionType
                        // =================

struct OptionType {
    // This 'struct' provides a namespace for enumerating types used for
    // command-line option values.  See {Enumerators} for details.
    //
    // This 'struct':
    //: o Supports a complete set of *enumeration* operations.
    //: o Provides several additional utility functions.
    //
    // For terminology {'bsldoc_glossary'}.

  public:
    // TYPES
    typedef bool                            Bool;
    typedef char                            Char;
    typedef int                             Int;
    typedef bsls::Types::Int64              Int64;
    typedef double                          Double;
    typedef bsl::string                     String;
    typedef bdlt::Datetime                  Datetime;
    typedef bdlt::Date                      Date;
    typedef bdlt::Time                      Time;
    typedef bsl::vector<char>               CharArray;
    typedef bsl::vector<int>                IntArray;
    typedef bsl::vector<bsls::Types::Int64> Int64Array;
    typedef bsl::vector<double>             DoubleArray;
    typedef bsl::vector<bsl::string>        StringArray;
    typedef bsl::vector<bdlt::Datetime>     DatetimeArray;
    typedef bsl::vector<bdlt::Date>         DateArray;
    typedef bsl::vector<bdlt::Time>         TimeArray;
        // Aliases for each of the supported command-line-option types.

    enum Enum {
        // Enumerate the set of value types available for command-line options.
        e_VOID
      , e_BOOL
      , e_CHAR
      , e_INT
      , e_INT64
      , e_DOUBLE
      , e_STRING
      , e_DATETIME
      , e_DATE
      , e_TIME
      , e_CHAR_ARRAY
      , e_INT_ARRAY
      , e_INT64_ARRAY
      , e_DOUBLE_ARRAY
      , e_STRING_ARRAY
      , e_DATETIME_ARRAY
      , e_DATE_ARRAY
      , e_TIME_ARRAY
    };

    template <Enum TYPE_ENUMERATOR>
    struct EnumToType {
        // 'EnumToType<TYPE_ENUMERATOR>::type' is the (C++) type corresponding
        // to the (template parameter) 'TYPE_ENUMERATOR' 'Enum' value.  See
        // 'TypeToEnum' for the inverse metafunction.
    };

    template <class TYPE>
    struct TypeToEnum {
        // 'TypeToEnum<TYPE>::value' is the 'Enum' value corresponding to the
        // (template parameter) 'TYPE' where 'TYPE' matches one of the
        // 'typedef's defined above or 'void'.  See 'EnumToType' for the
        // inverse metafunction.
    };

    // PUBLIC CLASS DATA

// BDE_VERIFY pragma: -MN04: // Pointer member names must end in '_p'

// BDE_VERIFY pragma: -KS02: // Tag implicitly requires private declaration
                             // These rules not intended for public data.

    static Bool          * const k_BOOL;
    static Char          * const k_CHAR;
    static Int           * const k_INT;
    static Int64         * const k_INT64;
    static Double        * const k_DOUBLE;
    static String        * const k_STRING;
    static Datetime      * const k_DATETIME;
    static Date          * const k_DATE;
    static Time          * const k_TIME;
    static CharArray     * const k_CHAR_ARRAY;
    static IntArray      * const k_INT_ARRAY;
    static Int64Array    * const k_INT64_ARRAY;
    static DoubleArray   * const k_DOUBLE_ARRAY;
    static StringArray   * const k_STRING_ARRAY;
    static DatetimeArray * const k_DATETIME_ARRAY;
    static DateArray     * const k_DATE_ARRAY;
    static TimeArray     * const k_TIME_ARRAY;
        // Statically initialized null pointers, one for each supported
        // command-line-option type.

// BDE_VERIFY pragma: +KS02: // Tag implicitly requires private declaration

// BDE_VERIFY pragma: +MN04: // Pointer member names must end in '_p'

    // CLASS METHODS
    static Enum fromArrayType(Enum type);
        // If 'isArrayType(type)' for the specified 'type', then return the
        // type of the elements of that array 'type'; otherwise return
        // 'e_VOID'.

    static bool isArrayType(Enum type);
        // Return 'true' if the specified 'type' corresponds to an array type,
        // and 'false' otherwise.

    static Enum toArrayType(Enum type);
        // If there is an array type whose elements have the specified 'type'
        // then return that array type; otherwise return 'e_VOID'.
        // 'e_VOID == toArrayType(e_BOOL)' because an array of boolean values
        // is not allowed as a command-line-option type.  Note that
        // 'type == fromArrayType(TYPE)' when 'TYPE != e_VOID' is returned.

                                  // Aspects

    static bsl::ostream& print(bsl::ostream&    stream,
                               OptionType::Enum value,
                               int              level          = 0,
                               int              spacesPerLevel = 4);
        // Write the string representation of the specified enumeration 'value'
        // to the specified output 'stream', and return a reference to
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  See 'toAscii' for
        // what constitutes the string representation of a 'OptionType::Enum'
        // value.

    static const char *toAscii(OptionType::Enum value);
        // Return the (non-modifiable) string representation corresponding to
        // the specified enumeration 'value', if it exists, and a unique
        // (error) string otherwise.  The string representation of 'value'
        // matches its corresponding enumerator name with the 'e_' prefix
        // elided.  For example:
        //..
        //  bsl::cout << balcl::OptionType::toAscii(
        //                                    OptionType::e_STRING);
        //..
        // will print the following on standard output:
        //..
        //  STRING
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, OptionType::Enum value);
    // Write the string representation of the specified enumeration 'value' to
    // the specified output 'stream' in a single-line format, and return a
    // reference to 'stream'.  See 'toAscii' for what constitutes the string
    // representation of a 'OptionType::Enum' value.  Note that this method has
    // the same behavior as:
    //..
    //  balcl::OptionType::print(stream, value, 0, -1);
    //..

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // -----------------
                             // struct OptionType
                             // -----------------

// CLASS METHODS
inline
OptionType::Enum OptionType::fromArrayType(OptionType::Enum type)
{
    BSLS_ASSERT(e_VOID <= type);
    BSLS_ASSERT(type   <= e_TIME_ARRAY);

    if (type < e_CHAR_ARRAY) {
        return e_VOID;                                                // RETURN
    }

    return static_cast<OptionType::Enum>(
                     static_cast<int>(type) - (  static_cast<int>(e_CHAR_ARRAY)
                                               - static_cast<int>(e_CHAR)));
}

inline
bool OptionType::isArrayType(OptionType::Enum type)
{
    BSLS_ASSERT(e_VOID <= type);
    BSLS_ASSERT(type   <= e_TIME_ARRAY);

    return e_CHAR_ARRAY <= type;
}

inline
OptionType::Enum OptionType::toArrayType(OptionType::Enum type)
{
    BSLS_ASSERT(e_VOID <= type);
    BSLS_ASSERT(type   <= e_TIME_ARRAY);

    return e_VOID == type || e_BOOL == type || e_CHAR_ARRAY <= type
         ? e_VOID
         : static_cast<OptionType::Enum>(
                    static_cast<int>(type) + (  static_cast<int>(e_CHAR_ARRAY)
                                              - static_cast<int>(e_CHAR)));
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& balcl::operator<<(bsl::ostream&    stream,
                                OptionType::Enum value)
{
    return OptionType::print(stream, value, 0, -1);
}

namespace balcl {

                        // -----------------------------
                        // struct OptionType::EnumToType
                        // -----------------------------

// TYPES
template <>
struct OptionType::EnumToType<OptionType::e_VOID> {
    // TYPES
    typedef void type;
};

template <>
struct OptionType::EnumToType<OptionType::e_BOOL> {
    // TYPES
    typedef Bool type;
};

template <>
struct OptionType::EnumToType<OptionType::e_CHAR> {
    // TYPES
    typedef Char type;
};

template <>
struct OptionType::EnumToType<OptionType::e_INT> {
    // TYPES
    typedef Int type;
};

template <>
struct OptionType::EnumToType<OptionType::e_INT64> {
    // TYPES
    typedef Int64 type;
};

template <>
struct OptionType::EnumToType<OptionType::e_DOUBLE> {
    // TYPES
    typedef Double type;
};

template <>
struct OptionType::EnumToType<OptionType::e_STRING> {
    // TYPES
    typedef String type;
};

template <>
struct OptionType::EnumToType<OptionType::e_DATETIME> {
    // TYPES
    typedef Datetime type;
};

template <>
struct OptionType::EnumToType<OptionType::e_DATE> {
    // TYPES
    typedef Date type;
};

template <>
struct OptionType::EnumToType<OptionType::e_TIME> {
    // TYPES
    typedef Time type;
};

template <>
struct OptionType::EnumToType<OptionType::e_CHAR_ARRAY> {
    // TYPES
    typedef CharArray type;
};

template <>
struct OptionType::EnumToType<OptionType::e_INT_ARRAY> {
    // TYPES
    typedef IntArray type;
};

template <>
struct OptionType::EnumToType<OptionType::e_INT64_ARRAY> {
    // TYPES
    typedef Int64Array type;
};

template <>
struct OptionType::EnumToType<OptionType::e_DOUBLE_ARRAY> {
    // TYPES
    typedef DoubleArray type;
};

template <>
struct OptionType::EnumToType<OptionType::e_STRING_ARRAY> {
    // TYPES
    typedef StringArray type;
};

template <>
struct OptionType::EnumToType<OptionType::e_DATETIME_ARRAY> {
    // TYPES
    typedef DatetimeArray type;
};

template <>
struct OptionType::EnumToType<OptionType::e_DATE_ARRAY> {
    // TYPES
    typedef DateArray type;
};

template <>
struct OptionType::EnumToType<OptionType::e_TIME_ARRAY> {
    // TYPES
    typedef OptionType::TimeArray type;
};

                        // -----------------------------
                        // struct OptionType::TypeToEnum
                        // -----------------------------

// TYPES
template <>
struct OptionType::TypeToEnum<void> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::Bool> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::Char> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::Int> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::Int64> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::Double> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::String> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::Datetime> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::Date> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::Time> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::CharArray> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::IntArray> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::Int64Array> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::DoubleArray> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::StringArray> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::DatetimeArray> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::DateArray> {
    // PUBLIC DATA
    static const Enum value;
};

template <>
struct OptionType::TypeToEnum<OptionType::TimeArray> {
    // PUBLIC DATA
    static const Enum value;
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
