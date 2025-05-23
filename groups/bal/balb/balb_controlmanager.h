// balb_controlmanager.h                                              -*-C++-*-
#ifndef INCLUDED_BALB_CONTROLMANAGER
#define INCLUDED_BALB_CONTROLMANAGER

//@PURPOSE: Provide a mechanism for mapping control messages to callbacks.
//
//@CLASSES:
//   balb::ControlManager: mechanism that maps control messages
//
//@DESCRIPTION: The `balb::ControlManager` mechanism provided by this component
// maps control messages to callback functions on the basis of message
// prefixes.
//
// The prefix (case-insensitive) "HELP" is reserved for use by the
// `registerUsageHandler`.
//
///Callback Function Requirements
///------------------------------
// Functions registered as callbacks for messages must be invokable as
// `void(*)(const bsl::string&, bsl::istream&)`.  (This signature is
// `balb::ControlManager::ControlHandler`).  When the function is invoked, the
// first argument is the message prefix, and the second is a stream on the
// remainder of the message.
//
///Thread Safety
///-------------
// This component is thread-safe and thread-enabled: it is safe to access and
// manipulate multiple distinct instances from different threads, and it is
// safe to access and manipulate a single shared instance from different
// threads.
//
///Default Handler
///---------------
// The `balb::ControlManager` ignores messages having (case-insensitive)
// prefixes that have not been previously registered.  Optionally,
// users can install a (default) handler for messages messages with
// un-registered prefixes.  Note that "default" refers to the message prefix --
// there is no such handler unless the user explicitly installs one using the
// `setDefaultHandler` method.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating an ECHO Message Handler
///- - - - - - - - - - - - - - - - - - - - - -
// First define a trivial callback to be invoked when an "ECHO" message is
// received:
// ```
// void onEcho(const bsl::string& prefix, bsl::istream& stream)
// {
//    bsl::string word;
//    bsl::cout << "onEcho: \"" << prefix;
//    while (stream.good()) {
//       stream >> word;
//       bsl::cout << ' ' << word;
//    }
//    bsl::cout << '\"' << bsl::endl;
// }
// ```
// Now create a `balb::ControlManager` object and register a handler for
// "ECHO".  Also register a handler for HELP to observe the auto-generated
// documentation for ECHO:
// ```
// balb::ControlManager manager;
// manager.registerHandler("ECHO", "<text>",
//                         "Print specified text to the standard output",
//                         &onEcho);
// manager.registerHandler("HELP", "",
//                         "Print documentation",
//                         bdlf::BindUtil::bind(
//                                 &balb::ControlManager::printUsageHelper,
//                                 &manager, &bsl::cout, bsl::string(
//              "The following commands are accepted by the test driver:")));
//
// manager.dispatchMessage("ECHO repeat this text");
// manager.dispatchMessage("echo matching is case-insensitive");
// manager.dispatchMessage("HELP");
// ```

#include <balscm_version.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_rwmutex.h>

#include <bsl_functional.h>
#include <bsl_iosfwd.h>
#include <bsl_map.h>
#include <bsl_optional.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace balb {

                            // ====================
                            // class ControlManager
                            // ====================

/// Dispatch control messages to callbacks by name.
class ControlManager {

  public:
    // TYPES

    /// Defines a type alias for the function called to handle control
    /// messages.  The `prefix` argument is the first space-delimited word
    /// read from the message, and the `stream` argument is the
    /// `bsl::istream` containing the remainder of the message.
    typedef bsl::function<void(const bsl::string& prefix,
                               bsl::istream&      stream)>
        ControlHandler;

  private:
    // PRIVATE TYPES

                        // ==========================
                        // class ControlManager_Entry
                        // ==========================

    // IMPLEMENTATION NOTE: The Sun Studio 12.3 compiler does not support
    // 'map's holding types that are incomplete at the point of declaration of
    // a data member.  Other compilers allow us to complete
    // 'ControlManager_Entry' at a later point in the code, but before any
    // operation (such as 'insert') that would require the type to be complete.
    // If we did not have to support this compiler, this whole class could be
    // defined in the .cpp file; as it stands, it *must* be defined before
    // class 'ControlManager'.

    /// This component-private class represents a function with documentation.
    class ControlManager_Entry {

        // DATA
        ControlManager::ControlHandler d_callback;     // processing callback
        bsl::string                    d_arguments;    // argument description
        bsl::string                    d_description;  // function description

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(ControlManager_Entry,
                                       bslma::UsesBslmaAllocator);

        // CREATORS

        /// Create a `ControlManager_Entry` object.  Optionally specify a
        /// `basicAllocator` used to supply memory.  If `basicAllocator` is
        /// 0, the currently installed default allocator is used.
        explicit ControlManager_Entry(bslma::Allocator *basicAllocator = 0);

        /// Create an `ControlManager_Entry` object with the specified initial
        /// values.
        ControlManager_Entry(
                    const ControlManager::ControlHandler&  callback,
                    const bsl::string_view&                arguments,
                    const bsl::string_view&                description,
                    bslma::Allocator                      *basicAllocator = 0);

        /// Create an `ControlManager_Entry` object having the value of the
        /// specified `original` object.  Optionally specify a
        /// `basicAllocator` used to supply memory.  If `basicAllocator` is
        /// 0, the currently installed default allocator is used.
        ControlManager_Entry(const ControlManager_Entry&  original,
                             bslma::Allocator            *basicAllocator = 0);

        /// Destroy this object.
        ~ControlManager_Entry();

        // MANIPULATORS

        /// Assign to this object the value of the specified `rhs` object.
        ControlManager_Entry& operator=(const ControlManager_Entry& rhs);

        /// Set the specified `callback` as the value of the `callback`
        /// member of this object.
        void setCallback(const ControlManager::ControlHandler& callback);

        /// Return a modifiable reference to the `arguments` member of this
        /// object.
        bsl::string& arguments();

        /// Return a modifiable reference to the `description` member of
        /// this object.
        bsl::string& description();

        // ACCESSORS

        /// Return a non-modifiable reference to the `callback` member of
        /// this object.
        const ControlManager::ControlHandler& callback() const;

        /// Return a non-modifiable reference to the `arguments` member of
        /// this object.
        const bsl::string& arguments() const;

        /// Return a non-modifiable reference to the `arguments` member of
        /// this object.
        const bsl::string& description() const;
    };

    struct CaselessLessThan {
        // TYPES
        typedef void is_transparent;

        // ACCESSOR

        /// Return `true` if the specified `lhs` is less than the specified
        /// `rhs` in a case-insensitive comparison, and `false` otherwise.
        bool operator()(const bsl::string_view& lhs,
                        const bsl::string_view& rhs) const;
    };

    /// Defines a type alias for the ordered associative data structure that
    /// maps a message prefix to a `StringComparator` functor.
    typedef bsl::map<bsl::string, ControlManager_Entry, CaselessLessThan>
        Registry;

    // DATA
    bslma::Allocator              *d_allocator_p;    // memory allocator (held)

    Registry                       d_registry;       // registry

    mutable bslmt::RWMutex         d_registryMutex;  // mutex for registry
                                                     // and default handler

    bsl::optional<ControlHandler>  d_defaultHandler; // default handler

    // NOT IMPLEMENTED
    ControlManager(const ControlManager&);             // = deleted
    ControlManager& operator=(const ControlManager&);  // = deleted

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ControlManager, bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create a control manager object.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    explicit ControlManager(bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~ControlManager();

    // MANIPULATORS

    /// Register the specified `handler` to be invoked whenever a control
    /// message having the specified case-insensitive `prefix` is received
    /// by this control manager.  Also register the specified `arguments`
    /// string to describe the arguments accepted by the message, and the
    /// specified `description` to describe its operation; these are printed
    /// by `printUsage`.  Return a positive value if an existing callback
    /// was replaced, return 0 if no replacement occurred, and return a
    /// negative value otherwise.
    int registerHandler(const bsl::string_view& prefix,
                        const bsl::string_view& arguments,
                        const bsl::string_view& description,
                        const ControlHandler&   handler);

    /// Register a handler that, on receipt of a (case-insensitive) "HELP"
    /// message, prints to the specified stream a list of this
    /// `ControlManager`s registered commands and their documentation.
    /// Return a positive value if an existing callback was replaced, return
    /// 0 if no replacement occurred, and return a negative value otherwise.
    int registerUsageHandler(bsl::ostream& stream);

    /// Register the specified `handler` to be invoked whenever a control
    /// message having an unregistered (case-insensitive) `prefix` is received
    /// by this control manager.  Return a positive value if an existing
    /// callback was replaced, return 0 if no replacement occurred, and return
    /// a negative value otherwise.
    int registerDefaultHandler(const ControlHandler& hander);

    /// Deregister the callback function previously registered to handle the
    /// specified `prefix`.  Return 0 on success or a non-zero value
    /// otherwise.
    int deregisterHandler(const bsl::string_view& prefix);

    // Deregister the callback function previously registered to handle "HELP"
    // messages (see `registerUsageHandler`).  Return 0 on success or a
    // non-zero value otherwise.
    int deregisterUsageHandler();

    /// Deregister the callback function previously registered (see
    /// `registerDefaultHandler`) to handle messages with unregistered
    /// prefixes.  Return 0 on success or a non-zero value otherwise.
    int deregisterDefaultHandler();

    // ACCESSOR

    /// Parse the specified complete `message` and dispatch it.  Return
    /// 0 on success, and a non-zero value otherwise; in particular return
    /// non-zero if no registered callback could be found for the
    /// case-insensitive prefix in `message` and no default handler is
    /// installed.
    int dispatchMessage(const bsl::string_view& message) const;

    /// Dispatch the message contained in the specified `stream` to the
    /// callback associated with the specified `prefix`.  Return 0 on
    /// success, and a non-zero value otherwise; in particular return
    /// non-zero if no registered callback could be found for the
    /// case-insensitive `prefix` and no default handler is installed.
    int dispatchMessage(const bsl::string& prefix, bsl::istream& stream) const;

    /// Return `true` if this control manager has a default message handler
    /// installed, and `false` otherwise.
    bool hasDefaultHandler() const;

    /// Print to the specified `stream` the specified `preamble` text,
    /// followed by the registered commands and documentation for this
    /// control manager.  Note that a newline is appended to `preamble` in
    /// the output.
    void printUsage(bsl::ostream&           stream,
                    const bsl::string_view& preamble) const;

    /// Invoke `printUsage` passing the specified `*stream` and `preamble`.
    /// Suitable for binding using the bdlf::BindUtil package.
    void printUsageHelper(bsl::ostream            *stream,
                          const bsl::string_view&  preamble) const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.  Note
    /// that if no allocator was supplied at construction the default
    /// allocator in effect at construction is used.
    bslma::Allocator *allocator() const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                 // ------------------------------------------
                 // class ControlManager::ControlManager_Entry
                 // ------------------------------------------

// MANIPULATORS
inline
void ControlManager::ControlManager_Entry::setCallback(
                                const ControlManager::ControlHandler& callback)
{
    d_callback = callback;
}

inline
bsl::string& ControlManager::ControlManager_Entry::arguments()
{
    return d_arguments;
}

inline
bsl::string& ControlManager::ControlManager_Entry::description()
{
    return d_description;
}

// ACCESSORS
inline
const ControlManager::ControlHandler&
ControlManager::ControlManager_Entry::callback() const
{
    return d_callback;
}

inline
const bsl::string& ControlManager::ControlManager_Entry::arguments() const
{
    return d_arguments;
}

inline
const bsl::string& ControlManager::ControlManager_Entry::description() const
{
    return d_description;
}

                            // --------------------
                            // class ControlManager
                            // --------------------

// MANIPULATORS
inline
int ControlManager::deregisterUsageHandler()
{
    return deregisterHandler("HELP");
}

// ACCESSORS

inline
void ControlManager::printUsageHelper(bsl::ostream            *stream,
                                      const bsl::string_view&  preamble) const
{
    printUsage(*stream, preamble);
}

                                  // Aspects

inline
bslma::Allocator *ControlManager::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
