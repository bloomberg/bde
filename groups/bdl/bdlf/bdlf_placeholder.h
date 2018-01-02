// bdlf_placeholder.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLF_PLACEHOLDER
#define INCLUDED_BDLF_PLACEHOLDER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a parameterized placeholder and specialized placeholders.
//
//@CLASSES:
//   bdlf::PlaceHolder: namespace for parameterized placeholders
//
//@SEE_ALSO: bdlf_bind
//
//@DESCRIPTION: This component provides a parameterized placeholder,
// 'bdlf::PlaceHolder', and fourteen placeholder objects ('_1', '_2', etc.),
// each of a distinct type.  '_1' is of type 'const bdlf::PlaceHolder<1>', '_2'
// is of type 'const bdlf::PlaceHolder<2>', and so on.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

namespace BloombergLP {

namespace bdlf {
template <int I>
struct PlaceHolder {
    BSLMF_NESTED_TRAIT_DECLARATION(PlaceHolder, bslmf::IsBitwiseMoveable);
    enum { VALUE = I };
};

namespace PlaceHolders {

extern const PlaceHolder< 1>  _1;
extern const PlaceHolder< 2>  _2;
extern const PlaceHolder< 3>  _3;
extern const PlaceHolder< 4>  _4;
extern const PlaceHolder< 5>  _5;
extern const PlaceHolder< 6>  _6;
extern const PlaceHolder< 7>  _7;
extern const PlaceHolder< 8>  _8;
extern const PlaceHolder< 9>  _9;
extern const PlaceHolder<10> _10;
extern const PlaceHolder<11> _11;
extern const PlaceHolder<12> _12;
extern const PlaceHolder<13> _13;
extern const PlaceHolder<14> _14;

}  // close namespace PlaceHolders
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
