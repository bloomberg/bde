// s_baltst_testchoice.h                                              -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTCHOICE
#define INCLUDED_S_BALTST_TESTCHOICE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test implementation of a 'bdlat' "choice" type.
//
//@CLASSES:
//  s_baltst::TestChoice: test implementation of a 'bdlat' "choice" type

#include <bdlb_variant.h>

#include <bdlat_choicefunctions.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bslmf_integralconstant.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>

#include <bslalg_constructorproxy.h>

#include <s_baltst_testnilvalue.h>
#include <s_baltst_testselection.h>

namespace BloombergLP {
namespace s_baltst {

                         // ==========================
                         // struct TestChoice_ImplUtil
                         // ==========================

struct TestChoice_ImplUtil {

    // CLASS METHODS
    template <class ACCESSOR>
    static int access(ACCESSOR&                  accessor,
                      const TestNilValue         selection,
                      const bdlat_SelectionInfo& selectionInfo);
    template <class ACCESSOR, class SELECTION>
    static int access(ACCESSOR&                  accessor,
                      const SELECTION&           selection,
                      const bdlat_SelectionInfo& selectionInfo);

    template <class MANIPULATOR>
    static int manipulate(MANIPULATOR&                manipulator,
                          TestNilValue               *selection,
                          const bdlat_SelectionInfo&  selectionInfo);
    template <class MANIPULATOR, class SELECTION>
    static int manipulate(MANIPULATOR&                manipulator,
                          SELECTION                  *selection,
                          const bdlat_SelectionInfo&  selectionInfo);
};


                              // ================
                              // class TestChoice
                              // ================

template <class TYPED_SELECTION_0 =
              TypedTestSelection<TestNilValue, TestSelection<0> >,
          class TYPED_SELECTION_1 =
              TypedTestSelection<TestNilValue, TestSelection<0> >,
          class TYPED_SELECTION_2 =
              TypedTestSelection<TestNilValue, TestSelection<0> > >
class TestChoice {
    // This in-core value-semantic class provides a basic implementation of the
    // concept defined by the 'Choice' 'bdlat' type category.  The template
    // parameters 'TYPED_SELECTION_0', 'TYPED_SELECTION_1', and
    // 'TYPED_SELECTION_2' must all satisfy the following requirements:
    //: o The type must have two member type definitions, 'Type', and
    //:   'Selection'.
    //: o 'Type' must meet the requirements of an in-core value-semantic type.
    //: o 'Type' must meet the requirements of exactly one of the 'bdlat' value
    //:   categories.
    //: o 'Selection' must be a specialization of the 'TestSelection' type.
    // Further, each 'TestSelection' member type definition of a template
    // argument must return values for 'TestSelection::id()' and
    // 'TestSelection::name()' that are different from all others within this
    // 'TestChoice' specialization.
    //
    // Additionally, The 'Type' of any template argument may be 'TestNilValue'
    // if all 'Type' member type definitions of subsequent template arguments
    // are also 'TestNilValue'.
    //
    // The 'Type' and 'Selection' member type definitions of the template
    // arguments define the type and 'bdlat_SelectionInfo' of the selections of
    // the 'bdlat' 'Choice' implementation provided by this class.  A template
    // argument having a 'TestNilValue' 'Type' indicates that the corresponding
    // selection does not exist.

  public:
    // TYPES
    typedef typename TYPED_SELECTION_0::Selection Selection0;
    typedef typename TYPED_SELECTION_0::Type      Selection0Type;
    typedef typename TYPED_SELECTION_1::Selection Selection1;
    typedef typename TYPED_SELECTION_1::Type      Selection1Type;
    typedef typename TYPED_SELECTION_2::Selection Selection2;
    typedef typename TYPED_SELECTION_2::Type      Selection2Type;

  private:
    // PRIVATE TYPES
    typedef TestChoice_ImplUtil ImplUtil;
    typedef bdlb::Variant<Selection0Type, Selection1Type, Selection2Type>
        Value;

    // PRIVATE CLASS DATA
    enum {
        k_HAS_CHOICE_0 = !bslmf::IsSame<TestNilValue, Selection0Type>::value,
        k_HAS_CHOICE_1 = !bslmf::IsSame<TestNilValue, Selection1Type>::value,
        k_HAS_CHOICE_2 = !bslmf::IsSame<TestNilValue, Selection2Type>::value
    };

    // DATA
    bslalg::ConstructorProxy<Value> d_value;  // underlying value
    bslma::Allocator *d_allocator_p;  // memory supply (held, not owned)

  public:
    // CLASS METHODS
    static bool areEqual(const TestChoice& lhs, const TestChoice& rhs);

    // CREATORS
    TestChoice();

    explicit TestChoice(bslma::Allocator *basicAllocator);

    template <class VALUE>
    explicit TestChoice(const VALUE&      value,
                        bslma::Allocator *basicAllocator = 0);

    TestChoice(const TestChoice&  original,
               bslma::Allocator  *basicAllocator = 0);

    // MANIPULATORS
    TestChoice& operator=(const TestChoice& original);

    int makeSelection(int selectionId);

    int makeSelection(const char *selectionName, int selectionNameLength);

    template <class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);

    void reset();

    // ACCESSORS
    template <class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;

    bool hasSelection(int selectionId) const;

    bool hasSelection(const char *selectionName,
                      int         selectionNameLength) const;

    const char *className() const;

    int selectionId() const;

    const Selection0Type& theSelection0() const;

    const Selection1Type& theSelection1() const;

    const Selection2Type& theSelection2() const;
};

// FREE OPERATORS
template <class V0, class V1, class V2>
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const TestChoice<V0, V1, V2>& choice);

template <class C0, class C1, class C2>
inline
bool operator==(const TestChoice<C0, C1, C2>& lhs,
                const TestChoice<C0, C1, C2>& rhs);

template <class C0, class C1, class C2>
inline
bool operator!=(const TestChoice<C0, C1, C2>& lhs,
                const TestChoice<C0, C1, C2>& rhs);

// TRAITS
template <class C0, class C1, class C2>
const char *bdlat_TypeName_className(const TestChoice<C0, C1, C2>& object);

template <class C0, class C1, class C2>
int bdlat_choiceMakeSelection(TestChoice<C0, C1, C2> *object, int selectionId);


template <class C0, class C1, class C2>
int bdlat_choiceMakeSelection(TestChoice<C0, C1, C2> *object,
                              const char             *selectionName,
                              int                     selectionNameLength);

template <class C0, class C1, class C2, class MANIPULATOR>
int bdlat_choiceManipulateSelection(TestChoice<C0, C1, C2> *object,
                                    MANIPULATOR&            manipulator);

template <class C0, class C1, class C2, class ACCESSOR>
int bdlat_choiceAccessSelection(const TestChoice<C0, C1, C2>& object,
                                ACCESSOR&                     accessor);

template <class C0, class C1, class C2>
bool bdlat_choiceHasSelection(
                           const TestChoice<C0, C1, C2>&  object,
                           const char                    *selectionName,
                           int                            selectionNameLength);

template <class C0, class C1, class C2>
bool bdlat_choiceHasSelection(const TestChoice<C0, C1, C2>& object,
                              int                           selectionId);

template <class C0, class C1, class C2>
int bdlat_choiceSelectionId(const TestChoice<C0, C1, C2>& object);

template <class C0, class C1, class C2>
int bdlat_valueTypeAssign(TestChoice<C0, C1, C2>        *lhs,
                          const TestChoice<C0, C1, C2>&  rhs);

template <class C0, class C1, class C2>
void bdlat_valueTypeReset(TestChoice<C0, C1, C2> *object);

}  // close package namespace

namespace bdlat_ChoiceFunctions {

template <class C0, class C1, class C2>
struct IsChoice<s_baltst::TestChoice<C0, C1, C2> > {
    enum { VALUE = 1 };
};

}  // close namespace bdlat_ChoiceFunctions

namespace s_baltst {

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // struct TestChoice_ImplUtil
                         // --------------------------

// CLASS METHODS
template <class ACCESSOR>
int TestChoice_ImplUtil::access(ACCESSOR&,
                                const TestNilValue,
                                const bdlat_SelectionInfo&)
{
    return 0;
}

template <class ACCESSOR, class SELECTION>
int TestChoice_ImplUtil::access(ACCESSOR&                  accessor,
                                const SELECTION&           selection,
                                const bdlat_SelectionInfo& selectionInfo)
{
    return accessor(selection, selectionInfo);
}

template <class MANIPULATOR>
int TestChoice_ImplUtil::manipulate(MANIPULATOR&,
                                    TestNilValue               *,
                                    const bdlat_SelectionInfo&)
{
    return 0;
}

template <class MANIPULATOR, class SELECTION>
int TestChoice_ImplUtil::manipulate(MANIPULATOR&                manipulator,
                                    SELECTION                  *selection,
                                    const bdlat_SelectionInfo&  selectionInfo)
{
    return manipulator(selection, selectionInfo);
}

                              // ----------------
                              // class TestChoice
                              // ----------------

// CLASS METHODS
template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
bool TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    areEqual(const TestChoice& lhs, const TestChoice& rhs)
{
    return lhs.d_value.object() == rhs.d_value.object();
}

// CREATORS
template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    TestChoice()
: d_value(Selection0Type(), bslma::Default::allocator())
, d_allocator_p(bslma::Default::allocator())
{
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    TestChoice(bslma::Allocator *basicAllocator)
: d_value(Selection0Type(), basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
template <class VALUE>
TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    TestChoice(const VALUE& value, bslma::Allocator *basicAllocator)
: d_value(value, bslma::Default::allocator(basicAllocator))
, d_allocator_p(basicAllocator)
{
    BSLMF_ASSERT((bslmf::IsSame<Selection0Type, VALUE>::value ||
                  bslmf::IsSame<Selection1Type, VALUE>::value ||
                  bslmf::IsSame<Selection2Type, VALUE>::value));
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    TestChoice(const TestChoice& original, bslma::Allocator *basicAllocator)
: d_value(original.d_value.object(), basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>&
TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
operator=(const TestChoice& original)
{
    d_value.object() = original.d_value.object();
    return *this;
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
int TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    makeSelection(int selectionId)
    // Set the value of this object to the default for the selection indicated
    // by the specified 'selectionId'.  Return 0 on success, and a non-zero
    // value otherwise.
{
    switch (selectionId) {
      case 0: {
        if (!k_HAS_CHOICE_0) {
            return -1;
        }                                                             // RETURN

        d_value.object().template createInPlace<Selection0Type>();
        return 0;                                                     // RETURN
      } break;
      case 1: {
        if (!k_HAS_CHOICE_1) {
            return -1;                                                // RETURN
        }

        d_value.object().template createInPlace<Selection1Type>();
        return 0;                                                     // RETURN
      } break;
      case 2: {
        if (!k_HAS_CHOICE_2) {
            return -1;
        }                                                             // RETURN

        d_value.object().template createInPlace<Selection2Type>();
        return 0;                                                     // RETURN
      } break;
    }

    return -1;
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
int TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    makeSelection(const char *selectionName, int selectionNameLength)
    // Set the value of this object to the default for the selection indicated
    // by the specified 'selectionName' of the specified 'selectionNameLength'.
    // Return 0 on success, and a non-zero value otherwise.
{
    const bsl::string_view selection(selectionName, selectionNameLength);

    if (k_HAS_CHOICE_0 && Selection0::name() == selection) {
        d_value.object().template createInPlace<Selection0Type>();
        return 0;                                                     // RETURN
    }

    if (k_HAS_CHOICE_1 && Selection1::name() == selection) {
        d_value.object().template createInPlace<Selection1Type>();
        return 0;                                                     // RETURN
    }

    if (k_HAS_CHOICE_2 && Selection2::name() == selection) {
        d_value.object().template createInPlace<Selection2Type>();
        return 0;                                                     // RETURN
    }

    return -1;
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
template <class MANIPULATOR>
int TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    manipulateSelection(MANIPULATOR& manipulator)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // selection of this object, supplying 'manipulator' with the corresponding
    // selection information structure.  Return -1 if the selection is
    // undefined, and the value returned from the invocation of 'manipulator'
    // otherwise.
{
    switch (d_value.object().typeIndex()) {
      case 0: {
        return -1;                                                    // RETURN
      } break;
      case 1: {
        BSLS_ASSERT(k_HAS_CHOICE_0);
        BSLS_ASSERT(d_value.object().template is<Selection0Type>());
        return ImplUtil::manipulate(
            manipulator,
            &d_value.object().template the<Selection0Type>(),
            Selection0::selectionInfo());                             // RETURN
      } break;
      case 2: {
        BSLS_ASSERT(k_HAS_CHOICE_1);
        BSLS_ASSERT(d_value.object().template is<Selection1Type>());
        return ImplUtil::manipulate(
            manipulator,
            &d_value.object().template the<Selection1Type>(),
            Selection1::selectionInfo());                             // RETURN
      } break;
      case 3: {
        BSLS_ASSERT(k_HAS_CHOICE_2);
        BSLS_ASSERT(d_value.object().template is<Selection2Type>());
        return ImplUtil::manipulate(
            manipulator,
            &d_value.object().template the<Selection2Type>(),
            Selection2::selectionInfo());                             // RETURN
      } break;
    }

    return -1;
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
void TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    reset()
{
    d_value.object().template createInPlace<Selection0Type>();
}

// ACCESSORS

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
template <class ACCESSOR>
int TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    accessSelection(ACCESSOR& accessor) const
    // Invoke the specified 'accessor' on the (non-modifiable) selection of the
    // this object, supplying 'accessor' with the corresponding selection
    // information structure.  Return -1 if the selection is undefined, and the
    // value returned from the invocation of 'accessor' otherwise.
{
    switch (d_value.object().typeIndex()) {
      case 0: {
        return -1;                                                    // RETURN
      } break;
      case 1: {
        BSLS_ASSERT(k_HAS_CHOICE_0);
        BSLS_ASSERT(d_value.object().template is<Selection0Type>());
        return ImplUtil::access(
            accessor,
            d_value.object().template the<Selection0Type>(),
            Selection0::selectionInfo());                             // RETURN
      } break;
      case 2: {
        BSLS_ASSERT(k_HAS_CHOICE_1);
        BSLS_ASSERT(d_value.object().template is<Selection1Type>());
        return ImplUtil::access(
            accessor,
            d_value.object().template the<Selection1Type>(),
            Selection1::selectionInfo());                             // RETURN
      } break;
      case 3: {
        BSLS_ASSERT(k_HAS_CHOICE_2);
        BSLS_ASSERT(d_value.object().template is<Selection2Type>());
        return ImplUtil::access(
            accessor,
            d_value.object().template the<Selection2Type>(),
            Selection2::selectionInfo());                             // RETURN
      } break;
    }

    return -1;
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
bool TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    hasSelection(int selectionId) const
{
    if (k_HAS_CHOICE_0 && Selection0::id() == selectionId) {
        return true;                                                  // RETURN
    }

    if (k_HAS_CHOICE_1 && Selection1::id() == selectionId) {
        return true;                                                  // RETURN
    }

    if (k_HAS_CHOICE_2 && Selection2::id() == selectionId) {
        return true;                                                  // RETURN
    }

    return false;
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
bool TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    hasSelection(const char *selectionName, int selectionNameLength) const
{
    const bsl::string_view selection(selectionName, selectionNameLength);

    if (k_HAS_CHOICE_0 && Selection0::name() == selection) {
        return true;                                                  // RETURN
    }

    if (k_HAS_CHOICE_1 && Selection1::name() == selection) {
        return true;                                                  // RETURN
    }

    if (k_HAS_CHOICE_2 && Selection2::name() == selection) {
        return true;                                                  // RETURN
    }

    return false;
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
const char *TestChoice<TYPED_SELECTION_0,
                       TYPED_SELECTION_1,
                       TYPED_SELECTION_2>::className() const
    // Return a null-terminated string containing the exported name for this
    // type.
{
    return "MyChoice";
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
int TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    selectionId() const
{
    return d_value.object().typeIndex() - 1;
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
const typename TYPED_SELECTION_0::Type&
TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    theSelection0() const
{
    BSLS_ASSERT(d_value.object().typeIndex() == 1);
    return d_value.object().template the<Selection0Type>();
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
const typename TYPED_SELECTION_1::Type&
TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    theSelection1() const
{
    BSLS_ASSERT(d_value.object().typeIndex() == 2);
    return d_value.object().template the<Selection1Type>();
}

template <class TYPED_SELECTION_0,
          class TYPED_SELECTION_1,
          class TYPED_SELECTION_2>
const typename TYPED_SELECTION_2::Type&
TestChoice<TYPED_SELECTION_0, TYPED_SELECTION_1, TYPED_SELECTION_2>::
    theSelection2() const
{
    BSLS_ASSERT(d_value.object().typeIndex() == 3);
    return d_value.object().template the<Selection2Type>();
}

// FREE OPERATORS
template <class V0, class V1, class V2>
bsl::ostream& operator<<(bsl::ostream&                 stream,
                         const TestChoice<V0, V1, V2>& choice)
{
    typedef TestChoice<V0, V1, V2> Choice;

    stream << "[" << choice.className() << " ";
    switch (choice.selectionId()) {
      case 0: {
        stream << Choice::Selection0::name() << " = "
               << choice.theSelection0();
      } break;
      case 1: {
        stream << Choice::Selection1::name() << " = "
               << choice.theSelection1();
      } break;
      case 2: {
        stream << Choice::Selection2::name() << " = "
               << choice.theSelection2();
      } break;
    }

    return stream << "]";
}

template <class C0, class C1, class C2>
inline
bool operator==(const TestChoice<C0, C1, C2>& lhs,
                const TestChoice<C0, C1, C2>& rhs)
{
    return TestChoice<C0, C1, C2>::areEqual(lhs, rhs);
}

template <class C0, class C1, class C2>
inline
bool operator!=(const TestChoice<C0, C1, C2>& lhs,
                const TestChoice<C0, C1, C2>& rhs)
{
    return !TestChoice<C0, C1, C2>::areEqual(lhs, rhs);
}

// TRAITS
template <class C0, class C1, class C2>
const char *bdlat_TypeName_className(const TestChoice<C0, C1, C2>& object)
    // Return a null-terminated string containing the exported name of the type
    // for the specified 'object'.
{
    return object.className();
}

template <class C0, class C1, class C2>
int bdlat_choiceMakeSelection(TestChoice<C0, C1, C2> *object, int selectionId)
    // Set the value of the specified 'object' to the default for the selection
    // indicated by the specified 'selectionId'.  Return 0 on success, and a
    // non-zero value otherwise.
{
    return object->makeSelection(selectionId);
}

template <class C0, class C1, class C2>
int bdlat_choiceMakeSelection(TestChoice<C0, C1, C2> *object,
                              const char             *selectionName,
                              int                     selectionNameLength)
    // Set the value of the specified 'object' to be the default for the
    // selection indicated by the specified 'selectionName' of the specified
    // 'selectionNameLength'.  Return 0 on success, and a non-zero value
    // otherwise.
{
    return object->makeSelection(selectionName, selectionNameLength);
}

template <class C0, class C1, class C2, class MANIPULATOR>
int bdlat_choiceManipulateSelection(TestChoice<C0, C1, C2> *object,
                                    MANIPULATOR&            manipulator)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // selection of the specified 'object', supplying 'manipulator' with the
    // corresponding selection information structure.  Return -1 if the
    // selection is undefined, and the value returned from the invocation of
    // 'manipulator' otherwise.
{
    return object->manipulateSelection(manipulator);
}

template <class C0, class C1, class C2, class ACCESSOR>
int bdlat_choiceAccessSelection(const TestChoice<C0, C1, C2>& object,
                                ACCESSOR&                     accessor)
    // Invoke the specified 'accessor' on the (non-modifiable) selection of the
    // specified 'object', supplying 'accessor' with the corresponding
    // selection information structure.  Return -1 if the selection is
    // undefined, and the value returned from the invocation of 'accessor'
    // otherwise.
{
    return object.accessSelection(accessor);
}

template <class C0, class C1, class C2>
bool bdlat_choiceHasSelection(
                            const TestChoice<C0, C1, C2>&  object,
                            const char                    *selectionName,
                            int                            selectionNameLength)
{
    return object.hasSelection(selectionName, selectionNameLength);
}

template <class C0, class C1, class C2>
bool bdlat_choiceHasSelection(const TestChoice<C0, C1, C2>& object,
                              int                           selectionId)
{
    return object.hasSelection(selectionId);
}

template <class C0, class C1, class C2>
int bdlat_choiceSelectionId(const TestChoice<C0, C1, C2>& object)
    // Return the id of the current selection if the selection is defined, and
    // 'k_UNDEFINED_SELECTION_ID' otherwise.
{
    return object.selectionId();
}

template <class C0, class C1, class C2>
int bdlat_valueTypeAssign(TestChoice<C0, C1, C2>        *lhs,
                          const TestChoice<C0, C1, C2>&  rhs)
{
    *lhs = rhs;
    return 0;
}

template <class C0, class C1, class C2>
void bdlat_valueTypeReset(TestChoice<C0, C1, C2> *object)
{
    object->reset();
}



}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTCHOICE

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
