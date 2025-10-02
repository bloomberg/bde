// bdlar_fieldinfo.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLAR_FIELDINFO
#define INCLUDED_BDLAR_FIELDINFO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::FieldInfoRef: ...
//
//@DESCRIPTION: This component provides ...
//
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#include <bslmf_movableref.h>

#include <bsl_concepts.h>

namespace BloombergLP {
namespace bdlar {

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS

                           // =================
                           // concept FieldInfo
                           // =================

template <class t_TYPE>
concept FieldInfo = requires (t_TYPE object)
{
    { object.formattingMode() } -> bsl::convertible_to<int>;
    { object.id() } -> bsl::convertible_to<int>;
    { object.name() } -> bsl::convertible_to<const char *>;
    { object.nameLength() } -> bsl::convertible_to<int>;
};

#endif

                         // ======================
                         // struct FieldInfoVtable
                         // ======================

struct FieldInfoVtable {
    // TYPES
    typedef const char *Annotation(const void *self);
    typedef int FormattingMode(const void *self);
    typedef int Id(const void *self);
    typedef const char *Name(const void *self);
    typedef int NameLength(const void *self);

    // PUBLIC DATA
    Annotation *d_annotation_p;
    FormattingMode *d_formattingMode_p;
    Id *d_id_p;
    Name *d_name_p;
    NameLength *d_nameLength_p;
};

                           // ==================
                           // class FieldInfoRef
                           // ==================

class FieldInfoRef {

    // DATA
    const void *d_target_p;
    const FieldInfoVtable *d_vtable_p;

  public:
    // CREATORS

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS
    template <FieldInfo t_FIELD_INFO>
    FieldInfoRef(const t_FIELD_INFO& target) BSLS_KEYWORD_NOEXCEPT;
#else
    template <class t_FIELD_INFO>
    explicit FieldInfoRef(const t_FIELD_INFO& target);
#endif

    FieldInfoRef(const void *target, const FieldInfoVtable *vtable);

    // ACCESSORS
    const char *annotation() const;

    int formattingMode() const;

    int id() const;

    const char *name() const;

    int nameLength() const;

    const void *target() const;

    const FieldInfoVtable *vtable() const;
};

                         // ===========================
                         // struct FieldInfo_VtableUtil
                         // ===========================

struct FieldInfo_VtableUtil {
  private:
    // PRIVATE CLASS METHODS

    template <class t_FIELD_INFO>
    static const char *annotation(const void *target);

    template <class t_FIELD_INFO>
    static int formattingMode(const void *target);

    template <class t_FIELD_INFO>
    static int id(const void *target);

    template <class t_FIELD_INFO>
    static const char *name(const void *target);

    template <class t_FIELD_INFO>
    static int nameLength(const void *target);

  public:
    // CLASS METHODS
    template <class t_FIELD_INFO>
    static const FieldInfoVtable *getVtable();
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ------------------
                           // class FieldInfoRef
                           // ------------------

// CREATORS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS
template <FieldInfo t_FIELD_INFO>
inline
FieldInfoRef::FieldInfoRef(const t_FIELD_INFO& target) BSLS_KEYWORD_NOEXCEPT
: d_target_p(&target)
, d_vtable_p(FieldInfo_VtableUtil::getVtable<t_FIELD_INFO>())
{
}
#else
template <class t_FIELD_INFO>
inline
FieldInfoRef::FieldInfoRef(const t_FIELD_INFO& target)
: d_target_p(&target)
, d_vtable_p(FieldInfo_VtableUtil::getVtable<t_FIELD_INFO>())
{
}
#endif

inline
FieldInfoRef::FieldInfoRef(const void *target, const FieldInfoVtable *vtable)
: d_target_p(target)
, d_vtable_p(vtable)
{
}

// ACCESSORS
inline
const char *FieldInfoRef::annotation() const
{
    return (d_vtable_p->d_annotation_p)(d_target_p);
}

inline
int FieldInfoRef::formattingMode() const
{
    return (d_vtable_p->d_formattingMode_p)(d_target_p);
}

inline
int FieldInfoRef::id() const
{
    return (d_vtable_p->d_id_p)(d_target_p);
}

inline
const char *FieldInfoRef::name() const
{
    return (d_vtable_p->d_name_p)(d_target_p);
}

inline
int FieldInfoRef::nameLength() const
{
    return (d_vtable_p->d_nameLength_p)(d_target_p);
}

inline
const void *FieldInfoRef::target() const
{
    return d_target_p;
}

inline
const FieldInfoVtable *FieldInfoRef::vtable() const
{
    return d_vtable_p;
}

// PRIVATE CLASS METHODS
template <class t_FIELD_INFO>
const char *FieldInfo_VtableUtil::annotation(const void *target)
{
    const t_FIELD_INFO& typedTarget =
        *static_cast<const t_FIELD_INFO *>(target);

    return typedTarget.annotation();
}

template <class t_FIELD_INFO>
int FieldInfo_VtableUtil::formattingMode(const void *target)
{
    const t_FIELD_INFO& typedTarget =
        *static_cast<const t_FIELD_INFO *>(target);

    return typedTarget.formattingMode();
}

template <class t_FIELD_INFO>
int FieldInfo_VtableUtil::id(const void *target)
{
    const t_FIELD_INFO& typedTarget =
        *static_cast<const t_FIELD_INFO *>(target);

    return typedTarget.id();
}

template <class t_FIELD_INFO>
const char *FieldInfo_VtableUtil::name(const void *target)
{
    const t_FIELD_INFO& typedTarget =
        *static_cast<const t_FIELD_INFO *>(target);

    return typedTarget.name();
}

template <class t_FIELD_INFO>
int FieldInfo_VtableUtil::nameLength(const void *target)
{
    const t_FIELD_INFO& typedTarget =
        *static_cast<const t_FIELD_INFO *>(target);

    return typedTarget.nameLength();
}

// CLASS METHODS
template <class t_FIELD_INFO>
const FieldInfoVtable *FieldInfo_VtableUtil::getVtable()
{
    static const FieldInfoVtable result = {
        &annotation<t_FIELD_INFO>,
        &formattingMode<t_FIELD_INFO>,
        &id<t_FIELD_INFO>,
        &name<t_FIELD_INFO>,
        &nameLength<t_FIELD_INFO>
    };

    return &result;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BDLAR_FIELDINFO

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
