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

// ===========================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

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
// Copyright (C) 2012 Bloomberg L.P.
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
