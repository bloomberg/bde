// bdlmxxx_functiontemplates.h                                        -*-C++-*-
#ifndef INCLUDED_BDLMXXX_FUNCTIONTEMPLATES
#define INCLUDED_BDLMXXX_FUNCTIONTEMPLATES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide function templates for common operations on elements.
//
//@CLASSES:
//   bdlmxxx::FunctionTemplates: namespace for function templates and descriptions
//
//@SEE_ALSO: bdlmxxx_descriptor, bdlmxxx_properties
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
// The non-stream-related functions in 'bdlmxxx::FunctionTemplates' are templatized
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
//      void (*copyConstruct)(void                                 *obj,
//                            const void                           *rhs,
//                            bdlmxxx::AggregateOption::AllocationMode  allocMode,
//                            bslma::Allocator                     *allocator);
//
//      void (*destroy)(void *obj);
//  };
//
//  const my_ObjFuncs my_FuncsDispatch[] = {
//      { bdlmxxx::FunctionTemplates::copyConstruct<int>,
//        bdlmxxx::FunctionTemplates::destroy<int>
//      },
//      { bdlmxxx::FunctionTemplates::copyConstruct<bdlt::Date>,
//        bdlmxxx::FunctionTemplates::destroy<bdlt::Date>
//      },
//      { bdlmxxx::FunctionTemplates::copyConstruct<int>,
//        bdlmxxx::FunctionTemplates::destroy<bsl::string>
//      }
//  };
//
//  // Object capable of holding an int, date, or string.
//  class my_MultiObject {
//      union {
//          int                             d_int;
//          bsls::ObjectBuffer<bdlt::Date>   d_date;
//          bsls::ObjectBuffer<bsl::string> d_string;
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
//      const bdlt::Date& theDate() const { return d_buffer.d_date.object(); }
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
//                             bdlmxxx::AggregateOption::BDEM_PASS_THROUGH, 0);
//  }
//..
// The above 'my_MultiObject' class could then be used as follows:
//..
//  int i = 5;
//  bdlt::Date d(2003, 12, 25);
//  bsl::string s("Hello world");
//
//  my_MultiObject obj;
//  obj.setValue(INT_OBJ, &i);          assert(obj.theInt() == i);
//  obj.setValue(DATE_OBJ, &d);         assert(obj.theDate() == d);
//  obj.setValue(STRING_OBJ, &i);       assert(obj.theString() == s);
//  // Destructor deletes string object.
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_AGGREGATEOPTION
#include <bdlmxxx_aggregateoption.h>
#endif

#ifndef INCLUDED_BDLIMPXXX_BITWISECOPY
#include <bdlimpxxx_bitwisecopy.h>
#endif

#ifndef INCLUDED_BDLTUXXX_UNSET
#include <bdltuxxx_unset.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BDLXXXX_INSTREAMFUNCTIONS
#include <bdlxxxx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDLXXXX_OUTSTREAMFUNCTIONS
#include <bdlxxxx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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

namespace BloombergLP {


namespace bdlmxxx {struct Descriptor;

template <class STREAM> struct DescriptorStreamIn;
template <class STREAM> struct DescriptorStreamOut;

                        // =============================
                        // struct FunctionTemplates
                        // =============================

struct FunctionTemplates {
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
                      AggregateOption::AllocationStrategy  allocationMode,
                      bslma::Allocator                         *allocator);
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
                      AggregateOption::AllocationStrategy  allocationMode,
                      bslma::Allocator                         *allocator);
        // Construct a 'T' object with an unset value as defined in
        // 'bdltuxxx_unset'.  This function will fail to compile if instantiated
        // for a type that does not have an unset value specialization in
        // 'bdltuxxx_unset'.  Only the fundamental types have an unset value
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
                      AggregateOption::AllocationStrategy  allocationMode,
                      bslma::Allocator                         *allocator);
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
        // 'bdltuxxx_unset'.  This function will fail to compile if instantiated
        // for a type that does not have an unset value specialization in
        // 'bdltuxxx_unset'.  The behavior is undefined unless 'obj' points to an
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
        // 'bdltuxxx_unset', and 'false' otherwise.  This function will fail to
        // compile if instantiated for a type that does not have an unset
        // value specialization in 'bdltuxxx_unset'.  The behavior is undefined
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
                    const DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const Descriptor                 *const attrLookup[]);

    template <class T, class STREAM>
    static void streamInFundamental(
                    void                                  *obj,
                    STREAM&                                stream,
                    int                                    version,
                    const DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const Descriptor                 *const attrLookup[]);

    template <class T, class STREAM>
    static void streamInArray(
                    void                                  *obj,
                    STREAM&                                stream,
                    int                                    version,
                    const DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const Descriptor                 *const attrLookup[]);

    template <class T, class STREAM>
    static void
    streamOut(const void                             *obj,
              STREAM&                                 stream,
              int                                     version,
              const DescriptorStreamOut<STREAM> *strmAttrLookup);

    template <class T, class STREAM>
    static void
    streamOutFundamental(
                       const void                             *obj,
                       STREAM&                                 stream,
                       int                                     version,
                       const DescriptorStreamOut<STREAM> *strmAttrLookup);

    template <class T, class STREAM>
    static void
    streamOutArray(const void                             *obj,
                   STREAM&                                 stream,
                   int                                     version,
                   const DescriptorStreamOut<STREAM> *strmAttrLookup);
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // struct FunctionTemplates
                        // -----------------------------

// CLASS METHODS
template <class T>
void FunctionTemplates::defaultConstruct(
                      void                                     *obj,
                      AggregateOption::AllocationStrategy,
                      bslma::Allocator                         *allocator)
{
    BSLS_ASSERT_SAFE(obj);

    bslalg::ScalarPrimitives::defaultConstruct(static_cast<T *>(obj),
                                              allocator);
}

template <class T>
void FunctionTemplates::unsetConstruct(
                       void                                     *obj,
                       AggregateOption::AllocationStrategy,
                       bslma::Allocator                         *allocator)
{
    BSLS_ASSERT_SAFE(obj);

    // This implementation works only for types that have unset values as
    // defined in 'bdltuxxx_unset'.  For most other types, the default
    // constructor produces the appropriate unset value.

    bslalg::ScalarPrimitives::defaultConstruct(static_cast<T *>(obj),
                                              allocator);
    bdltuxxx::Unset<T>::makeUnset(static_cast<T *>(obj));
}

template <class T>
void FunctionTemplates::copyConstruct(
                      void                                     *obj,
                      const void                               *rhs,
                      AggregateOption::AllocationStrategy,
                      bslma::Allocator                         *allocator)
{
    BSLS_ASSERT_SAFE(obj);
    BSLS_ASSERT_SAFE(rhs);

    bslalg::ScalarPrimitives::copyConstruct(static_cast<T *>(obj),
                                           *static_cast<const T *>(rhs),
                                           allocator);
}

template <class T>
void FunctionTemplates::destroy(void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<T *>(obj)->~T();
}

template <class T>
void FunctionTemplates::assign(void *lhs, const void *rhs)
{
    BSLS_ASSERT_SAFE(lhs);
    BSLS_ASSERT_SAFE(rhs);

    *static_cast<T *>(lhs) = *static_cast<const T *>(rhs);
}

template <class T>
void FunctionTemplates::bitwiseMove(void *lhs, void *rhs)
{
    BSLS_ASSERT_SAFE(lhs);
    BSLS_ASSERT_SAFE(rhs);

    // Most types, even types containing references and pointers, can
    // be bit-wise moved.  This function shouldn't be used for types
    // containing pointers or references pointing within themselves.

    bdlimpxxx::BitwiseCopy<T>::copy(static_cast<T *>(lhs), static_cast<T *>(rhs));
}

template <class T>
void FunctionTemplates::makeUnset(void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    // This implementation works only for types that have unset values defined
    // in 'bdltuxxx_unset'.  For most other types, use some other function that
    // returns the object to its default state.

    bdltuxxx::Unset<T>::makeUnset(static_cast<T *>(obj));
}

template <class T>
void FunctionTemplates::removeAll(void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    // This function works for types containing a 'removeAll()' method that
    // returns the object to its initial, default value.

    static_cast<T *>(obj)->clear();
}

template <class T>
bool FunctionTemplates::isUnset(const void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    return bdltuxxx::Unset<T>::isUnset(*static_cast<const T *>(obj));
}

template <class T>
bool FunctionTemplates::isEmpty(const void *obj)
{
    BSLS_ASSERT_SAFE(obj);

    return 0 == static_cast<const T *>(obj)->size();
}

template <class T>
bool FunctionTemplates::areEqual(const void *lhs, const void *rhs)
{
    BSLS_ASSERT_SAFE(lhs);
    BSLS_ASSERT_SAFE(rhs);

    return *static_cast<const T *>(lhs) == *static_cast<const T *>(rhs);
}

template <class T>
bsl::ostream& FunctionTemplates::print(const void    *obj,
                                            bsl::ostream&  stream,
                                            int            level,
                                            int            spacesPerLevel)
{
    BSLS_ASSERT_SAFE(obj);

    bdlb::PrintMethods::print(stream,
                             *static_cast<const T *>(obj),
                             level,
                             spacesPerLevel);
    return stream;
}

template <class T>
bsl::ostream& FunctionTemplates::printArray(const void    *obj,
                                                 bsl::ostream&  stream,
                                                 int            level,
                                                 int            spacesPerLevel)
{
    BSLS_ASSERT_SAFE(obj);

    const T& vectorItem = *static_cast<const T *>(obj);

    return bdlb::PrintMethods::print(stream,
                                    vectorItem,
                                    level,
                                    spacesPerLevel);
}

template <class T, class STREAM>
void FunctionTemplates::streamIn(
                     void                                  *obj,
                     STREAM&                                stream,
                     int                                    version,
                     const DescriptorStreamIn<STREAM> *,
                     const Descriptor                 *const attrLookup[])
{
    BSLS_ASSERT_SAFE(obj);

    // 'attrLookup' is unused by most types.
    (void) attrLookup;

    bdex_InStreamFunctions::streamIn(stream, *static_cast<T *>(obj), version);
}

template <class T, class STREAM>
void FunctionTemplates::streamInFundamental(
                     void                                  *obj,
                     STREAM&                                stream,
                     int                                    version,
                     const DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const Descriptor                 *const attrLookup[])
{
    BSLS_ASSERT_SAFE(obj);

    FunctionTemplates::streamIn<T, STREAM>(obj,
                                                stream,
                                                version,
                                                strmAttrLookup,
                                                attrLookup);
}

template <class T, class STREAM>
void FunctionTemplates::streamInArray(
                     void                                  *obj,
                     STREAM&                                stream,
                     int                                    version,
                     const DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const Descriptor                 *const attrLookup[])
{
    BSLS_ASSERT_SAFE(obj);

    FunctionTemplates::streamIn<bsl::vector<T>, STREAM>(obj,
                                                             stream,
                                                             version,
                                                             strmAttrLookup,
                                                             attrLookup);
}

template <class T, class STREAM>
void FunctionTemplates::streamOut(
                        const void                             *obj,
                        STREAM&                                 stream,
                        int                                     version,
                        const DescriptorStreamOut<STREAM> *)
{
    BSLS_ASSERT_SAFE(obj);

    const T *item = static_cast<const T *>(obj);
    bdex_OutStreamFunctions::streamOut(stream, *item, version);
}

template <class T, class STREAM>
void FunctionTemplates::streamOutFundamental(
                        const void                             *obj,
                        STREAM&                                 stream,
                        int                                     version,
                        const DescriptorStreamOut<STREAM> *strmAttrLookup)
{
    BSLS_ASSERT_SAFE(obj);

    FunctionTemplates::streamOut<T, STREAM>(obj,
                                                 stream,
                                                 version,
                                                 strmAttrLookup);
}

template <class T, class STREAM>
void FunctionTemplates::streamOutArray(
                        const void                             *obj,
                        STREAM&                                 stream,
                        int                                     version,
                        const DescriptorStreamOut<STREAM> *strmAttrLookup)
{
    BSLS_ASSERT_SAFE(obj);

    FunctionTemplates::streamOut<bsl::vector<T>, STREAM>(obj,
                                                              stream,
                                                              version,
                                                              strmAttrLookup);
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
