// bslalg_autoscalardestructor.h                                      -*-C++-*-
#ifndef INCLUDED_BSLALG_AUTOSCALARDESTRUCTOR
#define INCLUDED_BSLALG_AUTOSCALARDESTRUCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor for destroying an object.
//
//@CLASSES:
//  bslalg::AutoScalarDestructor: exception-neutrality proctor for an object
//
//@SEE_ALSO: bslma_autodestructor, bslalg_scalardestructionprimitives
//
//@DESCRIPTION: This component provides a proctor object to manage an
// otherwise-unmanaged instance of a user-defined type.  If not explicitly
// released, the instance managed by the proctor object is automatically
// destroyed by the proctor's destructor, using the
// 'bslalg::ScalarDestructionPrimitives' utility.
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

namespace bslalg {

                     // ==========================
                     // class AutoScalarDestructor
                     // ==========================

template <class OBJECT_TYPE>
class AutoScalarDestructor {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method is called, destroys the
    // guarded object of the parameterized 'OBJECT_TYPE'.

    // DATA
    OBJECT_TYPE *d_object_p;  // address of guarded object

  private:
    // NOT IMPLEMENTED
    AutoScalarDestructor(const AutoScalarDestructor&);
    AutoScalarDestructor& operator=(const AutoScalarDestructor&);

  public:
    // CREATORS
    AutoScalarDestructor(OBJECT_TYPE *object);
        // Create a guard object, proctoring the specified 'object' of the
        // parameterized 'OBJECT_TYPE' (if non-zero), that will invoke the
        // destructor of 'object' upon destruction.

    ~AutoScalarDestructor();
        // Call the destructor on the object of the parameterized 'OBJECT_TYPE'
        // that is proctored by this guard object, if any, and destroy this
        // object.

    // MANIPULATORS
    void release();
        // Set the address of the object proctored by this guard object to 0,
        // thereby releasing from management the object currently managed by
        // this proctor, if any.

    void reset(OBJECT_TYPE *object);
        // Set this guard object to manage the specified 'object', thereby
        // releasing from management any object currently managed by this
        // proctor, if any.
};

// ============================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                     // --------------------------
                     // class AutoScalarDestructor
                     // --------------------------

// CREATORS
template <class OBJECT_TYPE>
inline
AutoScalarDestructor<OBJECT_TYPE>::AutoScalarDestructor(OBJECT_TYPE *object)
: d_object_p(object)
{
}

template <class OBJECT_TYPE>
inline
AutoScalarDestructor<OBJECT_TYPE>::~AutoScalarDestructor()
{
    if (d_object_p) {
        ScalarDestructionPrimitives::destroy(d_object_p);
    }
}

// MANIPULATORS
template <class OBJECT_TYPE>
inline
void AutoScalarDestructor<OBJECT_TYPE>::release()
{
    d_object_p = 0;
}

template <class OBJECT_TYPE>
inline
void AutoScalarDestructor<OBJECT_TYPE>::reset(OBJECT_TYPE *object)
{
    d_object_p = object;
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
