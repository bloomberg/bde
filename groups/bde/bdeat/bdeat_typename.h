// bdeat_typename.h          -*-C++-*-
#ifndef INCLUDED_BDEAT_TYPENAME
#define INCLUDED_BDEAT_TYPENAME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide string representations for data type names.
//
//@CLASSES:
//  bdeat_TypeName: namespace for type-name functions
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@SEE_ALSO: http://www.w3.org/TR/xmlschema-2/#built-in-datatypes
//
//@DESCRIPTION: This component defines a structure 'bdeat_TypeName' which
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
//  'bdeat_TypeTraitBasicChoice'
//  'bdeat_TypeTraitBasicSequence'
//  'bdeat_TypeTraitBasicCustomizedType'
//  'bdeat_TypeTraitBasicEnumeration'
//..
// If metadata is not available for the object type, the function 'className'
// returns 0 unless the function 'bdeat_TypeName_className' is overloaded by
// developer.
//
///Overloable Class Name Functions For User Defined Classes
///--------------------------------------------------------
// To provide the custom name for the given user-defined C++ class, the
// developer should overload the template function 'bdeat_TypeName_className'
// for this type in the namespace where the type is defined.
//
//WARNING! Do not extend 'bdeat_TypeName_Overloadable' namespace.
//
///Generic Type Name Information
///-----------------------------
// The template functions 'name' returns the generic type name for the given
// object.  The generic type name is one of the following:
//..
//  o predefined name for fundamental types
//  o class name from 'bdeat_TypeName_className', if such function returns a
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
//   o class name from 'bdeat_TypeName_className', if such function returns
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
//  bsls_PlatformUtil::Int64      DEFAULT/DEC                 long
//  bsls_PlatformUtil::Uint64     DEFAULT/DEC                 unsignedLong
//  float                         DEFAULT                     float
//  float                         DEC                         decimal
//  double                        DEFAULT                     double
//  double                        DEC                         decimal
//  bsl::string                   DEFAULT/TEXT                string
//  bsl::string                   BASE64                      base64Binary
//  bsl::string                   HEX                         hexBinary
//  bdet_Date                     DEFAULT                     date
//  bdet_DateTz                   DEFAULT                     date
//  bdet_Datetime                 DEFAULT                     dateTime
//  bdet_DatetimeTz               DEFAULT                     dateTime
//  bdet_Time                     DEFAULT                     time
//  bdet_TimeTz                   DEFAULT                     time
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
//      static const int DEFAULT = bdeat_FormattingMode::DEFAULT;
//      static const int DEC     = bdeat_FormattingMode::DEC;
//      static const int HEX     = bdeat_FormattingMode::HEX;
//      static const int BASE64  = bdeat_FormattingMode::BASE64;
//      static const int TEXT    = bdeat_FormattingMode::TEXT;
//
//      short                    theShort;
//      unsigned                 theUint;
//      float                    theFloat;
//      const char              *theCharPtr;
//      bsl::string              theString;
//
//      bdet_Date                theDate;
//      bdet_DatetimeTz          theDatetime;
//      bsl::vector<char>        theCharVector;
//      bsl::vector<bsl::string> theStrVector;
//..
// None of these types are generated types with metadata, so 'className' will
// return a null pointer for each of them:
//..
//      assert(0 == bdeat_TypeName::className(theShort));
//      assert(0 == bdeat_TypeName::className(theUint));
//      assert(0 == bdeat_TypeName::className(theFloat));
//      assert(0 == bdeat_TypeName::className(theCharPtr));
//      assert(0 == bdeat_TypeName::className(theString));
//
//      assert(0 == bdeat_TypeName::className(theDate));
//      assert(0 == bdeat_TypeName::className(theDatetime));
//      assert(0 == bdeat_TypeName::className(theCharVector));
//      assert(0 == bdeat_TypeName::className(theStrVector));
//..
// The 'name' function will never return a null pointer.  For each of the
// fundamental and vocabulary types, it returns the known type name.  For
// vector types, it returns the appropriate "vector<X>" string:
//..
//      assert(0 == bsl::strcmp("short", bdeat_TypeName::name(theShort)));
//      assert(0 == bsl::strcmp("unsigned int",
//                              bdeat_TypeName::name(theUint)));
//      assert(0 == bsl::strcmp("float", bdeat_TypeName::name(theFloat)));
//      assert(0 == bsl::strcmp("const char*",
//                              bdeat_TypeName::name(theCharPtr)));
//
//      assert(0 == bsl::strcmp("string", bdeat_TypeName::name(theString)));
//      assert(0 == bsl::strcmp("bdet_Date", bdeat_TypeName::name(theDate)));
//      assert(0 == bsl::strcmp("bdet_DatetimeTz",
//                              bdeat_TypeName::name(theDatetime)));
//      assert(0 == bsl::strcmp("vector<char>",
//                              bdeat_TypeName::name(theCharVector)));
//      assert(0 == bsl::strcmp("vector<string>",
//                              bdeat_TypeName::name(theStrVector)));
//..
// Each of the above types except 'vector<string>' has one or more
// corresponding XSD types.  The XSD type is affected by a formatting mode so
// that, for example, a 'vector<char>' can be represented as a text string
// (formatting mode 'TEXT') or as a sequence of binary bytes (formatting mode
// 'HEX' or 'BASE64').
//..
//      assert(0 == bsl::strcmp("short",
//                              bdeat_TypeName::xsdName(theShort, DEFAULT)));
//      assert(0 == bsl::strcmp("unsignedInt",
//                              bdeat_TypeName::xsdName(theUint, DEFAULT)));
//      assert(0 == bsl::strcmp("float",
//                              bdeat_TypeName::xsdName(theFloat, DEFAULT)));
//      assert(0 == bsl::strcmp("decimal",
//                              bdeat_TypeName::xsdName(theFloat, DEC)));
//      assert(0 == bsl::strcmp("base64Binary",
//                           bdeat_TypeName::xsdName(theCharVector, DEFAULT)));
//      assert(0 == bsl::strcmp("string",
//                              bdeat_TypeName::xsdName(theCharVector, TEXT)));
//..
// For types that have not corresponding XSD type, 'xsdName' returns
// "anyType", regardless of formatting mode:
//..
//      assert(0 == bsl::strcmp("anyType",
//                            bdeat_TypeName::xsdName(theStrVector, DEFAULT)));
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
// 'bdeat_TypeName_className' function in the class's namespace:
//..
//      const char *bdeat_TypeName_className(const MyClass&) {
//          return "MyClass";
//      }
//
//  } // Close MyNamespace
//..
// Note that 'bdeat_TypeName_className' must return a string that is
// valid and does not change for remaining duration the program.  The
// overloaded 'bdeat_TypeName_className' function is automatically used for
// 'name' and 'xsdName', as well as for 'className':
//..
//  int main()
//  {
//      static const int DEFAULT = bdeat_FormattingMode::DEFAULT;
//
//      MyNamespace::MyClass myClassObj;
//
//      assert(0 == bsl::strcmp("MyClass",
//                              bdeat_TypeName::className(myClassObj)));
//      assert(0 == bsl::strcmp("MyClass", bdeat_TypeName::name(myClassObj)));
//      assert(0 == bsl::strcmp("MyClass",
//                              bdeat_TypeName::xsdName(myClassObj, DEFAULT)));
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#include <bdeat_formattingmode.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
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

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
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

class bdet_Date;
class bdet_DateTz;
class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_Time;
class bdet_TimeTz;

                          // =====================
                          // struct bdeat_TypeName
                          // =====================

struct bdeat_TypeName {
    // Static template functions for returning a string representation for the
    // name of a type.

    template <typename TYPE>
    static const char *className(const TYPE& object);
        // Return a null-terminated string containing the exported name for
        // the specified 'TYPE', or a 0 pointer if 'TYPE' does not export a
        // name.  A type exports a name by overloading the function
        // 'bdeat_TypeName_className(const TYPE&)' in TYPE's namespace.  The
        // default implementation of 'bdeat_TypeName_className' will
        // automatically return the 'CLASS_NAME' value for types that have the
        // 'bdeat_TypeTraitBasicChoice', 'bdeat_TypeTraitBasicSequence',
        // 'bdeat_TypeTraitBasicCustomizedType', or
        // 'bdeat_TypeTraitBasicEnumeration' trait (i.e., types generated
        // using 'bas_codegen.pl').

    template <typename TYPE>
    static const char *name(const TYPE& object);
        // Return a null-terminated string containing the name of the
        // specified 'TYPE'.  If 'TYPE' is a fundamental type, string, date,
        // time, or datetime, then return a canonical representation of the
        // type's name.  Otherwise, if 'className(object)' returns a non-null
        // value, then return that value.  Otherwise, return
        // 'typeid(TYPE).name()'.  Note that the returned name refers to the
        // *static* 'TYPE', not to the dynamic type of 'object'.

    template <typename TYPE>
    static const char *xsdName(const TYPE& object, int format);
        // Return a null-terminated text string containing the name of the the
        // specified 'TYPE' with the specified 'format' as it would appear in
        // an XML Schema (XSD) element declaration.  The 'format' is
        // interpreted as the bit-wise OR of one or more of the values defined
        // in the 'bdeat_formattingmode' component.  Formatting mode bits
        // outside of 'bdeat_FormattingMode::TYPE_MASK' are ignored.  If
        // 'object' corresponds to one of the XSD built-in types, then return
        // the XSD type's name.  Otherwise, if 'className(object)' returns a
        // non-null value, then return that value.  Otherwise, return
        // "anyType".  The behavior is undefined unless the 'format' is valid
        // for the specified 'TYPE'.
};

namespace bdeat_TypeName_Overloadable {
    // Namespace that provides default implementations.

    template <typename TYPE>
    const char *bdeat_TypeName_className(const TYPE& object);
        // Default implementation for 'className'

    template <typename TYPE>
    const char *bdeat_TypeName_name(const TYPE& object);
        // Default implementation for 'name'

    template <typename TYPE>
    const char *bdeat_TypeName_xsdName(const TYPE& object, int format);
        // Default implementation for 'xsdName'
}

// ---  Anything below this line is implementation specific.  Do not use.  ----

                          // =========================
                          // struct bdeat_TypeName_Imp
                          // =========================

struct bdeat_TypeName_Imp {
    // Private class providing implementation of 'bdeat_TypeName'.

  private:
    // PRIVATE TYPES
    typedef bdeat_FormattingMode FMode;

    struct HasClassName       { };
    struct IsBasicEnumeration { };
    struct Other              { };

    // PRIVATE CLASS METHODS
    template <typename TYPE>
    static const char *classNameImp(const TYPE *object, HasClassName);

    template <typename TYPE>
    static const char *classNameImp(const TYPE *object, IsBasicEnumeration);

    template <typename TYPE>
    static const char *classNameImp(const TYPE *object, Other);

    // PRIVATE CLASS DATA
    static const char BDEAT_NAME_BOOL[];
    static const char BDEAT_NAME_CHAR[];
    static const char BDEAT_NAME_SIGNED_CHAR[];
    static const char BDEAT_NAME_UNSIGNED_CHAR[];
    static const char BDEAT_NAME_SHORT[];
    static const char BDEAT_NAME_UNSIGNED_SHORT[];
    static const char BDEAT_NAME_INT[];
    static const char BDEAT_NAME_UNSIGNED_INT[];
    static const char BDEAT_NAME_LONG[];
    static const char BDEAT_NAME_UNSIGNED_LONG[];
    static const char BDEAT_NAME_INT64[];
    static const char BDEAT_NAME_UINT64[];
    static const char BDEAT_NAME_FLOAT[];
    static const char BDEAT_NAME_DOUBLE[];
    static const char BDEAT_NAME_CONST_CHAR_PTR[];
    static const char BDEAT_NAME_CONST_SIGNED_CHAR_PTR[];
    static const char BDEAT_NAME_CONST_UNSIGNED_CHAR_PTR[];
    static const char BDEAT_NAME_STRING[];
    static const char BDEAT_NAME_DATE[];
    static const char BDEAT_NAME_DATE_TZ[];
    static const char BDEAT_NAME_DATETIME[];
    static const char BDEAT_NAME_DATETIME_TZ[];
    static const char BDEAT_NAME_TIME[];
    static const char BDEAT_NAME_TIME_TZ[];
    static const char BDEAT_NAME_VECTOR_BEGIN[];
    static const char BDEAT_NAME_VECTOR_END[];

    static const char BDEAT_XSDNAME_BOOLEAN[];
    static const char BDEAT_XSDNAME_BYTE[];
    static const char BDEAT_XSDNAME_UNSIGNED_BYTE[];
    static const char BDEAT_XSDNAME_SHORT[];
    static const char BDEAT_XSDNAME_UNSIGNED_SHORT[];
    static const char BDEAT_XSDNAME_INT[];
    static const char BDEAT_XSDNAME_UNSIGNED_INT[];
    static const char BDEAT_XSDNAME_LONG[];
    static const char BDEAT_XSDNAME_UNSIGNED_LONG[];
    static const char BDEAT_XSDNAME_FLOAT[];
    static const char BDEAT_XSDNAME_DOUBLE[];
    static const char BDEAT_XSDNAME_DECIMAL[];
    static const char BDEAT_XSDNAME_STRING[];
    static const char BDEAT_XSDNAME_BASE64_BINARY[];
    static const char BDEAT_XSDNAME_HEX_BINARY[];
    static const char BDEAT_XSDNAME_DATE[];
    static const char BDEAT_XSDNAME_DATETIME[];
    static const char BDEAT_XSDNAME_TIME[];
    static const char BDEAT_XSDNAME_ANY_TYPE[];

  public:
    // CLASS METHODS
    template <typename TYPE>
    static const char *className(const TYPE                 *object);
        // Overloads for basic class types.

    template <typename TYPE>
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
    static const char *name(const bsls_PlatformUtil::Int64  *object);
    static const char *name(const bsls_PlatformUtil::Uint64 *object);
    static const char *name(const float                     *object);
    static const char *name(const double                    *object);
    static const char *name(const char               *const *object);
    static const char *name(const signed char        *const *object);
    static const char *name(const unsigned char      *const *object);
    static const char *name(const bsl::string               *object);
    static const char *name(const bdet_Date                 *object);
    static const char *name(const bdet_DateTz               *object);
    static const char *name(const bdet_Datetime             *object);
    static const char *name(const bdet_DatetimeTz           *object);
    static const char *name(const bdet_Time                 *object);
    static const char *name(const bdet_TimeTz               *object);
        // Overloads for fundamental types and char pointers.

    template <typename TYPE>
    static const char *name(const bsl::vector<TYPE>         *object);
        // Specialization for vectors.
        // Return the null-terminated string constructed by replacing the "X"
        // in the string "vector<X>" with the result of calling 'name' on an
        // object of the specified 'TYPE'.  If the constructed string exceeds
        // 100 characters, then truncate to 100 characters.  Note that 'TYPE'
        // may itself be a vector, leading to a recursive call to this
        // function.

    template <typename TYPE>
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
    static const char *xsdName(const bsls_PlatformUtil::Int64
                                                          *object, int format);
    static const char *xsdName(const bsls_PlatformUtil::Uint64
                                                          *object, int format);
    static const char *xsdName(const float                *object, int format);
    static const char *xsdName(const double               *object, int format);
    static const char *xsdName(const bsl::string          *object, int format);
    static const char *xsdName(const char          *const *object, int format);
    static const char *xsdName(const signed char   *const *object, int format);
    static const char *xsdName(const unsigned char *const *object, int format);
    static const char *xsdName(const bdet_Date            *object, int format);
    static const char *xsdName(const bdet_DateTz          *object, int format);
    static const char *xsdName(const bdet_Datetime        *object, int format);
    static const char *xsdName(const bdet_DatetimeTz      *object, int format);
    static const char *xsdName(const bdet_Time            *object, int format);
    static const char *xsdName(const bdet_TimeTz          *object, int format);
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
                          // struct bdeat_TypeName_Imp
                          // -------------------------

template <typename TYPE>
inline
const char *bdeat_TypeName_Imp::classNameImp(const TYPE *, HasClassName)
{
    return TYPE::CLASS_NAME;
}

template <typename TYPE>
inline
const char *bdeat_TypeName_Imp::classNameImp(const TYPE *, IsBasicEnumeration)
{
    typedef typename bdeat_BasicEnumerationWrapper<TYPE>::Wrapper Wrapper;
    return Wrapper::CLASS_NAME;
}

template <typename TYPE>
inline
const char *bdeat_TypeName_Imp::classNameImp(const TYPE *, Other)
{
    return 0;
}

template <typename TYPE>
inline
const char *bdeat_TypeName_Imp::className(const TYPE *object)
{
    enum {
        HAS_CLASS_NAME =
              bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicChoice>::VALUE
            | bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicSequence>::VALUE
            | bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicCustomizedType>::VALUE,

        IS_BASIC_ENUMERATION =
            bslalg_HasTrait<TYPE, bdeat_TypeTraitBasicEnumeration>::VALUE,

        SELECTOR = (HAS_CLASS_NAME ? 0 : (IS_BASIC_ENUMERATION ? 1 : 2))
    };

    typedef typename
    bslmf_Switch<SELECTOR,
                 HasClassName,
                 IsBasicEnumeration,
                 Other>::Type        Switch;

    return classNameImp(object, Switch());
}

template <typename TYPE>
inline
const char *bdeat_TypeName_Imp::name(const TYPE *object)
{
    const char *cname = bdeat_TypeName::className(*object);
    return cname ? cname : typeid(TYPE).name();
}

inline
const char *bdeat_TypeName_Imp::name(const bool *)
{
    return BDEAT_NAME_BOOL;
}

inline
const char *bdeat_TypeName_Imp::name(const char *)
{
    return BDEAT_NAME_CHAR;
}

inline
const char *bdeat_TypeName_Imp::name(const unsigned char *)
{
    return BDEAT_NAME_UNSIGNED_CHAR;
}

inline
const char *bdeat_TypeName_Imp::name(const signed char *)
{
    return BDEAT_NAME_SIGNED_CHAR;
}

inline
const char *bdeat_TypeName_Imp::name(const short *)
{
    return BDEAT_NAME_SHORT;
}

inline
const char *bdeat_TypeName_Imp::name(const unsigned short *)
{
    return BDEAT_NAME_UNSIGNED_SHORT;
}

inline
const char *bdeat_TypeName_Imp::name(const int *)
{
    return BDEAT_NAME_INT;
}

inline
const char *bdeat_TypeName_Imp::name(const unsigned int *)
{
    return BDEAT_NAME_UNSIGNED_INT;
}

inline
const char *bdeat_TypeName_Imp::name(const long *)
{
    return BDEAT_NAME_LONG;
}

inline
const char *bdeat_TypeName_Imp::name(const unsigned long *)
{
    return BDEAT_NAME_UNSIGNED_LONG;
}

inline
const char *bdeat_TypeName_Imp::name(const bsls_PlatformUtil::Int64  *)
{
    return BDEAT_NAME_INT64;
}

inline
const char *bdeat_TypeName_Imp::name(const bsls_PlatformUtil::Uint64 *)
{
    return BDEAT_NAME_UINT64;
}

inline
const char *bdeat_TypeName_Imp::name(const float *)
{
    return BDEAT_NAME_FLOAT;
}

inline
const char *bdeat_TypeName_Imp::name(const double *)
{
    return BDEAT_NAME_DOUBLE;
}

inline
const char *bdeat_TypeName_Imp::name(const char *const *)
{
    return BDEAT_NAME_CONST_CHAR_PTR;
}

inline
const char *bdeat_TypeName_Imp::name(const signed char *const *)
{
    return BDEAT_NAME_CONST_SIGNED_CHAR_PTR;
}

inline
const char *bdeat_TypeName_Imp::name(const unsigned char *const *)
{
    return BDEAT_NAME_CONST_UNSIGNED_CHAR_PTR;
}

inline
const char *bdeat_TypeName_Imp::name(const bsl::string *)
{
    return BDEAT_NAME_STRING;
}

inline
const char *bdeat_TypeName_Imp::name(const bdet_Date *)
{
    return BDEAT_NAME_DATE;
}

inline
const char *bdeat_TypeName_Imp::name(const bdet_DateTz *)
{
    return BDEAT_NAME_DATE_TZ;
}

inline
const char *bdeat_TypeName_Imp::name(const bdet_Datetime *)
{
    return BDEAT_NAME_DATETIME;
}

inline
const char *bdeat_TypeName_Imp::name(const bdet_DatetimeTz *)
{
    return BDEAT_NAME_DATETIME_TZ;
}

inline
const char *bdeat_TypeName_Imp::name(const bdet_Time *)
{
    return BDEAT_NAME_TIME;
}

inline
const char *bdeat_TypeName_Imp::name(const bdet_TimeTz *)
{
    return BDEAT_NAME_TIME_TZ;
}

template <typename TYPE>
const char *bdeat_TypeName_Imp::name(const bsl::vector<TYPE> *)
{
    static const int MAX_LEN = 100;
    static char name[MAX_LEN + 1];
    static bool initialized = false;

    if (! initialized) {
        // This is thread-safe because even if two threads execute this code
        // simultaneously, the same values will be written on top of each
        // other (i.e., the operations are idempotent).

        const char *segments[3] = {
            (const char*)BDEAT_NAME_VECTOR_BEGIN,
            bdeat_TypeName::name(*(TYPE*)0),
            (const char*)BDEAT_NAME_VECTOR_END,
        };

        initialized = bdeat_TypeName_Imp::idempotentConcat(name,
                                                           MAX_LEN + 1,
                                                           segments,
                                                           3);
    }

    return name;
}

template <typename TYPE>
inline
const char *bdeat_TypeName_Imp::xsdName(const TYPE *object, int)
{
    const char *cname = bdeat_TypeName::className(*object);
    return cname ? cname : (const char *)BDEAT_XSDNAME_ANY_TYPE;
}

inline
const char *bdeat_TypeName_Imp::xsdName(const bool *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK)
                 || FMode::BDEAT_DEC     == (format & FMode::BDEAT_TYPE_MASK)
                 || FMode::BDEAT_TEXT    == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_BOOLEAN;
}

inline
const char *bdeat_TypeName_Imp::xsdName(const char *, int format)
{
    return xsdName((const signed char*)0, format);
}

inline
const char*
bdeat_TypeName_Imp::xsdName(const unsigned short *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK)
                 || FMode::BDEAT_DEC     == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_UNSIGNED_SHORT;
}

inline
const char *bdeat_TypeName_Imp::xsdName(const int *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK)
                 || FMode::BDEAT_DEC     == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_INT;
}

inline
const char *bdeat_TypeName_Imp::xsdName(const unsigned int *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK)
                 || FMode::BDEAT_DEC     == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_UNSIGNED_INT;
}

inline
const char *bdeat_TypeName_Imp::xsdName(const long *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK)
                 || FMode::BDEAT_DEC     == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_INT;
}

inline
const char*
bdeat_TypeName_Imp::xsdName(const unsigned long *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK)
                 || FMode::BDEAT_DEC     == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_UNSIGNED_INT;
}

inline
const char*
bdeat_TypeName_Imp::xsdName(const bsls_PlatformUtil::Int64 *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK)
                 || FMode::BDEAT_DEC     == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_LONG;
}

inline
const char*
bdeat_TypeName_Imp::xsdName(const bsls_PlatformUtil::Uint64 *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK)
                 || FMode::BDEAT_DEC     == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_UNSIGNED_LONG;
}

inline
const char*
bdeat_TypeName_Imp::xsdName(const char *const *, int format)
{
    return xsdName((const bsl::string*) 0, format);
}

inline
const char*
bdeat_TypeName_Imp::xsdName(const signed char *const *, int format)
{
    return xsdName((const bsl::string*) 0, format);
}

inline
const char*
bdeat_TypeName_Imp::xsdName(const unsigned char *const *, int format)
{
    return xsdName((const bsl::string*) 0, format);
}

inline
const char *bdeat_TypeName_Imp::xsdName(const bdet_Date *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_DATE;
}

inline
const char *bdeat_TypeName_Imp::xsdName(const bdet_DateTz *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_DATE;
}

inline
const char*
bdeat_TypeName_Imp::xsdName(const bdet_Datetime *, int format)
{
    BSLS_ASSERT_SAFE(
                    FMode::BDEAT_DEFAULT == (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_DATETIME;
}

inline
const char*
bdeat_TypeName_Imp::xsdName(const bdet_DatetimeTz *, int format)
{
    BSLS_ASSERT_SAFE(FMode::BDEAT_DEFAULT ==
                                            (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_DATETIME;
}

inline
const char *bdeat_TypeName_Imp::xsdName(const bdet_Time *, int format)
{
    BSLS_ASSERT_SAFE(FMode::BDEAT_DEFAULT ==
                                            (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_TIME;
}

inline
const char *bdeat_TypeName_Imp::xsdName(const bdet_TimeTz *, int format)
{
    BSLS_ASSERT_SAFE(FMode::BDEAT_DEFAULT ==
                                            (format & FMode::BDEAT_TYPE_MASK));

    (void)format;  // suppress warning if assert is disabled
    return BDEAT_XSDNAME_TIME;
}

                          // -------------------------------------
                          // namespace bdeat_TypeName_Overloadable
                          // -------------------------------------

template <typename TYPE>
inline
const char *
bdeat_TypeName_Overloadable::bdeat_TypeName_className(const TYPE& object)
{
    // Indirection prevents conversion to 'TYPE':
    return bdeat_TypeName_Imp::className(&object);
}

template <typename TYPE>
inline
const char *
bdeat_TypeName_Overloadable::bdeat_TypeName_name(const TYPE& object)
{
    // Indirection prevents conversion to 'TYPE':
    return bdeat_TypeName_Imp::name(&object);
}

template <typename TYPE>
inline
const char *
bdeat_TypeName_Overloadable::bdeat_TypeName_xsdName(const TYPE& object,
                                                    int         format)
{
    // Indirection prevents conversion to 'TYPE':

    return bdeat_TypeName_Imp::xsdName(&object, format);
}

                          // --------------------
                          // class bdeat_TypeName
                          // --------------------

template <typename TYPE>
inline
const char *bdeat_TypeName::className(const TYPE& object)
{
    using namespace bdeat_TypeName_Overloadable;

    // Select function using Koenig lookup:

    return bdeat_TypeName_className(object);
}

template <typename TYPE>
inline
const char *bdeat_TypeName::name(const TYPE& object)
{
    using namespace bdeat_TypeName_Overloadable;

    // Select function using Koenig lookup:

    return bdeat_TypeName_name(object);
}

template <typename TYPE>
inline
const char *bdeat_TypeName::xsdName(const TYPE& object, int format)
{
    using namespace bdeat_TypeName_Overloadable;

    // Select function using Koenig lookup:

    return bdeat_TypeName_xsdName(object, format);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
