// s_baltst_sequencewithanonymity.h     *DO NOT EDIT*      @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_SEQUENCEWITHANONYMITY
#define INCLUDED_S_BALTST_SEQUENCEWITHANONYMITY

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_sequencewithanonymity_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bdlb_nullablevalue.h>

#include <bsl_string.h>

#include <s_baltst_customint.h>

#include <s_baltst_customstring.h>

#include <s_baltst_ratsnest.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class SequenceWithAnonymityChoice1; }
namespace s_baltst { class SequenceWithAnonymityChoice; }
namespace s_baltst { class SequenceWithAnonymityChoice2; }
namespace s_baltst { class SequenceWithAnonymity; }
namespace s_baltst {

                     // ==================================
                     // class SequenceWithAnonymityChoice1
                     // ==================================

class SequenceWithAnonymityChoice1 {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< bool >        d_selection5;
        bsls::ObjectBuffer< bsl::string > d_selection6;
    };

    int                                   d_selectionId;
    bslma::Allocator                     *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION5 = 0
      , SELECTION_ID_SELECTION6 = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_SELECTION5 = 0
      , SELECTION_INDEX_SELECTION6 = 1
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
    explicit SequenceWithAnonymityChoice1(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice1' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    SequenceWithAnonymityChoice1(const SequenceWithAnonymityChoice1& original,
                                bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice1' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymityChoice1(SequenceWithAnonymityChoice1&& original) noexcept;
        // Create an object of type 'SequenceWithAnonymityChoice1' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    SequenceWithAnonymityChoice1(SequenceWithAnonymityChoice1&& original,
                                bslma::Allocator *basicAllocator);
        // Create an object of type 'SequenceWithAnonymityChoice1' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~SequenceWithAnonymityChoice1();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymityChoice1& operator=(const SequenceWithAnonymityChoice1& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymityChoice1& operator=(SequenceWithAnonymityChoice1&& rhs);
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

    bool& makeSelection5();
    bool& makeSelection5(bool value);
        // Set the value of this object to be a "Selection5" value.  Optionally
        // specify the 'value' of the "Selection5".  If 'value' is not
        // specified, the default "Selection5" value is used.

    bsl::string& makeSelection6();
    bsl::string& makeSelection6(const bsl::string& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::string& makeSelection6(bsl::string&& value);
#endif
        // Set the value of this object to be a "Selection6" value.  Optionally
        // specify the 'value' of the "Selection6".  If 'value' is not
        // specified, the default "Selection6" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    bool& selection5();
        // Return a reference to the modifiable "Selection5" selection of this
        // object if "Selection5" is the current selection.  The behavior is
        // undefined unless "Selection5" is the selection of this object.

    bsl::string& selection6();
        // Return a reference to the modifiable "Selection6" selection of this
        // object if "Selection6" is the current selection.  The behavior is
        // undefined unless "Selection6" is the selection of this object.

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

    const bool& selection5() const;
        // Return a reference to the non-modifiable "Selection5" selection of
        // this object if "Selection5" is the current selection.  The behavior
        // is undefined unless "Selection5" is the selection of this object.

    const bsl::string& selection6() const;
        // Return a reference to the non-modifiable "Selection6" selection of
        // this object if "Selection6" is the current selection.  The behavior
        // is undefined unless "Selection6" is the selection of this object.

    bool isSelection5Value() const;
        // Return 'true' if the value of this object is a "Selection5" value,
        // and return 'false' otherwise.

    bool isSelection6Value() const;
        // Return 'true' if the value of this object is a "Selection6" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const SequenceWithAnonymityChoice1& lhs, const SequenceWithAnonymityChoice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'SequenceWithAnonymityChoice1' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const SequenceWithAnonymityChoice1& lhs, const SequenceWithAnonymityChoice1& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceWithAnonymityChoice1& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::SequenceWithAnonymityChoice1)

namespace s_baltst {

                     // =================================
                     // class SequenceWithAnonymityChoice
                     // =================================

class SequenceWithAnonymityChoice {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< s_baltst::Sequence6 >    d_selection1;
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
    explicit SequenceWithAnonymityChoice(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    SequenceWithAnonymityChoice(const SequenceWithAnonymityChoice& original,
                               bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymityChoice(SequenceWithAnonymityChoice&& original) noexcept;
        // Create an object of type 'SequenceWithAnonymityChoice' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    SequenceWithAnonymityChoice(SequenceWithAnonymityChoice&& original,
                               bslma::Allocator *basicAllocator);
        // Create an object of type 'SequenceWithAnonymityChoice' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~SequenceWithAnonymityChoice();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymityChoice& operator=(const SequenceWithAnonymityChoice& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymityChoice& operator=(SequenceWithAnonymityChoice&& rhs);
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

    s_baltst::Sequence6& makeSelection1();
    s_baltst::Sequence6& makeSelection1(const s_baltst::Sequence6& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::Sequence6& makeSelection1(s_baltst::Sequence6&& value);
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

    s_baltst::Sequence6& selection1();
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

    const s_baltst::Sequence6& selection1() const;
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
bool operator==(const SequenceWithAnonymityChoice& lhs, const SequenceWithAnonymityChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'SequenceWithAnonymityChoice' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const SequenceWithAnonymityChoice& lhs, const SequenceWithAnonymityChoice& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceWithAnonymityChoice& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::SequenceWithAnonymityChoice)

namespace s_baltst {

                     // ==================================
                     // class SequenceWithAnonymityChoice2
                     // ==================================

class SequenceWithAnonymityChoice2 {

    // INSTANCE DATA
    union {
        s_baltst::Sequence4   *d_selection7;
        s_baltst::Choice2     *d_selection8;
    };

    int                         d_selectionId;
    bslma::Allocator           *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED  = -1
      , SELECTION_ID_SELECTION7 = 0
      , SELECTION_ID_SELECTION8 = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_SELECTION7 = 0
      , SELECTION_INDEX_SELECTION8 = 1
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
    explicit SequenceWithAnonymityChoice2(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice2' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    SequenceWithAnonymityChoice2(const SequenceWithAnonymityChoice2& original,
                                bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymityChoice2' having the
        // value of the specified 'original' object.  Use the optionally
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymityChoice2(SequenceWithAnonymityChoice2&& original) noexcept;
        // Create an object of type 'SequenceWithAnonymityChoice2' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.

    SequenceWithAnonymityChoice2(SequenceWithAnonymityChoice2&& original,
                                bslma::Allocator *basicAllocator);
        // Create an object of type 'SequenceWithAnonymityChoice2' having the
        // value of the specified 'original' object.  After performing this
        // action, the 'original' object will be left in a valid, but
        // unspecified state.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.
#endif

    ~SequenceWithAnonymityChoice2();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymityChoice2& operator=(const SequenceWithAnonymityChoice2& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymityChoice2& operator=(SequenceWithAnonymityChoice2&& rhs);
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

    s_baltst::Sequence4& makeSelection7();
    s_baltst::Sequence4& makeSelection7(const s_baltst::Sequence4& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::Sequence4& makeSelection7(s_baltst::Sequence4&& value);
#endif
        // Set the value of this object to be a "Selection7" value.  Optionally
        // specify the 'value' of the "Selection7".  If 'value' is not
        // specified, the default "Selection7" value is used.

    s_baltst::Choice2& makeSelection8();
    s_baltst::Choice2& makeSelection8(const s_baltst::Choice2& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::Choice2& makeSelection8(s_baltst::Choice2&& value);
#endif
        // Set the value of this object to be a "Selection8" value.  Optionally
        // specify the 'value' of the "Selection8".  If 'value' is not
        // specified, the default "Selection8" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    s_baltst::Sequence4& selection7();
        // Return a reference to the modifiable "Selection7" selection of this
        // object if "Selection7" is the current selection.  The behavior is
        // undefined unless "Selection7" is the selection of this object.

    s_baltst::Choice2& selection8();
        // Return a reference to the modifiable "Selection8" selection of this
        // object if "Selection8" is the current selection.  The behavior is
        // undefined unless "Selection8" is the selection of this object.

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

    const s_baltst::Sequence4& selection7() const;
        // Return a reference to the non-modifiable "Selection7" selection of
        // this object if "Selection7" is the current selection.  The behavior
        // is undefined unless "Selection7" is the selection of this object.

    const s_baltst::Choice2& selection8() const;
        // Return a reference to the non-modifiable "Selection8" selection of
        // this object if "Selection8" is the current selection.  The behavior
        // is undefined unless "Selection8" is the selection of this object.

    bool isSelection7Value() const;
        // Return 'true' if the value of this object is a "Selection7" value,
        // and return 'false' otherwise.

    bool isSelection8Value() const;
        // Return 'true' if the value of this object is a "Selection8" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const SequenceWithAnonymityChoice2& lhs, const SequenceWithAnonymityChoice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'SequenceWithAnonymityChoice2' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const SequenceWithAnonymityChoice2& lhs, const SequenceWithAnonymityChoice2& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceWithAnonymityChoice2& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::SequenceWithAnonymityChoice2)

namespace s_baltst {

                        // ===========================
                        // class SequenceWithAnonymity
                        // ===========================

class SequenceWithAnonymity {

    // INSTANCE DATA
    s_baltst::Sequence6                                d_element4;
    bdlb::NullableValue<SequenceWithAnonymityChoice2>  d_choice2;
    SequenceWithAnonymityChoice1                       d_choice1;
    SequenceWithAnonymityChoice                        d_choice;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_CHOICE   = 0
      , ATTRIBUTE_ID_CHOICE1  = 1
      , ATTRIBUTE_ID_CHOICE2  = 2
      , ATTRIBUTE_ID_ELEMENT4 = 3
    };

    enum {
        NUM_ATTRIBUTES = 4
    };

    enum {
        ATTRIBUTE_INDEX_CHOICE   = 0
      , ATTRIBUTE_INDEX_CHOICE1  = 1
      , ATTRIBUTE_INDEX_CHOICE2  = 2
      , ATTRIBUTE_INDEX_ELEMENT4 = 3
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
    explicit SequenceWithAnonymity(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymity' having the default
        // value.  Use the optionally specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    SequenceWithAnonymity(const SequenceWithAnonymity& original,
                          bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'SequenceWithAnonymity' having the value of
        // the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymity(SequenceWithAnonymity&& original) noexcept;
        // Create an object of type 'SequenceWithAnonymity' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    SequenceWithAnonymity(SequenceWithAnonymity&& original,
                          bslma::Allocator *basicAllocator);
        // Create an object of type 'SequenceWithAnonymity' having the value of
        // the specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~SequenceWithAnonymity();
        // Destroy this object.

    // MANIPULATORS
    SequenceWithAnonymity& operator=(const SequenceWithAnonymity& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    SequenceWithAnonymity& operator=(SequenceWithAnonymity&& rhs);
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

    SequenceWithAnonymityChoice& choice();
        // Return a reference to the modifiable "Choice" attribute of this
        // object.

    SequenceWithAnonymityChoice1& choice1();
        // Return a reference to the modifiable "Choice1" attribute of this
        // object.

    bdlb::NullableValue<SequenceWithAnonymityChoice2>& choice2();
        // Return a reference to the modifiable "Choice2" attribute of this
        // object.

    s_baltst::Sequence6& element4();
        // Return a reference to the modifiable "Element4" attribute of this
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

    const SequenceWithAnonymityChoice& choice() const;
        // Return a reference offering non-modifiable access to the "Choice"
        // attribute of this object.

    const SequenceWithAnonymityChoice1& choice1() const;
        // Return a reference offering non-modifiable access to the "Choice1"
        // attribute of this object.

    const bdlb::NullableValue<SequenceWithAnonymityChoice2>& choice2() const;
        // Return a reference offering non-modifiable access to the "Choice2"
        // attribute of this object.

    const s_baltst::Sequence6& element4() const;
        // Return a reference offering non-modifiable access to the "Element4"
        // attribute of this object.
};

// FREE OPERATORS
inline
bool operator==(const SequenceWithAnonymity& lhs, const SequenceWithAnonymity& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const SequenceWithAnonymity& lhs, const SequenceWithAnonymity& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const SequenceWithAnonymity& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::SequenceWithAnonymity)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                     // ----------------------------------
                     // class SequenceWithAnonymityChoice1
                     // ----------------------------------

// CLASS METHODS
// CREATORS
inline
SequenceWithAnonymityChoice1::SequenceWithAnonymityChoice1(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
SequenceWithAnonymityChoice1::~SequenceWithAnonymityChoice1()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int SequenceWithAnonymityChoice1::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case SequenceWithAnonymityChoice1::SELECTION_ID_SELECTION5:
        return manipulator(&d_selection5.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5]);
      case SequenceWithAnonymityChoice1::SELECTION_ID_SELECTION6:
        return manipulator(&d_selection6.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6]);
      default:
        BSLS_ASSERT(SequenceWithAnonymityChoice1::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
bool& SequenceWithAnonymityChoice1::selection5()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
bsl::string& SequenceWithAnonymityChoice1::selection6()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

// ACCESSORS
inline
int SequenceWithAnonymityChoice1::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int SequenceWithAnonymityChoice1::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION5:
        return accessor(d_selection5.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION5]);
      case SELECTION_ID_SELECTION6:
        return accessor(d_selection6.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION6]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const bool& SequenceWithAnonymityChoice1::selection5() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION5 == d_selectionId);
    return d_selection5.object();
}

inline
const bsl::string& SequenceWithAnonymityChoice1::selection6() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION6 == d_selectionId);
    return d_selection6.object();
}

inline
bool SequenceWithAnonymityChoice1::isSelection5Value() const
{
    return SELECTION_ID_SELECTION5 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice1::isSelection6Value() const
{
    return SELECTION_ID_SELECTION6 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice1::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                     // ---------------------------------
                     // class SequenceWithAnonymityChoice
                     // ---------------------------------

// CLASS METHODS
// CREATORS
inline
SequenceWithAnonymityChoice::SequenceWithAnonymityChoice(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
SequenceWithAnonymityChoice::~SequenceWithAnonymityChoice()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int SequenceWithAnonymityChoice::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case SequenceWithAnonymityChoice::SELECTION_ID_SELECTION1:
        return manipulator(&d_selection1.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION1]);
      case SequenceWithAnonymityChoice::SELECTION_ID_SELECTION2:
        return manipulator(&d_selection2.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION2]);
      case SequenceWithAnonymityChoice::SELECTION_ID_SELECTION3:
        return manipulator(&d_selection3.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION3]);
      case SequenceWithAnonymityChoice::SELECTION_ID_SELECTION4:
        return manipulator(&d_selection4.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION4]);
      default:
        BSLS_ASSERT(SequenceWithAnonymityChoice::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
s_baltst::Sequence6& SequenceWithAnonymityChoice::selection1()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
unsigned char& SequenceWithAnonymityChoice::selection2()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
s_baltst::CustomString& SequenceWithAnonymityChoice::selection3()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
s_baltst::CustomInt& SequenceWithAnonymityChoice::selection4()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

// ACCESSORS
inline
int SequenceWithAnonymityChoice::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int SequenceWithAnonymityChoice::accessSelection(ACCESSOR& accessor) const
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
const s_baltst::Sequence6& SequenceWithAnonymityChoice::selection1() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION1 == d_selectionId);
    return d_selection1.object();
}

inline
const unsigned char& SequenceWithAnonymityChoice::selection2() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION2 == d_selectionId);
    return d_selection2.object();
}

inline
const s_baltst::CustomString& SequenceWithAnonymityChoice::selection3() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION3 == d_selectionId);
    return d_selection3.object();
}

inline
const s_baltst::CustomInt& SequenceWithAnonymityChoice::selection4() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION4 == d_selectionId);
    return d_selection4.object();
}

inline
bool SequenceWithAnonymityChoice::isSelection1Value() const
{
    return SELECTION_ID_SELECTION1 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice::isSelection2Value() const
{
    return SELECTION_ID_SELECTION2 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice::isSelection3Value() const
{
    return SELECTION_ID_SELECTION3 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice::isSelection4Value() const
{
    return SELECTION_ID_SELECTION4 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                     // ----------------------------------
                     // class SequenceWithAnonymityChoice2
                     // ----------------------------------

// CLASS METHODS
// CREATORS
inline
SequenceWithAnonymityChoice2::SequenceWithAnonymityChoice2(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
SequenceWithAnonymityChoice2::~SequenceWithAnonymityChoice2()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int SequenceWithAnonymityChoice2::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case SequenceWithAnonymityChoice2::SELECTION_ID_SELECTION7:
        return manipulator(d_selection7,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7]);
      case SequenceWithAnonymityChoice2::SELECTION_ID_SELECTION8:
        return manipulator(d_selection8,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8]);
      default:
        BSLS_ASSERT(SequenceWithAnonymityChoice2::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
s_baltst::Sequence4& SequenceWithAnonymityChoice2::selection7()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION7 == d_selectionId);
    return *d_selection7;
}

inline
s_baltst::Choice2& SequenceWithAnonymityChoice2::selection8()
{
    BSLS_ASSERT(SELECTION_ID_SELECTION8 == d_selectionId);
    return *d_selection8;
}

// ACCESSORS
inline
int SequenceWithAnonymityChoice2::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int SequenceWithAnonymityChoice2::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SELECTION7:
        return accessor(*d_selection7,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION7]);
      case SELECTION_ID_SELECTION8:
        return accessor(*d_selection8,
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SELECTION8]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const s_baltst::Sequence4& SequenceWithAnonymityChoice2::selection7() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION7 == d_selectionId);
    return *d_selection7;
}

inline
const s_baltst::Choice2& SequenceWithAnonymityChoice2::selection8() const
{
    BSLS_ASSERT(SELECTION_ID_SELECTION8 == d_selectionId);
    return *d_selection8;
}

inline
bool SequenceWithAnonymityChoice2::isSelection7Value() const
{
    return SELECTION_ID_SELECTION7 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice2::isSelection8Value() const
{
    return SELECTION_ID_SELECTION8 == d_selectionId;
}

inline
bool SequenceWithAnonymityChoice2::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}


                        // ---------------------------
                        // class SequenceWithAnonymity
                        // ---------------------------

// CLASS METHODS
// MANIPULATORS
template <class MANIPULATOR>
int SequenceWithAnonymity::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_choice2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class MANIPULATOR>
int SequenceWithAnonymity::manipulateAttribute(MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHOICE: {
        return manipulator(&d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      case ATTRIBUTE_ID_CHOICE1: {
        return manipulator(&d_choice1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1]);
      }
      case ATTRIBUTE_ID_CHOICE2: {
        return manipulator(&d_choice2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return manipulator(&d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
int SequenceWithAnonymity::manipulateAttribute(
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
SequenceWithAnonymityChoice& SequenceWithAnonymity::choice()
{
    return d_choice;
}

inline
SequenceWithAnonymityChoice1& SequenceWithAnonymity::choice1()
{
    return d_choice1;
}

inline
bdlb::NullableValue<SequenceWithAnonymityChoice2>& SequenceWithAnonymity::choice2()
{
    return d_choice2;
}

inline
s_baltst::Sequence6& SequenceWithAnonymity::element4()
{
    return d_element4;
}

// ACCESSORS
template <class ACCESSOR>
int SequenceWithAnonymity::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_choice2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <class ACCESSOR>
int SequenceWithAnonymity::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_CHOICE: {
        return accessor(d_choice, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE]);
      }
      case ATTRIBUTE_ID_CHOICE1: {
        return accessor(d_choice1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE1]);
      }
      case ATTRIBUTE_ID_CHOICE2: {
        return accessor(d_choice2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_CHOICE2]);
      }
      case ATTRIBUTE_ID_ELEMENT4: {
        return accessor(d_element4, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ELEMENT4]);
      }
      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
int SequenceWithAnonymity::accessAttribute(
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
const SequenceWithAnonymityChoice& SequenceWithAnonymity::choice() const
{
    return d_choice;
}

inline
const SequenceWithAnonymityChoice1& SequenceWithAnonymity::choice1() const
{
    return d_choice1;
}

inline
const bdlb::NullableValue<SequenceWithAnonymityChoice2>& SequenceWithAnonymity::choice2() const
{
    return d_choice2;
}

inline
const s_baltst::Sequence6& SequenceWithAnonymity::element4() const
{
    return d_element4;
}

}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::SequenceWithAnonymityChoice1& lhs,
        const s_baltst::SequenceWithAnonymityChoice1& rhs)
{
    typedef s_baltst::SequenceWithAnonymityChoice1 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION5:
            return lhs.selection5() == rhs.selection5();
          case Class::SELECTION_ID_SELECTION6:
            return lhs.selection6() == rhs.selection6();
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
        const s_baltst::SequenceWithAnonymityChoice1& lhs,
        const s_baltst::SequenceWithAnonymityChoice1& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::SequenceWithAnonymityChoice1& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::SequenceWithAnonymityChoice& lhs,
        const s_baltst::SequenceWithAnonymityChoice& rhs)
{
    typedef s_baltst::SequenceWithAnonymityChoice Class;
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
        const s_baltst::SequenceWithAnonymityChoice& lhs,
        const s_baltst::SequenceWithAnonymityChoice& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::SequenceWithAnonymityChoice& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::SequenceWithAnonymityChoice2& lhs,
        const s_baltst::SequenceWithAnonymityChoice2& rhs)
{
    typedef s_baltst::SequenceWithAnonymityChoice2 Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SELECTION7:
            return lhs.selection7() == rhs.selection7();
          case Class::SELECTION_ID_SELECTION8:
            return lhs.selection8() == rhs.selection8();
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
        const s_baltst::SequenceWithAnonymityChoice2& lhs,
        const s_baltst::SequenceWithAnonymityChoice2& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::SequenceWithAnonymityChoice2& rhs)
{
    return rhs.print(stream, 0, -1);
}


inline
bool s_baltst::operator==(
        const s_baltst::SequenceWithAnonymity& lhs,
        const s_baltst::SequenceWithAnonymity& rhs)
{
    return  lhs.choice() == rhs.choice()
         && lhs.choice1() == rhs.choice1()
         && lhs.choice2() == rhs.choice2()
         && lhs.element4() == rhs.element4();
}

inline
bool s_baltst::operator!=(
        const s_baltst::SequenceWithAnonymity& lhs,
        const s_baltst::SequenceWithAnonymity& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::SequenceWithAnonymity& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_sequencewithanonymity.xsd --mode msg --includedir . --msgComponent sequencewithanonymity --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
