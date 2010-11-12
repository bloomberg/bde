// bdem_list.t.cpp                                                    -*-C++-*-

#include <bdem_list.h>

#include <bdem_choicearrayimp.h>          // for dummy bdem_ChoiceArray
#include <bdem_choicearrayitem.h>         // used by choicearray
#include <bdem_choiceimp.h>               // for dummy bdem_Choice
#include <bdem_elemattrlookup.h>          // for dummy bdem_Table
#include <bdem_row.h>                     // used by list and table
#include <bdem_tableimp.h>                // for dummy bdem_Table

#include <bdema_multipoolallocator.h>     // for testing only

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_time.h>
#include <bdetu_unset.h>                  // for testing only
#include <bdeu_printmethods.h>            // for testing only
#include <bdex_byteinstream.h>            // for testing only
#include <bdex_byteoutstream.h>           // for testing only
#include <bdex_testinstream.h>            // for testing only
#include <bdex_testoutstream.h>           // for testing only

#include <bslma_default.h>                // for testing only
#include <bslma_defaultallocatorguard.h>  // for testing only
#include <bslma_newdeleteallocator.h>     // for testing only
#include <bslma_testallocator.h>          // for testing only
#include <bslmf_isconvertible.h>          // for testing only
#include <bsls_objectbuffer.h>            // for testing only
#include <bsls_platform.h>                // for testing only
#include <bsls_types.h>                   // for testing only

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_new.h>                      // placement syntax
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                          *** Overview ***
//
// With its nearly 200 methods, the 'bdem_List' class serves as the
// presentation view for 'bdem' list functionality, and "knows about" all
// 'bdem' element types.  As part of its contract, 'bdem_List' contains exactly
// one data member of type 'bdem_ListImp' -- hence it is possible to cast
// safely from one list type (address) to the other.  Although 'bdem_List' is
// a fully value-semantic class, all of the substantive implementation reides
// in 'bdem_ListImp', which in turn resides in a separate (lower-level)
// component.
//
// Our goal here is primarily to ensure correct function signatures and that
// function arguments and return values are being propagated properly.
// Important issues related to the various allocation strategies, aliasing,
// thread safety, and exception neutrality are presumed to have already been
// tested thoroughly via the lower-level interface.
//
// Finally note that both 'bdem_List' and 'bdem_Table' are implemented as
// peers.  In this test driver, we will provide a watered-down "dummy" version
// of 'bdem_Table' (implemented in terms of 'bdem_TableImp'); i.e.,
// 'bdem_list.t.cpp' will NOT depend on 'bdem_table', leaving open the
// possibility that 'bdem_table.t.cpp' (and therefore -- by fiat --  also
// 'bdem_table.cpp') to depend on 'bdem_list.h' just as 'bdem_list.t.cpp (and
// therefore -- by fiat -- also 'bdem_list.cpp') currently depends on
// 'bdem_Row'.
//
//       Primary Constructors, Primary Manipulators, and Basic Accessors
//       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Primary Constructors:
//   A 'bdem_List' is created with an allocation mode and an allocator,
//   neither of which can be changed throughout the lifetime of an instance.
//   The constructor taking both an allocation mode and an allocator is
//   therefore sufficient for creating instances that can attain any
//   achievable (white-box) state.  Note, however, that the internal workings
//   of allocation are presumed to have already been tested in the underlying
//   'bdem_listimp' component.  The default constructor, therefore will be
//   sufficient for our purposes here.
//
//    o bdem_List(bslma_Allocator *basicAllocator = 0);
//
// Primary Manipulators:
//   We would normally need to bring this object to any achievable white-box
//   state, but since the details of aliasing, exception neutrality, and
//   such have already been tested thoroughly (in 'bdem_listimp'), it will
//   be sufficient (but not necessary) to achieve all black-box states.
//   Hence, appending an element at a time gets the job done (note that
//   appending a STRING given a (const char *) and appending a LIST given a
//   (const bdem_Row&) are only conveniences and are not necessary):
//
//    o void appendBool(bool value);
//    o void appendChar(char value);
//    o void appendShort(short value);
//    o void appendInt(int value);
//    o void appendInt64(Int64 value);
//    o void appendFloat(float value);
//    o void appendDouble(double value);
//    o void appendString(const bsl::string& value);
//    o void appendDatetime(const bdet_Datetime& value);
//    o void appendDatetime(const bdet_Datetime& value);
//    o void appendDate(const bdet_Date& value);
//    o void appendDate(const bdet_Date& value);
//    o void appendTime(const bdet_Time& value);
//    o void appendTime(const bdet_Time& value);
//    o void appendBoolArray(const bsl::vector<bool>& value);
//    o void appendCharArray(const bsl::vector<char>& value);
//    o void appendShortArray(const bsl::vector<short>& value);
//    o void appendIntArray(const bsl::vector<int>& value);
//    o void appendInt64Array(const bsl::vector<Int64>& value);
//    o void appendFloatArray(const bsl::vector<float>& value);
//    o void appendDoubleArray(const bsl::vector<double>& value);
//    o void appendStringArray(const bsl::vector<bsl::string>& value);
//    o void appendDatetimeArray(const bsl::vector<bdet_Datetime>& value);
//    o void appendDatetimeArray(const bsl::vector<bdet_Datetime>& value);
//    o void appendDateArray(const bsl::vector<bdet_Date>& value);
//    o void appendDateArray(const bsl::vector<bdet_Date>& value);
//    o void appendTimeArray(const bsl::vector<bdet_Time>& value);
//    o void appendTimeArray(const bsl::vector<bdet_Time>& value);
//    o void appendChoice(const bdem_Choice& value);
//    o void appendChoiceArray(const bdem_ChoiceArray& value);
//    o void appendList(const bdem_List& value);
//    o void appendList(const bdem_List& value);
//    o void appendTable(const bdem_Table& value);
//
// Basic Accessors:
//   We would like to find the largest set of *direct* accessors that can be
//   used generally to report back on the state of the object.  Methods
//   returning the number of elements ('length') and a reference to each
//   ('operator[]') are obvious first choices.  In addition, having a method
//   that explicitly returns the type and then a corresponding method for
//   each type that returns the data seem like good candidates as well.
//
//    o int length() const;
//    o bdem_ElemType::Type elemType(int index) const;
//    o void elemTypes(bsl::vector<bdem_ElemType::Type> *result) const;
//    o bdem_ConstElemRef operator[](int index) const;
//    o const bool& theBool(int index) const;
//    o const char& theChar(int index) const;
//    o const short& theShort(int index) const;
//    o const int& theInt(int index) const;
//    o const Int64& theInt64(int index) const;
//    o const float& theFloat(int index) const;
//    o const double& theDouble(int index) const;
//    o const bsl::string& theString(int index) const;
//    o const bdet_Datetime& theDatetime(int index) const;
//    o const bdet_Datetime& theDatetime(int index) const;
//    o const bdet_Date& theDate(int index) const;
//    o const bdet_DateTz& theDateTz(int index) const;
//    o const bdet_Time& theTime(int index) const;
//    o const bdet_Time& theTime(int index) const;
//    o const bsl::vector<bool>& theBoolArray(int index) const;
//    o const bsl::vector<char>& theCharArray(int index) const;
//    o const bsl::vector<short>& theShortArray(int index) const;
//    o const bsl::vector<int>& theIntArray(int index) const;
//    o const bsl::vector<Int64>& theInt64Array(int index) const;
//    o const bsl::vector<float>& theFloatArray(int index) const;
//    o const bsl::vector<double>& theDoubleArray(int index) const;
//    o const bsl::vector<bsl::string>& theStringArray(int index) const;
//    o const bsl::vector<bdet_Datetime>& theDatetimeArray(int i) const;
//    o const bsl::vector<bdet_Datetime>& theDatetimeArray(int i) const;
//    o const bsl::vector<bdet_Date>& theDateArray(int index) const;
//    o const bsl::vector<bdet_DateTz>& theDateTzArray(int index) const;
//    o const bsl::vector<bdet_Time>& theTimeArray(int index) const;
//    o const bsl::vector<bdet_Time>& theTimeArray(int index) const;
//    o const bdem_Choice& theChoice(int index) const;
//    o const bdem_ChoiceArray& theChoiceArray(int index) const;
//    o const bdem_List& theList(int index) const;
//    o const bdem_List& theList(int index) const;
//    o const bdem_Table& theTable(int index) const;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// NESTED TYPES
// class InitialMemory; // needs no testing, simple wrapper on int for ctor
//
// CLASS METHODS
// [  ] int maxSupportedBdexVersion();
//
// CREATORS
// [ 3] bdem_List(bslma_Allocator *basicAllocator = 0);
// [11] bdem_List(bdem_AggregateOption::AllocationStrategy am, ba);
// [11] bdem_List(am, const InitialMemory& im, ba);
// [11] bdem_List(const bdem_ElemType::Type elemType[], int ne, ba);
// [11] bdem_List(const bdem_ElemType::Type elemType[], int ne, am, ba);
// [21] bdem_List(const bdem_ElemType::Type elemType[], int ne, am, im, ba);
// [11] bdem_List(const bsl::vector<bdem_ElemType::Type>& et, ba);
// [11] bdem_List(const bsl::vector<bdem_ElemType::Type>& et, am, ba);
// [21] bdem_List(const bsl::vector<bdem_ElemType::Type>& et, am, im, ba);
// [ 7] bdem_List(const bdem_Row& original, ba);
// [ 7] bdem_List(const bdem_List& original, ba);
// [11] bdem_List(const bdem_Row& original, am, ba);
// [21] bdem_List(const bdem_Row& original, am, im, ba);
// [11] bdem_List(const bdem_List& original, am, ba);
// [21] bdem_List(const bdem_List& original, am, im, ba);
// [11] ~bdem_List();
//
// MANIPULATORS
// [ 9] bdem_List& operator=(const bdem_List& rhs);
// [ 9] bdem_List& operator=(const bdem_Row& rhs);
// [ 6] bdem_ElemRef operator[](int index);
// [12] bdem_Row& row();
// [ 4] bool& theModifiableBool(int index);
// [ 4] char& theModifiableChar(int index);
// [ 4] short& theModifiableShort(int index);
// [ 4] int& theModifiableInt(int index);
// [ 4] Int64& theModifiableInt64(int index);
// [ 4] float& theModifiableFloat(int index);
// [ 4] double& theModifiableDouble(int index);
// [ 4] bsl::string& theModifiableString(int index);
// [ 4] bdet_Datetime& theModifiableDatetime(int index);
// [ 4] bdet_DatetimeTz& theModifiableDatetimeTz(int index);
// [ 4] bdet_Date& theModifiableDate(int index);
// [ 4] bdet_DateTz& theModifiableDateTz(int index);
// [ 4] bdet_Time& theModifiableTime(int index);
// [ 4] bdet_TimeTz& theModifiableTimeTz(int index);
// [ 4] bsl::vector<bool>& theModifiableBoolArray(int index);
// [ 4] bsl::vector<char>& theModifiableCharArray(int index);
// [ 4] bsl::vector<short>& theModifiableShortArray(int index);
// [ 4] bsl::vector<int>& theModifiableIntArray(int index);
// [ 4] bsl::vector<Int64>& theModifiableInt64Array(int index);
// [ 4] bsl::vector<float>& theModifiableFloatArray(int index);
// [ 4] bsl::vector<double>& theModifiableDoubleArray(int index);
// [ 4] bsl::vector<bsl::string>& theModifiableStringArray(int index);
// [ 4] bsl::vector<bdet_Datetime>& theModifiableDatetimeArray(int index);
// [ 4] bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray(int index);
// [ 4] bsl::vector<bdet_Date>& theModifiableDateArray(int index);
// [ 4] bsl::vector<bdet_DateTz>& theModifiableDateTzArray(int index);
// [ 4] bsl::vector<bdet_Time>& theModifiableTimeArray(int index);
// [ 4] bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray(int index);
// [ 4] bdem_Choice& theModifiableChoice(int index);
// [ 4] bdem_ChoiceArray& theModifiableChoiceArray(int index);
// [ 4] bdem_List& theModifiableList(int index);
// [ 4] bdem_Table& theModifiableTable(int index);
// [13] void appendNullBool()();
// [ 4] void appendBool(bool value);
// [13] void appendNullChar()();
// [ 4] void appendChar(char value);
// [13] void appendNullShort()();
// [ 4] void appendShort(short value);
// [13] void appendNullInt()();
// [ 4] void appendInt(int value);
// [13] void appendNullInt64()();
// [ 4] void appendInt64(Int64 value);
// [13] void appendNullFloat();
// [ 4] void appendFloat(float value);
// [13] void appendNullDouble();
// [ 4] void appendDouble(double value);
// [13] void appendNullString();
// [15] void appendString(const char *value);
// [ 4] void appendString(const bsl::string& value);
// [13] void appendNullDatetime();
// [ 4] void appendDatetime(const bdet_Datetime& value);
// [13] void appendNullDatetimeTz();
// [ 4] void appendDatetimeTz(const bdet_DatetimeTz& value);
// [13] void appendNullDate();
// [ 4] void appendDate(const bdet_Date& value);
// [13] void appendNullDateTz();
// [ 4] void appendDateTz(const bdet_DateTz& value);
// [13] void appendNullTime();
// [ 4] void appendTime(const bdet_Time& value);
// [13] void appendNullTimeTz();
// [ 4] void appendTimeTz(const bdet_TimeTz& value);
// [13] void appendNullBoolArray();
// [ 4] void appendBoolArray(const bsl::vector<bool>& value);
// [13] void appendNullCharArray();
// [ 4] void appendCharArray(const bsl::vector<char>& value);
// [13] void appendNullShortArray();
// [ 4] void appendShortArray(const bsl::vector<short>& value);
// [13] void appendNullIntArray();
// [ 4] void appendIntArray(const bsl::vector<int>& value);
// [13] void appendNullInt64Array();
// [ 4] void appendInt64Array(const bsl::vector<Int64>& value);
// [13] void appendNullFloatArray();
// [ 4] void appendFloatArray(const bsl::vector<float>& value);
// [13] void appendNullDoubleArray();
// [ 4] void appendDoubleArray(const bsl::vector<double>& value);
// [13] void appendNullStringArray();
// [ 4] void appendStringArray(const bsl::vector<bsl::string>& value);
// [13] void appendNullDatetimeArray();
// [ 4] void appendDatetimeArray(const bsl::vector<bdet_Datetime>& value);
// [ 4] void appendDatetimeTzArray(const bsl::vector<bdet_DatetimeTz>& value);
// [13] void appendNullDateArray();
// [ 4] void appendDateArray(const bsl::vector<bdet_Date>& value);
// [13] void appendNullDateTzArray();
// [ 4] void appendDateTzArray(const bsl::vector<bdet_DateTz>& value);
// [13] void appendNullTimeArray();
// [ 4] void appendTimeArray(const bsl::vector<bdet_Time>& value);
// [13] void appendNullTimeTzArray();
// [ 4] void appendTimeTzArray(const bsl::vector<bdet_TimeTz>& value);
// [13] void appendNullChoice();
// [ 4] void appendChoice(const bdem_Choice& value);
// [13] void appendNullChoiceArray();
// [ 4] void appendChoiceArray(const bdem_ChoiceArray& value);
// [13] void appendNullList();
// [ 4] void appendList(const bdem_List& value);
// [15] void appendList(const bdem_Row& value);
// [13] void appendNullTable();
// [ 4] void appendTable(const bdem_Table& value);
// [13] void appendNullElement(bdem_ElemType::Type elementType);
// [13] void appendNullElements(const bdem_ElemType::Type et[], int ne);
// [13] void appendNullElements(const bsl::vector<bdem_ElemType::Type>&);
// [ 6] void appendElement(const bdem_ConstElemRef& srcElement);
// [16] void appendElements(const bdem_Row& srcElements);
// [16] void appendElements(const bdem_List& srcElements);
// [16] void appendElements(const bdem_Row&  se, int si, int ne);
// [16] void appendElements(const bdem_List& se, int si, int ne);
// [14] void insertNullBool(int dstIndex);
// [15] void insertBool(int dstIndex, bool value);
// [14] void insertNullChar(int dstIndex);
// [15] void insertChar(int dstIndex, char value);
// [14] void insertNullShort(int dstIndex);
// [15] void insertShort(int dstIndex, short value);
// [14] void insertNullInt(int dstIndex);
// [15] void insertInt(int dstIndex, int value);
// [14] void insertNullInt64(int dstIndex);
// [15] void insertInt64(int dstIndex, Int64 value);
// [14] void insertNullFloat(int dstIndex);
// [15] void insertFloat(int dstIndex, float value);
// [14] void insertNullDouble(int dstIndex);
// [15] void insertDouble(int dstIndex, double value);
// [14] void insertNullString(int dstIndex);
// [15] void insertString(int dstIndex, const char *value);
// [15] void insertString(int dstIndex, const bsl::string& value);
// [14] void insertNullDatetime(int dstIndex);
// [15] void insertDatetime(int dstIndex, const bdet_Datetime& value);
// [14] void insertNullDatetimeTz(int dstIndex);
// [15] void insertDatetimeTz(int dstIndex, const bdet_DatetimeTz& value);
// [14] void insertNullDate(int dstIndex);
// [15] void insertDate(int dstIndex, const bdet_Date& value);
// [14] void insertNullDateTz(int dstIndex);
// [15] void insertDateTz(int dstIndex, const bdet_DateTz& value);
// [14] void insertNullTime(int dstIndex);
// [15] void insertTime(int dstIndex, const bdet_Time& value);
// [14] void insertNullTimeTz(int dstIndex);
// [15] void insertTimeTz(int dstIndex, const bdet_TimeTz& value);
// [14] void insertNullBoolArray(int dstIndex);
// [15] void insertBoolArray(int dstIndex, const bsl::vector<bool>& val);
// [14] void insertNullCharArray(int dstIndex);
// [15] void insertCharArray(int dstIndex, const bsl::vector<char>& val);
// [14] void insertNullShortArray(int dstIndex);
// [15] void insertShortArray(int dstIndex, const bsl::vector<short>& v);
// [14] void insertNullIntArray(int dstIndex);
// [15] void insertIntArray(int dstIndex, const bsl::vector<int>& value);
// [14] void insertNullInt64Array(int dstIndex);
// [15] void insertInt64Array(int dstIndex, const bsl::vector<Int64>& v);
// [14] void insertNullFloatArray(int dstIndex);
// [15] void insertFloatArray(int dstIndex);
// [14] void insertNullDoubleArray(int dstIndex);
// [15] void insertDoubleArray(int dstIndex, const bsl::vector<double>&v);
// [14] void insertNullStringArray(int dstIndex);
// [15] void insertStringArray(int di, const bsl::vector<bsl::string>& v);
// [14] void insertNullDatetimeArray(int dstIndex);
// [15] void insertDatetimeArray(int dstIndex);
// [14] void insertNullDatetimeTzArray(int dstIndex);
// [15] void insertDatetimeTzArray(int dstIndex);
// [14] void insertNullDateArray(int dstIndex);
// [15] void insertDateArray(int di, const bsl::vector<bdet_Date>& value);
// [14] void insertNullDateTzArray(int dstIndex);
// [15] void insertDateTzArray(int di, const bsl::vector<bdet_DateTz>& value);
// [14] void insertNullTimeArray(int dstIndex);
// [15] void insertTimeArray(int di, const bsl::vector<bdet_Time>& value);
// [14] void insertNullTimeTzArray(int dstIndex);
// [15] void insertTimeTzArray(int di, const bsl::vector<bdet_TimeTz>& value);
// [14] void insertNullChoice(int dstIndex);
// [15] void insertChoice(int dstIndex, const bdem_Choice& value);
// [14] void insertNullChoiceArray(int dstIndex);
// [15] void insertChoiceArray(int dstIndex, const bdem_ChoiceArray& value);
// [14] void insertNullList(int dstIndex);
// [15] void insertList(int dstIndex, const bdem_List& value);
// [15] void insertList(int dstIndex, const bdem_Row& value);
// [14] void insertNullTable(int dstIndex);
// [15] void insertTable(int dstIndex, const bdem_Table& value);
// [14] void insertNullElement(int dstIndex, bdem_ElemType::Type et);
// [14] void insertNullElements(int, const bdem_ElemType::Type [], int);
// [14] void insertNullElements(int, const bsl::vector<bdem_ElemType::Type>&);
// [16] void insertElement(int di, const bdem_ConstElemRef& srcElement);
// [16] void insertElements(int dstIndex, const bdem_Row& srcElements);
// [16] void insertElements(int dstIndex, const bdem_List& srcElements);
// [16] void insertElements(int di, const bdem_Row&  sr, int si, int ne);
// [16] void insertElements(int di, const bdem_List& sl, int si, int ne);
// [18] void makeAllNull();
// [22] void reserveMemory(int numBytes);
// [16] void removeElement(int index);
// [16] void removeElements(int startIndex, int numElements);
// [16] void removeAll();
// [17] void replaceElement(int dstIndex, const bdem_ConstElemRef& se);
// [17] void resetElement(int dstIndex, bdem_ElemType::Type elemType);
// [17] void reset(const bdem_ElemType::Type elementTypes[], int ne);
// [17] void reset(const bsl::vector<bdem_ElemType::Type>& et);
// [20] void compact();
// [19] void swapElements(int index1, int index2);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] bdem_ConstElemRef operator[](int index) const;
// [ 4] bdem_ElemType::Type elemType(int index) const;
// [ 4] void elemTypes(bsl::vector<bdem_ElemType::Type> *result) const;
// [ 4] int length() const;
// [18] bool isAnyNull() const;
// [18] bool isAnyNonNull() const;
// [ 7] const bdem_Row& row() const;
// [ 4] const bool& theBool(int index) const;
// [ 4] const char& theChar(int index) const;
// [ 4] const short& theShort(int index) const;
// [ 4] const int& theInt(int index) const;
// [ 4] const Int64& theInt64(int index) const;
// [ 4] const float& theFloat(int index) const;
// [ 4] const double& theDouble(int index) const;
// [ 4] const bsl::string& theString(int index) const;
// [ 4] const bdet_Datetime& theDatetime(int index) const;
// [ 4] const bdet_DatetimeTz& theDatetimeTz(int index) const;
// [ 4] const bdet_Date& theDate(int index) const;
// [ 4] const bdet_DateTz& theDateTz(int index) const;
// [ 4] const bdet_Time& theTime(int index) const;
// [ 4] const bdet_TimeTz& theTimeTz(int index) const;
// [ 4] const bsl::vector<bool>& theBoolArray(int index) const;
// [ 4] const bsl::vector<char>& theCharArray(int index) const;
// [ 4] const bsl::vector<short>& theShortArray(int index) const;
// [ 4] const bsl::vector<int>& theIntArray(int index) const;
// [ 4] const bsl::vector<Int64>& theInt64Array(int index) const;
// [ 4] const bsl::vector<float>& theFloatArray(int index) const;
// [ 4] const bsl::vector<double>& theDoubleArray(int index) const;
// [ 4] const bsl::vector<bsl::string>& theStringArray(int index) const;
// [ 4] const bsl::vector<bdet_Datetime>& theDatetimeArray(int i) const;
// [ 4] const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(int i) const;
// [ 4] const bsl::vector<bdet_Date>& theDateArray(int index) const;
// [ 4] const bsl::vector<bdet_DateTz>& theDateTzArray(int index) const;
// [ 4] const bsl::vector<bdet_Time>& theTimeArray(int index) const;
// [ 4] const bsl::vector<bdet_TimeTz>& theTimeTzArray(int index) const;
// [ 4] const bdem_Choice& theChoice(int index) const;
// [ 4] const bdem_ChoiceArray& theChoiceArray(int index) const;
// [ 4] const bdem_List& theList(int index) const;
// [ 4] const bdem_Table& theTable(int index) const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] bsl::ostream& print(bsl::ostream& s, int l, int spl) const;
//
// // FREE OPERATORS
// [ 6] operator==(const bdem_List& lhs, const bdem_List& rhs);
// [ 6] operator!=(const bdem_List& lhs, const bdem_List& rhs);
// [10] operator>>(STREAM& stream, bdem_List& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] CONCERN: That basic operations on tables (seem to) work properly.
// [ 2] CONCERN: That operator== and operator!= work properly on tables.
// [23] CONCERN: BDEMA ALLOCATOR MODEL AND ALLOCATOR TRAITS
// [24] CONCERN: EXCEPTION NEUTRALITY
// [ 2] TEST APPARATUS
// [ 5] operator<<(ostream&, const bsl::vector<T>&);
// [ 4] void loadReferenceA(bdem_List *result);
// [ 6] void loadReferenceB(bdem_List *result);
// [13] void loadReferenceN(bdem_List *result);
// [ 8] bdem_List g(const char *spec, const bdem_List& referenceList)
// [25] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { P(I); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { P_(I) P(J) aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { P_(I) P_(J) P(K) aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { P_(I) P_(J) P_(K) P(L) aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { P_(I) P_(J) P_(K) P_(L) P(M) aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { P_(I) P_(J) P_(K) P_(L) P_(M) P(N) aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) { bsl::cout << #X " = "; \
               bdeu_PrintMethods::print(bsl::cout, X, -1, -1); \
               bsl::cout << bsl::endl; }
#define P_(X) { bsl::cout << #X " = "; \
                bdeu_PrintMethods::print(bsl::cout, X, -1, -1); \
                bsl::cout << ", " << bsl::flush; }

#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL STUB CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

                       // =======================
                       // DUMMY class bdem_Choice
                       // =======================

class bdem_Choice {
    // This class is a dummy bdem_Choice class to facilitate testing.
    // Note that it contains only the functions called in this test driver
    // and not the entire interface of bdem_ChoiceArray.

    bdem_ChoiceImp     d_choiceImp;

    friend bool operator==(const bdem_Choice& lhs, const bdem_Choice& rhs);
    friend bsl::ostream& operator<<(bsl::ostream& os, const bdem_Choice& rhs);

    public:
    // CREATORS
    bdem_Choice();
    bdem_Choice(const bdem_ElemType::Type types[], int numTypes);

    // MANIPULATORS
    bdem_ElemRef makeSelection(int index);

};

bool operator==(const bdem_Choice& lhs, const bdem_Choice& rhs);
bool operator!=(const bdem_Choice& lhs, const bdem_Choice& rhs);
bsl::ostream& operator<<(bsl::ostream& os, const bdem_Choice& rhs);

                        // ============================
                        // DUMMY class bdem_ChoiceArray
                        // ============================

class bdem_ChoiceArray {
    // This class is a dummy bdem_ChoiceArray class to facilitate testing.
    // Note that it contains only the functions called in this test driver
    // and not the entire interface of bdem_ChoiceArray.

    bdem_ChoiceArrayImp d_choiceArrayImp;

    friend bool operator==(const bdem_ChoiceArray& lhs,
                          const bdem_ChoiceArray& rhs);
    friend bsl::ostream& operator<<(bsl::ostream& os,
                                    const bdem_ChoiceArray& rhs);

  public:
    // CREATORS
    bdem_ChoiceArray();
    bdem_ChoiceArray(const bdem_ElemType::Type types[], int numTypes);

    // MANIPULATORS
    void appendNullItems(int elemCount);
    bdem_ChoiceArrayItem& operator[](int index);
};

bool operator==(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs);
bool operator!=(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs);
bsl::ostream& operator<<(bsl::ostream& os, const bdem_ChoiceArray& rhs);

                        // ======================
                        // DUMMY class bdem_Table
                        // ======================

class bdem_Table {
    // This class is a dummy bdem_Table class to facilitate testing

    bdem_TableImp d_tableImp;

    friend bool operator==(const bdem_Table& lhs, const bdem_Table& rhs);
    friend bsl::ostream& operator<<(bsl::ostream& os, const bdem_Table& rhs);

  public:
    // CREATORS
    bdem_Table();
    bdem_Table(const bdem_ElemType::Type types[], int numTypes);
    bdem_Table(const bdem_Table& original);

    // MANIPULATORS
    bdem_Table& operator=(const bdem_Table& rhs);
    void appendNullRow();
    bdem_Row& operator[](int index);
};

bool operator==(const bdem_Table& lhs, const bdem_Table& rhs);
bool operator!=(const bdem_Table& lhs, const bdem_Table& rhs);
bsl::ostream& operator<<(bsl::ostream& os, const bdem_Table& rhs);

                        // =======================
                        // DUMMY class bdem_Choice
                        // =======================

bdem_Choice::bdem_Choice()
{
}

bdem_Choice::bdem_Choice(const bdem_ElemType::Type types[], int numTypes)
: d_choiceImp(types,
              numTypes,
              bdem_ElemAttrLookup::lookupTable(),
              bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

bdem_ElemRef bdem_Choice::makeSelection(int index)
{
    return d_choiceImp.makeSelection(index);
}

bool operator==(const bdem_Choice& lhs, const bdem_Choice& rhs)
{
    return lhs.d_choiceImp == rhs.d_choiceImp;
}

bool operator!=(const bdem_Choice& lhs, const bdem_Choice& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const bdem_Choice& rhs)
{
    rhs.d_choiceImp.print(stream, 0, -1);
    return stream;
}

                        // ============================
                        // DUMMY class bdem_ChoiceArray
                        // ============================

bdem_ChoiceArray::bdem_ChoiceArray()
{
}

bdem_ChoiceArray::bdem_ChoiceArray(const bdem_ElemType::Type types[],
                                   int                       numTypes)
: d_choiceArrayImp(types,
                   numTypes,
                   bdem_ElemAttrLookup::lookupTable(),
                   bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

void bdem_ChoiceArray::appendNullItems(int elemCount)
{
    d_choiceArrayImp.insertNullItems(d_choiceArrayImp.length(), elemCount);
}

bdem_ChoiceArrayItem& bdem_ChoiceArray::operator[](int index)
{
    return (bdem_ChoiceArrayItem&) d_choiceArrayImp.theModifiableItem(index);
}

bool operator==(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs)
{
    return lhs.d_choiceArrayImp == rhs.d_choiceArrayImp;
}

bool operator!=(const bdem_ChoiceArray& lhs, const bdem_ChoiceArray& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ChoiceArray& rhs)
{
    rhs.d_choiceArrayImp.print(stream, 0, -1);
    return stream;
}

                        // ======================
                        // DUMMY class bdem_Table
                        // ======================

bdem_Table::bdem_Table()
: d_tableImp(bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

bdem_Table::bdem_Table(const bdem_ElemType::Type types[], int numTypes)
: d_tableImp(types,
             numTypes,
             bdem_ElemAttrLookup::lookupTable(),
             bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

bdem_Table::bdem_Table(const bdem_Table& original)
: d_tableImp(original.d_tableImp, bdem_AggregateOption::BDEM_PASS_THROUGH)
{
}

bdem_Table& bdem_Table::operator=(const bdem_Table& rhs)
{
    d_tableImp = rhs.d_tableImp;
    return *this;
}

void bdem_Table::appendNullRow()
{
    d_tableImp.insertNullRows(d_tableImp.numRows(), 1);
}

bdem_Row& bdem_Table::operator[](int index)
{
    return (bdem_Row&) d_tableImp.theModifiableRow(index);
}

bool operator==(const bdem_Table& lhs, const bdem_Table& rhs)
{
    return lhs.d_tableImp == rhs.d_tableImp;
}

bool operator!=(const bdem_Table& lhs, const bdem_Table& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream& operator<<(bsl::ostream& stream, const bdem_Table& rhs)
{
    rhs.d_tableImp.print(stream, 0, -1);
    return stream;
}

}  // close namespace BloombergLP

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_List     Obj;
typedef bdem_ElemType ET;

        // Create Three Distinct Exemplars For Each Element Type
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const bool               A22 = true;
const bool               B22 = false;
const bool               N22 = bdetu_Unset<bool>::unsetValue();

const char               A00 = 'A';
const char               B00 = 'B';
const char               N00 = bdetu_Unset<char>::unsetValue();

const short              A01 = -1;
const short              B01 = -2;
const short              N01 = bdetu_Unset<short>::unsetValue();

const int                A02 = 10;
const int                B02 = 20;
const int                N02 = bdetu_Unset<int>::unsetValue();

const bsls_Types::Int64  A03 = -100;
const bsls_Types::Int64  B03 = -200;
const bsls_Types::Int64  N03 = bdetu_Unset<bsls_Types::Int64>::unsetValue();

const float              A04 = -1.5;
const float              B04 = -2.5;
const float              N04 = bdetu_Unset<float>::unsetValue();

const double             A05 = 10.5;
const double             B05 = 20.5;
const double             N05 = bdetu_Unset<double>::unsetValue();

const bsl::string        A06 = "one";
const bsl::string        B06 = "two";
const bsl::string        N06 = bdetu_Unset<bsl::string>::unsetValue();

// Note: bdet_Datetime X07 implemented in terms of X08 and X09.

const bdet_Date          A08(2000,  1, 1);
const bdet_Date          B08(9999, 12,31);
const bdet_Date          N08 = bdetu_Unset<bdet_Date>::unsetValue();

const bdet_Time          A09(0, 1, 2, 3);
const bdet_Time          B09(4, 5, 6, 789);
const bdet_Time          N09 = bdetu_Unset<bdet_Time>::unsetValue();

const bdet_Datetime      A07(A08, A09);
const bdet_Datetime      B07(B08, B09);
const bdet_Datetime      N07 = bdetu_Unset<bdet_Datetime>::unsetValue();

const bdet_DateTz        A24(A08, -5);
const bdet_DateTz        B24(B08, -4);
const bdet_DateTz        N24 = bdetu_Unset<bdet_DateTz>::unsetValue();

const bdet_TimeTz        A25(A09, -5);
const bdet_TimeTz        B25(B09, -5);
const bdet_TimeTz        N25 = bdetu_Unset<bdet_TimeTz>::unsetValue();

const bdet_DatetimeTz    A23(A07, -5);
const bdet_DatetimeTz    B23(B07, -5);
const bdet_DatetimeTz    N23 = bdetu_Unset<bdet_DatetimeTz>::unsetValue();

static bsl::vector<bool>                        fA26() {
       bsl::vector<bool> t;          t.push_back(A22); return t; }
static bsl::vector<bool>                        fB26() {
       bsl::vector<bool> t;          t.push_back(B22); return t; }
const  bsl::vector<bool>                         A26 = fA26();
const  bsl::vector<bool>                         B26 = fB26();
const  bsl::vector<bool>                         N26;

static bsl::vector<char>                        fA10() {
       bsl::vector<char> t;          t.push_back(A00); return t; }
static bsl::vector<char>                        fB10() {
       bsl::vector<char> t;          t.push_back(B00); return t; }
const  bsl::vector<char>                         A10 = fA10();
const  bsl::vector<char>                         B10 = fB10();
const  bsl::vector<char>                         N10;

static bsl::vector<short>                       fA11() {
       bsl::vector<short> t;         t.push_back(A01); return t; }
static bsl::vector<short>                       fB11() {
       bsl::vector<short> t;         t.push_back(B01); return t; }
const  bsl::vector<short>                        A11 = fA11();
const  bsl::vector<short>                        B11 = fB11();
const  bsl::vector<short>                        N11;

static bsl::vector<int>                         fA12() {
       bsl::vector<int> t;           t.push_back(A02); return t; }
static bsl::vector<int>                         fB12() {
       bsl::vector<int> t;           t.push_back(B02); return t; }
const  bsl::vector<int>                          A12 = fA12();
const  bsl::vector<int>                          B12 = fB12();
const  bsl::vector<int>                          N12;

static bsl::vector<bsls_Types::Int64>    fA13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(A03); return t; }
static bsl::vector<bsls_Types::Int64>    fB13() {
       bsl::vector<bsls_Types::Int64> t;
                                     t.push_back(B03); return t; }
const  bsl::vector<bsls_Types::Int64>     A13 = fA13();
const  bsl::vector<bsls_Types::Int64>     B13 = fB13();
const  bsl::vector<bsls_Types::Int64>     N13;

static bsl::vector<float>                       fA14() {
       bsl::vector<float> t;         t.push_back(A04); return t; }
static bsl::vector<float>                       fB14() {
       bsl::vector<float> t;         t.push_back(B04); return t; }
const  bsl::vector<float>                        A14 = fA14();
const  bsl::vector<float>                        B14 = fB14();
const  bsl::vector<float>                        N14;

static bsl::vector<double>                      fA15() {
       bsl::vector<double> t;        t.push_back(A05); return t; }
static bsl::vector<double>                      fB15() {
       bsl::vector<double> t;        t.push_back(B05); return t; }
const  bsl::vector<double>                       A15 = fA15();
const  bsl::vector<double>                       B15 = fB15();
const  bsl::vector<double>                       N15;

static bsl::vector<bsl::string>                 fA16() {
       bsl::vector<bsl::string> t;   t.push_back(A06); return t; }
static bsl::vector<bsl::string>                 fB16() {
       bsl::vector<bsl::string> t;   t.push_back(B06); return t; }
const  bsl::vector<bsl::string>                  A16 = fA16();
const  bsl::vector<bsl::string>                  B16 = fB16();
const  bsl::vector<bsl::string>                  N16;

static bsl::vector<bdet_Datetime>               fA17() {
       bsl::vector<bdet_Datetime> t; t.push_back(A07); return t; }
static bsl::vector<bdet_Datetime>               fB17() {
       bsl::vector<bdet_Datetime> t; t.push_back(B07); return t; }
const  bsl::vector<bdet_Datetime>                A17 = fA17();
const  bsl::vector<bdet_Datetime>                B17 = fB17();
const  bsl::vector<bdet_Datetime>                N17;

static bsl::vector<bdet_Date>                   fA18() {
       bsl::vector<bdet_Date> t;     t.push_back(A08); return t; }
static bsl::vector<bdet_Date>                   fB18() {
       bsl::vector<bdet_Date> t;     t.push_back(B08); return t; }
const  bsl::vector<bdet_Date>                    A18 = fA18();
const  bsl::vector<bdet_Date>                    B18 = fB18();
const  bsl::vector<bdet_Date>                    N18;

static bsl::vector<bdet_Time>                   fA19() {
       bsl::vector<bdet_Time> t;     t.push_back(A09); return t; }
static bsl::vector<bdet_Time>                   fB19() {
       bsl::vector<bdet_Time> t;     t.push_back(B09); return t; }
const  bsl::vector<bdet_Time>                    A19 = fA19();
const  bsl::vector<bdet_Time>                    B19 = fB19();
const  bsl::vector<bdet_Time>                    N19;

static bsl::vector<bdet_DatetimeTz>             fA27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(A23); return t; }
static bsl::vector<bdet_DatetimeTz>             fB27() {
       bsl::vector<bdet_DatetimeTz> t; t.push_back(B23); return t; }
const  bsl::vector<bdet_DatetimeTz>              A27 = fA27();
const  bsl::vector<bdet_DatetimeTz>              B27 = fB27();
const  bsl::vector<bdet_DatetimeTz>              N27;

static bsl::vector<bdet_DateTz>                 fA28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(A24); return t; }
static bsl::vector<bdet_DateTz>                 fB28() {
       bsl::vector<bdet_DateTz> t;     t.push_back(B24); return t; }
const  bsl::vector<bdet_DateTz>                  A28 = fA28();
const  bsl::vector<bdet_DateTz>                  B28 = fB28();
const  bsl::vector<bdet_DateTz>                  N28;

static bsl::vector<bdet_TimeTz>                 fA29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(A25); return t; }
static bsl::vector<bdet_TimeTz>                 fB29() {
       bsl::vector<bdet_TimeTz> t;     t.push_back(B25); return t; }
const  bsl::vector<bdet_TimeTz>                  A29 = fA29();
const  bsl::vector<bdet_TimeTz>                  B29 = fB29();
const  bsl::vector<bdet_TimeTz>                  N29;

static bdem_List                                fA20() {
       bdem_List t;              t.appendInt(A02);
                                 t.appendDouble(A05);
                                 t.appendString(A06);
                                 t.appendStringArray(A16); return t; }
static bdem_List                            fB20() {
       bdem_List t;              t.appendInt(B02);
                                 t.appendDouble(B05);
                                 t.appendString(B06);
                                 t.appendStringArray(B16); return t; }

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr are
// 0x00000000 before entering main().  Consequently, the calls in fA20() and
// fB20() to t.appendInt() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.

#ifdef BSLS_PLATFORM__CMP_MSVC
bdem_List                                       mA20; // Initialized in main
bdem_List                                       mB20; // Initialized in main
const  bdem_List&                                A20 = mA20;
const  bdem_List&                                B20 = mB20;
#else
const  bdem_List                                 A20 = fA20();
const  bdem_List                                 B20 = fB20();
#endif
const  bdem_List                                 N20;

static bdem_Table                               fA21() {
    const bdem_ElemType::Type types[] =
    {
        bdem_ElemType::BDEM_INT,
        bdem_ElemType::BDEM_DOUBLE,
        bdem_ElemType::BDEM_STRING,
        bdem_ElemType::BDEM_STRING_ARRAY
    };
    const int NUM_TYPES = sizeof(types) / sizeof(*types);
    bdem_Table t(types, NUM_TYPES); t.appendNullRow();
                                    t[0][0].theModifiableInt() = A02;
                                    t[0][1].theModifiableDouble() = A05;
                                    t[0][2].theModifiableString() = A06;
                                    t[0][3].theModifiableStringArray() = A16;
                                    return t;
}

static bdem_Table                               fB21() {
    const bdem_ElemType::Type types[] =
    {
        bdem_ElemType::BDEM_INT,
        bdem_ElemType::BDEM_DOUBLE,
        bdem_ElemType::BDEM_STRING,
        bdem_ElemType::BDEM_STRING_ARRAY
    };
    const int NUM_TYPES = sizeof(types) / sizeof(*types);
    bdem_Table t(types, NUM_TYPES); t.appendNullRow();
                                    t[0][0].theModifiableInt() = B02;
                                    t[0][1].theModifiableDouble() = B05;
                                    t[0][2].theModifiableString() = B06;
                                    t[0][3].theModifiableStringArray() = B16;
                                    return t;
}

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.

#ifdef BSLS_PLATFORM__CMP_MSVC
bdem_Table                                      mA21;
bdem_Table                                      mB21;
const  bdem_Table&                               A21 = mA21;
const  bdem_Table&                               B21 = mB21;
#else
const  bdem_Table                                A21 = fA21();
const  bdem_Table                                B21 = fB21();
#endif
const  bdem_Table                                N21;

static bdem_Choice                              fA30() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Choice t(types, NUM_TYPES);
                              t.makeSelection(2).theModifiableString() = A06;
                              return t;
}

static bdem_Choice                              fB30() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_Choice t(types, NUM_TYPES);
                          t.makeSelection(3).theModifiableStringArray() = A16;
                          return t;
}

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.

#ifdef BSLS_PLATFORM__CMP_MSVC
bdem_Choice                                     mA30;
bdem_Choice                                     mB30;
const  bdem_Choice&                              A30 = mA30;
const  bdem_Choice&                              B30 = mB30;
#else
const  bdem_Choice                               A30 = fA30();
const  bdem_Choice                               B30 = fB30();
#endif
const  bdem_Choice                               N30;

static bdem_ChoiceArray                         fA31() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_ChoiceArray t(types, NUM_TYPES); t.appendNullItems(4);
                      t[0].makeSelection(0).theModifiableInt() = A02;
                      t[1].makeSelection(1).theModifiableDouble() = A05;
                      t[2].makeSelection(2).theModifiableString() = A06;
                      t[3].makeSelection(3).theModifiableStringArray() = A16;
                      return t;
}

static bdem_ChoiceArray                         fB31() {
       const bdem_ElemType::Type types[] =
       {
           bdem_ElemType::BDEM_INT,
           bdem_ElemType::BDEM_DOUBLE,
           bdem_ElemType::BDEM_STRING,
           bdem_ElemType::BDEM_STRING_ARRAY
       };
       const int NUM_TYPES = sizeof(types) / sizeof(*types);
       bdem_ChoiceArray t(types, NUM_TYPES); t.appendNullItems(4);
                      t[0].makeSelection(0).theModifiableInt() = B02;
                      t[1].makeSelection(1).theModifiableDouble() = B05;
                      t[2].makeSelection(2).theModifiableString() = B06;
                      t[3].makeSelection(3).theModifiableStringArray() = B16;
                      return t;
}

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.

#ifdef BSLS_PLATFORM__CMP_MSVC
bdem_ChoiceArray                                mA31;
bdem_ChoiceArray                                mB31;
const  bdem_ChoiceArray&                         A31 = mA31;
const  bdem_ChoiceArray&                         B31 = mB31;
#else
const  bdem_ChoiceArray                          A31 = fA31();
const  bdem_ChoiceArray                          B31 = fB31();
#endif
const  bdem_ChoiceArray                          N31;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

        // =======================================================
        // operator<< for bdem_AggregateOption::AllocationStrategy
        // =======================================================

namespace BloombergLP {

// declaration goes before implementation per component rules
inline
bsl::ostream& operator<<(bsl::ostream& stream,
                         const bdem_AggregateOption::AllocationStrategy mode);

inline
bsl::ostream& operator<<(bsl::ostream& stream,
                         const bdem_AggregateOption::AllocationStrategy mode)
{
    switch(mode) {
      case bdem_AggregateOption::BDEM_PASS_THROUGH: {
                      stream << "BDEM_PASS_THROUGH" << bsl::flush;
      } break;
      case bdem_AggregateOption::BDEM_WRITE_ONCE: {
                      stream << "BDEM_WRITE_ONCE" << bsl::flush;
      } break;
      case bdem_AggregateOption::BDEM_WRITE_MANY: {
                      stream << "BDEM_WRITE_MANY" << bsl::flush;
      } break;
      case bdem_AggregateOption::BDEM_SUBORDINATE: {
                      stream << "BDEM_SUBORDINATE" << bsl::flush;
      } break;
      default: {
          stream << "Unknown bdem_AggregateOption: "
                 << (int) mode << bsl::flush;
      }
    };
    return stream;
}

}  // close namespace BloombergLP

                        // ==============
                        // loadReferenceA
                        // ==============

void loadReferenceA(bdem_List *result)
    // Append to the specified 'result' a sequence of element types
    // corresponding to the 'A' category of element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendChar(A00);                         ASSERT(LEN +  1 == L.length());
    mL.appendShort(A01);                        ASSERT(LEN +  2 == L.length());
    mL.appendInt(A02);                          ASSERT(LEN +  3 == L.length());
    mL.appendInt64(A03);                        ASSERT(LEN +  4 == L.length());
    mL.appendFloat(A04);                        ASSERT(LEN +  5 == L.length());
    mL.appendDouble(A05);                       ASSERT(LEN +  6 == L.length());
    mL.appendString(A06);                       ASSERT(LEN +  7 == L.length());
    mL.appendDatetime(A07);                     ASSERT(LEN +  8 == L.length());
    mL.appendDate(A08);                         ASSERT(LEN +  9 == L.length());
    mL.appendTime(A09);                         ASSERT(LEN + 10 == L.length());

    mL.appendCharArray(A10);                    ASSERT(LEN + 11 == L.length());
    mL.appendShortArray(A11);                   ASSERT(LEN + 12 == L.length());
    mL.appendIntArray(A12);                     ASSERT(LEN + 13 == L.length());
    mL.appendInt64Array(A13);                   ASSERT(LEN + 14 == L.length());
    mL.appendFloatArray(A14);                   ASSERT(LEN + 15 == L.length());
    mL.appendDoubleArray(A15);                  ASSERT(LEN + 16 == L.length());
    mL.appendStringArray(A16);                  ASSERT(LEN + 17 == L.length());
    mL.appendDatetimeArray(A17);                ASSERT(LEN + 18 == L.length());
    mL.appendDateArray(A18);                    ASSERT(LEN + 19 == L.length());
    mL.appendTimeArray(A19);                    ASSERT(LEN + 20 == L.length());

    mL.appendList(A20);                         ASSERT(LEN + 21 == L.length());
    mL.appendTable(A21);                        ASSERT(LEN + 22 == L.length());

    mL.appendBool(A22);                         ASSERT(LEN + 23 == L.length());
    mL.appendDatetimeTz(A23);                   ASSERT(LEN + 24 == L.length());
    mL.appendDateTz(A24);                       ASSERT(LEN + 25 == L.length());
    mL.appendTimeTz(A25);                       ASSERT(LEN + 26 == L.length());
    mL.appendBoolArray(A26);                    ASSERT(LEN + 27 == L.length());
    mL.appendDatetimeTzArray(A27);              ASSERT(LEN + 28 == L.length());
    mL.appendDateTzArray(A28);                  ASSERT(LEN + 29 == L.length());
    mL.appendTimeTzArray(A29);                  ASSERT(LEN + 30 == L.length());

    mL.appendChoice(A30);                       ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(A31);                  ASSERT(LEN + 32 == L.length());
}

                        // ==============
                        // loadReferenceB
                        // ==============

void loadReferenceB(bdem_List *result)
    // Append to the specified 'result' a sequence of 30 element types
    // corresponding to the 'B' category of element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendChar(B00);                         ASSERT(LEN +  1 == L.length());
    mL.appendShort(B01);                        ASSERT(LEN +  2 == L.length());
    mL.appendInt(B02);                          ASSERT(LEN +  3 == L.length());
    mL.appendInt64(B03);                        ASSERT(LEN +  4 == L.length());
    mL.appendFloat(B04);                        ASSERT(LEN +  5 == L.length());
    mL.appendDouble(B05);                       ASSERT(LEN +  6 == L.length());
    mL.appendString(B06);                       ASSERT(LEN +  7 == L.length());
    mL.appendDatetime(B07);                     ASSERT(LEN +  8 == L.length());
    mL.appendDate(B08);                         ASSERT(LEN +  9 == L.length());
    mL.appendTime(B09);                         ASSERT(LEN + 10 == L.length());

    mL.appendCharArray(B10);                    ASSERT(LEN + 11 == L.length());
    mL.appendShortArray(B11);                   ASSERT(LEN + 12 == L.length());
    mL.appendIntArray(B12);                     ASSERT(LEN + 13 == L.length());
    mL.appendInt64Array(B13);                   ASSERT(LEN + 14 == L.length());
    mL.appendFloatArray(B14);                   ASSERT(LEN + 15 == L.length());
    mL.appendDoubleArray(B15);                  ASSERT(LEN + 16 == L.length());
    mL.appendStringArray(B16);                  ASSERT(LEN + 17 == L.length());
    mL.appendDatetimeArray(B17);                ASSERT(LEN + 18 == L.length());
    mL.appendDateArray(B18);                    ASSERT(LEN + 19 == L.length());
    mL.appendTimeArray(B19);                    ASSERT(LEN + 20 == L.length());

    mL.appendList(B20);                         ASSERT(LEN + 21 == L.length());
    mL.appendTable(B21);                        ASSERT(LEN + 22 == L.length());

    mL.appendBool(B22);                         ASSERT(LEN + 23 == L.length());
    mL.appendDatetimeTz(B23);                   ASSERT(LEN + 24 == L.length());
    mL.appendDateTz(B24);                       ASSERT(LEN + 25 == L.length());
    mL.appendTimeTz(B25);                       ASSERT(LEN + 26 == L.length());
    mL.appendBoolArray(B26);                    ASSERT(LEN + 27 == L.length());
    mL.appendDatetimeTzArray(B27);              ASSERT(LEN + 28 == L.length());
    mL.appendDateTzArray(B28);                  ASSERT(LEN + 29 == L.length());
    mL.appendTimeTzArray(B29);                  ASSERT(LEN + 30 == L.length());

    mL.appendChoice(B30);                       ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(B31);                  ASSERT(LEN + 32 == L.length());
}

                        // ==============
                        // loadReferenceN
                        // ==============

void loadReferenceN(bdem_List *result)
    // Append to the specified 'result' a sequence of 30 element types
    // corresponding to the 'N' category Element values defined above.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendChar(N00);                         ASSERT(LEN +  1 == L.length());
    mL.appendShort(N01);                        ASSERT(LEN +  2 == L.length());
    mL.appendInt(N02);                          ASSERT(LEN +  3 == L.length());
    mL.appendInt64(N03);                        ASSERT(LEN +  4 == L.length());
    mL.appendFloat(N04);                        ASSERT(LEN +  5 == L.length());
    mL.appendDouble(N05);                       ASSERT(LEN +  6 == L.length());
    mL.appendString(N06);                       ASSERT(LEN +  7 == L.length());
    mL.appendDatetime(N07);                     ASSERT(LEN +  8 == L.length());
    mL.appendDate(N08);                         ASSERT(LEN +  9 == L.length());
    mL.appendTime(N09);                         ASSERT(LEN + 10 == L.length());

    mL.appendCharArray(N10);                    ASSERT(LEN + 11 == L.length());
    mL.appendShortArray(N11);                   ASSERT(LEN + 12 == L.length());
    mL.appendIntArray(N12);                     ASSERT(LEN + 13 == L.length());
    mL.appendInt64Array(N13);                   ASSERT(LEN + 14 == L.length());
    mL.appendFloatArray(N14);                   ASSERT(LEN + 15 == L.length());
    mL.appendDoubleArray(N15);                  ASSERT(LEN + 16 == L.length());
    mL.appendStringArray(N16);                  ASSERT(LEN + 17 == L.length());
    mL.appendDatetimeArray(N17);                ASSERT(LEN + 18 == L.length());
    mL.appendDateArray(N18);                    ASSERT(LEN + 19 == L.length());
    mL.appendTimeArray(N19);                    ASSERT(LEN + 20 == L.length());

    mL.appendList(N20);                         ASSERT(LEN + 21 == L.length());
    mL.appendTable(N21);                        ASSERT(LEN + 22 == L.length());

    mL.appendBool(N22);                         ASSERT(LEN + 23 == L.length());
    mL.appendDatetimeTz(N23);                   ASSERT(LEN + 24 == L.length());
    mL.appendDateTz(N24);                       ASSERT(LEN + 25 == L.length());
    mL.appendTimeTz(N25);                       ASSERT(LEN + 26 == L.length());
    mL.appendBoolArray(N26);                    ASSERT(LEN + 27 == L.length());
    mL.appendDatetimeTzArray(N27);              ASSERT(LEN + 28 == L.length());
    mL.appendDateTzArray(N28);                  ASSERT(LEN + 29 == L.length());
    mL.appendTimeTzArray(N29);                  ASSERT(LEN + 30 == L.length());

    mL.appendChoice(N30);                       ASSERT(LEN + 31 == L.length());
    mL.appendChoiceArray(N31);                  ASSERT(LEN + 32 == L.length());
}

                        // =================
                        // loadReferenceNull
                        // =================

void loadReferenceNull(bdem_List *result)
    // Append to the specified 'result' a sequence of 30 element types having
    // the null value.
{
    ASSERT(result);
    bdem_List& mL = *result; const bdem_List& L = mL; const int LEN=L.length();

    mL.appendNullChar();                        ASSERT(LEN +  1 == L.length());
    mL.appendNullShort();                       ASSERT(LEN +  2 == L.length());
    mL.appendNullInt();                         ASSERT(LEN +  3 == L.length());
    mL.appendNullInt64();                       ASSERT(LEN +  4 == L.length());
    mL.appendNullFloat();                       ASSERT(LEN +  5 == L.length());
    mL.appendNullDouble();                      ASSERT(LEN +  6 == L.length());
    mL.appendNullString();                      ASSERT(LEN +  7 == L.length());
    mL.appendNullDatetime();                    ASSERT(LEN +  8 == L.length());
    mL.appendNullDate();                        ASSERT(LEN +  9 == L.length());
    mL.appendNullTime();                        ASSERT(LEN + 10 == L.length());

    mL.appendNullCharArray();                   ASSERT(LEN + 11 == L.length());
    mL.appendNullShortArray();                  ASSERT(LEN + 12 == L.length());
    mL.appendNullIntArray();                    ASSERT(LEN + 13 == L.length());
    mL.appendNullInt64Array();                  ASSERT(LEN + 14 == L.length());
    mL.appendNullFloatArray();                  ASSERT(LEN + 15 == L.length());
    mL.appendNullDoubleArray();                 ASSERT(LEN + 16 == L.length());
    mL.appendNullStringArray();                 ASSERT(LEN + 17 == L.length());
    mL.appendNullDatetimeArray();               ASSERT(LEN + 18 == L.length());
    mL.appendNullDateArray();                   ASSERT(LEN + 19 == L.length());
    mL.appendNullTimeArray();                   ASSERT(LEN + 20 == L.length());

    mL.appendNullList();                        ASSERT(LEN + 21 == L.length());
    mL.appendNullTable();                       ASSERT(LEN + 22 == L.length());

    mL.appendNullBool();                        ASSERT(LEN + 23 == L.length());
    mL.appendNullDatetimeTz();                  ASSERT(LEN + 24 == L.length());
    mL.appendNullDateTz();                      ASSERT(LEN + 25 == L.length());
    mL.appendNullTimeTz();                      ASSERT(LEN + 26 == L.length());
    mL.appendNullBoolArray();                   ASSERT(LEN + 27 == L.length());
    mL.appendNullDatetimeTzArray();             ASSERT(LEN + 28 == L.length());
    mL.appendNullDateTzArray();                 ASSERT(LEN + 29 == L.length());
    mL.appendNullTimeTzArray();                 ASSERT(LEN + 30 == L.length());

    mL.appendNullChoice();                      ASSERT(LEN + 31 == L.length());
    mL.appendNullChoiceArray();                 ASSERT(LEN + 32 == L.length());
}

void makeElemRefUnset(bdem_ElemRef elemRef)
{
    const bdem_Descriptor *desc = elemRef.descriptor();
    desc->makeUnset(elemRef.data());
}

template <typename TYPE>
bool isAnySet(const TYPE& object)
    // Return 'true' if any element in the specified 'object' is "set", that is
    // it does not have the "unset" value, and 'false' otherwise.
{
    const int len = object.length();
    for (int i = 0; i < len; ++i) {
        const bdem_ConstElemRef& elemRef = object[i];
        if (!elemRef.descriptor()->isUnset(elemRef.data())) {
            return true;
        }
    }
    return false;
}

                        // ====================
                        // generator function g
                        // ====================

static char SPECIFICATIONS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
                                                       // guaranteed
                                                       // contiguous
static const int SPEC_LEN    = strlen(SPECIFICATIONS);

ET::Type getElemType(char spec)
    // Return the element type corresponding to the specified
    // 'spec' value.  Valid input consists of uppercase letters where the
    // index of each letter is in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef".
{
    // Reminder: the 'bdem' element types are:
    //
    //    A   CHAR
    //    B   SHORT
    //    C   INT
    //    D   INT64
    //    E   FLOAT
    //    F   DOUBLE
    //    G   STRING
    //    H   DATETIME
    //    I   DATE
    //    J   TIME

    //    K   CHAR_ARRAY
    //    L   SHORT_ARRAY
    //    M   INT_ARRAY
    //    N   INT64_ARRAY
    //    O   FLOAT_ARRAY
    //    P   DOUBLE_ARRAY
    //    Q   STRING_ARRAY
    //    R   DATETIME_ARRAY
    //    S   DATE_ARRAY
    //    T   TIME_ARRAY

    //    U   LIST
    //    V   TABLE

    //    W   BOOL
    //    X   DATETIMETZ
    //    Y   DATETZ
    //    Z   TIMETZ
    //    a   BOOL_ARRAY
    //    b   DATETIMETZ_ARRAY
    //    c   DATETZ_ARRAY
    //    d   TIMETZ_ARRAY

    //    e   CHOICE
    //    f   CHOICE_ARRAY

    char *p = bsl::strchr(SPECIFICATIONS, spec);
    LOOP_ASSERT(spec, p);
    int index = p - SPECIFICATIONS;
    LOOP3_ASSERT(*p, index, SPEC_LEN, index < SPEC_LEN);
    return (ET::Type) index;
}

bdem_List g(const char *spec, const bdem_List& referenceList)
    // Return a list composed of the specified 'referenceList' elements
    // identified by the characters in the specified 'spec' string.  Valid
    // input consists of uppercase letters where the index of each letter in
    // "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef" is in the range
    // '[ 0 .. srcList.length() - 1 ]'.  Note that this function assumes that
    // the 'bdem_List' copy constructor and its 'appendElement' method have
    // been demonstrated to work properly.  Additionally, the specification can
    // optionally contain the characters '@' and '*' after each element
    // character.   Specifying the '@' character makes the preceding element
    // unset and the '*' character makes it null.
{
    const char UNSET_CHAR = '@';
    const char NULL_CHAR  = '*';

    ASSERT(spec);
    const int LEN = referenceList.length();

    bdem_List result;

    for (const char *s = spec; *s; ++s) {
        if (' ' == *s || '\t' == *s || '\n' == *s) continue; // ignore WS
        char *p = strchr(SPECIFICATIONS, *s);
        LOOP_ASSERT(*s, p);
        int index = p - SPECIFICATIONS;
        LOOP3_ASSERT(*p, index, LEN, index < LEN);
        result.appendElement(referenceList[index]);
        char next = *(s + 1);
        if (UNSET_CHAR == next) {
            makeElemRefUnset(result[result.length() - 1]);
            ++s;
        }
        else if (NULL_CHAR == next) {
            result[result.length() - 1].makeNull();
            ++s;
        }
    }

    return result;
}

                        // ================
                        // class BdexHelper
                        // ================

template <class CHAR_TYPE>
class BdexHelper {
    // This 'class' is used in testing of 'bdex' streaming whereby version 1
    // maps null values to "unset", and version 2 maps "unset" values to null.

    // DATA
    int d_version;

  public:
    BdexHelper(int version)
    : d_version(version)
    {
    }

    bool operator()(CHAR_TYPE& value) const
    {
       if (1 == d_version) {
           if ('*' == value) {
               value = '@';
           }
       }
       else if (2 == d_version) {
           if ('@' == value) {
               value = '*';
           }
       }
       return true;
    }
};

bool isUnset(const bdem_ConstElemRef& ref)
    // Return 'true' if the specified element 'ref' references an element that
    // has the "unset" value for its type, and 'false' otherwise.
{
    return ref.descriptor()->isUnset(ref.data());
}

//=============================================================================
//                  FUNCTIONS FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

    int unpackList(bdem_List *result, const char *buffer, int length)
        // Load into the specified 'result' the list data supplied in
        // specified 'buffer' of the specified 'length'.  Return 0 on success,
        // and a non-zero value (with no effect on *result) otherwise.
    {
        enum { FAILURE = -1, SUCCESS = 0 };
//..
// The first step for the client is to create, from the given externalized
// 'data and length, a corresponding 'bdex_ByteInStream', from which to
// re-hydrate a temporary local 'bdem_List':
//..
        bdex_ByteInStream in(buffer, length);
        bdem_List tmpList;

        int version;
        in.getVersion(version);
        bdex_InStreamFunctions::streamIn(in, tmpList, version);
//..
// If after the streaming process, the input stream is valid, the function
// succeeds; otherwise it fails:
//..
        if (!in) {
            return FAILURE;                                           // RETURN
        }

        ASSERT(in);
        *result = tmpList;  // 'result->swap(tmpList)' is faster when valid
        return SUCCESS;
    }

//=============================================================================
//                GLOBALS MOVED OUT OF MAIN WITH TEST CASES
//-----------------------------------------------------------------------------

        // **************************************************************
        // INSTALLING A TEST ALLOCATOR AS THE DEFAULT FOR ALL TEST CASES.
        // **************************************************************

    bslma_TestAllocator defaultAllocator;
    const bslma_DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

//=============================================================================
//                              TEST CASES
//-----------------------------------------------------------------------------

static void testCase25(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating how one might use a list.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Change all 'assert' to 'ASSERT' and ensure the code compiles.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// A 'bdem_List' is ideal for representing and transmitting messages as a
// heterogeneous sequence of arbitrarily complex, self-describing data in and
// out of a process.  For example, suppose we want to send a message containing
// price-update information for a given security to an interested client in the
// form of a 'bdem_List' object.  The first step is to decide what data types
// will be needed:
//..
//  STRING secid  // the character-string identifier for the security
//  DOUBLE bid    // the price someone is willing to pay for some quantity
//  DOUBLE ask    // the price someone is willing to sell at for some quantity
//  DOUBLE last   // the price at which some quantity last traded
//..
// Suppose the values we wish to send are respectively "BAC", 17.51, 17.54,
// and 17.52.
//
// The next step is to create a list of these types and populate it.  One
// way to do that is to create an empty list and append the elements in order:
//..
    bdem_List aList;
    aList.appendString("BAC");
    aList.appendDouble(17.51);
    aList.appendDouble(17.54);
    aList.appendDouble(17.52);
//..
// If, as is often the case, we are planning to populate the list just once,
// specifying 'bdem_AggregateOption::WRITE_ONCE' as a constructor argument
// optimizes internal memory management.
//
// We can also create the list by passing in an array of element types and then
// assigning the values, which is guaranteed to lead to an optimally-packed
// representation, ideal for repeated use:
//..
    static bdem_ElemType::Type MSG_TYPES[] = {
        bdem_ElemType::BDEM_STRING,
        bdem_ElemType::BDEM_DOUBLE,
        bdem_ElemType::BDEM_DOUBLE,
        bdem_ElemType::BDEM_DOUBLE,
        // ...
    };

    enum {
        SEC_ID,
        BID,
        ASK,
        LAST
        // ...
    };

    bdem_List anotherList(MSG_TYPES, 4, bdem_AggregateOption::BDEM_WRITE_MANY);
    anotherList[SEC_ID].theModifiableString() = "BAC";
    anotherList[BID]   .theModifiableDouble() = 17.51;
    anotherList[ASK]   .theModifiableDouble() = 17.54;
    anotherList[LAST]  .theModifiableDouble() = 17.52;
//..
// Notice the use of 'bdem_AggregateOption::BDEM_WRITE_MANY' as a trailing
// constructor argument, which establishes a local memory manager for
// optimizing repeated read/write access.
//
// The next step is to create a 'bdex_ByteOutStream' in which to stream
// (externalize) the list data:
//..
    bdex_ByteOutStream out;
    const int VERSION = 3;
    out.putVersion(VERSION);
    bdex_OutStreamFunctions::streamOut(out, aList, VERSION);
//..
// We can extract the platform-neutral representation of the list data from the
// 'out' stream as a 'const char *' and an integer length as follows:
//..
    const int length = out.length();
    const char *data = out.data();
//..
// At this point we can send this data anywhere we want, any way we want (e.g.,
// via a socket, writing to disc).
//
// Let's now assume that a client receives this data in some form and wants to
// unpack it:
//..
//  int unpackList(bdem_List *result, const char *buffer, int length)
//      // Load into the specified 'result' the list data supplied in the
//      // specified 'buffer' of the specified 'length'.  Return 0 on success,
//      // and a non-zero value (with no effect on *result) otherwise.
//  {
//      enum { FAILURE = -1; SUCCESS = 0 };
//..
// The first step for the client is to create, from the given externalized
// data and length, a corresponding 'bdex_ByteInStream', from which to
// re-hydrate a temporary local 'bdem_List':
//..
//      bdex_ByteInStream in(data, length);
//      bdem_List tmpList(result->allocator());
//
//      int version;
//      in.getVersion(version);
//      bdex_InStreamFunctions::streamIn(in, tmpList, version);
//..
// If after the streaming process, the input stream is valid, the function
// succeeds; otherwise it fails:
//..
//      if (!in) {
//          return FAILURE;                                           // RETURN
//      }
//
//      assert(in);
//      *result = tempList;  // 'result->swap(tmpList)' is faster when valid
//      return SUCCESS;
//  }
//..
// Note that the extra copy is necessary if we don't know that the two lists
// were constructed with the same allocator object and memory management hint.
//
// The client can now access the contents of the newly-hydrated list:
//..
    bdem_List clientList;
    int status = unpackList(&clientList, data, length);
    ASSERT(!status);

    ASSERT(clientList.length() >= 4);
    ASSERT("BAC" == clientList[0].theString());
    ASSERT(17.51 == clientList[1].theDouble());
    ASSERT(17.54 == clientList[2].theDouble());
    ASSERT(17.52 == clientList[3].theDouble());
//..
// Notice that the client is expecting a list with at least (as opposed to
// exactly) four fields.  It is predominantly this feature that makes messaging
// with 'bdem_List's more robust than with hard-coded structures.  For example,
// suppose the program sending the information is upgraded to transmit an
// additional field, say an integer quantity:
//..
    bdem_List newList;
    newList.appendString("BAC");
    newList.appendDouble(17.51);
    newList.appendDouble(17.54);
    newList.appendDouble(17.52);
    newList.appendInt(1000);

    bdex_ByteOutStream newOut;
    newOut.putVersion(VERSION);
    bdex_OutStreamFunctions::streamOut(newOut, newList, VERSION);

    const int newLength = newOut.length();
    const char *newData = newOut.data();
//..
// The original client program will continue to work just the same without
// rebuilding, but a new client will be able to access the additional field:
//..
    enum { SID = 0, QNT = 4 };
    bdex_ByteInStream newIn(newData, newLength);
    bdem_List newClientList;

    int newVersion;
    newIn.getVersion(newVersion);
    bdex_InStreamFunctions::streamIn(newIn, newClientList, newVersion);

    ASSERT(newClientList.length() >= 5);
    ASSERT("BAC" == newClientList[SID].theString());
    ASSERT(1000  == newClientList[QNT].theInt());
//..
// It is frequently useful in practice to have a facility for associating a
// name with a field in such a way that the field can be looked up by that
// name at runtime.  For information on how to establish named fields, see the
// 'bdem_schema' component.
//
// Sometimes a message will contain one or more null fields -- i.e., fields
// in the null state.  As an optimization, the underlying unset value of the
// null field is treated specially and is not streamed explicitly; hence, the
// size of the streamed data of a long sparsely-populated list can be
// significantly smaller than if the list were fully populated.  For example,
// in the following we create two empty lists, 'a' and 'b', then append 100
// elements of type 'double' to each.  However, unlike for 'a', most of the
// 'double' values for 'b' are left as null values.  The size of the resulting
// stream data for list 'b' will be considerably smaller than that of 'a':
//..
    {
        bdem_List a, b;
        const int N = 100;
        for (int i = 0; i < N; ++i) {
            a.appendDouble(N + 0.5);
            b.appendNullDouble();
        }
        b[25].theModifiableDouble() = 25.5;  // make just a couple of the
        b[75].theModifiableDouble() = 75.5;  // element values in 'b' non-null

        ASSERT(100 == a.length());           // both lists hold 100 elements
        ASSERT(100 == b.length());

        bdex_ByteOutStream aOut, bOut;       // create two separate streams
        bdex_OutStreamFunctions::streamOut(aOut, a, VERSION);
        bdex_OutStreamFunctions::streamOut(bOut, b, VERSION);

        const int aLength = aOut.length();   // extract each stream's length
        const int bLength = bOut.length();

if (verbose)
        bsl::cout << "aLength = " << aLength << ", bLength = " << bLength
                  << bsl::endl;

        ASSERT(bLength < aLength);  // 'bLength' is *much* less than 'aLength'
    }
//..
// The above code will print the following on standard output:
//..
//  aLength = 917, bLength = 133
//..
}

static void testCase24(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING EXCEPTION NEUTRALITY
        //
        // Concerns:
        //   That appending and inserting are exception neutral.
        //
        // Plan:
        //   Repeat some of the testing in case 16.  The reason we are not
        //   just adding these tests to case 16 is that there are too many
        //   test cases in 16; if we added exception-testing to them, it
        //   would take too long.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting exception neutrality\n"
                               "============================\n";

        if (verbose) cout << "\nCreate a general reference list." << endl;

        bdem_List mL; const bdem_List& L = mL;  ASSERT( 0 == L.length());
        loadReferenceA(&mL);                    ASSERT(32 == L.length());
        loadReferenceB(&mL);                    ASSERT(64 == L.length());
        loadReferenceN(&mL);                    ASSERT(96 == L.length());

        if (veryVerbose) { cout << "\tL = "; L.print(cout, -1, 4); }

        if (verbose) cout << "\nTesting NON-ALIAS case." << endl;
        {
            static const struct {
                int         d_lineNum; // source line number
                const char *d_dSpec;   // initial (destination) list value (dl)
                int         d_di;      // index at which to insert into dl
                const char *d_sSpec;   // source list value (sl)
                int         d_si;      // index at which to insert from sl
                int         d_ne;      // number of elements to insert
                const char *d_eSpec;   // expected final value of dl
            } DATA[] = {
                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 1
                { L_,   "A",     1,  "",      0,  0,  "A"     },

                { L_,   "",      0,  "A",     1,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 2
                { L_,   "AB",    2,  "",      0,  0,  "AB"    },

                { L_,   "A",     1,  "B",     1,  0,  "A"     },

                { L_,   "",      0,  "AB",    2,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 3
                { L_,   "ABC",   3,  "",      0,  0,  "ABC"   },

                { L_,   "AB",    2,  "C",     1,  0,  "AB"    },

                { L_,   "A",     1,  "BC",    2,  0,  "A"     },

                { L_,   "",      0,  "ABC",   2,  1,  "C"     },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  "",      0,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  "",      0,  0,  "ABCD"  },

                { L_,   "ABC",   0,  "D",     0,  0,  "ABC"   },
                { L_,   "ABC",   3,  "D",     1,  0,  "ABC"   },

                { L_,   "AB",    0,  "CD",    0,  0,  "AB"    },
                { L_,   "AB",    2,  "CD",    2,  0,  "AB"    },

                { L_,   "A",     0,  "BCD",   0,  0,  "A"     },
                { L_,   "A",     1,  "BCD",   3,  0,  "A"     },

                { L_,   "",      0,  "ABCD",  0,  0,  ""      },
                { L_,   "",      0,  "ABCD",  4,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  "",      0,  0,  "ABCDE" },

                { L_,   "ABCD",  0,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  "E",     1,  0,  "ABCD"  },

                { L_,   "ABC",   0,  "DE",    0,  0,  "ABC"   },
                { L_,   "ABC",   3,  "DE",    2,  0,  "ABC"   },

                { L_,   "AB",    0,  "CDE",   0,  0,  "AB"    },
                { L_,   "AB",    2,  "CDE",   3,  0,  "AB"    },

                { L_,   "A",     0,  "BCDE",  0,  0,  "A"     },
                { L_,   "A",     1,  "BCDE",  3,  0,  "A"     },

                { L_,   "",      0,  "ABCDE", 0,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 5,  0,  ""      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bdem_ElemType::Type OFFSETS[] = {
                bdem_ElemType::BDEM_INT,
                bdem_ElemType::BDEM_STRING,
                bdem_ElemType::BDEM_INT_ARRAY,
                bdem_ElemType::BDEM_STRING_ARRAY,
                bdem_ElemType::BDEM_LIST,

            };
            const int NUM_OFFSETS = sizeof OFFSETS / sizeof *OFFSETS;

            const bdem_AggregateOption::AllocationStrategy MODES[] = {
                bdem_AggregateOption::BDEM_PASS_THROUGH,
                bdem_AggregateOption::BDEM_WRITE_ONCE,
                bdem_AggregateOption::BDEM_WRITE_MANY,
            };
            const int NUM_MODES = sizeof MODES / sizeof *MODES;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_sSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_eSpec;

                const int   DEPTH  = strlen(D_SPEC) + strlen(S_SPEC);
                if (DEPTH != oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                for (int ri = 0; ri < NUM_OFFSETS; ++ri) { // START POSITION
                    bdem_List reference;
                    {
                        for (int i = 0; i < 5; ++i) {   // LOAD FIVE EXEMPLARS
                            reference.appendElement(L[OFFSETS[ri] + i]);
                        }
                    }
                    const bdem_List& REFERENCE = reference;

                    Obj DD(g(D_SPEC, REFERENCE));   // control for destination
                    Obj SS(g(S_SPEC, REFERENCE));   // control for source
                    Obj EE(g(E_SPEC, REFERENCE));   // control for expected val
                    Obj NN(g(S_SPEC, REFERENCE));   // null control for source
                    NN.makeAllNull();

                    if (veryVerbose && !ri || veryVeryVerbose) {
                        cout << "\t  =================================="
                                    "=================================="
                        << endl;

                        cout << "\t  "; P_(LINE); P_(D_SPEC); P_(DI);
                                         P_(S_SPEC); P_(SI); P_(NE); P(E_SPEC);
                        cout << "\t  "; P_(ri); P(OFFSETS[ri]);
                        cout << "\t\t"; P(DD);
                        cout << "\t\t"; P(SS);
                        cout << "\t\t"; P(EE);
                    }

                    for (int ai = 0; ai < NUM_MODES; ++ai) { // ALLOCATION MODE

                        const bool PRINT = veryVerbose && !ri && !ai
                                        || veryVeryVerbose;

                        const bdem_AggregateOption::AllocationStrategy
                                                              MODE = MODES[ai];
                        if (PRINT) {
                            cout << "\t  ----------------------------------"
                                        "----------------------------------"
                            << endl;
                            cout << "\t  "; P(MODE);
                        }
 // v-------------------^
    if (DI == DD.length() && NE == SS.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_Row& srcElements);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(DD, MODE, &ta);  const Obj &X = mX;
            {
                Obj mS(SS, &ta);  const Obj &S = mS;
                if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
                mX.appendElements(S.row());
                if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
            }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (DI == DD.length() && NE == SS.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_List& srcElements);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(DD, MODE, &ta);  const Obj &X = mX;
            {
                Obj mS(SS, MODE, &ta);  const Obj &S = mS;
                if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
                mX.appendElements(S);
                if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
            }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (DI == DD.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
           "\t\tappendElements(const bdem_Row& se, int si, int ne);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(DD, MODE, &ta);  const Obj &X = mX;
            {
                Obj mS(SS, MODE, &ta);  const Obj &S = mS;
                if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
                mX.appendElements(S.row(), SI, NE);
                if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
            }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (DI == DD.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_List& se, int si, int ne);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(DD, MODE, &ta);  const Obj &X = mX;
            {
                Obj mS(SS, MODE, &ta);  const Obj &S = mS;
                if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
                mX.appendElements(S, SI, NE);
                if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
            }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
           "\t\tinsertElement(int di, const bdem_ConstElemRef& srcElement);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(DD, MODE, &ta);  const Obj &X = mX;
            {
                Obj mS(SS, MODE, &ta);  const Obj &S = mS;
                if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
                mX.insertElement(DI, S.row()[SI]);
                if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
                mX.insertElement(DI, NN.row()[SI]);
                if (PRINT) { cout << "\t\t\t AFTER NULL: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, X[DI].isNull());
                mX.removeElement(DI);
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
            }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (NE == SS.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tinsertElements(int dstIndex, const bdem_Row& srcElements);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(DD, MODE, &ta);  const Obj &X = mX;
            {
                Obj mS(SS, MODE, &ta);  const Obj &S = mS;
                if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
                mX.insertElements(DI, S.row());
                if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
            }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (NE == SS.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tinsertElements(int dstIndex, const bdem_List& srcElements);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(DD, MODE, &ta);  const Obj &X = mX;
            {
                Obj mS(SS, MODE, &ta);  const Obj &S = mS;
                if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
                mX.insertElements(DI, S);
                if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
            }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tinsertElements(int di, const bdem_Row&  sr, int si, int ne);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(DD, MODE, &ta);  const Obj &X = mX;
            {
                Obj mS(SS, MODE, &ta);  const Obj &S = mS;
                if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
                mX.insertElements(DI, S.row(), SI, NE);
                if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
            }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tinsertElements(int di, const bdem_List& sl, int si, int ne);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(DD, MODE, &ta);  const Obj &X = mX;
            {
                Obj mS(SS, MODE, &ta);  const Obj &S = mS;
                if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
                mX.insertElements(DI, S, SI, NE);
                if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
                LOOP3_ASSERT(LINE, ri, ai, EE == X);
                LOOP3_ASSERT(LINE, ri, ai, SS == S);
            }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout << "\t\tremoveElement(int index);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(EE, MODE, &ta);  const Obj &X = mX;

            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.removeElement(DI);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, DD == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tremoveElements(int startIndex, int numElements);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(EE, MODE, &ta);  const Obj &X = mX;

            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.removeElements(DI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, DD == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }
 // ^-------------------v
                    } // end for each allocation mode
                } // end for each exemplar sequence
            } // end for each test vector
        } // end block scope

        //---------------------------------------------------------------------
        if (verbose) cout << "\nTesting ALIAS case." << endl;
        {
            static const struct {
                int         d_lineNum; // source line number
                const char *d_xSpec;   // initial list value (xl)
                int         d_di;      // index at which to insert into xl
                int         d_si;      // index at which to insert from xl
                int         d_ne;      // number of elements to insert
                const char *d_eSpec;   // expected final value of dl

            } DATA[] = {
                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 0
                { L_,   "",       0,  0,  0,  ""           },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 1
                { L_,   "A",      0,  0,  0,  "A"          },
                { L_,   "A",      0,  1,  0,  "A"          },

                { L_,   "A",      1,  0,  0,  "A"          },
                { L_,   "A",      1,  1,  0,  "A"          },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 2

                { L_,   "AB",     0,  2,  0,  "AB"         },

                { L_,   "AB",     1,  0,  0,  "AB"         },
                { L_,   "AB",     1,  0,  2,  "AABB"       },

                { L_,   "AB",     2,  0,  0,  "AB"         },
                { L_,   "AB",     2,  2,  0,  "AB"         },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 3
                { L_,   "ABC",    0,  0,  0,  "ABC"        },
                { L_,   "ABC",    0,  3,  0,  "ABC"        },

                { L_,   "ABC",    1,  0,  0,  "ABC"        },
                { L_,   "ABC",    1,  3,  0,  "ABC"        },

                { L_,   "ABC",    2,  0,  0,  "ABC"        },
                { L_,   "ABC",    2,  3,  0,  "ABC"        },

                { L_,   "ABC",    3,  0,  0,  "ABC"        },
                { L_,   "ABC",    3,  3,  0,  "ABC"        },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 4
                { L_,   "ABCD",   0,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   0,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   1,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   1,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   2,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   2,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   3,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   3,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   4,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   4,  4,  0,  "ABCD"       },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 5
                { L_,   "ABCDE",  0,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  1,  4,  "BCDEABCDE"  },
                { L_,   "ABCDE",  0,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  1,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  0,  5,  "AABCDEBCDE" },
                { L_,   "ABCDE",  1,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  2,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  3,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  4,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  5,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  5,  0,  "ABCDE"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bdem_ElemType::Type OFFSETS[] = {
                bdem_ElemType::BDEM_INT,
                bdem_ElemType::BDEM_STRING,
                bdem_ElemType::BDEM_INT_ARRAY,
                bdem_ElemType::BDEM_STRING_ARRAY,
                bdem_ElemType::BDEM_LIST,

            };
            const int NUM_OFFSETS = sizeof OFFSETS / sizeof *OFFSETS;

            const bdem_AggregateOption::AllocationStrategy MODES[] = {
                bdem_AggregateOption::BDEM_PASS_THROUGH,
                bdem_AggregateOption::BDEM_WRITE_ONCE,
                bdem_AggregateOption::BDEM_WRITE_MANY,
            };
            const int NUM_MODES = sizeof MODES / sizeof *MODES;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_eSpec;

                const int   DEPTH  = (int)strlen(X_SPEC);
                if (DEPTH != oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                for (int ri = 0; ri < NUM_OFFSETS; ++ri) { // START POSITION
                    bdem_List reference;
                    {
                        for (int i = 0; i < 5; ++i) {   // LOAD FIVE EXEMPLARS
                            reference.appendElement(L[OFFSETS[ri] + i]);
                        }
                    }
                    const bdem_List& REFERENCE = reference;

                    Obj XX(g(X_SPEC, REFERENCE));  // control for initial value
                    Obj EE(g(E_SPEC, REFERENCE));  // control for expected val

                    if (veryVerbose && !ri || veryVeryVerbose) {
                        cout << "\t  =================================="
                                    "=================================="
                        << endl;

                        cout << "\t  "; P_(X_SPEC); P_(DI);
                                                    P_(SI); P_(NE); P(E_SPEC);
                        cout << "\t  "; P_(ri); P(OFFSETS[ri]);
                        cout << "\t\t"; P(XX);
                        cout << "\t\t"; P(EE);
                    }

                    for (int ai = 0; ai < NUM_MODES; ++ai) { // ALLOCATION MODE

                        const bool PRINT = veryVerbose && !ri && !ai
                                        || veryVeryVerbose;

                        const bdem_AggregateOption::AllocationStrategy
                                                              MODE = MODES[ai];
                        if (PRINT) {
                            cout << "\t  ----------------------------------"
                                        "----------------------------------"
                            << endl;
                            cout << "\t  "; P(MODE);
                        }
 // v-------------------^
    if (DI == XX.length() && NE == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_Row& srcElements);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendElements(X.row());
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (DI == XX.length() && NE == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_List& srcElements);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendElements(X);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (DI == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
           "\t\tappendElements(const bdem_Row& se, int si, int ne);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendElements(X.row(), SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (DI == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_List& se, int si, int ne);" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendElements(X, SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
           "\t\tinsertElement(int di, const bdem_ConstElemRef& srcElement);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElement(DI, X.row()[SI]);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (NE == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tinsertElements(int dstIndex, const bdem_Row& srcElements);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElements(DI, X.row());
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (NE == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tinsertElements(int dstIndex, const bdem_List& srcElements);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElements(DI, X);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tinsertElements(int di, const bdem_Row&  sr, int si, int ne);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElements(DI, X.row(), SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);
        bslma_TestAllocator ta;
        bslma_TestAllocator& testAllocator = ta;

        if (PRINT) cout <<
            "\t\tinsertElements(int di, const bdem_List& sl, int si, int ne);"
                        << endl;
        BEGIN_BSLMA_EXCEPTION_TEST
            Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElements(DI, X, SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);

//             LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
        END_BSLMA_EXCEPTION_TEST
    }
 // ^-------------------v
                    } // end for each allocation mode
                } // end for each exemplar sequence
            } // end for each test vector
        } // end block scope
}

static void testCase23(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING BDEMA ALLOCATOR MODEL AND ALLOCATOR TRAITS
        //
        // Concerns: That the type under testing, which uses an allocator, when
        //   it is placed into a container, is correctly propagated the
        //   allocator of the container and will use the allocator (not the
        //   default allocator) for its future memory needs.
        //
        // Plan: Place the type under testing inside a standard container
        //   (e.g., 'bsl::vector') and construct an instance of the container
        //   using a test allocator.  Then perturb the object of the type under
        //   testing within the proxy to force it to allocate memory, and
        //   assert that (1) no memory for the default allocator is used, and
        //   (2) memory was allocated from the test allocator.
        //
        // Testing:
        //   bdema allocator model
        //   correct declaration of bslalg_TypeTraitUsesBslmaAllocator
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting allocator traits"
                          << "\n========================" << endl;

        typedef bdem_List Obj;

        ASSERT((0 == bslmf_IsConvertible<bslma_Allocator*, Obj>::VALUE));
        ASSERT((1 ==
             bslalg_HasTrait<Obj, bslalg_TypeTraitUsesBslmaAllocator>::VALUE));
}

static void testCase22(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING 'reserveMemory'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void reserveMemory(int numBytes);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'reserveMemory'"
                          << "\n=======================" << endl;

        const int SIZE = 4000;

        if (verbose) cout << "\nUsing 'BDEM_WRITE_ONCE'" << endl;
        {
            bslma_TestAllocator allocator;
            bdem_List mX(bdem_AggregateOption::BDEM_WRITE_ONCE, &allocator);

            const int beforeSize = allocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            LOOP2_ASSERT(allocator.numBytesInUse(), beforeSize,
                         SIZE <= allocator.numBytesInUse() - beforeSize);
        }

        if (verbose) cout << "\nUsing 'BDEM_WRITE_MANY'" << endl;
        {
            bslma_TestAllocator allocator;
            bdem_List mX(bdem_AggregateOption::BDEM_WRITE_MANY, &allocator);

            const int beforeSize = allocator.numBytesInUse();

            mX.reserveMemory(SIZE);

            LOOP2_ASSERT(allocator.numBytesInUse(), beforeSize,
                         SIZE <= allocator.numBytesInUse() - beforeSize);
        }

        if (verbose) cout << "\nEnd of 'reserveMemory' Test." << endl;
}

static void testCase21(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TESTING CTORS WITH INITIAL MEMORY
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   InitialMemory
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING CTORS WITH INITIAL MEMORY"
                          << "\n=================================" << endl;

        const int SIZE = 4000;  // initial memory

        if (verbose) cout << "\nWith allocMode only." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                bslma_TestAllocator allocator;

                bdem_List mX(bdem_AggregateOption::BDEM_WRITE_ONCE,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                bslma_TestAllocator allocator;

                bdem_List mX(bdem_AggregateOption::BDEM_WRITE_MANY,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nWith element types." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                const bdem_ElemType::Type CHAR = bdem_ElemType::BDEM_CHAR;

                bslma_TestAllocator allocator;

                bdem_List mX(&CHAR,
                             1,
                             bdem_AggregateOption::BDEM_WRITE_ONCE,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                const bdem_ElemType::Type CHAR = bdem_ElemType::BDEM_CHAR;

                bslma_TestAllocator allocator;

                bdem_List mX(&CHAR,
                             1,
                             bdem_AggregateOption::BDEM_WRITE_MANY,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nWith element types vector." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                const bsl::vector<bdem_ElemType::Type> CHAR_VECTOR(
                                                     1,
                                                     bdem_ElemType::BDEM_CHAR);

                bslma_TestAllocator allocator;

                bdem_List mX(CHAR_VECTOR,
                             bdem_AggregateOption::BDEM_WRITE_ONCE,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                const bsl::vector<bdem_ElemType::Type> CHAR_VECTOR(
                                                     1,
                                                     bdem_ElemType::BDEM_CHAR);

                bslma_TestAllocator allocator;

                bdem_List mX(CHAR_VECTOR,
                             bdem_AggregateOption::BDEM_WRITE_MANY,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nWith row." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_List list;

                bdem_List mX(list.row(),
                             bdem_AggregateOption::BDEM_WRITE_ONCE,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_List list;

                bdem_List mX(list.row(),
                             bdem_AggregateOption::BDEM_WRITE_MANY,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nWith copy ctor." << endl;
        {
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_ONCE'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_List original;

                bdem_List mX(original,
                             bdem_AggregateOption::BDEM_WRITE_ONCE,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
            if (veryVerbose) cout << "\tUsing 'BDEM_WRITE_MANY'." << endl;
            {
                bslma_TestAllocator allocator;
                bdem_List original;

                bdem_List mX(original,
                             bdem_AggregateOption::BDEM_WRITE_MANY,
                             bdem_List::InitialMemory(SIZE),
                             &allocator);

                LOOP_ASSERT(allocator.numBytesInUse(),
                            SIZE <= allocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\nEnd of 'InitialMemory' Test." << endl;
}

static void testCase20(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // COMPACT
        //   Ability to consolidate the memory footprint of an eclectic list.
        //
        // Concerns:
        //   - That something useful happens.
        //   - That the memory footprint BLOCKS and BYTES is not increased.
        //   - That the number of bytes allocated to a list decreases if the
        //     element sequence consists of many small object types
        //     interleaved with large object types.
        //   - That space allocated to elements that were deleted is recovered.
        //   - That the logical value of the list does not change as a result
        //     of compaction.
        //   - That a list created through copy-construction is automatically
        //     compacted.
        //
        // Plan:
        //   PART 1:
        //   - Create two logically identical 10-element lists consisting of
        //     alternating 'char' and 'double' elements created in two
        //     different ways:
        //     + (L1) Append all of the doubles first then insert the chars
        //            into every 2nd position.
        //     + (L2) Append all 10 elements in the order in which they appear.
        //     (Surprisingly, L1 is more compact than L2 because elements are
        //     physically grouped by alignment).
        //   - Confirm that L1 == L2
        //   - Confirm that L2 uses more memory than L1
        //   - Copy-construct L2b from L2.
        //   - Confirm that L2b uses no more memory than L1
        //   - Compact L2
        //   - Confirm that L2 now uses the same amount of memory as L2b.
        //
        //   PART 2:
        //   - Create two logically identical lists consisting of 10 'int'
        //     elements created in two different ways:
        //     + (L1) Append the 10 integers in the obvious manner and compact.
        //     + (L2) Append each integer twice, compact, then delete every
        //            2nd element.
        //   - Confirm that L1 == L2
        //   - Confirm that L2 uses more memory than L1
        //   - Copy-construct L2b from L2.
        //   - Confirm that L2b uses no more memory than L1 and less memory
        //     than L2.
        //   - Compact L2 (to recover space formerly occupied by deleted ints).
        //   - Confirm that L2 now uses the same amount of memory as L2b.
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   void compact();                             // <- note this moved!
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "COMPACT" << endl
                                  << "=======" << endl;

        {
            if (verbose) cout << "\nCompacting alternating char-double lists"
                              << endl;

            bslma_TestAllocator ta1, ta2, ta2b;
            bdem_List mL1(&ta1), mL2(&ta2);
            const bdem_List& L1 = mL1;
            const bdem_List& L2 = mL2;

            // Insert elements 1, 3, 5, 7 and 9 as doubles
            for (int i = 1; i < 10; i += 2) {
                mL1.appendDouble(double(i));
            }

            // Insert elements 0, 2, 4, 6 and 8 as chars
            for (int i = 0; i < 10; i += 2) {
                mL1.insertChar(i, 'A' + i);
            }

            // Now create an identical list adding elements in order.
            for (int i = 0; i < 10; i += 2) {
                mL2.appendChar('A' + i);
                mL2.appendDouble(double(i + 1));
            }

            const int L1_BYTES = ta1.numBytesInUse();
            const int L2_BYTES = ta2.numBytesInUse();

            if (veryVerbose) { T_ P(L1) T_ P(L2) }
            ASSERT(10 == L1.length());
            ASSERT(L1 == L2);
            ASSERT(L1_BYTES <= L2_BYTES);

            const bdem_List L2b(L2, &ta2b);
            const int L2B_BYTES = ta2b.numBytesInUse();
            ASSERT(L2B_BYTES <  L2_BYTES);
            ASSERT(L2B_BYTES <= L1_BYTES);

            mL2.compact();
            const int L2_COMPACT_BYTES = ta2.numBytesInUse();
            LOOP2_ASSERT(L1, L2, L1 == L2);
            ASSERT(L2_COMPACT_BYTES <  L2_BYTES);
            ASSERT(L2_COMPACT_BYTES == L2B_BYTES);

            if (veryVerbose) {
                P_(L1_BYTES) P_(L2_BYTES) P_(L2B_BYTES) P(L2_COMPACT_BYTES) }
        }

        {
            if (verbose) cout << "\nCompacting list after deletes" << endl;

            bslma_TestAllocator ta1, ta2, ta2b;
            bdem_List mL1(&ta1), mL2(&ta2);
            const bdem_List& L1 = mL1;
            const bdem_List& L2 = mL2;

            for (int i = 0; i < 10; ++i) {
                // Append once to L1
                mL1.appendInt(i);

                // Append twice to L2
                mL2.appendInt(i);
                mL2.appendInt(i);
            }
            mL1.compact();
            mL2.compact();

            // Delete every 2nd element from L2
            for (int i = 18; i >= 0; i -= 2) {
                mL2.removeElement(i);
            }

            const int L1_BYTES = ta1.numBytesInUse();
            const int L2_BYTES = ta2.numBytesInUse();

            if (veryVerbose) { T_ P(L1) T_ P(L2) }
            ASSERT(10 == L1.length());
            ASSERT(L1 == L2);
            ASSERT(L1_BYTES < L2_BYTES);

            const bdem_List L2b(L2, &ta2b);
            const int L2B_BYTES = ta2b.numBytesInUse();
            ASSERT(L2B_BYTES < L2_BYTES);
            ASSERT(L2B_BYTES <= L1_BYTES);

            mL2.compact();
            const int L2_COMPACT_BYTES = ta2.numBytesInUse();
            ASSERT(L1 == L2);
            ASSERT(L2_COMPACT_BYTES < L2_BYTES);
            ASSERT(L2_COMPACT_BYTES == L2B_BYTES);
            if (veryVerbose) {
                P_(L1_BYTES) P_(L2_BYTES) P_(L2B_BYTES) P(L2_COMPACT_BYTES) }
        }
}

static void testCase19(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // SWAP ELEMENTS
        //   Ability to swap two list elements efficiently.
        //
        // Concerns:
        //   - That all element types are handled properly.
        //   - That elements at all positions are handled properly.
        //   - That indices need not be ordered (e.g., i < j).
        //   - That no allocation occurs as a result of a swap.
        //   - That swapping an element with itself has no effect.
        //   - That swapping elements does not alter the address of either.
        //
        // Plan:
        //   - Create every type combination of two-element list in various
        //      allocation modes and swap them (two ways); verify the result.
        //   - Create a list from reference list A.  Use swapping adjacent
        //      elements to reverse the list.  Do it again to resore the list
        //      and compare with list A.  Finally, swap every element with
        //      itself and verify that there is no effect.
        //   - In the first test block, rely on the default allocator and
        //      ensure that the total number of blocks/bytes does not increase
        //      after construction.
        //   - In the second test block, provide an explicit allocator; ensure
        //      that no additional allocation occur after construction and
        //      that the default allocator is not used at all.
        //   - Finally, create a 2 element list; record the addresses of
        //      of the two elements, swap them, and verify that the address
        //      did not change.
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Loop-Based implementation technique
        //
        // Testing:
        //   void swapElements(int index1, int index2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "SWAP ELEMENTS" << endl
                  << "=============" << endl;

        if (verbose) cout << "\nSet up reference list A." << endl;

        bdem_List mA; const bdem_List& A = mA;  ASSERT( 0 == A.length());
        loadReferenceA(&mA);                    ASSERT(32 == A.length());

        if (veryVerbose) {
            cout << "\tlist A: "; A.print(cout, -1, 8);
        }

        const bdem_AggregateOption::AllocationStrategy MODES[] = {
            bdem_AggregateOption::BDEM_PASS_THROUGH,
            bdem_AggregateOption::BDEM_WRITE_ONCE,
            bdem_AggregateOption::BDEM_WRITE_MANY,
        };
        const int NUM_MODES = sizeof MODES / sizeof *MODES;

        if (verbose) cout << "\ntest all two-element-list swaps." << endl;
        {
            for (int i = 0; i < 32; ++i) {
                if (veryVeryVerbose) { T_ P(i) }

                for (int j = 0; j < 32; ++j) {
                    if (veryVeryVerbose) {T_ T_ P(j) }

                    bdem_List mX; const bdem_List& X = mX;
                    mX.appendElement(A[i]);
                    mX.appendElement(A[j]);
                    if (veryVeryVerbose) { T_ T_ P(X) }

                    bdem_List mY; const bdem_List& Y = mY;
                    mY.appendElement(A[j]);
                    mY.appendElement(A[i]);
                    if (veryVeryVerbose) {T_ T_ P(Y) }

                    for (int ai = 0; ai < NUM_MODES; ++ai) { // ALLOCATION MODE
                        const bdem_AggregateOption::AllocationStrategy
                                                              MODE = MODES[ai];
                        if (veryVeryVerbose) {
                             T_ T_ T_ P_(ai) P(MODE)
                        }

                        bslma_TestAllocator da;         // default allocation
                        const bslma_DefaultAllocatorGuard DAG(&da);

                        bdem_List mZ(X, MODE); bdem_List& Z = mZ;

                        const int USAGE = da.numBlocksTotal();
                        ASSERT(USAGE > 0);

                        LOOP3_ASSERT(i, j, ai, X == Z);
                        LOOP3_ASSERT(i, j, ai, Y != Z || i == j);
                        mZ.swapElements(0, 1);
                        LOOP3_ASSERT(i, j, ai, X != Z || i == j);
                        LOOP3_ASSERT(i, j, ai, Y == Z);
                        mZ.swapElements(1, 0);
                        LOOP3_ASSERT(i, j, ai, X == Z);
                        LOOP3_ASSERT(i, j, ai, Y != Z || i == j);

                        LOOP3_ASSERT(i, j, ai, USAGE == da.numBlocksTotal());
                    }
                }
            }
        }

        if (verbose) cout << "\ntest all two-element-list swaps (nulls)."
                          << endl;
        {
            for (int i = 0; i < 32; ++i) {
                if (veryVeryVerbose) { T_ P(i) }

                for (int j = 0; j < 32; ++j) {
                    if (veryVeryVerbose) {T_ T_ P(j) }

                    bdem_List mX; const bdem_List& X = mX;
                    mX.appendNullElement(A[i].type());
                    mX.appendElement(A[j]);
                    if (veryVeryVerbose) { T_ T_ P(X) }

                    bdem_List mY; const bdem_List& Y = mY;
                    mY.appendElement(A[j]);
                    mY.appendNullElement(A[i].type());
                    if (veryVeryVerbose) {T_ T_ P(Y) }

                    for (int ai = 0; ai < NUM_MODES; ++ai) { // ALLOCATION MODE
                        const bdem_AggregateOption::AllocationStrategy
                                                              MODE = MODES[ai];
                        if (veryVeryVerbose) {
                             T_ T_ T_ P_(ai) P(MODE)
                        }

                        bslma_TestAllocator da;         // default allocation
                        const bslma_DefaultAllocatorGuard DAG(&da);

                        bdem_List mZ(X, MODE); bdem_List& Z = mZ;

                        const int USAGE = da.numBlocksTotal();
                        ASSERT(USAGE > 0);

                        LOOP3_ASSERT(i, j, ai, X == Z);
                        LOOP3_ASSERT(i, j, ai, Y != Z || i == j);
                        mZ.swapElements(0, 1);
                        LOOP3_ASSERT(i, j, ai, X != Z || i == j);
                        LOOP3_ASSERT(i, j, ai, Y == Z);
                        mZ.swapElements(1, 0);
                        LOOP3_ASSERT(i, j, ai, X == Z);
                        LOOP3_ASSERT(i, j, ai, Y != Z || i == j);

                        LOOP3_ASSERT(i, j, ai, USAGE == da.numBlocksTotal());
                    }
                }
            }
        }

        if (verbose) cout << "\nReverse list of all 32 element types." << endl;
        {
            bslma_TestAllocator da;             // default allocation
            const bslma_DefaultAllocatorGuard DAG(&da);

            bslma_TestAllocator a;              // explicit allocation

            bdem_List mX(A, &a); const bdem_List& X = mX;
            const int USAGE = a.numBlocksTotal();
            ASSERT(USAGE > 0);

            if (veryVeryVerbose) {
                cout << "\tBefore X = "; X.print(cout, -1, 8); }
            ASSERT(A == X);

            if (veryVerbose) {
                cout << endl <<
                "*************************** BEFORE **************************"
                << endl;
                cout << "SPECIFIED ALLOCATOR:" << endl;
                a.print();
                cout << "\nDEFAULT ALLOCATOR:" << endl;
                da.print();
            }
            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());

            int i; // resued index

            for (i = 1; i < 32; ++i) {
                if (veryVeryVerbose) { T_ T_ P(i) }
                for (int j = 0; j < i; ++j) {
                    if (veryVeryVerbose) { T_ T_ T_ P(j) }

                    if (veryVeryVerbose) { T_ T_ T_ P(X) }
                    mX.swapElements(i, j);
                    if (veryVeryVerbose) { T_ T_ T_ P(X) }
                }
            }

            if (veryVeryVerbose) {
                cout << "\tDuring X = "; X.print(cout, -1, 8); }
            ASSERT(A != X);

            if (veryVerbose) {
                cout << endl <<
                "*************************** DURING **************************"
                << endl;
                cout << "SPECIFIED ALLOCATOR:" << endl;
                a.print();
                cout << "\nDEFAULT ALLOCATOR:" << endl;
                da.print();
            }
            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());

            for (i = 0; i < 31; ++i) {
                if (veryVeryVerbose) { T_ T_ P(i) }
                for (int j = i + 1; j < 32; ++j) {
                    if (veryVeryVerbose) { T_ T_ T_ P(j) }

                    if (veryVeryVerbose) { T_ T_ T_ P(X) }
                    mX.swapElements(i, j);
                    if (veryVeryVerbose) { T_ T_ T_ P(X) }
                }
            }

            if (veryVeryVerbose) {
                cout << "\tAfter X = "; X.print(cout, -1, 8); }
            ASSERT(A == X);

            if (veryVerbose) {
                cout << endl <<
                "*************************** AFTER ***************************"
                << endl;
                cout << "SPECIFIED ALLOCATOR:" << endl;
                a.print();
                cout << "\nDEFAULT ALLOCATOR:" << endl;
                da.print();
            }
            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());

            if (verbose) cout <<
                "\nAliasing check: swap each element with itself." << endl;

            for (i = 0; i < 32; ++i) {
                if (veryVeryVerbose) { T_ P(i) }
                if (veryVeryVerbose) { T_ T_ P(X) }
                mX.swapElements(i, i);
                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, A == X);
            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());
        }

        if (verbose) cout <<
            "\nVerify that element addresses do not change." << endl;
        {
            bdem_List mL; const bdem_List& L = mL;
            mL.appendInt(A02);
            mL.appendInt(B02);

            bdem_List mX(L); const bdem_List& X = mX;

            const int *const P0 = &X[0].theInt();
            const int *const P1 = &X[1].theInt();
            ASSERT( L ==  X);
            ASSERT(P0 == &X[0].theInt());
            ASSERT(P0 != &X[1].theInt());
            ASSERT(P1 != &X[0].theInt());
            ASSERT(P1 == &X[1].theInt());

            mX.swapElements(0, 1);
            ASSERT( L !=  X);
            ASSERT(P0 != &X[0].theInt());
            ASSERT(P0 == &X[1].theInt());
            ASSERT(P1 == &X[0].theInt());
            ASSERT(P1 != &X[1].theInt());

            mX.swapElements(0, 1);
            ASSERT( L ==  X);
            ASSERT(P0 == &X[0].theInt());
            ASSERT(P0 != &X[1].theInt());
            ASSERT(P1 != &X[0].theInt());
            ASSERT(P1 == &X[1].theInt());
        }

        static const struct {
            int d_line;       // line
            int d_index1;     // first index
            int d_index2;     // second index
        } DATA[] = {
        // LINE INDEX1 INDEX2
        // ==== ====== ======
          { L_ , 0     , 0  },
          { L_ , 0     , 1  },
          { L_ , 0     , 15 },
          { L_ , 0     , 16 },
          { L_ , 0     , 31 },
          { L_ , 0     , 32 },
          { L_ , 0     , 46 },
          { L_ , 0     , 47 },
          { L_ , 0     , 62 },
          { L_ , 0     , 63 },

          { L_ , 1     , 0  },
          { L_ , 1     , 1  },
          { L_ , 1     , 15 },
          { L_ , 1     , 16 },
          { L_ , 1     , 31 },
          { L_ , 1     , 32 },
          { L_ , 1     , 46 },
          { L_ , 1     , 47 },
          { L_ , 1     , 62 },
          { L_ , 1     , 63 },

          { L_ , 15    , 0  },
          { L_ , 15    , 1  },
          { L_ , 15    , 15 },
          { L_ , 15    , 16 },
          { L_ , 15    , 31 },
          { L_ , 15    , 32 },
          { L_ , 15    , 46 },
          { L_ , 15    , 47 },
          { L_ , 15    , 62 },
          { L_ , 15    , 63 },

          { L_ , 16    , 0  },
          { L_ , 16    , 1  },
          { L_ , 16    , 15 },
          { L_ , 16    , 16 },
          { L_ , 16    , 31 },
          { L_ , 16    , 32 },
          { L_ , 16    , 46 },
          { L_ , 16    , 47 },
          { L_ , 16    , 62 },
          { L_ , 16    , 63 },

          { L_ , 31    , 0  },
          { L_ , 31    , 1  },
          { L_ , 31    , 15 },
          { L_ , 31    , 16 },
          { L_ , 31    , 31 },
          { L_ , 31    , 32 },
          { L_ , 31    , 46 },
          { L_ , 31    , 47 },
          { L_ , 31    , 62 },
          { L_ , 31    , 63 },

          { L_ , 32    , 0  },
          { L_ , 32    , 1  },
          { L_ , 32    , 15 },
          { L_ , 32    , 16 },
          { L_ , 32    , 31 },
          { L_ , 32    , 32 },
          { L_ , 32    , 46 },
          { L_ , 32    , 47 },
          { L_ , 32    , 62 },
          { L_ , 32    , 63 },

          { L_ , 46    , 0  },
          { L_ , 46    , 1  },
          { L_ , 46    , 15 },
          { L_ , 46    , 16 },
          { L_ , 46    , 31 },
          { L_ , 46    , 32 },
          { L_ , 46    , 46 },
          { L_ , 46    , 47 },
          { L_ , 46    , 62 },
          { L_ , 46    , 63 },

          { L_ , 47    , 0  },
          { L_ , 47    , 1  },
          { L_ , 47    , 15 },
          { L_ , 47    , 16 },
          { L_ , 47    , 31 },
          { L_ , 47    , 32 },
          { L_ , 47    , 46 },
          { L_ , 47    , 47 },
          { L_ , 47    , 62 },
          { L_ , 47    , 63 },

          { L_ , 62    , 0  },
          { L_ , 62    , 1  },
          { L_ , 62    , 15 },
          { L_ , 62    , 16 },
          { L_ , 62    , 31 },
          { L_ , 62    , 32 },
          { L_ , 62    , 46 },
          { L_ , 62    , 47 },
          { L_ , 62    , 62 },
          { L_ , 62    , 63 },

          { L_ , 63    , 0  },
          { L_ , 63    , 1  },
          { L_ , 63    , 15 },
          { L_ , 63    , 16 },
          { L_ , 63    , 31 },
          { L_ , 63    , 32 },
          { L_ , 63    , 46 },
          { L_ , 63    , 47 },
          { L_ , 63    , 62 },
          { L_ , 63    , 63 }
        };

        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bdem_List mU; const bdem_List& U = mU;  ASSERT( 0 == U.length());
        loadReferenceN(&mU);                    ASSERT(32 == U.length());
        loadReferenceN(&mU);                    ASSERT(64 == U.length());

        for (int i = 0; i < NUM_DATA; ++i) {
            const int  LINE     = DATA[i].d_line;
            const int  INDEX1   = DATA[i].d_index1;
            const int  INDEX2   = DATA[i].d_index2;

            for (int x = 0; x < 2; ++x) {
                for (int y = 0; y < 2; ++y) {
                    if (INDEX1 == INDEX2 && x != y) {
                        continue;
                    }

                    // setup
                    if (x) {
                        mU[INDEX1].makeNull();
                        LOOP_ASSERT(LINE, U[INDEX1].isNull());
                    }
                    else {
                        makeElemRefUnset(mU[INDEX1]);
                        LOOP_ASSERT(LINE, U[INDEX1].isNonNull());
                        LOOP_ASSERT(LINE, isUnset(U[INDEX1]));
                    }

                    if (y) {
                        mU[INDEX2].makeNull();
                        LOOP_ASSERT(LINE, U[INDEX2].isNull());
                    }
                    else {
                        makeElemRefUnset(mU[INDEX2]);
                        LOOP_ASSERT(LINE, U[INDEX2].isNonNull());
                        LOOP_ASSERT(LINE, isUnset(U[INDEX2]));
                    }

                    mU.swapElements(INDEX1, INDEX2);

                    if (y) {
                        LOOP_ASSERT(LINE, U[INDEX1].isNull());
                    }
                    else {
                        LOOP_ASSERT(LINE, U[INDEX1].isNonNull());
                        LOOP_ASSERT(LINE, isUnset(U[INDEX1]));
                    }

                    if (x) {
                        LOOP_ASSERT(LINE, U[INDEX2].isNull());
                    }
                    else {
                        LOOP_ASSERT(LINE, U[INDEX2].isNonNull());
                        LOOP_ASSERT(LINE, isUnset(U[INDEX2]));
                    }
                }
            }
        }
}

static void testCase18(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // IS UNSET/MAKE NULL/IS NULL
        //   Ability to make and verify that elements are unset.
        //
        // Concerns:
        //   - That all element types are considered.
        //   - That elements at all relevant positions are considered.
        //   - That empty lists are handled properly.
        //
        // Plan:
        //   - Create three reference lists, A, B, N each containing all 32
        //      element types (N stands for null).
        //   - To test 'makeAllNull', apply the function to copies of both
        //      set reference lists A and B and verify that they are
        //      subsequently the same value as N and 'isAnyNonNull' yields
        //      false.
        //   - Finally, to verify the boundary condition, create an empty set
        //      and apply all three functions to it.
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based implementation techniques
        //
        // Testing:
        //   bool isAnyNull() const;
        //   bool isAnyNonNull() const;
        //   void makeAllNull();
        //   void makeNull(int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "IS UNSET/MAKE NULL/IS NULL"
                          << endl << "=========================="
                          << endl;

        if (verbose) cout << "\nSet up reference lists." << endl;

        static const int N_ELEM = 64;

        bdem_List mA; const bdem_List& A = mA;  ASSERT(     0 == A.length());
        loadReferenceA(&mA);                    ASSERT(    32 == A.length());
        loadReferenceA(&mA);                    ASSERT(N_ELEM == A.length());

        bdem_List mB; const bdem_List& B = mB;  ASSERT(     0 == B.length());
        loadReferenceB(&mB);                    ASSERT(    32 == B.length());
        loadReferenceB(&mB);                    ASSERT(N_ELEM == B.length());

        bdem_List mU; const bdem_List& U = mU;  ASSERT(     0 == U.length());
        loadReferenceN(&mU);                    ASSERT(    32 == U.length());
        loadReferenceN(&mU);                    ASSERT(N_ELEM == U.length());

        bdem_List mN; const bdem_List& N = mN;   ASSERT(     0 == N.length());
        loadReferenceA(&mN);                     ASSERT(    32 == N.length());
        loadReferenceA(&mN);                     ASSERT(N_ELEM == N.length());

        bdem_List mNN; const bdem_List& NN = mNN;ASSERT(     0 == NN.length());
        loadReferenceNull(&mNN);                 ASSERT(    32 == NN.length());
        loadReferenceNull(&mNN);                 ASSERT(N_ELEM == NN.length());

        for (int i = 0; i < N_ELEM; ++i) {
            LOOP_ASSERT(i, N[i].isNonNull());
            mN[i].makeNull();
            LOOP_ASSERT(i, N[i].isNull());
        }

        if (veryVerbose) {
            cout << "\tlist A: "; A.print(cout, -1, 8);
            cout << "\tlist B: "; B.print(cout, -1, 8);
            cout << "\tlist U: "; U.print(cout, -1, 8);
            cout << "\tlist N: "; N.print(cout, -1, 8);
        }

        if (verbose) cout << "\nTesting 'makeAllNull'." << endl;
        {
            {
                bdem_List mX(A); const bdem_List& X = mX;
                ASSERT(U != X);
                ASSERT(1 == X.isAnyNonNull());
                ASSERT(1 == isAnySet(X));
                ASSERT(0 == X.isAnyNull());
                mX.makeAllNull();
                ASSERT(U  != X);
                LOOP2_ASSERT(X, NN, NN == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
            }
            {
                bdem_List mX(B); const bdem_List& X = mX;
                ASSERT(U != X);
                ASSERT(1 == X.isAnyNonNull());
                ASSERT(1 == isAnySet(X));
                ASSERT(0 == X.isAnyNull());
                mX.makeAllNull();
                ASSERT(U  != X);
                ASSERT(NN == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
            }
            {
                bdem_List mX(U); const bdem_List& X = mX;
                ASSERT(U == X);
                ASSERT(1 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(0 == X.isAnyNull());
                mX.makeAllNull();
                ASSERT(U  != X);
                ASSERT(NN == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
            }
            {
                bdem_List mX(N); const bdem_List& X = mX;
                ASSERT(N == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
                mX.makeAllNull();
                ASSERT(U  != X);
                ASSERT(NN == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
            }
            {
                bdem_List mX(NN); const bdem_List& X = mX;
                ASSERT(NN == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
                mX.makeAllNull();
                ASSERT(U  != X);
                ASSERT(NN == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
            }
        }

        if (verbose) cout << "\nTesting 'makeNull'." << endl;
        {
            for (int i = 0; i < N_ELEM; ++i) {
                if (veryVerbose) { T_ P(i) }
                bdem_List mX(A); const bdem_List& X = mX;
                ASSERT(U != X);
                ASSERT(1 == X.isAnyNonNull());
                ASSERT(1 == isAnySet(X));
                ASSERT(0 == X.isAnyNull());

                mX.makeNull(i);
                ASSERT(1 == X.isAnyNonNull());
                ASSERT(1 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());

                for (int j = 0; j < N_ELEM; ++j) {
                    ASSERT((i == j) == X[j].isNull());
                }
            }

        }

        if (verbose) cout <<
                        "\nTesting the empty-list boundary condition." << endl;
        {
            bdem_List mX; const bdem_List& X = mX;
            ASSERT(0 == X.isAnyNull());
            ASSERT(0 == X.isAnyNonNull());
            ASSERT(0 == isAnySet(X));
            mX.makeAllNull();
            ASSERT(0 == X.isAnyNull());
            ASSERT(0 == X.isAnyNonNull());
            ASSERT(0 == isAnySet(X));
        }

        if (verbose) cout << "\nTesting 'isAnyNull'." << endl;
        {
            ASSERT(0 == A.isAnyNull());

            for (int i = 0; i < N_ELEM; ++i) {
                if (veryVerbose) { T_ P(i) }
                if (veryVeryVerbose) { cout << "\t\t-----------------------"
                                "----------------------------------------\n"; }
                bdem_List mX(A); const bdem_List& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, 0 == X.isAnyNull());
                mX[i].replaceValue(N[i]);
                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, 1 == X.isAnyNull());
                mX[i].replaceValue(A[i]);
                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, 0 == X.isAnyNull());
            }
        }

        if (verbose) cout << "\nTesting 'isAnyNonNull'." << endl;
        {
            ASSERT(0 == N.isAnyNonNull());
            ASSERT(0 == isAnySet(N));

            for (int i = 0; i < N_ELEM; ++i) {
                if (veryVerbose) { T_ P(i) }
                if (veryVeryVerbose) { cout << "\t\t-----------------------"
                                "----------------------------------------\n"; }
                bdem_List mX(N); const bdem_List& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, 0 == X.isAnyNonNull());
                LOOP_ASSERT(i, 0 == isAnySet(X));
                mX[i].replaceValue(A[i]);
                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, 1 == X.isAnyNonNull());
                LOOP_ASSERT(i, 1 == isAnySet(X));
                mX[i].replaceValue(N[i]);
                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, 0 == X.isAnyNonNull());
                LOOP_ASSERT(i, 0 == isAnySet(X));
                mX[i].replaceValue(A[i]);
                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, 1 == X.isAnyNonNull());
                LOOP_ASSERT(i, 1 == isAnySet(X));
                mX[i].replaceValue(N[i]);
                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, 0 == X.isAnyNonNull());
                LOOP_ASSERT(i, 0 == isAnySet(X));
            }
        }

        if (verbose) cout << "\nTesting 'makeAllNull'." << endl;
        {
            {
                bdem_List mX(A); const bdem_List& X = mX;
                ASSERT(N != X);
                ASSERT(1 == X.isAnyNonNull());
                ASSERT(1 == isAnySet(X));
                ASSERT(0 == X.isAnyNull());
                mX.makeAllNull();
                ASSERT(N == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
            }
            {
                bdem_List mX(U); const bdem_List& X = mX;
                ASSERT(N != X);
                ASSERT(1 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(0 == X.isAnyNull());
                mX.makeAllNull();
                ASSERT(N == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
            }
            {
                bdem_List mX(N); const bdem_List& X = mX;
                ASSERT(N == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
                mX.makeAllNull();
                ASSERT(N == X);
                ASSERT(0 == X.isAnyNonNull());
                ASSERT(0 == isAnySet(X));
                ASSERT(1 == X.isAnyNull());
            }
        }

        if (verbose) cout <<
                        "\nTesting the empty-list boundary condition." << endl;
        {
            bdem_List mX; const bdem_List& X = mX;
            ASSERT(0 == X.isAnyNull());
            ASSERT(0 == isAnySet(X));
            ASSERT(0 == X.isAnyNonNull());
            mX.makeAllNull();
            ASSERT(0 == X.isAnyNull());
            ASSERT(0 == X.isAnyNonNull());
            ASSERT(0 == isAnySet(X));
        }
}

static void testCase17(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // REPLACING/RESETTING ELEMENTS
        //   Ability to replace/reset elements of a list
        //
        // Concerns:
        //   - That the indices are implemented correctly.
        //   - That the correct values are inserted properly.
        //   - That the source objects are not changed (except with aliasing).
        //   - That (shallow) aliasing works properly.
        //
        // Plan:
        //   - Create three reference lists, A, B, N each containing all 32
        //      element types (N stands for null).  Create Duplicates AA, BB,
        //      and NN as controls, respectively.
        //   - To test replaceElement, go through and replace each element to
        //      each possible element type in the second list in a loop.
        //      Verify each type and value.  Then restore the type and value,
        //      and verify that the list is the same as initially.
        //   - Follow with a test that replaces every element type with itself
        //      and verify that it is unchanged.
        //   - To test resetElement, go through and reset each element to
        //      each possible element type (and corresponding unset value)
        //      in a loop.  Verify each type and unset value.  Then restore
        //      the type and value, and verify that the list is the same as
        //      initially.
        //   - To test reset, create a few different lists of unset values
        //      and a few different lists, each using the g function.  For
        //      each result list, extract the array of types, apply that
        //      array to each of the initial list values and verify that it
        //      now has the expected value.
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force, Loop-Based, and Array-Driven Implementation
        //     Techniques
        //
        // Testing:
        //   void replaceElement(int dstIndex, const bdem_ConstElemRef& se);
        //   void resetElement(int dstIndex, bdem_ElemType::Type elemType);
        //   void reset(const bdem_ElemType::Type elementTypes[], int ne);
        //   void reset(const bsl::vector<bdem_ElemType::Type>& et);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "REPLACING AND RESETTING LIST ELEMENTS" << endl
                  << "=====================================" << endl;

        if (verbose) cout << "\nSet up reference lists." << endl;

        const int N_ELEM = 64;

        bdem_List mA; const bdem_List& A = mA;  ASSERT(     0 == A.length());
        loadReferenceA(&mA);                    ASSERT(    32 == A.length());
        loadReferenceA(&mA);                    ASSERT(N_ELEM == A.length());

        bdem_List mB; const bdem_List& B = mB;  ASSERT(     0 == B.length());
        loadReferenceB(&mB);                    ASSERT(    32 == B.length());
        loadReferenceB(&mB);                    ASSERT(N_ELEM == B.length());

        bdem_List mU; const bdem_List& U = mU;  ASSERT(     0 == U.length());
        loadReferenceN(&mU);                    ASSERT(    32 == U.length());
        loadReferenceN(&mU);                    ASSERT(N_ELEM == U.length());

        bdem_List mN; const bdem_List& N = mN;  ASSERT(     0 == N.length());
        loadReferenceN(&mN);                    ASSERT(    32 == N.length());
        loadReferenceN(&mN);                    ASSERT(N_ELEM == N.length());
        for (int i = 0; i < N_ELEM; ++i) {
            mN[i].makeNull();
        }

        const bdem_List AA(A), BB(B), UU(U), NN(N);

        if (veryVerbose) {
            cout << "\tlist AA: "; AA.print(cout, -1, 8);
            cout << "\tlist BB: "; BB.print(cout, -1, 8);
            cout << "\tlist UU: "; UU.print(cout, -1, 8);
            cout << "\tlist NN: "; NN.print(cout, -1, 8);
        }

        if (verbose) cout << "\nTesting 'replaceElement'." << endl;
        {
            bslma_TestAllocator da(veryVeryVerbose);
            bslma_DefaultAllocatorGuard dag(&da);

            bslma_TestAllocator ta(veryVeryVerbose);
            bdem_List mX(A, &ta); bdem_List& X = mX;

            for (int i = 0; i < N_ELEM; ++i) {     // for each initial element
                for (int j = 0; j < N_ELEM; ++j) { // for every kind of element
                    if (veryVerbose) {
                        T_ T_ cout << "------------------------------------\n";
                        T_ T_ P_(i) P(j)
                    }

                    mX.replaceElement(i, B[j]);   // Change i'th entry.
                    if (veryVeryVerbose) {
                        cout << "\t\tchanged list X: "; X.print(cout, -2, 8);
                    }

                    LOOP2_ASSERT(i, j, A != X);
                    LOOP2_ASSERT(i, j, B[j] == X[i]);

                    mX.replaceElement(i, A[i]);   // Restore i'th entry.
                    if (veryVeryVerbose) {
                        cout << "\t\trestored list X: "; X.print(cout, -2, 8);
                    }
                    LOOP2_ASSERT(i, j, A == X);
                    LOOP2_ASSERT(i, j, A[i] == X[i]);

                    LOOP2_ASSERT(i, j, AA == A);  // Verify that sources are
                    LOOP2_ASSERT(i, j, BB == B);  // not affected.
                }
            }
        }

        if (verbose) cout << "\nTesting 'replaceElement' (nulls)." << endl;
        {
            bslma_TestAllocator ta(veryVeryVerbose);
            bdem_List mX(A, &ta); bdem_List& X = mX;
            for (int i = 0; i < N_ELEM; ++i) {     // for each initial element
                for (int j = 0; j < N_ELEM; ++j) { // for every kind of element

                    if (veryVerbose) {
                        T_ T_ cout << "------------------------------------\n";
                        T_ T_ P_(i) P(j)
                    }

                    LOOP2_ASSERT(i, j, X[i].isNonNull());
                    mX.replaceElement(i, N[j]);   // Change i'th entry.
                    if (veryVeryVerbose) {
                        cout << "\t\tchanged list X: "; X.print(cout, -2, 8);
                    }
                    LOOP2_ASSERT(i, j, A != X);
                    LOOP2_ASSERT(i, j, N[j] == X[i]);
                    LOOP2_ASSERT(i, j, X[i].isNull());

                    mX.replaceElement(i, A[i]);   // Restore i'th entry.
                    if (veryVeryVerbose) {
                        cout << "\t\trestored list X: "; X.print(cout, -2, 8);
                    }
                    LOOP2_ASSERT(i, j, A == X);
                    LOOP2_ASSERT(i, j, A[i] == X[i]);
                    LOOP2_ASSERT(i, j, X[i].isNonNull());

                    LOOP2_ASSERT(i, j, AA == A);  // Verify that sources are
                    LOOP2_ASSERT(i, j, BB == B);  // not affected.
                }
            }
        }

        if (verbose) cout << "\tShallow alias test." << endl;
        {
            bslma_TestAllocator ta(veryVeryVerbose);
            bdem_List mX(A, &ta); bdem_List& X = mX;
            for (int i = 0; i < N_ELEM; ++i) {        // for each element
                mX.replaceElement(i, X[i]);       // Change i'th entry.
                if (veryVerbose) { T_ T_ P_(i)
                    cout << ", changed list X: "; X.print(cout, -2, 8);
                }
                LOOP_ASSERT(i, A == X);
            }
        }

        if (verbose) cout << "\tShallow alias test (null)." << endl;
        {
            bslma_TestAllocator ta(veryVeryVerbose);
            bdem_List mX(N, &ta); bdem_List& X = mX;
            for (int i = 0; i < N_ELEM; ++i) {    // for each element
                mX.replaceElement(i, X[i]);       // Change i'th entry.
                if (veryVerbose) { T_ T_ P_(i)
                    cout << ", changed list X: "; X.print(cout, -2, 8);
                }
                LOOP_ASSERT(i, N == X);
            }
        }

        if (verbose) cout << "\nTesting 'resetElement'." << endl;
        {
            bslma_TestAllocator ta(veryVeryVerbose);
            bdem_List mX(A, &ta); bdem_List& X = mX;

            for (int i = 0; i < N_ELEM; ++i) {     // for each initial element
                for (int j = 0; j < N_ELEM; ++j) { // for every kind of element

                    if (veryVerbose) {
                        T_ T_ cout << "------------------------------------\n";
                        T_ T_ P_(i) P(j)
                    }

                    mX.resetElement(i, B.elemType(j)); // Change i'th entry.
                    if (veryVeryVerbose) {
                        cout << "\t\tchanged list X: "; X.print(cout, -2, 8);
                    }
                    LOOP2_ASSERT(i, j, A != X);
                    LOOP2_ASSERT(i, j, N[j] == X[i]);

                    mX.replaceElement(i, A[i]);   // Restore i'th entry.
                    if (veryVeryVerbose) {
                        cout << "\t\trestored list X: "; X.print(cout, -2, 8);
                    }
                    LOOP2_ASSERT(i, j, A == X);
                    LOOP2_ASSERT(i, j, A[i] == X[i]);

                    LOOP2_ASSERT(i, j, AA == A);  // Verify that sources are
                    LOOP2_ASSERT(i, j, BB == B);  // not affected.
                }
            }
        }

        if (verbose) cout << "\nTesting 'resetElement' (null)." << endl;
        {
            bslma_TestAllocator ta(veryVeryVerbose);
            bdem_List mX(N, &ta); bdem_List& X = mX;

            for (int i = 0; i < N_ELEM; ++i) {     // for each initial element
                for (int j = 0; j < N_ELEM; ++j) { // for every kind of element

                    if (veryVerbose) {
                        T_ T_ cout << "------------------------------------\n";
                        T_ T_ P_(i) P(j)
                    }

                    mX.resetElement(i, A.elemType(j)); // Change i'th entry.
                    if (veryVeryVerbose) {
                        cout << "\t\tchanged list X: "; X.print(cout, -2, 8);
                    }
                    if (i == j || (i + 32) == j || (j + 32) == i) {
                        LOOP2_ASSERT(i, j, N == X);
                    }
                    else {
                        LOOP2_ASSERT(i, j, N != X);
                    }
                    LOOP2_ASSERT(i, j, N[j] == X[i]);

                    mX.replaceElement(i, N[i]);   // Restore i'th entry.
                    if (veryVeryVerbose) {
                        cout << "\t\trestored list X: "; X.print(cout, -2, 8);
                    }
                    LOOP2_ASSERT(i, j, N == X);
                    LOOP2_ASSERT(i, j, N[i] == X[i]);

                    LOOP2_ASSERT(i, j, NN == N);  // Verify that sources are
                    LOOP2_ASSERT(i, j, BB == B);  // not affected.
                }
            }
        }

        if (verbose) cout << "\nTesting 'reset'." << endl;
        {
            const char SPECS[][N_ELEM] = {
                "", "A", "BC", "DEF", "GHIJ", "KLMNO",
                "PQRSTU", "VABCDEF", "GHIJKLMN", "OPQRSTUVA",
                "VUTSRQPONMLKJIHGFEDCBA"
            };
            const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

            const bdem_AggregateOption::AllocationStrategy MODES[] = {
                bdem_AggregateOption::BDEM_PASS_THROUGH,
                bdem_AggregateOption::BDEM_WRITE_ONCE,
                bdem_AggregateOption::BDEM_WRITE_MANY
            };
            const int NUM_MODES = sizeof MODES / sizeof *MODES;

            // for each final expected value
            //   Set up the expected value (ref U).
            //   Extract the 'bdem_ElemType[]' array.
            //   for each initial value
            //       Set up the initial value.
            //       for each allocation mode
            //           Create a test list with the initial value (ref A).
            //           Apply 'reset' with the extracted array.
            //           Verify the expected value.

            for (int ei = 0; ei < NUM_SPECS; ++ei) {
                const bdem_List EXPECTED_VALUE = g(SPECS[ei], N);

                bsl::vector<bdem_ElemType::Type> elementTypes;
                EXPECTED_VALUE.elemTypes(&elementTypes);
                const int                  LENGTH = EXPECTED_VALUE.length();
                const bdem_ElemType::Type *ETA    = 0;
                if (LENGTH > 0) {
                    ETA = &elementTypes[0];
                }

                if (veryVerbose) {
                    cout << "\t============================================\n";
                    T_ P_(ei) P(elementTypes)
                }

                for (int si = 0; si < NUM_SPECS; ++si) { // INITIAL VALUES
                    const bdem_List INITIAL_VALUE = g(SPECS[si], A);
                    if (veryVerbose) {
                         cout << "\t\t-------------------------------------\n";
                         T_ T_ P_(si) P(INITIAL_VALUE)
                    }

                    for (int ai = 0; ai < NUM_MODES; ++ai) { // ALLOCATION MODE
                        const bdem_AggregateOption::AllocationStrategy
                                                              MODE = MODES[ai];
                        if (veryVerbose) {
                             T_ T_ T_ P_(ai) P(MODE)
                        }

                        bslma_TestAllocator da;
                         const bslma_DefaultAllocatorGuard dag(&da);

                        bslma_TestAllocator ta;
                        bdem_List mX(INITIAL_VALUE, MODE, &ta);
                        bdem_List mY(INITIAL_VALUE, MODE, &ta);
                        const bdem_List& X = mX; const bdem_List& Y = mY;

                        LOOP3_ASSERT(ei, si, ai,  INITIAL_VALUE == X);
                        LOOP3_ASSERT(ei, si, ai,  INITIAL_VALUE == Y);
                        LOOP3_ASSERT(ei, si, ai, EXPECTED_VALUE != X||!LENGTH);
                        LOOP3_ASSERT(ei, si, ai, EXPECTED_VALUE != Y||!LENGTH);

                        mX.reset(ETA, LENGTH);
                        mY.reset(elementTypes);

                        LOOP3_ASSERT(ei, si, ai,  INITIAL_VALUE != X||!LENGTH);
                        LOOP3_ASSERT(ei, si, ai, EXPECTED_VALUE == X);
                        LOOP3_ASSERT(ei, si, ai,  INITIAL_VALUE != Y||!LENGTH);
                        LOOP3_ASSERT(ei, si, ai, EXPECTED_VALUE == Y);
                    } // end for each allocation mode
                } // end for each exemplar sequence
            } // end for each test vector
        } // end block scope
}

static void testCase16(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // APPENDING/INSERTING/REMOVING LIST/ROW ELEMENTS
        //   Ability to append/insert/remove 0 or more elements from list/row.
        //
        // Concerns:
        //   - That the indices are implemented correctly.
        //   - That the correct values are inserted properly.
        //   - That the source objects are not changed (except with aliasing).
        //   - That (shallow) aliasing works properly.
        //   - That if an allocator is specified, the default is NOT used.
        //
        // Plan:
        //  Create two tables - the second will address aliasing.
        //   - Create tests for the most general case implemented as a table.
        //   - For each special case that applies, apply that vector.
        //   - Implement the tables such that each list is created with
        //      an explicit allocator and verify that the default is NOT used.
        //   - Use orthogonal perturbation to exercise:
        //      - different exemplar element types/values.
        //      - different allocation strategies.
        //   - Explicitly test the locally installed default allocator for zero
        //      total usage prior to leaving scope in each test block.
        //
        //  Separately test removeAll by creating a few lists (Ad Hoc), remove
        //   all the elements, and verify (Brute Force) that the list is empty.
        //
        // Tactics:
        //   - Depth-Ordered-Enumeration and Ad-Hoc Data Selection Methods
        //   - Table-Based and Brute-Force Implementation Technique
        //
        // Testing:
        //   void appendElements(const bdem_Row& srcElements);
        //   void appendElements(const bdem_List& srcElements);
        //   void appendElements(const bdem_Row&  se, int si, int ne);
        //   void appendElements(const bdem_List& se, int si, int ne);
        //
        //   void insertElement(int di, const bdem_ConstElemRef& srcElement);
        //   void insertElements(int dstIndex, const bdem_Row& srcElements);
        //   void insertElements(int dstIndex, const bdem_List& srcElements);
        //   void insertElements(int di, const bdem_Row&  sr, int si, int ne);
        //   void insertElements(int di, const bdem_List& sl, int si, int ne);
        //
        //   void removeElement(int index);
        //   void removeElements(int startIndex, int numElements);
        //
        //
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "APPENDING/INSERTING/REMOVING LIST/ROW ELEMENTS" << endl
                  << "==============================================" << endl;

        bslma_TestAllocator testAllocator;

        if (verbose) cout << "\nCreate a general reference list." << endl;

        bdem_List mL; const bdem_List& L = mL;  ASSERT( 0 == L.length());
        loadReferenceA(&mL);                    ASSERT(32 == L.length());
        loadReferenceB(&mL);                    ASSERT(64 == L.length());
        loadReferenceN(&mL);                    ASSERT(96 == L.length());

        if (veryVerbose) { cout << "\tL = "; L.print(cout, -1, 4); }

        if (verbose) cout << "\nTesting NON-ALIAS case." << endl;
        {
            static const struct {
                int         d_lineNum; // source line number
                const char *d_dSpec;   // initial (destination) list value (dl)
                int         d_di;      // index at which to insert into dl
                const char *d_sSpec;   // source list value (sl)
                int         d_si;      // index at which to insert from sl
                int         d_ne;      // number of elements to insert
                const char *d_eSpec;   // expected final value of dl
            } DATA[] = {
                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 1
                { L_,   "A",     0,  "",      0,  0,  "A"     },
                { L_,   "A",     1,  "",      0,  0,  "A"     },

                { L_,   "",      0,  "A",     0,  0,  ""      },
                { L_,   "",      0,  "A",     0,  1,  "A"     },
                { L_,   "",      0,  "A",     1,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 2
                { L_,   "AB",    0,  "",      0,  0,  "AB"    },
                { L_,   "AB",    1,  "",      0,  0,  "AB"    },
                { L_,   "AB",    2,  "",      0,  0,  "AB"    },

                { L_,   "A",     0,  "B",     0,  0,  "A"     },
                { L_,   "A",     0,  "B",     0,  1,  "BA"    },
                { L_,   "A",     0,  "B",     1,  0,  "A"     },
                { L_,   "A",     1,  "B",     0,  0,  "A"     },
                { L_,   "A",     1,  "B",     0,  1,  "AB"    },
                { L_,   "A",     1,  "B",     1,  0,  "A"     },

                { L_,   "",      0,  "AB",    0,  0,  ""      },
                { L_,   "",      0,  "AB",    0,  1,  "A"     },
                { L_,   "",      0,  "AB",    0,  2,  "AB"    },
                { L_,   "",      0,  "AB",    1,  0,  ""      },
                { L_,   "",      0,  "AB",    1,  1,  "B"     },
                { L_,   "",      0,  "AB",    2,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 3
                { L_,   "ABC",   0,  "",      0,  0,  "ABC"   },
                { L_,   "ABC",   1,  "",      0,  0,  "ABC"   },
                { L_,   "ABC",   2,  "",      0,  0,  "ABC"   },
                { L_,   "ABC",   3,  "",      0,  0,  "ABC"   },

                { L_,   "AB",    0,  "C",     0,  0,  "AB"    },
                { L_,   "AB",    0,  "C",     0,  1,  "CAB"   },
                { L_,   "AB",    0,  "C",     1,  0,  "AB"    },
                { L_,   "AB",    1,  "C",     0,  0,  "AB"    },
                { L_,   "AB",    1,  "C",     0,  1,  "ACB"   },
                { L_,   "AB",    1,  "C",     1,  0,  "AB"    },
                { L_,   "AB",    2,  "C",     0,  0,  "AB"    },
                { L_,   "AB",    2,  "C",     0,  1,  "ABC"   },
                { L_,   "AB",    2,  "C",     1,  0,  "AB"    },

                { L_,   "A",     0,  "BC",    0,  0,  "A"     },
                { L_,   "A",     0,  "BC",    0,  1,  "BA"    },
                { L_,   "A",     0,  "BC",    0,  2,  "BCA"   },
                { L_,   "A",     0,  "BC",    1,  0,  "A"     },
                { L_,   "A",     0,  "BC",    1,  1,  "CA"    },
                { L_,   "A",     0,  "BC",    2,  0,  "A"     },
                { L_,   "A",     1,  "BC",    0,  0,  "A"     },
                { L_,   "A",     1,  "BC",    0,  1,  "AB"    },
                { L_,   "A",     1,  "BC",    0,  2,  "ABC"   },
                { L_,   "A",     1,  "BC",    1,  0,  "A"     },
                { L_,   "A",     1,  "BC",    1,  1,  "AC"    },
                { L_,   "A",     1,  "BC",    2,  0,  "A"     },

                { L_,   "",      0,  "ABC",   0,  0,  ""      },
                { L_,   "",      0,  "ABC",   0,  1,  "A"     },
                { L_,   "",      0,  "ABC",   0,  2,  "AB"    },
                { L_,   "",      0,  "ABC",   0,  3,  "ABC"   },
                { L_,   "",      0,  "ABC",   1,  0,  ""      },
                { L_,   "",      0,  "ABC",   1,  1,  "B"     },
                { L_,   "",      0,  "ABC",   1,  2,  "BC"    },
                { L_,   "",      0,  "ABC",   2,  0,  ""      },
                { L_,   "",      0,  "ABC",   2,  1,  "C"     },
                { L_,   "",      0,  "ABC",   3,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 4
                { L_,   "ABCD",  0,  "",      0,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  "",      0,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  "",      0,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  "",      0,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  "",      0,  0,  "ABCD"  },

                { L_,   "ABC",   0,  "D",     0,  0,  "ABC"   },
                { L_,   "ABC",   0,  "D",     0,  1,  "DABC"  },
                { L_,   "ABC",   0,  "D",     1,  0,  "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  0,  "ABC"   },
                { L_,   "ABC",   1,  "D",     0,  1,  "ADBC"  },
                { L_,   "ABC",   1,  "D",     1,  0,  "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  0,  "ABC"   },
                { L_,   "ABC",   2,  "D",     0,  1,  "ABDC"  },
                { L_,   "ABC",   2,  "D",     1,  0,  "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  0,  "ABC"   },
                { L_,   "ABC",   3,  "D",     0,  1,  "ABCD"  },
                { L_,   "ABC",   3,  "D",     1,  0,  "ABC"   },

                { L_,   "AB",    0,  "CD",    0,  0,  "AB"    },
                { L_,   "AB",    0,  "CD",    0,  1,  "CAB"   },
                { L_,   "AB",    0,  "CD",    0,  2,  "CDAB"  },
                { L_,   "AB",    0,  "CD",    1,  0,  "AB"    },
                { L_,   "AB",    0,  "CD",    1,  1,  "DAB"   },
                { L_,   "AB",    0,  "CD",    2,  0,  "AB"    },
                { L_,   "AB",    1,  "CD",    0,  0,  "AB"    },
                { L_,   "AB",    1,  "CD",    0,  1,  "ACB"   },
                { L_,   "AB",    1,  "CD",    0,  2,  "ACDB"  },
                { L_,   "AB",    1,  "CD",    1,  0,  "AB"    },
                { L_,   "AB",    1,  "CD",    1,  1,  "ADB"   },
                { L_,   "AB",    1,  "CD",    2,  0,  "AB"    },
                { L_,   "AB",    2,  "CD",    0,  0,  "AB"    },
                { L_,   "AB",    2,  "CD",    0,  1,  "ABC"   },
                { L_,   "AB",    2,  "CD",    0,  2,  "ABCD"  },
                { L_,   "AB",    2,  "CD",    1,  0,  "AB"    },
                { L_,   "AB",    2,  "CD",    1,  1,  "ABD"   },
                { L_,   "AB",    2,  "CD",    2,  0,  "AB"    },

                { L_,   "A",     0,  "BCD",   0,  0,  "A"     },
                { L_,   "A",     0,  "BCD",   0,  1,  "BA"    },
                { L_,   "A",     0,  "BCD",   0,  2,  "BCA"   },
                { L_,   "A",     0,  "BCD",   0,  3,  "BCDA"  },
                { L_,   "A",     0,  "BCD",   1,  0,  "A"     },
                { L_,   "A",     0,  "BCD",   1,  1,  "CA"    },
                { L_,   "A",     0,  "BCD",   1,  2,  "CDA"   },
                { L_,   "A",     0,  "BCD",   2,  0,  "A"     },
                { L_,   "A",     0,  "BCD",   2,  1,  "DA"    },
                { L_,   "A",     0,  "BCD",   3,  0,  "A"     },
                { L_,   "A",     1,  "BCD",   0,  0,  "A"     },
                { L_,   "A",     1,  "BCD",   0,  1,  "AB"    },
                { L_,   "A",     1,  "BCD",   0,  2,  "ABC"   },
                { L_,   "A",     1,  "BCD",   0,  3,  "ABCD"  },
                { L_,   "A",     1,  "BCD",   1,  0,  "A"     },
                { L_,   "A",     1,  "BCD",   1,  1,  "AC"    },
                { L_,   "A",     1,  "BCD",   1,  2,  "ACD"   },
                { L_,   "A",     1,  "BCD",   2,  0,  "A"     },
                { L_,   "A",     1,  "BCD",   2,  1,  "AD"    },
                { L_,   "A",     1,  "BCD",   3,  0,  "A"     },

                { L_,   "",      0,  "ABCD",  0,  0,  ""      },
                { L_,   "",      0,  "ABCD",  0,  1,  "A"     },
                { L_,   "",      0,  "ABCD",  0,  2,  "AB"    },
                { L_,   "",      0,  "ABCD",  0,  3,  "ABC"   },
                { L_,   "",      0,  "ABCD",  0,  4,  "ABCD"  },
                { L_,   "",      0,  "ABCD",  1,  0,  ""      },
                { L_,   "",      0,  "ABCD",  1,  1,  "B"     },
                { L_,   "",      0,  "ABCD",  1,  2,  "BC"    },
                { L_,   "",      0,  "ABCD",  1,  3,  "BCD"   },
                { L_,   "",      0,  "ABCD",  2,  0,  ""      },
                { L_,   "",      0,  "ABCD",  2,  1,  "C"     },
                { L_,   "",      0,  "ABCD",  2,  2,  "CD"    },
                { L_,   "",      0,  "ABCD",  3,  0,  ""      },
                { L_,   "",      0,  "ABCD",  3,  1,  "D"     },
                { L_,   "",      0,  "ABCD",  4,  0,  ""      },

                //line  d-object di  s-object si  ne  expected
                //----  -------- --  -------- --  --  --------   Depth = 5
                { L_,   "ABCDE", 0,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 1,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 2,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 3,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 4,  "",      0,  0,  "ABCDE" },
                { L_,   "ABCDE", 5,  "",      0,  0,  "ABCDE" },

                { L_,   "ABCD",  0,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  0,  "E",     0,  1,  "EABCD" },
                { L_,   "ABCD",  0,  "E",     1,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  1,  "E",     0,  1,  "AEBCD" },
                { L_,   "ABCD",  1,  "E",     1,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  2,  "E",     0,  1,  "ABECD" },
                { L_,   "ABCD",  2,  "E",     1,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  3,  "E",     0,  1,  "ABCED" },
                { L_,   "ABCD",  3,  "E",     1,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  0,  "ABCD"  },
                { L_,   "ABCD",  4,  "E",     0,  1,  "ABCDE" },
                { L_,   "ABCD",  4,  "E",     1,  0,  "ABCD"  },

                { L_,   "ABC",   0,  "DE",    0,  0,  "ABC"   },
                { L_,   "ABC",   0,  "DE",    0,  1,  "DABC"  },
                { L_,   "ABC",   0,  "DE",    0,  2,  "DEABC" },
                { L_,   "ABC",   0,  "DE",    1,  0,  "ABC"   },
                { L_,   "ABC",   0,  "DE",    1,  1,  "EABC"  },
                { L_,   "ABC",   0,  "DE",    2,  0,  "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  0,  "ABC"   },
                { L_,   "ABC",   1,  "DE",    0,  1,  "ADBC"  },
                { L_,   "ABC",   1,  "DE",    0,  2,  "ADEBC" },
                { L_,   "ABC",   1,  "DE",    1,  0,  "ABC"   },
                { L_,   "ABC",   1,  "DE",    1,  1,  "AEBC"  },
                { L_,   "ABC",   1,  "DE",    2,  0,  "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  0,  "ABC"   },
                { L_,   "ABC",   2,  "DE",    0,  1,  "ABDC"  },
                { L_,   "ABC",   2,  "DE",    0,  2,  "ABDEC" },
                { L_,   "ABC",   2,  "DE",    1,  0,  "ABC"   },
                { L_,   "ABC",   2,  "DE",    1,  1,  "ABEC"  },
                { L_,   "ABC",   2,  "DE",    2,  0,  "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  0,  "ABC"   },
                { L_,   "ABC",   3,  "DE",    0,  1,  "ABCD"  },
                { L_,   "ABC",   3,  "DE",    0,  2,  "ABCDE" },
                { L_,   "ABC",   3,  "DE",    1,  0,  "ABC"   },
                { L_,   "ABC",   3,  "DE",    1,  1,  "ABCE"  },
                { L_,   "ABC",   3,  "DE",    2,  0,  "ABC"   },

                { L_,   "AB",    0,  "CDE",   0,  0,  "AB"    },
                { L_,   "AB",    0,  "CDE",   0,  1,  "CAB"   },
                { L_,   "AB",    0,  "CDE",   0,  2,  "CDAB"  },
                { L_,   "AB",    0,  "CDE",   0,  3,  "CDEAB" },
                { L_,   "AB",    0,  "CDE",   1,  0,  "AB"    },
                { L_,   "AB",    0,  "CDE",   1,  1,  "DAB"   },
                { L_,   "AB",    0,  "CDE",   1,  2,  "DEAB"  },
                { L_,   "AB",    0,  "CDE",   2,  0,  "AB"    },
                { L_,   "AB",    0,  "CDE",   2,  1,  "EAB"   },
                { L_,   "AB",    0,  "CDE",   3,  0,  "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  0,  "AB"    },
                { L_,   "AB",    1,  "CDE",   0,  1,  "ACB"   },
                { L_,   "AB",    1,  "CDE",   0,  2,  "ACDB"  },
                { L_,   "AB",    1,  "CDE",   0,  3,  "ACDEB" },
                { L_,   "AB",    1,  "CDE",   1,  0,  "AB"    },
                { L_,   "AB",    1,  "CDE",   1,  1,  "ADB"   },
                { L_,   "AB",    1,  "CDE",   1,  2,  "ADEB"  },
                { L_,   "AB",    1,  "CDE",   2,  0,  "AB"    },
                { L_,   "AB",    1,  "CDE",   2,  1,  "AEB"   },
                { L_,   "AB",    1,  "CDE",   3,  0,  "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  0,  "AB"    },
                { L_,   "AB",    2,  "CDE",   0,  1,  "ABC"   },
                { L_,   "AB",    2,  "CDE",   0,  2,  "ABCD"  },
                { L_,   "AB",    2,  "CDE",   0,  3,  "ABCDE" },
                { L_,   "AB",    2,  "CDE",   1,  0,  "AB"    },
                { L_,   "AB",    2,  "CDE",   1,  1,  "ABD"   },
                { L_,   "AB",    2,  "CDE",   1,  2,  "ABDE"  },
                { L_,   "AB",    2,  "CDE",   2,  0,  "AB"    },
                { L_,   "AB",    2,  "CDE",   2,  1,  "ABE"   },
                { L_,   "AB",    2,  "CDE",   3,  0,  "AB"    },

                { L_,   "A",     0,  "BCDE",  0,  0,  "A"     },
                { L_,   "A",     0,  "BCDE",  0,  1,  "BA"    },
                { L_,   "A",     0,  "BCDE",  0,  2,  "BCA"   },
                { L_,   "A",     0,  "BCDE",  0,  3,  "BCDA"  },
                { L_,   "A",     0,  "BCDE",  1,  0,  "A"     },
                { L_,   "A",     0,  "BCDE",  1,  1,  "CA"    },
                { L_,   "A",     0,  "BCDE",  1,  2,  "CDA"   },
                { L_,   "A",     0,  "BCDE",  2,  0,  "A"     },
                { L_,   "A",     0,  "BCDE",  2,  1,  "DA"    },
                { L_,   "A",     0,  "BCDE",  3,  0,  "A"     },
                { L_,   "A",     1,  "BCDE",  0,  0,  "A"     },
                { L_,   "A",     1,  "BCDE",  0,  1,  "AB"    },
                { L_,   "A",     1,  "BCDE",  0,  2,  "ABC"   },
                { L_,   "A",     1,  "BCDE",  0,  3,  "ABCD"  },
                { L_,   "A",     1,  "BCDE",  1,  0,  "A"     },
                { L_,   "A",     1,  "BCDE",  1,  1,  "AC"    },
                { L_,   "A",     1,  "BCDE",  1,  2,  "ACD"   },
                { L_,   "A",     1,  "BCDE",  2,  0,  "A"     },
                { L_,   "A",     1,  "BCDE",  2,  1,  "AD"    },
                { L_,   "A",     1,  "BCDE",  3,  0,  "A"     },

                { L_,   "",      0,  "ABCDE", 0,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 0,  1,  "A"     },
                { L_,   "",      0,  "ABCDE", 0,  2,  "AB"    },
                { L_,   "",      0,  "ABCDE", 0,  3,  "ABC"   },
                { L_,   "",      0,  "ABCDE", 0,  4,  "ABCD"  },
                { L_,   "",      0,  "ABCDE", 0,  5,  "ABCDE" },
                { L_,   "",      0,  "ABCDE", 1,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 1,  1,  "B"     },
                { L_,   "",      0,  "ABCDE", 1,  2,  "BC"    },
                { L_,   "",      0,  "ABCDE", 1,  3,  "BCD"   },
                { L_,   "",      0,  "ABCDE", 1,  4,  "BCDE"  },
                { L_,   "",      0,  "ABCDE", 2,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 2,  1,  "C"     },
                { L_,   "",      0,  "ABCDE", 2,  2,  "CD"    },
                { L_,   "",      0,  "ABCDE", 2,  3,  "CDE"   },
                { L_,   "",      0,  "ABCDE", 3,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 3,  1,  "D"     },
                { L_,   "",      0,  "ABCDE", 3,  2,  "DE"    },
                { L_,   "",      0,  "ABCDE", 4,  0,  ""      },
                { L_,   "",      0,  "ABCDE", 4,  1,  "E"     },
                { L_,   "",      0,  "ABCDE", 5,  0,  ""      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bdem_ElemType::Type OFFSETS[] = {
                bdem_ElemType::BDEM_INT,
                bdem_ElemType::BDEM_STRING,
                bdem_ElemType::BDEM_INT_ARRAY,
                bdem_ElemType::BDEM_STRING_ARRAY,
                bdem_ElemType::BDEM_LIST,

            };
            const int NUM_OFFSETS = sizeof OFFSETS / sizeof *OFFSETS;

            const bdem_AggregateOption::AllocationStrategy MODES[] = {
                bdem_AggregateOption::BDEM_PASS_THROUGH,
                bdem_AggregateOption::BDEM_WRITE_ONCE,
                bdem_AggregateOption::BDEM_WRITE_MANY,
            };
            const int NUM_MODES = sizeof MODES / sizeof *MODES;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_sSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_eSpec;

                const int   DEPTH  = strlen(D_SPEC) + strlen(S_SPEC);
                if (DEPTH != oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                for (int ri = 0; ri < NUM_OFFSETS; ++ri) { // START POSITION
                    bdem_List reference;
                    {
                        for (int i = 0; i < 5; ++i) {   // LOAD FIVE EXEMPLARS
                            reference.appendElement(L[OFFSETS[ri] + i]);
                        }
                    }
                    const bdem_List& REFERENCE = reference;

                    Obj DD(g(D_SPEC, REFERENCE));   // control for destination
                    Obj SS(g(S_SPEC, REFERENCE));   // control for source
                    Obj EE(g(E_SPEC, REFERENCE));   // control for expected val
                    Obj NN(g(S_SPEC, REFERENCE));   // null control for source
                    NN.makeAllNull();

                    if (veryVerbose && !ri || veryVeryVerbose) {
                        cout << "\t  =================================="
                                    "=================================="
                        << endl;

                        cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                    P_(SI); P_(NE); P(E_SPEC);
                        cout << "\t  "; P_(ri); P(OFFSETS[ri]);
                        cout << "\t\t"; P(DD);
                        cout << "\t\t"; P(SS);
                        cout << "\t\t"; P(EE);
                    }

                    for (int ai = 0; ai < NUM_MODES; ++ai) { // ALLOCATION MODE

                        const bool PRINT = veryVerbose && !ri && !ai
                                        || veryVeryVerbose;

                        const bdem_AggregateOption::AllocationStrategy
                                                              MODE = MODES[ai];
                        if (PRINT) {
                            cout << "\t  ----------------------------------"
                                        "----------------------------------"
                            << endl;
                            cout << "\t  "; P(MODE);
                        }
 // v-------------------^
    if (DI == DD.length() && NE == SS.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_Row& srcElements);" << endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendElements(S.row());
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (DI == DD.length() && NE == SS.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_List& srcElements);" << endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, MODE, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendElements(S);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (DI == DD.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
           "\t\tappendElements(const bdem_Row& se, int si, int ne);" << endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, MODE, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendElements(S.row(), SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (DI == DD.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_List& se, int si, int ne);" << endl;

        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, MODE, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.appendElements(S, SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
           "\t\tinsertElement(int di, const bdem_ConstElemRef& srcElement);"
                        << endl;
        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, MODE, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElement(DI, S.row()[SI]);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
            mX.insertElement(DI, NN.row()[SI]);
            if (PRINT) { cout << "\t\t\t AFTER NULL: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, X[DI].isNull());
            mX.removeElement(DI);
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (NE == SS.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertElements(int dstIndex, const bdem_Row& srcElements);"
                        << endl;
        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, MODE, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElements(DI, S.row());
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (NE == SS.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertElements(int dstIndex, const bdem_List& srcElements);"
                        << endl;
        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, MODE, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElements(DI, S);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertElements(int di, const bdem_Row&  sr, int si, int ne);"
                        << endl;
        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, MODE, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElements(DI, S.row(), SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertElements(int di, const bdem_List& sl, int si, int ne);"
                        << endl;
        Obj mX(DD, MODE, &testAllocator);  const Obj &X = mX;
        {
            Obj mS(SS, MODE, &testAllocator);  const Obj &S = mS;
            if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
            mX.insertElements(DI, S, SI, NE);
            if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
            LOOP3_ASSERT(LINE, ri, ai, EE == X);
            LOOP3_ASSERT(LINE, ri, ai, SS == S);
        }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);  // source is out of scope

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout << "\t\tremoveElement(int index);" << endl;

        Obj mX(EE, MODE, &testAllocator);  const Obj &X = mX;

        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.removeElement(DI);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, DD == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tremoveElements(int startIndex, int numElements);" << endl;

        Obj mX(EE, MODE, &testAllocator);  const Obj &X = mX;

        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.removeElements(DI, NE);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, DD == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }
 // ^-------------------v
                    } // end for each allocation mode
                } // end for each exemplar sequence
            } // end for each test vector
        } // end block scope

        //---------------------------------------------------------------------

        if (verbose) cout << "\nTesting ALIAS case." << endl;
        {
            static const struct {
                int         d_lineNum; // source line number
                const char *d_xSpec;   // initial list value (xl)
                int         d_di;      // index at which to insert into xl
                int         d_si;      // index at which to insert from xl
                int         d_ne;      // number of elements to insert
                const char *d_eSpec;   // expected final value of dl

            } DATA[] = {
                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 0
                { L_,   "",       0,  0,  0,  ""           },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 1
                { L_,   "A",      0,  0,  0,  "A"          },
                { L_,   "A",      0,  0,  1,  "AA"         },
                { L_,   "A",      0,  1,  0,  "A"          },

                { L_,   "A",      1,  0,  0,  "A"          },
                { L_,   "A",      1,  0,  1,  "AA"         },
                { L_,   "A",      1,  1,  0,  "A"          },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 2
                { L_,   "AB",     0,  0,  0,  "AB"         },
                { L_,   "AB",     0,  0,  1,  "AAB"        },
                { L_,   "AB",     0,  0,  2,  "ABAB"       },
                { L_,   "AB",     0,  1,  0,  "AB"         },
                { L_,   "AB",     0,  1,  1,  "BAB"        },
                { L_,   "AB",     0,  2,  0,  "AB"         },

                { L_,   "AB",     1,  0,  0,  "AB"         },
                { L_,   "AB",     1,  0,  1,  "AAB"        },
                { L_,   "AB",     1,  0,  2,  "AABB"       },
                { L_,   "AB",     1,  1,  0,  "AB"         },
                { L_,   "AB",     1,  1,  1,  "ABB"        },
                { L_,   "AB",     1,  2,  0,  "AB"         },

                { L_,   "AB",     2,  0,  0,  "AB"         },
                { L_,   "AB",     2,  0,  1,  "ABA"        },
                { L_,   "AB",     2,  0,  2,  "ABAB"       },
                { L_,   "AB",     2,  1,  0,  "AB"         },
                { L_,   "AB",     2,  1,  1,  "ABB"        },
                { L_,   "AB",     2,  2,  0,  "AB"         },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 3
                { L_,   "ABC",    0,  0,  0,  "ABC"        },
                { L_,   "ABC",    0,  0,  1,  "AABC"       },
                { L_,   "ABC",    0,  0,  2,  "ABABC"      },
                { L_,   "ABC",    0,  0,  3,  "ABCABC"     },
                { L_,   "ABC",    0,  1,  0,  "ABC"        },
                { L_,   "ABC",    0,  1,  1,  "BABC"       },
                { L_,   "ABC",    0,  1,  2,  "BCABC"      },
                { L_,   "ABC",    0,  2,  0,  "ABC"        },
                { L_,   "ABC",    0,  2,  1,  "CABC"       },
                { L_,   "ABC",    0,  3,  0,  "ABC"        },

                { L_,   "ABC",    1,  0,  0,  "ABC"        },
                { L_,   "ABC",    1,  0,  1,  "AABC"       },
                { L_,   "ABC",    1,  0,  2,  "AABBC"      },
                { L_,   "ABC",    1,  0,  3,  "AABCBC"     },
                { L_,   "ABC",    1,  1,  0,  "ABC"        },
                { L_,   "ABC",    1,  1,  1,  "ABBC"       },
                { L_,   "ABC",    1,  1,  2,  "ABCBC"      },
                { L_,   "ABC",    1,  2,  0,  "ABC"        },
                { L_,   "ABC",    1,  2,  1,  "ACBC"       },
                { L_,   "ABC",    1,  3,  0,  "ABC"        },

                { L_,   "ABC",    2,  0,  0,  "ABC"        },
                { L_,   "ABC",    2,  0,  1,  "ABAC"       },
                { L_,   "ABC",    2,  0,  2,  "ABABC"      },
                { L_,   "ABC",    2,  0,  3,  "ABABCC"     },
                { L_,   "ABC",    2,  1,  0,  "ABC"        },
                { L_,   "ABC",    2,  1,  1,  "ABBC"       },
                { L_,   "ABC",    2,  1,  2,  "ABBCC"      },
                { L_,   "ABC",    2,  2,  0,  "ABC"        },
                { L_,   "ABC",    2,  2,  1,  "ABCC"       },
                { L_,   "ABC",    2,  3,  0,  "ABC"        },

                { L_,   "ABC",    3,  0,  0,  "ABC"        },
                { L_,   "ABC",    3,  0,  1,  "ABCA"       },
                { L_,   "ABC",    3,  0,  2,  "ABCAB"      },
                { L_,   "ABC",    3,  0,  3,  "ABCABC"     },
                { L_,   "ABC",    3,  1,  0,  "ABC"        },
                { L_,   "ABC",    3,  1,  1,  "ABCB"       },
                { L_,   "ABC",    3,  1,  2,  "ABCBC"      },
                { L_,   "ABC",    3,  2,  0,  "ABC"        },
                { L_,   "ABC",    3,  2,  1,  "ABCC"       },
                { L_,   "ABC",    3,  3,  0,  "ABC"        },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 4
                { L_,   "ABCD",   0,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   0,  0,  1,  "AABCD"      },
                { L_,   "ABCD",   0,  0,  2,  "ABABCD"     },
                { L_,   "ABCD",   0,  0,  3,  "ABCABCD"    },
                { L_,   "ABCD",   0,  0,  4,  "ABCDABCD"   },
                { L_,   "ABCD",   0,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   0,  1,  1,  "BABCD"      },
                { L_,   "ABCD",   0,  1,  2,  "BCABCD"     },
                { L_,   "ABCD",   0,  1,  3,  "BCDABCD"    },
                { L_,   "ABCD",   0,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   0,  2,  1,  "CABCD"      },
                { L_,   "ABCD",   0,  2,  2,  "CDABCD"     },
                { L_,   "ABCD",   0,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   0,  3,  1,  "DABCD"      },
                { L_,   "ABCD",   0,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   1,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   1,  0,  1,  "AABCD"      },
                { L_,   "ABCD",   1,  0,  2,  "AABBCD"     },
                { L_,   "ABCD",   1,  0,  3,  "AABCBCD"    },
                { L_,   "ABCD",   1,  0,  4,  "AABCDBCD"   },
                { L_,   "ABCD",   1,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   1,  1,  1,  "ABBCD"      },
                { L_,   "ABCD",   1,  1,  2,  "ABCBCD"     },
                { L_,   "ABCD",   1,  1,  3,  "ABCDBCD"    },
                { L_,   "ABCD",   1,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   1,  2,  1,  "ACBCD"      },
                { L_,   "ABCD",   1,  2,  2,  "ACDBCD"     },
                { L_,   "ABCD",   1,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   1,  3,  1,  "ADBCD"      },
                { L_,   "ABCD",   1,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   2,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   2,  0,  1,  "ABACD"      },
                { L_,   "ABCD",   2,  0,  2,  "ABABCD"     },
                { L_,   "ABCD",   2,  0,  3,  "ABABCCD"    },
                { L_,   "ABCD",   2,  0,  4,  "ABABCDCD"   },
                { L_,   "ABCD",   2,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   2,  1,  1,  "ABBCD"      },
                { L_,   "ABCD",   2,  1,  2,  "ABBCCD"     },
                { L_,   "ABCD",   2,  1,  3,  "ABBCDCD"    },
                { L_,   "ABCD",   2,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   2,  2,  1,  "ABCCD"      },
                { L_,   "ABCD",   2,  2,  2,  "ABCDCD"     },
                { L_,   "ABCD",   2,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   2,  3,  1,  "ABDCD"      },
                { L_,   "ABCD",   2,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   3,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   3,  0,  1,  "ABCAD"      },
                { L_,   "ABCD",   3,  0,  2,  "ABCABD"     },
                { L_,   "ABCD",   3,  0,  3,  "ABCABCD"    },
                { L_,   "ABCD",   3,  0,  4,  "ABCABCDD"   },
                { L_,   "ABCD",   3,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   3,  1,  1,  "ABCBD"      },
                { L_,   "ABCD",   3,  1,  2,  "ABCBCD"     },
                { L_,   "ABCD",   3,  1,  3,  "ABCBCDD"    },
                { L_,   "ABCD",   3,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   3,  2,  1,  "ABCCD"      },
                { L_,   "ABCD",   3,  2,  2,  "ABCCDD"     },
                { L_,   "ABCD",   3,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   3,  3,  1,  "ABCDD"      },
                { L_,   "ABCD",   3,  4,  0,  "ABCD"       },

                { L_,   "ABCD",   4,  0,  0,  "ABCD"       },
                { L_,   "ABCD",   4,  0,  1,  "ABCDA"      },
                { L_,   "ABCD",   4,  0,  2,  "ABCDAB"     },
                { L_,   "ABCD",   4,  0,  3,  "ABCDABC"    },
                { L_,   "ABCD",   4,  0,  4,  "ABCDABCD"   },
                { L_,   "ABCD",   4,  1,  0,  "ABCD"       },
                { L_,   "ABCD",   4,  1,  1,  "ABCDB"      },
                { L_,   "ABCD",   4,  1,  2,  "ABCDBC"     },
                { L_,   "ABCD",   4,  1,  3,  "ABCDBCD"    },
                { L_,   "ABCD",   4,  2,  0,  "ABCD"       },
                { L_,   "ABCD",   4,  2,  1,  "ABCDC"      },
                { L_,   "ABCD",   4,  2,  2,  "ABCDCD"     },
                { L_,   "ABCD",   4,  3,  0,  "ABCD"       },
                { L_,   "ABCD",   4,  3,  1,  "ABCDD"      },
                { L_,   "ABCD",   4,  4,  0,  "ABCD"       },

                //line  x-object  di  si  ne  expected
                //----  --------  --  --  --  --------        Depth = 5
                { L_,   "ABCDE",  0,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  0,  1,  "AABCDE"     },
                { L_,   "ABCDE",  0,  0,  2,  "ABABCDE"    },
                { L_,   "ABCDE",  0,  0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE",  0,  0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE",  0,  0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE",  0,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  1,  1,  "BABCDE"     },
                { L_,   "ABCDE",  0,  1,  2,  "BCABCDE"    },
                { L_,   "ABCDE",  0,  1,  3,  "BCDABCDE"   },
                { L_,   "ABCDE",  0,  1,  4,  "BCDEABCDE"  },
                { L_,   "ABCDE",  0,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  2,  1,  "CABCDE"     },
                { L_,   "ABCDE",  0,  2,  2,  "CDABCDE"    },
                { L_,   "ABCDE",  0,  2,  3,  "CDEABCDE"   },
                { L_,   "ABCDE",  0,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  3,  1,  "DABCDE"     },
                { L_,   "ABCDE",  0,  3,  2,  "DEABCDE"    },
                { L_,   "ABCDE",  0,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  0,  4,  1,  "EABCDE"     },
                { L_,   "ABCDE",  0,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  1,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  0,  1,  "AABCDE"     },
                { L_,   "ABCDE",  1,  0,  2,  "AABBCDE"    },
                { L_,   "ABCDE",  1,  0,  3,  "AABCBCDE"   },
                { L_,   "ABCDE",  1,  0,  4,  "AABCDBCDE"  },
                { L_,   "ABCDE",  1,  0,  5,  "AABCDEBCDE" },
                { L_,   "ABCDE",  1,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  1,  1,  "ABBCDE"     },
                { L_,   "ABCDE",  1,  1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE",  1,  1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE",  1,  1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE",  1,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  2,  1,  "ACBCDE"     },
                { L_,   "ABCDE",  1,  2,  2,  "ACDBCDE"    },
                { L_,   "ABCDE",  1,  2,  3,  "ACDEBCDE"   },
                { L_,   "ABCDE",  1,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  3,  1,  "ADBCDE"     },
                { L_,   "ABCDE",  1,  3,  2,  "ADEBCDE"    },
                { L_,   "ABCDE",  1,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  1,  4,  1,  "AEBCDE"     },
                { L_,   "ABCDE",  1,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  2,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  0,  1,  "ABACDE"     },
                { L_,   "ABCDE",  2,  0,  2,  "ABABCDE"    },
                { L_,   "ABCDE",  2,  0,  3,  "ABABCCDE"   },
                { L_,   "ABCDE",  2,  0,  4,  "ABABCDCDE"  },
                { L_,   "ABCDE",  2,  0,  5,  "ABABCDECDE" },
                { L_,   "ABCDE",  2,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  1,  1,  "ABBCDE"     },
                { L_,   "ABCDE",  2,  1,  2,  "ABBCCDE"    },
                { L_,   "ABCDE",  2,  1,  3,  "ABBCDCDE"   },
                { L_,   "ABCDE",  2,  1,  4,  "ABBCDECDE"  },
                { L_,   "ABCDE",  2,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  2,  1,  "ABCCDE"     },
                { L_,   "ABCDE",  2,  2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE",  2,  2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE",  2,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  3,  1,  "ABDCDE"     },
                { L_,   "ABCDE",  2,  3,  2,  "ABDECDE"    },
                { L_,   "ABCDE",  2,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  2,  4,  1,  "ABECDE"     },
                { L_,   "ABCDE",  2,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  3,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  0,  1,  "ABCADE"     },
                { L_,   "ABCDE",  3,  0,  2,  "ABCABDE"    },
                { L_,   "ABCDE",  3,  0,  3,  "ABCABCDE"   },
                { L_,   "ABCDE",  3,  0,  4,  "ABCABCDDE"  },
                { L_,   "ABCDE",  3,  0,  5,  "ABCABCDEDE" },
                { L_,   "ABCDE",  3,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  1,  1,  "ABCBDE"     },
                { L_,   "ABCDE",  3,  1,  2,  "ABCBCDE"    },
                { L_,   "ABCDE",  3,  1,  3,  "ABCBCDDE"   },
                { L_,   "ABCDE",  3,  1,  4,  "ABCBCDEDE"  },
                { L_,   "ABCDE",  3,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  2,  1,  "ABCCDE"     },
                { L_,   "ABCDE",  3,  2,  2,  "ABCCDDE"    },
                { L_,   "ABCDE",  3,  2,  3,  "ABCCDEDE"   },
                { L_,   "ABCDE",  3,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  3,  1,  "ABCDDE"     },
                { L_,   "ABCDE",  3,  3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE",  3,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  3,  4,  1,  "ABCEDE"     },
                { L_,   "ABCDE",  3,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  4,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  0,  1,  "ABCDAE"     },
                { L_,   "ABCDE",  4,  0,  2,  "ABCDABE"    },
                { L_,   "ABCDE",  4,  0,  3,  "ABCDABCE"   },
                { L_,   "ABCDE",  4,  0,  4,  "ABCDABCDE"  },
                { L_,   "ABCDE",  4,  0,  5,  "ABCDABCDEE" },
                { L_,   "ABCDE",  4,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  1,  1,  "ABCDBE"     },
                { L_,   "ABCDE",  4,  1,  2,  "ABCDBCE"    },
                { L_,   "ABCDE",  4,  1,  3,  "ABCDBCDE"   },
                { L_,   "ABCDE",  4,  1,  4,  "ABCDBCDEE"  },
                { L_,   "ABCDE",  4,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  2,  1,  "ABCDCE"     },
                { L_,   "ABCDE",  4,  2,  2,  "ABCDCDE"    },
                { L_,   "ABCDE",  4,  2,  3,  "ABCDCDEE"   },
                { L_,   "ABCDE",  4,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  3,  1,  "ABCDDE"     },
                { L_,   "ABCDE",  4,  3,  2,  "ABCDDEE"    },
                { L_,   "ABCDE",  4,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  4,  4,  1,  "ABCDEE"     },
                { L_,   "ABCDE",  4,  5,  0,  "ABCDE"      },

                { L_,   "ABCDE",  5,  0,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  0,  1,  "ABCDEA"     },
                { L_,   "ABCDE",  5,  0,  2,  "ABCDEAB"    },
                { L_,   "ABCDE",  5,  0,  3,  "ABCDEABC"   },
                { L_,   "ABCDE",  5,  0,  4,  "ABCDEABCD"  },
                { L_,   "ABCDE",  5,  0,  5,  "ABCDEABCDE" },
                { L_,   "ABCDE",  5,  1,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  1,  1,  "ABCDEB"     },
                { L_,   "ABCDE",  5,  1,  2,  "ABCDEBC"    },
                { L_,   "ABCDE",  5,  1,  3,  "ABCDEBCD"   },
                { L_,   "ABCDE",  5,  1,  4,  "ABCDEBCDE"  },
                { L_,   "ABCDE",  5,  2,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  2,  1,  "ABCDEC"     },
                { L_,   "ABCDE",  5,  2,  2,  "ABCDECD"    },
                { L_,   "ABCDE",  5,  2,  3,  "ABCDECDE"   },
                { L_,   "ABCDE",  5,  3,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  3,  1,  "ABCDED"     },
                { L_,   "ABCDE",  5,  3,  2,  "ABCDEDE"    },
                { L_,   "ABCDE",  5,  4,  0,  "ABCDE"      },
                { L_,   "ABCDE",  5,  4,  1,  "ABCDEE"     },
                { L_,   "ABCDE",  5,  5,  0,  "ABCDE"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bdem_ElemType::Type OFFSETS[] = {
                bdem_ElemType::BDEM_INT,
                bdem_ElemType::BDEM_STRING,
                bdem_ElemType::BDEM_INT_ARRAY,
                bdem_ElemType::BDEM_STRING_ARRAY,
                bdem_ElemType::BDEM_LIST,

            };
            const int NUM_OFFSETS = sizeof OFFSETS / sizeof *OFFSETS;

            const bdem_AggregateOption::AllocationStrategy MODES[] = {
                bdem_AggregateOption::BDEM_PASS_THROUGH,
                bdem_AggregateOption::BDEM_WRITE_ONCE,
                bdem_AggregateOption::BDEM_WRITE_MANY,
            };
            const int NUM_MODES = sizeof MODES / sizeof *MODES;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_eSpec;

                const int   DEPTH  = (int)strlen(X_SPEC);
                if (DEPTH != oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                for (int ri = 0; ri < NUM_OFFSETS; ++ri) { // START POSITION
                    bdem_List reference;
                    {
                        for (int i = 0; i < 5; ++i) {   // LOAD FIVE EXEMPLARS
                            reference.appendElement(L[OFFSETS[ri] + i]);
                        }
                    }
                    const bdem_List& REFERENCE = reference;

                    Obj XX(g(X_SPEC, REFERENCE));  // control for initial value
                    Obj EE(g(E_SPEC, REFERENCE));  // control for expected val

                    if (veryVerbose && !ri || veryVeryVerbose) {
                        cout << "\t  =================================="
                                    "=================================="
                        << endl;

                        cout << "\t  "; P_(X_SPEC); P_(DI);
                                                    P_(SI); P_(NE); P(E_SPEC);
                        cout << "\t  "; P_(ri); P(OFFSETS[ri]);
                        cout << "\t\t"; P(XX);
                        cout << "\t\t"; P(EE);
                    }

                    for (int ai = 0; ai < NUM_MODES; ++ai) { // ALLOCATION MODE

                        const bool PRINT = veryVerbose && !ri && !ai
                                        || veryVeryVerbose;

                        const bdem_AggregateOption::AllocationStrategy
                                                              MODE = MODES[ai];
                        if (PRINT) {
                            cout << "\t  ----------------------------------"
                                        "----------------------------------"
                            << endl;
                            cout << "\t  "; P(MODE);
                        }
 // v-------------------^
    if (DI == XX.length() && NE == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_Row& srcElements);" << endl;

        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.appendElements(X.row());
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (DI == XX.length() && NE == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_List& srcElements);" << endl;

        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.appendElements(X);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (DI == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
           "\t\tappendElements(const bdem_Row& se, int si, int ne);" << endl;

        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.appendElements(X.row(), SI, NE);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (DI == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tappendElements(const bdem_List& se, int si, int ne);" << endl;

        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.appendElements(X, SI, NE);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (1 == NE)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
           "\t\tinsertElement(int di, const bdem_ConstElemRef& srcElement);"
                        << endl;
        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.insertElement(DI, X.row()[SI]);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (NE == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertElements(int dstIndex, const bdem_Row& srcElements);"
                        << endl;
        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.insertElements(DI, X.row());
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP5_ASSERT(LINE, ri, ai, EE, X, EE == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (NE == XX.length())
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertElements(int dstIndex, const bdem_List& srcElements);"
                        << endl;
        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.insertElements(DI, X);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertElements(int di, const bdem_Row&  sr, int si, int ne);"
                        << endl;
        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.insertElements(DI, X.row(), SI, NE);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }

    if (1)
    {
        bslma_TestAllocator da; const bslma_DefaultAllocatorGuard dag(&da);

        if (PRINT) cout <<
            "\t\tinsertElements(int di, const bdem_List& sl, int si, int ne);"
                        << endl;
        Obj mX(XX, MODE, &testAllocator);  const Obj &X = mX;
        if (PRINT) { cout << "\t\t\tBEFORE: "; P(X); }
        mX.insertElements(DI, X, SI, NE);
        if (PRINT) { cout << "\t\t\t AFTER: "; P(X); }
        LOOP3_ASSERT(LINE, ri, ai, EE == X);

//         LOOP3_ASSERT(LINE, ri, ai, 0 == da.numBlocksTotal());
    }
 // ^-------------------v
                    } // end for each allocation mode
                } // end for each exemplar sequence
            } // end for each test vector
        } // end block scope

        //---------------------------------------------------------------------

        if (verbose) cout << "\nremoveAll();" << endl;
        {
            ASSERT(96 == L.length());
            {
                if (verbose) cout << "\tBDEM_PASS_THROUGH" << endl;
                bslma_TestAllocator da; bslma_DefaultAllocatorGuard dag(&da);
                bdem_List mX(L, bdem_AggregateOption::BDEM_PASS_THROUGH);
                ASSERT(96 == mX.length());
                mX.removeAll();
                ASSERT( 0 == mX.length());
            }
            {
                if (verbose) cout << "\tBDEM_WRITE_ONCE" << endl;
                bslma_TestAllocator da; bslma_DefaultAllocatorGuard dag(&da);
                bdem_List mX(L, bdem_AggregateOption::BDEM_WRITE_ONCE);
                ASSERT(96 == mX.length());
                mX.removeAll();
                ASSERT( 0 == mX.length());
            }
            {
                if (verbose) cout << "\tBDEM_WRITE_MANY" << endl;
                bslma_TestAllocator da; bslma_DefaultAllocatorGuard dag(&da);
                bdem_List mX(L, bdem_AggregateOption::BDEM_WRITE_MANY);
                ASSERT(96 == mX.length());
                mX.removeAll();
                ASSERT( 0 == mX.length());
            }
        }
}

static void testCase15(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // INSERT ELEMENTS OF EXPLICIT TYPE
        //   Make sure all 32 insertTYPE functions work as expected.
        //
        // Concerns:
        //   - That an element of the specified type is inserted in the
        //      appropriate location.
        //   - That the specialized append and insert methods (taking
        //      'const char *' and 'bdem_Row') work the same as the normal
        //      methods taking 'bsl::string' and 'bdem_List'.
        //
        // Plan:
        //   - Create a reference table of all 32 types by inserting
        //      non-null elements in reverse order.
        //   - Create a second table of all 32 types by appending elements
        //      in forward order
        //   - Create a special table consisting of just strings and lists;
        //      the compare with an equivalent table created from the
        //      corresponding 'const char *' and 'bdem_Row'.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   void insertChar(int dstIndex, char value);
        //   void insertShort(int dstIndex, short value);
        //   void insertInt(int dstIndex, int value);
        //   void insertInt64(int dstIndex, Int64 value);
        //   void insertFloat(int dstIndex, float value);
        //   void insertDouble(int dstIndex, double value);
        //   void insertString(int dstIndex, const bsl::string& value);
        //   void insertDatetime(int dstIndex, const bdet_Datetime& value);
        //   void insertDate(int dstIndex, const bdet_Date& value);
        //   void insertTime(int dstIndex, const bdet_Time& value);
        //   void insertCharArray(int dstIndex, const bsl::vector<char>& val);
        //   void insertShortArray(int dstIndex, const bsl::vector<short>& v);
        //   void insertIntArray(int dstIndex, const bsl::vector<int>& value);
        //   void insertInt64Array(int dstIndex, const bsl::vector<Int64>& v);
        //   void insertFloatArray(int dstIndex);
        //   void insertDoubleArray(int dstIndex, const bsl::vector<double>&v);
        //   void insertStringArray(int di, const bsl::vector<bsl::string>& v);
        //   void insertDatetimeArray(int dstIndex);
        //   void insertDateArray(int di, const bsl::vector<bdet_Date>& value);
        //   void insertTimeArray(int di, const bsl::vector<bdet_Time>& value);
        //   void insertList(int dstIndex, const bdem_List& value);
        //   void insertTable(int dstIndex, const bdem_Table& value);
        //   void insertChoice(int dstIndex, const bdem_Choice& value);
        //   void insertChoiceArray(int dstIndex, const bdem_ChoiceArray&);
        //
        //   void appendString(const char *value);
        //   void insertString(int dstIndex, const char *value);
        //
        //   void appendList(const bdem_Row& value);
        //   void insertList(int dstIndex, const bdem_Row& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INSERT ELEMENTS OF EXPLICIT TYPE" << endl
                          << "================================" << endl;

        if (verbose) cout <<
            "\nCreate a list of unset elements using 'insertTYPE'."<<endl;

        bdem_List mL; const bdem_List& L = mL;  ASSERT( 0 == L.length());

        mL.insertChoiceArray(0, A31);           ASSERT( 1 == L.length());
        mL.insertChoice(0, A30);                ASSERT( 2 == L.length());

        mL.insertTimeTzArray(0, A29);           ASSERT( 3 == L.length());
        mL.insertDateTzArray(0, A28);           ASSERT( 4 == L.length());
        mL.insertDatetimeTzArray(0, A27);       ASSERT( 5 == L.length());
        mL.insertBoolArray(0, A26);             ASSERT( 6 == L.length());
        mL.insertTimeTz(0, A25);                ASSERT( 7 == L.length());
        mL.insertDateTz(0, A24);                ASSERT( 8 == L.length());
        mL.insertDatetimeTz(0, A23);            ASSERT( 9 == L.length());
        mL.insertBool(0, A22);                  ASSERT(10 == L.length());

        mL.insertTable(0, A21);                 ASSERT(11 == L.length());
        mL.insertList(0, A20);                  ASSERT(12 == L.length());
        mL.insertTimeArray(0, A19);             ASSERT(13 == L.length());
        mL.insertDateArray(0, A18);             ASSERT(14 == L.length());
        mL.insertDatetimeArray(0, A17);         ASSERT(15 == L.length());
        mL.insertStringArray(0, A16);           ASSERT(16 == L.length());
        mL.insertDoubleArray(0, A15);           ASSERT(17 == L.length());
        mL.insertFloatArray(0, A14);            ASSERT(18 == L.length());
        mL.insertInt64Array(0, A13);            ASSERT(19 == L.length());
        mL.insertIntArray(0, A12);              ASSERT(20 == L.length());
        mL.insertShortArray(0, A11);            ASSERT(21 == L.length());
        mL.insertCharArray(0, A10);             ASSERT(22 == L.length());
        mL.insertTime(0, A09);                  ASSERT(23 == L.length());
        mL.insertDate(0, A08);                  ASSERT(24 == L.length());
        mL.insertDatetime(0, A07);              ASSERT(25 == L.length());
        mL.insertString(0, A06);                ASSERT(26 == L.length());
        mL.insertDouble(0, A05);                ASSERT(27 == L.length());
        mL.insertFloat(0, A04);                 ASSERT(28 == L.length());
        mL.insertInt64(0, A03);                 ASSERT(29 == L.length());
        mL.insertInt(0, A02);                   ASSERT(30 == L.length());
        mL.insertShort(0, A01);                 ASSERT(31 == L.length());
        mL.insertChar(0, A00);                  ASSERT(32 == L.length());

        ASSERT(A00 == L[0].theChar());
        ASSERT(A01 == L[1].theShort());
        ASSERT(A02 == L[2].theInt());
        ASSERT(A03 == L[3].theInt64());
        ASSERT(A04 == L[4].theFloat());
        ASSERT(A05 == L[5].theDouble());
        ASSERT(A06 == L[6].theString());
        ASSERT(A07 == L[7].theDatetime());
        ASSERT(A08 == L[8].theDate());
        ASSERT(A09 == L[9].theTime());

        ASSERT(A10 == L[10].theCharArray());
        ASSERT(A11 == L[11].theShortArray());
        ASSERT(A12 == L[12].theIntArray());
        ASSERT(A13 == L[13].theInt64Array());
        ASSERT(A14 == L[14].theFloatArray());
        ASSERT(A15 == L[15].theDoubleArray());
        ASSERT(A16 == L[16].theStringArray());
        ASSERT(A17 == L[17].theDatetimeArray());
        ASSERT(A18 == L[18].theDateArray());
        ASSERT(A19 == L[19].theTimeArray());

        ASSERT(A20 == L[20].theList());
        ASSERT(A21 == L[21].theTable());

        ASSERT(A22 == L[22].theBool());
        ASSERT(A23 == L[23].theDatetimeTz());
        ASSERT(A24 == L[24].theDateTz());
        ASSERT(A25 == L[25].theTimeTz());
        ASSERT(A26 == L[26].theBoolArray());
        ASSERT(A27 == L[27].theDatetimeTzArray());
        ASSERT(A28 == L[28].theDateTzArray());
        ASSERT(A29 == L[29].theTimeTzArray());

        ASSERT(A30 == L[30].theChoice());
        ASSERT(A31 == L[31].theChoiceArray());

        if (verbose) cout <<
            "\nCreate a second list (appending) using 'insertTYPE'." << endl;

        bdem_List mL2; const bdem_List& L2 = mL2;  ASSERT( 0 == L2.length());

        mL2.insertChar(0, A00);                    ASSERT( 1 == L2.length());
        mL2.insertShort(1, A01);                   ASSERT( 2 == L2.length());
        mL2.insertInt(2, A02);                     ASSERT( 3 == L2.length());
        mL2.insertInt64(3, A03);                   ASSERT( 4 == L2.length());
        mL2.insertFloat(4, A04);                   ASSERT( 5 == L2.length());
        mL2.insertDouble(5, A05);                  ASSERT( 6 == L2.length());
        mL2.insertString(6, A06);                  ASSERT( 7 == L2.length());
        mL2.insertDatetime(7, A07);                ASSERT( 8 == L2.length());
        mL2.insertDate(8, A08);                    ASSERT( 9 == L2.length());
        mL2.insertTime(9, A09);                    ASSERT(10 == L2.length());

        mL2.insertCharArray(10, A10);              ASSERT(11 == L2.length());
        mL2.insertShortArray(11, A11);             ASSERT(12 == L2.length());
        mL2.insertIntArray(12, A12);               ASSERT(13 == L2.length());
        mL2.insertInt64Array(13, A13);             ASSERT(14 == L2.length());
        mL2.insertFloatArray(14, A14);             ASSERT(15 == L2.length());
        mL2.insertDoubleArray(15, A15);            ASSERT(16 == L2.length());
        mL2.insertStringArray(16, A16);            ASSERT(17 == L2.length());
        mL2.insertDatetimeArray(17, A17);          ASSERT(18 == L2.length());
        mL2.insertDateArray(18, A18);              ASSERT(19 == L2.length());
        mL2.insertTimeArray(19, A19);              ASSERT(20 == L2.length());

        mL2.insertList(20, A20);                   ASSERT(21 == L2.length());
        mL2.insertTable(21, A21);                  ASSERT(22 == L2.length());

        mL2.insertBool(22, A22);                   ASSERT(23 == L2.length());
        mL2.insertDatetimeTz(23, A23);             ASSERT(24 == L2.length());
        mL2.insertDateTz(24, A24);                 ASSERT(25 == L2.length());
        mL2.insertTimeTz(25, A25);                 ASSERT(26 == L2.length());
        mL2.insertBoolArray(26, A26);              ASSERT(27 == L2.length());
        mL2.insertDatetimeTzArray(27, A27);        ASSERT(28 == L2.length());
        mL2.insertDateTzArray(28, A28);            ASSERT(29 == L2.length());
        mL2.insertTimeTzArray(29, A29);            ASSERT(30 == L2.length());

        mL2.insertChoice(30, A30);                 ASSERT(31 == L2.length());
        mL2.insertChoiceArray(31, A31);            ASSERT(32 == L2.length());

        ASSERT(L == L2);

        if (verbose) cout <<
            "\nCreate a third list appending/inserting string/list." << endl;

        bdem_List mL3; const bdem_List& L3 = mL3;  ASSERT( 0 == L3.length());

        mL3.appendString(A06);
        mL3.appendList(A20);
        mL3.appendString(B06);
        mL3.appendList(B20);
        mL3.insertString(4, A06);
        mL3.insertList(0, A20);
        mL3.insertString(4, B06);
        mL3.insertList(2, B20);

        if (veryVerbose) L3.print(cout, 1, 8);

        if (verbose) cout <<
            "\nCreate a fourth list appending/inserting (char *)/row." << endl;

        bdem_List mL4; const bdem_List& L4 = mL4;  ASSERT( 0 == L4.length());

        mL4.appendString(A06.c_str());
        mL4.appendList(A20.row());
        mL4.appendString(B06.c_str());
        mL4.appendList(B20.row());
        mL4.insertString(4, A06.c_str());
        mL4.insertList(0, A20.row());
        mL4.insertString(4, B06.c_str());
        mL4.insertList(2, B20.row());

        if (veryVerbose) L4.print(cout, 1, 8);

        ASSERT(L3 == L4);
}

static void testCase14(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // INSERT UNSET/NULL ELEMENTS
        //
        // Concerns:
        //   - That each insertNull method inserts a null element of the
        //      specified type at the appropriate location
        //   - That the appendNull method works with empty & non-empty lists.
        //
        // Plan:
        //   - Test all 32 insertNullTYPE methods by creating a list of
        //      elements with each type (in reverse order by inserting at
        //      position 0) and make sure that each inserted element has the
        //      null value of the corresponding type.
        //   - Test the 'insertNullElement' method by creating a similar list
        //      in a for-loop (again at the beginning) and compare the
        //      resulting list with the previous one.
        //   - Test the 'appendNullElements' method by first creating an
        //      array of all 32 'bdem_ElemType::Type' values and making
        //      sure the method works with:
        //       - A single list with all 32 element types (i.e., == previous).
        //       - A zero-length list.
        //       - All 32 1-element lists from an empty list.
        //       - All 32 1-element lists in different initial positions.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //
        //   void insertNullBool();
        //   void insertNullChar();
        //   void insertNullShort();
        //   void insertNullInt();
        //   void insertNullInt64();
        //   void insertNullFloat();
        //   void insertNullDouble();
        //   void insertNullString();
        //   void insertNullDatetime();
        //   void insertNullDatetimeTz();
        //   void insertNullDate();
        //   void insertNullDateTz();
        //   void insertNullTime();
        //   void insertNullTimeTz();
        //   void insertNullBoolArray();
        //   void insertNullCharArray();
        //   void insertNullShortArray();
        //   void insertNullIntArray();
        //   void insertNullInt64Array();
        //   void insertNullFloatArray();
        //   void insertNullDoubleArray();
        //   void insertNullStringArray();
        //   void insertNullDatetimeArray();
        //   void insertNullDatetimeTzArray();
        //   void insertNullDateArray();
        //   void insertNullDateTzArray();
        //   void insertNullTimeArray();
        //   void insertNullTimeTzArray();
        //   void insertNullList();
        //   void insertNullTable();
        //   void insertNullChoice();
        //   void insertNullChoiceArray();
        //   void insertNullElement(bdem_ElemType::Type elementType);
        //   void insertNullElements(int, const bdem_ElemType::Type [], int);
        //   void insertNullElements(int, const vector<bdem_ElemType::Type>&);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INSERT UNSET ELEMENTS" << endl
                          << "=====================" << endl;

        if (verbose) cout <<
           "\nCreate a list of unset elements using 'insertNullTYPE'."<<endl;

        bdem_List mL; const bdem_List& L = mL;  ASSERT( 0 == L.length());

        mL.insertNullChoiceArray(0);           ASSERT( 1 == L.length());
        mL.insertNullChoice(0);                ASSERT( 2 == L.length());

        mL.insertNullTimeTzArray(0);           ASSERT( 3 == L.length());
        mL.insertNullDateTzArray(0);           ASSERT( 4 == L.length());
        mL.insertNullDatetimeTzArray(0);       ASSERT( 5 == L.length());
        mL.insertNullBoolArray(0);             ASSERT( 6 == L.length());
        mL.insertNullTimeTz(0);                ASSERT( 7 == L.length());
        mL.insertNullDateTz(0);                ASSERT( 8 == L.length());
        mL.insertNullDatetimeTz(0);            ASSERT( 9 == L.length());
        mL.insertNullBool(0);                  ASSERT(10 == L.length());

        mL.insertNullTable(0);                 ASSERT(11 == L.length());
        mL.insertNullList(0);                  ASSERT(12 == L.length());
        mL.insertNullTimeArray(0);             ASSERT(13 == L.length());
        mL.insertNullDateArray(0);             ASSERT(14 == L.length());
        mL.insertNullDatetimeArray(0);         ASSERT(15 == L.length());
        mL.insertNullStringArray(0);           ASSERT(16 == L.length());
        mL.insertNullDoubleArray(0);           ASSERT(17 == L.length());
        mL.insertNullFloatArray(0);            ASSERT(18 == L.length());
        mL.insertNullInt64Array(0);            ASSERT(19 == L.length());
        mL.insertNullIntArray(0);              ASSERT(20 == L.length());
        mL.insertNullShortArray(0);            ASSERT(21 == L.length());
        mL.insertNullCharArray(0);             ASSERT(22 == L.length());
        mL.insertNullTime(0);                  ASSERT(23 == L.length());
        mL.insertNullDate(0);                  ASSERT(24 == L.length());
        mL.insertNullDatetime(0);              ASSERT(25 == L.length());
        mL.insertNullString(0);                ASSERT(26 == L.length());
        mL.insertNullDouble(0);                ASSERT(27 == L.length());
        mL.insertNullFloat(0);                 ASSERT(28 == L.length());
        mL.insertNullInt64(0);                 ASSERT(29 == L.length());
        mL.insertNullInt(0);                   ASSERT(30 == L.length());
        mL.insertNullShort(0);                 ASSERT(31 == L.length());
        mL.insertNullChar(0);                  ASSERT(32 == L.length());

        if (verbose) cout <<
          "\tCompare with equivalent list created using loadReferenceN."<<endl;
        {
            bdem_List mX; const bdem_List& X = mX;  ASSERT( 0 == X.length());
            loadReferenceN(&mX);                    ASSERT(32 == X.length());
            bdem_List mY; const bdem_List& Y = mY;  ASSERT( 0 == Y.length());
            loadReferenceNull(&mY);                 ASSERT(32 == Y.length());
            ASSERT(X != L);
            LOOP2_ASSERT(Y, L, Y == L);
        }

        if (verbose) cout <<
           "\tCompare with a third list (appending) using 'insertNullTYPE'."
                                                                        <<endl;

        bdem_List mL2; const bdem_List& L2 = mL2;  ASSERT( 0 == L2.length());

        mL2.insertNullChar(0);                    ASSERT( 1 == L2.length());
        mL2.insertNullShort(1);                   ASSERT( 2 == L2.length());
        mL2.insertNullInt(2);                     ASSERT( 3 == L2.length());
        mL2.insertNullInt64(3);                   ASSERT( 4 == L2.length());
        mL2.insertNullFloat(4);                   ASSERT( 5 == L2.length());
        mL2.insertNullDouble(5);                  ASSERT( 6 == L2.length());
        mL2.insertNullString(6);                  ASSERT( 7 == L2.length());
        mL2.insertNullDatetime(7);                ASSERT( 8 == L2.length());
        mL2.insertNullDate(8);                    ASSERT( 9 == L2.length());
        mL2.insertNullTime(9);                    ASSERT(10 == L2.length());

        mL2.insertNullCharArray(10);              ASSERT(11 == L2.length());
        mL2.insertNullShortArray(11);             ASSERT(12 == L2.length());
        mL2.insertNullIntArray(12);               ASSERT(13 == L2.length());
        mL2.insertNullInt64Array(13);             ASSERT(14 == L2.length());
        mL2.insertNullFloatArray(14);             ASSERT(15 == L2.length());
        mL2.insertNullDoubleArray(15);            ASSERT(16 == L2.length());
        mL2.insertNullStringArray(16);            ASSERT(17 == L2.length());
        mL2.insertNullDatetimeArray(17);          ASSERT(18 == L2.length());
        mL2.insertNullDateArray(18);              ASSERT(19 == L2.length());
        mL2.insertNullTimeArray(19);              ASSERT(20 == L2.length());

        mL2.insertNullList(20);                   ASSERT(21 == L2.length());
        mL2.insertNullTable(21);                  ASSERT(22 == L2.length());

        mL2.insertNullBool(22);                   ASSERT(23 == L2.length());
        mL2.insertNullDatetimeTz(23);             ASSERT(24 == L2.length());
        mL2.insertNullDateTz(24);                 ASSERT(25 == L2.length());
        mL2.insertNullTimeTz(25);                 ASSERT(26 == L2.length());
        mL2.insertNullBoolArray(26);              ASSERT(27 == L2.length());
        mL2.insertNullDatetimeTzArray(27);        ASSERT(28 == L2.length());
        mL2.insertNullDateTzArray(28);            ASSERT(29 == L2.length());
        mL2.insertNullTimeTzArray(29);            ASSERT(30 == L2.length());

        mL2.insertNullChoice(30);                 ASSERT(31 == L2.length());
        mL2.insertNullChoiceArray(31);            ASSERT(32 == L2.length());

        ASSERT(L == L2);

        if (verbose) cout <<
            "\tCompare with a fourth list (insert) using 'insertNullElement'."
                                                                        <<endl;
        {
            bdem_List x;
            for (int i = 0; i < 32; ++i) {
                x.insertNullElement(0, bdem_ElemType::Type(31 - i));
            }
            ASSERT(L == x);
        }

        if (verbose) cout <<
            "\tCompare with a fifth list (append) using 'insertNullElement'."
                                                                       << endl;
        {
            bdem_List x;
            for (int i = 0; i < 32; ++i) {
                x.insertNullElement(i, bdem_ElemType::Type(i));
            }
            ASSERT(L == x);
        }

        const bdem_ElemType::Type TYPES[] = {
            bdem_ElemType::BDEM_CHAR,
            bdem_ElemType::BDEM_SHORT,
            bdem_ElemType::BDEM_INT,
            bdem_ElemType::BDEM_INT64,
            bdem_ElemType::BDEM_FLOAT,
            bdem_ElemType::BDEM_DOUBLE,
            bdem_ElemType::BDEM_STRING,
            bdem_ElemType::BDEM_DATETIME,
            bdem_ElemType::BDEM_DATE,
            bdem_ElemType::BDEM_TIME,

            bdem_ElemType::BDEM_CHAR_ARRAY,
            bdem_ElemType::BDEM_SHORT_ARRAY,
            bdem_ElemType::BDEM_INT_ARRAY,
            bdem_ElemType::BDEM_INT64_ARRAY,
            bdem_ElemType::BDEM_FLOAT_ARRAY,
            bdem_ElemType::BDEM_DOUBLE_ARRAY,
            bdem_ElemType::BDEM_STRING_ARRAY,
            bdem_ElemType::BDEM_DATETIME_ARRAY,
            bdem_ElemType::BDEM_DATE_ARRAY,
            bdem_ElemType::BDEM_TIME_ARRAY,

            bdem_ElemType::BDEM_LIST,
            bdem_ElemType::BDEM_TABLE,

            bdem_ElemType::BDEM_BOOL,
            bdem_ElemType::BDEM_DATETIMETZ,
            bdem_ElemType::BDEM_DATETZ,
            bdem_ElemType::BDEM_TIMETZ,
            bdem_ElemType::BDEM_BOOL_ARRAY,
            bdem_ElemType::BDEM_DATETIMETZ_ARRAY,
            bdem_ElemType::BDEM_DATETZ_ARRAY,
            bdem_ElemType::BDEM_TIMETZ_ARRAY,

            bdem_ElemType::BDEM_CHOICE,
            bdem_ElemType::BDEM_CHOICE_ARRAY,
        };
        const int NUM_TYPES = sizeof TYPES/sizeof *TYPES;
        ASSERT(32 == NUM_TYPES);

        const bsl::vector<bdem_ElemType::Type> TYPES_VEC(TYPES,
                                                         TYPES + NUM_TYPES);

        if (verbose) cout <<
            "\tCompare with a sixth list using 'insertNullElements'." << endl;
        {
            bdem_List x, y;
            x.insertNullElements(0, TYPES, NUM_TYPES);
            y.insertNullElements(0, TYPES_VEC);
            ASSERT(L == x);
            ASSERT(L == y);
        }

        if (verbose) cout <<
            "\nTest various subsets using insertNullElements." << endl;

        if (verbose) cout << "\tTest length 0." << endl;
        {
            bdem_List x, y;
            const bsl::vector<bdem_ElemType::Type> EMPTY_TYPES_VEC;

            x.insertNullElements(0, TYPES, 0);
            y.insertNullElements(0, EMPTY_TYPES_VEC);

            ASSERT(bdem_List() == x);
            ASSERT(bdem_List() == y);
        }

        if (verbose) cout << "\tTest each length 1 on empty list position."
                                                                       << endl;
        {
            for (int i = 0; i < 32; ++i) {
                bdem_List x, y;

                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, TYPES_VEC[i]);

                x.insertNullElements(0, TYPES + i, 1);
                y.insertNullElements(0, ONE_TYPE_VEC);

                LOOP_ASSERT(i, L[i] == x[0]);
                LOOP_ASSERT(i, L[i] == y[0]);
            }
        }

        if (verbose) cout <<
            "\tTest each length 1 in a different position (reverse)." << endl;
        {
            bdem_List x, y;
            for (int i = 0; i < 32; ++i) {
                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(
                                                            1,
                                                            TYPES_VEC[31 - i]);

                x.insertNullElements(0, TYPES + 31 - i, 1);
                y.insertNullElements(0, ONE_TYPE_VEC);
            }
            ASSERT(L == x);
            ASSERT(L == y);
        }

        if (verbose) cout <<
            "\tTest each length 1 in a different position (forward)." << endl;
        {
            bdem_List x, y;
            for (int i = 0; i < 32; ++i) {
                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, TYPES_VEC[i]);

                x.insertNullElements(i, TYPES + i, 1);
                y.insertNullElements(i, ONE_TYPE_VEC);
            }
            ASSERT(L == x);
            ASSERT(L == y);
        }

        if (verbose) cout << "\tTest 11 + 11 (append)." << endl;
        {
            bdem_List x, y;

            bsl::vector<bdem_ElemType::Type> MULTIPLE_TYPES_VEC1(TYPES,
                                                                 TYPES + 11);
            bsl::vector<bdem_ElemType::Type> MULTIPLE_TYPES_VEC2(
                                                            TYPES + 11,
                                                            TYPES + NUM_TYPES);

            x.insertNullElements(0, TYPES, 11);
            x.insertNullElements(11, TYPES + 11, NUM_TYPES - 11);
            y.insertNullElements(0, MULTIPLE_TYPES_VEC1);
            y.insertNullElements(11, MULTIPLE_TYPES_VEC2);

            ASSERT(L == x);
            ASSERT(L == y);
        }

        if (verbose) cout << "\tTest 11 * 2 (insert)." << endl;
        {
            bdem_List x, y;
            for (int i = 0; i < 32; i += 2) {
                bsl::vector<bdem_ElemType::Type> TWO_TYPES_VEC(TYPES + 30 - i,
                                                               TYPES + 32 - i);

                x.insertNullElements(0, TYPES + 30 - i, 2);
                y.insertNullElements(0, TWO_TYPES_VEC);
            }
            ASSERT(L == x);
            ASSERT(L == y);
        }

        if (veryVerbose) {
            cout << "\nDefault allocator usage:" << endl;

                P_(defaultAllocator.numBytesInUse())
                                        P(defaultAllocator.numBlocksInUse())
                P_(defaultAllocator.numBytesMax())
                                        P(defaultAllocator.numBlocksMax())
                P_(defaultAllocator.numBytesTotal())
                                        P(defaultAllocator.numBlocksTotal())
        }

        if (verbose) cout << endl
                          << "INSERT NULL ELEMENTS" << endl
                          << "====================" << endl;

        if (verbose) cout <<
           "\nCreate a list of unset elements using 'insertNullTYPE'."<<endl;

        bdem_List mN; const bdem_List& N = mN;  ASSERT( 0 == N.length());

        mN.insertNullChoiceArray(0);           ASSERT( 1 == N.length());
        mN.insertNullChoice(0);                ASSERT( 2 == N.length());

        mN.insertNullTimeTzArray(0);           ASSERT( 3 == N.length());
        mN.insertNullDateTzArray(0);           ASSERT( 4 == N.length());
        mN.insertNullDatetimeTzArray(0);       ASSERT( 5 == N.length());
        mN.insertNullBoolArray(0);             ASSERT( 6 == N.length());
        mN.insertNullTimeTz(0);                ASSERT( 7 == N.length());
        mN.insertNullDateTz(0);                ASSERT( 8 == N.length());
        mN.insertNullDatetimeTz(0);            ASSERT( 9 == N.length());
        mN.insertNullBool(0);                  ASSERT(10 == N.length());

        mN.insertNullTable(0);                 ASSERT(11 == N.length());
        mN.insertNullList(0);                  ASSERT(12 == N.length());
        mN.insertNullTimeArray(0);             ASSERT(13 == N.length());
        mN.insertNullDateArray(0);             ASSERT(14 == N.length());
        mN.insertNullDatetimeArray(0);         ASSERT(15 == N.length());
        mN.insertNullStringArray(0);           ASSERT(16 == N.length());
        mN.insertNullDoubleArray(0);           ASSERT(17 == N.length());
        mN.insertNullFloatArray(0);            ASSERT(18 == N.length());
        mN.insertNullInt64Array(0);            ASSERT(19 == N.length());
        mN.insertNullIntArray(0);              ASSERT(20 == N.length());
        mN.insertNullShortArray(0);            ASSERT(21 == N.length());
        mN.insertNullCharArray(0);             ASSERT(22 == N.length());
        mN.insertNullTime(0);                  ASSERT(23 == N.length());
        mN.insertNullDate(0);                  ASSERT(24 == N.length());
        mN.insertNullDatetime(0);              ASSERT(25 == N.length());
        mN.insertNullString(0);                ASSERT(26 == N.length());
        mN.insertNullDouble(0);                ASSERT(27 == N.length());
        mN.insertNullFloat(0);                 ASSERT(28 == N.length());
        mN.insertNullInt64(0);                 ASSERT(29 == N.length());
        mN.insertNullInt(0);                   ASSERT(30 == N.length());
        mN.insertNullShort(0);                 ASSERT(31 == N.length());
        mN.insertNullChar(0);                  ASSERT(32 == N.length());

        if (verbose) cout <<
          "\tCompare with equivalent list created using loadReferenceN."<<endl;
        {
            bdem_List mX; const bdem_List& X = mX;  ASSERT( 0 == X.length());
            loadReferenceN(&mX);                    ASSERT(32 == X.length());

            ASSERT(X != N);
            for (int i = 0; i < 32; ++i) {
                mX[i].makeNull();
            }
            ASSERT(X == N);
        }

        if (verbose) cout <<
           "\tCompare with a third list (appending) using 'insertNullTYPE'."
                                                                        <<endl;

        bdem_List mN2; const bdem_List& N2 = mN2;  ASSERT( 0 == N2.length());

        mN2.insertNullChar(0);                    ASSERT( 1 == N2.length());
        mN2.insertNullShort(1);                   ASSERT( 2 == N2.length());
        mN2.insertNullInt(2);                     ASSERT( 3 == N2.length());
        mN2.insertNullInt64(3);                   ASSERT( 4 == N2.length());
        mN2.insertNullFloat(4);                   ASSERT( 5 == N2.length());
        mN2.insertNullDouble(5);                  ASSERT( 6 == N2.length());
        mN2.insertNullString(6);                  ASSERT( 7 == N2.length());
        mN2.insertNullDatetime(7);                ASSERT( 8 == N2.length());
        mN2.insertNullDate(8);                    ASSERT( 9 == N2.length());
        mN2.insertNullTime(9);                    ASSERT(10 == N2.length());

        mN2.insertNullCharArray(10);              ASSERT(11 == N2.length());
        mN2.insertNullShortArray(11);             ASSERT(12 == N2.length());
        mN2.insertNullIntArray(12);               ASSERT(13 == N2.length());
        mN2.insertNullInt64Array(13);             ASSERT(14 == N2.length());
        mN2.insertNullFloatArray(14);             ASSERT(15 == N2.length());
        mN2.insertNullDoubleArray(15);            ASSERT(16 == N2.length());
        mN2.insertNullStringArray(16);            ASSERT(17 == N2.length());
        mN2.insertNullDatetimeArray(17);          ASSERT(18 == N2.length());
        mN2.insertNullDateArray(18);              ASSERT(19 == N2.length());
        mN2.insertNullTimeArray(19);              ASSERT(20 == N2.length());

        mN2.insertNullList(20);                   ASSERT(21 == N2.length());
        mN2.insertNullTable(21);                  ASSERT(22 == N2.length());

        mN2.insertNullBool(22);                   ASSERT(23 == N2.length());
        mN2.insertNullDatetimeTz(23);             ASSERT(24 == N2.length());
        mN2.insertNullDateTz(24);                 ASSERT(25 == N2.length());
        mN2.insertNullTimeTz(25);                 ASSERT(26 == N2.length());
        mN2.insertNullBoolArray(26);              ASSERT(27 == N2.length());
        mN2.insertNullDatetimeTzArray(27);        ASSERT(28 == N2.length());
        mN2.insertNullDateTzArray(28);            ASSERT(29 == N2.length());
        mN2.insertNullTimeTzArray(29);            ASSERT(30 == N2.length());

        mN2.insertNullChoice(30);                 ASSERT(31 == N2.length());
        mN2.insertNullChoiceArray(31);            ASSERT(32 == N2.length());

        ASSERT(N == N2);

        if (verbose) cout <<
           "\tCompare with a third list (appending) using 'insertNullTYPE'."
                                                                        <<endl;

        const int N_ELEM = NUM_TYPES;
        bdem_List mN3; const bdem_List& N3 = mN3;ASSERT(0      == N3.length());
        loadReferenceA(&mN3);                    ASSERT(N_ELEM == N3.length());

        mN3.insertNullChar(0);             ASSERT( 1 + N_ELEM == N3.length());
        mN3.insertNullShort(2);            ASSERT( 2 + N_ELEM == N3.length());
        mN3.insertNullInt(4);              ASSERT( 3 + N_ELEM == N3.length());
        mN3.insertNullInt64(6);            ASSERT( 4 + N_ELEM == N3.length());
        mN3.insertNullFloat(8);            ASSERT( 5 + N_ELEM == N3.length());
        mN3.insertNullDouble(10);          ASSERT( 6 + N_ELEM == N3.length());
        mN3.insertNullString(12);          ASSERT( 7 + N_ELEM == N3.length());
        mN3.insertNullDatetime(14);        ASSERT( 8 + N_ELEM == N3.length());
        mN3.insertNullDate(16);            ASSERT( 9 + N_ELEM == N3.length());
        mN3.insertNullTime(18);            ASSERT(10 + N_ELEM == N3.length());

        mN3.insertNullCharArray(20);       ASSERT(11 + N_ELEM == N3.length());
        mN3.insertNullShortArray(22);      ASSERT(12 + N_ELEM == N3.length());
        mN3.insertNullIntArray(24);        ASSERT(13 + N_ELEM == N3.length());
        mN3.insertNullInt64Array(26);      ASSERT(14 + N_ELEM == N3.length());
        mN3.insertNullFloatArray(28);      ASSERT(15 + N_ELEM == N3.length());
        mN3.insertNullDoubleArray(30);     ASSERT(16 + N_ELEM == N3.length());
        mN3.insertNullStringArray(32);     ASSERT(17 + N_ELEM == N3.length());
        mN3.insertNullDatetimeArray(34);   ASSERT(18 + N_ELEM == N3.length());
        mN3.insertNullDateArray(36);       ASSERT(19 + N_ELEM == N3.length());
        mN3.insertNullTimeArray(38);       ASSERT(20 + N_ELEM == N3.length());

        mN3.insertNullList(40);            ASSERT(21 + N_ELEM == N3.length());
        mN3.insertNullTable(42);           ASSERT(22 + N_ELEM == N3.length());

        mN3.insertNullBool(44);            ASSERT(23 + N_ELEM == N3.length());
        mN3.insertNullDatetimeTz(46);      ASSERT(24 + N_ELEM == N3.length());
        mN3.insertNullDateTz(48);          ASSERT(25 + N_ELEM == N3.length());
        mN3.insertNullTimeTz(50);          ASSERT(26 + N_ELEM == N3.length());
        mN3.insertNullBoolArray(52);       ASSERT(27 + N_ELEM == N3.length());
        mN3.insertNullDatetimeTzArray(54); ASSERT(28 + N_ELEM == N3.length());
        mN3.insertNullDateTzArray(56);     ASSERT(29 + N_ELEM == N3.length());
        mN3.insertNullTimeTzArray(58);     ASSERT(30 + N_ELEM == N3.length());

        mN3.insertNullChoice(60);          ASSERT(31 + N_ELEM == N3.length());
        mN3.insertNullChoiceArray(62);     ASSERT(32 + N_ELEM == N3.length());

        mN3.removeElement(1);
        mN3.removeElement(2);
        mN3.removeElement(3);
        mN3.removeElement(4);
        mN3.removeElement(5);
        mN3.removeElement(6);
        mN3.removeElement(7);
        mN3.removeElement(8);
        mN3.removeElement(9);
        mN3.removeElement(10);
        mN3.removeElement(11);
        mN3.removeElement(12);
        mN3.removeElement(13);
        mN3.removeElement(14);
        mN3.removeElement(15);
        mN3.removeElement(16);
        mN3.removeElement(17);
        mN3.removeElement(18);
        mN3.removeElement(19);
        mN3.removeElement(20);
        mN3.removeElement(21);
        mN3.removeElement(22);
        mN3.removeElement(23);
        mN3.removeElement(24);
        mN3.removeElement(25);
        mN3.removeElement(26);
        mN3.removeElement(27);
        mN3.removeElement(28);
        mN3.removeElement(29);
        mN3.removeElement(30);
        mN3.removeElement(31);
        mN3.removeElement(32);

        ASSERT(N == N3);

        if (verbose) cout <<
            "\tCompare with a fourth list (insert) using 'insertNullElement'."
                                                                        <<endl;
        {
            bdem_List x;
            for (int i = 0; i < 32; ++i) {
                x.insertNullElement(0, bdem_ElemType::Type(31 - i));
            }
            ASSERT(N == x);
        }

        if (verbose) cout <<
            "\tCompare with a fifth list (append) using 'insertNullElement'."
                                                                       << endl;
        {
            bdem_List x;
            for (int i = 0; i < 32; ++i) {
                x.insertNullElement(i, bdem_ElemType::Type(i));
            }
            ASSERT(N == x);
        }

        if (verbose) cout <<
            "\tCompare with a sixth list using 'insertNullElements'." << endl;
        {
            bdem_List x, y;
            x.insertNullElements(0, TYPES, NUM_TYPES);
            y.insertNullElements(0, TYPES_VEC);
            ASSERT(N == x);
            ASSERT(N == y);
        }

        if (verbose) cout <<
            "\nTest various subsets using insertNullElements." << endl;

        if (verbose) cout << "\tTest length 0." << endl;
        {
            bdem_List x, y;
            const bsl::vector<bdem_ElemType::Type> EMPTY_TYPES_VEC;

            x.insertNullElements(0, TYPES, 0);
            y.insertNullElements(0, EMPTY_TYPES_VEC);

            ASSERT(bdem_List() == x);
            ASSERT(bdem_List() == y);
        }

        if (verbose) cout << "\tTest each length 1 on empty list position."
                                                                       << endl;
        {
            for (int i = 0; i < 32; ++i) {
                bdem_List x, y;

                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, TYPES_VEC[i]);

                x.insertNullElements(0, TYPES + i, 1);
                y.insertNullElements(0, ONE_TYPE_VEC);

                LOOP_ASSERT(i, N[i] == x[0]);
                LOOP_ASSERT(i, N[i] == y[0]);
            }
        }

        if (verbose) cout <<
            "\tTest each length 1 in a different position (reverse)." << endl;
        {
            bdem_List x, y;
            for (int i = 0; i < 32; ++i) {
                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(
                                                            1,
                                                            TYPES_VEC[31 - i]);

                x.insertNullElements(0, TYPES + 31 - i, 1);
                y.insertNullElements(0, ONE_TYPE_VEC);
            }
            ASSERT(N == x);
            ASSERT(N == y);
        }

        if (verbose) cout <<
            "\tTest each length 1 in a different position (forward)." << endl;
        {
            bdem_List x, y;
            for (int i = 0; i < 32; ++i) {
                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, TYPES_VEC[i]);

                x.insertNullElements(i, TYPES + i, 1);
                y.insertNullElements(i, ONE_TYPE_VEC);
            }
            ASSERT(N == x);
            ASSERT(N == y);
        }

        if (verbose) cout << "\tTest 11 + 11 (append)." << endl;
        {
            bdem_List x, y;

            bsl::vector<bdem_ElemType::Type> MULTIPLE_TYPES_VEC1(TYPES,
                                                                 TYPES + 11);
            bsl::vector<bdem_ElemType::Type> MULTIPLE_TYPES_VEC2(
                                                            TYPES + 11,
                                                            TYPES + NUM_TYPES);

            x.insertNullElements(0, TYPES, 11);
            x.insertNullElements(11, TYPES + 11, NUM_TYPES - 11);
            y.insertNullElements(0, MULTIPLE_TYPES_VEC1);
            y.insertNullElements(11, MULTIPLE_TYPES_VEC2);

            ASSERT(N == x);
            ASSERT(N == y);
        }

        if (verbose) cout << "\tTest 11 * 2 (insert)." << endl;
        {
            bdem_List x, y;
            for (int i = 0; i < 32; i += 2) {
                bsl::vector<bdem_ElemType::Type> TWO_TYPES_VEC(TYPES + 30 - i,
                                                               TYPES + 32 - i);

                x.insertNullElements(0, TYPES + 30 - i, 2);
                y.insertNullElements(0, TWO_TYPES_VEC);
            }
            ASSERT(N == x);
            ASSERT(N == y);
        }

        if (veryVerbose) {
            cout << "\nDefault allocator usage:" << endl;

                P_(defaultAllocator.numBytesInUse())
                                        P(defaultAllocator.numBlocksInUse())
                P_(defaultAllocator.numBytesMax())
                                        P(defaultAllocator.numBlocksMax())
                P_(defaultAllocator.numBytesTotal())
                                        P(defaultAllocator.numBlocksTotal())
        }
}

static void testCase13(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // APPEND UNSET/NULL ELEMENTS
        //
        // Concerns:
        //   - That each appendNull method appends an unset element of the
        //      specified type.
        //   - That the appendNull method works with empty & non-empty lists.
        //
        // Plan:
        //   - Test all 32 appendNullTYPE methods by creating a list of
        //      elements with each type and comparing it with an equivalent
        //      list created using 'loadReferenceN').
        //   - Test the 'appendNullElement' method by creating a similar list
        //      in a for-loop and compare the resulting list with the
        //      previous one.
        //   - Test the 'appendNullElements' method by first creating an
        //      array of all 32 'bdem_ElemType::Type' values and making
        //      sure the method works with:
        //       - A single list with all 32 element types (i.e., == previous).
        //       - A zero-length list.
        //       - All 32 1-element lists from an empty list.
        //       - All 32 1-element lists in different initial positions.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   void loadReferenceN(bdem_List *result);
        //
        //   void appendNullBool();
        //   void appendNullChar();
        //   void appendNullShort();
        //   void appendNullInt();
        //   void appendNullInt64();
        //   void appendNullFloat();
        //   void appendNullDouble();
        //   void appendNullString();
        //   void appendNullDatetime();
        //   void appendNullDatetimeTz();
        //   void appendNullDate();
        //   void appendNullDateTz();
        //   void appendNullTime();
        //   void appendNullTimeTz();
        //   void appendNullBoolArray();
        //   void appendNullCharArray();
        //   void appendNullShortArray();
        //   void appendNullIntArray();
        //   void appendNullInt64Array();
        //   void appendNullFloatArray();
        //   void appendNullDoubleArray();
        //   void appendNullStringArray();
        //   void appendNullDatetimeArray();
        //   void appendNullDatetimeTzArray();
        //   void appendNullDateArray();
        //   void appendNullDateTzArray();
        //   void appendNullTimeArray();
        //   void appendNullTimeTzArray();
        //   void appendNullList();
        //   void appendNullTable();
        //   void appendNullChoice();
        //   void appendNullChoiceArray();
        //   void appendNullElement(bdem_ElemType::Type elementType);
        //   void appendNullElements(const bdem_ElemType::Type et[], int ne);
        //   void appendNullElements(const bsl::vector<bdem_ElemType::Type>&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "APPEND UNSET ELEMENTS" << endl
                          << "=====================" << endl;

        if (verbose) cout <<
           "\nCreate a list of unset elements using appendNullTYPE." << endl;

        bdem_List mL; const bdem_List& L = mL;  ASSERT( 0 == L.length());

        mL.appendNullChar();                   ASSERT( 1 == L.length());
        mL.appendNullShort();                  ASSERT( 2 == L.length());
        mL.appendNullInt();                    ASSERT( 3 == L.length());
        mL.appendNullInt64();                  ASSERT( 4 == L.length());
        mL.appendNullFloat();                  ASSERT( 5 == L.length());
        mL.appendNullDouble();                 ASSERT( 6 == L.length());
        mL.appendNullString();                 ASSERT( 7 == L.length());
        mL.appendNullDatetime();               ASSERT( 8 == L.length());
        mL.appendNullDate();                   ASSERT( 9 == L.length());
        mL.appendNullTime();                   ASSERT(10 == L.length());

        mL.appendNullCharArray();              ASSERT(11 == L.length());
        mL.appendNullShortArray();             ASSERT(12 == L.length());
        mL.appendNullIntArray();               ASSERT(13 == L.length());
        mL.appendNullInt64Array();             ASSERT(14 == L.length());
        mL.appendNullFloatArray();             ASSERT(15 == L.length());
        mL.appendNullDoubleArray();            ASSERT(16 == L.length());
        mL.appendNullStringArray();            ASSERT(17 == L.length());
        mL.appendNullDatetimeArray();          ASSERT(18 == L.length());
        mL.appendNullDateArray();              ASSERT(19 == L.length());
        mL.appendNullTimeArray();              ASSERT(20 == L.length());

        mL.appendNullList();                   ASSERT(21 == L.length());
        mL.appendNullTable();                  ASSERT(22 == L.length());

        mL.appendNullBool();                   ASSERT(23 == L.length());
        mL.appendNullDatetimeTz();             ASSERT(24 == L.length());
        mL.appendNullDateTz();                 ASSERT(25 == L.length());
        mL.appendNullTimeTz();                 ASSERT(26 == L.length());
        mL.appendNullBoolArray();              ASSERT(27 == L.length());
        mL.appendNullDatetimeTzArray();        ASSERT(28 == L.length());
        mL.appendNullDateTzArray();            ASSERT(29 == L.length());
        mL.appendNullTimeTzArray();            ASSERT(30 == L.length());

        mL.appendNullChoice();                 ASSERT(31 == L.length());
        mL.appendNullChoiceArray();            ASSERT(32 == L.length());

        if (verbose) cout <<
            "\tCompare with an equivalent list created using loadReferenceN."
                                                                       << endl;
        {
            bdem_List mX; const bdem_List& X = mX;  ASSERT( 0 == X.length());
            loadReferenceN(&mX);                    ASSERT(32 == X.length());
            bdem_List mY; const bdem_List& Y = mY;  ASSERT( 0 == Y.length());
            loadReferenceNull(&mY);                 ASSERT(32 == Y.length());
            LOOP2_ASSERT(X, L, X != L);
            ASSERT(Y == L);
        }

        if (verbose) cout <<
            "\tCompare with a third list created using appendNullElement."
                                                                       << endl;
        {
            bdem_List x;
            for (int i = 0; i < 32; ++i) {
                x.appendNullElement(bdem_ElemType::Type(i));
            }
            ASSERT(L == x);
        }

        static bdem_ElemType::Type TYPES[] = {
            bdem_ElemType::BDEM_CHAR,
            bdem_ElemType::BDEM_SHORT,
            bdem_ElemType::BDEM_INT,
            bdem_ElemType::BDEM_INT64,
            bdem_ElemType::BDEM_FLOAT,
            bdem_ElemType::BDEM_DOUBLE,
            bdem_ElemType::BDEM_STRING,
            bdem_ElemType::BDEM_DATETIME,
            bdem_ElemType::BDEM_DATE,
            bdem_ElemType::BDEM_TIME,

            bdem_ElemType::BDEM_CHAR_ARRAY,
            bdem_ElemType::BDEM_SHORT_ARRAY,
            bdem_ElemType::BDEM_INT_ARRAY,
            bdem_ElemType::BDEM_INT64_ARRAY,
            bdem_ElemType::BDEM_FLOAT_ARRAY,
            bdem_ElemType::BDEM_DOUBLE_ARRAY,
            bdem_ElemType::BDEM_STRING_ARRAY,
            bdem_ElemType::BDEM_DATETIME_ARRAY,
            bdem_ElemType::BDEM_DATE_ARRAY,
            bdem_ElemType::BDEM_TIME_ARRAY,

            bdem_ElemType::BDEM_LIST,
            bdem_ElemType::BDEM_TABLE,

            bdem_ElemType::BDEM_BOOL,
            bdem_ElemType::BDEM_DATETIMETZ,
            bdem_ElemType::BDEM_DATETZ,
            bdem_ElemType::BDEM_TIMETZ,
            bdem_ElemType::BDEM_BOOL_ARRAY,
            bdem_ElemType::BDEM_DATETIMETZ_ARRAY,
            bdem_ElemType::BDEM_DATETZ_ARRAY,
            bdem_ElemType::BDEM_TIMETZ_ARRAY,

            bdem_ElemType::BDEM_CHOICE,
            bdem_ElemType::BDEM_CHOICE_ARRAY
        };
        const int NUM_TYPES = sizeof TYPES/sizeof *TYPES;
        ASSERT(32 == NUM_TYPES);

        const bsl::vector<bdem_ElemType::Type> TYPES_VEC(TYPES,
                                                         TYPES + NUM_TYPES);

        if (verbose) cout <<
          "\tCompare a fourth list created using appendNullElements." << endl;
        {
            bdem_List x, y;
            x.appendNullElements(TYPES, NUM_TYPES);
            y.appendNullElements(TYPES_VEC);

            LOOP2_ASSERT(L, x, L == x);
            ASSERT(L == y);
        }

        if (verbose) cout <<
            "\nTest various subsets using appendNullElements" << endl;

        if (verbose) cout << "\tTest length 0." << endl;
        {
            bdem_List x, y;
            const bsl::vector<bdem_ElemType::Type> EMPTY_TYPES_VEC;

            x.appendNullElements(TYPES, 0);
            y.appendNullElements(EMPTY_TYPES_VEC);

            ASSERT(bdem_List() == x);
            ASSERT(bdem_List() == y);
        }

        if (verbose) cout << "\tTest each length 1 on empty list position."
                                                                       << endl;
        {
            for (int i = 0; i < 32; ++i) {
                bdem_List x, y;

                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, TYPES_VEC[i]);

                x.appendNullElements(TYPES + i, 1);
                y.appendNullElements(ONE_TYPE_VEC);

                LOOP_ASSERT(i, L[i] == x[0]);
                LOOP_ASSERT(i, L[i] == y[0]);
            }
        }

        if (verbose) cout << "\tTest each length 1 in a different position."
                                                                       << endl;
        {
            bdem_List x, y;
            for (int i = 0; i < 32; ++i) {
                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, TYPES_VEC[i]);

                x.appendNullElements(TYPES + i, 1);
                y.appendNullElements(ONE_TYPE_VEC);
            }

            ASSERT(L == x);
            ASSERT(L == y);
        }

        if (veryVerbose) {
            cout << "\nDefault allocator usage:" << endl;

                P_(defaultAllocator.numBytesInUse())
                                        P(defaultAllocator.numBlocksInUse())
                P_(defaultAllocator.numBytesMax())
                                        P(defaultAllocator.numBlocksMax())
                P_(defaultAllocator.numBytesTotal())
                                        P(defaultAllocator.numBlocksTotal())
        }

        if (verbose) cout << endl
                          << "APPEND NULL ELEMENTS" << endl
                          << "====================" << endl;

        if (verbose) cout <<
           "\nCreate a list of null elements using appendNullTYPE." << endl;

        bdem_List mN; const bdem_List& N = mN;  ASSERT( 0 == N.length());

        mN.appendNullChar();                   ASSERT( 1 == N.length());
        mN.appendNullShort();                  ASSERT( 2 == N.length());
        mN.appendNullInt();                    ASSERT( 3 == N.length());
        mN.appendNullInt64();                  ASSERT( 4 == N.length());
        mN.appendNullFloat();                  ASSERT( 5 == N.length());
        mN.appendNullDouble();                 ASSERT( 6 == N.length());
        mN.appendNullString();                 ASSERT( 7 == N.length());
        mN.appendNullDatetime();               ASSERT( 8 == N.length());
        mN.appendNullDate();                   ASSERT( 9 == N.length());
        mN.appendNullTime();                   ASSERT(10 == N.length());

        mN.appendNullCharArray();              ASSERT(11 == N.length());
        mN.appendNullShortArray();             ASSERT(12 == N.length());
        mN.appendNullIntArray();               ASSERT(13 == N.length());
        mN.appendNullInt64Array();             ASSERT(14 == N.length());
        mN.appendNullFloatArray();             ASSERT(15 == N.length());
        mN.appendNullDoubleArray();            ASSERT(16 == N.length());
        mN.appendNullStringArray();            ASSERT(17 == N.length());
        mN.appendNullDatetimeArray();          ASSERT(18 == N.length());
        mN.appendNullDateArray();              ASSERT(19 == N.length());
        mN.appendNullTimeArray();              ASSERT(20 == N.length());

        mN.appendNullList();                   ASSERT(21 == N.length());
        mN.appendNullTable();                  ASSERT(22 == N.length());

        mN.appendNullBool();                   ASSERT(23 == N.length());
        mN.appendNullDatetimeTz();             ASSERT(24 == N.length());
        mN.appendNullDateTz();                 ASSERT(25 == N.length());
        mN.appendNullTimeTz();                 ASSERT(26 == N.length());
        mN.appendNullBoolArray();              ASSERT(27 == N.length());
        mN.appendNullDatetimeTzArray();        ASSERT(28 == N.length());
        mN.appendNullDateTzArray();            ASSERT(29 == N.length());
        mN.appendNullTimeTzArray();            ASSERT(30 == N.length());

        mN.appendNullChoice();                 ASSERT(31 == N.length());
        mN.appendNullChoiceArray();            ASSERT(32 == N.length());

        if (verbose) cout <<
            "\tCompare with an equivalent list created using loadReferenceN."
                                                                       << endl;
        {
            bdem_List mX; const bdem_List& X = mX;  ASSERT( 0 == X.length());
            loadReferenceN(&mX);                    ASSERT(32 == X.length());
            for (int i = 0; i < 32; ++i) {
                mX[i].makeNull();
            }
            LOOP2_ASSERT(X, N, X == N);
        }

        bdem_List mN2; const bdem_List& N2 = mN2;  ASSERT( 0 == N2.length());
        bdem_List mA;  const bdem_List& A = mA;
        loadReferenceA(&mA);
        mN2.appendElement(A[31]);
        mN2.appendNullChar();                   ASSERT( 2 == N2.length());
        mN2.appendElement(A[30]);
        mN2.appendNullShort();                  ASSERT( 4 == N2.length());
        mN2.appendElement(A[29]);
        mN2.appendNullInt();                    ASSERT( 6 == N2.length());
        mN2.appendElement(A[28]);
        mN2.appendNullInt64();                  ASSERT( 8 == N2.length());
        mN2.appendElement(A[27]);
        mN2.appendNullFloat();                  ASSERT(10 == N2.length());
        mN2.appendElement(A[26]);
        mN2.appendNullDouble();                 ASSERT(12 == N2.length());
        mN2.appendElement(A[25]);
        mN2.appendNullString();                 ASSERT(14 == N2.length());
        mN2.appendElement(A[24]);
        mN2.appendNullDatetime();               ASSERT(16 == N2.length());
        mN2.appendElement(A[23]);
        mN2.appendNullDate();                   ASSERT(18 == N2.length());
        mN2.appendElement(A[22]);
        mN2.appendNullTime();                   ASSERT(20 == N2.length());

        mN2.appendElement(A[21]);
        mN2.appendNullCharArray();              ASSERT(22 == N2.length());
        mN2.appendElement(A[20]);
        mN2.appendNullShortArray();             ASSERT(24 == N2.length());
        mN2.appendElement(A[19]);
        mN2.appendNullIntArray();               ASSERT(26 == N2.length());
        mN2.appendElement(A[18]);
        mN2.appendNullInt64Array();             ASSERT(28 == N2.length());
        mN2.appendElement(A[17]);
        mN2.appendNullFloatArray();             ASSERT(30 == N2.length());
        mN2.appendElement(A[16]);
        mN2.appendNullDoubleArray();            ASSERT(32 == N2.length());
        mN2.appendElement(A[15]);
        mN2.appendNullStringArray();            ASSERT(34 == N2.length());
        mN2.appendElement(A[14]);
        mN2.appendNullDatetimeArray();          ASSERT(36 == N2.length());
        mN2.appendElement(A[13]);
        mN2.appendNullDateArray();              ASSERT(38 == N2.length());
        mN2.appendElement(A[12]);
        mN2.appendNullTimeArray();              ASSERT(40 == N2.length());

        mN2.appendElement(A[11]);
        mN2.appendNullList();                   ASSERT(42 == N2.length());
        mN2.appendElement(A[10]);
        mN2.appendNullTable();                  ASSERT(44 == N2.length());

        mN2.appendElement(A[9]);
        mN2.appendNullBool();                   ASSERT(46 == N2.length());
        mN2.appendElement(A[8]);
        mN2.appendNullDatetimeTz();             ASSERT(48 == N2.length());
        mN2.appendElement(A[7]);
        mN2.appendNullDateTz();                 ASSERT(50 == N2.length());
        mN2.appendElement(A[6]);
        mN2.appendNullTimeTz();                 ASSERT(52 == N2.length());
        mN2.appendElement(A[5]);
        mN2.appendNullBoolArray();              ASSERT(54 == N2.length());
        mN2.appendElement(A[4]);
        mN2.appendNullDatetimeTzArray();        ASSERT(56 == N2.length());
        mN2.appendElement(A[3]);
        mN2.appendNullDateTzArray();            ASSERT(58 == N2.length());
        mN2.appendElement(A[2]);
        mN2.appendNullTimeTzArray();            ASSERT(60 == N2.length());

        mN2.appendElement(A[1]);
        mN2.appendNullChoice();                 ASSERT(62 == N2.length());
        mN2.appendElement(A[0]);
        mN2.appendNullChoiceArray();            ASSERT(64 == N2.length());

        if (verbose) cout <<
            "\tCompare with a third list created using appendNullElement."
                                                                       << endl;
        {
            bdem_List x;
            for (int i = 0; i < 32; ++i) {
                x.appendNullElement(bdem_ElemType::Type(i));
            }
            ASSERT(N == x);
        }

        if (verbose) cout <<
           "\tCompare a fourth list created using appendNullElements." << endl;
        {
            bdem_List x, y;
            x.appendNullElements(TYPES, NUM_TYPES);
            y.appendNullElements(TYPES_VEC);

            ASSERT(N == x);
            ASSERT(N == y);
        }

        if (verbose) cout <<
            "\nTest various subsets using appendNullElements" << endl;

        if (verbose) cout << "\tTest length 0." << endl;
        {
            bdem_List x, y;
            const bsl::vector<bdem_ElemType::Type> EMPTY_TYPES_VEC;

            x.appendNullElements(TYPES, 0);
            y.appendNullElements(EMPTY_TYPES_VEC);

            ASSERT(bdem_List() == x);
            ASSERT(bdem_List() == y);
        }

        if (verbose) cout << "\tTest each length 1 on empty list position."
                                                                       << endl;
        {
            for (int i = 0; i < 32; ++i) {
                bdem_List x, y;

                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, TYPES_VEC[i]);

                x.appendNullElements(TYPES + i, 1);
                y.appendNullElements(ONE_TYPE_VEC);

                LOOP_ASSERT(i, N[i] == x[0]);
                LOOP_ASSERT(i, N[i] == y[0]);
            }
        }

        if (verbose) cout << "\tTest each length 1 in a different position."
                                                                       << endl;
        {
            bdem_List x, y;
            for (int i = 0; i < 32; ++i) {
                bsl::vector<bdem_ElemType::Type> ONE_TYPE_VEC(1, TYPES_VEC[i]);

                x.appendNullElements(TYPES + i, 1);
                y.appendNullElements(ONE_TYPE_VEC);
            }

            ASSERT(N == x);
            ASSERT(N == y);
        }

        if (veryVerbose) {
            cout << "\nDefault allocator usage:" << endl;

                P_(defaultAllocator.numBytesInUse())
                                        P(defaultAllocator.numBlocksInUse())
                P_(defaultAllocator.numBytesMax())
                                        P(defaultAllocator.numBlocksMax())
                P_(defaultAllocator.numBytesTotal())
                                        P(defaultAllocator.numBlocksTotal())
        }
}

static void testCase12(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // RETURNING REFERENCES TO MODIFIABLE ELEMENTS AND ROWS
        //
        // Concerns:
        //   - That each function returns the appropriate reference.
        //
        // Plan:
        //  Note that we currently cannot use 'operator=' on element references
        //  and must instead use the 'replaceValue' method.
        //   - Create a list containing all 32 element types.
        //   - Use each type of manipulator to modify an element.
        //   - use both styles of access in each case.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   bdem_Row& row();
        //
        //   const char& theChar(int index) const;
        //   const char& theChar(int index) const;
        //   const short& theShort(int index) const;
        //   const int& theInt(int index) const;
        //   const Int64& theInt64(int index) const;
        //   const float& theFloat(int index) const;
        //   const double& theDouble(int index) const;
        //   const bsl::string& theString(int index) const;
        //   const bdet_Datetime& theDatetime(int index) const;
        //   const bdet_Datetime& theDatetime(int index) const;
        //   const bdet_Date& theDate(int index) const;
        //   const bdet_Date& theDate(int index) const;
        //   const bdet_Time& theTime(int index) const;
        //   const bdet_Time& theTime(int index) const;
        //   const bsl::vector<char>& theCharArray(int index) const;
        //   const bsl::vector<char>& theCharArray(int index) const;
        //   const bsl::vector<short>& theShortArray(int index) const;
        //   const bsl::vector<int>& theIntArray(int index) const;
        //   const bsl::vector<Int64>& theInt64Array(int index) const;
        //   const bsl::vector<float>& theFloatArray(int index) const;
        //   const bsl::vector<double>& theDoubleArray(int index) const;
        //   const bsl::vector<bsl::string>& theStringArray(int index) const;
        //   const bsl::vector<bdet_Datetime>&
        //                                   theDatetimeArray(int index) const;
        //   const bsl::vector<bdet_Datetime>&
        //                                   theDatetimeArray(int index) const;
        //   const bsl::vector<bdet_Date>& theDateArray(int index) const;
        //   const bsl::vector<bdet_Date>& theDateArray(int index) const;
        //   const bsl::vector<bdet_Time>& theTimeArray(int index) const;
        //   const bsl::vector<bdet_Time>& theTimeArray(int index) const;
        //   const bdem_List& theList(int index) const;
        //   const bdem_Table& theTable(int index) const;
        //   const bdem_Choice& theChoice(int index) const;
        //   const bdem_ChoiceArray& theChoiceArray(int index) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
             << "RETURNING REFERENCES TO MODIFIABLE ELEMENTS AND ROWS" << endl
             << "====================================================" << endl;

        if (verbose) cout << "\nSet up reference lists." << endl;

        bdem_List mA; const bdem_List& A = mA;  ASSERT( 0 == A.length());
        loadReferenceA(&mA);                    ASSERT(32 == A.length());

        bdem_List mB; const bdem_List& B = mB;  ASSERT( 0 == B.length());
        loadReferenceB(&mB);                    ASSERT(32 == B.length());

        bdem_List mN; const bdem_List& N = mN;  ASSERT( 0 == N.length());
        loadReferenceN(&mN);                    ASSERT(32 == N.length());

        if (veryVerbose) {
            cout << "\tlist A: "; A.print(cout, -1, 8);
            cout << "\tlist B: "; B.print(cout, -1, 8);
        }

        if (verbose) cout << "\nTest 'row' " << endl;
        {
            bdem_List mX(A); const bdem_List& X = mX;
            for (int i = 0; i < 32; ++i) {

                LOOP_ASSERT(i, A == X);
                LOOP_ASSERT(i, A[i] == X[i]);
                LOOP_ASSERT(i, B[i] != X[i]);

                mX[i].replaceValue(B[i]); // list-based

                if (veryVerbose) { Q(--------------------------) P_(i) P(X) }

                LOOP_ASSERT(i, A != X);
                LOOP_ASSERT(i, A[i] != X[i]);
                LOOP_ASSERT(i, B[i] == X[i]);

                mX[i].replaceValue(A[i]); // row-based

                LOOP_ASSERT(i, A == X);
                LOOP_ASSERT(i, A[i] == X[i]);
                LOOP_ASSERT(i, B[i] != X[i]);
            }
        }

        if (verbose) cout << "\nTest 'theModifiableTYPE(n)'." << endl;

        ASSERT(B != A);

        mA.theModifiableChar              ( 0) = B.theChar                ( 0);
        mA.theModifiableShort             ( 1) = B.theShort               ( 1);
        mA.theModifiableInt               ( 2) = B.theInt                 ( 2);
        mA.theModifiableInt64             ( 3) = B.theInt64               ( 3);
        mA.theModifiableFloat             ( 4) = B.theFloat               ( 4);
        mA.theModifiableDouble            ( 5) = B.theDouble              ( 5);
        mA.theModifiableString            ( 6) = B.theString              ( 6);
        mA.theModifiableDatetime          ( 7) = B.theDatetime            ( 7);
        mA.theModifiableDate              ( 8) = B.theDate                ( 8);
        mA.theModifiableTime              ( 9) = B.theTime                ( 9);

        mA.theModifiableCharArray         (10) = B.theCharArray           (10);
        mA.theModifiableShortArray        (11) = B.theShortArray          (11);
        mA.theModifiableIntArray          (12) = B.theIntArray            (12);
        mA.theModifiableInt64Array        (13) = B.theInt64Array          (13);
        mA.theModifiableFloatArray        (14) = B.theFloatArray          (14);
        mA.theModifiableDoubleArray       (15) = B.theDoubleArray         (15);
        mA.theModifiableStringArray       (16) = B.theStringArray         (16);
        mA.theModifiableDatetimeArray     (17) = B.theDatetimeArray       (17);
        mA.theModifiableDateArray         (18) = B.theDateArray           (18);
        mA.theModifiableTimeArray         (19) = B.theTimeArray           (19);

        mA.theModifiableList              (20) = B.theList                (20);
        mA.theModifiableTable             (21) = B.theTable               (21);

        ASSERT(B != A);

        mA.theModifiableBool              (22) = B.theBool                (22);
        mA.theModifiableDatetimeTz        (23) = B.theDatetimeTz          (23);
        mA.theModifiableDateTz            (24) = B.theDateTz              (24);
        mA.theModifiableTimeTz            (25) = B.theTimeTz              (25);
        mA.theModifiableBoolArray         (26) = B.theBoolArray           (26);
        mA.theModifiableDatetimeTzArray   (27) = B.theDatetimeTzArray     (27);
        mA.theModifiableDateTzArray       (28) = B.theDateTzArray         (28);
        mA.theModifiableTimeTzArray       (29) = B.theTimeTzArray         (29);

        ASSERT(B != A);

        mA.theModifiableChoice            (30) = B.theChoice              (30);
        mA.theModifiableChoiceArray       (31) = B.theChoiceArray         (31);

        ASSERT(B == A);

        if (verbose) cout << "\nTest 'theModifiable<Type>(n)'." << endl;

        ASSERT(N != A);

        mA.theModifiableChar              ( 0) = N.theChar                ( 0);
        mA.theModifiableShort             ( 1) = N.theShort               ( 1);
        mA.theModifiableInt               ( 2) = N.theInt                 ( 2);
        mA.theModifiableInt64             ( 3) = N.theInt64               ( 3);
        mA.theModifiableFloat             ( 4) = N.theFloat               ( 4);
        mA.theModifiableDouble            ( 5) = N.theDouble              ( 5);
        mA.theModifiableString            ( 6) = N.theString              ( 6);
        mA.theModifiableDatetime          ( 7) = N.theDatetime            ( 7);
        mA.theModifiableDate              ( 8) = N.theDate                ( 8);
        mA.theModifiableTime              ( 9) = N.theTime                ( 9);

        mA.theModifiableCharArray         (10) = N.theCharArray           (10);
        mA.theModifiableShortArray        (11) = N.theShortArray          (11);
        mA.theModifiableIntArray          (12) = N.theIntArray            (12);
        mA.theModifiableInt64Array        (13) = N.theInt64Array          (13);
        mA.theModifiableFloatArray        (14) = N.theFloatArray          (14);
        mA.theModifiableDoubleArray       (15) = N.theDoubleArray         (15);
        mA.theModifiableStringArray       (16) = N.theStringArray         (16);
        mA.theModifiableDatetimeArray     (17) = N.theDatetimeArray       (17);
        mA.theModifiableDateArray         (18) = N.theDateArray           (18);
        mA.theModifiableTimeArray         (19) = N.theTimeArray           (19);

        mA.theModifiableList              (20) = N.theList                (20);
        mA.theModifiableTable             (21) = N.theTable               (21);

        ASSERT(N != A);

        mA.theModifiableBool              (22) = N.theBool                (22);
        mA.theModifiableDatetimeTz        (23) = N.theDatetimeTz          (23);
        mA.theModifiableDateTz            (24) = N.theDateTz              (24);
        mA.theModifiableTimeTz            (25) = N.theTimeTz              (25);
        mA.theModifiableBoolArray         (26) = N.theBoolArray           (26);
        mA.theModifiableDatetimeTzArray   (27) = N.theDatetimeTzArray     (27);
        mA.theModifiableDateTzArray       (28) = N.theDateTzArray         (28);
        mA.theModifiableTimeTzArray       (29) = N.theTimeTzArray         (29);

        ASSERT(N != A);

        mA.theModifiableChoice            (30) = N.theChoice              (30);
        mA.theModifiableChoiceArray       (31) = N.theChoiceArray         (31);

        ASSERT(N == A);
}

static void testCase11(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // NON-PRIMARY CONSTRUCTORS AND DESTRUCTOR.
        //
        // Concerns:
        //   - That the specified or bslma_Default::defaultAllocator is used.
        //   - That the specified or default (BDEM_PASS_THROUGH) strategy is
        //      used.
        //   - That the value is initialized properly.
        //   - That the destructor cleans up properly.
        //
        // Plan:
        //   - Use 'bslma_TestAllocator' to verify that specified allocator
        //      is used.
        //   - Install a 'bslma_TestAllocator' in 'bdema_DefaultAllocator' to
        //      verify that 'bslma_Default::defaultAllocator()' is used by
        //      default.
        //   - For each constructor, create four instances: the three
        //      strategies other than BDEM_SUBORDINATE and one with the
        //      strategy omitted.  Verify that 'BDEM_PASS_THROUGH' and "not
        //      specified" have the same allocation pattern.
        //   - Throughout this test case, confirm that each constructors
        //      produces the correct value.
        //   - Allow the destructor to execute in essentially all relevant
        //      white-box states (and asserts all appropriate invariants).
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   bdem_List(bdem_AggregateOption::AllocationStrategy am, ba);
        //
        //   bdem_List(const bdem_ElemType::Type elemType[], int ne, ba);
        //   bdem_List(const bdem_ElemType::Type elemType[], int ne, am, ba);
        //
        //   bdem_List(const bsl::vector<bdem_ElemType::Type>& et, ba);
        //   bdem_List(const bsl::vector<bdem_ElemType::Type>& et, am, ba);
        //
        //   bdem_List(const bdem_List& original, am, ba);
        //   bdem_List(const bdem_Row& original, am, ba);
        //
        //   ~bdem_List();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "NON-PRIMARY CONSTRUCTORS AND DESTRUCTOR" << endl
                          << "=======================================" << endl;

        // ---------------- VERIFY SPECIFIED ALLOCATOR IS USED ----------------

        if (verbose) cout << "\nVerify that the specified allocator is used."
                                                                       << endl;

        // The following variables are used to compare usage between the
        // explicit allocator via BDEM_PASS_THROUGH and the one supplied by
        // default.

        int usage1_one,   usage2_one;    // bdem_List(am, ba);
        int usage1_two,   usage2_two;    // bdem_List(et, ne, ba);
        int usage1_three, usage2_three;  // bdem_List(et, ne, am, ba);
        int usage1_four,  usage2_four;   // bdem_List(originalList, am, ba);
        int usage1_five,  usage2_five;   // bdem_List(originalRow, am, ba);
        int usage1_six,   usage2_six;    // bdem_List(et, ba);
        int usage1_seven, usage2_seven;  // bdem_List(et, am, ba);

        // - - - - - - - - - - - - - - - ONE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tONE: bdem_List(am, ba);" << endl;
        {
            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_List list(bdem_AggregateOption::BDEM_PASS_THROUGH, &a);
            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            list.appendString(A06);
            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(1 == list.length());         // Confirm value.
            ASSERT(A06 == list.theString(0));

            usage1_one = usage1; // Record usage for comparison with
            usage2_one = usage2; // allocations from the default allocator.
        }

        // - - - - - - - - - - - - - - - TWO - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTWO: bdem_List(et, ne, ba);" << endl;
        {
            const bdem_ElemType::Type TYPES[] = {
                bdem_ElemType::BDEM_SHORT,
                bdem_ElemType::BDEM_INT
            };
            const int N = sizeof TYPES / sizeof *TYPES;

            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_List list(TYPES, N, &a);
            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            list.appendString(A06);
            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(3 == list.length());         // Confirm value.
            ASSERT(N01 == list.theShort(0));
            ASSERT(N02 == list.theInt(1));
            ASSERT(A06 == list.theString(2));

            usage1_two = usage1; // Record usage for comparison with
            usage2_two = usage2; // allocations from the default allocator.
        }

        // - - - - - - - - - - - - - - - THREE - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTHREE: bdem_List(et, ne, am, ba);" << endl;
        {
            const bdem_ElemType::Type TYPES[] = {
                bdem_ElemType::BDEM_SHORT,
                bdem_ElemType::BDEM_INT
            };
            const int N = sizeof TYPES / sizeof *TYPES;

            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_List list(TYPES, N,
                           bdem_AggregateOption::BDEM_PASS_THROUGH, &a);
            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            list.appendString(A06);
            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(3 == list.length());         // Confirm value.
            ASSERT(N01 == list.theShort(0));
            ASSERT(N02 == list.theInt(1));
            ASSERT(A06 == list.theString(2));

            usage1_three = usage1; // Record usage for comparison with
            usage2_three = usage2; // allocations from the default allocator.

            // The specified allocator should have the same usage whether the
            // allocation strategy is allowed to default to 'BDEM_PASS_THROUGH'
            // (two) or 'BDEM_PASS_THROUGH' is specified explicitly (three).

            LOOP2_ASSERT(usage1_two, usage1_three, usage1_two == usage1_three);
            LOOP2_ASSERT(usage2_two, usage2_three, usage2_two == usage2_three);
        }

        // - - - - - - - - - - - - - - - FOUR - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFOUR: bdem_List(originalList, am, ba);"
                          << endl;
        {
            bslma_TestAllocator aA;                // A: original list
            bslma_TestAllocator aB;                // B: copy constructed list

            int usageA0 = aA.numBlocksTotal();
            int usageB0 = aB.numBlocksTotal();

            if (veryVerbose) { T_ T_ P_(usageA0) P(usageB0) }
            ASSERT(0 == usageA0); ASSERT(0 == usageB0);

            bdem_List listA(bdem_AggregateOption::BDEM_PASS_THROUGH, &aA);
            bdem_List listB(listA,
                            bdem_AggregateOption::BDEM_PASS_THROUGH, &aB);

            int usageA1 = aA.numBlocksTotal();
            int usageB1 = aB.numBlocksTotal();

            if (veryVerbose) { T_ T_ P_(usageA1) P(usageB1) }

            ASSERT(usageA1 > 0); ASSERT(usageB1 > 0);

            listB.appendString(A06);

            int usageA2 = aA.numBlocksTotal();
            int usageB2 = aB.numBlocksTotal();

            if (veryVerbose) { T_ T_ P_(usageA2) P(usageB2) }

            ASSERT(usageA1 == usageA2);
            ASSERT(usageB1  < usageB2);

            ASSERT(1 == listB.length());        // Confirm value.
            ASSERT(A06 == listB.theString(0));

            usage1_four = usageB1; // Record usage for comparison with
            usage2_four = usageB2; // allocations from the default allocator.
        }

        // - - - - - - - - - - - - - - - FIVE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFIVE: bdem_List(originalRow, am, ba);" << endl;
        {
            bslma_TestAllocator aA;                // A: original list
            bslma_TestAllocator aB;                // B: copy constructed list

            int usageA0 = aA.numBlocksTotal();
            int usageB0 = aB.numBlocksTotal();

            if (veryVerbose) { T_ T_ P_(usageA0) P(usageB0) }
            ASSERT(0 == usageA0); ASSERT(0 == usageB0);

            bdem_List listA(bdem_AggregateOption::BDEM_PASS_THROUGH, &aA);
            bdem_List listB(listA.row(),
                            bdem_AggregateOption::BDEM_PASS_THROUGH, &aB);

            int usageA1 = aA.numBlocksTotal();
            int usageB1 = aB.numBlocksTotal();

            if (veryVerbose) { T_ T_ P_(usageA1) P(usageB1) }

            ASSERT(usageA1 > 0); ASSERT(usageB1 > 0);

            listB.appendString(A06);

            int usageA2 = aA.numBlocksTotal();
            int usageB2 = aB.numBlocksTotal();

            if (veryVerbose) { T_ T_ P_(usageA2) P(usageB2) }

            ASSERT(usageA1 == usageA2);
            ASSERT(usageB1  < usageB2);

            ASSERT(1 == listB.length());        // Confirm value.
            ASSERT(A06 == listB.theString(0));

            usage1_five = usageB1; // Record usage for comparison with
            usage2_five = usageB2; // allocations from the default allocator.
        }

        // - - - - - - - - - - - - - - - SIX - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tSIX: bdem_List(et, ba);" << endl;
        {
            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_SHORT);
            TYPES.push_back(bdem_ElemType::BDEM_INT);

            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_List list(TYPES, &a);
            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            list.appendString(A06);
            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(3 == list.length());         // Confirm value.
            ASSERT(N01 == list.theShort(0));
            ASSERT(N02 == list.theInt(1));
            ASSERT(A06 == list.theString(2));

            usage1_six = usage1; // Record usage for comparison with
            usage2_six = usage2; // allocations from the default allocator.
        }

        // - - - - - - - - - - - - - - - SEVEN - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tSEVEN: bdem_List(et, am, ba);" << endl;
        {
            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_SHORT);
            TYPES.push_back(bdem_ElemType::BDEM_INT);

            bslma_TestAllocator a;
            int usage0 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage0) }
            ASSERT(0 == usage0);

            bdem_List list(TYPES, bdem_AggregateOption::BDEM_PASS_THROUGH, &a);
            int usage1 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage1) }
            ASSERT(usage1 > usage0);

            list.appendString(A06);
            int usage2 = a.numBlocksTotal();
            if (veryVerbose) { T_ T_ P(usage2) }
            ASSERT(usage2 > usage1);

            ASSERT(3 == list.length());         // Confirm value.
            ASSERT(N01 == list.theShort(0));
            ASSERT(N02 == list.theInt(1));
            ASSERT(A06 == list.theString(2));

            usage1_seven = usage1; // Record usage for comparison with
            usage2_seven = usage2; // allocations from the default allocator.

            // The specified allocator should have the same usage whether the
            // allocation strategy is allowed to default to 'BDEM_PASS_THROUGH'
            // (six) or 'BDEM_PASS_THROUGH' is specified explicitly (seven).

            LOOP2_ASSERT(usage1_six, usage1_seven, usage1_six == usage1_seven);
            LOOP2_ASSERT(usage2_six, usage2_seven, usage2_six == usage2_seven);
        }

        // ---------------- VERIFY DEFAULT ALLOCATOR IS USED ------------------

        if (verbose) cout <<
             "\nVerify that the 'bslma_Default::defaultAllocator' is used."
                                                                       << endl;

        // - - - - - - - - - - - - - - - ONE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tONE: bdem_List(am);" << endl;
        {
            bslma_TestAllocator a;
            const bslma_DefaultAllocatorGuard dag(&a);
            {
                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_List list(bdem_AggregateOption::BDEM_PASS_THROUGH);
                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                list.appendString(A06);
                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_one):

                LOOP2_ASSERT(usage1_one, usage1, usage1_one == usage1);
                LOOP2_ASSERT(usage2_one, usage2, usage2_one == usage2);
            }
        }

        // - - - - - - - - - - - - - - - TWO - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTWO: bdem_List(et, ne);" << endl;
        {
            bslma_TestAllocator a;
            const bslma_DefaultAllocatorGuard dag(&a);
            {
                const bdem_ElemType::Type TYPES[] = {
                    bdem_ElemType::BDEM_SHORT,
                    bdem_ElemType::BDEM_INT
                };
                const int N = sizeof TYPES / sizeof *TYPES;

                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_List list(TYPES, N);
                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                list.appendString(A06);
                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                ASSERT(N + 1 == list.length());             // Confirm value.
                ASSERT(A06 == list.theString(N));

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_two):

                LOOP2_ASSERT(usage1_two, usage1, usage1_two == usage1);
                LOOP2_ASSERT(usage2_two, usage2, usage2_two == usage2);
            }
        }

        // - - - - - - - - - - - - - - - THREE - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTHREE: bdem_List(et, ne, am);" << endl;
        {
            bslma_TestAllocator a;
            const bslma_DefaultAllocatorGuard dag(&a);
            {
                const bdem_ElemType::Type TYPES[] = {
                    bdem_ElemType::BDEM_SHORT,
                    bdem_ElemType::BDEM_INT
                };
                const int N = sizeof TYPES / sizeof *TYPES;

                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_List list(TYPES, N,
                               bdem_AggregateOption::BDEM_PASS_THROUGH);
                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                list.appendString(A06);
                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                ASSERT(N + 1 == list.length());     // Confirm value.
                ASSERT(A06 == list.theString(N));

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_three):

                LOOP2_ASSERT(usage1_three, usage1, usage1_three == usage1);
                LOOP2_ASSERT(usage2_three, usage2, usage2_three == usage2);
            }
        }

        // - - - - - - - - - - - - - - - FOUR - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFOUR: bdem_List(originalList, am);" << endl;
        {
            bslma_TestAllocator aB;                // B: copy constructed list
            const bslma_DefaultAllocatorGuard dag(&aB);
            {
                bslma_TestAllocator aA;                // A: original list
                int usageA0 = aA.numBlocksTotal();
                int usageB0 = aB.numBlocksTotal();
                if (veryVerbose) { T_ T_ P_(usageA0) P(usageB0) }
                ASSERT(0 == usageA0); ASSERT(0 == usageB0);

                bdem_List listA(bdem_AggregateOption::BDEM_PASS_THROUGH, &aA);
                bdem_List listB(listA,
                                bdem_AggregateOption::BDEM_PASS_THROUGH);

                int usageA1 = aA.numBlocksTotal();
                int usageB1 = aB.numBlocksTotal();

                if (veryVerbose) { T_ T_ P_(usageA1) P(usageB1) }

                ASSERT(usageA1 > 0); ASSERT(usageB1 > 0);

                listB.appendString(A06);

                int usageA2 = aA.numBlocksTotal();
                int usageB2 = aB.numBlocksTotal();

                if (veryVerbose) { T_ T_ P_(usageA2) P(usageB2) }

                ASSERT(usageA1 == usageA2);
                ASSERT(usageB1  < usageB2);

                ASSERT(1 == listB.length());    // Confirm value.
                ASSERT(A06 == listB.theString(0));

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_four):

                LOOP2_ASSERT(usage1_four, usageB1, usage1_four == usageB1);
                LOOP2_ASSERT(usage2_four, usageB2, usage2_four == usageB2);
            }
        }

        // - - - - - - - - - - - - - - - FIVE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFIVE: tbdem_List(originalRow, am);" << endl;
        {
            bslma_TestAllocator aB;                // B: copy-constructed list
            const bslma_DefaultAllocatorGuard dag(&aB);
            {
                bslma_TestAllocator aA;                // A: original list
                int usageA0 = aA.numBlocksTotal();
                int usageB0 = aB.numBlocksTotal();
                if (veryVerbose) { T_ T_ P_(usageA0) P(usageB0) }
                ASSERT(0 == usageA0); ASSERT(0 == usageB0);

                bdem_List listA(bdem_AggregateOption::BDEM_PASS_THROUGH, &aA);
                bdem_List listB(listA.row(),
                                bdem_AggregateOption::BDEM_PASS_THROUGH);

                int usageA1 = aA.numBlocksTotal();
                int usageB1 = aB.numBlocksTotal();

                if (veryVerbose) { T_ T_ P_(usageA1) P(usageB1) }

                ASSERT(usageA1 > 0); ASSERT(usageB1 > 0);

                listB.appendString(A06);

                int usageA2 = aA.numBlocksTotal();
                int usageB2 = aB.numBlocksTotal();

                if (veryVerbose) { T_ T_ P_(usageA2) P(usageB2) }

                ASSERT(usageA1 == usageA2);
                ASSERT(usageB1  < usageB2);

                ASSERT(1 == listB.length());    // Confirm value.
                ASSERT(A06 == listB.theString(0));

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_five):

                LOOP2_ASSERT(usage1_five, usageB1, usage1_five == usageB1);
                LOOP2_ASSERT(usage2_five, usageB2, usage2_five == usageB2);
            }
        }

        // - - - - - - - - - - - - - - - SIX - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tSIX: bdem_List(et);" << endl;
        {
            bslma_TestAllocator a;
            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_SHORT);
            TYPES.push_back(bdem_ElemType::BDEM_INT);

            const bslma_DefaultAllocatorGuard dag(&a);
            {
                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_List list(TYPES);
                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                list.appendString(A06);
                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                ASSERT(3   == list.length());             // Confirm value.
                ASSERT(A06 == list.theString(2));

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_six):

                LOOP2_ASSERT(usage1_six, usage1, usage1_six == usage1);
                LOOP2_ASSERT(usage2_six, usage2, usage2_six == usage2);
            }
        }

        // - - - - - - - - - - - - - - - SEVEN - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tSEVEN: bdem_List(et, am);" << endl;
        {
            bslma_TestAllocator a;
            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_SHORT);
            TYPES.push_back(bdem_ElemType::BDEM_INT);

            const bslma_DefaultAllocatorGuard dag(&a);
            {
                int usage0 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage0) }
                ASSERT(0 == usage0);

                bdem_List list(TYPES, bdem_AggregateOption::BDEM_PASS_THROUGH);
                int usage1 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage1) }
                ASSERT(usage1 > usage0);

                list.appendString(A06);
                int usage2 = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(usage2) }
                ASSERT(usage2 > usage1);

                ASSERT(3   == list.length());     // Confirm value.
                ASSERT(A06 == list.theString(2));

                // Usage for the default allocator (usageN) should be the
                // same as when it is specified explicitly (usageN_seven):

                LOOP2_ASSERT(usage1_seven, usage1, usage1_seven == usage1);
                LOOP2_ASSERT(usage2_seven, usage2, usage2_seven == usage2);
            }
        }

        // ------- VERIFY *SPECIFIED* AND *DEFAULT* ALLOCATION POLICY ------

        if (verbose) cout <<
             "\nVerify *specified* and *default* allocation mode." << endl;

        const bdem_AggregateOption::AllocationStrategy S1 =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;
        const bdem_AggregateOption::AllocationStrategy S2 =
                                       bdem_AggregateOption::BDEM_WRITE_MANY;
        const bdem_AggregateOption::AllocationStrategy S3 =
                                       bdem_AggregateOption::BDEM_WRITE_ONCE;
        const bdem_AggregateOption::AllocationStrategy S4 =
                                       bdem_AggregateOption::BDEM_SUBORDINATE;

        // - - - - - - - - - - - - - - - ONE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tONE: bdem_List(am, ba);" << endl;
        {
            bslma_TestAllocator a;
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4);  // to handle
                                                    // BDEM_SUBORDINATE

            bdem_List mL(     &a);  const bdem_List& L  = mL;
            bdem_List mL1(S1, &a1); const bdem_List& L1 = mL1;
            bdem_List mL2(S2, &a2); const bdem_List& L2 = mL2;
            bdem_List mL3(S3, &a3); const bdem_List& L3 = mL3;
            bdem_List *p = new(ma4) bdem_List(S4, &ma4);
            bdem_List& mL4 = *p;    const bdem_List& L4 = mL4;
            // Note that if BDEM_SUBORDINATE were the default, it would leek
            // memory, which would be reported by the bslma_TestAllocator a4.

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            int use, use1, use2, use3, use4;

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

             mL.appendStringArray(A16);
            mL1.appendStringArray(A16);
            mL2.appendStringArray(A16);
            mL3.appendStringArray(A16);
            mL4.appendStringArray(A16);

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(use1 == use);
            ASSERT(use2 != use); ASSERT(use2 != use3);  ASSERT(use2 != use4)
            ASSERT(use3 != use); ASSERT(use3 != use4);
            ASSERT(use4 != use);

            ASSERT(1 == L.length());    // Confirm value.
            ASSERT(A16 == L.theStringArray(0));
        } // Note: 'bdema_MultipoolAllocator' releases memory on destruction.

        // - - - - - - - - - - - - - - - TWO/THREE - - - - - - - - - - - - - -

        if (verbose) cout << "\tTWO/THREE: bdem_List(et, ne, am, ba);" <<endl;
        {
            const bdem_ElemType::Type TYPES[] = {
                bdem_ElemType::BDEM_INT
            };
            const int N = sizeof TYPES / sizeof *TYPES;

            bslma_TestAllocator a;
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4);  // to handle
                                                    // BDEM_SUBORDINATE

            bdem_List  mL(TYPES, N,     &a);  const bdem_List& L  = mL;
            bdem_List mL1(TYPES, N, S1, &a1); const bdem_List& L1 = mL1;
            bdem_List mL2(TYPES, N, S2, &a2); const bdem_List& L2 = mL2;
            bdem_List mL3(TYPES, N, S3, &a3); const bdem_List& L3 = mL3;
            bdem_List *p = new(ma4) bdem_List(TYPES, N, S4, &ma4);
            bdem_List& mL4 = *p;    const bdem_List& L4 = mL4;
            // Note that if BDEM_SUBORDINATE were the default, it would leek
            // memory, which would be reported by the bslma_TestAllocator a4.

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            int use, use1, use2, use3, use4;

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

             mL.appendStringArray(A16);
            mL1.appendStringArray(A16);
            mL2.appendStringArray(A16);
            mL3.appendStringArray(A16);
            mL4.appendStringArray(A16);

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(use1 == use);
            ASSERT(use2 != use); ASSERT(use2 != use3);  ASSERT(use2 != use4)
            ASSERT(use3 != use); ASSERT(use3 != use4);
            ASSERT(use4 != use);

            ASSERT(2 == L.length());    // Confirm value.
            ASSERT(N02 == L.theInt(0));
            ASSERT(A16 == L.theStringArray(1));
        }

        // - - - - - - - - - - - - - - - FOUR - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFOUR: bdem_List(originalList, am, ba);" <<endl;
        {
            bdem_List mZ(S2); const bdem_List& Z = mZ;  // non-default mode
            mZ.appendDouble(A05);

            bslma_TestAllocator a;
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4);  // to handle
                                                    // BDEM_SUBORDINATE

            bdem_List  mL(Z,     &a);  const bdem_List& L  = mL;
            bdem_List mL1(Z, S1, &a1); const bdem_List& L1 = mL1;
            bdem_List mL2(Z, S2, &a2); const bdem_List& L2 = mL2;
            bdem_List mL3(Z, S3, &a3); const bdem_List& L3 = mL3;
            bdem_List *p = new(ma4) bdem_List(Z, S4, &ma4);
            bdem_List& mL4 = *p;    const bdem_List& L4 = mL4;
            // Note that if BDEM_SUBORDINATE were the default, it would leek
            // memory, which would be reported by the bslma_TestAllocator a4.

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            int use, use1, use2, use3, use4;

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

             mL.appendStringArray(A16);
            mL1.appendStringArray(A16);
            mL2.appendStringArray(A16);
            mL3.appendStringArray(A16);
            mL4.appendStringArray(A16);

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(use1 == use);
            ASSERT(use2 != use); ASSERT(use2 != use3);  ASSERT(use2 != use4)
            ASSERT(use3 != use); ASSERT(use3 != use4);
            ASSERT(use4 != use);

            ASSERT(2 == L.length());    // Confirm value.
            ASSERT(A05 == L.theDouble(0));
            ASSERT(A16 == L.theStringArray(1));
        }

        // - - - - - - - - - - - - - - - FIVE - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tFIVE: bdem_List(originalRow, am, ba);" << endl;
        {
            bdem_List mZ(S3); const bdem_List& Z = mZ; // non-default mode
            mZ.appendDouble(A05);

            bslma_TestAllocator a;
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4);  // to handle
                                                    // BDEM_SUBORDINATE

            bdem_List  mL(Z.row(),     &a);  const bdem_List& L  = mL;
            bdem_List mL1(Z.row(), S1, &a1); const bdem_List& L1 = mL1;
            bdem_List mL2(Z.row(), S2, &a2); const bdem_List& L2 = mL2;
            bdem_List mL3(Z.row(), S3, &a3); const bdem_List& L3 = mL3;
            bdem_List *p = new(ma4) bdem_List(Z.row(), S4, &ma4);
            bdem_List& mL4 = *p;    const bdem_List& L4 = mL4;
            // Note that if BDEM_SUBORDINATE were the default, it would leek
            // memory, which would be reported by the bslma_TestAllocator a4.

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            int use, use1, use2, use3, use4;

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

             mL.appendStringArray(A16);
            mL1.appendStringArray(A16);
            mL2.appendStringArray(A16);
            mL3.appendStringArray(A16);
            mL4.appendStringArray(A16);

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(use1 == use);
            ASSERT(use2 != use); ASSERT(use2 != use3);  ASSERT(use2 != use4)
            ASSERT(use3 != use); ASSERT(use3 != use4);
            ASSERT(use4 != use);

            ASSERT(2 == L.length());    // Confirm value.
            ASSERT(A05 == L.theDouble(0));
            ASSERT(A16 == L.theStringArray(1));
        }

        // - - - - - - - - - - - - - - - SIX/SEVEN - - - - - - - - - - - - - -

        if (verbose) cout << "\tSIX/SEVEN: bdem_List(et, am, ba);" <<endl;
        {
            bsl::vector<bdem_ElemType::Type> TYPES;
            TYPES.push_back(bdem_ElemType::BDEM_INT);

            bslma_TestAllocator a;
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator a3;
            bslma_TestAllocator a4;
            bdema_MultipoolAllocator ma4(20, &a4);  // to handle
                                                    // BDEM_SUBORDINATE

            bdem_List  mL(TYPES,     &a);  const bdem_List& L  = mL;
            bdem_List mL1(TYPES, S1, &a1); const bdem_List& L1 = mL1;
            bdem_List mL2(TYPES, S2, &a2); const bdem_List& L2 = mL2;
            bdem_List mL3(TYPES, S3, &a3); const bdem_List& L3 = mL3;
            bdem_List *p = new(ma4) bdem_List(TYPES, S4, &ma4);
            bdem_List& mL4 = *p;    const bdem_List& L4 = mL4;
            // Note that if BDEM_SUBORDINATE were the default, it would leek
            // memory, which would be reported by the bslma_TestAllocator a4.

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            int use, use1, use2, use3, use4;

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

             mL.appendStringArray(A16);
            mL1.appendStringArray(A16);
            mL2.appendStringArray(A16);
            mL3.appendStringArray(A16);
            mL4.appendStringArray(A16);

            ASSERT(L == L1); ASSERT(L == L2); ASSERT(L == L3); ASSERT(L == L4);

            use  =  a.numBytesInUse();
            use1 = a1.numBytesInUse();
            use2 = a2.numBytesInUse();
            use3 = a3.numBytesInUse();
            use4 = a4.numBytesInUse();

            if (veryVerbose) {T_ T_ P_(use) P_(use1) P_(use2) P_(use3) P(use4)}

            ASSERT(use1 == use);
            ASSERT(use2 != use); ASSERT(use2 != use3);  ASSERT(use2 != use4)
            ASSERT(use3 != use); ASSERT(use3 != use4);
            ASSERT(use4 != use);

            ASSERT(2 == L.length());    // Confirm value.
            ASSERT(N02 == L.theInt(0));
            ASSERT(A16 == L.theStringArray(1));
        }
}

static void testCase10(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //   Ensure that a 'bdem_List' can convert itself to/from byte stream.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //
        // Testing:
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BDEX STREAMING" << endl
                                  << "==============" << endl;
        const int MAX_VERSION = 3;
        {
            ASSERT(MAX_VERSION == Obj::maxSupportedBdexVersion());
        }

        // version    Out      In
        // -------    -----    -----
        //    1       unset    unset
        //            null     unset  (so * -> @ below)
        //
        //    2       unset    null   (so @ -> * below)
        //            null     null
        //
        //    3       unset    unset
        //            null     null

        const struct {
            const int   d_line;    // line
            const char *d_spec_p;  // list spec
        } DATA[] = {
               // Line          Spec
               // ----          ----
            {      L_,          ""                                     },

            // One element
            {      L_,          "A"                                    },
            {      L_,          "C"                                    },
            {      L_,          "F"                                    },
            {      L_,          "G"                                    },
            {      L_,          "O"                                    },
            {      L_,          "Q"                                    },
            {      L_,          "R"                                    },
            {      L_,          "U"                                    },
            {      L_,          "V"                                    },
            {      L_,          "W"                                    },
            {      L_,          "b"                                    },
            {      L_,          "e"                                    },
            {      L_,          "f"                                    },
            {      L_,          "B@",                                  },
            {      L_,          "B*",                                  },
            {      L_,          "D*",                                  },
            {      L_,          "U*",                                  },
            {      L_,          "V@",                                  },
            {      L_,          "e@",                                  },
            {      L_,          "V*",                                  },
            {      L_,          "e*",                                  },
            {      L_,          "f*",                                  },

            // Two elements
            {      L_,          "AB"                                   },
            {      L_,          "CD"                                   },
            {      L_,          "EF"                                   },
            {      L_,          "GH"                                   },
            {      L_,          "OM"                                   },
            {      L_,          "QP"                                   },
            {      L_,          "RS"                                   },
            {      L_,          "UT"                                   },
            {      L_,          "VZ"                                   },
            {      L_,          "Wc"                                   },
            {      L_,          "ab"                                   },
            {      L_,          "ef"                                   },
            {      L_,          "A*B"                                  },
            {      L_,          "CD@"                                  },
            {      L_,          "U@T"                                  },
            {      L_,          "CD*"                                  },
            {      L_,          "U*T"                                  },
            {      L_,          "V*Z"                                  },
            {      L_,          "U*T*"                                 },
            {      L_,          "V*Z@"                                 },
            {      L_,          "a@b@"                                 },
            {      L_,          "V*Z*"                                 },
            {      L_,          "a*b*"                                 },
            {      L_,          "e*f*"                                 },

            // Three elements
            {      L_,          "ABa"                                  },
            {      L_,          "CDd"                                  },
            {      L_,          "EFA"                                  },
            {      L_,          "GHF"                                  },
            {      L_,          "OMW"                                  },
            {      L_,          "QPQ"                                  },
            {      L_,          "RSJ"                                  },
            {      L_,          "UTK"                                  },
            {      L_,          "VZb"                                  },
            {      L_,          "WcA"                                  },
            {      L_,          "abD"                                  },
            {      L_,          "efE"                                  },
            {      L_,          "a@bD"                                 },
            {      L_,          "a*bD"                                 },
            {      L_,          "e*f@E"                                },
            {      L_,          "U*T*K@"                               },
            {      L_,          "V@Z@b*"                               },
            {      L_,          "e*f*E"                                },
            {      L_,          "U*T*K*"                               },
            {      L_,          "V*Z*b*"                               },

            {      L_,        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"       },
            {      L_,        "ABCD@EFG*HIJK*LMNOPQRSTUVWXYZa*bcde@f*" },
            {      L_,        "ABCDEFG*HIJK*LMNOPQRSTUVWXYZa*bcde*f*"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        Obj mA;  const Obj& A = mA;
        loadReferenceA(&mA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec_p;

            for (int version = 1; version < 4; ++version) {

                Obj mX = g(SPEC, A);  const Obj& X = mX;
                bdex_TestOutStream out;
                X.bdexStreamOut(out, version);
                LOOP2_ASSERT(LINE, version, out);

                if (veryVerbose) {
                    P(LINE) P(version) P(X)
                }

                bdex_TestInStream in(out.data(), out.length());
                LOOP2_ASSERT(LINE, version, in);
                LOOP2_ASSERT(LINE, version, !in.isEmpty());
                in.setSuppressVersionCheck(1);

                Obj mY;  const Obj& Y = mY;
                mY.bdexStreamIn(in, version);
                LOOP2_ASSERT(LINE, version, in);

                if (veryVerbose) {
                    P(Y)
                }

                bsl::string altSpec(SPEC);
                bsl::for_each(altSpec.begin(),
                              altSpec.end(),
                              BdexHelper<bsl::string::value_type>(version));

                Obj mE = g(altSpec.c_str(), A);  const Obj& E = mE;
                LOOP5_ASSERT(LINE, version, X, Y, E, E == Y);
            }
        }
}

static void testCase9(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR: '='
        //   Ensure that assignment is "wired-up" properly.
        //
        // Concerns:
        //   - That the assigned object has the same value.
        //   - That neither the allocator nor allocation strategy is copied.
        //   - That it works with a const bdem_List or const bdem_Row argument.
        //   - That there are no memory leaks.
        //   - That aliasing isn't a problem.
        //
        // Plan:
        //  For each of a small set of lists constructed with a test allocator
        //  and a specific strategy, ensure that the assigned instance has the
        //  same value as the source, and that neither its allocator nor its
        //  strategy is modified.  The general approach is to use the
        //  'bslma_TestAllocator' to verify that subsequent allocations from
        //  assigned object are (1) from its original allocator and (2) that
        //  the allocation pattern exactly matches a similar list explicitly
        //  constructed with that strategy.  Verify the value aspects on
        //  various initial values including the following:
        //   - The empty list.
        //   - Each of the 32 one-element lists.
        //   - Various combinations of two-element lists.
        //   - A list consisting of all 32 element types.
        //  Specifically make sure that we can assign a long list to each
        //  of the short lists and vice versa.  Make sure the we try a few
        //  aliasing examples.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   bdem_List& operator=(const bdem_List& rhs);
        //   bdem_List& operator=(const bdem_Row& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ASSIGNMENT OPERATOR" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nVerify Allocators aren't modified." << endl;
        {
            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bdem_List list1(&a1);
            bdem_List list2(&a2);

            const int USAGE_1 = a1.numBlocksTotal();
            const int USAGE_2 = a2.numBlocksTotal();
            ASSERT(USAGE_1 == USAGE_2);

            if (veryVerbose) { P_(list1) P(list2); }

            list2 = list1;

            list2.appendIntArray(A12);

            ASSERT(USAGE_1 == a1.numBlocksTotal());
            ASSERT(USAGE_2  < a2.numBlocksTotal());
        }

        if (verbose) cout << "\nVerify Strategies aren't modified." << endl;
        {
            const bdem_AggregateOption::AllocationStrategy S1 =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;
            const bdem_AggregateOption::AllocationStrategy S2 =
                                       bdem_AggregateOption::BDEM_WRITE_ONCE;

            if (verbose) cout << "\tCreate 4 test allocators." << endl;

            bslma_TestAllocator a1;
            bslma_TestAllocator a2;
            bslma_TestAllocator aX;
            bslma_TestAllocator aY;

            int usage1 = a1.numBytesTotal();
            int usage2 = a2.numBytesTotal();
            int usageX = aX.numBytesTotal();
            int usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose)
                cout << "\tCreate 4 lists where 1 is BDEM_PASS_THROUGH."
                     << endl;
            bdem_List list1(S1, &a1);   // PASS THROUGH
            bdem_List list2(S2, &a2);
            bdem_List listX(S2, &aX);
            bdem_List listY(S2, &aY);

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(listX == list1); ASSERT(listY == listX);
            ASSERT(listX == list2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAssign from different and same strategies."
                                                                       << endl;
            listX = list1;      // Assign X from strategy 1 (different).
            listY = list2;      // Assign Y from strategy 2 (same).

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)
                                  P(listX) }

            ASSERT(listX == list1); ASSERT(listY == listX);
            ASSERT(listX == list2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAppend a string array to each list."
                                                                       << endl;
            list1.appendStringArray(A16); list2.appendStringArray(A16);
            listX.appendStringArray(A16); listY.appendStringArray(A16);

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(listX == list1); ASSERT(listY == listX);
            ASSERT(listX == list2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAssign from different and same strategies."
                                                                       << endl;
            listX = list1;      // Assign X from strategy 1 (different).
            listY = list2;      // Assign Y from strategy 2 (same).

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(listX == list1); ASSERT(listY == listX);
            ASSERT(listX == list2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAppend a list to each list." << endl;

            list1.appendList(A20); list2.appendList(A20);
            listX.appendList(A20); listY.appendList(A20);

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(listX == list1); ASSERT(listY == listX);
            ASSERT(listX == list2); ASSERT(usageY == usageX);

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAssign from different and same strategies."
                                                                       << endl;
            listX = list1;      // Assign X from strategy 1 (different).
            listY = list2;      // Assign Y from strategy 2 (same).

            usage1 = a1.numBytesTotal();
            usage2 = a2.numBytesTotal();
            usageX = aX.numBytesTotal();
            usageY = aY.numBytesTotal();

            if (veryVerbose) {T_ T_ P_(usage1) P_(usage2) P_(usageX) P(usageY)}

            ASSERT(listX == list1); ASSERT(listY == listX);
            ASSERT(listX == list2); ASSERT(usageY == usageX);
        }

        bdem_List mL; const bdem_List& L = mL;  ASSERT( 0 == L.length());

        mL.appendChar(A00);                     ASSERT( 1 == L.length());
        mL.appendShort(A01);                    ASSERT( 2 == L.length());
        mL.appendInt(A02);                      ASSERT( 3 == L.length());
        mL.appendInt64(A03);                    ASSERT( 4 == L.length());
        mL.appendFloat(A04);                    ASSERT( 5 == L.length());
        mL.appendDouble(A05);                   ASSERT( 6 == L.length());
        mL.appendString(A06);                   ASSERT( 7 == L.length());
        mL.appendDatetime(A07);                 ASSERT( 8 == L.length());
        mL.appendDate(A08);                     ASSERT( 9 == L.length());
        mL.appendTime(A09);                     ASSERT(10 == L.length());

        mL.appendCharArray(A10);                ASSERT(11 == L.length());
        mL.appendShortArray(A11);               ASSERT(12 == L.length());
        mL.appendIntArray(A12);                 ASSERT(13 == L.length());
        mL.appendInt64Array(A13);               ASSERT(14 == L.length());
        mL.appendFloatArray(A14);               ASSERT(15 == L.length());
        mL.appendDoubleArray(A15);              ASSERT(16 == L.length());
        mL.appendStringArray(A16);              ASSERT(17 == L.length());
        mL.appendDatetimeArray(A17);            ASSERT(18 == L.length());
        mL.appendDateArray(A18);                ASSERT(19 == L.length());
        mL.appendTimeArray(A19);                ASSERT(20 == L.length());

        mL.appendList(A20);                     ASSERT(21 == L.length());
        mL.appendTable(A21);                    ASSERT(22 == L.length());

        mL.appendBool(A22);                     ASSERT(23 == L.length());
        mL.appendDatetimeTz(A23);               ASSERT(24 == L.length());
        mL.appendDateTz(A24);                   ASSERT(25 == L.length());
        mL.appendTimeTz(A25);                   ASSERT(26 == L.length());
        mL.appendBoolArray(A26);                ASSERT(27 == L.length());
        mL.appendDatetimeTzArray(A27);          ASSERT(28 == L.length());
        mL.appendDateTzArray(A28);              ASSERT(29 == L.length());
        mL.appendTimeTzArray(A29);              ASSERT(30 == L.length());

        mL.appendChoice(A30);                   ASSERT(31 == L.length());
        mL.appendChoiceArray(A31);              ASSERT(32 == L.length());

        mL.appendChar(B00);                     ASSERT(33 == L.length());
        mL.appendShort(B01);                    ASSERT(34 == L.length());
        mL.appendInt(B02);                      ASSERT(35 == L.length());
        mL.appendInt64(B03);                    ASSERT(36 == L.length());
        mL.appendFloat(B04);                    ASSERT(37 == L.length());
        mL.appendDouble(B05);                   ASSERT(38 == L.length());
        mL.appendString(B06);                   ASSERT(39 == L.length());
        mL.appendDatetime(B07);                 ASSERT(40 == L.length());
        mL.appendDate(B08);                     ASSERT(41 == L.length());
        mL.appendTime(B09);                     ASSERT(42 == L.length());

        mL.appendCharArray(B10);                ASSERT(43 == L.length());
        mL.appendShortArray(B11);               ASSERT(44 == L.length());
        mL.appendIntArray(B12);                 ASSERT(45 == L.length());
        mL.appendInt64Array(B13);               ASSERT(46 == L.length());
        mL.appendFloatArray(B14);               ASSERT(47 == L.length());
        mL.appendDoubleArray(B15);              ASSERT(48 == L.length());
        mL.appendStringArray(B16);              ASSERT(49 == L.length());
        mL.appendDatetimeArray(B17);            ASSERT(50 == L.length());
        mL.appendDateArray(B18);                ASSERT(51 == L.length());
        mL.appendTimeArray(B19);                ASSERT(52 == L.length());

        mL.appendList(B20);                     ASSERT(53 == L.length());
        mL.appendTable(B21);                    ASSERT(54 == L.length());

        mL.appendBool(B22);                     ASSERT(55 == L.length());
        mL.appendDatetimeTz(B23);               ASSERT(56 == L.length());
        mL.appendDateTz(B24);                   ASSERT(57 == L.length());
        mL.appendTimeTz(B25);                   ASSERT(58 == L.length());
        mL.appendBoolArray(B26);                ASSERT(59 == L.length());
        mL.appendDatetimeTzArray(B27);          ASSERT(60 == L.length());
        mL.appendDateTzArray(B28);              ASSERT(61 == L.length());
        mL.appendTimeTzArray(B29);              ASSERT(62 == L.length());

        mL.appendChoice(B30);                   ASSERT(63 == L.length());
        mL.appendChoiceArray(B31);              ASSERT(64 == L.length());

        if (verbose) cout <<
             "\nVerify various lengths and all types are copied." << endl;
        {
            // This list will grow on each iteration:
            bdem_List mZ; const bdem_List& Z = mZ;
            const int LEN = 44;
            ASSERT(LEN <= L.length());

            for (int i = 0; i < LEN; ++i) {
                // increasingly long list
                mZ.appendElement(L[i]);

                bdem_List x0;       // empty lists

                bdem_List x1;       // one-element lists
                x1.appendElement(L[(i + 1) % LEN]);

                bdem_List x2;       // two-element lists
                x2.appendElement(L[i]);
                x2.appendElement(L[(i + 2) % LEN]);

                if (veryVerbose) { P_(x0) P_(x1) P(x2) }

                bdem_List Y0(x0);   // Keep these as "controls".
                bdem_List Y1(x1);
                bdem_List Y2(x2);

                LOOP_ASSERT(i, Y0 == x0);
                LOOP_ASSERT(i, Y1 == x1);
                LOOP_ASSERT(i, Y2 == x2);

                LOOP_ASSERT(i, Z != x0);
                LOOP_ASSERT(i, Z != x1);
                LOOP_ASSERT(i, Z != x2);

                x0 = Z;             // Assign long values
                x1 = Z;
                x2 = Z;

                if (veryVeryVerbose) { P(x0) P(x1) P(x2) Q(=============) }

                LOOP_ASSERT(i, Y0 != x0);
                LOOP_ASSERT(i, Y1 != x1);
                LOOP_ASSERT(i, Y2 != x2);

                LOOP_ASSERT(i, Z == x0);
                LOOP_ASSERT(i, Z == x1);
                LOOP_ASSERT(i, Z == x2);

                x0 = Y0.row();      // Resore original (short) values
                x1 = Y1.row();      // via a 'const bdem_Row'.
                x2 = Y2.row();

                LOOP_ASSERT(i, Y0 == x0);
                LOOP_ASSERT(i, Y1 == x1);
                LOOP_ASSERT(i, Y2 == x2);

                LOOP_ASSERT(i, Z != x0);
                LOOP_ASSERT(i, Z != x1);
                LOOP_ASSERT(i, Z != x2);
            }
        }

        if (verbose) cout <<
             "\nVerify various lengths and all types and null info is copied."
                     << endl;
        {
            // This list will grow on each iteration:
            bdem_List mZ; const bdem_List& Z = mZ;
            const int LEN = 44;
            ASSERT(LEN <= L.length());

            for (int i = 0; i < LEN; ++i) {
                // increasingly long list
                mZ.appendElement(L[i]);
                mZ[i].makeNull();

                bdem_List x0;       // empty lists

                bdem_List x1;       // one-element lists
                x1.appendElement(L[(i + 1) % LEN]);

                bdem_List x2;       // two-element lists
                x2.appendElement(L[i]);
                x2.appendElement(L[(i + 2) % LEN]);

                if (veryVerbose) { P_(x0) P_(x1) P(x2) }

                bdem_List Y0(x0);   // Keep these as "controls".
                bdem_List Y1(x1);
                bdem_List Y2(x2);

                LOOP_ASSERT(i, Y0 == x0);
                LOOP_ASSERT(i, Y1 == x1);
                LOOP_ASSERT(i, Y2 == x2);

                LOOP_ASSERT(i, Z != x0);
                LOOP_ASSERT(i, Z != x1);
                LOOP_ASSERT(i, Z != x2);

                x0 = Z;             // Assign long values
                x1 = Z;
                x2 = Z;

                if (veryVeryVerbose) { P(x0) P(x1) P(x2) Q(=============) }

                LOOP_ASSERT(i, Y0 != x0);
                LOOP_ASSERT(i, Y1 != x1);
                LOOP_ASSERT(i, Y2 != x2);

                LOOP_ASSERT(i, Z == x0);
                LOOP_ASSERT(i, Z == x1);
                LOOP_ASSERT(i, Z == x2);

                x0 = Y0.row();      // Resore original (short) values
                x1 = Y1.row();      // via a 'const bdem_Row'.
                x2 = Y2.row();

                LOOP_ASSERT(i, Y0 == x0);
                LOOP_ASSERT(i, Y1 == x1);
                LOOP_ASSERT(i, Y2 == x2);

                LOOP_ASSERT(i, Z != x0);
                LOOP_ASSERT(i, Z != x1);
                LOOP_ASSERT(i, Z != x2);

                mZ[i].replaceValue(L[i]);
            }

            if (verbose) cout << "\nTry some examples with aliasing" << endl;
            {
                bdem_List mA; const bdem_List& A = mA;ASSERT( 0 == A.length());
                loadReferenceA(&mA);                  ASSERT(32 == A.length());

                bdem_List x;
                x = x;
                ASSERT(bdem_List() == x);

                bdem_List y(A);
                y = y;
                ASSERT(A == y);
            }
        }
}

static void testCase8(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TEST GENERATOR FUNCTIONS
        //   Verify the list generator function g.
        //
        // Concerns:
        //   - That all 32 values select the correct list item.
        //   - That common whitespace is ignored { space, tab, newline }.
        //
        // Plan:
        //   - Create a list of 32 items and select each unique item out
        //      of it and verify that the lists are the same.
        //   - Create a similar list with whitespace between each active char.
        //   - Create an empty list (empty spec string); verify length.
        //   - Create another couple of lists and verify them.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdem_List g(const char *spec, const bdem_List& referenceList)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST GENERATOR FUNCTIONS" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nCreate a list of 32 unique elements." << endl;

        bdem_List mL; const bdem_List& L = mL;  ASSERT( 0 == L.length());
        loadReferenceA(&mL);                    ASSERT(32 == L.length());

        if (verbose) cout << "\tCreate an equivalent list using spec." << endl;
        {
            const bdem_List RESULT = g("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef", L);
            if (veryVerbose) {
                cout << "\t\tL: ";           L.print(cout, -3, 4);
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(L == RESULT);
        }

        if (verbose) cout << "\tCreate a list with unset and nulls." << endl;
        {
            const char      *SPEC       = "A@B@C*D*c@d@e*f*";
            const int        LEN        = strlen(SPEC);
            const bdem_List  RESULT     = g(SPEC, L);
            const char       UNSET_CHAR = '@';
            const char       NULL_CHAR  = '*';

            if (veryVerbose) {
                cout << "\t\tL: ";           L.print(cout, -3, 4);
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }

            for (int i = 0, j = 0; i < LEN; i += 2, ++j) {
                ET::Type TYPE = getElemType(SPEC[i]);
                LOOP3_ASSERT(j, TYPE, RESULT[j].type(),
                             TYPE == RESULT[j].type());
                if (UNSET_CHAR == SPEC[i + 1]) {
                    LOOP_ASSERT(i, RESULT[j].isNonNull());
                    LOOP_ASSERT(i, isUnset(RESULT[j]));
                }
                else if (NULL_CHAR == SPEC[i + 1]) {
                    LOOP_ASSERT(i, RESULT[j].isNull());
                }
            }
        }

        if (verbose) cout << "\tRepeat with whitespace." << endl;
        {
            const bdem_List RESULT = g(
                " A B \t\t C \t\t D E \t\n"
                "  F  G  H  I  J   K   \tL"
                "\t\t\tM\n\n\n\tN\tO  P\tQ"
                " R S T U V W X Y \tZ \t\n"
                " a   \n\t b cd\n e f", L);
            if (veryVerbose) {
                cout << "\t\tL: ";           L.print(cout, -3, 4);
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(L == RESULT);
        }

        if (verbose) cout << "\tCreate an empty list using spec." << endl;
        {
            const bdem_List RESULT = g("", L);
            if (veryVerbose) {
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(0 == RESULT.length())
        }

        if (verbose) cout << "\tCreate a short list using spec." << endl;
        {
            const bdem_List RESULT = g("dAdA", L);
            if (veryVerbose) {
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(4 == RESULT.length())
            ASSERT(A29 == RESULT[0].theTimeTzArray())
            ASSERT(A00 == RESULT[1].theChar())
            ASSERT(A29 == RESULT[0].theTimeTzArray())
            ASSERT(A00 == RESULT[1].theChar())
        }

        if (verbose) cout << "\tCreate another short list using spec." << endl;
        {
            const bdem_List RESULT = g("JIHBCE", L);
            if (veryVerbose) {
                cout << "\t\tRESULT: "; RESULT.print(cout, -3, 4);
            }
            ASSERT(6 == RESULT.length())
            ASSERT(A09 == RESULT[0].theTime())
            ASSERT(A08 == RESULT[1].theDate())
            ASSERT(A07 == RESULT[2].theDatetime())

            ASSERT(A01 == RESULT[3].theShort())
            ASSERT(A02 == RESULT[4].theInt())
            ASSERT(A04 == RESULT[5].theFloat())
        }
}

static void testCase7(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTION
        //   Ensure that this constructor is "wired-up" and defaults properly.
        //
        // Concerns:
        //   - That neither the allocator nor allocation strategy is copied.
        //   - That the current default allocator is uses by default.
        //   - That when an allocator is specified, net default allocator
        //      usage between operations is zero.
        //   - That the copy-constructed object has the same value.
        //   - That it works with a const bdem_List or const bdem_Row argument.
        //
        // Plan:
        //   - To ensure that the allocator isn't copied and that the current
        //      default allocator is used, start by installing a test allocator
        //      as the default.  Create lists from both non-modifiable and
        //      modifiable lists installed explicitly with a separate test
        //      allocator; append to the copied lists and observer that
        //      default and not the explicitly specified allocator was used.
        //   - To ensure that the default strategy is used by creating 3
        //      separate lists, each created with an explicit strategy.  Create
        //      four additional lists, each with its own test allocator.
        //      Three of these lists are copy constructed from a different
        //      original list with the corresponding strategy specified
        //      explicitly; the fourth is copied from an original with a
        //      non-default strategy, but its own is left unspecified.
        //      After appending the same value to each of the four lists,
        //      respective usages are compared to ensure that the fourth
        //      conforms to the default and not the other two.  Periodically
        //      confirm that the default allocator net usage is zero.
        //   - Ensure that the copy constructed value is correct using:
        //      + The empty list.
        //      + Each of the 32 one-element lists.
        //      + Other variations including 2 and 3 element lists, (some
        //         applied to a const 'bdem_Row').
        //   - Note that this final test also validates 'row() const'.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   bdem_List(const bdem_List& original, ba);
        //   bdem_List(const bdem_Row& original, ba);
        //
        //   const bdem_Row& row() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Copy Construction" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nVerify Allocators aren't copied." << endl;
        {
            if (verbose) cout << "\tFirst create initial objects." << endl;
            bslma_TestAllocator da;  // Must be created first!

            bslma_TestAllocator ta0;  // used with initial list L
            bslma_TestAllocator ta1;  // used to copy construct list1a
            bslma_TestAllocator ta2;  // used to copy construct list2a

            if (verbose) cout <<
                "\tCreate/install a test allocator as the default." << endl;

            const bslma_DefaultAllocatorGuard dag(&da);

            if (verbose) cout << "\tConstruct an initial list L." << endl;

            ASSERT(0 == ta0.numBlocksTotal());

            if (verbose) cout <<
                "\tCopy construct from the empty initial list." << endl;

            bdem_List L(&ta0);

            const int TA0_USAGE_FINAL = ta0.numBlocksTotal();

            if (verbose) cout <<
               "\tCopy empty lists without/with supplying an allocator."<<endl;

            ASSERT(0 == da.numBlocksTotal());
            bdem_List list1(L);
            const int DA_USAGE_1 = da.numBlocksTotal();
            ASSERT(0 < DA_USAGE_1);
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(L == list1);

            ASSERT(0 == ta1.numBlocksTotal());
            bdem_List list1a(L, &ta1);
            const int TA1_USAGE_1 = ta1.numBlocksTotal();
            ASSERT(0 < TA1_USAGE_1);
            ASSERT(DA_USAGE_1 == da.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(L == list1a);

            if (verbose) cout <<
               "\t\tVerify gross/net allocations with/without are same."
                                                                       << endl;
            {
                // Gross and net allocations from 'ta1' and 'da' should be eq.
                const int daByInUse = da.numBytesInUse();
                const int daBkInUse = da.numBlocksInUse();
                const int daByTotal = da.numBytesTotal();
                const int daBkTotal = da.numBlocksTotal();

                const int ta1ByInUse = ta1.numBytesInUse();
                const int ta1BkInUse = ta1.numBlocksInUse();
                const int ta1ByTotal = ta1.numBytesTotal();
                const int ta1BkTotal = ta1.numBlocksTotal();

                if (veryVerbose) { P_(daByInUse) P_(daBkInUse)
                                   P_(daByTotal)  P(daBkTotal) }

                LOOP2_ASSERT(ta1ByInUse, daByInUse, ta1ByInUse == daByInUse);
                LOOP2_ASSERT(ta1BkInUse, daBkInUse, ta1BkInUse == daBkInUse);
                LOOP2_ASSERT(ta1ByTotal, daByTotal, ta1ByTotal == daByTotal);
                LOOP2_ASSERT(ta1BkTotal, daBkTotal, ta1BkTotal == daBkTotal);
            }

            if (verbose) cout << "\tAppend to each list." << endl;

            ASSERT(DA_USAGE_1 == da.numBlocksTotal());
            list1.appendString(A06);
            const int DA_USAGE_2 = da.numBlocksTotal();
            ASSERT(DA_USAGE_1 < DA_USAGE_2);
            ASSERT(TA1_USAGE_1 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(list1a != list1);

            ASSERT(TA1_USAGE_1 == ta1.numBlocksTotal());
            list1a.appendString(A06);
            const int TA1_USAGE_2 = ta1.numBlocksTotal();
            ASSERT(TA1_USAGE_1 < TA1_USAGE_2);
            ASSERT(DA_USAGE_2 == da.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(list1a == list1);

            if (verbose) cout <<
                "\t\tVerify gross/net allocations with/without are still same."
                                                                       << endl;
            {
                // Gross and net allocations from 'ta1' and 'da' should be eq.
                const int daByInUse = da.numBytesInUse();
                const int daBkInUse = da.numBlocksInUse();
                const int daByTotal = da.numBytesTotal();
                const int daBkTotal = da.numBlocksTotal();

                const int ta1ByInUse = ta1.numBytesInUse();
                const int ta1BkInUse = ta1.numBlocksInUse();
                const int ta1ByTotal = ta1.numBytesTotal();
                const int ta1BkTotal = ta1.numBlocksTotal();

                if (veryVerbose) { P_(daByInUse) P_(daBkInUse)
                                   P_(daByTotal)  P(daBkTotal) }

                LOOP2_ASSERT(ta1ByInUse, daByInUse, ta1ByInUse == daByInUse);
                LOOP2_ASSERT(ta1BkInUse, daBkInUse, ta1BkInUse == daBkInUse);
                LOOP2_ASSERT(ta1ByTotal, daByTotal, ta1ByTotal == daByTotal);
                LOOP2_ASSERT(ta1BkTotal, daBkTotal, ta1BkTotal == daBkTotal);
            }

            if (verbose) cout <<
                "\tCopy non-empty lists without/with supplying an allocator."
                                                                      << endl;
            ASSERT(0 == ta2.numBlocksTotal());
            bdem_List list2a(list1a, &ta2);
            const int TA2_USAGE_1 = ta2.numBlocksTotal();
            ASSERT(0 < TA2_USAGE_1);
            ASSERT(DA_USAGE_2 == da.numBlocksTotal());
            ASSERT(TA1_USAGE_2 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(list2a == list1a);

            ASSERT(DA_USAGE_2 == da.numBlocksTotal());
            bdem_List list2(list1);
            const int DA_USAGE_3 = da.numBlocksTotal();
            ASSERT(DA_USAGE_2 < DA_USAGE_3);
            ASSERT(TA2_USAGE_1 == ta2.numBlocksTotal());
            ASSERT(TA1_USAGE_2 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(list2a == list2);

            if (verbose) cout << "\tAppend to each copied list." << endl;

            ASSERT(TA2_USAGE_1 == ta2.numBlocksTotal());
            list2a.appendStringArray(A16);
            const int TA2_USAGE_2 = ta2.numBlocksTotal();
            ASSERT(TA2_USAGE_1 < TA2_USAGE_2);
            ASSERT(DA_USAGE_3 == da.numBlocksTotal());
            ASSERT(TA1_USAGE_2 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(list2a != list2);

            ASSERT(DA_USAGE_3 == da.numBlocksTotal());
            list2.appendStringArray(A16);
            const int DA_USAGE_4 = da.numBlocksTotal();
            ASSERT(DA_USAGE_3 < DA_USAGE_4);
            ASSERT(TA2_USAGE_2 == ta2.numBlocksTotal());
            ASSERT(TA1_USAGE_2 == ta1.numBlocksTotal());
            ASSERT(TA0_USAGE_FINAL == ta0.numBlocksTotal());

            ASSERT(list2a == list2);
        }

        if (verbose) cout << "\nVerify Strategies aren't copied." << endl;
        {
            if (verbose) cout << "\tFirst create allocators." << endl;
            bslma_TestAllocator da;  // will be default - must be created first

            bslma_TestAllocator taA;  // A: BDEM_PASS_THROUGH
            bslma_TestAllocator taB;  // B: BDEM_WRITE_ONCE
            bslma_TestAllocator taC;  // C: BDEM_WRITE_MANY
            bslma_TestAllocator taX;  // D: UNSPECIFIED

            bslma_TestAllocator ta;   // Used to create original lists.

            if (verbose) cout <<
              "\tNow create/install a test allocator as the default." << endl;

            const bslma_DefaultAllocatorGuard dag(&da);

            if (verbose) cout <<
                "\tNext initial lists with explicit strategies." << endl;

            const bdem_AggregateOption::AllocationStrategy SA =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;
            const bdem_AggregateOption::AllocationStrategy SB =
                                       bdem_AggregateOption::BDEM_WRITE_ONCE;
            const bdem_AggregateOption::AllocationStrategy SC =
                                       bdem_AggregateOption::BDEM_WRITE_MANY;

            ASSERT(0 == da.numBytesInUse());
            ASSERT(0 == ta.numBytesTotal());

            bdem_List listAA(SA, &ta);
            bdem_List listBB(SB, &ta);
            bdem_List listCC(SC, &ta);

            const int INITIAL_USAGE = ta.numBytesTotal();

            ASSERT(0 == da.numBytesInUse());
            ASSERT(0 <  INITIAL_USAGE);

            LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            if (verbose) cout << "\tNow create the four copied lists." << endl;

            bdem_List listA(listAA, SA, &taA);
            bdem_List listB(listBB, SB, &taB);
            bdem_List listC(listCC, SC, &taC);
            bdem_List listX(listBB,     &taX);         // should act like listA

                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            ASSERT(0 == da.numBytesTotal());
            ASSERT(INITIAL_USAGE == ta.numBytesTotal());

            int usageA, usageB, usageC, usageX;

            usageA = taA.numBytesTotal();
            usageB = taB.numBytesTotal();
            usageC = taC.numBytesTotal();
            usageX = taX.numBytesTotal();          // Should be same as usageA.

            if (veryVerbose) { P_(usageA) P_(usageB) P_(usageC) P(usageX) }

            if (verbose) cout << "\t\tAppend a string." << endl;

                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listA.appendString(A06);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listB.appendString(A06);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listC.appendString(A06);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listX.appendString(A06);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            usageA = taA.numBytesTotal();
            usageB = taB.numBytesTotal();
            usageC = taC.numBytesTotal();
            usageX = taX.numBytesTotal();          // Should be same as usageA.

            if (veryVerbose) { P_(usageA) P_(usageB) P_(usageC) P(usageX) }

            if (verbose) cout << "\t\tAppend an int array." << endl;

                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listA.appendIntArray(A12);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listB.appendIntArray(A12);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listC.appendIntArray(A12);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listX.appendIntArray(A12);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            usageA = taA.numBytesTotal();
            usageB = taB.numBytesTotal();
            usageC = taC.numBytesTotal();
            usageX = taX.numBytesTotal();          // Should be same as usageA.

            if (veryVerbose) { P_(usageA) P_(usageB) P_(usageC) P(usageX) }

            if (verbose) cout << "\t\tAppend a string array." << endl;

                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listA.appendStringArray(A16);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listB.appendStringArray(A16);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listC.appendStringArray(A16);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());
            listX.appendStringArray(A16);
                    LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            usageA = taA.numBytesTotal();
            usageB = taB.numBytesTotal();
            usageC = taC.numBytesTotal();
            usageX = taX.numBytesTotal();          // Should be same as usageA.

            if (veryVerbose) { P_(usageA) P_(usageB) P_(usageC) P(usageX) }

            ASSERT(listA == listX);
            ASSERT(listB == listX);
            ASSERT(listC == listX);

            ASSERT(usageA == usageX);  // if fail: either bad copy or default.
            ASSERT(usageB != usageX);  // if fail: either bad copy or default.
            ASSERT(usageC != usageX);  // if fail: bad default was
                                       // BDEM_WRITE_MANY!

            ASSERT(0 == da.numBytesInUse());
            ASSERT(INITIAL_USAGE == ta.numBytesTotal());
                      LOOP_ASSERT(da.numBytesInUse(), 0 == da.numBytesInUse());

            if (veryVeryVerbose) P(listX);

        } // da is un-installed as default allocator and then leaves scope.

        if (verbose) cout <<
            "\nCreate reference list A/B with 44 unique elements." << endl;

        bdem_List mL; const bdem_List& L = mL;  ASSERT( 0 == L.length());
        loadReferenceA(&mL);                    ASSERT(32 == L.length());
        loadReferenceB(&mL);                    ASSERT(64 == L.length());

        if (verbose) cout <<
             "\tVerify various lengths and all types are copied." << endl;
        {
            // This list will grow on each iteration:
            bdem_List mZ; const bdem_List& Z = mZ;

            for (int i = 0; i < 44; ++i) {
                // increasingly long list
                {
                    mZ.appendElement(L[i]);
                    const bdem_List Y(Z);
                    if (veryVerbose) P(Y)
                    LOOP_ASSERT(i, Z == Y);
                }

                // every type of single element list (twice)
                {
                    bdem_List mX1; const bdem_List& X1 = mX1;
                    mX1.appendElement(L[i]);
                    const bdem_List Y1(X1);
                    if (veryVerbose) P(Y1)
                    LOOP_ASSERT(i, X1 == Y1);
                }

                // pairs of sequential elements
                {
                    bdem_List mX2; const bdem_List& X2 = mX2;
                    mX2.appendElement(L[i]);
                    mX2.appendElement(L[(i + 1) % 44]);
                    const bdem_List Y2(X2);
                    if (veryVerbose) P(Y2)
                    LOOP_ASSERT(i, X2 == Y2);
                }

                // triples of elements: explicitly use const 'bdem_Row'
                {
                    bdem_List mX3; const bdem_List& X3 = mX3;
                    mX3.appendElement(L[i]);
                    mX3.appendElement(L[(i + 2) % 44]);
                    mX3.appendElement(L[(i + 4) % 44]);
                    const bdem_List Y3(X3.row());
                    if (veryVerbose) P(Y3)
                    LOOP3_ASSERT(i, X3, Y3, X3 == Y3);
                }
            }
        }

}

static void testCase6(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS: '==' and '!='
        //   Ensure that these operators correctly interpret value.
        //
        // Concerns:
        //   - That equality is observed.
        //   - That subtle inequality is observed.
        //   - That the allocator is not part of the value.
        //   - That the allocation strategy is not part of the value.
        //
        // Plan:
        //   - Create two reference lists (and verify 'loadReferenceB').
        //   - Verify that we can append an arbitrary element via the
        //      'appendElement' method taking a const 'bdem_ConstElemRef' arg.
        //   - Ensure that the empty lists compare ==.
        //   - Create each type of 1 element list:
        //       + Verify alias case is always ==.
        //       + Verify each is distinct from every other 1-elem list type.
        //       + Verify that changing the value makes them not equal.
        //       + Verify that equal type/values are equal.
        //       + Verify that appending another element makes them not equal.
        //   - Create two similar lists each containing the 32 element types
        //       + Change the value at every position, and verify not equal.
        //       + Note the side effect of testing non-const operator[].
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force and Loop-Based Implementation Techniques
        //
        // Testing:
        //   void loadReferenceB(bdem_List *result);
        //
        //   void appendElement(const bdem_ConstElemRef& srcElement);
        //   bdem_ElemRef operator[](int index);
        //   operator==(const bdem_List& lhs, const bdem_List& rhs);
        //   operator!=(const bdem_List& lhs, const bdem_List& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY OPERATORS '==' and '!='" << endl
                          << "================================" << endl;

        if (verbose) cout <<
            "\nCreate list with each element type." << endl;

        const int N_ELEM = 32;
        bdem_List mL; const bdem_List& L = mL;  ASSERT(     0 == L.length());
        loadReferenceA(&mL);                    ASSERT(N_ELEM == L.length());

        if (verbose) cout << "\tCreate a second identical list." << endl;

        bdem_List mL2; const bdem_List& L2 = mL2; ASSERT(0 == L2.length());

        int i;
        for (i = 0; i < N_ELEM; ++i) {
            mL2.appendElement(L[i]);
            LOOP_ASSERT(i, L[i] == L2[i]);
        }

        ASSERT(N_ELEM == L2.length());

        if (verbose) cout << "\tCreate a list with null elements." << endl;

        bdem_List mLN; const bdem_List& LN = mLN; ASSERT(0 == LN.length());

        // null tests should be larger than int to test boundaries
        loadReferenceA(&mLN);               ASSERT(N_ELEM     == LN.length());
        loadReferenceA(&mLN);               ASSERT(N_ELEM * 2 == LN.length());

        for (i = 0; i < N_ELEM * 2; ++i) {
            LOOP_ASSERT(i, L[i % N_ELEM] == LN[i]);
            mLN[i].makeNull();
            LOOP_ASSERT(i, LN[i].isNull());
            LOOP_ASSERT(i, L[i % N_ELEM] != LN[i]);
        }

        ASSERT(L != LN);

        // Interleaved null and non-null elements
        bdem_List mLN2; const bdem_List& LN2 = mLN2; ASSERT(0 == LN2.length());
        loadReferenceA(&mLN2);              ASSERT(N_ELEM     == LN2.length());
        loadReferenceA(&mLN2);              ASSERT(N_ELEM * 2 == LN2.length());

        for (i = 0; i < N_ELEM * 2; i += 2) {
            LOOP_ASSERT(i, L[i % N_ELEM] == LN2[i]);
            mLN2[i].makeNull();
            LOOP_ASSERT(i, LN2[i].isNull());
            LOOP_ASSERT(i, L[i % N_ELEM] != LN2[i]);
        }

        ASSERT(L != LN2);

        ASSERT(N_ELEM * 2 == LN.length());
        ASSERT(N_ELEM * 2 == LN2.length());

        if (veryVerbose) { P(L); P(L2); P(LN); P(LN2); }

        // single null element
        bdem_List mLN3; const bdem_List& LN3 = mLN3; ASSERT(0 == LN3.length());
        loadReferenceA(&mLN3);                  ASSERT(N_ELEM == LN3.length());

        ASSERT(L == LN3);
        mLN3[0].makeNull();
        ASSERT(L != LN3);

        mLN3 = L;
        ASSERT(L == LN3);
        mLN3[31].makeNull();
        ASSERT(L != LN3);

        if (verbose) cout << "\tUse == and != to verify equality." << endl;
        ASSERT(L == L2);  ASSERT(!(L != L2));

        if (verbose) cout <<
            "\tCreate another different list with each element type." << endl;

        bdem_List mLB; const bdem_List& LB = mLB;  ASSERT( 0 == LB.length());

        mLB.appendChar(B00);                       ASSERT( 1 == LB.length());
        mLB.appendShort(B01);                      ASSERT( 2 == LB.length());
        mLB.appendInt(B02);                        ASSERT( 3 == LB.length());
        mLB.appendInt64(B03);                      ASSERT( 4 == LB.length());
        mLB.appendFloat(B04);                      ASSERT( 5 == LB.length());
        mLB.appendDouble(B05);                     ASSERT( 6 == LB.length());
        mLB.appendString(B06);                     ASSERT( 7 == LB.length());
        mLB.appendDatetime(B07);                   ASSERT( 8 == LB.length());
        mLB.appendDate(B08);                       ASSERT( 9 == LB.length());
        mLB.appendTime(B09);                       ASSERT(10 == LB.length());

        mLB.appendCharArray(B10);                  ASSERT(11 == LB.length());
        mLB.appendShortArray(B11);                 ASSERT(12 == LB.length());
        mLB.appendIntArray(B12);                   ASSERT(13 == LB.length());
        mLB.appendInt64Array(B13);                 ASSERT(14 == LB.length());
        mLB.appendFloatArray(B14);                 ASSERT(15 == LB.length());
        mLB.appendDoubleArray(B15);                ASSERT(16 == LB.length());
        mLB.appendStringArray(B16);                ASSERT(17 == LB.length());
        mLB.appendDatetimeArray(B17);              ASSERT(18 == LB.length());
        mLB.appendDateArray(B18);                  ASSERT(19 == LB.length());
        mLB.appendTimeArray(B19);                  ASSERT(20 == LB.length());

        mLB.appendList(B20);                       ASSERT(21 == LB.length());
        mLB.appendTable(B21);                      ASSERT(22 == LB.length());

        mLB.appendBool(B22);                       ASSERT(23 == LB.length());
        mLB.appendDatetimeTz(B23);                 ASSERT(24 == LB.length());
        mLB.appendDateTz(B24);                     ASSERT(25 == LB.length());
        mLB.appendTimeTz(B25);                     ASSERT(26 == LB.length());
        mLB.appendBoolArray(B26);                  ASSERT(27 == LB.length());
        mLB.appendDatetimeTzArray(B27);            ASSERT(28 == LB.length());
        mLB.appendDateTzArray(B28);                ASSERT(29 == LB.length());
        mLB.appendTimeTzArray(B29);                ASSERT(30 == LB.length());

        mLB.appendChoice(B30);                     ASSERT(31 == LB.length());
        mLB.appendChoiceArray(B31);                ASSERT(32 == LB.length());

        if (veryVerbose) { P(L); P(LB); }

        if (verbose) cout << "\tVerify loadReferenceB." << endl;
        {
            bdem_List mX; const bdem_List& X = mX; ASSERT( 0 == X.length());
            loadReferenceB(&mX);                  ASSERT(N_ELEM == X.length());
            ASSERT(X == LB);
        }

        if (verbose) cout << "\tVerify lists with null elements are equal."
                          << endl;
        {
            bdem_List mX; const bdem_List& X = mX; ASSERT( 0 == X.length());
            loadReferenceB(&mX);              ASSERT(N_ELEM     == X.length());
            loadReferenceB(&mX);              ASSERT(N_ELEM * 2 == X.length());
            LOOP2_ASSERT(X, LN, X != LN);
            for (i = 0; i < 64; ++i) {
                mX[i].makeNull();
            }
            LOOP2_ASSERT(X, LN, X == LN);
        }

        if (verbose) cout << "\tVerify Empty lists are equal." << endl;

        ASSERT(1 == (bdem_List() == bdem_List()));
        ASSERT(0 == (bdem_List() != bdem_List()));

        if (verbose) cout <<
            "\tVerify single-element lists differ for type/value." << endl;

        for (i = 0; i < N_ELEM; ++i) {

            // Verify same type same value is equal.
            bdem_List mX; const bdem_List& X = mX;
            LOOP_ASSERT(i, 1 == (bdem_List() == X));
            LOOP_ASSERT(i, 0 == (bdem_List() != X));
            mX.appendElement(L[i]);
            LOOP_ASSERT(i, 0 == (bdem_List() == X));
            LOOP_ASSERT(i, 1 == (bdem_List() != X));

            // Verify same type different value is not equal.
            bdem_List mZ; const bdem_List& Z = mZ;
            LOOP_ASSERT(i, 1 == (bdem_List() == Z));
            LOOP_ASSERT(i, 0 == (bdem_List() != Z));
            mZ.appendElement(LB[i]);
            LOOP_ASSERT(i, 0 == (bdem_List() == Z));
            LOOP_ASSERT(i, 1 == (bdem_List() != Z));

            if (veryVerbose) { P_(X) P(Z) }

            LOOP_ASSERT(i, 0 == (X == Z));
            LOOP_ASSERT(i, 1 == (X != Z));

            // Verify alias case.
            LOOP_ASSERT(i, 1 == (X == X));
            LOOP_ASSERT(i, 0 == (X != X));

            for (int j = 0; j < N_ELEM; ++j) {

                // Verify equal iff same type (and value).
                bdem_List mY; const bdem_List& Y = mY;
                LOOP2_ASSERT(i, j, bdem_List() == Y);
                mY.appendElement(L2[j]);
                LOOP2_ASSERT(i, j, bdem_List() != Y);

                if (veryVeryVerbose) { T_ P(Y) }

                LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                LOOP2_ASSERT(i, j, (i != j) == (X != Y));
            }
        }

        if (verbose) cout <<
            "\tVerify single-element lists differ for type/null." << endl;

        for (i = 0; i < N_ELEM; ++i) {

            // Verify same type same 'nullness' is equal.
            bdem_List mX; const bdem_List& X = mX;
            LOOP_ASSERT(i, 1 == (bdem_List() == X));
            LOOP_ASSERT(i, 0 == (bdem_List() != X));
            mX.appendElement(L[i]);
            mX[0].makeNull();
            LOOP_ASSERT(i, 0 == (bdem_List() == X));
            LOOP_ASSERT(i, 1 == (bdem_List() != X));

            // Verify same type different 'nullness' is not equal.
            bdem_List mZ; const bdem_List& Z = mZ;
            LOOP_ASSERT(i, 1 == (bdem_List() == Z));
            LOOP_ASSERT(i, 0 == (bdem_List() != Z));
            mZ.appendElement(LB[i]);
            mZ[0].makeNull();
            LOOP_ASSERT(i, 0 == (bdem_List() == Z));
            LOOP_ASSERT(i, 1 == (bdem_List() != Z));

            if (veryVerbose) { P_(X) P(Z) }

            LOOP_ASSERT(i, 1 == (X == Z));
            LOOP_ASSERT(i, 0 == (X != Z));

            // Verify alias case.
            LOOP_ASSERT(i, 1 == (X == X));
            LOOP_ASSERT(i, 0 == (X != X));

            for (int j = 0; j < 32; ++j) {

                // Verify equal iff same type (and value).
                bdem_List mY; const bdem_List& Y = mY;
                LOOP2_ASSERT(i, j, bdem_List() == Y);
                mY.appendElement(L2[j]);
                mY[0].makeNull();
                LOOP2_ASSERT(i, j, bdem_List() != Y);

                if (veryVeryVerbose) { T_ P(Y) }

                LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                LOOP2_ASSERT(i, j, (i != j) == (X != Y));
            }
        }

        if (verbose) cout <<
                "\tcompare lists with just 1 of 32 values different." << endl;
        for (i = 0; i < 30; ++i) {
            LOOP_ASSERT(i, 1 == (L2 == L)); LOOP_ASSERT(i, 0 == (L2 != L));
            mL[i].replaceValue(LB[i]);
            LOOP_ASSERT(i, 0 == (L2 == L)); LOOP_ASSERT(i, 1 == (L2 != L));
            mL[i].replaceValue(L2[i]);
            LOOP_ASSERT(i, 1 == (L2 == L)); LOOP_ASSERT(i, 0 == (L2 != L));

            for (int j = 0; j < 32; ++j) {
                LOOP2_ASSERT(i, j, (i == j) == (L2[j] == L[i]));
                mL.replaceElement(i, L2[j]);
                LOOP2_ASSERT(i, j, L2[j] == L[i]);

                LOOP2_ASSERT(i, j, (i == j) == (L2 == L));
                LOOP2_ASSERT(i, j, (i != j) == (L2 != L));

                LOOP2_ASSERT(i, j, L2[j] == L[i]);
                mL.replaceElement(i, L2[i]);
                LOOP2_ASSERT(i, j, (i == j) == (L2[j] == L[i]));

                LOOP2_ASSERT(i, j, 1 == (L2 == L));
                LOOP2_ASSERT(i, j, 0 == (L2 != L));
            }
        }

        if (verbose) cout << "Append extra element to make !=." << endl;
        {
             bslma_TestAllocator a1;
             bslma_TestAllocator a2;
             bslma_TestAllocator a3;
             bslma_TestAllocator a4;
             bdema_MultipoolAllocator ma4(20, &a4); // to handle
                                                    // BDEM_SUBORDINATE

             const bdem_AggregateOption::AllocationStrategy S1 =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;
             const bdem_AggregateOption::AllocationStrategy S2 =
                                       bdem_AggregateOption::BDEM_WRITE_MANY;
             const bdem_AggregateOption::AllocationStrategy S3 =
                                       bdem_AggregateOption::BDEM_WRITE_ONCE;
             const bdem_AggregateOption::AllocationStrategy S4 =
                                       bdem_AggregateOption::BDEM_SUBORDINATE;

               bdem_List mX(S1, &a1); const bdem_List& X = mX;
               bdem_List mY(S2, &a2); const bdem_List& Y = mY;
               bdem_List mZ(S3, &a3); const bdem_List& Z = mZ;
               bdem_List *p = new(ma4) bdem_List(S4, &ma4);
               bdem_List& mW = *p;    const bdem_List& W = mW;

               ASSERT(X == Y);

           for (i = 0; i < 32; ++i) {
                 mX.appendElement(L[i]);
                 LOOP_ASSERT(i, 0 == (Y == X));
                 LOOP_ASSERT(i, 1 == (Y != X));
                 LOOP_ASSERT(i, 1 == (Z == Y));
                 LOOP_ASSERT(i, 0 == (Z != Y));
                 LOOP_ASSERT(i, 1 == (W == Z));
                 LOOP_ASSERT(i, 0 == (W != Z));

                 if (veryVeryVerbose) { P_(i) Q(==============) P(X) P(Y) }

                 mY.appendElement(L[i]);
                 LOOP_ASSERT(i, 1 == (Y == X));
                 LOOP_ASSERT(i, 0 == (Y != X));
                 LOOP_ASSERT(i, 0 == (Z == Y));
                 LOOP_ASSERT(i, 1 == (Z != Y));
                 LOOP_ASSERT(i, 1 == (W == Z));
                 LOOP_ASSERT(i, 0 == (W != Z));

                 mZ.appendElement(L[i]);
                 LOOP_ASSERT(i, 1 == (Y == X));
                 LOOP_ASSERT(i, 0 == (Y != X));
                 LOOP_ASSERT(i, 1 == (Z == Y));
                 LOOP_ASSERT(i, 0 == (Z != Y));
                 LOOP_ASSERT(i, 0 == (W == Z));
                 LOOP_ASSERT(i, 1 == (W != Z));

                 mW.appendElement(L[i]);
                 LOOP_ASSERT(i, 1 == (Y == X));
                 LOOP_ASSERT(i, 0 == (Y != X));
                 LOOP_ASSERT(i, 1 == (Z == Y));
                 LOOP_ASSERT(i, 0 == (Z != Y));
                 LOOP_ASSERT(i, 1 == (W == Z));
                 LOOP_ASSERT(i, 0 == (W != Z));
             }

        } // Note: 'bdema_MultipoolAllocator' releases memory on destruction.
}

static void testCase5(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // OUTPUT: bsl::ostream& 'operator<<' and 'print'
        //
        // Concerns:
        //   - that value is formatted correctly (e.g., '{}' vs '[]' for list).
        //   - that rows do not print the type, just the data.
        //   - that character and string data is quoted (') & (") respectively.
        //   - that no additional characters are written after terminating.
        //   - that these functions work on references to 'const' instances.
        //   - that each return a reference to the modifiable stream argument.
        //   - that print properly forwards its arguments.
        //   - that negative 'level' suppresses indentation on first line.
        //   - that negative 'spacesPerLevel' forces single-line output.
        //   - that negative 'spacesPerLevel' does NOT suppress indentation.
        //   - that if an allocator is supplied, it is not used for temporary
        //      allocations (e.g., to format float or double values).
        //   - that if an allocator is supplied, the default allocator is not
        //      used AT ALL!
        //
        //  Note that formatting FLOAT, DOUBLE, FLOAT_ARRAY, AND DOUBLE_ARRAY
        //  appear to be problematic with respect to use of the default
        //  allocator.
        //
        // Plan:
        //  For each of a small representative set of object values use
        //  'ostrstream' to write that object's value to two separate
        //  character buffers each with different initial values.  Compare
        //  the contents of these buffers with the literal expected output
        //  format and verify that the characters beyond the null characters
        //  are unaffected in both buffers.
        //   - Test operator<< on the empty List and all 32 one-element lists.
        //   - Test print on each of the following with various arguments:
        //      + the empty list: { }
        //      + a one-element list (containing a scalar): { CHAR 'A' }
        //      + a one-element list (containing an array):
        //          { INT_ARRAY [ INT 10 ] }
        //      + a two-element list: { INT 10 STRING_ARRAY [ "one" ] }
        //  To insure that no memory is allocated from the default allocator
        //  during printing, install a test allocator as the default and also
        //  supply a separate test allocator explicitly.  Measure the total
        //  usage of both allocators before and after printing:
        //      + Create an empty ostrstream.
        //      + Print an empty list.
        //      + Print a list with all 32 kinds of elements.
        //      + Print all of the 32 kinds of 1-element lists.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Table-Driven Implementation Technique
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& s, int l, int spl) const;
        //   operator<<(ostream&, const bdem_List&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                  << "OUTPUT bsl::ostream& 'operator<<' and 'print'" << endl
                  << "=============================================" << endl;

        const bdem_List L;
        bdem_List mL_A00; const bdem_List& L_A00 = mL_A00;
        mL_A00.appendChar(A00);
        bdem_List mL_A01; const bdem_List& L_A01 = mL_A01;
        mL_A01.appendShort(A01);
        bdem_List mL_A02; const bdem_List& L_A02 = mL_A02;
        mL_A02.appendInt(A02);
        bdem_List mL_A03; const bdem_List& L_A03 = mL_A03;
        mL_A03.appendInt64(A03);
        bdem_List mL_A04; const bdem_List& L_A04 = mL_A04;
        mL_A04.appendFloat(A04);
        bdem_List mL_A05; const bdem_List& L_A05 = mL_A05;
        mL_A05.appendDouble(A05);
        bdem_List mL_A06; const bdem_List& L_A06 = mL_A06;
        mL_A06.appendString(A06);
        bdem_List mL_A07; const bdem_List& L_A07 = mL_A07;
        mL_A07.appendDatetime(A07);
        bdem_List mL_A08; const bdem_List& L_A08 = mL_A08;
        mL_A08.appendDate(A08);
        bdem_List mL_A09; const bdem_List& L_A09 = mL_A09;
        mL_A09.appendTime(A09);

        bdem_List mL_A10; const bdem_List& L_A10 = mL_A10;
        mL_A10.appendCharArray(A10);
        bdem_List mL_A11; const bdem_List& L_A11 = mL_A11;
        mL_A11.appendShortArray(A11);
        bdem_List mL_A12; const bdem_List& L_A12 = mL_A12;
        mL_A12.appendIntArray(A12);
        bdem_List mL_A13; const bdem_List& L_A13 = mL_A13;
        mL_A13.appendInt64Array(A13);
        bdem_List mL_A14; const bdem_List& L_A14 = mL_A14;
        mL_A14.appendFloatArray(A14);
        bdem_List mL_A15; const bdem_List& L_A15 = mL_A15;
        mL_A15.appendDoubleArray(A15);
        bdem_List mL_A16; const bdem_List& L_A16 = mL_A16;
        mL_A16.appendStringArray(A16);
        bdem_List mL_A17; const bdem_List& L_A17 = mL_A17;
        mL_A17.appendDatetimeArray(A17);
        bdem_List mL_A18; const bdem_List& L_A18 = mL_A18;
        mL_A18.appendDateArray(A18);
        bdem_List mL_A19; const bdem_List& L_A19 = mL_A19;
        mL_A19.appendTimeArray(A19);

        bdem_List mL_A20; const bdem_List& L_A20 = mL_A20;
        mL_A20.appendList(A20);
        bdem_List mL_A21; const bdem_List& L_A21 = mL_A21;
        mL_A21.appendTable(A21);
        bdem_List mL_A30; const bdem_List& L_A30 = mL_A30;
        mL_A30.appendChoice(A30);
        bdem_List mL_A31; const bdem_List& L_A31 = mL_A31;
        mL_A31.appendChoiceArray(A31);

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int              d_lineNum; // source line number
                const bdem_List *d_obj_p;   // address of object to be printed
                const char      *d_fmt_p;   // expected output format
            } DATA[] = {

                //L# Object  Format
                //-- ------  --------------------------------------------------
                {L_, &L,     "{ }"                                           },

                {L_, &L_A00, "{ CHAR A }"                                    },
                {L_, &L_A01, "{ SHORT -1 }"                                  },
                {L_, &L_A02, "{ INT 10 }"                                    },
                {L_, &L_A03, "{ INT64 -100 }"                                },
                {L_, &L_A04, "{ FLOAT -1.5 }"                                },
                {L_, &L_A05, "{ DOUBLE 10.5 }"                               },
                {L_, &L_A06, "{ STRING one }"                                },
                {L_, &L_A07, "{ DATETIME 01JAN2000_00:01:02.003 }"           },
                {L_, &L_A08, "{ DATE 01JAN2000 }"                            },
                {L_, &L_A09, "{ TIME 00:01:02.003 }"                         },

                {L_, &L_A10, "{ CHAR_ARRAY \"A\" }"                          },
                {L_, &L_A11, "{ SHORT_ARRAY [ -1 ] }"                        },
                {L_, &L_A12, "{ INT_ARRAY [ 10 ] }"                          },
                {L_, &L_A13, "{ INT64_ARRAY [ -100 ] }"                      },
                {L_, &L_A14, "{ FLOAT_ARRAY [ -1.5 ] }"                      },
                {L_, &L_A15, "{ DOUBLE_ARRAY [ 10.5 ] }"                     },
                {L_, &L_A16, "{ STRING_ARRAY [ one ] }"                      },
                {L_, &L_A17, "{ DATETIME_ARRAY [ 01JAN2000_00:01:02.003 ] }" },
                {L_, &L_A18, "{ DATE_ARRAY [ 01JAN2000 ] }"                  },
                {L_, &L_A19, "{ TIME_ARRAY [ 00:01:02.003 ] }"               },

                {L_, &L_A20, "{ LIST {"
                    " INT 10"
                    " DOUBLE 10.5"
                    " STRING one"
                    " STRING_ARRAY [ one ] } }"                              },
                {L_, &L_A21, "{ TABLE {"
                    " Column Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                    " Row 0: { 10 10.5 one [ one ] } } }"                    },
                {L_, &L_A30, "{ CHOICE {"
                    " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                    " { STRING one } } }"                                    },
                {L_, &L_A31, "{ CHOICE_ARRAY {"
                    " Selection Types: [ INT DOUBLE STRING STRING_ARRAY ]"
                    " Item 0: { INT 10 }"
                    " Item 1: { DOUBLE 10.5 }"
                    " Item 2: { STRING one }"
                    " Item 3: { STRING_ARRAY [ one ] }"
                    " } }"                                                   }

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE        =  DATA[ti].d_lineNum;
                const bdem_List& LIST = *DATA[ti].d_obj_p;
                const char *const FMT =  DATA[ti].d_fmt_p;

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout   // print format BEFORE output operation
                    << "EXPECTED FORMAT: ``" << FMT << "''" << endl;

                ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
                out1 << LIST << ends;  // Ensure modifiable
                out2 << LIST << ends;  // stream is returned.

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                const bool failure = 0 != memcmp(buf1, FMT, SZ);

                if (!veryVerbose && failure) cout << // print AFTER if needed
                    "EXPECTED FORMAT: ``" << FMT << "''" << endl;
                if (veryVerbose || failure) cout <<  // print result if needed
                    "  ACTUAL FORMAT: ``" << buf1 << "''" << endl;

                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ,REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ,REST));
            }

            for (int ti = 1; ti < NUM_DATA; ++ti) {
                // index from 1, the list itself cannot be null
                const int LINE        =  DATA[ti].d_lineNum;
                const bdem_List& LIST = *DATA[ti].d_obj_p;
                const char *const FMT =  DATA[ti].d_fmt_p;

                char buf1[SIZE];

                bdem_List list = LIST;
                list.makeAllNull();  // function not yet tested
                ostrstream out1(buf1, SIZE);
                out1 << list << ends;  // Ensure modifiable

                if (veryVerbose) cout <<  // print result if needed
                    "  ACTUAL FORMAT: ``" << buf1 << "''" << endl;

                LOOP_ASSERT(LINE, 0 != strstr(buf1, "NULL"));
            }
        }

        if (verbose) cout << "\nTesting 'print'." << endl;
        {
            bdem_List mL_A02_A16; const bdem_List& L_A02_A16 = mL_A02_A16;
            mL_A02_A16.appendInt(A02);
            mL_A02_A16.appendStringArray(A16);

            static const struct {
                int              d_lineNum; // source line number
                const bdem_List *d_obj_p;   // address of object to be printed
                int              d_level;
                int              d_spacesPerLevel;
                const char      *d_fmt_p;   // expected output format
            } DATA[] = {
                //L# Object      Lev SPL Format
                //-- ----------  --- --- ------------------------------------
                {L_, &L,         0,  0,  "{\n"
                                         "}\n"                               },
                {L_, &L,         2,  1,  "  {\n"
                                         "  }\n"                             },
                {L_, &L,         -2, 1,  "{\n"
                                         "  }\n"                             },
                {L_, &L,         2,  -1, "  { }"                             },
                {L_, &L,         -2, -1, "{ }"                               },
                //-- ----------  --- --- ------------------------------------
                {L_, &L_A00,     1,  0,  "{\n"
                                         "CHAR A\n"
                                         "}\n"                               },
                {L_, &L_A00,     0,  1,  "{\n"
                                         " CHAR A\n"
                                         "}\n"                               },
                {L_, &L_A00,     1,  3,  "   {\n"
                                         "      CHAR A\n"
                                         "   }\n"                            },
                {L_, &L_A00,     3,  1,  "   {\n"
                                         "    CHAR A\n"
                                         "   }\n"                            },
                //-- ----------  --- --- ------------------------------------
                {L_, &L_A12,     0,  0,  "{\n"
                                         "INT_ARRAY [\n"
                                         "10\n"
                                         "]\n"
                                         "}\n"                               },
                {L_, &L_A12,     3,  2,  "      {\n"
                                         "        INT_ARRAY [\n"
                                         "          10\n"
                                         "        ]\n"
                                         "      }\n"                         },
                {L_, &L_A12,     2,  3,  "      {\n"
                                         "         INT_ARRAY [\n"
                                         "            10\n"
                                         "         ]\n"
                                         "      }\n"                         },
                {L_, &L_A12,     -3, 2,  "{\n"
                                         "        INT_ARRAY [\n"
                                         "          10\n"
                                         "        ]\n"
                                         "      }\n"                         },
                {L_, &L_A12,     3,  -2, "      { INT_ARRAY [ 10 ] }"        },
                {L_, &L_A12,     -3, -2, "{ INT_ARRAY [ 10 ] }"              },
                //-- ----------  --- --- ------------------------------------
                {L_, &L_A02_A16, -2,  5, "{\n"
                                         "               INT 10\n"
                                         "               STRING_ARRAY [\n"
                                         "                    one\n"
                                         "               ]\n"
                                         "          }\n"                     },
                //-- ----------  --- --- ------------------------------------
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = (char) 0xff;  // Used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE        =  DATA[ti].d_lineNum;
                const int LEVEL       =  DATA[ti].d_level;
                const int SPL         =  DATA[ti].d_spacesPerLevel;
                const bdem_List& LIST = *DATA[ti].d_obj_p;
                const char *const FMT =  DATA[ti].d_fmt_p;

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout   // print format BEFORE output operation
                    << "EXPECTED FORMAT: ``" << FMT << "''" << endl;

                ostrstream out1(buf1, SIZE), out2(buf2, SIZE);
                LIST.print(out1, LEVEL, SPL) << ends;  // Ensure modifiable
                LIST.print(out2, LEVEL, SPL) << ends;  // stream is returned.

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                const bool failure = 0 != memcmp(buf1, FMT, SZ);

                if (!veryVerbose && failure) cout << // print AFTER if needed
                    "EXPECTED FORMAT: ``" << FMT << "''" << endl;
                if (veryVerbose || failure) cout <<  // print result if needed
                    "  ACTUAL FORMAT: ``" << buf1 << "''" << endl;

                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ,REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ,REST));
            }
        }

        if (verbose) cout <<
           "\nEnsure default allocator not used inappropriately." << endl;
        {
            if (verbose) cout << "\tCreate reference list A." << endl;

            bdem_List mA; const bdem_List& A = mA;  ASSERT( 0 == A.length());
            loadReferenceA(&mA);                    ASSERT(32 == A.length());

            const int SIZE = 10000;     // sufficiently large buffer size

            if (verbose) cout << "\tCreat an empty ostrstream." << endl;
            {
                // Since ostream uses string, which uses the default
                // allocator, we initialize our output stream BEFORE pushing a
                // new default allocator.  This prevents the output stream
                // from interfering with the memory allocation count.
                char buffer[SIZE];  ostrstream out(buffer, SIZE);

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            }

            if (verbose) cout << "\tPrint an empty list." << endl;
            {
                char buffer[SIZE];  ostrstream out(buffer, SIZE);

                // Note that in this test block, we create the ostream before
                // swapping in our own test allocator.

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                bslma_TestAllocator a;  // specified allocator

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), 0 == a.numBlocksTotal());

                bdem_List mX(&a); const bdem_List& X = mX;
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                const int USAGE = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(USAGE) }
                ASSERT(USAGE > 0);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* BEFORE ************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());

                out << X << ends;
                if (veryVerbose) P(buffer);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* AFTER *************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());
            }

            if (verbose) cout << "\tPrint the reference list A." << endl;
            {
                char buffer[SIZE];  ostrstream out(buffer, SIZE);

                // Note that in this test block, we again create the ostream
                // before swapping in our own test allocator.

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                bslma_TestAllocator a;  // specified allocator

                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), 0 == a.numBlocksTotal());

                bdem_List mX(A, &a); const bdem_List& X = mX;
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

                const int USAGE = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(USAGE) }
                ASSERT(USAGE > 0);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* BEFORE ************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());

                out << X << ends;
                if (veryVerbose) P(buffer);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* AFTER *************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
//                 LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP_ASSERT(a.numBlocksTotal(), USAGE == a.numBlocksTotal());
            }

            if (verbose) cout << "\tPrint each kind of 1-element list." <<endl;
            for (int i = 0; i < 32; ++i) {
                if (veryVerbose) { T_ T_
                    Q(########################################################)
                    T_ T_ cout << "BEGIN "; P_(i) P(bdem_ElemType::Type(i))
                }

                char buffer[SIZE];  ostrstream out(buffer, SIZE);

                // Note that in this test block, we yet again create the
                // ostream before swapping in our own test allocator.

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                bslma_TestAllocator a;  // specified allocator

                LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP2_ASSERT(i, a.numBlocksTotal(), 0 == a.numBlocksTotal());

                bdem_List mX(&a); const bdem_List& X = mX;
                LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());

                mX.appendElement(A[i]);
                LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());

                const int USAGE = a.numBlocksTotal();
                if (veryVerbose) { T_ T_ P(USAGE) }
                LOOP_ASSERT(i, USAGE > 0);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* BEFORE ************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
                LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP2_ASSERT(i, a.numBlocksTotal(), USAGE==a.numBlocksTotal());

                out << X << ends;
                if (veryVerbose) P(buffer);

                if (veryVerbose) {
                    cout << endl <<
                    "************************* AFTER *************************"
                    << endl;
                    cout << "SPECIFIED ALLOCATOR:" << endl;
                    a.print();
                    cout << "\nDEFAULT ALLOCATOR:" << endl;
                    da.print();
                }
//              LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());
                LOOP2_ASSERT(i, a.numBlocksTotal(), USAGE==a.numBlocksTotal());

                if (veryVerbose) {
                    T_ T_ cout << "END "; P_(i) P(bdem_ElemType::Type(i))
                }
            }
        }
}

static void testCase4(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS/BASIC ACCESSORS
        //   Verify the most basic functionality of a 'bdem_List'
        //
        // Concerns:
        //  Basically to make sure that functions are forwarding properly:
        //   - that each manipulators delegate to the underlying bdem_listImp.
        //   - that basic (direct) accessors also delegate correctly.
        //   - that accessors work off of references to 'const' instances.
        //
        // Plan:
        //   - Create an empty list and verify its length.
        //   - Create an instances of each of the 32 "types" of one-element
        //      list, and verify the result using all basic accessors,
        //      each via a non-modifiable reference.
        //   - Create a forward and a backward list of all 32 types, and
        //      verify each using all basic accessors.
        //   - In the process, verify the 'loadReferenceA' function for future
        //      use.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   void appendBool(bool value);
        //   void appendChar(char value);
        //   void appendShort(short value);
        //   void appendInt(int value);
        //   void appendInt64(Int64 value);
        //   void appendFloat(float value);
        //   void appendDouble(double value);
        //   void appendString(const bsl::string& value);
        //   void appendDatetime(const bdet_Datetime& value);
        //   void appendDatetimeTz(const bdet_DatetimeTz& value);
        //   void appendDate(const bdet_Date& value);
        //   void appendDateTz(const bdet_DateTz& value);
        //   void appendTime(const bdet_Time& value);
        //   void appendTimeTz(const bdet_TimeTz& value);
        //   void appendBoolArray(const bsl::vector<bool>& value);
        //   void appendCharArray(const bsl::vector<char>& value);
        //   void appendShortArray(const bsl::vector<short>& value);
        //   void appendIntArray(const bsl::vector<int>& value);
        //   void appendInt64Array(const bsl::vector<Int64>& value);
        //   void appendFloatArray(const bsl::vector<float>& value);
        //   void appendDoubleArray(const bsl::vector<double>& value);
        //   void appendStringArray(const bsl::vector<bsl::string>& value);
        //   void appendDatetimeArray(const bsl::vector<bdet_Datetime>& value);
        //   void appendDatetimeTzArray(
        //                          const bsl::vector<bdet_DatetimeTz>& value);
        //   void appendDateArray(const bsl::vector<bdet_Date>& value);
        //   void appendDateTzArray(const bsl::vector<bdet_DateTz>& value);
        //   void appendTimeArray(const bsl::vector<bdet_Time>& value);
        //   void appendTimeTzArray(const bsl::vector<bdet_TimeTz>& value);
        //   void appendList(const bdem_List& value);
        //   void appendTable(const bdem_Table& value);
        //
        //   int length() const;
        //   bdem_ElemType::Type elemType(int index) const;
        //   void elemTypes(bsl::vector<bdem_ElemType::Type> *result) const;
        //   bdem_ConstElemRef operator[](int index) const;
        //   const bool& theBool(int index) const;
        //   const char& theChar(int index) const;
        //   const short& theShort(int index) const;
        //   const int& theInt(int index) const;
        //   const Int64& theInt64(int index) const;
        //   const float& theFloat(int index) const;
        //   const double& theDouble(int index) const;
        //   const bsl::string& theString(int index) const;
        //   const bdet_Datetime& theDatetime(int index) const;
        //   const bdet_DatetimeTz& theDatetimeTz(int index) const;
        //   const bdet_Date& theDate(int index) const;
        //   const bdet_DateTz& theDateTz(int index) const;
        //   const bdet_Time& theTime(int index) const;
        //   const bdet_TimeTz& theTimeTz(int index) const;
        //   const bsl::vector<bool>& theBoolArray(int index) const;
        //   const bsl::vector<char>& theCharArray(int index) const;
        //   const bsl::vector<short>& theShortArray(int index) const;
        //   const bsl::vector<int>& theIntArray(int index) const;
        //   const bsl::vector<Int64>& theInt64Array(int index) const;
        //   const bsl::vector<float>& theFloatArray(int index) const;
        //   const bsl::vector<double>& theDoubleArray(int index) const;
        //   const bsl::vector<bsl::string>& theStringArray(int index) const;
        //   const bsl::vector<bdet_Datetime>& theDatetimeArray(int i) const;
        //   const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(int i)
        //                                                               const;
        //   const bsl::vector<bdet_Date>& theDateArray(int index) const;
        //   const bsl::vector<bdet_DateTz>& theDateTzArray(int index) const;
        //   const bsl::vector<bdet_Time>& theTimeArray(int index) const;
        //   const bsl::vector<bdet_TimeTz>& theTimeTzArray(int index) const;
        //   const bdem_List& theList(int index) const;
        //   const bdem_Table& theTable(int index) const;
        //
        //   bool& theModifiableBool(int index);
        //   char& theModifiableChar(int index);
        //   short& theModifiableShort(int index);
        //   int& theModifiableInt(int index);
        //   Int64& theModifiableInt64(int index);
        //   float& theModifiableFloat(int index);
        //   double& theModifiableDouble(int index);
        //   bsl::string& theModifiableString(int index);
        //   bdet_Datetime& theModifiableDatetime(int index);
        //   bdet_DatetimeTz& theModifiableDatetimeTz(int index);
        //   bdet_Date& theModifiableDate(int index);
        //   bdet_DateTz& theModifiableDateTz(int index);
        //   bdet_Time& theModifiableTime(int index);
        //   bdet_TimeTz& theModifiableTimeTz(int index);
        //   bsl::vector<bool>& theModifiableBoolArray(int index);
        //   bsl::vector<char>& theModifiableCharArray(int index);
        //   bsl::vector<short>& theModifiableShortArray(int index);
        //   bsl::vector<int>& theModifiableIntArray(int index);
        //   bsl::vector<Int64>& theModifiableInt64Array(int index);
        //   bsl::vector<float>& theModifiableFloatArray(int index);
        //   bsl::vector<double>& theModifiableDoubleArray(int index);
        //   bsl::vector<bsl::string>& theModifiableStringArray(int index);
        //   bsl::vector<bdet_Datetime>& theModifiableDatetimeArray(int index);
        //   bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray(int);
        //   bsl::vector<bdet_Date>& theModifiableDateArray(int index);
        //   bsl::vector<bdet_DateTz>& theModifiableDateTzArray(int index);
        //   bsl::vector<bdet_Time>& theModifiableTimeArray(int index);
        //   bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray(int index);
        //   bdem_List& theModifiableList(int index);
        //   bdem_Table& theModifiableTable(int index);
        //
        //   void loadReferenceA(bdem_List *result);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS/BASIC ACCESSORS" << endl
                          << "====================================" << endl;

        if (verbose) cout << "\nEmpty list." << endl;
        {
            const bdem_List LIST; ASSERT(0 == LIST.length());
        }

        bsl::vector<bdem_ElemType::Type> elemTypesVec;
        if (verbose) cout << "\nAll kinds of one-element list." << endl;
        {
            if (verbose) cout << "\tBOOL" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendBool(A22); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_BOOL == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_BOOL == elemTypesVec[0]);
            ASSERT(A22 == L[0].theBool());
            ASSERT(A22 == L.theBool(0));

            mL.theModifiableBool(0) = B22;
            ASSERT(B22 == L.theBool(0));
            mL.theModifiableBool(0) = N22;
            ASSERT(N22 == L.theBool(0));
        }
        {
            if (verbose) cout << "\tCHAR" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendChar(A00); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_CHAR == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_CHAR == elemTypesVec[0]);
            ASSERT(A00 == L[0].theChar());
            ASSERT(A00 == L.theChar(0));

            mL.theModifiableChar(0) = B00;
            ASSERT(B00 == L.theChar(0));
            mL.theModifiableChar(0) = N00;
            ASSERT(N00 == L.theChar(0));
        }
        {
            if (verbose) cout << "\tSHORT" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendShort(A01); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_SHORT == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_SHORT == elemTypesVec[0]);
            ASSERT(A01 == L[0].theShort());
            ASSERT(A01 == L.theShort(0));

            mL.theModifiableShort(0) = B01;
            ASSERT(B01 == L.theShort(0));
            mL.theModifiableShort(0) = N01;
            ASSERT(N01 == L.theShort(0));
        }
        {
            if (verbose) cout << "\tINT" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendInt(A02); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_INT == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_INT == elemTypesVec[0]);
            ASSERT(A02 == L[0].theInt());
            ASSERT(A02 == L.theInt(0));

            mL.theModifiableInt(0) = B02;
            ASSERT(B02 == L.theInt(0));
            mL.theModifiableInt(0) = N02;
            ASSERT(N02 == L.theInt(0));
        }
        {
            if (verbose) cout << "\tINT64" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendInt64(A03); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_INT64 == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_INT64 == elemTypesVec[0]);
            ASSERT(A03 == L[0].theInt64());
            ASSERT(A03 == L.theInt64(0));

            mL.theModifiableInt64(0) = B03;
            ASSERT(B03 == L.theInt64(0));
            mL.theModifiableInt64(0) = N03;
            ASSERT(N03 == L.theInt64(0));
        }
        {
            if (verbose) cout << "\tFLOAT" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendFloat(A04); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_FLOAT == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_FLOAT == elemTypesVec[0]);
            ASSERT(A04 == L[0].theFloat());
            ASSERT(A04 == L.theFloat(0));

            mL.theModifiableFloat(0) = B04;
            ASSERT(B04 == L.theFloat(0));
            mL.theModifiableFloat(0) = N04;
            ASSERT(N04 == L.theFloat(0));
        }
        {
            if (verbose) cout << "\tDOUBLE" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDouble(A05); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DOUBLE == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DOUBLE == elemTypesVec[0]);
            ASSERT(A05 == L[0].theDouble());
            ASSERT(A05 == L.theDouble(0));

            mL.theModifiableDouble(0) = B05;
            ASSERT(B05 == L.theDouble(0));
            mL.theModifiableDouble(0) = N05;
            ASSERT(N05 == L.theDouble(0));
        }
        {
            if (verbose) cout << "\tSTRING" << endl;
            bslma_TestAllocator ta;
            bdem_List mL(&ta); const bdem_List& L = mL;
            mL.appendString(A06); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_STRING == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_STRING == elemTypesVec[0]);
            ASSERT(A06 == L[0].theString());
            ASSERT(A06 == L.theString(0));

            mL.theModifiableString(0) = B06;
            ASSERT(B06 == L.theString(0));
            mL.theModifiableString(0) = N06;
            ASSERT(N06 == L.theString(0));
        }
        {
            if (verbose) cout << "\tDATETIME" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDatetime(A07); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DATETIME == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DATETIME == elemTypesVec[0]);
            ASSERT(A07 == L[0].theDatetime());
            ASSERT(A07 == L.theDatetime(0));

            mL.theModifiableDatetime(0) = B07;
            ASSERT(B07 == L.theDatetime(0));
            mL.theModifiableDatetime(0) = N07;
            ASSERT(N07 == L.theDatetime(0));
        }
        {
            if (verbose) cout << "\tDATETIMETZ" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDatetimeTz(A23); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DATETIMETZ == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ == elemTypesVec[0]);
            ASSERT(A23 == L[0].theDatetimeTz());
            ASSERT(A23 == L.theDatetimeTz(0));

            mL.theModifiableDatetimeTz(0) = B23;
            ASSERT(B23 == L.theDatetimeTz(0));
            mL.theModifiableDatetimeTz(0) = N23;
            ASSERT(N23 == L.theDatetimeTz(0));
        }
        {
            if (verbose) cout << "\tDATE" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDate(A08); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DATE == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DATE == elemTypesVec[0]);
            ASSERT(A08 == L[0].theDate());
            ASSERT(A08 == L.theDate(0));

            mL.theModifiableDate(0) = B08;
            ASSERT(B08 == L.theDate(0));
            mL.theModifiableDate(0) = N08;
            ASSERT(N08 == L.theDate(0));
        }
        {
            if (verbose) cout << "\tDATETZ" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDateTz(A24); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DATETZ == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DATETZ == elemTypesVec[0]);
            ASSERT(A24 == L[0].theDateTz());
            ASSERT(A24 == L.theDateTz(0));

            mL.theModifiableDateTz(0) = B24;
            ASSERT(B24 == L.theDateTz(0));
            mL.theModifiableDateTz(0) = N24;
            ASSERT(N24 == L.theDateTz(0));
        }
        {
            if (verbose) cout << "\tTIME" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendTime(A09); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_TIME == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_TIME == elemTypesVec[0]);
            ASSERT(A09 == L[0].theTime());
            ASSERT(A09 == L.theTime(0));

            mL.theModifiableTime(0) = B09;
            ASSERT(B09 == L.theTime(0));
            mL.theModifiableTime(0) = N09;
            ASSERT(N09 == L.theTime(0));
        }
        {
            if (verbose) cout << "\tTIMETZ" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendTimeTz(A25); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_TIMETZ == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_TIMETZ == elemTypesVec[0]);
            ASSERT(A25 == L[0].theTimeTz());
            ASSERT(A25 == L.theTimeTz(0));

            mL.theModifiableTimeTz(0) = B25;
            ASSERT(B25 == L.theTimeTz(0));
            mL.theModifiableTimeTz(0) = N25;
            ASSERT(N25 == L.theTimeTz(0));
        }
        {
            if (verbose) cout << "\tBOOL_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendBoolArray(A26); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY == elemTypesVec[0]);
            ASSERT(A26 == L[0].theBoolArray());
            ASSERT(A26 == L.theBoolArray(0));

            mL.theModifiableBoolArray(0) = B26;
            ASSERT(B26 == L.theBoolArray(0));
            mL.theModifiableBoolArray(0) = N26;
            ASSERT(N26 == L.theBoolArray(0));
        }
        {
            if (verbose) cout << "\tCHAR_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendCharArray(A10); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY == elemTypesVec[0]);
            ASSERT(A10 == L[0].theCharArray());
            ASSERT(A10 == L.theCharArray(0));

            mL.theModifiableCharArray(0) = B10;
            ASSERT(B10 == L.theCharArray(0));
            mL.theModifiableCharArray(0) = N10;
            ASSERT(N10 == L.theCharArray(0));
        }
        {
            if (verbose) cout << "\tSHORT_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendShortArray(A11); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY == elemTypesVec[0]);
            ASSERT(A11 == L[0].theShortArray());
            ASSERT(A11 == L.theShortArray(0));

            mL.theModifiableShortArray(0) = B11;
            ASSERT(B11 == L.theShortArray(0));
            mL.theModifiableShortArray(0) = N11;
            ASSERT(N11 == L.theShortArray(0));
        }
        {
            if (verbose) cout << "\tINT_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendIntArray(A12); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_INT_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY == elemTypesVec[0]);
            ASSERT(A12 == L[0].theIntArray());
            ASSERT(A12 == L.theIntArray(0));

            mL.theModifiableIntArray(0) = B12;
            ASSERT(B12 == L.theIntArray(0));
            mL.theModifiableIntArray(0) = N12;
            ASSERT(N12 == L.theIntArray(0));
        }
        {
            if (verbose) cout << "\tINT64_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendInt64Array(A13); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY == elemTypesVec[0]);
            ASSERT(A13 == L[0].theInt64Array());
            ASSERT(A13 == L.theInt64Array(0));

            mL.theModifiableInt64Array(0) = B13;
            ASSERT(B13 == L.theInt64Array(0));
            mL.theModifiableInt64Array(0) = N13;
            ASSERT(N13 == L.theInt64Array(0));
        }
        {
            if (verbose) cout << "\tFLOAT_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendFloatArray(A14); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY == elemTypesVec[0]);
            ASSERT(A14 == L[0].theFloatArray());
            ASSERT(A14 == L.theFloatArray(0));

            mL.theModifiableFloatArray(0) = B14;
            ASSERT(B14 == L.theFloatArray(0));
            mL.theModifiableFloatArray(0) = N14;
            ASSERT(N14 == L.theFloatArray(0));
        }
        {
            if (verbose) cout << "\tDOUBLE_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDoubleArray(A15); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY == elemTypesVec[0]);
            ASSERT(A15 == L[0].theDoubleArray());
            ASSERT(A15 == L.theDoubleArray(0));

            mL.theModifiableDoubleArray(0) = B15;
            ASSERT(B15 == L.theDoubleArray(0));
            mL.theModifiableDoubleArray(0) = N15;
            ASSERT(N15 == L.theDoubleArray(0));
        }
        {
            if (verbose) cout << "\tSTRING_ARRAY" << endl;
            bslma_TestAllocator ta;
            bdem_List mL(&ta); const bdem_List& L = mL;
            mL.appendStringArray(A16); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY == elemTypesVec[0]);
            ASSERT(A16 == L[0].theStringArray());
            ASSERT(A16 == L.theStringArray(0));

            mL.theModifiableStringArray(0) = B16;
            ASSERT(B16 == L.theStringArray(0));
            mL.theModifiableStringArray(0) = N16;
            ASSERT(N16 == L.theStringArray(0));
        }
        {
            if (verbose) cout << "\tDATETIME_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDatetimeArray(A17); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY == elemTypesVec[0]);
            ASSERT(A17 == L[0].theDatetimeArray());
            ASSERT(A17 == L.theDatetimeArray(0));

            mL.theModifiableDatetimeArray(0) = B17;
            ASSERT(B17 == L.theDatetimeArray(0));
            mL.theModifiableDatetimeArray(0) = N17;
            ASSERT(N17 == L.theDatetimeArray(0));
        }
        {
            if (verbose) cout << "\tDATETIMETZ_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDatetimeTzArray(A27); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == elemTypesVec[0]);
            ASSERT(A27 == L[0].theDatetimeTzArray());
            ASSERT(A27 == L.theDatetimeTzArray(0));

            mL.theModifiableDatetimeTzArray(0) = B27;
            ASSERT(B27 == L.theDatetimeTzArray(0));
            mL.theModifiableDatetimeTzArray(0) = N27;
            ASSERT(N27 == L.theDatetimeTzArray(0));
        }
        {
            if (verbose) cout << "\tDATE_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDateArray(A18); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY == elemTypesVec[0]);
            ASSERT(A18 == L[0].theDateArray());
            ASSERT(A18 == L.theDateArray(0));

            mL.theModifiableDateArray(0) = B18;
            ASSERT(B18 == L.theDateArray(0));
            mL.theModifiableDateArray(0) = N18;
            ASSERT(N18 == L.theDateArray(0));
        }
        {
            if (verbose) cout << "\tDATETZ_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendDateTzArray(A28); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY == elemTypesVec[0]);
            ASSERT(A28 == L[0].theDateTzArray());
            ASSERT(A28 == L.theDateTzArray(0));

            mL.theModifiableDateTzArray(0) = B28;
            ASSERT(B28 == L.theDateTzArray(0));
            mL.theModifiableDateTzArray(0) = N28;
            ASSERT(N28 == L.theDateTzArray(0));
        }
        {
            if (verbose) cout << "\tTIME_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendTimeArray(A19); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY == elemTypesVec[0]);
            ASSERT(A19 == L[0].theTimeArray());
            ASSERT(A19 == L.theTimeArray(0));

            mL.theModifiableTimeArray(0) = B19;
            ASSERT(B19 == L.theTimeArray(0));
            mL.theModifiableTimeArray(0) = N19;
            ASSERT(N19 == L.theTimeArray(0));
        }
        {
            if (verbose) cout << "\tTIMETZ_ARRAY" << endl;
            bdem_List mL; const bdem_List& L = mL;
            mL.appendTimeTzArray(A29); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY == elemTypesVec[0]);
            ASSERT(A29 == L[0].theTimeTzArray());
            ASSERT(A29 == L.theTimeTzArray(0));

            mL.theModifiableTimeTzArray(0) = B29;
            ASSERT(B29 == L.theTimeTzArray(0));
            mL.theModifiableTimeTzArray(0) = N29;
            ASSERT(N29 == L.theTimeTzArray(0));
        }
        {
            if (verbose) cout << "\tLIST" << endl;
            bslma_TestAllocator ta;
            bdem_List mL(&ta); const bdem_List& L = mL;
            mL.appendList(A20); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_LIST == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_LIST == elemTypesVec[0]);
            LOOP2_ASSERT(A20, L[0].theList(), A20 == L[0].theList());
            ASSERT(A20 == L.theList(0));

            mL.theModifiableList(0) = B20;
            ASSERT(B20 == L.theList(0));
            mL.theModifiableList(0) = N20;
            ASSERT(N20 == L.theList(0));
        }
        {
            if (verbose) cout << "\tTABLE" << endl;
            bslma_TestAllocator ta;
            bdem_List mL(&ta); const bdem_List& L = mL;
            mL.appendTable(A21); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_TABLE == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_TABLE == elemTypesVec[0]);
            ASSERT(A21 == L[0].theTable());
            ASSERT(A21 == L.theTable(0));

            mL.theModifiableTable(0) = B21;
            ASSERT(B21 == L.theTable(0));
            mL.theModifiableTable(0) = N21;
            ASSERT(N21 == L.theTable(0));
        }

        {
            if (verbose) cout << "\tCHOICE" << endl;
            bslma_TestAllocator ta;
            bdem_List mL(&ta); const bdem_List& L = mL;
            mL.appendChoice(A30); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_CHOICE == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_CHOICE == elemTypesVec[0]);
            ASSERT(A30 == L[0].theChoice());
            ASSERT(A30 == L.theChoice(0));

            mL.theModifiableChoice(0) = B30;
            ASSERT(B30 == L.theChoice(0));
            mL.theModifiableChoice(0) = N30;
            ASSERT(N30 == L.theChoice(0));
        }
        {
            if (verbose) cout << "\tCHOICE_ARRAY" << endl;
            bslma_TestAllocator ta;
            bdem_List mL(&ta); const bdem_List& L = mL;
            mL.appendChoiceArray(A31); ASSERT(1 == L.length());
            L.elemTypes(&elemTypesVec);

            ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY == elemTypesVec[0]);
            ASSERT(A31 == L[0].theChoiceArray());
            ASSERT(A31 == L.theChoiceArray(0));

            mL.theModifiableChoiceArray(0) = B31;
            ASSERT(B31 == L.theChoiceArray(0));
            mL.theModifiableChoiceArray(0) = N31;
            ASSERT(N31 == L.theChoiceArray(0));
        }

        if (verbose) cout << "\nForward List with all element types." << endl;
        {
            bslma_TestAllocator ta;
            bdem_List mL(&ta); const bdem_List& L = mL;
                                                    ASSERT( 0 == L.length());
            loadReferenceA(&mL);                    ASSERT(32 == L.length());

            if (veryVerbose) cout << "\tCHAR" << endl;
            L.elemTypes(&elemTypesVec);
            ASSERT(bdem_ElemType::BDEM_CHAR == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_CHAR == elemTypesVec[0]);
            ASSERT(A00 == L[0].theChar());
            ASSERT(A00 == L.theChar(0));
            ASSERT(A00 == mL.theModifiableChar(0));

            if (veryVerbose) cout << "\tSHORT" << endl;
            ASSERT(bdem_ElemType::BDEM_SHORT == L.elemType(1));
            ASSERT(bdem_ElemType::BDEM_SHORT == elemTypesVec[1]);
            ASSERT(A01 == L[1].theShort());
            ASSERT(A01 == L.theShort(1));
            ASSERT(A01 == mL.theModifiableShort(1));

            if (veryVerbose) cout << "\tINT" << endl;
            ASSERT(bdem_ElemType::BDEM_INT == L.elemType(2));
            ASSERT(bdem_ElemType::BDEM_INT == elemTypesVec[2]);
            ASSERT(A02 == L[2].theInt());
            ASSERT(A02 == L.theInt(2));
            ASSERT(A02 == mL.theModifiableInt(2));

            if (veryVerbose) cout << "\tINT64" << endl;
            ASSERT(bdem_ElemType::BDEM_INT64 == L.elemType(3));
            ASSERT(bdem_ElemType::BDEM_INT64 == elemTypesVec[3]);
            ASSERT(A03 == L[3].theInt64());
            ASSERT(A03 == L.theInt64(3));
            ASSERT(A03 == mL.theModifiableInt64(3));

            if (veryVerbose) cout << "\tFLOAT" << endl;
            ASSERT(bdem_ElemType::BDEM_FLOAT == L.elemType(4));
            ASSERT(bdem_ElemType::BDEM_FLOAT == elemTypesVec[4]);
            ASSERT(A04 == L[4].theFloat());
            ASSERT(A04 == L.theFloat(4));
            ASSERT(A04 == mL.theModifiableFloat(4));

            if (veryVerbose) cout << "\tDOUBLE" << endl;
            ASSERT(bdem_ElemType::BDEM_DOUBLE == L.elemType(5));
            ASSERT(bdem_ElemType::BDEM_DOUBLE == elemTypesVec[5]);
            ASSERT(A05 == L[5].theDouble());
            ASSERT(A05 == L.theDouble(5));
            ASSERT(A05 == mL.theModifiableDouble(5));

            if (veryVerbose) cout << "\tSTRING" << endl;
            ASSERT(bdem_ElemType::BDEM_STRING == L.elemType(6));
            ASSERT(bdem_ElemType::BDEM_STRING == elemTypesVec[6]);
            ASSERT(A06 == L[6].theString());
            ASSERT(A06 == L.theString(6));
            ASSERT(A06 == mL.theModifiableString(6));

            if (veryVerbose) cout << "\tDATETIME" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETIME == L.elemType(7));
            ASSERT(bdem_ElemType::BDEM_DATETIME == elemTypesVec[7]);
            ASSERT(A07 == L[7].theDatetime());
            ASSERT(A07 == L.theDatetime(7));
            ASSERT(A07 == mL.theModifiableDatetime(7));

            if (veryVerbose) cout << "\tDATE" << endl;
            ASSERT(bdem_ElemType::BDEM_DATE == L.elemType(8));
            ASSERT(bdem_ElemType::BDEM_DATE == elemTypesVec[8]);
            ASSERT(A08 == L[8].theDate());
            ASSERT(A08 == L.theDate(8));
            ASSERT(A08 == mL.theModifiableDate(8));

            if (veryVerbose) cout << "\tTIME" << endl;
            ASSERT(bdem_ElemType::BDEM_TIME == L.elemType(9));
            ASSERT(bdem_ElemType::BDEM_TIME == elemTypesVec[9]);
            ASSERT(A09 == L[9].theTime());
            ASSERT(A09 == L.theTime(9));
            ASSERT(A09 == mL.theModifiableTime(9));

            if (veryVerbose) cout << "\tCHAR_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY == L.elemType(10));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY == elemTypesVec[10]);
            ASSERT(A10 == L[10].theCharArray());
            ASSERT(A10 == L.theCharArray(10));
            ASSERT(A10 == mL.theModifiableCharArray(10));

            if (veryVerbose) cout << "\tSHORT_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY == L.elemType(11));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY == elemTypesVec[11]);
            ASSERT(A11 == L[11].theShortArray());
            ASSERT(A11 == L.theShortArray(11));
            ASSERT(A11 == mL.theModifiableShortArray(11));

            if (veryVerbose) cout << "\tINT_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY == L.elemType(12));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY == elemTypesVec[12]);
            ASSERT(A12 == L[12].theIntArray());
            ASSERT(A12 == L.theIntArray(12));
            ASSERT(A12 == mL.theModifiableIntArray(12));

            if (veryVerbose) cout << "\tINT64_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY == L.elemType(13));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY == elemTypesVec[13]);
            ASSERT(A13 == L[13].theInt64Array());
            ASSERT(A13 == L.theInt64Array(13));
            ASSERT(A13 == mL.theModifiableInt64Array(13));

            if (veryVerbose) cout << "\tFLOAT_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY == L.elemType(14));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY == elemTypesVec[14]);
            ASSERT(A14 == L[14].theFloatArray());
            ASSERT(A14 == L.theFloatArray(14));
            ASSERT(A14 == mL.theModifiableFloatArray(14));

            if (veryVerbose) cout << "\tDOUBLE_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY == L.elemType(15));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY == elemTypesVec[15]);
            ASSERT(A15 == L[15].theDoubleArray());
            ASSERT(A15 == L.theDoubleArray(15));
            ASSERT(A15 == mL.theModifiableDoubleArray(15));

            if (veryVerbose) cout << "\tSTRING_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY == L.elemType(16));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY == elemTypesVec[16]);
            ASSERT(A16 == L[16].theStringArray());
            ASSERT(A16 == L.theStringArray(16));
            ASSERT(A16 == mL.theModifiableStringArray(16));

            if (veryVerbose) cout << "\tDATETIME_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY == L.elemType(17));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY == elemTypesVec[17]);
            ASSERT(A17 == L[17].theDatetimeArray());
            ASSERT(A17 == L.theDatetimeArray(17));
            ASSERT(A17 == mL.theModifiableDatetimeArray(17));

            if (veryVerbose) cout << "\tDATE_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY == L.elemType(18));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY == elemTypesVec[18]);
            ASSERT(A18 == L[18].theDateArray());
            ASSERT(A18 == L.theDateArray(18));
            ASSERT(A18 == mL.theModifiableDateArray(18));

            if (veryVerbose) cout << "\tTIME_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY == L.elemType(19));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY == elemTypesVec[19]);
            ASSERT(A19 == L[19].theTimeArray());
            ASSERT(A19 == L.theTimeArray(19));
            ASSERT(A19 == mL.theModifiableTimeArray(19));

            if (veryVerbose) cout << "\tLIST" << endl;
            ASSERT(bdem_ElemType::BDEM_LIST == L.elemType(20));
            ASSERT(bdem_ElemType::BDEM_LIST == elemTypesVec[20]);
            LOOP2_ASSERT(A20, L[20].theList(), A20 == L[20].theList());
            ASSERT(A20 == L.theList(20));
            ASSERT(A20 == mL.theModifiableList(20));

            if (veryVerbose) cout << "\tTABLE" << endl;
            ASSERT(bdem_ElemType::BDEM_TABLE == L.elemType(21));
            ASSERT(bdem_ElemType::BDEM_TABLE == elemTypesVec[21]);
            LOOP2_ASSERT(A21, L[21].theTable(), A21 == L[21].theTable());
            ASSERT(A21 == L.theTable(21));
            ASSERT(A21 == mL.theModifiableTable(21));

            if (veryVerbose) cout << "\tBOOL" << endl;
            L.elemTypes(&elemTypesVec);
            ASSERT(bdem_ElemType::BDEM_BOOL == L.elemType(22));
            ASSERT(bdem_ElemType::BDEM_BOOL == elemTypesVec[22]);
            ASSERT(A22 == L[22].theBool());
            ASSERT(A22 == L.theBool(22));
            ASSERT(A22 == mL.theModifiableBool(22));

            if (veryVerbose) cout << "\tDATETIMETZ" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ == L.elemType(23));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ == elemTypesVec[23]);
            ASSERT(A23 == L[23].theDatetimeTz());
            ASSERT(A23 == L.theDatetimeTz(23));
            ASSERT(A23 == mL.theModifiableDatetimeTz(23));

            if (veryVerbose) cout << "\tDATETZ" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETZ == L.elemType(24));
            ASSERT(bdem_ElemType::BDEM_DATETZ == elemTypesVec[24]);
            ASSERT(A24 == L[24].theDateTz());
            ASSERT(A24 == L.theDateTz(24));
            ASSERT(A24 == mL.theModifiableDateTz(24));

            if (veryVerbose) cout << "\tTIMETZ" << endl;
            ASSERT(bdem_ElemType::BDEM_TIMETZ == L.elemType(25));
            ASSERT(bdem_ElemType::BDEM_TIMETZ == elemTypesVec[25]);
            ASSERT(A25 == L[25].theTimeTz());
            ASSERT(A25 == L.theTimeTz(25));
            ASSERT(A25 == mL.theModifiableTimeTz(25));

            if (veryVerbose) cout << "\tBOOL_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY == L.elemType(26));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY == elemTypesVec[26]);
            ASSERT(A26 == L[26].theBoolArray());
            ASSERT(A26 == L.theBoolArray(26));
            ASSERT(A26 == mL.theModifiableBoolArray(26));

            if (veryVerbose) cout << "\tDATETIMETZ_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == L.elemType(27));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY == elemTypesVec[27]);
            ASSERT(A27 == L[27].theDatetimeTzArray());
            ASSERT(A27 == L.theDatetimeTzArray(27));
            ASSERT(A27 == mL.theModifiableDatetimeTzArray(27));

            if (veryVerbose) cout << "\tDATETZ_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY == L.elemType(28));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY == elemTypesVec[28]);
            ASSERT(A28 == L[28].theDateTzArray());
            ASSERT(A28 == L.theDateTzArray(28));
            ASSERT(A28 == mL.theModifiableDateTzArray(28));

            if (veryVerbose) cout << "\tTIMETZ_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY == L.elemType(29));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY == elemTypesVec[29]);
            ASSERT(A29 == L[29].theTimeTzArray());
            ASSERT(A29 == L.theTimeTzArray(29));
            ASSERT(A29 == mL.theModifiableTimeTzArray(29));

            if (veryVerbose) cout << "\tCHOICE " << endl;
            ASSERT(bdem_ElemType::BDEM_CHOICE == L.elemType(30));
            ASSERT(bdem_ElemType::BDEM_CHOICE == elemTypesVec[30]);
            ASSERT(A30 == L[30].theChoice());
            ASSERT(A30 == L.theChoice(30));
            ASSERT(A30 == mL.theModifiableChoice(30));

            if (veryVerbose) cout << "\tCHOICE_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY == L.elemType(31));
            ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY == elemTypesVec[31]);
            ASSERT(A31 == L[31].theChoiceArray());
            ASSERT(A31 == L.theChoiceArray(31));
            ASSERT(A31 == mL.theModifiableChoiceArray(31));
        }

        if (verbose) cout << "\nBackward List with all element types." << endl;
        {
            bslma_TestAllocator ta;
            bdem_List mL(&ta); const bdem_List& L = mL;
                                                    ASSERT( 0 == L.length());

            mL.appendChoiceArray(A31);              ASSERT( 1 == L.length());
            mL.appendChoice(A30);                   ASSERT( 2 == L.length());

            mL.appendTimeTzArray(A29);              ASSERT( 3 == L.length());
            mL.appendDateTzArray(A28);              ASSERT( 4 == L.length());
            mL.appendDatetimeTzArray(A27);          ASSERT( 5 == L.length());
            mL.appendBoolArray(A26);                ASSERT( 6 == L.length());
            mL.appendTimeTz(A25);                   ASSERT( 7 == L.length());
            mL.appendDateTz(A24);                   ASSERT( 8 == L.length());
            mL.appendDatetimeTz(A23);               ASSERT( 9 == L.length());
            mL.appendBool(A22);                     ASSERT(10 == L.length());

            mL.appendTable(A21);                    ASSERT(11 == L.length());
            mL.appendList(A20);                     ASSERT(12 == L.length());

            mL.appendTimeArray(A19);                ASSERT(13 == L.length());
            mL.appendDateArray(A18);                ASSERT(14 == L.length());
            mL.appendDatetimeArray(A17);            ASSERT(15 == L.length());
            mL.appendStringArray(A16);              ASSERT(16 == L.length());
            mL.appendDoubleArray(A15);              ASSERT(17 == L.length());
            mL.appendFloatArray(A14);               ASSERT(18 == L.length());
            mL.appendInt64Array(A13);               ASSERT(19 == L.length());
            mL.appendIntArray(A12);                 ASSERT(20 == L.length());
            mL.appendShortArray(A11);               ASSERT(21 == L.length());
            mL.appendCharArray(A10);                ASSERT(22 == L.length());

            mL.appendTime(A09);                     ASSERT(23 == L.length());
            mL.appendDate(A08);                     ASSERT(24 == L.length());
            mL.appendDatetime(A07);                 ASSERT(25 == L.length());
            mL.appendString(A06);                   ASSERT(26 == L.length());
            mL.appendDouble(A05);                   ASSERT(27 == L.length());
            mL.appendFloat(A04);                    ASSERT(28 == L.length());
            mL.appendInt64(A03);                    ASSERT(29 == L.length());
            mL.appendInt(A02);                      ASSERT(30 == L.length());
            mL.appendShort(A01);                    ASSERT(31 == L.length());
            mL.appendChar(A00);                     ASSERT(32 == L.length());

            L.elemTypes(&elemTypesVec);

            if (veryVerbose) cout << "\tCHAR" << endl;
            ASSERT(bdem_ElemType::BDEM_CHAR == L.elemType(31 - 0));
            ASSERT(bdem_ElemType::BDEM_CHAR == elemTypesVec[31 - 0]);
            ASSERT(A00 == L[31 - 0].theChar());
            ASSERT(A00 == L.theChar(31 - 0));
            ASSERT(A00 == mL.theModifiableChar(31 - 0));

            if (veryVerbose) cout << "\tSHORT" << endl;
            ASSERT(bdem_ElemType::BDEM_SHORT == L.elemType(31 - 1));
            ASSERT(bdem_ElemType::BDEM_SHORT == elemTypesVec[31 - 1]);
            ASSERT(A01 == L[31 - 1].theShort());
            ASSERT(A01 == L.theShort(31 - 1));
            ASSERT(A01 == mL.theModifiableShort(31 - 1));

            if (veryVerbose) cout << "\tINT" << endl;
            ASSERT(bdem_ElemType::BDEM_INT == L.elemType(31 - 2));
            ASSERT(bdem_ElemType::BDEM_INT == elemTypesVec[31 - 2]);
            ASSERT(A02 == L[31 - 2].theInt());
            ASSERT(A02 == L.theInt(31 - 2));
            ASSERT(A02 == mL.theModifiableInt(31 - 2));

            if (veryVerbose) cout << "\tINT64" << endl;
            ASSERT(bdem_ElemType::BDEM_INT64 == L.elemType(31 - 3));
            ASSERT(bdem_ElemType::BDEM_INT64 == elemTypesVec[31 - 3]);
            ASSERT(A03 == L[31 - 3].theInt64());
            ASSERT(A03 == L.theInt64(31 - 3));
            ASSERT(A03 == mL.theModifiableInt64(31 - 3));

            if (veryVerbose) cout << "\tFLOAT" << endl;
            ASSERT(bdem_ElemType::BDEM_FLOAT == L.elemType(31 - 4));
            ASSERT(bdem_ElemType::BDEM_FLOAT == elemTypesVec[31 - 4]);
            ASSERT(A04 == L[31 - 4].theFloat());
            ASSERT(A04 == L.theFloat(31 - 4));
            ASSERT(A04 == mL.theModifiableFloat(31 - 4));

            if (veryVerbose) cout << "\tDOUBLE" << endl;
            ASSERT(bdem_ElemType::BDEM_DOUBLE == L.elemType(31 - 5));
            ASSERT(bdem_ElemType::BDEM_DOUBLE == elemTypesVec[31 - 5]);
            ASSERT(A05 == L[31 - 5].theDouble());
            ASSERT(A05 == L.theDouble(31 - 5));
            ASSERT(A05 == mL.theModifiableDouble(31 - 5));

            if (veryVerbose) cout << "\tSTRING" << endl;
            ASSERT(bdem_ElemType::BDEM_STRING == L.elemType(31 - 6));
            ASSERT(bdem_ElemType::BDEM_STRING == elemTypesVec[31 - 6]);
            ASSERT(A06 == L[31 - 6].theString());
            ASSERT(A06 == L.theString(31 - 6));
            ASSERT(A06 == mL.theModifiableString(31 - 6));

            if (veryVerbose) cout << "\tDATETIME" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETIME == L.elemType(31 - 7));
            ASSERT(bdem_ElemType::BDEM_DATETIME == elemTypesVec[31 - 7]);
            ASSERT(A07 == L[31 - 7].theDatetime());
            ASSERT(A07 == L.theDatetime(31 - 7));
            ASSERT(A07 == mL.theModifiableDatetime(31 - 7));

            if (veryVerbose) cout << "\tDATE" << endl;
            ASSERT(bdem_ElemType::BDEM_DATE == L.elemType(31 - 8));
            ASSERT(bdem_ElemType::BDEM_DATE == elemTypesVec[31 - 8]);
            ASSERT(A08 == L[31 - 8].theDate());
            ASSERT(A08 == L.theDate(31 - 8));
            ASSERT(A08 == mL.theModifiableDate(31 - 8));

            if (veryVerbose) cout << "\tTIME" << endl;
            ASSERT(bdem_ElemType::BDEM_TIME == L.elemType(31 - 9));
            ASSERT(bdem_ElemType::BDEM_TIME == elemTypesVec[31 - 9]);
            ASSERT(A09 == L[31 - 9].theTime());
            ASSERT(A09 == L.theTime(31 - 9));
            ASSERT(A09 == mL.theModifiableTime(31 - 9));

            if (veryVerbose) cout << "\tCHAR_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY == L.elemType(31 - 10));
            ASSERT(bdem_ElemType::BDEM_CHAR_ARRAY == elemTypesVec[31 - 10]);
            ASSERT(A10 == L[31 - 10].theCharArray());
            ASSERT(A10 == L.theCharArray(31 - 10));
            ASSERT(A10 == mL.theModifiableCharArray(31 - 10));

            if (veryVerbose) cout << "\tSHORT_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY == L.elemType(31 - 11));
            ASSERT(bdem_ElemType::BDEM_SHORT_ARRAY == elemTypesVec[31 - 11]);
            ASSERT(A11 == L[31 - 11].theShortArray());
            ASSERT(A11 == L.theShortArray(31 - 11));
            ASSERT(A11 == mL.theModifiableShortArray(31 - 11));

            if (veryVerbose) cout << "\tINT_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY == L.elemType(31 - 12));
            ASSERT(bdem_ElemType::BDEM_INT_ARRAY == elemTypesVec[31 - 12]);
            ASSERT(A12 == L[31 - 12].theIntArray());
            ASSERT(A12 == L.theIntArray(31 - 12));
            ASSERT(A12 == mL.theModifiableIntArray(31 - 12));

            if (veryVerbose) cout << "\tINT64_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY == L.elemType(31 - 13));
            ASSERT(bdem_ElemType::BDEM_INT64_ARRAY == elemTypesVec[31 - 13]);
            ASSERT(A13 == L[31 - 13].theInt64Array());
            ASSERT(A13 == L.theInt64Array(31 - 13));
            ASSERT(A13 == mL.theModifiableInt64Array(31 - 13));

            if (veryVerbose) cout << "\tFLOAT_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY == L.elemType(31 - 14));
            ASSERT(bdem_ElemType::BDEM_FLOAT_ARRAY == elemTypesVec[31 - 14]);
            ASSERT(A14 == L[31 - 14].theFloatArray());
            ASSERT(A14 == L.theFloatArray(31 - 14));
            ASSERT(A14 == mL.theModifiableFloatArray(31 - 14));

            if (veryVerbose) cout << "\tDOUBLE_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY == L.elemType(31 - 15));
            ASSERT(bdem_ElemType::BDEM_DOUBLE_ARRAY == elemTypesVec[31 - 15]);
            ASSERT(A15 == L[31 - 15].theDoubleArray());
            ASSERT(A15 == L.theDoubleArray(31 - 15));
            ASSERT(A15 == mL.theModifiableDoubleArray(31 - 15));

            if (veryVerbose) cout << "\tSTRING_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY == L.elemType(31 - 16));
            ASSERT(bdem_ElemType::BDEM_STRING_ARRAY == elemTypesVec[31 - 16]);
            ASSERT(A16 == L[31 - 16].theStringArray());
            ASSERT(A16 == L.theStringArray(31 - 16));
            ASSERT(A16 == mL.theModifiableStringArray(31 - 16));

            if (veryVerbose) cout << "\tDATETIME_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY == L.elemType(31 - 17));
            ASSERT(bdem_ElemType::BDEM_DATETIME_ARRAY ==
                                                        elemTypesVec[31 - 17]);
            ASSERT(A17 == L[31 - 17].theDatetimeArray());
            ASSERT(A17 == L.theDatetimeArray(31 - 17));
            ASSERT(A17 == mL.theModifiableDatetimeArray(31 - 17));

            if (veryVerbose) cout << "\tDATE_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY == L.elemType(31 - 18));
            ASSERT(bdem_ElemType::BDEM_DATE_ARRAY == elemTypesVec[31 - 18]);
            ASSERT(A18 == L[31 - 18].theDateArray());
            ASSERT(A18 == L.theDateArray(31 - 18));
            ASSERT(A18 == mL.theModifiableDateArray(31 - 18));

            if (veryVerbose) cout << "\tTIME_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY == L.elemType(31 - 19));
            ASSERT(bdem_ElemType::BDEM_TIME_ARRAY == elemTypesVec[31 - 19]);
            ASSERT(A19 == L[31 - 19].theTimeArray());
            ASSERT(A19 == L.theTimeArray(31 - 19));
            ASSERT(A19 == mL.theModifiableTimeArray(31 - 19));

            if (veryVerbose) cout << "\tLIST" << endl;
            ASSERT(bdem_ElemType::BDEM_LIST == L.elemType(31 - 20));
            ASSERT(bdem_ElemType::BDEM_LIST == elemTypesVec[31 - 20]);
            ASSERT(A20 == L[31 - 20].theList());
            ASSERT(A20 == L.theList(31 - 20));
            ASSERT(A20 == mL.theModifiableList(31 - 20));

            if (veryVerbose) cout << "\tTABLE" << endl;
            ASSERT(bdem_ElemType::BDEM_TABLE == L.elemType(31 - 21));
            ASSERT(bdem_ElemType::BDEM_TABLE == elemTypesVec[31 - 21]);
            ASSERT(A21 == L[31 - 21].theTable());
            ASSERT(A21 == L.theTable(31 - 21));
            ASSERT(A21 == mL.theModifiableTable(31 - 21));

            if (veryVerbose) cout << "\tBOOL" << endl;
            L.elemTypes(&elemTypesVec);
            ASSERT(bdem_ElemType::BDEM_BOOL == L.elemType(31 - 22));
            ASSERT(bdem_ElemType::BDEM_BOOL == elemTypesVec[31 - 22]);
            ASSERT(A22 == L[31 - 22].theBool());
            ASSERT(A22 == L.theBool(31 - 22));
            ASSERT(A22 == mL.theModifiableBool(31 - 22));

            if (veryVerbose) cout << "\tDATETIMETZ" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ == L.elemType(31 - 23));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ == elemTypesVec[31 - 23]);
            ASSERT(A23 == L[31 - 23].theDatetimeTz());
            ASSERT(A23 == L.theDatetimeTz(31 - 23));
            ASSERT(A23 == mL.theModifiableDatetimeTz(31 - 23));

            if (veryVerbose) cout << "\tDATETZ" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETZ == L.elemType(31 - 24));
            ASSERT(bdem_ElemType::BDEM_DATETZ == elemTypesVec[31 - 24]);
            ASSERT(A24 == L[31 - 24].theDateTz());
            ASSERT(A24 == L.theDateTz(31 - 24));
            ASSERT(A24 == mL.theModifiableDateTz(31 - 24));

            if (veryVerbose) cout << "\tTIMETZ" << endl;
            ASSERT(bdem_ElemType::BDEM_TIMETZ == L.elemType(31 - 25));
            ASSERT(bdem_ElemType::BDEM_TIMETZ == elemTypesVec[31 - 25]);
            ASSERT(A25 == L[31 - 25].theTimeTz());
            ASSERT(A25 == L.theTimeTz(31 - 25));
            ASSERT(A25 == mL.theModifiableTimeTz(31 - 25));

            if (veryVerbose) cout << "\tBOOL_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY == L.elemType(31 - 26));
            ASSERT(bdem_ElemType::BDEM_BOOL_ARRAY == elemTypesVec[31 - 26]);
            ASSERT(A26 == L[31 - 26].theBoolArray());
            ASSERT(A26 == L.theBoolArray(31 - 26));
            ASSERT(A26 == mL.theModifiableBoolArray(31 - 26));

            if (veryVerbose) cout << "\tDATETIMETZ_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY ==
                                                          L.elemType(31 - 27));
            ASSERT(bdem_ElemType::BDEM_DATETIMETZ_ARRAY ==
                                                        elemTypesVec[31 - 27]);
            ASSERT(A27 == L[31 - 27].theDatetimeTzArray());
            ASSERT(A27 == L.theDatetimeTzArray(31 - 27));
            ASSERT(A27 == mL.theModifiableDatetimeTzArray(31 - 27));

            if (veryVerbose) cout << "\tDATETZ_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY == L.elemType(31 - 28));
            ASSERT(bdem_ElemType::BDEM_DATETZ_ARRAY == elemTypesVec[31 - 28]);
            ASSERT(A28 == L[31 - 28].theDateTzArray());
            ASSERT(A28 == L.theDateTzArray(31 - 28));
            ASSERT(A28 == mL.theModifiableDateTzArray(31 - 28));

            if (veryVerbose) cout << "\tTIMETZ_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY == L.elemType(31 - 29));
            ASSERT(bdem_ElemType::BDEM_TIMETZ_ARRAY == elemTypesVec[31 - 29]);
            ASSERT(A29 == L[31 - 29].theTimeTzArray());
            ASSERT(A29 == L.theTimeTzArray(31 - 29));
            ASSERT(A29 == mL.theModifiableTimeTzArray(31 - 29));

            if (veryVerbose) cout << "\tCHOICE " << endl;
            ASSERT(bdem_ElemType::BDEM_CHOICE == L.elemType(31 - 30));
            ASSERT(bdem_ElemType::BDEM_CHOICE == elemTypesVec[31 - 30]);
            ASSERT(A30 == L[31 - 30].theChoice());
            ASSERT(A30 == L.theChoice(31 - 30));
            ASSERT(A30 == mL.theModifiableChoice(31 - 30));

            if (veryVerbose) cout << "\tCHOICE_ARRAY" << endl;
            ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY == L.elemType(31 - 31));
            ASSERT(bdem_ElemType::BDEM_CHOICE_ARRAY == elemTypesVec[31 - 31]);
            ASSERT(A31 == L[31 - 31].theChoiceArray());
            ASSERT(A31 == L.theChoiceArray(31 - 31));
            ASSERT(A31 == mL.theModifiableChoiceArray(31 - 31));
        }
}

static void testCase3(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // PRIMARY CONSTRUCTOR
        //   Ensure that this constructor is "wired-up" and defaults properly.
        //
        // Concerns:
        //   - That the default allocator comes from 'bslma_Default::allocator'
        //   - That changing the default allocator after construction has
        //      no effect on an existing 'bdem_List' object.
        //   - That the primary constructor defaults to BDEM_PASS_THROUGH mode.
        //
        // Plan:
        //   - To ensure that the basic constructor takes its allocator by
        //      default from 'bslma_Default::allocator', install a separate
        //      instance of 'bdem_TestAllocator' as the default allocator and
        //      use its instance-specific statistics to verify that it is in
        //      fact the source of default allocations and deallocations.
        //   - To ensure that changing the default allocator has no effect
        //      on existing list objects, swap it out and then continue to
        //      allocate additional objects.
        //   - To ensure that the default mode is 'BDEM_PASS_THROUGH', install
        //      and remove an instance of a long string two times and use the
        //      test allocator's statistics to verify that memory is being
        //      allocated exactly as if 'BDEM_PASS_THROUGH' were supplied.
        //      Also perform the same test on the other three modes to show
        //      that in each of those cases the statistics are different.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   bdem_List(bslma_Allocator *basicAllocator = 0);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY CONSTRUCTOR" << endl
                          << "===================" << endl;

        if (verbose) cout <<
            "\nEnsure bdem_Default::allocator() is used by default." << endl;
        {

            bslma_TestAllocator da; // This allocator must be installed as the
                                    // default in order to pass this test.

            if (verbose) cout <<
                       "\tInstall test allocator 'da' as the default." << endl;

            {
                const bslma_DefaultAllocatorGuard dag(&da);

                ASSERT(0 == da.numBlocksTotal());
                bdem_List x;
                const int NBT = da.numBlocksTotal();
                ASSERT(NBT > 0);

                bslma_TestAllocator ta;
                ASSERT(0 == ta.numBlocksTotal());

                bdem_List y(&ta);
                ASSERT(NBT == da.numBlocksTotal());
                ASSERT(NBT == ta.numBlocksTotal());

                bslma_TestAllocator oa; // Installing this other allocator
                                        // should have no effect on subsequent
                                        // use of pre-existing list objects.
                if (verbose) cout <<
                       "\tInstall test allocator 'oa' as the default." << endl;
                {
                    const bslma_DefaultAllocatorGuard oag(&oa);

                    ASSERT(0 == oa.numBlocksTotal());
                    bdem_List z;
                    ASSERT(NBT == oa.numBlocksTotal());

                    ASSERT(NBT == da.numBlocksTotal());
                    ASSERT(NBT == ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());

                    x.appendNullStringArray(); // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT == ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());

                    y.appendNullStringArray(); // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT <  ta.numBlocksTotal());
                    ASSERT(NBT == oa.numBlocksTotal());

                    z.appendNullStringArray(); // Note: method not yet tested.

                    ASSERT(NBT <  da.numBlocksTotal());
                    ASSERT(NBT <  ta.numBlocksTotal());
                    ASSERT(NBT <  oa.numBlocksTotal());

                    ASSERT(ta.numBlocksTotal() == da.numBlocksTotal());
                    ASSERT(ta.numBlocksTotal() == oa.numBlocksTotal());
                }

                if (verbose) cout <<
                    "\tUn-install test allocator 'oa' as the default." << endl;

            }
            if (verbose) cout <<
                    "\tUn-install test allocator 'da' as the default." << endl;
        }

        if (verbose)
            cout << "\nEnsure default mode is BDEM_PASS_THROUGH." << endl;

        bslma_TestAllocator aX, aA, aB, aC, aD;
        {
            const int N0X = aX.numBlocksInUse(), M0X = aX.numBytesInUse();
            const int N0A = aA.numBlocksInUse(), M0A = aA.numBytesInUse();
            const int N0B = aB.numBlocksInUse(), M0B = aB.numBytesInUse();
            const int N0C = aC.numBlocksInUse(), M0C = aC.numBytesInUse();
            const int N0D = aD.numBlocksInUse(), M0D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N0X); P_(N0A); P_(N0B); P_(N0C); P(N0D);
                T_; T_; P_(M0X); P_(M0A); P_(M0B); P_(M0C); P(M0D);
            }

            if (verbose) cout << "\tCreate five lists." << endl;

            bdem_List listX(                                         &aX);
            bdem_List listA(bdem_AggregateOption::BDEM_PASS_THROUGH, &aA);
            bdem_List listB(bdem_AggregateOption::BDEM_WRITE_ONCE,   &aB);
            bdem_List listC(bdem_AggregateOption::BDEM_WRITE_MANY,   &aC);
            //bdem_List listD(bdem_AggregateOption::BDEM_SUBORDINATE,&aD);
            bdem_List listD(bdem_AggregateOption::BDEM_WRITE_MANY,   &aD);
                                                                      // dummy
            // As it turns out, we would get a memory leak if BDEM_SUBORDINATE
            // were the default, and so this case is addressed as well
            // because the test allocator would report the leak and assert.

            const int N1X = aX.numBlocksInUse(), M1X = aX.numBytesInUse();
            const int N1A = aA.numBlocksInUse(), M1A = aA.numBytesInUse();
            const int N1B = aB.numBlocksInUse(), M1B = aB.numBytesInUse();
            const int N1C = aC.numBlocksInUse(), M1C = aC.numBytesInUse();
            const int N1D = aD.numBlocksInUse(), M1D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N1X); P_(N1A); P_(N1B); P_(N1C); P(N1D);
                T_; T_; P_(M1X); P_(M1A); P_(M1B); P_(M1C); P(M1D);
            }

            const char *const S = "These are the times that try men's soles.";

            if (verbose) cout << "\tAppend a long string." << endl;

            listX.appendString(S);
            listA.appendString(S);
            listB.appendString(S);
            listC.appendString(S);
            listD.appendString(S);

            const int N2X = aX.numBlocksInUse(), M2X = aX.numBytesInUse();
            const int N2A = aA.numBlocksInUse(), M2A = aA.numBytesInUse();
            const int N2B = aB.numBlocksInUse(), M2B = aB.numBytesInUse();
            const int N2C = aC.numBlocksInUse(), M2C = aC.numBytesInUse();
            const int N2D = aD.numBlocksInUse(), M2D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N2X); P_(N2A); P_(N2B); P_(N2C); P(N2D);
                T_; T_; P_(M2X); P_(M2A); P_(M2B); P_(M2C); P(M2D);
            }

            if (verbose) cout << "\tRemove that string element." << endl;

            listX.removeElement(0);
            listA.removeElement(0);
            listB.removeElement(0);
            listC.removeElement(0);
            listD.removeElement(0);

            const int N3X = aX.numBlocksInUse(), M3X = aX.numBytesInUse();
            const int N3A = aA.numBlocksInUse(), M3A = aA.numBytesInUse();
            const int N3B = aB.numBlocksInUse(), M3B = aB.numBytesInUse();
            const int N3C = aC.numBlocksInUse(), M3C = aC.numBytesInUse();
            const int N3D = aD.numBlocksInUse(), M3D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N3X); P_(N3A); P_(N3B); P_(N3C); P(N3D);
                T_; T_; P_(M3X); P_(M3A); P_(M3B); P_(M3C); P(M3D);
            }

            if (verbose) cout << "\tAppend the long string again." << endl;

            listX.appendString(S);
            listA.appendString(S);
            listB.appendString(S);
            listC.appendString(S);
            listD.appendString(S);

            const int N4X = aX.numBlocksInUse(), M4X = aX.numBytesInUse();
            const int N4A = aA.numBlocksInUse(), M4A = aA.numBytesInUse();
            const int N4B = aB.numBlocksInUse(), M4B = aB.numBytesInUse();
            const int N4C = aC.numBlocksInUse(), M4C = aC.numBytesInUse();
            const int N4D = aD.numBlocksInUse(), M4D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N4X); P_(N4A); P_(N4B); P_(N4C); P(N4D);
                T_; T_; P_(M4X); P_(M4A); P_(M4B); P_(M4C); P(M4D);
            }

            if (verbose) cout << "\tRemove that string element again." << endl;

            listX.removeElement(0);
            listA.removeElement(0);
            listB.removeElement(0);
            listC.removeElement(0);
            listD.removeElement(0);

            const int N5X = aX.numBlocksInUse(), M5X = aX.numBytesInUse();
            const int N5A = aA.numBlocksInUse(), M5A = aA.numBytesInUse();
            const int N5B = aB.numBlocksInUse(), M5B = aB.numBytesInUse();
            const int N5C = aC.numBlocksInUse(), M5C = aC.numBytesInUse();
            const int N5D = aD.numBlocksInUse(), M5D = aD.numBytesInUse();

            if (veryVerbose) {
                T_; T_; P_(N5X); P_(N5A); P_(N5B); P_(N5C); P(N5D);
                T_; T_; P_(M5X); P_(M5A); P_(M5B); P_(M5C); P(M5D);
            }

            if (verbose) cout << "\tEnsure BDEM_PASS_THROUGH's the default."
                              << endl;

            ASSERT(N5A == N5X);         ASSERT(M5A == M5X);
            ASSERT(N5B != N5X);         ASSERT(M5B != M5X);
            ASSERT(N5C != N5X);         ASSERT(M5C != M5X);
            ASSERT(N5D != N5X);         ASSERT(M5D != M5X);
        }
}

static void testCase2(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //   Verify the equipment we have set up to test itself works properly.
        //
        // Concerns:
        //   - That all three distinct instances of each of the element
        //      types was instantiated correctly.
        //   - That all of the Nxx values are respectively "NULL".
        //   - That '==' and !=' for 'bdem_List' work as expected (and
        //      specifically with respect to the exemplar list elements 'A20',
        //      'B20', and 'N20').  Note that the general functionality has
        //      not yet been tested.
        //   - That '==' and !=' for the dummy classes work as expected.
        //   - There was a bug in 'bslma_TestAllocator' on an old Cygwin!
        //
        // Plan:
        //   - Create all 90 instances at file scope to facilitate reuse.
        //   - Display these values only in 'veryVerbose' mode.
        //   - Use the native operator == to verify that each instance of
        //      a given element type differs from the other two.
        //   - Verify that each N0x scalar holds its respective "null" value.
        //   - Verify that each N1x array has length a length of zero;
        //   - Take extra measures with list and table, as their operator
        //      equals have not yet been verified.
        // Note: these two items have been removed:
        //   - Expose the 'bslma_TestAllocator' bug on the old Cygwin!!!
        //   - *** Duplicate this test in 'bdema_testallocator.t.cpp' *** !!!
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   TEST APPARATUS
        //   CONCERN: That operator== and operator!= work properly on tables.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST APPARATUS" << endl
                          << "==============" << endl;

        if (verbose) cout <<
                "\nVerify all 90 static element instances." << endl;

        if (veryVerbose) {
            P_(A00); P_(B00); P(N00);   // Scalar values
            P_(A01); P_(B01); P(N01);
            P_(A02); P_(B02); P(N02);
            P_(A03); P_(B03); P(N03);
            P_(A04); P_(B04); P(N04);
            P_(A05); P_(B05); P(N05);
            P_(A06); P_(B06); P(N06);
            P_(A07); P_(B07); P(N07);
            P_(A08); P_(B08); P(N08);
            P_(A09); P_(B09); P(N09);

            P_(A10); P_(B10); P(N10);   // Array values
            P_(A11); P_(B11); P(N11);
            P_(A12); P_(B12); P(N12);
            P_(A13); P_(B13); P(N13);
            P_(A14); P_(B14); P(N14);
            P_(A15); P_(B15); P(N15);
            P_(A16); P_(B16); P(N16);
            P_(A17); P_(B17); P(N17);
            P_(A18); P_(B18); P(N18);
            P_(A19); P_(B19); P(N19);

            P_(A20); P_(B20); P(N20);   // Aggregate values
            P_(A21); P_(B21); P(N21);

            P_(A22); P_(B22); P(N22);
            P_(A23); P_(B23); P(N23);
            P_(A24); P_(B24); P(N24);
            P_(A25); P_(B25); P(N25);
            P_(A26); P_(B26); P(N26);
            P_(A27); P_(B27); P(N27);
            P_(A28); P_(B28); P(N28);
            P_(A29); P_(B29); P(N29);
            P_(A30); P_(B30); P(N30);
            P_(A31); P_(B31); P(N31);
        }

        if (verbose) cout <<
                "\tThat each set of 3 instances has distinct values." << endl;
        ASSERT(A00 != B00); ASSERT(B00 != N00); ASSERT(N00 != A00);
        ASSERT(A01 != B01); ASSERT(B01 != N01); ASSERT(N01 != A01);
        ASSERT(A02 != B02); ASSERT(B02 != N02); ASSERT(N02 != A02);
        ASSERT(A03 != B03); ASSERT(B03 != N03); ASSERT(N03 != A03);
        ASSERT(A04 != B04); ASSERT(B04 != N04); ASSERT(N04 != A04);
        ASSERT(A05 != B05); ASSERT(B05 != N05); ASSERT(N05 != A05);
        ASSERT(A06 != B06); ASSERT(B06 != N06); ASSERT(N06 != A06);
        ASSERT(A07 != B07); ASSERT(B07 != N07); ASSERT(N07 != A07);
        ASSERT(A08 != B08); ASSERT(B08 != N08); ASSERT(N08 != A08);
        ASSERT(A09 != B09); ASSERT(B09 != N09); ASSERT(N09 != A09);

        ASSERT(A10 != B10); ASSERT(B10 != N10); ASSERT(N10 != A10);
        ASSERT(A11 != B11); ASSERT(B11 != N11); ASSERT(N11 != A11);
        ASSERT(A12 != B12); ASSERT(B12 != N12); ASSERT(N12 != A12);
        ASSERT(A13 != B13); ASSERT(B13 != N13); ASSERT(N13 != A13);
        ASSERT(A14 != B14); ASSERT(B14 != N14); ASSERT(N14 != A14);
        ASSERT(A15 != B15); ASSERT(B15 != N15); ASSERT(N15 != A15);
        ASSERT(A16 != B16); ASSERT(B16 != N16); ASSERT(N16 != A16);
        ASSERT(A17 != B17); ASSERT(B17 != N17); ASSERT(N17 != A17);
        ASSERT(A18 != B18); ASSERT(B18 != N18); ASSERT(N18 != A18);
        ASSERT(A19 != B19); ASSERT(B19 != N19); ASSERT(N19 != A19);

        ASSERT(A20 != B20); ASSERT(B20 != N20); ASSERT(N20 != A20);
        ASSERT(A21 != B21); ASSERT(B21 != N21); ASSERT(N21 != A21);

        ASSERT(A22 != B22); ASSERT(B22 == N22); ASSERT(N22 != A22);
        ASSERT(A23 != B23); ASSERT(B23 != N23); ASSERT(N23 != A23);
        ASSERT(A24 != B24); ASSERT(B24 != N24); ASSERT(N24 != A24);
        ASSERT(A25 != B25); ASSERT(B25 != N25); ASSERT(N25 != A25);
        ASSERT(A26 != B26); ASSERT(B26 != N26); ASSERT(N26 != A26);
        ASSERT(A27 != B27); ASSERT(B27 != N27); ASSERT(N27 != A27);
        ASSERT(A28 != B28); ASSERT(B28 != N28); ASSERT(N28 != A28);
        ASSERT(A29 != B29); ASSERT(B29 != N29); ASSERT(N29 != A29);

        ASSERT(A30 != B30); ASSERT(B30 != N30); ASSERT(N30 != A30);
        ASSERT(A31 != B31); ASSERT(B31 != N31); ASSERT(N31 != A31);

        if (verbose) cout <<
                "\tVerify N00 - N09 have unset values." << endl;

        ASSERT(1 == bdetu_Unset<char>::isUnset(N00));
        ASSERT(1 == bdetu_Unset<short>::isUnset(N01));
        ASSERT(1 == bdetu_Unset<int>::isUnset(N02));
        ASSERT(1 == bdetu_Unset<bsls_Types::Int64>::isUnset(N03));
        ASSERT(1 == bdetu_Unset<float>::isUnset(N04));
        ASSERT(1 == bdetu_Unset<double>::isUnset(N05));
        ASSERT(1 == bdetu_Unset<bsl::string>::isUnset(N06));
        ASSERT(1 == bdetu_Unset<bdet_Datetime>::isUnset(N07));
        ASSERT(1 == bdetu_Unset<bdet_Date>::isUnset(N08));
        ASSERT(1 == bdetu_Unset<bdet_Time>::isUnset(N09));

        ASSERT(1 == bdetu_Unset<bool>::isUnset(N22));
        ASSERT(1 == bdetu_Unset<bdet_DatetimeTz>::isUnset(N23));
        ASSERT(1 == bdetu_Unset<bdet_DateTz>::isUnset(N24));
        ASSERT(1 == bdetu_Unset<bdet_TimeTz>::isUnset(N25));

        if (verbose) cout <<
                "\tVerify N10 - N19 have length zero." << endl;

        ASSERT(0 == N10.size());
        ASSERT(0 == N11.size());
        ASSERT(0 == N12.size());
        ASSERT(0 == N13.size());
        ASSERT(0 == N14.size());
        ASSERT(0 == N15.size());
        ASSERT(0 == N16.size());
        ASSERT(0 == N17.size());
        ASSERT(0 == N18.size());
        ASSERT(0 == N19.size());

        ASSERT(0 == N26.size());
        ASSERT(0 == N27.size());
        ASSERT(0 == N28.size());
        ASSERT(0 == N29.size());

        if (verbose)
            cout << "\tThat list, table, choice and choicearray values "
                 << "compare == with self." << endl;

        ASSERT(A20 == A20); ASSERT(B20 == B20); ASSERT(N20 == N20);
        ASSERT(A21 == A21); ASSERT(B21 == B21); ASSERT(N21 == N21);
        ASSERT(A30 == A30); ASSERT(B30 == B30); ASSERT(N30 == N30);
        ASSERT(A31 == A31); ASSERT(B31 == B31); ASSERT(N31 == N31);

        ASSERT(!(A20 == B20)); ASSERT(!(B20 == N20)); ASSERT(!(N20 == A20));
        ASSERT(!(A21 == B21)); ASSERT(!(B21 == N21)); ASSERT(!(N21 == A21));
        ASSERT(!(A30 == B30)); ASSERT(!(B30 == N30)); ASSERT(!(N30 == A30));
        ASSERT(!(A31 == B31)); ASSERT(!(B31 == N31)); ASSERT(!(N31 == A31));

        if (verbose)
            cout << "\tThat list values are as they should be." << endl;

        ASSERT(4 == A20.length());
        ASSERT(bdem_ElemType::BDEM_INT == A20.elemType(0));
                                        ASSERT(A02 == A20[0].theInt());
        ASSERT(bdem_ElemType::BDEM_DOUBLE == A20.elemType(1));
                                        ASSERT(A05 == A20[1].theDouble());
        ASSERT(bdem_ElemType::BDEM_STRING == A20.elemType(2));
                                        ASSERT(A06 == A20[2].theString());
        ASSERT(bdem_ElemType::BDEM_STRING_ARRAY == A20.elemType(3));
                                        ASSERT(A16 == A20[3].theStringArray());
        ASSERT(4 == B20.length());
        ASSERT(bdem_ElemType::BDEM_INT == B20.elemType(0));
                                        ASSERT(B02 == B20[0].theInt());
        ASSERT(bdem_ElemType::BDEM_DOUBLE == B20.elemType(1));
                                        ASSERT(B05 == B20[1].theDouble());
        ASSERT(bdem_ElemType::BDEM_STRING == B20.elemType(2));
                                        ASSERT(B06 == B20[2].theString());
        ASSERT(bdem_ElemType::BDEM_STRING_ARRAY == B20.elemType(3));
                                        ASSERT(B16 == B20[3].theStringArray());
        ASSERT(0 == N20.length());
}

static void testCase1(bool verbose, bool veryVerbose, bool veryVeryVerbose) {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //   Note: This test exercises basic functionality, but tests nothing!
        //
        // Concerns:
        //   - That basic essential functionality is superficially operational.
        //   - Provide "Developers' Sandbox".
        //
        // Plan:
        //  Do whatever is needed:
        //   - Try basic append, accessor, and print functionality on lists.
        //   - Ensure we can create and use a simplified *dummy* 'bdem_Table'.
        //   - Isolate Cygwin "alling" bug which was also causing a memory
        //      leak involving the default allocator.
        //
        // Tactics:
        //   - Ad-Hoc Data Selection Method
        //   - Brute-Force Implementation Technique
        //
        // Testing:
        //   BREATHING TEST
        //   CONCERN: That basic operations on tables (seem to) work properly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\tSizeof bdem_List :"
                          << sizeof(bdem_List) << bsl::endl;

        if (verbose) cout <<
            "\nExercise append, length, elemType, op[], and printing." << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            bdem_List mL; const bdem_List& L = mL;
            if (verbose) { cout << "\tCreate an empty list: "; P(L) }
            ASSERT(0 == L.length());
            if (veryVerbose) { P_(da.numBytesTotal()) P(da.numBlocksTotal()); }

            mL.appendInt(123);
            if (verbose) { cout << "\tAppend INT 123: "; P(L) }
            ASSERT(1 == L.length());
            ASSERT(bdem_ElemType::BDEM_INT == L.elemType(0));
            ASSERT(123 == L[0].theInt());
            ASSERT(123 == L.theInt(0));
            if (veryVerbose) { P_(da.numBytesTotal()) P(da.numBlocksTotal()); }

            mL.appendDouble(25.75);
            if (verbose) { cout << "\tAppend DOUBLE 25.75: "; P(L) }
            ASSERT(2 == L.length());
            ASSERT(bdem_ElemType::BDEM_INT == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DOUBLE == L.elemType(1));
            ASSERT(123 == L[0].theInt());
            ASSERT(123 == L.theInt(0));
            ASSERT(25.75 == L[1].theDouble());
            ASSERT(25.75 == L.theDouble(1));
            if (veryVerbose) { P_(da.numBytesTotal()) P(da.numBlocksTotal()); }

            mL.appendString(bsl::string("foo")); // explicitly bsl::string
            if (verbose) { cout << "\tAppend STRING \"foo\": "; P(L) }
            ASSERT(3 == L.length());
            ASSERT(bdem_ElemType::BDEM_INT == L.elemType(0));
            ASSERT(bdem_ElemType::BDEM_DOUBLE == L.elemType(1));
            ASSERT(bdem_ElemType::BDEM_STRING == L.elemType(2));
            ASSERT(123 == L[0].theInt());
            ASSERT(123 == mL[0].theModifiableInt());
            ASSERT(123 == L.theInt(0));
            ASSERT(123 == mL.theModifiableInt(0));
            ASSERT(25.75 == L[1].theDouble());
            ASSERT(25.75 == mL[1].theModifiableDouble());
            ASSERT(25.75 == L.theDouble(1));
            ASSERT(25.75 == mL.theModifiableDouble(1));
            ASSERT("foo" == L[2].theString());
            ASSERT("foo" == mL[2].theModifiableString());
            ASSERT("foo" == L.theString(2));
            ASSERT("foo" == mL.theModifiableString(2));
            if (veryVerbose) { P_(da.numBytesTotal()) P(da.numBlocksTotal()); }
        } // default allocator da is un-installed and then destroyed.

        // --------------------------------------------------------------------

        if (verbose) cout <<
            "\nExplicitly look for leaks with different strategies." << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tStrategy: BDEM_PASS_THROUGH" << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            const bdem_AggregateOption::AllocationStrategy S =
                                       bdem_AggregateOption::BDEM_PASS_THROUGH;

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate local test allocators." << endl;

            bslma_TestAllocator ta;

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate list." << endl;

            bdem_List list(S, &ta);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tAppend a String Array to list." << endl;

            list.appendStringArray(A16);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }
        } // default allocator da is un-installed and then destroyed.

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tStrategy: BDEM_WRITE_ONCE" << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            const bdem_AggregateOption::AllocationStrategy S =
                                        bdem_AggregateOption::BDEM_WRITE_ONCE;

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate local test allocators." << endl;

            bslma_TestAllocator ta;

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate list." << endl;

            bdem_List list(S, &ta);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tAppend a String Array to list." << endl;

            list.appendStringArray(A16);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }
        } // default allocator da is un-installed and then destroyed.

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tStrategy: BDEM_WRITE_MANY" << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);
            const bdem_AggregateOption::AllocationStrategy S =
                                        bdem_AggregateOption::BDEM_WRITE_ONCE;

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate local test allocators." << endl;

            bslma_TestAllocator ta;

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate list." << endl;

            bdem_List list(S, &ta);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tAppend a String Array to list." << endl;

            list.appendStringArray(A16);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }
        } // default allocator da is un-installed and then destroyed.

        // --------------------------------------------------------------------

        if (verbose) cout <<
           "\nExplicitly look for leaks with different element types." << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tElement Type: INT" << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            const bdem_AggregateOption::AllocationStrategy S =
                                        bdem_AggregateOption::BDEM_WRITE_ONCE;

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate local test allocators." << endl;

            bslma_TestAllocator ta;

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate list." << endl;

            bdem_List list(S, &ta);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tAppend a int to list." << endl;

            list.appendInt(A02);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }
        } // default allocator da is un-installed and then destroyed.

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tElement Type: STRING" << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            const bdem_AggregateOption::AllocationStrategy S =
                                        bdem_AggregateOption::BDEM_WRITE_ONCE;

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate local test allocators." << endl;

            bslma_TestAllocator ta;

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate list." << endl;

            bdem_List list(S, &ta);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tAppend a bsl::string to list." << endl;

            list.appendString(A06);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }
        } // default allocator da is un-installed and then destroyed.

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tElement Type: INT_ARRAY" << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            const bdem_AggregateOption::AllocationStrategy S =
                                        bdem_AggregateOption::BDEM_WRITE_ONCE;

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate local test allocators." << endl;

            bslma_TestAllocator ta;

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate list." << endl;

            bdem_List list(S, &ta);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tAppend a vector<int> to list." << endl;

            list.appendIntArray(A12);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }
        } // default allocator da is un-installed and then destroyed.

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tElement Type: STRING_ARRAY" << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            const bdem_AggregateOption::AllocationStrategy S =
                                        bdem_AggregateOption::BDEM_WRITE_ONCE;

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate local test allocators." << endl;

            bslma_TestAllocator ta;

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate list." << endl;

            bdem_List list(S, &ta);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tAppend a vector<string> to list." <<endl;

            list.appendStringArray(A16);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }
        } // default allocator da is un-installed and then destroyed.

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tElement Type: LIST { INT }" << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            const bdem_AggregateOption::AllocationStrategy S =
                                        bdem_AggregateOption::BDEM_WRITE_ONCE;

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate local test allocators." << endl;

            bslma_TestAllocator ta;

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tCreate list." << endl;

            bdem_List list(S, &ta);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -

            if (verbose) cout << "\t\tAppend a bdem_List{int} to list." <<endl;

            list.appendList(A20);

            if (veryVerbose) {
                T_ T_ T_ P_(ta.numBytesInUse()); P(ta.numBlocksInUse());
                T_ T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }
        } // default allocator da is un-installed and then destroyed.

        // --------------------------------------------------------------------

        if (verbose) cout <<
            "\nThe following used to dump core on an old Cygwin." << endl;
        {
            bslma_TestAllocator da;
            const bslma_DefaultAllocatorGuard dag(&da);

            const bdem_AggregateOption::AllocationStrategy S =
                // bdem_AggregateOption::BDEM_PASS_THROUGH; // was no leak with
                                                            // this
                bdem_AggregateOption::BDEM_WRITE_ONCE;      // this caused a
                                                            // leak
                // bdem_AggregateOption::BDEM_WRITE_MANY;   // this caused a
                                                            // leak

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tCreate local test allocators." << endl;

            bslma_TestAllocator a1;
            bslma_TestAllocator aX;

            if (veryVerbose) {
                T_ T_ P_(a1.numBytesInUse()); P(a1.numBlocksInUse());
                T_ T_ P_(aX.numBytesInUse()); P(aX.numBlocksInUse());
                T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tCreate list 1." << endl;

            bdem_List list1(S, &a1);

            if (veryVerbose) {
                T_ T_ P_(a1.numBytesInUse()); P(a1.numBlocksInUse());
                T_ T_ P_(aX.numBytesInUse()); P(aX.numBlocksInUse());
                T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tCreate list X." << endl;

            bdem_List listX(S, &aX);

            if (veryVerbose) {
                T_ T_ P_(a1.numBytesInUse()); P(a1.numBlocksInUse());
                T_ T_ P_(aX.numBytesInUse()); P(aX.numBlocksInUse());
                T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAppend a String Array to list 1." << endl;

            list1.appendStringArray(A16);

            if (veryVerbose) {
                T_ T_ P_(a1.numBytesInUse()); P(a1.numBlocksInUse());
                T_ T_ P_(aX.numBytesInUse()); P(aX.numBlocksInUse());
                T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            if (verbose) cout << "\tAppend a String Array to list X." << endl;

            if (verbose) {
                T_ Q(WE USED TO DUMP CORE ON THE NEXT LINE ON CYGWIN);
            }

            listX.appendStringArray(A16);

            if (verbose) {
                T_ Q(WE DID NOT GET HERE ON THE OLD CYGWIN);
            }

            if (veryVerbose) {
                T_ T_ P_(a1.numBytesInUse()); P(a1.numBlocksInUse());
                T_ T_ P_(aX.numBytesInUse()); P(aX.numBlocksInUse());
                T_ T_ P_(da.numBytesInUse()); P(da.numBlocksInUse());
            }

        } // default allocator da is un-installed and then destroyed.
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

// Note: on Windows, the function pointers in bdem_Properties::d_intAttr
// are 0x00000000 before entering main().  Consequently, the calls in
// fAxx() and fBxx() cause a runtime exception.  Our workaround is to
// initialize these constants at the start of main.
#ifdef BSLS_PLATFORM__CMP_MSVC
    mA20 = fA20();
    mB20 = fB20();
    mA21 = fA21();
    mB21 = fB21();
    mA30 = fA30();
    mB30 = fB30();
    mA31 = fA31();
    mB31 = fB31();
#endif

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER) \
    case NUMBER: testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose); break
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
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
