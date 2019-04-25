// balber_berdecoderoptions.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balber_berdecoderoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdem_berdecoderoptions_t_cpp,"$Id$ $CSID$")

#include <bdlat_symbolicconverter.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// The component under test implements a value-semantic attribute class that
// provides the options available for configuring a 'balber::BerDecoder'
// object.
//
// Primary Manipulators:
//: o 'setMaxDepth'
//: o 'setTraceLevel'
//: o 'setMaxSequenceSize'
//: o 'setSkipUnknownElements'
//: o 'setDefaultEmptyStrings'
//
// BasicAccessors:
//: o 'maxDepth'
//: o 'traceLevel'
//: o 'maxSequenceSize'
//: o 'skipUnknownElements'
//: o 'defaultEmptyStrings'
//
// This attribute class also provides 'bdlat'-support.
//
// Global Concerns:
//: o The "DefaultEmptyStrings" attribute, introduced in the second version
//:   of the representation, correctly implements the 'bdlat' attribute-info
//:   API.
//: o Prior versions of the 'balber::BerDecoderOptions' representation can be
//:   loaded into the current representation without loss of information.  This
//:   implies the loading operation (through 'bdlat') is backwards-compatible.
// ----------------------------------------------------------------------------
// CREATORS
// [ 1] balber::BerDecoderOptions();
//
// MANIPULATORS
// [ 1] operator=(const balber::BerDecoderOptions& rhs);
// [ 1] void setMaxDepth(int maxDepth);
// [ 1] void setTraceLevel(int traceLevel);
// [ 1] void setMaxSequenceSize(int maxSequenceSize);
// [ 1] void setSkipUnknownElements(bool skipUnknownElements);
// [ 1] void setDefaultEmptyStrings(bool defaultEmptyStrings);
//
// ACCESSORS
// [ 1] const int& maxDepth() const;
// [ 1] const bool& skipUnknownElements() const;
// [ 1] const int& traceLevel() const;
// [ 1] const int& maxSequenceSize() const;
// [ 1] const bool& defaultEmptyStrings() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] CONCERN: The "DefaultEmptyStrings" attribute is "AttributeInfo"-aware
// [ 3] CONCERN: The current format is 'bdlat'-backwards-compatible
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

#define ASSERT_EQ(X,Y) LOOP2_ASSERT(X,Y,(X) == (Y))
#define ASSERT_NE(X,Y) LOOP2_ASSERT(X,Y,(X) != (Y))

// ============================================================================
//                          GLOBAL TYPES FOR TESTING
// ----------------------------------------------------------------------------

typedef balber::BerDecoderOptions Obj;

// ============================================================================
//                         GENERATED CODE FOR TESTING
// ----------------------------------------------------------------------------
//
// The following commented-out XSD can be used to generate the
// 'bdlat'-compatible types used to test backwards-compatibility of different
// versions 'balber::BerDecoderOptions'.
//
// ----------------------------------------------------------------------------
//
// <?xml version="1.0" encoding="UTF-8"?>
// <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"
//            xmlns:bdem="http://bloomberg.com/schemas/bdem"
//            xmlns="http://bloomberg.com/schemas/test_codec"
//            targetNamespace="http://bloomberg.com/schemas/test_codec"
//            elementFormDefault="qualified">
//     <!-- "$Id: $  $CSID:  $  $SCMId:  $" -->
//
//     <xs:annotation>
//         <xs:documentation>
//             This component replicates all historical representations of
//             'balber::BerDecoderOptions' in order to test the backwards
//             compatibility of later representations.
//         </xs:documentation>
//     </xs:annotation>
//
//                        <!-- ========================= -->
//                        <!-- class BerDecoderOptionsV1 -->
//                        <!-- ========================= -->
//
//     <xs:complexType name="BerDecoderOptionsV1">
//         <xs:annotation>
//             <xs:documentation>
//                 The first representation of 'balber::BerDecoderOptions'.
//             </xs:documentation>
//         </xs:annotation>
//
//         <xs:sequence>
//             <xs:element
//                 name='MaxDepth'
//                 type='xs:int'
//                 minOccurs='0'
//                 maxOccurs='1'
//                 default='32'
//                 bdem:allowsDirectManipulation='0'>
//             </xs:element>
//
//             <xs:element
//                 name='SkipUnknownElements'
//                 type='xs:boolean'
//                 minOccurs='0'
//                 maxOccurs='1'
//                 default='true'
//                 bdem:allowsDirectManipulation='0'>
//             </xs:element>
//
//             <xs:element
//                 name='TraceLevel'
//                 type='xs:int'
//                 minOccurs='0'
//                 maxOccurs='1'
//                 default='0'
//                 bdem:allowsDirectManipulation='0'>
//             </xs:element>
//
//             <xs:element
//                 name='MaxSequenceSize'
//                 type='xs:int'
//                 minOccurs='0'
//                 maxOccurs='1'
//                 default='1073741824'
//                 bdem:allowsDirectManipulation='0'>
//             </xs:element>
//         </xs:sequence>
//     </xs:complexType>
//
//                        <!-- ========================= -->
//                        <!-- class BerDecoderOptionsV2 -->
//                        <!-- ========================= -->
//
//     <!--
//     <xs:complexType name="BerDecoderOptionsV2">
//         <xs:annotation>
//             <xs:documentation>
//                 The current representation of 'balber::BerDecoderOptions'.
//             </xs:documentation>
//         </xs:annotation>
//
//         <xs:sequence>
//             <xs:element
//                 name='MaxDepth'
//                 type='xs:int'
//                 minOccurs='0'
//                 maxOccurs='1'
//                 default='32'
//                 bdem:allowsDirectManipulation='0'>
//             </xs:element>
//
//             <xs:element
//                 name='SkipUnknownElements'
//                 type='xs:boolean'
//                 minOccurs='0'
//                 maxOccurs='1'
//                 default='true'
//                 bdem:allowsDirectManipulation='0'>
//             </xs:element>
//
//             <xs:element
//                 name='TraceLevel'
//                 type='xs:int'
//                 minOccurs='0'
//                 maxOccurs='1'
//                 default='0'
//                 bdem:allowsDirectManipulation='0'>
//             </xs:element>
//
//             <xs:element
//                 name='MaxSequenceSize'
//                 type='xs:int'
//                 minOccurs='0'
//                 maxOccurs='1'
//                 default='1073741824'
//                 bdem:allowsDirectManipulation='0'>
//             </xs:element>
//
//             <xs:element
//                 name='DefaultEmptyStrings'
//                 type='xs:boolean'
//                 minOccurs='0'
//                 maxOccurs='1'
//                 default='true'
//                 bdem:allowsDirectManipulation='0'>
//             </xs:element>
//         </xs:sequence>
//     </xs:complexType>
//     -->
//
// </xs:schema>

// ************************ START OF GENERATED CODE **************************
// test_schema.h               *DO NOT EDIT*               @generated -*-C++-*-
#ifndef INCLUDED_TEST_SCHEMA
#define INCLUDED_TEST_SCHEMA

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bsls_assert.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {

namespace test { class BerDecoderOptionsV1; }
namespace test {

                         // =========================
                         // class BerDecoderOptionsV1
                         // =========================

class BerDecoderOptionsV1 {
    // The first representation of 'balber::BerDecoderOptions'.

    // INSTANCE DATA
    int   d_maxDepth;
    int   d_traceLevel;
    int   d_maxSequenceSize;
    bool  d_skipUnknownElements;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_MAX_DEPTH             = 0
      , ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS = 1
      , ATTRIBUTE_ID_TRACE_LEVEL           = 2
      , ATTRIBUTE_ID_MAX_SEQUENCE_SIZE     = 3
    };

    enum {
        NUM_ATTRIBUTES = 4
    };

    enum {
        ATTRIBUTE_INDEX_MAX_DEPTH             = 0
      , ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS = 1
      , ATTRIBUTE_INDEX_TRACE_LEVEL           = 2
      , ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE     = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const int DEFAULT_INITIALIZER_MAX_DEPTH;

    static const bool DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS;

    static const int DEFAULT_INITIALIZER_TRACE_LEVEL;

    static const int DEFAULT_INITIALIZER_MAX_SEQUENCE_SIZE;

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    BerDecoderOptionsV1();
        // Create an object of type 'BerDecoderOptionsV1' having the default
        // value.

    BerDecoderOptionsV1(const BerDecoderOptionsV1& original);
        // Create an object of type 'BerDecoderOptionsV1' having the value of
        // the specified 'original' object.

    ~BerDecoderOptionsV1();
        // Destroy this object.

    // MANIPULATORS
    BerDecoderOptionsV1& operator=(const BerDecoderOptionsV1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void reset();
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'manipulator' (i.e., the invocation that
        // terminated the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'id',
        // supplying 'manipulator' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if 'id' identifies an attribute of this
        // class, and -1 otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of
        // the (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    void setMaxDepth(int value);
        // Set the "MaxDepth" attribute of this object to the specified
        // 'value'.

    void setSkipUnknownElements(bool value);
        // Set the "SkipUnknownElements" attribute of this object to the
        // specified 'value'.

    void setTraceLevel(int value);
        // Set the "TraceLevel" attribute of this object to the specified
        // 'value'.

    void setMaxSequenceSize(int value);
        // Set the "MaxSequenceSize" attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the
        // last invocation of 'accessor' (i.e., the invocation that terminated
        // the sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute
        // of this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.

    int maxDepth() const;
        // Return a reference to the non-modifiable "MaxDepth" attribute of
        // this object.

    bool skipUnknownElements() const;
        // Return a reference to the non-modifiable "SkipUnknownElements"
        // attribute of this object.

    int traceLevel() const;
        // Return a reference to the non-modifiable "TraceLevel" attribute of
        // this object.

    int maxSequenceSize() const;
        // Return a reference to the non-modifiable "MaxSequenceSize" attribute
        // of this object.
};

// FREE OPERATORS
inline
bool operator==(const BerDecoderOptionsV1& lhs, const BerDecoderOptionsV1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const BerDecoderOptionsV1& lhs, const BerDecoderOptionsV1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BerDecoderOptionsV1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(test::BerDecoderOptionsV1)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace test {

                         // -------------------------
                         // class BerDecoderOptionsV1
                         // -------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int BerDecoderOptionsV1::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_traceLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_maxSequenceSize, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class MANIPULATOR>
int BerDecoderOptionsV1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return manipulator(&d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
      } break;
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return manipulator(&d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
      } break;
      case ATTRIBUTE_ID_TRACE_LEVEL: {
        return manipulator(&d_traceLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
      } break;
      case ATTRIBUTE_ID_MAX_SEQUENCE_SIZE: {
        return manipulator(&d_maxSequenceSize, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int BerDecoderOptionsV1::manipulateAttribute(
        MANIPULATOR&  manipulator,
        const char   *name,
        int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
                                         lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

inline
void BerDecoderOptionsV1::setMaxDepth(int value)
{
    d_maxDepth = value;
}

inline
void BerDecoderOptionsV1::setSkipUnknownElements(bool value)
{
    d_skipUnknownElements = value;
}

inline
void BerDecoderOptionsV1::setTraceLevel(int value)
{
    d_traceLevel = value;
}

inline
void BerDecoderOptionsV1::setMaxSequenceSize(int value)
{
    d_maxSequenceSize = value;
}

// ACCESSORS
template <class ACCESSOR>
int BerDecoderOptionsV1::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_traceLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_maxSequenceSize, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
    if (ret) {
        return ret;
    }

    return ret;
}

template <class ACCESSOR>
int BerDecoderOptionsV1::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH: {
        return accessor(d_maxDepth, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH]);
      } break;
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS: {
        return accessor(d_skipUnknownElements, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS]);
      } break;
      case ATTRIBUTE_ID_TRACE_LEVEL: {
        return accessor(d_traceLevel, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL]);
      } break;
      case ATTRIBUTE_ID_MAX_SEQUENCE_SIZE: {
        return accessor(d_maxSequenceSize, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE]);
      } break;
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int BerDecoderOptionsV1::accessAttribute(
        ACCESSOR&   accessor,
        const char *name,
        int         nameLength) const
{
    enum { NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo =
          lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
       return NOT_FOUND;
    }

    return accessAttribute(accessor, attributeInfo->d_id);
}

inline
int BerDecoderOptionsV1::maxDepth() const
{
    return d_maxDepth;
}

inline
bool BerDecoderOptionsV1::skipUnknownElements() const
{
    return d_skipUnknownElements;
}

inline
int BerDecoderOptionsV1::traceLevel() const
{
    return d_traceLevel;
}

inline
int BerDecoderOptionsV1::maxSequenceSize() const
{
    return d_maxSequenceSize;
}

}  // close package namespace
}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_VERSION Tue Feb 26 14:33:51 2019
// USING bas_codegen.pl -m msg -p test --author @Generated --noAggregateConversion --noExternalization balber_berdecoderoptions.t.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2019
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
// test_schema.cpp              *DO NOT EDIT*              @generated -*-C++-*-

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bdlb_print.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace test {

                         // -------------------------
                         // class BerDecoderOptionsV1
                         // -------------------------

// CONSTANTS

const char BerDecoderOptionsV1::CLASS_NAME[] = "BerDecoderOptionsV1";

const int BerDecoderOptionsV1::DEFAULT_INITIALIZER_MAX_DEPTH = 32;

const bool BerDecoderOptionsV1::DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS = true;

const int BerDecoderOptionsV1::DEFAULT_INITIALIZER_TRACE_LEVEL = 0;

const int BerDecoderOptionsV1::DEFAULT_INITIALIZER_MAX_SEQUENCE_SIZE =
        1073741824;

const bdlat_AttributeInfo BerDecoderOptionsV1::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_MAX_DEPTH,
        "MaxDepth",
        sizeof("MaxDepth") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS,
        "SkipUnknownElements",
        sizeof("SkipUnknownElements") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        ATTRIBUTE_ID_TRACE_LEVEL,
        "TraceLevel",
        sizeof("TraceLevel") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    },
    {
        ATTRIBUTE_ID_MAX_SEQUENCE_SIZE,
        "MaxSequenceSize",
        sizeof("MaxSequenceSize") - 1,
        "",
        bdlat_FormattingMode::e_DEC
    }
};

// CLASS METHODS

const bdlat_AttributeInfo *BerDecoderOptionsV1::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 4; ++i) {
        const bdlat_AttributeInfo& attributeInfo =
                    BerDecoderOptionsV1::ATTRIBUTE_INFO_ARRAY[i];

        if (nameLength == attributeInfo.d_nameLength
        &&  0 == bsl::memcmp(attributeInfo.d_name_p, name, nameLength))
        {
            return &attributeInfo;
        }
    }

    return 0;
}

const bdlat_AttributeInfo *BerDecoderOptionsV1::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_MAX_DEPTH:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_DEPTH];
      case ATTRIBUTE_ID_SKIP_UNKNOWN_ELEMENTS:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SKIP_UNKNOWN_ELEMENTS];
      case ATTRIBUTE_ID_TRACE_LEVEL:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TRACE_LEVEL];
      case ATTRIBUTE_ID_MAX_SEQUENCE_SIZE:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_SEQUENCE_SIZE];
      default:
        return 0;
    }
}

// CREATORS

BerDecoderOptionsV1::BerDecoderOptionsV1()
: d_maxDepth(DEFAULT_INITIALIZER_MAX_DEPTH)
, d_traceLevel(DEFAULT_INITIALIZER_TRACE_LEVEL)
, d_maxSequenceSize(DEFAULT_INITIALIZER_MAX_SEQUENCE_SIZE)
, d_skipUnknownElements(DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS)
{
}

BerDecoderOptionsV1::BerDecoderOptionsV1(const BerDecoderOptionsV1& original)
: d_maxDepth(original.d_maxDepth)
, d_traceLevel(original.d_traceLevel)
, d_maxSequenceSize(original.d_maxSequenceSize)
, d_skipUnknownElements(original.d_skipUnknownElements)
{
}

BerDecoderOptionsV1::~BerDecoderOptionsV1()
{
}

// MANIPULATORS

BerDecoderOptionsV1&
BerDecoderOptionsV1::operator=(const BerDecoderOptionsV1& rhs)
{
    if (this != &rhs) {
        d_maxDepth = rhs.d_maxDepth;
        d_skipUnknownElements = rhs.d_skipUnknownElements;
        d_traceLevel = rhs.d_traceLevel;
        d_maxSequenceSize = rhs.d_maxSequenceSize;
    }

    return *this;
}

void BerDecoderOptionsV1::reset()
{
    d_maxDepth = DEFAULT_INITIALIZER_MAX_DEPTH;
    d_skipUnknownElements = DEFAULT_INITIALIZER_SKIP_UNKNOWN_ELEMENTS;
    d_traceLevel = DEFAULT_INITIALIZER_TRACE_LEVEL;
    d_maxSequenceSize = DEFAULT_INITIALIZER_MAX_SEQUENCE_SIZE;
}

// ACCESSORS

bsl::ostream& BerDecoderOptionsV1::print(
        bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("maxDepth", d_maxDepth);
    printer.printAttribute("skipUnknownElements", d_skipUnknownElements);
    printer.printAttribute("traceLevel", d_traceLevel);
    printer.printAttribute("maxSequenceSize", d_maxSequenceSize);
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// GENERATED BY BLP_BAS_CODEGEN_VERSION Tue Feb 26 14:33:51 2019
// USING bas_codegen.pl -m msg -p test --author @Generated --noAggregateConversion --noExternalization balber_berdecoderoptions.t.xsd
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2019
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
// ************************* END OF GENERATED CODE ****************************

// ============================================================================
//                          GLOBAL DATA FOR TESTING
// ----------------------------------------------------------------------------

static bool             verbose = false;
static bool         veryVerbose = false;
static bool     veryVeryVerbose = false;
static bool veryVeryVeryVerbose = false;

                          // =======================
                          // class AttributeAccessor
                          // =======================

template <class ATTRIBUTE_TYPE>
class AttributeAccessor {

  public:
    // TYPES
    typedef ATTRIBUTE_TYPE Attribute;

  private:
    // DATA
    int                 d_attributeId;
    Attribute           d_attribute;
    bdlat_AttributeInfo d_attributeInfo;

  public:
    // CREATORS
    AttributeAccessor(int attributeId)
    : d_attributeId(attributeId)
    , d_attribute()
    , d_attributeInfo()
    {
    }

    // MANIPULATORS
    int operator()(const Attribute&           attribute,
                   const bdlat_AttributeInfo& attributeInfo)
    {
        if (attributeInfo.id() != d_attributeId) {
            return 0;                                                 // RETURN
        }

        d_attribute     = attribute;
        d_attributeInfo = attributeInfo;
        return 0;
    }

    template <class OTHER_ATTRIBUTE_TYPE>
    int operator()(const OTHER_ATTRIBUTE_TYPE&, const bdlat_AttributeInfo&)
    {
        return 0;
    }

    // ACCESSORS
    const Attribute& attribute() const
    {
        return d_attribute;
    }

    const bdlat_AttributeInfo& attributeInfo() const
    {
        return d_attributeInfo;
    }
};

                            // ====================
                            // AttributeManipulator
                            // ====================

template <class ATTRIBUTE_TYPE>
class AttributeManipulator {

  public:
    // TYPES
    typedef ATTRIBUTE_TYPE Attribute;

  private:
    // DATA
    int                  d_attributeId;
    Attribute           *d_attribute_p;
    bdlat_AttributeInfo  d_attributeInfo;

  public:
    // CREATORS
    AttributeManipulator(int attributeId)
    : d_attributeId(attributeId)
    , d_attribute_p()
    , d_attributeInfo()
    {
    }

    // MANIPULATORS
    int operator()(Attribute                  *attribute,
                   const bdlat_AttributeInfo&  attributeInfo)
    {
        if (attributeInfo.id() != d_attributeId) {
            return 0;                                                 // RETURN
        }

        d_attribute_p   = attribute;
        d_attributeInfo = attributeInfo;
        return 0;
    }

    template <class OTHER_ATTRIBUTE_TYPE>
    int operator()(OTHER_ATTRIBUTE_TYPE *, const bdlat_AttributeInfo&)
    {
        return 0;
    }

    // ACCESSORS
    Attribute *attribute() const
    {
        return d_attribute_p;
    }

    const bdlat_AttributeInfo& attributeInfo() const
    {
        return d_attributeInfo;
    }
};

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? bsl::atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    // Attribute Types

    typedef int  T1;  // 'maxDepth'
    typedef int  T2;  // 'traceLevel'
    typedef int  T3;  // 'maxSequenceSize'
    typedef bool T4;  // 'skipUnknownElements'
    typedef bool T5;  // 'defaultEmptyStrings'

    // Attribute 1 Values: 'maxDepth'
    // In Versions: 1, 2

    static const T1 D1 = 32;  // default value
    static const T1 A1 = 10;

    // Attribute 2 Values: 'traceLevel'
    // In Versions: 1, 2

    static const T2 D2 = 0;  // default value
    static const T2 A2 = 3;

    // Attribute 3 Values: 'maxSequenceSize'
    // In Versions: 1, 2

    static const T3 D3 = 1 * 1024 * 1024 * 1024;  // default value
    static const T3 A3 = 12345;

    // Attribute 4 Values: 'skipUnknownElements'
    // In Versions: 1, 2

    static const T4 D4 = true;  // default value
    static const T4 A4 = false;

    // Attribute 5 Values: 'defaultEmptyStrings'
    // In Versions: 2

    static const char ATTR5[] = "DefaultEmptyStrings";
    static const T5   D5      = true;  // default value
    static const T5   A5      = false;

    // Boolean Aliases

    static const bool T = true;
    static const bool F = false;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // 'bdlat' BACKWARD COMPATIBILITY TEST
        //   Ensure that the current version of the representation of
        //   'balber::BerDecoderOptions' is backwards-compatible with all older
        //   versions with respect to loading the value via the components in
        //   the 'bdlat' package.
        //
        // Concerns:
        //: 1 A 'balber::BerDecoderOptions' object having any prior
        //:   representation can have its value loaded into an object
        //:   with current representation without error.
        //:
        //: 2 Corresponding attributes between representations maintain their
        //:   original values when loaded into the other representation.
        //:
        //: 3 Attributes present in a later version (V2) but not a prior
        //:   version (V1) receive their default value when V1 object is
        //:   loaded into a v2 object.
        //
        // Plan:
        //: 1 Create an object 'Src0' in the first version of the
        //:   representation with the default value.
        //:
        //: 2 Create an object 'Dst0' in the second version of the
        //:   representation having any value.
        //:
        //: 3 Create an object 'Exp0' in the second version of the
        //:   representation having the default value.
        //:
        //: 4 Load the value of 'Src0' into 'Dst0'.  Validate that the
        //:   new value of 'Dst0' is equal to 'Exp0'.
        //:
        //: 5 Create an object 'Src1' in the first version of the
        //:   representation having non-default values for all attributes.
        //:
        //: 6 Create an object 'Dst1' in the second version of the
        //:   representation having any value.
        //:
        //: 7 Create an object 'Exp1' in the second versino of the
        //:   representation having the same non-default values for all of
        //:   its attributes that are shared with 'Src1', and default values
        //:   for all other attributes.
        //:
        //: 8 Load the value of 'Src1' into 'Dst1'.  Validate that the new
        //:   value of 'Dst1' is equal to 'Exp1'.
        // --------------------------------------------------------------------

        typedef test::BerDecoderOptionsV1 ObjV1;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Convert the default value of a set of V1 options to "
                    "a set of V2 options"
                 << endl;

        typedef ObjV1 SRC0;
        typedef Obj   DST0;

        SRC0        mSrc0;
        const SRC0& Src0 = mSrc0;

        DST0        mDst0;
        const DST0& Dst0 = mDst0;

        DST0        mExp0;
        const DST0& Exp0 = mExp0;

        int rc = bdlat_SymbolicConverter::convert(&mDst0, Src0);
        ASSERT_EQ(0, rc);
        ASSERT_EQ(Exp0, Dst0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Convert the value of a set of V1 options to a set of "
                    "V2 options"
                 << endl;

        typedef ObjV1 SRC1;
        typedef Obj   DST1;

        SRC1        mSrc1;
        const SRC1& Src1 = mSrc1;

        mSrc1.setMaxDepth(A1);
        mSrc1.setTraceLevel(A2);
        mSrc1.setMaxSequenceSize(A3);
        mSrc1.setSkipUnknownElements(A4);

        DST1        mDst1;
        const DST1& Dst1 = mDst1;

        DST1        mExp1;
        const DST1& Exp1 = mExp1;

        mExp1.setMaxDepth(A1);
        mExp1.setTraceLevel(A2);
        mExp1.setMaxSequenceSize(A3);
        mExp1.setSkipUnknownElements(A4);
        mExp1.setDefaultEmptyStrings(D5);

        rc = bdlat_SymbolicConverter::convert(&mDst1, Src1);
        ASSERT_EQ(0, rc);
        ASSERT_EQ(Exp1, Dst1);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ATTRIBUTE INFO TEST
        //   Ensure that the new attribute introduced in the second version
        //   of the representation of 'balber::BerDecoderOptions' correctly
        //   implements the attribute access and manipulation API expected
        //   by the components in the 'bdlat' package.
        //
        // Concerns:
        //: 1 One can look up the attribute info of "DefaultEmptyStrings"
        //:   by ID or name.
        //:
        //: 2 One can manipulate the "DefaultEmptyStrings" attribute during
        //:   bulk attribute manipulation, by ID, or by name.
        //:
        //: 3 One can access the "DefaultEmptyStrings" attribute during
        //:   bulk attribute access, by ID, or by name.
        //
        // Plan:
        //: 1 Create a 'balber::BerDecoderOptions' object 'X' having the
        //:   default value.
        //:
        //: 2 Look up the "DefaultEmptyStrings" attribute of 'X' by ID and
        //:   then by name.
        //:
        //: 3 Manipulate the "DefaultEmptyStrings" attribute of 'X' during
        //:   a bulk attribute manipulation, by ID, and then by name.
        //:
        //: 4 Access the "DefaultEmptyStrings" attribute of 'x' during a
        //:   bulk attribute access, by ID, and then by name.
        // --------------------------------------------------------------------

        Obj        mX;
        const Obj& X = mX;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Look up the \"DefaultEmptyStrings\" attribute by ID"
                 << endl;

        const bdlat_AttributeInfo *const A5Info0 = mX.lookupAttributeInfo(
                                     Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS);
        if (veryVerbose) {
            P(*A5Info0);
        }
        ASSERT_EQ(Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS,
                  A5Info0->id());
        ASSERT_EQ(0, std::strcmp(ATTR5, A5Info0->name()));
        ASSERT_EQ(std::strlen(ATTR5), A5Info0->nameLength());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Look up the \"DefaultEmptyStrings\" attribute by name"
                 << endl;

        const bdlat_AttributeInfo *const A5Info1 = mX.lookupAttributeInfo(
                                                           ATTR5              ,
                                                           std::strlen(ATTR5));
        if (veryVerbose) {
            P(*A5Info1);
        }
        ASSERT_EQ(Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS,
                  A5Info1->id());
        ASSERT_EQ(0, std::strcmp(ATTR5, A5Info1->name()));
        ASSERT_EQ(std::strlen(ATTR5), A5Info1->nameLength());

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Manipulate the \"DefaultEmptyStrings\" attribute"
                 << endl;

        typedef AttributeManipulator<bool> BoolManipulator;

        BoolManipulator A5Manipulator0(
                                     Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS);
        int rc = mX.manipulateAttributes(A5Manipulator0);
        ASSERT_EQ(0, rc);
        ASSERT_EQ(D5, *A5Manipulator0.attribute());
        ASSERT_EQ(0,
                  std::strcmp(ATTR5, A5Manipulator0.attributeInfo().name()));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Manipulate the \"DefaultEmptyStrings\" attribute by ID"
                 << endl;

        BoolManipulator A5Manipulator1(
                                     Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS);

        rc = mX.manipulateAttribute(A5Manipulator1                            ,
                                    Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS);
        ASSERT_EQ(0, rc);
        ASSERT_EQ(D5, *A5Manipulator1.attribute());
        ASSERT_EQ(0,
                  std::strcmp(ATTR5, A5Manipulator1.attributeInfo().name()));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Manipulate the \"DefaultEmptyStrings\" attribute by name"
                 << endl;

        BoolManipulator A5Manipulator2(
                                     Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS);

        rc = mX.manipulateAttribute(A5Manipulator2, ATTR5, std::strlen(ATTR5));
        ASSERT_EQ(0, rc);
        ASSERT_EQ(D5, *A5Manipulator2.attribute());
        ASSERT_EQ(0,
                  std::strcmp(ATTR5, A5Manipulator2.attributeInfo().name()));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Access the \"DefaultEmptyStrings\" attribute"
                 << endl;

        typedef AttributeAccessor<bool> BoolAccessor;

        BoolAccessor A5Accessor0(Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS);
        rc = X.accessAttributes(A5Accessor0);
        ASSERT_EQ(0, rc);
        ASSERT_EQ(D5, A5Accessor0.attribute());
        ASSERT_EQ(0, std::strcmp(ATTR5, A5Accessor0.attributeInfo().name()));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Access the \"DefaultEmptyStrings\" attribute by ID"
                 << endl;

        BoolAccessor A5Accessor1(Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS);
        rc = X.accessAttribute(A5Accessor1,
                               Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS);
        ASSERT_EQ(0, rc);
        ASSERT_EQ(D5, A5Accessor1.attribute());
        ASSERT_EQ(0, std::strcmp(ATTR5, A5Accessor1.attributeInfo().name()));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Access the \"DefaultEmptyStrings\" attribute by name"
                 << endl;

        BoolAccessor A5Accessor2(Obj::e_ATTRIBUTE_ID_DEFAULT_EMPTY_STRINGS);
        rc = X.accessAttribute(A5Accessor2, ATTR5, bsl::strlen(ATTR5));
        ASSERT_EQ(0, rc);
        ASSERT_EQ(D5, A5Accessor2.attribute());
        ASSERT_EQ(0, std::strcmp(ATTR5, A5Accessor2.attributeInfo().name()));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 This class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases
        //
        // Plan:
        //: 1 Create an object 'W' using the default constructor, such that
        //:   it has the default value.  Validate that it has the default
        //:   value.
        //:
        //: 2 Create an object 'X' as a copy of 'W'.  Validate that 'X' has the
        //:   same value as 'W'.
        //:
        //: 3 Set each attribute of 'X' to a non-default value.  Validate that
        //:   the attributes of 'X' have the respective non-default values.
        //:
        //: 4 Create an object 'Y' and set each attribute to the same
        //:   respective non-default values as the attributes of 'X'.  Validate
        //:   that 'X' and 'Y' have the same value.
        //:
        //: 5 Create an object 'Z' as a copy of 'Y'.  Validate that 'Z' has the
        //:   same value as 'Y'.
        //:
        //: 6 Set each attribute of 'Z' to its respective default value.
        //:   Validate that 'Z' has the same value as 'W'.
        //:
        //: 7 Assign the value of 'X' to 'W'.  Validate that 'X' has the same
        //:   value as 'W'.
        //:
        //: 8 Assign the value of 'Z' to 'W'.  Validate that 'Z' has the same
        //:   value as 'W'.
        //:
        //: 9 Assign the value of 'X' to 'X'.  Validate that 'X' has the same
        //:   value as 'Y'.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Create object W having the default value" << endl;

        Obj        mW;
        const Obj& W = mW;

        ASSERT_EQ(D1, W.maxDepth());
        ASSERT_EQ(D2, W.traceLevel());
        ASSERT_EQ(D3, W.maxSequenceSize());
        ASSERT_EQ(D4, W.skipUnknownElements());
        ASSERT_EQ(D5, W.defaultEmptyStrings());

        ASSERT_EQ(T, W == W);
        ASSERT_EQ(F, W != W);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Create object X having the value of W" << endl;

        Obj        mX(W);
        const Obj& X = mX;

        ASSERT_EQ(D1, X.maxDepth());
        ASSERT_EQ(D2, X.traceLevel());
        ASSERT_EQ(D3, X.maxSequenceSize());
        ASSERT_EQ(D4, X.skipUnknownElements());
        ASSERT_EQ(D5, X.defaultEmptyStrings());

        ASSERT_EQ(T, X == X);
        ASSERT_EQ(T, X == W);
        ASSERT_EQ(T, W == X);

        ASSERT_EQ(F, X != X);
        ASSERT_EQ(F, X != W);
        ASSERT_EQ(F, X != W);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Set X to the non-default value" << endl;

        mX.setMaxDepth(A1);
        mX.setTraceLevel(A2);
        mX.setMaxSequenceSize(A3);
        mX.setSkipUnknownElements(A4);
        mX.setDefaultEmptyStrings(A5);

        ASSERT_EQ(A1, X.maxDepth());
        ASSERT_EQ(A2, X.traceLevel());
        ASSERT_EQ(A3, X.maxSequenceSize());
        ASSERT_EQ(A4, X.skipUnknownElements());
        ASSERT_EQ(A5, X.defaultEmptyStrings());

        ASSERT_EQ(T, X == X);
        ASSERT_EQ(F, X == W);
        ASSERT_EQ(F, W == X);

        ASSERT_EQ(F, X != X);
        ASSERT_EQ(T, X != W);
        ASSERT_EQ(T, W != X);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Create an object Y and set it to the non-default value"
                 << endl;

        Obj        mY;
        const Obj& Y = mY;

        mY.setMaxDepth(A1);
        mY.setTraceLevel(A2);
        mY.setMaxSequenceSize(A3);
        mY.setSkipUnknownElements(A4);
        mY.setDefaultEmptyStrings(A5);

        ASSERT_EQ(A1, Y.maxDepth());
        ASSERT_EQ(A2, Y.traceLevel());
        ASSERT_EQ(A3, Y.maxSequenceSize());
        ASSERT_EQ(A4, Y.skipUnknownElements());
        ASSERT_EQ(A5, Y.defaultEmptyStrings());

        ASSERT_EQ(F, Y == W);
        ASSERT_EQ(T, Y == X);
        ASSERT_EQ(T, Y == Y);

        ASSERT_EQ(T, Y != W);
        ASSERT_EQ(F, Y != X);
        ASSERT_EQ(F, Y != Y);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Create object Z having the value of Y" << endl;

        Obj        mZ(Y);
        const Obj& Z = mZ;

        ASSERT_EQ(A1, Z.maxDepth());
        ASSERT_EQ(A2, Z.traceLevel());
        ASSERT_EQ(A3, Z.maxSequenceSize());
        ASSERT_EQ(A4, Z.skipUnknownElements());
        ASSERT_EQ(A5, Z.defaultEmptyStrings());

        ASSERT_EQ(F, Z == W);
        ASSERT_EQ(T, Z == X);
        ASSERT_EQ(T, Z == Y);
        ASSERT_EQ(T, Z == Z);

        ASSERT_EQ(T, Z != W);
        ASSERT_EQ(F, Z != X);
        ASSERT_EQ(F, Z != Y);
        ASSERT_EQ(F, Z != Z);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Set Z to the default value" << endl;

        mZ.setMaxDepth(D1);
        mZ.setTraceLevel(D2);
        mZ.setMaxSequenceSize(D3);
        mZ.setSkipUnknownElements(D4);
        mZ.setDefaultEmptyStrings(D5);

        ASSERT_EQ(D1, Z.maxDepth());
        ASSERT_EQ(D2, Z.traceLevel());
        ASSERT_EQ(D3, Z.maxSequenceSize());
        ASSERT_EQ(D4, Z.skipUnknownElements());
        ASSERT_EQ(D5, Z.defaultEmptyStrings());

        ASSERT_EQ(T, Z == W);
        ASSERT_EQ(F, Z == X);
        ASSERT_EQ(F, Z == Y);
        ASSERT_EQ(T, Z == Z);

        ASSERT_EQ(F, Z != W);
        ASSERT_EQ(T, Z != X);
        ASSERT_EQ(T, Z != Y);
        ASSERT_EQ(F, Z != Z);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Assign the value of X to W" << endl;

        mW = X;

        ASSERT_EQ(A1, W.maxDepth());
        ASSERT_EQ(A2, W.traceLevel());
        ASSERT_EQ(A3, W.maxSequenceSize());
        ASSERT_EQ(A4, W.skipUnknownElements());
        ASSERT_EQ(A5, W.defaultEmptyStrings());

        ASSERT_EQ(T, W == W);
        ASSERT_EQ(T, W == X);
        ASSERT_EQ(T, W == Y);
        ASSERT_EQ(F, W == Z);

        ASSERT_EQ(F, W != W);
        ASSERT_EQ(F, W != X);
        ASSERT_EQ(F, W != Y);
        ASSERT_EQ(T, W != Z);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Assign the value of Z to W" << endl;

        mW = Z;

        ASSERT_EQ(D1, W.maxDepth());
        ASSERT_EQ(D2, W.traceLevel());
        ASSERT_EQ(D3, W.maxSequenceSize());
        ASSERT_EQ(D4, W.skipUnknownElements());
        ASSERT_EQ(D5, W.defaultEmptyStrings());

        ASSERT_EQ(T, W == W);
        ASSERT_EQ(F, W == X);
        ASSERT_EQ(F, W == Y);
        ASSERT_EQ(T, W == Z);

        ASSERT_EQ(F, W != W);
        ASSERT_EQ(T, W != X);
        ASSERT_EQ(T, W != Y);
        ASSERT_EQ(F, W != Z);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (veryVerbose)
            cout << "Assign the value of X to X to check aliasing" << endl;

        mX = X;

        ASSERT_EQ(A1, X.maxDepth());
        ASSERT_EQ(A2, X.traceLevel());
        ASSERT_EQ(A3, X.maxSequenceSize());
        ASSERT_EQ(A4, X.skipUnknownElements());
        ASSERT_EQ(A5, X.defaultEmptyStrings());

        ASSERT_EQ(F, X == W);
        ASSERT_EQ(T, X == X);
        ASSERT_EQ(T, X == Y);
        ASSERT_EQ(F, X == Z);

        ASSERT_EQ(T, X != W);
        ASSERT_EQ(F, X != X);
        ASSERT_EQ(F, X != Y);
        ASSERT_EQ(T, X != Z);

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
