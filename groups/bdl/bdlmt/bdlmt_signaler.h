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
//@DESCRIPTION: This component provides the template 'class' 'bdlmt::Signaler'
// an implementation of a managed signals and slots system.  Signals represent
// callbacks with multiple targets, and are also called publishers or events in
// similar systems.  Signals are connected to some set of slots, which are
// callback receivers (also called event targets or subscribers), which are
// executed when the signal is "emitted".
//
// Signals and slots are managed, in that signals and slots (or, more properly,
// objects that occur as part of the slots) can track connections and are
// capable of automatically disconnecting signal/slot connections when either
// is destroyed.  This enables the user to make signal/slot connections without
// expanding a great effort to manage the lifetimes of those connections with
// regard to the lifetimes of all objects involved.
//
// When a slot is added to the signaler, a 'bdlmt::SignalerConnection' is
// returned, and the slot can be removed from the signaler by calling the
// 'disconnect' mthod on that signaler.  There is a second type of connection,
// 'bdlmt::SignalerScopedConnection', which is derived from
// 'bdlmt::SignalerConnection' and whose destructor will disconnect the slot.
// 'SignlaerConnection's can be copied, while 'SignalerScopedConnection's can
// only be moved.
//
// Note that the function signatures of all the callbacks managed by a signaler
// must have arguments such that there is an implicit cast from the
// corresponding argument in 'PROT' to the argument of the passed function, and
// an implicit cast from the return type of the function cast to the return
// type of 'PROT'.
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
// Internally, 'bdlmt::Signaler' stores copies of connected slot objects.  The
// copy of the slot object held by is destroyed after the slot is disconnected
// from the signaler, or after the signaler is destroyed, but the exact moment
// is unspecified.  It is only guaranteed that the lifetime of such object will
// not exceed the collective lifetime of the signaler and all connection
// objects associated with to that signaler.
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
//:   'disconnect*()', 'disconnect*AndWait()', 'release()' and 'isConnected()'
//:   member function on a connection object throws an exception.
//:
//: o no 'swap()' function throws an exception.
//:
//: o if an exception is thrown by the signaler's 'connect()' member function,
//:   that function has no effects.
//
///Comparison of 'SignalerConnection's and 'SignalerScopedConnection's
///-------------------------------------------------------------------
// In equality or ordering comparisons, two default constructed connections
// compare equivalent and a default constructed connection is never equivalent
// to a connection to a slot.  If a connection is not default constructed, it
// is equivalent only to another connection that refers to the same slot.  The
// 'less than' and 'greater than' relationships are transitive and will form an
// ordering of a set of non-equivalent connections, but the specific ordering
// is arbitrary and not predictable by the client.  Note that a connection's
// ordering does not change when it is disconnected.  Releasing a connection
// sets it to a default-constructed value.
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

#include <bslmf_allocatorargt.h>
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
#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>
#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_utility.h>  // bsl::pair

namespace BloombergLP {

// FORWARD DECLARATION
namespace bslma { class Allocator; }

namespace bdlmt {

template <class>
class Signaler_Node;
class SignalerConnection;

                             // ==================
                             // struct Signaler_NA
                             // ==================

struct Signaler_NotArg {
    // Provides a "Not an Argument" tag type.
};

                        // ===========================
                        // class Signaler_ArgumentType
                        // ===========================

template <class PROT>
struct Signaler_ArgumentType {
    // For a function prototype 'PROT' of up to 9 arguments, provide types
    // 'TypeN' where 'N' is the index of the argument, and provide types
    // 'ForwardingTypeN' which is the most convenient way to forward an
    // argument of type 'TypeN':
    //: o as 'TypeN' (in the case of some fundamental types)
    //: o as a const reference (if 'TypeN' is large and either by value or by
    //:   const reference), or
    //: o as a reference to a modifiable object, if that's how the argument was
    //:   passed in the first place.
    // Note that nothing is passed as an rvalue reference, since if there are
    // multiple slots, the argument will be moved from by the first one and
    // then unsuitable for the ones following.
    //
    // Also provide 'NA' (Not an Argument) and 'ForwardingNotArg' the type that
    // forwards it.

  private:
    // PRIVATE TYPES
    typedef typename bslmf::FunctionPointerTraits<PROT*>::ArgumentList Args;

    template <int NUM>
    struct Forward {
        // PUBLIC TYPES
        typedef typename bslmf::TypeListTypeOf<NUM, Args, Signaler_NotArg>::
                                                         TypeOrDefault ArgType;

        typedef typename bslmf::ForwardingType<ArgType>::Type Type;
    };

  public:
    // PUBLIC TYPES
    typedef typename bslmf::ForwardingType<Signaler_NotArg>::Type
                                         ForwardingNotArg;

    typedef typename Forward<1>::Type    ForwardingType1;
    typedef typename Forward<2>::Type    ForwardingType2;
    typedef typename Forward<3>::Type    ForwardingType3;
    typedef typename Forward<4>::Type    ForwardingType4;
    typedef typename Forward<5>::Type    ForwardingType5;
    typedef typename Forward<6>::Type    ForwardingType6;
    typedef typename Forward<7>::Type    ForwardingType7;
    typedef typename Forward<8>::Type    ForwardingType8;
    typedef typename Forward<9>::Type    ForwardingType9;
};

                          // =========================
                          // struct Signaler_Invocable
                          // =========================

template <class SIGNALER, class PROT>
struct Signaler_Invocable {
    // Provides a call operator for the derived class 'bdlmt::Signaler', such
    // that its call signature is identical to that of 'PROT', except that the
    // return type is 'void'.
    //
    // Note that there is a template parameter 'RET' for the return type of the
    // function to be called.  It is always discarded, but having this template
    // parameter rather than hard-coding it to 'void' allows us to at least
    // call functions with return values, provided that we're not interested in
    // the return value, without having to write a wrapper function that calls
    // them, discards the return value, and returns 'void'.
};

template <class SIGNALER, class RET>
struct Signaler_Invocable<SIGNALER, RET()> {

    // ACCESSOR
    void operator()() const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class RET, class ARG1>
struct Signaler_Invocable<SIGNALER, RET(ARG1)> {

    // ACCESSOR
    void operator()(ARG1) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2>
struct Signaler_Invocable<SIGNALER, RET(ARG1, ARG2)> {

    // ACCESSOR
    void operator()(ARG1, ARG2) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3>
struct Signaler_Invocable<SIGNALER, RET(ARG1, ARG2, ARG3)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4>
struct Signaler_Invocable<SIGNALER, RET(ARG1, ARG2, ARG3, ARG4)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5>
struct Signaler_Invocable<SIGNALER, RET(ARG1, ARG2, ARG3, ARG4, ARG5)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4, ARG5) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5,
                                     class ARG6>
struct Signaler_Invocable<SIGNALER, RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5,
                                     class ARG6,
                                     class ARG7>
struct Signaler_Invocable<SIGNALER,
                          RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5,
                                     class ARG6,
                                     class ARG7,
                                     class ARG8>
struct Signaler_Invocable<SIGNALER,
                          RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>{

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5,
                                     class ARG6,
                                     class ARG7,
                                     class ARG8,
                                     class ARG9>
struct Signaler_Invocable<
                   SIGNALER,
                   RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)
                                                                         const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

                       // ============================
                       // class Signaler_SlotNode_Base
                       // ============================

class Signaler_SlotNode_Base {
    // Provide a non-template protocol base class for 'Signaler_SlotNode' so
    // 'SignlerConnection' objects, which are not templated, can refer to and
    // manipulate 'Signaler_SlotNode' objects.

  private:
    // NOT IMPLEMENTED
    Signaler_SlotNode_Base(           const Signaler_SlotNode_Base&)
                                                          BSLS_KEYWORD_DELETED;
    Signaler_SlotNode_Base& operator=(const Signaler_SlotNode_Base&)
                                                          BSLS_KEYWORD_DELETED;

  protected:
    // PROTECTED CREATORS
    explicit
    Signaler_SlotNode_Base();
        // Create a 'Signaler_SlotNodebase' object.

    virtual ~Signaler_SlotNode_Base();
        // Virtual d'tor.

  public:
    // MANIPULATORS
    virtual void disconnect() BSLS_KEYWORD_NOEXCEPT = 0;
        // Disconnect this slot.  If the slot was already disconnected, this
        // function has no effect.
        //
        // Note that this function does not block the calling thread pending
        // completion of the slot.  There is also no guarantee that this slot
        // will not be invoked from another thread after this function
        // completes.

    virtual void disconnectAndWait() BSLS_KEYWORD_NOEXCEPT = 0;
        // Disconnect this slot and block the calling thread pending its
        // completion.  If the slot was already disconnected, this function has
        // no effect.  The behavior is undefined if this function is invoked
        // recursively from this slot, and unless the caller holds a shared
        // pointer to this object.
        //
        // Note that it is guaranteed that this slot will not be invoked after
        // this function completes.
        //
        // Note also that this function does block pending completion of this
        // slot, even if it is already disconnected.

    // ACCESSOR
    virtual bool isConnected() const BSLS_KEYWORD_NOEXCEPT = 0;
        // Return 'true' if this slot is connected to its associated signaler,
        // and 'false' otherwise.
};

                            // =======================
                            // class Signaler_SlotNode
                            // =======================

template <class PROT>
class Signaler_SlotNode : public Signaler_SlotNode_Base {
    // Dynamically-allocated container for one slot, containing a function
    // object that can be called by a signaler.  Owned by a shared pointer in a
    // skip list container in the 'Signaler_Node'.  Also referred to by weak
    // pointers from 'SignalerConnector' and 'SignalerScopedConnector' objects.

  private:
    // PRIVATE TYPES
    typedef Signaler_ArgumentType<PROT>               ArgumentType;
    typedef typename ArgumentType::ForwardingNotArg   ForwardingNotArg;
    typedef Signaler_Node<PROT>                       SignalerNode;

  public:
    // PUBLIC TYPE
    typedef bsl::pair<int, unsigned> SlotMapKey;
        // Defines a "key" used to index slots in an associative collection.
        // The first element of the pair is the slot call group; the second is
        // the slot ID.

  private:
    // PRIVATE DATA
    mutable bslmt::ReaderWriterMutex d_slotMutex;
        // The purpose of this mutex is to implement the waiting behavior of
        // the 'disconnectAndWait()' function.

    SlotMapKey                       d_slotMapKey;
        // Slot key containing the call group and the slot ID.  Used when
        // notifying the signaler about disconnection.

    bsls::AtomicBool                 d_isConnected;
        // Set to 'true' on construction, and to 'false' on disconnection.
        // Used for preventing calling a slot after it has been disconnected.

    bsl::weak_ptr<SignalerNode>      d_signalerNodePtr;
        // Weak reference to the associated signaler node.

    bsl::function<PROT>              d_func;
        // The target callback.

  private:
    // NOT IMPLEMENTED
    Signaler_SlotNode(           const Signaler_SlotNode&)
                                                          BSLS_KEYWORD_DELETED;
    Signaler_SlotNode& operator=(const Signaler_SlotNode&)
                                                          BSLS_KEYWORD_DELETED;

  private:
    // PRIVATE ACCESSORS
    void doInvoke(bsl::integral_constant<int, 0>, // arguments count tag
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg) const;
    void doInvoke(bsl::integral_constant<int, 1>, // arguments count tag
                  typename ArgumentType::ForwardingType1 arg1,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg) const;
    void doInvoke(bsl::integral_constant<int, 2>, // arguments count tag
                  typename ArgumentType::ForwardingType1 arg1,
                  typename ArgumentType::ForwardingType2 arg2,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg) const;
    void doInvoke(bsl::integral_constant<int, 3>, // arguments count tag
                  typename ArgumentType::ForwardingType1 arg1,
                  typename ArgumentType::ForwardingType2 arg2,
                  typename ArgumentType::ForwardingType3 arg3,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg) const;
    void doInvoke(bsl::integral_constant<int, 4>, // arguments count tag
                  typename ArgumentType::ForwardingType1 arg1,
                  typename ArgumentType::ForwardingType2 arg2,
                  typename ArgumentType::ForwardingType3 arg3,
                  typename ArgumentType::ForwardingType4 arg4,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg) const;
    void doInvoke(bsl::integral_constant<int, 5>, // arguments count tag
                  typename ArgumentType::ForwardingType1 arg1,
                  typename ArgumentType::ForwardingType2 arg2,
                  typename ArgumentType::ForwardingType3 arg3,
                  typename ArgumentType::ForwardingType4 arg4,
                  typename ArgumentType::ForwardingType5 arg5,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg) const;
    void doInvoke(bsl::integral_constant<int, 6>, // arguments count tag
                  typename ArgumentType::ForwardingType1 arg1,
                  typename ArgumentType::ForwardingType2 arg2,
                  typename ArgumentType::ForwardingType3 arg3,
                  typename ArgumentType::ForwardingType4 arg4,
                  typename ArgumentType::ForwardingType5 arg5,
                  typename ArgumentType::ForwardingType6 arg6,
                  ForwardingNotArg,
                  ForwardingNotArg,
                  ForwardingNotArg) const;
    void doInvoke(bsl::integral_constant<int, 7>, // arguments count tag
                  typename ArgumentType::ForwardingType1 arg1,
                  typename ArgumentType::ForwardingType2 arg2,
                  typename ArgumentType::ForwardingType3 arg3,
                  typename ArgumentType::ForwardingType4 arg4,
                  typename ArgumentType::ForwardingType5 arg5,
                  typename ArgumentType::ForwardingType6 arg6,
                  typename ArgumentType::ForwardingType7 arg7,
                  ForwardingNotArg,
                  ForwardingNotArg) const;
    void doInvoke(bsl::integral_constant<int, 8>,
                  typename ArgumentType::ForwardingType1 arg1,
                  typename ArgumentType::ForwardingType2 arg2,
                  typename ArgumentType::ForwardingType3 arg3,
                  typename ArgumentType::ForwardingType4 arg4,
                  typename ArgumentType::ForwardingType5 arg5,
                  typename ArgumentType::ForwardingType6 arg6,
                  typename ArgumentType::ForwardingType7 arg7,
                  typename ArgumentType::ForwardingType8 arg8,
                  ForwardingNotArg) const;
    void doInvoke(bsl::integral_constant<int, 9>,
                  typename ArgumentType::ForwardingType1 arg1,
                  typename ArgumentType::ForwardingType2 arg2,
                  typename ArgumentType::ForwardingType3 arg3,
                  typename ArgumentType::ForwardingType4 arg4,
                  typename ArgumentType::ForwardingType5 arg5,
                  typename ArgumentType::ForwardingType6 arg6,
                  typename ArgumentType::ForwardingType7 arg7,
                  typename ArgumentType::ForwardingType8 arg8,
                  typename ArgumentType::ForwardingType9 arg9) const;
        // Dispatch function to be called by the 'invoke' function, the first
        // argument is an 'integral_constant' containing the number of
        // specified arguments 'argN' that follow it.  Each function takes 9
        // arguments in addition to the integral constant, but the last ones of
        // type 'ForwardingNotArg' are not used.

  public:
    // CREATORS
    template <class FUNC>
    Signaler_SlotNode(const bsl::shared_ptr<SignalerNode>&     signalerNodePtr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(FUNC)  slot,
                      SlotMapKey                               slotMapKey,
                      bslma::Allocator                        *allocator);
        // Create a 'Signaler_SlotNode' object associated with signaler node at
        // the specified 'signalerNodePtr' using the specified 'slotMapKey' and
        // with the specified 'slot' callable object.  Specify an 'allocator'
        // used to supply memory.

    virtual ~Signaler_SlotNode();
        // Destroy this object.

  public:
    // MANIPULATOR
    void disconnect() BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Disconnect this slot.  If the slot was already disconnected, this
        // function has no effect.
        //
        // Note that this function does not block the calling thread pending
        // completion of the slot.  There is also no guarantee that this slot
        // will not be invoked from another thread after this function
        // completes.

    void disconnectAndWait() BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Disconnect this slot and block the calling thread pending its
        // completion.  If the slot was already disconnected, this function has
        // no effect.  The behavior is undefined if this function is invoked
        // recursively from this slot, and unless the caller holds a shared
        // pointer to this object.
        //
        // Note that it is guaranteed that this slot will not be invoked after
        // this function completes.
        //
        // Note also that this function does block pending completion of this
        // slot, even if it is already disconnected.

    void notifyDisconnected() BSLS_KEYWORD_NOEXCEPT;
        // Notify this slot that is was disconnected from its associated
        // signaler.  After this function completes, 'isConnected()' returns
        // 'false'.

    // ACCESSORS
    void invoke(typename ArgumentType::ForwardingType1 arg1,
                typename ArgumentType::ForwardingType2 arg2,
                typename ArgumentType::ForwardingType3 arg3,
                typename ArgumentType::ForwardingType4 arg4,
                typename ArgumentType::ForwardingType5 arg5,
                typename ArgumentType::ForwardingType6 arg6,
                typename ArgumentType::ForwardingType7 arg7,
                typename ArgumentType::ForwardingType8 arg8,
                typename ArgumentType::ForwardingType9 arg9) const;
        // Invoke the stored callback 'c', as if by 'c(args...)', where
        // 'args...' are the specified arguments 'arg1', 'arg2', 'arg3', etc.,
        // except that the actual number of arguments passed to 'c' is equal to
        // the number of arguments for 'PROT'.  If this slot is disconnected,
        // this function has no effect.

    bool isConnected() const BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Return 'true' if this slot is connected to its associated signaler,
        // and 'false' otherwise.
};

                            // ===================
                            // class Signaler_Node
                            // ===================

template <class PROT>
class Signaler_Node :
                    public bsl::enable_shared_from_this<Signaler_Node<PROT> > {
    // Provides the implementation of a signaler.  This object has a 1-1
    // relationship with the 'Signaler', which has a shared pointer to it.
    // This allows other objects to refer to it via shared and weak pointers.
    // This allows 'SignalerConnection' objects to outlive the
    // 'Signaler - Signaler_Node' pair, since they can test or lock weak
    // pointers to see if the 'Signaler_Node' still exists when they are trying
    // to disconnect themselves.

  private:
    // PRIVATE TYPES
    typedef Signaler_SlotNode<PROT>                     SlotNode;
    typedef typename SlotNode::SlotMapKey               SlotMapKey;
    typedef Signaler_ArgumentType<PROT>                 ArgumentType;

    typedef bdlcc::SkipList<SlotMapKey,                // [GROUP, ID] pair
                            bsl::shared_ptr<SlotNode> > KeyToSlotMap;

  private:
    // PRIVATE DATA
    mutable bslmt::ReaderWriterMutex  d_signalerMutex;
        // The purpose of this mutex is to implement the waiting behavior of
        // 'disconnectGroupAndWait()' and 'disconnectAllSlotsAndWait()'
        // functions.

    KeyToSlotMap                      d_slotMap;
        // Thread-safe collection containing slots indexed (and ordered) by
        // their respective keys.

    bsls::AtomicUint                  d_keyId;
        // For supplying 'second' members of the 'SlotMapKey' values that are
        // unique to a signaler.

    bslma::Allocator                 *d_allocator_p;
        // The allocator that was passed at construction, used for 'd_slotMap'
        // and for allocating 'SlotNode' objects.

  private:
    // NOT IMPLEMENTED
    Signaler_Node(const Signaler_Node&, bslma::Allocator * = 0)
                                                   BSLS_KEYWORD_DELETED;
    Signaler_Node& operator=(const Signaler_Node&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit
    Signaler_Node(bslma::Allocator *allocator);
        // Create a 'Signaler_Node' object having no connected slots.  Specify
        // an 'allocator' used to supply memory.
        //
        // Note that the supplied allocator must remain valid until all
        // connection objects associated with this signaler are destroyed.

  public:
    // MANIPULATORS
    template <class FUNC>
    SignalerConnection connect(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) slot,
                               int                                     group);
        // Implements 'Signaler::connect()'.
        //
        // Connect the specified 'slot', a copyable object that can be called
        // with the function signature 'PROT', or an object callable with an
        // interface matching 'PROT', to this signaler.  Optionally specify a
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
        // lvalues of types 'bsl::decay_t<ARGS>...'.  Given an lvalue 'f' of
        // type 'bsl::decay_t<FUNC>', the expression 'f(args...)' shall be well
        // formed and have a well-defined behavior.  'FUNC' must meet the
        // requirements of Destructible and CopyConstructible as specified in
        // the C++ standard.

    void disconnectAllSlots() BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::disconnectAllSlots()'.
        //
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

    void disconnectAllSlotsAndWait() BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::disconnectAllSlotsAndWait()'.
        //
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
        // already disconnected, but still executing, slots.

    void disconnectGroup(int group) BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::disconnectGroup()'.
        //
        // Disconnect all slots in the specified 'group', if any.
        //
        // Note that this function does not block the calling thread pending
        // completion of currently executing slots.  There is also no guarantee
        // that disconnected slots will not be invoked from another thread
        // after this function completes.  If such behavior is desired, use
        // 'disconnectGroupAndWait()' instead.
        //
        // Note also that if a slot is connected to this signaler during a call
        // to this function, it is unspecified whether that slot will be
        // disconnected.

    void disconnectGroupAndWait(int group) BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::disconnectGroupAndWait()'.
        //
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
        // already disconnected, but still executing, slots.

    void notifyDisconnected(SlotMapKey slotMapKey) BSLS_KEYWORD_NOEXCEPT;
        // Notify this signaler that a slot with the specified 'slotMapKey' was
        // disconnected.

  public:
    // ACCESSORS
    void invoke(
               typename ArgumentType::ForwardingType1 arg1,
               typename ArgumentType::ForwardingType2 arg2,
               typename ArgumentType::ForwardingType3 arg3,
               typename ArgumentType::ForwardingType4 arg4,
               typename ArgumentType::ForwardingType5 arg5,
               typename ArgumentType::ForwardingType6 arg6,
               typename ArgumentType::ForwardingType7 arg7,
               typename ArgumentType::ForwardingType8 arg8,
               typename ArgumentType::ForwardingType9 arg9) const;
        // Implements 'Signaler's call operator.

    bsl::size_t slotCount() const BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::slotCount()'.
};

                               // ==============
                               // class Signaler
                               // ==============

template <class PROT>
class Signaler : public Signaler_Invocable<Signaler<PROT>, PROT> {
    // This class template provides a thread-safe signaler that executes
    // connected slots when invoked via its call operator.

  public:
    // TYPES
    typedef void ResultType;    // Defines the result type of 'operator()'.  If
                                // 'PROT' has a result type that is not 'void',
                                // the return values of the calls to the slots
                                // are discarded.

  private:
    // PRIVATE DATA
    bsl::shared_ptr<Signaler_Node<PROT> >     d_signalerNodePtr;

    // FRIENDS
    friend struct Signaler_Invocable<Signaler<PROT>, PROT>;

  private:
    // NOT IMPLEMENTED
    Signaler(const Signaler&, bslma::Allocator * = 0) BSLS_KEYWORD_DELETED;
    Signaler& operator=(const Signaler&)              BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit Signaler(bslma::Allocator *basicAllocator = 0);
        // Create a 'Signaler' object having no connected slots.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
        //
        // No that the supplied allocator must remain valid until all
        // connection objects associated with this signaler are destroyed.

    ~Signaler();
        // Destroy this object.  Call 'disconnectAllSlots()'.  The behavior is
        // undefined if this function is invoked from a slot connected to this
        // signaler.

  public:
    // MANIPULATORS
    template <class FUNC>
    SignalerConnection connect(
                            BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) slot,
                            int                                     group = 0);
        // Connect the specified 'slot', a pointer to a function matching
        // 'PROT', or an object callable with an interface matching 'PROT', to
        // this signaler.  Optionally specify a 'group' used to order slots
        // upon invocation.  Return an instance of 'SignalerConnection'
        // representing the created connection.
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
        // lvalues of types 'bsl::decay_t<ARGS>...'.  Given an lvalue 'f' of
        // type 'bsl::decay_t<FUNC>', the expression 'f(args...)' shall be well
        // formed and have a well-defined behavior.  'FUNC' must meet the
        // requirements of Destructible and CopyConstructible as specified in
        // the C++ standard.

    void disconnectAllSlots() BSLS_KEYWORD_NOEXCEPT;
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

    void disconnectAllSlotsAndWait() BSLS_KEYWORD_NOEXCEPT;
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
        // already disconnected, but still executing, slots.

    void disconnectGroup(int group) BSLS_KEYWORD_NOEXCEPT;
        // Disconnect all slots in the specified 'group', if any.
        //
        // Note that this function does not block the calling thread pending
        // completion of currently executing slots.  There is also no guarantee
        // that disconnected slots will not be invoked from another thread
        // after this function completes.  If such behavior is desired, use
        // 'disconnectGroupAndWait()' instead.
        //
        // Note also that if a slot is connected to this signaler during a call
        // to this function, it is unspecified whether that slot will be
        // disconnected.

    void disconnectGroupAndWait(int group) BSLS_KEYWORD_NOEXCEPT;
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
        // already disconnected, but still executing, slots.

  public:
    // ACCESSORS
        // void operator()(ARGS... args) const;
        //
        // 'bdlmt::Signaler_Invocable', from which this 'class' inherits,
        // provides a call operator that, in C++11, would be defined and behave
        // exacly this way, except that the number of arguments is limited to
        // to 9.
        //
        // Sequentially invoke each slot connected to this signaler as if by
        // 'f_i(args...)', where 'f_i' is the i-th connected slot.  The
        // behavior is undefined if this function is invoked from a slot
        // connected to this signaler.
        //
        // Note that slots are ordered by their respective groups, and within
        // groups, by the order in which they were connected.
        //
        // Note also that the call operator does not forward rvalue references.
        // That is done explicitly to prevent invocation arguments from being
        // moved to the first slot, leaving them "empty" for all subsequent
        // slots.
        //
        // Note also that if a slot is connected to or disconnected from this
        // signaler during a call to this function, it is unspecified whether
        // that slot will be invoked.
        //
        // Note also that if execution of a slot throws an exception, the
        // invocation sequence is interrupted and the exception is propagated
        // to the caller immediately.
        //
        // NOTE: 'ARGS...' are the argument types of 'PROT'.

    bsl::size_t slotCount() const BSLS_KEYWORD_NOEXCEPT;
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
    typedef Signaler_SlotNode_Base SlotNode_Base;

  private:
    // PRIVATE DATA
    bsl::weak_ptr<SlotNode_Base> d_slotNodeBasePtr;
        // Weak reference to the associated slot.

    // FRIENDS
    template <class>
    friend class Signaler_Node;
    friend bool operator<(const SignalerConnection&,
                          const SignalerConnection&) BSLS_KEYWORD_NOEXCEPT;

  private:
    // PRIVATE CREATORS
    explicit
    SignalerConnection(const bsl::shared_ptr<SlotNode_Base>& slotNodeBasePtr)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create 'SignalerConnection' object weakly linked to the specified
        // 'slotNodeBasePtr'.

  public:
    // CREATORS
    SignalerConnection() BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerConnection' object having no associated slot.

    SignalerConnection(const SignalerConnection& original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerConnection' object that refers to and assumes
        // management of the same slot (if any) as the specified 'original'
        // object.

    SignalerConnection(bslmf::MovableRef<SignalerConnection> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerConnection' object that refers to and assumes
        // management of the same slot (if any) as the specified 'original'
        // object, and reset 'original' to a default-constructed state.

  public:
    // MANIPULATORS
    SignalerConnection&
    operator=(const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection.  Return '*this'.

    SignalerConnection&
    operator=(bslmf::MovableRef<SignalerConnection> rhs) BSLS_KEYWORD_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection, and reset 'rhs' to a
        // default-constructed state.  Return '*this'.

    void disconnect() BSLS_KEYWORD_NOEXCEPT;
        // Disconnect the associated slot.  If '*this' does not have an
        // associated slot or the slot was already disconnected, this function
        // has no effect.
        //
        // Note that this function does not block the calling thread pending
        // completion of the associated slot.  There is also no guarantee that
        // the disconnected slot will not be invoked from another thread after
        // this function completes.  If such behavior is desired, use
        // 'disconnectndWait()' instead.

    void disconnectAndWait() BSLS_KEYWORD_NOEXCEPT;
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

    SignalerConnection release() BSLS_KEYWORD_NOEXCEPT;
        // Disassociate this connection object from its associated slot, if
        // any, and reset '*this' to a default-constructed state.  Return a
        // copy of '*this' in the state that it was in prior to release.

    void swap(SignalerConnection& other) BSLS_KEYWORD_NOEXCEPT;
        // Swap the contents of '*this' and the specified 'other'.

  public:
    // ACCESSORS
    bool isConnected() const BSLS_KEYWORD_NOEXCEPT;
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
                                                          BSLS_KEYWORD_DELETED;
    SignalerScopedConnection& operator=(const SignalerScopedConnection&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    SignalerScopedConnection() BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerScopedConnection' object having no associated
        // slot.

    SignalerScopedConnection(
                          bslmf::MovableRef<SignalerScopedConnection> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerScopedConnection' that refers to and assumes
        // management of the same slot (if any) as the specified 'original'
        // object, and reset 'original' to a default-constructed state.

    SignalerScopedConnection(const SignalerConnection& connection)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Create a 'SignalerScopedConnection' object that refers to and
        // assumes management of the same slot (if any) as the specified
        // 'connection' object.

    SignalerScopedConnection(bslmf::MovableRef<SignalerConnection> connection)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Create a 'SignalerScopedConnection' object that refers to and
        // assumes management of the same slot (if any) as the specified
        // 'connection' object, and reset 'connection' to a default-
        // constructed state.

    ~SignalerScopedConnection();
        // Destroy this object.  Call 'disconnect()'.

  public:
    // MANIPULATORS
    SignalerScopedConnection&
    operator=(bslmf::MovableRef<SignalerScopedConnection> rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection, and reset 'rhs' to a
        // default-constructed state.  If, prior to this call '*this' has an
        // associated slot, disconnect that slot by a call to 'disconnect()'.
        // Return '*this'.

    SignalerScopedConnection&
    operator=(const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection.  If, prior to this call
        // '*this' has an associated slot, disconnect that slot by a call to
        // 'disconnect()'.  Return '*this'.

    SignalerScopedConnection&
    operator=(bslmf::MovableRef<SignalerConnection> rhs) BSLS_KEYWORD_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection, and reset 'rhs' to a
        // default-constructed state.  If, prior to this call '*this' has an
        // associated slot, disconnect that slot by a call to 'disconnect()'.
        // Return '*this'.

    void swap(SignalerScopedConnection& other) BSLS_KEYWORD_NOEXCEPT;
        // Swap the contents of '*this' and the specified 'other'.
};

// FREE OPERATORS
bool operator==(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' and 'rhs' referring to the same
    // slot and 'false' otherwise.

bool operator!=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'false' if the specified 'lhs' and 'rhs' referring to the same
    // slot and 'true' otherwise.

bool operator<(const SignalerConnection& lhs,
               const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' is less than the specified 'rhs'
    // and 'false' otherwise.

bool operator>(const SignalerConnection& lhs,
               const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' is grater than the specified 'rhs'
    // and 'false' otherwise.

bool operator<=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' is less than or equal to the
    // specified 'rhs' and 'false' otherwise.

bool operator>=(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT;
    // Return 'true' if the specified 'lhs' is greater than or equal to the
    // specified 'rhs' and 'false' otherwise.

// FREE FUNCTIONS
void swap(SignalerConnection& a,
          SignalerConnection& b) BSLS_KEYWORD_NOEXCEPT;
    // Swap the contents of the specified 'a' and 'b'.

void swap(SignalerScopedConnection& a,
          SignalerScopedConnection& b) BSLS_KEYWORD_NOEXCEPT;
    // Swap the contents of the specified 'a' and 'b'.

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                         // -------------------------
                         // struct Signaler_Invocable
                         // -------------------------

template <class SIGNALER, class RET>
inline
void Signaler_Invocable<SIGNALER, RET()>::operator()() const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                                                            Signaler_NotArg(),
                                                            Signaler_NotArg(),
                                                            Signaler_NotArg(),
                                                            Signaler_NotArg(),
                                                            Signaler_NotArg(),
                                                            Signaler_NotArg(),
                                                            Signaler_NotArg(),
                                                            Signaler_NotArg(),
                                                            Signaler_NotArg());
}

template <class SIGNALER, class RET, class ARG1>
inline
void Signaler_Invocable<SIGNALER, RET(ARG1)>::operator()(ARG1 arg1) const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                        bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(arg1),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg());
}

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2>
inline
void Signaler_Invocable<SIGNALER, RET(ARG1, ARG2)>::operator()(ARG1 arg1,
                                                               ARG2 arg2) const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                        bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(arg1),
                        bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(arg2),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg());
}

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3>
inline
void Signaler_Invocable<SIGNALER, RET(ARG1, ARG2, ARG3)>::operator()(
                                                               ARG1 arg1,
                                                               ARG2 arg2,
                                                               ARG3 arg3) const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                        bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(arg1),
                        bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(arg2),
                        bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(arg3),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg());
}

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4>
inline
void Signaler_Invocable<SIGNALER, RET(ARG1, ARG2, ARG3, ARG4)>::operator()(
                                                               ARG1 arg1,
                                                               ARG2 arg2,
                                                               ARG3 arg3,
                                                               ARG4 arg4) const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                        bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(arg1),
                        bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(arg2),
                        bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(arg3),
                        bslmf::ForwardingTypeUtil<ARG4>::forwardToTarget(arg4),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg());
}

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5>
inline
void Signaler_Invocable<SIGNALER, RET(ARG1, ARG2, ARG3, ARG4, ARG5)>::
                                                    operator()(ARG1 arg1,
                                                               ARG2 arg2,
                                                               ARG3 arg3,
                                                               ARG4 arg4,
                                                               ARG5 arg5) const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                       bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(arg1),
                       bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(arg2),
                       bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(arg3),
                       bslmf::ForwardingTypeUtil<ARG4>::forwardToTarget(arg4),
                       bslmf::ForwardingTypeUtil<ARG5>::forwardToTarget(arg5),
                       Signaler_NotArg(),
                       Signaler_NotArg(),
                       Signaler_NotArg(),
                       Signaler_NotArg());
}

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5,
                                     class ARG6>
inline
void Signaler_Invocable<SIGNALER, RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)>::
                                                   operator()(ARG1 arg1,
                                                              ARG2 arg2,
                                                              ARG3 arg3,
                                                              ARG4 arg4,
                                                              ARG5 arg5,
                                                              ARG6 arg6) const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                        bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(arg1),
                        bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(arg2),
                        bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(arg3),
                        bslmf::ForwardingTypeUtil<ARG4>::forwardToTarget(arg4),
                        bslmf::ForwardingTypeUtil<ARG5>::forwardToTarget(arg5),
                        bslmf::ForwardingTypeUtil<ARG6>::forwardToTarget(arg6),
                        Signaler_NotArg(),
                        Signaler_NotArg(),
                        Signaler_NotArg());
}

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5,
                                     class ARG6,
                                     class ARG7>
inline
void Signaler_Invocable<SIGNALER,
                        RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)>::
                                                    operator()(ARG1 arg1,
                                                               ARG2 arg2,
                                                               ARG3 arg3,
                                                               ARG4 arg4,
                                                               ARG5 arg5,
                                                               ARG6 arg6,
                                                               ARG7 arg7) const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                        bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(arg1),
                        bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(arg2),
                        bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(arg3),
                        bslmf::ForwardingTypeUtil<ARG4>::forwardToTarget(arg4),
                        bslmf::ForwardingTypeUtil<ARG5>::forwardToTarget(arg5),
                        bslmf::ForwardingTypeUtil<ARG6>::forwardToTarget(arg6),
                        bslmf::ForwardingTypeUtil<ARG7>::forwardToTarget(arg7),
                        Signaler_NotArg(),
                        Signaler_NotArg());
}

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5,
                                     class ARG6,
                                     class ARG7,
                                     class ARG8>
inline
void Signaler_Invocable<SIGNALER,
                        RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>::
                                                    operator()(ARG1 arg1,
                                                               ARG2 arg2,
                                                               ARG3 arg3,
                                                               ARG4 arg4,
                                                               ARG5 arg5,
                                                               ARG6 arg6,
                                                               ARG7 arg7,
                                                               ARG8 arg8) const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                        bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(arg1),
                        bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(arg2),
                        bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(arg3),
                        bslmf::ForwardingTypeUtil<ARG4>::forwardToTarget(arg4),
                        bslmf::ForwardingTypeUtil<ARG5>::forwardToTarget(arg5),
                        bslmf::ForwardingTypeUtil<ARG6>::forwardToTarget(arg6),
                        bslmf::ForwardingTypeUtil<ARG7>::forwardToTarget(arg7),
                        bslmf::ForwardingTypeUtil<ARG8>::forwardToTarget(arg8),
                        Signaler_NotArg());
}

template <class SIGNALER, class RET, class ARG1,
                                     class ARG2,
                                     class ARG3,
                                     class ARG4,
                                     class ARG5,
                                     class ARG6,
                                     class ARG7,
                                     class ARG8,
                                     class ARG9>
inline
void
Signaler_Invocable<SIGNALER,
                   RET(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>::
                                                    operator()(ARG1 arg1,
                                                               ARG2 arg2,
                                                               ARG3 arg3,
                                                               ARG4 arg4,
                                                               ARG5 arg5,
                                                               ARG6 arg6,
                                                               ARG7 arg7,
                                                               ARG8 arg8,
                                                               ARG9 arg9) const
{
    static_cast<const SIGNALER *>(this)->d_signalerNodePtr->invoke(
                       bslmf::ForwardingTypeUtil<ARG1>::forwardToTarget(arg1),
                       bslmf::ForwardingTypeUtil<ARG2>::forwardToTarget(arg2),
                       bslmf::ForwardingTypeUtil<ARG3>::forwardToTarget(arg3),
                       bslmf::ForwardingTypeUtil<ARG4>::forwardToTarget(arg4),
                       bslmf::ForwardingTypeUtil<ARG5>::forwardToTarget(arg5),
                       bslmf::ForwardingTypeUtil<ARG6>::forwardToTarget(arg6),
                       bslmf::ForwardingTypeUtil<ARG7>::forwardToTarget(arg7),
                       bslmf::ForwardingTypeUtil<ARG8>::forwardToTarget(arg8),
                       bslmf::ForwardingTypeUtil<ARG9>::forwardToTarget(arg9));
}

                            // -----------------------
                            // class Signaler_SlotNode
                            // -----------------------


// PRIVATE ACCESSORS
template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(bsl::integral_constant<int, 0>,
                                       ForwardingNotArg,
                                       ForwardingNotArg,
                                       ForwardingNotArg,
                                       ForwardingNotArg,
                                       ForwardingNotArg,
                                       ForwardingNotArg,
                                       ForwardingNotArg,
                                       ForwardingNotArg,
                                       ForwardingNotArg) const
{
    d_func();
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(
                                   bsl::integral_constant<int, 1>,
                                   typename ArgumentType::ForwardingType1 arg1,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg) const
{
    // NOTE: Does not forward

    d_func(arg1);
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(
                                   bsl::integral_constant<int, 2>,
                                   typename ArgumentType::ForwardingType1 arg1,
                                   typename ArgumentType::ForwardingType2 arg2,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg) const
{
    // NOTE: Does not forward

    d_func(arg1, arg2);
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(
                                   bsl::integral_constant<int, 3>,
                                   typename ArgumentType::ForwardingType1 arg1,
                                   typename ArgumentType::ForwardingType2 arg2,
                                   typename ArgumentType::ForwardingType3 arg3,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg) const
{
    // NOTE: Does not forward

    d_func(arg1, arg2, arg3);
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(
                                   bsl::integral_constant<int, 4>,
                                   typename ArgumentType::ForwardingType1 arg1,
                                   typename ArgumentType::ForwardingType2 arg2,
                                   typename ArgumentType::ForwardingType3 arg3,
                                   typename ArgumentType::ForwardingType4 arg4,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg) const
{
    // NOTE: Does not forward

    d_func(arg1, arg2, arg3, arg4);
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(
                                   bsl::integral_constant<int, 5>,
                                   typename ArgumentType::ForwardingType1 arg1,
                                   typename ArgumentType::ForwardingType2 arg2,
                                   typename ArgumentType::ForwardingType3 arg3,
                                   typename ArgumentType::ForwardingType4 arg4,
                                   typename ArgumentType::ForwardingType5 arg5,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg) const
{
    // NOTE: Does not forward

    d_func(arg1, arg2, arg3, arg4, arg5);
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(
                                   bsl::integral_constant<int, 6>,
                                   typename ArgumentType::ForwardingType1 arg1,
                                   typename ArgumentType::ForwardingType2 arg2,
                                   typename ArgumentType::ForwardingType3 arg3,
                                   typename ArgumentType::ForwardingType4 arg4,
                                   typename ArgumentType::ForwardingType5 arg5,
                                   typename ArgumentType::ForwardingType6 arg6,
                                   ForwardingNotArg,
                                   ForwardingNotArg,
                                   ForwardingNotArg) const
{
    // NOTE: Does not forward

    d_func(arg1, arg2, arg3, arg4, arg5, arg6);
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(
                                   bsl::integral_constant<int, 7>,
                                   typename ArgumentType::ForwardingType1 arg1,
                                   typename ArgumentType::ForwardingType2 arg2,
                                   typename ArgumentType::ForwardingType3 arg3,
                                   typename ArgumentType::ForwardingType4 arg4,
                                   typename ArgumentType::ForwardingType5 arg5,
                                   typename ArgumentType::ForwardingType6 arg6,
                                   typename ArgumentType::ForwardingType7 arg7,
                                   ForwardingNotArg,
                                   ForwardingNotArg) const
{
    // NOTE: Does not forward

    d_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(
                                   bsl::integral_constant<int, 8>,
                                   typename ArgumentType::ForwardingType1 arg1,
                                   typename ArgumentType::ForwardingType2 arg2,
                                   typename ArgumentType::ForwardingType3 arg3,
                                   typename ArgumentType::ForwardingType4 arg4,
                                   typename ArgumentType::ForwardingType5 arg5,
                                   typename ArgumentType::ForwardingType6 arg6,
                                   typename ArgumentType::ForwardingType7 arg7,
                                   typename ArgumentType::ForwardingType8 arg8,
                                   ForwardingNotArg) const
{
    // NOTE: Does not forward

    d_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::doInvoke(
                                   bsl::integral_constant<int, 9>,
                                   typename ArgumentType::ForwardingType1 arg1,
                                   typename ArgumentType::ForwardingType2 arg2,
                                   typename ArgumentType::ForwardingType3 arg3,
                                   typename ArgumentType::ForwardingType4 arg4,
                                   typename ArgumentType::ForwardingType5 arg5,
                                   typename ArgumentType::ForwardingType6 arg6,
                                   typename ArgumentType::ForwardingType7 arg7,
                                   typename ArgumentType::ForwardingType8 arg8,
                                   typename ArgumentType::ForwardingType9 arg9)
                                                                          const
{
    // NOTE: Does not forward

    d_func(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

// CREATORS
template <class PROT>
template <class FUNC>
Signaler_SlotNode<PROT>::Signaler_SlotNode(
                      const bsl::shared_ptr<SignalerNode>&     signalerNodePtr,
                      BSLS_COMPILERFEATURES_FORWARD_REF(FUNC)  func,
                      SlotMapKey                               slotMapKey,
                      bslma::Allocator                        *allocator)
: Signaler_SlotNode_Base()
, d_slotMutex()
, d_slotMapKey(slotMapKey)
, d_isConnected(true)
, d_signalerNodePtr(signalerNodePtr)
, d_func(bsl::allocator_arg,
         allocator,
         BSLS_COMPILERFEATURES_FORWARD(FUNC, func))
{
    BSLS_ASSERT(signalerNodePtr);
    BSLS_ASSERT(allocator);
}

template <class PROT>
Signaler_SlotNode<PROT>::~Signaler_SlotNode()
{
    // Note this d'tor destroys 'd_signalerNodePtr' and 'd_func'.
}

// MANIPULATORS
template <class PROT>
void Signaler_SlotNode<PROT>::disconnect() BSLS_KEYWORD_NOEXCEPT
{
    if (!d_isConnected.testAndSwap(true, false)) {
        // Already disconnected.  Do nothing.

        return;                                                       // RETURN
    }

    // Notify the associated signaler

    bsl::shared_ptr<SignalerNode> signalerNodePtr = d_signalerNodePtr.lock();
    if (signalerNodePtr) {
        signalerNodePtr->notifyDisconnected(d_slotMapKey);
    }
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::disconnectAndWait() BSLS_CPP11_NOEXCEPT
{
    // Disconnect the slot.

    disconnect();

    // Synchronize with the call operator.

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> lock(&d_slotMutex); // LOCK
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::notifyDisconnected() BSLS_CPP11_NOEXCEPT
{
    d_isConnected = false;
}

// ACCESSORS
template <class PROT>
inline
void Signaler_SlotNode<PROT>::invoke(
                             typename ArgumentType::ForwardingType1 arg1,
                             typename ArgumentType::ForwardingType2 arg2,
                             typename ArgumentType::ForwardingType3 arg3,
                             typename ArgumentType::ForwardingType4 arg4,
                             typename ArgumentType::ForwardingType5 arg5,
                             typename ArgumentType::ForwardingType6 arg6,
                             typename ArgumentType::ForwardingType7 arg7,
                             typename ArgumentType::ForwardingType8 arg8,
                             typename ArgumentType::ForwardingType9 arg9) const
{

    // The only way we are called is from a 'Signaler', which should exist
    // throughout the call and be holding a shared ptr to the 'Signaler_Node'.

    BSLS_ASSERT(!d_signalerNodePtr.expired());

    // Hold this mutex (in read mode), so that 'disconnectAndWait()' can
    // synchronize with the call operator

    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> lock(&d_slotMutex);  // LOCK

    if (!d_isConnected) {
        // The slot was evidently disconnected by another thread.  Do nothing.

        return;                                                       // RETURN
    }

    typedef typename bslmf::FunctionPointerTraits<PROT*>::ArgumentList Args;

    doInvoke(bsl::integral_constant<int, Args::LENGTH>(),
             arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

template <class PROT>
inline
bool Signaler_SlotNode<PROT>::isConnected() const BSLS_CPP11_NOEXCEPT
{
    return d_isConnected;
}

                            // -------------------
                            // class Signaler_Node
                            // -------------------

// CREATORS
template <class PROT>
Signaler_Node<PROT>::Signaler_Node(bslma::Allocator *allocator)
: d_signalerMutex()
, d_slotMap(allocator)
, d_keyId(0)
, d_allocator_p(allocator)
{
    BSLS_ASSERT(allocator);
}

// MANIPULATORS
template <class PROT>
inline
void Signaler_Node<PROT>::invoke(
                             typename ArgumentType::ForwardingType1 arg1,
                             typename ArgumentType::ForwardingType2 arg2,
                             typename ArgumentType::ForwardingType3 arg3,
                             typename ArgumentType::ForwardingType4 arg4,
                             typename ArgumentType::ForwardingType5 arg5,
                             typename ArgumentType::ForwardingType6 arg6,
                             typename ArgumentType::ForwardingType7 arg7,
                             typename ArgumentType::ForwardingType8 arg8,
                             typename ArgumentType::ForwardingType9 arg9) const
{
    // Hold this mutex (in read mode), so that 'disconnectGroupAndWait()' (or
    // 'disconnectAllSlotsAndWait()') can synchonize with the call operator by
    // momemntarily locking it for write.

    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> lock(&d_signalerMutex);

    // 'slotHandle' points in to a node in the skiplist, which has a reference
    // count to prevent it from being deallocated & destroyed by another thread
    // as long as we have 'slotHandle' referring to it.  The node may be
    // removed from the skip list, though, in which case its 'next' pointers
    // will be null.

    typename KeyToSlotMap::PairHandle slotHandle;
    if (d_slotMap.front(&slotHandle) != 0) {
        // No slots.  Do nothing.

        return;                                                       // RETURN
    }

    do {
        const SlotNode   *slotNodePtr = &*slotHandle.data();
        const SlotMapKey  slotMapKey  = slotHandle.key();

        // invoke the slot

        slotNodePtr->invoke(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8,
                                                                         arg9);

        if (0 != d_slotMap.skipForward(&slotHandle)) {
            // 'slot' has been removed from the skip list and we can't use the
            // 'next' pointers to get to the next node, but we can still access
            // 'slotMapKey' to tell us where we were and directly look up the
            // next slot after that.

            if (0 != d_slotMap.findUpperBound(&slotHandle, slotMapKey)) {
                // No slots left.  We're done.

                return;                                               // RETURN
            }
        }
    } while (slotHandle);
}

template <class PROT>
template <class FUNC>
SignalerConnection
Signaler_Node<PROT>::connect(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func,
                             int                                     group)
{
    // create a key the slot will be indexed by

    const SlotMapKey slotMapKey(group, ++d_keyId);

    // create a slot

    bsl::shared_ptr<SlotNode> slotNodePtr =
                         bsl::allocate_shared<SlotNode>(
                                     d_allocator_p,
                                     this->shared_from_this(),
                                     BSLS_COMPILERFEATURES_FORWARD(FUNC, func),
                                     slotMapKey,
                                     d_allocator_p);

    // connect the slot

    d_slotMap.addR(slotMapKey, slotNodePtr);

    // return the connection

    return SignalerConnection(slotNodePtr);
}

template <class PROT>
void Signaler_Node<PROT>::disconnectGroup(int group) BSLS_KEYWORD_NOEXCEPT
{
    typename KeyToSlotMap::PairHandle slotHandle;

    while (d_slotMap.findLowerBound(&slotHandle, bsl::make_pair(group, 0)) == 0
           && slotHandle.key().first == group) {
        // disconnect all slots in the specified 'group', one by one

        // notify the slot it's being disconnected

        slotHandle.data()->notifyDisconnected();

        // remove the slot from the collection

        d_slotMap.remove(slotHandle);
    }

    // TODO: This function complexity is N*logN. Should investigate if it can
    //       be made linear.
}

template <class PROT>
void Signaler_Node<PROT>::disconnectAllSlots() BSLS_KEYWORD_NOEXCEPT
{
    typename KeyToSlotMap::PairHandle slotHandle;

    while (d_slotMap.front(&slotHandle) == 0) {
        // disconnect all slots in the collection, one by one

        // notify the slot it's being disconnected

        slotHandle.data()->notifyDisconnected();

        // remove the slot from the collection

        d_slotMap.remove(slotHandle);
    }
}

template <class PROT>
void Signaler_Node<PROT>::disconnectAllSlotsAndWait() BSLS_KEYWORD_NOEXCEPT
{
    // disconnect slots

    disconnectAllSlots();

    // synchronize with the call operator

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> lock(&d_signalerMutex);
}

template <class PROT>
void Signaler_Node<PROT>::disconnectGroupAndWait(int group)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    // disconnect slots

    disconnectGroup(group);

    // synchronize with the call operator

    bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> lock(&d_signalerMutex);
}

template <class PROT>
void Signaler_Node<PROT>::notifyDisconnected(SlotMapKey slotMapKey)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    typename KeyToSlotMap::PairHandle slotHandle;

    if (d_slotMap.find(&slotHandle, slotMapKey) != 0) {
        // Slot was already removed, probably by some form of 'disconnect*'
        // called on the 'Signaler'.  Do nothing.

        return;                                                       // RETURN
    }

    // remove the slot from the collection

    d_slotMap.remove(slotHandle);
}

// ACCESSORS
template <class PROT>
inline
bsl::size_t Signaler_Node<PROT>::slotCount() const BSLS_KEYWORD_NOEXCEPT
{
    return d_slotMap.length();
}

                               // --------------
                               // class Signaler
                               // --------------

// CREATORS
template <class PROT>
Signaler<PROT>::Signaler(bslma::Allocator *basicAllocator)
: d_signalerNodePtr(bsl::allocate_shared<Signaler_Node<PROT> >(
                                    basicAllocator,
                                    bslma::Default::allocator(basicAllocator)))
{
    // NOTHING
}

template <class PROT>
inline
Signaler<PROT>::~Signaler()
{
    d_signalerNodePtr->disconnectAllSlots();
}

// MANIPULATORS
template <class PROT>
template <class FUNC>
inline
SignalerConnection Signaler<PROT>::connect(
                                 BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func,
                                 int                                     group)
{
    return d_signalerNodePtr->connect(
                                     BSLS_COMPILERFEATURES_FORWARD(FUNC, func),
                                     group);
}

template <class PROT>
inline
void Signaler<PROT>::disconnectAllSlots() BSLS_KEYWORD_NOEXCEPT
{
    d_signalerNodePtr->disconnectAllSlots();
}

template <class PROT>
inline
void Signaler<PROT>::disconnectAllSlotsAndWait() BSLS_KEYWORD_NOEXCEPT
{
    d_signalerNodePtr->disconnectAllSlotsAndWait();
}

template <class PROT>
inline
void Signaler<PROT>::disconnectGroup(int group) BSLS_KEYWORD_NOEXCEPT
{
    d_signalerNodePtr->disconnectGroup(group);
}

template <class PROT>
inline
void Signaler<PROT>::disconnectGroupAndWait(int group) BSLS_KEYWORD_NOEXCEPT
{
    d_signalerNodePtr->disconnectGroupAndWait(group);
}

// ACCESSORS
template <class PROT>
inline
bsl::size_t Signaler<PROT>::slotCount() const BSLS_KEYWORD_NOEXCEPT
{
    return d_signalerNodePtr->slotCount();
}

// FREE OPERATORS
inline
bool operator<(const SignalerConnection& lhs,
               const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.d_slotNodeBasePtr.owner_before(rhs.d_slotNodeBasePtr);
}

// FREE FUNCTIONS
inline
void swap(SignalerConnection& a,
          SignalerConnection& b) BSLS_KEYWORD_NOEXCEPT
{
    a.swap(b);
}

inline
void swap(SignalerScopedConnection& a,
          SignalerScopedConnection& b) BSLS_KEYWORD_NOEXCEPT
{
    a.swap(b);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2019
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
