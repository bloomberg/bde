// balb_pipetaskmanager.cpp                                           -*-C++-*-
#include <balb_pipetaskmanager.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balb_pipetaskmanager_cpp,"$Id$ $CSID$")

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdls_pipeutil.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_iostream.h>

namespace BloombergLP {
namespace balb {

namespace {
namespace u {

static
PipeControlChannel *makeControlChannel(ControlManager   *controlManager,
                                       bslma::Allocator *basicAllocator)

    // Return the address, in memory acquired from the specified
    // 'basicAllocator', of a 'balb::PipeControlChannel' object configured to
    // invoke the 'dispatchMethod' function of the specified 'controlManager'
    // and that uses 'basicAllocator' to supply memory.
{
    BSLS_ASSERT(controlManager);
    BSLS_ASSERT(basicAllocator);

    typedef int (ControlManager::*DispatchMessagePointer)(
                                        const bsl::string_view& message) const;

    DispatchMessagePointer dmp = &ControlManager::dispatchMessage;

    return new (*basicAllocator) PipeControlChannel(
                                  bdlf::BindUtil::bind(dmp,
                                                       controlManager,
                                                       bdlf::PlaceHolders::_1),
                                  basicAllocator);
}

}  // close namespace u
}  // close unnamed namespace

                        // ---------------------
                        // class PipeTaskManager
                        // ---------------------

// CREATORS
PipeTaskManager::PipeTaskManager(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_controlChannel_p(0)
, d_controlManager_p(0)
{
    d_controlManager_p = bsl::allocate_shared<ControlManager>(d_allocator_p);
    d_controlChannel_p = u::makeControlChannel(d_controlManager_p.get(),
                                               d_allocator_p);
}

PipeTaskManager::PipeTaskManager(
                              bsl::shared_ptr<ControlManager>&  controlManager,
                              bslma::Allocator                 *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_controlChannel_p(0)
, d_controlManager_p(controlManager)
{
    BSLS_ASSERT(controlManager);

    d_controlChannel_p = u::makeControlChannel(d_controlManager_p.get(),
                                               d_allocator_p);
}

PipeTaskManager::~PipeTaskManager()
{
    BSLS_ASSERT(d_controlManager_p);
    BSLS_ASSERT(d_controlChannel_p);

    shutdown();

    d_allocator_p->deleteObject(d_controlChannel_p);
}

// MANIPULATORS

                        // Execution

int PipeTaskManager::start(const bsl::string_view& pipeBasename)
{
    // Normalize the pipe name
    bsl::string pipeName;
    if (0 != bdls::PipeUtil::makeCanonicalName(&pipeName,
                                               bsl::string(pipeBasename))) {
        return -1;                                                    // RETURN
    }

    return d_controlChannel_p->start(pipeName);
}

}  // close package namespace
}  // close enterprise namespace

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
