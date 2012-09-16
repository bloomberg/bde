// bdeu_printmethods.h                                                -*-C++-*-
#ifndef INCLUDED_BDEU_PRINTMETHODS
#define INCLUDED_BDEU_PRINTMETHODS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide methods for uniform printing of value-semantic types.
//
//@CLASSES:
//  bdeu_PrintMethods: templates for uniform printing of value-semantic types
//  bdeu_HasPrintMethod: trait indicating existence of 'print' method
//  bdeu_TypeTraitHasPrintMethod: Old-style version of 'bdeu_HasPrintMethod'
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bslalg_typetraits
//
//@DESCRIPTION: This component provides a namespace for print utilities
// that support uniform 'ostream' printing across all printable types,
// including template types and containers.  The 'bdeu_PrintMethods' namespace
// enables clients to output the value of any printable object according to the
// standard BDE 'print' protocol.  Using the 'bdeu_PrintMethods::print' method,
// it is convenient to print objects of unknown types, with an optionally-
// specified indentation level and also an optionally-specified number of
// spaces per indentation level.
//
///Traits Affecting Printing
///-------------------------
// By default, 'bdeu_PrintMethods::print' uses the '<<' stream output operator
// to print a value.  This formats the entire output on one line, suppressing
// all indentation.  A class can override this behavior by declaring certain
// traits related to printing.  This component detects these traits and invokes
// an appropriate print operation.  The following lists the traits recognized
// by this component:
//..
//  bdeu_TypeTraitHasPrintMethod       ( highest precedence )
//  bslalg_TypeTraitHasStlIterators
//  bslalg_TypeTraitPair               ( lowest precedence  )
//..
// Since a class may declare multiple traits (see the component-level
// documentation of 'bslalg_typetraits' for information about declaring
// traits), the relative precedence of the traits is shown above.  The next
// sub-sections describe these traits and their affects on printing.
//
///Affect of 'bdeu_TypeTraitHasPrintMethod' Trait
/// - - - - - - - - - - - - - - - - - - - - - - -
// If a class 'X' declares the 'bdeu_TypeTraitHasPrintMethod' trait, then it
// must provide a 'print' method with the following signature:
//..
//  bsl::ostream& print(bsl::ostream& stream,
//                      int           level          = 0,
//                      int           spacesPerLevel = 4) const;
//..
// To output an 'X' object with this trait declared, the
// 'bdeu_PrintMethods::print' method simply forwards to this method.  This
// means that the print operation is completely defined by the class.  Ideally,
// it should behave according to the standard BDE 'print' protocol which is
// documented as follows:
//..
//  Format this object to the specified output 'stream' at the (absolute value
//  of) the optionally specified indentation 'level' and return a reference to
//  'stream'.  If 'level' is specified, optionally specify 'spacesPerLevel',
//  the number of spaces per indentation level for this and all of its nested
//  objects.  If 'level' is negative, suppress indentation of the first line.
//  If 'spacesPerLevel' is negative, format the entire output on one line,
//  suppressing all but the initial indentation (as governed by 'level').  If
//  'stream' is not valid on entry, this operation has no effect.
//..
//
///Affect of 'bslalg_TypeTraitHasStlIterators' Trait
///- - - - - - - - - - - - - - - - - - - - - - - - -
// If a class 'X' declares the 'bslalg_TypeTraitHasStlIterators' trait, then
// it must provide access to iterators using the standard STL protocol.  The
// BDE implementation of STL declares this trait for all STL container types
// that have STL iterators.  Other containers that provide STL iterators should
// declare this trait to get correct printing behavior.
//
// When an 'X' object with this trait is printed using
// 'bdeu_PrintMethods::print', the contents of the object is traversed via an
// iterator and the output is formatted according to the standard BDE 'print'
// protocol, as documented above.  Additionally, an opening '[' character is
// prepended at the beginning of the output and a closing ']' character is
// appended at the end of the output.  Each iterated element is printed using
// its own print method, and with an indentation level one higher than that of
// the container.
//
///Affect of 'bslalg_TypeTraitPair' Trait
/// - - - - - - - - - - - - - - - - - - -
// If a class 'X' declares the 'bslalg_TypeTraitPair' trait, then the class
// must contain two 'public' data members named 'first' and 'second'.  The
// BDE implementation of STL declares this trait for the 'bsl::pair' 'struct'.
// Other classes that have 'public' 'first' and 'second' data members may
// declare this trait to get printing behavior similar to that of 'bsl::pair'.
//
// When an 'X' object with this trait is printed using
// 'bdeu_PrintMethods::print', its output is formatted based on the standard
// BDE 'print' protocol, as documented above.  Additionally, an opening '['
// character is prepended at the beginning of the output and a closing ']'
// character is appended at the end of the output.  The 'first' and 'second'
// elements are printed using their own 'print' methods, and with an
// indentation level one higher than that of the pair object.
//
///Usage
///-----
// Consider the following value-semantic class that holds an object of
// parameterized 'TYPE':
//..
//  template <typename TYPE>
//  class MyWrapper {
//    // An example wrapper class for a 'TYPE' object.
//
//    // PRIVATE DATA MEMBERS
//    TYPE d_obj;  // wrapped object
//
//    public:
//      // CREATORS
//      MyWrapper() { }
//      MyWrapper(const TYPE& original) : d_obj(original) { }
//          // ... other constructors and destructor ...
//
//      // MANIPULATORS
//          // ... assignment operator, etc. ...
//
//      // ACCESSORS
//      bsl::ostream& print(bsl::ostream& stream,
//                          int           level          = 0,
//                          int           spacesPerLevel = 4) const;
//          // Format the contained 'TYPE' to the specified output 'stream' at
//          // the (absolute value of) the optionally specified indentation
//          // 'level' and return a reference to 'stream'.  If 'level' is
//          // specified, optionally specify 'spacesPerLevel', the number of
//          // spaces per indentation level for this and all of its nested
//          // objects.  If 'level' is negative, suppress indentation of the
//          // first line.  If 'spacesPerLevel' is negative, format the entire
//          // output on one line, suppressing all but the initial indentation
//          // (as governed by 'level').  If 'stream' is not valid on entry,
//          // this operation has no effect.
//  };
//..
// This class contains a standard BDE 'print' method for value-semantic
// components.  Ideally, the implementation of 'MyWrapper<TYPE>::print' would
// delegate the task of printing to the corresponding 'print' method of
// 'd_obj':
//..
//  template <typename TYPE>
//  bsl::ostream& MyWrapper<TYPE>::print(bsl::ostream& stream,
//                                       int           level,
//                                       int           spacesPerLevel) const
//  {
//      return d_obj.print(stream, level, spacesPerLevel);
//  }
//..
// This method will work fine when 'TYPE' is a standard BDE value-semantic
// component (e.g., 'bdem_List'):
//..
//  bdem_List            myList;
//  MyWrapper<bdem_List> myWrapperForList(myList);
//  myWrapperForList.print(bsl::cout);  // No problem: 'bdem_List' has a
//                                      // corresponding 'print' method.
//..
// However, when 'TYPE' is not a standard BDE value-semantic component (e.g.,
// suppose 'TYPE' does not have the standard BDE 'print' method -- for example,
// fundamental types, enumerations, pointers, and 'bsl::vector'), the 'print'
// method defined above will not work:
//..
//  int            myInt = 123;
//  MyWrapper<int> myWrapperForInt(myInt);
//  myWrapperForInt.print(bsl::cout);  // Error: 'int' has no corresponding
//                                     // 'print' method.
//..
// The solution is to use the 'bdeu_PrintMethods::print' method, which provides
// generic printing capabilities, as follows:
//..
//  template <typename TYPE>
//  bsl::ostream& MyWrapper<TYPE>::print(bsl::ostream& stream,
//                                       int           level,
//                                       int           spacesPerLevel) const
//  {
//      return bdeu_PrintMethods::print(stream, d_obj, level, spacesPerLevel);
//  }
//..
// One thing missing from the 'MyWrapper' definition above is a declaration of
// the 'print' method trait.  Since the 'MyWrapper' class has a 'print' method,
// it is desirable to make it programmatically detectable that the 'print'
// method is available.  This is done by declaring the
// 'bdeu_TypeTraitHasPrintMethod' trait inside the 'MyWrapper' class
// definition:
//..
//  template <typename TYPE>
//  class MyWrapper {
//      // An example wrapper for a 'TYPE' object that also declares the
//      // 'bdeu_TypeTraitHasPrintMethod' trait.
//
//      ... private data members ...
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(MyWrapper, bdeu_TypeTraitHasPrintMethod);
//
//      ... rest of class definition ...
//  };
//..
// (See the 'bslalg_typetraits' component for more information about declaring
// traits for user-defined classes.)
//
// Now that 'MyWrapper' declares the 'bdeu_TypeTraitHasPrintMethod' trait, it
// can be reliably used as a template parameter for any other template class,
// including itself.  For example, the following code works:
//..
//  MyWrapper<MyWrapper<int> > myWrappedWrapper;
//  myWrappedWrapper.print(bsl::cout);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEU_PRINT
#include <bdeu_print.h>
#endif

#ifndef INCLUDED_BSLMF_SELECTTRAIT
#include <bslmf_selecttrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITPAIR
#include <bslalg_typetraitpair.h>
#endif

#ifndef INCLUDED_BSL_IOMANIP
#include <bsl_iomanip.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BDEU_CHARTYPE
#include <bdeu_chartype.h>
#endif

#endif

namespace bsl {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_string;

}

namespace BloombergLP {

                    // ==========================
                    // struct bdeu_HasPrintMethod
                    // ==========================

template <class TYPE>
struct bdeu_HasPrintMethod :
        bslmf::DetectNestedTrait<TYPE, bdeu_HasPrintMethod>::type {
    // A class, 'TYPE', should specialize this trait to derive from
    // 'true_type' if it has a 'print' method with the following signature:
    //..
    //  bsl::ostream& print(bsl::ostream& stream,
    //                      int           level          = 0,
    //                      int           spacesPerLevel = 4) const;
    //..
};

                    // ===================================
                    // struct bdeu_TypeTraitHasPrintMethod
                    // ===================================

struct bdeu_TypeTraitHasPrintMethod {
    // A class should declare this trait if it has a 'print' method with the
    // following signature:
    //..
    //  bsl::ostream& print(bsl::ostream& stream,
    //                      int           level          = 0,
    //                      int           spacesPerLevel = 4) const;
    //..

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bdeu_HasPrintMethod>
    {
        // This class template ties the 'bslalg::TypeTaitBitwiseMoveable'
        // trait tag to the 'bslmf::IsBitwiseMoveable' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bdeu_HasPrintMethod<TYPE>::type { };
};

                        // ===========================
                        // namespace bdeu_PrintMethods
                        // ===========================

namespace bdeu_PrintMethods {
    // This 'namespace' contains parameterized 'print' methods having the
    // standard BDE signature for such methods.

    // CLASS METHODS
    template <typename TYPE>
    bsl::ostream& print(bsl::ostream& stream,
                        const TYPE&   object,
                        int           level          = 0,
                        int           spacesPerLevel = 4);
        // Format the specified 'object' to the specified output 'stream' at
        // the (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

    template <typename CHAR_T, typename CHAR_TRAITS_T, typename ALLOC>
    bsl::ostream& print(
                    bsl::ostream&                          stream,
                    const bsl::basic_string<CHAR_T,
                                            CHAR_TRAITS_T,
                                            ALLOC>&        object,
                    int                                    level          = 0,
                    int                                    spacesPerLevel = 4);
        // Format the specified 'object' to the specified output 'stream' at
        // the (absolute value of) the optionally specified indentation 'level'
        // and return a reference to stream.  Note that output will be
        // formatted on one line.  If 'stream' is not valid on entry, this
        // operation has no effect.

    template <typename ALLOC>
    bsl::ostream& print(bsl::ostream&                   stream,
                        const bsl::vector<char, ALLOC>& object,
                        int                             level          = 0,
                        int                             spacesPerLevel = 4);
        // Format the specified 'object' to the specified output 'stream' at
        // the (absolute value of) the optionally specified indentation 'level'
        // and return a reference to stream.  Note that output will be
        // formatted on one line.  Also note that non-printable characters in
        // 'object' will be printed using their hexadecimal representation.  If
        // 'stream' is not valid on entry, this operation has no effect.

}  // close namespace bdeu_PrintMethods

// ---- Anything below this line is implementation specific.  Do not use.  ----

                // --------------------------------------------
                // struct bdeu_PrintMethods_Imp<TYPE, SELECTOR>
                // --------------------------------------------

template <typename TYPE, typename SELECTOR>
struct bdeu_PrintMethods_Imp;
    // Component-private 'struct'.  Do not use outside of this component.
    // Specializations on specific 'SELECTOR' values are defined below.

              // --------------------------------------------------------------
              // struct bdeu_PrintMethods_Imp<TYPE, bdeu_HasPrintMethod<TYPE> >
              // --------------------------------------------------------------

template <typename TYPE>
struct bdeu_PrintMethods_Imp<TYPE, bdeu_HasPrintMethod<TYPE> > {
    // Component-private 'struct'.  Do not use outside of this component.  This
    // 'struct' provides a 'print' function that prints objects of the
    // parameterized 'TYPE' that are associated with the 'bdeu_HasPrintMethod'
    // trait.

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
};

         // ------------------------------------------------------------------
         // struct bdeu_PrintMethods_Imp<TYPE, bslalg::HasStlIterators<TYPE> >
         // ------------------------------------------------------------------

template <typename TYPE>
struct bdeu_PrintMethods_Imp<TYPE, bslalg::HasStlIterators<TYPE> > {
    // Component-private 'struct'.  Do not use outside of this component.  This
    // 'struct' provides a 'print' function that prints objects of the
    // parameterized 'TYPE' that have the 'bslalg::StlIterators'
    // trait declared.

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
};

                  // ---------------------------------------------------------
                  // struct bdeu_PrintMethods_Imp<TYPE, bslmf::IsPair<TYPE> >
                  // ---------------------------------------------------------

template <typename TYPE>
struct bdeu_PrintMethods_Imp<TYPE, bslmf::IsPair<TYPE> > {
    // Component-private 'struct'.  Do not use outside of this component.  This
    // 'struct' provides a 'print' function that prints objects of
    // parameterized 'TYPE' that declare the 'bslmf::IsPair' trait.

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
};

            // -----------------------------------------------------
            // struct bdeu_PrintMethods_Imp<TYPE, bsl::false_type>
            // -----------------------------------------------------

template <typename TYPE>
struct bdeu_PrintMethods_Imp<TYPE, bsl::false_type> {
    // Component-private 'struct'.  Do not use outside of this component.  This
    // 'struct' provides a 'print' function that prints objects of
    // parameterized 'TYPE' that do not declare any of the traits recognized
    // by this component.

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
        // Print the specified 'object' to the specified 'stream' using its
        // '<<' output stream operator.  Note that a compiler error will result
        // if the specified 'TYPE' does not have a '<<' output stream operator.
};

// ============================================================================
//                   TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

          // ---------------------------------------------------------------
          // struct bdeu_PrintMethods_Imp<TYPE, bdeu_HasPrintMethod<TYPE> >
          // ---------------------------------------------------------------

// CLASS METHODS
template <typename TYPE>
inline
bsl::ostream&
bdeu_PrintMethods_Imp<TYPE, bdeu_HasPrintMethod<TYPE> >::
print(bsl::ostream& stream,
      const TYPE&   object,
      int           level,
      int           spacesPerLevel)
{
    // A compilation error indicating the next line of code implies the
    // 'TYPE' parameter does not have a 'print' method with the expected
    // signature.

    return object.print(stream, level, spacesPerLevel);
}

         // ------------------------------------------------------------------
         // struct bdeu_PrintMethods_Imp<TYPE, bslalg::HasStlIterators<TYPE> >
         // ------------------------------------------------------------------

// CLASS METHODS
template <typename TYPE>
bsl::ostream&
bdeu_PrintMethods_Imp<TYPE, bslalg::HasStlIterators<TYPE> >::
print(bsl::ostream& stream,
      const TYPE&   object,
      int           level,
      int           spacesPerLevel)

{
    if (stream.bad()) {
        return stream;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    // A compilation error indicating the next line of code implies the
    // 'TYPE' parameter does not have STL-compliant iterators.

    typedef typename TYPE::const_iterator Iterator;

    if (0 <= spacesPerLevel) {
        // Multi-line output.

        if (level < 0) {
            level = -level;
        }

        stream << "[\n";

        const int levelPlus1 = level + 1;

        for (Iterator it = object.begin(); it != object.end(); ++it) {
            bdeu_PrintMethods::print(stream,
                                     *it,
                                     levelPlus1,
                                     spacesPerLevel);
        }

        bdeu_Print::indent(stream, level, spacesPerLevel);

        stream << "]\n";
    }
    else {
        // Output on a single line and suppress any further indentation.

        stream << "[ ";

        for (Iterator it = object.begin(); it != object.end(); ++it) {
            bdeu_PrintMethods::print(stream, *it, 0, -1);
            stream << ' ';
        }

        stream << ']';
    }

    return stream << bsl::flush;
}

                  // ---------------------------------------------------------
                  // struct bdeu_PrintMethods_Imp<TYPE, bslmf::IsPair<TYPE> >
                  // ---------------------------------------------------------

// CLASS METHODS
template <typename TYPE>
bsl::ostream&
bdeu_PrintMethods_Imp<TYPE, bslmf::IsPair<TYPE> >::
print(bsl::ostream& stream,
      const TYPE&   object,
      int           level,
      int           spacesPerLevel)
{
    if (stream.bad()) {
        return stream;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    if (0 <= spacesPerLevel) {
        // Multi-line output.

        if (level < 0) {
            level = -level;
        }

        stream << "[\n";

        const int levelPlus1 = level + 1;

        // A compilation error indicating the next line of code implies the
        // 'TYPE' parameter is not a pair.

        bdeu_PrintMethods::print(stream,
                                 object.first,
                                 levelPlus1,
                                 spacesPerLevel);
        bdeu_PrintMethods::print(stream,
                                 object.second,
                                 levelPlus1,
                                 spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);

        stream << "]\n";
    }
    else {
        // Output on a single line and suppress any further indentation.

        stream << "[ ";

        // A compilation error indicating the next line of code implies the
        // 'TYPE' parameter is not a pair.

        bdeu_PrintMethods::print(stream, object.first, 0, -1);
        stream << ' ';

        bdeu_PrintMethods::print(stream, object.second, 0, -1);
        stream << " ]";
    }

    return stream << bsl::flush;
}

            // -----------------------------------------------------
            // struct bdeu_PrintMethods_Imp<TYPE, bsl::false_type>
            // -----------------------------------------------------

// CLASS METHODS
template <typename TYPE>
bsl::ostream&
bdeu_PrintMethods_Imp<TYPE, bsl::false_type>::
print(bsl::ostream& stream,
      const TYPE&   object,
      int           level,
      int           spacesPerLevel)
{
    if (stream.bad()) {
        return stream;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    // A compilation error indicating the next line of code implies the
    // 'TYPE' parameter does not have the '<<' output stream operator.

    stream << object;

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream;
}

                        // ---------------------------
                        // namespace bdeu_PrintMethods
                        // ---------------------------

// CLASS METHODS
template <typename TYPE>
bsl::ostream&
bdeu_PrintMethods::print(bsl::ostream& stream,
                         const TYPE&   object,
                         int           level,
                         int           spacesPerLevel)
{
    typedef typename bslmf::SelectTrait<
                TYPE,
                bdeu_HasPrintMethod,
                bslalg::HasStlIterators,
                bslmf::IsPair
            >::Type BdeuSelector;
        
    return bdeu_PrintMethods_Imp<TYPE, BdeuSelector>::print(stream,
                                                            object,
                                                            level,
                                                            spacesPerLevel);
}

template <typename CHAR_T, typename CHAR_TRAITS_T, typename ALLOC>
bsl::ostream&
bdeu_PrintMethods::print(
         bsl::ostream&                                          stream,
         const bsl::basic_string<CHAR_T, CHAR_TRAITS_T, ALLOC>& object,
         int                                                    level,
         int                                                    spacesPerLevel)
{
    return bdeu_PrintMethods_Imp<
                          bsl::basic_string<CHAR_T, CHAR_TRAITS_T, ALLOC>,
                          bsl::false_type>::print(stream,
                                                  object,
                                                  level,
                                                  spacesPerLevel);
}

template <typename ALLOC>
bsl::ostream&
bdeu_PrintMethods::print(bsl::ostream&                   stream,
                         const bsl::vector<char, ALLOC>& object,
                         int                             level,
                         int                             spacesPerLevel)
{
    if (stream.bad()) {
        return stream;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << "\"";

    const int len = static_cast<int>(object.size());

    if (0 < len) {
        bdeu_Print::printString(stream, &object[0], len, false);
    }

    stream << "\"";

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream;
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
