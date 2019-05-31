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
: d_slotNodeBasePtr()
{
    SignalerConnection& local = original;

    d_slotNodeBasePtr.swap(local.d_slotNodeBasePtr);
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

void SignalerConnection::reset() BSLS_CPP11_NOEXCEPT
{
    d_slotNodeBasePtr.reset();
}

// ACCESSORS
void SignalerConnection::disconnect(bool wait) const BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotNode_Base> slotNodeBasePtr = d_slotNodeBasePtr.lock();
    if (slotNodeBasePtr) {
        slotNodeBasePtr->disconnect(wait);
    }
}

bool SignalerConnection::isConnected() const BSLS_CPP11_NOEXCEPT
{
    bsl::shared_ptr<SlotNode_Base> slotNodeBasePtr = d_slotNodeBasePtr.lock();
    return slotNodeBasePtr ? slotNodeBasePtr->isConnected() : false;
}

// FREE OPERATORS
bool operator!=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return !(lhs == rhs);
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

                        // -----------------------
                        // SignalerConnectionGuard
                        // -----------------------

// CREATORS
SignalerConnectionGuard::SignalerConnectionGuard(bool waitOnDisconnect)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_connection()
, d_waitOnDisconnect(waitOnDisconnect)
{
    // NOTHING
}

SignalerConnectionGuard::SignalerConnectionGuard(
              const SignalerConnection& connection,
              bool                      waitOnDisconnect) BSLS_KEYWORD_NOEXCEPT
: d_connection(connection)
, d_waitOnDisconnect(waitOnDisconnect)
{
    // NOTHING
}

SignalerConnectionGuard::SignalerConnectionGuard(
                        bslmf::MovableRef<SignalerConnection> connection,
                        bool                                  waitOnDisconnect)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_connection()
, d_waitOnDisconnect(waitOnDisconnect)
{
    bdlmt::SignalerConnection& localConnection = connection;
    d_connection.swap(localConnection);
}

SignalerConnectionGuard::SignalerConnectionGuard(
                           bslmf::MovableRef<SignalerConnectionGuard> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_connection()
{
    bdlmt::SignalerConnectionGuard& localOriginal = original;
    d_connection.swap(localOriginal.d_connection);
    d_waitOnDisconnect = localOriginal.d_waitOnDisconnect;
}

SignalerConnectionGuard::SignalerConnectionGuard(
                   bslmf::MovableRef<SignalerConnectionGuard> original,
                   bool                                       waitOnDisconnect)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_connection()
, d_waitOnDisconnect(waitOnDisconnect)
{
    bdlmt::SignalerConnectionGuard& localOriginal = original;
    d_connection.swap(localOriginal.d_connection);
}

SignalerConnectionGuard::~SignalerConnectionGuard()
{
    d_connection.disconnect(d_waitOnDisconnect);
}

// MANIPULATORS
SignalerConnectionGuard& SignalerConnectionGuard::operator=(
                                bslmf::MovableRef<SignalerConnectionGuard> rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_connection.disconnect(d_waitOnDisconnect);
    d_connection.reset();

    SignalerConnectionGuard& localRhs = rhs;
    d_connection.swap(localRhs.d_connection);
    d_waitOnDisconnect = localRhs.d_waitOnDisconnect;

    return *this;
}

SignalerConnectionGuard& SignalerConnectionGuard::operator=(
                           const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT
{
    d_connection.disconnect(d_waitOnDisconnect);
    d_connection = rhs;

    return *this;
}

SignalerConnectionGuard& SignalerConnectionGuard::operator=(
               bslmf::MovableRef<SignalerConnection> rhs) BSLS_KEYWORD_NOEXCEPT
{
    SignalerConnection& localRhs = rhs;
    d_connection.disconnect(d_waitOnDisconnect);
    d_connection.reset();
    d_connection.swap(localRhs);

    return *this;
}

SignalerConnection SignalerConnectionGuard::release() BSLS_KEYWORD_NOEXCEPT
{
    SignalerConnection ret;

    d_connection.swap(ret);

    return ret;
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
