// bdlat_enumutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLAT_ENUMUTIL
#define INCLUDED_BDLAT_ENUMUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions for decoding enumerations with fallback values.
//
//@CLASSES:
//  bdlat::EnumUtil: namespace for utility functions for decoding enumerations
//
//@SEE_ALSO: bdlat_enumfunctions
//
//@DESCRIPTION: This component provides a 'struct', 'bdlat::EnumUtil', with two
// utility function templates, 'fromIntOrFallbackIfEnabled' and
// 'fromStringOrFallbackIfEnabled', which respectively attempt to decode a
// 'bdlat' "enumeration" type from the integral or string representation.
// However, unlike the decoding functions in the 'bdlat_EnumFunctions'
// namespace, when the functions in this component are given values that do not
// correspond to any enumerator, they attempt to set the result to the
// "fallback" enumerator value, if possible, instead of failing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// Suppose you have a C++ 'enum' type called 'ImageType' whose enumerators
// represent supported formats for image files, and it exposes "enumeration"
// behavior as described in {'bdlat_enumfunctions'}:
//..
// #include <bdlat_enumfunctions.h>
// #include <bdlb_string.h>
// #include <bsl_string.h>
//
// namespace BloombergLP {
// namespace mine {
//
// enum ImageType {
//     JPG     = 0,
//     PNG     = 1,
//     GIF     = 2,
//     UNKNOWN = 100
// };
//
// // MANIPULATORS
//
// int bdlat_enumFromInt(ImageType* result, int number);
//
// int bdlat_enumFromString(ImageType  *result,
//                          const char *string,
//                          int         stringLength);
//
// int bdlat_enumMakeFallback(ImageType *result);
//
// // ACCESSORS
//
// void bdlat_enumToInt(int *result, const ImageType& value);
//
// void bdlat_enumToString(bsl::string *result, const ImageType& value);
//
// bool bdlat_enumHasFallback(const ImageType&);
//
// bool bdlat_enumIsFallback(const ImageType& value);
//
// }  // close namespace mine
//..
// Next, we provide definitions for the 'bdlat_enum*' customization point
// function overloads:
//..
//  // MANIPULATORS
//
//  inline
//  int mine::bdlat_enumFromInt(ImageType *result, int number)
//  {
//      enum { SUCCESS = 0, NOT_FOUND = -1 };
//
//      switch (number) {
//        case JPG: {
//          *result = JPG;
//          return SUCCESS;
//        }
//        case PNG: {
//          *result = PNG;
//          return SUCCESS;
//        }
//        case GIF: {
//          *result = GIF;
//          return SUCCESS;
//        }
//        case UNKNOWN: {
//          *result = UNKNOWN;
//          return SUCCESS;
//        }
//        default: {
//          return NOT_FOUND;
//        }
//      }
//  }
//
//  inline
//  int mine::bdlat_enumFromString(ImageType  *result,
//                                 const char *string,
//                                 int         stringLength)
//  {
//      enum { SUCCESS = 0, NOT_FOUND = -1 };
//
//      if (bdlb::String::areEqualCaseless("jpg",
//                                         string,
//                                         stringLength)) {
//          *result = JPG;
//          return SUCCESS;
//      }
//
//      if (bdlb::String::areEqualCaseless("png",
//                                         string,
//                                         stringLength)) {
//          *result = PNG;
//          return SUCCESS;
//      }
//
//      if (bdlb::String::areEqualCaseless("gif",
//                                         string,
//                                         stringLength)) {
//          *result = GIF;
//          return SUCCESS;
//      }
//      if (bdlb::String::areEqualCaseless("unknown",
//                                         string,
//                                         stringLength)) {
//          *result = UNKNOWN;
//          return SUCCESS;
//      }
//
//      return NOT_FOUND;
//  }
//
//  inline
//  int mine::bdlat_enumMakeFallback(ImageType *result)
//  {
//      *result = UNKNOWN;
//      return 0;
//  }
//
//  // ACCESSORS
//
//  inline
//  void mine::bdlat_enumToInt(int *result, const ImageType& value)
//  {
//      *result = static_cast<int>(value);
//  }
//
//  inline
//  void mine::bdlat_enumToString(bsl::string *result, const ImageType& value)
//  {
//      switch (value) {
//        case JPG: {
//          *result = "JPG";
//        } break;
//        case PNG: {
//          *result = "PNG";
//        } break;
//        case GIF: {
//          *result = "GIF";
//        } break;
//        case UNKNOWN: {
//          *result = "UNKNOWN";
//        } break;
//        default: {
//          *result = "INVALID";
//        } break;
//      }
//  }
//
//  inline
//  bool mine::bdlat_enumHasFallback(const ImageType&)
//  {
//      return true;
//  }
//
//  inline
//  bool mine::bdlat_enumIsFallback(const ImageType& value)
//  {
//      return value == UNKNOWN;
//  }
//..
// To complete the implementation of 'mine::ImageType' as an "enumeration" type
// with fallback enumerator recognized by the 'bdlat' framework, we specialize
// the necessary traits:
//..
//  namespace bdlat_EnumFunctions {
//  template <>
//  struct IsEnumeration<mine::ImageType> {
//      enum { VALUE = 1 };
//  };
//  template <>
//  struct HasFallbackEnumerator<mine::ImageType> {
//      enum { VALUE = 1 };
//  };
//  }  // close namespace bdlat_EnumFunctions
//  }  // close enterprise namespace
//..
// We can now use the methods in 'EnumUtil' to decode integral and string
// values into 'mine::ImageType' values, falling back to the 'mine::UNKNOWN'
// enumerator value when the integral or string value does not correspond to
// any enumerator:
//..
// void usageExample()
// {
//     using namespace BloombergLP;
//
//     mine::ImageType imageType;
//     int rc;
//
//     rc = bdlat::EnumUtil::fromIntOrFallbackIfEnabled(&imageType, 1);
//     assert(0         == rc);
//     assert(mine::PNG == imageType);
//
//     rc = bdlat::EnumUtil::fromIntOrFallbackIfEnabled(&imageType, 4);
//     assert(0             == rc);
//     assert(mine::UNKNOWN == imageType);
//
//     rc = bdlat::EnumUtil::fromStringOrFallbackIfEnabled(&imageType,
//                                                         "GIF",
//                                                         3);
//     assert(0         == rc);
//     assert(mine::GIF == imageType);
//
//     rc = bdlat::EnumUtil::fromStringOrFallbackIfEnabled(&imageType,
//                                                         "WEBP",
//                                                         4);
//     assert(0             == rc);
//     assert(mine::UNKNOWN == imageType);
// }
//..
// Note that the methods in 'EnumUtil' may also be used with 'bdlat'
// "enumeration" types that do not support a fallback enumerator.  In such
// cases, they can fail by returning a non-zero error code.

#include <bdlat_enumfunctions.h>

#include <bslmf_assert.h>

#include <bsls_platform.h>

namespace BloombergLP {
namespace bdlat {

                              // ===============
                              // struct EnumUtil
                              // ===============

struct EnumUtil {
    // This 'struct' provides a namespace for functions that decode 'bdlat'
    // "enumerations" from integral and string representations, possibly to the
    // "fallback" value (if one exists).

    // CLASS METHODS
    template <class TYPE>
    static int fromIntOrFallbackIfEnabled(TYPE *result, int number);
        // Load into the specified 'result' the enumerator matching the
        // specified 'number' and return 0 if such an enumerator exists.
        // Otherwise, if 'result' supports a fallback enumerator, load into
        // 'result' the fallback enumerator value and return 0.  Otherwise,
        // return a non-zero value with no effect on 'result'.  The behavior is
        // undefined unless 'TYPE' is a 'bdlat' enumeration type.

    template <class TYPE>
    static int fromStringOrFallbackIfEnabled(TYPE       *result,
                                             const char *string,
                                             int         stringLength);
        // Load into the specified 'result' the enumerator matching the
        // specified 'string' of the specified 'stringLength' and return 0 if
        // such an enumerator exists.  Otherwise, if 'result' supports a
        // fallback enumerator, load into 'result' the fallback enumerator
        // value and return 0.  Otherwise, return a non-zero value with no
        // effect on 'result'.  The behavior is undefined unless 'TYPE' is a
        // 'bdlat' enumeration type.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct EnumUtil
                              // ---------------

// CLASS METHODS
template <class TYPE>
int EnumUtil::fromIntOrFallbackIfEnabled(TYPE *result, int number)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT((bdlat_EnumFunctions::IsEnumeration<TYPE>::VALUE));
#endif

    if (0 == bdlat_EnumFunctions::fromInt(result, number)) {
        return 0;                                                     // RETURN
    }

    return bdlat_EnumFunctions::makeFallback(result);
}

template <class TYPE>
int EnumUtil::fromStringOrFallbackIfEnabled(TYPE       *result,
                                            const char *string,
                                            int         stringLength)
{
#if !defined(BSLS_PLATFORM_CMP_SUN)
    BSLMF_ASSERT((bdlat_EnumFunctions::IsEnumeration<TYPE>::VALUE));
#endif

    if (0 == bdlat_EnumFunctions::fromString(result, string, stringLength)) {
        return 0;                                                     // RETURN
    }

    return bdlat_EnumFunctions::makeFallback(result);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BDLAT_ENUMUTIL

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
