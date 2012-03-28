// bdem_list.h                                                        -*-C++-*-
#ifndef INCLUDED_BDEM_LIST
#define INCLUDED_BDEM_LIST

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a heterogeneous list of 'bdem' elements.
//
//@CLASSES:
//  bdem_List: container for a heterogeneous sequence of 'bdem' elements
//
//@SEE_ALSO: bdem_row, bdem_table, bdem_choice, bdem_choicearray, bdem_schema
//
//@AUTHOR: Pablo Halpern (phalpern)    (originally by Dan Glaser)
//
//@DESCRIPTION: This component implements a fully value-semantic container
// class, 'bdem_List', capable of holding a sequence of elements of
// heterogeneous types.  A 'bdem_List' holds exactly one (logical) row, which
// may be efficiently rendered as a 'bdem_Row' representation of its elements
// (see 'bdem_row').  The 'bdem_List' type provides a superset of the
// 'bdem_Row' interface: In addition to the element-access and streaming
// operations provided by 'bdem_Row', 'bdem_List' provides operations for
// inserting, removing, and re-arranging elements.  Value-semantic operations
// such as copy construction, assignment, and equality operations are also
// provided; in general, 'bdem_List' methods can modify the structure
// (sequence of element types) as well as the element values within the
// container.
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
// To illustrate, consider a 'BDEM_BOOL' element within a 'bdem_List'.  The
// element can be in one of three possible states:
//: o null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//: o non-null with underlying value 'false'
//: o non-null with underlying value 'true'
//
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdem_List', 'myList', whose first element is
// of type 'BDEM_STRING':
//..
//  myList.theModifiableString(0) = "HELLO";
//  assert(myList[0].isNonNull());
//..
// Making the element null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myList[0].makeNull();
//  assert(myList[0].isNull());
//  assert(myList[0].theString().empty());
//..
///List Nullability
/// - - - - - - - -
// Although a list can contain null elements (each element has "nullability
// information" associated with it), the list itself can be null only in
// relation to another object in which it is contained (i.e., its parent).  The
// nullness of an element in a list can be queried via the 'bdem_ElemRef' (or
// 'bdem_ConstElemRef') that is returned through 'operator[]' (for example,
// 'list[0].isNull()')
//
// The preferred way to assign a value to an element within a list is through
// calling the 'theModifiableTYPE' methods (e.g., 'theModifiableBool(index)')
// to first obtain a reference to the modifiable element.  The
// 'theModifiableTYPE' methods have the side-effect of making a null value
// non-null, so the 'const' 'theTYPE' methods should be used instead if the
// value is only being accessed (and *not* modified).  The 'replaceElement'
// method provides an alternate means for assigning a value to an element.  In
// particular, unlike the 'theModifiableTYPE' methods, 'replaceElement' can be
// used to assign both the null value and non-null values to an element.  The
// 'makeNull' method provides the most direct way to make a specified element
// within a list null.
//
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
//  bdem_List aList;
//  aList.appendString("BAC");
//  aList.appendDouble(17.51);
//  aList.appendDouble(17.54);
//  aList.appendDouble(17.52);
//..
// If, as is often the case, we are planning to populate the list just once,
// specifying 'bdem_AggregateOption::WRITE_ONCE' as a constructor argument
// optimizes internal memory management.
//
// We can also create the list by passing in an array of element types and then
// assigning the values, which is guaranteed to lead to an optimally-packed
// representation, ideal for repeated use:
//..
//  static bdem_ElemType::Type MSG_TYPES[] = {
//      bdem_ElemType::BDEM_STRING,
//      bdem_ElemType::BDEM_DOUBLE,
//      bdem_ElemType::BDEM_DOUBLE,
//      bdem_ElemType::BDEM_DOUBLE,
//      // ...
//  };
//
//  enum {
//      SEC_ID,
//      BID,
//      ASK,
//      LAST
//      // ...
//  };
//
//  bdem_List anotherList(MSG_TYPES, 4, bdem_AggregateOption::BDEM_WRITE_MANY);
//  anotherList[SEC_ID].theModifiableString() = "BAC";
//  anotherList[BID]   .theModifiableDouble() = 17.51;
//  anotherList[ASK]   .theModifiableDouble() = 17.54;
//  anotherList[LAST]  .theModifiableDouble() = 17.52;
//..
// Notice the use of 'bdem_AggregateOption::BDEM_WRITE_MANY' as a trailing
// constructor argument, which establishes a local memory manager for
// optimizing repeated read/write access.
//
// The next step is to create a 'bdex_ByteOutStream' in which to stream
// (externalize) the list data:
//..
//  bdex_ByteOutStream out;
//  const int VERSION = 3;
//  out.putVersion(VERSION);
//  bdex_OutStreamFunctions::streamOut(out, aList, VERSION);
//..
// We can extract the platform-neutral representation of the list data from the
// 'out' stream as a 'const char *' and an integer length as follows:
//..
//  const int length = out.length();
//  const char *data = out.data();
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
//      enum { FAILURE = -1, SUCCESS = 0 };
//..
// The first step for the client is to create, from the given externalized
// data and length, a corresponding 'bdex_ByteInStream', from which to
// re-hydrate a temporary local 'bdem_List':
//..
//      bdex_ByteInStream in(buffer, length);
//      bdem_List tmpList;
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
//      *result = tmpList;  // 'result->swap(tmpList)' is faster when valid
//      return SUCCESS;
//  }
//..
// Note that the extra copy is necessary if we don't know that the two lists
// were constructed with the same allocator object and memory management hint.
//
// The client can now access the contents of the newly-hydrated list:
//..
//  bdem_List clientList;
//  int status = unpackList(&clientList, data, length);
//  assert(!status);
//
//  assert(clientList.length() >= 4);
//  assert("BAC" == clientList[0].theString());
//  assert(17.51 == clientList[1].theDouble());
//  assert(17.54 == clientList[2].theDouble());
//  assert(17.52 == clientList[3].theDouble());
//..
// Notice that the client is expecting a list with at least (as opposed to
// exactly) four fields.  It is predominantly this feature that makes messaging
// with 'bdem_List's more robust than with hard-coded structures.  For example,
// suppose the program sending the information is upgraded to transmit an
// additional field, say an integer quantity:
//..
//  bdem_List newList;
//  newList.appendString("BAC");
//  newList.appendDouble(17.51);
//  newList.appendDouble(17.54);
//  newList.appendDouble(17.52);
//  newList.appendInt(1000);
//
//  bdex_ByteOutStream newOut;
//  newOut.putVersion(VERSION);
//  bdex_OutStreamFunctions::streamOut(newOut, newList, VERSION);
//
//  const int newLength = newOut.length();
//  const char *newData = newOut.data();
//..
// The original client program will continue to work just the same without
// rebuilding, but a new client will be able to access the additional field:
//..
//  enum { SID = 0, QNT = 4 };
//  bdex_ByteInStream newIn(newData, newLength);
//  bdem_List newClientList;
//
//  int newVersion;
//  newIn.getVersion(newVersion);
//  bdex_InStreamFunctions::streamIn(newIn, newClientList, newVersion);
//
//  assert(newClientList.length() >= 5);
//  assert("BAC" == newClientList[SID].theString());
//  assert(1000  == newClientList[QNT].theInt());
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
//  {
//      bdem_List a, b;
//      const int N = 100;
//      for (int i = 0; i < N; ++i) {
//          a.appendDouble(N + 0.5);
//          b.appendNullDouble();
//      }
//      b[25].theModifiableDouble() = 25.5;  // make just a couple of the
//      b[75].theModifiableDouble() = 75.5;  // element values in 'b' non-null
//
//      assert(100 == a.length());           // both lists hold 100 elements
//      assert(100 == b.length());
//
//      bdex_ByteOutStream aOut, bOut;       // create two separate streams
//      bdex_OutStreamFunctions::streamOut(aOut, a, VERSION);
//      bdex_OutStreamFunctions::streamOut(bOut, b, VERSION);
//
//      const int aLength = aOut.length();   // extract each stream's length
//      const int bLength = bOut.length();
//
//      bsl::cout << "aLength = " << aLength << ", bLength = " << bLength
//                << bsl::endl;
//
//      assert(bLength < aLength);  // 'bLength' is *much* less than 'aLength'
//  }
//..
// The above code will print the following on standard output:
//..
//  aLength = 917, bLength = 133
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#include <bdem_aggregateoption.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEARRAYIMP
#include <bdem_choicearrayimp.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEIMP
#include <bdem_choiceimp.h>
#endif

#ifndef INCLUDED_BDEM_ELEMATTRLOOKUP
#include <bdem_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEM_LISTIMP
#include <bdem_listimp.h>
#endif

#ifndef INCLUDED_BDEM_PROPERTIES
#include <bdem_properties.h>
#endif

#ifndef INCLUDED_BDEM_TABLEIMP
#include <bdem_tableimp.h>
#endif

#ifndef INCLUDED_BDETU_UNSET
#include <bdetu_unset.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE

#ifndef INCLUDED_BDEM_ROW
#include <bdem_row.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

#endif

namespace BloombergLP {

class bdet_Date;
class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_DateTz;
class bdet_Time;
class bdet_TimeTz;

class bdem_Choice;
class bdem_ChoiceArray;
class bdem_Row;
class bdem_Table;

                        // ===============
                        // class bdem_List
                        // ===============

class bdem_List {
    // This class implements a value-semantic, heterogeneous, indexable
    // sequence container of scalar, array, and aggregate values, managing a
    // single underlying 'bdem' row object, to which a reference may be readily
    // obtained.  A list can be configured, either by appending each element
    // (types and value) one at a time, or all at once by suppling a
    // 'bsl::vector' of 'bdem_ElemType::Type' objects and then assigning to
    // these elements their respective values, which, in some cases may lead to
    // a better packed, more space-efficient internal representation.  Note
    // however that the runtime cost of assigning array or aggregate values may
    // be prohibitive, which argues for populating such values in place.
    //
    // More generally, this class supports a complete set of *value-semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex'-compatible serialization.
    // (A precise operational definition of when two objects have the same
    // value can be found in the description of 'operator==' for the class.)
    // This class is *exception* *neutral* and, unless otherwise stated,
    // provides no guarantee of rollback: If an exception is thrown during the
    // invocation of a method on a pre-existing object, the object is left in a
    // valid state, but its value is unspecified.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.

    // DATA
    bdem_ListImp d_listImp;  // list implementation

    // FRIENDS
    friend bool operator==(const bdem_List&, const bdem_List&);

  private:
    // PRIVATE MANIPULATORS
    void *elemData(int index);
        // Return the address of the modifiable element at the specified
        // 'index' position in this list.  The nullness bit corresponding to
        // the element at 'index' is reset to 0.  The behavior is undefined
        // unless '0 <= index < length()'.

    // PRIVATE ACCESSORS
    const void *elemData(int index) const;
        // Return the address of the non-modifiable element at the specified
        // 'index' position in this list.  The nullness bit corresponding to
        // the element at 'index' is unaffected.  The behavior is undefined
        // unless '0 <= index < length()'.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdem_List,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // TYPES
    class InitialMemory {
        // DATA
        int d_i;

      public:
        // CREATORS
        explicit InitialMemory(int i) : d_i(i) { }
        ~InitialMemory() { }

        // ACCESSORS
        operator int() const { return d_i; }
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    explicit
    bdem_List(bslma_Allocator                          *basicAllocator = 0);
    explicit
    bdem_List(bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              bslma_Allocator                          *basicAllocator = 0);
    bdem_List(bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma_Allocator                          *basicAllocator = 0);
        // Create a list of length 0.  Optionally specify a memory
        // 'allocationStrategy'.  If 'allocationStrategy' is not specified,
        // then 'BDEM_PASS_THROUGH' is used.  (The meanings of the various
        // 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  If 'allocationStrategy' is specified,
        // optionally specify an 'initialMemorySize' (in bytes) that will be
        // preallocated in order to satisfy allocation requests without
        // replenishment (i.e., without internal allocation): it has no effect
        // unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialMemorySize', if 'initialMemorySize' is
        // specified.

    bdem_List(const bdem_ElemType::Type                *elementTypes,
              int                                       numElements,
              bslma_Allocator                          *basicAllocator = 0);
    bdem_List(const bdem_ElemType::Type                *elementTypes,
              int                                       numElements,
              bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              bslma_Allocator                          *basicAllocator = 0);
    bdem_List(const bdem_ElemType::Type                *elementTypes,
              int                                       numElements,
              bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma_Allocator                          *basicAllocator = 0);
        // Create a list having the specified 'numElements', whose types are
        // the same as those in the specified 'elementTypes', with each element
        // set to null and having its respective unset value (see
        // 'bdetu_unset').  Optionally specify a memory 'allocationStrategy'.
        // If 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH'
        // is used.  (The meanings of the various 'allocationStrategy' values
        // are described in 'bdem_aggregateoption'.)  If 'allocationStrategy'
        // is specified, optionally specify an 'initialMemorySize' (in bytes)
        // that will be preallocated in order to satisfy allocation requests
        // without replenishment (i.e., without internal allocation): it has no
        // effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'elementTypes' contains at least 'numElements'
        // types and '0 <= initialMemorySize', if 'initialMemorySize' is
        // specified.

    bdem_List(const bsl::vector<bdem_ElemType::Type>&   elementTypes,
              bslma_Allocator                          *basicAllocator = 0);
    bdem_List(const bsl::vector<bdem_ElemType::Type>&   elementTypes,
              bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              bslma_Allocator                          *basicAllocator = 0);
    bdem_List(const bsl::vector<bdem_ElemType::Type>&   elementTypes,
              bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma_Allocator                          *basicAllocator = 0);
        // Create a list having the sequence of element types that is the same
        // as that of the specified 'elementTypes', with each element set to
        // null and having its respective unset value (see 'bdetu_unset').
        // Optionally specify a memory 'allocationStrategy'.  If
        // 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH' is
        // used.  (The meanings of the various 'allocationStrategy' values are
        // described in 'bdem_aggregateoption'.)  If 'allocationStrategy' is
        // specified, optionally specify an 'initialMemorySize' (in bytes) that
        // will be preallocated in order to satisfy allocation requests without
        // replenishment (i.e., without internal allocation): it has no effect
        // unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialMemorySize', if 'initialMemorySize' is
        // specified.

    explicit bdem_List(const bdem_Row&                  original,
                       bslma_Allocator                 *basicAllocator = 0);
    bdem_List(const bdem_List&                          original,
              bslma_Allocator                          *basicAllocator = 0);
    bdem_List(const bdem_Row&                           original,
              bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              bslma_Allocator                          *basicAllocator = 0);
    bdem_List(const bdem_Row&                           original,
              bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma_Allocator                          *basicAllocator = 0);
    bdem_List(const bdem_List&                          original,
              bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              bslma_Allocator                          *basicAllocator = 0);
    bdem_List(const bdem_List&                          original,
              bdem_AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma_Allocator                          *basicAllocator = 0);
        // Create a list having the value of the specified 'original' object
        // (row or list).  Optionally specify a memory 'allocationStrategy'.
        // If 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH'
        // is used.  (The meanings of the various 'allocationStrategy' values
        // are described in 'bdem_aggregateoption'.)  If 'allocationStrategy'
        // is specified, optionally specify an 'initialMemorySize' (in bytes)
        // that will be preallocated in order to satisfy allocation requests
        // without replenishment (i.e., without internal allocation): it has no
        // effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialMemorySize', if 'initialMemorySize' is
        // specified.  Note that the implicit conversion of a 'bdem_Row' to a
        // 'bdem_List' is deliberately suppressed.

    ~bdem_List();
        // Destroy this list object.  If this list object was constructed with
        // any memory allocation strategy other than 'BDEM_PASS_THROUGH', then
        // destructors on individually contained elements are not invoked.  The
        // memory used by those elements will be released efficiently (all at
        // once) when the internal (managed) memory allocator is destroyed.

    // MANIPULATORS
    bdem_List& operator=(const bdem_Row& rhs);
    bdem_List& operator=(const bdem_List& rhs);
        // Assign to this list the value of the specified 'rhs' object, and
        // return a reference to this modifiable list.  Note that after the
        // assignment, both objects will have the same sequence of elements
        // (types and values).

    bdem_ElemRef operator[](int index);
        // Return a 'bdem' element reference to the modifiable element at the
        // specified 'index' position in this list.  The behavior is undefined
        // unless '0 <= index < length()'.  Note that a 'bdem_ElemRef' object
        // does not permit the type of its element to be modified.

    bdem_Row& row();
        // Return a reference to the underlying, heterogeneous row of
        // modifiable elements in this list.  Note that a 'bdem_Row' object
        // does not permit the number or types of its elements to be modified.

    bool& theModifiableBool(int index);
    char& theModifiableChar(int index);
    short& theModifiableShort(int index);
    int& theModifiableInt(int index);
    bsls_Types::Int64& theModifiableInt64(int index);
    float& theModifiableFloat(int index);
    double& theModifiableDouble(int index);
    bsl::string& theModifiableString(int index);
    bdet_Datetime& theModifiableDatetime(int index);
    bdet_DatetimeTz& theModifiableDatetimeTz(int index);
    bdet_Date& theModifiableDate(int index);
    bdet_DateTz& theModifiableDateTz(int index);
    bdet_Time& theModifiableTime(int index);
    bdet_TimeTz& theModifiableTimeTz(int index);
    bsl::vector<bool>& theModifiableBoolArray(int index);
    bsl::vector<char>& theModifiableCharArray(int index);
    bsl::vector<short>& theModifiableShortArray(int index);
    bsl::vector<int>& theModifiableIntArray(int index);
    bsl::vector<bsls_Types::Int64>& theModifiableInt64Array(int index);
    bsl::vector<float>& theModifiableFloatArray(int index);
    bsl::vector<double>& theModifiableDoubleArray(int index);
    bsl::vector<bsl::string>& theModifiableStringArray(int index);
    bsl::vector<bdet_Datetime>& theModifiableDatetimeArray(int index);
    bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray(int index);
    bsl::vector<bdet_Date>& theModifiableDateArray(int index);
    bsl::vector<bdet_DateTz>& theModifiableDateTzArray(int index);
    bsl::vector<bdet_Time>& theModifiableTimeArray(int index);
    bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray(int index);
    bdem_Choice& theModifiableChoice(int index);
    bdem_ChoiceArray& theModifiableChoiceArray(int index);
    bdem_List& theModifiableList(int index);
    bdem_Table& theModifiableTable(int index);
        // Return a reference to the modifiable 'bdem' element at the specified
        // 'index' position in this list.  If the indexed element is null, it
        // is made non-null before returning, but its value remains the
        // corresponding unset value for that type (see 'bdetu_unset').  The
        // behavior is undefined unless '0 <= index < length()' and the type of
        // the element at 'index' is of the type indicated by the name of the
        // method used.  Note that, unless there is an intention of modifying
        // the element, the corresponding 'theTYPE' method returning a
        // reference to a non-modifiable object of the same 'bdem' element type
        // should be used instead.  Also note that for a given modifiable list
        // 'mL' and a given 'bdem' 'TYPE', the expression
        // 'mL.theModifiableTYPE(index)' has the same side-effect, and returns
        // a reference to the same element, as 'mL[index].theModifiableTYPE()'
        // and 'mL.row().theModifiableTYPE(index)'.

    void appendBool(bool value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_BOOL' having the specified 'value'.

    void appendNullBool();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_BOOL'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bool' (see 'bdetu_unset').

    void appendChar(char value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_CHAR' having the specified 'value'.

    void appendNullChar();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_CHAR'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'char' (see 'bdetu_unset').

    void appendShort(short value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_SHORT' having the specified 'value'.

    void appendNullShort();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_SHORT'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'short' (see
        // 'bdetu_unset').

    void appendInt(int value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_INT' having the specified 'value'.

    void appendNullInt();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_INT'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'int' (see 'bdetu_unset').

    void appendInt64(bsls_Types::Int64 value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_INT64' having the specified 'value'.

    void appendNullInt64();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_INT64'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'Int64' (see 'bdetu_unset').

    void appendFloat(float value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_FLOAT' having the specified 'value'.

    void appendNullFloat();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_FLOAT'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'float' (see 'bdetu_unset').

    void appendDouble(double value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DOUBLE' having the specified 'value'.

    void appendNullDouble();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DOUBLE'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'double' (see
        // 'bdetu_unset').

    void appendString(const char *value);
    void appendString(const bsl::string& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_STRING' having the specified 'value'.

    void appendNullString();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_STRING'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bsl::string' (see
        // 'bdetu_unset').

    void appendDatetime(const bdet_Datetime& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DATETIME' having the specified 'value'.

    void appendNullDatetime();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DATETIME'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bdet_Datetime' (see
        // 'bdetu_unset').

    void appendDatetimeTz(const bdet_DatetimeTz& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DATETIMETZ' having the specified 'value'.

    void appendNullDatetimeTz();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DATETIMETZ'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bdet_DatetimeTz' (see
        // 'bdetu_unset').

    void appendDate(const bdet_Date& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DATE' having the specified 'value'.

    void appendNullDate();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DATE'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdet_Date' (see
        // 'bdetu_unset').

    void appendDateTz(const bdet_DateTz& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DATETZ' having the specified 'value'.

    void appendNullDateTz();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DATETZ'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bdet_DateTz' (see
        // 'bdetu_unset').

    void appendTime(const bdet_Time& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_TIME' having the specified 'value'.

    void appendNullTime();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_TIME'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdet_Time' (see
        // 'bdetu_unset').

    void appendTimeTz(const bdet_TimeTz& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_TIMETZ' having the specified 'value'.

    void appendNullTimeTz();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_TIMETZ'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bdet_TimeTz' (see
        // 'bdetu_unset').

    void appendBoolArray(const bsl::vector<bool>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_BOOL_ARRAY' having the specified 'value'.

    void appendNullBoolArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_BOOL_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendCharArray(const bsl::vector<char>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_CHAR_ARRAY' having the specified 'value'.

    void appendNullCharArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_CHAR_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendShortArray(const bsl::vector<short>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_SHORT_ARRAY' having the specified 'value'.

    void appendNullShortArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_SHORT_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendIntArray(const bsl::vector<int>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_INT_ARRAY' having the specified 'value'.

    void appendNullIntArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_INT_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendInt64Array(const bsl::vector<bsls_Types::Int64>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_INT64_ARRAY' having the specified 'value'.

    void appendNullInt64Array();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_INT64_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendFloatArray(const bsl::vector<float>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_FLOAT_ARRAY' having the specified 'value'.

    void appendNullFloatArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_FLOAT_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendDoubleArray(const bsl::vector<double>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DOUBLE_ARRAY' having the specified 'value'.

    void appendNullDoubleArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DOUBLE_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendStringArray(const bsl::vector<bsl::string>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_STRING_ARRAY' having the specified 'value'.

    void appendNullStringArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_STRING_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendDatetimeArray(const bsl::vector<bdet_Datetime>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DATETIME_ARRAY' having the specified 'value'.

    void appendNullDatetimeArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DATETIME_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendDatetimeTzArray(const bsl::vector<bdet_DatetimeTz>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DATETIMETZ_ARRAY' having the specified 'value'.

    void appendNullDatetimeTzArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DATETIMETZ_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendDateArray(const bsl::vector<bdet_Date>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DATE_ARRAY' having the specified 'value'.

    void appendNullDateArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DATE_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendDateTzArray(const bsl::vector<bdet_DateTz>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_DATETZ_ARRAY' having the specified 'value'.

    void appendNullDateTzArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_DATETZ_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendTimeArray(const bsl::vector<bdet_Time>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_TIME_ARRAY' having the specified 'value'.

    void appendNullTimeArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_TIME_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendTimeTzArray(const bsl::vector<bdet_TimeTz>& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_TIMETZ_ARRAY' having the specified 'value'.

    void appendNullTimeTzArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_TIMETZ_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendChoice(const bdem_Choice& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_CHOICE' having the specified 'value'.

    void appendNullChoice();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_CHOICE'.  Note that, if accessed, the choice
        // will have no selections and 'selector() < 0'.

    void appendChoiceArray(const bdem_ChoiceArray& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_CHOICE_ARRAY' having the specified 'value'.

    void appendNullChoiceArray();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_CHOICE_ARRAY'.  Note that, if accessed, the
        // choice array will have a size of 0 and no selections.

    void appendList(const bdem_Row& value);
    void appendList(const bdem_List& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_LIST' having the specified 'value'.  Note that
        // in the event of aliasing the *original* value of this list object
        // is assigned to the newly added list element.

    void appendNullList();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_LIST'.  Note that, if accessed, the list will
        // have a length of 0.

    void appendTable(const bdem_Table& value);
        // Append to this list a non-null element of type
        // 'bdem_ElemType::BDEM_TABLE' having the specified 'value'.

    void appendNullTable();
        // Append to this list a null element of type
        // 'bdem_ElemType::BDEM_TABLE'.  Note that, if accessed, the table will
        // have no rows and no columns.

    void appendElement(const bdem_ConstElemRef& srcElement);
        // Append to this list the value of the specified 'srcElement'.
        // Note that the behavior of this method is the same as:
        //..
        //  insertElement(length(), srcElement);
        //..

    void appendNullElement(bdem_ElemType::Type elementType);
        // Append to this list a null element of the type specified by
        // 'elementType'.  Note that, if accessed, the value will be the
        // corresponding unset value for 'elementType' (see 'bdetu_unset').
        // Also note that the behavior of this method is the same as:
        //..
        //  insertNullElement(length(), elementType);
        //..

    void appendElements(const bdem_Row& srcElements);
    void appendElements(const bdem_List& srcElements);
        // Append to this list, as individual elements, the values from the
        // specified 'srcElements'.  Note that the behavior of this method is
        // the same as:
        //..
        //  insertElements(length(), srcElements);
        //..

    void appendElements(const bdem_Row&  srcElements,
                        int              srcIndex,
                        int              numElements);
    void appendElements(const bdem_List& srcElements,
                        int              srcIndex,
                        int              numElements);
        // Append to this list, as individual elements, the specified
        // 'numElements' values from the specified 'srcElements', beginning at
        // the specified 'srcIndex'.  The behavior is undefined unless
        // '0 <= srcIndex', '0 <= numElements', and
        // 'srcIndex + numElements <= srcElements.length()'.  Note that the
        // behavior of this method is the same as:
        //..
        //  insertElements(length(), srcElements, srcIndex, numElements);
        //..

    void appendNullElements(const bdem_ElemType::Type *elementTypes,
                            int                        length);
        // Append to this list a sequence of null elements of the specified
        // 'length' having the specified 'elementTypes'.  The behavior is
        // undefined unless 'elementTypes' contains at least 'length' types.
        // Note that, if accessed, the values will be the corresponding unset
        // values for each element added (see 'bdetu_unset').  Also note that,
        // given a list 'mL', the behavior of this method is the same as:
        //..
        //  mL.insertNullElements(mL.length(), elementTypes, length);
        //..

    void appendNullElements(
                         const bsl::vector<bdem_ElemType::Type>& elementTypes);
        // Append to this list a sequence of null elements having the
        // specified 'elementTypes'.  Note that, if accessed, the values will
        // be the corresponding unset value for each element added (see
        // 'bdetu_unset').  Also note that the behavior of this method is the
        // same as:
        //..
        //  insertNullElements(length(), elementTypes);
        //..

    void insertBool(int dstIndex, bool value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_BOOL' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullBool(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_BOOL'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.   The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bool' (see 'bdetu_unset').

    void insertChar(int dstIndex, char value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_CHAR' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullChar(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_CHAR'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.   The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'char' (see 'bdetu_unset').

    void insertShort(int dstIndex, short value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_SHORT' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullShort(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_SHORT'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'short' (see 'bdetu_unset').

    void insertInt(int dstIndex, int value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_INT' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullInt(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_INT'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'int' (see 'bdetu_unset').

    void insertInt64(int dstIndex, bsls_Types::Int64 value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_INT64' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullInt64(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_INT64'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'Int64' (see 'bdetu_unset').

    void insertFloat(int dstIndex, float value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_FLOAT' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullFloat(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_FLOAT'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'float' (see 'bdetu_unset').

    void insertDouble(int dstIndex, double value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DOUBLE' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullDouble(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DOUBLE'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'double' (see 'bdetu_unset').

    void insertString(int dstIndex, const char *value);
    void insertString(int dstIndex, const bsl::string& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_STRING' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullString(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_STRING'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bsl::string' (see
        // 'bdetu_unset').

    void insertDatetime(int dstIndex, const bdet_Datetime& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DATETIME' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullDatetime(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DATETIME'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdet_Datetime' (see
        // 'bdetu_unset').

    void insertDatetimeTz(int dstIndex, const bdet_DatetimeTz& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DATETIMETZ' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDatetimeTz(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DATETIMETZ'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdet_DatetimeTz' (see
        // 'bdetu_unset').

    void insertDate(int dstIndex, const bdet_Date& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DATE' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullDate(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DATE'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdet_Date' (see
        // 'bdetu_unset').

    void insertDateTz(int dstIndex, const bdet_DateTz& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DATETZ' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullDateTz(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DATETZ'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdetDateTz' (see
        // 'bdetu_unset').

    void insertTime(int dstIndex, const bdet_Time& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_TIME' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullTime(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_TIME'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdet_Time' (see
        // 'bdetu_unset').

    void insertTimeTz(int dstIndex, const bdet_TimeTz& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_TIMETZ' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullTimeTz(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_TIMETZ'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdet_TimeTz' (see
        // 'bdetu_unset').

    void insertBoolArray(int dstIndex, const bsl::vector<bool>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_BOOL_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullBoolArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_BOOL_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertCharArray(int dstIndex, const bsl::vector<char>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_CHAR_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullCharArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_CHAR_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertShortArray(int dstIndex, const bsl::vector<short>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_SHORT_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullShortArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_SHORT_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertIntArray(int dstIndex, const bsl::vector<int>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_INT_ARRAY' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullIntArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_INT_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertInt64Array(int                                   dstIndex,
                          const bsl::vector<bsls_Types::Int64>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_INT64_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullInt64Array(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_INT64_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertFloatArray(int dstIndex, const bsl::vector<float>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_FLOAT_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullFloatArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_FLOAT_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDoubleArray(int dstIndex, const bsl::vector<double>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DOUBLE_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDoubleArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DOUBLE_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertStringArray(int                             dstIndex,
                           const bsl::vector<bsl::string>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_STRING_ARRAY' having the
        // specified 'value'.   Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullStringArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_STRING_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDatetimeArray(int                               dstIndex,
                             const bsl::vector<bdet_Datetime>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DATETIME_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDatetimeArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DATETIME_ARRAY'.  Each element having
        // an index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDatetimeTzArray(int                               dstIndex,
                             const bsl::vector<bdet_DatetimeTz>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DATETIMETZ_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDatetimeTzArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DATETIMETZ_ARRAY'.  Each element having
        // an index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDateArray(int dstIndex, const bsl::vector<bdet_Date>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DATE_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDateArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DATE_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDateTzArray(int                             dstIndex,
                           const bsl::vector<bdet_DateTz>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_DATETZ_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDateTzArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_DATETZ_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertTimeArray(int dstIndex, const bsl::vector<bdet_Time>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_TIME_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullTimeArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_TIME_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertTimeTzArray(int                             dstIndex,
                           const bsl::vector<bdet_TimeTz>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_TIMETZ_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullTimeTzArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_TIMETZ_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertChoice(int dstIndex, const bdem_Choice& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_CHOICE' having the value
        // represented by the specified 'value'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullChoice(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_CHOICE'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the choice
        // will have no selections and 'selector() < 0'.

    void insertChoiceArray(int dstIndex, const bdem_ChoiceArray& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_CHOICE_ARRAY' having the value
        // represented by the specified 'value'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullChoiceArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_CHOICE_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the choice
        // array will have a size of 0 and no selections.

    void insertList(int dstIndex, const bdem_Row& value);
    void insertList(int dstIndex, const bdem_List& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_LIST' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.  Note
        // that in the event of aliasing the *original* value of this list
        // object is assigned to the newly added list element.

    void insertNullList(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_LIST'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the list will
        // have a length of 0.

    void insertTable(int dstIndex, const bdem_Table& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'bdem_ElemType::BDEM_TABLE' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullTable(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'bdem_ElemType::BDEM_TABLE'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the table will
        // have no rows and no columns.

    void insertElement(int dstIndex, const bdem_ConstElemRef& srcElement);
        // Insert into this list, at the specified 'dstIndex', the value of the
        // specified 'srcElement'.  Each element having an index greater than
        // or equal to 'dstIndex' before the insertion is shifted up by one
        // index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullElement(int dstIndex, bdem_ElemType::Type elementType);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of the specified 'elementType'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'elementType' (see
        // 'bdetu_unset').

    void insertElements(int dstIndex, const bdem_Row& srcElements);
    void insertElements(int dstIndex, const bdem_List& srcElements);
        // Insert into this list, as individual elements beginning at the
        // specified 'dstIndex', the values of the specified 'srcElements'.
        // Each element having an index greater than or equal to 'dstIndex'
        // before the insertion is shifted up by 'srcElements.length()' index
        // positions.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertElements(int              dstIndex,
                        const bdem_Row&  srcElements,
                        int              srcIndex,
                        int              numElements);
    void insertElements(int              dstIndex,
                        const bdem_List& srcElements,
                        int              srcIndex,
                        int              numElements);
        // Insert into this list, as individual elements beginning at the
        // specified 'dstIndex', the specified 'numElements' from the
        // specified 'srcElements' beginning at the specified 'srcIndex'.
        // Each element having an index greater than or equal to 'dstIndex'
        // before the insertion is shifted up by 'numElements' index positions.
        // The behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numElements', 'dstIndex <= length()', and
        // 'srcIndex + numElements <= srcElements.length()'.

    void insertNullElements(
                         int                                     dstIndex,
                         const bsl::vector<bdem_ElemType::Type>& elementTypes);
        // Insert into this list, at the specified 'dstIndex', a sequence of
        // null elements having the specified 'elementTypes'.  Each element
        // having an index greater than or equal to 'dstIndex' before the
        // insertion is shifted up by 'elementTypes.size()' index positions.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.  Note
        // that, if accessed, each value will be the corresponding unset value
        // for the respective element type (see 'bdetu_unset').

    void insertNullElements(int                        dstIndex,
                            const bdem_ElemType::Type *elementTypes,
                            int                        length);
        // Insert into this list, at the specified 'dstIndex', a sequence of
        // null elements of the specified 'length' having the specified
        // 'elementTypes'.  Each element having an index greater than or equal
        // to 'dstIndex' before the insertion is shifted up by 'length' index
        // positions.  The behavior is undefined unless
        // '0 <= dstIndex <= length()' and 'elementTypes' contains at least
        // 'length' types.  Note that, if accessed, the values will be the
        // corresponding unset values value for each element added (see
        // 'bdetu_unset').

    void makeAllNull();
        // Set the value of each element in this list to null.  Note that, if
        // accessed, the values will be the corresponding unset values for the
        // the respective element types (see 'bdetu_unset').  Also note that
        // the behavior of this method is the same as:
        //..
        //  row().makeAllNull();
        //..

    void makeNull(int index);
        // Set the value of the element at the specified 'index' in this list
        // to null.  The behavior is undefined unless '0 <= index < length()'.
        // Note that, if accessed, the value will be the corresponding unset
        // value for its element type (see 'bdetu_unset').

    void reserveMemory(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // internal allocation).  The behavior is undefined unless
        // '0 <= numBytes'.  Note that this method has no effect unless the
        // internal allocation mode is 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY'.

    void removeElement(int index);
        // Remove from this list the element at the specified 'index'.  Each
        // element having an index greater than 'dstIndex' before the removal
        // is shifted down by one index position.  The behavior is undefined
        // unless '0 <= index < length()'.

    void removeElements(int startIndex, int numElements);
        // Remove from this list the specified 'numElements' beginning at
        // the specified 'startIndex'.  Each element having an index greater
        // than 'startIndex + numElements' before the removal is shifted down
        // by 'numElements' index positions.  The behavior is undefined unless
        // '0 <= startIndex', '0 <= numElements', and
        // 'startIndex + numElements <= length()'.

    void removeAll();
        // Remove all of the elements from this list.

    void replaceElement(int dstIndex, const bdem_ConstElemRef& srcElement);
        // Replace the type and value of the element at the specified
        // 'dstIndex' in this list with the type and value of the specified
        // 'srcElement'.  The behavior is undefined unless
        // '0 <= dstIndex < length()'.

    void resetElement(int index, bdem_ElemType::Type elemType);
        // Replace the type and value of the element at the specified 'index'
        // in this list to that of a null element having the specified
        // 'elemType'.  The behavior is undefined unless
        // '0 <= dstIndex < length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'elemType' (see 'bdetu_unset').
        // Also note that, absent aliasing, the behavior of this method is the
        // same as:
        //..
        //  removeElement(dstIndex);
        //  insertNullElement(dstIndex, elemType);
        //..

    void reset(const bdem_ElemType::Type *elementTypes, int length);
        // Replace all elements (types and values) in this list with
        // a sequence of null elements of the specified 'length' having the
        // specified 'elementTypes'.  The behavior is undefined unless
        // '0 <= length' and 'elementTypes' contains at least 'length' types.
        // Note that, if accessed, the values will be the corresponding unset
        // values for the respective element types (see 'bdetu_unset').  Also
        // note that the behavior of this method is the same as:
        //..
        //  removeAll();
        //  appendNullElements(elementTypes, length);
        //..

    void reset(const bsl::vector<bdem_ElemType::Type>& elementTypes);
        // Replace all elements (types and values) in this list with a
        // sequence of null elements having the specified 'elementTypes'.  Note
        // that, if accessed, the values will be the corresponding unset values
        // for the corresponding element types (see 'bdetu_unset').  Also
        // note that, absent aliasing, the behavior of this method is the same
        // as:
        //..
        //  removeAll();
        //  appendNullElements(elementTypes);
        //..

    void compact();
        // Reorganize the internal representation of this list to produce
        // optimal packing of its element values.  The type and value at each
        // index position is unaffected.  Note that imprudent use of this
        // method, such as repeatedly calling:
        //..
        //  a.insert( ... );
        //  a.compact();
        //..
        // may lead to unacceptably poor runtime performance.

    void swap(bdem_List& other);
        // Swap the value of this list with that of the specified 'other' list
        // in constant time.  The behavior is undefined unless 'other' has the
        // same allocator and allocation hint as this list.

    void swapElements(int index1, int index2);
        // Swap the type and value of the element at the specified 'index1'
        // position in this list with the type and value of the element at the
        // specified 'index2' position in constant time without the possibility
        // of an exception being thrown (i.e., this method provides the
        // no-throw guarantee).  The behavior is undefined unless
        // '0 <= index1 < length()' and '0 <= index2 <= length'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is not
        // specified.  If 'version' is not supported, 'stream' is marked
        // invalid and this object is unaltered.  Note that no version is read
        // from 'stream'.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // ACCESSORS
    bdem_ConstElemRef operator[](int index) const;
        // Return a 'bdem' element reference to the non-modifiable element
        // value at the specified 'index' in this list.  The behavior is
        // undefined unless '0 <= index < length()'.

    bdem_ElemType::Type elemType(int index) const;
        // Return the 'bdem' type of the element at the specified 'index'
        // position in this list.  The behavior is undefined unless
        // '0 <= index < length()'.

    void elemTypes(bsl::vector<bdem_ElemType::Type> *result) const;
        // Load, into the specified 'result', the sequence of 'bdem' types
        // corresponding to the elements in this list.

    int length() const;
        // Return the number of elements in this list.

    bool isAnyNull() const;
        // Return 'true' if the value of any element in this list is null, and
        // 'false' otherwise.  Note that an element that is null is also
        // considered to be unset.  Also note that the behavior of this method
        // is the same as:
        //..
        //  row().isAnyNull();
        //..

    bool isAnyNonNull() const;
        // Return 'true' if the value of any element in this list is not null,
        // and 'false' otherwise.  Note that the behavior of this method is
        // the same as:
        //..
        //  row().isAnyNonNull();
        //..

    const bdem_Row& row() const;
        // Return a reference to the underlying, heterogeneous row of
        // non-modifiable elements in this list.  Note that a 'bdem_Row' object
        // does not permit the number or types of its elements to be modified.

    const bool& theBool(int index) const;
    const char& theChar(int index) const;
    const short& theShort(int index) const;
    const int& theInt(int index) const;
    const bsls_Types::Int64& theInt64(int index) const;
    const float& theFloat(int index) const;
    const double& theDouble(int index) const;
    const bsl::string& theString(int index) const;
    const bdet_Datetime& theDatetime(int index) const;
    const bdet_DatetimeTz& theDatetimeTz(int index) const;
    const bdet_Date& theDate(int index) const;
    const bdet_DateTz& theDateTz(int index) const;
    const bdet_Time& theTime(int index) const;
    const bdet_TimeTz& theTimeTz(int index) const;
    const bsl::vector<bool>& theBoolArray(int index) const;
    const bsl::vector<char>& theCharArray(int index) const;
    const bsl::vector<short>& theShortArray(int index) const;
    const bsl::vector<int>& theIntArray(int index) const;
    const bsl::vector<bsls_Types::Int64>& theInt64Array(int index) const;
    const bsl::vector<float>& theFloatArray(int index) const;
    const bsl::vector<double>& theDoubleArray(int index) const;
    const bsl::vector<bsl::string>& theStringArray(int index) const;
    const bsl::vector<bdet_Datetime>& theDatetimeArray(int index) const;
    const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(int index) const;
    const bsl::vector<bdet_Date>& theDateArray(int index) const;
    const bsl::vector<bdet_DateTz>& theDateTzArray(int index) const;
    const bsl::vector<bdet_Time>& theTimeArray(int index) const;
    const bsl::vector<bdet_TimeTz>& theTimeTzArray(int index) const;
    const bdem_Choice& theChoice(int index) const;
    const bdem_ChoiceArray& theChoiceArray(int index) const;
    const bdem_List& theList(int index) const;
    const bdem_Table& theTable(int index) const;
        // Return a reference to the non-modifiable 'bdem' element at the
        // specified 'index' position in this list.  The nullness of the
        // indexed element is not affected.  The behavior is undefined unless
        // '0 <= index < length()' and the 'bdem' type of the element at
        // 'index' is one indicated by the name of the method used.  Note that
        // for a given non-modifiable list 'L' and a given 'bdem' 'TYPE', the
        // expression 'L.theTYPE(index)' returns a reference to the same
        // element as 'L[index].theTYPE()' and 'L.row().theTYPE(index)'.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // absolute value of the optionally specified indentation 'level' and
        // return a reference to 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bool operator==(const bdem_List& lhs, const bdem_List& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' list objects have the
    // same value, and 'false' otherwise.  Two list objects have the same value
    // if they have the same number of elements, and corresponding elements at
    // each index position have the same type and value.

bool operator!=(const bdem_List& lhs, const bdem_List& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' list objects do not have
    // the same value, and 'false' otherwise.  Two list objects do not have the
    // same value if they do not have the same number of elements, or there are
    // corresponding elements at some index position that do not have the same
    // type or value.

bsl::ostream& operator<<(bsl::ostream& stream, const bdem_List& list);
    // Format the value of the specified 'list', unindented on a single line,
    // to the specified output 'stream', and return a reference to 'stream'.
    // If stream is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is the same as what would result from
    // invoking:
    //..
    //  list.print(stream, 0, -1);
    //..
    // which is not fully specified, and subject to change without notice.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ---------------
                        // class bdem_List
                        // ---------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
int bdem_List::length() const
{
    return d_listImp.length();
}

                        // -------------------
                        // All Other Functions
                        // -------------------

// PRIVATE MANIPULATORS
inline
void *bdem_List::elemData(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.rowData().elemData(index);
}

// PRIVATE ACCESSORS
inline
const void *bdem_List::elemData(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.rowData().elemData(index);
}

// CLASS METHODS
inline
int bdem_List::maxSupportedBdexVersion()
{
    return 3;
}

// CREATORS
inline
bdem_List::bdem_List(bslma_Allocator *basicAllocator)
: d_listImp(bdem_AggregateOption::BDEM_PASS_THROUGH, basicAllocator)
{
}

inline
bdem_List::bdem_List(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(allocationStrategy, basicAllocator)
{
}

inline
bdem_List::bdem_List(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(allocationStrategy,
            bdem_ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= initialMemorySize);
}

inline
bdem_List::bdem_List(const bdem_ElemType::Type *elementTypes,
                     int                        numElements,
                     bslma_Allocator           *basicAllocator)
: d_listImp(elementTypes,
            numElements,
            bdem_ElemAttrLookup::lookupTable(),
            bdem_AggregateOption::BDEM_PASS_THROUGH,
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numElements);
}

inline
bdem_List::bdem_List(
                  const bdem_ElemType::Type                *elementTypes,
                  int                                       numElements,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(elementTypes,
            numElements,
            bdem_ElemAttrLookup::lookupTable(),
            allocationStrategy,
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numElements);
}

inline
bdem_List::bdem_List(
                  const bdem_ElemType::Type                *elementTypes,
                  int                                       numElements,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(elementTypes,
            numElements,
            bdem_ElemAttrLookup::lookupTable(),
            allocationStrategy,
            bdem_ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(0 <= initialMemorySize);
}

inline
bdem_List::bdem_List(const bdem_Row& original, bslma_Allocator *basicAllocator)
: d_listImp(reinterpret_cast<const bdem_RowData&>(original),
            bdem_AggregateOption::BDEM_PASS_THROUGH,
            basicAllocator)
{
}

inline
bdem_List::bdem_List(const bdem_List&  original,
                     bslma_Allocator  *basicAllocator)
: d_listImp(reinterpret_cast<const bdem_ListImp&>(original),
            bdem_AggregateOption::BDEM_PASS_THROUGH,
            basicAllocator)
{
}

inline
bdem_List::bdem_List(
                  const bdem_Row&                           original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(reinterpret_cast<const bdem_RowData&>(original),
            allocationStrategy,
            basicAllocator)
{
}

inline
bdem_List::bdem_List(
                  const bdem_Row&                           original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(reinterpret_cast<const bdem_RowData&>(original),
            allocationStrategy,
            bdem_ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= initialMemorySize);
}

inline
bdem_List::bdem_List(
                  const bdem_List&                          original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(reinterpret_cast<const bdem_ListImp&>(original),
            allocationStrategy,
            basicAllocator)
{
}

inline
bdem_List::bdem_List(
                  const bdem_List&                          original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(reinterpret_cast<const bdem_ListImp&>(original),
            allocationStrategy,
            bdem_ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= initialMemorySize);
}

inline
bdem_List::~bdem_List()
{
    // All destructor actions are in bdem_ListImp::~bdem_ListImp().
}

// MANIPULATORS
inline
bdem_List& bdem_List::operator=(const bdem_Row& rhs)
{
    d_listImp = reinterpret_cast<const bdem_RowData&>(rhs);
    return *this;
}

inline
bdem_List& bdem_List::operator=(const bdem_List& rhs)
{
    d_listImp = rhs.d_listImp;
    return *this;
}

inline
bdem_ElemRef bdem_List::operator[](int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.rowData().elemRef(index);
}

inline
bdem_Row& bdem_List::row()
{
    return reinterpret_cast<bdem_Row&>(d_listImp.rowData());
}

inline
bool& bdem_List::theModifiableBool(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bool *>(elemData(index));
}

inline
char& bdem_List::theModifiableChar(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<char *>(elemData(index));
}

inline
short& bdem_List::theModifiableShort(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<short *>(elemData(index));
}

inline
int& bdem_List::theModifiableInt(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<int *>(elemData(index));
}

inline
bsls_Types::Int64& bdem_List::theModifiableInt64(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsls_Types::Int64 *>(elemData(index));
}

inline
float& bdem_List::theModifiableFloat(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<float *>(elemData(index));
}

inline
double& bdem_List::theModifiableDouble(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<double *>(elemData(index));
}

inline
bsl::string& bdem_List::theModifiableString(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::string *>(elemData(index));
}

inline
bdet_Datetime& bdem_List::theModifiableDatetime(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdet_Datetime *>(elemData(index));
}

inline
bdet_DatetimeTz& bdem_List::theModifiableDatetimeTz(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdet_DatetimeTz *>(elemData(index));
}

inline
bdet_Date& bdem_List::theModifiableDate(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdet_Date *>(elemData(index));
}

inline
bdet_DateTz& bdem_List::theModifiableDateTz(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdet_DateTz *>(elemData(index));
}

inline
bdet_Time& bdem_List::theModifiableTime(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdet_Time *>(elemData(index));
}

inline
bdet_TimeTz& bdem_List::theModifiableTimeTz(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdet_TimeTz *>(elemData(index));
}

inline
bsl::vector<bool>& bdem_List::theModifiableBoolArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bool> *>(elemData(index));
}

inline
bsl::vector<char>& bdem_List::theModifiableCharArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<char> *>(elemData(index));
}

inline
bsl::vector<short>& bdem_List::theModifiableShortArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<short> *>(elemData(index));
}

inline
bsl::vector<int>& bdem_List::theModifiableIntArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<int> *>(elemData(index));
}

inline
bsl::vector<bsls_Types::Int64>&
bdem_List::theModifiableInt64Array(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bsls_Types::Int64> *>(elemData(index));
}

inline
bsl::vector<float>& bdem_List::theModifiableFloatArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<float> *>(elemData(index));
}

inline
bsl::vector<double>& bdem_List::theModifiableDoubleArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<double> *>(elemData(index));
}

inline
bsl::vector<bsl::string>& bdem_List::theModifiableStringArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bsl::string> *>(elemData(index));
}

inline
bsl::vector<bdet_Datetime>& bdem_List::theModifiableDatetimeArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdet_Datetime> *>(elemData(index));
}

inline
bsl::vector<bdet_DatetimeTz>&
bdem_List::theModifiableDatetimeTzArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdet_DatetimeTz> *>(elemData(index));
}

inline
bsl::vector<bdet_Date>& bdem_List::theModifiableDateArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdet_Date> *>(elemData(index));
}

inline
bsl::vector<bdet_DateTz>& bdem_List::theModifiableDateTzArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdet_DateTz> *>(elemData(index));
}

inline
bsl::vector<bdet_Time>& bdem_List::theModifiableTimeArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdet_Time> *>(elemData(index));
}

inline
bsl::vector<bdet_TimeTz>& bdem_List::theModifiableTimeTzArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdet_TimeTz> *>(elemData(index));
}

inline
bdem_Choice& bdem_List::theModifiableChoice(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdem_Choice *>(elemData(index));
}

inline
bdem_ChoiceArray& bdem_List::theModifiableChoiceArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdem_ChoiceArray *>(elemData(index));
}

inline
bdem_List& bdem_List::theModifiableList(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdem_List *>(elemData(index));
}

inline
bdem_Table& bdem_List::theModifiableTable(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdem_Table *>(elemData(index));
}

inline
void bdem_List::appendNullBool()
{
    insertNullBool(length());
}

inline
void bdem_List::appendBool(bool value)
{
    insertBool(length(), value);
}

inline
void bdem_List::appendNullChar()
{
    insertNullChar(length());
}

inline
void bdem_List::appendChar(char value)
{
    insertChar(length(), value);
}

inline
void bdem_List::appendNullShort()
{
    insertNullShort(length());
}

inline
void bdem_List::appendShort(short value)
{
    insertShort(length(), value);
}

inline
void bdem_List::appendNullInt()
{
    insertNullInt(length());
}

inline
void bdem_List::appendInt(int value)
{
    insertInt(length(), value);
}

inline
void bdem_List::appendNullInt64()
{
    insertNullInt64(length());
}

inline
void bdem_List::appendInt64(bsls_Types::Int64 value)
{
    insertInt64(length(), value);
}

inline
void bdem_List::appendNullFloat()
{
    insertNullFloat(length());
}

inline
void bdem_List::appendFloat(float value)
{
    insertFloat(length(), value);
}

inline
void bdem_List::appendNullDouble()
{
    insertNullDouble(length());
}

inline
void bdem_List::appendDouble(double value)
{
    insertDouble(length(), value);
}

inline
void bdem_List::appendNullString()
{
    insertNullString(length());
}

inline
void bdem_List::appendString(const char *value)
{
    insertString(length(), value);
}

inline
void bdem_List::appendString(const bsl::string& value)
{
    insertString(length(), value);
}

inline
void bdem_List::appendNullDatetime()
{
    insertNullDatetime(length());
}

inline
void bdem_List::appendDatetime(const bdet_Datetime& value)
{
    insertDatetime(length(), value);
}

inline
void bdem_List::appendNullDatetimeTz()
{
    insertNullDatetimeTz(length());
}

inline
void bdem_List::appendDatetimeTz(const bdet_DatetimeTz& value)
{
    insertDatetimeTz(length(), value);
}

inline
void bdem_List::appendNullDate()
{
    insertNullDate(length());
}

inline
void bdem_List::appendDate(const bdet_Date& value)
{
    insertDate(length(), value);
}

inline
void bdem_List::appendNullDateTz()
{
    insertNullDateTz(length());
}

inline
void bdem_List::appendDateTz(const bdet_DateTz& value)
{
    insertDateTz(length(), value);
}

inline
void bdem_List::appendNullTime()
{
    insertNullTime(length());
}

inline
void bdem_List::appendTime(const bdet_Time& value)
{
    insertTime(length(), value);
}

inline
void bdem_List::appendNullTimeTz()
{
    insertNullTimeTz(length());
}

inline
void bdem_List::appendTimeTz(const bdet_TimeTz& value)
{
    insertTimeTz(length(), value);
}

inline
void bdem_List::appendNullBoolArray()
{
    insertNullBoolArray(length());
}

inline
void bdem_List::appendBoolArray(const bsl::vector<bool>& value)
{
    insertBoolArray(length(), value);
}

inline
void bdem_List::appendNullCharArray()
{
    insertNullCharArray(length());
}

inline
void bdem_List::appendCharArray(const bsl::vector<char>& value)
{
    insertCharArray(length(), value);
}

inline
void bdem_List::appendNullShortArray()
{
    insertNullShortArray(length());
}

inline
void bdem_List::appendShortArray(const bsl::vector<short>& value)
{
    insertShortArray(length(), value);
}

inline
void bdem_List::appendNullIntArray()
{
    insertNullIntArray(length());
}

inline
void bdem_List::appendIntArray(const bsl::vector<int>& value)
{
    insertIntArray(length(), value);
}

inline
void bdem_List::appendNullInt64Array()
{
    insertNullInt64Array(length());
}

inline
void
bdem_List::appendInt64Array(const bsl::vector<bsls_Types::Int64>& value)
{
    insertInt64Array(length(), value);
}

inline
void bdem_List::appendNullFloatArray()
{
    insertNullFloatArray(length());
}

inline
void bdem_List::appendFloatArray(const bsl::vector<float>& value)
{
    insertFloatArray(length(), value);
}

inline
void bdem_List::appendNullDoubleArray()
{
    insertNullDoubleArray(length());
}

inline
void bdem_List::appendDoubleArray(const bsl::vector<double>& value)
{
    insertDoubleArray(length(), value);
}

inline
void bdem_List::appendNullStringArray()
{
    insertNullStringArray(length());
}

inline
void bdem_List::appendStringArray(const bsl::vector<bsl::string>& value)
{
    insertStringArray(length(), value);
}

inline
void bdem_List::appendNullDatetimeArray()
{
    insertNullDatetimeArray(length());
}

inline
void bdem_List::appendDatetimeArray(const bsl::vector<bdet_Datetime>& value)
{
    insertDatetimeArray(length(), value);
}

inline
void bdem_List::appendNullDatetimeTzArray()
{
    insertNullDatetimeTzArray(length());
}

inline
void bdem_List::appendDatetimeTzArray(
                                     const bsl::vector<bdet_DatetimeTz>& value)
{
    insertDatetimeTzArray(length(), value);
}

inline
void bdem_List::appendNullDateArray()
{
    insertNullDateArray(length());
}

inline
void bdem_List::appendDateArray(const bsl::vector<bdet_Date>& value)
{
    insertDateArray(length(), value);
}

inline
void bdem_List::appendNullDateTzArray()
{
    insertNullDateTzArray(length());
}

inline
void bdem_List::appendDateTzArray(const bsl::vector<bdet_DateTz>& value)
{
    insertDateTzArray(length(), value);
}

inline
void bdem_List::appendNullTimeArray()
{
    insertNullTimeArray(length());
}

inline
void bdem_List::appendTimeArray(const bsl::vector<bdet_Time>& value)
{
    insertTimeArray(length(), value);
}

inline
void bdem_List::appendNullTimeTzArray()
{
    insertNullTimeTzArray(length());
}

inline
void bdem_List::appendTimeTzArray(const bsl::vector<bdet_TimeTz>& value)
{
    insertTimeTzArray(length(), value);
}

inline
void bdem_List::appendNullChoice()
{
    insertNullChoice(length());
}

inline
void bdem_List::appendChoice(const bdem_Choice& value)
{
    insertChoice(length(), value);
}

inline
void bdem_List::appendNullChoiceArray()
{
    insertNullChoiceArray(length());
}

inline
void bdem_List::appendChoiceArray(const bdem_ChoiceArray& value)
{
    insertChoiceArray(length(), value);
}

inline
void bdem_List::appendNullList()
{
    insertNullList(length());
}

inline
void bdem_List::appendList(const bdem_List& value)
{
    insertList(length(), value);
}

inline
void bdem_List::appendList(const bdem_Row& value)
{
    insertList(length(), value);
}

inline
void bdem_List::appendNullTable()
{
    insertNullTable(length());
}

inline
void bdem_List::appendTable(const bdem_Table& value)
{
    insertTable(length(), value);
}

inline
void bdem_List::appendNullElement(bdem_ElemType::Type elementType)
{
    insertNullElement(length(), elementType);
}

inline
void bdem_List::appendNullElements(const bdem_ElemType::Type *elementTypes,
                                   int                        length)
{
    BSLS_ASSERT_SAFE(0 <= length);

    insertNullElements(this->length(), elementTypes, length);
}

inline
void bdem_List::appendNullElements(
                          const bsl::vector<bdem_ElemType::Type>& elementTypes)
{
    insertNullElements(length(), elementTypes);
}

inline
void bdem_List::appendElement(const bdem_ConstElemRef& srcElement)
{
    insertElement(length(), srcElement);
}

inline
void bdem_List::appendElements(const bdem_Row& srcElements)
{
    insertElements(length(), srcElements);
}

inline
void bdem_List::appendElements(const bdem_List& srcElements)
{
    insertElements(length(), srcElements);
}

inline
void bdem_List::appendElements(const bdem_Row& srcElements,
                               int             srcIndex,
                               int             numElements)
{
    BSLS_ASSERT_SAFE(0 <= srcIndex);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(srcIndex + numElements <= srcElements.length());

    insertElements(length(), srcElements, srcIndex, numElements);
}

inline
void bdem_List::appendElements(const bdem_List& srcElements,
                               int              srcIndex,
                               int              numElements)
{
    BSLS_ASSERT_SAFE(0 <= srcIndex);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(srcIndex + numElements <= srcElements.length());

    insertElements(length(), srcElements, srcIndex, numElements);
}

inline
void bdem_List::insertNullBool(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_boolAttr);
}

inline
void bdem_List::insertBool(int dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_boolAttr);
}

inline
void bdem_List::insertNullChar(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_charAttr);
}

inline
void bdem_List::insertChar(int dstIndex, char value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_charAttr);
}

inline
void bdem_List::insertNullShort(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_shortAttr);
}

inline
void bdem_List::insertShort(int dstIndex, short value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_shortAttr);
}

inline
void bdem_List::insertNullInt(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_intAttr);
}

inline
void bdem_List::insertInt(int dstIndex, int value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_intAttr);
}

inline
void bdem_List::insertNullInt64(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_int64Attr);
}

inline
void bdem_List::insertInt64(int dstIndex, bsls_Types::Int64 value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_int64Attr);
}

inline
void bdem_List::insertNullFloat(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_floatAttr);
}

inline
void bdem_List::insertFloat(int dstIndex, float value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_floatAttr);
}

inline
void bdem_List::insertNullDouble(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_doubleAttr);
}

inline
void bdem_List::insertDouble(int dstIndex, double value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_doubleAttr);
}

inline
void bdem_List::insertNullString(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_stringAttr);
}

inline
void bdem_List::insertString(int dstIndex, const char *value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertString(dstIndex, value);
}

inline
void bdem_List::insertString(int dstIndex, const bsl::string& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, &value, &bdem_Properties::d_stringAttr);
}

inline
void bdem_List::insertNullDatetime(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_datetimeAttr);
}

inline
void bdem_List::insertDatetime(int dstIndex, const bdet_Datetime& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_datetimeAttr);
}

inline
void bdem_List::insertNullDatetimeTz(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_datetimeTzAttr);
}

inline
void bdem_List::insertDatetimeTz(int dstIndex, const bdet_DatetimeTz& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_datetimeTzAttr);
}

inline
void bdem_List::insertNullDate(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_dateAttr);
}

inline
void bdem_List::insertDate(int dstIndex, const bdet_Date& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_dateAttr);
}

inline
void bdem_List::insertNullDateTz(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_dateTzAttr);
}

inline
void bdem_List::insertDateTz(int dstIndex, const bdet_DateTz& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_dateTzAttr);
}

inline
void bdem_List::insertNullTime(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_timeAttr);
}

inline
void bdem_List::insertTime(int dstIndex, const bdet_Time& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_timeAttr);
}

inline
void bdem_List::insertNullTimeTz(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_timeTzAttr);
}

inline
void bdem_List::insertTimeTz(int dstIndex, const bdet_TimeTz& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &bdem_Properties::d_timeTzAttr);
}

inline
void bdem_List::insertNullBoolArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_boolArrayAttr);
}

inline
void bdem_List::insertBoolArray(int dstIndex, const bsl::vector<bool>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_boolArrayAttr);
}

inline
void bdem_List::insertNullCharArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_charArrayAttr);
}

inline
void bdem_List::insertCharArray(int dstIndex, const bsl::vector<char>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_charArrayAttr);
}

inline
void bdem_List::insertNullShortArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_shortArrayAttr);
}

inline
void bdem_List::insertShortArray(int dstIndex, const bsl::vector<short>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_shortArrayAttr);
}

inline
void bdem_List::insertNullIntArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_intArrayAttr);
}

inline
void bdem_List::insertIntArray(int dstIndex, const bsl::vector<int>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_intArrayAttr);
}

inline
void bdem_List::insertNullInt64Array(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &bdem_Properties::d_int64ArrayAttr);
}

inline
void
bdem_List::insertInt64Array(int                                   dstIndex,
                            const bsl::vector<bsls_Types::Int64>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_int64ArrayAttr);
}

inline
void bdem_List::insertNullFloatArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &bdem_Properties::d_floatArrayAttr);
}

inline
void bdem_List::insertFloatArray(int                       dstIndex,
                                 const bsl::vector<float>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_floatArrayAttr);
}

inline
void bdem_List::insertNullDoubleArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &bdem_Properties::d_doubleArrayAttr);
}

inline
void bdem_List::insertDoubleArray(int                        dstIndex,
                                  const bsl::vector<double>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_doubleArrayAttr);
}

inline
void bdem_List::insertNullStringArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &bdem_Properties::d_stringArrayAttr);
}

inline
void bdem_List::insertStringArray(int                             dstIndex,
                                  const bsl::vector<bsl::string>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_stringArrayAttr);
}

inline
void bdem_List::insertNullDatetimeArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &bdem_Properties::d_datetimeArrayAttr);
}

inline
void bdem_List::insertDatetimeArray(int                               dstIndex,
                                    const bsl::vector<bdet_Datetime>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_datetimeArrayAttr);
}

inline
void bdem_List::insertNullDatetimeTzArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &bdem_Properties::d_datetimeTzArrayAttr);
}

inline
void bdem_List::insertDatetimeTzArray(
                                 int                                 dstIndex,
                                 const bsl::vector<bdet_DatetimeTz>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_datetimeTzArrayAttr);
}

inline
void bdem_List::insertNullDateArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_dateArrayAttr);
}

inline
void bdem_List::insertDateArray(int                           dstIndex,
                                const bsl::vector<bdet_Date>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_dateArrayAttr);
}

inline
void bdem_List::insertNullDateTzArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &bdem_Properties::d_dateTzArrayAttr);
}

inline
void bdem_List::insertDateTzArray(int                           dstIndex,
                                const bsl::vector<bdet_DateTz>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_dateTzArrayAttr);
}

inline
void bdem_List::insertNullTimeArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_timeArrayAttr);
}

inline
void bdem_List::insertTimeArray(int                           dstIndex,
                                const bsl::vector<bdet_Time>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_timeArrayAttr);
}

inline
void bdem_List::insertNullTimeTzArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_Properties::d_timeTzArrayAttr);
}

inline
void bdem_List::insertTimeTzArray(int                             dstIndex,
                                  const bsl::vector<bdet_TimeTz>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_Properties::d_timeTzArrayAttr);
}

inline
void bdem_List::insertNullChoice(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_ChoiceImp::d_choiceAttr);
}

inline
void bdem_List::insertChoice(int dstIndex, const bdem_Choice& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, &value, &bdem_ChoiceImp::d_choiceAttr);
}

inline
void bdem_List::insertNullChoiceArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &bdem_ChoiceArrayImp::d_choiceArrayAttr);
}

inline
void bdem_List::insertChoiceArray(int                     dstIndex,
                                  const bdem_ChoiceArray& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &bdem_ChoiceArrayImp::d_choiceArrayAttr);
}

inline
void bdem_List::insertNullList(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_ListImp::d_listAttr);
}

inline
void bdem_List::insertList(int dstIndex, const bdem_List& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, &value, &bdem_ListImp::d_listAttr);
}

inline
void bdem_List::insertList(int dstIndex, const bdem_Row& srcRow)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertList(dstIndex,
                         reinterpret_cast<const bdem_RowData&>(srcRow));
}

inline
void bdem_List::insertNullTable(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &bdem_TableImp::d_tableAttr);
}

inline
void bdem_List::insertTable(int dstIndex, const bdem_Table& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, &value, &bdem_TableImp::d_tableAttr);
}

inline
void bdem_List::insertNullElement(int                 dstIndex,
                                  bdem_ElemType::Type elementType)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(
                              dstIndex,
                              bdem_ElemAttrLookup::lookupTable()[elementType]);
}

inline
void bdem_List::insertNullElements(int                        dstIndex,
                                   const bdem_ElemType::Type *elementTypes,
                                   int                        length)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= this->length());
    BSLS_ASSERT_SAFE(0 <= length);

    d_listImp.insertNullElements(dstIndex,
                                 elementTypes,
                                 length,
                                 bdem_ElemAttrLookup::lookupTable());
}

inline
void bdem_List::insertElement(int                      dstIndex,
                              const bdem_ConstElemRef& srcElement)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, srcElement);
}

inline
void bdem_List::insertElements(int dstIndex, const bdem_Row& srcRow)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    const bdem_RowData& rowData =
                                 reinterpret_cast<const bdem_RowData&>(srcRow);
    insertElements(dstIndex, srcRow, 0, rowData.length());
}

inline
void bdem_List::insertElements(int dstIndex, const bdem_List& srcList)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    const bdem_RowData& rowData =
                          reinterpret_cast<const bdem_RowData&>(srcList.row());
    insertElements(dstIndex, srcList.row(), 0, rowData.length());
}

inline
void bdem_List::insertElements(int              dstIndex,
                               const bdem_Row&  srcElements,
                               int              srcIndex,
                               int              numElements)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());
    BSLS_ASSERT_SAFE(0 <= srcIndex);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(srcIndex + numElements <= srcElements.length());

    if (numElements) {
        d_listImp.insertElements(
                            dstIndex,
                            reinterpret_cast<const bdem_RowData&>(srcElements),
                            srcIndex,
                            numElements);
    }
}

inline
void bdem_List::insertElements(int              dstIndex,
                               const bdem_List& srcElements,
                               int              srcIndex,
                               int              numElements)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());
    BSLS_ASSERT_SAFE(0 <= srcIndex);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(srcIndex + numElements <= srcElements.length());

    insertElements(dstIndex, srcElements.row(), srcIndex, numElements);
}

inline
void bdem_List::makeAllNull()
{
    d_listImp.rowData().makeAllNull();
}

inline
void bdem_List::makeNull(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    d_listImp.rowData().makeNull(index);
}

inline
void bdem_List::removeElement(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    d_listImp.removeElement(index);
}

inline
void bdem_List::removeElements(int startIndex, int numElements)
{
    BSLS_ASSERT_SAFE(0 <= startIndex);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(startIndex + numElements <= length());

    d_listImp.removeElements(startIndex, numElements);
}

inline
void bdem_List::removeAll()
{
    d_listImp.removeAll();
}

inline
void
bdem_List::replaceElement(int dstIndex, const bdem_ConstElemRef& srcElement)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex < length());

    d_listImp.replaceElement(dstIndex, srcElement);
}

inline
void bdem_List::reserveMemory(int numBytes)
{
    BSLS_ASSERT_SAFE(0 <= numBytes);

    d_listImp.reserveMemory(numBytes);
}

inline
void bdem_List::resetElement(int index, bdem_ElemType::Type elemType)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    d_listImp.resetElement(index,
                           bdem_ElemAttrLookup::lookupTable()[elemType]);
}

inline
void bdem_List::reset(const bdem_ElemType::Type *elementTypes, int length)
{
    BSLS_ASSERT_SAFE(0 <= length);

    d_listImp.reset(elementTypes,
                    length,
                    bdem_ElemAttrLookup::lookupTable());
}

inline
void bdem_List::compact()
{
    d_listImp.compact();
}

inline
void bdem_List::swap(bdem_List& other)
{
    d_listImp.swap(other.d_listImp);
}

inline
void bdem_List::swapElements(int index1, int index2)
{
    BSLS_ASSERT_SAFE(0 <= index1);
    BSLS_ASSERT_SAFE(     index1 < length());
    BSLS_ASSERT_SAFE(0 <= index2);
    BSLS_ASSERT_SAFE(     index2 < length());

    d_listImp.swap(index1, index2);
}

template <class STREAM>
inline
STREAM& bdem_List::bdexStreamIn(STREAM& stream, int version)
{
    d_listImp.bdexStreamInImp(
                            stream,
                            version,
                            bdem_ElemStreamInAttrLookup<STREAM>::lookupTable(),
                            bdem_ElemAttrLookup::lookupTable());
    return stream;
}

// ACCESSORS
inline
bdem_ConstElemRef bdem_List::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.rowData().elemRef(index);
}

inline
bdem_ElemType::Type bdem_List::elemType(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.elemType(index);
}

inline
bool bdem_List::isAnyNull() const
{
    return d_listImp.rowData().isAnyInRangeNull(0, d_listImp.length());
}

inline
bool bdem_List::isAnyNonNull() const
{
    return d_listImp.rowData().isAnyInRangeNonNull(0, d_listImp.length());
}

inline
const bdem_Row& bdem_List::row() const
{
    return reinterpret_cast<const bdem_Row&>(d_listImp.rowData());
}

inline
const bool& bdem_List::theBool(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bool *>(elemData(index));
}

inline
const char& bdem_List::theChar(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const char *>(elemData(index));
}

inline
const short& bdem_List::theShort(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const short *>(elemData(index));
}

inline
const int& bdem_List::theInt(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const int *>(elemData(index));
}

inline
const bsls_Types::Int64& bdem_List::theInt64(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsls_Types::Int64 *>(elemData(index));
}

inline
const float& bdem_List::theFloat(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const float *>(elemData(index));
}

inline
const double& bdem_List::theDouble(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const double *>(elemData(index));
}

inline
const bsl::string& bdem_List::theString(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::string *>(elemData(index));
}

inline
const bdet_Datetime& bdem_List::theDatetime(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdet_Datetime *>(elemData(index));
}

inline
const bdet_DatetimeTz& bdem_List::theDatetimeTz(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdet_DatetimeTz *>(elemData(index));
}

inline
const bdet_Date& bdem_List::theDate(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdet_Date *>(elemData(index));
}

inline
const bdet_DateTz& bdem_List::theDateTz(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdet_DateTz *>(elemData(index));
}

inline
const bdet_Time& bdem_List::theTime(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdet_Time *>(elemData(index));
}

inline
const bdet_TimeTz& bdem_List::theTimeTz(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdet_TimeTz *>(elemData(index));
}

inline
const bsl::vector<bool>& bdem_List::theBoolArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bool> *>(elemData(index));
}

inline
const bsl::vector<char>& bdem_List::theCharArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<char> *>(elemData(index));
}

inline
const bsl::vector<short>& bdem_List::theShortArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<short> *>(elemData(index));
}

inline
const bsl::vector<int>& bdem_List::theIntArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<int> *>(elemData(index));
}

inline
const
bsl::vector<bsls_Types::Int64>& bdem_List::theInt64Array(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bsls_Types::Int64> *>(
                                                              elemData(index));
}

inline
const bsl::vector<float>& bdem_List::theFloatArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<float> *>(elemData(index));
}

inline
const bsl::vector<double>& bdem_List::theDoubleArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<double> *>(elemData(index));
}

inline
const bsl::vector<bsl::string>& bdem_List::theStringArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bsl::string> *>(elemData(index));
}

inline
const bsl::vector<bdet_Datetime>& bdem_List::theDatetimeArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdet_Datetime> *>(elemData(index));
}

inline
const bsl::vector<bdet_DatetimeTz>&
bdem_List::theDatetimeTzArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdet_DatetimeTz> *>(elemData(index));
}

inline
const bsl::vector<bdet_Date>& bdem_List::theDateArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdet_Date> *>(elemData(index));
}

inline
const bsl::vector<bdet_DateTz>&
bdem_List::theDateTzArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdet_DateTz> *>(elemData(index));
}

inline
const bsl::vector<bdet_Time>& bdem_List::theTimeArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdet_Time> *>(elemData(index));
}

inline
const bsl::vector<bdet_TimeTz>&
bdem_List::theTimeTzArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdet_TimeTz> *>(elemData(index));
}

inline
const bdem_Choice& bdem_List::theChoice(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdem_Choice *>(elemData(index));
}

inline
const bdem_ChoiceArray& bdem_List::theChoiceArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdem_ChoiceArray *>(elemData(index));
}

inline
const bdem_List& bdem_List::theList(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdem_List *>(elemData(index));
}

inline
const bdem_Table& bdem_List::theTable(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdem_Table *>(elemData(index));
}

template <class STREAM>
inline
STREAM& bdem_List::bdexStreamOut(STREAM& stream, int version) const
{
    d_listImp.bdexStreamOutImp(
                          stream,
                          version,
                          bdem_ElemStreamOutAttrLookup<STREAM>::lookupTable());
    return stream;
}

inline
bsl::ostream& bdem_List::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    return d_listImp.print(stream, level, spacesPerLevel);
}

// FREE OPERATORS
inline
bool operator==(const bdem_List& lhs, const bdem_List& rhs)
{
    return lhs.d_listImp == rhs.d_listImp;
}

inline
bool operator!=(const bdem_List& lhs, const bdem_List& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_List& list)
{
    return list.print(stream, 0, -1);
}

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
