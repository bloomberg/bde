// bdlmxxx_list.h                                                        -*-C++-*-
#ifndef INCLUDED_BDLMXXX_LIST
#define INCLUDED_BDLMXXX_LIST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a heterogeneous list of 'bdem' elements.
//
//@CLASSES:
//  bdlmxxx::List: container for a heterogeneous sequence of 'bdem' elements
//
//@SEE_ALSO: bdlmxxx_row, bdlmxxx_table, bdlmxxx_choice, bdlmxxx_choicearray, bdlmxxx_schema
//
//@AUTHOR: Pablo Halpern (phalpern)    (originally by Dan Glaser)
//
//@DESCRIPTION: This component implements a fully value-semantic container
// class, 'bdlmxxx::List', capable of holding a sequence of elements of
// heterogeneous types.  A 'bdlmxxx::List' holds exactly one (logical) row, which
// may be efficiently rendered as a 'bdlmxxx::Row' representation of its elements
// (see 'bdlmxxx_row').  The 'bdlmxxx::List' type provides a superset of the
// 'bdlmxxx::Row' interface: In addition to the element-access and streaming
// operations provided by 'bdlmxxx::Row', 'bdlmxxx::List' provides operations for
// inserting, removing, and re-arranging elements.  Value-semantic operations
// such as copy construction, assignment, and equality operations are also
// provided; in general, 'bdlmxxx::List' methods can modify the structure
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
//  scalar (BDEM_INT, etc.)           bdltuxxx::Unset<TYPE>::unsetValue()
//
//  vector (BDEM_INT_ARRAY, etc.)     0 == size()
//..
// To illustrate, consider a 'BDEM_BOOL' element within a 'bdlmxxx::List'.  The
// element can be in one of three possible states:
//: o null with underlying value 'bdltuxxx::Unset<bool>::unsetValue()'
//: o non-null with underlying value 'false'
//: o non-null with underlying value 'true'
//
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdlmxxx::List', 'myList', whose first element is
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
// nullness of an element in a list can be queried via the 'bdlmxxx::ElemRef' (or
// 'bdlmxxx::ConstElemRef') that is returned through 'operator[]' (for example,
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
// A 'bdlmxxx::List' is ideal for representing and transmitting messages as a
// heterogeneous sequence of arbitrarily complex, self-describing data in and
// out of a process.  For example, suppose we want to send a message containing
// price-update information for a given security to an interested client in the
// form of a 'bdlmxxx::List' object.  The first step is to decide what data types
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
//  bdlmxxx::List aList;
//  aList.appendString("BAC");
//  aList.appendDouble(17.51);
//  aList.appendDouble(17.54);
//  aList.appendDouble(17.52);
//..
// If, as is often the case, we are planning to populate the list just once,
// specifying 'bdlmxxx::AggregateOption::WRITE_ONCE' as a constructor argument
// optimizes internal memory management.
//
// We can also create the list by passing in an array of element types and then
// assigning the values, which is guaranteed to lead to an optimally-packed
// representation, ideal for repeated use:
//..
//  static bdlmxxx::ElemType::Type MSG_TYPES[] = {
//      bdlmxxx::ElemType::BDEM_STRING,
//      bdlmxxx::ElemType::BDEM_DOUBLE,
//      bdlmxxx::ElemType::BDEM_DOUBLE,
//      bdlmxxx::ElemType::BDEM_DOUBLE,
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
//  bdlmxxx::List anotherList(MSG_TYPES, 4, bdlmxxx::AggregateOption::BDEM_WRITE_MANY);
//  anotherList[SEC_ID].theModifiableString() = "BAC";
//  anotherList[BID]   .theModifiableDouble() = 17.51;
//  anotherList[ASK]   .theModifiableDouble() = 17.54;
//  anotherList[LAST]  .theModifiableDouble() = 17.52;
//..
// Notice the use of 'bdlmxxx::AggregateOption::BDEM_WRITE_MANY' as a trailing
// constructor argument, which establishes a local memory manager for
// optimizing repeated read/write access.
//
// The next step is to create a 'bdlxxxx::ByteOutStream' in which to stream
// (externalize) the list data:
//..
//  bdlxxxx::ByteOutStream out;
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
//  int unpackList(bdlmxxx::List *result, const char *buffer, int length)
//      // Load into the specified 'result' the list data supplied in the
//      // specified 'buffer' of the specified 'length'.  Return 0 on success,
//      // and a non-zero value (with no effect on *result) otherwise.
//  {
//      enum { FAILURE = -1, SUCCESS = 0 };
//..
// The first step for the client is to create, from the given externalized
// data and length, a corresponding 'bdlxxxx::ByteInStream', from which to
// re-hydrate a temporary local 'bdlmxxx::List':
//..
//      bdlxxxx::ByteInStream in(buffer, length);
//      bdlmxxx::List tmpList;
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
//  bdlmxxx::List clientList;
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
// with 'bdlmxxx::List's more robust than with hard-coded structures.  For example,
// suppose the program sending the information is upgraded to transmit an
// additional field, say an integer quantity:
//..
//  bdlmxxx::List newList;
//  newList.appendString("BAC");
//  newList.appendDouble(17.51);
//  newList.appendDouble(17.54);
//  newList.appendDouble(17.52);
//  newList.appendInt(1000);
//
//  bdlxxxx::ByteOutStream newOut;
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
//  bdlxxxx::ByteInStream newIn(newData, newLength);
//  bdlmxxx::List newClientList;
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
// 'bdlmxxx_schema' component.
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
//      bdlmxxx::List a, b;
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
//      bdlxxxx::ByteOutStream aOut, bOut;       // create two separate streams
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_AGGREGATEOPTION
#include <bdlmxxx_aggregateoption.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEARRAYIMP
#include <bdlmxxx_choicearrayimp.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEIMP
#include <bdlmxxx_choiceimp.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMATTRLOOKUP
#include <bdlmxxx_elemattrlookup.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMREF
#include <bdlmxxx_elemref.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_LISTIMP
#include <bdlmxxx_listimp.h>
#endif

#ifndef INCLUDED_BDLMXXX_PROPERTIES
#include <bdlmxxx_properties.h>
#endif

#ifndef INCLUDED_BDLMXXX_TABLEIMP
#include <bdlmxxx_tableimp.h>
#endif

#ifndef INCLUDED_BDLTUXXX_UNSET
#include <bdltuxxx_unset.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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

#ifndef INCLUDED_BDLMXXX_ROW
#include <bdlmxxx_row.h>
#endif

#endif

namespace BloombergLP {



// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class Date; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Date Date;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Datetime; }                              // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Datetime Datetime;            // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DatetimeTz; }                            // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DatetimeTz DatetimeTz;        // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DateTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DateTz DateTz;                // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Time; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Time Time;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class TimeTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::TimeTz TimeTz;                // bdet -> bdlt
}  // close package namespace


namespace bdlmxxx {class Choice;
class ChoiceArray;
class Row;
class Table;

                        // ===============
                        // class List
                        // ===============

class List {
    // This class implements a value-semantic, heterogeneous, indexable
    // sequence container of scalar, array, and aggregate values, managing a
    // single underlying 'bdem' row object, to which a reference may be readily
    // obtained.  A list can be configured, either by appending each element
    // (types and value) one at a time, or all at once by suppling a
    // 'bsl::vector' of 'ElemType::Type' objects and then assigning to
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
    ListImp d_listImp;  // list implementation

    // FRIENDS
    friend bool operator==(const List&, const List&);

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
    BSLALG_DECLARE_NESTED_TRAITS(List,
                                 bslalg::TypeTraitUsesBslmaAllocator);

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
    List(bslma::Allocator                         *basicAllocator = 0);
    explicit
    List(AggregateOption::AllocationStrategy  allocationStrategy,
              bslma::Allocator                         *basicAllocator = 0);
    List(AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma::Allocator                         *basicAllocator = 0);
        // Create a list of length 0.  Optionally specify a memory
        // 'allocationStrategy'.  If 'allocationStrategy' is not specified,
        // then 'BDEM_PASS_THROUGH' is used.  (The meanings of the various
        // 'allocationStrategy' values are described in
        // 'bdlmxxx_aggregateoption'.)  If 'allocationStrategy' is specified,
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

    List(const ElemType::Type                *elementTypes,
              int                                       numElements,
              bslma::Allocator                         *basicAllocator = 0);
    List(const ElemType::Type                *elementTypes,
              int                                       numElements,
              AggregateOption::AllocationStrategy  allocationStrategy,
              bslma::Allocator                         *basicAllocator = 0);
    List(const ElemType::Type                *elementTypes,
              int                                       numElements,
              AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma::Allocator                         *basicAllocator = 0);
        // Create a list having the specified 'numElements', whose types are
        // the same as those in the specified 'elementTypes', with each element
        // set to null and having its respective unset value (see
        // 'bdltuxxx_unset').  Optionally specify a memory 'allocationStrategy'.
        // If 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH'
        // is used.  (The meanings of the various 'allocationStrategy' values
        // are described in 'bdlmxxx_aggregateoption'.)  If 'allocationStrategy'
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

    List(const bsl::vector<ElemType::Type>&   elementTypes,
              bslma::Allocator                         *basicAllocator = 0);
    List(const bsl::vector<ElemType::Type>&   elementTypes,
              AggregateOption::AllocationStrategy  allocationStrategy,
              bslma::Allocator                         *basicAllocator = 0);
    List(const bsl::vector<ElemType::Type>&   elementTypes,
              AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma::Allocator                         *basicAllocator = 0);
        // Create a list having the sequence of element types that is the same
        // as that of the specified 'elementTypes', with each element set to
        // null and having its respective unset value (see 'bdltuxxx_unset').
        // Optionally specify a memory 'allocationStrategy'.  If
        // 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH' is
        // used.  (The meanings of the various 'allocationStrategy' values are
        // described in 'bdlmxxx_aggregateoption'.)  If 'allocationStrategy' is
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

    explicit List(const Row&                  original,
                       bslma::Allocator                *basicAllocator = 0);
    List(const List&                          original,
              bslma::Allocator                         *basicAllocator = 0);
    List(const Row&                           original,
              AggregateOption::AllocationStrategy  allocationStrategy,
              bslma::Allocator                         *basicAllocator = 0);
    List(const Row&                           original,
              AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma::Allocator                         *basicAllocator = 0);
    List(const List&                          original,
              AggregateOption::AllocationStrategy  allocationStrategy,
              bslma::Allocator                         *basicAllocator = 0);
    List(const List&                          original,
              AggregateOption::AllocationStrategy  allocationStrategy,
              const InitialMemory&                      initialMemorySize,
              bslma::Allocator                         *basicAllocator = 0);
        // Create a list having the value of the specified 'original' object
        // (row or list).  Optionally specify a memory 'allocationStrategy'.
        // If 'allocationStrategy' is not specified, then 'BDEM_PASS_THROUGH'
        // is used.  (The meanings of the various 'allocationStrategy' values
        // are described in 'bdlmxxx_aggregateoption'.)  If 'allocationStrategy'
        // is specified, optionally specify an 'initialMemorySize' (in bytes)
        // that will be preallocated in order to satisfy allocation requests
        // without replenishment (i.e., without internal allocation): it has no
        // effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialMemorySize', if 'initialMemorySize' is
        // specified.  Note that the implicit conversion of a 'Row' to a
        // 'List' is deliberately suppressed.

    ~List();
        // Destroy this list object.  If this list object was constructed with
        // any memory allocation strategy other than 'BDEM_PASS_THROUGH', then
        // destructors on individually contained elements are not invoked.  The
        // memory used by those elements will be released efficiently (all at
        // once) when the internal (managed) memory allocator is destroyed.

    // MANIPULATORS
    List& operator=(const Row& rhs);
    List& operator=(const List& rhs);
        // Assign to this list the value of the specified 'rhs' object, and
        // return a reference to this modifiable list.  Note that after the
        // assignment, both objects will have the same sequence of elements
        // (types and values).

    ElemRef operator[](int index);
        // Return a 'bdem' element reference to the modifiable element at the
        // specified 'index' position in this list.  The behavior is undefined
        // unless '0 <= index < length()'.  Note that a 'ElemRef' object
        // does not permit the type of its element to be modified.

    Row& row();
        // Return a reference to the underlying, heterogeneous row of
        // modifiable elements in this list.  Note that a 'Row' object
        // does not permit the number or types of its elements to be modified.

    bool& theModifiableBool(int index);
    char& theModifiableChar(int index);
    short& theModifiableShort(int index);
    int& theModifiableInt(int index);
    bsls::Types::Int64& theModifiableInt64(int index);
    float& theModifiableFloat(int index);
    double& theModifiableDouble(int index);
    bsl::string& theModifiableString(int index);
    bdlt::Datetime& theModifiableDatetime(int index);
    bdlt::DatetimeTz& theModifiableDatetimeTz(int index);
    bdlt::Date& theModifiableDate(int index);
    bdlt::DateTz& theModifiableDateTz(int index);
    bdlt::Time& theModifiableTime(int index);
    bdlt::TimeTz& theModifiableTimeTz(int index);
    bsl::vector<bool>& theModifiableBoolArray(int index);
    bsl::vector<char>& theModifiableCharArray(int index);
    bsl::vector<short>& theModifiableShortArray(int index);
    bsl::vector<int>& theModifiableIntArray(int index);
    bsl::vector<bsls::Types::Int64>& theModifiableInt64Array(int index);
    bsl::vector<float>& theModifiableFloatArray(int index);
    bsl::vector<double>& theModifiableDoubleArray(int index);
    bsl::vector<bsl::string>& theModifiableStringArray(int index);
    bsl::vector<bdlt::Datetime>& theModifiableDatetimeArray(int index);
    bsl::vector<bdlt::DatetimeTz>& theModifiableDatetimeTzArray(int index);
    bsl::vector<bdlt::Date>& theModifiableDateArray(int index);
    bsl::vector<bdlt::DateTz>& theModifiableDateTzArray(int index);
    bsl::vector<bdlt::Time>& theModifiableTimeArray(int index);
    bsl::vector<bdlt::TimeTz>& theModifiableTimeTzArray(int index);
    Choice& theModifiableChoice(int index);
    ChoiceArray& theModifiableChoiceArray(int index);
    List& theModifiableList(int index);
    Table& theModifiableTable(int index);
        // Return a reference to the modifiable 'bdem' element at the specified
        // 'index' position in this list.  If the indexed element is null, it
        // is made non-null before returning, but its value remains the
        // corresponding unset value for that type (see 'bdltuxxx_unset').  The
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
        // 'ElemType::BDEM_BOOL' having the specified 'value'.

    void appendNullBool();
        // Append to this list a null element of type
        // 'ElemType::BDEM_BOOL'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bool' (see 'bdltuxxx_unset').

    void appendChar(char value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_CHAR' having the specified 'value'.

    void appendNullChar();
        // Append to this list a null element of type
        // 'ElemType::BDEM_CHAR'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'char' (see 'bdltuxxx_unset').

    void appendShort(short value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_SHORT' having the specified 'value'.

    void appendNullShort();
        // Append to this list a null element of type
        // 'ElemType::BDEM_SHORT'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'short' (see
        // 'bdltuxxx_unset').

    void appendInt(int value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_INT' having the specified 'value'.

    void appendNullInt();
        // Append to this list a null element of type
        // 'ElemType::BDEM_INT'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'int' (see 'bdltuxxx_unset').

    void appendInt64(bsls::Types::Int64 value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_INT64' having the specified 'value'.

    void appendNullInt64();
        // Append to this list a null element of type
        // 'ElemType::BDEM_INT64'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'Int64' (see 'bdltuxxx_unset').

    void appendFloat(float value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_FLOAT' having the specified 'value'.

    void appendNullFloat();
        // Append to this list a null element of type
        // 'ElemType::BDEM_FLOAT'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'float' (see 'bdltuxxx_unset').

    void appendDouble(double value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DOUBLE' having the specified 'value'.

    void appendNullDouble();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DOUBLE'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'double' (see
        // 'bdltuxxx_unset').

    void appendString(const char *value);
    void appendString(const bsl::string& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_STRING' having the specified 'value'.

    void appendNullString();
        // Append to this list a null element of type
        // 'ElemType::BDEM_STRING'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bsl::string' (see
        // 'bdltuxxx_unset').

    void appendDatetime(const bdlt::Datetime& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DATETIME' having the specified 'value'.

    void appendNullDatetime();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DATETIME'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bdlt::Datetime' (see
        // 'bdltuxxx_unset').

    void appendDatetimeTz(const bdlt::DatetimeTz& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DATETIMETZ' having the specified 'value'.

    void appendNullDatetimeTz();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DATETIMETZ'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bdlt::DatetimeTz' (see
        // 'bdltuxxx_unset').

    void appendDate(const bdlt::Date& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DATE' having the specified 'value'.

    void appendNullDate();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DATE'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdlt::Date' (see
        // 'bdltuxxx_unset').

    void appendDateTz(const bdlt::DateTz& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DATETZ' having the specified 'value'.

    void appendNullDateTz();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DATETZ'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bdlt::DateTz' (see
        // 'bdltuxxx_unset').

    void appendTime(const bdlt::Time& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_TIME' having the specified 'value'.

    void appendNullTime();
        // Append to this list a null element of type
        // 'ElemType::BDEM_TIME'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdlt::Time' (see
        // 'bdltuxxx_unset').

    void appendTimeTz(const bdlt::TimeTz& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_TIMETZ' having the specified 'value'.

    void appendNullTimeTz();
        // Append to this list a null element of type
        // 'ElemType::BDEM_TIMETZ'.  Note that, if accessed, the value
        // will be the corresponding unset value for 'bdlt::TimeTz' (see
        // 'bdltuxxx_unset').

    void appendBoolArray(const bsl::vector<bool>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_BOOL_ARRAY' having the specified 'value'.

    void appendNullBoolArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_BOOL_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendCharArray(const bsl::vector<char>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_CHAR_ARRAY' having the specified 'value'.

    void appendNullCharArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_CHAR_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendShortArray(const bsl::vector<short>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_SHORT_ARRAY' having the specified 'value'.

    void appendNullShortArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_SHORT_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendIntArray(const bsl::vector<int>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_INT_ARRAY' having the specified 'value'.

    void appendNullIntArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_INT_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendInt64Array(const bsl::vector<bsls::Types::Int64>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_INT64_ARRAY' having the specified 'value'.

    void appendNullInt64Array();
        // Append to this list a null element of type
        // 'ElemType::BDEM_INT64_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendFloatArray(const bsl::vector<float>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_FLOAT_ARRAY' having the specified 'value'.

    void appendNullFloatArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_FLOAT_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendDoubleArray(const bsl::vector<double>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DOUBLE_ARRAY' having the specified 'value'.

    void appendNullDoubleArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DOUBLE_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendStringArray(const bsl::vector<bsl::string>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_STRING_ARRAY' having the specified 'value'.

    void appendNullStringArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_STRING_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendDatetimeArray(const bsl::vector<bdlt::Datetime>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DATETIME_ARRAY' having the specified 'value'.

    void appendNullDatetimeArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DATETIME_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendDatetimeTzArray(const bsl::vector<bdlt::DatetimeTz>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DATETIMETZ_ARRAY' having the specified 'value'.

    void appendNullDatetimeTzArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DATETIMETZ_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendDateArray(const bsl::vector<bdlt::Date>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DATE_ARRAY' having the specified 'value'.

    void appendNullDateArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DATE_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendDateTzArray(const bsl::vector<bdlt::DateTz>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_DATETZ_ARRAY' having the specified 'value'.

    void appendNullDateTzArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_DATETZ_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendTimeArray(const bsl::vector<bdlt::Time>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_TIME_ARRAY' having the specified 'value'.

    void appendNullTimeArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_TIME_ARRAY'.  Note that, if accessed, the array
        // will have a size of 0.

    void appendTimeTzArray(const bsl::vector<bdlt::TimeTz>& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_TIMETZ_ARRAY' having the specified 'value'.

    void appendNullTimeTzArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_TIMETZ_ARRAY'.  Note that, if accessed, the
        // array will have a size of 0.

    void appendChoice(const Choice& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_CHOICE' having the specified 'value'.

    void appendNullChoice();
        // Append to this list a null element of type
        // 'ElemType::BDEM_CHOICE'.  Note that, if accessed, the choice
        // will have no selections and 'selector() < 0'.

    void appendChoiceArray(const ChoiceArray& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_CHOICE_ARRAY' having the specified 'value'.

    void appendNullChoiceArray();
        // Append to this list a null element of type
        // 'ElemType::BDEM_CHOICE_ARRAY'.  Note that, if accessed, the
        // choice array will have a size of 0 and no selections.

    void appendList(const Row& value);
    void appendList(const List& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_LIST' having the specified 'value'.  Note that
        // in the event of aliasing the *original* value of this list object
        // is assigned to the newly added list element.

    void appendNullList();
        // Append to this list a null element of type
        // 'ElemType::BDEM_LIST'.  Note that, if accessed, the list will
        // have a length of 0.

    void appendTable(const Table& value);
        // Append to this list a non-null element of type
        // 'ElemType::BDEM_TABLE' having the specified 'value'.

    void appendNullTable();
        // Append to this list a null element of type
        // 'ElemType::BDEM_TABLE'.  Note that, if accessed, the table will
        // have no rows and no columns.

    void appendElement(const ConstElemRef& srcElement);
        // Append to this list the value of the specified 'srcElement'.
        // Note that the behavior of this method is the same as:
        //..
        //  insertElement(length(), srcElement);
        //..

    void appendNullElement(ElemType::Type elementType);
        // Append to this list a null element of the type specified by
        // 'elementType'.  Note that, if accessed, the value will be the
        // corresponding unset value for 'elementType' (see 'bdltuxxx_unset').
        // Also note that the behavior of this method is the same as:
        //..
        //  insertNullElement(length(), elementType);
        //..

    void appendElements(const Row& srcElements);
    void appendElements(const List& srcElements);
        // Append to this list, as individual elements, the values from the
        // specified 'srcElements'.  Note that the behavior of this method is
        // the same as:
        //..
        //  insertElements(length(), srcElements);
        //..

    void appendElements(const Row&  srcElements,
                        int              srcIndex,
                        int              numElements);
    void appendElements(const List& srcElements,
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

    void appendNullElements(const ElemType::Type *elementTypes,
                            int                        length);
        // Append to this list a sequence of null elements of the specified
        // 'length' having the specified 'elementTypes'.  The behavior is
        // undefined unless 'elementTypes' contains at least 'length' types.
        // Note that, if accessed, the values will be the corresponding unset
        // values for each element added (see 'bdltuxxx_unset').  Also note that,
        // given a list 'mL', the behavior of this method is the same as:
        //..
        //  mL.insertNullElements(mL.length(), elementTypes, length);
        //..

    void appendNullElements(
                         const bsl::vector<ElemType::Type>& elementTypes);
        // Append to this list a sequence of null elements having the
        // specified 'elementTypes'.  Note that, if accessed, the values will
        // be the corresponding unset value for each element added (see
        // 'bdltuxxx_unset').  Also note that the behavior of this method is the
        // same as:
        //..
        //  insertNullElements(length(), elementTypes);
        //..

    void insertBool(int dstIndex, bool value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_BOOL' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullBool(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_BOOL'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.   The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bool' (see 'bdltuxxx_unset').

    void insertChar(int dstIndex, char value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_CHAR' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullChar(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_CHAR'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.   The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'char' (see 'bdltuxxx_unset').

    void insertShort(int dstIndex, short value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_SHORT' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullShort(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_SHORT'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'short' (see 'bdltuxxx_unset').

    void insertInt(int dstIndex, int value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_INT' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullInt(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_INT'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'int' (see 'bdltuxxx_unset').

    void insertInt64(int dstIndex, bsls::Types::Int64 value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_INT64' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullInt64(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_INT64'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'Int64' (see 'bdltuxxx_unset').

    void insertFloat(int dstIndex, float value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_FLOAT' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullFloat(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_FLOAT'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'float' (see 'bdltuxxx_unset').

    void insertDouble(int dstIndex, double value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DOUBLE' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullDouble(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DOUBLE'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'double' (see 'bdltuxxx_unset').

    void insertString(int dstIndex, const char *value);
    void insertString(int dstIndex, const bsl::string& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_STRING' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullString(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_STRING'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bsl::string' (see
        // 'bdltuxxx_unset').

    void insertDatetime(int dstIndex, const bdlt::Datetime& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DATETIME' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullDatetime(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DATETIME'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdlt::Datetime' (see
        // 'bdltuxxx_unset').

    void insertDatetimeTz(int dstIndex, const bdlt::DatetimeTz& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DATETIMETZ' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDatetimeTz(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DATETIMETZ'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdlt::DatetimeTz' (see
        // 'bdltuxxx_unset').

    void insertDate(int dstIndex, const bdlt::Date& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DATE' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullDate(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DATE'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdlt::Date' (see
        // 'bdltuxxx_unset').

    void insertDateTz(int dstIndex, const bdlt::DateTz& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DATETZ' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullDateTz(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DATETZ'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdetDateTz' (see
        // 'bdltuxxx_unset').

    void insertTime(int dstIndex, const bdlt::Time& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_TIME' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullTime(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_TIME'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdlt::Time' (see
        // 'bdltuxxx_unset').

    void insertTimeTz(int dstIndex, const bdlt::TimeTz& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_TIMETZ' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullTimeTz(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_TIMETZ'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'bdlt::TimeTz' (see
        // 'bdltuxxx_unset').

    void insertBoolArray(int dstIndex, const bsl::vector<bool>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_BOOL_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullBoolArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_BOOL_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertCharArray(int dstIndex, const bsl::vector<char>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_CHAR_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullCharArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_CHAR_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertShortArray(int dstIndex, const bsl::vector<short>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_SHORT_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullShortArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_SHORT_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertIntArray(int dstIndex, const bsl::vector<int>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_INT_ARRAY' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullIntArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_INT_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertInt64Array(int                                    dstIndex,
                          const bsl::vector<bsls::Types::Int64>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_INT64_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullInt64Array(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_INT64_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertFloatArray(int dstIndex, const bsl::vector<float>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_FLOAT_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullFloatArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_FLOAT_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDoubleArray(int dstIndex, const bsl::vector<double>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DOUBLE_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDoubleArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DOUBLE_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertStringArray(int                             dstIndex,
                           const bsl::vector<bsl::string>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_STRING_ARRAY' having the
        // specified 'value'.   Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullStringArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_STRING_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDatetimeArray(int                               dstIndex,
                             const bsl::vector<bdlt::Datetime>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DATETIME_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDatetimeArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DATETIME_ARRAY'.  Each element having
        // an index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDatetimeTzArray(int                               dstIndex,
                             const bsl::vector<bdlt::DatetimeTz>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DATETIMETZ_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDatetimeTzArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DATETIMETZ_ARRAY'.  Each element having
        // an index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDateArray(int dstIndex, const bsl::vector<bdlt::Date>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DATE_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDateArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DATE_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertDateTzArray(int                             dstIndex,
                           const bsl::vector<bdlt::DateTz>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_DATETZ_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullDateTzArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_DATETZ_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertTimeArray(int dstIndex, const bsl::vector<bdlt::Time>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_TIME_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullTimeArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_TIME_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertTimeTzArray(int                             dstIndex,
                           const bsl::vector<bdlt::TimeTz>& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_TIMETZ_ARRAY' having the
        // specified 'value'.  Each element having an index greater than or
        // equal to 'dstIndex' before the insertion is shifted up by one index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullTimeTzArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_TIMETZ_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the array will
        // have a size of 0.

    void insertChoice(int dstIndex, const Choice& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_CHOICE' having the value
        // represented by the specified 'value'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullChoice(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_CHOICE'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the choice
        // will have no selections and 'selector() < 0'.

    void insertChoiceArray(int dstIndex, const ChoiceArray& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_CHOICE_ARRAY' having the value
        // represented by the specified 'value'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullChoiceArray(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_CHOICE_ARRAY'.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the choice
        // array will have a size of 0 and no selections.

    void insertList(int dstIndex, const Row& value);
    void insertList(int dstIndex, const List& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_LIST' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.  Note
        // that in the event of aliasing the *original* value of this list
        // object is assigned to the newly added list element.

    void insertNullList(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_LIST'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the list will
        // have a length of 0.

    void insertTable(int dstIndex, const Table& value);
        // Insert into this list, at the specified 'dstIndex', a non-null
        // element of type 'ElemType::BDEM_TABLE' having the specified
        // 'value'.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertNullTable(int dstIndex);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of type 'ElemType::BDEM_TABLE'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the table will
        // have no rows and no columns.

    void insertElement(int dstIndex, const ConstElemRef& srcElement);
        // Insert into this list, at the specified 'dstIndex', the value of the
        // specified 'srcElement'.  Each element having an index greater than
        // or equal to 'dstIndex' before the insertion is shifted up by one
        // index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertNullElement(int dstIndex, ElemType::Type elementType);
        // Insert into this list, at the specified 'dstIndex', a null element
        // of the specified 'elementType'.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'elementType' (see
        // 'bdltuxxx_unset').

    void insertElements(int dstIndex, const Row& srcElements);
    void insertElements(int dstIndex, const List& srcElements);
        // Insert into this list, as individual elements beginning at the
        // specified 'dstIndex', the values of the specified 'srcElements'.
        // Each element having an index greater than or equal to 'dstIndex'
        // before the insertion is shifted up by 'srcElements.length()' index
        // positions.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertElements(int              dstIndex,
                        const Row&  srcElements,
                        int              srcIndex,
                        int              numElements);
    void insertElements(int              dstIndex,
                        const List& srcElements,
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
                         const bsl::vector<ElemType::Type>& elementTypes);
        // Insert into this list, at the specified 'dstIndex', a sequence of
        // null elements having the specified 'elementTypes'.  Each element
        // having an index greater than or equal to 'dstIndex' before the
        // insertion is shifted up by 'elementTypes.size()' index positions.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.  Note
        // that, if accessed, each value will be the corresponding unset value
        // for the respective element type (see 'bdltuxxx_unset').

    void insertNullElements(int                        dstIndex,
                            const ElemType::Type *elementTypes,
                            int                        length);
        // Insert into this list, at the specified 'dstIndex', a sequence of
        // null elements of the specified 'length' having the specified
        // 'elementTypes'.  Each element having an index greater than or equal
        // to 'dstIndex' before the insertion is shifted up by 'length' index
        // positions.  The behavior is undefined unless
        // '0 <= dstIndex <= length()' and 'elementTypes' contains at least
        // 'length' types.  Note that, if accessed, the values will be the
        // corresponding unset values value for each element added (see
        // 'bdltuxxx_unset').

    void makeAllNull();
        // Set the value of each element in this list to null.  Note that, if
        // accessed, the values will be the corresponding unset values for the
        // the respective element types (see 'bdltuxxx_unset').  Also note that
        // the behavior of this method is the same as:
        //..
        //  row().makeAllNull();
        //..

    void makeNull(int index);
        // Set the value of the element at the specified 'index' in this list
        // to null.  The behavior is undefined unless '0 <= index < length()'.
        // Note that, if accessed, the value will be the corresponding unset
        // value for its element type (see 'bdltuxxx_unset').

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

    void replaceElement(int dstIndex, const ConstElemRef& srcElement);
        // Replace the type and value of the element at the specified
        // 'dstIndex' in this list with the type and value of the specified
        // 'srcElement'.  The behavior is undefined unless
        // '0 <= dstIndex < length()'.

    void resetElement(int index, ElemType::Type elemType);
        // Replace the type and value of the element at the specified 'index'
        // in this list to that of a null element having the specified
        // 'elemType'.  The behavior is undefined unless
        // '0 <= dstIndex < length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for 'elemType' (see 'bdltuxxx_unset').
        // Also note that, absent aliasing, the behavior of this method is the
        // same as:
        //..
        //  removeElement(dstIndex);
        //  insertNullElement(dstIndex, elemType);
        //..

    void reset(const ElemType::Type *elementTypes, int length);
        // Replace all elements (types and values) in this list with
        // a sequence of null elements of the specified 'length' having the
        // specified 'elementTypes'.  The behavior is undefined unless
        // '0 <= length' and 'elementTypes' contains at least 'length' types.
        // Note that, if accessed, the values will be the corresponding unset
        // values for the respective element types (see 'bdltuxxx_unset').  Also
        // note that the behavior of this method is the same as:
        //..
        //  removeAll();
        //  appendNullElements(elementTypes, length);
        //..

    void reset(const bsl::vector<ElemType::Type>& elementTypes);
        // Replace all elements (types and values) in this list with a
        // sequence of null elements having the specified 'elementTypes'.  Note
        // that, if accessed, the values will be the corresponding unset values
        // for the corresponding element types (see 'bdltuxxx_unset').  Also
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

    void swap(List& other);
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
    ConstElemRef operator[](int index) const;
        // Return a 'bdem' element reference to the non-modifiable element
        // value at the specified 'index' in this list.  The behavior is
        // undefined unless '0 <= index < length()'.

    ElemType::Type elemType(int index) const;
        // Return the 'bdem' type of the element at the specified 'index'
        // position in this list.  The behavior is undefined unless
        // '0 <= index < length()'.

    void elemTypes(bsl::vector<ElemType::Type> *result) const;
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

    const Row& row() const;
        // Return a reference to the underlying, heterogeneous row of
        // non-modifiable elements in this list.  Note that a 'Row' object
        // does not permit the number or types of its elements to be modified.

    const bool& theBool(int index) const;
    const char& theChar(int index) const;
    const short& theShort(int index) const;
    const int& theInt(int index) const;
    const bsls::Types::Int64& theInt64(int index) const;
    const float& theFloat(int index) const;
    const double& theDouble(int index) const;
    const bsl::string& theString(int index) const;
    const bdlt::Datetime& theDatetime(int index) const;
    const bdlt::DatetimeTz& theDatetimeTz(int index) const;
    const bdlt::Date& theDate(int index) const;
    const bdlt::DateTz& theDateTz(int index) const;
    const bdlt::Time& theTime(int index) const;
    const bdlt::TimeTz& theTimeTz(int index) const;
    const bsl::vector<bool>& theBoolArray(int index) const;
    const bsl::vector<char>& theCharArray(int index) const;
    const bsl::vector<short>& theShortArray(int index) const;
    const bsl::vector<int>& theIntArray(int index) const;
    const bsl::vector<bsls::Types::Int64>& theInt64Array(int index) const;
    const bsl::vector<float>& theFloatArray(int index) const;
    const bsl::vector<double>& theDoubleArray(int index) const;
    const bsl::vector<bsl::string>& theStringArray(int index) const;
    const bsl::vector<bdlt::Datetime>& theDatetimeArray(int index) const;
    const bsl::vector<bdlt::DatetimeTz>& theDatetimeTzArray(int index) const;
    const bsl::vector<bdlt::Date>& theDateArray(int index) const;
    const bsl::vector<bdlt::DateTz>& theDateTzArray(int index) const;
    const bsl::vector<bdlt::Time>& theTimeArray(int index) const;
    const bsl::vector<bdlt::TimeTz>& theTimeTzArray(int index) const;
    const Choice& theChoice(int index) const;
    const ChoiceArray& theChoiceArray(int index) const;
    const List& theList(int index) const;
    const Table& theTable(int index) const;
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
bool operator==(const List& lhs, const List& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' list objects have the
    // same value, and 'false' otherwise.  Two list objects have the same value
    // if they have the same number of elements, and corresponding elements at
    // each index position have the same type and value.

bool operator!=(const List& lhs, const List& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' list objects do not have
    // the same value, and 'false' otherwise.  Two list objects do not have the
    // same value if they do not have the same number of elements, or there are
    // corresponding elements at some index position that do not have the same
    // type or value.

bsl::ostream& operator<<(bsl::ostream& stream, const List& list);
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
                        // class List
                        // ---------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
int List::length() const
{
    return d_listImp.length();
}

                        // -------------------
                        // All Other Functions
                        // -------------------

// PRIVATE MANIPULATORS
inline
void *List::elemData(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.rowData().elemData(index);
}

// PRIVATE ACCESSORS
inline
const void *List::elemData(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.rowData().elemData(index);
}

// CLASS METHODS
inline
int List::maxSupportedBdexVersion()
{
    return 3;
}

// CREATORS
inline
List::List(bslma::Allocator *basicAllocator)
: d_listImp(AggregateOption::BDEM_PASS_THROUGH, basicAllocator)
{
}

inline
List::List(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(allocationStrategy, basicAllocator)
{
}

inline
List::List(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(allocationStrategy,
            ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= initialMemorySize);
}

inline
List::List(const ElemType::Type *elementTypes,
                     int                        numElements,
                     bslma::Allocator          *basicAllocator)
: d_listImp(elementTypes,
            numElements,
            ElemAttrLookup::lookupTable(),
            AggregateOption::BDEM_PASS_THROUGH,
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numElements);
}

inline
List::List(
                  const ElemType::Type                *elementTypes,
                  int                                       numElements,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(elementTypes,
            numElements,
            ElemAttrLookup::lookupTable(),
            allocationStrategy,
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numElements);
}

inline
List::List(
                  const ElemType::Type                *elementTypes,
                  int                                       numElements,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(elementTypes,
            numElements,
            ElemAttrLookup::lookupTable(),
            allocationStrategy,
            ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(0 <= initialMemorySize);
}

inline
List::List(const Row&   original,
                     bslma::Allocator *basicAllocator)
: d_listImp(reinterpret_cast<const RowData&>(original),
            AggregateOption::BDEM_PASS_THROUGH,
            basicAllocator)
{
}

inline
List::List(const List&  original,
                     bslma::Allocator *basicAllocator)
: d_listImp(reinterpret_cast<const ListImp&>(original),
            AggregateOption::BDEM_PASS_THROUGH,
            basicAllocator)
{
}

inline
List::List(
                  const Row&                           original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(reinterpret_cast<const RowData&>(original),
            allocationStrategy,
            basicAllocator)
{
}

inline
List::List(
                  const Row&                           original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(reinterpret_cast<const RowData&>(original),
            allocationStrategy,
            ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= initialMemorySize);
}

inline
List::List(
                  const List&                          original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(reinterpret_cast<const ListImp&>(original),
            allocationStrategy,
            basicAllocator)
{
}

inline
List::List(
                  const List&                          original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(reinterpret_cast<const ListImp&>(original),
            allocationStrategy,
            ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT_SAFE(0 <= initialMemorySize);
}

inline
List::~List()
{
    // All destructor actions are in ListImp::~ListImp().
}

// MANIPULATORS
inline
List& List::operator=(const Row& rhs)
{
    d_listImp = reinterpret_cast<const RowData&>(rhs);
    return *this;
}

inline
List& List::operator=(const List& rhs)
{
    d_listImp = rhs.d_listImp;
    return *this;
}

inline
ElemRef List::operator[](int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.rowData().elemRef(index);
}

inline
Row& List::row()
{
    return reinterpret_cast<Row&>(d_listImp.rowData());
}

inline
bool& List::theModifiableBool(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bool *>(elemData(index));
}

inline
char& List::theModifiableChar(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<char *>(elemData(index));
}

inline
short& List::theModifiableShort(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<short *>(elemData(index));
}

inline
int& List::theModifiableInt(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<int *>(elemData(index));
}

inline
bsls::Types::Int64& List::theModifiableInt64(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsls::Types::Int64 *>(elemData(index));
}

inline
float& List::theModifiableFloat(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<float *>(elemData(index));
}

inline
double& List::theModifiableDouble(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<double *>(elemData(index));
}

inline
bsl::string& List::theModifiableString(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::string *>(elemData(index));
}

inline
bdlt::Datetime& List::theModifiableDatetime(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdlt::Datetime *>(elemData(index));
}

inline
bdlt::DatetimeTz& List::theModifiableDatetimeTz(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdlt::DatetimeTz *>(elemData(index));
}

inline
bdlt::Date& List::theModifiableDate(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdlt::Date *>(elemData(index));
}

inline
bdlt::DateTz& List::theModifiableDateTz(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdlt::DateTz *>(elemData(index));
}

inline
bdlt::Time& List::theModifiableTime(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdlt::Time *>(elemData(index));
}

inline
bdlt::TimeTz& List::theModifiableTimeTz(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bdlt::TimeTz *>(elemData(index));
}

inline
bsl::vector<bool>& List::theModifiableBoolArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bool> *>(elemData(index));
}

inline
bsl::vector<char>& List::theModifiableCharArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<char> *>(elemData(index));
}

inline
bsl::vector<short>& List::theModifiableShortArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<short> *>(elemData(index));
}

inline
bsl::vector<int>& List::theModifiableIntArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<int> *>(elemData(index));
}

inline
bsl::vector<bsls::Types::Int64>&
List::theModifiableInt64Array(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bsls::Types::Int64> *>(elemData(index));
}

inline
bsl::vector<float>& List::theModifiableFloatArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<float> *>(elemData(index));
}

inline
bsl::vector<double>& List::theModifiableDoubleArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<double> *>(elemData(index));
}

inline
bsl::vector<bsl::string>& List::theModifiableStringArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bsl::string> *>(elemData(index));
}

inline
bsl::vector<bdlt::Datetime>& List::theModifiableDatetimeArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdlt::Datetime> *>(elemData(index));
}

inline
bsl::vector<bdlt::DatetimeTz>&
List::theModifiableDatetimeTzArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdlt::DatetimeTz> *>(elemData(index));
}

inline
bsl::vector<bdlt::Date>& List::theModifiableDateArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdlt::Date> *>(elemData(index));
}

inline
bsl::vector<bdlt::DateTz>& List::theModifiableDateTzArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdlt::DateTz> *>(elemData(index));
}

inline
bsl::vector<bdlt::Time>& List::theModifiableTimeArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdlt::Time> *>(elemData(index));
}

inline
bsl::vector<bdlt::TimeTz>& List::theModifiableTimeTzArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<bsl::vector<bdlt::TimeTz> *>(elemData(index));
}

inline
Choice& List::theModifiableChoice(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<Choice *>(elemData(index));
}

inline
ChoiceArray& List::theModifiableChoiceArray(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<ChoiceArray *>(elemData(index));
}

inline
List& List::theModifiableList(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<List *>(elemData(index));
}

inline
Table& List::theModifiableTable(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<Table *>(elemData(index));
}

inline
void List::appendNullBool()
{
    insertNullBool(length());
}

inline
void List::appendBool(bool value)
{
    insertBool(length(), value);
}

inline
void List::appendNullChar()
{
    insertNullChar(length());
}

inline
void List::appendChar(char value)
{
    insertChar(length(), value);
}

inline
void List::appendNullShort()
{
    insertNullShort(length());
}

inline
void List::appendShort(short value)
{
    insertShort(length(), value);
}

inline
void List::appendNullInt()
{
    insertNullInt(length());
}

inline
void List::appendInt(int value)
{
    insertInt(length(), value);
}

inline
void List::appendNullInt64()
{
    insertNullInt64(length());
}

inline
void List::appendInt64(bsls::Types::Int64 value)
{
    insertInt64(length(), value);
}

inline
void List::appendNullFloat()
{
    insertNullFloat(length());
}

inline
void List::appendFloat(float value)
{
    insertFloat(length(), value);
}

inline
void List::appendNullDouble()
{
    insertNullDouble(length());
}

inline
void List::appendDouble(double value)
{
    insertDouble(length(), value);
}

inline
void List::appendNullString()
{
    insertNullString(length());
}

inline
void List::appendString(const char *value)
{
    insertString(length(), value);
}

inline
void List::appendString(const bsl::string& value)
{
    insertString(length(), value);
}

inline
void List::appendNullDatetime()
{
    insertNullDatetime(length());
}

inline
void List::appendDatetime(const bdlt::Datetime& value)
{
    insertDatetime(length(), value);
}

inline
void List::appendNullDatetimeTz()
{
    insertNullDatetimeTz(length());
}

inline
void List::appendDatetimeTz(const bdlt::DatetimeTz& value)
{
    insertDatetimeTz(length(), value);
}

inline
void List::appendNullDate()
{
    insertNullDate(length());
}

inline
void List::appendDate(const bdlt::Date& value)
{
    insertDate(length(), value);
}

inline
void List::appendNullDateTz()
{
    insertNullDateTz(length());
}

inline
void List::appendDateTz(const bdlt::DateTz& value)
{
    insertDateTz(length(), value);
}

inline
void List::appendNullTime()
{
    insertNullTime(length());
}

inline
void List::appendTime(const bdlt::Time& value)
{
    insertTime(length(), value);
}

inline
void List::appendNullTimeTz()
{
    insertNullTimeTz(length());
}

inline
void List::appendTimeTz(const bdlt::TimeTz& value)
{
    insertTimeTz(length(), value);
}

inline
void List::appendNullBoolArray()
{
    insertNullBoolArray(length());
}

inline
void List::appendBoolArray(const bsl::vector<bool>& value)
{
    insertBoolArray(length(), value);
}

inline
void List::appendNullCharArray()
{
    insertNullCharArray(length());
}

inline
void List::appendCharArray(const bsl::vector<char>& value)
{
    insertCharArray(length(), value);
}

inline
void List::appendNullShortArray()
{
    insertNullShortArray(length());
}

inline
void List::appendShortArray(const bsl::vector<short>& value)
{
    insertShortArray(length(), value);
}

inline
void List::appendNullIntArray()
{
    insertNullIntArray(length());
}

inline
void List::appendIntArray(const bsl::vector<int>& value)
{
    insertIntArray(length(), value);
}

inline
void List::appendNullInt64Array()
{
    insertNullInt64Array(length());
}

inline
void
List::appendInt64Array(const bsl::vector<bsls::Types::Int64>& value)
{
    insertInt64Array(length(), value);
}

inline
void List::appendNullFloatArray()
{
    insertNullFloatArray(length());
}

inline
void List::appendFloatArray(const bsl::vector<float>& value)
{
    insertFloatArray(length(), value);
}

inline
void List::appendNullDoubleArray()
{
    insertNullDoubleArray(length());
}

inline
void List::appendDoubleArray(const bsl::vector<double>& value)
{
    insertDoubleArray(length(), value);
}

inline
void List::appendNullStringArray()
{
    insertNullStringArray(length());
}

inline
void List::appendStringArray(const bsl::vector<bsl::string>& value)
{
    insertStringArray(length(), value);
}

inline
void List::appendNullDatetimeArray()
{
    insertNullDatetimeArray(length());
}

inline
void List::appendDatetimeArray(const bsl::vector<bdlt::Datetime>& value)
{
    insertDatetimeArray(length(), value);
}

inline
void List::appendNullDatetimeTzArray()
{
    insertNullDatetimeTzArray(length());
}

inline
void List::appendDatetimeTzArray(
                                     const bsl::vector<bdlt::DatetimeTz>& value)
{
    insertDatetimeTzArray(length(), value);
}

inline
void List::appendNullDateArray()
{
    insertNullDateArray(length());
}

inline
void List::appendDateArray(const bsl::vector<bdlt::Date>& value)
{
    insertDateArray(length(), value);
}

inline
void List::appendNullDateTzArray()
{
    insertNullDateTzArray(length());
}

inline
void List::appendDateTzArray(const bsl::vector<bdlt::DateTz>& value)
{
    insertDateTzArray(length(), value);
}

inline
void List::appendNullTimeArray()
{
    insertNullTimeArray(length());
}

inline
void List::appendTimeArray(const bsl::vector<bdlt::Time>& value)
{
    insertTimeArray(length(), value);
}

inline
void List::appendNullTimeTzArray()
{
    insertNullTimeTzArray(length());
}

inline
void List::appendTimeTzArray(const bsl::vector<bdlt::TimeTz>& value)
{
    insertTimeTzArray(length(), value);
}

inline
void List::appendNullChoice()
{
    insertNullChoice(length());
}

inline
void List::appendChoice(const Choice& value)
{
    insertChoice(length(), value);
}

inline
void List::appendNullChoiceArray()
{
    insertNullChoiceArray(length());
}

inline
void List::appendChoiceArray(const ChoiceArray& value)
{
    insertChoiceArray(length(), value);
}

inline
void List::appendNullList()
{
    insertNullList(length());
}

inline
void List::appendList(const List& value)
{
    insertList(length(), value);
}

inline
void List::appendList(const Row& value)
{
    insertList(length(), value);
}

inline
void List::appendNullTable()
{
    insertNullTable(length());
}

inline
void List::appendTable(const Table& value)
{
    insertTable(length(), value);
}

inline
void List::appendNullElement(ElemType::Type elementType)
{
    insertNullElement(length(), elementType);
}

inline
void List::appendNullElements(const ElemType::Type *elementTypes,
                                   int                        length)
{
    BSLS_ASSERT_SAFE(0 <= length);

    insertNullElements(this->length(), elementTypes, length);
}

inline
void List::appendNullElements(
                          const bsl::vector<ElemType::Type>& elementTypes)
{
    insertNullElements(length(), elementTypes);
}

inline
void List::appendElement(const ConstElemRef& srcElement)
{
    insertElement(length(), srcElement);
}

inline
void List::appendElements(const Row& srcElements)
{
    insertElements(length(), srcElements);
}

inline
void List::appendElements(const List& srcElements)
{
    insertElements(length(), srcElements);
}

inline
void List::appendElements(const Row& srcElements,
                               int             srcIndex,
                               int             numElements)
{
    BSLS_ASSERT_SAFE(0 <= srcIndex);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(srcIndex + numElements <= srcElements.length());

    insertElements(length(), srcElements, srcIndex, numElements);
}

inline
void List::appendElements(const List& srcElements,
                               int              srcIndex,
                               int              numElements)
{
    BSLS_ASSERT_SAFE(0 <= srcIndex);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(srcIndex + numElements <= srcElements.length());

    insertElements(length(), srcElements, srcIndex, numElements);
}

inline
void List::insertNullBool(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_boolAttr);
}

inline
void List::insertBool(int dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_boolAttr);
}

inline
void List::insertNullChar(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_charAttr);
}

inline
void List::insertChar(int dstIndex, char value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_charAttr);
}

inline
void List::insertNullShort(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_shortAttr);
}

inline
void List::insertShort(int dstIndex, short value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_shortAttr);
}

inline
void List::insertNullInt(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_intAttr);
}

inline
void List::insertInt(int dstIndex, int value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_intAttr);
}

inline
void List::insertNullInt64(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_int64Attr);
}

inline
void List::insertInt64(int dstIndex, bsls::Types::Int64 value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_int64Attr);
}

inline
void List::insertNullFloat(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_floatAttr);
}

inline
void List::insertFloat(int dstIndex, float value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_floatAttr);
}

inline
void List::insertNullDouble(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_doubleAttr);
}

inline
void List::insertDouble(int dstIndex, double value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_doubleAttr);
}

inline
void List::insertNullString(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_stringAttr);
}

inline
void List::insertString(int dstIndex, const char *value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertString(dstIndex, value);
}

inline
void List::insertString(int dstIndex, const bsl::string& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, &value, &Properties::s_stringAttr);
}

inline
void List::insertNullDatetime(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_datetimeAttr);
}

inline
void List::insertDatetime(int dstIndex, const bdlt::Datetime& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_datetimeAttr);
}

inline
void List::insertNullDatetimeTz(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_datetimeTzAttr);
}

inline
void List::insertDatetimeTz(int dstIndex, const bdlt::DatetimeTz& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_datetimeTzAttr);
}

inline
void List::insertNullDate(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_dateAttr);
}

inline
void List::insertDate(int dstIndex, const bdlt::Date& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_dateAttr);
}

inline
void List::insertNullDateTz(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_dateTzAttr);
}

inline
void List::insertDateTz(int dstIndex, const bdlt::DateTz& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_dateTzAttr);
}

inline
void List::insertNullTime(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_timeAttr);
}

inline
void List::insertTime(int dstIndex, const bdlt::Time& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_timeAttr);
}

inline
void List::insertNullTimeTz(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_timeTzAttr);
}

inline
void List::insertTimeTz(int dstIndex, const bdlt::TimeTz& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            static_cast<const void *>(&value),
                            &Properties::s_timeTzAttr);
}

inline
void List::insertNullBoolArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_boolArrayAttr);
}

inline
void List::insertBoolArray(int dstIndex, const bsl::vector<bool>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_boolArrayAttr);
}

inline
void List::insertNullCharArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_charArrayAttr);
}

inline
void List::insertCharArray(int dstIndex, const bsl::vector<char>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_charArrayAttr);
}

inline
void List::insertNullShortArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_shortArrayAttr);
}

inline
void List::insertShortArray(int dstIndex, const bsl::vector<short>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_shortArrayAttr);
}

inline
void List::insertNullIntArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_intArrayAttr);
}

inline
void List::insertIntArray(int dstIndex, const bsl::vector<int>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_intArrayAttr);
}

inline
void List::insertNullInt64Array(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &Properties::s_int64ArrayAttr);
}

inline
void
List::insertInt64Array(int                                    dstIndex,
                            const bsl::vector<bsls::Types::Int64>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_int64ArrayAttr);
}

inline
void List::insertNullFloatArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &Properties::s_floatArrayAttr);
}

inline
void List::insertFloatArray(int                       dstIndex,
                                 const bsl::vector<float>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_floatArrayAttr);
}

inline
void List::insertNullDoubleArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &Properties::s_doubleArrayAttr);
}

inline
void List::insertDoubleArray(int                        dstIndex,
                                  const bsl::vector<double>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_doubleArrayAttr);
}

inline
void List::insertNullStringArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &Properties::s_stringArrayAttr);
}

inline
void List::insertStringArray(int                             dstIndex,
                                  const bsl::vector<bsl::string>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_stringArrayAttr);
}

inline
void List::insertNullDatetimeArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &Properties::s_datetimeArrayAttr);
}

inline
void List::insertDatetimeArray(int                               dstIndex,
                                    const bsl::vector<bdlt::Datetime>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_datetimeArrayAttr);
}

inline
void List::insertNullDatetimeTzArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &Properties::s_datetimeTzArrayAttr);
}

inline
void List::insertDatetimeTzArray(
                                 int                                 dstIndex,
                                 const bsl::vector<bdlt::DatetimeTz>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_datetimeTzArrayAttr);
}

inline
void List::insertNullDateArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_dateArrayAttr);
}

inline
void List::insertDateArray(int                           dstIndex,
                                const bsl::vector<bdlt::Date>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_dateArrayAttr);
}

inline
void List::insertNullDateTzArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &Properties::s_dateTzArrayAttr);
}

inline
void List::insertDateTzArray(int                           dstIndex,
                                const bsl::vector<bdlt::DateTz>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_dateTzArrayAttr);
}

inline
void List::insertNullTimeArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_timeArrayAttr);
}

inline
void List::insertTimeArray(int                           dstIndex,
                                const bsl::vector<bdlt::Time>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_timeArrayAttr);
}

inline
void List::insertNullTimeTzArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &Properties::s_timeTzArrayAttr);
}

inline
void List::insertTimeTzArray(int                             dstIndex,
                                  const bsl::vector<bdlt::TimeTz>& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &Properties::s_timeTzArrayAttr);
}

inline
void List::insertNullChoice(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &ChoiceImp::s_choiceAttr);
}

inline
void List::insertChoice(int dstIndex, const Choice& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, &value, &ChoiceImp::s_choiceAttr);
}

inline
void List::insertNullChoiceArray(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex,
                                &ChoiceArrayImp::s_choiceArrayAttr);
}

inline
void List::insertChoiceArray(int                     dstIndex,
                                  const ChoiceArray& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex,
                            &value,
                            &ChoiceArrayImp::s_choiceArrayAttr);
}

inline
void List::insertNullList(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &ListImp::s_listAttr);
}

inline
void List::insertList(int dstIndex, const List& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, &value, &ListImp::s_listAttr);
}

inline
void List::insertList(int dstIndex, const Row& srcRow)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertList(dstIndex,
                         reinterpret_cast<const RowData&>(srcRow));
}

inline
void List::insertNullTable(int dstIndex)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(dstIndex, &TableImp::s_tableAttr);
}

inline
void List::insertTable(int dstIndex, const Table& value)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, &value, &TableImp::s_tableAttr);
}

inline
void List::insertNullElement(int                 dstIndex,
                                  ElemType::Type elementType)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertNullElement(
                              dstIndex,
                              ElemAttrLookup::lookupTable()[elementType]);
}

inline
void List::insertNullElements(int                        dstIndex,
                                   const ElemType::Type *elementTypes,
                                   int                        length)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= this->length());
    BSLS_ASSERT_SAFE(0 <= length);

    d_listImp.insertNullElements(dstIndex,
                                 elementTypes,
                                 length,
                                 ElemAttrLookup::lookupTable());
}

inline
void List::insertElement(int                      dstIndex,
                              const ConstElemRef& srcElement)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    d_listImp.insertElement(dstIndex, srcElement);
}

inline
void List::insertElements(int dstIndex, const Row& srcRow)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    const RowData& rowData =
                                 reinterpret_cast<const RowData&>(srcRow);
    insertElements(dstIndex, srcRow, 0, rowData.length());
}

inline
void List::insertElements(int dstIndex, const List& srcList)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex <= length());

    const RowData& rowData =
                          reinterpret_cast<const RowData&>(srcList.row());
    insertElements(dstIndex, srcList.row(), 0, rowData.length());
}

inline
void List::insertElements(int              dstIndex,
                               const Row&  srcElements,
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
                            reinterpret_cast<const RowData&>(srcElements),
                            srcIndex,
                            numElements);
    }
}

inline
void List::insertElements(int              dstIndex,
                               const List& srcElements,
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
void List::makeAllNull()
{
    d_listImp.rowData().makeAllNull();
}

inline
void List::makeNull(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    d_listImp.rowData().makeNull(index);
}

inline
void List::removeElement(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    d_listImp.removeElement(index);
}

inline
void List::removeElements(int startIndex, int numElements)
{
    BSLS_ASSERT_SAFE(0 <= startIndex);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(startIndex + numElements <= length());

    d_listImp.removeElements(startIndex, numElements);
}

inline
void List::removeAll()
{
    d_listImp.removeAll();
}

inline
void
List::replaceElement(int dstIndex, const ConstElemRef& srcElement)
{
    BSLS_ASSERT_SAFE(0 <= dstIndex);
    BSLS_ASSERT_SAFE(     dstIndex < length());

    d_listImp.replaceElement(dstIndex, srcElement);
}

inline
void List::reserveMemory(int numBytes)
{
    BSLS_ASSERT_SAFE(0 <= numBytes);

    d_listImp.reserveMemory(numBytes);
}

inline
void List::resetElement(int index, ElemType::Type elemType)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    d_listImp.resetElement(index,
                           ElemAttrLookup::lookupTable()[elemType]);
}

inline
void List::reset(const ElemType::Type *elementTypes, int length)
{
    BSLS_ASSERT_SAFE(0 <= length);

    d_listImp.reset(elementTypes,
                    length,
                    ElemAttrLookup::lookupTable());
}

inline
void List::compact()
{
    d_listImp.compact();
}

inline
void List::swap(List& other)
{
    d_listImp.swap(other.d_listImp);
}

inline
void List::swapElements(int index1, int index2)
{
    BSLS_ASSERT_SAFE(0 <= index1);
    BSLS_ASSERT_SAFE(     index1 < length());
    BSLS_ASSERT_SAFE(0 <= index2);
    BSLS_ASSERT_SAFE(     index2 < length());

    d_listImp.swap(index1, index2);
}

template <class STREAM>
inline
STREAM& List::bdexStreamIn(STREAM& stream, int version)
{
    d_listImp.bdexStreamInImp(
                            stream,
                            version,
                            ElemStreamInAttrLookup<STREAM>::lookupTable(),
                            ElemAttrLookup::lookupTable());
    return stream;
}

// ACCESSORS
inline
ConstElemRef List::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.rowData().elemRef(index);
}

inline
ElemType::Type List::elemType(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return d_listImp.elemType(index);
}

inline
bool List::isAnyNull() const
{
    return d_listImp.rowData().isAnyInRangeNull(0, d_listImp.length());
}

inline
bool List::isAnyNonNull() const
{
    return d_listImp.rowData().isAnyInRangeNonNull(0, d_listImp.length());
}

inline
const Row& List::row() const
{
    return reinterpret_cast<const Row&>(d_listImp.rowData());
}

inline
const bool& List::theBool(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bool *>(elemData(index));
}

inline
const char& List::theChar(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const char *>(elemData(index));
}

inline
const short& List::theShort(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const short *>(elemData(index));
}

inline
const int& List::theInt(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const int *>(elemData(index));
}

inline
const bsls::Types::Int64& List::theInt64(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsls::Types::Int64 *>(elemData(index));
}

inline
const float& List::theFloat(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const float *>(elemData(index));
}

inline
const double& List::theDouble(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const double *>(elemData(index));
}

inline
const bsl::string& List::theString(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::string *>(elemData(index));
}

inline
const bdlt::Datetime& List::theDatetime(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdlt::Datetime *>(elemData(index));
}

inline
const bdlt::DatetimeTz& List::theDatetimeTz(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdlt::DatetimeTz *>(elemData(index));
}

inline
const bdlt::Date& List::theDate(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdlt::Date *>(elemData(index));
}

inline
const bdlt::DateTz& List::theDateTz(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdlt::DateTz *>(elemData(index));
}

inline
const bdlt::Time& List::theTime(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdlt::Time *>(elemData(index));
}

inline
const bdlt::TimeTz& List::theTimeTz(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bdlt::TimeTz *>(elemData(index));
}

inline
const bsl::vector<bool>& List::theBoolArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bool> *>(elemData(index));
}

inline
const bsl::vector<char>& List::theCharArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<char> *>(elemData(index));
}

inline
const bsl::vector<short>& List::theShortArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<short> *>(elemData(index));
}

inline
const bsl::vector<int>& List::theIntArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<int> *>(elemData(index));
}

inline
const
bsl::vector<bsls::Types::Int64>& List::theInt64Array(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bsls::Types::Int64> *>(
                                                              elemData(index));
}

inline
const bsl::vector<float>& List::theFloatArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<float> *>(elemData(index));
}

inline
const bsl::vector<double>& List::theDoubleArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<double> *>(elemData(index));
}

inline
const bsl::vector<bsl::string>& List::theStringArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bsl::string> *>(elemData(index));
}

inline
const bsl::vector<bdlt::Datetime>& List::theDatetimeArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdlt::Datetime> *>(elemData(index));
}

inline
const bsl::vector<bdlt::DatetimeTz>&
List::theDatetimeTzArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdlt::DatetimeTz> *>(elemData(index));
}

inline
const bsl::vector<bdlt::Date>& List::theDateArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdlt::Date> *>(elemData(index));
}

inline
const bsl::vector<bdlt::DateTz>&
List::theDateTzArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdlt::DateTz> *>(elemData(index));
}

inline
const bsl::vector<bdlt::Time>& List::theTimeArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdlt::Time> *>(elemData(index));
}

inline
const bsl::vector<bdlt::TimeTz>&
List::theTimeTzArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const bsl::vector<bdlt::TimeTz> *>(elemData(index));
}

inline
const Choice& List::theChoice(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const Choice *>(elemData(index));
}

inline
const ChoiceArray& List::theChoiceArray(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const ChoiceArray *>(elemData(index));
}

inline
const List& List::theList(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const List *>(elemData(index));
}

inline
const Table& List::theTable(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return *static_cast<const Table *>(elemData(index));
}

template <class STREAM>
inline
STREAM& List::bdexStreamOut(STREAM& stream, int version) const
{
    d_listImp.bdexStreamOutImp(
                          stream,
                          version,
                          ElemStreamOutAttrLookup<STREAM>::lookupTable());
    return stream;
}

inline
bsl::ostream& List::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    return d_listImp.print(stream, level, spacesPerLevel);
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlmxxx::operator==(const List& lhs, const List& rhs)
{
    return lhs.d_listImp == rhs.d_listImp;
}

inline
bool bdlmxxx::operator!=(const List& lhs, const List& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlmxxx::operator<<(bsl::ostream& stream, const List& list)
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
