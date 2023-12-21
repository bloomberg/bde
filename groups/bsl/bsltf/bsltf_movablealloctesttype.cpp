// bsltf_movablealloctesttype.cpp                                     -*-C++-*-
#include <bsltf_movablealloctesttype.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma warning(disable:4355) // ctor uses 'this' used in member-initializer
#endif

namespace BloombergLP {
namespace bsltf {

                        // --------------------------
                        // class MovableAllocTestType
                        // --------------------------

// CREATORS
MovableAllocTestType::MovableAllocTestType()
: d_data_p(0)
, d_allocator_p(bslma::Default::allocator(0))
, d_self_p(this)
{
    d_data_p = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = 0;
}

MovableAllocTestType::MovableAllocTestType(bslma::Allocator *basicAllocator)
: d_data_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    d_data_p = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = 0;
}

MovableAllocTestType::MovableAllocTestType(int data,
                                           bslma::Allocator *basicAllocator)
: d_data_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    d_data_p = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = data;
}

MovableAllocTestType::MovableAllocTestType(
        bslmf::MovableRef<MovableAllocTestType> original) BSLS_KEYWORD_NOEXCEPT
: d_data_p(0)
, d_allocator_p(bslmf::MovableRefUtil::access(original).d_allocator_p)
, d_self_p(this)
, d_tracker(bslmf::MovableRefUtil::move(
                            bslmf::MovableRefUtil::access(original).d_tracker))
{
    MovableAllocTestType& lvalue = original;

    d_data_p = lvalue.d_data_p;
    lvalue.d_data_p = 0;

    if (! d_data_p) {
        // Moved from a moved-from object; this object is also considered moved
        // from (as well as moved into).
        d_tracker.setCopyMoveState(
            CopyMoveState::Enum(CopyMoveState::e_MOVED_INTO |
                                CopyMoveState::e_MOVED_FROM));
    }
}

MovableAllocTestType::MovableAllocTestType(
                        bslmf::MovableRef<MovableAllocTestType> original,
                        bslma::Allocator                       *basicAllocator)
: d_data_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    MovableAllocTestType& lvalue = original;

    if (d_allocator_p == lvalue.d_allocator_p) {
        d_data_p = lvalue.d_data_p;
        lvalue.d_data_p = 0;
    }
    else {
        d_data_p = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_data_p = lvalue.data();
        if (lvalue.d_data_p) {
            lvalue.d_allocator_p->deallocate(lvalue.d_data_p);
            lvalue.d_data_p = 0;
        }
    }

    // Do not modify original tracker until move succeeds.
    d_tracker = bslmf::MovableRefUtil::move(lvalue.d_tracker);

    if (! d_data_p) {
        // Moved from a moved-from object; this object is also considered moved
        // from (as well as moved into).
        d_tracker.setCopyMoveState(
            CopyMoveState::Enum(CopyMoveState::e_MOVED_INTO |
                                CopyMoveState::e_MOVED_FROM));
    }
}

MovableAllocTestType::MovableAllocTestType(
                                   const MovableAllocTestType&  original,
                                   bslma::Allocator            *basicAllocator)
: d_data_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
, d_tracker(original.d_tracker)
{
    d_data_p = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = original.data();
}

MovableAllocTestType::~MovableAllocTestType()
{
    BSLS_ASSERT_OPT(!d_data_p == CopyMoveState::isMovedFrom(*this));

    d_allocator_p->deallocate(d_data_p);

    // Ensure that this objects has not been bitwise moved.

    BSLS_ASSERT_OPT(this == d_self_p);
}

// MANIPULATORS
MovableAllocTestType&
MovableAllocTestType::operator=(const MovableAllocTestType& rhs)
{
    if (&rhs != this)
    {
        int *newData = static_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        d_allocator_p->deallocate(d_data_p);
        d_data_p = newData;
        *d_data_p = rhs.data();

        d_tracker = rhs.d_tracker;
    }
    return *this;
}

MovableAllocTestType&
MovableAllocTestType::operator=(bslmf::MovableRef<MovableAllocTestType> rhs)
{
    MovableAllocTestType& lvalue = rhs;

    if (&lvalue != this)
    {
        if (lvalue.d_data_p && d_allocator_p == lvalue.d_allocator_p) {
            if (d_data_p) {
                d_allocator_p->deallocate(d_data_p);
            }
            d_data_p = lvalue.d_data_p;
            lvalue.d_data_p = 0;
        }
        else {
            int *newData = static_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
            if (d_data_p) {
                d_allocator_p->deallocate(d_data_p);
            }
            d_data_p = newData;
            *d_data_p = lvalue.data();
            if (lvalue.d_data_p) {
                lvalue.d_allocator_p->deallocate(lvalue.d_data_p);
                lvalue.d_data_p = 0;
            }
        }
    }

    d_tracker = bslmf::MovableRefUtil::move(lvalue.d_tracker);
    if (! d_data_p) {
        // Moved from a moved-from object; this object is also considered moved
        // from (as well as moved into).
        d_tracker.setCopyMoveState(
            CopyMoveState::Enum(CopyMoveState::e_MOVED_INTO |
                                CopyMoveState::e_MOVED_FROM));
    }

    return *this;
}

void MovableAllocTestType::setData(int value)
{
    if (!d_data_p) {
        d_data_p = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    }
    *d_data_p = value;

    d_tracker.resetCopyMoveState();
}

}  // close package namespace
}  // close enterprise namespace

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
