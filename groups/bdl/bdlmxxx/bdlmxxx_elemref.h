// bdlmxxx_elemref.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLMXXX_ELEMREF
#define INCLUDED_BDLMXXX_ELEMREF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide proxies to 'bdem' data elements.
//
//@CLASSES:
//  bdlmxxx::ConstElemRef: reference to non-modifiable 'bdem' element
//       bdlmxxx::ElemRef: reference to modifiable 'bdem' element
//
//@AUTHOR: John Lakos (jlakos)
//         Anthony Comerico (acomeric)
//         Bill Chapman (bchapman)
//
//@SEE_ALSO: bdlmxxx_elemtype, bdlmxxx_descriptor
//
//@DESCRIPTION: This component provides objects ("element references") that act
// as proxies for a 'bdem' data "element" and its "nullability information"
// (please see the section Element Reference Nullability below).  A 'bdem'
// element is an object of a type corresponding to one of the enumerated
// 'bdlmxxx::ElemType::Type' values.  An element reference provides non-modifiable
// or modifiable access to its referenced element value.  The two classes
// providing element references are:
//..
//  1. 'bdlmxxx::ConstElemRef' : Non-modifiable
//  2. 'bdlmxxx::ElemRef'      : Modifiable
//..
// An element reference is most commonly used to provide uniform access to an
// arbitrary element contained within a 'bdlmxxx::Row'; element references appear
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
//                         ( bdlmxxx::ElemRef )          to enable modification
//                          `------------'           of non-const elements,
//                                 |                 but no additional data.
//                                 V
//                        ,-----------------.
//                       ( bdlmxxx::ConstElemRef )       No virtual functions.
//                        `-----------------'
//..
// The (base) class 'bdlmxxx::ConstElemRef' provides the subset of functionality
// applicable to both 'const' and non-'const' elements.  'bdlmxxx::ElemRef',
// derived from 'bdlmxxx::ConstElemRef', extends the capabilities but must be
// applied only to non-'const' element instances.  In other words, a
// 'bdlmxxx::ElemRef' *isA* 'bdlmxxx::ConstElemRef' that also provides modifiable
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
// The constant (base-class) version, 'bdlmxxx::ConstElemRef', omit all operations
// that would enable the client to modify the referenced element; the
// modifiable (derived) version, 'bdlmxxx::ElemRef', extend the behavior of their
// respective base classes to permit element modification.  Note that, as is
// the case for a fundamental reference, an element reference has no management
// responsibility for its referenced element, and, in particular, cannot
// determine whether or not the referenced *element* is valid.  It is the
// user's responsibility to keep track of the valid lifetime of a referenced
// element.  This is especially important when the element is contained in a
// 'bdlmxxx::List', a 'bdlmxxx::Table', or other container, because modification of the
// container may affect the validity of the reference.  Unless otherwise noted,
// the documented behavior of all methods provided by this component assume
// valid references.  In particular, 'isBound' is the only method whose
// behavior is defined when called on an element reference that refers to a
// 'bdem' element that is no longer valid.
//
///Semantics of 'const'
///--------------------
// A 'bdlmxxx::ElemRef' object (whether or not that instance is 'const') can be
// used to access or modify the (value of the) element to which it refers.  A
// 'bdlmxxx::ConstElemRef' object can be used to access, but *not* modify, its
// referenced element.  Unless the *element* is modifiable, a 'bdlmxxx::ElemRef'
// object cannot be created for it, and a 'bdlmxxx::ConstElemRef' or object must be
// used instead.
//
///Element Reference Types
///-----------------------
// An element reference is bound to a 'bdem' element by supplying, at
// construction, the address of the element and the address of a
// 'bdlmxxx::Descriptor'.  In what follows, the type of the 'bdem' element will be
// denoted as the "actual type" (of the referenced element), and the type
// indicated by the 'bdlmxxx::Descriptor' will be denoted as the "descriptor type"
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
//  scalar (BDEM_INT, etc.)           bdltuxxx::Unset<TYPE>::unsetValue()
//
//  vectors (BDEM_INT_ARRAY, etc.)    0 == size()
//..
// To illustrate, consider a 'BDEM_BOOL' element within a 'bdlmxxx::ElemRef'.  The
// element can be in one of three possible states:
//..
//  * null with underlying value 'bdltuxxx::Unset<bool>::unsetValue()'
//  * non-null with underlying value 'false'
//  * non-null with underlying value 'true'
//..
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdlmxxx::ElemRef', 'myElemref', that refers to
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
// A 'bdlmxxx::ConstElemRef' or 'bdlmxxx::ElemRef' is a proxy for a specific (typed)
// non-modifiable or modifiable element (respectively), which can be used to
// facilitate communication across intermediaries that do not care about the
// specific type or value of that element.  For example, suppose we have a
// heterogeneous collection of values whose types correspond to those
// identified by the enumeration 'bdlmxxx::ElemType::Type':
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
//  bsls::ObjectBuffer<bdlmxxx::ConstElemRef> rawSpaceForList[LENGTH];
//
//  bdlmxxx::ConstElemRef *list = reinterpret_cast<bdlmxxx::ConstElemRef *>
//                                                          (rawSpaceForList);
//  new(list + 0) bdlmxxx::ConstElemRef(&aChar,      &charDescriptor);
//  new(list + 1) bdlmxxx::ConstElemRef(&anInt,      &intDescriptor);
//  new(list + 2) bdlmxxx::ConstElemRef(&anotherInt, &intDescriptor);
//  new(list + 3) bdlmxxx::ConstElemRef(&myDouble,   &doubleDescriptor);
//  new(list + 4) bdlmxxx::ConstElemRef(&theString,  &stringDescriptor);
//  new(list + 5) bdlmxxx::ConstElemRef(&someShort,  &shortDescriptor);
//  new(list + 6) bdlmxxx::ConstElemRef(&yourDouble, &doubleDescriptor);
//  // ...
//..
// Note that the creation of the various 'bdlmxxx::Descriptor' structures is not
// shown.  See 'bdlmxxx_descriptor' for more information on descriptors.  Also
// note that descriptor objects corresponding to the 20 "leaf" scalar and
// vector 'bdlmxxx::ElemType' enumerators can be obtained from the
// 'bdlmxxx_properties' component; similarly descriptor objects for 'BDEM_LIST',
// 'BDEM_TABLE', 'BDEM_ROW', 'BDEM_CHOICE', 'BDEM_CHOICE_ARRAY', and
// 'BDEM_CHOICE_ARRAY_ITEM' descriptors can be obtained from the
// 'bdlmxxx_listimp', 'bdlmxxx_tableimp', 'bdlmxxx_rowdata', 'bdlmxxx_choiceimp',
// 'bdlmxxx_choicearrayimp', and 'bdlmxxx_choiceheader' components, respectively.
//
// We can now write the 'isMember' function (at file scope) as follows:
//..
//  static bool isMember(const bdlmxxx::ConstElemRef  sequence[],
//                       int                      length,
//                       const bdlmxxx::ConstElemRef& element)
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
//                             bdlmxxx::ConstElemRef cer3(&v3, &stringDescriptor);
//  bsl::string v4 = "Foo Bar";
//                             const bdlmxxx::ElemRef ER4(&v4, &stringDescriptor);
//
//  assert(1 == isMember(list, 7, bdlmxxx::ConstElemRef(&V1, &intDescriptor)));
//  assert(0 == isMember(list, 7, bdlmxxx::ConstElemRef(&V2, &intDescriptor)));
//  assert(0 == isMember(list, 7, cer3));
//  assert(1 == isMember(list, 5, ER4))
//  assert(0 == isMember(list, 4, ER4));
//..
// An excellent use of 'bdlmxxx::ElemRef', as well as 'bdlmxxx::ConstElemRef', can be
// found in the 'bdlmxxx::Row' component.
//
// Without 'bdlmxxx::ConstElemRef' (and the underlying 'bdlmxxx::Descriptor'), we would
// represent an element as a pointer to its data and its type:
//..
//  const void *DATA[] = {                     // Should NOT be at file scope.
//      &aChar,      &anInt,     &anotherInt,
//      &myDouble,   &theString, &someShort,   // String is not POD.
//      &yourDouble,
//  };
//
//  const bdlmxxx::ElemType::Type TYPES[] = {      // Could be at file scope.
//      bdlmxxx::ElemType::BDEM_CHAR,
//      bdlmxxx::ElemType::BDEM_INT,
//      bdlmxxx::ElemType::BDEM_DOUBLE,
//      bdlmxxx::ElemType::BDEM_STRING,
//      bdlmxxx::ElemType::BDEM_SHORT,
//      bdlmxxx::ElemType::BDEM_DOUBLE
//  };
//..
// Without element references, the 'isMember' function would be much more
// cumbersome to write.  In particular, 'isMember' would need first to check
// the type and, if it is the same as the element in question, cast both
// elements to that type before invoking the native ('operator==') comparison:
//..
//  static bool isMember(const void                *sequenceData[],
//                       const bdlmxxx::ElemType::Type  sequenceTypes[],
//                       int                        sequenceLength,
//                       const void                *dataValue,
//                       bdlmxxx::ElemType::Type        dataType)
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
//            case bdlmxxx::ElemType::BDEM_CHAR: {
//              if (*static_cast<const char *>(dataValue) ==
//                  *static_cast<const char *>(sequenceData[i])) return true;
//            } break;
//            case bdlmxxx::ElemType::BDEM_SHORT: {
//              if (*static_cast<const short *>(dataValue) ==
//                  *static_cast<const short *>(sequenceData[i])) return true;
//            } break;
//            case bdlmxxx::ElemType::BDEM_INT: {
//              if (*static_cast<const int *>(dataValue) ==
//                  *static_cast<const int *>(sequenceData[i])) return true;
//            } break;
//            case bdlmxxx::ElemType::BDEM_INT64: {
//              typedef bsls::Types::Int64 Int64;
//              if (*static_cast<const Int64 *>(dataValue) ==
//                  *static_cast<const Int64 *>(sequenceData[i])) return true;
//            } break;
//            case bdlmxxx::ElemType::BDEM_FLOAT: {
//              if (*static_cast<const float *>(dataValue) ==
//                  *static_cast<const float *>(sequenceData[i])) return true;
//            } break;
//            case bdlmxxx::ElemType::BDEM_DOUBLE: {
//              if (*static_cast<const double *>(dataValue) ==
//                  *static_cast<const double *>(sequenceData[i])) return true;
//            } break;
//            case bdlmxxx::ElemType::BDEM_STRING: {
//              typedef bsl::string String;
//              if (*static_cast<const String *>(dataValue) ==
//                  *static_cast<const String *>(sequenceData[i])) return true;
//            } break;
//              // .
//              // .   (14 case statements omitted)
//              // .
//            case bdlmxxx::ElemType::BDEM_TABLE: {
//              typedef bdlmxxx::Table Table;
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
//  assert(1 == isMember(DATA, TYPES, 7, &V1, bdlmxxx::ElemType::BDEM_INT));
//  assert(0 == isMember(DATA, TYPES, 7, &V2, bdlmxxx::ElemType::BDEM_INT));
//  assert(0 == isMember(DATA, TYPES, 7, &v3, bdlmxxx::ElemType::BDEM_STRING));
//  assert(1 == isMember(DATA, TYPES, 5, &v4, bdlmxxx::ElemType::BDEM_STRING));
//  assert(0 == isMember(DATA, TYPES, 4, &v4, bdlmxxx::ElemType::BDEM_STRING));
//..
// Element references are valuable because the alternative, in addition to
// being bulky and error-prone, forces not only a *link-time*, but also a
// *COMPILE-TIME*, physical dependency of 'isMember' on *SPECIFIC*
// *IMPLEMENTATIONS* of *ALL* 22 types -- even though many implementations
// (e.g., of datetime, date, time, all ten arrays, list, and table) are not
// needed by the client of this code.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_DESCRIPTOR
#include <bdlmxxx_descriptor.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_PROPERTIES
#include <bdlmxxx_properties.h>
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



// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class DatetimeTz; }                            // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DatetimeTz DatetimeTz;        // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class Datetime; }                              // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Datetime Datetime;            // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class DateTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DateTz DateTz;                // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class Date; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Date Date;                    // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class TimeTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::TimeTz TimeTz;                // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class Time; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Time Time;                    // bdet -> bdlt
}  // close namespace bdet

namespace bdlmxxx {class ChoiceArrayItem;
class Choice;
class ChoiceArray;
class Row;
class List;
class Table;
class ElemRef;

                        // =======================
                        // class ConstElemRef
                        // =======================

class ConstElemRef {
    // This class represents a reference to a non-modifiable 'bdem' element
    // object.  Accessing the element is guaranteed to be an efficient,
    // constant-time operation.  Instances of this class can be created from
    // both modifiable and non-modifiable instances of 'bdem' elements.  Use of
    // a reference when its referenced element is no longer valid, apart from
    // the 'isBound' method, will result in undefined behavior.  In particular,
    // if the referenced element is contained within a 'List' or
    // 'Table', the element may become invalid if the container is
    // (structurally) modified, i.e., if the number or types of the elements in
    // the container are modified.
    //
    // Note that this class is decidedly *not* value-semantic, and instead has
    // *reference* *semantics*.  There is no conventional notion of assignment,
    // although copy construction and equality are supported.  Except for the
    // destructor, all instance methods of this object are 'const' and apply
    // not to this object, but rather to the "element" and "nullability
    // information" (pointer only, not offset) to which this object refers.
    // Note also that, unlike the derived 'ElemRef' class (also defined
    // within this component), none of the methods within this base class
    // enable modifiable access to the indicated element or nullability
    // information, thus ensuring 'const' correctness.

    // DATA
    union {
        const void *d_constData_p; // data element (held, not owned)
        void       *d_data_p;      // for use by 'ElemRef', never to be
                                   // used in this class (held, not owned)
    };

    const Descriptor *d_descriptor_p; // elem attributes (held, not owned)

    union {
        const int *d_constNullnessWord_p; // pointer to the nullness word that
        int       *d_nullnessWord_p;      // contains the nullability
                                          // information (held, not owned)
    };

    int d_nullnessBitOffset; // offset to the specific bit in the nullness word
                             // that holds the nullability information

    // FRIENDS
    friend class ElemRef;  // can access the data members as if they
                                // were protected, but no one else can

    friend bool operator==(const ConstElemRef&, const ConstElemRef&);

    // NOT IMPLEMENTED
    ConstElemRef& operator=(const ConstElemRef&);

  public:
    // CLASS METHODS
    static ConstElemRef unboundElemRef();
        // Return a reference that is not bound to any 'bdem' element.  Note
        // that an unbound reference may be used only to invoke a selected set
        // of accessors (see the function-level documentation for details), and
        // with the equality and streaming operators.  Also note that the
        // 'type' method returns 'ElemType::BDEM_VOID' for an unbound
        // reference.

    // CREATORS
    ConstElemRef(const void *data, const Descriptor *descriptor);
        // Create a reference to the specified non-modifiable and non-nullable
        // 'data' element that is characterized by the specified non-modifiable
        // 'descriptor'.  The behavior is undefined unless the type of 'data'
        // corresponds to 'descriptor', or else 'descriptor' corresponds to
        // 'ElemType::BDEM_VOID' (indicating an unbound reference).  Note
        // that if the referenced element is contained within a 'List' or
        // 'Table', then the behavior of this reference is undefined if it
        // is used after a modification of the number or types of the elements
        // in the container.

    ConstElemRef(const void            *data,
                      const Descriptor *descriptor,
                      const int             *nullnessWord,
                      int                    nullnessBitOffset);
        // Create a reference to the specified non-modifiable and nullable
        // 'data' element that is characterized by the specified non-modifiable
        // 'descriptor' and whose nullness bit is at the specified 0-based
        // 'nullnessBitOffset' in the specified 'nullnessWord' (indexed from
        // least-significant bit to most-significant bit).  The behavior is
        // undefined unless the type of 'data' corresponds to 'descriptor' or
        // else 'descriptor' corresponds to 'ElemType::BDEM_VOID'
        // (indicating an unbound reference), 'nullnessWord' is non-null, and
        // '0 <= nullnessBitOffset < sizeof(int) * 8'.  Note that if the
        // referenced element is contained within a 'List' or
        // 'Table', then the behavior of this reference is undefined if it
        // is used after a modification of the number or types of the elements
        // in the container.

    ConstElemRef(const ConstElemRef& original);
        // Create a reference to the non-modifiable element indicated by the
        // specified 'original' element reference.  The element referenced by
        // the new object is nullable if and only if 'original.isNullable()'.
        // Note that if the referenced element is contained within a
        // 'List' or 'Table', then the behavior of this reference is
        // undefined if it is used after a modification of the number or types
        // of the elements in the container.

    //! ~ConstElemRef();
        // Destroy this element reference.  Note that this method has no
        // effect on the referenced element.  Also note that this method's
        // definition is compiler generated.

    // ACCESSORS
    const bool& theBool() const;
    const char& theChar() const;
    const short& theShort() const;
    const int& theInt() const;
    const bsls::Types::Int64& theInt64() const;
    const float& theFloat() const;
    const double& theDouble() const;
    const bsl::string& theString() const;
    const bdlt::Datetime& theDatetime() const;
    const bdlt::DatetimeTz& theDatetimeTz() const;
    const bdlt::Date& theDate() const;
    const bdlt::DateTz& theDateTz() const;
    const bdlt::Time& theTime() const;
    const bdlt::TimeTz& theTimeTz() const;
    const bsl::vector<bool>& theBoolArray() const;
    const bsl::vector<char>& theCharArray() const;
    const bsl::vector<short>& theShortArray() const;
    const bsl::vector<int>& theIntArray() const;
    const bsl::vector<bsls::Types::Int64>& theInt64Array() const;
    const bsl::vector<float>& theFloatArray() const;
    const bsl::vector<double>& theDoubleArray() const;
    const bsl::vector<bsl::string>& theStringArray() const;
    const bsl::vector<bdlt::Datetime>& theDatetimeArray() const;
    const bsl::vector<bdlt::DatetimeTz>& theDatetimeTzArray() const;
    const bsl::vector<bdlt::Date>& theDateArray() const;
    const bsl::vector<bdlt::DateTz>& theDateTzArray() const;
    const bsl::vector<bdlt::Time>& theTimeArray() const;
    const bsl::vector<bdlt::TimeTz>& theTimeTzArray() const;
    const Choice& theChoice() const;
    const ChoiceArray& theChoiceArray() const;
    const ChoiceArrayItem& theChoiceArrayItem() const;
    const List& theList() const;
    const Row& theRow() const;
    const Table& theTable() const;
        // Return a reference to the non-modifiable element referenced by this
        // object.  The nullness of the element is not affected.  The behavior
        // is undefined unless the return type of the method used matches the
        // actual type and descriptor type of the element referenced by this
        // object.

    ElemType::Type type() const;
        // Return the type of the 'bdem' element referenced by this object, or
        // 'ElemType::BDEM_VOID' if this reference is unbound.

    const Descriptor *descriptor() const;
        // Return the address of the non-modifiable type-specific attributes of
        // this object.  Note that this method should *NOT* be called; it is
        // for *internal* use only.  Also note that if this reference is
        // unbound, the descriptor for 'ElemType::BDEM_VOID' is returned.

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
bool operator==(const ConstElemRef& lhs, const ConstElemRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' element references are
    // bound to 'bdem' elements having the same type, same nullability and the
    // same value (as defined by 'operator==' for that type) or if both the
    // 'lhs' and 'rhs' element references are unbound or if both the 'lhs' and
    // 'rhs' element references are null, and 'false' otherwise.  If 'lhs' and
    // 'rhs' are both bound, and their respective descriptor types match, then
    // the behavior is undefined unless their respective actual types
    // correspond to their common descriptor type.

bool operator!=(const ConstElemRef& lhs, const ConstElemRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' element references are
    // bound to 'bdem' elements having different types, different nullability
    // or different values, or if exactly one of the 'lhs' or 'rhs' element
    // references is unbound, or if exactly one of the 'lhs' or 'rhs' element
    // references is null, and 'false' otherwise.  If 'lhs' and 'rhs' are both
    // bound, and their respective descriptor types match, then the behavior is
    // undefined unless their respective actual types correspond to their
    // common descriptor type.

bsl::ostream& operator<<(bsl::ostream& stream, const ConstElemRef& rhs);
    // Format the element referenced by the specified 'rhs' element reference
    // to the specified output 'stream' in some reasonable (multi-line) format.
    // If 'stream' is valid, then the behavior is undefined unless 'rhs' is
    // unbound, or the descriptor type and the actual type of the element
    // referenced by 'rhs' correspond.  Note that if 'stream' is not valid,
    // then this operation has no effect.

                        // ==================
                        // class ElemRef
                        // ==================

class ElemRef : public ConstElemRef {
    // This class represents a reference to a modifiable 'bdem' element object.
    // Accessing the element is guaranteed to be an efficient, constant-time
    // operation.  Instances of this class can be created only from modifiable
    // instances of 'bdem' elements.  Use of a reference when its referenced
    // element is no longer valid, apart from the 'isBound' method, will result
    // in undefined behavior.  In particular, if the referenced element is
    // contained within a 'List' or 'Table', the element may become
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
    ElemRef& operator=(const ElemRef&);

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
    static ElemRef unboundElemRef();
        // Return a reference that is not bound to any 'bdem' element.  Note
        // that an unbound reference may be used only to invoke a selected set
        // of accessors (see the function-level documentation for details), and
        // with the equality and streaming operators.  Also note that the
        // 'type' method returns 'ElemType::BDEM_VOID' for an unbound
        // reference.

    // CREATORS
    ElemRef(void *data, const Descriptor *descriptor);
        // Create a reference to the specified modifiable and non-nullable
        // 'data' element that is characterized by the specified non-modifiable
        // 'descriptor'.  The behavior is undefined unless the type of 'data'
        // corresponds to 'descriptor', or else 'descriptor' corresponds to
        // 'ElemType::BDEM_VOID' (indicating an unbound reference).  Note
        // that if the referenced element is contained within a 'List' or
        // 'Table', then the behavior of this reference is undefined if it
        // is used after a modification of the number or types of the elements
        // in the container.

    ElemRef(void                  *data,
                 const Descriptor *descriptor,
                 int                   *nullnessWord,
                 int                    nullnessBitOffset);
        // Create a reference to the specified modifiable and nullable 'data'
        // element that is characterized by the specified non-modifiable
        // 'descriptor' and whose nullness bit is at the specified 0-based
        // 'nullnessBitOffset' in the specified 'nullnessWord' (indexed from
        // least-significant bit to most-significant bit).  The behavior is
        // undefined unless the type of 'data' corresponds to 'descriptor' or
        // else 'descriptor' corresponds to 'ElemType::BDEM_VOID'
        // (indicating an unbound reference), 'nullnessWord' is non-null, and
        // '0 <= nullnessBitOffset < sizeof(int) * 8'.  Note that if the
        // referenced element is contained within a 'List' or
        // 'Table', then the behavior of this reference is undefined if it
        // is used after a modification of the number or types of the elements
        // in the container.

    ElemRef(const ElemRef& original);
        // Create a reference to the modifiable element indicated by the
        // specified 'original' element reference.  The element referenced by
        // the new object is nullable if and only if 'original.isNullable()'.
        // Note that if the referenced element is contained within a
        // 'List' or 'Table', then the behavior of this reference is
        // undefined if it is used after a modification of the number or types
        // of the elements in the container.

    //! ~ElemRef();
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
    bsls::Types::Int64& theModifiableInt64() const;
    float& theModifiableFloat() const;
    double& theModifiableDouble() const;
    bsl::string& theModifiableString() const;
    bdlt::Datetime& theModifiableDatetime() const;
    bdlt::DatetimeTz& theModifiableDatetimeTz() const;
    bdlt::Date& theModifiableDate() const;
    bdlt::DateTz& theModifiableDateTz() const;
    bdlt::Time& theModifiableTime() const;
    bdlt::TimeTz& theModifiableTimeTz() const;
    bsl::vector<bool>& theModifiableBoolArray() const;
    bsl::vector<char>& theModifiableCharArray() const;
    bsl::vector<short>& theModifiableShortArray() const;
    bsl::vector<int>& theModifiableIntArray() const;
    bsl::vector<bsls::Types::Int64>& theModifiableInt64Array() const;
    bsl::vector<float>& theModifiableFloatArray() const;
    bsl::vector<double>& theModifiableDoubleArray() const;
    bsl::vector<bsl::string>& theModifiableStringArray() const;
    bsl::vector<bdlt::Datetime>& theModifiableDatetimeArray() const;
    bsl::vector<bdlt::DatetimeTz>& theModifiableDatetimeTzArray() const;
    bsl::vector<bdlt::Date>& theModifiableDateArray() const;
    bsl::vector<bdlt::DateTz>& theModifiableDateTzArray() const;
    bsl::vector<bdlt::Time>& theModifiableTimeArray() const;
    bsl::vector<bdlt::TimeTz>& theModifiableTimeTzArray() const;
    Choice& theModifiableChoice() const;
    ChoiceArray& theModifiableChoiceArray() const;
    ChoiceArrayItem& theModifiableChoiceArrayItem() const;
    List& theModifiableList() const;
    Row& theModifiableRow() const;
    Table& theModifiableTable() const;
        // Return a reference to the modifiable element referenced by this
        // object.  If the referenced element is null, it is made non-null
        // before returning, but its value is not otherwise modified.  The
        // behavior is undefined unless the return type of the method used
        // matches the actual type and descriptor type of the element
        // referenced by this object.  Note that unless there is an intention
        // of modifying the element, the corresponding 'theTYPE' method should
        // be used instead.

    void replaceValue(const ConstElemRef& referenceObject) const;
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
                        // class ConstElemRef
                        // -----------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
bool ConstElemRef::isBound() const
{
    BSLS_ASSERT_SAFE(d_descriptor_p);

    // TBD the check that 'd_descriptor_p' is non-null is there for
    // backward compatibility (e.g., 'ConstElemRef(0, 0)').

    return d_descriptor_p
        && ElemType::BDEM_VOID != d_descriptor_p->d_elemEnum;
}

// CLASS METHODS
inline
ConstElemRef ConstElemRef::unboundElemRef()
{
    return ConstElemRef(0, &Properties::s_voidAttr);
}

// CREATORS
inline
ConstElemRef::ConstElemRef(const void            *data,
                                     const Descriptor *descriptor)
: d_constData_p(data)
, d_descriptor_p(descriptor)
, d_constNullnessWord_p(0)
, d_nullnessBitOffset(0)
{
    BSLS_ASSERT_SAFE(descriptor
                  && (data
                   || ElemType::BDEM_VOID == d_descriptor_p->d_elemEnum));
}

inline
ConstElemRef::ConstElemRef(const void            *data,
                                     const Descriptor *descriptor,
                                     const int             *nullnessWord,
                                     int                    nullnessBitOffset)
: d_constData_p(data)
, d_descriptor_p(descriptor)
, d_constNullnessWord_p(nullnessWord)
, d_nullnessBitOffset(nullnessBitOffset)
{
    BSLS_ASSERT_SAFE(descriptor
                  && (data
                   || ElemType::BDEM_VOID == d_descriptor_p->d_elemEnum));
    BSLS_ASSERT_SAFE(nullnessWord);
    BSLS_ASSERT_SAFE((unsigned)nullnessBitOffset < sizeof(int) * 8);
}

inline
ConstElemRef::ConstElemRef(const ConstElemRef& original)
: d_constData_p(original.d_constData_p)
, d_descriptor_p(original.d_descriptor_p)
, d_nullnessWord_p(original.d_nullnessWord_p)
, d_nullnessBitOffset(original.d_nullnessBitOffset)
{
}

// ACCESSORS
inline
const bool& ConstElemRef::theBool() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_BOOL == d_descriptor_p->d_elemEnum);

    return *(const bool *)d_constData_p;
}

inline
const char& ConstElemRef::theChar() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_CHAR == d_descriptor_p->d_elemEnum);

    return *(const char *)d_constData_p;
}

inline
const short& ConstElemRef::theShort() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_SHORT == d_descriptor_p->d_elemEnum);

    return *(const short *)d_constData_p;
}

inline
const int& ConstElemRef::theInt() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_INT == d_descriptor_p->d_elemEnum);

    return *(const int *)d_constData_p;
}

inline
const bsls::Types::Int64& ConstElemRef::theInt64() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_INT64 == d_descriptor_p->d_elemEnum);

    return *(const bsls::Types::Int64 *)d_constData_p;
}

inline
const float& ConstElemRef::theFloat() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_FLOAT == d_descriptor_p->d_elemEnum);

    return *(const float *)d_constData_p;
}

inline
const double& ConstElemRef::theDouble() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_DOUBLE == d_descriptor_p->d_elemEnum);

    return *(const double *)d_constData_p;
}

inline
const bsl::string& ConstElemRef::theString() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_STRING == d_descriptor_p->d_elemEnum);

    return *(const bsl::string *)d_constData_p;
}

inline
const bdlt::Datetime& ConstElemRef::theDatetime() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                   ElemType::BDEM_DATETIME == d_descriptor_p->d_elemEnum);

    return *(const bdlt::Datetime *)d_constData_p;
}

inline
const bdlt::DatetimeTz& ConstElemRef::theDatetimeTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_DATETIMETZ == d_descriptor_p->d_elemEnum);

    return *(const bdlt::DatetimeTz *)d_constData_p;
}

inline
const bdlt::Date& ConstElemRef::theDate() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_DATE == d_descriptor_p->d_elemEnum);

    return *(const bdlt::Date *)d_constData_p;
}

inline
const bdlt::DateTz& ConstElemRef::theDateTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_DATETZ == d_descriptor_p->d_elemEnum);

    return *(const bdlt::DateTz *)d_constData_p;
}

inline
const bdlt::Time& ConstElemRef::theTime() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_TIME == d_descriptor_p->d_elemEnum);

    return *(const bdlt::Time *)d_constData_p;
}

inline
const bdlt::TimeTz& ConstElemRef::theTimeTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_TIMETZ == d_descriptor_p->d_elemEnum);

    return *(const bdlt::TimeTz *)d_constData_p;
}

inline
const bsl::vector<bool>& ConstElemRef::theBoolArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_BOOL_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bool> *)d_constData_p;
}

inline
const bsl::vector<char>& ConstElemRef::theCharArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_CHAR_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<char> *)d_constData_p;
}

inline
const bsl::vector<short>& ConstElemRef::theShortArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                ElemType::BDEM_SHORT_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<short> *)d_constData_p;
}

inline
const bsl::vector<int>& ConstElemRef::theIntArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                  ElemType::BDEM_INT_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<int> *)d_constData_p;
}

inline
const bsl::vector<bsls::Types::Int64>& ConstElemRef::theInt64Array() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                ElemType::BDEM_INT64_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bsls::Types::Int64> *)d_constData_p;
}

inline
const bsl::vector<float>& ConstElemRef::theFloatArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                ElemType::BDEM_FLOAT_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<float> *)d_constData_p;
}

inline
const bsl::vector<double>& ConstElemRef::theDoubleArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_DOUBLE_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<double> *)d_constData_p;
}

inline
const bsl::vector<bsl::string>& ConstElemRef::theStringArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_STRING_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bsl::string> *)d_constData_p;
}

inline
const bsl::vector<bdlt::Datetime>& ConstElemRef::theDatetimeArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
             ElemType::BDEM_DATETIME_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdlt::Datetime> *)d_constData_p;
}

inline
const bsl::vector<bdlt::DatetimeTz>&
ConstElemRef::theDatetimeTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
           ElemType::BDEM_DATETIMETZ_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdlt::DatetimeTz> *)d_constData_p;
}

inline
const bsl::vector<bdlt::Date>& ConstElemRef::theDateArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_DATE_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdlt::Date> *)d_constData_p;
}

inline
const bsl::vector<bdlt::DateTz>& ConstElemRef::theDateTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_DATETZ_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdlt::DateTz> *)d_constData_p;
}

inline
const bsl::vector<bdlt::Time>& ConstElemRef::theTimeArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_TIME_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdlt::Time> *)d_constData_p;
}

inline
const bsl::vector<bdlt::TimeTz>& ConstElemRef::theTimeTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_TIMETZ_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const bsl::vector<bdlt::TimeTz> *)d_constData_p;
}

inline
const Row& ConstElemRef::theRow() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_ROW == d_descriptor_p->d_elemEnum);

    return *(const Row *)d_constData_p;
}

inline
const List& ConstElemRef::theList() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_LIST == d_descriptor_p->d_elemEnum);

    return *(const List *)d_constData_p;
}

inline
const Table& ConstElemRef::theTable() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_TABLE == d_descriptor_p->d_elemEnum);

    return *(const Table *)d_constData_p;
}

inline
const ChoiceArrayItem& ConstElemRef::theChoiceArrayItem() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
          ElemType::BDEM_CHOICE_ARRAY_ITEM == d_descriptor_p->d_elemEnum);

    return *(const ChoiceArrayItem *)d_constData_p;
}

inline
const Choice& ConstElemRef::theChoice() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_CHOICE == d_descriptor_p->d_elemEnum);

    return *(const Choice *)d_constData_p;
}

inline
const ChoiceArray& ConstElemRef::theChoiceArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_CHOICE_ARRAY == d_descriptor_p->d_elemEnum);

    return *(const ChoiceArray *)d_constData_p;
}

inline
ElemType::Type ConstElemRef::type() const
{
    BSLS_ASSERT_SAFE(d_descriptor_p);

    return (ElemType::Type)d_descriptor_p->d_elemEnum;
}

inline
const Descriptor *ConstElemRef::descriptor() const
{
    BSLS_ASSERT_SAFE(d_descriptor_p);

    return d_descriptor_p;
}

inline
const void *ConstElemRef::data() const
{
    return d_constData_p;
}

inline
bool ConstElemRef::isNull() const
{
    BSLS_ASSERT_SAFE(isBound());

    if (!d_constNullnessWord_p) {
        return false;                                                 // RETURN
    }

    return ((1 << d_nullnessBitOffset) & *d_constNullnessWord_p) != 0;
}

inline
bool ConstElemRef::isNonNull() const
{
    BSLS_ASSERT_SAFE(isBound());

    return !isNull();
}

inline
bool ConstElemRef::isNullable() const
{
    BSLS_ASSERT_SAFE(isBound());

    return 0 != d_constNullnessWord_p;
}
}  // close package namespace

// FREE OPERATORS

inline
bool bdlmxxx::operator!=(const ConstElemRef& lhs, const ConstElemRef& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlmxxx::operator<<(bsl::ostream& stream, const ConstElemRef& rhs)
{
    return rhs.print(stream, 0 , -1);
}

namespace bdlmxxx {
                        // ------------------
                        // class ElemRef
                        // ------------------

// PRIVATE ACCESSORS
inline
void ElemRef::setNullnessBit() const
{
    if (d_constNullnessWord_p) {
        *d_nullnessWord_p |= (1 << d_nullnessBitOffset);
    }
}

inline
void ElemRef::clearNullnessBit() const
{
    if (d_constNullnessWord_p) {
        *d_nullnessWord_p &= ~(1 << d_nullnessBitOffset);
    }
}

// CLASS METHODS
inline
ElemRef ElemRef::unboundElemRef()
{
    return ElemRef(0, &Properties::s_voidAttr);
}

// CREATORS
inline
ElemRef::ElemRef(void *data, const Descriptor *descriptor)
: ConstElemRef(data, descriptor)
{
}

inline
ElemRef::ElemRef(void                  *data,
                           const Descriptor *descriptor,
                           int                   *nullnessWord,
                           int                    nullnessBitOffset)
: ConstElemRef(data, descriptor, nullnessWord, nullnessBitOffset)
{
}

inline
ElemRef::ElemRef(const ElemRef& original)
: ConstElemRef(original)
{
}

// REFERENCED-VALUE MANIPULATORS
inline
void ElemRef::makeNull() const
{
    BSLS_ASSERT_SAFE(isBound());

    if (isNullable()) {
        setNullnessBit();
    }
    d_descriptor_p->makeUnset(d_data_p);
}

inline
bool& ElemRef::theModifiableBool() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_BOOL == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bool *)d_data_p;
}

inline
char& ElemRef::theModifiableChar() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_CHAR == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(char *)d_data_p;
}

inline
short& ElemRef::theModifiableShort() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_SHORT == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(short *)d_data_p;
}

inline
int& ElemRef::theModifiableInt() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_INT == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(int *)d_data_p;
}

inline
bsls::Types::Int64& ElemRef::theModifiableInt64() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_INT64 == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsls::Types::Int64 *)d_data_p;
}

inline
float& ElemRef::theModifiableFloat() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_FLOAT == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(float *)d_data_p;
}

inline
double& ElemRef::theModifiableDouble() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_DOUBLE == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(double *)d_data_p;
}

inline
bsl::string& ElemRef::theModifiableString() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_STRING == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::string *)d_data_p;
}

inline
bdlt::Datetime& ElemRef::theModifiableDatetime() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                   ElemType::BDEM_DATETIME == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdlt::Datetime *)d_data_p;
}

inline
bdlt::DatetimeTz& ElemRef::theModifiableDatetimeTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_DATETIMETZ == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdlt::DatetimeTz *)d_data_p;
}

inline
bdlt::Date& ElemRef::theModifiableDate() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_DATE == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdlt::Date *)d_data_p;
}

inline
bdlt::DateTz& ElemRef::theModifiableDateTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_DATETZ == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdlt::DateTz *)d_data_p;
}

inline
bdlt::Time& ElemRef::theModifiableTime() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_TIME == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdlt::Time *)d_data_p;
}

inline
bdlt::TimeTz& ElemRef::theModifiableTimeTz() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_TIMETZ == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bdlt::TimeTz *)d_data_p;
}

inline
bsl::vector<bool>& ElemRef::theModifiableBoolArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_BOOL_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bool> *)d_data_p;
}

inline
bsl::vector<char>& ElemRef::theModifiableCharArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_CHAR_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<char> *)d_data_p;
}

inline
bsl::vector<short>& ElemRef::theModifiableShortArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                ElemType::BDEM_SHORT_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<short> *)d_data_p;
}

inline
bsl::vector<int>& ElemRef::theModifiableIntArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                  ElemType::BDEM_INT_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<int> *)d_data_p;
}

inline
bsl::vector<bsls::Types::Int64>& ElemRef::theModifiableInt64Array() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                ElemType::BDEM_INT64_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bsls::Types::Int64> *)d_data_p;
}

inline
bsl::vector<float>& ElemRef::theModifiableFloatArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                ElemType::BDEM_FLOAT_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<float> *)d_data_p;
}

inline
bsl::vector<double>& ElemRef::theModifiableDoubleArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_DOUBLE_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<double> *)d_data_p;
}

inline
bsl::vector<bsl::string>& ElemRef::theModifiableStringArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_STRING_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bsl::string> *)d_data_p;
}

inline
bsl::vector<bdlt::Datetime>& ElemRef::theModifiableDatetimeArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
             ElemType::BDEM_DATETIME_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdlt::Datetime> *)d_data_p;
}

inline
bsl::vector<bdlt::DatetimeTz>&
                             ElemRef::theModifiableDatetimeTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
           ElemType::BDEM_DATETIMETZ_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdlt::DatetimeTz> *)d_data_p;
}

inline
bsl::vector<bdlt::Date>& ElemRef::theModifiableDateArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_DATE_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdlt::Date> *)d_data_p;
}

inline
bsl::vector<bdlt::DateTz>& ElemRef::theModifiableDateTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_DATETZ_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdlt::DateTz> *)d_data_p;
}

inline
bsl::vector<bdlt::Time>& ElemRef::theModifiableTimeArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
                 ElemType::BDEM_TIME_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdlt::Time> *)d_data_p;
}

inline
bsl::vector<bdlt::TimeTz>& ElemRef::theModifiableTimeTzArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_TIMETZ_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(bsl::vector<bdlt::TimeTz> *)d_data_p;
}

inline
Row& ElemRef::theModifiableRow() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_ROW == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(Row *)d_data_p;
}

inline
List& ElemRef::theModifiableList() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_LIST == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(List *)d_data_p;
}

inline
Table& ElemRef::theModifiableTable() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_TABLE == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(Table *)d_data_p;
}

inline
ChoiceArrayItem& ElemRef::theModifiableChoiceArrayItem() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
          ElemType::BDEM_CHOICE_ARRAY_ITEM == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(ChoiceArrayItem *)d_data_p;
}

inline
Choice& ElemRef::theModifiableChoice() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(ElemType::BDEM_CHOICE == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(Choice *)d_data_p;
}

inline
ChoiceArray& ElemRef::theModifiableChoiceArray() const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(
               ElemType::BDEM_CHOICE_ARRAY == d_descriptor_p->d_elemEnum);

    clearNullnessBit();
    return *(ChoiceArray *)d_data_p;
}

// ACCESSORS
inline
void *ElemRef::data() const
{
    clearNullnessBit();
    return d_data_p;
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
