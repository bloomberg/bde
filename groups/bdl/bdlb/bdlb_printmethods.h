// bdlb_printmethods.h                                                -*-C++-*-
#ifndef INCLUDED_BDLB_PRINTMETHODS
#define INCLUDED_BDLB_PRINTMETHODS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide methods for uniform printing of value-semantic types.
//
//@CLASSES:
//  bdlb::PrintMethods: templates for uniform printing of value-semantic types
//  bdlb::HasPrintMethod: trait indicating existence of 'print' method
//  bdlb::TypeTraitHasPrintMethod: Old-style version of 'bdlb::HasPrintMethod'
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bslalg_typetraits
//
//@DESCRIPTION: This component provides a namespace for print utilities
// that support uniform 'ostream' printing across all printable types,
// including template types and containers.  The 'bdlb::PrintMethods' namespace
// enables clients to output the value of any printable object according to the
// standard BDE 'print' protocol.  Using the 'bdlb::PrintMethods::print' method,
// it is convenient to print objects of unknown types, with an optionally-
// specified indentation level and also an optionally-specified number of
// spaces per indentation level.
//
///Traits Affecting Printing
///-------------------------
// By default, 'bdlb::PrintMethods::print' uses the '<<' stream output operator
// to print a value.  This formats the entire output on one line, suppressing
// all indentation.  A class can override this behavior by declaring certain
// traits related to printing.  This component detects these traits and invokes
// an appropriate print operation.  The following lists the traits recognized
// by this component:
//..
//  bdlb::TypeTraitHasPrintMethod       ( highest precedence )
//  bslalg::TypeTraitHasStlIterators
//  bslalg::TypeTraitPair              ( lowest precedence  )
//..
// Since a class may declare multiple traits (see the component-level
// documentation of 'bslalg_typetraits' for information about declaring
// traits), the relative precedence of the traits is shown above.  The next
// sub-sections describe these traits and their affects on printing.
//
///Effect of 'bdlb::TypeTraitHasPrintMethod' Trait
/// - - - - - - - - - - - - - - - - - - - - - - -
// If a class 'X' declares the 'bdlb::TypeTraitHasPrintMethod' trait, then it
// must provide a 'print' method with the following signature:
//..
//  bsl::ostream& print(bsl::ostream& stream,
//                      int           level          = 0,
//                      int           spacesPerLevel = 4) const;
//..
// To output an 'X' object with this trait declared, the
// 'bdlb::PrintMethods::print' method simply forwards to this method.  This
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
///Effect of 'bslalg::TypeTraitHasStlIterators' Trait
///- - - - - - - - - - - - - - - - - - - - - - - - -
// If a class 'X' declares the 'bslalg::TypeTraitHasStlIterators' trait, then
// it must provide access to iterators using the standard STL protocol.  The
// BDE implementation of STL declares this trait for all STL container types
// that have STL iterators.  Other containers that provide STL iterators should
// declare this trait to get correct printing behavior.
//
// When an 'X' object with this trait is printed using
// 'bdlb::PrintMethods::print', the contents of the object is traversed via an
// iterator and the output is formatted according to the standard BDE 'print'
// protocol, as documented above.  Additionally, an opening '[' character is
// prepended at the beginning of the output and a closing ']' character is
// appended at the end of the output.  Each iterated element is printed using
// its own print method, and with an indentation level one higher than that of
// the container.
//
///Effect of 'bslalg::TypeTraitPair' Trait
/// - - - - - - - - - - - - - - - - - - -
// If a class 'X' declares the 'bslalg::TypeTraitPair' trait, then the class
// must contain two 'public' data members named 'first' and 'second'.  The
// BDE implementation of STL declares this trait for the 'bsl::pair' 'struct'.
// Other classes that have 'public' 'first' and 'second' data members may
// declare this trait to get printing behavior similar to that of 'bsl::pair'.
//
// When an 'X' object with this trait is printed using
// 'bdlb::PrintMethods::print', its output is formatted based on the standard
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
// component (e.g., 'bdlmxxx::List'):
//..
//  bdlmxxx::List            myList;
//  MyWrapper<bdlmxxx::List> myWrapperForList(myList);
//  myWrapperForList.print(bsl::cout);  // No problem: 'bdlmxxx::List' has a
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
// The solution is to use the 'bdlb::PrintMethods::print' method, which provides
// generic printing capabilities, as follows:
//..
//  template <typename TYPE>
//  bsl::ostream& MyWrapper<TYPE>::print(bsl::ostream& stream,
//                                       int           level,
//                                       int           spacesPerLevel) const
//  {
//      return bdlb::PrintMethods::print(stream, d_obj, level, spacesPerLevel);
//  }
//..
// One thing missing from the 'MyWrapper' definition above is a declaration of
// the 'print' method trait.  Since the 'MyWrapper' class has a 'print' method,
// it is desirable to make it programmatically detectable that the 'print'
// method is available.  This is done by declaring the
// 'bdlb::TypeTraitHasPrintMethod' trait inside the 'MyWrapper' class
// definition:
//..
//  template <typename TYPE>
//  class MyWrapper {
//      // An example wrapper for a 'TYPE' object that also declares the
//      // 'bdlb::TypeTraitHasPrintMethod' trait.
//
//      ... private data members ...
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(MyWrapper, bdlb::TypeTraitHasPrintMethod);
//
//      ... rest of class definition ...
//  };
//..
// (See the 'bslalg_typetraits' component for more information about declaring
// traits for user-defined classes.)
//
// Now that 'MyWrapper' declares the 'bdlb::TypeTraitHasPrintMethod' trait, it
// can be reliably used as a template parameter for any other template class,
// including itself.  For example, the following code works:
//..
//  MyWrapper<MyWrapper<int> > myWrappedWrapper;
//  myWrappedWrapper.print(bsl::cout);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_PRINT
#include <bdlb_print.h>
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

namespace bsl {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_string;

}  // close namespace bsl

namespace BloombergLP {

namespace bdlb {
                    // ==========================
                    // struct HasPrintMethod
                    // ==========================

template <class TYPE>
struct HasPrintMethod :
        bslmf::DetectNestedTrait<TYPE, HasPrintMethod>::type {
    // A class, 'TYPE', should specialize this trait to derive from
    // 'true_type' if it has a 'print' method with the following signature:
    //..
    //  bsl::ostream& print(bsl::ostream& stream,
    //                      int           level          = 0,
    //                      int           spacesPerLevel = 4) const;
    //..
};

                    // ===================================
                    // struct TypeTraitHasPrintMethod
                    // ===================================

struct TypeTraitHasPrintMethod {
    // A class should declare this trait if it has a 'print' method with the
    // following signature:
    //..
    //  bsl::ostream& print(bsl::ostream& stream,
    //                      int           level          = 0,
    //                      int           spacesPerLevel = 4) const;
    //..

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, HasPrintMethod>
    {
        // This class template ties the 'bslalg::TypeTaitBitwiseMoveable'
        // trait tag to the 'bslmf::IsBitwiseMoveable' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : HasPrintMethod<TYPE>::type { };
};

                        // ===========================
                        // namespace PrintMethods
                        // ===========================

namespace PrintMethods {
    // This 'namespace' contains parameterized 'print' methods having the
    // standard BDE signature for such methods.

    // CLASS METHODS
    template <class TYPE>
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

    template <class CHAR_T, class CHAR_TRAITS_T, class ALLOC>
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

    template <class ALLOC>
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

}  // close namespace PrintMethods
}  // close package namespace

// ---- Anything below this line is implementation specific.  Do not use.  ----

                // --------------------------------------------
                // struct bdeu::PrintMethods_Imp<TYPE, SELECTOR>
                // --------------------------------------------


namespace bdlb {template <class TYPE, class SELECTOR>
struct PrintMethods_Imp;

template <class TYPE>
struct PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<> > {
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

              // --------------------------------------------------------------
              // struct PrintMethods_Imp<TYPE, HasPrintMethod<TYPE> >
              // --------------------------------------------------------------

template <class TYPE>
struct PrintMethods_Imp<TYPE,
                             bslmf::SelectTraitCase<HasPrintMethod> > {
    // Component-private 'struct'.  Do not use outside of this component.  This
    // 'struct' provides a 'print' function that prints objects of the
    // parameterized 'TYPE' that are associated with the 'HasPrintMethod'
    // trait.

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
};

         // ------------------------------------------------------------------
         // struct PrintMethods_Imp<TYPE, bslalg::HasStlIterators<TYPE> >
         // ------------------------------------------------------------------

template <class TYPE>
struct PrintMethods_Imp<TYPE,
                            bslmf::SelectTraitCase<bslalg::HasStlIterators> > {
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
                  // struct PrintMethods_Imp<TYPE, bslmf::IsPair<TYPE> >
                  // ---------------------------------------------------------

template <class TYPE>
struct PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<bslmf::IsPair> > {
    // Component-private 'struct'.  Do not use outside of this component.  This
    // 'struct' provides a 'print' function that prints objects of
    // parameterized 'TYPE' that declare the 'bslmf::IsPair' trait.

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
};

// ============================================================================
//                   TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                // --------------------------------------------
                // struct PrintMethods_Imp<TYPE, SELECTOR>
                // --------------------------------------------

// CLASS METHODS
template <class TYPE>
bsl::ostream&
PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<> >::
print(bsl::ostream& stream,
      const TYPE&   object,
      int           level,
      int           spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

    // A compilation error indicating the next line of code implies the
    // 'TYPE' parameter does not have the '<<' output stream operator.

    stream << object;

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream;
}

          // ---------------------------------------------------------------
          // struct PrintMethods_Imp<TYPE, HasPrintMethod<TYPE> >
          // ---------------------------------------------------------------

// CLASS METHODS
template <class TYPE>
inline
bsl::ostream&
PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<HasPrintMethod> >::
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
         // struct PrintMethods_Imp<TYPE, bslalg::HasStlIterators<TYPE> >
         // ------------------------------------------------------------------

// CLASS METHODS
template <class TYPE>
bsl::ostream&
PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<bslalg::HasStlIterators> >::
print(bsl::ostream& stream,
      const TYPE&   object,
      int           level,
      int           spacesPerLevel)

{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

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
            PrintMethods::print(stream,
                                     *it,
                                     levelPlus1,
                                     spacesPerLevel);
        }

        Print::indent(stream, level, spacesPerLevel);

        stream << "]\n";
    }
    else {
        // Output on a single line and suppress any further indentation.

        stream << "[ ";

        for (Iterator it = object.begin(); it != object.end(); ++it) {
            PrintMethods::print(stream, *it, 0, -1);
            stream << ' ';
        }

        stream << ']';
    }

    return stream << bsl::flush;
}

                  // ---------------------------------------------------------
                  // struct PrintMethods_Imp<TYPE, bslmf::IsPair<TYPE> >
                  // ---------------------------------------------------------

// CLASS METHODS
template <class TYPE>
bsl::ostream&
PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<bslmf::IsPair> >::
print(bsl::ostream& stream,
      const TYPE&   object,
      int           level,
      int           spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

    if (0 <= spacesPerLevel) {
        // Multi-line output.

        if (level < 0) {
            level = -level;
        }

        stream << "[\n";

        const int levelPlus1 = level + 1;

        // A compilation error indicating the next line of code implies the
        // 'TYPE' parameter is not a pair.

        PrintMethods::print(stream,
                                 object.first,
                                 levelPlus1,
                                 spacesPerLevel);
        PrintMethods::print(stream,
                                 object.second,
                                 levelPlus1,
                                 spacesPerLevel);

        Print::indent(stream, level, spacesPerLevel);

        stream << "]\n";
    }
    else {
        // Output on a single line and suppress any further indentation.

        stream << "[ ";

        // A compilation error indicating the next line of code implies the
        // 'TYPE' parameter is not a pair.

        PrintMethods::print(stream, object.first, 0, -1);
        stream << ' ';

        PrintMethods::print(stream, object.second, 0, -1);
        stream << " ]";
    }

    return stream << bsl::flush;
}

                        // ---------------------------
                        // namespace PrintMethods
                        // ---------------------------

// CLASS METHODS
template <class TYPE>
bsl::ostream&
PrintMethods::print(bsl::ostream& stream,
                         const TYPE&   object,
                         int           level,
                         int           spacesPerLevel)
{
    typedef typename bslmf::SelectTrait<
                TYPE,
                HasPrintMethod,
                bslalg::HasStlIterators,
                bslmf::IsPair
            >::Type BdeuSelector;

    return PrintMethods_Imp<TYPE, BdeuSelector>::print(stream,
                                                            object,
                                                            level,
                                                            spacesPerLevel);
}

template <class CHAR_T, class CHAR_TRAITS_T, class ALLOC>
bsl::ostream&
PrintMethods::print(
         bsl::ostream&                                          stream,
         const bsl::basic_string<CHAR_T, CHAR_TRAITS_T, ALLOC>& object,
         int                                                    level,
         int                                                    spacesPerLevel)
{
    return PrintMethods_Imp<
                          bsl::basic_string<CHAR_T, CHAR_TRAITS_T, ALLOC>,
                          bslmf::SelectTraitCase<> >::print(stream,
                                                            object,
                                                            level,
                                                            spacesPerLevel);
}

template <class ALLOC>
bsl::ostream&
PrintMethods::print(bsl::ostream&                   stream,
                         const bsl::vector<char, ALLOC>& object,
                         int                             level,
                         int                             spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

    stream << "\"";

    const int len = static_cast<int>(object.size());

    if (0 < len) {
        Print::printString(stream, &object[0], len, false);
    }

    stream << "\"";

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream;
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
