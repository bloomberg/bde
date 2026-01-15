// bslstl_stoptoken.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_STOPTOKEN
#define INCLUDED_BSLSTL_STOPTOKEN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocator-aware standard-compliant `stop_source` type.
//
//@CLASSES:
//  bsl::nostopstate_t: tag type for creating an empty `stop_source`
//  bsl::stop_callback: callback to be invoked when a stop is requested
//  bsl::stop_source: mechanism for requesting stops and invoking callbacks
//  bsl::stop_token: mechanism for observing stops and registering callbacks
//
//@CANONICAL_HEADER: bsl_stop_token.h
//
//@DESCRIPTION: This component defines the `bsl::stop_callback`,
// `bsl::stop_source`, and `bsl::stop_token` classes, which provide a
// thread-safe facility for requesting a cancellation (known as "making a stop
// request" in the standard), observing cancellation requests, and registering
// callbacks to be invoked when a cancellation is requested.  The interfaces of
// these classes are identical to those of their `std` counterparts (available
// in C++20 and later), except that `bsl::stop_callback` is allocator-aware and
// `bsl::stop_source` has a constructor that accepts an allocator, which is
// used to allocate the stop state.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Condition variable with interruptible wait
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// `bsl::stop_token` can be used to implement a condition variable wrapper that
// allows a wait to be interrupted by a stop.  (In C++20, such functionality is
// available as `std::condition_variable_any`.)  The wrapper must hold a
// `bsl::stop_token` object that is used to check whether a stop has been
// requested, before entering a wait.  It is also necessary to ensure that the
// thread that requests a stop is able to actually wake up any threads that are
// waiting; for this reason, a `bsl::stop_callback` must be used to notify the
// waiting threads automatically when a stop is requested.  For simplicity, we
// will only implement one signature for the `wait` method.
// ```
// class InterruptibleCV {
//   private:
//     std::condition_variable d_condvar;
//
//   public:
//     void notify_one()
//     {
//         d_condvar.notify_one();
//     }
//
//     void notify_all()
//     {
//         d_condvar.notify_all();
//     }
//
//     template <class t_PREDICATE>
//     void wait(std::unique_lock<std::mutex>& lock,
//               t_PREDICATE                   pred,
//               bsl::stop_token               stopToken)
//     {
//         auto cb = [this] { notify_all(); };
//
//         bsl::stop_callback<decltype(cb)> stopCb(stopToken, cb);
//         while (!stopToken.stop_requested()) {
//             if (pred()) {
//                 return;
//             }
//             d_condvar.wait(lock);
//         }
//     }
// };
// ```
// The `bsl::stop_token` object passed to `InterruptibleCV::wait` will reflect
// that a stop has been requested only after `request_stop` is called on a
// `bsl::stop_source` object from which the `bsl::stop_token` was derived (or a
// copy of that `bsl::stop_source`).
//
// In the `UsageExample` class below, the child thread will wait until the
// value of `d_counter` is at least 50.  However, because the main thread
// requests a stop after setting `d_counter` to 10, the child thread wakes up.
// ```
// struct UsageExample {
//     std::condition_variable d_startCv;
//     InterruptibleCV         d_stopCv;
//     std::mutex              d_mutex;
//     long long               d_counter;
//     bool                    d_ready;
//
//     void threadFunc(bsl::stop_token stopToken)
//     {
//         std::unique_lock<std::mutex> lg(d_mutex);
//         d_ready = true;
//         lg.unlock();
//         d_startCv.notify_one();
//
//         lg.lock();
//         d_stopCv.wait(lg, [this] { return d_counter >= 50; },
//                       std::move(stopToken));
//
//         assert(d_counter >= 10 && d_counter < 50);
//     }
//
//     UsageExample()
//     : d_counter(0)
//     , d_ready(false)
//     {
//         bsl::stop_source stopSource;
//
//         std::thread t(&UsageExample::threadFunc,
//                       this,
//                       stopSource.get_token());
//
//         std::unique_lock<std::mutex> lg(d_mutex);
//         d_startCv.wait(lg, [this] { return d_ready; });
//         lg.unlock();
//
//         for (int i = 0; i < 10; i++) {
//             lg.lock();
//             ++d_counter;
//             lg.unlock();
//         }
//
//         assert(stopSource.request_stop());
//
//         t.join();
//     }
// };
// ```
// Due to the levelization of this component, the example above uses the C++11
// standard library instead of `bslmt::Mutex` and similar components, and will
// therefore compile only in C++11 and higher.  However, a similar example can
// be implemented in C++03 by using `bslmt` components in a package that is
// levelized above `bslmt`.

#include <bsla_nodiscard.h>

#include <bslma_bslallocator.h>
#include <bslma_constructionutil.h>
#include <bslma_destructionutil.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>
#include <bslmf_removereference.h>

#include <bsls_atomic.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

#include <bslstl_sharedptr.h>
#include <bslstl_stopstate.h>
#include <bsls_exceptionutil.h>

#include <utility>

namespace bsl {
template <class t_CALLBACK> class stop_callback;
}  // close namespace bsl

namespace BloombergLP {
namespace bslstl {

                         // ==========================
                         // class StopCallback_NoAlloc
                         // ==========================

/// This component-private empty class is used as a dummy "allocator" when
/// `bsl::stop_callback` wraps a non-allocator-aware type.
class StopCallback_NoAlloc {

  private:
    // FRIENDS
    template <class t_CALLBACK>
    friend class bsl::stop_callback;

    // PRIVATE CREATORS

    /// This private constructor declaration prevents `StopCallback_NoAlloc`
    /// from being an aggregate.
    StopCallback_NoAlloc();
};

                      // ================================
                      // struct StopToken_RefCountedState
                      // ================================

/// This component-private struct adds a reference count to the internal
/// `StopState` class.  This reference count represents the number of
/// `bsl::stop_source` objects that refer to the stop state (NOT the total
/// number of objects that refer to the stop state).
///
/// Implementation note: The reference count has been kept outside the
/// `StopState` object in order to enable `StopState` to potentially be
/// reused to implement `in_place_stop_source` (from WG21 proposal P2300R7)
/// without the overhead from the reference count.
struct StopToken_RefCountedState : StopState {

    // PUBLIC DATA

    /// The number of `bsl::stop_source` objects that refer to this stop
    /// state.
    bsls::AtomicUint64 d_stopSourceCount;
};

                     // ==================================
                     // class StopCallback_CallbackStorage
                     // ==================================

/// The primary class template stores an object of non-reference type given
/// by the template parameter `t_CALLBACK`.  (That is, the primary template
/// provides the implementation only when `t_IS_REFERENCE` is `false`.)
template <class t_CALLBACK,
          bool  t_IS_REFERENCE = bsl::is_reference<t_CALLBACK>::value>
class StopCallback_CallbackStorage {

  private:
    // DATA
    bsls::ObjectBuffer<typename bsl::remove_cv<t_CALLBACK>::type> d_buf;

    // PRIVATE CLASS METHODS

    /// Return a pointer derived from the specified `allocator` or the
    /// specified `noAlloc` suitable for being passed to
    /// `bslma::ConstructionUtil::construct`, i.e., `allocator.mechanism()`
    /// or a null pointer, respectively.
    static bslma::Allocator *mechanism(const bsl::allocator<char>& allocator);
    static void             *mechanism(const StopCallback_NoAlloc& noAlloc);

  public:
    // TYPES
    typedef typename bsl::conditional<
        BloombergLP::bslma::UsesBslmaAllocator<t_CALLBACK>::value,
        bsl::allocator<char>,
        StopCallback_NoAlloc>::type allocator_type;

    // CREATORS

    /// Initialize the stored callback by forwarding the specified `arg` to
    /// the constructor of `t_CALLBACK`; the specified `allocator` is used
    /// to supply memory if `t_CALLBACK` is allocator-aware (and ignored
    /// otherwise).
    template <class t_ARG>
    StopCallback_CallbackStorage(
                            const allocator_type&                    allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg);
    template <class t_ARG>
    StopCallback_CallbackStorage(
                            const allocator_type&                    allocator,
                            t_ARG&                                   arg);

    /// Destroy this object.
    ~StopCallback_CallbackStorage();

    // MANIPULATORS

    /// Return a reference to the stored callback.
    t_CALLBACK&       callback();

    // ACCESSORS

    /// Return a `const` reference to the stored callback.
    const t_CALLBACK& callback() const;
};

/// This partial specialization stores a reference to a callback.
template <class t_CALLBACK>
class StopCallback_CallbackStorage<t_CALLBACK, true> {

  private:
    // DATA
    t_CALLBACK d_callback;

  public:
    // TYPES
    typedef StopCallback_NoAlloc allocator_type;

    // CREATORS

    /// Initialize the stored reference by forwarding the specified `arg`.
    /// Note that the allocator argument is ignored because references are
    /// never allocator-aware.
    template <class t_ARG>
    StopCallback_CallbackStorage(const allocator_type&,
                                 BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg);
    template <class t_ARG>
    StopCallback_CallbackStorage(const allocator_type&,
                                 t_ARG&                                   arg);

    // ACCESSORS

    /// Return an lvalue referring to the callback.
    typename bsl::remove_reference<t_CALLBACK>::type& callback() const;
};

                          // =======================
                          // class StopCallback_Node
                          // =======================

/// This component-private class is used to implement `bsl::stop_callback`.
/// It overrides the virtual `invoke` method of
/// `bslstl::StopStateCallbackNode`, which allows it to be registered and
/// invoked by `bslstl::StopState`.
template <class t_CALLBACK>
class StopCallback_Node : public StopCallback_CallbackStorage<t_CALLBACK>,
                          public StopStateCallbackNode {

  private:
    // PRIVATE MANIPULATORS

    /// Invoke the stored callback.
    void invoke() BSLS_NOTHROW_SPEC BSLS_KEYWORD_OVERRIDE;

  public:
    // CREATORS

    /// Create a `StopCallback_Node` object whose stored callable is
    /// constructed by forwarding from the specified `arg`; the specified
    /// `allocator` is used to supply memory if `t_CALLBACK` is
    /// allocator-aware (and ignored otherwise).
    template <class t_ALLOC, class t_ARG>
    StopCallback_Node(const t_ALLOC&                           allocator,
                      BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg);
    template <class t_ALLOC, class t_ARG>
    StopCallback_Node(const t_ALLOC&                           allocator,
                      t_ARG&                                   arg);
};
}  // close package namespace
}  // close enterprise namespace

namespace bsl {
class stop_source;

                            // ====================
                            // struct nostopstate_t
                            // ====================

/// An object of this empty struct can be passed to the constructor of
/// `stop_source` to create a `stop_source` object that does not refer to
/// any stop state.
struct nostopstate_t {

    // CREATORS

    /// Create a `nostopstate_t` value.
    explicit BSLS_KEYWORD_CONSTEXPR nostopstate_t() BSLS_KEYWORD_NOEXCEPT;
};

                            // --------------------
                            // struct nostopstate_t
                            // --------------------

// CREATORS

/// This `constexpr` function must be defined before it can be used to
/// initialize the `constexpr` variable `nostopstate`, below.
inline
BSLS_KEYWORD_CONSTEXPR nostopstate_t::nostopstate_t() BSLS_KEYWORD_NOEXCEPT
{
}

/// Value of type `nostopstate_t` used as an argument to functions that take
/// a `nostopstate_t` argument.
#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
inline constexpr nostopstate_t nostopstate{};
#else
extern const nostopstate_t nostopstate;
#endif

                              // ================
                              // class stop_token
                              // ================

/// This class is a mechanism for observing cancellation requests.  An
/// object of this class either has (possibly shared) ownership of a stop
/// state and can be used to observe whether a cancellation request has been
/// made on that stop state, or does not own a stop state.  A `stop_token`
/// cannot be used to make a cancellation request.
class stop_token {

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslstl::StopToken_RefCountedState RefCountedState;
    typedef BloombergLP::bslmf::MovableRefUtil             MoveUtil;

    // DATA

    // pointer to the stop state owned by this object, if any
    bsl::shared_ptr<RefCountedState> d_state_p;

    // FRIENDS
    friend class stop_source;

    template <class t_CALLBACK>
    friend class stop_callback;

    /// Return `true` if the specified `lhs` and `rhs` refer to the same
    /// stop state, or if neither refers to a stop state; `false` otherwise.
    /// Implementation note: this function is required by the standard to be
    /// a hidden friend ([hidden.friends], [stoptoken.general]).
    BSLA_NODISCARD friend bool operator==(
                                   const stop_token& lhs,
                                   const stop_token& rhs) BSLS_KEYWORD_NOEXCEPT
    {
        return lhs.d_state_p == rhs.d_state_p;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    /// Return `true` if the specified `lhs` and `rhs` refer to different
    /// stop states, or if only one refers to a stop state; `false`
    /// otherwise.
    BSLA_NODISCARD friend bool operator!=(
                                   const stop_token& lhs,
                                   const stop_token& rhs) BSLS_KEYWORD_NOEXCEPT
    {
        return lhs.d_state_p != rhs.d_state_p;
    }
#endif

    /// Set `lhs` to refer to the stop state (or lack thereof) that `rhs`
    /// referred to, and vice versa.  Implementation note: this function is
    /// required by the standard to be a hidden friend ([hidden.friends],
    /// [stoptoken.general]).
    friend void swap(stop_token& lhs, stop_token& rhs) BSLS_KEYWORD_NOEXCEPT
    {
        lhs.d_state_p.swap(rhs.d_state_p);
    }

    // PRIVATE CREATORS

    /// Create a `stop_token` object that refers to the stop state that the
    /// specified `state` points to (if any).
    explicit stop_token(bsl::shared_ptr<RefCountedState> state);

  public:
    // CREATORS

    /// Create a `stop_token` object that does not refer to a stop state.
    stop_token() BSLS_KEYWORD_NOEXCEPT;

    /// Create a `stop_token` object that refers to the same stop state (or
    /// lack thereof) as the specified `original` object.
    stop_token(const stop_token& original) BSLS_KEYWORD_NOEXCEPT;

    /// Create a `stop_token` object that refers to the same stop state (or
    /// lack) thereof as the specified `original` object, and reset
    /// `original` to not refer to a stop state.
    stop_token(BloombergLP::bslmf::MovableRef<stop_token> original)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Destroy this object.
    ~stop_token();

    // MANIPULATORS

    /// Set this object to refer to the same stop state (or lack thereof) as
    /// the specified `other` object.
    stop_token& operator=(const stop_token& other) BSLS_KEYWORD_NOEXCEPT;

    /// Set this object to refer to the stop state (or lack thereof) that
    /// the specified `other` object refers to, and reset `other` to not
    /// refer to a stop state.
    stop_token& operator=(BloombergLP::bslmf::MovableRef<stop_token> other)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Set `*this` to refer to the stop state (or lack thereof) that the
    /// specified `other` referred to, and vice versa.  Equivalent to
    /// `swap(*this, other)`.
    void swap(stop_token& other) BSLS_KEYWORD_NOEXCEPT;

    // ACCESSORS

    /// Return `true` if `*this` refers to a stop state, and either a stop
    /// was already requested on that stop state or there is at least one
    /// `stop_source` object that refers to that stop state (implying that a
    /// stop could still be requested using the `request_stop` function),
    /// and `false` otherwise.  A call to `stop_possible` that is
    /// potentially concurrent with a call to `stop_requested` or
    /// `stop_possible` does not cause a data race.
    BSLA_NODISCARD bool stop_possible() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `true` if `*this` refers to a stop state on which
    /// `request_stop` has been called, and `false` otherwise.  If this
    /// function returns `true`, then the successful call to `request_stop`
    /// synchronizes with this call.  A call to `stop_requested` that is
    /// potentially concurrent with a call to `stop_requested` or
    /// `stop_possible` does not cause a data race.
    BSLA_NODISCARD bool stop_requested() const BSLS_KEYWORD_NOEXCEPT;
};

                             // =================
                             // class stop_source
                             // =================

/// This class is a mechanism for making and observing cancellation
/// requests.  An object of this class may have (possibly shared) ownership
/// of a stop state, in which case it can be used to make a cancellation
/// request or observe whether a cancellation request has been made on the
/// owned stop state; it is also possible for a `stop_source` object to not
/// own a stop state.  Due to its shared ownership semantics, it is safe to
/// pass a copy of a `stop_source` object to a callback that might outlive
/// the original `stop_source` object; however, a callback that should only
/// be able to observe a cancellation request, without being able to
/// request cancellation itself, should instead be passed a `stop_token`,
/// which can be created by calling `stop_source::get_token`.
class stop_source {

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslstl::StopToken_RefCountedState RefCountedState;
    typedef BloombergLP::bslmf::MovableRefUtil             MoveUtil;

    // DATA

    // pointer to the stop state owned by this object, if any
    shared_ptr<RefCountedState> d_state_p;

    // FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` refer to the same
    /// stop state, or if neither refers to a stop state; `false` otherwise.
    /// Implementation note: this function is required by the standard to be
    /// a hidden friend ([hidden.friends], [stopsource.general]).
    BSLA_NODISCARD friend bool operator==(
                                  const stop_source& lhs,
                                  const stop_source& rhs) BSLS_KEYWORD_NOEXCEPT
    {
        return lhs.d_state_p == rhs.d_state_p;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    /// Return `true` if the specified `lhs` and `rhs` refer to different
    /// stop states, or if only one refers to a stop state; `false`
    /// otherwise.
    BSLA_NODISCARD friend bool operator!=(
                                  const stop_source& lhs,
                                  const stop_source& rhs) BSLS_KEYWORD_NOEXCEPT
    {
        return lhs.d_state_p != rhs.d_state_p;
    }
#endif

    /// Set `lhs` to refer to the stop state (or lack thereof) that `rhs`
    /// referred to before the call, and vice versa.  Implementation note:
    /// this function is required by the standard to be a hidden friend
    /// ([hidden.friends], [stopsource.general]).
    friend void swap(stop_source& lhs, stop_source& rhs) BSLS_KEYWORD_NOEXCEPT
    {
        lhs.d_state_p.swap(rhs.d_state_p);
    }

  public:
    // CREATORS

    /// Create a `stop_source` object that refers to a distinct stop state,
    /// using the currently installed default allocator to supply memory.
    stop_source();

    /// Create a `stop_source` object that does not refer to a stop state
    /// and, therefore, cannot be used to request a stop.
    explicit stop_source(nostopstate_t) BSLS_KEYWORD_NOEXCEPT;

    /// Create a `stop_source` object that refers to the same stop state (or
    /// lack thereof) as the specified 'original.
    stop_source(const stop_source& original) BSLS_KEYWORD_NOEXCEPT;

    /// Create a `stop_source` object that refers to the stop state (or lack
    /// thereof) referred to by the specified `original`, and reset
    /// `original` to not refer to a stop state.
    stop_source(BloombergLP::bslmf::MovableRef<stop_source> original)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Create a `stop_source` object that refers to a distinct stop state,
    /// using the specified `allocator` to supply memory.  Note, however,
    /// that `stop_source` is not allocator-aware.
    explicit stop_source(bsl::allocator<char> allocator);

    /// Destroy this object.
    ~stop_source();

    // MANIPULATORS

    /// Set this object to refer to the same stop state (or lack thereof) as
    /// the specified `other` object.
    stop_source& operator=(const stop_source& other) BSLS_KEYWORD_NOEXCEPT;

    /// Set this object to refer to the stop state (or lack thereof) that
    /// the specified `other` object refers to, and reset `other` to not
    /// refer to a stop state.
    stop_source& operator=(BloombergLP::bslmf::MovableRef<stop_source> other)
                                                         BSLS_KEYWORD_NOEXCEPT;

    /// Set `*this` to refer to the stop state (or lack thereof) that the
    /// specified `other` referred to, and vice versa.  Equivalent to
    /// `swap(*this, other)`.
    void swap(stop_source& other) BSLS_KEYWORD_NOEXCEPT;

    /// If `*this` refers to a stop state and that stop state has not had a
    /// stop requested yet, atomically request a stop on that stop state,
    /// invoke all registered callbacks in an unspecified order, and finally
    /// return `true`.  Otherwise, return `false`.  If this function returns
    /// `true`, the call synchronizes with any call to `stop_requested` that
    /// returns `true`.  A call to `request_stop` that is potentially
    /// concurrent with a call to `stop_requested`, `stop_possible`, or
    /// `request_stop` does not cause a data race.
    bool request_stop() BSLS_KEYWORD_NOEXCEPT;

    // ACCESSORS

    /// Return a `stop_token` that refers to the stop state (or lack
    /// thereof) that `*this` refers to.
    BSLA_NODISCARD stop_token get_token() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `true` if `*this` refers to a stop state, and `false`
    /// otherwise.  A call to `stop_possible` that is potentially concurrent
    /// with a call to `stop_requested`, `stop_possible`, or `request_stop`
    /// does not cause a data race.
    BSLA_NODISCARD bool stop_possible() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `true` if `*this` refers to a stop state on which
    /// `request_stop` has been called, and `false` otherwise.  If this
    /// function returns `true`, then the successful call to `request_stop`
    /// synchronizes with this call.  A call to `stop_requested` that is
    /// potentially concurrent with a call to `stop_requested`,
    /// `stop_possible`, or `request_stop` does not cause a data race.
    BSLA_NODISCARD bool stop_requested() const BSLS_KEYWORD_NOEXCEPT;
};

                            // ===================
                            // class stop_callback
                            // ===================

/// This class holds an object or reference of type `t_CALLBACK` and, when
/// constructed using a `stop_token` that owns a stop state, schedules the
/// held object or reference to be executed by the thread that requests
/// cancellation on that stop state (if any).  However, if cancellation was
/// already requested before the `stop_callback` was constructed, the
/// constructor invokes the callback immediately.  If there is no stop
/// state, or `request_stop` is never called for the stop state, then the
/// callback is not invoked.  `stop_callback` stores its callback within its
/// own footprint, and thus never requires memory allocation; however,
/// `stop_callback<t_CALLBACK>` is an allocator-aware class, if `t_CALLBACK`
/// is an allocator-aware class, and any supplied allocator will then be
/// passed to the constructor of `t_CALLBACK`.
template <class t_CALLBACK>
class stop_callback {

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    // DATA

    // object that holds the `t_CALLBACK` object or reference and can be a
    // member of the intrusive linked list maintained by
    // `bslstl_StopState`
    BloombergLP::bslstl::StopCallback_Node<t_CALLBACK> d_node;

    // pointer to the stop state on which `d_node` is scheduled to be
    // invoked, if any
    bsl::shared_ptr<BloombergLP::bslstl::StopState>    d_state_p;

    // PRIVATE MANIPULATORS

    /// Attempt to register the stored callable with the stop state that
    /// `*this` refers to (if any), and reset `d_state_p` if unsuccessful.
    void init();

  private:
    // NOT IMPLEMENTED
    stop_callback(const stop_callback&) BSLS_KEYWORD_DELETED;
    stop_callback& operator=(const stop_callback&) BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef t_CALLBACK callback_type;

    /// The allocator type is `bsl::allocator<char>` if `t_CALLBACK` is
    /// allocator-aware, and an empty dummy type otherwise.
    typedef typename BloombergLP::bslstl::StopCallback_Node<
        t_CALLBACK>::allocator_type allocator_type;

    // CREATORS

    /// Create a `stop_callback` object whose stored callable is constructed
    /// by forwarding from the specified `arg`; if `t_CALLBACK` is
    /// allocator-aware, the optionally specified `alloc` will be used to
    /// supply memory instead of the default allocator (otherwise, `alloc`
    /// is ignored).  If the specified `token` refers to a stop state on
    /// which a stop has been requested, invoke the callback before
    /// returning; otherwise, if `token` refers to a stop state, associate
    /// `*this` with that stop state and register the callback with that
    /// stop state.  Unlike the constructors of `std::stop_callback`, these
    /// constructors do not currently have a `noexcept` specification.  Note
    /// that if `token` is an rvalue reference, it is unspecified whether
    /// this function moves from `token`.
    template <class t_ARG>
    explicit stop_callback(
          const stop_token&                          token,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg,
          const allocator_type&                      alloc = allocator_type());
    template <class t_ARG>
    explicit stop_callback(
          BloombergLP::bslmf::MovableRef<stop_token> token,
          BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg,
          const allocator_type&                      alloc = allocator_type());
    template <class t_ARG>
    explicit stop_callback(
          const stop_token&                          token,
          t_ARG&                                     arg,
          const allocator_type&                      alloc = allocator_type());
    template <class t_ARG>
    explicit stop_callback(
          BloombergLP::bslmf::MovableRef<stop_token> token,
          t_ARG&                                     arg,
          const allocator_type&                      alloc = allocator_type());

    /// Destroy this object.  If `*this` refers to a stop state and the
    /// stored callback is registered with the stop state but has not yet
    /// begun execution, deregister the callback from that stop state.
    /// Otherwise, if the callback is executing on a thread other than the
    /// thread invoking the destructor, the completion of the callback
    /// strongly happens before the destructor returns.
    ~stop_callback();

    // ACCESSORS

    /// If `t_CALLBACK` is allocator-aware, return the allocator used to
    /// construct this object; otherwise, the return type is `void` and the
    /// definition of this function is ill-formed.
    allocator_type get_allocator() const;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES
template <class t_CALLBACK>
stop_callback(stop_token, t_CALLBACK) -> stop_callback<t_CALLBACK>;
#endif
}  // close namespace bsl

namespace BloombergLP {
namespace bslma {
template <class t_CALLBACK>
struct UsesBslmaAllocator<bsl::stop_callback<t_CALLBACK> >
: UsesBslmaAllocator<t_CALLBACK> {
};
}  // close namespace bslma
}  // close enterprise namespace

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslstl {

                         // --------------------------
                         // class StopCallback_NoAlloc
                         // --------------------------

inline StopCallback_NoAlloc::StopCallback_NoAlloc()
{
}

           // -----------------------------------------------------
           // class StopCallback_CallbackStorage<t_CALLBACK, false>
           // -----------------------------------------------------


template <class t_CALLBACK, bool t_IS_REFERENCE>
bslma::Allocator *
StopCallback_CallbackStorage<t_CALLBACK, t_IS_REFERENCE>::mechanism(
                                         const bsl::allocator<char>& allocator)
{
    return allocator.mechanism();
}

template <class t_CALLBACK, bool t_IS_REFERENCE>
void *StopCallback_CallbackStorage<t_CALLBACK, t_IS_REFERENCE>::mechanism(
                                                   const StopCallback_NoAlloc&)
{
    return 0;
}

template <class t_CALLBACK, bool t_IS_REFERENCE>
template <class t_ARG>
StopCallback_CallbackStorage<t_CALLBACK, t_IS_REFERENCE>::
    StopCallback_CallbackStorage(
                            const allocator_type&                    allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg)
{
    BloombergLP::bslma::ConstructionUtil::construct(
                                    d_buf.address(),
                                    mechanism(allocator),
                                    BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg));
}

template <class t_CALLBACK, bool t_IS_REFERENCE>
template <class t_ARG>
StopCallback_CallbackStorage<t_CALLBACK, t_IS_REFERENCE>::
    StopCallback_CallbackStorage(const allocator_type& allocator, t_ARG& arg)
{
    BloombergLP::bslma::ConstructionUtil::construct(d_buf.address(),
                                                    mechanism(allocator),
                                                    arg);
}

template <class t_CALLBACK, bool t_IS_REFERENCE>
StopCallback_CallbackStorage<t_CALLBACK,
                             t_IS_REFERENCE>::~StopCallback_CallbackStorage()
{
    BloombergLP::bslma::DestructionUtil::destroy(d_buf.address());
}

template <class t_CALLBACK, bool t_IS_REFERENCE>
t_CALLBACK&
StopCallback_CallbackStorage<t_CALLBACK, t_IS_REFERENCE>::callback()
{
    return d_buf.object();
}

template <class t_CALLBACK, bool t_IS_REFERENCE>
const t_CALLBACK&
StopCallback_CallbackStorage<t_CALLBACK, t_IS_REFERENCE>::callback() const
{
    return d_buf.object();
}

            // ----------------------------------------------------
            // class StopCallback_CallbackStorage<t_CALLBACK, true>
            // ----------------------------------------------------

template <class t_CALLBACK>
template <class t_ARG>
StopCallback_CallbackStorage<t_CALLBACK, true>::StopCallback_CallbackStorage(
                                  const allocator_type&,
                                  BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg)
: d_callback(BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg))
{
}

template <class t_CALLBACK>
template <class t_ARG>
StopCallback_CallbackStorage<t_CALLBACK, true>::StopCallback_CallbackStorage(
                                                         const allocator_type&,
                                                         t_ARG& arg)
: d_callback(arg)
{
}

template <class t_CALLBACK>
typename bsl::remove_reference<t_CALLBACK>::type&
StopCallback_CallbackStorage<t_CALLBACK, true>::callback() const
{
    return d_callback;
}

                    // -----------------------------------
                    // class StopCallback_Node<t_CALLBACK>
                    // -----------------------------------

template <class t_CALLBACK>
void StopCallback_Node<t_CALLBACK>::invoke() BSLS_NOTHROW_SPEC
{
    // We cannot use 'BSLS_COMPILERFEATURES_FORWARD' here because it will add
    // 'const' in C++03.
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    std::forward<t_CALLBACK>(
                       StopCallback_CallbackStorage<t_CALLBACK>::callback())();
#else
    StopCallback_CallbackStorage<t_CALLBACK>::callback()();
#endif
}

template <class t_CALLBACK>
template <class t_ALLOC, class t_ARG>
StopCallback_Node<t_CALLBACK>::StopCallback_Node(
                            const t_ALLOC&                           allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg)
: StopCallback_CallbackStorage<t_CALLBACK>(allocator,
                                           BSLS_COMPILERFEATURES_FORWARD(t_ARG,
                                                                         arg))
{
}

template <class t_CALLBACK>
template <class t_ALLOC, class t_ARG>
StopCallback_Node<t_CALLBACK>::StopCallback_Node(const t_ALLOC& allocator,
                                                 t_ARG&         arg)
: StopCallback_CallbackStorage<t_CALLBACK>(allocator, arg)
{
}
}  // close package namespace
}  // close enterprise namespace

                              // ----------------
                              // class stop_token
                              // ----------------

namespace bsl {
// PRIVATE CREATORS
inline
stop_token::stop_token(bsl::shared_ptr<RefCountedState> state)
: d_state_p(MoveUtil::move(state))
{
}

// CREATORS
inline
stop_token::stop_token() BSLS_KEYWORD_NOEXCEPT
: d_state_p()
{
}

inline
stop_token::stop_token(const stop_token& original) BSLS_KEYWORD_NOEXCEPT
: d_state_p(original.d_state_p)
{
}

inline
stop_token::stop_token(BloombergLP::bslmf::MovableRef<stop_token> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_state_p(MoveUtil::move(MoveUtil::access(original).d_state_p))
{
}

inline
stop_token::~stop_token()
{
}

// MANIPULATORS
inline
stop_token& stop_token::operator=(
                                 const stop_token& other) BSLS_KEYWORD_NOEXCEPT
{
    d_state_p = other.d_state_p;
    return *this;
}

inline
stop_token& stop_token::operator=(
        BloombergLP::bslmf::MovableRef<stop_token> other) BSLS_KEYWORD_NOEXCEPT
{
    d_state_p = MoveUtil::move(MoveUtil::access(other).d_state_p);
    return *this;
}

inline
void stop_token::swap(stop_token& other) BSLS_KEYWORD_NOEXCEPT
{
    d_state_p.swap(other.d_state_p);
}

                             // -----------------
                             // class stop_source
                             // -----------------

// CREATORS
inline
stop_source::stop_source(nostopstate_t) BSLS_KEYWORD_NOEXCEPT
: d_state_p()
{
}

inline
stop_source::stop_source(BloombergLP::bslmf::MovableRef<stop_source> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_state_p(MoveUtil::move(MoveUtil::access(original).d_state_p))
{
}

// MANIPULATORS
inline
void stop_source::swap(stop_source& other) BSLS_KEYWORD_NOEXCEPT
{
    d_state_p.swap(other.d_state_p);
}

                            // -------------------
                            // class stop_callback
                            // -------------------

// PRIVATE MANIPULATORS
template <class t_CALLBACK>
void stop_callback<t_CALLBACK>::init()
{
    if (d_state_p && !d_state_p->enregister(&d_node)) {
        d_state_p.reset();
    }
}

// CREATORS
template <class t_CALLBACK>
template <class t_ARG>
stop_callback<t_CALLBACK>::stop_callback(
                                const stop_token&                        token,
                                BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG) arg,
                                const allocator_type&                    alloc)
: d_node(alloc, BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg))
, d_state_p(token.d_state_p)
{
    init();
}

template <class t_CALLBACK>
template <class t_ARG>
stop_callback<t_CALLBACK>::stop_callback(
                              BloombergLP::bslmf::MovableRef<stop_token> token,
                              BSLS_COMPILERFEATURES_FORWARD_REF(t_ARG)   arg,
                              const allocator_type&                      alloc)
: d_node(alloc, BSLS_COMPILERFEATURES_FORWARD(t_ARG, arg))
, d_state_p(MoveUtil::move(MoveUtil::access(token).d_state_p))
{
    init();
}

template <class t_CALLBACK>
template <class t_ARG>
stop_callback<t_CALLBACK>::stop_callback(const stop_token&     token,
                                         t_ARG&                arg,
                                         const allocator_type& alloc)
: d_node(alloc, arg)
, d_state_p(token.d_state_p)
{
    init();
}

template <class t_CALLBACK>
template <class t_ARG>
stop_callback<t_CALLBACK>::stop_callback(
                              BloombergLP::bslmf::MovableRef<stop_token> token,
                              t_ARG&                                     arg,
                              const allocator_type&                      alloc)
: d_node(alloc, arg)
, d_state_p(MoveUtil::move(MoveUtil::access(token).d_state_p))
{
    init();
}

template <class t_CALLBACK>
stop_callback<t_CALLBACK>::~stop_callback()
{
    if (d_state_p) {
        d_state_p->deregister(&d_node);
    }
}

template <class t_CALLBACK>
typename stop_callback<t_CALLBACK>::allocator_type
stop_callback<t_CALLBACK>::get_allocator() const
{
    return d_node.callback().get_allocator();
}
}  // close namespace bsl
#endif  // INCLUDED_BSLSTL_STOPTOKEN

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
