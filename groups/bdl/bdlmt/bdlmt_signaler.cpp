// bdlmt_signaler.cpp                                                 -*-C++-*-
#include <bdlmt_signaler.h>

#include <bsls_atomicoperations.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_multiprioritythreadpool_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace bdlmt {

                           // -----------------------
                           // class Signaler_SlotBase
                           // -----------------------

// PRIVATE MANIPULATOR
void Signaler_SlotBase::disconnectAndWait() BSLS_CPP11_NOEXCEPT
{
    // Disconnect the slot.

    disconnect();

    // Synchronize with the call operator.

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> lock(&d_callMutex); // LOCK
}

// CLASS METHOD
bsls::Types::Uint64 Signaler_SlotBase::getId() BSLS_CPP11_NOEXCEPT
{
    typedef BloombergLP::bsls::AtomicOperations AtomicOps;

    static AtomicOps::AtomicTypes::Uint64 autoId = { 0 };

    return AtomicOps::incrementUint64NvAcqRel(&autoId);
}


// CREATOR
Signaler_SlotBase::Signaler_SlotBase(SlotKey slotKey)
: d_callMutex()
, d_slotKey(slotKey)
, d_isConnected(true)
{}

// MANIPULATORS
void Signaler_SlotBase::notifyDisconnected() BSLS_CPP11_NOEXCEPT
{
    d_isConnected = false;
}

// ACCESSORS
bool Signaler_SlotBase::isConnected() const BSLS_CPP11_NOEXCEPT
{
    return d_isConnected;
}


                          // ------------------------
                          // class SignalerConnection
                          // ------------------------

// PRIVATE CREATORS
SignalerConnection::SignalerConnection(
              const bsl::shared_ptr<SlotBase>& slotBasePtr) BSLS_CPP11_NOEXCEPT
: d_slotBasePtr(slotBasePtr)
{
    BSLS_ASSERT(slotBasePtr);
}

// CREATORS
SignalerConnection::SignalerConnection() BSLS_CPP11_NOEXCEPT
: d_slotBasePtr()
{
    // NOTHING
}

SignalerConnection::SignalerConnection(const SignalerConnection& original)
                                                            BSLS_CPP11_NOEXCEPT
: d_slotBasePtr(original.d_slotBasePtr)
{
    // NOTHING
}

SignalerConnection::SignalerConnection(
            bslmf::MovableRef<SignalerConnection> original) BSLS_CPP11_NOEXCEPT
: d_slotBasePtr(bslmf::MovableRefUtil::move(
                        bslmf::MovableRefUtil::access(original).d_slotBasePtr))
{
    // NOTHING
}

// MANIPULATORS
SignalerConnection&
SignalerConnection::operator=(const SignalerConnection& rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    d_slotBasePtr = rhs.d_slotBasePtr;
    return *this;
}

SignalerConnection&
SignalerConnection::operator=(bslmf::MovableRef<SignalerConnection> rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    SignalerConnection& rhsRef = rhs;

    d_slotBasePtr = bslmf::MovableRefUtil::move(rhsRef.d_slotBasePtr);

    return *this;
}

void SignalerConnection::disconnect() BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotBase> slotBasePtr = d_slotBasePtr.lock();
    if (slotBasePtr) {
        slotBasePtr->disconnect();
    }
}

void SignalerConnection::disconnectAndWait() BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotBase> slotBasePtr = d_slotBasePtr.lock();
    if (slotBasePtr) {
        slotBasePtr->disconnectAndWait();
    }
}

void SignalerConnection::swap(SignalerConnection& other) BSLS_CPP11_NOEXCEPT
{
    d_slotBasePtr.swap(other.d_slotBasePtr);
}

// ACCESSORS
bool SignalerConnection::isConnected() const BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotBase> slotBasePtr = d_slotBasePtr.lock();
    return slotBasePtr ? slotBasePtr->isConnected() : false;
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
    SignalerConnection& rhsBaseRef = rhs;
    SignalerConnection& lhsBaseRef = *this;

    disconnect();

    lhsBaseRef = bslmf::MovableRefUtil::move(rhsBaseRef);
    return *this;
}

SignalerScopedConnection&
SignalerScopedConnection::operator=(const SignalerConnection& rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    SignalerConnection& lhsBaseRef = *this;

    disconnect();

    lhsBaseRef = rhs;
    return *this;
}

SignalerScopedConnection&
SignalerScopedConnection::operator=(bslmf::MovableRef<SignalerConnection> rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    SignalerConnection& lhsBaseRef = *this;

    lhsBaseRef = bslmf::MovableRefUtil::move(rhs);
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
    SignalerConnection& otherBaseRef = other;

    SignalerConnection::swap(otherBaseRef);
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
