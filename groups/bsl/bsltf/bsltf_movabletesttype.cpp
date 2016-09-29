// bsltf_movabletesttype.cpp                                     -*-C++-*-
#include <bsltf_movabletesttype.h>

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
                        // class MovableTestType
                        // --------------------------

// CREATORS
MovableTestType::MovableTestType()
: d_data(0)
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
}

MovableTestType::MovableTestType(int data)
: d_data(data)
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
}

MovableTestType::MovableTestType(const MovableTestType&  original)
: d_data(original.d_data)
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_NOT_MOVED)
{
}

MovableTestType::MovableTestType(
               bslmf::MovableRef<MovableTestType> original) BSLS_CPP11_NOEXCEPT
: d_data(bslmf::MovableRefUtil::access(original).d_data)
, d_self_p(this)
, d_movedFrom(bsltf::MoveState::e_NOT_MOVED)
, d_movedInto(bsltf::MoveState::e_MOVED)
{
    MovableTestType& lvalue = original;
    lvalue.d_data = 0;
    lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
}

MovableTestType::~MovableTestType()
{
    BSLS_ASSERT_OPT(bsltf::MoveState::e_MOVED != d_movedFrom || 0 == d_data);

    BSLS_ASSERT_OPT(this == d_self_p);
}

// MANIPULATORS
MovableTestType&
MovableTestType::operator=(const MovableTestType& rhs)
{
    if (&rhs != this)
    {
        d_data = rhs.d_data;
        d_movedFrom = bsltf::MoveState::e_NOT_MOVED;
        d_movedInto = bsltf::MoveState::e_NOT_MOVED;
    }
    return *this;
}

MovableTestType&
MovableTestType::operator=(bslmf::MovableRef<MovableTestType> rhs)
{
    MovableTestType& lvalue = rhs;
    if (&lvalue != this)
    {
        d_data = lvalue.d_data;
        d_movedFrom        = bsltf::MoveState::e_NOT_MOVED;
        d_movedInto        = bsltf::MoveState::e_MOVED;
        lvalue.d_data      = 0;
        lvalue.d_movedFrom = bsltf::MoveState::e_MOVED;
    }
    return *this;
}

void MovableTestType::setData(int value)
{
    d_movedFrom = bsltf::MoveState::e_NOT_MOVED;
    d_movedInto = bsltf::MoveState::e_NOT_MOVED;

    d_data = value;
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
