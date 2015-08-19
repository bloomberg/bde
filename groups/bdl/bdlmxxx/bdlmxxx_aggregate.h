// bdlmxxx_aggregate.h                                                -*-C++-*-
#ifndef INCLUDED_BDLMXXX_AGGREGATE
#define INCLUDED_BDLMXXX_AGGREGATE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an aggregation of various 'bdem' containers.
//
//@CLASSES:
//
//@SEE_ALSO: bdlmxxx_choice, bdlmxxx_choicearray, bdlmxxx_list, bdlmxxx_table
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
// 'bdlmxxx::Choice', 'bdlmxxx::ChoiceArray', 'bdlmxxx::List', and 'bdlmxxx::Table' that allow
// clients to construct arbitrary hierarchical data.  Each 'bdem' container
// may hold one or more objects of one of the 'bdem' element types, which are
// enumerated in 'bdlmxxx_elemtype'.  This component also provides access to two
// additional 'bdem' aggregate types, 'bdlmxxx::Row' and 'bdlmxxx::ChoiceArrayItem',
// that cannot be independently created, but are exposed in the interface of
// the other four aggregate container types.
//
///Synopsis of Aggregate Types
///---------------------------
// The following is a brief synopsis of 'bdem' aggregate types.  A 'bdlmxxx::List'
// contains a single row ('bdlmxxx::Row') representing a heterogeneous, indexable
// sequence of scalars, arrays, and/or other aggregates.  A 'bdlmxxx::Table'
// contains an indexable, homogeneous sequence of 'bdlmxxx::Row' objects, each
// having the same sequence of element types.  A 'bdlmxxx::Choice' contains a
// single item (of type 'bdlmxxx::ChoiceArrayItem') representing a catalog of
// 'bdem' types and a single value that is of one of the types from that type
// catalog (or is null).  A 'bdlmxxx::ChoiceArray' contains a homogeneous,
// indexable sequence of 'bdlmxxx::ChoiceArrayItem' objects that share a common
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
//  scalar (BDEM_INT, etc.)           bdltuxxx::Unset<TYPE>::unsetValue()
//
//  vector (BDEM_INT_ARRAY, etc.)     0 == size()
//..
//
///Usage
///-----
// This component defines no classes and therefore a usage example is not
// provided.  Please refer to the components that define the individual 'bdem'
// aggregate types ('bdlmxxx_choice', 'bdlmxxx_choicearray', 'bdlmxxx_choicearrayitem',
// 'bdlmxxx_list', 'bdlmxxx_row', and 'bdlmxxx_table') for guidance on usage.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICE
#include <bdlmxxx_choice.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEARRAY
#include <bdlmxxx_choicearray.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEARRAYITEM
#include <bdlmxxx_choicearrayitem.h>
#endif

#ifndef INCLUDED_BDLMXXX_LIST
#include <bdlmxxx_list.h>
#endif

#ifndef INCLUDED_BDLMXXX_ROW
#include <bdlmxxx_row.h>
#endif

#ifndef INCLUDED_BDLMXXX_TABLE
#include <bdlmxxx_table.h>
#endif

namespace BloombergLP {

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
