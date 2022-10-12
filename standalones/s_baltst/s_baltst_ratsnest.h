// s_baltst_ratsnest.h            *DO NOT EDIT*            @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_RATSNEST
#define INCLUDED_S_BALTST_RATSNEST

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_ratsnest_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bdlb_nullableallocatedvalue.h>

#include <bdlb_nullablevalue.h>

#include <bdlt_datetimetz.h>

#include <bsl_string.h>

#include <bsl_vector.h>

#include <s_baltst_customint.h>

#include <s_baltst_customstring.h>

#include <s_baltst_enumerated.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class Sequence3; }
namespace s_baltst { class Sequence5; }
namespace s_baltst { class Sequence6; }
namespace s_baltst { class Choice3; }
namespace s_baltst { class Choice1; }
namespace s_baltst { class Choice2; }
namespace s_baltst { class Sequence4; }
namespace s_baltst { class Sequence1; }
namespace s_baltst { class Sequence2; }
namespace s_baltst {

                              // ===============
                              // class Sequence3
                              // ===============

class Sequence3 {

    // INSTANCE DATA
    bsl::vector<bsl::string>                                        d_element2;
    bdlb::NullableValue<bsl::string>                                d_element4;
    bdlb::NullableAllocatedValue<Sequence5>                         d_element5;
    bsl::vector<s_baltst::Enumerated::Value>                        d_element1;
    bsl::vector<bdlb::NullableValue<s_baltst::Enumerated::Value> >  d_element6;
    bdlb::NullableValue<bool>                                       d_element3;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1 = 0
      , ATTRIBUTE_ID_ELEMENT2 = 1
      , ATTRIBUTE_ID_ELEMENT3 = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
      , ATTRIBUTE_ID_ELEMENT5 = 4
      , ATTRIBUTE_ID_ELEMENT6 = 5
    };

    enum {
        NUM_ATTRIBUTES = 6
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1 = 0
      , ATTRIBUTE_INDEX_ELEMENT2 = 1
      , ATTRIBUTE_INDEX_ELEMENT3 = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
      , ATTRIBUTE_INDEX_ELEMENT5 = 4
      , ATTRIBUTE_INDEX_ELEMENT6 = 5
    };

    // CONSTANTS
    static const char CLASS_NAME[];

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
    explicit Sequence3(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence3' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence3(const Sequence3& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence3' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence3(Sequence3&& original) noexcept;
        // Create an object of type 'Sequence3' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    Sequence3(Sequence3&& original,
              bslma::Allocator *basicAllocator);
        // Create an object of type 'Sequence3' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Sequence3();
        // Destroy this object.

    // MANIPULATORS
    Sequence3& operator=(const Sequence3& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence3& operator=(Sequence3&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    bsl::vector<s_baltst::Enumerated::Value>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bsl::vector<bsl::string>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdlb::NullableValue<bool>& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdlb::NullableValue<bsl::string>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bdlb::NullableAllocatedValue<Sequence5>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<s_baltst::Enumerated::Value> >& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

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

    const bsl::vector<s_baltst::Enumerated::Value>& element1() const;
        // Return a reference offering non-modifiable access to the "Element1"
        // attribute of this object.

    const bsl::vector<bsl::string>& element2() const;
        // Return a reference offering non-modifiable access to the "Element2"
        // attribute of this object.

    const bdlb::NullableValue<bool>& element3() const;
        // Return a reference offering non-modifiable access to the "Element3"
        // attribute of this object.

    const bdlb::NullableValue<bsl::string>& element4() const;
        // Return a reference offering non-modifiable access to the "Element4"
        // attribute of this object.

    const bdlb::NullableAllocatedValue<Sequence5>& element5() const;
        // Return a reference offering non-modifiable access to the "Element5"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<s_baltst::Enumerated::Value> >& element6() const;
        // Return a reference offering non-modifiable access to the "Element6"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence3& lhs, const Sequence3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence3& lhs, const Sequence3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence3& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Sequence3)

namespace s_baltst {

                              // ===============
                              // class Sequence5
                              // ===============

class Sequence5 {

    // INSTANCE DATA
    bslma::Allocator                                      *d_allocator_p;
    bsl::vector<bdlb::NullableValue<int> >                 d_element5;
    bsl::vector<bdlb::NullableValue<double> >              d_element3;
    bsl::vector<bdlb::NullableValue<bool> >                d_element2;
    bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >    d_element6;
    bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >  d_element7;
    bdlb::NullableValue<bsl::vector<char> >                d_element4;
    Sequence3                                             *d_element1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1 = 0
      , ATTRIBUTE_ID_ELEMENT2 = 1
      , ATTRIBUTE_ID_ELEMENT3 = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
      , ATTRIBUTE_ID_ELEMENT5 = 4
      , ATTRIBUTE_ID_ELEMENT6 = 5
      , ATTRIBUTE_ID_ELEMENT7 = 6
    };

    enum {
        NUM_ATTRIBUTES = 7
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1 = 0
      , ATTRIBUTE_INDEX_ELEMENT2 = 1
      , ATTRIBUTE_INDEX_ELEMENT3 = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
      , ATTRIBUTE_INDEX_ELEMENT5 = 4
      , ATTRIBUTE_INDEX_ELEMENT6 = 5
      , ATTRIBUTE_INDEX_ELEMENT7 = 6
    };

    // CONSTANTS
    static const char CLASS_NAME[];

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
    explicit Sequence5(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence5' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence5(const Sequence5& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence5' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence5(Sequence5&& original) noexcept;
        // Create an object of type 'Sequence5' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    Sequence5(Sequence5&& original,
              bslma::Allocator *basicAllocator);
        // Create an object of type 'Sequence5' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Sequence5();
        // Destroy this object.

    // MANIPULATORS
    Sequence5& operator=(const Sequence5& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence5& operator=(Sequence5&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    Sequence3& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<bool> >& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<double> >& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdlb::NullableValue<bsl::vector<char> >& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<int> >& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >& element7();
        // Return a reference to the modifiable "Element7" attribute of this
        // object.

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

    const Sequence3& element1() const;
        // Return a reference offering non-modifiable access to the "Element1"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<bool> >& element2() const;
        // Return a reference offering non-modifiable access to the "Element2"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<double> >& element3() const;
        // Return a reference offering non-modifiable access to the "Element3"
        // attribute of this object.

    const bdlb::NullableValue<bsl::vector<char> >& element4() const;
        // Return a reference offering non-modifiable access to the "Element4"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<int> >& element5() const;
        // Return a reference offering non-modifiable access to the "Element5"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >& element6() const;
        // Return a reference offering non-modifiable access to the "Element6"
        // attribute of this object.

    const bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >& element7() const;
        // Return a reference offering non-modifiable access to the "Element7"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence5& lhs, const Sequence5& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence5& lhs, const Sequence5& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence5& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Sequence5)

namespace s_baltst {

                              // ===============
                              // class Sequence6
                              // ===============

class Sequence6 {

    // INSTANCE DATA
    s_baltst::CustomString                                  d_element7;
    bsl::vector<unsigned int>                               d_element12;
    bsl::vector<unsigned char>                              d_element10;
    bsl::vector<s_baltst::CustomString>                     d_element11;
    bsl::vector<bdlb::NullableValue<unsigned int> >         d_element15;
    bsl::vector<bdlb::NullableValue<unsigned char> >        d_element13;
    bdlb::NullableValue<s_baltst::CustomString>             d_element2;
    unsigned int                                            d_element4;
    s_baltst::CustomInt                                     d_element8;
    bsl::vector<s_baltst::CustomInt>                        d_element14;
    bsl::vector<bdlb::NullableValue<s_baltst::CustomInt> >  d_element6;
    bdlb::NullableValue<unsigned int>                       d_element9;
    bdlb::NullableValue<s_baltst::CustomInt>                d_element3;
    unsigned char                                           d_element5;
    bdlb::NullableValue<unsigned char>                      d_element1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1  = 0
      , ATTRIBUTE_ID_ELEMENT2  = 1
      , ATTRIBUTE_ID_ELEMENT3  = 2
      , ATTRIBUTE_ID_ELEMENT4  = 3
      , ATTRIBUTE_ID_ELEMENT5  = 4
      , ATTRIBUTE_ID_ELEMENT6  = 5
      , ATTRIBUTE_ID_ELEMENT7  = 6
      , ATTRIBUTE_ID_ELEMENT8  = 7
      , ATTRIBUTE_ID_ELEMENT9  = 8
      , ATTRIBUTE_ID_ELEMENT10 = 9
      , ATTRIBUTE_ID_ELEMENT11 = 10
      , ATTRIBUTE_ID_ELEMENT12 = 11
      , ATTRIBUTE_ID_ELEMENT13 = 12
      , ATTRIBUTE_ID_ELEMENT14 = 13
      , ATTRIBUTE_ID_ELEMENT15 = 14
    };

    enum {
        NUM_ATTRIBUTES = 15
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1  = 0
      , ATTRIBUTE_INDEX_ELEMENT2  = 1
      , ATTRIBUTE_INDEX_ELEMENT3  = 2
      , ATTRIBUTE_INDEX_ELEMENT4  = 3
      , ATTRIBUTE_INDEX_ELEMENT5  = 4
      , ATTRIBUTE_INDEX_ELEMENT6  = 5
      , ATTRIBUTE_INDEX_ELEMENT7  = 6
      , ATTRIBUTE_INDEX_ELEMENT8  = 7
      , ATTRIBUTE_INDEX_ELEMENT9  = 8
      , ATTRIBUTE_INDEX_ELEMENT10 = 9
      , ATTRIBUTE_INDEX_ELEMENT11 = 10
      , ATTRIBUTE_INDEX_ELEMENT12 = 11
      , ATTRIBUTE_INDEX_ELEMENT13 = 12
      , ATTRIBUTE_INDEX_ELEMENT14 = 13
      , ATTRIBUTE_INDEX_ELEMENT15 = 14
    };

    // CONSTANTS
    static const char CLASS_NAME[];

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
    explicit Sequence6(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence6' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence6(const Sequence6& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence6' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence6(Sequence6&& original) noexcept;
        // Create an object of type 'Sequence6' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    Sequence6(Sequence6&& original,
              bslma::Allocator *basicAllocator);
        // Create an object of type 'Sequence6' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Sequence6();
        // Destroy this object.

    // MANIPULATORS
    Sequence6& operator=(const Sequence6& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence6& operator=(Sequence6&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    bdlb::NullableValue<unsigned char>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bdlb::NullableValue<s_baltst::CustomString>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdlb::NullableValue<s_baltst::CustomInt>& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    unsigned int& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    unsigned char& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<s_baltst::CustomInt> >& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    s_baltst::CustomString& element7();
        // Return a reference to the modifiable "Element7" attribute of this
        // object.

    s_baltst::CustomInt& element8();
        // Return a reference to the modifiable "Element8" attribute of this
        // object.

    bdlb::NullableValue<unsigned int>& element9();
        // Return a reference to the modifiable "Element9" attribute of this
        // object.

    bsl::vector<unsigned char>& element10();
        // Return a reference to the modifiable "Element10" attribute of this
        // object.

    bsl::vector<s_baltst::CustomString>& element11();
        // Return a reference to the modifiable "Element11" attribute of this
        // object.

    bsl::vector<unsigned int>& element12();
        // Return a reference to the modifiable "Element12" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<unsigned char> >& element13();
        // Return a reference to the modifiable "Element13" attribute of this
        // object.

    bsl::vector<s_baltst::CustomInt>& element14();
        // Return a reference to the modifiable "Element14" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<unsigned int> >& element15();
        // Return a reference to the modifiable "Element15" attribute of this
        // object.

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

    const bdlb::NullableValue<unsigned char>& element1() const;
        // Return a reference offering non-modifiable access to the "Element1"
        // attribute of this object.

    const bdlb::NullableValue<s_baltst::CustomString>& element2() const;
        // Return a reference offering non-modifiable access to the "Element2"
        // attribute of this object.

    const bdlb::NullableValue<s_baltst::CustomInt>& element3() const;
        // Return a reference offering non-modifiable access to the "Element3"
        // attribute of this object.

    unsigned int element4() const;
        // Return the value of the "Element4" attribute of this object.

    unsigned char element5() const;
        // Return the value of the "Element5" attribute of this object.

    const bsl::vector<bdlb::NullableValue<s_baltst::CustomInt> >& element6() const;
        // Return a reference offering non-modifiable access to the "Element6"
        // attribute of this object.

    const s_baltst::CustomString& element7() const;
        // Return a reference offering non-modifiable access to the "Element7"
        // attribute of this object.

    const s_baltst::CustomInt& element8() const;
        // Return a reference offering non-modifiable access to the "Element8"
        // attribute of this object.

    const bdlb::NullableValue<unsigned int>& element9() const;
        // Return a reference offering non-modifiable access to the "Element9"
        // attribute of this object.

    const bsl::vector<unsigned char>& element10() const;
        // Return a reference offering non-modifiable access to the "Element10"
        // attribute of this object.

    const bsl::vector<s_baltst::CustomString>& element11() const;
        // Return a reference offering non-modifiable access to the "Element11"
        // attribute of this object.

    const bsl::vector<unsigned int>& element12() const;
        // Return a reference offering non-modifiable access to the "Element12"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<unsigned char> >& element13() const;
        // Return a reference offering non-modifiable access to the "Element13"
        // attribute of this object.

    const bsl::vector<s_baltst::CustomInt>& element14() const;
        // Return a reference offering non-modifiable access to the "Element14"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<unsigned int> >& element15() const;
        // Return a reference offering non-modifiable access to the "Element15"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence6& lhs, const Sequence6& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence6& lhs, const Sequence6& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence6& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Sequence6)

namespace s_baltst {

                               // =============
                               // class Choice3
                               // =============

class Choice3 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< Sequence6 >              d_selection1;
        bsls::ObjectBuffer< unsigned char >          d_selection2;
        bsls::ObjectBuffer< s_baltst::CustomString > d_selection3;
        bsls::ObjectBuffer< s_baltst::CustomInt >    d_selection4;
    };

    int                                              d_selectionId;
    bslma::Allocator                                *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION1 = 0
      , SELECTION_ID_SELECTION2 = 1
      , SELECTION_ID_SELECTION3 = 2
      , SELECTION_ID_SELECTION4 = 3
    };

    enum {
        NUM_SELECTIONS = 4
    };

    enum {
        SELECTION_INDEX_SELECTION1 = 0
      , SELECTION_INDEX_SELECTION2 = 1
      , SELECTION_INDEX_SELECTION3 = 2
      , SELECTION_INDEX_SELECTION4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Choice3(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice3' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Choice3(const Choice3& original,
           bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice3' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Choice3(Choice3&& original) noexcept;
        // Create an object of type 'Choice3' having the value of the specified
        // 'original' object.  After performing this action, the 'original'
        // object will be left in a valid, but unspecified state.

    Choice3(Choice3&& original,
           bslma::Allocator *basicAllocator);
        // Create an object of type 'Choice3' having the value of the specified
        // 'original' object.  After performing this action, the 'original'
        // object will be left in a valid, but unspecified state.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Choice3();
        // Destroy this object.

    // MANIPULATORS
    Choice3& operator=(const Choice3& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Choice3& operator=(Choice3&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    Sequence6& makeSelection1();
    Sequence6& makeSelection1(const Sequence6& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence6& makeSelection1(Sequence6&& value);
#endif
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    unsigned char& makeSelection2();
    unsigned char& makeSelection2(unsigned char value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    s_baltst::CustomString& makeSelection3();
    s_baltst::CustomString& makeSelection3(const s_baltst::CustomString& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::CustomString& makeSelection3(s_baltst::CustomString&& value);
#endif
        // Set the value of this object to be a "Selection3" value.  Optionally
        // specify the 'value' of the "Selection3".  If 'value' is not
        // specified, the default "Selection3" value is used.

    s_baltst::CustomInt& makeSelection4();
    s_baltst::CustomInt& makeSelection4(const s_baltst::CustomInt& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::CustomInt& makeSelection4(s_baltst::CustomInt&& value);
#endif
        // Set the value of this object to be a "Selection4" value.  Optionally
        // specify the 'value' of the "Selection4".  If 'value' is not
        // specified, the default "Selection4" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    Sequence6& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    unsigned char& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    s_baltst::CustomString& selection3();
        // Return a reference to the modifiable "Selection3" selection of this
        // object if "Selection3" is the current selection.  The behavior is
        // undefined unless "Selection3" is the selection of this object.

    s_baltst::CustomInt& selection4();
        // Return a reference to the modifiable "Selection4" selection of this
        // object if "Selection4" is the current selection.  The behavior is
        // undefined unless "Selection4" is the selection of this object.

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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const Sequence6& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const unsigned char& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const s_baltst::CustomString& selection3() const;
        // Return a reference to the non-modifiable "Selection3" selection of
        // this object if "Selection3" is the current selection.  The behavior
        // is undefined unless "Selection3" is the selection of this object.

    const s_baltst::CustomInt& selection4() const;
        // Return a reference to the non-modifiable "Selection4" selection of
        // this object if "Selection4" is the current selection.  The behavior
        // is undefined unless "Selection4" is the selection of this object.

    bool isSelection1Value() const;
        // Return 'true' if the value of this object is a "Selection1" value,
        // and return 'false' otherwise.

    bool isSelection2Value() const;
        // Return 'true' if the value of this object is a "Selection2" value,
        // and return 'false' otherwise.

    bool isSelection3Value() const;
        // Return 'true' if the value of this object is a "Selection3" value,
        // and return 'false' otherwise.

    bool isSelection4Value() const;
        // Return 'true' if the value of this object is a "Selection4" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const Choice3& lhs, const Choice3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Choice3' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Choice3& lhs, const Choice3& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Choice3& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Choice3)

namespace s_baltst {

                               // =============
                               // class Choice1
                               // =============

class Choice1 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< int >     d_selection1;
        bsls::ObjectBuffer< double >  d_selection2;
        Sequence4                   *d_selection3;
        Choice2                     *d_selection4;
    };

    int                               d_selectionId;
    bslma::Allocator                 *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION1 = 0
      , SELECTION_ID_SELECTION2 = 1
      , SELECTION_ID_SELECTION3 = 2
      , SELECTION_ID_SELECTION4 = 3
    };

    enum {
        NUM_SELECTIONS = 4
    };

    enum {
        SELECTION_INDEX_SELECTION1 = 0
      , SELECTION_INDEX_SELECTION2 = 1
      , SELECTION_INDEX_SELECTION3 = 2
      , SELECTION_INDEX_SELECTION4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Choice1(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice1' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Choice1(const Choice1& original,
           bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice1' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Choice1(Choice1&& original) noexcept;
        // Create an object of type 'Choice1' having the value of the specified
        // 'original' object.  After performing this action, the 'original'
        // object will be left in a valid, but unspecified state.

    Choice1(Choice1&& original,
           bslma::Allocator *basicAllocator);
        // Create an object of type 'Choice1' having the value of the specified
        // 'original' object.  After performing this action, the 'original'
        // object will be left in a valid, but unspecified state.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Choice1();
        // Destroy this object.

    // MANIPULATORS
    Choice1& operator=(const Choice1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Choice1& operator=(Choice1&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    int& makeSelection1();
    int& makeSelection1(int value);
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    double& makeSelection2();
    double& makeSelection2(double value);
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    Sequence4& makeSelection3();
    Sequence4& makeSelection3(const Sequence4& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence4& makeSelection3(Sequence4&& value);
#endif
        // Set the value of this object to be a "Selection3" value.  Optionally
        // specify the 'value' of the "Selection3".  If 'value' is not
        // specified, the default "Selection3" value is used.

    Choice2& makeSelection4();
    Choice2& makeSelection4(const Choice2& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Choice2& makeSelection4(Choice2&& value);
#endif
        // Set the value of this object to be a "Selection4" value.  Optionally
        // specify the 'value' of the "Selection4".  If 'value' is not
        // specified, the default "Selection4" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    int& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    double& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    Sequence4& selection3();
        // Return a reference to the modifiable "Selection3" selection of this
        // object if "Selection3" is the current selection.  The behavior is
        // undefined unless "Selection3" is the selection of this object.

    Choice2& selection4();
        // Return a reference to the modifiable "Selection4" selection of this
        // object if "Selection4" is the current selection.  The behavior is
        // undefined unless "Selection4" is the selection of this object.

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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const int& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const double& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const Sequence4& selection3() const;
        // Return a reference to the non-modifiable "Selection3" selection of
        // this object if "Selection3" is the current selection.  The behavior
        // is undefined unless "Selection3" is the selection of this object.

    const Choice2& selection4() const;
        // Return a reference to the non-modifiable "Selection4" selection of
        // this object if "Selection4" is the current selection.  The behavior
        // is undefined unless "Selection4" is the selection of this object.

    bool isSelection1Value() const;
        // Return 'true' if the value of this object is a "Selection1" value,
        // and return 'false' otherwise.

    bool isSelection2Value() const;
        // Return 'true' if the value of this object is a "Selection2" value,
        // and return 'false' otherwise.

    bool isSelection3Value() const;
        // Return 'true' if the value of this object is a "Selection3" value,
        // and return 'false' otherwise.

    bool isSelection4Value() const;
        // Return 'true' if the value of this object is a "Selection4" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const Choice1& lhs, const Choice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Choice1' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Choice1& lhs, const Choice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Choice1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Choice1)

namespace s_baltst {

                               // =============
                               // class Choice2
                               // =============

class Choice2 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< bool >          d_selection1;
        bsls::ObjectBuffer< bsl::string >   d_selection2;
        Choice1                           *d_selection3;
        bsls::ObjectBuffer< unsigned int >  d_selection4;
    };

    int                                     d_selectionId;
    bslma::Allocator                       *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION1 = 0
      , SELECTION_ID_SELECTION2 = 1
      , SELECTION_ID_SELECTION3 = 2
      , SELECTION_ID_SELECTION4 = 3
    };

    enum {
        NUM_SELECTIONS = 4
    };

    enum {
        SELECTION_INDEX_SELECTION1 = 0
      , SELECTION_INDEX_SELECTION2 = 1
      , SELECTION_INDEX_SELECTION3 = 2
      , SELECTION_INDEX_SELECTION4 = 3
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Choice2(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice2' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Choice2(const Choice2& original,
           bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Choice2' having the value of the specified
        // 'original' object.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Choice2(Choice2&& original) noexcept;
        // Create an object of type 'Choice2' having the value of the specified
        // 'original' object.  After performing this action, the 'original'
        // object will be left in a valid, but unspecified state.

    Choice2(Choice2&& original,
           bslma::Allocator *basicAllocator);
        // Create an object of type 'Choice2' having the value of the specified
        // 'original' object.  After performing this action, the 'original'
        // object will be left in a valid, but unspecified state.  Use the
        // optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Choice2();
        // Destroy this object.

    // MANIPULATORS
    Choice2& operator=(const Choice2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Choice2& operator=(Choice2&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    bool& makeSelection1();
    bool& makeSelection1(bool value);
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    bsl::string& makeSelection2();
    bsl::string& makeSelection2(const bsl::string& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::string& makeSelection2(bsl::string&& value);
#endif
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    Choice1& makeSelection3();
    Choice1& makeSelection3(const Choice1& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Choice1& makeSelection3(Choice1&& value);
#endif
        // Set the value of this object to be a "Selection3" value.  Optionally
        // specify the 'value' of the "Selection3".  If 'value' is not
        // specified, the default "Selection3" value is used.

    unsigned int& makeSelection4();
    unsigned int& makeSelection4(unsigned int value);
        // Set the value of this object to be a "Selection4" value.  Optionally
        // specify the 'value' of the "Selection4".  If 'value' is not
        // specified, the default "Selection4" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    bool& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    bsl::string& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    Choice1& selection3();
        // Return a reference to the modifiable "Selection3" selection of this
        // object if "Selection3" is the current selection.  The behavior is
        // undefined unless "Selection3" is the selection of this object.

    unsigned int& selection4();
        // Return a reference to the modifiable "Selection4" selection of this
        // object if "Selection4" is the current selection.  The behavior is
        // undefined unless "Selection4" is the selection of this object.

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

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const bool& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const bsl::string& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const Choice1& selection3() const;
        // Return a reference to the non-modifiable "Selection3" selection of
        // this object if "Selection3" is the current selection.  The behavior
        // is undefined unless "Selection3" is the selection of this object.

    const unsigned int& selection4() const;
        // Return a reference to the non-modifiable "Selection4" selection of
        // this object if "Selection4" is the current selection.  The behavior
        // is undefined unless "Selection4" is the selection of this object.

    bool isSelection1Value() const;
        // Return 'true' if the value of this object is a "Selection1" value,
        // and return 'false' otherwise.

    bool isSelection2Value() const;
        // Return 'true' if the value of this object is a "Selection2" value,
        // and return 'false' otherwise.

    bool isSelection3Value() const;
        // Return 'true' if the value of this object is a "Selection3" value,
        // and return 'false' otherwise.

    bool isSelection4Value() const;
        // Return 'true' if the value of this object is a "Selection4" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const Choice2& lhs, const Choice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Choice2' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Choice2& lhs, const Choice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Choice2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Choice2)

namespace s_baltst {

                              // ===============
                              // class Sequence4
                              // ===============

class Sequence4 {

    // INSTANCE DATA
    double                                            d_element10;
    bsl::vector<s_baltst::CustomString>               d_element19;
    bsl::vector<int>                                  d_element17;
    bsl::vector<double>                               d_element15;
    bsl::vector<char>                                 d_element11;
    bsl::vector<bool>                                 d_element14;
    bsl::vector<bdlt::DatetimeTz>                     d_element18;
    bsl::vector<Sequence3>                            d_element1;
    bsl::vector<Choice1>                              d_element2;
    bsl::string                                       d_element9;
    bdlb::NullableValue<s_baltst::CustomString>       d_element6;
    bdlb::NullableValue<bsl::vector<char> >           d_element3;
    bdlb::NullableValue<bsl::vector<char> >           d_element16;
    bdlb::NullableValue<bdlt::DatetimeTz>             d_element5;
    s_baltst::Enumerated::Value                       d_element13;
    int                                               d_element12;
    bdlb::NullableValue<s_baltst::Enumerated::Value>  d_element7;
    bdlb::NullableValue<int>                          d_element4;
    bool                                              d_element8;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1  = 0
      , ATTRIBUTE_ID_ELEMENT2  = 1
      , ATTRIBUTE_ID_ELEMENT3  = 2
      , ATTRIBUTE_ID_ELEMENT4  = 3
      , ATTRIBUTE_ID_ELEMENT5  = 4
      , ATTRIBUTE_ID_ELEMENT6  = 5
      , ATTRIBUTE_ID_ELEMENT7  = 6
      , ATTRIBUTE_ID_ELEMENT8  = 7
      , ATTRIBUTE_ID_ELEMENT9  = 8
      , ATTRIBUTE_ID_ELEMENT10 = 9
      , ATTRIBUTE_ID_ELEMENT11 = 10
      , ATTRIBUTE_ID_ELEMENT12 = 11
      , ATTRIBUTE_ID_ELEMENT13 = 12
      , ATTRIBUTE_ID_ELEMENT14 = 13
      , ATTRIBUTE_ID_ELEMENT15 = 14
      , ATTRIBUTE_ID_ELEMENT16 = 15
      , ATTRIBUTE_ID_ELEMENT17 = 16
      , ATTRIBUTE_ID_ELEMENT18 = 17
      , ATTRIBUTE_ID_ELEMENT19 = 18
    };

    enum {
        NUM_ATTRIBUTES = 19
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1  = 0
      , ATTRIBUTE_INDEX_ELEMENT2  = 1
      , ATTRIBUTE_INDEX_ELEMENT3  = 2
      , ATTRIBUTE_INDEX_ELEMENT4  = 3
      , ATTRIBUTE_INDEX_ELEMENT5  = 4
      , ATTRIBUTE_INDEX_ELEMENT6  = 5
      , ATTRIBUTE_INDEX_ELEMENT7  = 6
      , ATTRIBUTE_INDEX_ELEMENT8  = 7
      , ATTRIBUTE_INDEX_ELEMENT9  = 8
      , ATTRIBUTE_INDEX_ELEMENT10 = 9
      , ATTRIBUTE_INDEX_ELEMENT11 = 10
      , ATTRIBUTE_INDEX_ELEMENT12 = 11
      , ATTRIBUTE_INDEX_ELEMENT13 = 12
      , ATTRIBUTE_INDEX_ELEMENT14 = 13
      , ATTRIBUTE_INDEX_ELEMENT15 = 14
      , ATTRIBUTE_INDEX_ELEMENT16 = 15
      , ATTRIBUTE_INDEX_ELEMENT17 = 16
      , ATTRIBUTE_INDEX_ELEMENT18 = 17
      , ATTRIBUTE_INDEX_ELEMENT19 = 18
    };

    // CONSTANTS
    static const char CLASS_NAME[];

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
    explicit Sequence4(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence4' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence4(const Sequence4& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence4' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence4(Sequence4&& original) noexcept;
        // Create an object of type 'Sequence4' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    Sequence4(Sequence4&& original,
              bslma::Allocator *basicAllocator);
        // Create an object of type 'Sequence4' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Sequence4();
        // Destroy this object.

    // MANIPULATORS
    Sequence4& operator=(const Sequence4& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence4& operator=(Sequence4&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    bsl::vector<Sequence3>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bsl::vector<Choice1>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdlb::NullableValue<bsl::vector<char> >& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdlb::NullableValue<int>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bdlb::NullableValue<bdlt::DatetimeTz>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bdlb::NullableValue<s_baltst::CustomString>& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    bdlb::NullableValue<s_baltst::Enumerated::Value>& element7();
        // Return a reference to the modifiable "Element7" attribute of this
        // object.

    bool& element8();
        // Return a reference to the modifiable "Element8" attribute of this
        // object.

    bsl::string& element9();
        // Return a reference to the modifiable "Element9" attribute of this
        // object.

    double& element10();
        // Return a reference to the modifiable "Element10" attribute of this
        // object.

    bsl::vector<char>& element11();
        // Return a reference to the modifiable "Element11" attribute of this
        // object.

    int& element12();
        // Return a reference to the modifiable "Element12" attribute of this
        // object.

    s_baltst::Enumerated::Value& element13();
        // Return a reference to the modifiable "Element13" attribute of this
        // object.

    bsl::vector<bool>& element14();
        // Return a reference to the modifiable "Element14" attribute of this
        // object.

    bsl::vector<double>& element15();
        // Return a reference to the modifiable "Element15" attribute of this
        // object.

    bdlb::NullableValue<bsl::vector<char> >& element16();
        // Return a reference to the modifiable "Element16" attribute of this
        // object.

    bsl::vector<int>& element17();
        // Return a reference to the modifiable "Element17" attribute of this
        // object.

    bsl::vector<bdlt::DatetimeTz>& element18();
        // Return a reference to the modifiable "Element18" attribute of this
        // object.

    bsl::vector<s_baltst::CustomString>& element19();
        // Return a reference to the modifiable "Element19" attribute of this
        // object.

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

    const bsl::vector<Sequence3>& element1() const;
        // Return a reference offering non-modifiable access to the "Element1"
        // attribute of this object.

    const bsl::vector<Choice1>& element2() const;
        // Return a reference offering non-modifiable access to the "Element2"
        // attribute of this object.

    const bdlb::NullableValue<bsl::vector<char> >& element3() const;
        // Return a reference offering non-modifiable access to the "Element3"
        // attribute of this object.

    const bdlb::NullableValue<int>& element4() const;
        // Return a reference offering non-modifiable access to the "Element4"
        // attribute of this object.

    const bdlb::NullableValue<bdlt::DatetimeTz>& element5() const;
        // Return a reference offering non-modifiable access to the "Element5"
        // attribute of this object.

    const bdlb::NullableValue<s_baltst::CustomString>& element6() const;
        // Return a reference offering non-modifiable access to the "Element6"
        // attribute of this object.

    const bdlb::NullableValue<s_baltst::Enumerated::Value>& element7() const;
        // Return a reference offering non-modifiable access to the "Element7"
        // attribute of this object.

    bool element8() const;
        // Return the value of the "Element8" attribute of this object.

    const bsl::string& element9() const;
        // Return a reference offering non-modifiable access to the "Element9"
        // attribute of this object.

    double element10() const;
        // Return the value of the "Element10" attribute of this object.

    const bsl::vector<char>& element11() const;
        // Return a reference offering non-modifiable access to the "Element11"
        // attribute of this object.

    int element12() const;
        // Return the value of the "Element12" attribute of this object.

    s_baltst::Enumerated::Value element13() const;
        // Return the value of the "Element13" attribute of this object.

    const bsl::vector<bool>& element14() const;
        // Return a reference offering non-modifiable access to the "Element14"
        // attribute of this object.

    const bsl::vector<double>& element15() const;
        // Return a reference offering non-modifiable access to the "Element15"
        // attribute of this object.

    const bdlb::NullableValue<bsl::vector<char> >& element16() const;
        // Return a reference offering non-modifiable access to the "Element16"
        // attribute of this object.

    const bsl::vector<int>& element17() const;
        // Return a reference offering non-modifiable access to the "Element17"
        // attribute of this object.

    const bsl::vector<bdlt::DatetimeTz>& element18() const;
        // Return a reference offering non-modifiable access to the "Element18"
        // attribute of this object.

    const bsl::vector<s_baltst::CustomString>& element19() const;
        // Return a reference offering non-modifiable access to the "Element19"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence4& lhs, const Sequence4& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence4& lhs, const Sequence4& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence4& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Sequence4)

namespace s_baltst {

                              // ===============
                              // class Sequence1
                              // ===============

class Sequence1 {

    // INSTANCE DATA
    bslma::Allocator                           *d_allocator_p;
    bsl::vector<bdlb::NullableValue<Choice1> >  d_element4;
    bsl::vector<Choice3>                        d_element5;
    bsl::vector<Choice1>                        d_element2;
    bdlb::NullableValue<Choice3>                d_element1;
    Choice2                                    *d_element3;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1 = 0
      , ATTRIBUTE_ID_ELEMENT2 = 1
      , ATTRIBUTE_ID_ELEMENT3 = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
      , ATTRIBUTE_ID_ELEMENT5 = 4
    };

    enum {
        NUM_ATTRIBUTES = 5
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1 = 0
      , ATTRIBUTE_INDEX_ELEMENT2 = 1
      , ATTRIBUTE_INDEX_ELEMENT3 = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
      , ATTRIBUTE_INDEX_ELEMENT5 = 4
    };

    // CONSTANTS
    static const char CLASS_NAME[];

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
    explicit Sequence1(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence1' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence1(const Sequence1& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence1' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence1(Sequence1&& original) noexcept;
        // Create an object of type 'Sequence1' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    Sequence1(Sequence1&& original,
              bslma::Allocator *basicAllocator);
        // Create an object of type 'Sequence1' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Sequence1();
        // Destroy this object.

    // MANIPULATORS
    Sequence1& operator=(const Sequence1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence1& operator=(Sequence1&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    bdlb::NullableValue<Choice3>& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    bsl::vector<Choice1>& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    Choice2& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<Choice1> >& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bsl::vector<Choice3>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

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

    const bdlb::NullableValue<Choice3>& element1() const;
        // Return a reference offering non-modifiable access to the "Element1"
        // attribute of this object.

    const bsl::vector<Choice1>& element2() const;
        // Return a reference offering non-modifiable access to the "Element2"
        // attribute of this object.

    const Choice2& element3() const;
        // Return a reference offering non-modifiable access to the "Element3"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<Choice1> >& element4() const;
        // Return a reference offering non-modifiable access to the "Element4"
        // attribute of this object.

    const bsl::vector<Choice3>& element5() const;
        // Return a reference offering non-modifiable access to the "Element5"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence1& lhs, const Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence1& lhs, const Sequence1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Sequence1)

namespace s_baltst {

                              // ===============
                              // class Sequence2
                              // ===============

class Sequence2 {

    // INSTANCE DATA
    s_baltst::CustomString                                     d_element1;
    bsl::vector<bdlb::NullableValue<s_baltst::CustomString> >  d_element7;
    bsl::vector<bdlb::NullableValue<bsl::string> >             d_element6;
    bdlt::DatetimeTz                                           d_element3;
    bdlb::NullableValue<double>                                d_element5;
    bdlb::NullableValue<Choice1>                               d_element4;
    unsigned char                                              d_element2;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ELEMENT1 = 0
      , ATTRIBUTE_ID_ELEMENT2 = 1
      , ATTRIBUTE_ID_ELEMENT3 = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
      , ATTRIBUTE_ID_ELEMENT5 = 4
      , ATTRIBUTE_ID_ELEMENT6 = 5
      , ATTRIBUTE_ID_ELEMENT7 = 6
    };

    enum {
        NUM_ATTRIBUTES = 7
    };

    enum {
        ATTRIBUTE_INDEX_ELEMENT1 = 0
      , ATTRIBUTE_INDEX_ELEMENT2 = 1
      , ATTRIBUTE_INDEX_ELEMENT3 = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
      , ATTRIBUTE_INDEX_ELEMENT5 = 4
      , ATTRIBUTE_INDEX_ELEMENT6 = 5
      , ATTRIBUTE_INDEX_ELEMENT7 = 6
    };

    // CONSTANTS
    static const char CLASS_NAME[];

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
    explicit Sequence2(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence2' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Sequence2(const Sequence2& original,
              bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Sequence2' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence2(Sequence2&& original) noexcept;
        // Create an object of type 'Sequence2' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    Sequence2(Sequence2&& original,
              bslma::Allocator *basicAllocator);
        // Create an object of type 'Sequence2' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Sequence2();
        // Destroy this object.

    // MANIPULATORS
    Sequence2& operator=(const Sequence2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Sequence2& operator=(Sequence2&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

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

    s_baltst::CustomString& element1();
        // Return a reference to the modifiable "Element1" attribute of this
        // object.

    unsigned char& element2();
        // Return a reference to the modifiable "Element2" attribute of this
        // object.

    bdlt::DatetimeTz& element3();
        // Return a reference to the modifiable "Element3" attribute of this
        // object.

    bdlb::NullableValue<Choice1>& element4();
        // Return a reference to the modifiable "Element4" attribute of this
        // object.

    bdlb::NullableValue<double>& element5();
        // Return a reference to the modifiable "Element5" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<bsl::string> >& element6();
        // Return a reference to the modifiable "Element6" attribute of this
        // object.

    bsl::vector<bdlb::NullableValue<s_baltst::CustomString> >& element7();
        // Return a reference to the modifiable "Element7" attribute of this
        // object.

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

    const s_baltst::CustomString& element1() const;
        // Return a reference offering non-modifiable access to the "Element1"
        // attribute of this object.

    unsigned char element2() const;
        // Return the value of the "Element2" attribute of this object.

    const bdlt::DatetimeTz& element3() const;
        // Return a reference offering non-modifiable access to the "Element3"
        // attribute of this object.

    const bdlb::NullableValue<Choice1>& element4() const;
        // Return a reference offering non-modifiable access to the "Element4"
        // attribute of this object.

    const bdlb::NullableValue<double>& element5() const;
        // Return a reference offering non-modifiable access to the "Element5"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<bsl::string> >& element6() const;
        // Return a reference offering non-modifiable access to the "Element6"
        // attribute of this object.

    const bsl::vector<bdlb::NullableValue<s_baltst::CustomString> >& element7() const;
        // Return a reference offering non-modifiable access to the "Element7"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const Sequence2& lhs, const Sequence2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const Sequence2& lhs, const Sequence2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sequence2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Sequence2)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                              // ---------------
                              // class Sequence3
                              // ---------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int Sequence3::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int Sequence3::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence3::manipulateAttribute(
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
bsl::vector<s_baltst::Enumerated::Value>& Sequence3::element1()
{
    return d_element1;
}

inline
bsl::vector<bsl::string>& Sequence3::element2()
{
    return d_element2;
}

inline
bdlb::NullableValue<bool>& Sequence3::element3()
{
    return d_element3;
}

inline
bdlb::NullableValue<bsl::string>& Sequence3::element4()
{
    return d_element4;
}

inline
bdlb::NullableAllocatedValue<Sequence5>& Sequence3::element5()
{
    return d_element5;
}

inline
bsl::vector<bdlb::NullableValue<s_baltst::Enumerated::Value> >& Sequence3::element6()
{
    return d_element6;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence3::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int Sequence3::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence3::accessAttribute(
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
const bsl::vector<s_baltst::Enumerated::Value>& Sequence3::element1() const
{
    return d_element1;
}

inline
const bsl::vector<bsl::string>& Sequence3::element2() const
{
    return d_element2;
}

inline
const bdlb::NullableValue<bool>& Sequence3::element3() const
{
    return d_element3;
}

inline
const bdlb::NullableValue<bsl::string>& Sequence3::element4() const
{
    return d_element4;
}

inline
const bdlb::NullableAllocatedValue<Sequence5>& Sequence3::element5() const
{
    return d_element5;
}

inline
const bsl::vector<bdlb::NullableValue<s_baltst::Enumerated::Value> >& Sequence3::element6() const
{
    return d_element6;
}



                              // ---------------
                              // class Sequence5
                              // ---------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int Sequence5::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    BSLS_ASSERT(d_element1);
    ret = manipulator(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int Sequence5::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        BSLS_ASSERT(d_element1);
        return manipulator(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      case ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence5::manipulateAttribute(
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
Sequence3& Sequence5::element1()
{
    BSLS_ASSERT(d_element1);
    return *d_element1;
}

inline
bsl::vector<bdlb::NullableValue<bool> >& Sequence5::element2()
{
    return d_element2;
}

inline
bsl::vector<bdlb::NullableValue<double> >& Sequence5::element3()
{
    return d_element3;
}

inline
bdlb::NullableValue<bsl::vector<char> >& Sequence5::element4()
{
    return d_element4;
}

inline
bsl::vector<bdlb::NullableValue<int> >& Sequence5::element5()
{
    return d_element5;
}

inline
bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >& Sequence5::element6()
{
    return d_element6;
}

inline
bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >& Sequence5::element7()
{
    return d_element7;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence5::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    BSLS_ASSERT(d_element1);
    ret = accessor(*d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int Sequence5::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        BSLS_ASSERT(d_element1);
        return accessor(*d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      case ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence5::accessAttribute(
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
const Sequence3& Sequence5::element1() const
{
    BSLS_ASSERT(d_element1);
    return *d_element1;
}

inline
const bsl::vector<bdlb::NullableValue<bool> >& Sequence5::element2() const
{
    return d_element2;
}

inline
const bsl::vector<bdlb::NullableValue<double> >& Sequence5::element3() const
{
    return d_element3;
}

inline
const bdlb::NullableValue<bsl::vector<char> >& Sequence5::element4() const
{
    return d_element4;
}

inline
const bsl::vector<bdlb::NullableValue<int> >& Sequence5::element5() const
{
    return d_element5;
}

inline
const bsl::vector<bdlb::NullableValue<bdlt::DatetimeTz> >& Sequence5::element6() const
{
    return d_element6;
}

inline
const bsl::vector<bdlb::NullableAllocatedValue<Sequence3> >& Sequence5::element7() const
{
    return d_element7;
}



                              // ---------------
                              // class Sequence6
                              // ---------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int Sequence6::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int Sequence6::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      case ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      }
      case ATTRIBUTE_ID_ELEMENT8: {
        return manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
      }
      case ATTRIBUTE_ID_ELEMENT9: {
        return manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
      }
      case ATTRIBUTE_ID_ELEMENT10: {
        return manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
      }
      case ATTRIBUTE_ID_ELEMENT11: {
        return manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
      }
      case ATTRIBUTE_ID_ELEMENT12: {
        return manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
      }
      case ATTRIBUTE_ID_ELEMENT13: {
        return manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
      }
      case ATTRIBUTE_ID_ELEMENT14: {
        return manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
      }
      case ATTRIBUTE_ID_ELEMENT15: {
        return manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence6::manipulateAttribute(
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
bdlb::NullableValue<unsigned char>& Sequence6::element1()
{
    return d_element1;
}

inline
bdlb::NullableValue<s_baltst::CustomString>& Sequence6::element2()
{
    return d_element2;
}

inline
bdlb::NullableValue<s_baltst::CustomInt>& Sequence6::element3()
{
    return d_element3;
}

inline
unsigned int& Sequence6::element4()
{
    return d_element4;
}

inline
unsigned char& Sequence6::element5()
{
    return d_element5;
}

inline
bsl::vector<bdlb::NullableValue<s_baltst::CustomInt> >& Sequence6::element6()
{
    return d_element6;
}

inline
s_baltst::CustomString& Sequence6::element7()
{
    return d_element7;
}

inline
s_baltst::CustomInt& Sequence6::element8()
{
    return d_element8;
}

inline
bdlb::NullableValue<unsigned int>& Sequence6::element9()
{
    return d_element9;
}

inline
bsl::vector<unsigned char>& Sequence6::element10()
{
    return d_element10;
}

inline
bsl::vector<s_baltst::CustomString>& Sequence6::element11()
{
    return d_element11;
}

inline
bsl::vector<unsigned int>& Sequence6::element12()
{
    return d_element12;
}

inline
bsl::vector<bdlb::NullableValue<unsigned char> >& Sequence6::element13()
{
    return d_element13;
}

inline
bsl::vector<s_baltst::CustomInt>& Sequence6::element14()
{
    return d_element14;
}

inline
bsl::vector<bdlb::NullableValue<unsigned int> >& Sequence6::element15()
{
    return d_element15;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence6::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int Sequence6::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      case ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      }
      case ATTRIBUTE_ID_ELEMENT8: {
        return accessor(d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
      }
      case ATTRIBUTE_ID_ELEMENT9: {
        return accessor(d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
      }
      case ATTRIBUTE_ID_ELEMENT10: {
        return accessor(d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
      }
      case ATTRIBUTE_ID_ELEMENT11: {
        return accessor(d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
      }
      case ATTRIBUTE_ID_ELEMENT12: {
        return accessor(d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
      }
      case ATTRIBUTE_ID_ELEMENT13: {
        return accessor(d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
      }
      case ATTRIBUTE_ID_ELEMENT14: {
        return accessor(d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
      }
      case ATTRIBUTE_ID_ELEMENT15: {
        return accessor(d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence6::accessAttribute(
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
const bdlb::NullableValue<unsigned char>& Sequence6::element1() const
{
    return d_element1;
}

inline
const bdlb::NullableValue<s_baltst::CustomString>& Sequence6::element2() const
{
    return d_element2;
}

inline
const bdlb::NullableValue<s_baltst::CustomInt>& Sequence6::element3() const
{
    return d_element3;
}

inline
unsigned int Sequence6::element4() const
{
    return d_element4;
}

inline
unsigned char Sequence6::element5() const
{
    return d_element5;
}

inline
const bsl::vector<bdlb::NullableValue<s_baltst::CustomInt> >& Sequence6::element6() const
{
    return d_element6;
}

inline
const s_baltst::CustomString& Sequence6::element7() const
{
    return d_element7;
}

inline
const s_baltst::CustomInt& Sequence6::element8() const
{
    return d_element8;
}

inline
const bdlb::NullableValue<unsigned int>& Sequence6::element9() const
{
    return d_element9;
}

inline
const bsl::vector<unsigned char>& Sequence6::element10() const
{
    return d_element10;
}

inline
const bsl::vector<s_baltst::CustomString>& Sequence6::element11() const
{
    return d_element11;
}

inline
const bsl::vector<unsigned int>& Sequence6::element12() const
{
    return d_element12;
}

inline
const bsl::vector<bdlb::NullableValue<unsigned char> >& Sequence6::element13() const
{
    return d_element13;
}

inline
const bsl::vector<s_baltst::CustomInt>& Sequence6::element14() const
{
    return d_element14;
}

inline
const bsl::vector<bdlb::NullableValue<unsigned int> >& Sequence6::element15() const
{
    return d_element15;
}



                               // -------------
                               // class Choice3
                               // -------------

// CLASS METHODS
// CREATORS
inline
Choice3::Choice3(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Choice3::~Choice3()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int Choice3::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Choice3::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case Choice3::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case Choice3::SELECTION_ID_SELECTION3:
        return manipulator(&d_selection3.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case Choice3::SELECTION_ID_SELECTION4:
        return manipulator(&d_selection4.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(Choice3::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
Sequence6& Choice3::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
unsigned char& Choice3::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
s_baltst::CustomString& Choice3::selection3()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
s_baltst::CustomInt& Choice3::selection4()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

// ACCESSORS
inline
int Choice3::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Choice3::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case SELECTION_ID_SELECTION3:
        return accessor(d_selection3.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case SELECTION_ID_SELECTION4:
        return accessor(d_selection4.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const Sequence6& Choice3::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const unsigned char& Choice3::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const s_baltst::CustomString& Choice3::selection3() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
const s_baltst::CustomInt& Choice3::selection4() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

inline
bool Choice3::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool Choice3::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool Choice3::isSelection3Value() const
{
    return SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool Choice3::isSelection4Value() const
{
    return SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool Choice3::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // -------------
                               // class Choice1
                               // -------------

// CLASS METHODS
// CREATORS
inline
Choice1::Choice1(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Choice1::~Choice1()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int Choice1::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Choice1::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case Choice1::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case Choice1::SELECTION_ID_SELECTION3:
        return manipulator(d_selection3,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case Choice1::SELECTION_ID_SELECTION4:
        return manipulator(d_selection4,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(Choice1::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
int& Choice1::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
double& Choice1::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
Sequence4& Choice1::selection3()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
Choice2& Choice1::selection4()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

// ACCESSORS
inline
int Choice1::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Choice1::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case SELECTION_ID_SELECTION3:
        return accessor(*d_selection3,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case SELECTION_ID_SELECTION4:
        return accessor(*d_selection4,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const int& Choice1::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const double& Choice1::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const Sequence4& Choice1::selection3() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
const Choice2& Choice1::selection4() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

inline
bool Choice1::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool Choice1::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool Choice1::isSelection3Value() const
{
    return SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool Choice1::isSelection4Value() const
{
    return SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool Choice1::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                               // -------------
                               // class Choice2
                               // -------------

// CLASS METHODS
// CREATORS
inline
Choice2::Choice2(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Choice2::~Choice2()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int Choice2::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Choice2::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case Choice2::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case Choice2::SELECTION_ID_SELECTION3:
        return manipulator(d_selection3,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case Choice2::SELECTION_ID_SELECTION4:
        return manipulator(&d_selection4.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(Choice2::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
bool& Choice2::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
bsl::string& Choice2::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
Choice1& Choice2::selection3()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
unsigned int& Choice2::selection4()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

// ACCESSORS
inline
int Choice2::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Choice2::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION1:
        return accessor(d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SELECTION_ID_SELECTION2:
        return accessor(d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case SELECTION_ID_SELECTION3:
        return accessor(*d_selection3,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case SELECTION_ID_SELECTION4:
        return accessor(d_selection4.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const bool& Choice2::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const bsl::string& Choice2::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const Choice1& Choice2::selection3() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return *d_selection3;
}

inline
const unsigned int& Choice2::selection4() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

inline
bool Choice2::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool Choice2::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool Choice2::isSelection3Value() const
{
    return SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool Choice2::isSelection4Value() const
{
    return SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool Choice2::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                              // ---------------
                              // class Sequence4
                              // ---------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int Sequence4::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element16, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element17, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element18, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element19, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int Sequence4::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      case ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      }
      case ATTRIBUTE_ID_ELEMENT8: {
        return manipulator(&d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
      }
      case ATTRIBUTE_ID_ELEMENT9: {
        return manipulator(&d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
      }
      case ATTRIBUTE_ID_ELEMENT10: {
        return manipulator(&d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
      }
      case ATTRIBUTE_ID_ELEMENT11: {
        return manipulator(&d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
      }
      case ATTRIBUTE_ID_ELEMENT12: {
        return manipulator(&d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
      }
      case ATTRIBUTE_ID_ELEMENT13: {
        return manipulator(&d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
      }
      case ATTRIBUTE_ID_ELEMENT14: {
        return manipulator(&d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
      }
      case ATTRIBUTE_ID_ELEMENT15: {
        return manipulator(&d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
      }
      case ATTRIBUTE_ID_ELEMENT16: {
        return manipulator(&d_element16, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16]);
      }
      case ATTRIBUTE_ID_ELEMENT17: {
        return manipulator(&d_element17, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17]);
      }
      case ATTRIBUTE_ID_ELEMENT18: {
        return manipulator(&d_element18, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18]);
      }
      case ATTRIBUTE_ID_ELEMENT19: {
        return manipulator(&d_element19, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence4::manipulateAttribute(
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
bsl::vector<Sequence3>& Sequence4::element1()
{
    return d_element1;
}

inline
bsl::vector<Choice1>& Sequence4::element2()
{
    return d_element2;
}

inline
bdlb::NullableValue<bsl::vector<char> >& Sequence4::element3()
{
    return d_element3;
}

inline
bdlb::NullableValue<int>& Sequence4::element4()
{
    return d_element4;
}

inline
bdlb::NullableValue<bdlt::DatetimeTz>& Sequence4::element5()
{
    return d_element5;
}

inline
bdlb::NullableValue<s_baltst::CustomString>& Sequence4::element6()
{
    return d_element6;
}

inline
bdlb::NullableValue<s_baltst::Enumerated::Value>& Sequence4::element7()
{
    return d_element7;
}

inline
bool& Sequence4::element8()
{
    return d_element8;
}

inline
bsl::string& Sequence4::element9()
{
    return d_element9;
}

inline
double& Sequence4::element10()
{
    return d_element10;
}

inline
bsl::vector<char>& Sequence4::element11()
{
    return d_element11;
}

inline
int& Sequence4::element12()
{
    return d_element12;
}

inline
s_baltst::Enumerated::Value& Sequence4::element13()
{
    return d_element13;
}

inline
bsl::vector<bool>& Sequence4::element14()
{
    return d_element14;
}

inline
bsl::vector<double>& Sequence4::element15()
{
    return d_element15;
}

inline
bdlb::NullableValue<bsl::vector<char> >& Sequence4::element16()
{
    return d_element16;
}

inline
bsl::vector<int>& Sequence4::element17()
{
    return d_element17;
}

inline
bsl::vector<bdlt::DatetimeTz>& Sequence4::element18()
{
    return d_element18;
}

inline
bsl::vector<s_baltst::CustomString>& Sequence4::element19()
{
    return d_element19;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence4::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element16, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element17, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element18, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element19, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int Sequence4::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      case ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      }
      case ATTRIBUTE_ID_ELEMENT8: {
        return accessor(d_element8, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT8]);
      }
      case ATTRIBUTE_ID_ELEMENT9: {
        return accessor(d_element9, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT9]);
      }
      case ATTRIBUTE_ID_ELEMENT10: {
        return accessor(d_element10, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT10]);
      }
      case ATTRIBUTE_ID_ELEMENT11: {
        return accessor(d_element11, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT11]);
      }
      case ATTRIBUTE_ID_ELEMENT12: {
        return accessor(d_element12, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT12]);
      }
      case ATTRIBUTE_ID_ELEMENT13: {
        return accessor(d_element13, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT13]);
      }
      case ATTRIBUTE_ID_ELEMENT14: {
        return accessor(d_element14, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT14]);
      }
      case ATTRIBUTE_ID_ELEMENT15: {
        return accessor(d_element15, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT15]);
      }
      case ATTRIBUTE_ID_ELEMENT16: {
        return accessor(d_element16, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT16]);
      }
      case ATTRIBUTE_ID_ELEMENT17: {
        return accessor(d_element17, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT17]);
      }
      case ATTRIBUTE_ID_ELEMENT18: {
        return accessor(d_element18, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT18]);
      }
      case ATTRIBUTE_ID_ELEMENT19: {
        return accessor(d_element19, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT19]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence4::accessAttribute(
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
const bsl::vector<Sequence3>& Sequence4::element1() const
{
    return d_element1;
}

inline
const bsl::vector<Choice1>& Sequence4::element2() const
{
    return d_element2;
}

inline
const bdlb::NullableValue<bsl::vector<char> >& Sequence4::element3() const
{
    return d_element3;
}

inline
const bdlb::NullableValue<int>& Sequence4::element4() const
{
    return d_element4;
}

inline
const bdlb::NullableValue<bdlt::DatetimeTz>& Sequence4::element5() const
{
    return d_element5;
}

inline
const bdlb::NullableValue<s_baltst::CustomString>& Sequence4::element6() const
{
    return d_element6;
}

inline
const bdlb::NullableValue<s_baltst::Enumerated::Value>& Sequence4::element7() const
{
    return d_element7;
}

inline
bool Sequence4::element8() const
{
    return d_element8;
}

inline
const bsl::string& Sequence4::element9() const
{
    return d_element9;
}

inline
double Sequence4::element10() const
{
    return d_element10;
}

inline
const bsl::vector<char>& Sequence4::element11() const
{
    return d_element11;
}

inline
int Sequence4::element12() const
{
    return d_element12;
}

inline
s_baltst::Enumerated::Value Sequence4::element13() const
{
    return d_element13;
}

inline
const bsl::vector<bool>& Sequence4::element14() const
{
    return d_element14;
}

inline
const bsl::vector<double>& Sequence4::element15() const
{
    return d_element15;
}

inline
const bdlb::NullableValue<bsl::vector<char> >& Sequence4::element16() const
{
    return d_element16;
}

inline
const bsl::vector<int>& Sequence4::element17() const
{
    return d_element17;
}

inline
const bsl::vector<bdlt::DatetimeTz>& Sequence4::element18() const
{
    return d_element18;
}

inline
const bsl::vector<s_baltst::CustomString>& Sequence4::element19() const
{
    return d_element19;
}



                              // ---------------
                              // class Sequence1
                              // ---------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int Sequence1::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    BSLS_ASSERT(d_element3);
    ret = manipulator(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int Sequence1::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        BSLS_ASSERT(d_element3);
        return manipulator(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence1::manipulateAttribute(
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
bdlb::NullableValue<Choice3>& Sequence1::element1()
{
    return d_element1;
}

inline
bsl::vector<Choice1>& Sequence1::element2()
{
    return d_element2;
}

inline
Choice2& Sequence1::element3()
{
    BSLS_ASSERT(d_element3);
    return *d_element3;
}

inline
bsl::vector<bdlb::NullableValue<Choice1> >& Sequence1::element4()
{
    return d_element4;
}

inline
bsl::vector<Choice3>& Sequence1::element5()
{
    return d_element5;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence1::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    BSLS_ASSERT(d_element3);
    ret = accessor(*d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int Sequence1::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        BSLS_ASSERT(d_element3);
        return accessor(*d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence1::accessAttribute(
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
const bdlb::NullableValue<Choice3>& Sequence1::element1() const
{
    return d_element1;
}

inline
const bsl::vector<Choice1>& Sequence1::element2() const
{
    return d_element2;
}

inline
const Choice2& Sequence1::element3() const
{
    BSLS_ASSERT(d_element3);
    return *d_element3;
}

inline
const bsl::vector<bdlb::NullableValue<Choice1> >& Sequence1::element4() const
{
    return d_element4;
}

inline
const bsl::vector<Choice3>& Sequence1::element5() const
{
    return d_element5;
}



                              // ---------------
                              // class Sequence2
                              // ---------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int Sequence2::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int Sequence2::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return manipulator(&d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return manipulator(&d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return manipulator(&d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return manipulator(&d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return manipulator(&d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      case ATTRIBUTE_ID_ELEMENT7: {
        return manipulator(&d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int Sequence2::manipulateAttribute(
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
s_baltst::CustomString& Sequence2::element1()
{
    return d_element1;
}

inline
unsigned char& Sequence2::element2()
{
    return d_element2;
}

inline
bdlt::DatetimeTz& Sequence2::element3()
{
    return d_element3;
}

inline
bdlb::NullableValue<Choice1>& Sequence2::element4()
{
    return d_element4;
}

inline
bdlb::NullableValue<double>& Sequence2::element5()
{
    return d_element5;
}

inline
bsl::vector<bdlb::NullableValue<bsl::string> >& Sequence2::element6()
{
    return d_element6;
}

inline
bsl::vector<bdlb::NullableValue<s_baltst::CustomString> >& Sequence2::element7()
{
    return d_element7;
}

// ACCESSORS
template <class ACCESSOR>
int Sequence2::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int Sequence2::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ELEMENT1: {
        return accessor(d_element1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT1]);
      }
      case ATTRIBUTE_ID_ELEMENT2: {
        return accessor(d_element2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT2]);
      }
      case ATTRIBUTE_ID_ELEMENT3: {
        return accessor(d_element3, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT3]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      case ATTRIBUTE_ID_ELEMENT5: {
        return accessor(d_element5, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT5]);
      }
      case ATTRIBUTE_ID_ELEMENT6: {
        return accessor(d_element6, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT6]);
      }
      case ATTRIBUTE_ID_ELEMENT7: {
        return accessor(d_element7, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT7]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int Sequence2::accessAttribute(
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
const s_baltst::CustomString& Sequence2::element1() const
{
    return d_element1;
}

inline
unsigned char Sequence2::element2() const
{
    return d_element2;
}

inline
const bdlt::DatetimeTz& Sequence2::element3() const
{
    return d_element3;
}

inline
const bdlb::NullableValue<Choice1>& Sequence2::element4() const
{
    return d_element4;
}

inline
const bdlb::NullableValue<double>& Sequence2::element5() const
{
    return d_element5;
}

inline
const bsl::vector<bdlb::NullableValue<bsl::string> >& Sequence2::element6() const
{
    return d_element6;
}

inline
const bsl::vector<bdlb::NullableValue<s_baltst::CustomString> >& Sequence2::element7() const
{
    return d_element7;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::Sequence3& lhs,
        const s_baltst::Sequence3& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6();
}

inline
bool s_baltst::operator!=(
        const s_baltst::Sequence3& lhs,
        const s_baltst::Sequence3& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Sequence3& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::Sequence5& lhs,
        const s_baltst::Sequence5& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6()
         && lhs.element7() == rhs.element7();
}

inline
bool s_baltst::operator!=(
        const s_baltst::Sequence5& lhs,
        const s_baltst::Sequence5& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Sequence5& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::Sequence6& lhs,
        const s_baltst::Sequence6& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6()
         && lhs.element7() == rhs.element7()
         && lhs.element8() == rhs.element8()
         && lhs.element9() == rhs.element9()
         && lhs.element10() == rhs.element10()
         && lhs.element11() == rhs.element11()
         && lhs.element12() == rhs.element12()
         && lhs.element13() == rhs.element13()
         && lhs.element14() == rhs.element14()
         && lhs.element15() == rhs.element15();
}

inline
bool s_baltst::operator!=(
        const s_baltst::Sequence6& lhs,
        const s_baltst::Sequence6& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Sequence6& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::Choice3& lhs,
        const s_baltst::Choice3& rhs)
{
    typedef s_baltst::Choice3 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool s_baltst::operator!=(
        const s_baltst::Choice3& lhs,
        const s_baltst::Choice3& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Choice3& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::Choice1& lhs,
        const s_baltst::Choice1& rhs)
{
    typedef s_baltst::Choice1 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool s_baltst::operator!=(
        const s_baltst::Choice1& lhs,
        const s_baltst::Choice1& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Choice1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::Choice2& lhs,
        const s_baltst::Choice2& rhs)
{
    typedef s_baltst::Choice2 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION1:
            return lhs.selection1() == rhs.selection1();
          case Class::SELECTION_ID_SELECTION2:
            return lhs.selection2() == rhs.selection2();
          case Class::SELECTION_ID_SELECTION3:
            return lhs.selection3() == rhs.selection3();
          case Class::SELECTION_ID_SELECTION4:
            return lhs.selection4() == rhs.selection4();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool s_baltst::operator!=(
        const s_baltst::Choice2& lhs,
        const s_baltst::Choice2& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Choice2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::Sequence4& lhs,
        const s_baltst::Sequence4& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6()
         && lhs.element7() == rhs.element7()
         && lhs.element8() == rhs.element8()
         && lhs.element9() == rhs.element9()
         && lhs.element10() == rhs.element10()
         && lhs.element11() == rhs.element11()
         && lhs.element12() == rhs.element12()
         && lhs.element13() == rhs.element13()
         && lhs.element14() == rhs.element14()
         && lhs.element15() == rhs.element15()
         && lhs.element16() == rhs.element16()
         && lhs.element17() == rhs.element17()
         && lhs.element18() == rhs.element18()
         && lhs.element19() == rhs.element19();
}

inline
bool s_baltst::operator!=(
        const s_baltst::Sequence4& lhs,
        const s_baltst::Sequence4& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Sequence4& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::Sequence1& lhs,
        const s_baltst::Sequence1& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5();
}

inline
bool s_baltst::operator!=(
        const s_baltst::Sequence1& lhs,
        const s_baltst::Sequence1& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Sequence1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::Sequence2& lhs,
        const s_baltst::Sequence2& rhs)
{
    return  lhs.element1() == rhs.element1()
         && lhs.element2() == rhs.element2()
         && lhs.element3() == rhs.element3()
         && lhs.element4() == rhs.element4()
         && lhs.element5() == rhs.element5()
         && lhs.element6() == rhs.element6()
         && lhs.element7() == rhs.element7();
}

inline
bool s_baltst::operator!=(
        const s_baltst::Sequence2& lhs,
        const s_baltst::Sequence2& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Sequence2& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_ratsnest.xsd --mode msg --includedir . --msgComponent ratsnest --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
