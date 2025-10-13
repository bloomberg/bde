// bdldfp_decimal.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMAL
#define INCLUDED_BDLDFP_DECIMAL

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide IEEE-754 decimal floating-point types.
//
//@CLASSES:
//  bdldfp::Decimal32:   32bit IEEE-754 decimal floating-point type
//  bdldfp::Decimal64:   64bit IEEE-754 decimal floating-point type
//  bdldfp::Decimal128: 128bit IEEE-754 decimal floating-point type
//  bdldfp::DecimalNumGet: Stream Input Facet
//  bdldfp::DecimalNumPut: Stream Output Facet
//  template bsl::formatter<bdldfp::Decimal32, t_CHAR>: formatter impl
//  template bsl::formatter<bdldfp::Decimal64, t_CHAR>: formatter impl
//  template bsl::formatter<bdldfp::Decimal128, t_CHAR>: formatter impl
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
// These classes are `bdldfp::Decimal32` for 32-bit Decimal floating point
// numbers, `bdldfp::Decimal64` for 64-bit Decimal floating point numbers, and
// `bdldfp::Decimal128` for 128-bit decimal floating point numbers.
//
// Decimal encoded floating-point numbers are important where exact
// representation of decimal fractions is required, such as in financial
// transactions.  Binary encoded floating-point numbers are generally optimal
// for complex computation but cannot exactly represent commonly encountered
// numbers such as 0.1, 0.2, and 0.99.
//
// NOTE: Interconversion between binary and decimal floating-point values is
// fraught with misunderstanding and must be done carefully and with intent,
// taking into account the provenance of the data.  See the discussion on
// conversion below and in the `bdldfp_decimalconvertutil` component.
//
// The BDE decimal floating-point system has been designed from the ground up
// to be portable and support writing portable decimal floating-point user
// code, even for systems that do not have compiler or native library support
// for it; while taking advantage of native support (such as ISO/IEC TR
// 24732 - C99 decimal TR) when available.
//
// `bdldfp::DecimalNumGet` and `bdldfp::DecimalNumPut` are IO stream facets.
//
// This component also provides support for `bsl::format` formatting as
// described in {`bsl::format` Format Strings}
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
// ```
//  Significand | Exponent | Value        |
// -------------+----------+--------------+  In the Value column you may
//      1234567 |        0 |   1234567.0  |  observer how the decimal point
//      1234567 |        1 |  12345670.0  |  is "floating" about the digits
//      1234567 |        2 | 123456700.0  |  of the significand.
//      1234567 |       -1 |    123456.7  |
//      1234567 |       -2 |     12345.67 |
// ```
// Floating-point numbers are standardized by IEEE-754 2008, in two major
// flavors: binary and decimal.  Binary floating-point numbers are supported by
// most computer systems in the forms of the `float`, `double` and
// `long double` fundamental data types.  While they are not required to be
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
// 1. Floating-point types cannot exactly represent every number in their
//    range.  The consequences are surprising and unexpected for the newcomer.
//    For example: when using binary floating-point numbers, the following
//    expression is typically *false*: `0.1 + 0.2 == 0.3`.  The problem is not
//    limited to binary floating-point.  Decimal floating-point cannot
//    represent the value of one third exactly.
// 2. Unlike with real numbers, the order of operations on floating-point
//    numbers is significant, due to accumulation of round off errors.
//    Therefore floating-point arithmetic is neither commutative nor
//    transitive.  E.g., 2e-30 + 1e30 - 1e-30 - 1e30 will typically produce 0
//    (unless your significand can hold 60 decimal digits).  Alternatively,
//:   1e30 - 1e30 + 2e-30 - 1e-30 will typically produce 1e-30.
//:
// 3. IEEE floating-point types can have special values: negative zero,
//    negative and positive infinity; and they can be NaN (Not a Number, in two
//    variants: quiet or signaling).  A NaN (any variant) is never equal to
//    anything else - including NaN or itself!
// 4. In IEEE floating-point there are at least two representations of 0, the
//    positive zero and negative zero.  Consequently unary - operators change
//    the sign of the value 0; therefore leading to surprising results: if
//    `f == 0.0` then `0 - f` and `-f` will not result in the same value,
//    because `0 - f` will be +0.0' while `-f` will be -0.0.
// 5. Most IEEE floating-point operations (like arithmetic) have implicit input
//    parameters and output parameters (that do not show up in function
//    signatures.  The implicit input parameters are called *attributes* by
//    IEEE while the outputs are called status flags.  The C/C++ programming
//    language defines a so-called floating-point environment that contains
//    those attributes and flags (`<fenv.h>` C and `<cfenv>` C++ headers).  To
//    learn more about the floating point environment read the subsection of
//    the same title, but first make sure you read the next point as well.
// 6. IEEE floating-points overloads some very common programming language
//    terms: *exception*, *signal* and *handler* with IEEE floating-point
//    specific meanings that are not to be confused with C or C++ or Posix
//    terms of the same spelling.  Floating-point exceptions are events that
//    occur when a floating-point operations on the specified operands is
//    unable to produce a perfect outcome; such as when the result of an
//    operation is inexact.  When a floating point exception occurs the
//    (floating-point) - and reporting it is requested by a so-called trap
//    attribute - the implementation signals the user(*) by invoking a default
//    or a user-defined handler.  None of the words *exception*, *signal*, and
//    *handler* used above have nothing to do with C++ exceptions, Posix
//    signals and the handlers of those.  (To complicate matters more, C and
//    Posix has decided to implement IEEE floating-point exception reporting as
//    C/Posix signals - and therefore rendered them mostly useless.)
// 7. While a 32bit integer is a quite useful type for (integer) calculations,
//    a 32bit floating-point type has such low accuracy (its significand is so
//    short) that it is all but useless for calculation.  Such types are called
//    "interchange formats" by the IEEE standard and should not be used for
//    calculations.  (Except in special circumstances and by floating-point
//    experts.  Even a 16 bit binary floating-point type can be useful for an
//    expert in special circumstances, for example in graphics acceleration
//    hardware.)
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
// An example of a generally applicable rounding mode would be `FE_TOWARDZERO`
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
// The flags work much like individual, boolean `errno` values.  Operations may
// set them to true.  Users may examine them (when interested) and also reset
// them (set them to 0) before an operation.
//
///Floating-Point Traps
/// -  -  -  -  -  -  -
// IEEE says that certain floating-point events are floating-point exceptions
// and they result in invoking a handler.  It may be a default handler (set a
// status flag and continue) or a user defined handler.  Floating point traps
// are a C invention to enable "sort-of handlers" for floating point
// exceptions, but unfortunately they all go to the same handler: the `SIGFPE`
// handler.  To add insult to injury, setting what traps are active (what will)
// cause a `SIGFPE`) is not standardized.  So floating-point exceptions and
// handlers are considered pretty much useless in C.  (All is not lost, since
// we do have the status flags.  An application that wants to know about
// floating-point events can clear the flags prior to an operation and check
// their values afterwards.)
//
/// Error Reporting
///-  -  -  -  -  -
// The `bdldfp_decimalutil` utility component provides a set of decimal math
// functions that parallel those provided for binary floating point in the C++
// standard math library.  Errors during computation of these functions (e.g.,
// domain errors) will be reported through the setting of `errno` as described
// in the "Status Flags" section above.  (Note that this method of reporting
// errors is atypical for BDE-provided interfaces, but matches the style used
// by the standard functions.)
//
///Floating-Point Terminology
/// - - - - - - - - - - - - -
// A floating-point representation of a number is defined as follows:
// `sign * significand * BASE^exponent`, where sign is -1 or +1, significand is
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
//:    number is: `sign * significand * base^exponent`
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
//:    `1 <= significand <= base`
//:
//: "normalization":
//:    finding the exponent such as `1 <= significand <= base`
//:
//: "denormal number":
//:    `significand < 1`
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
// with binary floating-point `0.1 + 0.2 != 0.3`.  (Why?  Because none of the
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
// less numbers (than their binary counterparts) and being slower, but
// providing exact representations for the numbers humans care about.
//
// In decimal floating-point world `0.1 + 0.2 == 0.3`, as humans expect;
// because each of those 3 numbers can be represented *exactly* in a decimal
// floating-point format.
//
///*WARNING*: Conversions from `float` and `double`
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Clients should *be* *careful* when using the conversions from `float` and
// `double` provided by this component.  In situations where a `float` or
// `double` was originally obtained from a decimal floating point
// representation (e.g., a `bdldfp::Decimal`, or a string, like "4.1"), the
// conversions in `bdldfp_decimalconvertutil` will provide the correct
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
// significant digits.  We would display the former number as `1.00` and the
// latter number as `1.00000`.  The decimal floating-point types are able to
// store both numbers *and* their precision using so called cohorts.  The
// `1.00` will be stored as `100e-2` while `1.00000` will be stored as
// `100000e-5`.
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
///Arithmetic And Computing Support For `Decimal32`
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// IEEE-754 designates the 32 bit floating-point types "interchange formats"
// and does not require or recommend arithmetic or computing support of any
// kind for them.  The C (and consequently the C++) TR goes against the IEEE
// design and requires `_Decimal32` (and `std::decimal32`) to provide computing
// support, however, in a twist, allows it to be performed using one of the
// larger types (64 or 128 bits).  The rationale from the C committee is that
// small embedded systems may need to do their calculations using the small
// type (so they have made it mandatory for everyone).  To conform the
// requirement we provide arithmetic and computing support for Decimal32 type
// but users need to be aware of the drawbacks of calculations using the small
// type. Industry experience with the `float` C type (32bit floating-point
// type, usually binary) has shown that enabling computing using small
// floating-point types are a mistake that causes novice programmers to write
// calculations that are very slow and inaccurate.
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
///`Decimal32` Type
///----------------
// A basic format type that supports input, output, relational operators
// construction from the TR mandates data types and arithmetic or operations.
// The type has the size of exactly 32 bits.  It supports 7 significant decimal
// digits and an exponent range of -95 to 96.  The smallest non-zero value that
// can be represented is 1e-101.
//
// Portable `Decimal32` literals are created using the `BDLDFP_DECIMAL_DF`
// macro.
//
///`Decimal64` Type
///----------------
// A basic format type that supports input, output, relational operators
// construction from the TR mandates data types and arithmetic or operations.
// The type has the size of exactly 64 bits.  It supports 16 significant
// decimal digits and an exponent range of -383 to 384.  The smallest non-zero
// value that can be represented is 1e-398.
//
// Portable `Decimal64` literals are created using the `BDLDFP_DECIMAL_DD`
// macro.
//
///`Decimal128` Type
///-----------------
// A basic format type that supports input, output, relational operators
// construction from the TR mandates data types and arithmetic or operations.
// The type has the size of exactly 128 bits.  It supports 34 significant
// decimal digits and an exponent range of -6143 to 6144.  The smallest
// non-zero value that can be represented is 1e-6176.
//
// Portable `Decimal128` literals are created using the `BDLDFP_DECIMAL_DL`
// macro.
//
///Decimal Number Stream-based Formatting
///--------------------------------------
// Streaming decimal floating point numbers to an output stream supports
// formatting flags for width, capitalization and justification and flags used
// to output numbers in natural, scientific and fixed notations.  When
// scientific or fixed flags are set then the precision manipulator specifies
// how many digits of the decimal number are to be printed, otherwise all
// significant digits of the decimal number are output using native notation.
//
///`bsl::format` Format Strings
///----------------------------
// `bsl::format` formatting is supported with format strings very similar to
// binary floating point formats strings, with two major exceptions: there is
// no support for hexadecimal format, and the default precision is the
// precision stored within the decimal floating point number (see {Cohorts}),
// unlike binary floating point where it is always 6.
//
// The formats supported are the so-called General, Fixed, Scientific, and
// their uppercase equivalents.
//
// The sign support is the usual: Negative-only,  Always, and
// Positive-as-space.
//
// The alternative format flag ('#') causes the decimal point to be always
// written (but does not remove trailing zeros).
//
// Alignment, fill, and leading zeros are supported as usual.
//
///User-defined literals
///---------------------
// The user-defined literal `operator""_d32`, `operator""_d64`, and
// `operator""_d128` are declared for the `bdldfp::Decimal32`,
// `bdldfp::Decimal64`, and `bdldfp::Decimal128` types respectively .  These
// user-defined literal suffixes can be applied to both numeric and string
// literals, (i.e., 1.2_d128, "1.2"_d128 or "inf"_d128) to produce a decimal
// floating-point value of the indicated type by parsing the argument string
// or numeric value:
// ```
// using namespace bdldfp::DecimalLiterals;
//
// bdldfp::Decimal32   d0  = "1.2"_d32;
// bdldfp::Decimal32   d1  =  1.2_d32;
// assert(d0 == d1);
//
// bdldfp::Decimal64   d2  = "3.45678901234"_d64;
// bdldfp::Decimal64   d3  =  3.45678901234_d64;
// assert(d2 == d3);
//
// bdldfp::Decimal128  inf = "inf"_d128;
// bdldfp::Decimal128  nan = "nan"_d128;
// ```
// The operators providing literals are available in the
// `BloombergLP::bdldfp::literals::DecimalLiterals` namespace (where `literals`
// and `DecimalLiterals` are both inline namespaces). Because of inline
// namespaces, there are several viable options for a using declaration, but
// *we* *recommend* `using namespace bdldfp::DecimalLiterals`, which minimizes
// the scope of the using declaration.
//
// Note that the parsing follows the rules as specified for the `strtod32`,
// `strtod64` and `strtod128` functions in section 9.6 of the ISO/EIC TR 247128
// C Decimal Floating-Point Technical Report.
//
// Also note that these operators can be used only if the compiler supports
// C++11 standard.
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
// ```
// bdldfp::Decimal32  d32( BDLDFP_DECIMAL_DF(0.1));
// bdldfp::Decimal64  d64( BDLDFP_DECIMAL_DD(0.2));
// bdldfp::Decimal128 d128(BDLDFP_DECIMAL_DL(0.3));
//
// assert(d32 + d64 == d128);
// assert(bdldfp::Decimal64(d32)  * 10 == bdldfp::Decimal64(1));
// assert(d64  * 10 == bdldfp::Decimal64(2));
// assert(d128 * 10 == bdldfp::Decimal128(3));
// ```
//
///Example 2: Precise Calculations with Decimal Values
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need to add two (decimal) numbers and then tell if the result is
// a particular decimal number or not.  That can get difficult with binary
// floating-point, but easy with decimal:
// ```
// if (std::numeric_limits<double>::radix == 2) {
//   assert(.1 + .2 != .3);
// }
// assert(BDLDFP_DECIMAL_DD(0.1) + BDLDFP_DECIMAL_DD(0.2)
//     == BDLDFP_DECIMAL_DD(0.3));
// ```

#include <bdldfp_decimal.fwd.h>

#include <bdlscm_version.h>

#include <bdldfp_decimalimputil.h>
#include <bdldfp_decimalstorage.h>

#include <bsla_fallthrough.h>

#include <bslh_hash.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatspecificationparser.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formattercharutil.h>

#include <bslma_deallocatebytesproctor.h>
#include <bslma_default.h>
#include <bslma_polymorphicallocator.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <bsl_cstddef.h>
#include <bsl_cstring.h>
#include <bsl_ios.h>
#include <bsl_iosfwd.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_locale.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
  #include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

             // Portable decimal floating-point literal support

#define BDLDFP_DECIMAL_DF(lit)                                                \
    BloombergLP::bdldfp::Decimal32(BDLDFP_DECIMALIMPUTIL_DF(lit))

#define BDLDFP_DECIMAL_DD(lit)                                                \
    BloombergLP::bdldfp::Decimal64(BDLDFP_DECIMALIMPUTIL_DD(lit))

#define BDLDFP_DECIMAL_DL(lit)                                                \
    BloombergLP::bdldfp::Decimal128(BDLDFP_DECIMALIMPUTIL_DL(lit))

namespace BloombergLP {
namespace bdldfp {

typedef Decimal_Type32  Decimal32;
typedef Decimal_Type64  Decimal64;

/// The decimal floating-point types are typedefs to the unspecified
/// implementation types.
typedef Decimal_Type128 Decimal128;

                            // ====================
                            // class Decimal_Type32
                            // ====================

/// This value-semantic class implements the IEEE-754 32 bit decimal
/// floating-point interchange format type.  This class is a standard layout
/// type that is `const` thread-safe and exception-neutral.
class Decimal_Type32 {

  private:
    // DATA
    DecimalImpUtil::ValueType32 d_value; // The underlying IEEE representation

  public:
    // CLASS METHODS

                                  // Aspects

    /// Return the maximum valid BDEX format version, as indicated by the
    /// specified `versionSelector`, to be passed to the `bdexStreamOut`
    /// method.  Note that it is highly recommended that `versionSelector`
    /// be formatted as "YYYYMMDD", a date representation.  Also note that
    /// `versionSelector` should be a *compile*-time-chosen value that
    /// selects a format version supported by both externalizer and
    /// unexternalizer.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    static int maxSupportedBdexVersion();
    static int maxSupportedBdexVersion(int versionSelector);

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Decimal_Type32,
                                   bsl::is_trivially_copyable);

    // CREATORS

    /// Create a `Decimal32_Type` object having the value positive zero and
    /// the smallest exponent value.
    Decimal_Type32();

    /// Create a `Decimal32_Type` object having the specified `value`.
    Decimal_Type32(DecimalImpUtil::ValueType32 value);              // IMPLICIT

    /// Create a `Decimal32_Type` object having the value closest to the
    /// value of the specified `other` following the conversion rules as
    /// defined by IEEE-754:
    ///
    /// * If `other` is NaN, initialize this object to a NaN.
    /// * Otherwise if `other` is infinity (positive or negative), then
    ///   initialize this object to infinity with the same sign.
    /// * Otherwise if `other` has a zero value, then initialize this
    ///   object to zero with the same sign.
    /// * Otherwise if `other` has an absolute value that is larger than
    ///   `std::numeric_limits<Decimal32>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and initialize this object to
    ///   infinity with the same sign as `other`.
    /// * Otherwise if `other` has an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal32>::min()` then store the value of
    ///   the macro `ERANGE` into `errno` and initialize this object to
    ///   zero with the same sign as `other`.
    /// * Otherwise if `other` has a value that has more significant digits
    ///   than `std::numeric_limits<Decimal32>::max_digit` then initialize
    ///   this object to the value of `other` rounded according to the
    ///   rounding direction.
    /// * Otherwise initialize this object to the value of the `other`.
    explicit Decimal_Type32(Decimal_Type64  other);
    explicit Decimal_Type32(Decimal_Type128 other);

    /// Create a `Decimal32_Type` object having the value closest to the
    /// value of the specified `other` value.  *Warning:* clients requiring
    /// a conversion for an exact decimal value should use
    /// `bdldfp_decimalconvertutil` (see *WARNING*: Conversions from
    /// `float` and `double`}.  This conversion follows the conversion
    /// rules as defined by IEEE-754:
    ///
    /// * If `other` is NaN, initialize this object to a NaN.
    /// * Otherwise if `other` is infinity (positive or negative), then
    ///   initialize this object to infinity value with the same sign.
    /// * Otherwise if `other` has a zero value, then initialize this
    ///   object to zero with the same sign.
    /// * Otherwise if `other` has an absolute value that is larger than
    ///   `std::numeric_limits<Decimal32>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and initialize this object to
    ///   infinity with the same sign as `other`.
    /// * Otherwise if `other` has an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal32>::min()` then store the value of
    ///   the macro `ERANGE` into `errno` and initialize this object to
    ///   zero with the same sign as `other`.
    /// * Otherwise if `other` has a value that has more significant digits
    ///   than `std::numeric_limits<Decimal32>::max_digit` then initialize
    ///   this object to the value of `other` rounded according to the
    ///   rounding direction.
    /// * Otherwise initialize this object to the value of the `other`.
    explicit Decimal_Type32(float       other);
    explicit Decimal_Type32(double      other);

    /// Create a `Decimal32_Type` object having the value closest to the
    /// value of the specified `other` following the conversion rules as
    /// defined by IEEE-754:
    ///
    /// * If `value` is zero then initialize this object to a zero with an
    ///   unspecified sign and an unspecified exponent.
    /// * Otherwise if `other` has a value that is not exactly
    ///   representable using `std::numeric_limits<Decimal32>::max_digit`
    ///   decimal digits then initialize this object to the value of
    ///   `other` rounded according to the rounding direction.
    /// * Otherwise initialize this object to the value of `other` with
    ///   exponent 0.
    explicit Decimal_Type32(int                other);
    explicit Decimal_Type32(unsigned int       other);
    explicit Decimal_Type32(long int           other);
    explicit Decimal_Type32(unsigned long int  other);
    explicit Decimal_Type32(long long          other);
    explicit Decimal_Type32(unsigned long long other);

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

    /// Add 1.0 to the value of this object and return a reference to it.
    /// Note that this is a floating-point value so this operation may not
    /// change the value of this object at all (if the value is large) or it
    /// may just set it to 1.0 (if the original value is small).
    Decimal_Type32& operator++();

    /// Add -1.0 to the value of this object and return a reference to it.
    /// Note that this is a floating-point value so this operation may not
    /// change the value of this object at all (if the value is large) or it
    /// may just set it to -1.0 (if the original value is small).
    Decimal_Type32& operator--();

    /// Add the value of the specified `rhs` object to the value of this as
    /// described by IEEE-754, store the result in this object, and return a
    /// reference to this object.
    ///
    /// * If either of this object or `rhs` is signaling NaN, then store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise if this object and `rhs` have infinity value of
    ///   differing signs, store the value of the macro `EDOM` into `errno`
    ///   and set this object to a NaN.
    /// * Otherwise if this object and `rhs` have infinite values of the
    ///   same sign, then do not change this object.
    /// * Otherwise if `rhs` has a zero value (positive or negative), do
    ///   not change this object.
    /// * Otherwise if the sum of this object and `rhs` has an absolute
    ///   value that is larger than `std::numeric_limits<Decimal32>::max()`
    ///   then store the value of the macro `ERANGE` into `errno` and
    ///   set this object to infinity with the same sign as that result.
    /// * Otherwise set this object to the sum of the number represented by
    ///   `rhs` and the number represented by this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    ///
    /// Also note that when `rhs` is a `Decimal64`, this operation is
    /// always performed with 64 bits precision to prevent loss of
    /// precision of the `rhs` operand (prior to the operation).  The
    /// result is then rounded back to 32 bits and stored to this object.
    /// See IEEE-754 2008, 5.1, first paragraph, second sentence for
    /// specification.
    ///
    /// Also note that when `rhs` is a `Decimal128`, this operation is
    /// always performed with 128 bits precision to prevent loss of
    /// precision of the `rhs` operand (prior to the operation).  The
    /// result is then rounded back to 32 bits and stored to this object.
    /// See IEEE-754 2008, 5.1, first paragraph, second sentence for
    /// specification.
    Decimal_Type32& operator+=(Decimal32  rhs);
    Decimal_Type32& operator+=(Decimal64  rhs);
    Decimal_Type32& operator+=(Decimal128 rhs);

    /// Add the specified `rhs` to the value of this object as described by
    /// IEEE-754, store the result in this object, and return a reference to
    /// this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN, then do not change this object.
    /// * Otherwise if this object is infinity, then do not change it.
    /// * Otherwise if the sum of this object and `rhs` has an absolute
    ///   value that is larger than `std::numeric_limits<Decimal32>::max()`
    ///   then store the value of the macro `ERANGE` into `errno` and
    ///   set this object to infinity with the same sign as that result.
    /// * Otherwise set this object to sum of adding `rhs` and the number
    ///   represented by this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    ///
    /// Also note that this operation is always performed with 64 bits
    /// precision to prevent loss of precision of the `rhs` operand (prior
    /// to the operation).  The result is then rounded back to 32 bits and
    /// stored to this object.  See IEEE-754 2008, 5.1, first paragraph,
    /// second sentence for specification.
    Decimal_Type32& operator+=(int                rhs);
    Decimal_Type32& operator+=(unsigned int       rhs);
    Decimal_Type32& operator+=(long               rhs);
    Decimal_Type32& operator+=(unsigned long      rhs);
    Decimal_Type32& operator+=(long long          rhs);
    Decimal_Type32& operator+=(unsigned long long rhs);


    /// Subtract the value of the specified `rhs` from the value of this
    /// object as described by IEEE-754, store the result in this object,
    /// and return a reference to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise if this object and `rhs` have infinity value of the
    ///   same signs, store the value of the macro `EDOM` into `errno`
    ///   and set this object to a NaN.
    /// * Otherwise if this object and the `rhs` have infinite values of
    ///   differing signs, then do not change this object.
    /// * Otherwise if the `rhs` has a zero value (positive or negative),
    ///   do not change this object.
    /// * Otherwise if subtracting the value of the `rhs` object from this
    ///   results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal32>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise set this object to the result of subtracting the value
    ///   of `rhs` from the value of this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    ///
    /// Also note that when `rhs` is a `Decimal64`, this operation is
    /// always performed with 64 bits precision to prevent loss of
    /// precision of the `rhs` operand (prior to the operation).  The
    /// result is then rounded back to 32 bits and stored to this object.
    /// See IEEE-754 2008, 5.1, first paragraph, second sentence for
    /// specification.
    ///
    /// Also note that when `rhs` is a `Decimal128`, this operation is
    /// always performed with 128 bits precision to prevent loss of
    /// precision of the `rhs` operand (prior to the operation).  The
    /// result is then rounded back to 32 bits and stored to this object.
    /// See IEEE-754 2008, 5.1, first paragraph, second sentence for
    /// specification.
    Decimal_Type32& operator-=(Decimal32  rhs);
    Decimal_Type32& operator-=(Decimal64  rhs);
    Decimal_Type32& operator-=(Decimal128 rhs);

    /// Subtract the specified `rhs` from the value of this object as
    /// described by IEEE-754, store the result in this object, and return a
    /// reference to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN, then do not change this object.
    /// * Otherwise if this object is infinity, then do not change it.
    /// * Otherwise if subtracting `rhs` from this object's value results
    ///   in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal32>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise set this object to the result of subtracting `rhs` from
    ///   the value of this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    ///
    /// Also note that this operation is always performed with 64 bits
    /// precision to prevent loss of precision of the `rhs` operand (prior
    /// to the operation).  The result is then rounded back to 32 bits and
    /// stored to this object.  See IEEE-754 2008, 5.1, first paragraph,
    /// second sentence for specification.
    Decimal_Type32& operator-=(int                rhs);
    Decimal_Type32& operator-=(unsigned int       rhs);
    Decimal_Type32& operator-=(long               rhs);
    Decimal_Type32& operator-=(unsigned long      rhs);
    Decimal_Type32& operator-=(long long          rhs);
    Decimal_Type32& operator-=(unsigned long long rhs);

    /// Multiply the value of the specified `rhs` object by the value of
    /// this as described by IEEE-754, store the result in this object, and
    /// return a reference to this object.
    ///
    /// * If either of this object or `rhs` is signaling NaN, then store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise, if one of this object and `rhs` is zero (positive or
    ///   negative) and the other is infinity (positive or negative), store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise, if either this object or `rhs` is positive or negative
    ///   infinity, set this object to infinity.  The sign of this object
    ///   will be positive if this object and `rhs` had the same sign, and
    ///   negative otherwise.
    /// * Otherwise, if either this object or `rhs` is zero, set this
    ///   object to zero.  The sign of this object will be positive if this
    ///   object and `rhs` had the same sign, and negative otherwise.
    /// * Otherwise if the product of this object and `rhs` has an absolute
    ///   value that is larger than `std::numeric_limits<Decimal32>::max()`
    ///   then store the value of the macro `ERANGE` into `errno` and set
    ///   this object to infinity with the same sign of that result.
    /// * Otherwise if the product of this object and `rhs` has an absolute
    ///   value that is smaller than
    ///   `std::numeric_limits<Decimal32>::min()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to zero value
    ///   with the same sign as that result.
    /// * Otherwise set this object to the product of the value of `rhs`
    ///   and the value of this object.
    ///
    /// Note that when `rhs` is a `Decimal64`, this operation is always
    /// performed with 64 bits precision to prevent loss of precision of the
    /// `rhs` operand (prior to the operation).  The result is then rounded
    /// back to 32 bits and stored to this object.  See IEEE-754 2008, 5.1,
    /// first paragraph, second sentence for specification.
    ///
    /// Also note that when `rhs` is a `Decimal128`, this operation is
    /// always performed with 128 bits precision to prevent loss of
    /// precision of the `rhs` operand (prior to the operation).  The
    /// result is then rounded back to 32 bits and stored to this object.
    /// See IEEE-754 2008, 5.1, first paragraph, second sentence for
    /// specification.
    Decimal_Type32& operator*=(Decimal32  rhs);
    Decimal_Type32& operator*=(Decimal64  rhs);
    Decimal_Type32& operator*=(Decimal128 rhs);

    /// Multiply the specified `rhs` by the value of this object as
    /// described by IEEE-754, store the result in this object, and return a
    /// reference to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN, then do not change this object.
    /// * Otherwise if this object is infinity (positive or negative), and
    ///   `rhs` is zero, then store the value of the macro `EDOM` into
    ///   `errno` and set this object to a NaN.
    /// * Otherwise if this object is infinity (positive or negative), then
    ///   do not change it.
    /// * Otherwise if `rhs` is zero, then set this object to zero with the
    ///   same sign as its value had prior to this operation.
    /// * Otherwise if the product of `rhs` and the value of this object
    ///   results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal32>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise if the product of `rhs` and the value of this object
    ///   results in an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal32>::min()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to zero with
    ///   the same sign as that result.
    /// * Otherwise set this object to the product of the value of this
    ///   object and the value `rhs`.
    ///
    /// Note that this operation is always performed with 64 bits precision
    /// to prevent loss of precision of the `rhs` operand (prior to the
    /// operation).  The result is then rounded back to 32 bits and stored
    /// to this object.  See IEEE-754 2008, 5.1, first paragraph,
    Decimal_Type32& operator*=(int                rhs);
    Decimal_Type32& operator*=(unsigned int       rhs);
    Decimal_Type32& operator*=(long               rhs);
    Decimal_Type32& operator*=(unsigned long      rhs);
    Decimal_Type32& operator*=(long long          rhs);
    Decimal_Type32& operator*=(unsigned long long rhs);

    /// Divide the value of this object by the value of the specified `rhs`
    /// as described by IEEE-754, store the result in this object, and
    /// return a reference to this object.
    ///
    /// * If either of this object or `rhs` is signaling NaN, then store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise if this object and `rhs` are both infinity (positive or
    ///   negative) or both zero (positive or negative) then store the
    ///   value of the macro `EDOM` into `errno` and return a NaN.
    /// * Otherwise if `rhs` has a positive zero value, then store the
    ///   value of the macro `ERANGE` into `errno` and set this object to
    ///   infinity with the same sign as its original value.
    /// * Otherwise if `rhs` has a negative zero value, then store the
    ///   value of the macro `ERANGE` into `errno` and set this object to
    ///   infinity with the opposite sign as its original value.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal32>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and return infinity with the same
    ///   sign as that result.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal32>::min()` then store the value of
    ///   the macro `ERANGE` into `errno`and return zero with the same sign
    ///   as that result.
    /// * Otherwise set this object to the result of dividing the value of
    ///   this object by the value of `rhs`.
    ///
    /// Note that when `rhs` is a `Decimal64`, this operation is always
    /// performed with 64 bits precision to prevent loss of precision of the
    /// `rhs` operand (prior to the operation).  The result is then rounded
    /// back to 32 bits and stored to this object.  See IEEE-754 2008, 5.1,
    /// first paragraph, second sentence for specification.
    ///
    /// Also note that when `rhs` is a `Decimal128`, this operation is
    /// always performed with 128 bits precision to prevent loss of
    /// precision of the `rhs` operand (prior to the operation).  The
    /// result is then rounded back to 32 bits and stored to this object.
    /// See IEEE-754 2008, 5.1, first paragraph, second sentence for
    /// specification.
    Decimal_Type32& operator/=(Decimal32  rhs);
    Decimal_Type32& operator/=(Decimal64  rhs);
    Decimal_Type32& operator/=(Decimal128 rhs);

    /// Divide the value of this object by the specified `rhs` as described
    /// by IEEE-754, store the result in this object, and return a reference
    /// to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN then set this object to a NaN.
    /// * Otherwise if this object is infinity (positive or negative) and
    ///   `rhs` is positive value then set this object to infinity value
    ///   with the same sign as its original value.
    /// * Otherwise if this object is infinity (positive or negative) and
    ///   `rhs` is negative value then set this object to infinity value
    ///   with the opposite sign as its original value.
    /// * Otherwise if `rhs` is zero, store the value of the macro `ERANGE`
    ///   into `errno` and set this object to infinity with the same sign
    ///   it had prior to this operation.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal32>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and return infinity with the same
    ///   sign as that result.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal32>::min()` then store the value of
    ///   the macro `ERANGE` into `errno`and return zero with the same sign
    ///   as that result.
    /// * Otherwise set this object to the result of dividing the value of
    ///   this object by the value of `rhs`.
    ///
    /// Note that this operation is always performed with 64 bits precision
    /// to prevent loss of precision of the `rhs` operand (prior to the
    /// operation).  The result is then rounded back to 32 bits and stored
    /// to this object.  See IEEE-754 2008, 5.1, first paragraph,
    Decimal_Type32& operator/=(int                rhs);
    Decimal_Type32& operator/=(unsigned int       rhs);
    Decimal_Type32& operator/=(long               rhs);
    Decimal_Type32& operator/=(unsigned long      rhs);
    Decimal_Type32& operator/=(long long          rhs);
    Decimal_Type32& operator/=(unsigned long long rhs);

    /// Return a pointer providing modifiable access to the underlying
    /// implementation.
    DecimalImpUtil::ValueType32 *data();

                                  // Aspects

    /// Assign to this object the value read from the specified input
    /// `stream` using the specified `version` format, and return a
    /// reference to `stream`.  If `stream` is initially invalid, this
    /// operation has no effect.  If `version` is not supported, this object
    /// is unaltered and `stream` is invalidated, but otherwise unmodified.
    /// If `version` is supported but `stream` becomes invalid during this
    /// operation, this object has an undefined, but valid, state.  Note
    /// that no version is read from `stream`.  See the `bslx` package-level
    /// documentation for more information on BDEX streaming of
    /// value-semantic types and containers.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    // ACCESSORS

    /// Return a pointer providing non-modifiable access to the underlying
    /// implementation.
    const DecimalImpUtil::ValueType32 *data() const;

    /// Return the value of the underlying implementation.
    DecimalImpUtil::ValueType32 value() const;

                                  // Aspects

    /// Write the value of this object, using the specified `version`
    /// format, to the specified output `stream`, and return a reference to
    /// `stream`.  If `stream` is initially invalid, this operation has no
    /// effect.  If `version` is not supported, `stream` is invalidated, but
    /// otherwise unmodified.  Note that `version` is not written to
    /// `stream`.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
};

// FREE OPERATORS

/// Return a copy of the specified `value` if the value is not negative
/// zero, and return positive zero otherwise.
Decimal32 operator+(Decimal32 value);

/// Return the result of applying the unary - operator to the specified
/// `value` as described by IEEE-754, essentially reversing the sign bit.
/// Note that floating-point numbers have signed zero, so this operation is
/// not the same as `0 - value`.
Decimal32 operator-(Decimal32 value);

/// Apply the prefix ++ operator to the specified `value` and return its
/// original value.  Note that this is a floating-point value so this
/// operation may not change the value of this object at all (if the value
/// is large) or it may just set it to 1.0 (if the original value is small).
Decimal32 operator++(Decimal32& value, int);

/// Apply the prefix -- operator to the specified `value` and return its
/// original value.  Note that this is a floating-point value so this
/// operation may not change the value of this object at all (if the value
/// is large) or it may just set it to -1.0 (if the original value is
/// small).
Decimal32 operator--(Decimal32& value, int);

/// Add the value of the specified `rhs` to the value of the specified `lhs`
/// as described by IEEE-754 and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if `lhs` and `rhs` are infinities of differing signs, store
///   the value of the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if `lhs` and `rhs` are infinities of the same sign then
///   return infinity of that sign.
/// * Otherwise if the sum of `lhs` and `rhs` has an absolute value that is
///   larger than `std::numeric_limits<Decimal32>::max()` then store the
///   value of the macro `ERANGE` into `errno` and set this object to
///   infinity with the same sign as that result.
/// * Otherwise return the sum of the number represented by `lhs` and the
///   number represented by `rhs`.
Decimal32 operator+(Decimal32 lhs, Decimal32 rhs);

/// Add the specified `rhs` to the value of the specified `lhs` as described
/// by IEEE-754 and return the result.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` object is NaN, then return a NaN.
/// * Otherwise if `lhs` is infinity, then return infinity.
/// * Otherwise if the sum of `lhs` and `rhs` has an absolute value that is
///   larger than `std::numeric_limits<Decimal32>::max()` then store the
///   value of the macro `ERANGE` into `errno` and return infinity with the
///   same sign as that result.
/// * Otherwise return the sum of `rhs` and the number represented by
///   `lhs`.
Decimal32 operator+(Decimal32 lhs, int                rhs);
Decimal32 operator+(Decimal32 lhs, unsigned int       rhs);
Decimal32 operator+(Decimal32 lhs, long               rhs);
Decimal32 operator+(Decimal32 lhs, unsigned long      rhs);
Decimal32 operator+(Decimal32 lhs, long long          rhs);
Decimal32 operator+(Decimal32 lhs, unsigned long long rhs);

/// Add the specified `lhs` to the value of the specified `rhs` as described
/// by IEEE-754 and return the result.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` object is NaN, then return a NaN.
/// * Otherwise if `rhs` is infinity, then return infinity.
/// * Otherwise if the sum of `lhs` and `rhs` has an absolute value that is
///   larger than `std::numeric_limits<Decimal32>::max()` then store the
///   value of the macro `ERANGE` into `errno` and return infinity with the
///   same sign as that result.
/// * Otherwise return the sum of `lhs` and the number represented by
///   `rhs`.
Decimal32 operator+(int                lhs, Decimal32 rhs);
Decimal32 operator+(unsigned int       lhs, Decimal32 rhs);
Decimal32 operator+(long               lhs, Decimal32 rhs);
Decimal32 operator+(unsigned long      lhs, Decimal32 rhs);
Decimal32 operator+(long long          lhs, Decimal32 rhs);
Decimal32 operator+(unsigned long long lhs, Decimal32 rhs);

/// Subtract the value of the specified `rhs` from the value of the
/// specified `lhs` as described by IEEE-754 and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if `lhs` and the `rhs` have infinity values of the same
///   sign, store the value of the macro `EDOM` into `errno` and return a
///   NaN.
/// * Otherwise if `lhs` and the `rhs` have infinity values of differing
///   signs, then return `lhs`.
/// * Otherwise if the subtracting of `lhs` and `rhs` has an absolute value
///   that is larger than `std::numeric_limits<Decimal32>::max()` then
///   store the value of the macro `ERANGE` into `errno` and return
///   infinity with the same sign as that result.
/// * Otherwise return the result of subtracting the value of `rhs` from
///   the value of `lhs`.
Decimal32 operator-(Decimal32 lhs, Decimal32 rhs);

/// Subtract the specified `rhs` from the value of the specified `lhs` as
/// described by IEEE-754 and return a reference to this object.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` is NaN, then return a NaN.
/// * Otherwise if `lhs` is infinity, then return infinity.
/// * Otherwise if subtracting `rhs` from `lhs` object's value results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal32>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise return the result of subtracting `rhs` from the value of
///   `lhs`.
Decimal32 operator-(Decimal32 lhs, int                rhs);
Decimal32 operator-(Decimal32 lhs, unsigned int       rhs);
Decimal32 operator-(Decimal32 lhs, long               rhs);
Decimal32 operator-(Decimal32 lhs, unsigned long      rhs);
Decimal32 operator-(Decimal32 lhs, long long          rhs);
Decimal32 operator-(Decimal32 lhs, unsigned long long rhs);

/// Subtract the specified `rhs` from the value of the specified `lhs` as
/// described by IEEE-754 and return a reference to this object.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` is NaN, then return a NaN.
/// * Otherwise if `rhs` is infinity, then return infinity.
/// * Otherwise if subtracting `rhs` from `lhs` object's value results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal32>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise return the result of subtracting the value of `rhs` from
///   the number `lhs`.
Decimal32 operator-(int                lhs, Decimal32 rhs);
Decimal32 operator-(unsigned int       lhs, Decimal32 rhs);
Decimal32 operator-(long               lhs, Decimal32 rhs);
Decimal32 operator-(unsigned long      lhs, Decimal32 rhs);
Decimal32 operator-(long long          lhs, Decimal32 rhs);
Decimal32 operator-(unsigned long long lhs, Decimal32 rhs);

/// Multiply the value of the specified `lhs` object by the value of the
/// specified `rhs` as described by IEEE-754 and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if one of the operands is infinity (positive or negative)
///   and the other is zero (positive or negative), then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if both `lhs` and `rhs` are infinity (positive or
///   negative), return infinity.  The sign of the returned value will be
///   positive if `lhs` and `rhs` have the same sign, and negative
///   otherwise.
/// * Otherwise, if either `lhs` or `rhs` is zero, return zero.  The sign
///   of the returned value will be positive if `lhs` and `rhs` have the
///   same sign, and negative otherwise.
/// * Otherwise if the product of `lhs` and `rhs` has an absolute value
///   that is larger than `std::numeric_limits<Decimal32>::max()` then
///   store the value of the macro `ERANGE` into `errno` and return
///   infinity with the same sign as that result.
/// * Otherwise if the product of `lhs` and `rhs` has an absolute value
///   that is smaller than `std::numeric_limits<Decimal32>::min()` then
///   store the value of the macro `ERANGE` into `errno` and return zero
///   with the same sign as that result.
/// * Otherwise return the product of the value of `rhs` and the number
///   represented by `rhs`.
Decimal32 operator*(Decimal32 lhs, Decimal32 rhs);

/// Multiply the specified `rhs` by the value of the specified `lhs` as
/// described by IEEE-754, and return the result.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` is NaN, then return a NaN.
/// * Otherwise if `lhs` is infinity (positive or negative), and `rhs` is
///   zero, then store the value of the macro `EDOM` into `errno` and
///   return a NaN.
/// * Otherwise if `lhs` is infinity (positive or negative), then return
///   `lhs`.
/// * Otherwise if `rhs` is zero, then return zero with the sign of `lhs`.
/// * Otherwise if the product of `rhs` and the value of `lhs` results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal32>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if the product of `rhs` and the value of `lhs` results in
///   an absolute value that is smaller than
///   `std::numeric_limits<Decimal32>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the product of the value of `lhs` and value `rhs`.
Decimal32 operator*(Decimal32 lhs, int                rhs);
Decimal32 operator*(Decimal32 lhs, unsigned int       rhs);
Decimal32 operator*(Decimal32 lhs, long               rhs);
Decimal32 operator*(Decimal32 lhs, unsigned long      rhs);
Decimal32 operator*(Decimal32 lhs, long long          rhs);
Decimal32 operator*(Decimal32 lhs, unsigned long long rhs);

/// Multiply the specified `lhs` by the value of the specified `rhs` as
/// described by IEEE-754, and return the result.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` is NaN, then return a NaN.
/// * Otherwise if `rhs` is infinity (positive or negative), and `lhs` is
///   zero, then store the value of the macro `EDOM` into `errno` and
///   return a NaN.
/// * Otherwise if `rhs` is infinity (positive or negative), then return
///   `rhs`.
/// * Otherwise if `lhs` is zero, then return zero with the sign of `rhs`.
/// * Otherwise if the product of `lhs` and the value of `rhs` results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal32>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if the product of `lhs` and the value of `rhs` results in
///   an absolute value that is smaller than
///   `std::numeric_limits<Decimal32>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the product of the value of `lhs` and value `rhs`.
Decimal32 operator*(int                lhs, Decimal32 rhs);
Decimal32 operator*(unsigned int       lhs, Decimal32 rhs);
Decimal32 operator*(long               lhs, Decimal32 rhs);
Decimal32 operator*(unsigned long      lhs, Decimal32 rhs);
Decimal32 operator*(long long          lhs, Decimal32 rhs);
Decimal32 operator*(unsigned long long lhs, Decimal32 rhs);

/// Divide the value of the specified `lhs` by the value of the specified
/// `rhs` as described by IEEE-754, and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if `lhs` and `rhs` are both infinity (positive or negative)
///   or both zero (positive or negative) then store the value of the macro
///   `EDOM` into `errno` and return a NaN.
/// * Otherwise if `lhs` has a normal value and `rhs` has a positive zero
///   value, store the value of the macro `ERANGE` into `errno` and return
///   infinity with the sign of `lhs`.
/// * Otherwise if `lhs` has a normal value and `rhs` has a negative zero
///   value, store the value of the macro `ERANGE` into `errno` and return
///   infinity with the opposite sign as `lhs`.
/// * Otherwise if `lhs` has infinity value and `rhs` has a positive zero
///   value, return infinity with the sign of `lhs`.
/// * Otherwise if `lhs` has infinity value and `rhs` has a negative zero
///   value, return infinity with the opposite sign as `lhs`.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is larger than
///   `std::numeric_limits<Decimal32>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is smaller than
///   `std::numeric_limits<Decimal32>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the result of dividing the value of `lhs` by the
///   value of `rhs`.
Decimal32 operator/(Decimal32 lhs, Decimal32 rhs);

/// Divide the value of the specified `lhs` by the specified `rhs` as
/// described by IEEE-754, and return the result.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` is NaN then return a NaN.
/// * Otherwise if `lhs` is infinity (positive or negative) and `rhs` is
///   positive value then return infinity value with the same sign as its
///   original value.
/// * Otherwise if `lhs` is infinity (positive or negative) and `rhs` is
///   negative value then return infinity value with the opposite sign as
///   its original value.
/// * Otherwise if `rhs` is zero, store the value of the macro `ERANGE`
///   into `errno` and return infinity with the same sign it had prior to
///   this operation.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is larger than
///   `std::numeric_limits<Decimal32>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is smaller than
///   `std::numeric_limits<Decimal32>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the result of dividing the value of `lhs` by the
///   value `rhs`.
Decimal32 operator/(Decimal32 lhs, int                rhs);
Decimal32 operator/(Decimal32 lhs, unsigned int       rhs);
Decimal32 operator/(Decimal32 lhs, long               rhs);
Decimal32 operator/(Decimal32 lhs, unsigned long      rhs);
Decimal32 operator/(Decimal32 lhs, long long          rhs);
Decimal32 operator/(Decimal32 lhs, unsigned long long rhs);

/// Divide the specified `lhs` by the value of the specified `rhs` as
/// described by IEEE-754, and return the result.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` is NaN then return a NaN.
/// * Otherwise if `rhs` is infinity (positive or negative), and `lhs` is
///   zero, store the value of the macro `ERANGE` into `errno` and return a
///   NaN.
/// * Otherwise if `rhs` is zero (positive or negative), store the value of
///   the macro `ERANGE` into `errno` and return infinity with the sign of
///   `lhs`.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is larger than
///   `std::numeric_limits<Decimal32>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is smaller than
///   `std::numeric_limits<Decimal32>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the result of dividing the value of `lhs` by the
///   value `rhs`.  Note that this is a floating-point operation, not
///   integer.
Decimal32 operator/(int                lhs, Decimal32 rhs);
Decimal32 operator/(unsigned int       lhs, Decimal32 rhs);
Decimal32 operator/(long               lhs, Decimal32 rhs);
Decimal32 operator/(unsigned long      lhs, Decimal32 rhs);
Decimal32 operator/(long long          lhs, Decimal32 rhs);
Decimal32 operator/(unsigned long long lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `Decimal32` objects have the same value if the
/// `compareQuietEqual` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representations equal.  In
/// other words, two `Decimal32` objects have the same value if:
///
/// * both have a zero value (positive or negative), or
/// * both have the same infinity value (both positive or negative), or
/// * both have the value of a real number that are equal, even if they are
///   represented differently (cohorts have the same value)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
///
/// Note that a NaN is never equal to anything, including itself:
/// ```
/// Decimal32 aNaN = std::numeric_limits<Decimal32>::quiet_NaN();
/// assert(!(aNan == aNan));
/// ```
bool operator==(Decimal32 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` and `rhs` do not have the same
/// value, and `false` otherwise.  Two `Decimal32` objects do not have the
/// same value if the `compareQuietEqual` operation (IEEE-754 defined,
/// non-total ordering comparison) considers the underlying IEEE
/// representations not equal.  In other words, two `Decimal32` objects do
/// not have the same value if:
///
/// * both are NaN, or
/// * one is zero (positive or negative) and the other is not, or
/// * one is positive infinity and the other is not, or
/// * one is negative infinity and the other is not, or
/// * both have the value of a real number that are not equal, regardless
///   of their representation (cohorts are equal)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
///
/// Note that a NaN is never equal to anything, including itself:
/// ```
/// Decimal32 aNaN = std::numeric_limits<Decimal32>::quiet_NaN();
/// assert(aNan != aNan);
/// ```
bool operator!=(Decimal32 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` has a value less than the specified
/// `rhs` and `false` otherwise.  The value of a `Decimal32` object `lhs` is
/// less than that of an object `rhs` if the `compareQuietLess` operation
/// (IEEE-754 defined, non-total ordering comparison) considers the
/// underlying IEEE representation of `lhs` to be less than of that of
/// `rhs`.  In other words, `lhs` is less than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` is zero (positive or negative) and `rhs` positive, or
/// * `rhs` is zero (positive or negative) and `lhs` negative, or
/// * `lhs` is not positive infinity, or
/// * `lhs` is negative infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is less than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<(Decimal32 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` has a value less than or equal the
/// value of the specified `rhs` and `false` otherwise.  The value of a
/// `Decimal32` object `lhs` is less than or equal to the value of an object
/// `rhs` if the `compareQuietLessEqual` operation (IEEE-754 defined,
/// non-total ordering comparison) considers the underlying IEEE
/// representation of `lhs` to be less or equal to that of `rhs`.  In other
/// words, `lhs` is less or equal than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are positive infinity, or
/// * `lhs` is negative infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is less or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<=(Decimal32 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` has a greater value than the
/// specified `rhs` and `false` otherwise.  The value of a `Decimal32`
/// object `lhs` is greater than that of an object `rhs` if the
/// `compareQuietGreater` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representation of `lhs` to be
/// greater than of that of `rhs`.  In other words, `lhs` is greater than
/// `rhs`if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are not both zero (positive or negative), or
/// * `lhs` is not negative infinity, or
/// * `lhs` is positive infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>(Decimal32 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` has a value greater than or equal
/// to the value of the specified `rhs` and `false` otherwise.  The value of
/// a `Decimal32` object `lhs` is greater or equal to a `Decimal32` object
/// `rhs` if the `compareQuietGreaterEqual` operation (IEEE-754 defined,
/// non-total ordering comparison ) considers the underlying IEEE
/// representation of `lhs` to be greater or equal to that of `rhs`.  In
/// other words, `lhs` is greater than or equal to `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are negative infinity, or
/// * `lhs` is positive infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>=(Decimal32 lhs, Decimal32 rhs);

/// Read, into the specified `object`, from the specified input `stream` an
/// IEEE 32 bit decimal floating-point value as described in the IEEE-754
/// 2008 standard (5.12 Details of conversions between floating point
/// numbers and external character sequences) and return a reference
/// providing modifiable access to `stream`.  If `stream` contains a NaN
/// value, it is unspecified if `object` will receive a quiet or signaling
/// `Nan`.  If `stream` is not valid on entry `stream.good() == false`, this
/// operation has no effect other than setting `stream.fail()` to `true`.
/// If eof (end-of-file) is found before any non-whitespace characters
/// `stream.fail()` is set to `true` and `object` remains unchanged.  If eof
/// is detected after some characters have been read (and successfully
/// interpreted as part of the textual representation of a floating-point
/// value as specified by IEEE-754) then `stream.eof()` is set to true.  If
/// the first non-whitespace character sequence is not a valid textual
/// representation of a floating-point value (e.g., 12e or e12 or 1*2) the
/// `stream.fail()` is set to true and `object` will remain unchanged.  If a
/// real number value is represented by the character sequence but it is a
/// large positive or negative value that cannot be stored into `object`
/// then store the value of the macro `ERANGE` into `errno` and positive or
/// negative infinity is stored into `object`, respectively.  If a real
/// number value is represented by the character sequence but it is a small
/// positive or negative value that cannot be stored into `object` then
/// store the value of the macro `ERANGE` into `errno` and positive or
/// negative zero is stored into `object`, respectively.  If a real number
/// value is represented by the character sequence but it cannot be stored
/// exactly into `object`, the value is rounded according to the current
/// rounding direction (of the environment) and then stored into `object`.
///
/// NOTE: This method does not yet fully support iostream flags or the
/// decimal floating point exception context.
template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
operator>>(bsl::basic_istream<CHARTYPE, TRAITS>& stream, Decimal32& object);

/// Write the value of the specified `object` to the specified output
/// `stream` in a single line format as described in the IEEE-754 2008
/// standard (5.12 Details of conversions between floating point numbers and
/// external character sequences), and return a reference providing
/// modifiable access to `stream`.  If `stream` is not valid on entry, this
/// operation has no effect.
///
/// NOTE: This method does not yet fully support iostream flags or the
/// decimal floating point exception context.
template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
operator<<(bsl::basic_ostream<CHARTYPE, TRAITS>& stream, Decimal32 object);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
inline namespace literals {
inline namespace DecimalLiterals {
/// Produce an object of the indicated return type by parsing the specified
/// `str` having the specified `len` excluding the terminating null
/// character that represents a floating-point number written in both fixed
/// and scientific notations.  These user-defined literal suffixes can be
/// applied to both numeric and string literals, (i.e., 1.2_d32, "1.2"_d32
/// or "inf"_d32). The resulting decimal object is initialized as follows:
///
/// * If `str` does not represent a floating-point value, then return a
///   decimal object of the indicated return type initialized to a NaN.
/// * Otherwise if `str` represents infinity (positive or negative), then
///   return a decimal object of the indicated return type initialized to
///   infinity value with the same sign.
/// * Otherwise if `str` represents zero (positive or negative), then
///   return a decimal object of the indicated return type initialized to
///   zero with the same sign.
/// * Otherwise if `str` represents a value that has an absolute value that
///   is larger than the maximum value supported by the indicated return
///   type, then store the value of the macro `ERANGE` into `errno` and
///   return a decimal object of the return type initialized to infinity
///   with the same sign.
/// * Otherwise if `str` represents a value that has an absolute value that
///   is smaller than min value of the indicated return type, then store
///   the value of the macro `ERANGE` into `errno` and return a decimal
///   object of the return type initialized to zero with the same sign.
/// * Otherwise if `str` has a value that is not exactly representable
///   using the maximum digit number supported by the indicated return
///   type, then return a decimal object of the return type initialized to
///   the value represented by `str` rounded according to the rounding
///   direction.
/// * Otherwise return a decimal object of the indicated return type
///   initialized to the decimal value representation of `str`.
///
/// Note that the parsing follows the rules as specified for the `strtod32`
/// function in section 9.6 of the ISO/EIC TR 247128 C Decimal
/// Floating-Point Technical Report.
///
/// Also note that the numeric literal version omits the optional leading
/// sign in `str`.  For example, if the string is -1.2_d32 then the string
/// "1.2" is passed to the one-argument form, not "-1.2", because leading
/// signs are operators, not parts of literals.  On the other hand, the
/// string literal version does not omit leading sign and if the string is
/// "-1.2"_d32 then the string "-1.2" is passed to the two-argument form.
///
/// Also note that the quantum of the resultant value is affected by the
/// number of decimal places in `str` string in both numeric and string
/// literal formats starting with the most significand digit and cannot
/// exceed the maximum number of digits necessary to differentiate all
/// values of the indicated return type, for example:
///
/// `0.015_d32;     "0.015"_d32     =>      15e-3`
/// `1.5_d32;       "1.5"_d32       =>      15e-1`
/// `1.500_d32;     "1.500"d_32     =>    1500e-3`
/// `1.2345678_d32; "1.2345678_d32" => 1234568e-6`
bdldfp::Decimal32  operator""_d32 (const char *str);
bdldfp::Decimal32  operator""_d32 (const char *str, bsl::size_t len);

}  // close DecimalLiterals namespace
}  // close literals namespace
#endif

// FREE FUNCTIONS

/// Pass the specified `object` to the specified `hashAlg`.  This function
/// integrates with the `bslh` modular hashing system and effectively
/// provides a `bsl::hash` specialization for `Decimal32`.  Note that two
/// objects which have the same value but different representations will
/// hash to the same value.
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Decimal32& object);

                            // ====================
                            // class Decimal_Type64
                            // ====================

/// This value-semantic class implements the IEEE-754 64 bit decimal
/// floating-point format arithmetic type.  This class is a standard layout
/// type that is `const` thread-safe and exception-neutral.
class Decimal_Type64 {

  private:
    // DATA
    DecimalImpUtil::ValueType64 d_value; // The underlying IEEE representation

  public:
    // CLASS METHODS

                                  // Aspects

    /// Return the maximum valid BDEX format version, as indicated by the
    /// specified `versionSelector`, to be passed to the `bdexStreamOut`
    /// method.  Note that it is highly recommended that `versionSelector`
    /// be formatted as "YYYYMMDD", a date representation.  Also note that
    /// `versionSelector` should be a *compile*-time-chosen value that
    /// selects a format version supported by both externalizer and
    /// unexternalizer.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    static int maxSupportedBdexVersion();
    static int maxSupportedBdexVersion(int versionSelector);

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Decimal_Type64,
                                   bsl::is_trivially_copyable);

    // CREATORS

    /// Create a `Decimal64_Type` object having the value positive zero and
    /// the smallest exponent value.
    Decimal_Type64();

    /// Create a `Decimal64_Type` object having the specified `value`.
    Decimal_Type64(DecimalImpUtil::ValueType64 value);              // IMPLICIT

    /// Create a `Decimal64_Type` object having the value of the specified
    /// `other` following the conversion rules as defined by IEEE-754:
    ///
    /// * If `other` is NaN, initialize this object to a NaN.
    /// * Otherwise if `other` is infinity (positive or negative), then
    ///   initialize this object to infinity with the same sign.
    /// * Otherwise if `other` is zero, then initialize this object to zero
    ///   with the same sign.
    /// * Otherwise initialize this object to the value of the `other`.
    Decimal_Type64(Decimal32 other);                                // IMPLICIT

    /// Create a `Decimal64_Type` object having the value closest to the
    /// value of the specified `other` following the conversion rules as
    /// defined by IEEE-754:
    ///
    /// * If `other` is NaN, initialize this object to a NaN.
    /// * Otherwise if `other` is infinity (positive or negative), then
    ///   initialize this object to infinity with the same sign.
    /// * Otherwise if `other` is zero, then initialize this object to
    ///   zero with the same sign.
    /// * Otherwise if `other` has an absolute value that is larger than
    ///   `std::numeric_limits<Decimal64>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and initialize this object to
    ///   infinity with the same sign as `other`.
    /// * Otherwise if `other` has an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal64>::min()` then store the value of
    ///   the macro `ERANGE` into `errno` and initialize this object to
    ///   zero with the same sign as `other`.
    /// * Otherwise if `other` has a value that has more significant
    ///   digits than `std::numeric_limits<Decimal64>::max_digit` then
    ///   initialize this object to the value of `other` rounded according
    ///   to the rounding direction.
    /// * Otherwise initialize this object to the value as the `other`.
    explicit Decimal_Type64(Decimal128 other);

    /// Create a `Decimal64_Type` object having the value closest to the
    /// value of the specified `other` value.  *Warning:* clients requiring
    /// a conversion for an exact decimal value should use
    /// `bdldfp_decimalconvertutil` (see *WARNING*: Conversions from
    /// `float` and `double`}.  This conversion follows the conversion
    /// rules as defined by IEEE-754:
    ///
    /// * If `other` is NaN, initialize this object to a NaN.
    /// * Otherwise if `other` is infinity (positive or negative), then
    ///   initialize this object to infinity value with the same sign.
    /// * Otherwise if `other` has a zero value, then initialize this
    ///   object to zero with the same sign.
    /// * Otherwise if `other` has a value that needs more than
    ///   `std::numeric_limits<Decimal64>::max_digit` significant decimal
    ///   digits to represent then initialize this object to the value of
    ///   `other` rounded according to the rounding direction.
    /// * Otherwise initialize this object to the value of the `other`.
    explicit Decimal_Type64(float       other);
    explicit Decimal_Type64(double      other);

    /// Create a `Decimal64_Type` object having the value closest to the
    /// value of the specified `other` following the conversion rules as
    /// defined by IEEE-754:
    ///
    /// * Otherwise if `other` has a value that is not exactly
    ///   representable using `std::numeric_limits<Decimal64>::max_digit`
    ///   decimal digits then initialize this object to the value of
    ///   `other` rounded according to the rounding direction.
    /// * Otherwise initialize this object to the value of `other` with
    ///   exponent 0.
    explicit Decimal_Type64(int                other);
    explicit Decimal_Type64(unsigned int       other);
    explicit Decimal_Type64(long               other);
    explicit Decimal_Type64(unsigned long      other);
    explicit Decimal_Type64(long long          other);
    explicit Decimal_Type64(unsigned long long other);

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

    /// Add 1.0 to the value of this object and return a reference to it.
    /// Note that this is a floating-point value so this operation may not
    /// change the value of this object at all (if the value is large) or it
    /// may just set it to 1.0 (if the original value is small).
    Decimal_Type64& operator++();

    /// Add -1.0 to the value of this object and return a reference to it.
    /// Note that this is a floating-point value so this operation may not
    /// change the value of this object at all (if the value is large) or it
    /// may just set it to -1.0 (if the original value is small).
    Decimal_Type64& operator--();

    /// Add the value of the specified `rhs` object to the value of this as
    /// described by IEEE-754, store the result in this object, and return a
    /// reference to this object.
    ///
    /// * If either of this object or `rhs` is signaling NaN, then store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise if this object and `rhs` have infinite values of
    ///   differing signs, store the value of the macro `EDOM` into `errno`
    ///   and set this object to a NaN.
    /// * Otherwise if this object and `rhs` have infinite values of the
    ///   same sign, then do not change this object.
    /// * Otherwise if `rhs` has a zero value (positive or negative), do
    ///   not change this object.
    /// * Otherwise if the sum of this object and `rhs` has an absolute
    ///   value that is larger than `std::numeric_limits<Decimal64>::max()`
    ///   then store the value of the macro `ERANGE` into `errno` and
    ///   set this object to infinity with the same sign as that result.
    /// * Otherwise set this object to the sum of the number represented by
    ///   `rhs` and the number represented by this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    ///
    /// Note that when `rhs` is a `Decimal128`, this operation is always
    /// performed with 128 bits precision to prevent loss of precision of
    /// the `rhs` operand (prior to the operation).  The result is then
    /// rounded back to 64 bits and stored to this object.  See IEEE-754
    /// 2008, 5.1, first paragraph, second sentence for specification.
    Decimal_Type64& operator+=(Decimal32  rhs);
    Decimal_Type64& operator+=(Decimal64  rhs);
    Decimal_Type64& operator+=(Decimal128 rhs);

    /// Add the specified `rhs` to the value of this object as described by
    /// IEEE-754, store the result in this object, and return a reference to
    /// this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN, then do not change this object.
    /// * Otherwise if this object is infinity, then do not change it.
    /// * Otherwise if the sum of this object and `rhs` has an absolute
    ///   value that is larger than `std::numeric_limits<Decimal64>::max()`
    ///   then store the value of the macro `ERANGE` into `errno` and
    ///   set this object to infinity with the same sign as that result.
    /// * Otherwise set this object to sum of adding `rhs` and the number
    ///   represented by this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    Decimal_Type64& operator+=(int                rhs);
    Decimal_Type64& operator+=(unsigned int       rhs);
    Decimal_Type64& operator+=(long               rhs);
    Decimal_Type64& operator+=(unsigned long      rhs);
    Decimal_Type64& operator+=(long long          rhs);
    Decimal_Type64& operator+=(unsigned long long rhs);

    /// Subtract the value of the specified `rhs` from the value of this
    /// object as described by IEEE-754, store the result in this object,
    /// and return a reference to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise if this object and `rhs` have infinity value of the
    ///   same signs, store the value of the macro `EDOM` into `errno`
    ///   and set this object to a NaN.
    /// * Otherwise if this object and the `rhs` have infinite values of
    ///   differing signs, then do not change this object.
    /// * Otherwise if the `rhs` has a zero value (positive or negative),
    ///   do not change this object.
    /// * Otherwise if subtracting the value of the `rhs` object from this
    ///   results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal64>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise set this object to the result of subtracting the value
    ///   of `rhs` from the value of this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    ///
    /// Note that when `rhs` is a `Decimal128`, this operation is always
    /// performed with 128 bits precision to prevent loss of precision of
    /// the `rhs` operand (prior to the operation).  The result is then
    /// rounded back to 64 bits and stored to this object.  See IEEE-754
    /// 2008, 5.1, first paragraph, second sentence for specification.
    Decimal_Type64& operator-=(Decimal32  rhs);
    Decimal_Type64& operator-=(Decimal64  rhs);
    Decimal_Type64& operator-=(Decimal128 rhs);

    /// Subtract the specified `rhs` from the value of this object as
    /// described by IEEE-754, store the result in this object, and return a
    /// reference to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN, then do not change this object.
    /// * Otherwise if this object is infinity, then do not change it.
    /// * Otherwise if subtracting `rhs` from this object's value results
    ///   in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal64>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise set this object to the result of subtracting `rhs` from
    ///   the value of this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    Decimal_Type64& operator-=(int                rhs);
    Decimal_Type64& operator-=(unsigned int       rhs);
    Decimal_Type64& operator-=(long               rhs);
    Decimal_Type64& operator-=(unsigned long      rhs);
    Decimal_Type64& operator-=(long long          rhs);
    Decimal_Type64& operator-=(unsigned long long rhs);

    /// Multiply the value of the specified `rhs` object by the value of
    /// this as described by IEEE-754, store the result in this object, and
    /// return a reference to this object.
    ///
    /// * If either of this object or `rhs` is signaling NaN, then store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise, if one of this object and `rhs` is zero (positive or
    ///   negative) and the other is infinity (positive or negative), store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise, if either this object or `rhs` is positive or negative
    ///   infinity, set this object to infinity.  The sign of this object
    ///   will be positive if this object and `rhs` had the same sign, and
    ///   negative otherwise.
    /// * Otherwise, if either this object or `rhs` is zero, set this
    ///   object to zero.  The sign of this object will be positive if this
    ///   object and `rhs` had the same sign, and negative otherwise.
    /// * Otherwise if the product of this object and `rhs` has an absolute
    ///   value that is larger than `std::numeric_limits<Decimal64>::max()`
    ///   then store the value of the macro `ERANGE` into `errno` and set
    ///   this object to infinity with the same sign of that result.
    /// * Otherwise if the product of this object and `rhs` has an absolute
    ///   value that is smaller than
    ///   `std::numeric_limits<Decimal64>::min()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to zero value
    ///   with the same sign as that result.
    /// * Otherwise set this object to the product of the value of `rhs`
    ///   and the value of this object.
    ///
    /// Note that when `rhs` is a `Decimal128`, this operation is always
    /// performed with 128 bits precision to prevent loss of precision of
    /// the `rhs` operand (prior to the operation).  The result is then
    /// rounded back to 64 bits and stored to this object.  See IEEE-754
    /// 2008, 5.1, first paragraph, second sentence for specification.
    Decimal_Type64& operator*=(Decimal32  rhs);
    Decimal_Type64& operator*=(Decimal64  rhs);
    Decimal_Type64& operator*=(Decimal128 rhs);

    /// Multiply the specified `rhs` by the value of this object as
    /// described by IEEE-754, store the result in this object, and return a
    /// reference to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN, then do not change this object.
    /// * Otherwise if this object is infinity (positive or negative), and
    ///   `rhs` is zero, then store the value of the macro `EDOM` into
    ///   `errno` and set this object to a NaN.
    /// * Otherwise if this object is infinity (positive or negative), then
    ///   do not change it.
    /// * Otherwise if `rhs` is zero, then set this object to zero with the
    ///   same sign as its value had prior to this operation.
    /// * Otherwise if the product of `rhs` and the value of this object
    ///   results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal64>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise if the product of `rhs` and the value of this object
    ///   results in an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal64>::min()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to zero with
    ///   the same sign as that result.
    Decimal_Type64& operator*=(int                rhs);
    Decimal_Type64& operator*=(unsigned int       rhs);
    Decimal_Type64& operator*=(long               rhs);
    Decimal_Type64& operator*=(unsigned long      rhs);
    Decimal_Type64& operator*=(long long          rhs);
    Decimal_Type64& operator*=(unsigned long long rhs);

    /// Divide the value of this object by the value of the specified `rhs`
    /// as described by IEEE-754, store the result in this object, and
    /// return a reference to this object.
    ///
    /// * If either of this object or `rhs` is signaling NaN, then store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise if this object and `rhs` are both infinity (positive or
    ///   negative) or both zero (positive or negative), then store the
    ///   value of the macro `EDOM` into `errno` and return a NaN.
    /// * Otherwise if `rhs` has a positive zero value, then store the
    ///   value of the macro `ERANGE` into `errno` and set this object to
    ///   infinity with the same sign as its original value.
    /// * Otherwise if `rhs` has a negative zero value, then store the
    ///   value of the macro `ERANGE` into `errno` and set this object to
    ///   infinity with the opposite sign as its original value.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal64>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and return infinity with the same
    ///   sign as that result.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal64>::min()` then store the value of
    ///   the macro `ERANGE` into `errno`and return zero with the same sign
    ///   as that result.
    /// * Otherwise set this object to the result of dividing the value of
    ///   this object by the value of `rhs`.
    ///
    /// Note that when `rhs` is a `Decimal128`, this operation is always
    /// performed with 128 bits precision to prevent loss of precision of
    /// the `rhs` operand (prior to the operation).  The result is then
    /// rounded back to 64 bits and stored to this object.  See IEEE-754
    /// 2008, 5.1, first paragraph, second sentence for specification.
    Decimal_Type64& operator/=(Decimal32  rhs);
    Decimal_Type64& operator/=(Decimal64  rhs);
    Decimal_Type64& operator/=(Decimal128 rhs);

    /// Divide the value of this object by the specified `rhs` as described
    /// by IEEE-754, store the result in this object, and return a reference
    /// to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN then set this object to a NaN.
    /// * Otherwise if this object is infinity (positive or negative) and
    ///   `rhs` is positive value then set this object to infinity value
    ///   with the same sign as its original value.
    /// * Otherwise if this object is infinity (positive or negative) and
    ///   `rhs` is negative value then set this object to infinity value
    ///   with the opposite sign as its original value.
    /// * Otherwise if `rhs` is zero, store the value of the macro `ERANGE`
    ///   into `errno` and set this object to infinity with the same sign
    ///   it had prior to this operation.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal64>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and return infinity with the same
    ///   sign as that result.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal64>::min()` then store the value of
    ///   the macro `ERANGE` into `errno`and return zero with the same sign
    ///   as that result.
    /// * Otherwise set this object to the result of dividing the value of
    ///   this object by the value of `rhs`.
    Decimal_Type64& operator/=(int                rhs);
    Decimal_Type64& operator/=(unsigned int       rhs);
    Decimal_Type64& operator/=(long               rhs);
    Decimal_Type64& operator/=(unsigned long      rhs);
    Decimal_Type64& operator/=(long long          rhs);
    Decimal_Type64& operator/=(unsigned long long rhs);

    /// Return a modifiable pointer to the underlying implementation.
    DecimalImpUtil::ValueType64 *data();

                                  // Aspects

    /// Assign to this object the value read from the specified input
    /// `stream` using the specified `version` format, and return a
    /// reference to `stream`.  If `stream` is initially invalid, this
    /// operation has no effect.  If `version` is not supported, this object
    /// is unaltered and `stream` is invalidated, but otherwise unmodified.
    /// If `version` is supported but `stream` becomes invalid during this
    /// operation, this object has an undefined, but valid, state.  Note
    /// that no version is read from `stream`.  See the `bslx` package-level
    /// documentation for more information on BDEX streaming of
    /// value-semantic types and containers.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    // ACCESSORS

    /// Return a non-modifiable pointer to the underlying implementation.
    const DecimalImpUtil::ValueType64 *data() const;

    /// Return the value of the underlying implementation.
    DecimalImpUtil::ValueType64 value() const;

                                  // Aspects

    /// Write the value of this object, using the specified `version`
    /// format, to the specified output `stream`, and return a reference to
    /// `stream`.  If `stream` is initially invalid, this operation has no
    /// effect.  If `version` is not supported, `stream` is invalidated, but
    /// otherwise unmodified.  Note that `version` is not written to
    /// `stream`.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to `stream`.
    /// Optionally specify an initial indentation `level`, whose absolute
    /// value is incremented recursively for nested objects.  If `level` is
    /// specified, optionally specify `spacesPerLevel`, whose absolute value
    /// indicates the number of spaces per indentation level for this and
    /// all of its nested objects.  If `level` is negative, suppress
    /// indentation of the first line.  If `spacesPerLevel` is negative,
    /// format the entire output on one line, suppressing all but the
    /// initial indentation (as governed by `level`).  If `stream` is not
    /// valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return a copy of the specified `value`.
Decimal64 operator+(Decimal64 value);

/// Return the result of applying the unary - operator to the specified
/// `value` as described by IEEE-754.  Note that floating-point numbers have
/// signed zero, therefore this operation is not the same as `0-value`.
Decimal64 operator-(Decimal64 value);

/// Apply the prefix ++ operator to the specified `value` and return its
/// original value.  Note that this is a floating-point value so this
/// operations may not change the value of this object at all (if the value
/// is large) or it may just set it to 1.0 (if the original value is small).
Decimal64 operator++(Decimal64& value, int);

/// Apply the prefix -- operator to the specified `value` and return its
/// original value.  Note that this is a floating-point value so this
/// operations may not change the value of this object at all (if the value
/// is large) or it may just set it to -1.0 (if the original value is
/// small).
Decimal64 operator--(Decimal64& value, int);

/// Add the value of the specified `rhs` to the value of the specified `lhs`
/// as described by IEEE-754 and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if `lhs` and `rhs` are infinities of differing signs, store
///   the value of the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if `lhs` and `rhs` are infinities of the same sign then
///   return infinity of that sign.
/// * Otherwise if `rhs` is zero (positive or negative), return `lhs`.
/// * Otherwise if the sum of `lhs` and `rhs` has an absolute value that is
///   larger than `std::numeric_limits<Decimal64>::max()` then store the
///   value of the macro `ERANGE` into `errno` and set this object to
///   infinity with the same sign as that result.
/// * Otherwise return the sum of the number represented by `lhs` and the
///   number represented by `rhs`.
Decimal64 operator+(Decimal64 lhs, Decimal64 rhs);
Decimal64 operator+(Decimal32 lhs, Decimal64 rhs);
Decimal64 operator+(Decimal64 lhs, Decimal32 rhs);

/// Add the specified `rhs` to the value of the specified `lhs` as described
/// by IEEE-754 and return the result.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` object is NaN, then return a NaN.
/// * Otherwise if `lhs` is infinity, then return infinity.
/// * Otherwise if the sum of `lhs` and `rhs` has an absolute value that is
///   larger than `std::numeric_limits<Decimal64>::max()` then store the
///   value of the macro `ERANGE` into `errno` and return infinity with the
///   same sign as that result.
/// * Otherwise return the sum of `rhs` and the number represented by
///   `lhs`.
Decimal64 operator+(Decimal64 lhs, int                rhs);
Decimal64 operator+(Decimal64 lhs, unsigned int       rhs);
Decimal64 operator+(Decimal64 lhs, long               rhs);
Decimal64 operator+(Decimal64 lhs, unsigned long      rhs);
Decimal64 operator+(Decimal64 lhs, long long          rhs);
Decimal64 operator+(Decimal64 lhs, unsigned long long rhs);

/// Add the specified `lhs` to the value of the specified `rhs` as described
/// by IEEE-754 and return the result.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` object is NaN, then return a NaN.
/// * Otherwise if `rhs` is infinity, then return infinity.
/// * Otherwise if the sum of `lhs` and `rhs` has an absolute value that is
///   larger than `std::numeric_limits<Decimal64>::max()` then store the
///   value of the macro `ERANGE` into `errno` and return infinity with the
///   same sign as that result.
/// * Otherwise return the sum of `lhs` and the number represented by
///   `rhs`.
Decimal64 operator+(int                lhs, Decimal64 rhs);
Decimal64 operator+(unsigned int       lhs, Decimal64 rhs);
Decimal64 operator+(long               lhs, Decimal64 rhs);
Decimal64 operator+(unsigned long      lhs, Decimal64 rhs);
Decimal64 operator+(long long          lhs, Decimal64 rhs);
Decimal64 operator+(unsigned long long lhs, Decimal64 rhs);

/// Subtract the value of the specified `rhs` from the value of the
/// specified `lhs` as described by IEEE-754 and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if `lhs` and the `rhs` have infinity values of the same
///   sign, store the value of the macro `EDOM` into `errno` and return a
///   NaN.
/// * Otherwise if `lhs` and the `rhs` have infinity values of differing
///   signs, then return `lhs`.
/// * Otherwise if the subtracting of `lhs` and `rhs` has an absolute value
///   that is larger than `std::numeric_limits<Decimal64>::max()` then
///   store the value of the macro `ERANGE` into `errno` and return
///   infinity with the same sign as that result.
/// * Otherwise return the result of subtracting the value of `rhs`from the
///   value of `lhs`.
Decimal64 operator-(Decimal64 lhs, Decimal64 rhs);
Decimal64 operator-(Decimal32 lhs, Decimal64 rhs);
Decimal64 operator-(Decimal64 lhs, Decimal32 rhs);

/// Subtract the specified `rhs` from the value of the specified `lhs` as
/// described by IEEE-754 and return a reference to this object.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` is NaN, then return a NaN.
/// * Otherwise if `lhs` is infinity, then return infinity.
/// * Otherwise if subtracting `rhs` from `lhs` object's value results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal32>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise return the result of subtracting `rhs` from the value of
///   `lhs`.
Decimal64 operator-(Decimal64 lhs, int                rhs);
Decimal64 operator-(Decimal64 lhs, unsigned int       rhs);
Decimal64 operator-(Decimal64 lhs, long               rhs);
Decimal64 operator-(Decimal64 lhs, unsigned long      rhs);
Decimal64 operator-(Decimal64 lhs, long long          rhs);
Decimal64 operator-(Decimal64 lhs, unsigned long long rhs);

/// Subtract the specified `rhs` from the value of the specified `lhs` as
/// described by IEEE-754 and return a reference to this object.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` is NaN, then return a NaN.
/// * Otherwise if `rhs` is infinity, then return infinity.
/// * Otherwise if subtracting `rhs` from `lhs` object's value results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal64>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise return the result of subtracting the value of `rhs` from
///   the number `lhs`.
Decimal64 operator-(int                lhs, Decimal64 rhs);
Decimal64 operator-(unsigned int       lhs, Decimal64 rhs);
Decimal64 operator-(long               lhs, Decimal64 rhs);
Decimal64 operator-(unsigned long      lhs, Decimal64 rhs);
Decimal64 operator-(long long          lhs, Decimal64 rhs);
Decimal64 operator-(unsigned long long lhs, Decimal64 rhs);

/// Multiply the value of the specified `lhs` object by the value of the
/// specified `rhs` as described by IEEE-754 and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if one of the operands is infinity (positive or negative)
///   and the other is zero (positive or negative), then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if both `lhs` and `rhs` are infinity (positive or
///   negative), return infinity.  The sign of the returned value will be
///   positive if `lhs` and `rhs` have the same sign, and negative
///   otherwise.
/// * Otherwise, if either `lhs` or `rhs` is zero, return zero.  The sign
///   of the returned value will be positive if `lhs` and `rhs` have the
///   same sign, and negative otherwise.
/// * Otherwise if the product of `lhs` and `rhs` has an absolute value
///   that is larger than `std::numeric_limits<Decimal64>::max()` then
///   store the value of the macro `ERANGE` into `errno` and return an
///   infinity with the same sign as that result.
/// * Otherwise if the product of `lhs` and `rhs` has an absolute value
///   that is smaller than `std::numeric_limits<Decimal64>::min()` then
///   store the value of the macro `ERANGE` into `errno` and return zero
///   with the same sign as that result.
/// * Otherwise return the product of the value of `rhs` and the number
///   represented by `rhs`.
Decimal64 operator*(Decimal64 lhs, Decimal64 rhs);
Decimal64 operator*(Decimal32 lhs, Decimal64 rhs);
Decimal64 operator*(Decimal64 lhs, Decimal32 rhs);

/// Multiply the specified `rhs` by the value of the specified `lhs` as
/// described by IEEE-754, and return the result.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` is NaN, then return a NaN.
/// * Otherwise if `lhs` is infinity (positive or negative), and `rhs` is
///   zero, then store the value of the macro `EDOM` into'errno' and return
///   a NaN.
/// * Otherwise if `lhs` is infinity (positive or negative), then return
///   `lhs`.
/// * Otherwise if `rhs` is zero, then return zero with the sign of `lhs`.
/// * Otherwise if the product of `rhs` and the value of `lhs` results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal64>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if the product of `rhs` and the value of `lhs` results in
///   an absolute value that is smaller than
///   `std::numeric_limits<Decimal64>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the product of the value of `lhs` and value `rhs`.
Decimal64 operator*(Decimal64 lhs, int                rhs);
Decimal64 operator*(Decimal64 lhs, unsigned int       rhs);
Decimal64 operator*(Decimal64 lhs, long               rhs);
Decimal64 operator*(Decimal64 lhs, unsigned long      rhs);
Decimal64 operator*(Decimal64 lhs, long long          rhs);
Decimal64 operator*(Decimal64 lhs, unsigned long long rhs);

/// Multiply the specified `lhs` by the value of the specified `rhs` as
/// described by IEEE-754, and return the result.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` is NaN, then return a NaN.
/// * Otherwise if `rhs` is infinity (positive or negative), and `lhs` is
///   zero, then store the value of the macro `EDOM` into'errno' and return
///   a NaN.
/// * Otherwise if `rhs` is infinity (positive or negative), then return
///   `rhs`.
/// * Otherwise if `lhs` is zero, then return zero with the sign of `rhs`.
/// * Otherwise if the product of `lhs` and the value of `rhs` results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal64>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if the product of `lhs` and the value of `rhs` results in
///   an absolute value that is smaller than
///   `std::numeric_limits<Decimal64>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the product of the value of `lhs` and value `rhs`.
Decimal64 operator*(int                lhs, Decimal64 rhs);
Decimal64 operator*(unsigned int       lhs, Decimal64 rhs);
Decimal64 operator*(long               lhs, Decimal64 rhs);
Decimal64 operator*(unsigned long      lhs, Decimal64 rhs);
Decimal64 operator*(long long          lhs, Decimal64 rhs);
Decimal64 operator*(unsigned long long lhs, Decimal64 rhs);

/// Divide the value of the specified `lhs` by the value of the specified
/// `rhs` as described by IEEE-754, and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if `lhs` and `rhs` are both infinity (positive or negative)
///   or both zero (positive or negative) then store the value of the macro
///   `EDOM` into `errno` and return a NaN.
/// * Otherwise if `lhs` has a normal value and `rhs` has a positive zero
///   value, store the value of the macro `ERANGE` into `errno` and return
///   infinity with the sign of `lhs`.
/// * Otherwise if `lhs` has a normal value and `rhs` has a negative zero
///   value, store the value of the macro `ERANGE` into `errno` and return
///   infinity with the opposite sign as `lhs`.
/// * Otherwise if `lhs` has infinity value and `rhs` has a positive zero
///   value, return infinity with the sign of `lhs`.
/// * Otherwise if `lhs` has infinity value and `rhs` has a negative zero
///   value, return infinity with the opposite sign as `lhs`.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is larger than
///   `std::numeric_limits<Decimal64>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is smaller than
///   `std::numeric_limits<Decimal64>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the result of dividing the value of `lhs` by the
///   value of `rhs`.
Decimal64 operator/(Decimal64 lhs, Decimal64 rhs);
Decimal64 operator/(Decimal32 lhs, Decimal64 rhs);
Decimal64 operator/(Decimal64 lhs, Decimal32 rhs);

/// Divide the value of the specified `lhs` by the specified `rhs` as
/// described by IEEE-754, and return the result.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` is NaN then return a NaN.
/// * Otherwise if `lhs` is infinity (positive or negative) and `rhs` is
///   positive value then return infinity value with the same sign as its
///   original value.
/// * Otherwise if `lhs` is infinity (positive or negative) and `rhs` is
///   negative value then return infinity value with the opposite sign as
///   its original value.
/// * Otherwise if `rhs` is zero, store the value of the macro `ERANGE`
///   into `errno` and return infinity with the same sign it had prior to
///   this operation.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is larger than
///   `std::numeric_limits<Decimal64>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is smaller than
///   `std::numeric_limits<Decimal64>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the result of dividing the value of `lhs` by the
///   value `rhs`.
Decimal64 operator/(Decimal64 lhs, int                rhs);
Decimal64 operator/(Decimal64 lhs, unsigned int       rhs);
Decimal64 operator/(Decimal64 lhs, long               rhs);
Decimal64 operator/(Decimal64 lhs, unsigned long      rhs);
Decimal64 operator/(Decimal64 lhs, long long          rhs);
Decimal64 operator/(Decimal64 lhs, unsigned long long rhs);

/// Divide the specified `lhs` by the value of the specified `rhs` as
/// described by IEEE-754, and return the result.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` is NaN then return a NaN.
/// * Otherwise if `rhs` is infinity (positive or negative), and `lhs` is
///   zero, store the value of the macro `ERANGE` into `errno` and return a
///   NaN.
/// * Otherwise if `rhs` is zero (positive or negative), store the value of
///   the macro `ERANGE` into `errno` and return infinity with the sign of
///   `lhs`.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is larger than
///   `std::numeric_limits<Decimal64>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is smaller than
///   `std::numeric_limits<Decimal64>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the result of dividing the value of `lhs` by the
///   value of `rhs`.  Note that this is a floating-point operation, not
///   integer.
Decimal64 operator/(int                lhs, Decimal64 rhs);
Decimal64 operator/(unsigned int       lhs, Decimal64 rhs);
Decimal64 operator/(long               lhs, Decimal64 rhs);
Decimal64 operator/(unsigned long      lhs, Decimal64 rhs);
Decimal64 operator/(long long          lhs, Decimal64 rhs);
Decimal64 operator/(unsigned long long lhs, Decimal64 rhs);

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `Decimal64` objects have the same value if the
/// `compareQuietEqual` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representations equal.  In
/// other words, two `Decimal64` objects have the same value if:
///
/// * both have a zero value (positive or negative), or
/// * both have the same infinity value (both positive or negative), or
/// * both have the value of a real number that are equal, even if they are
///   represented differently (cohorts have the same value)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
///
/// Note that a NaN is never equal to anything, including itself:
/// ```
/// Decimal64 aNaN = std::numeric_limits<Decimal64>::quiet_NaN();
/// assert(!(aNan == aNan));
/// ```
bool operator==(Decimal64 lhs, Decimal64 rhs);

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two decimal objects have the same value if the
/// `compareQuietEqual` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representations equal.  In
/// other words, two decimal objects have the same value if:
///
/// * both have a zero value (positive or negative), or
/// * both have the same infinity value (both positive or negative), or
/// * both have the value of a real number that are equal, even if they are
///   represented differently (cohorts have the same value)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator==(Decimal32 lhs, Decimal64 rhs);
bool operator==(Decimal64 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` and `rhs` do not have the same
/// value, and `false` otherwise.  Two `Decimal64` objects do not have the
/// same value if the `compareQuietEqual` operation (IEEE-754 defined,
/// non-total ordering comparison) considers the underlying IEEE
/// representations not equal.  In other words, two `Decimal64` objects do
/// not have the same value if:
///
/// * both are a NaN, or
/// * one has zero value (positive or negative) and the other does not, or
/// * one has the value of positive infinity and the other does not, or
/// * one has the value of negative infinity and the other does not, or
/// * both have the value of a real number that are not equal, regardless
///   of their representation (cohorts are equal)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
///
/// Note that a NaN is never equal to anything, including itself:
/// ```
/// Decimal64 aNaN = std::numeric_limits<Decimal64>::quiet_NaN();
/// assert(aNan != aNan);
/// ```
bool operator!=(Decimal64 lhs, Decimal64 rhs);

/// Return `true` if the specified `lhs` and `rhs` do not have the same
/// value, and `false` otherwise.  Two decimal objects do not have the same
/// value if the `compareQuietEqual` operation (IEEE-754 defined, non-total
/// ordering comparison) considers the underlying IEEE representations not
/// equal.  In other words, two decimal objects do not have the same value
/// if:
///
/// * both are NaN, or
/// * one has zero value (positive or negative) and the other does not, or
/// * one has the value of positive infinity and the other does not, or
/// * one has the value of negative infinity and the other does not, or
/// * both have the value of a real number that are not equal, regardless
///   of their representation (cohorts are equal)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator!=(Decimal32 lhs, Decimal64 rhs);
bool operator!=(Decimal64 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` has a value less than the specified
/// `rhs` and `false` otherwise.  The value of a `Decimal64` object `lhs` is
/// less than that of an object `rhs` if the `compareQuietLess` operation
/// (IEEE-754 defined, non-total ordering comparison) considers the
/// underlying IEEE representation of `lhs` to be less than of that of
/// `rhs`.  In other words, `lhs` is less than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` is zero (positive or negative) and `rhs` is positive, or
/// * `rhs` is zero (positive or negative) and `lhs` negative, or
/// * `lhs` is not positive infinity, or
/// * `lhs` is negative infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is less than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<(Decimal64 lhs, Decimal64 rhs);

/// Return `true` if the specified `lhs` has a value less than the specified
/// `rhs` and `false` otherwise.  The value of a decimal object `lhs` is
/// less than that of an object `rhs` if the `compareQuietLess` operation
/// (IEEE-754 defined, non-total ordering comparison) considers the
/// underlying IEEE representation of `lhs` to be less than of that of
/// `rhs`.  In other words, `lhs` is less than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` is zero (positive or negative) and `rhs` is positive, or
/// * `rhs` is zero (positive or negative) and `lhs` negative, or
/// * `lhs` is not positive infinity, or
/// * `lhs` is negative infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is less than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<(Decimal32 lhs, Decimal64 rhs);
bool operator<(Decimal64 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` has a value less than or equal the
/// value of the specified `rhs` and `false` otherwise.  The value of a
/// `Decimal64` object `lhs` is less than or equal to the value of an object
/// `rhs` if the `compareQuietLessEqual` operation (IEEE-754 defined,
/// non-total ordering comparison) considers the underlying IEEE
/// representation of `lhs` to be less or equal to that of `rhs`.  In other
/// words, `lhs` is less or equal than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are positive infinity, or
/// * `lhs` is negative infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is less or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<=(Decimal64 lhs, Decimal64 rhs);

/// Return `true` if the specified `lhs` has a value less than or equal the
/// value of the specified `rhs` and `false` otherwise.  The value of a
/// decimal object `lhs` is less than or equal to the value of an object
/// `rhs` if the `compareQuietLessEqual` operation (IEEE-754 defined,
/// non-total ordering comparison) considers the underlying IEEE
/// representation of `lhs` to be less or equal to that of `rhs`.  In other
/// words, `lhs` is less or equal than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are positive infinity, or
/// * `lhs` is negative infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is less or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<=(Decimal32 lhs, Decimal64 rhs);
bool operator<=(Decimal64 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` has a greater value than the
/// specified `rhs` and `false` otherwise.  The value of a `Decimal64`
/// object `lhs` is greater than that of an object `rhs` if the
/// `compareQuietGreater` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representation of `lhs` to be
/// greater than of that of `rhs`.  In other words, `lhs` is greater than
/// `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `rhs` is zero (positive or negative) and `lhs` positive, or
/// * `lhs` is zero (positive or negative) and `rhs` negative, or
/// * `lhs` is not negative infinity, or
/// * `lhs` is positive infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>(Decimal64 lhs, Decimal64 rhs);

/// Return `true` if the specified `lhs` has a greater value than the
/// specified `rhs` and `false` otherwise.  The value of a decimal object
/// `lhs` is greater than that of an object `rhs` if the
/// `compareQuietGreater` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representation of `lhs` to be
/// greater than of that of `rhs`.  In other words, `lhs` is greater than
/// `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `rhs` is zero (positive or negative) and `lhs` positive, or
/// * `lhs` is zero (positive or negative) and `rhs` negative, or
/// * `lhs` is not negative infinity, or
/// * `lhs` is positive infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>(Decimal32 lhs, Decimal64 rhs);
bool operator>(Decimal64 lhs, Decimal32 rhs);

/// Return `true` if the specified `lhs` has a value greater than or equal
/// to the value of the specified `rhs` and `false` otherwise.  The value of
/// a `Decimal64` object `lhs` is greater or equal to a `Decimal64` object
/// `rhs` if the `compareQuietGreaterEqual` operation (IEEE-754 defined,
/// non-total ordering comparison ) considers the underlying IEEE
/// representation of `lhs` to be greater or equal to that of `rhs`.  In
/// other words, `lhs` is greater than or equal to `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are negative infinity, or
/// * `lhs` is positive infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>=(Decimal64 lhs, Decimal64 rhs);

/// Return `true` if the specified `lhs` has a value greater than or equal
/// to the value of the specified `rhs` and `false` otherwise.  The value of
/// a decimal object `lhs` is greater or equal to a decimal object `rhs` if
/// the `compareQuietGreaterEqual` operation (IEEE-754 defined, non-total
/// ordering comparison ) considers the underlying IEEE representation of
/// `lhs` to be greater or equal to that of `rhs`.  In other words, `lhs` is
/// greater than or equal to `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are negative infinity, or
/// * `lhs` is positive infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>=(Decimal32 lhs, Decimal64 rhs);
bool operator>=(Decimal64 lhs, Decimal32 rhs);

/// Read, into the specified `object`, from the specified input `stream` an
/// IEEE 64 bit decimal floating-point value as described in the IEEE-754
/// 2008 standard (5.12 Details of conversions between floating point
/// numbers and external character sequences) and return a reference
/// providing modifiable access to `stream`.  If `stream` contains a Nan
/// value, it is unspecified if `object` will receive a quiet or signaling
/// `Nan`.  If `stream` is not valid on entry `stream.good() == false`, this
/// operation has no effect other than setting `stream.fail()` to `true`.
/// If eof (end-of-file) is found before any non-whitespace characters
/// `stream.fail()` is set to `true` and `object` remains unchanged.  If eof
/// is detected after some characters have been read (and successfully
/// interpreted as part of the textual representation of a floating-point
/// value as specified by IEEE-754) then `stream.eof()` is set to true.  If
/// the first non-whitespace character sequence is not a valid textual
/// representation of a floating-point value (e.g., 12e or e12 or 1*2) the
/// `stream.fail()` is set to true and `object` will remain unchanged.  If a
/// real number value is represented by the character sequence but it is a
/// large positive or negative value that cannot be stored into `object`
/// then store the value of the macro `ERANGE` into `errno` and positive or
/// negative infinity is stored into `object`, respectively.  If a real
/// number value is represented by the character sequence but it is a small
/// positive or negative value that cannot be stored into `object` then
/// store the value of the macro `ERANGE` into `errno` and positive or
/// negative zero is stored into `object`, respectively.  If a real number
/// value is represented by the character sequence but it cannot be stored
/// exactly into `object`, the value is rounded according to the current
/// rounding direction (of the environment) and then stored into `object`.
///
/// NOTE: This method does not yet fully support iostream flags or the
/// decimal floating point exception context.
template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
operator>> (bsl::basic_istream<CHARTYPE, TRAITS>& stream, Decimal64& object);

/// Write the value of the specified `object` to the specified output
/// `stream` in a single line format as described in the IEEE-754 2008
/// standard (5.12 Details of conversions between floating point numbers and
/// external character sequences), and return a reference providing
/// modifiable access to `stream`.  If `stream` is not valid on entry, this
/// operation has no effect.
///
/// NOTE: This method does not yet fully support iostream flags or the
/// decimal floating point exception context.
template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
operator<< (bsl::basic_ostream<CHARTYPE, TRAITS>& stream, Decimal64 object);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
inline namespace literals {
inline namespace DecimalLiterals {
/// Produce an object of the indicated return type by parsing the specified
/// `str` having the specified `len` excluding the terminating null
/// character that represents a floating-point number written in both fixed
/// and scientific notations.  These user-defined literal suffixes can be
/// applied to both numeric and string literals, (i.e., 1.2_d128, "1.2"_d64
/// or "inf"_d64). The resulting decimal object is initialized as follows:
///
/// * If `str` does not represent a floating-point value, then return a
///   decimal object of the indicated return type initialized to a NaN.
/// * Otherwise if `str` represents infinity (positive or negative), then
///   return a decimal object of the indicated return type initialized to
///   infinity value with the same sign.
/// * Otherwise if `str` represents zero (positive or negative), then
///   return a decimal object of the indicated return type initialized to
///   zero with the same sign.
/// * Otherwise if `str` represents a value that has an absolute value that
///   is larger than the maximum value supported by the indicated return
///   type, then store the value of the macro `ERANGE` into `errno` and
///   return a decimal object of the return type initialized to infinity
///   with the same sign.
/// * Otherwise if `str` represents a value that has an absolute value that
///   is smaller than min value of the indicated return type, then store
///   the value of the macro `ERANGE` into `errno` and return a decimal
///   object of the return type initialized to zero with the same sign.
/// * Otherwise if `str` has a value that is not exactly representable
///   using the maximum digit number supported by the indicated return
///   type, then return a decimal object of the return type initialized to
///   the value represented by `str` rounded according to the rounding
///   direction.
/// * Otherwise return a decimal object of the indicated return type
///   initialized to the decimal value representation of `str`.
///
/// Note that the parsing follows the rules as specified for the `strtod64`
/// function in section 9.6 of the ISO/EIC TR 247128 C Decimal
/// Floating-Point Technical Report.
///
/// Also note that the numeric literal version omits the optional leading
/// sign in `str`.  For example, if the string is -1.2_d64 then the string
/// "1.2" is passed to the one-argument form, not "-1.2", because leading
/// signs are operators, not parts of literals.  On the other hand, the
/// string literal version does not omit leading sign and if the string is
/// "-1.2"_d64 then the string "-1.2" is passed to the two-argument form.
///
/// Also note that the quantum of the resultant value is affected by the
/// number of decimal places in `str` string in both numeric and string
/// literal formats starting with the most significand digit and cannot
/// exceed the maximum number of digits necessary to differentiate all
/// values of the indicated return type, for example:
///
/// `0.015_d64;               =>              15e-3`
/// `1.5_d64;                 =>              15e-1`
/// `1.500_d64;               =>            1500e-3`
/// `1.2345678901234567_d64;  => 1234567890123458-15`
bdldfp::Decimal64  operator""_d64 (const char *str);
bdldfp::Decimal64  operator""_d64 (const char *str, bsl::size_t len);

}  // close DecimalLiterals namespace
}  // close literals namespace
#endif

// FREE FUNCTIONS

/// Pass the specified `object` to the specified `hashAlg`.  This function
/// integrates with the `bslh` modular hashing system and effectively
/// provides a `bsl::hash` specialization for `Decimal64`.  Note that two
/// objects which have the same value but different representations will
/// hash to the same value.
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Decimal64& object);


                           // =====================
                           // class Decimal_Type128
                           // =====================

/// This value-semantic class implements the IEEE-754 128 bit decimal
/// floating-point format arithmetic type.  This class is a standard layout
/// type that is `const` thread-safe and exception-neutral.
class Decimal_Type128 {

  private:
    // DATA
    DecimalImpUtil::ValueType128 d_value;
                                          // The underlying IEEE representation

  public:
    // CLASS METHODS

                                  // Aspects

    /// Return the maximum valid BDEX format version, as indicated by the
    /// specified `versionSelector`, to be passed to the `bdexStreamOut`
    /// method.  Note that it is highly recommended that `versionSelector`
    /// be formatted as "YYYYMMDD", a date representation.  Also note that
    /// `versionSelector` should be a *compile*-time-chosen value that
    /// selects a format version supported by both externalizer and
    /// unexternalizer.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    static int maxSupportedBdexVersion();
    static int maxSupportedBdexVersion(int versionSelector);

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Decimal_Type128,
                                   bsl::is_trivially_copyable);

    // CREATORS

    /// Create a `Decimal128_Type` object having the value positive zero and
    /// the smallest exponent value.
    Decimal_Type128();

    /// Create a `Decimal128_Type` object having the specified `value`.
    Decimal_Type128(DecimalImpUtil::ValueType128 value);            // IMPLICIT

    /// Create a `Decimal128_Type` object having the specified `value`,
    /// subject to the conversion rules as defined by IEEE-754:
    ///
    /// * If `value` is NaN, initialize this object to a NaN.
    /// * Otherwise if `value` is infinity, then initialize this object to
    ///   infinity with the same sign.
    /// * Otherwise if `value` is zero, then initialize this object to zero
    ///   with the same sign.
    /// * Otherwise initialize this object to `value`.
    Decimal_Type128(Decimal32 value);                               // IMPLICIT
    Decimal_Type128(Decimal64 value);                               // IMPLICIT

    /// Create a `Decimal128_Type` object having the value closest to the
    /// specified `other` value.  *Warning:* clients requiring a conversion
    /// for an exact decimal value should use `bdldfp_decimalconvertutil`
    /// (see *WARNING*: Conversions from `float` and `double`}.  This
    /// conversion follows the conversion rules as defined by IEEE-754:
    ///
    /// * If `value` is NaN, initialize this object to a NaN.
    /// * Otherwise if `value` is infinity, then initialize this object to
    ///   infinity value with the same sign.
    /// * Otherwise if `value` has a zero value, then initialize this
    ///   object to zero with the same sign.
    /// * Otherwise initialize this object to `value`.
    explicit Decimal_Type128(float  other);
    explicit Decimal_Type128(double other);

    /// Create a `Decimal128_Type` object having the value closest to the
    /// specified `value` subject to the conversion rules as defined by
    /// IEEE-754:
    ///
    /// * If `value` has an absolute value that is larger than
    ///   `std::numeric_limits<Decimal128>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and initialize this object to
    ///   infinity with the same sign as `other`.
    /// * Otherwise if `value` has a value that is not exactly
    ///   representable using `std::numeric_limits<Decimal128>::max_digit`
    ///   decimal digits then initialize this object to the value of
    ///   `value` rounded according to the rounding direction.
    /// * Otherwise initialize this object to `value` with exponent 0.
    explicit Decimal_Type128(int                value);
    explicit Decimal_Type128(unsigned int       value);
    explicit Decimal_Type128(long               value);
    explicit Decimal_Type128(unsigned long      value);
    explicit Decimal_Type128(long long          value);
    explicit Decimal_Type128(unsigned long long value);

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

    /// Add 1.0 to the value of this object and return a reference to it.
    /// Note that this is a floating-point value so this operation may not
    /// change the value of this object at all (if the value is large) or it
    /// may just set it to 1.0 (if the original value is small).
    Decimal_Type128& operator++();

    /// Add -1.0 to the value of this object and return a reference to it.
    /// Note that this is a floating-point value so this operation may not
    /// change the value of this object at all (if the value is large) or it
    /// may just set it to -1.0 (if the original value is small).
    Decimal_Type128& operator--();

    /// Add the value of the specified `rhs` object to the value of this as
    /// described by IEEE-754, store the result in this object, and return a
    /// reference to this object.
    ///
    /// * If either of this object or `rhs` is signaling NaN, then store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise if this object and `rhs` have infinity value of
    ///   differing signs, store the value of the macro `EDOM` into `errno`
    ///   and set this object to a NaN.
    /// * Otherwise if this object and `rhs` have infinite values of the
    ///   same sign, then do not change this object.
    /// * Otherwise if `rhs` has a zero value (positive or negative), do
    ///   not change this object.
    /// * Otherwise if the sum of this object and `rhs` has an absolute
    ///   value that is larger than
    ///   `std::numeric_limits<Decimal128>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise set this object to the sum of the number represented by
    ///   `rhs` and the number represented by this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    Decimal_Type128& operator+=(Decimal32  rhs);
    Decimal_Type128& operator+=(Decimal64  rhs);
    Decimal_Type128& operator+=(Decimal128 rhs);

    /// Add the specified `rhs` to the value of this object as described by
    /// IEEE-754, store the result in this object, and return a reference to
    /// this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN, then do not change this object.
    /// * Otherwise if this object is infinity, then do not change it.
    /// * Otherwise if the sum of this object and `rhs` has an absolute
    ///   value that is larger than
    ///   `std::numeric_limits<Decimal128>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise set this object to sum of adding `rhs` and the number
    ///   represented by this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    Decimal_Type128& operator+=(int                rhs);
    Decimal_Type128& operator+=(unsigned int       rhs);
    Decimal_Type128& operator+=(long               rhs);
    Decimal_Type128& operator+=(unsigned long      rhs);
    Decimal_Type128& operator+=(long long          rhs);
    Decimal_Type128& operator+=(unsigned long long rhs);

    /// Subtract the value of the specified `rhs` from the value of this
    /// object as described by IEEE-754, store the result in this object,
    /// and return a reference to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise if this object and `rhs` have infinity value of the
    ///   same signs, store the value of the macro `EDOM` into `errno`
    ///   and set this object to a NaN.
    /// * Otherwise if this object and the `rhs` have infinite values of
    ///   differing signs, then do not change this object.
    /// * Otherwise if the `rhs` has a zero value (positive or negative),
    ///   do not change this object.
    /// * Otherwise if subtracting the value of the `rhs` object from this
    ///   results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal128>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise set this object to the result of subtracting the value
    ///   of `rhs` from the value of this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    Decimal_Type128& operator-=(Decimal32  rhs);
    Decimal_Type128& operator-=(Decimal64  rhs);
    Decimal_Type128& operator-=(Decimal128 rhs);

    /// Subtract the specified `rhs` from the value of this object as
    /// described by IEEE-754, store the result in this object, and return a
    /// reference to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN, then do not change this object.
    /// * Otherwise if this object is infinity, then do not change it.
    /// * Otherwise if subtracting `rhs` from this object's value results
    ///   in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal128>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise set this object to the result of subtracting `rhs` from
    ///   the value of this object.
    ///
    /// Note that this is a floating-point value so this operations may not
    /// change the value of this object at all (if the value is large) or it
    /// may seem to update it to the value of the `other` (if the original
    /// value is small).
    Decimal_Type128& operator-=(int                rhs);
    Decimal_Type128& operator-=(unsigned int       rhs);
    Decimal_Type128& operator-=(long               rhs);
    Decimal_Type128& operator-=(unsigned long      rhs);
    Decimal_Type128& operator-=(long long          rhs);
    Decimal_Type128& operator-=(unsigned long long rhs);

    /// Multiply the value of the specified `rhs` object by the value of
    /// this as described by IEEE-754, store the result in this object, and
    /// return a reference to this object.
    ///
    /// * If either of this object or `rhs` is signaling NaN, then store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise, if one of this object and `rhs` is zero (positive or
    ///   negative) and the other is infinity (positive or negative), store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise, if either this object or `rhs` is positive or negative
    ///   infinity, set this object to infinity.  The sign of this object
    ///   will be positive if this object and `rhs` had the same sign, and
    ///   negative otherwise.
    /// * Otherwise, if either this object or `rhs` is zero, set this
    ///   object to zero.  The sign of this object will be positive if this
    ///   object and `rhs` had the same sign, and negative otherwise.
    /// * Otherwise if the product of this object and `rhs` has an absolute
    ///   value that is larger than
    ///   `std::numeric_limits<Decimal128>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign of that result.
    /// * Otherwise if the product of this object and `rhs` has an absolute
    ///   value that is smaller than
    ///   `std::numeric_limits<Decimal128>::min()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to zero value
    ///   with the same sign as that result.
    /// * Otherwise set this object to the product of the value of `rhs`
    ///   and the value of this object.
    Decimal_Type128& operator*=(Decimal32  rhs);
    Decimal_Type128& operator*=(Decimal64  rhs);
    Decimal_Type128& operator*=(Decimal128 rhs);

    /// Multiply the specified `rhs` by the value of this object as
    /// described by IEEE-754, store the result in this object, and return a
    /// reference to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN, then do not change this object.
    /// * Otherwise if this object is infinity (positive or negative), and
    ///   `rhs` is zero, then store the value of the macro `EDOM` into
    ///   `errno` and set this object to a NaN.
    /// * Otherwise if this object is infinity (positive or negative), then
    ///   do not change it.
    /// * Otherwise if `rhs` is zero, then set this object to zero with the
    ///   same sign as its value had prior to this operation.
    /// * Otherwise if the product of `rhs` and the value of this object
    ///   results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal128>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to infinity
    ///   with the same sign as that result.
    /// * Otherwise if the product of `rhs` and the value of this object
    ///   results in an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal128>::min()` then store the value of
    ///   the macro `ERANGE` into `errno` and set this object to zero with
    ///   the same sign as that result.
    /// * Otherwise set this object to the product of the value of this
    ///   object and the value `rhs`.
    Decimal_Type128& operator*=(int                rhs);
    Decimal_Type128& operator*=(unsigned int       rhs);
    Decimal_Type128& operator*=(long               rhs);
    Decimal_Type128& operator*=(unsigned long      rhs);
    Decimal_Type128& operator*=(long long          rhs);
    Decimal_Type128& operator*=(unsigned long long rhs);

    /// Divide the value of this object by the value of the specified `rhs`
    /// as described by IEEE-754, store the result in this object, and
    /// return a reference to this object.
    ///
    /// * If either of this object or `rhs` is signaling NaN, then store
    ///   the value of the macro `EDOM` into `errno` and set this object to
    ///   a NaN.
    /// * Otherwise if either of this object or `rhs` is NaN then set this
    ///   object to a NaN.
    /// * Otherwise if this object and `rhs` are both infinity (positive or
    ///   negative) or both zero (positive or negative) then store the
    ///   value of the macro `EDOM` into `errno` and return a NaN.
    /// * Otherwise if `rhs` has a positive zero value, then store the
    ///   value of the macro `ERANGE` into `errno` and set this object to
    ///   infinity with the same sign as its original value.
    /// * Otherwise if `rhs` has a negative zero value, then store the
    ///   value of the macro `ERANGE` into `errno` and set this object to
    ///   infinity with the opposite sign as its original value.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal128>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and return infinity with the same
    ///   sign as that result.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal128>::min()` then store the value of
    ///   the macro `ERANGE` into `errno`and return zero with the same sign
    ///   as that result.
    /// * Otherwise set this object to the result of dividing the value of
    ///   this object by the value of `rhs`.
    Decimal_Type128& operator/=(Decimal32  rhs);
    Decimal_Type128& operator/=(Decimal64  rhs);
    Decimal_Type128& operator/=(Decimal128 rhs);

    /// Divide the value of this object by the specified `rhs` as described
    /// by IEEE-754, store the result in this object, and return a reference
    /// to this object.
    ///
    /// * If this object is signaling NaN, then store the value of the
    ///   macro `EDOM` into `errno` and set this object to a NaN.
    /// * Otherwise if this object is NaN then set this object to a NaN.
    /// * Otherwise if this object is infinity (positive or negative) and
    ///   `rhs` is positive value then set this object to infinity value
    ///   with the same sign as its original value.
    /// * Otherwise if this object is infinity (positive or negative) and
    ///   `rhs` is negative value then set this object to infinity value
    ///   with the opposite sign as its original value.
    /// * Otherwise if `rhs` is zero, store the value of the macro `ERANGE`
    ///   into `errno` and set this object to infinity with the same sign
    ///   it had prior to this operation.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is larger than
    ///   `std::numeric_limits<Decimal128>::max()` then store the value of
    ///   the macro `ERANGE` into `errno` and return infinity with the same
    ///   sign as that result.
    /// * Otherwise if dividing the value of this object by the value of
    ///   `rhs` results in an absolute value that is smaller than
    ///   `std::numeric_limits<Decimal128>::min()` then store the value of
    ///   the macro `ERANGE` into `errno`and return zero with the same sign
    ///   as that result.
    /// * Otherwise set this object to the result of dividing the value of
    ///   this object by the value of `rhs`.
    Decimal_Type128& operator/=(int                rhs);
    Decimal_Type128& operator/=(unsigned int       rhs);
    Decimal_Type128& operator/=(long               rhs);
    Decimal_Type128& operator/=(unsigned long      rhs);
    Decimal_Type128& operator/=(long long          rhs);
    Decimal_Type128& operator/=(unsigned long long rhs);

    /// Return a modifiable pointer to the underlying implementation.
    DecimalImpUtil::ValueType128 *data();

                                  // Aspects

    /// Assign to this object the value read from the specified input
    /// `stream` using the specified `version` format, and return a
    /// reference to `stream`.  If `stream` is initially invalid, this
    /// operation has no effect.  If `version` is not supported, this object
    /// is unaltered and `stream` is invalidated, but otherwise unmodified.
    /// If `version` is supported but `stream` becomes invalid during this
    /// operation, this object has an undefined, but valid, state.  Note
    /// that no version is read from `stream`.  See the `bslx` package-level
    /// documentation for more information on BDEX streaming of
    /// value-semantic types and containers.
    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);

    // ACCESSORS

    /// Return a non-modifiable pointer to the underlying implementation.
    const DecimalImpUtil::ValueType128 *data() const;

    /// Return the value of the underlying implementation.
    DecimalImpUtil::ValueType128 value() const;

                                  // Aspects

    /// Write the value of this object, using the specified `version`
    /// format, to the specified output `stream`, and return a reference to
    /// `stream`.  If `stream` is initially invalid, this operation has no
    /// effect.  If `version` is not supported, `stream` is invalidated, but
    /// otherwise unmodified.  Note that `version` is not written to
    /// `stream`.  See the `bslx` package-level documentation for more
    /// information on BDEX streaming of value-semantic types and
    /// containers.
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
};

// FREE OPERATORS

/// Return a copy of the specified `value` if the value is not negative
/// zero, and return positive zero otherwise.
Decimal128 operator+(Decimal128 value);

/// Return the result of applying the unary - operator to the specified
/// `value` as described by IEEE-754.  Note that floating-point numbers have
/// signed zero, therefore this operation is not the same as `0-value`.
Decimal128 operator-(Decimal128 value);

/// Apply the prefix ++ operator to the specified `value` and return its
/// original value.  Note that this is a floating-point value so this
/// operations may not change the value of this object at all (if the value
/// is large) or it may just set it to 1.0 (if the original value is small).
Decimal128 operator++(Decimal128& value, int);

/// Apply the prefix -- operator to the specified `value` and return its
/// original value.  Note that this is a floating-point value so this
/// operations may not change the value of this object at all (if the value
/// is large) or it may just set it to -1.0 (if the original value is
/// small).
Decimal128 operator--(Decimal128& value, int);

/// Add the value of the specified `rhs` to the value of the specified `lhs`
/// as described by IEEE-754 and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if `lhs` and `rhs` are infinities of differing signs, store
///   the value of the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if `lhs` and `rhs` are infinities of the same sign then
///   return infinity of that sign.
/// * Otherwise if `rhs` is zero (positive or negative), return `lhs`.
/// * Otherwise if the sum of `lhs` and `rhs` has an absolute value that is
///   larger than `std::numeric_limits<Decimal128>::max()` then store the
///   value of the macro `ERANGE` into `errno` and set this object to
///   infinity with the same sign as that result.
/// * Otherwise return the sum of the number represented by `lhs` and the
///   number represented by `rhs`.
Decimal128 operator+(Decimal128 lhs, Decimal128 rhs);
Decimal128 operator+(Decimal32  lhs, Decimal128 rhs);
Decimal128 operator+(Decimal128 lhs, Decimal32  rhs);
Decimal128 operator+(Decimal64  lhs, Decimal128 rhs);
Decimal128 operator+(Decimal128 lhs, Decimal64  rhs);

/// Add the specified `rhs` to the value of the specified `lhs` as described
/// by IEEE-754 and return the result.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` object is NaN, then return a NaN.
/// * Otherwise if `lhs` is infinity, then return infinity.
/// * Otherwise if the sum of `lhs` and `rhs` has an absolute value that is
///   larger than `std::numeric_limits<Decimal128>::max()` then store the
///   value of the macro `ERANGE` into `errno` and return infinity with the
///   same sign as that result.
/// * Otherwise return the sum of `rhs` and the number represented by
///   `lhs`.
Decimal128 operator+(Decimal128 lhs, int                rhs);
Decimal128 operator+(Decimal128 lhs, unsigned int       rhs);
Decimal128 operator+(Decimal128 lhs, long               rhs);
Decimal128 operator+(Decimal128 lhs, unsigned long      rhs);
Decimal128 operator+(Decimal128 lhs, long long          rhs);
Decimal128 operator+(Decimal128 lhs, unsigned long long rhs);

/// Add the specified `lhs` to the value of the specified `rhs` as described
/// by IEEE-754 and return the result.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` object is NaN, then return a NaN.
/// * Otherwise if `rhs` is infinity, then return infinity.
/// * Otherwise if the sum of `lhs` and `rhs` has an absolute value that is
///   larger than `std::numeric_limits<Decimal128>::max()` then store the
///   value of the macro `ERANGE` into `errno` and return infinity with the
///   same sign as that result.
/// * Otherwise return the sum of `lhs` and the number represented by
///   `rhs`.
Decimal128 operator+(int                lhs, Decimal128 rhs);
Decimal128 operator+(unsigned int       lhs, Decimal128 rhs);
Decimal128 operator+(long               lhs, Decimal128 rhs);
Decimal128 operator+(unsigned long      lhs, Decimal128 rhs);
Decimal128 operator+(long long          lhs, Decimal128 rhs);
Decimal128 operator+(unsigned long long lhs, Decimal128 rhs);

/// Subtract the value of the specified `rhs` from the value of the
/// specified `lhs` as described by IEEE-754 and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if `lhs` and the `rhs` have infinity values of the same
///   sign, store the value of the macro `EDOM` into `errno` and return a
///   NaN.
/// * Otherwise if `lhs` and the `rhs` have infinity values of differing
///   signs, then return `lhs`.
/// * Otherwise if the subtracting of `lhs` and `rhs` has an absolute value
///   that is larger than `std::numeric_limits<Decimal128>::max()` then
///   store the value of the macro `ERANGE` into `errno` and return
///   infinity with the same sign as that result.
/// * Otherwise return the result of subtracting the value of `rhs`from the
///   value of `lhs`.
Decimal128 operator-(Decimal128 lhs, Decimal128 rhs);
Decimal128 operator-(Decimal32  lhs, Decimal128 rhs);
Decimal128 operator-(Decimal128 lhs, Decimal32  rhs);
Decimal128 operator-(Decimal64  lhs, Decimal128 rhs);
Decimal128 operator-(Decimal128 lhs, Decimal64  rhs);

/// Subtract the specified `rhs` from the value of the specified `lhs` as
/// described by IEEE-754 and return a reference to this object.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` is NaN, then return a NaN.
/// * Otherwise if `lhs` is infinity, then return infinity.
/// * Otherwise if subtracting `rhs` from `lhs` object's value results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal128>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise return the result of subtracting `rhs` from the value of
///   `lhs`.
Decimal128 operator-(Decimal128 lhs, int                rhs);
Decimal128 operator-(Decimal128 lhs, unsigned int       rhs);
Decimal128 operator-(Decimal128 lhs, long               rhs);
Decimal128 operator-(Decimal128 lhs, unsigned long      rhs);
Decimal128 operator-(Decimal128 lhs, long long          rhs);
Decimal128 operator-(Decimal128 lhs, unsigned long long rhs);

/// Subtract the specified `rhs` from the value of the specified `lhs` as
/// described by IEEE-754 and return a reference to this object.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` is NaN, then return a NaN.
/// * Otherwise if `rhs` is infinity, then return infinity.
/// * Otherwise if subtracting `rhs` from `lhs` object's value results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal128>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise return the result of subtracting the value of `rhs` from
///   the number `lhs`.
Decimal128 operator-(int                lhs, Decimal128 rhs);
Decimal128 operator-(unsigned int       lhs, Decimal128 rhs);
Decimal128 operator-(long               lhs, Decimal128 rhs);
Decimal128 operator-(unsigned long      lhs, Decimal128 rhs);
Decimal128 operator-(long long          lhs, Decimal128 rhs);
Decimal128 operator-(unsigned long long lhs, Decimal128 rhs);

/// Multiply the value of the specified `lhs` object by the value of the
/// specified `rhs` as described by IEEE-754 and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if one of the operands is infinity (positive or negative)
///   and the other is zero (positive or negative), then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if both `lhs` and `rhs` are infinity (positive or
///   negative), return infinity.  The sign of the returned value will be
///   positive if `lhs` and `rhs` have the same sign, and negative
///   otherwise.
/// * Otherwise, if either `lhs` or `rhs` is zero, return zero.  The sign
///   of the returned value will be positive if `lhs` and `rhs` have the
///   same sign, and negative otherwise.
/// * Otherwise if the product of `lhs` and `rhs` has an absolute value
///   that is larger than `std::numeric_limits<Decimal128>::max()` then
///   store the value of the macro `ERANGE` into `errno` and return an
///   infinity with the same sign as that result.
/// * Otherwise if the product of `lhs` and `rhs` has an absolute value
///   that is smaller than `std::numeric_limits<Decimal128>::min()` then
///   store the value of the macro `ERANGE` into `errno` and return zero
///   with the same sign as that result.
/// * Otherwise return the product of the value of `rhs` and the number
///   represented by `rhs`.
Decimal128 operator*(Decimal128 lhs, Decimal128 rhs);
Decimal128 operator*(Decimal32  lhs, Decimal128 rhs);
Decimal128 operator*(Decimal128 lhs, Decimal32  rhs);
Decimal128 operator*(Decimal64  lhs, Decimal128 rhs);
Decimal128 operator*(Decimal128 lhs, Decimal64  rhs);

/// Multiply the specified `rhs` by the value of the specified `lhs` as
/// described by IEEE-754, and return the result.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` is NaN, then return a NaN.
/// * Otherwise if `lhs` is infinity (positive or negative), and `rhs` is
///   zero, then store the value of the macro `EDOM` into'errno' and return
///   a NaN.
/// * Otherwise if `lhs` is infinity (positive or negative), then return
///   `lhs`.
/// * Otherwise if `rhs` is zero, then return zero with the sign of `lhs`.
/// * Otherwise if the product of `rhs` and the value of `lhs` results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal128>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if the product of `rhs` and the value of `lhs` results in
///   an absolute value that is smaller than
///   `std::numeric_limits<Decimal128>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the product of the value of `lhs` and value `rhs`.
Decimal128 operator*(Decimal128 lhs, int                rhs);
Decimal128 operator*(Decimal128 lhs, unsigned int       rhs);
Decimal128 operator*(Decimal128 lhs, long               rhs);
Decimal128 operator*(Decimal128 lhs, unsigned long      rhs);
Decimal128 operator*(Decimal128 lhs, long long          rhs);
Decimal128 operator*(Decimal128 lhs, unsigned long long rhs);

/// Multiply the specified `lhs` by the value of the specified `rhs` as
/// described by IEEE-754, and return the result.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` is NaN, then return a NaN.
/// * Otherwise if `rhs` is infinity (positive or negative), and `lhs` is
///   zero, then store the value of the macro `EDOM` into'errno' and return
///   a NaN.
/// * Otherwise if `rhs` is infinity (positive or negative), then return
///   `rhs`.
/// * Otherwise if `lhs` is zero, then return zero with the sign of `rhs`.
/// * Otherwise if the product of `lhs` and the value of `rhs` results in
///   an absolute value that is larger than
///   `std::numeric_limits<Decimal128>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if the product of `lhs` and the value of `rhs` results in
///   an absolute value that is smaller than
///   `std::numeric_limits<Decimal128>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the product of the value of `lhs` and value `rhs`.
Decimal128 operator*(int                lhs, Decimal128 rhs);
Decimal128 operator*(unsigned int       lhs, Decimal128 rhs);
Decimal128 operator*(long               lhs, Decimal128 rhs);
Decimal128 operator*(unsigned long      lhs, Decimal128 rhs);
Decimal128 operator*(long long          lhs, Decimal128 rhs);
Decimal128 operator*(unsigned long long lhs, Decimal128 rhs);

/// Divide the value of the specified `lhs` by the value of the specified
/// `rhs` as described by IEEE-754, and return the result.
///
/// * If either of `lhs` or `rhs` is signaling NaN, then store the value of
///   the macro `EDOM` into `errno` and return a NaN.
/// * Otherwise if either of `lhs` or `rhs` is NaN, return a NaN.
/// * Otherwise if `lhs` and `rhs` are both infinity (positive or negative)
///   or both zero (positive or negative) then store the value of the macro
///   `EDOM` into `errno` and return a NaN.
/// * Otherwise if `lhs` has a normal value and `rhs` has a positive zero
///   value, store the value of the macro `ERANGE` into `errno` and return
///   infinity with the sign of `lhs`.
/// * Otherwise if `lhs` has a normal value and `rhs` has a negative zero
///   value, store the value of the macro `ERANGE` into `errno` and return
///   infinity with the opposite sign as `lhs`.
/// * Otherwise if `lhs` has infinity value and `rhs` has a positive zero
///   value, return infinity with the sign of `lhs`.
/// * Otherwise if `lhs` has infinity value and `rhs` has a negative zero
///   value, return infinity with the opposite sign as `lhs`.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is larger than
///   `std::numeric_limits<Decimal128>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is smaller than
///   `std::numeric_limits<Decimal128>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the result of dividing the value of `lhs` by the
///   value of `rhs`.
Decimal128 operator/(Decimal128 lhs, Decimal128 rhs);
Decimal128 operator/(Decimal32  lhs, Decimal128 rhs);
Decimal128 operator/(Decimal128 lhs, Decimal32  rhs);
Decimal128 operator/(Decimal64  lhs, Decimal128 rhs);
Decimal128 operator/(Decimal128 lhs, Decimal64  rhs);

/// Divide the value of the specified `lhs` by the specified `rhs` as
/// described by IEEE-754, and return the result.
///
/// * If `lhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `lhs` is NaN then return a NaN.
/// * Otherwise if `lhs` is infinity (positive or negative) and `rhs` is
///   positive value then return infinity value with the same sign as its
///   original value.
/// * Otherwise if `lhs` is infinity (positive or negative) and `rhs` is
///   negative value then return infinity value with the opposite sign as
///   its original value.
/// * Otherwise if `rhs` is zero, store the value of the macro `ERANGE`
///   into `errno` and return infinity with the same sign it had prior to
///   this operation.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is larger than
///   `std::numeric_limits<Decimal128>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is smaller than
///   `std::numeric_limits<Decimal128>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the result of dividing the value of `lhs` by the
///   value of `rhs`.
Decimal128 operator/(Decimal128 lhs, int                rhs);
Decimal128 operator/(Decimal128 lhs, unsigned int       rhs);
Decimal128 operator/(Decimal128 lhs, long               rhs);
Decimal128 operator/(Decimal128 lhs, unsigned long      rhs);
Decimal128 operator/(Decimal128 lhs, long long          rhs);
Decimal128 operator/(Decimal128 lhs, unsigned long long rhs);

/// Divide the specified `lhs` by the value of the specified `rhs` as
/// described by IEEE-754, and return the result.
///
/// * If `rhs` is signaling NaN, then store the value of the macro `EDOM`
///   into `errno` and return a NaN.
/// * Otherwise if `rhs` is NaN then return a NaN.
/// * Otherwise if `rhs` is infinity (positive or negative), and `lhs` is
///   zero, store the value of the macro `ERANGE` into `errno` and return a
///   NaN.
/// * Otherwise if `rhs` is zero (positive or negative), store the value of
///   the macro `ERANGE` into `errno` and return infinity with the sign of
///   `lhs`.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is larger than
///   `std::numeric_limits<Decimal128>::max()` then store the value of the
///   macro `ERANGE` into `errno` and return infinity with the same sign as
///   that result.
/// * Otherwise if dividing the value of `lhs` by the value of `rhs`
///   results in an absolute value that is smaller than
///   `std::numeric_limits<Decimal128>::min()` then store the value of the
///   macro `ERANGE` into `errno` and return zero with the same sign as
///   that result.
/// * Otherwise return the result of dividing the value of `lhs` by the
///   value of `rhs`.  Note that this is a floating-point operation, not
///   integer.
Decimal128 operator/(int                lhs, Decimal128 rhs);
Decimal128 operator/(unsigned int       lhs, Decimal128 rhs);
Decimal128 operator/(long               lhs, Decimal128 rhs);
Decimal128 operator/(unsigned long      lhs, Decimal128 rhs);
Decimal128 operator/(long long          lhs, Decimal128 rhs);
Decimal128 operator/(unsigned long long lhs, Decimal128 rhs);

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `Decimal128` objects have the same value if the
/// `compareQuietEqual` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representations equal.  In
/// other words, two `Decimal128` objects have the same value if:
///
/// * both have a zero value (positive or negative), or
/// * both have the same infinity value (both positive or negative), or
/// * both have the value of a real number that are equal, even if they are
///   represented differently (cohorts have the same value)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
///
/// Note that a NaN is never equal to anything, including itself:
/// ```
/// Decimal128 aNaN = std::numeric_limits<Decimal128>::quiet_NaN();
/// assert(!(aNan == aNan));
/// ```
bool operator==(Decimal128 lhs, Decimal128 rhs);

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two decimal objects have the same value if the
/// `compareQuietEqual` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representations equal.  In
/// other words, two decimal objects have the same value if:
///
/// * both have a zero value (positive or negative), or
/// * both have the same infinity value (both positive or negative), or
/// * both have the value of a real number that are equal, even if they are
///   represented differently (cohorts have the same value)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator==(Decimal32  lhs, Decimal128 rhs);
bool operator==(Decimal128 lhs, Decimal32  rhs);
bool operator==(Decimal64  lhs, Decimal128 rhs);
bool operator==(Decimal128 lhs, Decimal64  rhs);

/// Return `true` if the specified `lhs` and `rhs` do not have the same
/// value, and `false` otherwise.  Two `Decimal128` objects do not have the
/// same value if the `compareQuietEqual` operation (IEEE-754 defined,
/// non-total ordering comparison) considers the underlying IEEE
/// representations not equal.  In other words, two `Decimal128` objects do
/// not have the same value if:
///
/// * both are a NaN, or
/// * one has zero value (positive or negative) and the other does not, or
/// * one has the value of positive infinity and the other does not, or
/// * one has the value of negative infinity and the other does not, or
/// * both have the value of a real number that are not equal, regardless
///   of their representation (cohorts are equal)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
///
/// Note that a NaN is never equal to anything, including itself:
/// ```
/// Decimal128 aNaN = std::numeric_limits<Decimal128>::quiet_NaN();
/// assert(aNan != aNan);
/// ```
bool operator!=(Decimal128 lhs, Decimal128 rhs);

/// Return `true` if the specified `lhs` and `rhs` do not have the same
/// value, and `false` otherwise.  Two decimal objects do not have the same
/// value if the `compareQuietEqual` operation (IEEE-754 defined, non-total
/// ordering comparison) considers the underlying IEEE representations not
/// equal.  In other words, two decimal objects do not have the same value
/// if:
///
/// * both are NaN, or
/// * one has zero value (positive or negative) and the other does not, or
/// * one has the value of positive infinity and the other does not, or
/// * one has the value of negative infinity and the other does not, or
/// * both have the value of a real number that are not equal, regardless
///   of their representation (cohorts are equal)
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator!=(Decimal32  lhs, Decimal128 rhs);
bool operator!=(Decimal128 lhs, Decimal32  rhs);
bool operator!=(Decimal64  lhs, Decimal128 rhs);
bool operator!=(Decimal128 lhs, Decimal64  rhs);

/// Return `true` if the specified `lhs` has a value less than the specified
/// `rhs` and `false` otherwise.  The value of a `Decimal128` object `lhs`
/// is less than that of an object `rhs` if the `compareQuietLess` operation
/// (IEEE-754 defined, non-total ordering comparison) considers the
/// underlying IEEE representation of `lhs` to be less than of that of
/// `rhs`.  In other words, `lhs` is less than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` is zero (positive or negative) and `rhs` is positive, or
/// * `rhs` is zero (positive or negative) and `lhs` negative, or
/// * `lhs` is not positive infinity, or
/// * `lhs` is negative infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is less than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<(Decimal128 lhs, Decimal128 rhs);

/// Return `true` if the specified `lhs` has a value less than the specified
/// `rhs` and `false` otherwise.  The value of a decimal object `lhs` is
/// less than that of an object `rhs` if the `compareQuietLess` operation
/// (IEEE-754 defined, non-total ordering comparison) considers the
/// underlying IEEE representation of `lhs` to be less than of that of
/// `rhs`.  In other words, `lhs` is less than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` is zero (positive or negative) and `rhs` is positive, or
/// * `rhs` is zero (positive or negative) and `lhs` negative, or
/// * `lhs` is not positive infinity, or
/// * `lhs` is negative infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs`is less than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<(Decimal32  lhs, Decimal128 rhs);
bool operator<(Decimal128 lhs, Decimal32  rhs);
bool operator<(Decimal64  lhs, Decimal128 rhs);
bool operator<(Decimal128 lhs, Decimal64  rhs);

/// Return `true` if the specified `lhs` has a value less than or equal the
/// value of the specified `rhs` and `false` otherwise.  The value of a
/// `Decimal128` object `lhs` is less than or equal to the value of an
/// object `rhs` if the `compareQuietLessEqual` operation (IEEE-754 defined,
/// non-total ordering comparison) considers the underlying IEEE
/// representation of `lhs` to be less or equal to that of `rhs`.  In other
/// words, `lhs` is less or equal than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are positive infinity, or
/// * `lhs` is negative infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is less or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<=(Decimal128 lhs, Decimal128 rhs);

/// Return `true` if the specified `lhs` has a value less than or equal the
/// value of the specified `rhs` and `false` otherwise.  The value of a
/// decimal object `lhs` is less than or equal to the value of an object
/// `rhs` if the `compareQuietLessEqual` operation (IEEE-754 defined,
/// non-total ordering comparison) considers the underlying IEEE
/// representation of `lhs` to be less or equal to that of `rhs`.  In other
/// words, `lhs` is less or equal than `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are positive infinity, or
/// * `lhs` is negative infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is less or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator<=(Decimal32  lhs, Decimal128 rhs);
bool operator<=(Decimal128 lhs, Decimal32  rhs);
bool operator<=(Decimal64  lhs, Decimal128 rhs);
bool operator<=(Decimal128 lhs, Decimal64  rhs);

/// Return `true` if the specified `lhs` has a greater value than the
/// specified `rhs` and `false` otherwise.  The value of a `Decimal128`
/// object `lhs` is greater than that of an object `rhs` if the
/// `compareQuietGreater` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representation of `lhs` to be
/// greater than of that of `rhs`.  In other words, `lhs` is greater than
/// `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `rhs` is zero (positive or negative) and `lhs` positive, or
/// * `lhs` is zero (positive or negative) and `rhs` negative, or
/// * `lhs` is not negative infinity, or
/// * `lhs` is positive infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>(Decimal128 lhs, Decimal128 rhs);

/// Return `true` if the specified `lhs` has a greater value than the
/// specified `rhs` and `false` otherwise.  The value of a decimal object
/// `lhs` is greater than that of an object `rhs` if the
/// `compareQuietGreater` operation (IEEE-754 defined, non-total ordering
/// comparison) considers the underlying IEEE representation of `lhs` to be
/// greater than of that of `rhs`.  In other words, `lhs` is greater than
/// `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `rhs` is zero (positive or negative) and `lhs` positive, or
/// * `lhs` is zero (positive or negative) and `rhs` negative, or
/// * `lhs` is not negative infinity, or
/// * `lhs` is positive infinity and `rhs` is not, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater than that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>(Decimal32  lhs, Decimal128 rhs);
bool operator>(Decimal128 lhs, Decimal32  rhs);
bool operator>(Decimal64  lhs, Decimal128 rhs);
bool operator>(Decimal128 lhs, Decimal64  rhs);

/// Return `true` if the specified `lhs` has a value greater than or equal
/// to the value of the specified `rhs` and `false` otherwise.  The value of
/// a `Decimal128` object `lhs` is greater or equal to a `Decimal128` object
/// `rhs` if the `compareQuietGreaterEqual` operation (IEEE-754 defined,
/// non-total ordering comparison ) considers the underlying IEEE
/// representation of `lhs` to be greater or equal to that of `rhs`.  In
/// other words, `lhs` is greater than or equal to `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are negative infinity, or
/// * `lhs` is positive infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>=(Decimal128 lhs, Decimal128 rhs);

/// Return `true` if the specified `lhs` has a value greater than or equal
/// to the value of the specified `rhs` and `false` otherwise.  The value of
/// a decimal object `lhs` is greater or equal to a decimal object `rhs` if
/// the `compareQuietGreaterEqual` operation (IEEE-754 defined, non-total
/// ordering comparison ) considers the underlying IEEE representation of
/// `lhs` to be greater or equal to that of `rhs`.  In other words, `lhs` is
/// greater than or equal to `rhs` if:
///
/// * neither `lhs` nor `rhs` are NaN, or
/// * `lhs` and `rhs` are both zero (positive or negative), or
/// * both `lhs` and `rhs` are negative infinity, or
/// * `lhs` is positive infinity, or
/// * `lhs` and `rhs` both represent a real number and the real number of
///   `lhs` is greater or equal to that of `rhs`
///
/// This operation stores the value of the macro `EDOM` into `errno` if
/// either or both operands are signaling NaN.
bool operator>=(Decimal32  lhs, Decimal128 rhs);
bool operator>=(Decimal128 lhs, Decimal32  rhs);
bool operator>=(Decimal64  lhs, Decimal128 rhs);
bool operator>=(Decimal128 lhs, Decimal64  rhs);

/// Read, into the specified `object`, from the specified input `stream` an
/// IEEE 128 bit decimal floating-point value as described in the IEEE-754
/// 2008 standard (5.12 Details of conversions between floating point
/// numbers and external character sequences) and return a reference
/// providing modifiable access to `stream`.  If `stream` contains a Nan
/// value, it is unspecified if `object` will receive a quiet or signaling
/// `Nan`.  If `stream` is not valid on entry `stream.good() == false`, this
/// operation has no effect other than setting `stream.fail()` to `true`.
/// If eof (end-of-file) is found before any non-whitespace characters
/// `stream.fail()` is set to `true` and `object` remains unchanged.  If eof
/// is detected after some characters have been read (and successfully
/// interpreted as part of the textual representation of a floating-point
/// value as specified by IEEE-754) then `stream.eof()` is set to true.  If
/// the first non-whitespace character sequence is not a valid textual
/// representation of a floating-point value (e.g., 12e or e12 or 1*2) the
/// `stream.fail()` is set to true and `object` will remain unchanged.  If a
/// real number value is represented by the character sequence but it is a
/// large positive or negative value that cannot be stored into `object`
/// then store the value of the macro `ERANGE` into `errno` and positive or
/// negative infinity is stored into `object`, respectively.  If a real
/// number value is represented by the character sequence but it is a small
/// positive or negative value that cannot be stored into `object` then
/// store the value of the macro `ERANGE` into `errno` and positive or
/// negative zero is stored into `object`, respectively.  If a real number
/// value is represented by the character sequence but it cannot be stored
/// exactly into `object`, the value is rounded according to the current
/// rounding direction (of the environment) and then stored into `object`.
///
/// NOTE: This method does not yet fully support iostream flags or the
/// decimal floating point exception context.
template <class CHARTYPE, class TRAITS>
bsl::basic_istream<CHARTYPE, TRAITS>&
operator>> (bsl::basic_istream<CHARTYPE, TRAITS>& stream, Decimal128& object);

/// Write the value of the specified `object` to the specified output
/// `stream` in a single line format as described in the IEEE-754 2008
/// standard (5.12 Details of conversions between floating point numbers and
/// external character sequences), and return a reference providing
/// modifiable access to `stream`.  If `stream` is not valid on entry, this
/// operation has no effect.
///
/// NOTE: This method does not yet fully support iostream flags or the
/// decimal floating point exception context.
template <class CHARTYPE, class TRAITS>
bsl::basic_ostream<CHARTYPE, TRAITS>&
operator<< (bsl::basic_ostream<CHARTYPE, TRAITS>& stream, Decimal128 object);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
inline namespace literals {
inline namespace DecimalLiterals {
/// Produce an object of the indicated return type by parsing the specified
/// `str` having the specified `len` excluding the terminating null
/// character that represents a floating-point number written in both fixed
/// and scientific notations.  These user-defined literal suffixes can be
/// applied to both numeric and string literals, (i.e., 1.2_d128, "1.2"_d128
/// or "inf"_d128). The resulting decimal object is initialized as follows:
///
/// * If `str` does not represent a floating-point value, then return a
///   decimal object of the indicated return type initialized to a NaN.
/// * Otherwise if `str` represents infinity (positive or negative), then
///   return a decimal object of the indicated return type initialized to
///   infinity value with the same sign.
/// * Otherwise if `str` represents zero (positive or negative), then
///   return a decimal object of the indicated return type initialized to
///   zero with the same sign.
/// * Otherwise if `str` represents a value that has an absolute value that
///   is larger than the maximum value supported by the indicated return
///   type, then store the value of the macro `ERANGE` into `errno` and
///   return a decimal object of the return type initialized to infinity
///   with the same sign.
/// * Otherwise if `str` represents a value that has an absolute value that
///   is smaller than min value of the indicated return type, then store
///   the value of the macro `ERANGE` into `errno` and return a decimal
///   object of the return type initialized to zero with the same sign.
/// * Otherwise if `str` has a value that is not exactly representable
///   using the maximum digit number supported by the indicated return
///   type, then return a decimal object of the return type initialized to
///   the value represented by `str` rounded according to the rounding
///   direction.
/// * Otherwise return a decimal object of the indicated return type
///   initialized to the decimal value representation of `str`.
///
/// Note that the parsing follows the rules as specified for the `strtod128`
/// function in section 9.6 of the ISO/EIC TR 247128 C Decimal
/// Floating-Point Technical Report.
///
/// Also note that the numeric literal version omits the optional leading
/// sign in `str`.  For example, if the string is -1.2_d128 then the string
/// "1.2" is passed to the one-argument form, not "-1.2", because leading
/// signs are operators, not parts of literals.  On the other hand, the
/// string literal version does not omit leading sign and if the string is
/// "-1.2"_d128 then the string "-1.2" is passed to the two-argument form.
///
/// Also note that the quantum of the resultant value is affected by the
/// number of decimal places in `str` string in both numeric and string
/// literal formats starting with the most significand digit and cannot
/// exceed the maximum number of digits necessary to differentiate all
/// values of the indicated return type, for example:
///
/// `0.015_d128;                  =>                                 15e-3`
/// `1.5_d128;                    =>                                 15e-1`
/// `1.500_d128;                  =>                               1500e-3`
/// '1.2345678901234567890123456789012349_d128;
///                               => 1234567890123456789012345678901235e-33'
bdldfp::Decimal128 operator""_d128(const char *str);
bdldfp::Decimal128 operator""_d128(const char *str, bsl::size_t len);

}  // close DecimalLiterals namespace
}  // close literals namespace
#endif

// FREE FUNCTIONS

/// Pass the specified `object` to the specified `hashAlg`.  This function
/// integrates with the `bslh` modular hashing system and effectively
/// provides a `bsl::hash` specialization for `Decimal128`.  Note that two
/// objects which have the same value but different representations will
/// hash to the same value.
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Decimal128& object);

                        // MISCELLANEOUS RELATED TYPES

                          // ===================
                          // class DecimalNumGet
                          // ===================

/// A facet type (mechanism) used in reading decimal floating-point types.
/// Note that this type does not follow BDE conventions because its content
/// is dictated by the C++ standard and native standard library
/// implementations.  See ISO/IEC TR 24733 3.10.2 for details.
template <class CHARTYPE,
          class INPUTITERATOR = bsl::istreambuf_iterator<CHARTYPE> >
class DecimalNumGet : public bsl::locale::facet {

#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
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

    /// Constructs a `DecimalNumGet` object.  Optionally specify starting
    /// reference count `refs`, which will default to 0.  If `refs` is
    /// non-zero, the `DecimalNumGet` object will not be deleted when the
    /// last locale referencing it goes out of scope.
    explicit DecimalNumGet(bsl::size_t refs = 0);

    // ACCESSORS

    /// Forward to, and return using the specified `begin`, `end`, `str`,
    /// `err`, and `value`, the results of
    /// `this->do_get(begin, end, str, err, value)`.
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

  protected:
    // CREATORS

    /// Destroy this object.  Note that the destructor is virtual.
    ~DecimalNumGet() BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS

    /// Interpret characters from the half-open iterator range denoted by
    /// the specified `begin` and `end`, generate a decimal floating-point
    /// number and store it into the specified `value`.  During conversion
    /// the formatting flags of the specified `str` (`str.flags()`) are
    /// obeyed; character classifications are determined by the `bsl::ctype`
    /// while punctuation characters are determined by the `bsl::numpunct`
    /// facet imbued to the `str` stream-base.  Use the specified `err` to
    /// report back failure or EOF streams states.  For further, more
    /// detailed information please consult the section
    /// [lib.facet.num.get.virtuals] of the C++ Standard.  Note that for the
    /// conversions to the `Decimal32`, 64 and 128 types the conversion
    /// specifiers are %Hg, %Dg and %DDg, respectively.  Also note that
    /// these (possibly overridden) `do_get` virtual function are used by
    /// every formatted C++ stream input operator call (`in >> aDecNumber`).
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
};

                // ============================================
                // template <class CHARTYPE, bool WCHAR_8_BITS>
                // class WideBufferWrapper
                // ============================================

/// This class provides a wrapper around a buffer of the specified (template
/// parameter) `CHARTYPE`.  `CHARTYPE` shall be either plain character type
/// `char` or wide character type `wchar_t`.  The width of `wchar_t` is
/// compiler-specific and can be as small as 8 bits.  The template parameter
/// `WCHAR_8_BITS` shall be `true` if `wchar_t` and `char` widths are the
/// same, i.e. 8 bits, and `false` otherwise.  This class provides accessors
/// to the beginning and the end of the buffer of `CHARTYPE` characters.
template <class CHARTYPE, bool WCHAR_8_BITS>
class DecimalNumPut_WideBufferWrapper;

            // ========================================================
            // template <bool WCHAR_8_BIT>
            // class DecimalNumPut_WideBufferWrapper<char, WCHAR_8_BIT>
            // ========================================================

/// This class is specialization of the template
/// `WideBufferWrapper<CHARTYPE, WCHAR_8_BITS>` for `char` type and
/// `wchar_t` type which width is 8 bits.
template <bool WCHAR_8_BIT>
class DecimalNumPut_WideBufferWrapper<char, WCHAR_8_BIT> {

    // DATA
    const char *d_begin;  // pointer to the beginning of plain character buffer
    const char *d_end;    // pointer to the end of plain character buffer

    // NOT IMPLEMENTED
    DecimalNumPut_WideBufferWrapper(const DecimalNumPut_WideBufferWrapper&);
    DecimalNumPut_WideBufferWrapper& operator=(
                                    const DecimalNumPut_WideBufferWrapper&);

  public:
    // CREATORS

    /// Create a wide buffer wrapper for the specified `buffer` of the
    /// specified length `len`.
    DecimalNumPut_WideBufferWrapper(const char         *buffer,
                                    int                 len,
                                    const bsl::locale&);

    // ACCESSORS

    /// Return a pointer to the beginning of the buffer of plain characters
    /// provided in this class constructor.
    const char *begin() const;

    /// Return a pointer to the end of the buffer of plain characters
    /// provided in this class constructor.
    const char *end() const;
};

            // =====================================================
            // template <>
            // class DecimalNumPut_WideBufferWrapper<wchar_t, false>
            // =====================================================

/// This class is specialization of the template
/// `WideBufferWrapper<CHARTYPE, WCHAR_8_BIT>` for `wchar_t` type which
/// width exceeds 8 bits.
template <>
class DecimalNumPut_WideBufferWrapper<wchar_t, false> {

    // DATA
    wchar_t *d_buffer_p;  // Buffer of wide characters
    size_t   d_len;       // Length of the buffer

    // NOT IMPLEMENTED
    DecimalNumPut_WideBufferWrapper(const DecimalNumPut_WideBufferWrapper&);
    DecimalNumPut_WideBufferWrapper& operator=(
                                    const DecimalNumPut_WideBufferWrapper&);

  public:
    // CREATORS

    /// Create a wide buffer wrapper for the specified `buffer` of the
    /// specified length `len`.  Use the specified locale `loc` to widen
    /// character in the buffer into wide characters representation.
    inline
    DecimalNumPut_WideBufferWrapper(const char         *buffer,
                                    int                 len,
                                    const bsl::locale&  loc);

    /// Destroy this object.
    ~DecimalNumPut_WideBufferWrapper();

    // ACCESSORS

    /// Return a pointer to the beginning of the buffer of wide characters.
    const wchar_t *begin() const;

    /// Return a pointer to the end the buffer of wide characters.
    const wchar_t *end() const;
};

             // =============================================
             // template class Decimal_FormatterSpecification
             // =============================================

/// This template is the implementation of the `bsl::format` specification for
/// `bdldfp::Decimal*` types.  It parses as well as postprocesses format
/// specifications for `bdldfp::Decimal*` type formatted arguments, as well as
/// stores the results of that pasring: the specification.
template <class t_CHAR>
struct Decimal_FormatterSpecification {

    // PUBLIC TYPES

    /// This enumeration is used as an indication of the format-type letter in
    /// a decimal floating point format string.
    enum FormatType {
        e_TYPE_UNASSIGNED,       /// Default value

        // Decimal floating point types
        e_FORMAT_DEFAULT,        /// none
        e_FORMAT_SCIENTIFIC,     /// `e`
        e_FORMAT_SCIENTIFIC_UC,  /// `E`
        e_FORMAT_FIXED,          /// `f`
        e_FORMAT_FIXED_UC,       /// `F`
        e_FORMAT_GENERAL,        /// `g`
        e_FORMAT_GENERAL_UC,     /// `g`
    };

  private:
    // PRIVATE TYPES

    ///  Just an abbreviation for shorter lines.
    typedef bslfmt::FormatSpecificationParser<t_CHAR> Parser;

    // DATA
    typename Parser::ProcessingState d_parsingStatus;  // the state of parsing

    Parser                           d_parser;         // specification parser

    FormatType                       d_formatType;     // what type was
                                                       // requested

    // PRIVATE MANIPULATORS

    /// Parse, from the specified `typeString`, the requested format-type and
    /// load it into the `d_formatType`.  This method will throw a
    /// `bsl::format_error` exception in case the `typeString` is not empty or
    /// a valid, single format character.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseType(
                             const bsl::basic_string_view<t_CHAR>& typeString);

    // PRIVATE ACCESSORS

    /// If `d_parsingStatus` is not yet at least `e_PARSED` throw a
    /// `bsl::format_error` exception, otherwise if `d_parsingStatus` is at
    /// least `e_PARSED` or higher (later in the process) do nothing.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void ensureParsingComplete() const;

    /// If `d_parsingStatus` is not `e_STATE_POSTPROCESSED` (the final
    /// state) throw a `bsl::format_error` exception, otherwise if
    /// `d_parsingStatus` is `e_STATE_POSTPROCESSED` do nothing.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void ensurePostprocessingComplete() const;

  public:
    // CREATORS

    /// Create an uninitialized `Decimal_FormatterSpecification` object.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Decimal_FormatterSpecification();

    // MANIPULATORS

    /// Parse a decimal floating point format string using the iterator-range
    /// from the specified `context` and if successful load the results into
    /// this object as well as set its status to `e_PARSED`; otherwise, if the
    /// format specification denoted by the `context` iterator-range is not a
    /// valid decimal floating point format specification throw a
    /// `bsl::format_error` exception.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parse(t_PARSE_CONTEXT *context);

    /// Postprocess this object using the argument values provided by the
    /// specified `context` to fill in the values of nested width or precision
    /// parameters if such deferred parameters exist and set the status to
    /// `e_STATE_POSTPROCESSED`.  By nested format parameters we mean
    /// parameters whose value comes from an argument to the formatter
    /// function, and not an literal integer value within the format string.
    /// In case of an error throw a `bsl::format_error` exception.
    template <typename t_FORMAT_CONTEXT>
    void postprocess(const t_FORMAT_CONTEXT& context);

    // ACCESSORS

    /// Return a pointer to the character array that stored the parsed filler
    /// character that may be a multibyte code point or just a single character
    /// unless the status is not `e_STATE_POSTPROCESSED` in which case throw
    /// a `bsl::format_error` exception indicating that error.  See also
    /// `numFillerCharacters()` that provides the number of characters in the
    /// array returned by this function (at least one).
    BSLS_KEYWORD_CONSTEXPR_CPP20 const t_CHAR *filler() const;

    /// Return the number of filler characters in the array returned by
    /// `filler()`unless the status is not `e_STATE_POSTPROCESSED` in which
    /// case throw a `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int fillerCharacters() const;

    /// Return the display width of the code point represented by the array
    /// returned by `filler()`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int fillerCodePointDisplayWidth() const;

    /// Return the enumerator representing the requested alignment unless the
    /// status is not at least `e_STATE_PARSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename Parser::Alignment alignment() const;

    /// Return the enumerator representing the requested sign-treatment option
    /// unless the status is not at least `e_STATE_PARSED` in which case
    /// throw a `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename Parser::Sign sign() const;

    /// Return a boolean indicating if alternative formatting was requested
    /// unless the status is not at least `e_STATE_PARSED` in which case
    /// throw a `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool alternativeFlag() const;

    /// Return a boolean indicating if zero padding was requested unless the
    /// status is not at least `e_STATE_PARSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool zeroPaddingFlag() const;

    /// Return an optional value representing the requested width unless the
    /// status is not `e_STATE_POSTPROCESSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.  Note that the
    /// returned type is capable of representing more than just an optional
    /// integer, but after preprocessing it will have only two possible states:
    /// no value, or an integer value.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    const bslfmt::FormatterSpecificationNumericValue
    postprocessedWidth() const;

    /// Return an optional value representing the requested precision unless
    /// the status is not `e_STATE_POSTPROCESSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.  Note that the
    /// returned type is capable of representing more than just an optional
    /// integer, but after preprocessing it will have only two possible states:
    /// no value, or an integer value.
    BSLS_KEYWORD_CONSTEXPR_CPP20
    const bslfmt::FormatterSpecificationNumericValue
    postprocessedPrecision() const;

    /// Return a boolean indicating if the locale specific flag was present in
    /// the format specification unless the status is not at least
    /// `e_STATE_PARSED` in which case throw a `bsl::format_error` exception
    /// indicating that error.  Note that the locale specific flag is not yet
    /// supported hence the attempt to format with a specification that has
    /// this flags set will result in an exception indicating that.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool localeSpecificFlag() const;

    /// Return the format-type requested unless the status is not at least
    /// `e_STATE_PARSED` in which case throw a `bsl::format_error` exception
    /// indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatType formatType() const;
};

              // ===========================================
              // template struct Decimal_BslFmtFormatterImpl
              // ===========================================

/// This class template provides the implementation for all possible decimal
/// floating point formatting styles and the parsing of the format
/// specification.  The specified `t_VALUE` template type argument determines
/// the type of decimal floating point value use, while the specified `t_CHAR`
/// determines both the formatting string and the output's character type.  The
/// behavior is undefined unless `t_VALUE` is one of `Decimal32`, `Decimal64,
/// or `Decimal128``, and `t_CHAR` is on of `char` or `wchar_t`.
template <class t_VALUE, class t_CHAR>
struct Decimal_BslFmtFormatterImpl {
  private:
    // PRIVATE CLASS TYPES

    /// A type alias for the `FormatterSpecificationDecimal<t_CHAR>`.
    typedef Decimal_FormatterSpecification<t_CHAR> Specification;

    // DATA
    Specification d_spec;  /// Parsed specification.

    // PRIVATE CONSTANTS

    /// A constant representing the case when a position is not found in a
    /// string-search.
    static const size_t k_NO_POS = (size_t)(-1);

  private:
    // PRIVATE MANIPULATORS

    /// Copy the specified `numberBuffer` of size `numberLength` aligned with
    /// fills according to the specified `finalSpec` to the output iterator of
    /// the `formatContext` and return an iterator one-past the last written.
    /// The behavior is undefined unless `t_FORMAT_CONTEXT` is either
    /// `std::format_context` if that is supported, or otherwise
    /// `bslfmt::format_context`.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator alignAndCopy(
                                        const char           *numberBuffer,
                                        size_t                numberLength,
                                        t_FORMAT_CONTEXT&     formatContext,
                                        const Specification&  finalSpec) const;
  public:
    // MANIPULATORS

    /// Create string representation of the specified `value`, customized in
    /// accordance with the requested format and the specified `formatContext`,
    /// and copy it to the output that the output iterator of the
    /// `formatContext` points to.  The behavior is undefined unless
    /// `t_FORMAT_CONTEXT` is either `std::format_context` if that is
    /// supported, or otherwise `bslfmt::format_context`.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        const t_VALUE&    value,
                                        t_FORMAT_CONTEXT& formatContext) const;

    /// Parse the specified `parseContext` and return an iterator, pointing to
    /// the beginning of the unparsed section of the format string.  The
    /// behavior is undefined unless `t_PARSE_CONTEXT` is either
    /// `std::parse_context` if that is supported, or otherwise
    /// `bslfmt::parse_context`.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20
    typename t_PARSE_CONTEXT::iterator
    parse(t_PARSE_CONTEXT& parseContext);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                // -------------------------------------------
                // class DecimalNumPut_WideBufferWrapper<char>
                // -------------------------------------------

//CREATORS
template <bool WCHAR_8_BIT>
inline
DecimalNumPut_WideBufferWrapper<char, WCHAR_8_BIT>::
DecimalNumPut_WideBufferWrapper(const char         *buffer,
                                int                 len,
                                const bsl::locale&)
: d_begin(buffer)
, d_end(buffer + len)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(len >= 0);
}

// ACCESSORS
template <bool WCHAR_8_BIT>
inline
const char *DecimalNumPut_WideBufferWrapper<char, WCHAR_8_BIT>::begin() const
{
    return d_begin;
}

template <bool WCHAR_8_BIT>
inline
const char *DecimalNumPut_WideBufferWrapper<char, WCHAR_8_BIT>::end() const
{
    return d_end;
}

                // ----------------------------------------------
                // class DecimalNumPut_WideBufferWrapper<wchar_t>
                // ----------------------------------------------

//CREATORS
inline
DecimalNumPut_WideBufferWrapper<wchar_t, false>::
DecimalNumPut_WideBufferWrapper(const char         *buffer,
                                int                 len,
                                const bsl::locale&  loc)
: d_buffer_p(0)
, d_len(len)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(len >= 0);

    bslma::Allocator *allocator = bslma::Default::allocator();

    d_buffer_p = (wchar_t *)allocator->allocate(sizeof(wchar_t) * len);

    bsl::use_facet<std::ctype<wchar_t> >(loc).widen(buffer,
                                                    buffer + len,
                                                    d_buffer_p);
}

inline
DecimalNumPut_WideBufferWrapper<wchar_t, false>::
~DecimalNumPut_WideBufferWrapper()
{
    bslma::Allocator *allocator = bslma::Default::allocator();
    allocator->deallocate(d_buffer_p);
}

    // ACCESSORS
inline
const wchar_t *DecimalNumPut_WideBufferWrapper<wchar_t, false>::begin() const
{
    return d_buffer_p;
}

inline
const wchar_t *DecimalNumPut_WideBufferWrapper<wchar_t, false>::end() const
{
    return d_buffer_p + d_len;
}

                          // ===================
                          // class DecimalNumPut
                          // ===================

/// A facet type (mechanism) used in writing decimal floating-point types.
/// Note that this type does not follow BDE conventions because its content
/// is dictated by the C++ standard and native standard library
/// implementations.  See ISO/IEC TR 24733 3.10.3 for details.
template <class CHARTYPE,
          class OUTPUTITERATOR = bsl::ostreambuf_iterator<CHARTYPE> >
class DecimalNumPut : public bsl::locale::facet {

#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
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

    /// Constructs a `DecimalNumPut` object.  Optionally specify starting
    /// reference count `refs`, which will default to 0.  If `refs` is
    /// non-zero, the `DecimalNumPut` object will not be deleted when the
    /// last locale referencing it goes out of scope.
    explicit DecimalNumPut(bsl::size_t refs = 0);

    // ACCESSORS

    /// Forward to, and return using the specified `out`, `str`, `fill`, and
    /// `value`, the results of `this->do_put(out, str, fill, value)`.
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

  protected:
    // CREATORS

    /// Destroy this object.  Note that the destructor is virtual.
    ~DecimalNumPut() BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS

    /// Write characters (of `char_type`) that represent the specified
    /// `value` to the output stream determined by the specified `out`
    /// output iterator.  Use the `bsl::ctype` and the `bsl::numpunct`
    /// facets imbued to the specified stream-base `ios_format` as well as
    /// the formatting flags of the `ios_format` (`bsl.flags()`) to generate
    /// the properly localized output.  The specified `fill` character will
    /// be used as a placeholder character in padded output.  For further,
    /// more detailed information please consult the section
    /// [lib.facet.num.put.virtuals] of the C++ Standard noting that the
    /// length modifiers "H", "D" and "DD" are added to the conversion
    /// specifiers of for the types Decimal32, 64 and 128, respectively.
    /// Also note that these (possibly overridden) `do_put` virtual function
    /// are used by every formatted C++ stream output operator call
    /// (`out << aDecNumber`).  Note that currently, only the width,
    /// capitalization, justification, fixed and scientific formatting flags
    /// are supported, and the operators only support code pages that
    /// include the ASCII sub-range.  Because of potential future
    /// improvements to support additional formatting flags, the operations
    /// should not be used for serialization.
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

    /// Write characters that represent the specified `value` into a string
    /// of the specified `char_type`, and output the represented decimal
    /// number to the specified `out`, adjusting for the formatting flags in
    /// the specified `ios_format` and using the specified `fill` character.
    /// Currently, formatting for the formatting flags of justification,
    /// width, uppercase, showpos, fixed and scientific are supported.
    template <class DECIMAL>
    iter_type do_put_impl(iter_type      out,
                          bsl::ios_base& ios_format,
                          char_type      fill,
                          DECIMAL        value) const;
};

                   // =====================================
                   // class Decimal_StandardNamespaceCanary
                   // =====================================

/// An empty class used for error detection when looking for the original
/// name of the standard namespace.  Do not use it.
class Decimal_StandardNamespaceCanary {
};

    // =================================================================
    // template<...> class faux_numeric_limits<NUMERIC_TYPE, DUMMY_TYPE>
    // =================================================================

/// This class is used as a base-class for manifest constants in the
/// `std::numeric_limits` specializations to overcome a Sun compiler issue.
template<class NUMERIC_TYPE, class DUMMY_TYPE = void>
class faux_numeric_limits;

      // ===============================================================
      // class faux_numeric_limits<Decimal_StandardNamespaceCanary, ...>
      // ===============================================================

/// Explicit full specialization of the standard "traits" template
/// `std::numeric_limits` for the type
/// `BloombergLP::bdldfp::Decimal_StandardNamespaceCanary`.  Note that this
/// specialization is required for technical reasons and it is identical to
/// the non-specialized default traits.
template<class DUMMY_TYPE>
class faux_numeric_limits<Decimal_StandardNamespaceCanary, DUMMY_TYPE>
{

  public:
    // CLASS DATA

    /// `BloombergLP::bdldfp::Decimal_StandardNamespaceCanary` is not a
    /// numeric type.
    static const bool is_specialized = false;
};

    // ==============================================================
    // template<...> class faux_numeric_limits<Decimal32, DUMMY_TYPE>
    // ==============================================================

template<class DUMMY_TYPE>
class faux_numeric_limits<BloombergLP::bdldfp::Decimal32, DUMMY_TYPE> {
        // Explicit full specialization of the standard "traits" template
        // 'std::numeric_limits' for the type 'BloombergLP::bdldfp::Decimal32'.

  public:
    // CLASS DATA

    /// The template instance
    /// `std::numeric_limits<BloombergLP::bdldfp::Decimal32>` is
    /// meaningfully specialized.  Also means that
    /// `BloombergLP::bdldfp::Decimal32` is a numeric type.
    static const bool is_specialized = true;

    /// The maximum number of significant digits, in the native (10) radix
    /// of the `BloombergLP::bdldfp::Decimal32` type that the type is able
    /// to represent.  Defined to be 7 by IEEE-754.
    static const int digits = 7;

    /// The maximum number of significant decimal digits that the
    /// `BloombergLP::bdldfp::Decimal32` type is able to represent.  Defined
    /// to be 7 by IEEE-754.
    static const int digits10 = digits;

    /// The number of significant decimal digits necessary to uniquely
    /// represent the significant digits of any
    /// `BloombergLP::bdldfp::Decimal32` value.  Note that max_digit10 is
    /// the same as digits10 for decimal floating-point values.
    static const int max_digits10 = digits;

    /// `BloombergLP::bdldfp::Decimal32` is a signed type.
    static const bool is_signed = true;

    /// `BloombergLP::bdldfp::Decimal32` is not an integer type.
    static const bool is_integer = false;

    /// `BloombergLP::bdldfp::Decimal32` is not an exact type, i.e.:
    /// calculations done on the type are not free of rounding errors.  Note
    /// that integer and possibly rational types may be exact,
    /// floating-point types are never exact.
    static const bool is_exact = false;

    /// The base for `BloombergLP::bdldfp::Decimal32` is decimal or 10.
    static const int radix = 10;

    /// The lowest possible negative exponent for the native base of the
    /// `BloombergLP::bdldfp::Decimal32` type that does not yet represent a
    /// denormal number.  Defined to be -95 by IEEE-754.
    static const int min_exponent = -95;

    /// The lowest possible negative decimal exponent in the
    /// `BloombergLP::bdldfp::Decimal32` type that does not yet represent a
    /// denormal number.  Defined to be -95 by IEEE-754.  Note that
    /// `min_exponent10` is the same as `min_exponent` for decimal types.
    static const int min_exponent10 = min_exponent;

    /// The highest possible positive exponent for the native base of the
    /// `BloombergLP::bdldfp::Decimal32` type that represents a finite
    /// value.  Defined to be 96 by IEEE-754.
    static const int max_exponent = 96;

    /// The highest possible positive decimal exponent of the
    /// `BloombergLP::bdldfp::Decimal32` type that represents a finite
    /// value.  Defined to be 97 by IEEE-754.  Note that `max_exponent10` is
    /// the same as `max_exponent` for decimal types.
    static const int max_exponent10 = max_exponent;

    /// `BloombergLP::bdldfp::Decimal32` can represent infinity.
    static const bool has_infinity = true;

    /// `BloombergLP::bdldfp::Decimal32` can be a non-signaling Not a
    /// Number.
    static const bool has_quiet_NaN = true;

    /// `BloombergLP::bdldfp::Decimal32` can be a signaling Not a Number.
    static const bool has_signaling_NaN = true;

    /// `BloombergLP::bdldfp::Decimal32` may contain denormal values.
    static const std::float_denorm_style has_denorm = std::denorm_present;

    /// `BloombergLP::bdldfp::Decimal32` is able to distinguish loss of
    /// precision (floating-point underflow) due to denormalization from
    /// other causes.
    static const bool has_denorm_loss = true;

    /// Decimal floating-point types represent a finite set of values.
    static const bool is_bounded = true;

    /// Decimal floating-point is not covered by the IEC 559 standard.
    static const bool is_iec559 = false;

    /// Decimal floating-point types do not have modulo representation.
    static const bool is_modulo = false;

    /// Decimal floating-point types are able to detect if a value is too
    /// small to represent as a normalized value before rounding it.
    static const bool tinyness_before = true;

    /// Decimal floating-point types implement traps to report arithmetic
    /// exceptions (required by IEEE-754).
    static const bool traps = true;

    /// The highest possible precision in the
    /// `BloombergLP::bdldfp::Decimal32` type that is large enough to
    /// output the smallest non-zero denormalized value in fixed notation.
    static const int max_precision = digits10 - 1 + (-min_exponent10);

                        // Rounding style

    /// Decimal floating-point rounding style is defined to be indeterminate
    /// by the C and C++ Decimal TRs.
    static const std::float_round_style round_style = std::round_indeterminate;
};

    // ==============================================================
    // template<...> class faux_numeric_limits<Decimal64, DUMMY_TYPE>
    // ==============================================================

template<class DUMMY_TYPE>
class faux_numeric_limits<BloombergLP::bdldfp::Decimal64, DUMMY_TYPE> {
        // Explicit full specialization of the standard "traits" template
        // 'std::numeric_limits' for the type 'BloombergLP::bdldfp::Decimal64'.

  public:
    // CLASS DATA

    /// The template instance
    /// `std::numeric_limits<BloombergLP::bdldfp::Decimal64>` is
    /// meaningfully specialized.  Also means that
    /// `BloombergLP::bdldfp::Decimal64` is a numeric type.
    static const bool is_specialized = true;

    /// The maximum number of significant digits, in the native (10) radix
    /// of the `BloombergLP::bdldfp::Decimal64` type that the type is able
    /// to represent.  Defined to be 16 by IEEE-754.
    static const int digits = 16;

    /// The maximum number of significant decimal digits that the
    /// `BloombergLP::bdldfp::Decimal64` type is able to represent.  Defined
    /// to be 16 by IEEE-754.
    static const int digits10 = digits;

    /// The number of significant decimal digits necessary to uniquely
    /// represent the significant digits of any
    /// `BloombergLP::bdldfp::Decimal64` value.  Note that max_digit10 is
    /// the same as digits10 for decimal floating-point values.
    static const int max_digits10 = digits;

    /// `BloombergLP::bdldfp::Decimal64` is a signed type.
    static const bool is_signed = true;

    /// `BloombergLP::bdldfp::Decimal64` is not an integer type.
    static const bool is_integer = false;

    /// `BloombergLP::bdldfp::Decimal64` is not an exact type, i.e.:
    /// calculations done on the type are not free of rounding errors.  Note
    /// that integer and possibly rational types may be exact,
    /// floating-point types are never exact.
    static const bool is_exact = false;

    /// The base for `BloombergLP::bdldfp::Decimal64` is decimal or 10.
    static const int radix = 10;

    /// The lowest possible negative exponent for the native base of the
    /// `BloombergLP::bdldfp::Decimal64` type that does not yet represent a
    /// denormal number.  Defined to be -383 by IEEE-754.
    static const int min_exponent = -383;

    /// The lowest possible negative decimal exponent in the
    /// `BloombergLP::bdldfp::Decimal64` type that does not yet represent a
    /// denormal number.  Defined to be -382 by IEEE-754.  Note that
    /// `min_exponent10` is the same as `min_exponent` for decimal types.
    static const int min_exponent10 = min_exponent;

    /// The highest possible positive exponent for the native base of the
    /// `BloombergLP::bdldfp::Decimal64` type that represents a finite
    /// value.  Defined to be 384 by IEEE-754.
    static const int max_exponent = 384;

    /// The highest possible positive decimal exponent of the
    /// `BloombergLP::bdldfp::Decimal64` type that represents a finite
    /// value.  Defined to be 384 by IEEE-754.  Note that `max_exponent10`
    /// is the same as `max_exponent` for decimal types.
    static const int max_exponent10 = max_exponent;

    /// `BloombergLP::bdldfp::Decimal64` can represent infinity.
    static const bool has_infinity = true;

    /// `BloombergLP::bdldfp::Decimal64` can be a non-signaling Not a
    /// Number.
    static const bool has_quiet_NaN = true;

    /// `BloombergLP::bdldfp::Decimal64` can be a signaling Not a Number.
    static const bool has_signaling_NaN = true;

    /// `BloombergLP::bdldfp::Decimal64` may contain denormal values.
    static const std::float_denorm_style has_denorm = std::denorm_present;

    /// `BloombergLP::bdldfp::Decimal64` is able to distinguish loss of
    /// precision (floating-point underflow) due to denormalization from
    /// other causes.
    static const bool has_denorm_loss = true;

    /// Decimal floating-point is not covered by the IEC 559 standard.
    static const bool is_iec559 = false;

    /// Decimal floating-point types represent a finite set of values.
    static const bool is_bounded = true;

    /// Decimal floating-point types do not have modulo representation.
    static const bool is_modulo = false;

    /// Decimal floating-point types implement traps to report arithmetic
    /// exceptions (required by IEEE-754).
    static const bool traps = true;

    /// Decimal floating-point types are able to detect if a value is too
    /// small to represent as a normalized value before rounding it.
    static const bool tinyness_before = true;

    /// The highest possible precision in the
    /// `BloombergLP::bdldfp::Decimal64` type that is large enough to
    /// output the smallest non-zero denormalized value in fixed notation.
    static const int max_precision = digits10 - 1 + (-min_exponent10);

    /// Decimal floating-point rounding style is defined to be indeterminate
    /// by the C and C++ Decimal TRs.
    static const std::float_round_style round_style = std::round_indeterminate;
};

    // ===============================================================
    // template<...> class faux_numeric_limits<Decimal128, DUMMY_TYPE>
    // ===============================================================

template<class DUMMY_TYPE>
class faux_numeric_limits<BloombergLP::bdldfp::Decimal128, DUMMY_TYPE> {
        // Explicit full specialization of the standard "traits" template
        // 'std::numeric_limits' for the type
        // 'BloombergLP::bdldfp::Decimal128'.

  public:
    // CLASS DATA

    /// The template instance
    /// `std::numeric_limits<BloombergLP::bdldfp::Decimal128>` is
    /// meaningfully specialized.  Also means that
    /// `BloombergLP::bdldfp::Decimal128` is a numeric type.
    static const bool is_specialized = true;

    /// The maximum number of significant digits, in the native (10) radix
    /// of the `BloombergLP::bdldfp::Decimal128` type that the type is able
    /// to represent.  Defined to be 34 by IEEE-754.
    static const int digits = 34;

    /// The maximum number of significant decimal digits that the
    /// `BloombergLP::bdldfp::Decimal128` type is able to represent.
    /// Defined to be 34 by IEEE-754.
    static const int digits10 = digits;

    /// The number of significant decimal digits necessary to uniquely
    /// represent the significant digits of any
    /// `BloombergLP::bdldfp::Decimal128` value.  Note that max_digit10 is
    /// the same as digits10 for decimal floating-point values.
    static const int max_digits10 = digits;

    /// `BloombergLP::bdldfp::Decimal128` is a signed type.
    static const bool is_signed = true;

    /// `BloombergLP::bdldfp::Decimal128` is not an integer type.
    static const bool is_integer = false;

    /// `BloombergLP::bdldfp::Decimal128` is not an exact type, i.e.:
    /// calculations done on the type are not free of rounding errors.  Note
    /// that integer and possibly rational types may be exact,
    /// floating-point types are never exact.
    static const bool is_exact = false;

    /// The base for `BloombergLP::bdldfp::Decimal128` is decimal or 10.
    static const int radix = 10;

    /// The lowest possible negative exponent for the native base of the
    /// `BloombergLP::bdldfp::Decimal128` type that does not yet represent a
    /// denormal number.  Defined to be -6143 by IEEE-754.
    static const int min_exponent = -6143;

    /// The lowest possible negative decimal exponent in the
    /// `BloombergLP::bdldfp::Decimal128` type that does not yet represent a
    /// denormal number.  Defined to be -6142 by IEEE-754.  Note that
    /// `min_exponent10` is the same as `min_exponent` for decimal types.
    static const int min_exponent10 = min_exponent;

    /// The highest possible positive exponent for the native base of the
    /// `BloombergLP::bdldfp::Decimal128` type that represents a finite
    /// value.  Defined to be 385 by IEEE-754.
    static const int max_exponent = 6144;

    /// The highest possible positive decimal exponent of the
    /// `BloombergLP::bdldfp::Decimal128` type that represents a finite
    /// value.  Defined to be 6145 by IEEE-754.  Note that `max_exponent10`
    /// is the same as `max_exponent` for decimal types.
    static const int max_exponent10 = max_exponent;

    /// `BloombergLP::bdldfp::Decimal128` can represent infinity.
    static const bool has_infinity = true;

    /// `BloombergLP::bdldfp::Decimal128` can be a non-signaling Not a
    /// Number.
    static const bool has_quiet_NaN = true;

    /// `BloombergLP::bdldfp::Decimal128` can be a signaling Not a Number.
    static const bool has_signaling_NaN = true;

    /// `BloombergLP::bdldfp::Decimal128` may contain denormal values.
    static const std::float_denorm_style has_denorm = std::denorm_present;

    /// `BloombergLP::bdldfp::Decimal128` is able to distinguish loss of
    /// precision (floating-point underflow) due to denormalization from
    /// other causes.
    static const bool has_denorm_loss = true;

    /// Decimal floating-point is not covered by the IEC 559 standard.
    static const bool is_iec559 = false;

    /// Decimal floating-point types represent a finite set of values.
    static const bool is_bounded = true;

    /// Decimal floating-point types do not have modulo representation.
    static const bool is_modulo = false;

    /// Decimal floating-point types implement traps to report arithmetic
    /// exceptions (required by IEEE-754).
    static const bool traps = true;

    /// Decimal floating-point types are able to detect if a value is too
    /// small to represent as a normalized value before rounding it.
    static const bool tinyness_before = true;

    /// The highest possible precision in the
    /// `BloombergLP::bdldfp::Decimal128` type that is large enough to
    /// output the smallest non-zero denormalized value in fixed notation.
    static const int max_precision = digits10 - 1 + (-min_exponent10);

    /// Decimal floating-point rounding style is defined to be indeterminate
    /// by the C and C++ Decimal TRs.
    static const std::float_round_style round_style = std::round_indeterminate;

};

             // --------------------------------------------------
             // faux_numeric_limits<Decimal32, ...> member storage
             // --------------------------------------------------

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::is_specialized;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal32, DUMMY_TYPE>::digits;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal32, DUMMY_TYPE>::digits10;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal32, DUMMY_TYPE>::max_digits10;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::is_signed;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::is_integer;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::is_exact;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal32, DUMMY_TYPE>::radix;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal32, DUMMY_TYPE>::min_exponent;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal32, DUMMY_TYPE>::min_exponent10;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal32, DUMMY_TYPE>::max_exponent;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal32, DUMMY_TYPE>::max_exponent10;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::has_infinity;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::has_quiet_NaN;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::has_signaling_NaN;

template<class DUMMY_TYPE>
const std::float_denorm_style
faux_numeric_limits<Decimal32, DUMMY_TYPE>::has_denorm;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::has_denorm_loss;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::is_iec559;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::is_bounded;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::is_modulo;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::traps;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal32, DUMMY_TYPE>::tinyness_before;

template<class DUMMY_TYPE>
const std::float_round_style
faux_numeric_limits<Decimal32, DUMMY_TYPE>::round_style;

             // --------------------------------------------------
             // faux_numeric_limits<Decimal64, ...> member storage
             // --------------------------------------------------

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::is_specialized;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal64, DUMMY_TYPE>::digits;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal64, DUMMY_TYPE>::digits10;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal64, DUMMY_TYPE>::max_digits10;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::is_signed;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::is_integer;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::is_exact;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal64, DUMMY_TYPE>::radix;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal64, DUMMY_TYPE>::min_exponent;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal64, DUMMY_TYPE>::min_exponent10;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal64, DUMMY_TYPE>::max_exponent;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal64, DUMMY_TYPE>::max_exponent10;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::has_infinity;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::has_quiet_NaN;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::has_signaling_NaN;

template<class DUMMY_TYPE>
const std::float_denorm_style
faux_numeric_limits<Decimal64, DUMMY_TYPE>::has_denorm;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::has_denorm_loss;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::is_iec559;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::is_bounded;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::is_modulo;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::traps;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal64, DUMMY_TYPE>::tinyness_before;

template<class DUMMY_TYPE>
const std::float_round_style
faux_numeric_limits<Decimal64, DUMMY_TYPE>::round_style;

            // ---------------------------------------------------
            // faux_numeric_limits<Decimal128, ...> member storage
            // ---------------------------------------------------

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::is_specialized;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal128, DUMMY_TYPE>::digits;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal128, DUMMY_TYPE>::digits10;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal128, DUMMY_TYPE>::max_digits10;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::is_signed;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::is_integer;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::is_exact;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal128, DUMMY_TYPE>::radix;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal128, DUMMY_TYPE>::min_exponent;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal128, DUMMY_TYPE>::min_exponent10;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal128, DUMMY_TYPE>::max_exponent;

template<class DUMMY_TYPE>
const int faux_numeric_limits<Decimal128, DUMMY_TYPE>::max_exponent10;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::has_infinity;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::has_quiet_NaN;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::has_signaling_NaN;

template<class DUMMY_TYPE>
const std::float_denorm_style
faux_numeric_limits<Decimal128, DUMMY_TYPE>::has_denorm;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::has_denorm_loss;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::is_iec559;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::is_bounded;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::is_modulo;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::traps;

template<class DUMMY_TYPE>
const bool faux_numeric_limits<Decimal128, DUMMY_TYPE>::tinyness_before;

template<class DUMMY_TYPE>
const std::float_round_style
faux_numeric_limits<Decimal128, DUMMY_TYPE>::round_style;

}  // close package namespace
}  // close enterprise namespace

namespace std {

  // ========================================================================
  // template<> class numeric_limits<bdldfp::Decimal_StandardNamespaceCanary>
  // ========================================================================

/// Explicit full specialization of the standard "traits" template
/// `std::numeric_limits` for the type
/// `BloombergLP::bdldfp::Decimal_StandardNamespaceCanary`.  Note that this
/// specialization is required for technical reasons and it is identical to
/// the non-specialized default traits.
template<>
class numeric_limits<BloombergLP::bdldfp::Decimal_StandardNamespaceCanary>
    : public BloombergLP::bdldfp::faux_numeric_limits<
        BloombergLP::bdldfp::Decimal_StandardNamespaceCanary> {
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

    /// Return the smallest positive (also non-zero) number
    /// `BloombergLP::bdldfp::Decimal32` can represent (IEEE-754: +1e-95).
    static BloombergLP::bdldfp::Decimal32 min() BSLS_KEYWORD_NOEXCEPT;

    /// Return the largest number `BloombergLP::bdldfp::Decimal32` can
    /// represent (IEEE-754: +9.999999e+96).
    static BloombergLP::bdldfp::Decimal32 max() BSLS_KEYWORD_NOEXCEPT;

    /// Return the difference between 1 and the smallest value representable
    /// by the `BloombergLP::bdldfp::Decimal32` type.  (IEEE-754: +1e-6)
    static BloombergLP::bdldfp::Decimal32 epsilon() BSLS_KEYWORD_NOEXCEPT;

    /// Return the maximum rounding error for the
    /// `BloombergLP::bdldfp::Decimal32` type.  The actual value returned
    /// depends on the current decimal floating point rounding setting.
    static BloombergLP::bdldfp::Decimal32 round_error() BSLS_KEYWORD_NOEXCEPT;

    /// Return the smallest non-zero denormalized value for the
    /// `BloombergLP::bdldfp::Decimal32` type.  (IEEE-754: +0.000001E-95)
    static BloombergLP::bdldfp::Decimal32 denorm_min() BSLS_KEYWORD_NOEXCEPT;

    /// Return the value that represents positive infinity for the
    /// `BloombergLP::bdldfp::Decimal32` type.
    static BloombergLP::bdldfp::Decimal32 infinity() BSLS_KEYWORD_NOEXCEPT;

    /// Return a value that represents non-signaling NaN for the
    /// `BloombergLP::bdldfp::Decimal32` type.
    static BloombergLP::bdldfp::Decimal32 quiet_NaN() BSLS_KEYWORD_NOEXCEPT;

    /// Return a value that represents signaling NaN for the
    /// `BloombergLP::bdldfp::Decimal32` type.
    static
    BloombergLP::bdldfp::Decimal32 signaling_NaN() BSLS_KEYWORD_NOEXCEPT;
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

    /// Return the smallest positive (also non-zero) number
    /// `BloombergLP::bdldfp::Decimal64` can represent (IEEE-754: +1e-383).
    static BloombergLP::bdldfp::Decimal64 min() BSLS_KEYWORD_NOEXCEPT;

    /// Return the largest number `BloombergLP::bdldfp::Decimal64` can
    /// represent (IEEE-754: +9.999999999999999e+384).
    static BloombergLP::bdldfp::Decimal64 max() BSLS_KEYWORD_NOEXCEPT;

    /// Return the difference between 1 and the smallest value representable
    /// by the `BloombergLP::bdldfp::Decimal64` type.  (IEEE-754: +1e-15)
    static BloombergLP::bdldfp::Decimal64 epsilon() BSLS_KEYWORD_NOEXCEPT;

    /// Return the maximum rounding error for the
    /// `BloombergLP::bdldfp::Decimal64` type.  The actual value returned
    /// depends on the current decimal floating point rounding setting.
    static BloombergLP::bdldfp::Decimal64 round_error() BSLS_KEYWORD_NOEXCEPT;

    /// Return the smallest non-zero denormalized value for the
    /// `BloombergLP::bdldfp::Decimal64` type.  (IEEE-754:
    /// +0.000000000000001e-383)
    static BloombergLP::bdldfp::Decimal64 denorm_min() BSLS_KEYWORD_NOEXCEPT;

    /// Return the value that represents positive infinity for the
    /// `BloombergLP::bdldfp::Decimal64` type.
    static BloombergLP::bdldfp::Decimal64 infinity() BSLS_KEYWORD_NOEXCEPT;

    /// Return a value that represents non-signaling NaN for the
    /// `BloombergLP::bdldfp::Decimal64` type.
    static BloombergLP::bdldfp::Decimal64 quiet_NaN() BSLS_KEYWORD_NOEXCEPT;

    /// Return a value that represents signaling NaN for the
    /// `BloombergLP::bdldfp::Decimal64` type.
    static
    BloombergLP::bdldfp::Decimal64 signaling_NaN() BSLS_KEYWORD_NOEXCEPT;

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

    /// Return the smallest positive (also non-zero) number
    /// `BloombergLP::bdldfp::Decimal128` can represent (IEEE-754:
    /// +1e-6143).
    static BloombergLP::bdldfp::Decimal128 min() BSLS_KEYWORD_NOEXCEPT;

    /// Return the largest number `BloombergLP::bdldfp::Decimal128` can
    /// represent (IEEE-754: +9.999999999999999999999999999999999e+6144).
    static BloombergLP::bdldfp::Decimal128 max() BSLS_KEYWORD_NOEXCEPT;

    /// Return the difference between 1 and the smallest value representable
    /// by the `BloombergLP::bdldfp::Decimal128` type.  (IEEE-754: +1e-33)
    static BloombergLP::bdldfp::Decimal128 epsilon() BSLS_KEYWORD_NOEXCEPT;

    /// Return the maximum rounding error for the
    /// `BloombergLP::bdldfp::Decimal128` type.  The actual value returned
    /// depends on the current decimal floating point rounding setting.
    static BloombergLP::bdldfp::Decimal128 round_error() BSLS_KEYWORD_NOEXCEPT;

    /// Return the smallest non-zero denormalized value for the
    /// `BloombergLP::bdldfp::Decimal128` type.  (IEEE-754:
    /// +0.000000000000000000000000000000001e-6143)
    static BloombergLP::bdldfp::Decimal128 denorm_min() BSLS_KEYWORD_NOEXCEPT;

    /// Return the value that represents positive infinity for the
    /// `BloombergLP::bdldfp::Decimal128` type.
    static BloombergLP::bdldfp::Decimal128 infinity() BSLS_KEYWORD_NOEXCEPT;

    /// Return a value that represents non-signaling NaN for the
    /// `BloombergLP::bdldfp::Decimal128` type.
    static BloombergLP::bdldfp::Decimal128 quiet_NaN() BSLS_KEYWORD_NOEXCEPT;

    /// Return a value that represents signaling NaN for the
    /// `BloombergLP::bdldfp::Decimal128` type.
    static
    BloombergLP::bdldfp::Decimal128 signaling_NaN() BSLS_KEYWORD_NOEXCEPT;
};

}  // close namespace std

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bdldfp {

                      // THE DECIMAL FLOATING-POINT TYPES

                            // --------------------
                            // class Decimal_Type32
                            // --------------------

// CLASS METHODS

                                  // Aspects
inline
int Decimal_Type32::maxSupportedBdexVersion()
{
    return 1;
}

inline
int Decimal_Type32::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// CREATORS
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
Decimal_Type32::Decimal_Type32(Decimal_Type128 other)
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

// MANIPULATORS

                     // Incrementation and Decrementation

inline Decimal_Type32& Decimal_Type32::operator++()
{
    return *this += Decimal32(1);
}

inline Decimal_Type32& Decimal_Type32::operator--()
{
    return *this -= Decimal32(1);
}

                                  // Addition

inline Decimal_Type32& Decimal_Type32::operator+=(Decimal32 rhs)
{
    this->d_value = DecimalImpUtil::add(this->d_value, rhs.d_value);
    return *this;
}

inline Decimal_Type32& Decimal_Type32::operator+=(Decimal64 rhs)
{
    return *this = Decimal32(Decimal64(*this) + rhs);
}

inline Decimal_Type32& Decimal_Type32::operator+=(Decimal128 rhs)
{
    return *this = Decimal32(Decimal128(*this) + rhs);
}

inline Decimal_Type32& Decimal_Type32::operator+=(int rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator+=(unsigned int rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator+=(long rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator+=(unsigned long rhs)
{
    return *this += Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator+=(long long rhs)
{
    return *this += Decimal128(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator+=(unsigned long long rhs)
{
    return *this += Decimal128(rhs);
}

                                // Subtraction

inline Decimal_Type32& Decimal_Type32::operator-=(Decimal32 rhs)
{
    this->d_value = DecimalImpUtil::subtract(this->d_value, rhs.d_value);
    return *this;
}

inline Decimal_Type32& Decimal_Type32::operator-=(Decimal64 rhs)
{
    return *this = Decimal32(Decimal64(*this) - rhs);
}

inline Decimal_Type32& Decimal_Type32::operator-=(Decimal128 rhs)
{
    return *this = Decimal32(Decimal128(*this) - rhs);
}

inline Decimal_Type32& Decimal_Type32::operator-=(int rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator-=(unsigned int rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator-=(long rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator-=(unsigned long rhs)
{
    return *this -= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator-=(long long rhs)
{
    return *this -= Decimal128(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator-=(unsigned long long rhs)
{
    return *this -= Decimal128(rhs);
}

                               // Multiplication

inline Decimal_Type32& Decimal_Type32::operator*=(Decimal32 rhs)
{
    this->d_value = DecimalImpUtil::multiply(this->d_value, rhs.d_value);
    return *this;
}

inline Decimal_Type32& Decimal_Type32::operator*=(Decimal64 rhs)
{
    return *this = Decimal32(Decimal64(*this) * rhs);
}

inline Decimal_Type32& Decimal_Type32::operator*=(Decimal128 rhs)
{
    return *this = Decimal32(Decimal128(*this) * rhs);
}

inline Decimal_Type32& Decimal_Type32::operator*=(int rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator*=(unsigned int rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator*=(long rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator*=(unsigned long rhs)
{
    return *this *= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator*=(long long rhs)
{
    return *this *= Decimal128(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator*=(unsigned long long rhs)
{
    return *this *= Decimal128(rhs);
}

                                  // Division

inline Decimal_Type32& Decimal_Type32::operator/=(Decimal32 rhs)
{
    this->d_value = DecimalImpUtil::divide(this->d_value, rhs.d_value);
    return *this;
}

inline Decimal_Type32& Decimal_Type32::operator/=(Decimal64 rhs)
{
    return *this = Decimal32(Decimal64(*this) / rhs);
}

inline Decimal_Type32& Decimal_Type32::operator/=(Decimal128 rhs)
{
    return *this = Decimal32(Decimal128(*this) / rhs);
}

inline Decimal_Type32& Decimal_Type32::operator/=(int rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator/=(unsigned int rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator/=(long rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator/=(unsigned long rhs)
{
    return *this /= Decimal64(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator/=(long long rhs)
{
    return *this /= Decimal128(rhs);
}

inline Decimal_Type32& Decimal_Type32::operator/=(unsigned long long rhs)
{
    return *this /= Decimal128(rhs);
}


inline
DecimalImpUtil::ValueType32 *Decimal_Type32::data()
{
    return &d_value;
}

                                  // Aspects

template <class STREAM>
STREAM& Decimal_Type32::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            DecimalStorage::Type32 bidVal;
            stream.getUint32(bidVal);

            if (stream) {
                d_value.d_raw = bidVal;
            }
            else {
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
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

                                  // Aspects

template <class STREAM>
STREAM& Decimal_Type32::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            stream.putUint32(d_value.d_raw);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

                            // --------------------
                            // class Decimal_Type64
                            // --------------------

// CLASS METHODS

                                  // Aspects
inline
int Decimal_Type64::maxSupportedBdexVersion()
{
    return 1;
}

inline
int Decimal_Type64::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

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


// MANIPULATORS

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
    return *this = Decimal64(Decimal128(*this) + rhs);
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
    return *this += Decimal128(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator+=(unsigned long long rhs)
{
    return *this += Decimal128(rhs);
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
    return *this = Decimal64(Decimal128(*this) - rhs);
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
    return *this -= Decimal128(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator-=(unsigned long long rhs)
{
    return *this -= Decimal128(rhs);
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
    return *this = Decimal64(Decimal128(*this) * rhs);
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
    return *this *= Decimal128(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator*=(unsigned long long rhs)
{
    return *this *= Decimal128(rhs);
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
    return *this = Decimal64(Decimal128(*this) / rhs);
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
    return *this /= Decimal128(rhs);
}

inline Decimal_Type64& Decimal_Type64::operator/=(unsigned long long rhs)
{
    return *this /= Decimal128(rhs);
}

                                  // Aspects

template <class STREAM>
STREAM& Decimal_Type64::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            DecimalStorage::Type64 bidVal;
            stream.getUint64(bidVal);

            if (stream) {
                d_value.d_raw = bidVal;
            }
            else {
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }

    return stream;
}

//ACCESSORS

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

                                  // Aspects

template <class STREAM>
STREAM& Decimal_Type64::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            stream.putUint64(d_value.d_raw);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

                           // ---------------------
                           // class Decimal_Type128
                           // ---------------------

// CLASS METHODS

                                  // Aspects
inline
int Decimal_Type128::maxSupportedBdexVersion()
{
    return 1;
}

inline
int Decimal_Type128::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// CREATORS
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

                                  // Aspects

template <class STREAM>
STREAM& Decimal_Type128::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            DecimalStorage::Type128 bidVal;
            const int len = sizeof(DecimalStorage::Type128)
                            / sizeof(unsigned char);

            unsigned char *value_p =
                                    reinterpret_cast<unsigned char *>(&bidVal);

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
            for (int i(0); i < len; ++i) {
                stream.getUint8(*(value_p + i));
            }
#elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
            for (int i(len - 1); i >= 0; --i) {
                stream.getUint8(*(value_p + i));
            }
#endif
            if (stream) {
                d_value.d_raw = bidVal;
            }
            else {
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
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

                                  // Aspects

template <class STREAM>
STREAM& Decimal_Type128::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            const int len = sizeof(DecimalStorage::Type128)
                            / sizeof(unsigned char);

            const unsigned char *value_p =
                       reinterpret_cast<const unsigned char *>(&d_value.d_raw);

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
            for (int i(0); i < len; ++i) {
                stream.putUint8(*(value_p + i));
            }
#elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
            for (int i(len - 1); i >= 0; --i) {
                stream.putUint8(*(value_p + i));
            }
#endif
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

                  // ------------------------------------
                  // class Decimal_FormatterSpecification
                  // ------------------------------------

// PRIVATE MANIPULATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void Decimal_FormatterSpecification<t_CHAR>::parseType(
                             const bsl::basic_string_view<t_CHAR>&  typeString)
{
    // Handle empty string or empty specification.
    if (typeString.empty()) {
        d_formatType = e_FORMAT_DEFAULT;
        return;                                                       // RETURN
    }

    // Standard format strings only allow a single type character.
    if (typeString.size() > 1) {
        d_formatType = e_TYPE_UNASSIGNED;
        BSLS_THROW(bsl::format_error(                                  // THROW
                  "Decimal floating point format types are single-character"));
    }

    const t_CHAR frontChar = typeString.front();

    // The type character can only be ASCII so we can do a simple cast.
    const char typeChar = (frontChar >= 0 && frontChar <= 0x7f)
                        ? static_cast<char>(frontChar)
                        : static_cast<char>(0);

    switch (typeChar) {
      case 'e': {
        d_formatType = e_FORMAT_SCIENTIFIC;
      } break;
      case 'E': {
        d_formatType = e_FORMAT_SCIENTIFIC_UC;
      } break;
      case 'f': {
        d_formatType = e_FORMAT_FIXED;
      } break;
      case 'F': {
        d_formatType = e_FORMAT_FIXED_UC;
      } break;
      case 'g': {
        d_formatType = e_FORMAT_GENERAL;
      } break;
      case 'G': {
        d_formatType = e_FORMAT_GENERAL_UC;
      } break;
      default: {
        d_formatType = e_TYPE_UNASSIGNED;
        BSLS_THROW(bsl::format_error(
                  "Invalid format type for decimal floating points")); // THROW
      }
    }

    if (e_TYPE_UNASSIGNED == d_formatType)
        BSLS_THROW(bsl::format_error(                                  // THROW
                          "Failed to parse decimal floating point format type "
                                                          "(reason unknown)"));

    return;
}

// PRIVATE ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void Decimal_FormatterSpecification<t_CHAR>::ensureParsingComplete() const
{
    if (d_parsingStatus == Parser::e_STATE_UNPARSED) {
        BSLS_THROW(bsl::format_error(                                  // THROW
                    "Decimal format specification '.parse()' was not called"));
    }
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
Decimal_FormatterSpecification<t_CHAR>::ensurePostprocessingComplete() const
{
    if (d_parsingStatus != Parser::e_STATE_POSTPROCESSED) {
        BSLS_THROW(bsl::format_error(                                  // THROW
              "Decimal format specification '.postprocess()' was not called"));
    }
}

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
Decimal_FormatterSpecification<t_CHAR>::Decimal_FormatterSpecification()
: d_parsingStatus(Parser::e_STATE_UNPARSED)
, d_parser()
, d_formatType(e_TYPE_UNASSIGNED)
{
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const t_CHAR * Decimal_FormatterSpecification<t_CHAR>::filler() const
{
    ensurePostprocessingComplete();
    return d_parser.filler();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int Decimal_FormatterSpecification<t_CHAR>::fillerCharacters() const
{
    ensurePostprocessingComplete();
    return d_parser.numFillerCharacters();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int Decimal_FormatterSpecification<t_CHAR>::fillerCodePointDisplayWidth() const
{
    ensurePostprocessingComplete();
    return d_parser.fillerCodePointDisplayWidth();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename Decimal_FormatterSpecification<t_CHAR>::Parser::Alignment
Decimal_FormatterSpecification<t_CHAR>::alignment() const
{
    ensureParsingComplete();
    return d_parser.alignment();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename Decimal_FormatterSpecification<t_CHAR>::Parser::Sign
Decimal_FormatterSpecification<t_CHAR>::sign() const
{
    ensureParsingComplete();
    return d_parser.sign();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Decimal_FormatterSpecification<t_CHAR>::alternativeFlag() const
{
    ensureParsingComplete();
    return d_parser.alternativeFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Decimal_FormatterSpecification<t_CHAR>::zeroPaddingFlag() const
{
    ensureParsingComplete();
    return d_parser.zeroPaddingFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const bslfmt::FormatterSpecificationNumericValue
Decimal_FormatterSpecification<t_CHAR>::postprocessedWidth() const
{
    ensurePostprocessingComplete();
    return d_parser.postprocessedWidth();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const bslfmt::FormatterSpecificationNumericValue
Decimal_FormatterSpecification<t_CHAR>::postprocessedPrecision() const
{
    ensurePostprocessingComplete();
    return d_parser.postprocessedPrecision();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool Decimal_FormatterSpecification<t_CHAR>::localeSpecificFlag() const
{
    ensureParsingComplete();
    return d_parser.localeSpecificFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename Decimal_FormatterSpecification<t_CHAR>::FormatType
Decimal_FormatterSpecification<t_CHAR>::formatType() const
{
    ensureParsingComplete();
    return d_formatType;
}

// CLASS METHODS
template <class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
Decimal_FormatterSpecification<t_CHAR>::parse(t_PARSE_CONTEXT *context)
{
    BSLMF_ASSERT((
        bsl::is_same<typename bsl::iterator_traits<
                         typename t_PARSE_CONTEXT::const_iterator>::value_type,
                     t_CHAR>::value));

    d_parsingStatus = Parser::e_STATE_PARSED;

    const typename Parser::Sections sect =
            static_cast<typename Parser::Sections>(
                Parser::e_SECTIONS_FILL_ALIGN |
                Parser::e_SECTIONS_SIGN_FLAG |
                Parser::e_SECTIONS_ALTERNATE_FLAG |
                Parser::e_SECTIONS_ZERO_PAD_FLAG |
                Parser::e_SECTIONS_WIDTH |
                Parser::e_SECTIONS_PRECISION |
                Parser::e_SECTIONS_LOCALE_FLAG |
                Parser::e_SECTIONS_REMAINING_SPEC);

    d_parser.parse(context, sect);

    parseType(d_parser.remainingSpec());

    if (context->begin() == context->end() || *context->begin() == '}') {
        return;                                                       // RETURN
    }

    BSLS_THROW(bsl::format_error(                                      // THROW
                          "Decimal floating point specification parse failure "
                          "(invalid character)"));
}

template <class t_CHAR>
template <typename t_FORMAT_CONTEXT>
void Decimal_FormatterSpecification<t_CHAR>::postprocess(
                                               const t_FORMAT_CONTEXT& context)
{
    ensureParsingComplete();

    d_parser.postprocess(context);

    switch (d_parser.postprocessedWidth().category()) {
      case bslfmt::FormatterSpecificationNumericValue::e_DEFAULT: break;

      case bslfmt::FormatterSpecificationNumericValue::e_VALUE: {
        if (d_parser.postprocessedWidth().value() <= 0)
            BSLS_THROW(bsl::format_error("Zero or negative width value"));
                                                                       // THROW
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Failed to find valid width value"));
                                                                       // THROW
      }
    }

    switch (d_parser.postprocessedPrecision().category()) {
      case bslfmt::FormatterSpecificationNumericValue::e_DEFAULT: break;

      case bslfmt::FormatterSpecificationNumericValue::e_VALUE: {
        if (d_parser.postprocessedPrecision().value() < 0)
            BSLS_THROW(bsl::format_error("Negative precision value")); // THROW
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Failed to find valid precision value"));
                                                                       // THROW
      }
    }

    d_parsingStatus = Parser::e_STATE_POSTPROCESSED;
}

              // -------------------------------------------
              // template struct Decimal_BslFmtFormatterImpl
              // -------------------------------------------

// PRIVATE MANIPULATORS
template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
Decimal_BslFmtFormatterImpl<t_VALUE, t_CHAR>::alignAndCopy(
                                         const char           *numberBuffer,
                                         size_t                numberLength,
                                         t_FORMAT_CONTEXT&     formatContext,
                                         const Specification&  finalSpec) const
{
    typedef bslfmt::FormatterSpecificationNumericValue NumericValue;
    typedef bslfmt::FormatSpecificationParser<t_CHAR>  Parser;

    NumericValue finalWidth(finalSpec.postprocessedWidth());

    ptrdiff_t leftPadFillerCopiesNum  = 0;
    ptrdiff_t rightPadFillerCopiesNum = 0;
    ptrdiff_t zeroPadFillerCopiesNum  = 0;

    const char signChar = *numberBuffer == '-'
                           ? *numberBuffer
                           : *numberBuffer == '+'
                           ? *numberBuffer
                           : *numberBuffer == ' '
                           ? *numberBuffer
                           : '\0';
    const bool hasSignChar = (signChar != 0);

    if (hasSignChar) {
        // We are going to add the sign back "by hand"
        ++numberBuffer;
        --numberLength;
    }

    // Check if we have a non-numerical value of "inf" or "nan", in which case
    // we must no use zero padding.
    const char lastChar = numberBuffer[numberLength - 1];
    const bool specialValue = 'f' == lastChar || 'n' == lastChar
                           || 'F' == lastChar || 'N' == lastChar;

    if ((NumericValue::e_DEFAULT != finalWidth.category()) &&
        (numberLength + hasSignChar <
         static_cast<size_t>(finalWidth.value()))) {
        // We need to fill the remaining space.

        const ptrdiff_t totalPadDisplayWidth =
                             finalWidth.value() - (numberLength + hasSignChar);

        if (!specialValue &&
            Parser::e_ALIGN_DEFAULT == finalSpec.alignment() &&
            finalSpec.zeroPaddingFlag()) {
            // Space will be filled with zeros.

            zeroPadFillerCopiesNum = totalPadDisplayWidth;
        }
        else {
            // Alignment with appropriate symbol is required.

            switch (d_spec.alignment()) {
              case Parser::e_ALIGN_LEFT: {
                leftPadFillerCopiesNum  = 0;
                rightPadFillerCopiesNum = totalPadDisplayWidth;
              } break;
              case Parser::e_ALIGN_MIDDLE: {
                leftPadFillerCopiesNum  = (totalPadDisplayWidth / 2);
                rightPadFillerCopiesNum = ((totalPadDisplayWidth + 1) / 2);
              } break;
              case Parser::e_ALIGN_DEFAULT:
              case Parser::e_ALIGN_RIGHT: {
                leftPadFillerCopiesNum  = totalPadDisplayWidth;
                rightPadFillerCopiesNum = 0;
              } break;
              default: {
                BSLS_THROW(bsl::format_error("Invalid alignment"));
              } break;
            }
        }
    }

    // Assembling the final string.

    typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

    for (ptrdiff_t i = 0; i < leftPadFillerCopiesNum; ++i) {
        outIterator = bsl::copy(
                            finalSpec.filler(),
                            finalSpec.filler() + finalSpec.fillerCharacters(),
                            outIterator);
    }

    if (hasSignChar) {
        outIterator = BloombergLP::bslfmt::FormatterCharUtil<
                             t_CHAR>::outputFromChar(signChar, outIterator);
    }

    for (ptrdiff_t i = 0; i < zeroPadFillerCopiesNum; ++i) {
        outIterator = BloombergLP::bslfmt::FormatterCharUtil<
                                    t_CHAR>::outputFromChar('0', outIterator);
    }

    outIterator =
            BloombergLP::bslfmt::FormatterCharUtil<t_CHAR>::outputFromChar(
                                                   numberBuffer,
                                                   numberBuffer + numberLength,
                                                   outIterator);

    for (ptrdiff_t i = 0; i < rightPadFillerCopiesNum; ++i) {
        outIterator = bsl::copy(
                             finalSpec.filler(),
                             finalSpec.filler() + finalSpec.fillerCharacters(),
                             outIterator);
    }

    return outIterator;
}

template <class t_VALUE, class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
Decimal_BslFmtFormatterImpl<t_VALUE, t_CHAR>::parse(
                                                 t_PARSE_CONTEXT& parseContext)
{
    d_spec.parse(&parseContext);

    if (d_spec.localeSpecificFlag()) {
        BSLS_THROW(bsl::format_error(                                  // THROW
                          "Formatting with the L specifier is not supported"));
    }

    return parseContext.begin();
}

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
Decimal_BslFmtFormatterImpl<t_VALUE, t_CHAR>::format(
                                         const t_VALUE&    value,
                                         t_FORMAT_CONTEXT& formatContext) const
{

    typedef bslfmt::FormatterSpecificationNumericValue NumericValue;
    typedef bslfmt::FormatSpecificationParser<t_CHAR>  Parser;

    Specification finalSpec(d_spec);
    finalSpec.postprocess(formatContext);

    const bool isDefaultPrecision =
                                 NumericValue::e_DEFAULT ==
                                 finalSpec.postprocessedPrecision().category();
    const int precision = isDefaultPrecision
                        ? -1
                        : finalSpec.postprocessedPrecision().value();

    DecimalFormatConfig cfg(precision);

    bool upperCase = false;

    switch (finalSpec.formatType()) {
      case Specification::e_FORMAT_SCIENTIFIC_UC:
        upperCase = true;
        BSLA_FALLTHROUGH;
      case Specification::e_FORMAT_SCIENTIFIC: {
        cfg.setStyle(DecimalFormatConfig::e_SCIENTIFIC);
      } break;

      case Specification::e_FORMAT_FIXED_UC:
        upperCase = true;
        BSLA_FALLTHROUGH;
      case Specification::e_FORMAT_FIXED: {
          cfg.setStyle(DecimalFormatConfig::e_FIXED);
      } break;

      // These are all the default "natural" format
      case Specification::e_FORMAT_GENERAL_UC:
        upperCase = true;
        BSLA_FALLTHROUGH;
      case Specification::e_FORMAT_DEFAULT: BSLA_FALLTHROUGH;
      case Specification::e_FORMAT_GENERAL: break;

      default: {
        BSLS_THROW(bsl::format_error("Unknown decimal floating point format"));
      }
    }

    switch (finalSpec.sign()) {
      case Parser::e_SIGN_DEFAULT:  BSLA_FALLTHROUGH;
      case Parser::e_SIGN_NEGATIVE: {
        cfg.setSign(DecimalFormatConfig::e_NEGATIVE_ONLY);
      } break;
      case Parser::e_SIGN_POSITIVE: {
        cfg.setSign(DecimalFormatConfig::e_ALWAYS);
      } break;
      case Parser::e_SIGN_SPACE: {
        cfg.setSign(DecimalFormatConfig::e_POSITIVE_AS_SPACE);
      } break;
    }

    if (finalSpec.alternativeFlag()) {
        cfg.setShowpoint(DecimalFormatConfig::e_ALWAYS);
    }
    if (upperCase) {
        cfg.setInfinity("INF");
        cfg.setNan("NAN");
        cfg.setSNan("NAN");
        cfg.setExponent('E');
    }
    else {
        cfg.setInfinity("inf");
        cfg.setNan("nan");
        cfg.setSNan("nan");
        cfg.setExponent('e');
    }

    typedef bsl::numeric_limits<t_VALUE> Limits;
    const int effectivePrecision = precision > Limits::max_precision
                                 ? precision
                                 : Limits::max_precision;

    const int k_STACK_BUF_LEN = 1                          // sign
                              + 1 + Limits::max_exponent10 // integer part
                              + 1                          // decimal point
                              + Limits::max_precision;     // partial part

    const int reqdBufferSize = 1                          // sign
                             + 1 + Limits::max_exponent10 // integer part
                             + 1                          // decimal point
                             + effectivePrecision;        // partial part
        // The size of the buffer sufficient to store max 'DECIMAL' value in
        // fixed notation with the max precision supported by 'DECIMAL' type,
        // or the requested precision if it is larger than the maximum.

    char  sbuf[k_STACK_BUF_LEN];
    char *dbuf = 0;
    char *buffer = sbuf;

    bsl::polymorphic_allocator<char> allocator;
    if (reqdBufferSize > k_STACK_BUF_LEN) {
        dbuf = allocator.allocate(reqdBufferSize);
        buffer = dbuf;
    }
    bslma::DeallocateBytesProctor<bsl::polymorphic_allocator<char> > proctor(
                                                    allocator,
                                                    dbuf,
                                                    dbuf ? reqdBufferSize : 0);

    const int len = DecimalImpUtil::format(buffer,
                                           reqdBufferSize,
                                           *value.data(),
                                           cfg);
    BSLS_ASSERT(len <= reqdBufferSize);

    return alignAndCopy(buffer, len, formatContext, finalSpec);
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
bdldfp::Decimal32 bdldfp::operator++(bdldfp::Decimal32& value, int)
{
    bdldfp::Decimal32 result(value);
    ++value;
    return result;
}

inline
bdldfp::Decimal32 bdldfp::operator--(bdldfp::Decimal32& value, int)
{
    bdldfp::Decimal32 result(value);
    --value;
    return result;
}

                                  // Addition

inline
bdldfp::Decimal32 bdldfp::operator+(bdldfp::Decimal32 lhs,
                                    bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::add(*lhs.data(), *rhs.data());
}

inline
bdldfp::Decimal32 bdldfp::operator+(bdldfp::Decimal32 lhs,
                                    int               rhs)
{
    return Decimal32(lhs + Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator+(bdldfp::Decimal32 lhs,
                                    unsigned int      rhs)
{
    return Decimal32(lhs + Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator+(bdldfp::Decimal32 lhs,
                                    long              rhs)
{
    return Decimal32(lhs + Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator+(bdldfp::Decimal32 lhs,
                                    unsigned long     rhs)
{
    return Decimal32(lhs + Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator+(bdldfp::Decimal32 lhs,
                                    long long         rhs)
{
    return Decimal32(lhs + Decimal128(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator+(bdldfp::Decimal32  lhs,
                                    unsigned long long rhs)
{
    return Decimal32(lhs + Decimal128(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator+(int               lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) + rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator+(unsigned int      lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) + rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator+(long              lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) + rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator+(unsigned long     lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) + rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator+(long long         lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal128(lhs) + rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator+(unsigned long long lhs,
                                    bdldfp::Decimal32  rhs)
{
    return Decimal32(Decimal128(lhs) + rhs);
}


                                // Subtraction

inline
bdldfp::Decimal32 bdldfp::operator-(bdldfp::Decimal32 lhs,
                                    bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::subtract(*lhs.data(), *rhs.data());
}

inline
bdldfp::Decimal32 bdldfp::operator-(bdldfp::Decimal32 lhs,
                                    int               rhs)
{
    return Decimal32(lhs - Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator-(bdldfp::Decimal32 lhs,
                                    unsigned int      rhs)
{
    return Decimal32(lhs - Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator-(bdldfp::Decimal32 lhs,
                                    long              rhs)
{
    return Decimal32(lhs - Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator-(bdldfp::Decimal32 lhs,
                                    unsigned long     rhs)
{
    return Decimal32(lhs - Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator-(bdldfp::Decimal32 lhs,
                                    long long         rhs)
{
    return Decimal32(lhs - Decimal128(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator-(bdldfp::Decimal32  lhs,
                                    unsigned long long rhs)
{
    return Decimal32(lhs - Decimal128(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator-(int               lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) - rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator-(unsigned int      lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) - rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator-(long              lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) - rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator-(unsigned long     lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) - rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator-(long long         lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal128(lhs) - Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator-(unsigned long long lhs,
                                    bdldfp::Decimal32  rhs)
{
    return Decimal32(Decimal128(lhs) - Decimal64(rhs));
}

                               // Multiplication

inline bdldfp::Decimal32 bdldfp::operator*(bdldfp::Decimal32 lhs,
                                           bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::multiply(*lhs.data(), *rhs.data());
}

inline
bdldfp::Decimal32 bdldfp::operator*(bdldfp::Decimal32 lhs,
                                    int               rhs)
{
    return Decimal32(lhs * Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator*(bdldfp::Decimal32 lhs,
                                    unsigned int      rhs)
{
    return Decimal32(lhs * Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator*(bdldfp::Decimal32 lhs,
                                    long              rhs)
{
    return Decimal32(lhs * Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator*(bdldfp::Decimal32 lhs,
                                    unsigned long     rhs)
{
    return Decimal32(lhs * Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator*(bdldfp::Decimal32 lhs,
                                    long long         rhs)
{
    return Decimal32(lhs * Decimal128(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator*(bdldfp::Decimal32  lhs,
                                    unsigned long long rhs)
{
    return Decimal32(lhs * Decimal128(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator*(int               lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) * rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator*(unsigned int      lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) * rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator*(long              lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) * rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator*(unsigned long     lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) * rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator*(long long         lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal128(lhs) * rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator*(unsigned long long lhs,
                                    bdldfp::Decimal32  rhs)
{
    return Decimal32(Decimal128(lhs) * rhs);
}

                               // Division

inline
bdldfp::Decimal32 bdldfp::operator/(bdldfp::Decimal32 lhs,
                                    bdldfp::Decimal32 rhs)
{
    return DecimalImpUtil::divide(*lhs.data(), *rhs.data());
}

inline
bdldfp::Decimal32 bdldfp::operator/(bdldfp::Decimal32 lhs,
                                    int               rhs)
{
    return Decimal32(lhs / Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator/(bdldfp::Decimal32 lhs,
                                    unsigned int      rhs)
{
    return Decimal32(lhs / Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator/(bdldfp::Decimal32 lhs,
                                    long              rhs)
{
    return Decimal32(lhs / Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator/(bdldfp::Decimal32 lhs,
                                    unsigned long     rhs)
{
    return Decimal32(lhs / Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator/(bdldfp::Decimal32 lhs,
                                    long long         rhs)
{
    return Decimal32(lhs / Decimal128(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator/(bdldfp::Decimal32  lhs,
                                    unsigned long long rhs)
{
    return Decimal32(lhs / Decimal128(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator/(int               lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) / rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator/(unsigned int      lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) / rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator/(long              lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) / Decimal64(rhs));
}

inline
bdldfp::Decimal32 bdldfp::operator/(unsigned long     lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal64(lhs) / rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator/(long long         lhs,
                                    bdldfp::Decimal32 rhs)
{
    return Decimal32(Decimal128(lhs) / rhs);
}

inline
bdldfp::Decimal32 bdldfp::operator/(unsigned long long lhs,
                                    bdldfp::Decimal32  rhs)
{
    return Decimal32(Decimal128(lhs) / rhs);
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
inline
bdldfp::Decimal32 bdldfp::DecimalLiterals::operator""_d32(const char *str)
{
    return DecimalImpUtil::parse32(str);
}

inline
bdldfp::Decimal32 bdldfp::DecimalLiterals::operator""_d32(
                                                  const char *str, bsl::size_t)
{
    return DecimalImpUtil::parse32(str);
}
#endif

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
    return Decimal64(lhs + Decimal128(rhs));
}

inline
bdldfp::Decimal64 bdldfp::operator+(bdldfp::Decimal64  lhs,
                                    unsigned long long rhs)
{
    return Decimal64(lhs + Decimal128(rhs));
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
    return Decimal64(Decimal128(lhs) + rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator+(unsigned long long lhs,
                                    bdldfp::Decimal64  rhs)
{
    return Decimal64(Decimal128(lhs) + rhs);
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
    return Decimal64(lhs - Decimal128(rhs));
}

inline
bdldfp::Decimal64 bdldfp::operator-(bdldfp::Decimal64  lhs,
                                    unsigned long long rhs)
{
    return Decimal64(lhs - Decimal128(rhs));
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
    return Decimal64(Decimal128(lhs) - rhs);
}

inline
bdldfp::Decimal64 bdldfp::operator-(unsigned long long lhs,
                                    bdldfp::Decimal64  rhs)
{
    return Decimal64(Decimal128(lhs) - rhs);
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
    return Decimal64(lhs * Decimal128(rhs));
}

inline bdldfp::Decimal64 bdldfp::operator*(bdldfp::Decimal64  lhs,
                                           unsigned long long rhs)
{
    return Decimal64(lhs * Decimal128(rhs));
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
    return Decimal64(Decimal128(lhs) * rhs);
}

inline bdldfp::Decimal64 bdldfp::operator*(unsigned long long lhs,
                                           bdldfp::Decimal64  rhs)
{
    return Decimal64(Decimal128(lhs) * rhs);
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
    return Decimal64(lhs / Decimal128(rhs));
}

inline bdldfp::Decimal64 bdldfp::operator/(bdldfp::Decimal64  lhs,
                                           unsigned long long rhs)
{
    return Decimal64(lhs / Decimal128(rhs));
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
    return Decimal64(Decimal128(lhs) / rhs);
}

inline bdldfp::Decimal64 bdldfp::operator/(unsigned long long lhs,
                                           bdldfp::Decimal64  rhs)
{
    return Decimal64(Decimal128(lhs) / rhs);
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
inline
bdldfp::Decimal64 bdldfp::DecimalLiterals::operator""_d64(const char *str)
{
    return DecimalImpUtil::parse64(str);
}

inline
bdldfp::Decimal64 bdldfp::DecimalLiterals::operator""_d64(
                                                  const char *str, bsl::size_t)
{
    return DecimalImpUtil::parse64(str);
}
#endif

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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
inline
bdldfp::Decimal128 bdldfp::DecimalLiterals::operator""_d128(const char *str)
{
    return DecimalImpUtil::parse128(str);
}

inline
bdldfp::Decimal128 bdldfp::DecimalLiterals::operator""_d128(
                                                  const char *str, bsl::size_t)
{
    return DecimalImpUtil::parse128(str);
}
#endif

// FREE FUNCTIONS
template <class HASHALG>
inline
void bdldfp::hashAppend(HASHALG& hashAlg, const bdldfp::Decimal32& object)
{
    using ::BloombergLP::bslh::hashAppend;

    bdldfp::Decimal32 normalizedObject = DecimalImpUtil::normalize(
                                                               object.value());
    hashAlg(&normalizedObject, sizeof(normalizedObject));
}

template <class HASHALG>
inline
void bdldfp::hashAppend(HASHALG& hashAlg, const bdldfp::Decimal64& object)
{
    using ::BloombergLP::bslh::hashAppend;

    bdldfp::Decimal64 normalizedObject = DecimalImpUtil::normalize(
                                                               object.value());

    hashAlg(&normalizedObject, sizeof(normalizedObject));
}

template <class HASHALG>
inline
void bdldfp::hashAppend(HASHALG& hashAlg, const bdldfp::Decimal128& object)
{
    using ::BloombergLP::bslh::hashAppend;

    bdldfp::Decimal128 normalizedObject = DecimalImpUtil::normalize(
                                                               object.value());

    hashAlg(&normalizedObject, sizeof(normalizedObject));
}

}  // close enterprise namespace

// FORMATTER SPECIALIZATIONS
namespace bsl {

/// This template partial specialization defines `bsl::formatter` for
/// `bdldfp::Decimal32` values for both (`char` and `wchar_t`) character types.
template <class t_CHAR>
struct formatter<BloombergLP::bdldfp::Decimal32, t_CHAR>
: BloombergLP::bdldfp::Decimal_BslFmtFormatterImpl<
                                                BloombergLP::bdldfp::Decimal32,
                                                t_CHAR>
{ };

/// This template partial specialization defines `bsl::formatter` for
/// `bdldfp::Decimal64` values for both (`char` and `wchar_t`) character types.
template <class t_CHAR>
struct formatter<BloombergLP::bdldfp::Decimal64, t_CHAR>
: BloombergLP::bdldfp::Decimal_BslFmtFormatterImpl<
                                                BloombergLP::bdldfp::Decimal64,
                                                t_CHAR>
{ };

/// This template partial specialization defines `bsl::formatter` for
/// `bdldfp::Decimal128` values for both (`char` & `wchar_t`) character types.
template <class t_CHAR>
struct formatter<BloombergLP::bdldfp::Decimal128, t_CHAR>
: BloombergLP::bdldfp::Decimal_BslFmtFormatterImpl<
                                               BloombergLP::bdldfp::Decimal128,
                                               t_CHAR>
{ };

}  // close namespace bsl


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
