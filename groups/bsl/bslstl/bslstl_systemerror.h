// bslstl_systemerror.h                                               -*-C++-*-
#ifndef INCLUDED_BSLSTL_SYSTEMERROR
#define INCLUDED_BSLSTL_SYSTEMERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide C++11-defined error classes and functions for C++03.
//
//@CLASSES:
//  bsl::system_error:            C++03 version of std::system_error
//
//@DESCRIPTION: This component defines class 'bsl::system_error', a class used
// for annotated exception objects about 'errno'-style errors.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding Annotation to an Error
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to add an informative message when a system error occurs and
// include that as part of an exception that we throw when reporting the error.
// We can use 'bsl::system_error' to do that.
//
// First, reset 'errno' to avoid detecting old problems.
//..
//  errno = 0;
//..
// Then, do something that will fail and set 'errno'.
//..
//  (void)sqrt(-3.5);
//..
// Next, check that 'errno' was actually set.
//..
//  assert(EDOM == errno);
//..
//  Finally, throw an annotated exception and verify the annotaion and the
//  error code stored within it.
//..
//  try {
//      throw bsl::system_error(errno, system_category(), "sqrt(-3.5)");
//  }
//  catch (bsl::runtime_error& e) {
//      assert(0 != strstr(e.what(), "sqrt(-3.5)"));
//      try {
//          throw;
//      }
//      catch (bsl::system_error& e) {
//          assert(bsl::errc::argument_out_of_domain == e.code().value());
//          assert(&system_category() == &e.code().category());
//      }
//  }
//..

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#include <bslstl_errc.h>
#include <bslstl_error.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <bsls_nativestd.h>

#include <system_error>

namespace bsl {
    using native_std::system_error;
}  // close namespace bsl

#else

#include <errno.h>

#include <cstring>
#include <functional>
#include <ostream>
#include <stdexcept>
#include <string>

namespace bsl {

                             // ==================
                             // class system_error
                             // ==================

class system_error : public std::runtime_error {
    // This class represents exceptions that have an associated error code.

  public:
    // CREATORS
    system_error(error_code code, const std::string& what);
    system_error(error_code code, const char *what);
    system_error(error_code code);                                  // IMPLICIT
        // Create an object of this type holding the specified 'code'.
        // Optionally specify a string 'what' to be added to the description of
        // this object.

    system_error(int                   value,
                 const error_category& category,
                 const std::string&    what);
    system_error(int value, const error_category& category, const char *what);
    system_error(int value, const error_category& category);
        // Create an object of this type holding an error code holding the
        // specified 'value' and 'category'.  Optionally specify a string
        // 'what' to be added to the description of this object.

    // ACCESSORS
    const error_code& code() const;
        // Return a 'const' reference to the error code held by this object.

  private:
    // DATA
    error_code d_code;  // error code
};

}  // close namespace bsl

#endif
#endif

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
