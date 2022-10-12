// s_baltst_featuretestmessage.h       *DO NOT EDIT*       @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_FEATURETESTMESSAGE
#define INCLUDED_S_BALTST_FEATURETESTMESSAGE

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_featuretestmessage_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bdlt_datetimetz.h>

#include <bsl_vector.h>

#include <s_baltst_customstring.h>

#include <s_baltst_enumerated.h>

#include <s_baltst_ratsnest.h>

#include <s_baltst_sequencewithanonymity.h>

#include <s_baltst_unsignedsequence.h>

#include <s_baltst_voidsequence.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class FeatureTestMessage; }
namespace s_baltst {

                          // ========================
                          // class FeatureTestMessage
                          // ========================

class FeatureTestMessage {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< s_baltst::Sequence1 >              d_selection1;
        bsls::ObjectBuffer< bsl::vector<char> >                d_selection2;
        bsls::ObjectBuffer< s_baltst::Sequence2 >              d_selection3;
        s_baltst::Sequence3                                  *d_selection4;
        bsls::ObjectBuffer< bdlt::DatetimeTz >                 d_selection5;
        bsls::ObjectBuffer< s_baltst::CustomString >           d_selection6;
        bsls::ObjectBuffer< s_baltst::Enumerated::Value >      d_selection7;
        bsls::ObjectBuffer< s_baltst::Choice3 >                d_selection8;
        bsls::ObjectBuffer< s_baltst::VoidSequence >           d_selection9;
        bsls::ObjectBuffer< s_baltst::UnsignedSequence >       d_selection10;
        bsls::ObjectBuffer< s_baltst::SequenceWithAnonymity >  d_selection11;
    };

    int                                                        d_selectionId;
    bslma::Allocator                                          *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED   = -1
      , SELECTION_ID_SELECTION1  = 0
      , SELECTION_ID_SELECTION2  = 1
      , SELECTION_ID_SELECTION3  = 2
      , SELECTION_ID_SELECTION4  = 3
      , SELECTION_ID_SELECTION5  = 4
      , SELECTION_ID_SELECTION6  = 5
      , SELECTION_ID_SELECTION7  = 6
      , SELECTION_ID_SELECTION8  = 7
      , SELECTION_ID_SELECTION9  = 8
      , SELECTION_ID_SELECTION10 = 9
      , SELECTION_ID_SELECTION11 = 10
    };

    enum {
        NUM_SELECTIONS = 11
    };

    enum {
        SELECTION_INDEX_SELECTION1  = 0
      , SELECTION_INDEX_SELECTION2  = 1
      , SELECTION_INDEX_SELECTION3  = 2
      , SELECTION_INDEX_SELECTION4  = 3
      , SELECTION_INDEX_SELECTION5  = 4
      , SELECTION_INDEX_SELECTION6  = 5
      , SELECTION_INDEX_SELECTION7  = 6
      , SELECTION_INDEX_SELECTION8  = 7
      , SELECTION_INDEX_SELECTION9  = 8
      , SELECTION_INDEX_SELECTION10 = 9
      , SELECTION_INDEX_SELECTION11 = 10
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
    explicit FeatureTestMessage(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'FeatureTestMessage' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    FeatureTestMessage(const FeatureTestMessage& original,
                      bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'FeatureTestMessage' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    FeatureTestMessage(FeatureTestMessage&& original) noexcept;
        // Create an object of type 'FeatureTestMessage' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    FeatureTestMessage(FeatureTestMessage&& original,
                      bslma::Allocator *basicAllocator);
        // Create an object of type 'FeatureTestMessage' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~FeatureTestMessage();
        // Destroy this object.

    // MANIPULATORS
    FeatureTestMessage& operator=(const FeatureTestMessage& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    FeatureTestMessage& operator=(FeatureTestMessage&& rhs);
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

    s_baltst::Sequence1& makeSelection1();
    s_baltst::Sequence1& makeSelection1(const s_baltst::Sequence1& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::Sequence1& makeSelection1(s_baltst::Sequence1&& value);
#endif
        // Set the value of this object to be a "Selection1" value.  Optionally
        // specify the 'value' of the "Selection1".  If 'value' is not
        // specified, the default "Selection1" value is used.

    bsl::vector<char>& makeSelection2();
    bsl::vector<char>& makeSelection2(const bsl::vector<char>& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::vector<char>& makeSelection2(bsl::vector<char>&& value);
#endif
        // Set the value of this object to be a "Selection2" value.  Optionally
        // specify the 'value' of the "Selection2".  If 'value' is not
        // specified, the default "Selection2" value is used.

    s_baltst::Sequence2& makeSelection3();
    s_baltst::Sequence2& makeSelection3(const s_baltst::Sequence2& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::Sequence2& makeSelection3(s_baltst::Sequence2&& value);
#endif
        // Set the value of this object to be a "Selection3" value.  Optionally
        // specify the 'value' of the "Selection3".  If 'value' is not
        // specified, the default "Selection3" value is used.

    s_baltst::Sequence3& makeSelection4();
    s_baltst::Sequence3& makeSelection4(const s_baltst::Sequence3& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::Sequence3& makeSelection4(s_baltst::Sequence3&& value);
#endif
        // Set the value of this object to be a "Selection4" value.  Optionally
        // specify the 'value' of the "Selection4".  If 'value' is not
        // specified, the default "Selection4" value is used.

    bdlt::DatetimeTz& makeSelection5();
    bdlt::DatetimeTz& makeSelection5(const bdlt::DatetimeTz& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bdlt::DatetimeTz& makeSelection5(bdlt::DatetimeTz&& value);
#endif
        // Set the value of this object to be a "Selection5" value.  Optionally
        // specify the 'value' of the "Selection5".  If 'value' is not
        // specified, the default "Selection5" value is used.

    s_baltst::CustomString& makeSelection6();
    s_baltst::CustomString& makeSelection6(const s_baltst::CustomString& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::CustomString& makeSelection6(s_baltst::CustomString&& value);
#endif
        // Set the value of this object to be a "Selection6" value.  Optionally
        // specify the 'value' of the "Selection6".  If 'value' is not
        // specified, the default "Selection6" value is used.

    s_baltst::Enumerated::Value& makeSelection7();
    s_baltst::Enumerated::Value& makeSelection7(s_baltst::Enumerated::Value value);
        // Set the value of this object to be a "Selection7" value.  Optionally
        // specify the 'value' of the "Selection7".  If 'value' is not
        // specified, the default "Selection7" value is used.

    s_baltst::Choice3& makeSelection8();
    s_baltst::Choice3& makeSelection8(const s_baltst::Choice3& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::Choice3& makeSelection8(s_baltst::Choice3&& value);
#endif
        // Set the value of this object to be a "Selection8" value.  Optionally
        // specify the 'value' of the "Selection8".  If 'value' is not
        // specified, the default "Selection8" value is used.

    s_baltst::VoidSequence& makeSelection9();
    s_baltst::VoidSequence& makeSelection9(const s_baltst::VoidSequence& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::VoidSequence& makeSelection9(s_baltst::VoidSequence&& value);
#endif
        // Set the value of this object to be a "Selection9" value.  Optionally
        // specify the 'value' of the "Selection9".  If 'value' is not
        // specified, the default "Selection9" value is used.

    s_baltst::UnsignedSequence& makeSelection10();
    s_baltst::UnsignedSequence& makeSelection10(const s_baltst::UnsignedSequence& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::UnsignedSequence& makeSelection10(s_baltst::UnsignedSequence&& value);
#endif
        // Set the value of this object to be a "Selection10" value.
        // Optionally specify the 'value' of the "Selection10".  If 'value' is
        // not specified, the default "Selection10" value is used.

    s_baltst::SequenceWithAnonymity& makeSelection11();
    s_baltst::SequenceWithAnonymity& makeSelection11(const s_baltst::SequenceWithAnonymity& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::SequenceWithAnonymity& makeSelection11(s_baltst::SequenceWithAnonymity&& value);
#endif
        // Set the value of this object to be a "Selection11" value.
        // Optionally specify the 'value' of the "Selection11".  If 'value' is
        // not specified, the default "Selection11" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    s_baltst::Sequence1& selection1();
        // Return a reference to the modifiable "Selection1" selection of this
        // object if "Selection1" is the current selection.  The behavior is
        // undefined unless "Selection1" is the selection of this object.

    bsl::vector<char>& selection2();
        // Return a reference to the modifiable "Selection2" selection of this
        // object if "Selection2" is the current selection.  The behavior is
        // undefined unless "Selection2" is the selection of this object.

    s_baltst::Sequence2& selection3();
        // Return a reference to the modifiable "Selection3" selection of this
        // object if "Selection3" is the current selection.  The behavior is
        // undefined unless "Selection3" is the selection of this object.

    s_baltst::Sequence3& selection4();
        // Return a reference to the modifiable "Selection4" selection of this
        // object if "Selection4" is the current selection.  The behavior is
        // undefined unless "Selection4" is the selection of this object.

    bdlt::DatetimeTz& selection5();
        // Return a reference to the modifiable "Selection5" selection of this
        // object if "Selection5" is the current selection.  The behavior is
        // undefined unless "Selection5" is the selection of this object.

    s_baltst::CustomString& selection6();
        // Return a reference to the modifiable "Selection6" selection of this
        // object if "Selection6" is the current selection.  The behavior is
        // undefined unless "Selection6" is the selection of this object.

    s_baltst::Enumerated::Value& selection7();
        // Return a reference to the modifiable "Selection7" selection of this
        // object if "Selection7" is the current selection.  The behavior is
        // undefined unless "Selection7" is the selection of this object.

    s_baltst::Choice3& selection8();
        // Return a reference to the modifiable "Selection8" selection of this
        // object if "Selection8" is the current selection.  The behavior is
        // undefined unless "Selection8" is the selection of this object.

    s_baltst::VoidSequence& selection9();
        // Return a reference to the modifiable "Selection9" selection of this
        // object if "Selection9" is the current selection.  The behavior is
        // undefined unless "Selection9" is the selection of this object.

    s_baltst::UnsignedSequence& selection10();
        // Return a reference to the modifiable "Selection10" selection of this
        // object if "Selection10" is the current selection.  The behavior is
        // undefined unless "Selection10" is the selection of this object.

    s_baltst::SequenceWithAnonymity& selection11();
        // Return a reference to the modifiable "Selection11" selection of this
        // object if "Selection11" is the current selection.  The behavior is
        // undefined unless "Selection11" is the selection of this object.

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

    const s_baltst::Sequence1& selection1() const;
        // Return a reference to the non-modifiable "Selection1" selection of
        // this object if "Selection1" is the current selection.  The behavior
        // is undefined unless "Selection1" is the selection of this object.

    const bsl::vector<char>& selection2() const;
        // Return a reference to the non-modifiable "Selection2" selection of
        // this object if "Selection2" is the current selection.  The behavior
        // is undefined unless "Selection2" is the selection of this object.

    const s_baltst::Sequence2& selection3() const;
        // Return a reference to the non-modifiable "Selection3" selection of
        // this object if "Selection3" is the current selection.  The behavior
        // is undefined unless "Selection3" is the selection of this object.

    const s_baltst::Sequence3& selection4() const;
        // Return a reference to the non-modifiable "Selection4" selection of
        // this object if "Selection4" is the current selection.  The behavior
        // is undefined unless "Selection4" is the selection of this object.

    const bdlt::DatetimeTz& selection5() const;
        // Return a reference to the non-modifiable "Selection5" selection of
        // this object if "Selection5" is the current selection.  The behavior
        // is undefined unless "Selection5" is the selection of this object.

    const s_baltst::CustomString& selection6() const;
        // Return a reference to the non-modifiable "Selection6" selection of
        // this object if "Selection6" is the current selection.  The behavior
        // is undefined unless "Selection6" is the selection of this object.

    const s_baltst::Enumerated::Value& selection7() const;
        // Return a reference to the non-modifiable "Selection7" selection of
        // this object if "Selection7" is the current selection.  The behavior
        // is undefined unless "Selection7" is the selection of this object.

    const s_baltst::Choice3& selection8() const;
        // Return a reference to the non-modifiable "Selection8" selection of
        // this object if "Selection8" is the current selection.  The behavior
        // is undefined unless "Selection8" is the selection of this object.

    const s_baltst::VoidSequence& selection9() const;
        // Return a reference to the non-modifiable "Selection9" selection of
        // this object if "Selection9" is the current selection.  The behavior
        // is undefined unless "Selection9" is the selection of this object.

    const s_baltst::UnsignedSequence& selection10() const;
        // Return a reference to the non-modifiable "Selection10" selection of
        // this object if "Selection10" is the current selection.  The behavior
        // is undefined unless "Selection10" is the selection of this object.

    const s_baltst::SequenceWithAnonymity& selection11() const;
        // Return a reference to the non-modifiable "Selection11" selection of
        // this object if "Selection11" is the current selection.  The behavior
        // is undefined unless "Selection11" is the selection of this object.

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

    bool isSelection5Value() const;
        // Return 'true' if the value of this object is a "Selection5" value,
        // and return 'false' otherwise.

    bool isSelection6Value() const;
        // Return 'true' if the value of this object is a "Selection6" value,
        // and return 'false' otherwise.

    bool isSelection7Value() const;
        // Return 'true' if the value of this object is a "Selection7" value,
        // and return 'false' otherwise.

    bool isSelection8Value() const;
        // Return 'true' if the value of this object is a "Selection8" value,
        // and return 'false' otherwise.

    bool isSelection9Value() const;
        // Return 'true' if the value of this object is a "Selection9" value,
        // and return 'false' otherwise.

    bool isSelection10Value() const;
        // Return 'true' if the value of this object is a "Selection10" value,
        // and return 'false' otherwise.

    bool isSelection11Value() const;
        // Return 'true' if the value of this object is a "Selection11" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const FeatureTestMessage& lhs, const FeatureTestMessage& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'FeatureTestMessage' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const FeatureTestMessage& lhs, const FeatureTestMessage& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const FeatureTestMessage& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::FeatureTestMessage)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                          // ------------------------
                          // class FeatureTestMessage
                          // ------------------------

// CLASS METHODS
// CREATORS
inline
FeatureTestMessage::FeatureTestMessage(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
FeatureTestMessage::~FeatureTestMessage()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int FeatureTestMessage::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case FeatureTestMessage::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case FeatureTestMessage::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case FeatureTestMessage::SELECTION_ID_SELECTION3:
        return manipulator(&d_selection3.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case FeatureTestMessage::SELECTION_ID_SELECTION4:
        return manipulator(d_selection4,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      case FeatureTestMessage::SELECTION_ID_SELECTION5:
        return manipulator(&d_selection5.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5]);
      case FeatureTestMessage::SELECTION_ID_SELECTION6:
        return manipulator(&d_selection6.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6]);
      case FeatureTestMessage::SELECTION_ID_SELECTION7:
        return manipulator(&d_selection7.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7]);
      case FeatureTestMessage::SELECTION_ID_SELECTION8:
        return manipulator(&d_selection8.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8]);
      case FeatureTestMessage::SELECTION_ID_SELECTION9:
        return manipulator(&d_selection9.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION9]);
      case FeatureTestMessage::SELECTION_ID_SELECTION10:
        return manipulator(&d_selection10.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION10]);
      case FeatureTestMessage::SELECTION_ID_SELECTION11:
        return manipulator(&d_selection11.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION11]);
      default:
        BSLS_ASSERT(FeatureTestMessage::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
s_baltst::Sequence1& FeatureTestMessage::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
bsl::vector<char>& FeatureTestMessage::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
s_baltst::Sequence2& FeatureTestMessage::selection3()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
s_baltst::Sequence3& FeatureTestMessage::selection4()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

inline
bdlt::DatetimeTz& FeatureTestMessage::selection5()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
s_baltst::CustomString& FeatureTestMessage::selection6()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

inline
s_baltst::Enumerated::Value& FeatureTestMessage::selection7()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION7 == d_selectionId);
    return d_selection7.object();
}

inline
s_baltst::Choice3& FeatureTestMessage::selection8()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION8 == d_selectionId);
    return d_selection8.object();
}

inline
s_baltst::VoidSequence& FeatureTestMessage::selection9()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION9 == d_selectionId);
    return d_selection9.object();
}

inline
s_baltst::UnsignedSequence& FeatureTestMessage::selection10()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION10 == d_selectionId);
    return d_selection10.object();
}

inline
s_baltst::SequenceWithAnonymity& FeatureTestMessage::selection11()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION11 == d_selectionId);
    return d_selection11.object();
}

// ACCESSORS
inline
int FeatureTestMessage::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int FeatureTestMessage::accessSelection(ACCESSOR& accessor) const
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
        return accessor(*d_selection4,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      case SELECTION_ID_SELECTION5:
        return accessor(d_selection5.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5]);
      case SELECTION_ID_SELECTION6:
        return accessor(d_selection6.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6]);
      case SELECTION_ID_SELECTION7:
        return accessor(d_selection7.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7]);
      case SELECTION_ID_SELECTION8:
        return accessor(d_selection8.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8]);
      case SELECTION_ID_SELECTION9:
        return accessor(d_selection9.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION9]);
      case SELECTION_ID_SELECTION10:
        return accessor(d_selection10.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION10]);
      case SELECTION_ID_SELECTION11:
        return accessor(d_selection11.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION11]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const s_baltst::Sequence1& FeatureTestMessage::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const bsl::vector<char>& FeatureTestMessage::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const s_baltst::Sequence2& FeatureTestMessage::selection3() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
const s_baltst::Sequence3& FeatureTestMessage::selection4() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return *d_selection4;
}

inline
const bdlt::DatetimeTz& FeatureTestMessage::selection5() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
const s_baltst::CustomString& FeatureTestMessage::selection6() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

inline
const s_baltst::Enumerated::Value& FeatureTestMessage::selection7() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION7 == d_selectionId);
    return d_selection7.object();
}

inline
const s_baltst::Choice3& FeatureTestMessage::selection8() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION8 == d_selectionId);
    return d_selection8.object();
}

inline
const s_baltst::VoidSequence& FeatureTestMessage::selection9() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION9 == d_selectionId);
    return d_selection9.object();
}

inline
const s_baltst::UnsignedSequence& FeatureTestMessage::selection10() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION10 == d_selectionId);
    return d_selection10.object();
}

inline
const s_baltst::SequenceWithAnonymity& FeatureTestMessage::selection11() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION11 == d_selectionId);
    return d_selection11.object();
}

inline
bool FeatureTestMessage::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection3Value() const
{
    return SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection4Value() const
{
    return SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection5Value() const
{
    return SELECTION_ID_SELECTION5 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection6Value() const
{
    return SELECTION_ID_SELECTION6 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection7Value() const
{
    return SELECTION_ID_SELECTION7 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection8Value() const
{
    return SELECTION_ID_SELECTION8 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection9Value() const
{
    return SELECTION_ID_SELECTION9 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection10Value() const
{
    return SELECTION_ID_SELECTION10 == d_selectionId;
}

inline
bool FeatureTestMessage::isSelection11Value() const
{
    return SELECTION_ID_SELECTION11 == d_selectionId;
}

inline
bool FeatureTestMessage::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::FeatureTestMessage& lhs,
        const s_baltst::FeatureTestMessage& rhs)
{
    typedef s_baltst::FeatureTestMessage Class;
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
          case Class::SELECTION_ID_SELECTION5:
            return lhs.selection5() == rhs.selection5();
          case Class::SELECTION_ID_SELECTION6:
            return lhs.selection6() == rhs.selection6();
          case Class::SELECTION_ID_SELECTION7:
            return lhs.selection7() == rhs.selection7();
          case Class::SELECTION_ID_SELECTION8:
            return lhs.selection8() == rhs.selection8();
          case Class::SELECTION_ID_SELECTION9:
            return lhs.selection9() == rhs.selection9();
          case Class::SELECTION_ID_SELECTION10:
            return lhs.selection10() == rhs.selection10();
          case Class::SELECTION_ID_SELECTION11:
            return lhs.selection11() == rhs.selection11();
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
        const s_baltst::FeatureTestMessage& lhs,
        const s_baltst::FeatureTestMessage& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::FeatureTestMessage& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_featuretestmessage.xsd --mode msg --includedir . --msgComponent featuretestmessage --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
