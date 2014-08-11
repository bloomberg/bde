// bsltf_stdtestallocator.cpp                                         -*-C++-*-
#include <bsltf_stdtestallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_newdeleteallocator.h>

#include <bsls_assert.h>

#include <limits.h>

namespace
{
// STATIC DATA
// This global static data is declared and defined entirely hidden inside
// the .cpp file, as the IBM compiler may create multiple copies if accessed
// through inline functions defined in the header.
    static ::BloombergLP::bslma::Allocator
                              *s_StdTestAllocatorConfiguration_allocator_p = 0;
}  // close anonymous namespace

namespace BloombergLP {
namespace bsltf {


                        // -----------------------------------
                        // class StdTestAllocatorConfiguration
                        // -----------------------------------

// CLASS METHODS
bslma::Allocator* StdTestAllocatorConfiguration::delegateAllocator()
{
    return s_StdTestAllocatorConfiguration_allocator_p
         ? s_StdTestAllocatorConfiguration_allocator_p
         : &bslma::NewDeleteAllocator::singleton();
}

void StdTestAllocatorConfiguration::setDelegateAllocatorRaw(
                                              bslma::Allocator *basicAllocator)
{
    BSLS_ASSERT_OPT(basicAllocator);

    s_StdTestAllocatorConfiguration_allocator_p = basicAllocator;
}

                        // -----------------------
                        // struct StdTestAllocator
                        // -----------------------

unsigned int StdTestAllocator_CommonUtil::maxSize(size_t elementSize)
{
    return static_cast<unsigned int>(UINT_MAX  / elementSize);
}

                        // ----------------------
                        // class StdTestAllocator
                        // ----------------------


}  // close package namespace
}  // close enterprise namespace

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
