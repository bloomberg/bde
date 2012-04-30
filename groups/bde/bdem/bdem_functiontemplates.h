// bdem_functiontemplates.h                                           -*-C++-*-
#ifndef INCLUDED_BDEM_FUNCTIONTEMPLATES
#define INCLUDED_BDEM_FUNCTIONTEMPLATES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide function templates for common operations on elements.
//
//@CLASSES:
//   bdem_FunctionTemplates: namespace for function templates and descriptions
//
//@SEE_ALSO: bdem_descriptor, bdem_properties
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: Within 'bdem', there are a number of operations that need to
// be implemented for many data types.  For example, every data type needs a
// way to construct an "unset" value, copy one value to another, or compare
// two values for equality.  When these functions are called from
// type-agnostic layers, functions must be provided to convert untyped
// ('void*') references to the values into type-specific operations.  For
// large categories of data types, these operations are identical and can
// profitably be implemented using function templates.  This component
// provides templated implementations for many common operations on several
// categories of data types.
//
// The non-stream-related functions in 'bdem_FunctionTemplates' are templatized
// only on the type of object on which they operate.
//
// The 'streamIn' and 'streamOut' functions are parameterized on both the
// object type and the stream type.  These functions provide type-specific
// streaming operations.
//
// Note that not all of these functions are suitable for all data types.  For
// example, a function with 'Fundamental' in its name should be instantiated
// using a fundamental type (e.g., 'char', 'int') and not a user-defined type.
//
///Usage
///-----
// These functions would typically be instantiated in arrays or structs
// containing function pointers.  A dispatch to the correct function can then
// be performed at run-time.
//..
//  enum my_ObjType { INT_OBJ, DATE_OBJ, STRING_OBJ };
//
//  struct my_ObjFuncs {
//      void (*copyConstruct)(
//                           void                                  *obj,
//                           const void                            *rhs,
//                           bdem_AggregateOption::AllocationMode  allocMode,
//                           bslma_Allocator                       *allocator);
//
//      void (*destroy)(void *obj);
//  };
//
//  const my_ObjFuncs my_FuncsDispatch[] = {
//      { bdem_FunctionTemplates::copyConstruct<int>,
//        bdem_FunctionTemplates::destroy<int>
//      },
//      { bdem_FunctionTemplates::copyConstruct<bdet_Date>,
//        bdem_FunctionTemplates::destroy<bdet_Date>
//      },
//      { bdem_FunctionTemplates::copyConstruct<int>,
//        bdem_FunctionTemplates::destroy<bsl::string>
//      }
//  };
//
//  // Object capable of holding an int, date, or string.
//  class my_MultiObject {
//      union {
//          int                            d_int;
//          bsls_ObjectBuffer<bdet_Date>   d_date;
//          bsls_ObjectBuffer<bsl::string> d_string;
//      } d_buffer;
//      const my_ObjFuncs *d_funcs;
//
//    public:
//      my_MultiObject() : d_funcs(&my_FuncsDispatch[INT_OBJ]) { }
//      ~my_MultiObject();
//
//      void setValue(my_ObjType type, const void* value);
//
//      int theInt() const { return d_buffer.d_int; }
//      const bdet_Date& theDate() const { return d_buffer.d_date.object(); }
//      const bsl::string& theString() const
//          { return d_buffer.d_string.object(); }
//  };
//..
// Note the use of dynamic dispatch in the implementation of these functions.
//..
//  void my_MultiObject::~my_MultiObject()
//  {
//      d_funcs.destroy(d_buffer);
//  }
//
//  void my_MultiObject::setValue(my_ObjType type, const void* value);
//  {
//      d_funcs->destroy(&d_buffer);
//      d_funcs = &my_FuncsDispatch[type];
//      d_funcs->copyConstruct(&d_buffer, value,
//                             bdem_AggregateOption::BDEM_PASS_THROUGH, 0);
//  }
//..
// The above 'my_MultiObject' class could then be used as follows:
//..
//  int i = 5;
//  bdet_Date d(2003, 12, 25);
//  bsl::string s("Hello world");
//
//  my_MultiObject obj;
//  obj.setValue(INT_OBJ, &i);          assert(obj.theInt() == i);
//  obj.setValue(DATE_OBJ, &d);         assert(obj.theDate() == d);
//  obj.setValue(STRING_OBJ, &i);       assert(obj.theString() == s);
//  // Destructor deletes string object.
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#include <bdem_aggregateoption.h>
#endif

#ifndef INCLUDED_BDEIMP_BITWISECOPY
#include <bdeimp_bitwisecopy.h>
#endif

#ifndef INCLUDED_BDETU_UNSET
#include <bdetu_unset.h>
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

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

struct bdem_Descriptor;

template <class STREAM> struct bdem_DescriptorStreamIn;
template <class STREAM> struct bdem_DescriptorStreamOut;

                        // =============================
                        // struct bdem_FunctionTemplates
                        // =============================

struct bdem_FunctionTemplates {
    // This 'struct' provides a namespace for a suite of function templates.
    // The functions provide wrappers for common operations such as
    // construction, destruction, assignment, etc., on objects that are passed
    // in as raw memory.  This untemplatized 'struct' contains templatized
    // member functions because, in actual use, the functions will be
    // instantiated individually rather than as a group.

  public:
    // CLASS METHODS
    template <class T>
    static
    void defaultConstruct(
                      void                                     *obj,
                      bdem_AggregateOption::AllocationStrategy  allocationMode,
                      bslma_Allocator                          *allocator);
        // Construct the a 'T' object using its default constructor.  The
        // 'allocationMode' is discarded in this implementation, making it
        // inappropriate for types that expect this constructor option.  Such
        // types must have their own (custom) function that does not discard
        // 'allocationMode'.  'allocationMode' is present for signature
        // compatibility with such custom functions.  The 'allocator' pointer
        // is propagated to constructors of types that can use it and ignored
        // for other types.

    template <class T>
    static
    void unsetConstruct(
                      void                                     *obj,
                      bdem_AggregateOption::AllocationStrategy  allocationMode,
                      bslma_Allocator                          *allocator);
        // Construct a 'T' object with an unset value as defined in
        // 'bdetu_unset'.  This function will fail to compile if instantiated
        // for a type that does not have an unset value specialization in
        // 'bdetu_unset'.  Only the fundamental types have an unset value
        // distinct from the default-constructed value.  For all other types,
        // either 'defaultConstruct()' or a custom function should be used
        // instead.  The 'allocationMode' is ignored and exists only for
        // signature compatibility with 'defaultConstruct()'.  The 'allocator'
        // pointer is propagated to constructors of types that can use it and
        // ignored for other types.

    template <class T>
    static
    void copyConstruct(
                      void                                     *obj,
                      const void                               *rhs,
                      bdem_AggregateOption::AllocationStrategy  allocationMode,
                      bslma_Allocator                          *allocator);
        // Construct a 'T' object as a copy of '*obj', using the copy
        // constructor for 'T'.  The 'allocationMode' is discarded in this
        // implementation, making it inappropriate for types that expect his
        // constructor option.  Such types must have their own (custom)
        // function that does not discard 'allocationMode'.  'allocationMode'
        // is present for signature compatibility with such custom functions.
        // The 'allocator' pointer is propagated to constructors of types that
        // can it use and ignored for other types.  The behavior is undefined
        // unless 'rhs' points to an object of type 'T'.

    template <class T>
    static void destroy(void *obj);
        // Call the destructor for the 'T' object pointed to by 'obj'.  The
        // behavior is undefined unless 'obj' is the address of a valid object
        // of type 'T'.

    template <class T>
    static void assign(void *lhs, const void *rhs);
        // Use 'T's assignment operator to copy '*rhs' into '*lhs'.  The
        // behavior is undefined unless both 'lhs' and 'rhs' are addresses of
        // valid objects of type 'T'.

    template <class T>
    static void bitwiseMove(void *lhs, void *rhs);
        // Move an object from rhs location to lhs location.  The !rhs is not
        // const! because it is altered (effectively destroyed) by the move
        // operation.  The behavior is undefined unless 'T' is bitwise-movable
        // and both 'lhs' or 'rhs' point to objects of type 'T'.

    template <class T>
    static void makeUnset(void *obj);
        // Set object, '*obj', of type 'T' to its unset value, as defined in
        // 'bdetu_unset'.  This function will fail to compile if instantiated
        // for a type that does not have an unset value specialization in
        // 'bdetu_unset'.  The behavior is undefined unless 'obj' points to an
        // object of type 'T'.

    template <class T>
    static void removeAll(void *obj);
        // Remove all elements from the container, '*obj', of type 'T'.  This
        // function will fail to compile if instantiated for a type that does
        // not have a 'removeAll' method.  The behavior is undefined unless
        // 'obj' points to an object of type 'T'.

    template <class T>
    static bool isUnset(const void *obj);
        // Return 'true' if '*obj' has an unset value, as defined in
        // 'bdetu_unset', and 'false' otherwise.  This function will fail to
        // compile if instantiated for a type that does not have an unset
        // value specialization in 'bdetu_unset'.  The behavior is undefined
        // unless 'obj' points to an object of type 'T'.

    template <class T>
    static bool isEmpty(const void *obj);
        // Return 'true' if '*obj' is an empty container, and 'false'
        // otherwise.  This function will fail to compile if instantiated for
        // a type that does not have an 'isEmpty' method.  The behavior is
        // undefined unless 'obj' points to an object of type 'T'.

    template <class T>
    static bool areEqual(const void *lhs, const void *rhs);
        // Return the result of comparing two 'T' objects, '*lhs' and '*rhs'
        // using 'operator==(const T&, const T&)'.  The behavior is undefined
        // unless both 'lhs' and 'rhs' point to objects of type 'T'.

    template <class T>
    static bsl::ostream& print(const void    *obj,
                               bsl::ostream&  stream,
                               int            level,
                               int            spacesPerLevel);

    template <class T>
    static
    bsl::ostream& printArray(const void    *obj,
                             bsl::ostream&  stream,
                             int            level,
                             int            spacesPerLevel);

    template <class T, class STREAM>
    static void streamIn(
                    void                                  *obj,
                    STREAM&                                stream,
                    int                                    version,
                    const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const bdem_Descriptor                 *const attrLookup[]);

    template <class T, class STREAM>
    static void streamInFundamental(
                    void                                  *obj,
                    STREAM&                                stream,
                    int                                    version,
                    const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const bdem_Descriptor                 *const attrLookup[]);

    template <class T, class STREAM>
    static void streamInArray(
                    void                                  *obj,
                    STREAM&                                stream,
                    int                                    version,
                    const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const bdem_Descriptor                 *const attrLookup[]);

    template <class T, class STREAM>
    static void
    streamOut(const void                             *obj,
              STREAM&                                 stream,
              int                                     version,
              const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup);

    template <class T, class STREAM>
    static void
    streamOutFundamental(
                       const void                             *obj,
                       STREAM&                                 stream,
                       int                                     version,
                       const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup);

    template <class T, class STREAM>
    static void
    streamOutArray(const void                             *obj,
                   STREAM&                                 stream,
                   int                                     version,
                   const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup);
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -----------------------------
                        // struct bdem_FunctionTemplates
                        // -----------------------------

// CLASS METHODS
template <class T>
void bdem_FunctionTemplates::defaultConstruct(
                      void                                     *obj,
                      bdem_AggregateOption::AllocationStrategy,
                      bslma_Allocator                          *allocator)
{
    BSLS_ASSERT_SAFE(obj);

    bslalg_ScalarPrimitives::defaultConstruct(static_cast<T *>(obj),
                                              allocator);
}

template <class T>
void bdem_FunctionTemplates::unsetConstruct(
                       void                                     *obj,
                       bdem_AggregateOption::AllocationStrategy,
                       bslma_Allocator                          *allocator)
{
    BSLS_ASSERT_SAFE(obj);

    // This implementation works only for types that have unset values as
    // defined in 'bdetu_unset'.  For most other types, the default
    // constructor produces the appropriate unset value.

    bslalg_ScalarPrimitives::defaultConstruct(static_cast<T *>(obj),
                                              allocator);
    bdetu_Unset<T>::makeUnset(static_cast<T *>(obj));
}

template <class T>
void bdem_FunctionTemplates::copyConstruct(
                      void                                     *obj,
                      const void                               *rhs,
                      bdem_AggregateOption::AllocationStrategy,
                      bslma_Allocator                          *allocator)
{
    BSLS_ASSERT_SAFE(obj);
    BSLS_ASSERT_SAFE(rhs);

    bslalg_ScalarPrimitives::copyConstruct(static_cast<T *>(obj),
                                           *static_cast<const T *>(rhs),
                                           allocator);
}

template <class T>
void bdem_FunctionTemplates::destroy(void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<T *>(obj)->~T();
}

template <class T>
void bdem_FunctionTemplates::assign(void *lhs, const void *rhs)
{
    BSLS_ASSERT_SAFE(lhs);
    BSLS_ASSERT_SAFE(rhs);

    *static_cast<T *>(lhs) = *static_cast<const T *>(rhs);
}

template <class T>
void bdem_FunctionTemplates::bitwiseMove(void *lhs, void *rhs)
{
    BSLS_ASSERT_SAFE(lhs);
    BSLS_ASSERT_SAFE(rhs);

    // Most types, even types containing references and pointers, can
    // be bit-wise moved.  This function shouldn't be used for types
    // containing pointers or references pointing within themselves.

    bdeimp_BitwiseCopy<T>::copy(static_cast<T *>(lhs), static_cast<T *>(rhs));
}

template <class T>
void bdem_FunctionTemplates::makeUnset(void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    // This implementation works only for types that have unset values defined
    // in 'bdetu_unset'.  For most other types, use some other function that
    // returns the object to its default state.

    bdetu_Unset<T>::makeUnset(static_cast<T *>(obj));
}

template <class T>
void bdem_FunctionTemplates::removeAll(void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    // This function works for types containing a 'removeAll()' method that
    // returns the object to its initial, default value.

    static_cast<T *>(obj)->clear();
}

template <class T>
bool bdem_FunctionTemplates::isUnset(const void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    return bdetu_Unset<T>::isUnset(*static_cast<const T *>(obj));
}

template <class T>
bool bdem_FunctionTemplates::isEmpty(const void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    return 0 == static_cast<const T *>(obj)->size();
}

template <class T>
bool bdem_FunctionTemplates::areEqual(const void *lhs, const void *rhs)
{
    BSLS_ASSERT_SAFE(lhs);
    BSLS_ASSERT_SAFE(rhs);

    return *static_cast<const T *>(lhs) == *static_cast<const T *>(rhs);
}

template <class T>
bsl::ostream& bdem_FunctionTemplates::print(const void    *obj,
                                            bsl::ostream&  stream,
                                            int            level,
                                            int            spacesPerLevel)
{
    BSLS_ASSERT_SAFE(obj);

    bdeu_PrintMethods::print(stream,
                             *static_cast<const T *>(obj),
                             level,
                             spacesPerLevel);
    return stream;
}

template <class T>
bsl::ostream& bdem_FunctionTemplates::printArray(const void    *obj,
                                                 bsl::ostream&  stream,
                                                 int            level,
                                                 int            spacesPerLevel)
{
    BSLS_ASSERT_SAFE(obj);

    const T& vectorItem = *static_cast<const T *>(obj);

    return bdeu_PrintMethods::print(stream,
                                    vectorItem,
                                    level,
                                    spacesPerLevel);
}

template <class T, class STREAM>
void bdem_FunctionTemplates::streamIn(
                     void                                  *obj,
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *,
                     const bdem_Descriptor                 *const attrLookup[])
{
    BSLS_ASSERT_SAFE(obj);

    // 'attrLookup' is unused by most types.
    (void) attrLookup;

    bdex_InStreamFunctions::streamIn(stream, *static_cast<T *>(obj), version);
}

template <class T, class STREAM>
void bdem_FunctionTemplates::streamInFundamental(
                     void                                  *obj,
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const bdem_Descriptor                 *const attrLookup[])
{
    BSLS_ASSERT_SAFE(obj);

    bdem_FunctionTemplates::streamIn<T, STREAM>(obj,
                                                stream,
                                                version,
                                                strmAttrLookup,
                                                attrLookup);
}

template <class T, class STREAM>
void bdem_FunctionTemplates::streamInArray(
                     void                                  *obj,
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const bdem_Descriptor                 *const attrLookup[])
{
    BSLS_ASSERT_SAFE(obj);

    bdem_FunctionTemplates::streamIn<bsl::vector<T>, STREAM>(obj,
                                                             stream,
                                                             version,
                                                             strmAttrLookup,
                                                             attrLookup);
}

template <class T, class STREAM>
void bdem_FunctionTemplates::streamOut(
                        const void                             *obj,
                        STREAM&                                 stream,
                        int                                     version,
                        const bdem_DescriptorStreamOut<STREAM> *)
{
    BSLS_ASSERT_SAFE(obj);

    const T *item = static_cast<const T *>(obj);
    bdex_OutStreamFunctions::streamOut(stream, *item, version);
}

template <class T, class STREAM>
void bdem_FunctionTemplates::streamOutFundamental(
                        const void                             *obj,
                        STREAM&                                 stream,
                        int                                     version,
                        const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup)
{
    BSLS_ASSERT_SAFE(obj);

    bdem_FunctionTemplates::streamOut<T, STREAM>(obj,
                                                 stream,
                                                 version,
                                                 strmAttrLookup);
}

template <class T, class STREAM>
void bdem_FunctionTemplates::streamOutArray(
                        const void                             *obj,
                        STREAM&                                 stream,
                        int                                     version,
                        const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup)
{
    BSLS_ASSERT_SAFE(obj);

    bdem_FunctionTemplates::streamOut<bsl::vector<T>, STREAM>(obj,
                                                              stream,
                                                              version,
                                                              strmAttrLookup);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
