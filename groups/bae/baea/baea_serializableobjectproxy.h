// baea_serializableobjectproxy.h                                     -*-C++-*-
#ifndef INCLUDED_BAEA_SERIALIZABLEOBJECTPROXY
#define INCLUDED_BAEA_SERIALIZABLEOBJECTPROXY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baea_serializableobjectproxy_h,"$Id: baea_serializableobjectproxy.h 443282 2012-10-29 18:52:27Z mgiroux $ $CSID: 508ED0180994E500FF $")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide a dynamically-typed proxy for serializable types.
//
//@CLASSES:
//           baea_SerializableObjectProxy: proxy for auto-generated types
//  baea_SerializableObjectProxyFunctions: namespace for function type aliases
//
//@AUTHOR: David Schumann (dschumann1), Raymond Chiu (schiu49)
//
//@SEE_ALSO: baea_serializableobjectproxyutil, bdeat_typecategory
//
//@DESCRIPTION: This component provides a mechanism,
// 'baea_SerializableObjectProxy', that serves as a proxy, encapsulating a
// 'bdeat'-compliant object with a common interface providing the operations
// required by 'bdeat' encoders and decoders.  A 'baea_SerializableObjectProxy'
// object implements all the accessors and manipulators required by
// 'bdeat'-based encoders/decoders (such as 'bdem_berencoder',
// 'bdem_berdecoder', and similar encoders/decoders for XML).  A
// 'baea_SerializableObjectProxy' object provide a set of "load" methods for
// configuring a proxy object to represent the intended 'bdeat'-compliant
// object so that it can be used to either encode or decode (but not both) the
// proxied object.  However, in practice, clients should use a higher level
// component (e.g., 'baea_serializableobjectproxyutil') to configure the proxy
// object.
//
// The purpose of 'baea_SerializableObjectProxy' is to reduce the size of
// executable code associated with encoding and decoding auto-generated
// ('bdeat'-compliant) objects.  The 'bdeat'-based encoders and decoders are
// templatized on the objects types that they encode and decode, and
// instantiating the encoders and decoders for a many different types of
// objects, can significantly increase the code size of an application.  This
// component provides a proxy type such that the encoders and decoders are
// instantiated in terms of only 'baea_SerializableObjectProxy', instead of all
// the individual types that are encoded and decoded.  Rather than encoding or
// decoding 'bdeat'-compliant object directly, a client will create and
// configure a proxy to represent that object (either for encoding or
// decoding), and then encode or decode the proxy.

// The type categories of the 'bdeat' package are used throughout this
// component, and the terms "Array", "Choice", "Customized", "Enumeration",
// "Nullable", "Sequence", and "Simple" all refer to the corresponding 'bdeat'
// categories (see the 'bdeat_typecategory' component for description of each
// type).
//
///Treatment of "Customized" Types
///-------------------------------
// The 'bdeat' package has a concept of Customized type.  Types in this
// category contains an object of another type and impose certain restrictions
// on the value of that object.  A common example is a string of restricted
// length.  'baea_SerializableObjectProxy' cannot directly represent a
// Customized type, but can be used to represent its underlying base type.
//
// Customized types are required to have conversion methods to their underlying
// type -- for example, a Customized type based on 'bsl::string' is required to
// have a 'toString' method.  'baea_SerializableObjectProxy' is compatible only
// with Customized type that offer conversion methods that return a reference
// to their underlying object so that it is safe to take the address of the
// return value and store it for later use.  Using a Customized type with a
// conversion method that returns by value will result in undefined behavior.
// This is not required by the 'bdeat' package.  A Customized type based on
// 'bsl::string' is allowed to return a 'bsl::string' by value from 'toString',
// but such types cannot be used with this component.  Typically,
// auto-generated message components (e.g, those created with 'bas_codegen.pl')
// emit Customized types having conversion methods that return 'const'
// references to their underlying value and are thus compatible with this
// component.
//
///Restrictions On Proxied Types
///-----------------------------
// 'baea_SerializableObjectProxy' imposes restrictions on the implementation
// of the proxied type that are stricter than those imposed by the 'bdeat'
// framework in order to simplify the resulting object code.
//
//: o All Array objects must be instantiations of 'bsl::vector'.
//:
//: o All Nullable objects must be instantiations of either
//:   'bdeut_NullableValue' or 'bdeut_NullableAllocatedValue'.
//:
//: o All Choice types must provide public access to an enumerated value,
//:   'NUM_SELECTIONS', and class members named 'SELECTION_INFO_ARRAY' of type
//:   'const bdeat_SelectionInfo *', and 'CLASS_NAME' of type 'const char *'.
//:
//: o All Sequence types must provide public access to an enumerated value,
//:   'NUM_ATTRIBUTES', and class members named 'ATTRIBUTE_INFO_ARRAY' of type
//:   'const bdeat_AttributeInfo *', and 'CLASS_NAME' of type 'const char *'.
//:
//: o The accessors (e.g., 'toString') of Customized types are required to
//:   return const references to member variables -- the addresses of these
//:   return values will be taken, so they must not be temporaries.
//
// Violation of these restrictions except for the last one listed will lead to
// a compile-time error.  Violation of the last restriction listed
// (regarding the accessors of Customized types) will lead to undefined
// behavior at runtime.  Note that types auto-generated by 'bas_codegen.pl'
// will meet all of these restrictions.
//
///Using 'baea_SerializableObjectProxy'
///-------------------------------
// Use of 'baea_SerializableObjectProxy' is intended to reduce the amount of
// compiled object code required to encode or decode objects.  As a result,
// certain optimizations have been made which require careful attention by the
// users of the type.
//
//: o When decoding, 'baea_SerializableObjectProxy' does not 'reset' the state
//:   of the target variable prior to decoding a value into it, whereas the BER
//:   and XML encoders automatically do.  As a result, users of
//:   'baea_SerializableObjectProxy' should manually reset the target variable
//:   before decoding.
//:
//: o When decoding, 'baea_SerializableObjectProxy' represents the underlying
//:   object of the Customized type.  Therefore, it does not impose the
//:   restrictions of the Customized types (e.g., length-limited strings).
//:
//: o A particular 'baea_SerializableObjectProxy' object can be used for either
//:   encoding or decoding, but not both.
//:
//: o 'baea_SerializableObjectProxy' does not support BDEX serialization.
//:
//: o The encoder or decoder should be invoked on a proxy representing a Choice
//:   or Sequence object.  There may be compile-time or run-time issues if
//:   attempting to encode or decode a top-level object of one of the other
//:   'bdeat' types (Array, Nullable, etc).  In other words, the other 'bdeat'
//:   types are required to be contained, recursively, within a top-level
//:   object that is a Choice or Sequence object.
//
///Proxy Object Structure
///----------------------
// If the proxied object contains another object -- as is the case for
// Sequence, Choice, Nullable, and Array objects -- the proxy requires a
// function that configures a 'baea_SerializableObjectProxy' object to
// represent the contained object.
//
// This can be thought of as a form of "lazy evaluation".  An example may
// help illustrate the approach.  We start by outlining the definition of
// a 'MyChoiceType' type generated by "bas_codegen.pl", having 'MySequenceType'
// as one of its selections, that we wish to encode/decode:
//..
//  class MyChoiceType {
//    union {
//      bsls_ObjectBuffer<MySequenceType> d_mySequence;  // selection 0
//      // ...
//    };
//    // ...
//    MySequenceType& makeMySequenceValue();
//    // ...
//    MySequenceType& mySequenceValue();
//    // ...
//  };
//..
// Then we define an object of 'MyChoiceType', 'myChoice', and an initially
// empty 'baea_SerializableObjectProxy', 'myProxy', that we will use for
// encoding 'myChoice'.
//..
//  MyChoiceType myChoice;
//  myChoice.makeMySequenceValue();
//  baea_SerializableObjectProxy myProxy;
//..
// Now we invoke the 'loadChoice' method.  We assume we have a function named
// 'makeEncodeProxy' that configures a (different)
// 'baea_SerializableObjectProxy' object to represent the selection (i.e.,
// 'myChoice.d_mySequence').
//..
//  myProxy.loadChoice(&myChoice.mySequenceValue(),
//                     myChoice.lookupSelectionInfo(0),
//                     myChoice::CLASS_NAME,
//                     &makeEncodeProxy);
//..
//  This will configure 'myProxy' in the following way:
//..
//  ,----------------------------------.
//  |  baea_SerializableObjectProxy    |
//  |    void         *d_object_p;   --+-->[ &myChoice.d_mySequence ]
//  |    Variant<...>  d_objectInfo; --+--.
//  `----------------------------------'  |
//                                        |
//                                        V
//  ,--------------------------------------------.
//  |  ChoiceEncodeInfo                          |
//  |    bdeat_SelectionInfo *d_selection;   ----+--> [ selection description ]
//  |    const char          *d_classname_p; ----+--> "MyChoiceType"
//  |    Loader               d_loader;      ----+--> makeEncodeProxy
//  `--------------------------------------------'
//..
// So for a Choice object, the proxy object configured for encoding that Choice
// contains: a pointer to the underlying selection, information about that
// selection, and a pointer to a function that, when invoked, will configure
// another proxy for that selection.  That function is not invoked until the
// object is later encoded.  A similar structure is created for
// encoding/decoding any container type, but the details vary slightly based on
// the type and the direction (encoding/decoding).
//
// When a "visitor" compliant with the 'bdeat' framework is invoked on a
// 'baea_SerializableObjectProxy', the proxy creates *another*
// 'baea_SerializableObjectProxy' on the stack, invokes the function pointer
// supplied to its "load" method to configure that new object, and then
// recursively visits that object.  Note that 'baea_SerializableObjectProxy'
// objects are PODs so these temporary objects are relatively inexpensive to
// create and destroy.
//
// Note that the 'serializableobjectproxyutil' component provides
// implementation of functions for the 'load' methods.
//
///Usage Example
///-------------
// This section illustrates intended use of this component.
//
///Example 1: Creating a Proxy Object for a Simple Request
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to decode some XML data.  The data conforms to this schema:
//..
//  <?xml version='1.0' encoding='UTF-8'?>
//  <xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'
//             xmlns:test='http://bloomberg.com/schemas/test'
//             targetNamespace='http://bloomberg.com/schemas/test'
//             elementFormDefault='unqualified'>
//
//      <xs:complexType name='Simple'>
//          <xs:sequence>
//              <xs:element name='status' type='xs:int'/>
//          </xs:sequence>
//      </xs:complexType>
//
//      <xs:element name='Simple' type='test:Simple'/>
//  </xs:schema>
//..
// This schema specifies a Sequence type named 'Simple' containing one element
// of type 'int' named 'status'.  Using the 'bas_codegen.pl' tool, we generate
// a C++ class corresponding to this schema:
//..
//  $ bas_codegen.pl -m msg -p test -E simple.xsd
//..
// This generates the header and implementation files for the 'test_simple'
// component, which contains a 'Test::Simple' class.
//
// Since 'Test::Simple' is a Sequence type, we first need to create a function
// that configures a proxy object for its element.  Note that in this case,
// 'Test::Simple' has only one element, 'status'.  Also note that, for
// simplification, this function will only work with 'Test::Simple' type, and
// not other Sequence types:
//..
//  void elementLoader(baea_SerializableObjectProxy        *proxy,
//                     const baea_SerializableObjectProxy&  object,
//                     int                                  attributeId)
//     // Configure the specified 'proxy' to represent the 'status' of the
//     // 'Test::Simple' object represented by the specified 'object'.  The
//     // specified 'attributeId' is ignored.
//  {
//      test::Simple *simpleObject = (test::Simple *)object.object();
//      proxy->loadSimple(&simpleObject->status());
//  }
//..
// Then, we define the XML string that we would like to decode.  This XML
// string defines a 'Simple' object to have a 'status' of 42:
//..
//  const char data[] =
//      "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
//      "<Simple xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
//      "    <status>42</status>"
//      "</Simple>";
//..
// Next, we create a 'test::Simple' object, 'result', to decode the XML string
// into:
//..
//  test::Simple result;
//..
// Then, instead of decoding the XML string directly into 'result', we create a
// proxy object for 'result'.
//..
//  baea_SerializableObjectProxy decodeProxy;
//..
// Since 'test::Simple' is a Sequence object, we use the 'loadSequence' method
// to configure the proxy object, supplying the necessary information required
// to represent 'result'.  Note that this is done for demonstration purposes.
// Users should use methods in the 'baea_serializableobjectproxyutil' component
// to create the proxy object instead:
//..
//  decodeProxy.loadSequence(test::Simple::NUM_ATTRIBUTES,
//                           &result,
//                           test::Simple::ATTRIBUTE_INFO_ARRAY,
//                           test::Simple::CLASS_NAME,
//                           &elementLoader);
//..
// Now, we create a decoder and decode the XML string into 'result' through
// 'decodeProxy':
//..
//  baexml_DecoderOptions dOptions;
//  baexml_MiniReader reader;
//  baexml_Decoder decoder(&dOptions, &reader);
//
//  bdesb_FixedMemInStreamBuf isb(data, sizeof(data) - 1);
//  decoder.decode(&isb, &decodeProxy);
//..
// Finally, we verify that 'result' has the expected value.
//..
//  assert(42 == result.status());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CHOICEFUNCTIONS
#include <bdeat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMERATORINFO
#include <bdeat_enumeratorinfo.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_SEQUENCEFUNCTIONS
#include <bdeat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDET_DATETIMETZ
#include <bdet_datetimetz.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEALLOCATEDVALUE
#include <bdeut_nullableallocatedvalue.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

#ifndef INCLUDED_BDEUT_VARIANT
#include <bdeut_variant.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class baea_SerializableObjectProxy;

                   // ============================================
                   // struct baea_SerializableObjectProxyFunctions
                   // ============================================

struct baea_SerializableObjectProxyFunctions {
    // This 'struct' provides a namespace for type aliases for common function
    // pointer types used in the interface of this component.

    typedef void (*Loader)(baea_SerializableObjectProxy *proxy, void *object);
        // This 'typedef' is an alias for a function that configures the
        // specified 'proxy' with the specified 'object'.    

    typedef void (*SelectionLoader)(
                                 baea_SerializableObjectProxy  *proxy,
                                 void                          *object,
                                 const bdeat_SelectionInfo    **selectInfoPtr);
        // This 'typedef' is an alias for a function that configures the
        // specified 'proxy' with the specified 'object' and loads the current
        // selection to the specified 'selectInfoPtr'.  If the 'object' is 
        // an unselected choice, this method has no effect.  

    typedef void (*ElementLoader)(baea_SerializableObjectProxy        *proxy,
                                  const baea_SerializableObjectProxy&  object,
                                  int                                  id);
        // This 'typedef' is an alias for a function that configures the
        // specified 'proxy' with the element having the specified 'id' in the
        // specified 'object'.

    typedef int (*Chooser)(void *object, int selectionId);
        // This 'typedef' is an alias for a function that changes the selection
        // of the specified Choice object to the specified 'selectionId'.

    typedef void (*Resizer)(void         *object,
                            void        **newBeginAddress,
                            bsl::size_t   newSize);
        // This 'typedef' is an alias for a function that loads, into the
        // specified 'newBeginAddress', the address of the first element after
        // resizing a 'bsl::vector' referred to by the specified 'object' to
        // the specified 'newSize'.

    typedef void * (*ObjectFetcher)(void *object);
        // This 'typedef' is an alias for a function that returns the address
        // of the specified Nullable 'object'.

    typedef int (*IntSetter)(void *object, int value);
        // This 'typedef' is an alias for a function that configures the
        // specified Enumeration 'object' with the specified 'value'.

    typedef int (*StringSetter)(void *object, const char *value, int length);
        // This 'typedef' is an alias for a function that configures the
        // specified Enumeration 'object' using the specified string, 'value',
        // having the specified 'length'.

    typedef void (*ValueMaker)(void *object);
        // This 'typedef' is an alias for a function that creates a value for
        // the specified Nullable 'object', making it non-null.
};

              // ====================================================
              // struct baea_SerializableObjectProxy_ChoiceEncodeInfo
              // ====================================================

struct baea_SerializableObjectProxy_ChoiceEncodeInfo {
    // [!PRIVATE!] This 'struct' holds the data necessary to represent a
    // Choice-type object for encoding.

    // TYPES
    typedef baea_SerializableObjectProxyFunctions Functions;  // for brevity

    // PUBLIC DATA
    const bdeat_SelectionInfo *d_selectionInfo_p; // info for current selection

    Functions::Loader          d_loader;          // address of a function that
                                                  // will create proxy for
                                                  // selection

    const char                *d_className_p;     // class name, may be 0

    // CREATORS
    baea_SerializableObjectProxy_ChoiceEncodeInfo (
                                      const bdeat_SelectionInfo *selectionInfo,
                                      Functions::Loader          accessor,
                                      const char                *className);
        // Create a 'baea_SerializableObjectProxy_ChoiceEncodeInfo' object
        // having the specified 'selectionInfo', 'accessor' and 'className'
        // attribute values.  'className' may be 0.
};

              // ====================================================
              // struct baea_SerializableObjectProxy_ChoiceDecodeInfo
              // ====================================================

struct baea_SerializableObjectProxy_ChoiceDecodeInfo {
    // [!PRIVATE!] This 'struct' holds the data necessary to represent a
    // Choice-type object for decoding.

    // TYPES
    typedef baea_SerializableObjectProxyFunctions Functions; // for brevity

    // PUBLIC DATA
    int                         d_numSelections;  // total number of selections

    const bdeat_SelectionInfo  *d_selectionInfoArray_p;
                                                  // array of selection info
                                                  // (held, not owned)

    int                         d_currentSelection;
                                                  // current selection of the
                                                  // Choice object

    Functions::SelectionLoader  d_loader;         // address of a function that
                                                  // will creates a proxy and
                                                  // gets info for current
                                                  // selection

    Functions::Chooser          d_chooser;        // address of a function that
                                                  // will change the current
                                                  // selection

    // CREATORS
    baea_SerializableObjectProxy_ChoiceDecodeInfo(
                                int                         numSelections,
                                const bdeat_SelectionInfo  *selectionInfoArray,
                                Functions::SelectionLoader  loader,
                                Functions::Chooser          chooser);
        // Create a 'baea_SerializableObjectProxy_ChoiceDecodeInfo' object
        // having the specified 'numSelections', 'selectionInfoArray', 'loader'
        // and 'chooser' attribute values.
};

              // ===================================================
              // struct baea_SerializableObjectProxy_ArrayEncodeInfo
              // ===================================================

struct baea_SerializableObjectProxy_ArrayEncodeInfo {
    // [!PRIVATE!] This 'struct' holds the data necessary to represent an
    // Array-type object for encoding.  As described in the component- level
    // documentation, only instantiations of 'bsl::vector' are supported.

    // TYPES
    typedef baea_SerializableObjectProxyFunctions Functions;  // for brevity

    // PUBLIC DATA
    int               d_length;       // number of elements in vector

    int               d_elementSize;  // element size, in bytes

    Functions::Loader d_loader;       // address of a function that will create
                                      // a proxy for an object of the contained
                                      // type

    // CREATORS
    baea_SerializableObjectProxy_ArrayEncodeInfo(int               length,
                                                 int               elementSize,
                                                 Functions::Loader loader);
        // Create a 'baea_SerializableObjectProxy_ArrayEncodeInfo' object
        // having the specified 'length', 'elementSize' and 'loader' attribute
        // values.
};

              // ===================================================
              // struct baea_SerializableObjectProxy_ArrayDecodeInfo
              // ===================================================

struct baea_SerializableObjectProxy_ArrayDecodeInfo {
    // [!PRIVATE!] This 'struct' holds the data necessary to represent an
    // Array-type object for decoding.  As described in the component-level
    // documentation, only instantiations of 'bsl::vector' are supported.

    // TYPES
    typedef baea_SerializableObjectProxyFunctions Functions;  // for brevity

    // PUBLIC DATA
    int                 d_length;       // number of elements in vector

    int                 d_elementSize;  // element size, in bytes

    void               *d_begin;        // address of first element, 0 if empty

    Functions::Resizer  d_resizer;      // address of a function that will
                                        // changes the size of the Array object

    Functions::Loader   d_loader;       // address of a function that will
                                        // create a proxy for an object of the
                                        // contained type

    // CREATORS
    baea_SerializableObjectProxy_ArrayDecodeInfo(
                                               int                 length,
                                               int                 elementSize,
                                               void               *begin,
                                               Functions::Resizer  resizer,
                                               Functions::Loader   loader);
        // Create a 'baea_SerializableObjectProxy_ArrayDecodeInfo' object
        // having the specified 'length', 'elementSize', 'begin', 'resizer' and
        // 'loader' attribute values.
};

              // ==================================================
              // struct baea_SerializableObjectProxy_EnumDecodeInfo
              // ==================================================

struct baea_SerializableObjectProxy_EnumDecodeInfo {
    // [!PRIVATE!] This 'struct' holds the data necessary to represent an
    // Enumeration-type object for decoding.

    // TYPES
    typedef baea_SerializableObjectProxyFunctions Functions;  // for brevity

    // PUBLIC DATA
    Functions::IntSetter        d_intSetter;      // address of a function that
                                                  // will set the value based
                                                  // on an 'int'

    Functions::StringSetter     d_stringSetter;   // address of a function that
                                                  // will set the value based
                                                  // on a string

    const bdeat_EnumeratorInfo *d_infoArray_p;    // array of enumerator info
                                                  // (held, not owned)

    int                         d_infoArraySize;  // size of 'd_infoArray_p'
};

              // ==================================================
              // struct baea_SerializableObjectProxy_EnumEncodeInfo
              // ==================================================

struct baea_SerializableObjectProxy_EnumEncodeInfo {
    // [!PRIVATE!] This 'struct' holds the data necessary to represent an
    // Enumeration-type object for encoding.

    // PUBLIC DATA
    int                         d_intVal;         // the current value as int

    const bdeat_EnumeratorInfo *d_infoArray_p;    // array of enumerator info
                                                  // (held, not owned)

    int                         d_infoArraySize;  // size of 'd_infoArray_p'
};

              // ================================================
              // struct baea_SerializableObjectProxy_SequenceInfo
              // ================================================

struct baea_SerializableObjectProxy_SequenceInfo {
    // [!PRIVATE!] This 'struct' holds the data necessary to represent a
    // Sequence-type object for either encoding or decoding

    // TYPES
    typedef baea_SerializableObjectProxyFunctions Functions;  // for brevity

    // PUBLIC DATA
    int                        d_numAttributes;    // number of attributes

    const bdeat_AttributeInfo *d_attributeInfo_p;  // array of attribute info
                                                   // (held, not owned)

    Functions::ElementLoader   d_loader;           // address of a function
                                                   // that will create a proxy
                                                   // for the element with an
                                                   // id

    const char                *d_className_p;      // class name, may be 0

    // CREATORS
    baea_SerializableObjectProxy_SequenceInfo(
                                      int                        numAttributes,
                                      const bdeat_AttributeInfo *attributeInfo,
                                      const char                *className,
                                      Functions::ElementLoader   loader);
        // Create a 'baea_SerializableObjectProxy_SequenceInfo' object having
        // the specified 'numAttributes', 'attributeInfo', 'className' and
        // 'loader' attribute values.  'className' may be null.
};

              // =================================================
              // struct baea_SerializableObjectProxy_SimplePointer
              // =================================================

struct baea_SerializableObjectProxy_SimplePointer {
    // [!PRIVATE!] This 'struct' is used for representing a simple type.  The
    // underlying type is represented by this 'struct', rather than by a
    // 'bdeut_Variant', to avoid extremely long mangled names that can result
    // from placing a large Variant inside another large Variant.

    // CONSTANTS
    enum Type {
        // This enumeration identifies the Simple type that is being
        // represented by the proxy object.

        TYPE_CHAR,        // represents a 'char' value
        TYPE_UCHAR,       // represents a 'unsigned char' value
        TYPE_SHORT,       // represents a 'short' value
        TYPE_INT,         // represents a 'int' value
        TYPE_INT64,       // represents a 'bsls_Types::Int64' value
        TYPE_USHORT,      // represents a 'unsigned short' value
        TYPE_UINT,        // represents a 'unsigned int' value
        TYPE_UINT64,      // represents a 'bsls_Types::Uint64' value
        TYPE_FLOAT,       // represents a 'float' value
        TYPE_DOUBLE,      // represents a 'double' value
        TYPE_STRING,      // represents a 'bsl::string' value
        TYPE_DATETIME,    // represents a 'bdet_Datetime' value
        TYPE_DATE,        // represents a 'bdet_Date' value
        TYPE_TIME,        // represents a 'bdet_Time' value
        TYPE_BOOL,        // represents a 'bool' value
        TYPE_DATETIMETZ,  // represents a 'bdet_DatetimeTz' value
        TYPE_DATETZ,      // represents a 'bdet_DateTz' value
        TYPE_TIMETZ       // represents a 'bdet_TimeTz' value
    };

    Type d_type;  // type of the represented value
};

              // ==================================================
              // struct baea_SerializableObjectProxy_NullableDecode
              // ==================================================

struct baea_SerializableObjectProxy_NullableDecodeInfo {
    // [!PRIVATE!] This 'struct' is used when the proxy represents a
    // Nullable-type object for decoding.  As described in the component-level
    // documentation, only instantiations of bdeut_NullableValue or
    // bdeut_NullableAllocatedValue are supported.

    // TYPES
    typedef baea_SerializableObjectProxyFunctions Functions;  // for brevity

    // PUBLIC DATA
    Functions::ValueMaker    d_valueMaker; // address of a function that will
                                           // make the value non-null

    Functions::ObjectFetcher d_fetcher;    // address of a function that will
                                           // return address of underlying data

    Functions::Loader        d_loader;     // address of a function that will
                                           // make proxy for underlying data
};

typedef baea_SerializableObjectProxyFunctions::Loader
                               baea_SerializableObjectProxy_NullableEncodeInfo;
    // This 'typedef' represents a Nullable object for encoding.  The only data
    // necessary is the function for creating a proxy for the underlying object

              // ===================================================
              // struct baea_SerializableObjectProxy_NullableAdapter
              // ===================================================

struct baea_SerializableObjectProxy_NullableAdapter {
    // [!PRIVATE!] This 'struct' provides a type that holds a pointer to a
    // 'baea_SerializableObjectProxy', but is only identified as a Nullable
    // type (through the implementation of the 'bdeat' functions overloads),
    // and not any other 'bdeat' type.  'baea_SerializableObjectProxy' is
    // identified as multiple 'bdeat' types, and is therefore considered to be
    // a Dynamic type (see 'bdeat_typecategory' for how Dynamic type is
    // defined).  'baea_SerializableObjectProxy_NullableAdapter' is required
    // for integration with at least the BER decoder, which cannot handle a
    // nullable element of Dynamic type contained within a Dynamic type.  So
    // this adapter allows for a nullable 'baea_SerializableObjectProxy' of
    // non-Dynamic type.
    //
    // As described in the component-level documentation, the encoders/decoders
    // will not be invoked directly on Nullable proxies, but instead on a
    // Choice or Sequence; thus any Nullable object will be held within another
    // object.  So the accessor/manipulator methods of proxy objects
    // representing a Choice, Array, and Sequence object will test whether that
    // element is a Nullable (before invoking the accessor/manipulator on a
    // proxy for a contained element) , and if so, construct an object of this
    // type (NullableAdapter) and then invoke the accessor/manipulator on it.

    baea_SerializableObjectProxy *d_proxy_p;  // pointer to the proxy it
                                              // represents (held, not owned)
};

                     // ==================================
                     // class baea_SerializableObjectProxy
                     // ==================================

class baea_SerializableObjectProxy {
    // This class provides a proxy mechanism for encoding and decoding a
    // proxied object.  'baea_SerializableObjectProxy' exposes the minimal
    // interface required by 'bdeat'-based codecs (see 'bdem_berencoder',
    // 'baexml_encoder', etc) and insulates the encoder/decoder from type
    // information about the proxied types, primarily types autogenerated by
    // 'bas_codegen.pl' or types that expose the same introspection API.  Note
    // that clients are encouraged to use utilities provided in the
    // 'baea_serializableobjectproxyutil' component to configure a
    // 'baea_SerializableObjectProxy' object instead of invoking the "load"
    // methods directly.
    //
    // This class:
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // PRIVATE TYPES
    typedef baea_SerializableObjectProxy_ChoiceEncodeInfo   ChoiceEncodeInfo;
    typedef baea_SerializableObjectProxy_ChoiceDecodeInfo   ChoiceDecodeInfo;
    typedef baea_SerializableObjectProxy_SequenceInfo       SequenceInfo;
    typedef baea_SerializableObjectProxy_ArrayEncodeInfo    ArrayEncodeInfo;
    typedef baea_SerializableObjectProxy_ArrayDecodeInfo    ArrayDecodeInfo;
    typedef baea_SerializableObjectProxy_EnumEncodeInfo     EnumEncodeInfo;
    typedef baea_SerializableObjectProxy_EnumDecodeInfo     EnumDecodeInfo;
    typedef baea_SerializableObjectProxy_NullableEncodeInfo NullableEncodeInfo;
    typedef baea_SerializableObjectProxy_NullableDecodeInfo NullableDecodeInfo;
    typedef baea_SerializableObjectProxy_SimplePointer      SimplePointer;

    typedef bdeut_Variant<ChoiceEncodeInfo,
                          ChoiceDecodeInfo,
                          SequenceInfo,
                          ArrayEncodeInfo,
                          ArrayDecodeInfo,
                          EnumEncodeInfo,
                          EnumDecodeInfo,
                          NullableEncodeInfo,
                          NullableDecodeInfo,
                          SimplePointer> ObjectInfo;

    // DATA
    bsls_ObjectBuffer<ObjectInfo> d_objectInfoArena;
                                               // held as an ObjectBuffer to
                                               // avoid the expense of
                                               // default-constructing and
                                               // (especially) destroying the
                                               // variant 'ObjectInfo' object.

    ObjectInfo&                   d_objectInfo;
                                               // the object held in the arena

    void                         *d_object_p;  // meaning depends on the type
                                               // being represented:
                                               // (1) if representing a
                                               // Nullable object for encoding,
                                               // has a value of 0 if the
                                               // object is null, and the
                                               // address of the contained
                                               // element otherwise;
                                               // (2) if representing a Choice
                                               // object for encoding, the
                                               // address of the selected
                                               // element;
                                               // (3) if representing an
                                               // Enumeration for encoding, has
                                               // a value 0;
                                               // (4) if representing an Array
                                               // object for encoding, has a
                                               // value of 0 if the array is
                                               // empty, and the address of the
                                               // first element otherwise;
                                               // (5) in all other cases, the
                                               // address of the proxied object

    bdeat_TypeCategory::Value     d_category;  // dynamic type of proxied value

  private:
    // NOT IMPLEMENTED
    baea_SerializableObjectProxy(const baea_SerializableObjectProxy&);
    baea_SerializableObjectProxy& operator=(
                                          const baea_SerializableObjectProxy&);

    // PRIVATE CLASS METHODS
    template<typename MANIPULATOR>
    static int manipulateContainedElement(
                                    baea_SerializableObjectProxy *proxy,
                                    MANIPULATOR&                  manipulator);
    static int manipulateContainedElement(
           baea_SerializableObjectProxy                          *proxy,
           bdef_Function<int(*)(baea_SerializableObjectProxy*)>&  manipulator);
        // Invoke the specified 'manipulator' on the object represented by the
        // specified 'proxy', creating a
        // 'baea_SerializableObjectProxy_NullableAdapter' to represent it if
        // necessary (see the class-level documentation of that type for
        // further details), and return the result of that invocation.
        // 'MANIPULATOR' shall be a functor providing methods that can be
        // called as if it had the following signature:
        //..
        //  int operator()(baea_SerializableObjectProxy *);
        //  int operator()(baea_SerializableObjectProxy_NullableAdapter *);
        //..
        // IMPLEMENTATION NOTE: see the .cpp file for a discussion of why this
        // method is overloaded.

    template<typename MANIPULATOR>
    static int manipulateContainedElement(
                                       baea_SerializableObjectProxy    *proxy,
                                       MANIPULATOR&                manipulator,
                                       const bdeat_SelectionInfo&  info);
    static int manipulateContainedElement(
              baea_SerializableObjectProxy                            *proxy,
              bdef_Function<int(*)(baea_SerializableObjectProxy*,
                                   const bdeat_SelectionInfo&)>&  manipulator,
              const bdeat_SelectionInfo&                          info);
        // Invoke the specified 'manipulator' on the object represented by the
        // specified 'proxy', and the specified 'info', creating a
        // 'baea_SerializableObjectProxy_NullableAdapter' to represent it if
        // necessary (see the class-level documentation of that type for
        // further details), and return the result of that invocation.
        // 'MANIPULATOR' shall be a functor providing methods that can be
        // called as if it had the following signature:
        //..
        //  int operator()(baea_SerializableObjectProxy *,
        //                 const bdeat_SelectionInfo&)
        //                 const bdeat_SelectionInfo&)
        //..
        // IMPLEMENTATION NOTE: see the .cpp file for a discussion of why this
        // method is overloaded.

    template<typename MANIPULATOR>
    static int manipulateContainedElement(
                                     baea_SerializableObjectProxy *proxy,
                                     MANIPULATOR&                  manipulator,
                                     const bdeat_AttributeInfo&    info);
    static int manipulateContainedElement(
              baea_SerializableObjectProxy                            *proxy,
              bdef_Function<int(*)(baea_SerializableObjectProxy*,
                                   const bdeat_AttributeInfo&)>&  manipulator,
              const bdeat_AttributeInfo&                          info);
        // Invoke the specified 'manipulator' on the object represented by the
        // specified 'proxy', and the specified 'info', creating a
        // 'baea_SerializableObjectProxy_NullableAdapter' to represent it if
        // necessary (see the class-level documentation of that type for
        // further details), and return the result of that invocation.
        // 'MANIPULATOR' shall be a functor providing a method that can be
        // called as if it had the following signature:
        //..
        //  int operator()(baea_SerializableObjectProxy *,
        //                 const bdeat_AttributeInfo&);
        //
        //  int operator()(baea_SerializableObjectProxy_NullableAdapter *,
        //                 const bdeat_AttributeInfo&)
        //..
        // IMPLEMENTATION NOTE: see the .cpp file for a discussion of why this
        // method is overloaded.

    template<typename ACCESSOR>
    static int accessContainedElement(
                                 const baea_SerializableObjectProxy& proxy,
                                 ACCESSOR&                           accessor);
        // Invoke the specified 'accessor' on the object represented by the
        // specified 'proxy', creating a
        // 'baea_SerializableObjectProxy_NullableAdapter' to represent it if
        // necessary (see the class-level documentation of that type for
        // further details), and return the result of that invocation.
        // 'ACCESSOR' shall be a functor providing methods that can be called
        // as if it had the following signatures:
        //..
        //  int operator()(const baea_SerializableObjectProxy&);
        //  int operator()(
        //                const baea_SerializableObjectProxy_NullableAdapter&);
        //..

    template<typename ACCESSOR>
    static int accessContainedElement(
                                  const baea_SerializableObjectProxy& proxy,
                                  ACCESSOR&                           accessor,
                                  const bdeat_SelectionInfo&          info);
        // Invoke the specified 'accessor' on the object represented by the
        // specified 'proxy', and the specified 'info', creating a
        // 'baea_SerializableObjectProxy_NullableAdapter' to represent it if
        // 'ACCESSOR' shall be a functor providing methods that can be called
        // as if it had the following signatures:
        //..
        //  int operator()(const baea_SerializableObjectProxy&,
        //                 const bdeat_SelectionInfo&);
        //  int operator()(const baea_SerializableObjectProxy_NullableAdapter&,
        //                 const bdeat_SelectionInfo&);
        //..

    template<typename ACCESSOR>
    static int accessContainedElement(
                                   const baea_SerializableObjectProxy& proxy,
                                   ACCESSOR&                          accessor,
                                   const bdeat_AttributeInfo&     info);
        // Invoke the specified 'accessor' on the object represented by the
        // specified 'proxy', and the specified 'info', creating a
        // 'baea_SerializableObjectProxy_NullableAdapter' to represent it if
        // necessary (see the class-level documentation of that type for
        // further details), and return the result of that invocation.
        // 'ACCESSOR' shall be a functor providing methods that can be called
        // as if it had the following signature:
        //..
        //  int operator()(const baea_SerializableObjectProxy&,
        //                 const bdeat_AttributeInfo&);
        //  int operator()(const baea_SerializableObjectProxy_NullableAdapter&,
        //                 const bdeat_AttributeInfo&);
        //..

    // PRIVATE ACCESSORS
    bool isValidForEncoding() const;
        // Return 'true' if this proxy represents a 'bdeat'-compliant object so
        // that it can be used encoding, and 'false' otherwise.  Note that if
        // the proxy represents a Sequence, Simple, or byte array object, both
        // 'isValidForEncoding' and 'isValidForDecoding' return 'true'.

    bool isValidForDecoding() const;
        // Return 'true' if this proxy represents a 'bdeat'-compliant object so
        // that it can be used decoding, and 'false' otherwise.  Note that if
        // the proxy represents a Sequence, Simple, or byte array object, both
        // 'isValidForEncoding' and 'isValidForDecoding' return 'true'.

    void loadArrayElementEncodeProxy(
                                    baea_SerializableObjectProxy *proxy,
                                    int                           index) const;
        // Populate the specified 'proxy' for encoding the object at the
        // specified 'index' within the Array object represented by this proxy.
        // The behavior is undefined unless this object represents an Array
        // object for encoding (i.e.,  'loadArrayForEncoding' was called), and
        // that Array object contains at least 'index + 1' elements.

    void loadArrayElementDecodeProxy(
                                    baea_SerializableObjectProxy *proxy,
                                    int                           index) const;
        // Populate the specified 'proxy' for decoding into the object at the
        // specified 'index' within the Array object represented by this proxy.
        // The behavior is undefined unless this object represents an Array
        // object for decoding (i.e.,  'loadArrayForDecoding' was called), and
        // that Array object contains at least 'index + 1' elements.

    int loadSequenceElementProxy(
                               baea_SerializableObjectProxy  *proxy,
                               const bdeat_AttributeInfo    **info,
                               int                            elementId) const;
        // Populate the specified 'proxy' to represent the element with the
        // specified 'elementId', and load, into the specified 'info', the
        // address of the 'bdeat_AttributeInfo' for that element if the
        // Sequence object (represented by this proxy) contains such an
        // element.  Return 0 on success, and a non-zero value otherwise.  The
        // behavior is undefined unless this object represents a Sequence
        // object (i.e., 'loadSequence' was called).

    int loadSequenceElementProxy(
                       baea_SerializableObjectProxy  *proxy,
                       const bdeat_AttributeInfo    **info,
                       const char                    *elementName,
                       int                            elementNameLength) const;
        // Populate the specified 'proxy' to represent the element with the
        // specified 'elementName' of the specified 'elementNameLength', and
        // load, into the specified 'info', the address of the
        // 'bdeat_AttributeInfo' for that element if the Sequence object
        // (represented by this proxy) contains such an element.  Return 0 on
        // success, and a non-zero value otherwise.  The behavior is undefined
        // unless this object represents a Sequence object (i.e.,
        // 'loadSequence' was called).

  public:
    // CREATORS
    baea_SerializableObjectProxy();
        // Create a 'baea_SerializableObjectProxy' object that does not refer
        // to any object.  The object must be populated before being used for
        // encoding or decoding.  Note that clients are encouraged to use
        // utilities provided in the 'baea_serializableobjectproxyutil'
        // component to populate the 'baea_SerializableObjectProxy' object.

    // MANIPULATORS
    void resize(size_t newSize);
        // Change the size of the Array object represented by this proxy to the
        // specified 'newSize'.  The behavior is undefined unless this proxy
        // represents an Array object for decoding (i.e.,
        // 'loadArrayForDecoding' was called).

    int choiceMakeSelection(int selectionId);
        // Change the selection of the Choice object represented by this proxy
        // to the selection having the specified 'selectionId' by invoking the
        // chooser function supplied to the 'loadChoiceForDecoding' method, and
        // return the result of that invocation.  The behavior is undefined
        // unless this object represents a Choice object for decoding (i.e.,
        // 'loadChoiceForDecoding' was called).

    int choiceMakeSelection(const char *selectionName,
                            int         selectionNameLength);
        // Change the selection of the Choice object represented by this proxy
        // to the selection having the specified 'selectionName' having the
        // specified 'selectionNameLength' by invoking the chooser function
        // supplied to the 'loadChoiceForDecoding' method if the object
        // (represented by this proxy) contains such selection.  Return the
        // result of that invocation on success, and a non-zero value
        // otherwise.  The behavior is undefined unless this object represents
        // a Choice object for decoding (i.e., 'loadChoiceForDecoding' was
        // called).

    template<typename MANIPULATOR>
    int choiceManipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on a proxy object (populated by
        // the 'loader' function supplied to the 'loadChoiceForDecoding'
        // method) representing the current selection of the Choice object
        // represented by this proxy.  'MANIPULATOR' shall be a functor 
        // providing methods that can be called as if it had the following
        // signatures:
        //..
        //  int operator()(baea_SerializableObjectProxy *,
        //                 const bdeat_SelectionInfo&);
        //  int operator()(baea_SerializableObjectProxy_NullableAdapter *,
        //                 const bdeat_SelectionInfo&);
        //..
        // Return -1 if this object represents an unselected Choice, and the 
        // value returned by 'manipulator' otherwise.  The behavior is 
        // undefined unless this object represents a Choice object for decoding.

    template<typename MANIPULATOR>
    int arrayManipulateElement(MANIPULATOR& manipulator, int index);
        // Invoke the specified 'manipulator' on a proxy object (populated by
        // the 'loader' function supplied to the 'loadArrayForDecoding' method)
        // representing the element at the specified 'index' in the Array
        // object represented by this proxy, and return the result of that
        // invocation.  'MANIPULATOR' shall be a functor providing methods that
        // can be called as if it had the following signatures:
        //..
        //  int operator()(const baea_SerializableObjectProxy&);
        //  int operator()(
        //                const baea_SerializableObjectProxy_NullableAdapter&);
        //..
        // The behavior is undefined unless this object represents an Array
        // object for decoding (i.e., 'loadArrayForDecoding' was called),
        // 'false == isByteArrayValue()', and '0 <= index < size()'.

    template<typename MANIPULATOR>
    int sequenceManipulateAttribute(MANIPULATOR& manipulator, int attributeId);
        // Invoke the specified 'manipulator' on a proxy object (populated by
        // the loader function supplied to the 'loadSequence' method)
        // representing the element with the specified 'attributeId' if the
        // Sequence object (represented by this proxy) contains such an
        // element.  Return the result of that invocation on success, and a
        // non-zero value otherwise.  'MANIPULATOR' shall be a functor
        // providing methods that can be called as if it had the following
        // signatures:
        //..
        //  int operator()(baea_SerializableObjectProxy *,
        //                 const bdeat_AttributeInfo&);
        //  int operator()(baea_SerializableObjectProxy_NullableAdapter *,
        //                 const bdeat_AttributeInfo&);
        //..
        // The behavior is undefined unless this object represents a Sequence
        // object (i.e., 'loadSequence' was called).

    template<typename MANIPULATOR>
    int sequenceManipulateAttribute(MANIPULATOR&  manipulator,
                                    const char   *attributeName,
                                    int           nameLength);
        // Invoke the specified 'manipulator' on a proxy object (populated by
        // the loader function supplied to the 'loadSequence' method)
        // representing the element with the specified 'attributeName' of the
        // specified 'nameLength' if the Sequence object (represented by this
        // proxy) contain such an element.  Return the result of that
        // invocation on success, and a non-zero value otherwise.
        // 'MANIPULATOR' shall be a functor providing methods that can be
        // called as if it had the following signatures:
        //..
        //  int operator()(baea_SerializableObjectProxy *,
        //                 const bdeat_AttributeInfo&);
        //  int operator()(baea_SerializableObjectProxy_NullableAdapter *,
        //                 const bdeat_AttributeInfo&)
        //..
        // The behavior is undefined unless this object represents a Sequence
        // object (i.e., 'loadSequence' was called).

    template<typename MANIPULATOR>
    int sequenceManipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the proxy object for each
        // element in the sequence referred to through this Sequence proxy
        // object, where each sub-proxy object in the sequence has been
        // populated by the 'loader' function supplied to this proxy object's
        // 'loadSequence' method.  Return the first non-zero value obtained by
        // invoking 'manipulator' on each sub-proxy object in the sequence, and
        // return 0 otherwise.  'MANIPULATOR' shall be a functor providing a
        // method that can be called as if it had the following signature:
        //..
        //  int operator()(baea_SerializableObjectProxy *,
        //                 const bdeat_AttributeInfo&);
        //  int operator()(baea_SerializableObjectProxy_NullableAdapter *,
        //                 const bdeat_AttributeInfo&)
        //..
        // The behavior is undefined unless this object represents a Sequence
        // object (i.e., 'loadSequence' was called).

    template<typename MANIPULATOR>
    int manipulateSimple(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the Simple object represented
        // by this proxy, and return the result of that invocation.
        // 'MANIPULATOR' shall be a functor providing a method that can be
        // called as if it had the following signature:
        //..
        // template <class TYPE>
        // int operator()(TYPE *, const bdeat_TypeCategory::Simple&)
        //..
        // The behavior in undefined unless this object represents a Simple
        // object (i.e., 'loadSimple' was called).

    template<typename MANIPULATOR>
    int manipulateNullable(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the (non-null) value contained
        // in the Nullable object (represented by this proxy), and return the
        // result of that invocation.  'MANIPULATOR' shall be a functor
        // providing a method that can be called as if it had the following
        // signature:
        //..
        //  template <class TYPE, class CATEGORY>
        //  int operator()(TYPE *, CATEGORY)
        //..
        // The behavior is undefined unless this object represents a non-null
        // Nullable object for decoding.

    int enumFromInt(int value);
        // Set the Enumeration object represented by this proxy to the
        // specified 'value' by invoking the 'intSetter' supplied to the
        // 'loadEnumerationForDecoding' method, and return the result of the
        // invocation.  The behavior is undefined unless this object represents
        // an Enumeration object for decoding.

    int enumFromString(const char *stringValue, int stringLength);
        // Set the Enumeration object represented by this proxy to the
        // specified 'stringValue' of the specified 'stringLength' by invoking
        // the 'stringSetter' supplied to the 'loadEnumerationForDecoding'
        // method, and return the result of the invocation.  The behavior is
        // undefined unless this proxy represents an Enumeration value for
        // decoding.

    void makeValue();
        // Invoke 'makeValue' on the Nullable object represented by this proxy.
        // The behavior is undefined unless this proxy represents a Nullable
        // value of type for decoding.  Note that the nullable object this
        // proxy represents is of type 'bdeut_NullableValue' or
        // 'bdeut_NullableAllocatedValue'.

    void loadSimple(char               *value);
    void loadSimple(unsigned char      *value);
    void loadSimple(short              *value);
    void loadSimple(int                *value);
    void loadSimple(bsls_Types::Int64  *value);
    void loadSimple(unsigned short     *value);
    void loadSimple(unsigned int       *value);
    void loadSimple(bsls_Types::Uint64 *value);
    void loadSimple(float              *value);
    void loadSimple(double             *value);
    void loadSimple(bsl::string        *value);
    void loadSimple(bdet_Datetime      *value);
    void loadSimple(bdet_Date          *value);
    void loadSimple(bdet_Time          *value);
    void loadSimple(bool               *value);
    void loadSimple(bdet_DatetimeTz    *value);
    void loadSimple(bdet_DateTz        *value);
    void loadSimple(bdet_TimeTz        *value);
        // Configure this proxy to represent the specified primitive 'value' so
        // that it can be used for encoding or decoding.

    void loadChoiceForEncoding(
                  void                                          *object,
                  const bdeat_SelectionInfo                     *selectionInfo,
                  const char                                    *className,
                  baea_SerializableObjectProxyFunctions::Loader  loader);
        // Configure this proxy to represent a Choice 'object' having the
        // specified 'className', and having the specified 'object' as the
        // current selection, with the information about that selection
        // represented by the specified 'selectionInfo', such that it can be
        // used for encoding; the specified 'loader' function will be used to
        // configure another 'baea_SerializableObjectProxy' object to represent
        // the selection of the Choice object (i.e., 'object').  The behavior
        // is undefined unless 'object' refers the selection of a Choice
        // object.

    void loadChoiceForDecoding(
    int                                                     numSelections,
    void                                                   *object,
    const bdeat_SelectionInfo                              *selectionInfoArray,
    baea_SerializableObjectProxyFunctions::SelectionLoader  loader,
    baea_SerializableObjectProxyFunctions::Chooser          chooser);
        // Configure this proxy to represent the specified Choice 'object',
        // containing the specified 'numSelections' that are described by the
        // specified 'selectionInfoArray', so that it can be used for decoding;
        // the specified 'loader' function will be used to configure another
        // 'baea_SerializableObjectProxy' object to represent the current
        // selection of the Choice object, and the specified 'chooser' function
        // will be used to change the selection of 'choice'.  The behavior is
        // undefined unless 'choice' refers to a Choice object.

    void loadSequence(
           int                                                   numAttributes,
           void                                                 *object,
           const bdeat_AttributeInfo                            *attributeInfo,
           const char                                           *className,
           baea_SerializableObjectProxyFunctions::ElementLoader  loader);
        // Configure this proxy to represent the specified Sequence 'object'
        // having the specified 'className', and containing the specified
        // 'numAttributes' that are described by the specified 'attributeInfo',
        // so that it can be used for encoding or decoding; the specified
        // 'loader' function will be used to configure another
        // 'baea_SerializableObjectProxy' object to represent an element of the
        // Sequence object.

    void loadByteArray(bsl::vector<char> *object);
        // Configure this proxy to represent the specified byte array,
        // 'object', so that it can be use for encoding or decoding.

    void loadArrayForEncoding(
                    int                                            length,
                    int                                            elementSize,
                    void                                          *begin,
                    baea_SerializableObjectProxyFunctions::Loader  loader);
        // Configure this proxy to represent an Array object having the
        // specified 'length', 'elementSize' in bytes, and 'begin' address (0
        // if empty), so that it can be used for encoding; the specified
        // 'loader' function will be used to configure another
        // 'baea_SerializableObjectProxy' to represent an element in the Array
        // object.  The behavior is undefined unless the Array object
        // represented by this proxy is of type 'bsl::vector' (see
        // "Restrictions On Proxied Types" in the @DESCRIPTION section).

    void loadArrayForDecoding(
                   void                                           *object,
                   int                                             length,
                   int                                             elementSize,
                   void                                           *begin,
                   baea_SerializableObjectProxyFunctions::Resizer  resizer,
                   baea_SerializableObjectProxyFunctions::Loader   loader);
        // Configure this proxy to represent the specified Array 'object'
        // having the specified 'length', 'elementSize' in bytes, and 'begin'
        // address (0 if empty) so that it can be used for decoding; the
        // specified 'loader' function will be used to configure another
        // 'baea_SerializableObjectProxy' to represent an element in the Array
        // object; the specified 'resizer' function will be used to change the
        // size of the Array object.  The behavior is undefined unless 'object'
        // refers to a 'bsl::vector' (see "Restrictions On Proxied Types" in
        // the @DESCRIPTION section).

    void loadNullableForEncoding(
                        void                                          *object,
                        baea_SerializableObjectProxyFunctions::Loader  loader);
        // Configure this proxy to represent a Nullable object having the
        // specified contained 'object' so that it can be used for encoding, or
        // having a null value if 'object' is 0; the specified 'loader'
        // function will be used to configure another
        // 'baea_SerializableObjectProxy' to represent the value of the
        // Nullable object, or it is ignored if 'object' is 0.

    void loadNullableForDecoding(
           void                                                 *object,
           baea_SerializableObjectProxyFunctions::Loader         loader,
           baea_SerializableObjectProxyFunctions::ValueMaker     valueMaker,
           baea_SerializableObjectProxyFunctions::ObjectFetcher  valueFetcher);
        // Configure this proxy to represent the specified Nullable 'object' so
        // that it can be used for decoding; the specified 'loader' function
        // will be used to configure another 'baea_SerializableObjectProxy' to
        // represent the *contained* object within the nullable value;
        // specified a 'valueMaker' function to will make 'object' non-null;
        // and the specified 'valueFetcher' function will be used to return the
        // address of the contained object.  The behavior is undefined unless
        // 'object' is the address of an object of type
        // 'bdeut_NullableAllocatedValue' or 'bdeut_NullableValue'.

    void loadEnumerationForEncoding(int                         value,
                                    const bdeat_EnumeratorInfo *infoArray,
                                    int                         infoArraySize);
        // Configure this proxy to represent an Enumeration object having the
        // specified 'value', and described by the specified 'infoArray' of the
        // specified 'infoArraySize', so that it can be used for encoding.

    void loadEnumerationForDecoding(
           void                                                *object,
           baea_SerializableObjectProxyFunctions::IntSetter     intSetter,
           baea_SerializableObjectProxyFunctions::StringSetter  stringSetter,
           const bdeat_EnumeratorInfo                          *infoArray,
           int                                                  infoArraySize);
        // Configure this proxy to represent the specified Enumeration
        // 'object', described by the specified 'infoArray' of the specified
        // 'infoArraySize', so that it can be used for decoding; specify an
        // 'intSetter' function to populate 'object' given an 'int', and
        // specify a 'stringSetter' function to populate 'object' given a
        // string.

    // NO-OP FUNCTIONS FOR INTEGRATION

    void reset();
        // Do nothing.  This method is required by 'bdeat'-based decoders but
        // is frequently unnecessary.  Note that because this method is a
        // no-op, users of 'baea_SerializableObjectProxy' are responsible for
        // resetting the object represented by this proxy to it default value
        // when decoding if needed.

    // ACCESSORS
    const char *className() const;
        // Return the class name of the Choice or Sequence object represented
        // by this proxy, or 0 if this object has another type or the name is
        // unknown.

    bdeat_TypeCategory::Value category() const;
        // Return the category of the type represented by this proxy, or
        // 'bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY' if this proxy has not
        // been populated.

    int selectionId() const;
        // If this object represents a Choice, return the current selection of
        // that Choice; return
        // 'bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID' otherwise.

    bsl::size_t size() const;
        // Return the size of the Array object represented by this proxy.  The
        // behavior is undefined unless this object represents an Array.

    const void *object() const;
        // Return the address provided to the "load" method (e.g.,
        // 'loadSequence') used to populate this proxy.  The meaning of this
        // address depends on the represented type and codec direction
        // (encoding/decoding):
        //: 1 If representing a Nullable object for encoding, return 0 if the
        //:   value if the value is null, otherwise return the address of the
        //:   contained element.
        //: 2 If representing a Choice object for encoding, return the address
        //:   of the selected element.
        //: 3 If representing an Enumeration object for encoding, return 0.
        //: 4 If representing an Array object for encoding, return 0 if the
        //:   array is empty, and the address of the first element otherwise.
        //: 5 In all other cases, return the address of the proxied value.

    bool isByteArrayValue() const;
        // Return 'true' if this object represents a 'bsl::vector<char>' for
        // encoding or decoding, and 'false' otherwise.

    bool isNull() const;
        // Return 'true' if this object represents a Nullable value that is
        // currently null, and 'false' otherwise.  The behavior is undefined
        // unless this object represent a Nullable value for encoding or
        // decoding.

    bool choiceHasSelection(const char *selectionName,
                            int         selectionNameLength) const;
        // Return 'true' if the Choice object represented by this proxy
        // contains a selection with the specified 'selectionName' of the
        // specified 'selectionNameLength', and 'false' otherwise.  The
        // behavior is undefined unless this proxy represent a Choice object
        // for decoding.

    bool choiceHasSelection(int selectionId) const;
        // Return 'true' if the Choice object represented by this proxy
        // contains a selection with the specified 'selectionId', and 'false'
        // otherwise.  The behavior is undefined unless this proxy represents
        // a Choice object for decoding.

    bool sequenceHasAttribute(const char *name, int nameLength) const;
        // Return 'true' if the Sequence object represented by this proxy
        // contains an attribute with the specified 'name' of the specified
        // 'nameLength', and 'false' otherwise.  The behavior is undefined
        // unless this proxy represents a Sequence object.

    bool sequenceHasAttribute(int attributeId) const;
        // Return 'true' if the Sequence object represented by this proxy
        // contains an attribute with the specified 'attributeId', and 'false'
        // otherwise.  The behavior is undefined unless this proxy represents
        // a Sequence object.

    template<typename ACCESSOR>
    int choiceAccessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on a proxy object (populated by the
        // 'loader' function supplied to the 'loadChoiceForEncoding' method)
        // representing the current selection of the Choice object represented
        // by this proxy, and return the result of that invocation.  'ACCESSOR'
        // shall be a functor providing methods that can be called as if it had
        // the following signatures:
        //..
        //  int operator()(const baea_SerializableObjectProxy&,
        //                 const bdeat_SelectionInfo&));
        //  int operator()(const baea_SerializableObjectProxy_NullableAdapter&,
        //                 const bdeat_SelectionInfo&));
        //..
        // The behavior is undefined unless this object represents a Choice
        // object for encoding.

    template<typename ACCESSOR>
    int sequenceAccessAttribute(ACCESSOR& accessor, int attributeId) const;
        // Invoke the specified 'accessor' on a proxy object (populated by the
        // loader function supplied to the 'loadSequence' method) representing
        // the element with the specified 'attributeId' if the Sequence object
        // (represented by this proxy) contains such an element; return the
        // result of that invocation on success, and a non-zero value
        // otherwise.  'ACCESSOR' shall be a functor providing methods that can
        // be called as if it had the following signatures:
        //..
        //  int operator()(const baea_SerializableObjectProxy&,
        //                 const bdeat_AttributeInfo&);
        //  int operator()(const baea_SerializableObjectProxy_NullableAdapter&,
        //                 const bdeat_AttributeInfo&);
        //..
        // The behavior is undefined unless this object represents a Sequence.

    template<typename ACCESSOR>
    int sequenceAccessAttribute(ACCESSOR&   accessor,
                                const char *attributeName,
                                int         nameLength) const;
        // Invoke the specified 'accessor' on a proxy object (populated by the
        // loader function supplied to the 'loadSequence' method) representing
        // the element with the specified 'attributeName' of the specified
        // 'nameLength' if the Sequence object (represented by this proxy)
        // contain such an element; return the result of that invocation on
        // success, and a non-zero value otherwise.  'ACCESSOR' shall be a
        // functor providing methods that can be called as if it had the
        // following signatures:
        //..
        //  int operator()(const baea_SerializableObjectProxy&,
        //                 const bdeat_AttributeInfo&);
        //  int operator()(const baea_SerializableObjectProxy_NullableAdapter&,
        //                 const bdeat_AttributeInfo&);
        //..
        // The behavior is undefined unless this object represents a Sequence.

    template<typename ACCESSOR>
    int sequenceAccessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on proxy objects (populated by
        // the 'loader' function supplied to the 'loadSequence' method), each
        // representing an attribute in the Sequence object represented by this
        // proxy .  If any invocation returns a non-zero value, return the
        // result of that invocation; return 0 after 'manipulator' return 0 on
        // all attributes otherwise.  'ACCESSOR' shall be a functor
        // providing a method that can be called as if it had the following
        // signature:
        //..
        //  int operator()(const baea_SerializableObjectProxy&,
        //                 const bdeat_AttributeInfo&);
        //
        //  int operator()(const baea_SerializableObjectProxy_NullableAdapter&,
        //                 const bdeat_AttributeInfo&);
        //..
        // The behavior is undefined unless this object represents a sequence.

    template<typename ACCESSOR>
    int accessNullable(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the (non-null) value contained in
        // the Nullable object represented by this proxy, and return the
        // result of that invocation.  'ACCESSOR' shall be a functor
        // providing a method that can be called as if it had the following
        // signature:
        //..
        //  template <class TYPE, class CATEGORY>
        //  int operator()(const TYPE&, TYPE_CATEGORY)
        //..
        // The behavior is undefined unless this object represents a non-null
        // Nullable object for encoding.

    template<typename ACCESSOR>
    int accessSimple(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the Simple object represented by
        // this object, and return the result of the invocation.  'ACCESSOR'
        // shall be a functor providing a method that can be called as if it
        // had the following signature:
        //..
        // template <class TYPE>
        // int operator()(const TYPE&, bdeat_TypeCategory::Simple)
        //..
        // The behavior in undefined unless this object represents a Simple
        // value.

    template<typename ACCESSOR>
    int arrayAccessElement(ACCESSOR& accessor, int index) const;
        // Invoke the specified 'accessor' on a proxy object (populated by the
        // 'loader' function supplied to the 'loadArrayForDecoding' method)
        // representing the element at the specified 'index' in the Array
        // object represented by this proxy, and return the result of that
        // invocation.  'ACCESSOR' shall be a functor providing methods that
        // can be called as if it had the following signatures:
        //..
        //  int operator()(const baea_SerializableObjectProxy&);
        //  int operator()(
        //                const baea_SerializableObjectProxy_NullableAdapter&);
        //..
        // The behavior is undefined unless this object represents a vector for
        // encoding, 'false == isByteArrayValue()', and '0 <= index < size()'.

    int enumToInt() const;
        // Return the integer representation of the Enumeration object
        // represented by this proxy.  The behavior is undefined unless this
        // proxy represents an Enumeration object for encoding.

    const char *enumToString() const;
        // Return the string representation of the Enumeration represented by
        // this proxy.  The behavior is undefined unless this proxy
        // represents an Enumeration object for encoding.
};

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

              // ----------------------------------------------------
              // struct baea_SerializableObjectProxy_ChoiceEncodeInfo
              // ----------------------------------------------------

// CREATORS
inline
baea_SerializableObjectProxy_ChoiceEncodeInfo
                               ::baea_SerializableObjectProxy_ChoiceEncodeInfo(
                                      const bdeat_SelectionInfo *selectionInfo,
                                      Functions::Loader          accessor,
                                      const char                *className)
: d_selectionInfo_p(selectionInfo)
, d_loader(accessor)
, d_className_p(className)
{
}

              // ----------------------------------------------------
              // struct baea_SerializableObjectProxy_ChoiceDecodeInfo
              // ----------------------------------------------------

// CREATORS
inline
baea_SerializableObjectProxy_ChoiceDecodeInfo
                               ::baea_SerializableObjectProxy_ChoiceDecodeInfo(
                                int                         numSelections,
                                const bdeat_SelectionInfo  *selectionInfoArray,
                                Functions::SelectionLoader  loader,
                                Functions::Chooser          chooser)
: d_numSelections(numSelections)
, d_selectionInfoArray_p(selectionInfoArray)
, d_currentSelection(bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID)
, d_loader(loader)
, d_chooser(chooser)
{
}

              // ---------------------------------------------------
              // struct baea_SerializableObjectProxy_ArrayEncodeInfo
              // ---------------------------------------------------

// CREATORS
inline
baea_SerializableObjectProxy_ArrayEncodeInfo
  ::baea_SerializableObjectProxy_ArrayEncodeInfo(int               length,
                                                 int               elementSize,
                                                 Functions::Loader loader)
: d_length(length)
, d_elementSize(elementSize)
, d_loader(loader)
{
}

              // ---------------------------------------------------
              // struct baea_SerializableObjectProxy_ArrayDecodeInfo
              // ---------------------------------------------------

// CREATORS
inline
baea_SerializableObjectProxy_ArrayDecodeInfo
::baea_SerializableObjectProxy_ArrayDecodeInfo(int                 length,
                                               int                 elementSize,
                                               void               *begin,
                                               Functions::Resizer  resizer,
                                               Functions::Loader   loader)
: d_length(length)
, d_elementSize(elementSize)
, d_begin(begin)
, d_resizer(resizer)
, d_loader(loader)
{
}

                // ------------------------------------------------
                // struct baea_SerializableObjectProxy_SequenceInfo
                // ------------------------------------------------

// CREATORS
inline
baea_SerializableObjectProxy_SequenceInfo
                                   ::baea_SerializableObjectProxy_SequenceInfo(
                                      int                        numAttributes,
                                      const bdeat_AttributeInfo *attributeInfo,
                                      const char                *className,
                                      Functions::ElementLoader   loader)
: d_numAttributes(numAttributes)
, d_attributeInfo_p(attributeInfo)
, d_loader(loader)
, d_className_p(className)
{
}

                       // ----------------------------------
                       // class baea_SerializableObjectProxy
                       // ----------------------------------

// PRIVATE CLASS METHODS
template<typename MANIPULATOR>
int baea_SerializableObjectProxy::manipulateContainedElement(
                                     baea_SerializableObjectProxy *proxy,
                                     MANIPULATOR&                  manipulator)
{
    if (proxy->category() ==
                           bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY) {
        baea_SerializableObjectProxy_NullableAdapter adapter = { proxy };
        return manipulator(&adapter);                                 // RETURN
    }
    else {
        return manipulator(proxy);                                    // RETURN
    }
}

template<typename MANIPULATOR>
int baea_SerializableObjectProxy::manipulateContainedElement(
                                     baea_SerializableObjectProxy *proxy,
                                     MANIPULATOR&                  manipulator,
                                     const bdeat_SelectionInfo&    info)
{
    if (proxy->category() ==
                           bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY) {
        baea_SerializableObjectProxy_NullableAdapter adapter = { proxy };
        return manipulator(&adapter, info);                           // RETURN
    }
    else {
        return manipulator(proxy, info);                              // RETURN
    }
}

template<typename MANIPULATOR>
int baea_SerializableObjectProxy::manipulateContainedElement(
                                     baea_SerializableObjectProxy *proxy,
                                     MANIPULATOR&                  manipulator,
                                     const bdeat_AttributeInfo&    info)
{
    if (proxy->category() ==
                           bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY) {
        baea_SerializableObjectProxy_NullableAdapter adapter = { proxy };
        return manipulator(&adapter, info);                           // RETURN
    }
    else {
        return manipulator(proxy, info);                              // RETURN
    }
}

template<typename ACCESSOR>
int baea_SerializableObjectProxy::accessContainedElement(
                                  const baea_SerializableObjectProxy& proxy,
                                  ACCESSOR&                           accessor)
{
    if (proxy.category() ==
                           bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY) {
        baea_SerializableObjectProxy_NullableAdapter adapter = {
            const_cast<baea_SerializableObjectProxy*>(&proxy)
        };
        return accessor(adapter);                                     // RETURN
    }
    else {
        return accessor(proxy);                                       // RETURN
    }
}

template<typename ACCESSOR>
int baea_SerializableObjectProxy::accessContainedElement(
                                  const baea_SerializableObjectProxy& proxy,
                                  ACCESSOR&                           accessor,
                                  const bdeat_SelectionInfo&          info)
{
    if (proxy.category() ==
                           bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY) {
        baea_SerializableObjectProxy_NullableAdapter adapter = {
            const_cast<baea_SerializableObjectProxy*>(&proxy)
        };
        return accessor(adapter, info);                               // RETURN
    }
    else {
        return accessor(proxy, info);                                 // RETURN
    }
}

template<typename ACCESSOR>
int baea_SerializableObjectProxy::accessContainedElement(
                                  const baea_SerializableObjectProxy& proxy,
                                  ACCESSOR&                           accessor,
                                  const bdeat_AttributeInfo&          info)
{
    if (proxy.category() ==
                           bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY) {
        baea_SerializableObjectProxy_NullableAdapter adapter = {
            const_cast<baea_SerializableObjectProxy*>(&proxy)
        };
        return accessor(adapter, info);                               // RETURN
    }
    else {
        return accessor(proxy, info);                                 // RETURN
    }
}

// PRIVATE ACCESSORS
inline
bool baea_SerializableObjectProxy::isValidForEncoding() const
{
    return isByteArrayValue()
        || d_objectInfo.is<ChoiceEncodeInfo>()
        || d_objectInfo.is<SequenceInfo>()
        || d_objectInfo.is<ArrayEncodeInfo>()
        || d_objectInfo.is<EnumEncodeInfo>()
        || d_objectInfo.is<NullableEncodeInfo>()
        || d_objectInfo.is<SimplePointer>();
}

inline
bool baea_SerializableObjectProxy::isValidForDecoding() const
{
    return isByteArrayValue()
        || d_objectInfo.is<ChoiceDecodeInfo>()
        || d_objectInfo.is<SequenceInfo>()
        || d_objectInfo.is<ArrayDecodeInfo>()
        || d_objectInfo.is<EnumDecodeInfo>()
        || d_objectInfo.is<NullableDecodeInfo>()
        || d_objectInfo.is<SimplePointer>();
}

// CREATORS
inline
baea_SerializableObjectProxy::baea_SerializableObjectProxy()
: d_objectInfo(d_objectInfoArena.object())
, d_object_p(0)
, d_category(bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY) // 0 ("invalid")
{
}

// MANIPULATORS
template<typename MANIPULATOR>
int baea_SerializableObjectProxy::choiceManipulateSelection(
                                                      MANIPULATOR& manipulator)
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<ChoiceDecodeInfo>());

    baea_SerializableObjectProxy selectionProxy;
    const ChoiceDecodeInfo& info = d_objectInfo.the<ChoiceDecodeInfo>();
    const bdeat_SelectionInfo *selectionInfoPtr;

    info.d_loader(&selectionProxy, d_object_p, &selectionInfoPtr);

    // if there is no selection, 'selectionProxy' will have the default
    // DYNAMIC category 
    if (bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY != 
        selectionProxy.category()) 
    {
        return manipulateContainedElement(&selectionProxy,
                                          manipulator,
                                          *selectionInfoPtr);
    }
    return -1;
}

template<typename MANIPULATOR>
int baea_SerializableObjectProxy::arrayManipulateElement(
                                                      MANIPULATOR& manipulator,
                                                      int          index)
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<ArrayDecodeInfo>());
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < static_cast<int>(size()));

    baea_SerializableObjectProxy elementProxy;
    loadArrayElementDecodeProxy(&elementProxy, index);

    return manipulateContainedElement(&elementProxy, manipulator);
}

template<typename MANIPULATOR>
int baea_SerializableObjectProxy::sequenceManipulateAttribute(
                                                      MANIPULATOR& manipulator,
                                                      int          id)
{
    baea_SerializableObjectProxy elementProxy;
    const bdeat_AttributeInfo *info;
    if (0 == loadSequenceElementProxy(&elementProxy, &info, id)) {
        BSLS_ASSERT_SAFE(elementProxy.isValidForDecoding());

        return manipulateContainedElement(&elementProxy, manipulator, *info);
                                                                      // RETURN
    }
    return -1;
}

template<typename MANIPULATOR>
int baea_SerializableObjectProxy::sequenceManipulateAttribute(
                                                   MANIPULATOR&  manipulator,
                                                   const char   *attributeName,
                                                   int           nameLength)
{
    baea_SerializableObjectProxy elementProxy;
    const bdeat_AttributeInfo *info;
    if (0 == loadSequenceElementProxy(&elementProxy,
                                      &info,
                                      attributeName,
                                      nameLength)) {
        BSLS_ASSERT_SAFE(elementProxy.isValidForDecoding());

        return manipulateContainedElement(&elementProxy, manipulator, *info);
                                                                      // RETURN
    }
    return -1;
}

template<typename MANIPULATOR>
int baea_SerializableObjectProxy::sequenceManipulateAttributes(
                                                      MANIPULATOR& manipulator)
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<SequenceInfo>());

    baea_SerializableObjectProxy elementProxy;
    const SequenceInfo& info = d_objectInfo.the<SequenceInfo>();

    for(int i = 0; i < info.d_numAttributes; ++i)
    {
        info.d_loader(&elementProxy, *this,
                      info.d_attributeInfo_p[i].d_id);

        BSLS_ASSERT_SAFE(elementProxy.isValidForDecoding());

        int rc = manipulateContainedElement(&elementProxy,
                                            manipulator,
                                            info.d_attributeInfo_p[i]);
        if (0 != rc) {
            return rc;                                                // RETURN
        }
    }
    return 0;
}

template <typename MANIPULATOR>
int baea_SerializableObjectProxy::manipulateSimple(MANIPULATOR& manipulator)
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<SimplePointer>());

    const SimplePointer& simplePtr = d_objectInfo.the<SimplePointer>();

    switch (simplePtr.d_type) {
      case SimplePointer::TYPE_CHAR: {
        return manipulator((char*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_UCHAR: {
        return manipulator((unsigned char*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_SHORT: {
        return manipulator((short*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_INT: {
        return manipulator((int*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_INT64: {
        return manipulator((bsls_Types::Int64*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_USHORT: {
        return manipulator((unsigned short*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_UINT: {
        return manipulator((unsigned int*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_UINT64: {
        return manipulator((bsls_Types::Uint64*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_FLOAT: {
        return manipulator((float*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_DOUBLE: {
        return manipulator((double*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_STRING: {
        return manipulator((bsl::string*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_DATETIME: {
        return manipulator((bdet_Datetime*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_DATE: {
        return manipulator((bdet_Date*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_TIME: {
        return manipulator((bdet_Time*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_BOOL: {
        return manipulator((bool*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_DATETIMETZ: {
        return manipulator((bdet_DatetimeTz*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_DATETZ: {
        return manipulator((bdet_DateTz*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
      case SimplePointer::TYPE_TIMETZ: {
        return manipulator((bdet_TimeTz*)d_object_p,
                           bdeat_TypeCategory::Simple());             // RETURN
      }
    }
    return -1;
}

template<typename MANIPULATOR>
int baea_SerializableObjectProxy::manipulateNullable(MANIPULATOR& manipulator)
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<NullableDecodeInfo>());

    const NullableDecodeInfo& info = d_objectInfo.the<NullableDecodeInfo>();

    baea_SerializableObjectProxy proxy;
    info.d_loader(&proxy, info.d_fetcher(d_object_p));

    return bdeat_TypeCategoryUtil::manipulateByCategory(&proxy, manipulator);
}

// NO-OP FUNCTIONS FOR INTEGRATION

inline
void baea_SerializableObjectProxy::reset()
{
}

// ACCESSORS
inline
bdeat_TypeCategory::Value baea_SerializableObjectProxy::category() const
{
    return d_category;
}

inline
const void *baea_SerializableObjectProxy::object() const
{
    return d_object_p;
}

inline
bool baea_SerializableObjectProxy::isByteArrayValue() const
{
    return d_objectInfo.is<ArrayEncodeInfo>()
        && d_objectInfo.the<ArrayEncodeInfo>().d_loader == 0;
}

template<typename ACCESSOR>
int baea_SerializableObjectProxy::choiceAccessSelection(
                                                      ACCESSOR& accessor) const
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<ChoiceEncodeInfo>());

    baea_SerializableObjectProxy selectionProxy;
    const ChoiceEncodeInfo& info = d_objectInfo.the<ChoiceEncodeInfo>();
    info.d_loader(&selectionProxy, d_object_p);

    return accessContainedElement(selectionProxy,
                                  accessor,
                                  *info.d_selectionInfo_p);
}

template<typename ACCESSOR>
int baea_SerializableObjectProxy::sequenceAccessAttribute(ACCESSOR& accessor,
                                                     int       id) const
{
    baea_SerializableObjectProxy elementProxy;
    const bdeat_AttributeInfo *info;
    if (0 == loadSequenceElementProxy(&elementProxy, &info, id)) {
        BSLS_ASSERT_SAFE(elementProxy.isValidForEncoding());

        return accessContainedElement(elementProxy, accessor, *info);
                                                                      // RETURN
    }
    return -1;
}

template<typename ACCESSOR>
int baea_SerializableObjectProxy::sequenceAccessAttribute(
                                                  ACCESSOR&   accessor,
                                                  const char *attributeName,
                                                  int         nameLength) const
{
    baea_SerializableObjectProxy elementProxy;
    const bdeat_AttributeInfo *info;
    if (0 == loadSequenceElementProxy(&elementProxy,
                                      &info,
                                      attributeName,
                                      nameLength)) {
        BSLS_ASSERT_SAFE(elementProxy.isValidForEncoding());

        return accessContainedElement(elementProxy, accessor, *info);
                                                                      // RETURN
    }
    return -1;
}

template<typename ACCESSOR>
int baea_SerializableObjectProxy::sequenceAccessAttributes(
                                                      ACCESSOR& accessor) const
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<SequenceInfo>());

    baea_SerializableObjectProxy elementProxy;
    const SequenceInfo& info = d_objectInfo.the<SequenceInfo>();

    for(int i = 0; i < info.d_numAttributes; ++i)
    {
        info.d_loader(&elementProxy, *this,
                      info.d_attributeInfo_p[i].d_id);

        BSLS_ASSERT_SAFE(elementProxy.isValidForEncoding());

        int rc = accessContainedElement(elementProxy,
                                        accessor,
                                        info.d_attributeInfo_p[i]);
        if (0 != rc) {
            return rc;                                                // RETURN
        }
    }
    return 0;
}

template<typename ACCESSOR>
int baea_SerializableObjectProxy::accessNullable(ACCESSOR& accessor) const
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<NullableEncodeInfo>());
    BSLS_ASSERT_SAFE(d_object_p);

    baea_SerializableObjectProxy proxy;
    d_objectInfo.the<NullableEncodeInfo>()(&proxy,
                                           const_cast<void*>(d_object_p));

    return bdeat_TypeCategoryUtil::accessByCategory(proxy, accessor);
}

template <typename ACCESSOR>
int baea_SerializableObjectProxy::accessSimple(ACCESSOR& accessor) const
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<SimplePointer>());

    const SimplePointer& simplePtr = d_objectInfo.the<SimplePointer>();

    switch (simplePtr.d_type) {
      case SimplePointer::TYPE_CHAR: {
        return accessor(*(char*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_UCHAR: {
        return accessor(*(unsigned char*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_SHORT: {
        return accessor(*(short*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_INT: {
        return accessor(*(int*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_INT64: {
        return accessor(*(bsls_Types::Int64*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_USHORT: {
        return accessor(*(unsigned short*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_UINT: {
        return accessor(*(unsigned int*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_UINT64: {
        return accessor(*(bsls_Types::Uint64*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_FLOAT: {
        return accessor(*(float*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_DOUBLE: {
        return accessor(*(double*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_STRING: {
        return accessor(*(bsl::string*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_DATETIME: {
        return accessor(*(bdet_Datetime*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_DATE: {
        return accessor(*(bdet_Date*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_TIME: {
        return accessor(*(bdet_Time*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_BOOL: {
        return accessor(*(bool*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_DATETIMETZ: {
        return accessor(*(bdet_DatetimeTz*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_DATETZ: {
        return accessor(*(bdet_DateTz*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      case SimplePointer::TYPE_TIMETZ: {
        return accessor(*(bdet_TimeTz*)d_object_p,
                        bdeat_TypeCategory::Simple());                // RETURN
      }
      default: {
        return -1;                                                    // RETURN
      }
    }
    return -1;
}

template<typename ACCESSOR>
int baea_SerializableObjectProxy::arrayAccessElement(ACCESSOR& accessor,
                                                     int       index) const
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<ArrayEncodeInfo>());
    BSLS_ASSERT_SAFE(0 <= index);

    baea_SerializableObjectProxy elementProxy;
    loadArrayElementEncodeProxy(&elementProxy, index);

    return accessContainedElement(elementProxy, accessor);
}

// ============================================================================
//                            Basic Type Traits
// ============================================================================

template <>
struct bdeat_IsBasicChoice<baea_SerializableObjectProxy> : bsl::true_type
{};

template <>
struct bdeat_IsBasicSequence<baea_SerializableObjectProxy> : bsl::true_type
{};

template <>
struct bdeat_IsBasicEnumeration<baea_SerializableObjectProxy> : bsl::true_type
{};

// ============================================================================
//                      'bdeat_typecategory' overloads
// ============================================================================

template<>
struct bdeat_TypeCategoryDeclareDynamic<baea_SerializableObjectProxy> {
    enum { VALUE = 1 };
};

namespace bdeat_TypeCategoryFunctions {

template <>
inline
bdeat_TypeCategory::Value
bdeat_typeCategorySelect<baea_SerializableObjectProxy>(
                                    const baea_SerializableObjectProxy& object)
{
    return object.category();
}

}  // close namespace bdeat_TypeCategoryFunctions

template <typename ACCESSOR>
inline
int bdeat_typeCategoryAccessSimple(
                                  const baea_SerializableObjectProxy& object,
                                  ACCESSOR&                           accessor)
{
    return object.accessSimple(accessor);
}

template <typename MANIPULATOR>
inline
int bdeat_typeCategoryManipulateSimple(
                                     baea_SerializableObjectProxy *object,
                                     MANIPULATOR&                  manipulator)
{
    return object->manipulateSimple(manipulator);
}

template <typename MANIPULATOR>
int bdeat_typeCategoryManipulateArray(
                                     baea_SerializableObjectProxy *object,
                                     MANIPULATOR&                  manipulator)
{
    if (object->isByteArrayValue()) {
        bsl::vector<char>* arrayPtr =
            (bsl::vector<char>*)object->object();
        return manipulator(arrayPtr, bdeat_TypeCategory::Array());    // RETURN
    }
    else {
        return manipulator(object, bdeat_TypeCategory::Array());      // RETURN
    }
}

template <typename ACCESSOR>
int bdeat_typeCategoryAccessArray(
                                  const baea_SerializableObjectProxy& object,
                                  ACCESSOR&                           accessor)
{
    if (object.isByteArrayValue()) {
        const bsl::vector<char>* arrayPtr =
                                   (const bsl::vector<char> *) object.object();
        return accessor(*arrayPtr, bdeat_TypeCategory::Array());      // RETURN
    }
    else {
        return accessor(object, bdeat_TypeCategory::Array());         // RETURN
    }
}

template <typename ACCESSOR>
int bdeat_typeCategoryAccessNullableValue(
                                  const baea_SerializableObjectProxy& object,
                                  ACCESSOR&                           accessor)
{
    if (object.category() == bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY
        && !object.isNull())
    {
        return object.accessNullable(accessor);                       // RETURN
    }
    return -1;
}

template <typename MANIPULATOR>
int bdeat_typeCategoryManipulateNullableValue(
                                     baea_SerializableObjectProxy* object,
                                     MANIPULATOR&                  manipulator)
{
    if (object->category() ==
        bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY)
    {
        BSLS_ASSERT_SAFE(!object->isNull()); // the default imp.  does this too
        return object->manipulateNullable(manipulator);               // RETURN
    }
    return -1;
}

// ============================================================================
//           'bdeat_enumfunctions' overloads and specializations
// ============================================================================
namespace bdeat_EnumFunctions {

// META-FUNCTIONS
bslmf_MetaInt<1> isEnumerationMetaFunction(
                                          const baea_SerializableObjectProxy&);
    // This function can be overloaded to support partial specialization
    // (Sun5.2 compiler is unable to partially specialize the 'struct'
    // below).  Note that this function is has no definition and should not
    // be called at run-time.

template <>
struct IsEnumeration<baea_SerializableObjectProxy> {
    enum { VALUE = 1 };
};

}  // close namespace bdeat_EnumFunctions

inline
void bdeat_enumToInt(int *result, const baea_SerializableObjectProxy& object)
{
    *result = object.enumToInt();
}

inline
int bdeat_enumFromInt(baea_SerializableObjectProxy *result,
                      int                           value)
{
    return result->enumFromInt(value);
}


inline
void bdeat_enumToString(bsl::string                         *result,
                        const baea_SerializableObjectProxy&  object)
{
    *result = object.enumToString();
}

inline
int bdeat_enumFromString(baea_SerializableObjectProxy *result,
                         const char                    *stringValue,
                         int                            stringLength)
{
    return result->enumFromString(stringValue, stringLength);
}

// ============================================================================
//           'bdeat_arrayfunctions' overloads and specializations
// ============================================================================

namespace bdeat_ArrayFunctions {

bslmf_MetaInt<1> isArrayMetaFunction(const baea_SerializableObjectProxy&);

template <>
struct IsArray<baea_SerializableObjectProxy> {
    enum { VALUE = 1 };
};

template <>
struct ElementType<baea_SerializableObjectProxy> {
    typedef baea_SerializableObjectProxy Type;
};

}  // close namespace bdeat_ArrayFunctions

inline
bsl::size_t bdeat_arraySize(const baea_SerializableObjectProxy& object)
{
    return object.size();
}

inline
void bdeat_arrayResize(baea_SerializableObjectProxy* object, int newSize)
{
    object->resize(newSize);
}

template <typename ACCESSOR>
inline
int bdeat_arrayAccessElement(const baea_SerializableObjectProxy& object,
                             ACCESSOR&                           accessor,
                             int                                 index)
{
    return object.arrayAccessElement(accessor, index);
}

template <typename MANIPULATOR>
inline
int bdeat_arrayManipulateElement(baea_SerializableObjectProxy* object,
                                 MANIPULATOR&                  manipulator,
                                 int                           index)
{
    return object->arrayManipulateElement(manipulator, index);
}

// ============================================================================
//           'bdeat_sequencefunctions' overloads and specializations
// ============================================================================

namespace bdeat_SequenceFunctions {

bslmf_MetaInt<1> isSequenceMetaFunction(const baea_SerializableObjectProxy&);

template <>
struct IsSequence<baea_SerializableObjectProxy> {
    enum { VALUE = 1 };
};

}  // close namespace bdeat_SequenceFunctions

template <typename ACCESSOR>
inline
int bdeat_sequenceAccessAttributes(
                                 const baea_SerializableObjectProxy& object,
                                 ACCESSOR&                           accessor)
{
    return object.sequenceAccessAttributes(accessor);
}

template <typename ACCESSOR>
inline
int bdeat_sequenceAccessAttribute(
                              const baea_SerializableObjectProxy& object,
                              ACCESSOR&                           accessor,
                              int                                 attributeId)
{
    return object.sequenceAccessAttribute(accessor, attributeId);
}

template <typename ACCESSOR>
inline
int bdeat_sequenceAccessAttribute(
                           const baea_SerializableObjectProxy&  object,
                           ACCESSOR&                            accessor,
                           const char                          *attributeName,
                           int                                  nameLength)
{
    return object.sequenceAccessAttribute(accessor, attributeName, nameLength);
}

template <typename MANIPULATOR>
inline
int bdeat_sequenceManipulateAttributes(
                                    baea_SerializableObjectProxy *object,
                                    MANIPULATOR&                  manipulator)
{
    return object->sequenceManipulateAttributes(manipulator);
}

template <typename MANIPULATOR>
inline
int bdeat_sequenceManipulateAttribute(
                                    baea_SerializableObjectProxy *object,
                                    MANIPULATOR&                  manipulator,
                                    int                           attributeId)
{
    return object->sequenceManipulateAttribute(manipulator, attributeId);
}

template <typename MANIPULATOR>
inline
int bdeat_sequenceManipulateAttribute(
                                  baea_SerializableObjectProxy *object,
                                  MANIPULATOR&                  manipulator,
                                  const char                   *attributeName,
                                  int                            nameLength)
{
    return object->sequenceManipulateAttribute(manipulator,
                                               attributeName,
                                               nameLength);
}

inline
bool bdeat_sequenceHasAttribute(
                              const baea_SerializableObjectProxy& object,
                              int                                 attributeId)
{
    return object.sequenceHasAttribute(attributeId);
}

inline
bool bdeat_sequenceHasAttribute(
                            const baea_SerializableObjectProxy&  object,
                            const char                          *attributeName,
                            int                                  nameLength)
{
    return object.sequenceHasAttribute(attributeName, nameLength);
}

// ============================================================================
//           'bdeat_nullablevaluefunctions' overloads and specializations
// ============================================================================
namespace bdeat_NullableValueFunctions {

bslmf_MetaInt<1> isNullableValueMetaFunction(
                          const baea_SerializableObjectProxy_NullableAdapter&);

template <>
struct IsNullableValue<baea_SerializableObjectProxy_NullableAdapter> {
    enum { VALUE = 1 };
};

template <>
struct ValueType<baea_SerializableObjectProxy_NullableAdapter> {
    typedef baea_SerializableObjectProxy Type;
};

}  // close namespace bdeat_NullableValueFunctions

inline
bool bdeat_nullableValueIsNull(
                    const baea_SerializableObjectProxy_NullableAdapter& object)
{
    return object.d_proxy_p->isNull();
}

inline
void bdeat_nullableValueMakeValue(
                          baea_SerializableObjectProxy_NullableAdapter *object)
{
    object->d_proxy_p->makeValue();
}

template <typename MANIPULATOR>
inline
int bdeat_nullableValueManipulateValue(
                     baea_SerializableObjectProxy_NullableAdapter *object,
                     MANIPULATOR&                                  manipulator)
{
    return manipulator(object->d_proxy_p);
}

template <typename ACCESSOR>
inline
int bdeat_nullableValueAccessValue(
                 const baea_SerializableObjectProxy_NullableAdapter& object,
                 ACCESSOR&                                           accessor)
{
    return accessor(*object.d_proxy_p);
}

// ============================================================================
//                       'bdeat_typename' overloads
// ============================================================================
namespace bdeat_TypeName_Overloadable {

inline
const char *bdeat_TypeName_className(
                                   const baea_SerializableObjectProxy& object)
     // Return the type name of the type represented by the specified
     // 'object', or 0 if there is no such name.
{
    return object.className();
}

}  // close namespace bdeat_TypeName_Overloadable

// ============================================================================
//           'bdeat_choicefunctions' overloads and specializations
// ============================================================================

namespace bdeat_ChoiceFunctions {
bslmf_MetaInt<1> isChoiceMetaFunction(const baea_SerializableObjectProxy&);

template <>
struct IsChoice<baea_SerializableObjectProxy> {
    enum { VALUE = 1 };
};

}  // close namespace bdeat_ChoiceFunctions

inline
int bdeat_choiceSelectionId(const baea_SerializableObjectProxy& object)
{
    return object.selectionId();
}

template <typename ACCESSOR>
inline
int bdeat_choiceAccessSelection(const baea_SerializableObjectProxy& object,
                                ACCESSOR&                           accessor)
{
    return object.choiceAccessSelection(accessor);
}

inline
bool bdeat_choiceHasSelection(
                     const baea_SerializableObjectProxy&  object,
                     const char                          *selectionName,
                     int                                  selectionNameLength)
{
    return object.choiceHasSelection(selectionName, selectionNameLength);
}

inline
bool bdeat_choiceHasSelection(const baea_SerializableObjectProxy& object,
                              int                                 selectionId)
{
    return object.choiceHasSelection(selectionId);
}

inline
int bdeat_choiceMakeSelection(baea_SerializableObjectProxy *object,
                              int                           selectionId)
{
    return object->choiceMakeSelection(selectionId);
}

inline
int bdeat_choiceMakeSelection(
                            baea_SerializableObjectProxy *object,
                            const char                   *selectionName,
                            int                           selectionNameLength)
{
    return object->choiceMakeSelection(selectionName, selectionNameLength);
}

template <typename MANIPULATOR>
inline
int bdeat_choiceManipulateSelection(baea_SerializableObjectProxy *object,
                                    MANIPULATOR&                  manipulator)
{
    return object->choiceManipulateSelection(manipulator);
}

}  // close namespace BloombergLP

#endif


// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
