// bdem_binding.t.cpp                                                 -*-C++-*-

#include <bdem_binding.h>

#include <bdet_datetime.h>
#include <bdet_date.h>
#include <bdet_time.h>

#include <bdet_datetimetz.h>
#include <bdet_datetz.h>
#include <bdet_timetz.h>

#include <bslma_testallocator.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_exception.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>      // 'rand'

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// TBD tests with data length > meta-data length

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This component implements five families of classes, with the classes in a
// family providing indexed and *by-name* access to a proxied 'bdem_Row', a
// proxied 'bdem_Table', a proxied column in a 'bdem_Table',
// a proxied 'bdem_Choice', or a proxied 'bdem_ChoiceArray'.  Within the
// Table, Row, and Column families, there are four classes that
// provide 'const' and non-'const' access to its referenced row, table, or
// column.  For choice and choice array families there are only two classes,
// one for 'const' access the other for 'non-const' access.  In short, each
// class binds data (a 'bdem_Row', 'bdem_Table', 'bdem_Choice' or
// 'bdem_ChoiceArray') to meta-data (a 'bdem_RecordDef' or a 'bdem_FieldDef').
// Each binding class contains multiple creators, manipulators, and accessors,
// including functions that return other bindings (by value).
//
// In the following, we use "underlying data" to refer to the 'bdem_Row',
// 'bdem_Table', 'bdem_Choice', or 'bdem_ChoiceArray' proxied by the
// binding.  We use "constraint" to refer to the 'bdem_RecordDef' or
// 'bdem_FieldDef' (meta-data) held by the binding.
//
// The general plan for this test driver is as follows (applied to all 12
// binding classes):
//
// 1) Test CREATORS and DIRECT ACCESSORS: Ensure that bindings are created
//    properly.  Bindings hold pointers to their underlying data and
//    constraint.  In testing the creators, confirm that both pointers are
//    *installed* correctly by employing the direct accessors.
//
// 2) Test ACCESSORS: The (non-direct) accessors of bindings perform simple
//    function-forwarding to the appropriate methods corresponding to the
//    underlying data.  After ascertaining that the bindings are created
//    correctly, check that the accessors forward arguments and return values
//    properly.
//
// 3) Test MANIPULATORS: All of the binding classes provide manipulators that
//    allow the underlying data and (or) constraint to be "rebound" (via the
//    'rebind' methods).  As with the creators, confirm that the pointers held
//    by the binds are *installed* correctly when rebound.
//
// 4) Test BINDING-CREATING FUNCTIONS: All of the binding classes provide
//    methods that create, and return by-value, other bindings.  Ensure that
//    returned bindings refer to the correct underlying data and constraint.
//
// 5) Test EQUALITY OPERATORS: Free operators 'operator==' and 'operator!=' are
//    provided for equality comparison of binding instances.
//
// General concerns:
//   (1) Without explicit casting, ensure that it is not possible to modify the
//       underlying data via a 'const' binding.  Additionally, ensure by a
//       negative (manual) test that modifiable bindings cannot be created from
//       'const' underlying data.
//
// Test case organization:
//   o Verify base-level functionality in the Breathing Test (case 1).  Define
//     a constraint and create underlying data satisfying that constraint.
//     Create several bindings from the underlying data and constraint, and
//     ensure that at least one function from each of the creators,
//     manipulators, accessors, and binding-creating functions work correctly.
//   o To test the binding methods, we need instances of 'bdem_List',
//     'bdem_Table', 'bdem_Choice', 'bdem_ChoiceArray', and 'bdem_Schema'.
//     This data is created by the 'createSchema', 'createList', and
//     'createTable' helper functions.  The helper functions are verified
//     in case 2.
//   o Ensure that both CREATORs work correctly for all five families of
//     bindings.
//   o Verify that ACCESSORs work correctly for all five families of bindings.
//   o Confirm that MANIPULATOR functions work correctly for all five families
//     of bindings.
//   o Test the equality operators.
//   o Verify that the BINDING-CREATING FUNCTIONS work correctly for all five
//     families of bindings.
//   o Finally, verify the Usage example.
//-----------------------------------------------------------------------------
//                         ==========================
//                         class bdem_ConstRowBinding
//                         ==========================
// CREATORS
// [ 3] bdem_ConstRowBinding(const Row *, *record);
// [ 3] bdem_ConstRowBinding(const Row *, *schema, *name);
// [ 3] bdem_ConstRowBinding(const Row *, *schema, int);
// [ 3] bdem_ConstRowBinding(const List *, *record);
// [ 3] bdem_ConstRowBinding(const List *, *schema, *name);
// [ 3] bdem_ConstRowBinding(const List *, *schema, int,
// [ 3] bdem_ConstRowBinding(const Table *, int, *record);
// [ 3] bdem_ConstRowBinding(const Table *, int, *schema, *name);
// [ 3] bdem_ConstRowBinding(const Table *, int, *schema, int);
// [ 3] bdem_ConstRowBinding(const& original);
// [ 3] ~bdem_ConstRowBinding();
//
// MANIPULATORS
// [12] void rebind(const bdem_ConstRowBinding& binding);
// [27] void rebind(const Row *);
// [27] void rebind(const List *);
// [27] void rebind(const Table *, int);
// [12] void rebind(const Row *, *record);
// [12] void rebind(const Row *, *schema, *name);
// [12] void rebind(const Row *, *schema, int);
// [12] void rebind(const List *, *record);
// [12] void rebind(const List *, *schema, *name);
// [12] void rebind(const List *, *schema, int);
// [12] void rebind(const Table *, int, *record);
// [12] void rebind(const Table *, int, *schema, *name);
// [12] void rebind(const Table *, int, *schema, int);
//
// ACCESSORS
// [8] bdem_ConstElemRef operator[](int) const;
// [8] bdem_ConstElemRef element(*name) const;
// [8] bdem_ConstElemRef element(int) const;
// [8] bdem_ElemType::Type elemType(*name) const;
// [8] bdem_ElemType::Type elemType(int) const;
// [ 3] int length() const;
// [ 3] const bdem_RecordDef& record() const;
// [ 3] const bdem_Row& row() const;
// [8] const char& theChar(*name) const;
// [8] const short& theShort(*name) const;
// [8] const int& theInt(*name) const;
// [8] const Int64& theInt64(*name) const;
// [8] const float& theFloat(*name) const;
// [8] const double& theDouble(*name) const;
// [8] const string& theString(*name) const;
// [8] const bdet_Datetime& theDatetime(*name) const;
// [8] const bdet_Date& theDate(*name) const;
// [8] const bdet_Time& theTime(*name) const;
// [8] const vector<char>& theCharArray(*name) const;
// [8] const vector<short>& theShortArray(*name) const;
// [8] const vector<int>& theIntArray(*name) const;
// [8] const vector<Int64>& theInt64Array(*name) const;
// [8] const vector<float>& theFloatArray(*name) const;
// [8] const vector<double>& theDoubleArray(*name) const;
// [8] const vector<string>& theStringArray(*name) const;
// [8] const vector<Datetime>& theDatetimeArray(*name) const;
// [8] const vector<Date>& theDateArray(*name) const;
// [8] const vector<Time>& theTimeArray(*name) const;
// [8] const bdem_List& theList(*name) const;
// [8] const bdem_Table& theTable(*name) const;
// [8] const bdem_Bool& theBool(*name) const;
// [8] const bdem_DatetimeTz& theDatetimeTz(*name) const;
// [8] const bdem_DateTz& theDateTz(*name) const;
// [8] const bdem_TimeTz& theTimeTz(*name) const;
// [8] const bdem_BoolArray& theBoolArray(*name) const;
// [8] const bdem_DatetimeTzArray& theDatetimeTzArray(*name) const;
// [8] const bdem_DateTzArray& theDateTzArray(*name) const;
// [8] const bdem_TimeTzArray& theTimeTzArray(*name) const;
// [8] const bdem_Choice& theChoice(*name) const;
// [8] const bdem_ChoiceArray& theChoiceArray(int) const;
// [22] bdem_ConstRowBinding rowBinding(*name) const;
// [22] bdem_ConstRowBinding rowBinding(int) const;
// [22] bdem_ConstTableBinding tableBinding(*name) const;
// [22] bdem_ConstTableBinding tableBinding(int) const;
// [22] bdem_ConstChoiceBinding choiceBinding(*name) const;
// [22] bdem_ConstChoiceBinding choiceBinding(int) const;
// [22] bdem_ConstChoiceArrayBinding choiceArrayBinding(*name) const;
// [22] bdem_ConstChoiceArrayBinding choiceArrayBinding(int) const;
//
// FREE OPERATORS
// [17] bool operator==(const ConstRowBinding& lhs, const& rhs);
// [17] bool operator!=(const ConstRowBinding& lhs, const& rhs);
//
//                         =====================
//                         class bdem_RowBinding
//                         =====================
// CREATORS
// [ 3] bdem_RowBinding(Row *, *record);
// [ 3] bdem_RowBinding(Row *, *schema, *name);
// [ 3] bdem_RowBinding(Row *, *schema, int);
// [ 3] bdem_RowBinding(List *, *record);
// [ 3] bdem_RowBinding(List *, *schema, *name);
// [ 3] bdem_RowBinding(List *, *schema, int,
// [ 3] bdem_RowBinding(Table *, int, *record);
// [ 3] bdem_RowBinding(Table *, int, *schema, *name);
// [ 3] bdem_RowBinding(Table *, int, *schema, int);
// [ 3] bdem_RowBinding(const& original);
// [ 3] ~bdem_RowBinding();
//
// MANIPULATORS
// [12] void rebind(const bdem_RowBinding& binding);
// [27] void rebind(Row *);
// [27] void rebind(List *);
// [27] void rebind(Table *, int);
// [12] void rebind(Row *, *record);
// [12] void rebind(Row *, *schema, *name);
// [12] void rebind(Row *, *schema, int);
// [12] void rebind(List *, *record);
// [12] void rebind(List *, *schema, *name);
// [12] void rebind(List *, *schema, int);
// [12] void rebind(Table *, int, *record);
// [12] void rebind(Table *, int, *schema, *name);
// [12] void rebind(Table *, int, *schema, int);
//
// ACCESSORS
// [8] bdem_ElemRef operator[](int) const;
// [8] bdem_ElemRef element(*name) const;
// [8] bdem_ElemRef element(int) const;
// [3] bdem_Row& row() const;
// [8] char& theChar(*name) const;
// [8] short& theShort(*name) const;
// [8] int& theInt(*name) const;
// [8] Int64& theInt64(*name) const;
// [8] float& theFloat(*name) const;
// [8] double& theDouble(*name) const;
// [8] string& theString(*name) const;
// [8] bdet_Datetime& theDatetime(*name) const;
// [8] bdet_Time& theTime(*name) const;
// [8] bdet_Date& theDate(*name) const;
// [8] vector<char>& theCharArray(*name) const;
// [8] vector<short>& theShortArray(*name) const;
// [8] vector<int>& theIntArray(*name) const;
// [8] vector<Int64>& theInt64Array(*name) const;
// [8] vector<float>& theFloatArray(*name) const;
// [8] vector<double>& theDoubleArray(*name) const;
// [8] vector<string>& theStringArray(*name) const;
// [8] vector<Datetime>& theDatetimeArray(*name) const;
// [8] vector<Date>& theDateArray(*name) const;
// [8] vector<Time>& theTimeArray(*name) const;
// [8] bdem_List& theList(*name) const;
// [8] bdem_Table& theTable(*name) const;
// [8] bdem_Bool& theBool(*name) const;
// [8] bdem_DatetimeTz& theDatetimeTz(*name) const;
// [8] bdem_DateTz& theDateTz(*name) const;
// [8] bdem_TimeTz& theTimeTz(*name) const;
// [8] bdem_BoolArray& theBoolArray(*name) const;
// [8] bdem_DatetimeTzArray& theDatetimeTzArray(*name) const;
// [8] bdem_DateTzArray& theDateTzArray(*name) const;
// [8] bdem_TimeTzArray& theTimeTzArray(*name) const;
// [8] bdem_Choice& theChoice(*name) const;
// [8] bdem_ChoiceArray& theChoiceArray(int) const;
// [22] bdem_RowBinding rowBinding(*name) const;
// [22] bdem_RowBinding rowBinding(int) const;
// [22] bdem_TableBinding tableBinding(*name) const;
// [22] bdem_TableBinding tableBinding(int) const;
// [22] bdem_ChoiceBinding choiceBinding(*name) const;
// [22] bdem_ChoiceBinding choiceBinding(int ) const;
// [22] bdem_ChoiceArrayBinding choiceArrayBinding(*name) const;
// [22] bdem_ChoiceArrayBinding choiceArrayBinding(int) const;
//
//                       ============================
//                       class bdem_ConstTableBinding
//                       ============================
// CREATORS
// [ 4] bdem_ConstTableBinding(const Table *, *record);
// [ 4] bdem_ConstTableBinding(const Table *, *schema, *name);
// [ 4] bdem_ConstTableBinding(const Table *, *schema, int);
// [ 4] bdem_ConstTableBinding(const& original);
// [ 4] ~bdem_ConstTableBinding();
//
// MANIPULATORS
// [13] void rebind(const bdem_ConstTableBinding& binding);
// [28] void rebind(const Table *);
// [13] void rebind(const Table *, *record);
// [13] void rebind(const Table *, *schema, *name);
// [13] void rebind(const Table *, *schema, int);
//
// ACCESSORS
// [ 4] const bdem_Row& operator[](int) const;
// [ 9] bdem_ConstElemRef element(int, *name) const;
// [ 9] bdem_ConstElemRef element(int, int) const;
// [ 9] bdem_ElemType::Type elemType(*name) const;
// [ 9] bdem_ElemType::Type elemType(int) const;
// [ 4] int numColumns() const;
// [ 4] int numRows() const;
// [ 4] const bdem_RecordDef& record() const;
// [ 4] const bdem_Table& table() const;
// [ 9] const char& theChar(int, *name) const;
// [ 9] const short& theShort(int, *name) const;
// [ 9] const int& theInt(int, *name) const;
// [ 9] const Int64& theInt64(int, *name) const;
// [ 9] const float& theFloat(int, *name) const;
// [ 9] const double& theDouble(int, *name) const;
// [ 9] const string& theString(int, *name) const;
// [ 9] const bdet_Datetime& theDatetime(int, *name) const;
// [ 9] const bdet_Date& theDate(int, *name) const;
// [ 9] const bdet_Time& theTime(int, *name) const;
// [ 9] const vector<char>& theCharArray(int, *name) const;
// [ 9] const vector<short>& theShortArray(int, *name) const;
// [ 9] const vector<int>& theIntArray(int, *name) const;
// [ 9] const vector<Int64>& theInt64Array(int, *name) const;
// [ 9] const vector<float>& theFloatArray(int, *name) const;
// [ 9] const vector<double>& theDoubleArray(int, *name) const;
// [ 9] const vector<string>& theStringArray(int, *name) const;
// [ 9] const vector<Datetime>& theDatetimeArray(int, *name) const;
// [ 9] const vector<Date>& theDateArray(int, *name) const;
// [ 9] const vector<Time>& theTimeArray(int, *name) const;
// [ 9] const bdem_List& theList(int, *name) const;
// [ 9] const bdem_Table& theTable(int, *name) const;
// [ 9] const bdem_Bool& theBool(int, *name) const;
// [ 9] const bdem_DatetimeTz& theDatetimeTz(int, *name) const;
// [ 9] const bdem_DateTz& theDateTz(int, *name) const;
// [ 9] const bdem_TimeTz& theTimeTz(int, *name) const;
// [ 9] const bdem_BoolArray& theBoolArray(int, *name) const;
// [ 9] const bdem_DatetimeTzArray& theDatetimeTzArray(int, *name) const;
// [ 9] const bdem_DateTzArray& theDateTzArray(int, *name) const;
// [ 9] const bdem_TimeTzArray& theTimeTzArray(int, *name) const;
// [ 9] const bdem_Choice& theChoice(int, *name) const;
// [ 9] const bdem_ChoiceArray& theChoiceArray(int, *name) const;
// [23] bdem_ConstRowBinding rowBinding(int) const;
// [23] bdem_ConstRowBinding rowBinding(int, *name) const;
// [23] bdem_ConstRowBinding rowBinding(int, int) const;
// [23] bdem_ConstTableBinding tableBinding(int, *name) const;
// [23] bdem_ConstTableBinding tableBinding(int, int) const;
// [23] bdem_ConstColumnBinding columnBinding(*name) const;
// [23] bdem_ConstColumnBinding columnBinding(int) const;
// [23] bdem_ConstChoiceBinding choiceBinding(int, *name) const;
// [23] bdem_ConstChoiceBinding choiceBinding(int, int) const;
// [23] bdem_ConstChoiceArrayBinding choiceArrayBinding(int, *name) const;
// [23] bdem_ConstChoiceArrayBinding choiceArrayBinding(int, int) const;
//
// FREE OPERATORS
// [18] bool operator==(const ConstTableBinding& lhs, const& rhs);
// [18] bool operator!=(const ConstTableBinding& lhs, const& rhs);
//
//                         =======================
//                         class bdem_TableBinding
//                         =======================
// CREATORS
// [ 4] bdem_TableBinding(Table *, *record);
// [ 4] bdem_TableBinding(Table *, *schema, *name);
// [ 4] bdem_TableBinding(Table *, *schemaint);
// [ 4] bdem_TableBinding(const& original);
// [ 4] ~bdem_TableBinding();
//
// MANIPULATORS
// [13] void rebind(const bdem_TableBinding& binding);
// [28] void rebind(Table *);
// [13] void rebind(Table *, *record);
// [13] void rebind(Table *, *schema, *name);
// [13] void rebind(Table *, *schema, int);
//
// ACCESSORS
// [ 4] bdem_Row& operator[](int) const;
// [ 9] bdem_ElemRef element(int, *name) const;
// [ 9] bdem_ElemRef element(int, int) const;
// [ 4] bdem_Table& table() const;
// [ 9] char& theChar(int, *name) const;
// [ 9] short& theShort(int, *name) const;
// [ 9] int& theInt(int, *name) const;
// [ 9] Int64& theInt64(int, *name) const;
// [ 9] float& theFloat(int, *name) const;
// [ 9] double& theDouble(int, *name) const;
// [ 9] string& theString(int, *name) const;
// [ 9] bdet_Datetime& theDatetime(int, *name) const;
// [ 9] bdet_Date& theDate(int, *name) const;
// [ 9] bdet_Time& theTime(int, *name) const;
// [ 9] vector<char>& theCharArray(int, *name) const;
// [ 9] vector<short>& theShortArray(int, *name) const;
// [ 9] vector<int>& theIntArray(int, *name) const;
// [ 9] vector<Int64>& theInt64Array(int, *name) const;
// [ 9] vector<float>& theFloatArray(int, *name) const;
// [ 9] vector<double>& theDoubleArray(int, *name) const;
// [ 9] vector<string>& theStringArray(int, *name) const;
// [ 9] vector<Datetime>& theDatetimeArray(int, *name) const;
// [ 9] vector<Date>& theDateArray(int, *name) const;
// [ 9] vector<Time>& theTimeArray(int, *name) const;
// [ 9] bdem_Bool& theBool(int, *name) const;
// [ 9] bdem_DatetimeTz& theDatetimeTz(int, *name) const;
// [ 9] bdem_DateTz& theDateTz(int, *name) const;
// [ 9] bdem_TimeTz& theTimeTz(int, *name) const;
// [ 9] bdem_BoolArray& theBoolArray(int, *name) const;
// [ 9] bdem_DatetimeTzArray& theDatetimeTzArray(int, *name) const;
// [ 9] bdem_DateTzArray& theDateTzArray(int, *name) const;
// [ 9] bdem_TimeTzArray& theTimeTzArray(int, *name) const;
// [ 9] bdem_Choice& theChoice(int, *name) const;
// [ 9] bdem_ChoiceArray& theChoiceArray(int, *name) const;
// [ 9] bdem_List& theList(int, *name) const;
// [ 9] bdem_Table& theTable(int, *name) const;
// [23] bdem_RowBinding rowBinding(int) const;
// [23] bdem_RowBinding rowBinding(int, *name) const;
// [23] bdem_RowBinding rowBinding(int, int) const;
// [23] bdem_TableBinding tableBinding(int, *name) const;
// [23] bdem_TableBinding tableBinding(int, int) const;
// [23] bdem_ColumnBinding columnBinding(*name) const;
// [23] bdem_ColumnBinding columnBinding(int) const;
// [23] bdem_ChoiceBinding choiceBinding(int, *name) const;
// [23] bdem_ChoiceBinding choiceBinding(int, int) const;
// [23] bdem_ChoiceArrayBinding choiceArrayBinding(int, *name) const;
// [23] bdem_ChoiceArrayBinding choiceArrayBinding(int, int) const;
//
//                       =============================
//                       class bdem_ConstColumnBinding
//                       =============================
// CREATORS
// [ 5] bdem_ConstColumnBinding(const Table *, int, *field);
// [ 5] bdem_ConstColumnBinding(const& original);
// [ 5] ~bdem_ConstColumnBinding();
//
// MANIPULATORS
// [14] void rebind(const bdem_ConstColumnBinding& binding);
// [14] void rebind(const Table *, int, *field);
//
// ACCESSORS
// [ 5] bdem_ConstElemRef operator[](int) const;
// [ 5] bdem_ConstElemRef element(int) const;
// [ 5] bdem_ElemType::Type elemType() const;
// [ 5] int numRows() const;
// [ 5] int columnIndex() const;
// [ 5] const bdem_FieldDef& field() const;
// [24] bdem_ConstRowBinding rowBinding(int) const;
// [ 5] const bdem_Table& table() const;
// [24] bdem_ConstTableBinding tableBinding(int) const;
// [24] bdem_ConstTableBinding tableBinding(int) const;
// [24] bdem_ConstChoiceBinding choiceBinding(int) const;
// [24] bdem_ConstChoiceArrayBinding choiceArrayBinding(int) const;
//
// FREE OPERATORS
// [19] bool operator==(const ConstColumnBinding& lhs, const& rhs);
// [19] bool operator!=(const ConstColumnBinding& lhs, const& rhs);
//
//                         ========================
//                         class bdem_ColumnBinding
//                         ========================
// CREATORS
// [ 5] bdem_ColumnBinding(Table *, int, *field);
// [ 5] bdem_ColumnBinding(const& original);
// [ 5] ~bdem_ColumnBinding();
//
// MANIPULATORS
// [14] void rebind(const bdem_ColumnBinding& binding);
// [14] void rebind(Table *, int, *field);
//
// ACCESSORS
// [ 5] bdem_ElemRef operator[](int) const;
// [ 5] bdem_ElemRef element(int) const;
// [24] bdem_RowBinding rowBinding(int) const;
// [ 5] bdem_Table& table() const;
// [24] bdem_TableBinding tableBinding(int) const;
// [24] bdem_ChoiceBinding choiceBinding(int) const;
// [24] bdem_ChoiceArrayBinding choiceArrayBinding(int) const;
//
//                         ========================
//                         class bdem_ChoiceBinding
//                         ========================
// CREATORS
// [ 6] bdem_ChoiceBinding(ChoiceArrayItem *, *record);
// [ 6] bdem_ChoiceBinding(ChoiceArrayItem *, *schema, *name);
// [ 6] bdem_ChoiceBinding(ChoiceArrayItem *, *schemaint);
// [ 6] bdem_ChoiceBinding(Choice *, *record);
// [ 6] bdem_ChoiceBinding(Choice *, *schema, *name);
// [ 6] bdem_ChoiceBinding(Choice *, *schemaint);
// [ 6] bdem_ChoiceBinding(ChoiceArray *, int, *record);
// [ 6] bdem_ChoiceBinding(ChoiceArray *, int, *schema, *name);
// [ 6] bdem_ChoiceBinding(ChoiceArray *, int, *schemaint);
// [ 6] bdem_ChoiceBinding(const ChoiceBinding& original);
// [ 6] ~bdem_ChoiceBinding();
//
// MANIPULATORS
// [15] void rebind(const bdem_ChoiceBinding& binding);
// [29] void rebind(ChoiceBinding *);
// [29] void rebind(Choice *);
// [29] void rebind(ChoiceArray *,int);
// [15] void rebind(ChoiceArrayItem *, *record);
// [15] void rebind(ChoiceArrayItem *, *schema, *name);
// [15] void rebind(ChoiceArrayItem *, *schemaint);
// [15] void rebind(Choice *, *record);
// [15] void rebind(Choice *, *schema, *name);
// [15] void rebind(Choice *, *schemaint);
// [15] void rebind(ChoiceArray *, int, *record);
// [15] void rebind(ChoiceArray *, int, *schema, *name);
// [15] void rebind(ChoiceArray *, int, *schemaint);
// [31] bdem_ElemRef makeSelection(int)
// [31] bdem_ElemRef makeSelection(const char *);
//
// ACCESSORS
// [10] bdem_ElemRef selection() const;
// [ 6] bdem_ChoiceArrayItem& item() const;
// [ 6] bdem_RecordDef &record() const;
// [10] char& theChar() const;
// [10] short& theShort() const;
// [10] int& theInt() const;
// [10] Int64& theInt64() const;
// [10] float& theFloat() const;
// [10] double& theDouble() const;
// [10] string& theString() const;
// [10] bdet_Datetime& theDatetime() const;
// [10] bdet_Date& theDate() const;
// [10] bdet_Time& theTime() const;
// [10] vector<char>& theCharArray() const;
// [10] vector<short>& theShortArray() const;
// [10] vector<int>& theIntArray() const;
// [10] vector<Int64>& theInt64Array() const;
// [10] vector<float>& theFloatArray() const;
// [10] vector<double>& theDoubleArray() const;
// [10] vector<string>& theStringArray() const;
// [10] vector<Datetime>& theDatetimeArray() const;
// [10] vector<Date>& theDateArray() const;
// [10] vector<Time>& theTimeArray() const;
// [10] bdem_List& theList() const;
// [10] bdem_Table& theTable() const;
// [10] bdem_Bool& theBool() const;
// [10] bdem_DatetimeTz& theDatetimeTz() const;
// [10] bdem_DateTz& theDateTz() const;
// [10] bdem_TimeTz& theTimeTz() const;
// [10] bdem_BoolArray& theBoolArray() const;
// [10] bdem_DatetimeTzArray& theDatetimeTzArray() const;
// [10] bdem_DateTzArray& theDateTzArray() const;
// [10] bdem_TimeTzArray& theTimeTzArray() const;
// [10] bdem_Choice& theChoice() const;
// [10] bdem_ChoiceArray& theChoiceArray() const;
// [25] bdem_RowBinding rowBinding() const;
// [25] bdem_TableBinding tableBinding() const;
// [25] bdem_TableBinding choiceBinding() const;
// [25] bdem_TableBinding choiceArrayBinding() const;
//
//
//                       =============================
//                       class bdem_ConstChoiceBinding
//                       =============================
// CREATORS
// [ 6] bdem_ConstChoiceBinding(const ChoiceArrayItem *, *record);
// [ 6] bdem_ConstChoiceBinding(const ChoiceArrayItem *, *schema, *name);
// [ 6] bdem_ConstChoiceBinding(const ChoiceArrayItem *, *schemaint);
// [ 6] bdem_ConstChoiceBinding(const Choice *, *record);
// [ 6] bdem_ConstChoiceBinding(const Choice *, *schema, *name);
// [ 6] bdem_ConstChoiceBinding(const Choice *, *schemaint);
// [ 6] bdem_ConstChoiceBinding(const ChoiceArray *, int, *record);
// [ 6] bdem_ConstChoiceBinding(const ChoiceArray *, int, *schema, *name);
// [ 6] bdem_ConstChoiceBinding(const ChoiceArray *, int, *schemaint);
// [ 6] bdem_ConstChoiceBinding(const ChoiceBinding& original);
// [ 6] ~bdem_ConstChoiceBinding();
//
// MANIPULATORS
// [15] void rebind(const bdem_ChoiceBinding& binding);
// [29] void rebind(const ChoiceBinding *);
// [29] void rebind(const Choice *);
// [29] void rebind(const ChoiceArray *);
// [15] void rebind(const ChoiceArrayItem *, *record);
// [15] void rebind(const ChoiceArrayItem *, *schema, *name);
// [15] void rebind(const ChoiceArrayItem *, *schemaint);
// [15] void rebind(const Choice *, *record);
// [15] void rebind(const Choice *, *schema, *name);
// [15] void rebind(const Choice *, *schemaint);
// [15] void rebind(const ChoiceArray *, int, *record);
// [15] void rebind(const ChoiceArray *, int, *schema, *name);
// [15] void rebind(const ChoiceArray *, int, *schemaint);
//
// ACCESSORS
// [10] const bdem_ElemRef selection() const;
// [ 6] const bdem_ChoiceArrayItem& item() const;
// [ 6] const bdem_RecordDef &record() const;
// [10] const char& theChar() const;
// [10] const short& theShort() const;
// [10] const int& theInt() const;
// [10] const Int64& theInt64() const;
// [10] const float& theFloat() const;
// [10] const double& theDouble() const;
// [10] const string& theString() const;
// [10] const bdet_Datetime& theDatetime() const;
// [10] const bdet_Date& theDate() const;
// [10] const bdet_Time& theTime() const;
// [10] const vector<char>& theCharArray() const;
// [10] const vector<short>& theShortArray() const;
// [10] const vector<int>& theIntArray() const;
// [10] const vector<Int64>& theInt64Array() const;
// [10] const vector<float>& theFloatArray() const;
// [10] const vector<double>& theDoubleArray() const;
// [10] const vector<string>& theStringArray() const;
// [10] const vector<Datetime>& theDatetimeArray() const;
// [10] const vector<Date>& theDateArray() const;
// [10] const vector<Time>& theTimeArray() const;
// [10] const bdem_List& theList() const;
// [10] const bdem_Table& theTable() const;
// [10] const bdem_Bool& theBool() const;
// [10] const bdem_DatetimeTz& theDatetimeTz() const;
// [10] const bdem_DateTz& theDateTz() const;
// [10] const bdem_TimeTz& theTimeTz() const;
// [10] const bdem_BoolArray& theBoolArray() const;
// [10] const bdem_DatetimeTzArray& theDatetimeTzArray() const;
// [10] const bdem_DateTzArray& theDateTzArray() const;
// [10] const bdem_TimeTzArray& theTimeTzArray() const;
// [10] const bdem_Choice& theChoice() const;
// [10] const bdem_ChoiceArray& theChoiceArray() const;
// [25] bdem_ConstRowBinding rowBinding() const;
// [25] bdem_ConstTableBinding tableBinding() const;
// [25] bdem_ConstTableBinding choiceBinding() const;
// [25] bdem_ConstTableBinding choiceArrayBinding() const;
//
// FREE OPERATORS
// [20] bool operator==(const ConstChoiceBinding& lhs, const& rhs);
// [20] bool operator!=(const ConstChoiceBinding& lhs, const& rhs);
//
//
//                       =============================
//                       class bdem_ChoiceArrayBinding
//                       =============================
// CREATORS
// [ 7] bdem_ChoiceArrayBinding(ChoiceArray *, *record);
// [ 7] bdem_ChoiceArrayBinding(ChoiceArray *, *schema, *name);
// [ 7] bdem_ChoiceArrayBinding(ChoiceArray *, *schemaint);
// [ 7] bdem_ChoiceArrayBinding(const ChoiceArrayBinding& original);
// [ 7] ~bdem_ChoiceArrayBinding();
//
// MANIPULATORS
// [16] void rebind(const bdem_ChoiceArrayBinding& binding);
// [16] void rebind(ChoiceArray *, *record);
// [16] void rebind(ChoiceArray *, *schema, *name);
// [16] void rebind(ChoiceArray *, *schemaint);
// [32] bdem_ElemRef makeSelection(int,int)
// [32] bdem_ElemRef makeSelection(int,char *)
//
// ACCESSORS
// [11] bdem_ElemRef selection(int) const;
// [ 7] bdem_ChoiceArrayItem& operator[](int) const;
// [ 7] bdem_ChoiceArray& choiceArray() const;
// [ 7] bdem_RecordDef &record() const;
// [11] char& theChar(int) const;
// [11] short& theShort(int) const;
// [11] int& theInt(int) const;
// [11] Int64& theInt64(int) const;
// [11] float& theFloat(int) const;
// [11] double& theDouble(int) const;
// [11] string& theString(int) const;
// [11] bdet_Datetime& theDatetime(int) const;
// [11] bdet_Date& theDate(int) const;
// [11] bdet_Time& theTime(int) const;
// [11] vector<char>& theCharArray(int) const;
// [11] vector<short>& theShortArray(int) const;
// [11] vector<int>& theIntArray(int) const;
// [11] vector<Int64>& theInt64Array(int) const;
// [11] vector<float>& theFloatArray(int) const;
// [11] vector<double>& theDoubleArray(int) const;
// [11] vector<string>& theStringArray(int) const;
// [11] vector<Datetime>& theDatetimeArray(int) const;
// [11] vector<Date>& theDateArray(int) const;
// [11] vector<Time>& theTimeArray(int) const;
// [11] bdem_List& theList(int) const;
// [11] bdem_Table& theTable(int) const;
// [11] bdem_Bool& theBool(int) const;
// [11] bdem_DatetimeTz& theDatetimeTz(int) const;
// [11] bdem_DateTz& theDateTz(int) const;
// [11] bdem_TimeTz& theTimeTz(int) const;
// [11] bdem_BoolArray& theBoolArray(int) const;
// [11] bdem_DatetimeTzArray& theDatetimeTzArray(int) const;
// [11] bdem_DateTzArray& theDateTzArray(int) const;
// [11] bdem_TimeTzArray& theTimeTzArray(int) const;
// [11] bdem_Choice& theChoice(int) const;
// [11] bdem_ChoiceArray& theChoiceArray(int) const;
// [26] bdem_RowBinding rowBinding(int) const;
// [26] bdem_TableBinding tableBinding(int) const;
// [26] bdem_TableBinding choiceBinding(int) const;
// [26] bdem_TableBinding choiceArrayBinding(int) const;
//
//                     ==================================
//                     class bdem_ConstChoiceArrayBinding
//                     ==================================
// CREATORS
// [ 7] bdem_ConstChoiceArrayBinding(const ChoiceArray *, *record);
// [ 7] bdem_ConstChoiceArrayBinding(const ChoiceArray *, *schema, *name);
// [ 7] bdem_ConstChoiceArrayBinding(const ChoiceArray *, *schemaint);
// [ 7] bdem_ConstChoiceArrayBinding(const ChoiceArrayBinding& original);
// [ 7] ~bdem_ConstChoiceArrayBinding();
//
// MANIPULATORS
// [16] void rebind(const bdem_ChoiceArrayBinding& binding);
// [30] void rebind(const ChoiceArray *);
// [16] void rebind(const ChoiceArray *, *record);
// [16] void rebind(const ChoiceArray *, *schema, *name);
// [16] void rebind(const ChoiceArray *, *schemaint);
//
// ACCESSORS
// [11] const bdem_ElemRef selection(int) const;
// [ 7] const bdem_ChoiceArrayItem& operator[](int) const;
// [ 7] const bdem_ChoiceArray& choiceArray() const;
// [ 7] const bdem_RecordDef &record() const;
// [11] const char& theChar(int) const;
// [11] const short& theShort(int) const;
// [11] const int& theInt(int) const;
// [11] const Int64& theInt64(int) const;
// [11] const float& theFloat(int) const;
// [11] const double& theDouble(int) const;
// [11] const string& theString(int) const;
// [11] const bdet_Datetime& theDatetime(int) const;
// [11] const bdet_Date& theDate(int) const;
// [11] const bdet_Time& theTime(int) const;
// [11] const vector<char>& theCharArray(int) const;
// [11] const vector<short>& theShortArray(int) const;
// [11] const vector<int>& theIntArray(int) const;
// [11] const vector<Int64>& theInt64Array(int) const;
// [11] const vector<float>& theFloatArray(int) const;
// [11] const vector<double>& theDoubleArray(int) const;
// [11] const vector<string>& theStringArray(int) const;
// [11] const vector<Datetime>& theDatetimeArray(int) const;
// [11] const vector<Date>& theDateArray(int) const;
// [11] const vector<Time>& theTimeArray(int) const;
// [11] const bdem_List& theList(int) const;
// [11] const bdem_Table& theTable(int) const;
// [11] const bdem_Bool& theBool(int) const;
// [11] const bdem_DatetimeTz& theDatetimeTz(int) const;
// [11] const bdem_DateTz& theDateTz(int) const;
// [11] const bdem_TimeTz& theTimeTz(int) const;
// [11] const bdem_BoolArray& theBoolArray(int) const;
// [11] const bdem_DatetimeTzArray& theDatetimeTzArray(int) const;
// [11] const bdem_DateTzArray& theDateTzArray(int) const;
// [11] const bdem_TimeTzArray& theTimeTzArray(int) const;
// [11] const bdem_Choice& theChoice(int) const;
// [11] const bdem_ChoiceArray& theChoiceArray(int) const;
// [26] bdem_ConstRowBinding rowBinding(int) const;
// [26] bdem_ConstTableBinding tableBinding(int) const;
// [26] bdem_ConstTableBinding choiceBinding(int) const;
// [26] bdem_ConstTableBinding choiceArrayBinding(int) const;
//
// FREE OPERATORS
// [21] bool operator==(const ConstChoiceArrayBinding& lhs, const& rhs);
// [21] bool operator!=(const ConstChoiceArrayBinding& lhs, const& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] createSchema, createList, createTable helper functions
// [33] USAGE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
                    << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_  cout << "\t";                   // Print a tab  (w/o newline)
#define T2_ cout << "\t\t";                 // Print 2 tabs (w/o newline)
#define N_  cout << "\n" << flush;          // Print a newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// In the following:
//  - "C"     abbreviates  "Const"
//  - "Bind"  abbreviates  "Binding"

typedef bdem_ConstRowBinding             CRowBind;
typedef bdem_RowBinding                   RowBind;
typedef bdem_ConstTableBinding           CTblBind;
typedef bdem_TableBinding                 TblBind;
typedef bdem_ConstColumnBinding          CColBind;
typedef bdem_ColumnBinding                ColBind;

typedef bdem_ChoiceBinding                  CBind;
typedef bdem_ConstChoiceBinding            CCBind;
typedef bdem_ChoiceArrayBinding          CArrBind;
typedef bdem_ConstChoiceArrayBinding    CCArrBind;

typedef bdem_ConstElemRef                CElemRef;
typedef bdem_ElemRef                      ElemRef;

typedef bdem_ElemType                     ElemType;
typedef bdem_List                         List;
typedef bdem_Row                          Row;
typedef bdem_Table                        Table;
typedef bdem_Choice                       Choice;
typedef bdem_ChoiceArray                  ChoiceArray;

typedef bdem_FieldDef                     FldDef;
typedef bdem_RecordDef                    RecDef;
typedef bdem_Schema                       Schema;

typedef bdet_Time                         Time;
typedef bdet_Date                         Date;
typedef bdet_Datetime                     Datetime;
typedef bdet_DatetimeTz                   DatetimeTz;
typedef bdet_DateTz                       DateTz;
typedef bdet_TimeTz                       TimeTz;

typedef bsls_Types::Int64                 Int64;

const int NUM_TYPES = ElemType::BDEM_NUM_TYPES;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static
const char *recName(const bdem_RecordDef& record)
    // Return the name of the specified 'record'.  Note that 'record' does not
    // know its name.  Rather, 'record.schema()' accesses its parent schema,
    // which in turn has a 'recordName' method.  This helper just implements
    // less-convenient 'bdem_schema' functionality.
{
    return record.schema().recordName(record.recordIndex());
}

static
int strCmp(const char* lhs, const char* rhs)
    // Return 0 if the specified 'lhs' and 'rhs' null-terminated strings are
    // equal, a negative value if lhs < rhs, and a positive value if lhs > rhs.
    // Note that the behavior is well-defined for null-pointer arguments.
{
    if (0 == lhs && 0 == rhs) { return 0; }
    if (0 == lhs) { return -1; }
    if (0 == rhs) { return 1; }
    return strcmp(lhs, rhs);
}

                        // ----------------------
                        // TEST CALLBACK FUNCTION
                        // ----------------------

// TBD: Windows: compiler limit : too many exception
//               handler states in function 'main'.  simplify function
#if defined(BDE_BUILD_TARGET_EXC) && !defined(BSLS_PLATFORM__CMP_MSVC)
static int globalTestErrorHandlerFlag;

static void testErrorHandler(void)
{
    ++globalTestErrorHandlerFlag;
    throw bsl::exception();
}

static void testErrorHandlerOther(void)
{
    --globalTestErrorHandlerFlag;
    throw bsl::exception();
}

#define EHRESET globalTestErrorHandlerFlag = 0;
#define EHFLAG  globalTestErrorHandlerFlag
#endif

// The following code creates a 'bdem_Schema' object given a specification.
// It was copied verbatim from bdem_schema.t.cpp.

//=============================================================================
//                 GENERATOR LANGUAGE FOR 'createSchema' FUNCTION
//=============================================================================
//
// The 'createSchema' function interprets a given 'spec' in order from left to
// right to build up a schema according to a custom language.  Lowercase
// letters '[a .. j]' specify arbitrary (but unique) names to be used for
// records or fields.  Records and fields may be unnamed; this is specified by
// using the '*' character for the name.  Field types are indicated with
// uppercase letters '[A .. V]' for unconstrained fields, '+' for a constrained
// list, and '#' for a constrained table.
//
// 'spec' consists of 0 or more record definitions.  (An empty spec will result
// in a valid empty schema.)  Records are specified by a leading ':' and name
// followed by 0 or more field definitions.  A field definition
// is either an unconstrained type specifier followed by a name, or a
// constrained field specifier followed by a name, and then a constraint
// specification.  A constraint specification is either the name of a
// (previously defined) record within this schema, or the 0-based index of a
// record within this schema, less than or equal to the 0-based index of the
// current record.  As may or may not be obvious from this description,
// encountering a name specification character where a field type would have
// been expected indicates the start of a new record.  A '~' indicates that
// 'removeAll' is performed on the target schema.  The '~' is legal anywhere
// that a record definition could begin.  Spaces, tabs, and newlines are
// allowed in the specification, but are ignored; they are purely cosmetic.
//
// Annotated EBNF grammar:
//  (For those unfamiliar with Extended-BNF, parentheses indicate grouping,
//  and "*" means "0 or more of the preceding".  Although this grammar does
//  not use it, EBNF also defines "+" as meaning "1 or more of the preceding"
//  and "?" as "0 or 1 of the preceding".  As in BNF, "|" separates
//  alternatives.)
//
//  Note that items following %% comment markers are not part of the grammar,
//  but are comments about semantics.
//
//  Spaces, tabs, and newlines are ignored by the parser, and are not specified
//  in this grammar although they are legal anywhere in the spec.
//
//  Spec      := ('~' | RecordDef)*
//
//  RecordDef := ':' Name ['?'] (FieldDef)*
//              %% Optional '?' indicates CHOICE record.
//
//  FieldDef  := (UnconstrainedRecordType Name)
//             | (ConstrainedRecordType Name ConstraintRef)
//
//  Name      :=
//            '*'                   %% No name, the 0 char *
//           |'a'|'b'|'c'|'d'       %% "a".."d"
//           |'e'                   %% ""  <- An empty string, not "e"
//           |'f'|'g'|'h'|'i'|'j'   %% "f".."j"
//           |'k'|'l'|'m'|'n'|'o'   %% "k".."o"
//           |'p'|'q'|'r'|'s'|'t'   %% "p".."t"
//           |'u'|'v'|'w'|'x'       %% "u".."x"
//
//  UnconstrainedRecordType :=
//            'A'|'B'|'C'|'D'|'E'   %% CHAR,SHORT,INT,INT64,FLOAT,
//           |'F'|'G'|'H'|'I'|'J'   %% DOUBLE,STRING,DATETIME,DATE,TIME
//           |'K'|'L'|'M'|'N'|'O'   %% (CHAR|SHORT|INT|INT64|FLOAT)_ARRAY
//           |'P'|'Q'|'R'|'S'|'T'   %% (DOUBLE|STRING|DATETIME|DATE|TIME)_ARRAY
//           |'U'|'V'               %% LIST, TABLE
//           |'W'|'X'|'Y'|'Z'       %% BOOL, DATETIMETZ, DATETZ, TIMETZ
//           |'a'|'b'|'c'|'d'       %% (BOOL|DATETIMETZ|DATETZ|TIMETZ)_ARRAY
//           |'e'|'f'               %% CHOICE, CHOICE_ARRAY
//
//  ConstrainedRecordType :=
//            '+'|'#'|'%'|'@'       %% LIST, TABLE, CHOICE, CHOICE_ARRAY
//
//  ConstraintRef := Name | Index
//
//  %% If a constraint is referenced by index, the index is the 0-based number
//  %% of the constraint record in the schema.  Since we're limiting this to
//  %% one digit, we can't constrain by index on any record past the 10th one.
//  %% We also allow constraining by name, so this isn't a big problem.
//  Index     :=
//         '0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'
//
// 'createSchema' syntax usage examples:
//  Here are some examples, and the schema which would result when applied to
//  an empty schema.  Since our 'createSchema' language defines '~' for
//  'removeAll', all 'spec' strings should begin with '~' to insure expected
//  behavior.
//
// Spec String      Result schema          Description
// -----------      -------------          -----------
// ""               (empty)                An empty string is a legal spec,
//                                         but has no affect.
//
// " \t\n  "        (empty)                Whitespace characters are ignored;
//                                         this is equivalent to "".
//
// ":a"              SCHEMA {               A schema containing a single empty
//                      RECORD "a" {       record named "a".
//                      }
//                  }
//
// ":a~"             SCHEMA {               An empty schema, since ~ performs
//                  }                      a removeAll().
//
// ":j"              SCHEMA {               A schema containing a single empty
//                      RECORD "j" {       record named "j".
//                      }
//                  }
//
// ":*"              SCHEMA {               A schema containing a single empty
//                      RECORD {           unnamed record.
//                      }
//                  }
//
// ":a:b"             SCHEMA {               A schema containing two empty
//                      RECORD "a" {       records named "a" and "b".
//                      }
//                      RECORD "b" {
//                      }
//                  }
//
// ":aFc"            SCHEMA {              A schema containing one record named
//                      RECORD "a" {       "a" with one DOUBLE field named "c".
//                          DOUBLE "c";
//                      }
//                  }
//
// ":aCa"            SCHEMA {              A schema containing one record named
//                      RECORD "a" {       "a" with one INT field named "a".
//                          INT "a";       Note that fields and records have
//                      }                  separate namespaces.
//                  }
//
// ":aG*"            SCHEMA {              A schema containing one record named
//                      RECORD "a" {       "a" with one STRING field which is
//                          STRING;        unnamed.
//                      }
//                  }
//
// ":aGbHc"          SCHEMA {              A schema containing one record named
//                      RECORD "a" {       "a" with two fields, one a STRING
//                          STRING "b";    named "b" and the other a DATETIME
//                          DATETIME "c";  named "c".
//                      }
//                  }
//
// ":aGb:cHc"         SCHEMA {               A schema containing two records.
//                      RECORD "a" {       Record "a" has one STRING field
//                          STRING "b";    named "b".
//                      }
//                      RECORD "c" {       Record "c" has one DATETIME field
//                          DATETIME "c";  named "c".
//                      }
//                  }
//
// ":aGb :cHc"        SCHEMA {               Exactly the same as the previous
//                      RECORD "a" {       example "aGbcHc", but more readable
//                          STRING "b";    with a space inserted between the
//                      }                  record definitions.
//                      RECORD "c" {
//                          DATETIME "c";
//                      }
//                  }
//
// ":*C*F* :*G*H* :*:*" SCHEMA {            Any number of records or fields can
//                      RECORD {           be unnamed.
//                          INT;
//                          DOUBLE;
//                      }
//                      RECORD {
//                          STRING;
//                          DATETIME;
//                      }
//                      RECORD {           Empty records can be defined,
//                      }                  although this is of limited
//                      RECORD {           practical use.
//                      }
//                  }
//
// ":aFc :d+ea"       SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained LIST
//                          LIST<"a"> "";  field named "" constrained on
//                      }                  record "a".
//                  }
//
// ":aFc :d#ea"       SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained TABLE
//                          TABLE<"a"> ""; field named "" constrained on record
//                      }                  "a".
//                  }
//
// ":aFc :d#e0"       SCHEMA {               A schema containing 2 records.
//                      RECORD "a" {       Record "a" has one DOUBLE field
//                          DOUBLE "c";    named "c".
//                      }
//                      RECORD "d" {       Record "d" has one constrained TABLE
//                          TABLE<0> "";   field named "" constrained on record
//                      }                  "a", referring to "a" by it's index,
//                  }                      0.
//
// ":a?F*G*     SCHEMA {         A schema containing 1 Choice Record.
//           RECORD:CHOICE "a" { Record "a" is a CHOICE between an
//              DOUBLE;      unnamed DOUBLE and unnamed STRING.
//              STRING;
//           }
//           }
//:
// ":a?F*G* :b@ca   SCHEMA {         A schema containing a Choice Record,
//           RECORD:CHOICE "a" { "a", which is a CHOICE between an
//              DOUBLE;      unnamed DOUBLE and unnamed STRING.
//              STRING;      It also contains a Record "c"
//           }           which is a CHOICE_ARRAY
//                                           constrained on record "a"
//           RECORD "c" {
//              CHOICE_ARRAY<"a"> "d";
//          }
//           }
//
// ":a~:a~:a~:a~"       SCHEMA {           Equivalent to "" except for
//                                         white-box
//                  }                      implications.
//
//=============================================================================

static const ElemType::Type csElemTypes[] = {
    ElemType::BDEM_CHAR,            //  0
    ElemType::BDEM_SHORT,           //  1
    ElemType::BDEM_INT,             //  2
    ElemType::BDEM_INT64,           //  3
    ElemType::BDEM_FLOAT,           //  4
    ElemType::BDEM_DOUBLE,          //  5
    ElemType::BDEM_STRING,          //  6
    ElemType::BDEM_DATETIME,        //  7
    ElemType::BDEM_DATE,            //  8
    ElemType::BDEM_TIME,            //  9
    ElemType::BDEM_CHAR_ARRAY,      // 10
    ElemType::BDEM_SHORT_ARRAY,     // 11
    ElemType::BDEM_INT_ARRAY,       // 12
    ElemType::BDEM_INT64_ARRAY,     // 13
    ElemType::BDEM_FLOAT_ARRAY,     // 14
    ElemType::BDEM_DOUBLE_ARRAY,    // 15
    ElemType::BDEM_STRING_ARRAY,    // 16
    ElemType::BDEM_DATETIME_ARRAY,  // 17
    ElemType::BDEM_DATE_ARRAY,      // 18
    ElemType::BDEM_TIME_ARRAY,      // 19
    ElemType::BDEM_LIST,            // 20
    ElemType::BDEM_TABLE,           // 21
    ElemType::BDEM_BOOL,            // 22
    ElemType::BDEM_DATETIMETZ,      // 23
    ElemType::BDEM_DATETZ,          // 24
    ElemType::BDEM_TIMETZ,          // 25
    ElemType::BDEM_BOOL_ARRAY,      // 26
    ElemType::BDEM_DATETIMETZ_ARRAY,// 27
    ElemType::BDEM_DATETZ_ARRAY,    // 28
    ElemType::BDEM_TIMETZ_ARRAY,    // 29
    ElemType::BDEM_CHOICE,          // 30
    ElemType::BDEM_CHOICE_ARRAY,    // 31
    ElemType::BDEM_LIST,            // 32 (constrained, but type is 'LIST')
    ElemType::BDEM_TABLE,           // 33 (constrained, but type is 'TABLE')
    ElemType::BDEM_CHOICE,          // 34 (constrained, but type is 'CHOICE')
    ElemType::BDEM_CHOICE_ARRAY     // 35 (constrained, but type is
                                    //     'CHOICE_ARRAY')
};
const int NUM_CS_TYPES = sizeof csElemTypes / sizeof *csElemTypes;

//=============================================================================
//                'bdem_Schema' HELPER DATA AND FUNCTIONS
//=============================================================================

// NOTE: 'index' is used in string.h on AIX so switched to 'indexStr'.
static const char removeTilde[]    = "~";
static const char name[]           = "*abcdefghijklmnopqrstuvwxyzABCDEF";
static const char indexStr[]       = "0123456789";
static const char bdemType[]       = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
static const char constrBdemType[] = "+#%@";  // Note: includes terminal '\0'

const int NUM_CS_NAMES     = sizeof(name) / sizeof(char) - 1;
const int NUM_CONSTR_TYPES = sizeof(constrBdemType) / sizeof(char) - 1;

static
bool isName(char token)
{
    return '\0' != token && 0 != strchr(name, token);
}

static
bool isConstrainedFieldType(char token)
{
    return '\0' != token && 0 != strchr(constrBdemType, token);
}

static
bool isUnconstrainedFieldType(char token)
{
    return '\0' != token && 0 != strchr(bdemType, token);
}

static
bool isFieldType(char token)
{
    return isUnconstrainedFieldType(token) || isConstrainedFieldType(token);
}

static
const char *getName(char nameCode)
    // Return the name corresponding to the specified 'nameCode'.
    // '*'      => 0     (a null name)
    // 'a'-'d'  => "a"-"d"
    // 'e'      => ""    (an empty name)
    // 'f'-'x'  => "f"-"x"
{
    switch (nameCode) {
      case '*': return   0;  // Note that name code '*' is the null string.
      case 'a': return "a";
      case 'b': return "b";
      case 'c': return "c";
      case 'd': return "d";
      case 'e': return  "";  // Note that name code 'e' is the empty string.
      case 'f': return "f";
      case 'g': return "g";
      case 'h': return "h";
      case 'i': return "i";
      case 'j': return "j";
      case 'k': return "k";
      case 'l': return "l";
      case 'm': return "m";
      case 'n': return "n";
      case 'o': return "o";
      case 'p': return "p";
      case 'q': return "q";
      case 'r': return "r";
      case 's': return "s";
      case 't': return "t";
      case 'u': return "u";
      case 'v': return "v";
      case 'w': return "w";
      case 'x': return "x";
      case 'y': return "y";
      case 'z': return "z";
      case 'A': return "A";
      case 'B': return "B";
      case 'C': return "C";
      case 'D': return "D";
      case 'E': return "E";
      case 'F': return "F";
    }
    ASSERT("Invalid name used in createSchema script" && 0);
    cout << "INVALID NAME CHARACTER USED (" << (int)nameCode << ") '"
     << nameCode << "'" << endl;
    return 0;
}

static
ElemType::Type getType(char typeCode)
    // Return the element type corresponding to the specified 'typeCode'.
    // 'A' => CHAR           'K' => CHAR_ARRAY
    // 'B' => SHORT          'L' => SHORT_ARRAY
    // 'C' => INT            'M' => INT_ARRAY
    // 'D' => INT64          'N' => INT64_ARRAY
    // 'E' => FLOAT          'O' => FLOAT_ARRAY
    // 'F' => DOUBLE         'P' => DOUBLE_ARRAY
    // 'G' => STRING         'Q' => STRING_ARRAY
    // 'H' => DATETIME       'R' => DATETIME_ARRAY
    // 'I' => DATE           'S' => DATE_ARRAY
    // 'J' => TIME           'T' => TIME_ARRAY
    //
    // 'U' => LIST           'V' => TABLE       (used for unconstrained fields)
    //
    // 'W' => BOOL         'a' => BOOL_ARRAY
    // 'X' => DATETIMETZ   'b' => DATETIMETZ_ARRAY
    // 'Y' => DATETZ       'c' => DATETZ_ARRAY
    // 'Z' => TIMETZ       'd' => TIMETZ_ARRAY
    //
    // 'e' => CHOICE       'f' => CHOICE_ARRAY  (used for unconstrained fields)
    //
    // '+' => LIST         '#' => TABLE         (used for constrained fields)
    // '%' => CHOICE       '@' => CHOICE_ARRAY  (used for constrained fields)
{
    switch (typeCode) {
      case 'A': return ElemType::BDEM_CHAR;
      case 'B': return ElemType::BDEM_SHORT;
      case 'C': return ElemType::BDEM_INT;
      case 'D': return ElemType::BDEM_INT64;
      case 'E': return ElemType::BDEM_FLOAT;
      case 'F': return ElemType::BDEM_DOUBLE;
      case 'G': return ElemType::BDEM_STRING;
      case 'H': return ElemType::BDEM_DATETIME;
      case 'I': return ElemType::BDEM_DATE;
      case 'J': return ElemType::BDEM_TIME;
      case 'K': return ElemType::BDEM_CHAR_ARRAY;
      case 'L': return ElemType::BDEM_SHORT_ARRAY;
      case 'M': return ElemType::BDEM_INT_ARRAY;
      case 'N': return ElemType::BDEM_INT64_ARRAY;
      case 'O': return ElemType::BDEM_FLOAT_ARRAY;
      case 'P': return ElemType::BDEM_DOUBLE_ARRAY;
      case 'Q': return ElemType::BDEM_STRING_ARRAY;
      case 'R': return ElemType::BDEM_DATETIME_ARRAY;
      case 'S': return ElemType::BDEM_DATE_ARRAY;
      case 'T': return ElemType::BDEM_TIME_ARRAY;
      case 'U': return ElemType::BDEM_LIST;
      case 'V': return ElemType::BDEM_TABLE;
      case 'W': return ElemType::BDEM_BOOL;
      case 'X': return ElemType::BDEM_DATETIMETZ;
      case 'Y': return ElemType::BDEM_DATETZ;
      case 'Z': return ElemType::BDEM_TIMETZ;
      case 'a': return ElemType::BDEM_BOOL_ARRAY;
      case 'b': return ElemType::BDEM_DATETIMETZ_ARRAY;
      case 'c': return ElemType::BDEM_DATETZ_ARRAY;
      case 'd': return ElemType::BDEM_TIMETZ_ARRAY;
      case 'e': return ElemType::BDEM_CHOICE;
      case 'f': return ElemType::BDEM_CHOICE_ARRAY;

      case '+': return ElemType::BDEM_LIST;
      case '#': return ElemType::BDEM_TABLE;
      case '%': return ElemType::BDEM_CHOICE;
      case '@': return ElemType::BDEM_CHOICE_ARRAY;
    }
    ASSERT("Invalid element type used in createSchema script" && 0);
    cout << "INVALID TYPE CHARACTER USED (" << (int)typeCode << ") '"
     << typeCode << "'" << endl;
    return ElemType::BDEM_INT;
}

static
const bdem_RecordDef *getConstraint(bdem_Schema *object, char token)
{
    if (strchr(indexStr, token)) {
        return &object->record(token - '0');      // constrained by index
    }
    return object->lookupRecord(getName(token));  // else constrained by name
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bdem_Schema& createSchema(bdem_Schema *object, const char *spec)
    // Configure the specified schema 'object' according to the specified
    // 'spec' using the primary manipulators and accessors of 'bdem_Schema'
    // and return a reference to 'object.  Note that normal usage is that
    // 'object' is an empty schema, but the manipulators will be applied to
    // 'object' in whatever state it is in.
{
    enum { MAX_SPEC_LEN = 20000 };
    ASSERT(strlen(spec) < MAX_SPEC_LEN);

    char cleanSpec[MAX_SPEC_LEN];  // spec after removing all white spaces

    int i = 0;
    for (int j = 0; spec[j]; ++j) {
        if (!isspace(spec[j])) {
            cleanSpec[i++] = spec[j];
        }
    }
    cleanSpec[i] = '\0';

    const char *p = cleanSpec;
    while (*p) {
        if ('~' == *p) {                    // removeAll
            object->removeAll();
            ++p;
        }
        else {                              // create a record
            ASSERT(':' == *p);              // create a record
            ++p;
            const char N = *p++;            // record name index
            ASSERT(isName(N));

            RecDef::RecordType recType;
            if ('?' == *p) {
                recType = RecDef::BDEM_CHOICE_RECORD;
                ++p;
            }
            else {
                recType = RecDef::BDEM_SEQUENCE_RECORD;
            }
            RecDef *record = object->createRecord(getName(N), recType);
            ASSERT(record);
            int fieldId = 0;
            while (isFieldType(*p)) {       // add a field
                if (isUnconstrainedFieldType(*p)) {
                    ElemType::Type fieldType = getType(*p++);
                    ASSERT(isName(*p));
                    const char *fieldName = getName(*p++);
                    const bdem_FieldDef *fld =
                        record->appendField(fieldType, fieldName, fieldId);
                    ASSERT(fld);
                }
                else if (isConstrainedFieldType(*p)) {
                    ElemType::Type fieldType = getType(*p++);
                    ASSERT(isName(*p));
                    const char *fieldName = getName(*p++);
                    const bdem_RecordDef *constr = getConstraint(object, *p++);
                    ASSERT(constr);

                    ASSERT(ElemType::isAggregateType(fieldType));

                    const FldDef *fld =record->appendField(
                                        fieldType, constr, fieldName, fieldId);
                    ASSERT(fld);
                }
                ++fieldId;
            }
        }
    }
    return *object;
}

//=============================================================================
//            One Last Helper That Uses a 'createSchema' Helper
//-----------------------------------------------------------------------------

static
const char *csName(int index)
    // Return the 'createSchema' test name corresponding to the specified
    // 'index' if index < NUM_CS_NAMES, and 0 otherwise.
{
    return index < NUM_CS_NAMES ? getName(name[index]) : 0;
}

//---------------------------------------------------
// Data arrays of standard and vocabulary data types
//---------------------------------------------------

static const char        charData[]   = {  'a',  'b', 'c', 'd',  'e'    };
static const short       shortData[]  = {   15,   25, 100, 999, 1999    };
static const int         intData[]    = {   -1, -345,  -8,  59,  856    };
static const float       floatData[]  = { -10.32, 98.7, 0               };
static const double      doubleData[] = { 99.32345, 83234.3434, 1.987   };
static const bsl::string stringData[] = { "abcdef", "zxqwert", "qwertu" };
static const Int64       int64Data[]  = { 1, 2, 3, 4, 5                 };
static const bool        boolData[]   = { true, false, true };  // 3 is the min
                                                                // # of entries

static const Datetime datetimeData[] = {
    Datetime(1999, 9, 19, 10, 20, 30, 40),
    Datetime(2000, 1, 1, 23, 59, 59, 999),
    Datetime()
};

static const Date dateData[] = {
    Date(1999, 9, 19),
    Date(2000, 1, 1),
    Date()
};

static const Time timeData[] = {
    Time(10, 20, 30, 40),
    Time(23, 59, 59, 999),
    Time()
};

static const DateTz dateTzData[] = {
    DateTz(Date(1999,9,19),312),
    DateTz(Date(1999,9,19),-52),
    DateTz(Date(),0),
};

static const TimeTz timeTzData[] = {
    TimeTz(Time(10,20,30,40),20),
    TimeTz(Time(10,20,30,40),-5),
    TimeTz(Time(23,59,59,999), 500)
};

static const DatetimeTz datetimeTzData[] = {
     DatetimeTz(Datetime(1999, 9, 19, 10, 20, 30, 40),150),
     DatetimeTz(Datetime(1999, 9, 19, 10, 20, 30, 40),-20),
     DatetimeTz(Datetime(),0)
};

// ------------------------------------------------------------------------
// Functions and data to fill vectors of standard and vocabulary data types
// ------------------------------------------------------------------------

const int numCharVec = sizeof(charData) / sizeof(*charData);
static bsl::vector<bsl::vector<char> > charVecData(numCharVec);

static void createCharVec() {
    for (int i = 0; i < numCharVec; ++i) {
        bsl::vector<char> vec(1, charData[i]);
        charVecData[i] = vec;
    }
}

const int numShortVec = sizeof(shortData) / sizeof(*shortData);
static bsl::vector<bsl::vector<short> > shortVecData(numShortVec);

static void createShortVec() {
    for (int i = 0; i < numShortVec; ++i) {
        bsl::vector<short> vec(1, shortData[i]);
        shortVecData[i] = vec;
    }
}

const int numIntVec = sizeof(intData) / sizeof(*intData);
static bsl::vector<bsl::vector<int> > intVecData(numIntVec);

static void createIntVec() {
    for (int i = 0; i < numIntVec; ++i) {
        bsl::vector<int> vec(1, intData[i]);
        intVecData[i] = vec;
    }
}

const int numFloatVec = sizeof(floatData) / sizeof(*floatData);
static bsl::vector<bsl::vector<float> > floatVecData(numFloatVec);

static void createFloatVec() {
    for (int i = 0; i < numFloatVec; ++i) {
        bsl::vector<float> vec(1, floatData[i]);
        floatVecData[i] = vec;
    }
}

const int numDoubleVec = sizeof(doubleData) / sizeof(*doubleData);
static bsl::vector<bsl::vector<double> > doubleVecData(numDoubleVec);

static void createDoubleVec() {
    for (int i = 0; i < numDoubleVec; ++i) {
        bsl::vector<double> vec(1, doubleData[i]);
        doubleVecData[i] = vec;
    }
}

const int numStringVec = sizeof(stringData) / sizeof(*stringData);
static bsl::vector<bsl::vector<bsl::string> > stringVecData(numStringVec);

static void createStringVec() {
    for (int i = 0; i < numStringVec; ++i) {
        bsl::vector<bsl::string> vec(1, stringData[i]);
        stringVecData[i] = vec;
    }
}

const int numInt64Vec = sizeof(int64Data) / sizeof(*int64Data);
static bsl::vector<bsl::vector<Int64> > int64VecData(numInt64Vec);

static void createInt64Vec() {
    for (int i = 0; i < numInt64Vec; ++i) {
        bsl::vector<Int64> vec(1, int64Data[i]);
        int64VecData[i] = vec;
    }
}

const int numDateVec = sizeof(dateData) / sizeof(*dateData);
static bsl::vector<bsl::vector<Date> > dateVecData(numDateVec);

static void createDateVec() {
    for (int i = 0; i < numDateVec; ++i) {
        bsl::vector<Date> vec(1, dateData[i]);
        dateVecData[i] = vec;
    }
}

const int numTimeVec = sizeof(timeData) / sizeof(*timeData);
static bsl::vector<bsl::vector<Time> > timeVecData(numTimeVec);

static void createTimeVec() {
    for (int i = 0; i < numTimeVec; ++i) {
        bsl::vector<Time> vec(1, timeData[i]);
        timeVecData[i] = vec;
    }
}

const int numDatetimeVec = sizeof(datetimeData) / sizeof(*datetimeData);
static bsl::vector<bsl::vector<Datetime> > datetimeVecData(numDatetimeVec);

static void createDatetimeVec() {
    for (int i = 0; i < numDatetimeVec; ++i) {
        bsl::vector<Datetime> vec(1, datetimeData[i]);
        datetimeVecData[i] = vec;
    }
}

const int numBoolVec = sizeof(boolData) / sizeof(*boolData);
static bsl::vector<bsl::vector<bool> > boolVecData(numBoolVec);

static void createBoolVec() {
    for (int i = 0; i < numBoolVec; ++i) {
        bsl::vector<bool> vec(1, boolData[i]);
        boolVecData[i] = vec;
    }
}

const int numDatetimeTzVec = sizeof(datetimeTzData) / sizeof(*datetimeTzData);
static bsl::vector<bsl::vector<DatetimeTz> >
                          datetimeTzVecData(numDatetimeTzVec);

static void createDatetimeTzVec() {
    for (int i = 0; i < numDatetimeTzVec; ++i) {
        bsl::vector<DatetimeTz> vec(1, datetimeTzData[i]);
        datetimeTzVecData[i] = vec;
    }
}

const int numDateTzVec = sizeof(dateTzData) / sizeof(*dateTzData);
static bsl::vector<bsl::vector<DateTz> > dateTzVecData(numDateTzVec);

static void createDateTzVec() {
    for (int i = 0; i < numDateTzVec; ++i) {
        bsl::vector<DateTz> vec(1, dateTzData[i]);
        dateTzVecData[i] = vec;
    }
}

const int numTimeTzVec = sizeof(timeTzData) / sizeof(*timeTzData);
static bsl::vector<bsl::vector<TimeTz> > timeTzVecData(numTimeTzVec);

static void createTimeTzVec() {
    for (int i = 0; i < numTimeTzVec; ++i) {
        bsl::vector<TimeTz> vec(1, timeTzData[i]);
        timeTzVecData[i] = vec;
    }
}

// ------------------------------------------------------------------------
// Function to create a vector of 'bdem_List' and 'bdem_Table'
// ------------------------------------------------------------------------

const int numList = 3;
static bsl::vector<List> listData(numList);

static void createListVec()
{
    for (int i = 0; i < numList; ++i) {
        List list;
        list.appendChar(charData[i]);
        list.appendShort(shortData[i]);
        list.appendInt(intData[i]);
        list.appendInt64(int64Data[i]);
        list.appendFloat(floatData[i]);
        list.appendDouble(doubleData[i]);
        list.appendString(stringData[i]);
        list.appendDate(dateData[i]);
        list.appendTime(timeData[i]);
        list.appendDatetime(datetimeData[i]);
        list.appendCharArray(charVecData[i]);
        list.appendShortArray(shortVecData[i]);
        list.appendIntArray(intVecData[i]);
        list.appendInt64Array(int64VecData[i]);
        list.appendFloatArray(floatVecData[i]);
        list.appendDoubleArray(doubleVecData[i]);
        list.appendStringArray(stringVecData[i]);
        list.appendDateArray(dateVecData[i]);
        list.appendTimeArray(timeVecData[i]);
        list.appendDatetimeArray(datetimeVecData[i]);

        listData.push_back(list);
    }
}

const int numTable = 3;
static bsl::vector<Table> tableData(numTable);

static void createTableVec()
{
    // create first table
    const bdem_ElemType::Type colTypes1[] = {
        bdem_ElemType::BDEM_DOUBLE,
        bdem_ElemType::BDEM_INT
    };

    const int numCols1 = sizeof(colTypes1) / sizeof(*colTypes1);

    Table table1(colTypes1, numCols1);

    List list1; list1.appendDouble(23.56);
                list1.appendInt(-10);

    List list2; list2.appendDouble(9.87);
                list2.appendInt(0);

    table1.appendRow(list1);
    table1.appendRow(list2);

    // create second table
    const bdem_ElemType::Type colTypes2[] = {
        bdem_ElemType::BDEM_STRING,
        bdem_ElemType::BDEM_CHAR
    };

    const int numCols2 = sizeof(colTypes2) / sizeof(*colTypes2);

    Table table2(colTypes2, numCols2);

    List list3;  list3.appendString("Somebody");
                 list3.appendChar('Z');

    List list4;  list4.appendString("anybody");
                 list4.appendChar('x');

    table2.appendRow(list3);
    table2.appendRow(list4);

    // create third table
    const bdem_ElemType::Type colTypes3[] = {
        bdem_ElemType::BDEM_DATE,
        bdem_ElemType::BDEM_INT
    };

    const int numCols3 = sizeof(colTypes3) / sizeof(*colTypes3);
    Table table3(colTypes3, numCols3);

    List list5;  list5.appendDate(Date(1, 10, 1));
                 list5.appendInt(100);

    List list6;  list6.appendDate(Date(5, 7, 9));
                 list6.appendInt(-99);

    table3.appendRow(list5);
    table3.appendRow(list6);
}

// -------------------------------------------
// Function to initialize all the data vectors
// -------------------------------------------

static void initVectors()
{
    createCharVec();
    createShortVec();
    createIntVec();
    createFloatVec();
    createDoubleVec();
    createStringVec();
    createInt64Vec();
    createDateVec();
    createTimeVec();
    createDatetimeVec();
    createBoolVec();
    createDateTzVec();
    createTimeTzVec();
    createDatetimeTzVec();
    createListVec();
    createTableVec();
}

//=============================================================================
//       HELPER FUNCTIONS THAT FACILITATE CREATION OF A LIST OR TABLE
//=============================================================================

// The minimum size of any of the above defined data arrays.
const int MIN_ENTRIES = 3;

void addElementToList(bdem_List *list, ElemType::Type type)
    // Append a data element of the specified 'type' to the specified 'list'.
{
    int randNum = rand() % MIN_ENTRIES;

    switch (type) {
      case ElemType::BDEM_CHAR: {
        list->appendChar(charData[randNum]);
      } break;
      case ElemType::BDEM_SHORT: {
        list->appendShort(shortData[randNum]);
      } break;
      case ElemType::BDEM_INT: {
        list->appendInt(intData[randNum]);
      } break;
      case ElemType::BDEM_INT64: {
        list->appendInt64(int64Data[randNum]);
      } break;
      case ElemType::BDEM_FLOAT: {
        list->appendFloat(floatData[randNum]);
      } break;
      case ElemType::BDEM_DOUBLE: {
        list->appendDouble(doubleData[randNum]);
      } break;
      case ElemType::BDEM_STRING: {
        list->appendString(stringData[randNum]);
      } break;
      case ElemType::BDEM_DATETIME: {
        list->appendDatetime(datetimeData[randNum]);
      } break;
      case ElemType::BDEM_DATE: {
        list->appendDate(dateData[randNum]);
      } break;
      case ElemType::BDEM_TIME: {
        list->appendTime(timeData[randNum]);
      } break;
      case ElemType::BDEM_CHAR_ARRAY: {
        list->appendCharArray(charVecData[randNum]);
      } break;
      case ElemType::BDEM_SHORT_ARRAY: {
        list->appendShortArray(shortVecData[randNum]);
      } break;
      case ElemType::BDEM_INT_ARRAY: {
        list->appendIntArray(intVecData[randNum]);
      } break;
      case ElemType::BDEM_INT64_ARRAY: {
        list->appendInt64Array(int64VecData[randNum]);
      } break;
      case ElemType::BDEM_FLOAT_ARRAY: {
        list->appendFloatArray(floatVecData[randNum]);
      } break;
      case ElemType::BDEM_DOUBLE_ARRAY: {
        list->appendDoubleArray(doubleVecData[randNum]);
      } break;
      case ElemType::BDEM_STRING_ARRAY: {
        list->appendStringArray(stringVecData[randNum]);
      } break;
      case ElemType::BDEM_TIME_ARRAY: {
        list->appendTimeArray(timeVecData[randNum]);
      } break;
      case ElemType::BDEM_DATE_ARRAY: {
        list->appendDateArray(dateVecData[randNum]);
      } break;
      case ElemType::BDEM_DATETIME_ARRAY: {
        list->appendDatetimeArray(datetimeVecData[randNum]);
      } break;
      case ElemType::BDEM_BOOL: {
        list->appendBool(boolData[randNum]);
      } break;
      case ElemType::BDEM_DATETIMETZ: {
        list->appendDatetimeTz(datetimeTzData[randNum]);
      } break;
      case ElemType::BDEM_DATETZ: {
        list->appendDateTz(dateTzData[randNum]);
      } break;
      case ElemType::BDEM_TIMETZ: {
        list->appendTimeTz(timeTzData[randNum]);
      } break;
      case ElemType::BDEM_BOOL_ARRAY: {
        list->appendBoolArray(boolVecData[randNum]);
      } break;
      case ElemType::BDEM_DATETIMETZ_ARRAY: {
        list->appendDatetimeTzArray(datetimeTzVecData[randNum]);
      } break;
      case ElemType::BDEM_DATETZ_ARRAY: {
        list->appendDateTzArray(dateTzVecData[randNum]);
      } break;
      case ElemType::BDEM_TIMETZ_ARRAY: {
        list->appendTimeTzArray(timeTzVecData[randNum]);
      } break;
      default: {
        cout << "unidentified element type: " << type << endl;
        ASSERT(0);
      } break;
    }
}

void createTable(bdem_Table *table, const bdem_RecordDef *record);
void createChoice(bdem_Choice *choice, const bdem_RecordDef *constraint);
void createChoiceArray(bdem_ChoiceArray *choiceArray,
                       const bdem_RecordDef *constraint);

void createList(bdem_List *list, const bdem_RecordDef *record)
    // Populate the specified 'list' with data that satisfies the specified
    // 'record'.  Recursively walks the record creating sub containers &
    // values.  Works with createTable, createChoice, createChoiceArray
{
    if (!record) {  // 'record' will be 0 in case of UNCONSTRAINED LIST
        return;
    }
    for (int i = 0; i < record->numFields(); ++i) {
        ElemType::Type type = record->field(i).elemType();

        if (ElemType::BDEM_LIST == type) {
            List listElement;
            createList(&listElement, record->field(i).recordConstraint());
            list->appendList(listElement);
        }
        else if (ElemType::BDEM_TABLE == type) {
            Table table;
            createTable(&table, record->field(i).recordConstraint());
            list->appendTable(table);
        }
        else if (ElemType::BDEM_CHOICE== type) {
            Choice choice;
            createChoice(&choice,record->field(i).recordConstraint());
            list->appendChoice(choice);
        }
        else if (ElemType::BDEM_CHOICE_ARRAY==type) {
            ChoiceArray choiceArray;
            createChoiceArray(&choiceArray,
                              record->field(i).recordConstraint());
            list->appendChoiceArray(choiceArray);
        }
        else {
            addElementToList(list, type);
        }
    }
}

void createTable(bdem_Table *table, const bdem_RecordDef *constraint)
    // Populate the specified 'table' with rows that satisfy the specified
    // 'record'.Recursively walks the record creating values and sub-containers
    // works with createList, createCHoice,createChoiceArray
{
    if (!constraint) {  // UNCONSTRAINED TABLE
        return;
    }

    bsl::vector<bdem_ElemType::Type> existingTypes;
    table->columnTypes(&existingTypes);
    // create the catalog of possible choice types
    bsl::vector<bdem_ElemType::Type> typesCatalog;
    for (int i=0;i<constraint->numFields();++i)
    {
        typesCatalog.push_back(constraint->field(i).elemType());
    }
    if (existingTypes != typesCatalog)
    {
      // the new types catalog is not the same
      // as the old one, we need to reset the table to the new catalog.
      // Otherwise do nothing and just append rows to the existing table
      table->reset(typesCatalog);
    }

    const int numRows = 2;

    for (int i = 0; i < numRows; ++i) {
        bdem_List list;
        createList(&list, constraint);
        table->appendRow(list);
    }

}

void appendElementToList(bdem_List *list, ElemType::Type type,
              const bdem_RecordDef *constraint)
// creates a value of the type and adds it to the list.  For aggregate types
// it creates an empty aggregate
{

   // re-use code to create an object of the specified type
    switch (type)   {
      case ElemType::BDEM_LIST: {
        bdem_List nlist;
        createList(&nlist,constraint);
        list->appendList(nlist);
      } break;
      case ElemType::BDEM_TABLE: {
        Table table;
        createTable(&table, constraint);
        list->appendTable(table);
      } break;
      case ElemType::BDEM_CHOICE: {
        bdem_Choice choice;
        createChoice(&choice,constraint);
        list->appendChoice(choice);
      } break;
      case ElemType::BDEM_CHOICE_ARRAY: {
        bdem_ChoiceArray choiceArray;
        createChoiceArray(&choiceArray,constraint);
        list->appendChoiceArray(choiceArray);
      } break;
      default: {
        addElementToList(list,type);
      } break;
    }
}

void createChoice(bdem_Choice *choice, const bdem_RecordDef *constraint)
    // Populates the specified 'choice' with a value that satisfies the
    // specified record.  Recursively walks the record creating values &
    // subcontainers.  The value selected from the Choices catalog is chosen
    // randomly.  Works with createList, createChoiceArray, createTable
{
    if (!constraint) {
        return;

    }

    bsl::vector<bdem_ElemType::Type> existingTypes;
    choice->selectionTypes(&existingTypes);
    // create the catalog of possible choice types
    bsl::vector<bdem_ElemType::Type> typesCatalog;
    for (int i=0;i<constraint->numFields();++i)
    {
        typesCatalog.push_back(constraint->field(i).elemType());
    }
    if (existingTypes != typesCatalog)
    {
      // the new types catalog is not the same
      // as the old one, we need to reset the choice to the new catalog.
      choice->reset(typesCatalog);
    }

    // add a value to the selection
    if (typesCatalog.size() > 0) {

        int randNum = rand() % typesCatalog.size();

        ElemType::Type type = constraint->field(randNum).elemType();

        // use a utility method to add an element of the type to a list
        bdem_List list;

        appendElementToList(&list,type,
                 constraint->field(randNum).recordConstraint());
        // make the selection and add the value to the choice
        choice->makeSelection(randNum).replaceValue(list[0]);
    }
}

void createChoiceArray(bdem_ChoiceArray *choiceArray,
               const bdem_RecordDef *constraint)
    // Populates the specified 'choice array' with values that satisfies the
    // specified record.  Recursively walks the record creating values &
    // subcontainers works with createList, createChoice, createTable
{
    if (!constraint) {

        return;
    }

    bsl::vector<bdem_ElemType::Type> existingTypes;
    choiceArray->selectionTypes(&existingTypes);

    // create the catalog of possible choice types
    bsl::vector<bdem_ElemType::Type> typesCatalog;
    for (int i=0;i<constraint->numFields();++i)
    {
        typesCatalog.push_back(constraint->field(i).elemType());
    }
    if (existingTypes != typesCatalog)
    {
      // the new types catalog is not the same
      // as the old one, we need to reset the choice array to the new catalog.
      // Otherwise do nothing and just append choices to the existing array
      choiceArray->reset(typesCatalog);
    }

    // select only 4 of the choice types to keep the recursive definition
    // manageable
    for (int i=0; (i<4) && (i < (int) typesCatalog.size()); ++i)
    {
        choiceArray->appendNullItems(1);
        int randNum = rand() % typesCatalog.size();
         ElemType::Type type = constraint->field(randNum).elemType();

        // use the utility method to add an element of the type to a list
        bdem_List list;
        appendElementToList(&list,type,
                 constraint->field(randNum).recordConstraint());

        // make the selection and add the value to the choice
        (*choiceArray).theModifiableItem(i).makeSelection(randNum).
                                                         replaceValue(list[0]);
    }

}

//=============================================================================
//    Pre-defined schema test values (as spec strings for 'createSchema')
//=============================================================================

// Specs for creating instances of 'bdem_Schema', and instances of 'bdem_List'
// that satisfy those schemas.

const char *listSpecs[] = {
    ":aCaDbBcAd :f+ga",
    ":aBaFbAcDd :f+ga :h#ia",
    ":aAaBbCcDd :b?AaBb :c%ab :d@ab :e+ac :f+ac",
    ":aAaBbCcDd :b?+aa#ba :c%ab :e+ac",
    ":aAaBbCcDd :b?+aa#ba :c@ab :e+ac",
};

const int NUM_LIST_SPECS = sizeof listSpecs / sizeof *listSpecs;

// Specs for creating instances of 'bdem_Schema', and instances of 'bdem_Table'
// that satisfy those schemas.
//
// NOTE: Code that uses these specs require that the last element in the last
//       'record' is a 'bdem_Table'.
const char *tableSpecs[] = {
    ":aAaBbCcDd :f#ga",
    ":aAaBbCcDd :f+ga :h#if",
    ":aAaBbCcDd :f+ga :h#if :b#jh",
    ":aAaBbCcDd :b?AaBb :c%ab :h#ac",
    ":aAaBbCcDd :b?AaBb :c@ab :h#ac"
};

const int NUM_TABLE_SPECS = sizeof tableSpecs / sizeof *tableSpecs;

// Specs used to construct a Choices and ChoiceArrays
const char *choiceSpecs[] = {
    ":a?AaBbCc" ,
    ":aAaBbCc :b?+aaBb",
    ":aAaBbCc :b?#aaCb",
    ":a?AaBb  :b?%aaDb",
    ":a?AaBb  :b?@aaEb"
};
const int NUM_CHOICE_SPECS = sizeof choiceSpecs / sizeof *choiceSpecs;

//=============================================================================
//                          SWITCH SUPPORT MACROS
//-----------------------------------------------------------------------------

#define DECLARE_TEST_SCHEMA                                                   \
                                                                              \
    /* ----------------------------- Test schema --------------------------*/ \
                                                                              \
    Schema S1_;  const bdem_Schema& S1 = S1_;                                 \
    RecDef *pR = S1_.createRecord("A");                                       \
                                                                              \
    pR->appendField(ElemType::BDEM_CHAR,           "a");                      \
    pR->appendField(ElemType::BDEM_SHORT,          "b");                      \
    pR->appendField(ElemType::BDEM_INT,            "c");                      \
    pR->appendField(ElemType::BDEM_INT64,          "d");                      \
    pR->appendField(ElemType::BDEM_FLOAT,          "e");                      \
    pR->appendField(ElemType::BDEM_DOUBLE,         "f");                      \
    pR->appendField(ElemType::BDEM_STRING,         "g");                      \
    pR->appendField(ElemType::BDEM_DATETIME,       "h");                      \
    pR->appendField(ElemType::BDEM_DATE,           "i");                      \
    pR->appendField(ElemType::BDEM_TIME,           "j");                      \
                                                                              \
    pR->appendField(ElemType::BDEM_CHAR_ARRAY,     "k");                      \
    pR->appendField(ElemType::BDEM_SHORT_ARRAY,    "l");                      \
    pR->appendField(ElemType::BDEM_INT_ARRAY,      "m");                      \
    pR->appendField(ElemType::BDEM_INT64_ARRAY,    "n");                      \
    pR->appendField(ElemType::BDEM_FLOAT_ARRAY,    "o");                      \
    pR->appendField(ElemType::BDEM_DOUBLE_ARRAY,   "p");                      \
    pR->appendField(ElemType::BDEM_STRING_ARRAY,   "q");                      \
    pR->appendField(ElemType::BDEM_DATETIME_ARRAY, "r");                      \
    pR->appendField(ElemType::BDEM_DATE_ARRAY,     "s");                      \
    pR->appendField(ElemType::BDEM_TIME_ARRAY,     "t");                      \
                                                                              \
    pR->appendField(ElemType::BDEM_LIST,           "u");                      \
    pR->appendField(ElemType::BDEM_TABLE,          "v");                      \
                                                                              \
    pR->appendField(ElemType::BDEM_BOOL,           "w");                      \
    pR->appendField(ElemType::BDEM_DATETIMETZ,     "x");                      \
    pR->appendField(ElemType::BDEM_DATETZ,         "y");                      \
    pR->appendField(ElemType::BDEM_TIMETZ,         "z");                      \
    pR->appendField(ElemType::BDEM_BOOL_ARRAY,     "A");                      \
    pR->appendField(ElemType::BDEM_DATETIMETZ_ARRAY,"B");                     \
    pR->appendField(ElemType::BDEM_DATETZ_ARRAY,   "C");                      \
    pR->appendField(ElemType::BDEM_TIMETZ_ARRAY,   "D");                      \
    pR->appendField(ElemType::BDEM_CHOICE,         "E");                      \
    pR->appendField(ElemType::BDEM_CHOICE_ARRAY,   "F");                      \
                                                                              \
    const char *fieldNames[] = { "a", "b", "c", "d", "e", "f", "g", "h",      \
                                 "i", "j", "k", "l", "m", "n", "o", "p",      \
                                 "q", "r", "s", "t", "u", "v","w","x","y",    \
                                 "z", "A", "B", "C", "D","E", "F"             \
    };                                                                        \
                                                                              \
    ASSERT(NUM_TYPES == S1.record(0).numFields());

#define DECLARE_TEST_LIST_OBJECTS                                             \
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  \
    //                           Helper Data                                  \
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ \
                                                                              \
    const char                        XA = 'x';                               \
    const short                       XB = 12;                                \
    const int                         XC = 103;                               \
    const Int64                       XD = 10004;                             \
    const float                       XE = 105.5;                             \
    const double                      XF = 106.006;                           \
    const bsl::string                 XG = "one-seven---"; /* not in-place */ \
    const bdet_Datetime               XH(bdet_Date(108, 8, 8),                \
                                             bdet_Time(8, 8, 8, 108));        \
    const bdet_Date                   XI(109, 9, 9);                          \
    const bdet_Time                   XJ(10, 10, 10, 110);                    \
                                                                              \
    bsl::vector<char>                 XK_;                                    \
    const bsl::vector<char>&          XK = XK_;                               \
    XK_.push_back(XA);                                                        \
    XK_.push_back(XA);                                                        \
                                                                              \
    bsl::vector<short>                XL_;                                    \
    const bsl::vector<short>&         XL = XL_;                               \
    XL_.push_back(XB);                                                        \
    XL_.push_back(XB);                                                        \
                                                                              \
    bsl::vector<int>                  XM_;                                    \
    const bsl::vector<int>&           XM = XM_;                               \
    XM_.push_back(XC);                                                        \
    XM_.push_back(XC);                                                        \
                                                                              \
    bsl::vector<Int64>                XN_;                                    \
    const bsl::vector<Int64>&         XN = XN_;                               \
    XN_.push_back(XD);                                                        \
    XN_.push_back(XD);                                                        \
                                                                              \
    bsl::vector<float>                XO_;                                    \
    const bsl::vector<float>&         XO = XO_;                               \
    XO_.push_back(XE);                                                        \
    XO_.push_back(XE);                                                        \
                                                                              \
    bsl::vector<double>               XP_;                                    \
    const bsl::vector<double>&        XP = XP_;                               \
    XP_.push_back(XF);                                                        \
    XP_.push_back(XF);                                                        \
                                                                              \
    bsl::vector<bsl::string>          XQ_;                                    \
    const bsl::vector<bsl::string>&   XQ = XQ_;                               \
    XQ_.push_back(XG);                                                        \
    XQ_.push_back(XG);                                                        \
                                                                              \
    bsl::vector<bdet_Datetime>        XR_;                                    \
    const bsl::vector<bdet_Datetime>& XR = XR_;                               \
    XR_.push_back(XH);                                                        \
    XR_.push_back(XH);                                                        \
                                                                              \
    bsl::vector<bdet_Date>            XS_;                                    \
    const bsl::vector<bdet_Date>&     XS = XS_;                               \
    XS_.push_back(XI);                                                        \
    XS_.push_back(XI);                                                        \
                                                                              \
    bsl::vector<bdet_Time>            XT_;                                    \
    const bsl::vector<bdet_Time>&     XT = XT_;                               \
    XT_.push_back(XJ);                                                        \
    XT_.push_back(XJ);                                                        \
                                                                              \
    bdem_List                         XU_;                                    \
    const bdem_List&                  XU = XU_;                               \
    XU_.appendInt(XC);                                                        \
    XU_.appendString(XG);                                                     \
                                                                              \
    const bdem_ElemType::Type colTypes[] = {                                  \
        bdem_ElemType::BDEM_INT,                                              \
        bdem_ElemType::BDEM_STRING                                            \
    };                                                                        \
                                                                              \
    const int numColTypes = sizeof(colTypes) / sizeof(*colTypes);             \
                                                                              \
    bdem_Table                        XV_(colTypes, numColTypes);             \
    const bdem_Table&                 XV = XV_;                               \
    XV_.appendNullRow();                                                      \
    XV_.theModifiableRow(0).theModifiableInt(0)    = XC;                      \
    XV_.theModifiableRow(0).theModifiableString(1) = XG;                      \
    XV_.appendNullRow();                                                      \
    XV_.theModifiableRow(1).theModifiableInt(0)    = XC;                      \
    XV_.theModifiableRow(1).theModifiableString(1) = XG;                      \
                                                                              \
    const bool                        XW = true;                              \
    const DatetimeTz                  XX(Datetime(208,8,8,8,8,8,208),-10);    \
    const DateTz                      XY = DateTz(Date(204,4,4),50);          \
    const TimeTz                      XZ = TimeTz(Time(4,30,0,500),100);      \
                                                                              \
    bsl::vector<bool>                 Xa;                                     \
    const bsl::vector<bool>&          Xa_ = Xa;                               \
    Xa.push_back(XW);                                                         \
    Xa.push_back(XW);                                                         \
                                                                              \
    bsl::vector<DatetimeTz>           Xb;                                     \
    const bsl::vector<DatetimeTz>&    Xb_ = Xb;                               \
    Xb.push_back(XX);                                                         \
    Xb.push_back(XX);                                                         \
                                                                              \
    bsl::vector<DateTz>               Xc;                                     \
    const bsl::vector<DateTz>&        Xc_ = Xc;                               \
    Xc.push_back(XY);                                                         \
    Xc.push_back(XY);                                                         \
                                                                              \
    bsl::vector<TimeTz>               Xd;                                     \
    const bsl::vector<TimeTz>&        Xd_ = Xd;                               \
    Xd.push_back(XZ);                                                         \
    Xd.push_back(XZ);                                                         \
                                                                              \
    const ElemType::Type types[] = {                                          \
        ElemType::BDEM_CHAR,                                                  \
        ElemType::BDEM_INT                                                    \
    };                                                                        \
                                                                              \
    Choice                                       Xe(types,2);                 \
    const Choice                                 Xe_=Xe;                      \
    Xe.makeSelection(1).theModifiableInt() =     XC;                          \
                                                                              \
    ChoiceArray                                  Xf(types,2);                 \
    const ChoiceArray                            Xf_=Xf;                      \
    Xf.appendNullItems(2);                                                    \
    Xf.theModifiableItem(0).makeSelection(0).theModifiableChar() = XA;        \
    Xf.theModifiableItem(1).makeSelection(1).theModifiableInt()  = XC;        \
                                                                              \
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ \
                                                                              \
    const char                        YA = 'y';                               \
    const short                       YB = 22;                                \
    const int                         YC = 203;                               \
    const Int64                       YD = 20004;                             \
    const float                       YE = 205.5;                             \
    const double                      YF = 206.006;                           \
    const bsl::string                 YG = "two-seven";                       \
    const bdet_Datetime               YH(bdet_Date(208, 8, 8),                \
                                         bdet_Time(8, 8, 8, 208));            \
    const bdet_Date                   YI(209, 9, 9);                          \
    const bdet_Time                   YJ(10, 10, 10, 210);                    \
                                                                              \
    bsl::vector<char>                 YK_;                                    \
    const bsl::vector<char>&          YK = YK_;                               \
    YK_.push_back(YA);                                                        \
    YK_.push_back(YA);                                                        \
                                                                              \
    bsl::vector<short>                YL_;                                    \
    const bsl::vector<short>&         YL = YL_;                               \
    YL_.push_back(YB);                                                        \
    YL_.push_back(YB);                                                        \
                                                                              \
    bsl::vector<int>                  YM_;                                    \
    const bsl::vector<int>&           YM = YM_;                               \
    YM_.push_back(YC);                                                        \
    YM_.push_back(YC);                                                        \
                                                                              \
    bsl::vector<Int64>                YN_;                                    \
    const bsl::vector<Int64>&         YN = YN_;                               \
    YN_.push_back(YD);                                                        \
    YN_.push_back(YD);                                                        \
                                                                              \
    bsl::vector<float>                YO_;                                    \
    const bsl::vector<float>&         YO = YO_;                               \
    YO_.push_back(YE);                                                        \
    YO_.push_back(YE);                                                        \
                                                                              \
    bsl::vector<double>               YP_;                                    \
    const bsl::vector<double>&        YP = YP_;                               \
    YP_.push_back(YF);                                                        \
    YP_.push_back(YF);                                                        \
                                                                              \
    bsl::vector<bsl::string>          YQ_;                                    \
    const bsl::vector<bsl::string>&   YQ = YQ_;                               \
    YQ_.push_back(YG);                                                        \
    YQ_.push_back(YG);                                                        \
                                                                              \
    bsl::vector<bdet_Datetime>        YR_;                                    \
    const bsl::vector<bdet_Datetime>& YR = YR_;                               \
    YR_.push_back(YH);                                                        \
    YR_.push_back(YH);                                                        \
                                                                              \
    bsl::vector<bdet_Date>            YS_;                                    \
    const bsl::vector<bdet_Date>&     YS = YS_;                               \
    YS_.push_back(YI);                                                        \
    YS_.push_back(YI);                                                        \
                                                                              \
    bsl::vector<bdet_Time>            YT_;                                    \
    const bsl::vector<bdet_Time>&     YT = YT_;                               \
    YT_.push_back(YJ);                                                        \
    YT_.push_back(YJ);                                                        \
                                                                              \
    bdem_List                         YU_;                                    \
    const bdem_List&                  YU = YU_;                               \
    YU_.appendInt(YC);                                                        \
    YU_.appendString(YG);                                                     \
                                                                              \
    bdem_Table                        YV_(colTypes, numColTypes);             \
    const bdem_Table&                 YV = YV_;                               \
    YV_.appendNullRow();                                                      \
    YV_.theModifiableRow(0).theModifiableInt(0)    = YC;                      \
    YV_.theModifiableRow(0).theModifiableString(1) = YG;                      \
    YV_.appendNullRow();                                                      \
    YV_.theModifiableRow(1).theModifiableInt(0)    = YC;                      \
    YV_.theModifiableRow(1).theModifiableString(1) = YG;                      \
                                                                              \
    const bool                        YW = false;                             \
    const DatetimeTz                  YX(Datetime(108,1,10,3,3,28),10);       \
    const DateTz                      YY = DateTz(Date(201,3,3),-50);         \
    const TimeTz                      YZ = TimeTz(Time(11,0,10,50),-100);     \
                                                                              \
    bsl::vector<bool>                 Ya;                                     \
    const bsl::vector<bool>&          Ya_ = Ya;                               \
    Ya.push_back(YW);                                                         \
    Ya.push_back(YW);                                                         \
                                                                              \
    bsl::vector<DatetimeTz>           Yb;                                     \
    const bsl::vector<DatetimeTz>&    Yb_ = Yb;                               \
    Yb.push_back(YX);                                                         \
    Yb.push_back(YX);                                                         \
                                                                              \
    bsl::vector<DateTz>               Yc;                                     \
    const bsl::vector<DateTz>&        Yc_ = Yc;                               \
    Yc.push_back(YY);                                                         \
    Yc.push_back(YY);                                                         \
                                                                              \
    bsl::vector<TimeTz>               Yd;                                     \
    const bsl::vector<TimeTz>&        Yd_ = Yd;                               \
    Yd.push_back(YZ);                                                         \
    Yd.push_back(YZ);                                                         \
                                                                              \
    const ElemType::Type ytypes[] =                                           \
        { ElemType::BDEM_STRING, ElemType::BDEM_DATE };                       \
    Choice                                         Ye(ytypes,2);              \
    const Choice                                   Ye_=Ye;                    \
    Ye.makeSelection(1).theModifiableDate() =      YI;                        \
                                                                              \
    ChoiceArray                                    Yf(ytypes,2);              \
    const ChoiceArray                              Yf_=Yf;                    \
    Yf.appendNullItems(2);                                                    \
    Yf.theModifiableItem(0).makeSelection(0).theModifiableString() = YG;      \
    Yf.theModifiableItem(1).makeSelection(1).theModifiableDate()   = YI;      \
                                                                              \
                                                                              \
    /*--------------------------------------------------------------------    \
    // Convenient test list objects                                           \
    //------------------------------------------------------------------*/    \
    List LX_;                         List LY_;                               \
    const List& LX = LX_;             const List& LY = LY_;                   \
    LX_.appendChar(XA);               LY_.appendChar(YA);                     \
    LX_.appendShort(XB);              LY_.appendShort(YB);                    \
    LX_.appendInt(XC);                LY_.appendInt(YC);                      \
    LX_.appendInt64(XD);              LY_.appendInt64(YD);                    \
    LX_.appendFloat(XE);              LY_.appendFloat(YE);                    \
    LX_.appendDouble(XF);             LY_.appendDouble(YF);                   \
    LX_.appendString(XG);             LY_.appendString(YG);                   \
    LX_.appendDatetime(XH);           LY_.appendDatetime(YH);                 \
    LX_.appendDate(XI);               LY_.appendDate(YI);                     \
    LX_.appendTime(XJ);               LY_.appendTime(YJ);                     \
    LX_.appendCharArray(XK);          LY_.appendCharArray(YK);                \
    LX_.appendShortArray(XL);         LY_.appendShortArray(YL);               \
    LX_.appendIntArray(XM);           LY_.appendIntArray(YM);                 \
    LX_.appendInt64Array(XN);         LY_.appendInt64Array(YN);               \
    LX_.appendFloatArray(XO);         LY_.appendFloatArray(YO);               \
    LX_.appendDoubleArray(XP);        LY_.appendDoubleArray(YP);              \
    LX_.appendStringArray(XQ);        LY_.appendStringArray(YQ);              \
    LX_.appendDatetimeArray(XR);      LY_.appendDatetimeArray(YR);            \
    LX_.appendDateArray(XS);          LY_.appendDateArray(YS);                \
    LX_.appendTimeArray(XT);          LY_.appendTimeArray(YT);                \
    LX_.appendList(XU);               LY_.appendList(YU);                     \
    LX_.appendTable(XV);              LY_.appendTable(YV);                    \
    LX_.appendBool(XW);               LY_.appendBool(YW);                     \
    LX_.appendDatetimeTz(XX);         LY_.appendDatetimeTz(YX);               \
    LX_.appendDateTz(XY);             LY_.appendDateTz(YY);                   \
    LX_.appendTimeTz(XZ);             LY_.appendTimeTz(YZ);                   \
    LX_.appendBoolArray(Xa);          LY_.appendBoolArray(Ya);                \
    LX_.appendDatetimeTzArray(Xb);    LY_.appendDatetimeTzArray(Yb);          \
    LX_.appendDateTzArray(Xc);        LY_.appendDateTzArray(Yc);              \
    LX_.appendTimeTzArray(Xd);        LY_.appendTimeTzArray(Yd);              \
    LX_.appendChoice(Xe);             LY_.appendChoice(Ye);                   \
    LX_.appendChoiceArray(Xf);        LY_.appendChoiceArray(Yf);              \
                                                                              \
    ASSERT(NUM_TYPES == LX.length());                                         \
    ASSERT(NUM_TYPES == LY.length());                                         \
                                                                              \

#define DECLARE_TABLE_OBJECTS                                                 \
    initVectors();  /* initialize vectors used as helper data */              \
                                                                              \
    /* Declare a 'list' vector and 'schema' vector corresponding to the */    \
    /* entries in the 'list' vector.                                    */    \
                                                                              \
    bsl::vector<Schema>       listSchemaVec(NUM_LIST_SPECS);                  \
    bsl::vector<List>         listVec(NUM_LIST_SPECS);                        \
    bsl::vector<const char *> listRecordName(NUM_LIST_SPECS);                 \
                                                                              \
    /* Create vectors of 'bdem_List' and 'bdem_Schema' from 'listSpecs'. */   \
                                                                              \
    for (int i = 0; i < NUM_LIST_SPECS; ++i) {                                \
        createSchema(&listSchemaVec[i], listSpecs[i]);                        \
        const RecDef *rec =                                                   \
                 &listSchemaVec[i].record(listSchemaVec[i].numRecords() - 1); \
                                                                              \
        createList(&listVec[i], rec);                                         \
        listRecordName[i] =                                                   \
              listSchemaVec[i].recordName(listSchemaVec[i].numRecords() - 1); \
    }                                                                         \
                                                                              \
    /* Declare a 'table' vector and a 'record' vector corresponding to the    \
    // entries in the 'table' vector. */                                      \
                                                                              \
    bsl::vector<Schema>         tableSchemaVec(NUM_TABLE_SPECS);              \
    bsl::vector<const char *>   tableRecordName(NUM_TABLE_SPECS);             \
    bsl::vector<const RecDef *> tableRecVec(NUM_TABLE_SPECS);                 \
    bsl::vector<Table>          tableVec(NUM_TABLE_SPECS);                    \
                                                                              \
    /* Create vectors of 'bdem_Table' and 'bdem_Schema' from 'tableSpecs'.    \
    // NOTE: This code requires that the last 'field' in the last 'record'    \
    //       is a 'bdem_Table'. */                                            \
                                                                              \
    for (int i = 0; i < NUM_TABLE_SPECS; ++i) {                               \
        createSchema(&tableSchemaVec[i], tableSpecs[i]);                      \
                                                                              \
        const RecDef *rec =                                                   \
                &tableSchemaVec[i].record(tableSchemaVec[i].numRecords() - 1);\
        tableRecVec[i] = rec->field(rec->numFields() - 1).recordConstraint(); \
                                                                              \
        tableVec[i] = Table();                                                \
        createTable(&tableVec[i],                                             \
                    rec->field(rec->numFields() - 1).recordConstraint());     \
        tableRecordName[i] = recName(*tableRecVec[i]);                        \
    }

#define DECLARE_CHOICE_OBJECTS                                                \
    /* Declare a 'choice' vector and 'schema' vector corresponding to the     \
    // entries in the 'choice' vector. */                                     \
    bsl::vector<Schema>              choiceSchemaVec(NUM_CHOICE_SPECS);       \
    bsl::vector<Choice>              choiceVec(NUM_CHOICE_SPECS);             \
    bsl::vector<const char *>        choiceRecordName(NUM_CHOICE_SPECS);      \
                                                                              \
    /* Create vectors of 'bdem_Choice' and 'bdem_Schema' from 'listChoice'.*/ \
    for (int i = 0; i < NUM_CHOICE_SPECS; ++i) {                              \
        createSchema(&choiceSchemaVec[i], choiceSpecs[i]);                    \
        const RecDef *rec =                                                   \
             &choiceSchemaVec[i].record(choiceSchemaVec[i].numRecords() - 1); \
        createChoice(&choiceVec[i], rec);                                     \
        choiceRecordName[i] =                                                 \
          choiceSchemaVec[i].recordName(choiceSchemaVec[i].numRecords() - 1); \
    }                                                                         \
                                                                              \
    /* Declare a 'choice array' vector and 'schema' vector corresponding to   \
    // the entries in the 'choice array' vector. */                           \
    bsl::vector<Schema>       choiceArraySchemaVec(NUM_CHOICE_SPECS);         \
    bsl::vector<ChoiceArray>         choiceArrayVec(NUM_CHOICE_SPECS);        \
    bsl::vector<const char *> choiceArrayRecordName(NUM_CHOICE_SPECS);        \
                                                                              \
    /* Create vectors of 'bdem_List' and 'bdem_Schema' from 'listSpecs'. */   \
    for (int i = 0; i < NUM_CHOICE_SPECS; ++i) {                              \
        createSchema(&choiceArraySchemaVec[i], choiceSpecs[i]);               \
        const RecDef *rec =                                                   \
      &choiceArraySchemaVec[i].record(                                        \
                 choiceArraySchemaVec[i].numRecords() - 1);                   \
        createChoiceArray(&choiceArrayVec[i], rec);                           \
        choiceArrayRecordName[i] =                                            \
                    choiceArraySchemaVec[i].recordName(                       \
                 choiceArraySchemaVec[i].numRecords() - 1);                   \
    }

//=============================================================================
//                              TEST CASES
//-----------------------------------------------------------------------------
#define DEFINE_TEST_CASE(NUMBER)                                              \
void testCase##NUMBER(bool verbose, bool veryVerbose, bool veryVeryVerbose)   \

DEFINE_TEST_CASE(33) {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   This case verifies that the usage example works as advertised.
        //
        // Concerns:
        //   That the usage example compiles, links, and runs as expected.
        //
        // Plan:
        //   Replicate the usage example from the component header here.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// Given the large number of classes provided by this component, we present
// three examples illustrating basic use of several of the binding classes.
// The three examples make use of a common schema and common data, which is
// described in this initial section.  The three examples follow in separate
// sub-sections.
//
// The schema used in these examples is shown in Figure 1 below:
//..
//        +------------------------------------------+
//        |{                                         |
//        |    RECORD "EMPLOYEE" {                   |
//        |        STRING            "empName";      |
//        |        INT               "salary";       |
//        |        DATE              "dateOfHire";   |
//        |    }                                     |
//        |                                          |
//        |    RECORD "TEAM" {                       |
//        |        STRING            "teamName";     |
//        |        DOUBLE            "revenue";      |
//        |        LIST<"EMPLOYEE">  "leader";       |
//        |        TABLE<"EMPLOYEE"> "members";      |
//        |    }                                     |
//        |                                          |
//        |    RECORD "DEPARTMENT" {                 |
//        |        STRING            "deptName";     |
//        |        DOUBLE            "revenue";      |
//        |        TABLE<"TEAM">     "teams";        |
//        |    }                                     |
//        |}                                         |
//        +------------------------------------------+
//
//               Fig 1 -- "DEPARTMENT" Schema
//..
// This schema describes hypothetical departments, teams, and employees.  A
// department has a name ("deptName"), earns "revenue", and consists of one or
// more "teams".  Each team also has a name ("teamName") and earns "revenue".
// Furthermore, a team consists of one or more employees (its "members"), with
// one of these employees designated the team "leader".  Lastly, an employee
// has a name ("empName"), earns a "salary", and was hired on some date
// ("dateOfHire").  The following block of code initializes an instance of
// 'bdem_Schema' corresponding to the schema in Figure 1:
//..
    bdem_Schema deptSchema;
    {
        // EMPLOYEE record

        bdem_RecordDef *employee = deptSchema.createRecord("EMPLOYEE");

        employee->appendField(bdem_ElemType::BDEM_STRING, "empName");
        employee->appendField(bdem_ElemType::BDEM_INT,    "salary");
        employee->appendField(bdem_ElemType::BDEM_DATE,   "dateOfHire");

        // TEAM record

        bdem_RecordDef *team = deptSchema.createRecord("TEAM");

        team->appendField(bdem_ElemType::BDEM_STRING, "teamName");
        team->appendField(bdem_ElemType::BDEM_DOUBLE, "revenue");

        team->appendField(bdem_ElemType::BDEM_LIST,  employee, "leader");
        team->appendField(bdem_ElemType::BDEM_TABLE, employee, "members");

        // DEPARTMENT record

        bdem_RecordDef *department = deptSchema.createRecord("DEPARTMENT");

        department->appendField(bdem_ElemType::BDEM_STRING, "deptName");
        department->appendField(bdem_ElemType::BDEM_DOUBLE, "revenue");

        department->appendField(bdem_ElemType::BDEM_TABLE, team, "teams");
    }
//..
// When the 'deptSchema' instance is printed on 'bsl::cout':
//..
if (veryVerbose)  // REMOVE FROM USAGE
    bsl::cout << deptSchema << bsl::endl;
//..
// it displays as follows:
//..
//  {
//      RECORD "EMPLOYEE" {
//          STRING "empName";
//          INT "salary";
//          DATE "dateOfHire";
//      }
//
//      RECORD "TEAM" {
//          STRING "teamName";
//          DOUBLE "revenue";
//          LIST<"EMPLOYEE"> "leader";
//          TABLE<"EMPLOYEE"> "members";
//      }
//
//      RECORD "DEPARTMENT" {
//          STRING "deptName";
//          DOUBLE "revenue";
//          TABLE<"TEAM"> "teams";
//      }
//  }
//..
// Next we provide sample data used to populate a department consisting of two
// teams, each having four members.  For convenience, we first populate a
// 'struct'.  A more realistic example might make use of the text-parsing
// methods in 'bdempu' to go directly from "source" text to 'bdem' data:
//..
    static const struct Employee {
        bsl::string d_name;
        int         d_salary;
        bdet_Date   d_hireDate;
    } EMPLOYEES[] = {
        // name                salary (knuts x K)   hireDate
        // -----------------   ------------------   -----------------------
        {  "Alice Field",      135,                 bdet_Date(2001,  1, 27)  },
        {  "Martina Hingis",   107,                 bdet_Date(1999, 11,  9)  },
        {  "Sean O'Casey",     126,                 bdet_Date(1998,  5,  5)  },
        {  "Harry Tonto",       95,                 bdet_Date(2003,  4,  9)  },
        {  "Michael Blix",     125,                 bdet_Date(1998,  5,  5)  },
        {  "Jen-Pi Huang",     110,                 bdet_Date(2004,  2, 16)  },
        {  "John Smith",       105,                 bdet_Date(2004,  7, 17)  },
        {  "Sandra Mueller",    95,                 bdet_Date(2004,  8,  8)  },
    };
    const int numEmployees = sizeof(EMPLOYEES) / sizeof(*EMPLOYEES);

    const bdem_ElemType::Type EmpTypes[] = {
        bdem_ElemType::BDEM_STRING,
        bdem_ElemType::BDEM_INT,
        bdem_ElemType::BDEM_DATE
    };
    const int numEmpTypes = sizeof(EmpTypes) / sizeof(*EmpTypes);

    static const struct Team {
        bsl::string d_name;
        double      d_revenue;
    } TEAMS[] = {
        // name  revenue (knuts x M)
        // ----  -------------------
        {  "A",  100.50             },
        {  "B",   73.27             },
    };
    const int numTeams = sizeof(TEAMS) / sizeof(*TEAMS);  ASSERT(numTeams);

    const bdem_ElemType::Type TeamTypes[] = {
        bdem_ElemType::BDEM_STRING,
        bdem_ElemType::BDEM_DOUBLE,
        bdem_ElemType::BDEM_LIST,
        bdem_ElemType::BDEM_TABLE
    };
    const int numTeamTypes = sizeof(TeamTypes) / sizeof(*TeamTypes);

    const bdem_ElemType::Type DeptTypes[] = {
        bdem_ElemType::BDEM_STRING,
        bdem_ElemType::BDEM_DOUBLE,
        bdem_ElemType::BDEM_TABLE
    };
    const int numDeptTypes = sizeof(DeptTypes) / sizeof(*DeptTypes);
//..
// Teams and team leaders are instances of 'bdem_List'.  The team leaders for
// our two teams are 'leaderA' and 'leaderB':
//..
    bdem_List leaderA(EmpTypes, numEmpTypes);
    leaderA[0].theModifiableString() = EMPLOYEES[0].d_name;
    leaderA[1].theModifiableInt()    = EMPLOYEES[0].d_salary;
    leaderA[2].theModifiableDate()   = EMPLOYEES[0].d_hireDate;

    bdem_List leaderB(EmpTypes, numEmpTypes);
    leaderB[0].theModifiableString() = EMPLOYEES[4].d_name;
    leaderB[1].theModifiableInt()    = EMPLOYEES[4].d_salary;
    leaderB[2].theModifiableDate()   = EMPLOYEES[4].d_hireDate;
//..
// 'teamA' and 'teamB' are the teams led by 'leaderA' and 'leaderB',
// respectively:
//..
    bdem_List teamA(TeamTypes, numTeamTypes);
    teamA[0].theModifiableString() = TEAMS[0].d_name;
    teamA[1].theModifiableDouble() = TEAMS[0].d_revenue;
    teamA[2].theModifiableList()   = leaderA;
    teamA[3].theModifiableTable()  = bdem_Table(EmpTypes, numEmpTypes);
    int i;
    for (i = 0; i < 4; ++i) {
        bdem_List member(EmpTypes, numEmpTypes);
        member[0].theModifiableString() = EMPLOYEES[i].d_name;
        member[1].theModifiableInt()    = EMPLOYEES[i].d_salary;
        member[2].theModifiableDate()   = EMPLOYEES[i].d_hireDate;
        teamA[3].theModifiableTable().appendRow(member);
    }

    bdem_List teamB(TeamTypes, numTeamTypes);
    teamB[0].theModifiableString() = TEAMS[1].d_name;
    teamB[1].theModifiableDouble() = TEAMS[1].d_revenue;
    teamB[2].theModifiableList()   = leaderB;
    teamB[3].theModifiableTable()  = bdem_Table(EmpTypes, numEmpTypes);
    for (i = 4; i < numEmployees; ++i) {
        bdem_List member(EmpTypes, numEmpTypes);
        member[0].theModifiableString() = EMPLOYEES[i].d_name;
        member[1].theModifiableInt()    = EMPLOYEES[i].d_salary;
        member[2].theModifiableDate()   = EMPLOYEES[i].d_hireDate;
        teamB[3].theModifiableTable().appendRow(member);
    }
//..
// Finally, we create a 'bdem_List' comprised of 'teamA' and 'teamB' to
// represent the "Widgets" department:
//..
    bdem_List widgetsDept(DeptTypes, numDeptTypes);
    widgetsDept[0].theModifiableString() = "Widgets";  // dept name
    widgetsDept[1].theModifiableDouble() = 195.7;      // revenue (knuts x M)
    widgetsDept[2].theModifiableTable()  = bdem_Table(TeamTypes, numTeamTypes);
    widgetsDept[2].theModifiableTable().appendRow(teamA);
    widgetsDept[2].theModifiableTable().appendRow(teamB);

//..
// When printed on 'bsl::cout':
//..
if (veryVerbose) {  // REMOVE FROM USAGE
    bsl::cout << "Team A" << bsl::endl
              << "------" << bsl::endl;  teamA.print(bsl::cout, -2, 2);
    bsl::cout << "Team B" << bsl::endl
              << "------" << bsl::endl;  teamB.print(bsl::cout, -2, 2);
}                   // REMOVE FROM USAGE
//..
// the two teams display as follows:
//..
//  Team A
//  ------
//  [
//        STRING A
//        DOUBLE 100.5
//        LIST [
//          STRING Alice Field
//          INT 135
//          DATE 27JAN2001
//        ]
//        TABLE [
//          Row 0: [
//            STRING Alice Field
//            INT 135
//            DATE 27JAN2001
//          ]
//          Row 1: [
//            STRING Martina Hingis
//            INT 107
//            DATE 09NOV1999
//          ]
//          Row 2: [
//            STRING Sean O'Casey
//            INT 126
//            DATE 05MAY1998
//          ]
//          Row 3: [
//            STRING Harry Tonto
//            INT 95
//            DATE 09APR2003
//          ]
//        ]
//      ]
//  Team B
//  ------
//  [
//        STRING B
//        DOUBLE 73.27
//        LIST [
//          STRING Michael Blix
//          INT 125
//          DATE 05MAY1998
//        ]
//        TABLE [
//          Row 0: [
//            STRING Michael Blix
//            INT 125
//            DATE 05MAY1998
//          ]
//          Row 1: [
//            STRING Jen-Pi Huang
//            INT 110
//            DATE 16FEB2004
//          ]
//          Row 2: [
//            STRING John Smith
//            INT 105
//            DATE 17JUL2004
//          ]
//          Row 3: [
//            STRING Sandra Mueller
//            INT 95
//            DATE 08AUG2004
//          ]
//        ]
//      ]
//..
///Usage Example 1
///- - - - - - - -
// This first example illustrates basic use of row bindings.  First we create
// a row binding useful for accessing elements of 'teamA' using the constructor
// that takes a 'const bdem_List *', a 'const bdem_Schema *', and a
// 'const char *' (naming a record in the supplied schema):
//..
    {
        const bdem_ConstRowBinding CRB(&teamA, &deptSchema, "TEAM");
        ASSERT(teamA.length() >= CRB.length());
//..
// Using names corresponding to fields in the bound record, the name and
// revenue of Team A may be accessed as follows:
//..
        const bsl::string& teamName = CRB.theString("teamName");
        const double       revenue  = CRB.theDouble("revenue");

if (veryVerbose)  // REMOVE FROM USAGE
        bsl::cout << "Team "                           << teamName
                  << " current revenue (knuts x M) = " << revenue << bsl::endl;
//..
// The same two fields also may be accessed by their index in the record.
// The 'operator[]' method of 'bdem_ConstRowBinding' returns an instance of
// 'bdem_ConstElemRef' (an element reference):
//..
        ASSERT(teamName == CRB[0].theString());
        ASSERT(revenue  == CRB[1].theDouble());
//..
// Next we create a second row binding to access elements of the "leader" of
// Team A.  The 'rowBinding' method is used to construct this binding:
//..
        bdem_ConstRowBinding leader(CRB.rowBinding("leader"));
        const bsl::string&   leaderName = leader.theString("empName");
        const bdet_Date      hiredOn    = leader.theDate  ("dateOfHire");

if (veryVerbose)  // REMOVE FROM USAGE
        bsl::cout << "  Led by "   << leaderName
                  << " (hired on " << hiredOn << ")" << bsl::endl;
//..
// The 'CRB' binding cannot be used to modify its bound aggregate.  We create
// a second binding for that purpose.  In particular, we update the revenue of
// Team A using the 'RB' binding:
//..
        const bdem_RowBinding RB(&teamA, &deptSchema, "TEAM");
        RB.theModifiableDouble("revenue") += 12.7;
//..
// We access the revenue of Team A through the 'CRB' binding a second time to
// obtain the latest figure:
//..
        const double revisedRevenue = CRB.theDouble("revenue");

if (veryVerbose)  // REMOVE FROM USAGE
        bsl::cout << "Team "                           << teamName
                  << " revised revenue (knuts x M) = " << revisedRevenue
                  << bsl::endl;
//..
// The 'leader' binding may be rebound to another 'bdem_List' that satisfies
// the 'EMPLOYEE' record.  In the following, we rebind 'leader' to the leader
// of Team B:
//..
        leader.rebind(&leaderB);
        const bsl::string& anotherLeaderName = leader.theString("empName");

if (veryVerbose)  // REMOVE FROM USAGE
        bsl::cout << anotherLeaderName << " is another team leader."
                  << bsl::endl;
//..
// Although 'RB' binds a modifiable row, it may *not* be rebound to another row
// since it was declared 'const':
//..
        // RB.rebind(&teamB);  // compile-time error
    }
//..
// The above block of code prints the following on 'bsl::cout':
//..
//  Team A current revenue (knuts x M) = 100.5
//    Led by Alice Field (hired on 27JAN2001)
//  Team A revised revenue (knuts x M) = 113.2
//  Michael Blix is another team leader.
//..
///Usage Example 2
///- - - - - - - -
// This second example illustrates row, table, and column bindings used in
// combination to print out the members of the Widgets department.  We create
// a row binding to access the contents of the department:
//..
    {
        const bdem_ConstRowBinding dept(&widgetsDept,
                                        &deptSchema,
                                        "DEPARTMENT");
        const bsl::string& deptName = dept.theString("deptName");
if (veryVerbose)  // REMOVE FROM USAGE
        bsl::cout << deptName << " Department" << bsl::endl;
//..
// Next, we create a table binding to access the teams that belong to the
// Widgets department.  The 'tableBinding' method yields a table binding that
// is used to initialize the 'allTeams' binding:
//..
        const bdem_ConstTableBinding allTeams(dept.tableBinding("teams"));
//..
// We now iterate over the teams in the Widgets department, printing the names
// of the members of each team:
//..
        const int numTeams = allTeams.numRows();
        for (int i = 0; i < numTeams; ++i) {
//..
// The table binding's 'rowBinding' method returns a row binding for the team
// that will be processed in the current iteration:
//..
            const bdem_ConstRowBinding team(allTeams.rowBinding(i));
            const bsl::string& teamName = team.theString("teamName");
//..
// We access the name of the leader of the current team through the temporary
// row binding returned by the call to 'rowBinding("leader")':
//..
            const bsl::string& leaderName =
                                team.rowBinding("leader").theString("empName");
if (veryVerbose)  // REMOVE FROM USAGE
            bsl::cout << "  Team "   << teamName
                      << ", led by " << leaderName << ", includes:"
                      << bsl::endl;
//..
// Since we are interested in accessing only the names of team members, we
// opt to use a column binding that is bound to the column holding the names.
// The call to 'tableBinding("members")' returns a temporary table binding.
// The call to 'columnBinding("empName")' on that transient binding yields the
// column binding that is desired:
//..
            const bdem_ConstColumnBinding members(
                        team.tableBinding("members").columnBinding("empName"));
//..
// Finally, we iterate over all of the members of the current team and print
// their names.  The name of the team leader was already printed above, so it
// is treated as a special case and filtered out in the following loop.
// Similar to row bindings, the column binding's 'operator[]' method returns an
// element reference:
//..
            const int numMembers = members.numRows();
            for (int j = 0; j < numMembers; ++j) {
                const bsl::string& memberName = members[j].theString();
                if (memberName != leaderName) {
if (veryVerbose)  // REMOVE FROM USAGE
                    bsl::cout << "    " << memberName << bsl::endl;
                }
            }
        }
    }
//..
// The previous block of code prints the following on 'bsl::cout':
//..
//  Widgets Department
//    Team A, led by Alice Field, includes:
//      Martina Hingis
//      Sean O'Casey
//      Harry Tonto
//    Team B, led by Michael Blix, includes:
//      Jen-Pi Huang
//      John Smith
//      Sandra Mueller
//..
      }

DEFINE_TEST_CASE(32) {
        // --------------------------------------------------------------------
        // TESTING CHOICE ARRAY MUTATOR METHODS
        //   Test the remaining ChoiceArray Binding mutator function.
        //
        // Concerns:
        //   - The 'mutator' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The mutator methods return the proper results
        //
        // Plan:
        //   Create two instance of 'bdem_ChoiceArray' and bindings for the
        //   choice objects.  Loop through the possible values for
        //  makeSelection.  Verify that:
        //     1) After making a selection via the binding the 'bdem_Choice'
        //        has the appropriate selector
        //     2) The makeSelection method returns the correct element ref
        //
        // Testing:
        //
        //             bdem_ChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~
        //   bdem_ElemRef makeSelection(int, int)
        //   bdem_ElemRef makeSelection(int, const char *)
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Choice Mutator methods" << endl
                          << "============================" << endl;
        {

            Schema mS;  const Schema& S = mS;
            createSchema(&mS, ":a?AaBbCcDdFfGgHh");

            const RecDef *rec      = &S.record(0);
            ChoiceArray mCA1; const ChoiceArray& CA1 = mCA1;
            createChoiceArray(&mCA1, rec);
            ChoiceArray mCA2; const ChoiceArray& CA2 = mCA2;
            createChoiceArray(&mCA2, rec);

            mCA1.removeAllItems(); mCA1.appendNullItems(CA1.numSelections());
            mCA2.removeAllItems(); mCA2.appendNullItems(CA2.numSelections());
            mCA1.theModifiableItem(0).makeSelection(0);
            mCA2.theModifiableItem(0).makeSelection(0);

            CArrBind   cab1(&mCA1, rec);
            CArrBind   cab2(&mCA2, rec);

            // SANITY CHECK
            ASSERT(CA1[0].selector()>=0);
            ASSERT(CA2[0].selector()>=0);

            bdem_ElemRef rt1 = cab1.makeSelection(0,-1);
            ASSERT( CA1[0].selection() == rt1);
            ASSERT( CA1[0].selector() == -1);
            for (int i = 0; i < CA1.numSelections(); ++i) {
                // Test across the diagonal
                const char *name = rec->fieldName(i);
                bdem_ElemRef r1 = cab1.makeSelection(i,i);
                bdem_ElemRef r2 = cab2.makeSelection(i,name);

                ASSERT( CA1[i].selection() == r1);
                ASSERT( CA2[i].selection() == r2);

                ASSERT( CA1[i].selector() == i);
                ASSERT( CA2[i].selector() == i);
            }
        }
      }

DEFINE_TEST_CASE(31) {
        // --------------------------------------------------------------------
        // TESTING CHOICE MUTATOR METHODS
        //   Test the remaining Choice Binding mutator function.
        //
        // Concerns:
        //   - The 'mutator' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The mutator methods return the proper results
        //
        // Plan:
        //   Create two instance of 'bdem_Choice' and bindings for the
        //   choice objects.  Loop through the possible values for
        //  makeSelection to to the other.  Verify that:
        //     1) After making a selection via the binding the 'bdem_Choice'
        //        has the appropriate selector
        //     2) The makeSelection method returns the correct element ref
        //
        // Testing:
        //
        //             bdem_ChoiceBinding
        //             ~~~~~~~~~~~~~~~
        //   bdem_ElemRef makeSelection(int)
        //   bdem_ElemRef makeSelection(const char *)
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Choice Mutator methods" << endl
                          << "============================" << endl;
        {

            Schema mS;  const Schema& S = mS;
            createSchema(&mS, ":a?AaBbCcDdFfGgHh");

            const RecDef *rec      = &S.record(0);
            Choice mC1; const Choice& C1 = mC1; createChoice(&mC1, rec);
            Choice mC2; const Choice& C2 = mC2; createChoice(&mC2, rec);

            CBind   cb1(&mC1, rec);
            CBind   cb2(&mC2, rec);

            // SANITY CHECK
            ASSERT(C1.selector()>=0);
            ASSERT(C2.selector()>=0);

            bdem_ElemRef rt1 = cb1.makeSelection(-1);
            ASSERT( C1.selection() == rt1);
            ASSERT( C1.selector() == -1);
            for (int i = 0; i < C1.numSelections(); ++i) {
                const char *name = rec->fieldName(i);
                bdem_ElemRef r1 = cb1.makeSelection(i);
                bdem_ElemRef r2 = cb2.makeSelection(name);

                ASSERT( C1.selection() == r1);
                ASSERT( C2.selection() == r2);

                ASSERT( C1.selector() == i);
                ASSERT( C2.selector() == i);
            }
        }
      }

DEFINE_TEST_CASE(30) {
        // --------------------------------------------------------------------
        // TESTING CHOICE ARRAY BINDING REBIND
        //   Test the remaining Choice Binding 'rebind' function(s).
        //
        // Concerns:
        //   - The 'rebind' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The rebound data and meta-data are hooked up correctly.
        //
        // Plan:
        //   Create two instance of 'bdem_ChoiceArray'.
        //   Create choice array bindings with one and rebind
        //   to the other.  Verify that:
        //     1) The 'bdem_ChoiceArray' after rebinding refers to the
        //        appropriate ChoiceArray
        //     2) Ensure that the meta-data is not effected.
        //
        // Testing:
        //             bdem_ConstChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   void rebind(const ChoiceArray *);
        //
        //             bdem_ChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~
        //   void rebind(ChoiceArray *);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Choice Binding 'rebind'" << endl
                          << "============================" << endl;
        {
            Schema mS;  const Schema& S = mS;
            createSchema(&mS, ":a?AaBbCcDdFfGgHh");

            const RecDef *rec      = &S.record(0);

            ChoiceArray mCA1;  const ChoiceArray &CA1=mCA1;
            createChoiceArray(&mCA1,rec);

            ChoiceArray mCA2;  const ChoiceArray &CA2=mCA2;
            createChoiceArray(&mCA2,rec);

                   CCArrBind  ccab(&CA1, rec);
                    CArrBind   cab(&mCA1, rec);

            const  CCArrBind&  CCAB = ccab;
            const   CArrBind&   CAB = cab;

            ASSERT(&CA1 ==  &CCAB.choiceArray());
            ASSERT(&CA1 ==  &CAB.choiceArray());

            ASSERT(rec       ==  &CCAB.record());
            ASSERT(rec       ==  &CAB.record());

            ccab.rebind(&CA2);
             cab.rebind(&mCA2);

            ASSERT(&CA2 ==  &CCAB.choiceArray());
            ASSERT(&CA2 ==  &CAB.choiceArray());

            ASSERT(rec       ==  &CCAB.record());
            ASSERT(rec       ==  &CAB.record());
        }
      }

DEFINE_TEST_CASE(29) {
        // --------------------------------------------------------------------
        // TESTING CHOICE BINDING REBIND
        //   Test the remaining Choice Binding 'rebind' functions.
        //
        // Concerns:
        //   - The 'rebind' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The rebound data and meta-data are hooked up correctly.
        //
        // Plan:
        //   Create two instance of 'bdem_Choice' and two instances of
        //   'bdem_ChoiceArray' satisfying the same meta-data, but having
        //   different *data*.  Create choice bindings with one and rebind
        //   to the other.  Verify that:
        //     1) The 'bdem_Choice' after rebinding refers to the appropriate
        //        Choice in the second 'bdem_Choice' or in the
        //        'bdem_ChoiceArray'.
        //     2) Ensure that the meta-data is not effected.
        //
        // Testing:
        //             bdem_ConstChoiceBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   void rebind(const ChoiceArrayItem *);
        //   void rebind(const Choice *);
        //   void rebind(const ChoiceArray *, int);
        //
        //             bdem_ChoiceBinding
        //             ~~~~~~~~~~~~~~~
        //   void rebind(ChoiceArrayItem *);
        //   void rebind(Choice *);
        //   void rebind(ChoiceArray *, int);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Choice Binding 'rebind'" << endl
                          << "============================" << endl;
        {
            Schema mS;  const Schema& S = mS;
            createSchema(&mS, ":a?AaBbCcDdFfGgHh");

            const RecDef *rec      = &S.record(0);

            Choice mC1;  const Choice& C1 = mC1;  createChoice(&mC1, rec);
            Choice mC2;  const Choice& C2 = mC2;  createChoice(&mC2, rec);

            ChoiceArray mCA1;  const ChoiceArray &CA1=mCA1;
            createChoiceArray(&mCA1,rec);

            ChoiceArray mCA2;  const ChoiceArray &CA2=mCA2;
            createChoiceArray(&mCA2,rec);

                   CCBind  ccb1(&C1.item(), rec);
                   CCBind  ccb2(&C1, rec);
                   CCBind  ccb3(&CA1, 0, rec);

                    CBind   cb1(&mC1.item(), rec);
                    CBind   cb2(&mC1, rec);
                    CBind   cb3(&mCA1, 0, rec);

            const  CCBind&  CCB1 = ccb1;
            const  CCBind&  CCB2 = ccb2;
            const  CCBind&  CCB3 = ccb3;

            const   CBind&   CB1 = cb1;
            const   CBind&   CB2 = cb2;
            const   CBind&   CB3 = cb3;

            ASSERT(&C1.item() ==  &CCB1.item());
            ASSERT(&C1.item() ==  &CCB2.item());
            ASSERT(&CA1[0]    ==  &CCB3.item());

            ASSERT(&C1.item() ==   &CB1.item());
            ASSERT(&C1.item() ==   &CB2.item());
            ASSERT(&CA1[0]    ==   &CB3.item());

            ASSERT(rec       ==  &CCB1.record());
            ASSERT(rec       ==  &CCB2.record());
            ASSERT(rec       ==  &CCB3.record());

            ASSERT(rec       ==   &CB1.record());
            ASSERT(rec       ==   &CB2.record());
            ASSERT(rec       ==   &CB3.record());

            ccb1.rebind(&C2.item());
            ccb2.rebind(&C2);
            ccb3.rebind(&mCA2, 0);

            cb1.rebind(&mC2.item());
            cb2.rebind(&mC2.item());
            cb3.rebind(&mCA2, 0);

            ASSERT(&C2.item() ==  &CCB1.item());
            ASSERT(&C2.item() ==  &CCB2.item());
            ASSERT(&CA2[0]    ==  &CCB3.item());

            ASSERT(&C2.item() ==   &CB1.item());
            ASSERT(&C2.item() ==   &CB2.item());
            ASSERT(&CA2[0]    ==   &CB3.item());

            ASSERT(rec       ==  &CCB1.record());
            ASSERT(rec       ==  &CCB2.record());
            ASSERT(rec       ==  &CCB3.record());

            ASSERT(rec       ==   &CB1.record());
            ASSERT(rec       ==   &CB2.record());
            ASSERT(rec       ==   &CB3.record());
        }
      }

DEFINE_TEST_CASE(28) {
        // --------------------------------------------------------------------
        // TESTING TABLE BINDING REBIND
        //   Test the remaining Table Binding 'rebind' functions.
        //
        // Concerns:
        //   - The 'rebind' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The rebound data and meta-data are hooked up correctly.
        //
        // Plan:
        //   Create two instances of 'bdem_Table' having different *data*, but
        //   satisfying the same 'bdem_RecordDef' meta-data.  Create a table
        //   binding from one table and the meta-data, then rebind to the
        //   other table.  Verify that the table after rebinding refers to the
        //   second table.
        //
        // Testing:
        //             bdem_ConstTableBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~
        //   void rebind(const Table *);
        //
        //             bdem_TableBinding
        //             ~~~~~~~~~~~~~~~~~
        //   void rebind(Table *);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Table Binding 'rebind'" << endl
                          << "==============================" << endl;
        {
            Schema mS;  const Schema& S = mS;
            createSchema(&mS, ":aAaBbCcDdFfGgHh :f+ga :h#if");

            const RecDef *rec      = &S.record(2);
            const RecDef *tableRec = &S.record(1);

            List mL1;  createList(&mL1, rec);
            List mL2;  createList(&mL2, rec);

            Table& mT1 = mL1.theModifiableTable(0);  const Table& T1 = mT1;
            Table& mT2 = mL2.theModifiableTable(0);  const Table& T2 = mT2;

                   CTblBind   ctb(&T1, tableRec);
                    TblBind    tb(&mT1, tableRec);

            const  CTblBind&  CTB = ctb;
            const   TblBind&   TB =  tb;

            ASSERT(&T1 ==  &CTB.table());
            ASSERT(&T1 ==   &TB.table());

            ASSERT(tableRec ==  &CTB.record());
            ASSERT(tableRec ==   &TB.record());

             ctb.rebind(&T2);
              tb.rebind(&mT2);

            ASSERT(&T2 ==  &CTB.table());
            ASSERT(&T2 ==   &TB.table());

            ASSERT(tableRec ==  &CTB.record());
            ASSERT(tableRec ==   &TB.record());
        }
      }

DEFINE_TEST_CASE(27) {
        // --------------------------------------------------------------------
        // TESTING ROW BINDING REBIND
        //   Test the remaining Row Binding 'rebind' functions.
        //
        // Concerns:
        //   - The 'rebind' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The rebound data and meta-data are hooked up correctly.
        //
        // Plan:
        //   Create two instance of 'bdem_List' and two instances of
        //   'bdem_Table' satisfying the same meta-data, but having different
        //   *data*.  Create row bindings with one and rebind to the other.
        //   Verify that:
        //     1) The 'bdem_Row' after rebinding refers to the appropriate row
        //        in the second 'bdem_List' or 'bdem_Table'.
        //     2) Ensure that the meta-data is not effected.
        //
        // Testing:
        //             bdem_ConstRowBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   void rebind(const Row *);
        //   void rebind(const List *);
        //   void rebind(const Table *, int);
        //
        //             bdem_RowBinding
        //             ~~~~~~~~~~~~~~~
        //   void rebind(Row *);
        //   void rebind(List *);
        //   void rebind(Table *, int);
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Row Binding 'rebind'" << endl
                          << "============================" << endl;
        {
            Schema mS;  const Schema& S = mS;
            createSchema(&mS, ":aAaBbCcDdFfGgHh :f+ga :h#if");

            const RecDef *rec      = &S.record(2);
            const RecDef *tableRec = &S.record(1);

            List mL1;  const List& L1 = mL1;  createList(&mL1, rec);
            List mL2;  const List& L2 = mL2;  createList(&mL2, rec);

            Table& mT1 = mL1.theModifiableTable(0);  const Table& T1 = mT1;
            Table& mT2 = mL2.theModifiableTable(0);  const Table& T2 = mT2;

                   CRowBind  crb1(&L1.row(), rec);
                   CRowBind  crb2(&L1, rec);
                   CRowBind  crb3(&T1, 0, tableRec);

                    RowBind   rb1(&mL1.row(), rec);
                    RowBind   rb2(&mL1, rec);
                    RowBind   rb3(&mT1, 0, tableRec);

            const  CRowBind&  CRB1 = crb1;
            const  CRowBind&  CRB2 = crb2;
            const  CRowBind&  CRB3 = crb3;

            const   RowBind&   RB1 = rb1;
            const   RowBind&   RB2 = rb2;
            const   RowBind&   RB3 = rb3;

            ASSERT(&L1.row() ==  &CRB1.row());
            ASSERT(&L1.row() ==  &CRB2.row());
            ASSERT(&T1[0]    ==  &CRB3.row());

            ASSERT(&L1.row() ==   &RB1.row());
            ASSERT(&L1.row() ==   &RB2.row());
            ASSERT(&T1[0]    ==   &RB3.row());

            ASSERT(rec       ==  &CRB1.record());
            ASSERT(rec       ==  &CRB2.record());
            ASSERT(tableRec  ==  &CRB3.record());

            ASSERT(rec       ==   &RB1.record());
            ASSERT(rec       ==   &RB2.record());
            ASSERT(tableRec  ==   &RB3.record());

            crb1.rebind(&L2.row());
            crb2.rebind(&L2.row());
            crb3.rebind(&T2, 0);

            rb1.rebind(&mL2.row());
            rb2.rebind(&mL2.row());
            rb3.rebind(&mT2, 0);

            ASSERT(&L2.row() ==  &CRB1.row());
            ASSERT(&L2.row() ==  &CRB2.row());
            ASSERT(&T2[0]    ==  &CRB3.row());

            ASSERT(&L2.row() ==   &RB1.row());
            ASSERT(&L2.row() ==   &RB2.row());
            ASSERT(&T2[0]    ==   &RB3.row());

            ASSERT(rec       ==  &CRB1.record());
            ASSERT(rec       ==  &CRB2.record());
            ASSERT(tableRec  ==  &CRB3.record());

            ASSERT(rec       ==   &RB1.record());
            ASSERT(rec       ==   &RB2.record());
            ASSERT(tableRec  ==   &RB3.record());
        }
      }

DEFINE_TEST_CASE(26) {
        // ------------------------------------------------------------------
        // TESTING CHOICE ARRAY BINDING xxxBINDING
        //   Test Choice Array Binding functions that return bindings.
        //
        // Concerns:
        //   - Valid bindings are returned by the 'xxxBinding' methods.
        //   - The data and meta-data are hooked up correctly.
        //
        // Plan:
        //   - Functions returning Row Bindings
        //     Create a 'bdem_List' containing an element of an unconstrained
        //     type.  Create a 'bdem_ChoiceArray' having the
        //     'bdem_List' as an element.  Create "const" and
        //     non-"const" choice array bindings from
        //     the 'bdem_ChoiceArray' and call the 'rowBinding' methods to
        //     obtain new choice array bindings for the first 'bdem_List'.
        //     Use the direct accessors to verify that the row and record
        //     have been installed correctly in the returned bindings.
        //
        //     Perform the same test when the first 'bdem_ChoiceArray'
        //     contains an element of a constrained type.
        //
        //   - Functions returning Table Bindings
        //     Create a 'bdem_Table' containing rows of an unconstrained type.
        //     Create a 'bdem_ChoiceArray' having the 'bdem_Table' as an
        //     element.  Create "const" and non-"const" choice array bindings
        //     from the 'bdem_ChoiceArray' and call the 'tableBinding' methods
        //      o obtain new table bindings for the 'bdem_Table'.
        //     Use the direct accessors to verify that
        //     the table and record have been installed correctly in the
        //     returned bindings.
        //
        //     Perform the same test when the 'bdem_Table' contains rows of a
        //     constrained type.
        //
        //   - Functions returning Choice Bindings
        //     NOTE: Choice binding methods on ChoiceArrayBinding
        //     behave differently from other xxxBinding methods.
        //     choiceBinding(int i) methods return a choice around the array
        //     element at index i (instead of a choice _inside_ array element
        //     i)
        //     Create a 'bdem_ChoiceArray' having choice elements of
        //     unconstrained type.  Create "const" and non-"const" choice array
        //     bindings from the 'bdem_ChoiceArray' and call the
        //     'choiceBinding' methods to obtain choice bindings for the
        //     elements of the array.
        //     Construct a ChoiceBinding for same ChoiceArrayItem and use the
        //     direct accessors to verify that the choice and record have been
        //     installed correctly in the returned bindings.
        //
        //   - Functions returning Choice Array Bindings
        //     Create a 'bdem_ChoiceArray' containing rows of an
        //     unconstrained type.  Create a second 'bdem_ChoiceArray'
        //     having the first 'bdem_ChoiceArray' as an element.
        //     Create "const" and non-"const" choice array bindings from the
        //     second 'bdem_ChoiceArray' and call the 'choiceArrayBinding'
        //     methods to obtain new choice array bindings for the
        //      'bdem_ChoiceArray'.  Use the direct accessors to verify that
        //     the choice array and record have been installed correctly in the
        //     returned bindings.
        //
        //     Perform the same test when the 'bdem_ChoiceArray' contains
        //     rows of a constrained type.
        //
        //
        // Testing:
        //             bdem_ConstChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstRowBinding rowBinding(int) const;
        //   bdem_ConstTableBinding tableBinding(int) const;
        //   bdem_ConstChoiceBinding choiceBinding(int) const;
        //   bdem_ConstChoiceArrayBinding choiceArrayBinding(int) const;
        //
        //             bdem_ChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   bdem_RowBinding rowBinding(int) const;
        //   bdem_TableBinding tableBinding(int) const;
        //   bdem_ChoiceBinding choiceBinding(int) const;
        //   bdem_ChoiceArrayBinding choiceArrayBinding(int) const;

        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Choice Array Binding 'XXXBinding'" << endl
                 << "==========================================" << endl;

        if (veryVerbose) {
          cout << "Functions returning row bindings." << endl;
        }
        {
            if (veryVerbose) {
                T_ cout << "Having element of an unconstrained type." << endl;
            }
            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b?+ca";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                ChoiceArray mCA;  const ChoiceArray& CA = mCA;
                createChoiceArray(&mCA, bRec);

                CCArrBind ccab( &CA, &S, "b");  const CCArrBind& CCAB = ccab;
                 CArrBind  cab(&mCA, &S, "b");  const  CArrBind&  CAB =  cab;

                for (int index = 0; index < CA.length(); ++ index) {
                  List *mL = &mCA.theModifiableItem(index).theModifiableList();
                  const List& L = *mL;

                  if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Row: "; P(L); N_
                  }

                         CRowBind  crb1(CCAB.rowBinding(index));
                   const CRowBind&  CRB1 = crb1;

                          RowBind  rb1(CAB.rowBinding(index));
                   const  RowBind& RB1 = rb1;

                   ASSERT(&L.row() == &CRB1.row());
                   ASSERT(&L.row() == &RB1.row());

                   ASSERT(aRec == &CRB1.record());
                   ASSERT(aRec ==  &RB1.record());
                 }
            }

            if (veryVerbose) {
                T_ cout << "Having element of a constrained type." << endl;
            }
            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d?+fb";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *dRec = S.lookupRecord("d");

                ChoiceArray mCA;  const ChoiceArray& CA = mCA;
                createChoiceArray(&mCA, dRec);

                CCArrBind ccab( &CA, &S, "d");  const CCArrBind& CCAB = ccab;
                 CArrBind  cab(&mCA, &S, "d");  const  CArrBind&  CAB =  cab;

                for (int index = 0; index < CA.length(); ++ index) {
                  List *mL = &mCA.theModifiableItem(index).theModifiableList();
                  const List& L = *mL;

                  if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Row: "; P(L); N_
                  }

                        CRowBind  crb1(CCAB.rowBinding(index));
                  const CRowBind& CRB1 = crb1;

                          RowBind   rb1(CAB.rowBinding(index));
                  const  RowBind&  RB1 = rb1;

                  ASSERT(&L.row() == &CRB1.row());
                  ASSERT(&L.row() ==  &RB1.row());

                  ASSERT(bRec == &CRB1.record());
                  ASSERT(bRec ==  &RB1.record());
                }
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning table bindings." << endl;
        }
        {
            if (veryVerbose) {
                T_ cout << "Having an element of unconstrained type." << endl;
            }
            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b?#ca";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                ChoiceArray mCA;  const ChoiceArray& CA = mCA;
                createChoiceArray(&mCA, bRec);

                CCArrBind ccab( &CA, &S, "b");  const CCArrBind& CCAB = ccab;
                 CArrBind  cab(&mCA, &S, "b"); const  CArrBind&  CAB =  cab;

                for (int index = 0; index < CA.length(); ++ index) {
                  Table *mTx =
                            &mCA.theModifiableItem(index).theModifiableTable();
                  const Table& Tx = *mTx;

                  if (veryVeryVerbose) {
                      N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                  }

                        CTblBind  ctb1(CCAB.tableBinding(index));
                  const CTblBind& CTB1 = ctb1;

                         TblBind   tb1(CAB.tableBinding(index));
                  const  TblBind&  TB1 = tb1;

                  ASSERT(&Tx == &CTB1.table());
                  ASSERT(&Tx ==  &TB1.table());

                  ASSERT(aRec == &CTB1.record());
                  ASSERT(aRec ==  &TB1.record());
                }
            }

            if (veryVerbose) {
                T_ cout << "Having an element of a constrained type." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d?#fb";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if
                // 'b' contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *dRec = S.lookupRecord("d");

                ChoiceArray mCA;  const ChoiceArray& CA = mCA;
                createChoiceArray(&mCA, dRec);

                CCArrBind ccab( &CA, &S, "d");  const CCArrBind& CCAB = ccab;
                 CArrBind  cab(&mCA, &S, "d");  const  CArrBind&  CAB =  cab;

                for (int index = 0; index < CA.length(); ++ index) {
                  Table *mTx =
                            &mCA.theModifiableItem(index).theModifiableTable();
                  const Table& Tx = *mTx;

                  if (veryVeryVerbose) {
                      N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                  }

                        CTblBind  ctb1(CCAB.tableBinding(index));
                  const CTblBind& CTB1 = ctb1;

                         TblBind   tb1(CAB.tableBinding(index));
                  const  TblBind&  TB1 = tb1;

                  ASSERT(&Tx == &CTB1.table());
                  ASSERT(&Tx ==  &TB1.table());

                  ASSERT(bRec == &CTB1.record());
                  ASSERT(bRec ==  &TB1.record());
                }
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice bindings." << endl;
        }

        {
            if (veryVerbose) {
                N_ cout << "Returning choices between unconstrained "
                        << "types." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?$a$b :b?%aa";
                spec[5] = bdemType[i];  // replace '$' in 'spec'
                // replace the second $ by another type
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");

                ChoiceArray mCA;  const ChoiceArray& CA = mCA;
                createChoiceArray(&mCA, bRec);

                CCArrBind ccab( &CA, &S, "b");  const CCArrBind& CCAB = ccab;
                 CArrBind  cab(&mCA, &S, "b");  const  CArrBind&  CAB =  cab;

                for (int index = 0; index < CA.length(); ++ index) {
                    // choiceBinding methods return a choice over the
                    // specified index - do that directly and compare.

                    if (veryVeryVerbose) {
                       N_ T2_ cout << "Underlying Element: "; P(CCAB[index]);N_
                    }

                          CCBind  ccb(CCAB.choiceBinding(index));
                    const CCBind& CCB = ccb;

                          CBind   cb(CAB.choiceBinding(index));
                    const CBind&  CB = cb;

                    ASSERT(&CCAB[index] == &CCB.item());
                    ASSERT( &CAB[index] ==  &CB.item());

                    ASSERT(bRec == &CCB.record());
                    ASSERT(bRec ==  &CB.record());
                }
            }

        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice array bindings." << endl;
        }

        {
            if (veryVerbose) {
                 N_ cout << "Returning choices arrays between "
                         << "unconstrained types." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?%a%b :b?@aa";
                spec[5] = bdemType[i];  // replace '%' in 'spec'
                // replace the second % by another type
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                ChoiceArray mCA;  const ChoiceArray& CA = mCA;
                createChoiceArray(&mCA, bRec);

                CCArrBind ccab( &CA, &S, "b");  const CCArrBind& CCAB = ccab;
                 CArrBind  cab(&mCA, &S, "b");  const  CArrBind&  CAB =  cab;

                for (int index = 0; index < CA.length(); ++ index) {
                  ChoiceArray *mCAx =
                      &mCA.theModifiableItem(index).theModifiableChoiceArray();
                  const ChoiceArray& CAx = *mCAx;

                  if (veryVeryVerbose) {
                      N_ T2_ cout << "Underlying ChoiceArray: "; P(CAx); N_
                  }

                        CCArrBind  ccab1(CCAB.choiceArrayBinding(index));
                  const CCArrBind& CCAB1 = ccab1;

                         CArrBind   cab1(CAB.choiceArrayBinding(index));
                  const  CArrBind&  CAB1 = cab1;

                  ASSERT(&CAx == &CCAB1.choiceArray());
                  ASSERT(&CAx ==  &CAB1.choiceArray());

                  ASSERT(aRec == &CCAB1.record());
                  ASSERT(aRec ==  &CAB1.record());
                }
            }

            if (veryVerbose) {
                N_ cout << "Returning choice arrays between "
                        << "constrained types." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a$Aa :b?$aa :c?@ab";

                const char type = constrBdemType[i];
                // Replace '$' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[4] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = type;

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *cRec = S.lookupRecord("c");

                ChoiceArray mCA;  const ChoiceArray& CA = mCA;
                createChoiceArray(&mCA, cRec);

                CCArrBind ccab( &CA, &S, "c");  const CCArrBind& CCAB = ccab;
                 CArrBind  cab(&mCA, &S, "c");  const  CArrBind&  CAB =  cab;

                for (int index = 0; index < CA.length(); ++ index) {
                  ChoiceArray *mCAx =
                      &mCA.theModifiableItem(index).theModifiableChoiceArray();
                  const ChoiceArray& CAx = *mCAx;

                  if (veryVeryVerbose) {
                      N_ T2_ cout << "Underlying Choice: "; P(CAx); N_
                  }

                        CCArrBind  ccab1(CCAB.choiceArrayBinding(index));
                  const CCArrBind& CCAB1 = ccab1;

                         CArrBind   cab1(CAB.choiceArrayBinding(index));
                  const  CArrBind&  CAB1 = cab1;

                  ASSERT(&CAx == &CCAB1.choiceArray());
                  ASSERT(&CAx ==  &CAB1.choiceArray());

                  ASSERT(bRec == &CCAB1.record());
                  ASSERT(bRec ==  &CAB1.record());
                }
            }

        }

      }

DEFINE_TEST_CASE(25) {
        // ------------------------------------------------------------------
        // TESTING CHOICE BINDING xxxBINDING
        //   Test Choice Binding functions that return bindings.
        //
        // Concerns:
        //   - Valid bindings are returned by the 'xxxBinding' methods.
        //   - The data and meta-data are hooked up correctly.
        //
        // Plan:
        //   - Functions returning Row Bindings
        //     Create a 'bdem_List' containing an element of an unconstrained
        //     type.  Create a 'bdem_Choice' having the
        //     'bdem_List' as an element.  Create "const" and
        //     non-"const" choice bindings from
        //     the 'bdem_Choice' and call the 'rowBinding' methods to
        //     obtain new choice bindings for the 'bdem_Row'.  Use the
        //     direct accessors to verify that the row and record have been
        //     installed correctly in the returned bindings.
        //
        //     Perform the same test when the first 'bdem_Choice' contains an
        //     element of a constrained type.
        //
        //   - Functions returning Table Bindings
        //     Create a 'bdem_Table' containing rows of an unconstrained type.
        //     Create a 'bdem_Choice' having the 'bdem_Table' as an element.
        //     Create "const" and non-"const" choice bindings from the
        //     'bdem_Choice' and call the 'tableBinding' methods to
        //     obtain new table bindings for the 'bdem_Table'.
        //     Use the direct accessors to verify that
        //     the table and record have been installed correctly in the
        //     returned bindings.
        //
        //     Perform the same test when the 'bdem_Table' contains rows of a
        //     constrained type.
        //
        //   - Functions returning Choice Bindings
        //     Create a 'bdem_Choice' containing an element of an unconstrained
        //     type.  Create a second 'bdem_Choice' having the first
        //     'bdem_Choice' as an element.  Create "const" and
        //     non-"const" choice bindings from
        //     the second 'bdem_Choice' and call the 'choiceBinding' methods to
        //     obtain new choice bindings for the first 'bdem_Choice'.  Use the
        //     direct accessors to verify that the choice and record have been
        //     installed correctly in the returned bindings.
        //
        //     Perform the same test when the first 'bdem_Choice' contains an
        //     element of a constrained type.
        //   - Functions returning Choice Array Bindings
        //     Create a 'bdem_ChoiceArray' containing selections of an
        //     unconstrained type.  Create a 'bdem_Choice'
        //     having the 'bdem_ChoiceArray' as an element.
        //     Create "const" and non-"const" choice bindings from the
        //     'bdem_Choice' and call the 'choiceArrayBinding' methods to
        //     obtain new choice array bindings for the 'bdem_ChoiceArray'.
        //     Use the direct accessors to verify that
        //     the table and record have been installed correctly in the
        //     returned bindings.
        //
        //     Perform the same test when the 'bdem_ChoiceArray' contains
        //     rows of a constrained type.
        //
        //
        // Testing:
        //             bdem_ConstChoiceBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstRowBinding rowBinding() const;
        //   bdem_ConstTableBinding tableBinding() const;
        //   bdem_ConstChoiceBinding choiceBinding() const;
        //   bdem_ConstChoiceArrayBinding choiceArrayBinding() const;
        //
        //             bdem_ChoiceBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   bdem_RowBinding rowBinding() const;
        //   bdem_TableBinding tableBinding() const;
        //   bdem_ChoiceBinding choiceBinding() const;
        //   bdem_ChoiceArrayBinding choiceArrayBinding() const;

        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Choice Binding 'XXXBinding'" << endl
                 << "==========================================" << endl;

        if (veryVerbose) {
          cout << "Functions returning row bindings." << endl;
        }
        {
            if (veryVerbose) {
                T_ cout << "Having element of an unconstrained type." << endl;
            }
            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b?+ca";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                Choice mC;  const Choice& C = mC;  createChoice(&mC, bRec);

                CCBind ccb( &C, &S, "b");  const CCBind& CCB = ccb;
                 CBind  cb(&mC, &S, "b");  const  CBind&  CB =  cb;

                List *mL = &mC.theModifiableList();
                const List& L = *mL;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Row: "; P(L); N_
                }

                      CRowBind  crb1(CCB.rowBinding());
                const CRowBind&  CRB1 = crb1;

                       RowBind  rb1(CB.rowBinding());
                const  RowBind& RB1 = rb1;

                ASSERT(&L.row() == &CRB1.row());
                ASSERT(&L.row() == &RB1.row());

                ASSERT(aRec == &CRB1.record());
                ASSERT(aRec ==  &RB1.record());
            }

            if (veryVerbose) {
                T_ cout << "Having element of a constrained type." << endl;
            }
            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d?+fb";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *dRec = S.lookupRecord("d");

                Choice mC;  const Choice& C = mC;  createChoice(&mC, dRec);

                CCBind ccb( &C, &S, "d");  const CCBind& CCB = ccb;
                 CBind  cb(&mC, &S, "d");  const  CBind&  CB =  cb;

                List *mL = &mC.theModifiableList();
                const List& L = *mL;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Row: "; P(L); N_
                }

                      CRowBind  crb1(CCB.rowBinding());
                const CRowBind& CRB1 = crb1;

                       RowBind   rb1(CB.rowBinding());
                const  RowBind&  RB1 = rb1;

                ASSERT(&L.row() == &CRB1.row());
                ASSERT(&L.row() ==  &RB1.row());

                ASSERT(bRec == &CRB1.record());
                ASSERT(bRec ==  &RB1.record());
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning table bindings." << endl;
        }
        {
            if (veryVerbose) {
                T_ cout << "Having an element of unconstrained type." << endl;
            }
            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b?#ca";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                Choice mC;  const Choice& C = mC;  createChoice(&mC, bRec);

                CCBind ccb( &C, &S, "b");  const CCBind& CCB = ccb;
                 CBind  cb(&mC, &S, "b");  const  CBind&  CB =  cb;

                Table *mTx = &mC.theModifiableTable();
                const Table& Tx = *mTx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                }

                      CTblBind  ctb1(CCB.tableBinding());
                const CTblBind& CTB1 = ctb1;

                       TblBind   tb1(CB.tableBinding());
                const  TblBind&  TB1 = tb1;

                ASSERT(&Tx == &CTB1.table());
                ASSERT(&Tx ==  &TB1.table());

                ASSERT(aRec == &CTB1.record());
                ASSERT(aRec ==  &TB1.record());
            }

            if (veryVerbose) {
                T_ cout << "Having an element of a constrained type." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d?#fb";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if
                // 'b' contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *dRec = S.lookupRecord("d");

                Choice mC;  const Choice& C = mC;  createChoice(&mC, dRec);

                CCBind ccb( &C, &S, "d");  const CCBind& CCB = ccb;
                 CBind  cb(&mC, &S, "d");  const  CBind&  CB =  cb;

                Table *mTx = &mC.theModifiableTable();  const Table& Tx = *mTx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                }

                      CTblBind  ctb1(CCB.tableBinding());
                const CTblBind& CTB1 = ctb1;

                       TblBind   tb1(CB.tableBinding());
                const  TblBind&  TB1 = tb1;

                ASSERT(&Tx == &CTB1.table());
                ASSERT(&Tx ==  &TB1.table());

                ASSERT(bRec == &CTB1.record());
                ASSERT(bRec ==  &TB1.record());
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice bindings." << endl;
        }

        {
            if (veryVerbose) {
                N_ cout << "Returning choices between unconstrained "
                        << "types." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?$a$b :b?%aa";
                spec[5] = bdemType[i];  // replace '$' in 'spec'
                // replace the second $ by another type
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                Choice mC;  const Choice& C = mC;  createChoice(&mC, bRec);

                CCBind ccb( &C, &S, "b");  const CCBind& CCB = ccb;
                 CBind  cb(&mC, &S, "b");  const  CBind&  CB =  cb;

                Choice *mCx = &mC.theModifiableChoice();
                const Choice& Cx = *mCx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Choice: "; P(Cx); N_
                }

                      CCBind  ccb1(CCB.choiceBinding());
                const CCBind& CCB1 = ccb1;

                       CBind   cb1(CB.choiceBinding());
                const  CBind&  CB1 = cb1;

                ASSERT(&Cx.item() == &CCB1.item());
                ASSERT(&Cx.item() ==  &CB1.item());

                ASSERT(aRec == &CCB1.record());
                ASSERT(aRec ==  &CB1.record());
            }

            if (veryVerbose) {
                N_ cout << "Returning choices between constrained "
                        << "types." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED
                // CHOICE Field must reference a CHOICE Record
                static char spec[] = "~ :a$Aa :b?$aa :c?%ab";

                const char type = constrBdemType[i];
                // Replace '$' - record 'a' type must be "choice" if
                // 'b' contains a constrained choice
                spec[4] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = type;

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *cRec = S.lookupRecord("c");

                Choice mC;  const Choice& C = mC;  createChoice(&mC, cRec);

                CCBind ccb( &C, &S, "c");  const CCBind& CCB = ccb;
                 CBind  cb(&mC, &S, "c");  const  CBind&  CB =  cb;

                Choice *mCx = &mC.theModifiableChoice();
                const Choice& Cx = *mCx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Choice: "; P(Cx); N_
                }

                      CCBind  ccb1(CCB.choiceBinding());
                const CCBind& CCB1 = ccb1;

                       CBind   cb1(CB.choiceBinding());
                const  CBind&  CB1 = cb1;

                ASSERT(&Cx.item() == &CCB1.item());
                ASSERT(&Cx.item() ==  &CB1.item());

                ASSERT(bRec == &CCB1.record());
                ASSERT(bRec ==  &CB1.record());
            }

        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice array bindings." << endl;
        }

        {
            if (veryVerbose) {
                 N_ cout << "Returning choices arrays between "
                         << "unconstrained types." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?%a%b :b?@aa";
                spec[5] = bdemType[i];  // replace '%' in 'spec'
                // replace the second % by another type
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                Choice mC;  const Choice& C = mC;  createChoice(&mC, bRec);

                CCBind ccb( &C, &S, "b");  const CCBind& CCB = ccb;
                 CBind  cb(&mC, &S, "b");  const  CBind&  CB =  cb;

                ChoiceArray *mCAx = &mC.theModifiableChoiceArray();
                const ChoiceArray& CAx = *mCAx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying ChoiceArray: "; P(CAx); N_
                }

                      CCArrBind  ccab1(CCB.choiceArrayBinding());
                const CCArrBind& CCAB1 = ccab1;

                       CArrBind   cab1(CB.choiceArrayBinding());
                const  CArrBind&  CAB1 = cab1;

                ASSERT(&CAx == &CCAB1.choiceArray());
                ASSERT(&CAx ==  &CAB1.choiceArray());

                ASSERT(aRec == &CCAB1.record());
                ASSERT(aRec ==  &CAB1.record());
            }

            if (veryVerbose) {
                N_ cout << "Returning choice arrays between "
                        << "constrained types." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a$Aa :b?$aa :c?@ab";

                const char type = constrBdemType[i];
                // Replace '$' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[4] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = type;

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *cRec = S.lookupRecord("c");

                Choice mC;  const Choice& C = mC;  createChoice(&mC, cRec);

                CCBind ccb( &C, &S, "c");  const CCBind& CCB = ccb;
                 CBind  cb(&mC, &S, "c");  const  CBind&  CB =  cb;

                ChoiceArray *mCAx = &mC.theModifiableChoiceArray();
                const ChoiceArray& CAx = *mCAx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Choice: "; P(CAx); N_
                }

                      CCArrBind  ccab1(CCB.choiceArrayBinding());
                const CCArrBind& CCAB1 = ccab1;

                       CArrBind   cab1(CB.choiceArrayBinding());
                const  CArrBind&  CAB1 = cab1;

                ASSERT(&CAx == &CCAB1.choiceArray());
                ASSERT(&CAx ==  &CAB1.choiceArray());

                ASSERT(bRec == &CCAB1.record());
                ASSERT(bRec ==  &CAB1.record());
            }

        }

      }

DEFINE_TEST_CASE(24) {
        // ------------------------------------------------------------------
        // TESTING COLUMN BINDING xxxBINDING
        //   Test Column Binding functions that return bindings.
        //
        // Concerns:
        //   - Valid bindings are returned by the 'xxxBinding' methods.
        //   - The data, column index, and meta-data are hooked up correctly.
        //
        // Plan:
        //   - Functions returning Row Bindings
        //     Create a 'bdem_Table' whose rows have a 'bdem_List' that
        //     contain an element of an unconstrained type.  Create "const"
        //     and non-"const" column bindings from the 'bdem_Table' and the
        //     column index corresponding to the 'bdem_List' and call the
        //     'rowBinding' methods to obtain new row bindings for the
        //     'bdem_List'.  Use the direct accessors to verify that the row
        //     and record have been installed correctly in the returned
        //     bindings.
        //
        //     Perform the same test when the 'bdem_List' contains an element
        //     of a constrained type.
        //
        //   - Functions returning Table Bindings
        //     Create a 'bdem_Table' whose rows have a 'bdem_Table' where each
        //     row of the second 'bdem_Table' contains an element of an
        //     unconstrained type.  Create "const" and non-"const" column
        //     bindings from the second 'bdem_Table' and the column index
        //     corresponding to the first 'bdem_Table', and call the
        //     'tableBinding' methods to obtain new table bindings for the
        //     second 'bdem_Table'.  Use the direct accessors to verify that
        //     the table and record have been installed correctly in the
        //     returned bindings.
        //
        //     Perform the same test when the second 'bdem_Table' contains
        //     elements of a constrained type.
        //
        //   - Functions returning Choice Bindings
        //     Create a 'bdem_Choice' containing an element of an unconstrained
        //     type.  Create a 'bdem_Table' having the first
        //     'bdem_Choice' as an element.  Create "const" and
        //     non-"const" column bindings from
        //     the 'bdem_Table' and call the 'choiceBinding' methods to
        //     obtain new choice bindings for the first 'bdem_Choice'.  Use the
        //     direct accessors to verify that the choice and record have been
        //     installed correctly in the returned bindings.
        //
        //     Perform the same test when the first 'bdem_Choice' contains an
        //     element of a constrained type.
        //   - Functions returning Choice Array Bindings
        //     Create a 'bdem_ChoiceArray' containing rows of an
        //     unconstrained type.  Create a 'bdem_Table'
        //     having the 'bdem_ChoiceArray' as an element.
        //     Create "const" and non-"const" column bindings from the
        //     'bdem_Table'.
        //     Use the direct accessors to verify that the choice array and
        //     record have been installed correctly in the returned bindings.
        //     Perform the same test when the 'bdem_ChoiceArray' contains
        //     rows of a constrained type.
        //
        // Testing:
        //             bdem_ConstColumnBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstRowBinding rowBinding(int) const;
        //   bdem_ConstTableBinding tableBinding(int) const;
        //   bdem_ConstChoiceBinding choiceBinding(int) const;
        //   bdem_ConstChoiceArrayBinding choiceArrayBinding(int) const;
        //
        //             bdem_ColumnBinding
        //             ~~~~~~~~~~~~~~~~~~
        //   bdem_RowBinding rowBinding(int) const;
        //   bdem_TableBinding tableBinding(int) const;
        //   bdem_ConstChoiceBinding choiceBinding(int) const;
        //   bdem_ConstChoiceArrayBinding choiceArrayBinding(int) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Column Binding 'XXXBinding'" << endl
                 << "===================================" << endl;

        if (veryVerbose) {
            cout << "Functions returning row bindings." << endl;
        }
        {
            if (veryVerbose) {
                T_ cout << "Having element of an unconstrained type." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b+ca :d#fb";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *dRec =  S.lookupRecord("d");
                const RecDef *aRec =  S.lookupRecord("a");
                const FldDef *bFld = &S.lookupRecord("b")->field(0);

                List mL;  createList(&mL, dRec);

                Table *mT = &mL.theModifiableTable(0);

                      CColBind  ccb(mT, 0, bFld);
                       ColBind   cb(mT, 0, bFld);

                const CColBind& CCB = ccb;
                const  ColBind&  CB =  cb;

                for (int row = 0; row < CCB.numRows(); ++row) {
                    List *mL2 = &CB[row].theModifiableList();
                    const List& L2 = *mL2;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Row: "; P(L2); N_
                    }

                          CRowBind  crb(CCB.rowBinding(row));
                           RowBind   rb( CB.rowBinding(row));

                    const CRowBind& CRB = crb;
                    const  RowBind&  RB =  rb;

                    ASSERT(&L2.row() == &CRB.row());
                    ASSERT(&L2.row() ==  &RB.row());

                    ASSERT(aRec == &CRB.record());
                    ASSERT(aRec ==  &RB.record());
                }
            }

            if (veryVerbose) {
                T_ cout << "Having element of a constrained type." << endl;
            }
            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE
                // Field must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d+fb :h#gd";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if
                // 'b' contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *hRec =  S.lookupRecord("h");
                const RecDef *bRec =  S.lookupRecord("b");
                const FldDef *dFld = &S.lookupRecord("d")->field(0);

                List mL;  createList(&mL, hRec);

                Table *mT = &mL.theModifiableTable(0);

                      CColBind  ccb(mT, 0, dFld);
                       ColBind   cb(mT, 0, dFld);

                const CColBind& CCB = ccb;
                const  ColBind&  CB =  cb;

                for (int row = 0; row < CCB.numRows(); ++row) {
                    List *mL2 = &CB[row].theModifiableList();
                    const List& L2 = *mL2;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Row: "; P(L2); N_
                    }

                          CRowBind  crb(CCB.rowBinding(row));
                           RowBind   rb( CB.rowBinding(row));

                    const CRowBind& CRB = crb;
                    const  RowBind&  RB =  rb;

                    ASSERT(&L2.row() == &CRB.row());
                    ASSERT(&L2.row() ==  &RB.row());

                    ASSERT(bRec == &CRB.record());
                    ASSERT(bRec ==  &RB.record());
                }
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning table bindings." << endl;
        }
        {
            if (veryVerbose) {
                T_ cout << "Having rows of an unconstrained type." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b#ca :d#fb";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *dRec =  S.lookupRecord("d");
                const RecDef *aRec =  S.lookupRecord("a");
                const FldDef *bFld = &S.lookupRecord("b")->field(0);

                List mL;  createList(&mL, dRec);

                Table *mT = &mL.theModifiableTable(0);

                      CColBind  ccb(mT, 0, bFld);
                       ColBind   cb(mT, 0, bFld);

                const CColBind& CCB = ccb;
                const  ColBind&  CB =  cb;

                for (int row = 0; row < CCB.numRows(); ++row) {
                    Table *mTx = &CB[row].theModifiableTable();
                    const Table& Tx = *mTx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                    }

                          CTblBind  ctb(CCB.tableBinding(row));
                           TblBind   tb( CB.tableBinding(row));

                    const CTblBind& CTB = ctb;
                    const  TblBind&  TB =  tb;

                    ASSERT(&Tx == &CTB.table());
                    ASSERT(&Tx ==  &TB.table());

                    ASSERT(aRec == &CTB.record());
                    ASSERT(aRec ==  &TB.record());
                }
            }

            if (veryVerbose) {
                T_ cout << "Having rows of a constrained type." << endl;
            }
            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE
                // Field must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d#fb :h#gd";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if
                // 'b' contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *hRec =  S.lookupRecord("h");
                const RecDef *bRec =  S.lookupRecord("b");
                const FldDef *dFld = &S.lookupRecord("d")->field(0);

                List mL;  createList(&mL, hRec);

                Table *mT = &mL.theModifiableTable(0);

                      CColBind  ccb(mT, 0, dFld);
                       ColBind   cb(mT, 0, dFld);

                const CColBind& CCB = ccb;
                const  ColBind&  CB = cb;

                for (int row = 0; row < CCB.numRows(); ++row) {
                    Table *mTx = &CB[row].theModifiableTable();
                    const Table& Tx = *mTx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                    }

                          CTblBind  ctb(CCB.tableBinding(row));
                           TblBind   tb( CB.tableBinding(row));

                    const CTblBind& CTB = ctb;
                    const  TblBind&  TB =  tb;

                    ASSERT(&Tx == &CTB.table());
                    ASSERT(&Tx ==  &TB.table());

                    ASSERT(bRec == &CTB.record());
                    ASSERT(bRec ==  &TB.record());
                }
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice bindings." << endl;
        }

        {
            if (veryVerbose) {
                N_ cout << "Returning choices between unconstrained types."
                        << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?%a%b :b%aa :c#ab";
                spec[5] = bdemType[i];  // replace '%' in 'spec'
                // replace the second % by another type
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *cRec = S.lookupRecord("c");
                const FldDef *aFld = &S.lookupRecord("b")->field(0);

                List mL;  createList(&mL, cRec);

                Table *mT = &mL.theModifiableTable(0);

                CColBind ccb( mT, 0, aFld);  const CColBind& CCB = ccb;
                 ColBind  cb( mT, 0, aFld);  const  ColBind&  CB =  cb;

                for (int row = 0; row < CCB.numRows(); ++row) {
                    Choice *mCx = &CB[row].theModifiableChoice();
                    const Choice& Cx = *mCx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Choice: "; P(Cx); N_
                    }

                          CCBind  ccb1(CCB.choiceBinding(row));
                          CCBind  ccb2(CCB.choiceBinding(row));
                    const CCBind& CCB1 = ccb1;
                    const CCBind& CCB2 = ccb2;

                           CBind   cb1(CB.choiceBinding(row));
                           CBind   cb2(CB.choiceBinding(row));
                    const  CBind&  CB1 = cb1;
                    const  CBind&  CB2 = cb2;

                    ASSERT(&Cx.item() == &CCB1.item());
                    ASSERT(&Cx.item() == &CCB2.item());
                    ASSERT(&Cx.item() ==  &CB1.item());
                    ASSERT(&Cx.item() ==  &CB2.item());

                    ASSERT(aRec == &CCB1.record());
                    ASSERT(aRec == &CCB2.record());
                    ASSERT(aRec ==  &CB1.record());
                    ASSERT(aRec ==  &CB2.record());
                }
            }

            if (veryVerbose) {
                N_ cout << "Returning choices between constrained types."
                        << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field must
                // reference a CHOICE Record
                static char spec[] = "~ :a$Aa :b?$aa :c%ab :d#ac";

                const char type = constrBdemType[i];
                // Replace '$' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[4] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = type;

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *dRec = S.lookupRecord("d");
                const FldDef *cFld = &S.lookupRecord("c")->field(0);

                List mL;  createList(&mL, dRec);

                Table *mT = &mL.theModifiableTable(0);

                CColBind ccb( mT, 0, cFld);  const CColBind& CCB = ccb;
                 ColBind  cb( mT, 0,cFld);   const  ColBind&  CB =  cb;

                for (int row = 0; row < CCB.numRows(); ++row) {
                    Choice *mCx = &CB[row].theModifiableChoice();
                    const Choice& Cx = *mCx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Choice: "; P(Cx); N_
                    }

                          CCBind  ccb1(CCB.choiceBinding(row));
                          CCBind  ccb2(CCB.choiceBinding(row));
                    const CCBind& CCB1 = ccb1;
                    const CCBind& CCB2 = ccb2;

                           CBind   cb1(CB.choiceBinding(row));
                           CBind   cb2(CB.choiceBinding(row));
                    const  CBind&  CB1 = cb1;
                    const  CBind&  CB2 = cb2;

                    ASSERT(&Cx.item() == &CCB1.item());
                    ASSERT(&Cx.item() == &CCB2.item());
                    ASSERT(&Cx.item() ==  &CB1.item());
                    ASSERT(&Cx.item() ==  &CB2.item());

                    ASSERT(bRec == &CCB1.record());
                    ASSERT(bRec == &CCB2.record());
                    ASSERT(bRec ==  &CB1.record());
                    ASSERT(bRec ==  &CB2.record());
                }
            }

        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice array bindings." << endl;
        }

        {
            if (veryVerbose) {
                N_ cout << "Returning choices arrays between"
                        << " unconstrained types." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?%a%b :b@aa :c#ab";
                spec[5] = bdemType[i];  // replace '%' in 'spec'
                // replace the second % by another type
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *cRec = S.lookupRecord("c");
                const FldDef *bFld = &S.lookupRecord("b")->field(0);

               List mL;  createList(&mL, cRec);

               Table *mT = &mL.theModifiableTable(0);

               CColBind ccb( mT, 0, bFld);  const CColBind& CCB =   ccb;
               ColBind   cb(mT, 0, bFld);   const ColBind&  CB  =   cb;

                for (int row = 0; row < CCB.numRows(); ++row) {
                    ChoiceArray *mCAx = &CB[row].theModifiableChoiceArray();
                    const ChoiceArray& CAx = *mCAx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying ChoiceArray: "; P(CAx); N_
                    }

                          CCArrBind  ccab1(CCB.choiceArrayBinding(row));
                          CCArrBind  ccab2(CCB.choiceArrayBinding(row));
                    const CCArrBind& CCAB1 = ccab1;
                    const CCArrBind& CCAB2 = ccab2;

                           CArrBind   cab1(CB.choiceArrayBinding(row));
                           CArrBind   cab2(CB.choiceArrayBinding(row));
                    const  CArrBind&  CAB1 = cab1;
                    const  CArrBind&  CAB2 = cab2;

                    ASSERT(&CAx == &CCAB1.choiceArray());
                    ASSERT(&CAx == &CCAB2.choiceArray());
                    ASSERT(&CAx ==  &CAB1.choiceArray());
                    ASSERT(&CAx ==  &CAB2.choiceArray());

                    ASSERT(aRec == &CCAB1.record());
                    ASSERT(aRec == &CCAB2.record());
                    ASSERT(aRec ==  &CAB1.record());
                    ASSERT(aRec ==  &CAB2.record());
                }
            }

            if (veryVerbose) {
                N_ cout << "Returning choice arrays between "
                        << "constrained types." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a$Aa :b?$aa :c@ab :d#ac";

                const char type = constrBdemType[i];
                // Replace '$' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[4] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = type;

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *dRec = S.lookupRecord("d");
                const FldDef *cFld = &S.lookupRecord("c")->field(0);

                List mL;  createList(&mL, dRec);

                Table *mT = &mL.theModifiableTable(0);

                CColBind ccb( mT, 0, cFld);  const CColBind& CCB = ccb;
                 ColBind  cb( mT, 0, cFld);  const  ColBind&  CB =  cb;

                for (int row = 0; row < CB.numRows(); ++row) {
                    ChoiceArray *mCAx = &CB[row].theModifiableChoiceArray();
                    const ChoiceArray& CAx = *mCAx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Choice: "; P(CAx); N_
                    }

                          CCArrBind  ccab1(CCB.choiceArrayBinding(row));
                          CCArrBind  ccab2(CCB.choiceArrayBinding(row));
                    const CCArrBind& CCAB1 = ccab1;
                    const CCArrBind& CCAB2 = ccab2;

                           CArrBind   cab1(CB.choiceArrayBinding(row));
                           CArrBind   cab2(CB.choiceArrayBinding(row));
                    const  CArrBind&  CAB1 = cab1;
                    const  CArrBind&  CAB2 = cab2;

                    ASSERT(&CAx == &CCAB1.choiceArray());
                    ASSERT(&CAx == &CCAB2.choiceArray());
                    ASSERT(&CAx ==  &CAB1.choiceArray());
                    ASSERT(&CAx ==  &CAB2.choiceArray());

                    ASSERT(bRec == &CCAB1.record());
                    ASSERT(bRec == &CCAB2.record());
                    ASSERT(bRec ==  &CAB1.record());
                    ASSERT(bRec ==  &CAB2.record());
                }
            }

        }

      }

DEFINE_TEST_CASE(23) {
        // ------------------------------------------------------------------
        // TESTING TABLE BINDING xxxBINDING
        //   Test Table Binding functions that return bindings.
        //
        // Concerns:
        //   - Valid bindings are returned by the 'xxxBinding' methods.
        //   - The data and meta-data are hooked up correctly.
        //
        // Plan:
        //   - Functions returning Row Bindings
        //     Create a 'bdem_Table' whose rows have a 'bdem_List' that
        //     contains an element of an unconstrained type.  Create "const"
        //     and non-"const" table bindings from the 'bdem_Table' and call
        //     the 'rowBinding' methods to obtain new row bindings for the
        //     'bdem_List'.  Use the direct accessors to verify that the row
        //     and record have been installed correctly in the returned
        //     bindings.
        //
        //     Perform the same test when the 'bdem_List' contains elements of
        //     a constrained type.
        //
        //   - Functions returning Table Bindings
        //     Create a 'bdem_Table' whose rows have a 'bdem_Table' where each
        //     row of the second 'bdem_Table' contains an element of an
        //     unconstrained type.  Create "const" and non-"const" table
        //     bindings from the first 'bdem_Table' and call the 'tableBinding'
        //     methods to obtain new table bindings for the second
        //     'bdem_Table'.  Use the direct accessors to verify that the table
        //     and record have been installed correctly in the returned
        //     bindings.
        //
        //     Perform the same test when the second 'bdem_Table' contains
        //     elements of a constrained type.
        //
        //   - Functions returning Column Bindings
        //     Create a 'bdem_Table' whose rows contain elements of an
        //     unconstrained type.  Create "const" and non-"const" table
        //     bindings from that 'bdem_Table' and call the 'columnBinding'
        //     methods to obtain new column bindings for the column of the
        //     unconstrained types.  Use the direct accessors to verify that
        //     the table, column index, and field have been installed correctly
        //     in the returned bindings.
        //
        //     Perform the same test when the 'bdem_Table' contains elements of
        //     a constrained type.
        //
        //   - Functions returning Choice Bindings
        //     Create a 'bdem_Choice' containing an element of an unconstrained
        //     type.  Create a 'bdem_Table' having the first
        //     'bdem_Choice' as an element.  Create "const" and
        //     non-"const" table bindings from
        //     the 'bdem_Table' and call the 'choiceBinding' methods to
        //     obtain new choice bindings for the first 'bdem_Choice'.  Use the
        //     direct accessors to verify that the choice and record have been
        //     installed correctly in the returned bindings.
        //
        //     Perform the same test when the first 'bdem_Choice' contains an
        //     element of a constrained type.
        //   - Functions returning Choice Array Bindings
        //     Create a 'bdem_ChoiceArray' containing rows of an
        //     unconstrained type.  Create a 'bdem_Table'
        //     having the 'bdem_ChoiceArray' as an element.
        //     Create "const" and non-"const" table bindings from the
        //     'bdem_Table'.
        //     Use the direct accessors to verify that
        //     the choice array and record have been installed correctly in the
        //     returned bindings.
        //
        //     Perform the same test when the 'bdem_ChoiceArray' contains
        //     rows of a constrained type.
        //
        //
        // Testing:
        //             bdem_ConstTableBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstRowBinding rowBinding(int) const;
        //   bdem_ConstRowBinding rowBinding(int, *name) const;
        //   bdem_ConstRowBinding rowBinding(int, int) const;
        //   bdem_ConstTableBinding tableBinding(int, *name) const;
        //   bdem_ConstTableBinding tableBinding(int, int) const;
        //   bdem_ConstColumnBinding columnBinding(*name) const;
        //   bdem_ConstColumnBinding columnBinding(int) const;
        //   bdem_ConstChoiceBinding choiceBinding(*name) const;
        //   bdem_ConstChoiceBinding choiceBinding(int) const;
        //   bdem_ConstChoiceArrayBinding choiceArrayBinding(*name) const;
        //   bdem_ConstChoiceArrayBinding choiceArrayBinding(int) const;
        //
        //             bdem_TableBinding
        //             ~~~~~~~~~~~~~~~~~
        //   bdem_RowBinding rowBinding(int) const;
        //   bdem_RowBinding rowBinding(int, *name) const;
        //   bdem_RowBinding rowBinding(int, int) const;
        //   bdem_TableBinding tableBinding(int, *name) const;
        //   bdem_TableBinding tableBinding(int, int) const;
        //   bdem_ColumnBinding columnBinding(*name) const;
        //   bdem_ColumnBinding columnBinding(int) const;
        //   bdem_ChoiceBinding choiceBinding(*name) const;
        //   bdem_ChoiceBinding choiceBinding(int) const;
        //   bdem_ChoiceArrayBinding choiceArrayBinding(*name) const;
        //   bdem_ChoiceArrayBinding choiceArrayBinding(int) const;

        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Table Binding 'XXXBinding'" << endl
                 << "==================================" << endl;

        if (veryVerbose) cout << "Functions returning row bindings." << endl;
        {
            if (veryVerbose) {
                T_ cout << "Having element of an unconstrained type." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b+ca";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *aRec = S.lookupRecord("a");

                Table mT;  const Table& T = mT;
                createTable(&mT, bRec);

                CTblBind ctb(&T, bRec);   const CTblBind& CTB = ctb;
                 TblBind  tb(&mT, bRec);  const  TblBind&  TB =  tb;

                for (int row = 0; row < T.numRows(); ++row) {
                    List *mL = &mT.theModifiableRow(row).theModifiableList(0);
                    const List& L = *mL;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Row: "; P(L); N_
                    }

                          CRowBind  crb1(CTB.rowBinding(row, "c"));
                          CRowBind  crb2(CTB.rowBinding(row,  0));
                          CRowBind  crb3(CTB.rowBinding(row));

                    const CRowBind& CRB1 = crb1;
                    const CRowBind& CRB2 = crb2;
                    const CRowBind& CRB3 = crb3;

                           RowBind   rb1(TB.rowBinding(row, "c"));
                           RowBind   rb2(TB.rowBinding(row, 0));
                           RowBind   rb3(TB.rowBinding(row));

                    const  RowBind&  RB1 = rb1;
                    const  RowBind&  RB2 = rb2;
                    const  RowBind&  RB3 = rb3;

                    ASSERT(&L.row() == &CRB1.row());
                    ASSERT(&L.row() == &CRB2.row());
                    ASSERT(&T[row]  == &CRB3.row());

                    ASSERT(&L.row() ==  &RB1.row());
                    ASSERT(&L.row() ==  &RB2.row());
                    ASSERT(&mT.theModifiableRow(row) ==  &RB3.row());

                    ASSERT(aRec == &CRB1.record());
                    ASSERT(aRec == &CRB2.record());
                    ASSERT(bRec == &CRB3.record());

                    ASSERT(aRec ==  &RB1.record());
                    ASSERT(aRec ==  &RB2.record());
                    ASSERT(bRec ==  &RB3.record());
                }
            }

            if (veryVerbose) {
                T_ cout << "Having element of a constrained type." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d+fb";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *dRec = S.lookupRecord("d");
                const RecDef *bRec = S.lookupRecord("b");

                Table mT;  const Table& T = mT;
                createTable(&mT, dRec);

                CTblBind ctb(&T, dRec);   const CTblBind& CTB = ctb;
                 TblBind  tb(&mT, dRec);  const  TblBind&  TB =  tb;

                for (int row = 0; row < T.numRows(); ++row) {
                    List *mL = &mT.theModifiableRow(row).theModifiableList(0);
                    const List& L = *mL;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Row: "; P(L); N_
                    }

                          CRowBind  crb1(CTB.rowBinding(row, "f"));
                          CRowBind  crb2(CTB.rowBinding(row,  0));
                          CRowBind  crb3(CTB.rowBinding(row));

                    const CRowBind& CRB1 = crb1;
                    const CRowBind& CRB2 = crb2;
                    const CRowBind& CRB3 = crb3;

                           RowBind   rb1(TB.rowBinding(row, "f"));
                           RowBind   rb2(TB.rowBinding(row, 0));
                           RowBind   rb3(TB.rowBinding(row));

                    const  RowBind&  RB1 = rb1;
                    const  RowBind&  RB2 = rb2;
                    const  RowBind&  RB3 = rb3;

                    ASSERT(&L.row() == &CRB1.row());
                    ASSERT(&L.row() == &CRB2.row());
                    ASSERT(&T[row]  == &CRB3.row());

                    ASSERT(&L.row() ==  &RB1.row());
                    ASSERT(&L.row() ==  &RB2.row());
                    ASSERT(&mT.theModifiableRow(row) ==  &RB3.row());

                    ASSERT(bRec == &CRB1.record());
                    ASSERT(bRec == &CRB2.record());
                    ASSERT(dRec == &CRB3.record());

                    ASSERT(bRec ==  &RB1.record());
                    ASSERT(bRec ==  &RB2.record());
                    ASSERT(dRec ==  &RB3.record());
                }
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning table bindings." << endl;
        }
        {
            if (veryVerbose) {
                T_ cout << "Having rows of an unconstrained type." << endl;
            }
            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b#ca";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *aRec = S.lookupRecord("a");

                Table mT;  const Table& T = mT;
                createTable(&mT, bRec);

                CTblBind ctb(&T, bRec);   const CTblBind& CTB = ctb;
                 TblBind  tb(&mT, bRec);  const  TblBind&  TB =  tb;

                for (int row = 0; row < T.numRows(); ++row) {
                    Table *mTx =
                               &mT.theModifiableRow(row).theModifiableTable(0);
                    const Table& Tx = *mTx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                    }

                          CTblBind  ctb1(CTB.tableBinding(row, "c"));
                          CTblBind  ctb2(CTB.tableBinding(row,  0));

                    const CTblBind& CTB1 = ctb1;
                    const CTblBind& CTB2 = ctb2;

                           TblBind   tb1(TB.tableBinding(row, "c"));
                           TblBind   tb2(TB.tableBinding(row, 0));

                    const  TblBind&  TB1 = tb1;
                    const  TblBind&  TB2 = tb2;

                    ASSERT(&Tx == &CTB1.table());
                    ASSERT(&Tx == &CTB2.table());
                    ASSERT(&Tx ==  &TB1.table());
                    ASSERT(&Tx ==  &TB2.table());

                    ASSERT(aRec == &CTB1.record());
                    ASSERT(aRec == &CTB2.record());
                    ASSERT(aRec ==  &TB1.record());
                    ASSERT(aRec ==  &TB2.record());
                }
            }

            if (veryVerbose) {
                T_ cout << "Having rows of a constrained type." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d#fb";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *dRec = S.lookupRecord("d");
                const RecDef *bRec = S.lookupRecord("b");

                Table mT;  const Table& T = mT;
                createTable(&mT, dRec);

                CTblBind ctb(&T, dRec);   const CTblBind& CTB = ctb;
                 TblBind  tb(&mT, dRec);  const  TblBind&  TB =  tb;

                for (int row = 0; row < CTB.numRows(); ++row) {
                    Table *mTx =
                               &mT.theModifiableRow(row).theModifiableTable(0);
                    const Table& Tx = *mTx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                    }
                          CTblBind  ctb1(CTB.tableBinding(row, "f"));
                          CTblBind  ctb2(CTB.tableBinding(row,  0));

                    const CTblBind& CTB1 = ctb1;
                    const CTblBind& CTB2 = ctb2;

                           TblBind   tb1(TB.tableBinding(row, "f"));
                           TblBind   tb2(TB.tableBinding(row, 0));

                    const  TblBind&  TB1 = tb1;
                    const  TblBind&  TB2 = tb2;

                    ASSERT(&Tx == &CTB1.table());
                    ASSERT(&Tx == &CTB2.table());
                    ASSERT(&Tx ==  &TB1.table());
                    ASSERT(&Tx ==  &TB2.table());

                    ASSERT(bRec == &CTB1.record());
                    ASSERT(bRec == &CTB2.record());
                    ASSERT(bRec ==  &TB1.record());
                    ASSERT(bRec ==  &TB2.record());
                }
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning column bindings." << endl;
        }
        {
            if (veryVerbose) {
                T_ cout << "Having rows of an unconstrained type." << endl;
            }
            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b#ca";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                // Not creating columns of unconstrained aggregate type as
                // column bindings to such are not allowed.

                if (ElemType::isAggregateType(getType(bdemType[i]))) {
                    continue;
                }

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *aRec = S.lookupRecord("a");
                const FldDef *aFld = &aRec->field(0);

                List mL;  createList(&mL, bRec);

                Table *mT = &mL.theModifiableTable(0);
                const Table& T = *mT;

                CTblBind ctb(mT, aRec);  const CTblBind& CTB = ctb;
                 TblBind  tb(mT, aRec);  const  TblBind&  TB =  tb;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Table: "; P(T); N_
                }

                      CColBind  ccb1(CTB.columnBinding("a"));
                      CColBind  ccb2(CTB.columnBinding(0));

                const CColBind& CCB1 = ccb1;
                const CColBind& CCB2 = ccb2;

                       ColBind   cb1(TB.columnBinding("a"));
                       ColBind   cb2(TB.columnBinding(0));

                const  ColBind&  CB1 = cb1;
                const  ColBind&  CB2 = cb2;

                ASSERT(&T == &CCB1.table());
                ASSERT(&T == &CCB2.table());
                ASSERT(&T ==  &CB1.table());
                ASSERT(&T ==  &CB2.table());

                ASSERT(0 == CCB1.columnIndex());
                ASSERT(0 == CCB2.columnIndex());
                ASSERT(0 ==  CB1.columnIndex());
                ASSERT(0 ==  CB2.columnIndex());

                ASSERT(aFld == &CCB1.field());
                ASSERT(aFld == &CCB2.field());
                ASSERT(aFld ==  &CB1.field());
                ASSERT(aFld ==  &CB2.field());
            }

            if (veryVerbose) {
                T_ cout << "Having rows of a constrained type." << endl;
            }
            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d#fb";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *dRec = S.lookupRecord("d");
                const RecDef *bRec = S.lookupRecord("b");
                const FldDef *bFld = &bRec->field(0);

                List mL;  createList(&mL, dRec);

                Table *mT = &mL.theModifiableTable(0);
                const Table& T = *mT;

                CTblBind ctb(mT, bRec);  const CTblBind& CTB = ctb;
                 TblBind  tb(mT, bRec);  const  TblBind&  TB =  tb;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Table: "; P(T); N_
                }

                      CColBind  ccb1(CTB.columnBinding("c"));
                      CColBind  ccb2(CTB.columnBinding(0));

                const CColBind& CCB1 = ccb1;
                const CColBind& CCB2 = ccb2;

                       ColBind   cb1(TB.columnBinding("c"));
                       ColBind   cb2(TB.columnBinding(0));

                const  ColBind&  CB1 = cb1;
                const  ColBind&  CB2 = cb2;

                ASSERT(&T == &CCB1.table());
                ASSERT(&T == &CCB2.table());
                ASSERT(&T ==  &CB1.table());
                ASSERT(&T ==  &CB2.table());

                ASSERT(0 == CCB1.columnIndex());
                ASSERT(0 == CCB2.columnIndex());
                ASSERT(0 ==  CB1.columnIndex());
                ASSERT(0 ==  CB2.columnIndex());

                ASSERT(bFld == &CCB1.field());
                ASSERT(bFld == &CCB2.field());
                ASSERT(bFld ==  &CB1.field());
                ASSERT(bFld ==  &CB2.field());
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice bindings." << endl;
        }

        {
            if (veryVerbose) {
                N_ cout << "Returning choices between unconstrained types."
                        << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?%a%b :b%aa";
                spec[5] = bdemType[i];  // replace '%' in 'spec'
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                // replace the second % by another type
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                Table mT;  const Table& T = mT;
                createTable(&mT, bRec);

                CTblBind ctb( &T, &S, "b");  const CTblBind& CTB = ctb;
                 TblBind  tb(&mT, &S, "b");  const  TblBind&  TB =  tb;

                for (int row = 0; row < T.numRows(); ++row) {
                    Choice *mCx =
                              &mT.theModifiableRow(row).theModifiableChoice(0);
                    const Choice& Cx = *mCx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Choice: "; P(Cx); N_
                    }

                          CCBind  ccb1(CTB.choiceBinding(row,"a"));
                          CCBind  ccb2(CTB.choiceBinding(row, 0));
                    const CCBind& CCB1 = ccb1;
                    const CCBind& CCB2 = ccb2;

                           CBind   cb1(TB.choiceBinding(row,"a"));
                           CBind   cb2(TB.choiceBinding(row,0));
                    const  CBind&  CB1 = cb1;
                    const  CBind&  CB2 = cb2;

                    ASSERT(&Cx.item() == &CCB1.item());
                    ASSERT(&Cx.item() == &CCB2.item());
                    ASSERT(&Cx.item() ==  &CB1.item());
                    ASSERT(&Cx.item() ==  &CB2.item());

                    ASSERT(aRec == &CCB1.record());
                    ASSERT(aRec == &CCB2.record());
                    ASSERT(aRec ==  &CB1.record());
                    ASSERT(aRec ==  &CB2.record());
                }
            }

            if (veryVerbose) {
                N_ cout << "Returning choices between constrained types."
                        << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a$Aa :b?$aa :c%ab";

                const char type = constrBdemType[i];
                // Replace '$' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[4] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = type;

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *cRec = S.lookupRecord("c");

                Table mT;  const Table& T = mT;
                createTable(&mT, cRec);

                CTblBind ctb( &T, &S, "c");  const CTblBind& CTB = ctb;
                 TblBind  tb(&mT, &S, "c");  const  TblBind&  TB =  tb;

                for (int row = 0; row < T.numRows(); ++row) {
                    Choice *mCx =
                              &mT.theModifiableRow(row).theModifiableChoice(0);
                    const Choice& Cx = *mCx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Choice: "; P(Cx); N_
                    }

                          CCBind  ccb1(CTB.choiceBinding(row,"a"));
                          CCBind  ccb2(CTB.choiceBinding(row, 0));
                    const CCBind& CCB1 = ccb1;
                    const CCBind& CCB2 = ccb2;

                           CBind   cb1(TB.choiceBinding(row,"a"));
                           CBind   cb2(TB.choiceBinding(row,0));
                    const  CBind&  CB1 = cb1;
                    const  CBind&  CB2 = cb2;

                    ASSERT(&Cx.item() == &CCB1.item());
                    ASSERT(&Cx.item() == &CCB2.item());
                    ASSERT(&Cx.item() ==  &CB1.item());
                    ASSERT(&Cx.item() ==  &CB2.item());

                    ASSERT(bRec == &CCB1.record());
                    ASSERT(bRec == &CCB2.record());
                    ASSERT(bRec ==  &CB1.record());
                    ASSERT(bRec ==  &CB2.record());
                }
            }

        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice array bindings." << endl;
        }

        {
            if (veryVerbose) {
                N_ cout << "Returning choices arrays between"
                        << " unconstrained types." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?%a%b :b@aa";
                spec[5] = bdemType[i];  // replace '%' in 'spec'
                // replace the second % by another type
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                Table mT;  const Table& T = mT;
                createTable(&mT, bRec);

                CTblBind ctb( &T, &S, "b");  const CTblBind& CTB = ctb;
                 TblBind  tb(&mT, &S, "b");  const TblBind&  TB =  tb;

                for (int row = 0; row < T.numRows(); ++row) {
                    ChoiceArray *mCAx =
                         &mT.theModifiableRow(row).theModifiableChoiceArray(0);
                    const ChoiceArray& CAx = *mCAx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying ChoiceArray: "; P(CAx); N_
                    }

                          CCArrBind  ccab1(CTB.choiceArrayBinding(row,"a"));
                          CCArrBind  ccab2(CTB.choiceArrayBinding(row,0));
                    const CCArrBind& CCAB1 = ccab1;
                    const CCArrBind& CCAB2 = ccab2;

                           CArrBind   cab1(TB.choiceArrayBinding(row,"a"));
                           CArrBind   cab2(TB.choiceArrayBinding(row,0));
                    const  CArrBind&  CAB1 = cab1;
                    const  CArrBind&  CAB2 = cab2;

                    ASSERT(&CAx == &CCAB1.choiceArray());
                    ASSERT(&CAx == &CCAB2.choiceArray());
                    ASSERT(&CAx ==  &CAB1.choiceArray());
                    ASSERT(&CAx ==  &CAB2.choiceArray());

                    ASSERT(aRec == &CCAB1.record());
                    ASSERT(aRec == &CCAB2.record());
                    ASSERT(aRec ==  &CAB1.record());
                    ASSERT(aRec ==  &CAB2.record());
                }
            }

            if (veryVerbose) {
                N_ cout << "Returning choice arrays between constrained"
                        << " types." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field must
                // reference a CHOICE Record
                static char spec[] = "~ :a$Aa :b?$aa :c@ab";

                const char type = constrBdemType[i];
                // Replace '$' - record 'a' type must be "choice" if
                // 'b' contains a constrained choice
                spec[4] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = type;

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *cRec = S.lookupRecord("c");

                Table mT;  const Table& T = mT;
                createTable(&mT,cRec);

                CTblBind ctb( &T, &S, "c");  const CTblBind& CTB = ctb;
                 TblBind  tb(&mT, &S, "c");  const  TblBind&  TB =  tb;

                for (int row = 0; row < T.numRows(); ++row) {
                    ChoiceArray *mCAx =
                         &mT.theModifiableRow(row).theModifiableChoiceArray(0);
                    const ChoiceArray& CAx = *mCAx;

                    if (veryVeryVerbose) {
                        N_ T2_ cout << "Underlying Choice: "; P(CAx); N_
                    }

                          CCArrBind  ccab1(CTB.choiceArrayBinding(row,"a"));
                          CCArrBind  ccab2(CTB.choiceArrayBinding(row,0));
                    const CCArrBind& CCAB1 = ccab1;
                    const CCArrBind& CCAB2 = ccab2;

                           CArrBind   cab1(TB.choiceArrayBinding(row,"a"));
                           CArrBind   cab2(TB.choiceArrayBinding(row,0));
                    const  CArrBind&  CAB1 = cab1;
                    const  CArrBind&  CAB2 = cab2;

                    ASSERT(&CAx == &CCAB1.choiceArray());
                    ASSERT(&CAx == &CCAB2.choiceArray());
                    ASSERT(&CAx ==  &CAB1.choiceArray());
                    ASSERT(&CAx ==  &CAB2.choiceArray());

                    ASSERT(bRec == &CCAB1.record());
                    ASSERT(bRec == &CCAB2.record());
                    ASSERT(bRec ==  &CAB1.record());
                    ASSERT(bRec ==  &CAB2.record());
                }
            }
        }

      }

DEFINE_TEST_CASE(22) {
        // ------------------------------------------------------------------
        // TESTING ROW BINDING xxxBINDING
        //   Test Row Binding functions that return bindings.
        //
        // Concerns:
        //   - Valid bindings are returned by the 'xxxBinding' methods.
        //   - The data and meta-data are hooked up correctly.
        //
        // Plan:
        //   - Functions returning Row Bindings
        //     Create a 'bdem_List' containing an element of an unconstrained
        //     type.  Create a second 'bdem_List' having the first 'bdem_List'
        //     as an element.  Create "const" and non-"const" row bindings from
        //     the second 'bdem_List' and call the 'rowBinding' methods to
        //     obtain new row bindings for the first 'bdem_List'.  Use the
        //     direct accessors to verify that the row and record have been
        //     installed correctly in the returned bindings.
        //
        //     Perform the same test when the first 'bdem_List' contains an
        //     element of a constrained type.
        //
        //   - Functions returning Table Bindings
        //     Create a 'bdem_Table' containing rows of an unconstrained type.
        //     Create a 'bdem_List' having the 'bdem_Table' as an element.
        //     Create "const" and non-"const" row bindings from the 'bdem_List'
        //     and call the 'tableBinding' methods to obtain new table bindings
        //     for the 'bdem_Table'.  Use the direct accessors to verify that
        //     the table and record have been installed correctly in the
        //     returned bindings.
        //
        //     Perform the same test when the 'bdem_Table' contains rows of a
        //     constrained type.
        //
        //   - Functions returning Choice Bindings
        //     Create a 'bdem_Choice' containing an element of an unconstrained
        //     type.  Create a 'bdem_List' having the first
        //     'bdem_Choice' as an element.  Create "const" and
        //     non-"const" Row bindings from
        //     the 'bdem_List' and call the 'choiceBinding' methods to
        //     obtain new choice bindings for the first 'bdem_Choice'.  Use the
        //     direct accessors to verify that the choice and record have been
        //     installed correctly in the returned bindings.
        //
        //     Perform the same test when the first 'bdem_Choice' contains an
        //     element of a constrained type.
        //   - Functions returning Choice Array Bindings
        //     Create a 'bdem_ChoiceArray' containing rows of an
        //     unconstrained type.  Create a 'bdem_List'
        //     having the 'bdem_ChoiceArray' as an element.
        //     Create "const" and non-"const" row bindings from the
        //     'bdem_Table' and call the 'choiceArrayBinding' methods to
        //     obtain new bindings for the 'bdem_ChoiceArray'.
        //     Use the direct accessors to verify that
        //     the choice array and record have been installed correctly in the
        //     returned bindings.
        //
        //     Perform the same test when the 'bdem_ChoiceArray' contains
        //     rows of a constrained type.
        //
        // Testing:
        //             bdem_ConstRowBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstRowBinding rowBinding(*name) const;
        //   bdem_ConstRowBinding rowBinding(int) const;
        //   bdem_ConstTableBinding tableBinding(*name) const;
        //   bdem_ConstTableBinding tableBinding(int) const;
        //   bdem_ConstChoiceBinding choiceBinding(*name) const;
        //   bdem_ConstChoiceBinding choiceBinding(int) const;
        //   bdem_ConstChoiceArrayBinding choiceArrayBinding(*name) const;
        //   bdem_ConstChoiceArrayBinding choiceArrayBinding(int) const;
        //             bdem_RowBinding
        //             ~~~~~~~~~~~~~~~
        //   bdem_RowBinding rowBinding(*name) const;
        //   bdem_RowBinding rowBinding(int) const;
        //   bdem_TableBinding tableBinding(*name) const;
        //   bdem_TableBinding tableBinding(int) const;
        //   bdem_ChoiceBinding choiceBinding(*name) const;
        //   bdem_ChoiceBinding choiceBinding(int) const;
        //   bdem_ChoiceArrayBinding choiceArrayBinding(*name) const;
        //   bdem_ChoiceArrayBinding choiceArrayBinding(int) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Row Binding 'XXXBinding'" << endl
                 << "================================" << endl;

        if (veryVerbose) cout << "Functions returning row bindings." << endl;
        {
            if (veryVerbose) {
                T_ cout << "Having element of an unconstrained type." << endl;
            }
            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b+ca";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *aRec = S.lookupRecord("a");

                List mL;  const List& L = mL;  createList(&mL, bRec);

                CRowBind crb( &L, &S, "b");  const CRowBind& CRB = crb;
                 RowBind  rb(&mL, &S, "b");  const  RowBind&  RB =  rb;

                List *mL2 = &mL.theModifiableList(0);
                const List& L2 = *mL2;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Row: "; P(L2); N_
                }

                      CRowBind  ckrb1(CRB.rowBinding("c"));
                      CRowBind  ckrb2(CRB.rowBinding(0));
                const CRowBind& CKRB1 = ckrb1;
                const CRowBind& CKRB2 = ckrb2;

                       RowBind   krb1(RB.rowBinding("c"));
                       RowBind   krb2(RB.rowBinding(0));
                const  RowBind&  KRB1 = krb1;
                const  RowBind&  KRB2 = krb2;

                ASSERT(&L2.row() == &CKRB1.row());
                ASSERT(&L2.row() == &CKRB2.row());
                ASSERT(&L2.row() ==  &KRB1.row());
                ASSERT(&L2.row() ==  &KRB2.row());

                ASSERT(aRec == &CKRB1.record());
                ASSERT(aRec == &CKRB2.record());
                ASSERT(aRec ==  &KRB1.record());
                ASSERT(aRec ==  &KRB2.record());
            }

            if (veryVerbose) {
                T_ cout << "Having element of a constrained type." << endl;
            }
            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d+fb";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *dRec = S.lookupRecord("d");
                const RecDef *bRec = S.lookupRecord("b");

                List mL;  const List& L = mL;  createList(&mL, dRec);

                CRowBind crb( &L, &S, "d");  const CRowBind& CRB = crb;
                 RowBind  rb(&mL, &S, "d");  const  RowBind&  RB =  rb;

                List *mL2 = &mL.theModifiableList(0);
                const List& L2 = *mL2;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Row: "; P(L2); N_
                }

                      CRowBind  ckrb1(CRB.rowBinding("f"));
                      CRowBind  ckrb2(CRB.rowBinding(0));
                const CRowBind& CKRB1 = ckrb1;
                const CRowBind& CKRB2 = ckrb2;

                       RowBind   krb1(RB.rowBinding("f"));
                       RowBind   krb2(RB.rowBinding(0));
                const  RowBind&  KRB1 = krb1;
                const  RowBind&  KRB2 = krb2;

                ASSERT(&L2.row() == &CKRB1.row());
                ASSERT(&L2.row() == &CKRB2.row());
                ASSERT(&L2.row() ==  &KRB1.row());
                ASSERT(&L2.row() ==  &KRB2.row());

                ASSERT(bRec == &CKRB1.record());
                ASSERT(bRec == &CKRB2.record());
                ASSERT(bRec ==  &KRB1.record());
                ASSERT(bRec ==  &KRB2.record());
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning table bindings." << endl;
        }
        {
            if (veryVerbose) {
                T_ cout << "Having rows of an unconstrained type." << endl;
            }
            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a%a :b#ca";
                spec[4] = bdemType[i];  // replace '%' in 'spec'

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *aRec = S.lookupRecord("a");

                List mL;  const List& L = mL;  createList(&mL, bRec);

                CRowBind crb( &L, &S, "b");  const CRowBind& CRB = crb;
                 RowBind  rb(&mL, &S, "b");  const  RowBind&  RB =  rb;

                Table *mTx = &mL.theModifiableTable(0);
                const Table& Tx = *mTx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                }

                      CTblBind  ctb1(CRB.tableBinding("c"));
                      CTblBind  ctb2(CRB.tableBinding(0));
                const CTblBind& CTB1 = ctb1;
                const CTblBind& CTB2 = ctb2;

                       TblBind   tb1(RB.tableBinding("c"));
                       TblBind   tb2(RB.tableBinding(0));
                const  TblBind&  TB1 = tb1;
                const  TblBind&  TB2 = tb2;

                ASSERT(&Tx == &CTB1.table());
                ASSERT(&Tx == &CTB2.table());
                ASSERT(&Tx ==  &TB1.table());
                ASSERT(&Tx ==  &TB2.table());

                ASSERT(aRec == &CTB1.record());
                ASSERT(aRec == &CTB2.record());
                ASSERT(aRec ==  &TB1.record());
                ASSERT(aRec ==  &TB2.record());
            }

            if (veryVerbose) {
                T_ cout << "Having rows of a constrained type." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a %Aa :b%ca :d#fb";
                const char type = constrBdemType[i];
                // Replace '%' - record 'a' type must be "choice" if
                // 'b' contains a constrained choice
                spec[5] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = constrBdemType[i];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *dRec = S.lookupRecord("d");
                const RecDef *bRec = S.lookupRecord("b");

                List mL;  const List& L = mL;  createList(&mL, dRec);

                CRowBind crb( &L, &S, "d");  const CRowBind& CRB = crb;
                 RowBind  rb(&mL, &S, "d");  const  RowBind&  RB =  rb;

                Table *mTx = &mL.theModifiableTable(0);
                const Table& Tx = *mTx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Table: "; P(Tx); N_
                }

                      CTblBind  ctb1(CRB.tableBinding("f"));
                      CTblBind  ctb2(CRB.tableBinding(0));
                const CTblBind& CTB1 = ctb1;
                const CTblBind& CTB2 = ctb2;

                       TblBind   tb1(RB.tableBinding("f"));
                       TblBind   tb2(RB.tableBinding(0));
                const  TblBind&  TB1 = tb1;
                const  TblBind&  TB2 = tb2;

                ASSERT(&Tx == &CTB1.table());
                ASSERT(&Tx == &CTB2.table());
                ASSERT(&Tx ==  &TB1.table());
                ASSERT(&Tx ==  &TB2.table());

                ASSERT(bRec == &CTB1.record());
                ASSERT(bRec == &CTB2.record());
                ASSERT(bRec ==  &TB1.record());
                ASSERT(bRec ==  &TB2.record());
            }
        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice bindings." << endl;
        }

        {
            if (veryVerbose) {
                N_ cout << "Returning choices between unconstrained "
                        << "types." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?%a%b :b%aa";
                spec[5] = bdemType[i];  // replace '%' in 'spec'
                // replace the second % by another type
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                List mL;  const List& L = mL;  createList(&mL, bRec);

                CRowBind crb( &L, &S, "b");  const CRowBind& CRB = crb;
                 RowBind  rb(&mL, &S, "b");  const  RowBind&  RB =  rb;

                Choice *mCx = &mL.theModifiableChoice(0);
                const Choice& Cx = *mCx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Choice: "; P(Cx); N_
                }

                      CCBind  ccb1(CRB.choiceBinding("a"));
                      CCBind  ccb2(CRB.choiceBinding(0));
                const CCBind& CCB1 = ccb1;
                const CCBind& CCB2 = ccb2;

                       CBind   cb1(RB.choiceBinding("a"));
                       CBind   cb2(RB.choiceBinding(0));
                const  CBind&  CB1 = cb1;
                const  CBind&  CB2 = cb2;

                ASSERT(&Cx.item() == &CCB1.item());
                ASSERT(&Cx.item() == &CCB2.item());
                ASSERT(&Cx.item() ==  &CB1.item());
                ASSERT(&Cx.item() ==  &CB2.item());

                ASSERT(aRec == &CCB1.record());
                ASSERT(aRec == &CCB2.record());
                ASSERT(aRec ==  &CB1.record());
                ASSERT(aRec ==  &CB2.record());
            }

            if (veryVerbose) {
                N_ cout << "Returning choices between constrained "
                        << "types." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED
                // CHOICE Field must reference a CHOICE Record
                static char spec[] = "~ :a$Aa :b?$aa :c%ab";

                const char type = constrBdemType[i];
                // Replace '$' - record 'a' type must be "choice" if
                // 'b' contains a constrained choice
                spec[4] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = type;

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *cRec = S.lookupRecord("c");

                List mL;  const List& L = mL;  createList(&mL, cRec);

                CRowBind crb( &L, &S, "c");  const CRowBind& CRB = crb;
                 RowBind  rb(&mL, &S, "c");  const  RowBind&  RB =  rb;

                Choice *mCx = &mL.theModifiableChoice(0);
                const Choice& Cx = *mCx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Choice: "; P(Cx); N_
                }

                      CCBind  ccb1(CRB.choiceBinding("a"));
                      CCBind  ccb2(CRB.choiceBinding(0));
                const CCBind& CCB1 = ccb1;
                const CCBind& CCB2 = ccb2;

                       CBind   cb1(RB.choiceBinding("a"));
                       CBind   cb2(RB.choiceBinding(0));
                const  CBind&  CB1 = cb1;
                const  CBind&  CB2 = cb2;

                ASSERT(&Cx.item() == &CCB1.item());
                ASSERT(&Cx.item() == &CCB2.item());
                ASSERT(&Cx.item() ==  &CB1.item());
                ASSERT(&Cx.item() ==  &CB2.item());

                ASSERT(bRec == &CCB1.record());
                ASSERT(bRec == &CCB2.record());
                ASSERT(bRec ==  &CB1.record());
                ASSERT(bRec ==  &CB2.record());
            }

        }

        if (veryVerbose) {
            N_ cout << "Functions returning choice array bindings." << endl;
        }

        {
            if (veryVerbose) {
                 N_ cout << "Returning choices arrays between "
                         << "unconstrained types." << endl;
            }

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :a?%a%b :b@aa";
                spec[5] = bdemType[i];  // replace '%' in 'spec'
                // replace the second % by another type
                spec[7] = bdemType[(i+2)  % NUM_TYPES];
                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *aRec = S.lookupRecord("a");
                const RecDef *bRec = S.lookupRecord("b");

                List mL;  const List& L = mL;  createList(&mL, bRec);

                CRowBind crb( &L, &S, "b");  const CRowBind& CRB = crb;
                 RowBind  rb(&mL, &S, "b");  const  RowBind&  RB =  rb;

                ChoiceArray *mCAx = &mL.theModifiableChoiceArray(0);
                const ChoiceArray& CAx = *mCAx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying ChoiceArray: "; P(CAx); N_
                }

                      CCArrBind  ccab1(CRB.choiceArrayBinding("a"));
                      CCArrBind  ccab2(CRB.choiceArrayBinding(0));
                const CCArrBind& CCAB1 = ccab1;
                const CCArrBind& CCAB2 = ccab2;

                       CArrBind   cab1(RB.choiceArrayBinding("a"));
                       CArrBind   cab2(RB.choiceArrayBinding(0));
                const  CArrBind&  CAB1 = cab1;
                const  CArrBind&  CAB2 = cab2;

                ASSERT(&CAx == &CCAB1.choiceArray());
                ASSERT(&CAx == &CCAB2.choiceArray());
                ASSERT(&CAx ==  &CAB1.choiceArray());
                ASSERT(&CAx ==  &CAB2.choiceArray());

                ASSERT(aRec == &CCAB1.record());
                ASSERT(aRec == &CCAB2.record());
                ASSERT(aRec ==  &CAB1.record());
                ASSERT(aRec ==  &CAB2.record());
            }

            if (veryVerbose) {
                N_ cout << "Returning choice arrays between "
                        << "constrained types." << endl;
            }

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                // need 2 wildcards (%), since a CONSTRAINED CHOICE Field
                // must reference a CHOICE Record
                static char spec[] = "~ :a$Aa :b?$aa :c@ab";

                const char type = constrBdemType[i];
                // Replace '$' - record 'a' type must be "choice" if 'b'
                // contains a constrained choice
                spec[4] = (type == '%' || type == '@') ? '?' : ' ';
                spec[11] = type;

                Schema mS;  const Schema& S = mS;  createSchema(&mS, spec);

                const RecDef *bRec = S.lookupRecord("b");
                const RecDef *cRec = S.lookupRecord("c");

                List mL;  const List& L = mL;  createList(&mL, cRec);

                CRowBind crb( &L, &S, "c");  const CRowBind& CRB = crb;
                 RowBind  rb(&mL, &S, "c");  const  RowBind&  RB =  rb;

                ChoiceArray *mCAx = &mL.theModifiableChoiceArray(0);
                const ChoiceArray& CAx = *mCAx;

                if (veryVeryVerbose) {
                    N_ T2_ cout << "Underlying Choice: "; P(CAx); N_
                }

                      CCArrBind  ccab1(CRB.choiceArrayBinding("a"));
                      CCArrBind  ccab2(CRB.choiceArrayBinding(0));
                const CCArrBind& CCAB1 = ccab1;
                const CCArrBind& CCAB2 = ccab2;

                       CArrBind   cab1(RB.choiceArrayBinding("a"));
                       CArrBind   cab2(RB.choiceArrayBinding(0));
                const  CArrBind&  CAB1 = cab1;
                const  CArrBind&  CAB2 = cab2;

                ASSERT(&CAx == &CCAB1.choiceArray());
                ASSERT(&CAx == &CCAB2.choiceArray());
                ASSERT(&CAx ==  &CAB1.choiceArray());
                ASSERT(&CAx ==  &CAB2.choiceArray());

                ASSERT(bRec == &CCAB1.record());
                ASSERT(bRec == &CCAB2.record());
                ASSERT(bRec ==  &CAB1.record());
                ASSERT(bRec ==  &CAB2.record());
            }

        }

      }

DEFINE_TEST_CASE(21) {
        // --------------------------------------------------------------
        // TESTING EQUALITY OPERATORS (CHOICE ARRAY BINDINGS)
        //   Test 'operator==' and 'operator!=' for CHOICE ARRAY Bindings.
        //
        // Concerns:
        //   The operators return correct results.
        //
        // Plan:
        //   The operators are tested for the following conditions:
        //   - Comparison to self (X == X).
        //   - The two bindings being compared store the same
        //     'bdem_ChoiceArray' pointer and the same 'bdem_RecordDef'.
        //   - The two bindings being compared store different
        //     'bdem_ChoiceArray' pointers and the same 'bdem_RecordDef',
        //     with the two choice arrays having identical data.
        //   - The two bindings being compared store different
        //     'bdem_ChoiceArray' pointers and the same 'bdem_RecordDef',
        //     with the two choice arrays having different data.
        //   - The two bindings being compared store the same 'bdem_Choice'
        //     pointer, but different 'bdem_RecordDef's.
        //
        //   To check these conditions we:
        //   1) Create a 'bdem_ChoiceArray' and a 'bdem_RecordDef' and
        //      initialize a choice array binding from them and test
        //      the operators for comparison to self.  'operator==' should
        //      return 'true' and 'operator!=' should return 'false'.
        //   2) Create a 'bdem_ChoiceArray' and a 'bdem_RecordDef' and
        //      initialize two choice array bindings
        //      with that 'bdem_ChoiceArray' and
        //      'bdem_RecordDef'.  On comparing the two bindings,
        //      'operator==' should return 'true' and 'operator!=' should
        //      return 'false'.
        //   3) Create two 'bdem_ChoiceArray's having the *same* value
        //      and the same 'bdem_RecordDef', and use them to initialize
        //      two choice array bindings.  The 'bdem_RecordDef' used for
        //      creating the choice array bindings is the same
        //      'bdem_RecordDef'.  On comparing the two bindings,
        //      'operator==' should return 'true' and 'operator!=' should
        //      return 'false'.
        //   4) Create two 'bdem_ChoiceArray's having *different* values, but
        //      satisfying the same 'bdem_RecordDef', and use them to
        //      initialize two choice array bindings.  The 'bdem_RecordDef'
        //      used for creating the choice bindings is the same
        //      'bdem_RecordDef'.  On comparing the two bindings,
        //      'operator==' should return 'false' and 'operator!=' should
        //      return 'true'.
        //   5) Create two choice bindings using the same 'bdem_ChoiceArray',
        //      but different 'bdem_RecordDef' instances (having the
        //      same value).  On comparing the two choice bindings,
        //      'operator==' should return 'true' and 'operator!=' should
        //      return 'false'.
        //
        // Testing:
        //   bool operator==(const ConstChoiceArrayBinding& lhs, const& rhs);
        //   bool operator!=(const ConstChoiceArrayBinding& lhs, const& rhs);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Choice Array Equality Operators" << endl
                 << "======================================" << endl;

        {
            // ---------------------- TEST DATA ---------------------------

            Schema S;  createSchema(&S, ":a?AaBbCcDd");

            const RecDef *rec = &S.record(0);

            ChoiceArray mCA;  const ChoiceArray& CA = mCA;
            createChoiceArray(&mCA, rec);

            if (veryVerbose) cout << "Checking self-comparison." << endl;
            {
                 CCArrBind  ccab(&CA, rec);   const  CCArrBind&  CCAB =  ccab;
                  CArrBind   cab(&mCA, rec);  const   CArrBind&   CAB =   cab;

                ASSERT(   CCAB ==  CCAB);
                ASSERT(    CAB ==   CAB);

                ASSERT(!( CCAB !=  CCAB));
                ASSERT(!(  CAB !=   CAB));
            }

            if (veryVerbose) {
                cout << "With same 'choice Array' and same 'record'." << endl;
            }
            {
                 CCArrBind  ccab1(&CA, rec); const  CCArrBind&  CCAB1 =  ccab1;
                 CCArrBind  ccab2(&CA, rec); const  CCArrBind&  CCAB2 =  ccab2;

                 CArrBind   cab1(&mCA, rec);  const CArrBind&   CAB1 =   cab1;
                 CArrBind   cab2(&mCA, rec);  const CArrBind&   CAB2 =   cab2;

                ASSERT(   CCAB1 ==  CCAB2);
                ASSERT(    CAB1 ==   CAB2);

                ASSERT(!( CCAB1 !=  CCAB2));
                ASSERT(!(  CAB1 !=   CAB2));
            }

            if (veryVerbose) {
                cout << "With different 'choice array's, having"
                        " same *data* and same 'record'." << endl;
            }
            {
                ChoiceArray mCA2 = CA; const ChoiceArray& CA2 = mCA2;

                CCArrBind  ccab1(&CA, rec);  const  CCArrBind&  CCAB1 =  ccab1;
                CCArrBind  ccab2(&CA2, rec); const  CCArrBind&  CCAB2 =  ccab2;

                CArrBind   cab1(&mCA, rec);  const   CArrBind&   CAB1 =   cab1;
                CArrBind   cab2(&mCA2, rec); const   CArrBind&   CAB2 =   cab2;

                ASSERT(   CCAB1 ==  CCAB2);
                ASSERT(    CAB1 ==   CAB2);

                ASSERT(!( CCAB1 !=  CCAB2));
                ASSERT(!(  CAB1 !=   CAB2));
            }

            if (veryVerbose) {
                cout << "With different 'choice array's, having"
                        " different *data* and same 'record'." << endl;
            }
            {
                ChoiceArray mCA2 = CA; const ChoiceArray& CA2 = mCA2;

                CCArrBind  ccab1(&CA, rec);  const  CCArrBind&  CCAB1 =  ccab1;
                CArrBind   cab1(&mCA, rec);  const   CArrBind&   CAB1 =   cab1;

                createChoiceArray(&mCA2, rec);
                // append data to the second list

                ASSERT(CA != CA2);

                CCArrBind  ccab2(&CA2, rec); const  CCArrBind&  CCAB2 =  ccab2;
                CArrBind   cab2(&mCA2, rec); const   CArrBind&   CAB2 =   cab2;

                ASSERT(!( CCAB1 ==  CCAB2));
                ASSERT(!(  CAB1 ==   CAB2));

                ASSERT(   CCAB1 !=  CCAB2);
                ASSERT(    CAB1 !=   CAB2);
            }

            if (veryVerbose) {
                cout << "With same 'choice array', different 'record's."
                     << endl;
            }
            {
                Schema S2(S);
                const RecDef *rec2 = &S2.record(0);

                ASSERT(rec != rec2);

                CCArrBind  ccab1(&CA, rec);  const  CCArrBind&  CCAB1 =  ccab1;
                CCArrBind  ccab2(&CA, rec2); const  CCArrBind&  CCAB2 =  ccab2;

                CArrBind   cab1(&mCA, rec);  const  CArrBind&   CAB1 =   cab1;
                CArrBind   cab2(&mCA, rec2); const  CArrBind&   CAB2 =   cab2;

                ASSERT(   CCAB1 ==  CCAB2);
                ASSERT(    CAB1 ==   CAB2);

                ASSERT(!( CCAB1 !=  CCAB2));
                ASSERT(!(  CAB1 !=   CAB2));
            }
        }
      }

DEFINE_TEST_CASE(20) {
        // --------------------------------------------------------------
        // TESTING EQUALITY OPERATORS (CHOICE BINDINGS)
        //   Test 'operator==' and 'operator!=' for CHOICE Bindings.
        //
        // Concerns:
        //   The operators return correct results.
        //
        // Plan:
        //   The operators are tested for the following conditions:
        //   - Comparison to self (X == X).
        //   - The two bindings being compared store the same 'bdem_Choice'
        //     pointer and the same 'bdem_RecordDef'.
        //   - The two bindings being compared store different 'bdem_Choice'
        //     pointers and the same 'bdem_RecordDef', with the two choices
        //     having identical data.
        //   - The two bindings being compared store different 'bdem_Choice'
        //     pointers and the same 'bdem_RecordDef', with the two choices
        //     having different data.
        //   - The two bindings being compared store the same 'bdem_Choice'
        //     pointer, but different 'bdem_RecordDef's.
        //
        //   To check these conditions we:
        //   1) Create a 'bdem_Choice' and a 'bdem_RecordDef' and initialize
        //      a choice binding from them and test the operators for
        //      comparison to self.  'operator==' should return 'true' and
        //      'operator!=' should return 'false'.
        //   2) Create a 'bdem_Choice' and a 'bdem_RecordDef' and initialize
        //      two choice bindings with that 'bdem_Choice' and
        //      'bdem_RecordDef'.  On comparing the two bindings,
        //      'operator==' should return 'true' and 'operator!=' should
        //      return 'false'.
        //   3) Create two 'bdem_Choice's having the *same* value and the same
        //      'bdem_RecordDef', and use them to initialize two choice
        //      bindings.  The 'bdem_RecordDef' used for creating the
        //      choice bindings is the same 'bdem_RecordDef'.
        //      On comparing the two bindings,
        //      'operator==' should return 'true' and 'operator!=' should
        //      return 'false'.
        //   4) Create two 'bdem_Choice's having *different* values, but
        //      satisfying the same 'bdem_RecordDef', and use them to
        //      initialize two choice bindings.  The 'bdem_RecordDef' used for
        //      creating the choice bindings is the same 'bdem_RecordDef'.  On
        //      comparing the two bindings, 'operator==' should return 'false'
        //      and 'operator!=' should return 'true'.
        //   5) Create two row bindings using the same 'bdem_Choice', but
        //      different 'bdem_RecordDef' instances (having the same value).
        //      On comparing the two row bindings, 'operator==' should return
        //      'true' and 'operator!=' should return 'false'.
        //
        // Testing:
        //   bool operator==(const ConstBinding& lhs, const& rhs);
        //   bool operator!=(const ConstBinding& lhs, const& rhs);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Choice Equality Operators" << endl
                 << "======================================" << endl;

        {
            // ---------------------- TEST DATA ---------------------------

            Schema S;  createSchema(&S, ":a?AaBbCcDd");

            const RecDef *rec = &S.record(0);

            Choice mC;  const Choice& C = mC;  createChoice(&mC, rec);

            if (veryVerbose) cout << "Checking self-comparison." << endl;
            {
                 CCBind  ccb(&C, rec);   const  CCBind&  CCB =  ccb;
                  CBind   cb(&mC, rec);  const   CBind&   CB =   cb;

                ASSERT(   CCB ==  CCB);
                ASSERT(    CB ==   CB);

                ASSERT(!( CCB !=  CCB));
                ASSERT(!(  CB !=   CB));
            }

            if (veryVerbose) {
                cout << "With same 'choice' and same 'record'." << endl;
            }
            {
                 CCBind  ccb1(&C, rec);   const  CCBind&  CCB1 =  ccb1;
                 CCBind  ccb2(&C, rec);   const  CCBind&  CCB2 =  ccb2;

                  CBind   cb1(&mC, rec);  const   CBind&   CB1 =   cb1;
                  CBind   cb2(&mC, rec);  const   CBind&   CB2 =   cb2;

                ASSERT(   CCB1 ==  CCB2);
                ASSERT(    CB1 ==   CB2);

                ASSERT(!( CCB1 !=  CCB2));
                ASSERT(!(  CB1 !=   CB2));
            }

            if (veryVerbose) {
                cout << "With different 'choice's, having"
                        " same *data* and same 'record'." << endl;
            }
            {
                Choice mC2 = C; const Choice& C2 = mC2;

                 CCBind  ccb1(&C, rec);    const  CCBind&  CCB1 =  ccb1;
                 CCBind  ccb2(&C2, rec);   const  CCBind&  CCB2 =  ccb2;

                  CBind   cb1(&mC, rec);   const   CBind&   CB1 =   cb1;
                  CBind   cb2(&mC2, rec);  const   CBind&   CB2 =   cb2;

                ASSERT(   CCB1 ==  CCB2);
                ASSERT(    CB1 ==   CB2);

                ASSERT(!( CCB1 !=  CCB2));
                ASSERT(!(  CB1 !=   CB2));
            }

            if (veryVerbose) {
                cout << "With different 'choice's, having"
                        " different *data* and same 'record'." << endl;
            }
            {
                Choice mC;  const Choice& C = mC;  createChoice(&mC, rec);
                mC.makeSelection(0);     mC.theModifiableChar() = 'a';

                Choice mC2 = C; const Choice& C2 = mC2;

                 CCBind  ccb1(&C, rec);    const  CCBind&  CCB1 =  ccb1;
                  CBind   cb1(&mC, rec);   const   CBind&   CB1 =   cb1;

                // assure we actually have different data
                mC2.theModifiableChar() = 'b';

                ASSERT(C != C2);

                 CCBind  ccb2(&C2, rec);   const  CCBind&  CCB2 =  ccb2;
                  CBind   cb2(&mC2, rec);  const   CBind&   CB2 =   cb2;

                ASSERT(!( CCB1 ==  CCB2));
                ASSERT(!(  CB1 ==   CB2));

                ASSERT(   CCB1 !=  CCB2);
                ASSERT(    CB1 !=   CB2);
            }

            if (veryVerbose) {
                cout << "With same 'choice', different 'record's." << endl;
            }
            {
                Schema S2(S);
                const RecDef *rec2 = &S2.record(0);

                ASSERT(rec != rec2);

                 CCBind  ccb1(&C, rec);    const  CCBind&  CCB1 =  ccb1;
                 CCBind  ccb2(&C, rec2);   const  CCBind&  CCB2 =  ccb2;

                  CBind   cb1(&mC, rec);   const   CBind&   CB1 =   cb1;
                  CBind   cb2(&mC, rec2);  const   CBind&   CB2 =   cb2;

                ASSERT(   CCB1 ==  CCB2);
                ASSERT(    CB1 ==   CB2);

                ASSERT(!( CCB1 !=  CCB2));
                ASSERT(!(  CB1 !=   CB2));
            }
        }
      }

DEFINE_TEST_CASE(19) {
        // --------------------------------------------------------------
        // TESTING EQUALITY OPERATORS (COLUMN BINDINGS)
        //   Test 'operator==' and 'operator!=' for Column Bindings.
        //
        // Concerns:
        //   The operators return correct results.
        //
        // Plan:
        //   The operators are tested for the following conditions:
        //   - Comparison to self (X == X).
        //   - The two bindings being compared store the same 'bdem_Table'
        //     pointer, column index, and 'bdem_FieldDef'.
        //   - The two bindings being compared store different 'bdem_Table'
        //     pointers and the same 'bdem_FieldDef', where the two columns
        //     bound by the bindings have identical data.
        //   - The two bindings being compared store different 'bdem_Table'
        //     pointers and the same 'bdem_FieldDef', where the two columns
        //     bound by the bindings have different data.
        //   - The two bindings being compared store the same 'bdem_Table'
        //     pointer and column index, but different 'bdem_FieldDef'
        //     instances.
        //
        //   To check these conditions we:
        //   1) Create a 'bdem_Table' and a 'bdem_FieldDef', initialize a
        //      column binding from them and test the operators for comparison
        //      to self.  'operator==' should return 'true' and 'operator!='
        //      should return 'false'.
        //   2) Create a 'bdem_Table' and a 'bdem_FieldDef' and initialize two
        //      column bindings with that 'bdem_Table', using the same column
        //      index and 'bdem_FieldDef'.  On comparing the two bindings,
        //      'operator==' should return 'true' and 'operator!=' should
        //      return 'false'.
        //   3) Create two 'bdem_Table' instances having the same value for
        //      some column 'x' and the same 'bdem_FieldDef', and use them to
        //      initialize two column bindings binding column 'x'.  On
        //      comparing the two bindings, 'operator==' should return 'true'
        //      and 'operator!=' should return 'false'.
        //   4) Create two 'bdem_Table' instances having different data for
        //      some column 'x' and satisfying the same 'bdem_FieldDef', and
        //      use them to initialize two column bindings using column 'x'.
        //      On comparing the two bindings, 'operator==' should return
        //      'false' and 'operator!=' should return 'true'.
        //   5) Create two column bindings using the same 'bdem_Table' and the
        //      same column index, but different 'bdem_FieldDef' instances
        //      (having the same value).  On comparing the two column
        //      bindings, 'operator==' should return 'true' and 'operator!='
        //      should return 'false'.
        //
        // Testing:
        //   bool operator==(const ConstColumnBinding& lhs, const& rhs);
        //   bool operator!=(const ConstColumnBinding& lhs, const& rhs);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Column Binding Equality Operators" << endl
                 << "=========================================" << endl;

        {
            // ---------------------- TEST DATA ---------------------------
            Schema S;  createSchema(&S, ":aAa Bb Cc Dd :b#ba");

            const FldDef *F       = &S.record(0).field(0);
            const RecDef *listRec = &S.record(1);
            const int     col     = 0;

            List mL;  createList(&mL, listRec);

            Table *mT = &mL.theModifiableTable(0);
            const Table *T = mT;

            if (veryVerbose) cout << "Checking for self-comparison." << endl;
            {
                 CColBind  ccb(T, col, F);   const  CColBind&  CCB =  ccb;

                  ColBind   cb(mT, col, F);  const   ColBind&   CB =   cb;

                ASSERT(   CCB ==  CCB);
                ASSERT(    CB ==   CB);

                ASSERT(!( CCB !=  CCB));
                ASSERT(!(  CB !=   CB));
            }

            if (veryVerbose) {
                cout << "With same 'table', column Index, and 'field'."
                     << endl;
            }
            {
                 CColBind  ccb1(T, col, F);   const  CColBind&  CCB1 =  ccb1;
                 CColBind  ccb2(T, col, F);   const  CColBind&  CCB2 =  ccb2;

                  ColBind   cb1(mT, col, F);  const   ColBind&   CB1 =   cb1;
                  ColBind   cb2(mT, col, F);  const   ColBind&   CB2 =   cb2;

                ASSERT(   CCB1 ==  CCB2);
                ASSERT(    CB1 ==   CB2);

                ASSERT(!( CCB1 !=  CCB2));
                ASSERT(!(  CB1 !=   CB2));
            }

            if (veryVerbose) {
                cout << "With different 'table', same column"
                        " data and same 'field'." << endl;
            }
            {
                // Create a second table that has the same value as T for one
                // column but is totally different from the first table
                // otherwise.

                Schema S2;  createSchema(&S2, ":aFaCbAc :b#ba");

                const int col2 = 2;  // column index of elem type CHAR

                // Create the new table
                const bdem_ElemType::Type colTypes[] = {
                    bdem_ElemType::BDEM_DOUBLE,
                    bdem_ElemType::BDEM_INT,
                    bdem_ElemType::BDEM_CHAR
                };
                const int numCols = sizeof(colTypes) / sizeof(*colTypes);

                Table mT2(colTypes, numCols);  const Table& T2 = mT2;
                mT2.appendNullRows(T->numRows());

                for (int i = 0; i < T->numRows(); ++i) {
                    mT2.theModifiableRow(i)[col2].replaceValue((*T)[i][col]);
                }

                 CColBind  ccb1(T,   col,  F);  const  CColBind&  CCB1 =  ccb1;
                 CColBind  ccb2(&T2, col2, F);  const  CColBind&  CCB2 =  ccb2;

                  ColBind   cb1(mT,   col,  F); const   ColBind&   CB1 =   cb1;
                  ColBind   cb2(&mT2, col2, F); const   ColBind&   CB2 =   cb2;

                ASSERT(   CCB1 ==  CCB2);
                ASSERT(    CB1 ==   CB2);

                ASSERT(!( CCB1 !=  CCB2));
                ASSERT(!(  CB1 !=   CB2));
            }

            if (veryVerbose) {
                cout << "With different 'table's, different"
                        " column *data* and same 'field'." << endl;
            }
            {
                Table mT2 = *T;  const Table& T2 = mT2;
                mT2.appendNullRow();

                ASSERT(*T != T2);

                 CColBind  ccb1(T, col, F);     const  CColBind&  CCB1 =  ccb1;
                 CColBind  ccb2(&T2, col, F);   const  CColBind&  CCB2 =  ccb2;

                  ColBind   cb1(mT, col, F);    const   ColBind&   CB1 =   cb1;
                  ColBind   cb2(&mT2, col, F);  const   ColBind&   CB2 =   cb2;

                ASSERT(!( CCB1 ==  CCB2));
                ASSERT(!(  CB1 ==   CB2));

                ASSERT(   CCB1 !=  CCB2);
                ASSERT(    CB1 !=   CB2);
            }

            if (veryVerbose) {
                cout << "With same 'table' and column index"
                        " and different 'field's." << endl;
            }
            {
                Schema S2(S);
                const FldDef *F2 = &S2.record(0).field(0);

                ASSERT(F != F2);

                 CColBind  ccb1(T, col, F);    const  CColBind&  CCB1 =  ccb1;
                 CColBind  ccb2(T, col, F2);   const  CColBind&  CCB2 =  ccb2;

                  ColBind   cb1(mT, col, F);   const   ColBind&   CB1 =   cb1;
                  ColBind   cb2(mT, col, F2);  const   ColBind&   CB2 =   cb2;

                ASSERT(   CCB1 ==  CCB2);
                ASSERT(    CB1 ==   CB2);

                ASSERT(!( CCB1 !=  CCB2));
                ASSERT(!(  CB1 !=   CB2));
            }
        }
      }

DEFINE_TEST_CASE(18) {
        // --------------------------------------------------------------
        // TESTING EQUALITY OPERATORS (TABLE BINDINGS)
        //   Test 'operator==' and 'operator!=' for Table Bindings.
        //
        // Concerns:
        //   The operators return correct results.
        //
        // Plan:
        //   The operators are tested for the following conditions:
        //   - Comparison to self (X == X).
        //   - The two bindings being compared store the same 'bdem_Table'
        //     pointer and the same 'bdem_RecordDef'.
        //   - The two bindings being compared store different 'bdem_Table'
        //     pointers and the same 'bdem_RecordDef', with the two
        //     'bdem_Table's having identical data.
        //   - The two bindings being compared store different 'bdem_Table'
        //     pointers and the same 'bdem_RecordDef', with the two
        //     'bdem_Table's having different data.
        //   - The two bindings being compared store the same 'bdem_Table'
        //     pointer but different 'bdem_RecordDef's.
        //
        //   To check these conditions we:
        //   1)  Create a 'bdem_Table' and a 'bdem_RecordDef', initialize a
        //      table binding from them and test the operators for comparison
        //      to self.  'operator==' should return 'true' and 'operator!='
        //      should return 'false'.
        //   2) Create a 'bdem_Table' and a 'bdem_RecordDef' and initialize two
        //      table bindings with that 'bdem_Table' and 'bdem_RecordDef'.  On
        //      comparing the two bindings, 'operator==' should return 'true'
        //      and 'operator!=' should return 'false'.
        //   3) Create two 'bdem_Table' instances having the same value and the
        //      same 'bdem_RecordDef' definition, and use them to initialize
        //      two table bindings.  The 'bdem_RecordDef' used for creating the
        //      table bindings is the same 'bdem_RecordDef'.  On comparing the
        //      two bindings, 'operator==' should return 'true' and
        //      'operator!=' should return 'false'.
        //   4) Create two 'bdem_Table' instances having different values and
        //      the same 'bdem_RecordDef' definition, and use them to
        //      initialize two table bindings.  The 'bdem_RecordDef' used for
        //      creating the table bindings is the same 'bdem_RecordDef'.  On
        //      comparing the two bindings, 'operator==' should return 'false'
        //      and 'operator!=' should return 'true'.
        //   5) Create two table bindings using the same 'bdem_Table', but
        //      different 'bdem_RecordDef' instances (having the same value).
        //      On comparing the two table bindings 'operator==' should return
        //      'true' and 'operator!=' should return 'false'.
        //
        // Testing:
        //   bool operator==(const ConstTableBinding& lhs, const& rhs);
        //   bool operator!=(const ConstTableBinding& lhs, const& rhs);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Table Binding Equality Operators" << endl
                 << "========================================" << endl;

        {
            // ---------------------- TEST DATA ---------------------------

            Schema S;  createSchema(&S, ":aAaBbCcDd :b#ba");

            const RecDef *rec     = &S.record(0);
            const RecDef *listRec = &S.record(1);

            List mL;  createList(&mL, listRec);

            Table *mT = &mL.theModifiableTable(0); const Table *T = mT;

            if (veryVerbose) {
                cout << "Checking for self-comparison." << endl;
            }
            {
                 CTblBind  ctb(T, rec);   const  CTblBind&  CTB =  ctb;

                  TblBind   tb(mT, rec);  const   TblBind&   TB =   tb;

                ASSERT(   CTB ==  CTB);
                ASSERT(    TB ==   TB);

                ASSERT(!( CTB !=  CTB));
                ASSERT(!(  TB !=   TB));
            }

            if (veryVerbose) {
                cout << "With same 'table' and same 'record'." << endl;
            }
            {
                 CTblBind  ctb1(T, rec);   const  CTblBind&  CTB1 =  ctb1;
                 CTblBind  ctb2(T, rec);   const  CTblBind&  CTB2 =  ctb2;

                  TblBind   tb1(mT, rec);  const   TblBind&   TB1 =   tb1;
                  TblBind   tb2(mT, rec);  const   TblBind&   TB2 =   tb2;

                ASSERT(   CTB1 ==  CTB2);
                ASSERT(    TB1 ==   TB2);

                ASSERT(!( CTB1 !=  CTB2));
                ASSERT(!(  TB1 !=   TB2));
            }

            if (veryVerbose) {
                cout << "With different 'table's, having"
                        " same *data* and same 'record'." << endl;
            }
            {
                Table mT2 = *T;  const Table& T2 = mT2;

                 CTblBind  ctb1(T, rec);     const  CTblBind&  CTB1 =  ctb1;
                 CTblBind  ctb2(&T2, rec);   const  CTblBind&  CTB2 =  ctb2;

                  TblBind   tb1(mT, rec);    const   TblBind&   TB1 =   tb1;
                  TblBind   tb2(&mT2, rec);  const   TblBind&   TB2 =   tb2;

                ASSERT(   CTB1 ==  CTB2);
                ASSERT(    TB1 ==   TB2);

                ASSERT(!( CTB1 !=  CTB2));
                ASSERT(!(  TB1 !=   TB2));
            }

            if (veryVerbose) {
                cout << "With different 'table's, having"
                        " different *data* and same 'record'." << endl;
            }
            {
                Table mT2 = *T;  const Table& T2 = mT2;

                mT2.appendNullRow();

                ASSERT(*T != T2);

                 CTblBind  ctb1(T, rec);     const  CTblBind&  CTB1 =  ctb1;
                 CTblBind  ctb2(&T2, rec);   const  CTblBind&  CTB2 =  ctb2;

                  TblBind   tb1(mT, rec);    const   TblBind&   TB1 =   tb1;
                  TblBind   tb2(&mT2, rec);  const   TblBind&   TB2 =   tb2;

                ASSERT(!( CTB1 ==  CTB2));
                ASSERT(!(  TB1 ==   TB2));

                ASSERT(   CTB1 !=  CTB2);
                ASSERT(    TB1 !=   TB2);
            }

            if (veryVerbose) {
                cout << "With same 'table', different 'record's." << endl;
            }
            {
                Schema        S2(S);
                const RecDef *rec2 = &S2.record(0);

                ASSERT(rec != rec2);

                 CTblBind  ctb1(T, rec);    const  CTblBind&  CTB1 =  ctb1;
                 CTblBind  ctb2(T, rec2);   const  CTblBind&  CTB2 =  ctb2;

                  TblBind   tb1(mT, rec);   const   TblBind&   TB1 =   tb1;
                  TblBind   tb2(mT, rec2);  const   TblBind&   TB2 =   tb2;

                ASSERT(   CTB1 ==  CTB2);
                ASSERT(    TB1 ==   TB2);

                ASSERT(!( CTB1 !=  CTB2));
                ASSERT(!(  TB1 !=   TB2));
            }
        }
      }

DEFINE_TEST_CASE(17) {
        // --------------------------------------------------------------
        // TESTING EQUALITY OPERATORS (ROW BINDINGS)
        //   Test 'operator==' and 'operator!=' for Row Bindings.
        //
        // Concerns:
        //   The operators return correct results.
        //
        // Plan:
        //   The operators are tested for the following conditions:
        //   - Comparison to self (X == X).
        //   - The two bindings being compared store the same 'bdem_Row'
        //     pointer and the same 'bdem_RecordDef'.
        //   - The two bindings being compared store different 'bdem_Row'
        //     pointers and the same 'bdem_RecordDef', with the two rows having
        //     identical data.
        //   - The two bindings being compared store different 'bdem_Row'
        //     pointers and the same 'bdem_RecordDef', with the two rows having
        //     different data.
        //   - The two bindings being compared store the same 'bdem_Row'
        //     pointer, but different 'bdem_RecordDef's.
        //
        //   To check these conditions we:
        //   1) Create a 'bdem_Row' and a 'bdem_RecordDef' and initialize a row
        //      binding from them and test the operators for comparison to
        //      self.  'operator==' should return 'true' and 'operator!='
        //      should return 'false'.
        //   2) Create a 'bdem_Row' and a 'bdem_RecordDef' and initialize two
        //      row bindings with that 'bdem_Row' and 'bdem_RecordDef'.  On
        //      comparing the two bindings, 'operator==' should return 'true'
        //      and 'operator!=' should return 'false'.
        //   3) Create two 'bdem_Row's having the *same* value and the same
        //      'bdem_RecordDef', and use them to initialize two row bindings.
        //      The 'bdem_RecordDef' used for creating the row bindings is the
        //      same 'bdem_RecordDef'.  On comparing the two bindings,
        //      'operator==' should return 'true' and 'operator!=' should
        //      return 'false'.
        //   4) Create two 'bdem_Row's having *different* values, but
        //      satisfying the same 'bdem_RecordDef', and use them to
        //      initialize two row bindings.  The 'bdem_RecordDef' used for
        //      creating the row bindings is the same 'bdem_RecordDef'.  On
        //      comparing the two bindings, 'operator==' should return 'false'
        //      and 'operator!=' should return 'true'.
        //   5) Create two row bindings using the same 'bdem_Row', but
        //      different 'bdem_RecordDef' instances (having the same value).
        //      On comparing the two row bindings, 'operator==' should return
        //      'true' and 'operator!=' should return 'false'.
        //
        // Testing:
        //   bool operator==(const ConstRowBinding& lhs, const& rhs);
        //   bool operator!=(const ConstRowBinding& lhs, const& rhs);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "Testing Row Binding Equality Operators" << endl
                 << "======================================" << endl;

        {
            // ---------------------- TEST DATA ---------------------------

            Schema S;  createSchema(&S, ":aAaBbCcDd");

            const RecDef *rec = &S.record(0);

            List mL;  const List& L = mL;  createList(&mL, rec);

            if (veryVerbose) cout << "Checking self-comparison." << endl;
            {
                 CRowBind  crb(&L, rec);   const  CRowBind&  CRB =  crb;
                  RowBind   rb(&mL, rec);  const   RowBind&   RB =   rb;

                ASSERT(   CRB ==  CRB);
                ASSERT(    RB ==   RB);

                ASSERT(!( CRB !=  CRB));
                ASSERT(!(  RB !=   RB));
            }

            if (veryVerbose) {
                cout << "With same 'row' and same 'record'." << endl;
            }
            {
                 CRowBind  crb1(&L, rec);   const  CRowBind&  CRB1 =  crb1;
                 CRowBind  crb2(&L, rec);   const  CRowBind&  CRB2 =  crb2;

                  RowBind   rb1(&mL, rec);  const   RowBind&   RB1 =   rb1;
                  RowBind   rb2(&mL, rec);  const   RowBind&   RB2 =   rb2;

                ASSERT(   CRB1 ==  CRB2);
                ASSERT(    RB1 ==   RB2);

                ASSERT(!( CRB1 !=  CRB2));
                ASSERT(!(  RB1 !=   RB2));
            }

            if (veryVerbose) {
                cout << "With different 'row's, having"
                        " same *data* and same 'record'." << endl;
            }
            {
                List mL2 = L; const List& L2 = mL2;

                 CRowBind  crb1(&L, rec);    const  CRowBind&  CRB1 =  crb1;
                 CRowBind  crb2(&L2, rec);   const  CRowBind&  CRB2 =  crb2;

                  RowBind   rb1(&mL, rec);   const   RowBind&   RB1 =   rb1;
                  RowBind   rb2(&mL2, rec);  const   RowBind&   RB2 =   rb2;

                ASSERT(   CRB1 ==  CRB2);
                ASSERT(    RB1 ==   RB2);

                ASSERT(!( CRB1 !=  CRB2));
                ASSERT(!(  RB1 !=   RB2));
            }

            if (veryVerbose) {
                cout << "With different 'row's, having"
                        " different *data* and same 'record'." << endl;
            }
            {
                List mL2 = L; const List& L2 = mL2;

                 CRowBind  crb1(&L, rec);    const  CRowBind&  CRB1 =  crb1;
                  RowBind   rb1(&mL, rec);   const   RowBind&   RB1 =   rb1;

                createList(&mL2, rec);  // append data to the second list

                ASSERT(L != L2);

                 CRowBind  crb2(&L2, rec);   const  CRowBind&  CRB2 =  crb2;
                  RowBind   rb2(&mL2, rec);  const   RowBind&   RB2 =   rb2;

                ASSERT(!( CRB1 ==  CRB2));
                ASSERT(!(  RB1 ==   RB2));

                ASSERT(   CRB1 !=  CRB2);
                ASSERT(    RB1 !=   RB2);
            }

            if (veryVerbose) {
                cout << "With same 'row', different 'record's." << endl;
            }
            {
                Schema S2(S);
                const RecDef *rec2 = &S2.record(0);

                ASSERT(rec != rec2);

                 CRowBind  crb1(&L, rec);    const  CRowBind&  CRB1 =  crb1;
                 CRowBind  crb2(&L, rec2);   const  CRowBind&  CRB2 =  crb2;

                  RowBind   rb1(&mL, rec);   const   RowBind&   RB1 =   rb1;
                  RowBind   rb2(&mL, rec2);  const   RowBind&   RB2 =   rb2;

                ASSERT(   CRB1 ==  CRB2);
                ASSERT(    RB1 ==   RB2);

                ASSERT(!( CRB1 !=  CRB2));
                ASSERT(!(  RB1 !=   RB2));
            }
        }
      }

DEFINE_TEST_CASE(16) {
        // --------------------------------------------------------------------
        // TESTING CHOICE ARRAY BINDING REBIND
        //     Test Choice Array Binding 'rebind' methods.
        //
        // Concerns:
        //   - The 'rebind' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The rebound data and meta-data are hooked up correctly.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_ChoiceArray' objects that satisfy the final
        //        records contained in the schemas from 1.
        //     3) A vector of record names.  This vector stores the *names* of
        //        the records in the schemas from 1) corresponding to the data
        //        in the lists from 2.
        //
        //   For every element in the vector of test choice arrays, create
        //   instances of 'bdem_ConstChoiceArrayBinding' and
        //   'bdem_ChoiceArrayBinding' (for each of the
        //   'rebind' methods under test).  Then rebind these objects (using
        //   one 'rebind' method per object) to all of the other test choices,
        //   and each element in the choice arrays.  Use the direct accessors
        //   to verify that the rebound row and record have been installed
        //   correctly.
        //
        // Testing:
        //             bdem_ConstChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   void rebind(const bdem_ConstChoiceArrayBinding& binding);
        //   void rebind(const bdem_ChoiceArray *,  *record);
        //   void rebind(const bdem_ChoiceArray *,  *schema, *name);
        //   void rebind(const bdem_ChoiceArray *,  *schema, int);
        //
        //             bdem_ChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   void rebind(bdem_ChoiceArrayBinding& binding);
        //   void rebind(bdem_ChoiceArray *, *record);
        //   void rebind(bdem_ChoiceArray *, *schema, *name);
        //   void rebind(bdem_ChoiceArray *, *schema, int);
        // -------------------------------------------------------------------

        DECLARE_CHOICE_OBJECTS

        if (verbose) cout << "Testing Choice Binding 'rebind'" << endl
                          << "===============================" << endl;
        {
            for (int i = 0; i < NUM_CHOICE_SPECS; ++i) {
                ChoiceArray mCA = choiceArrayVec[i];
          const ChoiceArray& CA = mCA;

                const Schema&  S   = choiceArraySchemaVec[i];
                const RecDef&  rec = S.record(S.numRecords() - 1);

                      CCArrBind  ccab1(&CA, &rec);
                      CCArrBind  ccab2(&CA, &rec);
                      CCArrBind  ccab3(&CA, &rec);
                      CCArrBind  ccab4(&CA, &rec);

                const CCArrBind& CCAB1 = ccab1;
                const CCArrBind& CCAB2 = ccab2;
                const CCArrBind& CCAB3 = ccab3;
                const CCArrBind& CCAB4 = ccab4;

                       CArrBind   cab1(&mCA, &rec);
                       CArrBind   cab2(&mCA, &rec);
                       CArrBind   cab3(&mCA, &rec);
                       CArrBind   cab4(&mCA, &rec);

                const  CArrBind&  CAB1 = cab1;
                const  CArrBind&  CAB2 = cab2;
                const  CArrBind&  CAB3 = cab3;
                const  CArrBind&  CAB4 = cab4;

                if (veryVerbose) {
                    cout << "'rebind' taking 'ChoiceArray' pointers." << endl;
                }

                for (int j = 0; j < NUM_CHOICE_SPECS; ++j) {
                    ChoiceArray mLx = choiceArrayVec[j];
            const ChoiceArray& Lx = mLx;

                    const Schema&  Sx       = choiceArraySchemaVec[j];
                    const RecDef&  recx     = Sx.record(Sx.numRecords() - 1);
                    const char    *recNamex = choiceArrayRecordName[j];

                    ccab1.rebind(&Lx, &recx);
                    ccab2.rebind(&Lx, &Sx, recNamex);
                    ccab3.rebind(&Lx, &Sx, Sx.numRecords() - 1);
                    ccab4.rebind(CCAB1);

                    cab1.rebind(&mLx, &recx);
                    cab2.rebind(&mLx, &Sx, recNamex);
                    cab3.rebind(&mLx, &Sx, Sx.numRecords() - 1);
                    cab4.rebind(CAB1);

                    ASSERT( &Lx == &CCAB1.choiceArray());
                    ASSERT( &Lx == &CCAB2.choiceArray());
                    ASSERT( &Lx == &CCAB3.choiceArray());
                    ASSERT( &Lx == &CCAB4.choiceArray());

                    ASSERT(&mLx ==  &CAB1.choiceArray());
                    ASSERT(&mLx ==  &CAB2.choiceArray());
                    ASSERT(&mLx ==  &CAB3.choiceArray());
                    ASSERT(&mLx ==  &CAB4.choiceArray());

                    ASSERT(&recx == &CCAB1.record());
                    ASSERT(&recx == &CCAB2.record());
                    ASSERT(&recx == &CCAB3.record());
                    ASSERT(&recx == &CCAB4.record());

                    ASSERT(&recx ==  &CAB1.record());
                    ASSERT(&recx ==  &CAB2.record());
                    ASSERT(&recx ==  &CAB3.record());
                    ASSERT(&recx ==  &CAB4.record());
                }

            }
        }
      }

DEFINE_TEST_CASE(15) {
        // --------------------------------------------------------------------
        // TESTING CHOICE BINDING REBIND
        //     Test Choice Binding 'rebind' methods.
        //
        // Concerns:
        //   - The 'rebind' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The rebound data and meta-data are hooked up correctly.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_Choice' objects that satisfy the final
        //        records contained in the schemas from 1)
        //     3) A vector of record names.  This vector stores the *names* of
        //        the records in the schemas from 1) corresponding to the data
        //        in the lists from 2).
        //     4) A second vector of 'bdem_Schema' objects.
        //     5) A vector of 'bdem_ChoiceArray' objects that satisfy the final
        //        records contained in the schemas from 4).
        //     6) A second vector of record names.  This vector stores the
        //        *names* of the records in the schemas from 4) corresponding
        //        to the data in the tables from 5).
        //
        //   For every element in the vector of test lists, create instances of
        //   'bdem_ConstChoiceBinding' and 'bdem_ChoiceBinding' (for each of
        //   the 'rebind' methods under test).  Then rebind these objects (with
        //   one 'rebind' method per object) to all of the other test choices,
        //   and each element in the choice arrays.  Use the direct accessors
        //   to verify that the rebound row and record have been installed
        //   correctly.
        //
        // Testing:
        //             bdem_ConstChoiceBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   void rebind(const bdem_ConstChoiceBinding& binding);
        //   void rebind(const bdem_ChoiceArrayItem *, *record);
        //   void rebind(const bdem_ChoiceArrayItem *, *schema, *name);
        //   void rebind(const bdem_ChoiceArrayItem *, *schema, int);
        //   void rebind(const bdem_Choice *, *record);
        //   void rebind(const bdem_Choice *, *schema, *name);
        //   void rebind(const bdem_Choice *, *schema, int);
        //   void rebind(const bdem_ChoiceArray *, int, *record);
        //   void rebind(const bdem_ChoiceArray *, int, *schema, *name);
        //   void rebind(const bdem_ChoiceArray *, int, *schema, int);
        //
        //             bdem_ChoiceBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   void rebind(bdem_ChoiceBinding& binding);
        //   void rebind(bdem_ChoiceArrayItem *, *record);
        //   void rebind(bdem_ChoiceArrayItem *, *schema, *name);
        //   void rebind(bdem_ChoiceArrayItem *, *schema, int);
        //   void rebind(bdem_Choice *, *record);
        //   void rebind(bdem_Choice *, *schema, *name);
        //   void rebind(bdem_Choice *, *schema, int);
        //   void rebind(bdem_ChoiceArray *, int, *record);
        //   void rebind(bdem_ChoiceArray *, int, *schema, *name);
        //   void rebind(bdem_ChoiceArray *, int, *schema, int);
        // -------------------------------------------------------------------

        DECLARE_CHOICE_OBJECTS

        if (verbose) cout << "Testing Choice  Binding 'rebind'" << endl
                          << "======================================" << endl;
        {
            for (int i = 0; i < NUM_CHOICE_SPECS; ++i) {
                Choice mC = choiceVec[i];  const Choice& C = mC;

                const Schema&  S   = choiceSchemaVec[i];
                const RecDef&  rec = S.record(S.numRecords() - 1);

                      CCBind  ccb1(&C, &rec);
                      CCBind  ccb2(&C, &rec);
                      CCBind  ccb3(&C, &rec);
                      CCBind  ccb4(&C, &rec);
                      CCBind  ccb5(&C, &rec);
                      CCBind  ccb6(&C, &rec);
                      CCBind  ccb7(&C, &rec);

                const CCBind& CCB1 = ccb1;
                const CCBind& CCB2 = ccb2;
                const CCBind& CCB3 = ccb3;
                const CCBind& CCB4 = ccb4;
                const CCBind& CCB5 = ccb5;
                const CCBind& CCB6 = ccb6;
                const CCBind& CCB7 = ccb7;

                       CBind   cb1(&mC, &rec);
                       CBind   cb2(&mC, &rec);
                       CBind   cb3(&mC, &rec);
                       CBind   cb4(&mC, &rec);
                       CBind   cb5(&mC, &rec);
                       CBind   cb6(&mC, &rec);
                       CBind   cb7(&mC, &rec);

                const  CBind&  CB1 = cb1;
                const  CBind&  CB2 = cb2;
                const  CBind&  CB3 = cb3;
                const  CBind&  CB4 = cb4;
                const  CBind&  CB5 = cb5;
                const  CBind&  CB6 = cb6;
                const  CBind&  CB7 = cb7;

                if (veryVerbose) {
                    cout << "'rebind' taking 'Choice' or 'ChoiceArrayItem' "
              "pointers." << endl;
                }

                for (int j = 0; j < NUM_CHOICE_SPECS; ++j) {
                    Choice mLx = choiceVec[j];  const Choice& Lx = mLx;

                    const Schema&  Sx       = choiceSchemaVec[j];
                    const RecDef&  recx     = Sx.record(Sx.numRecords() - 1);
                    const char    *recNamex = choiceRecordName[j];

                    ccb1.rebind(&Lx.item(), &recx);
                    ccb2.rebind(&Lx.item(), &Sx, recNamex);
                    ccb3.rebind(&Lx.item(), &Sx, Sx.numRecords() - 1);
                    ccb4.rebind(&Lx, &recx);
                    ccb5.rebind(&Lx, &Sx, recNamex);
                    ccb6.rebind(&Lx, &Sx, Sx.numRecords() - 1);
                    ccb7.rebind(CCB1);

                    cb1.rebind(&mLx.item(), &recx);
                    cb2.rebind(&mLx.item(), &Sx, recNamex);
                    cb3.rebind(&mLx.item(), &Sx, Sx.numRecords() - 1);
                    cb4.rebind(&mLx, &recx);
                    cb5.rebind(&mLx, &Sx, recNamex);
                    cb6.rebind(&mLx, &Sx, Sx.numRecords() - 1);
                    cb7.rebind(CB1);

                    ASSERT( &Lx.item() == &CCB1.item());
                    ASSERT( &Lx.item() == &CCB2.item());
                    ASSERT( &Lx.item() == &CCB3.item());
                    ASSERT( &Lx.item() == &CCB4.item());
                    ASSERT( &Lx.item() == &CCB5.item());
                    ASSERT( &Lx.item() == &CCB6.item());
                    ASSERT( &Lx.item() == &CCB7.item());

                    ASSERT(&mLx.item() ==  &CB1.item());
                    ASSERT(&mLx.item() ==  &CB2.item());
                    ASSERT(&mLx.item() ==  &CB3.item());
                    ASSERT(&mLx.item() ==  &CB4.item());
                    ASSERT(&mLx.item() ==  &CB5.item());
                    ASSERT(&mLx.item() ==  &CB6.item());
                    ASSERT(&mLx.item() ==  &CB7.item());

                    ASSERT(&recx == &CCB1.record());
                    ASSERT(&recx == &CCB2.record());
                    ASSERT(&recx == &CCB3.record());
                    ASSERT(&recx == &CCB4.record());
                    ASSERT(&recx == &CCB5.record());
                    ASSERT(&recx == &CCB6.record());
                    ASSERT(&recx == &CCB7.record());

                    ASSERT(&recx ==  &CB1.record());
                    ASSERT(&recx ==  &CB2.record());
                    ASSERT(&recx ==  &CB3.record());
                    ASSERT(&recx ==  &CB4.record());
                    ASSERT(&recx ==  &CB5.record());
                    ASSERT(&recx ==  &CB6.record());
                    ASSERT(&recx ==  &CB7.record());
                }

                if (veryVerbose) {
                    cout << "'rebind' taking 'ChoiceArrayItem' pointer"
             << "and index."  << endl;
                }

                for (int cai = 0; cai < NUM_CHOICE_SPECS; ++cai) {
                    ChoiceArray mCA(choiceArrayVec[cai]);
            const ChoiceArray& CA = mCA;

                    const Schema&  Sy       = choiceArraySchemaVec[cai];
                    const RecDef&  recy     = Sy.record(Sy.numRecords() - 1);
                    const char    *recNamey = choiceRecordName[cai];

                    for (int index = 0; index < CA.length(); ++index) {
                        if (veryVeryVerbose) {
                            T_ cout << "Original index: " << CA[index] << endl;
                        }

                        ccb1.rebind(&CA, index, &recy);
                        ccb2.rebind(&CA, index, &Sy, recNamey);
                        ccb3.rebind(&CA, index, &Sy, Sy.recordIndex(recNamey));

                        cb1.rebind(&mCA, index, &recy);
                        cb2.rebind(&mCA, index, &Sy, recNamey);
                        cb3.rebind(&mCA, index, &Sy, Sy.recordIndex(recNamey));

                        ASSERT( &CA[index] == &CCB1.item());
                        ASSERT( &CA[index] == &CCB2.item());
                        ASSERT( &CA[index] == &CCB3.item());

                        ASSERT(&mCA.theModifiableItem(index) ==  &CB1.item());
                        ASSERT(&mCA.theModifiableItem(index) ==  &CB2.item());
                        ASSERT(&mCA.theModifiableItem(index) ==  &CB1.item());

                        ASSERT(&recy == &CCB1.record());
                        ASSERT(&recy == &CCB2.record());
                        ASSERT(&recy == &CCB3.record());

                        ASSERT(&recy ==  &CB1.record());
                        ASSERT(&recy ==  &CB2.record());
                        ASSERT(&recy ==  &CB3.record());
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(14) {
        // --------------------------------------------------------------------
        // TESTING COLUMN BINDING REBIND
        //     Test Column Binding 'rebind' methods.
        //
        // Concerns:
        //   - The 'rebind' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The rebound data, column index, and meta-data are hooked up
        //     correctly.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_Table' objects that satisfy the final
        //        records contained in the schemas from 1).
        //     3) A vector of record names.  This vector stores the
        //        *names* of the records in the schemas from 1) corresponding
        //        to the data in the tables from 2).
        //
        //   For each column in every element in the vector of test tables,
        //   create instances of 'bdem_ConstColumnBinding' and
        //   'bdem_ColumnBinding' (for each of the 'rebind' methods under
        //   test).  Then rebind these objects (using one 'rebind' method per
        //   object) to all of the other columns in the test tables.  Use the
        //   direct accessors to verify that the rebound table, column index,
        //   and record have been installed correctly.
        //
        // Testing:
        //             bdem_ConstColumnBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~~
        //   void rebind(const bdem_ConstColumnBinding& binding);
        //   void rebind(const Table *, int, *field);
        //
        //             bdem_ColumnBinding
        //             ~~~~~~~~~~~~~~~~~~
        //   void rebind(const bdem_ColumnBinding& binding);
        //   void rebind(Table *, int, *field);
        // --------------------------------------------------------------------

        DECLARE_TABLE_OBJECTS

        if (verbose)
            cout << "Testing Column Binding 'rebind'" << endl
                 << "===============================" << endl;

        {
            for (int i = 0; i < NUM_TABLE_SPECS; ++i) {
                Table mT(tableVec[i]);  const Table& T = mT;
                const RecDef& rec = *tableRecVec[i];

                for (int col = 0; col < T.numColumns(); ++col) {
                    const FldDef& fld  = rec.field(col);

                    if (veryVeryVerbose) { T2_ P_(col) P(T); }

                          CColBind  ccb1(&T, col, &fld);
                    const CColBind& CCB1 = ccb1;

                          CColBind  ccb2(&T, col, &fld);
                    const CColBind& CCB2 = ccb2;

                           ColBind   cb1(&mT, col, &fld);
                    const  ColBind&  CB1 = cb1;

                           ColBind   cb2(&mT, col, &fld);
                    const  ColBind&  CB2 = cb2;

                    ASSERT( &T == &CCB1.table());
                    ASSERT (&T == &CCB2.table());

                    ASSERT(&mT ==  &CB1.table());
                    ASSERT(&mT ==  &CB2.table());

                    ASSERT(col == CCB1.columnIndex());
                    ASSERT(col == CCB2.columnIndex());

                    ASSERT(col ==  CB1.columnIndex());
                    ASSERT(col ==  CB2.columnIndex());

                    ASSERT(&fld == &CCB1.field());
                    ASSERT(&fld == &CCB2.field());

                    ASSERT(&fld ==  &CB1.field());
                    ASSERT(&fld ==  &CB2.field());

                    for (int j = 0; j < NUM_TABLE_SPECS; ++j) {
                        Table mTx(tableVec[j]); const Table& Tx = mTx;
                        const RecDef& recx = *tableRecVec[j];

                        for (int colx = 0; colx < Tx.numColumns(); ++colx) {
                            const FldDef& fldx = recx.field(colx);

                            ccb1.rebind(&Tx, colx, &fldx);
                            ccb2.rebind(CCB1);

                            cb1.rebind(&mTx, colx, &fldx);
                            cb2.rebind(CB1);

                            ASSERT( &Tx == &CCB1.table());
                            ASSERT( &Tx == &CCB2.table());

                            ASSERT(&mTx ==  &CB1.table());
                            ASSERT(&mTx ==  &CB2.table());

                            ASSERT(colx == CCB1.columnIndex());
                            ASSERT(colx == CCB2.columnIndex());

                            ASSERT(colx ==  CB1.columnIndex());
                            ASSERT(colx ==  CB2.columnIndex());

                            ASSERT(&fldx == &CCB1.field());
                            ASSERT(&fldx == &CCB2.field());

                            ASSERT(&fldx ==  &CB1.field());
                            ASSERT(&fldx ==  &CB2.field());
                        }
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(13) {
        // --------------------------------------------------------------------
        // TESTING TABLE BINDING REBIND
        //     Test Table Binding 'rebind' methods.
        //
        // Concerns:
        //   - The 'rebind' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The rebound data and meta-data are hooked up correctly.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_Table' objects that satisfy the final
        //        records contained in the schemas from 1).
        //     3) A vector of record names.  This vector stores the
        //        *names* of the records in the schemas from 1) corresponding
        //        to the data in the tables from 2).
        //
        //   For every element in the vector of test tables, create instances
        //   of 'bdem_ConstTableBinding' and 'bdem_TableBinding' (for each of
        //   the 'rebind' methods under test).  Then rebind these objects
        //   (using one 'rebind' method per object) to all of the other test
        //   tables.  Use the direct accessors to verify that the rebound table
        //   and record have been installed correctly.
        //
        // Testing:
        //             bdem_ConstTableBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~
        //   void rebind(const bdem_ConstTableBinding& binding);
        //   void rebind(const Table *, *record);
        //   void rebind(const Table *, *schema, *name);
        //   void rebind(const Table *, *schema, int);
        //
        //             bdem_TableBinding
        //             ~~~~~~~~~~~~~~~~~
        //   void rebind(const bdem_TableBinding& binding);
        //   void rebind(Table *, *record);
        //   void rebind(Table *, *schema, *name);
        //   void rebind(Table *, *schema, int);
        // --------------------------------------------------------------------

        DECLARE_TABLE_OBJECTS

        if (verbose)
            cout << "Testing Table Binding 'rebind'" << endl
                 << "==============================" << endl;

        {
           for (int i = 0; i < NUM_TABLE_SPECS; ++i) {
               Table mT(tableVec[i]);  const Table& T = mT;
               const RecDef&  rec = *tableRecVec[i];

                     CTblBind  ctb1(&T, &rec);
                     CTblBind  ctb2(&T, &rec);
                     CTblBind  ctb3(&T, &rec);
                     CTblBind  ctb4(&T, &rec);

               const CTblBind& CTB1 = ctb1;
               const CTblBind& CTB2 = ctb2;
               const CTblBind& CTB3 = ctb3;
               const CTblBind& CTB4 = ctb4;

                      TblBind   tb1(&mT, &rec);
                      TblBind   tb2(&mT, &rec);
                      TblBind   tb3(&mT, &rec);
                      TblBind   tb4(&mT, &rec);

               const  TblBind&  TB1 = tb1;
               const  TblBind&  TB2 = tb2;
               const  TblBind&  TB3 = tb3;
               const  TblBind&  TB4 = tb4;

               ASSERT( &T == &CTB1.table());
               ASSERT( &T == &CTB2.table());
               ASSERT( &T == &CTB3.table());
               ASSERT( &T == &CTB4.table());

               ASSERT(&mT ==  &TB1.table());
               ASSERT(&mT ==  &TB2.table());
               ASSERT(&mT ==  &TB3.table());
               ASSERT(&mT ==  &TB4.table());

               ASSERT(&rec == &CTB1.record());
               ASSERT(&rec == &CTB2.record());
               ASSERT(&rec == &CTB3.record());
               ASSERT(&rec == &CTB4.record());

               ASSERT(&rec ==  &TB1.record());
               ASSERT(&rec ==  &TB2.record());
               ASSERT(&rec ==  &TB3.record());
               ASSERT(&rec ==  &TB4.record());

               for (int j = 0; j < NUM_TABLE_SPECS; ++j) {
                   Table mTx(tableVec[j]);  const Table& Tx = mTx;

                   const Schema&  Sx       = tableSchemaVec[j];
                   const RecDef&  recx     = *tableRecVec[j];
                   const char    *recNamex = tableRecordName[j];

                   ctb1.rebind(&Tx, &recx);
                   ctb2.rebind(&Tx, &Sx, recNamex);
                   ctb3.rebind(&Tx, &Sx, Sx.recordIndex(recNamex));
                   ctb4.rebind(CTB1);

                   tb1.rebind(&mTx, &recx);
                   tb2.rebind(&mTx, &Sx, recNamex);
                   tb3.rebind(&mTx, &Sx, Sx.recordIndex(recNamex));
                   tb4.rebind(TB1);

                   ASSERT( &Tx == &CTB1.table());
                   ASSERT( &Tx == &CTB2.table());
                   ASSERT( &Tx == &CTB3.table());
                   ASSERT( &Tx == &CTB4.table());

                   ASSERT(&mTx ==  &TB1.table());
                   ASSERT(&mTx ==  &TB2.table());
                   ASSERT(&mTx ==  &TB3.table());
                   ASSERT(&mTx ==  &TB4.table());

                   ASSERT(&recx == &CTB1.record());
                   ASSERT(&recx == &CTB2.record());
                   ASSERT(&recx == &CTB3.record());
                   ASSERT(&recx == &CTB4.record());

                   ASSERT(&recx ==  &TB1.record());
                   ASSERT(&recx ==  &TB2.record());
                   ASSERT(&recx ==  &TB3.record());
                   ASSERT(&recx ==  &TB4.record());
               }
           }
        }
      }

DEFINE_TEST_CASE(12) {
        // --------------------------------------------------------------------
        // TESTING ROW BINDING REBIND
        //     Test Row Binding 'rebind' methods.
        //
        // Concerns:
        //   - The 'rebind' functions change the internal state of objects
        //     correctly and leave the objects in a valid state.
        //   - The rebound data and meta-data are hooked up correctly.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_List' objects that satisfy the final
        //        records contained in the schemas from 1).
        //     3) A vector of record names.  This vector stores the *names* of
        //        the records in the schemas from 1) corresponding to the data
        //        in the lists from 2).
        //     4) A second vector of 'bdem_Schema' objects.
        //     5) A vector of 'bdem_Table' objects that satisfy the final
        //        records contained in the schemas from 4).
        //     6) A second vector of record names.  This vector stores the
        //        *names* of the records in the schemas from 4) corresponding
        //        to the data in the tables from 5).
        //
        //   For every element in the vector of test lists, create instances of
        //   'bdem_ConstRowBinding' and 'bdem_RowBinding' (for each of the
        //   'rebind' methods under test).  Then rebind these objects (using
        //   one 'rebind' method per object) to all of the other test lists,
        //   and each row in the test tables.  Use the direct accessors to
        //   verify that the rebound row and record have been installed
        //   correctly.
        //
        // Testing:
        //             bdem_ConstRowBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   void rebind(const bdem_ConstRowBinding& binding);
        //   void rebind(const Row *, *record);
        //   void rebind(const Row *, *schema, *name);
        //   void rebind(const Row *, *schema, int);
        //   void rebind(const List *, *record);
        //   void rebind(const List *, *schema, *name);
        //   void rebind(const List *, *schema, int);
        //   void rebind(const Table *, int, *record);
        //   void rebind(const Table *, int, *schema, *name);
        //   void rebind(const Table *, int, *schema, int);
        //
        //             bdem_RowBinding
        //             ~~~~~~~~~~~~~~~
        //   void rebind(const bdem_RowBinding& binding);
        //   void rebind(Row *, *record);
        //   void rebind(Row *, *schema, *name);
        //   void rebind(Row *, *schema, int);
        //   void rebind(List *, *record);
        //   void rebind(List *, *schema, *name);
        //   void rebind(List *, *schema, int);
        //   void rebind(Table *, int, *record);
        //   void rebind(Table *, int, *schema, *name);
        //   void rebind(Table *, int, *schema, int);
        // -------------------------------------------------------------------

        DECLARE_TABLE_OBJECTS

        if (verbose) cout << "Testing Row Binding 'rebind'" << endl
                          << "============================" << endl;
        {
            for (int i = 0; i < NUM_LIST_SPECS; ++i) {
                List mL = listVec[i];  const List& L = mL;

                const Schema&  S   = listSchemaVec[i];
                const RecDef&  rec = S.record(S.numRecords() - 1);

                      CRowBind  crb1(&L.row(), &rec);
                      CRowBind  crb2(&L.row(), &rec);
                      CRowBind  crb3(&L.row(), &rec);
                      CRowBind  crb4(&L.row(), &rec);
                      CRowBind  crb5(&L.row(), &rec);
                      CRowBind  crb6(&L.row(), &rec);
                      CRowBind  crb7(&L.row(), &rec);

                const CRowBind& CRB1 = crb1;
                const CRowBind& CRB2 = crb2;
                const CRowBind& CRB3 = crb3;
                const CRowBind& CRB4 = crb4;
                const CRowBind& CRB5 = crb5;
                const CRowBind& CRB6 = crb6;
                const CRowBind& CRB7 = crb7;

                       RowBind   rb1(&mL.row(), &rec);
                       RowBind   rb2(&mL.row(), &rec);
                       RowBind   rb3(&mL.row(), &rec);
                       RowBind   rb4(&mL.row(), &rec);
                       RowBind   rb5(&mL.row(), &rec);
                       RowBind   rb6(&mL.row(), &rec);
                       RowBind   rb7(&mL.row(), &rec);

                const  RowBind&  RB1 = rb1;
                const  RowBind&  RB2 = rb2;
                const  RowBind&  RB3 = rb3;
                const  RowBind&  RB4 = rb4;
                const  RowBind&  RB5 = rb5;
                const  RowBind&  RB6 = rb6;
                const  RowBind&  RB7 = rb7;

                ASSERT( &L.row() == &CRB1.row());
                ASSERT( &L.row() == &CRB2.row());
                ASSERT( &L.row() == &CRB3.row());
                ASSERT( &L.row() == &CRB4.row());
                ASSERT( &L.row() == &CRB5.row());
                ASSERT( &L.row() == &CRB6.row());
                ASSERT( &L.row() == &CRB7.row());

                ASSERT(&mL.row() ==  &RB1.row());
                ASSERT(&mL.row() ==  &RB2.row());
                ASSERT(&mL.row() ==  &RB3.row());
                ASSERT(&mL.row() ==  &RB4.row());
                ASSERT(&mL.row() ==  &RB5.row());
                ASSERT(&mL.row() ==  &RB6.row());
                ASSERT(&mL.row() ==  &RB7.row());

                ASSERT(&rec == &CRB1.record());
                ASSERT(&rec == &CRB2.record());
                ASSERT(&rec == &CRB3.record());
                ASSERT(&rec == &CRB4.record());
                ASSERT(&rec == &CRB5.record());
                ASSERT(&rec == &CRB6.record());
                ASSERT(&rec == &CRB7.record());

                ASSERT(&rec ==  &RB1.record());
                ASSERT(&rec ==  &RB2.record());
                ASSERT(&rec ==  &RB3.record());
                ASSERT(&rec ==  &RB4.record());
                ASSERT(&rec ==  &RB5.record());
                ASSERT(&rec ==  &RB6.record());
                ASSERT(&rec ==  &RB7.record());

                if (veryVerbose) {
                    cout << "'rebind' taking 'row' or 'list' pointer." << endl;
                }

                for (int j = 0; j < NUM_LIST_SPECS; ++j) {
                    List mLx = listVec[j];  const List& Lx = mLx;

                    const Schema&  Sx       = listSchemaVec[j];
                    const RecDef&  recx     = Sx.record(Sx.numRecords() - 1);
                    const char    *recNamex = listRecordName[j];

                    crb1.rebind(&Lx.row(), &recx);
                    crb2.rebind(&Lx.row(), &Sx, recNamex);
                    crb3.rebind(&Lx.row(), &Sx, Sx.numRecords() - 1);
                    crb4.rebind(&Lx, &recx);
                    crb5.rebind(&Lx, &Sx, recNamex);
                    crb6.rebind(&Lx, &Sx, Sx.numRecords() - 1);
                    crb7.rebind(CRB1);

                    rb1.rebind(&mLx.row(), &recx);
                    rb2.rebind(&mLx.row(), &Sx, recNamex);
                    rb3.rebind(&mLx.row(), &Sx, Sx.numRecords() - 1);
                    rb4.rebind(&mLx, &recx);
                    rb5.rebind(&mLx, &Sx, recNamex);
                    rb6.rebind(&mLx, &Sx, Sx.numRecords() - 1);
                    rb7.rebind(RB1);

                    ASSERT( &Lx.row() == &CRB1.row());
                    ASSERT( &Lx.row() == &CRB2.row());
                    ASSERT( &Lx.row() == &CRB3.row());
                    ASSERT( &Lx.row() == &CRB4.row());
                    ASSERT( &Lx.row() == &CRB5.row());
                    ASSERT( &Lx.row() == &CRB6.row());
                    ASSERT( &Lx.row() == &CRB7.row());

                    ASSERT(&mLx.row() ==  &RB1.row());
                    ASSERT(&mLx.row() ==  &RB2.row());
                    ASSERT(&mLx.row() ==  &RB3.row());
                    ASSERT(&mLx.row() ==  &RB4.row());
                    ASSERT(&mLx.row() ==  &RB5.row());
                    ASSERT(&mLx.row() ==  &RB6.row());
                    ASSERT(&mLx.row() ==  &RB7.row());

                    ASSERT(&recx == &CRB1.record());
                    ASSERT(&recx == &CRB2.record());
                    ASSERT(&recx == &CRB3.record());
                    ASSERT(&recx == &CRB4.record());
                    ASSERT(&recx == &CRB5.record());
                    ASSERT(&recx == &CRB6.record());
                    ASSERT(&recx == &CRB7.record());

                    ASSERT(&recx ==  &RB1.record());
                    ASSERT(&recx ==  &RB2.record());
                    ASSERT(&recx ==  &RB3.record());
                    ASSERT(&recx ==  &RB4.record());
                    ASSERT(&recx ==  &RB5.record());
                    ASSERT(&recx ==  &RB6.record());
                    ASSERT(&recx ==  &RB7.record());
                }

                if (veryVerbose) {
                    cout << "'rebind' taking 'table' pointer and index."
                         << endl;
                }

                for (int ti = 0; ti < NUM_TABLE_SPECS; ++ti) {
                    Table mT(tableVec[ti]);  const Table& T = mT;

                    const Schema&  Sy       = tableSchemaVec[ti];
                    const RecDef&  recy     = *tableRecVec[ti];
                    const char    *recNamey = tableRecordName[ti];

                    for (int row = 0; row < T.numRows(); ++row) {
                        if (veryVeryVerbose) {
                            T_ cout << "Original Row: " << T[row] << endl;
                        }

                        crb1.rebind(&T, row, &recy);
                        crb2.rebind(&T, row, &Sy, recNamey);
                        crb3.rebind(&T, row, &Sy, Sy.recordIndex(recNamey));

                        rb1.rebind(&mT, row, &recy);
                        rb2.rebind(&mT, row, &Sy, recNamey);
                        rb3.rebind(&mT, row, &Sy, Sy.recordIndex(recNamey));

                        ASSERT( &T[row] == &CRB1.row());
                        ASSERT( &T[row] == &CRB2.row());
                        ASSERT( &T[row] == &CRB3.row());

                        ASSERT(&mT.theModifiableRow(row) ==  &RB1.row());
                        ASSERT(&mT.theModifiableRow(row) ==  &RB2.row());
                        ASSERT(&mT.theModifiableRow(row) ==  &RB1.row());

                        ASSERT(&recy == &CRB1.record());
                        ASSERT(&recy == &CRB2.record());
                        ASSERT(&recy == &CRB3.record());

                        ASSERT(&recy ==  &RB1.record());
                        ASSERT(&recy ==  &RB2.record());
                        ASSERT(&recy ==  &RB3.record());
                    }
                }
            }
        }
    }

DEFINE_TEST_CASE(11) {
        // --------------------------------------------------------------------
        // TESTING CHOICE ARRAY BINDING ACCESSORS
        //   Test Choice Binding accessor methods.
        //
        // Concerns:
        //   - That value-returning methods return the correct result.
        //   - That references of the appropriate type and value are returned.
        //   - That references of the appropriate 'const'-ness are returned.
        //
        // Plan:
        //   Create a 'bdem_ChoiceArray' containing a selection for each
        //   'bdem_ElemType::Type'.  Using that 'bdem_ChoiceArray',
        //   create instances of both Choice binding classes.
        //   Loop through the all the types using the
        //   bdem_ChoiceArray object to set the selection and value then
        //   access the 'bdem_ChoiceArray' using the non-modifiable
        //   accessor functions provided by the choice bindings and
        //   verify that the references returned match
        //   the original data.  Use the accessors that return modifiable
        //   references to change the value of the 'bdem_Array'.
        //   Check that the value of the original 'bdem_ChoiceArray'
        //   has been correctly modified via the bindings.
        //   As a negative test, confirm that the accessors of
        //   "const" row bindings return non-modifiable references.
        //
        // Testing:
        //
        //             bdem_ConstChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~
        //   bdem_ConstElemRef selection(int) const;
        //   bdem_ElemType::Type selectionType(int ) const;
        //   bdem_ElemType::Type selectionType(const char *elementName) const;
        //   bdem_ElemType::Type selectorType(int) const;
        //   int selector(int) const;
        //   int selectorId(int) const;
        //   const char *selectorName(int) const;
        //   char& theChar(int) const;
        //   short& theShort(int) const;
        //   int& theInt(int) const;
        //   Int64& theInt64(int) const;
        //   float& theFloat(int) const;
        //   double& theDouble(int) const;
        //   string& theString(int) const;
        //   bdet_Datetime& theDatetime(int) const;
        //   bdet_Time& theTime(int) const;
        //   bdet_Date& theDate(int) const;
        //   vector<char>& theCharArray(int) const;
        //   vector<short>& theShortArray(int) const;
        //   vector<int>& theIntArray(int) const;
        //   vector<Int64>& theInt64Array(int) const;
        //   vector<float>& theFloatArray(int) const;
        //   vector<double>& theDoubleArray(int) const;
        //   vector<string>& theStringArray(int) const;
        //   vector<Datetime>& theDatetimeArray(int) const;
        //   vector<Date>& theDateArray(int) const;
        //   vector<Time>& theTimeArray(int) const;
        //   bdem_List& theList(int) const;
        //   bdem_Table& theTable(int) const;
        //   bdem_Bool&  theBool(int)  const;
        //   bdem_DatetimeTz&  theDatetimeTz(int)  const;
        //   bdem_DateTz&  theDateTz(int)  const;
        //   bdem_TimeTz&  theTimeTz(int)  const;
        //   bdem_BoolArray&  theBoolArray(int)  const;
        //   bdem_DatetimeTzArray&  theDatetimeTzArray(int)  const;
        //   bdem_DateTzArray&  theDateTzArray(int)  const;
        //   bdem_TimeTzArray&  theTimeTzArray(int)  const;
        //   bdem_Choice&  theChoice(int)  const;
        //   bdem_ChoiceArray&  theChoiceArray(int)  const;
        //
        //             bdem_ChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~
        //   bdem_ConstElemRef selection(int) const;
        //   bdem_ElemType::Type selectionType(int ) const;
        //   bdem_ElemType::Type selectionType(const char *elementName) const;
        //   bdem_ElemType::Type selectorType(int) const;
        //   int selector(int) const;
        //   int selectorId(int) const;
        //   const char *selectorName(int) const;
        //   char& theChar(int) const;
        //   short& theShort(int) const;
        //   int& theInt(int) const;
        //   Int64& theInt64(int) const;
        //   float& theFloat(int) const;
        //   double& theDouble(int) const;
        //   string& theString(int) const;
        //   bdet_Datetime& theDatetime(int) const;
        //   bdet_Time& theTime(int) const;
        //   bdet_Date& theDate(int) const;
        //   vector<char>& theCharArray(int) const;
        //   vector<short>& theShortArray(int) const;
        //   vector<int>& theIntArray(int) const;
        //   vector<Int64>& theInt64Array(int) const;
        //   vector<float>& theFloatArray(int) const;
        //   vector<double>& theDoubleArray(int) const;
        //   vector<string>& theStringArray(int) const;
        //   vector<Datetime>& theDatetimeArray(int) const;
        //   vector<Date>& theDateArray(int) const;
        //   vector<Time>& theTimeArray(int) const;
        //   bdem_List& theList(int) const;
        //   bdem_Table& theTable(int) const;
        //   bdem_Bool&  theBool(int)  const;
        //   bdem_DatetimeTz&  theDatetimeTz(int)  const;
        //   bdem_DateTz&  theDateTz(int)  const;
        //   bdem_TimeTz&  theTimeTz(int)  const;
        //   bdem_BoolArray&  theBoolArray(int)  const;
        //   bdem_DatetimeTzArray&  theDatetimeTzArray(int)  const;
        //   bdem_DateTzArray&  theDateTzArray(int)  const;
        //   bdem_TimeTzArray&  theTimeTzArray(int)  const;
        //   bdem_Choice&  theChoice(int)  const;
        //   bdem_ChoiceArray&  theChoiceArray(int)  const;
        // --------------------------------------------------------------------

        DECLARE_TEST_LIST_OBJECTS

        if (verbose) cout << "Testing Choice Array Binding Accessors" << endl
                          << "=============================" << endl;

        {
            const char *testSpec =
       ":a?AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZzaAbBcCdDeEfF";

            Schema S;
            createSchema(&S,testSpec);
            const RecDef *rec = &S.record(S.numRecords() - 1);
            // SANITY CHECK ON SAMPLE DATA
            ASSERT (rec->numFields() == NUM_TYPES);
            ChoiceArray    mCA; const ChoiceArray& CA = mCA;
            createChoiceArray(&mCA, rec);
            mCA.removeAllItems();
            mCA.appendNullItems(NUM_TYPES);

            CArrBind     cab(&mCA,rec);    const  CArrBind&  CAB = cab;
            CCArrBind    ccab(&CA,rec);   const  CCArrBind& CCAB = ccab;

            if (veryVerbose) {
                cout << "Testing selector and selection accessors" << endl;
            }
            for (int i = 0; i < rec->numFields(); ++i ) {
                // to save time, just test the "diagonal" of the choice array
                // (i is both the index of the choice in the array,
                // and selector index for that choice)

                const bdem_FieldDef &field = rec->field(i);
                const char *fieldName = rec->fieldName(i);
                const int  fieldId = rec->fieldId(i);

                // SANITY CHECK ON SAMPLE DATA
                ASSERT (field.elemType() == CA.selectionType(i));
                ASSERT (field.elemType() == LX.elemType(i));
                ASSERT (i == fieldId);

                // Check if the value is unset
                ASSERT (bdem_ElemType::BDEM_VOID == CAB.selectorType(i));
                ASSERT (bdem_ElemType::BDEM_VOID == CCAB.selectorType(i));
                ASSERT (-1 == CAB.selector(i));
                ASSERT (-1 == CCAB.selector(i));

                // set the selection to that type
                mCA.theModifiableItem(i).makeSelection(i).replaceValue(LX[i]);

                ASSERT(CA.selectionType(i) == CAB.selectionType(i));
                ASSERT(CA.selectionType(i) == CAB.selectionType(fieldName));
                ASSERT(CA[i].selector()       == CAB.selector(i));
                ASSERT(CA[i].selectionType()  == CAB.selectorType(i));
                ASSERT(fieldId            == CAB.selectorId(i));
                ASSERT(fieldName          == CAB.selectorName(i));

                ASSERT(CA.selectionType(i) == CCAB.selectionType(i));
                ASSERT(CA.selectionType(i) == CCAB.selectionType(fieldName));
                ASSERT(CA[i].selector()       == CCAB.selector(i));
                ASSERT(CA[i].selectionType()  == CCAB.selectorType(i));
                ASSERT(fieldId            == CCAB.selectorId(i));
                ASSERT(fieldName          == CCAB.selectorName(i));

                ElemType::Type type = CA.selectionType(i);
                switch(type) {
                  // In each case, test the accessors and modify the value.
                  // The modify test is outside the switch.
                  case ElemType::BDEM_CHAR     : {
                    ASSERT (CA[i].theChar() == CAB.theChar(i));
                    ASSERT (CA[i].theChar() == CCAB.theChar(i));

                    CAB.theModifiableChar(i) = XA;
                    ASSERT (XA == LX.theChar(i));

                    CAB.theModifiableChar(i) = YA;
                    ASSERT (YA == LY.theChar(i));
                  } break;
                  case ElemType::BDEM_SHORT : {
                    ASSERT (CA[i].theShort() == CAB.theShort(i));
                    ASSERT (CA[i].theShort() == CCAB.theShort(i));

                    CAB.theModifiableShort(i) = XB;
                    ASSERT (XB == LX.theShort(i));

                    CAB.theModifiableShort(i) = YB;
                    ASSERT (YB == LY.theShort(i));
                  } break;
                  case ElemType::BDEM_INT : {
                    ASSERT (CA[i].theInt() == CAB.theInt(i));
                    ASSERT (CA[i].theInt() == CCAB.theInt(i));

                    CAB.theModifiableInt(i) = XC;
                    ASSERT (XC == LX.theInt(i));

                    CAB.theModifiableInt(i) = YC;
                    ASSERT (YC == LY.theInt(i));
                  } break;
                  case ElemType::BDEM_INT64 : {
                    ASSERT (CA[i].theInt64() == CAB.theInt64(i));
                    ASSERT (CA[i].theInt64() == CCAB.theInt64(i));

                    CAB.theModifiableInt64(i) = XD;
                    ASSERT (XD == LX.theInt64(i));

                    CAB.theModifiableInt64(i) = YD;
                    ASSERT (YD == LY.theInt64(i));
                  } break;
                  case ElemType::BDEM_FLOAT : {
                    ASSERT (CA[i].theFloat() == CAB.theFloat(i));
                    ASSERT (CA[i].theFloat() == CCAB.theFloat(i));

                    CAB.theModifiableFloat(i) = XE;
                    ASSERT (XE == LX.theFloat(i));

                    CAB.theModifiableFloat(i) = YE;
                    ASSERT (YE == LY.theFloat(i));
                  } break;
                  case ElemType::BDEM_DOUBLE : {
                    ASSERT (CA[i].theDouble() == CAB.theDouble(i));
                    ASSERT (CA[i].theDouble() == CCAB.theDouble(i));

                    CAB.theModifiableDouble(i) = XF;
                    ASSERT (XF == LX.theDouble(i));

                    CAB.theModifiableDouble(i) = YF;
                    ASSERT (YF == LY.theDouble(i));
                  } break;
                  case ElemType::BDEM_STRING : {
                    ASSERT (CA[i].theString() == CAB.theString(i));
                    ASSERT (CA[i].theString() == CCAB.theString(i));

                    CAB.theModifiableString(i) = XG;
                    ASSERT (XG == LX.theString(i));

                    CAB.theModifiableString(i) = YG;
                    ASSERT (YG == LY.theString(i));
                  } break;
                  case ElemType::BDEM_DATETIME : {
                    ASSERT (CA[i].theDatetime() == CAB.theDatetime(i));
                    ASSERT (CA[i].theDatetime() == CCAB.theDatetime(i));

                    CAB.theModifiableDatetime(i) = XH;
                    ASSERT (XH == LX.theDatetime(i));

                    CAB.theModifiableDatetime(i) = YH;
                    ASSERT (YH == LY.theDatetime(i));
                  } break;
                  case ElemType::BDEM_DATE : {
                    ASSERT (CA[i].theDate() == CAB.theDate(i));
                    ASSERT (CA[i].theDate() == CCAB.theDate(i));

                    CAB.theModifiableDate(i) = XI;
                    ASSERT (XI == LX.theDate(i));

                    CAB.theModifiableDate(i) = YI;
                    ASSERT (YI == LY.theDate(i));
                  } break;
                  case ElemType::BDEM_TIME : {
                    ASSERT (CA[i].theTime() == CAB.theTime(i));
                    ASSERT (CA[i].theTime() == CCAB.theTime(i));

                    CAB.theModifiableTime(i) = XJ;
                    ASSERT (XJ == LX.theTime(i));

                    CAB.theModifiableTime(i) = YJ;
                    ASSERT (YJ == LY.theTime(i));
                  } break;
                  case ElemType::BDEM_CHAR_ARRAY : {
                    ASSERT (CA[i].theCharArray() == CAB.theCharArray(i));
                    ASSERT (CA[i].theCharArray() == CCAB.theCharArray(i));

                    CAB.theModifiableCharArray(i) = XK;
                    ASSERT (XK == LX.theCharArray(i));

                    CAB.theModifiableCharArray(i) = YK;
                    ASSERT (YK == LY.theCharArray(i));
                  } break;
                  case ElemType::BDEM_SHORT_ARRAY : {
                    ASSERT (CA[i].theShortArray() == CAB.theShortArray(i));
                    ASSERT (CA[i].theShortArray() == CCAB.theShortArray(i));

                    CAB.theModifiableShortArray(i) = XL;
                    ASSERT (XL == LX.theShortArray(i));

                    CAB.theModifiableShortArray(i) = YL;
                    ASSERT (YL == LY.theShortArray(i));
                  } break;
                  case ElemType::BDEM_INT_ARRAY : {
                    ASSERT (CA[i].theIntArray() == CAB.theIntArray(i));
                    ASSERT (CA[i].theIntArray() == CCAB.theIntArray(i));

                    CAB.theModifiableIntArray(i) = XM;
                    ASSERT (XM == LX.theIntArray(i));

                    CAB.theModifiableIntArray(i) = YM;
                    ASSERT (YM == LY.theIntArray(i));
                  } break;
                  case ElemType::BDEM_INT64_ARRAY : {
                    ASSERT (CA[i].theInt64Array() == CAB.theInt64Array(i));
                    ASSERT (CA[i].theInt64Array() == CCAB.theInt64Array(i));

                    CAB.theModifiableInt64Array(i) = XN;
                    ASSERT (XN == LX.theInt64Array(i));

                    CAB.theModifiableInt64Array(i) = YN;
                    ASSERT (YN == LY.theInt64Array(i));
                  } break;
                  case ElemType::BDEM_FLOAT_ARRAY : {
                    ASSERT (CA[i].theFloatArray() == CAB.theFloatArray(i));
                    ASSERT (CA[i].theFloatArray() == CCAB.theFloatArray(i));

                    CAB.theModifiableFloatArray(i) = XO;
                    ASSERT (XO == LX.theFloatArray(i));

                    CAB.theModifiableFloatArray(i) = YO;
                    ASSERT (YO == LY.theFloatArray(i));
                  } break;
                  case ElemType::BDEM_DOUBLE_ARRAY : {
                    ASSERT (CA[i].theDoubleArray() == CAB.theDoubleArray(i));
                    ASSERT (CA[i].theDoubleArray() == CCAB.theDoubleArray(i));

                    CAB.theModifiableDoubleArray(i) = XP;
                    ASSERT (XP == LX.theDoubleArray(i));

                    CAB.theModifiableDoubleArray(i) = YP;
                    ASSERT (YP == LY.theDoubleArray(i));
                  } break;
                  case ElemType::BDEM_STRING_ARRAY : {
                    ASSERT (CA[i].theStringArray() == CAB.theStringArray(i));
                    ASSERT (CA[i].theStringArray() ==
                            CCAB.theStringArray(i));

                    CAB.theModifiableStringArray(i) = XQ;
                    ASSERT (XQ == LX.theStringArray(i));

                    CAB.theModifiableStringArray(i) = YQ;
                    ASSERT (YQ == LY.theStringArray(i));
                  } break;
                  case ElemType::BDEM_DATETIME_ARRAY: {
                    ASSERT (CA[i].theDatetimeArray() ==
                            CAB.theDatetimeArray(i));
                    ASSERT (CA[i].theDatetimeArray() ==
                            CCAB.theDatetimeArray(i));

                    CAB.theModifiableDatetimeArray(i) = XR;
                    ASSERT (XR == LX.theDatetimeArray(i));

                    CAB.theModifiableDatetimeArray(i) = YR;
                    ASSERT (YR == LY.theDatetimeArray(i));
                  } break;
                  case ElemType::BDEM_DATE_ARRAY : {
                    ASSERT (CA[i].theDateArray() == CAB.theDateArray(i));
                    ASSERT (CA[i].theDateArray() == CCAB.theDateArray(i));

                    CAB.theModifiableDateArray(i) = XS;
                    ASSERT (XS == LX.theDateArray(i));

                    CAB.theModifiableDateArray(i) = YS;
                    ASSERT (YS == LY.theDateArray(i));
                  } break;
                  case ElemType::BDEM_TIME_ARRAY : {
                    ASSERT (CA[i].theTimeArray() == CAB.theTimeArray(i));
                    ASSERT (CA[i].theTimeArray() == CCAB.theTimeArray(i));

                    CAB.theModifiableTimeArray(i) = XT;
                    ASSERT (XT == LX.theTimeArray(i));

                    CAB.theModifiableTimeArray(i) = YT;
                    ASSERT (YT == LY.theTimeArray(i));
                  } break;
                  case ElemType::BDEM_LIST : {
                    ASSERT (CA[i].theList() == CAB.theList(i));
                    ASSERT (CA[i].theList() == CCAB.theList(i));

                    CAB.theModifiableList(i) = XU;
                    ASSERT (XU == LX.theList(i));

                    CAB.theModifiableList(i) = YU;
                    ASSERT (YU == LY.theList(i));
                  } break;
                  case ElemType::BDEM_TABLE : {
                    ASSERT (CA[i].theTable() == CAB.theTable(i));
                    ASSERT (CA[i].theTable() == CCAB.theTable(i));

                    CAB.theModifiableTable(i) = XV;
                    ASSERT (XV == LX.theTable(i));

                    CAB.theModifiableTable(i) = YV;
                    ASSERT (YV == LY.theTable(i));
                  } break;
                  case ElemType::BDEM_BOOL : {
                    ASSERT (CA[i].theBool() == CAB.theBool(i));
                    ASSERT (CA[i].theBool() == CCAB.theBool(i));

                    CAB.theModifiableBool(i) = XW;
                    ASSERT (XW == LX.theBool(i));

                    CAB.theModifiableBool(i) = YW;
                    ASSERT (YW == LY.theBool(i));
                  } break;
                  case ElemType::BDEM_DATETIMETZ : {
                    ASSERT (CA[i].theDatetimeTz() == CAB.theDatetimeTz(i));
                    ASSERT (CA[i].theDatetimeTz() == CCAB.theDatetimeTz(i));

                    CAB.theModifiableDatetimeTz(i) = XX;
                    ASSERT (XX == LX.theDatetimeTz(i));

                    CAB.theModifiableDatetimeTz(i) = YX;
                    ASSERT (YX == LY.theDatetimeTz(i));
                  } break;
                  case ElemType::BDEM_DATETZ : {
                    ASSERT (CA[i].theDateTz() == CAB.theDateTz(i));
                    ASSERT (CA[i].theDateTz() == CCAB.theDateTz(i));

                    CAB.theModifiableDateTz(i) = XY;
                    ASSERT (XY == LX.theDateTz(i));

                    CAB.theModifiableDateTz(i) = YY;
                    ASSERT (YY == LY.theDateTz(i));
                  } break;
                  case ElemType::BDEM_TIMETZ : {
                    ASSERT (CA[i].theTimeTz() == CAB.theTimeTz(i));
                    ASSERT (CA[i].theTimeTz() == CCAB.theTimeTz(i));

                    CAB.theModifiableTimeTz(i) = XZ;
                    ASSERT (XZ == LX.theTimeTz(i));

                    CAB.theModifiableTimeTz(i) = YZ;
                    ASSERT (YZ == LY.theTimeTz(i));
                  } break;
                  case ElemType::BDEM_BOOL_ARRAY : {
                    ASSERT (CA[i].theBoolArray() == CAB.theBoolArray(i));
                    ASSERT (CA[i].theBoolArray() == CCAB.theBoolArray(i));

                    CAB.theModifiableBoolArray(i) = Xa;
                    ASSERT (Xa == LX.theBoolArray(i));

                    CAB.theModifiableBoolArray(i) = Ya;
                    ASSERT (Ya == LY.theBoolArray(i));
                  } break;
                  case ElemType::BDEM_DATETIMETZ_ARRAY : {
                    ASSERT (CA[i].theDatetimeTzArray() ==
                            CAB.theDatetimeTzArray(i));
                    ASSERT (CA[i].theDatetimeTzArray() ==
                            CCAB.theDatetimeTzArray(i));

                    CAB.theModifiableDatetimeTzArray(i) = Xb;
                    ASSERT (Xb == LX.theDatetimeTzArray(i));

                    CAB.theModifiableDatetimeTzArray(i) = Yb;
                    ASSERT (Yb == LY.theDatetimeTzArray(i));
                  } break;
                  case ElemType::BDEM_DATETZ_ARRAY : {
                    ASSERT (CA[i].theDateTzArray() == CAB.theDateTzArray(i));
                    ASSERT (CA[i].theDateTzArray() == CCAB.theDateTzArray(i));

                    CAB.theModifiableDateTzArray(i) = Xc;
                    ASSERT (Xc == LX.theDateTzArray(i));

                    CAB.theModifiableDateTzArray(i) = Yc;
                    ASSERT (Yc == LY.theDateTzArray(i));
                  } break;
                  case ElemType::BDEM_TIMETZ_ARRAY : {
                    ASSERT (CA[i].theTimeTzArray() == CAB.theTimeTzArray(i));
                    ASSERT (CA[i].theTimeTzArray() == CCAB.theTimeTzArray(i));

                    CAB.theModifiableTimeTzArray(i) = Xd;
                    ASSERT (Xd == LX.theTimeTzArray(i));

                    CAB.theModifiableTimeTzArray(i) = Yd;
                    ASSERT (Yd == LY.theTimeTzArray(i));
                  } break;
                  case ElemType::BDEM_CHOICE : {
                    ASSERT (CA[i].theChoice() == CAB.theChoice(i));
                    ASSERT (CA[i].theChoice() == CCAB.theChoice(i));

                    CAB.theModifiableChoice(i) = Xe;
                    ASSERT (Xe == LX.theChoice(i));

                    CAB.theModifiableChoice(i) = Ye;
                    ASSERT (Ye == LY.theChoice(i));
                  } break;
                  case ElemType::BDEM_CHOICE_ARRAY : {
                    ASSERT (CA[i].theChoiceArray() == CAB.theChoiceArray(i));
                    ASSERT (CA[i].theChoiceArray() == CCAB.theChoiceArray(i));

                    CAB.theModifiableChoiceArray(i) = Xf;
                    ASSERT (Xf == LX.theChoiceArray(i));

                    CAB.theModifiableChoiceArray(i) = Yf;
                    ASSERT (Yf == LY.theChoiceArray(i));
                  } break;
                  default: {
                    cout << "unidentified element type: " << type << endl;
                    ASSERT(0);
                  } break;
                }
                ASSERT (LY[i] == CAB.selection(i));
                ASSERT (LY[i] == CCAB.selection(i));
            }
        }
      }

DEFINE_TEST_CASE(10) {
        // --------------------------------------------------------------------
        // TESTING CHOICE BINDING ACCESSORS
        //   Test Choice Binding accessor methods.
        //
        // Concerns:
        //   - That value-returning methods return the correct result.
        //   - That references of the appropriate type and value are returned.
        //   - That references of the appropriate 'const'-ness are returned.
        //
        // Plan:
        //   Create a 'bdem_Choice' containing a selection for each
        //   'bdem_ElemType::Type'.  Using that 'bdem_Choice', create instances
        //   of both Choice binding classes.  Loop through the all the types
        //   using the bdem_Choice object to set the selection and value then
        //   access the 'bdem_Choice' using the non-modifiable accessor
        //   functions provided by the choice bindings and verify that the
        //   references returned match the original data.  Use the accessors
        //   that return modifiable references to change the value of the
        //   'bdem_List'.  Check that the value of the original 'bdem_List'
        //   has been correctly modified via the bindings.  As a negative
        //  test, confirm that the accessors of  "const" row bindings return
        //  non-modifiable references.
        //
        // Testing:
        //
        //             bdem_ConstChoiceBinding
        //             ~~~~~~~~~~~~~~~
        //   bdem_ConstElemRef selection() const;
        //   bdem_ElemType::Type selectionType(int index) const;
        //   bdem_ElemType::Type selectionType(const char *elementName) const;
        //   int selector() const;
        //   int selectorId() const;
        //   const char *selectorName() const;
        //   char& theChar() const;
        //   short& theShort() const;
        //   int& theInt() const;
        //   Int64& theInt64() const;
        //   float& theFloat() const;
        //   double& theDouble() const;
        //   string& theString() const;
        //   bdet_Datetime& theDatetime() const;
        //   bdet_Time& theTime() const;
        //   bdet_Date& theDate() const;
        //   vector<char>& theCharArray() const;
        //   vector<short>& theShortArray() const;
        //   vector<int>& theIntArray() const;
        //   vector<Int64>& theInt64Array() const;
        //   vector<float>& theFloatArray() const;
        //   vector<double>& theDoubleArray() const;
        //   vector<string>& theStringArray() const;
        //   vector<Datetime>& theDatetimeArray() const;
        //   vector<Date>& theDateArray() const;
        //   vector<Time>& theTimeArray() const;
        //   bdem_List& theList() const;
        //   bdem_Table& theTable() const;
        //   bdem_Bool&  theBool()  const;
        //   bdem_DatetimeTz&  theDatetimeTz()  const;
        //   bdem_DateTz&  theDateTz()  const;
        //   bdem_TimeTz&  theTimeTz()  const;
        //   bdem_BoolArray&  theBoolArray()  const;
        //   bdem_DatetimeTzArray&  theDatetimeTzArray()  const;
        //   bdem_DateTzArray&  theDateTzArray()  const;
        //   bdem_TimeTzArray&  theTimeTzArray()  const;
        //   bdem_Choice&  theChoice()  const;
        //   bdem_ChoiceArray&  theChoiceArray()  const;
        //
        //             bdem_ChoiceBinding
        //             ~~~~~~~~~~~~~~~
        //   bdem_ElemRef selection() const;
        //   bdem_ElemType::Type selectionType(int index) const;
        //   bdem_ElemType::Type selectionType(const char *elementName) const;
        //   int selector() const;
        //   int selectorId() const;
        //   const char *selectorName() const;
        //   char& theChar() const;
        //   short& theShort() const;
        //   int& theInt() const;
        //   Int64& theInt64() const;
        //   float& theFloat() const;
        //   double& theDouble() const;
        //   string& theString() const;
        //   bdet_Datetime& theDatetime() const;
        //   bdet_Time& theTime() const;
        //   bdet_Date& theDate() const;
        //   vector<char>& theCharArray() const;
        //   vector<short>& theShortArray() const;
        //   vector<int>& theIntArray() const;
        //   vector<Int64>& theInt64Array() const;
        //   vector<float>& theFloatArray() const;
        //   vector<double>& theDoubleArray() const;
        //   vector<string>& theStringArray() const;
        //   vector<Datetime>& theDatetimeArray() const;
        //   vector<Date>& theDateArray() const;
        //   vector<Time>& theTimeArray() const;
        //   bdem_List& theList() const;
        //   bdem_Table& theTable() const;
        //   bdem_Bool&  theBool()  const;
        //   bdem_DatetimeTz&  theDatetimeTz()  const;
        //   bdem_DateTz&  theDateTz()  const;
        //   bdem_TimeTz&  theTimeTz()  const;
        //   bdem_BoolArray&  theBoolArray()  const;
        //   bdem_DatetimeTzArray&  theDatetimeTzArray()  const;
        //   bdem_DateTzArray&  theDateTzArray()  const;
        //   bdem_TimeTzArray&  theTimeTzArray()  const;
        //   bdem_Choice&  theChoice()  const;
        //   bdem_ChoiceArray&  theChoiceArray()  const;
        // --------------------------------------------------------------------

        DECLARE_TEST_LIST_OBJECTS

        if (verbose) cout << "Testing Choice Binding Accessors" << endl
                          << "=============================" << endl;

        {
            const char *testSpec =
      ":a?AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZzaAbBcCdDeEfF";

            Schema S;
            createSchema(&S,testSpec);
            const RecDef *rec = &S.record(S.numRecords() - 1);
            Choice    mC; const Choice& C = mC;
            createChoice(&mC, rec);

            CBind     cb(&mC,rec);    const  CBind&  CB = cb;
            CCBind    ccb(&C,rec);   const  CCBind& CCB = ccb;

            // SANITY CHECK ON SAMPLE DATA
            ASSERT (rec->numFields() == NUM_TYPES);

            if (veryVerbose) {
                cout << "Testing selector and selection accessors" << endl;
            }
            for (int i = 0; i < rec->numFields(); ++i ) {
                const bdem_FieldDef &field = rec->field(i);
                const char *fieldName = rec->fieldName(i);
                const int  fieldId = rec->fieldId(i);

                // SANITY CHECK ON SAMPLE DATA
                ASSERT (field.elemType()   == C.selectionType(i));
                ASSERT (field.elemType()   == LX.elemType(i));
                ASSERT (i == fieldId);

                // set the selection to that type
                if (veryVeryVerbose) { P(mC) P(LX[i]); }

                mC.makeSelection(i).replaceValue(LX[i]);

                if (veryVeryVerbose) { P(mC) P(LX[i]); }

                ASSERT(C.selectionType(i) == CB.selectionType(i));
                ASSERT(C.selectionType(i) == CB.selectionType(fieldName));
                ASSERT(C.selector()       == CB.selector());
                ASSERT(fieldId            == CB.selectorId());
                ASSERT(fieldName          == CB.selectorName());

                ASSERT(C.selectionType(i) == CCB.selectionType(i));
                ASSERT(C.selectionType(i) == CCB.selectionType(fieldName));
                ASSERT(C.selector()       == CCB.selector());
                ASSERT(fieldId            == CCB.selectorId());
                ASSERT(fieldName          == CCB.selectorName());

                ElemType::Type type = C.selectionType(i);
                switch(type) {
                  // In each case, test the accessors and modify the value.
                  // The modify test is outside the switch.
                  case ElemType::BDEM_CHAR     : {
                    ASSERT (C.theChar() == CB.theChar());
                    ASSERT (C.theChar() == CCB.theChar());

                    CB.theModifiableChar() = XA;
                    ASSERT (XA == LX.theChar(i));

                    CB.theModifiableChar() = YA;
                    ASSERT (YA == LY.theChar(i));
                  } break;
                  case ElemType::BDEM_SHORT : {
                    ASSERT (C.theShort() == CB.theShort());
                    ASSERT (C.theShort() == CCB.theShort());

                    CB.theModifiableShort() = XB;
                    ASSERT (XB == LX.theShort(i));

                    CB.theModifiableShort() = YB;
                    ASSERT (YB == LY.theShort(i));
                  } break;
                  case ElemType::BDEM_INT : {
                    ASSERT (C.theInt() == CB.theInt());
                    ASSERT (C.theInt() == CCB.theInt());

                    CB.theModifiableInt() = XC;
                    ASSERT (XC == LX.theInt(i));

                    CB.theModifiableInt() = YC;
                    ASSERT (YC == LY.theInt(i));
                  } break;
                  case ElemType::BDEM_INT64 : {
                    ASSERT (C.theInt64() == CB.theInt64());
                    ASSERT (C.theInt64() == CCB.theInt64());

                    CB.theModifiableInt64() = XD;
                    ASSERT (XD == LX.theInt64(i));

                    CB.theModifiableInt64() = YD;
                    ASSERT (YD == LY.theInt64(i));
                  } break;
                  case ElemType::BDEM_FLOAT : {
                    ASSERT (C.theFloat() == CB.theFloat());
                    ASSERT (C.theFloat() == CCB.theFloat());

                    CB.theModifiableFloat() = XE;
                    ASSERT (XE == LX.theFloat(i));

                    CB.theModifiableFloat() = YE;
                    ASSERT (YE == LY.theFloat(i));
                  } break;
                  case ElemType::BDEM_DOUBLE : {
                    ASSERT (C.theDouble() == CB.theDouble());
                    ASSERT (C.theDouble() == CCB.theDouble());

                    CB.theModifiableDouble() = XF;
                    ASSERT (XF == LX.theDouble(i));

                    CB.theModifiableDouble() = YF;
                    ASSERT (YF == LY.theDouble(i));
                  } break;
                  case ElemType::BDEM_STRING : {
                    ASSERT (C.theString() == CB.theString());
                    ASSERT (C.theString() == CCB.theString());

                    CB.theModifiableString() = XG;
                    ASSERT (XG == LX.theString(i));

                    CB.theModifiableString() = YG;
                    ASSERT (YG == LY.theString(i));
                  } break;
                  case ElemType::BDEM_DATETIME : {
                    ASSERT (C.theDatetime() == CB.theDatetime());
                    ASSERT (C.theDatetime() == CCB.theDatetime());

                    CB.theModifiableDatetime() = XH;
                    ASSERT (XH == LX.theDatetime(i));

                    CB.theModifiableDatetime() = YH;
                    ASSERT (YH == LY.theDatetime(i));
                  } break;
                  case ElemType::BDEM_DATE : {
                    ASSERT (C.theDate() == CB.theDate());
                    ASSERT (C.theDate() == CCB.theDate());

                    CB.theModifiableDate() = XI;
                    ASSERT (XI == LX.theDate(i));

                    CB.theModifiableDate() = YI;
                    ASSERT (YI == LY.theDate(i));
                  } break;
                  case ElemType::BDEM_TIME : {
                    ASSERT (C.theTime() == CB.theTime());
                    ASSERT (C.theTime() == CCB.theTime());

                    CB.theModifiableTime() = XJ;
                    ASSERT (XJ == LX.theTime(i));

                    CB.theModifiableTime() = YJ;
                    ASSERT (YJ == LY.theTime(i));
                  } break;
                  case ElemType::BDEM_CHAR_ARRAY : {
                    ASSERT (C.theCharArray() == CB.theCharArray());
                    ASSERT (C.theCharArray() == CCB.theCharArray());

                    CB.theModifiableCharArray() = XK;
                    ASSERT (XK == LX.theCharArray(i));

                    CB.theModifiableCharArray() = YK;
                    ASSERT (YK == LY.theCharArray(i));
                  } break;
                  case ElemType::BDEM_SHORT_ARRAY : {
                    ASSERT (C.theShortArray() == CB.theShortArray());
                    ASSERT (C.theShortArray() == CCB.theShortArray());

                    CB.theModifiableShortArray() = XL;
                    ASSERT (XL == LX.theShortArray(i));

                    CB.theModifiableShortArray() = YL;
                    ASSERT (YL == LY.theShortArray(i));
                  } break;
                  case ElemType::BDEM_INT_ARRAY : {
                    ASSERT (C.theIntArray() == CB.theIntArray());
                    ASSERT (C.theIntArray() == CCB.theIntArray());

                    CB.theModifiableIntArray() = XM;
                    ASSERT (XM == LX.theIntArray(i));

                    CB.theModifiableIntArray() = YM;
                    ASSERT (YM == LY.theIntArray(i));
                  } break;
                  case ElemType::BDEM_INT64_ARRAY : {
                    ASSERT (C.theInt64Array() == CB.theInt64Array());
                    ASSERT (C.theInt64Array() == CCB.theInt64Array());

                    CB.theModifiableInt64Array() = XN;
                    ASSERT (XN == LX.theInt64Array(i));

                    CB.theModifiableInt64Array() = YN;
                    ASSERT (YN == LY.theInt64Array(i));
                  } break;
                  case ElemType::BDEM_FLOAT_ARRAY : {
                    ASSERT (C.theFloatArray() == CB.theFloatArray());
                    ASSERT (C.theFloatArray() == CCB.theFloatArray());

                    CB.theModifiableFloatArray() = XO;
                    ASSERT (XO == LX.theFloatArray(i));

                    CB.theModifiableFloatArray() = YO;
                    ASSERT (YO == LY.theFloatArray(i));
                  } break;
                  case ElemType::BDEM_DOUBLE_ARRAY : {
                    ASSERT (C.theDoubleArray() == CB.theDoubleArray());
                    ASSERT (C.theDoubleArray() == CCB.theDoubleArray());

                    CB.theModifiableDoubleArray() = XP;
                    ASSERT (XP == LX.theDoubleArray(i));

                    CB.theModifiableDoubleArray() = YP;
                    ASSERT (YP == LY.theDoubleArray(i));
                  } break;
                  case ElemType::BDEM_STRING_ARRAY : {
                    ASSERT (C.theStringArray() == CB.theStringArray());
                    ASSERT (C.theStringArray() == CCB.theStringArray());

                    CB.theModifiableStringArray() = XQ;
                    ASSERT (XQ == LX.theStringArray(i));

                    CB.theModifiableStringArray() = YQ;
                    ASSERT (YQ == LY.theStringArray(i));
                  } break;
                  case ElemType::BDEM_DATETIME_ARRAY: {
                    ASSERT (C.theDatetimeArray() == CB.theDatetimeArray());
                    ASSERT (C.theDatetimeArray() == CCB.theDatetimeArray());

                    CB.theModifiableDatetimeArray() = XR;
                    ASSERT (XR == LX.theDatetimeArray(i));

                    CB.theModifiableDatetimeArray() = YR;
                    ASSERT (YR == LY.theDatetimeArray(i));
                  } break;
                  case ElemType::BDEM_DATE_ARRAY : {
                    ASSERT (C.theDateArray() == CB.theDateArray());
                    ASSERT (C.theDateArray() == CCB.theDateArray());

                    CB.theModifiableDateArray() = XS;
                    ASSERT (XS == LX.theDateArray(i));

                    CB.theModifiableDateArray() = YS;
                    ASSERT (YS == LY.theDateArray(i));
                  } break;
                  case ElemType::BDEM_TIME_ARRAY : {
                    ASSERT (C.theTimeArray() == CB.theTimeArray());
                    ASSERT (C.theTimeArray() == CCB.theTimeArray());

                    CB.theModifiableTimeArray() = XT;
                    ASSERT (XT == LX.theTimeArray(i));

                    CB.theModifiableTimeArray() = YT;
                    ASSERT (YT == LY.theTimeArray(i));
                  } break;
                  case ElemType::BDEM_LIST : {
                    ASSERT (C.theList() == CB.theList());
                    ASSERT (C.theList() == CCB.theList());

                    CB.theModifiableList() = XU;
                    ASSERT (XU == LX.theList(i));

                    CB.theModifiableList() = YU;
                    ASSERT (YU == LY.theList(i));
                  } break;
                  case ElemType::BDEM_TABLE : {
                    ASSERT (C.theTable() == CB.theTable());
                    ASSERT (C.theTable() == CCB.theTable());

                    CB.theModifiableTable() = XV;
                    ASSERT (XV == LX.theTable(i));

                    CB.theModifiableTable() = YV;
                    ASSERT (YV == LY.theTable(i));
                  } break;
                  case ElemType::BDEM_BOOL : {
                    ASSERT (C.theBool() == CB.theBool());
                    ASSERT (C.theBool() == CCB.theBool());

                    CB.theModifiableBool() = XW;
                    ASSERT (XW == LX.theBool(i));

                    CB.theModifiableBool() = YW;
                    ASSERT (YW == LY.theBool(i));
                  } break;
                  case ElemType::BDEM_DATETIMETZ : {
                    ASSERT (C.theDatetimeTz() == CB.theDatetimeTz());
                    ASSERT (C.theDatetimeTz() == CCB.theDatetimeTz());

                    CB.theModifiableDatetimeTz() = XX;
                    ASSERT (XX == LX.theDatetimeTz(i));

                    CB.theModifiableDatetimeTz() = YX;
                    ASSERT (YX == LY.theDatetimeTz(i));
                  } break;
                  case ElemType::BDEM_DATETZ : {
                    ASSERT (C.theDateTz() == CB.theDateTz());
                    ASSERT (C.theDateTz() == CCB.theDateTz());

                    CB.theModifiableDateTz() = XY;
                    ASSERT (XY == LX.theDateTz(i));

                    CB.theModifiableDateTz() = YY;
                    ASSERT (YY == LY.theDateTz(i));
                  } break;
                  case ElemType::BDEM_TIMETZ : {
                    ASSERT (C.theTimeTz() == CB.theTimeTz());
                    ASSERT (C.theTimeTz() == CCB.theTimeTz());

                    CB.theModifiableTimeTz() = XZ;
                    ASSERT (XZ == LX.theTimeTz(i));

                    CB.theModifiableTimeTz() = YZ;
                    ASSERT (YZ == LY.theTimeTz(i));
                  } break;
                  case ElemType::BDEM_BOOL_ARRAY : {
                    ASSERT (C.theBoolArray() == CB.theBoolArray());
                    ASSERT (C.theBoolArray() == CCB.theBoolArray());

                    CB.theModifiableBoolArray() = Xa;
                    ASSERT (Xa == LX.theBoolArray(i));

                    CB.theModifiableBoolArray() = Ya;
                    ASSERT (Ya == LY.theBoolArray(i));
                  } break;
                  case ElemType::BDEM_DATETIMETZ_ARRAY : {
                    ASSERT (C.theDatetimeTzArray() == CB.theDatetimeTzArray());
                    ASSERT (C.theDatetimeTzArray() ==CCB.theDatetimeTzArray());

                    CB.theModifiableDatetimeTzArray() = Xb;
                    ASSERT (Xb == LX.theDatetimeTzArray(i));

                    CB.theModifiableDatetimeTzArray() = Yb;
                    ASSERT (Yb == LY.theDatetimeTzArray(i));
                  } break;
                  case ElemType::BDEM_DATETZ_ARRAY : {
                    ASSERT (C.theDateTzArray() == CB.theDateTzArray());
                    ASSERT (C.theDateTzArray() == CCB.theDateTzArray());

                    CB.theModifiableDateTzArray() = Xc;
                    ASSERT (Xc == LX.theDateTzArray(i));

                    CB.theModifiableDateTzArray() = Yc;
                    ASSERT (Yc == LY.theDateTzArray(i));
                  } break;
                  case ElemType::BDEM_TIMETZ_ARRAY : {
                    ASSERT (C.theTimeTzArray() == CB.theTimeTzArray());
                    ASSERT (C.theTimeTzArray() == CCB.theTimeTzArray());

                    CB.theModifiableTimeTzArray() = Xd;
                    ASSERT (Xd == LX.theTimeTzArray(i));

                    CB.theModifiableTimeTzArray() = Yd;
                    ASSERT (Yd == LY.theTimeTzArray(i));
                  } break;
                  case ElemType::BDEM_CHOICE : {
                    ASSERT (C.theChoice() == CB.theChoice());
                    ASSERT (C.theChoice() == CCB.theChoice());

                    CB.theModifiableChoice() = Xe;
                    ASSERT (Xe == LX.theChoice(i));

                    CB.theModifiableChoice() = Ye;
                    ASSERT (Ye == LY.theChoice(i));
                  } break;
                  case ElemType::BDEM_CHOICE_ARRAY : {
                    ASSERT (C.theChoiceArray() == CB.theChoiceArray());
                    ASSERT (C.theChoiceArray() == CCB.theChoiceArray());

                    CB.theModifiableChoiceArray() = Xf;
                    ASSERT (Xf == LX.theChoiceArray(i));

                    CB.theModifiableChoiceArray() = Yf;
                    ASSERT (Yf == LY.theChoiceArray(i));
                  } break;
                  default: {
                    cout << "unidentified element type: " << type << endl;
                    ASSERT(0);
                  } break;
                }
                ASSERT (LY[i] == CB.selection());
                ASSERT (LY[i] == CCB.selection());
            }
        }
      }

DEFINE_TEST_CASE(9) {
        // --------------------------------------------------------------------
        // TESTING TABLE BINDING ACCESSORS
        //   Test Table Binding accessor methods.
        //
        // Concerns:
        //   - That value-returning methods return the correct result.
        //   - That references of the appropriate type and value are returned.
        //   - That references of the appropriate 'const'-ness are returned.
        //
        // Plan:
        //   Create a 'bdem_Table' having a column for each
        //   'bdem_ElemType::Type' and containing at least one row of data.
        //   Using that 'bdem_Table', create instances of all 4 table binding
        //   classes.  Access the elements in the original 'bdem_Table' using
        //   the non-modifiable accessor functions provided by the table
        //   bindings and verify that the references returned match the
        //   original data.  Use the accessors that return modifiable
        //   references to change the value of the 'bdem_Table'.  Check that
        //   the value of the original 'bdem_Table' has been correctly modified
        //   via the bindings.  As a negative test, confirm that the "const"
        //   table bindings return non-modifiable references.
        //
        // Testing:
        //             bdem_ConstTableBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstElemRef element(int, *name) const;
        //   bdem_ConstElemRef element(int, int) const;
        //   bdem_ElemType::Type elemType(*name) const;
        //   bdem_ElemType::Type elemType(int) const;
        //   const char& theChar(int, *name) const;
        //   const short& theShort(int, *name) const;
        //   const int& theInt(int, *name) const;
        //   const Int64& theInt64(int, *name) const;
        //   const float& theFloat(int, *name) const;
        //   const double& theDouble(int, *name) const;
        //   const string& theString(int, *name) const;
        //   const bdet_Datetime& theDatetime(int, *name) const;
        //   const bdet_Date& theDate(int, *name) const;
        //   const bdet_Time& theTime(int, *name) const;
        //   const vector<char>& theCharArray(int, *name) const;
        //   const vector<short>& theShortArray(int, *name) const;
        //   const vector<int>& theIntArray(int, *name) const;
        //   const vector<Int64>& theInt64Array(int, *name) const;
        //   const vector<float>& theFloatArray(int, *name) const;
        //   const vector<double>& theDoubleArray(int, *name) const;
        //   const vector<string>& theStringArray(int, *name) const;
        //   const vector<Datetime>& theDatetimeArray(int, *name) const;
        //   const vector<Date>& theDateArray(int, *name) const;
        //   const vector<Time>& theTimeArray(int, *name) const;
        //   const bdem_List& theList(int, *name) const;
        //   const bdem_Table& theTable(int, *name) const;
        //   const bdem_Bool&  theBool(*name)  const;
        //   const bdem_DatetimeTz&  theDatetimeTz(*name)  const;
        //   const bdem_DateTz&  theDateTz(*name)  const;
        //   const bdem_TimeTz&  theTimeTz(*name)  const;
        //   const bdem_BoolArray&  theBoolArray(*name)  const;
        //   const bdem_DatetimeTzArray&  theDatetimeTzArray(*name)  const;
        //   const bdem_DateTzArray&  theDateTzArray(*name)  const;
        //   const bdem_TimeTzArray&  theTimeTzArray(*name)  const;
        //   const bdem_Choice&  theChoice(*name)  const;
        //   const bdem_ChoiceArray&  theChoiceArray(*name)  const;
        //
        //             bdem_TableBinding
        //             ~~~~~~~~~~~~~~~~~
        //   bdem_ElemRef element(int, *name) const;
        //   bdem_ElemRef element(int, int) const;
        //   char& theChar(int, *name) const;
        //   short& theShort(int, *name) const;
        //   int& theInt(int, *name) const;
        //   Int64& theInt64(int, *name) const;
        //   float& theFloat(int, *name) const;
        //   double& theDouble(int, *name) const;
        //   string& theString(int, *name) const;
        //   bdet_Datetime& theDatetime(int, *name) const;
        //   bdet_Date& theDate(int, *name) const;
        //   bdet_Time& theTime(int, *name) const;
        //   vector<char>& theCharArray(int, *name) const;
        //   vector<short>& theShortArray(int, *name) const;
        //   vector<int>& theIntArray(int, *name) const;
        //   vector<Int64>& theInt64Array(int, *name) const;
        //   vector<float>& theFloatArray(int, *name) const;
        //   vector<double>& theDoubleArray(int, *name) const;
        //   vector<string>& theStringArray(int, *name) const;
        //   vector<Datetime>& theDatetimeArray(int, *name) const;
        //   vector<Date>& theDateArray(int, *name) const;
        //   vector<Time>& theTimeArray(int, *name) const;
        //   bdem_List& theList(int, *name) const;
        //   bdem_Table& theTable(int, *name) const;
        //   bdem_Bool&  theBool(*name)  const;
        //   bdem_DatetimeTz&  theDatetimeTz(*name)  const;
        //   bdem_DateTz&  theDateTz(*name)  const;
        //   bdem_TimeTz&  theTimeTz(*name)  const;
        //   bdem_BoolArray&  theBoolArray(*name)  const;
        //   bdem_DatetimeTzArray&  theDatetimeTzArray(*name)  const;
        //   bdem_DateTzArray&  theDateTzArray(*name)  const;
        //   bdem_TimeTzArray&  theTimeTzArray(*name)  const;
        //   bdem_Choice&  theChoice(*name)  const;
        //   bdem_ChoiceArray&  theChoiceArray(*name)  const;
        // --------------------------------------------------------------------

        //--------------------------------------------------------------------
        // Convenient test table objects
        //--------------------------------------------------------------------

        DECLARE_TEST_SCHEMA
        DECLARE_TEST_LIST_OBJECTS

        const ElemType::Type tableCols[] = {
            ElemType::BDEM_CHAR,
            ElemType::BDEM_SHORT,
            ElemType::BDEM_INT,
            ElemType::BDEM_INT64,
            ElemType::BDEM_FLOAT,
            ElemType::BDEM_DOUBLE,
            ElemType::BDEM_STRING,
            ElemType::BDEM_DATETIME,
            ElemType::BDEM_DATE,
            ElemType::BDEM_TIME,
            ElemType::BDEM_CHAR_ARRAY,
            ElemType::BDEM_SHORT_ARRAY,
            ElemType::BDEM_INT_ARRAY,
            ElemType::BDEM_INT64_ARRAY,
            ElemType::BDEM_FLOAT_ARRAY,
            ElemType::BDEM_DOUBLE_ARRAY,
            ElemType::BDEM_STRING_ARRAY,
            ElemType::BDEM_DATETIME_ARRAY,
            ElemType::BDEM_DATE_ARRAY,
            ElemType::BDEM_TIME_ARRAY,
            ElemType::BDEM_LIST,
            ElemType::BDEM_TABLE,
            ElemType::BDEM_BOOL,
            ElemType::BDEM_DATETIMETZ,
            ElemType::BDEM_DATETZ,
            ElemType::BDEM_TIMETZ,
            ElemType::BDEM_BOOL_ARRAY,
            ElemType::BDEM_DATETIMETZ_ARRAY,
            ElemType::BDEM_DATETZ_ARRAY,
            ElemType::BDEM_TIMETZ_ARRAY,
            ElemType::BDEM_CHOICE,
            ElemType::BDEM_CHOICE_ARRAY
        };
        const int numTableCols = sizeof(tableCols) / sizeof(*tableCols);

        Table TX_(tableCols, numTableCols);  const Table& TX = TX_;

        TX_.appendRow(LX);
        TX_.appendRow(LY);

        // --------------------------- Code begins ---------------------------

        if (verbose) cout << "Testing Table Binding Accessors" << endl
                          << "===============================" << endl;

        {
                   CTblBind   ctb(&TX, pR);
                    TblBind    tb(&TX_, pR);

            const  CTblBind&  CTB =  ctb;
            const   TblBind&   TB =   tb;

            ASSERT(TX.numRows() ==  CTB.numRows());
            ASSERT(TX.numRows() ==   TB.numRows());

            ASSERT(TX.numColumns() ==  CTB.numColumns());
            ASSERT(TX.numColumns() ==   TB.numColumns());

            if (veryVerbose) {
                cout << "Testing non-modifiable accessors." << endl;
            }

            if (veryVerbose) { T_ cout << "Bindings." << endl; }

            for (int i = 0; i < TX.numRows(); ++i) {
                ASSERT(TX[i][0].theChar()     == CTB.theChar(i, "a"));
                ASSERT(TX[i][1].theShort()    == CTB.theShort(i, "b"));
                ASSERT(TX[i][2].theInt()      == CTB.theInt(i, "c"));
                ASSERT(TX[i][3].theInt64()    == CTB.theInt64(i, "d"));
                ASSERT(TX[i][4].theFloat()    == CTB.theFloat(i, "e"));
                ASSERT(TX[i][5].theDouble()   == CTB.theDouble(i, "f"));
                ASSERT(TX[i][6].theString()   == CTB.theString(i, "g"));
                ASSERT(TX[i][7].theDatetime() == CTB.theDatetime(i, "h"));
                ASSERT(TX[i][8].theDate()     == CTB.theDate(i, "i"));
                ASSERT(TX[i][9].theTime()     == CTB.theTime(i, "j"));
                ASSERT(TX[i][10].theCharArray()
                                              == CTB.theCharArray(i, "k"));
                ASSERT(TX[i][11].theShortArray()
                                              == CTB.theShortArray(i, "l"));
                ASSERT(TX[i][12].theIntArray()
                                              == CTB.theIntArray(i, "m"));
                ASSERT(TX[i][13].theInt64Array()
                                              == CTB.theInt64Array(i, "n"));
                ASSERT(TX[i][14].theFloatArray()
                                              == CTB.theFloatArray(i, "o"));
                ASSERT(TX[i][15].theDoubleArray()
                                              == CTB.theDoubleArray(i, "p"));
                ASSERT(TX[i][16].theStringArray()
                                              == CTB.theStringArray(i, "q"));
                ASSERT(TX[i][17].theDatetimeArray()
                                              == CTB.theDatetimeArray(i, "r"));
                ASSERT(TX[i][18].theDateArray()
                                              == CTB.theDateArray(i, "s"));
                ASSERT(TX[i][19].theTimeArray()
                                              == CTB.theTimeArray(i, "t"));
                ASSERT(TX[i][20].theList()    == CTB.theList(i, "u"));
                ASSERT(TX[i][21].theTable()   == CTB.theTable(i, "v"));

                ASSERT(TX[i][22].theBool()    == CTB.theBool(i, "w"));
                ASSERT(TX[i][23].theDatetimeTz() == CTB.theDatetimeTz(i, "x"));
                ASSERT(TX[i][24].theDateTz()     == CTB.theDateTz(i, "y"));
                ASSERT(TX[i][25].theTimeTz()     == CTB.theTimeTz(i, "z"));
                ASSERT(TX[i][26].theBoolArray()  == CTB.theBoolArray(i, "A"));
                ASSERT(TX[i][27].theDatetimeTzArray()
                                       == CTB.theDatetimeTzArray(i, "B"));
                ASSERT(TX[i][28].theDateTzArray()
                                       == CTB.theDateTzArray(i, "C"));
                ASSERT(TX[i][29].theTimeTzArray()
                                        == CTB.theTimeTzArray(i, "D"));
                ASSERT(TX[i][30].theChoice()     == CTB.theChoice(i, "E"));
                ASSERT(TX[i][31].theChoiceArray()
                                        == CTB.theChoiceArray(i, "F"));

                ASSERT(TX[i][0].theChar()     ==  TB.theChar(i, "a"));
                ASSERT(TX[i][1].theShort()    ==  TB.theShort(i, "b"));
                ASSERT(TX[i][2].theInt()      ==  TB.theInt(i, "c"));
                ASSERT(TX[i][3].theInt64()    ==  TB.theInt64(i, "d"));
                ASSERT(TX[i][4].theFloat()    ==  TB.theFloat(i, "e"));
                ASSERT(TX[i][5].theDouble()   ==  TB.theDouble(i, "f"));
                ASSERT(TX[i][6].theString()   ==  TB.theString(i, "g"));
                ASSERT(TX[i][7].theDatetime() ==  TB.theDatetime(i, "h"));
                ASSERT(TX[i][8].theDate()     ==  TB.theDate(i, "i"));
                ASSERT(TX[i][9].theTime()     ==  TB.theTime(i, "j"));
                ASSERT(TX[i][10].theCharArray()
                                              ==  TB.theCharArray(i, "k"));
                ASSERT(TX[i][11].theShortArray()
                                              ==  TB.theShortArray(i, "l"));
                ASSERT(TX[i][12].theIntArray()
                                              ==  TB.theIntArray(i, "m"));
                ASSERT(TX[i][13].theInt64Array()
                                              ==  TB.theInt64Array(i, "n"));
                ASSERT(TX[i][14].theFloatArray()
                                              ==  TB.theFloatArray(i, "o"));
                ASSERT(TX[i][15].theDoubleArray()
                                              ==  TB.theDoubleArray(i, "p"));
                ASSERT(TX[i][16].theStringArray()
                                              ==  TB.theStringArray(i, "q"));
                ASSERT(TX[i][17].theDatetimeArray()
                                              ==  TB.theDatetimeArray(i, "r"));
                ASSERT(TX[i][18].theDateArray()
                                              ==  TB.theDateArray(i, "s"));
                ASSERT(TX[i][19].theTimeArray()
                                              ==  TB.theTimeArray(i, "t"));
                ASSERT(TX[i][20].theList()    ==  TB.theList(i, "u"));
                ASSERT(TX[i][21].theTable()   ==  TB.theTable(i, "v"));

                ASSERT(TX[i][22].theBool()       == TB.theBool(i, "w"));
                ASSERT(TX[i][23].theDatetimeTz() == TB.theDatetimeTz(i, "x"));
                ASSERT(TX[i][24].theDateTz()     == TB.theDateTz(i, "y"));
                ASSERT(TX[i][25].theTimeTz()     == TB.theTimeTz(i, "z"));
                ASSERT(TX[i][26].theBoolArray()  == TB.theBoolArray(i, "A"));
                ASSERT(TX[i][27].theDatetimeTzArray()
                       == TB.theDatetimeTzArray(i, "B"));
                ASSERT(TX[i][28].theDateTzArray()== TB.theDateTzArray(i, "C"));
                ASSERT(TX[i][29].theTimeTzArray()== TB.theTimeTzArray(i, "D"));
                ASSERT(TX[i][30].theChoice()     == TB.theChoice(i, "E"));
                ASSERT(TX[i][31].theChoiceArray()== TB.theChoiceArray(i, "F"));
                for (int j = 0; j < TX.numColumns(); ++j) {
                    ASSERT(TX[i][j]  == CTB.element(i, j));
                    ASSERT(TX_[i][j] ==  TB.element(i, j));
                    ASSERT(TX[i][j]  == CTB.element(i, fieldNames[j]));
                    ASSERT(TX_[i][j] ==  TB.element(i, fieldNames[j]));
                }
            }

            for (int j = 0; j < TX.numColumns(); ++j) {
                ASSERT(TX.columnType(j) ==  CTB.elemType(j));
                ASSERT(TX.columnType(j) ==  CTB.elemType(fieldNames[j]));
            }

            if (veryVerbose) {
                cout << "Testing modifiable accessors." << endl;
            }
            {
                if (veryVerbose) { T_ cout << "Bindings." << endl; }

                TB.theModifiableChar(0, "a")          = YA;
                TB.theModifiableShort(0, "b")         = YB;
                TB.theModifiableInt(0, "c")           = YC;
                TB.theModifiableInt64(0, "d")         = YD;
                TB.theModifiableFloat(0, "e")         = YE;
                TB.theModifiableDouble(0, "f")        = YF;
                TB.theModifiableString(0, "g")        = YG;
                TB.theModifiableDatetime(0, "h")      = YH;
                TB.theModifiableDate(0, "i")          = YI;
                TB.theModifiableTime(0, "j")          = YJ;
                TB.theModifiableCharArray(0, "k")     = YK;
                TB.theModifiableShortArray(0, "l")    = YL;
                TB.theModifiableIntArray(0, "m")      = YM;
                TB.theModifiableInt64Array(0, "n")    = YN;
                TB.theModifiableFloatArray(0, "o")    = YO;
                TB.theModifiableDoubleArray(0, "p")   = YP;
                TB.theModifiableStringArray(0, "q")   = YQ;
                TB.theModifiableDatetimeArray(0, "r") = YR;
                TB.theModifiableDateArray(0, "s")     = YS;
                TB.theModifiableTimeArray(0, "t")     = YT;
                TB.theModifiableList(0, "u")          = YU;
                TB.theModifiableTable(0, "v")         = YV;
                TB.theModifiableBool(0, "w")          = YW;
                TB.theModifiableDatetimeTz(0, "x")    = YX;
                TB.theModifiableDateTz(0, "y")        = YY;
                TB.theModifiableTimeTz(0, "z")        = YZ;
                TB.theModifiableBoolArray(0, "A")     = Ya;
                TB.theModifiableDatetimeTzArray(0, "B")   = Yb;
                TB.theModifiableDateTzArray(0, "C")   = Yc;
                TB.theModifiableTimeTzArray(0, "D")   = Yd;
                TB.theModifiableChoice(0, "E")        = Ye;
                TB.theModifiableChoiceArray(0, "F")   = Yf;

                ASSERT(YA == TX[0][0].theChar());
                ASSERT(YB == TX[0][1].theShort());
                ASSERT(YC == TX[0][2].theInt());
                ASSERT(YD == TX[0][3].theInt64());
                ASSERT(YE == TX[0][4].theFloat());
                ASSERT(YF == TX[0][5].theDouble());
                ASSERT(YG == TX[0][6].theString());
                ASSERT(YH == TX[0][7].theDatetime());
                ASSERT(YI == TX[0][8].theDate());
                ASSERT(YJ == TX[0][9].theTime());
                ASSERT(YK == TX[0][10].theCharArray());
                ASSERT(YL == TX[0][11].theShortArray());
                ASSERT(YM == TX[0][12].theIntArray());
                ASSERT(YN == TX[0][13].theInt64Array());
                ASSERT(YO == TX[0][14].theFloatArray());
                ASSERT(YP == TX[0][15].theDoubleArray());
                ASSERT(YQ == TX[0][16].theStringArray());
                ASSERT(YR == TX[0][17].theDatetimeArray());
                ASSERT(YS == TX[0][18].theDateArray());
                ASSERT(YT == TX[0][19].theTimeArray());
                ASSERT(YU == TX[0][20].theList());
                ASSERT(YV == TX[0][21].theTable());
                ASSERT(YW == TX[0][22].theBool());
                ASSERT(YX == TX[0][23].theDatetimeTz());
                ASSERT(YY == TX[0][24].theDateTz());
                ASSERT(YZ == TX[0][25].theTimeTz());
                ASSERT(Ya == TX[0][26].theBoolArray());
                ASSERT(Yb == TX[0][27].theDatetimeTzArray());
                ASSERT(Yc == TX[0][28].theDateTzArray());
                ASSERT(Yd == TX[0][29].theTimeTzArray());
                ASSERT(Ye == TX[0][30].theChoice());
                ASSERT(Yf == TX[0][31].theChoiceArray());

                for (int i = 0; i < TX.numRows(); ++i) {
                    for (int j = 0; j < TX.numColumns(); ++j) {
                        ASSERT(TX[i][j]  == CTB.element(i, j));
                        ASSERT(TX_[i][j] ==  TB.element(i, j));

                        ASSERT(TX[i][j]  == CTB.element(i, fieldNames[j]));
                        ASSERT(TX_[i][j] ==  TB.element(i, fieldNames[j]));
                    }
                }

                TB.theModifiableChar(0, "a")          = XA;
                TB.theModifiableShort(0, "b")         = XB;
                TB.theModifiableInt(0, "c")           = XC;
                TB.theModifiableInt64(0, "d")         = XD;
                TB.theModifiableFloat(0, "e")         = XE;
                TB.theModifiableDouble(0, "f")        = XF;
                TB.theModifiableString(0, "g")        = XG;
                TB.theModifiableDatetime(0, "h")      = XH;
                TB.theModifiableDate(0, "i")          = XI;
                TB.theModifiableTime(0, "j")          = XJ;
                TB.theModifiableCharArray(0, "k")     = XK;
                TB.theModifiableShortArray(0, "l")    = XL;
                TB.theModifiableIntArray(0, "m")      = XM;
                TB.theModifiableInt64Array(0, "n")    = XN;
                TB.theModifiableFloatArray(0, "o")    = XO;
                TB.theModifiableDoubleArray(0, "p")   = XP;
                TB.theModifiableStringArray(0, "q")   = XQ;
                TB.theModifiableDatetimeArray(0, "r") = XR;
                TB.theModifiableDateArray(0, "s")     = XS;
                TB.theModifiableTimeArray(0, "t")     = XT;
                TB.theModifiableList(0, "u")          = XU;
                TB.theModifiableTable(0, "v")         = XV;
                TB.theModifiableBool(0, "w")          = XW;
                TB.theModifiableDatetimeTz(0, "x")    = XX;
                TB.theModifiableDateTz(0, "y")        = XY;
                TB.theModifiableTimeTz(0, "z")        = XZ;
                TB.theModifiableBoolArray(0, "A")     = Xa;
                TB.theModifiableDatetimeTzArray(0, "B")   = Xb;
                TB.theModifiableDateTzArray(0, "C")   = Xc;
                TB.theModifiableTimeTzArray(0, "D")   = Xd;
                TB.theModifiableChoice(0, "E")        = Xe;
                TB.theModifiableChoiceArray(0, "F")   = Xf;

                ASSERT(XA == TX[0][0].theChar());
                ASSERT(XB == TX[0][1].theShort());
                ASSERT(XC == TX[0][2].theInt());
                ASSERT(XD == TX[0][3].theInt64());
                ASSERT(XE == TX[0][4].theFloat());
                ASSERT(XF == TX[0][5].theDouble());
                ASSERT(XG == TX[0][6].theString());
                ASSERT(XH == TX[0][7].theDatetime());
                ASSERT(XI == TX[0][8].theDate());
                ASSERT(XJ == TX[0][9].theTime());
                ASSERT(XK == TX[0][10].theCharArray());
                ASSERT(XL == TX[0][11].theShortArray());
                ASSERT(XM == TX[0][12].theIntArray());
                ASSERT(XN == TX[0][13].theInt64Array());
                ASSERT(XO == TX[0][14].theFloatArray());
                ASSERT(XP == TX[0][15].theDoubleArray());
                ASSERT(XQ == TX[0][16].theStringArray());
                ASSERT(XR == TX[0][17].theDatetimeArray());
                ASSERT(XS == TX[0][18].theDateArray());
                ASSERT(XT == TX[0][19].theTimeArray());
                ASSERT(XU == TX[0][20].theList());
                ASSERT(XV == TX[0][21].theTable());
                ASSERT(XW == TX[0][22].theBool());
                ASSERT(XX == TX[0][23].theDatetimeTz());
                ASSERT(XY == TX[0][24].theDateTz());
                ASSERT(XZ == TX[0][25].theTimeTz());
                ASSERT(Xa == TX[0][26].theBoolArray());
                ASSERT(Xb == TX[0][27].theDatetimeTzArray());
                ASSERT(Xc == TX[0][28].theDateTzArray());
                ASSERT(Xd == TX[0][29].theTimeTzArray());
                ASSERT(Xe == TX[0][30].theChoice());
                ASSERT(Xf == TX[0][31].theChoiceArray());

                for (int i = 0; i < TX.numRows(); ++i) {
                    for (int j = 0; j < TX.numColumns(); ++j) {
                        ASSERT(TX[i][j]  == CTB.element(i, j));
                        ASSERT(TX_[i][j] ==  TB.element(i, j));

                        ASSERT(TX[i][j]  == CTB.element(i, fieldNames[j]));
                        ASSERT(TX_[i][j] ==  TB.element(i, fieldNames[j]));
                    }
                }

                TB.theModifiableChar(1, "a")          = XA;
                TB.theModifiableShort(1, "b")         = XB;
                TB.theModifiableInt(1, "c")           = XC;
                TB.theModifiableInt64(1, "d")         = XD;
                TB.theModifiableFloat(1, "e")         = XE;
                TB.theModifiableDouble(1, "f")        = XF;
                TB.theModifiableString(1, "g")        = XG;
                TB.theModifiableDatetime(1, "h")      = XH;
                TB.theModifiableDate(1, "i")          = XI;
                TB.theModifiableTime(1, "j")          = XJ;
                TB.theModifiableCharArray(1, "k")     = XK;
                TB.theModifiableShortArray(1, "l")    = XL;
                TB.theModifiableIntArray(1, "m")      = XM;
                TB.theModifiableInt64Array(1, "n")    = XN;
                TB.theModifiableFloatArray(1, "o")    = XO;
                TB.theModifiableDoubleArray(1, "p")   = XP;
                TB.theModifiableStringArray(1, "q")   = XQ;
                TB.theModifiableDatetimeArray(1, "r") = XR;
                TB.theModifiableDateArray(1, "s")     = XS;
                TB.theModifiableTimeArray(1, "t")     = XT;
                TB.theModifiableList(1, "u")          = XU;
                TB.theModifiableTable(1, "v")         = XV;
                TB.theModifiableBool(1, "w")          = XW;
                TB.theModifiableDatetimeTz(1, "x")    = XX;
                TB.theModifiableDateTz(1, "y")        = XY;
                TB.theModifiableTimeTz(1, "z")        = XZ;
                TB.theModifiableBoolArray(1, "A")     = Xa;
                TB.theModifiableDatetimeTzArray(1, "B")   = Xb;
                TB.theModifiableDateTzArray(1, "C")   = Xc;
                TB.theModifiableTimeTzArray(1, "D")   = Xd;
                TB.theModifiableChoice(1, "E")        = Xe;
                TB.theModifiableChoiceArray(1, "F")   = Xf;

                ASSERT(XA == TX[1][0].theChar());
                ASSERT(XB == TX[1][1].theShort());
                ASSERT(XC == TX[1][2].theInt());
                ASSERT(XD == TX[1][3].theInt64());
                ASSERT(XE == TX[1][4].theFloat());
                ASSERT(XF == TX[1][5].theDouble());
                ASSERT(XG == TX[1][6].theString());
                ASSERT(XH == TX[1][7].theDatetime());
                ASSERT(XI == TX[1][8].theDate());
                ASSERT(XJ == TX[1][9].theTime());
                ASSERT(XK == TX[1][10].theCharArray());
                ASSERT(XL == TX[1][11].theShortArray());
                ASSERT(XM == TX[1][12].theIntArray());
                ASSERT(XN == TX[1][13].theInt64Array());
                ASSERT(XO == TX[1][14].theFloatArray());
                ASSERT(XP == TX[1][15].theDoubleArray());
                ASSERT(XQ == TX[1][16].theStringArray());
                ASSERT(XR == TX[1][17].theDatetimeArray());
                ASSERT(XS == TX[1][18].theDateArray());
                ASSERT(XT == TX[1][19].theTimeArray());
                ASSERT(XU == TX[1][20].theList());
                ASSERT(XV == TX[1][21].theTable());
                ASSERT(XW == TX[1][22].theBool());
                ASSERT(XX == TX[1][23].theDatetimeTz());
                ASSERT(XY == TX[1][24].theDateTz());
                ASSERT(XZ == TX[1][25].theTimeTz());
                ASSERT(Xa == TX[1][26].theBoolArray());
                ASSERT(Xb == TX[1][27].theDatetimeTzArray());
                ASSERT(Xc == TX[1][28].theDateTzArray());
                ASSERT(Xd == TX[1][29].theTimeTzArray());
                ASSERT(Xe == TX[1][30].theChoice());
                ASSERT(Xf == TX[1][31].theChoiceArray());

                for (int i = 0; i < TX.numRows(); ++i) {
                    for (int j = 0; j < TX.numColumns(); ++j) {
                        ASSERT(TX[i][j]  == CTB.element(i, j));
                        ASSERT(TX_[i][j] ==  TB.element(i, j));

                        ASSERT(TX[i][j]  == CTB.element(i, fieldNames[j]));
                        ASSERT(TX_[i][j] ==  TB.element(i, fieldNames[j]));
                    }
                }

                TB.theModifiableChar(1, "a")          = YA;
                TB.theModifiableShort(1, "b")         = YB;
                TB.theModifiableInt(1, "c")           = YC;
                TB.theModifiableInt64(1, "d")         = YD;
                TB.theModifiableFloat(1, "e")         = YE;
                TB.theModifiableDouble(1, "f")        = YF;
                TB.theModifiableString(1, "g")        = YG;
                TB.theModifiableDatetime(1, "h")      = YH;
                TB.theModifiableDate(1, "i")          = YI;
                TB.theModifiableTime(1, "j")          = YJ;
                TB.theModifiableCharArray(1, "k")     = YK;
                TB.theModifiableShortArray(1, "l")    = YL;
                TB.theModifiableIntArray(1, "m")      = YM;
                TB.theModifiableInt64Array(1, "n")    = YN;
                TB.theModifiableFloatArray(1, "o")    = YO;
                TB.theModifiableDoubleArray(1, "p")   = YP;
                TB.theModifiableStringArray(1, "q")   = YQ;
                TB.theModifiableDatetimeArray(1, "r") = YR;
                TB.theModifiableDateArray(1, "s")     = YS;
                TB.theModifiableTimeArray(1, "t")     = YT;
                TB.theModifiableList(1, "u")          = YU;
                TB.theModifiableTable(1, "v")         = YV;
                TB.theModifiableBool(1, "w")          = YW;
                TB.theModifiableDatetimeTz(1, "x")    = YX;
                TB.theModifiableDateTz(1, "y")        = YY;
                TB.theModifiableTimeTz(1, "z")        = YZ;
                TB.theModifiableBoolArray(1, "A")     = Ya;
                TB.theModifiableDatetimeTzArray(1, "B")   = Yb;
                TB.theModifiableDateTzArray(1, "C")   = Yc;
                TB.theModifiableTimeTzArray(1, "D")   = Yd;
                TB.theModifiableChoice(1, "E")        = Ye;
                TB.theModifiableChoiceArray(1, "F")   = Yf;

                ASSERT(YA == TX[1][0].theChar());
                ASSERT(YB == TX[1][1].theShort());
                ASSERT(YC == TX[1][2].theInt());
                ASSERT(YD == TX[1][3].theInt64());
                ASSERT(YE == TX[1][4].theFloat());
                ASSERT(YF == TX[1][5].theDouble());
                ASSERT(YG == TX[1][6].theString());
                ASSERT(YH == TX[1][7].theDatetime());
                ASSERT(YI == TX[1][8].theDate());
                ASSERT(YJ == TX[1][9].theTime());
                ASSERT(YK == TX[1][10].theCharArray());
                ASSERT(YL == TX[1][11].theShortArray());
                ASSERT(YM == TX[1][12].theIntArray());
                ASSERT(YN == TX[1][13].theInt64Array());
                ASSERT(YO == TX[1][14].theFloatArray());
                ASSERT(YP == TX[1][15].theDoubleArray());
                ASSERT(YQ == TX[1][16].theStringArray());
                ASSERT(YR == TX[1][17].theDatetimeArray());
                ASSERT(YS == TX[1][18].theDateArray());
                ASSERT(YT == TX[1][19].theTimeArray());
                ASSERT(YU == TX[1][20].theList());
                ASSERT(YV == TX[1][21].theTable());
                ASSERT(YW == TX[1][22].theBool());
                ASSERT(YX == TX[1][23].theDatetimeTz());
                ASSERT(YY == TX[1][24].theDateTz());
                ASSERT(YZ == TX[1][25].theTimeTz());
                ASSERT(Ya == TX[1][26].theBoolArray());
                ASSERT(Yb == TX[1][27].theDatetimeTzArray());
                ASSERT(Yc == TX[1][28].theDateTzArray());
                ASSERT(Yd == TX[1][29].theTimeTzArray());
                ASSERT(Ye == TX[1][30].theChoice());
                ASSERT(Yf == TX[1][31].theChoiceArray());

                for (int i = 0; i < TX.numRows(); ++i) {
                    for (int j = 0; j < TX.numColumns(); ++j) {
                        ASSERT(TX[i][j]  == CTB.element(i, j));
                        ASSERT(TX_[i][j] ==  TB.element(i, j));

                        ASSERT(TX[i][j]  == CTB.element(i, fieldNames[j]));
                        ASSERT(TX_[i][j] ==  TB.element(i, fieldNames[j]));
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(8) {
        // --------------------------------------------------------------------
        // TESTING ROW BINDING ACCESSORS
        //   Test Row Binding accessor methods.
        //
        // Concerns:
        //   - That value-returning methods return the correct result.
        //   - That references of the appropriate type and value are returned.
        //   - That references of the appropriate 'const'-ness are returned.
        //
        // Plan:
        //   Create a 'bdem_List' containing one data element for each
        //   'bdem_ElemType::Type'.  Using that 'bdem_List', create instances
        //   of all 4 row binding classes.  Access the elements in the original
        //   'bdem_List' using the non-modifiable accessor functions provided
        //   by the row bindings and verify that the references returned match
        //   the original data.  Use the accessors that return modifiable
        //   references to change the value of the 'bdem_List'.  Check that the
        //   value of the original 'bdem_List' has been correctly modified via
        //   the bindings.  As a negative test, confirm that the accessors of
        //   "const" row bindings return non-modifiable references.
        //
        // Testing:
        //             bdem_ConstRowBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstElemRef operator[](int) const;
        //   bdem_ConstElemRef element(*name) const;
        //   bdem_ConstElemRef element(int) const;
        //   bdem_ElemType::Type elemType(*name) const;
        //   bdem_ElemType::Type elemType(int) const;
        //   const char& theChar(*name) const;
        //   const short& theShort(*name) const;
        //   const int& theInt(*name) const;
        //   const Int64& theInt64(*name) const;
        //   const float& theFloat(*name) const;
        //   const double& theDouble(*name) const;
        //   const string& theString(*name) const;
        //   const bdet_Datetime& theDatetime(*name) const;
        //   const bdet_Date& theDate(*name) const;
        //   const bdet_Time& theTime(*name) const;
        //   const vector<char>& theCharArray(*name) const;
        //   const vector<short>& theShortArray(*name) const;
        //   const vector<int>& theIntArray(*name) const;
        //   const vector<Int64>& theInt64Array(*name) const;
        //   const vector<float>& theFloatArray(*name) const;
        //   const vector<double>& theDoubleArray(*name) const;
        //   const vector<string>& theStringArray(*name) const;
        //   const vector<Datetime>& theDatetimeArray(*name) const;
        //   const vector<Date>& theDateArray(*name) const;
        //   const vector<Time>& theTimeArray(*name) const;
        //   const bdem_List& theList(*name) const;
        //   const bdem_Table& theTable(*name) const;
        //   const bdem_Bool&  theBool(*name)  const;
        //   const bdem_DatetimeTz&  theDatetimeTz(*name)  const;
        //   const bdem_DateTz&  theDateTz(*name)  const;
        //   const bdem_TimeTz&  theTimeTz(*name)  const;
        //   const bdem_BoolArray&  theBoolArray(*name)  const;
        //   const bdem_DatetimeTzArray&  theDatetimeTzArray(*name)  const;
        //   const bdem_DateTzArray&  theDateTzArray(*name)  const;
        //   const bdem_TimeTzArray&  theTimeTzArray(*name)  const;
        //   const bdem_Choice&  theChoice(*name)  const;
        //   const bdem_ChoiceArray&  theChoiceArray(*name)  const;
        //
        //             bdem_RowBinding
        //             ~~~~~~~~~~~~~~~
        //   bdem_ElemRef operator[](int) const;
        //   bdem_ElemRef element(*name) const;
        //   bdem_ElemRef element(int) const;
        //   char& theChar(*name) const;
        //   short& theShort(*name) const;
        //   int& theInt(*name) const;
        //   Int64& theInt64(*name) const;
        //   float& theFloat(*name) const;
        //   double& theDouble(*name) const;
        //   string& theString(*name) const;
        //   bdet_Datetime& theDatetime(*name) const;
        //   bdet_Time& theTime(*name) const;
        //   bdet_Date& theDate(*name) const;
        //   vector<char>& theCharArray(*name) const;
        //   vector<short>& theShortArray(*name) const;
        //   vector<int>& theIntArray(*name) const;
        //   vector<Int64>& theInt64Array(*name) const;
        //   vector<float>& theFloatArray(*name) const;
        //   vector<double>& theDoubleArray(*name) const;
        //   vector<string>& theStringArray(*name) const;
        //   vector<Datetime>& theDatetimeArray(*name) const;
        //   vector<Date>& theDateArray(*name) const;
        //   vector<Time>& theTimeArray(*name) const;
        //   bdem_List& theList(*name) const;
        //   bdem_Table& theTable(*name) const;
        //   bdem_Bool&  theBool(*name)  const;
        //   bdem_DatetimeTz&  theDatetimeTz(*name)  const;
        //   bdem_DateTz&  theDateTz(*name)  const;
        //   bdem_TimeTz&  theTimeTz(*name)  const;
        //   bdem_BoolArray&  theBoolArray(*name)  const;
        //   bdem_DatetimeTzArray&  theDatetimeTzArray(*name)  const;
        //   bdem_DateTzArray&  theDateTzArray(*name)  const;
        //   bdem_TimeTzArray&  theTimeTzArray(*name)  const;
        //   bdem_Choice&  theChoice(*name)  const;
        //   bdem_ChoiceArray&  theChoiceArray(*name)  const;
        // --------------------------------------------------------------------

        DECLARE_TEST_SCHEMA
        DECLARE_TEST_LIST_OBJECTS

        if (verbose) cout << "Testing Row Binding Accessors" << endl
                          << "=============================" << endl;

        {
                   CRowBind   crb(&LX, pR);
                    RowBind    rb(&LX_, pR);

            const  CRowBind&  CRB = crb;
            const   RowBind&   RB = rb;

            ASSERT(LX.length() ==  CRB.length());
            ASSERT(LX.length() ==   RB.length());

            if (veryVerbose) {
                   cout << "Testing non-modifiable accessors." << endl;
                T_ cout << "Bindings." << endl;
            }

            ASSERT(XA == CRB.theChar("a"));
            ASSERT(XB == CRB.theShort("b"));
            ASSERT(XC == CRB.theInt("c"));
            ASSERT(XD == CRB.theInt64("d"));
            ASSERT(XE == CRB.theFloat("e"));
            ASSERT(XF == CRB.theDouble("f"));
            ASSERT(XG == CRB.theString("g"));
            ASSERT(XH == CRB.theDatetime("h"));
            ASSERT(XI == CRB.theDate("i"));
            ASSERT(XJ == CRB.theTime("j"));
            ASSERT(XK == CRB.theCharArray("k"));
            ASSERT(XL == CRB.theShortArray("l"));
            ASSERT(XM == CRB.theIntArray("m"));
            ASSERT(XN == CRB.theInt64Array("n"));
            ASSERT(XO == CRB.theFloatArray("o"));
            ASSERT(XP == CRB.theDoubleArray("p"));
            ASSERT(XQ == CRB.theStringArray("q"));
            ASSERT(XR == CRB.theDatetimeArray("r"));
            ASSERT(XS == CRB.theDateArray("s"));
            ASSERT(XT == CRB.theTimeArray("t"));
            ASSERT(XU == CRB.theList("u"));
            ASSERT(XV == CRB.theTable("v"));
            ASSERT(XW == CRB.theBool("w"));
            ASSERT(XX == CRB.theDatetimeTz("x"));
            ASSERT(XY == CRB.theDateTz("y"));
            ASSERT(XZ == CRB.theTimeTz("z"));
            ASSERT(Xa == CRB.theBoolArray("A"));
            ASSERT(Xb == CRB.theDatetimeTzArray("B"));
            ASSERT(Xc == CRB.theDateTzArray("C"));
            ASSERT(Xd == CRB.theTimeTzArray("D"));
            ASSERT(Xe == CRB.theChoice("E"));
            ASSERT(Xf == CRB.theChoiceArray("F"));

            ASSERT(XA ==  RB.theChar("a"));
            ASSERT(XB ==  RB.theShort("b"));
            ASSERT(XC ==  RB.theInt("c"));
            ASSERT(XD ==  RB.theInt64("d"));
            ASSERT(XE ==  RB.theFloat("e"));
            ASSERT(XF ==  RB.theDouble("f"));
            ASSERT(XG ==  RB.theString("g"));
            ASSERT(XH ==  RB.theDatetime("h"));
            ASSERT(XI ==  RB.theDate("i"));
            ASSERT(XJ ==  RB.theTime("j"));
            ASSERT(XK ==  RB.theCharArray("k"));
            ASSERT(XL ==  RB.theShortArray("l"));
            ASSERT(XM ==  RB.theIntArray("m"));
            ASSERT(XN ==  RB.theInt64Array("n"));
            ASSERT(XO ==  RB.theFloatArray("o"));
            ASSERT(XP ==  RB.theDoubleArray("p"));
            ASSERT(XQ ==  RB.theStringArray("q"));
            ASSERT(XR ==  RB.theDatetimeArray("r"));
            ASSERT(XS ==  RB.theDateArray("s"));
            ASSERT(XT ==  RB.theTimeArray("t"));
            ASSERT(XU ==  RB.theList("u"));
            ASSERT(XV ==  RB.theTable("v"));
            ASSERT(XW ==  RB.theBool("w"));
            ASSERT(XX ==  RB.theDatetimeTz("x"));
            ASSERT(XY ==  RB.theDateTz("y"));
            ASSERT(XZ ==  RB.theTimeTz("z"));
            ASSERT(Xa ==  RB.theBoolArray("A"));
            ASSERT(Xb ==  RB.theDatetimeTzArray("B"));
            ASSERT(Xc ==  RB.theDateTzArray("C"));
            ASSERT(Xd ==  RB.theTimeTzArray("D"));
            ASSERT(Xe ==  RB.theChoice("E"));
            ASSERT(Xf ==  RB.theChoiceArray("F"));

            ASSERT(XA ==  RB.theModifiableChar("a"));
            ASSERT(XB ==  RB.theModifiableShort("b"));
            ASSERT(XC ==  RB.theModifiableInt("c"));
            ASSERT(XD ==  RB.theModifiableInt64("d"));
            ASSERT(XE ==  RB.theModifiableFloat("e"));
            ASSERT(XF ==  RB.theModifiableDouble("f"));
            ASSERT(XG ==  RB.theModifiableString("g"));
            ASSERT(XH ==  RB.theModifiableDatetime("h"));
            ASSERT(XI ==  RB.theModifiableDate("i"));
            ASSERT(XJ ==  RB.theModifiableTime("j"));
            ASSERT(XK ==  RB.theModifiableCharArray("k"));
            ASSERT(XL ==  RB.theModifiableShortArray("l"));
            ASSERT(XM ==  RB.theModifiableIntArray("m"));
            ASSERT(XN ==  RB.theModifiableInt64Array("n"));
            ASSERT(XO ==  RB.theModifiableFloatArray("o"));
            ASSERT(XP ==  RB.theModifiableDoubleArray("p"));
            ASSERT(XQ ==  RB.theModifiableStringArray("q"));
            ASSERT(XR ==  RB.theModifiableDatetimeArray("r"));
            ASSERT(XS ==  RB.theModifiableDateArray("s"));
            ASSERT(XT ==  RB.theModifiableTimeArray("t"));
            ASSERT(XU ==  RB.theModifiableList("u"));
            ASSERT(XV ==  RB.theModifiableTable("v"));
            ASSERT(XW ==  RB.theModifiableBool("w"));
            ASSERT(XX ==  RB.theModifiableDatetimeTz("x"));
            ASSERT(XY ==  RB.theModifiableDateTz("y"));
            ASSERT(XZ ==  RB.theModifiableTimeTz("z"));
            ASSERT(Xa ==  RB.theModifiableBoolArray("A"));
            ASSERT(Xb ==  RB.theModifiableDatetimeTzArray("B"));
            ASSERT(Xc ==  RB.theModifiableDateTzArray("C"));
            ASSERT(Xd ==  RB.theModifiableTimeTzArray("D"));
            ASSERT(Xe ==  RB.theModifiableChoice("E"));
            ASSERT(Xf ==  RB.theModifiableChoiceArray("F"));

            for (int i = 0; i < LX.length(); ++i) {
                ASSERT(LX.elemType(i) ==  CRB.elemType(i));

                ASSERT(LX.elemType(i) ==  CRB.elemType(fieldNames[i]));

                ASSERT(LX[i]  == CRB[i]);
                ASSERT(LX_[i] ==  RB[i]);

                ASSERT(LX[i]  == CRB.element(i));
                ASSERT(LX_[i] ==  RB.element(i));

                ASSERT(LX[i]  == CRB.element(fieldNames[i]));
                ASSERT(LX_[i] ==  RB.element(fieldNames[i]));
            }

            if (veryVerbose) {
                   cout << "Testing modifiable accessors." << endl;
                T_ cout << "Bindings." << endl;
            }

            RB.theModifiableChar("a")          = YA;
            RB.theModifiableShort("b")         = YB;
            RB.theModifiableInt("c")           = YC;
            RB.theModifiableInt64("d")         = YD;
            RB.theModifiableFloat("e")         = YE;
            RB.theModifiableDouble("f")        = YF;
            RB.theModifiableString("g")        = YG;
            RB.theModifiableDatetime("h")      = YH;
            RB.theModifiableDate("i")          = YI;
            RB.theModifiableTime("j")          = YJ;
            RB.theModifiableCharArray("k")     = YK;
            RB.theModifiableShortArray("l")    = YL;
            RB.theModifiableIntArray("m")      = YM;
            RB.theModifiableInt64Array("n")    = YN;
            RB.theModifiableFloatArray("o")    = YO;
            RB.theModifiableDoubleArray("p")   = YP;
            RB.theModifiableStringArray("q")   = YQ;
            RB.theModifiableDatetimeArray("r") = YR;
            RB.theModifiableDateArray("s")     = YS;
            RB.theModifiableTimeArray("t")     = YT;
            RB.theModifiableList("u")          = YU;
            RB.theModifiableTable("v")         = YV;
            RB.theModifiableBool("w")          = YW;
            RB.theModifiableDatetimeTz("x")    = YX;
            RB.theModifiableDateTz("y")        = YY;
            RB.theModifiableTimeTz("z")        = YZ;
            RB.theModifiableBoolArray("A")     = Ya;
            RB.theModifiableDatetimeTzArray("B")  = Yb;
            RB.theModifiableDateTzArray("C")   = Yc;
            RB.theModifiableTimeTzArray("D")   = Yd;
            RB.theModifiableChoice("E")        = Ye;
            RB.theModifiableChoiceArray("F")   = Yf;

            ASSERT(YA == LX.theChar(0));
            ASSERT(YB == LX.theShort(1));
            ASSERT(YC == LX.theInt(2));
            ASSERT(YD == LX.theInt64(3));
            ASSERT(YE == LX.theFloat(4));
            ASSERT(YF == LX.theDouble(5));
            ASSERT(YG == LX.theString(6));
            ASSERT(YH == LX.theDatetime(7));
            ASSERT(YI == LX.theDate(8));
            ASSERT(YJ == LX.theTime(9));
            ASSERT(YK == LX.theCharArray(10));
            ASSERT(YL == LX.theShortArray(11));
            ASSERT(YM == LX.theIntArray(12));
            ASSERT(YN == LX.theInt64Array(13));
            ASSERT(YO == LX.theFloatArray(14));
            ASSERT(YP == LX.theDoubleArray(15));
            ASSERT(YQ == LX.theStringArray(16));
            ASSERT(YR == LX.theDatetimeArray(17));
            ASSERT(YS == LX.theDateArray(18));
            ASSERT(YT == LX.theTimeArray(19));
            ASSERT(YU == LX.theList(20));
            ASSERT(YV == LX.theTable(21));
            ASSERT(YW == LX.theBool(22));
            ASSERT(YX == LX.theDatetimeTz(23));
            ASSERT(YY == LX.theDateTz(24));
            ASSERT(YZ == LX.theTimeTz(25));
            ASSERT(Ya == LX.theBoolArray(26));
            ASSERT(Yb == LX.theDatetimeTzArray(27));
            ASSERT(Yc == LX.theDateTzArray(28));
            ASSERT(Yd == LX.theTimeTzArray(29));
            ASSERT(Ye == LX.theChoice(30));
            ASSERT(Yf == LX.theChoiceArray(31));

            for (int i = 0; i < LX.length(); ++i) {
                ASSERT(LX[i]  == CRB[i]);
                ASSERT(LX_[i] ==  RB[i]);

                ASSERT(LX[i]  == CRB.element(i));
                ASSERT(LX_[i] ==  RB.element(i));

                ASSERT(LX[i]  == CRB.element(fieldNames[i]));
                ASSERT(LX_[i] ==  RB.element(fieldNames[i]));
            }
        }
      }

DEFINE_TEST_CASE(7) {
        // -------------------------------------------------------------------
        // TESTING CHOICE ARRAY BINDING CREATORS AND ACCESSORS
        //   Test Choice Array Binding creators and accessors.
        //
        // Concerns:
        //   - All constructors create valid objects.
        //   - The data, selector, and meta-data are hooked up correctly.
        //   - That value-returning methods return the correct result.
        //   - That references of the appropriate type and value are returned.
        //   - The destructor leaves the bound data and meta-data unaffected.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_Choice' objects that satisfy the final
        //        records contained in the schemas from 1).
        //     3) A vector of record names.  This vector stores the
        //        *names* of the records in the schemas from 1) corresponding
        //        to the data in the tables from 2).
        //
        //   Create instances of 'bdem_ConstChoiceBinding' and
        //   'bdem_ChoiceBinding' (using all constructors) for each
        //   ever element of vector 'bdem_Choice's.  Use the direct accessors
        //   to verify that the bound choice, selector, and record have been
        //   installed correctly.  Use the non-direct accessors to verify that
        //   the binding acts correctly as a proxy for the bound column.
        //
        //   Provide explicit test for destructor vacuity.
        //
        // Testing:
        //             bdem_ConstChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~~
        //  bdem_ConstChoiceArrayBinding(const bdem_ChoiceArray *,
        //                               const bdem_RecordDef *);
        //  bdem_ConstChoiceArrayBinding(const bdem_ChoiceArray *,
        //                               const bdem_Schema *);
        //  bdem_ConstChoiceArrayBinding(const bdem_ChoiceArray *,
        //                               const bdem_Schema &, int);
        //  bdem_ConstChoiceArrayBinding(const bdem_ConstChoiceArrayBinding
        //                                         &original);
        //  ~bdem_ConstChoiceArrayBinding();
        //  const bdem_ChoiceArrayItem& operator[](int itemIndex) const;
        //  const bdem_ChoiceArrayItem& choiceArray() const;
        //  int length();
        //  const bdem_RecordDef& record() const;
        //
        //             bdem_ChoiceArrayBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~~
        //  bdem_ChoiceArrayBinding(const bdem_ChoiceArray *,
        //                               const bdem_RecordDef *);
        //  bdem_ChoiceArrayBinding(const bdem_ChoiceArray *,
        //                               const bdem_Schema *);
        //  bdem_ChoiceArrayBinding(const bdem_ChoiceArray *,
        //                               const bdem_Schema &, int);
        //  bdem_ChoiceArrayBinding(const bdem_ConstChoiceArrayBinding
        //                                         &original);
        //  ~bdem_ChoiceArrayBinding();
        //  int length();
        //  const bdem_ChoiceArrayItem& operator[](int itemIndex) const;
        //  const bdem_ChoiceArrayItem& choiceArray() const;
        //  const bdem_RecordDef& record() const;
        //
        // --------------------------------------------------------------------

        DECLARE_CHOICE_OBJECTS

        if (verbose)
            cout << "Testing Choice Array Binding Creators and Accessors"
                 << endl
                 << "====================================================="
                 << endl;

        {
            if (veryVerbose) {
                cout << "Ctors taking 'ChoiceArray' pointer."
             << endl;
            }

            for (int i = 0; i < NUM_CHOICE_SPECS; ++i) {
               ChoiceArray mCA = choiceArrayVec[i];
               const ChoiceArray& CA = mCA;

               const Schema&  S       = choiceArraySchemaVec[i];
               const RecDef  *rec     = &S.record(S.numRecords() - 1);
               const char    *recName = choiceArrayRecordName[i];

                    CCArrBind      ccab1(&CA,rec);
                    CCArrBind      ccab2(&CA,&S, recName);
                    CCArrBind      ccab3(&CA,&S, S.numRecords() - 1);
                const CCArrBind&   CCAB1 = ccab1;
                const CCArrBind&   CCAB2 = ccab2;
                const CCArrBind&   CCAB3 = ccab3;

                if (veryVerbose) { T_ cout << "Non-Const bindings." << endl; }

                    CArrBind      cab1(&mCA,rec);
                    CArrBind      cab2(&mCA,&S, recName);
                    CArrBind      cab3(&mCA,&S, S.numRecords() - 1);
                const CArrBind&   CAB1 = cab1;
                const CArrBind&   CAB2 = cab2;
                const CArrBind&   CAB3 = cab3;

                ASSERT( &CA == &CCAB1.choiceArray());
                ASSERT( &CA == &CCAB2.choiceArray());
                ASSERT( &CA == &CCAB3.choiceArray());

                ASSERT( &mCA == &CAB1.choiceArray());
                ASSERT( &mCA == &CAB2.choiceArray());
                ASSERT( &mCA == &CAB3.choiceArray());

                ASSERT( rec == &CCAB1.record());
                ASSERT( rec == &CCAB2.record());
                ASSERT( rec == &CCAB3.record());

                ASSERT( rec == &CAB1.record());
                ASSERT( rec == &CAB2.record());
                ASSERT( rec == &CAB3.record());

                ASSERT( CA.numSelections() == CCAB1.numSelections());
                ASSERT( CA.numSelections() == CCAB2.numSelections());
                ASSERT( CA.numSelections() == CCAB3.numSelections());

                ASSERT( mCA.numSelections() == CAB1.numSelections());
                ASSERT( mCA.numSelections() == CAB2.numSelections());
                ASSERT( mCA.numSelections() == CAB3.numSelections());

                ASSERT( CA.length() == CCAB1.length());
                ASSERT( CA.length() == CCAB2.length());
                ASSERT( CA.length() == CCAB3.length());
                ASSERT( CA.length() == CAB1.length());
                ASSERT( CA.length() == CAB2.length());
                ASSERT( CA.length() == CAB3.length());

                if (veryVerbose) {
                    T_ cout << "operator []" << endl;
                }
                for (int index = 0 ; index < CA.length() ; ++index) {
                    ASSERT( CA[index] == CCAB1[index] );
                    ASSERT( CA[index] == CCAB2[index] );
                    ASSERT( CA[index] == CCAB3[index] );

                    ASSERT( mCA.theModifiableItem(index) == CAB1[index] );
                    ASSERT( mCA.theModifiableItem(index) == CAB2[index] );
                    ASSERT( mCA.theModifiableItem(index) == CAB3[index] );
                }

            }
        }
        if (veryVerbose) {
            cout << "Explicit destructor test" << endl;
        }
        {
           ChoiceArray mCA(choiceArrayVec[0]);  const ChoiceArray& CA = mCA;

           const Schema&  S   = choiceArraySchemaVec[0];
           const RecDef  *rec = &S.record(S.numRecords() - 1);

           {
                CCArrBind ccab(&CA, rec);
           }
           ASSERT(CA == choiceArrayVec[0]);
           ASSERT(S == choiceArraySchemaVec[0]);

           {
               CArrBind cab(&mCA, rec);
           }
           ASSERT(CA == choiceArrayVec[0]);
           ASSERT(S == choiceArraySchemaVec[0]);
        }

        if (veryVerbose) { cout << "'const'-correctness." << endl; }
        {
            ChoiceArray mCA(choiceArrayVec[0]);  const ChoiceArray& CA = mCA;

            const Schema&  S   = choiceArraySchemaVec[0];
            const RecDef  *rec = &S.record(S.numRecords() - 1);

            {
                      CCArrBind  ccab(&CA, rec);
                const CCArrBind& CCAB = ccab;

                const bdem_ChoiceArray&   CA = CCAB.choiceArray();
                ASSERT(CA.length());
                const RecDef& RD = CCAB.record();
                ASSERT(RD.numFields());
            }
            {
                      CArrBind  cab(&mCA, rec);
                const CArrBind& CAB = cab;

                bdem_ChoiceArray&  CA = CAB.choiceArray();
                ASSERT(CA.length());
            }
        }
      }

DEFINE_TEST_CASE(6) {
        // -------------------------------------------------------------------
        // TESTING CHOICE BINDING CREATORS AND ACCESSORS
        //   Test Choice Binding creators and accessors.
        //
        // Concerns:
        //   - All constructors create valid objects.
        //   - The data, selector, and meta-data are hooked up correctly.
        //   - That value-returning methods return the correct result.
        //   - That references of the appropriate type and value are returned.
        //   - The destructor leaves the bound data and meta-data unaffected.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_Choice' objects that satisfy the final
        //        records contained in the schemas from 1).
        //     3) A vector of record names.  This vector stores the
        //        *names* of the records in the schemas from 1) corresponding
        //        to the data in the tables from 2).
        //
        //   Create instances of 'bdem_ConstChoiceBinding' and
        //   'bdem_ChoiceBinding' (using all constructors) for each
        //   ever element of vector 'bdem_Choice's.  Use the direct accessors
        //   to verify that the bound choice, selector, and record have been
        //   installed correctly.  Use the non-direct accessors to verify that
        //   the binding acts correctly as a proxy for the bound column.
        //
        //   Provide explicit test for destructor vacuity.
        //
        // Testing:
        //             bdem_ConstChoiceBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~~
        //  bdem_ConstChoiceBinding(const bdem_ChoiceArrayItem *,
        //                          const bdem_RecordDef *);
        //  bdem_ConstChoiceBinding(const bdem_ChoiceArrayItem *,
        //                          const bdem_Schema *);
        //  bdem_ConstChoiceBinding(const bdem_ChoiceArrayItem *,
        //                          const bdem_Schema &, int);
        //  bdem_ConstChoiceBinding(const bdem_Choice *,
        //                          const bdem_RecordDef *);
        //  bdem_ConstChoiceBinding(const bdem_Choice *,
        //                          const bdem_Schema, const char *);
        //  bdem_ConstChoiceBinding(const bdem_Choice *,
        //                          const bdem_Schema *, int);
        //  bdem_ConstChoiceBinding(const bdem_ChoiceArray *,
        //                          int , const bdem_RecordDef *);
        //  bdem_ConstChoiceBinding(const bdem_ChoiceArray *,
        //                          int , const bdem_schema *, const char *);
        //  bdem_ConstChoiceBinding(const bdem_ChoiceArray *, int ,
        //                          const bdem_schema *, int);
        //  bdem_ConstChoiceBinding(const bdem_ConstChoiceBinding &original);
        //  ~bdem_ConstChoiceBinding();
        //  const bdem_ChoiceArrayItem& item() const;
        //  const bdem_RecordDef& record() const;
        //  int numSelections();
        //
        //             bdem_ChoiceBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~~
        //  bdem_ChoiceBinding(const bdem_ChoiceArrayItem *,
        //                          const bdem_RecordDef *);
        //  bdem_ChoiceBinding(const bdem_ChoiceArrayItem *,
        //                          const bdem_Schema *);
        //  bdem_ChoiceBinding(const bdem_ChoiceArrayItem *,
        //                          const bdem_Schema &, int);
        //  bdem_ChoiceBinding(const bdem_Choice *,
        //                          const bdem_RecordDef *);
        //  bdem_ChoiceBinding(const bdem_Choice *,
        //                          const bdem_Schema, const char *);
        //  bdem_ChoiceBinding(const bdem_Choice *,
        //                          const bdem_Schema *, int);
        //  bdem_ChoiceBinding(const bdem_ChoiceArray *,
        //                          int , const bdem_RecordDef *);
        //  bdem_ChoiceBinding(const bdem_ChoiceArray *,
        //                          int , const bdem_schema *, const char *);
        //  bdem_ChoiceBinding(const bdem_ChoiceArray *, int ,
        //                          const bdem_schema *, int);
        //  bdem_ChoiceBinding(bdem_ConstChoiceBinding &original);
        //  ~bdem_ChoiceBinding();
        //  const bdem_ChoiceArrayItem& item() const;
        //  const bdem_RecordDef& record() const;
        //  int numSelections();
        // --------------------------------------------------------------------

        DECLARE_CHOICE_OBJECTS

        if (verbose)
            cout << "Testing Choice Binding Creators and Accessors"
                 << endl
                 << "====================================================="
                 << endl;

        {
            if (veryVerbose) {
                cout << "Ctors taking 'ChoiceArrayItem' or 'Choice' pointer."
             << endl;
            }

            for (int i = 0; i < NUM_CHOICE_SPECS; ++i) {
               Choice mC = choiceVec[i]; const Choice& C = mC;

               const Schema&  S       = choiceSchemaVec[i];
               const RecDef  *rec     = &S.record(S.numRecords() - 1);
               const char    *recName = choiceRecordName[i];

                    CCBind      ccb1(&C.item(),rec);
                    CCBind      ccb2(&C.item(),&S, recName);
                    CCBind      ccb3(&C.item(),&S, S.numRecords() - 1);
                    CCBind      ccb4(&C,rec);
                    CCBind      ccb5(&C,&S, recName);
                    CCBind      ccb6(&C,&S, S.numRecords() - 1);
                const CCBind&   CCB1 = ccb1;
                const CCBind&   CCB2 = ccb2;
                const CCBind&   CCB3 = ccb3;
                const CCBind&   CCB4 = ccb4;
                const CCBind&   CCB5 = ccb5;
                const CCBind&   CCB6 = ccb6;

                if (veryVerbose) { T_ cout << "Non-Const bindings." << endl; }

                    CBind      cb1(&mC.item(),rec);
                    CBind      cb2(&mC.item(),&S, recName);
                    CBind      cb3(&mC.item(),&S, S.numRecords() - 1);
                    CBind      cb4(&mC,rec);
                    CBind      cb5(&mC,&S, recName);
                    CBind      cb6(&mC,&S, S.numRecords() - 1);
                const CBind&   CB1 = cb1;
                const CBind&   CB2 = cb2;
                const CBind&   CB3 = cb3;
                const CBind&   CB4 = cb4;
                const CBind&   CB5 = cb5;
                const CBind&   CB6 = cb6;

                ASSERT( &C.item() == &CCB1.item());
                ASSERT( &C.item() == &CCB2.item());
                ASSERT( &C.item() == &CCB3.item());
                ASSERT( &C.item() == &CCB4.item());
                ASSERT( &C.item() == &CCB5.item());
                ASSERT( &C.item() == &CCB6.item());

                ASSERT( &mC.item() == &CB1.item());
                ASSERT( &mC.item() == &CB2.item());
                ASSERT( &mC.item() == &CB3.item());
                ASSERT( &mC.item() == &CB4.item());
                ASSERT( &mC.item() == &CB5.item());
                ASSERT( &mC.item() == &CB6.item());

                ASSERT( C.numSelections() == CCB1.numSelections());
                ASSERT( C.numSelections() == CCB2.numSelections());
                ASSERT( C.numSelections() == CCB3.numSelections());
                ASSERT( C.numSelections() == CCB4.numSelections());
                ASSERT( C.numSelections() == CCB5.numSelections());
                ASSERT( C.numSelections() == CCB6.numSelections());

                ASSERT( mC.numSelections() == CB1.numSelections());
                ASSERT( mC.numSelections() == CB2.numSelections());
                ASSERT( mC.numSelections() == CB3.numSelections());
                ASSERT( mC.numSelections() == CB4.numSelections());
                ASSERT( mC.numSelections() == CB5.numSelections());
                ASSERT( mC.numSelections() == CB6.numSelections());

                ASSERT( rec == &CCB1.record());
                ASSERT( rec == &CCB2.record());
                ASSERT( rec == &CCB3.record());
                ASSERT( rec == &CCB4.record());
                ASSERT( rec == &CCB5.record());
                ASSERT( rec == &CCB6.record());

                ASSERT( rec == &CB1.record());
                ASSERT( rec == &CB2.record());
                ASSERT( rec == &CB3.record());
                ASSERT( rec == &CB4.record());
                ASSERT( rec == &CB5.record());
                ASSERT( rec == &CB6.record());
        }
        }
        {
            if (veryVerbose) {
                cout << "Ctors taking 'ChoiceArray'r." << endl;
            }

            for (int i = 0; i < NUM_CHOICE_SPECS; ++i) {
               ChoiceArray mCA = choiceArrayVec[i];
               const ChoiceArray& CA = mCA;

               const Schema&  S       = choiceArraySchemaVec[i];
               const RecDef  *rec     = &S.record(S.numRecords() - 1);
               const char    *recName = choiceArrayRecordName[i];
               for (int index = 0; index < CA.length(); ++index) {
                   if (veryVeryVerbose) {
                        T_ cout << "Original Choice: " << CA[index] << endl;
                    }

                        CCBind      ccb1(&CA, index, rec);
                        CCBind      ccb2(&CA, index, &S, recName);
                        CCBind      ccb3(&CA, index, &S, S.numRecords() - 1);
                    const CCBind&   CCB1 = ccb1;
                    const CCBind&   CCB2 = ccb2;
                    const CCBind&   CCB3 = ccb3;

                    if (veryVerbose) { T_ cout << "Non-Const bindings."
                           << endl; }

                        CBind      cb1(&mCA, index, rec);
                        CBind      cb2(&mCA, index, &S, recName);
                        CBind      cb3(&mCA, index,&S, S.numRecords() - 1);
                    const CBind&   CB1 = cb1;
                    const CBind&   CB2 = cb2;
                    const CBind&   CB3 = cb3;

                    ASSERT( &CA[index] == &CCB1.item());
                    ASSERT( &CA[index] == &CCB2.item());
                    ASSERT( &CA[index] == &CCB3.item());

                    ASSERT( &mCA.theModifiableItem(index) == &CB1.item());
                    ASSERT( &mCA.theModifiableItem(index) == &CB2.item());
                    ASSERT( &mCA.theModifiableItem(index) == &CB3.item());

                    ASSERT( mCA.theModifiableItem(index).numSelections() ==
                            CCB1.numSelections());
                    ASSERT( mCA.theModifiableItem(index).numSelections() ==
                            CCB2.numSelections());
                    ASSERT( mCA.theModifiableItem(index).numSelections() ==
                            CCB3.numSelections());

                    ASSERT( mCA.theModifiableItem(index).numSelections() ==
                                                          CB1.numSelections());
                    ASSERT( mCA.theModifiableItem(index).numSelections() ==
                                                          CB2.numSelections());
                    ASSERT( mCA.theModifiableItem(index).numSelections() ==
                                                          CB3.numSelections());

                    ASSERT( rec == &CCB1.record());
                    ASSERT( rec == &CCB2.record());
                    ASSERT( rec == &CCB3.record());

                    ASSERT( rec == &CB1.record());
                    ASSERT( rec == &CB2.record());
                    ASSERT( rec == &CB3.record());
                }
            }
        }
        if (veryVerbose) {
            cout << "Explicit destructor test" << endl;
        }
        {
           Choice mC(choiceVec[0]);  const Choice& C = mC;

           const Schema&  S   = choiceSchemaVec[0];
           const RecDef  *rec = &S.record(S.numRecords() - 1);

           {
                CCBind ccb(&C.item(), rec);
           }
           ASSERT(C == choiceVec[0]);
           ASSERT(S == choiceSchemaVec[0]);

           {
               CBind cb(&mC.item(), rec);
           }
           ASSERT(C == choiceVec[0]);
           ASSERT(S == choiceSchemaVec[0]);
        }

        if (veryVerbose) { cout << "'const'-correctness." << endl; }
        {
            Choice mC(choiceVec[0]);  const Choice& C = mC;

            const Schema&  S   = choiceSchemaVec[0];
            const RecDef  *rec = &S.record(S.numRecords() - 1);

            {
                      CCBind  ccb(&C, rec);
                const CCBind& CCB = ccb;

                const bdem_ChoiceArrayItem&   CAI = CCB.item();
                ASSERT(CAI.numSelections());
                const RecDef& RD = CCB.record();
                ASSERT(RD.numFields());
            }
            {
                      CBind  cb(&mC.item(), rec);
                const CBind& CB = cb;

                bdem_ChoiceArrayItem&  CAI = CB.item();
                ASSERT(CAI.numSelections());
            }
        }

      }

DEFINE_TEST_CASE(5) {
        // -------------------------------------------------------------------
        // TESTING COLUMN BINDING CREATORS AND ACCESSORS
        //   Test Column Binding creators and accessors.
        //
        // Concerns:
        //   - All constructors create valid objects.
        //   - The data, column index, and meta-data are hooked up correctly.
        //   - That value-returning methods return the correct result.
        //   - That references of the appropriate type and value are returned.
        //   - 'const'-correctness is preserved.
        //   - The destructor leaves the bound data and meta-data unaffected.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_Table' objects that satisfy the final
        //        records contained in the schemas from 1).
        //     3) A vector of record names.  This vector stores the
        //        *names* of the records in the schemas from 1) corresponding
        //        to the data in the tables from 2).
        //
        //   Create instances of 'bdem_ConstColumnBinding' and
        //   'bdem_ColumnBinding' (using both constructors) for each column
        //   of every element in the vector of 'bdem_Table's.  Use the direct
        //   accessors to verify that the bound table, column index, and record
        //   have been installed correctly.  Use the non-direct accessors to
        //   verify that the binding acts correctly as a proxy for the bound
        //   column.
        //
        //   Provide explicit tests for 'const'-correctness of the direct
        //   accessors and for destructor vacuity.
        //
        // Testing:
        //             bdem_ConstColumnBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstColumnBinding(const Table *, int, *field);
        //   bdem_ConstColumnBinding(const& original);
        //   ~bdem_ConstColumnBinding();
        //   bdem_ConstElemRef operator[](int) const;
        //   bdem_ConstElemRef element(int) const;
        //   bdem_ElemType::Type elemType() const;
        //   int numRows() const;
        //   int columnIndex() const;
        //   const bdem_FieldDef& field() const;
        //   const bdem_Table& table() const;
        //
        //             bdem_ColumnBinding
        //             ~~~~~~~~~~~~~~~~~~
        //   bdem_ColumnBinding(Table *, int, *field);
        //   bdem_ColumnBinding(const& original);
        //   ~bdem_ColumnBinding();
        //   bdem_ElemRef operator[](int) const;
        //   bdem_ElemRef element(int) const;
        //   bdem_Table& table() const;
        // --------------------------------------------------------------------

        DECLARE_TABLE_OBJECTS

        if (verbose)
            cout << "Testing Unhecked Column Binding Creators and Accessors"
                 << endl
                 << "======================================================"
                 << endl;

        {
            for (int i = 0; i < NUM_TABLE_SPECS; ++i) {
                Table mT(tableVec[i]);  const Table& T = mT;
                const RecDef *rec = tableRecVec[i];

                for (int col = 0; col < T.numColumns(); ++col) {
                    const FldDef& fld = rec->field(col);

                          CColBind  ccb1(&T, col, &fld);
                    const CColBind& CCB1 = ccb1;
                          CColBind  ccb2(CCB1);
                    const CColBind& CCB2 = ccb2;

                           ColBind   cb1(&mT, col, &fld);
                    const  ColBind&  CB1 = cb1;
                           ColBind   cb2(CB1);
                    const  ColBind&  CB2 = cb2;

                    ASSERT( &T == &CCB1.table());
                    ASSERT( &T == &CCB2.table());

                    ASSERT(&mT ==  &CB1.table());
                    ASSERT(&mT ==  &CB2.table());

                    ASSERT(col == CCB1.columnIndex());
                    ASSERT(col == CCB2.columnIndex());

                    ASSERT(col ==  CB1.columnIndex());
                    ASSERT(col ==  CB2.columnIndex());

                    ASSERT(&fld == &CCB1.field());
                    ASSERT(&fld == &CCB2.field());

                    ASSERT(&fld ==  &CB1.field());
                    ASSERT(&fld ==  &CB2.field());

                    ASSERT(T.numRows() == CCB1.numRows());
                    ASSERT(T.numRows() == CCB2.numRows());

                    ASSERT(T.numRows() ==  CB1.numRows());
                    ASSERT(T.numRows() ==  CB2.numRows());

                    ASSERT(T.columnType(col) == CCB1.elemType());
                    ASSERT(T.columnType(col) == CCB2.elemType());

                    for (int row = 0; row < T.numRows(); ++row) {
                        ASSERT( T[row][col] == CCB1[row]);
                        ASSERT( T[row][col] == CCB2[row]);
                        ASSERT(mT.theModifiableRow(row)[col] ==  CB1[row]);
                        ASSERT(mT.theModifiableRow(row)[col] ==  CB2[row]);

                        ASSERT( T[row][col] == CCB1.element(row));
                        ASSERT( T[row][col] == CCB2.element(row));
                        ASSERT(mT.theModifiableRow(row)[col] ==
                                                CB1.element(row));
                        ASSERT(mT.theModifiableRow(row)[col] ==
                                                CB2.element(row));
                    }
                }
            }
        }

        if (veryVerbose) { cout << "Explicit destructor test." << endl; }
        {
            Table mT(tableVec[0]);  const Table& T = mT;

            const Schema&  S   = tableSchemaVec[0];
            const RecDef  *rec = &S.record(S.numRecords() - 1);
            const FldDef&  fld = rec->field(0);

            {
                CColBind ccb(&T, 0, &fld);
            }
            ASSERT(T == tableVec[0]);
            ASSERT(S == tableSchemaVec[0]);

            {
                ColBind cb(&mT, 0, &fld);
            }
            ASSERT(T == tableVec[0]);
            ASSERT(S == tableSchemaVec[0]);
        }

        if (veryVerbose) { cout << "'const'-correctness." << endl; }
        {
            Table mT(tableVec[0]);  const Table& T = mT;

            const Schema&  S   = tableSchemaVec[0];
            const RecDef  *rec = &S.record(S.numRecords() - 1);
            const FldDef&  fld = rec->field(0);

            {
                      CColBind  ccb(&T, 0, &fld);
                const CColBind& CCB = ccb;

                const Table&  Tx = CCB.table();  ASSERT(Tx.numRows());
                const FldDef& FD = CCB.field();  ASSERT(FD.elemType());
            }
            {
                      ColBind  cb(&mT, 0, &fld);
                const ColBind& CB = cb;

                Table& Tx = CB.table();          ASSERT(Tx.numRows());
            }
        }
      }

DEFINE_TEST_CASE(4) {
        // -------------------------------------------------------------------
        // TESTING TABLE BINDING CREATORS AND DIRECT ACCESSORS
        //   Test Table Binding creators and direct accessors.
        //
        // Concerns:
        //   - All constructors create valid objects.
        //   - The data and meta-data are hooked up correctly.
        //   - That value-returning methods return the correct result.
        //   - That references of the appropriate type and value are returned.
        //   - 'const'-correctness is preserved.
        //   - The destructor leaves the bound data and meta-data unaffected.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_Table' objects that satisfy the final
        //        records contained in the schemas from 1).
        //     3) A vector of record names.  This vector stores the
        //        *names* of the records in the schemas from 1) corresponding
        //        to the data in the tables from 2).
        //
        //   Create instances of 'bdem_ConstTableBinding' and
        //   'bdem_TableBinding' (using all constructors) for every element in
        //   the vector of 'bdem_Table's.  Use the direct accessors to verify
        //   that the bound table and record have been installed correctly.
        //
        //   Provide explicit tests for 'const'-correctness of the direct
        //   accessors and for destructor vacuity.
        //
        // Testing:
        //             bdem_ConstTableBinding
        //             ~~~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstTableBinding(const Table *, *record);
        //   bdem_ConstTableBinding(const Table *, *schema, *name);
        //   bdem_ConstTableBinding(const Table *, *schema, int);
        //   bdem_ConstTableBinding(const& original);
        //   ~bdem_ConstTableBinding();
        //   const bdem_Row& operator[](int) const;
        //   int numColumns() const;
        //   int numRows() const;
        //   const bdem_RecordDef& record() const;
        //   const bdem_Table& table() const;
        //
        //             bdem_TableBinding
        //             ~~~~~~~~~~~~~~~~~
        //   bdem_TableBinding(Table *, *record);
        //   bdem_TableBinding(Table *, *schema, *name);
        //   bdem_TableBinding(Table *, *schemaint);
        //   bdem_TableBinding(const& original);
        //   ~bdem_TableBinding();
        //   bdem_Row& operator[](int) const;
        //   bdem_Table& table() const;
        // --------------------------------------------------------------------

        DECLARE_TABLE_OBJECTS

        if (verbose)
            cout
            << "Testing Table Binding Creators & Direct Accessors"
            << endl
            << "================================================="
            << endl;

        {
            for (int i = 0; i < NUM_TABLE_SPECS; ++i) {
                Table mT(tableVec[i]);  const Table& T = mT;

                const Schema&  S       = tableSchemaVec[i];
                const RecDef&  rec     = *tableRecVec[i];
                const char    *recName = tableRecordName[i];

                      CTblBind  ctb1(&T, &rec);
                const CTblBind& CTB1 = ctb1;

                      CTblBind  ctb2(&T, &S, recName);
                      CTblBind  ctb3(&T, &S, S.recordIndex(recName));
                      CTblBind  ctb4(CTB1);

                const CTblBind& CTB2 = ctb2;
                const CTblBind& CTB3 = ctb3;
                const CTblBind& CTB4 = ctb4;

                       TblBind   tb1(&mT, &rec);
                const  TblBind&  TB1 = tb1;

                       TblBind   tb2(&mT, &S, recName);
                       TblBind   tb3(&mT, &S, S.recordIndex(recName));
                       TblBind   tb4(TB1);

                const  TblBind&  TB2 = tb2;
                const  TblBind&  TB3 = tb3;
                const  TblBind&  TB4 = tb4;

                ASSERT( &T == &CTB1.table());
                ASSERT( &T == &CTB2.table());
                ASSERT( &T == &CTB3.table());
                ASSERT( &T == &CTB4.table());

                ASSERT(&mT ==  &TB1.table());
                ASSERT(&mT ==  &TB2.table());
                ASSERT(&mT ==  &TB3.table());
                ASSERT(&mT ==  &TB4.table());

                ASSERT(&rec == &CTB1.record());
                ASSERT(&rec == &CTB2.record());
                ASSERT(&rec == &CTB3.record());
                ASSERT(&rec == &CTB4.record());

                ASSERT(&rec ==  &TB1.record());
                ASSERT(&rec ==  &TB2.record());
                ASSERT(&rec ==  &TB3.record());
                ASSERT(&rec ==  &TB4.record());

                ASSERT(T.numRows() == CTB1.numRows());
                ASSERT(T.numRows() == CTB2.numRows());
                ASSERT(T.numRows() == CTB3.numRows());
                ASSERT(T.numRows() == CTB4.numRows());

                ASSERT(T.numRows() ==  TB1.numRows());
                ASSERT(T.numRows() ==  TB2.numRows());
                ASSERT(T.numRows() ==  TB3.numRows());
                ASSERT(T.numRows() ==  TB4.numRows());

                ASSERT(T.numColumns() == CTB1.numColumns());
                ASSERT(T.numColumns() == CTB2.numColumns());
                ASSERT(T.numColumns() == CTB3.numColumns());
                ASSERT(T.numColumns() == CTB4.numColumns());

                ASSERT(T.numColumns() ==  TB1.numColumns());
                ASSERT(T.numColumns() ==  TB2.numColumns());
                ASSERT(T.numColumns() ==  TB3.numColumns());
                ASSERT(T.numColumns() ==  TB4.numColumns());

                for (int row = 0; row < T.numRows(); ++row) {
                    ASSERT( &T[row] == &CTB1[row]);
                    ASSERT( &T[row] == &CTB2[row]);
                    ASSERT( &T[row] == &CTB3[row]);
                    ASSERT( &T[row] == &CTB4[row]);

                    ASSERT(&mT.theModifiableRow(row) ==  &TB1[row]);
                    ASSERT(&mT.theModifiableRow(row) ==  &TB2[row]);
                    ASSERT(&mT.theModifiableRow(row) ==  &TB3[row]);
                    ASSERT(&mT.theModifiableRow(row) ==  &TB4[row]);
                }
            }
        }

        if (veryVerbose) { cout << "Explicit destructor test." << endl; }
        {
            Table mT(tableVec[0]);  const Table& T = mT;

            const Schema&  S   = tableSchemaVec[0];
            const RecDef  *rec = tableRecVec[0];

            {
                CTblBind ctb(&T, rec);
            }
            ASSERT(T == tableVec[0]);
            ASSERT(S == tableSchemaVec[0]);

            {
                TblBind tb(&mT, rec);
            }
            ASSERT(T == tableVec[0]);
            ASSERT(S == tableSchemaVec[0]);
        }

        if (veryVerbose) { cout << "'const'-correctness." << endl; }
        {
            Table mT(tableVec[0]);  const Table& T = mT;

            const Schema&  S   = tableSchemaVec[0];
            const RecDef  *rec = tableRecVec[0];

            {
                      CTblBind  ctb(&T, rec);
                const CTblBind& CTB = ctb;

                const Row&    R  = CTB[0];         ASSERT(R.length());
                const Table&  Tx = CTB.table();    ASSERT(Tx.numRows());
                const RecDef& RD = CTB.record();   ASSERT(RD.numFields());
            }
            {
                      TblBind  tb(&mT, rec);
                const TblBind& TB = tb;

                Row&   R  = TB[0];                 ASSERT(R.length());
                Table& Tx = TB.table();            ASSERT(Tx.numRows());
            }
        }
      }

DEFINE_TEST_CASE(3) {
        // -------------------------------------------------------------------
        // TESTING ROW BINDING CREATORS AND DIRECT ACCESSORS
        //   Test Row Binding creators and direct accessors.
        //
        // Concerns:
        //   - All constructors create valid objects.
        //   - The data and meta-data are hooked up correctly.
        //   - That value-returning methods return the correct result.
        //   - That references of the appropriate type and value are returned.
        //   - The destructor leaves the bound data and meta-data unaffected.
        //   - 'const'-correctness is preserved.
        //
        // Plan:
        //   The data used for this test is a collection of vectors as follows:
        //     1) A vector of 'bdem_Schema' objects.
        //     2) A vector of 'bdem_List' objects that satisfy the final
        //        records contained in the schemas from 1).
        //     3) A vector of record names.  This vector stores the *names* of
        //        the records in the schemas from 1) corresponding to the data
        //        in the lists from 2).
        //     4) A second vector of 'bdem_Schema' objects.
        //     5) A vector of 'bdem_Table' objects that satisfy the final
        //        records contained in the schemas from 4).
        //     6) A second vector of record names.  This vector stores the
        //        *names* of the records in the schemas from 4) corresponding
        //        to the data in the tables from 5).
        //
        //   Create instances of 'bdem_ConstRowBinding' and 'bdem_RowBinding'
        //   (using all constructors) for every element in the vector of
        //   'bdem_List's, and each row of every element in the vector of
        //   'bdem_Table's.  Use the direct accessors to verify that the bound
        //   row and record are installed correctly.
        //
        //   Provide explicit tests for 'const'-correctness of the direct
        //   accessors and for destructor vacuity.
        //
        // Testing:
        //             bdem_ConstRowBinding
        //             ~~~~~~~~~~~~~~~~~~~~
        //   bdem_ConstRowBinding(const Row *, *record);
        //   bdem_ConstRowBinding(const Row *, *schema, *name);
        //   bdem_ConstRowBinding(const Row *, *schema, int);
        //   bdem_ConstRowBinding(const List *, *record);
        //   bdem_ConstRowBinding(const List *, *schema, *name);
        //   bdem_ConstRowBinding(const List *, *schema, int,
        //   bdem_ConstRowBinding(const Table *, int, *record);
        //   bdem_ConstRowBinding(const Table *, int, *schema, *name);
        //   bdem_ConstRowBinding(const Table *, int, *schema, int);
        //   bdem_ConstRowBinding(const& original);
        //   ~bdem_ConstRowBinding();
        //   int length() const;
        //   const bdem_RecordDef& record() const;
        //   const bdem_Row& row() const;
        //
        //             bdem_RowBinding
        //             ~~~~~~~~~~~~~~~
        //   bdem_RowBinding(Row *, *record);
        //   bdem_RowBinding(Row *, *schema, *name);
        //   bdem_RowBinding(Row *, *schema, int);
        //   bdem_RowBinding(List *, *record);
        //   bdem_RowBinding(List *, *schema, *name);
        //   bdem_RowBinding(List *, *schema, int,
        //   bdem_RowBinding(Table *, int, *record);
        //   bdem_RowBinding(Table *, int, *schema, *name);
        //   bdem_RowBinding(Table *, int, *schema, int);
        //   bdem_RowBinding(const& original);
        //   ~bdem_RowBinding();
        //   bdem_Row& row() const;
        // --------------------------------------------------------------------

        DECLARE_TABLE_OBJECTS

        if (verbose)
            cout << "Testing Row Binding Creators & Direct Accessors"
                 << endl
                 << "==============================================="
                 << endl;

        {
            if (veryVerbose) {
                cout << "Ctors taking 'row' or 'list' pointer." << endl;
            }

            for (int i = 0; i < NUM_LIST_SPECS; ++i) {
                List mL = listVec[i];  const List& L = mL;

                const Schema&  S       = listSchemaVec[i];
                const RecDef  *rec     = &S.record(S.numRecords() - 1);
                const char    *recName = listRecordName[i];

                      CRowBind  crb1(&L.row(), rec);
                const CRowBind& CRB1 = crb1;

                      CRowBind  crb2(&L.row(), &S, recName);
                      CRowBind  crb3(&L.row(), &S, S.numRecords() - 1);
                      CRowBind  crb4(&L, rec);
                      CRowBind  crb5(&L, &S, recName);
                      CRowBind  crb6(&L, &S, S.numRecords() - 1);
                      CRowBind  crb7(crb1);

                const CRowBind& CRB2 = crb2;
                const CRowBind& CRB3 = crb3;
                const CRowBind& CRB4 = crb4;
                const CRowBind& CRB5 = crb5;
                const CRowBind& CRB6 = crb6;
                const CRowBind& CRB7 = crb7;

                if (veryVerbose) { T_ cout << "Non-Const bindings." << endl; }

                       RowBind   rb1(&mL.row(), rec);
                const  RowBind&  RB1 = rb1;

                       RowBind   rb2(&mL.row(), &S, recName);
                       RowBind   rb3(&mL.row(), &S, S.numRecords() - 1);
                       RowBind   rb4(&mL, rec);
                       RowBind   rb5(&mL, &S, recName);
                       RowBind   rb6(&mL, &S, S.numRecords() - 1);
                       RowBind   rb7(rb1);

                const  RowBind&  RB2 = rb2;
                const  RowBind&  RB3 = rb3;
                const  RowBind&  RB4 = rb4;
                const  RowBind&  RB5 = rb5;
                const  RowBind&  RB6 = rb6;
                const  RowBind&  RB7 = rb7;

                ASSERT( &L.row() == &CRB1.row());
                ASSERT( &L.row() == &CRB2.row());
                ASSERT( &L.row() == &CRB3.row());
                ASSERT( &L.row() == &CRB4.row());
                ASSERT( &L.row() == &CRB5.row());
                ASSERT( &L.row() == &CRB6.row());
                ASSERT( &L.row() == &CRB7.row());

                ASSERT(&mL.row() ==  &RB1.row());
                ASSERT(&mL.row() ==  &RB2.row());
                ASSERT(&mL.row() ==  &RB3.row());
                ASSERT(&mL.row() ==  &RB4.row());
                ASSERT(&mL.row() ==  &RB5.row());
                ASSERT(&mL.row() ==  &RB6.row());
                ASSERT(&mL.row() ==  &RB7.row());

                ASSERT(rec == &CRB1.record());
                ASSERT(rec == &CRB2.record());
                ASSERT(rec == &CRB3.record());
                ASSERT(rec == &CRB4.record());
                ASSERT(rec == &CRB5.record());
                ASSERT(rec == &CRB6.record());
                ASSERT(rec == &CRB7.record());

                ASSERT(rec ==  &RB1.record());
                ASSERT(rec ==  &RB2.record());
                ASSERT(rec ==  &RB3.record());
                ASSERT(rec ==  &RB4.record());
                ASSERT(rec ==  &RB5.record());
                ASSERT(rec ==  &RB6.record());
                ASSERT(rec ==  &RB7.record());

                ASSERT(rec->numFields() == CRB1.length());
                ASSERT(rec->numFields() == CRB2.length());
                ASSERT(rec->numFields() == CRB3.length());
                ASSERT(rec->numFields() == CRB4.length());
                ASSERT(rec->numFields() == CRB5.length());
                ASSERT(rec->numFields() == CRB6.length());
                ASSERT(rec->numFields() == CRB7.length());

                ASSERT(rec->numFields() ==  RB1.length());
                ASSERT(rec->numFields() ==  RB2.length());
                ASSERT(rec->numFields() ==  RB3.length());
                ASSERT(rec->numFields() ==  RB4.length());
                ASSERT(rec->numFields() ==  RB5.length());
                ASSERT(rec->numFields() ==  RB6.length());
                ASSERT(rec->numFields() ==  RB7.length());
            }
        }

        if (veryVerbose) {
            cout << "Ctors taking 'table' pointer and index." << endl;
        }
        {
            for (int i = 0; i < NUM_TABLE_SPECS; ++i) {
                Table mT(tableVec[i]);  const Table& T = mT;

                const Schema&  S       = tableSchemaVec[i];
                const RecDef  *rec     = tableRecVec[i];
                const char    *recName = tableRecordName[i];

                for (int row = 0; row < T.numRows(); ++row) {
                    if (veryVeryVerbose) {
                        T_ cout << "Original Row: " << T[row] << endl;
                    }

                          CRowBind  crb1(&T, row, rec);
                          CRowBind  crb2(&T, row, &S, recName);
                          CRowBind  crb3(&T, row, &S, S.recordIndex(recName));

                    const CRowBind& CRB1 = crb1;
                    const CRowBind& CRB2 = crb2;
                    const CRowBind& CRB3 = crb3;

                           RowBind   rb1(&mT, row, rec);
                           RowBind   rb2(&mT, row, &S, recName);
                           RowBind   rb3(&mT, row, &S, S.recordIndex(recName));

                    const  RowBind&  RB1 = rb1;
                    const  RowBind&  RB2 = rb2;
                    const  RowBind&  RB3 = rb3;

                    ASSERT( &T[row] == &CRB1.row());
                    ASSERT( &T[row] == &CRB2.row());
                    ASSERT( &T[row] == &CRB3.row());

                    ASSERT(&mT.theModifiableRow(row) ==  &RB1.row());
                    ASSERT(&mT.theModifiableRow(row) ==  &RB2.row());
                    ASSERT(&mT.theModifiableRow(row) ==  &RB3.row());

                    ASSERT(rec == &CRB1.record());
                    ASSERT(rec == &CRB2.record());
                    ASSERT(rec == &CRB3.record());

                    ASSERT(rec ==  &RB1.record());
                    ASSERT(rec ==  &RB2.record());
                    ASSERT(rec ==  &RB3.record());

                    ASSERT(T.numColumns() == CRB1.length());
                    ASSERT(T.numColumns() == CRB2.length());
                    ASSERT(T.numColumns() == CRB3.length());

                    ASSERT(T.numColumns() ==  RB1.length());
                    ASSERT(T.numColumns() ==  RB2.length());
                    ASSERT(T.numColumns() ==  RB3.length());
                }
            }
        }

        if (veryVerbose) { cout << "Explicit destructor test." << endl; }
        {
            List mL(listVec[0]);  const List& L = mL;

            const Schema&  S   = listSchemaVec[0];
            const RecDef  *rec = &S.record(S.numRecords() - 1);

            {
                CRowBind crb(&L.row(), rec);
            }
            ASSERT(L == listVec[0]);
            ASSERT(S == listSchemaVec[0]);

            {
                RowBind rb(&mL.row(), rec);
            }
            ASSERT(L == listVec[0]);
            ASSERT(S == listSchemaVec[0]);
        }

        if (veryVerbose) { cout << "'const'-correctness." << endl; }
        {
            List mL(listVec[0]);  const List& L = mL;

            const Schema&  S   = listSchemaVec[0];
            const RecDef  *rec = &S.record(S.numRecords() - 1);

            {
                      CRowBind  crb(&L.row(), rec);
                const CRowBind& CRB = crb;

                const Row&    R  = CRB.row();     ASSERT(R.length());
                const RecDef& RD = CRB.record();  ASSERT(RD.numFields());
            }
            {
                      RowBind  rb(&mL.row(), rec);
                const RowBind& RB = rb;

                Row& R = RB.row();                ASSERT(R.length());
            }
        }
      }

DEFINE_TEST_CASE(2) {
        // --------------------------------------------------------------------
        // TESTING HELPER FUNCTIONS
        //   Verify that the helper functions used to create instances of
        //   'bdem_Schema', 'bdem_List', and 'bdem_Table' work correctly.
        //
        // Concerns:
        //   - Given valid input, the generator functions create correct
        //     objects.
        //   - The 'createSchema' function is implemented as a finite state
        //     machine (FSM).  All symbols must be recognized successfully, and
        //     all possible state transitions must be verified.
        //
        // Plan:
        //   This test case was copied from the test case for the generator
        //   function 'gg' in 'bdem_schema'.  First, the test case verifies
        //   that, given a spec, the 'createSchema' function creates a valid
        //   'bdem_Schema'.  Then we check that the 'createList' and
        //   'createTable' methods create correct 'bdem_List' and 'bdem_Table'
        //   instances, respectively, from records in that 'bdem_Schema'.
        //
        //   For testing 'createSchema', evaluate a series of test strings of
        //   increasing complexity to validate every major state and transition
        //   in the FSM underlying 'createSchema'.  Add extra tests to validate
        //   less-likely transitions until they are all covered.
        //
        //   After 'bdem_Schema' creation, use records in that schema to create
        //   instances of 'bdem_List' and 'bdem_Table' using the 'createList'
        //   and 'createTable' helper functions, respectively.  Verify that the
        //   element types in the lists and tables match the element types in
        //   the corresponding 'bdem_Schema' records.
        //
        // Testing:
        //   void createSchema(bdem_Schema *schema, const char *spec);
        //   void createList(bdem_List *list, const bdem_RecordDef *record);
        //   void createTable(bdem_Table *table, const bdem_RecordDef *record);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING HELPER FUNCTIONS" << endl
                          << "========================" << endl;

        const int      MAX_TYPES = 100;
        ElemType::Type tabColTypes[MAX_TYPES];
        {
            if (veryVerbose) cout << "Testing with empty string." << endl;

            Schema mS;  const Schema& S = mS;  createSchema(&mS, "");
            ASSERT(0 == S.numRecords());

            if (veryVeryVerbose) cout << S << endl;
        }

        if (veryVerbose) cout << "Testing all unconstrained types." << endl;
        {
            Schema mS;  const Schema& S = mS;

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :* %*";
                spec[5] = bdemType[i];  // replace '%' in 'spec'

                if (veryVeryVerbose) {
                    T_ cout << "Testing using " << spec << endl;
                }

                createSchema(&mS, spec);
                const RecDef *rec = &S.record(0);

                ASSERT(1              == S.numRecords());
                ASSERT(1              == rec->numFields());
                ASSERT(0              == rec->fieldName(0));
                ASSERT(csElemTypes[i] == rec->field(0).elemType());
                ASSERT(0              == rec->field(0).recordConstraint());

                List mL;  const List& L = mL;  createList(&mL, rec);

                Table mT;  const Table& T = mT;
                createTable(&mT, rec);

                ASSERT(1 == L.length());
                ASSERT(1 == T.numColumns());

                if (veryVerbose) {
                    T_ cout << "Ensure data elements match." << endl;
                }

                for (int len = 0; len < rec->numFields(); ++len) {
                    ASSERT(L[len].type()     == rec->field(len).elemType());
                    ASSERT(T.columnType(len) == rec->field(len).elemType());
                }

                if (veryVeryVerbose) {
                    cout << S << endl << L << endl << T << endl << endl;
                }
            }
        }

        {
            if (veryVerbose) cout << "All unconstrained types, with name c"
                                     " (\"c\")." << endl;

            Schema mS;  const Schema& S = mS;

            for (int i = 0; i < NUM_TYPES; ++i) {
                static char spec[] = "~ :* %c";
                spec[5] = bdemType[i];  // replace '%' in 'spec'

                if (veryVeryVerbose) {
                    T_ cout << "Testing using " << spec << endl;
                }

                createSchema(&mS, spec);
                const RecDef *rec = &S.record(0);

                LOOP_ASSERT(i, 1              == S.numRecords());
                LOOP_ASSERT(i, 0              == recName(*rec));
                LOOP_ASSERT(i, 1              == rec->numFields());
                LOOP_ASSERT(i, 0              == strCmp(rec->fieldName(0),
                                                        "c"));
                LOOP_ASSERT(i, csElemTypes[i] == rec->field(0).elemType());
                LOOP_ASSERT(i, 0              ==
                                             rec->field(0).recordConstraint());

                List mL;  const List& L = mL;  createList(&mL, rec);

                Table mT;  const Table& T = mT;
                createTable(&mT, rec);

                LOOP_ASSERT(i, 1 == L.length());
                LOOP_ASSERT(i, 1 == T.numColumns());

                if (veryVerbose) {
                    T_ cout << "Ensure data elements match." << endl;
                }

                for (int len = 0; len < rec->numFields(); ++len) {
                    ASSERT(L[len].type()     == rec->field(len).elemType());
                    ASSERT(T.columnType(len) == rec->field(len).elemType());
                }

                if (veryVeryVerbose) {
                    cout << S << endl << L << endl << T << endl << endl;
                }
            }
        }

        if (veryVerbose) cout << "Testing with all constrained types." << endl;
        {
            Schema mS;  const Schema& S = mS;

            for (int i = 0; i < NUM_CONSTR_TYPES; ++i) {
                static char spec[] = "~ :c %Ad :* %dc";
                //'c' is a CHOICE if 'd' is a CONSTRAINED CHOICE type
                const char bdemType = constrBdemType[i];
                spec[5]  = ('%' == bdemType || '@' == bdemType) ? '?' : ' ';
                spec[12] = bdemType;  // replace '%' in 'spec'

                if (veryVeryVerbose) cout << "Testing using " << spec << endl;

                createSchema(&mS, spec);
                const RecDef *rec = &S.record(0);

                LOOP_ASSERT(i, 0 == strCmp(recName(*rec), "c"));
                LOOP_ASSERT(i, 1 == rec->numFields());
                LOOP_ASSERT(i, 0 == strCmp(rec->fieldName(0), "d"));
                LOOP_ASSERT(i, csElemTypes[0] == rec->field(0).elemType());

                rec = &S.record(1);

                LOOP_ASSERT(i, 0 == recName(*rec));
                LOOP_ASSERT(i, 1 == rec->numFields());
                LOOP_ASSERT(i, 0 == strCmp(rec->fieldName(0), "d"));
                LOOP_ASSERT(i, &S.record(0) ==
                                             rec->field(0).recordConstraint());
                ASSERT(csElemTypes[32 + i]   == rec->field(0).elemType());
                if (bdemType != '%' && bdemType != '@') {
                    List mL;  const List& L = mL;  createList(&mL, rec);

                    Table mT;  const Table& T = mT;
                    createTable(&mT, rec);

                    LOOP_ASSERT(i, 2 == S.numRecords());
                    LOOP_ASSERT(i, 1 == L.length());
                    LOOP_ASSERT(i, 1 == T.numColumns());

                    if (veryVerbose) {
                        T_ cout << "Ensure data elements match." << endl;
                    }

                    for (int len = 0; len < rec->numFields(); ++len) {
                        ASSERT(L[len].type()     ==
                               rec->field(len).elemType());
                        ASSERT(T.columnType(len) ==
                               rec->field(len).elemType());
                    }

                    if (veryVeryVerbose) {
                        cout << S << endl << L << endl << T << endl << endl;
                    }
                }
            }
        }

        if (veryVerbose) cout << "Testing with a large invocation"
                                 " touching every token." << endl;
        {
            Schema mS;  const Schema& S = mS;
            bsl::string spec = "~";

            int i, j;  // loop variables declared here to keep MSVC++ happy
            for (i = 0; i < NUM_CS_NAMES; ++i) {
                spec += "\n:";
                spec += name[i];
                if (i % 2) {
                   spec += '?';  // odd 'i' are CHOICE records
                }
                for (j = 0; j < NUM_TYPES; ++j) {
                    spec += ' ';
                    spec += bdemType[j];
                    if (j < NUM_CS_NAMES) {
                        spec += name[j];
                    }
                    else {
                        // test space transition in state FLD_NAME.
                        spec += " *";
                    }
                }

                // Add constrained fields by name on previous record.
                if (i > 1) {
                       if (i % 2) {
                            spec += " +*";
                            spec += name[i - 1];

                            spec += " #*";
                            spec += name[i - 1];
                        }
                        else {
                            spec += " %*";
                            spec += name[i - 1];

                            spec += " @*";
                            spec += name[i - 1];
                        }
                }

                // Add constrained fields by index on all previous records,
                // and on current record.  Note that this can only
                // reference the first 10 records, since we use a single
                // digit for reference by index.
                for (j = 0; j < i && j <= 9; ++j) {
                    if (j % 2) {
                         // Trailing space to exercise space transition in
                         // state CFLDCONSTR.
                         spec += " %* ";
                         spec += char(j + '0');

                         // Inner space to exercise space transition in
                         // state CFLDNAME.
                         spec += " @ * ";
                         spec += char(j + '0');
                    }
                    else {
                         spec += " +* ";
                         spec += char(j + '0');

                         spec += " # *";
                         spec += char(j + '0');
                     }
                 }
            }

            if (veryVerbose) { T_ cout << "Testing using " << spec << endl; }

            createSchema(&mS, spec.data());
            ASSERT(NUM_CS_NAMES == S.numRecords());

            // Note: reducing the number of iterations of the following loop
            //       speeds up this case *enormously*.

            if (veryVeryVerbose) {T_ cout << "Schema created" << endl; }

            for (i = 0; i < 5;  ++i) {
                const RecDef *R = &S.record(i);

                LOOP_ASSERT(i,0 != R);
                LOOP_ASSERT(i,0 == strCmp(recName(*R), csName(i)));

                for (j = 0; j < NUM_TYPES; ++j) {
                    LOOP2_ASSERT(i,j,csElemTypes[j] == R->field(j).elemType());
                    LOOP2_ASSERT(i,j,0 == R->field(j).recordConstraint());
                    LOOP2_ASSERT(i,j,0 == strCmp(R->fieldName(j), csName(j)));
                }

                if (i > 1) {
                    // Two fields constrained by name on previous record.
                    int k = i % 2 ? 0 : 2;  // "+#" or "%@"
                    for (j = NUM_TYPES; j < NUM_TYPES + 1; ++j, ++k) {
                         LOOP2_ASSERT(i,j,getType(constrBdemType[k]) ==
                                                       R->field(j).elemType());
                         LOOP2_ASSERT(i,j,0 != R->field(j).recordConstraint());
                         LOOP2_ASSERT(i,j,0 ==
                               strCmp(recName(*R->field(j).recordConstraint()),
                                            csName(i - 1)));
                    }
                }

                List mL;  const List& L = mL;  createList(&mL, R);

                Table mT;  const Table& T = mT;
                createTable(&mT, R);

                ASSERT(R->numFields() == L.length());
                ASSERT(R->numFields() == T.numColumns());

                if (veryVerbose) {
                    T_ cout << "Ensure data elements match." << endl;
                }

                for (int len = 0; len < R->numFields(); ++len) {
                    ASSERT(L.elemType(len)   == R->field(len).elemType());
                    ASSERT(T.columnType(len) == R->field(len).elemType());
                }
                if (veryVeryVerbose) {
                    cout << L << endl << T << endl << endl;
                }
            }
        }

        {
            Schema mS;  const Schema& S = mS;  createSchema(&mS, ":aAa~:a");

            if (veryVerbose) cout << "Force a test of remove transition in"
                                     " state FLD_END." << endl;

            const RecDef *rec = &S.record(S.numRecords() - 1);

            ASSERT(1 == S.numRecords());
            ASSERT(0 == strCmp(recName(S.record(0)), "a"));

            List mL;  const List& L = mL;  createList(&mL, rec);

            Table mT;  const Table& T = mT;
            createTable(&mT, rec);

            if (veryVeryVerbose) {
                cout << S << endl << L << endl << T << endl << endl;
            }
        }
      }

DEFINE_TEST_CASE(1) {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   Note: This test exercises basic functionality, but tests nothing!
        //
        // Concerns:
        //   That basic essential functionality is operational.
        //
        // Plan:
        //   From a 'bdem_Schema', create a 'bdem_List' that satisfies that
        //   schema, and a 'bdem_Table' having rows that satisfy the schema.
        //   Using the 'bdem_Schema', 'bdem_List', and 'bdem_Table', construct
        //   instances of all 12 binding classes.  Using accessors, verify that
        //   the bindings act correctly as proxies for their underlying data.
        //   Copy construct other bindings and similarly verify that they
        //   correctly proxy their underlying data.  Finally, create bindings
        //   from the methods that return bindings by value and verify the
        //   correctness of the returned bindings.
        //
        // Tactics:
        //   Ad-Hoc testing.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        // --------------------------- Test schema ----------------------------
        //
        //            +-----------------------------------------+
        //            |{                                        |
        //            |    RECORD "ADDRESS" {                   |
        //            |        STRING            "address";     |
        //            |    }                                    |
        //            |                                         |
        //            |    RECORD "INTLPHONE" {                 |
        //            |        STRING            "countryCode"; |
        //            |        STRING            "number";      |
        //            |    }                                    |
        //            |                                         |
        //            |    RECORD:CHOICE "PHONE" {              |
        //            |        STRING            "usNumber";    |
        //            |        LIST<"INTLPHONE"> "intlNumber";  |
        //            |    }                                    |
        //            |                                         |
        //            |    ENUMERATION "SEX" {                  |
        //            |        STRING            "sex";         |
        //            |    }                                    |
        //            |                                         |
        //            |    RECORD "EMPLOYEE" {                  |
        //            |        STRING            "empName";     |
        //            |        STRING            "empId";       |
        //            |        DATE              "dateOfHire";  |
        //            |        LIST<"ADDRESS">   "homeAddr";    |
        //            |        CHOICE<"PHONE">   "phoneNumber"; |
        //            |        STRING<"SEX">     "sex";         |
        //            |    }                                    |
        //            |                                         |
        //            |    ENUMERATION "LEAGUES" {              |
        //            |        STRING            "league";      |
        //            |    }                                    |
        //            |                                         |
        //            |    ENUMERATION "DIVISIONS" {            |
        //            |        INT               "division";    |
        //            |    }                                    |
        //            |                                         |
        //            |    RECORD:CHOICE "LEVEL" {              |
        //            |        STRING<"LEAGUES"> "league";      |
        //            |        INT<"DIVISIONS">  "division";    |
        //            |    }                                    |
        //            |                                         |
        //            |    RECORD "TEAM" {                      |
        //            |        STRING             "teamName"    |
        //            |        INT                "teamId";     |
        //            |        DOUBLE             "revenue";    |
        //            |        CHOICE<"LEVEL">    "level";      |
        //            |        LIST<"EMPLOYEE">   "leader";     |
        //            |        TABLE<"EMPLOYEE">  "members";    |
        //            |        CHOICE_ARRAY<"PHONE">            |
        //            |                    "officeNumbers";     |
        //            |    }                                    |
        //            |}                                        |
        //            +-----------------------------------------+

        // Create the above schema

        Schema S;

        RecDef *pAddress = S.createRecord("ADDRESS");
                pAddress->appendField(ElemType::BDEM_STRING, "address");

        RecDef *pIntlPhone = S.createRecord("INTLPHONE");
                pIntlPhone->appendField(ElemType::BDEM_STRING,"countryCode");
                pIntlPhone->appendField(ElemType::BDEM_STRING,"number");

        RecDef *pPhone = S.createRecord("PHONE",RecDef::BDEM_CHOICE_RECORD);
                pPhone->appendField(ElemType::BDEM_STRING,"usNumber");
                pPhone->appendField(ElemType::BDEM_LIST,
                                    pIntlPhone,
                                    "intlNumber");

        bdem_EnumerationDef *pSex = S.createEnumeration("SEX");
                pSex->addEnumerator("Male");
                pSex->addEnumerator("Female");

        RecDef *pEmployee = S.createRecord("EMPLOYEE");
                pEmployee->appendField(ElemType::BDEM_STRING, "empName");
                pEmployee->appendField(ElemType::BDEM_INT, "empId");
                pEmployee->appendField(ElemType::BDEM_DATE, "dateOfHire");
                pEmployee->appendField(ElemType::BDEM_LIST,
                                       pAddress,
                                       "homeAddr");
                pEmployee->appendField(ElemType::BDEM_CHOICE,pPhone,
                                       "phoneNumber");
                pEmployee->appendField(ElemType::BDEM_STRING, pSex, "sex");

        bdem_EnumerationDef *pLeagues = S.createEnumeration("LEAGUES");
                pLeagues->addEnumerator("Minor");
                pLeagues->addEnumerator("Major");

        bdem_EnumerationDef *pDivision = S.createEnumeration("DIVISIONS");
                pDivision->addEnumerator("Divsion 1", 1);
                pDivision->addEnumerator("Divsion 2");
                pDivision->addEnumerator("Divsion 3");
                pDivision->addEnumerator("Divsion 4");

        RecDef *pLevels = S.createRecord("LEVELS", RecDef::BDEM_CHOICE_RECORD);
               pLevels->appendField(ElemType::BDEM_STRING, pLeagues, "league");
               pLevels->appendField(ElemType::BDEM_INT, pDivision, "division");

        RecDef *pTeam = S.createRecord("TEAM");
                pTeam->appendField(ElemType::BDEM_STRING, "teamName");
                pTeam->appendField(ElemType::BDEM_INT, "teamId");
                pTeam->appendField(ElemType::BDEM_DOUBLE, "revenue");
                pTeam->appendField(ElemType::BDEM_CHOICE, pLevels, "level");
                pTeam->appendField(ElemType::BDEM_LIST,  pEmployee, "leader");
                pTeam->appendField(ElemType::BDEM_TABLE, pEmployee, "members");

                pTeam->appendField(ElemType::BDEM_CHOICE_ARRAY, pPhone,
                                   "officeNumbers");

        // -------------------------- Needed data ----------------------------

        // Team non-aggregate info

        const bsl::string teamName1 = "Yankees";
        const int         teamId1   = 101;
        const double      revenue1  = 345.35;
        const bsl::string league1("Major");
        const int         leagueAsInt1 = 1;

        const bsl::string teamName2 = "Mets";
        const int         teamId2   = 26;
        const double      revenue2  = 9876.99;
        const bsl::string league2("Minor");
        const int         leagueAsInt2 = 0;

        // Team aggregate info

        const bdem_ElemType::Type levelTypes[] = {
            bdem_ElemType::BDEM_STRING,
            bdem_ElemType::BDEM_INT
        };
        const int numLevelTypes =
            sizeof(levelTypes)/sizeof(bdem_ElemType::Type);

        Choice level1(levelTypes, numLevelTypes);
        level1.makeSelection(0).theModifiableString() = league1;

        Choice level2(levelTypes, numLevelTypes);
        level2.makeSelection(0).theModifiableString() = league2;

        ChoiceArray levelArray1(levelTypes, numLevelTypes);
        levelArray1.appendItem(level1); levelArray1.appendItem(level2);

        // Members Info

        const bsl::string addr1 = "124 Cobble Road, London";
        const bsl::string addr2 = "1354 Pittsburg, Pennsylvania";
        const bsl::string addr3 = "9 Seattle, Washington";

        const bdem_ElemType::Type numberTypes[] = {
            bdem_ElemType::BDEM_STRING,
            bdem_ElemType::BDEM_LIST
        };
        const int numNumberTypes =
            sizeof(numberTypes)/sizeof(bdem_ElemType::Type);
        const int NUM_CHOICES = numNumberTypes;

        List addrList1;  addrList1.appendString(addr1);
        List addrList2;  addrList2.appendString(addr2);
        List addrList3;  addrList3.appendString(addr3);

        const bsl::string phoneString1("011 01 532 1241");
        const bsl::string phoneString2("412 512 5555");
        const bsl::string phoneString3("402 142 1436");
        List  numberList1; numberList1.appendString("UK");
        numberList1.appendString(phoneString1);
        Choice number1(numberTypes,numNumberTypes);
        number1.makeSelection(1).theModifiableList() = numberList1;
        Choice number2(numberTypes,numNumberTypes);
        number2.makeSelection(0).theModifiableString() = phoneString2;
        Choice number3(numberTypes,numNumberTypes);
        number3.makeSelection(0).theModifiableString() = phoneString3;

        // the following constants help construct loops for testing
        // the Choices number1,number2,..
        Choice choices[] = { number1, number2, number3 };
        const int NUM_CHOICE_TESTS = sizeof choices / sizeof *choices;
        const ElemType::Type  selectorTypes[] = { ElemType::BDEM_LIST,
                                                  ElemType::BDEM_STRING,
                                                  ElemType::BDEM_STRING };
        const int             selectorIndex[] = { 1 , 0, 0 };
        const bsl::string phoneStrings[] = {phoneString1,
                                            phoneString2,
                                            phoneString3};

        const bsl::string memberName1("Stan");
        const int         memberId1 = 0100;
        const bdet_Date   memberHireDate1(1994, 2, 2);
        const bsl::string memberSex1("Male");
        const int         memberSexAsInt1 = 0;

        const bsl::string memberName2("Scott");
        const int         memberId2 = 0200;
        const bdet_Date   memberHireDate2(1998, 3, 3);
        const bsl::string memberSex2("Male");
        const int         memberSexAsInt2 = 0;

        const bsl::string memberName3("Bjarne");
        const int         memberId3 = 0001;
        const bdet_Date   memberHireDate3(1984, 1, 1);
        const bsl::string memberSex3("Female");
        const int         memberSexAsInt3 = 1;

        // List of leaders (members)

        const bsl::string leaderName1(memberName1);
        const int         leaderId1 = memberId1;
        const bdet_Date   leaderHireDate1(memberHireDate1);
        const bsl::string leaderSex1(memberSex1);
        const int         leaderSexAsInt1 = memberSexAsInt1;

        const bsl::string leaderName2(memberName2);
        const int         leaderId2 = memberId2;
        const bdet_Date   leaderHireDate2(memberHireDate2);
        const bsl::string leaderSex2(memberSex2);
        const int         leaderSexAsInt2 = memberSexAsInt2;

        const bsl::string leaderName3(memberName3);
        const int         leaderId3 = memberId3;
        const bdet_Date   leaderHireDate3(memberHireDate3);
        const bsl::string leaderSex3(memberSex3);
        const int         leaderSexAsInt3 = memberSexAsInt3;

        List  leader1;    leader1.appendString(leaderName1);
                          leader1.appendInt(leaderId1);
                          leader1.appendDate(leaderHireDate1);
                          leader1.appendList(addrList1);
                          leader1.appendChoice(number1);
                          leader1.appendString(leaderSex1);

        List  leader2;    leader2.appendString(leaderName2);
                          leader2.appendInt(leaderId2);
                          leader2.appendDate(leaderHireDate2);
                          leader2.appendList(addrList2);
                          leader2.appendChoice(number2);
                          leader2.appendString(leaderSex2);

        List  leader3;    leader3.appendString(leaderName3);
                          leader3.appendInt(leaderId3);
                          leader3.appendDate(leaderHireDate3);
                          leader3.appendList(addrList3);
                          leader3.appendChoice(number3);
                          leader3.appendString(leaderSex3);

        List members[] = { leader1, leader2, leader3 };

        const int NUM_MEMBERS = sizeof members / sizeof *members;
        const int NUM_ROWS    = NUM_MEMBERS;
        const int NUM_COLUMNS = leader1.length();

        const bdem_ElemType::Type colTypes[] = {
            bdem_ElemType::BDEM_STRING,
            bdem_ElemType::BDEM_INT,
            bdem_ElemType::BDEM_DATE,
            bdem_ElemType::BDEM_LIST,
            bdem_ElemType::BDEM_CHOICE,
            bdem_ElemType::BDEM_STRING
        };
        const int numColTypes = sizeof colTypes / sizeof *colTypes;

        Table memberTable1(colTypes, numColTypes);
              memberTable1.appendRow(leader1);
              memberTable1.appendRow(leader2);
              memberTable1.appendRow(leader3);

        const bsl::string phoneString4("011 01 532 1241");
        const bsl::string phoneString5("412 512 5555");
        const bsl::string phoneString6("402 142 1436");
        List  UKOfficeList;  UKOfficeList.appendString("UK");
        UKOfficeList.appendString(phoneString4);;
        Choice UKOffice(numberTypes, numNumberTypes);
        UKOffice.makeSelection(1).theModifiableList() = UKOfficeList;
        Choice NYOffice(numberTypes, numNumberTypes);
        NYOffice.makeSelection(0).theModifiableString() = phoneString5;
        Choice PGHOffice(numberTypes, numNumberTypes);
        PGHOffice.makeSelection(0).theModifiableString() = phoneString6;

        ChoiceArray officeArray1(numberTypes, numNumberTypes);
        officeArray1.appendItem(UKOffice);  officeArray1.appendItem(NYOffice);
        officeArray1.appendItem(PGHOffice);

        // Data used in modification

        Table memberTable2(colTypes, numColTypes);
              memberTable2.appendRow(leader2);
              memberTable2.appendRow(leader3);
              memberTable2.appendRow(leader1);

        List  CAOfficeList;  CAOfficeList.appendString("CA");
        CAOfficeList.appendString("012 20 431 4245");
        Choice CAOffice(numberTypes, numNumberTypes);
        CAOffice.makeSelection(1).theModifiableList() = CAOfficeList;
        Choice LAOffice(numberTypes, numNumberTypes);
        LAOffice.makeSelection(0).theModifiableString() = "410 123 4566";
        Choice SFOffice(numberTypes, numNumberTypes);
        SFOffice.makeSelection(0).theModifiableString() = "415 321 5674";

        ChoiceArray officeArray2(numberTypes, numNumberTypes);
        officeArray2.appendItem(CAOffice);  officeArray2.appendItem(LAOffice);
        officeArray2.appendItem(SFOffice);

        // Final list
        List teamList;
             teamList.appendString(teamName1);
             teamList.appendInt(teamId1);
             teamList.appendDouble(revenue1);
             teamList.appendChoice(level1);
             teamList.appendList(leader1);
             teamList.appendTable(memberTable1);
             teamList.appendChoiceArray(officeArray1);

        int NUM_ELEMENTS = teamList.length();

        // ----------------------- BREATHING TEST CODE ------------------------

        if (verbose) cout << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (veryVerbose) cout << "Row Bindings" << endl;
        {
            if (veryVerbose) { T_ cout << "Creators" << endl; }

            CRowBind crb(&teamList, &S, "TEAM");  const CRowBind& CRB = crb;
             RowBind  rb(&teamList, &S, "TEAM");  const  RowBind&  RB =  rb;

            if (veryVerbose) {
                T2_ cout << "Verify lengths of bindings." << endl;
            }

            ASSERT(NUM_ELEMENTS ==  CRB.length());
            ASSERT(NUM_ELEMENTS ==   RB.length());

            if (veryVerbose) { T_ cout << "Accessors" << endl; }
            {
                ASSERT(teamName1    == CRB.theString("teamName"));
                ASSERT(teamId1      == CRB.theInt("teamId"));
                ASSERT(revenue1     == CRB.theDouble("revenue"));
                ASSERT(leader1      == CRB.theList("leader"));
                ASSERT(memberTable1 == CRB.theTable("members"));
                ASSERT(officeArray1 == CRB.theChoiceArray("officeNumbers"));

                ASSERT(teamName1    ==  RB.theString("teamName"));
                ASSERT(teamId1      ==  RB.theInt("teamId"));
                ASSERT(revenue1     ==  RB.theDouble("revenue"));
                ASSERT(leader1      ==  RB.theList("leader"));
                ASSERT(memberTable1 ==  RB.theTable("members"));
                ASSERT(officeArray1 ==  RB.theChoiceArray("officeNumbers"));

                if (veryVerbose) {
                    T2_ cout << "Modify data and verify." << endl;
                }

                RB.theModifiableString("teamName") = teamName2;
                RB.theModifiableInt("teamId")      = teamId2;
                RB.theModifiableDouble("revenue")  = revenue2;
                RB.theModifiableList("leader")     = leader2;
                RB.theModifiableTable("members")   = memberTable2;
                RB.theModifiableChoiceArray("officeNumbers") = officeArray2;

                ASSERT(teamName2    == RB.theString("teamName"));
                ASSERT(teamId2      == RB.theInt("teamId"));
                ASSERT(revenue2     == RB.theDouble("revenue"));
                ASSERT(leader2      == RB.theList("leader"));
                ASSERT(memberTable2 == RB.theTable("members"));
                ASSERT(officeArray2 == RB.theChoiceArray("officeNumbers"));
            }

            if (veryVerbose) { T_ cout << "Manipulators" << endl; }
            {
                if (veryVerbose) {
                    T2_ cout << "Copy construct bindings" << endl;
                }

                CRowBind crbx(crb);  const CRowBind& CRBx = crbx;
                 RowBind  rbx(rb);   const RowBind&   RBx =  rbx;

                ASSERT(  CRBx.length() ==  CRB.length());
                ASSERT(   RBx.length() ==   RB.length());

                ASSERT(teamName2    == CRBx.theString("teamName"));
                ASSERT(teamId2      == CRBx.theInt("teamId"));
                ASSERT(revenue2     == CRBx.theDouble("revenue"));
                ASSERT(leader2      == CRBx.theList("leader"));
                ASSERT(memberTable2 == CRBx.theTable("members"));
                ASSERT(officeArray2 == CRBx.theChoiceArray("officeNumbers"));

                ASSERT(teamName2    ==  RBx.theString("teamName"));
                ASSERT(teamId2      ==  RBx.theInt("teamId"));
                ASSERT(revenue2     ==  RBx.theDouble("revenue"));
                ASSERT(leader2      ==  RBx.theList("leader"));
                ASSERT(memberTable2 ==  RBx.theTable("members"));
                ASSERT(officeArray2 ==  RBx.theChoiceArray("officeNumbers"));

                RBx.theModifiableString("teamName") = teamName1;
                RBx.theModifiableInt("teamId")      = teamId1;
                RBx.theModifiableDouble("revenue")  = revenue1;
                RBx.theModifiableList("leader")     = leader1;
                RBx.theModifiableTable("members")   = memberTable1;
                RBx.theModifiableChoiceArray("officeNumbers") = officeArray1;
            }

            if (veryVerbose) {
                T_ cout << "Functions Returning Bindings "
                        << "and enumerationAs functions" << endl;
            }
            {
                       CRowBind  crbx(crb.rowBinding("leader"));
                const  CRowBind& CRBx = crbx;

                        RowBind   rbx(RB.rowBinding("leader"));
                const   RowBind&  RBx = rbx;

                ASSERT(leaderName1     == CRBx.theString("empName"));
                ASSERT(leaderId1       == CRBx.theInt("empId"));
                ASSERT(leaderHireDate1 == CRBx.theDate("dateOfHire"));
                ASSERT(leaderSex1      == CRBx.enumerationAsString("sex"));
                ASSERT(leaderSex1      == CRBx.enumerationAsString(5));
                ASSERT(leaderSexAsInt1 == CRBx.enumerationAsInt("sex"));
                ASSERT(leaderSexAsInt1 == CRBx.enumerationAsInt(5));

                ASSERT(leaderName1     == RBx.theString("empName"));
                ASSERT(leaderId1       == RBx.theInt("empId"));
                ASSERT(leaderHireDate1 == RBx.theDate("dateOfHire"));
                ASSERT(leaderSex1      == RBx.enumerationAsString("sex"));
                ASSERT(leaderSex1      == RBx.enumerationAsString(5));
                ASSERT(leaderSexAsInt1 == RBx.enumerationAsInt("sex"));
                ASSERT(leaderSexAsInt1 == RBx.enumerationAsInt(5));

                if (veryVerbose) {
                    T2_ cout << "Modify the new binding." << endl;
                }

                rbx.theModifiableString("empName")  = leaderName3;
                rbx.theModifiableInt("empId")       = leaderId3;
                rbx.theModifiableDate("dateOfHire") = leaderHireDate3;
                rbx.theModifiableList("homeAddr")   = addrList3;
                rbx.theModifiableChoice("phoneNumber") = number3;
                rbx.setEnumeration("sex", leaderSex3);

                ASSERT(leaderName3     == RB.theList("leader").theString(0));
                ASSERT(leaderId3       == RB.theList("leader").theInt(1));
                ASSERT(leaderHireDate3 == RB.theList("leader").theDate(2));
                ASSERT(addrList3       == RB.theList("leader").theList(3));
                ASSERT(number3         == RB.theList("leader").theChoice(4));
                ASSERT(leaderSex3      == RB.theList("leader").theString(5));
            }
        }
      }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER)                                                     \
  case NUMBER: testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose ); break
        CASE(33);
        CASE(32);
        CASE(31);
        CASE(30);
        CASE(29);
        CASE(28);
        CASE(27);
        CASE(26);
        CASE(25);
        CASE(24);
        CASE(23);
        CASE(22);
        CASE(21);
        CASE(20);
        CASE(19);
        CASE(18);
        CASE(17);
        CASE(16);
        CASE(15);
        CASE(14);
        CASE(13);
        CASE(12);
        CASE(11);
        CASE(10);
        CASE(9);
        CASE(8);
        CASE(7);
        CASE(6);
        CASE(5);
        CASE(4);
        CASE(3);
        CASE(2);
        CASE(1);
#undef CASE
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }
    if (testStatus > 0) {
        cerr << "Error: non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
