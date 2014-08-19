// bslstl_sharedptr.cpp                                               -*-C++-*-
#include <bslstl_sharedptr.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslstl_deque.h>          // for testing only
#include <bslstl_list.h>           // for testing only
#include <bslstl_map.h>            // for testing only
#include <bslstl_string.h>         // for testing only
#include <bslstl_vector.h>         // for testing only

#include <bslma_newdeleteallocator.h>
#include <bsls_alignmentutil.h>

#include <stdio.h>                 // 'sprintf'

///IMPLEMENTATION NOTES
///--------------------
// The following expression is a class invariant of 'bcema_shared_ptr' and
// shall always be 'true':
//..
//  !d_rep_p || d_ptr_p
//..
// However, this will not be true for the standard 'bsl::shared_ptr', which
// must also permit the state:
//..
//  !d_rep_p && !d_ptr_p
//..
// implying that null pointer values are still reference counted, and will
// invoke their deleter (with a null pointer value) when the last shared owner
// is destroyed.

namespace BloombergLP {
namespace bslstl {

                         // ---------------------------
                         // class bslstl::SharedPtrUtil
                         // ---------------------------

// MANIPULATORS
bsl::shared_ptr<char>
SharedPtrUtil::createInplaceUninitializedBuffer(
                                              bsl::size_t       bufferSize,
                                              bslma::Allocator *basicAllocator)
{
    basicAllocator = bslma::Default::allocator(basicAllocator);
                                                       // allocator is optional

    // We have alignment concerns here: there are no alignment issues with
    // 'bsl::shared_ptrRep', but the buffer address (i.e., the address of
    // 'd_instance' in the 'bsl::shared_ptrInplaceRep' object) must be at least
    // *naturally* *aligned* to 'bufferSize'.  (See 'bslma' package
    // documentation for a definition of natural alignment.)  We achieve this
    // in the simplest way by always maximally aligning the returned pointer.

    typedef bslma::SharedPtrInplaceRep<bsls::AlignmentUtil::MaxAlignedType>
                                                                           Rep;

    enum {
        k_ALIGNMENT_MASK = ~(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
    };

    bsl::size_t repSize = (sizeof(Rep) + bufferSize - 1) & k_ALIGNMENT_MASK;

    Rep *rep = new (basicAllocator->allocate(repSize)) Rep(basicAllocator);

    return bsl::shared_ptr<char>(reinterpret_cast<char *>(rep->ptr()), rep);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg Finance L.P.
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
