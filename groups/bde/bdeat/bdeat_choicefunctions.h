// bdeat_choicefunctions.h                                            -*-C++-*-
#ifndef INCLUDED_BDEAT_CHOICEFUNCTIONS
#define INCLUDED_BDEAT_CHOICEFUNCTIONS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining choice functions.
//
//@CLASSES:
//  bdeat_ChoiceFunctions: namespace for calling choice functions
//
//@SEE_ALSO: bdeat_selectioninfo
//
//@AUTHOR: Clay Wilson (cwilson9), Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
// TBD: update this documentation to reflect the new overloaded functions
//
//@DESCRIPTION: The 'bdeat_ChoiceFunctions' 'namespace' provided in this
// component defines parameterized functions that expose "choice" behavior for
// "choice" types.  See the package-level documentation for a full description
// of "choice" types.  The functions in this namespace allow users to:
//..
//  o make a selection using either a selection id or a selection name
//    ('makeSelection').
//  o manipulate the current selection using a parameterized manipulator
//    ('manipulateSelection').
//  o access the current selection using a parameterized accessor
//    ('accessSelection').
//  o obtain the id for the current selection ('selectionId').
//..
// Also, the meta-function 'IsChoice' contains a compile-time constant 'VALUE'
// that is non-zero if the parameterized 'TYPE' exposes "choice" behavior
// through the 'bdeat_ChoiceFunctions' 'namespace'.
//
// This component specializes all of these functions for types that have the
// 'bdeat_TypeTraitBasicChoice' trait.
//
// Types that do not have the 'bdeat_TypeTraitBasicChoice' trait can be
// plugged into the bdeat framework.  This is done by overloading the
// 'bdeat_choice*' functions inside the namespace of the plugged in type.
// For example, suppose there is a type called 'mine::MyChoice'.  In order to
// plug this type into the 'bdeat' framework as a "Choice", the following
// functions must be declared and implemented in the 'mine' namespace:
//..
//  // MANIPULATORS
//  int bdeat_choiceMakeSelection(MyChoice *object, int selectionId);
//      // Set the value of the specified 'object' to be the default for
//      // the selection indicated by the specified 'selectionId'.  Return
//      // 0 on success, and non-zero value otherwise (i.e., the selection
//      // is not found).
//
//  int bdeat_choiceMakeSelection(MyChoice  *object,
//                                const char *selectionName,
//                                int         selectionNameLength);
//      // Set the value of the specified 'object' to be the default for
//      // the selection indicated by the specified 'selectionName' of the
//      // specified 'selectionNameLength'.  Return 0 on success, and
//      // non-zero value otherwise (i.e., the selection is not found).
//
//  template <typename MANIPULATOR>
//  int bdeat_choiceManipulateSelection(MyChoice *object,
//                                      MANIPULATOR& manipulator);
//      // Invoke the specified 'manipulator' on the address of the
//      // (modifiable) selection of the specified 'object', supplying
//      // 'manipulator' with the corresponding selection information
//      // structure.  Return -1 if the selection is undefined, and the
//      // value returned from the invocation of 'manipulator' otherwise.
//
//  // ACCESSORS
//  template <typename ACCESSOR>
//  int bdeat_choiceAccessSelection(const MyChoice& object,
//                                  ACCESSOR& accessor);
//      // Invoke the specified 'accessor' on the (non-modifiable)
//      // selection of the specified 'object', supplying 'accessor' with
//      // the corresponding selection information structure.  Return -1 if
//      // the selection is undefined, and the value returned from the
//      // invocation of 'accessor' otherwise.
//
//  int bdeat_choiceSelectionId(const MyChoice& object);
//      // Return the id of the current selection if the selection is
//      // defined, and 0 otherwise.
//..
// Also, the 'IsChoice' meta-function must be specialized for the
// 'mine::MyChoice' type in the 'bdeat_ChoiceFunctions' namespace.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you had a 'union' embedded inside a 'struct'.  The 'struct' also
// contains a 'd_selectionId' member that specifies which member of the 'union'
// is selected.  The default constructor of the 'struct' makes the selection
// undefined:
//..
//  #include <bdeat_choicefunctions.h>
//  #include <bdeat_selectioninfo.h>
//  #include <bdeat_formattingmode.h>
//  #include <bdeu_string.h>
//  #include <bsls_assert.h>
//
//  namespace BloombergLP {
//
//  namespace mine {
//
//  struct MyChoice {
//      // This struct represents a choice between a 'char' value, an 'int'
//      // value, and a 'float' value.
//
//      // CONSTANTS
//      enum {
//          UNDEFINED_SELECTION_ID = -1,
//          CHAR_SELECTION_ID      = 0,
//          INT_SELECTION_ID       = 1,
//          FLOAT_SELECTION_ID     = 2
//      };
//
//      // DATA MEMBERS
//      union {
//          char  d_charValue;
//          int   d_intValue;
//          float d_floatValue;
//      };
//      int d_selectionId;
//
//      // CREATORS
//      MyChoice()
//      : d_selectionId(UNDEFINED_SELECTION_ID)
//      {
//      }
//  };
//..
// We can now make 'MyChoice' expose "choice" behavior by implementing
// 'bdeat_ChoiceFunctions' for 'MyChoice'.  First, we should forward declare
// all the functions that we will implement inside the 'mine' namespace:
//..
//      // MANIPULATORS
//      int bdeat_choiceMakeSelection(MyChoice *object, int selectionId);
//          // Set the value of the specified 'object' to be the default for
//          // the selection indicated by the specified 'selectionId'.  Return
//          // 0 on success, and non-zero value otherwise (i.e., the selection
//          // is not found).
//
//      int bdeat_choiceMakeSelection(MyChoice   *object,
//                                    const char *selectionName,
//                                    int         selectionNameLength);
//          // Set the value of the specified 'object' to be the default for
//          // the selection indicated by the specified 'selectionName' of the
//          // specified 'selectionNameLength'.  Return 0 on success, and
//          // non-zero value otherwise (i.e., the selection is not found).
//
//      template <typename MANIPULATOR>
//      int bdeat_choiceManipulateSelection(MyChoice     *object,
//                                          MANIPULATOR&  manipulator);
//          // Invoke the specified 'manipulator' on the address of the
//          // (modifiable) selection of the specified 'object', supplying
//          // 'manipulator' with the corresponding selection information
//          // structure.  Return -1 if the selection is undefined, and the
//          // value returned from the invocation of 'manipulator' otherwise.
//
//      // ACCESSORS
//      template <typename ACCESSOR>
//      int bdeat_choiceAccessSelection(const MyChoice& object,
//                                      ACCESSOR&       accessor);
//          // Invoke the specified 'accessor' on the (non-modifiable)
//          // selection of the specified 'object', supplying 'accessor' with
//          // the corresponding selection information structure.  Return -1 if
//          // the selection is undefined, and the value returned from the
//          // invocation of 'accessor' otherwise.
//
//      int bdeat_choiceSelectionId(const MyChoice& object);
//          // Return the id of the current selection if the selection is
//          // defined, and 0 otherwise.
//
//  }  // close namespace mine
//..
// Next, we provide the definitions for each of these functions:
//..
//  // MANIPULATORS
//
//  inline
//  int mine::bdeat_choiceMakeSelection(MyChoice *object,
//                                      int        selectionId)
//  {
//      enum { SUCCESS = 0, NOT_FOUND = -1 };
//
//      switch (selectionId) {
//        case MyChoice::CHAR_SELECTION_ID: {
//          object->d_selectionId = selectionId;
//          object->d_charValue   = 0;
//
//          return SUCCESS;
//        }
//        case MyChoice::INT_SELECTION_ID: {
//          object->d_selectionId = selectionId;
//          object->d_intValue    = 0;
//
//          return SUCCESS;
//        }
//        case MyChoice::FLOAT_SELECTION_ID: {
//          object->d_selectionId = selectionId;
//          object->d_floatValue  = 0;
//
//          return SUCCESS;
//        }
//        case MyChoice::UNDEFINED_SELECTION_ID: {
//          object->d_selectionId = selectionId;
//
//          return SUCCESS;
//        }
//        default: {
//          return NOT_FOUND;
//        }
//      }
//  }
//
//  inline
//  int mine::bdeat_choiceMakeSelection(MyChoice   *object,
//                                      const char *selectionName,
//                                      int         selectionNameLength)
//  {
//    enum { NOT_FOUND = -1 };
//
//    if (bdeu_String::areEqualCaseless("charValue",
//                                      selectionName,
//                                      selectionNameLength)) {
//      return bdeat_choiceMakeSelection(object, MyChoice::CHAR_SELECTION_ID);
//    }
//
//    if (bdeu_String::areEqualCaseless("intValue",
//                                      selectionName,
//                                      selectionNameLength)) {
//      return bdeat_choiceMakeSelection(object, MyChoice::INT_SELECTION_ID);
//    }
//
//    if (bdeu_String::areEqualCaseless("floatValue",
//                                      selectionName,
//                                      selectionNameLength)) {
//      return bdeat_choiceMakeSelection(object, MyChoice::FLOAT_SELECTION_ID);
//    }
//
//    return NOT_FOUND;
//  }
//..
// For the 'manipulateSelection' and 'accessSelection' functions, we need to
// create a temporary 'bdeat_SelectionInfo' object and pass it along when
// invoking the manipulator or accessor.  See the 'bdeat_selectioninfo'
// component-level documentation for more information.  The implementation of
// the remaining functions are as follows:
//..
//  template <typename MANIPULATOR>
//  int mine::bdeat_choiceManipulateSelection(MyChoice    *object,
//                                            MANIPULATOR&  manipulator)
//  {
//    switch (object->d_selectionId) {
//      case MyChoice::CHAR_SELECTION_ID: {
//        bdeat_SelectionInfo info;
//
//        info.annotation()     = "Char Selection";
//        info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//        info.id()             = MyChoice::CHAR_SELECTION_ID;
//        info.name()           = "charValue";
//        info.nameLength()     = 9;
//
//        return manipulator(&object->d_charValue, info);
//      }
//      case MyChoice::INT_SELECTION_ID: {
//        bdeat_SelectionInfo info;
//
//        info.annotation()     = "Int Selection";
//        info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//        info.id()             = MyChoice::INT_SELECTION_ID;
//        info.name()           = "intValue";
//        info.nameLength()     = 8;
//
//        return manipulator(&object->d_intValue, info);
//      }
//      case MyChoice::FLOAT_SELECTION_ID: {
//        bdeat_SelectionInfo info;
//
//        info.annotation()     = "Float Selection";
//        info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//        info.id()             = MyChoice::FLOAT_SELECTION_ID;
//        info.name()           = "floatValue";
//        info.nameLength()     = 10;
//
//        return manipulator(&object->d_floatValue, info);
//      }
//      default:
//        BSLS_ASSERT_SAFE(!"Invalid selection!");
//    }
//    return 0;
//  }
//
//  // ACCESSORS
//
//  template <typename ACCESSOR>
//  int mine::bdeat_choiceAccessSelection(const MyChoice& object,
//                                        ACCESSOR&        accessor)
//  {
//      switch (object.d_selectionId) {
//        case MyChoice::CHAR_SELECTION_ID: {
//          bdeat_SelectionInfo info;
//
//          info.annotation()     = "Char Selection";
//          info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//          info.id()             = MyChoice::CHAR_SELECTION_ID;
//          info.name()           = "charValue";
//          info.nameLength()     = 9;
//
//          return accessor(object.d_charValue, info);
//        }
//        case MyChoice::INT_SELECTION_ID: {
//          bdeat_SelectionInfo info;
//
//          info.annotation()     = "Int Selection";
//          info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//          info.id()             = MyChoice::INT_SELECTION_ID;
//          info.name()           = "intValue";
//          info.nameLength()     = 8;
//
//          return accessor(object.d_intValue, info);
//        }
//        case MyChoice::FLOAT_SELECTION_ID: {
//          bdeat_SelectionInfo info;
//
//          info.annotation()     = "Float Selection";
//          info.formattingMode() = bdeat_FormattingMode::DEFAULT;
//          info.id()             = MyChoice::FLOAT_SELECTION_ID;
//          info.name()           = "floatValue";
//          info.nameLength()     = 10;
//
//          return accessor(object.d_floatValue, info);
//        }
//        default:
//          BSLS_ASSERT_SAFE(!"Invalid selection!");
//      }
//    return 0;
//  }
//
//  inline
//  int mine::bdeat_choiceSelectionId(const MyChoice& object)
//  {
//      return object.d_selectionId;
//  }
//..
// Finally, we need to specialize the 'IsChoice' meta-function in the
// 'bdeat_ChoiceFunctions' namespace for the 'mine::MyChoice' type.  This
// makes the 'bdeat' infrastructure recognize 'mine::MyChoice' as a choice
// abstraction:
//..
//  namespace bdeat_ChoiceFunctions {
//
//      template <>
//      struct IsChoice<mine::MyChoice> {
//          enum { VALUE = 1 };
//      };
//
//  } // close namespace 'bdeat_ChoiceFunctions'
//  } // close namespace 'BloombergLP'
//..
// The 'bdeat' infrastructure (and any component that uses this infrastructure)
// will now recognize 'mine::MyChoice' as a "choice" type.  For example,
// suppose we have the following XML data:
//..
//  <?xml version='1.0' encoding='UTF-8' ?>
//  <MyChoice>
//      <intValue>321</intValue>
//  </MyChoice>
//..
// Using the 'baexml_decoder' component, we can load this XML data into a
// 'mine::MyChoice' object:
//..
//  #include <baexml_decoder.h>
//
//  void decodeMySequenceFromXML(bsl::istream& inputData)
//  {
//      using namespace BloombergLP;
//
//      MyChoice object;
//
//      assert(MyChoice::UNDEFINED_SELECTION_ID == object.d_selectionId);
//
//      baexml_DecoderOptions options;
//      baexml_MiniReader     reader;
//      baexml_ErrorInfo      errInfo;
//
//      baexml_Decoder decoder(&options, &reader, &errInfo);
//      int result = decoder.decode(inputData, &object);
//
//      assert(0                          == result);
//      assert(MyChoice::INT_SELECTION_ID == object.d_selectionId);
//      assert(321                        == object.d_intValue);
//  }
//..
// Note that the 'bdeat' framework can be used for functionality other than
// encoding/decoding into XML.  When 'mine::MyChoice' is plugged into the
// framework, then it will be automatically usable within the framework.  For
// example, the following snippets of code will print out the selection value
// of a choice object:
//..
//  struct PrintSelection {
//      // Print each visited object to the bound 'd_stream_p' object.
//
//      // DATA MEMBERS
//      bsl::ostream *d_stream_p;
//
//      template <typename TYPE, typename INFO>
//      int operator()(const TYPE& object, const INFO& info)
//      {
//        (*d_stream_p) << info.name() << ": " << object << bsl::endl;
//        return 0;
//      }
//  };
//
//  template <typename TYPE>
//  void printChoiceSelection(bsl::ostream& stream, const TYPE& object)
//  {
//      using namespace BloombergLP;
//
//      PrintSelection accessor;
//      accessor.d_stream_p = &stream;
//
//      bdeat_choiceAccessSelection(object, accessor);
//  }
//..
// Now we have a generic function that takes an output stream and a choice
// object, and prints out the choice selection with its name and value.  We can
// use this generic function as follows:
//..
//  void printMyChoice(bsl::ostream& stream)
//  {
//      using namespace BloombergLP;
//
//      mine::MyChoice object;
//
//      object.d_selectionId = mine::MyChoice::INT_SELECTION_ID;
//      object.d_intValue    = 321;
//
//      printChoiceSelection(stream, object);
//  }
//..
// The function above will print the following to provided stream:
//..
//  intValue: 321
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

                      // ===============================
                      // namespace bdeat_ChoiceFunctions
                      // ===============================

namespace bdeat_ChoiceFunctions {
    // This 'namespace' provides functions that expose "choice" behavior for
    // "choice" types.  See the component-level documentation for more
    // information.

    // CONSTANTS
    enum {
        BDEAT_UNDEFINED_SELECTION_ID = -1  // indicates selection not made

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , UNDEFINED_SELECTION_ID = BDEAT_UNDEFINED_SELECTION_ID
#endif
    };

    // META-FUNCTIONS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    template <typename TYPE>
    bslmf_MetaInt<0> isChoiceMetaFunction(const TYPE&);
        // This function can be overloaded to support partial specialization
        // (Sun5.2 compiler is unable to partially specialize the 'struct'
        // below).  Note that this function is has no definition and should not
        // be called at runtime.
        //
        // *DEPRECATED*: Specialize the 'IsChoice' meta-function instead.

#endif
    template <typename TYPE>
    struct IsChoice {
        // This 'struct' should be specialized for third-party types that need
        // to expose "choice" behavior.  See the component-level documentation
        // for further information.

        enum {
            VALUE = bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
                 || BSLMF_METAINT_TO_BOOL(isChoiceMetaFunction(
                                                   bslmf_TypeRep<TYPE>::rep()))
#endif
        };
    };

    // MANIPULATORS
    template <typename TYPE>
    int makeSelection(TYPE *object, int selectionId);
        // Set the value of the specified 'object' to be the default for the
        // selection indicated by the specified 'selectionId'.  Return 0 on
        // success, and non-zero value otherwise (i.e., the selection is not
        // found).

    template <typename TYPE>
    int makeSelection(TYPE       *object,
                      const char *selectionName,
                      int         selectionNameLength);
        // Set the value of the specified 'object' to be the default for the
        // selection indicated by the specified 'selectionName' of the
        // specified 'selectionNameLength'.  Return 0 on success, and non-zero
        // value otherwise (i.e., the selection is not found).

    template <typename TYPE, typename MANIPULATOR>
    int manipulateSelection(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) selection of the specified 'object', supplying
        // 'manipulator' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'manipulator'.  The behavior is undefined unless
        // 'BDEAT_UNDEFINED_SELECTION_ID != selectionId(*object)'.

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int accessSelection(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor' on the (non-modifiable) selection of
        // the specified 'object', supplying 'accessor' with the corresponding
        // selection information structure.  Return the value returned from the
        // invocation of 'accessor'.  The behavior is undefined unless
        // 'BDEAT_UNDEFINED_SELECTION_ID != selectionId(object)'.

    template <typename TYPE>
    bool hasSelection(const TYPE&  object,
                      const char  *selectionName,
                      int          selectionNameLength);
        // Return true if the specified 'object' has a selection with the
        // specified 'selectionName' of the specified 'selectionNameLength',
        // and false otherwise.

    template <typename TYPE>
    bool hasSelection(const TYPE& object,
                      int         selectionId);
        // Return true if the specified 'object' has a selection with the
        // specified 'selectionId', and false otherwise.

    template <typename TYPE>
    int selectionId(const TYPE& object);
        // Return the id of the current selection if the selection is defined,
        // and BDEAT_UNDEFINED_SELECTION_ID otherwise.

#if ! defined(BSLS_PLATFORM__CMP_IBM)
    // OVERLOADABLE FUNCTIONS
    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

    // MANIPULATORS
    template <typename TYPE>
    int bdeat_choiceMakeSelection(TYPE *object, int selectionId);
    template <typename TYPE>
    int bdeat_choiceMakeSelection(TYPE       *object,
                                  const char *selectionName,
                                  int         selectionNameLength);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_choiceManipulateSelection(TYPE         *object,
                                        MANIPULATOR&  manipulator);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdeat_choiceAccessSelection(const TYPE& object, ACCESSOR& accessor);
    template <typename TYPE>
    bool bdeat_choiceHasSelection(const TYPE&  object,
                                  const char  *selectionName,
                                  int          selectionNameLength);
    template <typename TYPE>
    bool bdeat_choiceHasSelection(const TYPE& object,
                                  int         selectionId);
    template <typename TYPE>
    int bdeat_choiceSelectionId(const TYPE& object);
#endif

}  // close namespace bdeat_ChoiceFunctions

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // -------------------------------
                      // namespace bdeat_ChoiceFunctions
                      // -------------------------------

// MANIPULATORS

template <typename TYPE>
inline
int bdeat_ChoiceFunctions::makeSelection(TYPE *object, int selectionId)
{
    return bdeat_choiceMakeSelection(object, selectionId);
}

template <typename TYPE>
inline
int bdeat_ChoiceFunctions::makeSelection(TYPE       *object,
                                         const char *selectionName,
                                         int         selectionNameLength)
{
    return bdeat_choiceMakeSelection(object,
                                     selectionName,
                                     selectionNameLength);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_ChoiceFunctions::manipulateSelection(TYPE         *object,
                                               MANIPULATOR&  manipulator)
{
    BSLS_ASSERT_SAFE(BDEAT_UNDEFINED_SELECTION_ID
                                          != bdeat_choiceSelectionId(*object));

    return bdeat_choiceManipulateSelection(object, manipulator);
}

// ACCESSORS

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_ChoiceFunctions::accessSelection(const TYPE& object,
                                           ACCESSOR&   accessor)
{
    BSLS_ASSERT_SAFE(BDEAT_UNDEFINED_SELECTION_ID
                                           != bdeat_choiceSelectionId(object));

    return bdeat_choiceAccessSelection(object, accessor);
}

template <typename TYPE>
inline
bool bdeat_ChoiceFunctions::hasSelection(const TYPE&  object,
                                         const char  *selectionName,
                                         int          selectionNameLength)
{
    return bdeat_choiceHasSelection(object,
                                    selectionName,
                                    selectionNameLength);
}

template <typename TYPE>
inline
bool bdeat_ChoiceFunctions::hasSelection(const TYPE& object,
                                         int         selectionId)
{
    return bdeat_choiceHasSelection(object, selectionId);
}

template <typename TYPE>
inline
int bdeat_ChoiceFunctions::selectionId(const TYPE& object)
{
    return bdeat_choiceSelectionId(object);
}

          // --------------------------------------------------------
          // namespace bdeat_ChoiceFunctions (OVERLOADABLE FUNCTIONS)
          // --------------------------------------------------------

#if defined(BSLS_PLATFORM__CMP_IBM)
namespace bdeat_ChoiceFunctions {
    // xlC 6 will not do Koenig (argument-dependent) lookup is the function
    // being called has already been declared in some scope at the point of
    // the template function *definition* (not instantiation).  We work around
    // this bug by not declaring these functions until *after* the template
    // definitions that call them.

    // MANIPULATORS
    template <typename TYPE>
    int bdeat_choiceMakeSelection(TYPE *object, int selectionId);
    template <typename TYPE>
    int bdeat_choiceMakeSelection(TYPE       *object,
                                  const char *selectionName,
                                  int         selectionNameLength);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_choiceManipulateSelection(TYPE         *object,
                                        MANIPULATOR&  manipulator);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdeat_choiceAccessSelection(const TYPE& object, ACCESSOR& accessor);
    template <typename TYPE>
    bool bdeat_choiceHasSelection(const TYPE&  object,
                                  const char  *selectionName,
                                  int          selectionNameLength);
    template <typename TYPE>
    bool bdeat_choiceHasSelection(const TYPE& object,
                                  int         selectionId);
    template <typename TYPE>
    int bdeat_choiceSelectionId(const TYPE& object);
}  // close namespace bdeat_ChoiceFunctions
#endif

// MANIPULATORS

template <typename TYPE>
inline
int bdeat_ChoiceFunctions::bdeat_choiceMakeSelection(TYPE *object,
                                                     int   selectionId)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE));

    return object->makeSelection(selectionId);
}

template <typename TYPE>
inline
int bdeat_ChoiceFunctions::bdeat_choiceMakeSelection(
                                               TYPE       *object,
                                               const char *selectionName,
                                               int         selectionNameLength)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE));

    return object->makeSelection(selectionName, selectionNameLength);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_ChoiceFunctions::bdeat_choiceManipulateSelection(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE));

    return object->manipulateSelection(manipulator);
}

// ACCESSORS

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_ChoiceFunctions::bdeat_choiceAccessSelection(const TYPE& object,
                                                       ACCESSOR&   accessor)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE));

    return object.accessSelection(accessor);
}

// VC2008 does not detect that address is used.
#ifdef BSLS_PLATFORM__CMP_MSVC
#pragma warning( push )
#pragma warning( disable : 4100 )
#endif

template <typename TYPE>
inline
bool bdeat_ChoiceFunctions::bdeat_choiceHasSelection(
                                              const TYPE&  object,
                                              const char  *selectionName,
                                              int          selectionNameLength)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE));

    return 0 != object.lookupSelectionInfo(selectionName, selectionNameLength);
}

template <typename TYPE>
inline
bool bdeat_ChoiceFunctions::bdeat_choiceHasSelection(const TYPE& object,
                                                     int         selectionId)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE));

    return 0 != object.lookupSelectionInfo(selectionId);
}

#ifdef BSLS_PLATFORM__CMP_MSVC
#pragma warning( pop )
#endif

template <typename TYPE>
inline
int bdeat_ChoiceFunctions::bdeat_choiceSelectionId(const TYPE& object)
{
    BSLMF_ASSERT((bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE));

    return object.selectionId();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
