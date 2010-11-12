// bdem_aggregate.h                                                   -*-C++-*-
#ifndef INCLUDED_BDEM_AGGREGATE
#define INCLUDED_BDEM_AGGREGATE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an aggregation of various 'bdem' containers.
//
//@CLASSES:
//
//@SEE_ALSO: bdem_choice, bdem_choicearray, bdem_list, bdem_table
//
//@AUTHOR: Dan Glaser (dglaser)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component defines no classes or functions.  Its main
// purpose is to aggregate the various 'bdem' types in a single file making
// them conveniently available to clients and allow testing them in totality.
//
// This component provides access to the four 'bdem' container classes,
// 'bdem_Choice', 'bdem_ChoiceArray', 'bdem_List', and 'bdem_Table' that allow
// clients to construct arbitrary hierarchical data.  Each 'bdem' container
// may hold one or more objects of one of the 'bdem' element types, which are
// enumerated in 'bdem_elemtype'.  This component also provides access to two
// additional 'bdem' aggregate types, 'bdem_Row' and 'bdem_ChoiceArrayItem',
// that cannot be independently created, but are exposed in the interface of
// the other four aggregate container types.
//
///Synopsis of Aggregate Types
///---------------------------
// The following is a brief synopsis of 'bdem' aggregate types.  A 'bdem_List'
// contains a single row ('bdem_Row') representing a heterogeneous, indexable
// sequence of scalars, arrays, and/or other aggregates.  A 'bdem_Table'
// contains an indexable, homogeneous sequence of 'bdem_Row' objects, each
// having the same sequence of element types.  A 'bdem_Choice' contains a
// single item (of type 'bdem_ChoiceArrayItem') representing a catalog of
// 'bdem' types and a single value that is of one of the types from that type
// catalog (or is null).  A 'bdem_ChoiceArray' contains a homogeneous,
// indexable sequence of 'bdem_ChoiceArrayItem' objects that share a common
// catalog of 'bdem' types and hold a value of one of the types (or are null).
//
///'bdem' Null States
///------------------
// The concept of null applies to each 'bdem' type.  In addition to the range
// of values in a given 'bdem' type's domain (e.g., '[ INT_MIN .. INT_MAX ]'
// for 'BDEM_INT'), each type has a null value.  When a 'bdem' element is null,
// it has an underlying (unique) designated unset value (or state) as indicated
// in the following table:
//..
//       'bdem' element type                 "unset" value (or state)
//  ------------------------------    --------------------------------------
//  BDEM_CHOICE and
//  BDEM_CHOICE_ARRAY_ITEM            selector() < 0 && 0 == numSelections()
//
//  BDEM_CHOICE_ARRAY                 0 == length()  && 0 == numSelections()
//
//  BDEM_LIST                         0 == length()
//
//  BDEM_ROW                          All the elements in the row are "null"
//
//  BDEM_TABLE                        0 == numRows() && 0 == numColumns()
//
//  scalar (BDEM_INT, etc.)           bdetu_Unset<TYPE>::unsetValue()
//
//  vector (BDEM_INT_ARRAY, etc.)     0 == size()
//..
//
///Usage
///-----
// This component defines no classes and therefore a usage example is not
// provided.  Please refer to the components that define the individual 'bdem'
// aggregate types ('bdem_choice', 'bdem_choicearray', 'bdem_choicearrayitem',
// 'bdem_list', 'bdem_row', and 'bdem_table') for guidance on usage.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_CHOICE
#include <bdem_choice.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEARRAY
#include <bdem_choicearray.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEARRAYITEM
#include <bdem_choicearrayitem.h>
#endif

#ifndef INCLUDED_BDEM_LIST
#include <bdem_list.h>
#endif

#ifndef INCLUDED_BDEM_ROW
#include <bdem_row.h>
#endif

#ifndef INCLUDED_BDEM_TABLE
#include <bdem_table.h>
#endif

namespace BloombergLP {

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
