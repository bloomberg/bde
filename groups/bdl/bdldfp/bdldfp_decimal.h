// bdldfp_decimal.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMAL
#define INCLUDED_BDLDFP_DECIMAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide IEEE-754 decimal floating-point types.
//
//@CLASSES:
//  bdldfp::Decimal32:   32bit IEEE-754 decimal floating-point type
//  bdldfp::Decimal64:   64bit IEEE-754 decimal floating-point type
//  bdldfp::Decimal128: 128bit IEEE-754 decimal floating-point type
//  bdldfp::DecimalNumGet: Stream Input Facet
//  bdldfp::DecimalNumPut: Stream Output Facet
//
//@MACROS:
//  BDLDFP_DECIMAL_DF: Portable Decimal32 literal macro
//  BDLDFP_DECIMAL_DD: Portable Decimal64 literal macro
//  BDLDFP_DECIMAL_DL: Portable Decimal128 literal macro
//
//@SEE_ALSO: bdldfp_decimalutil, bdldfp_decimalconvertutil,
//           bdldfp_decimalplatform
//
//@DESCRIPTION: This component provides classes that implement decimal
// floating-point types that conform in layout, encoding and operations to the
// IEEE-754 2008 standard.  This component also provides two facets to support
// standard C++ streaming operators as specified by ISO/IEC TR-24733:2009.
// These classes are 'bdldfp::Decimal32' for 32-bit Decimal floating point
// numbers, 'bdldfp::Decimal64' for 64-bit Decimal floating point numbers, and
// 'bdldfp::Decimal128' for 128-bit decimal floating point numbers.
//
// Decimal encoded floating-point numbers are important where exact
// representation of decimal fractions is required, such as in financial
// transactions.  Binary encoded floating-point numbers are generally optimal
// for complex computation but cannot exactly represent commonly encountered
// numbers such as 0.1, 0.2, and 0.99.
//
// The BDE decimal floating-point system has been designed from the ground up
// to be portable and support writing portable decimal floating-point user
// code, even for systems that do not have compiler or native library support
// for it; while taking advantage of native support (such as ISO/IEC TR
// 24732 - C99 decimal TR) when available.
//
// 'bdldfp::DecimalNumGet' and 'bdldfp::DecimalNumPut' are IO stream facets.
//
///Floating-Point Primer
///---------------------
// There are several ways of represent numbers when using digital computers.
// The simplest would be an integer format, however such a format severely
// limits the range of numbers that can be represented; and it cannot represent
// real (non-integer) numbers directly at all.  Integers might be used to
// represent real numbers of limited precision by treating them as a multiple
// of the real value being represented; these are often known as fixed-point
// numbers.  However general computations require higher precision and a larger
// range than integer and fixed point types are able to efficiently provide.
// Floating-point numbers provide what integers cannot.  They are able to
// represent a large range of real values (although not precisely) while using
// a fixed (and reasonable) amount of storage.
//
// Floating-point numbers are constructed from a set of significant digits of a
// radix on a sliding scale, where their position is determined by an exponent
// over the same radix.  For example let's see some 32bit decimal (radix 10)
// floating-point numbers that have maximum 7 significant digits (significand):
//..
//   Significand | Exponent | Value        |
//  -------------+----------+--------------+  In the Value column you may
//       1234567 |        0 |   1234567.0  |  observer how the decimal point
//       1234567 |        1 |  12345670.0  |  is "floating" about the digits
//       1234567 |        2 | 123456700.0  |  of the significand.
//       1234567 |       -1 |    123456.7  |
//       1234567 |       -2 |     12345.67 |
//..
// Floating-point numbers are standardized by IEEE-754 2008, in two major
// flavors: binary and decimal.  Binary floating-point numbers are supported by
// most computer systems in the forms of the 'float', 'double' and
// 'long double' fundamental data types.  While they are not required to be
// binary that is almost always the choice on modern binary computer
// architectures.
//
///Floating-Point Peculiarities
/// - - - - - - - - - - - - - -
// Floating-point approximation of real numbers creates a deliberate illusion.
// While it looks like we are working with real numbers, floating-point
// encodings are not able to represent real numbers precisely since they have a
// restricted number of digits in the significand.  In fact, a 64 bit
// floating-point type can represent fewer distinct values than a 64 bit binary
// integer.  Yet, because floating-point encodings can represent numbers over a
// much larger range, including extremely small (fractional) numbers, they are
// useful in practice.
//
// Floating-point peculiarities may be split into three categories: those that
// are due to the (binary) radix/base, those that are inherent properties of
// any floating-point representation and finally those that are introduced by
// the IEEE-754 2008 standard.  Decimal floating-point addresses the first set
// of surprises only; so users still need to be aware of the rest.
//
//: 1 Floating-point types cannot exactly represent every number in their
//:   range.  The consequences are surprising and unexpected for the newcomer.
//:   For example: when using binary floating-point numbers, the following
//:   expression is typically *false*: '0.1 + 0.2 == 0.3'.  The problem is not
//:   limited to binary floating-point.  Decimal floating-point cannot
//:   represent the value of one third exactly.
//:
//: 2 Unlike with real numbers, the order of operations on floating-point
//:   numbers is significant, due to accumulation of round off errors.
//:   Therefore floating-point arithmetic is neither commutative nor
//:   transitive.  E.g., 2e-30 + 1e30 - 1e-30 - 1e30 will typically produce 0
//:   (unless your significand can hold 60 decimal digits).  Alternatively,
//:   1e30 - 1e30 + 2e-30 - 1e-30 will typically produce 1e-30.
//:
//: 3 IEEE floating-point types can have special values: negative zero,
//:   negative and positive infinity; and they can be NaN (Not a Number, in two
//:   variants: quiet or signaling).  A NaN (any variant) is never equal to
//:   anything else - including NaN or itself!
//:
//: 4 In IEEE floating-point there are at least two representations of 0, the
//:   positive zero and negative zero.  Consequently unary - operators change
//:   the sign of the value 0; therefore leading to surprising results: if
//:   'f == 0.0' then '0 - f' and '-f' will not result in the same value,
//:   because '0 - f' will be +0.0' while '-f' will be -0.0. ** HERE WE ARE **
//:
//: 5 Most IEEE floating-point operations (like arithmetic) have implicit input
//:   parameters and output parameters (that do not show up in function
//:   signatures.  The implicit input parameters are called *attributes* by
//:   IEEE while the outputs are called status flags.  The C/C++ programming
//:   language defines a so-called floating-point environment that contains
//:   those attributes and flags ('<fenv.h>' C and '<cfenv>' C++ headers).  To
//:   learn more about the floating point environment read the subsection of
//:   the same title, but first make sure you read the next point as well.
//:
//: 6 IEEE floating-points overloads some very common programming language
//:   terms: *exception*, *signal* and *handler* with IEEE floating-point
//:   specific meanings that are not to be confused with C or C++ or Posix
//:   terms of the same spelling.  Floating-point exceptions are events that
//:   occur when a floating-point operations on the specified operands is
//:   unable to produce a perfect outcome; such as when the result of an
//:   operation is inexact.  When a floating point exception occurs the
//:   (floating-point) - and reporting it is requested by a so-called trap
//:   attribute - the implementation signals the user(*) by invoking a default
//:   or a user-defined handler.  None of the words *exception*, *signal*, and
//:   *handler* used above have nothing to do with C++ exceptions, Posix
//:   signals and the handlers of those.  (To complicate matters more, C and
//:   Posix has decided to implement IEEE floating-point exception reporting as
//:   C/Posix signals - and therefore rendered them mostly useless.)
//:
//: 7 While a 32bit integer is a quite useful type for (integer) calculations,
//:   a 32bit floating-point type has such low accuracy (its significand is so
//:   short) that it is all but useless for calculation.  Such types are called
//:   "interchange formats" by the IEEE standard and should not be used for
//:   calculations.  (Except in special circumstances and by floating-point
//:   experts.  Even a 16 bit binary floating-point type can be useful for an
//:   expert in special circumstances, for example in graphics acceleration
//:   hardware.)
//
// Notes:
//   (*) IEEE Floating-point user is any person, hardware or software that uses
//       the IEEE floating-point implementation.
//
///Floating-Point Environment
/// - - - - - - - - - - - - -
// NOTE: We currently do not give access to the user to the floating-point
// environment used by our decimal system, so description of it here is
// preliminary and generic.  Note that since compilers and the C library
// already provides a (possibly binary floating-point only) environment and we
// cannot change that, our decimal floating-point environment implementation
// cannot conform to the C and C++ TRs (because those require extending the
// existing standard C library functions).
//
// The floating-point environment provides implicit input and output parameters
// to floating-point operations (that are defined to use them).  IEEE defined
// those parameters in principle, but how they are provided is left up to be
// designed/defined by the implementors of the programming languages.
//
// C (and consequently C++) decided to provide a so-called floating-point
// environment that has "thread storage duration", meaning that each thread of
// a multi-threaded program will have its own distinct floating-point
// environment.
//
// The C/C++ floating-point environment consists of 3 major parts: the rounding
// mode, the traps and the status flags.
//
///Rounding Direction in The Environment
///-  -  -  -  -  -  -  -  -  -  -  -  -
// A floating-point *rounding direction* determines how is the significand of a
// higher (or infinite) precision number get rounded to fit into the limited
// number of significant digits (significand) of the floating-point
// representation that needs to store it as a result of an operation.  Note
// that the rounding is done in the radix of the representation, so binary
// floating-point will do binary rounding while decimal floating-point will do
// decimal rounding - and not all rounding modes are useful with all radixes.
// An example of a generally applicable rounding mode would be 'FE_TOWARDZERO'
// (round towards zero).
//
// Most floating point operations in C and C++ do not take a rounding direction
// parameter (and the ones that are implemented as operators simply could not).
// When such operations (that do not have an explicit rounding direction
// parameter) need to do rounding, they use the rounding direction set in the
// floating-point environment (of their thread of execution).
//
///Status Flags
///  -  -  -  -
// Floating point operations in C and C++ do not take a status flag output
// parameter.  They report an important events (such as underflow, overflow or
// in inexact (rounded) result) by setting the appropriate status flag in the
// floating-point environment (of their thread of execution).  (Note that this
// is very similar to how flags work in CPUs, and that is not a coincidence.)
// The flags work much like individual, boolean 'errno' values.  Operations may
// set them to true.  Users may examine them (when interested) and also reset
// them (set them to 0) before an operation.
//
///Floating-Point Traps
/// -  -  -  -  -  -  -
// IEEE says that certain floating-point events are floating-point exceptions
// and they result in invoking a handler.  It may be a default handler (set a
// status flag and continue) or a user defined handler.  Floating point traps
// are a C invention to enable "sort-of handlers" for floating point
// exceptions, but unfortunately they all go to the same handler: the 'SIGFPE'
// handler.  To add insult to injury, setting what traps are active (what will)
// cause a 'SIGFPE') is not standardized.  So floating-point exceptions and
// handlers are considered pretty much useless in C.  (All is not lost, since
// we do have the status flags.  An application that wants to know about
// floating-point events can clear the flags prior to an operation and check
// their values afterwards.)
//
///Floating-Point Terminology
/// - - - - - - - - - - - - -
// A floating-point representation of a number is defined as follows:
// 'sign * significand * BASE^exponent', where sign is -1 or +1, significand is
// an integer, BASE is a positive integer (but usually 2 or 10) and exponent is
// a negative or positive integer.  Concrete examples of (decimal) numbers in
// the so-called scientific notation are: 123.4567 is 1.234567e2, while
// -0.000000000000000000000000000000000000001234567 would be -1.234567e-41.
//
//: "base":
//:    the number base of the scaling used by the exponent; and by the
//:    significand
//:
//: "bias":
//:    the number added to the exponent before it is stored in memory; 101, 398
//:    and 6176 for the 32, 64 and 128 bit types respectively.
//:
//: "exponent":
//:    the scaling applied to the significand is calculated by raising the base
//:    to the exponent (which may be also negative)
//:
//: "quantum":
//:     (IEEE-754) the value of one unit at the last significant digit
//:     position; in other words the smallest difference that can be
//:     represented by a floating-point number without changing its exponent.
//:
//:     o Note that the C++ Decimal TR uses a different, not-specified, and not
//:       very useful definition, which we chose to ignore here.
//:
//: "mantissa":
//:    the old name for the significand
//:
//: "radix":
//:    another name for base
//:
//: "sign":
//:    +1 or -1, determines if the number is positive or negative.  It is
//:    normally represented by a single sign bit.
//:
//: "significand":
//:    the significant digits of the floating-point number; the value of the
//:    number is: 'sign * significand * base^exponent'
//:
//: "precision":
//:    the significant digits of the floating-point type in its base
//:
//: "decimal precision":
//:    the maximum significant decimal digits of the floating-point type
//:
//: "range":
//:    the smallest and largest number the type can represent.  Note that for
//:    floating-point types there are at least *two* interpretations of
//:    minimum.  It may be the largest negative number *or* the smallest number
//:    in absolute value) that can be represented.
//:
//: "normalized number":
//:    '1 <= significand <= base'
//:
//: "normalization":
//:    finding the exponent such as '1 <= significand <= base'
//:
//: "denormal number":
//:    'significand < 1'
//:
//: "densely packed decimal":
//:    one of the two IEEE significand encoding schemes
//:
//: "binary integer significand":
//:    one of the two IEEE significand encoding schemes
//:
//: "cohorts":
//:    equal numbers encoded using different exponents (to signify accuracy)
//
///Decimal Floating-Point
///----------------------
// Binary floating-point formats give best accuracy, they are the fastest (on
// binary computers), and were carefully designed by IEEE to minimize rounding
// errors (errors due to the inherent imprecision of floating-point types)
// during a lengthy calculation.  This makes them the best solution for and
// serious scientific computation.  However, they have a fatal flow when it
// comes to numbers and calculations that involve humans.  Humans think in base
// 10 - decimal.  And as the example has shown earlier, binary floating-point
// formats are unable to precisely represent very common decimal real numbers;
// with binary floating-point '0.1 + 0.2 != 0.3'.  (Why?  Because none of the
// three numbers in that expression have an exact binary floating-point
// representation.)
//
// Financial calculations are governed by laws and expectations that are based
// on decimal (10 based) thinking.  Due to the inherent limitations of the
// binary floating-point format, doing such decimal based calculations and
// algorithms using binary floating-point numbers is so involved and hard that
// that it is considered not feasible.  The IEEE-754 committee have recognized
// the issue and added specifications for 3 decimal floating-point types into
// their 2008 standard: the 32, 64 and 128 bits decimal floating-point formats.
//
// Floating-point types are carefully designed trade-offs between saving space
// (in memory), CPU cycles (for calculations) and still provide useful accuracy
// for computations.  Decimal floating-point types represent further
// compromises (compared to binary floating-points) in being able to represent
// less numbers (than their binary counterpats) and being slower, but providing
// exact representations for the numbers humans care about.
//
// In decimal floating-point world '0.1 + 0.2 == 0.3', as humans expect;
// because each of those 3 numbers can be represented *exactly* in a decimal
// floating-point format.
//
///*WARNING*: Conversions from 'float' and 'double'
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Clients should *be* *careful* when using the conversions from 'float' and
// 'double' provided by this component.  In situations where a 'float' or
// 'double' was originally obtained from a decimal floating point
// representation (e.g., a 'bdldfp::Decimal', or a string, like "4.1"), the
// conversions in 'bdldfp_decimalconvertutil' will provide the correct
// conversion back to a decimal floating point value.  The conversions in this
// component provide the closest decimal floating point value to the supplied
// binary floating point representation, which may replicate imprecisions
// required to initially approximate the value in a binary representation.
// The conversions in this component are typically useful when converting
// binary floating point values that have undergone mathematical operations
// that require rounding (so they are already in-exact approximations).
//
///Cohorts
///- - - -
// In the binary floating-point world the formats are optimized for the highest
// precision, range and speed.  They are stored normalized and therefore store
// no information about their accuracy.  In finances, the area that decimal
// floating-point types target, accuracy of a number is usually very important.
// We may have a number that is 1, but we know it may be 1.001 or 1.002 etc.
// And we may have another number 1, which we know to be accurate to 6
// significant digits.  We would display the former number as '1.00' and the
// latter number as '1.00000'.  The decimal floating-point types are able to
// store both numbers *and* their precision using so call cohorts.  The '1.00'
// will be stored as '100e-2' while '1.00000' will be stored as 100000e-5.
//
// Cohorts compare equal, and mostly behave the same way in calculation except
// when it comes to the accuracy of the result.  If I have a number that is
// accurate to 5 digits only, it would be a mistake to try to expect more than
// 5 digits accuracy from a calculation involving it.  The IEEE-754 rules of
// cohorts (in calculations) ensures that results will be a cohort that
// indicates the proper expected accuracy.
//
///Standards Conformance
///---------------------
// The component has also been designed to resemble the C++ Decimal
// Floating-Point Technical Report ISO/IEC TR-24733 of 2009 and its C++11
// updates of ISO/IEC JTC1 SC22 WG21 N3407=12-0097 of 2012 as much as it is
// possible with C++03 compilers and environments that do not provide decimal
// floating-point support in any form.
//
// At the time of writing there is just one standard about decimal-floating
// point, the IEEE-754 2008 standard and the content of this component conforms
// to it.  The component does not fully implement all required IEEE-754
// functionality because due to our architectural design guidelines some of
// these must go into a separate so-called utility component.)
//
// The component uses the ISO/IEC TR 24732 - the C Decimal Floating-Point
// TR - in its implementation where it is available.
//
// The component closely resembles ISO/IEC TR 24733 - the C++ Decimal
// Floating-Point TR - but does not fully conform to it for several reasons.
// The major reasons are: it is well known that TR 24733 has to change before
// it is included into the C++ standard; the TR would require us to change
// system header files we do not have access to.
//
// In the following subsections the differences to the C++ technical report are
// explained in detail, including a short rationale.
//
///No Namespace Level Named Functions
/// - - - - - - - - - - - - - - - - -
// BDE design guidelines do not allow namespace level functions other than
// operators and aspects.  According to BDE design principles all such
// functions are placed into a utility component.
//
///All Converting Constructors from Integer Types are Explicit
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This change is necessary to disable the use of comparison operators without
// explicit casting.  See No Heterogeneous Comparisons Without Casting.
//
///No Heterogeneous Comparisons Without Casting
/// - - - - - - - - - - - - - - - - - - - - - -
// The C and C++ Decimal TRs refer to IEEE-754 for specifications of the
// heterogeneous comparison operators (comparing decimal floating-point types
// to binary floating-point types and integer types); however IEEE-754 does
// *not* specify such operations - leaving them unspecified.  To make matters
// worse, there are two possible ways to implement those operators (convert the
// decimal to the other type, or convert the other type to decimal first) and
// depending on which one is chosen, the result of the operator will be
// different.  Also, the C committee is considering the removal of those
// operators.  We have removed them until we know how to implement them.
// Comparing decimal types to those other types is still possible, it just
// requires explicit casting/conversion from the user code.
//
///No Arithmetic Or Computing Support For 'Decimal32'
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// IEEE-754 designates the 32 bit floating-point types "interchange formats"
// and does not require or recommend arithmetic or computing support of any
// kind for them.  The C (and consequently the C++) TR goes against the IEEE
// design and requires '_Decimal32' (and 'std::decimal32') to provide computing
// support, however, in a twist, allows it to be performed using one of the
// larger types (64 or 128 bits).  The rationale from the C committee is that
// small embedded systems may need to do their calculations using the small
// type (so they have made it mandatory for everyone).  Industry experience
// with the 'float' C type (32bit floating-point type, usually binary) has
// shown that enabling computing using small floating-point types are a mistake
// that causes novice programmers to write calculations that are very slow and
// inaccurate.  There we have decided that unless a compelling use case
// presents itself we are not going to make it easy to write slow and
// inaccurate programs using our implementation.
//
// We recommend what IEEE recommends: convert your 32 bit types on receipt to a
// type with higher precision (usually 64 bit will suffice), so you
// calculations using that larger type, and convert it back to 32 bit type only
// if your output interchange format requires it.
//
///Non-Standard Member Functions
///- - - - - - - - - - - - - - -
// Due to BDE rules of design and some implementation needs we have extended
// the C++ TR mandated interface of the decimal floating-point types to include
// support for accessing the underlying data (type), to parse literals for the
// portable literal support.
//
// Note that using any of these public member functions will render your code
// non-portable to non-BDE (but standards conforming) implementations.
//
///'Decimal32' Type
///----------------
// An interchange format type that supports input, output, relational operators
// construction from the TR mandates data types but no arithmetic or other
// computing operations.  The type has the size of exactly 32 bits.  It
// supports 7 significant decimal digits and an exponent range of -95 to 96.
// The smallest non-zero value that can be represented is 1e-101.
//
// Portable 'Decimal32' literals are created using the 'BDLDFP_DECIMAL_DF'
// macro.
//
///'Decimal64' Type
///----------------
// An basic format type that supports input, output, relational operators
// construction from the TR mandates data types and arithmetic or operations.
// The type has the size of exactly 64 bits.  It supports 16 significant
// decimal digits and an exponent range of -383 to 384.  The smallest non-zero
// value that can be represented is 1e-398.
//
// Portable 'Decimal64' literals are created using the 'BDLDFP_DECIMAL_DD'
// macro.
//
///'Decimal128' Type
///-----------------
// An basic format type that supports input, output, relational operators
// construction from the TR mandates data types and arithmetic or operations.
// The type has the size of exactly 128 bits.  It supports 34 significant
// decimal digits and an exponent range of -6143 to 6144.  The smallest
// non-zero value that can be represented is 1e-6176.
//
// Portable 'Decimal128' literals are created using the 'BDLDFP_DECIMAL_DL'
// macro.
//
///Decimal Number Formatting
///-------------------------
// Streaming decimal floating point nubmers to an output stream currently
// supports formatting flags for width, capitalization, and justification.
// Because of potential for future improvements in format handling, the
// operations should not be used for serialization.
//
///Preliminary Release: Known Bugs
///-------------------------------
// Stream output operators do not obey formatting flags or the precision
// algorithm as required by the TR; they instead print all significant digits
// all the time.
//
///Usage
///-----
// In this section, we show the intended usage of this component.
//
///Example 1: Portable Initialization of Non-Integer, Constant Values
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// If your compiler does not support the C Decimal TR, it does not support
// decimal floating-point literals, only binary floating-point literals.  The
// problem with binary floating-point literals is the same as with binary
// floating-point numbers in general: they cannot represent the decimal numbers
// we care about.  To solve this problem there are 3 macros provided by this
// component that can be used to initialize decimal floating-point types with
// non-integer values, precisely.  These macros will evaluate to real, C
// language literals where those are supported and to a runtime-parsed solution
// otherwise.  The following code demonstrates the use of these macros as well
// as mixed-type arithmetics and comparisons:
//..
//  bdldfp::Decimal32  d32( BDLDFP_DECIMAL_DF(0.1));
//  bdldfp::Decimal64  d64( BDLDFP_DECIMAL_DD(0.2));
//  bdldfp::Decimal128 d128(BDLDFP_DECIMAL_DL(0.3));
//
//  assert(d32 + d64 == d128);
//  assert(bdldfp::Decimal64(d32)  * 10 == bdldfp::Decimal64(1));
//  assert(d64  * 10 == bdldfp::Decimal64(2));
//  assert(d128 * 10 == bdldfp::Decimal128(3));
//..
//
///Example 2: Precise Calculations with Decimal Values
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to add two (decimal) numbers and then tell if the result is
// a particular decimal number or not.  That can get difficult with binary
// floating-point, but easy with decimal:
//..
//  if (std::numeric_limits<double>::radix == 2) {
//    assert(.1 + .2 != .3);
//  }
//  assert(BDLDFP_DECIMAL_DD(0.1) + BDLDFP_DECIMAL_DD(0.2)
//      == BDLDFP_DECIMAL_DD(0.3));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL
#include <bdldfp_decimalimputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#include <bsls_exceptionutil.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_LOCALE
#include <bsl_locale.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif


               // Portable decimal floating-point literal support

#define BDLDFP_DECIMAL_DF(lit)                                                \
    BloombergLP::bdldfp::Decimal32(BDLDFP_DECIMALIMPUTIL_DF(lit))

#define BDLDFP_DECIMAL_DD(lit)                                                \
    BloombergLP::bdldfp::Decimal64(BDLDFP_DECIMALIMPUTIL_DD(lit))

#define BDLDFP_DECIMAL_DL(lit)                                                \
    BloombergLP::bdldfp::Decimal128(BDLDFP_DECIMALIMPUTIL_DL(lit))

namespace BloombergLP {
namespace bdldfp {

                            // FORWARD DECLARATIONS

class Decimal_Type32;
class Decimal_Type64;
class Decimal_Type128;
    // These are the actual (decimal floating-point) types being implemented.
    // They use a different name to cause an error if the official types are
    // forward declared: The exact definition of the decimal types is left
    // unspecified so that that can potentially be aliases for built-in types.

typedef Decimal_Type32  Decimal32;
typedef Decimal_Type64  Decimal64;
typedef Decimal_Type128 Decimal128;
    // The decimal floating-point types are typedefs to the unspecified
    // implementation types.

                      // THE DECIMAL FLOATING-POINT TYPES

                            // ====================
                            // class Decimal_Type32
                            // ====================

class Decimal_Type32 {
    // This value-semantic class implements the IEEE-754 32 bit decimal
    // floating-point interchange format type.  This class is a standard layout
    // type that is 'const' thread-safe and exception-neutral.

  private:
    // DATA
    DecimalImpUtil::ValueType32 d_value; // The underlying IEEE representation

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Decimal_Type32,
                                 bslalg::TypeTraitBitwiseCopyable);

    // CREATORS
    Decimal_Type32();
        // Create a 'Decimal32_Type' object having the value positive zero, and
        // 0 exponent (quantum 1e-6).

    Decimal_Type32(DecimalImpUtil::ValueType32 value);              // IMPLICIT
        // Create a 'Decimal32_Type' object having the specified 'value'.

    explicit Decimal_Type32(Decimal_Type64 other);
        // Create a 'Decimal32_Type' object having the value closest to the
        // value of the specified 'other' following the conversion rules
        // defined by IEEE-754:
        //
        //: o If 'other' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise if 'other' is infinity (positive or negative), then
        //:   initialize this object to infinity with the same sign.
        //:
        //: o Otherwise if 'other' has a zero value, then initialize this
        //:   object to zero with the same sign.
        //:
        //: o Otherwise if 'other' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal32>::max()' then raise the "overflow"
        //:   floating-point exception and initialize this object to infinity
        //:   with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal32>::min()' then raise the
        //:   "underflow" floating-point exception and initialize this object
        //:   to zero with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has a value that has more significant digits
        //:   than 'std::numeric_limits<Decimal32>::max_digit' then raise the
        //:   "inexact" floating-point exception and initialize this object to
        //:   the value of 'other' rounded according to the rounding direction.
        //:
        //: o Otherwise initialize this object to the value of the 'other'.
        //
        // TODO: We may wish for a constructor from Decimal128.  The rounding
        // is not supported by IBM software.

    explicit Decimal_Type32(float       other);
    explicit Decimal_Type32(double      other);
        // Create a 'Decimal32_Type' object having the value closest to the
        // value of the specified 'other' value.  *Warning:* clients requiring
        // a conversion for an exact decimal value should use
        // 'bdldfp_decimalconverutil' (see *WARNING*: Conversions from
        // 'float' and 'double'}.  This conversion follows the conversion
        // rules as defined by IEEE-754:
        //
        //: o If 'other' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise if 'other' is infinity (positive or negative), then
        //:   initialize this object to infinity value with the same sign.
        //:
        //: o Otherwise if 'other' has a zero value, then initialize this
        //:   object to zero with the same sign.
        //:
        //: o Otherwise if 'other' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal32>::max()' then raise the "overflow"
        //:   floating-point exception and initialize this object to infinity
        //:   with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal32>::min()' then raise the
        //:   "underflow" floating-point exception and initialize this object
        //:   to zero with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has a value that needs more than
        //:   'std::numeric_limits<Decimal32>::max_digit' significant decimal
        //:   digits to represent then raise the "inexact" floating-point
        //:   exception and initialize this object to the value of 'other'
        //:   rounded according to the rounding direction.
        //:
        //: o Otherwise initialize this object to the value of the 'other'.

    explicit Decimal_Type32(int                other);
    explicit Decimal_Type32(unsigned int       other);
    explicit Decimal_Type32(long int           other);
    explicit Decimal_Type32(unsigned long int  other);
    explicit Decimal_Type32(long long          other);
    explicit Decimal_Type32(unsigned long long other);
        // Create a 'Decimal32_Type' object having the value closest to the
        // value of the specified 'other' following the conversion rules as
        // defined by IEEE-754:
        //
        //: o If 'other' is zero then initialize this object to positive zero
        //:   with a 0 exponent (quantum 1e-6).
        //:
        //: o Otherwise if 'other' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal32>::max()' then raise the "overflow"
        //:   floating-point exception and initialize this object to infinity
        //:   with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal32>::max_digit'
        //:   decimal digits then raise the "inexact" floating-point exception
        //:   and initialize this object to the value of 'other' rounded
        //:   according to the rounding direction.
        //:
        //: o Otherwise initialize this object to the value of the 'other'.
        //
        // The exponent 0 (quantum 1e-6) is preferred during conversion unless
        // it would cause unnecessary loss of precision.

    //! Decimal32_Type(const Decimal32_Type& original) = default;
        // Create a 'Decimal32_Type' object that is a copy of the specified
        // 'original' as defined by the 'copy' operation of IEEE-754 2008:
        //
        //: o If 'other' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise initialize this object to the value of the 'other'.
        //
        // Note that since floating-point types may be NaN, and NaNs are
        // unordered (do not compare equal even to themselves) it is possible
        // that a copy of a decimal will not compare equal to the original;
        // however it will behave as the original.

    //! ~Decimal32_Type() = default;
        // Destroy this object.

    // MANIPULATORS
    //! Decimal32_Type& operator=(const Decimal32_Type& rhs) = default;
        // Make this object a copy of the specified 'rhs' as defined by the
        // 'copy' operation of IEEE-754 2008 and return a reference providing
        // modifiable access to this object.
        //
        //: o If 'other' is NaN, set this object to a NaN.
        //:
        //: o Otherwise set this object to the value of the 'other'.
        //
        // Note that since floating-point types may be NaN, and NaNs are
        // unordered (do not compare equal even to themselves) it is possible
        // that, after an assignment, a decimal will not compare equal to the
        // original; however it will behave as the original.

    DecimalImpUtil::ValueType32 *data();
        // Return a pointer providing modifiable access to the underlying
        // implementation.

    // ACCESSORS
    const DecimalImpUtil::ValueType32 *data() const;
        // Return a pointer providing non-modifiable access to the underlying
        // implementation.

    DecimalImpUtil::ValueType32 value() const;
        // Return the value of the underlying implementation.
};

// FREE OPERATORS
Decimal32 operator+(Decimal32 value);
    // Return a copy of the specified 'value' if the value is not negative
    // zero, and return positive zero otherwise.

Decimal32 operator-(Decimal32 value);
    // Return the result of applying the unary - operator to the specified
    // 'value' as described by IEEE-754, essentially reversing the sign bit.
    // Note that floating-point numbers have signed zero, so this operation is
    // not the same as '0 - value'.

bool operator==(Decimal32 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'Decimal32' objects have the same value if the
    // 'compareQuietEqual' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representations equal.  In
    // other words, two 'Decimal32' objects have the same value if:
    //
    //: o both have a zero value (positive or negative), or
    //: o both have the same infinity value (both positive or negative), or
    //: o both have the value of a real number that are equal, even if they are
    //:   represented differently (cohorts have the same value)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.
    //
    // Note that a NaN is never equal to anything, including itself:
    //..
    //  Decimal32 aNaN = std::numeric_limits<Decimal32>::quiet_NaN();
    //  assert(!(aNan == aNan));
    //..

bool operator!=(Decimal32 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'Decimal32' objects do not have the
    // same value if the 'compareQuietEqual' operation (IEEE-754 defined,
    // non-total ordering comparison) considers the underlying IEEE
    // representations not equal.  In other words, two 'Decimal32' objects do
    // not have the same value if:
    //
    //: o both are NaN, or
    //: o one is zero (positive or negative) and the is not, or
    //: o one is positive infinity and the other is not, or
    //: o one is negative infinity and the other is not, or
    //: o both have the value of a real number that are not equal, regardless
    //:   of their representation (cohorts are equal)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.
    //
    // Note that a NaN is never equal to anything, including itself:
    //..
    //  Decimal32 aNaN = std::numeric_limits<Decimal32>::quiet_NaN();
    //  assert(aNan != aNan);
    //..

bool operator<(Decimal32 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs' and 'false' otherwise.  The value of a 'Decimal32' object 'lhs' is
    // less than that of an object 'rhs' if the 'compareQuietLess' operation
    // (IEEE-754 defined, non-total ordering comparison) considers the
    // underlying IEEE representation of 'lhs' to be less than of that of
    // 'rhs'.  In other words, 'lhs' is less than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' is zero (positive or negative) and 'rhs' positive, or
    //: o 'rhs' is zero (positive or negative) and 'lhs' negative, or
    //: o 'lhs' is not positive infinity, or
    //: o 'lhs' is negative infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is less than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator<=(Decimal32 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' has a value less than or equal the
    // value of the specified 'rhs' and 'false' otherwise.  The value of a
    // 'Decimal32' object 'lhs' is less than or equal to the value of an object
    // 'rhs' if the 'compareQuietLessEqual' operation (IEEE-754 defined,
    // non-total ordering comparison) considers the underlying IEEE
    // representation of 'lhs' to be less or equal to that of 'rhs'.  In other
    // words, 'lhs' is less or equal than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are positive infinity, or
    //: o 'lhs' is negative infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is less or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>(Decimal32 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' has a greater value than the
    // specified 'rhs' and 'false' otherwise.  The value of a 'Decimal32'
    // object 'lhs' is greater than that of an object 'rhs' if the
    // 'compareQuietGreater' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representation of 'lhs' to be
    // greater than of that of 'rhs'.  In other words, 'lhs' is greater than
    // 'rhs'if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are not both zero (positive or negative), or
    //: o 'lhs' is not negative infinity, or
    //: o 'lhs' is positive infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>=(Decimal32 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' has a value greater than or equal
    // to the value of the specified 'rhs' and 'false' otherwise.  The value of
    // a 'Decimal32' object 'lhs' is greater or equal to a 'Decimal32' object
    // 'rhs' if the 'compareQuietGreaterEqual' operation (IEEE-754 defined,
    // non-total ordering comparison ) considers the underlying IEEE
    // representation of 'lhs' to be greater or equal to that of 'rhs'.  In
    // other words, 'lhs' is greater than or equal to 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are negative infinity, or
    //: o 'lhs' is positive infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
operator>>(bsl::basic_istream<CHARTYPE, TRAITS>& stream, Decimal32& object);
    // Read, into the specified 'object', from the specified input 'stream' an
    // IEEE 32 bit decimal floating-point value as described in the IEEE-754
    // 2008 standard (5.12 Details of conversions between floating point
    // numbers and external character sequences) and return a reference
    // providing modifiable access to 'stream'.  If 'stream' contains a NaN
    // value, it is unspecified if 'object' will receive a quiet or signaling
    // 'Nan'.  If 'stream' is not valid on entry 'stream.good() == false', this
    // operation has no effect other than setting 'stream.fail()' to 'true'.
    // If eof (end-of-file) is found before any non-whitespace characters
    // 'stream.fail()' is set to 'true' and 'object' remains unchanged.  If eof
    // is detected after some characters have been read (and successfully
    // interpreted as part of the textual representation of a floating-point
    // value as specified by IEEE-754) then 'stream.eof()' is set to true.  If
    // the first non-whitespace character sequence is not a valid textual
    // representation of a floating-point value (e.g., 12e or e12 or 1*2) the
    // 'stream.fail()' is set to true and 'object' will remain unchanged.  If a
    // real number value is represented by the character sequence but it is a
    // large positive or negative value that cannot be stored into 'object' the
    // "overflow" floating-point exception is raised and positive or negative
    // infinity is stored into 'object', respectively.  If a real number value
    // is represented by the character sequence but it is a small positive or
    // negative value that cannot be stored into 'object' the "underflow"
    // floating-point exception is raised and positive or negative zero is
    // stored into 'object', respectively.  If a real number value is
    // represented by the character sequence but it cannot be stored exactly
    // into 'object' the "inexact" floating-point exception is raised, the
    // value is rounded according to the current rounding direction (of the
    // environment) and then stored into 'object.'
    //
    // NOTE: This method does not yet fully support iostream flags or the
    // decimal floating point exception context.

template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
operator<<(bsl::basic_ostream<CHARTYPE, TRAITS>& stream, Decimal32 object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single line format as described in the IEEE-754 2008
    // standard (5.12 Details of conversions between floating point numbers and
    // external character sequences), and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.
    //
    // NOTE: This method does not yet fully support iostream flags or the
    // decimal floating point exception context.

                            // ====================
                            // class Decimal_Type64
                            // ====================

class Decimal_Type64 {
    // This value-semantic class implements the IEEE-754 64 bit decimal
    // floating-point format arithmetic type.  This class is a standard layout
    // type that is 'const' thread-safe and exception-neutral.

  private:
    // DATA
    DecimalImpUtil::ValueType64 d_value; // The underlying IEEE representation

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Decimal_Type64,
                                 bslalg::TypeTraitBitwiseCopyable);

    // CREATORS
    Decimal_Type64();
        // Create a 'Decimal64_Type' object having the value positive zero, and
        // 0 exponent (quantum 1e-15).

    Decimal_Type64(DecimalImpUtil::ValueType64 value);              // IMPLICIT
        // Create a 'Decimal64_Type' object having the specified 'value'.

    Decimal_Type64(Decimal32 other);                                // IMPLICIT
        // Create a 'Decimal64_Type' object having the value of the specified
        // 'other' following the conversion rules defined by IEEE-754:
        //
        //: o If 'other' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise if 'other' is infinity (positive or negative), then
        //:   initialize this object to infinity with the same sign.
        //:
        //: o Otherwise if 'other' is zero, then initialize this object to zero
        //:   with the same sign.
        //:
        //: o Otherwise initialize this object to the value of the 'other'.

    explicit Decimal_Type64(Decimal128 other);
        // Create a 'Decimal64_Type' object having the value closest to the
        // value of the specified 'other' following the conversion rules
        // defined by IEEE-754:
        //
        //: o If 'other' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise if 'other' is infinity (positive or negative), then
        //:   initialize this object to infinity with the same sign.
        //:
        //: o Otherwise if 'other' is zero, then initialize this object to
        //:   zero with the same sign.
        //:
        //: o Otherwise if 'other' has an absolute value that is larger
        //:   than 'std::numeric_limits<Decimal64>::max()' then raise the
        //:   "overflow" floating-point exception and initialize this
        //:   object to infinity with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has an absolute value that is smaller
        //:   than 'std::numeric_limits<Decimal64>::min()' then raise the
        //:   "underflow" floating-point exception and initialize this
        //:   object to zero with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has a value that has more significant
        //:   digits than 'std::numeric_limits<Decimal64>::max_digit' then
        //:   raise the "inexact" floating-point exception and initialize
        //:   this object to the value of 'other' rounded according to the
        //:   rounding direction.
        //:
        //: o Otherwise initialize this object to the value as the 'other'.

    explicit Decimal_Type64(float       other);
    explicit Decimal_Type64(double      other);
        // Create a 'Decimal64_Type' object having the value closest to the
        // value of the specified 'other' value.  *Warning:* clients requiring
        // a conversion for an exact decimal value should use
        // 'bdldfp_decimalconverutil' (see *WARNING*: Conversions from
        // 'float' and 'double'}.  This conversion follows the conversion
        // rules as defined by IEEE-754:
        //
        //: o If 'other' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise if 'other' is infinity (positive or negative), then
        //:   initialize this object to infinity value with the same sign.
        //:
        //: o Otherwise if 'other' has a zero value, then initialize this
        //:   object to zero with the same sign.
        //:
        //: o Otherwise if 'other' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and initialize this object to infinity
        //:   with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal64>::min()' then raise the
        //:   "underflow" floating-point exception and initialize this object
        //:   to zero with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has a value that needs more than
        //:   'std::numeric_limits<Decimal64>::max_digit' significant decimal
        //:   digits to represent then raise the "inexact" floating-point
        //:   exception and initialize this object to the value of 'other'
        //:   rounded according to the rounding direction.
        //:
        //: o Otherwise initialize this object to the value of the 'other'.

    explicit Decimal_Type64(int                other);
    explicit Decimal_Type64(unsigned int       other);
    explicit Decimal_Type64(long               other);
    explicit Decimal_Type64(unsigned long      other);
    explicit Decimal_Type64(long long          other);
    explicit Decimal_Type64(unsigned long long other);
        // Create a 'Decimal64_Type' object having the value closest to the
        // value of the specified 'other' following the conversion rules as
        // defined by IEEE-754:
        //
        //: o If 'other' is zero then initialize this object to positive zero
        //:   with a 0 exponent (quantum 1e-15).
        //:
        //: o Otherwise if 'other' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and initialize this object to infinity
        //:   with the same sign as 'other'.
        //:
        //: o Otherwise if 'other' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal64>::max_digit'
        //:   decimal digits then raise the "inexact" floating-point exception
        //:   and initialize this object to the value of 'other' rounded
        //:   according to the rounding direction.
        //:
        //: o Otherwise initialize this object to the value of the 'other'.
        //
        // The exponent 0 (quantum 1e-15) is preferred during conversion unless
        // it would cause unnecessary loss of precision.

    //! Decimal64_Type(const Decimal64_Type& original) = default;
        // Create a 'Decimal64_Type' object that is a copy of the specified
        // 'original' as defined by the 'copy' operation of IEEE-754 2008:
        //
        //: o If 'other' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise initialize this object to the value of the 'other'.
        //
        // Note that since floating-point types may be NaN, and NaNs are
        // unordered (do not compare equal even to themselves) it is possible
        // that a copy of a decimal will not compare equal to the original;
        // however it will behave as the original.

    //! ~Decimal64_Type() = default;
        // Destroy this object.

    // MANIPULATORS
    //! Decimal64_Type& operator=(const Decimal64_Type& rhs) = default;
        // Make this object a copy of the specified 'rhs' as defined by the
        // 'copy' operation of IEEE-754 2008 and return a reference providing
        // modifiable access to this object.
        //
        //: o If 'other' is NaN, set this object to a NaN.
        //:
        //: o Otherwise set this object to the value of the 'other'.
        //
        // Note that since floating-point types may be NaN, and NaNs are
        // unordered (do not compare equal even to themselves) it is possible
        // that, after an assignment, a decimal will not compare equal to the
        // original; however it will behave as the original.

    Decimal_Type64& operator++();
        // Add 1.0 to the value of this object and return a reference to it.
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to just set it to 1.0 (if the original value is small).

    Decimal_Type64& operator--();
        // Add -1.0 to the value of this object and return a reference to it.
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to just set it to -1.0 (if the original value is small).

    Decimal_Type64& operator+=(Decimal32  rhs);
    Decimal_Type64& operator+=(Decimal64  rhs);
    Decimal_Type64& operator+=(Decimal128 rhs);
        // Add the value of the specified 'rhs' object to the value of this as
        // described by IEEE-754, store the result in this object, and return a
        // reference to this object.
        //
        //: o If either this object or 'rhs' is NaN, then raise the "invalid"
        //:   floating-point exception and set this object to a NaN.
        //:
        //: o Otherwise if this object and 'rhs' have infinite values of
        //:   differing signs, then raise the "invalid" floating-point
        //:   exception and set this object to a NaN.
        //:
        //: o Otherwise if this object and 'rhs' have infinite values of the
        //:   same sign, then do not change this object.
        //:
        //: o Otherwise if 'rhs' has a zero value (positive or negative), do
        //:   not change this object.
        //:
        //: o Otherwise if the sum of this object and 'rhs' has an absolute
        //:   value that is larger than 'std::numeric_limits<Decimal64>::max()'
        //:   then raise the "overflow" floating-point exception and set this
        //:   object to infinity value with the same sign as that result.
        //:
        //: o Otherwise set this object to the sum of the number represented by
        //:   'rhs' and the number represented by this object.
        //
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to update it to the value of the 'other' (if the original
        // value is small).
        //
        // Note that when 'rhs' is a 'Decimal128', this operation is always
        // performed with 128 bits precision to prevent loss of precision of
        // the 'rhs' operand (prior to the operation).  The result is then
        // rounded back to 64 bits and stored to this object.  See IEEE-754
        // 2008, 5.1, first paragraph, second sentence for specification.

    Decimal_Type64& operator+=(int                rhs);
    Decimal_Type64& operator+=(unsigned int       rhs);
    Decimal_Type64& operator+=(long               rhs);
    Decimal_Type64& operator+=(unsigned long      rhs);
    Decimal_Type64& operator+=(long long          rhs);
    Decimal_Type64& operator+=(unsigned long long rhs);
        // Add the specified 'rhs' to the value of this object as described by
        // IEEE-754, store the result in this object, and return a reference to
        // this object.
        //
        //: o If this object is NaN, then raise the "invalid" floating-point
        //:   exception and do not change this object.
        //:
        //: o Otherwise if this object is infinity, then do not change it.
        //:
        //: o Otherwise if the sum of this object and 'rhs' has an absolute
        //:   value that is larger than 'std::numeric_limits<Decimal64>::max()'
        //:   then raise the "overflow" floating-point exception and set this
        //:   object to infinity with the same sign as that result.
        //:
        //: o Otherwise set this object to sum of adding 'rhs' and the number
        //:   represented by this object.
        //
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to update it to the value of the 'other' (if the original
        // value is small).

    Decimal_Type64& operator-=(Decimal32  rhs);
    Decimal_Type64& operator-=(Decimal64  rhs);
    Decimal_Type64& operator-=(Decimal128 rhs);
        // Subtract the value of the specified 'rhs' from the value of this
        // object as described by IEEE-754, store the result in this object,
        // and return a reference to this object.
        //
        //: o If either this object or 'rhs' is NaN, then raise the "invalid"
        //:   floating-point exception and set this object to a NaN.
        //:
        //: o Otherwise if this object and the 'rhs' have infinite values of
        //:   the same sign, raise the "invalid" floating-point exception and
        //:   set this object to a NaN.
        //:
        //: o Otherwise if this object and the 'rhs' have infinite values of
        //:   differing signs, then do not change this object.
        //:
        //: o Otherwise if the 'rhs' has a zero value (positive or negative),
        //:   do not change this object.
        //:
        //: o Otherwise if subtracting the value of the 'rhs' object from this
        //:   results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and set this object to infinity with the
        //:   same sign as that result.
        //:
        //: o Otherwise set this object to the result of subtracting the value
        //:   of 'rhs' from the value of this object.
        //
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to update it to the value of the 'other' (if the original
        // value is small).
        //
        // Note that when 'rhs' is a 'Decimal128', this operation is always
        // performed with 128 bits precision to prevent loss of precision of
        // the 'rhs' operand (prior to the operation).  The result is then
        // rounded back to 64 bits and stored to this object.  See IEEE-754
        // 2008, 5.1, first paragraph, second sentence for specification.

    Decimal_Type64& operator-=(int                rhs);
    Decimal_Type64& operator-=(unsigned int       rhs);
    Decimal_Type64& operator-=(long               rhs);
    Decimal_Type64& operator-=(unsigned long      rhs);
    Decimal_Type64& operator-=(long long          rhs);
    Decimal_Type64& operator-=(unsigned long long rhs);
        // Subtract the specified 'rhs' from the value of this object as
        // described by IEEE-754, store the result in this object, and return a
        // reference to this object.
        //
        //: o If this object is NaN, raise the "invalid" floating-point
        //:   exception and do not change this object.
        //:
        //: o Otherwise if this object is infinity, then do not change it.
        //:
        //: o Otherwise if subtracting 'rhs' from this object's value results
        //:   in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and set this object to infinity with the
        //:   same sign as that result.
        //:
        //: o Otherwise set this object to the result of subtracting 'rhs' from
        //:   the value of this object.
        //
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to update it to the value of the 'other' (if the original
        // value is small).

    Decimal_Type64& operator*=(Decimal32  rhs);
    Decimal_Type64& operator*=(Decimal64  rhs);
    Decimal_Type64& operator*=(Decimal128 rhs);
        // Multiply the value of the specified 'rhs' object by the value of
        // this as described by IEEE-754, store the result in this object, and
        // return a reference to this object.
        //
        //: o If either this object or 'rhs' is NaN, raise the "invalid"
        //:   floating-point exception and set this object to NaN.
        //:
        //: o Otherwise, if one of this object and 'rhs' is zero (positive or
        //:   negative) and the other is infinity (positive or negative), raise
        //:   the "invalid" floating-point exception and set this object to a
        //:   NaN.
        //:
        //: o Otherwise, if either this object or 'rhs' is positive or negative
        //:   infinity, set this object to infinity.  The sign of this object
        //:   will be positive if this object and 'rhs' had the same sign, and
        //:   negative otherwise.
        //:
        //: o Otherwise, if either this object or 'rhs' is zero, set this
        //:   object to zero.  The sign of this object will be positive if this
        //:   object and 'rhs' had the same sign, and negative otherwise.
        //:
        //: o Otherwise if the product of this object and 'rhs' has an absolute
        //:   value that is larger than 'std::numeric_limits<Decimal64>::max()'
        //:   then raise the "overflow" floating-point exception and set this
        //:   object to infinity with the same sign of that result.
        //:
        //: o Otherwise if the product of this object and 'rhs' has an absolute
        //:   value that is smaller than
        //:   'std::numeric_limits<Decimal64>::min()' then raise the
        //:   "underflow" floating-point exception and set this object to zero
        //:   value with the same sign as that result.
        //:
        //: o Otherwise set this object to the product of the value of 'rhs'
        //:   and the value of this object.
        //
        // Note that when 'rhs' is a 'Decimal128', this operation is always
        // performed with 128 bits precision to prevent loss of precision of
        // the 'rhs' operand (prior to the operation).  The result is then
        // rounded back to 64 bits and stored to this object.  See IEEE-754
        // 2008, 5.1, first paragraph, second sentence for specification.

    Decimal_Type64& operator*=(int                rhs);
    Decimal_Type64& operator*=(unsigned int       rhs);
    Decimal_Type64& operator*=(long               rhs);
    Decimal_Type64& operator*=(unsigned long      rhs);
    Decimal_Type64& operator*=(long long          rhs);
    Decimal_Type64& operator*=(unsigned long long rhs);
        // Multiply the specified 'rhs' by the value of this object as
        // described by IEEE-754, store the result in this object, and return a
        // reference to this object.
        //
        //: o If this object is NaN, raise the "invalid" floating-point
        //:   exception and set this object to a NaN.
        //:
        //: o Otherwise if this object is infinity (positive or negative), and
        //:   'rhs' is zero, then raise the "invalid" floating-point exception
        //:   and set this object to a NaN.
        //:
        //: o Otherwise if this object is infinity (positive or negative), then
        //:   do not change it.
        //:
        //: o Otherwise if 'rhs' is zero, then set this object to zero with the
        //:   same sign as its value had prior to this operation.
        //:
        //: o Otherwise if the product of 'rhs' and the value of this object
        //:   results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and set this object to infinity with the
        //:   same sign as that result.
        //:
        //: o Otherwise if the product of 'rhs' and the value of this object
        //:   results in an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal64>::min()' then raise the
        //:   "underflow" floating-point exception and set this object to zero
        //:   with the same sign as that result.
        //:
        //: o Otherwise set this object to the product of the value of this
        //:   object and the value 'rhs'.

    Decimal_Type64& operator/=(Decimal32  rhs);
    Decimal_Type64& operator/=(Decimal64  rhs);
    Decimal_Type64& operator/=(Decimal128 rhs);
        // Divide the value of this object by the value of the specified 'rhs'
        // as described by IEEE-754, store the result in this object, and
        // return a reference to this object.
        //
        //: o If either this object or 'rhs' is NaN, then raise the "invalid"
        //:   floating-point exception and set this object to a NaN.
        //:
        //: o Otherwise if this object and 'rhs' are infinity (positive or
        //:   negative) or zero (positive or negative), then raise the
        //:   "invalid" floating-point exception and set this object to a NaN.
        //:
        //: o Otherwise if 'rhs' has a positive zero value, then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the same sign as its original value.
        //:
        //: o Otherwise if 'rhs' has a negative zero value, then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the opposite sign as its original value.
        //:
        //: o Otherwise if dividing the value of this object with the value of
        //:   'rhs' results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and set this object to infinity value
        //:   with the same sign as that result.
        //:
        //: o Otherwise if dividing the value of this object with the value of
        //:   'rhs' results in an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal64>::min()' then raise the
        //:   "underflow" floating-point exception and set this object to zero
        //:   with the same sign as that result.
        //:
        //: o Otherwise set this object to the result of dividing the value of
        //:   this object with the value of 'rhs'.
        //
        // Note that when 'rhs' is a 'Decimal128', this operation is always
        // performed with 128 bits precision to prevent loss of precision of
        // the 'rhs' operand (prior to the operation).  The result is then
        // rounded back to 64 bits and stored to this object.  See IEEE-754
        // 2008, 5.1, first paragraph, second sentence for specification.

    Decimal_Type64& operator/=(int                rhs);
    Decimal_Type64& operator/=(unsigned int       rhs);
    Decimal_Type64& operator/=(long               rhs);
    Decimal_Type64& operator/=(unsigned long      rhs);
    Decimal_Type64& operator/=(long long          rhs);
    Decimal_Type64& operator/=(unsigned long long rhs);
        // Divide the value of this object by the specified 'rhs' as described
        // by IEEE-754, store the result in this object, and return a reference
        // to this object.
        //
        //: o If this object is NaN, raise the "invalid" floating-point
        //:   exception and set this object to a NaN.
        //:
        //: o Otherwise if this object is infinity (positive or negative), and
        //:   'rhs' is zero, raise the "invalid" floating-point exception and
        //:   set this object to a NaN.
        //:
        //: o Otherwise if this object is infinity, then do not change it.
        //:
        //: o Otherwise if 'rhs' is zero, raise the "overflow" floating-point
        //:   exception and then set this object to infinity with the same sign
        //:   it had prior to this operation.
        //:
        //: o Otherwise if the quotient of the value of this object and 'rhs'
        //:   results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
        //:   floating-point exception and set this object to infinity with the
        //:   same sign as that result.
        //:
        //: o Otherwise if the quotient of the value of this object and 'rhs'
        //:   results in an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal64>::min()' then raise the
        //:   "underflow" floating-point exception and set this object to zero
        //:   with the same sign as that result.
        //:
        //: o Otherwise set this object to the result of dividing the number
        //:   represented by this object by 'rhs'.

    DecimalImpUtil::ValueType64 *data();
        // Return a modifiable pointer to the underlying implementation.

    // ACCESSORS
    const DecimalImpUtil::ValueType64 *data() const;
        // Return a non-modifiable pointer to the underlying implementation.

    DecimalImpUtil::ValueType64 value() const;
        // Return the value of the underlying implementation.
};

// FREE OPERATORS
Decimal64 operator+(Decimal64 value);
    // Return a copy of the specified 'value'.

Decimal64 operator-(Decimal64 value);
    // Return the result of applying the unary - operator to the specified
    // 'value' as described by IEEE-754.  Note that floating-point numbers have
    // signed zero, therefore this operation is not the same as '0-value'.

Decimal64 operator++(Decimal64& value, int);
    // Apply the prefix ++ operator to the specified 'value' and return its
    // original value.  Note that this is a floating-point value so this
    // operations may not change the value of this object at all (if the value
    // is large) or it may seem to just set it to 1.0 (if the original value is
    // small).

Decimal64 operator--(Decimal64& value, int);
    // Apply the prefix -- operator to the specified 'value' and return its
    // original value.  Note that this is a floating-point value so this
    // operations may not change the value of this object at all (if the value
    // is large) or it may seem to just set it to 1.0 (if the original value is
    // small).

Decimal64 operator+(Decimal64 lhs, Decimal64 rhs);
Decimal64 operator+(Decimal32 lhs, Decimal64 rhs);
Decimal64 operator+(Decimal64 lhs, Decimal32 rhs);
    // Add the value of the specified 'rhs' to the value of the specified 'lhs'
    // as described by IEEE-754 and return the result.
    //
    //: o If either of 'lhs' or 'rhs' is NaN, then raise the "invalid"
    //:   floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' and 'rhs' are infinities of differing signs, raise
    //:   the "invalid" floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' and 'rhs' are infinities of the same sign then
    //:   return infinity of that sign.
    //:
    //: o Otherwise if 'rhs' is zero (positive or negative), return 'lhs'.
    //:
    //: o Otherwise if the sum of 'lhs' and 'rhs' has an absolute value that is
    //:   larger than 'std::numeric_limits<Decimal64>::max()' then raise the
    //:   "overflow" floating-point exception and return infinity with the same
    //:   sign as that result.
    //:
    //: o Otherwise return the sum of the number represented by 'lhs' and the
    //:   number represented by 'rhs'.

Decimal64 operator+(Decimal64 lhs, int                rhs);
Decimal64 operator+(Decimal64 lhs, unsigned int       rhs);
Decimal64 operator+(Decimal64 lhs, long               rhs);
Decimal64 operator+(Decimal64 lhs, unsigned long      rhs);
Decimal64 operator+(Decimal64 lhs, long long          rhs);
Decimal64 operator+(Decimal64 lhs, unsigned long long rhs);
    // Add the specified 'rhs' to the value of the specified 'lhs' as described
    // by IEEE-754 and return the result.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity, then return that infinity.
    //:
    //: o Otherwise if the sum of 'lhs' and 'rhs' has an absolute value that is
    //:   larger than 'std::numeric_limits<Decimal64>::max()' then raise the
    //:   "overflow" floating-point exception and return infinity with the same
    //:   sign as that result.
    //:
    //: o Otherwise return the sum of 'rhs' and the number represented by
    //:   'lhs'.

Decimal64 operator+(int                lhs, Decimal64 rhs);
Decimal64 operator+(unsigned int       lhs, Decimal64 rhs);
Decimal64 operator+(long               lhs, Decimal64 rhs);
Decimal64 operator+(unsigned long      lhs, Decimal64 rhs);
Decimal64 operator+(long long          lhs, Decimal64 rhs);
Decimal64 operator+(unsigned long long lhs, Decimal64 rhs);
    // Add the specified 'lhs' to the value of the specified 'rhs' as described
    // by IEEE-754 and return the result.
    //
    //: o If 'rhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'rhs' is infinity, then return that infinity.
    //:
    //: o Otherwise if the sum of the value of 'rhs' and 'lhs' has an absolute
    //:   value that is larger than 'std::numeric_limits<Decimal64>::max()'
    //:   then raise the "overflow" floating-point exception and return
    //:   infinity with the same sign as that result.
    //:
    //: o Otherwise return the sum of 'lhs' and the number represented by
    //:   'rhs'.

Decimal64 operator-(Decimal64 lhs, Decimal64 rhs);
Decimal64 operator-(Decimal32 lhs, Decimal64 rhs);
Decimal64 operator-(Decimal64 lhs, Decimal32 rhs);
    // Subtract the value of the specified 'rhs' from the value of the
    // specified 'lhs' as described by IEEE-754 and return the result.
    //
    //: o If either 'lhs' or 'rhs' is NaN, then raise the "invalid"
    //:   floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' and the 'rhs' have infinity values of the same
    //:   sign, then raise the "invalid" floating-point exception and return a
    //:   NaN.
    //:
    //: o Otherwise if 'lhs' and the 'rhs' have infinity values of differing
    //:   signs, then return 'lhs'.
    //:
    //: o Otherwise if 'rhs' has a zero value (positive or negative), then
    //:   return 'lhs'.
    //:
    //: o Otherwise if subtracting the value of the 'rhs' object from the value
    //:   of 'lhs' results in an absolute value that is larger than
    //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise return the result of subtracting the value of 'rhs' from
    //:   the value of 'lhs'.

Decimal64 operator-(Decimal64 lhs, int                rhs);
Decimal64 operator-(Decimal64 lhs, unsigned int       rhs);
Decimal64 operator-(Decimal64 lhs, long               rhs);
Decimal64 operator-(Decimal64 lhs, unsigned long      rhs);
Decimal64 operator-(Decimal64 lhs, long long          rhs);
Decimal64 operator-(Decimal64 lhs, unsigned long long rhs);
    // Subtract the specified 'rhs' from the value of the specified 'lhs' as
    // described by IEEE-754 and return a reference to this object.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity, then return 'lhs'.
    //:
    //: o Otherwise if subtracting 'rhs' from the value of 'lhs' results in an
    //:   absolute value that is larger than
    //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise return the result of subtracting 'rhs' from the value of
    //:   'lhs'.

Decimal64 operator-(int                lhs, Decimal64 rhs);
Decimal64 operator-(unsigned int       lhs, Decimal64 rhs);
Decimal64 operator-(long               lhs, Decimal64 rhs);
Decimal64 operator-(unsigned long      lhs, Decimal64 rhs);
Decimal64 operator-(long long          lhs, Decimal64 rhs);
Decimal64 operator-(unsigned long long lhs, Decimal64 rhs);
    // Subtract the specified 'rhs' from the value of the specified 'lhs' as
    // described by IEEE-754 and return a reference to this object.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception return
    //:   a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity, then return 'lhs'.
    //:
    //: o Otherwise if 'rhs' is zero (positive or negative), then return 'lhs'.
    //:
    //: o Otherwise if subtracting 'rhs' from the value of 'lhs' results in an
    //:   absolute value that is larger than
    //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise return the result of subtracting the value of 'rhs'
    // from the number 'lhs'.

Decimal64 operator*(Decimal64 lhs, Decimal64 rhs);
Decimal64 operator*(Decimal32 lhs, Decimal64 rhs);
Decimal64 operator*(Decimal64 lhs, Decimal32 rhs);
    // Multiply the value of the specified 'lhs' object by the value of the
    // specified 'rhs' as described by IEEE-754 and return the result.
    //
    //: o If either of 'lhs' or 'rhs' is NaN, return a NaN.
    //:
    //: o Otherwise if one of the operands is infinity (positive or negative)
    //:   and the other is zero (positive or negative), then raise the
    //:   "invalid" floating-point exception raised and return a NaN.
    //:
    //: o Otherwise if both 'lhs' and 'rhs' are infinity (positive or
    //:   negative), return infinity.  The sign of the returned value will be
    //:   positive if 'lhs' and 'rhs' have the same sign, and negative
    //:   otherwise.
    //:
    //: o Otherwise, if either 'lhs' or 'rhs' is zero, return zero.  The sign
    //:   of the returned value will be positive if 'lhs' and 'rhs' have the
    //:   same sign, and negative otherwise.
    //:
    //: o Otherwise if the product of 'lhs' and 'rhs' has an absolute value
    //:   that is larger than 'std::numeric_limits<Decimal64>::max()' then
    //:   raise the "overflow" floating-point exception and return infinity
    //:   with the same sign as that result.
    //:
    //: o Otherwise if the product of 'lhs' and 'rhs' has an absolute value
    //:   that is smaller than 'std::numeric_limits<Decimal64>::min()' then
    //:   raise the "underflow" floating-point exception and return zero with
    //:   the same sign as that result.
    //:
    //: o Otherwise return the product of the value of 'rhs' and the number
    //:   represented by 'rhs'.

Decimal64 operator*(Decimal64 lhs, int                rhs);
Decimal64 operator*(Decimal64 lhs, unsigned int       rhs);
Decimal64 operator*(Decimal64 lhs, long               rhs);
Decimal64 operator*(Decimal64 lhs, unsigned long      rhs);
Decimal64 operator*(Decimal64 lhs, long long          rhs);
Decimal64 operator*(Decimal64 lhs, unsigned long long rhs);
    // Multiply the specified 'rhs' by the value of the specified 'lhs' as
    // described by IEEE-754, and return the result.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity (positive or negative), and 'rhs' is
    //:   zero, then raise the "invalid" floating-point exception and return a
    //:   NaN.
    //:
    //: o Otherwise if 'lhs' is infinity (positive or negative), then return
    //:   'lhs'.
    //:
    //: o Otherwise if 'rhs' is zero, then return zero with the sign of 'lhs'.
    //:
    //: o Otherwise if the product of 'rhs' and the value of 'lhs' has an
    //:   absolute value that is larger than
    //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if the product of 'rhs' and the value of 'lhs' has an
    //:   absolute value that is smaller than
    //:   'std::numeric_limits<Decimal64>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the product of the value of 'lhs' and value 'rhs'.

Decimal64 operator*(int                lhs, Decimal64 rhs);
Decimal64 operator*(unsigned int       lhs, Decimal64 rhs);
Decimal64 operator*(long               lhs, Decimal64 rhs);
Decimal64 operator*(unsigned long      lhs, Decimal64 rhs);
Decimal64 operator*(long long          lhs, Decimal64 rhs);
Decimal64 operator*(unsigned long long lhs, Decimal64 rhs);
    // Multiply the specified 'lhs' by the value of the specified 'rhs' as
    // described by IEEE-754, and return the result.
    //
    //: o If 'rhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'rhs' is infinity (positive or negative), and 'lhs' is
    //:   zero, then raise the "invalid" floating-point exception and return a
    //:   NaN.
    //:
    //: o Otherwise if 'rhs' is infinity (positive or negative), then return
    //:   'rhs'.
    //:
    //: o Otherwise if 'lhs' is zero, then return zero with the sign of 'rhs'.
    //:
    //: o Otherwise if the product of 'lhs' and the value of 'rhs' has an
    //:   absolute value that is larger than
    //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if the product of 'lhs' and the value of 'rhs' has an
    //:   absolute value that is smaller than
    //:   'std::numeric_limits<Decimal64>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the product of the value of 'rhs' and value 'lhs'.

Decimal64 operator/(Decimal64 lhs, Decimal64 rhs);
Decimal64 operator/(Decimal32 lhs, Decimal64 rhs);
Decimal64 operator/(Decimal64 lhs, Decimal32 rhs);
    // Divide the value of the specified 'lhs' by the value of the specified
    // 'rhs' as described by IEEE-754, and return the result.
    //
    //: o If 'lhs' or 'rhs' is NaN, raise the "invalid" floating-point
    //:   exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' and 'rhs' are both infinity (positive or negative)
    //:   or both zero (positive or negative), raise the "invalid"
    //:   floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'rhs' has a positive zero value, raise the "overflow"
    //:   floating-point exception and return infinity with the sign of 'lhs'.
    //:
    //: o Otherwise if 'rhs' has a negative zero value, raise the "overflow"
    //:   floating-point exception and return infinity with the opposite sign
    //:   as 'lhs'.
    //:
    //: o Otherwise if dividing the value of 'lhs' with the value of 'rhs'
    //:   results in an absolute value that is larger than
    //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if dividing the value of 'lhs' with the value of 'rhs'
    //:   results in an absolute value that is smaller than
    //:   'std::numeric_limits<Decimal64>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the result of dividing the value of 'lhs' with the
    //:   value of 'rhs'.

Decimal64 operator/(Decimal64 lhs, int                rhs);
Decimal64 operator/(Decimal64 lhs, unsigned int       rhs);
Decimal64 operator/(Decimal64 lhs, long               rhs);
Decimal64 operator/(Decimal64 lhs, unsigned long      rhs);
Decimal64 operator/(Decimal64 lhs, long long          rhs);
Decimal64 operator/(Decimal64 lhs, unsigned long long rhs);
    // Divide the value of the specified 'lhs' by the specified 'rhs' as
    // described by IEEE-754, and return the result.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity (positive or negative), and 'rhs' is
    //:   zero, raise the "invalid" floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity (positive or negative), then return
    //:   'lhs'.
    //:
    //: o Otherwise if 'rhs' is zero, raise the "overflow" floating-point
    //:   exception return zero with the sign of 'lhs'.
    //:
    //: o Otherwise if the quotient of the value of 'lhs' and 'rhs' results in
    //:   an absolute value that is larger than
    //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if the quotient of the value of 'lhs' and 'rhs' results in
    //:   an absolute value that is smaller than
    //:   'std::numeric_limits<Decimal64>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the result of dividing the value of 'lhs' by the
    //:   value 'rhs'.

Decimal64 operator/(int                lhs, Decimal64 rhs);
Decimal64 operator/(unsigned int       lhs, Decimal64 rhs);
Decimal64 operator/(long               lhs, Decimal64 rhs);
Decimal64 operator/(unsigned long      lhs, Decimal64 rhs);
Decimal64 operator/(long long          lhs, Decimal64 rhs);
Decimal64 operator/(unsigned long long lhs, Decimal64 rhs);
    // Divide the specified 'lhs' by the value of the specified 'rhs' as
    // described by IEEE-754, and return the result.
    //
    //: o If 'rhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'rhs' is infinity (positive or negative), and 'lhs' is
    //:   zero, raise the "invalid" floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'rhs' is zero (positive or negative), raise the
    //:   "overflow" floating-point exception return zero with the sign of
    //:   'lhs'.
    //:
    //: o Otherwise if the quotient of 'rhs' and the value of 'rhs' results in
    //:   an absolute value that is larger than
    //:   'std::numeric_limits<Decimal64>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if the quotient of 'lhs' and the value of 'rhs' results in
    //:   an absolute value that is smaller than
    //:   'std::numeric_limits<Decimal64>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the result of dividing the value 'lhs' by the value
    //:   of 'rhs'.  Note that this is a floating-point operation, not integer.

bool operator==(Decimal64 lhs, Decimal64 rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'Decimal64' objects have the same value if the
    // 'compareQuietEqual' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representations equal.  In
    // other words, two 'Decimal64' objects have the same value if:
    //
    //: o both have a zero value (positive or negative), or
    //: o both have the same infinity value (both positive or negative), or
    //: o both have the value of a real number that are equal, even if they are
    //:   represented differently (cohorts have the same value)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.
    //
    // Note that a NaN is never equal to anything, including itself:
    //..
    //  Decimal64 aNaN = std::numeric_limits<Decimal64>::quiet_NaN();
    //  assert(!(aNan == aNan));
    //..

bool operator==(Decimal32 lhs, Decimal64 rhs);
bool operator==(Decimal64 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two decimal objects have the same value if the
    // 'compareQuietEqual' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representations equal.  In
    // other words, two decimal objects have the same value if:
    //
    //: o both have a zero value (positive or negative), or
    //: o both have the same infinity value (both positive or negative), or
    //: o both have the value of a real number that are equal, even if they are
    //:   represented differently (cohorts have the same value)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator!=(Decimal64 lhs, Decimal64 rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'Decimal64' objects do not have the
    // same value if the 'compareQuietEqual' operation (IEEE-754 defined,
    // non-total ordering comparison) considers the underlying IEEE
    // representations not equal.  In other words, two 'Decimal64' objects do
    // not have the same value if:
    //
    //: o both are a NaN, or
    //: o one has zero value (positive or negative) and the other does not, or
    //: o one has the value of positive infinity and the other does not, or
    //: o one has the value of negative infinity and the other does not, or
    //: o both have the value of a real number that are not equal, regardless
    //:   of their representation (cohorts are equal)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.
    //
    // Note that a NaN is never equal to anything, including itself:
    //..
    //  Decimal64 aNaN = std::numeric_limits<Decimal64>::quiet_NaN();
    //  assert(aNan != aNan);
    //..

bool operator!=(Decimal32 lhs, Decimal64 rhs);
bool operator!=(Decimal64 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two decimal objects do not have the same
    // value if the 'compareQuietEqual' operation (IEEE-754 defined, non-total
    // ordering comparison) considers the underlying IEEE representations not
    // equal.  In other words, two decimal objects do not have the same value
    // if:
    //
    //: o both are NaN, or
    //: o one has zero value (positive or negative) and the other does not, or
    //: o one has the value of positive infinity and the other does not, or
    //: o one has the value of negative infinity and the other does not, or
    //: o both have the value of a real number that are not equal, regardless
    //:   of their representation (cohorts are equal)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator<(Decimal64 lhs, Decimal64 rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs' and 'false' otherwise.  The value of a 'Decimal64' object 'lhs' is
    // less than that of an object 'rhs' if the 'compareQuietLess' operation
    // (IEEE-754 defined, non-total ordering comparison) considers the
    // underlying IEEE representation of 'lhs' to be less than of that of
    // 'rhs'.  In other words, 'lhs' is less than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' is zero (positive or negative) and 'rhs' is positive, or
    //: o 'rhs' is zero (positive or negative) and 'lhs' negative, or
    //: o 'lhs' is not positive infinity, or
    //: o 'lhs' is negative infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is less than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator<(Decimal32 lhs, Decimal64 rhs);
bool operator<(Decimal64 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs' and 'false' otherwise.  The value of a decimal object 'lhs' is
    // less than that of an object 'rhs' if the 'compareQuietLess' operation
    // (IEEE-754 defined, non-total ordering comparison) considers the
    // underlying IEEE representation of 'lhs' to be less than of that of
    // 'rhs'.  In other words, 'lhs' is less than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' is zero (positive or negative) and 'rhs' is positive, or
    //: o 'rhs' is zero (positive or negative) and 'lhs' negative, or
    //: o 'lhs' is not positive infinity, or
    //: o 'lhs' is negative infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is less than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator<=(Decimal64 lhs, Decimal64 rhs);
    // Return 'true' if the specified 'lhs' has a value less than or equal the
    // value of the specified 'rhs' and 'false' otherwise.  The value of a
    // 'Decimal64' object 'lhs' is less than or equal to the value of an object
    // 'rhs' if the 'compareQuietLessEqual' operation (IEEE-754 defined,
    // non-total ordering comparison) considers the underlying IEEE
    // representation of 'lhs' to be less or equal to that of 'rhs'.  In other
    // words, 'lhs' is less or equal than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are positive infinity, or
    //: o 'lhs' is negative infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is less or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator<=(Decimal32 lhs, Decimal64 rhs);
bool operator<=(Decimal64 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' has a value less than or equal the
    // value of the specified 'rhs' and 'false' otherwise.  The value of a
    // decimal object 'lhs' is less than or equal to the value of an object
    // 'rhs' if the 'compareQuietLessEqual' operation (IEEE-754 defined,
    // non-total ordering comparison) considers the underlying IEEE
    // representation of 'lhs' to be less or equal to that of 'rhs'.  In other
    // words, 'lhs' is less or equal than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are positive infinity, or
    //: o 'lhs' is negative infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is less or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>(Decimal64 lhs, Decimal64 rhs);
    // Return 'true' if the specified 'lhs' has a greater value than the
    // specified 'rhs' and 'false' otherwise.  The value of a 'Decimal64'
    // object 'lhs' is greater than that of an object 'rhs' if the
    // 'compareQuietGreater' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representation of 'lhs' to be
    // greater than of that of 'rhs'.  In other words, 'lhs' is greater than
    // 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'rhs' is zero (positive or negative) and 'lhs' positive, or
    //: o 'lhs' is zero (positive or negative) and 'rhs' negative, or
    //: o 'lhs' is not negative infinity, or
    //: o 'lhs' is positive infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>(Decimal32 lhs, Decimal64 rhs);
bool operator>(Decimal64 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' has a greater value than the
    // specified 'rhs' and 'false' otherwise.  The value of a decimal object
    // 'lhs' is greater than that of an object 'rhs' if the
    // 'compareQuietGreater' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representation of 'lhs' to be
    // greater than of that of 'rhs'.  In other words, 'lhs' is greater than
    // 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'rhs' is zero (positive or negative) and 'lhs' positive, or
    //: o 'lhs' is zero (positive or negative) and 'rhs' negative, or
    //: o 'lhs' is not negative infinity, or
    //: o 'lhs' is positive infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>=(Decimal64 lhs, Decimal64 rhs);
    // Return 'true' if the specified 'lhs' has a value greater than or equal
    // to the value of the specified 'rhs' and 'false' otherwise.  The value of
    // a 'Decimal64' object 'lhs' is greater or equal to a 'Decimal64' object
    // 'rhs' if the 'compareQuietGreaterEqual' operation (IEEE-754 defined,
    // non-total ordering comparison ) considers the underlying IEEE
    // representation of 'lhs' to be greater or equal to that of 'rhs'.  In
    // other words, 'lhs' is greater than or equal to 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are negative infinity, or
    //: o 'lhs' is positive infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>=(Decimal32 lhs, Decimal64 rhs);
bool operator>=(Decimal64 lhs, Decimal32 rhs);
    // Return 'true' if the specified 'lhs' has a value greater than or equal
    // to the value of the specified 'rhs' and 'false' otherwise.  The value of
    // a decimal object 'lhs' is greater or equal to a decimal object 'rhs' if
    // the 'compareQuietGreaterEqual' operation (IEEE-754 defined, non-total
    // ordering comparison ) considers the underlying IEEE representation of
    // 'lhs' to be greater or equal to that of 'rhs'.  In other words, 'lhs' is
    // greater than or equal to 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are negative infinity, or
    //: o 'lhs' is positive infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
operator>> (bsl::basic_istream<CHARTYPE, TRAITS>& stream, Decimal64& object);
    // Read, into the specified 'object', from the specified input 'stream' an
    // IEEE 64 bit decimal floating-point value as described in the IEEE-754
    // 2008 standard (5.12 Details of conversions between floating point
    // numbers and external character sequences) and return a reference
    // providing modifiable access to 'stream'.  If 'stream' contains a Nan
    // value, it is unspecified if 'object' will receive a quiet or signaling
    // 'Nan'.  If 'stream' is not valid on entry 'stream.good() == false', this
    // operation has no effect other than setting 'stream.fail()' to 'true'.
    // If eof (end-of-file) is found before any non-whitespace characters
    // 'stream.fail()' is set to 'true' and 'object' remains unchanged.  If eof
    // is detected after some characters have been read (and successfully
    // interpreted as part of the textual representation of a floating-point
    // value as specified by IEEE-754) then 'stream.eof()' is set to true.  If
    // the first non-whitespace character sequence is not a valid textual
    // representation of a floating-point value (e.g., 12e or e12 or 1*2) the
    // 'stream.fail()' is set to true and 'object' will remain unchanged.  If a
    // real number value is represented by the character sequence but it is a
    // large positive or negative value that cannot be stored into 'object' the
    // "overflow" floating-point exception is raised and positive or negative
    // infinity is stored into 'object', respectively.  If a real number value
    // is represented by the character sequence but it is a small positive or
    // negative value that cannot be stored into 'object' the "underflow"
    // floating-point exception is raised and positive or negative zero is
    // stored into 'object', respectively.  If a real number value is
    // represented by the character sequence but it cannot be stored exactly
    // into 'object' the "inexact" floating-point exception is raised, the
    // value is rounded according to the current rounding direction (of the
    // environment) and then stored into 'object.'
    //
    // NOTE: This method does not yet fully support iostream flags or the
    // decimal floating point exception context.

template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
operator<< (bsl::basic_ostream<CHARTYPE, TRAITS>& stream, Decimal64 object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single line format as described in the IEEE-754 2008
    // standard (5.12 Details of conversions between floating point numbers and
    // external character sequences), and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.
    //
    // NOTE: This method does not yet fully support iostream flags or the
    // decimal floating point exception context.

                           // =====================
                           // class Decimal_Type128
                           // =====================

class Decimal_Type128 {
    // This value-semantic class implements the IEEE-754 128 bit decimal
    // floating-point format arithmetic type.  This class is a standard layout
    // type that is 'const' thread-safe and exception-neutral.

  private:
    // DATA
    DecimalImpUtil::ValueType128 d_value;
                                          // The underlying IEEE representation

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(Decimal_Type128,
                                 bslalg::TypeTraitBitwiseCopyable);

    // CREATORS
    Decimal_Type128();
        // Create a 'Decimal128_Type' object having the value positive zero,
        // and 0 exponent (quantum 1e-33).

    Decimal_Type128(DecimalImpUtil::ValueType128 value);            // IMPLICIT
        // Create a 'Decimal128_Type' object having the specified 'value'.

    Decimal_Type128(Decimal32 value);                               // IMPLICIT
    Decimal_Type128(Decimal64 value);                               // IMPLICIT
        // Create a 'Decimal128_Type' object having the specified 'value',
        // subject to the conversion rules defined by IEEE-754:
        //
        //: o If 'value' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise if 'value' is infinity, then initialize this object to
        //:   infinity with the same sign.
        //:
        //: o Otherwise if 'value' is zero, then initialize this object to zero
        //:   with the same sign.
        //:
        //: o Otherwise initialize this object to 'value'.

    explicit Decimal_Type128(float  other);
    explicit Decimal_Type128(double other);
        // Create a 'Decimal128_Type' object having the value closest to the
        // specified 'other' value.  *Warning:* clients requiring a conversion
        // for an exact decimal value should use 'bdldfp_decimalconverutil'
        // (see *WARNING*: Conversions from 'float' and 'double'}.  This
        // conversion follows the conversion rules as defined by IEEE-754:
        //
        //: o If 'value' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise if 'value' is infinity, then initialize this object to
        //:   infinity value with the same sign.
        //:
        //: o Otherwise if 'value' has a zero value, then initialize this
        //:   object to zero with the same sign.
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and initialize this object to
        //:   infinity with the same sign as 'value'.
        //:
        //: o Otherwise if 'value' has an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal128>::min()' then raise the
        //:   "underflow" floating-point exception and initialize this object
        //:   to zero with the same sign as 'value'.
        //:
        //: o Otherwise if 'value' has a value that needs more than
        //:   'std::numeric_limits<Decimal128>::max_digit' significant decimal
        //:   digits to represent then raise the "inexact" floating-point
        //:   exception and initialize this object to the value of 'value'
        //:   rounded according to the rounding direction.
        //:
        //: o Otherwise initialize this object to 'value'.

    explicit Decimal_Type128(int                value);
    explicit Decimal_Type128(unsigned int       value);
    explicit Decimal_Type128(long               value);
    explicit Decimal_Type128(unsigned long      value);
    explicit Decimal_Type128(long long          value);
    explicit Decimal_Type128(unsigned long long value);
        // Create a 'Decimal128_Type' object having the value closest to the
        // specified 'value' subject to the conversion rules as defined by
        // IEEE-754:
        //
        //: o If 'value' is zero then initialize this object to positive zero
        //:   with a 0 exponent (quantum 1e-33).
        //:
        //: o Otherwise if 'value' has an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and initialize this object to
        //:   infinity with the same sign as 'other'.
        //:
        //: o Otherwise if 'value' has a value that is not exactly
        //:   representable using 'std::numeric_limits<Decimal128>::max_digit'
        //:   decimal digits then raise the "inexact" floating-point exception
        //:   and initialize this object to the value of 'value' rounded
        //:   according to the rounding direction.
        //:
        //: o Otherwise initialize this object to 'value'.
        //
        // The exponent 0 (quantum 1e-33) is preferred during conversion unless
        // it would cause unnecessary loss of precision.

    //! Decimal128_Type(const Decimal128_Type& original) = default;
        // Create a 'Decimal128_Type' object that is a copy of the specified
        // 'original' as defined by the 'copy' operation of IEEE-754 2008:
        //
        //: o If 'original' is NaN, initialize this object to a NaN.
        //:
        //: o Otherwise initialize this object to the value of the 'original'.
        //
        // Note that since floating-point types may be NaN, and NaNs are
        // unordered (do not compare equal even to themselves) it is possible
        // that a copy of a decimal will not compare equal to the original;
        // however it will behave as the original.

    //! ~Decimal128_Type() = default;
        // Destroy this object.

    // MANIPULATORS
    //! Decimal128_Type& operator=(const Decimal128_Type& rhs) = default;
        // Make this object a copy of the specified 'rhs' as defined by the
        // 'copy' operation of IEEE-754 2008 and return a reference providing
        // modifiable access to this object.
        //
        //: o If 'rhs' is NaN, set this object to a NaN.
        //:
        //: o Otherwise set this object to the value of the 'other'.
        //
        // Note that since floating-point types may be NaN, and NaNs are
        // unordered (do not compare equal even to themselves) it is possible
        // that, after an assignment, a decimal will not compare equal to the
        // original; however it will behave as the original.

    Decimal_Type128& operator++();
        // Add 1.0 to the value of this object and return a reference to it.
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to just set it to 1.0 (if the original value is small).

    Decimal_Type128& operator--();
        // Add -1.0 to the value of this object and return a reference to it.
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to just set it to -1.0 (if the original value is small).

    Decimal_Type128& operator+=(Decimal32  rhs);
    Decimal_Type128& operator+=(Decimal64  rhs);
    Decimal_Type128& operator+=(Decimal128 rhs);
        // Add the value of the specified 'rhs' object to the value of this as
        // described by IEEE-754, store the result in this object, and return a
        // reference to this object.
        //
        //: o If either this object or 'rhs' is NaN, then raise the "invalid"
        //:   floating-point exception and set this object to a NaN.
        //:
        //: o Otherwise if this object and 'rhs' have infinite values of
        //:   differing signs, then raise the "invalid" floating-point
        //:   exception and set this object to a NaN.
        //:
        //: o Otherwise if this object and 'rhs' have infinite values of the
        //:   same sign, then do not change this object.
        //:
        //: o Otherwise if 'rhs' has a zero value (positive or negative), do
        //:   not change this object.
        //:
        //: o Otherwise if the sum of this object and 'rhs' has an absolute
        //:   value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity value with the same sign as that result.
        //:
        //: o Otherwise set this object to the sum of the number represented by
        //:   'rhs' and the number represented by this object.
        //
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to update it to the value of the 'other' (if the original
        // value is small).

    Decimal_Type128& operator+=(int                rhs);
    Decimal_Type128& operator+=(unsigned int       rhs);
    Decimal_Type128& operator+=(long               rhs);
    Decimal_Type128& operator+=(unsigned long      rhs);
    Decimal_Type128& operator+=(long long          rhs);
    Decimal_Type128& operator+=(unsigned long long rhs);
        // Add the specified 'rhs' to the value of this object as described by
        // IEEE-754, store the result in this object, and return a reference to
        // this object.
        //
        //: o If this object is NaN, then raise the "invalid" floating-point
        //:   exception and do not change this object.
        //:
        //: o Otherwise if this object is infinity, then do not change it.
        //:
        //: o Otherwise if the sum of this object and 'rhs' has an absolute
        //:   value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the same sign as that result.
        //:
        //: o Otherwise set this object to sum of adding 'rhs' and the number
        //:   represented by this object.
        //
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to update it to the value of the 'other' (if the original
        // value is small).

    Decimal_Type128& operator-=(Decimal32  rhs);
    Decimal_Type128& operator-=(Decimal64  rhs);
    Decimal_Type128& operator-=(Decimal128 rhs);
        // Subtract the value of the specified 'rhs' from the value of this
        // object as described by IEEE-754, store the result in this object,
        // and return a reference to this object.
        //
        //: o If either this object or 'rhs' is NaN, then raise the "invalid"
        //:   floating-point exception and set this object to a NaN.
        //:
        //: o Otherwise if this object and the 'rhs' have infinite values of
        //:   the same sign, raise the "invalid" floating-point exception and
        //:   set this object to a NaN.
        //:
        //: o Otherwise if this object and the 'rhs' have infinite values of
        //:   differing signs, then do not change this object.
        //:
        //: o Otherwise if the 'rhs' has a zero value (positive or negative),
        //:   do not change this object.
        //:
        //: o Otherwise if subtracting the value of the 'rhs' object from this
        //:   results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the same sign as that result.
        //:
        //: o Otherwise set this object to the result of subtracting the value
        //:   of 'rhs' from the value of this object.
        //
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to update it to the value of the 'other' (if the original
        // value is small).

    Decimal_Type128& operator-=(int                rhs);
    Decimal_Type128& operator-=(unsigned int       rhs);
    Decimal_Type128& operator-=(long               rhs);
    Decimal_Type128& operator-=(unsigned long      rhs);
    Decimal_Type128& operator-=(long long          rhs);
    Decimal_Type128& operator-=(unsigned long long rhs);
        // Subtract the specified 'rhs' from the value of this object as
        // described by IEEE-754, store the result in this object, and return a
        // reference to this object.
        //
        //: o If this object is NaN, raise the "invalid" floating-point
        //:   exception and do not change this object.
        //:
        //: o Otherwise if this object is infinity, then do not change it.
        //:
        //: o Otherwise if subtracting 'rhs' from this object's value results
        //:   in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the same sign as that result.
        //:
        //: o Otherwise set this object to the result of subtracting 'rhs' from
        //:   the value of this object.
        //
        // Note that this is a floating-point value so this operations may not
        // change the value of this object at all (if the value is large) or it
        // may seem to update it to the value of the 'other' (if the original
        // value is small).

    Decimal_Type128& operator*=(Decimal32  rhs);
    Decimal_Type128& operator*=(Decimal64  rhs);
    Decimal_Type128& operator*=(Decimal128 rhs);
        // Multiply the value of the specified 'rhs' object by the value of
        // this as described by IEEE-754, store the result in this object, and
        // return a reference to this object.
        //
        //: o If either this object or 'rhs' is NaN, raise the "invalid"
        //:   floating-point exception and set this object to NaN.
        //:
        //: o Otherwise, if one of this object and 'rhs' is zero (positive or
        //:   negative) and the other is infinity (positive or negative), raise
        //:   the "invalid" floating-point exception and set this object to a
        //:   NaN.
        //:
        //: o Otherwise, if either this object or 'rhs' is positive or negative
        //:   infinity, set this object to infinity.  The sign of this object
        //:   will be positive if this object and 'rhs' had the same sign, and
        //:   negative otherwise.
        //:
        //: o Otherwise, if either this object or 'rhs' is zero, set this
        //:   object to zero.  The sign of this object will be positive if this
        //:   object and 'rhs' had the same sign, and negative otherwise.
        //:
        //: o Otherwise if the product of this object and 'rhs' has an absolute
        //:   value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the same sign of that result.
        //:
        //: o Otherwise if the product of this object and 'rhs' has an absolute
        //:   value that is smaller than
        //:   'std::numeric_limits<Decimal128>::min()' then raise the
        //:   "underflow" floating-point exception and set this object to zero
        //:   value with the same sign as that result.
        //:
        //: o Otherwise set this object to the product of the value of 'rhs'
        //:   and the value of this object.

    Decimal_Type128& operator*=(int                rhs);
    Decimal_Type128& operator*=(unsigned int       rhs);
    Decimal_Type128& operator*=(long               rhs);
    Decimal_Type128& operator*=(unsigned long      rhs);
    Decimal_Type128& operator*=(long long          rhs);
    Decimal_Type128& operator*=(unsigned long long rhs);
        // Multiply the specified 'rhs' by the value of this object as
        // described by IEEE-754, store the result in this object, and return a
        // reference to this object.
        //
        //: o If this object is NaN, raise the "invalid" floating-point
        //:   exception and set this object to a NaN.
        //:
        //: o Otherwise if this object is infinity (positive or negative), and
        //:   'rhs' is zero, then raise the "invalid" floating-point exception
        //:   and set this object to a NaN.
        //:
        //: o Otherwise if this object is infinity (positive or negative), then
        //:   do not change it.
        //:
        //: o Otherwise if 'rhs' is zero, then set this object to zero with the
        //:   same sign as its value had prior to this operation.
        //:
        //: o Otherwise if the product of 'rhs' and the value of this object
        //:   results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the same sign as that result.
        //:
        //: o Otherwise if the product of 'rhs' and the value of this object
        //:   results in an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal128>::min()' then raise the
        //:   "underflow" floating-point exception and set this object to zero
        //:   with the same sign as that result.
        //:
        //: o Otherwise set this object to the product of the value of this
        //:   object and the value 'rhs'.

    Decimal_Type128& operator/=(Decimal32  rhs);
    Decimal_Type128& operator/=(Decimal64  rhs);
    Decimal_Type128& operator/=(Decimal128 rhs);
        // Divide the value of this object by the value of the specified 'rhs'
        // as described by IEEE-754, store the result in this object, and
        // return a reference to this object.
        //
        //: o If either this object or 'rhs' is NaN, then raise the "invalid"
        //:   floating-point exception and set this object to a NaN.
        //:
        //: o Otherwise if this object and 'rhs' are infinity (positive or
        //:   negative) or zero (positive or negative), then raise the
        //:   "invalid" floating-point exception and set this object to a NaN.
        //:
        //: o Otherwise if 'rhs' has a positive zero value, then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the same sign as its original value.
        //:
        //: o Otherwise if 'rhs' has a negative zero value, then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the opposite sign as its original value.
        //:
        //: o Otherwise if dividing the value of this object with the value of
        //:   'rhs' results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity value with the same sign as that result.
        //:
        //: o Otherwise if dividing the value of this object with the value of
        //:   'rhs' results in an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal128>::min()' then raise the
        //:   "underflow" floating-point exception and set this object to zero
        //:   with the same sign as that result.
        //:
        //: o Otherwise set this object to the result of dividing the value of
        //:   this object with the value of 'rhs'.

    Decimal_Type128& operator/=(int                rhs);
    Decimal_Type128& operator/=(unsigned int       rhs);
    Decimal_Type128& operator/=(long               rhs);
    Decimal_Type128& operator/=(unsigned long      rhs);
    Decimal_Type128& operator/=(long long          rhs);
    Decimal_Type128& operator/=(unsigned long long rhs);
        // Divide the value of this object by the specified 'rhs' as described
        // by IEEE-754, store the result in this object, and return a reference
        // to this object.
        //
        //: o If this object is NaN, raise the "invalid" floating-point
        //:   exception and set this object to a NaN.
        //:
        //: o Otherwise if this object is infinity (positive or negative), and
        //:   'rhs' is zero, raise the "invalid" floating-point exception and
        //:   set this object to a NaN.
        //:
        //: o Otherwise if this object is infinity (positive or negative), then
        //:   do not change it.
        //:
        //: o Otherwise if 'rhs' is zero, raise the "overflow" floating-point
        //:   exception and then set this object to infinity with the same sign
        //:   it had prior to this operation.
        //:
        //: o Otherwise if the quotient of the value of this object and 'rhs'
        //:   results in an absolute value that is larger than
        //:   'std::numeric_limits<Decimal128>::max()' then raise the
        //:   "overflow" floating-point exception and set this object to
        //:   infinity with the same sign as that result.
        //:
        //: o Otherwise if the quotient of the value of this object and 'rhs'
        //:   results in an absolute value that is smaller than
        //:   'std::numeric_limits<Decimal128>::min()' then raise the
        //:   "underflow" floating-point exception and set this object to zero
        //:   with the same sign as that result.
        //:
        //: o Otherwise set this object to the result of dividing the number
        //:   represented by this object by 'rhs'.

    DecimalImpUtil::ValueType128 *data();
        // Return a modifiable pointer to the underlying implementation.

    // ACCESSORS
    const DecimalImpUtil::ValueType128 *data() const;
        // Return a non-modifiable pointer to the underlying implementation.

    DecimalImpUtil::ValueType128 value() const;
        // Return the value of the underlying implementation.
};

// FREE OPERATORS
Decimal128 operator+(Decimal128 value);
    // Return a copy of the specified 'value' if the value is not negative
    // zero, and return positive zero otherwise.

Decimal128 operator-(Decimal128 value);
    // Return the result of applying the unary - operator to the specified
    // 'value' as described by IEEE-754.  Note that floating-point numbers have
    // signed zero, therefore this operation is not the same as '0-value'.

Decimal128 operator++(Decimal128& value, int);
    // Apply the prefix ++ operator to the specified 'value' and return its
    // original value.  Note that this is a floating-point value so this
    // operations may not change the value of this object at all (if the value
    // is large) or it may seem to just set it to 1.0 (if the original value is
    // small).

Decimal128 operator--(Decimal128& value, int);
    // Apply the prefix -- operator to the specified 'value' and return its
    // original value.  Note that this is a floating-point value so this
    // operations may not change the value of this object at all (if the value
    // is large) or it may seem to just set it to 1.0 (if the original value is
    // small).

Decimal128 operator+(Decimal128 lhs, Decimal128 rhs);
Decimal128 operator+(Decimal32  lhs, Decimal128 rhs);
Decimal128 operator+(Decimal128 lhs, Decimal32  rhs);
Decimal128 operator+(Decimal64  lhs, Decimal128 rhs);
Decimal128 operator+(Decimal128 lhs, Decimal64  rhs);
    // Add the value of the specified 'rhs' to the value of the specified 'lhs'
    // as described by IEEE-754 and return the result.
    //
    //: o If either of 'lhs' or 'rhs' is NaN, then raise the "invalid"
    //:   floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' and 'rhs' are infinities of differing signs, raise
    //:   the "invalid" floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' and 'rhs' are infinities of the same sign then
    //:   return infinity of that sign.
    //:
    //: o Otherwise if 'rhs' is zero (positive or negative), return 'lhs'.
    //:
    //: o Otherwise if the sum of 'lhs' and 'rhs' has an absolute value that is
    //:   larger than 'std::numeric_limits<Decimal128>::max()' then raise the
    //:   "overflow" floating-point exception and return infinity with the same
    //:   sign as that result.
    //:
    //: o Otherwise return the sum of the number represented by 'lhs' and the
    //:   number represented by 'rhs'.

Decimal128 operator+(Decimal128 lhs, int                rhs);
Decimal128 operator+(Decimal128 lhs, unsigned int       rhs);
Decimal128 operator+(Decimal128 lhs, long               rhs);
Decimal128 operator+(Decimal128 lhs, unsigned long      rhs);
Decimal128 operator+(Decimal128 lhs, long long          rhs);
Decimal128 operator+(Decimal128 lhs, unsigned long long rhs);
    // Add the specified 'rhs' to the value of the specified 'lhs' as described
    // by IEEE-754 and return the result.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity, then return that infinity.
    //:
    //: o Otherwise if the sum of 'lhs' and 'rhs' has an absolute value that is
    //:   larger than 'std::numeric_limits<Decimal128>::max()' then raise the
    //:   "overflow" floating-point exception and return infinity with the same
    //:   sign as that result.
    //:
    //: o Otherwise return the sum of 'rhs' and the number represented by
    //:   'lhs'.

Decimal128 operator+(int                lhs, Decimal128 rhs);
Decimal128 operator+(unsigned int       lhs, Decimal128 rhs);
Decimal128 operator+(long               lhs, Decimal128 rhs);
Decimal128 operator+(unsigned long      lhs, Decimal128 rhs);
Decimal128 operator+(long long          lhs, Decimal128 rhs);
Decimal128 operator+(unsigned long long lhs, Decimal128 rhs);
    // Add the specified 'lhs' to the value of the specified 'rhs' as described
    // by IEEE-754 and return the result.
    //
    //: o If 'rhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'rhs' is infinity, then return that infinity.
    //:
    //: o Otherwise if the sum of the value of 'rhs' and 'lhs' has an absolute
    //:   value that is larger than 'std::numeric_limits<Decimal128>::max()'
    //:   then raise the "overflow" floating-point exception and return
    //:   infinity with the same sign as that result.
    //:
    //: o Otherwise return the sum of 'lhs' and the number represented by
    //:   'rhs'.

Decimal128 operator-(Decimal128 lhs, Decimal128 rhs);
Decimal128 operator-(Decimal32  lhs, Decimal128 rhs);
Decimal128 operator-(Decimal128 lhs, Decimal32  rhs);
Decimal128 operator-(Decimal64  lhs, Decimal128 rhs);
Decimal128 operator-(Decimal128 lhs, Decimal64  rhs);
    // Subtract the value of the specified 'rhs' from the value of the
    // specified 'lhs' as described by IEEE-754 and return the result.
    //
    //: o If either 'lhs' or 'rhs' is NaN, then raise the "invalid"
    //:   floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' and the 'rhs' have infinity values of the same
    //:   sign, then raise the "invalid" floating-point exception and return a
    //:   NaN.
    //:
    //: o Otherwise if 'lhs' and the 'rhs' have infinity values of differing
    //:   signs, then return 'lhs'.
    //:
    //: o Otherwise if 'rhs' has a zero value (positive or negative), then
    //:   return 'lhs'.
    //:
    //: o Otherwise if subtracting the value of the 'rhs' object from the value
    //:   of 'lhs' results in an absolute value that is larger than
    //:   'std::numeric_limits<Decimal128>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise return the result of subtracting the value of 'rhs' from
    //:   the value of 'lhs'.

Decimal128 operator-(Decimal128 lhs, int                rhs);
Decimal128 operator-(Decimal128 lhs, unsigned int       rhs);
Decimal128 operator-(Decimal128 lhs, long               rhs);
Decimal128 operator-(Decimal128 lhs, unsigned long      rhs);
Decimal128 operator-(Decimal128 lhs, long long          rhs);
Decimal128 operator-(Decimal128 lhs, unsigned long long rhs);
    // Subtract the specified 'rhs' from the value of the specified 'lhs' as
    // described by IEEE-754 and return a reference to this object.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity, then return 'lhs'.
    //:
    //: o Otherwise if subtracting 'rhs' from the value of 'lhs' results in an
    //:   absolute value that is larger than
    //:   'std::numeric_limits<Decimal128>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise return the result of subtracting 'rhs' from the value of
    //:   'lhs'.

Decimal128 operator-(int                lhs, Decimal128 rhs);
Decimal128 operator-(unsigned int       lhs, Decimal128 rhs);
Decimal128 operator-(long               lhs, Decimal128 rhs);
Decimal128 operator-(unsigned long      lhs, Decimal128 rhs);
Decimal128 operator-(long long          lhs, Decimal128 rhs);
Decimal128 operator-(unsigned long long lhs, Decimal128 rhs);
    // Subtract the specified 'rhs' from the value of the specified 'lhs' as
    // described by IEEE-754 and return a reference to this object.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception return
    //:   a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity, then return 'lhs'.
    //:
    //: o Otherwise if 'rhs' is zero (positive or negative), then return 'lhs'.
    //:
    //: o Otherwise if subtracting 'rhs' from the value of 'lhs' results in an
    //:   absolute value that is larger than
    //:   'std::numeric_limits<Decimal128>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise return the result of subtracting the value of 'rhs'
    // from the number 'lhs'.

Decimal128 operator*(Decimal128 lhs, Decimal128 rhs);
Decimal128 operator*(Decimal32  lhs, Decimal128 rhs);
Decimal128 operator*(Decimal128 lhs, Decimal32  rhs);
Decimal128 operator*(Decimal64  lhs, Decimal128 rhs);
Decimal128 operator*(Decimal128 lhs, Decimal64  rhs);
    // Multiply the value of the specified 'lhs' object by the value of the
    // specified 'rhs' as described by IEEE-754 and return the result.
    //
    //: o If either of 'lhs' or 'rhs' is NaN, return a NaN.
    //:
    //: o Otherwise if one of the operands is infinity (positive or negative)
    //:   and the other is zero (positive or negative), then raise the
    //:   "invalid" floating-point exception raised and return a NaN.
    //:
    //: o Otherwise if both 'lhs' and 'rhs' are infinity (positive or
    //:   negative), return infinity.  The sign of the returned value will be
    //:   positive if 'lhs' and 'rhs' have the same sign, and negative
    //:   otherwise.
    //:
    //: o Otherwise, if either 'lhs' or 'rhs' is zero, return zero.  The sign
    //:   of the returned value will be positive if 'lhs' and 'rhs' have the
    //:   same sign, and negative otherwise.
    //:
    //: o Otherwise if the product of 'lhs' and 'rhs' has an absolute value
    //:   that is larger than 'std::numeric_limits<Decimal128>::max()' then
    //:   raise the "overflow" floating-point exception and return infinity
    //:   with the same sign as that result.
    //:
    //: o Otherwise if the product of 'lhs' and 'rhs' has an absolute value
    //:   that is smaller than 'std::numeric_limits<Decimal128>::min()' then
    //:   raise the "underflow" floating-point exception and return zero with
    //:   the same sign as that result.
    //:
    //: o Otherwise return the product of the value of 'rhs' and the number
    //:   represented by 'rhs'.

Decimal128 operator*(Decimal128 lhs, int                rhs);
Decimal128 operator*(Decimal128 lhs, unsigned int       rhs);
Decimal128 operator*(Decimal128 lhs, long               rhs);
Decimal128 operator*(Decimal128 lhs, unsigned long      rhs);
Decimal128 operator*(Decimal128 lhs, long long          rhs);
Decimal128 operator*(Decimal128 lhs, unsigned long long rhs);
    // Multiply the specified 'rhs' by the value of the specified 'lhs' as
    // described by IEEE-754, and return the result.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity (positive or negative), and 'rhs' is
    //:   zero, then raise the "invalid" floating-point exception and return a
    //:   NaN.
    //:
    //: o Otherwise if 'lhs' is infinity (positive or negative), then return
    //:   'lhs'.
    //:
    //: o Otherwise if 'rhs' is zero, then return zero with the sign of 'lhs'.
    //:
    //: o Otherwise if the product of 'rhs' and the value of 'lhs' has an
    //:   absolute value that is larger than
    //:   'std::numeric_limits<Decimal128>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if the product of 'rhs' and the value of 'lhs' has an
    //:   absolute value that is smaller than
    //:   'std::numeric_limits<Decimal128>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the product of the value of 'lhs' and value 'rhs'.

Decimal128 operator*(int                lhs, Decimal128 rhs);
Decimal128 operator*(unsigned int       lhs, Decimal128 rhs);
Decimal128 operator*(long               lhs, Decimal128 rhs);
Decimal128 operator*(unsigned long      lhs, Decimal128 rhs);
Decimal128 operator*(long long          lhs, Decimal128 rhs);
Decimal128 operator*(unsigned long long lhs, Decimal128 rhs);
    // Multiply the specified 'lhs' by the value of the specified 'rhs' as
    // described by IEEE-754, and return the result.
    //
    //: o If 'rhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'rhs' is infinity (positive or negative), and 'lhs' is
    //:   zero, then raise the "invalid" floating-point exception and return a
    //:   NaN.
    //:
    //: o Otherwise if 'rhs' is infinity (positive or negative), then return
    //:   'rhs'.
    //:
    //: o Otherwise if 'lhs' is zero, then return zero with the sign of 'rhs'.
    //:
    //: o Otherwise if the product of 'lhs' and the value of 'rhs' has an
    //:   absolute value that is larger than
    //:   'std::numeric_limits<Decimal128>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if the product of 'lhs' and the value of 'rhs' has an
    //:   absolute value that is smaller than
    //:   'std::numeric_limits<Decimal128>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the product of the value of 'rhs' and value 'lhs'.

Decimal128 operator/(Decimal128 lhs, Decimal128 rhs);
Decimal128 operator/(Decimal32  lhs, Decimal128 rhs);
Decimal128 operator/(Decimal128 lhs, Decimal32  rhs);
Decimal128 operator/(Decimal64  lhs, Decimal128 rhs);
Decimal128 operator/(Decimal128 lhs, Decimal64  rhs);
    // Divide the value of the specified 'lhs' by the value of the specified
    // 'rhs' as described by IEEE-754, and return the result.
    //
    //: o If 'lhs' or 'rhs' is NaN, raise the "invalid" floating-point
    //:   exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' and 'rhs' are both infinity (positive or negative)
    //:   or both zero (positive or negative), raise the "invalid"
    //:   floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'rhs' has a positive zero value, raise the "overflow"
    //:   floating-point exception and return infinity with the sign of 'lhs'.
    //:
    //: o Otherwise if 'rhs' has a negative zero value, raise the "overflow"
    //:   floating-point exception and return infinity with the opposite sign
    //:   as 'lhs'.
    //:
    //: o Otherwise if dividing the value of 'lhs' with the value of 'rhs'
    //:   results in an absolute value that is larger than
    //:   'std::numeric_limits<Decimal128>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if dividing the value of 'lhs' with the value of 'rhs'
    //:   results in an absolute value that is smaller than
    //:   'std::numeric_limits<Decimal128>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the result of dividing the value of 'lhs' with the
    //:   value of 'rhs'.

Decimal128 operator/(Decimal128 lhs, int                rhs);
Decimal128 operator/(Decimal128 lhs, unsigned int       rhs);
Decimal128 operator/(Decimal128 lhs, long               rhs);
Decimal128 operator/(Decimal128 lhs, unsigned long      rhs);
Decimal128 operator/(Decimal128 lhs, long long          rhs);
Decimal128 operator/(Decimal128 lhs, unsigned long long rhs);
    // Divide the value of the specified 'lhs' by the specified 'rhs' as
    // described by IEEE-754, and return the result.
    //
    //: o If 'lhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity (positive or negative), and 'rhs' is
    //:   zero, raise the "invalid" floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'lhs' is infinity (positive or negative), then return
    //:   'lhs'.
    //:
    //: o Otherwise if 'rhs' is zero, raise the "overflow" floating-point
    //:   exception return zero with the sign of 'lhs'.
    //:
    //: o Otherwise if the quotient of the value of 'lhs' and 'rhs' results in
    //:   an absolute value that is larger than
    //:   'std::numeric_limits<Decimal128>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if the quotient of the value of 'lhs' and 'rhs' results in
    //:   an absolute value that is smaller than
    //:   'std::numeric_limits<Decimal128>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the result of dividing the value of 'lhs' by the
    //:   value 'rhs'.

Decimal128 operator/(int                lhs, Decimal128 rhs);
Decimal128 operator/(unsigned int       lhs, Decimal128 rhs);
Decimal128 operator/(long               lhs, Decimal128 rhs);
Decimal128 operator/(unsigned long      lhs, Decimal128 rhs);
Decimal128 operator/(long long          lhs, Decimal128 rhs);
Decimal128 operator/(unsigned long long lhs, Decimal128 rhs);
    // Divide the specified 'lhs' by the value of the specified 'rhs' as
    // described by IEEE-754, and return the result.
    //
    //: o If 'rhs' is NaN, raise the "invalid" floating-point exception and
    //:   return a NaN.
    //:
    //: o Otherwise if 'rhs' is infinity (positive or negative), and 'lhs' is
    //:   zero, raise the "invalid" floating-point exception and return a NaN.
    //:
    //: o Otherwise if 'rhs' is zero (positive or negative), raise the
    //:   "overflow" floating-pointexception return zero with the sign of
    //:   'lhs'.
    //:
    //: o Otherwise if the quotient of 'rhs' and the value of 'rhs' results in
    //:   an absolute value that is larger than
    //:   'std::numeric_limits<Decimal128>::max()' then raise the "overflow"
    //:   floating-point exception and return infinity with the same sign as
    //:   that result.
    //:
    //: o Otherwise if the quotient of 'lhs' and the value of 'rhs' results in
    //:   an absolute value that is smaller than
    //:   'std::numeric_limits<Decimal128>::min()' then raise the "underflow"
    //:   floating-point exception and return zero with the same sign as that
    //:   result.
    //:
    //: o Otherwise return the result of dividing the value 'lhs' by the value
    //:   of 'rhs'.  Note that this is a floating-point operation, not integer.

bool operator==(Decimal128 lhs, Decimal128 rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'Decimal128' objects have the same value if the
    // 'compareQuietEqual' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representations equal.  In
    // other words, two 'Decimal128' objects have the same value if:
    //
    //: o both have a zero value (positive or negative), or
    //: o both have the same infinity value (both positive or negative), or
    //: o both have the value of a real number that are equal, even if they are
    //:   represented differently (cohorts have the same value)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.
    //
    // Note that a NaN is never equal to anything, including itself:
    //..
    //  Decimal128 aNaN = std::numeric_limits<Decimal128>::quiet_NaN();
    //  assert(!(aNan == aNan));
    //..

bool operator==(Decimal32  lhs, Decimal128 rhs);
bool operator==(Decimal128 lhs, Decimal32  rhs);
bool operator==(Decimal64  lhs, Decimal128 rhs);
bool operator==(Decimal128 lhs, Decimal64  rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two decimal objects have the same value if the
    // 'compareQuietEqual' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representations equal.  In
    // other words, two decimal objects have the same value if:
    //
    //: o both have a zero value (positive or negative), or
    //: o both have the same infinity value (both positive or negative), or
    //: o both have the value of a real number that are equal, even if they are
    //:   represented differently (cohorts have the same value)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator!=(Decimal128 lhs, Decimal128 rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'Decimal128' objects do not have the
    // same value if the 'compareQuietEqual' operation (IEEE-754 defined,
    // non-total ordering comparison) considers the underlying IEEE
    // representations not equal.  In other words, two 'Decimal128' objects do
    // not have the same value if:
    //
    //: o both are a NaN, or
    //: o one has zero value (positive or negative) and the other does not, or
    //: o one has the value of positive infinity and the other does not, or
    //: o one has the value of negative infinity and the other does not, or
    //: o both have the value of a real number that are not equal, regardless
    //:   of their representation (cohorts are equal)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.
    //
    // Note that a NaN is never equal to anything, including itself:
    //..
    //  Decimal128 aNaN = std::numeric_limits<Decimal128>::quiet_NaN();
    //  assert(aNan != aNan);
    //..

bool operator!=(Decimal32  lhs, Decimal128 rhs);
bool operator!=(Decimal128 lhs, Decimal32  rhs);
bool operator!=(Decimal64  lhs, Decimal128 rhs);
bool operator!=(Decimal128 lhs, Decimal64  rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two decimal objects do not have the same
    // value if the 'compareQuietEqual' operation (IEEE-754 defined, non-total
    // ordering comparison) considers the underlying IEEE representations not
    // equal.  In other words, two decimal objects do not have the same value
    // if:
    //
    //: o both are NaN, or
    //: o one has zero value (positive or negative) and the other does not, or
    //: o one has the value of positive infinity and the other does not, or
    //: o one has the value of negative infinity and the other does not, or
    //: o both have the value of a real number that are not equal, regardless
    //:   of their representation (cohorts are equal)
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator<(Decimal128 lhs, Decimal128 rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs' and 'false' otherwise.  The value of a 'Decimal128' object 'lhs'
    // is less than that of an object 'rhs' if the 'compareQuietLess' operation
    // (IEEE-754 defined, non-total ordering comparison) considers the
    // underlying IEEE representation of 'lhs' to be less than of that of
    // 'rhs'.  In other words, 'lhs' is less than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' is zero (positive or negative) and 'rhs' is positive, or
    //: o 'rhs' is zero (positive or negative) and 'lhs' negative, or
    //: o 'lhs' is not positive infinity, or
    //: o 'lhs' is negative infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is less than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator<(Decimal32  lhs, Decimal128 rhs);
bool operator<(Decimal128 lhs, Decimal32  rhs);
bool operator<(Decimal64  lhs, Decimal128 rhs);
bool operator<(Decimal128 lhs, Decimal64  rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs' and 'false' otherwise.  The value of a decimal object 'lhs' is
    // less than that of an object 'rhs' if the 'compareQuietLess' operation
    // (IEEE-754 defined, non-total ordering comparison) considers the
    // underlying IEEE representation of 'lhs' to be less than of that of
    // 'rhs'.  In other words, 'lhs' is less than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' is zero (positive or negative) and 'rhs' is positive, or
    //: o 'rhs' is zero (positive or negative) and 'lhs' negative, or
    //: o 'lhs' is not positive infinity, or
    //: o 'lhs' is negative infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs'is less than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator<=(Decimal128 lhs, Decimal128 rhs);
    // Return 'true' if the specified 'lhs' has a value less than or equal the
    // value of the specified 'rhs' and 'false' otherwise.  The value of a
    // 'Decimal128' object 'lhs' is less than or equal to the value of an
    // object 'rhs' if the 'compareQuietLessEqual' operation (IEEE-754 defined,
    // non-total ordering comparison) considers the underlying IEEE
    // representation of 'lhs' to be less or equal to that of 'rhs'.  In other
    // words, 'lhs' is less or equal than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are positive infinity, or
    //: o 'lhs' is negative infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is less or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator<=(Decimal32  lhs, Decimal128 rhs);
bool operator<=(Decimal128 lhs, Decimal32  rhs);
bool operator<=(Decimal64  lhs, Decimal128 rhs);
bool operator<=(Decimal128 lhs, Decimal64  rhs);
    // Return 'true' if the specified 'lhs' has a value less than or equal the
    // value of the specified 'rhs' and 'false' otherwise.  The value of a
    // decimal object 'lhs' is less than or equal to the value of an object
    // 'rhs' if the 'compareQuietLessEqual' operation (IEEE-754 defined,
    // non-total ordering comparison) considers the underlying IEEE
    // representation of 'lhs' to be less or equal to that of 'rhs'.  In other
    // words, 'lhs' is less or equal than 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are positive infinity, or
    //: o 'lhs' is negative infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is less or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>(Decimal128 lhs, Decimal128 rhs);
    // Return 'true' if the specified 'lhs' has a greater value than the
    // specified 'rhs' and 'false' otherwise.  The value of a 'Decimal128'
    // object 'lhs' is greater than that of an object 'rhs' if the
    // 'compareQuietGreater' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representation of 'lhs' to be
    // greater than of that of 'rhs'.  In other words, 'lhs' is greater than
    // 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'rhs' is zero (positive or negative) and 'lhs' positive, or
    //: o 'lhs' is zero (positive or negative) and 'rhs' negative, or
    //: o 'lhs' is not negative infinity, or
    //: o 'lhs' is positive infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>(Decimal32  lhs, Decimal128 rhs);
bool operator>(Decimal128 lhs, Decimal32  rhs);
bool operator>(Decimal64  lhs, Decimal128 rhs);
bool operator>(Decimal128 lhs, Decimal64  rhs);
    // Return 'true' if the specified 'lhs' has a greater value than the
    // specified 'rhs' and 'false' otherwise.  The value of a decimal object
    // 'lhs' is greater than that of an object 'rhs' if the
    // 'compareQuietGreater' operation (IEEE-754 defined, non-total ordering
    // comparison) considers the underlying IEEE representation of 'lhs' to be
    // greater than of that of 'rhs'.  In other words, 'lhs' is greater than
    // 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'rhs' is zero (positive or negative) and 'lhs' positive, or
    //: o 'lhs' is zero (positive or negative) and 'rhs' negative, or
    //: o 'lhs' is not negative infinity, or
    //: o 'lhs' is positive infinity and 'rhs' is not, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater than that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>=(Decimal128 lhs, Decimal128 rhs);
    // Return 'true' if the specified 'lhs' has a value greater than or equal
    // to the value of the specified 'rhs' and 'false' otherwise.  The value of
    // a 'Decimal128' object 'lhs' is greater or equal to a 'Decimal128' object
    // 'rhs' if the 'compareQuietGreaterEqual' operation (IEEE-754 defined,
    // non-total ordering comparison ) considers the underlying IEEE
    // representation of 'lhs' to be greater or equal to that of 'rhs'.  In
    // otherwords, 'lhs' is greater than or equal to 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are negative infinity, or
    //: o 'lhs' is positive infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

bool operator>=(Decimal32  lhs, Decimal128 rhs);
bool operator>=(Decimal128 lhs, Decimal32  rhs);
bool operator>=(Decimal64  lhs, Decimal128 rhs);
bool operator>=(Decimal128 lhs, Decimal64  rhs);
    // Return 'true' if the specified 'lhs' has a value greater than or equal
    // to the value of the specified 'rhs' and 'false' otherwise.  The value of
    // a decimal object 'lhs' is greater or equal to a decimal object 'rhs' if
    // the 'compareQuietGreaterEqual' operation (IEEE-754 defined, non-total
    // ordering comparison ) considers the underlying IEEE representation of
    // 'lhs' to be greater or equal to that of 'rhs'.  In other words, 'lhs' is
    // greater than or equal to 'rhs' if:
    //
    //: o neither 'lhs' nor 'rhs' are NaN, or
    //: o 'lhs' and 'rhs' are both zero (positive or negative), or
    //: o both 'lhs' and 'rhs' are negative infinity, or
    //: o 'lhs' is positive infinity, or
    //: o 'lhs' and 'rhs' both represent a real number and the real number of
    //:   'lhs' is greater or equal to that of 'rhs'
    //
    // This operation raises the "invalid" floating-point exception if either
    // or both operands are NaN.

template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
operator>> (bsl::basic_istream<CHARTYPE, TRAITS>& stream, Decimal128& object);
    // Read, into the specified 'object', from the specified input 'stream' an
    // IEEE 128 bit decimal floating-point value as described in the IEEE-754
    // 2008 standard (5.12 Details of conversions between floating point
    // numbers and external character sequences) and return a reference
    // providing modifiable access to 'stream'.  If 'stream' contains a Nan
    // value, it is unspecified if 'object' will receive a quiet or signaling
    // 'Nan'.  If 'stream' is not valid on entry 'stream.good() == false', this
    // operation has no effect other than setting 'stream.fail()' to 'true'.
    // If eof (end-of-file) is found before any non-whitespace characters
    // 'stream.fail()' is set to 'true' and 'object' remains unchanged.  If eof
    // is detected after some characters have been read (and successfully
    // interpreted as part of the textual representation of a floating-point
    // value as specified by IEEE-754) then 'stream.eof()' is set to true.  If
    // the first non-whitespace character sequence is not a valid textual
    // representation of a floating-point value (e.g., 12e or e12 or 1*2) the
    // 'stream.fail()' is set to true and 'object' will remain unchanged.  If a
    // real number value is represented by the character sequence but it is a
    // large positive or negative value that cannot be stored into 'object' the
    // "overflow" floating-point exception is raised and positive or negative
    // infinity is stored into 'object', respectively.  If a real number value
    // is represented by the character sequence but it is a small positive or
    // negative value that cannot be stored into 'object' the "underflow"
    // floating-point exception is raised and positive or negative zero is
    // stored into 'object', respectively.  If a real number value is
    // represented by the character sequence but it cannot be stored exactly
    // into 'object' the "inexact" floating-point exception is raised, the
    // value is rounded according to the current rounding direction (of the
    // environment) and then stored into 'object.'
    //
    // NOTE: This method does not yet fully support iostream flags or the
    // decimal floating point exception context.

template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
operator<< (bsl::basic_ostream<CHARTYPE, TRAITS>& stream, Decimal128 object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single line format as described in the IEEE-754 2008
    // standard (5.12 Details of conversions between floating point numbers and
    // external character sequences), and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.
    //
    // NOTE: This method does not yet fully support iostream flags or the
    // decimal floating point exception context.


                        // MISCELLANEOUS RELATED TYPES

                          // ===================
                          // class DecimalNumGet
                          // ===================

template <class CHARTYPE,
          class INPUTITERATOR = bsl::istreambuf_iterator<CHARTYPE> >
class DecimalNumGet : public bsl::locale::facet {
    // A facet type (mechanism) used in reading decimal floating-point types.
    // Note that this type does not follow BDE conventions because its content
    // is dictated by the C++ standard and native standard library
    // implementations.  See ISO/IEC TR 24733 3.10.2 for details.

#ifdef BSLS_PLATFORM_CMP_SUN
  private:
    // ACCESSORS
    bsl::locale::id& __get_id() const;
         // The function __get_id() is a pure virtual function in the Rogue
         // Wave implementation of locales.  It is in violation with the
         // standard.  We have to define it as a workaround.
#endif

  public:
    // -dk:TODO make private while making the output operator a friend

    // CLASS METHODS
    static const DecimalNumGet<CHARTYPE, INPUTITERATOR>& object();
        // TBD

  public:
    // PUBLIC TYPES
    static bsl::locale::id id; // The locale identifier

    typedef CHARTYPE      char_type;
    typedef INPUTITERATOR iter_type;

    // CREATORS
    explicit DecimalNumGet(bsl::size_t refs = 0);
        // Constructs a 'DecimalNumGet' object as if
        //..
        // explicit DecimalNumGet(bsl::size_t refs)
        //    : bsl::locale::facet(refs), baseloc(bsl::locale()) ...
        //..
        // and optionally specify a 'refs', which will default to 0.

    explicit DecimalNumGet(const bsl::locale& b, bsl::size_t refs = 0);
        // Constructs a 'DecimalNumPut' object, from the specified 'b', as if
        //..
        // explicit DecimalNumGet(bsl::size_t refs)
        //    : bsl::locale::facet(refs), baseloc(b) ...
        //..
        // and optionally specify a 'refs', which will default to 0.

    // ACCESSORS
    iter_type get(iter_type               begin,
                  iter_type               end,
                  bsl::ios_base&          str,
                  bsl::ios_base::iostate& err,
                  Decimal32&              value) const;
    iter_type get(iter_type               begin,
                  iter_type               end,
                  bsl::ios_base&          str,
                  bsl::ios_base::iostate& err,
                  Decimal64&              value) const;
    iter_type get(iter_type               begin,
                  iter_type               end,
                  bsl::ios_base&          str,
                  bsl::ios_base::iostate& err,
                  Decimal128&             value) const;
        // Forward to, and return using the specified 'begin', 'end', 'str',
        // 'err', and 'value', the results of
        // 'this->do_get(begin, end, str, err, value)'.

  protected:
    // CREATORS
    ~DecimalNumGet();
        // Destroy this object.  Note that the destructor is virtual.

    // ACCESSORS
    virtual iter_type do_get(iter_type               begin,
                             iter_type               end,
                             bsl::ios_base&          str,
                             bsl::ios_base::iostate& err,
                             Decimal32&              value) const;
    virtual iter_type do_get(iter_type               begin,
                             iter_type               end,
                             bsl::ios_base&          str,
                             bsl::ios_base::iostate& err,
                             Decimal64&              value) const;
    virtual iter_type do_get(iter_type               begin,
                             iter_type               end,
                             bsl::ios_base&          str,
                             bsl::ios_base::iostate& err,
                             Decimal128&             value) const;
        // Interpret characters from the half-open iterator range denoted by
        // the specified 'begin' and 'end', generate a decimal floating-point
        // number and store it into the specified 'value'.  During conversion
        // the formatting flags of the specified 'str' ('str.flags()') are
        // obeyed; character classifications are determined by the 'bsl::ctype'
        // while punctuation characters are determined by the 'bsl::numpunct'
        // facet imbued to the 'str' stream-base.  Use the specified 'err' to
        // report back failure or EOF streams states.  For further, more
        // detailed information please consult the section
        // [lib.facet.num.get.virtuals] of the C++ Standard.  Note that for the
        // conversions to the 'Decimal32', 64 and 128 types the conversion
        // specifiers are %Hg, %Dg and %DDg, respectively.  Also note that
        // these (possibly overridden) 'do_get' virtual function are used by
        // every formatted C++ stream input operator call ('in >> aDecNumber').
};

                          // ===================
                          // class DecimalNumPut
                          // ===================

template <class CHARTYPE,
          class OUTPUTITERATOR = bsl::ostreambuf_iterator<CHARTYPE> >
class DecimalNumPut : public bsl::locale::facet {
    // A facet type (mechanism) used in writing decimal floating-point types.
    // Note that this type does not follow BDE conventions because its content
    // is dictated by the C++ standard and native standard library
    // implementations.  See ISO/IEC TR 24733 3.10.3 for details.

#ifdef BSLS_PLATFORM_CMP_SUN
  private:
    // ACCESSORS
    bsl::locale::id& __get_id() const;
        // The function __get_id() is a pure virtual function in the Rogue Wave
        // implementation of locales.  It is in violation with the standard.
        // We have to define it as a workaround.
#endif

  public:
    // -dk:TODO make private while making the output operator a friend

    // CLASS METHODS
    static const DecimalNumPut<CHARTYPE, OUTPUTITERATOR>& object();
        // TBD

  public:
    // PUBLIC TYPES
    static bsl::locale::id id; // The locale identifier

    typedef CHARTYPE       char_type;
    typedef OUTPUTITERATOR iter_type;

    // CREATORS
    explicit DecimalNumPut(bsl::size_t refs = 0);
        // Constructs a 'DecimalNumPut' object as if
        //..
        // explicit DecimalNumPut(bsl::size_t refs)
        //    : bsl::locale::facet(refs), baseloc(bsl::locale()) ...
        //..
        // and optionally specify 'refs', which will default to 0.

    explicit DecimalNumPut(const bsl::locale & b, bsl::size_t refs = 0);
        // Constructs a 'DecimalNumPut' object, using the specified 'b', as if
        //..
        // explicit DecimalNumPut(bsl::size_t refs)
        //    : bsl::locale::facet(refs), baseloc(b) ...
        //..
        // and optionally specify 'refs', which will default to 0.

    // ACCESSORS
    iter_type put(iter_type      out,
                  bsl::ios_base& str,
                  char_type      fill,
                  Decimal32      value) const;
    iter_type put(iter_type      out,
                  bsl::ios_base& str,
                  char_type      fill,
                  Decimal64      value) const;
    iter_type put(iter_type      out,
                  bsl::ios_base& str,
                  char_type      fill,
                  Decimal128     value) const;
        // Forward to, and return using the specified 'out', 'str', 'fill', and
        // 'value', the results of 'this->do_put(out, str, fill, value)'.

  protected:
    // CREATORS
    ~DecimalNumPut();
        // Destroy this object.  Note that the destructor is virtual.

    // ACCESSORS
    virtual iter_type do_put(iter_type      out,
                             bsl::ios_base& ios_format,
                             char_type      fill,
                             Decimal32      value) const;
    virtual iter_type do_put(iter_type      out,
                             bsl::ios_base& ios_format,
                             char_type      fill,
                             Decimal64      value) const;
    virtual iter_type do_put(iter_type      out,
                             bsl::ios_base& ios_format,
                             char_type      fill,
                             Decimal128     value) const;
        // Write characters (of 'char_type') that represent the specified
        // 'value' to the output stream determined by the specified 'out'
        // output iterator.  Use the 'bsl::ctype' and the 'bsl::numpunct'
        // facets imbued to the specified stream-base 'ios_format' as well as
        // the formatting flags of the 'ios_format' ('bsl.flags()') to generate
        // the properly localized output.  The specified 'fill' character will
        // be used as a placeholder character in padded output.  For further,
        // more detailed information please consult the section
        // [lib.facet.num.put.virtuals] of the C++ Standard noting that the
        // length modifiers "H", "D" and "DD" are added to the conversion
        // specifiers of for the types Decimal32, 64 and 128, respectively.
        // Also note that these (possibly overridden) 'do_put' virtual function
        // are used by every formatted C++ stream output operator call
        // ('out << aDecNumber').  Note that currently, only the width,
        // capitalization, and justification formatting flags are supported,
        // and the operators only support code pages that include the ASCII
        // sub-range.  Because of potential future improvements to support
        // additional formatting flags, the operations should not be used for
        // serialization.
};

                   // =====================================
                   // class Decimal_StandardNamespaceCanary
                   // =====================================

class Decimal_StandardNamespaceCanary {
    // An empty class used for error detection when looking for the original
    // name of the standard namespace.  Do not use it.
};

          // =======================================================
          // template<...> class faux_numeric_limits<NUMERIC, DUMMY>
          // =======================================================

template<class NUMERIC, class DUMMY = void>
class faux_numeric_limits;
    // This class is used as a base-class for manifest constants in the
    // 'std::numeric_limits' specializations to overcome a Sun compiler issue.

      // ===============================================================
      // class faux_numeric_limits<Decimal_StandardNamespaceCanary, ...>
      // ===============================================================

template<class DUMMY>
class faux_numeric_limits<Decimal_StandardNamespaceCanary, DUMMY>
{
    // Explicit full specialization of the standard "traits" template
    // 'std::numeric_limits' for the type
    // 'BloombergLP::bdldfp::Decimal_StandardNamespaceCanary'.  Note that this
    // specialization is required for technical reasons and it is identical to
    // the non-specialized default traits.

  public:
    // CLASS DATA
    static const bool is_specialized = false;
        // 'BloombergLP::bdldfp::Decimal_StandardNamespaceCanary' is not a
        // numeric type.
};

         // =========================================================
         // template<...> class faux_numeric_limits<Decimal32, DUMMY>
         // =========================================================

template<class DUMMY>
class faux_numeric_limits<BloombergLP::bdldfp::Decimal32, DUMMY> {
        // Explicit full specialization of the standard "traits" template
        // 'std::numeric_limits' for the type 'BloombergLP::bdldfp::Decimal32'.

  public:
    // CLASS DATA
    static const bool is_specialized = true;
        // The template instance
        // 'std::numeric_limits<BloombergLP::bdldfp::Decimal32>' is
        // meaningfully specialized.  Also means that
        // 'BloombergLP::bdldfp::Decimal32' is a numeric type.

    static const int digits = 7;
        // The maximum number of significant digits, in the native (10) radix
        // of the 'BloombergLP::bdldfp::Decimal32' type that the type is able
        // to represent.  Defined to be 7 by IEEE-754.

    static const int digits10 = digits;
        // The maximum number of significant decimal digits that the
        // 'BloombergLP::bdldfp::Decimal32' type is able to represent.  Defined
        // to be 7 by IEEE-754.

    static const int max_digits10 = digits;
        // The number of significant decimal digits necessary to uniquely
        // represent the significant digits of any
        // 'BloombergLP::bdldfp::Decimal32' value.  Note that max_digit10 is
        // the same as digits10 for decimal floating-point values.

    static const bool is_signed = true;
        // 'BloombergLP::bdldfp::Decimal32' is a signed type.

    static const bool is_integer = false;
        // 'BloombergLP::bdldfp::Decimal32' is not an integer type.

    static const bool is_exact = false;
        // 'BloombergLP::bdldfp::Decimal32' is not an exact type, i.e.:
        // calculations done on the type are not free of rounding errors.  Note
        // that integer and possibly rational types may be exact,
        // floating-point types are never exact.

    static const int radix = 10;
        // The base for 'BloombergLP::bdldfp::Decimal32' is decimal or 10.

    static const int min_exponent = -95;
        // The lowest possible negative exponent for the native base of the
        // 'BloombergLP::bdldfp::Decimal32' type that does not yet represent a
        // denormal number.  Defined to be -94 by IEEE-754.

    static const int min_exponent10 = min_exponent;
        // The lowest possible negative decimal exponent in the
        // 'BloombergLP::bdldfp::Decimal32' type that does not yet represent a
        // denormal number.  Defined to be -94 by IEEE-754.  Note that
        // 'min_exponent10' os the same as 'min_exponent' for decimal types.

    static const int max_exponent = 96;
        // The highest possible positive exponent for the native base of the
        // 'BloombergLP::bdldfp::Decimal32' type that represents a finite
        // value.  Defined to be 97 by IEEE-754.

    static const int max_exponent10 = max_exponent;
        // The highest possible positive decimal exponent of the
        // 'BloombergLP::bdldfp::Decimal32' type that represents a finite
        // value.  Defined to be 97 by IEEE-754.  Note that 'max_exponent10' os
        // the same as 'max_exponent' for decimal types.

    static const bool has_infinity = true;
        // 'BloombergLP::bdldfp::Decimal32' can represent infinity.

    static const bool has_quiet_NaN = true;
        // 'BloombergLP::bdldfp::Decimal32' can be a non-signaling Not a
        // Number.

    static const bool has_signaling_NaN = true;
        // 'BloombergLP::bdldfp::Decimal32' can be a signaling Not a Number.

    static const std::float_denorm_style has_denorm = std::denorm_present;
        // 'BloombergLP::bdldfp::Decimal32' may contain denormal values.

    static const bool has_denorm_loss = true;
        // 'BloombergLP::bdldfp::Decimal32' is able to distinguish loss of
        // precision (floating-point underflow) due to denormalization from
        // other causes.

    static const bool is_bounded = true;
        // Decimal floating-point types represent a finite set of values.

    static const bool is_iec559 = false;
        // Decimal floating-point is not covered by the IEC 559 standard.

    static const bool is_modulo = false;
        // Decimal floating-point types do not have modulo representation.

    static const bool tinyness_before = true;
        // Decimal floating-point types are able to detect if a value is too
        // small to represent as a normalized value before rounding it.

    static const bool traps = true;
        // Decimal floating-point types implement traps to report arithmetic
        // exceptions (required by IEEE-754).

                        // Rounding style

    static const std::float_round_style round_style = std::round_indeterminate;
        // Decimal floating-point rounding style is defined to be indeterminate
        // by the C and C++ Decimal TRs.
};

         // =========================================================
         // template<...> class faux_numeric_limits<Decimal64, DUMMY>
         // =========================================================

template<class DUMMY>
class faux_numeric_limits<BloombergLP::bdldfp::Decimal64, DUMMY> {
        // Explicit full specialization of the standard "traits" template
        // 'std::numeric_limits' for the type 'BloombergLP::bdldfp::Decimal64'.

  public:
    // CLASS DATA
    static const bool is_specialized = true;
        // The template instance
        // 'std::numeric_limits<BloombergLP::bdldfp::Decimal64>' is
        // meaningfully specialized.  Also means that
        // 'BloombergLP::bdldfp::Decimal64' is a numeric type.

    static const int digits = 16;
        // The maximum number of significant digits, in the native (10) radix
        // of the 'BloombergLP::bdldfp::Decimal64' type that the type is able
        // to represent.  Defined to be 16 by IEEE-754.

    static const int digits10 = digits;
        // The maximum number of significant decimal digits that the
        // 'BloombergLP::bdldfp::Decimal64' type is able to represent.  Defined
        // to be 16 by IEEE-754.

    static const int max_digits10 = digits;
        // The number of significant decimal digits necessary to uniquely
        // represent the significant digits of any
        // 'BloombergLP::bdldfp::Decimal64' value.  Note that max_digit10 is
        // the same as digits10 for decimal floating-point values.

    static const bool is_signed = true;
        // 'BloombergLP::bdldfp::Decimal64' is a signed type.

    static const bool is_integer = false;
        // 'BloombergLP::bdldfp::Decimal64' is not an integer type.

    static const bool is_exact = false;
        // 'BloombergLP::bdldfp::Decimal64' is not an exact type, i.e.:
        // calculations done on the type are not free of rounding errors.  Note
        // that integer and possibly rational types may be exact,
        // floating-point types are never exact.

    static const int radix = 10;
        // The base for 'BloombergLP::bdldfp::Decimal64' is decimal or 10.

    static const int min_exponent = -382;
        // The lowest possible negative exponent for the native base of the
        // 'BloombergLP::bdldfp::Decimal64' type that does not yet represent a
        // denormal number.  Defined to be -382 by IEEE-754.

    static const int min_exponent10 = min_exponent;
        // The lowest possible negative decimal exponent in the
        // 'BloombergLP::bdldfp::Decimal64' type that does not yet represent a
        // denormal number.  Defined to be -382 by IEEE-754.  Note that
        // 'min_exponent10' os the same as 'min_exponent' for decimal types.

    static const int max_exponent = 385;
        // The highest possible positive exponent for the native base of the
        // 'BloombergLP::bdldfp::Decimal64' type that represents a finite
        // value.  Defined to be 385 by IEEE-754.

    static const int max_exponent10 = max_exponent;
        // The highest possible positive decimal exponent of the
        // 'BloombergLP::bdldfp::Decimal64' type that represents a finite
        // value.  Defined to be 385 by IEEE-754.  Note that 'max_exponent10'
        // os the same as 'max_exponent' for decimal types.

    static const bool has_infinity = true;
        // 'BloombergLP::bdldfp::Decimal64' can represent infinity.

    static const bool has_quiet_NaN = true;
        // 'BloombergLP::bdldfp::Decimal64' can be a non-signaling Not a
        // Number.

    static const bool has_signaling_NaN = true;
        // 'BloombergLP::bdldfp::Decimal64' can be a signaling Not a Number.

    static const std::float_denorm_style has_denorm = std::denorm_present;
        // 'BloombergLP::bdldfp::Decimal64' may contain denormal values.

    static const bool has_denorm_loss = true;
        // 'BloombergLP::bdldfp::Decimal64' is able to distinguish loss of
        // precision (floating-point underflow) due to denormalization from
        // other causes.

    static const bool is_iec559 = false;
        // Decimal floating-point is not covered by the IEC 559 standard.

    static const bool is_bounded = true;
        // Decimal floating-point types represent a finite set of values.

    static const bool is_modulo = false;
        // Decimal floating-point types do not have modulo representation.

    static const bool traps = true;
        // Decimal floating-point types implement traps to report arithmetic
        // exceptions (required by IEEE-754).

    static const bool tinyness_before = true;
        // Decimal floating-point types are able to detect if a value is too
        // small to represent as a normalized value before rounding it.

    static const std::float_round_style round_style = std::round_indeterminate;
        // Decimal floating-point rounding style is defined to be indeterminate
        // by the C and C++ Decimal TRs.
};

         // ==========================================================
         // template<...> class faux_numeric_limits<Decimal128, DUMMY>
         // ==========================================================

template<class DUMMY>
class faux_numeric_limits<BloombergLP::bdldfp::Decimal128, DUMMY> {
        // Explicit full specialization of the standard "traits" template
        // 'std::numeric_limits' for the type
        // 'BloombergLP::bdldfp::Decimal128'.

  public:
    // CLASS DATA
    static const bool is_specialized = true;
        // The template instance
        // 'std::numeric_limits<BloombergLP::bdldfp::Decimal128>' is
        // meaningfully specialized.  Also means that
        // 'BloombergLP::bdldfp::Decimal128' is a numeric type.

    static const int digits = 34;
        // The maximum number of significant digits, in the native (10) radix
        // of the 'BloombergLP::bdldfp::Decimal128' type that the type is able
        // to represent.  Defined to be 34 by IEEE-754.

    static const int digits10 = digits;
        // The maximum number of significant decimal digits that the
        // 'BloombergLP::bdldfp::Decimal128' type is able to represent.
        // Defined to be 34 by IEEE-754.

    static const int max_digits10 = digits;
        // The number of significant decimal digits necessary to uniquely
        // represent the significant digits of any
        // 'BloombergLP::bdldfp::Decimal128' value.  Note that max_digit10 is
        // the same as digits10 for decimal floating-point values.

    static const bool is_signed = true;
        // 'BloombergLP::bdldfp::Decimal128' is a signed type.

    static const bool is_integer = false;
        // 'BloombergLP::bdldfp::Decimal128' is not an integer type.

    static const bool is_exact = false;
        // 'BloombergLP::bdldfp::Decimal128' is not an exact type, i.e.:
        // calculations done on the type are not free of rounding errors.  Note
        // that integer and possibly rational types may be exact,
        // floating-point types are never exact.

    static const int radix = 10;
        // The base for 'BloombergLP::bdldfp::Decimal128' is decimal or 10.

    static const int min_exponent = -6142;
        // The lowest possible negative exponent for the native base of the
        // 'BloombergLP::bdldfp::Decimal128' type that does not yet represent a
        // denormal number.  Defined to be -6142 by IEEE-754.

    static const int min_exponent10 = min_exponent;
        // The lowest possible negative decimal exponent in the
        // 'BloombergLP::bdldfp::Decimal128' type that does not yet represent a
        // denormal number.  Defined to be -6142 by IEEE-754.  Note that
        // 'min_exponent10' os the same as 'min_exponent' for decimal types.

    static const int max_exponent = 6145;
        // The highest possible positive exponent for the native base of the
        // 'BloombergLP::bdldfp::Decimal128' type that represents a finite
        // value.  Defined to be 385 by IEEE-754.

    static const int max_exponent10 = max_exponent;
        // The highest possible positive decimal exponent of the
        // 'BloombergLP::bdldfp::Decimal128' type that represents a finite
        // value.  Defined to be 6145 by IEEE-754.  Note that 'max_exponent10'
        // os the same as 'max_exponent' for decimal types.

    static const bool has_infinity = true;
        // 'BloombergLP::bdldfp::Decimal128' can represent infinity.

    static const bool has_quiet_NaN = true;
        // 'BloombergLP::bdldfp::Decimal128' can be a non-signaling Not a
        // Number.

    static const bool has_signaling_NaN = true;
        // 'BloombergLP::bdldfp::Decimal128' can be a signaling Not a Number.

    static const std::float_denorm_style has_denorm = std::denorm_present;
        // 'BloombergLP::bdldfp::Decimal128' may contain denormal values.

    static const bool has_denorm_loss = true;
        // 'BloombergLP::bdldfp::Decimal128' is able to distinguish loss of
        // precision (floating-point underflow) due to denormalization from
        // other causes.

    static const bool is_iec559 = false;
        // Decimal floating-point is not covered by the IEC 559 standard.

    static const bool is_bounded = true;
        // Decimal floating-point types represent a finite set of values.

    static const bool is_modulo = false;
        // Decimal floating-point types do not have modulo representation.

    static const bool traps = true;
        // Decimal floating-point types implement traps to report arithmetic
        // exceptions (required by IEEE-754).

    static const bool tinyness_before = true;
        // Decimal floating-point types are able to detect if a value is too
        // small to represent as a normalized value before rounding it.

    static const std::float_round_style round_style = std::round_indeterminate;
        // Decimal floating-point rounding style is defined to be indeterminate
        // by the C and C++ Decimal TRs.

};

             // --------------------------------------------------
             // faux_numeric_limits<Decimal32, ...> member storage
             // --------------------------------------------------

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::is_specialized;

template<class DUMMY>
const int faux_numeric_limits<Decimal32, DUMMY>::digits;

template<class DUMMY>
const int faux_numeric_limits<Decimal32, DUMMY>::digits10;

template<class DUMMY>
const int faux_numeric_limits<Decimal32, DUMMY>::max_digits10;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::is_signed;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::is_integer;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::is_exact;

template<class DUMMY>
const int faux_numeric_limits<Decimal32, DUMMY>::radix;

template<class DUMMY>
const int faux_numeric_limits<Decimal32, DUMMY>::min_exponent;

template<class DUMMY>
const int faux_numeric_limits<Decimal32, DUMMY>::min_exponent10;

template<class DUMMY>
const int faux_numeric_limits<Decimal32, DUMMY>::max_exponent;

template<class DUMMY>
const int faux_numeric_limits<Decimal32, DUMMY>::max_exponent10;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::has_infinity;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::has_quiet_NaN;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::has_signaling_NaN;

template<class DUMMY>
const std::float_denorm_style
faux_numeric_limits<Decimal32, DUMMY>::has_denorm;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::has_denorm_loss;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::is_iec559;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::is_bounded;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::is_modulo;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::traps;

template<class DUMMY>
const bool faux_numeric_limits<Decimal32, DUMMY>::tinyness_before;

template<class DUMMY>
const std::float_round_style
faux_numeric_limits<Decimal32, DUMMY>::round_style;

             // --------------------------------------------------
             // faux_numeric_limits<Decimal64, ...> member storage
             // --------------------------------------------------

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::is_specialized;

template<class DUMMY>
const int faux_numeric_limits<Decimal64, DUMMY>::digits;

template<class DUMMY>
const int faux_numeric_limits<Decimal64, DUMMY>::digits10;

template<class DUMMY>
const int faux_numeric_limits<Decimal64, DUMMY>::max_digits10;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::is_signed;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::is_integer;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::is_exact;

template<class DUMMY>
const int faux_numeric_limits<Decimal64, DUMMY>::radix;

template<class DUMMY>
const int faux_numeric_limits<Decimal64, DUMMY>::min_exponent;

template<class DUMMY>
const int faux_numeric_limits<Decimal64, DUMMY>::min_exponent10;

template<class DUMMY>
const int faux_numeric_limits<Decimal64, DUMMY>::max_exponent;

template<class DUMMY>
const int faux_numeric_limits<Decimal64, DUMMY>::max_exponent10;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::has_infinity;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::has_quiet_NaN;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::has_signaling_NaN;

template<class DUMMY>
const std::float_denorm_style
faux_numeric_limits<Decimal64, DUMMY>::has_denorm;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::has_denorm_loss;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::is_iec559;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::is_bounded;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::is_modulo;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::traps;

template<class DUMMY>
const bool faux_numeric_limits<Decimal64, DUMMY>::tinyness_before;

template<class DUMMY>
const std::float_round_style
faux_numeric_limits<Decimal64, DUMMY>::round_style;

            // ---------------------------------------------------
            // faux_numeric_limits<Decimal128, ...> member storage
            // ---------------------------------------------------

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::is_specialized;

template<class DUMMY>
const int faux_numeric_limits<Decimal128, DUMMY>::digits;

template<class DUMMY>
const int faux_numeric_limits<Decimal128, DUMMY>::digits10;

template<class DUMMY>
const int faux_numeric_limits<Decimal128, DUMMY>::max_digits10;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::is_signed;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::is_integer;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::is_exact;

template<class DUMMY>
const int faux_numeric_limits<Decimal128, DUMMY>::radix;

template<class DUMMY>
const int faux_numeric_limits<Decimal128, DUMMY>::min_exponent;

template<class DUMMY>
const int faux_numeric_limits<Decimal128, DUMMY>::min_exponent10;

template<class DUMMY>
const int faux_numeric_limits<Decimal128, DUMMY>::max_exponent;

template<class DUMMY>
const int faux_numeric_limits<Decimal128, DUMMY>::max_exponent10;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::has_infinity;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::has_quiet_NaN;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::has_signaling_NaN;

template<class DUMMY>
const std::float_denorm_style
faux_numeric_limits<Decimal128, DUMMY>::has_denorm;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::has_denorm_loss;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::is_iec559;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::is_bounded;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::is_modulo;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::traps;

template<class DUMMY>
const bool faux_numeric_limits<Decimal128, DUMMY>::tinyness_before;

template<class DUMMY>
const std::float_round_style
faux_numeric_limits<Decimal128, DUMMY>::round_style;

}  // close package namespace
}  // close enterprise namespace

#if defined(BSL_OVERRIDES_STD) && defined(std)
#   undef std
#   define BDLDFP_DECIMAL_RESTORE_STD
#endif
namespace std {

  // ========================================================================
  // template<> class numeric_limits<bdldfp::Decimal_StandardNamespaceCanary>
  // ========================================================================

template<>
class numeric_limits<BloombergLP::bdldfp::Decimal_StandardNamespaceCanary>
    : public BloombergLP::bdldfp::faux_numeric_limits<
        BloombergLP::bdldfp::Decimal_StandardNamespaceCanary> {
    // Explicit full specialization of the standard "traits" template
    // 'std::numeric_limits' for the type
    // 'BloombergLP::bdldfp::Decimal_StandardNamespaceCanary'.  Note that this
    // specialization is required for technical reasons and it is identical to
    // the non-specialized default traits.
};

             // ==================================================
             // template<> class numeric_limits<bdldfp::Decimal32>
             // ==================================================

template<>
class numeric_limits<BloombergLP::bdldfp::Decimal32>
    : public BloombergLP::bdldfp::faux_numeric_limits<
        BloombergLP::bdldfp::Decimal32> {
        // Explicit full specialization of the standard "traits" template
        // 'std::numeric_limits' for the type 'BloombergLP::bdldfp::Decimal32'.

  public:
    // CLASS METHODS
    static BloombergLP::bdldfp::Decimal32 min() BSLS_NOTHROW_SPEC;
        // Return the smallest positive (also non-zero) number
        // 'BloombergLP::bdldfp::Decimal32' can represent (IEEE-754: +1e-95).

    static BloombergLP::bdldfp::Decimal32 max() BSLS_NOTHROW_SPEC;
        // Return the largest number 'BloombergLP::bdldfp::Decimal32' can
        // represent (IEEE-754: +9.999999e+96).

    static BloombergLP::bdldfp::Decimal32 epsilon() BSLS_NOTHROW_SPEC;
        // Return the difference between 1 and the smallest value representable
        // by the 'BloombergLP::bdldfp::Decimal32' type.  (IEEE-754: +1e-6)

    static BloombergLP::bdldfp::Decimal32 round_error() BSLS_NOTHROW_SPEC;
        // Return the maximum rounding error for the
        // 'BloombergLP::bdldfp::Decimal32' type.  The actual value returned
        // depends on the current decimal floating point rounding setting.

    static BloombergLP::bdldfp::Decimal32 denorm_min() BSLS_NOTHROW_SPEC;
        // Return the smallest non-zero denormalized value for the
        // 'BloombergLP::bdldfp::Decimal32' type.  (IEEE-754: +0.000001E-95)

    static BloombergLP::bdldfp::Decimal32 infinity() BSLS_NOTHROW_SPEC;
        // Return the the value that represents positive infinity for the
        // 'BloombergLP::bdldfp::Decimal32' type.

    static BloombergLP::bdldfp::Decimal32 quiet_NaN() BSLS_NOTHROW_SPEC;
        // Return a value that represents non-signaling NaN for the
        // 'BloombergLP::bdldfp::Decimal32' type.

    static BloombergLP::bdldfp::Decimal32 signaling_NaN() BSLS_NOTHROW_SPEC;
        // Return a value that represents signaling NaN for the
        // 'BloombergLP::bdldfp::Decimal32' type.
};

             // ==================================================
             // template<> class numeric_limits<bdldfp::Decimal64>
             // ==================================================

template<>
class numeric_limits<BloombergLP::bdldfp::Decimal64>
    : public BloombergLP::bdldfp::faux_numeric_limits<
        BloombergLP::bdldfp::Decimal64> {
        // Explicit full specialization of the standard "traits" template
        // 'std::numeric_limits' for the type 'BloombergLP::bdldfp::Decimal64'.

  public:
    // CLASS METHODS
    static BloombergLP::bdldfp::Decimal64 min() BSLS_NOTHROW_SPEC;
        // Return the smallest positive (also non-zero) number
        // 'BloombergLP::bdldfp::Decimal64' can represent (IEEE-754: +1e-383).

    static BloombergLP::bdldfp::Decimal64 max() BSLS_NOTHROW_SPEC;
        // Return the largest number 'BloombergLP::bdldfp::Decimal64' can
        // represent (IEEE-754: +9.999999999999999e+384).

    static BloombergLP::bdldfp::Decimal64 epsilon() BSLS_NOTHROW_SPEC;
        // Return the difference between 1 and the smallest value representable
        // by the 'BloombergLP::bdldfp::Decimal64' type.  (IEEE-754: +1e-15)

    static BloombergLP::bdldfp::Decimal64 round_error() BSLS_NOTHROW_SPEC;
        // Return the maximum rounding error for the
        // 'BloombergLP::bdldfp::Decimal64' type.  The actual value returned
        // depends on the current decimal floating point rounding setting.

    static BloombergLP::bdldfp::Decimal64 denorm_min() BSLS_NOTHROW_SPEC;
        // Return the smallest non-zero denormalized value for the
        // 'BloombergLP::bdldfp::Decimal64' type.  (IEEE-754:
        // +0.000000000000001e-383)

    static BloombergLP::bdldfp::Decimal64 infinity() BSLS_NOTHROW_SPEC;
        // Return the the value that represents positive infinity for the
        // 'BloombergLP::bdldfp::Decimal64' type.

    static BloombergLP::bdldfp::Decimal64 quiet_NaN() BSLS_NOTHROW_SPEC;
        // Return a value that represents non-signaling NaN for the
        // 'BloombergLP::bdldfp::Decimal64' type.

    static BloombergLP::bdldfp::Decimal64 signaling_NaN() BSLS_NOTHROW_SPEC;
        // Return a value that represents signaling NaN for the
        // 'BloombergLP::bdldfp::Decimal64' type.

};

            // ===================================================
            // template<> class numeric_limits<bdldfp::Decimal128>
            // ===================================================

template<>
class numeric_limits<BloombergLP::bdldfp::Decimal128>
    : public BloombergLP::bdldfp::faux_numeric_limits<
        BloombergLP::bdldfp::Decimal128> {
        // Explicit full specialization of the standard "traits" template
        // 'std::numeric_limits' for the type
        // 'BloombergLP::bdldfp::Decimal128'.

  public:
    // CLASS METHODS
    static BloombergLP::bdldfp::Decimal128 min() BSLS_NOTHROW_SPEC;
        // Return the smallest positive (also non-zero) number
        // 'BloombergLP::bdldfp::Decimal128' can represent (IEEE-754:
        // +1e-6143).

    static BloombergLP::bdldfp::Decimal128 max() BSLS_NOTHROW_SPEC;
        // Return the largest number 'BloombergLP::bdldfp::Decimal128' can
        // represent (IEEE-754: +9.999999999999999999999999999999999e+6144).

    static BloombergLP::bdldfp::Decimal128 epsilon() BSLS_NOTHROW_SPEC;
        // Return the difference between 1 and the smallest value representable
        // by the 'BloombergLP::bdldfp::Decimal128' type.  (IEEE-754: +1e-33)

    static BloombergLP::bdldfp::Decimal128 round_error() BSLS_NOTHROW_SPEC;
        // Return the maximum rounding error for the
        // 'BloombergLP::bdldfp::Decimal128' type.  The actual value returned
        // depends on the current decimal floating point rounding setting.

    static BloombergLP::bdldfp::Decimal128 denorm_min() BSLS_NOTHROW_SPEC;
        // Return the smallest non-zero denormalized value for the
        // 'BloombergLP::bdldfp::Decimal128' type.  (IEEE-754:
        // +0.000000000000000000000000000000001e-6143)

    static BloombergLP::bdldfp::Decimal128 infinity() BSLS_NOTHROW_SPEC;
        // Return the the value that represents positive infinity for the
        // 'BloombergLP::bdldfp::Decimal128' type.

    static BloombergLP::bdldfp::Decimal128 quiet_NaN() BSLS_NOTHROW_SPEC;
        // Return a value that represents non-signaling NaN for the
        // 'BloombergLP::bdldfp::Decimal128' type.

    static BloombergLP::bdldfp::Decimal128 signaling_NaN() BSLS_NOTHROW_SPEC;
        // Return a value that represents signaling NaN for the
        // 'BloombergLP::bdldfp::Decimal128' type.

};

}  // close namespace std

#if defined(BDLDFP_DECIMAL_RESTORE_STD)
#   define std bsl
#   undef BDLDFP_DECIMAL_RESTORE_STD
#endif

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bdldfp {

                      // THE DECIMAL FLOATING-POINT TYPES

                            // --------------------
                            // class Decimal_Type32
                            // --------------------

inline
Decimal_Type32::Decimal_Type32()
{
    bsl::memset(&d_value, 0, sizeof(d_value));
}

inline
Decimal_Type32::Decimal_Type32(DecimalImpUtil::ValueType32 value)
: d_value(value)
{
}

inline
Decimal_Type32::Decimal_Type32(Decimal_Type64 other)
: d_value(DecimalImpUtil::convertToDecimal32(*other.data()))
{
}

inline
Decimal_Type32::Decimal_Type32(float other)
: d_value(DecimalImpUtil::binaryToDecimal32(other))
{
}

inline
Decimal_Type32::Decimal_Type32(double other)
: d_value(DecimalImpUtil::binaryToDecimal32(other))
{
}

inline
Decimal_Type32::Decimal_Type32(int other)
: d_value(DecimalImpUtil::int32ToDecimal32(other))
{
}

inline
Decimal_Type32::Decimal_Type32(unsigned int other)
: d_value(DecimalImpUtil::uint32ToDecimal32(other))
{
}

inline
Decimal_Type32::Decimal_Type32(long int other)
: d_value(DecimalImpUtil::int64ToDecimal32(other))
{
}

inline
Decimal_Type32::Decimal_Type32(unsigned long int other)
: d_value(DecimalImpUtil::uint64ToDecimal32(other))
{
}

inline
Decimal_Type32::Decimal_Type32(long long other)
: d_value(DecimalImpUtil::int64ToDecimal32(other))
{
}

inline
Decimal_Type32::Decimal_Type32(unsigned long long other)
: d_value(DecimalImpUtil::uint64ToDecimal32(other))
{
}



inline
DecimalImpUtil::ValueType32 *Decimal_Type32::data()
{
    return &d_value;
}

inline
const DecimalImpUtil::ValueType32 *Decimal_Type32::data() const
{
    return &d_value;
}

inline
DecimalImpUtil::ValueType32 Decimal_Type32::value() const
{
    return d_value;
}

                            // --------------------
                            // class Decimal_Type64
                            // --------------------

// CREATORS
inline
Decimal_Type64::Decimal_Type64()
{
    bsl::memset(&d_value, 0, sizeof(d_value));
}

inline
Decimal_Type64::Decimal_Type64(DecimalImpUtil::ValueType64 value)
: d_value(value)
{
}

inline
Decimal_Type64::Decimal_Type64(Decimal32 other)
: d_value(DecimalImpUtil::convertToDecimal64(*other.data()))
{
}

inline
Decimal_Type64::Decimal_Type64(Decimal128 other)
: d_value(DecimalImpUtil::convertToDecimal64(*other.data()))
{
}

                     // Numerical Conversion Constructors

inline
Decimal_Type64::Decimal_Type64(float other)
: d_value(DecimalImpUtil::binaryToDecimal64(other))
{
}

inline
Decimal_Type64::Decimal_Type64(double other)
: d_value(DecimalImpUtil::binaryToDecimal64(other))
{
}

                      // Integral Conversion Constructors

inline
Decimal_Type64::Decimal_Type64(int other)
: d_value(DecimalImpUtil::int32ToDecimal64(other))
{
}

inline
Decimal_Type64::Decimal_Type64(unsigned int other)
: d_value(DecimalImpUtil::uint32ToDecimal64(other))
{
}

inline
Decimal_Type64::Decimal_Type64(long other)
: d_value(DecimalImpUtil::int64ToDecimal64(other))
{
}

inline
Decimal_Type64::Decimal_Type64(unsigned long other)
: d_value(DecimalImpUtil::uint64ToDecimal64(other))
{
}

inline
Decimal_Type64::Decimal_Type64(long long other)
: d_value(DecimalImpUtil::int64ToDecimal64(other))
{
}

inline
Decimal_Type64::Decimal_Type64(unsigned long long other)
: d_value(DecimalImpUtil::uint64ToDecimal64(other))
{
}

                     // Incrementation and Decrementation

inline Decimal_Type64& Decimal_Type64::operator++()
{
    return *this += Decimal64(1);
}

inline Decimal_Type64& Decimal_Type64::operator--()
{
    return *this -= Decimal64(1);
}

                                  // Addition

inline Decimal_Type64& Decimal_Type64::operator+=(Decimal32 rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator+=(Decimal64 rhs)
{
    this->d_value = DecimalImpUtil::add(this->d_value, rhs.d_value);
    return *this;
}

inline Decimal_Type64& Decimal_Type64::operator+=(Decimal128 rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator+=(int rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator+=(unsigned int rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator+=(long rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator+=(unsigned long rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator+=(long long rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator+=(unsigned long long rhs)
{
    return *this += Decimal64(rhs);
}

                                // Subtraction

inline Decimal_Type64& Decimal_Type64::operator-=(Decimal32 rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator-=(Decimal64 rhs)
{
    this->d_value = DecimalImpUtil::subtract(this->d_value, rhs.d_value);
    return *this;
}

inline Decimal_Type64& Decimal_Type64::operator-=(Decimal128 rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator-=(int rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator-=(unsigned int rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator-=(long rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator-=(unsigned long rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator-=(long long rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator-=(unsigned long long rhs)
{
    return *this -= Decimal64(rhs);
}

                               // Multiplication

inline Decimal_Type64& Decimal_Type64::operator*=(Decimal32 rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator*=(Decimal64 rhs)
{
    this->d_value = DecimalImpUtil::multiply(this->d_value, rhs.d_value);
    return *this;
}

inline Decimal_Type64& Decimal_Type64::operator*=(Decimal128 rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator*=(int rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator*=(unsigned int rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator*=(long rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator*=(unsigned long rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator*=(long long rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator*=(unsigned long long rhs)
{
    return *this *= Decimal64(rhs);
}

                                  // Division

inline Decimal_Type64& Decimal_Type64::operator/=(Decimal32 rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator/=(Decimal64 rhs)
{
    this->d_value = DecimalImpUtil::divide(this->d_value, rhs.d_value);
    return *this;
}

inline Decimal_Type64& Decimal_Type64::operator/=(Decimal128 rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator/=(int rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator/=(unsigned int rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator/=(long rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator/=(unsigned long rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator/=(long long rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator/=(unsigned long long rhs)
{
    return *this /= Decimal64(rhs);
}

                            // Internals Accessors

inline DecimalImpUtil::ValueType64 *Decimal_Type64::data()
{
    return &d_value;
}

inline const DecimalImpUtil::ValueType64 *Decimal_Type64::data() const
{
    return &d_value;
}

inline DecimalImpUtil::ValueType64 Decimal_Type64::value() const
{
    return d_value;
}

                           // ---------------------
                           // class Decimal_Type128
                           // ---------------------

inline
Decimal_Type128::Decimal_Type128()
{
    bsl::memset(&d_value, 0, sizeof(d_value));
}

inline
Decimal_Type128::Decimal_Type128(DecimalImpUtil::ValueType128 value)
: d_value(value)
{
}

inline
Decimal_Type128::Decimal_Type128(Decimal32 value)
: d_value(DecimalImpUtil::convertToDecimal128(*value.data()))
{
}

inline
Decimal_Type128::Decimal_Type128(Decimal64 value)
: d_value(DecimalImpUtil::convertToDecimal128(*value.data()))
{
}

inline
Decimal_Type128::Decimal_Type128(float other)
: d_value(DecimalImpUtil::binaryToDecimal128(other))
{
}

inline
Decimal_Type128::Decimal_Type128(double other)
: d_value(DecimalImpUtil::binaryToDecimal128(other))
{
}

inline
Decimal_Type128::Decimal_Type128(int value)
: d_value(DecimalImpUtil::int32ToDecimal128(value))
{
}

inline Decimal_Type128::Decimal_Type128(unsigned int value)
: d_value(DecimalImpUtil::uint32ToDecimal128(value))
{
}

inline Decimal_Type128::Decimal_Type128(long value)
: d_value(DecimalImpUtil::int64ToDecimal128(value))
{
}

inline Decimal_Type128::Decimal_Type128(unsigned long value)
: d_value(DecimalImpUtil::uint64ToDecimal128(value))
{
}

inline Decimal_Type128::Decimal_Type128(long long value)
: d_value(DecimalImpUtil::int64ToDecimal128(value))
{
}

inline Decimal_Type128::Decimal_Type128(unsigned long long value)
: d_value(DecimalImpUtil::uint64ToDecimal128(value))
{
}


inline
Decimal_Type128& Decimal_Type128::operator++()
{
    return *this += Decimal128(1);
}

inline
Decimal_Type128& Decimal_Type128::operator--()
{
    return *this -= Decimal128(1);
}

                                  // Addition

inline
Decimal_Type128& Decimal_Type128::operator+=(Decimal32 rhs)
{
    return *this += Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator+=(Decimal64 rhs)
{
    return *this += Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator+=(Decimal128 rhs)
{
    this->d_value = DecimalImpUtil::add(this->d_value, rhs.d_value);
    return *this;
}

inline
Decimal_Type128& Decimal_Type128::operator+=(int rhs)
{
    return *this += Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator+=(unsigned int rhs)
{
    return *this += Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator+=(long rhs)
{
    return *this += Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator+=(unsigned long rhs)
{
    return *this += Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator+=(long long rhs)
{
    return *this += Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator+=(unsigned long long rhs)
{
    return *this += Decimal128(rhs);
}

                                // Subtraction

inline
Decimal_Type128& Decimal_Type128::operator-=(Decimal32 rhs)
{
    return *this -= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator-=(Decimal64 rhs)
{
    return *this -= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator-=(Decimal128 rhs)
{
    this->d_value = DecimalImpUtil::subtract(this->d_value, rhs.d_value);
    return *this;
}


inline
Decimal_Type128& Decimal_Type128::operator-=(int rhs)
{
    return *this -= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator-=(unsigned int rhs)
{
    return *this -= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator-=(long rhs)
{
    return *this -= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator-=(unsigned long rhs)
{
    return *this -= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator-=(long long rhs)
{
    return *this -= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator-=(unsigned long long rhs)
{
    return *this -= Decimal128(rhs);
}

                               // Multiplication

inline
Decimal_Type128& Decimal_Type128::operator*=(Decimal32 rhs)
{
    return *this *= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator*=(Decimal64 rhs)
{
    return *this *= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator*=(Decimal128 rhs)
{
    this->d_value = DecimalImpUtil::multiply(this->d_value, rhs.d_value);
    return *this;
}


inline
Decimal_Type128& Decimal_Type128::operator*=(int rhs)
{
    return *this *= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator*=(unsigned int rhs)
{
    return *this *= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator*=(long rhs)
{
    return *this *= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator*=(unsigned long rhs)
{
    return *this *= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator*=(long long rhs)
{
    return *this *= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator*=(unsigned long long rhs)
{
    return *this *= Decimal128(rhs);
}

                                  // Division

inline
Decimal_Type128& Decimal_Type128::operator/=(Decimal32 rhs)
{
    return *this /= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator/=(Decimal64 rhs)
{
    return *this /= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator/=(Decimal128 rhs)
{
    this->d_value = DecimalImpUtil::divide(this->d_value, rhs.d_value);
    return *this;
}


inline
Decimal_Type128& Decimal_Type128::operator/=(int rhs)
{
    return *this /= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator/=(unsigned int rhs)
{
    return *this /= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator/=(long rhs)
{
    return *this /= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator/=(unsigned long rhs)
{
    return *this /= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator/=(long long rhs)
{
    return *this /= Decimal128(rhs);
}

inline
Decimal_Type128& Decimal_Type128::operator/=(unsigned long long rhs)
{
    return *this /= Decimal128(rhs);
}

                            // Internals Accessors

inline
DecimalImpUtil::ValueType128 *Decimal_Type128::data()
{
    return &d_value;
}

inline
const DecimalImpUtil::ValueType128 *Decimal_Type128::data() const
{
    return &d_value;
}

inline
DecimalImpUtil::ValueType128 Decimal_Type128::value() const
{
    return d_value;
}

}  // close package namespace

// FREE OPERATORS

inline
bdldfp::Decimal32 bdldfp::operator+(bdldfp::Decimal32 value)
{
    return value;
}

inline
bdldfp::Decimal32 bdldfp::operator-(bdldfp::Decimal32 value)
{
    return Decimal32(DecimalImpUtil::negate(value.value()));
}

inline
bool bdldfp::operator==(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::equal(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator!=(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::notEqual(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator<(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::less(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator<=(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::lessEqual(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator>(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::greater(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator>=(bdldfp::Decimal32 lhs, bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::greaterEqual(*lhs.data(), *rhs.data());
}

// FREE OPERATORS

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 value)
{
    return value;
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 value)
{
    return DecimalImpUtil::negate(*value.data());
}

inline
bdldfp::Decimal64 bdldfp::operator++(bdldfp::Decimal64& value, int)
{
    bdldfp::Decimal64 result(value);
    ++value;
    return result;
}

inline
bdldfp::Decimal64 bdldfp::operator--(bdldfp::Decimal64& value, int)
{
    bdldfp::Decimal64 result(value);
    --value;
    return result;
}

                                  // Addition

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(DecimalImpUtil::add(*lhs.data(), *rhs.data()));
}

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal32 lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    bdldfp::Decimal32 rhs)
{
    return lhs + Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    int               rhs)
{
    return lhs + Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    unsigned int      rhs)
{
    return lhs + Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    long              rhs)
{
    return lhs + Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    unsigned long     rhs)
{
    return lhs + Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64 lhs,
                                    long long         rhs)
{
    return lhs + Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64  lhs,
                                    unsigned long long rhs)
{
    return lhs + Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator+(int               lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator+(unsigned int      lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator+(long              lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator+(unsigned long     lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator+(long long         lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) + rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator+(unsigned long long lhs,
                                    bdldfp::Decimal64  rhs)
{
    return Decimal64(lhs) + rhs;
}

                                // Subtraction

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(DecimalImpUtil::subtract(*lhs.data(), *rhs.data()));
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal32 lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    bdldfp::Decimal32 rhs)
{
    return lhs - Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    int               rhs)
{
    return lhs - Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    unsigned int      rhs)
{
    return lhs - Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    long              rhs)
{
    return lhs - Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    unsigned long     rhs)
{
    return lhs - Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64 lhs,
                                    long long         rhs)
{
    return lhs - Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64  lhs,
                                    unsigned long long rhs)
{
    return lhs - Decimal64(rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator-(int               lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator-(unsigned int      lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator-(long              lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator-(unsigned long     lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator-(long long         lhs,
                                    bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) - rhs;
}

inline
bdldfp::Decimal64 bdldfp::operator-(unsigned long long lhs,
                                    bdldfp::Decimal64  rhs)
{
    return Decimal64(lhs) - rhs;
}

                               // Multiplication

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(DecimalImpUtil::multiply(*lhs.data(), *rhs.data()));
}

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal32 lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           bdldfp::Decimal32 rhs)
{
    return lhs * Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           int               rhs)
{
    return lhs * Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           unsigned int      rhs)
{
    return lhs * Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           long              rhs)
{
    return lhs * Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           unsigned long     rhs)
{
    return lhs * Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64 lhs,
                                           long long         rhs)
{
    return lhs * Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64  lhs,
                                           unsigned long long rhs)
{
    return lhs * Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator*(int               lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

inline bdldfp::Decimal64 bdldfp::operator*(unsigned int      lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

inline bdldfp::Decimal64 bdldfp::operator*(long              lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

inline bdldfp::Decimal64 bdldfp::operator*(unsigned long     lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

inline bdldfp::Decimal64 bdldfp::operator*(long long         lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) * rhs;
}

inline bdldfp::Decimal64 bdldfp::operator*(unsigned long long lhs,
                                           bdldfp::Decimal64  rhs)
{
    return Decimal64(lhs) * rhs;
}

                                  // Division

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(DecimalImpUtil::divide(*lhs.data(), *rhs.data()));
}

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal32 lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           bdldfp::Decimal32 rhs)
{
    return lhs / Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           int               rhs)
{
    return lhs / Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           unsigned int      rhs)
{
    return lhs / Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           long              rhs)
{
    return lhs / Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           unsigned long     rhs)
{
    return lhs / Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64 lhs,
                                           long long         rhs)
{
    return lhs / Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64  lhs,
                                           unsigned long long rhs)
{
    return lhs / Decimal64(rhs);
}

inline bdldfp::Decimal64 bdldfp::operator/(int               lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

inline bdldfp::Decimal64 bdldfp::operator/(unsigned int      lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

inline bdldfp::Decimal64 bdldfp::operator/(long              lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

inline bdldfp::Decimal64 bdldfp::operator/(unsigned long     lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

inline bdldfp::Decimal64 bdldfp::operator/(long long         lhs,
                                           bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) / rhs;
}

inline bdldfp::Decimal64 bdldfp::operator/(unsigned long long lhs,
                                           bdldfp::Decimal64  rhs)
{
    return Decimal64(lhs) / rhs;
}

                                  // Equality

inline bool bdldfp::operator==(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::equal(*lhs.data(), *rhs.data());
}

inline bool bdldfp::operator==(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) == rhs;
}

inline bool bdldfp::operator==(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs == Decimal64(rhs);
}

                                 // Inequality

inline bool bdldfp::operator!=(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::notEqual(*lhs.data(), *rhs.data());
}

inline bool bdldfp::operator!=(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) != rhs;
}

inline bool bdldfp::operator!=(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs != Decimal64(rhs);
}

                                 // Less Than

inline bool bdldfp::operator<(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::less(*lhs.data(), *rhs.data());
}

inline bool bdldfp::operator<(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) < rhs;
}

inline bool bdldfp::operator<(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs < Decimal64(rhs);
}

                                 // Less Equal

inline bool bdldfp::operator<=(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::lessEqual(*lhs.data(), *rhs.data());
}

inline bool bdldfp::operator<=(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) <= rhs;
}

inline bool bdldfp::operator<=(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs <= Decimal64(rhs);
}

                                // Greater Than

inline bool bdldfp::operator>(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::greater(*lhs.data(), *rhs.data());
}

inline bool bdldfp::operator>(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) > rhs;
}

inline bool bdldfp::operator>(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs > Decimal64(rhs);
}

                               // Greater Equal

inline bool bdldfp::operator>=(bdldfp::Decimal64 lhs, bdldfp::Decimal64 rhs)
{
    return DecimalImpUtil::greaterEqual(*lhs.data(), *rhs.data());
}

inline bool bdldfp::operator>=(bdldfp::Decimal32 lhs, bdldfp::Decimal64 rhs)
{
    return Decimal64(lhs) >= rhs;
}

inline bool bdldfp::operator>=(bdldfp::Decimal64 lhs, bdldfp::Decimal32 rhs)
{
    return lhs >= Decimal64(rhs);
}

// FREE OPERATORS

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 value)
{
    return value;
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 value)
{
    return Decimal128(DecimalImpUtil::negate(*value.data()));
}

inline
bdldfp::Decimal128 bdldfp::operator++(bdldfp::Decimal128& value, int)
{
    Decimal128 result = value;
    ++value;
    return result;
}

inline
bdldfp::Decimal128 bdldfp::operator--(bdldfp::Decimal128& value, int)
{
    Decimal128 result = value;
    --value;
    return result;
}

                                  // Addition

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(DecimalImpUtil::add(*lhs.data(), *rhs.data()));
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal32  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal32  rhs)
{
    return lhs + Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal64  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal64  rhs)
{
    return lhs + Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     int                rhs)
{
    return lhs + Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     unsigned int       rhs)
{
    return lhs + Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     long               rhs)
{
    return lhs + Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     unsigned long      rhs)
{
    return lhs + Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     long long          rhs)
{
    return lhs + Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator+(bdldfp::Decimal128 lhs,
                                     unsigned long long rhs)
{
    return lhs + Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator+(int                lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator+(unsigned int       lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator+(long               lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator+(unsigned long      lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator+(long long          lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator+(unsigned long long lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) + rhs;
}

                                // Subtraction

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(DecimalImpUtil::subtract(*lhs.data(), *rhs.data()));
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal32  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal32  rhs)
{
    return lhs - Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal64  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal64  rhs)
{
    return lhs - Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     int                rhs)
{
    return lhs - Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     unsigned int       rhs)
{
    return lhs - Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     long               rhs)
{
    return lhs - Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     unsigned long      rhs)
{
    return lhs - Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     long long          rhs)
{
    return lhs - Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator-(bdldfp::Decimal128 lhs,
                                     unsigned long long rhs)
{
    return lhs - Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator-(int                lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator-(unsigned int       lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator-(long               lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator-(unsigned long      lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator-(long long          lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator-(unsigned long long lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) - rhs;
}

                               // Multiplication

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(DecimalImpUtil::multiply(*lhs.data(), *rhs.data()));
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal32  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal32  rhs)
{
    return lhs * Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal64  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal64  rhs)
{
    return lhs * Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     int                rhs)
{
    return lhs * Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     unsigned int       rhs)
{
    return lhs * Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     long               rhs)
{
    return lhs * Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     unsigned long      rhs)
{
    return lhs * Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     long long          rhs)
{
    return lhs * Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator*(bdldfp::Decimal128 lhs,
                                     unsigned long long rhs)
{
    return lhs * Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator*(int                lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator*(unsigned int       lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator*(long               lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator*(unsigned long      lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator*(long long          lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator*(unsigned long long lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) * rhs;
}

                                  // Division

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(DecimalImpUtil::divide(*lhs.data(), *rhs.data()));
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal32  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal32  rhs)
{
    return lhs / Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal64  lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     bdldfp::Decimal64  rhs)
{
    return lhs / Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     int                rhs)
{
    return lhs / Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     unsigned int       rhs)
{
    return lhs / Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     long               rhs)
{
    return lhs / Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     unsigned long      rhs)
{
    return lhs / Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     long long          rhs)
{
    return lhs / Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator/(bdldfp::Decimal128 lhs,
                                     unsigned long long rhs)
{
    return lhs / Decimal128(rhs);
}

inline
bdldfp::Decimal128 bdldfp::operator/(int                lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator/(unsigned int       lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator/(long               lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator/(unsigned long      lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator/(long long          lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}

inline
bdldfp::Decimal128 bdldfp::operator/(unsigned long long lhs,
                                     bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) / rhs;
}

                                  // Equality

inline
bool bdldfp::operator==(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::equal(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator==(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) == rhs;
}

inline
bool bdldfp::operator==(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs == Decimal128(rhs);
}

inline
bool bdldfp::operator==(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) == rhs;
}

inline
bool bdldfp::operator==(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs == Decimal128(rhs);
}

                                 // Inequality

inline
bool bdldfp::operator!=(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::notEqual(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator!=(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) != rhs;
}

inline
bool bdldfp::operator!=(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs != Decimal128(rhs);
}

inline
bool bdldfp::operator!=(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) != rhs;
}

inline
bool bdldfp::operator!=(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs != Decimal128(rhs);
}

                                 // Less Than

inline
bool bdldfp::operator<(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::less(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator<(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) < rhs;
}

inline
bool bdldfp::operator<(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs < Decimal128(rhs);
}

inline
bool bdldfp::operator<(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) < rhs;
}

inline
bool bdldfp::operator<(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs < Decimal128(rhs);
}

                                 // Less Equal

inline
bool bdldfp::operator<=(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::lessEqual(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator<=(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) <= rhs;
}

inline
bool bdldfp::operator<=(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs <= Decimal128(rhs);
}

inline
bool bdldfp::operator<=(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) <= rhs;
}

inline
bool bdldfp::operator<=(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs <= Decimal128(rhs);
}

                                  // Greater

inline
bool bdldfp::operator>(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::greater(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator>(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) > rhs;
}

inline
bool bdldfp::operator>(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs > Decimal128(rhs);
}

inline
bool bdldfp::operator>(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) > rhs;
}

inline
bool bdldfp::operator>(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs > Decimal128(rhs);
}

                               // Greater Equal

inline
bool bdldfp::operator>=(bdldfp::Decimal128 lhs, bdldfp::Decimal128 rhs)
{
    return DecimalImpUtil::greaterEqual(*lhs.data(), *rhs.data());
}

inline
bool bdldfp::operator>=(bdldfp::Decimal32 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) >= rhs;
}

inline
bool bdldfp::operator>=(bdldfp::Decimal128 lhs, bdldfp::Decimal32 rhs)
{
    return lhs >= Decimal128(rhs);
}

inline
bool bdldfp::operator>=(bdldfp::Decimal64 lhs, bdldfp::Decimal128 rhs)
{
    return Decimal128(lhs) >= rhs;
}

inline
bool bdldfp::operator>=(bdldfp::Decimal128 lhs, bdldfp::Decimal64 rhs)
{
    return lhs >= Decimal128(rhs);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
