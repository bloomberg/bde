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

#include <bsl_algorithm.h>    // swap

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
    // This function is called by 'Signaler::connect', which requires its not
    // throwing for it to provide the strong exception guarantee.

    BSLS_ASSERT(slotNodeBasePtr);
}

// CREATORS
SignalerConnection::SignalerConnection()
: d_slotNodeBasePtr()
{
    // NOTHING
}

SignalerConnection::SignalerConnection(const SignalerConnection& original)
                                                            BSLS_CPP11_NOEXCEPT
: d_slotNodeBasePtr(original.d_slotNodeBasePtr)
{
    // Callers to 'Signaler::connect' and 'SignalerConnectionGuard::release'
    // use this function to copy the return value, so this must not throw for
    // the whole operation to provide the strong exception guarantee.
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

void SignalerConnection::reset() BSLS_CPP11_NOEXCEPT
{
    d_slotNodeBasePtr.reset();
}

// ACCESSORS
void SignalerConnection::disconnect() const BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotNode_Base> slotNodeBasePtr = d_slotNodeBasePtr.lock();
    if (slotNodeBasePtr) {
        slotNodeBasePtr->disconnect(false);
    }
}

void SignalerConnection::disconnectAndWait() const BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotNode_Base> slotNodeBasePtr = d_slotNodeBasePtr.lock();
    if (slotNodeBasePtr) {
        slotNodeBasePtr->disconnect(true);
    }
}

bool SignalerConnection::isConnected() const
{
    bsl::shared_ptr<SlotNode_Base> slotNodeBasePtr = d_slotNodeBasePtr.lock();
    return slotNodeBasePtr ? slotNodeBasePtr->isConnected() : false;
}

// FREE OPERATORS
bool operator!=(const SignalerConnection& lhs, const SignalerConnection& rhs)
{
    return !(lhs == rhs);
}

bool operator>( const SignalerConnection& lhs, const SignalerConnection& rhs)
{
    return rhs < lhs;
}

bool operator<=(const SignalerConnection& lhs, const SignalerConnection& rhs)
{
    return !(rhs < lhs);
}

bool operator>=(const SignalerConnection& lhs, const SignalerConnection& rhs)
{
    return !(lhs < rhs);
}

                        // -----------------------------
                        // class SignalerConnectionGuard
                        // -----------------------------

// CREATORS
SignalerConnectionGuard::SignalerConnectionGuard(bool waitOnDisconnect)
: d_connection()
, d_waitOnDisconnect(waitOnDisconnect)
{
    // NOTHING
}

SignalerConnectionGuard::SignalerConnectionGuard(
                                    const SignalerConnection& connection,
                                    bool                      waitOnDisconnect)
: d_connection(connection)
, d_waitOnDisconnect(waitOnDisconnect)
{
    // NOTHING
}

SignalerConnectionGuard::SignalerConnectionGuard(
                        bslmf::MovableRef<SignalerConnection> connection,
                        bool                                  waitOnDisconnect)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_connection(bslmf::MovableRefUtil::move(connection))
, d_waitOnDisconnect(waitOnDisconnect)
{
    // NOTHING
}

SignalerConnectionGuard::SignalerConnectionGuard(
                           bslmf::MovableRef<SignalerConnectionGuard> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_connection(bslmf::MovableRefUtil::move(
                         bslmf::MovableRefUtil::access(original).d_connection))
, d_waitOnDisconnect(
                    bslmf::MovableRefUtil::access(original).d_waitOnDisconnect)
{
    // NOTHING
}

SignalerConnectionGuard::SignalerConnectionGuard(
                   bslmf::MovableRef<SignalerConnectionGuard> original,
                   bool                                       waitOnDisconnect)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_connection(bslmf::MovableRefUtil::move(
                         bslmf::MovableRefUtil::access(original).d_connection))
, d_waitOnDisconnect(waitOnDisconnect)
{
    // NOTHING
}

SignalerConnectionGuard::~SignalerConnectionGuard()
{
    d_waitOnDisconnect ? d_connection.disconnectAndWait()
                       : d_connection.disconnect();
}

// MANIPULATORS
SignalerConnectionGuard& SignalerConnectionGuard::operator=(
                                bslmf::MovableRef<SignalerConnectionGuard> rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_waitOnDisconnect ? d_connection.disconnectAndWait()
                       : d_connection.disconnect();

    SignalerConnectionGuard& rhsRef = rhs;
    d_connection       = bslmf::MovableRefUtil::move(rhsRef.d_connection);
    d_waitOnDisconnect = rhsRef.d_waitOnDisconnect;

    return *this;
}

SignalerConnection SignalerConnectionGuard::release() BSLS_KEYWORD_NOEXCEPT
{
    SignalerConnection ret;

    d_connection.swap(ret);

    return ret;
}

void SignalerConnectionGuard::swap(SignalerConnectionGuard& other)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    using bsl::swap;
    swap(d_connection,       other.d_connection);
    swap(d_waitOnDisconnect, other.d_waitOnDisconnect);
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
