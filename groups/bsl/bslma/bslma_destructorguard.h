// bslma_destructorguard.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMA_DESTRUCTORGUARD
#define INCLUDED_BSLMA_DESTRUCTORGUARD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a guard to unconditionally manage an object.
//
//@CLASSES:
//  bslma::DestructorGuard: guard to unconditionally manage an object
//
//@SEE_ALSO: bslma_destructorproctor, bslma_autodestructor
//
//@DESCRIPTION: This component provides a guard class template,
// 'bslma::DestructorGuard', to unconditionally manage an (otherwise-unmanaged)
// object of parameterized 'TYPE' supplied at construction.  The managed object
// is destroyed automatically when the guard object goes out of scope by
// calling the (managed) object's destructor.
//
///Usage
///-----
// Suppose we have a situation where one of the two constructors will be called
// to create an object on the stack for performance reasons.  The construction
// thus occurs within either of the branches of an 'if' statement, so the
// object itself, to survive the end of the "then" or "else" block, must be
// constructed in a 'bsls::ObjectBuffer'.  Once constructed, the object would
// not be destroyed automatically, so to make sure it will be destroyed, we
// place it under the management of a 'bslma::DestructorGuard'.  After that, we
// know that however the routine exits -- either by a return or as a result of
// an exception being thrown -- the object will be destroyed.
//..
//  double usageExample(double startValue)
//  {
//      bsls::ObjectBuffer<std::vector<double> > buffer;
//      std::vector<double>& myVec = buffer.object();
//
//      if (startValue >= 0) {
//          new (&myVec) std::vector<double>(100, startValue);
//      }
//      else {
//          new (&myVec) std::vector<double>();
//      }
//
//      //***********************************************************
//      // Note the use of the destructor guard on 'myVec' (below). *
//      //***********************************************************
//
//      bslma::DestructorGuard<std::vector<double> > guard(&myVec);
//..
// Note that regardless of how this routine terminates, 'myVec' will be
// destroyed.
//..
//  // ...
//
//  myVec.push_back(3.0);
//..
// Note that 'push_back' could allocate memory and therefore may throw.
// However, if it does, 'myVec' will be destroyed automatically along with
// 'guard'.
//..
//  if (myVec[0] >= 5.0) {
//      return 5.0;                                               // RETURN
//..
// Note that 'myVec' is automatically destroyed as the function returns.
//..
//  }
//
//  return myVec[myVec.size() / 2];
//..
// Note that 'myVec' is destroyed after the temporary containing the return
// value is created.
//..
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // =====================
                        // class DestructorGuard
                        // =====================

template <class TYPE>
class DestructorGuard {
    // This class implements a guard that unconditionally destroys a managed
    // object upon destruction by invoking the (managed) object's destructor.

    // DATA
    TYPE *d_object_p;  // managed object

    // NOT IMPLEMENTED
    DestructorGuard(const DestructorGuard<TYPE>&);
    DestructorGuard<TYPE>& operator=(const DestructorGuard<TYPE>&);

  public:
    // CREATORS
    explicit DestructorGuard(TYPE *object);
        // Create a destructor guard that unconditionally manages the specified
        // 'object', and invokes the destructor of 'object' upon the
        // destruction of this guard.  The behavior is undefined unless
        // 'object' is non-zero.

    ~DestructorGuard();
        // Destroy this destructor guard and the object it manages by invoking
        // the destructor of the (managed) object.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class DestructorGuard
                        // ---------------------

// CREATORS
template <class TYPE>
inline
DestructorGuard<TYPE>::DestructorGuard(TYPE *object)
: d_object_p(object)
{
    BSLS_ASSERT_SAFE(object);
}

template <class TYPE>
inline
DestructorGuard<TYPE>::~DestructorGuard()
{
    BSLS_ASSERT_SAFE(d_object_p);

    d_object_p->~TYPE();
}

}  // close package namespace


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
