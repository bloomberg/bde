// bdlmt_signaler.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLMT_SIGNALER
#define INCLUDED_BDLMT_SIGNALER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of a managed signals and slots system.
//
//@CLASSES:
//  bdlmt::Signaler:                 a signaler
//  bdlmt::SignalerConnection:       signaler/slot connection
//  bdlmt::SignalerScopedConnection: RAII signaler/slot connection
//
//@DESCRIPTION:
// This component provides an implementation of a managed signals and slots
// system.  Signals represent callbacks with multiple targets, and are also
// called publishers or events in similar systems.  Signals are connected to
// some set of slots, which are callback receivers (also called event targets
// or subscribers), which are executed when the signal is "emitted".
//
// Signals and slots are managed, in that signals and slots (or, more properly,
// objects that occur as part of the slots) can track connections and are
// capable of automatically disconnecting signal/slot connections when either
// is destroyed.  This enables the user to make signal/slot connections without
// expanding a great effort to manage the lifetimes of those connections with
// regard to the lifetimes of all objects involved.
//
///Call groups
///-----------
// Slots are free to have side effects, and that can mean that some slots may
// have to be called before others even if they are not connected in that
// order.  'bdlmt::Signaler' allows slots to be placed into groups that are
// ordered in some way.  Group values are integers, and are ordered by the
// 'integer <' relation.  By default, all connected slots have the group value
// set to 0.
//
///Concurrency and order of execution
///----------------------------------
// Within a single thread of execution slots are always executed in the order
// defined by their respective groups and, within groups, by the order they
// were connected to the signaler.  If the signaler's call operator is invoked
// concurrently from multiple threads, slots may also be executed concurrently.
//
///Slots lifetime
///--------------
// Internally, 'bdlmt::Signaler' stores copies of connected slot objects.  A
// slot object is destroyed after the slot is disconnected from the signaller,
// but the exact moment is unspecified.  It is only guaranteed that the
// lifetime of such object will not exceed the collective lifetime of the
// signaler and all connection objects associated with to that signaler.
//
///'bdlmt::Signaler' call operator
///-------------------------------
// 'bdlmt::Signaler' provides a call operator that, in C++11, would be defined
// and behave exacly as follows:
//..
//  void operator()(ARGS... args) const;
//      // Sequentially invoke each slot connected to this signaler as if by
//      // 'f_i(args...)', where 'f_i' is the i-th connected slot.  The
//      // behavior is undefined if this function is invoked from a slot
//      // connected to this signaler.
//      //
//      // Note that slots are ordered by their respective groups, and within
//      // groups, by connection order.
//      //
//      // Note also that the call operator does not forward rvalue references.
//      // That is done explicitly to prevent invocation arguments from being
//      // moved to the first slot, leaving them "empty" for all subsequent
//      // slots.
//      //
//      // Note also that if a slot is connected to or disconnected from this
//      // signaler during a call to this function, it is unspecified whether
//      // that slot will be invoked.
//      //
//      // Note also that if execution of a slot throws an exception, the
//      // invocation sequence is interrupted and the exception is propagated
//      // to the caller immediately.
//      //
//      // NOTE: 'ARGS...' are the arguments types of 'PROT'.
//..
// The actual behavior of the call operator is virtually identical to the
// description below, except that the maximum number of arguments is limited
// to 9.
//
///Thread safety
///-------------
// 'bdlmt::Signaler' is fully thread-safe, meaning that multiple threads may
// use their own instances of the class or use a shared instance without
// further synchronization.
//
// With the exception of assignment operators, 'swap()' and 'release()' member
// functions, 'bdlmt::SignalerConnection' and 'bdlmt::SignalerScopedConnection'
// are thread-safe, meaning that multiple threads may use their own instances
// of the class or use a shared instance without further synchronization.
//
// It is safe to access or modify two distinct connection objects
// simultaneously, each from a separate thread, even if they represent the same
// slot connection.
//
///Exception Safety
///----------------
//: o no 'disconnectAllSlots()', 'disconnectAllSlotsAndWait()' or 'slotCount()'
//:   member function on a signaler object throws an exception.
//:
//: o no constructor, assingment operator or comparison operator, as well as
//:   'disconnect()', 'disconnectAndWait()', 'release()' and 'isConnected()'
//:   member function on a connection object throws an exception.
//:
//: o no 'swap()' function throws an exception.
//:
//: o if an exception is thrown by the signaler's 'connect()' member function,
//:   that function has no effects.
//
///Usage
///-----
// Lets implement a GUI button class that allows users to subscribe to its
// 'onClick' events.
//..
//  class Button {
//      // A pretend GUI button.
//
//    public:
//      // TYPES
//      typedef bsl::function<void(int, int)> OnClickSlotType;
//          // Slot arguments are the 'x' and 'y' cursor coordinates.
//
//    private:
//      // PRIVATE DATA
//      bdlmt::Signaler<void(int, int)> d_onClick;
//          // Signaler arguments are the 'x' and 'y' cursor coordinates.
//
//    public:
//      // MANIPULATORS
//      bdlmt::SignalerConnection onClickConnect(const OnClickSlotType& slot);
//          // Connect the specified 'slot' to this button.
//
//      void click(int x, int y);
//          // Simulate user clicking on GUI button at coordinates 'x', 'y'.
//  };
//
//  bdlmt::SignalerConnection
//  Button::onClickConnect(const OnClickSlotType& slot)
//  {
//      return d_onClick.connect(slot);
//  }
//
//  void Button::click(int x, int y)
//  {
//      d_onClick(x, y);
//  }
//..
// And provide an event handler function printing the cursor coordinates each
// time the button is clicked.
//..
//  void printCoordinates(int x, int y)
//  {
//      bsl::cout << "(" << x << ", " << y << ")" << bsl::endl;
//  }
//..
// Then, create a button and subscribe to its events.
//..
//  Button                   button;
//  bdlmt::SignalerConnection connection = button.onClickConnect(
//                                                          &printCoordinates);
//..
// When the button is "clicked", we will receive a notification.
//..
//  button.click(100, 200);
//  // "(100, 200)" was printed to cout
//..
// Finally, unsubscribe from button's events when we don't want to receive
// notifications anymore.
//..
//  connection.disconnect();
//..

#include <bdlscm_version.h>
#include <bdlcc_skiplist.h>

#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_forwardingtype.h>
#include <bslmf_functionpointertraits.h>
#include <bslmf_integralconstant.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_typelist.h>

#include <bslmt_readerwritermutex.h>
#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_compilerfeatures.h>
#include <bsls_cpp11.h>
#include <bsls_types.h>

#include <bsl_functional.h>
#include <bsl_limits.h>
#include <bsl_memory.h>
#include <bsl_utility.h>  // bsl::pair

namespace BloombergLP {

// FORWARD DECLARATION
namespace bslma { class Allocator; }

namespace bdlmt {

template <class>
class Signaler_Impl;
class SignalerConnection;

// ALIASES
typedef bsl::pair<int, bsls::Types::Uint64> Signaler_SlotKey;
    // Defines a "key" used to index slots in an associative collection.  The
    // first element of the pair is the slot call group; the second is the slot
    // ID.

                             // ==================
                             // struct Signaler_NA
                             // ==================

struct Signaler_NA {
    // Provides a "Not an Argument" tag type.
};

                           // =====================
                           // class Signaler_AutoId
                           // =====================

struct Signaler_AutoId {
    // Provides an atomic auto-increment ID counter utility class.

    // CLASS METHOD
    static bsls::Types::Uint64 get() BSLS_CPP11_NOEXCEPT;
        // Return '++x', where 'x' is the result of the previous invocation.
        // If this function is invoked for the first time, return 1.
};

                        // ===========================
                        // class Signaler_ArgumentType
                        // ===========================

template <class PROT>
struct Signaler_ArgumentType {

  private:
    // PRIVATE TYPES
    typedef Signaler_NA                                                NA;
    typedef typename bslmf::FunctionPointerTraits<PROT*>::ArgumentList Args;

  public:
    // TYPES
    typedef typename bslmf::TypeListTypeOf<1, Args, NA>::TypeOrDefault Type1;
    typedef typename bslmf::TypeListTypeOf<2, Args, NA>::TypeOrDefault Type2;
    typedef typename bslmf::TypeListTypeOf<3, Args, NA>::TypeOrDefault Type3;
    typedef typename bslmf::TypeListTypeOf<4, Args, NA>::TypeOrDefault Type4;
    typedef typename bslmf::TypeListTypeOf<5, Args, NA>::TypeOrDefault Type5;
    typedef typename bslmf::TypeListTypeOf<6, Args, NA>::TypeOrDefault Type6;
    typedef typename bslmf::TypeListTypeOf<7, Args, NA>::TypeOrDefault Type7;
    typedef typename bslmf::TypeListTypeOf<8, Args, NA>::TypeOrDefault Type8;
    typedef typename bslmf::TypeListTypeOf<9, Args, NA>::TypeOrDefault Type9;

    typedef typename bslmf::ForwardingType<Type1>::Type ForwardingType1;
    typedef typename bslmf::ForwardingType<Type2>::Type ForwardingType2;
    typedef typename bslmf::ForwardingType<Type3>::Type ForwardingType3;
    typedef typename bslmf::ForwardingType<Type4>::Type ForwardingType4;
    typedef typename bslmf::ForwardingType<Type5>::Type ForwardingType5;
    typedef typename bslmf::ForwardingType<Type6>::Type ForwardingType6;
    typedef typename bslmf::ForwardingType<Type7>::Type ForwardingType7;
    typedef typename bslmf::ForwardingType<Type8>::Type ForwardingType8;
    typedef typename bslmf::ForwardingType<Type9>::Type ForwardingType9;
};

                          // =========================
                          // struct Signaler_Invocable
                          // =========================

template <class TARGET, class PROT>
struct Signaler_Invocable {
    // Provides a call operator for the derived class 'bdlmt::Signaler', such
    // that its call signature is identical to that of 'PROT', except that the
    // return type is 'void'.
};

template <class TARGET, class R>
struct Signaler_Invocable<TARGET, R()> {

    // ACCESSOR
    void operator()() const;
};

template <class TARGET, class R, class A1>
struct Signaler_Invocable<TARGET, R(A1)> {

    // ACCESSOR
    void operator()(A1) const;
};

template <class TARGET, class R, class A1,
                                 class A2>
struct Signaler_Invocable<TARGET, R(A1, A2)> {

    // ACCESSOR
    void operator()(A1, A2) const;
};

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3>
struct Signaler_Invocable<TARGET, R(A1, A2, A3)> {

    // ACCESSOR
    void operator()(A1, A2, A3) const;
};

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4>
struct Signaler_Invocable<TARGET, R(A1, A2, A3, A4)> {

    // ACCESSOR
    void operator()(A1, A2, A3, A4) const;
};

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5>
struct Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5)> {

    // ACCESSOR
    void operator()(A1, A2, A3, A4, A5) const;
};

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5,
                                 class A6>
struct Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5, A6)> {

    // ACCESSOR
    void operator()(A1, A2, A3, A4, A5, A6) const;
};

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5,
                                 class A6,
                                 class A7>
struct Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5, A6, A7)> {

    // ACCESSOR
    void operator()(A1, A2, A3, A4, A5, A6, A7) const;
};

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5,
                                 class A6,
                                 class A7,
                                 class A8>
struct Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5, A6, A7, A8)> {

    // ACCESSOR
    void operator()(A1, A2, A3, A4, A5, A6, A7, A8) const;
};

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5,
                                 class A6,
                                 class A7,
                                 class A8,
                                 class A9>
struct Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> {

    // ACCESSOR
    void operator()(A1, A2, A3, A4, A5, A6, A7, A8, A9) const;
};

                       // =============================
                       // class Signaler_SlotController
                       // =============================

class Signaler_SlotController {
    // Provides an interface used by 'SignalerConnection' and
    // 'SignalerScopedConnection' to "talk" to the associated slot.

  public:
    // CREATORS
    virtual ~Signaler_SlotController();
        // Destroy this object and the stored callback with it.

  public:
    // MANIPULATORS
    virtual void disconnect() BSLS_CPP11_NOEXCEPT = 0;
        // Disconnect this slot.  If the slot was already disconnected, this
        // function has no effect.
        //
        // Note that this function does not block the calling thread pending
        // completion of the slot.  There is also no guarantee that this slot
        // will not be invoked from another thread after this function
        // completes.

    virtual void disconnectAndWait() BSLS_CPP11_NOEXCEPT = 0;
        // Disconnect this slot and block the calling thread pending its
        // completion.  If the slot was already disconnected, this function has
        // no effect.  The behavior is undefined if this function is invoked
        // recursively from this slot.
        //
        // Note that it is guaranteed that this slot will not be invoked after
        // this function completes.
        //
        // Note also that this function does block pending completion of this
        // slot, even if it is already disconnected.

  public:
    // ACCESSORS
    virtual bool isConnected() const BSLS_CPP11_NOEXCEPT = 0;
        // Return 'true' if this slot is connected to its associated signaler,
        // and 'false' otherwise.
};

                            // ===================
                            // class Signaler_Slot
                            // ===================

template <class PROT>
class Signaler_Slot : public Signaler_SlotController {
    //  Provides a slot connected to a signaler.

  private:
    // PRIVATE TYPES
    typedef Signaler_Impl<PROT> Signaler;

  private:
    // PRIVATE DATA
    mutable bslmt::ReaderWriterMutex d_callMutex;
        // The purpose of this mutex is to implement the waiting behavior of
        // the 'disconnectAndWait()' function.

    bsl::weak_ptr<Signaler>          d_signaler;
        // Weak reference to the associated signaler.

    bsl::function<PROT>              d_slot;
        // The target callback.

    Signaler_SlotKey                 d_slotKey;
        // Slot key containing the call group and the slot ID.  Used when
        // notifying the signaler about disconnection.

    bsls::AtomicBool                 d_isConnected;
        // Set to 'true' on construction, and to 'false' on disconnection.
        // Used for preventing calling a slot after it has been disconnected.

  private:
    // NOT IMPLEMENTED
    Signaler_Slot(const Signaler_Slot&)            BSLS_CPP11_DELETED;
    Signaler_Slot& operator=(const Signaler_Slot&) BSLS_CPP11_DELETED;

  private:
    // PRIVATE ACCESSORS
    void
    doInvoke(bsl::integral_constant<int, 0>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

    void
    doInvoke(bsl::integral_constant<int, 1>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

    void
    doInvoke(bsl::integral_constant<int, 2>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

    void
    doInvoke(bsl::integral_constant<int, 3>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

    void
    doInvoke(bsl::integral_constant<int, 4>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

    void
    doInvoke(bsl::integral_constant<int, 5>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

    void
    doInvoke(bsl::integral_constant<int, 6>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

    void
    doInvoke(bsl::integral_constant<int, 7>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

    void
    doInvoke(bsl::integral_constant<int, 8>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

    void
    doInvoke(bsl::integral_constant<int, 9>, // arguments count tag
             typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
             typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
             typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
             typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
             typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
             typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
             typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
             typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
             typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;

  public:
    // CREATORS
    template <class SLOT>
    Signaler_Slot(const bsl::shared_ptr<Signaler>&         signaler,
                  BSLS_COMPILERFEATURES_FORWARD_REF(SLOT)  slot,
                  Signaler_SlotKey                         slotKey,
                  bslma::Allocator                        *allocator);
        // Create a 'Signaler_Slot' object associated with the specified
        // 'signaler' using the specified 'slotKey' and with the specified
        // 'slot' callback.  Use the specified 'allocator' for memory
        // allocations.

  public:
    // MANIPULATORS
    void disconnect() BSLS_CPP11_NOEXCEPT BSLS_CPP11_OVERRIDE;
        // Implements 'Signaler_SlotController::disconnect()'.

    void disconnectAndWait() BSLS_CPP11_NOEXCEPT BSLS_CPP11_OVERRIDE;
        // Implements 'Signaler_SlotController::disconnectAndWait()'.

    void notifyDisconnected() BSLS_CPP11_NOEXCEPT;
        // Notify this slot that is was disconnected from its associated
        // signaler.  After this function completes, 'isConnected()' returns
        // 'false'.

  public:
    // ACCESSORS
    void
    operator()(typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
               typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
               typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
               typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
               typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
               typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
               typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
               typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
               typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;
        // Invoke the stored callback 'c', as if by 'c(args...)', where
        // 'args...' are the specified arguments 'a1', 'a2', 'a3', etc., except
        // that the actual number of arguments passed to 'c' is equal to the
        // number of arguments for 'PROT'.  If this slot is disconnected, this
        // function has no effect.

    bool isConnected() const BSLS_CPP11_NOEXCEPT BSLS_CPP11_OVERRIDE;
        // Implements 'Signaler_SlotController::isConnected()'.
};

                            // ===================
                            // class Signaler_Impl
                            // ===================

template <class PROT>
class Signaler_Impl :
                    public bsl::enable_shared_from_this<Signaler_Impl<PROT> > {
    // Provides the implementation of a signaler.

  private:
    // PRIVATE TYPES
    typedef Signaler_Slot<PROT>                     Slot;

    typedef bdlcc::SkipList<Signaler_SlotKey, // [GROUP, ID] pair
                            bsl::shared_ptr<Slot> > IdToSlotMap;

  private:
    // PRIVATE DATA
    mutable bslmt::ReaderWriterMutex  d_callMutex;
        // The purpose of this mutex is to implement the waiting behavior of
        // 'disconnectAndWait()' and 'disconnectAllSlotsAndWait()' functions.

    IdToSlotMap                       d_slots;
        // Thread-safe collection containing slots indexed (and ordered) by
        // their respective keys.

    bslma::Allocator                 *d_allocator_p;

  private:
    // NOT IMPLEMENTED
    Signaler_Impl(const Signaler_Impl&)            BSLS_CPP11_DELETED;
    Signaler_Impl& operator=(const Signaler_Impl&) BSLS_CPP11_DELETED;

  public:
    // CREATORS
    explicit Signaler_Impl(bslma::Allocator *allocator);
        // Create a 'Signaler_Impl' object having no connected slots.  Specify
        // an 'allocator' used to supply memory.
        //
        // Note that the supplied allocator shall remain valid until all
        // connection objects associated with this signaler are destroyed.

  public:
    // MANIPULATORS
    template <class SLOT>
    SignalerConnection connect(BSLS_COMPILERFEATURES_FORWARD_REF(SLOT) slot,
                               int                                     group);
        // Implements 'Signaler::connect()'.

    void disconnect(int group) BSLS_CPP11_NOEXCEPT;
        // Implements 'Signaler::disconnect()'.

    void disconnectAndWait(int group) BSLS_CPP11_NOEXCEPT;
        // Implements 'Signaler::disconnectAndWait()'.

    void disconnectAllSlots() BSLS_CPP11_NOEXCEPT;
        // Implements 'Signaler::disconnectAllSlots()'.

    void disconnectAllSlotsAndWait() BSLS_CPP11_NOEXCEPT;
        // Implements 'Signaler::disconnectAllSlotsAndWait()'.

    void notifyDisconnected(Signaler_SlotKey slotKey) BSLS_CPP11_NOEXCEPT;
        // Notify this signaler that a slot with the specified 'slotKey' was
        // disconnected.

  public:
    // ACCESSORS
    void
    operator()(typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
               typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
               typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
               typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
               typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
               typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
               typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
               typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
               typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const;
        // Implements 'Signaler's call operator.

    size_t slotCount() const BSLS_CPP11_NOEXCEPT;
        // Implements 'Signaler::slotCount()'.
};

                               // ==============
                               // class Signaler
                               // ==============

template <class PROT>
class Signaler : public Signaler_Invocable<Signaler<PROT>, PROT> {
    // This class template provides a thread-safe signaler that executes
    // connected slots when invoked via its call operator.
    //
    // Note that, unless otherwise specified, it is safe to invoke any method
    // of 'Signaler' from the context of a slot connected to that signaler.

  public:
    // TYPES
    typedef void ResultType;

  private:
    // PRIVATE DATA
    bsl::shared_ptr<Signaler_Impl<PROT> > d_impl;

    // FRIENDS
    friend struct Signaler_Invocable<Signaler<PROT>, PROT>;

  private:
    // NOT IMPLEMENTED
    Signaler(const Signaler&)            BSLS_CPP11_DELETED;
    Signaler& operator=(const Signaler&) BSLS_CPP11_DELETED;

  public:
    // CREATORS
    explicit Signaler(bslma::Allocator *basicAllocator = 0);
        // Create a 'Signaler' object having no connected slots.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
        //
        // No that the supplied allocator shall remain valid until all
        // connection objects associated with this signaler are destroyed.

    ~Signaler();
        // Destroy this object.  Call 'disconnectAllSlots()'.  The behavior is
        // undefined if this function is invoked from a slot connected to this
        // signaler.

  public:
    // MANIPULATORS
    template <class SLOT>
    SignalerConnection connect(
                            BSLS_COMPILERFEATURES_FORWARD_REF(SLOT) slot,
                            int                                     group = 0);
        // Connect the specified 'slot' to this signaler.  Optionally specify a
        // 'group' used to order slots upon invocation.  Return an instance of
        // 'SignalerConnection' representing the created connection.
        //
        // Note that the connected slot may be invoked from another thread
        // before this function completes.
        //
        // Note also that it is unspecified whether connecting a slot while the
        // signaler is calling will result in the slot being invoked
        // immediately.
        //
        // This function meets the strong exception guarantee.  If an exception
        // is thrown, this function has no effect.
        //
        // Let 'ARGS...' be the arguments types of 'PROT', and 'args...' be
        // lvalues of types 'bsl::decay_t<ARGS>...'. Given an lvalue 'f' of
        // type 'bsl::decay_t<SLOT>', the expression 'f(args...)' shall be well
        // formed and have a well-defined behavior. 'SLOT' must meet the
        // requirements of Destructible and MoveConstructible as specified in
        // the C++ standard.

    void disconnect(int group) BSLS_CPP11_NOEXCEPT;
        // Disconnect all slots in the specified 'group', if any.
        //
        // Note that this function does not block the calling thread pending
        // completion of currently executing slots.  There is also no guarantee
        // that disconnected slots will not be invoked from another thread
        // after this function completes.  If such behavior is desired, use
        // 'disconnectAndWait()' instead.
        //
        // Note also that if a slot is connected to this signaler during a
        // call to this function, it is unspecified whether that slot will
        // be disconnected.

    void disconnectAndWait(int group) BSLS_CPP11_NOEXCEPT;
        // Disconnect all slots in the specified 'group', if any, and block the
        // calling thread pending completion of currently executing slots.  The
        // behavior is undefined if this function is invoked from a slot
        // connected to this signaler.
        //
        // Note that it is guaranteed that disconnected slots will not be
        // invoked after this function completes.
        //
        // Note also that if a slot is connected to this signaler during a call
        // to this function, it is unspecified whether that slot will be
        // disconnected.
        //
        // Note also that this function does block pending completion of
        // already disconnected, but still executing slots.

    void disconnectAllSlots() BSLS_CPP11_NOEXCEPT;
        // Disconnect all slots connected to this signaler, if any.
        //
        // Note that this function does not block the calling thread pending
        // completion of currently executing slots.  There is also no guarantee
        // that disconnected slots will not be invoked from another thread
        // after this function completes.  If such behavior is desired, use
        // 'disconnectAllSlotsAndWait()' instead.
        //
        // Note also that if a slot is connected to this signaler during a call
        // to this function, it is unspecified whether that slot will be
        // disconnected.

    void disconnectAllSlotsAndWait() BSLS_CPP11_NOEXCEPT;
        // Disconnect all slots connected to this signaler, if any, and block
        // the calling thread pending completion of currently executing slots.
        // The behavior is undefined if this function is invoked from a slot
        // connected to this signaler.
        //
        // Note that it is guaranteed that disconnected slots will not be
        // invoked after this function completes.
        //
        // Note also that if a slot is connected to this signaler during a call
        // to this function, it is unspecified whether that slot will be
        // disconnected.
        //
        // Note also that this function does block pending completion of
        // already disconnected, but still executing slots.

  public:
    // ACCESSORS
    size_t slotCount() const BSLS_CPP11_NOEXCEPT;
        // Return the number of slots connected to this signaler.  Note that in
        // multithreaded environment, the value returned by 'slotCount()' is
        // approximate.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Signaler, bslma::UsesBslmaAllocator)
};

                          // ========================
                          // class SignalerConnection
                          // ========================

class SignalerConnection {
    // This class represents a connection between a signaler and a slot.  It is
    // a lightweight object that has the ability to query whether the signaler
    // and slot are currently connected, and to disconnect the slot from the
    // signaler.
    //
    // Note that, unless otherwise specified, it is safe to invoke any method
    // of 'SignalerConnection' from the context of its associated slot, or any
    // other slot.

  private:
    // PRIVATE TYPES
    typedef Signaler_SlotController Slot;

  private:
    // PRIVATE DATA
    bsl::weak_ptr<Slot> d_slot;
        // Weak reference to the associated slot.

    // FRIENDS
    template <class>
    friend class Signaler_Impl;

    friend bool operator<(const SignalerConnection&,
                          const SignalerConnection&) BSLS_CPP11_NOEXCEPT;

  private:
    // PRIVATE CREATORS
    explicit
    SignalerConnection(const bsl::shared_ptr<Slot>& slot) BSLS_CPP11_NOEXCEPT;

  public:
    // CREATORS
    SignalerConnection() BSLS_CPP11_NOEXCEPT;
        // Create a 'SignalerConnection' object having no associated slot.

    SignalerConnection(const SignalerConnection& original) BSLS_CPP11_NOEXCEPT;
        // Create a 'SignalerConnection' object that refers to and assumes
        // management of the same slot (if any) as the specified 'original'
        // object.

    SignalerConnection(bslmf::MovableRef<SignalerConnection> original)
                                                           BSLS_CPP11_NOEXCEPT;
        // Create a 'SignalerConnection' object that refers to and assumes
        // management of the same slot (if any) as the specified 'original'
        // object, and reset 'original' to a default-constructed state.

  public:
    // MANIPULATORS
    SignalerConnection&
    operator=(const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection. Return '*this'.

    SignalerConnection&
    operator=(bslmf::MovableRef<SignalerConnection> rhs) BSLS_CPP11_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection, and reset 'rhs' to a
        // default-constructed state. Return '*this'.

    void disconnect() BSLS_CPP11_NOEXCEPT;
        // Disconnect the associated slot.  If '*this' does not have an
        // associated slot or the slot was already disconnected, this function
        // has no effect.
        //
        // Note that this function does not block the calling thread pending
        // completion of the associated slot.  There is also no guarantee that
        // the disconnected slot will not be invoked from another thread after
        // this function completes.  If such behavior is desired, use
        // 'disconnectAndWait()' instead.

    void disconnectAndWait() BSLS_CPP11_NOEXCEPT;
        // Disconnect the associated slot and block the calling thread pending
        // its completion.  If '*this' does not have an associated slot, or the
        // slot was already disconnected, this function has no effect.  The
        // behavior is undefined if this function is invoked from the slot
        // associated with '*this'.
        //
        // Note that it is guaranteed that the disconnected slot will not be
        // invoked after this function completes.
        //
        // Note also that this function does block pending completion of an
        // already disconnected, but still executing slot.

    void swap(SignalerConnection& other) BSLS_CPP11_NOEXCEPT;
        // Swap the contents of '*this' and 'other'.

  public:
    // ACCESSORS
    bool isConnected() const BSLS_CPP11_NOEXCEPT;
        // Return 'true' if the associated slot is connected to the signaler
        // '*this' was obtained from, and 'false' otherwise.  If '*this' does
        // not have an associated slot (i.e., was default-constructed), return
        // 'false'.
};

                       // ==============================
                       // class SignalerScopedConnection
                       // ==============================

class SignalerScopedConnection : public SignalerConnection {
    // This class represents a scoped connection between a signaler and a slot.
    // It is a lightweight object that has the ability to query whether the
    // signaler and slot are currently connected, and to disconnect the slot
    // from the signaler.  The slot is automatically disconnected on
    // destruction.
    //
    // Note that, unless otherwise specified, it is safe to invoke any method
    // of 'SignalerScopedConnection' from the context of its associated slot,
    // or any other slot.

  private:
    // NOT IMPLEMENTED
    SignalerScopedConnection(const SignalerScopedConnection&)
                                                            BSLS_CPP11_DELETED;
    SignalerScopedConnection& operator=(const SignalerScopedConnection&)
                                                            BSLS_CPP11_DELETED;

  public:
    // CREATORS
    SignalerScopedConnection() BSLS_CPP11_NOEXCEPT;
        // Create a 'SignalerScopedConnection' object having no associated
        // slot.

    SignalerScopedConnection(
                          bslmf::MovableRef<SignalerScopedConnection> original)
                                                           BSLS_CPP11_NOEXCEPT;
        // Create a 'SignalerScopedConnection' that refers to and assumes
        // management of the same slot (if any) as the specified 'original'
        // object, and reset 'original' to a default-constructed state.

    SignalerScopedConnection(const SignalerConnection& connection)
                                               BSLS_CPP11_NOEXCEPT; // IMPLICIT
        // Create a 'SignalerScopedConnection' object that refers to and
        // assumes management of the same slot (if any) as the specified
        // 'connection' object.

    SignalerScopedConnection(bslmf::MovableRef<SignalerConnection> connection)
                                               BSLS_CPP11_NOEXCEPT; // IMPLICIT
        // Create a 'SignalerScopedConnection' object that refers to and
        // assumes management of the same slot (if any) as the specified
        // 'connection' object, and reset 'connection' to a default-
        // constructed state.

    ~SignalerScopedConnection();
        // Destroy this object. Call 'disconnect()'.

  public:
    // MANIPULATORS
    SignalerScopedConnection&
    operator=(bslmf::MovableRef<SignalerScopedConnection> rhs)
                                                           BSLS_CPP11_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection, and reset 'rhs' to a
        // default-constructed state.  If, prior to this call '*this' has an
        // associated slot, disconnect that slot by a call to
        // 'disconnect()'.  Return '*this'.

    SignalerScopedConnection&
    operator=(const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection.  If, prior to this call
        // '*this' has an associated slot, disconnect that slot by a call to
        // 'disconnect()'.  Return '*this'.

    SignalerScopedConnection&
    operator=(bslmf::MovableRef<SignalerConnection> rhs) BSLS_CPP11_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection, and reset 'rhs' to a
        // default-constructed state.  If, prior to this call '*this' has an
        // associated slot, disconnect that slot by a call to
        // 'disconnect()'.  Return '*this'.

    SignalerConnection release() BSLS_CPP11_NOEXCEPT;
        // Disassociate this connection object from its associated slot, if
        // any, and reset '*this' to a default-constructed state.  Return a
        // copy of the 'SignalerConnection' object '*this' was constructed
        // from.  If '*this' was default-constructed, return a
        // default-constructed value.

    void swap(SignalerScopedConnection& other) BSLS_CPP11_NOEXCEPT;
        // Swap the contents of '*this' and 'other'.
};

// FREE OPERATORS
bool operator==(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT;

bool operator!=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT;

bool operator<(const SignalerConnection& lhs,
               const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT;

bool operator>(const SignalerConnection& lhs,
               const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT;

bool operator<=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT;

bool operator>=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT;
    // Compare two 'SignalerConnection' objects. Return 'true' if the condition
    // holds, and 'false' otherwise.  The ordering is strict weak ordering
    // relation.  Two connections compare equal (and equivalent) only if both
    // of them are default-constructed or if they both refer to the same slot.
    //
    // Note that disconnecting a slot does not affect the result of a
    // comparison operation.

void swap(SignalerConnection& lhs,
          SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT;
    // Swap the contents of 'lhs' and 'rhs'.

void swap(SignalerScopedConnection& lhs,
          SignalerScopedConnection& rhs) BSLS_CPP11_NOEXCEPT;
    // Swap the contents of 'lhs' and 'rhs'.

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                         // -------------------------
                         // struct Signaler_Invocable
                         // -------------------------

template <class TARGET, class R>
inline
void Signaler_Invocable<TARGET, R()>::operator()() const
{
    (*static_cast<const TARGET&>(*this).d_impl)(Signaler_NA(),
                                                Signaler_NA(),
                                                Signaler_NA(),
                                                Signaler_NA(),
                                                Signaler_NA(),
                                                Signaler_NA(),
                                                Signaler_NA(),
                                                Signaler_NA(),
                                                Signaler_NA());
}

template <class TARGET, class R, class A1>
inline
void Signaler_Invocable<TARGET, R(A1)>::operator()(A1 a1) const
{
    (*static_cast<const TARGET&>(*this).d_impl)(
                            bslmf::ForwardingTypeUtil<A1>::forwardToTarget(a1),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA());
}

template <class TARGET, class R, class A1,
                                 class A2>
inline
void Signaler_Invocable<TARGET, R(A1, A2)>::operator()(A1 a1,
                                                       A2 a2) const
{
    (*static_cast<const TARGET&>(*this).d_impl)(
                            bslmf::ForwardingTypeUtil<A1>::forwardToTarget(a1),
                            bslmf::ForwardingTypeUtil<A2>::forwardToTarget(a2),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA());
}

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3>
inline
void Signaler_Invocable<TARGET, R(A1, A2, A3)>::operator()(A1 a1,
                                                           A2 a2,
                                                           A3 a3) const
{
    (*static_cast<const TARGET&>(*this).d_impl)(
                            bslmf::ForwardingTypeUtil<A1>::forwardToTarget(a1),
                            bslmf::ForwardingTypeUtil<A2>::forwardToTarget(a2),
                            bslmf::ForwardingTypeUtil<A3>::forwardToTarget(a3),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA());
}

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4>
inline
void Signaler_Invocable<TARGET, R(A1, A2, A3, A4)>::operator()(A1 a1,
                                                               A2 a2,
                                                               A3 a3,
                                                               A4 a4) const
{
    (*static_cast<const TARGET&>(*this).d_impl)(
                            bslmf::ForwardingTypeUtil<A1>::forwardToTarget(a1),
                            bslmf::ForwardingTypeUtil<A2>::forwardToTarget(a2),
                            bslmf::ForwardingTypeUtil<A3>::forwardToTarget(a3),
                            bslmf::ForwardingTypeUtil<A4>::forwardToTarget(a4),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA());
}

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5>
inline
void Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5)>::operator()(A1 a1,
                                                                   A2 a2,
                                                                   A3 a3,
                                                                   A4 a4,
                                                                   A5 a5) const
{
    (*static_cast<const TARGET&>(*this).d_impl)(
                           bslmf::ForwardingTypeUtil<A1>::forwardToTarget(a1),
                           bslmf::ForwardingTypeUtil<A2>::forwardToTarget(a2),
                           bslmf::ForwardingTypeUtil<A3>::forwardToTarget(a3),
                           bslmf::ForwardingTypeUtil<A4>::forwardToTarget(a4),
                           bslmf::ForwardingTypeUtil<A5>::forwardToTarget(a5),
                           Signaler_NA(),
                           Signaler_NA(),
                           Signaler_NA(),
                           Signaler_NA());
}

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5,
                                 class A6>
inline
void Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5, A6)>::operator()(
                                                                  A1 a1,
                                                                  A2 a2,
                                                                  A3 a3,
                                                                  A4 a4,
                                                                  A5 a5,
                                                                  A6 a6) const
{
    (*static_cast<const TARGET&>(*this).d_impl)(
                            bslmf::ForwardingTypeUtil<A1>::forwardToTarget(a1),
                            bslmf::ForwardingTypeUtil<A2>::forwardToTarget(a2),
                            bslmf::ForwardingTypeUtil<A3>::forwardToTarget(a3),
                            bslmf::ForwardingTypeUtil<A4>::forwardToTarget(a4),
                            bslmf::ForwardingTypeUtil<A5>::forwardToTarget(a5),
                            bslmf::ForwardingTypeUtil<A6>::forwardToTarget(a6),
                            Signaler_NA(),
                            Signaler_NA(),
                            Signaler_NA());
}

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5,
                                 class A6,
                                 class A7>
inline
void Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5, A6, A7)>::operator()(
                                                                   A1 a1,
                                                                   A2 a2,
                                                                   A3 a3,
                                                                   A4 a4,
                                                                   A5 a5,
                                                                   A6 a6,
                                                                   A7 a7) const
{
    (*static_cast<const TARGET&>(*this).d_impl)(
                            bslmf::ForwardingTypeUtil<A1>::forwardToTarget(a1),
                            bslmf::ForwardingTypeUtil<A2>::forwardToTarget(a2),
                            bslmf::ForwardingTypeUtil<A3>::forwardToTarget(a3),
                            bslmf::ForwardingTypeUtil<A4>::forwardToTarget(a4),
                            bslmf::ForwardingTypeUtil<A5>::forwardToTarget(a5),
                            bslmf::ForwardingTypeUtil<A6>::forwardToTarget(a6),
                            bslmf::ForwardingTypeUtil<A7>::forwardToTarget(a7),
                            Signaler_NA(),
                            Signaler_NA());
}

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5,
                                 class A6,
                                 class A7,
                                 class A8>
inline
void Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5, A6, A7, A8)>::operator()(
                                                                   A1 a1,
                                                                   A2 a2,
                                                                   A3 a3,
                                                                   A4 a4,
                                                                   A5 a5,
                                                                   A6 a6,
                                                                   A7 a7,
                                                                   A8 a8) const
{
    (*static_cast<const TARGET&>(*this).d_impl)(
                            bslmf::ForwardingTypeUtil<A1>::forwardToTarget(a1),
                            bslmf::ForwardingTypeUtil<A2>::forwardToTarget(a2),
                            bslmf::ForwardingTypeUtil<A3>::forwardToTarget(a3),
                            bslmf::ForwardingTypeUtil<A4>::forwardToTarget(a4),
                            bslmf::ForwardingTypeUtil<A5>::forwardToTarget(a5),
                            bslmf::ForwardingTypeUtil<A6>::forwardToTarget(a6),
                            bslmf::ForwardingTypeUtil<A7>::forwardToTarget(a7),
                            bslmf::ForwardingTypeUtil<A8>::forwardToTarget(a8),
                            Signaler_NA());
}

template <class TARGET, class R, class A1,
                                 class A2,
                                 class A3,
                                 class A4,
                                 class A5,
                                 class A6,
                                 class A7,
                                 class A8,
                                 class A9>
inline
void
Signaler_Invocable<TARGET, R(A1, A2, A3, A4, A5, A6, A7, A8, A9)>::operator()(
                                                                   A1 a1,
                                                                   A2 a2,
                                                                   A3 a3,
                                                                   A4 a4,
                                                                   A5 a5,
                                                                   A6 a6,
                                                                   A7 a7,
                                                                   A8 a8,
                                                                   A9 a9) const
{
    static_cast<const TARGET&>(*this).d_impl->invoke(
                           bslmf::ForwardingTypeUtil<A1>::forwardToTarget(a1),
                           bslmf::ForwardingTypeUtil<A2>::forwardToTarget(a2),
                           bslmf::ForwardingTypeUtil<A3>::forwardToTarget(a3),
                           bslmf::ForwardingTypeUtil<A4>::forwardToTarget(a4),
                           bslmf::ForwardingTypeUtil<A5>::forwardToTarget(a5),
                           bslmf::ForwardingTypeUtil<A6>::forwardToTarget(a6),
                           bslmf::ForwardingTypeUtil<A7>::forwardToTarget(a7),
                           bslmf::ForwardingTypeUtil<A8>::forwardToTarget(a8),
                           bslmf::ForwardingTypeUtil<A9>::forwardToTarget(a9));
}

                            // -------------------
                            // class Signaler_Slot
                            // -------------------

// PRIVATE MANIPULATORS
template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 0>,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    d_slot();
}

template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 1>,
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    // NOTE: Does not forward

    d_slot(a1);
}

template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 2>,
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    // NOTE: Does not forward

    d_slot(a1, a2);
}

template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 3>,
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    // NOTE: Does not forward

    d_slot(a1, a2, a3);
}

template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 4>,
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    // NOTE: Does not forward

    d_slot(a1, a2, a3, a4);
}

template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 5>,
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    // NOTE: Does not forward

    d_slot(a1, a2, a3, a4, a5);
}

template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 6>,
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    // NOTE: Does not forward

    d_slot(a1, a2, a3, a4, a5, a6);
}

template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 7>,
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    // NOTE: Does not forward

    d_slot(a1, a2, a3, a4, a5, a6, a7);
}

template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 8>,
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                BSLS_ANNOTATION_UNUSED
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    // NOTE: Does not forward

    d_slot(a1, a2, a3, a4, a5, a6, a7, a8);
}

template <class PROT>
inline
void Signaler_Slot<PROT>::doInvoke(
                BSLS_ANNOTATION_UNUSED bsl::integral_constant<int, 9>,
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    // NOTE: Does not forward

    d_slot(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

// CREATORS
template <class PROT>
template <class SLOT>
inline
Signaler_Slot<PROT>::Signaler_Slot(
                            const bsl::shared_ptr<Signaler>&         signaler,
                            BSLS_COMPILERFEATURES_FORWARD_REF(SLOT)  slot,
                            Signaler_SlotKey                         slotKey,
                            bslma::Allocator                        *allocator)
: d_callMutex()
, d_signaler(signaler)
, d_slot(bsl::allocator_arg,
         allocator,
         BSLS_COMPILERFEATURES_FORWARD(SLOT, slot))
, d_slotKey(slotKey)
, d_isConnected(true)
{
    BSLS_ASSERT(signaler);
    BSLS_ASSERT(allocator);
}

// MANIPULATORS
template <class PROT>
inline
void Signaler_Slot<PROT>::operator()(
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    typedef Signaler_ArgumentType<PROT>                                AT;
    typedef typename bslmf::FunctionPointerTraits<PROT*>::ArgumentList AL;

    BSLS_ASSERT(!d_signaler.expired());

    // Hold this mutex (in read mode), so that 'disconnectAndWait()' can
    // synchronize with the call operator

    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> lock(&d_callMutex);  // LOCK

    if (!d_isConnected) {
        // The slot was disconnected.  Do nothing.

        return;                                                       // RETURN
    }

    doInvoke(
           bsl::integral_constant<int, AL::LENGTH>(), // args count tag
           bslmf::ForwardingTypeUtil<typename AT::Type1>::forwardToTarget(a1),
           bslmf::ForwardingTypeUtil<typename AT::Type2>::forwardToTarget(a2),
           bslmf::ForwardingTypeUtil<typename AT::Type3>::forwardToTarget(a3),
           bslmf::ForwardingTypeUtil<typename AT::Type4>::forwardToTarget(a4),
           bslmf::ForwardingTypeUtil<typename AT::Type5>::forwardToTarget(a5),
           bslmf::ForwardingTypeUtil<typename AT::Type6>::forwardToTarget(a6),
           bslmf::ForwardingTypeUtil<typename AT::Type7>::forwardToTarget(a7),
           bslmf::ForwardingTypeUtil<typename AT::Type8>::forwardToTarget(a8),
           bslmf::ForwardingTypeUtil<typename AT::Type9>::forwardToTarget(a9));
}

template <class PROT>
inline
void Signaler_Slot<PROT>::disconnect() BSLS_CPP11_NOEXCEPT
{
    if (!d_isConnected.testAndSwap(true, false)) {
        // Already disconnected.  Do nothing.

        return;                                                       // RETURN
    }

    // Notify the associated signaler

    bsl::shared_ptr<Signaler> signaler = d_signaler.lock();
    if (signaler) {
        signaler->notifyDisconnected(d_slotKey);
    }
}

template <class PROT>
inline
void Signaler_Slot<PROT>::disconnectAndWait() BSLS_CPP11_NOEXCEPT
{
    // Disconnect the slot.

    disconnect();

    // Synchronize with the call operator.

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> lock(&d_callMutex); // LOCK
}

template <class PROT>
inline
void Signaler_Slot<PROT>::notifyDisconnected() BSLS_CPP11_NOEXCEPT
{
    d_isConnected = false;
}

// ACCESSORS
template <class PROT>
inline
bool Signaler_Slot<PROT>::isConnected() const BSLS_CPP11_NOEXCEPT
{
    return d_isConnected;
}

                            // -------------------
                            // class Signaler_Impl
                            // -------------------

// CREATORS
template <class PROT>
inline
Signaler_Impl<PROT>::Signaler_Impl(bslma::Allocator *allocator)
: d_callMutex()
, d_slots(allocator)
, d_allocator_p(allocator)
{
    BSLS_ASSERT(allocator);
}

// MANIPULATORS
template <class PROT>
inline
void Signaler_Impl<PROT>::operator()(
                typename Signaler_ArgumentType<PROT>::ForwardingType1 a1,
                typename Signaler_ArgumentType<PROT>::ForwardingType2 a2,
                typename Signaler_ArgumentType<PROT>::ForwardingType3 a3,
                typename Signaler_ArgumentType<PROT>::ForwardingType4 a4,
                typename Signaler_ArgumentType<PROT>::ForwardingType5 a5,
                typename Signaler_ArgumentType<PROT>::ForwardingType6 a6,
                typename Signaler_ArgumentType<PROT>::ForwardingType7 a7,
                typename Signaler_ArgumentType<PROT>::ForwardingType8 a8,
                typename Signaler_ArgumentType<PROT>::ForwardingType9 a9) const
{
    typename IdToSlotMap::PairHandle slotHandle;
    Signaler_SlotKey                 lastInvokedSlotKey(
                                           bsl::numeric_limits<int>::min(), 0);

    // Hold this mutex (in read mode), so that 'disconnectAndWait()'
    // (or 'disconnectAllSlotsAndWait()') can synchonize with the call operator

    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> lock(&d_callMutex);  // LOCK

    // NOTE: If a slot is removed from the connected slots list while we are
    //       iterating over it, we may have to start the iteration from the
    //       beginning.  To prevent invoking a slot twice, we remember the key
    //       of the last invoked slot.  That works because slots are ordered by
    //       their respective keys.  If a new slot is connected while this
    //       function is executing, that slot may (or may not) be invoked.

    do {
        if (d_slots.front(&slotHandle) != 0) {
            // No slots.  Do nothing.

            return;                                                   // RETURN
        }

        do {
            const Slot&             slot    = *slotHandle.data();
            const Signaler_SlotKey& slotKey = slotHandle.key();

            if (slotKey <= lastInvokedSlotKey) {
                // This slot was already invoked.  Skip it.

                continue;                                           // CONTINUE
            }

            // invoke the slot

            typedef Signaler_ArgumentType<PROT> AT;
            slot(bslmf::ForwardingTypeUtil<typename AT::Type1>
                                                        ::forwardToTarget(a1),
                 bslmf::ForwardingTypeUtil<typename AT::Type2>
                                                        ::forwardToTarget(a2),
                 bslmf::ForwardingTypeUtil<typename AT::Type3>
                                                        ::forwardToTarget(a3),
                 bslmf::ForwardingTypeUtil<typename AT::Type4>
                                                        ::forwardToTarget(a4),
                 bslmf::ForwardingTypeUtil<typename AT::Type5>
                                                        ::forwardToTarget(a5),
                 bslmf::ForwardingTypeUtil<typename AT::Type6>
                                                        ::forwardToTarget(a6),
                 bslmf::ForwardingTypeUtil<typename AT::Type7>
                                                        ::forwardToTarget(a7),
                 bslmf::ForwardingTypeUtil<typename AT::Type8>
                                                        ::forwardToTarget(a8),
                 bslmf::ForwardingTypeUtil<typename AT::Type9>
                                                        ::forwardToTarget(a9));

            lastInvokedSlotKey = slotKey; // remember we invoked that slot
        } while (d_slots.skipForward(&slotHandle) == 0 && slotHandle);
    } while (slotHandle);
}

template <class PROT>
template <class SLOT>
inline
SignalerConnection
Signaler_Impl<PROT>::connect(BSLS_COMPILERFEATURES_FORWARD_REF(SLOT) slot,
                             int                                     group)
{
    // create a key the slot will be indexed by

    const Signaler_SlotKey slotKey(group, Signaler_AutoId::get());

    // create a slot

    bsl::shared_ptr<Slot> slotImpl =
                                 bsl::allocate_shared<Slot>(
                                     d_allocator_p,
                                     this->shared_from_this(),
                                     BSLS_COMPILERFEATURES_FORWARD(SLOT, slot),
                                     slotKey,
                                     d_allocator_p);

    // connect the slot

    d_slots.addR(slotKey, slotImpl);

    // return the connection

    return SignalerConnection(slotImpl);
}

template <class PROT>
inline
void Signaler_Impl<PROT>::disconnect(int group) BSLS_CPP11_NOEXCEPT
{
    typename IdToSlotMap::PairHandle slotHandle;

    while (  d_slots.findLowerBound(&slotHandle, bsl::make_pair(group, 0)) == 0
           && slotHandle.key().first == group) {
        // disconnect all slots in the specified 'group', one by one

        // notify the slot it's being disconnected

        slotHandle.data()->notifyDisconnected();

        // remove the slot from the collection

        d_slots.remove(slotHandle);
    }

    // TODO: This function complexity is N*logN. Should investigate if it can
    //       be made linear.
}

template <class PROT>
inline
void Signaler_Impl<PROT>::disconnectAndWait(int group) BSLS_CPP11_NOEXCEPT
{
    // disconnect slots

    disconnect(group);

    // synchronize with the call operator

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> lock(&d_callMutex); // LOCK
}

template <class PROT>
inline
void Signaler_Impl<PROT>::disconnectAllSlots() BSLS_CPP11_NOEXCEPT
{
    typename IdToSlotMap::PairHandle slotHandle;

    while (d_slots.front(&slotHandle) == 0) {
        // disconnect all slots in the collection, one by one

        // notify the slot it's being disconnected

        slotHandle.data()->notifyDisconnected();

        // remove the slot from the collection

        d_slots.remove(slotHandle);
    }
}

template <class PROT>
inline
void Signaler_Impl<PROT>::disconnectAllSlotsAndWait() BSLS_CPP11_NOEXCEPT
{
    // disconnect slots

    disconnectAllSlots();

    // synchronize with the call operator

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> lock(&d_callMutex); // LOCK
}

template <class PROT>
inline
void Signaler_Impl<PROT>::notifyDisconnected(Signaler_SlotKey slotKey)
                                                            BSLS_CPP11_NOEXCEPT
{
    typename IdToSlotMap::PairHandle slotHandle;

    if (d_slots.find(&slotHandle, slotKey) != 0) {
        // Slot was already removed.  Do nothing.

        // NOTE: The slot may have been removed concurrently as the result of a
        //       call to 'removeAllSlots()' or 'removeAllSlotsAndWait()'.

        return;                                                       // RETURN
    }

    // remove the slot from the collection

    d_slots.remove(slotHandle);
}

// ACCESSORS
template <class PROT>
inline
size_t Signaler_Impl<PROT>::slotCount() const BSLS_CPP11_NOEXCEPT
{
    return d_slots.length();
}

                               // --------------
                               // class Signaler
                               // --------------

// CREATORS
template <class PROT>
inline
Signaler<PROT>::Signaler(bslma::Allocator *basicAllocator)
: d_impl(bsl::allocate_shared<Signaler_Impl<PROT> >(
                                    basicAllocator,
                                    bslma::Default::allocator(basicAllocator)))
{
    // NOTHING
}

template <class PROT>
inline
Signaler<PROT>::~Signaler()
{
    d_impl->disconnectAllSlots();
}

// MANIPULATORS
template <class PROT>
template <class SLOT>
inline
SignalerConnection Signaler<PROT>::connect(
                                 BSLS_COMPILERFEATURES_FORWARD_REF(SLOT) slot,
                                 int                                     group)
{
    return d_impl->connect(BSLS_COMPILERFEATURES_FORWARD(SLOT, slot), group);
}

template <class PROT>
inline
void Signaler<PROT>::disconnect(int group) BSLS_CPP11_NOEXCEPT
{
    d_impl->disconnect(group);
}

template <class PROT>
inline
void Signaler<PROT>::disconnectAndWait(int group) BSLS_CPP11_NOEXCEPT
{
    d_impl->disconnectAndWait(group);
}

template <class PROT>
inline
void Signaler<PROT>::disconnectAllSlots() BSLS_CPP11_NOEXCEPT
{
    d_impl->disconnectAllSlots();
}

template <class PROT>
inline
void Signaler<PROT>::disconnectAllSlotsAndWait() BSLS_CPP11_NOEXCEPT
{
    d_impl->disconnectAllSlotsAndWait();
}

// ACCESSORS
template <class PROT>
inline
size_t Signaler<PROT>::slotCount() const BSLS_CPP11_NOEXCEPT
{
    return d_impl->slotCount();
}

// FREE OPERATORS
inline
bool operator==(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT
{
    return !(lhs < rhs) && !(rhs < lhs);
}

inline
bool operator!=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT
{
    return !(lhs == rhs);
}

inline
bool operator<(const SignalerConnection& lhs,
               const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT
{
    return lhs.d_slot.owner_before(rhs.d_slot);
}

inline
bool operator>(const SignalerConnection& lhs,
               const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT
{
    return rhs < lhs;
}

inline
bool operator<=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT
{
    return !(rhs < lhs);
}

inline
bool operator>=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT
{
    return !(lhs < rhs);
}

inline
void swap(SignalerConnection& lhs,
          SignalerConnection& rhs) BSLS_CPP11_NOEXCEPT
{
    lhs.swap(rhs);
}

inline
void swap(SignalerScopedConnection& lhs,
          SignalerScopedConnection& rhs) BSLS_CPP11_NOEXCEPT
{
    lhs.swap(rhs);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2018
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
