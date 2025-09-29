// s_baltst_mysequencewithnillable.h     *DO NOT EDIT*     @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_MYSEQUENCEWITHNILLABLE
#define INCLUDED_S_BALTST_MYSEQUENCEWITHNILLABLE

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_mysequencewithnillable_h, "$Id$ $CSID$")
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

#include <bsl_vector.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class MySequenceWithNillable; }
namespace s_baltst { class MySequenceWithNillableBase64Binary; }
namespace s_baltst { class MySequenceWithNillableHexBinary; }
namespace s_baltst { class MySequenceWithNillableIntArray; }
namespace s_baltst { class MySequenceWithNillableIntSequenceArraySequence; }
namespace s_baltst { class MySequenceWithNillableStringArray; }
namespace s_baltst { class MySequenceWithNillableStringSequenceArraySequence; }
namespace s_baltst { class MySequenceWithNillableIntList; }
namespace s_baltst { class MySequenceWithNillableIntSequenceArray; }
namespace s_baltst { class MySequenceWithNillableStringList; }
namespace s_baltst { class MySequenceWithNillableStringSequenceArray; }
namespace s_baltst {

                        // ============================
                        // class MySequenceWithNillable
                        // ============================

class MySequenceWithNillable {

    // INSTANCE DATA
    bsl::string                       d_attribute2;
    bdlb::NullableValue<bsl::string>  d_myNillable;
    int                               d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1  = 0
      , ATTRIBUTE_ID_MY_NILLABLE = 1
      , ATTRIBUTE_ID_ATTRIBUTE2  = 2
    };

    enum {
        NUM_ATTRIBUTES = 3
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1  = 0
      , ATTRIBUTE_INDEX_MY_NILLABLE = 1
      , ATTRIBUTE_INDEX_ATTRIBUTE2  = 2
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNillable` having the default
    /// value.  Use the optionally specified `basicAllocator` to supply
    /// memory.  If `basicAllocator` is 0, the currently installed default
    /// allocator is used.
    explicit MySequenceWithNillable(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithNillable` having the value
    /// of the specified `original` object.  Use the optionally specified
    /// `basicAllocator` to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    MySequenceWithNillable(const MySequenceWithNillable& original,
                           bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNillable` having the value
    /// of the specified `original` object.  After performing this action,
    /// the `original` object will be left in a valid, but unspecified
    /// state.
    MySequenceWithNillable(MySequenceWithNillable&& original) noexcept;

    /// Create an object of type `MySequenceWithNillable` having the value
    /// of the specified `original` object.  After performing this action,
    /// the `original` object will be left in a valid, but unspecified
    /// state.  Use the optionally specified `basicAllocator` to supply
    /// memory.  If `basicAllocator` is 0, the currently installed default
    /// allocator is used.
    MySequenceWithNillable(MySequenceWithNillable&& original,
                           bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillable();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillable& operator=(const MySequenceWithNillable& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillable& operator=(MySequenceWithNillable&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    int& attribute1();

    /// Return a reference to the modifiable "MyNillable" attribute of this
    /// object.
    bdlb::NullableValue<bsl::string>& myNillable();

    /// Return a reference to the modifiable "Attribute2" attribute of this
    /// object.
    bsl::string& attribute2();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return the value of the "Attribute1" attribute of this object.
    int attribute1() const;

    /// Return a reference offering non-modifiable access to the
    /// "MyNillable" attribute of this object.
    const bdlb::NullableValue<bsl::string>& myNillable() const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute2" attribute of this object.
    const bsl::string& attribute2() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const MySequenceWithNillable& lhs,
                           const MySequenceWithNillable& rhs)
    {
        return lhs.attribute1() == rhs.attribute1() &&
               lhs.myNillable() == rhs.myNillable() &&
               lhs.attribute2() == rhs.attribute2();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const MySequenceWithNillable& lhs,
                           const MySequenceWithNillable& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream&                 stream,
                                    const MySequenceWithNillable& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillable)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillable> : bsl::true_type {};

namespace s_baltst {

                  // ========================================
                  // class MySequenceWithNillableBase64Binary
                  // ========================================

class MySequenceWithNillableBase64Binary {

    // INSTANCE DATA
    bdlb::NullableValue<bsl::vector<char> >  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNillableBase64Binary` having
    /// the default value.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit MySequenceWithNillableBase64Binary(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithNillableBase64Binary` having
    /// the value of the specified `original` object.  Use the optionally
    /// specified `basicAllocator` to supply memory.  If `basicAllocator` is
    /// 0, the currently installed default allocator is used.
    MySequenceWithNillableBase64Binary(const MySequenceWithNillableBase64Binary& original,
                                       bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNillableBase64Binary` having
    /// the value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.
    MySequenceWithNillableBase64Binary(MySequenceWithNillableBase64Binary&& original) noexcept;

    /// Create an object of type `MySequenceWithNillableBase64Binary` having
    /// the value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    MySequenceWithNillableBase64Binary(MySequenceWithNillableBase64Binary&& original,
                                       bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillableBase64Binary();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillableBase64Binary& operator=(const MySequenceWithNillableBase64Binary& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableBase64Binary& operator=(MySequenceWithNillableBase64Binary&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bdlb::NullableValue<bsl::vector<char> >& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute1" attribute of this object.
    const bdlb::NullableValue<bsl::vector<char> >& attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const MySequenceWithNillableBase64Binary& lhs,
                           const MySequenceWithNillableBase64Binary& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const MySequenceWithNillableBase64Binary& lhs,
                           const MySequenceWithNillableBase64Binary& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
                              bsl::ostream&                             stream,
                              const MySequenceWithNillableBase64Binary& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableBase64Binary)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableBase64Binary> : bsl::true_type {};

namespace s_baltst {

                   // =====================================
                   // class MySequenceWithNillableHexBinary
                   // =====================================

class MySequenceWithNillableHexBinary {

    // INSTANCE DATA
    bdlb::NullableValue<bsl::vector<char> >  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNillableHexBinary` having
    /// the default value.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit MySequenceWithNillableHexBinary(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithNillableHexBinary` having
    /// the value of the specified `original` object.  Use the optionally
    /// specified `basicAllocator` to supply memory.  If `basicAllocator` is
    /// 0, the currently installed default allocator is used.
    MySequenceWithNillableHexBinary(const MySequenceWithNillableHexBinary& original,
                                    bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNillableHexBinary` having
    /// the value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.
    MySequenceWithNillableHexBinary(MySequenceWithNillableHexBinary&& original) noexcept;

    /// Create an object of type `MySequenceWithNillableHexBinary` having
    /// the value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    MySequenceWithNillableHexBinary(MySequenceWithNillableHexBinary&& original,
                                    bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillableHexBinary();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillableHexBinary& operator=(const MySequenceWithNillableHexBinary& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableHexBinary& operator=(MySequenceWithNillableHexBinary&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bdlb::NullableValue<bsl::vector<char> >& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute1" attribute of this object.
    const bdlb::NullableValue<bsl::vector<char> >& attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const MySequenceWithNillableHexBinary& lhs,
                           const MySequenceWithNillableHexBinary& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const MySequenceWithNillableHexBinary& lhs,
                           const MySequenceWithNillableHexBinary& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
                                 bsl::ostream&                          stream,
                                 const MySequenceWithNillableHexBinary& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableHexBinary)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableHexBinary> : bsl::true_type {};

namespace s_baltst {

                    // ====================================
                    // class MySequenceWithNillableIntArray
                    // ====================================

class MySequenceWithNillableIntArray {

    // INSTANCE DATA
    bsl::vector<bdlb::NullableValue<int> >  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNillableIntArray` having the
    /// default value.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit MySequenceWithNillableIntArray(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithNillableIntArray` having the
    /// value of the specified `original` object.  Use the optionally
    /// specified `basicAllocator` to supply memory.  If `basicAllocator` is
    /// 0, the currently installed default allocator is used.
    MySequenceWithNillableIntArray(const MySequenceWithNillableIntArray& original,
                                   bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNillableIntArray` having the
    /// value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.
    MySequenceWithNillableIntArray(MySequenceWithNillableIntArray&& original) noexcept;

    /// Create an object of type `MySequenceWithNillableIntArray` having the
    /// value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    MySequenceWithNillableIntArray(MySequenceWithNillableIntArray&& original,
                                   bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillableIntArray();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillableIntArray& operator=(const MySequenceWithNillableIntArray& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableIntArray& operator=(MySequenceWithNillableIntArray&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bsl::vector<bdlb::NullableValue<int> >& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute1" attribute of this object.
    const bsl::vector<bdlb::NullableValue<int> >& attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const MySequenceWithNillableIntArray& lhs,
                           const MySequenceWithNillableIntArray& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const MySequenceWithNillableIntArray& lhs,
                           const MySequenceWithNillableIntArray& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
                                  bsl::ostream&                         stream,
                                  const MySequenceWithNillableIntArray& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableIntArray)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableIntArray> : bsl::true_type {};

namespace s_baltst {

            // ====================================================
            // class MySequenceWithNillableIntSequenceArraySequence
            // ====================================================

class MySequenceWithNillableIntSequenceArraySequence {

    // INSTANCE DATA
    int  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type
    /// `MySequenceWithNillableIntSequenceArraySequence` having the default
    /// value.
    MySequenceWithNillableIntSequenceArraySequence();


    // MANIPULATORS

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    int& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return the value of the "Attribute1" attribute of this object.
    int attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(
                     const MySequenceWithNillableIntSequenceArraySequence& lhs,
                     const MySequenceWithNillableIntSequenceArraySequence& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(
                     const MySequenceWithNillableIntSequenceArraySequence& lhs,
                     const MySequenceWithNillableIntSequenceArraySequence& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
                  bsl::ostream&                                         stream,
                  const MySequenceWithNillableIntSequenceArraySequence& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableIntSequenceArraySequence)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableIntSequenceArraySequence> : bsl::true_type {};

namespace s_baltst {

                  // =======================================
                  // class MySequenceWithNillableStringArray
                  // =======================================

class MySequenceWithNillableStringArray {

    // INSTANCE DATA
    bsl::vector<bdlb::NullableValue<bsl::string> >  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNillableStringArray` having
    /// the default value.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit MySequenceWithNillableStringArray(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithNillableStringArray` having
    /// the value of the specified `original` object.  Use the optionally
    /// specified `basicAllocator` to supply memory.  If `basicAllocator` is
    /// 0, the currently installed default allocator is used.
    MySequenceWithNillableStringArray(const MySequenceWithNillableStringArray& original,
                                      bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNillableStringArray` having
    /// the value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.
    MySequenceWithNillableStringArray(MySequenceWithNillableStringArray&& original) noexcept;

    /// Create an object of type `MySequenceWithNillableStringArray` having
    /// the value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    MySequenceWithNillableStringArray(MySequenceWithNillableStringArray&& original,
                                      bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillableStringArray();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillableStringArray& operator=(const MySequenceWithNillableStringArray& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableStringArray& operator=(MySequenceWithNillableStringArray&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bsl::vector<bdlb::NullableValue<bsl::string> >& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute1" attribute of this object.
    const bsl::vector<bdlb::NullableValue<bsl::string> >& attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const MySequenceWithNillableStringArray& lhs,
                           const MySequenceWithNillableStringArray& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const MySequenceWithNillableStringArray& lhs,
                           const MySequenceWithNillableStringArray& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
                               bsl::ostream&                            stream,
                               const MySequenceWithNillableStringArray& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableStringArray)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableStringArray> : bsl::true_type {};

namespace s_baltst {

          // =======================================================
          // class MySequenceWithNillableStringSequenceArraySequence
          // =======================================================

class MySequenceWithNillableStringSequenceArraySequence {

    // INSTANCE DATA
    bsl::string  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type
    /// `MySequenceWithNillableStringSequenceArraySequence` having the
    /// default value.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit MySequenceWithNillableStringSequenceArraySequence(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type
    /// `MySequenceWithNillableStringSequenceArraySequence` having the value
    /// of the specified `original` object.  Use the optionally specified
    /// `basicAllocator` to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    MySequenceWithNillableStringSequenceArraySequence(const MySequenceWithNillableStringSequenceArraySequence& original,
                                                      bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type
    /// `MySequenceWithNillableStringSequenceArraySequence` having the value
    /// of the specified `original` object.  After performing this action,
    /// the `original` object will be left in a valid, but unspecified
    /// state.
    MySequenceWithNillableStringSequenceArraySequence(MySequenceWithNillableStringSequenceArraySequence&& original) noexcept;

    /// Create an object of type
    /// `MySequenceWithNillableStringSequenceArraySequence` having the value
    /// of the specified `original` object.  After performing this action,
    /// the `original` object will be left in a valid, but unspecified
    /// state.  Use the optionally specified `basicAllocator` to supply
    /// memory.  If `basicAllocator` is 0, the currently installed default
    /// allocator is used.
    MySequenceWithNillableStringSequenceArraySequence(MySequenceWithNillableStringSequenceArraySequence&& original,
                                                      bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillableStringSequenceArraySequence();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillableStringSequenceArraySequence& operator=(const MySequenceWithNillableStringSequenceArraySequence& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableStringSequenceArraySequence& operator=(MySequenceWithNillableStringSequenceArraySequence&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bsl::string& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute1" attribute of this object.
    const bsl::string& attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(
                  const MySequenceWithNillableStringSequenceArraySequence& lhs,
                  const MySequenceWithNillableStringSequenceArraySequence& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(
                  const MySequenceWithNillableStringSequenceArraySequence& lhs,
                  const MySequenceWithNillableStringSequenceArraySequence& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
               bsl::ostream&                                            stream,
               const MySequenceWithNillableStringSequenceArraySequence& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableStringSequenceArraySequence)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableStringSequenceArraySequence> : bsl::true_type {};

namespace s_baltst {

                    // ===================================
                    // class MySequenceWithNillableIntList
                    // ===================================

class MySequenceWithNillableIntList {

    // INSTANCE DATA
    bsl::vector<bdlb::NullableValue<int> >  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNillableIntList` having the
    /// default value.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit MySequenceWithNillableIntList(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithNillableIntList` having the
    /// value of the specified `original` object.  Use the optionally
    /// specified `basicAllocator` to supply memory.  If `basicAllocator` is
    /// 0, the currently installed default allocator is used.
    MySequenceWithNillableIntList(const MySequenceWithNillableIntList& original,
                                  bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNillableIntList` having the
    /// value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.
    MySequenceWithNillableIntList(MySequenceWithNillableIntList&& original) noexcept;

    /// Create an object of type `MySequenceWithNillableIntList` having the
    /// value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    MySequenceWithNillableIntList(MySequenceWithNillableIntList&& original,
                                  bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillableIntList();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillableIntList& operator=(const MySequenceWithNillableIntList& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableIntList& operator=(MySequenceWithNillableIntList&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bsl::vector<bdlb::NullableValue<int> >& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute1" attribute of this object.
    const bsl::vector<bdlb::NullableValue<int> >& attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const MySequenceWithNillableIntList& lhs,
                           const MySequenceWithNillableIntList& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const MySequenceWithNillableIntList& lhs,
                           const MySequenceWithNillableIntList& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
                                   bsl::ostream&                        stream,
                                   const MySequenceWithNillableIntList& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableIntList)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableIntList> : bsl::true_type {};

namespace s_baltst {

                // ============================================
                // class MySequenceWithNillableIntSequenceArray
                // ============================================

class MySequenceWithNillableIntSequenceArray {

    // INSTANCE DATA
    bsl::vector<bdlb::NullableValue<MySequenceWithNillableIntSequenceArraySequence> >  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNillableIntSequenceArray`
    /// having the default value.  Use the optionally specified
    /// `basicAllocator` to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    explicit MySequenceWithNillableIntSequenceArray(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithNillableIntSequenceArray`
    /// having the value of the specified `original` object.  Use the
    /// optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    MySequenceWithNillableIntSequenceArray(const MySequenceWithNillableIntSequenceArray& original,
                                           bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNillableIntSequenceArray`
    /// having the value of the specified `original` object.  After
    /// performing this action, the `original` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableIntSequenceArray(MySequenceWithNillableIntSequenceArray&& original) noexcept;

    /// Create an object of type `MySequenceWithNillableIntSequenceArray`
    /// having the value of the specified `original` object.  After
    /// performing this action, the `original` object will be left in a
    /// valid, but unspecified state.  Use the optionally specified
    /// `basicAllocator` to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    MySequenceWithNillableIntSequenceArray(MySequenceWithNillableIntSequenceArray&& original,
                                           bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillableIntSequenceArray();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillableIntSequenceArray& operator=(const MySequenceWithNillableIntSequenceArray& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableIntSequenceArray& operator=(MySequenceWithNillableIntSequenceArray&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bsl::vector<bdlb::NullableValue<MySequenceWithNillableIntSequenceArraySequence> >& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute1" attribute of this object.
    const bsl::vector<bdlb::NullableValue<MySequenceWithNillableIntSequenceArraySequence> >& attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const MySequenceWithNillableIntSequenceArray& lhs,
                           const MySequenceWithNillableIntSequenceArray& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const MySequenceWithNillableIntSequenceArray& lhs,
                           const MySequenceWithNillableIntSequenceArray& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
                          bsl::ostream&                                 stream,
                          const MySequenceWithNillableIntSequenceArray& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableIntSequenceArray)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableIntSequenceArray> : bsl::true_type {};

namespace s_baltst {

                   // ======================================
                   // class MySequenceWithNillableStringList
                   // ======================================

class MySequenceWithNillableStringList {

    // INSTANCE DATA
    bsl::vector<bdlb::NullableValue<bsl::string> >  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNillableStringList` having
    /// the default value.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    explicit MySequenceWithNillableStringList(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithNillableStringList` having
    /// the value of the specified `original` object.  Use the optionally
    /// specified `basicAllocator` to supply memory.  If `basicAllocator` is
    /// 0, the currently installed default allocator is used.
    MySequenceWithNillableStringList(const MySequenceWithNillableStringList& original,
                                     bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNillableStringList` having
    /// the value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.
    MySequenceWithNillableStringList(MySequenceWithNillableStringList&& original) noexcept;

    /// Create an object of type `MySequenceWithNillableStringList` having
    /// the value of the specified `original` object.  After performing this
    /// action, the `original` object will be left in a valid, but
    /// unspecified state.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    MySequenceWithNillableStringList(MySequenceWithNillableStringList&& original,
                                     bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillableStringList();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillableStringList& operator=(const MySequenceWithNillableStringList& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableStringList& operator=(MySequenceWithNillableStringList&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bsl::vector<bdlb::NullableValue<bsl::string> >& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute1" attribute of this object.
    const bsl::vector<bdlb::NullableValue<bsl::string> >& attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(const MySequenceWithNillableStringList& lhs,
                           const MySequenceWithNillableStringList& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(const MySequenceWithNillableStringList& lhs,
                           const MySequenceWithNillableStringList& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
                                bsl::ostream&                           stream,
                                const MySequenceWithNillableStringList& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableStringList)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableStringList> : bsl::true_type {};

namespace s_baltst {

              // ===============================================
              // class MySequenceWithNillableStringSequenceArray
              // ===============================================

class MySequenceWithNillableStringSequenceArray {

    // INSTANCE DATA
    bsl::vector<bdlb::NullableValue<MySequenceWithNillableStringSequenceArraySequence> >  d_attribute1;

  public:
    // TYPES
    enum {
        ATTRIBUTE_ID_ATTRIBUTE1 = 0
    };

    enum {
        NUM_ATTRIBUTES = 1
    };

    enum {
        ATTRIBUTE_INDEX_ATTRIBUTE1 = 0
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];

  public:
    // CLASS METHODS

    /// Return attribute information for the attribute indicated by the
    /// specified `id` if the attribute exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);

    /// Return attribute information for the attribute indicated by the
    /// specified `name` of the specified `nameLength` if the attribute
    /// exists, and 0 otherwise.
    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `MySequenceWithNillableStringSequenceArray`
    /// having the default value.  Use the optionally specified
    /// `basicAllocator` to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    explicit MySequenceWithNillableStringSequenceArray(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `MySequenceWithNillableStringSequenceArray`
    /// having the value of the specified `original` object.  Use the
    /// optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    MySequenceWithNillableStringSequenceArray(const MySequenceWithNillableStringSequenceArray& original,
                                              bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `MySequenceWithNillableStringSequenceArray`
    /// having the value of the specified `original` object.  After
    /// performing this action, the `original` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableStringSequenceArray(MySequenceWithNillableStringSequenceArray&& original) noexcept;

    /// Create an object of type `MySequenceWithNillableStringSequenceArray`
    /// having the value of the specified `original` object.  After
    /// performing this action, the `original` object will be left in a
    /// valid, but unspecified state.  Use the optionally specified
    /// `basicAllocator` to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.
    MySequenceWithNillableStringSequenceArray(MySequenceWithNillableStringSequenceArray&& original,
                                              bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~MySequenceWithNillableStringSequenceArray();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    MySequenceWithNillableStringSequenceArray& operator=(const MySequenceWithNillableStringSequenceArray& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    MySequenceWithNillableStringSequenceArray& operator=(MySequenceWithNillableStringSequenceArray&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Invoke the specified `manipulator` sequentially on the address of
    /// each (modifiable) attribute of this object, supplying `manipulator`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `manipulator` (i.e., the invocation that
    /// terminated the sequence).
    template <typename t_MANIPULATOR>
    int manipulateAttributes(t_MANIPULATOR& manipulator);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `id`,
    /// supplying `manipulator` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if `id` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR& manipulator, int id);

    /// Invoke the specified `manipulator` on the address of
    /// the (modifiable) attribute indicated by the specified `name` of the
    /// specified `nameLength`, supplying `manipulator` with the
    /// corresponding attribute information structure.  Return the value
    /// returned from the invocation of `manipulator` if `name` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateAttribute(t_MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);

    /// Return a reference to the modifiable "Attribute1" attribute of this
    /// object.
    bsl::vector<bdlb::NullableValue<MySequenceWithNillableStringSequenceArraySequence> >& attribute1();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// Invoke the specified `accessor` sequentially on each
    /// (non-modifiable) attribute of this object, supplying `accessor`
    /// with the corresponding attribute information structure until such
    /// invocation returns a non-zero value.  Return the value from the
    /// last invocation of `accessor` (i.e., the invocation that terminated
    /// the sequence).
    template <typename t_ACCESSOR>
    int accessAttributes(t_ACCESSOR& accessor) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `id`, supplying `accessor`
    /// with the corresponding attribute information structure.  Return the
    /// value returned from the invocation of `accessor` if `id` identifies
    /// an attribute of this class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR& accessor, int id) const;

    /// Invoke the specified `accessor` on the (non-modifiable) attribute
    /// of this object indicated by the specified `name` of the specified
    /// `nameLength`, supplying `accessor` with the corresponding attribute
    /// information structure.  Return the value returned from the
    /// invocation of `accessor` if `name` identifies an attribute of this
    /// class, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessAttribute(t_ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;

    /// Return a reference offering non-modifiable access to the
    /// "Attribute1" attribute of this object.
    const bsl::vector<bdlb::NullableValue<MySequenceWithNillableStringSequenceArraySequence> >& attribute1() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` attribute objects
    /// have the same value, and `false` otherwise.  Two attribute objects
    /// have the same value if each respective attribute has the same value.
    friend bool operator==(
                          const MySequenceWithNillableStringSequenceArray& lhs,
                          const MySequenceWithNillableStringSequenceArray& rhs)
    {
        return lhs.attribute1() == rhs.attribute1();
    }

    /// Returns `!(lhs == rhs)`
    friend bool operator!=(
                          const MySequenceWithNillableStringSequenceArray& lhs,
                          const MySequenceWithNillableStringSequenceArray& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const MySequenceWithNillableStringSequenceArray& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_SEQUENCE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::MySequenceWithNillableStringSequenceArray)
template <>
struct bdlat_UsesDefaultValueFlag<s_baltst::MySequenceWithNillableStringSequenceArray> : bsl::true_type {};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace s_baltst {

                        // ----------------------------
                        // class MySequenceWithNillable
                        // ----------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillable::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_myNillable, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
    if (ret) {
        return ret;
    }

    ret = manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillable::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      case ATTRIBUTE_ID_MY_NILLABLE: {
        return manipulator(&d_myNillable, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return manipulator(&d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillable::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
int& MySequenceWithNillable::attribute1()
{
    return d_attribute1;
}

inline
bdlb::NullableValue<bsl::string>& MySequenceWithNillable::myNillable()
{
    return d_myNillable;
}

inline
bsl::string& MySequenceWithNillable::attribute2()
{
    return d_attribute2;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillable::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_myNillable, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
    if (ret) {
        return ret;
    }

    ret = accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillable::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      case ATTRIBUTE_ID_MY_NILLABLE: {
        return accessor(d_myNillable, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MY_NILLABLE]);
      }
      case ATTRIBUTE_ID_ATTRIBUTE2: {
        return accessor(d_attribute2, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE2]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillable::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
int MySequenceWithNillable::attribute1() const
{
    return d_attribute1;
}

inline
const bdlb::NullableValue<bsl::string>& MySequenceWithNillable::myNillable() const
{
    return d_myNillable;
}

inline
const bsl::string& MySequenceWithNillable::attribute2() const
{
    return d_attribute2;
}



                  // ----------------------------------------
                  // class MySequenceWithNillableBase64Binary
                  // ----------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableBase64Binary::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableBase64Binary::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableBase64Binary::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bdlb::NullableValue<bsl::vector<char> >& MySequenceWithNillableBase64Binary::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableBase64Binary::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableBase64Binary::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableBase64Binary::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bdlb::NullableValue<bsl::vector<char> >& MySequenceWithNillableBase64Binary::attribute1() const
{
    return d_attribute1;
}



                   // -------------------------------------
                   // class MySequenceWithNillableHexBinary
                   // -------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableHexBinary::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableHexBinary::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableHexBinary::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bdlb::NullableValue<bsl::vector<char> >& MySequenceWithNillableHexBinary::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableHexBinary::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableHexBinary::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableHexBinary::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bdlb::NullableValue<bsl::vector<char> >& MySequenceWithNillableHexBinary::attribute1() const
{
    return d_attribute1;
}



                    // ------------------------------------
                    // class MySequenceWithNillableIntArray
                    // ------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableIntArray::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableIntArray::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableIntArray::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bsl::vector<bdlb::NullableValue<int> >& MySequenceWithNillableIntArray::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableIntArray::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableIntArray::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableIntArray::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bsl::vector<bdlb::NullableValue<int> >& MySequenceWithNillableIntArray::attribute1() const
{
    return d_attribute1;
}



            // ----------------------------------------------------
            // class MySequenceWithNillableIntSequenceArraySequence
            // ----------------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableIntSequenceArraySequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableIntSequenceArraySequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableIntSequenceArraySequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
int& MySequenceWithNillableIntSequenceArraySequence::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableIntSequenceArraySequence::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableIntSequenceArraySequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableIntSequenceArraySequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
int MySequenceWithNillableIntSequenceArraySequence::attribute1() const
{
    return d_attribute1;
}



                  // ---------------------------------------
                  // class MySequenceWithNillableStringArray
                  // ---------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableStringArray::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableStringArray::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableStringArray::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bsl::vector<bdlb::NullableValue<bsl::string> >& MySequenceWithNillableStringArray::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableStringArray::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableStringArray::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableStringArray::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bsl::vector<bdlb::NullableValue<bsl::string> >& MySequenceWithNillableStringArray::attribute1() const
{
    return d_attribute1;
}



          // -------------------------------------------------------
          // class MySequenceWithNillableStringSequenceArraySequence
          // -------------------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableStringSequenceArraySequence::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableStringSequenceArraySequence::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableStringSequenceArraySequence::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bsl::string& MySequenceWithNillableStringSequenceArraySequence::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableStringSequenceArraySequence::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableStringSequenceArraySequence::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableStringSequenceArraySequence::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bsl::string& MySequenceWithNillableStringSequenceArraySequence::attribute1() const
{
    return d_attribute1;
}



                    // -----------------------------------
                    // class MySequenceWithNillableIntList
                    // -----------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableIntList::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableIntList::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableIntList::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bsl::vector<bdlb::NullableValue<int> >& MySequenceWithNillableIntList::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableIntList::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableIntList::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableIntList::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bsl::vector<bdlb::NullableValue<int> >& MySequenceWithNillableIntList::attribute1() const
{
    return d_attribute1;
}



                // --------------------------------------------
                // class MySequenceWithNillableIntSequenceArray
                // --------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableIntSequenceArray::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableIntSequenceArray::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableIntSequenceArray::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bsl::vector<bdlb::NullableValue<MySequenceWithNillableIntSequenceArraySequence> >& MySequenceWithNillableIntSequenceArray::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableIntSequenceArray::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableIntSequenceArray::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableIntSequenceArray::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bsl::vector<bdlb::NullableValue<MySequenceWithNillableIntSequenceArraySequence> >& MySequenceWithNillableIntSequenceArray::attribute1() const
{
    return d_attribute1;
}



                   // --------------------------------------
                   // class MySequenceWithNillableStringList
                   // --------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableStringList::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableStringList::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableStringList::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bsl::vector<bdlb::NullableValue<bsl::string> >& MySequenceWithNillableStringList::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableStringList::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableStringList::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableStringList::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bsl::vector<bdlb::NullableValue<bsl::string> >& MySequenceWithNillableStringList::attribute1() const
{
    return d_attribute1;
}



              // -----------------------------------------------
              // class MySequenceWithNillableStringSequenceArray
              // -----------------------------------------------

// CLASS METHODS
// MANIPULATORS
template <typename t_MANIPULATOR>
int MySequenceWithNillableStringSequenceArray::manipulateAttributes(t_MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableStringSequenceArray::manipulateAttribute(t_MANIPULATOR& manipulator, int id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return manipulator(&d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_MANIPULATOR>
int MySequenceWithNillableStringSequenceArray::manipulateAttribute(
        t_MANIPULATOR& manipulator,
        const char    *name,
        int            nameLength)
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
bsl::vector<bdlb::NullableValue<MySequenceWithNillableStringSequenceArraySequence> >& MySequenceWithNillableStringSequenceArray::attribute1()
{
    return d_attribute1;
}

// ACCESSORS
template <typename t_ACCESSOR>
int MySequenceWithNillableStringSequenceArray::accessAttributes(t_ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
    if (ret) {
        return ret;
    }

    return 0;
}

template <typename t_ACCESSOR>
int MySequenceWithNillableStringSequenceArray::accessAttribute(t_ACCESSOR& accessor, int id) const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_ATTRIBUTE1: {
        return accessor(d_attribute1, ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_ATTRIBUTE1]);
      }
      default:
        return NOT_FOUND;
    }
}

template <typename t_ACCESSOR>
int MySequenceWithNillableStringSequenceArray::accessAttribute(
        t_ACCESSOR&  accessor,
        const char  *name,
        int          nameLength) const
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
const bsl::vector<bdlb::NullableValue<MySequenceWithNillableStringSequenceArraySequence> >& MySequenceWithNillableStringSequenceArray::attribute1() const
{
    return d_attribute1;
}

}  // close package namespace

// FREE FUNCTIONS

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2025.08.21
// USING bas_codegen.pl s_baltst_mysequencewithnillable.xsd --mode msg --includedir . --msgComponent mysequencewithnillable --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
