// bdlar_anyref.cpp                                                   -*-C++-*-
#include <bdlar_anyref.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlar_anyref_cpp, "$Id$ $CSID$")

#include <bdlar_arrayvtable.h>
#include <bdlar_choicevtable.h>
#include <bdlar_customizedtypevtable.h>
#include <bdlar_dynamictyperef.h>
#include <bdlar_enumvtable.h>
#include <bdlar_nullablevaluevtable.h>
#include <bdlar_sequencevtable.h>
#include <bdlar_simpletypevtable.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdlar {

                           // ------------
                           // class AnyRef
                           // ------------

// MANIPULATORS
void AnyRef::reset() const
{
    switch(typeCategory()) {
      case bdlat_TypeCategory::e_DYNAMIC_CATEGORY: {
        d_vtable.d_dynamic_p->d_reset_fp(d_object_p);
      } break;
      case bdlat_TypeCategory::e_ARRAY_CATEGORY: {
        d_vtable.d_array_p->d_reset_fp(d_object_p);
      } break;
      case bdlat_TypeCategory::e_CHOICE_CATEGORY: {
        d_vtable.d_choice_p->d_reset_fp(d_object_p);
      } break;
      case bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY: {
        d_vtable.d_customized_p->d_reset_fp(d_object_p);
      } break;
      case bdlat_TypeCategory::e_ENUMERATION_CATEGORY: {
        d_vtable.d_enum_p->d_reset_fp(d_object_p);
      } break;
      case bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY: {
        d_vtable.d_nullable_p->d_reset_fp(d_object_p);
      } break;
      case bdlat_TypeCategory::e_SEQUENCE_CATEGORY: {
        d_vtable.d_sequence_p->d_reset_fp(d_object_p);
      } break;
      case bdlat_TypeCategory::e_SIMPLE_CATEGORY: {
        d_vtable.d_simple_p->d_reset_fp(d_object_p);
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Unexpected type category");
      }
    }
}

                           // -----------------
                           // class AnyConstRef
                           // -----------------

// CREATORS
AnyConstRef::AnyConstRef(const AnyRef& object)
: d_object_p(object.objectAddress())
, d_category(object.typeCategory())
{
    switch(d_category) {
      case bdlat_TypeCategory::e_DYNAMIC_CATEGORY: {
        d_vtable.d_dynamic_p = &object.dynamicTypeVtable()->d_const;
      } break;
      case bdlat_TypeCategory::e_ARRAY_CATEGORY: {
        d_vtable.d_array_p = &object.arrayVtable()->d_const;
      } break;
      case bdlat_TypeCategory::e_CHOICE_CATEGORY: {
        d_vtable.d_choice_p = &object.choiceVtable()->d_const;
      } break;
      case bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY: {
        d_vtable.d_customized_p = &object.customizedTypeVtable()->d_const;
      } break;
      case bdlat_TypeCategory::e_ENUMERATION_CATEGORY: {
        d_vtable.d_enum_p = &object.enumVtable()->d_const;
      } break;
      case bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY: {
        d_vtable.d_nullable_p = &object.nullableValueVtable()->d_const;
      } break;
      case bdlat_TypeCategory::e_SEQUENCE_CATEGORY: {
        d_vtable.d_sequence_p = &object.sequenceVtable()->d_const;
      } break;
      case bdlat_TypeCategory::e_SIMPLE_CATEGORY: {
        d_vtable.d_simple_p = &object.simpleTypeVtable()->d_const;
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Unexpected type category");
      }
    }
}

// ACCESSORS
const char *AnyConstRef::xsdName(int format) const
{
    switch(d_category) {
      case bdlat_TypeCategory::e_DYNAMIC_CATEGORY:
        return d_vtable.d_dynamic_p->d_xsdName_fp(d_object_p, format);
      case bdlat_TypeCategory::e_ARRAY_CATEGORY:
        return d_vtable.d_array_p->d_xsdName_fp(d_object_p, format);
      case bdlat_TypeCategory::e_CHOICE_CATEGORY:
        return d_vtable.d_choice_p->d_xsdName_fp(d_object_p, format);
      case bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY:
        return d_vtable.d_customized_p->d_xsdName_fp(d_object_p, format);
      case bdlat_TypeCategory::e_ENUMERATION_CATEGORY:
        return d_vtable.d_enum_p->d_xsdName_fp(d_object_p, format);
      case bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY:
        return d_vtable.d_nullable_p->d_xsdName_fp(d_object_p, format);
      case bdlat_TypeCategory::e_SEQUENCE_CATEGORY:
        return d_vtable.d_sequence_p->d_xsdName_fp(d_object_p, format);
      case bdlat_TypeCategory::e_SIMPLE_CATEGORY:
        return d_vtable.d_simple_p->d_xsdName_fp(d_object_p, format);
      default:
        BSLS_ASSERT_INVOKE_NORETURN("Unexpected type category");
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
