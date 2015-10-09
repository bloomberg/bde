// btlso_socketoptions.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <bsls_ident.h>
BSLS_IDENT_RCSID(bteso_socketoptions_t_cpp,"$Id$ $CSID$")

#include <btlso_socketoptions.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a single, unconstrained (value-semantic)
// attribute class.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] btlso::SocketOptions();
// [ 2] btlso::SocketOptions(const btlso::SocketOptions& original);
// [ 2] ~btlso::SocketOptions();
//
// MANIPULATORS
// [ 4] SocketOptions& operator=(const btlso::SocketOptions& rhs);
// [ 4] void reset();
// [ 3] void setDebugFlag(bool value);
// [ 3] void setAllowBroadcasting(bool value);
// [ 3] void setReuseAddress(bool value);
// [ 3] void setKeepAlive(bool value);
// [ 3] void setBypassNormalRouting(bool value);
// [ 3] void setLinger(const LingerOptions& value);
// [ 3] void setLeaveOutOfBandDataInline(bool value);
// [ 3] void setSendBufferSize(int value);
// [ 3] void setReceiveBufferSize(int value);
// [ 3] void setMinimumSendBufferSize(int value);
// [ 3] void setMinimumReceiveBufferSize(int value);
// [ 3] void setSendTimeout(int value);
// [ 3] void setReceiveTimeout(int value);
// [ 3] void setTcpNoDelay(bool value);
//
// ACCESSORS
// [ 3] const bdlb::NullableValue<bool>& debugFlag() const;
// [ 3] const bdlb::NullableValue<bool>& allowBroadcasting() const;
// [ 3] const bdlb::NullableValue<bool>& reuseAddress() const;
// [ 3] const bdlb::NullableValue<bool>& keepAlive() const;
// [ 3] const bdlb::NullableValue<bool>& bypassNormalRouting() const;
// [ 3] const bdlb::NullableValue<LingerOptions>& linger() const;
// [ 3] const bdlb::NullableValue<bool>& leaveOutOfBandDataInline() const;
// [ 3] const bdlb::NullableValue<int>& sendBufferSize() const;
// [ 3] const bdlb::NullableValue<int>& receiveBufferSize() const;
// [ 3] const bdlb::NullableValue<int>& minimumSendBufferSize() const;
// [ 3] const bdlb::NullableValue<int>& minimumReceiveBufferSize() const;
// [ 3] const bdlb::NullableValue<int>& sendTimeout() const;
// [ 3] const bdlb::NullableValue<int>& receiveTimeout() const;
// [ 3] const bdlb::NullableValue<bool>& tcpNoDelay() const;
// [  ] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 4] bool operator==(const btlso::SocketOptions& lhs, rhs);
// [ 4] bool operator!=(const btlso::SocketOptions& lhs, rhs);
// [  ] operator<<(ostream& s, const btlso::SocketOptions& d);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() bsl::cout << '\t' << bsl::flush; // Print tab w/o newline.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

typedef btlso::SocketOptions Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = (argc > 2);
    veryVerbose = (argc > 3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "USAGE EXAMPLE TEST" << bsl::endl
                               << "==================" << bsl::endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Setting the socket option on a socket
///- - - - - - - - - - - - - - - - - - - - - - - -
// 'btlso::SocketOptions' can be used to specify whether local addresses should
// be reused.  The following snippets of code illustrate how to set the
// 'BTESO_REUSEADDRESS' flag on a socket.  Note that we assume that a socket of
// type 'SOCK_STREAM' has already been created and that it can be accessed
// using 'socketHandle':
//..
    bool                 reuseLocalAddr = true;
    btlso::SocketOptions options;
    options.setReuseAddress(reuseLocalAddr);
    ASSERT(reuseLocalAddr == options.reuseAddress().value());
//
    // Set 'options' on 'socketHandle'
//
    // . . .
//..
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:11 The equality operator's signature and return type are standard.
        //:
        //:12 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-9..12)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that for each salient attribute, there exists a
        //:   pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute.
        //:
        //: 3 For each row 'R1' in the table of P-3:  (C-1..8)
        //:
        //:   1 Create a single object, and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-4..5)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X' having the value 'R1'.
        //:
        //:     3 Create an object 'Y' having the value 'R2'.
        //:
        //:     4 Verify the commutativity property and expected return value
        //:       for both '==' and '!='.  (C-1..3, 6..8)
        //
        // Testing:
        //   bool operator==(const btlso::SocketOptions& lhs, rhs);
        //   bool operator!=(const btlso::SocketOptions& lhs, rhs);
        //   SocketOptions& operator=(const btlso::SocketOptions& rhs);
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace btlso;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        // 'D' values: These are the default-constructed values.

        const int            D1   = 0;
        const bool           D2   = false;
        btlso::LingerOptions D3(D1, D2);

        // 'A' values.

        const int            A1   = 1;
        const bool           A2   = true;
        btlso::LingerOptions A3(A1, A2);

        // 'B' values.

        const int            B1 = INT_MAX;
        const bool           B2 = false;
        btlso::LingerOptions B3(B1, B2);

        {
            Obj mD;  const Obj& D = mD;

            Obj mA;  const Obj& A = mA;
            mA.setDebugFlag(A2);
            mA.setAllowBroadcasting(A2);
            mA.setReuseAddress(A2);
            mA.setKeepAlive(A2);
            mA.setBypassNormalRouting(A2);
            mA.setLeaveOutOfBandDataInline(A2);
            mA.setSendBufferSize(A1);
            mA.setReceiveBufferSize(A1);
            mA.setMinimumSendBufferSize(A1);
            mA.setMinimumReceiveBufferSize(A1);
            mA.setSendTimeout(A1);
            mA.setReceiveTimeout(A1);
            mA.setTcpNoDelay(A2);
            mA.setLinger(A3);

            Obj mB;  const Obj& B = mB;
            mB.setDebugFlag(B2);
            mB.setAllowBroadcasting(B2);
            mB.setReuseAddress(B2);
            mB.setKeepAlive(B2);
            mB.setBypassNormalRouting(B2);
            mB.setLeaveOutOfBandDataInline(B2);
            mB.setSendBufferSize(B1);
            mB.setReceiveBufferSize(B1);
            mB.setMinimumSendBufferSize(B1);
            mB.setMinimumReceiveBufferSize(B1);
            mB.setSendTimeout(B1);
            mB.setReceiveTimeout(B1);
            mB.setTcpNoDelay(B2);
            mB.setLinger(B3);

            LOOP2_ASSERT(D, A, D != A);
            LOOP2_ASSERT(D, B, D != B);
            LOOP2_ASSERT(A, B, A != B);

            {
                Obj mX = D;  const Obj& X = mX;

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setDebugFlag(A2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setAllowBroadcasting(A2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setReuseAddress(A2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setKeepAlive(A2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setBypassNormalRouting(A2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setLeaveOutOfBandDataInline(A2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSendBufferSize(A1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setReceiveBufferSize(A1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setMinimumSendBufferSize(A1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setMinimumReceiveBufferSize(A1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSendTimeout(A1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setReceiveTimeout(A1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setTcpNoDelay(A2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setLinger(A3);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A == X);
                LOOP2_ASSERT(B, X, B != X);

                mX.reset();

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);
            }

            {
                Obj mX = D;  const Obj& X = mX;

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setDebugFlag(B2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setAllowBroadcasting(B2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setReuseAddress(B2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setKeepAlive(B2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setBypassNormalRouting(B2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setLeaveOutOfBandDataInline(B2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSendBufferSize(B1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setReceiveBufferSize(B1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setMinimumSendBufferSize(B1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setMinimumReceiveBufferSize(B1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setSendTimeout(B1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setReceiveTimeout(B1);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setTcpNoDelay(B2);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);

                mX.setLinger(B3);

                LOOP2_ASSERT(D, X, D != X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B == X);

                mX.reset();

                LOOP2_ASSERT(D, X, D == X);
                LOOP2_ASSERT(A, X, A != X);
                LOOP2_ASSERT(B, X, B != X);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // MANIPULATORS & ACCESSORS
        //   Ensure that we can use the manipulators to put that object into
        //   any state relevant for testing and use the accessors to confirm
        //   the value set.
        //
        // Concerns:
        //: 1 Each attribute is modifiable independently.
        //:
        //: 2 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //: 3 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //
        // Plan:
        //: 1 Use the default constructor to create an object 'X'.
        //:
        //: 2 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's
        //:   value, passing a 'const' argument representing each of the
        //:   three test values, in turn (see P-1), first to 'Ai', then to
        //:   'Bi', and finally back to 'Di'.  After each transition, use the
        //:   (as yet unproven) basic accessors to verify that only the
        //:   intended attribute value changed.  (C-2, 4)
        //:
        //: 3 Corroborate that attributes are modifiable independently by
        //:   first setting all of the attributes to their 'A' values.  Then
        //:   incrementally set each attribute to its 'B' value and verify
        //:   after each manipulation that only that attribute's value
        //:   changed.  (C-3)
        //
        // Testing:
        //   void setDebugFlag(bool value);
        //   void setAllowBroadcasting(bool value);
        //   void setReuseAddress(bool value);
        //   void setKeepAlive(bool value);
        //   void setBypassNormalRouting(bool value);
        //   void setLinger(const LingerOptions& value);
        //   void setLeaveOutOfBandDataInline(bool value);
        //   void setSendBufferSize(int value);
        //   void setReceiveBufferSize(int value);
        //   void setMinimumSendBufferSize(int value);
        //   void setMinimumReceiveBufferSize(int value);
        //   void setSendTimeout(int value);
        //   void setReceiveTimeout(int value);
        //   void setTcpNoDelay(bool value);
        //   const bdlb::NullableValue<bool>& debugFlag() const;
        //   const bdlb::NullableValue<bool>& allowBroadcasting() const;
        //   const bdlb::NullableValue<bool>& reuseAddress() const;
        //   const bdlb::NullableValue<bool>& keepAlive() const;
        //   const bdlb::NullableValue<bool>& bypassNormalRouting() const;
        //   const bdlb::NullableValue<LingerOptions>& linger() const;
        //   const bdlb::NullableValue<bool>& leaveOutOfBandDataInline() const;
        //   const bdlb::NullableValue<int>& sendBufferSize() const;
        //   const bdlb::NullableValue<int>& receiveBufferSize() const;
        //   const bdlb::NullableValue<int>& minimumSendBufferSize() const;
        //   const bdlb::NullableValue<int>& minimumReceiveBufferSize() const;
        //   const bdlb::NullableValue<int>& sendTimeout() const;
        //   const bdlb::NullableValue<int>& receiveTimeout() const;
        //   const bdlb::NullableValue<bool>& tcpNoDelay() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MANIPULATORS & ACCESSORS" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const int            D1   = 0;
        const bool           D2   = false;
        btlso::LingerOptions D3(D1, D2);

        // 'A' values.

        const int            A1   = 1;
        const bool           A2   = true;
        btlso::LingerOptions A3(A1, A2);

        // 'B' values.

        const int            B1 = INT_MAX;
        const bool           B2 = false;
        btlso::LingerOptions B3(B1, B2);

        {
            if (verbose) cout <<
                     "Create an object using the default constructor." << endl;

            Obj mX;  const Obj& X = mX;

            if (verbose) cout << "Verify the object's attribute values."
                              << endl;

            // ---------------------------------------------
            // Verify the object's default attribute values.
            // ---------------------------------------------

            ASSERT(X.debugFlag().isNull());
            ASSERT(X.allowBroadcasting().isNull());
            ASSERT(X.reuseAddress().isNull());
            ASSERT(X.keepAlive().isNull());
            ASSERT(X.bypassNormalRouting().isNull());
            ASSERT(X.linger().isNull());
            ASSERT(X.leaveOutOfBandDataInline().isNull());
            ASSERT(X.sendBufferSize().isNull());
            ASSERT(X.receiveBufferSize().isNull());
            ASSERT(X.minimumSendBufferSize().isNull());
            ASSERT(X.minimumReceiveBufferSize().isNull());
            ASSERT(X.sendTimeout().isNull());
            ASSERT(X.receiveTimeout().isNull());
            ASSERT(X.tcpNoDelay().isNull());
        }

        if (verbose) cout <<
               "Verify that each attribute is independently settable." << endl;

        // -----------
        // 'debugFlag'
        // -----------
        {
            Obj mX;  const Obj& X = mX;

            mX.setDebugFlag(D2);
            ASSERT(D2 == X.debugFlag().value());

            mX.setDebugFlag(A2);
            ASSERT(A2 == X.debugFlag().value());

            mX.setDebugFlag(B2);
            ASSERT(B2 == X.debugFlag().value());

            mX.setDebugFlag(D2);
            ASSERT(D2 == X.debugFlag().value());
        }

        // -------------------
        // 'allowBroadcasting'
        // -------------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setAllowBroadcasting(D2);
            ASSERT(D2 == X.allowBroadcasting().value());

            mX.setAllowBroadcasting(A2);
            ASSERT(A2 == X.allowBroadcasting().value());

            mX.setAllowBroadcasting(B2);
            ASSERT(B2 == X.allowBroadcasting().value());

            mX.setAllowBroadcasting(D2);
            ASSERT(D2 == X.allowBroadcasting().value());
        }

        // --------------
        // 'reuseAddress'
        // --------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setReuseAddress(D2);
            ASSERT(D2 == X.reuseAddress().value());

            mX.setReuseAddress(A2);
            ASSERT(A2 == X.reuseAddress().value());

            mX.setReuseAddress(B2);
            ASSERT(B2 == X.reuseAddress().value());

            mX.setReuseAddress(D2);
            ASSERT(D2 == X.reuseAddress().value());
        }

        // -----------
        // 'keepAlive'
        // -----------
        {
            Obj mX;  const Obj& X = mX;

            mX.setKeepAlive(D2);
            ASSERT(D2 == X.keepAlive().value());

            mX.setKeepAlive(A2);
            ASSERT(A2 == X.keepAlive().value());

            mX.setKeepAlive(B2);
            ASSERT(B2 == X.keepAlive().value());

            mX.setKeepAlive(D2);
            ASSERT(D2 == X.keepAlive().value());
        }

        // ---------------------
        // 'bypassNormalRouting'
        // ---------------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setBypassNormalRouting(D2);
            ASSERT(D2 == X.bypassNormalRouting().value());

            mX.setBypassNormalRouting(A2);
            ASSERT(A2 == X.bypassNormalRouting().value());

            mX.setBypassNormalRouting(B2);
            ASSERT(B2 == X.bypassNormalRouting().value());

            mX.setBypassNormalRouting(D2);
            ASSERT(D2 == X.bypassNormalRouting().value());
        }

        // --------------------------
        // 'leaveOutOfBandDataInline'
        // --------------------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setLeaveOutOfBandDataInline(D2);
            ASSERT(D2 == X.leaveOutOfBandDataInline().value());

            mX.setLeaveOutOfBandDataInline(A2);
            ASSERT(A2 == X.leaveOutOfBandDataInline().value());

            mX.setLeaveOutOfBandDataInline(B2);
            ASSERT(B2 == X.leaveOutOfBandDataInline().value());

            mX.setLeaveOutOfBandDataInline(D2);
            ASSERT(D2 == X.leaveOutOfBandDataInline().value());
        }

        // ----------------
        // 'sendBufferSize'
        // ----------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setSendBufferSize(D1);
            ASSERT(D1 == X.sendBufferSize().value());

            mX.setSendBufferSize(A1);
            ASSERT(A1 == X.sendBufferSize().value());

            mX.setSendBufferSize(B1);
            ASSERT(B1 == X.sendBufferSize().value());

            mX.setSendBufferSize(D1);
            ASSERT(D1 == X.sendBufferSize().value());
        }

        // -------------------
        // 'receiveBufferSize'
        // -------------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setReceiveBufferSize(D1);
            ASSERT(D1 == X.receiveBufferSize().value());

            mX.setReceiveBufferSize(A1);
            ASSERT(A1 == X.receiveBufferSize().value());

            mX.setReceiveBufferSize(B1);
            ASSERT(B1 == X.receiveBufferSize().value());

            mX.setReceiveBufferSize(D1);
            ASSERT(D1 == X.receiveBufferSize().value());
        }

        // -----------------------
        // 'minimumSendBufferSize'
        // -----------------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setMinimumSendBufferSize(D1);
            ASSERT(D1 == X.minimumSendBufferSize().value());

            mX.setMinimumSendBufferSize(A1);
            ASSERT(A1 == X.minimumSendBufferSize().value());

            mX.setMinimumSendBufferSize(B1);
            ASSERT(B1 == X.minimumSendBufferSize().value());

            mX.setMinimumSendBufferSize(D1);
            ASSERT(D1 == X.minimumSendBufferSize().value());
        }

        // --------------------------
        // 'minimumReceiveBufferSize'
        // --------------------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setMinimumReceiveBufferSize(D1);
            ASSERT(D1 == X.minimumReceiveBufferSize().value());

            mX.setMinimumReceiveBufferSize(A1);
            ASSERT(A1 == X.minimumReceiveBufferSize().value());

            mX.setMinimumReceiveBufferSize(B1);
            ASSERT(B1 == X.minimumReceiveBufferSize().value());

            mX.setMinimumReceiveBufferSize(D1);
            ASSERT(D1 == X.minimumReceiveBufferSize().value());
        }

        // -------------
        // 'sendTimeout'
        // -------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setSendTimeout(D1);
            ASSERT(D1 == X.sendTimeout().value());

            mX.setSendTimeout(A1);
            ASSERT(A1 == X.sendTimeout().value());

            mX.setSendTimeout(B1);
            ASSERT(B1 == X.sendTimeout().value());

            mX.setSendTimeout(D1);
            ASSERT(D1 == X.sendTimeout().value());
        }

        // ----------------
        // 'receiveTimeout'
        // ----------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setReceiveTimeout(D1);
            ASSERT(D1 == X.receiveTimeout().value());

            mX.setReceiveTimeout(A1);
            ASSERT(A1 == X.receiveTimeout().value());

            mX.setReceiveTimeout(B1);
            ASSERT(B1 == X.receiveTimeout().value());

            mX.setReceiveTimeout(D1);
            ASSERT(D1 == X.receiveTimeout().value());
        }

        // ------------
        // 'tcpNoDelay'
        // ------------
        {
            Obj mX;  const Obj& X = mX;

            mX.setTcpNoDelay(D2);
            ASSERT(D2 == X.tcpNoDelay().value());

            mX.setTcpNoDelay(A2);
            ASSERT(A2 == X.tcpNoDelay().value());

            mX.setTcpNoDelay(B2);
            ASSERT(B2 == X.tcpNoDelay().value());

            mX.setTcpNoDelay(D2);
            ASSERT(D2 == X.tcpNoDelay().value());
        }

        // --------
        // 'linger'
        // --------
        {
            Obj mX;  const Obj& X = mX;

            mX.setLinger(D3);
            ASSERT(D3 == X.linger().value());

            mX.setLinger(A3);
            ASSERT(A3 == X.linger().value());

            mX.setLinger(B3);
            ASSERT(B3 == X.linger().value());

            mX.setLinger(D3);
            ASSERT(D3 == X.linger().value());
        }

        if (verbose) cout << "Corroborate attribute independence." << endl;
        {
            // ---------------------------------------
            // Set all attributes to their 'A' values.
            // ---------------------------------------

            Obj mX;  const Obj& X = mX;

            mX.setDebugFlag(A2);
            mX.setAllowBroadcasting(A2);
            mX.setReuseAddress(A2);
            mX.setKeepAlive(A2);
            mX.setBypassNormalRouting(A2);
            mX.setLeaveOutOfBandDataInline(A2);
            mX.setSendBufferSize(A1);
            mX.setReceiveBufferSize(A1);
            mX.setMinimumSendBufferSize(A1);
            mX.setMinimumReceiveBufferSize(A1);
            mX.setSendTimeout(A1);
            mX.setReceiveTimeout(A1);
            mX.setTcpNoDelay(A2);
            mX.setLinger(A3);

            ASSERT(A2 == X.debugFlag());
            ASSERT(A2 == X.allowBroadcasting());
            ASSERT(A2 == X.reuseAddress());
            ASSERT(A2 == X.keepAlive());
            ASSERT(A2 == X.bypassNormalRouting());
            ASSERT(A2 == X.leaveOutOfBandDataInline());
            ASSERT(A1 == X.sendBufferSize());
            ASSERT(A1 == X.receiveBufferSize());
            ASSERT(A1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            // ---------------------------------------
            // Set all attributes to their 'B' values.
            // ---------------------------------------

            mX.setDebugFlag(B2);

            ASSERT(B2 == X.debugFlag());
            ASSERT(A2 == X.allowBroadcasting());
            ASSERT(A2 == X.reuseAddress());
            ASSERT(A2 == X.keepAlive());
            ASSERT(A2 == X.bypassNormalRouting());
            ASSERT(A2 == X.leaveOutOfBandDataInline());
            ASSERT(A1 == X.sendBufferSize());
            ASSERT(A1 == X.receiveBufferSize());
            ASSERT(A1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setAllowBroadcasting(B2);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(A2 == X.reuseAddress());
            ASSERT(A2 == X.keepAlive());
            ASSERT(A2 == X.bypassNormalRouting());
            ASSERT(A2 == X.leaveOutOfBandDataInline());
            ASSERT(A1 == X.sendBufferSize());
            ASSERT(A1 == X.receiveBufferSize());
            ASSERT(A1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setReuseAddress(B2);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(A2 == X.keepAlive());
            ASSERT(A2 == X.bypassNormalRouting());
            ASSERT(A2 == X.leaveOutOfBandDataInline());
            ASSERT(A1 == X.sendBufferSize());
            ASSERT(A1 == X.receiveBufferSize());
            ASSERT(A1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setKeepAlive(B2);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(A2 == X.bypassNormalRouting());
            ASSERT(A2 == X.leaveOutOfBandDataInline());
            ASSERT(A1 == X.sendBufferSize());
            ASSERT(A1 == X.receiveBufferSize());
            ASSERT(A1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setBypassNormalRouting(B2);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(A2 == X.leaveOutOfBandDataInline());
            ASSERT(A1 == X.sendBufferSize());
            ASSERT(A1 == X.receiveBufferSize());
            ASSERT(A1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setLeaveOutOfBandDataInline(B2);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(B2 == X.leaveOutOfBandDataInline());
            ASSERT(A1 == X.sendBufferSize());
            ASSERT(A1 == X.receiveBufferSize());
            ASSERT(A1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setSendBufferSize(B1);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(B2 == X.leaveOutOfBandDataInline());
            ASSERT(B1 == X.sendBufferSize());
            ASSERT(A1 == X.receiveBufferSize());
            ASSERT(A1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setReceiveBufferSize(B1);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(B2 == X.leaveOutOfBandDataInline());
            ASSERT(B1 == X.sendBufferSize());
            ASSERT(B1 == X.receiveBufferSize());
            ASSERT(A1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setMinimumSendBufferSize(B1);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(B2 == X.leaveOutOfBandDataInline());
            ASSERT(B1 == X.sendBufferSize());
            ASSERT(B1 == X.receiveBufferSize());
            ASSERT(B1 == X.minimumSendBufferSize());
            ASSERT(A1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setMinimumReceiveBufferSize(B1);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(B2 == X.leaveOutOfBandDataInline());
            ASSERT(B1 == X.sendBufferSize());
            ASSERT(B1 == X.receiveBufferSize());
            ASSERT(B1 == X.minimumSendBufferSize());
            ASSERT(B1 == X.minimumReceiveBufferSize());
            ASSERT(A1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setSendTimeout(B1);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(B2 == X.leaveOutOfBandDataInline());
            ASSERT(B1 == X.sendBufferSize());
            ASSERT(B1 == X.receiveBufferSize());
            ASSERT(B1 == X.minimumSendBufferSize());
            ASSERT(B1 == X.minimumReceiveBufferSize());
            ASSERT(B1 == X.sendTimeout());
            ASSERT(A1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setReceiveTimeout(B1);

            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(B2 == X.leaveOutOfBandDataInline());
            ASSERT(B1 == X.sendBufferSize());
            ASSERT(B1 == X.receiveBufferSize());
            ASSERT(B1 == X.minimumSendBufferSize());
            ASSERT(B1 == X.minimumReceiveBufferSize());
            ASSERT(B1 == X.sendTimeout());
            ASSERT(B1 == X.receiveTimeout());
            ASSERT(A2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());

            mX.setTcpNoDelay(B2);
            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(B2 == X.leaveOutOfBandDataInline());
            ASSERT(B1 == X.sendBufferSize());
            ASSERT(B1 == X.receiveBufferSize());
            ASSERT(B1 == X.minimumSendBufferSize());
            ASSERT(B1 == X.minimumReceiveBufferSize());
            ASSERT(B1 == X.sendTimeout());
            ASSERT(B1 == X.receiveTimeout());
            ASSERT(B2 == X.tcpNoDelay());
            ASSERT(A3 == X.linger());


            mX.setLinger(B3);
            ASSERT(B2 == X.debugFlag());
            ASSERT(B2 == X.allowBroadcasting());
            ASSERT(B2 == X.reuseAddress());
            ASSERT(B2 == X.keepAlive());
            ASSERT(B2 == X.bypassNormalRouting());
            ASSERT(B2 == X.leaveOutOfBandDataInline());
            ASSERT(B1 == X.sendBufferSize());
            ASSERT(B1 == X.receiveBufferSize());
            ASSERT(B1 == X.minimumSendBufferSize());
            ASSERT(B1 == X.minimumReceiveBufferSize());
            ASSERT(B1 == X.sendTimeout());
            ASSERT(B1 == X.receiveTimeout());
            ASSERT(B2 == X.tcpNoDelay());
            ASSERT(B3 == X.linger());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value) and use the
        //   destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //
        // Plan:
        //: 1 Use the default constructor to create an object 'X'.
        //:
        //: 2 Use the individual (as yet unproven) salient attribute
        //:   accessors to verify the default-constructed value.  (C-1)
        //
        // Testing:
        //   btlso::SocketOptions();
        //   ~btlso::SocketOptions();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DEFAULT CTOR & DTOR" << endl
                          << "===================" << endl;

        if (verbose) cout <<
                     "Create an object using the default constructor." << endl;

        {
            Obj mX;  const Obj& X = mX;

            if (verbose) cout << "Verify the object's attribute values."
                              << endl;

            // ---------------------------------------------
            // Verify the object's default attribute values.
            // ---------------------------------------------

            ASSERT(X.debugFlag().isNull());
            ASSERT(X.allowBroadcasting().isNull());
            ASSERT(X.reuseAddress().isNull());
            ASSERT(X.keepAlive().isNull());
            ASSERT(X.bypassNormalRouting().isNull());
            ASSERT(X.linger().isNull());
            ASSERT(X.leaveOutOfBandDataInline().isNull());
            ASSERT(X.sendBufferSize().isNull());
            ASSERT(X.receiveBufferSize().isNull());
            ASSERT(X.minimumSendBufferSize().isNull());
            ASSERT(X.minimumReceiveBufferSize().isNull());
            ASSERT(X.sendTimeout().isNull());
            ASSERT(X.receiveTimeout().isNull());
            ASSERT(X.tcpNoDelay().isNull());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mX;  const Obj& X = mX;

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
