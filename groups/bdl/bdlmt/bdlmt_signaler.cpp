// bdlmt_signaler.cpp                                                 -*-C++-*-
#include <bdlmt_signaler.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_multiprioritythreadpool_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bdlmt {

                           // ---------------------
                           // class Signaler_AutoId
                           // ---------------------

// CLASS METHOD
bsls::Types::Uint64 Signaler_AutoId::get() BSLS_CPP11_NOEXCEPT
{
    static bsls::AtomicUint64 s_id(0);
    return ++s_id;
}

                       // -----------------------------
                       // class Signaler_SlotController
                       // -----------------------------

// CREATORS
Signaler_SlotController::~Signaler_SlotController()
{
    // NOTHING
}

                          // ------------------------
                          // class SignalerConnection
                          // ------------------------

// PRIVATE CREATORS
SignalerConnection::SignalerConnection(
                         const bsl::shared_ptr<Slot>& slot) BSLS_CPP11_NOEXCEPT
: d_slot(slot)
{
    BSLS_ASSERT(slot);
}

// CREATORS
SignalerConnection::SignalerConnection() BSLS_CPP11_NOEXCEPT
: d_slot()
{
    // NOTHING
}

SignalerConnection::SignalerConnection(const SignalerConnection& original)
                                                            BSLS_CPP11_NOEXCEPT
: d_slot(original.d_slot)
{
    // NOTHING
}

SignalerConnection::SignalerConnection(
            bslmf::MovableRef<SignalerConnection> original) BSLS_CPP11_NOEXCEPT
: d_slot(bslmf::MovableRefUtil::move(
                               bslmf::MovableRefUtil::access(original).d_slot))
{
    // NOTHING
}

// MANIPULATORS
SignalerConnection&
SignalerConnection::operator=(const SignalerConnection& rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    d_slot = rhs.d_slot;
    return *this;
}

SignalerConnection&
SignalerConnection::operator=(bslmf::MovableRef<SignalerConnection> rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    d_slot = bslmf::MovableRefUtil::move(
                                    bslmf::MovableRefUtil::access(rhs).d_slot);
    return *this;
}

void SignalerConnection::disconnect() BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<Slot> slot = d_slot.lock();
    if (slot) {
        slot->disconnect();
    }
}

void SignalerConnection::disconnectAndWait() BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<Slot> slot = d_slot.lock();
    if (slot) {
        slot->disconnectAndWait();
    }
}

void SignalerConnection::swap(SignalerConnection& other) BSLS_CPP11_NOEXCEPT
{
    d_slot.swap(other.d_slot);
}

// ACCESSORS
bool SignalerConnection::isConnected() const BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<Slot> slot = d_slot.lock();
    return slot ? slot->isConnected() : false;
}

                         // ------------------------------
                         // class SignalerScopedConnection
                         // ------------------------------

// CREATORS
SignalerScopedConnection::SignalerScopedConnection() BSLS_CPP11_NOEXCEPT
: SignalerConnection()
{
    // NOTHING
}

SignalerScopedConnection::SignalerScopedConnection(
      bslmf::MovableRef<SignalerScopedConnection> original) BSLS_CPP11_NOEXCEPT
: SignalerConnection(bslmf::MovableRefUtil::move(
                                 static_cast<SignalerConnection&>(
                                     bslmf::MovableRefUtil::access(original))))
{
    // NOTHING
}

SignalerScopedConnection::SignalerScopedConnection(                           \
                      const SignalerConnection& connection) BSLS_CPP11_NOEXCEPT
: SignalerConnection(connection)
{
    // NOTHING
}

SignalerScopedConnection::SignalerScopedConnection(
          bslmf::MovableRef<SignalerConnection> connection) BSLS_CPP11_NOEXCEPT
: SignalerConnection(bslmf::MovableRefUtil::move(connection))
{
    // NOTHING
}

SignalerScopedConnection::~SignalerScopedConnection()
{
    disconnect();
}

// MANIPULATORS
SignalerScopedConnection&
SignalerScopedConnection::operator=(
           bslmf::MovableRef<SignalerScopedConnection> rhs) BSLS_CPP11_NOEXCEPT
{
    static_cast<SignalerConnection&>(*this) =
                                  bslmf::MovableRefUtil::move(
                                      static_cast<SignalerConnection&>(
                                          bslmf::MovableRefUtil::access(rhs)));

    return *this;
}

SignalerScopedConnection&
SignalerScopedConnection::operator=(const SignalerConnection& rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    static_cast<SignalerConnection&>(*this) = rhs;
    return *this;
}

SignalerScopedConnection&
SignalerScopedConnection::operator=(bslmf::MovableRef<SignalerConnection> rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    static_cast<SignalerConnection&>(*this) = bslmf::MovableRefUtil::move(rhs);
    return *this;
}

SignalerConnection
SignalerScopedConnection::release() BSLS_CPP11_NOEXCEPT
{
    SignalerConnection connection;
    connection.swap(static_cast<SignalerConnection&>(*this));

    return connection;
}

void SignalerScopedConnection::swap(SignalerScopedConnection& other)
                                                            BSLS_CPP11_NOEXCEPT
{
    SignalerConnection::swap(static_cast<SignalerConnection&>(other));
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2018
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
