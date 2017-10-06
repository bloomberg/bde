// bslalg_scalardestructionprimitives.h                               -*-C++-*-
#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#define INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide primitive algorithms that destroy scalars.
//
//@DEPRECATED: Use 'bslma_destructionutil' instead.
//
//@CLASSES:
//  bslalg::ScalarDestructionPrimitives: namespace for scalar algorithms
//
//@SEE_ALSO: bslalg_scalarprimitives, bslalg_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides utilities to destroy scalars with a
// uniform interface, but select a different implementation according to the
// traits possessed by the underlying type.
//
// The trait under consideration by this component is:
//..
//  Trait                             Note
//  -------------------------------   -------------------------------------
//  bsl::is_trivially_copyable        Expressed in English as "TYPE has the
//                                    bit-wise copyable trait", or "TYPE is
//                                    bit-wise copyable", this trait also
//                                    implies that destructor calls can be
//                                    elided with no effect on observable
//                                    behavior.
//
//..
//
///Usage
///-----
// In this section we show intended use of this component.  Note that this
// component is for use by the 'bslstl' package.  Other clients should use the
// STL algorithms (in header '<algorithm>' and '<memory>').
//
///Example 1: Destroy 'int' and an Integer Wrapper
///- - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will use 'bslalg::ScalarDestructionPrimitives' to
// destroy both a scalar integer and a 'MyInteger' type object.  Calling the
// 'destroy' method on a scalar integer is a no-op while calling the 'destroy'
// method on an object of 'MyInteger' class invokes the destructor of the
// object.
//
// First, we define a 'MyInteger' class that represents an integer value:
//..
//  class MyInteger {
//      // This class represents an integer value.
//
//      // DATA
//      int d_intValue;  // integer value
//
//    public:
//      // CREATORS
//      MyInteger();
//          // Create a 'MyInteger' object having integer value '0'.
//
//      explicit MyInteger(int value);
//          // Create a 'MyInteger' object having the specified 'value'.
//
//      ~MyInteger();
//          // Destroy this object.
//
//      // ACCESSORS
//      int getValue() const;
//  };
//..
// Then, we create an object, 'myInteger', of type 'MyInteger':
//..
//  bsls::ObjectBuffer<MyInteger> buffer;
//  MyInteger *myInteger = &buffer.object();
//  new (myInteger) MyInteger(1);
//..
// Notice that we use an 'ObjectBuffer' to allow us to safely invoke the
// destructor explicitly.
//
// Now, we define a primitive integer:
//..
//  int scalarInteger = 2;
//..
// Finally, we use the uniform 'bslalg::ScalarDestructionPrimitives:destroy'
// method to destroy both 'myInteger' and 'scalarInteger':
//..
//  bslalg::ScalarDestructionPrimitives::destroy(myInteger);
//  bslalg::ScalarDestructionPrimitives::destroy(&scalarInteger);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DESTRUCTIONUTIL
#include <bslma_destructionutil.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_CSTRING
#include <cstring>  // 'memset', 'memcpy', 'memmove'
#define INCLUDED_CSTRING
#endif

#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslalg {

typedef bslma::DestructionUtil ScalarDestructionPrimitives;
    // This alias is defined for backward compatibility.

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bslalg::ScalarDestructionPrimitives bslalg_ScalarDestructionPrimitives;
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
