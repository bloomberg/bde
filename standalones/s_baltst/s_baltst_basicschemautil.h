// s_baltst_basicschemautil.h                                         -*-C++-*-
#ifndef INCLUDED_S_BALTST_BASICSCHEMAUTIL
#define INCLUDED_S_BALTST_BASICSCHEMAUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for 's_baltst::BasicSchema' objects.
//
//@CLASSES:
//  s_baltst::BasicSchemaUtil: Namespace for utilities for basic schema objects
//
//@SEE_ALSO: balxml_configschema, bdem_configschema
//
//@DESCRIPTION: This component defines a 'struct',
// 's_baltst::BasicSchemaUtil', that acts as a namespace for a suite of
// utility functions providing non-primitive operations on
// 's_baltst::BasicSchema' objects.
//
///Checking Structural Equivalence Up To Annotation
///------------------------------------------------
// The most important operation provided by 's_baltst::BasicSchemaUtil' is
// 'areEqualUpToAnnotation', which compares 2 basic schema objects and returns
// 'true' if they are structurally equal except for the presence or absence of
// annotations.  This can be used, for example, to test that a basic schema
// object parsed from an XML schema definition has an expected structure,
// without worrying about having to check the structure of its documentation.

#include <bdlat_choicefunctions.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_typecategory.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isconst.h>

#include <bsls_assert.h>

#include <s_baltst_basicschema.h>

namespace BloombergLP {
namespace s_baltst {

                           // ======================
                           // struct BasicSchemaUtil
                           // ======================

template <class ELEMENT_TYPE>
struct BasicSchemaUtil_IsElement;
    // forward declaration

template <class ELEMENT_TYPE>
struct BasicSchemaUtil_IsElementOrSchema;
    // forward declaration

struct BasicSchemaUtil {
    // This utility 'struct' provides a namespace for a suite of functions for
    // non-primitive operations on 's_baltst::BasicSchema' objects.

  private:
    // PRIVATE TYPES
    template <class ELEMENT_TYPE>
    struct IsElement : BasicSchemaUtil_IsElement<ELEMENT_TYPE> {
        // This boolean-valued metafunction provides a member constant 'value'
        // that is equal to 'true' if the specified 'ELEMENT_TYPE' represents
        // an element, excluding the top-level schema element, in an XML Schema
        // Definition.  Otherwise, 'value' is equal to 'false'.
    };

    template <class ELEMENT_TYPE>
    struct IsElementOrSchema
    : BasicSchemaUtil_IsElementOrSchema<ELEMENT_TYPE> {
        // This boolean-valued metafunction provides a member constant 'value'
        // that is equal to 'true' if the specified 'ELEMENT_TYPE' represents
        // an element, including the top-level schema element, in an XML Schema
        // Definition.  Otherwise, 'value' is equal to 'false'.
    };

  public:
    // CLASS METHODS
    template <class PARENT_TYPE, class CHILD_TYPE>
    static typename bsl::enable_if<!bsl::is_const<PARENT_TYPE>::value &&
                                   IsElementOrSchema<PARENT_TYPE>::value &&
                                   IsElement<CHILD_TYPE>::value>::type
    append(PARENT_TYPE *parent, const CHILD_TYPE& element);
        // Insert the specified 'element' after the last element in the
        // 'choice' vector of the specified 'parent'.  This function
        // participates in overload resolution if 'PARENT_TYPE' is a
        // non-'const' type for which 'IsElementOrSchema<PARENT_TYPE>::value'
        // is 'true' and 'IsElement<CHILD_TYPE>::value' is 'true'.

    static void appendEnumeration(BasicSchemaRestrictionElement *parent,
                                  const bsl::string_view&        value);
        // Insert a 's_baltst::EnumerationElement' with the specified 'value'
        // after the last element in the 'choice' vector of the specified
        // 'parent'.

    static bool areEqualUpToAnnotation(const BasicSchema& a,
                                       const BasicSchema& b);
        // Return 'true' if the specified 'a' and 'b' are structurally
        // equivalent except for the presence, absence, or order of
        // annotations, and return false otherwise.
};

                      // ================================
                      // struct BasicSchemaUtil_IsElement
                      // ================================

template <class ELEMENT_TYPE>
struct BasicSchemaUtil_IsElement : bsl::false_type {
    // This boolean-valued metafunction provides a member constant 'value' that
    // is equal to 'true' if the specified 'ELEMENT_TYPE' represents an
    // element, excluding the top-level schema element, in an XML Schema
    // Definition.  Otherwise, 'value' is equal to 'false'.
};

template <>
struct BasicSchemaUtil_IsElement<BasicSchemaAnnotationElement>
: bsl::true_type {
};

template <>
struct BasicSchemaUtil_IsElement<BasicSchemaChoiceElement>
: bsl::true_type {
};

template <>
struct BasicSchemaUtil_IsElement<BasicSchemaComplexTypeElement>
: bsl::true_type {
};

template <>
struct BasicSchemaUtil_IsElement<BasicSchemaElement>
: bsl::true_type {
};

template <>
struct BasicSchemaUtil_IsElement<BasicSchemaEnumerationElement>
: bsl::true_type {
};

template <>
struct BasicSchemaUtil_IsElement<BasicSchemaRestrictionElement>
: bsl::true_type {
};

template <>
struct BasicSchemaUtil_IsElement<BasicSchemaSequenceElement>
: bsl::true_type {
};

template <>
struct BasicSchemaUtil_IsElement<BasicSchemaSimpleTypeElement>
: bsl::true_type {
};

                  // ========================================
                  // struct BasicSchemaUtil_IsElementOrSchema
                  // ========================================

template <class ELEMENT_TYPE>
struct BasicSchemaUtil_IsElementOrSchema
: BasicSchemaUtil_IsElement<ELEMENT_TYPE> {
    // This boolean-valued metafunction provides a member constant 'value' that
    // is equal to 'true' if the specified 'ELEMENT_TYPE' represents an
    // element, including the top-level schema element, in an XML Schema
    // Definition.  Otherwise, 'value' is equal to 'false'.
};

template <>
struct BasicSchemaUtil_IsElementOrSchema<BasicSchema> : bsl::true_type {
};

                      // ===============================
                      // struct BasicSchemaUtil_ImplUtil
                      // ===============================

struct BasicSchemaUtil_ImplUtil {
    // This component-private utility 'struct' provides implementation details
    // for 'BasicSchemaUtil'.

    // TYPES
    typedef BasicSchemaAnnotationElement  AnnotationElement;
    typedef BasicSchemaChoiceElement      ChoiceElement;
    typedef BasicSchemaComplexTypeElement ComplexTypeElement;
    typedef BasicSchemaElement            Element;
    typedef BasicSchemaEnumerationElement EnumerationElement;
    typedef BasicSchemaRestrictionElement RestrictionElement;
    typedef BasicSchemaSequenceElement    SequenceElement;
    typedef BasicSchemaSimpleTypeElement  SimpleTypeElement;

  private:
    // PRIVATE CLASS METHODS
    static void doRemoveAnnotations(AnnotationElement *element);
        // The behavior of this function is undefined.  It is provided for the
        // purpose of overload set formation.  Note that the specified
        // 'element' is not used.

    template <class ELEMENT_TYPE>
    static void doRemoveAnnotations(ELEMENT_TYPE *element);
        // Remove all annotation sub-elements from the specified 'element'.

    template <class ELEMENT_CHOICE_FORWARD_ITERATOR>
    static ELEMENT_CHOICE_FORWARD_ITERATOR doRemoveAnnotations(
                           ELEMENT_CHOICE_FORWARD_ITERATOR beginElementChoices,
                           ELEMENT_CHOICE_FORWARD_ITERATOR endElementChoices);
        // Recursively remove all annotation sub-elements from the set of
        // element choices specified by the range '[beginElementChoices,
        // endElementChoices)', and return the past-the-end iterator for the
        // new range of element choices.  Note that the element re-ordering and
        // removal behavior of this function is similar to that of the
        // 'bsl::remove' algorithm.

  public:
    // CLASS METHODS
    template <class PARENT_TYPE>
    static void append(PARENT_TYPE *parent, const AnnotationElement& element);
    template <class PARENT_TYPE>
    static void append(PARENT_TYPE *parent, const ChoiceElement& element);
    template <class PARENT_TYPE>
    static void append(PARENT_TYPE *parent, const ComplexTypeElement& element);
    template <class PARENT_TYPE>
    static void append(PARENT_TYPE *parent, const Element& element);
    template <class PARENT_TYPE>
    static void append(PARENT_TYPE *parent, const EnumerationElement& element);
    template <class PARENT_TYPE>
    static void append(PARENT_TYPE *parent, const RestrictionElement& element);
    template <class PARENT_TYPE>
    static void append(PARENT_TYPE *parent, const SequenceElement& element);
    template <class PARENT_TYPE>
    static void append(PARENT_TYPE *parent, const SimpleTypeElement& element);
        // Insert the specified 'element' after the last element in the
        // 'choice' vector of the specified 'parent'.  The behavior is
        // undefined unless the specified 'PARENT_TYPE' parameter is one of the
        // non-"Choice"-suffixed types provided by the 's_baltst_basicschema'
        // component.

    template <class ELEMENT_CHOICE_TYPE>
    static bool isAnnotation(const ELEMENT_CHOICE_TYPE& choice);
        // Return 'true' if the specified 'choice' element choice is an
        // annotation element, and return 'false' otherwise.  The behavior is
        // undefined unless the specified 'ELEMENT_CHOICE_TYPE' parameter is
        // one of the "Choice"-suffixed types provided by
        // 's_baltst::BasicSchema'.

    template <class ELEMENT_TYPE>
    static void removeAnnotations(ELEMENT_TYPE *element);
        // Remove all annotation sub-elements from the specified 'element'.
};

               // =============================================
               // class BasicSchemaUtil_ChoiceAnnotationRemover
               // =============================================

class BasicSchemaUtil_ChoiceAnnotationRemover {
    // This function-object class provides a function-call operator that
    // removes all annotations from an element choice.

  public:
    // CREATORS
    BasicSchemaUtil_ChoiceAnnotationRemover();
        // Create a 'BasicSchemaUtil_ChoiceAnnotationRemover' object.

    // ACCESSORS
    template <class ELEMENT_CHOICE_TYPE>
    void operator()(ELEMENT_CHOICE_TYPE& element) const;
        // Recursively remove all annotation sub-elements from the specified
        // 'element'.  The behavior is undefined unless the specified
        // 'ELEMENT_CHOICE_TYPE' parameter is one of the "Choice"-suffixed
        // types provided by 's_baltst::BasicSchema'.
};

              // ================================================
              // class BasicSchemaUtil_SelectionAnnotationRemover
              // ================================================

class BasicSchemaUtil_SelectionAnnotationRemover {
    // This function-object class provides a function-call operator that
    // removes all annotations from an element that is the 'selection' of a
    // 'bdlat' 'Choice'-compatible object.

  public:
    // CREATORS
    BasicSchemaUtil_SelectionAnnotationRemover();
        // Create a 'BasicSchemaUtil_SelectionAnnotationRemover' object.

    // ACCESSORS
    template <class SELECTION_TYPE>
    int operator()(SELECTION_TYPE             *selection,
                   const bdlat_SelectionInfo&) const;
        // Recursively remove all annotation sub-elements from the specified
        // 'selection' element.  The behavior is undefined unless the specified
        // 'SELECTION_TYPE' is the 'selection' of a 'bdlat' 'Choice'-compatible
        // object, and is one of the non-"Choice"-suffixed types provided by
        // 's_baltst::BasicSchema'.
};

                // ===========================================
                // class BasicSchemaUtil_IsAnnotationPredicate
                // ===========================================

class BasicSchemaUtil_IsAnnotationPredicate {
    // This function-object class provides a function-call operator that
    // determines whether or not an object is an annotation element.

  public:
    // CREATORS
    BasicSchemaUtil_IsAnnotationPredicate();
        // Create a 'BasicSchemaUtil_IsAnnotationPredicate' object.

    // ACCESSORS
    template <class ELEMENT_CHOICE_TYPE>
    bool operator()(const ELEMENT_CHOICE_TYPE& element) const;
        // Return 'true' if the specified 'element' choice is an annotation
        // element, and return 'false' otherwise.  The behavior is undefined
        // unless the specified 'ELEMENT_CHOICE_TYPE' parameter is one of the
        // "Choice"-suffixed types provided by 's_baltst::BasicSchema'.
};

                    // ====================================
                    // struct BasicSchemaUtil_AlgorithmUtil
                    // ====================================

struct BasicSchemaUtil_AlgorithmUtil {
    // CLASS METHODS
    template <class SEQUENCE_CONTAINER>
    static void eraseToEnd(SEQUENCE_CONTAINER                    *container,
                           typename SEQUENCE_CONTAINER::iterator  position);
        // Erase all of the elements in the specified 'container' from the
        // specified 'position' iterator to the end of the container, as if by
        // 'container->erase(position, container.end())'.  The behavior is
        // undefined unless 'container->erase(position, container.end())' is
        // well-formed.

    template <class INPUT_ITERATOR, class TRANSFORM>
    static INPUT_ITERATOR forEach(INPUT_ITERATOR first,
                                  INPUT_ITERATOR last,
                                  TRANSFORM      transform);
        // Invoke the specified 'transform' on each object in the range
        // specified by '[first, last)' and return an input iterator equal to
        // 'last'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ----------------------------------------------------------------------------

                             // -----------------
                             // struct SchemaUtil
                             // -----------------

// CLASS METHODS
template <class PARENT_TYPE, class CHILD_TYPE>
typename bsl::enable_if<
    !bsl::is_const<PARENT_TYPE>::value &&
    BasicSchemaUtil::IsElementOrSchema<PARENT_TYPE>::value &&
    BasicSchemaUtil::IsElement<CHILD_TYPE>::value>::type
BasicSchemaUtil::append(PARENT_TYPE *parent, const CHILD_TYPE& child)
{
    BasicSchemaUtil_ImplUtil::append(parent, child);
}

void BasicSchemaUtil::appendEnumeration(BasicSchemaRestrictionElement *parent,
                                        const bsl::string_view&        value)
{
    BasicSchemaEnumerationElement enumeration;
    enumeration.value() = value;
    BasicSchemaUtil::append(parent, enumeration);
}

bool BasicSchemaUtil::areEqualUpToAnnotation(const BasicSchema& a,
                                             const BasicSchema& b)
{
    BasicSchema aCopy(a);
    BasicSchemaUtil_ImplUtil::removeAnnotations(&aCopy);

    BasicSchema bCopy(b);
    BasicSchemaUtil_ImplUtil::removeAnnotations(&bCopy);

    return aCopy == bCopy;
}

                      // -------------------------------
                      // struct BasicSchemaUtil_ImplUtil
                      // -------------------------------

// PRIVATE CLASS METHODS
void BasicSchemaUtil_ImplUtil::doRemoveAnnotations(AnnotationElement *)
{
    BSLS_ASSERT_INVOKE_NORETURN(
        "This overload of 'BasicSchemaUtil_ImplUtil::removeAnnotations' must "
        "not be invoked.");
}

template <class ELEMENT_TYPE>
void BasicSchemaUtil_ImplUtil::doRemoveAnnotations(ELEMENT_TYPE *element)
{
    BasicSchemaUtil_AlgorithmUtil::eraseToEnd(
        &element->choice(),
        BasicSchemaUtil_ImplUtil::doRemoveAnnotations(
            element->choice().begin(), element->choice().end()));
}

template <class ELEMENT_CHOICE_FORWARD_ITERATOR>
ELEMENT_CHOICE_FORWARD_ITERATOR BasicSchemaUtil_ImplUtil::doRemoveAnnotations(
                           ELEMENT_CHOICE_FORWARD_ITERATOR beginElementChoices,
                           ELEMENT_CHOICE_FORWARD_ITERATOR endElementChoices)
{
    const BasicSchemaUtil_IsAnnotationPredicate isAnnotation;
    const ELEMENT_CHOICE_FORWARD_ITERATOR       newEnd =
        std::remove_if(beginElementChoices, endElementChoices, isAnnotation);

    const BasicSchemaUtil_ChoiceAnnotationRemover removeAnnotations;
    return BasicSchemaUtil_AlgorithmUtil::forEach(
        beginElementChoices, newEnd, removeAnnotations);
}

// CLASS METHODS
template <class PARENT_TYPE>
void BasicSchemaUtil_ImplUtil::append(PARENT_TYPE              *parent,
                                      const AnnotationElement&  element)
{
    parent->choice().emplace_back();
    parent->choice().back().makeAnnotation(element);
}

template <class PARENT_TYPE>
void BasicSchemaUtil_ImplUtil::append(PARENT_TYPE          *parent,
                                      const ChoiceElement&  element)
{
    parent->choice().emplace_back();
    parent->choice().back().makeChoice(element);
}

template <class PARENT_TYPE>
void BasicSchemaUtil_ImplUtil::append(PARENT_TYPE               *parent,
                                      const ComplexTypeElement&  element)
{
    parent->choice().emplace_back();
    parent->choice().back().makeComplexType(element);
}

template <class PARENT_TYPE>
void BasicSchemaUtil_ImplUtil::append(PARENT_TYPE    *parent,
                                      const Element&  element)
{
    parent->choice().emplace_back();
    parent->choice().back().makeElement(element);
}

template <class PARENT_TYPE>
void BasicSchemaUtil_ImplUtil::append(PARENT_TYPE               *parent,
                                      const EnumerationElement&  element)
{
    parent->choice().emplace_back();
    parent->choice().back().makeEnumeration(element);
}

template <class PARENT_TYPE>
void BasicSchemaUtil_ImplUtil::append(PARENT_TYPE               *parent,
                                      const RestrictionElement&  element)
{
    parent->choice().emplace_back();
    parent->choice().back().makeRestriction(element);
}

template <class PARENT_TYPE>
void BasicSchemaUtil_ImplUtil::append(PARENT_TYPE            *parent,
                                      const SequenceElement&  element)
{
    parent->choice().emplace_back();
    parent->choice().back().makeSequence(element);
}

template <class PARENT_TYPE>
void BasicSchemaUtil_ImplUtil::append(PARENT_TYPE              *parent,
                                      const SimpleTypeElement&  element)
{
    parent->choice().emplace_back();
    parent->choice().back().makeSimpleType(element);
}

template <class ELEMENT_CHOICE_TYPE>
bool BasicSchemaUtil_ImplUtil::isAnnotation(const ELEMENT_CHOICE_TYPE& choice)
{
    return choice.isAnnotationValue();
}

template <class ELEMENT_TYPE>
void BasicSchemaUtil_ImplUtil::removeAnnotations(ELEMENT_TYPE *element)
{
    BasicSchemaUtil_ImplUtil::doRemoveAnnotations(element);
}

               // ----------------------------------------------
               // struct BasicSchemaUtil_ChoiceAnnotationRemover
               // ----------------------------------------------

// CREATORS
BasicSchemaUtil_ChoiceAnnotationRemover::
    BasicSchemaUtil_ChoiceAnnotationRemover()
{
}

// ACCESSORS
template <class ELEMENT_CHOICE_TYPE>
void BasicSchemaUtil_ChoiceAnnotationRemover::operator()(
                                            ELEMENT_CHOICE_TYPE& element) const
{
    BSLMF_ASSERT(
        bdlat_TypeCategory::Select<ELEMENT_CHOICE_TYPE>::e_SELECTION ==
        static_cast<unsigned>(bdlat_TypeCategory::e_CHOICE_CATEGORY));

    const BasicSchemaUtil_SelectionAnnotationRemover removeAnnotations;

    const int rc = bdlat_ChoiceFunctions::manipulateSelection(
        &element, removeAnnotations);

    BSLS_ASSERT(0 == rc);
}

              // ------------------------------------------------
              // class BasicSchemaUtil_SelectionAnnotationRemover
              // ------------------------------------------------

// CREATORS
BasicSchemaUtil_SelectionAnnotationRemover::
    BasicSchemaUtil_SelectionAnnotationRemover()
{
}

// ACCESSORS
template <class SELECTION_TYPE>
int BasicSchemaUtil_SelectionAnnotationRemover::operator()(
                                         SELECTION_TYPE             *selection,
                                         const bdlat_SelectionInfo&) const
{
    BasicSchemaUtil_ImplUtil::removeAnnotations(selection);
    return 0;
}

                // --------------------------------------------
                // struct BasicSchemaUtil_IsAnnotationPredicate
                // --------------------------------------------

// CREATORS
BasicSchemaUtil_IsAnnotationPredicate::BasicSchemaUtil_IsAnnotationPredicate()
{
}

// ACCESSORS
template <class ELEMENT_CHOICE_TYPE>
bool BasicSchemaUtil_IsAnnotationPredicate::operator()(
                                      const ELEMENT_CHOICE_TYPE& element) const
{
    return BasicSchemaUtil_ImplUtil::isAnnotation(element);
}

                    // ------------------------------------
                    // struct BasicSchemaUtil_AlgorithmUtil
                    // ------------------------------------

// CLASS METHODS
template <class SEQUENCE_CONTAINER>
void BasicSchemaUtil_AlgorithmUtil::eraseToEnd(
                              SEQUENCE_CONTAINER                    *container,
                              typename SEQUENCE_CONTAINER::iterator  position)
{
    container->erase(position, container->end());
}

template <class INPUT_ITERATOR, class TRANSFORM>
INPUT_ITERATOR BasicSchemaUtil_AlgorithmUtil::forEach(INPUT_ITERATOR first,
                                                      INPUT_ITERATOR last,
                                                      TRANSFORM      transform)
{
    for (; first != last; ++first) {
        transform(*first);
    }

    return first;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
