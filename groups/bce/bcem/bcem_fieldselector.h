// bcem_fieldselector.h                                               -*-C++-*-
#ifndef INCLUDED_BCEM_FIELDSELECTOR
#define INCLUDED_BCEM_FIELDSELECTOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

// TBD: Review doc
//@PURPOSE: Provide an attribute class for specifying a field selector.
//
//@CLASSES:
//  bcem_FieldSelector: selector for a field
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bcem_aggregate, bcem_aggregateraw
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'bcem_FieldSelector', that is used
// to provide a selector for a field.
//
///Usage
///-----
// This section illustrates intended use of this component.
//

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BDEM_RECORDDEF
#include <bdem_recorddef.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                        // ========================
                        // class bcem_FieldSelector
                        // ========================

class bcem_FieldSelector {
    // This class holds a *temporary* name string, an integer index, or neither
    // (the "empty" state).  It has conversion constructors from 'bsl::string',
    // 'const char *', and 'int'.  It does not own its string data.  This class
    // has in-core value semantics, except that it lacks printing support.

    enum {
        // Select name, index, or neither ('BCEM_NOI_EMPTY').

        BCEM_NOI_EMPTY,
        BCEM_NOI_NAME,
        BCEM_NOI_INDEX
    } d_state;

    union {
        const char *d_name_p; // set if 'BCEM_NOI_NAME' (held, *not* owned)
        int         d_index;  // set if 'BCEM_NOI_INDEX'
    };

  public:
    // CREATORS
    bcem_FieldSelector();
        // Create an empty field selector having no name or index.

    bcem_FieldSelector(const char *name);
        // Create a field selector having the specified 'name'.  The behavior
        // is undefined unless the character array pointed to by 'name' is
        // unchanged and remains valid during the lifetime of this object.

    bcem_FieldSelector(const bsl::string& name);
        // Create a field selector having the specified 'name'.  The behavior
        // is undefined unless 'name' is unchanged during the lifetime of this
        // object.

    bcem_FieldSelector(int index);
        // Create a field selector having the specified 'index'.  The behavior
        // is undefined unless 'index >= 0'.

    // ACCESSORS
    bool isEmpty() const;
        // Return 'true' if this is an empty field selector, and 'false'
        // otherwise.  An empty field selector has neither a name nor an index.

    bool isName() const;
        // Return 'true' if this field selector has a name, and 'false'
        // otherwise.

    bool isIndex() const;
        // Return 'true' if this field selector has an integral index, and
        // 'false' otherwise.

    const char *name() const;
        // Return the name of this field selector if 'isName() == true', and 0
        // otherwise.

    int index() const;
        // Return the integral index of this field selector if
        // 'isIndex() == true', and 'bdem_RecordDef::BDEM_NULL_FIELD_ID'
        // otherwise.
};

                     //-------------------------
                     // class bcem_FieldSelector
                     //-------------------------

// CREATORS
inline
bcem_FieldSelector::bcem_FieldSelector()
: d_state(BCEM_NOI_EMPTY)
, d_name_p(0)
{
}

inline
bcem_FieldSelector::bcem_FieldSelector(const char *name)
: d_state(BCEM_NOI_NAME)
, d_name_p(name)
{
}

inline
bcem_FieldSelector::bcem_FieldSelector(const bsl::string& name)
: d_state(BCEM_NOI_NAME)
, d_name_p(name.c_str())
{
}

inline
bcem_FieldSelector::bcem_FieldSelector(int index)
: d_state(BCEM_NOI_INDEX)
, d_index(index)
{
}

// ACCESSORS
inline
bool bcem_FieldSelector::isEmpty() const
{
    return BCEM_NOI_EMPTY == d_state;
}

inline
bool bcem_FieldSelector::isName() const
{
    return BCEM_NOI_NAME == d_state;
}

inline
bool bcem_FieldSelector::isIndex() const
{
    return BCEM_NOI_INDEX == d_state;
}

inline
const char *bcem_FieldSelector::name() const
{
    return BCEM_NOI_NAME == d_state ? d_name_p : 0;
}

inline
int bcem_FieldSelector::index() const
{
    return BCEM_NOI_INDEX == d_state
                           ? d_index
                           : bdem_RecordDef::BDEM_NULL_FIELD_ID;
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
