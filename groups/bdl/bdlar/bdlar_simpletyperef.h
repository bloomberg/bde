// bdlar_simpletyperef.h                                              -*-C++-*-
#ifndef INCLUDED_BDLAR_SIMPLETYPEREF
#define INCLUDED_BDLAR_SIMPLETYPEREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide ...
//
//@CLASSES:
//  bdlar::SimpleTypeRef: ...
//  bdlar::SimpleTypeConstRef: ...
//
//@DESCRIPTION: This component provides ...
//

#include <bdlar_simpletypevtable.h>
#include <bdlar_simpletypevtableutil.h>
#include <bdlar_typecategory.h>

#include <bdlat_valuetypefunctions.h>

#include <bslmf_enableif.h>

namespace BloombergLP {
namespace bdlar {

                           // ===================
                           // class SimpleTypeRef
                           // ===================

class SimpleTypeRef {
    // DATA
    void                   *d_object_p;
    const SimpleTypeVtable *d_vtable_p;

    // PRIVATE TYPES
    template <class t_MANIPULATOR>
    struct ManipulatorWithCategory {
        // PUBLIC DATA
        t_MANIPULATOR& manipulator;

        // MANIPULATORS
        template <class t_TYPE>
        int operator()(t_TYPE *value)
        {
            // `Category` is
            // `bdlat_TypeCategory::Array` for `bsl::vector<char>`
            // `bdlat_TypeCategory::Simple` otherwise
            typedef typename bdlat_TypeCategory::Select<t_TYPE>::Type Category;
            return manipulator(value, Category());
        }
    };

    // PRIVATE ACCESSORS
    template <class t_TYPE, class t_MANIPULATOR>
    static int manipulateImpl(void *object, t_MANIPULATOR& manipulator)
    {
        return manipulator(static_cast<t_TYPE *>(object));
    }

    template <class t_TYPE, class t_MANIPULATOR, class t_INFO>
    static int manipulateImpl(void           *object,
                              t_MANIPULATOR&  manipulator,
                              const t_INFO&   info)
    {
        return manipulator(static_cast<t_TYPE *>(object), info);
    }

  public:
    // CREATORS

    /// Create a `SimpleTypeRef` object that type erases the supplied "simple"
    /// object.
    template <class t_TYPE>
    explicit SimpleTypeRef(t_TYPE& object,
                           typename bsl::enable_if<
                               IsSimple<t_TYPE>::value>::type * = 0);

    /// Create a `SimpleTypeRef` object.  The behaviour is undefined unless
    /// `objectAddress` and `vtable` point to the valid objects.
    SimpleTypeRef(void *objectAddress, const SimpleTypeVtable *vtable);

    // MANIPULATORS

    /// Invoke the specified `manipulator` on the value held by this object.
    /// The supplied `manipulator` must be an object callable with a value of
    /// any simple type as if it had the following signature:
    /// ```
    /// int manipulator(VALUE *value);
    /// ```
    /// Return the value from the invocation of `manipulator`.
    template <class t_MANIPULATOR>
    int manipulate(t_MANIPULATOR& manipulator) const;

    /// Invoke the specified `manipulator` on the value held by this object
    /// supplying `manipulator` with the specified `info` object.  The supplied
    /// `manipulator` must be an object callable with a value of any simple
    /// type as if it had the following signature:
    /// ```
    /// int manipulator(VALUE *value, const t_INFO& info);
    /// ```
    /// Return the value from the invocation of `manipulator`.
    template <class t_MANIPULATOR, class t_INFO>
    int manipulate(t_MANIPULATOR& manipulator, const t_INFO& info) const;

    /// Invoke the specified `manipulator` on the value held by this object
    /// supplying `manipulator` with an object of `bdlat_TypeCategory::Simple`
    /// type for any type except `bsl::vector<char>`.  The latter is supplied
    /// with an object of `bdlat_TypeCategory::Array` type.  The supplied
    /// `manipulator` must be an object callable with a value of any simple
    /// type as if it had the following signature:
    /// ```
    /// int manipulator(VALUE *value, bdlat_TypeCategory::Simple);
    /// // and
    /// int manipulator(bsl::vector<char> *value, bdlat_TypeCategory::Array);
    /// ```
    /// Return the value from the invocation of `manipulator`.
    template <class t_MANIPULATOR>
    int manipulateWithCategory(t_MANIPULATOR& manipulator) const;

    /// Reset the referred object to the default value.
    void reset() const;

    // ACCESSORS

    /// Invoke the specified `accessor` on the value held by this object.  The
    /// supplied `accessor` must be an object callable with a value of any
    /// simple type as if it had the following signature:
    /// ```
    /// int accessor(const VALUE& value);
    /// ```
    /// Return the value from the invocation of `accessor`.
    template <class t_ACCESSOR>
    int access(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the value held by this object
    /// supplying `accessor` with the specified `info` object.  The supplied
    /// `accessor` must be an object callable with a value of any simple type
    /// as if it had the following signature:
    /// ```
    /// int accessor(const VALUE& value, const t_INFO& info);
    /// ```
    /// Return the value from the invocation of `accessor`.
    template <class t_ACCESSOR, class t_INFO>
    int access(t_ACCESSOR& accessor, const t_INFO& info) const;

    /// Invoke the specified `accessor` on the value held by this object
    /// supplying `accessor` with an object of `bdlat_TypeCategory::Simple`
    /// type for any type except `bsl::vector<char>`.  The latter is supplied
    /// with an object of `bdlat_TypeCategory::Array` type.  The supplied
    /// `accessor` must be an object callable with a value of any simple type
    /// as if it had the following signature:
    /// ```
    /// int accessor(const VALUE& value, bdlat_TypeCategory::Simple);
    /// // and
    /// int accessor(const bsl::vector<char>& value,
    ///              bdlat_TypeCategory::Array);
    /// ```
    /// Return the value from the invocation of `accessor`.
    template <class t_ACCESSOR>
    int accessWithCategory(t_ACCESSOR& accessor) const;

    /// Return a pointer to the vtable.
    const SimpleTypeVtable *vtable() const;

    /// Return a pointer to the wrapped object.
    void *objectAddress() const;

    /// Return the type ID corresponding to the simple type held by this
    /// object.
    SimpleTypeId typeId() const;
};

                           // ========================
                           // class SimpleTypeConstRef
                           // ========================

class SimpleTypeConstRef {
    // DATA
    const void                  *d_object_p;
    const SimpleTypeConstVtable *d_vtable_p;

    // PRIVATE TYPES
    template <class t_ACCESSOR>
    struct AccessorWithCategory {
        // PUBLIC DATA
        t_ACCESSOR& accessor;

        // MANIPULATORS
        template <class t_TYPE>
        int operator()(const t_TYPE& value)
        {
            // `Category` is
            // `bdlat_TypeCategory::Array` for `bsl::vector<char>`
            // `bdlat_TypeCategory::Simple` otherwise
            typedef typename bdlat_TypeCategory::Select<t_TYPE>::Type Category;
            return accessor(value, Category());
        }
    };

    // PRIVATE ACCESSORS
    template <class t_TYPE, class t_ACCESSOR>
    static int accessImpl(const void *object, t_ACCESSOR& accessor)
    {
        return accessor(*static_cast<const t_TYPE *>(object));
    }

    template <class t_TYPE, class t_ACCESSOR, class t_INFO>
    static int accessImpl(const void    *object,
                          t_ACCESSOR&    accessor,
                          const t_INFO&  info)
    {
        return accessor(*static_cast<const t_TYPE *>(object), info);
    }

  public:
    // CREATORS

    /// Create a `SimpleTypeConstRef` object that type erases the supplied
    /// "simple" object.
    template <class t_TYPE>
    explicit SimpleTypeConstRef(const t_TYPE& object,
                                typename bsl::enable_if<
                                    IsSimple<t_TYPE>::value>::type * = 0);

    /// Create a `SimpleTypeConstRef` object.  The behaviour is undefined
    /// unless `objectAddress` and `vtable` point to the valid objects.
    SimpleTypeConstRef(const void                  *objectAddress,
                       const SimpleTypeConstVtable *vtable);

    /// Create a 'SimpleTypeConstRef' object that wraps the same object as the
    /// specified 'object'.
    SimpleTypeConstRef(const SimpleTypeRef& object); // IMPLICIT

    // ACCESSORS

    /// Invoke the specified `accessor` on the value held by this object.  The
    /// supplied `accessor` must be an object callable with a value of any
    /// simple type as if it had the following signature:
    /// ```
    /// int accessor(const VALUE& value);
    /// ```
    /// Return the value from the invocation of `accessor`.
    template <class t_ACCESSOR>
    int access(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the value held by this object
    /// supplying `accessor` with the specified `info` object.  The supplied
    /// `accessor` must be an object callable with a value of any simple type
    /// as if it had the following signature:
    /// ```
    /// int accessor(const VALUE& value, const t_INFO& info);
    /// ```
    /// Return the value from the invocation of `accessor`.
    template <class t_ACCESSOR, class t_INFO>
    int access(t_ACCESSOR& accessor, const t_INFO& info) const;

    /// Invoke the specified `accessor` on the value held by this object
    /// supplying `accessor` with an object of `bdlat_TypeCategory::Simple`
    /// type for any type except `bsl::vector<char>`.  The latter is supplied
    /// with an object of `bdlat_TypeCategory::Array` type.  The supplied
    /// `accessor` must be an object callable with a value of any simple type
    /// as if it had the following signature:
    /// ```
    /// int accessor(const VALUE& value, bdlat_TypeCategory::Simple);
    /// // and
    /// int accessor(const bsl::vector<char>& value,
    ///              bdlat_TypeCategory::Array);
    /// ```
    /// Return the value from the invocation of `accessor`.
    template <class t_ACCESSOR>
    int accessWithCategory(t_ACCESSOR& accessor) const;

    /// Return a pointer to the vtable.
    const SimpleTypeConstVtable *vtable() const;

    /// Return a pointer to the wrapped object.
    const void *objectAddress() const;

    /// Return the type ID corresponding to the simple type held by this
    /// object.
    SimpleTypeId typeId() const;

    /// Return the `bdlat_TypeName::xsdName` value for the underlying object.
    const char *xsdName(int format) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -------------------
                           // class SimpleTypeRef
                           // -------------------

// CREATORS
template <class t_TYPE>
inline
SimpleTypeRef::SimpleTypeRef(t_TYPE& object,
                             typename bsl::enable_if<
                                 IsSimple<t_TYPE>::value>::type *)
: d_object_p(&object)
, d_vtable_p(SimpleTypeVtableUtil::getVtable<t_TYPE>())
{
}

inline
SimpleTypeRef::SimpleTypeRef(void                   *objectAddress,
                             const SimpleTypeVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

// MANIPULATORS
template <class t_MANIPULATOR>
int SimpleTypeRef::manipulate(t_MANIPULATOR& manipulator) const
{
    typedef int (*Thunk)(void *, t_MANIPULATOR&);
    static const Thunk thunks[] = {
        manipulateImpl<int, t_MANIPULATOR>,
        manipulateImpl<unsigned int, t_MANIPULATOR>,
        manipulateImpl<char, t_MANIPULATOR>,
        manipulateImpl<signed char, t_MANIPULATOR>,
        manipulateImpl<unsigned char, t_MANIPULATOR>,
        manipulateImpl<short, t_MANIPULATOR>,
        manipulateImpl<unsigned short, t_MANIPULATOR>,
        manipulateImpl<long, t_MANIPULATOR>,
        manipulateImpl<unsigned long, t_MANIPULATOR>,
        manipulateImpl<long long, t_MANIPULATOR>,
        manipulateImpl<unsigned long long, t_MANIPULATOR>,
        manipulateImpl<bool, t_MANIPULATOR>,
        manipulateImpl<float, t_MANIPULATOR>,
        manipulateImpl<double, t_MANIPULATOR>,
        manipulateImpl<bdldfp::Decimal64, t_MANIPULATOR>,
        manipulateImpl<bsl::string, t_MANIPULATOR>,
        manipulateImpl<bsl::vector<char>, t_MANIPULATOR>,
        manipulateImpl<bdlt::Date, t_MANIPULATOR>,
        manipulateImpl<bdlt::DateTz, t_MANIPULATOR>,
        manipulateImpl<bdlt::Datetime, t_MANIPULATOR>,
        manipulateImpl<bdlt::DatetimeTz, t_MANIPULATOR>,
        manipulateImpl<bdlt::Time, t_MANIPULATOR>,
        manipulateImpl<bdlt::TimeTz, t_MANIPULATOR>
    };
    return (thunks[typeId()])(d_object_p, manipulator);
}

template <class t_MANIPULATOR, class t_INFO>
int SimpleTypeRef::manipulate(t_MANIPULATOR& manipulator,
                              const t_INFO&  info) const
{
    typedef int (*Thunk)(void *, t_MANIPULATOR&, const t_INFO&);
    static const Thunk thunks[] = {
        manipulateImpl<int, t_MANIPULATOR, t_INFO>,
        manipulateImpl<unsigned int, t_MANIPULATOR, t_INFO>,
        manipulateImpl<char, t_MANIPULATOR, t_INFO>,
        manipulateImpl<signed char, t_MANIPULATOR, t_INFO>,
        manipulateImpl<unsigned char, t_MANIPULATOR, t_INFO>,
        manipulateImpl<short, t_MANIPULATOR, t_INFO>,
        manipulateImpl<unsigned short, t_MANIPULATOR, t_INFO>,
        manipulateImpl<long, t_MANIPULATOR, t_INFO>,
        manipulateImpl<unsigned long, t_MANIPULATOR, t_INFO>,
        manipulateImpl<long long, t_MANIPULATOR, t_INFO>,
        manipulateImpl<unsigned long long, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bool, t_MANIPULATOR, t_INFO>,
        manipulateImpl<float, t_MANIPULATOR, t_INFO>,
        manipulateImpl<double, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bdldfp::Decimal64, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bsl::string, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bsl::vector<char>, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bdlt::Date, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bdlt::DateTz, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bdlt::Datetime, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bdlt::DatetimeTz, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bdlt::Time, t_MANIPULATOR, t_INFO>,
        manipulateImpl<bdlt::TimeTz, t_MANIPULATOR, t_INFO>
    };
    return (thunks[typeId()])(d_object_p, manipulator, info);
}

template <class t_MANIPULATOR>
inline
int SimpleTypeRef::manipulateWithCategory(t_MANIPULATOR& manipulator) const
{
    ManipulatorWithCategory<t_MANIPULATOR> thunk = { manipulator };
    return this->manipulate(thunk);
}

inline
void SimpleTypeRef::reset() const
{
    return d_vtable_p->d_reset_fp(d_object_p);
}

// ACCESSORS
template <class t_ACCESSOR>
inline
int SimpleTypeRef::access(t_ACCESSOR& accessor) const
{
    return SimpleTypeConstRef(*this).access(accessor);
}

template <class t_ACCESSOR, class t_INFO>
inline
int SimpleTypeRef::access(t_ACCESSOR& accessor, const t_INFO& info) const
{
    return SimpleTypeConstRef(*this).access(accessor, info);
}

template <class t_ACCESSOR>
inline
int SimpleTypeRef::accessWithCategory(t_ACCESSOR& accessor) const
{
    return SimpleTypeConstRef(*this).accessWithCategory(accessor);
}

inline
const SimpleTypeVtable *SimpleTypeRef::vtable() const
{
    return d_vtable_p;
}

inline
void *SimpleTypeRef::objectAddress() const
{
    return d_object_p;
}

inline
SimpleTypeId SimpleTypeRef::typeId() const
{
    return d_vtable_p->d_const.d_typeId_fp();
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_ValueTypeFunctions`)

inline
void bdlat_valueTypeReset(SimpleTypeRef *ref)
{
    ref->reset();
}

                           // ------------------------
                           // class SimpleTypeConstRef
                           // ------------------------

// CREATORS
template <class t_TYPE>
inline
SimpleTypeConstRef::SimpleTypeConstRef(const t_TYPE& object,
                                       typename bsl::enable_if<
                                           IsSimple<t_TYPE>::value>::type *)
: d_object_p(&object)
, d_vtable_p(SimpleTypeVtableUtil::getConstVtable<t_TYPE>())
{
}

inline
SimpleTypeConstRef::SimpleTypeConstRef(
                                    const void                  *objectAddress,
                                    const SimpleTypeConstVtable *vtable)
: d_object_p(objectAddress)
, d_vtable_p(vtable)
{
}

inline
SimpleTypeConstRef::SimpleTypeConstRef(const SimpleTypeRef& object)
: d_object_p(object.objectAddress())
, d_vtable_p(&object.vtable()->d_const)
{
}

// ACCESSORS
template <class t_ACCESSOR>
int SimpleTypeConstRef::access(t_ACCESSOR& accessor) const
{
    typedef int (*Thunk)(const void *, t_ACCESSOR&);
    static const Thunk thunks[] = {
        accessImpl<int, t_ACCESSOR>,
        accessImpl<unsigned int, t_ACCESSOR>,
        accessImpl<char, t_ACCESSOR>,
        accessImpl<signed char, t_ACCESSOR>,
        accessImpl<unsigned char, t_ACCESSOR>,
        accessImpl<short, t_ACCESSOR>,
        accessImpl<unsigned short, t_ACCESSOR>,
        accessImpl<long, t_ACCESSOR>,
        accessImpl<unsigned long, t_ACCESSOR>,
        accessImpl<long long, t_ACCESSOR>,
        accessImpl<unsigned long long, t_ACCESSOR>,
        accessImpl<bool, t_ACCESSOR>,
        accessImpl<float, t_ACCESSOR>,
        accessImpl<double, t_ACCESSOR>,
        accessImpl<bdldfp::Decimal64, t_ACCESSOR>,
        accessImpl<bsl::string, t_ACCESSOR>,
        accessImpl<bsl::vector<char>, t_ACCESSOR>,
        accessImpl<bdlt::Date, t_ACCESSOR>,
        accessImpl<bdlt::DateTz, t_ACCESSOR>,
        accessImpl<bdlt::Datetime, t_ACCESSOR>,
        accessImpl<bdlt::DatetimeTz, t_ACCESSOR>,
        accessImpl<bdlt::Time, t_ACCESSOR>,
        accessImpl<bdlt::TimeTz, t_ACCESSOR>
    };
    return (thunks[typeId()])(d_object_p, accessor);
}

template <class t_ACCESSOR, class t_INFO>
int SimpleTypeConstRef::access(t_ACCESSOR& accessor, const t_INFO& info) const
{
    typedef int (*Thunk)(const void *, t_ACCESSOR&, const t_INFO&);
    static const Thunk thunks[] = {
        accessImpl<int, t_ACCESSOR, t_INFO>,
        accessImpl<unsigned int, t_ACCESSOR, t_INFO>,
        accessImpl<char, t_ACCESSOR, t_INFO>,
        accessImpl<signed char, t_ACCESSOR, t_INFO>,
        accessImpl<unsigned char, t_ACCESSOR, t_INFO>,
        accessImpl<short, t_ACCESSOR, t_INFO>,
        accessImpl<unsigned short, t_ACCESSOR, t_INFO>,
        accessImpl<long, t_ACCESSOR, t_INFO>,
        accessImpl<unsigned long, t_ACCESSOR, t_INFO>,
        accessImpl<long long, t_ACCESSOR, t_INFO>,
        accessImpl<unsigned long long, t_ACCESSOR, t_INFO>,
        accessImpl<bool, t_ACCESSOR, t_INFO>,
        accessImpl<float, t_ACCESSOR, t_INFO>,
        accessImpl<double, t_ACCESSOR, t_INFO>,
        accessImpl<bdldfp::Decimal64, t_ACCESSOR, t_INFO>,
        accessImpl<bsl::string, t_ACCESSOR, t_INFO>,
        accessImpl<bsl::vector<char>, t_ACCESSOR, t_INFO>,
        accessImpl<bdlt::Date, t_ACCESSOR, t_INFO>,
        accessImpl<bdlt::DateTz, t_ACCESSOR, t_INFO>,
        accessImpl<bdlt::Datetime, t_ACCESSOR, t_INFO>,
        accessImpl<bdlt::DatetimeTz, t_ACCESSOR, t_INFO>,
        accessImpl<bdlt::Time, t_ACCESSOR, t_INFO>,
        accessImpl<bdlt::TimeTz, t_ACCESSOR, t_INFO>
    };
    return (thunks[typeId()])(d_object_p, accessor, info);
}

template <class t_ACCESSOR>
inline
int SimpleTypeConstRef::accessWithCategory(t_ACCESSOR& accessor) const
{
    AccessorWithCategory<t_ACCESSOR> thunk = { accessor };
    return this->access(thunk);
}

inline
const SimpleTypeConstVtable *SimpleTypeConstRef::vtable() const
{
    return d_vtable_p;
}

inline
const void *SimpleTypeConstRef::objectAddress() const
{
    return d_object_p;
}

inline
SimpleTypeId SimpleTypeConstRef::typeId() const
{
    return d_vtable_p->d_typeId_fp();
}

inline
const char *SimpleTypeConstRef::xsdName(int format) const
{
    return d_vtable_p->d_xsdName_fp(d_object_p, format);
}

// FREE FUNCTIONS
// Customization-point functions (`bdlat_TypeName`)
inline
const char *bdlat_TypeName_xsdName(const SimpleTypeConstRef& ref, int format)
{
    return ref.xsdName(format);
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
