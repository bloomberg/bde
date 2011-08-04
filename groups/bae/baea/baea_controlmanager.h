// baea_controlmanager.h                                              -*-C++-*-
#ifndef INCLUDED_BAEA_CONTROLMANAGER
#define INCLUDED_BAEA_CONTROLMANAGER

//@PURPOSE: Provide a mechanism for mapping control messages to callbacks
//
//@CLASSES:
//   baea_ControlManager: mechanism that maps control messages
//
//@AUTHOR:  David Schumann (dschumann1@bloomberg.net)
//
//@DESCRIPTION: The 'baea_ControlManager' mechanism provided by this component
// maps control messages to callback functions on the basis of message
// prefixes.
//
///Callback Function Requirements
///------------------------------
// Functions registered as callbacks for messages must be invokable as
// 'void(*)(const bsl::string&, bsl::istream&).  (This signature is
// 'baea_ControlManager::ControlHandler').  When the function is invoked,
// the first argument is the message prefix, and the second is a stream
// on the remainder of the message.
//
///Thread Safety
///-------------
// This component is thread-safe and thread-enabled: it is safe to access and
// manipulate multiple distinct instances from different threads, and it is
// safe to access and manipulate a single shared instance from different
// threads.
//
///Usage Example
///-------------
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
// Now create a 'baea_ControlManager' object and register a handler for "ECHO".
// Also register a handler for HELP to observe the auto-generated
// documentation for ECHO:
//..
//  baea_ControlManager manager;
//  manager.registerHandler("ECHO", "<text>",
//                          "Print specified text to the standard output",
//                          &onEcho);
//  manager.registerHandler("HELP", "",
//                          "Print documentation",
//                          bdef_BindUtil::bind(
//                                  &baea_ControlManager::printUsageHelper,
//                                  &manager, &bsl::cout, bsl::string(
//               "The following commands are accepted by the test driver:")));
//
//  manager.dispatchMessage("ECHO repeat this text");
//  manager.dispatchMessage("echo matching is case-insensitive");
//  manager.dispatchMessage("HELP");
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_RWMUTEX
#include <bcemt_rwmutex.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

namespace BloombergLP {

class bslma_Allocator;
class baea_ControlManager_Entry;

                         // =========================
                         // class baea_ControlManager
                         // =========================

class baea_ControlManager {
    // Dispatch control messages to callbacks by name.

    // PRIVATE TYPES
    typedef bsl::map<bsl::string, baea_ControlManager_Entry,
                     bool(*)(const bsl::string&, const bsl::string&)>
                                                                   Registry;
        // Defines a type alias for the ordered associative data structure
        // that maps a message prefix to a 'StringComparator' functor.

    // INSTANCE DATA
    bslma_Allocator       *d_allocator_p;           // memory allocator (held)
    Registry               d_registry;              // registry
    mutable bcemt_RWMutex  d_registryMutex;         // registry mutex

  public:
    // TYPES
    typedef bdef_Function<void (*)(const bsl::string& prefix,
                                   bsl::istream&      stream)> ControlHandler;
        // Defines a type alias for the function called to handle control
        // messages.  The 'prefix' argument is the first space-delimited word
        // read from the message, and the 'stream' argument is the
        // 'bsl::istream' containing the remainder of the message.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baea_ControlManager,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    baea_ControlManager(bslma_Allocator *basicAllocator = 0);
        // Create a control manager object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~baea_ControlManager();
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

    int dispatchMessage(const bsl::string& prefix,
                        bsl::istream&      stream) const;
        // Dispatch the message contained in the specified 'stream' to the
        // callback associated with the specified 'prefix'.  Return 0 on
        // success, and a non-zero value otherwise; in particular return
        // non-zero if no registered callback could be found for the
        // case-insensitive 'prefix'.

    void printUsage(bsl::ostream       &stream,
                    const bsl::string&  preamble) const;
        // Print to the specified 'stream' the specified 'preamble' text,
        // followed by the registered commands and documentation for this
        // control manager.  Note that a newline is appended to 'preamble'
        // in the output.

    void printUsageHelper(bsl::ostream       *stream,
                          const bsl::string&  preamble) const;
        // Invoke 'printUsage(*stream, preamble)'.  Suitable for binding
        // using the bdef_BindUtil package.

};

                      // ===============================
                      // class baea_ControlManager_Entry
                      // ===============================

class baea_ControlManager_Entry {
    // This component-private class represents a function with documentation.

    // INSTANCE DATA
    baea_ControlManager::ControlHandler d_callback;    // processing callback
    bsl::string                         d_arguments;   // argument description
    bsl::string                         d_description; // function description

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baea_ControlManager_Entry,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    baea_ControlManager_Entry(bslma_Allocator *basicAllocator = 0);
        // Create a 'baea_ControlManager_Entry' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    baea_ControlManager_Entry(
            const baea_ControlManager::ControlHandler&  callback,
            const bsl::string&                          arguments,
            const bsl::string&                          description,
            bslma_Allocator                            *basicAllocator = 0);
        // Create an baea_ControlManager_Entry object with the specified
        // initial values.

    baea_ControlManager_Entry(
            const baea_ControlManager_Entry&  original,
            bslma_Allocator                 *basicAllocator=0);
        // Create an baea_ControlManager_Entry object having the value of the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~baea_ControlManager_Entry();
        // Destroy this object.

    // MANIPULATORS
    baea_ControlManager_Entry& operator=(const baea_ControlManager_Entry& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void setCallback(const baea_ControlManager::ControlHandler& callback);
        // Set the value of the 'callback' member of this object to the
        // specified 'callback'.

    bsl::string& arguments();
        // Return a modifiable reference to the 'arguments' member of this
        // object.

    bsl::string& description();
        // Return a modifiable reference to the 'description' member of
        // this object.

    // ACCESSORS
    const baea_ControlManager::ControlHandler& callback() const;
        // Return a non-modifiable reference to the 'callback' member of
        // this object.

    const bsl::string& arguments() const;
        // Return a non-modifiable reference to the 'arguments' member of
        // this object.

    const bsl::string& description() const;
        // Return a non-modifiable reference to the 'arguments' member of
        // this object.
};

// ============================================================================
//                          INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // --------------------------
                         // class ControlManager_Entry
                         // --------------------------

// MANIPULATORS
inline
void baea_ControlManager_Entry::setCallback(
            const baea_ControlManager::ControlHandler& function)
{
    d_callback = function;
}

inline
bsl::string& baea_ControlManager_Entry::arguments()
{
    return d_arguments;
}

inline
bsl::string& baea_ControlManager_Entry::description()
{
    return d_description;
}

// ACCESSORS
inline
const baea_ControlManager::ControlHandler&
baea_ControlManager_Entry::callback() const
{
    return d_callback;
}

inline
const bsl::string& baea_ControlManager_Entry::arguments() const
{
    return d_arguments;
}

inline
const bsl::string& baea_ControlManager_Entry::description() const
{
    return d_description;
}

                         // -------------------------
                         // class baea_ControlManager
                         // -------------------------

// ACCESSORS
inline
void baea_ControlManager::printUsageHelper(bsl::ostream       *stream,
                                           const bsl::string&  preamble) const
{
    printUsage(*stream, preamble);
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
