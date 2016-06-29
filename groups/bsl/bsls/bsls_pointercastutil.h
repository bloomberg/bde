// bsls_pointercastutil.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_POINTERCASTUTIL
#define INCLUDED_BSLS_POINTERCASTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide function to cast between function and data pointers.
//
//@CLASSES:
//  bsls::PointerCastUtil: namespace for pointer-casting function
//
//@DESCRIPTION: This component, 'bsls::PointerCastUtil', provides a utility
// function to allow casting between function and data pointers without
// triggering compiler warnings.  Such casts are legal in the latest C++
// standard, but were not always so.
//
///Usage
//------
// This section illustrates intended use of this component.
//
///Example 1: Using a function pointer as a closure parameter
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose there is an event-handling service that requires registration of a
// combination of object and closure value, and that invokes a method on the
// object, passing back the closure.
//
// First we define the service and its handler:
//..
//  struct Handler { virtual void handle(void *closure) = 0; };
//  class Service {
//      Handler *d_handler_p;
//      void    *d_closure_p;
//    public:
//      void registerHandler(Handler *handler, void *closure = 0) {
//          d_handler_p = handler;
//          d_closure_p = closure;
//      }
//      void eventOccurred() { d_handler_p->handle(d_closure_p); }
//  };
//..
// Then, we want to define a handler that will receive a function pointer as
// the closure object and invoke it.  In order to do that, we must cast it to a
// function pointer, but some compilers may not allow it.  We can use
// 'bsls::PointerCastUtil::cast' to accomplish this:
//..
//  struct MyHandler : Handler {
//      void handle(void *closure) {
//           bsls::PointerCastUtil::cast<void(*)()>(closure)();
//      }
//  };
//..
// Next, we will set up a sample service and our handler function:
//..
//  Service aService;
//  static int counter = 0;
//  void event() { ++counter; }
//..
// Finally, we will register our handler and then trigger events to verify that
// our handler is recording them correctly.  To register the function pointer
// as a closure object, we must cast it to a data pointer.  Again, we can use
// 'bsls::PointerCastUtil::cast' to accomplish this:
//..
//  MyHandler ah;
//  aService.registerHandler(&ah, bsls::PointerCastUtil::cast<void *>(event));
//  aService.eventOccurred();
//  aService.eventOccurred();
//  assert(counter == 2);
//..

#ifndef INCLUDED_BSLS_ANNOTATION
#include <bsls_annotation.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bsls {

                        //=======================
                        // struct PointerCastUtil
                        //=======================

struct PointerCastUtil {
    // This 'struct' provides a namespace for a 'static' utility function that
    // allows casting between function and data pointers.

    // CLASS METHODS
    template <class TO_TYPE, class FROM_TYPE>
    static TO_TYPE cast(FROM_TYPE from);
        // Return the specified 'from' value cast to 'TO_TYPE', casting it in
        // two steps, first to an integer type the size of a pointer and then
        // to the target type.  This function is intended to be used to cast
        // between function and data pointers, as doing such a cast directly
        // was once illegal.  The behavior is undefined unless both 'FROM_TYPE'
        // and 'TO_TYPE' are not larger than the intermediate integer type.
};

}  // close package namespace

template <class TO_TYPE, class FROM_TYPE>
inline
TO_TYPE bsls::PointerCastUtil::cast(FROM_TYPE from)
{
    typedef BSLS_ANNOTATION_UNUSED char FROM_TYPE_SizeCheck[
                    sizeof(bsls::Types::IntPtr) >= sizeof(FROM_TYPE) ? 1 : -1];
    typedef BSLS_ANNOTATION_UNUSED char   TO_TYPE_SizeCheck[
                    sizeof(bsls::Types::IntPtr) >= sizeof(TO_TYPE)   ? 1 : -1];
        // Static asserts ensuring that neither 'FROM_TYPE' nor 'TO_TYPE' is
        // larger than the intermediate integer type.  Note that 'bslmf_assert'
        // cannot be used here because of package dependency rules.

    return reinterpret_cast<TO_TYPE>(
                                  reinterpret_cast<bsls::Types::IntPtr>(from));
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
