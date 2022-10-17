// bsls_preconditions.cpp                                             -*-C++-*-
#include <bsls_preconditions.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {
namespace bsls {

AtomicOperations::AtomicTypes::Pointer
   PreconditionsHandler::s_beginHandler =
           {(void *)&PreconditionsHandler::noOpHandler};

AtomicOperations::AtomicTypes::Pointer
   PreconditionsHandler::s_endHandler =
           {(void *)PreconditionsHandler::noOpHandler};

                         // --------------------------
                         // class PreconditionsHandler
                         // --------------------------

PreconditionsHandler::PreconditionHandlerType
PreconditionsHandler::getBeginHandler()
{
    return PointerCastUtil::cast<PreconditionHandlerType>(
        AtomicOperations::getPtrAcquire(&s_beginHandler));
}

PreconditionsHandler::PreconditionHandlerType
PreconditionsHandler::getEndHandler()
{
    return PointerCastUtil::cast<PreconditionHandlerType>(
        AtomicOperations::getPtrAcquire(&s_endHandler));
}

void PreconditionsHandler::installHandlers(
                                          PreconditionHandlerType beginHandler,
                                          PreconditionHandlerType endHandler)
{
    AtomicOperations::setPtrRelease(
        &s_beginHandler, PointerCastUtil::cast<void *>(beginHandler));
    AtomicOperations::setPtrRelease(&s_endHandler,
                                    PointerCastUtil::cast<void *>(endHandler));
}

void PreconditionsHandler::invokeBeginHandler()
{
    PreconditionHandlerType beginHandlerPtr =
        PointerCastUtil::cast<PreconditionHandlerType>(
            AtomicOperations::getPtrAcquire(&s_beginHandler));
    beginHandlerPtr();
}

void PreconditionsHandler::invokeEndHandler()
{
    PreconditionHandlerType endHandlerPtr =
        PointerCastUtil::cast<PreconditionHandlerType>(
            AtomicOperations::getPtrAcquire(&s_endHandler));
    endHandlerPtr();
}

void PreconditionsHandler::noOpHandler()
{

}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
