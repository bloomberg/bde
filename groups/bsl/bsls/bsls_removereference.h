// bsls_removereference.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_REMOVEREFERENCE
#define INCLUDED_BSLS_REMOVEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type transformation trait removing reference qualifiers.
//
//@CLASSES:
//  bsls::RemoveReference: type traits to remove reference qualifiers
//
//@SEE_ALSO: bslmf::RemoveReference
//
//@DESCRIPTION: This component provides the class template
//  'bsls::RemoveReference' implementing a type transformation removing any
//  reference qualification from a type. The argument type is left unchanged
//  otherwise.
//
///Usage
///-----
//  'bsls::RemoveReference' can be used when a template argument may be
//  reference type but an object type is required:
//..
//  template <class TYPE>
//  typename bsls::RemoveReference<TYPE>::type read(std::istream& in)
//      // Read an object of template argument type from the specified stream
//      // 'in' and return the read value. To determine if the reading the
//      // value was successful check the status of 'in' (i.e., either the
//      // conversion to 'bool' or '!in.fail()'.
//  {
//      typedef typename bsls::RemoveReference<TYPE>::type type;
//      type value = type();
//      in >> value;
//      return value;
//  }
//..
//  This function works even if it is called with a reference type, e.g.
//..
//  std::istringstream in("13");
//  int                value = read<int&>(in);
//  assert(in);
//  assert(value == 13);
//..
//  Typically the template argument 'TYPE' to 'RemoveReference<TYPE>' is itself
//  a template argument or a type obtained from a template argument.

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

namespace BloombergLP {

namespace bsls {

template <class TYPE>
struct RemoveReference {
    // This class template provides a type transformation removing reference
    // qualification, if any, from the type 'TYPE'.
    typedef TYPE type;
        // The default implementation defines the nested type 'type' to be
        // identical to the template argument.
};

template <class TYPE>
struct RemoveReference<TYPE&> {
    typedef TYPE type;
        // The specialization for lvalue references defines the nested type
        // 'type' to be the template argument with the lvalue reference
        // qualification removed.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <class TYPE>
struct RemoveReference<TYPE&&> {
    typedef TYPE type;
        // The specialization for rvalue references defines the nested type
        // 'type' to be the template argument with the rvalue reference
        // qualification removed.
};

#endif

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------

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
