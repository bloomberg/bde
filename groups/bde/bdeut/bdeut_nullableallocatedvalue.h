// bdeut_nullableallocatedvalue.h                                     -*-C++-*-
#ifndef INCLUDED_BDEUT_NULLABLEALLOCATEDVALUE
#define INCLUDED_BDEUT_NULLABLEALLOCATEDVALUE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a template for nullable allocated values.
//
//@CLASSES:
//  bdeut_NullableAllocatedValue: template for nullable allocated values
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bdeut_nullablevalue
//
//@DESCRIPTION: This component provides a a 'bdeut_NullableAllocatedValue'
// template that has the same interface as 'bdeut_NullableValue', except that
// the implementation of this component does not require that the 'TYPE'
// parameter be fully defined when the *class* is instantiated.  However, the
// parameterized 'TYPE' must be fully defined when *methods* of the class are
// instantiated (also: constructors, destructor, operators).
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we want to create a linked list of nodes that contain integers:
//..
//  struct LinkedListNode {
//      int                                          d_value;
//      bdeut_NullableAllocatedValue<LinkedListNode> d_next;
//  };
//..
// Note that 'bdeut_NullableValue<LinkedListNode>' cannot be used for 'd_next'
// because 'bdeut_NullableValue' requires that the parameterized 'TYPE' is
// fully defined when the class is instantiated.
//
// We can traverse this linked list and add a new value to at the end using the
// following code:
//..
//  void addValueAtEnd(LinkedListNode *node, int value)
//  {
//      while (!node->d_next.isNull()) {
//          node = &node->d_next.value();
//      }
//
//      node->d_next.makeValue();
//      node = &node->d_next.value();
//      node->d_value = value;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENT
#include <bsls_alignment.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_VERSIONFUNCTIONS
#include <bdex_versionfunctions.h>
#endif

#ifdef BSLS_PLATFORM__CMP_IBM
// This is a temporary workaround for a bug that occurs when using AIX xlC
// compiler.  When compiling a CPP file that uses a nullable value, the AIX xlC
// compiler incorrectly gives an error that 'bdex_ByteInStreamFormatter' and
// 'bdex_ByteOutStreamFormatter' are not defined, even though the file
// correctly includes 'bdex_byteinstreamformatter.h' and
// 'bdex_byteoutstreamformatter.h'.  (Basically, it is the bug where the
// compiler processes an expression that depends on a template parameter,
// before the template has been instantiated).  The temporary workaround is to
// ensure that 'bdex_ByteInStreamFormatter' and 'bdex_ByteOutStreamFormatter'
// are defined before 'bdeut_NullableAllocatedValue' in the translation unit,
// so these files get included here (even though 'bdeut_nullableallocatedvalue'
// does not really depend on the two 'bdex' components).

#ifndef INCLUDED_BDEX_BYTEINSTREAMFORMATTER
#include <bdex_byteinstreamformatter.h>
#endif

#ifndef INCLUDED_BDEX_BYTEOUTSTREAMFORMATTER
#include <bdex_byteoutstreamformatter.h>
#endif

#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

namespace BloombergLP {

struct bdeut_NullableAllocatedValue_Traits
                                          : bslalg_TypeTraitUsesBslmaAllocator,
                                             bdeu_TypeTraitHasPrintMethod
{
};

                  // ========================================
                  // class bdeut_NullableAllocatedValue<TYPE>
                  // ========================================

template <typename TYPE>
class bdeut_NullableAllocatedValue {
    // This is a template for nullable values.

    // PRIVATE DATA MEMBERS
    TYPE            *d_value_p;
    bslma_Allocator *d_allocator_p;

  public:
    // TYPES
    typedef TYPE ValueType;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdeut_NullableAllocatedValue,
                                 bdeut_NullableAllocatedValue_Traits);

    // CREATORS
    explicit bdeut_NullableAllocatedValue(bslma_Allocator *basicAllocator = 0);
       // Create a nullable object with no value, and use the specified
       // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
       // currently installed default allocator is used.

    bdeut_NullableAllocatedValue(
                const bdeut_NullableAllocatedValue<TYPE>&  original,
                bslma_Allocator                           *basicAllocator = 0);
        // Create a copy of the specified 'original' nullable object, and use
        // the specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bdeut_NullableAllocatedValue(const TYPE&      value,
                                 bslma_Allocator *basicAllocator = 0);
        // Create an nullable object with the specified 'value', and use the
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    ~bdeut_NullableAllocatedValue();
        // Destroy this object.

    // MANIPULATORS
    bdeut_NullableAllocatedValue<TYPE>& operator=(
                                const bdeut_NullableAllocatedValue<TYPE>& rhs);
        // Assign to this object the value of the specified 'rhs' object, if
        // 'rhs' has a value.  Otherwise reset this object.  Return a reference
        // to this modifiable nullable object.

    TYPE& operator=(const TYPE& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    void swap(bdeut_NullableAllocatedValue& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee if the
        // 'TYPE' template parameter has a no-throw 'swap' and the result of
        // the 'isNull' method for the two objects being swapped is the same.
        // The behavior is undefined if the objects have non-equal allocators.

    TYPE& makeValue(const TYPE& rhs);
        // Assign the specified 'rhs' value to this nullable object (i.e., make
        // this object not null).

    TYPE& makeValue();
        // Assign the default value of 'TYPE' to this nullable object (i.e.,
        // make this object not null).

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    void reset();
        // Make this object null.

    TYPE& value();
        // Return the value held by this object.  The behavior is undefined
        // unless the object is not null.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.

    bool isNull() const;
        // Return true if this object is null, and false otherwise.

    int maxSupportedBdexVersion() const;
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    const TYPE& value() const;
        // Return the value held by this object.  The behavior is undefined
        // unless the object is not null.
};

// FREE OPERATORS
template <typename LHS_TYPE, typename RHS_TYPE>
bool operator==(const bdeut_NullableAllocatedValue<LHS_TYPE>& lhs,
                const bdeut_NullableAllocatedValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is equal to the specified 'rhs',
    // and 'false' otherwise.

template <typename LHS_TYPE, typename RHS_TYPE>
bool operator!=(const bdeut_NullableAllocatedValue<LHS_TYPE>& lhs,
                const bdeut_NullableAllocatedValue<RHS_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' is different from the specified
    // 'rhs', and 'false' otherwise.

template <typename TYPE>
bsl::ostream& operator<<(bsl::ostream&                             stream,
                         const bdeut_NullableAllocatedValue<TYPE>& rhs);
    // Print the specified 'rhs' to the specified 'stream' in a single line.

// FREE FUNCTIONS
template <typename TYPE>
void swap(bdeut_NullableAllocatedValue<TYPE>& a,
          bdeut_NullableAllocatedValue<TYPE>& b);
    // Swap the values of the specified 'a' and 'b' objects.  This method
    // provides the no-throw guarantee if the 'TYPE' template parameter has a
    // no-throw 'swap' and the result of the 'isNull' method for the two
    // objects being swapped is the same.  The behavior is undefined if the
    // objects have non-equal allocators.

// ---  Anything below this line is implementation specific.  Do not use.  ----

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // ----------------------------------------
                  // class bdeut_NullableAllocatedValue<TYPE>
                  // ----------------------------------------

// CREATORS

template <typename TYPE>
inline
bdeut_NullableAllocatedValue<TYPE>
::bdeut_NullableAllocatedValue(bslma_Allocator *basicAllocator)
: d_value_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <typename TYPE>
inline
bdeut_NullableAllocatedValue<TYPE>
::bdeut_NullableAllocatedValue(
                     const bdeut_NullableAllocatedValue<TYPE>&  original,
                     bslma_Allocator                           *basicAllocator)
: d_value_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    if (!original.isNull()) {
        makeValue(original.value());
    }
}

template <typename TYPE>
inline
bdeut_NullableAllocatedValue<TYPE>
::bdeut_NullableAllocatedValue(const TYPE&      value,
                               bslma_Allocator *basicAllocator)
: d_value_p(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    makeValue(value);
}

template <typename TYPE>
inline
bdeut_NullableAllocatedValue<TYPE>
::~bdeut_NullableAllocatedValue()
{
    reset();
}

// MANIPULATORS

template <typename TYPE>
inline
bdeut_NullableAllocatedValue<TYPE>&
bdeut_NullableAllocatedValue<TYPE>
::operator=(const bdeut_NullableAllocatedValue<TYPE>& rhs)
{
    if (!rhs.isNull()) {
        makeValue(rhs.value());
    }
    else {
        reset();
    }

    return *this;
}

template <typename TYPE>
inline
TYPE&
bdeut_NullableAllocatedValue<TYPE>
::operator=(const TYPE& rhs)
{
    return makeValue(rhs);
}

template <typename TYPE>
inline
void
bdeut_NullableAllocatedValue<TYPE>
::swap(bdeut_NullableAllocatedValue& other)
{
    // 'swap' is undefined for non-equal allocators.

    BSLS_ASSERT(d_allocator_p == other.d_allocator_p);

    // same 'isNull' flags

    if (isNull() && other.isNull()) {
        // nothing to do for empty objects
        return;                                                       // RETURN
    }

    if (!isNull() && !other.isNull()) {
        // swap typed values
        bslalg_SwapUtil::swap(&this->value(), &other.value());
        return;                                                       // RETURN
    }

    // different 'isNull' flags

    bdeut_NullableAllocatedValue *nullObj;
    bdeut_NullableAllocatedValue *nonNullObj;

    if (isNull()) {
        nullObj    = this;
        nonNullObj = &other;
    }
    else {
        nullObj    = &other;
        nonNullObj = this;
    }

    // copy-construct and reset
    nullObj->makeValue(nonNullObj->value()); // this can throw, and then 'swap'
                                             // is only strongly exception-safe
    nonNullObj->reset();
}

template <typename TYPE>
inline
TYPE&
bdeut_NullableAllocatedValue<TYPE>
::makeValue(const TYPE& rhs)
{
    if (d_value_p) {
        *d_value_p = rhs;
        return *d_value_p;                                           // RETURN
    }

    TYPE *value = reinterpret_cast<TYPE*>(
                                        d_allocator_p->allocate(sizeof(TYPE)));
    bslma_DeallocatorProctor<bslma_Allocator> proctor(value, d_allocator_p);
    bslalg_ScalarPrimitives::copyConstruct(value, rhs, d_allocator_p);
    proctor.release();
    d_value_p = value;

    return *d_value_p;
}

template <typename TYPE>
inline
TYPE&
bdeut_NullableAllocatedValue<TYPE>
::makeValue()
{
    reset();

    // Note that this alternative implementation instead of 'reset()' provides
    // stronger exception-safety, but it breaks some client code that uses
    // bdeut_NullableAllocatedValue with a non-value-semantic TYPE.
    //..
    // if (d_value_p) {
    //     *d_value_p = TYPE(d_allocator_p);
    //     return *d_value_p;                                         // RETURN
    // }
    //..

    TYPE *value = reinterpret_cast<TYPE*>(
                                        d_allocator_p->allocate(sizeof(TYPE)));
    bslma_DeallocatorProctor<bslma_Allocator> proctor(value, d_allocator_p);
    bslalg_ScalarPrimitives::defaultConstruct(value, d_allocator_p);
    proctor.release();
    d_value_p = value;

    return *d_value_p;
}

template <typename TYPE>
template <class STREAM>
STREAM&
bdeut_NullableAllocatedValue<TYPE>
::bdexStreamIn(STREAM& stream, int     version)
{
    char isNull;

    stream.getInt8(isNull);

    if (stream) {
        if (!isNull) {
            makeValue();
            bdex_InStreamFunctions::streamIn(stream, value(), version);
        }
        else {
            reset();
        }
    }

    return stream;
}

template <typename TYPE>
inline
void
bdeut_NullableAllocatedValue<TYPE>
::reset()
{
    if (d_value_p) {
        d_value_p->~TYPE();
        d_allocator_p->deallocate(d_value_p);
        d_value_p = 0;
    }
}

template <typename TYPE>
inline
TYPE&
bdeut_NullableAllocatedValue<TYPE>
::value()
{
    BSLS_ASSERT_SAFE(!isNull());

    return *d_value_p;
}

// ACCESSORS

template <typename TYPE>
template <class STREAM>
STREAM&
bdeut_NullableAllocatedValue<TYPE>
::bdexStreamOut(STREAM& stream, int version) const
{
    stream.putInt8(isNull() ? 1 : 0);

    if (!isNull()) {
        bdex_OutStreamFunctions::streamOut(stream, value(), version);
    }

    return stream;
}

template <typename TYPE>
inline
bool
bdeut_NullableAllocatedValue<TYPE>
::isNull() const
{
    return 0 == d_value_p;
}

template <typename TYPE>
inline
int
bdeut_NullableAllocatedValue<TYPE>
::maxSupportedBdexVersion() const
{
    return bdex_VersionFunctions::maxSupportedVersion(value());
}

template <typename TYPE>
inline
bsl::ostream&
bdeut_NullableAllocatedValue<TYPE>
::print(bsl::ostream& stream,
        int           level,
        int           spacesPerLevel) const
{
    if (isNull()) {
        return bdeu_PrintMethods::print(stream, "NULL", level, spacesPerLevel);
    }

    return bdeu_PrintMethods::print(stream, value(), level, spacesPerLevel);
}

template <typename TYPE>
inline
const TYPE&
bdeut_NullableAllocatedValue<TYPE>
::value() const
{
    // TBD
    // The assert below was commented out because a call to this function is
    // sometimes used as an argument to a template function that only looks at
    // the value type (and does not access the value).

    // BSLS_ASSERT_SAFE(!isNull());

    return *d_value_p;
}

// FREE OPERATORS

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator==(const bdeut_NullableAllocatedValue<LHS_TYPE>& lhs,
                const bdeut_NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        return lhs.value() == rhs.value();
    }

    return lhs.isNull() == rhs.isNull();
}

template <typename LHS_TYPE, typename RHS_TYPE>
inline
bool operator!=(const bdeut_NullableAllocatedValue<LHS_TYPE>& lhs,
                const bdeut_NullableAllocatedValue<RHS_TYPE>& rhs)
{
    if (!lhs.isNull() && !rhs.isNull()) {
        return lhs.value() != rhs.value();
    }

    return lhs.isNull() != rhs.isNull();
}

template <typename TYPE>
inline
bsl::ostream& operator<<(bsl::ostream&                             stream,
                         const bdeut_NullableAllocatedValue<TYPE>& rhs)
{
    return rhs.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <typename TYPE>
inline
void swap(bdeut_NullableAllocatedValue<TYPE>& a,
          bdeut_NullableAllocatedValue<TYPE>& b)
{
    a.swap(b);
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
