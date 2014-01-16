// bslma_managedptr_factorydeleter.h                                  -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDPTR_FACTORYDELETER
#define INCLUDED_BSLMA_MANAGEDPTR_FACTORYDELETER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a basic deleter for the managed pointer class.
//
//@CLASSES:
//  bdema::ManagedPtr_FactoryDeleter: deduced deleter used by bslma::ManagedPtr
//
//@AUTHOR: Alisdair Meredith (ameredith1@bloomberg.net)
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component provides a class used as an implementation
// detail by 'bslma::ManagedPtr' to produce a type-erasing deleter function
// that will destroy an object of a parameterized type, using a factory of
// another parameterized type that provides a 'deleteObject' method.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bslma {

                   // ================================
                   // struct ManagedPtr_FactoryDeleter
                   // ================================

template <class ELEMENT_TYPE, class FACTORY>
struct ManagedPtr_FactoryDeleter {
    // This utility provides a general deleter for objects that provide a
    // 'deleteObject' operation (e.g., 'bslma::Allocator', 'bdema_Pool').

    // CLASS METHODS
    static void deleter(void *object, void *factory);
        // Deleter function that deletes the specified 'object' by invoking
        // 'deleteObject' on the specified 'factory', casting 'object' to
        // 'ELEMENT_TYPE *' and 'factory' to 'FACTORY *'.  Note that the
        // behavior is undefined if '0 == object', or if 'factory' does not
        // point to an an object of type 'FACTORY', or 'object' does not point
        // to a complete object of type 'ELEMENT_TYPE' unless 'ELEMENT_TYPE'
        // has a virtual destructor and 'object' points to an object whose
        // dynamic type derives from 'ELEMENT_TYPE.
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // --------------------------------
                    // struct ManagedPtr_FactoryDeleter
                    // --------------------------------

template <class ELEMENT_TYPE, class FACTORY>
inline
void ManagedPtr_FactoryDeleter<ELEMENT_TYPE, FACTORY>::deleter(void *object,
                                                               void *factory)
{
    BSLS_ASSERT_SAFE(0 != object);
    BSLS_ASSERT_SAFE(0 != factory);

    static_cast<FACTORY *>(factory)->deleteObject(
                                          static_cast<ELEMENT_TYPE *>(object));
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
