// balb_controlmanager.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALB_CONTROLMANAGER
#define INCLUDED_BALB_CONTROLMANAGER

//@PURPOSE: Provide a mechanism for mapping control messages to callbacks.
//
//@CLASSES:
//   balb::ControlManager: mechanism that maps control messages
//
//@DESCRIPTION: The 'balb::ControlManager' mechanism provided by this component
// maps control messages to callback functions on the basis of message
// prefixes.
//
///Callback Function Requirements
///------------------------------
// Functions registered as callbacks for messages must be invokable as
// 'void(*)(const bsl::string&, bsl::istream&).  (This signature is
// 'balb::ControlManager::ControlHandler').  When the function is invoked, the
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
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating an ECHO Message Handler
///- - - - - - - - - - - - - - - - - - - - - -
// First define a trivial callback to be invoked when an "ECHO" message is
// received:
//..
//  void onEcho(const bsl::string& prefix, bsl::istream& stream)
//  {
//     bsl::string word;
//     bsl::cout << "onEcho: \"" << prefix;
//     while (stream.good()) {
//        stream >> word;
//        bsl::cout << ' ' << word;
//     }
//     bsl::cout << '\"' << bsl::endl;
//  }
//..
// Now create a 'balb::ControlManager' object and register a handler for
// "ECHO".  Also register a handler for HELP to observe the auto-generated
// documentation for ECHO:
//..
//  balb::ControlManager manager;
//  manager.registerHandler("ECHO", "<text>",
//                          "Print specified text to the standard output",
//                          &onEcho);
//  manager.registerHandler("HELP", "",
//                          "Print documentation",
//                          bdlf::BindUtil::bind(
//                                  &balb::ControlManager::printUsageHelper,
//                                  &manager, &bsl::cout, bsl::string(
//               "The following commands are accepted by the test driver:")));
//
//  manager.dispatchMessage("ECHO repeat this text");
//  manager.dispatchMessage("echo matching is case-insensitive");
//  manager.dispatchMessage("HELP");
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_RWMUTEX
#include <bslmt_rwmutex.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {


namespace balb {

class ControlManager_Entry;

                            // ====================
                            // class ControlManager
                            // ====================

class ControlManager {
    // Dispatch control messages to callbacks by name.

    // PRIVATE TYPES
    typedef bsl::map<bsl::string,
                     ControlManager_Entry,
                     bool(*)(const bsl::string&, const bsl::string&)> Registry;
        // Defines a type alias for the ordered associative data structure
        // that maps a message prefix to a 'StringComparator' functor.

    // INSTANCE DATA
    bslma::Allocator       *d_allocator_p;    // memory allocator (held)
    Registry                d_registry;       // registry
    mutable bslmt::RWMutex  d_registryMutex;  // registry mutex

    // NOT IMPLEMENTED
    ControlManager(const ControlManager&);             // = deleted
    ControlManager& operator=(const ControlManager&);  // = deletd

  public:
    // TYPES
    typedef bsl::function<void(const bsl::string& prefix,
                               bsl::istream&      stream)> ControlHandler;
        // Defines a type alias for the function called to handle control
        // messages.  The 'prefix' argument is the first space-delimited word
        // read from the message, and the 'stream' argument is the
        // 'bsl::istream' containing the remainder of the message.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ControlManager,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    ControlManager(bslma::Allocator *basicAllocator = 0);
        // Create a control manager object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~ControlManager();
        // Destroy this object.

    // MANIPULATORS
    int registerHandler(const bsl::string&    prefix,
                        const bsl::string&    arguments,
                        const bsl::string&    description,
                        const ControlHandler& handler);
        // Register the specified 'handler' to be invoked whenever a control
        // message having the specified case-insensitive 'prefix' is received
        // for this control manager.  Also register the specified 'arguments'
        // string to describe the arguments accepted by the message, and the
        // specified 'description' to describe its operation; these are
        // printed by 'printUsage'.  Return a positive value if an existing
        // callback was replaced, return 0 if no replacement occurred, and
        // return a negative value otherwise.

    int deregisterHandler(const bsl::string& prefix);
        // Deregister the callback function previously registered to handle the
        // specified 'prefix'.  Return 0 on success or a non-zero value
        // otherwise.

    // ACCESSOR
    int dispatchMessage(const bsl::string& message) const;
        // Parse the specified complete 'message' and dispatch it.  Return
        // 0 on success, and a non-zero value otherwise; in particular return
        // non-zero if no registered callback could be found for the
        // case-insensitive prefix in 'message'.

    int dispatchMessage(const bsl::string& prefix, bsl::istream& stream) const;
        // Dispatch the message contained in the specified 'stream' to the
        // callback associated with the specified 'prefix'.  Return 0 on
        // success, and a non-zero value otherwise; in particular return
        // non-zero if no registered callback could be found for the
        // case-insensitive 'prefix'.

    void printUsage(bsl::ostream& stream, const bsl::string& preamble) const;
        // Print to the specified 'stream' the specified 'preamble' text,
        // followed by the registered commands and documentation for this
        // control manager.  Note that a newline is appended to 'preamble' in
        // the output.

    void printUsageHelper(bsl::ostream       *stream,
                          const bsl::string&  preamble) const;
        // Invoke 'printUsage' passing the specified '*stream' and 'preamble'.
        // Suitable for binding using the bdlf::BindUtil package.

};

                         // ==========================
                         // class ControlManager_Entry
                         // ==========================

class ControlManager_Entry {
    // This component-private class represents a function with documentation.

    // INSTANCE DATA
    ControlManager::ControlHandler d_callback;    // processing callback
    bsl::string                    d_arguments;   // argument description
    bsl::string                    d_description; // function description

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ControlManager_Entry,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    ControlManager_Entry(bslma::Allocator *basicAllocator = 0);
        // Create a 'ControlManager_Entry' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ControlManager_Entry(
                    const ControlManager::ControlHandler&  callback,
                    const bsl::string&                     arguments,
                    const bsl::string&                     description,
                    bslma::Allocator                      *basicAllocator = 0);
        // Create an ControlManager_Entry object with the specified initial
        // values.

    ControlManager_Entry(const ControlManager_Entry&  original,
                         bslma::Allocator            *basicAllocator = 0);
        // Create an ControlManager_Entry object having the value of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~ControlManager_Entry();
        // Destroy this object.

    // MANIPULATORS
    ControlManager_Entry& operator=(const ControlManager_Entry& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void setCallback(const ControlManager::ControlHandler& callback);
        // Set the specified 'callback' as the value of the 'callback' member
        // of this object.

    bsl::string& arguments();
        // Return a modifiable reference to the 'arguments' member of this
        // object.

    bsl::string& description();
        // Return a modifiable reference to the 'description' member of this
        // object.

    // ACCESSORS
    const ControlManager::ControlHandler& callback() const;
        // Return a non-modifiable reference to the 'callback' member of this
        // object.

    const bsl::string& arguments() const;
        // Return a non-modifiable reference to the 'arguments' member of this
        // object.

    const bsl::string& description() const;
        // Return a non-modifiable reference to the 'arguments' member of this
        // object.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // --------------------------
                        // class ControlManager_Entry
                        // --------------------------

// MANIPULATORS
inline
void ControlManager_Entry::setCallback(
                                const ControlManager::ControlHandler& callback)
{
    d_callback = callback;
}

inline
bsl::string& ControlManager_Entry::arguments()
{
    return d_arguments;
}

inline
bsl::string& ControlManager_Entry::description()
{
    return d_description;
}

// ACCESSORS
inline
const ControlManager::ControlHandler& ControlManager_Entry::callback() const
{
    return d_callback;
}

inline
const bsl::string& ControlManager_Entry::arguments() const
{
    return d_arguments;
}

inline
const bsl::string& ControlManager_Entry::description() const
{
    return d_description;
}

                            // --------------------
                            // class ControlManager
                            // --------------------

// ACCESSORS
inline
void ControlManager::printUsageHelper(bsl::ostream       *stream,
                                      const bsl::string&  preamble) const
{
    printUsage(*stream, preamble);
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
