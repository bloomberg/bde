// bslstl_badweakptr.h                                                -*-C++-*-
#ifndef INCLUDED_BSLSTL_BADWEAKPTR
#define INCLUDED_BSLSTL_BADWEAKPTR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception class to indicate a weak_ptr has expired.
//
//@CLASSES:
//   bslstl::BadWeakPtr: exception class derived from 'native_std' classes
//    bsl::bad_weak_ptr: alias to an exception type thrown by the bsl library
//
//@SEE_ALSO: bslstl_sharedptr, bslstl_stdexceptionutil
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides ...
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: ...
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_memory.h> instead of <bslstl_badweakptr.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#include <bsls_exceptionutil.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_EXCEPTION
#include <exception>
#define INCLUDED_EXCEPTION
#endif

#ifndef INCLUDED_MEMORY
#include <memory>                // 'std::bad_weak_ptr' if defined
#define INCLUDED_MEMORY
#endif

namespace BloombergLP {
namespace bslstl {  class BadWeakPtr; }  // close namespace bslalg
}  // close enterprise namespace

namespace bsl {
#if 0
typedef native_std::bad_weak_ptr bad_weak_ptr;
#else
typedef ::BloombergLP::bslstl::BadWeakPtr bad_weak_ptr;
#endif

}  // close namespace bsl


namespace BloombergLP {

namespace bslstl {

                       // ================
                       // class BadWeakPtr
                       // ================

class BadWeakPtr : public native_std::exception {
  public:
    BadWeakPtr();
        // Create a 'BadWeakPtr' object.  Note that this function is explicitly
        // user-declared, to make it simple to declare 'const' objects of this
        // type.

    // ACCESSORS
    const char *what() const throw();
        // Return a pointer to the string literal "bad_weak_ptr", with a
        // storage duration of the lifetime of the program.  Note that the
        // caller should *not* attempt to free this memory.  Note that the
        // 'bsls_exceptionutil' macro 'BSLS_NOTHROW_SPEC' is deliberately not
        // used here, as a number of standard libraries declare the base
        // 'exception' class explicitly with the no-throw specification, even
        // in a build that may not recongnise exceptions.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       // ----------------
                       // class BadWeakPtr
                       // ----------------

inline
BadWeakPtr::BadWeakPtr()
: native_std::exception()
{
}

inline
const char *BadWeakPtr::what() const throw()
{
    return "bad_weak_ptr";
}

}  // close namespace bslalg

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
