// bdlb_nullopt.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLB_NULLOPT
#define INCLUDED_BDLB_NULLOPT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a tag type to precede allocator arguments.
//
//@CLASSES:
//  NullOptType: tag indicating the next parameter is an allocator
//
//  nullOpt: A literal value of type 'NullOptType'
//
//@SEE_ALSO: bdlb_nullablevalue
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component provides a class, 'bdlb::NullOptType', that
// defines a vocabulary to be used in functions that want to indicate an empty
// nullable value.
//
// In addition to the 'NullOptType' class type, this component defines a
// constant, 'bdlb::nullOpt', of type 'NullOptType', to act as a literal of
// this type, much as 'nullptr' is a (built in) literal for null pointers.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: ...

#include <bslscm_version.h>

#include <bsls_keyword.h>

namespace BloombergLP {
namespace bdlb {

                                // =================
                                // class NullOptType
                                // =================

class NullOptType {
    // This 'class' provides an empty tag type so that 'bdlb::NullableValue'
    // can be explicitly constructed in or assigned to an empty state.  There
    // are no publicly accessible constructors for this type other than the
    // copy constructor.  In particular, it is not default constructible, nor
    // list-initializable in C++11.

  private:
    // PRIVATE CREATORS

    BSLS_KEYWORD_CONSTEXPR NullOptType()
        // Create a 'NullOptType' value.  This constructor is not accessible to
        // users.
    {
        // This 'constexpr' function is defined inline only to allow use
        // initialializing the 'nullOpt' value below.
    }

  public:

    // CLASS METHODS

    static BSLS_KEYWORD_CONSTEXPR NullOptType makeInitialValue()
        // Return a value initialized 'NullOptType' object.  It is undefined
        // behavior to call this function other than to initialize the global
        // constant, 'nullOpt' below.
    {
        // This 'constexpr' function is defined inline only to allow use
        // initialializing the 'nullOpt' value below.

        return NullOptType();
    }

};

BSLS_KEYWORD_INLINE_CONSTEXPR NullOptType nullOpt =
                                               NullOptType::makeInitialValue();
    // Value of type 'NullOptType' that serves as a literal value for the empty
    // state of any nullable value.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close package namespace
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BDLB_NULLOPT)

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
