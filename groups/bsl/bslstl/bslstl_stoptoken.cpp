// bslstl_stoptoken.cpp                                               -*-C++-*-
#include <bslstl_stoptoken.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslstl_stoptoken_cpp, "$Id$ $CSID$")

///Implementation Notes
///--------------------
// The member 'stop_callback::d_node' was deliberately not made a base class,
// in order to avoid exposing any virtual methods (even private ones) in the
// interface of 'stop_callback'.

#include <bslstl_optional.h>  // for testing only

namespace bsl {

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
extern const nostopstate_t nostopstate = nostopstate_t();
#endif

                              // ----------------
                              // class stop_token
                              // ----------------

// ACCESSORS
bool stop_token::stop_possible() const BSLS_KEYWORD_NOEXCEPT
{
    if (!d_state_p) {
        return false;                                                 // RETURN
    }
    if (d_state_p->d_stopSourceCount.loadRelaxed()) {
        return true;                                                  // RETURN
    }
    return d_state_p->stopRequested();
}

bool stop_token::stop_requested() const BSLS_KEYWORD_NOEXCEPT
{
    return d_state_p && d_state_p->stopRequested();
}

                             // -----------------
                             // class stop_source
                             // -----------------

// CREATORS
stop_source::stop_source()
: d_state_p(make_shared<RefCountedState>())
{
    d_state_p->d_stopSourceCount.storeRelaxed(1);
}

stop_source::stop_source(const stop_source& original) BSLS_KEYWORD_NOEXCEPT
: d_state_p(original.d_state_p)
{
    if (d_state_p) {
        d_state_p->d_stopSourceCount.addRelaxed(1);
    }
}

stop_source::stop_source(bsl::allocator<char> allocator)
: d_state_p(allocate_shared<RefCountedState>(allocator.mechanism()))
{
    d_state_p->d_stopSourceCount.storeRelaxed(1);
}

stop_source::~stop_source()
{
    if (d_state_p) {
        d_state_p->d_stopSourceCount.addRelaxed(-1);
    }
}

// MANIPULATORS
stop_source& stop_source::operator=(const stop_source& other)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    stop_source(other).swap(*this);
    return *this;
}

stop_source& stop_source::operator=(
       BloombergLP::bslmf::MovableRef<stop_source> other) BSLS_KEYWORD_NOEXCEPT
{
    stop_source(MoveUtil::move(other)).swap(*this);
    return *this;
}

bool stop_source::request_stop() BSLS_KEYWORD_NOEXCEPT
{
    return d_state_p && d_state_p->requestStop();
}

// ACCESSORS
stop_token stop_source::get_token() const BSLS_KEYWORD_NOEXCEPT
{
    return stop_token(d_state_p);
}

bool stop_source::stop_possible() const BSLS_KEYWORD_NOEXCEPT
{
    return d_state_p;
}

bool stop_source::stop_requested() const BSLS_KEYWORD_NOEXCEPT
{
    return d_state_p && d_state_p->stopRequested();
}
}  // close namespace bsl

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
