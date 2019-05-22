// bdlmt_signaler.cpp                                                 -*-C++-*-
#include <bdlmt_signaler.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmt_signaler_cpp,"$Id$ $CSID$")

//-----------------------------------------------------------------------------
//                           IMPLEMENTATION NOTES
//
// The behavior is undefined if any method of 'Signaler_SlotNode',
// 'Signaler_SlotNode_Base', or 'Signaler_Node' is called by a thread that does
// not have a shared pointer to the object called.
//-----------------------------------------------------------------------------

namespace BloombergLP {
namespace bdlmt {

                         // ----------------------------
                         // class Signaler_SlotNode_Base
                         // ----------------------------

// CREATORS
Signaler_SlotNode_Base::~Signaler_SlotNode_Base()
{
    // NOTHING.
}

                          // ------------------------
                          // class SignalerConnection
                          // ------------------------

// PRIVATE CREATORS
SignalerConnection::SignalerConnection(
     const bsl::shared_ptr<SlotNode_Base>& slotNodeBasePtr) BSLS_CPP11_NOEXCEPT
: d_slotNodeBasePtr(slotNodeBasePtr)
{
    BSLS_ASSERT(slotNodeBasePtr);
}

// CREATORS
SignalerConnection::SignalerConnection() BSLS_CPP11_NOEXCEPT
: d_slotNodeBasePtr()
{
    // NOTHING
}

SignalerConnection::SignalerConnection(const SignalerConnection& original)
                                                            BSLS_CPP11_NOEXCEPT
: d_slotNodeBasePtr(original.d_slotNodeBasePtr)
{
    // NOTHING
}

SignalerConnection::SignalerConnection(
            bslmf::MovableRef<SignalerConnection> original) BSLS_CPP11_NOEXCEPT
: d_slotNodeBasePtr(bslmf::MovableRefUtil::move(
                    bslmf::MovableRefUtil::access(original).d_slotNodeBasePtr))
{
    // NOTHING
}

// MANIPULATORS
SignalerConnection&
SignalerConnection::operator=(const SignalerConnection& rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    d_slotNodeBasePtr = rhs.d_slotNodeBasePtr;
    return *this;
}

SignalerConnection&
SignalerConnection::operator=(bslmf::MovableRef<SignalerConnection> rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    SignalerConnection& rhsRef = rhs;

    d_slotNodeBasePtr = bslmf::MovableRefUtil::move(rhsRef.d_slotNodeBasePtr);

    return *this;
}

void SignalerConnection::disconnect() BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotNode_Base> slotNodeBasePtr = d_slotNodeBasePtr.lock();
    if (slotNodeBasePtr) {
        slotNodeBasePtr->disconnect();
    }
}

void SignalerConnection::disconnectAndWait() BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotNode_Base> slotNodeBasePtr = d_slotNodeBasePtr.lock();
    if (slotNodeBasePtr) {
        slotNodeBasePtr->disconnectAndWait();
    }
}

SignalerConnection SignalerConnection::release() BSLS_CPP11_NOEXCEPT
{
    SignalerConnection ret;

    this->swap(ret);

    return ret;
}

void SignalerConnection::swap(SignalerConnection& other) BSLS_CPP11_NOEXCEPT
{
    d_slotNodeBasePtr.swap(other.d_slotNodeBasePtr);
}

// ACCESSORS
bool SignalerConnection::isConnected() const BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotNode_Base> slotNodeBasePtr = d_slotNodeBasePtr.lock();
    return slotNodeBasePtr ? slotNodeBasePtr->isConnected() : false;
}

// FREE OPERATORS
bool operator==(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !(lhs < rhs) && !(rhs < lhs);
}

bool operator!=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return lhs < rhs || rhs < lhs;
}

bool operator>(const SignalerConnection& lhs,
               const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return rhs < lhs;
}

bool operator<=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !(rhs < lhs);
}

bool operator>=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !(lhs < rhs);
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

SignalerScopedConnection::SignalerScopedConnection(
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
    SignalerScopedConnection rhsMoved(bslmf::MovableRefUtil::move(rhs));

    this->swap(rhsMoved);

    // Destruction of 'rhsMoved' will disconnect the slot, if any, which
    // '*this' was previously referencing.

    return *this;
}

SignalerScopedConnection&
SignalerScopedConnection::operator=(const SignalerConnection& rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    SignalerScopedConnection rhsDerived(rhs);

    this->swap(rhsDerived);

    // Destruction of 'rhsDerived' will disconnect the slot, if any, which
    // '*this' was previously referencing.

    return *this;
}

SignalerScopedConnection&
SignalerScopedConnection::operator=(bslmf::MovableRef<SignalerConnection> rhs)
                                                            BSLS_CPP11_NOEXCEPT
{
    SignalerScopedConnection rhsDerived(bslmf::MovableRefUtil::move(rhs));

    this->swap(rhsDerived);

    // Destruction of 'rhsDerived' will disonnect the slot, if any, to which
    // '*this' was previously referring.

    return *this;
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
//      Copyright (C) Bloomberg L.P., 2019
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
