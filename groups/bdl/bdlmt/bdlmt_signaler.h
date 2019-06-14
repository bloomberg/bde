// bdlmt_signaler.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLMT_SIGNALER
#define INCLUDED_BDLMT_SIGNALER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of a managed signals and slots system.
//
//@CLASSES:
//  bdlmt::Signaler:                a signaler
//  bdlmt::SignalerConnection:      signaler/slot connection
//  bdlmt::SignalerConnectionGuard: RAII signaler/slot connection
//
//@DESCRIPTION: This component provides the template class
// 'bdlmt::Signaler<PROT>', an implementation of a managed signal and slots
// system for the void returning function prototype 'PROT'.  Each signaler
// represents a callback with multiple targets (called "slots") which are
// invoked in a known order when the signaler is invoked (called being
// "emitted").
//
// A slot being connected to a signaler is represented by a
// 'bdlmt::SignalerConnection' which can be used to disconnect that connection
// at any time, but can also be discarded if managing the lifetime of the
// individual connection is not needed.  A guard to disconnect a slot on its
// destruction is available in 'bdlmt::SignalerConnectionGuard'.
//
// Signalers and the slots connected to them are all managed.  Any connections
// will be automatically disconnected when a 'Signaler' is destroyed, or when
// explicitly disconnected, and all internally allocated resources will be
// destroyed when no more references to them remain.  This enables the user to
// make signaler/slot connections and emit signals without expanding effort on
// managing the lifetimes of any of the involved objects.
//
///Slot Object Requirements
///------------------------
// Slots connected to a signaler 'Signaler<PROT>' must be callable and copyable
// objects that may be passed to the constructor of 'bsl::function<PROT>'.
// I.e. a slot must be callable with the same arguments as 'PROT', and if a
// slot returns a value it will be discarded.
//
///Call groups
///-----------
// Slots are free to have side effects, which means that some slots may have to
// be called before others even if they are not connected in that order.
// 'bdlmt::Signaler' allows slots to be placed into groups that are ordered in
// some way.  Group values are integers, and are ordered by the 'integer <'
// relation.  By default, all connected slots have the group value set to 0.
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
// copy of the slot object is destroyed after the slot is disconnected from the
// signaler, or after the signaler is destroyed, but the exact moment is
// unspecified.  It is only guaranteed that the lifetime of such object will
// not exceed the collective lifetime of the signaler and all connection
// objects associated with to that signaler.
//
///Comparison of 'SignalerConnection's
///-----------------------------------
// Ordering comparisons of 'SignalerConnection's are transitve and are provided
// to facilitate their being stored in an associative container.  The ordering
// of a 'SignalerConnection' does not change when it is disconnected.
//
// In equality comparisons, two default constructed connections compare
// equivalent and a default constructed connection is never equivalent to a
// connection to a slot.  If a connection is not default constructed, it is
// equivalent only to another connection that refers to the same slot.
//
///Thread safety
///-------------
// 'bdlmt::Signaler' is fully thread-safe, meaning that multiple threads may
// use their own instances of the class or use a shared instance without
// further synchronization.
//
// With the exception of assignment operators, 'swap()', 'reset()' and
// 'release()' member functions, 'bdlmt::SignalerConnection' and
// 'bdlmt::SignalerConnectionGuard' are thread-safe, meaning that multiple
// threads may use their own instances of the class or use a shared instance
// without further synchronization.
//
// It is safe to access or modify two distinct connection objects
// simultaneously, each from a separate thread, even if they represent the same
// slot connection.
//
///Usage
///-----
// Suppose we want to implement a GUI button class that allows users to
// keep track of its 'press' events.
//
// First, we declare the 'class':
//..
//  class Button {
//      // A pretend GUI button.
//
//      // DATA
//      int d_numPresses;
//
//    public:
//      // TYPES
//      typedef bsl::function<void(int)> OnPressSlotType;
//          // Slot argument is the number of times the button has been
//          // pressed.
//
//    private:
//      // PRIVATE DATA
//      bdlmt::Signaler<void(int)> d_onPress;
//          // Signaler argument is the number of times the button has been
//          // pressed.
//
//    public:
//      // CREATORS
//      Button();
//          // Construct a 'Button' object.
//
//      // MANIPULATORS
//      bdlmt::SignalerConnection onPressConnect(const OnPressSlotType& slot);
//          // Connect the specified 'slot' to this button.
//
//      void press();
//          // Simulate user pressing on GUI button.
//  };
//..
// Then, we define its methods:
//..
//  // CREATORS
//  Button::Button()
//  : d_numPresses(0)
//  {
//  }
//
//  // MANIPULATORS
//  bdlmt::SignalerConnection Button::onPressConnect(
//                                                 const OnPressSlotType& slot)
//  {
//      return d_onPress.connect(slot);
//  }
//
//  void Button::press()
//  {
//      d_onPress(++d_numPresses);
//  }
//..
// Next, we provide an event handler callback printing its argument, which the
// class will pass the number of times the button has been pressed:
//..
//  void showPresses(int numPresses)
//  {
//      bsl::cout << "Button pressed " << numPresses << " times.\n";
//  }
//..
// Then, in 'main', create a button and subscribe to its events.
//..
//  u::Button                 button;
//  bdlmt::SignalerConnection connection = button.onPressConnect(
//                                                            &u::showPresses);
//..
// Next the button is "pressed", we will receive a notification.
//..
//  button.press();
//..
// Now, we see the following message:
//..
//  Button pressed 1 times.
//..
// Finally, unsubscribe from button's events when we don't want to receive
// notifications anymore.  (If we didn't call 'disconnect', 'button' would
// clean up all the allocated resources when it went out of scope):
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
#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>      // bsl::size_t
#include <bsl_functional.h>
#include <bsl_memory.h>
#include <bsl_utility.h>      // bsl::pair

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
    // 'ForwardingTypeN' which is the most convenient way to forward the 'Nth'
    // argument.
    //: o as the type of argument N itself (in the case of some fundamental
    //:   types)
    //: o as a const reference (if 'TypeN' is large and either by value or by
    //:   const reference), or
    //: o as a reference to a modifiable object, if that's how the argument was
    //:   passed in the first place.
    // Note that nothing is passed as an rvalue reference, since if there are
    // multiple slots (usually the case), the argument will be moved from by
    // the first one and then unsuitable to be used by the ones following.
    //
    // Also provide 'ForwardingNotArg' the type that forwards
    // 'Signaler_NotArg'.

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
    // that its call signature is identical to that of 'PROT'.
};

template <class SIGNALER>
struct Signaler_Invocable<SIGNALER, void()> {

    // ACCESSOR
    void operator()() const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class ARG1>
struct Signaler_Invocable<SIGNALER, void(ARG1)> {

    // ACCESSOR
    void operator()(ARG1) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class ARG1,
                          class ARG2>
struct Signaler_Invocable<SIGNALER, void(ARG1, ARG2)> {

    // ACCESSOR
    void operator()(ARG1, ARG2) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3>
struct Signaler_Invocable<SIGNALER, void(ARG1, ARG2, ARG3)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4>
struct Signaler_Invocable<SIGNALER, void(ARG1, ARG2, ARG3, ARG4)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4,
                          class ARG5>
struct Signaler_Invocable<SIGNALER, void(ARG1, ARG2, ARG3, ARG4, ARG5)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4, ARG5) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4,
                          class ARG5,
                          class ARG6>
struct Signaler_Invocable<SIGNALER, void(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4,
                          class ARG5,
                          class ARG6,
                          class ARG7>
struct Signaler_Invocable<SIGNALER,
                          void(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4,
                          class ARG5,
                          class ARG6,
                          class ARG7,
                          class ARG8>
struct Signaler_Invocable<
                        SIGNALER,
                        void(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)> {

    // ACCESSOR
    void operator()(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8) const;
        // Call the functions held in all slot holders, in the order of group
        // numbers and with the ordering within one group being the order in
        // which connections were made, passing the number and type of
        // arguments passed to this function.
};

template <class SIGNALER, class ARG1,
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
                  void(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)> {

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
    // 'SignalerConnection' objects, which are not templated, can refer to and
    // manipulate 'Signaler_SlotNode' objects.

  protected:
    // PROTECTED CREATORS
    virtual ~Signaler_SlotNode_Base();
        // Virtual d'tor.

  public:
    // MANIPULATORS
    virtual void disconnect(bool wait) BSLS_KEYWORD_NOEXCEPT = 0;
        // Disconnect this slot and, if the specified 'wait' is 'true', block
        // the calling thread pending the completion of calls to the signaler
        // made by any other threads.  If 'wait' is 'false', return after
        // disconnecting without waiting.  If the slot was already
        // disconnected, this function has no effect.  The behavior is
        // undefined if this function is invoked from a slot with 'true' passed
        // to 'wait'.  Note that it is guaranteed that this slot will not be
        // invoked by a call to the same signaler that begins after this
        // function completes, whether 'wait' is 'true' or not'

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
    // pointers from 'SignalerConnection' objects.

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
    SlotMapKey                  d_slotMapKey;
        // Slot key containing the call group and the slot ID.  Used when
        // notifying the signaler about disconnection.

    bsls::AtomicBool            d_isConnected;
        // Set to 'true' on construction, and to 'false' on disconnection.
        // Used for preventing calling a slot after it has been disconnected.

    bsl::weak_ptr<SignalerNode> d_signalerNodePtr;
        // Weak reference to the associated signaler node.

    bsl::function<PROT>         d_func;
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

    // ~Signaler_SlotNode() = default;
        // Destroy this object.

  public:
    // MANIPULATOR
    void disconnect(bool wait) BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;
        // Disconnect this slot and, if the specified 'wait' is 'true', block
        // the calling thread pending the completion of calls to the signaler
        // made by any other threads.  If 'wait' is 'false', return after
        // disconnecting without waiting.  If the slot was already
        // disconnected, this function has no effect.  The behavior is
        // undefined if this function is invoked from a slot with 'true' passed
        // to 'wait'.  Note that it is guaranteed that this slot will not be
        // invoked by a call to the same signaler that begins after this
        // function completes, whether 'wait' is 'true' or not'

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
    typedef typename KeyToSlotMap::PairHandle           SlotPairHandle;

  private:
    // PRIVATE DATA
    mutable bslmt::ReaderWriterMutex  d_signalerMutex;
        // The purpose of this mutex is to implement the waiting behavior of
        // diconnects in 'wait' mode.

    KeyToSlotMap                      d_slotMap;
        // Thread-safe collection containing slots indexed (and ordered) by
        // their respective keys.

    bsls::AtomicUint                  d_keyId;
        // For supplying 'second' members of the 'SlotMapKey' values that are
        // unique to a signaler.

  private:
    // NOT IMPLEMENTED
    Signaler_Node(           const Signaler_Node&) BSLS_KEYWORD_DELETED;
    Signaler_Node& operator=(const Signaler_Node&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit
    Signaler_Node(bslma::Allocator *allocator);
        // Create a 'Signaler_Node' object having no connected slots.  Specify
        // an 'allocator' used to supply memory.  Note that the supplied
        // allocator must remain valid until all connection objects associated
        // with this signaler are destroyed.

  public:
    // MANIPULATORS
    template <class FUNC>
    SignalerConnection connect(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) slot,
                               int                                     group);
        // Implements 'Signaler::connect'.  Connect the specified 'slot', a
        // callable object which must meet the 'Slot Object Requirements'
        // described in the component documentation, to this signaler.  Specify
        // a 'group' used to order slots upon invocation.  Return an instance
        // of 'SignalerConnection' representing the created connection.  This
        // function meets the strong exception guarantee.  Note that the
        // connected slot may be invoked from another thread before this
        // function completes.  Also note that it is unspecified whether
        // connecting a slot while the signaler is emitting will result in the
        // slot being invoked immediately.  Note that 'FUNC' may have a return
        // type other than 'void', but in that case, when the slot is called,
        // the return value will be discarded.

    void disconnectAllSlots() BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::disconnectAllSlots()'.  Disconnect all slots,
        // if any, connected to this signaler.  Any singals emitted on the
        // corresponding signaler that happens after this call to disconect
        // completes will not emit to any slots that were connected prior to
        // this call.  Note that this function does not block the calling
        // thread pending completion of ongoing signals to the slots.  Also
        // note that it is unspecified how many slots, if any, will be called
        // by any invocation on the signaler that begins before this function
        // completes.  Also note that if a slot is connected to this signaler
        // during a call to this function, it is unspecified whether that slot
        // will be disconnected.

    void disconnectAllSlotsAndWait() BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::disconnectAllSlotsAndWait'.  Disconnect all
        // slots, if any, connected to this signaler.  Any signales emitted on
        // the corresponding signaler that happens after this call to disconect
        // completes will not emit to any slots that were connected prior to
        // this call.  This function blocks the calling thread pending
        // completion of ongoing signals to the slots.  The behavior is
        // undefined if this method is called from a slot.  Note that it is
        // unspecified how many slots, if any, will be called by any invocation
        // on the signaler that begins before this function completes.  Also
        // note that if a slot is connected to this signaler during a call to
        // this function, it is unspecified whether that slot will be
        // disconnected.

    void disconnectGroup(int group) BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::disconnectGroup()'.  Disconnect all slots, if
        // any, connected to this signaler in the specified 'group'.  Any
        // signal emitted on the corresponding signaler that happens after this
        // call to disconect completes will not invoke any slots in 'group'
        // that were connected prior to this call.  Note that this function
        // does not block the calling thread pending completion of ongoing
        // calls to the slots.  Also note that it is unspecified how many
        // affected slots, if any, will be signalled to by any invocation on
        // the signaler that begins before this function completes.  Also note
        // that if a slot in 'group' is connected to this signaler during a
        // call to this function, it is unspecified whether that slot will be
        // disconnected.

    void disconnectGroupAndWait(int group) BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::disconnectGroupAndWait()'.  Disconnect all
        // slots, if any, connected to this signaler in the specified 'group'.
        // Any signal emitted on the corresponding signaler that happens after
        // this call to disconect completes will not emit to any slots in
        // 'group' that were connected prior to this call.  This function
        // blocks the calling thread pending completion of ongoing signals
        // emitted to the slots.  The behavior is undefined if this method is
        // called from a slot.  Note that it is unspecified how many affected
        // slots, if any, will be signaled to by any emission on the signaler
        // that begins before this function completes.  Also note that if a
        // slot in 'group' is connected to this signaler during a call to this
        // function, it is unspecified whether that slot will be disconnected.

    void notifyDisconnected(SlotMapKey slotMapKey) BSLS_KEYWORD_NOEXCEPT;
        // Notify this signaler that a slot with the specified 'slotMapKey' was
        // disconnected.

  public:
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
        // Called by 'Signaler_Invocable's call operators, passing the
        // specified 'arg1', 'arg2', 'arg3', 'arg4', 'arg5', 'arg6', 'arg7',
        // 'arg8' and 'arg9' on to the slots.

    bslmt::ReaderWriterMutex& mutex() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reference to the modifiable mutex of this signaler node.

    bsl::size_t slotCount() const BSLS_KEYWORD_NOEXCEPT;
        // Implements 'Signaler::slotCount()'.  Return the number of slots
        // connected to this signaler.  Note that in multithreaded environment,
        // the value returned by 'slotCount()' is approximate.
};

                               // ==============
                               // class Signaler
                               // ==============

template <class PROT>
class Signaler : public Signaler_Invocable<Signaler<PROT>, PROT> {
    // This class template provides a thread-safe signaler that executes
    // connected slots when invoked via its call operator.  'PROT' is a
    // function signature and must have a return type of 'void'.  The callable
    // objects assigned to the slots may have return types other than 'void',
    // in which case their return values will be discarded.

  public:
    // TYPES
    typedef void ResultType;    // Defines the result type of 'operator()'.  If
                                // 'PROT' has a result type that is not 'void',
                                // the return values of the calls to the slots
                                // are discarded.

  private:
    // PRIVATE DATA
    bsl::shared_ptr<Signaler_Node<PROT> > d_signalerNodePtr;

    // FRIENDS
    friend struct Signaler_Invocable<Signaler<PROT>, PROT>;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Signaler, bslma::UsesBslmaAllocator)

  private:
    // NOT IMPLEMENTED
    Signaler(           const Signaler&) BSLS_KEYWORD_DELETED;
    Signaler& operator=(const Signaler&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit
    Signaler(bslma::Allocator *basicAllocator = 0);
        // Create a 'Signaler' object having no connected slots.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that the supplied allocator must remain valid until all
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
        // Connect the specified 'slot', a callable object which must meet the
        // 'Slot Object Requirements' described in the component documentation,
        // to this signaler.  Optionally specify a 'group' used to order slots
        // upon invocation.  Return an instance of 'SignalerConnection'
        // representing the created connection.  This function meets the strong
        // exception guarantee.  Note that the connected slot may be invoked
        // from another thread before this function completes.  Also note that
        // it is unspecified whether connecting a slot while the signaler is
        // emitting will result in the slot being invoked immediately.  Note
        // that 'FUNC' may have a return type other than 'void', but in that
        // case, when the slot is called, the return value will be discarded.

    void disconnectAllSlots() BSLS_KEYWORD_NOEXCEPT;
        // Disconnect all slots, if any, connected to this signaler.  Any
        // singals emitted on this object that happens after this call to
        // disconect completes will not emit to any slots that were connected
        // prior to this call.  Note that this function does not block the
        // calling thread pending completion of ongoing signals to the slots.
        // Also note that it is unspecified how many slots, if any, will be
        // called by any invocation on the signaler that begins before this
        // function completes.  Also note that if a slot is connected to this
        // signaler during a call to this function, it is unspecified whether
        // that slot will be disconnected.

    void disconnectAllSlotsAndWait() BSLS_KEYWORD_NOEXCEPT;
        // Disconnect all slots, if any, connected to this signaler.  Any
        // signales emitted on this object that happens after this call to
        // disconect completes will not emit to any slots that were connected
        // prior to this call.  This function blocks the calling thread pending
        // completion of ongoing signals to the slots.  The behavior is
        // undefined if this method is called from a slot.  Note that it is
        // unspecified how many slots, if any, will be called by any invocation
        // on the signaler that begins before this function completes.  Also
        // note that if a slot is connected to this signaler during a call to
        // this function, it is unspecified whether that slot will be
        // disconnected.

    void disconnectGroup(int group) BSLS_KEYWORD_NOEXCEPT;
        // Disconnect all slots, if any, connected to this signaler in the
        // specified 'group'.  Any signal emitted on this object that happens
        // after this call to disconect completes will not invoke any slots in
        // 'group' that were connected prior to this call.  Note that this
        // function does not block the calling thread pending completion of
        // ongoing calls to the slots.  Also note that it is unspecified how
        // many affected slots, if any, will be signalled to by any invocation
        // on the signaler that begins before this function completes.  Also
        // note that if a slot in 'group' is connected to this signaler during
        // a call to this function, it is unspecified whether that slot will be
        // disconnected.

    void disconnectGroupAndWait(int group) BSLS_KEYWORD_NOEXCEPT;
        // Disconnect all slots, if any, connected to this signaler in the
        // specified 'group'.  Any signal emitted on this object that happens
        // after this call to disconect completes will not emit to any slots in
        // 'group' that were connected prior to this call.  This function
        // blocks the calling thread pending completion of ongoing signals
        // emitted to the slots.  The behavior is undefined if this method is
        // called from a slot.  Note that it is unspecified how many affected
        // slots, if any, will be signaled to by any emission on the signaler
        // that begins before this function completes.  Also note that if a
        // slot in 'group' is connected to this signaler during a call to this
        // function, it is unspecified whether that slot will be disconnected.

  public:
    // ACCESSORS
        // void operator()(ARGS... args) const;
        //
        // 'bdlmt::Signaler_Invocable', from which this 'class' inherits,
        // provides a call operator that, in C++11, would be defined and behave
        // exacly this way, except that the number of arguments is limited to
        // to 9, where 'ARGS...' are the arguments of 'PROT'.  Sequentially
        // emit the signal to each slot connected to this signaler as if by
        // 'f_i(args...)', where 'f_i' is the i-th connected slot.  The
        // behavior is undefined if this function is invoked from a slot
        // connected to this signaler.  Note that signals emitted to slots are
        // ordered by their respective groups, and within groups, by the order
        // in which they were connected.  Also note that the call operator does
        // not forward rvalue references.  That is done explicitly to prevent
        // invocation arguments from being moved to the first slot, leaving
        // them "empty" for all subsequent slots.  Also note that if a slot is
        // connected by a called slot and the group of the new slots is less
        // than the group of the called slot, the new slot will not be called,
        // otherwise it will.  If a slot that has not been visited in a
        // traversal is disconnected by a call to any of the 'disconnect*'
        // method from a slot, the disconnected slot will not be called in the
        // traversal.  Also note that if execution of a slot throws an
        // exception, the emission sequence is interrupted and the exception is
        // propagated to the caller of the signaler immediately.

    bsl::size_t slotCount() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of slots connected to this signaler.  Note that in
        // multithreaded environment, the value returned by 'slotCount()' is
        // approximate.
};

                          // ========================
                          // class SignalerConnection
                          // ========================

class SignalerConnection {
    // This 'class' represents a connection between a signaler and a slot.  It
    // is a lightweight object that has the ability to query whether the
    // signaler and slot are currently connected, and to disconnect the slot
    // from the signaler.  Note that, unless otherwise specified, it is safe to
    // invoke any method of 'SignalerConnection' from the context of its
    // associated slot, or any other slot.

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
    friend bool operator<( const SignalerConnection&,
                           const SignalerConnection&) BSLS_KEYWORD_NOEXCEPT;
    friend bool operator==(const SignalerConnection&,
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

    void reset() BSLS_KEYWORD_NOEXCEPT;
        // Disassociate this connection object from its associated slot, if
        // any, and reset '*this' to a default-constructed state.

    void swap(SignalerConnection& other) BSLS_KEYWORD_NOEXCEPT;
        // Swap the contents of '*this' and the specified 'other'.

  public:
    // ACCESSORS
    void disconnect() const BSLS_KEYWORD_NOEXCEPT;
        // Disconnect the associated slot.  If the slot was already
        // disconnected, this function has no effect.  This function returns
        // immediately without waiting on any calls to the signaler that may be
        // in progress.  Any signal emitted on the corresponding signaler that
        // happens after this call to 'disconect' completes will not emit to
        // the slot.  Note that it is unspecified if any signal that is emitted
        // before this function completes will invoke the slot.

    void disconnectAndWait() const BSLS_KEYWORD_NOEXCEPT;
        // Disconnect the associated slot.  If the slot was already
        // disconnected, this function has no effect.  This function blocks the
        // calling thread pending completion of singals emitted on the signaler
        // by any thread, even if the slot was disconnected prior to this call.
        // Any signal emitted on the corresponding signaler that happens after
        // this call to 'disconect' completes will not emit to the slot.  The
        // behavior is undefined if this method is called from any slot.  Note
        // that it is unspecified if any signal emitted on the signaler that
        // begins before this function completes will emit to the slot.

    bool isConnected() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the associated slot is connected to the signaler
        // '*this' was obtained from, and 'false' otherwise.  If '*this' does
        // not have an associated slot (i.e., was default-constructed), return
        // 'false'.
};

                        // =============================
                        // class SignalerConnectionGuard
                        // =============================

class SignalerConnectionGuard {
    // This guard type 'has a' connection to slot, and upon destruction will
    // call 'disconnect' on that connection.  It also contains a boolean
    // 'waitOnDisconnect', which will deterimine whether the 'disconnect' call
    // will wait be a call to 'SignalerConnection::disconnect' or
    // 'SignalerConnection::disconnectAndWait'.  If the connection contained in
    // the guard refers to no slot, destruction will have no effect.

    // DATA
    SignalerConnection d_connection;

    bool               d_waitOnDisconnect;    // determines whether
                                              // 'disconnect' or
                                              // 'disconnectAndWait' is called
                                              // on 'd_connection' at
                                              // destruction

  public:
    // CREATORS
    explicit
    SignalerConnectionGuard(bool waitOnDisconnect = false)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerConnectionGuard' object having no associated slot.
        // Upon destruction, the specified 'waitOnDisconnect' determines
        // whether 'disconnect()' or 'disconnectAndWait()' will be called on
        // the connection contained in this object.

    explicit
    SignalerConnectionGuard(const SignalerConnection& connection,
                            bool                      waitOnDisconnect = false)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerConnectionGuard' object that refers to and assumes
        // management of the same slot (if any) as the specified 'connection'
        // object.  Upon destruction, the specified 'waitOnDisconnect'
        // determines whether 'disconnect' or 'disconnectAndWait' will be
        // called on the 'SingalerConnection' held by this object.

    explicit
    SignalerConnectionGuard(bslmf::MovableRef<
           SignalerConnection> connection,
           bool                waitOnDisconnect = false) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerConnectionGuard' object that refers to and assumes
        // management of the same slot (if any) as the specified 'connection'
        // object, and reset 'connection' to a default-constructed state.  Upon
        // destruction, the specified 'waitOnDisconnect' determines whether
        // 'disconnect()' or 'disconnectAndWait()' will be called on the
        // connection contained in this object.

    explicit
    SignalerConnectionGuard(bslmf::MovableRef<
                      SignalerConnectionGuard> original) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerConnectionGuard' that refers to and assumes
        // management of the same slot (if any) as the specified 'original'
        // object, and reset the connection contained in 'original' to a
        // default-constructed state.  The 'waitOnDisconnect' state is copied
        // from 'original' to the newly created guard.  Upon destruction, the
        // 'waitOnDisconnect' state determines whether 'disconnect()' or
        // 'disconnectAndWait()' will be called on the connection contained in
        // this object.

    explicit
    SignalerConnectionGuard(bslmf::MovableRef<
              SignalerConnectionGuard> original,
              bool                     waitOnDisconnect) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'SignalerConnectionGuard' that refers to and assumes
        // management of the same slot (if any) as the specified 'original'
        // object, and reset the connection contained in 'original' to a
        // default-constructed state.  Upon destruction, the 'waitOnDisconnect'
        // state determines whether 'disconnect()' or 'disconnectAndWait()'
        // will be called on the connection contained in this object.

    ~SignalerConnectionGuard();
        // Destroy this object.  Call 'disconnect' or 'disconnectAndWait' on
        // the connection contained in this object depending on the value of
        // the 'waitOnDisconnect' 'bool' passed to the constructor or copied
        // when this guard is move-assigned to from another guard.

    // MANIPULATORS
    SignalerConnectionGuard&
                      operator=(bslmf::MovableRef<SignalerConnectionGuard> rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Make this connection refer to and assume management of the same slot
        // (if any) as the specified 'rhs' connection, and reset the connection
        // contained in 'rhs' to a default-constructed state.  Set the
        // 'waitOnDisconnect' state of this object to that of 'rhs'.  Prior to
        // the assignment, call 'disconnect' or 'disconnectAndWait' on the
        // connection contained in this guard, depending on the value of
        // 'waitOnDisconnect()'.  Return '*this'.

    SignalerConnection release() BSLS_KEYWORD_NOEXCEPT;
        // Disassociate this guard from its associated slot, if any, and reset
        // the connection contained in '*this' to a default-constructed state.
        // Return a connection object referring to the slot, if any, that this
        // guard was associated with prior to this call.

    void swap(SignalerConnectionGuard& other) BSLS_KEYWORD_NOEXCEPT;
        // Swap the contents of '*this' and the specified 'other'.

  public:
    // ACCESSORS
    const SignalerConnection& connection() const BSLS_KEYWORD_NOEXCEPT;
        // Return a const reference to the connection held by this object.

    bool waitOnDisconnect() const BSLS_KEYWORD_NOEXCEPT;
        // Return a 'bool' that indicates the value that determines whether
        // 'disconnect' or 'disconnectAndWait' will be called on the
        // 'SignalerConnection' contained in this object on destruction.
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

void swap(SignalerConnectionGuard& a,
          SignalerConnectionGuard& b) BSLS_KEYWORD_NOEXCEPT;
    // Swap the contents of the specified 'a' and 'b'.

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                         // -------------------------
                         // struct Signaler_Invocable
                         // -------------------------

template <class SIGNALER>
inline
void Signaler_Invocable<SIGNALER, void()>::operator()() const
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

template <class SIGNALER, class ARG1>
inline
void Signaler_Invocable<SIGNALER, void(ARG1)>::operator()(ARG1 arg1) const
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

template <class SIGNALER, class ARG1,
                          class ARG2>
inline
void Signaler_Invocable<SIGNALER, void(ARG1, ARG2)>::operator()(ARG1 arg1,
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

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3>
inline
void Signaler_Invocable<SIGNALER, void(ARG1, ARG2, ARG3)>::operator()(
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

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4>
inline
void Signaler_Invocable<SIGNALER, void(ARG1, ARG2, ARG3, ARG4)>::operator()(
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

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4,
                          class ARG5>
inline
void Signaler_Invocable<SIGNALER, void(ARG1, ARG2, ARG3, ARG4, ARG5)>::
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

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4,
                          class ARG5,
                          class ARG6>
inline
void Signaler_Invocable<SIGNALER, void(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)>::
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

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4,
                          class ARG5,
                          class ARG6,
                          class ARG7>
inline
void Signaler_Invocable<SIGNALER,
                        void(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)>::
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

template <class SIGNALER, class ARG1,
                          class ARG2,
                          class ARG3,
                          class ARG4,
                          class ARG5,
                          class ARG6,
                          class ARG7,
                          class ARG8>
inline
void Signaler_Invocable<SIGNALER,
                        void(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>::
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

template <class SIGNALER, class ARG1,
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
Signaler_Invocable<SIGNALER, void(
                       ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>::
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
: d_slotMapKey(slotMapKey)
, d_isConnected(true)
, d_signalerNodePtr(signalerNodePtr)
, d_func(bsl::allocator_arg,
         allocator,
         BSLS_COMPILERFEATURES_FORWARD(FUNC, func))
{
    BSLS_ASSERT(signalerNodePtr);
    BSLS_ASSERT(allocator);
}

// MANIPULATORS
template <class PROT>
inline
void Signaler_SlotNode<PROT>::disconnect(bool wait) BSLS_KEYWORD_NOEXCEPT
{
    if (!d_isConnected.testAndSwap(true, false)) {
        // Already disconnected.  Do nothing.

        return;                                                       // RETURN
    }

    // Notify the associated signaler

    bsl::shared_ptr<SignalerNode> signalerNodePtr = d_signalerNodePtr.lock();
    if (signalerNodePtr) {
        signalerNodePtr->notifyDisconnected(d_slotMapKey);

        if (wait) {
            // Synchronize with the call operator.

            bslmt::WriteLockGuard<bslmt::ReaderWriterMutex> lock(
                                            &signalerNodePtr->mutex()); // LOCK
        }
    }
}

template <class PROT>
inline
void Signaler_SlotNode<PROT>::notifyDisconnected() BSLS_KEYWORD_NOEXCEPT
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
bool Signaler_SlotNode<PROT>::isConnected() const BSLS_KEYWORD_NOEXCEPT
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
    // Hold this mutex (in read mode), so that disconnects in 'wait' mode can
    // synchonize with the call operator by momentarily locking it for write.

    bslmt::ReadLockGuard<bslmt::ReaderWriterMutex> lock(&d_signalerMutex);

    // 'slotHandle' points in to a node in the skiplist, which has a reference
    // count to prevent it from being deallocated & destroyed by another thread
    // as long as we have 'slotHandle' referring to it.  The node may be
    // removed from the skip list, though, in which case its 'next' pointers
    // will be null.

    SlotPairHandle slotHandle;
    if (d_slotMap.front(&slotHandle) != 0) {
        // No slots.  Do nothing.

        return;                                                       // RETURN
    }

    do {
        const SlotNode   *slotNodePtr = &*slotHandle.data();
        const SlotMapKey  slotMapKey  = slotHandle.key();

        // invoke the slot

        slotNodePtr->invoke(
                         arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

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
                                     d_slotMap.allocator(),
                                     this->shared_from_this(),
                                     BSLS_COMPILERFEATURES_FORWARD(FUNC, func),
                                     slotMapKey,
                                     d_slotMap.allocator());

    // connect the slot

    d_slotMap.addR(slotMapKey, slotNodePtr);

    // return the connection

    return SignalerConnection(slotNodePtr);
}

template <class PROT>
void Signaler_Node<PROT>::disconnectAllSlots() BSLS_KEYWORD_NOEXCEPT
{
    SlotPairHandle slotHandle;

    // disconnect all slots in the collection, one by one

    while (d_slotMap.front(&slotHandle) == 0) {
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
void Signaler_Node<PROT>::disconnectGroup(int group) BSLS_KEYWORD_NOEXCEPT
{
    // 'skipForward', below, may fail if a node is removed from the list while
    // we're visiting it, in which case we have to go back to the front and
    // start over.

    SlotPairHandle   slotHandle;
    const SlotMapKey boundary(group, 0);
    while (0 == d_slotMap.findLowerBound(&slotHandle, boundary) &&
                                             slotHandle.key().first == group) {
        do {
            // notify the slot it's being disconnected

            slotHandle.data()->notifyDisconnected();

            SlotPairHandle condemned(slotHandle);

            if (0 != d_slotMap.skipForward(&slotHandle)) {
                slotHandle.release();
            }

            // remove the slot from the collection

            d_slotMap.remove(condemned);
        } while (slotHandle && slotHandle.key().first == group);
    }
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
bslmt::ReaderWriterMutex& Signaler_Node<PROT>::mutex() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_signalerMutex;
}

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

                          // ------------------------
                          // class SignalerConnection
                          // ------------------------

// MANIPULATORS
inline
void SignalerConnection::swap(SignalerConnection& other) BSLS_CPP11_NOEXCEPT
{
    d_slotNodeBasePtr.swap(other.d_slotNodeBasePtr);
}

// FREE OPERATORS
inline
bool operator==(const SignalerConnection& lhs,
                const SignalerConnection& rhs) BSLS_KEYWORD_NOEXCEPT
{
    return lhs.d_slotNodeBasePtr.rep() == rhs.d_slotNodeBasePtr.rep();
}

inline
bool operator<( const SignalerConnection& lhs,
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

                      // -----------------------------
                      // class SignalerConnectionGuard
                      // -----------------------------

// ACCESSORS
inline
const SignalerConnection& SignalerConnectionGuard::connection() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_connection;
}

inline
bool SignalerConnectionGuard::waitOnDisconnect() const BSLS_KEYWORD_NOEXCEPT
{
    return d_waitOnDisconnect;
}

// FREE FUNCTIONS
inline
void swap(SignalerConnectionGuard& a,
          SignalerConnectionGuard& b) BSLS_KEYWORD_NOEXCEPT
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
