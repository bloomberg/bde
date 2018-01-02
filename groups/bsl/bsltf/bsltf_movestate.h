// bsltf_movestate.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLTF_MOVESTATE
#define INCLUDED_BDLTF_MOVESTATE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumeration of move-state, including unsupported.
//
//@CLASSES:
//  bsltf::MoveState: namespace for move-state enumeration
//
//@DESCRIPTION: This component provides a 'struct', 'bsltf_MoveState', which
// serves as a namespace for enumerating the move-state of an object, including
// an unknown value indicating that the test type does not support tracking
// of this information.  An object is involved in a move operation if that
// object was either the source or target of a move construction or move
// assignment.  It's up to the test type to support moved-into or moved-from
// or both using separate instances of this enum.
//
///Enumerators
///-----------
//..
//  Name          Description
//  -----------   -------------------------------------------------------------
//  e_NOT_MOVED   The type was not involved in a move operation.
//
//  e_MOVED       The type was involved in a move operation.
//
//  e_UNKNOWN     The type does not expose move-state infromation.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code provide a simple illustration of using
// 'bsltf::MoveState'.
//
// First, we create a variable 'value' of type 'bsltf::MoveState::Enum'
// and initialize it with the enumerator value
// 'bsltf::MoveState::e_MOVED':
//..
//  bsltf::MoveState::Enum value = bsltf::MoveState::e_MOVED;
//..
// Now, we store the address of its ASCII representation in a pointer variable,
// 'asciiValue', of type 'const char *':
//..
//  const char *asciiValue = bsltf::MoveState::toAscii(value);
//..
// Finally, we verify the value of 'asciiValue':
//..
//  assert(0 == strcmp(asciiValue, "MOVED"));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_STDIO_H
#include <stdio.h>  // for 'printf'
#define INCLUDED_STDIO_H
#endif

namespace BloombergLP {
namespace bsltf {

struct MoveState {
  public:
    // TYPES
    enum Enum {
        // Enumeration of move state.

        e_NOT_MOVED,  // The type was not involved in a move operation.

        e_MOVED,      // The type was involved in a move operation.

        e_UNKNOWN     // The type does not expose move-state information.
    };

  public:
    // CLASS METHOD
    static const char *toAscii(MoveState::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << MoveState::toAscii(MoveState::e_MOVED);
        //..
        // will print the following on standard output:
        //..
        //  MOVED
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.

};

// FREE FUNCTIONS
inline
void debugprint(const MoveState::Enum& value)
    // Print the specified 'value' as a string.
{
    printf("%s", MoveState::toAscii(value));
}

}  // close namespace bsltf
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
