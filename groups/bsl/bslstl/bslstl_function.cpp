// bslstl_function.cpp                  -*-C++-*-

#include <bslstl_function.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

const void *bsl::Function_Rep::unownedAllocManager(ManagerOpCode  opCode,
                                                   Function_Rep  *rep,
                                                   const void    *source)
{
    switch (opCode) {
      case e_MOVE_CONSTRUCT:
      case e_COPY_CONSTRUCT: {
        // Allocator pointer was simply copied. 
        // No copy/move construction needed.
        BSLS_ASSERT(rep->d_allocator_p == source);
      } break;

      case e_DESTROY: {
        std::size_t funcSize = reinterpret_cast<std::size_t>(source);
        if (funcSize > sizeof(InplaceBuffer)) {
            rep->d_allocator_p->deallocate(rep->d_objbuf.d_object_p);
        }
      } break;

      case e_GET_SIZE:    return NULL;
      case e_GET_TARGET:  return rep->d_allocator_p;
      case e_GET_TYPE_ID: return &typeid(bslma::Allocator);
    } // end switch

    return NULL;
}

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
