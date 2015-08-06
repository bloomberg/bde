// bdlat_typename.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLAT_TYPENAME
#define INCLUDED_BDLAT_TYPENAME

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide string representations for data type names.
//
//@CLASSES:
//  bdlat_TypeName: namespace for type-name functions
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@SEE_ALSO: http://www.w3.org/TR/xmlschema-2/#built-in-datatypes
//
//@DESCRIPTION: This component defines a structure 'bdlat_TypeName' which
// provides a namespace for functions returning information about the object
// types.  Functions in this namespace allow users to get access to three
// categories of information:
//..
//  o class name from the metadata associated with the object type
//  o generic type name
//  o XML/XSD type name, based on object type and formatting mode.
//..
///Class Name Information
///----------------------
// The template function 'className' returns the object class name from the
// metadata associated with given object type.  Metadata is available for
// the C++ types that have one the following traits:
//..
//  'bdlat_TypeTraitBasicChoice'
//  'bdlat_TypeTraitBasicSequence'
//  'bdlat_TypeTraitBasicCustomizedType'
//  'bdlat_TypeTraitBasicEnumeration'
//..
// If metadata is not available for the object type, the function 'className'
// returns 0 unless the function 'bdlat_TypeName_className' is overloaded by
// developer.
//
///Overloable Class Name Functions For User Defined Classes
///--------------------------------------------------------
// To provide the custom name for the given user-defined C++ class, the
// developer should overload the template function 'bdlat_TypeName_className'
// for this type in the namespace where the type is defined.
//
//WARNING! Do not extend 'bdlat_TypeName_Overloadable' namespace.
//
///Generic Type Name Information
///-----------------------------
// The template functions 'name' returns the generic type name for the given
// object.  The generic type name is one of the following:
//..
//  o predefined name for fundamental types
//  o class name from 'bdlat_TypeName_className', if such function returns a
//    non-null value
//  o name obtained from 'type_info' object provided by C++ runtime, if no
//    class name is available
//..
///XSD Type Name Information
///-------------------------
// The template functions 'xsdName' returns the XML/XSD type name, based on
// the object type and formatting mode.  The returned value is one of the
// following:
//..
//   o predefined name for built-in XSD types
//   o class name from 'bdlat_TypeName_className', if such function returns
//     a non-null value
//   o the "anyType" string, if no class name is available
//..
// This component also defines the XSD names for the following C++ types and
// formatting modes:
//..
//  C++ Type                      Formatting Mode             XML Name
//  --------                      ---------------             --------
//  bool                          DEFAULT/DEC/TEXT            boolean
//  char                          DEFAULT/DEC                 byte
//  char                          TEXT                        string
//  unsigned char                 DEFAULT/DEC                 unsignedByte
//  short                         DEFAULT/DEC                 short
//  short                         TEXT                        string
//  unsigned short                DEFAULT/DEC                 unsignedShort
//  int                           DEFAULT/DEC                 int
//  unsigned int                  DEFAULT/DEC                 unsignedInt
//  bsls::Types::Int64            DEFAULT/DEC                 long
//  bsls::Types::Uint64           DEFAULT/DEC                 unsignedLong
//  float                         DEFAULT                     float
//  float                         DEC                         decimal
//  double                        DEFAULT                     double
//  double                        DEC                         decimal
//  bsl::string                   DEFAULT/TEXT                string
//  bsl::string                   BASE64                      base64Binary
//  bsl::string                   HEX                         hexBinary
//  bdlt::Date                     DEFAULT                     date
//  bdlt::DateTz                   DEFAULT                     date
//  bdlt::Datetime                 DEFAULT                     dateTime
//  bdlt::DatetimeTz               DEFAULT                     dateTime
//  bdlt::Time                     DEFAULT                     time
//  bdlt::TimeTz                   DEFAULT                     time
//  bsl::vector<char>             DEFAULT/BASE64              base64Binary
//  bsl::vector<char>             HEX                         hexBinary
//  bsl::vector<char>             TEXT                        string
//  bsl::vector<short>            TEXT                        string
//..
///Usage
///-----
// We begin by creating abbreviations for formatting modes and by declaring
// objects of a number of types:
//..
//  int main() {
//
//      static const int DEFAULT = bdlat_FormattingMode::DEFAULT;
//      static const int DEC     = bdlat_FormattingMode::DEC;
//      static const int HEX     = bdlat_FormattingMode::HEX;
//      static const int BASE64  = bdlat_FormattingMode::BASE64;
//      static const int TEXT    = bdlat_FormattingMode::TEXT;
//
//      short                    theShort;
//      unsigned                 theUint;
//      float                    theFloat;
//      const char              *theCharPtr;
//      bsl::string              theString;
//
//      bdlt::Date                theDate;
//      bdlt::DatetimeTz          theDatetime;
//      bsl::vector<char>        theCharVector;
//      bsl::vector<bsl::string> theStrVector;
//..
// None of these types are generated types with metadata, so 'className' will
// return a null pointer for each of them:
//..
//      assert(0 == bdlat_TypeName::className(theShort));
//      assert(0 == bdlat_TypeName::className(theUint));
//      assert(0 == bdlat_TypeName::className(theFloat));
//      assert(0 == bdlat_TypeName::className(theCharPtr));
//      assert(0 == bdlat_TypeName::className(theString));
//
//      assert(0 == bdlat_TypeName::className(theDate));
//      assert(0 == bdlat_TypeName::className(theDatetime));
//      assert(0 == bdlat_TypeName::className(theCharVector));
//      assert(0 == bdlat_TypeName::className(theStrVector));
//..
// The 'name' function will never return a null pointer.  For each of the
// fundamental and vocabulary types, it returns the known type name.  For
// vector types, it returns the appropriate "vector<X>" string:
//..
//      assert(0 == bsl::strcmp("short", bdlat_TypeName::name(theShort)));
//      assert(0 == bsl::strcmp("unsigned int",
//                              bdlat_TypeName::name(theUint)));
//      assert(0 == bsl::strcmp("float", bdlat_TypeName::name(theFloat)));
//      assert(0 == bsl::strcmp("const char*",
//                              bdlat_TypeName::name(theCharPtr)));
//
//      assert(0 == bsl::strcmp("string", bdlat_TypeName::name(theString)));
//      assert(0 == bsl::strcmp("bdlt::Date", bdlat_TypeName::name(theDate)));
//      assert(0 == bsl::strcmp("bdlt::DatetimeTz",
//                              bdlat_TypeName::name(theDatetime)));
//      assert(0 == bsl::strcmp("vector<char>",
//                              bdlat_TypeName::name(theCharVector)));
//      assert(0 == bsl::strcmp("vector<string>",
//                              bdlat_TypeName::name(theStrVector)));
//..
// Each of the above types except 'vector<string>' has one or more
// corresponding XSD types.  The XSD type is affected by a formatting mode so
// that, for example, a 'vector<char>' can be represented as a text string
// (formatting mode 'TEXT') or as a sequence of binary bytes (formatting mode
// 'HEX' or 'BASE64').
//..
//      assert(0 == bsl::strcmp("short",
//                              bdlat_TypeName::xsdName(theShort, DEFAULT)));
//      assert(0 == bsl::strcmp("unsignedInt",
//                              bdlat_TypeName::xsdName(theUint, DEFAULT)));
//      assert(0 == bsl::strcmp("float",
//                              bdlat_TypeName::xsdName(theFloat, DEFAULT)));
//      assert(0 == bsl::strcmp("decimal",
//                              bdlat_TypeName::xsdName(theFloat, DEC)));
//      assert(0 == bsl::strcmp("base64Binary",
//                           bdlat_TypeName::xsdName(theCharVector, DEFAULT)));
//      assert(0 == bsl::strcmp("string",
//                              bdlat_TypeName::xsdName(theCharVector, TEXT)));
//..
// For types that have not corresponding XSD type, 'xsdName' returns
// "anyType", regardless of formatting mode:
//..
//      assert(0 == bsl::strcmp("anyType",
//                            bdlat_TypeName::xsdName(theStrVector, DEFAULT)));
//
//      return 0;
//  }
//..
// If we create our own class:
//..
//  namespace MyNamespace {
//
//      class MyClass {
//          //...
//      };
//..
// Then we can assign it a printable name by overloading the
// 'bdlat_TypeName_className' function in the class's namespace:
//..
//      const char *bdlat_TypeName_className(const MyClass&) {
//          return "MyClass";
//      }
//
//  } // Close MyNamespace
//..
// Note that 'bdlat_TypeName_className' must return a string that is
// valid and does not change for remaining duration the program.  The
// overloaded 'bdlat_TypeName_className' function is automatically used for
// 'name' and 'xsdName', as well as for 'className':
//..
//  int main()
//  {
//      static const int DEFAULT = bdlat_FormattingMode::DEFAULT;
//
//      MyNamespace::MyClass myClassObj;
//
//      assert(0 == bsl::strcmp("MyClass",
//                              bdlat_TypeName::className(myClassObj)));
//      assert(0 == bsl::strcmp("MyClass", bdlat_TypeName::name(myClassObj)));
//      assert(0 == bsl::strcmp("MyClass",
//                              bdlat_TypeName::xsdName(myClassObj, DEFAULT)));
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

#ifndef INCLUDED_BDLAT_FORMATTINGMODE
#include <bdlat_formattingmode.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLMF_SWITCH
#include <bslmf_switch.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_TYPEINFO
#include <bsl_typeinfo.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif




namespace BloombergLP {



// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class Date; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Date Date;                    // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class DateTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DateTz DateTz;                // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class Datetime; }                              // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Datetime Datetime;            // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class DatetimeTz; }                            // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DatetimeTz DatetimeTz;        // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class Time; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Time Time;                    // bdet -> bdlt
}  // close namespace bdet

namespace bdlt { class TimeTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::TimeTz TimeTz;                // bdet -> bdlt
}  // close namespace bdet

                          // =====================
                          // struct bdlat_TypeName
                          // =====================

struct bdlat_TypeName {
    // Static template functions for returning a string representation for the
    // name of a type.

    template <class TYPE>
    static const char *className(const TYPE& object);
        // Return a null-terminated string containing the exported name for
        // the specified 'TYPE', or a 0 pointer if 'TYPE' does not export a
        // name.  A type exports a name by overloading the function
        // 'bdlat_TypeName_className(const TYPE&)' in TYPE's namespace.  The
        // default implementation of 'bdlat_TypeName_className' will
        // automatically return the 'CLASS_NAME' value for types that have the
        // 'bdlat_TypeTraitBasicChoice', 'bdlat_TypeTraitBasicSequence',
        // 'bdlat_TypeTraitBasicCustomizedType', or
        // 'bdlat_TypeTraitBasicEnumeration' trait (i.e., types generated
        // using 'bas_codegen.pl').

    template <class TYPE>
    static const char *name(const TYPE& object);
        // Return a null-terminated string containing the name of the
        // specified 'TYPE'.  If 'TYPE' is a fundamental type, string, date,
        // time, or datetime, then return a canonical representation of the
        // type's name.  Otherwise, if 'className(object)' returns a non-null
        // value, then return that value.  Otherwise, return
        // 'typeid(TYPE).name()'.  Note that the returned name refers to the
        // *static* 'TYPE', not to the dynamic type of 'object'.

    template <class TYPE>
    static const char *xsdName(const TYPE& object, int format);
        // Return a null-terminated text string containing the name of the the
        // specified 'TYPE' with the specified 'format' as it would appear in
        // an XML Schema (XSD) element declaration.  The 'format' is
        // interpreted as the bit-wise OR of one or more of the values defined
        // in the 'bdlat_formattingmode' component.  Formatting mode bits
        // outside of 'bdlat_FormattingMode::TYPE_MASK' are ignored.  If
        // 'object' corresponds to one of the XSD built-in types, then return
        // the XSD type's name.  Otherwise, if 'className(object)' returns a
        // non-null value, then return that value.  Otherwise, return
        // "anyType".  The behavior is undefined unless the 'format' is valid
        // for the specified 'TYPE'.
};

namespace bdlat_TypeName_Overloadable {
    // Namespace that provides default implementations.

    template <class TYPE>
    const char *bdlat_TypeName_className(const TYPE& object);
        // Default implementation for 'className'

    template <class TYPE>
    const char *bdlat_TypeName_name(const TYPE& object);
        // Default implementation for 'name'

    template <class TYPE>
    const char *bdlat_TypeName_xsdName(const TYPE& object, int format);
        // Default implementation for 'xsdName'
}  // close namespace bdlat_TypeName_Overloadable

// ---  Anything below this line is implementation specific.  Do not use.  ----

                          // =========================
                          // struct bdlat_TypeName_Imp
                          // =========================

struct bdlat_TypeName_Imp {
    // Private class providing implementation of 'bdlat_TypeName'.

  private:
    // PRIVATE TYPES
    typedef bdlat_FormattingMode FMode;

    struct HasClassName       { };
    struct IsBasicEnumeration { };
    struct Other              { };

    // PRIVATE CLASS METHODS
    template <class TYPE>
    static const char *classNameImp(const TYPE *object, HasClassName);

    template <class TYPE>
    static const char *classNameImp(const TYPE *object, IsBasicEnumeration);

    template <class TYPE>
    static const char *classNameImp(const TYPE *object, Other);

    // PRIVATE CLASS DATA
    static const char BDLAT_NAME_BOOL[];
    static const char BDLAT_NAME_CHAR[];
    static const char BDLAT_NAME_SIGNED_CHAR[];
    static const char BDLAT_NAME_UNSIGNED_CHAR[];
    static const char BDLAT_NAME_SHORT[];
    static const char BDLAT_NAME_UNSIGNED_SHORT[];
    static const char BDLAT_NAME_INT[];
    static const char BDLAT_NAME_UNSIGNED_INT[];
    static const char BDLAT_NAME_LONG[];
    static const char BDLAT_NAME_UNSIGNED_LONG[];
    static const char BDLAT_NAME_INT64[];
    static const char BDLAT_NAME_UINT64[];
    static const char BDLAT_NAME_FLOAT[];
    static const char BDLAT_NAME_DOUBLE[];
    static const char BDLAT_NAME_CONST_CHAR_PTR[];
    static const char BDLAT_NAME_CONST_SIGNED_CHAR_PTR[];
    static const char BDLAT_NAME_CONST_UNSIGNED_CHAR_PTR[];
    static const char BDLAT_NAME_STRING[];
    static const char BDLAT_NAME_DATE[];
    static const char BDLAT_NAME_DATE_TZ[];
    static const char BDLAT_NAME_DATETIME[];
    static const char BDLAT_NAME_DATETIME_TZ[];
    static const char BDLAT_NAME_TIME[];
    static const char BDLAT_NAME_TIME_TZ[];
    static const char BDLAT_NAME_VECTOR_BEGIN[];
    static const char BDLAT_NAME_VECTOR_END[];

    static const char BDLAT_XSDNAME_BOOLEAN[];
    static const char BDLAT_XSDNAME_BYTE[];
    static const char BDLAT_XSDNAME_UNSIGNED_BYTE[];
    static const char BDLAT_XSDNAME_SHORT[];
    static const char BDLAT_XSDNAME_UNSIGNED_SHORT[];
    static const char BDLAT_XSDNAME_INT[];
    static const char BDLAT_XSDNAME_UNSIGNED_INT[];
    static const char BDLAT_XSDNAME_LONG[];
    static const char BDLAT_XSDNAME_UNSIGNED_LONG[];
    static const char BDLAT_XSDNAME_FLOAT[];
    static const char BDLAT_XSDNAME_DOUBLE[];
    static const char BDLAT_XSDNAME_DECIMAL[];
    static const char BDLAT_XSDNAME_STRING[];
    static const char BDLAT_XSDNAME_BASE64_BINARY[];
    static const char BDLAT_XSDNAME_HEX_BINARY[];
    static const char BDLAT_XSDNAME_DATE[];
    static const char BDLAT_XSDNAME_DATETIME[];
    static const char BDLAT_XSDNAME_TIME[];
    static const char BDLAT_XSDNAME_ANY_TYPE[];

  public:
    // CLASS METHODS
    template <class TYPE>
    static const char *className(const TYPE                 *object);
        // Overloads for basic class types.

    template <class TYPE>
    static const char *name(const TYPE                      *object);
        // Generic implementation for non-fundamental types.

    static const char *name(const bool                      *object);
    static const char *name(const char                      *object);
    static const char *name(const signed char               *object);
    static const char *name(const unsigned char             *object);
    static const char *name(const short                     *object);
    static const char *name(const unsigned short            *object);
    static const char *name(const int                       *object);
    static const char *name(const unsigned int              *object);
    static const char *name(const long                      *object);
    static const char *name(const unsigned long             *object);
    static const char *name(const bsls::Types::Int64        *object);
    static const char *name(const bsls::Types::Uint64       *object);
    static const char *name(const float                     *object);
    static const char *name(const double                    *object);
    static const char *name(const char               *const *object);
    static const char *name(const signed char        *const *object);
    static const char *name(const unsigned char      *const *object);
    static const char *name(const bsl::string               *object);
    static const char *name(const bdlt::Date                 *object);
    static const char *name(const bdlt::DateTz               *object);
    static const char *name(const bdlt::Datetime             *object);
    static const char *name(const bdlt::DatetimeTz           *object);
    static const char *name(const bdlt::Time                 *object);
    static const char *name(const bdlt::TimeTz               *object);
        // Overloads for fundamental types and char pointers.

    template <class TYPE>
    static const char *name(const bsl::vector<TYPE>         *object);
        // Specialization for vectors.
        // Return the null-terminated string constructed by replacing the "X"
        // in the string "vector<X>" with the result of calling 'name' on an
        // object of the specified 'TYPE'.  If the constructed string exceeds
        // 100 characters, then truncate to 100 characters.  Note that 'TYPE'
        // may itself be a vector, leading to a recursive call to this
        // function.

    template <class TYPE>
    static const char *xsdName(const TYPE                 *object, int format);
        // Generic implementation for non-fundamental and not predefined types.

    static const char *xsdName(const bool                 *object, int format);
    static const char *xsdName(const char                 *object, int format);
    static const char *xsdName(const signed char          *object, int format);
    static const char *xsdName(const unsigned char        *object, int format);
    static const char *xsdName(const short                *object, int format);
    static const char *xsdName(const unsigned short       *object, int format);
    static const char *xsdName(const int                  *object, int format);
    static const char *xsdName(const unsigned int         *object, int format);
    static const char *xsdName(const long                 *object, int format);
    static const char *xsdName(const unsigned long        *object, int format);
    static const char *xsdName(const bsls::Types::Int64   *object, int format);
    static const char *xsdName(const bsls::Types::Uint64  *object, int format);
    static const char *xsdName(const float                *object, int format);
    static const char *xsdName(const double               *object, int format);
    static const char *xsdName(const bsl::string          *object, int format);
    static const char *xsdName(const char          *const *object, int format);
    static const char *xsdName(const signed char   *const *object, int format);
    static const char *xsdName(const unsigned char *const *object, int format);
    static const char *xsdName(const bdlt::Date            *object, int format);
    static const char *xsdName(const bdlt::DateTz          *object, int format);
    static const char *xsdName(const bdlt::Datetime        *object, int format);
    static const char *xsdName(const bdlt::DatetimeTz      *object, int format);
    static const char *xsdName(const bdlt::Time            *object, int format);
    static const char *xsdName(const bdlt::TimeTz          *object, int format);
    static const char *xsdName(const bsl::vector<char>    *object, int format);
    static const char *xsdName(const bsl::vector<short>   *object, int format);
        // Overloads for fundamental types and some predefined types

    static bool idempotentConcat(char       *dest,
                                 int         destSize,
                                 const char *segments[],
                                 int         numSegments);
        // Concatenate 'numSegments' zero-terminated strings specified by
        // 'segments' array and load the result into 'dest' buffer with size
        // 'destSize'.  Return always true.  Note that this method is
        // idempotent and safe for multi-threaded environment.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // -------------------------
                          // struct bdlat_TypeName_Imp
                          // -------------------------

template <class TYPE>
inline
const char *bdlat_TypeName_Imp::classNameImp(const TYPE *, HasClassName)
{
    return TYPE::CLASS_NAME;
}

template <class TYPE>
inline
const char *bdlat_TypeName_Imp::classNameImp(const TYPE *, IsBasicEnumeration)
{
    typedef typename bdlat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    return Wrapper::CLASS_NAME;
}

template <class TYPE>
inline
const char *bdlat_TypeName_Imp::classNameImp(const TYPE *, Other)
{
    return 0;
}

template <class TYPE>
inline
const char *bdlat_TypeName_Imp::className(const TYPE *object)
{
    enum {
        HAS_CLASS_NAME =
             bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::VALUE
           | bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::VALUE
           | bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicCustomizedType>::VALUE,

        IS_BASIC_ENUMERATION =
            bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicEnumeration>::VALUE,

        SELECTOR = (HAS_CLASS_NAME ? 0 : (IS_BASIC_ENUMERATION ? 1 : 2))
    };

    typedef typename
    bslmf::Switch<SELECTOR,
                  HasClassName,
                  IsBasicEnumeration,
                  Other>::Type        Switch;

    return classNameImp(object, Switch());
}

template <class TYPE>
inline
const char *bdlat_TypeName_Imp::name(const TYPE *object)
{
    const char *cname = bdlat_TypeName::className(*object);
    return cname ? cname : typeid(TYPE).name();
}

inline
const char *bdlat_TypeName_Imp::name(const bool *)
{
    return BDLAT_NAME_BOOL;
}

inline
const char *bdlat_TypeName_Imp::name(const char *)
{
    return BDLAT_NAME_CHAR;
}

inline
const char *bdlat_TypeName_Imp::name(const unsigned char *)
{
    return BDLAT_NAME_UNSIGNED_CHAR;
}

inline
const char *bdlat_TypeName_Imp::name(const signed char *)
{
    return BDLAT_NAME_SIGNED_CHAR;
}

inline
const char *bdlat_TypeName_Imp::name(const short *)
{
    return BDLAT_NAME_SHORT;
}

inline
const char *bdlat_TypeName_Imp::name(const unsigned short *)
{
    return BDLAT_NAME_UNSIGNED_SHORT;
}

inline
const char *bdlat_TypeName_Imp::name(const int *)
{
    return BDLAT_NAME_INT;
}

inline
const char *bdlat_TypeName_Imp::name(const unsigned int *)
{
    return BDLAT_NAME_UNSIGNED_INT;
}

inline
const char *bdlat_TypeName_Imp::name(const long *)
{
    return BDLAT_NAME_LONG;
}

inline
const char *bdlat_TypeName_Imp::name(const unsigned long *)
{
    return BDLAT_NAME_UNSIGNED_LONG;
}

inline
const char *bdlat_TypeName_Imp::name(const bsls::Types::Int64 *)
{
    return BDLAT_NAME_INT64;
}

inline
const char *bdlat_TypeName_Imp::name(const bsls::Types::Uint64 *)
{
    return BDLAT_NAME_UINT64;
}

inline
const char *bdlat_TypeName_Imp::name(const float *)
{
    return BDLAT_NAME_FLOAT;
}

inline
const char *bdlat_TypeName_Imp::name(const double *)
{
    return BDLAT_NAME_DOUBLE;
}

inline
const char *bdlat_TypeName_Imp::name(const char *const *)
{
    return BDLAT_NAME_CONST_CHAR_PTR;
}

inline
const char *bdlat_TypeName_Imp::name(const signed char *const *)
{
    return BDLAT_NAME_CONST_SIGNED_CHAR_PTR;
}

inline
const char *bdlat_TypeName_Imp::name(const unsigned char *const *)
{
    return BDLAT_NAME_CONST_UNSIGNED_CHAR_PTR;
}

inline
const char *bdlat_TypeName_Imp::name(const bsl::string *)
{
    return BDLAT_NAME_STRING;
}

inline
const char *bdlat_TypeName_Imp::name(const bdlt::Date *)
{
    return BDLAT_NAME_DATE;
}

inline
const char *bdlat_TypeName_Imp::name(const bdlt::DateTz *)
{
    return BDLAT_NAME_DATE_TZ;
}

inline
const char *bdlat_TypeName_Imp::name(const bdlt::Datetime *)
{
    return BDLAT_NAME_DATETIME;
}

inline
const char *bdlat_TypeName_Imp::name(const bdlt::DatetimeTz *)
{
    return BDLAT_NAME_DATETIME_TZ;
}

inline
const char *bdlat_TypeName_Imp::name(const bdlt::Time *)
{
    return BDLAT_NAME_TIME;
}

inline
const char *bdlat_TypeName_Imp::name(const bdlt::TimeTz *)
{
    return BDLAT_NAME_TIME_TZ;
}

template <class TYPE>
const char *bdlat_TypeName_Imp::name(const bsl::vector<TYPE> *)
{
    static const int MAX_LEN = 100;
    static char name[MAX_LEN + 1];
    static bool initialized = false;

    if (! initialized) {
        // This is thread-safe because even if two threads execute this code
        // simultaneously, the same values will be written on top of each
        // other (i.e., the operations are idempotent).

        const char *segments[3] = {
            (const char*)BDLAT_NAME_VECTOR_BEGIN,
            bdlat_TypeName::name(*(TYPE*)0),
            (const char*)BDLAT_NAME_VECTOR_END,
        };

        initialized = bdlat_TypeName_Imp::idempotentConcat(name,
                                                           MAX_LEN + 1,
                                                           segments,
                                                           3);
    }

    return name;
}

template <class TYPE>
inline
const char *bdlat_TypeName_Imp::xsdName(const TYPE *object, int)
{
    const char *cname = bdlat_TypeName::className(*object);
    return cname ? cname : (const char *)BDLAT_XSDNAME_ANY_TYPE;
}

inline
const char *bdlat_TypeName_Imp::xsdName(const bool *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK)
                 || FMode::BDLAT_DEC     == (format & FMode::BDLAT_TYPE_MASK)
                 || FMode::BDLAT_TEXT    == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_BOOLEAN;
}

inline
const char *bdlat_TypeName_Imp::xsdName(const char *, int format)
{
    return xsdName((const signed char*)0, format);
}

inline
const char*
bdlat_TypeName_Imp::xsdName(const unsigned short *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK)
                 || FMode::BDLAT_DEC     == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_UNSIGNED_SHORT;
}

inline
const char *bdlat_TypeName_Imp::xsdName(const int *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK)
                 || FMode::BDLAT_DEC     == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_INT;
}

inline
const char *bdlat_TypeName_Imp::xsdName(const unsigned int *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK)
                 || FMode::BDLAT_DEC     == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_UNSIGNED_INT;
}

inline
const char *bdlat_TypeName_Imp::xsdName(const long *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK)
                 || FMode::BDLAT_DEC     == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_INT;
}

inline
const char*
bdlat_TypeName_Imp::xsdName(const unsigned long *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK)
                 || FMode::BDLAT_DEC     == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_UNSIGNED_INT;
}

inline
const char*
bdlat_TypeName_Imp::xsdName(const bsls::Types::Int64 *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK)
                 || FMode::BDLAT_DEC     == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_LONG;
}

inline
const char*
bdlat_TypeName_Imp::xsdName(const bsls::Types::Uint64 *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK)
                 || FMode::BDLAT_DEC     == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_UNSIGNED_LONG;
}

inline
const char*
bdlat_TypeName_Imp::xsdName(const char *const *, int format)
{
    return xsdName((const bsl::string*) 0, format);
}

inline
const char*
bdlat_TypeName_Imp::xsdName(const signed char *const *, int format)
{
    return xsdName((const bsl::string*) 0, format);
}

inline
const char*
bdlat_TypeName_Imp::xsdName(const unsigned char *const *, int format)
{
    return xsdName((const bsl::string*) 0, format);
}

inline
const char *bdlat_TypeName_Imp::xsdName(const bdlt::Date *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_DATE;
}

inline
const char *bdlat_TypeName_Imp::xsdName(const bdlt::DateTz *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_DATE;
}

inline
const char*
bdlat_TypeName_Imp::xsdName(const bdlt::Datetime *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDLAT_DEFAULT == (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_DATETIME;
}

inline
const char*
bdlat_TypeName_Imp::xsdName(const bdlt::DatetimeTz *, int format)
{
    BSLS_ASSERT_SAFE(FMode::BDLAT_DEFAULT ==
                                            (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_DATETIME;
}

inline
const char *bdlat_TypeName_Imp::xsdName(const bdlt::Time *, int format)
{
    BSLS_ASSERT_SAFE(FMode::BDLAT_DEFAULT ==
                                            (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_TIME;
}

inline
const char *bdlat_TypeName_Imp::xsdName(const bdlt::TimeTz *, int format)
{
    BSLS_ASSERT_SAFE(FMode::BDLAT_DEFAULT ==
                                            (format & FMode::BDLAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDLAT_XSDNAME_TIME;
}

                          // -------------------------------------
                          // namespace bdlat_TypeName_Overloadable
                          // -------------------------------------

template <class TYPE>
inline
const char *
bdlat_TypeName_Overloadable::bdlat_TypeName_className(const TYPE& object)
{
    // Indirection prevents conversion to 'TYPE':
    return bdlat_TypeName_Imp::className(&object);
}

template <class TYPE>
inline
const char *
bdlat_TypeName_Overloadable::bdlat_TypeName_name(const TYPE& object)
{
    // Indirection prevents conversion to 'TYPE':
    return bdlat_TypeName_Imp::name(&object);
}

template <class TYPE>
inline
const char *
bdlat_TypeName_Overloadable::bdlat_TypeName_xsdName(const TYPE& object,
                                                    int         format)
{
    // Indirection prevents conversion to 'TYPE':

    return bdlat_TypeName_Imp::xsdName(&object, format);
}

                          // --------------------
                          // class bdlat_TypeName
                          // --------------------

template <class TYPE>
inline
const char *bdlat_TypeName::className(const TYPE& object)
{
    using namespace bdlat_TypeName_Overloadable;

    // Select function using Koenig lookup:

    return bdlat_TypeName_className(object);
}

template <class TYPE>
inline
const char *bdlat_TypeName::name(const TYPE& object)
{
    using namespace bdlat_TypeName_Overloadable;

    // Select function using Koenig lookup:

    return bdlat_TypeName_name(object);
}

template <class TYPE>
inline
const char *bdlat_TypeName::xsdName(const TYPE& object, int format)
{
    using namespace bdlat_TypeName_Overloadable;

    // Select function using Koenig lookup:

    return bdlat_TypeName_xsdName(object, format);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
