// bdem_elemref.h                                                     -*-C++-*-
#ifndef INCLUDED_BDEM_ELEMREF
#define INCLUDED_BDEM_ELEMREF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide proxies to 'bdem' data elements.
//
//@CLASSES:
//  bdem_ConstElemRef: reference to non-modifiable 'bdem' element
//       bdem_ElemRef: reference to modifiable 'bdem' element
//
//@AUTHOR: John Lakos (jlakos)
//         Anthony Comerico (acomeric)
//         Bill Chapman (bchapman)
//
//@SEE_ALSO: bdem_elemtype, bdem_descriptor
//
//@DESCRIPTION: This component provides objects ("element references") that act
// as proxies for a 'bdem' data "element" and its "nullability information"
// (please see the section Element Reference Nullability below).  A 'bdem'
// element is an object of a type corresponding to one of the enumerated
// 'bdem_ElemType::Type' values.  An element reference provides non-modifiable
// or modifiable access to its referenced element value.  The two classes
// providing element references are:
//..
//  1. 'bdem_ConstElemRef' : Non-modifiable
//  2. 'bdem_ElemRef'      : Modifiable
//..
// An element reference is most commonly used to provide uniform access to an
// arbitrary element contained within a 'bdem_Row'; element references appear
// ubiquitously in the interfaces of 'bdem' containers and mechanisms.  This
// intended usage is not required, however.  An element reference can be
// constructed for any object of a recognized 'bdem' element type, provided the
// user also supplies an appropriate element descriptor.
//
// The methods of these classes perform no runtime error checks whatsoever
// (e.g., guarding against the dereferencing of a null pointer) for maximum
// efficiency.
//..
//                          ,------------.           Additional functionality
//                         ( bdem_ElemRef )          to enable modification
//                          `------------'           of non-const elements,
//                                 |                 but no additional data.
//                                 V
//                        ,-----------------.
//                       ( bdem_ConstElemRef )       No virtual functions.
//                        `-----------------'
//..
// The (base) class 'bdem_ConstElemRef' provides the subset of functionality
// applicable to both 'const' and non-'const' elements.  'bdem_ElemRef',
// derived from 'bdem_ConstElemRef', extends the capabilities but must be
// applied only to non-'const' element instances.  In other words, a
// 'bdem_ElemRef' *isA* 'bdem_ConstElemRef' that also provides modifiable
// access to the element to which it refers; hence, an instance of a modifiable
// element reference can stand in anywhere that the services of a constant
// element reference are required.
//
///Reference Semantics
///-------------------
// An element reference object provides (possibly modifiable) access to a
// particular element, using "reference semantics".  Reference semantics are
// analogous to the semantics of fundamental reference types and, in
// particular, differ significantly from the more common (within 'bde') value
// semantics.  An element reference object is bound to its referenced element
// at (reference) construction, and cannot ever be modified to refer to another
// element.  All "value-oriented" operations (e.g., 'operator==', 'makeNull',
// 'replaceValue'), *except* for copy construction, apply directly to the
// referenced *element*, and not to the reference object itself, as
// conventional (value) semantics might suggest.  Note, however, that since the
// conventional assignment operator might reasonably be expected to assign both
// the type and the value, but that that behavior is impractical (or even
// undesirable), 'operator=' has been suppressed from the (modifiable) element
// references.
//
// In further analogy to fundamental references, element references are
// provided in two forms, (logically) 'const' and (logically) non-'const'.
// The constant (base-class) version, 'bdem_ConstElemRef', omit all operations
// that would enable the client to modify the referenced element; the
// modifiable (derived) version, 'bdem_ElemRef', extend the behavior of their
// respective base classes to permit element modification.  Note that, as is
// the case for a fundamental reference, an element reference has no management
// responsibility for its referenced element, and, in particular, cannot
// determine whether or not the referenced *element* is valid.  It is the
// user's responsibility to keep track of the valid lifetime of a referenced
// element.  This is especially important when the element is contained in a
// 'bdem_List', a 'bdem_Table', or other container, because modification of the
// container may affect the validity of the reference.  Unless otherwise noted,
// the documented behavior of all methods provided by this component assume
// valid references.  In particular, 'isBound' is the only method whose
// behavior is defined when called on an element reference that refers to a
// 'bdem' element that is no longer valid.
//
///Semantics of 'const'
///--------------------
// A 'bdem_ElemRef' object (whether or not that instance is 'const') can be
// used to access or modify the (value of the) element to which it refers.  A
// 'bdem_ConstElemRef' object can be used to access, but *not* modify, its
// referenced element.  Unless the *element* is modifiable, a 'bdem_ElemRef'
// object cannot be created for it, and a 'bdem_ConstElemRef' or object must be
// used instead.
//
///Element Reference Types
///-----------------------
// An element reference is bound to a 'bdem' element by supplying, at
// construction, the address of the element and the address of a
// 'bdem_Descriptor'.  In what follows, the type of the 'bdem' element will be
// denoted as the "actual type" (of the referenced element), and the type
// indicated by the 'bdem_Descriptor' will be denoted as the "descriptor type"
// (of the referenced element).  To help ensure proper behavior of element
// reference objects, the descriptor type of the referenced element should
// correspond to the actual type of the referenced element.
//
///Types *Must* Match
/// - - - - - - - - -
// It is the user's responsibility, when assigning element values or accessing
// elements as a specific type, to insure that the element values are in fact
// of the proper type.  In particular, the methods of these classes perform
// *no* type checking at runtime; these methods will do as they are told and
// their behavior is undefined if types are not correct.
//
///'bdem' Null Values
///------------------
// The concept of null applies to each 'bdem' type.  In addition to the range
// of values in a given 'bdem' type's domain (e.g., '[ INT_MIN .. INT_MAX ]'
// for 'BDEM_INT'), each type has a null value.  When a 'bdem' element is null,
// it has an underlying (unique) designated unset value (or state) as indicated
// in the following table:
//..
//       'bdem' element type                  "unset" value or state
//  ------------------------------    --------------------------------------
//  BDEM_CHOICE
//  BDEM_CHOICE_ARRAY_ITEM            selector() < 0 && 0 == numSelections()
//
//  BDEM_CHOICE_ARRAY                 0 == length()  && 0 == numSelections()
//
//  BDEM_ROW                          All the elements in the row are "null"
//
//  BDEM_LIST                         0 == length()
//
//  BDEM_TABLE                        0 == numRows() && 0 == numColumns()
//
//  scalar (BDEM_INT, etc.)           bdetu_Unset<TYPE>::unsetValue()
//
//  vectors (BDEM_INT_ARRAY, etc.)    0 == size()
//..
// To illustrate, consider a 'BDEM_BOOL' element within a 'bdem_ElemRef'.  The
// element can be in one of three possible states:
//..
//  * null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//  * non-null with underlying value 'false'
//  * non-null with underlying value 'true'
//..
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdem_ElemRef', 'myElemref', that refers to
// an element of type 'BDEM_STRING':
//..
//  myElemref.theModifiableString() = "Hello";
//  assert(myElemref.isNonNull());
//..
// Making the element null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myElemref.makeNull();
//  assert(myElemref.isNull());
//  assert(myElemref.theString().empty());
//..
///Element Reference Nullability
///- - - - - - - - - - - - - - -
// An element reference keeps track of nullability via a bitmap and a
// (zero-based) offset into that map, which are bound to the element reference
// at construction.  We refer to this bitmap and offset as the "nullability
// information".  The nullness of an element is described by the its
// nullability information, i.e., the nth bit, where n equals offset to the bit
// indicating the nullness of the element referenced.  Specifically, if
// 'offset == 2' and the 3rd bit (zero-based, going from least-significant to
// most-significant) is 1, then the element referenced is null.  An element
// reference that is constructed without nullability information cannot be null
// (see the function-level documentation for more information concerning
// specific return values and default behaviors).  The offset supplied at
// construction is never changed, but the bit at the corresponding offset is
// mutable.  Note that when creating an element that has the bit at the offset
// within the bitmap set, the value of the data passed in must have "unset"
// value for its type (see the function-level documentation for the creators
// accepting nullability information for details).
//
///Usage
///-----
// A 'bdem_ConstElemRef' or 'bdem_ElemRef' is a proxy for a specific (typed)
// non-modifiable or modifiable element (respectively), which can be used to
// facilitate communication across intermediaries that do not care about the
// specific type or value of that element.  For example, suppose we have a
// heterogeneous collection of values whose types correspond to those
// identified by the enumeration 'bdem_ElemType::Type':
//..
//  // ...
//  const char        aChar      = 'a';
//  const int         anInt      = 10;
//  const int         anotherInt = 20;
//  const double      myDouble   = 12.5;
//  const bsl::string theString  = "Foo Bar";  // not at file scope!
//  const short       someShort  = -5;
//  const double      yourDouble = 34.75;
//..
// Suppose further that we want to create an 'isMember' function to determine
// whether a specified value (of the same type) is a member of that collection.
// We could create a linear sequence of element references as follows:
//..
//  const int LENGTH = 7;
//
//  bsls_ObjectBuffer<bdem_ConstElemRef> rawSpaceForList[LENGTH];
//
//  bdem_ConstElemRef *list = reinterpret_cast<bdem_ConstElemRef *>
//                                                          (rawSpaceForList);
//  new(list + 0) bdem_ConstElemRef(&aChar,      &charDescriptor);
//  new(list + 1) bdem_ConstElemRef(&anInt,      &intDescriptor);
//  new(list + 2) bdem_ConstElemRef(&anotherInt, &intDescriptor);
//  new(list + 3) bdem_ConstElemRef(&myDouble,   &doubleDescriptor);
//  new(list + 4) bdem_ConstElemRef(&theString,  &stringDescriptor);
//  new(list + 5) bdem_ConstElemRef(&someShort,  &shortDescriptor);
//  new(list + 6) bdem_ConstElemRef(&yourDouble, &doubleDescriptor);
//  // ...
//..
// Note that the creation of the various 'bdem_Descriptor' structures is not
// shown.  See 'bdem_descriptor' for more information on descriptors.  Also
// note that descriptor objects corresponding to the 20 "leaf" scalar and
// vector 'bdem_ElemType' enumerators can be obtained from the
// 'bdem_properties' component; similarly descriptor objects for 'BDEM_LIST',
// 'BDEM_TABLE', 'BDEM_ROW', 'BDEM_CHOICE', 'BDEM_CHOICE_ARRAY', and
// 'BDEM_CHOICE_ARRAY_ITEM' descriptors can be obtained from the
// 'bdem_listimp', 'bdem_tableimp', 'bdem_rowdata', 'bdem_choiceimp',
// 'bdem_choicearrayimp', and 'bdem_choiceheader' components, respectively.
//
// We can now write the 'isMember' function (at file scope) as follows:
//..
//  static bool isMember(const bdem_ConstElemRef  sequence[],
//                       int                      length,
//                       const bdem_ConstElemRef& element)
//      // Return 'true' if the value of the specified 'element' (with
//      // matching type) is contained in the specified 'sequence' of
//      // the specified 'length', and 'false' otherwise.
//  {
//      for (int i = 0; i < length; ++i) {
//          if (element == sequence[i]) {
//              return true;
//          }
//      }
//      return false;
//  }
//..
// As clients, we can invoke this function as follows:
//..
//  const int   V1 = 20;
//  const int   V2 = 15;
//  bsl::string v3 = "Foo";
//                             bdem_ConstElemRef cer3(&v3, &stringDescriptor);
//  bsl::string v4 = "Foo Bar";
//                             const bdem_ElemRef ER4(&v4, &stringDescriptor);
//
//  assert(1 == isMember(list, 7, bdem_ConstElemRef(&V1, &intDescriptor)));
//  assert(0 == isMember(list, 7, bdem_ConstElemRef(&V2, &intDescriptor)));
//  assert(0 == isMember(list, 7, cer3));
//  assert(1 == isMember(list, 5, ER4))
//  assert(0 == isMember(list, 4, ER4));
//..
// An excellent use of 'bdem_ElemRef', as well as 'bdem_ConstElemRef', can be
// found in the 'bdem_Row' component.
//
// Without 'bdem_ConstElemRef' (and the underlying 'bdem_Descriptor'), we would
// represent an element as a pointer to its data and its type:
//..
//  const void *DATA[] = {                     // Should NOT be at file scope.
//      &aChar,      &anInt,     &anotherInt,
//      &myDouble,   &theString, &someShort,   // String is not POD.
//      &yourDouble,
//  };
//
//  const bdem_ElemType::Type TYPES[] = {      // Could be at file scope.
//      bdem_ElemType::BDEM_CHAR,
//      bdem_ElemType::BDEM_INT,
//      bdem_ElemType::BDEM_DOUBLE,
//      bdem_ElemType::BDEM_STRING,
//      bdem_ElemType::BDEM_SHORT,
//      bdem_ElemType::BDEM_DOUBLE
//  };
//..
// Without element references, the 'isMember' function would be much more
// cumbersome to write.  In particular, 'isMember' would need first to check
// the type and, if it is the same as the element in question, cast both
// elements to that type before invoking the native ('operator==') comparison:
//..
//  static bool isMember(const void                *sequenceData[],
//                       const bdem_ElemType::Type  sequenceTypes[],
//                       int                        sequenceLength,
//                       const void                *dataValue,
//                       bdem_ElemType::Type        dataType)
//      // Return 'true' if the element value and type indicated by the
//      // specified 'dataValue' and 'dataType' respectively, match those of
//      // an element in the sequence indicated by the specified 'sequenceData'
//      // and 'sequenceTypes', each of the specified 'sequenceLength', and
//      // 'false' otherwise.
//  {
//      for (int i = 0; i < sequenceLength; ++i) {
//          if (dataType != sequenceTypes[i]) {
//              continue; // no need to compare values!
//          }
//          switch (dataType) {                 // Same type; must compare.
//            case bdem_ElemType::BDEM_CHAR: {
//              if (*static_cast<const char *>(dataValue) ==
//                  *static_cast<const char *>(sequenceData[i])) return true;
//            } break;
//            case bdem_ElemType::BDEM_SHORT: {
//              if (*static_cast<const short *>(dataValue) ==
//                  *static_cast<const short *>(sequenceData[i])) return true;
//            } break;
//            case bdem_ElemType::BDEM_INT: {
//              if (*static_cast<const int *>(dataValue) ==
//                  *static_cast<const int *>(sequenceData[i])) return true;
//            } break;
//            case bdem_ElemType::BDEM_INT64: {
//              typedef bsls_Types::Int64 Int64;
//              if (*static_cast<const Int64 *>(dataValue) ==
//                  *static_cast<const Int64 *>(sequenceData[i])) return true;
//            } break;
//            case bdem_ElemType::BDEM_FLOAT: {
//              if (*static_cast<const float *>(dataValue) ==
//                  *static_cast<const float *>(sequenceData[i])) return true;
//            } break;
//            case bdem_ElemType::BDEM_DOUBLE: {
//              if (*static_cast<const double *>(dataValue) ==
//                  *static_cast<const double *>(sequenceData[i])) return true;
//            } break;
//            case bdem_ElemType::BDEM_STRING: {
//              typedef bsl::string String;
//              if (*static_cast<const String *>(dataValue) ==
//                  *static_cast<const String *>(sequenceData[i])) return true;
//            } break;
//              // .
//              // .   (14 case statements omitted)
//              // .
//            case bdem_ElemType::BDEM_TABLE: {
//              typedef bdem_Table Table;
//              if (*static_cast<const Table *>(dataValue) ==
//                  *static_cast<const Table *>(sequenceData[i])) return true;
//            } break;
//            default: {
//              assert("Error: Enum Value out of range [0..21]" && 0); } } //
//              switch } // for
//      return false;
//  }
//..
// A client could invoke this second 'isMember' implementation as follows:
//..
//  assert(1 == isMember(DATA, TYPES, 7, &V1, bdem_ElemType::BDEM_INT));
//  assert(0 == isMember(DATA, TYPES, 7, &V2, bdem_ElemType::BDEM_INT));
//  assert(0 == isMember(DATA, TYPES, 7, &v3, bdem_ElemType::BDEM_STRING));
//  assert(1 == isMember(DATA, TYPES, 5, &v4, bdem_ElemType::BDEM_STRING));
//  assert(0 == isMember(DATA, TYPES, 4, &v4, bdem_ElemType::BDEM_STRING));
//..
// Element references are valuable because the alternative, in addition to
// being bulky and error-prone, forces not only a *link-time*, but also a
// *COMPILE-TIME*, physical dependency of 'isMember' on *SPECIFIC*
// *IMPLEMENTATIONS* of *ALL* 22 types -- even though many implementations
// (e.g., of datetime, date, time, all ten arrays, list, and table) are not
// needed by the client of this code.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_DESCRIPTOR
#include <bdem_descriptor.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEM_PROPERTIES
#include <bdem_properties.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bdet_DatetimeTz;
class bdet_Datetime;
class bdet_DateTz;
class bdet_Date;
class bdet_TimeTz;
class bdet_Time;
class bdem_ChoiceArrayItem;
class bdem_Choice;
class bdem_ChoiceArray;
class bdem_Row;
class bdem_List;
class bdem_Table;
class bdem_ElemRef;

                        // =======================
                        // class bdem_ConstElemRef
                        // =======================

class bdem_ConstElemRef {
    // This class represents a reference to a non-modifiable 'bdem' element
    // object.  Accessing the element is guaranteed to be an efficient,
    // constant-time operation.  Instances of this class can be created from
    // both modifiable and non-modifiable instances of 'bdem' elements.  Use of
    // a reference when its referenced element is no longer valid, apart from
    // the 'isBound' method, will result in undefined behavior.  In particular,
    // if the referenced element is contained within a 'bdem_List' or
    // 'bdem_Table', the element may become invalid if the container is
    // (structurally) modified, i.e., if the number or types of the elements in
    // the container are modified.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // destructor, all instance methods of this object are 'const' and apply
    // not to this object, but rather to the "element" and "nullability
    // information" (pointer only, not offset) to which this object refers.
    // Note also that, unlike the derived 'bdem_ElemRef' class (also defined
    // within this component), none of the methods within this base class
    // enable modifiable access to the indicated element or nullability
    // information, thus ensuring 'const' correctness.

    // DATA
    union {
        const void *d_constData_p; // data element (held, not owned)
        void       *d_data_p;      // for use by 'bdem_ElemRef', never to be
                                   // used in this class (held, not owned)
    };

    const bdem_Descriptor *d_descriptor_p; // elem attributes (held, not owned)

    union {
        const int *d_constNullnessWord_p; // pointer to the nullness word that
        int       *d_nullnessWord_p;      // contains the nullability
                                          // information (held, not owned)
    };

    int d_nullnessBitOffset; // offset to the specific bit in the nullness word
                             // that holds the nullability information

    // FRIENDS
    friend class bdem_ElemRef;  // can access the data members as if they
                                // were protected, but no one else can

    friend bool operator==(const bdem_ConstElemRef&, const bdem_ConstElemRef&);

    // NOT IMPLEMENTED
    bdem_ConstElemRef& operator=(const bdem_ConstElemRef&);

  public:
    // CLASS METHODS
    static bdem_ConstElemRef unboundElemRef();
        // Return a reference that is not bound to any 'bdem' element.  Note
        // that an unbound reference may be used only to invoke a selected set
        // of accessors (see the function-level documentation for details), and
        // with the equality and streaming operators.  Also note that the
        // 'type' method returns 'bdem_ElemType::BDEM_VOID' for an unbound
        // reference.

    // CREATORS
    bdem_ConstElemRef(const void *data, const bdem_Descriptor *descriptor);
        // Create a reference to the specified non-modifiable and non-nullable
        // 'data' element that is characterized by the specified non-modifiable
        // 'descriptor'.  The behavior is undefined unless the type of 'data'
        // corresponds to 'descriptor', or else 'descriptor' corresponds to
        // 'bdem_ElemType::BDEM_VOID' (indicating an unbound reference).  Note
        // that if the referenced element is contained within a 'bdem_List' or
        // 'bdem_Table', then the behavior of this reference is undefined if it
        // is used after a modification of the number or types of the elements
        // in the container.

    bdem_ConstElemRef(const void            *data,
                      const bdem_Descriptor *descriptor,
                      const int             *nullnessWord,
                      int                    nullnessBitOffset);
        // Create a reference to the specified non-modifiable and nullable
        // 'data' element that is characterized by the specified non-modifiable
        // 'descriptor' and whose nullness bit is at the specified 0-based
        // 'nullnessBitOffset' in the specified 'nullnessWord' (indexed from
        // least-significant bit to most-significant bit).  The behavior is
        // undefined unless the type of 'data' corresponds to 'descriptor' or
        // else 'descriptor' corresponds to 'bdem_ElemType::BDEM_VOID'
        // (indicating an unbound reference), 'nullnessWord' is non-null, and
        // '0 <= nullnessBitOffset < sizeof(int) * 8'.  Note that if the
        // referenced element is contained within a 'bdem_List' or
        // 'bdem_Table', then the behavior of this reference is undefined if it
        // is used after a modification of the number or types of the elements
        // in the container.

    bdem_ConstElemRef(const bdem_ConstElemRef& original);
        // Create a reference to the non-modifiable element indicated by the
        // specified 'original' element reference.  The element referenced by
        // the new object is nullable if and only if 'original.isNullable()'.
        // Note that if the referenced element is contained within a
        // 'bdem_List' or 'bdem_Table', then the behavior of this reference is
        // undefined if it is used after a modification of the number or types
        // of the elements in the container.

    //! ~bdem_ConstElemRef();
        // Destroy this element reference.  Note that this method has no
        // effect on the referenced element.  Also note that this method's
        // definition is compiler generated.

    // ACCESSORS
    const bool& theBool() const;
    const char& theChar() const;
    const short& theShort() const;
    const int& theInt() const;
    const bsls_Types::Int64& theInt64() const;
    const float& theFloat() const;
    const double& theDouble() const;
    const bsl::string& theString() const;
    const bdet_Datetime& theDatetime() const;
    const bdet_DatetimeTz& theDatetimeTz() const;
    const bdet_Date& theDate() const;
    const bdet_DateTz& theDateTz() const;
    const bdet_Time& theTime() const;
    const bdet_TimeTz& theTimeTz() const;
    const bsl::vector<bool>& theBoolArray() const;
    const bsl::vector<char>& theCharArray() const;
    const bsl::vector<short>& theShortArray() const;
    const bsl::vector<int>& theIntArray() const;
    const bsl::vector<bsls_Types::Int64>& theInt64Array() const;
    const bsl::vector<float>& theFloatArray() const;
    const bsl::vector<double>& theDoubleArray() const;
    const bsl::vector<bsl::string>& theStringArray() const;
    const bsl::vector<bdet_Datetime>& theDatetimeArray() const;
    const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray() const;
    const bsl::vector<bdet_Date>& theDateArray() const;
    const bsl::vector<bdet_DateTz>& theDateTzArray() const;
    const bsl::vector<bdet_Time>& theTimeArray() const;
    const bsl::vector<bdet_TimeTz>& theTimeTzArray() const;
    const bdem_Choice& theChoice() const;
    const bdem_ChoiceArray& theChoiceArray() const;
    const bdem_ChoiceArrayItem& theChoiceArrayItem() const;
    const bdem_List& theList() const;
    const bdem_Row& theRow() const;
    const bdem_Table& theTable() const;
        // Return a reference to the non-modifiable element referenced by this
        // object.  The nullness of the element is not affected.  The behavior
        // is undefined unless the return type of the method used matches the
        // actual type and descriptor type of the element referenced by this
        // object.

    bdem_ElemType::Type type() const;
        // Return the type of the 'bdem' element referenced by this object, or
        // 'bdem_ElemType::BDEM_VOID' if this reference is unbound.

    const bdem_Descriptor *descriptor() const;
        // Return the address of the non-modifiable type-specific attributes of
        // this object.  Note that this method should *NOT* be called; it is
        // for *internal* use only.  Also note that if this reference is
        // unbound, the descriptor for 'bdem_ElemType::BDEM_VOID' is returned.

    const void *data() const;
        // Return the address of the non-modifiable 'bdem' element referenced
        // by this object, or 0 if this reference is unbound.  The nullness of
        // the element is not affected.  Note that this method should *NOT* be
        // called; it is for *internal* use only.

    bool isBound() const;
        // Return 'true' if this element reference refers to a (possibly
        // invalid) 'bdem' element, and 'false' otherwise.

    bool isNull() const;
        // Return 'true' if the element referenced by this object is null, and
        // 'false' otherwise.  The behavior is undefined unless this reference
        // is bound.  Note that if 'false == isNullable()', 'false' is
        // returned.

    bool isNonNull() const;
        // Return 'true' if the element referenced by this object is non-null,
        // and 'false' otherwise.  The behavior is undefined unless this
        // reference is bound.  Note that if 'false == isNullable()', 'true' is
        // returned.

    bool isNullable() const;
        // Return 'true' if the element referenced by this object is nullable,
        // and 'false' otherwise.  An element is nullable if a 'nullnessWord'
        // and 'nullnessBitOffset' were provided at construction.  The behavior
        // is undefined unless this reference is bound.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format the value of the element referenced by this object to the
        // specified output 'stream' at the (absolute value of) the optionally
        // specified indentation 'level' and return a reference to 'stream'.
        // If 'level' is specified, optionally specify 'spacesPerLevel', the
        // non-negative number of spaces per indentation level for this and all
        // of its nested objects.  Making 'level' negative suppresses
        // indentation for the first line only.  If 'stream' is valid, then the
        // behavior is undefined unless this reference is unbound, or the
        // descriptor type of the referenced element corresponds to the actual
        // type of the referenced element.  Note that if 'stream' is not valid,
        // then this operation has no effect.
};

// FREE OPERATORS
bool operator==(const bdem_ConstElemRef& lhs, const bdem_ConstElemRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' element references are
    // bound to 'bdem' elements having the same type, same nullability and the
    // same value (as defined by 'operator==' for that type) or if both the
    // 'lhs' and 'rhs' element references are unbound or if both the 'lhs' and
    // 'rhs' element references are null, and 'false' otherwise.  If 'lhs' and
    // 'rhs' are both bound, and their respective descriptor types match, then
    // the behavior is undefined unless their respective actual types
    // correspond to their common descriptor type.

bool operator!=(const bdem_ConstElemRef& lhs, const bdem_ConstElemRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' element references are
    // bound to 'bdem' elements having different types, different nullability
    // or different values, or if exactly one of the 'lhs' or 'rhs' element
    // references is unbound, or if exactly one of the 'lhs' or 'rhs' element
    // references is null, and 'false' otherwise.  If 'lhs' and 'rhs' are both
    // bound, and their respective descriptor types match, then the behavior is
    // undefined unless their respective actual types correspond to their
    // common descriptor type.

bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ConstElemRef& rhs);
    // Format the element referenced by the specified 'rhs' element reference
    // to the specified output 'stream' in some reasonable (multi-line) format.
    // If 'stream' is valid, then the behavior is undefined unless 'rhs' is
    // unbound, or the descriptor type and the actual type of the element
    // referenced by 'rhs' correspond.  Note that if 'stream' is not valid,
    // then this operation has no effect.

                        // ==================
                        // class bdem_ElemRef
                        // ==================

class bdem_ElemRef : public bdem_ConstElemRef {
    // This class represents a reference to a modifiable 'bdem' element object.
    // Accessing the element is guaranteed to be an efficient, constant-time
    // operation.  Instances of this class can be created only from modifiable
    // instances of 'bdem' elements.  Use of a reference when its referenced
    // element is no longer valid, apart from the 'isBound' method, will result
    // in undefined behavior.  In particular, if the referenced element is
    // contained within a 'bdem_List' or 'bdem_Table', the element may become
    // invalid if the container is (structurally) modified, i.e., if the number
    // or types of the elements in the container are modified.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // destructor, all instance methods of this object are 'const' and apply
    // not to this object, but rather to the "element" and "nullability
    // information" (pointer only, not nullnessBitOffset) to which this object
    // refers.
    //
    // Note: all data resides in the base class.

    // NOT IMPLEMENTED
    bdem_ElemRef& operator=(const bdem_ElemRef&);

  private:
    // PRIVATE ACCESSORS
    void setNullnessBit() const;
        // Set the nullness bit corresponding to the element referenced by this
        // object to 1 if 'isNullable' returns 'true'.  This method has no
        // effect if 'isNullable' returns 'false', or if this reference is
        // unbound.

    void clearNullnessBit() const;
        // Set the nullness bit corresponding to the element referenced by this
        // object to 0 if 'isNullable' returns 'true'.  This method has no
        // effect if 'isNullable' returns 'false', or if this reference is
        // unbound.

  public:
    // CLASS METHODS
    static bdem_ElemRef unboundElemRef();
        // Return a reference that is not bound to any 'bdem' element.  Note
        // that an unbound reference may be used only to invoke a selected set
        // of accessors (see the function-level documentation for details), and
        // with the equality and streaming operators.  Also note that the
        // 'type' method returns 'bdem_ElemType::BDEM_VOID' for an unbound
        // reference.

    // CREATORS
    bdem_ElemRef(void *data, const bdem_Descriptor *descriptor);
        // Create a reference to the specified modifiable and non-nullable
        // 'data' element that is characterized by the specified non-modifiable
        // 'descriptor'.  The behavior is undefined unless the type of 'data'
        // corresponds to 'descriptor', or else 'descriptor' corresponds to
        // 'bdem_ElemType::BDEM_VOID' (indicating an unbound reference).  Note
        // that if the referenced element is contained within a 'bdem_List' or
        // 'bdem_Table', then the behavior of this reference is undefined if it
        // is used after a modification of the number or types of the elements
        // in the container.

    bdem_ElemRef(void                  *data,
                 const bdem_Descriptor *descriptor,
                 int                   *nullnessWord,
                 int                    nullnessBitOffset);
        // Create a reference to the specified modifiable and nullable 'data'
        // element that is characterized by the specified non-modifiable
        // 'descriptor' and whose nullness bit is at the specified 0-based
        // 'nullnessBitOffset' in the specified 'nullnessWord' (indexed from
        // least-significant bit to most-significant bit).  The behavior is
        // undefined unless the type of 'data' corresponds to 'descriptor' or
        // else 'descriptor' corresponds to 'bdem_ElemType::BDEM_VOID'
        // (indicating an unbound reference), 'nullnessWord' is non-null, and
        // '0 <= nullnessBitOffset < sizeof(int) * 8'.  Note that if the
        // referenced element is contained within a 'bdem_List' or
        // 'bdem_Table', then the behavior of this reference is undefined if it
        // is used after a modification of the number or types of the elements
        // in the container.

    bdem_ElemRef(const bdem_ElemRef& original);
        // Create a reference to the modifiable element indicated by the
        // specified 'original' element reference.  The element referenced by
        // the new object is nullable if and only if 'original.isNullable()'.
        // Note that if the referenced element is contained within a
        // 'bdem_List' or 'bdem_Table', then the behavior of this reference is
        // undefined if it is used after a modification of the number or types
        // of the elements in the container.

    //! ~bdem_ElemRef();
        // Destroy this element reference.  Note that this method has no effect
        // on the referenced element.  Also note that this method's definition
        // is compiler generated.

    // REFERENCED-VALUE MANIPULATORS
    void makeNull() const;
        // Set the element referenced by this object to the unset value
        // appropriate for its type; if the element is nullable, then also make
        // the element null.  The behavior is undefined unless this reference
        // is bound and the descriptor type of the referenced element
        // corresponds to the actual type of the referenced element.

    bool& theModifiableBool() const;
    char& theModifiableChar() const;
    short& theModifiableShort() const;
    int& theModifiableInt() const;
    bsls_Types::Int64& theModifiableInt64() const;
    float& theModifiableFloat() const;
    double& theModifiableDouble() const;
    bsl::string& theModifiableString() const;
    bdet_Datetime& theModifiableDatetime() const;
    bdet_DatetimeTz& theModifiableDatetimeTz() const;
    bdet_Date& theModifiableDate() const;
    bdet_DateTz& theModifiableDateTz() const;
    bdet_Time& theModifiableTime() const;
    bdet_TimeTz& theModifiableTimeTz() const;
    bsl::vector<bool>& theModifiableBoolArray() const;
    bsl::vector<char>& theModifiableCharArray() const;
    bsl::vector<short>& theModifiableShortArray() const;
    bsl::vector<int>& theModifiableIntArray() const;
    bsl::vector<bsls_Types::Int64>& theModifiableInt64Array() const;
    bsl::vector<float>& theModifiableFloatArray() const;
    bsl::vector<double>& theModifiableDoubleArray() const;
    bsl::vector<bsl::string>& theModifiableStringArray() const;
    bsl::vector<bdet_Datetime>& theModifiableDatetimeArray() const;
    bsl::vector<bdet_DatetimeTz>& theModifiableDatetimeTzArray() const;
    bsl::vector<bdet_Date>& theModifiableDateArray() const;
    bsl::vector<bdet_DateTz>& theModifiableDateTzArray() const;
    bsl::vector<bdet_Time>& theModifiableTimeArray() const;
    bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray() const;
    bdem_Choice& theModifiableChoice() const;
    bdem_ChoiceArray& theModifiableChoiceArray() const;
    bdem_ChoiceArrayItem& theModifiableChoiceArrayItem() const;
    bdem_List& theModifiableList() const;
    bdem_Row& theModifiableRow() const;
    bdem_Table& theModifiableTable() const;
        // Return a reference to the modifiable element referenced by this
        // object.  If the referenced element is null, it is made non-null
        // before returning, but its value is not otherwise modified.  The
        // behavior is undefined unless the return type of the method used
        // matches the actual type and descriptor type of the element
        // referenced by this object.  Note that unless there is an intention
        // of modifying the element, the corresponding 'theTYPE' method should
        // be used instead.

    void replaceValue(const bdem_ConstElemRef& referenceObject) const;
        // Replace the value of the element referenced by this object with the
        // value of the element referenced by the specified 'referenceObject'.
        // If this reference is not nullable (as defined by 'isNullable') but
        // 'referenceObject' is null, then the value of the element referenced
        // by this object is set equal to the "unset" value corresponding to
        // the element's type.  The behavior is undefined unless this reference
        // and 'referenceObject' are both bound, their respective descriptor
        // types match, and their respective actual types correspond to their
        // common descriptor type.

    void *data() const;
        // Return the address of the modifiable 'bdem' element referenced by
        // this object, or 0 if this reference is unbound.  If the referenced
        // element is null, it is made non-null before returning, but its value
        // is not otherwise modified.  Note that this method should *NOT* be
        // called; it is for *internal* use only.

    void *dataRaw() const;
        // Return the address of the modifiable 'bdem' element referenced by
        // this object, or 0 if this reference is unbound.  The nullness of the
        // element is not affected.  Note that this method should *NOT* be
        // called; it is for *internal* use only.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------
                        // class bdem_ConstElemRef
                        // -----------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
bool bdem_ConstElemRef::isBound() const
{
    BSLS_ASSERT_SAFE(d_descriptor_p);

    // TBD the check that 'd_descriptor_p' is non-null is there for
    // backward compatibility (e.g., 'bdem_ConstElemRef(0, 0)').

    return d_descriptor_p
        && bdem_ElemType::BDEM_VOID != d_descriptor_p->d_elemEnum;
}

// CLASS METHODS
inline
bdem_ConstElemRef bdem_ConstElemRef::unboundElemRef()
{
    return bdem_ConstElemRef(0, &bdem_Properties::d_voidAttr);
}

// CREATORS
inline
bdem_ConstElemRef::bdem_ConstElemRef(const void            *data,
                                     const bdem_Descriptor *descriptor)
: d_constData_p(data)
, d_descriptor_p(descriptor)
, d_constNullnessWord_p(0)
, d_nullnessBitOffset(0)
{
    BSLS_ASSERT_SAFE(descriptor
                  && (data
                   || bdem_ElemType::BDEM_VOID == d_descriptor_p->d_elemEnum));
}

inline
bdem_ConstElemRef::bdem_ConstElemRef(const void            *data,
                                     const bdem_Descriptor *descriptor,
                                     const int             *nullnessWord,
                                     int                    nullnessBitOffset)
: d_constData_p(data)
, d_descriptor_p(descriptor)
, d_constNullnessWord_p(nullnessWord)
, d_nullnessBitOffset(nullnessBitOffset)
{
    BSLS_ASSERT_SAFE(descriptor
                  && (data
                   || bdem_ElemType::BDEM_VOID == d_descriptor_p->d_elemEnum));
    BSLS_ASSERT_SAFE(nullnessWord);
    BSLS_ASSERT_SAFE((unsigned)nullnessBitOffset < sizeof(int) * 8);
}

inline
bdem_ConstElemRef::bdem_ConstElemRef(const bdem_ConstElemRef& original)
: d_constData_p(original.d_constData_p)
, d_descriptor_p(original.d_descriptor_p)
, d_nullnessWord_p(original.d_nullnessWord_p)
, d_nullnessBitOffset(original.d_nullnessBitOffset)
{
}

// ACCESSORS
inline
const bool& bdem_ConstElemRef::theBool() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == d_descriptor_p->d_elemEnum);

    return *(const bool *)d_constData_p;
}

inline
const char& bdem_ConstElemRef::theChar() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == d_descriptor_p->d_elemEnum);

    return *(const char *)d_constData_p;
}

inline
const short& bdem_ConstElemRef::theShort() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == d_descriptor_p->d_elemEnum);

    return *(const short *)d_constData_p;
}

inline
const int& bdem_ConstElemRef::theInt() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == d_descriptor_p->d_elemEnum);

    return *(const int *)d_constData_p;
}

inline
const bsls_Types::Int64& bdem_ConstElemRef::theInt64() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == d_descriptor_p->d_elemEnum);

    return *(const bsls_Types::Int64 *)d_constData_p;
}

inline
const float& bdem_ConstElemRef::theFloat() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == d_descriptor_p->d_elemEnum);

    return *(const float *)d_constData_p;
}

inline
const double& bdem_ConstElemRef::theDouble() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == d_descriptor_p->d_elemEnum);

    return *(const double *)d_constData_p;
}

inline
const bsl::string& bdem_ConstElemRef::theString() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == d_descriptor_p->d_elemEnum);

    return *(const bsl::string *)d_constData_p;
}

inline
const bdet_Datetime& bdem_ConstElemRef::theDatetime() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_DATETIME == d_descriptor_p->d_elemEnum);

    return *(const bdet_Datetime *)d_constData_p;
}

inline
const bdet_DatetimeTz& bdem_ConstElemRef::theDatetimeTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_DATETIMETZ == d_descriptor_p->d_elemEnum);

    return *(const bdet_DatetimeTz *)d_constData_p;
}

inline
const bdet_Date& bdem_ConstElemRef::theDate() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == d_descriptor_p->d_elemEnum);

    return *(const bdet_Date *)d_constData_p;
}

inline
const bdet_DateTz& bdem_ConstElemRef::theDateTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == d_descriptor_p->d_elemEnum);

    return *(const bdet_DateTz *)d_constData_p;
}

inline
const bdet_Time& bdem_ConstElemRef::theTime() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == d_descriptor_p->d_elemEnum);

    return *(const bdet_Time *)d_constData_p;
}

inline
const bdet_TimeTz& bdem_ConstElemRef::theTimeTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == d_descriptor_p->d_elemEnum);

    return *(const bdet_TimeTz *)d_constData_p;
}

inline
const bsl::vector<bool>& bdem_ConstElemRef::theBoolArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_BOOL_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bool> *)d_constData_p;
}

inline
const bsl::vector<char>& bdem_ConstElemRef::theCharArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_CHAR_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<char> *)d_constData_p;
}

inline
const bsl::vector<short>& bdem_ConstElemRef::theShortArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                bdem_ElemType::BDEM_SHORT_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<short> *)d_constData_p;
}

inline
const bsl::vector<int>& bdem_ConstElemRef::theIntArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                  bdem_ElemType::BDEM_INT_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<int> *)d_constData_p;
}

inline
const bsl::vector<bsls_Types::Int64>& bdem_ConstElemRef::theInt64Array() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                bdem_ElemType::BDEM_INT64_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bsls_Types::Int64> *)d_constData_p;
}

inline
const bsl::vector<float>& bdem_ConstElemRef::theFloatArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                bdem_ElemType::BDEM_FLOAT_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<float> *)d_constData_p;
}

inline
const bsl::vector<double>& bdem_ConstElemRef::theDoubleArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_DOUBLE_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<double> *)d_constData_p;
}

inline
const bsl::vector<bsl::string>& bdem_ConstElemRef::theStringArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_STRING_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bsl::string> *)d_constData_p;
}

inline
const bsl::vector<bdet_Datetime>& bdem_ConstElemRef::theDatetimeArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
             bdem_ElemType::BDEM_DATETIME_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdet_Datetime> *)d_constData_p;
}

inline
const bsl::vector<bdet_DatetimeTz>&
bdem_ConstElemRef::theDatetimeTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
           bdem_ElemType::BDEM_DATETIMETZ_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdet_DatetimeTz> *)d_constData_p;
}

inline
const bsl::vector<bdet_Date>& bdem_ConstElemRef::theDateArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_DATE_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdet_Date> *)d_constData_p;
}

inline
const bsl::vector<bdet_DateTz>& bdem_ConstElemRef::theDateTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_DATETZ_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdet_DateTz> *)d_constData_p;
}

inline
const bsl::vector<bdet_Time>& bdem_ConstElemRef::theTimeArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_TIME_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdet_Time> *)d_constData_p;
}

inline
const bsl::vector<bdet_TimeTz>& bdem_ConstElemRef::theTimeTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_TIMETZ_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdet_TimeTz> *)d_constData_p;
}

inline
const bdem_Row& bdem_ConstElemRef::theRow() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_ROW == d_descriptor_p->d_elemEnum);

    return *(const bdem_Row *)d_constData_p;
}

inline
const bdem_List& bdem_ConstElemRef::theList() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == d_descriptor_p->d_elemEnum);

    return *(const bdem_List *)d_constData_p;
}

inline
const bdem_Table& bdem_ConstElemRef::theTable() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == d_descriptor_p->d_elemEnum);

    return *(const bdem_Table *)d_constData_p;
}

inline
const bdem_ChoiceArrayItem& bdem_ConstElemRef::theChoiceArrayItem() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
          bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM == d_descriptor_p->d_elemEnum);

    return *(const bdem_ChoiceArrayItem *)d_constData_p;
}

inline
const bdem_Choice& bdem_ConstElemRef::theChoice() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == d_descriptor_p->d_elemEnum);

    return *(const bdem_Choice *)d_constData_p;
}

inline
const bdem_ChoiceArray& bdem_ConstElemRef::theChoiceArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_CHOICE_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bdem_ChoiceArray *)d_constData_p;
}

inline
bdem_ElemType::Type bdem_ConstElemRef::type() const
{
    BSLS_ASSERT_SAFE(d_descriptor_p);

    return (bdem_ElemType::Type)d_descriptor_p->d_elemEnum;
}

inline
const bdem_Descriptor *bdem_ConstElemRef::descriptor() const
{
    BSLS_ASSERT_SAFE(d_descriptor_p);

    return d_descriptor_p;
}

inline
const void *bdem_ConstElemRef::data() const
{
    return d_constData_p;
}

inline
bool bdem_ConstElemRef::isNull() const
{
    BSLS_ASSERT_SAFE(isBound());

    if (!d_constNullnessWord_p) {
        return false;
    }

    return (1 << d_nullnessBitOffset) & *d_constNullnessWord_p;
}

inline
bool bdem_ConstElemRef::isNonNull() const
{
    BSLS_ASSERT_SAFE(isBound());

    return !isNull();
}

inline
bool bdem_ConstElemRef::isNullable() const
{
    BSLS_ASSERT_SAFE(isBound());

    return 0 != d_constNullnessWord_p;
}

// FREE OPERATORS

inline
bool operator!=(const bdem_ConstElemRef& lhs, const bdem_ConstElemRef& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdem_ConstElemRef& rhs)
{
    return rhs.print(stream, 0 , -1);
}

                        // ------------------
                        // class bdem_ElemRef
                        // ------------------

// PRIVATE ACCESSORS
inline
void bdem_ElemRef::setNullnessBit() const
{
    if (d_constNullnessWord_p) {
        *d_nullnessWord_p |= (1 << d_nullnessBitOffset);
    }
}

inline
void bdem_ElemRef::clearNullnessBit() const
{
    if (d_constNullnessWord_p) {
        *d_nullnessWord_p &= ~(1 << d_nullnessBitOffset);
    }
}

// CLASS METHODS
inline
bdem_ElemRef bdem_ElemRef::unboundElemRef()
{
    return bdem_ElemRef(0, &bdem_Properties::d_voidAttr);
}

// CREATORS
inline
bdem_ElemRef::bdem_ElemRef(void *data, const bdem_Descriptor *descriptor)
: bdem_ConstElemRef(data, descriptor)
{
}

inline
bdem_ElemRef::bdem_ElemRef(void                  *data,
                           const bdem_Descriptor *descriptor,
                           int                   *nullnessWord,
                           int                    nullnessBitOffset)
: bdem_ConstElemRef(data, descriptor, nullnessWord, nullnessBitOffset)
{
}

inline
bdem_ElemRef::bdem_ElemRef(const bdem_ElemRef& original)
: bdem_ConstElemRef(original)
{
}

// REFERENCED-VALUE MANIPULATORS
inline
void bdem_ElemRef::makeNull() const
{
    BSLS_ASSERT_SAFE(isBound());

    if (isNullable()) {
        setNullnessBit();
    }
    d_descriptor_p->makeUnset(d_data_p);
}

inline
bool& bdem_ElemRef::theModifiableBool() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_BOOL == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bool *)d_data_p;
}

inline
char& bdem_ElemRef::theModifiableChar() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHAR == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(char *)d_data_p;
}

inline
short& bdem_ElemRef::theModifiableShort() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_SHORT == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(short *)d_data_p;
}

inline
int& bdem_ElemRef::theModifiableInt() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(int *)d_data_p;
}

inline
bsls_Types::Int64& bdem_ElemRef::theModifiableInt64() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_INT64 == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsls_Types::Int64 *)d_data_p;
}

inline
float& bdem_ElemRef::theModifiableFloat() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_FLOAT == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(float *)d_data_p;
}

inline
double& bdem_ElemRef::theModifiableDouble() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DOUBLE == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(double *)d_data_p;
}

inline
bsl::string& bdem_ElemRef::theModifiableString() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_STRING == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::string *)d_data_p;
}

inline
bdet_Datetime& bdem_ElemRef::theModifiableDatetime() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                   bdem_ElemType::BDEM_DATETIME == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdet_Datetime *)d_data_p;
}

inline
bdet_DatetimeTz& bdem_ElemRef::theModifiableDatetimeTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_DATETIMETZ == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdet_DatetimeTz *)d_data_p;
}

inline
bdet_Date& bdem_ElemRef::theModifiableDate() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATE == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdet_Date *)d_data_p;
}

inline
bdet_DateTz& bdem_ElemRef::theModifiableDateTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_DATETZ == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdet_DateTz *)d_data_p;
}

inline
bdet_Time& bdem_ElemRef::theModifiableTime() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIME == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdet_Time *)d_data_p;
}

inline
bdet_TimeTz& bdem_ElemRef::theModifiableTimeTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TIMETZ == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdet_TimeTz *)d_data_p;
}

inline
bsl::vector<bool>& bdem_ElemRef::theModifiableBoolArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_BOOL_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bool> *)d_data_p;
}

inline
bsl::vector<char>& bdem_ElemRef::theModifiableCharArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_CHAR_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<char> *)d_data_p;
}

inline
bsl::vector<short>& bdem_ElemRef::theModifiableShortArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                bdem_ElemType::BDEM_SHORT_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<short> *)d_data_p;
}

inline
bsl::vector<int>& bdem_ElemRef::theModifiableIntArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                  bdem_ElemType::BDEM_INT_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<int> *)d_data_p;
}

inline
bsl::vector<bsls_Types::Int64>& bdem_ElemRef::theModifiableInt64Array() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                bdem_ElemType::BDEM_INT64_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bsls_Types::Int64> *)d_data_p;
}

inline
bsl::vector<float>& bdem_ElemRef::theModifiableFloatArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                bdem_ElemType::BDEM_FLOAT_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<float> *)d_data_p;
}

inline
bsl::vector<double>& bdem_ElemRef::theModifiableDoubleArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_DOUBLE_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<double> *)d_data_p;
}

inline
bsl::vector<bsl::string>& bdem_ElemRef::theModifiableStringArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_STRING_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bsl::string> *)d_data_p;
}

inline
bsl::vector<bdet_Datetime>& bdem_ElemRef::theModifiableDatetimeArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
             bdem_ElemType::BDEM_DATETIME_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdet_Datetime> *)d_data_p;
}

inline
bsl::vector<bdet_DatetimeTz>&
                             bdem_ElemRef::theModifiableDatetimeTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
           bdem_ElemType::BDEM_DATETIMETZ_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdet_DatetimeTz> *)d_data_p;
}

inline
bsl::vector<bdet_Date>& bdem_ElemRef::theModifiableDateArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_DATE_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdet_Date> *)d_data_p;
}

inline
bsl::vector<bdet_DateTz>& bdem_ElemRef::theModifiableDateTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_DATETZ_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdet_DateTz> *)d_data_p;
}

inline
bsl::vector<bdet_Time>& bdem_ElemRef::theModifiableTimeArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 bdem_ElemType::BDEM_TIME_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdet_Time> *)d_data_p;
}

inline
bsl::vector<bdet_TimeTz>& bdem_ElemRef::theModifiableTimeTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_TIMETZ_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdet_TimeTz> *)d_data_p;
}

inline
bdem_Row& bdem_ElemRef::theModifiableRow() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_ROW == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdem_Row *)d_data_p;
}

inline
bdem_List& bdem_ElemRef::theModifiableList() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_LIST == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdem_List *)d_data_p;
}

inline
bdem_Table& bdem_ElemRef::theModifiableTable() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_TABLE == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdem_Table *)d_data_p;
}

inline
bdem_ChoiceArrayItem& bdem_ElemRef::theModifiableChoiceArrayItem() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
          bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdem_ChoiceArrayItem *)d_data_p;
}

inline
bdem_Choice& bdem_ElemRef::theModifiableChoice() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(bdem_ElemType::BDEM_CHOICE == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdem_Choice *)d_data_p;
}

inline
bdem_ChoiceArray& bdem_ElemRef::theModifiableChoiceArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               bdem_ElemType::BDEM_CHOICE_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdem_ChoiceArray *)d_data_p;
}

// ACCESSORS
inline
void *bdem_ElemRef::data() const
{
    clearNullnessBit();
    return d_data_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
